--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Hamel_Icon_NEW.lua (live data217 copy, 11641 B), adopted wholesale.
-- June dropped the clear-difficulty stars, the Henir button (moved to the Special map)
-- and switched the secret dungeon to its _HELL ID with a SecretDungeonDesc lock button;
-- all of that is kept. Each button's third AddDummyInt is the June Hero dungeon ID,
-- which March has no enum for, so it reads 0 (DI_NONE, as June's own Ruben file uses);
-- CX2LocalMapUI only reads it in hero mode. Controls naming June-only content are
-- omitted below with a note where each stood.
--}}

g_pButton_OUTSKIRTS_RESIAM = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_OUTSKIRTS_RESIAM)
g_pButton_OUTSKIRTS_RESIAM:SetName("OUTSKIRTS_RESIAM")
g_pButton_OUTSKIRTS_RESIAM:AddDummyInt(DUNGEON_ID.DI_HAMEL_OUTSKIRTS_RESIAM_NORMAL)
g_pButton_OUTSKIRTS_RESIAM:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_OUTSKIRTS_RESIAM:AddDummyInt(DUNGEON_ID.DI_HAMEL_OUTSKIRTS_RESIAM_HERO_NORMAL)
g_pButton_OUTSKIRTS_RESIAM:SetNormalTex("DLG_Common_Button01_NEW.tga", "OUTSKIRTS_RESIAM_NORMAL")
IconAni(g_pButton_OUTSKIRTS_RESIAM, "DLG_Common_Button01_NEW.tga", "OUTSKIRTS_RESIAM_OVER")
g_pButton_OUTSKIRTS_RESIAM:SetDownTex("DLG_Common_Button01_NEW.tga", "OUTSKIRTS_RESIAM_OVER")
g_pButton_OUTSKIRTS_RESIAM:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(158, 219)
})
g_pButton_OUTSKIRTS_RESIAM:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(151, 211)
})
g_pButton_OUTSKIRTS_RESIAM:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(153, 211)
})
g_pButton_OUTSKIRTS_RESIAM:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_OUTSKIRTS_RESIAM:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_OUTSKIRTS_RESIAM:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_BREAK_RESIAM = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_BREAK_RESIAM)
g_pButton_BREAK_RESIAM:SetName("BREAK_RESIAM")
g_pButton_BREAK_RESIAM:AddDummyInt(DUNGEON_ID.DI_HAMEL_BREAK_RESIAM_NORMAL)
g_pButton_BREAK_RESIAM:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_BREAK_RESIAM:AddDummyInt(DUNGEON_ID.DI_HAMEL_BREAK_RESIAM_HERO_NORMAL)
g_pButton_BREAK_RESIAM:SetNormalTex("DLG_Common_Button01_NEW.tga", "BREAK_RESIAM_Normal")
IconAni(g_pButton_BREAK_RESIAM, "DLG_Common_Button01_NEW.tga", "BREAK_RESIAM_Over")
g_pButton_BREAK_RESIAM:SetDownTex("DLG_Common_Button01_NEW.tga", "BREAK_RESIAM_Over")
g_pButton_BREAK_RESIAM:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(273, 235)
})
g_pButton_BREAK_RESIAM:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(266, 227)
})
g_pButton_BREAK_RESIAM:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(268, 227)
})
g_pButton_BREAK_RESIAM:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_BREAK_RESIAM:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_BREAK_RESIAM:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_ANCIENT_WATERWAY = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ANCIENT_WATERWAY)
g_pButton_ANCIENT_WATERWAY:SetName("ANCIENT_WATERWAY")
g_pButton_ANCIENT_WATERWAY:AddDummyInt(DUNGEON_ID.DI_HAMEL_ANCIENT_WATERWAY_NORMAL)
g_pButton_ANCIENT_WATERWAY:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_ANCIENT_WATERWAY:AddDummyInt(DUNGEON_ID.DI_HAMEL_ANCIENT_WATERWAY_HERO_NORMAL)
g_pButton_ANCIENT_WATERWAY:SetNormalTex("DLG_Common_Button01_NEW.tga", "ANCIENT_WATERWAY_Normal")
IconAni(g_pButton_ANCIENT_WATERWAY, "DLG_Common_Button01_NEW.tga", "ANCIENT_WATERWAY_Over")
g_pButton_ANCIENT_WATERWAY:SetDownTex("DLG_Common_Button01_NEW.tga", "ANCIENT_WATERWAY_Over")
g_pButton_ANCIENT_WATERWAY:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(385, 222)
})
g_pButton_ANCIENT_WATERWAY:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(380, 214)
})
g_pButton_ANCIENT_WATERWAY:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(381, 214)
})
g_pButton_ANCIENT_WATERWAY:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_ANCIENT_WATERWAY:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_ANCIENT_WATERWAY:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_ANCIENT_WATERWAY_CORE = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ANCIENT_WATERWAY_CORE)
g_pButton_ANCIENT_WATERWAY_CORE:SetName("ANCIENT_WATERWAY_CORE")
g_pButton_ANCIENT_WATERWAY_CORE:AddDummyInt(DUNGEON_ID.DI_HAMEL_ANCIENT_WATERWAY_CORE_NORMAL)
g_pButton_ANCIENT_WATERWAY_CORE:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_ANCIENT_WATERWAY_CORE:AddDummyInt(DUNGEON_ID.DI_HAMEL_ANCIENT_WATERWAY_CORE_HERO_NORMAL)
g_pButton_ANCIENT_WATERWAY_CORE:SetNormalTex("DLG_Common_Button01_NEW.tga", "ANCIENT_WATERWAY_CORE_Normal")
IconAni(g_pButton_ANCIENT_WATERWAY_CORE, "DLG_Common_Button01_NEW.tga", "ANCIENT_WATERWAY_CORE_Over")
g_pButton_ANCIENT_WATERWAY_CORE:SetDownTex("DLG_Common_Button01_NEW.tga", "ANCIENT_WATERWAY_CORE_Over")
g_pButton_ANCIENT_WATERWAY_CORE:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(441, 152)
})
g_pButton_ANCIENT_WATERWAY_CORE:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(434, 144)
})
g_pButton_ANCIENT_WATERWAY_CORE:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(436, 144)
})
g_pButton_ANCIENT_WATERWAY_CORE:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_ANCIENT_WATERWAY_CORE:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_ANCIENT_WATERWAY_CORE:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_CAVE_OF_MAGMANTA = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_CAVE_OF_MAGMANTA)
g_pButton_CAVE_OF_MAGMANTA:SetName("CAVE_OF_MAGMANTA")
g_pButton_CAVE_OF_MAGMANTA:AddDummyInt(DUNGEON_ID.DI_HAMEL_CAVE_OF_MAGMANTA_NORMAL)
g_pButton_CAVE_OF_MAGMANTA:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_CAVE_OF_MAGMANTA:AddDummyInt(DUNGEON_ID.DI_HAMEL_CAVE_OF_MAGMANTA_HERO_NORMAL)
g_pButton_CAVE_OF_MAGMANTA:SetNormalTex("DLG_Common_Button01_NEW.tga", "CAVE_OF_MAGMANTA_NORMAL")
g_pButton_CAVE_OF_MAGMANTA:SetOverTex("DLG_Common_Button01_NEW.tga", "CAVE_OF_MAGMANTA_Over")
IconAni(g_pButton_CAVE_OF_MAGMANTA, "DLG_Common_Button01_NEW.tga", "CAVE_OF_MAGMANTA_Over")
g_pButton_CAVE_OF_MAGMANTA:SetDownTex("DLG_Common_Button01_NEW.tga", "CAVE_OF_MAGMANTA_Over")
g_pButton_CAVE_OF_MAGMANTA:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(348, 81)
})
g_pButton_CAVE_OF_MAGMANTA:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(341, 73)
})
g_pButton_CAVE_OF_MAGMANTA:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(343, 73)
})
g_pButton_CAVE_OF_MAGMANTA:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_CAVE_OF_MAGMANTA:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_CAVE_OF_MAGMANTA:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_FROZEN_WATER_TEMPLE = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_FROZEN_WATER_TEMPLE)
g_pButton_FROZEN_WATER_TEMPLE:SetName("FROZEN_WATER_TEMPLE")
g_pButton_FROZEN_WATER_TEMPLE:AddDummyInt(DUNGEON_ID.DI_HAMEL_FROZEN_WATER_TEMPLE_NORMAL)
g_pButton_FROZEN_WATER_TEMPLE:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_FROZEN_WATER_TEMPLE:AddDummyInt(DUNGEON_ID.DI_HAMEL_FROZEN_WATER_TEMPLE_HERO_NORMAL)
g_pButton_FROZEN_WATER_TEMPLE:SetNormalTex("DLG_Common_Button01_NEW.tga", "FROZEN_WATER_TEMPLE_Normal")
g_pButton_FROZEN_WATER_TEMPLE:SetOverTex("DLG_Common_Button01_NEW.tga", "FROZEN_WATER_TEMPLE_Over")
IconAni(g_pButton_FROZEN_WATER_TEMPLE, "DLG_Common_Button01_NEW.tga", "FROZEN_WATER_TEMPLE_Over")
g_pButton_FROZEN_WATER_TEMPLE:SetDownTex("DLG_Common_Button01_NEW.tga", "FROZEN_WATER_TEMPLE_Over")
g_pButton_FROZEN_WATER_TEMPLE:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(233, 50)
})
g_pButton_FROZEN_WATER_TEMPLE:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(226, 42)
})
g_pButton_FROZEN_WATER_TEMPLE:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(228, 42)
})
g_pButton_FROZEN_WATER_TEMPLE:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_FROZEN_WATER_TEMPLE:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_FROZEN_WATER_TEMPLE:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_FROZEN_WATER_HALL = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_FROZEN_WATER_HALL)
g_pButton_FROZEN_WATER_HALL:SetName("FROZEN_WATER_HALL")
g_pButton_FROZEN_WATER_HALL:AddDummyInt(DUNGEON_ID.DI_HAMEL_FROZEN_WATER_HALL_NORMAL)
g_pButton_FROZEN_WATER_HALL:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_FROZEN_WATER_HALL:AddDummyInt(DUNGEON_ID.DI_HAMEL_FROZEN_WATER_HALL_HERO_NORMAL)
g_pButton_FROZEN_WATER_HALL:SetNormalTex("DLG_Common_Button01_NEW.tga", "FROZEN_WATER_HALL_Normal")
g_pButton_FROZEN_WATER_HALL:SetOverTex("DLG_Common_Button01_NEW.tga", "FROZEN_WATER_HALL_Over")
IconAni(g_pButton_FROZEN_WATER_HALL, "DLG_Common_Button01_NEW.tga", "FROZEN_WATER_HALL_Over")
g_pButton_FROZEN_WATER_HALL:SetDownTex("DLG_Common_Button01_NEW.tga", "FROZEN_WATER_HALL_Over")
g_pButton_FROZEN_WATER_HALL:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(138, 51)
})
g_pButton_FROZEN_WATER_HALL:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(131, 43)
})
g_pButton_FROZEN_WATER_HALL:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(133, 43)
})
g_pButton_FROZEN_WATER_HALL:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_FROZEN_WATER_HALL:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_FROZEN_WATER_HALL:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

