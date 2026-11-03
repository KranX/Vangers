#include "global.h"

#include "actint/aci_scr.h"
#include "iscreen/hfont.h"
#include "saves/text.h"
#include "settings/settings.h"
#include "settings/text_encoding.h"
#include "text/utf8.h"
#include "text_layer.h"
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

void acsParseScript(const char *, const char *);
void acsLoadFonts();
void acsFreeFonts();
void acsCompressData(char *);
void acsDecompressData(char *);
void aciPutSpr32(int, int, int, int, void *, int, int);
int acsStrLen(int, unsigned char *, int);
extern char *acsBackupStr;
extern HFont **acsFntTable;
extern const char *aciSTR_ON, *aciSTR_OFF;

namespace {
void check(bool ok, const char *message) {
	if (!ok)
		throw std::runtime_error(message);
}
void cleanup() {
	while (acsScrD->scrList->fPtr) {
		auto *screen = static_cast<aciScreen *>(acsScrD->scrList->fPtr);
		acsScrD->scrList->RemoveElement(screen);
		delete screen;
	}
	delete acsScrD;
	acsScrD = nullptr;
}
void parse(const char *source, const char *binary, bool utf8) {
	acsParseScript(source, binary);
	acsScrD->curScr = acsScrD->GetScreen(1);
	auto *field = static_cast<aciScreenInputField *>(acsScrD->GetObject(2));
	check(
		field && std::string(field->string) == (utf8 ? "Ёж 日本 😀" : "Ёж"),
		"ACS source/SCB lost text"
	);
	check(
		field->MaxStrLen == 20 && (field->flags & ACS_ISCREEN_FONT),
		"ACS opcode compatibility changed"
	);
	cleanup();
}
void init_video() {
	std::filesystem::create_directories("resource/actint/hd");
	auto *surface = SDL_CreateSurface(8, 8, SDL_PIXELFORMAT_RGB24);
	check(
		surface && SDL_SaveBMP(surface, "resource/actint/hd/hd_background.bmp"),
		"Background fixture failed"
	);
	SDL_DestroySurface(surface);
	SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
	XGR_Init(0);
}

void fonts() {
	std::filesystem::create_directories("resource/actint/640x480/hfonts");
	for (int f = 0; f < 2; ++f) {
		std::ofstream file(
			"resource/actint/640x480/hfonts/font0" + std::to_string(f) + ".fnc", std::ios::binary
		);
		file.write("HFNT 1.01", 9);
		const auto word = [&](int value, int n) {
			for (int i = 0; i < n; ++i)
				file.put(char(value >> (i * 8)));
		};
		const int w = f ? 16 : 20, h = f ? 31 : 39;
		word(w, 2);
		word(h, 2);
		word(0, 2);
		word(255, 2);
		for (int cp = 0; cp < 256; ++cp) {
			word(w, 2);
			word(h, 2);
			const int flags = cp == 0 || cp == ' ' ? NULL_HCHAR : 0;
			word(flags, 4);
			if (!flags) {
				for (int y = 0; y < h; ++y)
					for (int x = 0; x < w; ++x)
						file.put(
							x > 2 && x < w - 2 && y > 4 && y < h - 4 ? char(x == 3 ? 1 : 10 + x) : 0
						);
			}
		}
		check(file.good(), "Font fixture failed");
	}
}
void rendering(bool ttf) {
	init_video();
	fonts();
	vangers::settings::settings_manager().get_mutable().video.ttf_text = ttf;
	xgr::text::enabled = ttf;
	acsLoadFonts();
	acsScrD = new aciScreenDispatcher;
	aciScreenInputField field;
	field.MaxStrLen = 20;
	field.alloc_str();
	field.set_string("Ёж");
	field.PosX = 30;
	field.PosY = 40;
	field.SizeX = 300;
	field.SizeY = 50;
	field.flags = ACS_ALIGN_CENTER | ACS_ISCREEN_FONT;
	field.Color = 48 | (1 << 8);
	field.Space = 1;
	for (int f = 0; f < 2; ++f) {
		field.font = f;
		XGR_Rectangle(0, 0, XGR_MAXX, XGR_MAXY, 0, 0, XGR_FILLED);
		field.redraw();
		auto *buffer = XGR_Obj.get_active_render_buffer();
		if (ttf) {
			auto *layer = xgr::text::find(buffer);
			check(layer && !layer->empty(), "TTF path produced no native layer");
			std::array<SDL_Color, 256> palette{};
			for (int c = 0; c < 256; ++c)
				palette[c] = {uint8_t(c), uint8_t(c), uint8_t(c), 255};
			const auto pixels = layer->render(XGR_MAXX, XGR_MAXY, palette.data());
			check(
				std::any_of(
					pixels.begin(),
					pixels.end(),
					[](auto p) { return (p >> 24) && (p & 255) == 63; }
				),
				"ACS palette ramp changed"
			);

			// Centering must not follow the different space/underscore advances.
			field.flags |= ACS_ACTIVE_STRING;
			field.set_string("Ёж_");
			XGR_Rectangle(0, 0, XGR_MAXX, XGR_MAXY, 0, 0, XGR_FILLED);
			field.redraw();
			const auto visible = layer->render(XGR_MAXX, XGR_MAXY, palette.data());
			field.string[std::strlen(field.string) - 1] = ' ';
			XGR_Rectangle(0, 0, XGR_MAXX, XGR_MAXY, 0, 0, XGR_FILLED);
			field.redraw();
			const auto hidden = layer->render(XGR_MAXX, XGR_MAXY, palette.data());
			for (size_t i = 0; i < hidden.size(); ++i)
				if (hidden[i])
					check(visible[i] == hidden[i], "Cursor blink moved centered text");
			field.flags &= ~ACS_ACTIVE_STRING;

			field.set_string("\1");
			XGR_Rectangle(0, 0, XGR_MAXX, XGR_MAXY, 0, 0, XGR_FILLED);
			field.redraw();
			const auto icon_pixels = layer->render(XGR_MAXX, XGR_MAXY, palette.data());
			check(
				std::none_of(
					icon_pixels.begin(),
					icon_pixels.end(),
					[](auto p) { return (p >> 24) && (p & 255) == 48; }
				),
				"ACS icon zero palette level must stay transparent"
			);
			field.set_string("日本😀");
			field.redraw();
			check(std::string(field.string) == "日本😀", "Missing glyph fallback mutated storage");
			field.set_string("Ёж");
		} else {
			const std::vector<unsigned char> actual(buffer, buffer + XGR_MAXX * XGR_MAXY);
			XGR_Rectangle(0, 0, XGR_MAXX, XGR_MAXY, 0, 0, XGR_FILLED);
			const auto encoded = vangers::settings::utf8_to_cp866(field.string);
			int width = 0;
			for (unsigned char c : encoded) {
				const auto *g = acsFntTable[f]->data[c];
				width += g->SizeX - g->LeftOffs - g->RightOffs + field.Space;
			}
			check(
				acsStrLen(f, (unsigned char *)field.string, field.Space) == width,
				"Bitmap width changed"
			);
			int x = field.PosX + (field.SizeX - width) / 2;
			for (unsigned char c : encoded) {
				auto *g = acsFntTable[f]->data[c];
				aciPutSpr32(x - g->LeftOffs, field.PosY, g->SizeX, g->SizeY, g->HeightMap, 48, 1);
				x += g->SizeX - g->LeftOffs - g->RightOffs + field.Space;
			}
			check(
				std::equal(actual.begin(), actual.end(), buffer),
				"Bitmap redraw is not pixel-identical"
			);
		}
	}
	if (ttf) {
		// A real ACS field restores only its BMO rectangle, not the whole screen.
		// Native layout padding must not retain a draw after that restore.
		// Palette zero is transparent in ACS object restoration.
		XGR_Rectangle(0, 0, XGR_MAXX, XGR_MAXY, 7, 7, XGR_FILLED);
		field.PosX = 200;
		field.SizeX = 120;
		field.SizeY = 40;
		aciScreenResourceBMO background;
		background.flags = ACS_BUILD_RESOURCE;
		background.OffsX = field.PosX;
		background.OffsY = field.PosY;
		background.SizeX = field.SizeX;
		background.SizeY = field.SizeY;
		background.load();
		field.resPtr = &background;
		auto *layer = xgr::text::find(XGR_Obj.get_active_render_buffer());
		for (int frame = 0; frame < 128; ++frame) {
			acsScrD->flushLine->clear();
			field.font = frame % 2;
			field.Color = (frame % 3 + 3) * 16 | (1 << 8);
			field.set_string(frame % 2 ? "ШШШШШШШШШШШШШШШШШШШ_" : "Ёж_");
			const int mode = (frame / 2) % 2 ? XGR_CLIP_ALL : XGR_CLIP_PUTSPR;
			const SDL_Rect parent{
				field.PosX + 10, field.PosY + 5, field.SizeX - 20, field.SizeY - 10};
			XGR_SetClip(parent.x, parent.y, parent.x + parent.w, parent.y + parent.h);
			XGR_SetClipMode(mode);
			field.redraw();
			check(layer && !layer->empty(), "BMO field produced no native text");
			check(
				XGR_Obj.ClipMode == mode && XGR_Obj.clipLeft == parent.x &&
					XGR_Obj.clipTop == parent.y && XGR_Obj.clipRight == parent.x + parent.w &&
					XGR_Obj.clipBottom == parent.y + parent.h,
				"Field changed caller's clipping state"
			);
			if (frame < 4) {
				std::array<SDL_Color, 256> palette{};
				const int scale = frame < 2 ? 1 : 2;
				const auto pixels =
					layer->render(XGR_MAXX * scale, XGR_MAXY * scale, palette.data());
				const SDL_Rect clip =
					mode == XGR_CLIP_ALL ? parent : SDL_Rect{0, 0, XGR_MAXX, XGR_MAXY};
				bool visible = false, overhang = false;
				for (int y = 0; y < XGR_MAXY * scale; ++y)
					for (int x = 0; x < XGR_MAXX * scale; ++x) {
						if (!(pixels[size_t(y) * XGR_MAXX * scale + x] >> 24))
							continue;
						visible = true;
						overhang |=
							x < field.PosX * scale || x >= (field.PosX + field.SizeX) * scale;
						check(
							x >= clip.x * scale && y >= clip.y * scale &&
								x < (clip.x + clip.w) * scale && y < (clip.y + clip.h) * scale,
							"Native text escaped the parent clip"
						);
					}
				check(visible, "Parent clip removed all text");
				if (frame == 1)
					check(overhang, "Field rectangle clipped overhanging letters");
			}
			field.set_string("");
			field.redraw();
			check(layer->empty(), "BMO restore retained stale native draws outside field");
		}
		field.set_string("Ёж");
		field.redraw();
		check(!field.nativeText.expired(), "Field lost its native draw handle");
		layer->clear();
		check(field.nativeText.expired(), "Field keeps a cleared native draw alive");
		field.resPtr = nullptr;
		background.free();
	}
	delete acsScrD;
	acsScrD = nullptr;
	acsFreeFonts();
	XGR_Finit();
	xgr::text::enabled = false;
	SDL_Quit();
}
void save_format() {
	const std::string name = "Ёж日本😀", description = "Место日, 1:00:00";
	const auto &n = name;
	const auto &d = description;
	const std::string payload(90000, 'G');
	char filename[] = "save-cycle.dat";
	XStream file(filename, XS_OUT);
	file < vangers::saves::text_header(n, d);
	file.write(n.data(), n.size());
	file.write(d.data(), d.size());
	file.write(payload.data(), payload.size());
	file.close();
	for (int cycle = 0; cycle < 2; ++cycle) {
		acsCompressData(filename);
		auto text = vangers::saves::read_text(filename);
		check(
			text && text->name == name && text->description == description,
			"Production compression lost Unicode"
		);
		acsDecompressData(filename);
		text = vangers::saves::read_text(filename);
		check(text && text->name == name, "Production decompression lost Unicode");
		file.open(filename, XS_IN);
		int header;
		file > header;
		// Compression flag changes must preserve the encoding flag and payload offsets.
		file.seek(4 + (header & 255) + ((header >> 16) & 255), XS_BEG);
		std::string restored(payload.size(), '\0');
		file.read(restored.data(), restored.size());
		file.close();
		check(restored == payload, "Legacy gameplay payload changed");
	}
}
void runtime() {
	init_video();
	acsScrD = new aciScreenDispatcher;
	auto *screen = new aciScreen;
	screen->ID = 1;
	acsScrD->AddScreen(screen);
	acsScrD->curScr = screen;
	auto *field = new aciScreenInputField;
	field->ID = 2;
	field->MaxStrLen = 20;
	field->alloc_str();
	screen->AddObject(field);
	const auto text = [&](const char *value) {
		SDL_Event e{};
		e.type = SDL_EVENT_TEXT_INPUT;
		e.text.text = value;
		acsScrD->InputQuant(&e);
	};
	const auto key = [&](SDL_Keycode value) {
		SDL_Event e{};
		e.type = SDL_EVENT_KEY_DOWN;
		e.key.key = value;
		acsScrD->InputQuant(&e);
	};
	for (int maximum : {0, 1, 20, 511}) {
		field->MaxStrLen = maximum;
		field->alloc_str();
		std::string full;
		for (int n = 0; n < maximum; ++n)
			full += "😀";
		field->set_string(full.c_str());
		acsScrD->PrepareInput(2);
		text("Ё日😀");
		check(std::string(field->string) == full + "_", "Input overflow/cursor corruption");
		key(SDLK_BACKSPACE);
		std::string less = full;
		vangers::text::pop_back(less);
		check(std::string(field->string) == less + "_", "Backspace split a scalar");
		key(SDLK_ESCAPE);
		check(std::string(field->string) == full && !acsBackupStr, "Cancel lost backup");
		acsScrD->PrepareInput(2);
		field->string[std::strlen(field->string) - 1] = ' ';
		key(SDLK_RETURN);
		check(std::string(field->string) == full && !acsBackupStr, "Blink/commit changed input");
	}
	field->MaxStrLen = 20;
	field->alloc_str();
	acsScrD->PrepareInput(2);
	text("Ёж 日本 😀");
	text("\xff");
	text("\n\t");
	key(SDLK_RETURN);
	check(std::string(field->string) == "Ёж 日本 😀", "Complete SDL event/validation failed");
	field->set_string(field->string + 2);
	check(std::string(field->string) == "ж 日本 😀", "Aliased set_string failed");
	aciSTR_ON = "ВКЛ";
	aciSTR_OFF = "ВЫКЛ";
	field->set_state(1);
	check(field->get_state() == 1 && std::string(field->string) == "ВКЛ", "Toggle is not UTF-8");
	field->change_state();
	check(field->get_state() == 0 && std::string(field->string) == "ВЫКЛ", "Toggle failed");
	cleanup();
	XGR_Finit();
	SDL_Quit();
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
		else if (argc == 3 && std::string(argv[1]) == "--render")
			rendering(std::string(argv[2]) == "ttf");
		else if (argc == 2 && std::string(argv[1]) == "--save")
			save_format();
		else if (argc == 2 && std::string(argv[1]) == "--runtime")
			runtime();
		else
			throw std::runtime_error("Unknown invocation");
		return 0;
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return 2;
	}
}
