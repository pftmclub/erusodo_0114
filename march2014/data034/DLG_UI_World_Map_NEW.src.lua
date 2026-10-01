g_pUIDialog:SetName("DLG_UI_WORLD_MAP")
g_pUIDialog:SetPos(0, 0)
g_pUIDialog:SetModal(false)
g_pUIDialog:SetFront(true)
g_pUIDialog:SetLayer(X2_DIALOG_LAYER.XDL_POP_UP)
g_pUIDialog:SetEnableMoveByDrag(true)
g_pUIDialog:SetCloseCustomUIEventID(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_CLOSE_WORLD_MAP)
g_pUIDialog:SetCustomMsgRMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_WORLD_RIGHT_CLICK)
g_pUIDialog:SetCustomMsgStopDragging(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NAVI_WORLD_MAP_STOP_DRAGGING)
g_pUIDialog:SetSize(1800, 1200)
g_pUIDialog:SetEnableDraggingRange(true)
g_pUIDialog:SetDraggingRangeCenter(512, 384)
g_pUIDialog:SetDraggingRange(776, 432)

g_pStaticWorld_Map = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticWorld_Map)
g_pStaticWorld_Map:SetName("g_pStaticWorld_Map")

g_pPicture_Bg1 = g_pUIDialog:CreatePicture()

g_pStaticWorld_Map:AddPicture(g_pPicture_Bg1)
g_pPicture_Bg1:SetTex("DLG_UI_World_Map01.DDS", "world_bg1")
g_pPicture_Bg1:SetPoint({
	ADD_SIZE_X = 88,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 88,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})

g_pPicture_Bg2 = g_pUIDialog:CreatePicture()

g_pStaticWorld_Map:AddPicture(g_pPicture_Bg2)
g_pPicture_Bg2:SetTex("DLG_UI_World_Map02.DDS", "world_bg2")
g_pPicture_Bg2:SetPoint({
	ADD_SIZE_X = 88,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 88,
	LEFT_TOP = D3DXVECTOR2(600, 0)
})

g_pPicture_Bg3 = g_pUIDialog:CreatePicture()

g_pStaticWorld_Map:AddPicture(g_pPicture_Bg3)
g_pPicture_Bg3:SetTex("DLG_UI_World_Map03.DDS", "world_bg3")
g_pPicture_Bg3:SetPoint({
	ADD_SIZE_X = 88,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 88,
	LEFT_TOP = D3DXVECTOR2(1200, 0)
})

g_pPicture_Bg4 = g_pUIDialog:CreatePicture()

g_pStaticWorld_Map:AddPicture(g_pPicture_Bg4)
g_pPicture_Bg4:SetTex("DLG_UI_World_Map04.DDS", "world_bg4")
g_pPicture_Bg4:SetPoint({
	ADD_SIZE_X = 88,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 88,
	LEFT_TOP = D3DXVECTOR2(0, 600)
})

g_pPicture_Bg5 = g_pUIDialog:CreatePicture()

g_pStaticWorld_Map:AddPicture(g_pPicture_Bg5)
g_pPicture_Bg5:SetTex("DLG_UI_World_Map05.DDS", "world_bg5")
g_pPicture_Bg5:SetPoint({
	ADD_SIZE_X = 88,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 88,
	LEFT_TOP = D3DXVECTOR2(600, 600)
})

g_pPicture_Bg6 = g_pUIDialog:CreatePicture()

g_pStaticWorld_Map:AddPicture(g_pPicture_Bg6)
g_pPicture_Bg6:SetTex("DLG_UI_World_Map06.DDS", "world_bg6")
g_pPicture_Bg6:SetPoint({
	ADD_SIZE_X = 88,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 88,
	LEFT_TOP = D3DXVECTOR2(1200, 600)
})

g_pButton_RUBEN_TOWN = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_RUBEN_TOWN)
g_pButton_RUBEN_TOWN:SetName("RUBEN_TOWN")
g_pButton_RUBEN_TOWN:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_NORMAL")
g_pButton_RUBEN_TOWN:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_RUBEN_TOWN:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_RUBEN_TOWN:SetNormalPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(175, 501)
})
g_pButton_RUBEN_TOWN:SetOverPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(167, 493)
})
g_pButton_RUBEN_TOWN:SetDownPoint({
	ADD_SIZE_X = -18,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -18,
	LEFT_TOP = D3DXVECTOR2(169, 495)
})
g_pButton_RUBEN_TOWN:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_RUBEN_TOWN:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_RUBEN_TOWN:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_RUBEN_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_RUBEN)
g_pButton_RUBEN_TOWN:AddDummyPos(D3DXVECTOR3(145, 500, 0))

g_pPicture_RUBEN_TOWN = g_pUIDialog:CreatePicture()

g_pStaticWorld_Map:AddPicture(g_pPicture_RUBEN_TOWN)
g_pPicture_RUBEN_TOWN:SetTex("DLG_UI_Common_Texture71_NEW.tga", "TOWN_RUBEN")
g_pPicture_RUBEN_TOWN:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(163, 526)
})

g_pButton_RUBEN_FIELD_BATTLE_01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_RUBEN_FIELD_BATTLE_01)
g_pButton_RUBEN_FIELD_BATTLE_01:SetName("RUBEN_FIELD_BATTLE_01")
g_pButton_RUBEN_FIELD_BATTLE_01:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_RUBEN_FIELD_BATTLE_01:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RUBEN_FIELD_BATTLE_01:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RUBEN_FIELD_BATTLE_01:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(203, 421)
})
g_pButton_RUBEN_FIELD_BATTLE_01:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(195, 413)
})
g_pButton_RUBEN_FIELD_BATTLE_01:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(197, 415)
})
g_pButton_RUBEN_FIELD_BATTLE_01:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_RUBEN_FIELD_BATTLE_01:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_RUBEN_FIELD_BATTLE_01:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_RUBEN_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RUBEN_FIELD_01)
g_pButton_RUBEN_FIELD_BATTLE_01:AddDummyPos(D3DXVECTOR3(173, 420, 0))

g_pPicture_RUBEN_FIELD_BATTLE_01 = g_pUIDialog:CreatePicture()

g_pStaticWorld_Map:AddPicture(g_pPicture_RUBEN_FIELD_BATTLE_01)
g_pPicture_RUBEN_FIELD_BATTLE_01:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_RUBEN_0")
g_pPicture_RUBEN_FIELD_BATTLE_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(183, 441)
})

g_pButton_ELDER_TOWN = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ELDER_TOWN)
g_pButton_ELDER_TOWN:SetName("ELDER_TOWN")
g_pButton_ELDER_TOWN:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_NORMAL")
g_pButton_ELDER_TOWN:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_ELDER_TOWN:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_ELDER_TOWN:SetNormalPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(297, 417)
})
g_pButton_ELDER_TOWN:SetOverPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(289, 409)
})
g_pButton_ELDER_TOWN:SetDownPoint({
	ADD_SIZE_X = -18,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -18,
	LEFT_TOP = D3DXVECTOR2(291, 411)
})
g_pButton_ELDER_TOWN:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_ELDER_TOWN:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_ELDER_TOWN:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_ELDER_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_ELDER)
g_pButton_ELDER_TOWN:AddDummyPos(D3DXVECTOR3(277, 454, 0))

g_pStatic_ELDER_TOWN = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ELDER_TOWN)

g_pPicture_ELDER_TOWN = g_pUIDialog:CreatePicture()

g_pStatic_ELDER_TOWN:AddPicture(g_pPicture_ELDER_TOWN)
g_pPicture_ELDER_TOWN:SetTex("DLG_UI_Common_Texture71_NEW.tga", "TOWN_ELDER")
g_pPicture_ELDER_TOWN:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(285, 442)
})
g_pStatic_ELDER_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_ELDER)

g_pButton_ELDER_FIELD_BATTLE_01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ELDER_FIELD_BATTLE_01)
g_pButton_ELDER_FIELD_BATTLE_01:SetName("ELDER_FIELD_BATTLE_01")
g_pButton_ELDER_FIELD_BATTLE_01:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_ELDER_FIELD_BATTLE_01:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ELDER_FIELD_BATTLE_01:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ELDER_FIELD_BATTLE_01:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(303, 348)
})
g_pButton_ELDER_FIELD_BATTLE_01:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(295, 340)
})
g_pButton_ELDER_FIELD_BATTLE_01:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(297, 342)
})
g_pButton_ELDER_FIELD_BATTLE_01:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_ELDER_FIELD_BATTLE_01:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_ELDER_FIELD_BATTLE_01:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_ELDER_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_01)
g_pButton_ELDER_FIELD_BATTLE_01:AddDummyPos(D3DXVECTOR3(283, 341, 0))

g_pStatic_ELDER_FIELD_BATTLE_01 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ELDER_FIELD_BATTLE_01)

g_pPicture_ELDER_FIELD_BATTLE_01 = g_pUIDialog:CreatePicture()

g_pStatic_ELDER_FIELD_BATTLE_01:AddPicture(g_pPicture_ELDER_FIELD_BATTLE_01)
g_pPicture_ELDER_FIELD_BATTLE_01:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_ELDER_0")
g_pPicture_ELDER_FIELD_BATTLE_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(279, 368)
})
g_pStatic_ELDER_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_01)

g_pButton_ELDER_FIELD_REST = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ELDER_FIELD_REST)
g_pButton_ELDER_FIELD_REST:SetName("ELDER_FIELD_REST")
g_pButton_ELDER_FIELD_REST:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_REST_NORMAL")
g_pButton_ELDER_FIELD_REST:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_ELDER_FIELD_REST:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_ELDER_FIELD_REST:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(335, 308)
})
g_pButton_ELDER_FIELD_REST:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(327, 300)
})
g_pButton_ELDER_FIELD_REST:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(329, 302)
})
g_pButton_ELDER_FIELD_REST:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_ELDER_FIELD_REST:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_ELDER_FIELD_REST:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_ELDER_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_REST_00)
g_pButton_ELDER_FIELD_REST:AddDummyPos(D3DXVECTOR3(315, 301, 0))

g_pStatic_ELDER_FIELD_REST = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ELDER_FIELD_REST)

g_pPicture_ELDER_FIELD_REST = g_pUIDialog:CreatePicture()

