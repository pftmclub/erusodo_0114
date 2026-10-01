--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Ruben.lua (live data217 copy, 923 B), adopted wholesale.
-- DungeonIDList follows June: Henir moved to the Special map, secret dungeons by
-- their _HELL ID. A nil entry would end LocalParsing's read of the list early, so
-- IDs March has no enum for are omitted in place.
--}}

DLGBack = "DLG_Map_Local_Ruben_Back_NEW.lua"
LOCAL_MAP_DLG = "DLG_Map_Local_Ruben_Back_NEW.lua"
BaseWorldStateID = WORLD_MAP_ID.WMI_ELLIOS
EnterCondition = {
	RequireUnitLevel = 1,
	RequireClearDungeonID = {
		0
	}
}
PositionData = {
	AURORA_POS_X = -280,
	AURORA_POS_Y = 175
}
VillageData = {
	TEX_NAME = "DLG_Common_Texture10.TGA",
	PIECE_NAME = "Dungeon_Image_Ruben_Village",
	ID = VILLAGE_MAP_ID.VMI_RUBEN,
	EXPLANATION = STR_ID_1110
}
VillageDGData = {
	TEX_NAME = "DLG_Common_Texture10.TGA",
	PIECE_NAME = "Dungeon_Image_Ruben_Village",
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_GATE_RUBEN,
	EXPLANATION = STR_ID_1110
}
VillageDungeonLoungeData = {
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_RUBEN
}
SpaData = {
	-- [June UI parity Phase 3] omitted: ID = VILLAGE_MAP_ID.VMI_SPA_RUBEN (June Ruben spa, no March enum; March does not read SpaData)
}
DungeonIDList = {
	DUNGEON_ID.DI_RUBEN_EL_TREE_NORMAL,
	DUNGEON_ID.DI_RUBEN_RUIN_OF_ELF_NORMAL,
	DUNGEON_ID.DI_RUBEN_SWAMP_NORMAL
}
BattleFieldIDList = {
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_RUBEN_FIELD_01
}
