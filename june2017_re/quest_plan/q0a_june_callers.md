# Quest Q0a - June IsCompleteQuest callers and the inert-key readers

Source: Ghidra (unpacked_x2.exe, imagebase 31aa0000), june2017_re/funcmap.tsv, xorstrings.tsv (6,241 rows),
quest_plan/q_xor_quest.tsv, March X2Lib. Read-only. All VAs are June. "March" = TRUNK/X2Lib (relative paths).

## Key finding for the whole table

June `CX2QuestManager::IsCompleteQuest` **32322260** (thiscall on `g_pData(33775860)+0x4c`, one stack arg, `find` on the map at
this+0x34, end at this+0x38) is **March `CX2QuestManager::GetUnitCompleteQuest( int )`** (X2QuestManager.cpp:1339-1348,
`m_mapCompleteQuest.find()`). So every March site that calls GetUnitCompleteQuest has the same primitive; "same?" below compares
what the site tests, not the primitive.
Neighbours seen on the way: 32322310 = GetQuestTemplet (returns NULL also when templet[0] != -1 and != g_pInstanceData+0x120, a
server-group filter), 32322380 = GetUnitQuest (map this+0x24/+0x28), 3232e510 = IsForbiddenQuest, 32336e80 = SetUnitQuest
(March X2QuestManager.cpp:1022 / :1145).

## Part A - the 17 call sites

