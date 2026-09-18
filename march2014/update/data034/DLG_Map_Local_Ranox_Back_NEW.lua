g_pStaticSander_Local = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticSander_Local)
g_pStaticSander_Local:SetName("Ranox_Local")

g_pPictureSander_Bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Local:AddPicture(g_pPictureSander_Bg1)
g_pPictureSander_Bg1:SetTex("DLG_New_Region_Ranox_01.dds", "Region_RANOX")
g_pPictureSander_Bg1:SetPoint({
	ADD_SIZE_X = 62,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})

-- SERV_IRUHADEV_RANOX_PORT (2026-09-18): g_pStatic_HeroMode_Filter, June2017's
-- overlay for the Hero-difficulty toggle, omitted here. Not one of the 8
-- confirmed engine gaps by itself, but Hero mode is a whole separate
-- registry/UI feature this March2014 build has no other trace of -- see
-- RANOX_PORT_PLAN.md's "Hero difficulty tier: out of scope by default" and
-- Phase 12. This March2014 client's own Sander equivalent
-- (data034/DLG_Map_Local_Sander_Back_NEW.lua) has no such block either.
IncludeLua("DLG_Map_Local_Ranox_Icon_NEW.lua")
