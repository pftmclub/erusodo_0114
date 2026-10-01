--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Ruben_Icon_NEW.lua (live data034 copy, 2741 B), adopted wholesale.
-- June dropped the clear-difficulty stars, the Henir button (moved to the Special map)
-- and switched the secret dungeon to its _HELL ID with a SecretDungeonDesc lock button;
-- all of that is kept. Each button's third AddDummyInt is the June Hero dungeon ID,
-- which March has no enum for, so it reads 0 (DI_NONE, as June's own Ruben file uses);
-- CX2LocalMapUI only reads it in hero mode. Controls naming June-only content are
-- omitted below with a note where each stood.
--}}

g_pButton_Ruben_El_Tree = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Ruben_El_Tree)
g_pButton_Ruben_El_Tree:SetName("El_Tree")
g_pButton_Ruben_El_Tree:AddDummyInt(DUNGEON_ID.DI_RUBEN_EL_TREE_NORMAL)
g_pButton_Ruben_El_Tree:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Ruben_El_Tree:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Ruben_El_Tree:SetNormalTex("DLG_Common_Button00_NEW.tga", "El_tree_N")
IconAni(g_pButton_Ruben_El_Tree, "DLG_Common_Button00_NEW.tga", "El_tree_O")
g_pButton_Ruben_El_Tree:SetDownTex("DLG_Common_Button00_NEW.tga", "El_tree_O")
g_pButton_Ruben_El_Tree:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(240, 42)
})
g_pButton_Ruben_El_Tree:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(234, 34)
})
g_pButton_Ruben_El_Tree:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(236, 36)
})
g_pButton_Ruben_El_Tree:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Ruben_El_Tree:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Ruben_El_Tree:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Ruben_Ruin_Of_Elf = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Ruben_Ruin_Of_Elf)
g_pButton_Ruben_Ruin_Of_Elf:SetName("Ruben_Ruin_Of_Elf")
g_pButton_Ruben_Ruin_Of_Elf:AddDummyInt(DUNGEON_ID.DI_RUBEN_RUIN_OF_ELF_NORMAL)
g_pButton_Ruben_Ruin_Of_Elf:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Ruben_Ruin_Of_Elf:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Ruben_Ruin_Of_Elf:SetNormalTex("DLG_Common_Button00_NEW.tga", "Ruin_N")
IconAni(g_pButton_Ruben_Ruin_Of_Elf, "DLG_Common_Button00_NEW.tga", "Ruin_O")
g_pButton_Ruben_Ruin_Of_Elf:SetDownTex("DLG_Common_Button00_NEW.tga", "Ruin_O")
g_pButton_Ruben_Ruin_Of_Elf:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(103, 212)
})
g_pButton_Ruben_Ruin_Of_Elf:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(96, 207)
})
g_pButton_Ruben_Ruin_Of_Elf:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(98, 209)
})
g_pButton_Ruben_Ruin_Of_Elf:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Ruben_Ruin_Of_Elf:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Ruben_Ruin_Of_Elf:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Ruben_Swap = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Ruben_Swap)
g_pButton_Ruben_Swap:SetName("Ruben_Swap")
g_pButton_Ruben_Swap:AddDummyInt(DUNGEON_ID.DI_RUBEN_SWAMP_NORMAL)
g_pButton_Ruben_Swap:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Ruben_Swap:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Ruben_Swap:SetNormalTex("DLG_Common_Button00_NEW.tga", "Swamp_N")
IconAni(g_pButton_Ruben_Swap, "DLG_Common_Button00_NEW.tga", "Swamp_O")
g_pButton_Ruben_Swap:SetDownTex("DLG_Common_Button00_NEW.tga", "Swamp_O")
g_pButton_Ruben_Swap:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(395, 282)
})
g_pButton_Ruben_Swap:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(391, 276)
})
g_pButton_Ruben_Swap:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(393, 278)
})
g_pButton_Ruben_Swap:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Ruben_Swap:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Ruben_Swap:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("루벤_타이틀 모음")

g_pPictureTitleBG_Title_01 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_01)
g_pPictureTitleBG_Title_01:SetTex("DLG_Localmap_Title_01.tga", "Title_Ruben_01")
g_pPictureTitleBG_Title_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(269, 129)
})

g_pPictureTitleBG_Title_02 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_02)
g_pPictureTitleBG_Title_02:SetTex("DLG_Localmap_Title_01.tga", "Title_Ruben_02")
g_pPictureTitleBG_Title_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(124, 286)
})

g_pPictureTitleBG_Title_03 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_03)
g_pPictureTitleBG_Title_03:SetTex("DLG_Localmap_Title_01.tga", "Title_Ruben_03")
g_pPictureTitleBG_Title_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(418, 330)
})
