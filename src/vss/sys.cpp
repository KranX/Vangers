//
// Created by caiiiycuk on 16.05.2022.
//

#include "sys.h"

#include <duktape.h>

#include "sys-bridge.h"
#include "sys-modules.h"
#include "xgraph.h"

using namespace vss;

Sys::Sys() {
  ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr,
                        [](void* udata, const char* msg) { ErrH.Abort(msg); });

  initModules(ctx);
  initBridge(ctx);
}

Sys::~Sys() { duk_destroy_heap(ctx); }

duk_context* Sys::getContext() {
  return ctx;
}

void Sys::initScripts(const char* folder) {
  setScriptsFolder(ctx, folder);
}

QuantBuilder Sys::quant(const char* eventName) {
  return QuantBuilder(ctx, eventName);
}

Sys& vss::sys() {
  static Sys sys;
  return sys;
}

void sys_initScripts(const char* folder) { sys().initScripts(folder); }

bool sys_readyQuant() {
  auto result = sys().quant(READY_QUANT).send();
  return !result.isPreventDefault();
}

void sys_scaledRendererQuant(bool enabled) {
  static bool current = false;
  if (current == enabled) {
    return;
  }
  current = enabled;

  sys().quant(SCALED_RENDERER_QUANT)
    .prop("enabled", enabled)
    .send();
}

void sys_runtimeObjectQuant(int runtimeObjectId) {
  static int currentRuntimeObjectId = -1;
  if (currentRuntimeObjectId == runtimeObjectId) {
    return;
  }
  currentRuntimeObjectId = runtimeObjectId;

  sys().quant(RUNTIME_OBJECT_QUANT)
    .prop("runtimeObjectId", runtimeObjectId)
    .send();
}