g_pStatic_ELDER_FIELD_REST:AddPicture(g_pPicture_ELDER_FIELD_REST)
g_pPicture_ELDER_FIELD_REST:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_ELDER_REST")
g_pPicture_ELDER_FIELD_REST:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(311, 326)
})
g_pStatic_ELDER_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_REST_00)

g_pButton_ELDER_FIELD_BATTLE_02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ELDER_FIELD_BATTLE_02)
g_pButton_ELDER_FIELD_BATTLE_02:SetName("ELDER_FIELD_BATTLE_02")
g_pButton_ELDER_FIELD_BATTLE_02:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_ELDER_FIELD_BATTLE_02:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ELDER_FIELD_BATTLE_02:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ELDER_FIELD_BATTLE_02:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(377, 324)
})
g_pButton_ELDER_FIELD_BATTLE_02:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(369, 316)
})
g_pButton_ELDER_FIELD_BATTLE_02:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(371, 318)
})
g_pButton_ELDER_FIELD_BATTLE_02:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_ELDER_FIELD_BATTLE_02:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_ELDER_FIELD_BATTLE_02:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_ELDER_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_02)
g_pButton_ELDER_FIELD_BATTLE_02:AddDummyPos(D3DXVECTOR3(399, 356, 0))

g_pStatic_ELDER_FIELD_BATTLE_02 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ELDER_FIELD_BATTLE_02)

g_pPicture_ELDER_FIELD_BATTLE_02 = g_pUIDialog:CreatePicture()

g_pStatic_ELDER_FIELD_BATTLE_02:AddPicture(g_pPicture_ELDER_FIELD_BATTLE_02)
g_pPicture_ELDER_FIELD_BATTLE_02:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_ELDER_1")
g_pPicture_ELDER_FIELD_BATTLE_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(353, 342)
})
g_pStatic_ELDER_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_02)

g_pStatic_ELDER_CLOUD = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ELDER_CLOUD)

g_pPicture_ELDER_CLOUD = g_pUIDialog:CreatePicture()

g_pStatic_ELDER_CLOUD:AddPicture(g_pPicture_ELDER_CLOUD)
g_pPicture_ELDER_CLOUD:SetTex("Cloud_Elder.tga", "Bg_Cloud_Elder")
g_pPicture_ELDER_CLOUD:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(110, 41)
})
g_pStatic_ELDER_CLOUD:AddDummyInt(VILLAGE_MAP_ID.VMI_ELDER)
g_pStatic_ELDER_CLOUD:AddDummyInt(1)

g_pButton_BESMA_TOWN = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_BESMA_TOWN)
g_pButton_BESMA_TOWN:SetName("BESMA_TOWN")
g_pButton_BESMA_TOWN:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_NORMAL")
g_pButton_BESMA_TOWN:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_BESMA_TOWN:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_BESMA_TOWN:SetNormalPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(425, 271)
})
g_pButton_BESMA_TOWN:SetOverPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(417, 263)
})
g_pButton_BESMA_TOWN:SetDownPoint({
	ADD_SIZE_X = -18,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -18,
	LEFT_TOP = D3DXVECTOR2(419, 265)
})
g_pButton_BESMA_TOWN:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_BESMA_TOWN:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_BESMA_TOWN:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_BESMA_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_BESMA)
g_pButton_BESMA_TOWN:AddDummyPos(D3DXVECTOR3(405, 264, 0))

g_pStatic_BESMA_TOWN = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_BESMA_TOWN)

g_pPicture_BESMA_TOWN = g_pUIDialog:CreatePicture()

g_pStatic_BESMA_TOWN:AddPicture(g_pPicture_BESMA_TOWN)
g_pPicture_BESMA_TOWN:SetTex("DLG_UI_Common_Texture71_NEW.tga", "TOWN_BESMA")
g_pPicture_BESMA_TOWN:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(413, 296)
})
g_pStatic_BESMA_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_BESMA)

g_pButton_BESMA_FIELD_BATTLE_01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_BESMA_FIELD_BATTLE_01)
g_pButton_BESMA_FIELD_BATTLE_01:SetName("BESMA_FIELD_BATTLE_01")
g_pButton_BESMA_FIELD_BATTLE_01:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_BESMA_FIELD_BATTLE_01:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_BESMA_FIELD_BATTLE_01:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_BESMA_FIELD_BATTLE_01:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(459, 322)
})
g_pButton_BESMA_FIELD_BATTLE_01:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(451, 314)
})
g_pButton_BESMA_FIELD_BATTLE_01:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(453, 316)
})
g_pButton_BESMA_FIELD_BATTLE_01:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_BESMA_FIELD_BATTLE_01:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_BESMA_FIELD_BATTLE_01:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_BESMA_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_01)
g_pButton_BESMA_FIELD_BATTLE_01:AddDummyPos(D3DXVECTOR3(439, 315, 0))

g_pStatic_BESMA_FIELD_BATTLE_01 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_BESMA_FIELD_BATTLE_01)

g_pPicture_BESMA_FIELD_BATTLE_01 = g_pUIDialog:CreatePicture()

g_pStatic_BESMA_FIELD_BATTLE_01:AddPicture(g_pPicture_BESMA_FIELD_BATTLE_01)
g_pPicture_BESMA_FIELD_BATTLE_01:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_BESMA_0")
g_pPicture_BESMA_FIELD_BATTLE_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(435, 342)
})
g_pStatic_BESMA_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_01)

g_pButton_BESMA_FIELD_REST = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_BESMA_FIELD_REST)
g_pButton_BESMA_FIELD_REST:SetName("BESMA_FIELD_REST")
g_pButton_BESMA_FIELD_REST:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_REST_NORMAL")
g_pButton_BESMA_FIELD_REST:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_BESMA_FIELD_REST:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_BESMA_FIELD_REST:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(492, 364)
})
g_pButton_BESMA_FIELD_REST:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(484, 356)
})
g_pButton_BESMA_FIELD_REST:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(486, 358)
})
g_pButton_BESMA_FIELD_REST:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_BESMA_FIELD_REST:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_BESMA_FIELD_REST:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_BESMA_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_REST_00)
g_pButton_BESMA_FIELD_REST:AddDummyPos(D3DXVECTOR3(472, 357, 0))

g_pStatic_FIELD_BESMA_REST = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_FIELD_BESMA_REST)

g_pPicture_FIELD_BESMA_REST = g_pUIDialog:CreatePicture()

g_pStatic_FIELD_BESMA_REST:AddPicture(g_pPicture_FIELD_BESMA_REST)
g_pPicture_FIELD_BESMA_REST:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_BESMA_REST")
g_pPicture_FIELD_BESMA_REST:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(468, 382)
})
g_pStatic_FIELD_BESMA_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_REST_00)

g_pButton_BESMA_FIELD_BATTLE_02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_BESMA_FIELD_BATTLE_02)
g_pButton_BESMA_FIELD_BATTLE_02:SetName("BESMA_FIELD_BATTLE_02")
g_pButton_BESMA_FIELD_BATTLE_02:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_BESMA_FIELD_BATTLE_02:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_BESMA_FIELD_BATTLE_02:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_BESMA_FIELD_BATTLE_02:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(497, 411)
})
g_pButton_BESMA_FIELD_BATTLE_02:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(489, 403)
})
g_pButton_BESMA_FIELD_BATTLE_02:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(491, 405)
})
g_pButton_BESMA_FIELD_BATTLE_02:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_BESMA_FIELD_BATTLE_02:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_BESMA_FIELD_BATTLE_02:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_BESMA_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_02)
g_pButton_BESMA_FIELD_BATTLE_02:AddDummyPos(D3DXVECTOR3(477, 404, 0))

g_pStatic_BESMA_FIELD_BATTLE_02 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_BESMA_FIELD_BATTLE_02)

g_pPicture_BESMA_FIELD_BATTLE_02 = g_pUIDialog:CreatePicture()

g_pStatic_BESMA_FIELD_BATTLE_02:AddPicture(g_pPicture_BESMA_FIELD_BATTLE_02)
g_pPicture_BESMA_FIELD_BATTLE_02:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_BESMA_2")
g_pPicture_BESMA_FIELD_BATTLE_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(477, 429)
})
g_pStatic_BESMA_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_02)

g_pButton_BESMA_FIELD_BATTLE_03 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_BESMA_FIELD_BATTLE_03)
g_pButton_BESMA_FIELD_BATTLE_03:SetName("BESMA_FIELD_BATTLE_03")
g_pButton_BESMA_FIELD_BATTLE_03:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_BESMA_FIELD_BATTLE_03:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_BESMA_FIELD_BATTLE_03:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_BESMA_FIELD_BATTLE_03:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(585, 333)
})
g_pButton_BESMA_FIELD_BATTLE_03:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(577, 325)
})
g_pButton_BESMA_FIELD_BATTLE_03:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(579, 327)
})
g_pButton_BESMA_FIELD_BATTLE_03:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_BESMA_FIELD_BATTLE_03:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_BESMA_FIELD_BATTLE_03:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_BESMA_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_03)
g_pButton_BESMA_FIELD_BATTLE_03:AddDummyPos(D3DXVECTOR3(565, 326, 0))

g_pStatic_BESMA_FIELD_BATTLE_03 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_BESMA_FIELD_BATTLE_03)

g_pPicture_BESMA_FIELD_BATTLE_03 = g_pUIDialog:CreatePicture()

g_pStatic_BESMA_FIELD_BATTLE_03:AddPicture(g_pPicture_BESMA_FIELD_BATTLE_03)
g_pPicture_BESMA_FIELD_BATTLE_03:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_BESMA_1")
g_pPicture_BESMA_FIELD_BATTLE_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(561, 353)
})
g_pStatic_BESMA_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_03)

g_pStatic_BESMA_CLOUD = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_BESMA_CLOUD)

g_pPicture_BESMA_CLOUD = g_pUIDialog:CreatePicture()

g_pStatic_BESMA_CLOUD:AddPicture(g_pPicture_BESMA_CLOUD)
g_pPicture_BESMA_CLOUD:SetTex("Cloud_Besma_peita.tga", "Bg_Cloud_Besma")
g_pPicture_BESMA_CLOUD:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(354, 147)
})
g_pStatic_BESMA_CLOUD:AddDummyInt(VILLAGE_MAP_ID.VMI_BESMA)
g_pStatic_BESMA_CLOUD:AddDummyInt(1)

