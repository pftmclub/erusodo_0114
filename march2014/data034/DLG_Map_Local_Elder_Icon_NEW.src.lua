--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Elder_Icon_NEW.lua (live data034 copy, 9499 B), adopted wholesale.
-- June dropped the clear-difficulty stars, the Henir button (moved to the Special map)
-- and switched the secret dungeon to its _HELL ID with a SecretDungeonDesc lock button;
-- all of that is kept. Each button's third AddDummyInt is the June Hero dungeon ID,
-- which March has no enum for, so it reads 0 (DI_NONE, as June's own Ruben file uses);
-- CX2LocalMapUI only reads it in hero mode. Controls naming June-only content are
-- omitted below with a note where each stood.
--}}

g_pButton_Velder_Druid_Forest = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Velder_Druid_Forest)
g_pButton_Velder_Druid_Forest:SetName("Dungeon")
g_pButton_Velder_Druid_Forest:AddDummyInt(DUNGEON_ID.DI_ELDER_DRUID_FOREST_NORMAL)
g_pButton_Velder_Druid_Forest:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Velder_Druid_Forest:AddDummyInt(DUNGEON_ID.DI_ELDER_DRUID_FOREST_HERO_NORMAL)
g_pButton_Velder_Druid_Forest:AddDummyString("DruidForest_Star")
g_pButton_Velder_Druid_Forest:SetNormalTex("DLG_Common_Button00_NEW.tga", "Druid_Forest")
g_pButton_Velder_Druid_Forest:SetOverTex("DLG_Common_Button00_NEW.tga", "Druid_Forest_Over")
IconAni(g_pButton_Velder_Druid_Forest, "DLG_Common_Button00_NEW.tga", "Druid_Forest_Over")
g_pButton_Velder_Druid_Forest:SetDownTex("DLG_Common_Button00_NEW.tga", "Druid_Forest_Over")
g_pButton_Velder_Druid_Forest:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(78, 220)
})
g_pButton_Velder_Druid_Forest:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(72, 215)
})
g_pButton_Velder_Druid_Forest:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(74, 217)
})
g_pButton_Velder_Druid_Forest:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Velder_Druid_Forest:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Velder_Druid_Forest:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Velder_East_Benders_Cave = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Velder_East_Benders_Cave)
g_pButton_Velder_East_Benders_Cave:SetName("Dungeon")
g_pButton_Velder_East_Benders_Cave:AddDummyInt(DUNGEON_ID.DI_ELDER_BENDERS_CAVE_NORMAL)
g_pButton_Velder_East_Benders_Cave:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Velder_East_Benders_Cave:AddDummyInt(DUNGEON_ID.DI_ELDER_BENDERS_CAVE_HERO_NORMAL)
g_pButton_Velder_East_Benders_Cave:SetNormalTex("DLG_Common_Button00_NEW.tga", "Benders_Cave")
g_pButton_Velder_East_Benders_Cave:SetOverTex("DLG_Common_Button00_NEW.tga", "Benders_Cave_Over")
IconAni(g_pButton_Velder_East_Benders_Cave, "DLG_Common_Button00_NEW.tga", "Benders_Cave_Over")
g_pButton_Velder_East_Benders_Cave:SetDownTex("DLG_Common_Button00_NEW.tga", "Benders_Cave_Over")
g_pButton_Velder_East_Benders_Cave:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(172, 178)
})
g_pButton_Velder_East_Benders_Cave:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(167, 173)
})
g_pButton_Velder_East_Benders_Cave:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(169, 175)
})
g_pButton_Velder_East_Benders_Cave:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Velder_East_Benders_Cave:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Velder_East_Benders_Cave:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Velder_East_SubWay = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Velder_East_SubWay)
g_pButton_Velder_East_SubWay:SetName("Dungeon")
g_pButton_Velder_East_SubWay:AddDummyString("SubWay_Star")
g_pButton_Velder_East_SubWay:AddDummyInt(DUNGEON_ID.DI_ELDER_BELOW_PASSAGE_NORMAL)
g_pButton_Velder_East_SubWay:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Velder_East_SubWay:AddDummyInt(DUNGEON_ID.DI_ELDER_BELOW_PASSAGE_HERO_NORMAL)
g_pButton_Velder_East_SubWay:SetNormalTex("DLG_Common_Button00_NEW.tga", "SubWay")
g_pButton_Velder_East_SubWay:SetOverTex("DLG_Common_Button00_NEW.tga", "SubWay_Over")
IconAni(g_pButton_Velder_East_SubWay, "DLG_Common_Button00_NEW.tga", "SubWay_Over")
g_pButton_Velder_East_SubWay:SetDownTex("DLG_Common_Button00_NEW.tga", "SubWay_Over")
g_pButton_Velder_East_SubWay:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(273, 260)
})
g_pButton_Velder_East_SubWay:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(267, 255)
})
g_pButton_Velder_East_SubWay:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(269, 257)
})
g_pButton_Velder_East_SubWay:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Velder_East_SubWay:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Velder_East_SubWay:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Velder_East_Wally_Castle2 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Velder_East_Wally_Castle2)
g_pButton_Velder_East_Wally_Castle2:SetName("Dungeon")
g_pButton_Velder_East_Wally_Castle2:AddDummyInt(DUNGEON_ID.DI_ELDER_WALLY_CASTLE_ROOF_NORMAL)
g_pButton_Velder_East_Wally_Castle2:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Velder_East_Wally_Castle2:AddDummyInt(DUNGEON_ID.DI_ELDER_WALLY_CASTLE_ROOF_HERO_NORMAL)
g_pButton_Velder_East_Wally_Castle2:AddDummyString("Wally_Castle_OutLine_Star")
g_pButton_Velder_East_Wally_Castle2:SetNormalTex("DLG_Common_Button00_NEW.tga", "Wally_Castle2")
g_pButton_Velder_East_Wally_Castle2:SetOverTex("DLG_Common_Button00_NEW.tga", "Wally_Catsle2_Over")
IconAni(g_pButton_Velder_East_Wally_Castle2, "DLG_Common_Button00_NEW.tga", "Wally_Catsle2_Over")
g_pButton_Velder_East_Wally_Castle2:SetDownTex("DLG_Common_Button00_NEW.tga", "Wally_Catsle2_Over")
g_pButton_Velder_East_Wally_Castle2:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(455, 230)
})
g_pButton_Velder_East_Wally_Castle2:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(451, 225)
})
g_pButton_Velder_East_Wally_Castle2:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(453, 227)
})
g_pButton_Velder_East_Wally_Castle2:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Velder_East_Wally_Castle2:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Velder_East_Wally_Castle2:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Velder_East_Wally_Castle1 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Velder_East_Wally_Castle1)
g_pButton_Velder_East_Wally_Castle1:SetName("Dungeon")
g_pButton_Velder_East_Wally_Castle1:AddDummyInt(DUNGEON_ID.DI_ELDER_WALLY_CASTLE_CENTER_NORMAL)
g_pButton_Velder_East_Wally_Castle1:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Velder_East_Wally_Castle1:AddDummyInt(DUNGEON_ID.DI_ELDER_WALLY_CASTLE_CENTER_HERO_NORMAL)
g_pButton_Velder_East_Wally_Castle1:AddDummyString("Wally_Castle_Star")
g_pButton_Velder_East_Wally_Castle1:SetNormalTex("DLG_Common_Button00_NEW.tga", "Wally_Castle1")
g_pButton_Velder_East_Wally_Castle1:SetOverTex("DLG_Common_Button00_NEW.tga", "Wally_Catsle1_Over")
IconAni(g_pButton_Velder_East_Wally_Castle1, "DLG_Common_Button00_NEW.tga", "Wally_Catsle1_Over")
g_pButton_Velder_East_Wally_Castle1:SetDownTex("DLG_Common_Button00_NEW.tga", "Wally_Catsle1_Over")
g_pButton_Velder_East_Wally_Castle1:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(340, 94)
})
g_pButton_Velder_East_Wally_Castle1:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(335, 89)
})
g_pButton_Velder_East_Wally_Castle1:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(337, 91)
})
g_pButton_Velder_East_Wally_Castle1:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Velder_East_Wally_Castle1:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Velder_East_Wally_Castle1:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Velder_East_Secret_Dungeon = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Velder_East_Secret_Dungeon)
g_pButton_Velder_East_Secret_Dungeon:SetName("Dungeon")
g_pButton_Velder_East_Secret_Dungeon:AddDummyInt(DUNGEON_ID.DI_ELDER_SECRET_HELL)
g_pButton_Velder_East_Secret_Dungeon:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Velder_East_Secret_Dungeon:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Velder_East_Secret_Dungeon:SetNormalTex("DLG_Common_Button03_NEW.tga", "ELDER_SECRET_DUNGEON_NORMAL")
g_pButton_Velder_East_Secret_Dungeon:SetOverTex("DLG_Common_Button03_NEW.tga", "ELDER_SECRET_DUNGEON_OVER")
IconAni(g_pButton_Velder_East_Secret_Dungeon, "DLG_Common_Button03_NEW.tga", "ELDER_SECRET_DUNGEON_OVER")
g_pButton_Velder_East_Secret_Dungeon:SetDownTex("DLG_Common_Button03_NEW.tga", "ELDER_SECRET_DUNGEON_OVER")
g_pButton_Velder_East_Secret_Dungeon:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(333, 203)
})
g_pButton_Velder_East_Secret_Dungeon:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(333, 203)
})
g_pButton_Velder_East_Secret_Dungeon:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(335, 205)
})
g_pButton_Velder_East_Secret_Dungeon:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Velder_East_Secret_Dungeon:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Velder_East_Secret_Dungeon:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Velder_East_Secret_Dungeon_Desc = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Velder_East_Secret_Dungeon_Desc)
g_pButton_Velder_East_Secret_Dungeon_Desc:SetName("SecretDungeonDesc")
g_pButton_Velder_East_Secret_Dungeon_Desc:AddDummyInt(DUNGEON_ID.DI_ELDER_SECRET_HELL)
g_pButton_Velder_East_Secret_Dungeon_Desc:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Velder_East_Secret_Dungeon_Desc:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Velder_East_Secret_Dungeon_Desc:SetNormalTex("DLG_Common_Button05_NEW.tga", "ELDER_SECRET_DUNGEON_LOCK")
g_pButton_Velder_East_Secret_Dungeon_Desc:SetOverTex("DLG_Common_Button05_NEW.tga", "ELDER_SECRET_DUNGEON_LOCK")
g_pButton_Velder_East_Secret_Dungeon_Desc:SetDownTex("DLG_Common_Button05_NEW.tga", "ELDER_SECRET_DUNGEON_LOCK")
g_pButton_Velder_East_Secret_Dungeon_Desc:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(333, 203)
})
g_pButton_Velder_East_Secret_Dungeon_Desc:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(333, 203)
})
g_pButton_Velder_East_Secret_Dungeon_Desc:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(333, 203)
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
	LEFT_TOP = D3DXVECTOR2(39, 34)
})
g_pButton_Event_Dummy01:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(37, 34)
})
g_pButton_Event_Dummy01:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(39, 36)
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
	LEFT_TOP = D3DXVECTOR2(150, 34)
})
g_pButton_Event_Dummy02:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(150, 34)
})
g_pButton_Event_Dummy02:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(152, 36)
})
g_pButton_Event_Dummy02:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy02:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy02:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("엘더_타이틀 모음")

