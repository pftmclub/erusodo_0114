g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetLineMap("Ranox_GateWay_Stage1_LineMap.lua")
g_pWorld:SetBGM("Music068_Stage.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_SandStorm01.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_Firestorm.ogg")
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar03.ogg", -49254, 868, 0, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar01.ogg", -49988, 810, -182, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar02.ogg", -51606, 737, -918, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar03.ogg", -52604, 782, -948, 5000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar01.ogg", -55757, 7835, -2270, 5000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar03.ogg", -55431, 1670, -944, 5000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar02.ogg", -56880, 2253, -2331, 4000)
g_pWorld:Add3DEffectBGM_LUA("Amb_DarkGate_Lavar02.ogg", -58337, 1781, 803, 5000)
g_pWorld:SetFogWorld(11000, 25600, 0, 3000, 0, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1))
g_pWorld:SetProjection(10, 50000, 4, 0, 0, 0, 0)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(1)
g_pWorldLayer:SetTex("WorldLayer_Velder_Gate.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0.5, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(1, 0.501961, 1, 1))
g_pWorldLayer:SetTexUV(-0.2, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(0)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(2)
g_pWorldLayer:SetTex("Tree_Fog.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0.25, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(1, 1, 1, 0.356863))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(1)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("test_UI02.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(1, 1, 1, 1))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(2)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("Kindling.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(1, 1, 1, 1))
g_pWorldLayer:SetTexUV(-1, -1)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(3)
g_pWorldLayer:SetInitShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_A_Mt01.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_A_Mt01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectMesh:SetFog(11000, 25600, 0, 3000, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.31)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_A_Mt02.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_A_Mt02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.831373, 0.833333, 0.864706, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(11000, 25600, 0, 3000, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.31)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Firegrass01.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Firegrass01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Front_G01.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Front_G01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_G01.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_G01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 0.996078, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(11000, 25600, 0, 3000, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.31)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_G02.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_G02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(11000, 25600, 0, 3000, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.31)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Lava.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Lava.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Lava02.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Lava02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Mt01.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Mt01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(8000, 25600, 0, 3000, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.31)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Mt02.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Mt02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(16600, 34600, 600, 3700, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.4)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Mt03.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Mt03.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(14000, 35000, 0, 3000, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.31)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Mt04.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Mt04.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(8400, 25600, 0, 3000, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.31)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Skybox.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Skybox.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(14000, 35000, 0, 3800, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.1)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_VL01.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_VL01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_2)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("GateWay_1stage_Mt05.Y")
g_pObjectMesh:SetXMeshLOD("GateWay_1stage_Mt05.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(20000, 34600, 600, 3700, D3DXCOLOR(0.941176, 0.509804, 0.431373, 1), 0.4)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("GATEWAY_1STAGE_ROCK_ANI01.X")
g_pObjectSkinMesh:AddXSkinModel("GATEWAY_1STAGE_ROCK_ANI01.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_ONE_WAIT)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(-55800, -50000, -1387))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(false)

g_pRenderParam.bAlphaBlend = false

g_pObjectSkinMesh:SetForceShow(true)
-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): Phase 8 (GateWay) -- SetIsForceNotCull
-- doesn't exist anywhere in this repo's C++ at all, not just unbound (checked
-- X2Lib/KTDXLIB exhaustively; the closest analog, CKTDGObject::SetIsCulled, is
-- itself unbound to Lua under any name and has inverted semantics anyway).
-- Calling it would abort this file's load. Omitted; this rock mesh already
-- sits at y=-50000 (see SetPos above) and is never moved back up regardless
-- (its paired LineMap TAT_MESH_MOVE trigger is also gapped -- see
-- Ranox_GateWay_Stage1_LineMap.lua), so losing the force-render-when-offscreen
-- flag has no visible effect: the object stays hidden below the map either way.

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("GATEWAY_1STAGE_ROCK_ANI02.X")
g_pObjectSkinMesh:AddXSkinModel("GATEWAY_1STAGE_ROCK_ANI02.X")
g_pObjectSkinMesh:AddAnim("Normal")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_ONE_WAIT)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(-59260, -50000, 1440))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(false)

g_pRenderParam.bAlphaBlend = false

g_pObjectSkinMesh:SetForceShow(true)
-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): same SetIsForceNotCull gap and
-- disposition as the first "GATEWAY_1STAGE_ROCK_ANI01.X" object above.
