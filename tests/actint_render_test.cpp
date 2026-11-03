#include "global.h"

#include "actint/actint.h"
#include "iscreen/iscreen.h"
#include "settings/settings.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>

void aLoadFonts();
void aLoadFonts32();
void aFreeFonts();
extern aciFont **aScrFonts, **aScrFonts32;
extern unsigned char *aciCurColorScheme, *aCellFrame;
extern int aCellSize;
extern XBuffer *aciXConv;

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
		check(std::filesystem::create_directory(root), "Cannot create renderer fixture");
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
