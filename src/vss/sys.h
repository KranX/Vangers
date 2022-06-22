//
// Created by caiiiycuk on 16.05.2022.
//

#ifndef VANGERS_SYS_H
#define VANGERS_SYS_H

#include <functional>
#include <vector>
#include <duktape.h>

#include "event.h"
#include "quant-names.h"

namespace vss {

class QuantResult {
 public:
  explicit QuantResult(duk_context* ctx);
  ~QuantResult();
  bool isNotHandled();
  bool isPreventDefault();
  int getInt(const char* name, int defaultValue);
 private:
  duk_context* ctx;
  bool notHandled;
  bool preventDefault;
};

class QuantBuilder {
 public:
  QuantBuilder(duk_context* ctx, const char* eventName);
  QuantBuilder& prop(const char* name, int value);
  QuantResult send();
 private:
  duk_context* ctx;
  bool valid;
};

class Sys {
 public:
  Sys& operator=(const Sys&) = delete;
  Sys(const Sys&) = delete;

  duk_context* getContext();
  void initScripts(const char* folder);

  int rendererWidth();
  int rendererHeight();

  QuantBuilder quant(const char* eventName);

  void postEvent(const Event& event);
  size_t addEventListener(const std::function<void(Event)>& listener);
  void removeEventListener(size_t id);

 private:
  Sys();
  ~Sys();
  friend Sys& sys();

  duk_context* ctx;

  std::vector<std::function<void(Event)>> listeners;
};

Sys& sys();
}  // namespace vangers

// @caiiiycuk: to use without including <vangers/sys.h>
extern void sys_initScripts(const char* folder);
extern void sys_postReadyEvent();
extern void sys_postScaledRendererChangedEvent(bool enabled);
extern void sys_postRuntimeObjectChangedEvent(int runtimeObjectId);

#endif  // VANGERS_SYS_H
