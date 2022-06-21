import vss from "./vss";

const log = (...args: any[]) => {
    console.log("main.js (loader):", ...args);
};

log("== loading", vss.scripts.length, "scripts");
for (const next of vss.scripts) {
    log("starting module", next);
    const initFn = require(next).init;
    if (initFn === undefined) {
        log("unable start module, cause init() is not defined!");
    } else {
        initFn();
        log("... started");
    }
}
log("== loading ednded");
