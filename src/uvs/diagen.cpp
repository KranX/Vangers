/*
	Dialogue Generator Core Module
	(C)1997 by K-D Lab, VANGERS Project
	Creator: K-D Lab::KranK
*/

#include "../global.h"
#include "lang.h"
#include "localization/catalog.h"

#include "../common.h"
#include "../settings/text_encoding.h"
#include "diagen.h"

#include <algorithm>
#include <limits>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#ifdef DIAGEN_TEST
#	include <filesystem>
#endif

#include "../actint/credits.h"
#include "../actint/mlconsts.h"

#include "../3d/parser.h"
#include "../units/compas.h"
#include "../units/uvsapi.h"
#include "univang.h"

/*#ifdef DIAGEN_TEST
#	include "../diagen/inp.h"
#endif*/

#ifdef DIAGEN_TEST
// #define RUSSIAN_VERSION
// #define GERMAN_VERSION
#endif

#define STARTUP_TESTING
#define SAVE_SINGE_LANGUAGE

#define SINGLE_LANGUAGE

#define DGF_NONE 0
#define DGF_DUAL 1

#define SUBJ_SYMBOL '['
#define SUBJ_SYMBOL_RIGHT ']'
#define LINK_SYMBOL '{'
#define LINK_SYMBOL_RIGHT '}'
#define COMMAND_SYMBOL '@'

struct FileBox {
	int cN;
	char **names;
	char **data;
	int *lens;

	FileBox(): cN(0), names(nullptr), data(nullptr), lens(nullptr) {
#ifdef DIAGEN_TEST
		names = new char *[256]{};
		data = new char *[256]{};
		lens = new int[256]{};
#endif
	};

	~FileBox() {
		for (int i = 0; i < cN; ++i) {
			delete[] names[i];
			delete[] data[i];
		}
		delete[] names;
		delete[] data;
		delete[] lens;
	}
	void load(void);
	char *get(char *fname, int &len);
#ifdef DIAGEN_TEST
	void add(char *fname, char *buf, int len);
	void save(void);
#endif
};

/* --------------------------- PROTOTYPE SECTION --------------------------- */
extern int RACE_WAIT;
// extern int aciCurCredits;
extern int aciEscaveDead;
extern int aciEscaveEmpty;
extern int NetworkON;
extern int uvsCurrentWorldUnableBefore;

// #ifndef DIAGEN_TEST
extern uvsGamerResult GamerResult;
// #endif

void dg_SendEvent(int code);
void aciOpenWorldLink(int, int);
void uvsSetTownName(const char *_name_);
char *uvsGetLarvaWorld(int n);
int uvsgetDGdata(int code);
int getPreviousCycle(int cycle);
int isCycleExist(int cycle, char ew = 0);
int getWorldVisitStats(char wc);
int getPassangerStats(char efrom, char wto);
int getPassangerFrom(char efrom);
char *StringOfBoozeeniada(void);
int getSpobsState(void);
int getThreallState(void);
void addInfernalsKill2Protractor(void);
void addSpectorsKill2Messiah(void);
int isSpummyDeath(void);
void OpenCrypt(int world, int id);
void uvsBoorawchickZero(void);
int aciWorldLinkExist(int w1, int w2, int flag = 0);
void uvsLuckChange(int val);

void dgLog(char *string);
void dataList(int val = -1);
void loadList(void);
void saveList(void);
void showmap(void);
/* --------------------------- DEFINITION SECTION -------------------------- */
static char dgSyntaxError[] = "dgFile syntax error";

namespace {
std::string legacyQuestionId(const char *ru, const char *en);
}

const int saveVersion = 5;

int MP_GAME = 0;

DiagenDispatcher *dgD;
int dgMood;
int dgAbortStatus;

static int *dgLevel;
static int enter_sStatus;
static int Qcode, checkStatus, ClearStatus, singleRandAvailable;
static int wasRubeecated;

static int isLoading;
static int loadedVersion;
static int oldVersion;
static int solidQ;
static int eventOUT;

static FileBox *FBox;

const char *DiagenTextName() {
	return vangers::localization::resource("diagen");
}
// DiagenDispatcher* nDD;

#ifdef DIAGEN_TEST
int DGdata[DG_EXTERNS::MAX];
// int NetworkON = 0;
// int aciEscaveDead;
// int aciEscaveEmpty;
#endif

#ifdef DIAGEN_TEST
static char *strLine = (char *)"------------------------";
static int CommandMax;
char *Command[16];
XStream ffsave(1);

void ParseCommand(char *s) {
	int ind = 0;
	while (1) {
		while (*s && (*s == ' ' || *s == '\t'))
			s++;
		if (!*s)
			return;
		Command[ind++] = s;
		while (*s && !(*s == ' ' || *s == '\t'))
			s++;
		if (!*s)
			return;
		*s++ = '\0';
	}
	CommandMax = ind;
}

void doCommand(void) {
	char *c = Command[0];
	if (!strcmp(c, "GO"))
		dgD->startSession(Command[1]);
	else if (!strcmp(c, "FINISH"))
		dgD->endSession();
	else if (!strcmp(c, "STATUS"))
		dgD->getStatus();
	else if (!strcmp(c, "ASK")) {
		if (auto *answer = dgD->getAnswer(Command[1]))
			std::cout << answer << '\n';
	} else if (!strcmp(c, "ESTATUS"))
		dgD->eStatus = atoi(Command[1]);
	else if (!strcmp(c, "BSTATUS"))
		dgD->bStatus = atoi(Command[1]);
	else if (!strcmp(c, "ZSTATUS"))
		dgD->zStatus = atoi(Command[1]);
	else if (!strcmp(c, "SAVE"))
		dgD->save(ffsave);
	else if (!strcmp(c, "LOAD"))
		dgD->load(ffsave);
	else if (!strcmp(c, "DATA")) {
		DGdata[atoi(Command[1])] = atoi(Command[2]);
		saveList();
	} else if (!strcmp(c, "DATALIST"))
		dataList();
	else if (!strcmp(c, "DATALIST0"))
		dataList(0);
	else if (!strcmp(c, "MAP"))
		showmap();
	else if (!strcmp(c, "VISIT")) {
		dgRoom *r = dgD->seekR(Command[1], 1);
		if (r) {
			r->visitCounter = atoi(Command[2]);
			r->comingCounter = atoi(Command[3]);
		}
	} else
		std::cout << "\nError: unknown command...\n";
}

void console_clear() {
#	ifdef WIN32
	system("CLS");
#	else
	// Assume POSIX
	system("clear");
#	endif
}

void showmap(void) {
	if (!dgD->currentR)
		return;
	int sx = dgD->currentR->gridSX;
	int sy = dgD->currentR->gridSY;
	uchar *map = dgD->currentR->status;
	int i, j;
	console_clear();
	for (j = 0; j < sy; j++) {
		for (i = 0; i < sx; i++)
			switch (map[j * sx + i]) {
			case DG_CELLSTATUS::EMPTY:
				std::cout << " ";
				break;
			case DG_CELLSTATUS::DORMANT:
				std::cout << "D";
				break;
			case DG_CELLSTATUS::USED:
				std::cout << "U";
				break;
			case DG_CELLSTATUS::WAITING:
				std::cout << "W";
				break;
			case DG_CELLSTATUS::OPENED:
				std::cout << "O";
				break;
			case DG_CELLSTATUS::EXPLODED:
				std::cout << "E";
				break;
			}
		std::cout << "\n";
	}
}

void diagenEventHandle(int code) {
	char *s = NULL;
	int i;
	switch (code) {
	case DG_GET_NEXT_PHRASE:
		dgD->getNextPhrase();
		break;
	case DG_SKIP_ALL:
		for (i = 0; i < 24; i++)
			dgD->getNextPhrase();
		break;
	case DG_GET_QUERY_LIST:
		console_clear();
		std::cout << strLine << strLine << "\n";
		s = dgD->getQprefix();
		if (s)
			std::cout << s << ":\n\n";
		s = dgD->findQfirst();
		while (s) {
			//				dgLog(s);
			std::cout << s << "\n";
			s = dgD->findQnext();
		}
		s = dgD->getQpostfix();
		if (s)
			std::cout << "\n" << s << "\n";
		std::cout << strLine << strLine << "\n\n";
		break;
	case DG_TEST_QUERY:
		if (!dgD->varAtom) {
			console_clear();
			std::cout << strLine << strLine << "\n";
			s = dgD->findQfirst();
			while (s) {
				std::cout << s << "\n";
				if (auto *answer = dgD->getAnswerById(dgD->currentQuestionId().c_str()))
					std::cout << answer << '\n';
				s = dgD->findQnext();
			}
			std::cout << strLine << strLine << "\n\n";
		}
		break;
	case DG_ENTER_COMMAND:
		console_clear();
		// TODO stalkerg
		// inpS.initString();
		std::cout << "Command:\n";
		break;
	case DG_GET_COMMAND:
		// TODO stalkerg
		// s = inpS.getString();
		console_clear();
		if (s && *s) {
			std::cout << "Accept command (" << s << ")...\n";
			ParseCommand(s);
			doCommand();
		}
		break;
	}
}

// TODO stalkerg redifined func
// char* uvsGetLarvaWorld(int n){ return "Glorx"; }
// char* StringOfBoozeeniada(void){ return "BooZZZ"; }
// int getSpobsState(void){ return 1; }
// int getThreallState(void){ return 1; }
// int isSpummyDeath(void){ return 0; }
// int aciWorldLinkExist(int w1,int w2,int flag){ return 0; }

#endif

char *getDGname(const char *name, const char *ext) {
	static char buf[256];
	strcpy(buf, "data/");
	strcat(buf, name);
	strcat(buf, ext);
	return buf;
}

void dgLog(char *string) {
	/*
		static XStream ff(getDGname("current.log"),XS_OUT);
		ff < "\"" < string < "\"\r\n";
	*/
	if (!string)
		return;
#ifdef DIAGEN_TEST
	if (checkStatus)
		return;
	std::cout << string << "\n";
	std::cout << strLine << strLine << "\n\n";
#endif
}

inline char *getSubj(char *p, int &m) {
	int len = strlen(p);
	if (*p != SUBJ_SYMBOL || *(p + len - 1) != SUBJ_SYMBOL_RIGHT) {
		std::cout << "dgSyntaxError p:" << p << "|" << m << std::endl;
		ErrH.Abort(dgSyntaxError);
	}
	*(p + len - 1) = '\0';
	char *add = ++p;
	while (*add && *add != ':') {
		add++;
	}
	if (*add) {
		*add++ = '\0';
	}
	switch (*add) {
	case 'G':
		m = 1;
		break;
	case 'B':
		m = 2;
		break;
	default:
		m = 0;
	}
	return p;
}

inline char *getSubj(char *p) {
#ifdef GERMAN_VERSION
	if (*p != '$')
#else
	if (*p != '#')
#endif
		ErrH.Abort(dgSyntaxError);
	return p + 1;
}

inline void cutLink(char *&p) {
	if (p) {
		*p++ = '\0';
		*(p + strlen(p) - 1) = '\0';
	}
}

int Comparision(char *opr, int a, int b) {
	char mss[] = "Wrong operation";

	switch (*opr) {
	case '=':
		switch (*(opr + 1)) {
		case '=':
			return a == b;
		default:
			ErrH.Abort(mss);
		}
	case '>':
		switch (*(opr + 1)) {
		case '=':
			return a >= b;
		case '\0':
			return a > b;
		default:
			ErrH.Abort(mss);
		}
	case '<':
		switch (*(opr + 1)) {
		case '=':
			return a <= b;
		case '\0':
			return a < b;
		default:
			ErrH.Abort(mss);
		}
	default:
		ErrH.Abort(mss);
	}
	return 0;
}

dgRoom::~dgRoom() {
	while (mtail) {
		auto *next = mtail->next;
		delete mtail;
		mtail = next;
	}
	for (auto *head : {qAtail, qVtail})
		while (head) {
			auto *next = head->next;
			delete head;
			head = next;
		}
	for (int i = 0; i < gridSX * gridSY; ++i)
		delete grid[i];
	delete[] grid;
	delete[] status; // backup_status aliases the second half.
}

DiagenDispatcher::~DiagenDispatcher() {
	while (rtail) {
		auto *next = rtail->next;
		delete rtail;
		rtail = next;
	}
	delete invR;
	delete FBox;
	FBox = nullptr;
	if (dgD == this)
		dgD = nullptr;
}

void diagenPrepare(void) {
	dgD = new DiagenDispatcher;
	dgD->init();
#ifdef DIAGEN_TEST
	loadList();
#endif
}

int getSpectorsTaskStatus(void) {
	// #ifndef DIAGEN_TEST
	char es[] = "PIVLOZB";
	char ws[] = "WKBA";
	char esS[] = "???????";
	char wsS[] = "????";
	memcpy(esS, es, 7);
	memcpy(wsS, ws, 4);

	int i, j, res;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 7; j++) {
			auto res = getPassangerStats(es[j], ws[i]);
			if (res != 0) {
				esS[j] = '\0';
				wsS[i] = '\0';
			}
		}
	res = 0;
	for (i = 0; i < 4; i++)
		res += wsS[i];
	for (i = 0; i < 7; i++)
		res += esS[i];

	return !res;
	/*#else
		return 0;
	#endif*/
}

