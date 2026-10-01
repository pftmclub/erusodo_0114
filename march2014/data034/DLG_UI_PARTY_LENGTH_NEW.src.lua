--{{ Iruha : 2026-10-01 // June UI parity Phase 5 (SERV_IRUHADEV_JUNE_PARTY_UI)
-- June2017 DLG_UI_PARTY_LENGTH_NEW.lua (data034, 21712 B), adopted. New over
-- March: VSFOUR, HELL_HERO and ButtonErdaHelp (June's difficulty functions need
-- all three present and hide them for March's dungeon types), and Dungeon_Zone
-- pictures 8..11 (8 = RANOX, DLG_UI_Common_Texture_Field_00, June data057).
-- June-only controls are omitted with a note where each stood.
--}}

g_pUIDialog:SetName("DLG_UI_Party")
g_pUIDialog:SetPos(0, 0)
g_pUIDialog:SetModal(true)
g_pUIDialog:SetFront(true)
g_pUIDialog:SetLayer(X2_DIALOG_LAYER.XDL_POP_UP)

g_pStaticParty = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticParty)
g_pStaticParty:SetName("Party")

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticParty:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "DUNGEON_BG_TOP")
g_pPicture_bg:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 131)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticParty:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "DUNGEON_BG_MIDDLE")
g_pPicture_bg:SetPoint({
	ADD_SIZE_Y = 356,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 169)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticParty:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "DUNGEON_BG_BOTTOM")
g_pPicture_bg:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 530)
})

g_pPicture_bg2 = g_pUIDialog:CreatePicture()

g_pStaticParty:AddPicture(g_pPicture_bg2)
g_pPicture_bg2:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BOX_BG_TOP")
g_pPicture_bg2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(805, 452)
})

g_pPicture_bg2 = g_pUIDialog:CreatePicture()

g_pStaticParty:AddPicture(g_pPicture_bg2)
g_pPicture_bg2:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BOX_BG_MIDDLE")
g_pPicture_bg2:SetPoint({
	ADD_SIZE_Y = 65,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(805, 459)
})

g_pPicture_bg2 = g_pUIDialog:CreatePicture()

g_pStaticParty:AddPicture(g_pPicture_bg2)
g_pPicture_bg2:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BOX_BG_BOTTOM")
g_pPicture_bg2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(805, 526)
})

-- June's Static_HeroDungeonTitle (DLG_UI_Text_01 "Name_Hero") is omitted here:
-- it labels Hero dungeons, which March does not have.

g_pButtonX = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonX)
g_pButtonX:SetName("Close_Window")
g_pButtonX:SetNormalTex("DLG_UI_BUTTON19_NEW.tga", "X_NORMAL")
g_pButtonX:SetOverTex("DLG_UI_BUTTON19_NEW.tga", "X_OVER")
g_pButtonX:SetDownTex("DLG_UI_BUTTON19_NEW.tga", "X_OVER")
g_pButtonX:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(992, 140)
})
g_pButtonX:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(992, 140)
})
g_pButtonX:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(993, 141)
})
g_pButtonX:SetCustomMsgMouseUp(PARTY_UI_CUSTOM_MSG.PUCM_PARTY_CLOSE_WINDOW)

g_pButton_Help = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Help)
g_pButton_Help:SetName("Button_Help")
g_pButton_Help:SetNormalTex("DLG_UI_BUTTON19_NEW.tga", "Help_Normal")
g_pButton_Help:SetOverTex("DLG_UI_BUTTON19_NEW.tga", "Help_Over")
g_pButton_Help:SetDownTex("DLG_UI_BUTTON19_NEW.tga", "Help_Over")
g_pButton_Help:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(970, 140)
})
g_pButton_Help:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(970, 140)
})
g_pButton_Help:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(971, 141)
})
g_pButton_Help:SetCustomMsgMouseUp(STATE_MENU_UI_CUSTOM_MSG.SMUCM_OPEN_ELLIOS_GUIDE_GO_TO_PAGE)
g_pButton_Help:AddDummyInt(10)

g_pStaticimage = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticimage)
g_pStaticimage:SetName("image")

g_pPicture_DUNGEON_MAP_Bg1 = g_pUIDialog:CreatePicture()

