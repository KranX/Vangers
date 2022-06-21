// A simple module that redirects all events to console (stdout)

import vss, { iScreenOptionId } from "./vss";

export function init() {
    vss.addQuantListener("option", (payload) => {
        if (payload.id === iScreenOptionId.iCAMERA_TURN) {
            return {
                value: 1,
            };
        }

        return undefined;
    });

    vss.addQuantListener("camera", () => {
        return {
            turnAngle: Math.round(Math.random() * 255),
        };
    });
}
