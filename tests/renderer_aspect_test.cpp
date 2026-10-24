#include "xgraph.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>

namespace {

bool check(bool condition, const char *message) {
	if (!condition)
		std::cerr << "FAIL: " << message << " (SDL: " << SDL_GetError() << ")\n";
	return condition;
}

struct Size {
	int width;
	int height;
};

bool check_mouse(SDL_Renderer *renderer, SDL_Window *window, float x, float y) {
	SDL_Event event{};
	event.type = SDL_EVENT_MOUSE_MOTION;
	event.motion.windowID = SDL_GetWindowID(window);
	if (!check(
			SDL_RenderCoordinatesToWindow(renderer, x, y, &event.motion.x, &event.motion.y),
			"cannot map logical point to window"
		) ||
		!check(SDL_ConvertEventToRenderCoordinates(renderer, &event), "cannot convert mouse event"))
		return false;
	return check(
		std::fabs(event.motion.x - x) < 0.01f && std::fabs(event.motion.y - y) < 0.01f,
		"letterbox mouse coordinates do not round-trip"
	);
}

bool test_resize() {
	const std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window(
		SDL_CreateWindow(
			"Renderer aspect test", 800, 600, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE
		),
		SDL_DestroyWindow
	);
	if (!check(window != nullptr, "cannot create dummy window"))
		return false;
	const std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer(
		SDL_CreateRenderer(window.get(), "software"), SDL_DestroyRenderer
	);
	if (!check(renderer != nullptr, "cannot create software renderer"))
		return false;

	const Size canvases[] = {{800, 600}, {1280, 720}, {1280, 960}, {1280, 600}};
	const Size windows[] = {
		{800, 600}, {1280, 720}, {1000, 1000}, {1600, 600}, {600, 1000}, {1920, 1080}};
	for (const auto canvas : canvases) {
		for (const auto output : windows) {
			if (!check(
					SDL_SetWindowSize(window.get(), output.width, output.height),
					"cannot resize window"
				))
				return false;
			SDL_PumpEvents();
			int width = 0, height = 0;
			if (!check(
					SDL_GetRenderOutputSize(renderer.get(), &width, &height),
					"cannot get output size"
				) ||
				!check(
					width == output.width && height == output.height, "dummy output did not resize"
				))
				return false;

			// The full-output backdrop must not leave the scene in stretch mode.
			if (!check(
					SDL_SetRenderLogicalPresentation(
						renderer.get(), width, height, SDL_LOGICAL_PRESENTATION_STRETCH
					),
					"cannot set backdrop presentation"
				) ||
				!check(
					SDL_SetRenderLogicalPresentation(
						renderer.get(), canvas.width, canvas.height, XGR_Screen::ScenePresentation
					),
					"cannot set scene presentation"
				))
				return false;
			SDL_FRect rect{};
			if (!check(
					SDL_GetRenderLogicalPresentationRect(renderer.get(), &rect),
					"cannot get presentation rectangle"
				))
				return false;
			const float scale = std::min(
				static_cast<float>(width) / canvas.width, static_cast<float>(height) / canvas.height
			);
			// SDL rounds the destination size to output pixels, not logical pixels.
			if (!check(
					std::fabs(rect.w - canvas.width * scale) <= 1.01f &&
						std::fabs(rect.h - canvas.height * scale) <= 1.01f,
					"scene is stretched or cropped instead of aspect-fitted"
				) ||
				!check(
					std::fabs(rect.x - (width - rect.w) / 2.0f) < 0.01f &&
						std::fabs(rect.y - (height - rect.h) / 2.0f) < 0.01f,
					"scene is not centered"
				) ||
				!check_mouse(renderer.get(), window.get(), 0, 0) ||
				!check_mouse(
					renderer.get(), window.get(), canvas.width / 2.0f, canvas.height / 2.0f
				) ||
				!check_mouse(
					renderer.get(), window.get(), canvas.width - 1.0f, canvas.height - 1.0f
				))
				return false;
		}
	}
	return true;
}

} // namespace

int main() {
	SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
	if (!check(SDL_Init(SDL_INIT_VIDEO), "cannot initialize dummy video"))
		return 1;
	const bool passed = test_resize();
	SDL_Quit();
	return passed ? 0 : 1;
}
