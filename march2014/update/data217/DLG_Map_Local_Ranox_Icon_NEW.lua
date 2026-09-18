g_pButtonBurndedForest = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBurndedForest)
g_pButtonBurndedForest:SetName("Burnded_Forest")
g_pButtonBurndedForest:AddDummyInt(DUNGEON_ID.DI_RANOX_BURNDEDFOREST_NORMAL)
g_pButtonBurndedForest:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonBurndedForest:SetNormalTex("DLG_Common_Button03_NEW.tga", "Ranox_Burned_Forest_N")
g_pButtonBurndedForest:SetOverTex("DLG_Common_Button03_NEW.tga", "Ranox_Burned_Forest_O")
IconAni(g_pButtonBurndedForest, "DLG_Common_Button03_NEW.tga", "Ranox_Burned_Forest_O")
g_pButtonBurndedForest:SetDownTex("DLG_Common_Button03_NEW.tga", "Ranox_Burned_Forest_O")
g_pButtonBurndedForest:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(271, 268)
})
g_pButtonBurndedForest:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(271, 268)
})
g_pButtonBurndedForest:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(273, 270)
})
g_pButtonBurndedForest:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonBurndedForest:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonBurndedForest:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonAshesVillage = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonAshesVillage)
g_pButtonAshesVillage:SetName("Ashes_Village")
g_pButtonAshesVillage:AddDummyInt(DUNGEON_ID.DI_RANOX_ASHESVILLAGE_NORMAL)
g_pButtonAshesVillage:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonAshesVillage:SetNormalTex("DLG_Common_Button03_NEW.tga", "Ranox_Ashes_Village_N")
g_pButtonAshesVillage:SetOverTex("DLG_Common_Button03_NEW.tga", "Ranox_Ashes_Village_O")
IconAni(g_pButtonAshesVillage, "DLG_Common_Button03_NEW.tga", "Ranox_Ashes_Village_O")
g_pButtonAshesVillage:SetDownTex("DLG_Common_Button03_NEW.tga", "Ranox_Ashes_Village_O")
g_pButtonAshesVillage:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(474, 185)
})
g_pButtonAshesVillage:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(474, 185)
})
g_pButtonAshesVillage:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(476, 187)
})
g_pButtonAshesVillage:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonAshesVillage:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonAshesVillage:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonGeyser = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonGeyser)
g_pButtonGeyser:SetName("Geyser")
g_pButtonGeyser:AddDummyInt(DUNGEON_ID.DI_RANOX_GEYSER_NORMAL)
g_pButtonGeyser:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonGeyser:SetNormalTex("DLG_Common_Button03_NEW.tga", "Ranox_Geyser_N")
g_pButtonGeyser:SetOverTex("DLG_Common_Button03_NEW.tga", "Ranox_Geyser_O")
IconAni(g_pButtonGeyser, "DLG_Common_Button03_NEW.tga", "Ranox_Geyser_O")
g_pButtonGeyser:SetDownTex("DLG_Common_Button03_NEW.tga", "Ranox_Geyser_O")
g_pButtonGeyser:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(322, 169)
})
g_pButtonGeyser:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(322, 169)
})
g_pButtonGeyser:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(324, 171)
})
g_pButtonGeyser:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonGeyser:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonGeyser:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonUnderGarden = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonUnderGarden)
g_pButtonUnderGarden:SetName("Gateway")
g_pButtonUnderGarden:AddDummyInt(DUNGEON_ID.DI_RANOX_GATEWAY_NORMAL)
g_pButtonUnderGarden:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonUnderGarden:SetNormalTex("DLG_Common_Button03_NEW.tga", "Ranox_GateWay_N")
g_pButtonUnderGarden:SetOverTex("DLG_Common_Button03_NEW.tga", "Ranox_GateWay_O")
IconAni(g_pButtonUnderGarden, "DLG_Common_Button03_NEW.tga", "Ranox_GateWay_O")
g_pButtonUnderGarden:SetDownTex("DLG_Common_Button03_NEW.tga", "Ranox_GateWay_O")
g_pButtonUnderGarden:SetNormalPoint({
	CHANGE_TIME = 0,
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(74, 145),
	COLOR = D3DXCOLOR(1, 1, 1, 1)
})
g_pButtonUnderGarden:SetOverPoint({
	CHANGE_TIME = 0,
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(74, 145),
	COLOR = D3DXCOLOR(1, 1, 1, 1)
})
g_pButtonUnderGarden:SetDownPoint({
	CHANGE_TIME = 0,
	ADD_SIZE_X = -14,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(76, 147),
	COLOR = D3DXCOLOR(1, 1, 1, 1)
})
g_pButtonUnderGarden:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonUnderGarden:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonUnderGarden:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonSantilusShip = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonSantilusShip)
g_pButtonSantilusShip:SetName("FireTemple")
g_pButtonSantilusShip:AddDummyInt(DUNGEON_ID.DI_RANOX_FIRETEMPLE_NORMAL)
g_pButtonSantilusShip:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonSantilusShip:SetNormalTex("DLG_Common_Button04_NEW.tga", "Ranox_FIRETEMPLE_O")
g_pButtonSantilusShip:SetOverTex("DLG_Common_Button04_NEW.tga", "Ranox_FIRETEMPLE_N")
IconAni(g_pButtonSantilusShip, "DLG_Common_Button04_NEW.tga", "Ranox_FIRETEMPLE_N")
g_pButtonSantilusShip:SetDownTex("DLG_Common_Button04_NEW.tga", "Ranox_FIRETEMPLE_N")
g_pButtonSantilusShip:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(126, 21)
})
g_pButtonSantilusShip:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(126, 21)
})
g_pButtonSantilusShip:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(133, 28)
})
g_pButtonSantilusShip:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonSantilusShip:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonSantilusShip:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonBehimossHeart = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBehimossHeart)
g_pButtonBehimossHeart:SetName("FinalHall")
g_pButtonBehimossHeart:AddDummyInt(DUNGEON_ID.DI_RANOX_FINALHALL_NORMAL)
g_pButtonBehimossHeart:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonBehimossHeart:SetNormalTex("DLG_Common_Button04_NEW.tga", "Ranox_FINALHALL_N")
g_pButtonBehimossHeart:SetOverTex("DLG_Common_Button04_NEW.tga", "Ranox_FINALHALL_O")
IconAni(g_pButtonBehimossHeart, "DLG_Common_Button04_NEW.tga", "Ranox_FINALHALL_O")
g_pButtonBehimossHeart:SetDownTex("DLG_Common_Button04_NEW.tga", "Ranox_FINALHALL_O")
g_pButtonBehimossHeart:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(250, 0)
})
g_pButtonBehimossHeart:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(250, 0)
})
g_pButtonBehimossHeart:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(257, 7)
})
g_pButtonBehimossHeart:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonBehimossHeart:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonBehimossHeart:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Ranox_Secret_Dungeon = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Ranox_Secret_Dungeon)
g_pButton_Ranox_Secret_Dungeon:SetName("Ranox_Secret")
g_pButton_Ranox_Secret_Dungeon:AddDummyInt(DUNGEON_ID.DI_RANOX_SECRET_HELL)
g_pButton_Ranox_Secret_Dungeon:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Ranox_Secret_Dungeon:SetNormalTex("DLG_Common_Button04_NEW.tga", "Ranox_SECRET_DUNGEON_N")
g_pButton_Ranox_Secret_Dungeon:SetOverTex("DLG_Common_Button04_NEW.tga", "Ranox_SECRET_DUNGEON_O")
IconAni(g_pButton_Ranox_Secret_Dungeon, "DLG_Common_Button04_NEW.tga", "Ranox_SECRET_DUNGEON_O")
g_pButton_Ranox_Secret_Dungeon:SetDownTex("DLG_Common_Button04_NEW.tga", "Ranox_SECRET_DUNGEON_O")
g_pButton_Ranox_Secret_Dungeon:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(4, 0)
})
g_pButton_Ranox_Secret_Dungeon:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(4, 3)
})
g_pButton_Ranox_Secret_Dungeon:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(6, 5)
})
g_pButton_Ranox_Secret_Dungeon:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Ranox_Secret_Dungeon:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Ranox_Secret_Dungeon:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Ranox_Secret_Dungeon_Desc = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Ranox_Secret_Dungeon_Desc)
g_pButton_Ranox_Secret_Dungeon_Desc:SetName("SecretDungeonDesc")
g_pButton_Ranox_Secret_Dungeon_Desc:AddDummyInt(DUNGEON_ID.DI_RANOX_SECRET_HELL)
g_pButton_Ranox_Secret_Dungeon_Desc:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Ranox_Secret_Dungeon_Desc:SetNormalTex("DLG_Common_Button05_NEW.tga", "Ranox_SECRET_DUNGEON_LOCK")
g_pButton_Ranox_Secret_Dungeon_Desc:SetOverTex("DLG_Common_Button05_NEW.tga", "Ranox_SECRET_DUNGEON_LOCK")
g_pButton_Ranox_Secret_Dungeon_Desc:SetDownTex("DLG_Common_Button05_NEW.tga", "Ranox_SECRET_DUNGEON_LOCK")
g_pButton_Ranox_Secret_Dungeon_Desc:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(4, 0)
})
g_pButton_Ranox_Secret_Dungeon_Desc:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(4, 0)
})
g_pButton_Ranox_Secret_Dungeon_Desc:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(4, 0)
})

