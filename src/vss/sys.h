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
struct OptionQuant {
  int optionValue;
  const int optionId;
};

struct JoystickQuant {
  bool active;
  int traction;
  int rudder;
  bool helicopterStrife;

  const int tractionIncrement;
  const int tractionDecrement;
  const int tractionMax;
  const int rudderStep;
  const int rudderMax;
  const int unitAngle;
};

struct CameraQuant {
  int slopeAngle;
  int turnAngle;
};

typedef std::function<void(OptionQuant&)> OptionQuantFunction;
typedef std::function<void(JoystickQuant&)> JoystickQuantFunction;
typedef std::function<void(CameraQuant&)> CameraQuantFunction;

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

  void initScripts(const char* folder);

  int rendererWidth();
  int rendererHeight();

  QuantBuilder quant(const char* eventName);

  OptionQuantFunction& getOptionQuantFunction();
  void setOptionQuantFunction(const OptionQuantFunction& fn);

  JoystickQuantFunction& getJoystickQuantFunction();
  void setJoystickQuantFunction(const JoystickQuantFunction& fn);

  CameraQuantFunction& getCameraQuantFunction();
  void setCameraQuantFunction(const CameraQuantFunction& fn);

  void postEvent(const Event& event);
  size_t addEventListener(const std::function<void(Event)>& listener);
  void removeEventListener(size_t id);

 private:
  Sys();
  ~Sys();
  friend Sys& sys();

  duk_context* ctx;

  OptionQuantFunction optionQuantFunction;
  JoystickQuantFunction joystickQuantFunction;
  CameraQuantFunction cameraQuantFunction;
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