g_pButton_ALTERA_TOWN = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ALTERA_TOWN)
g_pButton_ALTERA_TOWN:SetName("ALTERA_TOWN")
g_pButton_ALTERA_TOWN:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_NORMAL")
g_pButton_ALTERA_TOWN:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_ALTERA_TOWN:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_ALTERA_TOWN:SetNormalPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(758, 356)
})
g_pButton_ALTERA_TOWN:SetOverPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(750, 348)
})
g_pButton_ALTERA_TOWN:SetDownPoint({
	ADD_SIZE_X = -18,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -18,
	LEFT_TOP = D3DXVECTOR2(752, 350)
})
g_pButton_ALTERA_TOWN:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_ALTERA_TOWN:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_ALTERA_TOWN:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_ALTERA_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_ALTERA)
g_pButton_ALTERA_TOWN:AddDummyPos(D3DXVECTOR3(738, 349, 0))

g_pStatic_ALTERA_TOWN = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ALTERA_TOWN)

g_pPicture_ALTERA_TOWN = g_pUIDialog:CreatePicture()

g_pStatic_ALTERA_TOWN:AddPicture(g_pPicture_ALTERA_TOWN)
g_pPicture_ALTERA_TOWN:SetTex("DLG_UI_Common_Texture71_NEW.tga", "TOWN_ALTERA")
g_pPicture_ALTERA_TOWN:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(746, 381)
})
g_pStatic_ALTERA_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_ALTERA)

g_pButton_ALTERA_FIELD_BATTLE_01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ALTERA_FIELD_BATTLE_01)
g_pButton_ALTERA_FIELD_BATTLE_01:SetName("ALTERA_FIELD_BATTLE_01")
g_pButton_ALTERA_FIELD_BATTLE_01:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_ALTERA_FIELD_BATTLE_01:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ALTERA_FIELD_BATTLE_01:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ALTERA_FIELD_BATTLE_01:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(805, 339)
})
g_pButton_ALTERA_FIELD_BATTLE_01:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(797, 331)
})
g_pButton_ALTERA_FIELD_BATTLE_01:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(799, 333)
})
g_pButton_ALTERA_FIELD_BATTLE_01:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_ALTERA_FIELD_BATTLE_01:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_ALTERA_FIELD_BATTLE_01:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_ALTERA_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_01)
g_pButton_ALTERA_FIELD_BATTLE_01:AddDummyPos(D3DXVECTOR3(806, 312, 0))

g_pStatic_ALTERA_FIELD_BATTLE_01 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ALTERA_FIELD_BATTLE_01)

g_pPicture_ALTERA_FIELD_BATTLE_01 = g_pUIDialog:CreatePicture()

g_pStatic_ALTERA_FIELD_BATTLE_01:AddPicture(g_pPicture_ALTERA_FIELD_BATTLE_01)
g_pPicture_ALTERA_FIELD_BATTLE_01:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_ALTERA_0")
g_pPicture_ALTERA_FIELD_BATTLE_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(783, 359)
})
g_pStatic_ALTERA_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_01)

g_pButton_ALTERA_FIELD_REST = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ALTERA_FIELD_REST)
g_pButton_ALTERA_FIELD_REST:SetName("ALTERA_FIELD_BATTLE_REST")
g_pButton_ALTERA_FIELD_REST:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_REST_NORMAL")
g_pButton_ALTERA_FIELD_REST:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_ALTERA_FIELD_REST:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_ALTERA_FIELD_REST:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(850, 345)
})
g_pButton_ALTERA_FIELD_REST:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(842, 337)
})
g_pButton_ALTERA_FIELD_REST:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(844, 339)
})
g_pButton_ALTERA_FIELD_REST:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_ALTERA_FIELD_REST:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_ALTERA_FIELD_REST:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_ALTERA_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_REST_00)
g_pButton_ALTERA_FIELD_REST:AddDummyPos(D3DXVECTOR3(852, 319, 0))

g_pStatic_FIELD_ALTERA_REST = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_FIELD_ALTERA_REST)

g_pPicture_FIELD_ALTERA_REST = g_pUIDialog:CreatePicture()

g_pStatic_FIELD_ALTERA_REST:AddPicture(g_pPicture_FIELD_ALTERA_REST)
g_pPicture_FIELD_ALTERA_REST:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_ALTERA_REST")
g_pPicture_FIELD_ALTERA_REST:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(864, 349)
})
g_pStatic_FIELD_ALTERA_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_REST_00)

g_pButton_ALTERA_FIELD_BATTLE_02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ALTERA_FIELD_BATTLE_02)
g_pButton_ALTERA_FIELD_BATTLE_02:SetName("ALTERA_FIELD_BATTLE_02")
g_pButton_ALTERA_FIELD_BATTLE_02:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_ALTERA_FIELD_BATTLE_02:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ALTERA_FIELD_BATTLE_02:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ALTERA_FIELD_BATTLE_02:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(850, 371)
})
g_pButton_ALTERA_FIELD_BATTLE_02:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(842, 363)
})
g_pButton_ALTERA_FIELD_BATTLE_02:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(844, 365)
})
g_pButton_ALTERA_FIELD_BATTLE_02:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_ALTERA_FIELD_BATTLE_02:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_ALTERA_FIELD_BATTLE_02:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_ALTERA_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_02)
g_pButton_ALTERA_FIELD_BATTLE_02:AddDummyPos(D3DXVECTOR3(852, 391, 0))

g_pStatic_ALTERA_FIELD_BATTLE_02 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ALTERA_FIELD_BATTLE_02)

g_pPicture_ALTERA_FIELD_BATTLE_02 = g_pUIDialog:CreatePicture()

g_pStatic_ALTERA_FIELD_BATTLE_02:AddPicture(g_pPicture_ALTERA_FIELD_BATTLE_02)
g_pPicture_ALTERA_FIELD_BATTLE_02:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_ALTERA_1")
g_pPicture_ALTERA_FIELD_BATTLE_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(865, 385)
})
g_pStatic_ALTERA_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_02)

g_pButton_ALTERA_FIELD_BATTLE_03 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_ALTERA_FIELD_BATTLE_03)
g_pButton_ALTERA_FIELD_BATTLE_03:SetName("ALTERA_FIELD_BATTLE_03")
g_pButton_ALTERA_FIELD_BATTLE_03:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_ALTERA_FIELD_BATTLE_03:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ALTERA_FIELD_BATTLE_03:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_ALTERA_FIELD_BATTLE_03:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(813, 375)
})
g_pButton_ALTERA_FIELD_BATTLE_03:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(805, 367)
})
g_pButton_ALTERA_FIELD_BATTLE_03:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(807, 369)
})
g_pButton_ALTERA_FIELD_BATTLE_03:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_ALTERA_FIELD_BATTLE_03:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_ALTERA_FIELD_BATTLE_03:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_ALTERA_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_03)
g_pButton_ALTERA_FIELD_BATTLE_03:AddDummyPos(D3DXVECTOR3(816, 409, 0))

g_pStatic_ALTERA_FIELD_BATTLE_03 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ALTERA_FIELD_BATTLE_03)

g_pPicture_ALTERA_FIELD_BATTLE_03 = g_pUIDialog:CreatePicture()

g_pStatic_ALTERA_FIELD_BATTLE_03:AddPicture(g_pPicture_ALTERA_FIELD_BATTLE_03)
g_pPicture_ALTERA_FIELD_BATTLE_03:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_ALTERA_2")
g_pPicture_ALTERA_FIELD_BATTLE_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(791, 395)
})
g_pStatic_ALTERA_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_03)

g_pStatic_ALTERA_CLOUD = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ALTERA_CLOUD)

g_pPicture_ALTERA_CLOUD1 = g_pUIDialog:CreatePicture()

g_pStatic_ALTERA_CLOUD:AddPicture(g_pPicture_ALTERA_CLOUD1)
g_pPicture_ALTERA_CLOUD1:SetTex("Cloud_peita_Altera.tga", "Bg_Cloud_Altera_01")
g_pPicture_ALTERA_CLOUD1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(678, 258)
})

g_pPicture_ALTERA_CLOUD2 = g_pUIDialog:CreatePicture()

g_pStatic_ALTERA_CLOUD:AddPicture(g_pPicture_ALTERA_CLOUD2)
g_pPicture_ALTERA_CLOUD2:SetTex("Cloud_Etc.tga", "Bg_Cloud_Altera_02")
g_pPicture_ALTERA_CLOUD2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(590, 185)
})
g_pStatic_ALTERA_CLOUD:AddDummyInt(VILLAGE_MAP_ID.VMI_BESMA)
g_pStatic_ALTERA_CLOUD:AddDummyInt(1)

g_pButton_PEITA_FIELD_BATTLE_01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_PEITA_FIELD_BATTLE_01)
g_pButton_PEITA_FIELD_BATTLE_01:SetName("PEITA_FIELD_BATTLE_01")
g_pButton_PEITA_FIELD_BATTLE_01:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_PEITA_FIELD_BATTLE_01:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_PEITA_FIELD_BATTLE_01:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_PEITA_FIELD_BATTLE_01:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(495, 470)
})
g_pButton_PEITA_FIELD_BATTLE_01:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(487, 462)
})
g_pButton_PEITA_FIELD_BATTLE_01:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(489, 464)
})
g_pButton_PEITA_FIELD_BATTLE_01:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_PEITA_FIELD_BATTLE_01:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_PEITA_FIELD_BATTLE_01:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_PEITA_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_01)
g_pButton_PEITA_FIELD_BATTLE_01:AddDummyPos(D3DXVECTOR3(475, 463, 0))

g_pStatic_PEITA_FIELD_BATTLE_01 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_PEITA_FIELD_BATTLE_01)

g_pPicture_PEITA_FIELD_BATTLE_01 = g_pUIDialog:CreatePicture()

g_pStatic_PEITA_FIELD_BATTLE_01:AddPicture(g_pPicture_PEITA_FIELD_BATTLE_01)
g_pPicture_PEITA_FIELD_BATTLE_01:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_PEITA_0")
g_pPicture_PEITA_FIELD_BATTLE_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(473, 490)
})
g_pStatic_PEITA_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_01)

