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
        std::vector<std::function<void(Event)>> listeners;
    };

    Sys &sys();
}

// @caiiiycuk: to use without including <vangers/sys.h>
extern void sys_postReadyEvent();
extern void sys_postScaledRendererEvent(bool enabled);

#endif //VANGERS_SYS_H
