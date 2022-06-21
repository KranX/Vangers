declare const bridge: VssNative;

class Vss {
    // all scripts that available to use in 'require'
    scripts: string[];

    constructor() {
        this.scripts = bridge.scripts().filter((value) => {
            return value !== "vss.js" && value !== "main.js";
        }).map((value) => value.substring(0, value.length - 3));
    }

    // show message and exit game with error
    fatal(msg: string) {
        bridge.fatal("vss: " + msg);
    }
}

const vss = new Vss();
export default vss;

// == native bridge

interface VssNative {
    fatal(msg: string): void;
    scripts(): string[];
}
