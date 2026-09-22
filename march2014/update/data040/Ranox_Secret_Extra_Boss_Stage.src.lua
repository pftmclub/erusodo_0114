g_pWorld:SetLightPos(50000, 50000, -100000)
g_pWorld:SetLineMap("Ranox_Secret_Boss_Stage_LineMap.lua")
g_pWorld:SetBGM("LI_Boss_Ranox001.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_SD_Stage03_A.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_SD_Stage03_B.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_SD_Stage03_C.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_SD_Stage03_D.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_SD_Stage03_E.ogg")
g_pWorld:AddEffectBGM_LUA("Amb_Ranox_SD_Stage03_F.ogg")
g_pWorld:SetProjection(10, 999999, 4, 0, 0, 0, 0)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("WorldLayer_Alpha.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0.5, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0, 0, 0.913725, 0.929412))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(0)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("WorldLayer_Velder_Gate.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0.5, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0.521569, 0, 1, 1))
g_pWorldLayer:SetTexUV(0.25, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(1)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("WorldLayer_Velder_Gate.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0.4, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0.709804, 0.4, 0.247059, 0.8))
g_pWorldLayer:SetTexUV(1, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(2)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(0)
g_pWorldLayer:SetTex("Velder_Extra_blend.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0.921569, 0, 0, 0.384314))
g_pWorldLayer:SetTexUV(0, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(3)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(1)
g_pWorldLayer:SetTex("WorldLayer_Velder_Gate.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0.1, 1, 1)
g_pWorldLayer:SetColor(D3DXCOLOR(0.878431, 0.721569, 0.321569, 0.658824))
g_pWorldLayer:SetTexUV(0.3, 0)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(4)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(1)
g_pWorldLayer:SetTex("dust_02.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 0.8, 0.8)
g_pWorldLayer:SetColor(D3DXCOLOR(0.466667, 0, 0.909804, 1))
g_pWorldLayer:SetTexUV(0.15, 0.4)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(5)
g_pWorldLayer:SetInitShow(true)

g_pWorldLayer = g_pWorld:CreateWorldLayer()

g_pWorldLayer:SetLayer(X2_LAYER.XL_SKY_WORLD_OBJECT_OVER_EFFECT_0)
g_pWorldLayer:SetBlendType(1)
g_pWorldLayer:SetTex("dust_01.tga")
g_pWorldLayer:SetAlphaObject(true)
g_pWorldLayer:SetPos(0, 0, 0.8, 0.8)
g_pWorldLayer:SetColor(D3DXCOLOR(0, 0.0313726, 0.913725, 1))
g_pWorldLayer:SetTexUV(0.3, 0.6)
g_pWorldLayer:SetFeedBackUV(0, 0)
g_pWorldLayer:SetLayerId(6)
g_pWorldLayer:SetInitShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Boss_barricade.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Boss_barricade.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-65000, -64500, 18050, 10500, D3DXCOLOR(0.992157, 0.0196078, 0.164706, 1), -0.39)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.768627, 0.741176, 0.996078, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pRenderParam.bAlphaTestEnable = true
g_pRenderParam.cullMode = D3DCULL.D3DCULL_NONE

g_pObjectMesh:SetFog(-65000, -64500, 18050, 10500, D3DXCOLOR(0.992157, 0.0196078, 0.164706, 1), -0.39)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_AP_Back.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_AP_Back.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.372549, 0.290196, 0.94902, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectMesh:SetFog(54000, -5000, 122500, -59500, D3DXCOLOR(0, 0.054902, 0.129412, 1), -0.21)
g_pObjectMesh:SetForceShow(true)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_AP_Bridge.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_AP_Bridge.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.619608, 0.611765, 0.984314, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_AP_Crack.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_AP_Crack.Y")
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
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_AP_Magma.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_AP_Magma.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0, -0.08)
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

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_Back.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_Back.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.0509804, 0.145098, 0.662745, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(6500, 0, 0, 0, D3DXCOLOR(0.0156863, 0.0156863, 0.333333, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_Boss_Foothold.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_Boss_Foothold.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.517647, 0.682353, 0.988235, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_BossObject.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_BossObject.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.239216, 0.882353, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_Cliff.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_Cliff.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(0.901961, 0.67451, 0.988235, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(50000, -2000, 0, 0, D3DXCOLOR(0.0901961, 0.0431373, 0.196078, 1), 0)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_CliffGround.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_CliffGround.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(-102000, 85500, 7500, 20000, D3DXCOLOR(0.0117647, 0.188235, 0.6, 1), -0.08)

g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_Magma.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_Magma.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(-0.05, -0.1)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_Magma_B.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_Magma_B.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetTex0UVSpeed(0.03, -0.07)
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()
g_pObjectMesh = g_pWorld:CreateObjectMesh()

g_pObjectMesh:SetXMesh("Ranox_Secret_Stage3_Rock.Y")
g_pObjectMesh:SetXMeshLOD("Ranox_Secret_Stage3_Rock.Y")
g_pObjectMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_0)
g_pObjectMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectMesh:SetCanHide(false, false)
g_pObjectMesh:SetNotRenderWhenHidden(false)

g_pRenderParam = g_pObjectMesh:GetRenderParam()

g_pObjectMesh:SetFog(16000, -5000, 26000, -4000, D3DXCOLOR(0.00784314, 0.160784, 0.521569, 1), 0.07)

g_pObjectSkinMesh = g_pWorld:CreateObjectSkinMesh()

g_pObjectSkinMesh:SetXSkinMotion("MOTION_BOSS_STAGE_RAND.X")
g_pObjectSkinMesh:AddXSkinModel("MOTION_BOSS_STAGE_RAND.X")
g_pObjectSkinMesh:AddAnim("Wait")
g_pObjectSkinMesh:PlayAnim(WORLD_OBJECT_SKIN_MESH_PLAY_TYPE.WOSMPY_LOOP)
g_pObjectSkinMesh:SetLayer(X2_LAYER.XL_SKY_WORLD_BASE_1)
g_pObjectSkinMesh:SetPos(D3DXVECTOR3(0, 0, 0))
g_pObjectSkinMesh:SetOriginalColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetColorFactor(D3DXCOLOR(1, 1, 1, 1))
g_pObjectSkinMesh:SetCanHide(false, false)

g_pRenderParam = g_pObjectSkinMesh:GetRenderParam()

g_pObjectSkinMesh:SetAlphaObject(true)

g_pRenderParam.bAlphaBlend = true

g_pObjectSkinMesh:SetForceShow(true)
g_pObjectSkinMesh:SetAnimSpeed(1)