g_pButton_Event_Dummy01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Event_Dummy01)
g_pButton_Event_Dummy01:SetName("Dungeon")
g_pButton_Event_Dummy01:AddDummyInt(DUNGEON_ID.DI_EVENT_MISSION_IMPOSSIBLE_STAGE1_NORMAL)
g_pButton_Event_Dummy01:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Event_Dummy01:SetNormalTex("DLG_Common_Button02_NEW.tga", "TREE_NORMAL")
IconAni(g_pButton_Event_Dummy01, "DLG_Common_Button02_NEW.tga", "TREE_OVER")
g_pButton_Event_Dummy01:SetDownTex("DLG_Common_Button02_NEW.tga", "TREE_OVER")
g_pButton_Event_Dummy01:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(2, 29),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(17, 34),
	LEFT_TOP_JP = D3DXVECTOR2(0, 44),
	LEFT_TOP_US = D3DXVECTOR2(2, 29)
})
g_pButton_Event_Dummy01:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(2, 29),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(17, 34),
	LEFT_TOP_JP = D3DXVECTOR2(0, 44),
	LEFT_TOP_US = D3DXVECTOR2(2, 29)
})
g_pButton_Event_Dummy01:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(4, 31),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(19, 36),
	LEFT_TOP_JP = D3DXVECTOR2(2, 46),
	LEFT_TOP_US = D3DXVECTOR2(4, 31)
})
g_pButton_Event_Dummy01:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy01:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy01:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Event_Dummy02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Event_Dummy02)
g_pButton_Event_Dummy02:SetName("Dungeon")
g_pButton_Event_Dummy02:AddDummyInt(DUNGEON_ID.DI_EVENT_MISSION_IMPOSSIBLE_STAGE2_NORMAL)
g_pButton_Event_Dummy02:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Event_Dummy02:SetNormalTex("DLG_Common_Button02_NEW.tga", "TREE_NORMAL")
IconAni(g_pButton_Event_Dummy02, "DLG_Common_Button02_NEW.tga", "TREE_OVER")
g_pButton_Event_Dummy02:SetDownTex("DLG_Common_Button02_NEW.tga", "TREE_OVER")
g_pButton_Event_Dummy02:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(150, 280),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(12, 20),
	LEFT_TOP_US = D3DXVECTOR2(150, 280)
})
g_pButton_Event_Dummy02:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(150, 280),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(12, 20),
	LEFT_TOP_US = D3DXVECTOR2(150, 280)
})
g_pButton_Event_Dummy02:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(152, 282),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(14, 22),
	LEFT_TOP_US = D3DXVECTOR2(152, 282)
})
g_pButton_Event_Dummy02:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy02:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy02:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("라녹스_타이틀 모음")

