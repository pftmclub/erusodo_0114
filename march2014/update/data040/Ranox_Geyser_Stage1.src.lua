g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetOriginColor_LUA(1, 0.898039, 0.894118)
g_pWorld:SetLineMap("Ranox_Geyser_Stage1_LineMap.lua")
g_pWorld:SetBGM("Music067_Stage.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_Cave01.ogg")
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall01.ogg", -4248, 1050, 3487, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Portal01.ogg", 2072, 1565, 1849, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Stream01.ogg", 11039, 823, 7210, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall01.ogg", 13902, 1145, 8078, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Stream01.ogg", 15745, 1006, 7220, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Stream01.ogg", 18191, 890, 8208, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Stream01.ogg", 19624, 732, 11581, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Stream01.ogg", 19798, 498, 14258, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Stream01.ogg", 21268, 358, 14835, 4000)
g_pWorld:SetFogWorld(5400, 6600, 6533, -666, 0, D3DXCOLOR(0.831373, 0.537255, 0.47451, 1))
g_pWorld:SetProjection(10, 5000000, 4, 0, 0, 0, 0)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(1)
g_pWorldLayer:SetTex("WorldLayer_Velder_Gate2.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 2.3)
g_pWorldLayer:SetColor(D3DXCOLOR(1, 0, 0, 1))
g_pWorldLayer:SetTexUV(0.2, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(0)
g_pWorldLayer:SetInitShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_AP_bubble.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_AP_bubble.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectMesh:SetFog(5400, 6600, 6533, -666, D3DXCOLOR(0.831373, 0.541176, 0.47451, 1), -0.06)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_AP_MT.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_AP_MT.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.85098, 0.811765, 0.941176, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(5400, 6600, 6533, -666, D3DXCOLOR(0.831373, 0.541176, 0.47451, 1), -0.06)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_AP_MT02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_AP_MT02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.905882, 0.870588, 0.894118, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_AP_smoke.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_AP_smoke.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.02, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.870588, 0.729412, 0.584314, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_AP_water02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_AP_water02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.4, -0.3)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.133333, 0.580392, 0.25098, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetForceShow(true)

g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G01.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(5400, 6600, 6533, -666, D3DXCOLOR(0.831373, 0.541176, 0.47451, 1), -0.06)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G01_AP_water.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G01_AP_water.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.1, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.509804, 0.839216, 0.721569, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetForceShow(true)

g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G01_AP_water02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G01_AP_water02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.2, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.254902, 0.380392, 0.435294, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G01_AP_water03.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G01_AP_water03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.03, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.286275, 0.490196, 0.25098, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G01_AP_water04.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G01_AP_water04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.02, 0.1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.580392, 0.619608, 0.47451, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(5400, 6600, 6533, -666, D3DXCOLOR(0.831373, 0.541176, 0.47451, 1), -0.06)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_AP_bubble.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_AP_bubble.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.4)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.929412, 0.909804, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_AP_bubble02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_AP_bubble02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.4)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.929412, 0.909804, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_AP_bubble03.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_AP_bubble03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.929412, 0.909804, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_AP_bubble04.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_AP_bubble04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.929412, 0.909804, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_water.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_water.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.3, 0.3)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.435294, 0.745098, 0.568627, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetForceShow(true)

g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_water02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_water02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.956863, 0.929412, 0.909804, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_waterfull.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_waterfull.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -3.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.960784, 0.929412, 0.909804, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectMesh:SetFog(5400, 6600, 6533, -666, D3DXCOLOR(0.831373, 0.541176, 0.47451, 1), -0.06)
g_pObjectMesh:SetForceShow(true)

g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_waterfull02-1.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_waterfull02-1.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -3.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.933333, 0.968627, 0.913725, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_waterfull02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_waterfull02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(2.2, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.568627, 0.756863, 0.596078, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_waterfull03.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_waterfull03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.458824, 0.431373, 0.435294, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_waterfull04.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_waterfull04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.376471, 0.494118, 0.513726, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_waterfull05.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_waterfull05.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.933333, 0.968627, 0.913725, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_G02_waterfull06.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_G02_waterfull06.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0, 2.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_water.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_water.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.1, 0.1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.176471, 0.298039, 0.333333, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetForceShow(true)

g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_water02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_water02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.02, -0.1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.619608, 0.619608, 0.45098, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_water03.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_water03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.04, 0.1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.294118, 0.211765, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_waterfull.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_waterfull.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.04)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_1stage_waterfull02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_1stage_waterfull02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.3)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.568627, 0.85098, 0.8, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("Motion_GEYSER_PUMP.x")
g_pObjectSkinMesh:AddXSkinModel("Motion_GEYSER_PUMP.x")
g_pObjectSkinMesh:AddAnim("WorkLoop")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(2050, 1349, 1800))
g_pObjectSkinMesh:SetAngleDegree(D3DXVECTOR3(0, 30, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)
-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): SetTriggerID(0) omitted -- not a
-- Lua-enum gap, a method-binding gap the plan's Findings table never
-- catalogued (it only audited enum members, not scene-file method calls).
-- SetTriggerID doesn't exist anywhere in this repo's C++ at all (checked
-- X2Lib and KTDXLIB exhaustively) -- not a binding gap, never implemented in
-- this build. SetShowObject(false, true) is also omitted, but its gap shape
-- differs: the underlying C++ method (CKTDGObject::SetShowObject(bool,bool),
-- KTDXLIB/KTDGObject.h:305) does exist and CX2WorldObjectSkinMesh's
-- inheritance chain does reach it in lua_tinker (class_inh<CX2WorldObjectSkinMesh,
-- CX2WorldObject> and class_inh<CX2WorldObject, CKTDGObject>, both registered
-- in X2Lib/X2Main.cpp) -- but CKTDGObject's own 5-entry class_def block
-- (KTDXLIB/KTDXApp.cpp:1651-1655) doesn't bind it, and the only other place
-- the Lua name "SetShowObject" is ever bound is nowhere: a similar-looking
-- pair of bindings on CX2GUNPC/CX2GUUser (X2Lib/X2Main.cpp:4231, :4574) maps
-- those classes' own SetShowObject C++ methods to the *different* Lua name
-- "SetShow", and neither class is an ancestor of CX2WorldObjectSkinMesh
-- anyway. Same gap class already documented on
-- march2014/update/data040/Ranox_Geyser_stage0.lua (this line's own Stage0).
-- A call to either aborts the whole script's load, so this can't be left in.
-- This mesh now starts visible immediately instead of hidden-until-triggered,
-- and plays its WorkLoop animation continuously from spawn instead of being
-- trigger-armed. Flagged for the repo/engine integration phase group.
g_pObjectSkinMesh:SetCartoonRender(true)
g_pObjectSkinMesh:SetAnimSpeed(1.5)
