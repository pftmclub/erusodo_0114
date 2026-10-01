--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Velder.lua (live data217 copy, 1457 B), adopted wholesale.
-- DungeonIDList follows June: Henir moved to the Special map, secret dungeons by
-- their _HELL ID. A nil entry would end LocalParsing's read of the list early, so
-- IDs March has no enum for are omitted in place.
--}}

DLGBack = "DLG_Map_Local_Velder_Back_NEW.lua"
LOCAL_MAP_DLG = "DLG_Map_Local_Velder_Back_NEW.lua"
BaseWorldStateID = WORLD_MAP_ID.WMI_ELLIOS
EnterCondition = {
	RequireUnitLevel = 41,
	RequireClearDungeonID = {
		DUNGEON_ID.DI_PEITA_OFFERINGS_ALTER_NORMAL
	}
}
PositionData = {
	AURORA_POS_X = -305,
	AURORA_POS_Y = 80
}
VillageData = {
	TEX_NAME = "DLG_New_Dungeon_Image5.dds",
	PIECE_NAME = "Local_Image_Besma_Village",
	ID = VILLAGE_MAP_ID.VMI_VELDER,
	EXPLANATION = STR_ID_1111
}
VillageDGData = {
	TEX_NAME = "DLG_New_Dungeon_Image5.dds",
	PIECE_NAME = "Local_Image_Besma_Village",
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_GATE_VELDER,
	EXPLANATION = STR_ID_1111
}
VillageDungeonLoungeData = {
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_VELDER
}
BattleFieldRestData = {
	ID = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_REST_00
}
SpaData = {
	ID = VILLAGE_MAP_ID.VMI_INVALID
}
DungeonIDList = {
	DUNGEON_ID.DI_VELDER_THIRD_DWELLING_NORMAL,
	DUNGEON_ID.DI_VELDER_BRIDGE_HOPE_NORMAL,
	DUNGEON_ID.DI_VELDER_PALACE_ENTRANCE_NORMAL,
	DUNGEON_ID.DI_VELDER_BRIDGE_BUNNING_NORMAL,
	DUNGEON_ID.DI_VELDER_MARKET_NORMAL,
	DUNGEON_ID.DI_VELDER_GATE_NORMAL,
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_2016_XMAS (June event, no March enum)
	DUNGEON_ID.DI_DEFENCE_DUNGEON_ELDER_NORMAL,
	DUNGEON_ID.DI_VELDER_SECRET_HELL
}
BattleFieldIDList = {
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_01,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_02,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_03,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_FIELD_04,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_VELDER_SHIP_STAGE
}
