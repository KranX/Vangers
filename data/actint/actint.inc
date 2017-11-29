/* actInt script include file */

#include "iscreen/scripts/vkeys.inc"
#include "actint/items.inc"

mode_key	$VK_F1

/* ------------------------------ Item IDs... ------------------------------- */
	// Weapons...
#define ACI_MACHOTINE_GUN_LIGHT 	1
#define ACI_MACHOTINE_GUN_HEAVY 	2

#define ACI_SPEETLE_SYSTEM_LIGHT	3
#define ACI_SPEETLE_SYSTEM_HEAVY	4

#define ACI_GHORB_GEAR_LIGHT		5
#define ACI_GHORB_GEAR_HEAVY		6

#define ACI_BEEBBANOZA_BLOCKADE 	7

#define ACI_CRUSTEST_CANNON		8

#define ACI_TERMINATOR			9
#define ACI_TERMINATOR2 		10

	// Ammo...
#define ACI_SPEETLE_SYSTEM_AMMO0	11
#define ACI_SPEETLE_SYSTEM_AMMO1	12

#define ACI_CRUSTEST_CANNON_AMMO	13

	// Devices...
#define ACI_COPTE_RIG			14
#define ACI_EMPTY_COPTE_RIG		15

#define ACI_CUTTE_RIG			16
#define ACI_EMPTY_CUTTE_RIG		17

#define ACI_CROT_RIG			18
#define ACI_EMPTY_CROT_RIG		19

#define ACI_LHARK_DEVICE		20
#define ACI_EMPTY_LHARK_DEVICE		21

#define ACI_RADAR_DEVICE		22

	// Items...
#define ACI_CIRTAINER			23
#define ACI_EMPTY_CIRTAINER		24

#define ACI_CONLARVER			25
#define ACI_EMPTY_CONLARVER		26

	// Wares...
#define ACI_NYMBOS			27
#define ACI_PHLEGMA			28
#define ACI_HEROIN			29
#define ACI_SHRUB			30
#define ACI_POPONKA			31
#define ACI_TOXICK			32

	// Cultic units...
#define ACI_ELEECH			33
#define ACI_KERNOBOO			34
#define ACI_PIPETKA			35
#define ACI_WEEZYK			36

	// Artefacts...
#define ACI_PROTRACTOR			37
#define ACI_MECHANIC_MESSIAH		38
#define ACI_FUNCTION83			39
#define ACI_SPUMMY			40
#define ACI_BOOT_SECTOR 		41
#define ACI_PEELOT			42

#define ACI_LEEPURINGA			43
#define ACI_DEAD_ELEECH 		44

	// Tokens...
#define ACI_RUBOX			45

	// Mehos parts...
#define ACI_SANDOLL_PART1		46
#define ACI_SANDOLL_PART2		47

#define ACI_LAST_MOGGY_PART1		48
#define ACI_LAST_MOGGY_PART2		49

#define ACI_QUEEN_FROG_PART1		50
#define ACI_QUEEN_FROG_PART2		51

#define ACI_LAWN_MOWER_PART1		52
#define ACI_LAWN_MOWER_PART2		53

#define ACI_WORMASTER_PART1		54
#define ACI_WORMASTER_PART2		55

#define ACI_TANKACID			56

	// Additional items...

	// Weapons...
#define ACI_AMPUTATOR			57
#define ACI_DEGRADATOR			58
#define ACI_MECHOSCOPE			59

	// Ammo...
#define ACI_GLUEK			60

	// Warez...
#define ACI_TABUTASK			61
#define ACI_ELEEPOD			62
#define ACI_BEEBOORAT			63
#define ACI_ZEEX			64

	// Personal Articles...
#define ACI_PALOCHKA			65
#define ACI_PIPKA			66
#define ACI_NOBOOL			67
#define ACI_BOORAWCHICK 		68
#define ACI_PEREPONKA			69
#define ACI_ZEEFICK			70

#define ACI_TABUTASK_FAILED		71
#define ACI_TABUTASK_SUCCESSFUL 	72

#define ACI_EMPTY_AMPUTATOR		73
#define ACI_EMPTY_DEGRADATOR		74
#define ACI_EMPTY_MECHOSCOPE		75

#define ACI_ROTTEN_KERNOBOO		76
#define ACI_ROTTEN_PIPETKA		77
#define ACI_ROTTEN_WEEZYK		78

/* ---------------------------- Item IDs & Names ---------------------------- */

#define ITM01_ID	$ACI_MACHOTINE_GUN_LIGHT
#define ITM02_ID	$ACI_MACHOTINE_GUN_HEAVY
#define ITM03_ID	$ACI_SPEETLE_SYSTEM_LIGHT
#define ITM04_ID	$ACI_SPEETLE_SYSTEM_HEAVY
#define ITM05_ID	$ACI_GHORB_GEAR_LIGHT
#define ITM06_ID	$ACI_GHORB_GEAR_HEAVY
#define ITM07_ID	$ACI_BEEBBANOZA_BLOCKADE
#define ITM08_ID	$ACI_CRUSTEST_CANNON
#define ITM09_ID	$ACI_TERMINATOR
#define ITM10_ID	$ACI_TERMINATOR2
#define ITM11_ID	$ACI_SPEETLE_SYSTEM_AMMO0
#define ITM12_ID	$ACI_SPEETLE_SYSTEM_AMMO1
#define ITM13_ID	$ACI_CRUSTEST_CANNON_AMMO
#define ITM14_ID	$ACI_COPTE_RIG
#define ITM15_ID	$ACI_EMPTY_COPTE_RIG
#define ITM16_ID	$ACI_CUTTE_RIG
#define ITM17_ID	$ACI_EMPTY_CUTTE_RIG
#define ITM18_ID	$ACI_CROT_RIG
#define ITM19_ID	$ACI_EMPTY_CROT_RIG
#define ITM20_ID	$ACI_LHARK_DEVICE
#define ITM21_ID	$ACI_EMPTY_LHARK_DEVICE
#define ITM22_ID	$ACI_RADAR_DEVICE
#define ITM23_ID	$ACI_CIRTAINER
#define ITM24_ID	$ACI_EMPTY_CIRTAINER
#define ITM25_ID	$ACI_CONLARVER
#define ITM26_ID	$ACI_EMPTY_CONLARVER
#define ITM27_ID	$ACI_NYMBOS
#define ITM28_ID	$ACI_PHLEGMA
#define ITM29_ID	$ACI_HEROIN
#define ITM30_ID	$ACI_SHRUB
#define ITM31_ID	$ACI_POPONKA
#define ITM32_ID	$ACI_TOXICK
#define ITM33_ID	$ACI_ELEECH
#define ITM34_ID	$ACI_KERNOBOO
#define ITM35_ID	$ACI_PIPETKA
#define ITM36_ID	$ACI_WEEZYK
#define ITM37_ID	$ACI_PROTRACTOR
#define ITM38_ID	$ACI_MECHANIC_MESSIAH
#define ITM39_ID	$ACI_FUNCTION83
#define ITM40_ID	$ACI_SPUMMY
#define ITM41_ID	$ACI_LEEPURINGA
#define ITM42_ID	$ACI_DEAD_ELEECH
#define ITM43_ID	$ACI_RUBOX
#define ITM44_ID	$ACI_SANDOLL_PART1
#define ITM45_ID	$ACI_SANDOLL_PART2
#define ITM46_ID	$ACI_LAST_MOGGY_PART1
#define ITM47_ID	$ACI_LAST_MOGGY_PART2
#define ITM48_ID	$ACI_QUEEN_FROG_PART1
#define ITM49_ID	$ACI_QUEEN_FROG_PART2
#define ITM50_ID	$ACI_LAWN_MOWER_PART1
#define ITM51_ID	$ACI_LAWN_MOWER_PART2
#define ITM52_ID	$ACI_WORMASTER_PART1
#define ITM53_ID	$ACI_WORMASTER_PART2
#define ITM54_ID	$ACI_TANKACID
#define ITM55_ID	$ACI_PEELOT
#define ITM56_ID	$ACI_BOOT_SECTOR

