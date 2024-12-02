import vss from "./vss";
import { ls } from "./ls";

const global = new Function("return this;")();

const log = (...args: any[]) => {
    console.log("== vss:", ...args);
};

export interface Config {
    isActive(id: string): boolean;
    log: typeof log;
}

export type InitFn = (config: Config) => boolean | void;

if (global.config === undefined) {
    global.config = (new class {
        enabledMap: { [id: string]: boolean } = {};
        constructor() {
            log("version", ls().getItem("version"));
            const addons: { id: string, enabled: boolean }[] =
                ls().getItem("addons") as any;
            for (const next of addons) {
                this.enabledMap[next.id] = next.enabled;
            }
            log("loading", vss.getScriptsFolder(), "scripts");
            for (const next of vss.scripts) {
                const initFn: InitFn | undefined = require(next).init;
                if (initFn !== undefined) {
                    if (this.isActive(next)) {
                        const started = initFn(this);
                        log(next, started === false ? "failed" : "started");
                    } else {
                        log(next, "disabled");
                    }
                }
            }
        }

        isActive(id: string) {
            return this.enabledMap[id] === true;
        }

        log = log;
    });
};