| # | call_site | caller | proposed name | quest id source | what it gates | March equivalent (file:line) | same? |
|---|---|---|---|---|---|---|---|
| 1 | 3293cbf1 | 3293c6f0 | CX2LocalMapUI::UpdateLocalMapDLG (funcmap "likely"; the region-tab loop) | local-map templet **+0x70** (EBX = templet from GetLocalMapTemplet 324f2360; `PUSH EAX` @3293cbf0; parser 32501bb0 key RequireCompletedQuestID, default 0) | tab radio hidden (`vslot 0x78` SetShowEnable(0,0)) when `+0x70 > 0` and quest not complete; sits right after the level gate (`level < templet+0x18` @3293cbc3-3293cbd9) | X2LocalMapUI.cpp:850 (live copy; level gate :1017-1022; the quest test is a comment at :1024-1029, omitted) | NO - March has the level gate only |
| 2 | 323d5a5a | 323d5650 | CX2UIQuestNew::UpdateOngoingQuestInfo (June; builds the ongoing/quick list rows; callers 323dd640, 323dfa80, 324708c0) | not a constant: the quest being listed, `[ESP+0x24]` = the vector element (`local_d8`, loop over this+0x9c list); epic branch = templet type 5 (sort 0) | state of an Epic row: complete -> state 0 (QSS_COMPLETE_EPIC); else level < templet+0x1d4 (PlayLevel) -> 2 (QSS_WAIT_EPIC); else inst complete -> 1, else 4 (ONGOING) | X2UIQuestNew.cpp:4725 (function); GetUnitCompleteQuest at :4777 | YES (same epic test, same order) |
| 3 | 323d5aec | 323d5650 | same function, the **QT_STORY** branch (templet type 10 -> UI sort 8, `local_bc == 8`) | the quest being listed (`local_d8`) | row state 6 (completed story) when complete; else 5 if no inst, else 1 / 4 from inst completeness; sets `param_1+0x758 / +0x760` | none: March has no QT_STORY (type 10); March's event/normal rows never test completion (X2UIQuestNew.cpp:4786-4816) | NO - June-only quest type |
| 4 | 323caba3 | 323cab40 | CX2UIQuestNew::CanIAcceptQuest( int ) (stdcall, 1 arg; GetQuestTemplet 32322310 + IsForbiddenQuest 3232e510, then level +200/+0x130, item +0xe4/+0xe8, NotBefore +0xec) | each entry of templet **+0xf0** vector (March `m_Condition.m_vecBeforeQuestID`), 0 entries skipped | returns false (quest not acceptable) if any before-quest is not complete | X2UIQuestNew.cpp:3013 (loop :3025-3033, GetUnitCompleteQuest :3028); duplicate in X2UIQuestReceive.cpp:3022 (loop :3034-3041). Callers of June fn: 323d4910, 323d4390 | YES |
| 5 | 327da29f | 327da190 | CX2DungeonManager::IsActiveDungeon( dungeonID, diff ) (thiscall; callers 322c30c0, 32350622, 324345c1, 324e4bfa, 324e5920, 324e5d6e) | DungeonData row **+0x1cc** (`m_iRequireCompleteQuestID`, parser 327e0f80 @327e4f01, default 0), `CMP [..+0x1cc],1; JL` | dungeon not enabled while `+0x1cc > 0` and quest not complete | X2DungeonManager.cpp:718 (function); :750-751 (ported, flag SERV_IRUHADEV_JUNE_DUNGEON_SUITABLE_LEVEL) | YES |
| 6 | 327da2ea | 327da190 | same function, second test | local-map templet **+0x70** of the dungeon's local map (`FUN_324f25f0(dungeon+0x10,0)` -> GetLocalMapTemplet 324f2360; only when `g_pData+0x268` is set) | dungeon not enabled while `+0x70 > 0` and quest not complete | X2DungeonManager.cpp:744-748 comment says it is not ported (March's templet has no field) | NO |
| 7 | 3226bb88 | 3226b6c0 | CX2GFXEventScene::End( bool ) (funcmap: GFx G0, confirmed) | hard-coded **0x2afd = 11005** (QT_EPIC, start NUI_HAGERS, March TQI_CHASE_THIEF X2PlayGuide.h:8) | when the finished scene is named `"Quest_11005"` + `"Start"` (strings @33486fa4 / @33486f9c, flag bVar2), code event 0x93a=2362 is on (instance map +0xa1c node byte 0x10 == 1), `this+0xd4 == 1`, and 11005 is NOT complete: clears `+0xd4` and shows an OK message box (DLG_UI_OKMsgBoxPlusNew, STR_ID 0xae74=44660) then 31e57100 | none (no GFx event scene in March; X2EventScene.cpp:1560 tests quest 11000, a different check) | NO - June-only |
| 8 | 31f1514f | 31f14e40 | CX2State::UIServerEventProc case 0x2db handler (31f2ab80 `case 0x2db` @31f2b7e9 -> 31f14e40). The packet is **unresolved**: June event numbering differs from March here (March id 0x2db = EGS_GOOD_JOB_26_REQ filler), and the payload's first int must equal 0x117 | hard-coded **0x67e8 = 26600** (QT_NORMAL, NUI_BILLBOARD, level 35); also 32322380(26600) = GetUnitQuest | after the notice path: if 26600 is not ongoing, not complete, and unit level 35..70 (0x23..0x46 @31f15167-31f1517b) shows a popup (STR_ID 0x238c=9100 + item text; 0xab63 / 0xab64 = 43875 / 43876 strings) | none (`grep 26600 X2Lib` empty) | NO - June-only |
| 9 | 31d79afb | 31d79ac0 | CX2Main::UnlockQuestMovies (my name): loops the June movie table g_pMain **+0x7f8..+0x7fc**, 0xc4-byte records | record **+0xc0** = `m_iQuestID` (parser 31d8cb90, funcmap C3t-1) | for each complete quest: 31d78bf0( movieId = rec+0x2c, 0 ) = add (id, flag) to g_pMain list +0x84c..+0x850 unless present (an "unlocked movie" list). Called only from 32336e80 (SetUnitQuest) @323372ff | none: March has no movie table (funcmap C3t-1; X2GUUser.cpp:39141 comment) | NO - June-only |
| 10 | 321738e3 | 32173120 | CX2UIInventory::CreateWarpDest (funcmap confirmed, DLG_Destination.lua) | LMI **0x2719 = 10009 (LMI_ELYSION)** templet `+0x70` (@321738b3 `PUSH 0x2719; call GetLocalMapTemplet`) | the Elysion rows of the warp list (Atlas Station 1008, 20009) are added only if level >= +0x18 and (`+0x70 == 0` or quest complete) | X2UIInventory.cpp:13996 (comment :14005 "LMI_ELYSION quest test omitted") | NO |
| 11 | 3225d745 | 3225d340 | CX2WorldMapUI::UpdateWorldMap (region controls loop; "WorldMap_Navi", Button_Mychar / PartyMember0..2 strings) | local-map templet **+0x70** (EDI/`iStack_50` templet from 324f2360 via 324f2890) | region control hidden (`vslot 0x68`(0)) when level < +0x18, or `+0x70 > 0` and not complete, or the event-flag `cStack_5d` | X2WorldMapUI.cpp:424; level gate :643-646 (`SetShow(false)`) | NO - level only |
| 12 | 3225dca3 | 3225d970 | CX2WorldMapUI::UpdateFieldMap (region buttons) | local-map templet **+0x70** | button enabled (`vslot 0x60`) iff level >= +0x18 and (`+0x70 <= 0` or complete) | X2WorldMapUI.cpp:1036; level gate :1099-1108 (`SetEnable`) | NO - level only |
| 13 | 323d44ca | 323d4390 | CX2UIQuestNew::UpdateNewQuestNotice (loops LMI 10000.. via 324f2360, builds the NPC->village map) | local-map templet **+0x70** (`*(iVar6+0x70)`) | the map's villages/houses/NPCs are included in the quest-notice candidate map only if (`+0x4c` clear-dungeon list empty) or (cleared && level >= +0x18 && (`+0x70 < 1` or complete)). Callers: 323e05f0, 323e15e0, 323e1710, 323e4aa0, 323e4cb0, 323e57e0, 323e5c10, 325091d0, 32509660, 32154680 | X2UIQuestNew.cpp:2825; local-map filter :2847-2862 (clear-dungeon + level only) | NO - quest test missing; rest same |
| 14 | 3289dd68 | 3289dd50 | CX2PlayGuide::StartGoEffectNewbie (my name; thiscall, then `JMP 3289d090` = CX2PlayGuide::StartGoEffect, already ported) | hard-coded **0x2afd = 11005** | "Go ->" particle (SubStageGoRight_UI, Go.ogg) only if 11005 not complete AND unit level < 3. Callers 323cee10, 324bb780, 32507840, 323c9fa0 (quest-UI / play-guide refreshers) | X2PlayGuide.cpp:1061 (StartGoEffect); callers X2PlayGuide.cpp:485-492 (`GetUnitCompleteQuest( TQI_CHASE_THIEF )`, else StartGoEffect), :524 | PARTLY - March has the 11005 test inside ResetNoviceGuide, no level<3 wrapper and none of the four June callers |
| 15 | 32462ffd | 324627f0 | CX2StateMenu::CreateWarpDest (funcmap confirmed, DLG_Destination_byButton.lua) | LMI **0x2719** templet `+0x70` (@32462fcd) | Elysion rows of the Cobo warp list (Atlas Station 1008 unless code event 2260, and 20009) | X2StateMenu.cpp:6230 (comment :6242 "quest test omitted") | NO |
| 16 | 32f16e4e | 32f16d50 | CX2InformerQuest::CheckHouseQuestInfo (June copy; 32f1... field-map/house icons "AcceptableQuestInfo", "CompleteQuestInfo", "AcceptableEventQuestInfo(BG)") | each id of the NPC's start-quest list (323357c0 = GetAvailableQuest-style list) | removes a completed quest from the acceptable list unless its templet type is 1 (QT_EVENT, March QT_EVENT = 1) | X2InformerQuest.cpp:70, test at :98-117 (GetUnitCompleteQuest, `m_eQuestType != QT_EVENT`) | YES |
| 17 | 31fbdea1 | 31fbde90 (not a Ghidra function; raw bytes) | CX2TutorialGuideManager::CheckToNeedTutorialGuide (Lua-bound: name decoded @31e3ae82, pointer pushed @31e3afe3 in the binder 31d95080) | hard-coded **0x2af8 = 11000** (QT_EPIC, NUI_LOW) | returns false if 11000 is complete; else false if another unit of the same account is level >= 10 (loop over g_pData+0x68 +0x94..+0x98, `+0xb4 xor >= 0xa`); true otherwise. The only Lua use: Benders_Actor `BENDERS_ACTOR_DYING_END_FUNC` (data037, 1 file): true -> `OnBeginGuideNo(101)` + invisible dummy NPC | none: no TutorialGuideManager in March (X2State.cpp:734 comment) | NO - June-only |

