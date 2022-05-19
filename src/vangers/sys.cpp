//
// Created by caiiiycuk on 16.05.2022.
//

#include "sys.h"
#include "xgraph.h"

using namespace vangers;

const char *vangers::SYS_EVENT_READY = "sys_ready";
const char *vangers::SYS_EVENT_SCALED_RENDERER_CHANGED = "sys_scaled_renderer_changed";
const char *vangers::SYS_EVENT_RUNTIME_OBJECT_CHANGED = "sys_runtime_object_changed";

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

void sys_postScaledRendererChangedEvent(bool enabled) {
    sys().postEvent({
        .type = vangers::SYS_EVENT_SCALED_RENDERER_CHANGED,
        .scaledRenderer = enabled
    });
}

void sys_postRuntimeObjectChangedEvent(int runtimeObjectId) {
    static int currentRuntimeObjectId = -1;
    if (currentRuntimeObjectId == runtimeObjectId) {
        return;
    }
    currentRuntimeObjectId = runtimeObjectId;
    sys().postEvent({
        .type = vangers::SYS_EVENT_RUNTIME_OBJECT_CHANGED,
        .runtimeObjectId = runtimeObjectId,
    });
}