int getInfernalsTaskStatus(void) {
	// #ifndef DIAGEN_TEST
	int res = 0, i;
	for (i = 0; i < 3; i++)
		res += isCycleExist(i, 'F');
	for (i = 0; i < 3; i++)
		res += isCycleExist(i, 'G');
	for (i = 0; i < 3; i++)
		res += isCycleExist(i, 'N');
	return !res;
	/*#else
		return 1;
	#endif*/
}

// ----------------------------------- dgFile ------------------------------------

bool ISEALPHA(char c) {
	if (lang() == RUSSIAN) {
		return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
	} else if (lang() == GERMAN) {
		return (c != '#' && !(c >= '0' && c <= '9'));
	}

	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}

char *detect_text(char *ret) {
	char *ret2 = ret;
	if (*ret2) {
		if (*ret2 == '$' && *(ret2 + 1) == '(') {
			std::cout << "Detect variable in start string." << std::endl;
			while (*ret2 && *ret2 != ')') { // Ищем конец переменной. Могут быть переполнения. Но мы
											// будем писать аккуратно. :)
				ret2++;
			}
			ret2++;
			while (*ret2 && *ret2 == ' ') { // Пропускаем пробелы
				ret2++;
			}
		}
	}

	if (*ret2 == SUBJ_SYMBOL || *ret2 == COMMAND_SYMBOL) {
		return ret;
	}
	return (*ret2 == LINK_SYMBOL || *ret2 == '<') ? ret2 + 1 : ret2;
}

void dgFile::load(char *fname, int _len, bool verbose) {
	static char mss[] = "Wrong dgFile format";
	int handle = 0;
	if (!_len) {
#ifdef DIAGEN_TEST
		external = 0;
		// Canonical authoring files are per locale; archive entry keys remain data/<name>.
		// Flat historical fixtures/mod sources retain their existing input layout.
		const std::filesystem::path logical(fname);
		auto source = logical;
		const auto &requested = vangers::localization::requested_locale();
		const auto directory = logical.parent_path() / (requested.empty() ? "en" : requested);
		if ((logical.extension() == ".query" || logical.extension() == ".text") &&
			(std::filesystem::is_directory(logical.parent_path() / "ru") ||
				std::filesystem::is_directory(logical.parent_path() / "en") ||
				std::filesystem::is_directory(directory))) {
			source = directory / logical.filename();
			const auto english = logical.parent_path() / "en" / logical.filename();
			if (!std::filesystem::exists(source) && directory != english.parent_path() &&
				std::filesystem::exists(english)) {
				std::cerr << "Localization: missing Diagen source " << source.string() << "; using "
						  << english.string() << '\n';
				source = english;
			}
		}
		XStream ff(source.string().c_str(), XS_IN);
		buf = new char[(len = ff.size()) + 1];
		ff.read(buf, len);
		ff.close();
		buf[len] = '\0';
		handle = 1;
		if (verbose)
			std::cout << "dgFile::load buf:" << buf << std::endl;
#else
		external = 1;
		buf = FBox->get(fname, len);
#endif
	} else {
		external = 1;
		len = _len;
		buf = fname;
		handle = 1;
	}

	if (!handle) {
		utf8_text = true; // FileBox already decoded/validated the complete entry.
		index = 0;
		return;
	}
	if (!_len) {
		std::string_view bytes(buf, len);
		constexpr std::string_view marker = "utf8_text";
		utf8_text = bytes.substr(0, marker.size()) == marker &&
					(bytes.size() == marker.size() || bytes[marker.size()] == '\n' ||
						bytes[marker.size()] == '\r');
		if (utf8_text)
			bytes.remove_prefix(marker.size());
		if (utf8_text && !vangers::settings::is_valid_utf8(bytes))
			ErrH.Abort("Invalid Diagen UTF-8 source", XERR_USER, -1, fname);
		const auto text = utf8_text ? std::string(bytes) : vangers::settings::cp1251_to_utf8(bytes);
		delete[] buf;
		len = int(text.size());
		buf = new char[text.size() + 1];
		memcpy(buf, text.data(), text.size());
		buf[text.size()] = 0;
	}

	int i = 0, mode = 1, j;
	char c;
	//(stalkerg)Разрезаем буфер там где есть комментарии
	while (i < len) {
		c = buf[i];
		if (mode) {
			if (c == '/' && buf[i + 1] == '*') {
				buf[i] = buf[i + 1] = '\0';
				i++;
				mode = 0;
			}
		} else {
			if (c == '*' && buf[i + 1] == '/') {
				buf[i] = buf[i + 1] = '\0';
				i++;
				mode = 1;
			} else {
				buf[i] = '\0';
			}
		}
		i++;
	}
	i = 0;
	while (i < len) {
		c = buf[i];
		if (c == '\"') {
			buf[i++] = '\0';
			j = i;
			while (i < len) {
				if (buf[i] == '\"') {
					break;
				}
				if (buf[i] == SUBJ_SYMBOL || buf[i] == LINK_SYMBOL) {
					ErrH.Abort(fname, XERR_USER, -1, buf + i + 1);
				}
				i++;
			}
			if (i == len) {
				ErrH.Abort(mss, XERR_USER, -1, "\" absent");
			} else {
				buf[i] = '\0';
			}
		} else {
			if (c == SUBJ_SYMBOL) {
				i++;
				while (i < len) {
					if (buf[i] == SUBJ_SYMBOL_RIGHT)
						break;
					i++;
				}
				if (i == len) {
					ErrH.Abort(mss, XERR_USER, -1, "RightSubjSymbol absent");
				}
			} else {
				if (c == LINK_SYMBOL) {
					i++;
					while (i < len) {
						if (buf[i] == LINK_SYMBOL_RIGHT)
							break;
						i++;
					}
					if (i == len) {
						ErrH.Abort(mss, XERR_USER, -1, "RightLinkSymbol absent");
					}
				} else {
					if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
						buf[i] = '\0';
					}
				}
			}
		}
		i++;
	}
	index = 0;
#ifdef DIAGEN_TEST
	if (external)
		return;
	char *b = new char[len];
	memset(b, 0, len);
	int off = 0, l;
	char *p, *p2;

#	ifdef SAVE_SINGE_LANGUAGE
	int skip = !memcmp(fname + strlen(fname) - 3, "dil", 3) ||
			   !memcmp(fname + strlen(fname) - 3, "lst", 3);
#	endif

	while (1) {
		p = getElement(DGF_NONE, 1);
		if (!p)
			break;

#	ifdef SAVE_SINGE_LANGUAGE
		if (!skip && !utf8_text) {
			p2 = detect_text(p);
			if (!(*p2 == SUBJ_SYMBOL || *p2 == COMMAND_SYMBOL || *p2 == '<' || *p2 == '#')) {
				if (!((lang() != RUSSIAN && ISEALPHA(*p2)) ||
						(lang() == RUSSIAN && !ISEALPHA(*p2))))
					continue;
			}
		}
#	endif
		l = strlen(p);
		memcpy(b + off, p, l);
		off += l + 1;
	}
	FBox->add(fname, b, off);
	delete[] b;
#endif
	index = 0;
}

char *dgFile::getElement(int DualElements, int empty_available) {
	if (!buf || index < 0 || index >= len) {
		if (!empty_available)
			ErrH.Abort(dgSyntaxError);
		return nullptr;
	}
	char *p = buf + index;
	while (index < len && !(*p))
		p++, index++;
	if (index == len) {
		if (!empty_available)
			ErrH.Abort(dgSyntaxError);
		return NULL;
	}
	char *ret = p;

	while (index < len && *p)
		p++, index++;

#if defined(DIAGEN_TEST) || !defined(SINGLE_LANGUAGE)
	char *ret2 = ret; //(stalkerg) Для определения символа по которому можно понять язык
	char *elem;

	if (DualElements == DGF_DUAL && !utf8_text) {
		if (*ret2) {
			if (*ret2 == '$' && *(ret2 + 1) == '(') {
				std::cout << "Detect variable in start string." << std::endl;
				while (*ret2 && *ret2 != ')') { // Ищем конец переменной. Могут быть переполнения.
												// Но мы будем писать аккуратно. :)
					ret2++;
				}
				ret2++;
				while (*ret2 && *ret2 == ' ') { // Пропускаем пробелы
					ret2++;
				}
			}
		}
		if (!(*ret2)) {
			ErrH.Abort("Nope closing bracket for variable", XERR_USER, -1, "");
		}

		if (*ret2 == SUBJ_SYMBOL || *ret2 == COMMAND_SYMBOL) {
			return ret;
		}
		elem = (*ret2 == LINK_SYMBOL || *ret2 == '<') ? ret2 + 1 : ret2;
		if ((lang() != RUSSIAN && ISEALPHA(*elem)) || (lang() == RUSSIAN && !ISEALPHA(*elem))) {
			return ret;
		} else {
			return getElement(DualElements, empty_available);
		}
	}
#endif
	return ret;
}

// ----------------------------------- dgAtom ------------------------------------
void dgAtom::link(dgAtom *&tail) {
	if (!tail)
		tail = this;
	else {
		tail->prev->next = this;
		prev = tail->prev;
	}
	next = NULL;
	tail->prev = this;
}

char *dgAtom::read(dgFile *pf, char *s) {
	data = s;
	char *p = pf->getElement(DGF_DUAL, 1);
	qlMax = 0;
	int log = 1, i;
	while (p && log)
		switch (*p) {
		case LINK_SYMBOL:
			qlinks = p + 1;
			do {
				qlMax++;
				cutLink(p);
				p = pf->getElement(DGF_DUAL, 1);
			} while (p && *p == LINK_SYMBOL);
			break;
		case COMMAND_SYMBOL:
			p++;
			switch (*p) {
			case '?':
				nvariant = atoi(pf->getElement(DGF_NONE));
				if (nvariant <= 0)
					ErrH.Abort("Wrong nvariant", XERR_USER, -1, data);
				variants = new char *[3 * nvariant];
				comments = variants + nvariant;
				vcodes = (int *)(comments + nvariant);
				for (i = 0; i < nvariant; i++)
					vcodes[i] = atoi(pf->getElement(DGF_NONE));
				for (i = 0; i < nvariant; i++) {
					variants[i] = pf->getElement(DGF_DUAL);
					comments[i] = pf->getElement(DGF_DUAL);
				}
				p = pf->getElement(DGF_DUAL, 1);
				break;
			default:
				p--;
				log = 0;
				break;
			}
			break;
		default:
			log = 0;
			break;
		}
	return p;
}

char *dgAtom::findQLfirst(void) {
	if (!qlMax)
		return NULL;
	qlNum = 1;
	qlPos = 0;
	while (qlinks[qlPos])
		qlPos++;
	return qlinks;
}

char *dgAtom::findQLnext(void) {
	if (++qlNum > qlMax)
		return NULL;
	while (!qlinks[qlPos])
		qlPos++;
	char *ret = qlinks + qlPos;
	while (qlinks[qlPos])
		qlPos++;
	return ret;
}

int dgAtom::getVcode(int index, char *&cs) {
	if (index < 0 || index >= nvariant)
		return 0;
	cs = comments[index];
	return vcodes[index];
}

// ----------------------------------- dgMolecule ------------------------------------
void dgMolecule::link(dgMolecule *&tail, int mode) {
	switch (mode) {
	case 0:
		if (!tail)
			tail = this;
		else {
			tail->prev->next = this;
			prev = tail->prev;
		}
		next = NULL;
		tail->prev = this;
		break;
	case 1:
		prev = tail->prev;
		tail->prev = this;
		next = tail;
		tail = this;
		break;
	case 2:
		next = tail->next;
		prev = tail;
		if (!next)
			tail->prev = this;
		else
			tail->next->prev = this;
		tail->next = this;
		break;
	}
}

dgMolecule *dgMolecule::delink(dgMolecule *&tail) {
	if (tail != this) {
		prev->next = next;
		if (!next)
			tail->prev = prev;
	} else {
		tail = next;
	}
	if (next)
		next->prev = prev;
	return this;
}

void dgMolecule::accept(dgFile *_dgf) {
	dgf = _dgf;
	char *p = NULL;
	dgAtom *pa;
	while (1) {
		if (!p)
			if (!(p = dgf->getElement(DGF_DUAL, 1)))
				break;
		(pa = new dgAtom)->link(tail);
		if (!(p = pa->read(dgf, p)))
			break;
	}
	curA = tail;
}

char *dgMolecule::getPhrase(int noHandle) {
	char *ret = curA ? curA->data : NULL;
	if (!ret) {
		return NULL;
	}

	if (!noHandle) {
		phrase = ret;
		while (getHandledPhrase(phrase)) {
		}
		return phrase.data();
	}
	return ret;
}