Not among the 17 but the same quest: SetUnitQuest 32336e80 @32336fxx inlines `find(0x2afd)` against `this+0x38` (sets `g_pData+0x338 (PlayGuide) +0x383`); it is the
only caller of 31d79ac0.

## Part B

### NEED_ONGOING_QUEST_ID and BATTLE_FIELD_DIRECTION_INFO
- Data: one file, one occurrence each. `june_src_fixed/data036_kom/BattleFieldData.src.lua:1642-1651`:
  `BATTLE_FIELD_DIRECTION_INFO = { { FORCE_MOVE_BATTLE_FIELD_START_POS = 32, NEED_ONGOING_QUEST_ID = 22218, DIRECTION_NPC_ID = { NUI_ELSION_EVENT_00, NUI_ELSION_EVENT_01 } } }`
  inside the 43rd `g_pBattleFieldManager:AddBattleFieldData({...})` (the Lanox F04 row). Count of June files setting the key: **1**.
- June client reader: **none found (unresolved as a reader, strong negative evidence).**
  - None of `NEED_ONGOING_QUEST_ID`, `BATTLE_FIELD_DIRECTION_INFO`, `FORCE_MOVE_BATTLE_FIELD_START_POS`, `DIRECTION_NPC_ID` is in the decoded
    XOR table (xorstrings.tsv, 6,180 rows `ok`, 61 `orphan/noinit`, none of which has a matching length), and none is a plain string (Ghidra `strings` filters empty).
  - The June BattleField loader chain is 32482ac0 (g_pBattleFieldManager, file read) -> 32485a80 (calls 324862d0 alloc, 32483f70 parse, 32486570 insert).
    32483f70 reads exactly 12 XOR keys (reg 337a3788 bits 0x1-0x800): BATTLE_FIELD_ID, WORLD_ID, RETURN_VILLAGE_ID, STANDARD_MONSTER_LEVEL,
    MAX_NUMBER_OF_MONSTER_IN_THIS_BATTLE_FIELD, BATTLE_FIELD_NAME, BATTLE_FIELD_TEXTURE_NAME / _KEY, BATTLE_FIELD_TYPE,
    RAID_FIELD_PORTAL_POSITION_INDEX, BATTLE_FIELD_LOADING_TEXTURE_NAME_L / _R, then plain-string tables via 32483350 (BATTLE_FIELD_PORTAL_MOVING_INFO),
    32483450 (BATTLE_FIELD_RISK_INFO), 32483e50 (BATTLE_FIELD_MIDDLE_BOSS_INFO). Nothing reads a DIRECTION table. The other top-level keys in the data
    (NPC_TABLE_FILE_NAME, LINE_MAP_FILE_NAME, REQUIRE_LEVEL, MAX_JOIN_USER_COUNT, ENTER_VILLAGE_MAP_ID, ADD_EXP_BY_NPC_KILL) are also client-unread = server keys.
  - So the key is a server-side key (the NPCs NUI_ELSION_EVENT_00/01 are server-spawned). No stored field, no client reader.
