# Multiplayer network refactor plan

Date: 2026-05-18

This is a living implementation plan for the current protocol-5 multiplayer
refactor. All currently identified wire/protocol-breaking pieces are intended
to be completed inside protocol `5`, so testers do not have to retest several
intermediate incompatible versions.

The goal is to make multiplayer easier to support and debug. We should not
add more compatibility layers, packet guessing, or special cases around the
old `DELETE_OBJECT + CREATE_OBJECT` semantics unless we are doing a temporary
hotfix.

## Main problem

The old network protocol exposes too much of the local client object model.

For example, item pickup/drop is encoded as low-level object destruction and
creation:

- world item: `NID_STUFF`;
- inventory/device item: `NID_DEVICE`;
- pickup/drop is represented as a paired `DELETE_OBJECT` and `CREATE_OBJECT`.

That makes the server guess intent from object lifecycle packets. This is why
we get fragile cases like:

- another player tries to delete a world `STUFF` while picking it up;
- the Rust server rejects it as a non-owner delete;
- then it accepts the new `DEVICE`;
- now the same logical item exists in conflicting states.

The protocol should describe gameplay intent, not local implementation details.

## Refactor principles

1. **Server owns multiplayer truth.**
   Clients may request actions, but the server validates and commits state.

2. **One gameplay action = one network command.**
   Pickup, drop, slot change, world switch, and player equipment change should
   not be reconstructed from several low-level object events.

3. **No hidden semantic meaning in generic packets.**
   `DELETE_OBJECT` should mean object deletion, not “maybe pickup
   confirmation if body byte is 1”.

4. **Prefer explicit snapshots over ordering assumptions.**
   After world switch/reconnect the client should receive a complete state
   snapshot for that world instead of relying on lucky ordering of later
   updates.

5. **Break protocol compatibility cleanly.**
   If we change semantics, bump protocol version and reject old clients with a
   clear error. Do not keep multiple old/new item-transfer paths alive unless
   absolutely necessary for a short transition.

6. **Keep the number of new concepts small.**
   Do not introduce complicated reliability classes, ownership exceptions, or
   per-object hacks if a simpler authoritative state model solves the issue.

## Protocol version strategy

Protocol `5` is the current compatibility break for this refactor.

Do not bump the protocol again while finishing the remaining protocol-5 pieces:

- end-to-end item transfer;
- deterministic world-entry snapshot boundary;
- client/server handling required by those packet ids.

Only bump again if we introduce another incompatible packet format after
protocol `5` has already been handed to testers as a stable baseline.

Reason: testers cannot continuously retest every intermediate bump. We should
finish the coherent protocol-5 change set first, then test that as one
multiplayer compatibility boundary.

## Target model

### Logical network items

Long-term, a networked item should be represented by one logical item identity:

```text
item_id
data_id
state = InWorld | InInventory | Deleted
owner_player_id / owner_vanger_id
slot
world
position
```

`NID_STUFF` and `NID_DEVICE` can remain client-side/rendering/storage forms,
but they should not be the authoritative multiplayer identity.

The server should not need to infer that `0x060B004F` and `0x0601004F` are two
faces of the same item by decoding object id type bits.

### Item commands

Replace low-level item transfer with explicit commands.

For protocol `5` this is implemented as one generic transfer request:

```text
ITEM_TRANSFER(kind, old_id, old_time, delete_marker, new_item_object)
```

where `kind` is `Pickup` or `Drop`.  This covers the currently observed
protocol-breaking item bug: a single gameplay transfer must no longer be split
into independent `DELETE_OBJECT` and `CREATE_OBJECT` lifecycle packets.

If we later need explicit item slot moves or item use requests, they should
reuse this state model or add new requests before protocol `5` is treated as a
stable external baseline. They are not part of the current observed bug because
the current slot/use paths do not require replacing the old split
pickup/drop lifecycle.

