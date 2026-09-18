g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetLineMap("Ranox_BurnedForest_Stage0_LineMap.lua")
g_pWorld:SetBGM("Music064_Stage.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_BurnForest01.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_BurnForest02.ogg")
g_pWorld:Add3DEffectBGM_LUA("Amb_FireBurn.ogg", -1402, 516, 435, 2000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Fire_Wood1.ogg", 411, 286, -691, 2000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Fire_Wood2.ogg", 1223, 817, 742, 1000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Fire_Wood3.ogg", 2590, 1079, 957, 1500)
g_pWorld:Add3DEffectBGM_LUA("Amb_Fire_Wood4.ogg", 10227, 1020, 969, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Fire_Wood5.ogg", 13143, 431, 928, 3000)
g_pWorld:SetFogWorld(-3199, 27800, -3100, 7800, -0.2, D3DXCOLOR(0.686275, 0.686275, 0.729412, 1))
g_pWorld:SetProjection(10, 500000, 4, 0, 0, 0, 0)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("rain-blend.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(1, 1, 1, 1))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(0)
g_pWorldLayer:SetInitShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Alpha_Fire01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Alpha_Fire01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.1, 0.8)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.0901961, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.847059, 0.890196, 0.996078, 0), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Alpha_Fire02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Alpha_Fire02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.1, 0.5)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.278431, 0, 0.121569, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.847059, 0.890196, 0.996078, 0), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Alpha_Fire03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Alpha_Fire03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.6)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.847059, 0.890196, 0.996078, 0), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Alpha_Fire04.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Alpha_Fire04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.08, 0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.815686, 0.0431373, 0.619608, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.847059, 0.890196, 0.996078, 0), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Alpha_stem01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Alpha_stem01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Alpha_stem02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Alpha_stem02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.847059, 0.890196, 0.996078, 0), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Alpha_stem03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Alpha_stem03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.611765, 0.635294, 0.713726, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_AP_Cloud01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_AP_Cloud01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.01, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.243137, 0.243137, 0.294118, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_AP_Cloud02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_AP_Cloud02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.02, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.223529, 0.203922, 0.254902, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_AP_Cloud03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_AP_Cloud03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.02, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.25098, 0.25098, 0.305882, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Canhide.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Canhide.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(true, true)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_G01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_G01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.811765, 0.913725, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_G02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_G02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.45098, 0.54902, 0.745098, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_G03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_G03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.219608, 0.32549, 0.498039, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Obj01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Obj01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Plants_A.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Plants_A.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.745098, 0.721569, 0.768627, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Plants_B.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Plants_B.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Sky.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Sky.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.278431, 0.403922, 0.47451, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Tree01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Tree01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Tree02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Tree02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.545098, 0.611765, 0.709804, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stage0_Tree03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stage0_Tree03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.356863, 0.403922, 0.564706, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-3199, 27800, -3100, 7800, D3DXCOLOR(0.686275, 0.690196, 0.733333, 1), -0.2)
