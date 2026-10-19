# June quest system: engine, data, offline completion, quest UI, gates, random mission

Plan key `quest-system` · branch `main` · backlog Stage 4 (`JUNE_PORT_BACKLOG_ORDER.md`) · written 2026-10-19 (Opus 5.5 high,
planning session from `kickoffs/june-backlog/stage4-plan.md`). Log tag `[JUNE_QUEST]` · flags `SERV_IRUHADEV_JUNE_QUEST_*`
(+ `_DEBUG`) and `SERV_IRUHADEV_JUNE_RANDOM_MISSION` · kickoffs `kickoffs/quest-system/`.

## Context

This is the "later quest plan" that Lanox, Atlas and Elysion deferred to. Its job is to bring March's quest system to June's
whole: the manager and its data, the offline emulator's completion of every sub-quest type, the quest UI, and the quest-gated
content that three regions left inert. Transcendence (Stage 6) is quest-gated, so this plan comes first.

**Decisions already made (user, 2026-10-17):**
- **Guild quests: parse and log.** `QT_GUILD`, `AddGuildQuestTemplet_LUA` and `GuildCoopQuestRewardTable` load so the scripts parse;
  guild quests are skipped with a log line. There is no guild stand-in in the emulator.
- **Scaleform is a runtime, in its own plan** (`june-gfx`). Its G3 ports `CX2GFXEventScene` and `PlayEventScene`. The quest-UI
  callers of the scene (`323cb2e0`, called from `323e1bf0`, `323e4cb0`, `323e57e0`, `323e5c10`, `31d896fb`) belong to this plan, built on G3.
- Atlas decision 6 half-stands: "saves that already cleared Lanox get the Lanox epic chain 22160-22217 marked complete (backup + ask)".
  Its GFx half was superseded by `june-gfx`.

**What the planning survey found (2026-10-19).** Evidence is in `june2017_re/quest_plan/`: `q_june_quest_census.tsv` (one row per June quest),
`q_june_quest_keys.tsv`, `q_june_quest_types.tsv`, `q_parser_keys.txt`, `q_xor_quest.tsv`, `q_rm_callsites.tsv`, `q_funcmap_quest_rows.tsv`
and the parser/walker `qlib.py`.
- **Data.**
  - June's `FieldQuest` has 1,633 quests: 1,203 in common with March, 430 June-only and 257 March-only. June dropped or re-IDed those 257.
  - `FieldSubQuest` has 3,219 sub-quests: 1,733 in common, 1,486 June-only.
  - 883 common quests and 874 common sub-quests differ in a non-text key. 344 sub-quests lost `m_eDungeonID` and became difficulty-only (`m_cDifficulty`, 824 uses).
  - New June files: `GuildQuest` (15), `AccountQuest` (5; March's is empty), `ChainQuestInfo`, `GuildCoopQuestRewardTable`, `Quest_Event_Scene_Info` (268 rows).
  - The `_LA` files are the Latin-American Spanish text. The US client uses the plain files.
  - Lanox, Atlas and Elysion hold at least 136 quests, all June-only.
- **Types March lacks.**
  - `QT_GUILD` 8, `QT_CHANGE_TRANSCENDENTAL_CLASS` 9, `QT_STORY` 10 (91 quests). March's `QT_END` is 8, which collides with June's `QT_GUILD`.
  - `SQT_PVE_WIN` 37, `GUILD_JOIN` 38, `GUILD_HONOR_POINT_COLLECTION` 39, `ITEM_EXCHANGE` 43, `TOGETHER_SUBQUEST_CLEAR` 44, `PVP_RANK` 45,
    `PLAY_TOGETHER_WITH_TITLE` 47 and `HAS_EXP_DUNGEON_CLEAR` 48: 36 sub-quests in all.
  - `QRT_WEEK` 3 (32 quests), `QRT_MONTH` 4, `QRT_CHAIN_REPEAT` 5, `CJT_JOB_FOURTH` 6, and the whole `QUEST_REWARD_TRAN_CLASS` table (`QR_NONE` 1000,
    `QR_TRANSCENDENTAL_CLASS` 1001).
  - The NPCs `NUI_STORY_QUEST` 10142 (155 uses) and `NUI_TRANSCEND_QUEST` 10153.
  - New PvP enum members.
  - Every name the two sides share has the same value. March's core `Enum.lua` lacks all of the above.
  - March's `AddSubQuestTemplet_LUA` returns false on an unknown SQT, so the sub-quest drops with only an `XEM_ERROR80`.
- **Keys March does not read.**
  - Quest level: `m_iChainID`, `m_vecAdditionalSceneQuestID`, `m_fSelectBGM`, `m_bIsAccount`, `m_bIsGuild`, `m_bIsCommon`, `Reward.m_iGuildCoopQuestRewardID`.
  - Clear condition: `m_bExpDungeonAndField` (36), `m_eExceptionDungeonID`, `m_ePvpRank`, `m_ePvpTournamentType`, `m_bIsGuildJoin`,
    `m_iCollectionHonorPointCount`, `m_iExchangeCount`, `m_vecExchangeSourceItem_ID`.
  - Binders: `AddChainQuestDays` (June `32322b10`) and `AddGroupChainQuest`. The second name is in neither the XOR table nor the plain strings: Q0a settles it.
- **June binary.**
  - Registration is in `31d95080`. `AddQuestTemplet` / `AddAccountQuestTemplet` / `AddGuildQuestTemplet` all push `323321f0` (push-scan; Q0a confirms).
  - The rest: sub-quest `32330ff0` -> clear condition `32323ad0`; condition `32323120`; reward `32330250`.
  - The loader is `3258c470`. It opens FieldQuest, FieldSubQuest, GuildQuest, AccountQuest, the `_LA` pair, AccountQuestDate, ChainQuestInfo,
    ChainCashItem and the relation data. The scene-info loader is `31eeb040`.
  - `IsCompleteQuest` is `32322260` (17 callers).
  - Quest-UI dispatcher `323e5fb0` cases: `0xeb`, `0xec` accept `323e4cb0`, `0xf1`, `0xf5`, `0xf8`, `0xfa`, `0x27b`-`0x27d`, `0x593`, `0x604`, `0x605`.
- **March today.**
  - `X2QuestManager` (5,772 lines) runs under `REFORM_QUEST` / `SERV_EPIC_QUEST`. The live window is `CX2UIQuestNew` (7,254 lines).
  - The offline module (`X2OfflineQuest`, `Handlers_Quest.cpp`) drives 21 SQT types and labels the PvP / pet / resolve / attrib ones undriven.
  - Save schema is v20.
  - The relation system (Elysion E1) is ported and dormant, waiting for quest data.
  - `IsActiveDungeon` already tests `m_iRequireCompleteQuestID` (`SERV_IRUHADEV_JUNE_DUNGEON_SUITABLE_LEVEL`). That is why AD Normal's key 22218 is commented out.
- **The user's save** (2026-10-19): 64 quests in progress and 95 completed, 58 quest ids in all. **26 of those 58 ids are not in June's
  `FieldQuest`**: 7600, 8940-8961, 9050, 10234, 10971/2, 11040, 11060, 12005, 12030, 12050, 12080, 12090, 14005, 18020, 21910, 26000-26020,
  26190, 60700. Several are early epics that June re-IDed. The old-save migration (Q2) is real work.
- **Random mission** (carried in from skill S3c). A singleton at `337a66b4` is set only by server event `0x492` (`31f2ab80` case -> `31efe2a0` ->
  `326798b0`). It counts 12 event types at 41 call sites and sends `0x493` / `0x494` on completion. No June Lua defines missions, so no data exists
  to serve offline. RM0 maps it, and the user decides on a source.

## Rules

- Standing rules: `JUNE_PORT_BACKLOG_ORDER.md` "Standing rules (every stage, every kickoff)". June Lua is read from `june_src_fixed/` only.
  Cite June addresses. No partial June updates. June wins, even when it removes a feature, and **this includes the 257 March-only quests**.
  June bugs are kept verbatim with a `JUNE_BUGS.md` row.
- **US data = the plain files.** Never adopt the `_LA` files. If June's loader picks `_LA` by country, port the US branch.
- **Quest Lua comes from June's bytecode.** Use `luasrc.py adopt-june` (`ljconv`). A quest file becomes BOM source only if a port must edit it
  (core-list row, `--source`). `Enum.lua` and `NPCTemplet.lua` are already core-list source: edit them as text and `luasrc.py refresh` them.
- **Every sub-quest type either completes offline or is labelled undriven**, with a reason in `IsClearTypeUndriven`. Guild and PvP types are
  undriven (decision above; the client has no PvP offline). A type June's server drove but the March server lacks gets a stand-in only under
  its own flag, proposed to the user first ([[design-missing-june-pieces]]).
- **Transcendence rewards are Stage 6's.** `QT_CHANGE_TRANSCENDENTAL_CLASS` quests and `QR_*` parse and show. A reward that would change the
  class to a transcendental one is refused with a log line until Stage 6 owns it.
- **Saves.** Snapshot to `db_backup/` before any migration. Rungs are forward-only (v21 and on). The migration's choices are the user's (Q2).
- **Shared structs.** If a quest packet's layout changes (Q0b says), the flag also goes in `ServerDefine.h` (`SECOND_HOMES`) and
  `X2ServerProtocol_2010.vcxproj` `X2TOOL|Win32`. Prove it with `cl /P` from both projects.
- **One code phase at a time per file family.** `X2QuestManager.*`: Q1, Q3, Q6. `X2UIQuestNew.*`: Q5a, Q5b. `X2Lib/Offline/*`: Q2, Q4, RM1.
  Families may run side by side.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "<heading>"` |
| Planning evidence | `june2017_re/quest_plan/` (tsvs above; `qlib.py` re-runs the census: June parser + March bytecode walker) |
| June quest Lua | `june_src_fixed/data036_kom/{FieldQuest,FieldSubQuest,AccountQuest,GuildQuest,ChainQuestInfo,GuildCoopQuestRewardTable,Quest_Event_Scene_Info}.src.lua` |
| March quest Lua | `24965799\kom_contents\data036_kom\` (bytecode), UI `data034_kom\DLG_UI_NEW_Quest_*.lua` |
| Ghidra | `ghidra` skill. Read-only in Q0a / Q0b / RM0 and in reviews |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june`), core list `luasrc.py core --check` |
| Pack / copy | `/elstools pack dataNNN`, then `/x2-kom-packed copy dataNNN` (ask first) |
| Save | `/x2-db` (`tools/elsdb.py`); snapshot before any migration |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `build.py --deploy` (background), `logs.py --tag JUNE_QUEST` |
| Dev console | auth level 3 (`python tools/elsdb.py setting auth_level 3`), END key |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| Q0a | Engine and data map (read-only) | Opus high | - |
| Q0b | Quest UI, packet and movie map (read-only) | Opus high | - |
| RM0 | Random mission map + source proposal (read-only) | Opus high | - |
| Q1 | Enums, loader keys, new binders, new June quest files | Sonnet high, Opus review | Q0a |
| Q2 | Data flip (June FieldQuest / FieldSubQuest / AccountQuest) + old-save migration | Opus high, review | Q1 review |
| Q3 | Quest manager runtime to June | Sonnet high, Opus review | Q2 review |
| Q4 | Offline completion of every sub-quest type | Opus high, review | Q2 review |
| Q5a | Quest UI: window, tabs, list / detail / quick, accept / complete / give-up | Sonnet high, Opus review | Q0b, Q2 review |
| Q5b | Quest UI: scenes, clear window, scene info | Sonnet high, Opus review | Q5a review, june-gfx G3 review |
| Q6 | Quest gates: the inert keys and their readers | Sonnet high, Opus review | Q3 review |
| Q7 | Quest NPCs and houses | Sonnet high | Q2 review |
| Q8 | Scene data and clear movies | Opus high, review | Q0b, Q5b review |
| RM1 | Random mission port | Sonnet high, Opus review | RM0 + user answer, Q4 review |
| Q9 | Region walk: Lanox -> Atlas -> Elysion chain | Sonnet medium, user play-test | Q3-Q8 |
| Z | Close-out | Sonnet medium | all |

