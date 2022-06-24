declare const bridge: VssNative;
const global = new Function("return this;")();

export type VssQuantName = keyof VssQuantMap;
export type VssQuantPayload<K extends keyof VssQuantMap> = VssQuantMap[K][0];
export type VssQuantResult<K extends keyof VssQuantMap> = {} | void | undefined | "preventDefault" | VssQuantMap[K][1];

export interface VssQuantMap {
    "ready": [void, void],
    "runtime_object": [VssRuntimeObjectQuant, void],
    "scaled_renderer": [VssScaledRendererQuant, void],
    "option": [VssOptionQuant, VssOptionQuantResult],
    "camera": [VssCameraQuant, VssCameraQuantResult],
    "mechos_traction": [VssMechosTractionQuant, VssMechosTractionQuantResult],
    "send_event": [VssSendEventQuant, void],
    "set_road_fullscreen": [VssRoadFullScreenQuant, VssRoadFullScreenQuantResult],
}

export interface VssRuntimeObjectQuant {
    runtimeObjectId: RoadRuntimeObjId,
}

export interface VssScaledRendererQuant {
    enabled: boolean,
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

export interface VssSendEventQuant {
    code: actintItemEvents | actEventCodes,
    data: number,
    ptr: number,
    asFlags: ASFlag,
    asMode: ASMode,
}

export interface VssRoadFullScreenQuant {
    enabled: boolean,
}

export interface VssRoadFullScreenQuantResult {
    enabled?: boolean,
}

export type VssQuantListener<K extends VssQuantName> = (payload: VssQuantPayload<K>,
    stopPropogation: () => void, quant: K) => VssQuantResult<K>;

export class VssMath {
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
            return value !== "vss.js" && value !== "main.js" && value.endsWith(".js");
        }).map((value) => value.substring(0, value.length - 3));

        global.onVssQuant = this.onVssQuant.bind(this);
    }

    fatal(msg: string) {
        bridge.fatal("vss: " + msg);
    }

    getScriptsFolder = bridge.getScriptsFolder;
    initScripts = bridge.initScripts;
    sendEvent = bridge.sendEvent;
    isKeyPressed = bridge.isKeyPressed;

    addQuantListener<K extends VssQuantName>(quant: K, listener: VssQuantListener<K>) {
        if (this.quantListeners[quant] === undefined) {
            this.quantListeners[quant] = [];
        }
        this.quantListeners[quant].push(listener);
    }

    removeQuantListener<K extends VssQuantName>(quant: K, listener: VssQuantListener<K>) {
        const index = this.quantListeners[quant]?.indexOf(listener);
        if (index !== undefined && index !== -1) {
            this.quantListeners[quant].splice(index, 1);
        }
    }

    private onVssQuant<K extends VssQuantName>(quant: K, payload: VssQuantPayload<K>) {
        const listeners = this.quantListeners[quant];
        if (listeners === undefined || listeners.length === 0) {
            return undefined;
        }


        const resultRef = {
            result: {
                preventDefault: false,
            },
            runNext: true,
            preventDefault: false,
        };
        const stopPropogation = () => {
            resultRef.runNext = false;
        };
        for (const next of listeners) {
            const result = next(payload, stopPropogation, quant);
            if (result === "preventDefault") {
                resultRef.result.preventDefault = true;
            } else if (result !== undefined) {
                resultRef.result = { ...resultRef.result, ...result };
            }

            if (!resultRef.runNext) {
                break;
            }
        }

        return resultRef.result;
    }
}

/**
 * Vangers Scripting Subsystem exports `vss` object as entry point to game API
 * you must use it to interact with game.
 */
const vss = new Vss();
export default vss;

/* eslint-disable no-unused-vars */

// == native bridge
interface VssNative {
    fatal(msg: string): void;
    scripts(): string[];
    getScriptsFolder(): string;
    initScripts(folder: string): void;
    sendEvent(code: actEventCodes): void;
    isKeyPressed(scanCode: number): boolean;
}

