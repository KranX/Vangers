#include "actint/aci_str.h"
#include "actint/mouse_prompt.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

// Only font measurement is stubbed. The UTF-8 adapter and XGR prompt lifecycle
// are the same production code used by the game.
int XGR_TextWidth(void *text, int, int) {
	return static_cast<int>(std::strlen(static_cast<const char *>(text))) * 8;
}

int XGR_TextHeight(void *, int, int) {
	return 16;
}

XGR_TextWidthHandler XGR_TextXFnc = nullptr;
XGR_TextHeightHandler XGR_TextYFnc = nullptr;

static void check(bool condition, const char *message) {
	if (!condition) {
		std::cerr << message << '\n';
		std::exit(1);
	}
}

static void check_text(XGR_MousePromptData &prompt, const std::string &expected) {
	// Read after the adapter/codec has returned, not just inside init(). This is
	// the missing lifetime check: set_text(utf8_to_cp866(...).c_str()) fails ASan.
	check(std::strcmp(prompt.textData, expected.c_str()) == 0, "Prompt text must remain alive");
	check(prompt.textSizeX == static_cast<int>(expected.size()) * 8, "Incorrect prompt width");
	check(prompt.textSizeY == 16, "Incorrect prompt height");
}

static void exercise(XGR_MousePromptData &prompt) {
	using vangers::actint::set_mouse_prompt;
	using vangers::settings::utf8_to_cp866;

	check(prompt.flags == 0 && !prompt.textData, "Both constructors must initialize ownership");
	check(!prompt.textSizeX && !prompt.textSizeY, "Both constructors must initialize text sizes");

	// A real long Russian label forces a heap allocation in the codec.
	const auto expected = utf8_to_cp866(aciSTR_PICKUP_ITEMS_OFF2);
	set_mouse_prompt(&prompt, aciSTR_PICKUP_ITEMS_OFF2);
	check_text(prompt, expected);
	check(prompt.flags & XGR_PROMPT_MEM_ALLOC, "XGR must own the converted bytes");

	// InitPrompt captures the field by assignment. Keep the very same field
	// type here: a borrowed char* would dangle after the following updates.
	decltype(XGR_MousePromptScreen::curText) displayed = prompt.textData;
	auto *unchanged = prompt.textData;
	set_mouse_prompt(&prompt, aciSTR_PICKUP_ITEMS_OFF2);
	check(prompt.textData == unchanged, "Unchanged hover text must not reallocate every frame");

	for (int i = 0; i < 1000; ++i) {
		// Short (SSO), heap-backed, unsupported Unicode, icons, and empty text.
		for (const auto *text : {"Ёж", aciSTR_PICKUP_ITEMS_OFF2, "ä 日本 😀 $\x1e\x1f", ""}) {
			set_mouse_prompt(&prompt, text);
			check_text(prompt, utf8_to_cp866(text));
		}
	}
	check(std::string(displayed) == expected, "Active prompt must own its displayed snapshot");

	set_mouse_prompt(&prompt, "Рюкзак");
	const auto backpack = utf8_to_cp866("Рюкзак");
	prompt.init_text(prompt.textData);
	check_text(prompt, backpack);
	prompt.init_text(prompt.textData + 1);
	check_text(prompt, backpack.substr(1));

	// Switching to a persistent legacy source must release owned storage, but
	// not free the borrowed stack buffer at the next update or destruction.
	char borrowed[] = "legacy prompt";
	prompt.set_text(borrowed);
	check(prompt.textData == borrowed, "Legacy set_text must retain borrowed semantics");
	check(!(prompt.flags & XGR_PROMPT_MEM_ALLOC), "Borrowed text must not be deleted");
	prompt.init_text(borrowed);
	check(prompt.textData != borrowed, "init_text must copy even a previously borrowed source");
	borrowed[0] = 'X';
	check_text(prompt, "legacy prompt");
	prompt.set_text(prompt.textData);
	check(prompt.flags & XGR_PROMPT_MEM_ALLOC, "Self-assignment must not lose ownership");

	// Exercise destructor with borrowed storage as well as owned storage below.
	static char persistent[] = "legacy";
	prompt.set_text(persistent);
}

int main() {
	XGR_MousePromptData default_prompt;
	exercise(default_prompt);
	XGR_MousePromptData positioned_prompt(10, 20, 30, 40, XGR_FONT0_8x8);
	exercise(positioned_prompt);
	check(positioned_prompt.check_xy(10, 20), "Constructor must preserve prompt bounds");
	check(!positioned_prompt.check_xy(40, 60), "Prompt bounds must remain half-open");
	check(positioned_prompt.font == XGR_FONT0_8x8, "Constructor must preserve font");

	// Also run init() with the callbacks used by actint, not just XGR defaults.
	XGR_TextXFnc = XGR_TextWidth;
	XGR_TextYFnc = XGR_TextHeight;
	vangers::actint::set_mouse_prompt(&default_prompt, "Подсказка");
	check_text(default_prompt, vangers::settings::utf8_to_cp866("Подсказка"));
	return 0;
}