g_pButton_TOWN_PEITA = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_TOWN_PEITA)
g_pButton_TOWN_PEITA:SetName("TOWN_PEITA")
g_pButton_TOWN_PEITA:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_NORMAL")
g_pButton_TOWN_PEITA:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_TOWN_PEITA:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_TOWN_PEITA:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(554, 497)
})
g_pButton_TOWN_PEITA:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(546, 489)
})
g_pButton_TOWN_PEITA:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(548, 491)
})
g_pButton_TOWN_PEITA:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_TOWN_PEITA:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_TOWN_PEITA:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_TOWN_PEITA:AddDummyInt(VILLAGE_MAP_ID.VMI_PEITA)
g_pButton_TOWN_PEITA:AddDummyPos(D3DXVECTOR3(555, 470, 0))

g_pStatic_TOWN_PEITA = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_TOWN_PEITA)

g_pPicture_TOWN_PEITA = g_pUIDialog:CreatePicture()

g_pStatic_TOWN_PEITA:AddPicture(g_pPicture_TOWN_PEITA)
g_pPicture_TOWN_PEITA:SetTex("DLG_UI_Common_Texture71_NEW.tga", "TOWN_PEITA")
g_pPicture_TOWN_PEITA:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(543, 525)
})
g_pStatic_TOWN_PEITA:AddDummyInt(VILLAGE_MAP_ID.VMI_PEITA)

g_pButton_PEITA_FIELD_BATTLE_02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_PEITA_FIELD_BATTLE_02)
g_pButton_PEITA_FIELD_BATTLE_02:SetName("PEITA_FIELD_BATTLE_02")
g_pButton_PEITA_FIELD_BATTLE_02:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_PEITA_FIELD_BATTLE_02:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_PEITA_FIELD_BATTLE_02:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_PEITA_FIELD_BATTLE_02:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(614, 463)
})
g_pButton_PEITA_FIELD_BATTLE_02:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(606, 455)
})
g_pButton_PEITA_FIELD_BATTLE_02:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(608, 457)
})
g_pButton_PEITA_FIELD_BATTLE_02:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_PEITA_FIELD_BATTLE_02:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_PEITA_FIELD_BATTLE_02:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_PEITA_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_02)
g_pButton_PEITA_FIELD_BATTLE_02:AddDummyPos(D3DXVECTOR3(594, 456, 0))

g_pStatic_PEITA_FIELD_BATTLE_02 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_PEITA_FIELD_BATTLE_02)

g_pPicture_PEITA_FIELD_BATTLE_02 = g_pUIDialog:CreatePicture()

g_pStatic_PEITA_FIELD_BATTLE_02:AddPicture(g_pPicture_PEITA_FIELD_BATTLE_02)
g_pPicture_PEITA_FIELD_BATTLE_02:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_PEITA_1")
g_pPicture_PEITA_FIELD_BATTLE_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(590, 483)
})
g_pStatic_PEITA_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_02)

g_pButton_PEITA_FIELD_BATTLE_03 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_PEITA_FIELD_BATTLE_03)
g_pButton_PEITA_FIELD_BATTLE_03:SetName("PEITA_FIELD_BATTLE_03")
g_pButton_PEITA_FIELD_BATTLE_03:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_PEITA_FIELD_BATTLE_03:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_PEITA_FIELD_BATTLE_03:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_PEITA_FIELD_BATTLE_03:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(624, 533)
})
g_pButton_PEITA_FIELD_BATTLE_03:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(616, 525)
})
g_pButton_PEITA_FIELD_BATTLE_03:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(618, 527)
})
g_pButton_PEITA_FIELD_BATTLE_03:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_PEITA_FIELD_BATTLE_03:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_PEITA_FIELD_BATTLE_03:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_PEITA_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_03)
g_pButton_PEITA_FIELD_BATTLE_03:AddDummyPos(D3DXVECTOR3(604, 530, 0))

g_pStatic_PEITA_FIELD_BATTLE_03 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_PEITA_FIELD_BATTLE_03)

g_pPicture_PEITA_FIELD_BATTLE_03 = g_pUIDialog:CreatePicture()

g_pStatic_PEITA_FIELD_BATTLE_03:AddPicture(g_pPicture_PEITA_FIELD_BATTLE_03)
g_pPicture_PEITA_FIELD_BATTLE_03:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_PEITA_2")
g_pPicture_PEITA_FIELD_BATTLE_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(602, 553)
})
g_pStatic_PEITA_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_03)

g_pStatic_PEITA_CLOUD = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_PEITA_CLOUD)

g_pPicture_PEITA_CLOUD1 = g_pUIDialog:CreatePicture()

g_pStatic_PEITA_CLOUD:AddPicture(g_pPicture_PEITA_CLOUD1)
g_pPicture_PEITA_CLOUD1:SetTex("Cloud_Besma_peita.tga", "Bg_Cloud_Peita_01")
g_pPicture_PEITA_CLOUD1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(138, 502)
})

g_pPicture_PEITA_CLOUD2 = g_pUIDialog:CreatePicture()

g_pStatic_PEITA_CLOUD:AddPicture(g_pPicture_PEITA_CLOUD2)
g_pPicture_PEITA_CLOUD2:SetTex("Cloud_peita_Altera.tga", "Bg_Cloud_Peita_02")
g_pPicture_PEITA_CLOUD2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(425, 377)
})
g_pStatic_PEITA_CLOUD:AddDummyInt(VILLAGE_MAP_ID.VMI_PEITA)
g_pStatic_PEITA_CLOUD:AddDummyInt(1)

g_pButton_VELDER_TOWN = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_VELDER_TOWN)
g_pButton_VELDER_TOWN:SetName("VELDER_TOWN")
g_pButton_VELDER_TOWN:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_NORMAL")
g_pButton_VELDER_TOWN:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_VELDER_TOWN:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_VELDER_TOWN:SetNormalPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(678, 483)
})
g_pButton_VELDER_TOWN:SetOverPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(670, 475)
})
g_pButton_VELDER_TOWN:SetDownPoint({
	ADD_SIZE_X = -18,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -18,
	LEFT_TOP = D3DXVECTOR2(672, 477)
})
g_pButton_VELDER_TOWN:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_VELDER_TOWN:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_VELDER_TOWN:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_VELDER_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_VELDER)
g_pButton_VELDER_TOWN:AddDummyPos(D3DXVECTOR3(658, 476, 0))

g_pStatic_VELDER_TOWN = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_VELDER_TOWN)

g_pPicture_VELDER_TOWN = g_pUIDialog:CreatePicture()

g_pStatic_VELDER_TOWN:AddPicture(g_pPicture_VELDER_TOWN)
g_pPicture_VELDER_TOWN:SetTex("DLG_UI_Common_Texture71_NEW.tga", "TOWN_VELDER")
g_pPicture_VELDER_TOWN:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(666, 508)
})
g_pStatic_VELDER_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_VELDER)

g_pButton_VELDER_FIELD_BATTLE_01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_VELDER_FIELD_BATTLE_01)
g_pButton_VELDER_FIELD_BATTLE_01:SetName("VELDER_FIELD_BATTLE_01")
g_pButton_VELDER_FIELD_BATTLE_01:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_VELDER_FIELD_BATTLE_01:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_01:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_01:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(730, 535)
})
g_pButton_VELDER_FIELD_BATTLE_01:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(722, 527)
})
g_pButton_VELDER_FIELD_BATTLE_01:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(724, 529)
})
g_pButton_VELDER_FIELD_BATTLE_01:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_VELDER_FIELD_BATTLE_01:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_VELDER_FIELD_BATTLE_01:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_VELDER_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_01)
g_pButton_VELDER_FIELD_BATTLE_01:AddDummyPos(D3DXVECTOR3(710, 530, 0))

g_pStatic_VELDER_FIELD_BATTLE_01 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_VELDER_FIELD_BATTLE_01)

g_pPicture_VELDER_FIELD_BATTLE_01 = g_pUIDialog:CreatePicture()

g_pStatic_VELDER_FIELD_BATTLE_01:AddPicture(g_pPicture_VELDER_FIELD_BATTLE_01)
g_pPicture_VELDER_FIELD_BATTLE_01:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_VELDER_0")
g_pPicture_VELDER_FIELD_BATTLE_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(706, 555)
})
g_pStatic_VELDER_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_01)

g_pButton_VELDER_FIELD_REST = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_VELDER_FIELD_REST)
g_pButton_VELDER_FIELD_REST:SetName("VELDER_FIELD_REST")
g_pButton_VELDER_FIELD_REST:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_REST_NORMAL")
g_pButton_VELDER_FIELD_REST:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_VELDER_FIELD_REST:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_VELDER_FIELD_REST:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(764, 556)
})
g_pButton_VELDER_FIELD_REST:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(756, 548)
})
g_pButton_VELDER_FIELD_REST:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(758, 550)
})
g_pButton_VELDER_FIELD_REST:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_VELDER_FIELD_REST:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_VELDER_FIELD_REST:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_VELDER_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_REST_00)
g_pButton_VELDER_FIELD_REST:AddDummyPos(D3DXVECTOR3(765, 529, 0))

g_pStatic_FIELD_VELDER_REST = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_FIELD_VELDER_REST)

g_pPicture_FIELD_VELDER_REST = g_pUIDialog:CreatePicture()

g_pStatic_FIELD_VELDER_REST:AddPicture(g_pPicture_FIELD_VELDER_REST)
g_pPicture_FIELD_VELDER_REST:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_VELDER_REST")
g_pPicture_FIELD_VELDER_REST:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(740, 576)
})
g_pStatic_FIELD_VELDER_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_REST_00)

g_pButton_VELDER_FIELD_BATTLE_02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_VELDER_FIELD_BATTLE_02)
g_pButton_VELDER_FIELD_BATTLE_02:SetName("VELDER_FIELD_BATTLE_02")
g_pButton_VELDER_FIELD_BATTLE_02:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_VELDER_FIELD_BATTLE_02:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_02:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_02:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(792, 638)
})
g_pButton_VELDER_FIELD_BATTLE_02:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(784, 630)
})
g_pButton_VELDER_FIELD_BATTLE_02:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(786, 632)
})
g_pButton_VELDER_FIELD_BATTLE_02:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_VELDER_FIELD_BATTLE_02:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_VELDER_FIELD_BATTLE_02:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_VELDER_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_02)
g_pButton_VELDER_FIELD_BATTLE_02:AddDummyPos(D3DXVECTOR3(772, 631, 0))

