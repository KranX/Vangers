//
// Created by caiiiycuk on 16.05.2022.
//

#ifndef VANGERS_SYS_H
#define VANGERS_SYS_H

#include <duktape.h>

#include <functional>
#include <vector>

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
  QuantBuilder& prop(const char* name, bool value);
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

  QuantBuilder quant(const char* eventName);

 private:
  Sys();
  ~Sys();
  friend Sys& sys();

  duk_context* ctx;
};

Sys& sys();
}  // namespace vss

// @caiiiycuk: to use without including <vangers/sys.h>
extern void sys_initScripts(const char* folder);
extern bool sys_readyQuant();
extern void sys_runtimeObjectQuant(int runtimeObjectId);
extern void sys_scaledRendererQuant(int enabled);

#endif  // VANGERS_SYS_H
