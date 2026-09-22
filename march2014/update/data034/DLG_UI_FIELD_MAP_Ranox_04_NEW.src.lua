g_pUIDialog:SetName("DLG_UI_FIELD_MAP_RANOX_04")
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
g_pStaticSander_Field04:SetName("Ranox_Field04")

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_Ranox_04_01.DDS", "FIELD_Ranox_BG1")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 256)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_Ranox_04_02.DDS", "FIELD_Ranox_BG2")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(512, 256)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_Ranox_04_03.DDS", "FIELD_Ranox_BG3")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1024, 256)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_Ranox_04_04.DDS", "FIELD_Ranox_BG4")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_Ranox_04_04.DDS", "FIELD_Ranox_BG5")
g_pPicture_bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(512, 0)
})

g_pPicture_bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Field04:AddPicture(g_pPicture_bg1)
g_pPicture_bg1:SetTex("DLG_UI_FIELD_MAP_Ranox_04_05.DDS", "FIELD_Ranox_BG6")
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
	LEFT_TOP = D3DXVECTOR2(55, 450)
})
g_pButtonLeft:SetOverPoint({
	ADD_SIZE_X = 4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 4,
	LEFT_TOP = D3DXVECTOR2(54, 448)
})
g_pButtonLeft:SetDownPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(55, 450)
})
g_pButtonLeft:SetDisablePoint({
	ADD_SIZE_Y = 2,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(58, 450)
})
g_pButtonLeft:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OVER)
g_pButtonLeft:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OUT)
g_pButtonLeft:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PREV_FIELD_MAP)

-- SERV_IRUHADEV_RANOX_PORT (2026-09-19): June2017 has a Right/next-field
-- button here pointing at VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELYSION_REST_00,
-- a forward reference into the Elysion chapter, which is not part of this
-- port and has no enum entry in march2014/data036/Enum.lua. Omitted the
-- whole button, matching March2014's own convention at a chapter-chain
-- end (its own DLG_UI_FIELD_MAP_Sander_04_NEW.lua has no g_pButtonRight
-- block either, since Sander was the last chapter before this port).
