//
// Created by caiiiycuk on 17.05.2022.
//

#ifndef VANGERS_SYS_EVENT_H
#define VANGERS_SYS_EVENT_H

namespace vangers {
    extern const char *SYS_EVENT_READY;
    extern const char *SYS_EVENT_SCALED_RENDERER_ENABLED;
    extern const char *SYS_EVENT_SCALED_RENDERER_DISABLED;

    struct Event {
        const char *type;
    };
}

#endif //VANGERS_SYS_EVENT_H