g_pStaticimage:AddPicture(g_pPicture_DUNGEON_MAP_Bg1)
g_pPicture_DUNGEON_MAP_Bg1:SetTex("DLG_UI_Common_Texture49_NEW.tga", "IMAGE_BG")
g_pPicture_DUNGEON_MAP_Bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_DUNGEON_MAP_BUTTON = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pPicture_DUNGEON_MAP_BUTTON)
g_pPicture_DUNGEON_MAP_BUTTON:SetName("Click_map")
g_pPicture_DUNGEON_MAP_BUTTON:SetNormalTex("DLG_Common_Button00.tga", "invisible")
g_pPicture_DUNGEON_MAP_BUTTON:SetOverTex("DLG_Common_Button00.tga", "invisible")
g_pPicture_DUNGEON_MAP_BUTTON:SetDownTex("DLG_Common_Button00.tga", "invisible")
g_pPicture_DUNGEON_MAP_BUTTON:SetNormalPoint({
	ADD_SIZE_X = 175,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 107,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})
g_pPicture_DUNGEON_MAP_BUTTON:SetOverPoint({
	ADD_SIZE_X = 175,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 107,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})
g_pPicture_DUNGEON_MAP_BUTTON:SetDownPoint({
	ADD_SIZE_X = 175,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 107,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})
g_pPicture_DUNGEON_MAP_BUTTON:SetCustomMsgMouseUp(PARTY_UI_CUSTOM_MSG.PUCM_PARTY_OPEN_LOCAL_MAP)

g_pStaticDungeon_Zone = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticDungeon_Zone)
g_pStaticDungeon_Zone:SetName("Dungeon_Zone")

g_pPicture_Ruben = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Ruben)
g_pPicture_Ruben:SetTex("DLG_UI_Common_Texture07.TGA", "ruben_out")
g_pPicture_Ruben:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Eldu = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Eldu)
g_pPicture_Eldu:SetTex("DLG_UI_Common_Texture07.TGA", "eldu_out")
g_pPicture_Eldu:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Besma = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Besma)
g_pPicture_Besma:SetTex("DLG_UI_Common_Texture07.TGA", "besma_out")
g_pPicture_Besma:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Altera = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Altera)
g_pPicture_Altera:SetTex("DLG_UI_Common_Texture07.TGA", "altera_out")
g_pPicture_Altera:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Peita = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Peita)
g_pPicture_Peita:SetTex("DLG_UI_Common_Texture07.TGA", "peita_out")
g_pPicture_Peita:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Velder = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Velder)
g_pPicture_Velder:SetTex("DLG_UI_Common_Texture07.TGA", "velder_capital")
g_pPicture_Velder:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Hamel_Ship = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Hamel_Ship)
g_pPicture_Hamel_Ship:SetTex("DLG_UI_Common_Texture07_A.TGA", "Hamel_CAPITAL")
g_pPicture_Hamel_Ship:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Sander = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Sander)
g_pPicture_Sander:SetTex("DLG_UI_Common_Texture_Field_00.tga", "SANDER")
g_pPicture_Sander:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Ranox = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Ranox)
g_pPicture_Ranox:SetTex("DLG_UI_Common_Texture_Field_00.tga", "RANOX")
g_pPicture_Ranox:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

-- Pictures 8..11 of Dungeon_Zone are picked by local-map ID (LMI_RUBEN + N).
-- 9 (ELYSION) and 10 (USUAL_QUEST) belong to June-only maps and never show; 9's
-- key is not in March's DLG_Localmap_Title_01 and is kept only to hold the index.
g_pPicture_Ranox = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Ranox)
g_pPicture_Ranox:SetTex("DLG_Localmap_Title_01.tga", "ELYSION")
g_pPicture_Ranox:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Ranox = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Ranox)
g_pPicture_Ranox:SetTex("DLG_UI_Common_Texture07.tga", "USUAL_QUEST")
g_pPicture_Ranox:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pPicture_Ranox = g_pUIDialog:CreatePicture()

g_pStaticDungeon_Zone:AddPicture(g_pPicture_Ranox)
g_pPicture_Ranox:SetTex("DLG_UI_Common_Texture07.tga", "SPECIAL_QUEST")
g_pPicture_Ranox:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 162)
})

