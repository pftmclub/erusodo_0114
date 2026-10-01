--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Sander_Icon_NEW.lua (live data217 copy, 9948 B), adopted wholesale.
-- June dropped the clear-difficulty stars, the Henir button (moved to the Special map)
-- and switched the secret dungeon to its _HELL ID with a SecretDungeonDesc lock button;
-- all of that is kept. Each button's third AddDummyInt is the June Hero dungeon ID,
-- which March has no enum for, so it reads 0 (DI_NONE, as June's own Ruben file uses);
-- CX2LocalMapUI only reads it in hero mode. Controls naming June-only content are
-- omitted below with a note where each stood.
--}}

g_pButtonDrySander = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonDrySander)
g_pButtonDrySander:SetName("Bonghun_temple")
g_pButtonDrySander:AddDummyInt(DUNGEON_ID.DI_SANDER_DRY_SANDER_NORMAL)
g_pButtonDrySander:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonDrySander:AddDummyInt(DUNGEON_ID.DI_SANDER_DRY_SANDER_HERO_NORMAL)
g_pButtonDrySander:SetNormalTex("DLG_Common_Button02_NEW.tga", "DRY_SANDER_NORMAL")
g_pButtonDrySander:SetOverTex("DLG_Common_Button02_NEW.tga", "DRY_SANDER_OVER")
IconAni(g_pButtonDrySander, "DLG_Common_Button02_NEW.tga", "DRY_SANDER_OVER")
g_pButtonDrySander:SetDownTex("DLG_Common_Button02_NEW.tga", "DRY_SANDER_OVER")
g_pButtonDrySander:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(197, 279)
})
g_pButtonDrySander:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(197, 279)
})
g_pButtonDrySander:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(199, 281)
})
g_pButtonDrySander:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonDrySander:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonDrySander:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonGarpaiRock = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonGarpaiRock)
g_pButtonGarpaiRock:SetName("screw_corridor")
g_pButtonGarpaiRock:AddDummyInt(DUNGEON_ID.DI_SANDER_GARPAI_ROCK_NORMAL)
g_pButtonGarpaiRock:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonGarpaiRock:AddDummyInt(DUNGEON_ID.DI_SANDER_GARPAI_ROCK_HERO_NORMAL)
g_pButtonGarpaiRock:SetNormalTex("DLG_Common_Button02_NEW.tga", "GARPAI_NORMAL")
g_pButtonGarpaiRock:SetOverTex("DLG_Common_Button02_NEW.tga", "GARPAI_OVER")
IconAni(g_pButtonGarpaiRock, "DLG_Common_Button02_NEW.tga", "GARPAI_OVER")
g_pButtonGarpaiRock:SetDownTex("DLG_Common_Button02_NEW.tga", "GARPAI_OVER")
g_pButtonGarpaiRock:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(392, 259)
})
g_pButtonGarpaiRock:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(392, 259)
})
g_pButtonGarpaiRock:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(394, 261)
})
g_pButtonGarpaiRock:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonGarpaiRock:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonGarpaiRock:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonChapel = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonChapel)
g_pButtonChapel:SetName("chapel")
g_pButtonChapel:AddDummyInt(DUNGEON_ID.DI_SANDER_DEN_OF_TROCK_NORMAL)
g_pButtonChapel:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonChapel:AddDummyInt(DUNGEON_ID.DI_SANDER_DEN_OF_TROCK_HERO_NORMAL)
g_pButtonChapel:SetNormalTex("DLG_Common_Button03_NEW.tga", "SANDER_DEN_OF_TROCK_NORMAL")
g_pButtonChapel:SetOverTex("DLG_Common_Button03_NEW.tga", "SANDER_DEN_OF_TROCK_OVER")
IconAni(g_pButtonChapel, "DLG_Common_Button03_NEW.tga", "SANDER_DEN_OF_TROCK_OVER")
g_pButtonChapel:SetDownTex("DLG_Common_Button03_NEW.tga", "SANDER_DEN_OF_TROCK_OVER")
g_pButtonChapel:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(322, 158)
})
g_pButtonChapel:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(322, 158)
})
g_pButtonChapel:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(324, 160)
})
g_pButtonChapel:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonChapel:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonChapel:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonUnderGarden = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonUnderGarden)
g_pButtonUnderGarden:SetName("underGarden")
g_pButtonUnderGarden:AddDummyInt(DUNGEON_ID.DI_SANDER_KARUSO_VILLAGE_NORMAL)
g_pButtonUnderGarden:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonUnderGarden:AddDummyInt(DUNGEON_ID.DI_SANDER_KARUSO_VILLAGE_HERO_NORMAL)
g_pButtonUnderGarden:SetNormalTex("DLG_Common_Button03_NEW.tga", "SANDER_KARUSO_VILLAGE_NORMAL")
g_pButtonUnderGarden:SetOverTex("DLG_Common_Button03_NEW.tga", "SANDER_KARUSO_VILLAGE_OVER")
IconAni(g_pButtonUnderGarden, "DLG_Common_Button03_NEW.tga", "SANDER_KARUSO_VILLAGE_OVER")
g_pButtonUnderGarden:SetDownTex("DLG_Common_Button03_NEW.tga", "SANDER_KARUSO_VILLAGE_OVER")
g_pButtonUnderGarden:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(81, 155)
})
g_pButtonUnderGarden:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(81, 155)
})
g_pButtonUnderGarden:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(83, 157)
})
g_pButtonUnderGarden:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonUnderGarden:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonUnderGarden:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonSantilusShip = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonSantilusShip)
g_pButtonSantilusShip:SetName("SantilusShip")
g_pButtonSantilusShip:AddDummyInt(DUNGEON_ID.DI_SANDER_SANTILUS_SHIP_NORMAL)
g_pButtonSantilusShip:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonSantilusShip:AddDummyInt(DUNGEON_ID.DI_SANDER_SANTILUS_SHIP_HERO_NORMAL)
g_pButtonSantilusShip:SetNormalTex("DLG_Common_Button03_NEW.tga", "Sander_Santilus_Ship_NORMAL")
g_pButtonSantilusShip:SetOverTex("DLG_Common_Button03_NEW.tga", "Sander_Santilus_Ship_OVER")
IconAni(g_pButtonSantilusShip, "DLG_Common_Button03_NEW.tga", "Sander_Santilus_Ship_OVER")
g_pButtonSantilusShip:SetDownTex("DLG_Common_Button03_NEW.tga", "Sander_Santilus_Ship_OVER")
g_pButtonSantilusShip:SetNormalPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(175, 70)
})
g_pButtonSantilusShip:SetOverPoint({
	ADD_SIZE_X = -10,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(175, 70)
})
g_pButtonSantilusShip:SetDownPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(177, 72)
})
g_pButtonSantilusShip:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonSantilusShip:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonSantilusShip:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonBehimossHeart = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBehimossHeart)
g_pButtonBehimossHeart:SetName("BehimossHeart")
g_pButtonBehimossHeart:AddDummyInt(DUNGEON_ID.DI_SANDER_BEHIMOSS_HEART_NORMAL)
g_pButtonBehimossHeart:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonBehimossHeart:AddDummyInt(DUNGEON_ID.DI_SANDER_BEHIMOSS_HEART_HERO_NORMAL)
g_pButtonBehimossHeart:SetNormalTex("DLG_Common_Button03_NEW.tga", "Sander_Behimoss_Heart_NORMAL")
g_pButtonBehimossHeart:SetOverTex("DLG_Common_Button03_NEW.tga", "Sander_Behimoss_Heart_OVER")
IconAni(g_pButtonBehimossHeart, "DLG_Common_Button03_NEW.tga", "Sander_Behimoss_Heart_OVER")
g_pButtonBehimossHeart:SetDownTex("DLG_Common_Button03_NEW.tga", "Sander_Behimoss_Heart_OVER")
g_pButtonBehimossHeart:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(337, 39)
})
g_pButtonBehimossHeart:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(337, 39)
})
g_pButtonBehimossHeart:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(339, 41)
})
g_pButtonBehimossHeart:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonBehimossHeart:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonBehimossHeart:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

