//
// Created by caiiiycuk on 16.05.2022.
//

#ifndef VANGERS_SYS_H
#define VANGERS_SYS_H

#include <vector>
#include <functional>
#include "event.h"

namespace vangers {
    struct JoystickQuant {
        bool active;
        int traction;
        int rudder;
        bool helicopterStrife;
    };
    typedef std::function<JoystickQuant(int traction, int tinc, int tdec, int tmax,
                                        int rudder, int rster, int rmax, float angle)> JoystickQuantFunction;

    class Sys {
    public:
        int rendererWidth();
        int rendererHeight();

        JoystickQuantFunction& getJoystickQuantFunction();
        void setJoystickQuantFunction(const JoystickQuantFunction& fn);

        void postEvent(const Event& event);
        size_t addEventListener(const std::function<void(Event)> listener);
        void removeEventListener(size_t id);
    private:
        Sys & operator=(const Sys&) = delete;
        Sys(const Sys&) = delete;
        Sys() = default;
        friend Sys &sys();

        JoystickQuantFunction joystickQuantFunction;
        std::vector<std::function<void(Event)>> listeners;
    };

    Sys &sys();
}

// @caiiiycuk: to use without including <vangers/sys.h>
extern void sys_postReadyEvent();
extern void sys_postScaledRendererChangedEvent(bool enabled);
extern void sys_postRuntimeObjectChangedEvent(int runtimeObjectId);

#endif //VANGERS_SYS_H