char *dgMolecule::getVarPhrase(char *s) {
	static char nbuf[32];
	char *ret = NULL;
	if (!strcmp(s, "World of Larva0"))
		ret = dgD->getWorldName(uvsGetLarvaWorld(0));
	else if (!strcmp(s, "World of Larva1"))
		ret = dgD->getWorldName(uvsGetLarvaWorld(1));
	else if (!strcmp(s, "World of Larva2"))
		ret = dgD->getWorldName(uvsGetLarvaWorld(2));
	else if (!strcmp(s, "Cirt Delivery"))
		ret = port_itoa(uvsgetDGdata(DG_EXTERNS::CIRT_DELIVERY), nbuf, 10);
	else if (!strcmp(s, "Cycle Name of Locked Larva")) {
		const int index = uvsgetDGdata(DG_EXTERNS::LARVA_CYCLE);
		const auto key =
			std::string("cycle.") + dgD->currentR->worldNames[0] + "." + std::to_string(index);
		const auto *localized = vangers::localization::catalog().find_text(key);
		ret = localized ? const_cast<char *>(localized->c_str())
						: dgD->CycleName[lang() == RUSSIAN ? 1 : 0][dgD->currentR->bios][index];
	} else if (!strcmp(s, "Boozeena Secret Code"))
		ret = StringOfBoozeeniada();

	if (!ret)
		return s;
	//		ErrH.Abort("Unknown VarPhrase",XERR_USER,-1,s);
	return ret;
}

int dgMolecule::getHandledPhrase(std::string &text) {
	const auto start = text.find('$');
	if (start == std::string::npos)
		return 0;
	const auto end = text.find(')', start + 2);
	if (text.compare(start, 2, "$(") || end == std::string::npos)
		ErrH.Abort("Wrong VarPhrase format");
	auto key = text.substr(start + 2, end - start - 2);
	const std::string replacement = getVarPhrase(key.data());
	text.replace(start, end - start + 1, replacement);
	return 1;
}

// ----------------------------------- dgQuery ------------------------------------
void dgQuery::link(dgQuery *&tail) {
	if (!tail)
		tail = this;
	else {
		tail->prev->next = this;
		prev = tail->prev;
	}
	next = NULL;
	tail->prev = this;
}

dgQuery *dgQuery::delink(dgQuery *&tail) {
	if (tail != this) {
		prev->next = next;
		if (!next)
			tail->prev = prev;
	} else {
		tail = next;
	}
	if (next)
		next->prev = prev;
	return this;
}

int dgQuery::getIndex(void) {
	// Custom localizations may contain fewer answers than the saved resource.
	// Keep the saved progress index for switching back, but never index beyond
	// the currently available translation or advance/regress its hidden progress.
	if (lindex >= nanswer)
		return nanswer - 1;
	int n = lindex;
	if (solidQ) {
		for (int i = 0; i < nanswer; i++) {
			if (levels[i] == *dgLevel) {
				lindex = i;
				return i;
			}
		}
	} else {
		if (n + 1 < nanswer && levels[n + 1] <= *dgLevel)
			lindex = ++n;
	}
	return n;
}

// ----------------------------------- dgCell ------------------------------------
void dgCell::read(dgFile *dgf) {
	x = atoi(dgf->getElement(DGF_NONE));
	y = atoi(dgf->getElement(DGF_NONE));
	char *p = dgf->getElement(DGF_NONE);
	if (*p != ' ')
		Name = p;
	Type = atoi(dgf->getElement(DGF_NONE));
	isWaiting = atoi(dgf->getElement(DGF_NONE));
	isLooping = atoi(dgf->getElement(DGF_NONE));
	p = dgf->getElement(DGF_NONE);
	if (*p != ' ') {
		Access.load(p, strlen(p));
	}
	p = dgf->getElement(DGF_NONE);
	if (*p != ' ') {
		PostCMD.load(p, strlen(p));
	}
	p = dgf->getElement(DGF_NONE);
	if (*p != ' ') {
		StartCMD.load(p, strlen(p));
	}
}

struct Q {
	enum {
		TRU,
		DATA,
		VISITE,
		VISITW,
		COMING,
		SINGLERAND,
		BSTATUS,
		ZSTATUS,
		SSTATUS,
		ENTER_SSTATUS,
		QCODE,
		MP_GAME,
		CYCLE_EXIST,
		PASSENGER_OUT,
		PASSENGER_ARRIVAL,
		SPECTORS_TASK,
		INFERNALS_TASK,
		SPOBS_ALIVE,
		THREALL_ALIVE,
		SUCCESS,
		FIASCO,
		SPUMMY_DEATH,
		KEY2GLORX_EXIST,
		KEY2ARKAZNOY_EXIST,
		KEY2BOOZEENA_EXIST,
		WAS_RUBEECATED
	};
};

struct Q2 {
	int id;
	const char *name;
} Query[] = {{Q::TRU, "true"},
	{Q::DATA, "data"},
	{Q::VISITE, "visitE"},
	{Q::VISITW, "visitW"},
	{Q::COMING, "coming"},
	{Q::SINGLERAND, "singlerand"},
	{Q::BSTATUS, "bstatus"},
	{Q::ZSTATUS, "zstatus"},
	{Q::SSTATUS, "sstatus"},
	{Q::ENTER_SSTATUS, "enter_sstatus"},
	{Q::QCODE, "qcode"},
	{Q::MP_GAME, "mp_game"},
	{Q::CYCLE_EXIST, "cycle_exist"},
	{Q::PASSENGER_OUT, "passenger_out"},
	{Q::PASSENGER_ARRIVAL, "passenger_arrival"},
	{Q::SPECTORS_TASK, "spectors_task"},
	{Q::INFERNALS_TASK, "infernals_task"},
	{Q::SPOBS_ALIVE, "spobs_alive"},
	{Q::THREALL_ALIVE, "threall_alive"},
	{Q::SUCCESS, "success"},
	{Q::FIASCO, "fiasco"},
	{Q::SPUMMY_DEATH, "spummy_death"},
	{Q::KEY2GLORX_EXIST, "key2glorx_exist"},
	{Q::KEY2ARKAZNOY_EXIST, "key2arkaznoy_exist"},
	{Q::KEY2BOOZEENA_EXIST, "key2boozeena_exist"},
	{Q::WAS_RUBEECATED, "wasRubeecated"},
	{-1, NULL}};

struct Q3 {
	int id;
	const char *name;
} ArgData[] = {{DG_EXTERNS::ELR_RESULT, "ELR_RESULT"},
	{DG_EXTERNS::ELR_TOTAL, "ELR_TOTAL"},
	{DG_EXTERNS::ELR_RATING, "ELR_RATING"},

	{DG_EXTERNS::KER_RESULT, "KER_RESULT"},
	{DG_EXTERNS::KER_TOTAL, "KER_TOTAL"},
	{DG_EXTERNS::KER_RATING, "KER_RATING"},

	{DG_EXTERNS::PIP_RESULT, "PIP_RESULT"},
	{DG_EXTERNS::PIP_TOTAL, "PIP_TOTAL"},
	{DG_EXTERNS::PIP_RATING, "PIP_RATING"},

	{DG_EXTERNS::ZYK_RESULT, "ZYK_RESULT"},
	{DG_EXTERNS::ZYK_TOTAL, "ZYK_TOTAL"},
	{DG_EXTERNS::ZYK_RATING, "ZYK_RATING"},

	{DG_EXTERNS::INV_BAD_CIRT, "INV_BAD_CIRT"},
	{DG_EXTERNS::INV_GOOD_CIRT, "INV_GOOD_CIRT"},
	{DG_EXTERNS::INV_NYMBOS, "INV_NYMBOS"},
	{DG_EXTERNS::INV_PHLEGMA, "INV_PHLEGMA"},
	{DG_EXTERNS::INV_HEROIN, "INV_HEROIN"},
	{DG_EXTERNS::INV_SHRUB, "INV_SHRUB"},
	{DG_EXTERNS::INV_POPONKA, "INV_POPONKA"},
	{DG_EXTERNS::INV_UNIQUE_POPONKA, "INV_UNIQUE_POPONKA"},
	{DG_EXTERNS::INV_TOXICK, "INV_TOXICK"},
	{DG_EXTERNS::INV_ELEECH, "INV_ELEECH"},
	{DG_EXTERNS::INV_DEAD_ELEECH, "INV_DEAD_ELEECH"},
	{DG_EXTERNS::INV_PIPETKA, "INV_PIPETKA"},
	{DG_EXTERNS::INV_KERNOBOO, "INV_KERNOBOO"},
	{DG_EXTERNS::INV_WEEZYK, "INV_WEEZYK"},
	{DG_EXTERNS::INV_RUBBOX, "INV_RUBBOX"},
	{DG_EXTERNS::INV_BAD_LARVA, "INV_BAD_LARVA"},
	{DG_EXTERNS::INV_GOOD_LARVA, "INV_GOOD_LARVA"},

	{DG_EXTERNS::INV_MESSIAH, "INV_MESSIAH"},
	{DG_EXTERNS::INV_PROTRACTOR, "INV_PROTRACTOR"},
	{DG_EXTERNS::INV_FUNCTION83, "INV_FUNCTION83"},
	{DG_EXTERNS::INV_BOOTSECTOR, "INV_BOOTSECTOR"},
	{DG_EXTERNS::INV_PEELOT, "INV_PEELOT"},
	{DG_EXTERNS::INV_SPUMMY, "INV_SPUMMY"},

	{DG_EXTERNS::INV_LEEPURINGA, "INV_LEEPURINGA"},
	{DG_EXTERNS::INV_PALOCHKA, "INV_PALOCHKA"},
	{DG_EXTERNS::INV_BOORAWCHICK, "INV_BOORAWCHICK"},
	{DG_EXTERNS::INV_NOBOOL, "INV_NOBOOL"},
	{DG_EXTERNS::INV_PIPKA, "INV_PIPKA"},
	{DG_EXTERNS::INV_PEREPONKA, "INV_PEREPONKA"},
	{DG_EXTERNS::INV_ZEEFICK, "INV_ZEEFICK"},

	{DG_EXTERNS::INV_ELEEPOD, "INV_ELEEPOD"},
	{DG_EXTERNS::INV_BEEBOORAT, "INV_BEEBOORAT"},
	{DG_EXTERNS::INV_ZEEX, "INV_ZEEX"},

	{DG_EXTERNS::THIEF_LEEPURINGA, "THIEF_LEEPURINGA"},
	{DG_EXTERNS::THIEF_PALOCHKA, "THIEF_PALOCHKA"},
	{DG_EXTERNS::THIEF_BOORAWCHICK, "THIEF_BOORAWCHICK"},
	{DG_EXTERNS::THIEF_NOBOOL, "THIEF_NOBOOL"},
	{DG_EXTERNS::THIEF_PIPKA, "THIEF_PIPKA"},
	{DG_EXTERNS::THIEF_PEREPONKA, "THIEF_PEREPONKA"},
	{DG_EXTERNS::THIEF_ZEEFICK, "THIEF_ZEEFICK"},

	{DG_EXTERNS::HERE_LEEPURINGA, "HERE_LEEPURINGA"},
	{DG_EXTERNS::HERE_PALOCHKA, "HERE_PALOCHKA"},
	{DG_EXTERNS::HERE_BOORAWCHICK, "HERE_BOORAWCHICK"},
	{DG_EXTERNS::HERE_NOBOOL, "HERE_NOBOOL"},
	{DG_EXTERNS::HERE_PIPKA, "HERE_PIPKA"},
	{DG_EXTERNS::HERE_PEREPONKA, "HERE_PEREPONKA"},
	{DG_EXTERNS::HERE_ZEEFICK, "HERE_ZEEFICK"},

	{DG_EXTERNS::KILLED_VANGERS, "KILLED_VANGERS"},
	{DG_EXTERNS::MECHOS_CHANGED, "MECHOS_CHANGED"},
	{DG_EXTERNS::MECHOS_TYPE, "MECHOS_TYPE"},
	{DG_EXTERNS::CYCLE, "CYCLE"},
	{DG_EXTERNS::BEEBS, "BEEBS"},
	{DG_EXTERNS::LUCK, "LUCK"},
	{DG_EXTERNS::DOMINANCE, "DOMINANCE"},
	{DG_EXTERNS::TABUTASKS, "TABUTASKS"},
	{DG_EXTERNS::FRESH_TABUTASKS, "FRESH_TABUTASKS"},
	{DG_EXTERNS::WTABUTASKS, "WTABUTASKS"},
	{DG_EXTERNS::TRUE_PASSENGER, "TRUE_PASSENGER"},
	{DG_EXTERNS::BOORAWCHICK_RETURN, "BOORAWCHICK_RETURN"},

	{DG_EXTERNS::PHLEGMA_DELIVERY, "PHLEGMA_DELIVERY"},
	{DG_EXTERNS::NYMBOS_DELIVERY, "NYMBOS_DELIVERY"},
	{DG_EXTERNS::HEROIN_DELIVERY, "HEROIN_DELIVERY"},
	{DG_EXTERNS::SHRUB_DELIVERY, "SHRUB_DELIVERY"},
	{DG_EXTERNS::POPONKA_DELIVERY, "POPONKA_DELIVERY"},
	{DG_EXTERNS::TOXICK_DELIVERY, "TOXICK_DELIVERY"},
	{DG_EXTERNS::CIRT_DELIVERY, "CIRT_DELIVERY"},
	{DG_EXTERNS::LARVA_DELIVERY, "LARVA_DELIVERY"},

	{DG_EXTERNS::PHLEGMA_BONUS, "PHLEGMA_BONUS"},
	{DG_EXTERNS::NYMBOS_BONUS, "NYMBOS_BONUS"},
	{DG_EXTERNS::HEROIN_BONUS, "HEROIN_BONUS"},
	{DG_EXTERNS::SHRUB_BONUS, "SHRUB_BONUS"},
	{DG_EXTERNS::POPONKA_BONUS, "POPONKA_BONUS"},
	{DG_EXTERNS::TOXICK_BONUS, "TOXICK_BONUS"},

	{DG_EXTERNS::WEEZYK_ASSIGNMENT, "WEEZYK_ASSIGNMENT"},
	{DG_EXTERNS::LARVA_CYCLE, "LARVA_CYCLE"},

	{DG_EXTERNS::ZIGGER_ZAGGER, "ZIGGER_ZAGGER"},
	{DG_EXTERNS::CART, "CART"},
	{DG_EXTERNS::PUT_PUT, "PUT_PUT"},
	{DG_EXTERNS::BOX, "BOX"},
	{DG_EXTERNS::RATTLE, "RATTLE"},
	{DG_EXTERNS::HEAVY_IRON, "HEAVY_IRON"},
	{DG_EXTERNS::STAR, "STAR"},
	{DG_EXTERNS::WHIZZER, "WHIZZER"},
	{DG_EXTERNS::HUSKER, "HUSKER"},
	{DG_EXTERNS::COGGED_WIDGET, "COGGED_WIDGET"},

	{-1, NULL}};