g_pStaticLocalQuest = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticLocalQuest)
g_pStaticLocalQuest:SetName("StaticLocalQuest")

g_pPictureQuest = g_pUIDialog:CreatePicture()

g_pStaticLocalQuest:AddPicture(g_pPictureQuest)
g_pPictureQuest:SetTex("DLG_UI_Common_Texture13.tga", "Quset_Notice_Small")
g_pPictureQuest:SetPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 186)
})

g_pCheckBox_LEVEL = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_LEVEL)
g_pCheckBox_LEVEL:SetName("NORMAL")
g_pCheckBox_LEVEL:SetNormalTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_EASY_INACTIVITY")
g_pCheckBox_LEVEL:SetOverTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_EASY_NORMAL")
g_pCheckBox_LEVEL:SetCheckedTex("DLG_UI_BUTTON20.TGA", "BT_LEVEL_EASY_OVER")
g_pCheckBox_LEVEL:SetDisableTex("DLG_Common_Button00.tga", "invisible")
g_pCheckBox_LEVEL:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(802, 277)
})
g_pCheckBox_LEVEL:SetCheckPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 278)
})
g_pCheckBox_LEVEL:AddDummyInt(DIFFICULTY_LEVEL.DL_NORMAL)
g_pCheckBox_LEVEL:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_CHECKED)
g_pCheckBox_LEVEL:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_UNCHECKED)

g_pCheckBox_LEVEL = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_LEVEL)
g_pCheckBox_LEVEL:SetName("HARD")
g_pCheckBox_LEVEL:SetNormalTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_HARD_INACTIVITY")
g_pCheckBox_LEVEL:SetOverTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_HARD_NORMAL")
g_pCheckBox_LEVEL:SetCheckedTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_HARD_OVER")
g_pCheckBox_LEVEL:SetDisableTex("DLG_Common_Button00.tga", "invisible")
g_pCheckBox_LEVEL:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(870, 277)
})
g_pCheckBox_LEVEL:SetCheckPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(871, 278)
})
g_pCheckBox_LEVEL:AddDummyInt(DIFFICULTY_LEVEL.DL_HARD)
g_pCheckBox_LEVEL:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_CHECKED)
g_pCheckBox_LEVEL:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_UNCHECKED)

g_pCheckBox_LEVEL = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_LEVEL)
g_pCheckBox_LEVEL:SetName("EXPERT")
g_pCheckBox_LEVEL:SetNormalTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_VERYHARD_INACTIVITY")
g_pCheckBox_LEVEL:SetOverTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_VERYHARD_NORMAL")
g_pCheckBox_LEVEL:SetCheckedTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_VERYHARD_OVER")
g_pCheckBox_LEVEL:SetDisableTex("DLG_Common_Button00.tga", "invisible")
g_pCheckBox_LEVEL:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(938, 277)
})
g_pCheckBox_LEVEL:SetCheckPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(939, 278)
})
g_pCheckBox_LEVEL:AddDummyInt(DIFFICULTY_LEVEL.DL_EXPERT)
g_pCheckBox_LEVEL:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_CHECKED)
g_pCheckBox_LEVEL:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_UNCHECKED)

g_pCheckBox_LEVEL = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_LEVEL)
g_pCheckBox_LEVEL:SetName("NORMAL_EX")
g_pCheckBox_LEVEL:SetNormalTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_HENIR_INACTIVITY")
g_pCheckBox_LEVEL:SetOverTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_HENIR_NORMAL")
g_pCheckBox_LEVEL:SetCheckedTex("DLG_UI_BUTTON20.TGA", "BT_LEVEL_HENIR_OVER")
g_pCheckBox_LEVEL:SetDisableTex("DLG_Common_Button00.tga", "invisible")
g_pCheckBox_LEVEL:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(802, 277)
})
g_pCheckBox_LEVEL:SetCheckPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 278)
})
g_pCheckBox_LEVEL:AddDummyInt(DIFFICULTY_LEVEL.DL_NORMAL)
g_pCheckBox_LEVEL:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_CHECKED)
g_pCheckBox_LEVEL:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_UNCHECKED)