#define ITM57_ID	$ACI_MECHOSCOPE
#define ITM58_ID	$ACI_GLUEK
#define ITM59_ID	$ACI_DEGRADATOR
#define ITM60_ID	$ACI_TABUTASK
#define ITM61_ID	$ACI_AMPUTATOR
#define ITM62_ID	$ACI_ELEEPOD
#define ITM63_ID	$ACI_BEEBOORAT
#define ITM64_ID	$ACI_ZEEX
#define ITM65_ID	$ACI_PALOCHKA
#define ITM66_ID	$ACI_PIPKA
#define ITM67_ID	$ACI_NOBOOL
#define ITM68_ID	$ACI_BOORAWCHICK
#define ITM69_ID	$ACI_PEREPONKA
#define ITM70_ID	$ACI_ZEEFICK

#define ITM73_ID	$ACI_TABUTASK_FAILED
#define ITM74_ID	$ACI_TABUTASK_SUCCESSFUL

#define ITM71_ID	$ACI_ROTTEN_WEEZYK
#define ITM72_ID	$ACI_ROTTEN_KERNOBOO

#define ITM75_ID	$ACI_EMPTY_AMPUTATOR
#define ITM76_ID	$ACI_EMPTY_DEGRADATOR
#define ITM77_ID	$ACI_ROTTEN_PIPETKA
#define ITM78_ID	$ACI_EMPTY_MECHOSCOPE

/* -------------------------------------------------------------------------- */

// Modes...
#define INV_MODE		0
#define INF_MODE		1

//Slot types
#define SLOT_NONE		-1
#define SLOT_WPN		1
#define SLOT_TWPN		2
#define SLOT_DVC		3

#define ITEM_TELEPORT_MENU    fncMenu { 					\
					menu_type	$FMENU_TELEPORT_MENU	\
					submenu 				\
										\
					visible_items	5			\
										\
					x	484				\
					y	48				\
										\
					size_x	152				\
					size_y	193				\
										\
					range_font				\
										\
					no_deactivate				\
					active_time	200			\
										\
					cur_function	$FMENU_OFF		\
										\
					fncMenuItem {				\
						string		$STR_BOOTSECTOR \
						function	$FMENU_TELEPORT \
										\
						up_menu_item			\
						submenu_id	$FMENU_TELEPORT_MENU	  \
										\
						font		0		\
						space		1		\
					}					\
					fncMenuItem {				\
						string		$STR_OFF	\
						function	$FMENU_OFF	\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
					}					\
				}

#define ITEM_PROTRACTOR_MENU	fncMenu {					\
					menu_type	$FMENU_PROTRACTOR_MENU	\
					submenu 				\
										\
					visible_items	5			\
										\
					x	484				\
					y	48				\
										\
					size_x	152				\
					size_y	193				\
										\
					range_font				\
										\
					no_deactivate				\
					active_time	200			\
										\
					cur_function	2001			\
										\
					fncMenuItem {				\
						string		$STR_PROTRACTOR \
						function	8384		\
										\
						up_menu_item			\
						submenu_id	$FMENU_PROTRACTOR_MENU	    \
										\
						font		0		\
						space		1		\
					}					\
					fncMenuItem {				\
						string		$STR_Spobsion	\
						function	2001		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_PROTRACTOR_EVENT 1	\
					}					\
					fncMenuItem {				\
						string		$STR_Resizing	\
						function	2002		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_PROTRACTOR_EVENT 2	\
					}					\
					fncMenuItem {				\
						string		$STR_Barrier	\
						function	2003		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_PROTRACTOR_EVENT 3	\
					}					\
					fncMenuItem {				\
						string		$STR_Beebs_Festival	\
						function	2004		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_PROTRACTOR_EVENT 4	\
					}					\
					fncMenuItem {				\
						string		$STR_Circus	\
						function	2005		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_PROTRACTOR_EVENT 5	\
					}					\
					fncMenuItem {				\
						string		$STR_Ostrich	\
						function	2006		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_PROTRACTOR_EVENT 6	\
					}					\
					fncMenuItem {				\
						string		$STR_Function83 	\
						function	2007		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_PROTRACTOR_EVENT 7	\
					}					\
					fncMenuItem {				\
						string		$STR_HyperBeacon	\
						function	2008		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_PROTRACTOR_EVENT 8	\
					}					\
				}

#define ITEM_MECH_MESSIAH_MENU	  fncMenu {					  \
					menu_type	$FMENU_MECH_MESSIAH_MENU  \
					submenu 				\
										\
					visible_items	5			\
										\
					x	484				\
					y	48				\
										\
					size_x	152				\
					size_y	193				\
										\
					no_deactivate				\
					active_time	200			\
										\
					range_font				\
					cur_function	2001			\
										\
					fncMenuItem {				\
						string		$STR_MECH_MESSIAH	\
						function	8385		\
										\
						up_menu_item			\
						submenu_id	$FMENU_MECH_MESSIAH_MENU      \
										\
						font		0		\
						space		1		\
					}					\
					fncMenuItem {				\
						string		$STR_Cloak	\
						function	2001		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_MECH_MESSIAH_EVENT 1	\
					}					\
					fncMenuItem {				\
						string		$STR_Luck	\
						function	2002		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_MECH_MESSIAH_EVENT 2	\
					}					\
					fncMenuItem {				\
						string		$STR_Beebsospy	\
						function	2003		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_MECH_MESSIAH_EVENT 3	\
					}					\
					fncMenuItem {				\
						string		$STR_Firework	\
						function	2004		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_MECH_MESSIAH_EVENT 4	\
					}					\
					fncMenuItem {				\
						string		$STR_Shake_Up	\
						function	2005		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_MECH_MESSIAH_EVENT 5	\
					}					\
					fncMenuItem {				\
						string		$STR_Blackada	\
						function	2006		\
										\
						font		$FMENU_TARGETS_FONT	\
						space		$FMENU_TARGETS_SPACE	\
										\
						event $EV_MECH_MESSIAH_EVENT 6	\
					}					\
				}

#define ITEM_WORLDS_MENU	fncMenu {					\
					menu_type	$FMENU_WORLDS_MENU	\
					submenu 				\
										\
					visible_items	5			\
										\
					x	484				\
					y	48				\
										\
					size_x	152				\
					size_y	193				\
										\
					range_font				\
					no_deactivate				\
					active_time	200			\
										\
					cur_function	1001			\
										\
					fncMenuItem {				\
						string		$STR_PASS_TO	\
						function	8383		\
										\
						up_menu_item			\
						submenu_id	$FMENU_WORLDS_MENU	\
										\
						font		0		\
						space		1		\
					}					\
					fncMenuItem {				\
						string		$STR_Fostral	\
						function	{ 1000 + $FOSTRAL_ID }	\
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $FOSTRAL_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_Glorx	\
						function	{ 1000 + $GLORX_ID }  \
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $GLORX_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_Necross	\
						function	{ 1000 + $NECROSS_ID }	\
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $NECROSS_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_Xplo	\
						function	{ 1000 + $XPLO_ID }  \
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $XPLO_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_Khox	\
						function	{ 1000 + $K_ID }  \
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $K_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_Boozeena	\
						function	{ 1000 + $B_ID }  \
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $B_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_Weexow	\
						function	{ 1000 + $W_ID }  \
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $W_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_hMok	\
						function	{ 1000 + $H_ID }  \
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $H_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_Threall	\
						function	{ 1000 + $T_ID }  \
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $T_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_Ark_a_Znoy \
						function	{ 1000 + $A_ID }  \
										\
						font		0		\
						space		1		\
										\
						event $EV_TELEPORT $A_ID	\
					}					\
					fncMenuItem {				\
						string		$STR_OFF	\
						function	$FMENU_OFF	\
										\
						font		0		\
						space		1		\
					}					\
				}

#define MENU_BODY	fncMenuItem {					\
				string		$STR_OFF		\
				function	$FMENU_OFF		\
									\
				font		0			\
				space		1			\
			}						\
			fncMenuItem {					\
				string		$STR_MAP		\
				function	$FMENU_MAP		\
									\
				font		0			\
				space		1			\
			}						\
			fncMenuItem {					\
				string		$STR_WMAP		\
				function	$FMENU_WMAP		\
									\
				font		0			\
				space		1			\
			}						\
			fncMenuItem {					\
				string		$STR_TARGETS		\
				function	$FMENU_TARGETS		\
				submenu_id	$FMENU_TARGETS_MENU	\
									\
				font		0			\
				space		1			\
			}						\
			fncMenuItem {					\
				string		$STR_CAMERAS		\
				function	$FMENU_CAMERAS		\
				bsubmenu_id	$BMENU_CAMERAS_MENU	\
									\
				font		0			\
				space		1			\
			}						\
			fncMenuItem {					\
				string		$STR_PARAMS		\
				function	$FMENU_PARAMETERS	\
									\
				font		0			\
				space		1			\
			}						\
			main_menu

