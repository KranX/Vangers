//
// Created by caiiiycuk on 16.05.2022.
//

#include "sys.h"

#include <duktape.h>

#include "sys-bridge.h"
#include "sys-modules.h"
#include "xgraph.h"

using namespace vss;

const char* vss::SYS_EVENT_READY = "sys_ready";
const char* vss::SYS_EVENT_SCALED_RENDERER_CHANGED =
    "sys_scaled_renderer_changed";
const char* vss::SYS_EVENT_RUNTIME_OBJECT_CHANGED =
    "sys_runtime_object_changed";

Sys::Sys() {
  ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr,
                        [](void* udata, const char* msg) { ErrH.Abort(msg); });

  initSysModules(ctx);
  initSysBridge(ctx);
}

Sys::~Sys() { duk_destroy_heap(ctx); }

void Sys::initScripts(const char* folder) {
  setScriptsFolder(ctx, folder);
}

QuantBuilder Sys::quant(const char* eventName) {
  return QuantBuilder(ctx, eventName);
}

int Sys::rendererWidth() { return XGR_Obj.hdWidth; }

int Sys::rendererHeight() { return XGR_Obj.hdHeight; }

OptionQuantFunction& Sys::getOptionQuantFunction() {
  return optionQuantFunction;
}

void Sys::setOptionQuantFunction(const OptionQuantFunction& fn) {
  optionQuantFunction = fn;
}

JoystickQuantFunction& Sys::getJoystickQuantFunction() {
  return joystickQuantFunction;
}

void Sys::setJoystickQuantFunction(const JoystickQuantFunction& fn) {
  joystickQuantFunction = fn;
}

CameraQuantFunction& Sys::getCameraQuantFunction() {
  return cameraQuantFunction;
}

void Sys::setCameraQuantFunction(const CameraQuantFunction& fn) {
  cameraQuantFunction = fn;
}

size_t Sys::addEventListener(const std::function<void(Event)>& listener) {
  auto id = this->listeners.size();
  this->listeners.push_back(listener);
  return id;
}

void Sys::removeEventListener(size_t id) {
  if (id < this->listeners.size()) {
    this->listeners.erase(this->listeners.begin() + id);
  }
}

void Sys::postEvent(const Event& event) {
  for (auto& next : this->listeners) {
    next(event);
  }
}

Sys& vss::sys() {
  static Sys sys;
  return sys;
}

void sys_initScripts(const char* folder) { sys().initScripts(folder); }

void sys_postReadyEvent() { sys().postEvent({.type = SYS_EVENT_READY}); }

void sys_postScaledRendererChangedEvent(bool enabled) {
  sys().postEvent({.type = vss::SYS_EVENT_SCALED_RENDERER_CHANGED,
                   .scaledRenderer = enabled});
}

void sys_postRuntimeObjectChangedEvent(int runtimeObjectId) {
  static int currentRuntimeObjectId = -1;
  if (currentRuntimeObjectId == runtimeObjectId) {
    return;
  }
  currentRuntimeObjectId = runtimeObjectId;
  sys().postEvent({
      .type = vss::SYS_EVENT_RUNTIME_OBJECT_CHANGED,
      .runtimeObjectId = runtimeObjectId,
  });
}
