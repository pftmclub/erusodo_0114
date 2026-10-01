--{{ Iruha : 2026-10-01 // June UI parity Phase 5 (SERV_IRUHADEV_JUNE_PARTY_UI)
-- June2017 DLG_UI_PARTY_BOTTON_NEW_20161103.lua (data034, 5679 B), adopted
-- unchanged. Loaded by CX2PartyUI::OpenPartyMenu in place of March's
-- DLG_UI_PARTY_BOTTON_NEW.lua. Differs from March's only in the matching
-- wait panel (DUNGEON_STAY/PVP_STAY): moved left/down by an offset, a
-- larger DUNGEON_STAY_1 backdrop (DLG_UI_Common_Texture49_NEW, June data057),
-- and a fifth DUNGEON_STAY string (STR_ID_43192) set by UpdateMatchingUI.
--}}

g_pUIDialog:SetName("DLG_UI_Party_Button")
g_pUIDialog:SetPos(0, 0)
g_pUIDialog:SetModal(true)
g_pUIDialog:SetFront(true)
g_pUIDialog:SetLayer(X2_DIALOG_LAYER.XDL_NORMAL)

g_pCheckBox_DUNGEON = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_DUNGEON)
g_pCheckBox_DUNGEON:SetName("g_pCheckBox_DUNGEON")
g_pCheckBox_DUNGEON:SetNormalTex("DLG_UI_Button19_NEW.TGA", "DUNGEON_NORMAL")
g_pCheckBox_DUNGEON:SetOverTex("DLG_UI_Button19_NEW.TGA", "DUNGEON_OVER")
g_pCheckBox_DUNGEON:SetDisableTex("DLG_UI_Button20.TGA", "DUNGEON_INACTIVITY")
g_pCheckBox_DUNGEON:SetCheckedTex("DLG_UI_Button19_NEW.TGA", "DUNGEON_DOWN")
g_pCheckBox_DUNGEON:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 96)
})
g_pCheckBox_DUNGEON:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 95)
})
g_pCheckBox_DUNGEON:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_SELECT_DUNGEON_MENU)
g_pCheckBox_DUNGEON:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_SELECT_DUNGEON_MENU)

g_pButton_WaitingForDungeonMatch = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_WaitingForDungeonMatch)
g_pButton_WaitingForDungeonMatch:SetName("DungeonMatchWaiting")
g_pButton_WaitingForDungeonMatch:SetNormalTex("DLG_UI_Button19_NEW.tga", "DUNGEON_STAY")
g_pButton_WaitingForDungeonMatch:SetOverTex("DLG_UI_Button19_NEW.tga", "DUNGEON_STAY")
g_pButton_WaitingForDungeonMatch:SetDownTex("DLG_UI_Button19_NEW.tga", "DUNGEON_STAY")
g_pButton_WaitingForDungeonMatch:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 96)
})
g_pButton_WaitingForDungeonMatch:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 96)
})
g_pButton_WaitingForDungeonMatch:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 96)
})
g_pButton_WaitingForDungeonMatch:SetShowEnable(false, false)

g_pButton_CancelForDungeonMatch = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_CancelForDungeonMatch)
g_pButton_CancelForDungeonMatch:SetName("DungeonMatchCancel")
g_pButton_CancelForDungeonMatch:SetNormalTex("DLG_UI_Button19_NEW.tga", "STAY_CANCEL")
g_pButton_CancelForDungeonMatch:SetOverTex("DLG_UI_Button19_NEW.tga", "STAY_CANCEL_OVER")
g_pButton_CancelForDungeonMatch:SetDownTex("DLG_UI_Button19_NEW.tga", "STAY_CANCEL_OVER")
g_pButton_CancelForDungeonMatch:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(907, 96)
})
g_pButton_CancelForDungeonMatch:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(907, 96)
})
g_pButton_CancelForDungeonMatch:SetDownPoint({
	ADD_SIZE_X = -2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -2,
	LEFT_TOP = D3DXVECTOR2(907, 96)
})
g_pButton_CancelForDungeonMatch:SetShowEnable(false, false)
g_pButton_CancelForDungeonMatch:SetCustomMsgMouseUp(PARTY_UI_CUSTOM_MSG.PUCM_DUNGEON_PARTY_MATCH_CANCEL)