#ifdef DIAGEN_TEST
// TODO Need XCon position stalkerg
void dataList(int val) {
	console_clear();
	int i = 0;
	while (ArgData[i].name) {
		if (val != -1)
			DGdata[i] = val;
		// XCon.setpos(22*(i/25),i%25);
		// XCon <= i < ":" < ArgData[i].name < ": " <= DGdata[i];
		i++;
	}

	i = 0;
	dgRoom *r = dgD->rtail;
	while (r) {
		// XCon.setpos(60,i++);
		// XCon < "[" < *r -> roomName < "]: " <= r -> visitCounter < "/" <= r -> comingCounter < "
		// (" < r -> roomName < ")";
		r = r->next;
	}
	// XCon.setpos(0,24);
	saveList();
}

void loadList(void) {
	XStream ff(0);
	if (!ff.open("data.lst", XS_IN))
		return;
	int i = 0;
	while (ArgData[i].name) {
		ff >= DGdata[i] < "\r\n";
		i++;
	}
	dgRoom *r = dgD->rtail;
	while (r) {
		ff >= r->visitCounter;
		ff >= r->comingCounter;
		r = r->next;
	}
	ff.close();
}

void saveList(void) {
	XStream ff("data.lst", XS_OUT);
	int i = 0;
	while (ArgData[i].name) {
		ff <= DGdata[i] < "\r\n";
		i++;
	}
	dgRoom *r = dgD->rtail;
	while (r) {
		ff <= r->visitCounter < "\r\n";
		ff <= r->comingCounter < "\r\n";
		r = r->next;
	}
	ff.close();
}

/*int uvsgetDGdata(int code)
{
	return DGdata[code];
}*/
#endif

int dgCell::analyzeACCESS(char *p) {
	int i = 0;
	do
		if (!strcasecmp(p, Query[i].name))
			break;
	while (Query[++i].name);
	if (!Query[i].name)
		std::cout << "Unknown query " << p << std::endl;
	char *opr, *arg2;
	int lvalue = 0, rvalue;
	dgRoom *r;

	switch (Query[i].id) {
	case Q::TRU:
		return 1;
	case Q::DATA:
		p = Access.getElement(DGF_NONE);
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		if (*arg2 == '$') {
			arg2++;
			if (!strcmp(arg2, "ELR_TOTAL"))
				rvalue = uvsgetDGdata(DG_EXTERNS::ELR_TOTAL);
			else if (!strcmp(arg2, "KER_TOTAL"))
				rvalue = uvsgetDGdata(DG_EXTERNS::KER_TOTAL);
			else if (!strcmp(arg2, "PIP_TOTAL"))
				rvalue = uvsgetDGdata(DG_EXTERNS::PIP_TOTAL);
			else if (!strcmp(arg2, "ZYK_TOTAL"))
				rvalue = uvsgetDGdata(DG_EXTERNS::ZYK_TOTAL);
#ifdef DIAGEN_TEST
			else if (!strcmp(arg2, "PREV_CYCLE_1"))
				rvalue = 0;
			else if (!strcmp(arg2, "PREV_CYCLE_2"))
				rvalue = 1;
#else
			else if (!strcmp(arg2, "PREV_CYCLE_1"))
				rvalue = getPreviousCycle(1);
			else if (!strcmp(arg2, "PREV_CYCLE_2"))
				rvalue = getPreviousCycle(2);
#endif
#ifdef DIAGEN_TEST
			else {
				XBuffer buf;
				buf < arg2 < "/" < owner->roomName;
				ErrH.Abort("Wrong ARG2", XERR_USER, -1, buf.GetBuf());
			}
#else
			else
				ErrH.Abort("Wrong ARG2");
#endif
		} else
			rvalue = atoi(arg2);
		i = 0;
		do
			if (!strcmp(p, ArgData[i].name))
				break;
		while (ArgData[++i].name);
		if (!ArgData[i].name)
			ErrH.Abort("Unknown data", XERR_USER, -1, p);
		lvalue = uvsgetDGdata(ArgData[i].id);
		return Comparision(opr, lvalue, rvalue);
	case Q::VISITE:
		p = Access.getElement(DGF_NONE);
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		r = owner->owner->seekR(p, 1);
		if (!r)
			ErrH.Abort("Unknown Escave-ID");
		lvalue = r->visitCounter;
		rvalue = atoi(arg2);
		return Comparision(opr, lvalue, rvalue);
	case Q::VISITW:
		p = Access.getElement(DGF_NONE);
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		// #ifndef DIAGEN_TEST
		lvalue = getWorldVisitStats(*p);
		// #endif
		rvalue = atoi(arg2);
		return Comparision(opr, lvalue, rvalue);
	case Q::COMING:
		p = Access.getElement(DGF_NONE);
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		r = owner->owner->seekR(p, 1);
		if (!r)
			ErrH.Abort("Unknown Escave-ID");
		lvalue = r->comingCounter;
		rvalue = atoi(arg2);
		return Comparision(opr, lvalue, rvalue);
	case Q::SINGLERAND:
		p = Access.getElement(DGF_NONE);
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		if (singleRandAvailable) {
			lvalue = realRND(atoi(p));
			rvalue = atoi(arg2);
			if (Comparision(opr, lvalue, rvalue)) {
				singleRandAvailable = 0;
				return 1;
			} else
				return 0;
		} else
			return 0;
	case Q::BSTATUS:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, owner->owner->bStatus, rvalue);
	case Q::ZSTATUS:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, owner->owner->zStatus, rvalue);
	case Q::SSTATUS:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, owner->owner->sStatus, rvalue);
	case Q::ENTER_SSTATUS:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, enter_sStatus, rvalue);
	case Q::QCODE:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, Qcode, rvalue);
	case Q::MP_GAME:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, MP_GAME, rvalue);
	case Q::CYCLE_EXIST:
		lvalue = atoi(Access.getElement(DGF_NONE));
		opr = Access.getElement(DGF_NONE);
		rvalue = atoi(Access.getElement(DGF_NONE));
#ifdef DIAGEN_TEST
		return 1;
#else
		return Comparision(opr, isCycleExist(lvalue), rvalue);
#endif
	case Q::PASSENGER_OUT:
		p = Access.getElement(DGF_NONE);
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		// #ifndef DIAGEN_TEST
		lvalue = getPassangerStats(*p, *(p + 1));
		// #endif
		rvalue = atoi(arg2);
		return Comparision(opr, lvalue, rvalue);
	case Q::PASSENGER_ARRIVAL:
		p = Access.getElement(DGF_NONE);
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		// #ifndef DIAGEN_TEST
		lvalue = getPassangerFrom(*p);
		// #endif
		rvalue = atoi(arg2);
		return Comparision(opr, lvalue, rvalue);
	case Q::SPECTORS_TASK:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, getSpectorsTaskStatus(), rvalue);
	case Q::INFERNALS_TASK:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, getInfernalsTaskStatus(), rvalue);
	case Q::SPOBS_ALIVE:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, getSpobsState(), rvalue);
	case Q::THREALL_ALIVE:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, getThreallState(), rvalue);
	case Q::SUCCESS:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, (int)realRND(100) < uvsgetDGdata(DG_EXTERNS::LUCK), rvalue);
	case Q::FIASCO:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, (int)realRND(60) >= uvsgetDGdata(DG_EXTERNS::LUCK), rvalue);
	case Q::SPUMMY_DEATH:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, isSpummyDeath(), rvalue);
	case Q::KEY2GLORX_EXIST:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, aciWorldLinkExist(0, 1, 0), rvalue);
	case Q::KEY2ARKAZNOY_EXIST:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, aciWorldLinkExist(2, 9, 0), rvalue);
	case Q::KEY2BOOZEENA_EXIST:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, aciWorldLinkExist(2, 5, 0), rvalue);
	case Q::WAS_RUBEECATED:
		opr = Access.getElement(DGF_NONE);
		arg2 = Access.getElement(DGF_NONE);
		rvalue = atoi(arg2);
		return Comparision(opr, wasRubeecated, rvalue);
	}
	return 0;
}

int dgCell::isACCESS(void) {
	Access.reset();
	char *p = Access.getElement(DGF_NONE);
	int result = -1, r;
	int logic = 1;
	while (p) {
		if (!strcmp(p, "(")) {
			r = expressACCESS(1, p, logic);
			if (result != -1)
				result = logic ? (result && r) : (result || r);
			else
				result = r;
		} else if (!strcasecmp(p, "AND")) {
			logic = 1;
			p = Access.getElement(DGF_NONE);
		} else if (!strcasecmp(p, "OR")) {
			logic = 0;
			p = Access.getElement(DGF_NONE);
		} else {
			r = expressACCESS(0, p, logic);
			if (result != -1)
				result = logic ? (result && r) : (result || r);
			else
				result = r;
		}
	}

	return result;
}

int dgCell::expressACCESS(int endreq, char *&p, int &uplogic) {
	if (endreq)
		p = Access.getElement(DGF_NONE);
	int logic = 1;
	int result = analyzeACCESS(p), r;
	p = Access.getElement(DGF_NONE, 1);
	while (p) {
		if (!strcmp(p, ")")) {
			if (!endreq)
				ErrH.Abort("Unexpected ')'");
			p = Access.getElement(DGF_NONE, 1);
			return result;
		} else if (!strcmp(p, "(")) {
			if (endreq)
				ErrH.Abort("Unexpected '('");
			p = Access.getElement(DGF_NONE);
			uplogic = logic;
			return result;
		} else if (!strcasecmp(p, "AND")) {
			logic = 1;
			p = Access.getElement(DGF_NONE);
		} else if (!strcasecmp(p, "OR")) {
			logic = 0;
			p = Access.getElement(DGF_NONE);
		} else {
			r = analyzeACCESS(p);
			result = logic ? (result && r) : (result || r);
			p = Access.getElement(DGF_NONE, 1);
		}
	}
	if (endreq)
		ErrH.Abort("Expected ')' missing");
	return result;
}

struct CMD {
	enum {
		ABORT,
		QUIT,
		BLOCK,
		UNBLOCK,
		CONTINUE,
		CLEAR,
		EPROMOTION,
		BPROMOTION,
		SPROMOTION,
		BDEGRADATION,
		FURY_LEVEL_INCREASE,
		FURY_LEVEL_DECREASE,
		ELR_WAITING_DECREASE,
		ELP_PRIZE_GIFT,
		KEY2GLORX_GIFT,
		KEY2NECROSS_GIFT,
		KEY2XPLO_GIFT,
		KEY2WEEXOW_GIFT,
		KEY2BOOZEENA_GIFT,
		KEY2KHOX_GIFT,
		KEY2ARKAZNOY_GIFT,
		KEY2THREALL_GIFT,
		EXPROPRIATION,
		ZERO_CASH,
		LAMPASSOTHE_END,
		TRANSITION2V,
		TRANSITION2L,
		TRANSITION2O,
		TRANSITION2B,
		CHANGE2RAFFA,
		ACCEPT_RUBBOX,
		ADD_RUBBOX,
		ZERO_KERRATING,
		ZERO_PIPRATING,
		BBR_PRIZE_GIFT,
		MAKE_WEEZYK,
		REGISTER_WEEZYK,
		COMINGOZERO,
		PENALTY1000,
		PENALTY10000,
		ADD_PROTRACTORKILL,
		ADD_MESSIAHKILL,
		OPEN_H00,
		OPEN_H01,
		OPEN_H10,
		OPEN_H11,
		ZERO_BOORAWCHICK,
		ZERO_ARKAZNOY,
		RUBEECATION
	};
};

