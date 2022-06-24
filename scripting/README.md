## Vangers Scirpting Subsytem (vss)

The vangers binary accepts optional argument for scripting directory. If this arguments present then scripting subsystem tried to load `main.js` from that folder.

So, to run your custom addon please do:

```bash
./vangres -vss <path-to-addon-directory>
```

## Example: camera addon

This directory contains simple camera addon. The camera addon will lock the camera
to front of mechos.

You can compile it using following steps:

1. Install node.js + yarn
2. Install dependencies: `yarn`
3. Build the code: `yarn run tsc` (--watch for watch mode)
4. Run the vangers: `./vangers --vss scripting/build`

## How to create new addon

The simplest way is to use [template repository](https://github.com/vangers-app/vss-fullscreen-game).
Make new repository from it, checkout and follow instructions how to develop and publish.

## The `vss` object

Vangers Scripting Subsystem exports `vss` object as entry point to game API.
You should use it to interact with game.

In plain js you can access it with require:

```js
const vss = require("vss");
```

In typescript you can use builting typescript "CommonJS" module resolver:

```ts
import vss from "./vss";
```

There is no API documentation for vss object yet. We recommend to use **vscode + TypeScript** to have autocomplete from vss object. You can generate raw documentation by typing `yarn run typedoc src/vss.ts`.

Basically you should attach one or more listeners to game quant to modify the behaviour of game. Supported
quants are defined in `vss.ts/VssQuantMap`.

## Module definition

Standard loader will load all js files in its own directory. You must proved **init** function to work
with standard loader. Like this:

```ts
import vss from "./vss";

export function init() {
    console.log("echo module started");
    // vss.<...>
}
```

Inside module you can use everything that **ts/js supports**, **vss object** and **console object**.

## Hot-reload

`vss` allows you to reload the scripts without restarting the game. You can load same scrips again (aka *hot-reload*) or
load another folder with scripts. To do this call:

```ts
vss.initScripts(<folder>);
```

You can do hot-reload from scripts by calling: 

```ts
vss.initScripts(vss.getScriptsFolder());
```

## Hot-reload from game

Press **F8** key to do hot-reload from game.