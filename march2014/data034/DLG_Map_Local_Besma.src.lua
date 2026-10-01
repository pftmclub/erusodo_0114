--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Besma.lua (live data217 copy, 1375 B), adopted wholesale.
-- DungeonIDList follows June: Henir moved to the Special map, secret dungeons by
-- their _HELL ID. A nil entry would end LocalParsing's read of the list early, so
-- IDs March has no enum for are omitted in place.
--}}

DLGBack = "DLG_Map_Local_Besma_Back_NEW.lua"
LOCAL_MAP_DLG = "DLG_Map_Local_Besma_Back_NEW.lua"
BaseWorldStateID = WORLD_MAP_ID.WMI_ELLIOS
EnterCondition = {
	RequireUnitLevel = 15,
	RequireClearDungeonID = {
		DUNGEON_ID.DI_ELDER_WALLY_CASTLE_CENTER_NORMAL
	}
}
PositionData = {
	AURORA_POS_X = -305,
	AURORA_POS_Y = 80
}
VillageData = {
	TEX_NAME = "DLG_New_Dungeon_Image5.dds",
	PIECE_NAME = "Local_Image_Besma_Village",
	ID = VILLAGE_MAP_ID.VMI_BESMA,
	EXPLANATION = STR_ID_1111
}
VillageDGData = {
	TEX_NAME = "DLG_New_Dungeon_Image5.dds",
	PIECE_NAME = "Local_Image_Besma_Village",
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_GATE_BESMA,
	EXPLANATION = STR_ID_1111
}
VillageDungeonLoungeData = {
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_BESMA
}
BattleFieldRestData = {
	ID = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_REST_00
}
SpaData = {
	ID = VILLAGE_MAP_ID.VMI_INVALID
}
DungeonIDList = {
	DUNGEON_ID.DI_BESMA_DRAGON_ROAD_NORMAL,
	DUNGEON_ID.DI_BESMA_LAKE_NORMAL,
	DUNGEON_ID.DI_BESMA_MINE2_NORMAL,
	DUNGEON_ID.DI_BESMA_AIRSHIP_NORMAL,
	DUNGEON_ID.DI_BESMA_LAKE_NIGHT_NORMAL,
	DUNGEON_ID.DI_BESMA_DRAGON_NEST_NORMAL,
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_HERO_DUMMY_COMMON (June Hero dungeon, no March enum)
	DUNGEON_ID.DI_BESMA_SECRET_HELL,
	DUNGEON_ID.DI_DEFENCE_DUNGEON_ELDER_NORMAL,
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_BESTERA_START (June Bestera, no March enum)
}
BattleFieldIDList = {
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_01,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_02,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_BESMA_FIELD_03
}
