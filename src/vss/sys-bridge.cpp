//
// Created by caiiiycuk on 21.06.22.
//
#include "sys-bridge.h"

#include <filesystem>

#ifdef _SURMAP_
void initBridge(duk_context* ctx) {}
#else

#include <SDL_keyboard.h>
#include <xerrhand.h>

#include "sys-modules.h"
#include "sys.h"
#include "xgraph.h"

// clang-format off
#include "../actint/item_api.h"
#include "../iscreen/iscreen.h"
#include "../actint/actint.h"
#include "../actint/a_consts.h"
// clang-format on

extern actIntDispatcher* aScrDisp;
extern void aciHandleCameraEvent(int code, int data);
extern iListElement* iShopItem;
extern int iEvLineID;

const duk_function_list_entry bridgeFunctions[] = {
    {"fatal",
     [](duk_context* ctx) -> duk_ret_t {
       auto message = duk_require_string(ctx, 0);
       ErrH.Abort(message);
       return 0;
     },
     1},
    {"scripts",
     [](duk_context* ctx) -> duk_ret_t {
       auto index = duk_push_array(ctx);
       auto i = 0;
       for (auto& next : getScriptToPathMapping()) {
         duk_push_string(ctx, next.first.c_str());
         duk_put_prop_index(ctx, index, i);
         ++i;
       }
       return 1;
     },
     0},
    {"initScripts",
     [](duk_context* ctx) -> duk_ret_t {
       const char* folder = duk_require_string(ctx, 0);
       vss::sys().initScripts(folder);
       return 0;
     },
     1},
    {"getScriptsFolder",
     [](duk_context* ctx) -> duk_ret_t {
       duk_push_string(ctx, vss::sys().getScriptsFolder().c_str());
       return 1;
     },
     0},
    {"sendEvent",
     [](duk_context* ctx) -> duk_ret_t {
       auto code = duk_require_int(ctx, 0);
       int data =
           duk_is_null_or_undefined(ctx, 1) ? 0 : duk_require_int(ctx, 1);
       if (code == EV_VSS_CAMERA_ROT_EVENT) {
         aciHandleCameraEvent(BMENU_ITEM_ROT, data);
       } else if (code == EV_VSS_CAMERA_ZOOM_EVENT) {
         aciHandleCameraEvent(BMENU_ITEM_ZOOM, data);
       } else if (code == EV_VSS_CAMERA_PERSP_EVENT) {
         aciHandleCameraEvent(BMENU_ITEM_PERSP, data);
       } else if (aScrDisp) {
         aScrDisp->send_event(code, data);
       }
       return 0;
     },
     2},
    {"isKeyPressed",
     [](duk_context* ctx) -> duk_ret_t {
       auto scancode = duk_require_int(ctx, 0);
       auto pressed = SDL_GetKeyboardState(nullptr)[scancode] != 0;
       duk_push_boolean(ctx, pressed);
       return 1;
     },
     1},
    {"isFileExists",
     [](duk_context* ctx) -> duk_ret_t {
       auto file = duk_require_string(ctx, 0);
       duk_push_boolean(ctx, std::filesystem::exists(file));
       return 1;
     },
     1},
    {"getRgbaData",
     [](duk_context* ctx) -> duk_ret_t {
       duk_size_t len;
       uint32_t* frame = (uint32_t*)duk_require_buffer(ctx, 0, &len);
       int frameWidth = duk_require_int(ctx, 1);
       int startX = duk_require_int(ctx, 2);
       int startY = duk_require_int(ctx, 3);
       int width = duk_require_int(ctx, 4);
       int height = duk_require_int(ctx, 5);

       duk_size_t rgbaLen;
       uint32_t* rgba = (uint32_t*)duk_require_buffer(ctx, 6, &rgbaLen);

       if (rgbaLen != width * height * 4) {
         ErrH.Abort(
             "getRgbaFrame: rgba data size must be greater or equal then width "
             "* height * 4");
         return 0;
       }

       for (int y = 0; y < height; ++y) {
         memcpy(rgba + y * width, frame + (y + startY) * frameWidth + startX,
                width * 4);
       }

       uint32_t* pixel = rgba;
       uint8_t* data = (uint8_t*)rgba;
       uint32_t* end = rgba + width * height;
       while (pixel < end) {
         std::swap(data[0], data[2]);
         data[3] = 255;
         data += 4;
         ++pixel;
       }

       return 0;
     },
     7},
    {"toBase64",
     [](duk_context* ctx) -> duk_ret_t {
       duk_base64_encode(ctx, 0);
       return 1;
     },
     1},
    {"getShopItem",
     [](duk_context* ctx) -> duk_ret_t {
       auto idx = duk_push_object(ctx);
       if (iShopItem) {
         if (iEvLineID == MECHOS_MODE || iEvLineID == MECHOS_LIST_MODE) {
           // mechos
           invMatrix* matrix = (invMatrix*)iShopItem;
           duk_push_int(ctx, matrix->internalID);
           duk_put_prop_string(ctx, idx, "internalId");
           duk_push_int(ctx, matrix->type);
           duk_put_prop_string(ctx, idx, "type");
           duk_push_string(ctx, matrix->mech_name);
           duk_put_prop_string(ctx, idx, "mechosName");
         }
       }
       return 1;
     },
     0},
    {NULL, NULL, 0}};

void initBridge(duk_context* ctx) {
  duk_push_global_object(ctx);
  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, bridgeFunctions);
  duk_put_prop_string(ctx, -2, "bridge");
  duk_pop(ctx);
}

#endif