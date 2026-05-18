# Multiplayer desync investigation, 2026-05-17 logs

Context: testers reported that in multiplayer items can disappear, be thrown to a
wrong place, and other players sometimes have incorrect weapon/slot visuals.

Logs checked:

- `../server.log`
- `../network-client_pak_05-17.log`
- `../network-client_klim_05-17.log`

No code changes were made during this investigation.

## Short conclusion

The strongest confirmed problem is server-side item ownership handling for
`NID_STUFF` pickup/drop transitions.

The Rust server currently rejects `DELETE_OBJECT` for a world `STUFF` if the
packet sender is not the stored owner. That looks safe, but it contradicts the
original C++ server semantics and breaks normal item transfer:

1. Player A drops/owns world `STUFF`.
2. Player B picks it up.
3. Client B sends `DELETE_OBJECT(STUFF)` with a confirmation body, then
   `CREATE_OBJECT(DEVICE)`.
4. Rust server rejects the `DELETE_OBJECT(STUFF)` as `rejected_non_owner`.
5. Rust server still accepts `CREATE_OBJECT(DEVICE)` for B.
6. The same logical item now exists as both old world `STUFF` and new inventory
   `DEVICE`.
7. When B later drops it, `CREATE_OBJECT(STUFF)` collides with the still-existing
   old `STUFF` and the server sends `duplicate CREATE_OBJECT replay_existing`.

This explains both symptoms:

- item visually disappears / reappears inconsistently;
- dropped item appears at an old/wrong position instead of the throw position.

## Evidence: game_id=8

Object pair:

- `0x0601004F` / `100728911` = `DEVICE`
- `0x060B004F` / `101384271` = `STUFF`

Timeline:

```text
17:57:13 zasis45 creates DEVICE 0x0601004F, owner=1
17:57:35 zasis45 deletes DEVICE 0x0601004F
17:57:35 zasis45 creates STUFF  0x060B004F, owner=1
18:04:52 server replays STUFF 0x060B004F to Klinn
18:05:13 Klinn sends DELETE_OBJECT for STUFF 0x060B004F
18:05:13 server rejects it: rejected_non_owner, stored_owner=1, sender=3
18:05:13 server accepts CREATE_OBJECT DEVICE 0x0601004F, owner=3
18:05:21 Klinn drops it: DELETE_OBJECT DEVICE 0x0601004F accepted
18:05:21 Klinn creates STUFF 0x060B004F
18:05:21 server says duplicate CREATE_OBJECT replay_existing
```

The key server lines:

```text
DELETE_OBJECT id=0x060B004F type=STUFF stored_owner=1 packet_sender=Klinn decision="rejected_non_owner"
CREATE_OBJECT id=0x0601004F type=DEVICE stored_owner=3 decision="accepted"
CREATE_OBJECT id=0x060B004F type=STUFF stored_owner=1 decision="duplicate CREATE_OBJECT replay_existing"
```

## Evidence: wider server log

Across `../server.log`:

- `decision="rejected_non_owner"`: 27 total
- all 27 are `type_name="STUFF"`
- by game:
  - `game_id=8`: 1 case
  - `game_id=32`: 26 cases

Duplicate `STUFF` creates:

- `duplicate CREATE_OBJECT replay_existing` for `STUFF`: 17 total
- by game:
  - `game_id=8`: 1 case
  - `game_id=32`: 16 cases

This is not a one-off packet race; it repeats in later multiplayer sessions.

## Why the C++ server behaved differently

Original C++ server `server/server.cpp` does not check owner in `DELETE_OBJECT`.

Relevant behavior:

```cpp
case DELETE_OBJECT:
    obj = world->search_object(obj_ID);
    if (!obj || obj->send_delete) {
        SKIP_DELETE_OBJECT;
        break;
    }
    obj->client_ID = ID;
    obj->time = in_buffer.get_dword();
    obj->death_body_size = in_buffer.event_size() - 9;
    in_buffer.read(obj->body, obj->death_body_size);
    world->process_delete(obj);
```

So the original protocol trusts a client in the same world to delete a world
object. This is required for item pickup because the item network id keeps the
creator/station id even after another player interacts with it.