g_pPictureTitleBG_Title_01 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_01)
g_pPictureTitleBG_Title_01:SetTex("DLG_Localmap_Title_01.tga", "Ranox_Burned_Forest")
g_pPictureTitleBG_Title_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(301, 358)
})

g_pPictureTitleBG_Title_02 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_02)
g_pPictureTitleBG_Title_02:SetTex("DLG_Localmap_Title_01.tga", "Ranox_Ashes_Village")
g_pPictureTitleBG_Title_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(499, 255),
	LEFT_TOP_EU = D3DXVECTOR2(479, 255),
	LEFT_TOP_CN = D3DXVECTOR2(484, 255),
	LEFT_TOP_JP = D3DXVECTOR2(479, 255),
	LEFT_TOP_TH = D3DXVECTOR2(479, 255),
	LEFT_TOP_TW = D3DXVECTOR2(479, 255),
	LEFT_TOP_US = D3DXVECTOR2(477, 255)
})

g_pPictureTitleBG_Title_03 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_03)
g_pPictureTitleBG_Title_03:SetTex("DLG_Localmap_Title_01.tga", "Ranox_Geyser")
g_pPictureTitleBG_Title_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(355, 232),
	LEFT_TOP_BR = D3DXVECTOR2(355, 226)
})

g_pPictureTitleBG_Title_04 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_04)
g_pPictureTitleBG_Title_04:SetTex("DLG_Localmap_Title_01.tga", "Ranox_GateWay")
g_pPictureTitleBG_Title_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(102, 224),
	LEFT_TOP_BR = D3DXVECTOR2(102, 218)
})