- March: not parsed by X2Lib (X2BattleFieldManager.cpp reads PORTAL_MOVING_INFO :183, RISK_INFO :224 only); the block sits verbatim and inert in
  kom_contents data036 `BattleFieldData.lua:2016-2024` under `-- quest plan:` comments. KncWX2Server (March server code) does not read it either.
  Reader for Q6: needs a new offline-server (emulator) behaviour, not a client port; the quest id 22218 is QT_EPIC, NUI_EDEL, Atlas+Lanox.

### REQUIRE_COMPLETE_QUEST_ID and REQUIRE_COMPLETE_SUB_QUEST_ID (MapData)
- Data: `june_src_fixed/data036_kom/MapData.src.lua:344-345` and `:358-359` (2 rows each key): VMI_BATTLE_FIELD_ELYSION_REST_00 and VMI_ELYSION, both
  `REQUIRE_COMPLETE_QUEST_ID = 22218`, `REQUIRE_COMPLETE_SUB_QUEST_ID = 22268`. 1 file, 2 uses of each key. (22268 is not a quest in
  q_june_quest_census.tsv; it is a sub-quest id.) March `KncWX2Server/ServerResource/US/MapData.lua` already carries the same 4 lines.
- June client reader: **none.** The June client loads MapData.lua only to fetch two Lua functions (loader 31ee6880, `"MapData.lua"` @31ee6926, then
  `CheckCOBOExpressTicketMapID` @3345b3b8 and `ComputeCOBOExpressTicketCost` @3345b3d4 resolved at @31ee6f83 / @31ee6f93); no `AddMapData` binding exists
  (neither in xorstrings nor plain strings), so the `MapData:AddMapData{...}` rows (keys ID/BEFORE/AFTER/REQUIRE_LEVEL/REQUIRE_DUNGEON/REQUIRE_COMPLETE_*) are never parsed
  by the client. The reader lived on the June server (CXSLMapData, not in this tree).