Q0a, Q0b and RM0 run now, alongside anything.

## Phase Q0a - Engine and data map

**Run with:** Opus 5.5 high. Read-only: no code, no flag, no build. Delegate the reads ([[opus-phases-delegate-mundane-work]]).

- **Templets.** June's `QuestTemplet` / `SubQuestTemplet` / condition / reward layout comes from `323321f0`, `32323120`, `32330250`, `32330ff0` and
  `32323ad0`. The decompile of `32323ad0` fails: read its disassembly.
  - For every key in `q_june_quest_keys.tsv`, give the March member or "new", and its semantics. Settle these keys:
    - quest level: `m_iChainID`, `m_vecAdditionalSceneQuestID`, `m_fSelectBGM`, `m_bIsAccount`, `m_bIsGuild`, `m_bIsCommon`;
    - clear condition: `m_bExpDungeonAndField`, `m_eExceptionDungeonID`, `m_ePvpRank`, `m_iExchangeCount`, `m_vecExchangeSourceItem_ID`.
  - What a `QR_*` value means at the end of `m_eUnitClass` and in `m_eChangeUnitClass`.
  - **What a sub-quest with `m_cDifficulty` and no `m_eDungeonID` means** (344 sub-quests). Compare it with what March's parser and emulator do today.
- **Binders.** Confirm that `AddAccountQuestTemplet` and `AddGuildQuestTemplet` share `323321f0` (`31daf0c0` / `31daf322`). Find out whether `AddGroupChainQuest`
  is bound at all. If it is not, June's `ChainQuestInfo` errors at its 6th call: that is a `JUNE_BUGS` row, kept.
  Decode `AddChainQuestDays` `32322b10` and the chain day rule it feeds.
- **Loader `3258c470`.** Give the file order, the `_LA` country switch and its US branch, `ChainCashItem.lua`, and the scene-info loader `31eeb040` (store, readers).
- **Runtime.** Map June's `CX2QuestManager` functions to March's:
  - the 17 callers of `IsCompleteQuest` `32322260`;
  - completion checks per SQT;
  - the progress-string builder;
  - the chain / story / week / month logic;
  - `GetBoardStartQuestList` `3232fd70`;
  - anything June added.
  Give each a verdict and a target phase.
- **The missing reader.** Find the `NEED_ONGOING_QUEST_ID` / `BATTLE_FIELD_DIRECTION_INFO` reader, which A1a did not search for. Also the
  `MapData` reader of `REQUIRE_COMPLETE_QUEST_ID` / `REQUIRE_COMPLETE_SUB_QUEST_ID`.
- **Output.**
  - `june2017_re/quest_plan/q0a_engine_map.tsv` (june_va, june_name, march_equivalent, verdict, target, evidence).
  - funcmap rows `Quest Q0a`.
  - Amendments to the Q1 / Q3 / Q4 / Q6 kickoffs.
  - A Done note.

**Done gate:** every key and type in the census tsvs and every function above has a row with a target phase or "June-only, not ported"
plus a reason.

### Q0a - Done (2026-10-19)

Read-only: no code, no flag, no build, no pack. Opus judged the results; Sonnet subagents read and dumped. I spot-checked every claim that
decides a port in Ghidra.
- **Map.** `june2017_re/quest_plan/q0a_engine_map.tsv`, built by `q0a_build.py`.
  - 551 rows: every key of `q_june_quest_keys.tsv`, every type of `q_june_quest_types.tsv`, and 82 function / binder / loader / reader rows.
  - 0 unmapped. `-` means same as March, no work.
  - Evidence:
    - `q0a_june_templets.md` and `q0a_june_clearcond.md`: per-key offsets;
    - `q0a_qm_functions.tsv`: all 244 functions in 32318000-3234a000;
    - `q0a_qm_summary.md`, `q0a_june_callers.md` and `q0a_march_digest.md`.
  - 26 funcmap rows `Quest Q0a`, and kickoff amendments in Q1 / Q3 / Q4 / Q6.
- **Binders.** Account, guild and normal quests share `323321f0`: the three pushes are identical, and only the stored `m_bIsAccount` / `m_bIsGuild`
  differ. `AddChainQuestDays` `32322b10` is the last quest binding. **`AddGroupChainQuest` is unbound** (JUNE_BUGS 320, kept).
- **Loader.**
  - `ResetQuestManager` `31edbf70` loads Field, Sub, Account, Guild, then AccountQuestDate (empty in June), ChainQuestInfo, and scene info `31eeb040`.
  - `_LA` = `argv[1] == "Axeso5"`; the US branch uses the plain files. `ChainCashItem.lua` is cash-shop data (Stage 16).
- **Keys.**
  - `m_fSelectBGM` is a wstring.
  - `QR_*` in `m_eUnitClass` -> its own scalar (`v % 1000`).
  - `m_eChangeUnitClass` splits on QT 9.
  - `m_eDungeonID` keeps 0.
  - `m_cDifficulty` is stored (-1 default) with no reader found.
  - Server-only keys, unknown to June's client: `m_bExpDungeonAndField`, `m_eExceptionDungeonID`, `m_ePublisher`, `m_fRandomRate`, `m_iFairDungeonID`,
    `m_vecAcceptRewardItem`, the MapData `REQUIRE_COMPLETE_*` pair and BattleFieldData `NEED_ONGOING_QUEST_ID` / `BATTLE_FIELD_DIRECTION_INFO`.
- **Correction to Context.** The "344 difficulty-only sub-quests" are March's `m_eDungeonID = {0}`, which June deleted. Both loaders give an empty set
  ("any dungeon"). Only 35373 and 74520 carry `m_cDifficulty` alone. `m_cDifficulty` disagrees with the id's last digit in 123 sub-quests.
- **Enum.**
  - Collisions: `QT_END` 8, `SQT_END` 37 and `CJT_JOB_END` 6 against June's new values.
  - `NUI_EDEL` differs: June 10067, March 10072; June uses 10072 for `NUI_STEEL_AND_STEING`.
  - `DL_HELL` / `DL_NOMAL` are nil in June too (JUNE_BUGS 322).