g_pStatic_VELDER_FIELD_BATTLE_02 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_VELDER_FIELD_BATTLE_02)

g_pPicture_VELDER_FIELD_BATTLE_02 = g_pUIDialog:CreatePicture()

g_pStatic_VELDER_FIELD_BATTLE_02:AddPicture(g_pPicture_VELDER_FIELD_BATTLE_02)
g_pPicture_VELDER_FIELD_BATTLE_02:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_VELDER_1")
g_pPicture_VELDER_FIELD_BATTLE_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(770, 658)
})
g_pStatic_VELDER_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_02)

g_pButton_VELDER_FIELD_BATTLE_03 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_VELDER_FIELD_BATTLE_03)
g_pButton_VELDER_FIELD_BATTLE_03:SetName("VELDER_FIELD_BATTLE_03")
g_pButton_VELDER_FIELD_BATTLE_03:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_VELDER_FIELD_BATTLE_03:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_03:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_03:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(840, 595)
})
g_pButton_VELDER_FIELD_BATTLE_03:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(832, 587)
})
g_pButton_VELDER_FIELD_BATTLE_03:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(834, 589)
})
g_pButton_VELDER_FIELD_BATTLE_03:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_VELDER_FIELD_BATTLE_03:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_VELDER_FIELD_BATTLE_03:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_VELDER_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_03)
g_pButton_VELDER_FIELD_BATTLE_03:AddDummyPos(D3DXVECTOR3(850, 626, 0))

g_pStatic_VELDER_FIELD_BATTLE_03 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_VELDER_FIELD_BATTLE_03)

g_pPicture_VELDER_FIELD_BATTLE_03 = g_pUIDialog:CreatePicture()

g_pStatic_VELDER_FIELD_BATTLE_03:AddPicture(g_pPicture_VELDER_FIELD_BATTLE_03)
g_pPicture_VELDER_FIELD_BATTLE_03:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_VELDER_2")
g_pPicture_VELDER_FIELD_BATTLE_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(818, 615)
})
g_pStatic_VELDER_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_03)

g_pButton_VELDER_FIELD_BATTLE_04 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_VELDER_FIELD_BATTLE_04)
g_pButton_VELDER_FIELD_BATTLE_04:SetName("VELDER_FIELD_BATTLE_04")
g_pButton_VELDER_FIELD_BATTLE_04:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_VELDER_FIELD_BATTLE_04:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_04:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_04:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(816, 673)
})
g_pButton_VELDER_FIELD_BATTLE_04:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(808, 665)
})
g_pButton_VELDER_FIELD_BATTLE_04:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(810, 667)
})
g_pButton_VELDER_FIELD_BATTLE_04:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_VELDER_FIELD_BATTLE_04:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_VELDER_FIELD_BATTLE_04:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_VELDER_FIELD_BATTLE_04:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_04)
g_pButton_VELDER_FIELD_BATTLE_04:AddDummyPos(D3DXVECTOR3(796, 666, 0))

g_pStatic_VELDER_FIELD_BATTLE_04 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_VELDER_FIELD_BATTLE_04)

g_pPicture_VELDER_FIELD_BATTLE_04 = g_pUIDialog:CreatePicture()

g_pStatic_VELDER_FIELD_BATTLE_04:AddPicture(g_pPicture_VELDER_FIELD_BATTLE_04)
g_pPicture_VELDER_FIELD_BATTLE_04:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_VELDER_3")
g_pPicture_VELDER_FIELD_BATTLE_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(794, 693)
})
g_pStatic_VELDER_FIELD_BATTLE_04:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_04)

g_pButton_VELDER_FIELD_BATTLE_SHIP = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_VELDER_FIELD_BATTLE_SHIP)
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetName("VELDER_FIELD_BATTLE_SHIP")
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(379, 713)
})
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(371, 705)
})
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(373, 707)
})
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_VELDER_FIELD_BATTLE_SHIP:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_VELDER_FIELD_BATTLE_SHIP:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_SHIP_STAGE)
g_pButton_VELDER_FIELD_BATTLE_SHIP:AddDummyPos(D3DXVECTOR3(382, 713, 0))

g_pStatic_VELDER_CLOUD = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_VELDER_CLOUD)

g_pPicture_VELDER_CLOUD1 = g_pUIDialog:CreatePicture()

g_pStatic_VELDER_CLOUD:AddPicture(g_pPicture_VELDER_CLOUD1)
g_pPicture_VELDER_CLOUD1:SetTex("Cloud_Velder_01.tga", "Bg_Cloud_Velder_01")
g_pPicture_VELDER_CLOUD1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(55, 579)
})

g_pPicture_VELDER_CLOUD2 = g_pUIDialog:CreatePicture()

g_pStatic_VELDER_CLOUD:AddPicture(g_pPicture_VELDER_CLOUD2)
g_pPicture_VELDER_CLOUD2:SetTex("Cloud_Velder_01.tga", "Bg_Cloud_Velder_02")
g_pPicture_VELDER_CLOUD2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(530, 446)
})

g_pPicture_VELDER_CLOUD3 = g_pUIDialog:CreatePicture()

g_pStatic_VELDER_CLOUD:AddPicture(g_pPicture_VELDER_CLOUD3)
g_pPicture_VELDER_CLOUD3:SetTex("Cloud_Etc.tga", "Bg_Cloud_Velder_03")
g_pPicture_VELDER_CLOUD3:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(846, 669)
})

g_pPicture_VELDER_CLOUD4 = g_pUIDialog:CreatePicture()

g_pStatic_VELDER_CLOUD:AddPicture(g_pPicture_VELDER_CLOUD4)
g_pPicture_VELDER_CLOUD4:SetTex("Cloud_Etc.tga", "Bg_Cloud_Velder_04")
g_pPicture_VELDER_CLOUD4:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1006, 511)
})
g_pStatic_VELDER_CLOUD:AddDummyInt(VILLAGE_MAP_ID.VMI_VELDER)
g_pStatic_VELDER_CLOUD:AddDummyInt(1)

g_pButton_HAMEL_FIELD_BATTLE_SHIP = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_HAMEL_FIELD_BATTLE_SHIP)
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetName("HAMEL_FIELD_BATTLE_SHIP")
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(666, 750)
})
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(658, 742)
})
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(660, 744)
})
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_HAMEL_FIELD_BATTLE_SHIP:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_HAMEL_FIELD_BATTLE_SHIP:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_SHIP_STAGE)
g_pButton_HAMEL_FIELD_BATTLE_SHIP:AddDummyPos(D3DXVECTOR3(696, 750, 0))

g_pButton_HAMEL_TOWN = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_HAMEL_TOWN)
g_pButton_HAMEL_TOWN:SetName("HAMEL_TOWN")
g_pButton_HAMEL_TOWN:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_NORMAL")
g_pButton_HAMEL_TOWN:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_HAMEL_TOWN:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_HAMEL_TOWN:SetNormalPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(324, 858)
})
g_pButton_HAMEL_TOWN:SetOverPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(316, 850)
})
g_pButton_HAMEL_TOWN:SetDownPoint({
	ADD_SIZE_X = -18,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -18,
	LEFT_TOP = D3DXVECTOR2(318, 852)
})
g_pButton_HAMEL_TOWN:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_HAMEL_TOWN:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_HAMEL_TOWN:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_HAMEL_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_HAMEL)
g_pButton_HAMEL_TOWN:AddDummyPos(D3DXVECTOR3(304, 851, 0))

g_pStatic_HAMEL_TOWN = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_HAMEL_TOWN)

g_pPicture_HAMEL_TOWN = g_pUIDialog:CreatePicture()

g_pStatic_HAMEL_TOWN:AddPicture(g_pPicture_HAMEL_TOWN)
g_pPicture_HAMEL_TOWN:SetTex("DLG_UI_Common_Texture71_NEW.tga", "TOWN_HAMEL")
g_pPicture_HAMEL_TOWN:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(312, 883)
})
g_pStatic_HAMEL_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_HAMEL)

g_pButton_HAMEL_FIELD_BATTLE_01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_HAMEL_FIELD_BATTLE_01)
g_pButton_HAMEL_FIELD_BATTLE_01:SetName("HAMEL_FIELD_BATTLE_01")
g_pButton_HAMEL_FIELD_BATTLE_01:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_HAMEL_FIELD_BATTLE_01:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_01:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_01:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(364, 909)
})
g_pButton_HAMEL_FIELD_BATTLE_01:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(356, 901)
})
g_pButton_HAMEL_FIELD_BATTLE_01:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(358, 903)
})
g_pButton_HAMEL_FIELD_BATTLE_01:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_HAMEL_FIELD_BATTLE_01:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_HAMEL_FIELD_BATTLE_01:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_HAMEL_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_01)
g_pButton_HAMEL_FIELD_BATTLE_01:AddDummyPos(D3DXVECTOR3(366, 942, 0))

g_pStatic_HAMEL_FIELD_BATTLE_01 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_HAMEL_FIELD_BATTLE_01)

g_pPicture_HAMEL_FIELD_BATTLE_01 = g_pUIDialog:CreatePicture()

g_pStatic_HAMEL_FIELD_BATTLE_01:AddPicture(g_pPicture_HAMEL_FIELD_BATTLE_01)
g_pPicture_HAMEL_FIELD_BATTLE_01:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_HAMEL_0")
g_pPicture_HAMEL_FIELD_BATTLE_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(342, 929)
})
g_pStatic_HAMEL_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_01)

g_pButton_HAMEL_FIELD_BATTLE_02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_HAMEL_FIELD_BATTLE_02)
g_pButton_HAMEL_FIELD_BATTLE_02:SetName("HAMEL_FIELD_BATTLE_02")
g_pButton_HAMEL_FIELD_BATTLE_02:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_HAMEL_FIELD_BATTLE_02:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_02:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_02:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(420, 909)
})
g_pButton_HAMEL_FIELD_BATTLE_02:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(412, 901)
})
g_pButton_HAMEL_FIELD_BATTLE_02:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(414, 903)
})
g_pButton_HAMEL_FIELD_BATTLE_02:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_HAMEL_FIELD_BATTLE_02:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_HAMEL_FIELD_BATTLE_02:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_HAMEL_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_02)
g_pButton_HAMEL_FIELD_BATTLE_02:AddDummyPos(D3DXVECTOR3(422, 941, 0))

