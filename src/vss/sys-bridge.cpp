//
// Created by caiiiycuk on 21.06.22.
//
#include "sys-bridge.h"

#include <xerrhand.h>
#include <SDL_keyboard.h>

#include "sys-modules.h"

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
     1},
    {"isKeyPressed",
            [](duk_context* ctx) -> duk_ret_t {
                auto scancode = duk_require_int(ctx, 0);
                auto pressed = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_A] != 0;
                duk_push_boolean(ctx, pressed);
                return 1;
            },
            1},
    { NULL, NULL, 0 }
};

void initBridge(duk_context* ctx) {
  duk_push_global_object(ctx);
  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, bridgeFunctions);
  duk_put_prop_string(ctx, -2, "bridge");
  duk_pop(ctx);
}
