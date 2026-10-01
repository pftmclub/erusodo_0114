--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Elder.lua (live data034 copy, 1284 B), adopted wholesale.
-- DungeonIDList follows June: Henir moved to the Special map, secret dungeons by
-- their _HELL ID. A nil entry would end LocalParsing's read of the list early, so
-- IDs March has no enum for are omitted in place.
--}}

DLGBack = "DLG_Map_Local_Elder_Back_NEW.lua"
LOCAL_MAP_DLG = "DLG_Map_Local_Elder_Back_NEW.lua"
BaseWorldStateID = WORLD_MAP_ID.WMI_ELLIOS
EnterCondition = {
	RequireUnitLevel = 6,
	RequireClearDungeonID = {
		DUNGEON_ID.DI_RUBEN_SWAMP_NORMAL
	}
}
PositionData = {
	AURORA_POS_X = -160,
	AURORA_POS_Y = 105
}
VillageData = {
	TEX_NAME = "DLG_Common_Texture10.TGA",
	PIECE_NAME = "Dungeon_Image_Elder_Village",
	ID = VILLAGE_MAP_ID.VMI_ELDER,
	EXPLANATION = STR_ID_1109
}
VillageDGData = {
	TEX_NAME = "DLG_Common_Texture10.TGA",
	PIECE_NAME = "Dungeon_Image_Elder_Village",
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_GATE_ELDER,
	EXPLANATION = STR_ID_1109
}
VillageDungeonLoungeData = {
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_ELDER
}
BattleFieldRestData = {
	ID = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_REST_00
}
SpaData = {
	ID = VILLAGE_MAP_ID.VMI_INVALID
}
DungeonIDList = {
	DUNGEON_ID.DI_ELDER_BENDERS_CAVE_NORMAL,
	DUNGEON_ID.DI_ELDER_DRUID_FOREST_NORMAL,
	DUNGEON_ID.DI_ELDER_WALLY_CASTLE_ROOF_NORMAL,
	DUNGEON_ID.DI_ELDER_BELOW_PASSAGE_NORMAL,
	DUNGEON_ID.DI_ELDER_WALLY_CASTLE_CENTER_NORMAL,
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_HERO_DUMMY_COMMON (June Hero dungeon, no March enum)
	DUNGEON_ID.DI_ELDER_SECRET_HELL,
	DUNGEON_ID.DI_DEFENCE_DUNGEON_ELDER_NORMAL
}
BattleFieldIDList = {
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_01,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ELDER_FIELD_02
}
