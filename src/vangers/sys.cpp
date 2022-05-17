//
// Created by caiiiycuk on 16.05.2022.
//

#include "sys.h"
#include "xgraph.h"

using namespace vangers;

const char *vangers::SYS_EVENT_READY = "sys_ready";
const char *vangers::SYS_EVENT_SCALED_RENDERER_ENABLED = "sys_scaled_renderer_enabled";
const char *vangers::SYS_EVENT_SCALED_RENDERER_DISABLED = "sys_scaled_renderer_disabled";

int Sys::rendererWidth() {
    return XGR_Obj.hdWidth;
}

int Sys::rendererHeight() {
    return XGR_Obj.hdHeight;
}

size_t Sys::addEventListener(const std::function<void(Event)> listener) {
    auto id = this->listeners.size();
    this->listeners.push_back(listener);
    return id;
}

void Sys::removeEventListener(size_t id) {
    if (id < this->listeners.size()) {
        this->listeners.erase(this->listeners.begin() + id);
    }
}

void Sys::postEvent(const Event &event) {
    for (auto& next: this->listeners) {
        next(event);
    }
}

Sys &vangers::sys() {
    static Sys sys;
    return sys;
}

void sys_postReadyEvent() {
    sys().postEvent({ .type = SYS_EVENT_READY });
}

void sys_postScaledRendererEvent(bool enabled) {
    if (enabled) {
        sys().postEvent({ .type = vangers::SYS_EVENT_SCALED_RENDERER_ENABLED });
    } else {
        sys().postEvent({ .type = vangers::SYS_EVENT_SCALED_RENDERER_DISABLED });
    }
}