g_pCheckBox_LEVEL = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_LEVEL)
g_pCheckBox_LEVEL:SetName("HELL")
g_pCheckBox_LEVEL:SetNormalTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_SECRETDUNGEON_HELL_INACTIVITY")
g_pCheckBox_LEVEL:SetOverTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_SECRETDUNGEON_HELL_NORMAL")
g_pCheckBox_LEVEL:SetCheckedTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_SECRETDUNGEON_HELL_OVER")
g_pCheckBox_LEVEL:SetDisableTex("DLG_Common_Button00.tga", "invisible")
g_pCheckBox_LEVEL:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(905, 277)
})
g_pCheckBox_LEVEL:SetCheckPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(906, 278)
})
g_pCheckBox_LEVEL:AddDummyInt(DIFFICULTY_LEVEL.DL_EXPERT)
g_pCheckBox_LEVEL:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_CHECKED)
g_pCheckBox_LEVEL:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_UNCHECKED)

g_pCheckBox_LEVEL = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_LEVEL)
g_pCheckBox_LEVEL:SetName("HELL_HERO")
g_pCheckBox_LEVEL:SetNormalTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_SECRETDUNGEON_HELL_INACTIVITY")
g_pCheckBox_LEVEL:SetOverTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_SECRETDUNGEON_HELL_NORMAL")
g_pCheckBox_LEVEL:SetCheckedTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_SECRETDUNGEON_HELL_OVER")
g_pCheckBox_LEVEL:SetDisableTex("DLG_Common_Button00.tga", "invisible")
g_pCheckBox_LEVEL:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(905, 277)
})
g_pCheckBox_LEVEL:SetCheckPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(906, 278)
})
g_pCheckBox_LEVEL:AddDummyInt(DIFFICULTY_LEVEL.DL_HARD)
g_pCheckBox_LEVEL:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_CHECKED)
g_pCheckBox_LEVEL:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_UNCHECKED)

g_pCheckBox_LEVEL = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_LEVEL)
g_pCheckBox_LEVEL:SetName("CHALLENGE")
g_pCheckBox_LEVEL:SetNormalTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_HENIR_CHALLENGE_INACTIVITY")
g_pCheckBox_LEVEL:SetOverTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_HENIR_CHALLENGE_NORMAL")
g_pCheckBox_LEVEL:SetCheckedTex("DLG_UI_BUTTON20.tga", "BT_LEVEL_HENIR_CHALLENGE_OVER")
g_pCheckBox_LEVEL:SetDisableTex("DLG_Common_Button00.tga", "invisible")
g_pCheckBox_LEVEL:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(905, 277)
})
g_pCheckBox_LEVEL:SetCheckPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(906, 278)
})
g_pCheckBox_LEVEL:AddDummyInt(DIFFICULTY_LEVEL.DL_EXPERT)
g_pCheckBox_LEVEL:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_CHECKED)
g_pCheckBox_LEVEL:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_UNCHECKED)

g_pCheckBox_LEVEL = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_LEVEL)
g_pCheckBox_LEVEL:SetName("VSFOUR")
g_pCheckBox_LEVEL:SetNormalTex("DLG_UI_Button21.tga", "BT_Pve4_Inactivity")
g_pCheckBox_LEVEL:SetOverTex("DLG_UI_Button21.tga", "BT_Pve4_N")
g_pCheckBox_LEVEL:SetCheckedTex("DLG_UI_Button21.tga", "BT_Pve4_O")
g_pCheckBox_LEVEL:SetDisableTex("DLG_Common_Button00.tga", "invisible")
g_pCheckBox_LEVEL:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(802, 277)
})
g_pCheckBox_LEVEL:SetCheckPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(803, 278)
})
g_pCheckBox_LEVEL:AddDummyInt(DIFFICULTY_LEVEL.DL_NORMAL)
g_pCheckBox_LEVEL:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_CHECKED)
g_pCheckBox_LEVEL:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_DIFFICULTY_UNCHECKED)