The semantics must stay direct:

- client requests an action;
- server validates current state;
- server applies the state transition atomically;
- server broadcasts the resulting item state.

### Item events from server

Server sends state, not guessed lifecycle:

```text
ITEM_STATE(item_id, full state)
ITEM_REMOVED(item_id, reason)
```

For the first implementation we can still translate server state into existing
client object creation/removal internally, but that translation should happen
on the client side from explicit item state, not by receiving arbitrary
`DELETE_OBJECT` from another client.

## World switch / reconnect

World entry should be deterministic:

1. client sends `SET_WORLD`;
2. server updates player world;
3. server replies `SET_WORLD_RESPONSE`;
4. server sends one `WORLD_SNAPSHOT`;
5. only after snapshot completion does the client apply live updates.

`WORLD_SNAPSHOT` should contain:

- players in the world;
- their vangers;
- current equipment/slots;
- world items;
- relevant global objects;
- current object counters/state needed to avoid duplicate ids.

This is simpler than trying to repair missing remote weapons or invisible
players after the fact.

## Remote player equipment and shooting

Remote weapons should not depend on incidental inventory/object ordering.

For protocol `5` this is handled through the deterministic world snapshot
rather than by introducing another packet family:

- `PLAYERS_DATA` describes remote players;
- `VANGER` snapshot entries create the remote mechoses first;
- `SLOT` and inventory `DEVICE` snapshot entries follow after the vanger;
- active `SHELL` entries can be replayed after equipment state exists.

This gives the client a stable construction order without inventing a separate
equipment protocol before we know that the existing object representation is
insufficient. If later testing proves that live weapon/shot state still needs a
dedicated event, that should be designed as an extension of the same snapshot
state model, not as another hidden ordering dependency.

## Separate fix for missing remote weapons/shots

The “remote player has no visible weapons / shooting is not visible” bug should
be handled by the same protocol break, but as a separate piece of work from item
pickup/drop.

The right fix is `WORLD_SNAPSHOT`.

After `SET_WORLD`, the server should send a complete snapshot for the target
world before the client starts applying live updates:

- players data;
- `VANGER` objects;
- `SLOT` objects;
- inventory `DEVICE` objects;
- active `SHELL` objects if they must survive world entry timing;
- world `STUFF`.

Only after the snapshot is complete should normal live `CREATE_OBJECT`,
`UPDATE_OBJECT`, `DELETE_OBJECT`, `HIDE_OBJECT`, shot, and slot updates be
processed.

This removes the fragile window where a client receives slot/device/update
events for a remote player, but does not yet have the local `VangerUnit` needed
to attach those events to something visible.

Important: this should not become another pile of per-packet exceptions. The
snapshot is a boundary:

```text
SET_WORLD
SET_WORLD_RESPONSE
WORLD_SNAPSHOT_BEGIN
WORLD_SNAPSHOT_ENTRY...
WORLD_SNAPSHOT_END
live updates
```

During snapshot loading, live updates can be queued or ignored according to a
simple rule, but they should not be mixed with partially constructed world
state.

## What to remove or stop relying on

These old semantics should be phased out for multiplayer gameplay state:

- non-owner `DELETE_OBJECT` as item pickup;
- paired `STUFF`/`DEVICE` object ids as authoritative item identity;
- object ownership inferred only from `NetOwner` inside client-provided body;
- world entry depending on later `UPDATE_OBJECT` to create missing vangers;
- slot state buffered indefinitely waiting for missing local objects;
- server accepting local client object lifecycle as truth without semantic
  validation.

## Suggested implementation phases

### Current protocol-5 status

All protocol-breaking pieces that were already identified for this refactor are
implemented in the current client/server patch. The remaining work should be
internal cleanup, better tests, or snapshot-content refinements that reuse the
same protocol-5 packet ids.

Implemented:

- protocol was bumped to `5`;
- old clients are rejected at handshake instead of being mixed with protocol-5
  clients;
