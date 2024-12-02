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

void Sys::initScripts(const char* folder,
                      void (*init)(std::shared_ptr<Context>&)) {
  scriptsFolder = folder;

  if (strlen(folder) == 0) {
    context = std::shared_ptr<Context>(nullptr);
    return;
  }

  context = std::make_shared<Context>();
  if (init) {
    init(context);
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

void sys_tickQuant() { sys().quant(TICK_QUANT).send(); }

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

void sys_frameQuant(void* frame, int width, int height, int bpp) {
  sys()
      .quant(FRAME_QUANT)
      .prop("frame", frame, width * height * bpp)
      .prop("width", width)
      .prop("height", height)
      .prop("bpp", bpp)
      .send();
}

extern "C" const char* sys_fileOpenQuant(const char* file, unsigned flags) {
  auto result = sys()
                    .quant(FILE_OPEN_QUANT)
                    .prop("file", file)
                    .prop("flags", (duk_int_t)flags)
                    .send();

  return result.getString("file", file);
}