- March: X2Lib parses nothing of MapData; the offline emulator's `CX2OfflineMapData::AddMapData_LUA` is a no-op (X2Lib/Offline/X2OfflineMapData.cpp:155-157), same as June's client.
  March server code `KncWX2Server/Common/X2Data/XSLMapData.cpp:55-118` reads ID/BEFORE/AFTER/REQUIRE_LEVEL/REQUIRE_DUNGEON/DEFAULT only. Not ported; the only possible
  reader here is an emulator gate (Handlers_Field EGS_STATE_CHANGE_FIELD_REQ), which is a design decision for Q6, not a June port.

### RequireCompletedQuestID (local-map templet) and its parser 32501bb0
- 32501bb0 = **CX2LocationManager::OpenScriptFile** (`"CX2LocationManager_Local_OpenScr"` @334a1300; tables Local / VillageData / VillageDGData / VillageDungeonLoungeData /
  BattleFieldRestData / SpaData / DungeonIDList / BattleFieldIDList / EnterCondition). Inside `EnterCondition`: `RequireClearDungeonID` (vector), then
  `RequireUnitLevel` (XOR @325029e4) -> templet **+0x18** (default 1) and `RequireCompletedQuestID` (XOR @32502b6a, reg 337a3fe8 bit 0x40) -> templet **+0x70** (default 0);
  `POS_NEW_DUNGEON_DATA` -> +0x2c (default -1).
- Readers (all through 32322260): 3293c6f0, 3225d340, 3225d970, 32173120, 324627f0, 323d4390, 327da190 (table rows 1, 11, 12, 10, 15, 13, 6).
- March: **key not parsed.** X2LocationManager.cpp:285-308 reads RequireClearDungeonID and RequireUnitLevel only; LocalMapTemplet has `m_RequireUnitLevel`
  (X2LocationManager.h:496) and no quest field. Data: **1 June file**, `june_src_fixed/data217_kom/DLG_Map_Local_Elysion.src.lua:6` (`RequireCompletedQuestID = 22218`,
  RequireUnitLevel 78); no other file sets it.

