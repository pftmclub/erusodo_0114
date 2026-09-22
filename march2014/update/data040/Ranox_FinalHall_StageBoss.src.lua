g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetOriginColor_LUA(0.666667, 0.705882, 0.94902)
g_pWorld:SetLineMap("Ranox_FinalHall_StageBoss_LineMap.lua")
g_pWorld:SetBGM("Music069_Boss.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_DarkGate_Drone01.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_DarkGate_Drone02.ogg")
g_pWorld:AddEffectBGM_LUA("AMB_Earthquake01.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_Firestorm.ogg")
g_pWorld:SetFogWorld(18200, 20200, 10000, -9599, 0, D3DXCOLOR(0.886275, 0.211765, 0.239216, 1))
g_pWorld:SetProjection(10, 999999, 4, 0, 0, 0, 0)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(1)
g_pWorldLayer:SetTex("WorldLayer_Velder_Gate.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 2)
g_pWorldLayer:SetColor(D3DXCOLOR(1, 0, 0, 1))
g_pWorldLayer:SetTexUV(0.2, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(0)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("rain-blend.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0, 0, 0, 0.607843))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(1)
g_pWorldLayer:SetInitShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_blue.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_blue.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.2)
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

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_blue02.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_blue02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.2)
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

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_blue03.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_blue03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.4)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.403922, 0.180392, 0.803922, 1))
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

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_blue04.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_blue04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.4)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0, 0.054902, 0.745098, 1))
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

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_blue05.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_blue05.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.0156863, 0.121569, 0.568627, 1))
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

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_blue06.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_blue06.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.4)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.545098, 0.121569, 0.65098, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_crack.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_crack.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_ground.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_ground.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_light.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_light.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.388235, 0.188235, 0.113725, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_magma.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_magma.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.3)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_red_fire.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_red_fire.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 2.3)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_spot.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_spot.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 1.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_spot02.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_spot02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 2.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_spot03.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_spot03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-1.5, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0, 0, 0, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_wind.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_wind.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.1, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.576471, 0, 0.388235, 1))
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

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_AP_wind02.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_AP_wind02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.2, 0)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0, 0.156863, 0.635294, 1))
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

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_bosstone.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_bosstone.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetUseWorldColor(false)
g_pObjectMesh:SetFog(18200, 20200, 10000, -9599, D3DXCOLOR(0.886275, 0.211765, 0.239216, 1), -0.28)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_G01.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_G01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(18200, 20200, 10000, -9599, D3DXCOLOR(0.886275, 0.211765, 0.239216, 1), -0.28)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_G02.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_G02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(18200, 20200, 10000, -9599, D3DXCOLOR(0.886275, 0.211765, 0.239216, 1), -0.28)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_lightball.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_lightball.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_StageBoss_magma.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_StageBoss_magma.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.04, -0.02)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetUseWorldColor(false)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FINALHALL_STAGEBOSS_X_MAGMA.X")
g_pObjectSkinMesh:AddXSkinModel("FINALHALL_STAGEBOSS_X_MAGMA.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetTex0UVSpeed(-0.04, 0.02)
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectSkinMesh:SetUseWorldColor(false)
g_pObjectSkinMesh:SetFog(18200, 20200, 10000, -9599, D3DXCOLOR(0.886275, 0.211765, 0.239216, 1), -0.28)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FINALHALL_STAGEBOSS_X_BLACKSTONE.X")
g_pObjectSkinMesh:AddXSkinModel("FINALHALL_STAGEBOSS_X_BLACKSTONE.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectSkinMesh:SetUseWorldColor(false)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FINALHALL_STAGEBOSS_X_BLACKSTONE02.X")
g_pObjectSkinMesh:AddXSkinModel("FINALHALL_STAGEBOSS_X_BLACKSTONE02.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectSkinMesh:SetUseWorldColor(false)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FINALHALL_STAGEBOSS_X_STONE.X")
g_pObjectSkinMesh:AddXSkinModel("FINALHALL_STAGEBOSS_X_STONE.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectSkinMesh:SetUseWorldColor(false)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FINALHALL_STAGEBOSS_X_STONE02.X")
g_pObjectSkinMesh:AddXSkinModel("FINALHALL_STAGEBOSS_X_STONE02.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FINALHALL_STAGEBOSS_X_MAGIC.X")
g_pObjectSkinMesh:AddXSkinModel("FINALHALL_STAGEBOSS_X_MAGIC.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_DOME_2)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(0.376471, 0.470588, 0.478431, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FINALHALL_STAGEBOSS_X_MAGIC02.X")
g_pObjectSkinMesh:AddXSkinModel("FINALHALL_STAGEBOSS_X_MAGIC02.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_DOME_2)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(0, 0.498039, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FINALHALL_STAGEBOSS_X_BLUE.X")
g_pObjectSkinMesh:AddXSkinModel("FINALHALL_STAGEBOSS_X_BLUE.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_DOME_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA
g_pRenderParam.bZEnable = true
g_pRenderParam.bZWriteEnable = false
g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_6MapBack_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-20464, 26938, 138704))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_6MapBack_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-13848, 26938, 131456))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_6MapBack_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-364, 26938, 127104))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_6MapBack_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(12227, 26500, 131985))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_6MapBack_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(17175, 26500, 140137))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)
