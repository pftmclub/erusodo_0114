--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Besma_Icon_NEW.lua (live data217 copy, 9946 B), adopted wholesale.
-- June dropped the clear-difficulty stars, the Henir button (moved to the Special map)
-- and switched the secret dungeon to its _HELL ID with a SecretDungeonDesc lock button;
-- all of that is kept. Each button's third AddDummyInt is the June Hero dungeon ID,
-- which March has no enum for, so it reads 0 (DI_NONE, as June's own Ruben file uses);
-- CX2LocalMapUI only reads it in hero mode. Controls naming June-only content are
-- omitted below with a note where each stood.
--}}

g_pButtonDragon_Way = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonDragon_Way)
g_pButtonDragon_Way:SetName("Dragon_Way")
g_pButtonDragon_Way:AddDummyInt(DUNGEON_ID.DI_BESMA_DRAGON_ROAD_NORMAL)
g_pButtonDragon_Way:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonDragon_Way:AddDummyInt(DUNGEON_ID.DI_BESMA_DRAGON_ROAD_HERO_NORMAL)
g_pButtonDragon_Way:SetNormalTex("DLG_Common_Button00_NEW.tga", "Dragon_Way_normal")
g_pButtonDragon_Way:SetOverTex("DLG_Common_Button00_NEW.tga", "Dragon_Way_Over")
IconAni(g_pButtonDragon_Way, "DLG_Common_Button00_NEW.tga", "Dragon_Way_Over")
g_pButtonDragon_Way:SetDownTex("DLG_Common_Button00_NEW.tga", "Dragon_Way_Over")
g_pButtonDragon_Way:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(49, 114)
})
g_pButtonDragon_Way:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(42, 107)
})
g_pButtonDragon_Way:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(44, 109)
})
g_pButtonDragon_Way:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonDragon_Way:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonDragon_Way:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonBesma_Lake = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBesma_Lake)
g_pButtonBesma_Lake:SetName("Besma_Lake")
g_pButtonBesma_Lake:AddDummyInt(DUNGEON_ID.DI_BESMA_LAKE_NORMAL)
g_pButtonBesma_Lake:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonBesma_Lake:AddDummyInt(DUNGEON_ID.DI_BESMA_LAKE_HERO_NORMAL)
g_pButtonBesma_Lake:SetNormalTex("DLG_Common_Button00_NEW.tga", "Besma_Lake_normal")
g_pButtonBesma_Lake:SetOverTex("DLG_Common_Button00_NEW.tga", "Besma_Lake_Over")
IconAni(g_pButtonBesma_Lake, "DLG_Common_Button00_NEW.tga", "Besma_Lake_Over")
g_pButtonBesma_Lake:SetDownTex("DLG_Common_Button00_NEW.tga", "Besma_Lake_Over")
g_pButtonBesma_Lake:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(135, 194)
})
g_pButtonBesma_Lake:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(129, 187)
})
g_pButtonBesma_Lake:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(131, 189)
})
g_pButtonBesma_Lake:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonBesma_Lake:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonBesma_Lake:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonBesma_Lakenight = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBesma_Lakenight)
g_pButtonBesma_Lakenight:SetName("Besma_Lakenight")
g_pButtonBesma_Lakenight:AddDummyInt(DUNGEON_ID.DI_BESMA_LAKE_NIGHT_NORMAL)
g_pButtonBesma_Lakenight:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonBesma_Lakenight:AddDummyInt(DUNGEON_ID.DI_BESMA_LAKE_NIGHT_HERO_NORMAL)
g_pButtonBesma_Lakenight:SetNormalTex("DLG_Common_Button00_NEW.tga", "Besma_Lakenight_normal")
g_pButtonBesma_Lakenight:SetOverTex("DLG_Common_Button00_NEW.tga", "Besma_Lakenight_Over")
IconAni(g_pButtonBesma_Lakenight, "DLG_Common_Button00_NEW.tga", "Besma_Lakenight_Over")
g_pButtonBesma_Lakenight:SetDownTex("DLG_Common_Button00_NEW.tga", "Besma_Lakenight_Over")
g_pButtonBesma_Lakenight:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(246, 259)
})
g_pButtonBesma_Lakenight:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(240, 253)
})
g_pButtonBesma_Lakenight:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(242, 255)
})
g_pButtonBesma_Lakenight:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonBesma_Lakenight:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonBesma_Lakenight:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonDragon_Nest = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonDragon_Nest)
g_pButtonDragon_Nest:SetName("Dragon_Nest")
g_pButtonDragon_Nest:AddDummyInt(DUNGEON_ID.DI_BESMA_DRAGON_NEST_NORMAL)
g_pButtonDragon_Nest:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonDragon_Nest:AddDummyInt(DUNGEON_ID.DI_BESMA_DRAGON_NEST_HERO_NORMAL)
g_pButtonDragon_Nest:SetNormalTex("DLG_Common_Button00_NEW.tga", "Dragon_Nest_normal")
g_pButtonDragon_Nest:SetOverTex("DLG_Common_Button00_NEW.tga", "Dragon_Nest_Over")
IconAni(g_pButtonDragon_Nest, "DLG_Common_Button00_NEW.tga", "Dragon_Nest_Over")
g_pButtonDragon_Nest:SetDownTex("DLG_Common_Button00_NEW.tga", "Dragon_Nest_Over")
g_pButtonDragon_Nest:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(378, 246)
})
g_pButtonDragon_Nest:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(373, 240)
})
g_pButtonDragon_Nest:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(375, 242)
})
g_pButtonDragon_Nest:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonDragon_Nest:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonDragon_Nest:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonBesma_Coal = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBesma_Coal)
g_pButtonBesma_Coal:SetName("Besma_Coal_Deep")
g_pButtonBesma_Coal:AddDummyInt(DUNGEON_ID.DI_BESMA_MINE2_NORMAL)
g_pButtonBesma_Coal:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonBesma_Coal:AddDummyInt(DUNGEON_ID.DI_BESMA_MINE2_HERO_NORMAL)
g_pButtonBesma_Coal:SetNormalTex("DLG_Common_Button00_NEW.tga", "Besma_Coal_normal")
g_pButtonBesma_Coal:SetOverTex("DLG_Common_Button00_NEW.tga", "Besma_Coal_Over")
IconAni(g_pButtonBesma_Coal, "DLG_Common_Button00_NEW.tga", "Besma_Coal_Over")
g_pButtonBesma_Coal:SetDownTex("DLG_Common_Button00_NEW.tga", "Besma_Coal_Over")
g_pButtonBesma_Coal:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(428, 140)
})
g_pButtonBesma_Coal:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(422, 134)
})
g_pButtonBesma_Coal:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(424, 136)
})
g_pButtonBesma_Coal:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonBesma_Coal:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonBesma_Coal:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButtonSky_Ship = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonSky_Ship)
g_pButtonSky_Ship:SetName("Sky_Ship")
g_pButtonSky_Ship:AddDummyInt(DUNGEON_ID.DI_BESMA_AIRSHIP_NORMAL)
g_pButtonSky_Ship:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButtonSky_Ship:AddDummyInt(DUNGEON_ID.DI_BESMA_AIRSHIP_HERO_NORMAL)
g_pButtonSky_Ship:SetNormalTex("DLG_Common_Button00_NEW.tga", "Sky_Ship_normal")
g_pButtonSky_Ship:SetOverTex("DLG_Common_Button00_NEW.tga", "Sky_Ship_Over")
IconAni(g_pButtonSky_Ship, "DLG_Common_Button00_NEW.tga", "Sky_Ship_Over")
g_pButtonSky_Ship:SetDownTex("DLG_Common_Button00_NEW.tga", "Sky_Ship_Over")
g_pButtonSky_Ship:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(330, 60)
})
g_pButtonSky_Ship:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(324, 54)
})
g_pButtonSky_Ship:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(326, 56)
})
g_pButtonSky_Ship:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButtonSky_Ship:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButtonSky_Ship:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

