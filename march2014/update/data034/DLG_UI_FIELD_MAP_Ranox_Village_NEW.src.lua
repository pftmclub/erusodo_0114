g_pUIDialog:SetName("DLG_UI_FIELD_MAP_RANOX_VILLAGE")
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

g_pStaticSander_town = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticSander_town)
g_pStaticSander_town:SetName("g_pStaticSander_town")

g_pPicture_Bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_town:AddPicture(g_pPicture_Bg1)
g_pPicture_Bg1:SetTex("DLG_UI_Town_RANOX_Map01.DDS", "RANOX_01_01")
g_pPicture_Bg1:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 256)
})

g_pPicture_Bg2 = g_pUIDialog:CreatePicture()

g_pStaticSander_town:AddPicture(g_pPicture_Bg2)
g_pPicture_Bg2:SetTex("DLG_UI_Town_RANOX_Map02.DDS", "RANOX_01_02")
g_pPicture_Bg2:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(512, 256)
})

g_pPicture_Bg3 = g_pUIDialog:CreatePicture()

g_pStaticSander_town:AddPicture(g_pPicture_Bg3)
g_pPicture_Bg3:SetTex("DLG_UI_Town_RANOX_Map03.DDS", "RANOX_01_03")
g_pPicture_Bg3:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1024, 256)
})

g_pPicture_Bg4 = g_pUIDialog:CreatePicture()

g_pStaticSander_town:AddPicture(g_pPicture_Bg4)
g_pPicture_Bg4:SetTex("DLG_UI_Town_RANOX_Map04.DDS", "RANOX_01_04")
g_pPicture_Bg4:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})

g_pPicture_Bg5 = g_pUIDialog:CreatePicture()

g_pStaticSander_town:AddPicture(g_pPicture_Bg5)
g_pPicture_Bg5:SetTex("DLG_UI_Town_RANOX_Map04.DDS", "RANOX_01_05")
g_pPicture_Bg5:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(512, 0)
})

g_pPicture_Bg6 = g_pUIDialog:CreatePicture()

g_pStaticSander_town:AddPicture(g_pPicture_Bg6)
g_pPicture_Bg6:SetTex("DLG_UI_Town_RANOX_Map05.DDS", "RANOX_01_06")
g_pPicture_Bg6:SetPoint({
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
g_pButtonLeft:SetNormalPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(25, 230)
})
g_pButtonLeft:SetOverPoint({
	ADD_SIZE_X = 4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 4,
	LEFT_TOP = D3DXVECTOR2(24, 228)
})
g_pButtonLeft:SetDownPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(25, 230)
})
g_pButtonLeft:SetDisablePoint({
	ADD_SIZE_Y = 2,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(28, 230)
})
g_pButtonLeft:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OVER)
g_pButtonLeft:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OUT)
g_pButtonLeft:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_PREV_FIELD_MAP)
g_pButtonLeft:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_04)

g_pButtonACC = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonACC)
g_pButtonACC:SetName("shop_acc")
g_pButtonACC:SetNormalTex("MiniMap_Shop_Acc.dds", "MINIMAP_SHOP_ACC")
g_pButtonACC:SetOverTex("MiniMap_Shop_Acc.dds", "MINIMAP_SHOP_ACC")
g_pButtonACC:SetDownTex("MiniMap_Shop_Acc.dds", "MINIMAP_SHOP_ACC")
g_pButtonACC:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(400, 320)
})
g_pButtonACC:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(400, 320)
})
g_pButtonACC:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(400, 320)
})
g_pButtonACC:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonACC:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonACC:AddDummyInt(NPC_UNIT_ID.NUI_SYOTA)

g_pButtonBank = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBank)
g_pButtonBank:SetName("LURIEL")
g_pButtonBank:SetNormalTex("MiniMap_Bank.dds", "MINIMAP_BANK")
g_pButtonBank:SetOverTex("MiniMap_Bank.dds", "MINIMAP_BANK")
g_pButtonBank:SetDownTex("MiniMap_Bank.dds", "MINIMAP_BANK")
g_pButtonBank:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(933, 325)
})
g_pButtonBank:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(933, 325)
})
g_pButtonBank:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(933, 325)
})
g_pButtonBank:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonBank:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonBank:AddDummyInt(NPC_UNIT_ID.NUI_LURIEL)

g_pButtonNPC1 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonNPC1)
g_pButtonNPC1:SetName("EDEL")
g_pButtonNPC1:SetNormalTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC1:SetOverTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC1:SetDownTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC1:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(565, 322)
})
g_pButtonNPC1:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(565, 322)
})
g_pButtonNPC1:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(565, 322)
})
g_pButtonNPC1:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonNPC1:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonNPC1:AddDummyInt(NPC_UNIT_ID.NUI_EDEL)

