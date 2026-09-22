g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetOriginColor_LUA(1, 0.894118, 0.803922)
g_pWorld:SetLineMap("Field_Ranox_Stage1_LineMap.lua")
g_pWorld:SetBGM("Field_Ranox002.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Velder_Bridge_Burning_Wind.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_Cave01.ogg")
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 4584, 1526, 2933, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 4771, 2796, 2987, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 4771, 2796, 2987, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 6366, 3561, 2611, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 6366, 3561, 2611, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Stream01.ogg", 7956, 2177, 1974, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Stream01.ogg", 8719, 660, 556, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Stream01.ogg", 10443, 537, -640, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 11303, 161, -1016, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 11213, 614, 2037, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall01.ogg", 11190, 2206, 2238, 4000)
g_pWorld:SetFogWorld(2800, 140000, 5800, -4599, 0, D3DXCOLOR(1, 0.639216, 0.321569, 1))
g_pWorld:SetProjection(10, 500000, 4, 300, 0, 0, 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Bubble_A.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Bubble_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.05)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Bubble_B.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Bubble_B.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.05)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Bubble_C.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Bubble_C.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.05)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Bubble_D.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Bubble_D.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.5)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.992157, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_DistantView_A.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_DistantView_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_DistantView_B.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_DistantView_B.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A1.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A1.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.784314, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A2.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A2.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A3.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A3.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A4.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A4.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A5.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A5.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A_Opacity.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A_Opacity.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A_Ornament_A.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A_Ornament_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A_Ornament_B.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A_Ornament_B.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A_Ornament_C.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A_Ornament_C.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_A_Ornament_D.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_A_Ornament_D.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B1.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B1.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.784314, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B2.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B2.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B3.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B3.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B4.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B4.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B5.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B5.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B_Flow_A.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B_Flow_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B_Ornament_A.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B_Ornament_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B_Ornament_B.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B_Ornament_B.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B_Ornament_C.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B_Ornament_C.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Foothold_B_Ornament_D.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Foothold_B_Ornament_D.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Hotspring_A.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Hotspring_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.05, 0.05)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Hotspring_B.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Hotspring_B.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.02, -0.02)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Hotspring_C.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Hotspring_C.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Hotspring_D.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Hotspring_D.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_mountain_A.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_mountain_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_mountain_B.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_mountain_B.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_mountain_C.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_mountain_C.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Sky.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Sky.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Type1_A.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Type1_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Type1_B.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Type1_B.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Water_A1.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Water_A1.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Water_A2.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Water_A2.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-1, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Water_A3.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Water_A3.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.5, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Water_A4.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Water_A4.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.5)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Water_A5.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Water_A5.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Way_A.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Way_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Way_C1.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Way_C1.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Field_Ranox_Stage1_Way_C2.Y")
g_pObjectMesh:SetXMeshLOD("Field_Ranox_Stage1_Way_C2.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.996078, 0.886275, 0.835294, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2800, 140000, 5800, -4599, D3DXCOLOR(1, 0.643137, 0.321569, 1), 0)