// == in game definitions
export enum RoadRuntimeObjId {
    RTO_GAME_QUANT_ID = 1,
    RTO_LOADING1_ID, // 2
    RTO_LOADING2_ID, // 3
    RTO_LOADING3_ID, // 4
    RTO_MAIN_MENU_ID, // 5
    RTO_FIRST_ESCAVE_ID, // 6
    RTO_FIRST_ESCAVE_OUT_ID, // 7
    RTO_ESCAVE_ID, // 8
    RTO_ESCAVE_OUT_ID, // 9
    RTO_PALETTE_TRANSFORM_ID, // 10
    RTO_SHOW_IMAGE_ID, // 11
    RTO_SHOW_AVI_ID, // 12
    RTO_MAX_ID
};


export enum SDLScanCode {
    SDL_SCANCODE_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    SDL_SCANCODE_A = 4,
    SDL_SCANCODE_B = 5,
    SDL_SCANCODE_C = 6,
    SDL_SCANCODE_D = 7,
    SDL_SCANCODE_E = 8,
    SDL_SCANCODE_F = 9,
    SDL_SCANCODE_G = 10,
    SDL_SCANCODE_H = 11,
    SDL_SCANCODE_I = 12,
    SDL_SCANCODE_J = 13,
    SDL_SCANCODE_K = 14,
    SDL_SCANCODE_L = 15,
    SDL_SCANCODE_M = 16,
    SDL_SCANCODE_N = 17,
    SDL_SCANCODE_O = 18,
    SDL_SCANCODE_P = 19,
    SDL_SCANCODE_Q = 20,
    SDL_SCANCODE_R = 21,
    SDL_SCANCODE_S = 22,
    SDL_SCANCODE_T = 23,
    SDL_SCANCODE_U = 24,
    SDL_SCANCODE_V = 25,
    SDL_SCANCODE_W = 26,
    SDL_SCANCODE_X = 27,
    SDL_SCANCODE_Y = 28,
    SDL_SCANCODE_Z = 29,

    SDL_SCANCODE_1 = 30,
    SDL_SCANCODE_2 = 31,
    SDL_SCANCODE_3 = 32,
    SDL_SCANCODE_4 = 33,
    SDL_SCANCODE_5 = 34,
    SDL_SCANCODE_6 = 35,
    SDL_SCANCODE_7 = 36,
    SDL_SCANCODE_8 = 37,
    SDL_SCANCODE_9 = 38,
    SDL_SCANCODE_0 = 39,

    SDL_SCANCODE_RETURN = 40,
    SDL_SCANCODE_ESCAPE = 41,
    SDL_SCANCODE_BACKSPACE = 42,
    SDL_SCANCODE_TAB = 43,
    SDL_SCANCODE_SPACE = 44,

    SDL_SCANCODE_MINUS = 45,
    SDL_SCANCODE_EQUALS = 46,
    SDL_SCANCODE_LEFTBRACKET = 47,
    SDL_SCANCODE_RIGHTBRACKET = 48,
    SDL_SCANCODE_BACKSLASH = 49, /** < Located at the lower left of the return
                                  *   key on ISO keyboards and at the right end
                                  *   of the QWERTY row on ANSI keyboards.
                                  *   Produces REVERSE SOLIDUS (backslash) and
                                  *   VERTICAL LINE in a US layout, REVERSE
                                  *   SOLIDUS and VERTICAL LINE in a UK Mac
                                  *   layout, NUMBER SIGN and TILDE in a UK
                                  *   Windows layout, DOLLAR SIGN and POUND SIGN
                                  *   in a Swiss German layout, NUMBER SIGN and
                                  *   APOSTROPHE in a German layout, GRAVE
                                  *   ACCENT and POUND SIGN in a French Mac
                                  *   layout, and ASTERISK and MICRO SIGN in a
                                  *   French Windows layout.
                                  */
    SDL_SCANCODE_NONUSHASH = 50, /** < ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate SDL_SCANCODE_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because SDL
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
    SDL_SCANCODE_SEMICOLON = 51,
    SDL_SCANCODE_APOSTROPHE = 52,
    SDL_SCANCODE_GRAVE = 53, /** < Located in the top left corner (on both ANSI
                              *   and ISO keyboards). Produces GRAVE ACCENT and
                              *   TILDE in a US Windows layout and in US and UK
                              *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                              *   and NOT SIGN in a UK Windows layout, SECTION
                              *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                              *   layouts on ISO keyboards, SECTION SIGN and
                              *   DEGREE SIGN in a Swiss German layout (Mac:
                              *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                              *   DEGREE SIGN in a German layout (Mac: only on
                              *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                              *   French Windows layout, COMMERCIAL AT and
                              *   NUMBER SIGN in a French Mac layout on ISO
                              *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                              *   SIGN in a Swiss German, German, or French Mac
                              *   layout on ANSI keyboards.
                              */
    SDL_SCANCODE_COMMA = 54,
    SDL_SCANCODE_PERIOD = 55,
    SDL_SCANCODE_SLASH = 56,