g_pButton_Erda_2017_Help = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Erda_2017_Help)
g_pButton_Erda_2017_Help:SetName("ButtonErdaHelp")
g_pButton_Erda_2017_Help:SetNormalTex("DLG_UI_Text_05.tga", "BT_Guidebook_N")
g_pButton_Erda_2017_Help:SetOverTex("DLG_UI_Text_05.tga", "BT_Guidebook_O")
g_pButton_Erda_2017_Help:SetDownTex("DLG_UI_Text_05.tga", "BT_Guidebook_D")
g_pButton_Erda_2017_Help:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(902, 277)
})
g_pButton_Erda_2017_Help:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(902, 277)
})
g_pButton_Erda_2017_Help:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(902, 277)
})
g_pButton_Erda_2017_Help:SetCustomMsgMouseUp(STATE_MENU_UI_CUSTOM_MSG.SMUCM_OPEN_ELLIOS_GUIDE_GO_TO_PAGE)
g_pButton_Erda_2017_Help:AddDummyInt(60)

g_pStaticuser = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticuser)
g_pStaticuser:SetName("need_user")
g_pStaticuser:AddString({
	MSG = "적정인원",
	FONT_INDEX = XUF_DODUM_11_NORMAL,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(815, 304),
	COLOR = D3DXCOLOR(0.3, 0.3, 0.3, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStaticuser:AddString({
	MSG = "1",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_RIGHT,
	POS = D3DXVECTOR2(982, 304),
	COLOR = D3DXCOLOR(0.3, 0.3, 0.3, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0.9)
})

g_pStaticlevel = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticlevel)
g_pStaticlevel:SetName("need_level")
g_pStaticlevel:AddString({
	MSG = "적정레벨",
	FONT_INDEX = XUF_DODUM_11_NORMAL,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(815, 320),
	COLOR = D3DXCOLOR(0.3, 0.3, 0.3, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStaticlevel:AddString({
	MSG = "1",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_RIGHT,
	POS = D3DXVECTOR2(982, 320),
	POS = D3DXVECTOR2(982, 320),
	COLOR = D3DXCOLOR(0.3, 0.3, 0.3, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0.9)
})

g_pStaticlevel = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticlevel)
g_pStaticlevel:SetName("AdequateItemLevel")
g_pStaticlevel:AddString({
	MSG = "적정아이템레벨",
	FONT_INDEX = XUF_DODUM_11_NORMAL,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(815, 336),
	COLOR = D3DXCOLOR(0.3, 0.3, 0.3, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStaticlevel:AddString({
	MSG = "1",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_RIGHT,
	POS = D3DXVECTOR2(982, 336),
	POS = D3DXVECTOR2(982, 336),
	COLOR = D3DXCOLOR(0.3, 0.3, 0.3, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0.9)
})

g_pPicture_BAR1 = g_pUIDialog:CreatePicture()

g_pStaticlevel:AddPicture(g_pPicture_BAR1)
g_pPicture_BAR1:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BAR")
g_pPicture_BAR1:SetPoint({
	ADD_SIZE_X = 33,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(804, 300)
})

g_pPicture_BAR2 = g_pUIDialog:CreatePicture()

g_pStaticlevel:AddPicture(g_pPicture_BAR2)
g_pPicture_BAR2:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BAR")
g_pPicture_BAR2:SetPoint({
	ADD_SIZE_X = 33,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(804, 352)
})

g_pPicture_BAR3 = g_pUIDialog:CreatePicture()

g_pStaticlevel:AddPicture(g_pPicture_BAR3)
g_pPicture_BAR3:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BAR")
g_pPicture_BAR3:SetPoint({
	ADD_SIZE_X = 33,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(804, 396)
})

g_pStaticDungeonDesc = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticDungeonDesc)
g_pStaticDungeonDesc:SetName("DungeonDescription")
g_pStaticDungeonDesc:AddString({
	MSG = "던전 설명입니다. 던전설명은 꼭 세줄\n까지만 해주시면 감사합니다. 부탁드\n립니다요. 혹시 네줄은",
	FONT_INDEX = XUF_DODUM_11_NORMAL,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(815, 353),
	COLOR = D3DXCOLOR(0.3, 0.3, 0.3, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0, 0, 0, 0)
})

g_pStaticDungeonToolTipDesc = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pStaticDungeonToolTipDesc)
g_pStaticDungeonToolTipDesc:SetName("DungeonDescriptionToolTip")
g_pStaticDungeonToolTipDesc:SetNormalTex("DLG_Common_New_Texture03.tga", "invisible")
g_pStaticDungeonToolTipDesc:SetOverTex("DLG_Common_New_Texture03.tga", "invisible")
g_pStaticDungeonToolTipDesc:SetDownTex("DLG_Common_New_Texture03.tga", "invisible")
g_pStaticDungeonToolTipDesc:SetNormalPoint({
	ADD_SIZE_X = 200,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 45,
	LEFT_TOP = D3DXVECTOR2(805, 353)
})
g_pStaticDungeonToolTipDesc:SetOverPoint({
	ADD_SIZE_X = 200,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 45,
	LEFT_TOP = D3DXVECTOR2(804, 353)
})
g_pStaticDungeonToolTipDesc:SetDownPoint({
	ADD_SIZE_X = 200,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 45,
	LEFT_TOP = D3DXVECTOR2(805, 353)
})
g_pStaticDungeonToolTipDesc:SetGuideDesc(STR_ID_27)

g_pStatic_ITEM = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ITEM)
g_pStatic_ITEM:SetName("ITEM")
g_pStatic_ITEM:AddString({
	MSG = "기대 보상",
	FONT_INDEX = XUF_DODUM_11_NORMAL,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(807, 400),
	COLOR = D3DXCOLOR(0.1, 0.1, 0.1, 1),
	OUTLINE_COLOR = D3DXCOLOR(0, 0, 0, 0)
})

g_pPicture_ITEM_BG1 = g_pUIDialog:CreatePicture()

g_pStatic_ITEM:AddPicture(g_pPicture_ITEM_BG1)
g_pPicture_ITEM_BG1:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "ITEM_BG")
g_pPicture_ITEM_BG1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(808, 415)
})

g_pPicture_ITEM_BG2 = g_pUIDialog:CreatePicture()

g_pStatic_ITEM:AddPicture(g_pPicture_ITEM_BG2)
g_pPicture_ITEM_BG2:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "ITEM_BG")
g_pPicture_ITEM_BG2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(841, 415)
})

