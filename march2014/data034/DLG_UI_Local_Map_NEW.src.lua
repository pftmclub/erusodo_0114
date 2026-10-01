--{{ Iruha : 2026-10-01 // June UI parity Phase 2 (SERV_IRUHADEV_JUNE_LOCAL_MAP_UI)
-- June2017 DLG_UI_Local_Map_NEW.lua (live data217 copy, 18273 B), adopted
-- wholesale: two pages of six tabs on DLG_UI_Button21, paged by
-- LocalMap_arrowL/R + StaticArrow (DLG_UI_Image_07, June data215).
-- CX2LocalMapUI::UpdateLocalMapDLG does the paging. June-only controls
-- are omitted below with a note where each stood; nothing else changed.
--}}

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
g_pButton_Background:SetEnable(false)

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
	ADD_SIZE_X = 500,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(31, 0)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_RIGHT_TOP")
g_pPicture_bg:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(551, 0)
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
	ADD_SIZE_X = 510,
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
	LEFT_TOP = D3DXVECTOR2(551, 47)
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
	ADD_SIZE_X = 500,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(31, 430)
})

g_pPicture_bg = g_pUIDialog:CreatePicture()

g_pStaticlocal:AddPicture(g_pPicture_bg)
g_pPicture_bg:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "BG_RIGHT_BOTTOM")
g_pPicture_bg:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(551, 430)
})

g_pStaticArrow = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticArrow)
g_pStaticArrow:SetName("StaticArrow")
g_pStaticArrow:SetOffsetPos(-6, -36)

g_pPicture_ArrowBg = g_pUIDialog:CreatePicture()

g_pStaticArrow:AddPicture(g_pPicture_ArrowBg)
g_pPicture_ArrowBg:SetTex("DLG_UI_Image_07.TGA", "BG_ARROW_L")
g_pPicture_ArrowBg:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(4, 9)
})
g_pPicture_ArrowBg:SetShow(false)

g_pPicture_ArrowBg2 = g_pUIDialog:CreatePicture()

g_pStaticArrow:AddPicture(g_pPicture_ArrowBg2)
g_pPicture_ArrowBg2:SetTex("DLG_UI_Image_07.TGA", "BG_ARROW_R")
g_pPicture_ArrowBg2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(372, 9)
})
g_pPicture_ArrowBg2:SetShow(false)

g_pButtonL_arrow = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonL_arrow)
g_pButtonL_arrow:SetName("LocalMap_arrowL")
g_pButtonL_arrow:SetOffsetPos(-6, -36)
g_pButtonL_arrow:SetNormalTex("DLG_UI_Image_07.tga", "ARROW_L_N")
g_pButtonL_arrow:SetOverTex("DLG_UI_Image_07.tga", "ARROW_L_O")
g_pButtonL_arrow:SetDownTex("DLG_UI_Image_07.tga", "ARROW_L_O")
g_pButtonL_arrow:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(4, 9)
})
g_pButtonL_arrow:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(4, 9)
})
g_pButtonL_arrow:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(4, 10)
})
g_pButtonL_arrow:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_LOCAL_MAP_ARROW_LEFT)
g_pButtonL_arrow:SetShow(false)

g_pButtonR_arrow = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonR_arrow)
g_pButtonR_arrow:SetName("LocalMap_arrowR")
g_pButtonR_arrow:SetOffsetPos(-6, -36)
g_pButtonR_arrow:SetNormalTex("DLG_UI_Image_07.tga", "ARROW_R_N")
g_pButtonR_arrow:SetOverTex("DLG_UI_Image_07.tga", "ARROW_R_O")
g_pButtonR_arrow:SetDownTex("DLG_UI_Image_07.tga", "ARROW_R_O")
g_pButtonR_arrow:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(372, 9)
})
g_pButtonR_arrow:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(372, 9)
})
g_pButtonR_arrow:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(372, 10)
})
g_pButtonR_arrow:SetCustomMsgMouseUp(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_LOCAL_MAP_ARROW_RIGHT)
g_pButtonR_arrow:SetShow(false)

MOVE_AREA_BUTTON_X = 27
g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtn0")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button21.tga", "RUBEN_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button21.tga", "RUBEN_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button21.tga", "RUBEN_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 4, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 4, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 4, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 60, 25)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 4, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_RUBEN)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtn1")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button21.tga", "ELDER_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button21.tga", "ELDER_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button21.tga", "ELDER_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 64, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_ELDER)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtn2")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button21.tga", "BESMA_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button21.tga", "BESMA_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button21.tga", "BESMA_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 - 1, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 - 1, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 - 1, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 + 64 - 1, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 - 1, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_BESMA)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtn3")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button21.tga", "ALTERA_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button21.tga", "ALTERA_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button21.tga", "ALTERA_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 116 - 2, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 116 - 2, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 116 - 2, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 116 + 64 - 2, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 116 - 2, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_ALTERA_ISLAND)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtn4")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button21.tga", "PEITA_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button21.tga", "PEITA_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button21.tga", "PEITA_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 174 - 3, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 174 - 3, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 174 - 3, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 174 + 64 - 3, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 174 - 3, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_PEITA)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtn5")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button21.tga", "VELDER_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button21.tga", "VELDER_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button21.tga", "VELDER_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 232 - 4, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 232 - 4, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 232 - 4, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 232 + 64 - 4, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 232 - 4, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_VELDER)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtn6")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button21.tga", "HAMEL_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button21.tga", "HAMEL_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button21.tga", "HAMEL_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 4, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 4, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 4, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 60, 25)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 4, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_HAMEL)
g_pRadioButtonruben:SetShow(false)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtn7")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button21.tga", "SANDER_BTN_NORMAL")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button21.tga", "SANDER_BTN_OVER")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button21.tga", "SANDER_BTN_DOWN")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 64, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_SANDER)
g_pRadioButtonruben:SetShow(false)