// Buttons...
#define B_MODE		83
#define B_MENU1 	 1
#define B_MENU2 	 2
#define B_INT1		 3
#define B_INT2		 4
#define B_INF0		 5

// Menus...
#define MENU1		1
#define MENU2		2

// Screens...
#define IBS1		1
#define IBS2		2
#define IBS3		3

// Backgrounds...
#define BG1		1
#define BG2		2
#define BG3		3

// aIndData::CornerNum values...
#define IND_NONE	0
#define IND_UP_LEFT	1
#define IND_UP_RIGHT	2
#define IND_DN_LEFT	3
#define IND_DN_RIGHT	4

// Indicator IDs...
#define IND_N0		0
#define IND_N1		1
#define IND_NRG 	2
#define IND_LOAD	3
#define IND_JUMP	4
#define IND_WPN 	5
#define IND_DVC 	6
#define IND_SPIRAL	7

// Indicator types...
#define IND_CLOCK	1
#define IND_BARREL	2

#define CONSTR1 	19
#define CONSTR2 	23
#define CONSTR3 	27
#define CONSTR4 	31
#define CONSTR5 	35

// Mechos IDs...
#define MECH00_ID	0	0
#define MECH01_ID	1	1
#define MECH02_ID	2	2
#define MECH03_ID	3	3
#define MECH04_ID	4	4
#define MECH05_ID	5	5
#define MECH06_ID	6	6
#define MECH07_ID	7	7
#define MECH08_ID	8	{ $CONSTR1 + 3 }
#define MECH09_ID	9	8
#define MECH10_ID	10	9
#define MECH11_ID	11	10
#define MECH12_ID	12	11
#define MECH13_ID	13	12
#define MECH14_ID	14	13
#define MECH15_ID	15	{ $CONSTR2 + 3 }
#define MECH16_ID	16	{ $CONSTR3 + 3 }
#define MECH17_ID	17	{ $CONSTR4 + 3 }
#define MECH18_ID	18	14
#define MECH19_ID	18	15
#define MECH20_ID	18	16
#define MECH21_ID	18	17
#define MECH22_ID	18	18
#define MECH23_ID	8	$CONSTR1
#define MECH24_ID	8	{ $CONSTR1 + 1 }
#define MECH25_ID	8	{ $CONSTR1 + 2 }
#define MECH26_ID	15	$CONSTR2
#define MECH27_ID	15	{ $CONSTR2 + 1 }
#define MECH28_ID	15	{ $CONSTR2 + 2 }
#define MECH29_ID	16	$CONSTR3
#define MECH30_ID	16	{ $CONSTR3 + 1 }
#define MECH31_ID	16	{ $CONSTR3 + 2 }
#define MECH32_ID	17	$CONSTR4
#define MECH33_ID	17	{ $CONSTR4 + 1 }
#define MECH34_ID	17	{ $CONSTR4 + 2 }
#define MECH35_ID	35	{ $CONSTR5 + 3 }
#define MECH36_ID	35	$CONSTR5
#define MECH37_ID	35	{ $CONSTR5 + 1 }
#define MECH38_ID	35	{ $CONSTR5 + 2 }

#define MATRIX_LOWRES	x	360	\
			y	60

#define MATRIX_HIRES	x	456	\
			y	76

#define iMATRIX_HIRES	x	476	\
			y	70

#define MATRIX00_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 1 0 0 0 0 0 1 	\
					 1 1 0 0 1 0 1 1	\
					0 1 0 0 0 0 0 1 	\
					 0 0 1 1 1 0 0 0	\
					0 0 0 1 1 0 0 0 	\
					 0 0 1 1 1 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 1 0 0 0 0 0 1 	\
					 1 1 0 0 2 0 1 1	\
					0 1 0 0 0 0 0 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 1 0 0 0 0 0 2 	\
					 1 1 0 0 4 0 2 2	\
					0 1 0 0 0 0 0 2 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH00_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech00.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech002.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech003.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech00.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech00r.avi"

#define MATRIX06_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 1 1 0 0 1 0	\
					0 0 1 1 1 0 1 1 	\
					 1 0 1 1 0 0 1 0	\
					0 0 1 1 1 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 1 1 0 0 	\
					 1 0 0 0 1 0 0 0	\
					1 1 1 0 0 0 0 1 	\
					 0 1 1 1 1 1 1 1	\
					0 0 1 1 1 1 0 0 	\
					 0 0 1 1 1 1 0 0	\
					1 1 1 1 1 1 1 1 	\
					 1 1 0 0 0 0 1 1	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 1 1 0 0 1 0	\
					0 0 1 1 1 0 1 1 	\
					 2 0 1 1 0 0 1 0	\
					0 0 1 1 1 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 3 3 0 0 	\
					 0 0 0 0 3 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 3 3 0 0 2 0	\
					0 0 3 3 3 0 2 2 	\
					 5 0 3 3 0 0 2 0	\
					0 0 3 3 3 0 2 2 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 4 4 0 0 	\
					 0 0 0 0 4 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH06_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech06.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech062.avi"       \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech063.avi"       \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech06.avi"         \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech06r.avi"

#define MATRIX01_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 1 1 0 0 1 0	\
					1 1 0 1 0 0 1 1 	\
					 1 0 0 0 1 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 1 1 1 1 1 0 	\
					 1 1 1 1 1 1 1 1	\
					0 0 1 1 1 1 1 0 	\
					 1 1 1 1 1 1 1 1	\
					0 0 1 1 1 1 1 0 	\
					 0 1 1 1 1 1 1 0	\
					0 0 1 1 0 1 1 0 	\
					 0 0 1 0 0 1 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 3 3 0 0 1 0	\
					1 1 0 3 0 0 1 1 	\
					 1 0 0 0 2 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 4 4 0 0 2 0	\
					1 1 0 4 0 0 2 2 	\
					 1 0 0 0 5 0 2 0	\
					1 1 0 0 0 0 2 2 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH01_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech01.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech012.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech013.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech01.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech01r.avi"