g_pPicture_ITEM_BG3 = g_pUIDialog:CreatePicture()

g_pStatic_ITEM:AddPicture(g_pPicture_ITEM_BG3)
g_pPicture_ITEM_BG3:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "ITEM_BG")
g_pPicture_ITEM_BG3:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(874, 415)
})

g_pPicture_ITEM_BG4 = g_pUIDialog:CreatePicture()

g_pStatic_ITEM:AddPicture(g_pPicture_ITEM_BG4)
g_pPicture_ITEM_BG4:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "ITEM_BG")
g_pPicture_ITEM_BG4:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(907, 415)
})

g_pPicture_ITEM_BG5 = g_pUIDialog:CreatePicture()

g_pStatic_ITEM:AddPicture(g_pPicture_ITEM_BG5)
g_pPicture_ITEM_BG5:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "ITEM_BG")
g_pPicture_ITEM_BG5:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(940, 415)
})

g_pPicture_ITEM_BG6 = g_pUIDialog:CreatePicture()

g_pStatic_ITEM:AddPicture(g_pPicture_ITEM_BG6)
g_pPicture_ITEM_BG6:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "ITEM_BG")
g_pPicture_ITEM_BG6:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(973, 415)
})

g_pButton_ITEM1 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ITEM1)
g_pButton_ITEM1:SetName("ITEM1")
g_pButton_ITEM1:SetNormalTex("DLG_Common_Button00.tga", "invisible")
g_pButton_ITEM1:SetOverTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM1:SetDownTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM1:SetNormalPoint({
	ADD_SIZE_X = 30,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 30,
	LEFT_TOP = D3DXVECTOR2(808, 415)
})
g_pButton_ITEM1:SetOverPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(808, 415)
})
g_pButton_ITEM1:SetDownPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(808, 415)
})
g_pButton_ITEM1:SetEnableClick(false)
g_pButton_ITEM1:SetCustomMsgMouseOver(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OVER)
g_pButton_ITEM1:SetCustomMsgMouseOut(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OUT)