- client sends explicit `ITEM_TRANSFER` for pickup/drop instead of sending
  `DELETE_OBJECT + CREATE_OBJECT` as two independent requests;
- Rust server validates `ITEM_TRANSFER` and commits server-side `vanjects`
  atomically: either old object is replaced by new object, or nothing changes;
- server validates that old/new ids are paired `STUFF`/`DEVICE` forms of the
  same item identity: same station, same world, same counter, different object
  type;
- accepted transfer is broadcast as explicit `ITEM_STATE`;
- generic item create/update is also broadcast as `ITEM_STATE`, not as
  `UPDATE_OBJECT`;
- generic item delete is broadcast as explicit `ITEM_REMOVED`, not as
  `DELETE_OBJECT`;
- server rejects old protocol-4 style item-transfer `DELETE_OBJECT` marker
  (`body[0] == 1`) instead of treating it as hidden pickup semantics;
- server rejects item `CREATE_OBJECT` if the paired `STUFF`/`DEVICE` form is
  already present, so old split transfer cannot leave both faces alive;
- client applies `ITEM_STATE` through one explicit item-state handler and marks
  the previous paired object as waiting for conversion before applying the new
  state;
- client applies `ITEM_REMOVED` through one explicit item-removal handler;
- Rust server sends a world snapshot after `SET_WORLD`;
- snapshot includes remote `VANGER`, `SLOT`, `DEVICE`, `SHELL`, and world
  `STUFF`, ordered as `VANGER -> SLOT -> DEVICE -> SHELL -> STUFF`;
- snapshot item entries use `ITEM_STATE`, so world-entry item state and live
  item state have the same semantics;
- client treats `WORLD_SNAPSHOT_BEGIN/END` as a real loading boundary and
  ignores stale pre-snapshot object/player/item updates.

This is not a full separate logical-item database yet: internally the server
still stores the currently active item face as a `Vanject`, and the paired id is
decoded from the item body. That is acceptable for protocol `5` because the
wire format no longer exposes the old split lifecycle semantics. A later server
data-structure cleanup can keep the same `ITEM_TRANSFER`, `ITEM_STATE`, and
`ITEM_REMOVED` packets.

### Phase 1: protocol break and explicit item transfer

Status: **implemented**.

- Bump protocol version.
- Reject old clients clearly.
- Add explicit `ITEM_TRANSFER` request packet.
- Client sends pickup/drop as one request.
- Server validates and commits pickup/drop atomically in server state.
- Keep old generic object packets only for objects that are still truly
  generic.

This removes the worst ambiguity on the request path: the server no longer has
to guess whether a non-owner `DELETE_OBJECT` is a real delete or an item pickup.

### Phase 2: authoritative item state packets

Status: **implemented at the protocol level**.

Server-to-client item state is now explicit:

```text
ITEM_STATE {
    item_state,
    previous_id,
    full_update_object_data
}

ITEM_REMOVED {
    item_id,
    paired_id,
    reason
}
```

Why this matters:

- pickup/drop no longer creates a window where clients see only `DELETE_OBJECT`
  or only `CREATE_OBJECT`;
- world item, inventory item, and snapshot item use the same server-authored
  state event;
- a true item removal is distinguishable from a transfer;
- old `DELETE_OBJECT body[0] == 1` semantics are rejected, not emulated.

The remaining “logical item table” idea is now an internal server refactor, not
a necessary protocol break. If we later replace `HashMap<i32, Vanject>` with a
dedicated item table, it can still emit the same `ITEM_STATE` / `ITEM_REMOVED`
events.

### Phase 3: deterministic world snapshot

Status: **implemented**.

- Server sends `WORLD_SNAPSHOT_BEGIN`, ordered snapshot entries, and
  `WORLD_SNAPSHOT_END` after `SET_WORLD`.
