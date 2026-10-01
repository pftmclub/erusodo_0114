--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Altera_Island.lua (live data217 copy, 1399 B), adopted wholesale.
-- DungeonIDList follows June: Henir moved to the Special map, secret dungeons by
-- their _HELL ID. A nil entry would end LocalParsing's read of the list early, so
-- IDs March has no enum for are omitted in place.
--}}

DLGBack = "DLG_Map_Local_Altera_Island_Back_NEW.lua"
LOCAL_MAP_DLG = "DLG_Map_Local_Altera_Island_Back_NEW.lua"
BaseWorldStateID = WORLD_MAP_ID.WMI_ELLIOS
EnterCondition = {
	RequireUnitLevel = 27,
	RequireClearDungeonID = {
		DUNGEON_ID.DI_BESMA_AIRSHIP_NORMAL
	}
}
PositionData = {
	AURORA_POS_X = -95,
	AURORA_POS_Y = 45
}
VillageData = {
	TEX_NAME = "DLG_Common_Texture10.TGA",
	PIECE_NAME = "Dungeon_Image_Elder_Village",
	ID = VILLAGE_MAP_ID.VMI_ALTERA,
	EXPLANATION = STR_ID_1101
}
VillageDGData = {
	TEX_NAME = "DLG_Common_Texture10.TGA",
	PIECE_NAME = "Dungeon_Image_Elder_Village",
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_GATE_ALTERA,
	EXPLANATION = STR_ID_1101
}
VillageDungeonLoungeData = {
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_ALTERA
}
BattleFieldRestData = {
	ID = VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_REST_00
}
SpaData = {
	ID = VILLAGE_MAP_ID.VMI_INVALID
}
DungeonIDList = {
	DUNGEON_ID.DI_ALTERA_BATTLE_AIR_SHIP_NORMAL,
	DUNGEON_ID.DI_ALTERA_PLAIN_RECYCLE_NORMAL,
	DUNGEON_ID.DI_ALTERA_BELOW_TUNNEL_NORMAL,
	DUNGEON_ID.DI_ALTERA_PLAIN_NORMAL,
	DUNGEON_ID.DI_ALTERA_NASOD_FACTORY_NORMAL,
	DUNGEON_ID.DI_ALTERA_CORE_NORMAL,
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_HERO_DUMMY_COMMON (June Hero dungeon, no March enum)
	DUNGEON_ID.DI_ALTERA_SECRET_HELL,
	DUNGEON_ID.DI_DEFENCE_DUNGEON_ELDER_NORMAL,
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_BESTERA_START (June Bestera, no March enum)
}
BattleFieldIDList = {
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_01,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_02,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_ALTERA_FIELD_03
}
