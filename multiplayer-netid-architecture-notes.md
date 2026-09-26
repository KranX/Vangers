# Multiplayer NetID / ownership architecture notes

Original notes: 2026-05-19. Last source audit: 2026-09-26.

These notes distinguish the implemented legacy identity/ownership model from a
future generation-based design. Client baseline: `71e95b8`. Rust baseline:
`stalkerg/vangers-srv`, `master` at `29fb0cb`, after merging
`integration/open-prs-2026-04-10`. Rust paths below refer to that revision.
The current matching pair uses protocol **6**.

The item-transfer/snapshot changes introduced in protocol 5 remain implemented
under protocol 6. The separate logical-item table and generation-based identities
below are still **deferred**, not completed by the protocol bump. See the
[network implementation plan](multiplayer-network-refactor-plan.md) for delivery,
validation and compatibility status.

The integration branch was merged into Rust `master` after the initial audit.
No deployment or end-to-end multiplayer session was performed for this
documentation update. The legacy C++ server examples describe the removed
protocol-4 implementation and remain useful for understanding identity semantics,
not as a compatible server for the current client.

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

The removed C++ server implementation is preserved in
[source history](https://github.com/KranX/Vangers/blob/71e95b86cf61c4bdc3df5b1279cd09122d3a81da/server/server.cpp).
The analysis below describes that legacy implementation, not the maintained
[Rust server](https://github.com/stalkerg/vangers-srv).

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

## Current Rust server implementation

Rust `master` implements the protocol-5 item-state model and now
requires protocol 6. Pickup/drop uses explicit `ITEM_TRANSFER`; the server
validates and replaces the active item face in one mutation path, emitting
`ITEM_STATE`. True deletion uses `ITEM_REMOVED`. Tests cover pickup races, wrong
world/owner/id rejection and preservation of transferred items on departure.

The identity model is still legacy-based:

- `Game::vanjects` is a `HashMap<i32, Vanject>` keyed by legacy object id.
- `Vanject.id` contains creator station/type/counter, not current ownership.
- `player_bind_id` is the server's owner field.
- A transferred item can keep station bits from its creator while its
  `player_bind_id` belongs to another player.
- `server/callback/leave_world.rs` preserves inventory transferred to another
  player, dropped world items and existing static world state; dedicated tests
  exercise those distinctions.

### Station reuse: offset mechanism exists; boundary validation remains

`server/callback/attach_to_game.rs` already scans **all live `game.vanjects`**
for the newly assigned creator station, regardless of their current owner.
It collects per-type counter maxima and sends the 16 offset fields in
`ATTACH_TO_GAME_RESPONSE` (nonzero maxima are advanced by one). Thus transferred
items with the reused station participate in that scan while they remain in
storage. Do not describe collision avoidance as an entirely missing feature.

Still required before treating reuse as fully validated:

- regression scenarios for disconnect/reconnect and slot reuse after a transfer;
- both paired item faces and relevant object types across worlds;
- zero/max counter boundaries, 16-bit exhaustion and indexing assumptions;
- stale owner references and pending events when the same station is assigned
  to another connection.

The offset mechanism is not a generation model and does not on its own establish
that stale packets can never refer to a newly valid object. No separate logical
item database, `server_object_id + generation`, or `owner_id + generation` model
was found in the audited Rust revision.

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

The transfer model introduced in protocol 5 and retained in protocol 6 is a
step in this direction: `ITEM_TRANSFER` / `ITEM_STATE` / `ITEM_REMOVED` make the
operation explicit, but the server still stores active faces as legacy `Vanject`
ids. An internal logical-item table may keep those wire packets. Exposing new
identity/generation semantics to clients would require a separate compatibility
design; it must not be slipped into an unrelated cleanup.

## Practical conclusion for the current code

1. Treat `station` as a creator namespace, not as the current owner.
2. Treat `NetOwner` as a legacy attachment reference, not server-authoritative
   ownership.
3. Preserve the explicit item-state path and the `player_bind_id` distinction.
4. Keep the existing offset scan; add targeted reuse/transfer/exhaustion tests
   rather than assuming either that it is absent or that it proves all cases.
5. Do not forbid station reuse indefinitely or re-key individual item references
   as an isolated workaround.
6. Keep a full logical-item/generation design as explicit future architecture,
   separate from the implemented protocol-6 client/Rust-server baseline.