g_pButton_ITEM2 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ITEM2)
g_pButton_ITEM2:SetName("ITEM2")
g_pButton_ITEM2:SetNormalTex("DLG_Common_Button00.tga", "invisible")
g_pButton_ITEM2:SetOverTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM2:SetDownTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM2:SetNormalPoint({
	ADD_SIZE_X = 30,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 30,
	LEFT_TOP = D3DXVECTOR2(841, 415)
})
g_pButton_ITEM2:SetOverPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(841, 415)
})
g_pButton_ITEM2:SetDownPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(841, 415)
})
g_pButton_ITEM2:SetEnableClick(false)
g_pButton_ITEM2:SetCustomMsgMouseOver(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OVER)
g_pButton_ITEM2:SetCustomMsgMouseOut(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OUT)

g_pButton_ITEM3 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ITEM3)
g_pButton_ITEM3:SetName("ITEM3")
g_pButton_ITEM3:SetNormalTex("DLG_Common_Button00.tga", "invisible")
g_pButton_ITEM3:SetOverTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM3:SetDownTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM3:SetNormalPoint({
	ADD_SIZE_X = 30,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 30,
	LEFT_TOP = D3DXVECTOR2(874, 415)
})
g_pButton_ITEM3:SetOverPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(874, 415)
})
g_pButton_ITEM3:SetDownPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(874, 415)
})
g_pButton_ITEM3:SetEnableClick(false)
g_pButton_ITEM3:SetCustomMsgMouseOver(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OVER)
g_pButton_ITEM3:SetCustomMsgMouseOut(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OUT)

g_pButton_ITEM4 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ITEM4)
g_pButton_ITEM4:SetName("ITEM4")
g_pButton_ITEM4:SetNormalTex("DLG_Common_Button00.tga", "invisible")
g_pButton_ITEM4:SetOverTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM4:SetDownTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM4:SetNormalPoint({
	ADD_SIZE_X = 30,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 30,
	LEFT_TOP = D3DXVECTOR2(907, 415)
})
g_pButton_ITEM4:SetOverPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(907, 415)
})
g_pButton_ITEM4:SetDownPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(907, 415)
})
g_pButton_ITEM4:SetEnableClick(false)
g_pButton_ITEM4:SetCustomMsgMouseOver(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OVER)
g_pButton_ITEM4:SetCustomMsgMouseOut(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OUT)

g_pButton_ITEM5 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ITEM5)
g_pButton_ITEM5:SetName("ITEM5")
g_pButton_ITEM5:SetNormalTex("DLG_Common_Button00.tga", "invisible")
g_pButton_ITEM5:SetOverTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM5:SetDownTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM5:SetNormalPoint({
	ADD_SIZE_X = 30,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 30,
	LEFT_TOP = D3DXVECTOR2(940, 415)
})
g_pButton_ITEM5:SetOverPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(940, 415)
})
g_pButton_ITEM5:SetDownPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(940, 415)
})
g_pButton_ITEM5:SetEnableClick(false)
g_pButton_ITEM5:SetCustomMsgMouseOver(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OVER)
g_pButton_ITEM5:SetCustomMsgMouseOut(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OUT)

g_pButton_ITEM6 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ITEM6)
g_pButton_ITEM6:SetName("ITEM6")
g_pButton_ITEM6:SetNormalTex("DLG_Common_Button00.tga", "invisible")
g_pButton_ITEM6:SetOverTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM6:SetDownTex("DLG_UI_Button04.tga", "pay_selec")
g_pButton_ITEM6:SetNormalPoint({
	ADD_SIZE_X = 30,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 30,
	LEFT_TOP = D3DXVECTOR2(973, 415)
})
g_pButton_ITEM6:SetOverPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(973, 415)
})
g_pButton_ITEM6:SetDownPoint({
	ADD_SIZE_X = -19,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -19,
	LEFT_TOP = D3DXVECTOR2(973, 415)
})
g_pButton_ITEM6:SetEnableClick(false)
g_pButton_ITEM6:SetCustomMsgMouseOver(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OVER)
g_pButton_ITEM6:SetCustomMsgMouseOut(PARTY_UI_CUSTOM_MSG.PUCM_REWARD_ITEM_MOUSE_OUT)

