## Vangers Scirpting Subsytem (vss)

The vangers binary accepts optional argument for scripting directory. If this arguments present then scripting subsystem tried to load `main.js` from that folder.

So, to run you custom addon please do:

```bash
./vangres -vss <path-to-addon-directory>
```

## Camera addon

This directory contains simple camera addon. The camera addon will lock the camera
to front of mechos.

You can compile it using following steps:

1. Install node.js + yarn
2. Install dependencies: `yarn`
3. Build the code: `yarn run tsc` (--watch for watch mode)
4. Run the vangers: `./vangers --vss scripting/build`

You can use this addon as template for new addons, just copy it to another location.

## TypeScript

You can use typescript "CommonJS" module resolver, the **vss** class is defined in `src/vss.ts`.
Use it as usual:


```ts
import vss from "./vss";

vss.fatal("not implemented");
```

## Module definition

Standard loader will load all js files in it's own directory. You must proved **init** function to work
with standard loader. Like this:

```ts
import vss from "./vss";

export function init() {
    console.log("echo module started");
    // vss.<...>
}
```

Inside module you can use everything that **ts/js supports**, **vss object** and **console object**.
