// A simple module that redirects all events to console (stdout)

import vss, { iScreenOptionId, VssOptionQuant, VssOptionQuantResult } from "./vss";

export function init() {
    vss.addQuantListener("option", (payload: VssOptionQuant): undefined | VssOptionQuantResult => {
        if (payload.id === iScreenOptionId.iCAMERA_TURN) {
            return {
                value: 1,
            };
        }

        return undefined;
    });
}