-- [June UI parity Phase 3] omitted: g_pButtonBastera (June-only Bestera content: DI_BESTERA_START and TITLE_Bestera are not in March).

g_pButton_Velder_South_Secret_Dungeon = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Velder_South_Secret_Dungeon)
g_pButton_Velder_South_Secret_Dungeon:SetName("Dungeon")
g_pButton_Velder_South_Secret_Dungeon:AddDummyInt(DUNGEON_ID.DI_BESMA_SECRET_HELL)
g_pButton_Velder_South_Secret_Dungeon:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Velder_South_Secret_Dungeon:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Velder_South_Secret_Dungeon:SetNormalTex("DLG_Common_Button03_NEW.tga", "DRAGON_NEST_SECRET_NORMAL")
g_pButton_Velder_South_Secret_Dungeon:SetOverTex("DLG_Common_Button03_NEW.tga", "DRAGON_NEST_SECRET_OVER")
IconAni(g_pButton_Velder_South_Secret_Dungeon, "DLG_Common_Button03_NEW.tga", "DRAGON_NEST_SECRET_OVER")
g_pButton_Velder_South_Secret_Dungeon:SetDownTex("DLG_Common_Button03_NEW.tga", "DRAGON_NEST_SECRET_OVER")
g_pButton_Velder_South_Secret_Dungeon:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(470, 280)
})
g_pButton_Velder_South_Secret_Dungeon:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(470, 280)
})
g_pButton_Velder_South_Secret_Dungeon:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(472, 282)
})
g_pButton_Velder_South_Secret_Dungeon:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Velder_South_Secret_Dungeon:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Velder_South_Secret_Dungeon:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Velder_South_Secret_Dungeon_Desc = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Velder_South_Secret_Dungeon_Desc)
g_pButton_Velder_South_Secret_Dungeon_Desc:SetName("SecretDungeonDesc")
g_pButton_Velder_South_Secret_Dungeon_Desc:AddDummyInt(DUNGEON_ID.DI_BESMA_SECRET_HELL)
g_pButton_Velder_South_Secret_Dungeon_Desc:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Velder_South_Secret_Dungeon_Desc:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Velder_South_Secret_Dungeon_Desc:SetNormalTex("DLG_Common_Button05_NEW.tga", "DRAGON_NEST_SECRET_LOCK")
g_pButton_Velder_South_Secret_Dungeon_Desc:SetOverTex("DLG_Common_Button05_NEW.tga", "DRAGON_NEST_SECRET_LOCK")
g_pButton_Velder_South_Secret_Dungeon_Desc:SetDownTex("DLG_Common_Button05_NEW.tga", "DRAGON_NEST_SECRET_LOCK")
g_pButton_Velder_South_Secret_Dungeon_Desc:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(470, 280)
})
g_pButton_Velder_South_Secret_Dungeon_Desc:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(470, 280)
})
g_pButton_Velder_South_Secret_Dungeon_Desc:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(470, 280)
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
	LEFT_TOP = D3DXVECTOR2(17, 283)
})
g_pButton_Event_Dummy01:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(17, 283)
})
g_pButton_Event_Dummy01:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(19, 285)
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
	LEFT_TOP = D3DXVECTOR2(87, 20)
})
g_pButton_Event_Dummy02:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(87, 20)
})
g_pButton_Event_Dummy02:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(89, 22)
})
g_pButton_Event_Dummy02:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy02:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy02:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("베스마_타이틀 모음")

