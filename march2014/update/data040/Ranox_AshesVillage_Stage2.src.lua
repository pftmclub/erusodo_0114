g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetLineMap("Ranox_AshesVillage_Stage2_LineMap.lua")
g_pWorld:SetBGM("Music066_Stage.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_CaveTrain.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Elder_Cave_Bat.ogg")
g_pWorld:SetFogWorld(-15999, 22200, 0, 0, 0, D3DXCOLOR(0, 0.0156863, 0.129412, 1))
g_pWorld:SetProjection(10, 50000, 4, 0, 0, 0, 0)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("besma_secret_red.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0.764706, 1, 1, 0.490196))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(0)
g_pWorldLayer:SetInitShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("ashs_Village_s2_AP_mark.Y")
g_pObjectMesh:SetXMeshLOD("ashs_Village_s2_AP_mark.Y")
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
g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("ASHS_VILLAGE_S2_CAVE.X")
g_pObjectSkinMesh:AddXSkinModel("ASHS_VILLAGE_S2_CAVE.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)
g_pObjectSkinMesh:SetTransition(false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(false)

g_pRenderParam.bAlphaBlend = false

g_pObjectSkinMesh:SetFog(-15999, 22200, 0, 0, D3DXCOLOR(0, 0.0156863, 0.133333, 1), -0.46)
g_pObjectSkinMesh:SetForceShow(1)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("ASHS_VILLAGE_S2_CART.X")
g_pObjectSkinMesh:AddXSkinModel("ASHS_VILLAGE_S2_CART.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(false)

g_pRenderParam.bAlphaBlend = false

g_pObjectSkinMesh:SetForceShow(1)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("ASHS_VILLAGE_S2_CAIN.X")
g_pObjectSkinMesh:AddXSkinModel("ASHS_VILLAGE_S2_CAIN.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(true, true)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(false)

g_pRenderParam.bAlphaBlend = false
g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Smoke_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-5400, 1550, -20300))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Stone_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-5400, 1550, -20300))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Stone_P02")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-5400, 1550, -20300))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Smoke_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-4910, 1550, -20240))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Stone_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-4910, 1550, -20240))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Stone_P02")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-4910, 1550, -20240))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Smoke_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-3450, 1500, -20060))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Stone_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-3450, 1500, -20060))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Stone_P02")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-3450, 1500, -20060))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Smoke_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-2950, 1510, -19980))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Stone_P01")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-2950, 1510, -19980))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)

g_pObjectParticle = g_pWorld:CreateObjectParticle(g_pUIMajorParticle, "Ranox_map_Stone_P02")

g_pObjectParticle:SetParticlePos(D3DXVECTOR3(-2950, 1510, -19980))
g_pObjectParticle:SetTriggerCount(-1)
g_pObjectParticle:SetTriggerTime(-1)
g_pObjectParticle:SetDrawCount(1)
g_pObjectParticle:SetCullCheck(false)
g_pObjectParticle:SetParticleType(PARTICLE_TYPE.PT_3D_PLANE)
g_pObjectParticle:SetLayer(X2_LAYER.XL_EFFECT_0)
