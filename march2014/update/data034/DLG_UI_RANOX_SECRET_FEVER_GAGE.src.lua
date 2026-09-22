g_pUIDialog:SetName("RanoxSecretFeverGage")
g_pUIDialog:SetPos(400, 50)
g_pUIDialog:SetModal(true)
g_pUIDialog:SetFront(true)
g_pUIDialog:SetLayer(X2_DIALOG_LAYER.XDL_NORMAL_1)

MAIN_POS_X = 0
MAIN_POS_Y = 0
MASTERY_POS_X = MAIN_POS_X - 20
MASTERY_POS_Y = MAIN_POS_Y
MASTERY_STEP_POS_X = MASTERY_POS_X + 50
MASTERY_STEP_POS_Y = MASTERY_POS_Y + 33
MASTERY_STEP_GAP_X = 30
ENEMY_NUM_POS_X = MASTERY_STEP_POS_X + MASTERY_STEP_GAP_X * 5 + 7
ENEMY_NUM_POS_Y = MASTERY_STEP_POS_Y - 6
ENEMY_NUM_MAX_POS_X = ENEMY_NUM_POS_X + 18
ENEMY_NUM_MAX_POS_Y = ENEMY_NUM_POS_Y
g_pStatic_RanoxFeverGage = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_RanoxFeverGage)
g_pStatic_RanoxFeverGage:SetName("g_pStatic_RanoxFeverGage")

g_pPicture_RanoxFeverGage = g_pUIDialog:CreatePicture()

g_pStatic_RanoxFeverGage:AddPicture(g_pPicture_RanoxFeverGage)
g_pPicture_RanoxFeverGage:SetTex("DLG_UI_Image_08.tga", "MonsterFerver")
g_pPicture_RanoxFeverGage:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MASTERY_POS_X, MASTERY_POS_Y)
})

g_pPicture_RanoxFeverGage = g_pUIDialog:CreatePicture()

g_pStatic_RanoxFeverGage:AddPicture(g_pPicture_RanoxFeverGage)
g_pPicture_RanoxFeverGage:SetTex("DLG_UI_Image_01.tga", "MonsterFever_1")
g_pPicture_RanoxFeverGage:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MASTERY_STEP_POS_X, MASTERY_STEP_POS_Y)
})

g_pPicture_RanoxFeverGage = g_pUIDialog:CreatePicture()

g_pStatic_RanoxFeverGage:AddPicture(g_pPicture_RanoxFeverGage)
g_pPicture_RanoxFeverGage:SetTex("DLG_UI_Image_01.tga", "MonsterFever_2")
g_pPicture_RanoxFeverGage:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MASTERY_STEP_POS_X + MASTERY_STEP_GAP_X * 1, MASTERY_STEP_POS_Y)
})

g_pPicture_RanoxFeverGage = g_pUIDialog:CreatePicture()

g_pStatic_RanoxFeverGage:AddPicture(g_pPicture_RanoxFeverGage)
g_pPicture_RanoxFeverGage:SetTex("DLG_UI_Image_01.tga", "MonsterFever_3")
g_pPicture_RanoxFeverGage:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MASTERY_STEP_POS_X + MASTERY_STEP_GAP_X * 2, MASTERY_STEP_POS_Y)
})

g_pPicture_RanoxFeverGage = g_pUIDialog:CreatePicture()

g_pStatic_RanoxFeverGage:AddPicture(g_pPicture_RanoxFeverGage)
g_pPicture_RanoxFeverGage:SetTex("DLG_UI_Image_01.tga", "MonsterFever_4")
g_pPicture_RanoxFeverGage:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MASTERY_STEP_POS_X + MASTERY_STEP_GAP_X * 3, MASTERY_STEP_POS_Y)
})

g_pPicture_RanoxFeverGage = g_pUIDialog:CreatePicture()

g_pStatic_RanoxFeverGage:AddPicture(g_pPicture_RanoxFeverGage)
g_pPicture_RanoxFeverGage:SetTex("DLG_UI_Image_01.tga", "MonsterFever_5")
g_pPicture_RanoxFeverGage:SetPoint({
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(MASTERY_STEP_POS_X + MASTERY_STEP_GAP_X * 4, MASTERY_STEP_POS_Y)
})

function CreateFeverNumberBase(slot0, slot1, slot2)
	g_pStatic_Number = g_pUIDialog:CreateStatic()

	g_pUIDialog:AddControl(g_pStatic_Number)
	g_pStatic_Number:SetName(slot0)

	for slot6 = 0, 1 do
		for slot10 = 0, 9 do
			strTexName = string.format("%s%d", "ROSE_EAGLE_RADAR0", tostring(slot10))
			g_pPicture_Number = g_pUIDialog:CreatePicture()

			g_pStatic_Number:AddPicture(g_pPicture_Number)
			g_pPicture_Number:SetTex("DLG_UI_Common_Texture93_NEW.tga", strTexName)
			g_pPicture_Number:SetPoint({
				ADD_SIZE_X = -5,
				CHANGE_TIME = 0,
				ADD_SIZE_Y = -5,
				LEFT_TOP = D3DXVECTOR2(slot1 - 10 * slot6, slot2)
			})
			g_pPicture_Number:SetShow(false)
		end
	end
end

g_pStatic_MagiCount = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_MagiCount)
g_pStatic_MagiCount:SetName("g_pStatic_MagiCount")

g_pPicture_MagiCount = g_pUIDialog:CreatePicture()

g_pStatic_MagiCount:AddPicture(g_pPicture_MagiCount)
g_pPicture_MagiCount:SetTex("DLG_UI_Common_Texture93_NEW.tga", "ROSE_EAGLE_RADAR05")
g_pPicture_MagiCount:SetPoint({
	ADD_SIZE_X = -5,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -5,
	LEFT_TOP = D3DXVECTOR2(ENEMY_NUM_MAX_POS_X, ENEMY_NUM_MAX_POS_Y)
})
CreateFeverNumberBase("g_pStatic_MagiCountNumber", ENEMY_NUM_POS_X, ENEMY_NUM_POS_Y)