g_pPictureTitleBG_Title_01 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_01)
g_pPictureTitleBG_Title_01:SetTex("DLG_Localmap_Title_01.tga", "Title_Besma_01")
g_pPictureTitleBG_Title_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(66, 165)
})

g_pPictureTitleBG_Title_02 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_02)
g_pPictureTitleBG_Title_02:SetTex("DLG_Localmap_Title_01.tga", "Title_Besma_02")
g_pPictureTitleBG_Title_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(152, 247)
})

g_pPictureTitleBG_Title_03 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_03)
g_pPictureTitleBG_Title_03:SetTex("DLG_Localmap_Title_01.tga", "Title_Besma_03")
g_pPictureTitleBG_Title_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(263, 313)
})

g_pPictureTitleBG_Title_04 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_04)
g_pPictureTitleBG_Title_04:SetTex("DLG_Localmap_Title_01.tga", "Title_Besma_04")
g_pPictureTitleBG_Title_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(397, 298),
	LEFT_TOP_BR = D3DXVECTOR2(377, 298)
})

g_pPictureTitleBG_Title_05 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_05)
g_pPictureTitleBG_Title_05:SetTex("DLG_Localmap_Title_01.tga", "Title_Besma_05")
g_pPictureTitleBG_Title_05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(444, 192)
})

g_pPictureTitleBG_Title_06 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_06)
g_pPictureTitleBG_Title_06:SetTex("DLG_Localmap_Title_01.tga", "Title_Besma_06")
g_pPictureTitleBG_Title_06:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(345, 110)
})

-- [June UI parity Phase 3] omitted: g_pButton_TitleBG (June-only Bestera content: DI_BESTERA_START and TITLE_Bestera are not in March).

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
	LEFT_TOP = D3DXVECTOR2(37, 338)
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
	LEFT_TOP = D3DXVECTOR2(107, 75)
})

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("TITLE_SECRET")
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_BESMA_SECRET_HELL)
g_pButton_TitleBG:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_NONE)

g_pPictureTitleBG_Title_Secret = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_Secret)
g_pPictureTitleBG_Title_Secret:SetTex("DLG_Localmap_Title_01.tga", "Title_Besma_Secret")
g_pPictureTitleBG_Title_Secret:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(493, 352),
	LEFT_TOP_BR = D3DXVECTOR2(433, 352),
	LEFT_TOP_CN = D3DXVECTOR2(483, 352)
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

g_pButton_STAR2 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR2)
g_pButton_STAR2:SetNameStrID(STR_ID_3501)

g_pPictureSTAR2_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR2:AddPicture(g_pPictureSTAR2_StarBG)
g_pPictureSTAR2_StarBG:SetTex("DLG_Common_Button00_NEW.tga", "Star_Bg")
g_pPictureSTAR2_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(485, 369)
})
g_pButton_STAR2:SetShow(false)