-- [June UI parity Phase 3] omitted: g_pButton_Sander_Secret_Dungeon (June-only Sander secret dungeon: DI_SANDER_SECRET_HELL and Title_Sander_Secret are not in March).

-- [June UI parity Phase 3] omitted: g_pButton_Sander_Secret_Dungeon_Desc (June-only Sander secret dungeon: DI_SANDER_SECRET_HELL and Title_Sander_Secret are not in March).

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
	LEFT_TOP = D3DXVECTOR2(433, 146),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(451, 146),
	LEFT_TOP_JP = D3DXVECTOR2(431, 146),
	LEFT_TOP_US = D3DXVECTOR2(433, 146)
})
g_pButton_Event_Dummy01:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(433, 146),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(451, 146),
	LEFT_TOP_JP = D3DXVECTOR2(431, 146),
	LEFT_TOP_US = D3DXVECTOR2(433, 146)
})
g_pButton_Event_Dummy01:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(435, 148),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(453, 148),
	LEFT_TOP_JP = D3DXVECTOR2(433, 148),
	LEFT_TOP_US = D3DXVECTOR2(435, 148)
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
	LEFT_TOP = D3DXVECTOR2(460, 230),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(460, 120),
	LEFT_TOP_US = D3DXVECTOR2(460, 230)
})
g_pButton_Event_Dummy02:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(460, 230),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(460, 120),
	LEFT_TOP_US = D3DXVECTOR2(460, 230)
})
g_pButton_Event_Dummy02:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(462, 232),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(462, 122),
	LEFT_TOP_US = D3DXVECTOR2(462, 232)
})
g_pButton_Event_Dummy02:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy02:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy02:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("샌더_타이틀 모음")