g_pPictureTitleBG_Title_01 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_01)
g_pPictureTitleBG_Title_01:SetTex("DLG_Localmap_Title_01.tga", "Title_Elder_01")
g_pPictureTitleBG_Title_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(99, 270)
})

g_pPictureTitleBG_Title_02 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_02)
g_pPictureTitleBG_Title_02:SetTex("DLG_Localmap_Title_01.tga", "Title_Elder_02")
g_pPictureTitleBG_Title_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(188, 228)
})

g_pPictureTitleBG_Title_03 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_03)
g_pPictureTitleBG_Title_03:SetTex("DLG_Localmap_Title_01.tga", "Title_Elder_03")
g_pPictureTitleBG_Title_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(291, 310)
})

g_pPictureTitleBG_Title_04 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_04)
g_pPictureTitleBG_Title_04:SetTex("DLG_Localmap_Title_01.tga", "Title_Elder_04")
g_pPictureTitleBG_Title_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(456, 285)
})

g_pPictureTitleBG_Title_05 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_05)
g_pPictureTitleBG_Title_05:SetTex("DLG_Localmap_Title_01.tga", "Title_Elder_05")
g_pPictureTitleBG_Title_05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(358, 158)
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
	LEFT_TOP = D3DXVECTOR2(37, 89)
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
	LEFT_TOP = D3DXVECTOR2(170, 89)
})

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("TITLE_SECRET")
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_ELDER_SECRET_HELL)
g_pButton_TitleBG:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_NONE)

