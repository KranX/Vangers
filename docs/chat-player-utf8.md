# Multiplayer chat and player names (stage 6b)

Display names and chat text live in UTF-8 in the client. The existing player
option is already UTF-8; that value goes to the registration boundary, not
through the legacy option getter. Chat accepts all committed SDL text, and
clipboard, selection, cursor movement, deletion and limits use complete Unicode
scalars. This is not grapheme editing or a custom IME composition UI.

Chat input/history, player buttons, the player list/results, compass selection
captions, vehicle labels, and notifications that contain player names use the
UTF-8 model. Remaining static UVS and multiplayer sentences aren't retranslated
by this change. The older Hall of Fame query remains CP866. TTF uses the
existing font roles/fallback; the optional bitmap renderer remains lossy only at
the rasterization boundary. Existing chat geometry, controls and history
scrolling are retained. Wider incoming text can use successive history rows.
Player buttons shorten only the name when necessary, leaving the world/team
suffix visible. If a player leaves or the roster changes the selected slot's
occupant, sending keeps the private draft until another recipient is explicitly
selected. Protocol 6 still routes by slot on the wire; this check uses the last
roster/registration received by the client.

## Protocol 6: explicit opt-in, per connection

Client and [Rust server](https://github.com/stalkerg/vangers-srv) must implement
the same switch:

1. Do the existing protocol-6 identification. The client offers the byte
   string `VANGERS-UTF8/1` as the body of its initial `SERVER_TIME_QUERY`
   before attaching or restoring a game. The server echoes the exact token after
   the four-byte clock only if it enabled the mode. A plain clock means CP866.
2. Registered name and direct-message fields, and the corresponding player
   update/roster fields, are UTF-8 on that connection only. Packet framing,
   NUL termination, masks, player IDs, passwords, room names and game state
   are unchanged. No byte-based encoding detection. Reconnection negotiates again.
   Legacy clients still send a plain time query and get the old response.
3. The new server sends each receiver its encoding. It preserves an exact
   CP866 projection when possible. A legacy peer sees `Vanger-<station>` for
   an unrepresentable name and `[UTF-8 message]` for an unrepresentable
   message. It never sees broken UTF-8 or an ambiguous string of question marks.
   An upgraded peer sees the original name/message, including when a roster
   entry or a finished player is copied into a snapshot.
4. On the old Rust server, a new client retains an unrepresentable draft and
   shows a notice instead of sending it. It announces an explicit temporary
   registration name when necessary; the configured UTF-8 name isn't changed.

Chat retains a 140-byte server field (139 bytes plus NUL), and the upgraded
client splits only at scalar boundaries. An upgraded recipient may receive
more bytes when the server expands a full legacy line into UTF-8. Unicode name
registration keeps at most 16 scalars; legacy limits and old packet layouts
remain in effect for CP866. Server fallback truncation cannot bisect a scalar.

Deployment order: upgrade the Rust server first if convenient; existing clients
still use protocol 6/CP866 without changes. Then deploy the client. The new
client also works with the prior Rust server with the limitations above.
Do not infer this compatibility for arbitrary third-party protocol forks.