g_pButtonNPC2 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonNPC2)
g_pButtonNPC2:SetName("IGNIA")
g_pButtonNPC2:SetNormalTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC2:SetOverTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC2:SetDownTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC2:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(400, 575)
})
g_pButtonNPC2:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(400, 575)
})
g_pButtonNPC2:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(400, 575)
})
g_pButtonNPC2:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonNPC2:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonNPC2:AddDummyInt(NPC_UNIT_ID.NUI_IGNIA)

g_pButtonServiceNPC = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonServiceNPC)
g_pButtonServiceNPC:SetName("MU")
g_pButtonServiceNPC:SetNormalTex("MiniMap_Char_NPC.dds", "MINIMAP_CHAR_NPC")
g_pButtonServiceNPC:SetOverTex("MiniMap_Char_NPC.dds", "MINIMAP_CHAR_NPC")
g_pButtonServiceNPC:SetDownTex("MiniMap_Char_NPC.dds", "MINIMAP_CHAR_NPC")
g_pButtonServiceNPC:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(675, 335)
})
g_pButtonServiceNPC:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(675, 335)
})
g_pButtonServiceNPC:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(675, 335)
})
g_pButtonServiceNPC:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonServiceNPC:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonServiceNPC:AddDummyInt(NPC_UNIT_ID.NUI_MU)

g_pButtonPost = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonPost)
g_pButtonPost:SetName("Post")
g_pButtonPost:SetNormalTex("MiniMap_Post.dds", "MiniMap_Post")
g_pButtonPost:SetOverTex("MiniMap_Post.dds", "MiniMap_Post")
g_pButtonPost:SetDownTex("MiniMap_Post.dds", "MiniMap_Post")
g_pButtonPost:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(800, 340)
})
g_pButtonPost:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(800, 340)
})
g_pButtonPost:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(800, 340)
})
g_pButtonPost:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_POSTBOX_MOUSE_OVER)
g_pButtonPost:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_POSTBOX_MOUSE_OUT)

g_pButtonAriel = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonAriel)
g_pButtonAriel:SetName("ARIEL")
g_pButtonAriel:SetNormalTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonAriel:SetOverTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonAriel:SetDownTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonAriel:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(680, 556)
})
g_pButtonAriel:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(680, 556)
})
g_pButtonAriel:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(680, 556)
})
g_pButtonAriel:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonAriel:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonAriel:AddDummyInt(NPC_UNIT_ID.NUI_ARIEL)

g_pButtonCamilla = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonCamilla)
g_pButtonCamilla:SetName("CAMILLA")
g_pButtonCamilla:SetNormalTex("MiniMap_Arena.dds", "MiniMap_Arena")
g_pButtonCamilla:SetOverTex("MiniMap_Arena.dds", "MiniMap_Arena")
g_pButtonCamilla:SetDownTex("MiniMap_Arena.dds", "MiniMap_Arena")
g_pButtonCamilla:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(816, 560)
})
g_pButtonCamilla:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(816, 560)
})
g_pButtonCamilla:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(816, 560)
})
g_pButtonCamilla:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonCamilla:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonCamilla:AddDummyInt(NPC_UNIT_ID.NUI_CAMILLA)

g_pButtonBOARD = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonBOARD)
g_pButtonBOARD:SetName("BOARD")
g_pButtonBOARD:SetNormalTex("MiniMap_BOARD.dds", "MiniMap_BOARD")
g_pButtonBOARD:SetOverTex("MiniMap_BOARD.dds", "MiniMap_BOARD")
g_pButtonBOARD:SetDownTex("MiniMap_BOARD.dds", "MiniMap_BOARD")
g_pButtonBOARD:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(1160, 555)
})
g_pButtonBOARD:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(1160, 555)
})
g_pButtonBOARD:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(1160, 555)
})
g_pButtonBOARD:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonBOARD:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonBOARD:AddDummyInt(NPC_UNIT_ID.NUI_BILLBOARD)

g_pButtonAlche = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonAlche)
g_pButtonAlche:SetName("Shop_Alche")
g_pButtonAlche:SetNormalTex("MiniMap_Shop_Alche.dds", "MiniMap_Shop_Alche")
g_pButtonAlche:SetOverTex("MiniMap_Shop_Alche.dds", "MiniMap_Shop_Alche")
g_pButtonAlche:SetDownTex("MiniMap_Shop_Alche.dds", "MiniMap_Shop_Alche")
g_pButtonAlche:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(1097, 325)
})
g_pButtonAlche:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(1097, 325)
})
g_pButtonAlche:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(1097, 325)
})
g_pButtonAlche:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonAlche:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonAlche:AddDummyInt(NPC_UNIT_ID.NUI_FACEOFF)

