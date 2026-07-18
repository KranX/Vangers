#include "ikeys.h"

#include <cstdio>
#include <cstring>
#include <iostream>

void set_key_handlers(void (*)(SDL_Event *), void (*)(SDL_Event *)) {}
bool XGamepadOwnsEvent(const SDL_Event &event) {
	return event.gbutton.which == 42;
}

namespace {

bool check(bool condition, const char *message) {
	if (!condition)
		std::cerr << "FAIL: " << message << '\n';
	return condition;
}

bool test_text_input_ownership() {
	KeyBuffer buffer;
	char source[] = "original text";
	SDL_Event event{};
	event.type = SDL_EVENT_TEXT_INPUT;
	event.text.text = source;
	buffer.put(&event, CUR_KEY_PRESSED);

	std::strcpy(source, "overwritten");
	SDL_Event *stored = buffer.get();
	return check(stored != nullptr, "stored text event is missing") &&
		   check(stored->type == SDL_EVENT_TEXT_INPUT, "stored event type changed") &&
		   check(
			   std::strcmp(stored->text.text, "original text") == 0,
			   "text input still points at SDL's transient event storage"
		   );
}

bool test_ring_slots_own_independent_text() {
	KeyBuffer buffer;
	char source[32];
	for (int i = 0; i < 32; ++i) {
		std::snprintf(source, sizeof(source), "event-%d", i);
		SDL_Event event{};
		event.type = SDL_EVENT_TEXT_INPUT;
		event.text.text = source;
		buffer.put(&event, CUR_KEY_PRESSED);
	}

	for (int i = 0; i < 32; ++i) {
		std::snprintf(source, sizeof(source), "event-%d", i);
		SDL_Event *stored = buffer.get();
		if (!check(stored != nullptr, "ring buffer lost a text event") ||
			!check(std::strcmp(stored->text.text, source) == 0, "ring buffer text slots alias"))
			return false;
	}
	return check(buffer.get() == nullptr, "empty ring buffer returned an event");
}

bool test_only_active_gamepad_events_are_buffered() {
	KeyBuffer buffer;
	KeyBuf = &buffer;

	SDL_Event event{};
	event.type = SDL_EVENT_GAMEPAD_BUTTON_DOWN;
	event.gbutton.which = 7;
	key(&event);
	if (!check(buffer.size == 0, "inactive gamepad event was buffered"))
		return false;

	event.gbutton.which = 42;
	key(&event);
	if (!check(buffer.size == 1, "active gamepad event was not buffered") ||
		!check(buffer.get()->gbutton.which == 42, "buffered gamepad event changed"))
		return false;

	event.type = SDL_EVENT_GAMEPAD_BUTTON_UP;
	key(&event);
	return check(buffer.size == 0, "gamepad button release was buffered as a binding press");
}

} // namespace

int main() {
	return test_text_input_ownership() && test_ring_slots_own_independent_text() &&
				   test_only_active_gamepad_events_are_buffered()
			   ? 0
			   : 1;
}
