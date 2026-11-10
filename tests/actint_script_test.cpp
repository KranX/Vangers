#include "global.h"

#include "actint/actint.h"
#include "actint/mlconsts.h"
#include "actint/mlstruct.h"
#include "actint/text.h"
#include "iscreen/ikeys.h"
#include "lang.h"
#include "localization/catalog.h"
#include "localization/description.h"
#include "runtime.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

void aParseScript(const char *, const char *);
void actint_menu_render_test(bool native);
void actint_counter_render_test(bool native);
void actint_description_switch_test();
extern actIntDispatcher *aScrDisp;
extern aciFont **aScrFonts;

namespace {

namespace fs = std::filesystem;
const std::string unicode_text = "Тест 日本 😀";

void check(bool condition, const char *message) {
	if (!condition)
		throw std::runtime_error(message);
}

void parse(const char *input, const char *output, const char *encoding) {
	aParseScript(input, output);
	const auto expected = std::string(encoding) == "keyed"	  ? "NEW GAME"
						  : std::string(encoding) == "legacy" ? "Ёж"
															  : unicode_text;
	const auto *world = (aciWorldInfo *)aScrDisp->wMap->world_list->last;
	const auto world_id = std::string(encoding) == "keyed" ? "@l10n:menu.iSTR_NEW_GAME" : expected;
	check(world && world->name == world_id, "A display translation changed a world lookup ID");
	const auto *item = (invItem *)aScrDisp->itemList->last;
	check(item && item->ID_ptr == expected, "Item label did not survive the parser");
	check(item->pTemplate == expected, "Item template did not survive the parser");
	check(item->promptData == expected, "Prompt text did not survive the parser");
	const auto *menu = (fncMenu *)aScrDisp->menuList->last;
	const auto *entry = menu ? (fncMenuItem *)menu->items->last : nullptr;
	check(entry && entry->name == expected, "Menu text did not survive the parser");
	check(aScrDisp->get_locdata_id("lookup.id") == 3, "A translation renamed the location ID");
	const auto *location = aScrDisp->get_locdata(3);
	check(
		location && std::string(location->nameID2) == expected,
		"Location display name was not translated"
	);
	delete aScrDisp;
	aScrDisp = nullptr;
	delete aciML_D;
	aciML_D = nullptr;
}

void comments(const char *input, const char *output, const char *locale) {
	setLocale(locale);
	aParseScript(input, output);
	int count = 0, described = 0, shop_described = 0;
	for (auto *item = static_cast<invItem *>(aScrDisp->itemList->last); item;
		item = static_cast<invItem *>(item->prev)) {
		++count;
		const auto *expected =
			vangers::localization::catalog().find_text("items." + item->ID_ptr + "_COMMENTS");
		if (!expected) {
			check(item->numComments == 0, "An originally empty comment became visible");
			continue;
		}
		check(
			item->numComments == 1 && item->comments[0] == *expected,
			"A shared script profile lost part of a semantic item comment"
		);
		++described;
		const bool shop = item->ID_ptr.front() == 'i';
		shop_described += shop;
		const auto comment_rows = 1 + std::count(expected->begin(), expected->end(), '\n');
		check(comment_rows <= (shop ? 1 : 3), "Stock text exceeds its authored comment rows");
		// Include title, optional parameter and price in the fixed panel budget.
		check(
			1 + comment_rows + (item->pTemplate ? 1 : 0) + 1 <= 5,
			"Stock item text exceeds the fixed panel row budget"
		);
	}
	check(
		count == 156 && described == 88 && shop_described == 15,
		"Stock inventory/shop comment fixture is incomplete"
	);
	delete aScrDisp;
	aScrDisp = nullptr;
	delete aciML_D;
	aciML_D = nullptr;
}

void description_map(const char *input, const char *output, const char *locale) {
	setLocale(locale);
	aParseScript(input, output);
	using namespace vangers::localization;
	int items = 0, mechos = 0;
	const auto check_key = [](DescriptionKind kind, int id, const char *avi) {
		const auto key = description_key(kind, id);
		check(!key.empty(), "Stock model ID is not mapped to a description");
		const std::string path(avi);
		const auto start = path.find(kind == DescriptionKind::Item ? "itm" : "mech");
		check(start != std::string::npos, "Unexpected stock description AVI path");
		const int offset = kind == DescriptionKind::Item ? 3 : 4;
		check(
			key.substr(key.size() - 2) == path.substr(start + offset, 2),
			"Stable game ID selected another item's description"
		);
		check(
			catalog().find_text(std::string(key)) != nullptr, "Description key absent from package"
		);
	};
	for (auto *item = static_cast<invItem *>(aScrDisp->i_itemList->last); item;
		item = static_cast<invItem *>(item->prev)) {
		check(item->numAviIDs >= 3, "Stock item lost its representation switch");
		check_key(DescriptionKind::Item, item->ID, item->avi_ids[1]);
		++items;
	}
	for (auto *matrix = static_cast<invMatrix *>(aScrDisp->i_matrixList->last); matrix;
		matrix = static_cast<invMatrix *>(matrix->prev)) {
		check(matrix->numAviIDs >= 3, "Stock mechos lost its representation switch");
		check_key(DescriptionKind::Mechos, matrix->type, matrix->avi_ids[1]);
		++mechos;
	}
	check(items == 78 && mechos == 39, "Incomplete stock description ID audit");
	// Iscreen item menus are process-lifetime shared models, not dispatcher
	// list members. The parser fixture owns these otherwise-unlisted menus.
	for (auto *item = static_cast<invItem *>(aScrDisp->i_itemList->last); item;
		item = static_cast<invItem *>(item->prev)) {
		delete static_cast<fncMenu *>(item->menu);
		item->menu = nullptr;
	}
	delete aScrDisp;
	aScrDisp = nullptr;
	delete aciML_D;
	aciML_D = nullptr;
}

std::string drawn;

int width(void *text, int, int) {
	return std::strlen((const char *)text) * 8;
}

int height(void *, int, int) {
	return 16;
}

void draw(int, int, int, void *text, int, int, int) {
	drawn = (const char *)text;
}

void mouse_prompt() {
	xgrScreenSizeX = 800;
	xgrScreenSizeY = 600;
	XGR_SetTextXFnc(width);
	XGR_SetTextYFnc(height);
	XGR_SetTextOutFnc(draw);
	XGR_Mouse mouse;
	mouse.SpotX = mouse.SpotY = 0;
	mouse.SizeX = 12;
	mouse.SizeY = 20;
	mouse.flags |= XGM_VISIBLE;
	XGR_MousePromptScreen screen;
	auto *prompt = new XGR_MousePromptData(0, 0, 100, 100, 0);
	prompt->init_text("old text");
	screen.AddElement(prompt);
	mouse.SetPromptData(&screen);
	mouse.InitPrompt();
	mouse.PutPrompt();
	check(drawn == "old text", "InitPrompt/PutPrompt did not display the source");
	prompt->init_text("new and longer text");
	// The snapshot stays owned until refresh, even after the old buffer is freed.
	check(screen.curText == "old text", "Active snapshot borrowed the replaced buffer");
	mouse.InitPrompt();
	mouse.PutPrompt();
	check(drawn == "new and longer text", "Stationary prompt did not refresh");
	check(mouse.PromptSizeX == width(prompt->textData, 0, 0), "Prompt geometry did not refresh");
	screen.RemoveElement(prompt);
	delete prompt;
	mouse.InitPrompt();
	drawn.clear();
	mouse.PutPrompt();
	check(drawn.empty() && screen.curText.empty(), "Removed prompt is still displayed");
	check(!(mouse.flags & XGM_PROMPT_ACTIVE), "No-hit prompt is still active");

	// A previously used incoming list must not revive its old snapshot or timer.
	XGR_MousePromptScreen replacement;
	replacement.curText = "stale";
	replacement.Timer = 100;
	mouse.flags |= XGM_PROMPT_ACTIVE;
	mouse.SetPromptData(&replacement);
	check(replacement.curText.empty() && !replacement.Timer, "List switch retained stale state");
	check(!(mouse.flags & XGM_PROMPT_ACTIVE), "List switch retained the active flag");
	auto *empty = new XGR_MousePromptData(0, 0, 100, 100, 0);
	empty->init_text("");
	replacement.AddElement(empty);
	mouse.InitPrompt();
	check(!(mouse.flags & XGM_PROMPT_ACTIVE), "Empty source activated a prompt");
	auto *expired = new XGR_MousePromptScreen;
	mouse.SetPromptData(expired);
	delete expired;
	mouse.SetPromptData(nullptr);
	delete[] mouse.PromptFon;
	XGR_SetTextXFnc(nullptr);
	XGR_SetTextYFnc(nullptr);
	XGR_SetTextOutFnc(nullptr);
}

void prompt_clipping() {
	actIntDispatcher dispatcher;
	aScrDisp = &dispatcher;
	dispatcher.flags |= AS_FULLSCR;
	// Zero-height glyphs exercise real layout/redraw without a video device.
	struct FontFixture {
		aciFont font;
		std::array<char, 256> offsets{};
		aciFont *fonts[8] = {};
		aciFont **previous = aScrFonts;
		FontFixture() {
			font.Size = 256;
			font.SizeX = 10;
			font.LeftOffs = font.RightOffs = offsets.data();
			fonts[7] = &font;
			aScrFonts = fonts;
		}
		~FontFixture() {
			aScrFonts = previous;
			font.LeftOffs = font.RightOffs = nullptr; // The fixture owns offsets.
		}
	} fixture;
	GAME_TIME_COEFF = 1;
	aciPromptData prompt(3);
	prompt.add_utf8_str(0, unicode_text.c_str());
	prompt.add_utf8_str(2, "Ёж"); // A fitting line and a null slot share the clipped presentation.
	prompt.CurTimer = 10000;
	prompt.redraw(0, 0, 24, 100);
	check(
		std::string((char *)prompt.StrBuf[0]) == unicode_text, "Narrow redraw destroyed model text"
	);
	check(
		!prompt.StrBuf[1] && std::string((char *)prompt.StrBuf[2]) == "Ёж",
		"Other prompt lines changed"
	);
	check(prompt.PosX[0] >= 0, "Clipped text did not fit the narrow viewport");
	prompt.redraw(0, 0, 1000, 100);
	check(
		prompt.PosX[0] == (1000 - aStrLenUtf8(unicode_text.c_str(), 7, 1)) / 2,
		"Wider redraw did not restore the full text"
	);
	aScrDisp = nullptr;
}

void resource_lifetimes() {
	aciML_Dispatcher dispatcher; // Owns the event/flush tables and their array heaps.
	aciML_Data data;
	data.name = vangers::actint::copy_text("resource name");
	aciML_ItemData item;
	item.frameName = vangers::actint::copy_text("frame name");
	item.frameData = new unsigned char[16]{};
	item.empty_frameData = new unsigned char[16]{};
	aciML_EventSeq sequence;
	sequence.alloc_mem(4);
	const auto file =
		fs::temp_directory_path() /
		("vangers-shutter-test-" +
			std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".bmp");
	{
		std::ofstream output(file, std::ios::binary);
		const char bytes[] = {4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		output.write(bytes, sizeof bytes);
		check(output.good(), "Cannot create shutter fixture");
	}
	{
		aciLocationShutterInfo shutter;
		shutter.name = vangers::actint::copy_text(file.string());
		shutter.Size.X = shutter.Size.Y = 4;
		shutter.loadData();
		check(shutter.data != nullptr, "Shutter did not load its array");
	} // Real production destructors must pair new[] with delete[].
	fs::remove(file);
}

void corrupt(const char *input, const char *output) {
	std::ifstream source(input, std::ios::binary);
	std::string bytes{std::istreambuf_iterator<char>(source), {}};
	const auto offset = bytes.find(unicode_text);
	check(offset != std::string::npos, "Cannot find UTF-8 payload in real SCB output");
	bytes[offset] = '\xff';
	std::ofstream destination(output, std::ios::binary);
	destination.write(bytes.data(), bytes.size());
	check(destination.good(), "Cannot create invalid SCB fixture");
}

void packed_buffer() {
	// Real SCB integers follow a 13-byte signature/header, not aligned addresses.
	alignas(double) std::array<char, 32> bytes{};
	const char prefix = 1;
	const int integer = 0x12345678;
	const double real = 1.25;
	XBuffer buffer(bytes.data(), bytes.size());
	buffer < prefix < integer < real;
	check(
		std::memcmp(bytes.data() + 1, &integer, sizeof integer) == 0, "Packed integer bytes changed"
	);
	check(
		std::memcmp(bytes.data() + 1 + sizeof integer, &real, sizeof real) == 0,
		"Packed double bytes changed"
	);
	buffer.set(0);
	char read_prefix;
	int read_integer;
	double read_real;
	buffer > read_prefix > read_integer > read_real;
	check(
		read_prefix == prefix && read_integer == integer && read_real == real,
		"Unaligned round-trip failed"
	);
}

} // namespace

void actint_flow_test(bool native);

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
		if (argc == 2 && std::string(argv[1]) == "--description-switch") {
			actint_description_switch_test();
		} else if (argc == 3 && std::string(argv[1]) == "--flow") {
			actint_flow_test(std::string(argv[2]) == "ttf");
		} else if (argc == 3 && std::string(argv[1]) == "--render-menu") {
			actint_menu_render_test(std::string(argv[2]) == "ttf");
		} else if (argc == 3 && std::string(argv[1]) == "--render-counter") {
			actint_counter_render_test(std::string(argv[2]) == "ttf");
		} else if (argc == 5 && std::string(argv[1]) == "--comments") {
			comments(argv[2], argv[3], argv[4]);
		} else if (argc == 5 && std::string(argv[1]) == "--description-map") {
			description_map(argv[2], argv[3], argv[4]);
		} else if (argc == 5 && std::string(argv[1]) == "--parse") {
			parse(argv[2], argv[3], argv[4]);
		} else if (argc == 4 && std::string(argv[1]) == "--corrupt") {
			corrupt(argv[2], argv[3]);
		} else if (argc == 2 && std::string(argv[1]) == "--runtime") {
			mouse_prompt();
			prompt_clipping();
			resource_lifetimes();
			packed_buffer();
		} else if (argc == 3 && std::string(argv[1]) == "--invalid-api") {
			if (std::string(argv[2]) == "screen") {
				aciScreenText text;
				text.alloc_utf8("\xff");
			} else {
				aciPromptData prompt(1);
				prompt.add_utf8_str(0, "\xff");
			}
			throw std::runtime_error("Invalid API input was accepted");
		} else {
			throw std::runtime_error("Unknown test invocation");
		}
		return 0;
	} catch (const std::exception &error) {
		std::cerr << "TEST_FAILURE: " << error.what() << '\n';
		return 2;
	}
}