struct Q4 {
	int id;
	const char *name;
} CmdData[] = {{CMD::ABORT, "abort"},
	{CMD::QUIT, "quit"},
	{CMD::BLOCK, "block"},
	{CMD::UNBLOCK, "unblock"},
	{CMD::CONTINUE, "continue"},
	{CMD::CLEAR, "clear"},
	{CMD::EPROMOTION, "ePromotion"},
	{CMD::BPROMOTION, "bPromotion"},
	{CMD::SPROMOTION, "sPromotion"},
	{CMD::BDEGRADATION, "bDegradation"},
	{CMD::FURY_LEVEL_INCREASE, "fury_level_increase"},
	{CMD::FURY_LEVEL_DECREASE, "fury_level_decrease"},
	{CMD::ELR_WAITING_DECREASE, "elr_waiting_decrease"},
	{CMD::ELP_PRIZE_GIFT, "elp_prize_gift"},
	{CMD::KEY2GLORX_GIFT, "key2glorx_gift"},
	{CMD::KEY2NECROSS_GIFT, "key2necross_gift"},
	{CMD::KEY2XPLO_GIFT, "key2xplo_gift"},
	{CMD::KEY2WEEXOW_GIFT, "key2weexow_gift"},
	{CMD::KEY2BOOZEENA_GIFT, "key2boozeena_gift"},
	{CMD::KEY2KHOX_GIFT, "key2khox_gift"},
	{CMD::KEY2ARKAZNOY_GIFT, "key2arkaznoy_gift"},
	{CMD::KEY2THREALL_GIFT, "key2threall_gift"},
	{CMD::EXPROPRIATION, "expropriation"},
	{CMD::ZERO_CASH, "zeroCash"},
	{CMD::LAMPASSOTHE_END, "LampassoTHE_END"},
	{CMD::TRANSITION2V, "transition2V"},
	{CMD::TRANSITION2L, "transition2L"},
	{CMD::TRANSITION2O, "transition2O"},
	{CMD::TRANSITION2B, "transition2B"},
	{CMD::CHANGE2RAFFA, "change2raffa"},
	{CMD::ACCEPT_RUBBOX, "acceptRubbox"},
	{CMD::ADD_RUBBOX, "addRubbox"},
	{CMD::ZERO_KERRATING, "zeroKERrating"},
	{CMD::ZERO_PIPRATING, "zeroPIPrating"},
	{CMD::BBR_PRIZE_GIFT, "bbr_prize_gift"},
	{CMD::MAKE_WEEZYK, "makeWeezyk"},
	{CMD::REGISTER_WEEZYK, "registerWeezyk"},
	{CMD::COMINGOZERO, "comingOzero"},
	{CMD::PENALTY1000, "penalty1000"},
	{CMD::PENALTY10000, "penalty10000"},
	{CMD::ADD_PROTRACTORKILL, "addProtractorKill"},
	{CMD::ADD_MESSIAHKILL, "addMessiahKill"},
	{CMD::OPEN_H00, "openH00"},
	{CMD::OPEN_H01, "openH01"},
	{CMD::OPEN_H10, "openH10"},
	{CMD::OPEN_H11, "openH11"},
	{CMD::ZERO_BOORAWCHICK, "zeroBoorawchick"},
	{CMD::ZERO_ARKAZNOY, "zeroArkaznoy"},
	{CMD::RUBEECATION, "Rubeecation"},
	{-1, NULL}};

int dgCell::doCMD(int startup) {
	dgFile *dgf = startup ? &StartCMD : &PostCMD;

	// #ifndef DIAGEN_TEST
	int aciCurCredits;
	// #endif
	dgf->reset();
	char *p = dgf->getElement(DGF_NONE, 1);
	int i;
	int ret = 0;
	while (p) {
		i = 0;
		do
			if (!strcmp(p, CmdData[i].name))
				break;
		while (CmdData[++i].name);
		if (!CmdData[i].name)
			ErrH.Abort("Unknown CMD", XERR_USER, -1, p);
#ifdef DIAGEN_TEST
		if (!checkStatus)
			std::cout << "   [DOING]: " << CmdData[i].name << "\n";
#endif
		if (checkStatus) {
			if (CmdData[i].id == CMD::ABORT)
				dgAbortStatus = 1;
		} else
			switch (CmdData[i].id) {
			case CMD::CONTINUE:
				ret++;
				break;
			case CMD::CLEAR:
				ClearStatus = 1;
				break;
			case CMD::EPROMOTION:
				owner->owner->eStatus++;
				owner->updateLevel();
				break;
			case CMD::BPROMOTION:
				owner->owner->bStatus++;
				owner->updateLevel();
				break;
			case CMD::SPROMOTION:
				owner->owner->sStatus++;
				owner->updateLevel();
				break;
			case CMD::BDEGRADATION:
				if (owner->owner->bStatus) {
					owner->owner->bStatus--;
					// #ifndef DIAGEN_TEST
					uvsLuckChange(-15);
					// #endif
				}
				owner->updateLevel();
				// #ifndef DIAGEN_TEST
				GamerResult.game_ker_result = 0;
				GamerResult.game_pip_result = 0;
				// #endif
				break;
			case CMD::MAKE_WEEZYK:
				owner->owner->zStatus = 2;
				break;
			case CMD::REGISTER_WEEZYK:
				owner->owner->zStatus = 1;
				owner->updateLevel();
				break;
			case CMD::COMINGOZERO:
				owner->owner->seekR("O", 1)->comingCounter = 0;
				break;
			case CMD::RUBEECATION:
				wasRubeecated = 1;
				break;
				// #ifndef DIAGEN_TEST
			case CMD::ABORT:
				dg_SendEvent(AML_KICK_OUT_FROM_ESCAVE);
				eventOUT = 1;
				break;
			case CMD::QUIT:
				dg_SendEvent(AML_ABORT_ESCAVE_SESSION);
				eventOUT = 1;
				break;
			case CMD::BLOCK:
				dg_SendEvent(AML_BLOCK_SHOP);
				break;
			case CMD::UNBLOCK:
				dg_SendEvent(AML_UNBLOCK_SHOP);
				break;
			case CMD::FURY_LEVEL_INCREASE:
				IncFuryFactor();
				break;
			case CMD::FURY_LEVEL_DECREASE:
				DecFuryFactor();
				break;
			case CMD::ELR_WAITING_DECREASE:
				RACE_WAIT = 5 * RACE_WAIT / 6;
				break;
			case CMD::ELP_PRIZE_GIFT:
				aciCurCredits = aciGetCurCredits();
				aciCurCredits += 3000;
				aciUpdateCurCredits(aciCurCredits);
				break;
			case CMD::BBR_PRIZE_GIFT:
				aciCurCredits = aciGetCurCredits();
				aciCurCredits += 5000;
				aciUpdateCurCredits(aciCurCredits);
				break;
			case CMD::PENALTY1000:
				aciCurCredits = aciGetCurCredits();
				aciCurCredits -= 1000;
				if (aciCurCredits < 0)
					aciCurCredits = 0;
				aciUpdateCurCredits(aciCurCredits);
				break;
			case CMD::PENALTY10000:
				aciCurCredits = aciGetCurCredits();
				aciCurCredits -= 10000;
				if (aciCurCredits < 0)
					aciCurCredits = 0;
				aciUpdateCurCredits(aciCurCredits);
				break;
			case CMD::KEY2GLORX_GIFT:
				aciOpenWorldLink(0, 1);
				break;
			case CMD::KEY2NECROSS_GIFT:
				aciOpenWorldLink(1, 2);
				break;
			case CMD::KEY2XPLO_GIFT:
				aciOpenWorldLink(1, 3);
				break;
			case CMD::KEY2WEEXOW_GIFT:
				aciOpenWorldLink(0, 6);
				break;
			case CMD::KEY2BOOZEENA_GIFT:
				aciOpenWorldLink(2, 5);
				break;
			case CMD::KEY2KHOX_GIFT:
				aciOpenWorldLink(1, 4);
				break;
			case CMD::KEY2ARKAZNOY_GIFT:
				aciOpenWorldLink(2, 9);
				break;
			case CMD::KEY2THREALL_GIFT:
				aciOpenWorldLink(3, 8);
				break;
			case CMD::EXPROPRIATION:
				dg_SendEvent(AML_EXPROPRIATION);
				break;
			case CMD::ZERO_CASH:
				aciCurCredits = aciGetCurCredits();
				if (aciCurCredits > 600)
					aciCurCredits = 600;
				aciUpdateCurCredits(aciCurCredits);
				break;
			case CMD::LAMPASSOTHE_END:
				dg_SendEvent(AML_LAMPASSO_THEEND);
				break;
			case CMD::TRANSITION2V:
				uvsSetTownName("VigBoo");
				break;
			case CMD::TRANSITION2L:
				uvsSetTownName("Lampasso");
				break;
			case CMD::TRANSITION2O:
				uvsSetTownName("Ogorod");
				break;
			case CMD::TRANSITION2B:
				uvsSetTownName("B-Zone");
				break;
			case CMD::CHANGE2RAFFA:
				dg_SendEvent(AML_CHANGE_TO_RAFFA);
				break;
			case CMD::ACCEPT_RUBBOX:
				dg_SendEvent(AML_ACCEPT_RUBBOX);
				break;
			case CMD::ADD_RUBBOX:
				dg_SendEvent(AML_ADD_RUBBOX);
				break;
			case CMD::ZERO_KERRATING:
				GamerResult.game_ker_result = 0;
				break;
			case CMD::ZERO_PIPRATING:
				GamerResult.game_pip_result = 0;
				break;
			case CMD::ADD_PROTRACTORKILL:
				addInfernalsKill2Protractor();
				break;
			case CMD::ADD_MESSIAHKILL:
				addSpectorsKill2Messiah();
				break;
			case CMD::OPEN_H00:
				OpenCrypt(0, 0);
				break;
			case CMD::OPEN_H01:
				OpenCrypt(0, 1);
				break;
			case CMD::OPEN_H10:
				OpenCrypt(1, 0);
				break;
			case CMD::OPEN_H11:
				OpenCrypt(1, 1);
				break;
			case CMD::ZERO_BOORAWCHICK:
				uvsBoorawchickZero();
				break;
			case CMD::ZERO_ARKAZNOY:
				WorldTable[9]->GamerVisit = 0;
				break;
				// #endif
			}
		p = dgf->getElement(DGF_NONE, 1);
	}
	return ret;
}

// ----------------------------------- dgRoom ------------------------------------
void dgRoom::link(dgRoom *&tail) {
	if (!tail)
		tail = this;
	else {
		tail->prev->next = this;
		prev = tail->prev;
	}
	next = NULL;
	tail->prev = this;
}

void dgRoom::read(dgFile *pf) {
	roomName = getSubj(pf->getElement(DGF_NONE));
	escaveNames[0] = roomName;
	// #if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
	escaveNames[1] = pf->getElement(DGF_NONE);
	// #endif
	counsillorNames[0] = getSubj(pf->getElement(DGF_NONE));
	// #if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
	counsillorNames[1] = pf->getElement(DGF_NONE);
	// #endif
	if (!oldVersion)
		bios = atoi(pf->getElement(DGF_NONE));
	worldNames[0] = getSubj(pf->getElement(DGF_NONE));
	// #if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
	worldNames[1] = pf->getElement(DGF_NONE);
	// #endif
	startX = atoi(pf->getElement(DGF_NONE));
	startY = atoi(pf->getElement(DGF_NONE));
	int MPstartX = atoi(pf->getElement(DGF_NONE));
	int MPstartY = atoi(pf->getElement(DGF_NONE));
	if (NetworkON) {
		startX = MPstartX;
		startY = MPstartY;
	}
	if (!oldVersion)
		repeatable = atoi(pf->getElement(DGF_NONE));
	acceptTEXT();
	acceptDIL();
	acceptQUERY();
#ifdef DIAGEN_TEST
	dgMolecule *m = mtail;
	static XStream ff("Check.LOG", XS_OUT);
	ff < "***Escave: " < roomName < "\r\n";
	int i;
	while (m) {
		for (i = 0; i < gridSX * gridSY; i++)
			if (grid[i] && !strcmp(m->name, grid[i]->Name))
				break;
		if (i == gridSX * gridSY)
			ff < "Unattached molecule: " < m->name < "\r\n";
		m = m->next;
	}
	ff < "\r\n";
#endif
}

void dgRoom::acceptTEXT(void) {
	// std::cout<<"dgRoom::acceptTEXT "<<getDGname(roomName,".text")<<std::endl;
	textFile = std::make_unique<dgFile>(getDGname(roomName, ".text"));
	dgFile *dgf = textFile.get();

	char *p = NULL;
	int m;
	dgMolecule *pm;
	dgAtom *pa;
	while (1) {
		if (!p) {
			if (!(p = dgf->getElement(DGF_DUAL, 1)))
				break;
		}
		(pm = new dgMolecule(getSubj(p, m)))->link(mtail);
		pm->mood = m;
		p = dgf->getElement(DGF_DUAL);

		while (p) {
			if (*p == SUBJ_SYMBOL)
				break;
			(pa = new dgAtom)->link(pm->tail);
			p = pa->read(dgf, p);
		}
		if (!pm->tail) {
			XBuffer m;
			m < "Empty molecule: " < pm->name;
			ErrH.Abort(m.GetBuf(), XERR_USER, -1, roomName);
		}
		if (!p)
			break;
	}
}

