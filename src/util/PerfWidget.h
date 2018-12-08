//
// Created by nikita on 15.03.18.
//

#ifndef VANGERS_PERFWIDGET_H
#define VANGERS_PERFWIDGET_H


#include "../../lib/xgraph/global.h"
#include "TimerStorage.h"

namespace util{
	const std::vector<int> PALETTE_COLORS = {
//			109, 119,
			159, 183, 220, 253
	};

	class PerfWidget {
	private:
		XGR_Screen* xgr_screen;
		std::map<std::string, int> counter_colors;
	public:
		explicit PerfWidget(XGR_Screen *xgr_screen): xgr_screen(xgr_screen) {};
		void draw_storage(const RingBuffer<NamedValues>& storage, int posX, int posY);
		std::map<std::string, int> get_colors(){ return counter_colors; };
	};
}



#endif //VANGERS_PERFWIDGET_H
