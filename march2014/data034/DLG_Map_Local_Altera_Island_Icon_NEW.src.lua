--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Altera_Island_Icon_NEW.lua (live data217 copy, 10141 B), adopted wholesale.
-- June dropped the clear-difficulty stars, the Henir button (moved to the Special map)
-- and switched the secret dungeon to its _HELL ID with a SecretDungeonDesc lock button;
-- all of that is kept. Each button's third AddDummyInt is the June Hero dungeon ID,
-- which March has no enum for, so it reads 0 (DI_NONE, as June's own Ruben file uses);
-- CX2LocalMapUI only reads it in hero mode. Controls naming June-only content are
-- omitted below with a note where each stood.
--}}

g_pButton_Altera_Battleship = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Altera_Battleship)
g_pButton_Altera_Battleship:SetName("Dungeon")
g_pButton_Altera_Battleship:AddDummyInt(DUNGEON_ID.DI_ALTERA_BATTLE_AIR_SHIP_NORMAL)
g_pButton_Altera_Battleship:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Altera_Battleship:AddDummyInt(DUNGEON_ID.DI_ALTERA_BATTLE_AIR_SHIP_HERO_NORMAL)
g_pButton_Altera_Battleship:SetNormalTex("DLG_Common_Button00_NEW.tga", "Altera_ship")
g_pButton_Altera_Battleship:SetOverTex("DLG_Common_Button00_NEW.tga", "Altera_Ship_Over")
IconAni(g_pButton_Altera_Battleship, "DLG_Common_Button00_NEW.tga", "Altera_Ship_Over")
g_pButton_Altera_Battleship:SetDownTex("DLG_Common_Button00_NEW.tga", "Altera_Ship_Over")
g_pButton_Altera_Battleship:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(67, 162),
	LEFT_TOP_BR = D3DXVECTOR2(68, 162),
	LEFT_TOP_CN = D3DXVECTOR2(68, 162)
})
g_pButton_Altera_Battleship:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(61, 152),
	LEFT_TOP_BR = D3DXVECTOR2(62, 152),
	LEFT_TOP_CN = D3DXVECTOR2(62, 152)
})
g_pButton_Altera_Battleship:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(63, 154),
	LEFT_TOP_BR = D3DXVECTOR2(64, 154),
	LEFT_TOP_CN = D3DXVECTOR2(64, 154)
})
g_pButton_Altera_Battleship:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Altera_Battleship:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Altera_Battleship:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Altera_Plain_Recycle = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Altera_Plain_Recycle)
g_pButton_Altera_Plain_Recycle:SetName("Dungeon")
g_pButton_Altera_Plain_Recycle:AddDummyInt(DUNGEON_ID.DI_ALTERA_PLAIN_RECYCLE_NORMAL)
g_pButton_Altera_Plain_Recycle:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Altera_Plain_Recycle:AddDummyInt(DUNGEON_ID.DI_ALTERA_PLAIN_RECYCLE_HERO_NORMAL)
g_pButton_Altera_Plain_Recycle:SetNormalTex("DLG_Common_Button00_NEW.tga", "ALTERA_PLAIN_RECYCLE_NORMAL")
g_pButton_Altera_Plain_Recycle:SetOverTex("DLG_Common_Button00_NEW.tga", "ALTERA_PLAIN_RECYCLE_OVER")
IconAni(g_pButton_Altera_Plain_Recycle, "DLG_Common_Button00_NEW.tga", "ALTERA_PLAIN_RECYCLE_OVER")
g_pButton_Altera_Plain_Recycle:SetDownTex("DLG_Common_Button00_NEW.tga", "ALTERA_PLAIN_RECYCLE_OVER")
g_pButton_Altera_Plain_Recycle:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(152, 74)
})
g_pButton_Altera_Plain_Recycle:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(146, 68)
})
g_pButton_Altera_Plain_Recycle:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(148, 70)
})
g_pButton_Altera_Plain_Recycle:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Altera_Plain_Recycle:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Altera_Plain_Recycle:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Altera_Below_Tunnel = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Altera_Below_Tunnel)
g_pButton_Altera_Below_Tunnel:SetName("Dungeon")
g_pButton_Altera_Below_Tunnel:AddDummyInt(DUNGEON_ID.DI_ALTERA_BELOW_TUNNEL_NORMAL)
g_pButton_Altera_Below_Tunnel:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Altera_Below_Tunnel:AddDummyInt(DUNGEON_ID.DI_ALTERA_BELOW_TUNNEL_HERO_NORMAL)
g_pButton_Altera_Below_Tunnel:SetNormalTex("DLG_Common_Button00_NEW.tga", "ALTERA_BELOW_TUNNEL_NORMAL")
g_pButton_Altera_Below_Tunnel:SetOverTex("DLG_Common_Button00_NEW.tga", "ALTERA_Below_Tunnel_OVER")
IconAni(g_pButton_Altera_Below_Tunnel, "DLG_Common_Button00_NEW.tga", "ALTERA_BELOW_TUNNEL_OVER")
g_pButton_Altera_Below_Tunnel:SetDownTex("DLG_Common_Button00_NEW.tga", "ALTERA_BELOW_TUNNEL_OVER")
g_pButton_Altera_Below_Tunnel:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(285, 106)
})
g_pButton_Altera_Below_Tunnel:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(277, 100)
})
g_pButton_Altera_Below_Tunnel:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(279, 102)
})
g_pButton_Altera_Below_Tunnel:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Altera_Below_Tunnel:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Altera_Below_Tunnel:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Altera_PLAIN = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Altera_PLAIN)
g_pButton_Altera_PLAIN:SetName("Dungeon")
g_pButton_Altera_PLAIN:AddDummyInt(DUNGEON_ID.DI_ALTERA_PLAIN_NORMAL)
g_pButton_Altera_PLAIN:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Altera_PLAIN:AddDummyInt(DUNGEON_ID.DI_ALTERA_PLAIN_HERO_NORMAL)
g_pButton_Altera_PLAIN:SetNormalTex("DLG_Common_Button00_NEW.tga", "ALTERA_PLAIN_NORMAL")
g_pButton_Altera_PLAIN:SetOverTex("DLG_Common_Button00_NEW.tga", "ALTERA_PLAIN_OVER")
IconAni(g_pButton_Altera_PLAIN, "DLG_Common_Button00_NEW.tga", "ALTERA_PLAIN_OVER")
g_pButton_Altera_PLAIN:SetDownTex("DLG_Common_Button00_NEW.tga", "ALTERA_PLAIN_OVER")
g_pButton_Altera_PLAIN:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(366, 50)
})
g_pButton_Altera_PLAIN:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(360, 44)
})
g_pButton_Altera_PLAIN:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(362, 46)
})
g_pButton_Altera_PLAIN:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Altera_PLAIN:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Altera_PLAIN:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Nasod_Factory = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Nasod_Factory)
g_pButton_Nasod_Factory:SetName("Dungeon")
g_pButton_Nasod_Factory:AddDummyInt(DUNGEON_ID.DI_ALTERA_NASOD_FACTORY_NORMAL)
g_pButton_Nasod_Factory:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Nasod_Factory:AddDummyInt(DUNGEON_ID.DI_ALTERA_NASOD_FACTORY_HERO_NORMAL)
g_pButton_Nasod_Factory:SetNormalTex("DLG_Common_Button01_NEW.tga", "NASOD_FACTORY_NORMAL")
g_pButton_Nasod_Factory:SetOverTex("DLG_Common_Button01_NEW.tga", "NASOD_FACTORY_OVER")
IconAni(g_pButton_Nasod_Factory, "DLG_Common_Button01_NEW.tga", "NASOD_FACTORY_OVER")
g_pButton_Nasod_Factory:SetDownTex("DLG_Common_Button01_NEW.tga", "NASOD_FACTORY_OVER")
g_pButton_Nasod_Factory:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(448, 178)
})
g_pButton_Nasod_Factory:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(442, 172)
})
g_pButton_Nasod_Factory:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(444, 174)
})
g_pButton_Nasod_Factory:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Nasod_Factory:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Nasod_Factory:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Altera_Core = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Altera_Core)
g_pButton_Altera_Core:SetName("Dungeon")
g_pButton_Altera_Core:AddDummyInt(DUNGEON_ID.DI_ALTERA_CORE_NORMAL)
g_pButton_Altera_Core:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Altera_Core:AddDummyInt(DUNGEON_ID.DI_ALTERA_CORE_HERO_NORMAL)
g_pButton_Altera_Core:SetNormalTex("DLG_Common_Button01_NEW.tga", "Altera_Core_NORMAL")
g_pButton_Altera_Core:SetOverTex("DLG_Common_Button01_NEW.tga", "Altera_Core_OVER")
IconAni(g_pButton_Altera_Core, "DLG_Common_Button01_NEW.tga", "Altera_Core_OVER")
g_pButton_Altera_Core:SetDownTex("DLG_Common_Button01_NEW.tga", "Altera_Core_OVER")
g_pButton_Altera_Core:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(322, 235)
})
g_pButton_Altera_Core:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(316, 229)
})
g_pButton_Altera_Core:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(318, 231)
})
g_pButton_Altera_Core:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Altera_Core:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Altera_Core:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Altera_Secret_Dungeon = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Altera_Secret_Dungeon)
g_pButton_Altera_Secret_Dungeon:SetName("Dungeon")
g_pButton_Altera_Secret_Dungeon:AddDummyInt(DUNGEON_ID.DI_ALTERA_SECRET_HELL)
g_pButton_Altera_Secret_Dungeon:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Altera_Secret_Dungeon:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Altera_Secret_Dungeon:SetNormalTex("DLG_Common_Button03_NEW.tga", "ALTERA_SECRET_DUNGEON_NORMAL")
g_pButton_Altera_Secret_Dungeon:SetOverTex("DLG_Common_Button03_NEW.tga", "ALTERA_SECRET_DUNGEON_OVER")
IconAni(g_pButton_Altera_Secret_Dungeon, "DLG_Common_Button03_NEW.tga", "ALTERA_SECRET_DUNGEON_OVER")
g_pButton_Altera_Secret_Dungeon:SetDownTex("DLG_Common_Button03_NEW.tga", "ALTERA_SECRET_DUNGEON_OVER")
g_pButton_Altera_Secret_Dungeon:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(184, 236)
})
g_pButton_Altera_Secret_Dungeon:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(184, 236)
})
g_pButton_Altera_Secret_Dungeon:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(186, 238)
})
g_pButton_Altera_Secret_Dungeon:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Altera_Secret_Dungeon:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Altera_Secret_Dungeon:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

