
#define ACI_NUM_SERVERS 	5
#define ACI_SERVER_NAME_LEN	36

#define ACI_LOAD_CODE		1000
#define ACI_GLOBAL_EXIT_CODE	1001

#define NUM_COL_SCHEMES 	2
#define COL_SCHEME_LEN		60

// Button IDs...
#define ACI_WPN_PICKUP_BUTTON	2001
#define ACI_ITM_PICKUP_BUTTON	2002

// Prompt IDs...
#define ACI_ITEM_PROMPT 	1001
#define ACI_NO_CASH_PROMPT	1002
#define ACI_NO_CASH_PROMPT2	1003
#define ACI_BT15_PROMPT 	1004
#define ACI_PICKUP_ITM_ON	1005
#define ACI_PICKUP_ITM_OFF	1006
#define ACI_PICKUP_WPN_ON	1007
#define ACI_PICKUP_WPN_OFF	1008
#define ACI_AVI_PROMPT1 	1009
#define ACI_AVI_PROMPT2 	1010

// Color scheme IDs...
#define SCH_DEFAULT		0
#define SCH_ISCREEN		1

// Color index...
#define ACI_BACK_COL		0

#define FM_UNSELECT_COL 	1
#define FM_SELECT_COL		2

#define IND_NRG_COL1		3
#define IND_NRG_COL2		4

#define IND_N0_COL		5

#define INV_CELL_COL_ITM	6
#define INV_BCELL_COL_ITM	7
#define INV_FLOOR_COL_ITM	8
#define INV_BFLOOR_COL_ITM	9

#define INV_CELL_COL_WPN	10
#define INV_BCELL_COL_WPN	11
#define INV_CELL_FLOOR_WPN	12
#define INV_BCELL_FLOOR_WPN	13

#define INV_CELL_COL_TWPN	14
#define INV_BCELL_COL_TWPN	15
#define INV_CELL_FLOOR_TWPN	16
#define INV_BCELL_FLOOR_TWPN	17

#define INV_CELL_COL_DVC	18
#define INV_BCELL_COL_DVC	19
#define INV_CELL_FLOOR_DVC	20
#define INV_BCELL_FLOOR_DVC	21

#define IND_BACK_COL		22

#define INV_CELL_SELECT_COL	23

#define INV_NUM_ITM_COL 	24
#define INV_NUM_WPN_COL 	25
#define INV_NUM_TWPN_COL	26
#define INV_NUM_DVC_COL 	27

#define COUNTER_COL		28

// Links...
#define WORLD_LINK_COL		29
#define WORLD_ACC_LINK_COL	30

// Floors...
#define WORLD_VISIBLE_COL	31
#define WORLD_CURRENT_COL	32
#define WORLD_SPECIAL_COL	33
#define WORLD_EXPLORED_COL	34
#define WORLD_ACCESSIBLE_COL	35

// Borders...
#define WORLD_BORDER_VIS_COL	36
#define WORLD_BORDER_CUR_COL	37
#define WORLD_BORDER_SPC_COL	38
#define WORLD_BORDER_EXP_COL	39
#define WORLD_BORDER_ACC_COL	40

#define FM_SELECT_BORDER_COL	41
#define FM_UNSELECT_BORDER_COL	42

#define COUNTER_BORDER_COL	43

#define IND_LOAD_COL		44
#define IND_WPN_COL		45
#define IND_DVC_COL		46

#define JUMP_BORDER_COL 	47
#define JUMP_EMPTY_COL		48
#define JUMP_FILLED_COL 	49

#define FM_UNSELECT_START	50
#define FM_SELECT_START 	51

#define FM_UNSELECT_SIZE	52
#define FM_SELECT_SIZE		53

#define FM_SELECT_START2	54
#define FM_SELECT_SIZE2 	55

#define ACI_SHUTTER_LEFT	-1001
#define ACI_SHUTTER_RIGHT	-1002
#define ACI_SHUTTER_CENTER	-1003

/* ---------------------------- LOCATION SECTION ---------------------------- */

// Location numIndex...
#define ACI_LOCATION_INDEX_SIZE 	2