g_pCheckBox_PVP = g_pUIDialog:CreateCheckBox()

g_pUIDialog:AddControl(g_pCheckBox_PVP)
g_pCheckBox_PVP:SetName("g_pCheckBox_PVP")
g_pCheckBox_PVP:SetNormalTex("DLG_UI_Button19_NEW.TGA", "PVP_NORMAL")
g_pCheckBox_PVP:SetOverTex("DLG_UI_Button19_NEW.TGA", "PVP_OVER")
g_pCheckBox_PVP:SetDisableTex("DLG_UI_Button20.TGA", "PVP_INACTIVITY")
g_pCheckBox_PVP:SetCheckedTex("DLG_UI_Button19_NEW.TGA", "PVP_DOWN")
g_pCheckBox_PVP:SetBGPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(907, 96)
})
g_pCheckBox_PVP:SetCheckPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(907, 96)
})
g_pCheckBox_PVP:SetCustomMsgChecked(PARTY_UI_CUSTOM_MSG.PUCM_SELECT_PVP_MENU)
g_pCheckBox_PVP:SetCustomMsgUnChecked(PARTY_UI_CUSTOM_MSG.PUCM_SELECT_PVP_MENU)

g_pButton_WaitingForPvpMatch = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_WaitingForPvpMatch)
g_pButton_WaitingForPvpMatch:SetName("PvpMatchWaiting")
g_pButton_WaitingForPvpMatch:SetNormalTex("DLG_UI_Button19_NEW.tga", "PVP_STAY")
g_pButton_WaitingForPvpMatch:SetOverTex("DLG_UI_Button19_NEW.tga", "PVP_STAY")
g_pButton_WaitingForPvpMatch:SetDownTex("DLG_UI_Button19_NEW.tga", "PVP_STAY")
g_pButton_WaitingForPvpMatch:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(907, 96)
})
g_pButton_WaitingForPvpMatch:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(907, 96)
})
g_pButton_WaitingForPvpMatch:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(907, 96)
})
g_pButton_WaitingForPvpMatch:SetShowEnable(false, false)

g_pButton_CancelForPvpMatch = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_CancelForPvpMatch)
g_pButton_CancelForPvpMatch:SetName("PvpMatchCancel")
g_pButton_CancelForPvpMatch:SetNormalTex("DLG_UI_Button19_NEW.tga", "STAY_CANCEL")
g_pButton_CancelForPvpMatch:SetOverTex("DLG_UI_Button19_NEW.tga", "STAY_CANCEL_OVER")
g_pButton_CancelForPvpMatch:SetDownTex("DLG_UI_Button19_NEW.tga", "STAY_CANCEL_OVER")
g_pButton_CancelForPvpMatch:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 96)
})
g_pButton_CancelForPvpMatch:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 96)
})
g_pButton_CancelForPvpMatch:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 96)
})
g_pButton_CancelForPvpMatch:SetShowEnable(false, false)
g_pButton_CancelForPvpMatch:SetCustomMsgMouseUp(PARTY_UI_CUSTOM_MSG.PUCM_PVP_CANCEL_MATCH)

g_pStatic_DUNGEON_STAY = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_DUNGEON_STAY)
g_pStatic_DUNGEON_STAY:SetName("DUNGEON_STAY")

DUNGEON_STAY_OFFSET_X = -370
DUNGEON_STAY_OFFSET_Y = 40
g_pPicture_DUNGEON_STAY_BG = g_pUIDialog:CreatePicture()