g_pStatic_HAMEL_FIELD_BATTLE_02 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_HAMEL_FIELD_BATTLE_02)

g_pPicture_HAMEL_FIELD_BATTLE_02 = g_pUIDialog:CreatePicture()

g_pStatic_HAMEL_FIELD_BATTLE_02:AddPicture(g_pPicture_HAMEL_FIELD_BATTLE_02)
g_pPicture_HAMEL_FIELD_BATTLE_02:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_HAMEL_1")
g_pPicture_HAMEL_FIELD_BATTLE_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(398, 929)
})
g_pStatic_HAMEL_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_02)

g_pButton_HAMEL_FIELD_REST = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_HAMEL_FIELD_REST)
g_pButton_HAMEL_FIELD_REST:SetName("HAMEL_FIELD_REST")
g_pButton_HAMEL_FIELD_REST:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_REST_NORMAL")
g_pButton_HAMEL_FIELD_REST:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_HAMEL_FIELD_REST:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_HAMEL_FIELD_REST:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(460, 909)
})
g_pButton_HAMEL_FIELD_REST:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(452, 901)
})
g_pButton_HAMEL_FIELD_REST:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(454, 903)
})
g_pButton_HAMEL_FIELD_REST:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_HAMEL_FIELD_REST:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_HAMEL_FIELD_REST:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_HAMEL_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_REST_00)
g_pButton_HAMEL_FIELD_REST:AddDummyPos(D3DXVECTOR3(462, 940, 0))

g_pStatic_HAMEL_FIELD_REST = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_HAMEL_FIELD_REST)

g_pPicture_HAMEL_FIELD_REST = g_pUIDialog:CreatePicture()

g_pStatic_HAMEL_FIELD_REST:AddPicture(g_pPicture_HAMEL_FIELD_REST)
g_pPicture_HAMEL_FIELD_REST:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_HAMEL_REST")
g_pPicture_HAMEL_FIELD_REST:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(446, 926)
})
g_pStatic_HAMEL_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_REST_00)

g_pButton_HAMEL_FIELD_BATTLE_03 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_HAMEL_FIELD_BATTLE_03)
g_pButton_HAMEL_FIELD_BATTLE_03:SetName("HAMEL_FIELD_BATTLE_03")
g_pButton_HAMEL_FIELD_BATTLE_03:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_HAMEL_FIELD_BATTLE_03:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_03:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_03:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(540, 902)
})
g_pButton_HAMEL_FIELD_BATTLE_03:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(532, 894)
})
g_pButton_HAMEL_FIELD_BATTLE_03:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(534, 896)
})
g_pButton_HAMEL_FIELD_BATTLE_03:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_HAMEL_FIELD_BATTLE_03:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_HAMEL_FIELD_BATTLE_03:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_HAMEL_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_03)
g_pButton_HAMEL_FIELD_BATTLE_03:AddDummyPos(D3DXVECTOR3(542, 937, 0))

g_pStatic_HAMEL_FIELD_BATTLE_03 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_HAMEL_FIELD_BATTLE_03)

g_pPicture_HAMEL_FIELD_BATTLE_03 = g_pUIDialog:CreatePicture()

g_pStatic_HAMEL_FIELD_BATTLE_03:AddPicture(g_pPicture_HAMEL_FIELD_BATTLE_03)
g_pPicture_HAMEL_FIELD_BATTLE_03:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_HAMEL_2")
g_pPicture_HAMEL_FIELD_BATTLE_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(518, 922)
})
g_pStatic_HAMEL_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_03)

g_pButton_HAMEL_FIELD_BATTLE_04 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_HAMEL_FIELD_BATTLE_04)
g_pButton_HAMEL_FIELD_BATTLE_04:SetName("HAMEL_FIELD_BATTLE_04")
g_pButton_HAMEL_FIELD_BATTLE_04:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_HAMEL_FIELD_BATTLE_04:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_04:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_HAMEL_FIELD_BATTLE_04:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(498, 848)
})
g_pButton_HAMEL_FIELD_BATTLE_04:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(490, 840)
})
g_pButton_HAMEL_FIELD_BATTLE_04:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(492, 842)
})
g_pButton_HAMEL_FIELD_BATTLE_04:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_HAMEL_FIELD_BATTLE_04:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_HAMEL_FIELD_BATTLE_04:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_HAMEL_FIELD_BATTLE_04:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_04)
g_pButton_HAMEL_FIELD_BATTLE_04:AddDummyPos(D3DXVECTOR3(478, 841, 0))

g_pStatic_HAMEL_FIELD_BATTLE_04 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_HAMEL_FIELD_BATTLE_04)

g_pPicture_HAMEL_FIELD_BATTLE_04 = g_pUIDialog:CreatePicture()

g_pStatic_HAMEL_FIELD_BATTLE_04:AddPicture(g_pPicture_HAMEL_FIELD_BATTLE_04)
g_pPicture_HAMEL_FIELD_BATTLE_04:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_HAMEL_3")
g_pPicture_HAMEL_FIELD_BATTLE_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(476, 868)
})
g_pStatic_HAMEL_FIELD_BATTLE_04:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_04)

g_pStatic_HAMEL_CLOUD = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_HAMEL_CLOUD)

g_pPicture_HAMEL_CLOUD1 = g_pUIDialog:CreatePicture()

g_pStatic_HAMEL_CLOUD:AddPicture(g_pPicture_HAMEL_CLOUD1)
g_pPicture_HAMEL_CLOUD1:SetTex("Cloud_Hamel.tga", "Bg_Cloud_Hamel_01")
g_pPicture_HAMEL_CLOUD1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(196, 754)
})

g_pPicture_HAMEL_CLOUD2 = g_pUIDialog:CreatePicture()

g_pStatic_HAMEL_CLOUD:AddPicture(g_pPicture_HAMEL_CLOUD2)
g_pPicture_HAMEL_CLOUD2:SetTex("Cloud_Hamel.tga", "Bg_Cloud_Hamel_02")
g_pPicture_HAMEL_CLOUD2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(609, 872)
})
g_pStatic_HAMEL_CLOUD:AddDummyInt(VILLAGE_MAP_ID.VMI_HAMEL)
g_pStatic_HAMEL_CLOUD:AddDummyInt(1)

g_pButton_SANDER_TOWN = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_SANDER_TOWN)
g_pButton_SANDER_TOWN:SetName("SANDER_TOWN")
g_pButton_SANDER_TOWN:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_NORMAL")
g_pButton_SANDER_TOWN:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_SANDER_TOWN:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_SANDER_TOWN:SetNormalPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(700, 991)
})
g_pButton_SANDER_TOWN:SetOverPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(692, 983)
})
g_pButton_SANDER_TOWN:SetDownPoint({
	ADD_SIZE_X = -18,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -18,
	LEFT_TOP = D3DXVECTOR2(694, 985)
})
g_pButton_SANDER_TOWN:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_SANDER_TOWN:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_SANDER_TOWN:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_SANDER_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_SANDER)
g_pButton_SANDER_TOWN:AddDummyPos(D3DXVECTOR3(741, 1004, 0))

g_pStatic_SANDER_TOWN = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_SANDER_TOWN)

g_pPicture_SANDER_TOWN = g_pUIDialog:CreatePicture()

g_pStatic_SANDER_TOWN:AddPicture(g_pPicture_SANDER_TOWN)
g_pPicture_SANDER_TOWN:SetTex("DLG_UI_Common_Texture71_NEW.tga", "TOWN_SANDER")
g_pPicture_SANDER_TOWN:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(688, 1016)
})
g_pStatic_SANDER_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_SANDER)

g_pButton_SANDER_FIELD_BATTLE_01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_SANDER_FIELD_BATTLE_01)
g_pButton_SANDER_FIELD_BATTLE_01:SetName("SANDER_FIELD_BATTLE_01")
g_pButton_SANDER_FIELD_BATTLE_01:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_SANDER_FIELD_BATTLE_01:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_SANDER_FIELD_BATTLE_01:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_SANDER_FIELD_BATTLE_01:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(750, 909)
})
g_pButton_SANDER_FIELD_BATTLE_01:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(742, 901)
})
g_pButton_SANDER_FIELD_BATTLE_01:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(744, 903)
})
g_pButton_SANDER_FIELD_BATTLE_01:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_SANDER_FIELD_BATTLE_01:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_SANDER_FIELD_BATTLE_01:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_SANDER_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_01)
g_pButton_SANDER_FIELD_BATTLE_01:AddDummyPos(D3DXVECTOR3(753, 873, 0))

g_pStatic_SANDER_FIELD_BATTLE_01 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_SANDER_FIELD_BATTLE_01)

g_pPicture_SANDER_FIELD_BATTLE_01 = g_pUIDialog:CreatePicture()

g_pStatic_SANDER_FIELD_BATTLE_01:AddPicture(g_pPicture_SANDER_FIELD_BATTLE_01)
g_pPicture_SANDER_FIELD_BATTLE_01:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_SANDER_0")
g_pPicture_SANDER_FIELD_BATTLE_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(728, 929)
})
g_pStatic_SANDER_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_01)

g_pButton_SANDER_FIELD_BATTLE_02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_SANDER_FIELD_BATTLE_02)
g_pButton_SANDER_FIELD_BATTLE_02:SetName("SANDER_FIELD_BATTLE_02")
g_pButton_SANDER_FIELD_BATTLE_02:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_SANDER_FIELD_BATTLE_02:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_SANDER_FIELD_BATTLE_02:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_SANDER_FIELD_BATTLE_02:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(821, 946)
})
g_pButton_SANDER_FIELD_BATTLE_02:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(813, 938)
})
g_pButton_SANDER_FIELD_BATTLE_02:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(815, 940)
})
g_pButton_SANDER_FIELD_BATTLE_02:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_SANDER_FIELD_BATTLE_02:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_SANDER_FIELD_BATTLE_02:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_SANDER_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_02)
g_pButton_SANDER_FIELD_BATTLE_02:AddDummyPos(D3DXVECTOR3(824, 916, 0))