#define ACI_BUTTON_ON_TYPE		0
#define ACI_BUTTON_OFF_TYPE		1

// Location objects IDs...
#define ACI_NUM_OBJECTS 	20

#define ACI_SHOP_SCREEN_ID	0
#define ACI_PRICE_PANEL_ID	1
#define ACI_MATRIX_BORDER_ID	2
#define ACI_SMATRIX_BORDER_ID	3
#define ACI_MECHOS_ID_OBJ	4
#define ACI_MECHOS_ID_STR	5
#define ACI_MECHOS_AVI_ID	6
#define ACI_TRIGGER_ID		7
#define ACI_INFO_PANEL_ID	8
#define ACI_CREDITS_PANEL_ID	9
#define ACI_FADE1_ID		10
#define ACI_FADE2_ID		11

#define ACI_ITEMS_MENU_ID	12
#define ACI_AVI_OBJ_ID		13

#define ACI_WEAPONS_BUTTON_ID	14
#define ACI_MECHOS_BUTTON_ID	15
#define ACI_ITEMS_BUTTON_ID	16
#define ACI_MENU_BUTTON_ID	17

#define ACI_NUM_S_OBJECTS	3

#define ACI_QUEST_PANEL_ID	0
#define ACI_ANSWER_PANEL_ID	1
#define ACI_TIME_PANEL_ID	2

// Location info panel IDs...
#define ACI_INFO_PANEL		0
#define ACI_QUEST_PANEL 	1
#define ACI_ANSWER_PANEL	2
#define ACI_PRICE_PANEL 	3
#define ACI_SHOP_TIME_PANEL	4

// Location info menus...
#define ACI_QUEST_MENU		2

// Location counter panels..
#define ACI_CREDITS_PANEL	1

// AVI Indexes...
#define ACI_PICTURE_AVI_ENG	0
#define ACI_TEXT_AVI_ENG	1
#define ACI_TEXT_AVI_RUS	2
#define ACI_PICTURE_AVI_ENG2	3
#define ACI_PICTURE_AVI_ENG3	4

#define ACI_NUM_AVI_ID		5

/* ------------------------ END OF LOCATION SECTION ------------------------- */

// actInt info panel types...
#define INT_PANEL		1
#define INV_PANEL		2
#define INF_PANEL		3

// actInt info panel IDs...
#define ACI_TIME_PANEL		4
#define ACI_PARAMS_PANEL	5

/* -------------------------- WORLD SCHEME SECTION -------------------------- */

// aciWorldInfo IDs...
#define GLORX_ID		0
#define FOSTRAL_ID		1
#define NECROSS_ID		2
#define XPLO_ID 		3
#define B_ID			4
#define C_ID			5
#define K_ID			6
#define T_ID			7
#define H_ID			8
#define E_ID			9
#define F_ID			10
#define W_ID			11
#define A_ID			12

#define AS_MAX_WORLD		13

#define GLORX_LETTER		71
#define FOSTRAL_LETTER		70
#define NECROSS_LETTER		78
#define XPLO_LETTER		88
#define B_LETTER		66
#define C_LETTER		67
#define K_LETTER		75
#define T_LETTER		84
#define H_LETTER		72
#define E_LETTER		69
#define F_LETTER		70
#define W_LETTER		87
#define A_LETTER		65

// aciWorldInfo flags...
#define AS_VISIBLE_WORLD	1
#define AS_EXPLORED_WORLD	2
#define AS_ACCESSIBLE_WORLD	4
#define AS_CURRENT_WORLD	8
#define AS_SECRET_WORLD 	16
#define AS_UNLINKED_WORLD	32
#define AS_SPECIAL_WORLD	64

// aciWorldInfo shapes...
#define AS_NUM_WORLD_SHAPES	3

#define AS_TRIANGLE		0
#define AS_SQUARE		1
#define AS_CIRCLE		2

// Counters...
#define CREDITS_COUNTER 	1
#define TIME_COUNTER		2

#define SHOP_ITEMS_MENU_ID	1

// Shop buy/sell modes...
#define MECHOS_MODE		3
#define ITEMS_MODE		2
#define WEAPONS_MODE		1
#define EMPTY_MODE		4

