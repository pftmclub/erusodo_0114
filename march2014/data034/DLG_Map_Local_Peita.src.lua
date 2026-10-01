--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Peita.lua (live data217 copy, 1087 B), adopted wholesale.
-- DungeonIDList follows June: Henir moved to the Special map, secret dungeons by
-- their _HELL ID. A nil entry would end LocalParsing's read of the list early, so
-- IDs March has no enum for are omitted in place.
-- LOCAL_MAP_DLG now names June's DLG_Map_Local_Peita_Back_NEW.lua.
--}}

DLGBack = "DLG_Map_Local_Peita_Back_NEW.lua"
LOCAL_MAP_DLG = "DLG_Map_Local_Peita_Back_NEW.lua"
BaseWorldStateID = WORLD_MAP_ID.WMI_ELLIOS
EnterCondition = {
	RequireUnitLevel = 34,
	RequireClearDungeonID = {
		DUNGEON_ID.DI_ALTERA_CORE_NORMAL
	}
}
VillageData = {
	ID = -1
}
VillageDGData = {
	TEX_NAME = "DLG_Common_Texture10.TGA",
	PIECE_NAME = "Dungeon_Image_Ruben_Village",
	ID = VILLAGE_MAP_ID.VMI_PEITA,
	EXPLANATION = STR_ID_1108
}
BattleFieldRestData = {
	ID = -1
}
VillageDungeonLoungeData = {
	ID = VILLAGE_MAP_ID.VMI_DUNGEON_LOUNGE_PEITA
}
SpaData = {
	ID = VILLAGE_MAP_ID.VMI_INVALID
}
DungeonIDList = {
	DUNGEON_ID.DI_PEITA_OFFERINGS_NORMAL,
	DUNGEON_ID.DI_PEITA_SPIRAL_CORRIDOR_NORMAL,
	DUNGEON_ID.DI_PEITA_CHAPEL_NORMAL,
	DUNGEON_ID.DI_PEITA_UNDER_GARDEN_NORMAL,
	DUNGEON_ID.DI_PEITA_TOWER_HEART_NORMAL,
	DUNGEON_ID.DI_PEITA_OFFERINGS_ALTER_NORMAL,
	DUNGEON_ID.DI_DEFENCE_DUNGEON_ELDER_NORMAL
}
BattleFieldIDList = {
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_01,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_02,
	VILLAGE_MAP_ID.VMI_BATTLE_FIELD_PEITA_FIELD_03
}