-- [June UI parity Phase 3] omitted: g_pButton_hamel_Secret_Dungeon (June-only Hamel secret dungeon: March has the DI_HAMEL_SECRET_HELL enum but no DungeonData for it, and its Title_Harmel_Secret key holds different art in March).

-- [June UI parity Phase 3] omitted: g_pButton_hamel_Secret_Dungeon_Desc (June-only Hamel secret dungeon: March has the DI_HAMEL_SECRET_HELL enum but no DungeonData for it, and its Title_Harmel_Secret key holds different art in March).

g_pButton_Event_Dummy01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Event_Dummy01)
g_pButton_Event_Dummy01:SetName("Dungeon")
g_pButton_Event_Dummy01:AddDummyInt(DUNGEON_ID.DI_EVENT_MISSION_IMPOSSIBLE_STAGE1_NORMAL)
g_pButton_Event_Dummy01:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Event_Dummy01:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Event_Dummy01:SetNormalTex("DLG_Common_Button02_NEW.tga", "TREE_NORMAL")
IconAni(g_pButton_Event_Dummy01, "DLG_Common_Button02_NEW.tga", "TREE_OVER")
g_pButton_Event_Dummy01:SetDownTex("DLG_Common_Button02_NEW.tga", "TREE_OVER")
g_pButton_Event_Dummy01:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(440, 300),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(448, 290),
	LEFT_TOP_JP = D3DXVECTOR2(430, 305)
})
g_pButton_Event_Dummy01:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(440, 300),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(448, 290),
	LEFT_TOP_JP = D3DXVECTOR2(430, 305)
})
g_pButton_Event_Dummy01:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(442, 302),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(450, 292),
	LEFT_TOP_JP = D3DXVECTOR2(432, 307)
})
g_pButton_Event_Dummy01:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy01:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy01:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Event_Dummy02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Event_Dummy02)
g_pButton_Event_Dummy02:SetName("Dungeon")
g_pButton_Event_Dummy02:AddDummyInt(DUNGEON_ID.DI_EVENT_MISSION_IMPOSSIBLE_STAGE2_NORMAL)
g_pButton_Event_Dummy02:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Event_Dummy02:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Event_Dummy02:SetNormalTex("DLG_Common_Button02_NEW.tga", "TREE_NORMAL")
IconAni(g_pButton_Event_Dummy02, "DLG_Common_Button02_NEW.tga", "TREE_OVER")
g_pButton_Event_Dummy02:SetDownTex("DLG_Common_Button02_NEW.tga", "TREE_OVER")
g_pButton_Event_Dummy02:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(20, 150)
})
g_pButton_Event_Dummy02:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(20, 150)
})
g_pButton_Event_Dummy02:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(22, 152)
})
g_pButton_Event_Dummy02:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy02:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy02:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("하멜_타이틀 모음")

