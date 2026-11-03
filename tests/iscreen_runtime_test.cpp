#include "global.h"
#include "iscreen/hfont.h"
#include "iscreen/i_mem.h"
#include "iscreen/ikeys.h"
#include "iscreen/iscreen.h"
#include "iscreen/iscreen_options.h"
#include "iscreen/ivmap.h"
#include "iscreen/relief_font.h"
#include "iscreen/settings_adapter.h"
#include "settings/settings.h"
#include "settings/text_encoding.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

extern iScreenDispatcher *iScrDisp;
extern iListElement **ObjTable;
extern char *BackupStr;
extern iScreenOption **iScrOpt;
extern int iFrameFlag;
extern int iNumFonts;
extern HFont **HFntTable;
extern iScreenFont **iScrFontTable;
extern uint iclip_mask_y;
void init_hfonts();
void free_hfonts();
void iPutStr2buf(int, int, int, int, int, unsigned char *, unsigned char *, int, int, int);
void scale_buf(int, int, int, unsigned char *, unsigned char *);
void smooth_buf(int, int, unsigned char *, int);

namespace {
void check(bool ok, const char *message) {
	if (!ok)
		throw std::runtime_error(message);
}

void font_fixture() {
	std::filesystem::create_directories("resource/iscreen/fonts");
	std::ofstream file("resource/iscreen/fonts/hfont00.fnh", std::ios::binary);
	file.write("HFNT 1.01", 9);
	const auto word = [&](int n) {
		const char bytes[]{char(n), char(n >> 8)};
		file.write(bytes, 2);
	};
	word(26);
	word(50);
	word(0);
	word(255);
	for (int cp = 0; cp < 256; ++cp) {
		word(26);
		word(50);
		const int flags = cp == 0 || cp == ' ' ? NULL_HCHAR : 0;
		const char bytes[]{char(flags), 0, 0, 0};
		file.write(bytes, 4);
		if (!flags) {
			std::array<char, 1300> heights;
			heights.fill(128);
			for (int y = 8; y < 44; ++y)
				for (int x = 4; x < 22; ++x)
					heights[y * 26 + x] = char(x < 8 ? 124 : 128 + (x - 7) * 2);
			file.write(heights.data(), heights.size());
		}
	}
	check(file.good(), "Cannot create height font fixture");
	file.close();
	for (const auto name : {"hfont01.fnh", "hfont02.fnh"})
		std::filesystem::copy_file(
			"resource/iscreen/fonts/hfont00.fnh",
			std::filesystem::path("resource/iscreen/fonts") / name,
			std::filesystem::copy_options::overwrite_existing
		);
}

// iscreen models live for the process in the game. Explicitly release the
// fixture's model and parser-owned globals so sanitizer tests can check leaks.
void free_dispatcher(bool parsed) {
	if (parsed) {
		auto *screen = static_cast<iScreen *>(iScrDisp->last);
		delete[] screen->t_objID;
		// Legacy iScreenElement is a non-virtual tagged base. Delete the known
		// fixture types explicitly; do not mask sanitizer type-mismatch checks.
		auto *object = static_cast<iScreenObject *>(screen->objList->last);
		while (object->ElementList->last) {
			auto *element = static_cast<iScreenElement *>(object->ElementList->last);
			object->remove_element(element);
			if (element->type == I_STRING_ELEM)
				delete static_cast<iStringElement *>(element);
			else
				delete static_cast<iS_StringElement *>(element);
		}
		delete screen;
		delete[] iScrDisp->t_scrID;
		delete[] ObjTable;
		ObjTable = nullptr;
		auto *id = static_cast<iObjID *>(iScrDisp->ScreenIDs->last);
		while (id) {
			auto *prev = static_cast<iObjID *>(id->prev);
			delete id;
			id = prev;
		}
		auto *page = static_cast<iTextData *>(iScrDisp->texts->last);
		while (page) {
			auto *prev = static_cast<iTextData *>(page->prev);
			delete page;
			page = prev;
		}
	}
	delete iScrDisp->ObjHeap;
	delete iScrDisp->drwObjHeap;
	delete iScrDisp->ScreenIDs;
	delete iScrDisp->ExtScreens;
	delete iScrDisp->texts;
	delete[] iScrDisp->curPal;
	delete[] iScrDisp->trgPal;
	delete[] BackupStr;
	BackupStr = nullptr;
	delete iScrDisp;
	iScrDisp = nullptr;
}

void parse(const char *in, const char *out, bool unicode) {
	font_fixture();
	std::array<char, 8 * 256> small_pixels{};
	iScreenFont small_font(small_pixels.data(), small_pixels.size());
	iScreenFont *small_fonts[]{&small_font};
	iScrFontTable = small_fonts;
	ParseScript(in, out, false);
	const auto expected = unicode ? std::string("Ёж 日本 😀") : std::string("Ёж");
	auto *screen = static_cast<iScreen *>(iScrDisp->last);
	auto *object = static_cast<iScreenObject *>(screen->objList->last);
	check(object && object->promptData == expected, "Prompt did not decode at the boundary");
	auto *small = static_cast<iS_StringElement *>(object->ElementList->last);
	auto *large = static_cast<iStringElement *>(small->prev);
	check(large && large->string == expected, "STRING lost Unicode in source/SCB");
	check(small->string == expected, "S_STRING lost Unicode in source/SCB");
	int text_count = 0;
	for (auto *e = large->prev; e; e = e->prev) {
		auto *element = static_cast<iScreenElement *>(e);
		const char *text = element->type == I_STRING_ELEM
							   ? static_cast<iStringElement *>(element)->string
							   : static_cast<iS_StringElement *>(element)->string;
		check(text == expected, "text/s_text resource decoded incorrectly");
		++text_count;
	}
	check(text_count == 4, "Static text loaders lost final lines or added blank elements");
	auto *page = static_cast<iTextData *>(iScrDisp->texts->last);
	check(page && page->objName == expected, "Text title lost Unicode");
	page->load();
	check(page->numLines == 3, "Page must preserve empty and unterminated lines");
	char line[iS_STR_BYTES];
	page->read_str(line, iS_STR_LEN - 1);
	check(line == expected, "Page line did not decode exactly once");
	page->read_str(line, iS_STR_LEN - 1);
	check(!*line, "Blank line was lost");
	page->read_str(line, iS_STR_LEN - 1);
	check(line == expected, "Last line was lost");
	page->free();
	page->load(); // Reload must not leak old arrays.
	init_hfonts();
	std::array<iScreenOption *, iMAX_OPTION_ID> options{};
	iScrOpt = options.data();
	iScreenOption option(iSTRING, 0, "screen", "large");
	options[iPLAYER_NAME2] = &option;
	const char *legacy_view = iGetOptionValueCHR(iPLAYER_NAME2);
	iSetOptionText(iPLAYER_NAME2, "Ёж日本😀");
	check(
		std::string(iGetOptionText(iPLAYER_NAME2)) == "Ёж日本😀",
		"Canonical option lost unsupported scalars"
	);
	check(
		legacy_view == iGetOptionValueCHR(iPLAYER_NAME2) &&
			std::string(legacy_view) == vangers::settings::utf8_to_cp866("Ёж日本😀"),
		"Stable legacy network view was not refreshed"
	);
	vangers::settings::capture_settings_from_interface();
	check(
		vangers::settings::settings_manager().get().network.player_name == "Ёж日本😀",
		"Saving settings round-tripped through CP866"
	);
	large->init_string("Имя日");
	option.update();
	check(
		std::string(legacy_view) == vangers::settings::utf8_to_cp866("Имя日"),
		"Committed edit did not refresh legacy view"
	);
	iSetOptionValueCHR(iPLAYER_NAME2, vangers::settings::utf8_to_cp866("Ёж").c_str());
	check(
		std::string(iGetOptionText(iPLAYER_NAME2)) == "Ёж",
		"Legacy option setter did not decode once"
	);
	delete option.pLinks;
	iScrOpt = nullptr;
	free_hfonts();
	free_dispatcher(true);
	iScrFontTable = nullptr;
	delete[] small_font.LeftOffs;
	delete[] small_font.RightOffs;
}

void input() {
	std::filesystem::create_directories("resource/actint/hd");
	auto *background = SDL_CreateSurface(8, 8, SDL_PIXELFORMAT_RGB24);
	check(
		background && SDL_SaveBMP(background, "resource/actint/hd/hd_background.bmp"),
		"Cannot create background fixture"
	);
	SDL_DestroySurface(background);
	SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
	XGR_Init(0);
	iScrDisp = new iScreenDispatcher;
	std::vector<char> heap(1 << 20);
	iMemHeap = heap.data();
	iHeapSize = heap.size();
	iHeapOffs = 0;
	KeyBuf = new KeyBuffer;
	iFrameFlag = 0;
	for (bool small : {false, true}) {
		iScreenObject object;
		iScreenElement *element =
			small ? static_cast<iScreenElement *>(new iS_StringElement) : new iStringElement;
		object.add_element(element);
		char *value = small ? static_cast<iS_StringElement *>(element)->string
							: static_cast<iStringElement *>(element)->string;
		std::strcpy(value, "Ё");
		iScrDisp->cur_max_input = 6;
		iScrDisp->init_input_string(element);
		const auto text = [&](const char *s) {
			SDL_Event ev{};
			ev.type = SDL_EVENT_TEXT_INPUT;
			ev.text.text = s;
			KeyBuf->put(&ev, CUR_KEY_PRESSED);
			iScrDisp->input_string_quant();
		};
		const auto key = [&](SDL_Keycode code) {
			SDL_Event ev{};
			ev.type = SDL_EVENT_KEY_DOWN;
			ev.key.key = code;
			KeyBuf->put(&ev, CUR_KEY_PRESSED);
			iScrDisp->input_string_quant();
		};
		text("ж日😀abc");
		check(std::string(value) == "Ёж日😀a_", "SDL event must append whole scalars up to limit");
		key(SDLK_BACKSPACE);
		key(SDLK_LEFT);
		check(std::string(value) == "Ёж日_", "Backspace/left split a scalar");
		key(SDLK_ESCAPE);
		check(std::string(value) == "Ё", "Escape did not restore original UTF-8");
		iScrDisp->init_input_string(element);
		text("ж日😀");
		key(SDLK_RETURN);
		check(std::string(value) == "Ёж日😀", "Return lost the last scalar/cursor");
		check(!(iScrDisp->flags & SD_INPUT_STRING), "Return did not finish input");
		object.flags &= ~OBJ_REDRAW;
		iScrDisp->drwObjHeap->init_list();
		iHeapOffs = 0;
		object.remove_element(element);
		if (small)
			delete static_cast<iS_StringElement *>(element);
		else
			delete static_cast<iStringElement *>(element);
	}
	delete KeyBuf;
	KeyBuf = nullptr;
	iMemHeap = nullptr;
	iHeapSize = 0;
	free_dispatcher(false);
	XGR_Finit();
}

void relief(bool ttf) {
	font_fixture();
	iNumFonts = 3;
	init_hfonts();
	vangers::settings::settings_manager().get_mutable().video.ttf_text = ttf;
	std::vector<char> heap(1 << 20);
	iMemHeap = heap.data();
	iHeapSize = heap.size();
	iHeapOffs = 0;
	const int w = 240, h = 60;
	std::vector<unsigned char> heights(w * h, 128), scaled(w * h), blurred;
	std::string label = "Есть Ёж";
	iPutStr2buf(
		0, 0, 0, w, h, reinterpret_cast<unsigned char *>(label.data()), heights.data(), 0, 256, 5
	);
	check(
		std::any_of(heights.begin(), heights.end(), [](auto n) { return n > 128; }),
		"Relief has no raised height"
	);
	check(
		std::any_of(heights.begin(), heights.end(), [](auto n) { return n < 128; }),
		"Relief lost negative source height"
	);
	for (int scale : {256, 128, 0, -128, -256}) {
		scale_buf(w, h, scale, heights.data(), scaled.data());
		for (size_t i = 0; i < scaled.size(); ++i)
			check(
				scaled[i] == 128 + ((int(heights[i]) - 128) * scale >> 8),
				"Signed click depth changed"
			);
		blurred = scaled;
		smooth_buf(w, h, blurred.data(), 3);
		if (scale == 0)
			check(
				std::all_of(blurred.begin(), blurred.end(), [](auto n) { return n == 128; }),
				"Zero click scale left relief behind"
			);
		else
			check(blurred != scaled, "Click smoothing did not operate on heights");
	}
	iScreenObject object;
	object.HeightScaleDelta = -64;
	for (int i = 0; i < 8; ++i)
		object.ScaleQuant();
	check(object.curHeightScale == -256, "Click quantization cannot invert relief");
	object.ScaleQuant();
	check(object.curHeightScale == -256, "Negative scale clamp changed");
	object.HeightScaleDelta = 64;
	for (int i = 0; i < 8; ++i)
		object.ScaleQuant();
	check(object.curHeightScale == 256, "Release did not restore relief");
	if (ttf) {
		vangers::iscreen::ReliefFont face(*HFntTable[0], vangers::actint::FontFamily::Display);
		vangers::actint::TtfFont outline(vangers::actint::FontFamily::Display);
		vangers::actint::FontStyle style;
		style.cell_width = 26;
		style.cell_height = 50;
		style.spacing = style.line_spacing = 0;
		style.highlight_numbers = false;
		// U+E000 exists in the TTF but not CP866; it must not borrow '?' metrics
		// or its signed source profile. This also covers future added alphabets.
		const auto &new_glyph = face.glyph(0xe000);
		check(
			new_glyph.advance == outline.layout("\xee\x80\x80", style).width,
			"New Unicode glyph inherited legacy '?' advance"
		);
		const auto new_heights = new_glyph.heights(new_glyph.bounds.w, new_glyph.bounds.h);
		check(
			std::any_of(new_heights.begin(), new_heights.end(), [](auto n) { return n > 128; }) &&
				std::none_of(
					new_heights.begin(), new_heights.end(), [](auto n) { return n < 128; }
				),
			"New Unicode glyph inherited legacy '?' relief instead of a generic profile"
		);
		std::vector<unsigned char> menu(w * h, 128), icon(w * h, 128), bitmap(w * h, 128);
		std::string capitals = "КПHj";
		iPutStr2buf(
			5,
			5,
			1,
			w,
			h,
			reinterpret_cast<unsigned char *>(capitals.data()),
			menu.data(),
			0,
			256,
			5
		);
		for (int y = 0; y < h; ++y)
			for (int x = 0; x < w; ++x)
				if (y < 5 || y >= 55 || x < 5)
					check(
						menu[size_t(y) * w + x] == 128,
						"Menu outline escaped its restored height cell"
					);
		iPutStr2buf(
			5, 5, 2, w, h, reinterpret_cast<unsigned char *>(label.data()), icon.data(), 0, 256, 5
		);
		vangers::settings::settings_manager().get_mutable().video.ttf_text = false;
		iPutStr2buf(
			5, 5, 2, w, h, reinterpret_cast<unsigned char *>(label.data()), bitmap.data(), 0, 256, 5
		);
		check(icon == bitmap, "Height icon cells must remain byte-exact in TTF mode");
		vangers::settings::settings_manager().get_mutable().video.ttf_text = true;
	}
	// Replay the actual button command schedule (buttons.inc), not a new tween.
	iScrDisp = new iScreenDispatcher;
	object.curHeightScale = 256;
	object.SmoothLevel = 0;
	iScreenEvent event;
	event.cur_time = 0;
	event.time = 14;
	const auto command = [&](int type, int start, int value) {
		auto *c = new iScreenEventCommand;
		c->EvType = type;
		c->obj = &object;
		c->start_time = start;
		c->time = 7;
		c->value = value;
		event.CommSeq->connect(c);
	};
	command(EV_SCALE_OBJECT, 0, 0);
	command(EV_SMOOTH_OBJECT, 0, 4);
	command(EV_SCALE_OBJECT, 7, 256);
	command(EV_SMOOTH_OBJECT, 7, 0);
	const std::array<int, 14> depth{220, 184, 148, 112, 76, 40, 4, 40, 76, 112, 148, 184, 220, 256};
	const std::array<int, 14> smooth{1, 2, 3, 4, 4, 4, 4, 3, 2, 1, 0, 0, 0, 0};
	for (int frame = 0; frame < 14; ++frame) {
		iScrDisp->ProcessEvent(&event);
		check(
			object.curHeightScale == depth[frame],
			"Original seven-tick press/release timing changed"
		);
		check(object.SmoothLevel == smooth[frame], "Original click blur timing changed");
	}
	free_dispatcher(false);
	free_hfonts();
	iMemHeap = nullptr;
	iHeapSize = 0;
}
} // namespace

extern "C" bool SDLCALL VangersTest_ShowSimpleMessageBox(
	SDL_MessageBoxFlags,
	const char *,
	const char *message,
	SDL_Window *
) {
	std::cerr << "ERROR_DIALOG: " << message << '\n';
	return true;
}
int main(int argc, char **argv) {
	try {
		if (argc == 5 && std::string(argv[1]) == "--parse")
			parse(argv[2], argv[3], std::string(argv[4]) == "unicode");
		else if (argc == 3 && std::string(argv[1]) == "--relief")
			relief(std::string(argv[2]) == "ttf");
		else if (argc == 2 && std::string(argv[1]) == "--input")
			input();
		else
			throw std::runtime_error("Unknown invocation");
		return 0;
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return 2;
	}
}
