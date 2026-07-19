# Vangers #

![Vangers](http://cdn.akamai.steamstatic.com/steam/apps/264080/header.jpg?t=1447359431)

![Vangers CI](https://github.com/KranX/Vangers/workflows/Vangers%20Linux%20Build/badge.svg)
![Vangers CI](https://github.com/KranX/Vangers/workflows/Vangers%20Windows%2064bit%20Build/badge.svg)
![Vangers CI](https://github.com/KranX/Vangers/workflows/Vangers%20Windows%2032bit%20Build/badge.svg)
![Vangers CI](https://github.com/KranX/Vangers/workflows/Vangers%20MacOS%20Build/badge.svg)
[![Join the chat at https://t.me/vangers](https://patrolavia.github.io/telegram-badge/chat.svg)](https://t.me/vangers)


Video game that combines elements of the racing and role-playing genres.

All source code is published under the GPLv3 license.

The necessary resources for the game (maps, sounds, textures, etc.) you can take from the games purchased here:

http://store.steampowered.com/app/264080

http://www.gog.com/game/vangers

## Required libraries ##

* SDL 3.2 or newer
* SDL_net 3.2 or newer
* libvorbis
* SDL3-native clunk from the `sdl3` branch (https://github.com/stalkerg/clunk/tree/sdl3)
* ffmpeg 6.0 or newer
* toml11 4.4.x
* zlib

You can see the [wiki pages](https://github.com/KranX/Vangers/wiki) to learn how to build this project.

## User settings

Vangers stores user preferences and input bindings in the UTF-8
`settings.toml` file. On the first launch after updating, existing
`options.dat` and `controls.dat` files are imported automatically. The legacy
files are kept byte-for-byte unchanged so an older game build can still use
them, but subsequent changes made by the new build are saved only to
`settings.toml`. Save-game files are not affected by this migration.

## Gamepad input

SDL3-compatible gamepads are detected and mapped automatically, including
hot-plugging. The left stick controls steering and throttle by default. The
right stick moves the UI cursor and, during gameplay, controls side impulses
and RIG movement. Face buttons operate actions, inventory, the handbrake, and
the jump spring; pressing the left stick activates Vector. The triggers provide
acceleration and fire; the D-pad fires individual weapon slots. In menus, the
D-pad follows the existing arrow-key navigation.
Gamepad buttons can be assigned on the regular controls screen or changed in
`[input.sdl_gamepad.bindings]` in `settings.toml`. Stick axes and trigger
bindings can be changed in
`[input.sdl_gamepad.axes]` and `[input.sdl_gamepad.bindings]`. The game uses one
active gamepad and falls back to the next connected device if it is unplugged.
Strong collisions involving the player's mechos use SDL gamepad rumble when
`input.controller.rumble` is enabled.

The open-source build does not call Steam Input directly. It keeps the active
`SDL_Gamepad`, so a Steam build can associate the same SDL-managed device with
Steam Input without introducing a second device manager.

## Server

To host server you can use Docker image or [build server](https://github.com/KranX/Vangers/wiki/Starting-up-server-compatible-with-web-&-native-versions)
manually.

To use docker image you need to pull `vangers-server` image and run it:

```sh
docker pull caiiiycuk/vangers-server:latest
docker run -v host-dir:container-dir -e SERVER=<server-name> -e CER_FILE=<path-to-cer-file> -e KEY_FILE=<path-to-key-file> caiiiycuk/vangers-server:latest
```

Vangers server requires cer/key files to host wss server.
For example, if you want to host server on `vangers.net` and your cer/key files are in `/root/websockify/` file, then you run command will be:

```
docker run -d -v /root/websockify:/root/websockify -e SERVER=vangers.net -e CER_FILE=/root/websockify/vangers.net.cer -e KEY_FILE=/root/websockify/vangers.net.key --network host caiiiycuk/vangers-server
```

Explanation:
* **-d**: means start in detached mode
* **-v /root/websockify:/root/websockify**: map host directory `/root/websockify` to container directory `/root/websockify`
* **-e SERVER=vangers.net**: should be name of domain you want to host server
* **-e CERT_FILE=/root/websockify/vangers.net.cer**: full path to cer file
* **-e KEY_FILE=/root/websockify/vangers.net.key**: full path to key file
* **--network host**: use host networking (required to bind on domain)