Important detail: pickup delete is not just "destroy item". The client sends a
delete body byte (`ch = 1`). Client-side `StuffObject::NetEvent(DELETE_OBJECT)`
interprets that as `delete_reason=wait_confirmation`, which is part of the
STUFF -> DEVICE transition.

If the server rejects that delete, other clients never get the transition marker
and the old world `STUFF` remains alive.

## Rust server difference

Current Rust `delete_object.rs` rejects any existing vanject whose
`player_bind_id` differs from the packet sender:

```rust
Some(vanject) if vanject.player_bind_id != player_auth_id => {
    decision = "rejected_non_owner";
    Err(DeleteObjectError::NotOwner(...))?;
}
```

This is stricter than C++ and appears wrong for world `NID_STUFF`.

## Likely direction for the item fix

The safest semantic fix is probably not "allow every non-owner delete for every
object class".

Better candidate:

- allow `DELETE_OBJECT` from a non-owner for non-static world `NID_STUFF` in the
  sender's current world;
- keep stricter checks for inventory/player objects where possible;
- preserve the delete body so `ch = 1` still reaches clients as the
  wait-confirmation transition;
- on accepted delete, remove the old `STUFF` so the following `CREATE_OBJECT`
  for `DEVICE`/later `STUFF` does not collide with stale state.

This follows the original C++ behavior where it matters for item pickup/drop,
without blindly opening unrelated object classes.

Need tests:

1. non-owner can delete world `STUFF` in same world;
2. the deleted `STUFF` is removed from `game.vanjects`;
3. the delete packet is broadcast with the original delete body byte;
4. after that, the same logical item can be recreated as `DEVICE` and later as
   `STUFF` without `duplicate CREATE_OBJECT replay_existing`;
5. non-owner delete of unrelated non-STUFF player/inventory objects remains
   rejected unless proven required by the original protocol.

## Weapon / remote mech visibility finding

There is a second, weaker finding around remote mech creation and weapon slots.

PAK client log has:

- `ignored_missing_vanger`: 17
- `decision=create_missing_from_update`: 709 for items/devices

The Rust server often sends object creation/replay as `UPDATE_OBJECT`.
Client-side items/devices can create a missing object from `UPDATE_OBJECT`, but
`VANGER` updates are ignored if the remote `VangerUnit` is not already created:

```cpp
ActionDispatcher::NetEvent:
    p = ActD.GetNetObject(id);
    if (!p)
        decision="ignored_missing_vanger";
```

Slot updates are partially protected: `NetSlotEvent()` stores slot ids in
`PlayerData` if the remote VANGER object is not yet present. Later
`InitPlayerPoint()` copies those buffered slot ids into `GunSlotData`.

So the weapon issue is less conclusively proven than the item issue, but there is
a real ordering window:

1. server sends remote VANGER/slot/device updates;
2. client has not created the remote VANGER yet;
3. VANGER updates are ignored;
4. slot state may be buffered, but VANGER/shell state and firing state can be
   missed until later updates.

Additional suspicious server behavior:

`set_world.rs` replays only non-player world objects:

```rust
v.get_type() != NID::VANGER && v.get_world() == world_id && !v.is_players()
```

That excludes:

- remote VANGER objects;
- `NID_SLOT`;
- inventory `NID_DEVICE`;
- shell/player objects.

Therefore a joining/switching client depends on later live updates and
`PLAYERS_DATA`/`PLAYERS_WORLD` ordering to reconstruct other players, instead of
getting a complete snapshot of currently relevant player objects.

## Open questions

1. Should Rust `DELETE_OBJECT` emulate C++ for all same-world non-global objects,
   or only for world `NID_STUFF`?
2. Are there legitimate non-owner `UPDATE_OBJECT` cases for `STUFF`/`DEVICE`, or
   is non-owner delete enough for item pickup/drop?
3. Should `SET_WORLD` send a fuller player snapshot to the joining client
   (`PLAYERS_DATA` / `PLAYERS_POSITION` / VANGER/SLOT/DEVICE objects), or should
   the client request `TOTAL_PLAYERS_DATA_QUERY` when it logs
   `ignored_missing_vanger`?

