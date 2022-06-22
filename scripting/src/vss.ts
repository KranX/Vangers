declare const bridge: VssNative;
const global = new Function("return this;")();

export type VssQuantName = keyof VssQuantMap;
export type VssQuantPayload<K extends keyof VssQuantMap> = VssQuantMap[K][0];
export type VssQuantResult<K extends keyof VssQuantMap> = void | undefined | "preventDefault" | VssQuantMap[K][1];

interface VssQuantMap {
    "option": [VssOptionQuant, VssOptionQuantResult],
    "camera": [VssCameraQuant, VssCameraQuantResult],
    "mechos_traction": [VssMechosTractionQuant, VssMechosTractionQuantResult],
}

export interface VssOptionQuant {
    id: iScreenOptionId,
    value: number,
}
export interface VssOptionQuantResult {
    value?: number;
}

export interface VssCameraQuant {
    turnAngle: number,
    slopAngle: number,
}
export interface VssCameraQuantResult {
    turnAngle?: number;
    slopeAngle?: number;
}

export interface VssMechosTractionQuant {
    traction: number,
    rudder: number,
    tractionIncrement: number,
    tractionDecrement: number,
    tractionMax: number,
    rudderStep: number,
    rudderMax: number,
    unitAngle: number,
    helicopterStrife: number,
}
export interface VssMechosTractionQuantResult {
    traction?: number,
    rudder?: number,
    helicopterStrife?: number,
}

export type VssQuantListener<K extends VssQuantName> = (payload: VssQuantPayload<K>,
    stopPropogation: () => void, quant: K) => VssQuantResult<K>;

class VssMath {
    PI = 1 << 11;
    PI_2 = this.PI / 2;

    angleToRadians(angle: number) {
        return angle * Math.PI / this.PI;
    }

    radiansToAngle(radians: number) {
        return Math.round(radians * this.PI / Math.PI);
    }
}

class Vss {
    // all scripts that available to use in 'require'
    scripts: string[];

    // game math
    math = new VssMath();

    private quantListeners: { [quantName: string]: VssQuantListener<any>[] } = {};

    constructor() {
        this.scripts = bridge.scripts().filter((value) => {
            return value !== "vss.js" && value !== "main.js";
        }).map((value) => value.substring(0, value.length - 3));

        global.onVssQuant = this.onVssQuant.bind(this);
    }

    fatal(msg: string) {
        bridge.fatal("vss: " + msg);
    }

    addQuantListener<K extends VssQuantName>(quant: K, listener: VssQuantListener<K>) {
        if (this.quantListeners[quant] === undefined) {
            this.quantListeners[quant] = [];
        }
        this.quantListeners[quant].push(listener);
    }

    removeQuantListener<K extends VssQuantName>(quant: K, listener: VssQuantListener<K>) {
        const index = this.quantListeners[quant]?.indexOf(listener);
        if (index && index !== -1) {
            this.quantListeners[quant].splice(index, 1);
        }
    }

    private onVssQuant<K extends VssQuantName>(quant: K, payload: VssQuantPayload<K>) {
        const listeners = this.quantListeners[quant];
        if (listeners === undefined || listeners.length === 0) {
            return undefined;
        }

        let runNext = true;
        const stopPropogation = () => {
            runNext = false;
        };

        let result = undefined;
        for (const next of listeners) {
            result = next(payload, stopPropogation, quant);
            if (!runNext) {
                return result;
            }
        }

        return result;
    }
}

const vss = new Vss();
export default vss;

// == native bridge
interface VssNative {
    fatal(msg: string): void;
    scripts(): string[];
}

/* eslint-disable no-unused-vars */

// == in game definitions
export enum iScreenOptionId {
    iSOUND_ON = 0, // 0
    iSOUND_VOLUME_CUR, // 1
    iSOUND_VOLUME_MAX, // 2

    iMUSIC_ON, // 3
    iMUSIC_VOLUME_CUR, // 4
    iMUSIC_VOLUME_MAX, // 5

    iTUTORIAL_ON, // 6
    iDETAIL_SETTING, // 7

    iPLAYER_COLOR3, // 8
    iPLAYER_COLOR, // 9

    iSERVER_NAME, // 10
    iSERVER_NAME2, // 11
    iPLAYER_PASSWORD, // 12

    iMPGAME0_ID, // 13
    iMPGAME1_ID, // 14
    iMPGAME2_ID, // 15
    iMPGAME3_ID, // 16
    iMPGAME4_ID, // 17

    iCUR_MPGAME_ID, // 18
    iCUR_MPGAME_ID2, // 19

    iPLAYER_NAME2, // 20
    iSCREEN_RESOLUTION, // 21
    iHOST_NAME, // 22

    iKEEP_IN_USE, // 23
    iKEEP_CLEAN_UP, // 24

    iKEEP_MODE, // 25
    iPANNING_ON, // 26
    iDESTR_MODE, // 27

    iPLAYER_COLOR2, // 28
    iPLAYER_NAME3, // 29

    iMECH_SOUND, // 30
    iBACK_SOUND, // 31

    iJOYSTICK_TYPE, // 32

    iPROXY_USAGE, // 33
    iPROXY_SERVER, // 34
    iPROXY_PORT, // 35

    iPROXY_SERVER_STR, // 36
    iPROXY_PORT_STR, // 37

    iSERVER_PORT, // 38

    iPLAYER_NAME_CR, // 39
    iPLAYER_PASSWORD_CR, // 40

    iIP_ADDRESS, // 41
    iCAMERA_TURN, // 42
    iCAMERA_SLOPE, // 43
    iCAMERA_SCALE, // 44

    iFULLSCREEN, // 45

    iAUTO_ACCELERATION, // 46

    iMAX_OPTION_ID
};