#define MECHOS_LIST_MODE	7
#define ITEMS_LIST_MODE 	6
#define WEAPONS_LIST_MODE	5

#define CHANGE_2_MECHOS_MODE	8
#define CHANGE_2_ITEMS_MODE	9
#define CHANGE_2_WEAPONS_MODE	10

// Jump counter...
#define JUMP_SIZE_X		500
#define JUMP_SIZE_Y		30

#define JUMP_Y			175

/* -------------------------------------------------------------------------- */

// fncMenu functions...
#define FMENU_OFF		0
#define FMENU_MAP		1
#define FMENU_WMAP		2
#define FMENU_TARGETS		3
#define FMENU_CAMERAS		4
#define FMENU_TELEPORT		5
#define FMENU_PARAMETERS	6

// fncMenu IDs...
#define FMENU_TARGETS_MENU	10
#define FMENU_WORLDS_MENU	11
#define FMENU_PROTRACTOR_MENU	12
#define FMENU_MECH_MESSIAH_MENU 13
#define FMENU_TELEPORT_MENU	14

// fncMenu fonts...
#define FMENU_TARGETS_FONT	1
#define FMENU_TARGETS_SPACE	1

// Bitmap Menu IDs...
#define BMENU_CAMERAS_MENU	1

// Panel fonts...
#define ACI_PHRASE_FONT 	0

#define BMENU_ITEM_ROT		1
#define BMENU_ITEM_ZOOM 	2
#define BMENU_ITEM_PERSP	3

// iScreen Text IDs...
#define iTEXT_ENG1_ID		1
#define iTEXT_ENG2_ID		2
#define iTEXT_ENG3_ID		3

#define iTEXT_RUS1_ID		4
#define iTEXT_RUS2_ID		5
#define iTEXT_RUS3_ID		6

#define iTEXT_ENG_MAX		3
#define iTEXT_RUS_MAX		6

// iScreen Text iEvLineID values...
#define iTEXT_NORMAL		1
#define iTEXT_CHANGE		2
#define iTEXT_END		3

// iScreen Text Event codes
#define iTEXT_PREV_EVENT_CODE0	1000
#define iTEXT_PREV_EVENT_CODE1	1001

#define iTEXT_NEXT_EVENT_CODE0	1002
#define iTEXT_NEXT_EVENT_CODE1	1003

#define iTEXT_END_EVENT_CODE0	1004
#define iTEXT_END_EVENT_CODE1	1005

// Mouse scancodes...
#define iMOUSE_LEFT_PRESS_CODE		-1
#define iMOUSE_RIGHT_PRESS_CODE 	-2
#define iMOUSE_MOVE_CODE		-3
#define iMOUSE_LEFT_MOVE		-4
#define iMOUSE_RIGHT_MOVE		-5

/* --------------------------------- SOUNDS --------------------------------- */

#define ACI_NOISE_SOUND 	0
#define ACI_WND_SOUND		1
#define ACI_SPEECH0_SOUND	2
#define ACI_SPEECH1_SOUND	3
#define ACI_SPEECH2_SOUND	4
#define ACI_SPEECH3_SOUND	5

#define ACI_GATE_SOUND		6

// Podish...
#define ACI_L0_OUTIN_SOUND	7
#define ACI_L0_SWARM_SOUND	8
#define ACI_L0_ANGRY_SOUND	9

#define ACI_L0_BUTTON0_SOUND	10
#define ACI_L0_BUTTON1_SOUND	11
#define ACI_L0_BUTTON2_SOUND	12
#define ACI_L0_BUTTON3_SOUND	13
#define ACI_L0_BUTTON4_SOUND	14

// Incubator...
#define ACI_L1_SWARM_SOUND	15
#define ACI_L1_STEP0_SOUND	16
#define ACI_L1_ANGRY_SOUND	17

#define ACI_L1_BUTTON0_SOUND	18
#define ACI_L1_BUTTON1_SOUND	19
#define ACI_L1_BUTTON2_SOUND	20

// VigBoo...
#define ACI_L2_STEP_SOUND	21
#define ACI_L2_OUTIN_SOUND	22

