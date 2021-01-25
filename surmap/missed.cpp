//
// Created by caiiiycuk on 15.01.2021.
//
#include <SDL.h>
#include <unordered_map>

#if defined(__unix__) || defined(__APPLE__)
#include <dirent.h>
#endif

typedef unsigned char uchar;

#include "xglobal.h"
#include "xstream.h"
#include "xtcore.h"

#include "../src/3d/3d_math.h"
#include "../src/3d/3dobject.h"
#include "../src/3d/parser.h"
#include "../src/actint/item_api.h"
#include "../src/common.h"
#include "../src/dast/poly3d.h"
#include "../src/units/hobj.h"
#include "../src/units/uvsapi.h"
#include "../src/units/items.h"
#include "../src/lang.h"
#include "../src/network.h"
#include "../src/sound/hsound.h"
#include "../src/xjoystick.h"

#include "missed.h"

extern "C" {
#include <iniparser/iniparser.h>
}

int NetworkON = 0;
int debug_view = 0;

char * iniparser_getstring_surmap(void * d, const char * cat, const char * key) {
	return iniparser_getstring((dictionary*) d, (std::string(cat) + ":" + key).c_str(), NULL);
}

XConWrapper& XConWrapper::operator<(const char* str) {
	printf("%s", str);
	return *this;
}

XConWrapper& XConWrapper::operator<=(int value) {
	printf("%d", value);
	return *this;
}

XConWrapper XCon;

XKeyWrapper* keyWrapper = 0;
std::function<void(int)> keyHandler;
std::unordered_map<int, bool> keyStates;
void onKeyDown(SDL_Event* e) {
	if (!keyHandler || e->type != SDL_KEYDOWN) {
		return;
	}

	keyHandler(e->key.keysym.sym);
	keyStates[e->key.keysym.sym] = true;
}
void onKeyUp(SDL_Event* e) {
	if (!keyHandler || e->type != SDL_KEYUP) {
		return;
	}
	keyStates[e->key.keysym.sym] = false;
}
XKeyWrapper::XKeyWrapper() {
	keyWrapper = this;
}
void XKeyWrapper::init(std::function<void(int)> handlerFn, void*) {
	set_key_handlers(onKeyDown, onKeyUp);
	keyHandler = handlerFn;
}
void XKeyWrapper::finit() {
}
bool XKeyWrapper::Pressed(int sdlkKey) {
	return keyStates[sdlkKey] == true;
}

XKeyWrapper XKey;

void mainWinMinimize() {
}

void mainWinMaximize() {
}

#if !(defined(__unix__) || defined(__APPLE__))
// implementation is in moveland.cpp
#else
int lastSearchCount = 0;
struct dirent **lastSearch = nullptr;
std::string extToSearch;

//http://www.delorie.com/djgpp/doc/libc/libc_167.html
char* win32_findnext() {
	if (lastSearchCount == 0 || lastSearch == nullptr) {
		return nullptr;
	}

	lastSearchCount--;
	lastSearch--;

	return (*lastSearch)->d_name;
}

//http://www.delorie.com/djgpp/doc/libc/libc_166.html
char* win32_findfirst(const char* cMask) {
	if (lastSearch != nullptr) {
		free(lastSearch);
		lastSearch = nullptr;
	}

	std::string maskAndPath = cMask;
	std::replace(maskAndPath.begin(), maskAndPath.end(), '\\', '/');
	auto lastSepIndex = maskAndPath.rfind('/');

	if (lastSepIndex == std::string::npos) {
		return nullptr;
	}

	auto path = maskAndPath.substr(0, lastSepIndex);
	auto mask = maskAndPath.substr(lastSepIndex + 1, maskAndPath.length() - lastSepIndex - 1);

	if (mask.length() < 3 || mask[0] != '*' || mask[1] != '.') {
		printf("Unsupported mask %s\n", cMask);
		abort();
	}

	extToSearch = mask.substr(1, mask.length() - 1);

	lastSearchCount = scandir(path.c_str(), &lastSearch, [](const struct dirent* next) -> int {
			std::string name = next->d_name;
			if (extToSearch.length() > name.length()) {
				return false;
			}
			return extToSearch == name.substr(name.length() - extToSearch.length(), extToSearch.length());
		}, alphasort);

	if (lastSearchCount < 0) {
		printf("scandir errored for path %s\n", path.c_str());
		return nullptr;
	}

	lastSearch += lastSearchCount;
	return win32_findnext();
}

#endif


// unused methods
void setLang(Language lang) {}
void StartEFFECT(EFFECT_VALUE EffectValue,int loop, int pan) {}
int iKeyPressed(int id) {
	return 0;
}
int XJoystickInput() {
	return 0;
}
void SetMotorSound(int speed) {}
InputEventBuffer::InputEventBuffer(unsigned int size): XBuffer(size) {}
OutputEventBuffer::OutputEventBuffer(unsigned int size): XBuffer(size) {}

// unimplemented methods, I think that they are unused but not sure
DebrisObject* DebrisList::CreateDebris(void) {
	abort();
}
void DebrisObject::CreateDebris(int id,int ind) {
	abort();
}
void DrawMechosBody(int x,int y,int speed,int level) {
	abort();
}
void DrawMechosParticle(int x,int y,int speed,int level,int n,Object* p) {
	abort();
}
int CheckStartJump(Object* p) {
	abort();
}
void Object::destroy_double_level() {
	abort();
}
void UnitBaseListType::ConnectTypeList(GeneralObject* p) {
	abort();
}
void UnitBaseListType::DisconnectTypeList(GeneralObject* p) {
	abort();
}
void UnitListType::ConnectTypeList(GeneralObject* p) {
	abort();
}
void UnitListType::DisconnectTypeList(GeneralObject* p) {
	abort();
}
void UnitListType::Init(void) {
	abort();
}
void UnitListType::Free(void) {
	abort();
}
void UnitListType::Quant(void) {
	abort();
}
void UnitListType::NetEvent(int type,int id) {
	abort();
}
void UnitListType::FreeUnit(GeneralObject* p) {
	abort();
}

// unused variables having it to avoid undefined reference errors
DebrisList DebrisD;
InputEventBuffer events_in(1);
OutputEventBuffer events_out(1);
XJOYSTATE XJoystickState;
uchar DestroyTerrainTable[TERRAIN_MAX];
uchar DestroyMoleTable[TERRAIN_MAX];
int JoystickStickSwitchButton = 0;
int JoystickMode = 0;
double camera_mi = 0;
double camera_miz = 0;
double camera_mit = 0;
double camera_mii = 0;
double camera_mis = 0;
double camera_drag = 0;
double camera_dragz = 0;
double camera_dragt = 0;
double camera_dragi = 0;
double camera_drags = 0;
double camera_vz_min = 0;
double camera_vt_min = 0;
double camera_vs_min = 0;
int camera_turn_impulse = 0;
int max_time_vibration = 0;
double A_vibration = 0;
double alpha_vibration = 0;
double oscillar_vibration = 0;
int SoundFlag = 0;
char WorldBorderEnable = 0;
int aciCurJumpImpulse = 0;
int aciMaxJumpImpulse = 0;
int aciKeyboardLocked = 0;
int aciAutoRun = 0;
int EngineNoise = 0;
