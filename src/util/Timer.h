//
// Created by nikita on 15.03.18.
//

#ifndef VANGERS_PERFCOUNTER_H
#define VANGERS_PERFCOUNTER_H

#include <chrono>
#include <memory>
#include <vector>
#include "ring_buffer.h"

namespace util{
	class Timer {
	private:
		std::chrono::time_point<std::chrono::system_clock> event_start_time{};
	public:
		explicit Timer() = default;

		void event_start(){
			event_start_time = std::chrono::system_clock::now();
		}

		double event_end(){
			auto now = std::chrono::system_clock::now();
			return (now - event_start_time).count() / 1000.0;
		}

	};
}



#endif //VANGERS_PERFCOUNTER_H
