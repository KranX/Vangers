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
		long long int event_start_time;
	public:
		explicit Timer() = default;

		void event_start();

		double event_end();
	};
}



#endif //VANGERS_PERFCOUNTER_H
