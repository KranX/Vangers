#include "global.h"

#include "actint/a_consts.h"
#include "actint/actint.h"

#include "network.h"
#include "units/uvsapi.h"
#include "uvs/univang.h"

#include "units/mechos.h"

#include "actint/text.h"
#include "localization/catalog.h"
#include "settings/settings.h"
#include "units/compas.h"
#include <array>
#include <iostream>
#include <stdexcept>

extern actIntDispatcher *aScrDisp;
extern aciFont **aScrFonts;
extern aciFont **aScrFonts32;
void aciAddTeleportMenuItem(int, int);
extern CompasObject CompasObj;
extern int uvsKronActive;
extern listElem *EscaveTail, *SpotTail;
void aciHandleSubmenuEvent(int, fncMenuItem *);
void aciRefreshTargetsMenu();
char *uvsGetNameByID(int, int &);
char *aciGetItemEscave(int);
namespace {
void check(bool v, const char *what) {
	if (!v)
		throw std::runtime_error(what);
}
struct Fonts {
	aciFont font;
	std::array<char, 256> offsets{};
	aciFont *faces[8]{};
	aciFont **before = aScrFonts;
	aciFont **before32 = aScrFonts32;
	Fonts() {
		font.Size = 256;
		font.SizeX = 10;
		font.SizeY = 18;
		font.LeftOffs = font.RightOffs = offsets.data();
		for (auto &f : faces)
			f = &font;
		aScrFonts = faces;
		aScrFonts32 = faces;
	}
	~Fonts() {
		aScrFonts = before;
		aScrFonts32 = before32;
		font.LeftOffs = font.RightOffs = nullptr;
	}
};
void check_reused_player_slot() {
	PlayersList list;
	auto *finished = new PlayerData(2);
	finished->name = "以前";
	finished->status = FINISHED_STATUS;
	list.append(finished);
	check(!list.find_active(2) && list.size() == 1, "Chat lookup created a player");
	auto *active = list.find(2);
	active->name = "新しい人";
	check(list.find_any(2) == finished, "Fixture must keep the old row first");
	check(list.find_active(2) == active && list.size() == 2, "Chat attributed sender to old row");
	list.delete_all();
}

void check_game_notifications(aciPromptData &prompt) {
	const auto before = my_server_data;
	const auto prev_active = ActD.Active;
	const auto prev_kron = uvsKronActive;
	VangerUnit active{};
	ActD.Active = &active;
	uvsKronActive = 1;

	PlayerData peer(2);
	peer.name = "日本語の参加者";
	peer.body.clear();
	const auto line_with_name = [&](int n) {
		return std::string_view(reinterpret_cast<const char *>(prompt.StrBuf[n])).find(peer.name) !=
			   std::string_view::npos;
	};
	my_server_data.GameType = MECHOSOMA;
	my_server_data.Mechosoma.ProductQuantity1 = 1;
	my_server_data.Mechosoma.ProductQuantity2 = 1;
	peer.body.MechosomaStat.ItemCount1 = 1;
	NetCheckRemovePlayer(&peer);
	check(prompt.NumStr == 2 && line_with_name(0), "Mechosoma counted only one delivery");
	peer.body.MechosomaStat.ItemCount2 = 1;
	NetCheckRemovePlayer(&peer);
	check(prompt.NumStr == 3 && line_with_name(1), "Mechosoma finish notice missing");

	my_server_data.GameType = PASSEMBLOSS;
	my_server_data.Passembloss.CheckpointsNumber = 1;
	peer.body.PassemblossStat.CheckpointLighting = 1;
	NetCheckRemovePlayer(&peer);
	check(prompt.NumStr == 3 && line_with_name(1), "Passembloss finish notice missing");
	peer.body.PassemblossStat.CheckpointLighting = 0;
	NetCheckRemovePlayer(&peer);
	check(prompt.NumStr == 2 && line_with_name(0), "Passembloss departure notice missing");

	my_server_data.GameType = VAN_WAR;
	NetCheckRemovePlayer(&peer);
	check(prompt.NumStr == 2 && line_with_name(0), "Van War departure notice missing");
	prompt.CurTimer = 50;
	uvsKronActive = 0;
	NetCheckRemovePlayer(&peer);
	check(prompt.CurTimer == 50, "Inactive world started a multiplayer notice");

	my_server_data = before;
	ActD.Active = prev_active;
	uvsKronActive = prev_kron;
}

void run() {
	using namespace vangers;
	check_reused_player_slot();
	settings::settings_manager().get_mutable().video.ttf_text = true;
	Fonts fonts;
	actIntDispatcher dispatcher;
	aScrDisp = &dispatcher;
	check_game_notifications(*dispatcher.curPrompt);
	CompasObj.Init();
	CompasObj.TargetData = nullptr;
	auto *menu = new fncMenu;
	menu->type = FMENU_TARGETS_MENU;
	menu->SizeX = 400;
	menu->SizeY = 260;
	dispatcher.add_menu(menu);
	auto *off = new fncMenuItem;
	off->init_name(localization::text("actint.aciSTR_OFF"));
	menu->add_item(off);
	menu->curItem = menu->firstItem = off;
	char incubator[] = "Incubator", podish[] = "Podish";
	AddTarget2Compas(CMP_TARGET_SPOT, incubator, "compass.CmpIncubator");
	auto *it = CompasObj.TargetData;
	AddTarget2Compas(CMP_TARGET_ESCAVE, podish, "compass.CmpPodish");
	auto *pt = CompasObj.TargetData;
	auto *a = menu->get_compass_item("compass.CmpIncubator"),
		 *b = menu->get_compass_item("compass.CmpPodish");
	check(a && b && a != b, "Translated captions merged different targets");
	check(
		std::string(a->name) == localization::text("compass.CmpIncubator"), "Caption decoded twice"
	);
	check(
		std::string(pt->Name) == "Podish" && std::string(it->Name) == "Incubator", "Lookup changed"
	);
	const int count = menu->items->Size;
	AddTarget2Compas(CMP_TARGET_SPOT, incubator, "compass.CmpIncubator");
	check(menu->items->Size == count, "Repeated semantic target duplicated");
	aciHandleSubmenuEvent(FMENU_TARGETS_MENU, a);
	check(CompasObj.CurrentTarget == it, "Selection used display text");
	aciRefreshTargetsMenu();
	check(menu->curItem == a, "Refresh used display text");
	aciHandleSubmenuEvent(FMENU_TARGETS_MENU, b);
	check(CompasObj.CurrentTarget == pt, "Second target not selected");
	fncMenuItem clone;
	b->clone(&clone);
	check(clone.compass_id == b->compass_id, "Clone lost selection identity");
	CompasObj.AddTarget(CMP_OBJECT_ITEM, UnitOrderType(), nullptr, "test.off-caption", off->name);
	auto *same_off = menu->get_compass_item("test.off-caption");
	aciHandleSubmenuEvent(FMENU_TARGETS_MENU, same_off);
	check(
		CompasObj.CurrentTarget && CompasObj.CurrentTarget->key == "test.off-caption",
		"OFF caption disabled a target"
	);
	aciHandleSubmenuEvent(FMENU_TARGETS_MENU, off);
	check(!GetCompasTarget() && menu->get_compass_item(nullptr) == off, "OFF identity changed");
	SelectCompasTarget("compass.CmpPodish");
	aciRefreshTargetsMenu();
	menu->curItem = a; // Removing the hovered row must not clear another destination.
	CompasObj.DeleteTarget(it);
	check(
		CompasObj.CurrentTarget == pt && menu->get_compass_item("compass.CmpIncubator") == nullptr,
		"Deleting another target changed selection"
	);
	menu->curItem = same_off; // Hover is not the selected destination.
	CompasObj.DeleteTarget(pt);
	check(!GetCompasTarget() && menu->curItem == off, "Deleting selected target left stale state");
	std::string long_caption;
	for (int i = 0; i < 100; ++i)
		long_caption += "名";
	CompasObj.AddTarget(
		CMP_OBJECT_ITEM, UnitOrderType(), nullptr, "test.long", long_caption.c_str()
	);
	auto *long_item = menu->get_compass_item("test.long");
	check(std::string(long_item->name) == long_caption, "Long UTF-8 model was truncated");
	aciHandleSubmenuEvent(FMENU_TARGETS_MENU, long_item);
	check(std::string(GetCompasTarget()) == "test.long", "Long caption changed target identity");
	CompasObj.Close();
	uvsEscave escave;
	escave.name = actint::copy_text("Podish");
	escave.TabuTaskID = 50;
	escave.TabuTaskCount = 10;
	char marker{};
	auto *enter = reinterpret_cast<EnterCenter *>(&marker);
	escave.unitPtr.EnterT = enter;
	auto *old_escaves = EscaveTail;
	auto *old_spots = SpotTail;
	EscaveTail = &escave;
	SpotTail = nullptr;
	auto *loc = new aciLocationInfo;
	loc->ID = 17;
	loc->nameID = actint::copy_text("Podish");
	loc->nameID2 = actint::copy_text(localization::text("hud.ESCAVE_NAME0"));
	dispatcher.locationList->connect(loc);
	int begin = -1;
	auto *lookup = uvsGetNameByID(55, begin);
	check(lookup == escave.name && begin == 50, "UVS quest lookup/ID changed");
	check(
		aciGetLocationNameUtf8(lookup) == localization::text("hud.ESCAVE_NAME0"),
		"UVS label not localized"
	);
	check(std::string(lookup) == "Podish", "Presentation changed UVS identity");
	check(
		std::string(aciGetItemEscave(55)) == localization::text("hud.ESCAVE_NAME0"),
		"Item escave caption lost Unicode"
	);
	auto *teleport = new fncMenu;
	teleport->type = FMENU_TELEPORT_MENU;
	teleport->SizeX = 400;
	teleport->SizeY = 260;
	dispatcher.add_menu(teleport);
	auto *old_location = aci_curLocationName;
	aci_curLocationName = lookup;
	aciAddTeleportMenuItem(-1, 7);
	aci_curLocationName = old_location;
	check(
		std::string(teleport->get_fnc_item(7)->name) == localization::text("hud.ESCAVE_NAME0"),
		"Teleport name lost Unicode"
	);

	const auto message = aciFormatTaskCompleteUtf8(55, 123);
	const auto expected_message = localization::format_task_complete(
		localization::text("uvs.task_complete"), localization::text("hud.ESCAVE_NAME0"), 123
	);
	check(
		message == expected_message && settings::is_valid_utf8(message),
		"UVS task message lost translated name/reward"
	);

	AddTarget2Compas(CMP_TARGET_ESCAVE, lookup, "compass.CmpPodish");
	CompasObj.Open();
	check(CompasObj.TargetData->Data.EnterT == enter, "Translation changed destination");
	CompasObj.Close();
	EscaveTail = old_escaves;
	SpotTail = old_spots;
	const auto legacy = settings::utf8_to_cp866("Ёж");
	check(aciGetLocationNameUtf8(legacy.c_str()) == "Ёж", "Legacy/mod caption lost fallback");
	check(aciGetLocationNameUtf8(nullptr).empty(), "Absent name not handled");
	aScrDisp = nullptr;
	std::cout << "Compass identity/UTF-8/select/delete/clone, UVS lookup/destination OK\n";
}
} // namespace
int main(int argc, char **argv) {
	try {
		check(argc == 2, "Expected locale");
		vangers::localization::request_locale(argv[1]);
		if (std::string(argv[1]) == "qa-ja") {
			const auto &c = vangers::localization::catalog();
			check(
				c.text("compass.CmpIncubator") == "日本語" &&
					c.text("compass.CmpPodish") == "日本語" &&
					c.text("actint.aciSTR_OFF") == "日本語",
				"Synthetic collision fixture missing"
			);
		}
		run();
		return 0;
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return 1;
	}
}

bool VangersTest_ShowSimpleMessageBox(
	SDL_MessageBoxFlags,
	const char *,
	const char *,
	SDL_Window *
) {
	return true;
}
