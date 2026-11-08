#include "global.h"

#include "actint/a_consts.h"
#include "actint/actint.h"
#include "actint/mlconsts.h"
#include "actint/mlstruct.h"
#include "iscreen/ikeys.h"
#include "iscreen/iscreen.h"
#include "settings/settings.h"
#include "uvs/diagen.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>

void aciBuildQuestList();
void aciNextPhrase();
void aciStartSpeech();
extern aciML_Dispatcher *aciML_D;
extern aciML_EventSeq *aciSpeechSeq;
extern int aciSpeechEndLocked, aciSpeechBegLocked, aciSpeechStarted, dgMood;
void aLoadFonts();
void aLoadFonts32();
void aFreeFonts();
extern aciFont **aScrFonts, **aScrFonts32;
extern actIntDispatcher *aScrDisp;
extern int aciEscaveEmpty;
extern unsigned char *aciCurColorScheme, *aCellFrame;
extern int aCellSize;
extern XBuffer *aciXConv;
extern int iMouseX, iMouseY;
extern int aciKeyboardLocked;

namespace {
void check(bool condition, const char *message) {
	if (!condition)
		throw std::runtime_error(message);
}

// Exercise the production loaders/backend without requiring private game assets.
// Each CTest invocation has its own process, font files and renderer selection.
struct RenderFixture {
	std::filesystem::path cwd = std::filesystem::current_path();
	std::filesystem::path root =
		std::filesystem::temp_directory_path() /
		("vangers-actint-render-" +
			std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
	std::array<unsigned char, 256> colours{};
	std::array<SDL_Color, 256> palette{};
	XBuffer conversion;
	bool native;

	explicit RenderFixture(bool native): native(native) {
		// Parallel bitmap/TTF tests may observe the same clock tick. Only retry
		// an existing name; filesystem errors still throw normally.
		const auto prefix = root.string();
		for (int suffix = 0; !std::filesystem::create_directory(root); ++suffix) {
			check(suffix < 100, "Cannot create renderer fixture");
			root = prefix + "-" + std::to_string(suffix);
		}
		std::filesystem::create_directories(root / "resource/actint/fonts");
		for (const auto range : {false, true}) {
			for (int font = 0; font < (range ? AS_NUM_FONTS_32 : AS_NUM_FONTS); ++font) {
				std::ofstream file(
					root / "resource/actint/fonts" /
						(std::string(range ? "cfont0" : "sfont0") + std::to_string(font) + ".bml"),
					std::ios::binary
				);
				const char header[]{10, 0, 18, 0, 0, 1}; // 256 cells, 10 x 18.
				file.write(header, sizeof header);
				for (int cp = 0; cp < 256; ++cp) {
					std::array<char, 180> cell{};
					if (cp != ' ')
						for (int y = 4; y < 14; ++y)
							for (int x = cp == '0' ? 2 : 1; x < 8; ++x)
								cell[y * 10 + x] = range ? 31 : 1;
					file.write(cell.data(), cell.size());
				}
				check(file.good(), "Cannot write bitmap font fixture");
			}
		}
		std::filesystem::current_path(root);
		vangers::settings::settings_manager().get_mutable().video.ttf_text = native;
		aLoadFonts();
		aLoadFonts32();
		colours.fill(1);
		aciCurColorScheme = colours.data();
		aciXConv = &conversion;
		for (auto &colour : palette)
			colour = {255, 255, 255, 255};
	}

	~RenderFixture() {
		aFreeFonts();
		for (int i = 0; i < AS_NUM_FONTS_32; ++i)
			delete aScrFonts32[i];
		delete[] aScrFonts32;
		aScrFonts32 = nullptr;
		aciCurColorScheme = aCellFrame = nullptr;
		aciXConv = nullptr;
		std::filesystem::current_path(cwd);
		std::filesystem::remove_all(root);
	}

	std::vector<uint32_t> pixels(const iBitmapElement &bitmap, int scale = 1) const {
		if (native) {
			if (auto *layer = xgr::text::find(bitmap.fdata))
				return layer->render(bitmap.SizeX * scale, bitmap.SizeY * scale, palette.data());
			return std::vector<uint32_t>(bitmap.SizeX * bitmap.SizeY * scale * scale);
		}
		check(scale == 1, "Bitmap fixture uses logical pixels");
		return {bitmap.fdata, bitmap.fdata + bitmap.SizeX * bitmap.SizeY};
	}
};

void resize(iBitmapElement &bitmap, int width, int height) {
	bitmap.free();
	bitmap.SizeX = bitmap.bSizeX = width;
	bitmap.SizeY = bitmap.bSizeY = height;
	bitmap.alloc_mem();
	std::fill_n(bitmap.fdata, width * height, 0);
}

bool has_ink(const std::vector<uint32_t> &pixels) {
	return std::any_of(pixels.begin(), pixels.end(), [](uint32_t p) { return p != 0; });
}

fncMenu *add_menu(fncMenuSet &set, std::initializer_list<const char *> labels, bool topic) {
	auto *menu = new fncMenu;
	menu->flags |= FM_RANGE_FONT;
	int code = 0;
	for (const auto *label : labels) {
		auto *item = new fncMenuItem;
		item->init_name(label);
		item->fnc_code = code++;
		item->font = 0;
		item->space = 1;
		menu->add_item(item);
	}
	set.add(menu, topic);
	return menu;
}
} // namespace

void actint_menu_render_test(bool native) {
	RenderFixture fixture(native);
	iScreenObject owner;
	iBitmapElement output;
	resize(output, 500, 100); // Leave room for the original up/down arrow cells.
	output.owner = &owner;
	fncMenuSet set;
	set.data = &output;
	set.redraw(); // No redraw buffer yet.
	check(!has_ink(fixture.pixels(output)), "Uninitialized question menu retained ink");
	set.init();
	add_menu(set, {"А что иль кто есть"}, false);
	auto *topic = add_menu(set, {"мехос", "Инкубатор"}, true);
	add_menu(set, {"советник?"}, false);
	set.init_redraw();
	set.build();

	const auto check_copy = [&](int from_x, int to_x, int width) {
		xgr::text::Layer composed(set.redraw_data->SizeX, set.redraw_data->SizeY);
		if (native) {
			// Compose independently rendered children. A topic's allocation is
			// sized for its longest label and may overlap the preceding prefix.
			auto *child = (fncMenu *)set.items->first;
			for (int i = 0; i < set.items->Size; ++i) {
				xgr::text::clear(set.redraw_data->fdata);
				child->redraw();
				composed.copy(
					*xgr::text::find(set.redraw_data->fdata),
					{0, 0, set.redraw_data->SizeX, set.redraw_data->SizeY},
					0,
					0
				);
				child = (fncMenu *)child->next;
			}
		}
		set.redraw();
		for (int scale : {1, native ? 2 : 1}) {
			const auto source = fixture.pixels(*set.redraw_data, scale);
			check(has_ink(source), "Question fixture did not render its child menus");
			if (native)
				check(
					source == composed.render(
								  set.redraw_data->SizeX * scale,
								  set.redraw_data->SizeY * scale,
								  fixture.palette.data()
							  ),
					"Question topic erased part of its neighbouring prefix/postfix"
				);
			std::vector<uint32_t> expected(output.SizeX * output.SizeY * scale * scale);
			for (int y = 0; y < output.SizeY * scale; ++y)
				std::copy_n(
					source.begin() + y * set.redraw_data->SizeX * scale + from_x * scale,
					width * scale,
					expected.begin() + y * output.SizeX * scale + to_x * scale
				);
			check(
				fixture.pixels(output, scale) == expected,
				"Question compositor lost or misplaced child text"
			);
		}
	};
	check_copy(0, (output.SizeX - set.redraw_data->SizeX) / 2, set.redraw_data->SizeX);
	topic->curItem = topic->firstItem = topic->get_fnc_item(1);
	set.build();
	set.redraw();
	const auto after_selection = fixture.pixels(*set.redraw_data);
	xgr::text::clear(set.redraw_data->fdata);
	set.redraw();
	check(
		after_selection == fixture.pixels(*set.redraw_data),
		"Changing the selected question retained text at old positions"
	);
	check_copy(0, (output.SizeX - set.redraw_data->SizeX) / 2, set.redraw_data->SizeX);
	resize(output, 60, output.SizeY);
	set.PosX = 0;
	set.shift(-7);
	check_copy(7, 0, 60);
	set.shift(-10000);
	check_copy(set.redraw_data->SizeX - 60, 0, 60);
	set.finit();
	check(!set.redraw_data->fdata, "Freed menu bitmap retained a dangling companion-layer key");
	set.redraw();
	check(!has_ink(fixture.pixels(output)), "Cleared question menu retained output text");

	// Build the actual dialogue menu, including its stable selection payload.
	// Identical labels must stay two independent selectable items in both backends.
	resize(output, 500, 100);
	aScrDisp = new actIntDispatcher;
	aScrDisp->qMenu = new fncMenuSet;
	aScrDisp->qMenu->data = &output;
	dgD = new DiagenDispatcher;
	auto *room = new dgRoom(dgD);
	room->link(dgD->rtail);
	dgD->currentR = room;
	char label[] = "Одинаковый вопрос";
	for (const auto *id : {"topic.first", "topic.second"}) {
		auto *q = new dgQuery;
		q->id = id;
		q->subj = label;
		q->link(room->qVtail);
	}
	aciBuildQuestList();
	auto *menu = aScrDisp->qMenu->qMenu;
	check(menu && menu->items->Size == 2, "Dialogue menu collapsed duplicate labels");
	auto *first = menu->get_fnc_item(0), *second = menu->get_fnc_item(1);
	check(
		first && second && !std::strcmp(first->name, second->name) &&
			first->dialogue_id != second->dialogue_id && !first->dialogue_id.empty() &&
			!second->dialogue_id.empty(),
		"Dialogue menu used labels/ordinals instead of question IDs"
	);
	fncMenuItem copy;
	delete copy.scankey; // clone borrows the source key object, as in production callers.
	second->clone(&copy);
	check(copy.dialogue_id == second->dialogue_id, "Cloning lost a dialogue selection ID");
	aScrDisp->qMenu->redraw();
	const auto first_pixels = fixture.pixels(output);
	check(has_ink(first_pixels), "ID-bearing menu did not render");
	menu->curItem = menu->firstItem = second;
	aScrDisp->qMenu->build();
	aScrDisp->qMenu->redraw();
	check(fixture.pixels(output) == first_pixels, "An invisible ID changed the rendered label");
	delete dgD;
	delete aScrDisp;
	aScrDisp = nullptr;
}

void actint_counter_render_test(bool native) {
	RenderFixture fixture(native);
	iScreenObject owner;
	iBitmapElement output, expected;
	resize(output, 38, 38);
	resize(expected, 38, 38);
	output.owner = &owner;
	aCellSize = 37;
	std::array<unsigned char, 38 * 38> frame{};
	aCellFrame = frame.data();
	int value = 0;
	CounterPanel counter;
	counter.ID = -1;
	counter.value_ptr = &value;
	counter.SizeX = counter.SizeY = 38;
	counter.MaxLen = 1;
	counter.font = 0;
	counter.iScreenOwner = &output;
	const int x = (38 - 10) / 2, y = (38 - 18) / 2;
	for (const auto range : {true, false}) {
		counter.flags = range ? CP_RANGE_FONT : 0;
		for (const int digit : {0, 1, 5, 8}) {
			value = digit;
			counter.redraw();
			std::fill_n(expected.fdata, 38 * 38, 0);
			xgr::text::prepare(expected.fdata, 38, 38);
			char text[]{char('0' + digit), 0};
			if (native) {
				if (range)
					aPutStr32Utf8(x, y, 0, 1, 1, text, 38, expected.fdata, 0);
				else
					aPutStrUtf8(
						x + aScrFonts[0]->LeftOffs[int(text[0])],
						y,
						0,
						0x101,
						text,
						38,
						reinterpret_cast<unsigned char *>(expected.fdata),
						0
					);
			} else if (range)
				aPutChar32(x, y, 0, 1, 1, text[0], 38, expected.fdata);
			else
				aPutChar(
					x,
					y,
					0,
					0x101,
					text[0],
					38,
					38,
					reinterpret_cast<unsigned char *>(expected.fdata)
				);
			for (int scale : {1, native ? 2 : 1}) {
				const auto reference = fixture.pixels(expected, scale);
				check(has_ink(reference), "Counter reference did not render its digit");
				check(
					fixture.pixels(output, scale) == reference,
					"Counter changed the character origin (trimmed range / untrimmed standard)"
				);
			}
		}
	}
}

void aciInitPanel(InfoPanel *, const char *);
namespace {
void item_panel_row_limit_test(RenderFixture &fixture) {
	for (const bool range : {false, true}) {
		iScreenObject owner;
		iBitmapElement output;
		resize(output, 261, 89);
		output.owner = &owner;
		InfoPanel panel;
		panel.SizeX = output.SizeX;
		panel.SizeY = output.SizeY;
		panel.vSpace = -2;
		panel.bCol = 0;
		panel.flags = range ? IP_RANGE_FONT : 0;
		panel.iScreenOwner = &output;
		for (const int rows : {3, 4, 5}) {
			panel.free_list();
			panel.add_item("Title", -1, 1);
			for (int i = 0; i < rows - 3; ++i)
				panel.add_item("Comment");
			panel.add_item("Resource: 42");
			panel.add_item("Price: 150", -1, 1);
			const auto view = panel.item_view();
			// All 3/4/5-row cases fit in the rectangular buffer. The authored
			// limit still protects the text from the shop's decorative frame.
			check(view.count == panel.MaxStr, "Physically fitting rows bypassed max_str");
			panel.redraw();
			check(has_ink(fixture.pixels(output)), "Intermediate description rendered no ink");
			if (rows == panel.MaxStr) {
				check(!panel.scroll_items(1), "A fitting description became scrollable");
				check(
					view.y == ((panel.SizeY - rows * view.step) >> 1),
					"A fitting description lost its original centering"
				);
			} else {
				for (int first = 1; first <= rows - view.count; ++first) {
					check(
						panel.scroll_items(1) && panel.firstVisibleItem == first,
						"Intermediate description cannot reach its final rows"
					);
					panel.redraw();
				}
				check(
					panel.scroll_items(-rows) && panel.firstVisibleItem == 0,
					"Intermediate description cannot return to its title"
				);
			}
		}
		// Non-item panels (e.g. full vehicle statistics) intentionally show
		// more than the default three rows and are not the scrollable iP.
		for (const int type : {ACI_PRICE_PANEL, ACI_ANSWER_PANEL}) {
			panel.type = type;
			check(panel.item_view().count == 5, "Item row limit truncated another panel");
			panel.redraw();
		}
		const auto original = fixture.pixels(output);
		panel.type = ACI_INFO_PANEL;
		panel.MaxStr = 5;
		check(
			panel.item_view().count == 5 && !panel.scroll_items(1),
			"An explicitly larger item panel ignored its row limit"
		);
		panel.redraw();
		check(fixture.pixels(output) == original, "Fitting rows changed their rendering");
	}
}

void item_panel_scroll_test(RenderFixture &fixture) {
	for (const bool range : {false, true}) {
		for (const auto size : {std::pair{580, 100}, {332, 112}, {261, 89}, {275, 94}, {220, 74}}) {
			iScreenObject owner;
			iBitmapElement output, expected;
			resize(output, size.first, size.second);
			resize(expected, size.first, size.second);
			output.owner = expected.owner = &owner;
			InfoPanel panel, control;
			panel.SizeX = control.SizeX = size.first;
			panel.SizeY = control.SizeY = size.second;
			panel.bCol = control.bCol = 0;
			panel.vSpace = control.vSpace = -2;
			panel.flags = control.flags = range ? IP_RANGE_FONT : 0;
			panel.iScreenOwner = &output;
			control.iScreenOwner = &expected;
			std::string comment;
			for (int i = 0; i < 18; ++i)
				comment += "Боеприпасы для Спитл Системы. Cruise missiles. ";
			const auto populate = [&] {
				panel.free_list();
				panel.add_item("Спитл / Speettle", 1, 1);
				char *value = comment.data();
				panel.add_items(1, &value);
				panel.add_item("Resource: 42", 1, 1);
				panel.add_item("Price: 150", 0, 1);
			};
			populate();
			check(panel.item_view().count < panel.items->Size, "Fixture did not overflow");
			check(
				panel.item_view().count <= panel.MaxStr, "Overflow ignored the authored row limit"
			);
			std::vector<bool> seen(panel.items->Size);
			actIntDispatcher dispatcher;
			dispatcher.iP = &panel;
			dispatcher.curMode = AS_INV_MODE;
			SDL_Event wheel{};
			wheel.type = SDL_EVENT_MOUSE_WHEEL;
			wheel.wheel.y = -1;
			for (int first = 0;; ++first) {
				const auto view = panel.item_view();
				check(panel.firstVisibleItem == first, "Wheel skipped description rows");
				check(
					view.y >= 0 &&
						view.y + (view.count - 1) * view.step + view.height <= panel.SizeY,
					"Description glyph cells exceed the panel"
				);
				panel.redraw(); // The old code aborted here on the first long description.
				control.free_list();
				auto *row = static_cast<InfoPanelItem *>(panel.items->first);
				for (int i = 0; i < first; ++i)
					row = static_cast<InfoPanelItem *>(row->next);
				for (int i = 0; i < view.count;
					++i, row = static_cast<InfoPanelItem *>(row->next)) {
					seen[first + i] = true;
					control.add_item(row->ID_ptr.c_str(), row->font, row->color);
				}
				control.OffsY = view.y - ((control.SizeY - view.count * view.step) >> 1);
				control.redraw();
				const auto actual = fixture.pixels(output), wanted = fixture.pixels(expected);
				check(has_ink(actual), "Long description rendered no ink");
				// Ignore only the separate continuation indicators in the gutters.
				check(
					std::equal(
						actual.begin() + view.y * panel.SizeX,
						actual.begin() +
							(view.y + (view.count - 1) * view.step + view.height) * panel.SizeX,
						wanted.begin() + view.y * panel.SizeX
					),
					"Scrolling changed row fonts, colors or placement"
				);
				populate(); // Actual hover producers rebuild every frame.
				check(
					panel.item_view().count == view.count && panel.firstVisibleItem == first,
					"Identical hover reset the reading position"
				);
				check(dispatcher.scroll_info_panel(wheel), "Overflow wheel event was not consumed");
				if (panel.firstVisibleItem == first)
					break;
			}
			check(
				std::all_of(seen.begin(), seen.end(), [](bool value) { return value; }),
				"Title, comment tail, parameters or price became inaccessible"
			);
			wheel.wheel.direction = SDL_MOUSEWHEEL_FLIPPED;
			const int last = panel.firstVisibleItem;
			check(
				dispatcher.scroll_info_panel(wheel) && panel.firstVisibleItem == last - 1,
				"Flipped wheel scrolled in the wrong direction"
			);
			{
				// Exercise the real SDL key buffer and world inventory consumer,
				// including its per-frame hover rebuild before reading the event.
				KeyBuffer buffer;
				auto *oldBuffer = KeyBuf;
				const int oldLock = aciKeyboardLocked, oldX = iMouseX, oldY = iMouseY;
				KeyBuf = &buffer;
				aciKeyboardLocked = 0;
				iMouseX = panel.PosX;
				iMouseY = panel.PosY;
				::key(&wheel);
				dispatcher.KeyQuant();
				check(
					!buffer.size && panel.firstVisibleItem == last - 2,
					"World inventory did not consume a buffered wheel event"
				);
				KeyBuf = oldBuffer;
				aciKeyboardLocked = oldLock;
				iMouseX = oldX;
				iMouseY = oldY;
			}
			SDL_Event key{};
			key.type = SDL_EVENT_KEY_DOWN;
			key.key.scancode = SDL_SCANCODE_PAGEUP;
			check(
				dispatcher.scroll_info_panel(key) && panel.firstVisibleItem < last - 1,
				"Page Up did not scroll the current description"
			);
			key.key.scancode = SDL_SCANCODE_PAGEDOWN;
			dispatcher.flags = AS_ISCREEN | AS_ISCREEN_INV_MODE;
			check(dispatcher.scroll_info_panel(key), "Escave descriptions cannot scroll");
			const int oldX = iMouseX, oldY = iMouseY;
			iMouseX = panel.PosX;
			iMouseY = panel.PosY;
			dispatcher.inv_mouse_move_quant();
			dispatcher.inv_mouse_imove_quant();
			check(
				panel.items->Size == int(seen.size()), "Hovering the panel erased its description"
			);
			iMouseX = oldX;
			iMouseY = oldY;
			for (const int flags :
				{AS_FULLSCR, AS_LOCKED, AS_TEXT_MODE, AS_CHAT_MODE, AS_INV_MOVE_ITEM, AS_ISCREEN}) {
				dispatcher.flags = flags;
				check(!dispatcher.scroll_info_panel(key), "Description stole another mode's input");
			}
			dispatcher.flags = 0;
			output.flags |= EL_HIDE;
			check(!dispatcher.scroll_info_panel(key), "Hidden description consumed input");
			output.flags &= ~EL_HIDE;
			comment += " Changed item.";
			populate();
			panel.item_view();
			check(
				panel.firstVisibleItem == 0, "New description inherited the old reading position"
			);
			panel.free_list();
			panel.add_item("Short text", 1, 1);
			panel.redraw();
			control.free_list();
			control.OffsY = 0;
			control.add_item("Short text", 1, 1);
			control.redraw();
			check(
				fixture.pixels(output) == fixture.pixels(expected) && !panel.scroll_items(1),
				"Short description changed or became scrollable"
			);
			panel.set_text_pages({{"one"}, {"two"}}, 1);
			check(
				!dispatcher.scroll_info_panel(key) && panel.textPage == 0,
				"Item scrolling advanced a Diagen page"
			);
			panel.free_list();
			populate();
			panel.SizeY = 1;
			resize(output, panel.SizeX, panel.SizeY);
			panel.redraw();
			check(!panel.item_view().count && !panel.scroll_items(1), "Tiny panel overflowed");
			dispatcher.iP = nullptr;
		}
	}
}
} // namespace

void actint_flow_test(bool native) {
	RenderFixture fixture(native);
	item_panel_row_limit_test(fixture);
	item_panel_scroll_test(fixture);
	{
		InfoPanel comments;
		comments.flags |= IP_RANGE_FONT;
		comments.SizeX = 600;
		comments.SizeY = 300;
		std::string text = "Самонаводящиеся боеприпасы\nдля Спитл Системы";
		char *value = text.data();
		comments.add_items(1, &value);
		check(comments.items->Size == 2, "Semantic comment lost original hard breaks");
		check(comments.items->last->ID_ptr == "для Спитл Системы", "Last comment line lost");
		comments.free_list();
		comments.SizeX = 150;
		comments.add_items(1, &value);
		std::string restored;
		auto *row = comments.items->first;
		for (int i = 0; i < comments.items->Size; ++i, row = row->next) {
			check(
				aStrLen32Utf8(row->ID_ptr.c_str(), 0, comments.hSpace) < comments.SizeX,
				"Measured comment exceeds the panel"
			);
			restored += row->ID_ptr;
		}
		text.erase(text.find('\n'), 1);
		check(restored == text, "Comment wrapping lost translated text");
	}
	InfoPanel panel;
	panel.font = 0;
	panel.SizeX = 150;
	panel.SizeY = 60;
	panel.flags |= IP_RANGE_FONT;
	const std::string phrase = "Привет мир. A long phrase must retain every word and every space "
							   "when it exceeds the panel height.";
	aciInitPanel(&panel, phrase.c_str());
	check(
		panel.textPages.size() > 1 && panel.items->Size <= 3,
		"Dialogue was truncated instead of paginated"
	);
	actIntDispatcher dispatcher;
	aScrDisp = &dispatcher;
	iBitmapElement output;
	panel.iScreenOwner = &output;
	panel.type = ACI_ANSWER_PANEL;
	dispatcher.i_infoPanels->connect(&panel);
	const int oldEmpty = aciEscaveEmpty;
	aciEscaveEmpty = 1;
	std::string restored;
	for (size_t page = 0; page < panel.textPages.size(); ++page) {
		auto *item = panel.items->first;
		for (int i = 0; i < panel.items->Size; ++i, item = item->next)
			restored += item->ID_ptr;
		if (page + 1 < panel.textPages.size()) {
			// dgD is deliberately null: visual continuation must not query Diagen
			// or touch the quest-menu path. Use the real click entry point.
			aciNextPhrase();
			check(panel.textPage == page + 1, "Click did not continue the visual phrase");
		}
	}
	// A terminal/service phrase must not close the actual ML continue button
	// after its first page. Test the real speech-sequence builder and click path.
	{
		aciML_Dispatcher animation;
		auto *set = new aciML_DataSet;
		animation.ml_data_sets->connect(set);
		animation.curDataSet = set;
		animation.ml_data = set->ml_data;
		auto *lock = new aciML_EventSeq;
		lock->ID = AML_LOCK0_SEQ;
		lock->alloc_mem(1);
		lock->SeqIDs[0] = 12345;
		lock->SeqModes[0] = AML_KEY;
		set->seq_list->connect(lock);
		aciML_EventSeq speech;
		speech.alloc_mem(256);
		aciML_D = &animation;
		aciSpeechSeq = &speech;
		dgMood = 0;
		aciSpeechEndLocked = aciSpeechBegLocked = aciSpeechStarted = 1;
		aciEscaveEmpty = 0;
		panel.set_text_pages({{"one"}, {"two"}, {"three"}}, 1);
		aciStartSpeech();
		check(speech.size == 0 && !aciSpeechBegLocked, "Terminal phrase locked unread pages");
		aciNextPhrase();
		check(speech.size == 0 && !aciSpeechBegLocked, "Middle page closed the speech window");
		aciNextPhrase();
		check(
			speech.size == 1 && aciSpeechBegLocked, "Final page lost the original closing animation"
		);
		aciML_D = nullptr;
		aciSpeechSeq = nullptr;
	}

	dispatcher.i_infoPanels->dconnect(&panel);
	aciEscaveEmpty = oldEmpty;
	aScrDisp = nullptr;
	check(restored == phrase, "Dialogue lost data between visual pages");
	panel.free_list();
	check(!panel.next_text_page(), "Old dialogue page survived reset");
	const int oldX = XGR_MAXX, oldY = XGR_MAXY;
	XGR_MAXX = 800;
	XGR_MAXY = 600;
	{
		aciScreenText text;
		std::string body;
		for (int i = 0; i < 300; ++i)
			body += "Текст word ";
		text.MaxPageStr = 1; // Flow mode must paginate by actual box height.
		text.alloc_utf8(body, vangers::text::LineMode::Paragraphs);
		check(text.ensure_layout() && text.PageTable->Size > 1, "Text page was not laid out");
		auto *first = (aciScreenTextPage *)text.PageTable->first;
		check(
			first->NumStr > 2 && first->StrTable[first->NumStr - 1] == nullptr,
			"Page sentinel/height contract"
		);
		auto *item = text.StrTable->first;
		for (int i = 0; i < text.StrTable->Size; ++i, item = item->next)
			check(
				aTextWidth32Utf8(((aciScreenTextString *)item)->data, text.font, 1) <= 680,
				"Full-screen row exceeds its box"
			);
		XGR_SetClip(11, 12, 400, 450);
		text.redraw(); // No visible rows yet; it must still restore the caller's viewport.
		check(
			XGR_Obj.clipLeft == 11 && XGR_Obj.clipTop == 12 && XGR_Obj.clipRight == 400 &&
				XGR_Obj.clipBottom == 450,
			"Text leaked its fullscreen clip to the world renderer"
		);
		const auto *stable = text.data;
		check(text.ensure_layout() && text.data == stable, "Unchanged layout was rebuilt");
		check(
			text.NextPage() == 1 && text.CurPageData == first->next, "Full-screen next page failed"
		);
		++text.DeltaY;
		check(
			text.ensure_layout() && text.CurPage == 0 && text.CurStr == 0,
			"Changed logical metrics skipped source text"
		);
		text.alloc_utf8("one\ftwo");
		check(text.ensure_layout() && text.PageTable->Size == 2, "Explicit page break lost");
		text.font = 3;
		text.DeltaY = -2;
		text.MaxPageStr = 19; // Final-image text uses a different font/vertical rhythm.
		text.alloc_utf8(body);
		check(text.ensure_layout() && text.PageTable->Size > 1, "Final text was not paginated");
		auto *page = (aciScreenTextPage *)text.PageTable->first;
		for (int i = 0; i < text.PageTable->Size; ++i, page = (aciScreenTextPage *)page->next) {
			check(page->NumStr <= 20, "Final text exceeds its authored page cap");
			for (int row = 0; row < page->NumStr - 1; ++row)
				check(
					aTextWidth32Utf8(page->StrTable[row], 3, 1) <= 680, "Final row exceeds its box"
				);
		}
		text.NextPage();
		text.clear();
		check(
			text.CurPage == 0 && text.CurPageData == text.PageTable->first,
			"Reset retained a stale page"
		);
	}
	XGR_MAXX = oldX;
	XGR_MAXY = oldY;
}
