--{{ Iruha : 2026-10-01 // June UI parity Phase 3 (SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS)
-- June2017 DLG_Map_Local_Special.lua (live data217 copy, 566 B), adopted wholesale.
-- March has no LMI_SPECIAL content of its own; this map exists to carry Henir,
-- which June moved off every village map onto it. Only Henir is kept.
--}}

DLGBack = "DLG_Map_Local_Special_Back_NEW.lua"
LOCAL_MAP_DLG = "DLG_Map_Local_Special_Back_NEW.lua"
BaseWorldStateID = WORLD_MAP_ID.WMI_ELLIOS
EnterCondition = {
	RequireUnitLevel = 40
}
PositionData = {
	AURORA_POS_X = -305,
	AURORA_POS_Y = 80
}
VillageData = {}
BattleFieldRestData = {}
DungeonIDList = {
	DUNGEON_ID.DI_ELDER_HENIR_SPACE,
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_HERO_DUMMY_COMMON (June-only content, no March enum)
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_2017_ERDA (June-only content, no March enum)
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_ADD_CENTER (June-only content, no March enum)
	-- [June UI parity Phase 3] omitted: DUNGEON_ID.DI_SPACE_OF_THOUGHT_DUMMY (June-only content, no March enum)
}
-- [June UI parity Phase 3] omitted: NewEventDungeonIDList (feeds June's g_pStatic_NewSpecial badge, a June-only event system).
BattleFieldIDList = {}