### m_iRequireCompleteQuestID in 327e0f80
- 327e0f80 = **CX2DungeonManager::AddDungeonData_LUA** (47 XOR keys; bound @31dc2f53 in 31d95080). `m_iRequireCompleteQuestID` (XOR @327e4f01, reg 337aa8c0 bit 0x80) -> DungeonData **+0x1cc**,
  default 0 (beside `m_SuitableLevelFixValue` -> +0xa8, default 2). Reader: 327da190 site 5 (table row 5).
- March: **parsed and read.** X2DungeonManager.cpp:241 (`LUA_GET_VALUE ... m_iRequireCompleteQuestID, 0`), X2Dungeon.h:208, test at X2DungeonManager.cpp:750-751 (flag
  SERV_IRUHADEV_JUNE_DUNGEON_SUITABLE_LEVEL). Data: **1 June file**, `june_src_fixed/data035_kom/DungeonData.src.lua:29939`
  (DI_ATLASDEVELOPMENT_NORMAL, 22218, min level 80). kom_contents data035 `DungeonData.lua:16788-16791` has it as a `-- quest plan:` comment (inert) - only the data edit remains.

### CONDITION_GOING_QUEST_ID / CONDITION_NEW_QUEST_ID / CONDITION_COMPLETE_QUEST_ID (31fbe4a0)
- 31fbe4a0 = **CX2TutorialGuideManager::LoadTutorialGuideInfo** (parser of `"TutorialGuideInfo.lua"` XOR @31fbe584; called only from 31fc3c20 @31fc3c45, itself from
  the boot loader 3259bb20). ~50 keys per guide row (row = 0x27 dwords): GUIDE_NO, WAIT, CLICK_WAIT, CONDITION_STATE, CONDITION_GOING_QUEST_ID @31fbf1a4 (reg 3379cb38 bit 0x20),
  CONDITION_NEW_QUEST_ID @31fbf3a3 (0x40), CONDITION_COMPLETE_QUEST_ID @31fbf591 (0x80), CONDITION_FIELD_ID, CONDITION_FAILED_TO_ENTER_VILLAGE, FIRST_GUIDE, ... NEXT_GUIDE_NO, REPLAY_GUIDE(_NO).
  Each defaults to 0 when absent (`FUN_32fe9fd0` fail -> 0).
- What it gates: the tutorial-guide step (what quest state starts a guide). Data `june_src_fixed/data036_kom/TutorialGuideInfo.src.lua` (1 file): GOING x4, NEW x4, COMPLETE x1, values from
  `CONDITION_QUEST_ID.CQI_CHASE_THIEF / CQI_SAVE_EL / CQI_VISIT_RUIN_OF_ELF / CQI_HOLY_ANIMAL`.
- **Evaluator unresolved.** No function in the 128 manager functions 31fba000-31fc8000 touches the quest manager except 31fbde90 (row 17) and none compares these three fields. Guide starts are driven from
  the quest-UI handlers: 323e05f0 / 323e08c0 / 323e4cb0 call manager 31fc5920( id ) (find row whose dword +0x14 == id, then 31fc4530 start) right after new/complete-quest events - these are the likely feeders.
- March: no equivalent (no CX2TutorialGuideManager; X2State.cpp:734 comment; X2NoviceGuide.cpp / X2PlayGuide.cpp are the March guides and hard-code 11000 / 11005 / 11010).

## Plan-relevant summary
- Quest-id sources: constants 11000 (row 17), 11005 (rows 7, 14), 26600 (row 8); data keys local-map templet +0x70 (rows 1, 6, 10-13, 15), DungeonData +0x1cc (row 5), movie record +0xc0 (row 9),
  templet before-quest list +0xf0 (row 4); loop variable (rows 2, 3, 16).
- Same in March: rows 2, 4, 5, 16. Different or missing: rows 1, 6, 10-13, 15 (templet +0x70 never parsed), 3, 7, 8, 9, 17 (June-only features), 14 (partial).
- NEED_ONGOING_QUEST_ID / BATTLE_FIELD_DIRECTION_INFO and REQUIRE_COMPLETE_*: no June client reader found; server-side keys.