g_pPictureTitleBG_Title_Secret = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_Secret)
g_pPictureTitleBG_Title_Secret:SetTex("DLG_Localmap_Title_01.tga", "Title_Elder_Secret")
g_pPictureTitleBG_Title_Secret:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(355, 267)
})

g_pButton_STAR8 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR8)
g_pButton_STAR8:SetNameStrID(STR_ID_3722)

g_pPictureSTAR8_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_StarBG)
g_pPictureSTAR8_StarBG:SetTex("DLG_Common_Button00_NEW.TGA", "Star_Bg")
g_pPictureSTAR8_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(469, 84)
})

g_pPictureSTAR8_Star0 = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_Star0)
g_pPictureSTAR8_Star0:SetTex("DLG_Common_Button00_NEW.TGA", "Star_1")
g_pPictureSTAR8_Star0:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(489, 104),
	COLOR = D3DXCOLOR(0, 1, 0, 1)
})

g_pPictureSTAR8_Star1 = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_Star1)
g_pPictureSTAR8_Star1:SetTex("DLG_Common_Button00_NEW.TGA", "Star_2")
g_pPictureSTAR8_Star1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(503, 104),
	COLOR = D3DXCOLOR(1, 1, 0, 1)
})

g_pPictureSTAR8_Star2 = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_Star2)
g_pPictureSTAR8_Star2:SetTex("DLG_Common_Button00_NEW.TGA", "Star_3")
g_pPictureSTAR8_Star2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(516, 104),
	COLOR = D3DXCOLOR(1, 0, 0, 1)
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
g_pButton_STAR2:SetNameStrID(STR_ID_3146)

g_pPictureSTAR2_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR2:AddPicture(g_pPictureSTAR2_StarBG)
g_pPictureSTAR2_StarBG:SetTex("DLG_Common_Button00_NEW.tga", "Star_Bg")
g_pPictureSTAR2_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(347, 284)
})
g_pButton_STAR2:SetShow(false)
g_pButton_STAR2:SetShow(false)

g_pButton_STAR2 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR2)
g_pButton_STAR2:SetNameStrID(STR_ID_11495)

g_pPictureSTAR2_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR2:AddPicture(g_pPictureSTAR2_StarBG)
g_pPictureSTAR2_StarBG:SetTex("DLG_Common_Button00_NEW.tga", "Star_Bg")
g_pPictureSTAR2_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(42, 357)
})
g_pButton_STAR2:SetShow(false)
