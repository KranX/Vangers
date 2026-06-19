# Multiplayer NetID / ownership architecture notes

Date: 2026-05-19

These notes record the investigation around legacy C++ server semantics,
`NetID`, `NetOwner`, station reuse, and item ownership. They are intentionally
separate from the implementation plan so we can continue the architecture
discussion later without losing context.

## Legacy NetID format

The multiplayer object id is a packed legacy id:

```text
[global flag][station:5][world:4][type:6][counter:16]
```

Important consequences:

- `station = 0` is used for objects without a player creator / global context.
- `station = 1..31` are player slots.
- There are 31 player namespaces, not an unbounded namespace per connection.
- `counter` is 16-bit and is per object type namespace inside a station/world
  context.
- This id was designed as a client-side object identity namespace, not as a
  stable lifetime owner id.

## What C++ server does

The C++ server is in `server/server.cpp`.

### Player station / id reuse

`Game::attach_player()` assigns the first free player id:

```cpp
for (int i = 0; i < 31; i++)
    if (!(used_players_IDs & (1 << i))) {
        used_players_IDs |= 1 << i;
        ...
        return player->ID = i + 1;
    }
```

When a player is removed, the bit is freed:

```cpp
used_players_IDs &= ~(1 << (p->ID - 1));
```

So C++ server **does reuse station/player ids** inside the same multiplayer
game.

### Collision avoidance for reused station

C++ server does not allocate a new namespace for every reconnect/new player.
Instead, when a client attaches, server sends `object_ID_offsets[16]` in
`ATTACH_TO_GAME_RESPONSE`.

`Game::get_object_ID_offsets()` scans existing objects in the current `Game`:

- world objects;
- inventory objects of all current players;
- global objects.

For every object where:

```cpp
CLIENT_ID(obj->ID) == client_ID
```

it finds the maximum existing counter for that object type and sends
`max_counter + 1` to the new client.

Client receives this in `src/network.cpp`:

```cpp
for(int i = 0;i < 16;i++)
    object_ID_offsets[i] = events_in.get_word();
```

Then item creation uses the offsets through `CREATE_STUFF_NET_ID(...)`.
For items, client also computes `stuff_ID_offsets` as the maximum of existing
`NID_STUFF` and `NID_DEVICE` offsets so the two paired item faces do not collide
with old objects.

This means:

```text
old player used station 1 and left item counter 100 in the world
new player receives station 1
server sends offset >= 101
new player creates future station-1 objects at higher counters
```

So the legacy C++ answer to station reuse is: reuse the station but continue
the counter range from all still-existing objects in that game.

### Scope of the 16-bit counter

The counter space is scoped to the current multiplayer `Game` state, not the
whole server and not permanent storage.

It survives within a game session because the server scans live game objects on
attach. It does not carry across unrelated games after the old game is gone.

For Vangers item counts this is probably practically enough, but it is still a
legacy workaround, not a strong generation-based identity model.

### C++ server does not treat station as item owner

For player inventory objects, C++ server ownership is represented by list
membership:

```cpp
player->inventory.append(obj);
obj->list = &(player->inventory);
```

That happens in `World::process_create_inventory(Player *player, Object *obj)`.

Therefore an item may have:

```text
CLIENT_ID(obj->ID) == 1
```

but be stored in player 2 inventory. That is valid in the C++ server model.
Station is the id namespace where the object was created, not necessarily the
current owner.

### C++ LEAVE_WORLD behavior

`World::detach_player()` deletes:

1. objects in the leaving player's own `inventory`;
2. private world objects whose `CLIENT_ID(obj->ID)` matches the leaving player.

Relevant logic:

```cpp
Object *obj = player->inventory.first();
while (obj) {
    process_delete(obj);
    obj = obj->next;
}

if (CLIENT_ID(obj->ID) == client_ID && PRIVATE_OBJECT(obj->ID)) {
    process_delete(obj);
}
```

It does **not** delete every `STUFF`/`DEVICE` whose station equals the leaving
player. This is why transferred items with an old station can survive in
another player's inventory.

### C++ DELETE_OBJECT behavior

C++ server does not do an owner check in `DELETE_OBJECT`.

It looks up the object and accepts the delete if the object exists and is not
already being deleted:

```cpp
obj = world->search_object(obj_ID);
...
if (!obj || obj->send_delete) {
    SKIP_DELETE_OBJECT;
    break;
}
...
world->process_delete(obj);
```

