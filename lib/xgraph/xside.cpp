//
// Created by caiiiycuk on 25.06.2021.
//

#include "xbmp.h"
#include <algorithm>
#include <functional>
#include <utility>

extern int xgrScreenSizeX;
extern int xgrScreenSizeY;

extern int getCurRtoId();
extern int getCurIScreenId();
extern int getCurIScreenX();
extern int CurrentWorld;

namespace {
SDL_Texture *HDLeftSideTexture = nullptr;
SDL_Texture *HDRightSideTexture = nullptr;
std::pair<const char *, const char *> activeSides = std::make_pair<>(nullptr, nullptr);
int currentRto = 0;
int currentIScreenId = 0;
constexpr int contentWidth = 160;

std::pair<const char *, const char *> getSideNames() {
	int activeRtoId = getCurRtoId();
	int activeIScreenId = getCurIScreenId();
	int activeIScreenX = getCurIScreenX();

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
		if (activeIScreenX == 0) {
			return std::make_pair<>(
				"resource/actint/hd/side/main_menu_left.bmp",
				"resource/actint/hd/side/main_menu_right.bmp"
			);
		}

		return std::make_pair<>(
			"resource/actint/hd/side/menu_left.bmp", "resource/actint/hd/side/menu_right.bmp"
		);
	} else if (currentRto == 6 /*RTO_FIRST_ESCAVE_ID*/ || currentRto == 8 /*RTO_ESCAVE_ID*/) {
		switch (CurrentWorld) {
		case -1:
			return activeSides;
		case 0:
			return std::make_pair<>(
				"resource/actint/hd/side/fostral_left.bmp",
				"resource/actint/hd/side/fostral_right.bmp"
			);
		case 1:
			return std::make_pair<>(
				"resource/actint/hd/side/glorx_left.bmp", "resource/actint/hd/side/glorx_right.bmp"
			);
		case 2:
			return std::make_pair<>(
				"resource/actint/hd/side/necross_left.bmp",
				"resource/actint/hd/side/necross_right.bmp"
			);
		case 3:
			return std::make_pair<>(
				"resource/actint/hd/side/parafin_left.bmp",
				"resource/actint/hd/side/parafin_right.bmp"
			);
		default:;
		}
	} else if (currentRto == 7 /*RTO_FIRST_ESCAVE_OUT_ID*/ ||
			   currentRto == 9 /*RTO_ESCAVE_OUT_ID*/) {
		return activeSides;
	}

	return std::make_pair<>(nullptr, nullptr);
}
} // namespace

bool XGR_RenderSides(SDL_Renderer *renderer, int renderWidth) {
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

	int outWidth = (xgrScreenSizeX - renderWidth) / 2;
	SDL_FRect dst_rect{0, 0, 0, static_cast<float>(xgrScreenSizeY)};
	if (HDLeftSideTexture != nullptr) {
		if (!SDL_GetTextureSize(HDLeftSideTexture, &dst_rect.w, nullptr))
			return false;
		dst_rect.x = std::max<int>(0, outWidth - contentWidth);
		if (!SDL_RenderTexture(renderer, HDLeftSideTexture, NULL, &dst_rect))
			return false;
	}

	if (HDRightSideTexture != nullptr) {
		if (!SDL_GetTextureSize(HDRightSideTexture, &dst_rect.w, nullptr))
			return false;
		dst_rect.x = xgrScreenSizeX - outWidth - (dst_rect.w - contentWidth);
		if (!SDL_RenderTexture(renderer, HDRightSideTexture, NULL, &dst_rect))
			return false;
	}
	return true;
}