g_pButton_Altera_Secret_Dungeon_Desc = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Altera_Secret_Dungeon_Desc)
g_pButton_Altera_Secret_Dungeon_Desc:SetName("SecretDungeonDesc")
g_pButton_Altera_Secret_Dungeon_Desc:AddDummyInt(DUNGEON_ID.DI_ALTERA_SECRET_HELL)
g_pButton_Altera_Secret_Dungeon_Desc:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_Altera_Secret_Dungeon_Desc:AddDummyInt(DUNGEON_ID.DI_NONE)
g_pButton_Altera_Secret_Dungeon_Desc:SetNormalTex("DLG_Common_Button05_NEW.tga", "ALTERA_SECRET_DUNGEON_LOCK")
g_pButton_Altera_Secret_Dungeon_Desc:SetOverTex("DLG_Common_Button05_NEW.tga", "ALTERA_SECRET_DUNGEON_LOCK")
g_pButton_Altera_Secret_Dungeon_Desc:SetDownTex("DLG_Common_Button05_NEW.tga", "ALTERA_SECRET_DUNGEON_LOCK")
g_pButton_Altera_Secret_Dungeon_Desc:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(184, 236)
})
g_pButton_Altera_Secret_Dungeon_Desc:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(184, 236)
})
g_pButton_Altera_Secret_Dungeon_Desc:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(184, 236)
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
	LEFT_TOP = D3DXVECTOR2(17, 280)
})
g_pButton_Event_Dummy01:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(17, 280)
})
g_pButton_Event_Dummy01:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(19, 282)
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
	LEFT_TOP = D3DXVECTOR2(450, 280)
})
g_pButton_Event_Dummy02:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(450, 280)
})
g_pButton_Event_Dummy02:SetDownPoint({
	ADD_SIZE_X = -4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -4,
	LEFT_TOP = D3DXVECTOR2(452, 282)
})
g_pButton_Event_Dummy02:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_CLICKED)
g_pButton_Event_Dummy02:SetCustomMsgMouseOver(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OVER)
g_pButton_Event_Dummy02:SetCustomMsgMouseOut(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_MOUSE_OUT)

