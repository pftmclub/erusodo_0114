g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetOriginColor_LUA(0.666667, 0.705882, 0.94902)
g_pWorld:SetLineMap("Ranox_FinalHall_Stage3_LineMap.lua")
g_pWorld:SetBGM("Ranox_FinalHall_Stage3.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_DarkGate_Drone01.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_DarkGate_Drone02.ogg")
g_pWorld:SetFogWorld(6000, 17400, 5067, -2199, -0.42, D3DXCOLOR(0.839216, 0.164706, 0.0156863, 1))
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

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_AP_blue.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_AP_blue.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.176471, 0.156863, 0.564706, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_AP_blue02.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_AP_blue02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.180392, 0.160784, 0.564706, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE

g_pObjectMesh:SetUseWorldColor(false)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_AP_chain.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_AP_chain.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_AP_crack.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_AP_crack.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_AP_dark.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_AP_dark.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.356863, 0, 0.376471, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_AP_fire.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_AP_fire.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.7)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.270588, 0.180392, 0.909804, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_AP_fire02.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_AP_fire02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_DOME_1)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, 0.8)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0, 0, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_stage03_bg_wall02.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_stage03_bg_wall02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.454902, 0.380392, 0.576471, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(6000, 17400, 5067, -2199, D3DXCOLOR(0.839216, 0.164706, 0.0156863, 1), -0.42)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_blackstone.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_blackstone.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_chain.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_chain.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.494118, 0.705882, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(6000, 17400, 5067, -2199, D3DXCOLOR(0.839216, 0.164706, 0.0156863, 1), -0.42)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_G01.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_G01.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true

g_pObjectMesh:SetFog(6000, 17400, 5067, -2199, D3DXCOLOR(0.839216, 0.164706, 0.0156863, 1), -0.42)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_G01_column.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_G01_column.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.839216, 0.615686, 0.529412, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(6000, 17400, 5067, -2199, D3DXCOLOR(0.839216, 0.164706, 0.0156863, 1), -0.42)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_magma.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_magma.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetUseWorldColor(false)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_magma02.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_magma02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.2)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetUseWorldColor(false)
g_pObjectMesh:SetFog(6000, 17400, 5067, -2199, D3DXCOLOR(0.839216, 0.164706, 0.0156863, 1), -0.42)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_wall.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_wall.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 0.74902, 0.796078, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(6000, 17400, 5067, -2199, D3DXCOLOR(0.839216, 0.164706, 0.0156863, 1), -0.42)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("FinalHall_Stage03_AP_chain02.Y")
g_pObjectMesh:SetXMeshLOD("FinalHall_Stage03_AP_chain02.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.494118, 0.701961, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE

g_pObjectMesh:SetFog(6000, 17400, 5067, -2199, D3DXCOLOR(0.839216, 0.164706, 0.0156863, 1), -0.42)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("FINALHALL_STAGE03_X_MAGMA.X")
g_pObjectSkinMesh:AddXSkinModel("FINALHALL_STAGE03_X_MAGMA.X")
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

g_pObjectSkinMesh:SetUseWorldColor(false)
g_pObjectSkinMesh:SetFog(6000, 17400, 5067, -2199, D3DXCOLOR(0.839216, 0.164706, 0.0156863, 1), -0.42)