#define MATRIX04_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 1 0 1 1 0 1 1 	\
					 1 1 0 1 0 1 1 1	\
					0 1 0 0 0 0 1 1 	\
					 1 1 0 1 0 1 1 1	\
					0 0 0 0 0 0 0 0 	\
					 1 0 1 1 1 1 1 1	\
					1 1 0 1 1 1 1 1 	\
					 1 0 1 1 1 1 1 1	\
					1 1 0 1 1 1 1 1 	\
					 0 0 1 1 1 1 1 1	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
				}				\
								\
				slot_types {			\
					0 1 0 3 3 0 1 1 	\
					 1 1 0 3 0 1 1 1	\
					0 1 0 0 0 0 1 1 	\
					 1 1 0 2 0 1 1 1	\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 0 0	\
					1 1 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 0 0	\
					1 1 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 1 0 4 4 0 3 3 	\
					 1 1 0 4 0 3 3 3	\
					0 1 0 0 0 0 3 3 	\
					 1 1 0 5 0 3 3 3	\
					0 0 0 0 0 0 0 0 	\
					 2 0 0 0 0 0 0 0	\
					2 2 0 0 0 0 0 0 	\
					 2 0 0 0 0 0 0 0	\
					2 2 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH04_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech04.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech042.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech043.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech04.avi"         \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech04r.avi"

#define MATRIX02_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 1 0 1 1 0 0 1 	\
					 1 1 0 1 0 0 1 1	\
					0 1 0 0 0 0 0 1 	\
					 1 1 0 0 1 0 1 1	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 1 0 1 0 0 	\
					 0 0 1 1 1 1 0 0	\
					0 0 0 1 1 1 0 0 	\
					 1 1 1 1 1 1 1 1	\
					0 1 1 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 1 0 3 3 0 0 1 	\
					 1 1 0 3 0 0 1 1	\
					0 1 0 0 0 0 0 1 	\
					 1 1 0 0 2 0 1 1	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 1 0 4 4 0 0 2 	\
					 1 1 0 4 0 0 2 2	\
					0 1 0 0 0 0 0 2 	\
					 1 1 0 0 5 0 2 2	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH02_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech02.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech022.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech023.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech02.avi"         \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech02r.avi"

#define MATRIX07_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 1 1 0 1 1 0	\
					1 1 0 1 0 1 1 1 	\
					 1 0 0 0 0 1 1 0	\
					1 1 0 1 0 1 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 1 1 0 0 0 1 1 1	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
					1 1 1 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 3 3 0 1 1 0	\
					1 1 0 3 0 1 1 1 	\
					 1 0 0 0 0 1 1 0	\
					1 1 0 2 0 1 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 4 4 0 3 3 0	\
					1 1 0 4 0 3 3 3 	\
					 1 0 0 0 0 3 3 0	\
					1 1 0 5 0 3 3 3 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH07_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech07.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech072.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech073.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech07.avi"         \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech07r.avi"

#define MATRIX11_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 1 0	\
					0 1 0 0 0 0 1 1 	\
					 1 1 0 0 0 0 1 0	\
					0 1 0 1 1 0 1 1 	\
					 0 0 0 1 0 0 0 0	\
					0 0 0 0 0 1 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 1 1 1 1 1 0 	\
					 0 0 0 1 1 0 0 0	\
					0 1 1 1 1 1 1 1 	\
					 0 0 1 1 1 1 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 1 0	\
					0 1 0 0 0 0 1 1 	\
					 1 1 0 0 0 0 1 0	\
					0 1 0 3 3 0 1 1 	\
					 0 0 0 3 0 0 0 0	\
					0 0 0 0 0 2 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 2 0	\
					0 1 0 0 0 0 2 2 	\
					 1 1 0 0 0 0 2 0	\
					0 1 0 4 4 0 2 2 	\
					 0 0 0 4 0 0 0 0	\
					0 0 0 0 0 5 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH11_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech11.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech112.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech113.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech11.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech11r.avi"

#define MATRIX03_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 1 0 0 0 0 0 1 	\
					 1 1 0 1 1 0 1 1	\
					0 1 0 1 1 1 0 1 	\
					 0 0 0 1 1 0 1 1	\
					0 1 0 0 0 0 0 0 	\
					 0 0 0 1 1 0 0 0	\
					0 0 1 1 1 1 1 0 	\
					 0 1 1 1 1 1 1 0	\
					0 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
					0 1 1 1 1 1 1 1 	\
					 0 1 1 1 1 1 1 0	\
					0 0 1 1 1 1 1 0 	\
					 0 0 1 1 1 1 0 0	\
				}				\
								\
				slot_types {			\
					0 1 0 0 0 0 0 1 	\
					 1 1 0 1 1 0 1 1	\
					0 1 0 1 1 1 0 1 	\
					 0 0 0 1 1 0 1 1	\
					0 2 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 1 0 0 0 0 0 2 	\
					 1 1 0 3 3 0 2 2	\
					0 1 0 3 3 3 0 2 	\
					 0 0 0 3 3 0 2 2	\
					0 4 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH03_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech03.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech032.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech033.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech03.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech03r.avi"

#define MATRIX05_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 0 1 0 0 0 1 1 0	\
					0 1 1 0 0 1 1 1 	\
					 0 1 0 1 0 1 1 0	\
					0 1 1 0 0 0 0 0 	\
					 0 0 0 1 1 0 0 0	\
					0 1 1 0 1 0 1 1 	\
					 0 1 1 0 0 1 1 0	\
					0 0 1 1 1 1 0 0 	\
					 0 0 1 1 1 0 0 0	\
					0 1 1 1 1 1 1 0 	\
					 1 1 0 0 0 1 1 1	\
					0 0 0 0 0 0 0 1 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 0 1 0 0 0 1 1 0	\
					0 1 1 0 0 1 1 1 	\
					 0 1 0 2 0 1 1 0	\
					0 1 1 0 0 0 0 0 	\
					 0 0 0 3 3 0 0 0	\
					0 0 0 0 3 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 0 1 0 0 0 2 2 0	\
					0 1 1 0 0 2 2 2 	\
					 0 1 0 4 0 2 2 0	\
					0 1 1 0 0 0 0 0 	\
					 0 0 0 5 5 0 0 0	\
					0 0 0 0 5 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH05_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech05.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech052.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech053.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech05.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech05r.avi"

#define MATRIX08_MATRIX 	msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 1 1 0 0 1 0	\
					1 1 0 1 0 0 1 1 	\
					 1 0 0 0 1 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 1 0 0 1 0 0 	\
					 0 1 1 1 1 1 0 0	\
					0 0 1 1 1 1 0 0 	\
					 1 1 1 1 1 1 1 0	\
					0 0 0 1 1 0 0 0 	\
					 1 1 1 0 1 1 1 0	\
					1 1 1 0 0 1 1 1 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 3 3 0 0 1 0	\
					1 1 0 3 0 0 1 1 	\
					 1 0 0 0 2 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 4 4 0 0 2 0	\
					1 1 0 4 0 0 2 2 	\
					 1 0 0 0 5 0 2 0	\
					1 1 0 0 0 0 2 2 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}

#define MATRIX08_BODY		id	$MECH08_NAME		\
				$MATRIX08_MATRIX		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech08.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech082.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech083.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech08.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech08r.avi"

#define MATRIX23_BODY		id	$MECH23_NAME		\
				$MATRIX08_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech23.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech23.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech23.avi"         \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech08.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech08r.avi"

#define MATRIX24_BODY		id	$MECH24_NAME		\
				$MATRIX08_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech24.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech24.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech24.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech08.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech08r.avi"

#define MATRIX25_BODY		id	$MECH25_NAME		\
				$MATRIX08_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech25.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech25.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech25.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech08.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech08r.avi"

#define MATRIX09_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 1 1 0 0 1 0	\
					1 1 0 1 0 0 1 1 	\
					 1 0 0 0 1 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					1 1 1 0 0 1 1 1 	\
					 0 0 1 1 1 0 0 0	\
					0 0 0 1 1 0 0 0 	\
					 1 1 1 1 1 1 1 1	\
					0 0 1 1 1 1 0 0 	\
					 0 0 1 1 1 0 0 0	\
					1 1 1 0 0 1 1 1 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 3 3 0 0 1 0	\
					1 1 0 3 0 0 1 1 	\
					 1 0 0 0 2 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 4 4 0 0 2 0	\
					1 1 0 4 0 0 2 2 	\
					 1 0 0 0 5 0 2 0	\
					1 1 0 0 0 0 2 2 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH09_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech09.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech092.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech093.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech09.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech09r.avi"

#define MATRIX10_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 1 0 0	\
					0 0 0 0 0 1 1 0 	\
					 0 1 0 0 0 1 0 0	\
					0 0 0 0 0 1 1 0 	\
					 0 0 0 1 0 0 0 0	\
					0 0 0 1 1 0 0 0 	\
					 0 0 1 1 1 0 0 0	\
					0 0 0 1 1 0 0 0 	\
					 0 0 0 1 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 1 0 0	\
					0 0 0 0 0 1 1 0 	\
					 0 2 0 0 0 1 0 0	\
					0 0 0 0 0 1 1 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 3 0 0	\
					0 0 0 0 0 3 3 0 	\
					 0 4 0 0 0 3 0 0	\
					0 0 0 0 0 3 3 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH10_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech10.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech102.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech103.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech10.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech10r.avi"