- **June bug found.** `SQT_ITEM_EXCHANGE`'s source-item loop is inverted, `@3232c0cf` (JUNE_BUGS 321).
- **Runtime.**
  - Chain rule `3232caf0`: local midnight of (last completion + AllowFewDays) <= now, with the TestMinute variant under enable-code 0x10.
  - Week: next Wednesday 00:00 (`32322050`). Month: the 1st, 00:00 (`32321d10`). Both run inside `SetUnitQuest` only.
  - `QRT_CHAIN_REPEAT` has no code. `QT_STORY` is indexed at `+0xec`.
- **`IsCompleteQuest` `32322260`** = March `GetUnitCompleteQuest`. Of its 17 callers, 4 are as in March, 7 are local-map `+0x70` gates March lacks (Q6),
  and 6 are June-only (Q5a / Q5b / Q8 / tutorial guide, out of plan; `31f14e40`'s packet goes to Q0b).

**Open for the user (each raised in its own phase, not now):**
- Q4/Q6: an emulator stand-in for each server-only gate above.
- Q7: whether `NUI_EDEL` moves to June's 10067.

## Phase Q0b - Quest UI, packet and movie map

**Run with:** Opus 5.5 high. Read-only.

- **`CX2UIQuestNew`, June vs March.** Use the candidate list in `323c8da0`-`323e8000` (planning digest in `q_funcmap_quest_rows.tsv` and this
  section's kickoff): builder `323d0ca0`, clear window `323d2c60`, list/detail/reward `323e1bf0`, accept `323e4cb0`, the handlers, Story / Guild /
  Request / Scenario / Event tabs, quick quest, `Button_Skip_Epic_Quest` (`323ccbc0`, request `322f4c60`), `GetSubQuestFail` `323cdfe0`. Give a verdict per
  function: port whole, already March, or June-only.
- **Packets.**
  - Map the dispatcher `323e5fb0` cases to June event names, then to March `EGS_*`. Name the June-only ones (`0x27b`-`0x27d`, `0x593`, `0x604` / `0x605`)
    and decide for each whether the emulator answers it.
  - Diff the payload layouts (`KEGS_NEW_QUEST_*`, `KQuestInstance`, complete ACK) and list the shared-struct changes.
- **DLG files.** Is March's `data034` `DLG_UI_NEW_Quest_*` already June's (`ljconv`) or March's? Plan their adoption with Q5a.
- **Scenes.**
  - `323cb2e0` and its five callers.
  - How a per-class `Quest_Event_Scene_Info` row is chosen over `m_wstrStartScene` / `m_wstrEndScene`.
  - The Lua `CX2EventScene` fallback.
  - `m_vecAdditionalSceneQuestID`.
- **Clear movies and scene data (for Q8).**
  - The movie table (`g_pMain +0x7f8`, parser `31d8cb90`, key `m_iQuestID`), `DungeonClearMovieTemplet` / `AddDungeonClearMovieTemplet_LUA`.
  - The player (`+0x80a`; G0 called it the AVI movie). Does March have an AVI player?
  - The coordination with june-gfx G5, which owes the `+0x80a` hide term.
  - Is June's `EventScene.lua` (2.6 MB against March's 1.07 MB) parse-compatible with March's `CX2EventScene`?
- **Other quest UI readers.** Bulletin board, `GetNPCFaceTextureKey`, world-map `GetNPCQuestDesc`, `CX2InformerQuest`, the field-NPC quest button
  (JUNE_BUGS 221: what is ported already), and the quick-quest toggle the buff plan left to Stage 9 (`JUNE_BUFF_PORT_PHASES.md:4578-4593`).
  Which are this plan's and which are Stage 9's?
- **Output.** `june2017_re/quest_plan/q0b_ui_map.tsv`, funcmap rows `Quest Q0b`, amendments to Q5a / Q5b / Q8, and a Done note.

**Done gate:** every dispatcher case, every function in the candidate list, and every quest DLG file has a row with a verdict and a target.

### Q0b - Done (2026-10-19, Opus 5.5 high; read-only: no code, no flag, no build, no pack)

**Output:** `june2017_re/quest_plan/q0b_ui_map.tsv` (71 rows, kinds case / send / func / dlg / data / player / reader / struct), 30 funcmap rows
`(Quest Q0b)`, amendments ("## Amended by Q0b") in `kickoffs/quest-system/Q4.md`, `Q5a.md`, `Q5b.md`, `Q8.md`. Raw dumps stayed in the session scratchpad.

**Event ids.** March's ids came from a `cl /P` preprocess of `X2Lib/stdafx.h` (US_SERVICE defines), every one of the 1,331 checked by a C2440
probe, and 119 `offline_packets.log` anchors agree. June has no event-name table; in the quest block June = March - 9. Proof: every handler
deletes its own id, and the senders pair up (0xea/0xeb, 0xf0/0xf1, 0xf4/0xf5, 0xf9/0xfa).

**Dispatcher `323e5fb0`, 12 cases** (March `UIServerEventProc` has 7):
- Already March: 0xeb NEW_QUEST_ACK.
- Port whole (Q5a): 0xec NEW_QUEST_NOT, 0xf1 QUEST_COMPLETE_ACK, 0xf5 ALL_COMPLETED ACK, 0xf8 GIVE_UP ACK, 0xfa TALK_WITH_NPC ACK, and
  0x27c, which is June's UPDATE_QUEST_NOT moved here from `CX2State` (`X2State.cpp:4776`).
- June-only server pushes (Q5a ports the bodies; Q4 decides whether the emulator sends them): 0x27b quest instance, 0x27d completion,
  0x593 batch, 0x605 id set.
- Dead: 0x604. Its request 0x603 has no reference in the image, and it deletes 0xf1's wait (a June quirk, unreachable).
- None of the June-only ids has a sender in June's client: they came from June's server.

**Corrections to earlier work:**
- `X2UIQuestNew.cpp:2032` (Atlas E1c) cites `323e1710` for ALL_COMPLETED. That is the 0x593 handler. June's ALL_COMPLETED `323e5c10` has no
  relation refresh. Routed to Q5a.
- The kickoff's skip-epic request `322f4c60` is the dungeon-start ESC skip (0x4ac). `Button_Skip_Epic_Quest` -> custom message 41045 ->
  OK 41046 -> `323d24f0`, a plain QUEST_COMPLETE_REQ.
- The range `323e3e8d`-`323e4cb0` is the tail of `UICustomEventProc` `323e1bf0` (case bodies), not separate functions.
  `3214f7c0` is not code: there is no function at that address.

**Functions:** every candidate has a verdict row. Already March: `323c8da0`, `323cee10` (unless Q5a's diff finds a change). Port whole (Q5a):
the ctor, the buttons, mode/tab, list, detail, notice, UICustomEventProc and the request senders. June-only: scenario-movie tab
`323ca6d0` / `323dfc00` -> Q8. Clear window `323d2c60` -> Q5b. June-only custom messages 41038-41047 need the C++ enum and `Enum.lua`.

**Scenes (Q5b).**
- `323cb2e0( quest, 0/1/2 )` reads `Quest_Event_Scene_Info` (`323cb190`, map `g_pData+0x498`). The unit's class sub-row wins over `DEFAULT_*`.
  Names go to GFx `3226ac80` / `3226b130`. End also plays every `m_vecAdditionalSceneQuestID`.
- **No Lua fallback found:** neither function calls the Lua scene, and no `GetQuestTemplet` caller reads templet `+0x1d8` / `+0x1f4`
  (`m_wstrStartScene` / `m_wstrEndScene`).
  So 780 of June's 968 Lua-scene quests play no scene in June. Q5b's kickoff re-proves this by a whole-image sweep before following June.
  It also replaces the done gate's "a Lua-scene quest still plays its scene".

**Movies (Q8).**
- Table: parser `31d8cb90` -> `g_pMain+0x7f8`. Three triggers: by quest `31d7a240` (record `+0xc0`, called first by the complete REQs, request
  deferred to `31d822b0`), by dungeon first clear `31d7a150`, by id from the scenario tab `31d884f0`.
- March has a player: Bandi `CMovieData`, with June's subtitles already ported. June's host DLG `DLG_Dungeon_Clear_Movie` is June-only.
- G5 leaves the `+0x80a` hide term to Q8. 13 `.avi` files are missing from March's `data/movie/`.

**DLGs.** None of March's quest DLGs was ever staged from June (all are March bytecode).
- Q5a adopts List / Detail / Quick, plus the dead-reader `DLG_UI_Quest*` / `DLG_UI_Request` and the guide DLGs (same-filename rule).
- Q5b adopts Clear; Q8 adopts `DLG_Dungeon_Clear_Movie`.
- The datai014 event-quest UIs are Stage 9's.

**EventScene.lua** is parse-compatible (same two binders). March does not read 2 June keys (`*_DUO_TYPE`): Q8 adopts it.

**Shared-struct changes** (Q4 / Q5a; `CommonPacket.h` -> `ServerDefine.h` second home):
- `KSubQuestInstance` clear data UCHAR -> short (0x27c reads a short).
- An int64 is added to `KQuestInstance`, `KCompleteQuestInfo` and `KQuestAck`.
- NEW_QUEST_NOT gains +2 int32 and the complete REQ +1 bool.
- One exe serves both ends, so each field lands only where a ported reader needs it.

**Other readers:**
- This plan: `GetBoardStartQuestList` (Q3), `GetNPCFaceTextureKey` (Q5a).
- Stage 9: `GetNPCQuestDesc` (world map), `CX2InformerQuest`, and the quick-quest toggle body (user, 2026-10-07).
- Already ported: the field-NPC quest button (Elysion E1, JUNE_BUGS 221).

**Open for the user:** none to answer now. Two notes. June drops 780 quests' Lua scenes (Q5b proves it, then follows June). And the 13 clear
`.avi` files need copying into the game's `data/movie/` when Q8 lands (Q8 asks).

## Phase RM0 - Random mission map and source proposal

**Run with:** Opus 5.5 high. Read-only.

- **What to map.** The brief's "Carried in" section (`kickoffs/june-backlog/stage4-plan.md`) and `q_rm_callsites.tsv` (41 sites). Cover:
  - the `0x492` payload (`31efe2a0` -> `326798b0`, impl ctor `32679bf0`, 0x44 B);
  - each of the 12 event types and its raise sites, named as March functions;
  - the four unmapped site families: `32154820`, `324d2e20` / `324db0f0`, `327d1820` / `327d38e0` / `327d2240`, `324d8f70`;
  - the state calls `32676070` / `326760a0` / `326760b0` / `326760c0` / `326765f0` / `32679a20` / `32679a50`;
  - the timer `326799a0`;
  - completion (`326797a0`, events `0x493` / `0x494`) and what the server owed back;
  - the UI (`32678910`, `326791c0`, `DLG_UI_Random_Mission(_PopUp).lua`: in June's koms? in March's?);
  - the three cheats (`31dd52e7`, `31dd5559`, `322f0e30`).
- **Source.** No June data defines missions. June's absent-value default is "no mission is ever set". Put the options to the user:
  - keep it dormant (client ported, the dev-console cheat sets a mission);
  - a labelled stand-in schedule under its own flag.
  Stop for the answer.
- **Output.** `june2017_re/quest_plan/rm0_map.tsv`, funcmap rows `Quest RM0`, the RM1 kickoff amended with the user's answer, and a Done note.

### RM0 - Done (2026-10-19)

Read-only, so there is no flag and no build. The evidence is `june2017_re/quest_plan/rm0_map.tsv` (56 rows: every raise site, state call,
payload field, UI function, kom file and cheat) plus 16 funcmap rows `(Quest RM0)`.

- **User decision (2026-10-19): dormant + cheat.** No mission appears in normal play, which is June's absent-value default. The emulator answers
  June's own cheat path. RM1's kickoff carries the details.
- **The server owned every value.** 0x492 carries `{ int id, type, goal, seconds, +0x10, float, set<int>, set<int> }`, then int `+0x38` and
  int `+0x3c`. The client keeps id, type, goal, seconds, the description STR id (`pkt[4]` -> `+0x18`) and the two reward fields
  (`+0x1c` likely the buff, `+0x20` likely the EXP; read from the UI text, not proven).
  - On success the client sends **0x493** with the mission id and waits 10 s for **0x494**. No client handler for 0x494 was found.
  - The reward button is an invisible button with no handler, so the server paid the reward.
- **Events (12)** and the March function each one is raised from:
  - 1 combo count: `CX2ComboManager::AddCombo`.
  - 2 boss damage % of max HP: `CX2GUNPC::DamageReact`.
  - 3 hits taken, must stay at or under the goal: `CX2GUUser::DamageReact`.
  - 4 kills: `Handler_EGS_NPC_UNIT_DIE_NOT`.
  - 5 survive: no raise site.
  - 6 sub-stage: `ClearSubStage`.
  - 7 quick slot: `Handler_EGS_USE_QUICK_SLOT_ACK`.
  - 8 resurrection: `Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK`.
  - 9 MP spent: June's virtual MP change, slot 3 of every gage class (March's non-virtual `UpNowMp`).
  - 10 / 11 / 12 skill types: hook `31fcd3d0` in 13 skill-use functions. 9 have March sites. Ain, Rose and Luciel (x2) have no March class.
  - Add's site has no `3235a710` port in March either.
- **Life cycle.**
  - `CX2DungeonGame::StageStart` (stage index > 0) starts a received mission, with a 5 s popup.
  - `CX2DungeonGame::OnFrameMove` ticks it. The time limit running out fails it.
  - `CX2StateDungeonGame::Handler_EGS_END_GAME_NOT` judges types 3 and 5. Any other type still running fails.
  - Three things reset it: `CX2State::Handler_EGS_GAME_START_TIME_COUNT_NOT` (June 0x385, count 3), `CX2DungeonGame::Release`, and agreeing to enter
    Gate of Darkness. `CX2UIManager::SetShowUI` and `CX2StateDungeonGame::ShortCutKeyProcess` mirror UI visibility.
- **Files.** `DLG_UI_Random_Mission.lua` and `_PopUp.lua` (June data034) and `DLG_UI_Common_Texture83_NEW` / `84_NEW` `.tga` + `.TET` (June data057)
  are absent from March. RM1 adopts them.
- **Cheats** (bound on `g_pX2Game`; Lua `Slash_rm` / `Slash_rmexp` / `Slash_rmbuff` in `GameEditCommandList`):
  - `SendRandomMissionCheat( a, b, c )` sends 0x495 with the three ints.
  - `SendRandomMissionRewardEXPCheat( n )` sends 0x496.
  - `SendRandomMissionRewardBuffCheat( n )` sends 0x497.
  - What a / b / c mean was the server's. RM1 proposes a labelled reading.
- **Not settled (RM1 reads them):** what sub-stage `+0x4` is (event 6), the exact key in `ShortCutKeyProcess`, and the Ain / Luciel flag test on event 9.

## Phase Q1 - Enums, loader keys, new binders, new June quest files

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_LOAD`. Sub-tag `LOAD`.

- **Enums, C++** (`X2QuestManager.h`): June's QT / SQT / QRT / CJT values, and `QUEST_REWARD_TRAN_CLASS`. `QT_END` and `SQT_END` take June's
  values; check every `QT_END` / `SQT_END` reader.
- **Enums, `Enum.lua`** (core source, as text): the same names. Add `NUI_STORY_QUEST` 10142 and `NUI_TRANSCEND_QUEST` 10153, plus the PvP
  members Q0a lists. Use June's values verbatim, and keep `DL_NOMAL` / `DL_HELL` nil as June has them (JUNE_BUGS row).
- **Parser.** `AddQuestTemplet_LUA` / `AddSubQuestTemplet_LUA` read every key Q0a marked new into June's members.
- **Binders.**
  - New: `AddChainQuestDays`, and `AddGroupChainQuest` only if June binds it.
  - `AddGuildQuestTemplet_LUA`: parse, log, skip.
  - The `GuildCoopQuestRewardTable` binder: parse and log.
  - The `Quest_Event_Scene_Info` loader (`31eeb040`), stored for Q5b.
  - Load order as June's `3258c470`.
- **Data.**
  - Adopt the June files that are new to March: `GuildQuest`, `ChainQuestInfo`, `GuildCoopQuestRewardTable`, `Quest_Event_Scene_Info`.
  - Leave `FieldQuest` / `FieldSubQuest` / `AccountQuest` alone: they are Q2's.
  - Use `adopt-june`, then pack and copy (ask).

**Done gate:**
- the load census line `[JUNE_QUEST] LOAD` shows quests 1,460 / sub-quests 1,733 (March data, unchanged), guild 15 skipped, chain 5 (+2 group if bound),
  scene-info 268, coop 45;
- no `XEM_ERROR79/80` new against a pre-Q1 ErrorLog.

### Q1 - Done (2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_LOAD` (`Always.h`; `MODS.md` has the file list and the revert). Code is built and deployed; the log census is the user's play-test.
- **Enums.** C++ and merged `Enum.lua`: QT 8-10 (`QT_END` 11), QRT 3-5, `CJT_JOB_FOURTH` 6 (`CJT_JOB_END` 7), SQT 37-48 (`SQT_END` 49), `QUEST_REWARD_TRAN_CLASS`, `PVP_TOURNAMENT_TYPE`,
  `PGT_WINNER` / `_1VS1_CHALLENGE_MODE` / `_AMUSEMENT_ITEM`, `PCC_1VS1_PVP_CHALLENGE`, `NUI_STORY_QUEST` / `NUI_TRANSCEND_QUEST` / `NUI_EVENT_KEEPER` / `NUI_EVENT_GUILD_EDAN_VILAGE`.
  `DL_HELL` / `DL_NOMAL` stay nil; `NUI_EDEL` stays 10072. The other June-only PVP members (`PGT_GUILD`, `PGT_EVENT_MODE`, ...) were not added: no quest file names them.
  `_END` readers: `QT_END` / `SQT_END` have none outside the header; the `CJT_JOB_END` loops (`X2QuestManager.cpp`, `X2UIQuestNew.cpp`) now also walk the empty `CJT_JOB_FOURTH` set.
- **Parser.** As the kickoff's list: quest keys, QR tail scalar, QT 9 reward split, clear-condition cases 37-39 / 43-47 / 48, `m_cDifficulty` (after the switch, June's 14 dungeon cases),
  `m_ePvpTournamentType`, `m_eDungeonID` keeping 0 (11 sites), case 43's inverted loop (JUNE_BUGS 321, plus a 1000-index cap June lacks so an empty table cannot hang).