g_pPictureTitleBG_Title_01 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_01)
g_pPictureTitleBG_Title_01:SetTex("DLG_Localmap_Title_01.tga", "Title_Harmel_01")
g_pPictureTitleBG_Title_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(172, 276),
	LEFT_TOP_BR = D3DXVECTOR2(152, 276)
})

g_pPictureTitleBG_Title_02 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_02)
g_pPictureTitleBG_Title_02:SetTex("DLG_Localmap_Title_01.tga", "Title_Harmel_02")
g_pPictureTitleBG_Title_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(290, 285),
	LEFT_TOP_IT = D3DXVECTOR2(290, 273)
})

g_pPictureTitleBG_Title_03 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_03)
g_pPictureTitleBG_Title_03:SetTex("DLG_Localmap_Title_01.tga", "Title_Harmel_03")
g_pPictureTitleBG_Title_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(402, 276)
})

g_pPictureTitleBG_Title_04 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_04)
g_pPictureTitleBG_Title_04:SetTex("DLG_Localmap_Title_01.tga", "Title_Harmel_04")
g_pPictureTitleBG_Title_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(460, 202),
	LEFT_TOP_EU = D3DXVECTOR2(460, 191),
	LEFT_TOP_BR = D3DXVECTOR2(410, 202),
	LEFT_TOP_TH = D3DXVECTOR2(440, 202),
	LEFT_TOP_US = D3DXVECTOR2(435, 202)
})

