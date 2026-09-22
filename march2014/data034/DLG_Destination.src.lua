g_pUIDialog:SetName("FREEWARP")
g_pUIDialog:SetModal(false)
g_pUIDialog:SetFront(true)
g_pUIDialog:SetLayer(X2_DIALOG_LAYER.XDL_POP_UP)
g_pUIDialog:SetCloseCustomUIEventID(UI_INVENTORY_MSG.UIM_WARP_DEST_CLOSE)

g_pStaticSkill_Acquisition = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticSkill_Acquisition)
g_pStaticSkill_Acquisition:SetName("g_pStaticSkill_Acquisition")

g_pPictureSkill_BG_top = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPictureSkill_BG_top)
g_pPictureSkill_BG_top:SetTex("DLG_UI_Common_Texture10.TGA", "Skill_BG01_TOP")
g_pPictureSkill_BG_top:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})

g_pPictureSkill_BG_middle = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPictureSkill_BG_middle)
g_pPictureSkill_BG_middle:SetTex("DLG_UI_Common_Texture10.TGA", "Skill_BG01_middle")
g_pPictureSkill_BG_middle:SetPoint({
	ADD_SIZE_Y = 254,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 23)
})

g_pPictureSkill_BG_bottom = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPictureSkill_BG_bottom)
g_pPictureSkill_BG_bottom:SetTex("DLG_UI_Common_Texture10.TGA", "Skill_BG01_bottom")
g_pPictureSkill_BG_bottom:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 279)
})

g_pPictureSkill_BG_top = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPictureSkill_BG_top)
g_pPictureSkill_BG_top:SetTex("DLG_UI_Common_Texture10.TGA", "BG01_TOP")
g_pPictureSkill_BG_top:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(15, 45)
})

g_pPictureSkill_BG_middle = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPictureSkill_BG_middle)
g_pPictureSkill_BG_middle:SetTex("DLG_UI_Common_Texture10.TGA", "BG01_middle")
g_pPictureSkill_BG_middle:SetPoint({
	ADD_SIZE_Y = 192,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(15, 50)
})

g_pPictureSkill_BG_bottom = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPictureSkill_BG_bottom)
g_pPictureSkill_BG_bottom:SetTex("DLG_UI_Common_Texture10.TGA", "BG01_bottom")
g_pPictureSkill_BG_bottom:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(15, 244)
})

g_pStatic_Notice = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Notice)
g_pStatic_Notice:SetName("Static_Notice")

g_pPicture_Notice = g_pUIDialog:CreatePicture()

g_pStatic_Notice:AddPicture(g_pPicture_Notice)
g_pPicture_Notice:SetTex("DLG_UI_Common_Texture02.TGA", "feel")
g_pPicture_Notice:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(15, 16)
})
g_pStatic_Notice:AddString({
	MSG = STR_ID_4611,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(36, 18),
	OUTLINE_COLOR = D3DXCOLOR(0, 0, 0, 1)
})

g_pPicture_line1 = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPicture_line1)
g_pPicture_line1:SetTex("DLG_UI_Common_Texture01.TGA", "bar1")
g_pPicture_line1:SetPoint({
	ADD_SIZE_X = 224,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(11, 40),
	COLOR = D3DXCOLOR(1, 1, 1, 0.8)
})

g_pPicture_line1 = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPicture_line1)
g_pPicture_line1:SetTex("DLG_UI_Common_Texture01.TGA", "bar1")
g_pPicture_line1:SetPoint({
	ADD_SIZE_X = 194,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(17, 79),
	COLOR = D3DXCOLOR(1, 1, 1, 0.8)
})

g_pPicture_line1 = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPicture_line1)
g_pPicture_line1:SetTex("DLG_UI_Common_Texture01.TGA", "bar1")
g_pPicture_line1:SetPoint({
	ADD_SIZE_X = 194,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(17, 113),
	COLOR = D3DXCOLOR(1, 1, 1, 0.8)
})

g_pPicture_line1 = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPicture_line1)
g_pPicture_line1:SetTex("DLG_UI_Common_Texture01.TGA", "bar1")
g_pPicture_line1:SetPoint({
	ADD_SIZE_X = 194,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(17, 147),
	COLOR = D3DXCOLOR(1, 1, 1, 0.8)
})

g_pPicture_line1 = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPicture_line1)
g_pPicture_line1:SetTex("DLG_UI_Common_Texture01.TGA", "bar1")
g_pPicture_line1:SetPoint({
	ADD_SIZE_X = 194,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(17, 181),
	COLOR = D3DXCOLOR(1, 1, 1, 0.8)
})

g_pPicture_line1 = g_pUIDialog:CreatePicture()

g_pStaticSkill_Acquisition:AddPicture(g_pPicture_line1)
g_pPicture_line1:SetTex("DLG_UI_Common_Texture01.TGA", "bar1")
g_pPicture_line1:SetPoint({
	ADD_SIZE_X = 194,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(17, 215),
	COLOR = D3DXCOLOR(1, 1, 1, 0.8)
})

