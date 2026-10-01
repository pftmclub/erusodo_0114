--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Peita_Icon_NEW.lua (live data217 copy, 7471 B), adopted wholesale.
-- June dropped the clear-difficulty stars, the Henir button (moved to the Special map)
-- and switched the secret dungeon to its _HELL ID with a SecretDungeonDesc lock button;
-- all of that is kept. Each button's third AddDummyInt is the June Hero dungeon ID,
-- which March has no enum for, so it reads 0 (DI_NONE, as June's own Ruben file uses);
-- June renamed Peita's chain from DLG_UI_Local_Peita_*; this replaces
-- data034/DLG_UI_Local_Peita_Icon_NEW.lua, which nothing loads any more.
--}}

g_pButtonBonghun_temple = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBonghun_temple)
g_pButtonBonghun_temple:SetName("Bonghun_temple")
g_pButtonBonghun_temple:AddDummyInt(DUNGEON_ID.DI_PEITA_OFFERINGS_NORMAL)
g_pButtonBonghun_temple:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonBonghun_temple:AddDummyInt(DUNGEON_ID.DI_PEITA_OFFERINGS_HERO_NORMAL)
g_pButtonBonghun_temple:SetNormalTex("DLG_Common_Button00_NEW.tga", "Bonghun_Temple_normal")
g_pButtonBonghun_temple:SetOverTex("DLG_Common_Button00_NEW.tga", "Bonghun_temple_Over")
IconAni(g_pButtonBonghun_temple, "DLG_Common_Button00_NEW.tga", "Bonghun_Temple_Over")
g_pButtonBonghun_temple:SetDownTex("DLG_Common_Button00_NEW.tga", "Bonghun_Temple_Over")
g_pButtonBonghun_temple:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(83, 200)
})
g_pButtonBonghun_temple:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(79, 192)
})
g_pButtonBonghun_temple:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(81, 194)
})
g_pButtonBonghun_temple:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonBonghun_temple:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonBonghun_temple:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonscrew_corridor = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonscrew_corridor)
g_pButtonscrew_corridor:SetName("screw_corridor")
g_pButtonscrew_corridor:AddDummyInt(DUNGEON_ID.DI_PEITA_SPIRAL_CORRIDOR_NORMAL)
g_pButtonscrew_corridor:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonscrew_corridor:AddDummyInt(DUNGEON_ID.DI_PEITA_SPIRAL_CORRIDOR_HERO_NORMAL)
g_pButtonscrew_corridor:SetNormalTex("DLG_Common_Button00_NEW.tga", "screw_corridor_normal")
g_pButtonscrew_corridor:SetOverTex("DLG_Common_Button00_NEW.tga", "screw_corridor_Over")
IconAni(g_pButtonscrew_corridor, "DLG_Common_Button00_NEW.tga", "screw_corridor_Over")
g_pButtonscrew_corridor:SetDownTex("DLG_Common_Button00_NEW.tga", "screw_corridor_Over")
g_pButtonscrew_corridor:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(160, 280)
})
g_pButtonscrew_corridor:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(154, 272)
})
g_pButtonscrew_corridor:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(156, 274)
})
g_pButtonscrew_corridor:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonscrew_corridor:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonscrew_corridor:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonChapel = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonChapel)
g_pButtonChapel:SetName("chapel")
g_pButtonChapel:AddDummyInt(DUNGEON_ID.DI_PEITA_CHAPEL_NORMAL)
g_pButtonChapel:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonChapel:AddDummyInt(DUNGEON_ID.DI_PEITA_CHAPEL_HERO_NORMAL)
g_pButtonChapel:SetNormalTex("DLG_Common_Button00_NEW.tga", "Chapel_normal")
g_pButtonChapel:SetOverTex("DLG_Common_Button00_NEW.tga", "Chapel_Over")
IconAni(g_pButtonChapel, "DLG_Common_Button00_NEW.tga", "Chapel_Over")
g_pButtonChapel:SetDownTex("DLG_Common_Button00_NEW.tga", "Chapel_Over")
g_pButtonChapel:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(321, 263)
})
g_pButtonChapel:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(318, 257)
})
g_pButtonChapel:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(320, 259)
})
g_pButtonChapel:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonChapel:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonChapel:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonUnderGarden = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonUnderGarden)
g_pButtonUnderGarden:SetName("underGarden")
g_pButtonUnderGarden:AddDummyInt(DUNGEON_ID.DI_PEITA_UNDER_GARDEN_NORMAL)
g_pButtonUnderGarden:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonUnderGarden:AddDummyInt(DUNGEON_ID.DI_PEITA_UNDER_GARDEN_HERO_NORMAL)
g_pButtonUnderGarden:SetNormalTex("DLG_Common_Button01_NEW.tga", "under_garden_normal")
g_pButtonUnderGarden:SetOverTex("DLG_Common_Button01_NEW.tga", "under_garden_Over")
IconAni(g_pButtonUnderGarden, "DLG_Common_Button01_NEW.tga", "under_garden_Over")
g_pButtonUnderGarden:SetDownTex("DLG_Common_Button01_NEW.tga", "under_garden_Over")
g_pButtonUnderGarden:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(355, 188)
})
g_pButtonUnderGarden:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(351, 182)
})
g_pButtonUnderGarden:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(353, 184)
})
g_pButtonUnderGarden:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonUnderGarden:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonUnderGarden:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonTowerHeart = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonTowerHeart)
g_pButtonTowerHeart:SetName("TowerHeart")
g_pButtonTowerHeart:AddDummyInt(DUNGEON_ID.DI_PEITA_TOWER_HEART_NORMAL)
g_pButtonTowerHeart:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonTowerHeart:AddDummyInt(DUNGEON_ID.DI_PEITA_TOWER_HEART_HERO_NORMAL)
g_pButtonTowerHeart:SetNormalTex("DLG_Common_Button00_NEW.tga", "Tower_Heart_Normal")
g_pButtonUnderGarden:SetOverTex("DLG_Common_Button00_NEW.tga", "Tower_Heart_Over")
IconAni(g_pButtonTowerHeart, "DLG_Common_Button00_NEW.tga", "Tower_Heart_Over")
g_pButtonTowerHeart:SetDownTex("DLG_Common_Button00_NEW.tga", "Tower_Heart_Over")
g_pButtonTowerHeart:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(195, 129)
})
g_pButtonTowerHeart:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(189, 123)
})
g_pButtonTowerHeart:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(191, 125)
})
g_pButtonTowerHeart:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonTowerHeart:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonTowerHeart:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonOfferingsAlter = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonOfferingsAlter)
g_pButtonOfferingsAlter:SetName("OfferingsAlter")
g_pButtonOfferingsAlter:AddDummyInt(DUNGEON_ID.DI_PEITA_OFFERINGS_ALTER_NORMAL)
g_pButtonOfferingsAlter:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonOfferingsAlter:AddDummyInt(DUNGEON_ID.DI_PEITA_OFFERINGS_ALTER_HERO_NORMAL)
g_pButtonOfferingsAlter:SetNormalTex("DLG_Common_Button00_NEW.tga", "Offerings_Alter_Normal")
g_pButtonUnderGarden:SetOverTex("DLG_Common_Button00_NEW.tga", "Offerings_Alter_Over")
IconAni(g_pButtonOfferingsAlter, "DLG_Common_Button00_NEW.tga", "Offerings_Alter_Over")
g_pButtonOfferingsAlter:SetDownTex("DLG_Common_Button00_NEW.tga", "Offerings_Alter_Over")
g_pButtonOfferingsAlter:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(306, 32)
})
g_pButtonOfferingsAlter:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(301, 26)
})
g_pButtonOfferingsAlter:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(303, 28)
})
g_pButtonOfferingsAlter:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonOfferingsAlter:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonOfferingsAlter:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

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
	LEFT_TOP = D3DXVECTOR2(430, 280)
})
g_pButton_Event_Dummy01:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(430, 280)
})
g_pButton_Event_Dummy01:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(432, 282)
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
	LEFT_TOP = D3DXVECTOR2(30, 28)
})
g_pButton_Event_Dummy02:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(30, 28)
})
g_pButton_Event_Dummy02:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(32, 30)
})
g_pButton_Event_Dummy02:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy02:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy02:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("페이타_타이틀 모음")

