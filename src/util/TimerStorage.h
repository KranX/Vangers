//
// Created by nikita on 15.03.18.
//

#ifndef VANGERS_PERFMONITOR_H
#define VANGERS_PERFMONITOR_H

#include <map>
#include <string>
#include "Timer.h"

namespace util{
	typedef std::shared_ptr<std::map<std::string, double>> NamedValues;

	class TimerStorage {
		std::map<std::string, util::Timer> timers;
		NamedValues current;
	public:
		TimerStorage(){
			next();
		}

		NamedValues next(){
			auto old_val = current;
			current = std::make_shared<std::map<std::string, double>>();
			return old_val;
		}

		void event_start(const std::string& counter_name){
			timers[counter_name].event_start();
		}

		void event_end(const std::string& counter_name){
			auto duration = timers[counter_name].event_end();
			(*current)[counter_name] = duration;
		}
	};

}


#endif //VANGERS_PERFMONITOR_H