void dgRoom::acceptDIL(void) {
	// std::cout<<"dgRoom::acceptDIL "<<getDGname(roomName,".dil")<<std::endl;
	dilFile = std::make_unique<dgFile>(getDGname(roomName, ".dil"));
	dgFile *dgf = dilFile.get();

	gridSX = atoi(dgf->getElement(DGF_NONE));
	gridSY = atoi(dgf->getElement(DGF_NONE));
	memset(grid = new dgCell *[gridSX * gridSY], 0, sizeof(dgCell *) * gridSX * gridSY);
	memset(status = new uchar[2 * gridSX * gridSY], DG_CELLSTATUS::EMPTY, 2 * gridSX * gridSY);
	backup_status = status + gridSX * gridSY;

	int num = atoi(dgf->getElement(DGF_NONE)), i;
	dgf->getElement(DGF_NONE);
	dgCell *pc;
	for (i = 0; i < num; i++) {
		(pc = new dgCell(this))->read(dgf);
		setGRID(pc->x, pc->y, pc);
		setSTATUS(pc->x, pc->y, DG_CELLSTATUS::DORMANT);
	}
	if (getSTATUS(startX, startY) != DG_CELLSTATUS::EMPTY) {
		setSTATUS(startX, startY, DG_CELLSTATUS::WAITING);
		explodeState(startX, startY);
	}
}

void dgRoom::acceptQUERY(void) {
	static dgAtom *adim[64];
	static int ldim[64];

	// std::cout<<"dgRoom::acceptQUERY "<<getDGname(roomName,".query")<<std::endl;
	queryFile = std::make_unique<dgFile>(getDGname(roomName, ".query"));
	dgFile *pf = queryFile.get();

	prefixQ = pf->getElement(DGF_DUAL, 1);
	postfixQ = pf->getElement(DGF_DUAL, 1);
	char *p = NULL;
	int m;
	dgQuery *pq;
	int i, j;
	while (1) {
		if (!p) {
			if (!(p = pf->getElement(DGF_DUAL, 1))) {
				break;
			}
		}
		(pq = new dgQuery)->link(qAtail);
		pq->explicit_id = p[0] == '[' && p[1] == '#';
		if (pq->explicit_id && std::strchr(p, ':'))
			ErrH.Abort("Invalid Diagen topic ID", XERR_USER, -1, roomName);
		pq->subj0 = getSubj(p, m);
		p = pf->getElement(DGF_DUAL);
		pq->subj1 = getSubj(p, m);

		if (pq->explicit_id) {
			pq->subj = pq->subj1;
		} else if (lang() == GERMAN) {
			pq->subj = pq->subj0;
		} else if (lang() == RUSSIAN) {
			pq->subj = pq->subj0;
		} else {
			pq->subj = pq->subj1;
		}

		pq->id = pq->explicit_id ? pq->subj0 + 1 : legacyQuestionId(pq->subj0, pq->subj1);
		if (pq->id.empty() || pq->id.size() > std::numeric_limits<short>::max() ||
			pq->id.compare(0, 8, "variant/") == 0 ||
			(pq->explicit_id && std::any_of(pq->id.begin(), pq->id.end(), [](unsigned char c) {
				return c < 32 || c >= 127 || c == '#' || c == '[' || c == ']' || c == '{' ||
					   c == '}';
			})))
			ErrH.Abort("Invalid Diagen topic ID", XERR_USER, -1, roomName);
		for (auto *q = qAtail; q; q = q->next)
			if (q != pq && q->id == pq->id)
				ErrH.Abort("Duplicate Diagen topic ID", XERR_USER, -1, pq->id.c_str());

		pq->mood = m;
		i = j = 0;
		p = pf->getElement(DGF_DUAL);
		while (p) {
			if (*p == SUBJ_SYMBOL) {
				break;
			}
			if (*p == COMMAND_SYMBOL) {
				i = ldim[j] = atoi(p + 1);
				p = pf->getElement(DGF_DUAL);
			} else {
				ldim[j] = i;
			}
			adim[j] = new dgAtom;
			p = adim[j++]->read(pf, p);
			i++;
		}
		if (!j) {
			XBuffer m;
			m < "Empty query in " < roomName;
			ErrH.Abort(m.GetBuf(), XERR_USER, -1, pq->subj);
		}
		pq->nanswer = j;
		pq->answers = new dgAtom *[2 * j];
		pq->levels = (int *)(pq->answers + j);
		for (i = 0; i < j; i++) {
			pq->answers[i] = adim[i];
			pq->levels[i] = ldim[i];
		}
		if (!p) {
			break;
		}
	}
}

void dgRoom::explodeState(int x, int y) {
	nextState(x - 1, y);
	nextState(x + 1, y);
	nextState(x, y - 1);
	nextState(x, y + 1);
}

void dgRoom::nextState(int x, int y) {
	if (x < 0 || x >= gridSX || y < 0 || y >= gridSY)
		return;
	if (getSTATUS(x, y) == DG_CELLSTATUS::DORMANT) {
		setSTATUS(x, y, DG_CELLSTATUS::WAITING);
		if (!getGRID(x, y)->isWaiting)
			explodeState(x, y);
	}
}

static void recoverSpobsSpectorsTask(dgRoom *room) {
	static const char protractorKillCellName[] = "Add Protractor InfernalsKill Function";

	if (checkStatus || strcmp(room->roomName, "Spobs") || room->owner->sStatus != 2 ||
		!getSpectorsTaskStatus())
		return;

	for (int i = 0; i < room->gridSX * room->gridSY; i++) {
		if (room->grid[i] && room->status[i] == DG_CELLSTATUS::USED &&
			!strcmp(room->grid[i]->Name, protractorKillCellName)) {
			room->status[i] = DG_CELLSTATUS::WAITING;
			break;
		}
	}
}

void dgRoom::startSession(void) {
	currentM = NULL;
	Qcode = eventOUT = 0;
	recoverSpobsSpectorsTask(this);
	owner->endSessionLog = isEmptySession();
	cIndex = -1;
	if (!isLoading) {
		singleRandAvailable = 1;
		ClearStatus = 0;
	}
	updateLevel();
	solidQ = (bios == 1);
	// #ifndef DIAGEN_TEST
	aciEscaveDead = uvsCurrentWorldUnableBefore;
	aciEscaveEmpty = (*roomName == 'V' && !uvsgetDGdata(DG_EXTERNS::HERE_BOORAWCHICK));
	// #endif
	if (checkStatus)
		backup();
}

void dgRoom::updateLevel(void) {
	switch (bios) {
	case 0:
		dgLevel = &owner->eStatus;
		break;
	case 1:
		dgLevel = &owner->bStatus;
		break;
	case 2:
		dgLevel = &owner->zStatus;
		break;
	case 3:
		dgLevel = &owner->sStatus;
		break;
	}
}

void dgRoom::endSession(void) {
	if (checkStatus)
		restore();
	else if (!repeatable && !((aciEscaveEmpty || aciEscaveDead))) {
		owner->restHandle(currentM);
		for (cIndex = 0; cIndex < gridSX * gridSY; cIndex++)
			switch (status[cIndex]) {
			case DG_CELLSTATUS::WAITING:
				if (grid[cIndex]->isACCESS()) {
					status[cIndex] = DG_CELLSTATUS::OPENED;
					currentM = seekM(grid[cIndex]->Name);
					if (currentM)
						currentM->reset();
					owner->restHandle(currentM);
					grid[cIndex]->doCMD(1);
					grid[cIndex]->doCMD(0);
				}
				break;
			}
	}
	currentM = NULL;
	if (!checkStatus && !(aciEscaveEmpty || aciEscaveDead)) {
		int i, j, ind = 0;
		for (j = 0; j < gridSY; j++)
			for (i = 0; i < gridSX; i++, ind++)
				switch (getSTATUS(i, j)) {
				case DG_CELLSTATUS::EXPLODED:
				case DG_CELLSTATUS::OPENED:
					if (grid[ind]->isLooping)
						setSTATUS(i, j, DG_CELLSTATUS::WAITING);
					else {
						explodeState(i, j);
						setSTATUS(i, j, DG_CELLSTATUS::USED);
					}
					break;
				case DG_CELLSTATUS::HALFLIFE:
					setSTATUS(i, j, DG_CELLSTATUS::WAITING);
					break;
				}
	}
}

int dgRoom::isEmptySession(void) {
	if (checkStatus)
		return 0;
	for (int i = 0; i < gridSX * gridSY; i++)
		switch (status[i]) {
		case DG_CELLSTATUS::WAITING:
			if (!grid[i]->isACCESS())
				continue;
		case DG_CELLSTATUS::OPENED:
			if (!strcasecmp(grid[i]->Name, "empty") || ClearStatus)
				continue;
			return 0;
		}
	return 1;
}

void dgRoom::locateActiveMolecule(void) {
	owner->varAtom = NULL;
	currentM = NULL;
	for (cIndex = 0; cIndex < gridSX * gridSY; cIndex++)
		switch (status[cIndex]) {
		case DG_CELLSTATUS::WAITING:
			if (!grid[cIndex]->isACCESS())
				continue;
			status[cIndex] = DG_CELLSTATUS::OPENED;
		case DG_CELLSTATUS::OPENED:
			if (!strcasecmp(grid[cIndex]->Name, "empty") || ClearStatus)
				continue;
			currentM = seekM(grid[cIndex]->Name);
			currentM->reset();
#ifdef DIAGEN_TEST
			if (!checkStatus)
				std::cout << "\n   Molecule [" << currentM->name << "] involved (mood "
						  << currentM->mood << "):" << "\n";
#endif
			if (grid[cIndex]->doCMD())
				explodeState(cIndex % gridSX, cIndex / gridSX);
#ifdef DIAGEN_TEST
			if (!checkStatus)
				std::cout << "\n";
#endif
			status[cIndex] = repeatable ? DG_CELLSTATUS::HALFLIFE : DG_CELLSTATUS::EXPLODED;
			return;
		}
	cIndex--;
}

dgMolecule *dgRoom::getCurrentMolecule(void) {
	if (currentM && !currentM->curA && cIndex >= 0 && grid[cIndex]) {
		if (repeatable && status[cIndex] == DG_CELLSTATUS::HALFLIFE)
			status[cIndex] = DG_CELLSTATUS::EXPLODED;
		grid[cIndex]->doCMD(0);
	}
	if (!currentM || (currentM && !currentM->curA))
		locateActiveMolecule();

	return currentM;
}

dgQuery *dgRoom::seekQall(const char *id) {
	if (!id)
		return NULL;
	dgQuery *q = qAtail;
	while (q) {
		if (q->id == id)
			return q;
		q = q->next;
	}
	// #ifndef DIAGEN_TEST
	//	ErrH.Abort("Query not found",XERR_USER,-1,id);
	// #endif
	return NULL;
}

dgQuery *dgRoom::seekQvisible(const char *id) {
	if (!id)
		return NULL;
	dgQuery *q = qVtail;
	while (q) {
		if (q->id == id)
			return q;
		q = q->next;
	}
	// #ifndef DIAGEN_TEST
	//	ErrH.Abort("Query not found",XERR_USER,-1,id);
	// #endif
	return NULL;
}

namespace {
struct LegacyTopic {
	const char *id, *ru, *en;
};
const LegacyTopic legacy_topics[]{
#include "legacy_topics.inc"
};
std::string legacyQuestionId(const char *ru, const char *en) {
	for (const auto &old : legacy_topics)
		if (!strcmp(ru, old.ru) && !strcmp(en, old.en))
			return old.id;
	// Unrecognized historical mod resources still retain their old two-header contract.
	return std::string("legacy/") + en;
}
struct LegacyAnswerIndices {
	const char *room, *topic;
	int count, indices[4];
};
const LegacyAnswerIndices legacy_answer_indices[]{
#include "legacy_answer_indices.inc"
};
const LegacyAnswerIndices *answerIndices(const dgRoom &room, const dgQuery &q) {
	for (const auto &old : legacy_answer_indices)
		if (q.id == old.topic && !strcmp(room.roomName, old.room))
			return &old;
	return nullptr;
}
int importAnswerIndex(const dgRoom &room, const dgQuery &q, std::string_view key, int index) {
	const auto *old = answerIndices(room, q);
	if (!old || index < 0 || index >= old->count)
		return index;
	for (const auto &alias : legacy_topics)
		if (q.id == alias.id && key == alias.en)
			return old->indices[index];
	return index; // Russian legacy subjects already use the canonical answer slots.
}
void normalizeLegacyAnswers(dgRoom &room) {
	if (lang() != ENGLISH)
		return;
	for (auto *q = room.qAtail; q; q = q->next) {
		if (q->explicit_id)
			continue;
		const auto *old = answerIndices(room, *q);
		if (!old || q->nanswer != old->count)
			continue;
		// Known legacy shape only; do not infer grouping for custom scripted answers.
		bool fragments = true;
		for (int i = 0; i < q->nanswer; ++i)
			if (q->levels[i] != i || q->answers[i]->nvariant)
				fragments = false;
		if (!fragments)
			continue;
		const std::vector<dgAtom *> original(q->answers, q->answers + q->nanswer);
		const int count = old->indices[old->count - 1] + 1;
		q->legacy_answers.resize(count);
		for (int target = 0; target < count; ++target) {
			dgAtom *keep = nullptr;
			auto &joined = q->legacy_answers[target];
			for (int i = 0; i < old->count; ++i) {
				if (old->indices[i] != target)
					continue;
				if (!keep)
					keep = original[i];
				if (!joined.empty())
					joined += ' ';
				joined += original[i]->data;
				if (original[i] != keep) {
					for (const auto &id : original[i]->question_ids)
						if (std::find(keep->question_ids.begin(), keep->question_ids.end(), id) ==
							keep->question_ids.end())
							keep->question_ids.push_back(id);
					delete original[i];
				}
			}
			keep->data = joined.data();
			q->answers[target] = keep;
			q->levels[target] = target;
		}
		q->nanswer = count;
	}
}
bool legacyAlias(const dgQuery &q, std::string_view text) {
	if (!q.explicit_id && (text == q.subj0 || text == q.subj1))
		return true;
	for (const auto &old : legacy_topics)
		if (q.id == old.id && (text == old.ru || text == old.en))
			return true;
	return false;
}
dgQuery *importTopic(dgRoom &room, std::string_view text) {
	// Frozen historical aliases take precedence over a coincidentally matching new label.
	dgQuery *match = nullptr;
	const auto find = [&](bool historical) {
		for (auto *q = room.qAtail; q; q = q->next)
			if (historical ? legacyAlias(*q, text) : text == q->subj) {
				if (match)
					ErrH.Abort("Ambiguous legacy Diagen saved topic", XERR_USER, -1, room.roomName);
				match = q;
			}
	};
	find(true);
	if (!match)
		find(false);
	return match;
}

std::string readSaveString(XStream &file, int length) {
	if (length < 0 || length > std::numeric_limits<short>::max())
		ErrH.Abort("Invalid Diagen save string length");
	std::string value(std::size_t(length) + 1, '\0');
	if (file.read(value.data(), value.size()) != value.size() || value.back() ||
		std::strlen(value.c_str()) != std::size_t(length))
		ErrH.Abort("Invalid or truncated Diagen save string");
	value.pop_back();
	return value;
}
void checkSaveLength(const char *value, std::size_t maximum) {
	if (std::strlen(value) > maximum)
		ErrH.Abort("Diagen save string too long");
}
} // namespace