    SDL_SCANCODE_CAPSLOCK = 57,

    SDL_SCANCODE_F1 = 58,
    SDL_SCANCODE_F2 = 59,
    SDL_SCANCODE_F3 = 60,
    SDL_SCANCODE_F4 = 61,
    SDL_SCANCODE_F5 = 62,
    SDL_SCANCODE_F6 = 63,
    SDL_SCANCODE_F7 = 64,
    SDL_SCANCODE_F8 = 65,
    SDL_SCANCODE_F9 = 66,
    SDL_SCANCODE_F10 = 67,
    SDL_SCANCODE_F11 = 68,
    SDL_SCANCODE_F12 = 69,

    SDL_SCANCODE_PRINTSCREEN = 70,
    SDL_SCANCODE_SCROLLLOCK = 71,
    SDL_SCANCODE_PAUSE = 72,
    SDL_SCANCODE_INSERT = 73, /** < insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    SDL_SCANCODE_HOME = 74,
    SDL_SCANCODE_PAGEUP = 75,
    SDL_SCANCODE_DELETE = 76,
    SDL_SCANCODE_END = 77,
    SDL_SCANCODE_PAGEDOWN = 78,
    SDL_SCANCODE_RIGHT = 79,
    SDL_SCANCODE_LEFT = 80,
    SDL_SCANCODE_DOWN = 81,
    SDL_SCANCODE_UP = 82,

    SDL_SCANCODE_NUMLOCKCLEAR = 83, /** < num lock on PC, clear on Mac keyboards
                                     */
    SDL_SCANCODE_KP_DIVIDE = 84,
    SDL_SCANCODE_KP_MULTIPLY = 85,
    SDL_SCANCODE_KP_MINUS = 86,
    SDL_SCANCODE_KP_PLUS = 87,
    SDL_SCANCODE_KP_ENTER = 88,
    SDL_SCANCODE_KP_1 = 89,
    SDL_SCANCODE_KP_2 = 90,
    SDL_SCANCODE_KP_3 = 91,
    SDL_SCANCODE_KP_4 = 92,
    SDL_SCANCODE_KP_5 = 93,
    SDL_SCANCODE_KP_6 = 94,
    SDL_SCANCODE_KP_7 = 95,
    SDL_SCANCODE_KP_8 = 96,
    SDL_SCANCODE_KP_9 = 97,
    SDL_SCANCODE_KP_0 = 98,
    SDL_SCANCODE_KP_PERIOD = 99,

    SDL_SCANCODE_NONUSBACKSLASH = 100, /** < This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    SDL_SCANCODE_APPLICATION = 101, /** < windows contextual menu, compose */
    SDL_SCANCODE_POWER = 102, /** < The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
    SDL_SCANCODE_KP_EQUALS = 103,
    SDL_SCANCODE_F13 = 104,
    SDL_SCANCODE_F14 = 105,
    SDL_SCANCODE_F15 = 106,
    SDL_SCANCODE_F16 = 107,
    SDL_SCANCODE_F17 = 108,
    SDL_SCANCODE_F18 = 109,
    SDL_SCANCODE_F19 = 110,
    SDL_SCANCODE_F20 = 111,
    SDL_SCANCODE_F21 = 112,
    SDL_SCANCODE_F22 = 113,
    SDL_SCANCODE_F23 = 114,
    SDL_SCANCODE_F24 = 115,
    SDL_SCANCODE_EXECUTE = 116,
    SDL_SCANCODE_HELP = 117,
    SDL_SCANCODE_MENU = 118,
    SDL_SCANCODE_SELECT = 119,
    SDL_SCANCODE_STOP = 120,
    SDL_SCANCODE_AGAIN = 121, /** < redo */
    SDL_SCANCODE_UNDO = 122,
    SDL_SCANCODE_CUT = 123,
    SDL_SCANCODE_COPY = 124,
    SDL_SCANCODE_PASTE = 125,
    SDL_SCANCODE_FIND = 126,
    SDL_SCANCODE_MUTE = 127,
    SDL_SCANCODE_VOLUMEUP = 128,
    SDL_SCANCODE_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     SDL_SCANCODE_LOCKINGCAPSLOCK = 130,  */
    /*     SDL_SCANCODE_LOCKINGNUMLOCK = 131, */
    /*     SDL_SCANCODE_LOCKINGSCROLLLOCK = 132, */
    SDL_SCANCODE_KP_COMMA = 133,
    SDL_SCANCODE_KP_EQUALSAS400 = 134,