#define MATRIX12_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					1 0 1 1 0 0 1 1 	\
					 0 1 1 1 0 1 1 1	\
					0 0 1 1 0 0 1 1 	\
					 1 0 0 0 0 1 1 1	\
					1 1 0 1 1 0 0 0 	\
					 1 0 1 1 1 0 1 1	\
					1 1 0 1 1 1 0 1 	\
					 0 0 1 1 1 1 0 0	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
				}				\
								\
				slot_types {			\
					2 0 1 1 0 0 1 1 	\
					 0 1 1 1 0 1 1 1	\
					0 0 1 1 0 0 1 1 	\
					 1 0 0 0 0 1 1 1	\
					1 1 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 3 3	\
					1 1 0 0 0 0 0 3 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					4 0 1 1 0 0 2 2 	\
					 0 1 1 1 0 2 2 2	\
					0 0 1 1 0 0 2 2 	\
					 3 0 0 0 0 2 2 2	\
					3 3 0 0 0 0 0 0 	\
					 3 0 0 0 0 0 5 5	\
					3 3 0 0 0 0 0 5 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH12_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech12.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech122.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech123.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech12.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech12r.avi"

#define MATRIX13_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 0 1 0 0 0 0 1 0	\
					0 1 1 0 0 0 1 1 	\
					 0 1 0 0 0 0 1 0	\
					0 0 0 0 0 0 1 1 	\
					 0 0 0 1 0 0 0 0	\
					1 1 0 0 0 0 1 1 	\
					 1 1 0 0 0 1 1 0	\
					0 0 1 1 1 1 0 0 	\
					 0 1 1 1 1 1 0 0	\
					0 1 1 1 1 1 1 0 	\
					 0 0 1 1 1 0 0 0	\
					0 1 1 0 0 1 1 0 	\
					 0 1 0 0 0 1 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 0 1 0 0 0 0 1 0	\
					0 1 1 0 0 0 1 1 	\
					 0 1 0 0 0 0 1 0	\
					0 0 0 0 0 0 1 1 	\
					 0 0 0 2 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 0 1 0 0 0 0 2 0	\
					0 1 1 0 0 0 2 2 	\
					 0 1 0 0 0 0 2 0	\
					0 0 0 0 0 0 2 2 	\
					 0 0 0 4 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH13_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech13.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech132.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech133.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech13.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech13r.avi"

#define MATRIX14_BODY		msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 1 1 0 0 1 0	\
					1 1 0 1 0 0 1 1 	\
					 1 0 0 0 1 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 1 1 1 1 1 1 0 	\
					 0 1 1 1 1 1 0 0	\
					1 1 1 1 1 1 1 1 	\
					 0 1 1 1 1 1 0 0	\
					0 0 1 1 1 1 0 0 	\
					 1 1 1 1 1 1 1 0	\
					0 0 0 1 1 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 3 3 0 0 1 0	\
					1 1 0 3 0 0 1 1 	\
					 1 0 0 0 2 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 4 4 0 0 2 0	\
					1 1 0 4 0 0 2 2 	\
					 1 0 0 0 5 0 2 0	\
					1 1 0 0 0 0 2 2 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				id	$MECH14_NAME		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech14.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech142.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech143.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech14.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech14r.avi"

#define MATRIX17_MATRIX 	msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 1 0	\
					1 1 0 1 1 0 1 1 	\
					 1 0 0 1 0 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 1 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 1 1 0 1 1 0 0	\
					0 1 1 1 1 1 1 0 	\
					 0 0 1 1 1 0 0 0	\
					0 1 1 1 1 1 1 0 	\
					 1 1 0 0 0 1 1 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 1 0	\
					1 1 0 3 3 0 1 1 	\
					 1 0 0 3 0 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 2 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 2 0	\
					1 1 0 4 4 0 2 2 	\
					 1 0 0 4 0 0 2 0	\
					1 1 0 0 0 0 2 2 	\
					 0 0 0 5 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}

#define MATRIX17_BODY		id	$MECH17_NAME		\
				$MATRIX17_MATRIX		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech17.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech172.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech173.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech17.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech17r.avi"

#define MATRIX32_BODY		id	$MECH32_NAME		\
				$MATRIX17_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech32.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech32.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech32.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech17.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech17r.avi"

#define MATRIX33_BODY		id	$MECH33_NAME		\
				$MATRIX17_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech33.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech33.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech33.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech17.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech17r.avi"

#define MATRIX34_BODY		id	$MECH34_NAME		\
				$MATRIX17_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech34.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech34.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech34.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech17.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech17r.avi"

#define MATRIX16_MATRIX 	msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 1 0	\
					1 1 0 1 1 0 1 1 	\
					 1 0 0 1 0 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 1 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 1 1 0 1 1 0 0	\
					0 0 0 1 1 0 0 0 	\
					 0 0 1 1 1 0 0 0	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 0 1 1 1 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 1 0	\
					1 1 0 3 3 0 1 1 	\
					 1 0 0 3 0 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 2 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 2 0	\
					1 1 0 4 4 0 2 2 	\
					 1 0 0 4 0 0 2 0	\
					1 1 0 0 0 0 2 2 	\
					 0 0 0 5 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}

#define MATRIX16_BODY		id	$MECH16_NAME		\
				$MATRIX16_MATRIX		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech16.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech162.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech163.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech16.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech16r.avi"

#define MATRIX29_BODY		id	$MECH29_NAME		\
				$MATRIX16_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech29.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech29.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech29.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech16.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech16r.avi"

#define MATRIX30_BODY		id	$MECH30_NAME		\
				$MATRIX16_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech30.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech30.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech30.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech16.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech16r.avi"

#define MATRIX31_BODY		id	$MECH31_NAME		\
				$MATRIX16_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech31.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech31.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech31.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech16.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech16r.avi"

#define MATRIX15_MATRIX 	msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 1 1 0 0 0 1 1 	\
					 1 1 1 0 0 1 1 1	\
					0 1 1 0 1 0 1 1 	\
					 1 1 1 0 0 1 1 1	\
					0 0 0 0 0 0 0 0 	\
					 1 1 0 1 1 0 1 1	\
					1 1 1 0 1 0 1 1 	\
					 1 1 1 0 0 1 1 1	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 1 1 1 1 1	\
					1 1 1 1 1 1 1 1 	\
					 1 1 1 0 0 1 1 1	\
				}				\
								\
				slot_types {			\
					0 1 1 0 0 0 1 1 	\
					 1 1 1 0 0 1 1 1	\
					0 1 1 0 2 0 1 1 	\
					 1 1 1 0 0 1 1 1	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 3 3 0 0 0	\
					0 0 0 0 3 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 1 1 0 0 0 2 2 	\
					 1 1 1 0 0 2 2 2	\
					0 1 1 0 4 0 2 2 	\
					 1 1 1 0 0 2 2 2	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 5 5 0 0 0	\
					0 0 0 0 5 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}


#define MATRIX15_BODY		id	$MECH15_NAME		\
				$MATRIX15_MATRIX		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech15.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech152.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech153.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech15.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech15r.avi"

#define MATRIX26_BODY		id	$MECH26_NAME		\
				$MATRIX15_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech26.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech26.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech26.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech15.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech15r.avi"

#define MATRIX27_BODY		id	$MECH27_NAME		\
				$MATRIX15_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech27.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech27.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech27.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech15.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech15r.avi"

#define MATRIX28_BODY		id	$MECH28_NAME		\
				$MATRIX15_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech28.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech28.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech28.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech15.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech15r.avi"

#define MATRIX18_MATRIX 	msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 1 0 0 0 0	\
					0 0 0 1 1 0 0 0 	\
					 0 0 0 1 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
								\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}

#define MATRIX18_BODY		$MATRIX18_MATRIX		\
				id	$MECH18_NAME		\
				raffa				\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech18.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech182.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech183.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech18.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech18r.avi"

#define MATRIX19_BODY		$MATRIX18_MATRIX		\
				id	$MECH19_NAME		\
				raffa				\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech19.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech192.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech193.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech19.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech19r.avi"

#define MATRIX20_BODY		$MATRIX18_MATRIX		\
				id	$MECH20_NAME		\
				raffa				\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech20.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech202.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech203.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech20.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech20r.avi"

#define MATRIX21_BODY		$MATRIX18_MATRIX		\
				id	$MECH21_NAME		\
				raffa				\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech21.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech212.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech213.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech21.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech21r.avi"

#define MATRIX22_BODY		$MATRIX18_MATRIX		\
				id	$MECH22_NAME		\
				raffa				\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech22.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech222.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech223.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech22.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech22r.avi"

