g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetOriginColor_LUA(0.913725, 0.858824, 0.913725)
g_pWorld:SetLineMap("Ranox_FireTemple_Stage1_LineMap.lua")
g_pWorld:SetBGM("Music070_Stage.ogg")
g_pWorld:AddEffectBGM_LUA("AMB_Velder_Secret_Stage4.ogg")
g_pWorld:AddEffectBGM_LUA("AMB_Velder_Secret_Stage3.ogg")
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar03.ogg", -2382, 3134, -1799, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar02.ogg", 155, 2807, -1383, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar01.ogg", 1865, 2844, -861, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar03.ogg", 3769, 2945, -1081, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar02.ogg", 4091, 2535, 2071, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar01.ogg", 3898, 2503, 3729, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar03.ogg", 4041, 2535, 4594, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar02.ogg", 4030, 2574, 6209, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar01.ogg", 3970, 2897, 8173, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar03.ogg", 6108, 2498, 8469, 4000)
g_pWorld:SetFogWorld(14400, 37000, 6933, 3333, 0, D3DXCOLOR(0.176471, 0.247059, 0, 1))
g_pWorld:SetProjection(10, 50000, 4, 0, 0, 0, 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FireTemple_Stage1_AP_crack.Y")
g_pObjectMesh:SetXMeshLOD("FireTemple_Stage1_AP_crack.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(14400, 37000, 6933, 3333, D3DXCOLOR(0.176471, 0.247059, 0, 1), 0.43)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FireTemple_Stage1_AP_dark.Y")
g_pObjectMesh:SetXMeshLOD("FireTemple_Stage1_AP_dark.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.466667, 1, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FireTemple_Stage1_AP_fire.Y")
g_pObjectMesh:SetXMeshLOD("FireTemple_Stage1_AP_fire.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 1.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.74902, 0.203922, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FireTemple_Stage1_G01.Y")
g_pObjectMesh:SetXMeshLOD("FireTemple_Stage1_G01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(14400, 37000, 6933, 3333, D3DXCOLOR(0.176471, 0.247059, 0, 1), 0.43)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FireTemple_Stage1_magma.Y")
g_pObjectMesh:SetXMeshLOD("FireTemple_Stage1_magma.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FireTemple_Stage1_X_ground.X")
g_pObjectSkinMesh:AddXSkinModel("FireTemple_Stage1_X_ground.X")
g_pObjectSkinMesh:AddAnim("wait01")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(true, true)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)
g_pObjectSkinMesh:SetBoundingRadius(300)
g_pObjectSkinMesh:SetCenter(D3DXVECTOR3(600, 2500, 0))
g_pObjectSkinMesh:SetIsForceNotCull(true)
g_pObjectSkinMesh:SetForceShow(true)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FireTemple_Stage1_X_ground02.X")
g_pObjectSkinMesh:AddXSkinModel("FireTemple_Stage1_X_ground02.X")
g_pObjectSkinMesh:AddAnim("wait02")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(true, true)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = false

g_pObjectSkinMesh:SetBoundingRadius(300)
g_pObjectSkinMesh:SetCenter(D3DXVECTOR3(6517, 2500, 9400))
g_pObjectSkinMesh:SetIsForceNotCull(true)
g_pObjectSkinMesh:SetForceShow(true)
