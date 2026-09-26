# Multiplayer network refactor: status and remaining work

Original plan: 2026-05-18. Last source audit: 2026-09-26.

## Audited baselines and compatibility

| Component | Inspected revision | Handshake protocol |
| --- | --- | --- |
| Current C++ client | Vangers `master`, `71e95b8` | **6** (`src/network.cpp`) |
| Current Rust server | `stalkerg/vangers-srv`, `master`, `29fb0cb` (merged from `integration/open-prs-2026-04-10`) | **6** (`vangers-srv/src/client.rs`) |
| Removed legacy C++ server (historical reference) | [Archived source at `71e95b8`](https://github.com/KranX/Vangers/blob/71e95b86cf61c4bdc3df5b1279cd09122d3a81da/server/server.cpp) | **4** only; not shipped |

The matching implementation is the C++ client plus Rust **master** at the
revision above. The Rust integration branch has been merged into `master`,
including the protocol-5 item/snapshot work (`071f278`) and the later protocol-6
handshake change (`29fb0cb`).

Rust source paths below are relative to that audited revision. To inspect the
exact baseline without switching branches:

```sh
git -C ../vangers-srv show 29fb0cb:vangers-srv/src/server/callback/item_transfer.rs
```

The initial source audit used locally available Git refs. The integration
branch was subsequently fast-forwarded into Rust `master` and pushed, with
`cargo test --workspace --locked` passing 112 tests at `29fb0cb`.
No deployed server was inspected or changed. **Implemented** means present in
source; it is not an end-to-end multiplayer sign-off.

## What changed from the original plan

The original protocol-4 pickup/drop path represented one gameplay action as two
independent requests: `DELETE_OBJECT(STUFF)` followed by `CREATE_OBJECT(DEVICE)`,
or the reverse on drop. Ownership rejection or stale events could leave both
faces of the same item alive.

Protocol **5** introduced explicit item transfers, authoritative item events and
world-entry snapshot boundaries. Those mechanisms are implemented in the client
and Rust `master`.

Protocol **6** subsequently defined analog steering/throttle semantics in
previously unused bits of the existing 16-bit control field for gamepad input.
Its packet size is unchanged, but older clients must not interpret the new bits
as the old controls. See [PR #678](https://github.com/KranX/Vangers/pull/678).

The old instruction to "finish everything inside protocol 5" is historical,
not a reason to revert the current handshake. Keep compatible cleanup on protocol
6; assess any future incompatible wire/semantic change explicitly rather than
mixing old and new peers through packet guessing.

## Implemented item-state refactor

| Feature | Current implementation |
| --- | --- |
| Explicit pickup/drop | `src/units/items.cpp` calls `begin_item_transfer()` in `src/network.cpp`; packet IDs are in `src/multiplayer.h` and Rust `src/protocol.rs`. |
| Atomic server transition | Rust `server/callback/item_transfer.rs` validates packet/type/paired-ID/linked-ID/world constraints, permits same-world pickup from another creator and requires inventory ownership for drop. Validation precedes replacement in `game.vanjects`. |
| Explicit state/removal | Accepted transfers and item create/update/snapshot entries use `ITEM_STATE`; actual item deletion uses `ITEM_REMOVED`. Client dispatch is in `src/network.cpp` and `src/units/hobj.cpp`. |
| Old split-transfer rejection | Rust `delete_object.rs` rejects the old item-transfer delete marker; `create_object.rs` rejects an item create when the paired face already exists. Ordinary non-item lifecycle packets remain supported. |
| Ownership on leave | Rust `leave_world.rs` distinguishes current owner from creator/station, preserving items transferred to someone else and dropped world items. Tests cover these cases. |

Current wire shapes remain:

```text
ITEM_TRANSFER(kind, old_id, old_time, delete_marker, new_item_object)
ITEM_STATE(item_state, previous_id, full_update_object_data)
ITEM_REMOVED(item_id, paired_id, reason)
```

`kind` distinguishes pickup (`STUFF -> DEVICE`) from drop (`DEVICE -> STUFF`).
Server validation is authoritative for stored state; do not restore non-owner
generic deletion as a pickup workaround. The successful transfer is relayed to
other clients in that world. Server-side race tests are not a substitute for
checking the losing client's local recovery in a real two-player session.

## Implemented world-entry snapshot

Rust `server/callback/set_world.rs` constructs the following sequence for the
joining client:

```text
SET_WORLD_RESPONSE
WORLD_SNAPSHOT_BEGIN
TOTAL_LIST_OF_PLAYERS_DATA
VANGER -> SLOT -> DEVICE -> SHELL -> STUFF
WORLD_SNAPSHOT_END
```

Objects are sorted by class and ID. Remote player/equipment/shell objects and
world `STUFF` are included; the joiner's own player objects and objects from other
worlds are excluded. Item entries use `ITEM_STATE`; other entries use
`UPDATE_OBJECT`. Snapshot packets use the ordinary reliable send path, not live
VANGER coalescing.

The client has explicit `wait_begin`, `loading` and idle states in
`src/network.cpp`, and ignores stale relevant packets before snapshot begin.
This replaces the old reliance on incidental later updates to reconstruct the
joining client's world. It does **not** implement a matching ordered spawn bundle
for every client already in the world, and it does not prove queue-saturation
ordering under all conditions.

`RESTORE_CONNECTION` also has a server implementation with timed retention and
client-ID rebinding. It is a separate path: its presence alone does not prove
that every reconnect triggers the complete world-entry reconstruction above.

## Latency/backlog work already implemented

The audited Rust revision also contains
[`NETWORK_LATENCY_PLAN_2026_05_17.md`](https://github.com/stalkerg/vangers-srv/blob/29fb0cb12317c44d820b82ffa0f3188ef8eb3e03/vangers-srv/docs/NETWORK_LATENCY_PLAN_2026_05_17.md).
Some of that document's examples describe the pre-fix state. Current source has:

- **Client:** one outstanding `SERVER_TIME_QUERY`, with timeout recovery;
  VANGER send interval bounded to **50..150 ms**, instead of unbounded
  `average_lag` (`src/units/mechos.cpp`).
- **Rust transport:** a latest-only `SERVER_TIME` slot and a per-recipient,
  per-object latest-update queue for live **VANGER** updates (`src/client.rs`).
- **Reliable default:** ordinary `UPDATE_OBJECT` sends, snapshot entries and
  lifecycle/item/control/chat events are not automatically coalesced. Only the
  explicitly selected live VANGER path uses `send_realtime_update()`.
- **Lifecycle cleanup:** `DELETE_OBJECT` / `HIDE_OBJECT` remove queued realtime
  updates for the same object.
- **Writer/backlog diagnostics:** separate queue statistics and scheduling of
  time replies, reliable packets and realtime updates.
- **World departure:** the old burst of HIDE packets to the leaving client was
  removed; there is a dedicated regression test in `leave_world.rs`.
- **SDL3 client transport:** pending-write backpressure is retained, with a
  separate synchronous-send path (`src/xsocket.cpp`).

Do not extend latest-only behavior to inventory, slots or other object classes
without evidence that their intermediate states are replaceable. No fixed
20/30/60 Hz server flush schedule is established by the current writer.

## Remaining work

### 1. Ordered live join for existing world clients — open

`set_world.rs` sends the full snapshot to the **joining** client. Existing clients
receive world/status notifications and ordinary subsequent object traffic; there
is no equivalent explicit ordered bundle of the entering player's complete
state. The historical symptom is `ignored_missing_vanger` / missing remote
weapons or shots.

Design and test an ordered sequence for existing recipients:

```text
player data -> VANGER -> SLOT -> DEVICE/ITEM_STATE -> SHELL if needed
then replaceable live updates
```

Reuse existing packets where sufficient. Do not patch this with indefinitely
buffered per-object exceptions or assume the joiner's snapshot solves both sides.

### 2. Reliable ordering and load validation — open verification/hardening

Rust `Client::send_reliable()` still spawns an asynchronous fallback send when the
bounded queue is full. A test checks that a full queue does not silently discard
a reliable packet; that is not a proof of ordering across multiple concurrent
fallbacks, snapshots and realtime traffic.

Stress-test world switch/reconnect and slow recipients, verify snapshot
begin/entries/end ordering, and decide whether queue/backpressure handling needs
further hardening. Measure latency before changing writer cadence or expanding
coalescing. Historical solo logs are not sufficient multiplayer evidence.

### 3. Missing static type-14 updates — open investigation

In Rust `update_object.rs`, missing objects still follow the generic
`ignored_missing` / `VanjectNotFound` path. No dedicated resolution of the
historical type-14 cases was found. Decide from gameplay/log evidence whether
these objects need creation/replication or whether the missing updates are
expected and should be downgraded/rate-limited. Do not confuse this with the
already implemented preservation of existing static objects on world departure.

### 4. Per-client logs and diagnostic cleanup — open

The C++ client still opens `network-client.log` with mode `w` in
`src/network.cpp`. Multiple processes in one data directory can overwrite/mix
that file. Add a process/connection identity to the filename, then reduce
high-volume temporary diagnostics once multi-client validation is reliable.
Server visibility and logging tuning remain evidence-driven follow-ups.

### 5. Stable item/owner identity — deferred architecture

Rust still stores the active legacy item face in `HashMap<i32, Vanject>` and
uses `player_bind_id` for owner state. There is no separate logical-item table
or generation-based identity model in the audited Rust revision.

Station-reuse offset calculation **does exist** in `attach_to_game.rs`, scanning
live `game.vanjects` by creator station and type. What remains is targeted
validation of reconnect/reuse, transferred items and counter limits, not writing
that mechanism from scratch. See the
[NetID architecture notes](multiplayer-netid-architecture-notes.md).

A future internal logical-item table may preserve the current packets; changing
client-visible identity/ownership semantics needs separate protocol design.

### 6. Snapshot content and client receive simplification — conditional follow-up

Extend snapshot contents only if tests reveal a missing class. The current
snapshot intentionally covers the five classes listed above, not every possible
world object. Simplify obsolete client receive assumptions after validating the
explicit item and snapshot paths, without reintroducing protocol-4 heuristics.

### 7. Repository/platform compatibility — explicit scope decision

The Rust integration work is now merged into Rust `master` at `29fb0cb`.
Use that matching protocol-6 server for tests and releases. The legacy C++
server, its build target and deployment packaging have been removed from this
repository. Host multiplayer with the separate Rust server; see the
[README](README.md#server) for build/run and deployment configuration.
Removing the bundled server does not change a deployed server automatically.

## Existing tests and remaining acceptance checks

The Rust server source includes tests for:

- current handshake acceptance and prior-version rejection;
- atomic non-owner world pickup, single-winner pickup races, drop ownership,
  wrong-world and malformed pair rejection without state mutation;
- explicit item packet bodies, paired-create rejection and legacy-marker rejection;
- snapshot classes, order and filtering;
- transferred inventory, dropped items and static objects surviving departure;
- restore-connection success/failure;
- reliable/default delivery, latest-only replacement, lifecycle cancellation,
  writer priority and full-queue fallback.

The client has eight CTest targets, including socket, event, settings and analog
control tests. These are implementation evidence, not newly executed results.

Before calling the network work fully validated, record a matching client/server
revision pair and multi-client results for:

- [ ] Pickup/drop races and the losing client's recovery; no duplicate item faces.
- [ ] Transfers followed by owner death, world departure and station reuse.
- [ ] World switching, late joining on both sides, weapons/shots and reconnect.
- [ ] Mixed keyboard/gamepad analog input under protocol 6.
- [ ] Slow-recipient/queue-saturation behavior, bounded update age and reliable order.
- [ ] Snapshot content and diagnostics for missing object classes.

The 2026-05-17 [desync investigation](multiplayer-desync-investigation-2026-05-17.md)
is retained as historical evidence, not as instructions to restore the old
split item-transfer protocol.