    SDL_SCANCODE_INTERNATIONAL1 = 135, /** < used on Asian keyboards, see
                                            footnotes in USB doc */
    SDL_SCANCODE_INTERNATIONAL2 = 136,
    SDL_SCANCODE_INTERNATIONAL3 = 137, /** < Yen */
    SDL_SCANCODE_INTERNATIONAL4 = 138,
    SDL_SCANCODE_INTERNATIONAL5 = 139,
    SDL_SCANCODE_INTERNATIONAL6 = 140,
    SDL_SCANCODE_INTERNATIONAL7 = 141,
    SDL_SCANCODE_INTERNATIONAL8 = 142,
    SDL_SCANCODE_INTERNATIONAL9 = 143,
    SDL_SCANCODE_LANG1 = 144, /** < Hangul/English toggle */
    SDL_SCANCODE_LANG2 = 145, /** < Hanja conversion */
    SDL_SCANCODE_LANG3 = 146, /** < Katakana */
    SDL_SCANCODE_LANG4 = 147, /** < Hiragana */
    SDL_SCANCODE_LANG5 = 148, /** < Zenkaku/Hankaku */
    SDL_SCANCODE_LANG6 = 149, /** < reserved */
    SDL_SCANCODE_LANG7 = 150, /** < reserved */
    SDL_SCANCODE_LANG8 = 151, /** < reserved */
    SDL_SCANCODE_LANG9 = 152, /** < reserved */

    SDL_SCANCODE_ALTERASE = 153, /** < Erase-Eaze */
    SDL_SCANCODE_SYSREQ = 154,
    SDL_SCANCODE_CANCEL = 155,
    SDL_SCANCODE_CLEAR = 156,
    SDL_SCANCODE_PRIOR = 157,
    SDL_SCANCODE_RETURN2 = 158,
    SDL_SCANCODE_SEPARATOR = 159,
    SDL_SCANCODE_OUT = 160,
    SDL_SCANCODE_OPER = 161,
    SDL_SCANCODE_CLEARAGAIN = 162,
    SDL_SCANCODE_CRSEL = 163,
    SDL_SCANCODE_EXSEL = 164,

    SDL_SCANCODE_KP_00 = 176,
    SDL_SCANCODE_KP_000 = 177,
    SDL_SCANCODE_THOUSANDSSEPARATOR = 178,
    SDL_SCANCODE_DECIMALSEPARATOR = 179,
    SDL_SCANCODE_CURRENCYUNIT = 180,
    SDL_SCANCODE_CURRENCYSUBUNIT = 181,
    SDL_SCANCODE_KP_LEFTPAREN = 182,
    SDL_SCANCODE_KP_RIGHTPAREN = 183,
    SDL_SCANCODE_KP_LEFTBRACE = 184,
    SDL_SCANCODE_KP_RIGHTBRACE = 185,
    SDL_SCANCODE_KP_TAB = 186,
    SDL_SCANCODE_KP_BACKSPACE = 187,
    SDL_SCANCODE_KP_A = 188,
    SDL_SCANCODE_KP_B = 189,
    SDL_SCANCODE_KP_C = 190,
    SDL_SCANCODE_KP_D = 191,
    SDL_SCANCODE_KP_E = 192,
    SDL_SCANCODE_KP_F = 193,
    SDL_SCANCODE_KP_XOR = 194,
    SDL_SCANCODE_KP_POWER = 195,
    SDL_SCANCODE_KP_PERCENT = 196,
    SDL_SCANCODE_KP_LESS = 197,
    SDL_SCANCODE_KP_GREATER = 198,
    SDL_SCANCODE_KP_AMPERSAND = 199,
    SDL_SCANCODE_KP_DBLAMPERSAND = 200,
    SDL_SCANCODE_KP_VERTICALBAR = 201,
    SDL_SCANCODE_KP_DBLVERTICALBAR = 202,
    SDL_SCANCODE_KP_COLON = 203,
    SDL_SCANCODE_KP_HASH = 204,
    SDL_SCANCODE_KP_SPACE = 205,
    SDL_SCANCODE_KP_AT = 206,
    SDL_SCANCODE_KP_EXCLAM = 207,
    SDL_SCANCODE_KP_MEMSTORE = 208,
    SDL_SCANCODE_KP_MEMRECALL = 209,
    SDL_SCANCODE_KP_MEMCLEAR = 210,
    SDL_SCANCODE_KP_MEMADD = 211,
    SDL_SCANCODE_KP_MEMSUBTRACT = 212,
    SDL_SCANCODE_KP_MEMMULTIPLY = 213,
    SDL_SCANCODE_KP_MEMDIVIDE = 214,
    SDL_SCANCODE_KP_PLUSMINUS = 215,
    SDL_SCANCODE_KP_CLEAR = 216,
    SDL_SCANCODE_KP_CLEARENTRY = 217,
    SDL_SCANCODE_KP_BINARY = 218,
    SDL_SCANCODE_KP_OCTAL = 219,
    SDL_SCANCODE_KP_DECIMAL = 220,
    SDL_SCANCODE_KP_HEXADECIMAL = 221,

