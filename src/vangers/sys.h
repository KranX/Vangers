//
// Created by caiiiycuk on 16.05.2022.
//

#ifndef VANGERS_SYS_H
#define VANGERS_SYS_H

#include <vector>
#include <functional>
#include "event.h"

namespace vangers {
	struct OptionQuant {
		int optionValue;
		const int optionId;
	};

    struct JoystickQuant {
		bool active;
		int traction;
		int rudder;
		bool helicopterStrife;

		const int tractionIncrement;
		const int tractionDecrement;
		const int tractionMax;
		const int rudderStep;
		const int rudderMax;
		const float unitAngle;
    };

    typedef std::function<void(OptionQuant&)> OptionQuantFunction;
	typedef std::function<void(JoystickQuant&)> JoystickQuantFunction;

class Sys {
    public:
		Sys & operator=(const Sys&) = delete;
		Sys(const Sys&) = delete;

		int rendererWidth();
        int rendererHeight();

        OptionQuantFunction& getOptionQuantFunction();
        void setOptionQuantFunction(const OptionQuantFunction& fn);

		JoystickQuantFunction& getJoystickQuantFunction();
		void setJoystickQuantFunction(const JoystickQuantFunction& fn);

	void postEvent(const Event& event);
        size_t addEventListener(const std::function<void(Event)>& listener);
        void removeEventListener(size_t id);
    private:
        Sys() = default;
        friend Sys &sys();

		OptionQuantFunction optionkQuantFunction;
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
