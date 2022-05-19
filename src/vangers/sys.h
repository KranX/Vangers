//
// Created by caiiiycuk on 16.05.2022.
//

#ifndef VANGERS_SYS_H
#define VANGERS_SYS_H

#include <vector>
#include <functional>
#include "event.h"

namespace vangers {
    class Sys {
    public:
        int rendererWidth();
        int rendererHeight();

        void postEvent(const Event& event);
        size_t addEventListener(const std::function<void(Event)> listener);
        void removeEventListener(size_t id);
    private:
        Sys & operator=(const Sys&) = delete;
        Sys(const Sys&) = delete;
        Sys() = default;
        friend Sys &sys();

        std::vector<std::function<void(Event)>> listeners;
    };

    Sys &sys();
}

// @caiiiycuk: to use without including <vangers/sys.h>
extern void sys_postReadyEvent();
extern void sys_postScaledRendererChangedEvent(bool enabled);
extern void sys_postRuntimeObjectChangedEvent(int runtimeObjectId);

#endif //VANGERS_SYS_H