g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtn8")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Button21.tga", "Btn_RANOX_N")
g_pRadioButtonruben:SetOverTex("DLG_UI_Button21.tga", "Btn_RANOX_O")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Button21.tga", "Btn_RANOX_D")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 - 1, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 - 1, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 - 1, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 + 64 - 1, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 58 - 1, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_RANOX)
g_pRadioButtonruben:SetShow(false)

-- [June UI parity Phase 2] omitted: LocalMapRadioBtn9 (Btn_ELISION_*,
-- LOCAL_MAP_ID.LMI_ELYSION) and LocalMapRadioBtnEvent (DLG_UI_Text_14
-- Btn_EVENT_*, LMI_EVENT). Elysion / Event maps are June-only content; March
-- has no LMI_* for them, so the dummy would read 0 and the tab would be dead.

--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June's LocalMapRadioBtnSpecial, verbatim: the Special map now carries Henir (June moved
-- it off every village map), and LMI_SPECIAL = 10011 is in Enum.lua. Atlas
-- DLG_UI_Text_14 (June data151). UpdateLocalMapDLG shows it by level alone on both pages.
--}}
g_pRadioButtonruben = g_pUIDialog:CreateRadioButton()

g_pUIDialog:AddControl(g_pRadioButtonruben)
g_pRadioButtonruben:SetFixOverByCheck(true)
g_pRadioButtonruben:SetShowOffBGByCheck(true)
g_pRadioButtonruben:SetName("LocalMapRadioBtnSpecial")
g_pRadioButtonruben:SetOffsetPos(-6, -36)
g_pRadioButtonruben:SetNormalTex("DLG_UI_Text_14.tga", "Btn_SPECIAL_N")
g_pRadioButtonruben:SetOverTex("DLG_UI_Text_14.tga", "Btn_SPECIAL_O")
g_pRadioButtonruben:SetCheckedTex("DLG_UI_Text_14.tga", "Btn_SPECIAL_D")
g_pRadioButtonruben:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 348 - 5, 9)
})
g_pRadioButtonruben:SetBGMouseOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 348 - 5, 9)
})
g_pRadioButtonruben:SetRBPoint({
	CHANGE_TIME = 0,
	USE_TEXTURE_SIZE = FALSE,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 348 - 5, 9),
	RIGHT_BOTTOM = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 348 + 64 - 5, 34)
})
g_pRadioButtonruben:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 61 + 348 - 5, 7)
})
g_pRadioButtonruben:SetGroupID(1)
g_pRadioButtonruben:SetCustomMsgChecked(LOCAL_MAP_UI_CUSTOM_MSG.LMUCM_DUNGEON_GATE_SELECTED)
g_pRadioButtonruben:AddDummyInt(LOCAL_MAP_ID.LMI_SPECIAL)
g_pRadioButtonruben:SetShow(false)

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
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 2, 2),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_RUBEN)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 58 + 1, 2),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_ELDER)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 116 + 1, 2),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_BESMA)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 174 - 1, 2),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_ALTERA_ISLAND)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 232 - 2, 2),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_PEITA)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 290 - 3, 2),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_VELDER)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 2, 2),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_HAMEL)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 58 + 1, 2),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_SANDER)
g_pPicture:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(MOVE_AREA_BUTTON_X + 116 + 1, 2),
	COLOR = D3DXCOLOR(1, 1, 1, 0.7)
})
g_pPicture:SetIndex(LOCAL_MAP_ID.LMI_RANOX)
g_pPicture:SetShow(false)

-- [June UI parity Phase 2] omitted: QuestGuide picture for LMI_ELYSION (June-only).

-- [June UI parity Phase 2] omitted: WeeklyMark static (DLG_UI_Text_02
-- S_Dungeon_Icon; the atlas is not in March and its show condition,
-- June FUN_327d9d50 / FUN_327da190, is a June-only weekly-dungeon system).
-- CX2LocalMapUI keeps the lookup NULL-safe.

-- [June UI parity Phase 2] omitted: June event statics APOCALYPSE_STATIC
-- (DLG_UI_Image_02), g_pStatic_Halloween2016 / g_pStatic_Event
-- (DLG_UI_Common_Texture78_NEW), g_pStatic_Event2017
-- (DLG_Common_New_Texture52), g_pStatic_NewEvent / g_pStatic_NewSpecial
-- (DLG_Common_New_Texture49). All hidden by default and shown only on June
-- event flags or June-only maps; none of their atlases is in March.
