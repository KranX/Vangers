//
// Created by nikita on 15.03.18.
//

#include "Timer.h"

long long int nowMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void util::Timer::event_start(){
    event_start_time = nowMillis();
}

double util::Timer::event_end(){
    auto now = nowMillis();
    return now - event_start_time;
}