--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Velder_Icon_NEW.lua (live data217 copy, 8959 B), adopted wholesale.
-- June dropped the clear-difficulty stars, the Henir button (moved to the Special map)
-- and switched the secret dungeon to its _HELL ID with a SecretDungeonDesc lock button;
-- all of that is kept. Each button's third AddDummyInt is the June Hero dungeon ID,
-- which March has no enum for, so it reads 0 (DI_NONE, as June's own Ruben file uses);
-- CX2LocalMapUI only reads it in hero mode. Controls naming June-only content are
-- omitted below with a note where each stood.
--}}

g_pButtonVelder_Dwelling = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonVelder_Dwelling)
g_pButtonVelder_Dwelling:SetName("THIRD_DWELLING")
g_pButtonVelder_Dwelling:AddDummyInt(DUNGEON_ID.DI_VELDER_THIRD_DWELLING_NORMAL)
g_pButtonVelder_Dwelling:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonVelder_Dwelling:AddDummyInt(DUNGEON_ID.DI_VELDER_THIRD_DWELLING_HERO_NORMAL)
g_pButtonVelder_Dwelling:SetNormalTex("DLG_Common_Button01_NEW.tga", "DWELLING_NORMAL")
g_pButtonVelder_Dwelling:SetOverTex("DLG_Common_Button01_NEW.tga", "DWELLING_OVER")
IconAni(g_pButtonVelder_Dwelling, "DLG_Common_Button01_NEW.tga", "DWELLING_OVER")
g_pButtonVelder_Dwelling:SetDownTex("DLG_Common_Button01_NEW.tga", "DWELLING_OVER")
g_pButtonVelder_Dwelling:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(119, 184)
})
g_pButtonVelder_Dwelling:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(109, 174)
})
g_pButtonVelder_Dwelling:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(112, 176)
})
g_pButtonVelder_Dwelling:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonVelder_Dwelling:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonVelder_Dwelling:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonVELDER_BRIDGE_HOPE = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonVELDER_BRIDGE_HOPE)
g_pButtonVELDER_BRIDGE_HOPE:SetName("BRIDGE_HOPE")
g_pButtonVELDER_BRIDGE_HOPE:AddDummyInt(DUNGEON_ID.DI_VELDER_BRIDGE_HOPE_NORMAL)
g_pButtonVELDER_BRIDGE_HOPE:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonVELDER_BRIDGE_HOPE:AddDummyInt(DUNGEON_ID.DI_VELDER_BRIDGE_HOPE_HERO_NORMAL)
g_pButtonVELDER_BRIDGE_HOPE:SetNormalTex("DLG_Common_Button01_NEW.tga", "Bridge_Normal")
g_pButtonVELDER_BRIDGE_HOPE:SetOverTex("DLG_Common_Button01_NEW.tga", "Bridge_Over")
IconAni(g_pButtonVELDER_BRIDGE_HOPE, "DLG_Common_Button01_NEW.tga", "Bridge_Over")
g_pButtonVELDER_BRIDGE_HOPE:SetDownTex("DLG_Common_Button01_NEW.tga", "Bridge_Over")
g_pButtonVELDER_BRIDGE_HOPE:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(207, 106)
})
g_pButtonVELDER_BRIDGE_HOPE:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(200, 97)
})
g_pButtonVELDER_BRIDGE_HOPE:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(202, 98)
})
g_pButtonVELDER_BRIDGE_HOPE:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonVELDER_BRIDGE_HOPE:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonVELDER_BRIDGE_HOPE:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonVELDER_ROAD_PALACE = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonVELDER_ROAD_PALACE)
g_pButtonVELDER_ROAD_PALACE:SetName("PALACE_ENTRANCE")
g_pButtonVELDER_ROAD_PALACE:AddDummyInt(DUNGEON_ID.DI_VELDER_PALACE_ENTRANCE_NORMAL)
g_pButtonVELDER_ROAD_PALACE:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonVELDER_ROAD_PALACE:AddDummyInt(DUNGEON_ID.DI_VELDER_PALACE_ENTRANCE_HERO_NORMAL)
g_pButtonVELDER_ROAD_PALACE:SetNormalTex("DLG_Common_Button01_NEW.tga", "ROAD_PALACE_Normal")
IconAni(g_pButtonVELDER_ROAD_PALACE, "DLG_Common_Button01_NEW.tga", "ROAD_PALACE_Over")
g_pButtonVELDER_ROAD_PALACE:SetDownTex("DLG_Common_Button01_NEW.tga", "ROAD_PALACE_Over")
g_pButtonVELDER_ROAD_PALACE:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(332, 80)
})
g_pButtonVELDER_ROAD_PALACE:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(325, 71)
})
g_pButtonVELDER_ROAD_PALACE:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(327, 72)
})
g_pButtonVELDER_ROAD_PALACE:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonVELDER_ROAD_PALACE:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonVELDER_ROAD_PALACE:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonVELDER_BRIDGE_BUNNING = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonVELDER_BRIDGE_BUNNING)
g_pButtonVELDER_BRIDGE_BUNNING:SetName("BRIDGE_BUNNING")
g_pButtonVELDER_BRIDGE_BUNNING:AddDummyInt(DUNGEON_ID.DI_VELDER_BRIDGE_BUNNING_NORMAL)
g_pButtonVELDER_BRIDGE_BUNNING:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonVELDER_BRIDGE_BUNNING:AddDummyInt(DUNGEON_ID.DI_VELDER_BRIDGE_BUNNING_HERO_NORMAL)
g_pButtonVELDER_BRIDGE_BUNNING:SetNormalTex("DLG_Common_Button01_NEW.tga", "BRIDGE_BUNNING_Normal")
IconAni(g_pButtonVELDER_BRIDGE_BUNNING, "DLG_Common_Button01_NEW.tga", "BRIDGE_BUNNING_Over")
g_pButtonVELDER_BRIDGE_BUNNING:SetDownTex("DLG_Common_Button01_NEW.tga", "BRIDGE_BUNNING_Over")
g_pButtonVELDER_BRIDGE_BUNNING:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(421, 145)
})
g_pButtonVELDER_BRIDGE_BUNNING:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(414, 136)
})
g_pButtonVELDER_BRIDGE_BUNNING:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(416, 137)
})
g_pButtonVELDER_BRIDGE_BUNNING:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonVELDER_BRIDGE_BUNNING:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonVELDER_BRIDGE_BUNNING:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonVELDER_MARKET = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonVELDER_MARKET)
g_pButtonVELDER_MARKET:SetName("MARKET")
g_pButtonVELDER_MARKET:AddDummyInt(DUNGEON_ID.DI_VELDER_MARKET_NORMAL)
g_pButtonVELDER_MARKET:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonVELDER_MARKET:AddDummyInt(DUNGEON_ID.DI_VELDER_MARKET_HERO_NORMAL)
g_pButtonVELDER_MARKET:SetNormalTex("DLG_Common_Button01_NEW.tga", "MARKET_Normal")
IconAni(g_pButtonVELDER_MARKET, "DLG_Common_Button01_NEW.tga", "MARKET_Over")
g_pButtonVELDER_MARKET:SetDownTex("DLG_Common_Button01_NEW.tga", "MARKET_Over")
g_pButtonVELDER_MARKET:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(366, 218)
})
g_pButtonVELDER_MARKET:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(360, 209)
})
g_pButtonVELDER_MARKET:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(362, 210)
})
g_pButtonVELDER_MARKET:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonVELDER_MARKET:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonVELDER_MARKET:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonVELDER_GATE = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonVELDER_GATE)
g_pButtonVELDER_GATE:SetName("GATE")
g_pButtonVELDER_GATE:AddDummyInt(DUNGEON_ID.DI_VELDER_GATE_NORMAL)
g_pButtonVELDER_GATE:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonVELDER_GATE:AddDummyInt(DUNGEON_ID.DI_VELDER_GATE_HERO_NORMAL)
g_pButtonVELDER_GATE:SetNormalTex("DLG_Common_Button01_NEW.tga", "GATE_Normal")
IconAni(g_pButtonVELDER_GATE, "DLG_Common_Button01_NEW.tga", "GATE_Over")
g_pButtonVELDER_GATE:SetDownTex("DLG_Common_Button01_NEW.tga", "GATE_Over")
g_pButtonVELDER_GATE:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(245, 256)
})
g_pButtonVELDER_GATE:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(239, 247)
})
g_pButtonVELDER_GATE:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(241, 248)
})
g_pButtonVELDER_GATE:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonVELDER_GATE:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonVELDER_GATE:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_velder_Secret_Dungeon = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_velder_Secret_Dungeon)
g_pButton_velder_Secret_Dungeon:SetName("VelderSecret")
g_pButton_velder_Secret_Dungeon:AddDummyInt(DUNGEON_ID.DI_VELDER_SECRET_HELL)
g_pButton_velder_Secret_Dungeon:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_velder_Secret_Dungeon:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_velder_Secret_Dungeon:SetNormalTex("DLG_Common_Button01_NEW.tga", "SCRET_NORMAL")
IconAni(g_pButton_velder_Secret_Dungeon, "DLG_Common_Button01_NEW.tga", "SCRET_OVER")
g_pButton_velder_Secret_Dungeon:SetDownTex("DLG_Common_Button01_NEW.tga", "SCRET_OVER")
g_pButton_velder_Secret_Dungeon:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(35, 25)
})
g_pButton_velder_Secret_Dungeon:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(29, 19)
})
g_pButton_velder_Secret_Dungeon:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(31, 20)
})
g_pButton_velder_Secret_Dungeon:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_velder_Secret_Dungeon:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_velder_Secret_Dungeon:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_velder_Secret_Dungeon_Desc = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_velder_Secret_Dungeon_Desc)
g_pButton_velder_Secret_Dungeon_Desc:SetName("SecretDungeonDesc")
g_pButton_velder_Secret_Dungeon_Desc:AddDummyInt(DUNGEON_ID.DI_VELDER_SECRET_HELL)
g_pButton_velder_Secret_Dungeon_Desc:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_velder_Secret_Dungeon_Desc:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_velder_Secret_Dungeon_Desc:SetNormalTex("DLG_Common_Button05_NEW.tga", "SCRET_LOCK")
g_pButton_velder_Secret_Dungeon_Desc:SetOverTex("DLG_Common_Button05_NEW.tga", "SCRET_LOCK")
g_pButton_velder_Secret_Dungeon_Desc:SetDownTex("DLG_Common_Button05_NEW.tga", "SCRET_LOCK")
g_pButton_velder_Secret_Dungeon_Desc:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(35, 25)
})
g_pButton_velder_Secret_Dungeon_Desc:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(35, 25)
})
g_pButton_velder_Secret_Dungeon_Desc:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(35, 25)
})

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
	LEFT_TOP = D3DXVECTOR2(440, 310)
})
g_pButton_Event_Dummy01:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(440, 310)
})
g_pButton_Event_Dummy01:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(442, 312)
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
	LEFT_TOP = D3DXVECTOR2(140, 10)
})
g_pButton_Event_Dummy02:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(140, 10)
})
g_pButton_Event_Dummy02:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(142, 12)
})
g_pButton_Event_Dummy02:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy02:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy02:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("벨더_타이틀 모음")

