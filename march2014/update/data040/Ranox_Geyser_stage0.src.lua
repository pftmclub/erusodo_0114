g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetLineMap("Ranox_Geyser_stage0_linemap.lua")
g_pWorld:SetBGM("Music067_Stage.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_Cave01.ogg")
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall01.ogg", 3800, 1780, -8624, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 6484, 721, -7055, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Portal01.ogg", 10945, 878, -6255, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 10661, 2058, -887, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 10690, 447, -1019, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 8502, 1036, 32, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Portal01.ogg", 7350, 741, -417, 3000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall01.ogg", 18960, 1737, 7956, 9000)
g_pWorld:Add3DEffectBGM_LUA("Amb_Ranox_Cave_Waterfall02.ogg", 12114, 1501, 7659, 8000)
g_pWorld:SetFogWorld(2200, 10800, 5870, -2860, -0.07, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1))
g_pWorld:SetProjection(10, 500000, 4, 0, 0, 0, 0)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("WorldLayer_Velder_Gate.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0.3, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0.921569, 0.52549, 0.341176, 1))
g_pWorldLayer:SetTexUV(0.15, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(0)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(1)
g_pWorldLayer:SetTex("Tree_Fog.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0.921569, 0.419608, 0.345098, 0.278431))
g_pWorldLayer:SetTexUV(0.15, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(1)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("WorldLayer_Velder_Gate.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0.2, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(1, 0.552941, 0.721569, 1))
g_pWorldLayer:SetTexUV(-0.1, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(2)
g_pWorldLayer:SetInitShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_Alpha_light01.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_Alpha_light01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.847059, 0.576471, 0.631373, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01_back.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01_back.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01_back_Alpha.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01_back_Alpha.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01_hide01.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01_hide01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.976471, 0.796078, 0.839216, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(true, true)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01_hide02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01_hide02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(true, true)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01_wall01.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01_wall01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.584314, 0.635294, 0.623529, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01_wall02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01_wall02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.568627, 0.478431, 0.521569, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01_wall_back01.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01_wall_back01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.764706, 0.368627, 0.631373, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01_wall_back02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01_wall_back02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.796078, 0.643137, 0.886275, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G01_wall_back03.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G01_wall_back03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.87451, 0.705882, 0.815686, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G03.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G04.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_G05_Alpha.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_G05_Alpha.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.909804, 0.819608, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_sky.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_sky.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water01.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.15, -0.07)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.341176, 0.14902, 0.0941176, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water02.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.15, 0.07)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.313726, 0.192157, 0.129412, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water03.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.05, -0.002)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.921569, 0.776471, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water04.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water05.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water05.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
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

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water06.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water06.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.3)
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

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water07.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water07.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.3)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.623529, 0.529412, 0.666667, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water08.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water08.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.3)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.623529, 0.529412, 0.666667, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water09.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water09.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.5)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Geyser_0stage_water10.Y")
g_pObjectMesh:SetXMeshLOD("Geyser_0stage_water10.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.3)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.srcBlend = D3DBLEND.D3DBLEND_SRCALPHA
g_pRenderParam.destBlend = D3DBLEND.D3DBLEND_DESTALPHA

g_pObjectMesh:SetFog(2200, 10800, 5870, -2860, D3DXCOLOR(0.839216, 0.466667, 0.376471, 1), -0.07)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("Motion_GEYSER_STEELPUMP.x")
g_pObjectSkinMesh:AddXSkinModel("Motion_GEYSER_STEELPUMP.x")
g_pObjectSkinMesh:AddAnim("WorkLoop")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(10940, 650, -6303))
g_pObjectSkinMesh:SetAngleDegree(D3DXVECTOR3(0, 30, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)
g_pObjectSkinMesh:SetCartoonRender(true)
g_pObjectSkinMesh:SetAnimSpeed(1.5)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("Motion_GEYSER_PUMP.x")
g_pObjectSkinMesh:AddXSkinModel("Motion_GEYSER_PUMP.x")
g_pObjectSkinMesh:AddAnim("WorkLoop")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(10940, 650, -6303))
g_pObjectSkinMesh:SetAngleDegree(D3DXVECTOR3(0, 30, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)
-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): SetTriggerID(0) omitted -- not a
-- Lua-enum gap like the ones above, a method-binding gap the plan's Findings
-- table never catalogued (it only audited enum members, not scene-file
-- method calls). SetTriggerID doesn't exist anywhere in this repo's C++ at
-- all (checked X2Lib and KTDXLIB exhaustively, not just
-- CX2WorldObjectSkinMesh's own header/class_def block) -- not a binding gap,
-- the method itself was never implemented in this build. A call to it aborts
-- the whole script's load, so this can't be left in. This mesh (the
-- steel-pump prop tied to Ranox_Geyser_stage0_linemap.lua's TRIGGER_ID 0,
-- itself already gap-omitted above) now just plays its WorkLoop animation
-- continuously from spawn instead of being trigger-armed. Flagged for the
-- repo/engine integration phase group.
g_pObjectSkinMesh:SetCartoonRender(true)
g_pObjectSkinMesh:SetAnimSpeed(1.5)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("Motion_GEYSER_PUMP.x")
g_pObjectSkinMesh:AddXSkinModel("Motion_GEYSER_PUMP.x")
g_pObjectSkinMesh:AddAnim("WorkLoop")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(7496, 510, -500))
g_pObjectSkinMesh:SetAngleDegree(D3DXVECTOR3(0, 30, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)
-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): SetTriggerID(2) omitted -- same
-- unimplemented-method gap as TRIGGER_ID 0's sibling above. SetShowObject(false,
-- true) is also omitted, but its gap shape is different and worth being
-- precise about: the underlying C++ method (CKTDGObject::SetShowObject(bool,
-- bool), KTDXLIB/KTDGObject.h:305) does exist, and CX2WorldObjectSkinMesh's
-- inheritance chain does reach it in lua_tinker (class_inh<CX2WorldObjectSkinMesh,
-- CX2WorldObject> and class_inh<CX2WorldObject, CKTDGObject>, both registered
-- in X2Lib/X2Main.cpp) -- but CKTDGObject's own 5-entry class_def block
-- (KTDXLIB/KTDXApp.cpp:1651-1655) doesn't bind it, so the Lua name
-- "SetShowObject" resolves to nothing through this object's chain. The only
-- place that Lua name string is ever bound at all is nowhere -- a
-- similar-looking pair of bindings on CX2GUNPC/CX2GUUser
-- (X2Lib/X2Main.cpp:4231, :4574) map the *C++* method
-- CX2GUNPC::SetShowObject/CX2GUUser::SetShowObject to the *different* Lua
-- name "SetShow", and neither class is an ancestor of CX2WorldObjectSkinMesh
-- anyway. Net effect same as SetTriggerID's: the call would abort the
-- script's load, so it's omitted. This mesh (tied to TRIGGER_ID 2, also
-- already gap-omitted above) starts visible immediately instead of
-- hidden-until-triggered, and keeps playing continuously instead of being
-- trigger-armed.
g_pObjectSkinMesh:SetCartoonRender(true)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("Motion_GEYSER_PUMP.x")
g_pObjectSkinMesh:AddXSkinModel("Motion_GEYSER_PUMP.x")
g_pObjectSkinMesh:AddAnim("WorkLoop")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(11450, 510, -500))
g_pObjectSkinMesh:SetAngleDegree(D3DXVECTOR3(0, 30, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)
-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): SetShowObject(false, true) and
-- SetTriggerID(3) omitted -- same gap class documented on this file's
-- TRIGGER_ID 0/2 siblings above (SetTriggerID: unimplemented anywhere in this
-- repo's C++; SetShowObject: implemented on CKTDGObject but never given a
-- lua_tinker binding for that name, on any class). Note this argument, "3",
-- doesn't correspond to any TRIGGER_ID actually defined in
-- Ranox_Geyser_stage0_linemap.lua (that file only has 0/1/2) -- an orphaned
-- reference already present in June2017's own source, not introduced by this
-- port; transcribed verbatim per this plan's "transcribe, don't tidy" rule
-- rather than "corrected."
g_pObjectSkinMesh:SetCartoonRender(true)
g_pObjectSkinMesh:SetAnimSpeed(1.5)
