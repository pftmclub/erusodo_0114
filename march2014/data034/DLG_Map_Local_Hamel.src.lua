--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Hamel.lua (live data217 copy, 1487 B), adopted wholesale.
-- DungeonIDList follows June: Henir moved to the Special map, secret dungeons by
-- their _HELL ID. A nil entry would end LocalParsing's read of the list early, so
-- IDs March has no enum for are omitted in place.
--}}

DLGBack = "DLG_Map_Local_Hamel_Back_NEW.lua"
LOCAL_MAP_DLG = "DLG_Map_Local_Hamel_Back_NEW.lua"
BaseWorldStateID = WORLD_MAP_ID.WMI_ELLIOS
EnterCondition = {
	RequireUnitLevel = 48,
	RequireClearDungeonID = {
		DUNGEON_ID.DI_VELDER_GATE_NORMAL
	}
}
PositionData = {
	AURORA_POS_X = -305,
	AURORA_POS_Y = 80
}
VillageData = {
	TEX_NAME = "DLG_New_Dungeon_Image5.dds",
	PIECE_NAME = "Local_Image_Besma_Village",
	ID = VILLAGE_MAP_ID.VMI_HAMEL,
	EXPLANATION = STR_ID_1111
}
VillageDGData = {
	TEX_NAME = "DLG_New_Dungeon_Image5.dds",
	PIECE_NAME = "Local_Image_Besma_Village",
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_GATE_HAMEL,
	EXPLANATION = STR_ID_1111
}
VillageDungeonLoungeData = {
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_HAMEL
}
BattleFieldRestData = {
	ID = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_REST_00
}
SpaData = {
	ID = VILLAGE_MAP_ID.VMI_INVALID
}
DungeonIDList = {
	DUNGEON_ID.DI_HAMEL_OUTSKIRTS_RESIAM_NORMAL,
	DUNGEON_ID.DI_HAMEL_BREAK_RESIAM_NORMAL,
	DUNGEON_ID.DI_HAMEL_ANCIENT_WATERWAY_NORMAL,
	DUNGEON_ID.DI_HAMEL_ANCIENT_WATERWAY_CORE_NORMAL,
	DUNGEON_ID.DI_HAMEL_CAVE_OF_MAGMANTA_NORMAL,
	DUNGEON_ID.DI_HAMEL_FROZEN_WATER_TEMPLE_NORMAL,
	DUNGEON_ID.DI_HAMEL_FROZEN_WATER_HALL_NORMAL,
	DUNGEON_ID.DI_DEFENCE_DUNGEON_ELDER_NORMAL,
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_HAMEL_SECRET_HELL (June Hamel secret dungeon, no March DungeonData)
}
BattleFieldIDList = {
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_01,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_02,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_03,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_FIELD_04,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_HAMEL_SHIP_STAGE
}