g_pPictureTitleBG_Title_01 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_01)
g_pPictureTitleBG_Title_01:SetTex("DLG_Localmap_Title_01.tga", "Title_Velder_01")
g_pPictureTitleBG_Title_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(137, 241)
})

g_pPictureTitleBG_Title_02 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_02)
g_pPictureTitleBG_Title_02:SetTex("DLG_Localmap_Title_01.tga", "Title_Velder_02")
g_pPictureTitleBG_Title_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(231, 169)
})

g_pPictureTitleBG_Title_03 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_03)
g_pPictureTitleBG_Title_03:SetTex("DLG_Localmap_Title_01.tga", "Title_Velder_03")
g_pPictureTitleBG_Title_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(350, 141)
})

g_pPictureTitleBG_Title_04 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_04)
g_pPictureTitleBG_Title_04:SetTex("DLG_Localmap_Title_01.tga", "Title_Velder_04")
g_pPictureTitleBG_Title_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(445, 207),
	LEFT_TOP_BR = D3DXVECTOR2(405, 207)
})

g_pPictureTitleBG_Title_05 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_05)
g_pPictureTitleBG_Title_05:SetTex("DLG_Localmap_Title_01.tga", "Title_Velder_05")
g_pPictureTitleBG_Title_05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(386, 287)
})

