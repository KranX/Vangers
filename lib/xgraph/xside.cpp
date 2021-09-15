//
// Created by caiiiycuk on 25.06.2021.
//

#include <utility>
#include <functional>
#include "xbmp.h"

extern int xgrScreenSizeX;
extern int xgrScreenSizeY;

extern int getCurRtoId();
extern int getCurIScreenId();
extern int CurrentWorld;

namespace {
SDL_Texture *HDLeftSideTexture = nullptr;
SDL_Texture *HDRightSideTexture = nullptr;
std::pair<const char *, const char *> activeSides = std::make_pair<>(nullptr, nullptr);
int currentRto = 0;
int currentIScreenId = 0;

std::pair<const char *, const char *> getSideNames() {
	int activeRtoId = getCurRtoId();
	int activeIScreenId = getCurIScreenId();

	if (activeRtoId == 0) {
		return activeSides;
	}

	if ((currentRto == activeRtoId && currentIScreenId == activeIScreenId) ||
		 activeRtoId == 10 /*RTO_PALETTE_TRANSFORM_ID*/) {
		return activeSides;
	}

	currentRto = activeRtoId;
	currentIScreenId = activeIScreenId;

	if (currentRto == 5 /*RTO_MAIN_MENU_ID*/) {
		if (activeIScreenId == 20 /* Genesis */ ||
			activeIScreenId == 27 /* Main menu */ ||
			activeIScreenId == 41 /* Graphics */ ||
			activeIScreenId == 59 /* Sound */ ||
			activeIScreenId == 123 /* Join */ ||
			activeIScreenId == 128 /* Create Server */ ||
			activeIScreenId == 358 /* Network Game Type */ ||
			activeIScreenId == 467 /* Identification */ ||
			activeIScreenId == 508 /* Chat */ ||
			activeIScreenId == 528 /* Game Result */ ||
			activeIScreenId == 654 /* Player Setup */ ||
			activeIScreenId == 662 /* Controls */ ||
			activeIScreenId == 678 /* Internet */ ||
			activeIScreenId == 698 /* Hall of Fame */ ||
			activeIScreenId == 770 /* Set Server Port */ ||
			activeIScreenId == 788 /* Addons (Web Version) */ ||
			activeIScreenId == 1126 /* Credits (2013+ Page 1) */ ||
			activeIScreenId == 1134 /* Credits (2013+ Page 3) */ ||
			activeIScreenId == 1139 /* Credits (2002 - 2013) */ ||
			activeIScreenId == 1143 /* Credits (IM) */ ||
			activeIScreenId == 1148 /* Credits (Buka) */ ||
			activeIScreenId == 1153 /* Credits (KD-Lab Page 1) */ ||
			activeIScreenId == 1161 /* Credits (KD-Lab Page 2) */ ||
			activeIScreenId == 0) {
			return std::make_pair<>(
				"resource/actint/hd/side/main_menu_left.bmp",
				"resource/actint/hd/side/main_menu_right.bmp");
		}

		return std::make_pair<>(
			"resource/actint/hd/side/menu_left.bmp",
			"resource/actint/hd/side/menu_right.bmp");
	} else if (
		currentRto == 6 /*RTO_FIRST_ESCAVE_ID*/ ||
		currentRto == 8 /*RTO_ESCAVE_ID*/) {

		switch (CurrentWorld) {
		case -1:
			return activeSides;
		case 0:
			return std::make_pair<>(
				"resource/actint/hd/side/fostral_left.bmp",
				"resource/actint/hd/side/fostral_right.bmp");
		case 1:
			return std::make_pair<>(
				"resource/actint/hd/side/glorx_left.bmp",
				"resource/actint/hd/side/glorx_right.bmp");
		case 2:
			return std::make_pair<>(
				"resource/actint/hd/side/necross_left.bmp",
				"resource/actint/hd/side/necross_right.bmp");
		case 3:
			return std::make_pair<>(
				"resource/actint/hd/side/parafin_left.bmp",
				"resource/actint/hd/side/parafin_right.bmp");
		default:;
		}
	} else if (currentRto == 7 /*RTO_FIRST_ESCAVE_OUT_ID*/ ||
			   currentRto == 9 /*RTO_ESCAVE_OUT_ID*/) {
		return activeSides;
	}

	return std::make_pair<>(nullptr, nullptr);
}
}

void XGR_RenderSides(SDL_Renderer *renderer) {
	auto sideNames = getSideNames();

	if (sideNames.first != activeSides.first) {
		SDL_DestroyTexture(HDLeftSideTexture);
		HDLeftSideTexture =
			sideNames.first ? BMP_CreateTexture(sideNames.first, renderer) : nullptr;
		activeSides.first = sideNames.first;
	}

	if (sideNames.second != activeSides.second) {
		SDL_DestroyTexture(HDRightSideTexture);
		HDRightSideTexture =
			sideNames.second ? BMP_CreateTexture(sideNames.second, renderer) : nullptr;
		activeSides.second = sideNames.second;
	}

	SDL_Rect dst_rect{0, 0, 0, xgrScreenSizeY};
	if (HDLeftSideTexture != nullptr) {
		SDL_QueryTexture(HDLeftSideTexture, nullptr, nullptr, &dst_rect.w, nullptr);
		SDL_RenderCopy(renderer, HDLeftSideTexture, NULL, &dst_rect);
	}

	if (HDRightSideTexture != nullptr) {
		SDL_QueryTexture(HDRightSideTexture, nullptr, nullptr, &dst_rect.w, nullptr);
		dst_rect.x = xgrScreenSizeX - dst_rect.w;
		SDL_RenderCopy(renderer, HDRightSideTexture, NULL, &dst_rect);
	}
}
