//
// Created by caiiiycuk on 16.05.2022.
//

#ifndef VANGERS_SYS_H
#define VANGERS_SYS_H

#include <functional>
#include <vector>

#include "event.h"

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

class Sys {
 public:
  Sys& operator=(const Sys&) = delete;
  Sys(const Sys&);

  void initScripts(const char* folder);

  int rendererWidth();
  int rendererHeight();

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

  OptionQuantFunction optionkQuantFunction;
  JoystickQuantFunction joystickQuantFunction;
  CameraQuantFunction cameraQuantFunction;
  std::vector<std::function<void(Event)>> listeners;
};

Sys& sys();
}  // namespace vangers

// @caiiiycuk: to use without including <vangers/sys.h>
extern void sys_postReadyEvent();
extern void sys_postScaledRendererChangedEvent(bool enabled);
extern void sys_postRuntimeObjectChangedEvent(int runtimeObjectId);

#endif  // VANGERS_SYS_H