g_pPictureTitleBG_Title_01 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_01)
g_pPictureTitleBG_Title_01:SetTex("DLG_Localmap_Title_01.tga", "Title_Sander_01")
g_pPictureTitleBG_Title_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(225, 354)
})

g_pPictureTitleBG_Title_02 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_02)
g_pPictureTitleBG_Title_02:SetTex("DLG_Localmap_Title_01.tga", "Title_Sander_02")
g_pPictureTitleBG_Title_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(426, 337)
})

g_pPictureTitleBG_Title_03 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_03)
g_pPictureTitleBG_Title_03:SetTex("DLG_Localmap_Title_01.tga", "Title_Sander_03")
g_pPictureTitleBG_Title_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(355, 232)
})

g_pPictureTitleBG_Title_04 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_04)
g_pPictureTitleBG_Title_04:SetTex("DLG_Localmap_Title_01.tga", "Title_Sander_04")
g_pPictureTitleBG_Title_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(102, 224)
})

g_pPictureTitleBG_Title_05 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_05)
g_pPictureTitleBG_Title_05:SetTex("DLG_Localmap_Title_01.tga", "Title_Sander_05")
g_pPictureTitleBG_Title_05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(210, 148)
})

g_pPictureTitleBG_Title_06 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_06)
g_pPictureTitleBG_Title_06:SetTex("DLG_Localmap_Title_01.tga", "Title_Sander_06")
g_pPictureTitleBG_Title_06:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(366, 109)
})

-- [June UI parity Phase 3] omitted: g_pButton_TitleBG (June-only Sander secret dungeon: DI_SANDER_SECRET_HELL and Title_Sander_Secret are not in March).

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
	LEFT_TOP = D3DXVECTOR2(453, 201),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(37, 81)
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
	LEFT_TOP = D3DXVECTOR2(480, 285),
	LEFT_TOP_GLOBAL = D3DXVECTOR2(480, 175)
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

g_pButton_STAR8 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR8)
g_pButton_STAR8:SetNameStrID(STR_ID_34700)

g_pPictureSTAR8_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_StarBG)
g_pPictureSTAR8_StarBG:SetTex("DLG_Common_Button00_NEW.TGA", "Star_Bg")
g_pPictureSTAR8_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(72, 112)
})
g_pButton_STAR8:SetShow(false)
