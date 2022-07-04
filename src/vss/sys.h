//
// Created by caiiiycuk on 16.05.2022.
//

#ifndef VANGERS_SYS_H
#define VANGERS_SYS_H

#include <duktape.h>

#include <memory>
#include <string>
#include <vector>

#include "quant-names.h"

namespace vss {

class Context {
 public:
  duk_context* ctx;
  Context();
  ~Context();
  Context& operator=(const Context&) = delete;
  Context(const Context&) = delete;
};

class QuantResult {
 public:
  explicit QuantResult(std::shared_ptr<Context>& context);
  ~QuantResult();
  bool isNotHandled();
  bool isPreventDefault();
  int getInt(const char* name, int defaultValue);
  bool getBool(const char* name, bool defaultValue);
  const char* getString(const char* name, const char* defaultValue);

 private:
  std::shared_ptr<Context> context;
  duk_context* ctx;
  bool notHandled;
  bool preventDefault;
};

class QuantBuilder {
 public:
  QuantBuilder(std::shared_ptr<Context>& context, const char* eventName);
  QuantBuilder& prop(const char* name, void *value, int size);
  QuantBuilder& prop(const char* name, int value);
  QuantBuilder& prop(const char* name, bool value);
  QuantBuilder& prop(const char* name, const char* value);
  QuantResult send();

 private:
  std::shared_ptr<Context> context;
  duk_context* ctx;
  bool valid;
};

class Sys {
 public:
  Sys& operator=(const Sys&) = delete;
  Sys(const Sys&) = delete;

  void initScripts(const char* folder,
                   void (*init)(std::shared_ptr<Context>&) = nullptr);
  std::string getScriptsFolder();

  QuantBuilder quant(const char* eventName);

 private:
  Sys();
  ~Sys();
  friend Sys& sys();

  std::string scriptsFolder;
  std::shared_ptr<Context> context;
};

Sys& sys();
}  // namespace vss

// @caiiiycuk: to use without including <vangers/sys.h>
extern void sys_initScripts(const char* folder);
extern bool sys_readyQuant();
extern void sys_tickQuant();
extern void sys_runtimeObjectQuant(int runtimeObjectId);
extern void sys_scaledRendererQuant(int enabled);
extern void sys_frameQuant(void* frame, int width, int height, int bpp);
extern "C" const char* sys_fileOpenQuant(const char* file, unsigned flags);

#endif  // VANGERS_SYS_H