This is why the old pickup path worked: client B could delete a world `STUFF`
created by client A, then create the inventory/device face for itself.

The Rust server initially rejected this as a non-owner delete, which exposed
the old split `DELETE_OBJECT + CREATE_OBJECT` transfer bug. Protocol 5 replaced
that split path with explicit `ITEM_TRANSFER`.

## NetOwner is not authoritative ownership

On the client, items carry `NetOwner`, usually the owner Vanger `NetID`.

This is useful as a local reference: the client can attach a device/item to the
currently existing `VangerUnit` when applying network state.

But `NetOwner` is also just a legacy `NetID`:

```text
station + world + type + counter
```

Therefore it is not a stable server-authoritative owner:

- the owning Vanger object can disappear;
- the player can leave;
- the player can respawn and get a new Vanger NetID;
- station can be reused by another connection;
- stale packets can still mention an old `NetOwner`.

So `NetOwner` must be treated as a client-side attachment hint / legacy object
reference, not as the source of truth for server ownership.

Server-side truth should remain a separate concept such as `player_bind_id`.
Long-term it should probably become `server_player_id + generation`.

## Current Rust server implications

The current protocol-5 server already fixed the observed item loss by making
pickup/drop an explicit `ITEM_TRANSFER`.

However, the underlying identity model still has old limitations:

- `Vanject.id` still contains the legacy station/counter namespace.
- `player_bind_id` is the actual server owner.
- A transferred item can still have old station bits in `Vanject.id` while its
  `player_bind_id` points to a different current owner.
- This is compatible with legacy C++ semantics, but can become confusing when
  a station is later reused by another player.

The C++ server handled this by sending object counter offsets to a newly
attached client. We need to verify that the Rust server fully preserves the
same collision-avoidance behavior for all relevant object classes and for
transferred inventory items.

## Why "do not reuse station while old objects exist" is not ideal

One possible workaround would be to avoid reusing a station while any object
with that station still exists.

That is not a good long-term solution:

- there are only 31 player stations;
- long-running games could exhaust stations;
- it turns a legacy identity quirk into a resource leak;
- it does not solve stale owner references or stale packet ABA problems.

The C++ solution reuses station and advances counters. A better modern solution
is to separate identity, owner, and connection lifetime explicitly.

## Why "re-key item into new owner's station" is risky

Another possible fix is to change the item `NetID` whenever ownership changes.

This can be correct only if it is done as a complete atomic state transition,
because item ids are referenced in several places:

- paired `STUFF` and `DEVICE` ids;
- item body fields such as `NetID` / `NetDeviceID`;
- `NetOwner`;
- active slots / weapon data;
- pending transfer acknowledgements;
- local client object lookup tables.

A naive re-key can break gameplay by leaving stale references to the previous
id. If we ever do this, it should be part of a clean authoritative logical-item
model, not a small local patch.

## Preferred future architecture

The clean model is to stop treating legacy `station/counter NetID` as the real
multiplayer identity.

A future architecture should separate:

```text
server_object_id + generation  = stable network identity
owner_player_id + generation   = current owner
legacy NetID                   = compatibility / client object mapping
NetOwner                       = compatibility / client attachment hint
```

For items:

```text
logical_item_id
data_id
state = InWorld | InInventory | Deleted
owner_player_id
owner_vanger_id or attachment target
slot
world
position
legacy_stuff_id
legacy_device_id
generation
```

This would solve:

- station reuse ambiguity;
- stale packets from old owners;
- stale `NetOwner`;
- paired `STUFF`/`DEVICE` identity confusion;
- ABA problems where an old id becomes valid again after reconnect/reuse.

Protocol 5 is a step in this direction because it already moves item transfer
to explicit `ITEM_TRANSFER` / `ITEM_STATE` / `ITEM_REMOVED`, but the server
internals still store active item faces as legacy `Vanject` ids. A later
internal refactor can keep the same protocol-5 packets while introducing a real
logical item table.

## Practical conclusion for now

For the current protocol-5 work:

1. `station` should be treated as a legacy id namespace, not owner.
2. `NetOwner` should not be trusted as authoritative owner.
3. `player_bind_id` is currently the server-side owner truth.
4. Rust server must preserve C++-style station reuse collision avoidance by
   sending correct object id offsets for all live objects in the game.
5. Future architecture should move to server object ids/generations, but this
   should be done coherently, not by patching individual references.
