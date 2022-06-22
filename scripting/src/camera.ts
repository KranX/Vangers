// A simple module that pin game camera to fornt of mechos

import vss, { iScreenOptionId } from "./vss";

export function init() {
    let unitAngle = 0;

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
            turnAngle: -unitAngle - vss.math.PI_2,
        };
    });

    vss.addQuantListener("mechos_traction", (payload) => {
        unitAngle = payload.unitAngle;
    });
}
