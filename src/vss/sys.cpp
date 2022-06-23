//
// Created by caiiiycuk on 16.05.2022.
//

#include "sys.h"

#include <duktape.h>

#include <memory>

#include "sys-bridge.h"
#include "sys-modules.h"
#include "xgraph.h"

using namespace vss;

Context::Context() {
  ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr,
                        [](void* udata, const char* msg) { ErrH.Abort(msg); });

  initModules(ctx);
  initBridge(ctx);
}

Context::~Context() { duk_destroy_heap(ctx); }

Sys::Sys() : context(nullptr) {}

Sys::~Sys() {}

void Sys::initScripts(const char* folder) {
  context = std::make_shared<Context>();
  scriptsFolder = folder;

  if (strlen(folder) == 0) {
    return;
  }

  setScriptsFolder(context->ctx, folder);
}

std::string Sys::getScriptsFolder() { return scriptsFolder; }

QuantBuilder Sys::quant(const char* eventName) {
  return QuantBuilder(context, eventName);
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

  sys().quant(SCALED_RENDERER_QUANT).prop("enabled", enabled).send();
}

void sys_runtimeObjectQuant(int runtimeObjectId) {
  static int currentRuntimeObjectId = -1;
  if (currentRuntimeObjectId == runtimeObjectId) {
    return;
  }
  currentRuntimeObjectId = runtimeObjectId;

  sys()
      .quant(RUNTIME_OBJECT_QUANT)
      .prop("runtimeObjectId", runtimeObjectId)
      .send();
}