    SDL_SCANCODE_LCTRL = 224,
    SDL_SCANCODE_LSHIFT = 225,
    SDL_SCANCODE_LALT = 226, /** < alt, option */
    SDL_SCANCODE_LGUI = 227, /** < windows, command (apple), meta */
    SDL_SCANCODE_RCTRL = 228,
    SDL_SCANCODE_RSHIFT = 229,
    SDL_SCANCODE_RALT = 230, /** < alt gr, option */
    SDL_SCANCODE_RGUI = 231, /** < windows, command (apple), meta */

    SDL_SCANCODE_MODE = 257, /** < I'm not sure if this is really not covered
                                 *   by any of the above, but since there's a
                                 *   special KMOD_MODE for it I'm adding it here
                                 */

    /* @} *//* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    SDL_SCANCODE_AUDIONEXT = 258,
    SDL_SCANCODE_AUDIOPREV = 259,
    SDL_SCANCODE_AUDIOSTOP = 260,
    SDL_SCANCODE_AUDIOPLAY = 261,
    SDL_SCANCODE_AUDIOMUTE = 262,
    SDL_SCANCODE_MEDIASELECT = 263,
    SDL_SCANCODE_WWW = 264,
    SDL_SCANCODE_MAIL = 265,
    SDL_SCANCODE_CALCULATOR = 266,
    SDL_SCANCODE_COMPUTER = 267,
    SDL_SCANCODE_AC_SEARCH = 268,
    SDL_SCANCODE_AC_HOME = 269,
    SDL_SCANCODE_AC_BACK = 270,
    SDL_SCANCODE_AC_FORWARD = 271,
    SDL_SCANCODE_AC_STOP = 272,
    SDL_SCANCODE_AC_REFRESH = 273,
    SDL_SCANCODE_AC_BOOKMARKS = 274,

    /* @} *//* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
    /* @{ */

    SDL_SCANCODE_BRIGHTNESSDOWN = 275,
    SDL_SCANCODE_BRIGHTNESSUP = 276,
    SDL_SCANCODE_DISPLAYSWITCH = 277, /** < display mirroring/dual display
                                           switch, video mode switch */
    SDL_SCANCODE_KBDILLUMTOGGLE = 278,
    SDL_SCANCODE_KBDILLUMDOWN = 279,
    SDL_SCANCODE_KBDILLUMUP = 280,
    SDL_SCANCODE_EJECT = 281,
    SDL_SCANCODE_SLEEP = 282,

    SDL_SCANCODE_APP1 = 283,
    SDL_SCANCODE_APP2 = 284,

    /* @} *//* Walther keys */

    /**
     *  \name Usage page 0x0C (additional media keys)
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    SDL_SCANCODE_AUDIOREWIND = 285,
    SDL_SCANCODE_AUDIOFASTFORWARD = 286,

    /* @} *//* Usage page 0x0C (additional media keys) */

    /* Add any other keys here. */

