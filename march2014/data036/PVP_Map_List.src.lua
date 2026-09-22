PVP_MAP_LIST = {
	{
		m_MapTexName = "DLG_New_PVP_Image.dds",
		m_MapTexKeyName = "PVP_EL_TREE_HIGH",
		m_WorldID = WORLD_ID.WI_TREE_OF_LIGHT_TOP,
		m_MapName = STR_ID_1145,
		m_MapSize = STR_ID_13600,
		m_MapDesc = STR_ID_13603
	},
	{
		m_MapTexName = "DLG_New_PVP_Image.dds",
		m_MapTexKeyName = "PVP_EL_TREE_LOW",
		m_WorldID = WORLD_ID.WI_FOREST_OF_EL,
		m_MapName = STR_ID_1146,
		m_MapSize = STR_ID_13600,
		m_MapDesc = STR_ID_13604
	},
	{
		m_MapTexName = "DLG_New_PVP_Image02.dds",
		m_MapTexKeyName = "PVP_BENDERS_ROOM",
		m_WorldID = WORLD_ID.WI_ELDER_BENDERS_CAVE_STAGE3,
		m_MapName = STR_ID_1147,
		m_MapSize = STR_ID_13601,
		m_MapDesc = STR_ID_13605
	},
	{
		m_MapTexName = "DLG_New_PVP_Image.dds",
		m_MapTexKeyName = "PVP_WALLY_CASTLE_CENTER",
		m_WorldID = WORLD_ID.WI_ELDER_WALLY_CASTLE_CENTER_STAGE4,
		m_MapName = STR_ID_1148,
		m_MapSize = STR_ID_13601,
		m_MapDesc = STR_ID_13606
	},
	{
		m_MapTexName = "DLG_New_PVP_Image.dds",
		m_MapTexKeyName = "PVP_WALLY_CASTLE_ROOF",
		m_WorldID = WORLD_ID.WI_ELDER_WALLY_CASTLE_ROOF_STAGE4,
		m_MapName = STR_ID_1149,
		m_MapSize = STR_ID_13600,
		m_MapDesc = STR_ID_13607
	},
	{
		m_MapTexName = "DLG_New_PVP_Image02.dds",
		m_MapTexKeyName = "PVP_BESMA_LAKE",
		m_WorldID = WORLD_ID.WI_BESMA_BESMA_LAKE_PVPMAP,
		m_MapName = STR_ID_1150,
		m_MapSize = STR_ID_13602,
		m_MapDesc = STR_ID_13608
	},
	{
		m_MapTexName = "DLG_New_PVP_Image02.dds",
		m_MapTexKeyName = "PVP_AIR_SHIP_IN",
		m_WorldID = WORLD_ID.WI_BESMA_AIR_SHIP_PVPMAP,
		m_MapName = STR_ID_1151,
		m_MapSize = STR_ID_13601,
		m_MapDesc = STR_ID_13609
	},
	{
		m_MapTexName = "DLG_New_PVP_Image02.dds",
		m_MapTexKeyName = "PVP_ELEOS_BAY",
		m_WorldID = WORLD_ID.WI_SUMMER_2008_PVP,
		m_MapName = STR_ID_1152,
		m_MapSize = STR_ID_13601,
		m_MapDesc = STR_ID_13610
	},
	{
		m_MapTexName = "DLG_New_PVP_Image03.dds",
		m_MapTexKeyName = "MAP_SELECT_IMAGE_DURAHAN",
		m_WorldID = WORLD_ID.WI_PEITA_OFFERING_PVP_STAGE,
		m_MapName = STR_ID_1153,
		m_MapSize = STR_ID_13600,
		m_MapDesc = STR_ID_13611
	},
	{
		m_MapTexName = "DLG_New_PVP_Image03.dds",
		m_MapTexKeyName = "MAP_SELECT_IMAGE_ARENA",
		m_WorldID = WORLD_ID.WI_PEITA_SPIRAL_CORRIDOR_PVP_STAGE,
		m_MapName = STR_ID_1154,
		m_MapSize = STR_ID_13602,
		m_MapDesc = STR_ID_13612
	},
	{
		m_MapTexName = "DLG_New_PVP_Image04.dds",
		m_MapTexKeyName = "PVP_Hope_Bridge",
		m_WorldID = WORLD_ID.WI_ELDER_BRIDGE_HOPE_PVPMAP,
		m_MapName = STR_ID_4620,
		m_MapSize = STR_ID_13602,
		m_MapDesc = STR_ID_13613
	},
	{
		m_MapTexName = "DLG_New_PVP_Image04.dds",
		m_MapTexKeyName = "PVP_Velder_Winter",
		m_WorldID = WORLD_ID.WI_WINTER_PVPMAP,
		m_MapName = STR_ID_4833,
		m_MapSize = STR_ID_13601,
		m_MapDesc = STR_ID_13614
	},
	{
		m_MapTexName = "DLG_New_PVP_Image05.dds",
		m_MapTexKeyName = "MAP_SELECT_IMAGE_KARUPAI",
		m_WorldID = WORLD_ID.WI_SANDER_GARPAI_ROCK_PVPMAP,
		m_MapName = STR_ID_23931,
		m_MapSize = STR_ID_13600,
		m_MapDesc = STR_ID_23948
	},
	{
		m_MapTexName = "DLG_New_PVP_Image05.dds",
		m_MapTexKeyName = "MAP_SELECT_IMAGE_SANDER_OASIS",
		m_WorldID = WORLD_ID.WI_SANDER_VILLIAGE_PVPMAP,
		m_MapName = STR_ID_23932,
		m_MapSize = STR_ID_13602,
		m_MapDesc = STR_ID_23949
	},
	-- SERV_IRUHADEV_RANOX_PORT (2026-09-19): Ranox port Phase 11. Transcribed
	-- verbatim from June2017's own PVP_Map_List.lua, which continues right
	-- after this with a WI_ELYSION_FIELD_PVPMAP row (m_MapTexKeyName
	-- "MAP_SELECT_ELISION") -- not ported, next content chapter, out of this
	-- port's scope. m_MapName/m_MapDesc (STR_ID_44260/44261) are the same
	-- unverified-string-table-existence risk Phases 5-8 already flagged for
	-- other Ranox STR_IDs (March2014's own data036/StringTable.lua is still
	-- bytecode, not decompiled, so these weren't checked to resolve to real
	-- display strings) -- cosmetic only if unresolved (blank map name/
	-- description in the PVP map-select UI), carried forward rather than
	-- re-closed.
	{
		m_MapTexName = "DLG_New_PVP_Image09.dds",
		m_MapTexKeyName = "MAP_SELECT_RANOX",
		m_WorldID = WORLD_ID.WI_RANOX_GATEWAY_PVPMAP,
		m_MapName = STR_ID_44260,
		m_MapSize = STR_ID_13600,
		m_MapDesc = STR_ID_44261
	}
}
