//
// Created by caiiiycuk on 16.05.2022.
//

#include "sys.h"

#include <duktape.h>

#include "xgraph.h"

using namespace vangers;

const char* vangers::SYS_EVENT_READY = "sys_ready";
const char* vangers::SYS_EVENT_SCALED_RENDERER_CHANGED =
    "sys_scaled_renderer_changed";
const char* vangers::SYS_EVENT_RUNTIME_OBJECT_CHANGED =
    "sys_runtime_object_changed";

duk_context* ctx = nullptr;

Sys::Sys() { ctx = duk_create_heap_default(); }

Sys::~Sys() { duk_destroy_heap(ctx); }

int Sys::rendererWidth() { return XGR_Obj.hdWidth; }

int Sys::rendererHeight() { return XGR_Obj.hdHeight; }

OptionQuantFunction& Sys::getOptionQuantFunction() {
  return optionkQuantFunction;
}

void Sys::setOptionQuantFunction(const OptionQuantFunction& fn) {
  optionkQuantFunction = fn;
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

Sys& vangers::sys() {
  static Sys sys;
  return sys;
}

void sys_postReadyEvent() { sys().postEvent({.type = SYS_EVENT_READY}); }

void sys_postScaledRendererChangedEvent(bool enabled) {
  sys().postEvent({.type = vangers::SYS_EVENT_SCALED_RENDERER_CHANGED,
                   .scaledRenderer = enabled});
}

void sys_postRuntimeObjectChangedEvent(int runtimeObjectId) {
  static int currentRuntimeObjectId = -1;
  if (currentRuntimeObjectId == runtimeObjectId) {
    return;
  }
  currentRuntimeObjectId = runtimeObjectId;
  sys().postEvent({
      .type = vangers::SYS_EVENT_RUNTIME_OBJECT_CHANGED,
      .runtimeObjectId = runtimeObjectId,
  });
}