g_pStatic_SANDER_FIELD_BATTLE_02 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_SANDER_FIELD_BATTLE_02)

g_pPicture_SANDER_FIELD_BATTLE_02 = g_pUIDialog:CreatePicture()

g_pStatic_SANDER_FIELD_BATTLE_02:AddPicture(g_pPicture_SANDER_FIELD_BATTLE_02)
g_pPicture_SANDER_FIELD_BATTLE_02:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_NAME_SAND_1")
g_pPicture_SANDER_FIELD_BATTLE_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(788, 965)
})
g_pStatic_SANDER_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_02)

g_pButton_SANDER_FIELD_REST = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_SANDER_FIELD_REST)
g_pButton_SANDER_FIELD_REST:SetName("SANDER_FIELD_REST")
g_pButton_SANDER_FIELD_REST:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_REST_NORMAL")
g_pButton_SANDER_FIELD_REST:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_SANDER_FIELD_REST:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_SANDER_FIELD_REST:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(814, 988)
})
g_pButton_SANDER_FIELD_REST:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(806, 980)
})
g_pButton_SANDER_FIELD_REST:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(808, 982)
})
g_pButton_SANDER_FIELD_REST:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_SANDER_FIELD_REST:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_SANDER_FIELD_REST:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_SANDER_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_REST_00)
g_pButton_SANDER_FIELD_REST:AddDummyPos(D3DXVECTOR3(820, 1022, 0))

g_pStatic_SANDER_FIELD_REST = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_SANDER_FIELD_REST)

g_pPicture_SANDER_FIELD_REST = g_pUIDialog:CreatePicture()

g_pStatic_SANDER_FIELD_REST:AddPicture(g_pPicture_SANDER_FIELD_REST)
g_pPicture_SANDER_FIELD_REST:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_REST_SANDER")
g_pPicture_SANDER_FIELD_REST:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(789, 1005)
})
g_pStatic_SANDER_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_REST_00)

g_pButton_SANDER_FIELD_BATTLE_03 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_SANDER_FIELD_BATTLE_03)
g_pButton_SANDER_FIELD_BATTLE_03:SetName("SANDER_FIELD_BATTLE_03")
g_pButton_SANDER_FIELD_BATTLE_03:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_SANDER_FIELD_BATTLE_03:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_SANDER_FIELD_BATTLE_03:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_SANDER_FIELD_BATTLE_03:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(821, 1036)
})
g_pButton_SANDER_FIELD_BATTLE_03:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(813, 1028)
})
g_pButton_SANDER_FIELD_BATTLE_03:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(815, 1030)
})
g_pButton_SANDER_FIELD_BATTLE_03:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_SANDER_FIELD_BATTLE_03:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_SANDER_FIELD_BATTLE_03:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_SANDER_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_03)
g_pButton_SANDER_FIELD_BATTLE_03:AddDummyPos(D3DXVECTOR3(807, 1029, 0))

g_pStatic_SANDER_FIELD_BATTLE_03 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_SANDER_FIELD_BATTLE_03)

g_pPicture_SANDER_FIELD_BATTLE_03 = g_pUIDialog:CreatePicture()

g_pStatic_SANDER_FIELD_BATTLE_03:AddPicture(g_pPicture_SANDER_FIELD_BATTLE_03)
g_pPicture_SANDER_FIELD_BATTLE_03:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_NAME_SAND_2")
g_pPicture_SANDER_FIELD_BATTLE_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(796, 1055)
})
g_pStatic_SANDER_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_03)

g_pButton_SANDER_FIELD_BATTLE_04 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_SANDER_FIELD_BATTLE_04)
g_pButton_SANDER_FIELD_BATTLE_04:SetName("SANDER_FIELD_BATTLE_03")
g_pButton_SANDER_FIELD_BATTLE_04:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_SANDER_FIELD_BATTLE_04:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_SANDER_FIELD_BATTLE_04:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_SANDER_FIELD_BATTLE_04:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(904, 1047)
})
g_pButton_SANDER_FIELD_BATTLE_04:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(896, 1039)
})
g_pButton_SANDER_FIELD_BATTLE_04:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(898, 1041)
})
g_pButton_SANDER_FIELD_BATTLE_04:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_SANDER_FIELD_BATTLE_04:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_SANDER_FIELD_BATTLE_04:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_SANDER_FIELD_BATTLE_04:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_04)
g_pButton_SANDER_FIELD_BATTLE_04:AddDummyPos(D3DXVECTOR3(907, 1017, 0))

g_pStatic_SANDER_FIELD_BATTLE_04 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_SANDER_FIELD_BATTLE_04)

g_pPicture_SANDER_FIELD_BATTLE_04 = g_pUIDialog:CreatePicture()

g_pStatic_SANDER_FIELD_BATTLE_04:AddPicture(g_pPicture_SANDER_FIELD_BATTLE_04)
g_pPicture_SANDER_FIELD_BATTLE_04:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_NAME_SAND_3")
g_pPicture_SANDER_FIELD_BATTLE_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(880, 1068)
})
g_pStatic_SANDER_FIELD_BATTLE_04:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_04)

g_pStatic_SANDER_CLOUD = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_SANDER_CLOUD)

g_pPicture_SANDER_CLOUD1 = g_pUIDialog:CreatePicture()

g_pStatic_SANDER_CLOUD:AddPicture(g_pPicture_SANDER_CLOUD1)
g_pPicture_SANDER_CLOUD1:SetTex("Cloud_SANDER.tga", "SANDER_CLOUD_1")
g_pPicture_SANDER_CLOUD1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(567, 812)
})
g_pStatic_SANDER_CLOUD:AddDummyInt(VILLAGE_MAP_ID.VMI_SANDER)
g_pStatic_SANDER_CLOUD:AddDummyInt(1)

g_pButton_RANOX_TOWN = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_RANOX_TOWN)
g_pButton_RANOX_TOWN:SetName("RANOX_TOWN")
g_pButton_RANOX_TOWN:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_NORMAL")
g_pButton_RANOX_TOWN:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_RANOX_TOWN:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_TOWN_OVER")
g_pButton_RANOX_TOWN:SetNormalPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(1211, 976)
})
g_pButton_RANOX_TOWN:SetOverPoint({
	ADD_SIZE_X = -14,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -14,
	LEFT_TOP = D3DXVECTOR2(1203, 968)
})
g_pButton_RANOX_TOWN:SetDownPoint({
	ADD_SIZE_X = -18,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -18,
	LEFT_TOP = D3DXVECTOR2(1205, 970)
})
g_pButton_RANOX_TOWN:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_RANOX_TOWN:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_RANOX_TOWN:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_RANOX_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_RANOX)
g_pButton_RANOX_TOWN:AddDummyPos(D3DXVECTOR3(1197, 970, 0))

g_pStatic_RANOX_TOWN = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_RANOX_TOWN)

g_pPicture_RANOX_TOWN = g_pUIDialog:CreatePicture()

g_pStatic_RANOX_TOWN:AddPicture(g_pPicture_RANOX_TOWN)
g_pPicture_RANOX_TOWN:SetTex("DLG_UI_Common_Texture71_NEW.tga", "TOWN_RANOX")
g_pPicture_RANOX_TOWN:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1200, 1001)
})
g_pStatic_RANOX_TOWN:AddDummyInt(VILLAGE_MAP_ID.VMI_RANOX)

g_pButton_RANOX_FIELD_BATTLE_01 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_RANOX_FIELD_BATTLE_01)
g_pButton_RANOX_FIELD_BATTLE_01:SetName("RANOX_FIELD_BATTLE_01")
g_pButton_RANOX_FIELD_BATTLE_01:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_RANOX_FIELD_BATTLE_01:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RANOX_FIELD_BATTLE_01:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RANOX_FIELD_BATTLE_01:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1283, 940)
})
g_pButton_RANOX_FIELD_BATTLE_01:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1275, 932)
})
g_pButton_RANOX_FIELD_BATTLE_01:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(1277, 934)
})
g_pButton_RANOX_FIELD_BATTLE_01:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_RANOX_FIELD_BATTLE_01:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_RANOX_FIELD_BATTLE_01:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_RANOX_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_01)
g_pButton_RANOX_FIELD_BATTLE_01:AddDummyPos(D3DXVECTOR3(1269, 934, 0))

g_pStatic_RANOX_FIELD_BATTLE_01 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_RANOX_FIELD_BATTLE_01)

g_pPicture_RANOX_FIELD_BATTLE_01 = g_pUIDialog:CreatePicture()

g_pStatic_RANOX_FIELD_BATTLE_01:AddPicture(g_pPicture_RANOX_FIELD_BATTLE_01)
g_pPicture_RANOX_FIELD_BATTLE_01:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_NAME_RANOX_0")
g_pPicture_RANOX_FIELD_BATTLE_01:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1258, 961)
})
g_pStatic_RANOX_FIELD_BATTLE_01:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_01)

g_pButton_RANOX_FIELD_BATTLE_02 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_RANOX_FIELD_BATTLE_02)
g_pButton_RANOX_FIELD_BATTLE_02:SetName("RANOX_FIELD_BATTLE_02")
g_pButton_RANOX_FIELD_BATTLE_02:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_RANOX_FIELD_BATTLE_02:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RANOX_FIELD_BATTLE_02:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RANOX_FIELD_BATTLE_02:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1310, 862)
})
g_pButton_RANOX_FIELD_BATTLE_02:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1302, 854)
})
g_pButton_RANOX_FIELD_BATTLE_02:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(1304, 856)
})
g_pButton_RANOX_FIELD_BATTLE_02:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_RANOX_FIELD_BATTLE_02:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_RANOX_FIELD_BATTLE_02:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_RANOX_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_02)
g_pButton_RANOX_FIELD_BATTLE_02:AddDummyPos(D3DXVECTOR3(1332, 855, 0))

g_pStatic_RANOX_FIELD_BATTLE_02 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_RANOX_FIELD_BATTLE_02)

g_pPicture_RANOX_FIELD_BATTLE_02 = g_pUIDialog:CreatePicture()