- **Binders / loader.** `AddGuildQuestTemplet_LUA` (same function as `AddQuestTemplet_LUA`), `AddChainQuestDays`, `CX2GuildManager::AddGuildRewardData` (+ `g_pGuildCoopQuestRewardTable`, a second
  name for the guild manager, June `324066c0`). `AddGroupChainQuest` is not bound (JUNE_BUGS 320). Order in `CX2Data::ResetQuestManager`: Field, Sub, Account, **Guild**, ChainQuestInfo, Quest_Event_Scene_Info,
  census line; `AccountQuestDate` keeps its place in `X2StateLoading` (an independent table, empty in June).
- **Choices to know about.**
  - Guild quests (`QT_GUILD` / `m_bIsGuild`) are parsed, logged one per line, and not stored (the 2026-10-17 decision).
  - Chain and story indexes are written only for a stored quest; June writes while parsing, so a failed quest stays in its chain set there.
  - Scene-info rows live on the quest manager (`GetQuestEventSceneInfoMap()`), not in `g_pData` as in June (`+0x498`). Q5b reads them from there.
  - `m_cDifficulty` is a `char` (June: int); `AddGuildRewardData`'s 4th argument is stored as `m_iFourth` (0 in all 45 rows).
  - `ChainQuestInfo.lua` returns false after its 5 `AddChainQuestDays` calls (the 6th is the unbound `AddGroupChainQuest`). With March's `FieldQuest` no quest has `m_iChainID`, so "applied" is 0 until Q2.
