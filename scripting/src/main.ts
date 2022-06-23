import vss from "./vss";

const log = (...args: any[]) => {
    console.log("main.js (loader):", ...args);
};

// load all scripts inside scripts folder
log("== loading", vss.getScriptsFolder(), "scripts");
for (const next of vss.scripts) {
    const initFn = require(next).init;
    if (initFn !== undefined) {
        initFn();
        log(next, " started");
    }
}
log("== loading ednded");