-- [June UI parity Phase 3] omitted: g_pButtonBastera (June-only Bestera content: DI_BESTERA_START and TITLE_Bestera are not in March).

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("알테라_타이틀 모음")

g_pPictureTitleBG_Title_01 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_01)
g_pPictureTitleBG_Title_01:SetTex("DLG_Localmap_Title_01.tga", "Title_Altera_01")
g_pPictureTitleBG_Title_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(88, 216)
})

g_pPictureTitleBG_Title_02 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_02)
g_pPictureTitleBG_Title_02:SetTex("DLG_Localmap_Title_01.tga", "Title_Altera_02")
g_pPictureTitleBG_Title_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(169, 138)
})

g_pPictureTitleBG_Title_03 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_03)
g_pPictureTitleBG_Title_03:SetTex("DLG_Localmap_Title_01.tga", "Title_Altera_03")
g_pPictureTitleBG_Title_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(303, 157)
})

g_pPictureTitleBG_Title_04 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_04)
g_pPictureTitleBG_Title_04:SetTex("DLG_Localmap_Title_01.tga", "Title_Altera_04")
g_pPictureTitleBG_Title_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(383, 113)
})

g_pPictureTitleBG_Title_05 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_05)
g_pPictureTitleBG_Title_05:SetTex("DLG_Localmap_Title_01.tga", "Title_Altera_05")
g_pPictureTitleBG_Title_05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(464, 229)
})