g_pPictureTitleBG_Title_05 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_05)
g_pPictureTitleBG_Title_05:SetTex("DLG_Localmap_Title_01.tga", "Title_Harmel_05")
g_pPictureTitleBG_Title_05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(363, 134),
	LEFT_TOP_DE = D3DXVECTOR2(363, 123),
	LEFT_TOP_EN = D3DXVECTOR2(363, 136),
	LEFT_TOP_ES = D3DXVECTOR2(363, 136),
	LEFT_TOP_FR = D3DXVECTOR2(363, 123),
	LEFT_TOP_IT = D3DXVECTOR2(363, 123),
	LEFT_TOP_PL = D3DXVECTOR2(363, 123)
})

g_pPictureTitleBG_Title_06 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_06)
g_pPictureTitleBG_Title_06:SetTex("DLG_Localmap_Title_01.tga", "Title_Harmel_06")
g_pPictureTitleBG_Title_06:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(249, 114),
	LEFT_TOP_EU = D3DXVECTOR2(249, 103),
	LEFT_TOP_BR = D3DXVECTOR2(249, 106),
	LEFT_TOP_IT = D3DXVECTOR2(249, 92)
})

g_pPictureTitleBG_Title_07 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_07)
g_pPictureTitleBG_Title_07:SetTex("DLG_Localmap_Title_01.tga", "Title_Harmel_07")
g_pPictureTitleBG_Title_07:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(152, 112),
	LEFT_TOP_EU = D3DXVECTOR2(152, 101),
	LEFT_TOP_IT = D3DXVECTOR2(152, 87)
})

