g_pUIDialog:SetName("DLG_UI_FIELD_MAP_SANDER_Rest")
g_pUIDialog:SetPos(0, 0)
g_pUIDialog:SetModal(false)
g_pUIDialog:SetFront(true)
g_pUIDialog:SetLayer(X2_DIALOG_LAYER.XDL_POP_UP)
g_pUIDialog:SetEnableMoveByDrag(true)
g_pUIDialog:SetCloseCustomUIEventID(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_FIELD_RIGHT_CLICK)
g_pUIDialog:SetCustomMsgRMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_FIELD_RIGHT_CLICK)
g_pUIDialog:SetCustomMsgStopDragging(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NAVI_FIELD_MAP_STOP_DRAGGING)
g_pUIDialog:SetSize(1536, 768)
g_pUIDialog:SetEnableDraggingRange(true)
g_pUIDialog:SetDraggingRangeCenter(512, 384)
g_pUIDialog:SetDraggingRange(512, 0)

g_pStaticHamel_Rest = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticHamel_Rest)
g_pStaticHamel_Rest:SetName("Hamel_Rest")

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticHamel_Rest:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_Rest_Ranox_Map01.DDS", "TOWN_Ranox_BG1")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 256)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticHamel_Rest:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_Rest_Ranox_Map02.DDS", "TOWN_Ranox_BG2")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(512, 256)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticHamel_Rest:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_Rest_Ranox_Map03.dds", "TOWN_Ranox_BG3")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1024, 256)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticHamel_Rest:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_Rest_Ranox_Map04.DDS", "TOWN_Ranox_BG4")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticHamel_Rest:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_Rest_Ranox_Map04.DDS", "TOWN_Ranox_BG5")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(512, 0)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticHamel_Rest:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_Rest_Ranox_Map05.DDS", "TOWN_Ranox_BG6")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1024, 0)
})

g_pButtonLeft = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonLeft)
g_pButtonLeft:SetName("LEFT_cusor")
g_pButtonLeft:SetNormalTex("DLG_UI_Button04.tga", "l_gate")
g_pButtonLeft:SetOverTex("DLG_UI_Button04.tga", "l_gate")
g_pButtonLeft:SetDownTex("DLG_UI_Button04.tga", "l_gate")
g_pButtonLeft:SetDisableTex("DLG_UI_Button04.tga", "l_gate_noactive")
g_pButtonLeft:SetOffsetPos(10, 25)
g_pButtonLeft:SetNormalPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(50, 440)
})
g_pButtonLeft:SetOverPoint({
	ADD_SIZE_X = 4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 4,
	LEFT_TOP = D3DXVECTOR2(49, 438)
})
g_pButtonLeft:SetDownPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(50, 440)
})
g_pButtonLeft:SetDisablePoint({
	ADD_SIZE_Y = 2,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(53, 440)
})
g_pButtonLeft:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OVER)
g_pButtonLeft:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OUT)
g_pButtonLeft:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PREV_FIELD_MAP)
g_pButtonLeft:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_02)

g_pButtonHelren = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonHelren)
g_pButtonHelren:SetName("HELREN")
g_pButtonHelren:SetNormalTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonHelren:SetOverTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonHelren:SetDownTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonHelren:SetOffsetPos(-340, 30)
g_pButtonHelren:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(830, 400)
})
g_pButtonHelren:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(830, 400)
})
g_pButtonHelren:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(830, 400)
})
g_pButtonHelren:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonHelren:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonHelren:AddDummyInt(NPC_UNIT_ID.NUI_HELLEN)

g_pButtonPost = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonPost)
g_pButtonPost:SetName("Post")
g_pButtonPost:SetNormalTex("MiniMap_Post.dds", "MiniMap_Post")
g_pButtonPost:SetOverTex("MiniMap_Post.dds", "MiniMap_Post")
g_pButtonPost:SetDownTex("MiniMap_Post.dds", "MiniMap_Post")
g_pButtonPost:SetOffsetPos(180, 35)
g_pButtonPost:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(450, 400)
})
g_pButtonPost:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(450, 400)
})
g_pButtonPost:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(450, 400)
})
g_pButtonPost:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_POSTBOX_MOUSE_OVER)
g_pButtonPost:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_POSTBOX_MOUSE_OUT)

g_pButtonBOARD = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBOARD)
g_pButtonBOARD:SetName("BOARD")
g_pButtonBOARD:SetNormalTex("MiniMap_BOARD.dds", "MiniMap_BOARD")
g_pButtonBOARD:SetOverTex("MiniMap_BOARD.dds", "MiniMap_BOARD")
g_pButtonBOARD:SetDownTex("MiniMap_BOARD.dds", "MiniMap_BOARD")
g_pButtonBOARD:SetOffsetPos(260, 40)
g_pButtonBOARD:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(625, 395)
})
g_pButtonBOARD:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(625, 395)
})
g_pButtonBOARD:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(625, 395)
})
g_pButtonBOARD:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonBOARD:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonBOARD:AddDummyInt(NPC_UNIT_ID.NUI_BILLBOARD)

g_pButtonGlave = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonGlave)
g_pButtonGlave:SetName("GLAVE")
g_pButtonGlave:SetNormalTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonGlave:SetOverTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonGlave:SetDownTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonGlave:SetOffsetPos(95, 50)
g_pButtonGlave:SetNormalPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(995, 385)
})
g_pButtonGlave:SetOverPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(995, 385)
})
g_pButtonGlave:SetDownPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(995, 385)
})
g_pButtonGlave:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonGlave:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonGlave:AddDummyInt(NPC_UNIT_ID.NUI_GLAVE)

g_pButtonRight = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonRight)
g_pButtonRight:SetName("Right_Cusor")
g_pButtonRight:SetNormalTex("DLG_UI_Button04.tga", "R_gate")
g_pButtonRight:SetOverTex("DLG_UI_Button04.tga", "R_gate")
g_pButtonRight:SetDownTex("DLG_UI_Button04.tga", "R_gate")
g_pButtonRight:SetDisableTex("DLG_UI_Button04.tga", "R_gate_noactive")
g_pButtonRight:SetOffsetPos(10, 15)
g_pButtonRight:SetNormalPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(1405, 390)
})
g_pButtonRight:SetOverPoint({
	ADD_SIZE_X = 4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 4,
	LEFT_TOP = D3DXVECTOR2(1404, 388)
})
g_pButtonRight:SetDownPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(1405, 390)
})
g_pButtonRight:SetDisablePoint({
	ADD_SIZE_Y = 2,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1408, 390)
})
g_pButtonRight:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OVER)
g_pButtonRight:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OUT)
g_pButtonRight:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NEXT_FIELD_MAP)
g_pButtonRight:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_03)