g_pPictureTitleBG_Title_06 = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_06)
g_pPictureTitleBG_Title_06:SetTex("DLG_Localmap_Title_01.tga", "Title_Altera_06")
g_pPictureTitleBG_Title_06:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(344, 301)
})

-- [June UI parity Phase 3] omitted: g_pButton_TitleBG (June-only Bestera content: DI_BESTERA_START and TITLE_Bestera are not in March).

g_pButton_TitleBG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_TitleBG)
g_pButton_TitleBG:SetName("TITLE_SECRET")
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_ALTERA_SECRET_HELL)
g_pButton_TitleBG:AddDummyInt(DETAIL_LOCAL_TYPE.DLT_DUNGEON)
g_pButton_TitleBG:AddDummyInt(DUNGEON_ID.DI_NONE)

g_pPictureTitleBG_Title_Secret = g_pUIDialog:CreatePicture()

g_pButton_TitleBG:AddPicture(g_pPictureTitleBG_Title_Secret)
g_pPictureTitleBG_Title_Secret:SetTex("DLG_Localmap_Title_01.tga", "Title_Altera_Secret")
g_pPictureTitleBG_Title_Secret:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(206, 303)
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
	LEFT_TOP = D3DXVECTOR2(37, 335)
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
	LEFT_TOP = D3DXVECTOR2(470, 335)
})

g_pButton_STAR9 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR9)
g_pButton_STAR9:SetNameStrID(STR_ID_4979)

g_pPictureSTAR9_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR9:AddPicture(g_pPictureSTAR9_StarBG)
g_pPictureSTAR9_StarBG:SetTex("DLG_Common_Button00_NEW.tga", "Star_Bg")
g_pPictureSTAR9_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(490, 357)
})
g_pButton_STAR9:SetShow(false)

g_pButton_STAR10 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pButton_STAR10)
g_pButton_STAR10:SetNameStrID(STR_ID_3550)

g_pPictureSTAR10_StarBG = g_pUIDialog:CreatePicture()

g_pButton_STAR10:AddPicture(g_pPictureSTAR10_StarBG)
g_pPictureSTAR10_StarBG:SetTex("DLG_Common_Button00_NEW.tga", "Star_Bg")
g_pPictureSTAR10_StarBG:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(196, 326)
})
g_pButton_STAR10:SetShow(false)
