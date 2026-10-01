--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Special_Icon_NEW.lua (live data217 copy, 7958 B), adopted wholesale.
-- March has no LMI_SPECIAL content of its own; this map exists to carry Henir,
-- which June moved off every village map onto it. Only Henir is kept.
-- STAR8 (SetNameStrID(STR_ID_3722), Henir's dungeonName) is kept: CreateDungeonClearStars
-- positions Henir's _LimitInfo plate from its first picture.
--}}

g_pButton_emir_space = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_emir_space)
g_pButton_emir_space:SetName("Dungeon")
g_pButton_emir_space:AddDummyString("Glow_Forest_Star")
g_pButton_emir_space:AddDummyInt(DUNGEON_ID.DI_ELDER_HENIR_SPACE)
g_pButton_emir_space:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_emir_space:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_emir_space:SetNormalTex("DLG_Common_Button02_NEW.tga", "EMIR_SPACE_NORMAL")
IconAni(g_pButton_emir_space, "DLG_Common_Button02_NEW.tga", "EMIR_SPACE_OVER")
g_pButton_emir_space:SetDownTex("DLG_Common_Button02_NEW.tga", "EMIR_SPACE_OVER")
g_pButton_emir_space:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(50, 50)
})
g_pButton_emir_space:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(41, 42)
})
g_pButton_emir_space:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(43, 50)
})
g_pButton_emir_space:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_emir_space:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_emir_space:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

-- [June UI parity Phase 3] omitted: g_pButton_HeroDungeon (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

-- [June UI parity Phase 3] omitted: g_pButton_Arena_Erda (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

-- [June UI parity Phase 3] omitted: g_pButton_Add_Cnter (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

-- [June UI parity Phase 3] omitted: g_pButton_Space_of_Thought (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("TITLE_HENIR")
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_ELDER_HENIR_SPACE)
g_pButton_TitleBG:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_NONE)

g_pPictureTitleBG_Title_Henir = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_Henir)
g_pPictureTitleBG_Title_Henir:SetTex("DLG_Localmap_Title_01.tga", "Title_Henir")
g_pPictureTitleBG_Title_Henir:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(43, 119),
	LEFT_TOP_BR = D3DXVECTOR2(20, 119),
	LEFT_TOP_CN = D3DXVECTOR2(48, 119),
	LEFT_TOP_JP = D3DXVECTOR2(46, 119),
	LEFT_TOP_TH = D3DXVECTOR2(34, 119),
	LEFT_TOP_TW = D3DXVECTOR2(48, 119),
	LEFT_TOP_US = D3DXVECTOR2(33, 119)
})

-- [June UI parity Phase 3] omitted: g_pButton_TitleBG (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

-- [June UI parity Phase 3] omitted: g_pStatic_Erda_Enter_StartTime (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

-- [June UI parity Phase 3] omitted: g_pButton_TitleBG (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

-- [June UI parity Phase 3] omitted: g_pButton_TitleBG (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

-- [June UI parity Phase 3] omitted: g_pButton_TitleBG (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

g_pButton_STAR8 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR8)
g_pButton_STAR8:SetNameStrID(STR_ID_3722)

g_pPictureSTAR8_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_StarBG)
g_pPictureSTAR8_StarBG:SetTex("DLG_Common_Button00_NEW.TGA", "Star_Bg")
g_pPictureSTAR8_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(46, 110)
})

g_pPictureSTAR8_Star0 = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_Star0)
g_pPictureSTAR8_Star0:SetTex("DLG_Common_Button00_NEW.TGA", "Star_1")
g_pPictureSTAR8_Star0:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(66, 130),
	COLOR = D3DXCOLOR(0, 1, 0, 1)
})

g_pPictureSTAR8_Star1 = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_Star1)
g_pPictureSTAR8_Star1:SetTex("DLG_Common_Button00_NEW.TGA", "Star_2")
g_pPictureSTAR8_Star1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(80, 130),
	COLOR = D3DXCOLOR(1, 1, 0, 1)
})

g_pPictureSTAR8_Star2 = g_pUIDialog:CreatePicture()

g_pButton_STAR8:AddPicture(g_pPictureSTAR8_Star2)
g_pPictureSTAR8_Star2:SetTex("DLG_Common_Button00_NEW.TGA", "Star_3")
g_pPictureSTAR8_Star2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(93, 130),
	COLOR = D3DXCOLOR(1, 0, 0, 1)
})
g_pButton_STAR8:SetShow(false)

-- [June UI parity Phase 3] omitted: g_pButton_STAR10 (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

-- [June UI parity Phase 3] omitted: g_pButton_STAR2 (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).

-- [June UI parity Phase 3] omitted: g_pButton_STAR9 (June-only Special-map content (Hero dungeon / Erda / Apocalypse / Space of Thought / events), none of it in March).
