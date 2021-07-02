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

* SDL2
* SDL2_net
* libvorbis
* clunk (https://github.com/stalkerg/clunk)
* ffmpeg
* zlib

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
* **--network host**: use host networking (reuired to bind on domain)