g_pButtonWeapon = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonWeapon)
g_pButtonWeapon:SetName("Shop_Weapon")
g_pButtonWeapon:SetNormalTex("MiniMap_Shop_Weapon.dds", "MiniMap_Shop_Weapon")
g_pButtonWeapon:SetOverTex("MiniMap_Shop_Weapon.dds", "MiniMap_Shop_Weapon")
g_pButtonWeapon:SetDownTex("MiniMap_Shop_Weapon.dds", "MiniMap_Shop_Weapon")
g_pButtonWeapon:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(1155, 435)
})
g_pButtonWeapon:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(1155, 435)
})
g_pButtonWeapon:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(1155, 435)
})
g_pButtonWeapon:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonWeapon:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonWeapon:AddDummyInt(NPC_UNIT_ID.NUI_STEEL)

g_pButtonNPC2 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonNPC2)
g_pButtonNPC2:SetName("MUL_MAI")
g_pButtonNPC2:SetNormalTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC2:SetOverTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC2:SetDownTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC2:SetNormalPoint({
	CHANGE_TIME = 0,
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(295, 445),
	COLOR = D3DXCOLOR(1, 1, 1, 1)
})
g_pButtonNPC2:SetOverPoint({
	CHANGE_TIME = 0,
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(295, 445),
	COLOR = D3DXCOLOR(1, 1, 1, 1)
})
g_pButtonNPC2:SetDownPoint({
	CHANGE_TIME = 0,
	ADD_SIZE_X = -20,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(295, 445),
	COLOR = D3DXCOLOR(1, 1, 1, 1)
})
g_pButtonNPC2:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonNPC2:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonNPC2:AddDummyInt(NPC_UNIT_ID.NUI_MUL_MAI_EVENT)

g_pButtonAdams = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonAdams)
g_pButtonAdams:SetName("ADAMS")
g_pButtonAdams:SetNormalTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonAdams:SetOverTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonAdams:SetDownTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonAdams:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(305, 550)
})
g_pButtonAdams:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(305, 550)
})
g_pButtonAdams:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(305, 550)
})
g_pButtonAdams:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonAdams:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonAdams:AddDummyInt(NPC_UNIT_ID.NUI_ADAMS)

g_pButtonNPC2 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonNPC2)
g_pButtonNPC2:SetName("EVENT")
g_pButtonNPC2:SetNormalTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC2:SetOverTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC2:SetDownTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC2:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(324, 447)
})
g_pButtonNPC2:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(324, 447)
})
g_pButtonNPC2:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(324, 447)
})
g_pButtonNPC2:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonNPC2:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonNPC2:AddDummyInt(10000001)

g_pButtonNPC3 = g_pUIDialog:CreateButton()

g_pUIDialog:AddControl(g_pButtonNPC3)
g_pButtonNPC3:SetName("EVENT2")
g_pButtonNPC3:SetNormalTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC3:SetOverTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC3:SetDownTex("MiniMap_Char_NPC.dds", "MiniMap_Char_NPC")
g_pButtonNPC3:SetNormalPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(315, 515)
})
g_pButtonNPC3:SetOverPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(315, 515)
})
g_pButtonNPC3:SetDownPoint({
	ADD_SIZE_X = -20,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -20,
	LEFT_TOP = D3DXVECTOR2(315, 515)
})
g_pButtonNPC3:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OVER)
g_pButtonNPC3:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NPC_MOUSE_OUT)
g_pButtonNPC3:AddDummyInt(10000002)

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
	LEFT_TOP = D3DXVECTOR2(1448, 230)
})
g_pButtonRight:SetOverPoint({
	ADD_SIZE_X = 4,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 4,
	LEFT_TOP = D3DXVECTOR2(1447, 229)
})
g_pButtonRight:SetDownPoint({
	ADD_SIZE_X = 2,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = 2,
	LEFT_TOP = D3DXVECTOR2(1448, 230)
})
g_pButtonRight:SetDisablePoint({
	ADD_SIZE_Y = 2,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(1451, 230)
})
g_pButtonRight:SetCustomMsgMouseOver(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OVER)
g_pButtonRight:SetCustomMsgMouseOut(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_OTHER_FIELD_MOUSE_OUT)
g_pButtonRight:SetCustomMsgMouseUp(WORLD_MAP_UI_CUSTOM_MSG.WMUCM_NEXT_FIELD_MAP)
g_pButtonRight:AddDummyInt(VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RANOX_FIELD_01)
