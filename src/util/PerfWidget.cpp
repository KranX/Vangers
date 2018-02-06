//
// Created by nikita on 15.03.18.
//

#include "PerfWidget.h"


namespace util{
	void PerfWidget::draw_storage(const RingBuffer<NamedValues> &storage, int posX, int posY) {

		for (int i = 0; i <= 50; i += 10) {
			int width = i % 50 == 0 ? 8 : 4;
			xgr_screen->line(posX, posY - i, posX + width, posY - i, 224 + 15);
			xgr_screen->line(posX, posY + i, posX + width, posY + i, 224 + 15);
		}

		posX += 10;
		for(int i = 0; i < storage.get_size(); i++){
			int lastY = posY;
			for(auto const& kv: *storage.at(i)){
				auto counter_name = kv.first;
				auto value = kv.second;
				value = std::min<double>(value, 50.0);

				if(!counter_colors.count(counter_name)){
					counter_colors[counter_name] = PALETTE_COLORS[counter_colors.size() % PALETTE_COLORS.size()];
				}
				int color = counter_colors[counter_name];
				if(counter_name == "draw"){
					int y1 = posY;
					int y2 = posY + static_cast<int>(value);
					int x = posX + i;
					xgr_screen->line(x, y1, x, y2, color);
				} else {
					int y1 = lastY;
					int y2 = lastY - static_cast<int>(value);
					int x = posX + i;
					xgr_screen->line(x, y1, x, y2, color);
					lastY = y2;
				}
			}

		}
	}
}

