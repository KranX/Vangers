// A simple module that redirects all events to console (stdout)

import vss from "./vss";

const log = (...args: any[]) => {
    console.log("echo.js (module):", ...args);
};

export function init() {
}
