--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Ruben_Back_NEW.lua (live data034 copy, 833 B), adopted wholesale.
-- Identical to March's apart from Static_HeroMode_Filter, which CX2LocalMapUI::
-- UpdateDungeonButtons shows only in hero mode (DLG_Map_Local_HeroMode_Filter, data065).
--}}

g_pStaticVelder_North = g_pUIDialog:CreateStatic()

g_pUIDialog:AddControl(g_pStaticVelder_North)
g_pStaticVelder_North:SetName("Velder_North_BG")

g_pPictureVelder_North_Bg1 = g_pUIDialog:CreatePicture()

g_pStaticVelder_North:AddPicture(g_pPictureVelder_North_Bg1)
g_pPictureVelder_North_Bg1:SetTex("DLG_New_Region_Ruben01.dds", "Region_Velder_North01")
g_pPictureVelder_North_Bg1:SetPoint({
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
IncludeLua("DLG_Map_Local_Ruben_Icon_NEW.lua")
