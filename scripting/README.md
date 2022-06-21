## Vangers Scirpting Subsytem (vss)

The vangers binary accepts optional argument for scripting directory. If this arguments present then scripting subsystem
tried to load `main.js` from that folder.

So, to run you custom addon please do:

```bash
./vangres --vss <path-to-directory>
```

This directory contains simple echo addon, you can compile it using following steps:

1. Install node.js + yarn
2. Install dependencies `yarn`
3. Build the code `yarn run tsc`
4. Run the vangers `./vangers --vss scripting/build`

You can copy simple echo addon in separate directory and develop your addon.