- Snapshot includes players data and the object types needed to reconstruct
  remote vangers, their slots/equipment, active shells, and visible items.
- Snapshot item entries use `ITEM_STATE`.
- Client enters `wait_begin` state when requesting `SET_WORLD`.
- Client ignores stale object/player/item packets before
  `WORLD_SNAPSHOT_BEGIN`.
- Client applies snapshot entries while `loading`.
- Client returns to normal live-update processing only after
  `WORLD_SNAPSHOT_END`.

This removes the fragile window where a client receives slot/device/update
events for a remote player before it has the local `VangerUnit` needed to attach
those events.

### Phase 4: obsolete protocol-4 compatibility removal

Status: **implemented for item transfer semantics**.

- Old pickup/drop through split `DELETE_OBJECT + CREATE_OBJECT` is no longer a
  valid protocol-5 item transfer path.
- Server rejects protocol-4 item-transfer delete markers.
- Server rejects paired item creates that would resurrect the old split
  transfer bug.
- Non-item `CREATE_OBJECT`, `UPDATE_OBJECT`, `DELETE_OBJECT`, and `HIDE_OBJECT`
  remain available for generic object lifecycle.

### Remaining non-protocol-breaking work

The following can be done after multiplayer tests without another protocol
bump:

- add a reliable live-join replay for players that are already in the world
  when another player enters that world. The `SET_WORLD` snapshot currently
  protects the joining client, but logs from 2026-05-19 still show a short
  window on existing clients where `UPDATE_OBJECT VANGER` can arrive before the
  local remote `VangerUnit` is fully available (`ignored_missing_vanger`). This
  should be fixed with a small ordered spawn/snapshot bundle to the existing
  world clients:

  ```text
  PLAYERS_DATA for the entering player
  VANGER state
  SLOT state
  DEVICE/ITEM_STATE state
  SHELL state if needed
  then live UPDATE_OBJECT
  ```

  The important point is semantic ordering, not a new packet family. Reuse
  existing protocol-5 packets and make the server send them reliably before the
  entering player's replaceable realtime updates can overtake them. This should
  address the remaining "remote player exists but weapons/shot visuals are
  missing" class of bugs without another protocol bump;
- investigate `UPDATE_OBJECT` for missing static type-14 objects. The
  2026-05-19 server log contains several harmless-but-noisy
  `ignored_missing` updates such as `0x880E02E4`, all decoded as
  `static_object=true`, `type_id=14`. These are not item-transfer objects and
  did not break the session, but the server currently reports them as errors.
  We need to decide whether type-14 objects should be accepted/created as
  replicated static world state, or whether missing updates for that class are
  expected and should be downgraded to an ignored/rate-limited diagnostic;
- make client diagnostic logs unambiguous when several local clients are run
  from the same `data/` directory. The 2026-05-19 local test had `station=1`
  and `station=2` writing into the same `network-client.log`, which is useful
  enough for rough analysis but makes exact ordering harder. A future
  diagnostic-only cleanup can include the process id, connection id, or final
  station id in the file name;
- improve Rust server internals by replacing paired-id decoding with a real
  logical item table while keeping the same wire packets;
- extend snapshot contents if logs show a missing object class, as long as
  existing `ITEM_STATE` / `UPDATE_OBJECT` snapshot entries are enough;
- simplify client receive paths now that item state and snapshot boundaries are
  explicit;
- remove temporary high-volume diagnostics after protocol-5 multiplayer is
  stable;
- tune logging and server visibility rules.

## Success criteria

- Item pickup/drop cannot leave both `STUFF` and `DEVICE` versions alive.
- Two players racing for one item produce one winner and one clear rejection.
- Another player’s trunk cannot lose items because the owner died or changed
  world.
- World switch/reconnect always reconstructs visible players and their weapons.
- Server logs describe gameplay decisions, not only low-level packet effects.
- The amount of special-case networking code decreases after the refactor.