g_pButton_START = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_START)
g_pButton_START:SetName("PartyRequest")
g_pButton_START:SetNormalTex("DLG_UI_BUTTON19_NEW.tga", "DUNGEON_START_NORMAL")
g_pButton_START:SetOverTex("DLG_UI_BUTTON19_NEW.tga", "DUNGEON_START_OVER")
g_pButton_START:SetDownTex("DLG_UI_BUTTON19_NEW.tga", "DUNGEON_START_OVER")
g_pButton_START:SetDisableTex("DLG_UI_BUTTON19_NEW.tga", "DUNGEON_START_NORMAL")
g_pButton_START:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(809, 457)
})
g_pButton_START:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(809, 457)
})
g_pButton_START:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(810, 458)
})
g_pButton_START:SetDisablePoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(809, 457),
	COLOR = D3DXCOLOR(0.8, 0.8, 0.8, 0.8)
})
g_pButton_START:SetCustomMsgMouseUp(PARTY_UI_CUSTOM_MSG.PUCM_PARTY_PLAY_SOLO)
g_pButton_START:SetCustomMsgMouseUp(PARTY_UI_CUSTOM_MSG.PUCM_PARTY_QUICK_PARTY)

g_pButton_START = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_START)
g_pButton_START:SetName("BonusPartyRequest")
g_pButton_START:SetNormalTex("DLG_UI_BUTTON19_NEW.tga", "HERO_START_NORMAL")
g_pButton_START:SetOverTex("DLG_UI_BUTTON19_NEW.tga", "HERO_START_OVER")
g_pButton_START:SetDownTex("DLG_UI_BUTTON19_NEW.tga", "HERO_START_OVER")
g_pButton_START:SetDisableTex("DLG_UI_BUTTON19_NEW.tga", "HERO_START_NORMAL")
g_pButton_START:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(809, 457)
})
g_pButton_START:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(809, 457)
})
g_pButton_START:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(810, 458)
})
g_pButton_START:SetDisablePoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(809, 457),
	COLOR = D3DXCOLOR(0.8, 0.8, 0.8, 0.8)
})
g_pButton_START:SetCustomMsgMouseUp(PARTY_UI_CUSTOM_MSG.PUCM_PARTY_PLAY_SOLO)
g_pButton_START:SetCustomMsgMouseUp(PARTY_UI_CUSTOM_MSG.PUCM_PARTY_QUICK_PARTY)

g_pButton_START2 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_START2)
g_pButton_START2:SetName("CurrentMemberStart")
g_pButton_START2:SetNormalTex("DLG_UI_BUTTON19_NEW.tga", "DUNGEON_START2_NORMAL")
g_pButton_START2:SetOverTex("DLG_UI_BUTTON19_NEW.tga", "DUNGEON_START2_OVER")
g_pButton_START2:SetDownTex("DLG_UI_BUTTON19_NEW.tga", "DUNGEON_START2_OVER")
g_pButton_START2:SetDisableTex("DLG_UI_BUTTON19_NEW.tga", "DUNGEON_START2_NORMAL")
g_pButton_START2:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(809, 500)
})
g_pButton_START2:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(809, 500)
})
g_pButton_START2:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(810, 501)
})
g_pButton_START2:SetDisablePoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(809, 500),
	COLOR = D3DXCOLOR(0.8, 0.8, 0.8, 0.8)
})
g_pButton_START2:SetCustomMsgMouseUp(PARTY_UI_CUSTOM_MSG.PUCM_PARTY_GAME_START)

-- June-only controls omitted here (JUNE_UI_PARITY_PHASES.md Phase 5):
--   GuildPve (Guild PvE event match; PUCM_GUILD_PVE_EVENT_MATCH_CHECKED/
--     UNCHECKED, 14065/14066, are not in March),
--   InhouseCurrentMemberHeroBuffBox (in-house Hero buff test box),
--   Static_Pve_RankGauge and Button_Pve_Rating_Notice (June's PvE rating
--     gauge, DLG_UI_Common_Texture_PVE_01 and STR_ID_44473).
-- All four were created hidden in June; nothing in the ported C++ needs them
-- beyond the NULL-safe GuildPve lookups.