    SDL_NUM_SCANCODES = 512 /** < not a key, just marks the number of scancodes
                                 for array bounds */
};


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

export enum actintItemEvents {
    ACI_PUT_ITEM = 1,
    ACI_DROP_ITEM,
    ACI_CHANGE_ITEM_DATA,

    ACI_ACTIVATE_ITEM,
    ACI_DEACTIVATE_ITEM,
    ACI_GET_ITEM_DATA,
    ACI_CHECK_MOUSE,
    ACI_SET_DROP_LEVEL,

    ACI_PUT_IN_SLOT,

    ACI_LOCK_INTERFACE,
    ACI_UNLOCK_INTERFACE,

    ACI_SHOW_TEXT,
    ACI_HIDE_TEXT,

    ACI_SHOW_ITEM_TEXT,

    ACI_DROP_CONFIRM,

    ACI_MAX_EVENT
};

export enum actEventCodes {
    EV_CHANGE_MODE = actintItemEvents.ACI_MAX_EVENT + 1,
    EV_SET_MODE,
    EV_ACTIVATE_MENU,
    EV_CHANGE_SCREEN,
    EV_FULLSCR_CHANGE,
    EV_ACTIVATE_IINV,
    EV_DEACTIVATE_IINV,
    EV_ACTIVATE_MATRIX,
    EV_DEACTIVATE_MATRIX,
    EV_EVINCE_PALETTE,
    EV_INIT_MATRIX_OBJ,
    EV_INIT_SC_MATRIX_OBJ,
    EV_REDRAW,
    EV_CANCEL_MATRIX,
    EV_AUTO_MOVE_ITEMS,
    EV_SET_MECH_NAME,

    EV_NEXT_SHOP_AVI,
    EV_PREV_SHOP_AVI,

    EV_CHANGE_AVI_LIST,

    EV_BUY_ITEM,

    EV_SET_ITM_PICKUP,
    EV_SET_WPN_PICKUP,

    EV_ACTIVATE_SHOP_MENU,
    EV_CHOOSE_SHOP_ITEM,

    EV_NEXT_PHRASE,
    EV_START_SPEECH,
    EV_SHOW_QUESTS,
    EV_ASK_QUEST,

    EV_TRY_2_ENTER,
    EV_GET_CIRT,

    EV_TAKE_ELEECH,
    EV_GET_ELEECH,
    EV_ISCR_KEYTRAP,

    EV_LOCK_ISCREEN,
    EV_UNLOCK_ISCREEN,

    EV_SELL_MOVE_ITEM,
    EV_CHANGE_AVI_INDEX,

    EV_TELEPORT,

    EV_INIT_BUTTONS,

    EV_ENTER_TEXT_MODE,
    EV_LEAVE_TEXT_MODE,

    EV_PROTRACTOR_EVENT,
    EV_MECH_MESSIAH_EVENT,

    EV_GET_BLOCK_PHRASE,

    EV_PAUSE_AML,
    EV_RESUME_AML,

    EV_ENTER_CHAT,
    EV_LEAVE_CHAT,

    EV_ITEM_TEXT,

    EV_GET_RUBOX,
    EV_INIT_AVI_OBJECT,

    EV_MAX_CODE
};

// actInt modes
export enum ASMode {
    AS_INV_MODE = 0x00,
    AS_INFO_MODE = 0x01,
}

// actInt flags
export enum ASFlag {
    AS_FULL_REDRAW = 0x01,
    aMS_LEFT_PRESS = 0x02,
    aMS_RIGHT_PRESS = 0x04,
    aMS_MOVED = 0x08,
    aMS_PRESS = aMS_LEFT_PRESS | aMS_RIGHT_PRESS | aMS_MOVED,
    AS_CHANGE_MODE = 0x10,
    AS_INV_MOVE_ITEM = 0x20,
    AS_INV_SET_DROP = 0x40,
    AS_FULL_FLUSH = 0x80,
    AS_FULLSCR = 0x100,
    AS_ISCREEN = 0x200,
    AS_ISCREEN_INV_MODE = 0x400,
    AS_EVINCE_PALETTE = 0x800,
    AS_LOCKED = 0x1000,
    AS_TEXT_MODE = 0x2000,
    AS_CHAT_MODE = 0x4000,
    AS_WORLDS_INIT = 0x8000,
}
