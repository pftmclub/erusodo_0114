g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetOriginColor_LUA(0.819608, 0.831373, 0.913725)
g_pWorld:SetLineMap("Ranox_BurnedForest_Stage2_LineMap.lua")
g_pWorld:SetBGM("Music064_Stage.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_BurnForest01.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_BurnForest02.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Besma_Secret_Water.ogg")
g_pWorld:Add3DEffectBGM_LUA("Amb_Fire_Wood1.ogg", -202, 2400, 0, 1500)
g_pWorld:SetFogWorld(-10799, 18200, 533, 10000, 0, D3DXCOLOR(0.337255, 0.219608, 0.329412, 1))
g_pWorld:SetProjection(10, 500000, 4, 0, 0, 0, 0)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("rain-blend.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0.380392, 0.443137, 0.380392, 0.764706))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(0)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(1)
g_pWorldLayer:SetTex("rain-light.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0.203922, 0.286275, 0.713726, 0.188235))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(1)
g_pWorldLayer:SetInitShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_S2_AP_Fire_light01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_S2_AP_Fire_light01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.5, 0.5)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_S2_AP_Fire_light02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_S2_AP_Fire_light02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.5, 1.5)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_S2_AP_Fire_light03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_S2_AP_Fire_light03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.1, 1.5)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_S2_AP_Fire_light04.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_S2_AP_Fire_light04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.5)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_S2_AP_Fire_light05.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_S2_AP_Fire_light05.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.5, 0.8)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.215686, 0.145098, 0.12549, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_S2_AP_Fire_light06.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_S2_AP_Fire_light06.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.5)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.764706, 0.568627, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_S2_AP_Fire_light07.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_S2_AP_Fire_light07.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.5, 0.9)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.2, 0.14902, 0.12549, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_S2_AP_Fire_light08.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_S2_AP_Fire_light08.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.2, 0.7)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.0509804, 0.0901961, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_AP_smoke.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_AP_smoke.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.01, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.145098, 0.168627, 0.254902, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_AP_smoke02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_AP_smoke02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.01, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.235294, 0.180392, 0.278431, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_AP_tree.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_AP_tree.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.4, 0.509804, 0.615686, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-10799, 18200, 533, 10000, D3DXCOLOR(0.341176, 0.223529, 0.329412, 1), -0.18)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_bine.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_bine.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.745098, 0.713726, 0.85098, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-10799, 18200, 533, 10000, D3DXCOLOR(0.341176, 0.223529, 0.329412, 1), -0.18)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_flower.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_flower.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.584314, 0.466667, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G01.Y")
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

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G01_grass.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G01_grass.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.933333, 0.941176, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G01_grass02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G01_grass02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.760784, 0.694118, 0.458824, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G01_ground.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G01_ground.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.690196, 0.776471, 0.87451, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G01_tree.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G01_tree.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.831373, 0.729412, 0.745098, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G01_tree02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G01_tree02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.803922, 0.796078, 0.909804, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.792157, 0.67451, 0.741176, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-10799, 18200, 533, 10000, D3DXCOLOR(0.341176, 0.223529, 0.329412, 1), -0.18)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G02_grass.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G02_grass.Y")
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

g_pObjectMesh:SetFog(-10799, 18200, 533, 10000, D3DXCOLOR(0.341176, 0.223529, 0.329412, 1), -0.18)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G02_ground.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G02_ground.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.643137, 0.690196, 0.745098, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-10799, 18200, 533, 10000, D3DXCOLOR(0.341176, 0.223529, 0.329412, 1), -0.18)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G02_ground02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G02_ground02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_G03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_G03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.27451, 0.329412, 0.505882, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-10799, 18200, 533, 10000, D3DXCOLOR(0.341176, 0.223529, 0.329412, 1), -0.18)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_grass01.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_grass01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.776471, 0.8, 0.921569, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-10799, 18200, 533, 10000, D3DXCOLOR(0.341176, 0.223529, 0.329412, 1), -0.18)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_grass02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_grass02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.541176, 0.537255, 0.666667, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-10799, 18200, 533, 10000, D3DXCOLOR(0.341176, 0.223529, 0.329412, 1), -0.18)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_grass03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_grass03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.529412, 0.54902, 0.584314, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_grass04.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_grass04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.341176, 0.380392, 0.47451, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_grass05.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_grass05.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.6, 0.658824, 0.623529, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_sky.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_sky.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.266667, 0.254902, 0.4, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(-10799, 18200, 533, 10000, D3DXCOLOR(0.341176, 0.223529, 0.329412, 1), -0.18)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_water02.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_water02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.08, -0.008)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.105882, 0.133333, 0.215686, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Fire_forest_s2_water03.Y")
g_pObjectMesh:SetXMeshLOD("Fire_forest_s2_water03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.47451, 0.419608, 0.380392, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetForceShow(true)