g_pStatic_Event_01 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Event_01)
g_pStatic_Event_01:AddDummyInt(DUNGEON_ID.DI_EVENT_MISSION_IMPOSSIBLE_STAGE1_NORMAL)
g_pStatic_Event_01:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_EVENT_DUNGEON_TITLE)
g_pStatic_Event_01:AddDummyInt(DUNGEON_ID.DI_NONE)

g_pPictureTitleBG_Title_Event_01 = g_pUIDialog:CreatePicture()

g_pStatic_Event_01:AddPicture(g_pPictureTitleBG_Title_Event_01)
g_pPictureTitleBG_Title_Event_01:SetTex("DLG_Localmap_Title_01.tga", "Title_Holloween")
g_pPictureTitleBG_Title_Event_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(458, 355)
})

g_pStatic_Event_02 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Event_02)
g_pStatic_Event_02:AddDummyInt(DUNGEON_ID.DI_EVENT_MISSION_IMPOSSIBLE_STAGE2_NORMAL)
g_pStatic_Event_02:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_EVENT_DUNGEON_TITLE)
g_pStatic_Event_02:AddDummyInt(DUNGEON_ID.DI_NONE)

g_pPictureTitleBG_Title_Event_02 = g_pUIDialog:CreatePicture()

g_pStatic_Event_02:AddPicture(g_pPictureTitleBG_Title_Event_02)
g_pPictureTitleBG_Title_Event_02:SetTex("DLG_Localmap_Title_01.tga", "Title_Holloween")
g_pPictureTitleBG_Title_Event_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(40, 205)
})

-- [June UI parity Phase 3] omitted: g_pButton_TitleBG (June-only Hamel secret dungeon: March has the DI_HAMEL_SECRET_HELL enum but no DungeonData for it, and its Title_Harmel_Secret key holds different art in March).

g_pButton_STAR8 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR8)
g_pButton_STAR8:SetNameStrID(STR_ID_27045)

g_pPictureSTAR8_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_StarBG)
g_pPictureSTAR8_StarBG:SetTex("DLG_Common_Button00_NEW.TGA", "Star_Bg")
g_pPictureSTAR8_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(38, 122)
})
g_pButton_STAR8:SetShow(false)

g_pButton_STAR2 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR2)
g_pButton_STAR2:SetNameStrID(STR_ID_4979)

g_pPictureSTAR2_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR2:AddPicture(g_pPictureSTAR2_StarBG)
g_pPictureSTAR2_StarBG:SetTex("DLG_Common_Button00_NEW.tga", "Star_Bg")
g_pPictureSTAR2_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(490, 357)
})
g_pButton_STAR2:SetShow(false)

g_pButton_STAR2 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR2)
g_pButton_STAR2:SetNameStrID(STR_ID_11495)

g_pPictureSTAR2_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR2:AddPicture(g_pPictureSTAR2_StarBG)
g_pPictureSTAR2_StarBG:SetTex("DLG_Common_Button00_NEW.tga", "Star_Bg")
g_pPictureSTAR2_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(44, 114)
})
g_pButton_STAR2:SetShow(false)

g_pButton_STAR2 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR2)
g_pButton_STAR2:SetNameStrID(STR_ID_10343)

g_pPictureSTAR2_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR2:AddPicture(g_pPictureSTAR2_StarBG)
g_pPictureSTAR2_StarBG:SetTex("DLG_Common_Button00_NEW.tga", "Star_Bg")
g_pPictureSTAR2_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(26, 347)
})
g_pButton_STAR2:SetShow(false)