#define MATRIX35_MATRIX 	msize_x 8			\
				msize_y 14			\
				matrix {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 1 0	\
					1 1 0 1 1 0 1 1 	\
					 1 0 0 1 0 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 1 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 1 1 0 0 1 1 0	\
					0 0 0 1 1 1 0 0 	\
					 1 1 1 1 1 1 1 1	\
					0 0 1 1 1 1 1 0 	\
					 0 0 1 0 0 0 1 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_types {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 1 0	\
					1 1 0 3 3 0 1 1 	\
					 1 0 0 3 0 0 1 0	\
					1 1 0 0 0 0 1 1 	\
					 0 0 0 2 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}				\
				slot_nums {			\
					0 0 0 0 0 0 0 0 	\
					 1 0 0 0 0 0 2 0	\
					1 1 0 4 4 0 2 2 	\
					 1 0 0 4 0 0 2 0	\
					1 1 0 0 0 0 2 2 	\
					 0 0 0 5 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
					0 0 0 0 0 0 0 0 	\
					 0 0 0 0 0 0 0 0	\
				}

#define MATRIX35_BODY		id	$MECH35_NAME		\
				$MATRIX35_MATRIX		\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech35.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech352.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech353.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech35.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech35r.avi"

#define MATRIX36_BODY		id	$MECH36_NAME		\
				$MATRIX35_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech36.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech36.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech36.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech35.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech35r.avi"

#define MATRIX37_BODY		id	$MECH37_NAME		\
				$MATRIX35_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech37.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech37.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech37.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech35.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech35r.avi"

#define MATRIX38_BODY		id	$MECH38_NAME		\
				$MATRIX35_MATRIX		\
				not_complete			\
				num_avi_ids	$ACI_NUM_AVI_ID \
				avi_id		$ACI_PICTURE_AVI_ENG  "mech38.avi"         \
				avi_id		$ACI_PICTURE_AVI_ENG2 "mech38.avi"        \
				avi_id		$ACI_PICTURE_AVI_ENG3 "mech38.avi"        \
				avi_id		$ACI_TEXT_AVI_ENG  "text/mech35.avi"       \
				avi_id		$ACI_TEXT_AVI_RUS  "text/mech35r.avi"

/* ----------------------------- ITEMS SECTION ------------------------------ */

// Item shapes...
#define SHAPE0		shape_len	1	\
			shape { 		\
				0 0		\
			}

#define SHAPE2		shape_len	2	\
			shape { 		\
				0 0		\
				1 0		\
			}

#define _SHAPE2 	shape_len	2	\
			shape { 		\
				0 0		\
				0 1		\
			}

#define SHAPE3		shape_len	3	\
			num_index	1	\
			shape { 		\
				0  0		\
				-1 1		\
				0  1		\
			}

#define SHAPE_DVC	shape_len	3	\
			shape { 		\
				0 0		\
				1 0		\
				0 1		\
			}

#define SHAPE_P 	shape_len	4	\
			shape { 		\
				0 0		\
				1 0		\
				0 1		\
				1 1		\
			}

#define WPN_SHAPE4	shape_len	4	\
			shape { 		\
				0  0		\
				-1 1		\
				0  1		\
				0  2		\
			}			\
			num_index	2

#define WPN_SHAPE6	shape_len	6	\
			shape { 		\
				0  0		\
				-1 1		\
				0  1		\
				0  2		\
				-1 3		\
				0  3		\
			}			\
			num_index	4

#define WPN_SHAPE7	shape_len	7	\
			shape { 		\
				0  0		\
				1  0		\
				-1 1		\
				0  1		\
				1  1		\
				0  2		\
				1  2		\
			}			\
			num_index	5

#define WPN_SHAPE10	num_index	7	\
			shape_len	10	\
			shape { 		\
				 0 0		\
				 1 0		\
				-1 1		\
				 0 1		\
				 1 1		\
				 0 2		\
				 1 2		\
				-1 3		\
				-0 3		\
				 1 3		\
			}

#define SHAPE10 	shape_len	10	\
			shape { 		\
				0 0		\
				1 0		\
				2 0		\
			       -1 1		\
				0 1		\
				1 1		\
				2 1		\
				0 2		\
				1 2		\
				2 2		\
			}

#define SHAPE30 	shape_len	30	\
			shape { 		\
				0	0	\
				1	0	\
				2	0	\
						\
				-1	1	\
				0	1	\
				1	1	\
				2	1	\
						\
				-1	2	\
				0	2	\
				1	2	\
				2	2	\
				3	2	\
						\
				-2	3	\
				-1	3	\
				0	3	\
				1	3	\
				2	3	\
				3	3	\
						\
				-1	4	\
				0	4	\
				1	4	\
				2	4	\
				3	4	\
						\
				-1	5	\
				0	5	\
				1	5	\
				2	5	\
						\
				0	6	\
				1	6	\
				2	6	\
			}

// Mechos parts shapes...
#define SHAPE_WHZ	shape_len	14	\
			shape { 		\
				0 0		\
				1 0		\
						\
			       -1 1		\
				0 1		\
				1 1		\
						\
			       -1 2		\
				0 2		\
				1 2		\
				2 2		\
						\
			       -1 3		\
				0 3		\
				1 3		\
						\
				0 4		\
				1 4		\
			}

#define SHAPE_BOX	shape_len	12	\
			shape { 		\
				0 0		\
				1 0		\
				2 0		\
				3 0		\
						\
				0 1		\
				1 1		\
				2 1		\
				3 1		\
						\
				1 2		\
				2 2		\
				3 2		\
				4 2		\
			}

#define SHAPE_ZZ	shape_len	6	\
			shape { 		\
				0 0		\
				1 0		\
				2 0		\
						\
				0 1		\
				1 1		\
				2 1		\
			}

#define SHAPE_PP	$WPN_SHAPE7

// MacHOTine Gun (light)
#define ITM01		id		$ITM01_ID				\
			$ITM01_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_WPN				\
			$ITM01_INFO						\
			show_shots						\
			$WPN_SHAPE4


// MacHOTine Gun (heavy)
#define ITM02		id		$ITM02_ID				\
			$ITM02_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_WPN				\
			$ITM02_INFO						\
			show_shots						\
			$WPN_SHAPE7

// Speetle System (light)
#define ITM03		id		$ITM03_ID				\
			$ITM03_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_WPN				\
			$ITM03_INFO						\
			show_shots						\
			$WPN_SHAPE6

// Speetle System (heavy)
#define ITM04		id		$ITM04_ID				\
			$ITM04_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_WPN				\
			$ITM04_INFO						\
			show_shots						\
			$WPN_SHAPE10

// ghOrb Gear (light)
#define ITM05		id		$ITM05_ID				\
			$ITM05_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_WPN				\
			$ITM05_INFO						\
			show_shots						\
			$WPN_SHAPE4

// ghOrb Gear (heavy)
#define ITM06		id		$ITM06_ID				\
			$ITM06_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_WPN				\
			$ITM06_INFO						\
			show_shots						\
			$WPN_SHAPE10

// Beebbanoza Blockade
#define ITM07		id		$ITM07_ID				\
			$ITM07_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_WPN				\
			$ITM07_INFO						\
			show_shots						\
			$WPN_SHAPE7

// Crustest Cannon
#define ITM08		id		$ITM08_ID				\
			$ITM08_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_WPN				\
			$ITM08_INFO						\
			show_shots						\
			$WPN_SHAPE10

// Terminator 'Wormer'
#define ITM09		id		$ITM09_ID			\
			$ITM09_PARAMS					\
			class		WEAPON				\
			slot_type	$SLOT_TWPN			\
			$ITM09_INFO					\
			show_shots					\
			$SHAPE0

// Terminator 'Wasp'
#define ITM10		id		$ITM10_ID			\
			$ITM10_PARAMS					\
			class		WEAPON				\
			slot_type	$SLOT_TWPN			\
			$ITM10_INFO					\
			show_shots					\
			$SHAPE0

// Pupok pile
#define ITM11		id		$ITM11_ID			\
			$ITM11_PARAMS					\
			class		WEAPON				\
			show_shots					\
			$SHAPE0

// OKorOK pile
#define ITM12		id		$ITM12_ID			\
			$ITM12_PARAMS					\
			class		WEAPON				\
			show_shots					\
			$SHAPE0

// Boltoon pack
#define ITM13		id		$ITM13_ID			\
			$ITM13_PARAMS					\
			class		WEAPON				\
			show_shots					\
			$SHAPE0

// CopteRig
#define ITM14		id		$ITM14_ID			\
			$ITM14_PARAMS					\
			class		WEAPON				\
			$ITM14_INFO					\
			slot_type	$SLOT_DVC			\
			$SHAPE_DVC


// CopteRig (empty)
#define ITM15		id		$ITM15_ID			\
			$ITM15_PARAMS					\
			class		WEAPON				\
			$ITM15_INFO					\
			$SHAPE_DVC

// CutteRig
#define ITM16		id		$ITM16_ID			\
			$ITM16_PARAMS					\
			class		WEAPON				\
			$ITM16_INFO					\
			slot_type	$SLOT_DVC			\
			$SHAPE_DVC

// CutteRig (empty)
#define ITM17		id		$ITM17_ID			\
			$ITM17_PARAMS					\
			class		WEAPON				\
			$ITM17_INFO					\
			$SHAPE_DVC

// CrotRig
#define ITM18		id		$ITM18_ID			\
			$ITM18_PARAMS					\
			class		WEAPON				\
			$ITM18_INFO					\
			slot_type	$SLOT_DVC			\
			$SHAPE_DVC

// CrotRig (empty)
#define ITM19		id		$ITM19_ID			\
			$ITM19_PARAMS					\
			class		WEAPON				\
			$ITM19_INFO					\
			$SHAPE_DVC

// Lhark - does not exist :)
#define ITM20		id		$ITM20_ID			\
			$ITM20_PARAMS					\
			$SHAPE3

// Lhark - does not exist :)
#define ITM21		id		$ITM21_ID			\
			$ITM21_PARAMS					\
			$SHAPE3


// Vector
#define ITM22		id		$ITM22_ID			\
			$ITM22_PARAMS					\
			class		WEAPON				\
			$ITM22_INFO					\
			slot_type	$SLOT_DVC			\
			$SHAPE_DVC

// Cirtainer
#define ITM23		id		$ITM23_ID			\
			$ITM23_PARAMS					\
			event		$EV_GET_CIRT			\
			$WPN_SHAPE4

// Cirtainer (empty)
#define ITM24		id		$ITM24_ID			\
			$ITM24_PARAMS					\
			$ITM24_INFO					\
			$WPN_SHAPE4					\
			event		$EV_GET_CIRT

// Conlarver
#define ITM25		id		$ITM25_ID			\
			$ITM25_PARAMS					\
			$SHAPE30

// Conlarver (empty)
#define ITM26		id		$ITM26_ID			\
			$ITM26_PARAMS					\
			$ITM26_INFO					\
			$SHAPE30

// Nymbos
#define ITM27		id		$ITM27_ID			\
			$ITM27_PARAMS					\
			$SHAPE3
// Phlegma
#define ITM28		id		$ITM28_ID			\
			$ITM28_PARAMS					\
			$SHAPE3

// Heroin
#define ITM29		id		$ITM29_ID			\
			$ITM29_PARAMS					\
			$SHAPE3

// Shrub
#define ITM30		id		$ITM30_ID			\
			$ITM30_PARAMS					\
			$SHAPE3

// Poponka
#define ITM31		id		$ITM31_ID			\
			$ITM31_PARAMS					\
			$ITM31_INFO					\
			event		$EV_ITEM_TEXT			\
			$SHAPE3

// Toxick
#define ITM32		id		$ITM32_ID			\
			$ITM32_PARAMS					\
			$SHAPE3

// eLeech
#define ITM33		id		$ITM33_ID			\
			$ITM33_PARAMS					\
			$SHAPE3

// Kernoboo
#define ITM34		id		$ITM34_ID			\
			$ITM34_PARAMS					\
			show_shots					\
			$SHAPE0

// Pipetka
#define ITM35		id		$ITM35_ID			\
			$ITM35_PARAMS					\
			show_shots					\
			$SHAPE0

// WeeZyk
#define ITM36		id		$ITM36_ID			\
			$ITM36_PARAMS					\
			$SHAPE3

// Leepuringa
#define ITM41		id		$ITM41_ID			\
			$ITM41_PARAMS					\
			$SHAPE3

// Dead eLeech
#define ITM42		id		$ITM42_ID			\
			$ITM42_PARAMS					\
			$SHAPE3

// Protractor
#define ITM37		id		$ITM37_ID			\
			$ITM37_PARAMS					\
			$ITM37_INFO					\
			$SHAPE3 					\
			$ITEM_PROTRACTOR_MENU

// Mechanic Messiah
#define ITM38		id		$ITM38_ID			\
			$ITM38_PARAMS					\
			$ITM38_INFO					\
			$SHAPE3 					\
			$ITEM_MECH_MESSIAH_MENU

// Function-83+
#define ITM39		id		$ITM39_ID			\
			$ITM39_PARAMS					\
			$ITM39_INFO					\
			$ITEM_WORLDS_MENU				\
			$SHAPE3

// SpuMMy
#define ITM40		id		$ITM40_ID			\
			$ITM40_PARAMS					\
			$SHAPE3

// Rubox
#define ITM43		id		$ITM43_ID			\
			$ITM43_PARAMS					\
			$SHAPE0

// Zigger-Zagger
#define ITM44		id		$ITM44_ID				\
			$ITM44_PARAMS						\
			part_data	$CONSTR1	     { $CONSTR1 + 1 }	\
					{ $CONSTR1 + 2 }     { $CONSTR1 + 3 }	\
			$SHAPE_ZZ

// Cart
#define ITM45		id		$ITM45_ID				\
			$ITM45_PARAMS						\
			part_data	$CONSTR1	     { $CONSTR1 + 2 }	\
					{ $CONSTR1 + 1 }     { $CONSTR1 + 3 }	\
			$SHAPE_BOX

// Put-Put
#define ITM46		id		$ITM46_ID				\
			$ITM46_PARAMS						\
			part_data	$CONSTR2	     { $CONSTR2 + 1 }	\
					{ $CONSTR2 + 2 }     { $CONSTR2 + 3 }	\
			$SHAPE_PP

// Box
#define ITM47		id		$ITM47_ID				\
			$ITM47_PARAMS						\
			part_data	$CONSTR2	     { $CONSTR2 + 2 }	\
					{ $CONSTR2 + 1 }     { $CONSTR2 + 3 }	\
			$SHAPE_BOX

// Rattle
#define ITM48		id		$ITM48_ID				\
			$ITM48_PARAMS						\
			part_data	$CONSTR3	     { $CONSTR3 + 1 }	\
					{ $CONSTR3 + 2 }     { $CONSTR3 + 3 }	\
			$SHAPE_ZZ

// Heavy Iron
#define ITM49		id		$ITM49_ID				\
			$ITM49_PARAMS						\
			part_data	$CONSTR3	     { $CONSTR3 + 2 }	\
					{ $CONSTR3 + 1 }     { $CONSTR3 + 3 }	\
			$SHAPE_BOX

// Star
#define ITM50		id		$ITM50_ID				\
			$ITM50_PARAMS						\
			part_data	$CONSTR4	     { $CONSTR4 + 1 }	\
					{ $CONSTR4 + 2 }     { $CONSTR4 + 3 }	\
			$SHAPE_PP

// Whizzer
#define ITM51		id		$ITM51_ID				\
			$ITM51_PARAMS						\
			part_data	$CONSTR4	     { $CONSTR4 + 2 }	\
					{ $CONSTR4 + 1 }     { $CONSTR4 + 3 }	\
			$SHAPE_WHZ

// Husker
#define ITM52		id		$ITM52_ID				\
			$ITM52_PARAMS						\
			part_data	$CONSTR5	     { $CONSTR5 + 1 }	\
					{ $CONSTR5 + 2 }     { $CONSTR5 + 3 }	\
			$SHAPE_PP

// Cogged Widget
#define ITM53		id		$ITM53_ID				\
			$ITM53_PARAMS						\
			part_data	$CONSTR5	     { $CONSTR5 + 2 }	\
					{ $CONSTR5 + 1 }     { $CONSTR5 + 3 }	\
			$SHAPE_BOX

// TankAcid
#define ITM54		id		$ITM54_ID				\
			$ITM54_PARAMS						\
			event		$EV_GET_CIRT				\
			$SHAPE0

// Boot Sector
#define ITM56		id		$ITM56_ID				\
			$ITM56_PARAMS						\
			$ITM56_INFO						\
			$ITEM_TELEPORT_MENU					\
			$SHAPE3

// Peelot
#define ITM55		id		$ITM55_ID				\
			$ITM55_PARAMS						\
			$ITM55_INFO						\
			event		$EV_GET_CIRT				\
			$SHAPE3

// Mechoscope
#define ITM57		id		$ITM57_ID				\
			$ITM57_PARAMS						\
			class		WEAPON					\
			show_shots						\
			$SHAPE0

// Gluek
#define ITM58		id		$ITM58_ID				\
			$ITM58_PARAMS						\
			class		WEAPON					\
			$ITM58_INFO						\
			event		$EV_GET_CIRT				\
			$SHAPE0

// Degradator
#define ITM59		id		$ITM59_ID				\
			$ITM59_PARAMS						\
			class		WEAPON					\
			show_shots						\
			$SHAPE0

// Tabutask (active)
#define ITM60		id		$ITM60_ID				\
			$ITM60_PARAMS						\
			$ITM60_INFO						\
			event		$EV_ITEM_TEXT				\
			$SHAPE2

// Amputator
#define ITM61		id		$ITM61_ID				\
			$ITM61_PARAMS						\
			class		WEAPON					\
			show_shots						\
			$SHAPE0

// Eleepod
#define ITM62		id		$ITM62_ID				\
			$ITM62_PARAMS						\
			$ITM62_INFO						\
			event		$EV_ITEM_TEXT				\
			$SHAPE10

// Beeboorat
#define ITM63		id		$ITM63_ID				\
			$ITM63_PARAMS						\
			$ITM63_INFO						\
			event		$EV_ITEM_TEXT				\
			$SHAPE10

// Zeex
#define ITM64		id		$ITM64_ID				\
			$ITM64_PARAMS						\
			$ITM64_INFO						\
			event		$EV_ITEM_TEXT				\
			$SHAPE10

// Palochka
#define ITM65		id		$ITM65_ID				\
			$ITM65_PARAMS						\
			$SHAPE2

// Pipka
#define ITM66		id		$ITM66_ID				\
			$ITM66_PARAMS						\
			$SHAPE0

// Nobool
#define ITM67		id		$ITM67_ID				\
			$ITM67_PARAMS						\
			$SHAPE0

// Boorawchick
#define ITM68		id		$ITM68_ID				\
			$ITM68_PARAMS						\
			$WPN_SHAPE7

// Pereponka
#define ITM69		id		$ITM69_ID				\
			$ITM69_PARAMS						\
			$SHAPE3

// Zeefick
#define ITM70		id		$ITM70_ID				\
			$ITM70_PARAMS						\
			$SHAPE3

// Dead WeeZyk
#define ITM71		id		$ITM71_ID				\
			$ITM71_PARAMS						\
			$SHAPE3

// Rotten Kernoboo
#define ITM72		id		$ITM72_ID				\
			$ITM72_PARAMS						\
			$SHAPE0

// Tabutask (failed)
#define ITM73		id		$ITM73_ID				\
			$ITM73_PARAMS						\
			$SHAPE2

// Tabutask (completed)
#define ITM74		id		$ITM74_ID				\
			$ITM74_PARAMS						\
			event		$EV_ITEM_TEXT				\
			$SHAPE2

// Amputator (empty)
#define ITM75		id		$ITM75_ID				\
			$ITM75_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_TWPN				\
			no_activate						\
			$SHAPE0

// Degradator (empty)
#define ITM76		id		$ITM76_ID				\
			$ITM76_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_TWPN				\
			no_activate						\
			$SHAPE0

// Rotten Pipetka
#define ITM77		id		$ITM77_ID				\
			$ITM77_PARAMS						\
			$SHAPE0

// Mechoscope (empty)
#define ITM78		id		$ITM78_ID				\
			$ITM78_PARAMS						\
			class		WEAPON					\
			slot_type	$SLOT_TWPN				\
			no_activate						\
			$SHAPE0

/* -------------------------------------------------------------------------- */


#define TARGETS_MENU_BODY	fncMenuItem {					\
					string		$STR_OFF		\
					function	$FMENU_OFF		\
										\
					font		$FMENU_TARGETS_FONT	\
					space		$FMENU_TARGETS_SPACE	\
				}