void dgRoom::save(XStream &ff) {
	ff < visitCounter < comingCounter;

	int n = gridSX * gridSY;
	int cnt, i;
	char *name;
	for (int ind = DG_CELLSTATUS::USED; ind <= DG_CELLSTATUS::EXPLODED; ind++) {
		for (cnt = 0, i = 0; i < n; i++)
			if (status[i] == ind)
				cnt++;
		ff < (short)cnt;
		for (i = 0; i < n; i++)
			if (status[i] == ind) {
				name = seekM(grid[i]->Name)->name;
				checkSaveLength(name, std::numeric_limits<uchar>::max());
				ff < (uchar)strlen(name);
				ff.write(name, strlen(name) + 1);
			}
	}

	int num = 0;
	dgQuery *q = qVtail;
	while (q) {
		num++;
		q = q->next;
	}
	ff < num;
	q = qVtail;
	while (q) {
		checkSaveLength(q->id.c_str(), std::numeric_limits<short>::max());
		ff < (short)q->id.size() < (short)q->lindex;
		ff.write(q->id.c_str(), q->id.size() + 1);
		q = q->next;
	}
}

void dgRoom::load(XStream &ff) {
	currentM = NULL;
	ff > visitCounter > comingCounter;

	int n = gridSX * gridSY, i, ind;
	short cnt;
	uchar l;

	/*
		if(loadedVersion == 0){
			ff > cIndex;
			dgRoom* r = nDD -> seekR(roomName);
			ff.read(r -> status,gridSX*gridSY);
			for(ind = DG_CELLSTATUS::USED;ind <= DG_CELLSTATUS::EXPLODED;ind++){
				for(i = 0;i < n;i++)
					if(r -> status[i] == ind && r -> grid[i])
						setCell(r -> grid[i] -> Name,ind);
				}
			}
		else {
	*/
	for (ind = DG_CELLSTATUS::USED; ind <= DG_CELLSTATUS::EXPLODED; ind++) {
		ff > cnt;
		for (i = 0; i < cnt; i++) {
			ff > l;
			auto name = readSaveString(ff, l);
			setCell(name.data(), ind);
		}
	}
	//		}

	dgQuery *q;
	int num;
	short len, li;
	ff > num;
	int available = 0;
	for (auto *closed = qAtail; closed; closed = closed->next)
		++available;
	if (num < 0 || (loadedVersion >= 5 && num > available))
		ErrH.Abort("Invalid Diagen saved topic count");
	for (i = 0; i < num; i++) {
		ff > len > li;
		auto subject = readSaveString(ff, len);
		if (loadedVersion < 4)
			subject = vangers::settings::cp1251_to_utf8(subject);
		else if (!vangers::settings::is_valid_utf8(subject))
			ErrH.Abort("Invalid UTF-8 topic in Diagen save");
		q = loadedVersion >= 5 ? seekQall(subject.c_str()) : importTopic(*this, subject);
		if (!q && loadedVersion >= 5)
			ErrH.Abort("Unknown Diagen saved topic ID", XERR_USER, -1, subject.c_str());
		if (q) {
			if (li < 0)
				ErrH.Abort("Invalid Diagen saved answer index", XERR_USER, li, q->id.c_str());
			(q->delink(qAtail))->link(qVtail);
			q->lindex = loadedVersion < 5 ? importAnswerIndex(*this, *q, subject, li) : li;
		}
	}
}

void dgRoom::setCell(char *name, int value) {
	for (int i = 0; i < gridSX * gridSY; i++)
		if (grid[i] && !strcasecmp(name, grid[i]->Name))
			status[i] = value;
	//	ErrH.Abort("Wrong setCell() working",XERR_USER,-1,name);
}

dgMolecule *dgRoom::seekM(const char *_name) {
	dgMolecule *m = mtail;
	while (m) {
		if (!strcmp(m->name, _name))
			return m;
		m = m->next;
	}
#ifdef DIAGEN_TEST
	XBuffer buf;
	buf < _name < "/" < roomName;
	ErrH.Abort("Molecule not found", XERR_USER, -1, buf.GetBuf());
#else
//	ErrH.Abort("Molecule not found",XERR_USER,-1,_name);
#endif
	return NULL;
}

#ifdef DIAGEN_TEST
void dgRoom::test(void) {
	int i = 0;
	while (ArgData[i].name)
		i++;
	if (i != DG_EXTERNS::MAX)
		ErrH.Abort("MAX comparing failed!");

#	ifdef STARTUP_TESTING
	std::cout << "*** " << roomName << " ***\n";
	dgMolecule *m;
	owner->currentR = this;
	for (i = 0; i < gridSX * gridSY; i++)
		if (grid[i]) {
			grid[i]->isACCESS();
			grid[i]->doCMD(1);
			grid[i]->doCMD(0);
			m = seekM(grid[i]->Name);
			m->reset();
			while (m->getPhrase(bios == 4))
				m->goNext();
			m->reset();
		}
	owner->currentR = NULL;
	console_clear();
#	endif
}
#endif

// ----------------------------------- DiagenDispatcher ------------------------------------
char *DiagenDispatcher::getQprefix(void) {
	if (!currentR)
		return NULL;
	return varAtom ? NULL : currentR->prefixQ;
}
char *DiagenDispatcher::getQpostfix(void) {
	if (!currentR)
		return NULL;
	return varAtom ? NULL : currentR->postfixQ;
}
char *DiagenDispatcher::getQempty(void) {
	return currentR ? currentR->seekM("Empty Question")->tail->data : NULL;
}
char *DiagenDispatcher::getQend(void) {
	return currentR ? (!ClearStatus ? currentR->seekM("Empty Molecule")->tail->data : (char *)"")
					: NULL;
}
char *DiagenDispatcher::getQblock(void) {
	return currentR ? currentR->seekM("Block Molecule")->tail->data : NULL;
}
char *DiagenDispatcher::getQout(void) {
	dgMood = 3;
	return currentR ? currentR->seekM("Out Molecule")->tail->data : NULL;
}
char *DiagenDispatcher::getQdead(void) {
	if (!currentR)
		return NULL;
	if (*currentR->roomName == 'P' && isSpummyDeath())
		return currentR->seekM("Last Letter")->tail->data;
	return currentR->seekM("Dead Molecule")->tail->data;
}

void DiagenDispatcher::init(void) {
	std::cout << "DiagenDispatcher::init " << getDGname("room", ".lst") << std::endl;
	FBox = new FileBox;
#ifndef DIAGEN_TEST
	FBox->load();
#endif
	roomFile = std::make_unique<dgFile>(getDGname("room", ".lst"), 0, true);
	dgFile *pf = roomFile.get();
	char *p = NULL;
	dgRoom *pr;
	int i, j;
	int max = oldVersion ? 8 : DG_ESCAVE_MAX;
	for (i = 0; i < max; i++) {
		(pr = new dgRoom(this))->link(rtail);
		pr->read(pf);
	}
	if (!oldVersion) {
		for (j = 0; j < 3; j++) {
			for (i = 0; i < 3; i++)
				CycleName[0][j][i] = getSubj(pf->getElement(DGF_NONE));
			// #if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
			for (i = 0; i < 3; i++)
				CycleName[1][j][i] = pf->getElement(DGF_NONE);
			// #endif
		}
		for (i = 0; i < DG_SECRETW_MAX; i++)
			SecretWname[0][i] = getSubj(pf->getElement(DGF_NONE));
		// #if defined(RUSSIAN_VERSION) || defined(DIAGEN_TEST)
		for (i = 0; i < DG_SECRETW_MAX; i++)
			SecretWname[1][i] = pf->getElement(DGF_NONE);
		// #endif
	}

	// stalkerg Тест диалогов и скриптов, нужно отключить для большого теста
	/*#ifdef DIAGEN_TEST
		pr = rtail;
		while(pr){
			pr -> test();
			pr = pr -> next;
			}
	#endif*/

	invR = new dgRoom(this);
	invR->roomName = "Inventory";
	invR->acceptTEXT();
	resolveQuestionLinks();
	for (auto *r = rtail; r; r = r->next)
		normalizeLegacyAnswers(*r);

#ifdef DIAGEN_TEST
	FBox->save();
#endif
}

char *DiagenDispatcher::findQfirst(void) {
	if (varAtom)
		return varAtom->variants[cVind = 0];
	else {
		if (!currentR)
			return NULL;
		cqp = currentR->qVtail;
		return cqp ? cqp->subj : NULL;
	}
}

char *DiagenDispatcher::findQnext(void) {
	if (varAtom) {
		if (++cVind >= varAtom->nvariant)
			return NULL;
		return varAtom->variants[cVind];
	} else {
		cqp = cqp->next;
		return cqp ? cqp->subj : NULL;
	}
}

void DiagenDispatcher::startSession(const char *rname) {
	if (currentR)
		endSession();

	dgAbortStatus = 0;
	endSessionLog = 0;
	dgRoom *r = seekR(rname);
#ifdef DIAGEN_TEST
	if (!r) {
		std::cout << "\nError: room not found...\n\n";
		return;
	}
	std::cout << "\n" << strLine << " SESSION IN " << r->roomName << " started" << strLine << "\n";
	checkSession(rname);
	if (dgAbortStatus)
		std::cout << "This session will be aborted!\n";
	std::cout << "\n";
#endif
	(currentR = r)->startSession();
	if (!isLoading) {
		if (currentR != lastR)
			currentR->visitCounter++;
		currentR->comingCounter++;
	}
	isLoading = 0;
	enter_sStatus = sStatus;
}

void DiagenDispatcher::checkSession(const char *rname) {
	if (currentR)
		endSession();
	dgRoom *r = seekR(rname);

	checkStatus = 1;
	(currentR = r)->startSession();
	while (!dgAbortStatus && !endSessionLog)
		getNextPhrase();
	endSession();
	checkStatus = 0;
}

void DiagenDispatcher::endSession(void) {
	if (currentR) {
#ifdef DIAGEN_TEST
		if (!checkStatus)
			std::cout << "\n"
					  << strLine << " SESSION IN " << currentR->roomName << " finished" << strLine
					  << "\n\n";
#endif
		currentR->endSession();
		if (!checkStatus)
			lastR = currentR;
		currentR = NULL;
	}
}

void DiagenDispatcher::resolveQuestionLinks() {
	std::unordered_map<std::string, std::vector<std::string>> aliases;
	std::unordered_set<std::string> known;
	bool legacy = false;
	const auto add_alias = [&](const char *name, const std::string &id) {
		auto &ids = aliases[name];
		if (std::find(ids.begin(), ids.end(), id) == ids.end())
			ids.push_back(id);
	};
	for (auto *r = rtail; r; r = r->next)
		for (auto *q = r->qAtail; q; q = q->next) {
			known.insert(q->id);
			if (!q->explicit_id) {
				legacy = true;
				add_alias(q->subj, q->id);
			}
		}
	const auto resolve = [&](dgAtom *atom) {
		for (auto *p = atom->findQLfirst(); p; p = atom->findQLnext()) {
			std::vector<std::string> ids;
			if (*p == '#') {
				if (!known.count(p + 1))
					ErrH.Abort("Unknown Diagen linked topic ID", XERR_USER, -1, p + 1);
				ids.emplace_back(p + 1);
			} else {
				if (!legacy && !known.empty())
					ErrH.Abort("Unkeyed Diagen link in explicit-ID resources", XERR_USER, -1, p);
				if (auto old = aliases.find(p); old != aliases.end())
					ids = old->second;
			}
			for (const auto &id : ids)
				if (std::find(atom->question_ids.begin(), atom->question_ids.end(), id) ==
					atom->question_ids.end())
					atom->question_ids.push_back(id);
		}
	};
	for (auto *r = rtail; r; r = r->next) {
		for (auto *m = r->mtail; m; m = m->next)
			for (auto *a = m->tail; a; a = a->next)
				resolve(a);
		for (auto *q = r->qAtail; q; q = q->next)
			for (int i = 0; i < q->nanswer; ++i)
				resolve(q->answers[i]);
	}
	for (auto *m = invR->mtail; m; m = m->next)
		for (auto *a = m->tail; a; a = a->next)
			resolve(a);
}

