g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetLineMap("Ranox_BurnedForest_StageBoss_LineMap.lua")
g_pWorld:SetBGM("Music044_Boss.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_BurnForest01.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_BurnForest02.ogg")
g_pWorld:SetFogWorld(1800, 6600, -4500, 2700, -0.1, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1))
g_pWorld:SetProjection(10, 50000, 4, 0, 0, 0, 0)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(2)
g_pWorldLayer:SetTex("rain-blend.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(1, 1, 1, 1))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(0)
g_pWorldLayer:SetInitShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_AP_Clo01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_AP_Clo01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.03, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.145098, 0.0352941, 0.0862745, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_AP_Clo02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_AP_Clo02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.04, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.0862745, 0.0431373, 0.160784, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_AP_glass01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_AP_glass01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.764706, 0.690196, 0.898039, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_AP_glass02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_AP_glass02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.533333, 0.576471, 0.721569, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_AP_glass03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_AP_glass03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_AP_glass04.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_AP_glass04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.564706, 0.376471, 0.478431, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_AP_stem.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_AP_stem.Y")
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

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_AP_Tree01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_AP_Tree01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.521569, 0.45098, 0.466667, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_Fire_A01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_Fire_A01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.3, 0.05)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.843137, 0.121569, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_Fire_A02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_Fire_A02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.1, 0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.529412, 0, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_Fire_A03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_Fire_A03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.1, 0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.333333, 0, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_Fire_B01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_Fire_B01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.7)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.905882, 0.168627, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_G01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_G01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.894118, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_G02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_G02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.776471, 0.67451, 0.745098, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_G03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_G03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.705882, 0.454902, 0.670588, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_Sky.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_Sky.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.654902, 0.690196, 0.741176, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_Tree01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_Tree01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_Tree02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_Tree02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.933333, 0.843137, 0.921569, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_Tree03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_Tree03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.541176, 0.611765, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_stageBoss_Tree04.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_stageBoss_Tree04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.376471, 0.164706, 0.219608, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(1800, 6600, -4500, 2700, D3DXCOLOR(0.156863, 0.121569, 0.184314, 1), -0.1)
