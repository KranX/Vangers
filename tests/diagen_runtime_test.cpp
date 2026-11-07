#include "global.h"

#include "actint/actint.h"
#include "actint/item_api.h"
#include "lang.h"
#include "settings/text_encoding.h"
#include "units/uvsapi.h"
#include "uvs/diagen.h"
#include "uvs/univang.h"

extern actIntDispatcher *aScrDisp;

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using namespace vangers::settings;
void check(bool ok, const char *message) {
	if (!ok)
		throw std::runtime_error(message);
}
std::string tokens(const std::vector<std::string> &values) {
	std::string out;
	for (const auto &v : values) {
		out += v;
		out += '\0';
	}
	return out;
}
// Only the fixture's small Russian alphabet; production has a full CP1251 decoder.
std::string legacy(std::string_view text) {
	std::string out;
	const char *p = text.data();
	size_t left = text.size();
	while (left) {
		if (!*p) {
			out += '\0';
			++p;
			--left;
			continue;
		}
		auto c = SDL_StepUTF8(&p, &left);
		if (c < 128)
			out += char(c);
		else if (c >= 0x410 && c <= 0x44f)
			out += char(c - 0x410 + 0xc0);
		else if (c == 0x401)
			out += char(0xa8);
		else if (c == 0x451)
			out += char(0xb8);
		else if (c == 0x2116)
			out += char(0xb9);
		else
			throw std::runtime_error("Unencodable fixture");
	}
	return out;
}
struct Fixture {
	bool old;
	Language language;
	std::string topic, greeting, cycle, answer, choice, comment;
	Fixture(bool old_, Language l): old(old_), language(l) {
		const bool ru = l == RUSSIAN;
		topic = old ? (ru ? "Ёж №" : "Question") : "Äöüß Ёж 日本 😀" + std::string(400, 'x');
		greeting = old ? (ru ? "Ёж говорит" : "Welcome back") : "Äöüß Ёж 日本 😀";
		if (!old) {
			auto unit = greeting;
			for (int i = 0; i < 100; ++i)
				greeting += unit;
		}
		cycle =
			old ? (ru ? "Длинное имя цикла для подстановки Ёё" : "Long cycle name for replacement")
				: "Длинный цикл 日本 😀" + std::string(120, 'x');
		answer = old ? (ru ? "Ответ Ёж" : "Answer") : "Antwort 日本 😀";
		choice = old ? (ru ? "Выбор" : "Choice") : "Wählen 日本 😀";
		comment = old ? (ru ? "Комментарий" : "Comment") : "Gewählt 日本 😀";
	}
	std::string encode(const std::string &v) const {
		return old ? legacy(v) : v;
	}
	void write(bool invalid = false, bool source = false) {
		std::vector<std::pair<std::string, std::string>> entries;
		std::vector<std::string> rooms;
		for (int i = 0; i < 9; ++i) {
			auto id = "Room" + std::to_string(i);
			std::vector<std::string> row = {"#" + id,
				"Комната",
				"#Councillor",
				"Советник",
				"0",
				"#World" + std::to_string(i),
				"Мир",
				"0",
				"0",
				"0",
				"0",
				"0"};
			rooms.insert(rooms.end(), row.begin(), row.end());
			entries.emplace_back(
				"data/" + id + ".text",
				tokens(
					{"[Greeting]",
						greeting + " $(Cycle Name of Locked Larva)",
						"{" + topic + "}",
						"[Variant]",
						greeting,
						"@?",
						"2",
						"0",
						"0",
						choice,
						comment,
						"Other",
						"Other comment",
						"[Empty Question]",
						answer,
						"[Empty Molecule]",
						answer,
						"[Block Molecule]",
						answer,
						"[Out Molecule]",
						answer,
						"[Dead Molecule]",
						answer}
				)
			);
			entries.emplace_back(
				"data/" + id + ".dil",
				tokens(
					{"2",
						"1",
						"2",
						"0",
						"0",
						"0",
						"Greeting",
						"0",
						"0",
						"0",
						"true",
						" ",
						"ePromotion",
						"1",
						"0",
						"Variant",
						"0",
						"0",
						"0",
						"true",
						" ",
						" "}
				)
			);
			const auto sub0 = old ? std::string("Ёж №") : topic,
					   sub1 = old ? std::string("Question") : topic;
			entries.emplace_back(
				"data/" + id + ".query",
				tokens(
					{greeting,
						answer,
						"[" + sub0 + "]",
						"[" + sub1 + "]",
						answer,
						"@1",
						answer + " 2"}
				)
			);
		}
		for (int j = 0; j < 3; ++j) {
			for (int i = 0; i < 3; ++i)
				rooms.push_back("#" + cycle);
			for (int i = 0; i < 3; ++i)
				rooms.push_back(cycle);
		}
		for (int i = 0; i < 6; ++i)
			rooms.push_back("#Secret" + std::to_string(i));
		for (int i = 0; i < 6; ++i)
			rooms.push_back("Тайный мир");
		entries.emplace_back("data/room.lst", tokens(rooms));
		entries.emplace_back(
			"data/Inventory.text", tokens({"[Final 1]", greeting, "[Poponka 1]", answer})
		);
		if (source) {
			std::filesystem::create_directories("data");
			for (const auto &[name, data] : entries) {
				std::ofstream f(name, std::ios::binary);
				// Explicit single-language source: no alphabet-based language guessing.
				f << "utf8_text\n";
				for (std::size_t pos = 0; pos < data.size();) {
					std::string token = data.c_str() + pos;
					pos += token.size() + 1;
					if (token.front() == '[' || token.front() == '{')
						f << token << '\n';
					else
						f << '"' << token << "\"\n";
				}
				check(f.good(), "Fixture source write failed");
			}
			return;
		}
		std::ofstream f(language == RUSSIAN ? "diagen.text" : "diagen_eng.text", std::ios::binary);
		f.write(old ? "DG" : "DU", 2);
		int count = entries.size();
		f.write((char *)&count, 4);
		for (auto &[name, data] : entries) {
			f.put(char(name.size()));
			f.write(name.c_str(), name.size() + 1);
			auto value = encode(data);
			if (invalid && name == "data/Inventory.text")
				value[10] = char(0xff);
			int size = value.size();
			f.write((char *)&size, 4);
			f.write(value.data(), value.size());
		}
		check(f.good(), "Fixture archive write failed");
	}
};
void old_save(const Fixture &f, int version) {
	XStream out("old-diagen.sav", XS_OUT);
	out < version < 0 < 0 < 0 < 0 < 1 < 0;
	if (version >= 3)
		out < 0;
	out < 9;
	for (int i = 0; i < 9; ++i) {
		auto id = "Room" + std::to_string(i);
		out < short(id.size());
		out.write(id.c_str(), id.size() + 1);
		out < 3 < 4;
		for (int state = DG_CELLSTATUS::USED; state <= DG_CELLSTATUS::EXPLODED; ++state) {
			out < short(state == DG_CELLSTATUS::USED ? 1 : 0);
			if (state == DG_CELLSTATUS::USED) {
				out < (uchar)8;
				out.write("Greeting", 9);
			}
		}
		const auto topic = legacy(f.topic);
		out < 1 < short(topic.size()) < short(1);
		out.write(topic.c_str(), topic.size() + 1);
	}
}
void run(bool old, Language language, [[maybe_unused]] bool compiled) {
	setLang(language);
	Fixture f(old, language);
#ifdef DIAGEN_TEST
	f.write(false, true);
#else
	if (!compiled)
		f.write();
#endif
	aScrDisp = new actIntDispatcher;
	dgD = new DiagenDispatcher;
	dgD->init();
	auto *r = dgD->seekR("Room0");
	check(
		std::string(r->counsillorNames[1]) == "Советник", "Legacy shared name table did not decode"
	);
	GamerResult.dolly_cycle = 0;
	dgD->startSession("Room0");
	check(
		std::string(dgD->getNextPhrase()) == f.greeting + " " + f.cycle,
		"Phrase/expanding variable lost UTF-8"
	);
	check(dgD->eStatus == 1, "Startup command changed");
	check(
		std::string(dgD->findQfirst()) == f.topic && dgD->findQnext() == nullptr,
		"UTF-8 query links failed"
	);
	check(std::string(dgD->getQprefix()) == f.greeting, "Prefix lost UTF-8");
	auto subject = f.topic;
	check(
		std::string(dgD->getAnswer(subject.data())) == f.answer + " 2",
		"UTF-8 topic/answer selection failed"
	);
	check(dgD->getNextPhrase() && dgD->varAtom, "Variant molecule missing");
	check(std::string(dgD->findQfirst()) == f.choice, "Variant topic lost UTF-8");
	auto selection = f.choice;
	check(std::string(dgD->getAnswer(selection.data())) == f.comment, "Variant answer lost UTF-8");
	check(
		std::string(dgD->getInvText(DG_FINAL, 1)) == f.greeting, "Inventory/final text lost UTF-8"
	);
	aciPrepareTextUtf8(dgD->getInvText(DG_FINAL, 1));
	check(
		is_valid_utf8(
			std::string_view(aScrDisp->ScrTextData->data, aScrDisp->ScrTextData->dataSize)
		),
		"Full-screen consumer split UTF-8"
	);
	if (!old)
		check(
			std::string_view(aScrDisp->ScrTextData->data, aScrDisp->ScrTextData->dataSize)
					.find("日本") != std::string_view::npos,
			"Full-screen consumer transcoded Unicode"
		);
	// Save while the room is active: counters, visited cells, visible query and answer index.
	{
		XStream out("new-diagen.sav", XS_OUT);
		dgD->save(out);
	}
	const int wantedStatus = dgD->eStatus;
	const int visits = r->visitCounter, comings = r->comingCounter;
	const std::vector<uchar> cells(r->status, r->status + r->gridSX * r->gridSY);
	delete dgD;
	dgD = new DiagenDispatcher;
	dgD->init();
	{
		XStream in("new-diagen.sav", XS_IN);
		int version;
		in > version;
		check(version == 4, "New save lacks explicit UTF-8 version");
		in.seek(0, XS_BEG);
		dgD->load(in);
	}
	r = dgD->seekR("Room0");
	check(dgD->eStatus == wantedStatus, "Progress status changed after reload");
	check(
		r->visitCounter == visits && r->comingCounter == comings,
		"Visit/coming counters changed after reload"
	);
	check(
		std::vector<uchar>(r->status, r->status + r->gridSX * r->gridSY) == cells,
		"Cell progress changed after reload"
	);
	check(
		r->qVtail && std::string(r->qVtail->subj) == f.topic && r->qVtail->lindex == 1,
		"Visible query/answer progress changed"
	);
	if (old)
		for (int version : {2, 3}) {
			delete dgD;
			dgD = new DiagenDispatcher;
			dgD->init();
			old_save(f, version);
			{
				XStream in("old-diagen.sav", XS_IN);
				dgD->load(in);
			}
			r = dgD->seekR("Room0");
			check(r->visitCounter == 3 && r->comingCounter == 4, "Old counters changed");
			check(r->status[0] == DG_CELLSTATUS::USED, "Old cell state changed");
			check(
				r->qVtail && std::string(r->qVtail->subj) == f.topic && r->qVtail->lindex == 1,
				"Old CP1251 topic progress not restored"
			);
		}
	delete dgD;
	delete aScrDisp;
	aScrDisp = nullptr;
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
		const std::string mode = argc > 1 ? argv[1] : "utf8";
		if (mode == "invalid-source") {
			setLang(ENGLISH);
			Fixture(false, ENGLISH).write(false, true);
			{
				std::fstream f(
					"data/Inventory.text", std::ios::in | std::ios::out | std::ios::binary
				);
				f.seekp(25);
				f.put(char(0xff));
			}
			diagenPrepare();
			return 2;
		}
		if (mode == "shipped") {
			setLang(argc > 2 && std::string(argv[2]) == "ru" ? RUSSIAN : ENGLISH);
			dgD = new DiagenDispatcher;
			dgD->init();
			int rooms = 0, atoms = 0, queries = 0;
			for (auto *r = dgD->rtail; r; r = r->next) {
				++rooms;
				for (auto *m = r->mtail; m; m = m->next)
					for (auto *a = m->tail; a; a = a->next) {
						check(is_valid_utf8(a->data), "Invalid shipped phrase");
						++atoms;
					}
				for (auto *q = r->qAtail; q; q = q->next) {
					check(is_valid_utf8(q->subj), "Invalid shipped topic");
					++queries;
				}
			}
			check(rooms == 9 && atoms > 500 && queries > 500, "Incomplete shipped resource");
			std::cout << rooms << " rooms, " << atoms << " phrases, " << queries << " topics\n";
			delete dgD;
			return 0;
		}
		if (mode == "invalid" || mode == "truncated" || mode == "signature" || mode == "count" ||
			mode == "unterminated") {
			setLang(ENGLISH);
			Fixture(false, ENGLISH).write(mode == "invalid");
			if (mode == "truncated") {
				std::filesystem::resize_file("diagen_eng.text", 1000);
			} else if (mode != "invalid") {
				std::fstream file(
					"diagen_eng.text", std::ios::in | std::ios::out | std::ios::binary
				);
				if (mode == "signature")
					file.write("XX", 2);
				else if (mode == "count") {
					file.seekp(2);
					int count = -1;
					file.write((char *)&count, 4);
				} else {
					file.seekp(-1, std::ios::end);
					file.put('X');
				}
			}
			diagenPrepare();
			return 2;
		}
		run(mode == "legacy",
			argc > 2 && std::string(argv[2]) == "ru" ? RUSSIAN : ENGLISH,
			argc > 3 && std::string(argv[3]) == "compiled");
		return 0;
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return 2;
	}
}
