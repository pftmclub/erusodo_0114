--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Special_Back_NEW.lua (live data217 copy, 818 B), adopted wholesale.
-- March has no LMI_SPECIAL content of its own; this map exists to carry Henir,
-- which June moved off every village map onto it. Only Henir is kept.
-- Background DLG_New_Region_SPECIAL_01.dds (June data215) is staged with it.
--}}

g_pStaticSander_Local = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticSander_Local)
g_pStaticSander_Local:SetName("Special_Local")

g_pPictureSander_Bg1 = g_pUIDialog:CreatePicture()

g_pStaticSander_Local:AddPicture(g_pPictureSander_Bg1)
g_pPictureSander_Bg1:SetTex("DLG_New_Region_SPECIAL_01.dds", "HAMEL_BG1")
g_pPictureSander_Bg1:SetPoint({
	ADD_SIZE_X = 62,
	CHANGE_TIME = 0,
	LEFT_TOP = D3DXVECTOR2(0, 0)
})

g_pStatic_HeroMode_Filter = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStatic_HeroMode_Filter)
g_pStatic_HeroMode_Filter:SetName("Static_HeroMode_Filter")

g_pPicture_HeroMode_Filter = g_pUIDialog:CreatePicture()

g_pStatic_HeroMode_Filter:AddPicture(g_pPicture_HeroMode_Filter)
g_pPicture_HeroMode_Filter:SetTex("DLG_Map_Local_HeroMode_Filter.tga", "Region_HeroMode_Filter")
g_pPicture_HeroMode_Filter:SetPoint({
	ADD_SIZE_X = 62,
	CHANGE_TIME = 0,
	ADD_SIZE_Y = -119,
	LEFT_TOP = D3DXVECTOR2(0, 1)
})
IncludeLua("DLG_Map_Local_Special_Icon_NEW.lua")
