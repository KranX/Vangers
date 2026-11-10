#include "global.h"

#include "actint/actint.h"
#include "iscreen/controls.h"
#include "iscreen/hfont.h"
#include "iscreen/i_mem.h"
#include "iscreen/ikeys.h"
#include "iscreen/iscreen.h"
#include "iscreen/iscreen_options.h"
#include "iscreen/ivmap.h"
#include "iscreen/relief_font.h"
#include "iscreen/settings_adapter.h"
#include "localization/description.h"
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
extern iStringElement **iControlsStr;
void iInitStrings();
void iDeleteControl(int, int);
void iYSetup();
void iRenderPrepare();
void init_hfonts();
void free_hfonts();
void aLoadFonts32();
extern aciFont **aScrFonts32;
void iPutStr2buf(int, int, int, int, int, unsigned char *, unsigned char *, int, int, int);
void scale_buf(int, int, int, const unsigned char *, unsigned char *);
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

void parse(const char *in, const char *out, bool unicode, bool flow = false, bool keyed = false) {
	font_fixture();
	std::array<char, 8 * 256> small_pixels{};
	iScreenFont small_font(small_pixels.data(), small_pixels.size());
	iScreenFont *small_fonts[]{&small_font};
	iScrFontTable = small_fonts;
	ParseScript(in, out, false);
	const auto expected = keyed		? std::string("NEW GAME")
						  : unicode ? std::string("Ёж 日本 😀")
									: std::string("Ёж");
	auto *screen = static_cast<iScreen *>(iScrDisp->last);
	auto *object = static_cast<iScreenObject *>(screen->objList->last);
	check(object && object->promptData == expected, "Prompt did not decode at the boundary");
	auto *small_element = static_cast<iS_StringElement *>(object->ElementList->last);
	auto *large = static_cast<iStringElement *>(small_element->prev);
	check(large && large->string == expected, "STRING lost Unicode in source/SCB");
	check(small_element->string == expected, "S_STRING lost Unicode in source/SCB");
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
	check(page->flow_width == (flow ? 240 : 0), "text_flow policy lost in source/SCB");
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

void flow(bool ttf) {
	font_fixture();
	vangers::settings::settings_manager().get_mutable().video.ttf_text = ttf;
	init_hfonts();
	std::filesystem::create_directories("resource/actint/fonts");
	for (int f = 0; f < AS_NUM_FONTS_32; ++f) {
		std::ofstream out(
			"resource/actint/fonts/cfont0" + std::to_string(f) + ".bml", std::ios::binary
		);
		const char header[]{4, 0, 18, 0, 0, 1};
		out.write(header, sizeof header);
		for (int cp = 0; cp < 256; ++cp) {
			std::array<char, 72> pixels{};
			if (cp != ' ')
				pixels[16] = 31;
			out.write(pixels.data(), pixels.size());
		}
	}
	aLoadFonts32();
	iScrDisp = new iScreenDispatcher;
	for (bool small_font : {false, true}) {
		iScreenObject object;
		object.SizeX = 240;
		object.SizeY = 90;
		for (int i = 0; i < 3; ++i) {
			iScreenElement *el = small_font ? static_cast<iScreenElement *>(new iS_StringElement)
											: new iStringElement;
			el->lY = i * 40;
			object.add_element(el);
		}
		iTextData text;
		text.flow_width = 240;
		text.source_text = "Первый абзац. A long paragraph without authored manual line breaks. "
						   "More words to ensure multiple pages.\n\nLast paragraph.\fEND";
		text.layout_for(&object);
		check(
			text.numLines > 3 && text.source_text.find("\f") != std::string::npos,
			"Layout discarded source/page breaks"
		);
		const auto *stable = text.mem_heap;
		text.layout_for(&object);
		check(text.mem_heap == stable, "Unchanged history was laid out again");
		text.reset();
		std::vector<std::string> shown;
		while (!text.eof()) {
			const int start = text.curLine;
			text.copy(&object);
			auto *el = (iScreenElement *)object.ElementList->first;
			for (int i = start; i < text.curLine; ++i, el = (iScreenElement *)el->next) {
				const char *value =
					small_font ? ((iS_StringElement *)el)->string : ((iStringElement *)el)->string;
				check(
					std::string(value) == text.data[i],
					"Flow page was clipped by old scalar input limit"
				);
				check(el->SizeX <= object.SizeX, "History row exceeds its box");
				shown.emplace_back(value);
			}
		}
		check(
			shown.size() == size_t(text.numLines) && shown.back() == "END",
			"History lost its last page"
		);
		while (object.ElementList->last) {
			auto *el = (iScreenElement *)object.ElementList->last;
			object.remove_element(el);
			if (small_font)
				delete (iS_StringElement *)el;
			else
				delete (iStringElement *)el;
		}
	}
	if (!ttf) {
		iScreenObject narrow;
		narrow.SizeX = 700;
		narrow.SizeY = 18;
		auto *row = new iS_StringElement;
		row->space = 0;
		narrow.add_element(row);
		iTextData text;
		text.source_text =
			std::string(1200, 'i'); // No flow flag is required for measured wrapping.
		text.copy(&narrow);
		check(std::strlen(row->string) > iS_STR_LEN, "Measured body still has the old scalar cap");
		std::string restored = row->string;
		while (!text.eof()) {
			text.copy(&narrow);
			restored += row->string;
		}
		check(restored == text.source_text, "Narrow unbroken text lost data across pages");
		narrow.remove_element(row);
		delete row;
	}

	// Display rows no longer obey editable-input caps. Cloning the element must
	// preserve the same owned capacity, including multi-byte UTF-8 text.
	std::string long_line;
	for (int i = 0; i < 400; ++i)
		long_line += "Ё日😀";
	iStringElement large_source, large_copy;
	large_source.init_text_line(long_line);
	large_source.copy(&large_copy);
	check(large_copy.string == long_line, "STRING clone truncated a dynamic display row");
	iS_StringElement small_source, small_copy;
	small_source.init_text_line(long_line);
	small_source.copy(&small_copy);
	check(small_copy.string == long_line, "S_STRING clone truncated a dynamic display row");

	// Exercise the production previous-page entry point, including exact full pages.
	iScreen screen;
	iScreenObject object;
	screen.curObj = &object;
	iObjID id;
	id.init("Text00", 0, &object);
	screen.ObjIDs->connect(&id);
	for (int i = 0; i < 2; ++i)
		object.add_element(new iStringElement);
	iTextData text;
	iScrDisp->curText = &text;
	for (int count : {3, 4, 5, 6}) {
		std::vector<std::string> lines;
		for (int i = 0; i < count; ++i)
			lines.push_back(std::to_string(i));
		text.store_lines(lines);
		text.layout_ready = true;
		text.layout_rows = 2;
		text.curLine = count;
		text.flags |= iTEXT_EOF;
		check(iScrDisp->copy_text_prev(&screen, 0) == 0, "Previous page rejected");
		const int expected = ((count - 1) / 2 - 1) * 2;
		check(
			std::string(((iStringElement *)object.ElementList->first)->string) ==
				std::to_string(expected),
			"Previous page skips or repeats rows"
		);
	}
	setLang(ENGLISH);
	text.ID = 1;
	iTextData following;
	following.ID = 2;
	following.layout_ready = true;
	following.layout_rows = 2;
	iScrDisp->texts->connect(&text);
	iScrDisp->texts->connect(&following);
	iScrDisp->curScr = &screen;
	// Borrowing a legacy layout is independent of the selected text language.
	setLang(RUSSIAN);
	iScrDisp->curText = nullptr;
	iScrDisp->prev_text();
	check(!iScrDisp->curText, "Previous history selected a document before initialization");
	iScrDisp->next_text();
	check(iScrDisp->curText == &text, "English fallback history depends on Language");
	following.ID = 42; // A package need not use adjacent legacy numeric IDs.
	iScrDisp->next_text();
	check(iScrDisp->curText == &following, "History skipped a nonconsecutive document ID");
	iScrDisp->next_text();
	check(iScrDisp->curText == &text, "History failed to wrap within the loaded package");
	iScrDisp->prev_text();
	check(iScrDisp->curText == &text, "Previous history wrapped before the first document");
	for (int count : {0, 1, 4, 5}) {
		std::vector<std::string> lines;
		for (int i = 0; i < count; ++i)
			lines.push_back(std::to_string(i));
		text.store_lines(lines);
		text.reset();
		following.curLine = 1;
		iScrDisp->curText = &following;
		iScreenEvent event;
		iScrDisp->ActiveEv = &event;
		check(
			iScrDisp->copy_text_prev(&screen, 0) == 1 && iScrDisp->curText == &text,
			"Previous document selection failed"
		);
		// The existing script schedules this second invocation after the transition.
		check(iScrDisp->copy_text_prev(&screen, 0) == 0, "Previous document last page failed");
		const auto value = std::string(((iStringElement *)object.ElementList->first)->string);
		check(
			count ? value == std::to_string((count - 1) / 2 * 2) : value == " ",
			"Previous document last page is off by one"
		);
	}
	iScrDisp->texts->dconnect(&text);
	iScrDisp->texts->dconnect(&following);
	iScrDisp->curScr = nullptr;

	screen.ObjIDs->dconnect(&id);
	iScrDisp->curText = nullptr;
	while (object.ElementList->last) {
		auto *el = (iStringElement *)object.ElementList->last;
		object.remove_element(el);
		delete el;
	}
	free_dispatcher(false);
	free_hfonts();
	for (int f = 0; f < AS_NUM_FONTS_32; ++f)
		delete aScrFonts32[f];
	delete[] aScrFonts32;
	aScrFonts32 = nullptr;
}

void init_renderer_fixture() {
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
}

void input() {
	init_renderer_fixture();
	// Catalog labels are already UTF-8, including the control-removal fallback.
	setLang(RUSSIAN);
	iInitStrings();
	check(std::string(iGetKeyNameText(0)) == "НЕТ", "Empty control lost the selected locale");
	{
		iScreenObject object;
		iStringElement element;
		object.add_element(&element);
		element.init_string("old binding");
		std::array<iStringElement *, iKEY_MAX_ID * iKEY_OBJECT_SIZE> controls{};
		controls[iKEY_MOVE_FORWARD * iKEY_OBJECT_SIZE] = &element;
		iControlsStr = controls.data();
		iDeleteControl(0, -1); // With no control model, the fixture's code is unbound.
		check(std::string(element.string) == "НЕТ", "Control removal decoded UTF-8 as CP866");
		iInitControlObjects();
		check(std::string(element.string) == "НЕТ", "Control refresh corrupted a catalog label");
		iControlsStr = nullptr;
		object.remove_element(&element);
	}
	iScrDisp = new iScreenDispatcher;
	std::vector<char> heap(1 << 20);
	iMemHeap = heap.data();
	iHeapSize = heap.size();
	iHeapOffs = 0;
	KeyBuf = new KeyBuffer;
	iFrameFlag = 0;
	for (bool small_font : {false, true}) {
		iScreenObject object;
		iScreenElement *element =
			small_font ? static_cast<iScreenElement *>(new iS_StringElement) : new iStringElement;
		object.add_element(element);
		char *value = small_font ? static_cast<iS_StringElement *>(element)->string
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
		if (small_font)
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

void shadow_damage() {
	init_renderer_fixture();
	XGR_Obj.set_resolution(I_RES_X, I_RES_Y);
	auto *screen = XGR_Obj.get_active_render_buffer();
	const size_t screen_size = size_t(XGR_MAXX) * XGR_MAXY;
	std::vector<unsigned char> heights(H_SIZE * 2), colours(H_SIZE);
	auto *height_row = heights.data();
	auto *colour_row = colours.data();
	ivrtMap map;
	map.lineT = &height_row;
	map.lineTcolor = &colour_row;
	ivMap = &map;
	iYSetup(); // One map row in this isolated fixture.
	iRenderPrepare();
	for (const auto [x, offset] :
		{std::pair{100, 0}, std::pair{4, 0}, std::pair{4, int(H_SIZE) - I_RES_X}}) {
		std::fill(heights.begin(), heights.end(), 0);
		std::fill_n(heights.begin(), H_SIZE, 100);
		heights[x] = heights[x + 1] = 230;
		std::fill(colours.begin(), colours.end(), 7);
		std::fill(screen, screen + screen_size, 99);
		iScreenOffs = offset;
		iregRender(x, 0, x + 10, 0);
		check(
			std::all_of(screen, screen + screen_size, [](auto n) { return n == 99; }),
			"Map-only rendering unexpectedly copied pixels"
		);
		std::fill(colours.begin(), colours.end(), 7);
		iregRenderToScreen(x, 0, x + 10, 0);
		bool copied_shadow = false;
		for (int i = 0; i < I_RES_X; ++i) {
			const int map_x = i + offset;
			const bool relit = colours[map_x] == 0;
			check(screen[i] == (relit ? 0 : 99), "Did not copy exactly the relit scanline span");
			if (relit && (map_x < x || map_x > x + 11))
				copied_shadow = true;
		}
		check(copied_shadow, "Fixture did not exercise a shadow outside the object");
		check(
			std::all_of(screen + I_RES_X, screen + screen_size, [](auto n) { return n == 99; }),
			"Shadow copy overran the framebuffer"
		);
	}
	XGR_Finit();
	ivMap = nullptr;
	iScreenOffs = 0;
}

void relief(bool ttf, bool procedural = false) {
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
	if (!ttf) {
		check(
			std::any_of(heights.begin(), heights.end(), [](auto n) { return n < 128; }),
			"Relief lost negative source height"
		);
	} else {
		check(
			std::all_of(
				heights.begin(), heights.end(), [](auto n) { return n >= 128 && n <= 170; }
			),
			"Procedural relief has invalid heights"
		);
		if (procedural)
			for (auto family :
				{vangers::actint::FontFamily::Menu, vangers::actint::FontFamily::Display}) {
				vangers::iscreen::ReliefFont face(*HFntTable[0], family);
				for (char32_t cp : {U'A', U'Ж', U'Ё', char32_t(0xe000)}) {
					// The TTF path must work even when legacy glyph height storage is absent.
					auto &legacy = *HFntTable[0]->data[vangers::settings::unicode_to_cp866(cp)];
					auto *saved = legacy.HeightMap;
					legacy.HeightMap = nullptr;
					auto glyph = face.glyph(cp);
					legacy.HeightMap = saved;
					check(glyph.source.empty(), "Text retained legacy heights");
					const auto uncached = glyph;
					check(
						glyph.heights(0, 50).empty() && glyph.heights(26, -1).empty(),
						"Invalid dimensions were accepted"
					);
					for (auto size :
						{std::pair{1, 1}, std::pair{7, 9}, std::pair{26, 50}, std::pair{52, 100}}) {
						const auto baseline = glyph.heights(size.first, size.second);
						check(
							baseline.size() == size_t(size.first * size.second),
							"Bad procedural output size"
						);
						check(
							std::all_of(
								baseline.begin(),
								baseline.end(),
								[](auto n) { return n >= 128 && n <= 170; }
							),
							"Invalid generated height"
						);
						check(
							glyph.heights(size.first, size.second) == baseline,
							"Cached relief changes between frames"
						);
						auto regenerated = uncached;
						regenerated.source.assign(100, 0);
						check(
							regenerated.heights(size.first, size.second) == baseline,
							"Procedural relief depends on old bitmap heights"
						);
					}
				}
			}
	}
	if (procedural) {
		struct MaskImage: xgr::text::Image {
			bool filled;
			mutable int calls = 0;
			explicit MaskImage(bool value): filled(value) {}
			xgr::text::Raster rasterize(int width, int height) const override {
				++calls;
				xgr::text::Raster r{
					width, height, std::vector<xgr::text::Pixel>(size_t(width) * height)};
				for (auto &pixel : r.pixels)
					pixel.alpha = filled ? 255 : 0;
				return r;
			}
		};
		vangers::iscreen::ReliefGlyph square;
		square.decorative = true;
		auto image = std::make_shared<MaskImage>(true);
		square.outline = image;
		const auto &raised = square.heights(32, 32);
		const auto *pixels = raised.data();
		for (int i = 0; i < 100; ++i) {
			check(&square.heights(32, 32) == &raised, "Cache hit returned a copy");
			check(square.heights(32, 32).data() == pixels, "Cache hit reallocated heights");
		}
		check(image->calls == 1, "Cache hit rerasterized the outline");
		square.heights(16, 16);
		check(
			&square.heights(32, 32) == &raised && raised.data() == pixels,
			"Another cached size invalidated a height reference"
		);
		check(image->calls == 2, "Size-specific cache missed unexpectedly");
		check(raised[16 * 32 + 16] > raised[16 * 32] + 5, "Solid glyph has no edge bevel");
		vangers::iscreen::ReliefGlyph transparent;
		transparent.outline = std::make_shared<MaskImage>(false);
		const auto empty = transparent.heights(32, 32);
		check(
			std::all_of(empty.begin(), empty.end(), [](auto n) { return n == 128; }),
			"Grain escaped the glyph contour"
		);
	}
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
		if (argc == 2 && std::string(argv[1]) == "--description") {
			using namespace vangers::localization;
			iAVIElement video;
			video.change_description(description_key(DescriptionKind::Item, 1).data());
			video.init_size();
			check(
				video.description && !video.data && video.SizeX == 340 && video.SizeY == 255,
				"Description tried to open an AVI or changed the panel"
			);
			const auto first = video.description->pixels();
			// Selection updates the pending key before the shop loader runs.
			video.description_key = description_key(DescriptionKind::Item, 27).data();
			video.change_description(video.description_key);
			check(first != video.description->pixels(), "New selection retained old cached text");
			video.free();
			check(!video.description && !video.data, "Freed description retained its surface");
			video.load();
			check(video.description && !video.data, "Reopened shop lost description mode");
			iAVIElement copy;
			video.copy(&copy);
			copy.load();
			check(
				copy.description && copy.description->pixels() == video.description->pixels(),
				"Copied screen element lost its description"
			);
			video.change_description(description_key(DescriptionKind::Item, 20).data());
			check(
				std::all_of(
					video.description->pixels().begin(),
					video.description->pixels().end(),
					[](uint32_t p) { return p == 0xFF000000u; }
				),
				"Missing source invented a description"
			);
			return 0;
		}
		if ((argc == 5 || argc == 6) && std::string(argv[1]) == "--parse")
			parse(
				argv[2],
				argv[3],
				std::string(argv[4]) == "unicode",
				argc == 6,
				std::string(argv[4]) == "keyed"
			);
		else if (argc == 3 && std::string(argv[1]) == "--flow")
			flow(std::string(argv[2]) == "ttf");
		else if (argc == 3 && std::string(argv[1]) == "--relief")
			relief(std::string(argv[2]) != "bitmap", std::string(argv[2]) == "procedural");
		else if (argc == 2 && std::string(argv[1]) == "--shadow")
			shadow_damage();
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
