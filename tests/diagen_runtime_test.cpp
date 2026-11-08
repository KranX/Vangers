#include "global.h"

#include "actint/actint.h"
#include "actint/item_api.h"
#include "lang.h"
#include "settings/text_encoding.h"
#include "units/uvsapi.h"
#include "uvs/diagen.h"
#include "uvs/univang.h"

extern actIntDispatcher *aScrDisp;

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
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
	bool ids = false, reverse_queries = false, text_link = false, no_topics = false;
	std::string main_id = "topic.main", linked_id = "topic.main";
	Language language;
	std::string topic, greeting, cycle, answer, choice, comment;
	Fixture(bool old_, Language l, bool ids_ = false): old(old_), ids(ids_), language(l) {
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
		if (ids) {
			topic = ru ? "Одинаковая подпись" : "Same label";
			greeting = ru ? "Приветствие" : "Greeting";
		}
	}
	std::string encode(const std::string &v) const {
		return old ? legacy(v) : v;
	}
	void write(bool invalid = false, bool source = false, bool empty_query = false) {
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
						ids && !text_link ? "{#" + linked_id + "}" : "{" + topic + "}",
						"[Variant]",
						greeting,
						"@?",
						"2",
						ids ? "9001" : "0",
						ids ? "9002" : "0",
						choice,
						comment,
						ids ? choice : "Other",
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
			std::vector<std::string> main = {"[" + (ids ? "#" + main_id : sub0) + "]",
				"[" + (ids ? topic : sub1) + "]",
				answer,
				"@1",
				answer + " 2"};
			if (ids)
				main.push_back("{#topic.other}");
			const std::vector<std::string> other = {
				"[#topic.other]", "[" + topic + "]", "Other answer"};
			std::vector<std::string> query{greeting, answer};
			const auto append = [&](const auto &values) {
				query.insert(query.end(), values.begin(), values.end());
			};
			if (ids && reverse_queries)
				append(other);
			append(main);
			if (ids && !reverse_queries)
				append(other);
			entries.emplace_back("data/" + id + ".query", tokens(query));
			if (empty_query && (i == 0 || no_topics))
				entries.back().second.clear();
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
void old_save(const Fixture &f, int version, int answer_index = 1) {
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
		const auto topic = version < 4 ? legacy(f.topic) : f.topic;
		out < 1 < short(topic.size()) < short(answer_index);
		out.write(topic.c_str(), topic.size() + 1);
	}
}
// Real resource parser, quest dispatcher and save reader; not a stand-alone ID mock.
void run_ids(Language language, [[maybe_unused]] bool compiled) {
	setLang(language);
	Fixture f(false, language, true);
#ifdef DIAGEN_TEST
	f.write(false, true);
#else
	if (!compiled)
		f.write();
#endif
	aScrDisp = new actIntDispatcher;
	dgD = new DiagenDispatcher;
	dgD->init();
	dgD->startSession("Room0");
	check(dgD->getNextPhrase(), "Missing initial phrase");
	check(
		std::string(dgD->findQfirst()) == f.topic && dgD->currentQuestionId() == f.main_id,
		"Display label became identity"
	);
	check(!dgD->getAnswerById(f.topic.c_str()), "By-ID selection accepted a display label");
	check(
		std::string(dgD->getAnswerById(f.main_id.c_str())) == f.answer + " 2",
		"ID selected wrong answer"
	);
	auto *room = dgD->seekR("Room0");
	check(
		room->seekQvisible("topic.main") && room->seekQvisible("topic.other"),
		"Explicit link lost a topic"
	);
	check(
		std::string(dgD->getAnswerById("topic.other")) == "Other answer",
		"Duplicate label selected wrong question"
	);
	int count = 0;
	for (auto *label = dgD->findQfirst(); label; label = dgD->findQnext()) {
		check(label == f.topic, "Duplicate-label fixture changed");
		check(!dgD->currentQuestionId().empty(), "Question iterator lost ID");
		++count;
	}
	check(count == 2, "Duplicate display labels collapsed");
	check(dgD->getNextPhrase() && dgD->varAtom, "Missing variant");
	check(
		std::string(dgD->findQfirst()) == f.choice && dgD->currentQuestionId() == "variant/0",
		"First variant ID lost"
	);
	check(
		std::string(dgD->findQnext()) == f.choice && dgD->currentQuestionId() == "variant/1",
		"Duplicate variant collapsed"
	);
	auto condition = tokens({"qcode", "==", "9002"});
	dgCell probe(room);
	probe.Access.buf = condition.data();
	probe.Access.len = int(condition.size());
	probe.Access.external = 1;
	check(!probe.isACCESS(), "Variant action was already applied");
	check(!dgD->getAnswerById("variant/99") && dgD->varAtom, "Invalid ID consumed a variant");
	check(!probe.isACCESS(), "Invalid variant ID changed quest action code");
	check(
		std::string(dgD->getAnswerById("variant/1")) == "Other comment" && !dgD->varAtom,
		"Duplicate variant label selected wrong comment/action"
	);
	check(probe.isACCESS(), "Duplicate variant label selected wrong quest action code");
	const int status = dgD->eStatus, visits = room->visitCounter, comings = room->comingCounter;
	const std::vector<uchar> cells(room->status, room->status + room->gridSX * room->gridSY);
	{
		XStream out("ids.sav", XS_OUT);
		dgD->save(out);
	}
	delete dgD;
#ifndef DIAGEN_TEST
	// Rename every display label, switch language and reorder definitions.
	const auto target = language == RUSSIAN ? ENGLISH : RUSSIAN;
	Fixture translated(false, target, true);
	translated.topic += " renamed";
	translated.reverse_queries = true;
	translated.write();
	setLang(target);
	dgD = new DiagenDispatcher;
	dgD->init();
	{
		XStream in("ids.sav", XS_IN);
		dgD->load(in);
	}
	room = dgD->seekR("Room0");
	auto *main = room->seekQvisible("topic.main"), *other = room->seekQvisible("topic.other");
	check(
		main && other && main->lindex == 1 && other->lindex == 0,
		"Language/order change lost question progress"
	);
	check(
		std::string(main->subj) == translated.topic && std::string(other->subj) == translated.topic,
		"Stale saved labels replaced translation"
	);
	check(
		dgD->eStatus == status && room->visitCounter == visits && room->comingCounter == comings &&
			std::vector<uchar>(room->status, room->status + room->gridSX * room->gridSY) == cells,
		"Language change altered quest state"
	);
	dgD->startSession("Room0");
	check(
		std::string(dgD->getAnswerById("topic.main")) == translated.answer + " 2",
		"Reloaded ID selected old-language answer"
	);
	delete dgD;
	// Frozen aliases still import v2/v3 CP1251 and v4 UTF8 saves after labels are replaced
	// entirely.
	translated.main_id = translated.linked_id = "topic.mechos";
	translated.topic =
		"mechos"; // Both labels collide with an old EN save key; only frozen ID matches.
	translated.write();
	for (int version : {2, 3, 4}) {
		for (const auto old_language : {RUSSIAN, ENGLISH}) {
			Fixture old(true, old_language);
			old.topic = old_language == RUSSIAN ? "мехос" : "mechos";
			old_save(old, version);
			dgD = new DiagenDispatcher;
			dgD->init();
			{
				XStream in("old-diagen.sav", XS_IN);
				dgD->load(in);
			}
			room = dgD->seekR("Room0");
			auto *q = room->seekQvisible("topic.mechos");
			check(
				q && q->lindex == 1 && std::string(q->subj) == translated.topic,
				"Localized old save did not import into renamed explicit-ID resource"
			);
			check(
				room->visitCounter == 3 && room->comingCounter == 4 &&
					room->status[0] == DG_CELLSTATUS::USED,
				"Old save import changed quest progress"
			);
			delete dgD;
		}
	}
#else
	(void)status;
	(void)visits;
	(void)comings;
#endif
	delete aScrDisp;
	aScrDisp = nullptr;
}