g_pStatic_RANOX_FIELD_BATTLE_02:AddPicture(g_pPicture_RANOX_FIELD_BATTLE_02)
g_pPicture_RANOX_FIELD_BATTLE_02:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_NAME_RANOX_1")
g_pPicture_RANOX_FIELD_BATTLE_02:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1292, 881)
})
g_pStatic_RANOX_FIELD_BATTLE_02:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_02)

g_pButton_RANOX_FIELD_REST = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_RANOX_FIELD_REST)
g_pButton_RANOX_FIELD_REST:SetName("RANOX_FIELD_REST")
g_pButton_RANOX_FIELD_REST:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_REST_NORMAL")
g_pButton_RANOX_FIELD_REST:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_RANOX_FIELD_REST:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_REST_OVER")
g_pButton_RANOX_FIELD_REST:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1392, 860)
})
g_pButton_RANOX_FIELD_REST:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1384, 852)
})
g_pButton_RANOX_FIELD_REST:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(1386, 854)
})
g_pButton_RANOX_FIELD_REST:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_RANOX_FIELD_REST:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_RANOX_FIELD_REST:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_RANOX_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_REST_00)
g_pButton_RANOX_FIELD_REST:AddDummyPos(D3DXVECTOR3(1372, 855, 0))

g_pStatic_RANOX_FIELD_REST = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_RANOX_FIELD_REST)

g_pPicture_RANOX_FIELD_REST = g_pUIDialog:CreatePicture()

g_pStatic_RANOX_FIELD_REST:AddPicture(g_pPicture_RANOX_FIELD_REST)
g_pPicture_RANOX_FIELD_REST:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_REST_RANOX")
g_pPicture_RANOX_FIELD_REST:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1370, 877)
})
g_pStatic_RANOX_FIELD_REST:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_REST_00)

g_pButton_RANOX_FIELD_BATTLE_03 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_RANOX_FIELD_BATTLE_03)
g_pButton_RANOX_FIELD_BATTLE_03:SetName("RANOX_FIELD_BATTLE_03")
g_pButton_RANOX_FIELD_BATTLE_03:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_RANOX_FIELD_BATTLE_03:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RANOX_FIELD_BATTLE_03:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RANOX_FIELD_BATTLE_03:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1372, 779)
})
g_pButton_RANOX_FIELD_BATTLE_03:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1364, 771)
})
g_pButton_RANOX_FIELD_BATTLE_03:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(1366, 773)
})
g_pButton_RANOX_FIELD_BATTLE_03:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_RANOX_FIELD_BATTLE_03:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_RANOX_FIELD_BATTLE_03:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_RANOX_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_03)
g_pButton_RANOX_FIELD_BATTLE_03:AddDummyPos(D3DXVECTOR3(1390, 775, 0))

g_pStatic_RANOX_FIELD_BATTLE_03 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_RANOX_FIELD_BATTLE_03)

g_pPicture_RANOX_FIELD_BATTLE_03 = g_pUIDialog:CreatePicture()

g_pStatic_RANOX_FIELD_BATTLE_03:AddPicture(g_pPicture_RANOX_FIELD_BATTLE_03)
g_pPicture_RANOX_FIELD_BATTLE_03:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_NAME_RANOX_2")
g_pPicture_RANOX_FIELD_BATTLE_03:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1346, 798)
})
g_pStatic_RANOX_FIELD_BATTLE_03:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_03)

g_pButton_RANOX_FIELD_BATTLE_04 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButton_RANOX_FIELD_BATTLE_04)
g_pButton_RANOX_FIELD_BATTLE_04:SetName("RANOX_FIELD_BATTLE_04")
g_pButton_RANOX_FIELD_BATTLE_04:SetNormalTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_NORMAL")
g_pButton_RANOX_FIELD_BATTLE_04:SetOverTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RANOX_FIELD_BATTLE_04:SetDownTex("DLG_Common_Button02_NEW.tga", "BTN_BATTLE_OVER")
g_pButton_RANOX_FIELD_BATTLE_04:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1454, 735)
})
g_pButton_RANOX_FIELD_BATTLE_04:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -12,
	LEFT_TOP = D3DXVECTOR2(1446, 727)
})
g_pButton_RANOX_FIELD_BATTLE_04:SetDownPoint({
	ADD_SIZE_X = -16,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -16,
	LEFT_TOP = D3DXVECTOR2(1448, 729)
})
g_pButton_RANOX_FIELD_BATTLE_04:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_CLICKED)
g_pButton_RANOX_FIELD_BATTLE_04:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OVER)
g_pButton_RANOX_FIELD_BATTLE_04:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_VILLAGE_MOUSE_OUT)
g_pButton_RANOX_FIELD_BATTLE_04:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_04)
g_pButton_RANOX_FIELD_BATTLE_04:AddDummyPos(D3DXVECTOR3(1434, 732, 0))

g_pStatic_RANOX_FIELD_BATTLE_04 = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_RANOX_FIELD_BATTLE_04)

g_pPicture_RANOX_FIELD_BATTLE_04 = g_pUIDialog:CreatePicture()

g_pStatic_RANOX_FIELD_BATTLE_04:AddPicture(g_pPicture_RANOX_FIELD_BATTLE_04)
g_pPicture_RANOX_FIELD_BATTLE_04:SetTex("DLG_UI_Common_Texture71_NEW.tga", "FIELD_NAME_RANOX_3")
g_pPicture_RANOX_FIELD_BATTLE_04:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1428, 754)
})
g_pStatic_RANOX_FIELD_BATTLE_04:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_04)

g_pStatic_RANOX_CLOUD = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_RANOX_CLOUD)

g_pPicture_RANOX_CLOUD1 = g_pUIDialog:CreatePicture()

g_pStatic_RANOX_CLOUD:AddPicture(g_pPicture_RANOX_CLOUD1)
g_pPicture_RANOX_CLOUD1:SetTex("Cloud_Ranox.tga", "Bg_Cloud_Ranox")
g_pPicture_RANOX_CLOUD1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1070, 627)
})
g_pStatic_RANOX_CLOUD:AddDummyInt(VILLAGE_MAP_ID.VMI_RANOX)
g_pStatic_RANOX_CLOUD:AddDummyInt(1)

-- SERV_IRUHADEV_RANOX_PORT (2026-10-01): June's world-map tiles (data057,
-- replacing the old cloud-baked-in art) draw Elysion's floating-island medallion
-- bare at (1300,262). June covers it with its own g_pStatic_ELISION_CLOUD, keyed
-- to VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELYSION_REST_00 so it unhides with that
-- town's level gate (X2WorldMapUI.cpp:603-660's generic per-village cloud logic).
-- March has no VMI_ELYSION/VMI_BATTLE_FIELD_ELYSION_* entries (nil), and passing
-- a nil DummyInt aborts this dialog script. Elysion is unported content, so this
-- cloud must never hide: DummyInt(0) is a literal sentinel (99999) that matches
-- no button's DummyInt(0) in this dialog, so UpdateWorldMap's button-driven
-- show/hide loop never touches this static and it stays permanently shown.
-- Position and rect are June's own (Cloud_Elision.tga's one key, BG_CLOUD_ELISION).
g_pStatic_ELISION_CLOUD = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_ELISION_CLOUD)

g_pPicture_ELISION_CLOUD1 = g_pUIDialog:CreatePicture()

g_pStatic_ELISION_CLOUD:AddPicture(g_pPicture_ELISION_CLOUD1)
g_pPicture_ELISION_CLOUD1:SetTex("Cloud_Elision.tga", "Bg_Cloud_Elision")
g_pPicture_ELISION_CLOUD1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1300, 262)
})
g_pStatic_ELISION_CLOUD:AddDummyInt(99999)
g_pStatic_ELISION_CLOUD:AddDummyInt(1)

g_pButtonMyChar = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonMyChar)
g_pButtonMyChar:SetName("Button_Mychar")
g_pButtonMyChar:SetNormalTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_My")
g_pButtonMyChar:SetOverTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_My")
g_pButtonMyChar:SetDownTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_My")
g_pButtonMyChar:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonMyChar:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonMyChar:SetDownPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonMyChar:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_MYCHAR_MOUSE_OVER)
g_pButtonMyChar:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_MYCHAR_MOUSE_OUT)

g_pButtonPartyMember0 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonPartyMember0)
g_pButtonPartyMember0:SetName("Button_PartyMember0")
g_pButtonPartyMember0:SetNormalTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_Party")
g_pButtonPartyMember0:SetOverTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_Party")
g_pButtonPartyMember0:SetDownTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_Party")
g_pButtonPartyMember0:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonPartyMember0:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonPartyMember0:SetDownPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonPartyMember0:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PARTY_MEMBER_MOUSE_OVER)
g_pButtonPartyMember0:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PARTY_MEMBER_MOUSE_OUT)
g_pButtonPartyMember0:AddDummyInt(0)

g_pButtonPartyMember1 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonPartyMember1)
g_pButtonPartyMember1:SetName("Button_PartyMember1")
g_pButtonPartyMember1:SetNormalTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_Party")
g_pButtonPartyMember1:SetOverTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_Party")
g_pButtonPartyMember1:SetDownTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_Party")
g_pButtonPartyMember1:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonPartyMember1:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonPartyMember1:SetDownPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonPartyMember1:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PARTY_MEMBER_MOUSE_OVER)
g_pButtonPartyMember1:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PARTY_MEMBER_MOUSE_OUT)
g_pButtonPartyMember1:AddDummyInt(1)

g_pButtonPartyMember2 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonPartyMember2)
g_pButtonPartyMember2:SetName("Button_PartyMember2")
g_pButtonPartyMember2:SetNormalTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_Party")
g_pButtonPartyMember2:SetOverTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_Party")
g_pButtonPartyMember2:SetDownTex("DLG_UI_Common_Texture70_NEW.TGA", "MiniMap_Char_Party")
g_pButtonPartyMember2:SetNormalPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonPartyMember2:SetOverPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonPartyMember2:SetDownPoint({
	ADD_SIZE_X = -12,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -22,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})
g_pButtonPartyMember2:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PARTY_MEMBER_MOUSE_OVER)
g_pButtonPartyMember2:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PARTY_MEMBER_MOUSE_OUT)
g_pButtonPartyMember2:AddDummyInt(2)