/* ----------------------------- Colors Section ----------------------------- */

num_col_schemes 	$NUM_COL_SCHEMES
col_scheme_len		$COL_SCHEME_LEN

color_scheme $SCH_ISCREEN {
	color $ACI_BACK_COL		1

	color $FM_SELECT_COL		60
	color $FM_UNSELECT_COL		8

	color $FM_SELECT_BORDER_COL	53
	color $FM_UNSELECT_BORDER_COL	4

	color $INV_CELL_COL_ITM 	4
	color $INV_BCELL_COL_ITM	3
	color $INV_FLOOR_COL_ITM	90
	color $INV_BFLOOR_COL_ITM	2

	color $INV_CELL_COL_WPN 	23
	color $INV_BCELL_COL_WPN	21
	color $INV_CELL_FLOOR_WPN	20
	color $INV_BCELL_FLOOR_WPN	18

	color $INV_CELL_COL_DVC 	53
	color $INV_BCELL_COL_DVC	51
	color $INV_CELL_FLOOR_DVC	52
	color $INV_BCELL_FLOOR_DVC	50

	color $INV_CELL_COL_TWPN	23
	color $INV_BCELL_COL_TWPN	21
	color $INV_CELL_FLOOR_TWPN	20
	color $INV_BCELL_FLOOR_TWPN	18

	color $INV_CELL_SELECT_COL	55

	color $INV_NUM_ITM_COL		0
	color $INV_NUM_WPN_COL		0
	color $INV_NUM_TWPN_COL 	0
	color $INV_NUM_DVC_COL		0

	color $IND_BACK_COL		1

	color $COUNTER_COL		60
	color $COUNTER_BORDER_COL	53

	color $FM_SELECT_START		48
	color $FM_SELECT_SIZE		1

	color $FM_UNSELECT_START	5
	color $FM_UNSELECT_SIZE 	2
}

