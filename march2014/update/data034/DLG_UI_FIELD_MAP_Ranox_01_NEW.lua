g_pUIDialog:SetName("DLG_UI_FIELD_MAP_RANOX_01")
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

g_pStaticSander_Field04 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticSander_Field04)
g_pStaticSander_Field04:SetName("Sander_Field04")

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_RANOX_01_01.DDS", "TOWN_RANOX_BG1")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 256)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_RANOX_01_02.DDS", "TOWN_RANOX_BG2")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(512, 256)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_RANOX_01_03.DDS", "TOWN_RANOX_BG3")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1024, 256)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_RANOX_01_04.DDS", "TOWN_RANOX_BG4")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_RANOX_01_04.DDS", "TOWN_RANOX_BG5")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(512, 0)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_RANOX_01_05.DDS", "TOWN_RANOX_BG6")
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
g_pButtonLeft:SetOffsetPos(35, 15)
g_pButtonLeft:SetNormalPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(60, 430)
})
g_pButtonLeft:SetOverPoint({
	ADD_SIZE_X = 4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 4,
	LEFT_TOP = D3DXVECTOR2(59, 428)
})
g_pButtonLeft:SetDownPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(60, 430)
})
g_pButtonLeft:SetDisablePoint({
	ADD_SIZE_Y = 2,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(63, 430)
})
g_pButtonLeft:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OVER)
g_pButtonLeft:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OUT)
g_pButtonLeft:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PREV_FIELD_MAP)
g_pButtonLeft:AddDummyInt(VILLAGE_MAP_ID.VMI_RANOX)

g_pButtonRight = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonRight)
g_pButtonRight:SetName("Right_Cusor")
g_pButtonRight:SetNormalTex("DLG_UI_Button04.tga", "R_gate")
g_pButtonRight:SetOverTex("DLG_UI_Button04.tga", "R_gate")
g_pButtonRight:SetDownTex("DLG_UI_Button04.tga", "R_gate")
g_pButtonRight:SetDisableTex("DLG_UI_Button04.tga", "R_gate_noactive")
g_pButtonRight:SetNormalPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(1448, 340)
})
g_pButtonRight:SetOverPoint({
	ADD_SIZE_X = 4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 4,
	LEFT_TOP = D3DXVECTOR2(1447, 339)
})
g_pButtonRight:SetDownPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(1448, 340)
})
g_pButtonRight:SetDisablePoint({
	ADD_SIZE_Y = 2,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1451, 340)
})
g_pButtonRight:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OVER)
g_pButtonRight:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OUT)
g_pButtonRight:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NEXT_FIELD_MAP)
g_pButtonRight:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_02)
