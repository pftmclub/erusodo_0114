g_pUIDialog:SetName("DLG_UI_LOCAL_MAP")
g_pUIDialog:SetPos(216, 132)
g_pUIDialog:SetFront(true)
g_pUIDialog:SetLayer(X2_DIALOG_LAYER.XDL_POP_UP)

g_pButton_Background = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_Background)
g_pButton_Background:SetName("ClickDislabe")
g_pButton_Background:SetNormalTex("DLG_Common_Button00.tga", "invisible")
g_pButton_Background:SetOverTex("DLG_Common_Button00.tga", "invisible")
g_pButton_Background:SetDownTex("DLG_Common_Button00.tga", "invisible")
g_pButton_Background:SetNormalPoint({
	CHANGE_TIME = 0,
	ADD_SIZE_X = 1024,
	ADD_SIZE_Y = 768,
	LEFT_TOP = D3DXVECTOR2(-216, 405),
	COLOR = D3DXCOLOR(1, 1, 1, 0.3)
})
g_pButton_Background:SetOverPoint({
	CHANGE_TIME = 0,
	ADD_SIZE_X = 1024,
	ADD_SIZE_Y = 768,
	LEFT_TOP = D3DXVECTOR2(-216, 405),
	COLOR = D3DXCOLOR(1, 1, 1, 0.3)
})
g_pButton_Background:SetDownPoint({
	CHANGE_TIME = 0,
	ADD_SIZE_X = 1024,
	ADD_SIZE_Y = 768,
	LEFT_TOP = D3DXVECTOR2(-216, 405),
	COLOR = D3DXCOLOR(1, 1, 1, 0.3)
})

function IconAni(slot0, slot1, slot2)
	slot0:SetOverAnimTex({
		SequenceID = 0,
		fShowTime = 0.1,
		FileName = slot1,
		KeyName = slot2,
		ScaleAxis = TEX_AXIS.TA_CENTER_CENTER,
		SCALE = D3DXVECTOR2(1.05, 0.95),
		POS = D3DXVECTOR2(0, 0)
	})
	slot0:SetOverAnimTex({
		fShowTime = 0.1,
		SequenceID = 1,
		ScaleAxis = TEX_AXIS.TA_CENTER_CENTER,
		SCALE = D3DXVECTOR2(0.95, 1.05),
		POS = D3DXVECTOR2(0, 0)
	})
	slot0:SetOverAnimTex({
		fShowTime = 0.2,
		SequenceID = 2,
		ScaleAxis = TEX_AXIS.TA_CENTER_CENTER,
		SCALE = D3DXVECTOR2(1, 1),
		POS = D3DXVECTOR2(0, 0),
		bEndTexChange = TRUE
	})
end

g_pStaticlocal = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticlocal)
g_pStaticlocal:SetName("local")
g_pStaticlocal:SetOffsetPos(-6, -36)

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_LEFT_TOP")
g_pPicture_bg:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_CENTER_TOP")
g_pPicture_bg:SetPoint({
	ADD_SIZE_X = 565,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(31, 0)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_RIGHT_TOP")
g_pPicture_bg:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(616, 0)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_LEFT_MIDDLE")
g_pPicture_bg:SetPoint({
	ADD_SIZE_Y = 373,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 47)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_CENTER_MIDDLE")
g_pPicture_bg:SetPoint({
	ADD_SIZE_X = 575,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 373,
	LEFT_TOP = D3DXVECTOR2(31, 47)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_RIGHT_MIDDLE")
g_pPicture_bg:SetPoint({
	ADD_SIZE_Y = 373,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(616, 47)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_LEFT_BOTTOM")
g_pPicture_bg:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 430)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_CENTER_BOTTOM")
g_pPicture_bg:SetPoint({
	ADD_SIZE_X = 565,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(31, 430)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_RIGHT_BOTTOM")
g_pPicture_bg:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(616, 430)
})

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("ruben")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button20_NEW.tga", "RUBEN_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button20_NEW.tga", "RUBEN_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button20_NEW.tga", "RUBEN_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(8, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(8, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(8, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(64, 25)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(8, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_RUBEN)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("ruben")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button20_NEW.tga", "ELDER_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button20_NEW.tga", "ELDER_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button20_NEW.tga", "ELDER_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(73, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(73, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(73, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(137, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(73, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_ELDER)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("ruben")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button20_NEW.tga", "BESMA_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button20_NEW.tga", "BESMA_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button20_NEW.tga", "BESMA_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(138, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(138, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(138, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(202, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(138, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_BESMA)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("ruben")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button20_NEW.tga", "PEITA_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button20_NEW.tga", "PEITA_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button20_NEW.tga", "PEITA_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(268, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(268, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(268, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(332, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(268, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_PEITA)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("ruben")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button20_NEW.tga", "ALTERA_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button20_NEW.tga", "ALTERA_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button20_NEW.tga", "ALTERA_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(203, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(203, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(203, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(267, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(203, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_ALTERA_ISLAND)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("ruben")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button20_NEW.tga", "VELDER_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button20_NEW.tga", "VELDER_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button20_NEW.tga", "VELDER_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(333, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(333, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(333, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(397, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(333, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_VELDER)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("ruben")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button20_NEW.tga", "HAMEL_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button20_NEW.tga", "HAMEL_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button20_NEW.tga", "HAMEL_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(398, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(398, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(398, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(462, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(398, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_HAMEL)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("ruben")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button20_NEW.tga", "SANDER_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button20_NEW.tga", "SANDER_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button20_NEW.tga", "SANDER_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(463, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(463, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(463, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(527, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(463, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_SANDER)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("ruben")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button20_NEW.tga", "RANOX_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button20_NEW.tga", "RANOX_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button20_NEW.tga", "RANOX_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(528, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(528, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(528, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(592, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(528, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_RANOX)

g_pStaticlocal = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticlocal)
g_pStaticlocal:SetOffsetPos(-6, -36)

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "MAIN_BAR")
g_pPicture_bg:SetPoint({
	ADD_SIZE_X = 309,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(4, 35)
})

g_pStatic_Quest = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Quest)
g_pStatic_Quest:SetName("QuestGuide")
g_pStatic_Quest:SetOffsetPos(-6, -36)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(0, -10),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_RUBEN)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(65, -10),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_ELDER)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(130, -10),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_BESMA)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(195, -10),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_ALTERA_ISLAND)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(260, -10),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_PEITA)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(325, -10),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_VELDER)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(390, -10),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_HAMEL)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(455, -10),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_SANDER)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(520, -10),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_RANOX)
g_pPicture:SetShow(false)