color_scheme $SCH_DEFAULT {
	color $ACI_BACK_COL		184

	color $FM_UNSELECT_COL		83
	color $FM_SELECT_COL		100

	color $COUNTER_COL		100
	color $COUNTER_BORDER_COL	91

	color $FM_UNSELECT_BORDER_COL	80
	color $FM_SELECT_BORDER_COL	91

	color $IND_NRG_COL1		229
	color $IND_NRG_COL2		143

	color $IND_N0_COL		87

	color $INV_CELL_COL_ITM 	82
	color $INV_BCELL_COL_ITM	81
	color $INV_FLOOR_COL_ITM	80
	color $INV_BFLOOR_COL_ITM	72

	color $INV_CELL_COL_WPN 	82
	color $INV_BCELL_COL_WPN	81
	color $INV_CELL_FLOOR_WPN	71
	color $INV_BCELL_FLOOR_WPN	64

	color $INV_CELL_COL_DVC 	82
	color $INV_BCELL_COL_DVC	81
	color $INV_CELL_FLOOR_DVC	44
	color $INV_BCELL_FLOOR_DVC	37

	color $INV_CELL_COL_TWPN	82
	color $INV_BCELL_COL_TWPN	81
	color $INV_CELL_FLOOR_TWPN	71
	color $INV_BCELL_FLOOR_TWPN	64

//	  color $INV_CELL_SELECT_COL	  16
	color $INV_CELL_SELECT_COL	103

	color $INV_NUM_ITM_COL		87
	color $INV_NUM_WPN_COL		87
	color $INV_NUM_TWPN_COL 	87
	color $INV_NUM_DVC_COL		87
	color $COUNTER_COL		87

	color $IND_BACK_COL		184

	color $WORLD_LINK_COL		83
	color $WORLD_ACC_LINK_COL	110

	color $WORLD_VISIBLE_COL	225
	color $WORLD_CURRENT_COL	225
	color $WORLD_SPECIAL_COL	225
	color $WORLD_EXPLORED_COL	225
	color $WORLD_ACCESSIBLE_COL	225
/*
	color $WORLD_BORDER_VIS_COL	228
	color $WORLD_BORDER_CUR_COL	63
	color $WORLD_BORDER_SPC_COL	149
	color $WORLD_BORDER_EXP_COL	230
	color $WORLD_BORDER_ACC_COL	100
*/

	color $WORLD_BORDER_VIS_COL	228
	color $WORLD_BORDER_CUR_COL	110
	color $WORLD_BORDER_SPC_COL	149
	color $WORLD_BORDER_EXP_COL	100
	color $WORLD_BORDER_ACC_COL	100

	color $IND_LOAD_COL		87
	color $IND_WPN_COL		16
	color $IND_DVC_COL		100

	color $JUMP_BORDER_COL		229
	color $JUMP_FILLED_COL		143
	color $JUMP_EMPTY_COL		1

	color $FM_SELECT_START		88
	color $FM_SELECT_SIZE		1

	color $FM_UNSELECT_START	227
	color $FM_UNSELECT_SIZE 	3

	color $FM_SELECT_START2 	104
	color $FM_SELECT_SIZE2		2
}

info_panel {	// Parameters info panel...
	id	$ACI_PARAMS_PANEL

	font		0
	range_font
}

/* EOF */