g_pPictureTitleBG_Title_06 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_06)
g_pPictureTitleBG_Title_06:SetTex("DLG_Localmap_Title_01.tga", "Title_Velder_06")
g_pPictureTitleBG_Title_06:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(264, 318)
})

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("TITLE_SECRET")
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_VELDER_SECRET_HELL)
g_pButton_TitleBG:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_NONE)

g_pPictureTitleBG_Title_Secret = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_Secret)
g_pPictureTitleBG_Title_Secret:SetTex("DLG_Localmap_Title_01.tga", "Title_Velder_Secret")
g_pPictureTitleBG_Title_Secret:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(56, 97),
	LEFT_TOP_BR = D3DXVECTOR2(26, 97)
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
	LEFT_TOP = D3DXVECTOR2(460, 365)
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
	LEFT_TOP = D3DXVECTOR2(160, 65)
})

g_pButton_STAR2 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR2)
g_pButton_STAR2:SetNameStrID(STR_ID_13861)

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
g_pButton_STAR2:SetNameStrID(STR_ID_10342)

g_pPictureSTAR2_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR2:AddPicture(g_pPictureSTAR2_StarBG)
g_pPictureSTAR2_StarBG:SetTex("DLG_Common_Button00_NEW.tga", "Star_Bg")
g_pPictureSTAR2_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(26, 347)
})
g_pButton_STAR2:SetShow(false)