void run_empty(bool old, [[maybe_unused]] bool compiled, bool all = false) {
	setLang(ENGLISH);
	Fixture f(old, ENGLISH);
	f.no_topics = all;
#ifdef DIAGEN_TEST
	f.write(false, true, true);
#else
	if (!compiled)
		f.write(false, false, true);
#endif
	dgD = new DiagenDispatcher;
	dgD->init();
	const auto *room = dgD->seekR("Room0");
	check(
		!room->prefixQ && !room->postfixQ && !room->qAtail && !room->qVtail,
		"Empty question file did not stay empty"
	);
	if (all) {
		for (auto *r = dgD->rtail; r; r = r->next)
			check(!r->qAtail && !r->qVtail, "Empty topic set became visible questions");
	} else
		check(dgD->seekR("Room1")->qAtail != nullptr, "Empty entry corrupted the following room");
	delete dgD;
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
		check(version == 5, "New save lacks stable topic IDs");
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
		for (int version : {2, 3, 4}) {
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
using Model = std::map<std::string, std::vector<std::string>>;
std::string text(const char *s) {
	return s ? s : "";
}
void append_file(std::vector<std::string> &values, const dgFile &file) {
	std::vector<std::string> tokens;
	for (int i = 0; file.buf && i < file.len;) {
		if (file.buf[i]) {
			tokens.emplace_back(file.buf + i);
			i += int(tokens.back().size()) + 1;
		} else
			++i;
	}
	values.push_back(std::to_string(tokens.size()));
	values.insert(values.end(), tokens.begin(), tokens.end());
}
std::string variables(const char *input) {
	const auto s = text(input);
	std::set<std::string> ids;
	for (auto pos = s.find("$("); pos != std::string::npos; pos = s.find("$(", pos + 2)) {
		const auto end = s.find(')', pos + 2);
		check(end != std::string::npos, "Unclosed dialogue substitution");
		ids.insert(s.substr(pos, end - pos + 1));
	}
	std::string result;
	for (const auto &id : ids)
		result += id + '\n';
	return result;
}
Model model(bool explicit_topics, bool include_text = true) {
	Model result;
	const auto project = [&](const char *s) { return include_text ? text(s) : variables(s); };
	const auto atom = [&](const std::string &key, dgAtom *a, bool raw_links = true) {
		auto &v = result[key];
		v = {project(a->data), std::to_string(a->nvariant)};
		for (int i = 0; i < a->nvariant; ++i) {
			v.push_back(std::to_string(a->vcodes[i]));
			v.push_back(project(a->variants[i]));
			v.push_back(project(a->comments[i]));
		}
		v.push_back(std::to_string(a->question_ids.size()));
		v.insert(v.end(), a->question_ids.begin(), a->question_ids.end());
		for (auto *link = a->findQLfirst(); link; link = a->findQLnext()) {
			if (explicit_topics)
				check(*link == '#', "Unconverted shipped text link");
		}
		if (!explicit_topics && raw_links) {
			std::vector<std::string> old_targets;
			for (auto *label = a->findQLfirst(); label; label = a->findQLnext())
				for (auto *r = dgD->rtail; r; r = r->next)
					for (auto *q = r->qAtail; q; q = q->next)
						if (!strcmp(q->subj, label) &&
							std::find(old_targets.begin(), old_targets.end(), q->id) ==
								old_targets.end())
							old_targets.push_back(q->id);
			check(
				old_targets == a->question_ids,
				"Legacy adapter changed question opening order/targets"
			);
		}
	};
	const auto molecules = [&](dgRoom *r) {
		for (auto *m = r->mtail; m; m = m->next) {
			const auto key = std::string(r->roomName) + "/molecule/" + m->name;
			result[key] = {std::to_string(m->mood)};
			int index = 0;
			for (auto *a = m->tail; a; a = a->next)
				atom(key + "/" + std::to_string(index++), a);
		}
	};
	int rooms = 0, queries = 0;
	for (auto *r = dgD->rtail; r; r = r->next) {
		++rooms;
		const auto key = std::string(r->roomName);
		result[key] = {project(r->prefixQ),
			project(r->postfixQ),
			std::to_string(r->bios),
			std::to_string(r->repeatable),
			std::to_string(r->gridSX),
			std::to_string(r->gridSY),
			std::to_string(r->startX),
			std::to_string(r->startY)};
		for (int i = 0; i < 2; ++i) {
			result[key].push_back(text(r->escaveNames[i]));
			result[key].push_back(text(r->counsillorNames[i]));
			result[key].push_back(text(r->worldNames[i]));
		}
		for (int i = 0; i < r->gridSX * r->gridSY; ++i) {
			auto *cell = r->grid[i];
			auto &v = result[key + "/cell/" + std::to_string(i)];
			v = {std::to_string(r->status[i])};
			if (!cell)
				continue;
			v.insert(
				v.end(),
				{text(cell->Name),
					std::to_string(cell->Type),
					std::to_string(cell->isWaiting),
					std::to_string(cell->isLooping),
					std::to_string(cell->x),
					std::to_string(cell->y)}
			);
			append_file(v, cell->Access);
			append_file(v, cell->PostCMD);
			append_file(v, cell->StartCMD);
		}
		molecules(r);
		for (auto *q = r->qAtail; q; q = q->next) {
			++queries;
			check(
				q->explicit_id == explicit_topics && q->id.rfind("topic.", 0) == 0,
				"Shipped topic still derives identity from a display header"
			);
			const auto qkey = key + "/query/" + q->id;
			result[qkey] = {project(q->subj), std::to_string(q->mood), std::to_string(q->nanswer)};
			for (int i = 0; i < q->nanswer; ++i) {
				result[qkey].push_back(std::to_string(q->levels[i]));
				atom(qkey + "/" + std::to_string(i), q->answers[i], q->legacy_answers.empty());
			}
		}
	}
	molecules(dgD->invR);
	for (int l = 0; l < 2; ++l) {
		for (int j = 0; j < 3; ++j)
			for (int i = 0; i < 3; ++i)
				result["cycles"].push_back(text(dgD->CycleName[l][j][i]));
		for (int i = 0; i < DG_SECRETW_MAX; ++i)
			result["worlds"].push_back(text(dgD->SecretWname[l][i]));
	}
	check(rooms == 9 && queries == 619, "Incomplete shipped dialogue resource");
	return result;
}
Model progress() {
	Model result;
	for (auto *r = dgD->rtail; r; r = r->next) {
		const auto key = std::string(r->roomName);
		result[key] = {std::to_string(r->visitCounter), std::to_string(r->comingCounter)};
		for (int i = 0; i < r->gridSX * r->gridSY; ++i)
			result[key].push_back(std::to_string(r->status[i]));
		for (auto *q = r->qVtail; q; q = q->next)
			result[key + "/" + q->id] = {std::to_string(q->lindex)};
	}
	return result;
}
void compare(const Model &expected, const Model &actual, const char *message) {
	if (expected == actual)
		return;
	for (const auto &[key, value] : expected) {
		const auto found = actual.find(key);
		if (found == actual.end() || found->second != value) {
			if (found != actual.end()) {
				for (std::size_t i = 0; i < std::min(value.size(), found->second.size()); ++i)
					if (value[i] != found->second[i])
						std::cerr << key << " field " << i << " before=[" << value[i] << "] after=["
								  << found->second[i] << "]\n";
				std::cerr << "record sizes: " << value.size() << " -> " << found->second.size()
						  << '\n';
			}
			throw std::runtime_error(std::string(message) + ": " + key);
		}
	}
	throw std::runtime_error(std::string(message) + ": unexpected extra records");
}
void write_old_shipped_save(int version, Language language) {
	const auto filename = "shipped-v" + std::to_string(version) + ".sav";
	XStream out(filename.c_str(), XS_OUT);
	if (version == 5) {
		dgD->save(out);
		return;
	}
	out < version < 0 < 0 < 0 < 0 < 1 < 0;
	if (version >= 3)
		out < 0;
	out < 9;
	for (auto *r = dgD->rtail; r; r = r->next) {
		const std::string name = r->roomName;
		out < short(name.size());
		out.write(name.c_str(), name.size() + 1);
		out < r->visitCounter < r->comingCounter;
		for (int state = DG_CELLSTATUS::USED; state <= DG_CELLSTATUS::EXPLODED; ++state) {
			std::vector<std::string> cells;
			for (int i = 0; i < r->gridSX * r->gridSY; ++i)
				if (r->status[i] == state)
					cells.push_back(r->grid[i]->Name);
			out < short(cells.size());
			for (const auto &cell : cells) {
				out < (uchar)cell.size();
				out.write(cell.c_str(), cell.size() + 1);
			}
		}
		int count = 0;
		for (auto *q = r->qVtail; q; q = q->next)
			++count;
		out < count;
		for (auto *q = r->qVtail; q; q = q->next) {
			auto key = std::string(language == RUSSIAN ? q->subj0 : q->subj1);
			if (version < 4)
				key = legacy(key);
			out < short(key.size()) < short(q->lindex);
			out.write(key.c_str(), key.size() + 1);
		}
	}
}
Model correctRussianChoices(Model result) {
	for (const auto *name :
		{"Lampasso/molecule/Kernowar Waiting?", "Ogorod/molecule/Pipiska Waiting?"}) {
		const std::string key = name;
		auto &v = result.at(key + "/0");
		check(v[1] == "2" && v[2] == "6" && v[5] == "0", "Unexpected legacy choice layout");
		const auto count = std::stoi(v[8]);
		if (key[0] == 'L') {
			check(v[6] == "{Жабли}", "Unexpected Jablees literal");
			v[6] = v[7];
			v[7] = result.at(key + "/1")[0];
			v[8] = std::to_string(count + 1);
			v.push_back("topic.jablees");
		} else {
			check(v[6] == "{Обурец}" && v[7] == "{Бюро}", "Unexpected Ogorod literals");
			v[6] = result.at(key + "/1")[0];
			v[7] = result.at(key + "/2")[0];
			v[8] = std::to_string(count + 2);
			v.push_back("topic.oboorez");
			v.push_back("topic.buro");
			result.erase(key + "/2");
		}
		result.erase(key + "/1");
	}
	return result;
}
std::string normalize_space(const std::string &text) {
	std::string result;
	bool gap = false;
	for (unsigned char c : text) {
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			gap = !result.empty();
		else {
			if (gap)
				result += ' ';
			result += char(c);
			gap = false;
		}
	}
	return result;
}
Model englishWords(const Model &input, bool old) {
	std::map<std::string, std::map<int, std::vector<std::string>>> objects;
	for (const auto &[key, value] : input) {
		const auto last = key.rfind('/');
		if (last == std::string::npos || (key.find("/query/") == std::string::npos &&
											 key.find("/molecule/") == std::string::npos))
			continue;
		const auto index = key.substr(last + 1);
		if (index.empty() || !std::all_of(index.begin(), index.end(), [](unsigned char c) {
				return c >= '0' && c <= '9';
			}))
			continue;
		objects[key.substr(0, last)][std::stoi(index)] = value;
	}
	Model result;
	for (const auto &[key, atoms] : objects) {
		std::string joined;
		for (const auto &[index, v] : atoms) {
			if (old && ((key == "VigBoo/molecule/Good Larva Delivery" && index == 4) ||
						   (key == "VigBoo/molecule/Shrub in Inventory (Boond)" && index == 3))) {
				check(v[0] == atoms.at(index + 1)[0], "Expected duplicated English phrase changed");
				continue;
			}
			joined += v[0] + ' ';
			for (int i = 0; i < std::stoi(v[1]); ++i)
				for (int field : {3 + 3 * i, 4 + 3 * i}) {
					const auto &t = v[field];
					if (old &&
						(key == "Lampasso/molecule/Kernowar Waiting?" ||
							key == "Ogorod/molecule/Pipiska Waiting?") &&
						(t == "{Jablees}" || t == "{Oboorez}" || t == "{The Buro}"))
						continue;
					joined += t + ' ';
				}
		}
		if (old && key == "Inventory/molecule/Tabutask P13") {
			const auto pos = joined.find("five jars of toxick");
			check(pos != std::string::npos, "Original P13 quantity changed");
			joined.replace(pos, 4, "six");
		}
		result[key] = {normalize_space(joined)};
	}
	return result;
}
void compareEnglishWording(const Model &before, const Model &after) {
	compare(
		englishWords(before, true),
		englishWords(after, false),
		"English wording changed beyond reviewed repairs"
	);
	// Labels and prefix/postfix are not rewritten by regrouping dialogue atoms.
	for (const auto &[key, value] : before) {
		if (key.find("/query/") != std::string::npos && key.rfind('/') == key.find("/query/") + 6)
			check(after.at(key)[0] == value[0], "English topic label changed");
		if (key.find('/') == std::string::npos)
			compare({{key, value}}, {{key, after.at(key)}}, "Shared metadata/prefix changed");
	}
}
void run_canonical() {
	setLang(RUSSIAN);
	dgD = new DiagenDispatcher;
	dgD->init();
	const auto ru = model(true, false);
	delete dgD;
	setLang(ENGLISH);
	dgD = new DiagenDispatcher;
	dgD->init();
	compare(ru, model(true, false), "English structure differs from Russian canon");
	std::set<std::string> ids;
	for (auto *r = dgD->rtail; r; r = r->next)
		for (auto *q = r->qAtail; q; q = q->next)
			ids.insert(q->id);
	check(ids.size() == 89, "English articles split a Russian-canonical concept");
	dgAtom soup;
	soup.question_ids = {"topic.creature_soup"};
	dgD->involveQ(&soup);
	auto *intro = dgD->seekR("Spobs")->seekM("Introduction")->tail;
	for (int i = 0; i < 3; ++i)
		intro = intro->next;
	dgD->involveQ(intro);
	int soup_rooms = 0, spectator_rooms = 0;
	for (auto *r = dgD->rtail; r; r = r->next) {
		soup_rooms += r->seekQvisible("topic.creature_soup") != nullptr;
		spectator_rooms += r->seekQvisible("topic.spectators") != nullptr;
	}
	check(
		soup_rooms == 7 && spectator_rooms == 7,
		"Repaired EN links did not open the canonical questions"
	);
	delete dgD;
	for (auto language : {RUSSIAN, ENGLISH}) {
		setLang(language);
		dgD = new DiagenDispatcher;
		dgD->init();
		aScrDisp = new actIntDispatcher;
		for (const auto *room : {"Lampasso", "Ogorod"}) {
			auto *m = dgD->seekR(room)->seekM(
				!strcmp(room, "Lampasso") ? "Kernowar Waiting?" : "Pipiska Waiting?"
			);
			check(
				m && m->tail && !m->tail->next && m->tail->nvariant == 2,
				"Malformed stock choice remains"
			);
			auto *a = m->tail;
			char *comment = nullptr;
			check(
				a->getVcode(0, comment) == 6 && comment == a->comments[0],
				"Yes action/comment changed"
			);
			check(
				a->getVcode(1, comment) == 0 && comment == a->comments[1],
				"No action/comment changed"
			);
			dgD->varAtom = a;
			check(
				*dgD->findQfirst() != '{' && *dgD->findQnext() != '{',
				"A link became a choice label"
			);
			check(
				dgD->getAnswerById("variant/1") == a->comments[1] && !dgD->varAtom,
				"Declining consumed wrong choice"
			);
		}
		delete aScrDisp;
		aScrDisp = nullptr;
		delete dgD;
	}
}
void run_legacy_indices(const std::filesystem::path &root) {
	struct Expected {
		std::string room, id;
		int index;
	};
	std::map<std::string, std::vector<Expected>> files;
	std::ifstream manifest(root / "english-indices.tsv");
	check(manifest.good(), "Missing legacy index fixture manifest");
	std::string line;
	while (std::getline(manifest, line)) {
		std::istringstream in(line);
		std::string filename, room, id;
		int old, canonical;
		check(
			bool(in >> filename >> room >> id >> old >> canonical), "Malformed legacy index case"
		);
		files[filename].push_back({room, id, canonical});
	}
	check(files.size() == 12, "Incomplete legacy index matrix");
	for (auto language : {RUSSIAN, ENGLISH})
		for (const auto &[file, cases] : files) {
			setLang(language);
			dgD = new DiagenDispatcher;
			dgD->init();
			{
				XStream in((root / file).string().c_str(), XS_IN);
				dgD->load(in);
			}
			const auto verify = [&] {
				for (const auto &entry : cases) {
					auto *q = dgD->seekR(entry.room.c_str())->seekQvisible(entry.id.c_str());
					check(
						q && q->lindex == entry.index,
						"Legacy EN fragment index did not map to canonical progress"
					);
				}
			};
			verify();
			{
				XStream out("canonical-indices.sav", XS_OUT);
				dgD->save(out);
			}
			delete dgD;
			setLang(language == RUSSIAN ? ENGLISH : RUSSIAN);
			dgD = new DiagenDispatcher;
			dgD->init();
			{
				XStream in("canonical-indices.sav", XS_IN);
				dgD->load(in);
			}
			verify();
			delete dgD;
		}
}

void run_shipped(Language language, const std::filesystem::path &legacy_root) {
	const auto cwd = std::filesystem::current_path();
	setLang(language);
	std::filesystem::current_path(legacy_root);
	dgD = new DiagenDispatcher;
	dgD->init();
	std::filesystem::current_path(cwd);
	const auto before = model(false);
	dgAtom links;
	for (auto *r = dgD->rtail; r; r = r->next)
		for (auto *q = r->qAtail; q; q = q->next)
			links.question_ids.push_back(q->id);
	dgD->involveQ(&links);
	for (auto *r = dgD->rtail; r; r = r->next) {
		r->visitCounter = 3;
		r->comingCounter = 4;
		for (auto *q = r->qVtail; q; q = q->next)
			q->lindex = q->nanswer - 1;
		for (int i = 0; i < r->gridSX * r->gridSY; ++i)
			if (r->grid[i] && r->grid[i]->Name) {
				r->setCell(r->grid[i]->Name, DG_CELLSTATUS::USED);
				break;
			}
	}
	const auto saved = progress();
	for (int version : {2, 3, 4, 5}) {
		if (language == ENGLISH && version < 5)
			std::filesystem::copy_file(
				legacy_root / ("legacy-en-all-v" + std::to_string(version) + ".sav"),
				"shipped-v" + std::to_string(version) + ".sav",
				std::filesystem::copy_options::overwrite_existing
			);
		else
			write_old_shipped_save(version, language);
	}
	delete dgD;
	dgD = new DiagenDispatcher;
	dgD->init();
	if (language == RUSSIAN)
		compare(
			correctRussianChoices(before),
			model(true),
			"Russian canonical corpus changed unexpectedly"
		);
	else
		compareEnglishWording(before, model(true));
	delete dgD;
	const auto old_progress = [&](Language target, const std::string &filename) {
		setLang(target);
		std::filesystem::current_path(legacy_root);
		dgD = new DiagenDispatcher;
		dgD->init();
		std::filesystem::current_path(cwd);
		{
			XStream in(filename.c_str(), XS_IN);
			dgD->load(in);
		}
		auto result = progress();
		delete dgD;
		// The historical grid format saves by molecule name, not by cell index.
		// Compare representable cell state with the legacy reader, but independently
		// require every original topic/index to survive, not merely match that reader.
		for (const auto &[key, values] : saved)
			if (key.find('/') != std::string::npos)
				check(
					result.count(key) && result.at(key) == values,
					"Legacy save import lost a topic/index"
				);
		return result;
	};
	for (int version : {2, 3, 4, 5}) {
		const auto filename = "shipped-v" + std::to_string(version) + ".sav";
		const auto target = language == RUSSIAN ? ENGLISH : RUSSIAN;
		const auto expected = old_progress(target, filename);
		dgD = new DiagenDispatcher;
		dgD->init();
		{
			XStream in(filename.c_str(), XS_IN);
			dgD->load(in);
		}
		compare(
			expected, progress(), "Old resource/save migration to another locale lost progress"
		);
		for (auto *r = dgD->rtail; r; r = r->next) {
			r->updateLevel();
			for (auto *q = r->qVtail; q; q = q->next)
				if (q->lindex >= q->nanswer) {
					const int index = q->lindex;
					check(
						q->getIndex() == q->nanswer - 1 && q->lindex == index,
						"Short translation lost answer progress or indexed out of bounds"
					);
				}
		}
		{
			XStream out("shipped-return.sav", XS_OUT);
			dgD->save(out);
		}
		delete dgD;
		const auto expected_return = old_progress(language, filename);
		dgD = new DiagenDispatcher;
		dgD->init();
		{
			XStream in("shipped-return.sav", XS_IN);
			dgD->load(in);
		}
		compare(
			expected_return, progress(), "Returning to original locale lost answer/cell progress"
		);
		delete dgD;
	}
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
		if (mode == "ids") {
			run_ids(
				argc > 2 && std::string(argv[2]) == "ru" ? RUSSIAN : ENGLISH,
				argc > 3 && std::string(argv[3]) == "compiled"
			);
			return 0;
		}

		if (mode == "bad-id-empty" || mode == "bad-id-colon" || mode == "bad-id-unicode" ||
			mode == "bad-id-reserved" || mode == "bad-id-length" || mode == "duplicate-id" ||
			mode == "unknown-link" || mode == "unkeyed-link" || mode == "saved-id" ||
			mode == "saved-index" || mode == "ambiguous-save") {
			setLang(ENGLISH);
			Fixture f(false, ENGLISH, true);
			if (mode == "bad-id-empty")
				f.main_id = "";
			if (mode == "bad-id-colon")
				f.main_id = "topic:bad";
			if (mode == "bad-id-unicode")
				f.main_id = "тема";
			if (mode == "bad-id-reserved")
				f.main_id = "variant/0";
			if (mode == "bad-id-length")
				f.main_id = std::string(32768, 'x');
			if (mode == "duplicate-id")
				f.main_id = "topic.other";
			if (mode == "unknown-link")
				f.linked_id = "topic.missing";
			if (mode == "unkeyed-link")
				f.text_link = true;
			f.write();
			dgD = new DiagenDispatcher;
			dgD->init();
			if (mode == "saved-id" || mode == "saved-index" || mode == "ambiguous-save") {
				if (mode != "ambiguous-save")
					f.topic = mode == "saved-id" ? "topic.missing" : "topic.main";
				old_save(f, mode == "ambiguous-save" ? 4 : 5, mode == "saved-index" ? -1 : 1);
				XStream in("old-diagen.sav", XS_IN);
				dgD->load(in);
			}
			return 2;
		}
		if (mode == "empty" || mode == "empty-legacy" || mode == "empty-all" ||
			mode == "empty-all-legacy") {
			run_empty(
				mode == "empty-legacy" || mode == "empty-all-legacy",
				argc > 3 && std::string(argv[3]) == "compiled",
				mode == "empty-all" || mode == "empty-all-legacy"
			);
			return 0;
		}
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
		if (mode == "canonical") {
			run_canonical();
			return 0;
		}
		if (mode == "legacy-indices") {
			check(argc > 2, "Missing index fixture directory");
			run_legacy_indices(argv[2]);
			return 0;
		}
		if (mode == "pack") {
			setLocale(argc > 2 ? argv[2] : "en");
			dgD = new DiagenDispatcher;
			dgD->init();
			delete dgD;
			return 0;
		}
		if (mode == "shipped") {
			check(argc > 3, "Missing legacy fixture directory");
			run_shipped(std::string(argv[2]) == "ru" ? RUSSIAN : ENGLISH, argv[3]);
			return 0;
		}
		if (mode == "invalid" || mode == "truncated" || mode == "signature" || mode == "count" ||
			mode == "unterminated" || mode == "negative-length") {
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
				} else if (mode == "negative-length") {
					file.seekg(6);
					const int name_length = file.get();
					file.seekp(8 + name_length);
					int length = -1;
					file.write((char *)&length, 4);
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