g_pButton_BG = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_BG)
g_pButton_BG:SetName("BG_ChangeIntroduce")
g_pButton_BG:SetNormalTex("DLG_UI_BUTTON13.tga", "BTN_BG_NORMAL")
g_pButton_BG:SetOverTex("DLG_UI_BUTTON13.tga", "BTN_BG_OVER")
g_pButton_BG:SetDownTex("DLG_UI_BUTTON13.tga", "BTN_BG_OVER")
g_pButton_BG:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(85, 256)
})
g_pButton_BG:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(83, 254)
})
g_pButton_BG:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(84, 255)
})
g_pButton_BG:SetCustomMsgMouseUp(UI_INVENTORY_MSG.UIM_WARP_DEST)

g_pStatic_BTN_GUILD1 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_BTN_GUILD1)
g_pStatic_BTN_GUILD1:SetName("BTN_ChangeIntroduce")
g_pStatic_BTN_GUILD1:AddString({
	MSG = STR_ID_4612,
	FONT_INDEX = XUF_DODUM_11_NORMAL,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_CENTER,
	POS = D3DXVECTOR2(124, 260),
	COLOR = D3DXCOLOR(1, 1, 0, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pControlList = g_pUIDialog:CreateControlList()

g_pUIDialog:AddControl(g_pControlList)
g_pControlList:SetName("WarpDestControlList")
g_pControlList:SetYScollBarUpButtonTex("DLG_Common_New_Texture61.tga", "Up_Arrow_Normal")
g_pControlList:SetYScollBarUpButtonDownTex("DLG_Common_New_Texture61.tga", "Up_Arrow_Over")
g_pControlList:SetYScollBarDownButtonTex("DLG_Common_New_Texture61.tga", "Down_Arrow_Normal")
g_pControlList:SetYScollBarDownButtonDownTex("DLG_Common_New_Texture61.tga", "Down_Arrow_Over")
g_pControlList:SetYScollBarThumbTex("DLG_Common_New_Texture61.tga", "Scroll_Bar")
g_pControlList:SetYScollBarTrackTex("DLG_Room_Button0.tga", "Invisible")

g_pScrollBar = g_pControlList:InitYScrollBar()

g_pControlList:SetYScrollBarPos(212, 47)
g_pControlList:SetYScrollBarSize(21, 202)
g_pControlList:SetScrollOffset(80, 34)
g_pControlList:SetPos(20, 51)
g_pControlList:SetViewSize(2, 6, true)

g_pButtonExit = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonExit)
g_pButtonExit:SetName("Button_Mini_Win_Exit")
g_pButtonExit:SetNormalTex("DLG_UI_Button10.tga", "Exit_Normal")
g_pButtonExit:SetOverTex("DLG_UI_Button10.tga", "Exit_Over")
g_pButtonExit:SetDownTex("DLG_UI_Button10.tga", "Exit_Over")
g_pButtonExit:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(213, 16)
})
g_pButtonExit:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(210, 13)
})
g_pButtonExit:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(211, 14)
})
g_pButtonExit:SetCustomMsgMouseUp(UI_INVENTORY_MSG.UIM_WARP_DEST_CLOSE)

g_pStatic_Ruben_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Ruben_Town)
g_pStatic_Ruben_Town:SetName("Village_20000")

g_pPicture_Icon01 = g_pUIDialog:CreatePicture()

