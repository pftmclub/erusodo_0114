--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Sander.lua (live data217 copy, 1260 B), adopted wholesale.
-- DungeonIDList follows June: Henir moved to the Special map, secret dungeons by
-- their _HELL ID. A nil entry would end LocalParsing's read of the list early, so
-- IDs March has no enum for are omitted in place.
--}}

DLGBack = "DLG_Map_Local_Sander_Back_NEW.lua"
LOCAL_MAP_DLG = "DLG_Map_Local_Sander_Back_NEW.lua"
BaseWorldStateID = WORLD_MAP_ID.WMI_ELLIOS
EnterCondition = {
	RequireUnitLevel = 58,
	RequireClearDungeonID = {
		DUNGEON_ID.DI_HAMEL_FROZEN_WATER_HALL_NORMAL
	}
}
PositionData = {
	AURORA_POS_X = -305,
	AURORA_POS_Y = 80
}
VillageData = {
	TEX_NAME = "DLG_New_Dungeon_Image5.dds",
	PIECE_NAME = "Local_Image_Besma_Village",
	ID = VILLAGE_MAP_ID.VMI_SANDER,
	EXPLANATION = STR_ID_1111
}
VillageDGData = {
	ID = -1
}
BattleFieldRestData = {
	ID = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_REST_00
}
VillageDungeonLoungeData = {
	ID = -1
}
SpaData = {
	ID = VILLAGE_MAP_ID.VMI_INVALID
}
DungeonIDList = {
	DUNGEON_ID.DI_SANDER_DRY_SANDER_NORMAL,
	DUNGEON_ID.DI_SANDER_GARPAI_ROCK_NORMAL,
	DUNGEON_ID.DI_SANDER_DEN_OF_TROCK_NORMAL,
	DUNGEON_ID.DI_SANDER_KARUSO_VILLAGE_NORMAL,
	DUNGEON_ID.DI_SANDER_SANTILUS_SHIP_NORMAL,
	DUNGEON_ID.DI_SANDER_BEHIMOSS_HEART_NORMAL,
	DUNGEON_ID.DI_DEFENCE_DUNGEON_ELDER_NORMAL,
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_SANDER_SECRET_HELL (June Sander secret dungeon, no March enum)
}
BattleFieldIDList = {
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_01,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_02,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_03,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_SANDER_FIELD_04
}