g_pStatic_DUNGEON_STAY:AddPicture(g_pPicture_DUNGEON_STAY_BG)
g_pPicture_DUNGEON_STAY_BG:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "DUNGEON_STAY_1")
g_pPicture_DUNGEON_STAY_BG:SetPoint({
	ADD_SIZE_X = 240,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 60,
	LEFT_TOP = D3DXVECTOR2(769 + DUNGEON_STAY_OFFSET_X - 100, 49 + DUNGEON_STAY_OFFSET_Y)
})
g_pStatic_DUNGEON_STAY:AddString({
	MSG = "2분 미만  /",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(810 + DUNGEON_STAY_OFFSET_X, 58 + DUNGEON_STAY_OFFSET_Y),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStatic_DUNGEON_STAY:AddString({
	MSG = "00 : 00 : 00",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_RIGHT,
	POS = D3DXVECTOR2(963 + DUNGEON_STAY_OFFSET_X, 58 + DUNGEON_STAY_OFFSET_Y),
	COLOR = D3DXCOLOR(1, 0, 0, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStatic_DUNGEON_STAY:AddString({
	MSG = "모집 현황 /",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(809 + DUNGEON_STAY_OFFSET_X, 75 + DUNGEON_STAY_OFFSET_Y),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStatic_DUNGEON_STAY:AddString({
	MSG = "2 / 6",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_CENTER,
	POS = D3DXVECTOR2(958 + DUNGEON_STAY_OFFSET_X, 75 + DUNGEON_STAY_OFFSET_Y),
	COLOR = D3DXCOLOR(0.9, 0.9, 0, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStatic_DUNGEON_STAY:AddString({
	MSG = STR_ID_43192,
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_CENTER,
	POS = D3DXVECTOR2(958 + DUNGEON_STAY_OFFSET_X - 50, 75 + DUNGEON_STAY_OFFSET_Y + 20),
	COLOR = D3DXCOLOR(0.9, 0.9, 0, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStatic_DUNGEON_STAY:SetShow(false)

g_pStatic_PVP_STAY = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_PVP_STAY)
g_pStatic_PVP_STAY:SetName("PVP_STAY")

PVP_STAY_OFFSET_X = -370
PVP_STAY_OFFSET_Y = 40
g_pPicture_PVP_STAY_BG = g_pUIDialog:CreatePicture()

g_pStatic_PVP_STAY:AddPicture(g_pPicture_PVP_STAY_BG)
g_pPicture_PVP_STAY_BG:SetTex("DLG_UI_Common_Texture49_NEW.TGA", "DUNGEON_STAY_1")
g_pPicture_PVP_STAY_BG:SetPoint({
	ADD_SIZE_X = 31,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 8,
	LEFT_TOP = D3DXVECTOR2(797 + PVP_STAY_OFFSET_X, 49 + PVP_STAY_OFFSET_Y)
})
g_pStatic_PVP_STAY:AddString({
	MSG = "2분 미만  /",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(810 + PVP_STAY_OFFSET_X, 58 + PVP_STAY_OFFSET_Y),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStatic_PVP_STAY:AddString({
	MSG = "00 : 00 : 00",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_RIGHT,
	POS = D3DXVECTOR2(963 + PVP_STAY_OFFSET_X, 58 + PVP_STAY_OFFSET_Y),
	COLOR = D3DXCOLOR(1, 0, 0, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStatic_PVP_STAY:AddString({
	MSG = "모집 현황 /",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_LEFT,
	POS = D3DXVECTOR2(809 + PVP_STAY_OFFSET_X, 75 + PVP_STAY_OFFSET_Y),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStatic_PVP_STAY:AddString({
	MSG = "2 / 6",
	FONT_INDEX = XUF_DODUM_13_SEMIBOLD,
	FONT_STYLE = FONT_STYLE.FS_SHELL,
	SORT_FLAG = DRAW_TEXT.DT_CENTER,
	POS = D3DXVECTOR2(958 + PVP_STAY_OFFSET_X, 75 + PVP_STAY_OFFSET_Y),
	COLOR = D3DXCOLOR(0.9, 0.9, 0, 1),
	OUTLINE_COLOR = D3DXCOLOR(0.2, 0.2, 0.2, 0)
})
g_pStatic_PVP_STAY:SetShow(false)

g_pStatic_Quest = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_Quest)
g_pStatic_Quest:SetName("ButtonQuestGuide")
g_pStatic_Quest:SetShow(false)

g_pPicture = g_pUIDialog:CreatePicture()

g_pStatic_Quest:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_UI_Common_Texture13.TGA", "Quset_Notice_Small")
g_pPicture:SetPoint({
	ADD_SIZE_X = -10,
	ADD_SIZE_Y = -10,
	LEFT_TOP = D3DXVECTOR2(788, 90)
})