#define ACI_L2_BUTTON0_SOUND	23
#define ACI_L2_BUTTON1_SOUND	24

#define ACI_L2_GETOUT_SOUND	25

// Lampasso...
#define ACI_L3_STEP_SOUND	26
#define ACI_L3_GETOUT_SOUND	27

// Ogorod...
#define ACI_L4_STEP_SOUND	28
#define ACI_L4_HAND_SOUND	29
#define ACI_L4_PUT_SOUND	30

#define ACI_L4_BUTTON0_SOUND	31
#define ACI_L4_BUTTON1_SOUND	32

#define ACI_L4_GETOUT_SOUND	33

// ZeePa...
#define ACI_L5_SCRATCH_SOUND	34
#define ACI_L5_CLAP_SOUND	35
#define ACI_L5_GETOUT_SOUND	36
#define ACI_L5_ANGRY_SOUND	37

#define ACI_L5_BUTTON0_SOUND	38
#define ACI_L5_BUTTON1_SOUND	39

// B-Zone...
#define ACI_L6_SCRATCH_SOUND	40
#define ACI_L6_CLAP_SOUND	41
#define ACI_L6_GETOUT_SOUND	42

/* ------------------------------- PARAMETERS ------------------------------- */

// Mechos...
#define ACI_MECHOS_ENERGY_SHIELD		0
#define ACI_MECHOS_RESTORING_SPEED		1
#define ACI_MECHOS_MECHANIC_ARMOR		2
#define ACI_MECHOS_VELOCITY			3
#define ACI_MECHOS_SPIRAL_CAPACITY		4
#define ACI_MECHOS_AIR_RESERVE			5

// Weapon...
#define ACI_WEAPON_DAMAGE			0
#define ACI_WEAPON_LOAD 			1
#define ACI_WEAPON_SHOTS_SEC			2
#define ACI_WEAPON_RANGE			3

// Ammo..
#define ACI_AMMO_DAMAGE 			0
#define ACI_AMMO_IN_PACK			1

// Device...
#define ACI_DEVICE_WORKING_TIME 		0

/* ----------------------- END OF PARAMETERS SECTION ------------------------ */

// Cheat codes...
#define ACI_ADD_BEEBOS		2
#define ACI_ADD_LUCK		3
#define ACI_ADD_DOMINANCE	4
#define ACI_ADD_KEYS		5

#define ACI_COMPLETE_TABUTASK	6

#define ACI_ADD_TABUTASK01	7
#define ACI_ADD_TABUTASK02	8
#define ACI_ADD_TABUTASK03	9
#define ACI_ADD_TABUTASK04	10
#define ACI_ADD_TABUTASK05	11
#define ACI_ADD_TABUTASK06	12
#define ACI_ADD_TABUTASK07	13
#define ACI_ADD_TABUTASK08	14
#define ACI_ADD_TABUTASK09	15
#define ACI_ADD_TABUTASK10	16
#define ACI_ADD_TABUTASK11	17
#define ACI_ADD_TABUTASK12	18
#define ACI_ADD_TABUTASK13	19
#define ACI_ADD_TABUTASK14	20
#define ACI_ADD_TABUTASK15	21
#define ACI_ADD_TABUTASK16	22
#define ACI_ADD_TABUTASK17	23
#define ACI_ADD_TABUTASK18	24
#define ACI_ADD_TABUTASK19	25
#define ACI_ADD_TABUTASK20	26

#define ACI_TELEPORT_G		27
#define ACI_TELEPORT_F		28
#define ACI_TELEPORT_N		29
#define ACI_TELEPORT_X		30
#define ACI_TELEPORT_B		31
#define ACI_TELEPORT_K		32
#define ACI_TELEPORT_T		33
#define ACI_TELEPORT_H		34
#define ACI_TELEPORT_W		35
#define ACI_TELEPORT_A		36

#define ACI_END_GAME0		37
#define ACI_END_GAME1		38
#define ACI_END_GAME2		39
#define ACI_END_GAME3		40

#define ACI_CHANGE_CYCLE	41
#define ACI_REPAIR		42

// Save header flags...
#define ACI_SAVE_COMPRESSED	1