g_pPictureTitleBG_Title_01 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_01)
g_pPictureTitleBG_Title_01:SetTex("DLG_Localmap_Title_01.tga", "Title_Peita_01")
g_pPictureTitleBG_Title_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(102, 267)
})

g_pPictureTitleBG_Title_02 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_02)
g_pPictureTitleBG_Title_02:SetTex("DLG_Localmap_Title_01.tga", "Title_Peita_02")
g_pPictureTitleBG_Title_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(179, 336)
})

g_pPictureTitleBG_Title_03 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_03)
g_pPictureTitleBG_Title_03:SetTex("DLG_Localmap_Title_01.tga", "Title_Peita_03")
g_pPictureTitleBG_Title_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(339, 316)
})

g_pPictureTitleBG_Title_04 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_04)
g_pPictureTitleBG_Title_04:SetTex("DLG_Localmap_Title_01.tga", "Title_Peita_04")
g_pPictureTitleBG_Title_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(378, 237)
})

g_pPictureTitleBG_Title_05 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_05)
g_pPictureTitleBG_Title_05:SetTex("DLG_Localmap_Title_01.tga", "Title_Peita_05")
g_pPictureTitleBG_Title_05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(212, 179)
})

g_pPictureTitleBG_Title_06 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_06)
g_pPictureTitleBG_Title_06:SetTex("DLG_Localmap_Title_01.tga", "Title_Peita_06")
g_pPictureTitleBG_Title_06:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(321, 95)
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
	LEFT_TOP = D3DXVECTOR2(450, 338)
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
	LEFT_TOP = D3DXVECTOR2(50, 78)
})

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