- **Data.** data036: June's `GuildQuest.lua` (14,570 B), `ChainQuestInfo.lua` (531 B), `GuildCoopQuestRewardTable.lua` (1,610 B), `Quest_Event_Scene_Info.lua` (129,348 B), all new, adopt-june run record
  `june2017_re/june_adopt_20261019_065305.tsv`; `Enum.lua` edited as text, `luasrc.py refresh` done, `ledger --check` PASS. Packed (`PACK OK data036`: +4 added, ~1 changed) and copied with the user's yes
  (`COPY OK data036`, md5 `2d100f15...`, backup `kom_backup_tools\data036.kom.20261019_070244`).
- **Hygiene.** `flagcheck JUNE_QUEST_LOAD --worktree --files <7 code files>`: 5 PASS, 1 PASS~ (header), Always.h DEFINE (OTHER only because a peer's flags sit in the same file), 0 FAIL, 0 deleted lines, 0 non-ASCII,
  0 EOL mismatch, 0 `#endif` without the token. `--range` after the commit: see the review.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,123,840 B, dated 2026-10-19 07:04:59, md5 8525b5e686972c2c504b2145bede96bc, equal to X2/US_SERVICE/x2.exe (2026-10-19 07:04:51); built at main 12cc4fb3 + 17 uncommitted code file(s).`
  The exe holds the `[JUNE_QUEST] LOAD quests` string and the three binder names.
- **To see it.** Start the game (any character, reach the village). `python tools/logs.py --tag JUNE_QUEST` should show one `LOAD quests 1460 sub-quests 1733 guild skipped 15` census line,
  `LOAD guild quest <id> skipped` x15, `LOAD ChainQuestInfo.lua loaded 0 ... calls 5 applied 0`, `scene-info 268 coop 45`. No new `XEM_ERROR79/80` against a pre-Q1 ErrorLog.
- **Play-test (user, 2026-10-19).** The first exe closed before character select: `LoadQuestEventSceneInfo` used a default `KLuaManager` at root, which reads the table on top of the Lua stack. Fixed with `KLuaManager( L, 0, true )` + `LoadAndDoMemory` (the `CX2Book::SetBookTable` form). Second run, log: `LOAD quests 1460 sub-quests 1733 guild skipped 15 chain entries 0 ( calls 5 applied 0 ) groups 0 story groups 0 scene-info 268 coop 45`, 15 guild-skip lines, `ChainQuestInfo.lua loaded 0` (the expected JUNE_BUGS 320 Lua error), 0 XEM_ERROR79/80, 0 EXCEPTION; the 2 UNHANDLED are `E_DISABLE_HB_CHECK_REQ`. Done gate met.
- **Open for the user.** Nothing. The review is `kickoffs/quest-system/Q1-review.md`.

### Q1 - Review (2026-10-19, Opus)

**Verdict: PASS, no fixes, no rebuild.** Commits `be8ba59a` and `72de2249` were checked against `q0a_june_clearcond.md`, `q0a_june_templets.md` and June's `Enum.src.lua`.
- **Hygiene.** `flagcheck JUNE_QUEST_LOAD --range be8ba59a^..be8ba59a`: `files: 5 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  `--range 72de2249^..72de2249`: `files: 1 PASS, ...`, 3 deleted lines (the scene loader's replaced `LoadLuaTinker` block, inside the flag), `RESULT: PASS`. No later commit touches the flag.
- **Enums.** QT 0-10 / `QT_END` 11, QRT 0-5, CJT 0-6 / `CJT_JOB_END` 7, SQT 0-48 (each once, no gap) / `SQT_END` 49. These match June and the merged `Enum.lua` by name and value, and so do
  `QUEST_REWARD_TRAN_CLASS` (1000 / 1001), `PVP_TOURNAMENT_TYPE` (0-2), `PGT_WINNER` 6 / `_1VS1_CHALLENGE_MODE` 7 / `_AMUSEMENT_ITEM` 8, `PCC_1VS1_PVP_CHALLENGE` 15 and the four NUI ids.
  `NUI_EDEL` 10072 is March's own value (June's 10067 is March's `NUI_EVENT_ADAMS_UI_SHOP`); it was already there before Q1. No June quest file names `PGT_GUILD` / `_EVENT_MODE` / `_BEST_OF_N_GAMES` /
  `_HERO_PVP_PRELIMINARY`. `DL_HELL` / `DL_NOMAL` occur in June's `FieldSubQuest` only and stay nil, as June has them (JUNE_BUGS 322).
  The `CJT_JOB_END` readers (`X2QuestManager.cpp` :668 / :1806 / :1919, `X2UIQuestNew.cpp` :2898 / :2958) stay in bounds. `X2UIQuestReceive.cpp` sits under `#ifndef SERV_EPIC_QUEST`, so it is dead code. `QT_END` / `SQT_END` have no reader.
- **Parser.**
  - `m_eDungeonID` keeps 0 at 11 March sites. These are cases 2, 4, 5, 6, 7, 8 (+32 / 48), 12, 17, 22, 26 and 27, which is June's dungeon list less 37, and 37 is new code that reads `>= 0` itself.
  - Cases 37-39 and 43-45 are required or optional exactly as the key table says. 46 / 47 return true with no keys. 48 shares case 8's body.
  - 19 / 20 / 29 / 30 / 40-42 have no March case, so they take the default and return false. 35 is `SERV_POINT_COUNT_SYSTEM`, which is not defined for US, so it also returns false, as in June.
  - The `m_cDifficulty` post-switch is June's 14 types (2 4 5 6 7 8 32 48 12 17 22 26 27 37). It runs only on success, which matches June, where the key is last and a miss gives -1 and still true.
  - `m_ePvpTournamentType` is optional on cases 9 / 10. Case 43's inverted loop matches the disassembly at `3232c0cf`.
- **Quest level.** The `m_eUnitClass` `>= 1000` -> `% 1000` scalar matches. The QT 9 reward split matches: the only caller of `32330250` passes `"Reward"`, a miss gives 0, and the class becomes `UC_NONE`. `m_iGuildCoopQuestRewardID` is read from `Reward` into the quest, as June does.
- **Ghidra spot checks (read-only).**
  1. `31eeb040`: the per-CHAR rows are `OpenTableAt( 1.. )` inside the entry. `CHAR` is an int (a miss gives 0), the strings default to `L""` and both END_FRAMEs to -1. The MENT helper `31edff90` pushes every int by index, with no filter.
     The rows are keyed by CHAR and the entries by QuestID. The file agrees: 268 entries, 268 distinct ids, 1,198 rows, contiguous from 1, `CHAR = UNIT_TYPE.UT_*`, no duplicate CHAR. Spot entry 22321 has a DEFAULT pair plus a ROSE row with its own MENT lists.
  2. `3232c410`: case 44 `m_iDungeonClearCount` is required, and a miss gives -1, then false.
  3. `323321f0`: the chain index is written at `32334377`, before the duplicate check (`3233539b`). The story index is written after it (`32335515`, `m_eQuestType == 0xA`, key `+0x1b4` EpisodeGroupID).
- **Deviations Q1 chose; they are kept and none is a defect.**
  - The chain index is written only for a stored quest. June leaves a failed or duplicate quest's id in its chain set, and a reader would get a NULL templet. The two differ only if a quest with a `m_iChainID` fails to load; the Q2 census line will show whether one does.
  - `m_fSelectBGM` is read before the duplicate check rather than after it. That is not observable.
  - Scene rows overwrite on a duplicate QuestID, where June inserts without overwriting. There are no duplicates in the data.
  - `m_eUnitClass` values below 0 are inserted, as March already did, while June drops them. The data has none.
- **Guild coop.** `g_pGuildCoopQuestRewardTable` is the guild manager under a second name. All 45 June rows are `AddGuildRewardData( reward id, item id, quantity, 0 )`, which matches the binder.
- **Open for the user.** Nothing. Next is Q2 (`kickoffs/quest-system/Q2.md`, Opus 5.5 high).

## Phase Q2 - Data flip and old-save migration

**Run with:** Opus 5.5 high, then a review. Flag `SERV_IRUHADEV_JUNE_QUEST_DATA`. Sub-tags `LOAD`, `MIG`.

1. **Census first.** Use the save copy and `elsdb.py`. For every unit, list:
   - in-progress quests with no June templet;
   - in-progress quests whose `m_vecSubQuest` changed (`SubQuestInst` is positional);
   - completed quests with no June templet;
   - each unit's epic-chain position, mapped to June's chain (`m_iAfterQuestID` / `m_iEpisodeGroupID`);
   - units that cleared `DI_RANOX_FINALHALL_NORMAL` (Atlas decision 6: Lanox chain 22160-22217).
   Show the user the counts per category, with proposed handling: drop and log, reset progress, keep, mark complete, set the epic position.
   **Stop for the answers.**
2. **Flip.** Adopt June's `FieldQuest`, `FieldSubQuest` and `AccountQuest`, then pack and copy (ask). June's wins at the same filename, so the
   257 March-only quests go.
3. **Migration.** Write rung v21 (or the next free number) as the user answered. Snapshot first, and log one `[JUNE_QUEST] MIG` line per unit and category.
   The rung must also handle a quest id with no templet at run time: `GetQuestTemplet` NULL readers get a guard plus a log line, never a crash.

**Done gate:**
- the load census shows June's 1,633 / 3,219 (guild 18 skipped);
- the MIG lines match the census;
- every character logs in and opens the quest window without a crash;
- the epic chain resumes where the user chose.

### Q2 - Done (2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_DATA` (Always.h, `#error` without `JUNE_QUEST_LOAD`). Opus did everything here; the reader survey went to a Sonnet subagent and
its 10 sites were re-read before the guards went in.
- **Census** (`june2017_re/quest_plan/q2_census.py` on a copy of the save -> `q2_census.tsv`). The save holds 58 distinct quest ids; 26 are not in June,
  which confirms the planning count.

  | Category | Rows | User's answer (2026-10-19) |
  |---|---|---|
  | in progress, no June templet | 16 (units 12 / 18 / 19 / 24 / 43; 5 of them epics) | drop; where an epic was dropped, accept June's next epic |
  | in progress, sub-quests changed | 42 (11005 ×19, 11010 ×13, 11030 ×4, 6 others) | reset all 42 to 0 |
  | completed, no June templet | 19 (units 12 / 18 / 19 / 25) | delete and log |
  | Lanox cleared (`DI_RANOX_FINALHALL_NORMAL`) | 0 (reyaa cleared Expert 33322 only; their chain is at Sander 21910) | nothing |

  Kept: 6 in progress, 76 completed. June opens its next epic only on a completion (none of the targets has a village, dungeon or before-list
  gate), so a dropped in-progress epic would leave its chain stuck. That is why the 5 epic next links exist: 18020 -> 18030, 21910 -> 21920,
  14005 -> 14010, and 12005 -> 12020 for units 24 and 43.
- **Flip.** `luasrc.py adopt-june FieldQuest.lua FieldSubQuest.lua AccountQuest.lua` (ljconv bytecode, 3 replaced; record
  `june2017_re/june_adopt_20261019_074358.tsv`). `PACK OK data036`, `COPY OK data036` (md5 929fad78; old kom in `kom_backup_tools\data036.kom.20261019_083315`). March's originals are snapshotted to `24965799\kom_contents_bytecode\data036_kom\`,
  where the census and the generator read them.
- **Rung v21.** `CX2OfflineDB::MigrateQuestsToJune` (`X2OfflineDB.cpp/.h`, `#error` without v20 `JUNE_SKILL_PAGE`) works in one transaction and logs one
  `[JUNE_QUEST] MIG` line per unit and category after COMMIT, then a `MIG v21 done` total. Its tables are in `X2Lib/Offline/X2OfflineJuneQuestMig.h`, generated
  by `q2_gen.py` (`--check` re-proves it):
  - 1,638 June quest ids (FieldQuest plus AccountQuest);
  - 833 reset ids: every common quest whose sub-quest list or a sub-quest's clear condition changed (42 are in this save);
  - 52 epic next links: for each March epic June dropped, the epics June's after-list opens from the same March predecessor.

  `q2_gen.py simulate` on the pre-v21 copy writes `q2_expected_mig.txt`: 25 units, 16 dropped / 5 accepted / 42 reset / 19 completed dropped, which equals the census.
  Snapshot: `db_backup\els_db_20261019_075100_pre_pre-quest-q2-v21.sql`.
- **Guards** (`X2UIQuestNew.cpp`; each logs `[JUNE_QUEST] MIG quest N has no templet - <where> skips it` once per id). The survey found 127 call sites,
  15 of them live and unchecked:
  - guarded: `Handler_EGS_QUEST_COMPLETE_ACK` and `Handler_EGS_ALL_COMPLETED_QUEST_COMPLETE_ACK` (repeat type, end scene, job-change test), plus
    `GetPartyUIQuestIcon`, `GetLocalMapQuestDesc`, `UpdateUIQuestSlotInfo` and `SetUIQuickQuestSlotInfo`;
  - not guarded: the other 5 (`UpdateAvailQuestInfo`, `UpdateOngoingQuestInfo` ×2, `X2WorldMapUI` ×2) take ids from lists that already drop NULL templets.

  The emulator already skipped a save row with no templet (`X2OfflineQuest.cpp` load), and its complete handler refuses one (`ERR_QUEST_08`).
- **Chain index (from the Q1 review).** Q1 writes a chain index only for a stored quest. A June quest that fails to load and carries `m_iChainID` would
  therefore drop out of its chain; the LOAD line's `XEM_ERROR79/80` count in the play-test shows whether any does.
- Hygiene: `flagcheck JUNE_QUEST_DATA --worktree`: 2 PASS, 2 PASS~ (blank/marker only), 1 OTHER (Always.h), 0 FAIL, 0 deleted lines, 0 encoding changes, RESULT: PASS.
- Deploy: `DEPLOY OK: X2_offline.exe is 25,305,088 B, dated 2026-10-19 08:02:40, md5 134d1b2edaaba175e675fe62d91841d1` (main 4c96fcc5 + uncommitted;
  the `MIG v21 done` and guard strings are in the exe).
- **Play-test (user, 2026-10-19): PASSED.** Log of the deployed exe:
  - `MIG v21 done: 25 unit(s); in-progress dropped 16, epic next accepted 5, progress reset 42, completed dropped 19`; the 38 per-unit lines equal
    `q2_expected_mig.txt` line for line.
  - The save is v21 and holds the accepted epics (12: 18030 / 21920; 19: 14010; 24 / 43: 12020). None of the dropped ids remain.
  - LOAD: `quests 1635 sub-quests 3218 guild skipped 18 chain entries 1 ( AddChainQuestDays calls 5 applied 1 )`.
    - 1,635 = 1,638 June ids minus the 3 guild quests in FieldQuest (35600 / 35610 / 35620). Guild skipped 18 = those 3 plus GuildQuest's 15.
    - 3,218 = 3,219 minus sub-quest 205680, a June dev test that names the nil `NPC_UNIT_ID.NUI_` (`SubQuestTempletLoadFail_205680`, JUNE_BUGS 331).
      No quest uses it. The ErrorLog writes XEM_ERROR80 as `ErrorEnum(10080)`; grep that.
    - Chain: all 91 June chain quests carry `m_iChainID = 1`, so only 1 of ChainQuestInfo's 5 rows can apply. That is June's data. No chain quest
      failed to load, so the Q1-review difference (chain index written only for a stored quest) does not show.
  - 0 EXCEPTION. The 6 UNHANDLED are `E_DISABLE_HB_CHECK_REQ`, outside this phase. No guard fired.

### Q2 - Review (2026-10-19, Opus)

**PASS, with one generator fix** (flag `SERV_IRUHADEV_JUNE_QUEST_DATA`; reader survey by a Sonnet subagent, two of its citations re-read).
- **Tables.** At review start `q2_gen.py --check` passed. Hand-derived EPIC_NEXT rows:
  - 12005 -> 12020: March has 12000 -> 12005 -> 12010 -> 12020. June's 12000 opens 12020 directly (June keeps 12010, but nothing opens it from 12000).
  - 15020 -> 15030: March's 14060 / 14070 open 15020, which opens 15030. June drops 15020 and has the predecessor open 15030.
  - Both match the after-lists.
- **Fix: the RESET definition counted a parse artefact as a change.**
  - March writes `m_eDungeonID = {0}` and June leaves the key out. March's loader keeps only ids > 0 (`X2QuestManager.cpp:2384`); June's
    `32323ad0` keeps ids >= 0 (Q0a map row 74). Both store the same empty set, but `subsig` compared the text.
  - `q2_census.subsig` now compares `m_eDungeonID` as the set each loader stores (`march=True` on the March side, also used by `q2_gen.py`).
  - RESET drops from 833 to 758: 75 false ids removed, 0 added. The header was regenerated (`--check` PASS) and rebuilt.
  - The save is already v21, so the fix only affects a save that has not migrated yet.
  - One of the 42 resets in this save was false: unit 18 (Freils), quest 13000. Its subs 12090 / 13000 match June's apart from the `{0}`. The reset deleted
    `(12090, clear 1, success 1)` and `(13000, 0, 0)`. User (2026-10-19): leave it reset, no restore. `q2_census.tsv` and `q2_expected_mig.txt` stay unchanged as the record of what ran.
  - The other 41 resets are real changes: a new or removed sub-quest (11005, 11010, 11030, 12020), a new `m_cDifficulty` (7780), a changed count or rate (12040, 14010),
    or a changed NPC id (9501 / 12020: 78 -> 1613).
- **Latent, not fixed:** 58 of March's 110 dropped epics have no EPIC_NEXT link.
  - In 30 of them the predecessor was dropped too: a run of dropped epics, and the rule only looks one step back.
  - The other 28 have no March after-list predecessor.
  - The only save is already past v21 and held none of these, so nothing is stuck. A future pre-v21 save would need the link to walk back to the nearest kept predecessor.
- **Rung.** Checked:
  - order: drop in-progress, then accept the next epic only when the unit neither holds nor completed it (checked against the live set, so 17050 / 17080 -> 18000
    cannot accept 18000 twice), then reset only quests with sub rows, then drop completed;
  - one `BEGIN` / `COMMIT`, with `ROLLBACK` on any failed step, the same shape as the v9-v20 rungs; the logs are written only after `COMMIT`;
  - the insert matches the emulator's accept (`X2OfflineDB.cpp:3517`, `state 0`);
  - the v21 enum branch replaces v20 wholesale (`SCHEMA_VERSION`, `DEFAULT_RESURRECTION_STONE`, as every earlier branch does), and the `#error` on missing `JUNE_SKILL_PAGE` is in place.
  - Side note: `JUNE_QUEST_IDS` includes FieldQuest's 3 guild quests, which the loader skips. A saved guild quest would be kept and then skipped by the guards. The save has none.
- **Play-test log and save.** The 38 MIG lines equal `q2_expected_mig.txt` (Done note). The save holds 18030 / 21920 (unit 12), 14010 (12 / 19),
  12020 (19 / 24 / 25 / 43) and 13000 (18), all `state 0`.
- **Guards.** 94 `GetQuestTemplet` call sites in X2Lib: 34 dead, 60 live. 55 of the live sites check NULL at the call, including the six Q2 guards. The other 5
  (`X2UIQuestNew.cpp:4644 / 4815 / 4959`, `X2WorldMapUI.cpp:2143 / 2194`) get ids that are already NULL-filtered upstream (`X2QuestManager.cpp:1833`,
  `X2UIQuestNew.cpp:4720`, `:4762`, `X2WorldMapUI.cpp:1877`). No other live, unchecked reader can see a saved id that has no templet.
- **Sub-quest 205680:** June refuses it too.
  - In the `SQT_NPC_TALK` case of `32323ad0`, the key is read by `32fe9fd0`, a number test that fails on nil. On failure it sets `m_eTalkNPCID` to 0 and returns false
    (`3232c537`).
  - `AddSubQuestTemplet_LUA` `32330ff0` (@`3233161d`) then logs 0x2760 (10080) and frees the templet without inserting it.
  - So the port matches June, and JUNE_BUGS row 331 is updated to "June refuses it too".
- **Flip.** `luasrc.py ledger --check` reports LEDGER CHECK PASS (0 FAIL, 0 WARN). The adopt record lists the three files as `replace` / ljconv bytecode, and LOAD moved to 1635 / 3218
  from March's 1,460 / 1,733.
- **Hygiene.** `flagcheck JUNE_QUEST_DATA --range 8be95c0a^..HEAD`: `files: 2 PASS, 2 PASS~ (blank/marker only), 2 DEFINE, 7 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 1 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`.
  - The OTHER rows are peers' commits in the range (G3 JPEG8, S7g) and Always.h.
  - The 1 deleted line is the user's `3b6408ca`, which moved a `////` separator between the GFX_EVENTSCENE and GFX_JPEG8 blocks; it is not this flag's.
  - Review fix, `--worktree --files X2OfflineJuneQuestMig.h`: 1 PASS, RESULT: PASS. Its 66 deleted lines are the regenerated RESET array, inside the file's own `#ifdef`.
- Deploy: `DEPLOY OK: X2_offline.exe is 25,305,600 B, dated 2026-10-19 09:04:58, md5 23214a712a98e1cf2726593bc2c1472c` (main 96bfd4fe + uncommitted; includes peers' edits). Nothing new to play-test: the rung already ran on this save.

## Phase Q3 - Quest manager runtime to June

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_RUNTIME`. Sub-tag `RUN`.

- Port the rows Q0a targeted Q3, whole:
  - completion checks for the new types;
  - the progress strings (`MakeSubQuestString`) for the new SQTs;
  - chain quests and chain days;
  - `QT_STORY` handling;
  - week / month repeat;
  - `IsCompleteQuest` and the readers Q0a listed;
  - `GetBoardStartQuestList` with June's village cases (the Lanox / Elysion board; fixes the unchecked deref at `X2QuestManager.cpp:1233`).
- After `RemoveUnitQuest`, `RefreshVillageNpcRelationDataByQuest` already runs. Confirm that it fires on a real completion.

**Done gate:** for each new type a dev-console or emulator path shows the right progress text and completion state. `[JUNE_QUEST] RUN` lines are per Q0a.

## Phase Q4 - Offline completion of every sub-quest type

**Run with:** Opus 5.5 high, then a review. Flag `SERV_IRUHADEV_JUNE_QUEST_OFFLINE`. Sub-tag `OFF`.

- In `X2OfflineQuest` / `Handlers_Quest.cpp`:
  - drive each new SQT Q0a says the server drove, or add it to `IsClearTypeUndriven` with a reason (guild and PvP: decision / no PvP);
  - apply `m_bExpDungeonAndField`, `m_eExceptionDungeonID` and difficulty-only dungeon clears in the hunt / clear hooks, as Q0a decoded them;
  - `QRT_WEEK` / `QRT_MONTH` resets;
  - the chain-day rule;
  - account quests (5);
  - `QT_STORY` auto-open;
  - the transcendental reward refusal (Stage 6);
  - pay `Reward.m_iBuff` and the title reward if the survey's "not traced" is a gap (check March's server).
- Answer or label every June-only quest packet Q0b listed.

**Stop and ask** before any stand-in for something June's server did that March's server code does not show.

**Done gate:** `IsClearTypeUndriven` and the driven list together cover SQT 0-48. A play-test completes one quest of each driven new type, and
`grep UNHANDLED` stays at baseline.

## Phase Q5a - Quest UI: window, tabs, list / detail / quick, accept / complete / give-up

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_UI`. Sub-tag `UI`.

- Port the `CX2UIQuestNew` rows Q0b targeted Q5a, whole: June's tabs (Story / Guild / Request / Scenario / Event), list / detail / reward builder `323e1bf0`,
  quick quest, skip-epic, the handlers.
- Adopt June's `DLG_UI_NEW_Quest_*` together with the code, if Q0b says they are not in yet.
- Leave the scene calls at their March behaviour (Q5b).

**Done gate:** the window opens on every tab, accepts and completes a quest, and gives one up, with June's layout. 0 EXCEPTION.

## Phase Q5b - Quest UI: scenes, clear window, scene info

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_SCENE`. Sub-tag `SCENE`. Needs june-gfx G3.

- `323cb2e0` and its callers, on G3's `CX2GFXEventScene`.
- The per-class `Quest_Event_Scene_Info` choice.
- The Lua-scene fallback for `m_wstrStartScene` / `m_wstrEndScene`.
- `m_vecAdditionalSceneQuestID`.
- The clear window `323d2c60`.

**Done gate:** accepting Elysion quest 22329 plays `Quest_22329_Start` (GFx). A pre-Elysion quest with a Lua scene still plays that scene.

## Phase Q6 - Quest gates: the inert keys and their readers

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_GATE`. Sub-tag `GATE`.

- Uncomment AD Normal's `m_iRequireCompleteQuestID = 22218` (the reader is live already).
- `MapData` `REQUIRE_COMPLETE_QUEST_ID` / `_SUB_QUEST_ID`, with Q0a's reader.
- Local-map `RequireCompletedQuestID` (templet `+0x70`, parser `32501bb0`) and its five readers: Cobo `324627f0`, warp `32173120`,
  local-map tab `3293c6f0`, world map `3225d340` / `3225d970`, and dungeon-enable `327da190`.
- `NEED_ONGOING_QUEST_ID` / `BATTLE_FIELD_DIRECTION_INFO` (the Lanox F04 row).
- `GetNPCFaceTextureKey`.
- Remove every `-- quest plan:` comment this phase activates.

**Done gate:** on a unit without 22218, the Elysion tab, AD Normal and the Cobo / warp rows are closed. Completing 22218 at the dev console
(`elsdb` or the cheat) opens them.

## Phase Q7 - Quest NPCs and houses

**Run with:** Sonnet 5.5 high. Flag `SERV_IRUHADEV_JUNE_QUEST_NPC` (only if C++ changes). Sub-tag `NPC`.

- `NPCTemplet` rows (core source) for `NUI_STORY_QUEST` / `NUI_TRANSCEND_QUEST`, plus `NUI_EVENT_KEEPER` / `NUI_EVENT_GUILD_EDAN_VILAGE` if a June quest needs them.
- Their meshes (`npc_mesh_census.py`, `stage-june`).
- The `HI_STORY_QUEST` / `HI_TRANSCEND_QUEST` house `NPC_ID_LIST` in Atlas Station and Elysion.
- The cutscene NPCs `NUI_ELSION_EVENT_00/01`.
- The relation keys `Change_NPCJoinTalk` / `NPCByeTalk` that March's relation loader skips (no partial update of E1).

**Done gate:** the Story and Transcend houses show their NPC. Story quests (start NPC `NUI_STORY_QUEST`) can be taken there.

### Q7 - Done (2026-10-19)

Lua and assets only: **no C++, no flag, no build**. The `[JUNE_QUEST] NPC` sub-tag has no emitter, so there is nothing for `logs.py --tag` to read; the check is a
clean `ErrorLog.txt` (below). Sonnet did everything here.
- **What was already done (nothing to add).** `Enum.lua` already carries all six ids (`NUI_STORY_QUEST` 10142, `NUI_TRANSCEND_QUEST` 10153, `NUI_EVENT_KEEPER` 4116,
  `NUI_EVENT_GUILD_EDAN_VILAGE` 4255, `NUI_ELSION_EVENT_00/01` 2139 / 2140). The Story / Transcend houses in `DLG_Map_House.lua` already list their NPC in
  `NPC_ID_LIST` (they pointed at undefined ids until the Enum edit, so they were empty holes). Both houses are already in the Ranox (20008), Elysion (20009) and
  Elysion rest-field village lists. `Change_NPCJoinTalk` / `Change_NPCByeTalk` are read since Atlas E1a (`X2QuestManager.cpp:102-123`, applied at
  `X2TFieldGame.cpp:3478`), so that bullet of the plan was stale. `CX2TFieldNpc::CreateFieldNpc` already treats both houses as static meshes (`:3376`).
- **`NPCTemplet.lua` (core source, data037).** June's 6 rows appended verbatim, in one `--{{ ... --}}` block: `NUI_STORY_QUEST`, `NUI_TRANSCEND_QUEST`,
  `NUI_ELSION_EVENT_00`, `NUI_ELSION_EVENT_01`, `NUI_EVENT_KEEPER`, `NUI_EVENT_GUILD_EDAN_VILAGE`. Keeper and Edan are the talk targets of June sub-quests 201090 and
  208020 (`FieldSubQuest`), so they are needed. `luasrc.py refresh data037 NPCTemplet.lua` done; `ledger --check` and `core --check` PASS.
- **Villages (`DLG_Map_Village.lua`, core source, data034).** `HI_STORY_QUEST` and `HI_TRANSCEND_QUEST` appended to the `HouseIDList` of Ruben, Elder, Besma,
  Altera, Peita, Velder, Hamel and Sander (June has them in all 10 villages; March had only Ranox and Elysion). Story quests have 78 start-NPC uses and
  Transcend 15 in June's census, and every village has to be able to host them. Refreshed.
  Not copied: the other houses June's lists carry that March's lack (`HI_ADAMS_HOUSE`, `HI_SPA_SIGN_*`, `HI_EVENT_GUILD_EDAN_VILAGE` outside Ranox ...); they are not quest NPCs.
- **Assets (`kom.py stage-june`, `adopt-june`).** Closure by `npc_mesh_census.py --seed` (its tsv overwrote `npc_anim_collision_need.tsv`; restored from git):
  - datai002: `Motion_NPC_Event_Keeper.x` + 3 `.tga`.
  - datai024: `Motion_NPC_Event_EDAN.x`, `HQ_UI_NPC_EVENT_KEEPER.X`, `UI_NPC_Event_EDAN.x` + 3 `.tga`.
  - datai016, June bytecode via `adopt-june`: `UI_NPC_EVENT_KEEPER.lua`, `UI_NPC_EVENT_EDAN.lua` (record `june_adopt_20261019_093327.tsv`).
  - Already staged: the `ELSION_EVENT_00/01` scripts (data037) and meshes (data181).
- **Packed**, not copied: `PACK OK` data034 (~1 `DLG_Map_Village.lua`), data037 (~1 `NPCTemplet.lua`), datai002 (+4), datai016 (+2), datai024 (+6). Diffs against the live koms
  show only these files. **Copy waits for the user's yes** (the game must be closed).
- **How to see it.** Houses with `Dummy_Ani_Wait.X` and no `COMMON_NPC_LOCATION` are invisible by design (June's own data). The proof is: the game starts with the
  new `NPCTemplet` (a bad one logs ErrorEnum 10002 at load), the 8 villages load, and `ErrorLog.txt` has no new NPC / house line. Taking a Story quest through the
  window is Q5a's UI and Q4's completion; Q9 walks it.
- **Open for later phases.** `BATTLE_FIELD_DIRECTION_INFO` (names the two cutscene NPCs) still has no reader in March (Q6 / A6 note); the NPC rows now exist for it.

## Phase Q8 - Scene data and clear movies

**Run with:** Opus 5.5 high, then a review. Flag `SERV_IRUHADEV_JUNE_QUEST_MOVIE`. Sub-tag `MOVIE`.

- Adopt June's `EventScene.lua` if Q0b finds it parse-compatible. If not, port what the parser needs: 980 of 1,100 quest scene names are in June's
  file and 907 in March's.
- The clear movies: the `DungeonClearMovieTemplet` binder, the movie table, and the player as Q0b specified.
  - Atlas 22236 -> `Movie_Atlas_Clear.avi`, Elysion 22336 -> `Movie_Elysion_Clear.avi` plus the `datai015` subtitles.
  - Coordinate the `+0x80a` hide term with june-gfx G5.

**Done gate:** completing 22336 (dev console) plays the Elysion clear movie with subtitles, then returns to the game.

## Phase RM1 - Random mission port

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_RANDOM_MISSION`. Sub-tag `RM`.

- The client side, whole, as RM0 mapped it: singleton, 12 raise sites incl. `31fcd3d0` in the 13 per-class skill-use functions, UI, timer, completion sends.
- The emulator side as the user answered in RM0.

**Done gate:** a mission set by the cheat or by the stand-in counts its event, completes, and shows the UI.

## Phase Q9 - Region walk

**Run with:** Sonnet 5.5 medium, then a user play-test (batched).

- Prepare one character (`/x2-create-char`, `/x2-db`) at 22140 complete.
- Write the checklist for the Lanox chain 22160 -> 22218, the Atlas epics, and the Elysion chain to 22336. Cover NPC swaps (Yuno -> Hyper Yuno -> Nono),
  gates opening, scenes, the clear movies and the board.
- Fix nothing here: findings go to the owning phase as a follow-up.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium.

- `MODS.md` flag rows and revert steps.
- The backlog's Stage 4 note.
- Memory.
- `_DEBUG` flags off.
- Owed items handed to Stage 6 (transcendental rewards) and Stage 9 (GFx quest screens, if any).

## Open questions (planning, 2026-10-19)

None for now. Each one arises inside its phase, after that phase's map exists:
- Q2: the migration choices per category;
- Q4: any server stand-in;
- RM0: the mission source.

## Play-test hand-off (every code phase)

Dev console first. Copy-ready lines with real values (character + job, quest id, `elsdb` commands). Per check: where / paste / what it
does / look for / if not. End with `python tools/logs.py --tag JUNE_QUEST` and the phase's sub-tag
([[playtest-handoff-dev-console-copy-ready]]).