g_pPictureTitleBG_Title_05 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_05)
g_pPictureTitleBG_Title_05:SetTex("DLG_Localmap_Title_01.tga", "Ranox_FIRETEMPLE")
g_pPictureTitleBG_Title_05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(163, 128)
})

g_pPictureTitleBG_Title_06 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_06)
g_pPictureTitleBG_Title_06:SetTex("DLG_Localmap_Title_01.tga", "Ranox_FINALHALL")
g_pPictureTitleBG_Title_06:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(284, 104),
	LEFT_TOP_US = D3DXVECTOR2(284, 99)
})

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("TITLE_SECRET")
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_RANOX_SECRET_HELL)
g_pButton_TitleBG:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)

g_pPictureTitleBG_Title_Secret = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_Secret)
g_pPictureTitleBG_Title_Secret:SetTex("DLG_Localmap_Title_01.tga", "Map_Title_Ranox")
g_pPictureTitleBG_Title_Secret:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(30, 98)
})

-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): g_pStatic_Event_01/02, the
-- "Title_Holloween" overlays for DI_EVENT_MISSION_IMPOSSIBLE_STAGE1/2_NORMAL,
-- omitted here. Both called AddDummyInt(DETAIL_LOCAL_TYPE.DLT_EVENT_DUNGEON_TITLE),
-- which is confirmed absent from March2014's DETAIL_LOCAL_TYPE enum (only
-- DLT_VILLAGE/DLT_DUNGEON/DLT_ARCADE exist) -- see RANOX_PORT_PLAN.md's
-- engine-gap table, revisited in Phase 9. The Mission Impossible dungeon
-- buttons themselves (g_pButton_Event_Dummy01/02, above) use only DLT_DUNGEON
-- and IDs that already exist in March2014's DungeonEnum.lua, so they are kept.
-- Phase 9 revisit: confirmed this gap does not affect Secret Hell's own icon
-- treatment -- g_pButton_Ranox_Secret_Dungeon,
-- g_pButton_Ranox_Secret_Dungeon_Desc, and g_pButton_TitleBG (this dungeon's
-- own icon/description/title-background buttons, all in this same file) use
-- only DUNGEON_ID.DI_RANOX_SECRET_HELL + DETAIL_LOCAL_TYPE.DLT_DUNGEON, the
-- same plain pattern every other dungeon icon on this map already uses --
-- not DLT_EVENT_DUNGEON_TITLE at all. Nothing is owed here beyond this
-- confirmation; the gap remains confined to the two already-omitted
-- Halloween overlay blocks.

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