g_pStatic_Ruben_Town:AddPicture(g_pPicture_Icon01)
g_pPicture_Icon01:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Ruben")
g_pPicture_Icon01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0),
	COLOR = D3DXCOLOR(1, 1, 1, 0.8)
})
g_pStatic_Ruben_Town:AddString({
	MSG = STR_ID_1112,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0, 0.5, 1, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Elder_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Elder_Town)
g_pStatic_Elder_Town:SetName("Village_20001")

g_pPicture_Icon02 = g_pUIDialog:CreatePicture()

g_pStatic_Elder_Town:AddPicture(g_pPicture_Icon02)
g_pPicture_Icon02:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Elder")
g_pPicture_Icon02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Elder_Town:AddString({
	MSG = STR_ID_1114,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0, 0.5, 1, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Elder_DG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Elder_DG)
g_pStatic_Elder_DG:SetName("Village_1000")

g_pPicture_Icon02 = g_pUIDialog:CreatePicture()

g_pStatic_Elder_DG:AddPicture(g_pPicture_Icon02)
g_pPicture_Icon02:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Elder")
g_pPicture_Icon02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Elder_DG:AddString({
	MSG = STR_ID_19920,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0.7, 0, 0, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Besma_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Besma_Town)
g_pStatic_Besma_Town:SetName("Village_20002")

g_pPicture_Icon03 = g_pUIDialog:CreatePicture()

g_pStatic_Besma_Town:AddPicture(g_pPicture_Icon03)
g_pPicture_Icon03:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Besma")
g_pPicture_Icon03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Besma_Town:AddString({
	MSG = STR_ID_1116,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0, 0.5, 1, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Besma_DG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Besma_DG)
g_pStatic_Besma_DG:SetName("Village_1001")

g_pPicture_Icon03 = g_pUIDialog:CreatePicture()

g_pStatic_Besma_DG:AddPicture(g_pPicture_Icon03)
g_pPicture_Icon03:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Besma")
g_pPicture_Icon03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Besma_DG:AddString({
	MSG = STR_ID_19921,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0.7, 0, 0, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Altera_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Altera_Town)
g_pStatic_Altera_Town:SetName("Village_20003")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Altera_Town:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Altera")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Altera_Town:AddString({
	MSG = STR_ID_1118,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0, 0.5, 1, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Altera_DG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Altera_DG)
g_pStatic_Altera_DG:SetName("Village_1002")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Altera_DG:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Altera")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Altera_DG:AddString({
	MSG = STR_ID_19922,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0.7, 0, 0, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Feita_DG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Feita_DG)
g_pStatic_Feita_DG:SetName("Village_20005")

g_pPicture_Icon04 = g_pUIDialog:CreatePicture()

g_pStatic_Feita_DG:AddPicture(g_pPicture_Icon04)
g_pPicture_Icon04:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Feita")
g_pPicture_Icon04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Feita_DG:AddString({
	MSG = STR_ID_1119,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0, 0.5, 1, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Velder_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Velder_Town)
g_pStatic_Velder_Town:SetName("Village_20004")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Velder_Town:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Velder")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Velder_Town:AddString({
	MSG = STR_ID_4622,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0, 0.5, 1, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Velder_DG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Velder_DG)
g_pStatic_Velder_DG:SetName("Village_1004")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Velder_DG:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Common_Texture10.TGA", "Icon_Velder")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Velder_DG:AddString({
	MSG = STR_ID_19924,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0.7, 0, 0, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Velder_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Velder_Town)
g_pStatic_Velder_Town:SetName("Village_20006")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Velder_Town:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Common_Texture10.TGA", "ICON_HAMEL")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Velder_Town:AddString({
	MSG = STR_ID_10232,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0, 0.5, 1, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Velder_DG = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Velder_DG)
g_pStatic_Velder_DG:SetName("Village_1005")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Velder_DG:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Common_Texture10.TGA", "ICON_HAMEL")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Velder_DG:AddString({
	MSG = STR_ID_19925,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0.7, 0, 0, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Sander_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Sander_Town)
g_pStatic_Sander_Town:SetName("Village_20007")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Sander_Town:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Button17.TGA", "SANDER_ICON")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Sander_Town:AddString({
	MSG = STR_ID_22048,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0, 0.5, 1, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Sander_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Sander_Town)
g_pStatic_Sander_Town:SetName("Village_1006")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Sander_Town:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Button17.TGA", "SANDER_ICON")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Sander_Town:AddString({
	MSG = STR_ID_23093,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0.7, 0, 0, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

--{{ Iruha : 2026-09-19 // Ranox port Phase 29 -- Village_20008/Village_1007
-- controls for the second, in-village warp-NPC dialog (CX2UIInventory::
-- CreateWarpDest/DLG_Destination.lua) -- distinct from Phase 21's Cobo
-- Express dialog (CX2StateMenu::CreateWarpDest/DLG_Destination_byButton.lua),
-- which this port had left unpatched. Transcribed verbatim from June2017
-- data034_kom/DLG_Destination.lua, same STR_IDs/colors/RANOX_ICON swatch
-- Phase 21 already used for the other dialog. See LANOX_TROUBLESHOOTING.md.

g_pStatic_Ranox_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Ranox_Town)
g_pStatic_Ranox_Town:SetName("Village_20008")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Ranox_Town:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Button17.TGA", "RANOX_ICON")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Ranox_Town:AddString({
	MSG = STR_ID_32562,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0, 0.5, 1, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})

g_pStatic_Ranox_Town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Ranox_Town)
g_pStatic_Ranox_Town:SetName("Village_1007")

g_pPicture_Icon05 = g_pUIDialog:CreatePicture()

g_pStatic_Ranox_Town:AddPicture(g_pPicture_Icon05)
g_pPicture_Icon05:SetTex("DLG_UI_Button17.TGA", "RANOX_ICON")
g_pPicture_Icon05:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pStatic_Ranox_Town:AddString({
	MSG = STR_ID_33882,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(30, 5),
	COLOR = D3DXCOLOR(0.7, 0, 0, 0.9),
	OUTLINE_COLOR = D3DXCOLOR(0.5, 0.3, 0, 1)
})