void DiagenDispatcher::involveQ(dgAtom *atom) {
	if (!atom)
		return;
	for (const auto &id : atom->question_ids)
		for (auto *r = rtail; r; r = r->next)
			if (auto *q = r->seekQall(id.c_str()))
				(q->delink(r->qAtail))->link(r->qVtail);
}

char *DiagenDispatcher::getNextPhrase(void) {
	if (!currentR) {
		return NULL;
	}

	dgMolecule *m = currentR->getCurrentMolecule();

	char *phr = NULL;
	if (eventOUT) {
		endSessionLog = 1;
		return NULL;
	}
	if (m) {
		dgMood = m->mood;
		involveQ(m->curA);
		phr = m->getPhrase(currentR->bios == 4);
		varAtom = m->getVariantsAtom();
		// #ifndef DIAGEN_TEST
		if (varAtom)
			dg_SendEvent(AML_GET_QUESTION);
		// #endif
		m->goNext();
	}

	if (!phr) {
		phr = getQend();
		endSessionLog = 1;
	} else {
		endSessionLog = 0;
	}
	dgLog(phr);
	return phr;
}

void DiagenDispatcher::restHandle(dgMolecule *m) {
	if (!m)
		return;
	while (m->curA) {
		involveQ(m->curA);
		m->goNext();
	}
}

dgRoom *DiagenDispatcher::seekR(const char *_name, int onlyfirstchar) {
	if (!*_name)
		return NULL;
	dgRoom *r = rtail;
	if (!onlyfirstchar)
		while (r) {
			if (!strcmp(r->roomName, _name))
				return r;
			r = r->next;
		}
	else
		while (r) {
			if (*r->roomName == *_name)
				return r;
			r = r->next;
		}
	// #ifndef DIAGEN_TEST
	ErrH.Abort("Room not found", XERR_USER, -1, _name);
	// #endif
	return NULL;
}

#ifdef DIAGEN_TEST
void DiagenDispatcher::getStatus(void) {
	console_clear();
	std::cout << "\nStatus:\n";
	if (currentR) {
		std::cout << "\troom: " << currentR->roomName << "\n";
		std::cout << "\tperson: " << currentR->counsillorNames[lang() == RUSSIAN ? 1 : 0] << "\n";
		if (dgLevel)
			std::cout << "\tlevel: " << *dgLevel << "\n";
	} else
		std::cout << "\tin fly...\n";
}
#endif

std::string DiagenDispatcher::currentQuestionId() const {
	if (varAtom)
		return cVind >= 0 && cVind < varAtom->nvariant ? "variant/" + std::to_string(cVind)
													   : std::string();
	return cqp ? cqp->id : std::string();
}

char *DiagenDispatcher::getAnswer(char *subject) {
	// Compatibility for the interactive debug console, not game/menu identity.
	if (!subject)
		return nullptr;
	if (varAtom) {
		for (int i = 0; i < varAtom->nvariant; ++i)
			if (!strcmp(varAtom->variants[i], subject))
				return getAnswerById(("variant/" + std::to_string(i)).c_str());
	} else if (currentR) {
		for (auto *q = currentR->qVtail; q; q = q->next)
			if (!strcmp(q->subj, subject))
				return getAnswerById(q->id.c_str());
	}
	return nullptr;
}

char *DiagenDispatcher::getAnswerById(const char *id) {
	if (!id)
		return nullptr;
	if (varAtom) {
		for (int i = 0; i < varAtom->nvariant; ++i) {
			if ("variant/" + std::to_string(i) != id)
				continue;
			char *ret = nullptr;
			Qcode = varAtom->getVcode(i, ret);
			dg_SendEvent(Qcode);
			dg_SendEvent(AML_CLEAR_QUESTION);
			varAtom = nullptr;
			return ret;
		}
		return nullptr;
	}
	if (!currentR || aciEscaveEmpty || aciEscaveDead)
		return nullptr;
	if (auto *q = currentR->seekQvisible(id)) {
		dgMood = q->mood;
		const int n = q->getIndex();
		char *str = q->answers[n]->data;
		involveQ(q->answers[n]);
		return str;
	}
	return nullptr;
}

void DiagenDispatcher::save(XStream &ff) {
	/*#ifdef DIAGEN_TEST
		ff.open("test.sav",XS_OUT);
	#endif*/

	ff < saveVersion;
	ff < eStatus < bStatus < zStatus < sStatus;
	ff < singleRandAvailable < ClearStatus < wasRubeecated;

	dgRoom *r = rtail;
	int num = 0;
	while (r) {
		num++;
		r = r->next;
	}
	ff < num;
	r = rtail;
	while (r) {
		checkSaveLength(r->roomName, std::numeric_limits<short>::max());
		ff < (short)strlen(r->roomName);
		ff.write(r->roomName, strlen(r->roomName) + 1);
		r->save(ff);
		r = r->next;
	}
}

void DiagenDispatcher::load(XStream &ff) {
	std::cout << "DiagenDispatcher::load" << std::endl;
	/*#ifdef DIAGEN_TEST
		ff.open("test.sav",XS_IN);
	//	ff.seek(68734,XS_BEG);
	#endif*/
	isLoading = 1;

	ff > loadedVersion;
	//	if(loadedVersion < 0 || loadedVersion > saveVersion) ErrH.Abort("Intolerable dgVersion");
	if (loadedVersion < 2 || loadedVersion > saveVersion)
		ErrH.Abort("Intolerable dgVersion");

	/*	if(loadedVersion == 0){
			strcpy(DiagenTextName + strlen(DiagenTextName) - 4,"lst");
			oldVersion = 1;
			nDD = new DiagenDispatcher;
			nDD -> init();
			strcpy(DiagenTextName + strlen(DiagenTextName) - 3,"text");
			oldVersion = 0;
			}
	*/
	ff > eStatus > bStatus > zStatus;
	if (loadedVersion >= 2)
		ff > sStatus;
	ff > singleRandAvailable > ClearStatus;
	wasRubeecated = 0;
	if (loadedVersion >= 3)
		ff > wasRubeecated;

	int num;
	short len;
	dgRoom *r, *rr;
	ff > num;
	for (int i = 0; i < num; i++) {
		ff > len;
		auto name = readSaveString(ff, len);
		r = seekR(name.c_str());
		if (r)
			r->load(ff);
	}

	dgQuery *q, *qq;
	r = rtail;
	while (r) {
		q = r->qVtail;
		while (q) {
			rr = rtail;
			while (rr) {
				if ((qq = rr->seekQall(q->id.c_str())) != NULL)
					(qq->delink(rr->qAtail))->link(rr->qVtail);
				rr = rr->next;
			}
			q = q->next;
		}
		r = r->next;
	}
}

char *DiagenDispatcher::getWorldName(char *name) {
	if (name) {
		if (const auto *localized =
				vangers::localization::catalog().find_text(std::string("world.") + name))
			return const_cast<char *>(localized->c_str());
		dgRoom *r = rtail;
		while (r) {
			if (!strcmp(name, r->worldNames[0]))
				return r->worldNames[lang() == RUSSIAN ? 1 : 0];
			r = r->next;
		}
		for (int i = 0; i < DG_SECRETW_MAX; i++)
			if (!strcmp(name, SecretWname[0][i]))
				return SecretWname[lang() == RUSSIAN ? 1 : 0][i];
	}
	ErrH.Abort("dgWorldName not found");
	return NULL;
}

char *DiagenDispatcher::getInvText(int type, int id, char *from, char *to) {
	static XBuffer buf(64);
	dgMolecule *m;
	buf.init();
	switch (type) {
	case DG_POPONKA:
		buf < "Poponka " <= id;
		m = invR->seekM(buf.GetBuf());
		return m->tail->data;
	case DG_TABUTASK:
		buf < "Tabutask " < *from <= id + 1;
		m = invR->seekM(buf.GetBuf());
		return m->tail->data;
	case DG_PASSENGER:
		buf < "Passenger " < *from < "->" < *to < "";
		m = invR->seekM(buf.GetBuf());
		return m->tail->data;
	case DG_FINAL:
		buf < "Final " <= id;
		m = invR->seekM(buf.GetBuf());
		return m->tail->data;
	}
	return NULL;
}

void FileBox::load(void) {
#ifdef _ROAD_
	Parser ff(DiagenTextName());
#else
	XStream ff(DiagenTextName(), XS_IN);
#endif
	const auto read = [&](void *destination, std::size_t count) {
#ifdef _ROAD_
		if (ff.tell() > ff.length() || count > ff.length() - ff.tell())
			ErrH.Abort("Truncated Diagen archive", XERR_USER, -1, DiagenTextName());
#endif
		if (ff.read(destination, count) != count)
			ErrH.Abort("Truncated Diagen archive", XERR_USER, -1, DiagenTextName());
	};
	char signature[2];
	read(signature, 2);
	const bool utf8 = !memcmp(signature, "DU", 2);
	if (!utf8 && memcmp(signature, "DG", 2))
		ErrH.Abort("Unknown Diagen archive encoding", XERR_USER, -1, DiagenTextName());
	read(&cN, sizeof(cN));
#ifdef _ROAD_
	const auto remaining = ff.length() - ff.tell();
#else
	const auto remaining = ff.size() - ff.tell();
#endif
	if (cN < 0 || std::size_t(cN) > remaining / 6)
		ErrH.Abort("Invalid Diagen archive entry count");
	names = new char *[cN]{};
	lens = new int[cN]{};
	data = new char *[cN]{};
	for (int i = 0; i < cN; ++i) {
		uchar length;
		read(&length, 1);
		const int name_size = int(length) + 1;
		names[i] = new char[name_size];
		read(names[i], name_size);
		if (names[i][name_size - 1] || std::strlen(names[i]) != std::size_t(length))
			ErrH.Abort("Invalid Diagen archive entry name");
		int size;
		read(&size, sizeof(size));
#ifdef _ROAD_
		if (size < 0 || std::size_t(size) > ff.length() - ff.tell())
#else
		if (size < 0 || std::size_t(size) > ff.size() - ff.tell())
#endif
			ErrH.Abort("Invalid Diagen archive entry length", XERR_USER, -1, names[i]);
		std::string bytes(size, '\0');
		read(bytes.data(), bytes.size());
		if ((!bytes.empty() && bytes.back()) || (utf8 && !vangers::settings::is_valid_utf8(bytes)))
			ErrH.Abort("Invalid Diagen text", XERR_USER, -1, names[i]);
		const auto text = utf8 ? bytes : vangers::settings::cp1251_to_utf8(bytes);
		if (text.size() > std::size_t(std::numeric_limits<int>::max()))
			ErrH.Abort("Diagen text too long");
		lens[i] = int(text.size());
		data[i] = new char[text.size() + 1];
		std::memcpy(data[i], text.data(), text.size());
		data[i][text.size()] = 0;
	}
}

char *FileBox::get(char *fname, int &len) {
	for (int i = 0; i < cN; i++) {
		if (!strcmp(fname, names[i])) {
			len = lens[i];
			return data[i];
		}
	}

	ErrH.Abort("FileBox: entry not found", XERR_USER, -1, fname);
	return NULL;
}

#ifdef DIAGEN_TEST
void FileBox::add(char *fname, char *buf, int len) {
	if (cN >= 256 || strlen(fname) > std::numeric_limits<uchar>::max())
		ErrH.Abort("Diagen source archive too large");
	names[cN] = new char[strlen(fname) + 1];
	strcpy(names[cN], fname);
	lens[cN] = len;
	memcpy(data[cN] = new char[len], buf, len);
	cN++;
}

void FileBox::save(void) {
	static int ret = 0;
	if (ret)
		return;

	XStream ff("diagen_new.text", XS_OUT); // see definition of DIAGEN_TEXT

	ff.write("DU", 2);
	ff < cN;
	for (int i = 0; i < cN; i++) {
		ff < (uchar)strlen(names[i]);
		ff.write(names[i], strlen(names[i]) + 1);
		ff < lens[i];
		ff.write(data[i], lens[i]);
	}
	ff.close();
	std::cout << "Diagen.text saved..." << std::endl;
	ret = 1;
}
#endif

void dgInThreall(void) {
	dgD->startSession("Threall");
}
void dgOutThreall(void) {
	dgD->endSession();
}
char *dgGetThreallPhrase(void) {
	return dgD->getNextPhrase();
}
