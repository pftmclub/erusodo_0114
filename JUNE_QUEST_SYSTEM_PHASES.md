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

### Q3 - Done (2026-10-19, Sonnet 5.5 high)

Flag `SERV_IRUHADEV_JUNE_QUEST_RUNTIME` (+ `_DEBUG`, off after the review); files `X2QuestManager.h/.cpp`, `X2PlayGuide.h/.cpp`, `Always.h`; `MODS.md` has the list and the revert. Every function below
was read in Ghidra (decompile + disassembly where the decompile hid a compare). Q0a's rows targeted Q3 are all ported except the four in "Not ported".
- **Chains.** `IsChainQuestInProgress` 323229d0, `GetChainQuestStartable` 32322ad0, `IsChainQuestAvailable` 3232caf0 (max completion time of the chain; code event 0x10 + TestMinute = minutes;
  else local 00:00 of the day AllowFewDays days after it <= now), `RebuildChainQuestStartable` 3232ef70, `ForgetCompletedQuest` 3232de60 (no caller yet; June's is 323cf130, Q5a). Hooks: `CreateUnitQuest`
  32331830, `RemoveUnitQuest` 32331fb0 (insert-unique time under the chain, then rebuild), `RefreshDailyQuest` 3232f830, `ClearUnitQuest` 32331dd0, `SetUnitQuest` 32336e80 (records each completed chain quest,
  rebuilds after the loop), `GetAvailableQuest` 323357c0 (a quest of a chain whose flag is false is dropped; no flag = stays). Chain 1 = the 91 QT_STORY quests (AllowFewDays 1), chains 43648 / 208840 / 208950 /
  43760 (TestMinute 3) the rest; the `ChainQuestInfo.lua` file stops after its 5 `AddChainQuestDays` calls (JUNE_BUGS 320).
- **Week / month.** `IsWeekQuestExpired` 32322050 (reset = the Wednesday strictly after the completion day, 00:00 local) and `IsMonthQuestExpired` 32321d10 (the 1st of the next month), run only inside
  `SetUnitQuest`, as in June (not at midnight). `SetUnitQuest` takes June's third argument `bClear_` (default true: no March caller passes false; June's false caller is a guild-agit handler March lacks).
- **Lists.** `GetStartQuestList` 3232fbd0: QT 2 / 9 quests with a start NPC are listed at every NPC (once), QT_GUILD only in a guild; `GetAvailableQuest`: QT_GUILD dropped in the non-repeat branch;
  `GetBoardStartQuestList` 3232fd70: June's table (state -> village -> board village; Glave, Luto, Dao always; Peita and the Elysion rest village return early; 1500 adds the billboard). NULL-checked, which
  fixes the unchecked deref the plan named. The Benders event NPC March added is gone (June wins). June's state 0xf reads a location-manager member March lacks: it falls to Ruben.
- **Gates.** `IsForbiddenQuest` 3232e510: the QR tail must equal the unit's transcendental step (always 0 in March; Stage 6) unless QT_EVENT, so June's 10 `QR_TRANSCENDENTAL_CLASS` QT_SKILL quests (42040..)
  are hidden for now; QT 9 hidden while code event 0x920 / 0x921 / 0x923 is on for UT_AIN / class 135 / 136 (March has no Ain: inert). Left out as server-fed with an absent value of "not forbidden": the 31000
  test (`g_pData +0x52c`), the `+0x51c` set with the `+0x530` records, and the quests 208020..208060 / 205530 / 37100, none of which is in June's `FieldQuest` (31000 is).
- **Completion and text.** `SubQuestInst::IsComplete` 323225e0 (collection = server flag OR count), `IsCompleteVisitQuest` 3232c750 (no last-sub-quest rule). `MakeSubQuestString` 32337db0: SQT 37 (auto and
  plain), 39, 43 as `description cur/need`, SQT 44 and 48 added to the clear-count group (`m_iDungeonClearCount`), the any-dungeon visit text (`IsAnyDungeonCondition` 32322290 -> `GET_STRING( 42924 )` +
  `STR_ID_17688`), SQT 45-47 print nothing. Trailing newline of the auto 37 form is a guess (stream calls in the disassembly are ambiguous). `m_cDifficulty` has no reader anywhere in June's quest code or
  Q0b's UI map: left stored.
- **Class-up.** `GetClassUpQuestNoticeState( step )` 32336310 (NPC Echo 10004; 0 / 1 / 2 for the notice buttons). Its step helper 31edf870 and the notice message 32344ab0 belong to the class-up UI (Stage 9).
- **Go effect.** `CX2PlayGuide::StartGoEffectNewbie` 3289dd50 (11005 not complete and level < 3) at X2PlayGuide.cpp:492; the in-progress site :524 is left (June's flow there was not read).
- **Relation refresh.** `RefreshVillageNpcRelationDataByQuest` follows `RemoveUnitQuest( id, true )` at all three completion sites (X2UIQuestNew.cpp:1493/1495, 1652/1654, 2061/2063); it now logs
  `[JUNE_QUEST] RUN relation refresh after quest N`.
- **Not ported.** `32335f50` (rebuilds `g_pData +0x570` from the story-book singleton 326ab090 / `DLG_Story_Book.lua`, no March consumer): new phase Q10. `32344ab0` + `31edf870`: Stage 9. `31d79ac0`
  (unlock quest movies) is Q8's. Open for Q4: June's `SubQuestInst` count is a `short`; March's `m_ucClearData` is a UCHAR, so the 8 NPC_HUNT quests with 300-500 kills cannot complete.
- **Hygiene.** `flagcheck JUNE_QUEST_RUNTIME --with JUNE_QUEST_RUNTIME_DEBUG --worktree --files <5 files>`: 4 PASS, 1 OTHER (Always.h, peers' flags), 0 FAIL, 0 deleted lines, 0 non-ASCII, 0 EOL mismatch, `RESULT: PASS`.
  JUNE_BUGS 337 (board falls back to Ruben for the Sander / Ranox / Elysion lounges), 338 (unchecked sub-quest instance in SQT 37 / 43, guarded).
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,378,816 B, dated 2026-10-19 12:09:26, md5 8ac3a5d7258ec3a5a9706ee3922a624e, equal to X2/US_SERVICE/x2.exe (2026-10-19 12:09:18); built at main dbc0f686 + 25 uncommitted code file(s).` The exe holds the four `[JUNE_QUEST] RUN` strings (chk, chain rebuild, repeat purge, relation refresh).
- **Play-test (user, 2026-10-19; exe `642e712f`, main f2bc9cc8, a peer's redeploy that holds this commit).** Log, 0 EXCEPTION, ErrorLog 0 matching lines, only the known `E_DISABLE_HB_CHECK_REQ` UNHANDLED. `chk type` 37 / 39 / 43 / 44 / 48 / 8 / 3 print `description cur/need` (e.g. `Win in Ereda Island 3/1`) and are complete with the flag on, not off; `chk repeat`: week expired now 0 / 8 days ago 1 / 1 day ago 0, month now 0 / 40 days ago 1; chain 1 (91 quests, 1 day) `available 1 startable 1`. Unit 20 login: `repeat purge` 31020 (week) and 42490 (month), 31030 kept (completed 2 of 4 records), `chain rebuild closed [ 1 ]` for the story chain completed today; starting a chain quest closes chain 1 and the next login reopens it. `RemoveUnitQuest` 26600 / 31110 / 11000 each followed by `relation refresh` (46 rows). Not covered by this run: the any-dungeon visit text (June's data has no SQT 26 sub-quest with { DI_NONE }), the board / forbidden / Go-effect changes (code-read only).
- **To see it.** Log in with any unit (unit 12 `reyaa` lv 99 is fine) and read `python tools/logs.py --grep "JUNE_QUEST\] RUN"`: `chk type ...` lines for SQT 37 / 39 / 43 / 44 / 48 / 8 / 3 / 26 (text without and with
  an instance, completion with the flag off / on), `chk repeat` (week / month expiry on synthetic dates), `chk chain` per chain, then `SetUnitQuest ...` and `chain rebuild ...` lines. Chain and repeat purge on
  real data: see the hand-off.

### Q3 - Review (2026-10-19, Opus 5.5 high)

**Verdict: PASS, no transcription errors, no code fix.** `c1e0f8f5` was checked against Ghidra (read-only) and `q0a_qm_summary.md`. The only change is `SERV_IRUHADEV_JUNE_QUEST_RUNTIME_DEBUG` off (`Always.h`).
- **Hygiene.** `flagcheck JUNE_QUEST_RUNTIME --range c1e0f8f5^..c1e0f8f5 --with JUNE_QUEST_RUNTIME_DEBUG`: `files: 4 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Flag off = Q2's build (insert-only, every `#else` intact).
- **Ghidra spot checks (all match).**
  1. `SetUnitQuest` 32336e80: clears only when param 4 == 1. Per record: a missing templet or the wrong server group is counted with no chain record; then the QRT_DAY skip; then type 3 -> `32322050` and type 4 -> `32321d10`, both skip (not counted); then templet +0x278 != 0 -> insert under +0x11c (a new list when the chain is absent); then the count. After the loop: `3232ef70`, `32335f50` (Q10), the 11005 novice flag, `323227e0`, `31d79ac0` (Q8). The port has the same order.
  2. `GetBoardStartQuestList` 3232fd70: all 35 village cases equal the port's table. Tail: 1500 adds 0x2729 (`NUI_BILLBOARD` 10025); the *current* village 20005 / 1008 returns early; then 10026 / 10048 / 10049 (Glave / Luto / Dao, counted in `X2UnitManager.h`). June state 0xe = March `XS_VILLAGE_MAP` (14); 0x15 calls `324826f0` (return village, funcmap "Atlas A1a" row) = March `XS_BATTLE_FIELD` (24 in March, whose enum still has the arcade states).
  3. `IsForbiddenQuest` 3232e510: `templet +0xe0 != -1 && != unit +0x3c && type != 1` (`QT_EVENT` = 1). At March's step 0 this is the port's `!= -1 && != 0`. **Stage 6 must replace the literal 0 with the step.** QT 9: 0x920 `[0] == 0xc`, 0x921 `[1] == 0x87`, 0x923 `[1] == 0x88`. The 9000 / 9010 test = March's `SERV_SKILL_NOTE` block.
  - Extra: `RemoveUnitQuest` 32331fb0 rebuilds for both complete and give-up, and only when the quest was in progress. The port matches.
- **Checked from the diff and the Q0a summary:** the chain rule (max time, minute branch, local midnight, the 4 rebuild sites, insert-unique); the week offset (`wday+1 > 3` is the same as June's `!< 4`; Wednesday -> 7); the December wrap; `<=` against time64; the list gates (QT 2 / 9 dedup, guild, QT 8 in the non-repeat branch, chain flag after the class-change loop); the `MakeSubQuestString` groups (0x2c / 0x30 count +0x80, 0x25 +0x80, 0x27 honor, 0x2b exchange, `42924` then `17688`).
- **Server-fed gates left out: agreed.** Of 208020..208060 / 205530 / 37100, none is in June's `FieldQuest.src.lua`. 31000 is, and its gate (`g_pData +0x52c == 1`) defaults to "not forbidden". The `+0x530` records are a server push.
- **Not re-proved, no change:**
  - the auto SQT 37 trailing newline (Q3 calls it a guess; cosmetic);
  - `GetClassUpQuestNoticeState` (no caller until Stage 9);
  - June's else-branch for a failed `localtime` (midnight(T) + days - 1), which the port reads as "available". It cannot be reached for a positive time.
- **Routed:** the Q3 note's "counts over 255" contradicted Q4's Done note. June's data has 8 SQT_NPC_HUNT sub-quests at 300-500, so it went into `Q4-review.md`. The Q4 review settled it: all 8 are orphans, and the field stays a UCHAR.
- **Kickoffs:** Q4b (keeps off Q3's runtime code), Q6 and Q10 still read right. Q6 can start now; Q10 still waits on the Q5a review.
- **Deploy.** `COALESCED` (peer build at b369047c), then `DEPLOY OK: X2_offline.exe is 25,378,304 B, dated 2026-10-19 13:32:18, md5 d6d5904ead27d67a1a21bf16e78a9b94, equal to X2/US_SERVICE/x2.exe (2026-10-19 13:32:11); built at main 6785efa9 + 5 uncommitted code file(s).` The exe no longer holds `[JUNE_QUEST] RUN chk` but still holds `chain rebuild` / `repeat purge`.

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

### Q4 - Done (2026-10-19, Opus 5.5 high)

Flags `SERV_IRUHADEV_JUNE_QUEST_OFFLINE` and the stand-in `SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN` (`Always.h`). Emulator only; no shared struct,
no save rung. MODS.md "offline completion of every sub-quest type".
- **User answers (2026-10-19):**
  - account quests are account-wide;
  - accept-time reward items are paid (split to Q4b, see the last item);
  - story quests open from their start NPC, with no stand-in;
  - type 48 and the exception list "follow June's".
- **Driven now, from March's server:**
  - SQT 32 `SUITABLE_LEVEL` (Room.cpp:1593 band: min - 2 <= level <= max + 2 at loading 100, id + difficulty row; UserQuestManager.cpp:2375 exclusions:
    tutorial / Henir / TC / Ruben). This drives 138 June sub-quests, including the 77 story quests' 41990 and Limit Break 1/5.
  - SQT 8 gets the server's rules (:2076: event / PC-bang quests count outside those four, and a step naming no dungeon also needs EXP; the others
    count anywhere but the Forest Gate and the tutorials).
  - A dungeon list with no id > 0 means any dungeon (`CheckValidDungeonID`). This corrects the kickoff's "a `{0}` set matches no dungeon": the
    server's loader drops 0, and event 43710 ("clear 5, Ruben excluded") lists `{0}`.
- **June rules at accept** (`CheckJuneRepeat`, ERR_QUEST_04):
  - QRT_WEEK: the next Wednesday 00:00 (`32322050`);
  - QRT_MONTH: the 1st, 00:00 (`32321d10`);
  - chain: midnight of (the chain's latest completion + AllowFewDays) (`3232caf0`; TestMinute needs code event 0x10, not mirrored). Only chain 1 (the 91
    story quests) is used by any quest, so story quests open one per day.
  - QRT_DAY now resets at 00:00 (US `QE_DAILY_REPEAT_HOUR` 0, XSLQuestManager.h:36; the emulator had the non-US 6).
- **Account quests (5):**
  - opened at character select (`CheckAccountOpen` = UserAccountQuestManager.cpp:361);
  - stored once per account under `unit_uid = -user_uid`, folded into each character's lists;
  - driven by the same hooks.
- **Labelled undriven** (`IsClearTypeUndriven`; orphan = no quest lists the sub-quest):
  - 0, 35 (server compiles it out);
  - 37 (15 orphans, dungeon 41130 has no row);
  - 38 / 39 (guild decision);
  - 40-42 / 46 (no data, no June parser case);
  - 43 (3 orphans, no item workshop offline);
  - 44 (quest 35363 also needs SQT_DUNGEON_RANK, which offline never ticks);
  - 45 (no PvP);
  - 47 (2 orphans, party-only).
  The census line must read `SQT coverage 0-48: 21 driven, 28 undriven, unclassified [none], both [none]`.
- **Stand-in:** SQT 48 (`[Army]` dailies 26600 / 26610) counts a clear only when the run paid EXP.
- **Decided, nothing to do:**
  - `m_bExpDungeonAndField` is on 36 orphan sub-quests only.
  - `m_cDifficulty` alone (35373, 74520) is read by neither June's client nor March's live server, so it is ignored.
  - `m_fRandomRate` / `m_iRandomGroupID`: the emulator still allows any (no rotation).
  - No June quest has `m_iBuff` != 0 or a title reward, and March's server pays neither.
  - No live sub-quest counts past 255, so `KSubQuestInstance` stays UCHAR. No ported reader needs the June int64s, so no shared-struct change.
  - Pushes 0x27b / 0x27d / 0x593 / 0x605 / 0x7a1 are not sent (March packets carry the same; no event ids until Q5a).
  - Quest 42030 (Limit Break 5/5, `QR_TRANSCENDENTAL_CLASS`) is refused at completion with ERR_QUEST_18 until Stage 6.
- **Split to Q4b** (`kickoffs/quest-system/Q4b.md`): `m_vecAcceptRewardItem` (7 quests) and `m_eExceptionDungeonID` need a parse in
  `X2QuestManager.cpp`, and Q3 was editing that file family during Q4.
- **Hygiene** (`flagcheck --worktree`, 7 files): `files: 0 PASS, 6 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token,
  0 encoding change(s)`; `RESULT: PASS` (OTHER = Always.h, Q3's define next to ours, not committed here).
- **Deploy:** `DEPLOY OK: X2_offline.exe is 25,350,656 B, dated 2026-10-19 11:57:00, md5 b5a84db238c64efbfc12bbb564b82b34` (built at main e0b19fc5 plus
  peers' uncommitted files, Q3's included); Q4's four log strings are in the exe.
- **Save:** unit 24 (Ara, Lv 47) was given quests 31110 (weekly, SQT 32 x4) and 26600 (daily, SQT 48 x3) for the play-test
  (backup `db_backup\els_db_20261019_113906_pre_sql.sql`).
- **Play-test PASSED 2026-10-19** (user, and the log of exe 642e712f, a later deploy that carries Q4):
  - `SQT coverage 0-48: 21 driven, 28 undriven, unclassified [none], both [none]`.
  - Account key -1 holds all 5 account quests, opened at an earlier select (37100 by a Lv 99 character). No unit-keyed copy exists.
  - Velder Bridge of Hope Normal (33012): `band 44..50 -> 1` on all 4 runs. 31110 counted 1 -> 4 / 4 and 26600 counted 1 -> 3 / 3. Both were handed in
    (items 78988 / 164547 / 164549).
  - Ruben 30000: `band 0..5 -> 0`, nothing counted.
  - 0 EXCEPTION; UNHANDLED = baseline `E_DISABLE_HB_CHECK_REQ`.
  - Cosmetic, for the review: a field load logs `suitable level: dungeon 0 has no DungeonData row` (the loading handler runs for fields too).

### Q4 - Review (2026-10-19, Opus 5.5 high)

**PASS with 3 fixes**, same flag (`SERV_IRUHADEV_JUNE_QUEST_OFFLINE`), emulator only.
- **Fix 1, transcription: `IsQuestCountedDungeon` was short.** It copied `Handlers_Skill.cpp`'s list, whose comment says the rest "are not
  enumerators here". They all are (one shared `DungeonEnum.h`). It now transcribes `XSLDungeon.cpp` itself:
  - `IsTutorialDungeon` :820, all 15 ids (new `IsQuestTutorialDungeon`; Q4 had only the 2 Elsword ones);
  - `IsRubenDungeon` :856, now with `DI_RUBEN_SECRET_HELL`;
  - `IsTCDungeon` :845, `DI_TRAINING_FREE <= id < DI_END`. Q4 stopped at `RAVEN_0 + 6`; the server range also takes in every event dungeon above
    39000 and June's 41xxx ids (41141 Sander / 41270-41271 Ranox secret, 41480-41500). `DI_END` is 41530 here and in June's `DungeonEnum.lua`; the
    March server's 41311 names the same live ids.
  - SQT 8's non-event branch (:2231) now uses the full tutorial list too.
  - Reachable effect: event quest 208480 (secret dungeons) no longer counts 41141 / 41271 (its 5 other dungeons still do); 43710 ("any dungeon")
    now skips tutorials and the TC range; 44010's 41480-41500 have no March DungeonData row. No type-32 sub-quest names a dungeon (all 12 live ones
    are "any dungeon").
- **Fix 2: field load.** `Handler_EGS_GAME_LOADING_REQ` skips `IsSuitableLevel` when `m_iDungeonID` is 0 and sets `m_bSuitableLevel = false`.
- **Fix 3: SQT 44's reason was stale.** "35363 also needs SQT_DUNGEON_RANK, which offline never ticks" was true only before `HasDungeonFilter`:
  35367 lists `{0}`, which now means any dungeon, so it ticks. The reason is now the real one: a June-server-only rule with no March server code.
- **Checked, matches** (no change):
  - `IsSuitableLevel` = Room.cpp:1615-1627 (loading >= 100, `GetDungeonIDAndDif`, slack 2 both sides, level from the packet). SQT 32 = UQM :2375-2392
    plus the :2520 counted branch. `SERV_THREE_COLOR_EVENT` is off for US.
  - SQT 8 = :2076-2236. `SERV_EVENT_TITLE_SUBQUEST` is off for US. The US-only id hooks (63780 relationship, 63962 party, 74580 / 74620 riding,
    85560 pet) name a quest June lacks or orphan sub-quests.
  - `HasDungeonFilter` = `CheckValidDungeonID` :2089, at the 5 sites. `OnVisitDungeon` keeps its plain `IsExistDungeonInSub`; no June
    SQT_VISIT_DUNGEON lists `{0}`.
  - `bHaveExp`: `m_iRewardEXP` builds up per kill (Handlers_Room.cpp:3300), is read before `QuestOnDungeonClear` (:4483) and is reset after it (:4607).
  - Ghidra: `32322050` week = Wed..Sat +(11 - dow), Sun..Tue +(4 - dow), from midnight of the completion date. `32321d10` month = the 1st of the
    next month (December goes to the next year). `3232caf0` chain = local midnight of (the latest completion + AllowFewDays x 86400). Its
    `*templet == -1 || == g_pInstanceData+0x120` test is templet +0x00 `m_iServerGroupID` against the current server group, and no June quest
    file sets that key, so every templet passes. June adds days as seconds, the emulator adds calendar days; they differ only across a DST change.
  - Account quests: `CheckAccountOpen` = UAQM :361-401 (skip in progress or completed, then accept with NUI_NONE). The server's account handler
    (:415-638) does not check unit type, class or the 30 cap, and the emulator's `Accept` does. All 5 account quests are EPIC / EVENT, so the cap
    never applies, and the play-test opened all 5. No live path writes with `m_nUnitUID` (only the `#else` arms), and the v21 rung runs before
    any account row exists.
  - `IsClearTypeUndriven`: the orphan counts are re-derived from June's sources, 37 = 15, 43 = 3, 47 = 2, and 35 / 40-42 / 46 have none. 48 is
    43650 in 26600 / 26610 only. Dungeon 41130 has a June row but no row in March's live `DungeonData.lua`. Only 42030 is transcendental (1001).
  - QRT_DAY: US `QE_DAILY_REPEAT_HOUR` is 0 (XSLQuestManager.h:36-40).
  - Counts over 255 (Q3 review's question): 8 SQT_NPC_HUNT sub-quests (36020, 203490, 203500, 203530, 203580, 204400, 204950, 206510; 300-500)
    and 2 PvP wins are all orphans. SQT 39 honor points are undriven (guild). Nothing reachable needs more than a UCHAR, so it stays one. Latent:
    `OnNpcDie` would wrap 255 -> 0 on such a sub-quest.
- **Not this flag, noted once:** `Handlers_Skill.cpp` `IsSkillUseCountedDungeon` (offline phase 6) has the same short list. All 22 June
  SQT_SKILL_USE sub-quests are orphans, so it is left as is.
- **Open question for the user:**
  - **Ultimate Road 4/5 (35363, 2nd jobs, Lv 63) cannot complete offline.** Its sub-quest 35365 is SQT 44 (5 runs that clear its SS-rank and
    Expert suitable-level steps together), a June-server rule. A stand-in would be needed, and it is yours to approve ([[design-missing-june-pieces]]).
  - **Should June's secret dungeons (41141 / 41271) count for event 208480?** March's TC range swallows them. June's server code is not available.
- **Hygiene:** review fixes `flagcheck --worktree` (3 files): `files: 3 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; 13 deleted lines =
  Q4's own flagged lines. Q4 commit `--range dbc0f686^..dbc0f686`: `files: 0 PASS, 6 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags),
  0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif
  without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 25,378,304 B, dated 2026-10-19 13:42:42, md5 9dfa205103632cedd696af90ebb2bf6b` (built at main
  fa78aa92 + the 3 review files + peers' Always.h / X2Unit.cpp); the new SQT 44 reason string is in the exe and the old one is gone.
- **Play-test (optional, log only):** load any field: no `dungeon 0 has no DungeonData row` line. Next load of a save: the census line still reads
  `SQT coverage 0-48: 21 driven, 28 undriven, unclassified [none], both [none]`.

### Q4 - Review stand-ins (2026-10-19, user answers)

The user's answers to the two open questions: "1: stand-in" (the strict reading was taken, as recommended) and "2: let Ranox secret count".
- **`SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER`**: SQT 44 is driven. In `OnDungeonClear`, a clear counts toward 35365 when the same run meets every
  other dungeon step of its quest (`IsTogetherRunMet`; types 5 / 6 / 7 / 8 / 32 / 48 with their own filter and rule; done state ignored). For 35363
  that means SS rank and inside the level band. It is a running total of `m_iDungeonClearCount` (5), as June's client draws it (case 0x2c = case 8's body).
  The census now reads `SQT coverage 0-48: 22 driven, 27 undriven, unclassified [none], both [none]`.
- **`SERV_IRUHADEV_JUNE_QUEST_STANDIN_RANOX_SECRET`** (needs `RANOX_PORT`, top-level at Always.h:3900; `DungeonEnum.h` is first read at X2Main.h:9,
  after KTDX.h): `IsQuestCountedDungeon` returns true for 41271 (`DI_RANOX_SECRET_HELL`) and 41270 (June's `DI_RANOX_SECRET_COMMON`) before the TC
  test. This covers SQT 32 and event SQT 8 (208480, 43710). Sander's secret dungeon (41140 / 41141) has no enumerator and no March DungeonData row.
- **Hygiene** (`flagcheck --worktree`): `TOGETHER`: `files: 0 PASS, 1 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`; `RANOX_SECRET`:
  `files: 0 PASS, 0 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS` (OTHER = each other's lines and a peer's Always.h edit); 0 deleted lines.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 25,389,568 B, dated 2026-10-19 14:15:23, md5 5b3e8252d9fd53faffc492745a69ed5f` (main d3c30f58 + peers'
  uncommitted files); both new `together` log strings are in the exe.
- **Save:** unit 25 (jtLordKnight, Lord Knight Lv 80) got 35360-35362 completed and 35363 in progress, with 35365 at 4 / 5 (backup
  `db_backup\els_db_20261019_135418_pre_sql.sql`).
- **Play-test PASSED 2026-10-19** (user + log, exe 5b3e8252): `SQT coverage 0-48: 22 driven, 27 undriven, unclassified [none], both [none]`;
  Ranox Secret Hell as unit 25: `suitable level: dungeon 41271 unit level 80 band 68..101 -> 1`, `35363 sub 35373 type 32 counted 1 / 1`
  (the Ranox stand-in), `together 35365: step 35367 ( type 6 ) not met this run ( ... rank 0 ... )` (the SQT 44 driver runs, blocked on
  rank as expected); no `dungeon 0` line on field loads; 0 EXCEPTION, UNHANDLED = baseline `E_DISABLE_HB_CHECK_REQ`.
- **Blocked on ranks (user, 2026-10-19):** every offline result is rank F. The emulator passes RT_NONE to the quest hooks and sends RT_F
  to the screen. So SQT_DUNGEON_RANK, and with it this stand-in for 35363, cannot complete until Phase Q4c. Q4c was added for that.

### Q4b - Done (2026-10-19, Sonnet 5.5 high)

Flags `SERV_IRUHADEV_JUNE_QUEST_OFFLINE` (accept items) and `SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN` (exception list). Log tag `[JUNE_QUEST] OFF`.
Emulator plus two parser keys in `X2QuestManager.*`; no shared struct, no save rung. Q3's runtime code was not touched.
- **`m_vecAcceptRewardItem`** (7 quests: 30130 epic, 30150, 30160, 30170, 30180, 30340, 35203; one item x1 each, 111371 / 111373-6 / 92070 / 92351, all in item.kim):
  - parsed in `AddQuestTemplet_LUA` next to Q1's keys into `QuestTemplet::m_mapAcceptRewardItem` (March's server shape, `XSLQuestManager.cpp:121-141`: both fields non-zero, first entry of an id wins);
  - `CX2OfflineQuest::Accept` pays it as its last refusal (`UserQuestManager.cpp:694-765`): `ERR_QUEST_24` when the item is already held (inventory; the emulator has no bank), `ERR_QUEST_25` when `HasRoomFor` fails, an item without a templet is skipped, nothing is inserted unless the quest is then taken;
  - the changed slots wait in `m_mapAcceptSlots` and ride `EGS_NEW_QUEST_NOT::m_vecUpdatedInventorySlot` on the REQ path and in `PushNewQuest` (auto-open and account auto-open);
  - **added, same server code:** a give-up deletes every copy of the accept items (`GSUserGameCommon.cpp:1835-1855`, `RemoveAcceptItems`), single and gather. The rows go in the ACK as the server does; March's client handler never reads them, so the bag may show the item until the next character select.
- **`m_eExceptionDungeonID`** (6 sub-quests: 42370, 42410, 43540, 43640 = SQT 32; 43670, 43710 = SQT 8): parsed into `ClearCondition::m_setExceptionDungeonID` (ids > 0, in the shared 8 / 32 / 48 case body).
  - **The list holds dungeon id + difficulty** (`iDungeonID + cDifficulty`, the packed number `IsExistDungeonInSub` compares: 30080 / 30081 / 30082 = Ruben Ruin of Elf N / H / E; 43640's 100xxx / 110xxx are the hero and TC ids).
  - Applied in `OnDungeonClear` right after the dungeon filter and before the type switch ("a listed dungeon never counts"), and on the other steps in `IsTogetherRunMet`. Each exclusion logs `[JUNE_QUEST] OFF <q> sub <s> not counted: dungeon <id>+<diff> is in its m_eExceptionDungeonID`.
  - Account event 43710 lists `{0}` (any dungeon) plus Ruben 30000 / 30080-92 and 41450 (`DI_EVENT_HIGHSCHOOL`).
- **Hygiene:** `flagcheck --worktree --files <5 files>`: OFFLINE `files: 1 PASS, 1 PASS~, 0 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`, `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS`; OFFLINE_STANDIN `0 PASS, 5 OTHER, 0 FAIL, 0 ERROR`, same zero counts, `RESULT: PASS` (OTHER = Q10's STORY hunks sharing `X2QuestManager.*` and earlier Q4 flags in the emulator files).
- **Deploy:** `DEPLOY OK: X2_offline.exe is 25,446,400 B, dated 2026-10-19 18:00:54, md5 36fbf5def5d982244a330e179b7837dd` (main 14420b7d plus peers' uncommitted files). The five new log strings are in the exe.
- **Save for the play-test:** unit 21 (eresisu, Lv 77) was given item 111371 (backup `db_backup\els_db_20261019_180120_pre_additem.sql`) to show the refusal.
- **Play-test:** see the hand-off in the session; log tag `[JUNE_QUEST] OFF`. Review kickoff `kickoffs/quest-system/Q4b-review.md`.

## Phase Q4c - Dungeon result ranks offline

**Run with:** Opus 5.5 high, then a review. Flag `SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK`. Sub-tag `RANK`. Kickoff `kickoffs/quest-system/Q4c.md`.

- Compute the combo / technical / damaged / time / total ranks as March's room server does (`CenterServer/ResultProcess.cpp` ~1696-2040),
  with its own `ServerResource/US/ResultData_new.lua`. That is server Lua the user packs into data036; load it the shipped way and degrade
  visibly. Bind the objects its functions read, with the server's method names.
- Feed the ranks to `SendDungeonResultData` (the result screen) and the total rank to `QuestOnDungeonClear` (it was RT_NONE). Pay the rank EXP
  bonus if March's server pays one.

**Done gate:** a good run shows a rank above F on the screen and in `[JUNE_QUEST] RANK`, and a run where the unit died shows F. Unit 25's
35363 then counts 35365 to 5 / 5 with an SS clear.

### Q4c - Done (2026-10-19, Opus 5.5 high)

Flag `SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK` (needs `OFFLINE_MONSTER_GRADE`, `OFFLINE_SECRET_POISON`; `#error` guards), emulator only.
- **Server flow (the reference).** `Result_DUNGEON` (ResultProcess.cpp:1355) is the dungeon result; `Result_DefenceDUNGEON` (:2820) is the same four
  calls for the Defence dungeon (unreachable offline); `LeaveRoomResultRank` (:3503) only logs a leaver's ranks to the DB. The Lua is
  `ResultData_new.lua` (CnSimLayer.cpp:371, `SERV_DUNGEON_RANK_NEW`). Per axis, C++ calls the function and reads `GetPercent` / `GetRank`, which
  `SetResultData` set (:1951-2007; `DUNGEON_TECHNICAL_RESULT` returns its score, not a rank, and that return is ignored). Total = Lua
  `DUNGEON_TOTAL_RANK_RESULT`, clamped `<= RT_NONE -> F`, `> SS -> SS` (:2014-2023). Dead unit: all F, no rank bonus (:1696-1703). Rank bonus EXP
  = `(EXP + party EXP) * min(sum of the 4 percents, 0.15)` (:2046-2063). The `SUB_STAGE_*` pair feeds only `BadAttitudeManager` (party vote kick).
- **Bindings** (`ResultProcess.cpp:54-101`): `ResultProcess` with the 3 `Add*` calls the file makes at load (party bonus rates; logged, unused
  by a party of one); `pUnitInfo` = `GetComboScore`, `GetTechnicalScore`, `GetBaseHP`, `GetTotalGivenDamamge`, `GetTotalAttackedDamage`,
  `GetTotalRecoveryHP`, `GetUsingKindOfSkill`, `SetResultData`; `pMonsterManager` = the 5 `Get*DieCount`. All fields come from the client's
  `KDungeonPlayResultInfo` (as KRoomUser holds it, RoomUser.cpp:421) except `GetBaseHP` = game stat HP (RoomUser.h:128) = `MakeGameStat`.
  `iEndNumMember` 1, `iPartyTotalGivenDamage` = own given damage. `DUNGEON_ID` / `RANK_TYPE` already exist in the client state (DungeonEnum.lua /
  Enum.lua); the file reassigns `RANK_TYPE` with the same values.
- **Die counts** (DungeonRoom.cpp:6240-6244): not NO_DROP, drop die state, `m_bIsNPC`, active; any attacker. Type = `GetMonsterTypeFactor`
  (grade 2 middle boss, 1 boss, else normal); attribute monsters (low / high elite) are never placed offline.
- **Play time** (`ComputeTotalPlayTime` :2632): start-game clock less the room's loading time, which the secret-poison port already keeps as
  `m_fPoisonLoadingSec` (same LOADING_TIMER, same start / end sites). It now feeds the screen, the time rank and the clear-time sub-quests.
- **Quests move** to `SendDungeonResultData`, before the result is sent, as GSUserDungeon.cpp:601-719 does: real total rank, `m_nSec`,
  `m_nDamaged` (was RT_NONE, play time with loading, damage 0); `bHaveExp` from `m_mapHaveExpInDungeon` (`m_iRewardEXP` is zeroed by then).
- **Rank bonus** is paid after the result (`ERM_UPDATE_DUNGEON_UNIT_INFO_NOT` order, DungeonRoom.cpp:1767-1770): `ApplyDungeonReward`, level-up push,
  then `EGS_UPDATE_UNIT_INFO_NOT` again with the item box (the client handler is idempotent).
- **Degrade:** file missing -> `[JUNE_QUEST] RANK ERROR 'ResultData_new.lua' not found ...` once, every rank F. No play result -> F, logged. A Lua error
  in one axis -> that axis F, logged. `lua_tinker::call`'s error path removes a stack slot below it, so each call is padded and `lua_settop` restores.
- **Not changed, noted once:** the emulator still pays the clear bonus and records the clear for a unit that died in a won run (server: neither).
- **Hygiene** (`flagcheck --worktree --files`, 7 files): `files: 3 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 2 FAIL, 0 ERROR`;
  `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
  The 2 FAIL rows are the new files' `#pragma once` / `#include "stdafx.h"`. `X2Lib_2010.vcxproj` +2 lines.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 25,409,024 B, dated 2026-10-19 15:08:51, md5 b304f50635a14b1de34766fd916bc0c4` (main c152c92c + this
  phase's files); the `RANK combo`, `RANK bonus` and `ResultData_new.lua` strings are in the exe.
- **Data (user asked, 2026-10-19):** `ResultData_new.lua` staged byte-exact as BOM source in `data036_kom` (manifest 2, ledger row tier SRV, `8590148f`);
  `PACK OK data036` (+1 entry), `COPY OK data036: ... 13950416 bytes, 263 entries, md5 086cf5fc5a330bcf8dffc4fb54c8fb32`. User owes the play-test.
- **Play-test 1 (2026-10-19, exe 6efbeb9a):** check 1 PASSED (user + log): unit 12 in Atlas Development Expert, `RANK combo F tech C damaged SS
  time A total B`, screen total B; level 99 so the 0.04 rate paid 0 EXP. Given damage arrived as `-1402157266` (the client's int counter wrapped;
  only the Lua's unused IndexC reads it). Check 2 could not run: a solo death never ended (below).
- **Death fix, own flag `SERV_IRUHADEV_OFFLINE_DUNGEON_CONTINUE_END`** (user: "no countdown ... need investigate"): the countdown is the client's
  (`NoticeContinue`, 10 s), the end is the room's, and the emulator had none. Now: die REQ reserves the end (15 s, RoomUser.cpp:1371); X
  (`EGS_SKIP_CONTINUE_DUNGEON_NOT`, was UNHANDLED) ends at once (DungeonRoom.cpp:8943); the play-status beat ends it on timeout (Tick :250); the
  cash-revive pause = 11 s window (:1398); a revive cancels (:8939). The end = `EGS_END_GAME_NOT` TN_MONSTER through `Handler_EGS_END_GAME_REQ`'s
  body, split after its ACK as `EndDungeonGame`. flagcheck worktree `files: 2 PASS, 2 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, 0 deleted;
  `DEPLOY OK: X2_offline.exe is 25,415,168 B, dated 2026-10-19 16:54:59, md5 927febd35036f48435f7821e0bc95fea`. User owes check 2 again.
- **Play-test 2 PASSED (2026-10-19, user + log, exe cad1ab82):** unit 12 died in AD Expert, `continue: the run ends in 15s`, X ->
  `continue skipped: the run ends now as a fail ( TN_MONSTER )`, `dungeonID=33330 FAILED in 20s`, `RANK combo F tech F damaged F time F total F
  ( the unit died, ResultProcess.cpp:1696 )`; 0 EXCEPTION, UNHANDLED = baseline only. Q4c done; 35365 -> 5 / 5 stays optional (needs an SS total). Review next.

### Q4c - Review (2026-10-19, Opus 5.5 high): PASS, no transcription error, 1 comment fix

Against March's server (no June code involved), every cited range re-read.
- **Result_DUNGEON** (ResultProcess.cpp:1355-2063): the four axis calls in order with the server's argument lists (`iDungeonID + cDifficulty`,
  the monster manager, `iEndNumMember`, `iPartyTotalGivenDamage`; time takes `kUnitInfoNot.m_nSec`), each read back by `GetPercent` / `GetRank`
  (:1951-2007); the total, its clamps (:2014-2023), the 0.15 cap (:2046-2048), `m_nRankBonusEXP` (:2063): all match. The dead branch
  (:1696-1783) `continue`s past the rank code (all F, no rank bonus): match. A living unit is ranked win or lose (no `bIsWin` gate): same in the port.
- **Bindings** (:54-101): the server binds 7 more (`GetUnitClass`, `GetKillNPC`, `GetDamageNum`, `GetTotalSumNpcHP`, `GetCommandTechScore`,
  `GetSkillTechScore`, `PrintLuaLog`); the file names three of them only in `--` comments (:163, :234, :313, :317-318) and the rest nowhere. Every
  call it makes is bound to the server's field (RoomUser.h:128, :251-262, :286; DungeonMonsterManager.h:65-69). All four axis functions reach
  `SetResultData` unconditionally (:139, :205, :293, :416), so `m_bSet` false means a Lua error only; there the server would read the previous
  axis's percent / rank (KRoomUser members, RoomUser.h:677-678) and the port gives F, a degrade path only. `RANK_TYPE` is reassigned with the same 9
  values as data036 Enum.lua:5088; a `DUNGEON_ID[...]` name missing from the client is nil and compares false, no error.
- **Die count** (DungeonRoom.cpp:6171-6244): not NO_DROP, `IsDropDieState` (:2576-2595), `m_bIsNPC`, active, no attacker test: same gate (the
  port adds "dungeon, known templet"; the server returned earlier without one). `GetMonsterTypeFactor` (XSLUnitManager.cpp:238-269); MG 1 / 2 agree
  (XSLUnitManager.h:2876-2877, X2GUNPC.h:234-235). The counts clear at StartGame / EndGame / OnCloseRoom (DungeonMonsterManager.cpp:21 / 29 / 37);
  EndGame runs from CheckResultSuccess (Room.cpp:580), after the result, so ClearPlayRun at game start matches. Inherited, not Q4c's: the emulator has
  no IsMonsterAlive gate, so a repeated die REQ for one UID would count twice (its reward path already does).
- **Play time:** `ComputeTotalPlayTime` at EndPlay (:1089-1095, :2632-2634) = TM_PLAY (StartGame, Room.cpp:661) less m_fLoadingTime (StartPlay
  :1065-1070, each stage load :3643, ended at the sub-stage start :2189-2190). m_fPoisonLoadingSec uses the same three sites (Handlers_Room.cpp:2100
  via StartSecretPoison, called for every dungeon at :1738; :2590; :2702), none gated on a secret dungeon. Taken at END_GAME as EndPlay takes it.
- **Quest move:** the GameServer gates on `m_bIsWin` and a dungeon room only (GSUserDungeon.cpp:435, :452; no `m_bIsDie` test), then the quest hook
  (:601), title hook (:649) with `m_cTotalRank`, `m_nSec`, `m_nDamaged`, `m_mapHaveExpInDungeon`, then the send (:719): same gate, order and figures.
  Nothing left in `Handler_EGS_END_GAME_REQ` used the old call. Both callers of `SendDungeonResultData` (:1783 play result, :4795 result state) run
  with the room open; `m_bResultDataPending` keeps it to one.
- **Rank bonus payout:** the server banks it in `ERM_UPDATE_DUNGEON_UNIT_INFO_NOT` (ResultProcess.cpp:2172, after the result DungeonRoom.cpp:1767-1770).
  The second `EGS_UPDATE_UNIT_INFO_NOT` is idempotent on the client: `m_bGameEnd`, item box reset and refilled (X2StateDungeonGame.cpp:3217-3290), unit
  `Reset`, `m_GetEXPInGame` zeroed (X2Game.cpp:9740-9760). Level-up: PushLevelUp + QuestOnLevelUp, as the clear bonus does.
- **lua_tinker (the fix):** the comments at X2OfflineDungeonRank.h:123 / .cpp:233 (and the Done note's Degrade line) said `call`'s error path removes
  a slot below it. That is KNCSDK's lua_tinker, the server's. The client compiles LuaCommon/lua_tinker.h (KTDX.h:1213), whose every overload pushes nil
  after the error pop and on the not-a-function path (:1015-1300), so the stack stays balanced. The pad and `lua_settop` are kept (harmless); both
  comments now say so. A failed total returns `pop<int>` of nil = 0 and clamps to F, as on the server.
- **Hygiene** (`flagcheck --range e2b8f21d^..e2b8f21d`): `files: 3 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 2 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token,
  0 encoding change(s)`; `RESULT: FAIL` from the 2 expected rows only (the new files' `#include "stdafx.h"` / `#pragma once`). No OTHER rows.
- **Deploy** (comment-only change; the build carries peers' uncommitted files too): `DEPLOY OK: X2_offline.exe is 25,420,288 B, dated 2026-10-19
  17:39:22, md5 82ec74d235280a818e541e214ee4a713` (main f743a5e2 + 18 uncommitted code files).
- Not in this review: `SERV_IRUHADEV_OFFLINE_DUNGEON_CONTINUE_END` (`4e3fefed`), its own flag, play-tested. User owes nothing.

## Phase Q5a - Quest UI: window, tabs, list / detail / quick, accept / complete / give-up

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_UI`. Sub-tag `UI`.

- Port the `CX2UIQuestNew` rows Q0b targeted Q5a, whole: June's tabs (Story / Guild / Request / Scenario / Event), list / detail / reward builder `323e1bf0`,
  quick quest, skip-epic, the handlers.
- Adopt June's `DLG_UI_NEW_Quest_*` together with the code, if Q0b says they are not in yet.
- Leave the scene calls at their March behaviour (Q5b).

**Done gate:** the window opens on every tab, accepts and completes a quest, and gives one up, with June's layout. 0 EXCEPTION.

### Q5a - Done (2026-10-19, Sonnet 5.5 high; play-test PASSED 2026-10-19, review next)

Flag `SERV_IRUHADEV_JUNE_QUEST_UI` (Always.h, needs `JUNE_QUEST_LOAD`). Log `[JUNE_QUEST] UI`. Deploy: `DEPLOY OK: X2_offline.exe is 25,383,936 B, md5 2b7944e2f16d3906e1d14db3280cb741, built at main 74719292 + 9 uncommitted code files` (a peer's build compiled these edits; the Q5a strings are in the exe).
- **Design.** June's bodies live in the new `X2Lib/X2UIQuestNewJune.cpp` (June* members; `X2Lib_2010.vcxproj` lists it). `X2UIQuestNew.cpp` only gained one early-return dispatch per replaced function plus a few in-place cases, so flag-off is March. Header: enums (`QUM_GUILD_QUEST` 5, `QUM_STORY_QUEST` 6, `QSS_COMPLETE_STORY` 6, `QSS_CHAIN_WAIT` 7, `QTS_GUILD/STORY/TRANSCEND` 7-9, `RTS_WEEK/MONTH`, UQNCM 41038-41047), 10 members, 25 declarations.
- **Ported (June address).**
  - Tabs and modes `323c9e80` / `323ca520` / `323c9250`: the request tab always lists the board, because June runs `MODIFY_ACCEPT_QUEST`. March's global define is NOT turned on: `X2StateMenu` dereferences a `Quest_New` control unguarded.
  - List data `323cf570` `323cf250` `323cf3e0` `323d0770` `323d5650` `323d4910` `323d4ae0` `323cab40` `323ca300`: story first/next quests from the Q1 story index, chain wait from Q3's `GetChainQuestStartable`.
  - List UI `323d9400` (7 tabs) `323dfa80` `323d80d0` `323d8be0`: June picture numbers 0-23, done/going marks, "Lv" and wait text.
  - `323ccbc0` buttons + `323ca880` skip rule + the skip flow (`323e1bf0` 0xa055/0xa056 = a plain QUEST_COMPLETE_REQ after a message box).
  - `323ce6f0` detail title (own picture map, event-NPC face), `323cc280` face keys (Edel, Ignia, Hugo, Pane, Story, Trans ...).
  - `323e4930` talk-ACK tab rule; `323e4aa0` OK 154 dropped; `323d2b40` board default; `323e57e0` quest 11000; `323e5c10` no relation refresh (the Atlas E1c citation fixed); `323cdfe0` play-together filter; category/scroll cases of `323e1bf0`; Enum.lua + the 10 UQNCM ids.
- **DLGs (data034, `adopt-june`).** List and Detail replaced; `DLG_UI_Quest_Clear` replaced (dead reader); Quick, Quest, Restor, Request, Guide, Guide_Quest01/02 were already byte-equal to June (Q0b's "never staged" was wrong). `PACK OK` data034 + data036 (Enum.lua). `COPY OK` data034 md5 0da59f517881d2a7d9152e68d6ee1254, data036 md5 937030834a15a552dd84ab3b54d29df6.
- **Not ported, owner.**
  - Scene calls (Q5b). Movie tab `323ca6d0`/`323dfc00` and the complete-REQ clear movie `31d7a240` (Q8; the movie UQNCM ids are logged and ignored).
  - Friendship manager hide (Stage 18; `JuneIsFriendshipQuest` = false, what June returns for an empty manager).
  - Story helper object `326ab210` family and the story remind button (Q3/Q4; the button stays hidden). Guild reward rows `323cdbc0` (guild quests are not loaded offline, Q1).
  - Tutorial guide `31d70b00`/`31fc5920`, the menu mark refresh (state vtable +0x104) and the New mark (Stage 9). Class-built sound names in `PlayQuestCompleteSound` (March's file names stay). Complete-REQ transcend guard + movie (`323d24f0`, Stage 6/Q8).
  - The four June-only server pushes `0x27b/0x27d/0x593/0x605` and the NEW_QUEST_NOT +2 int32 / KQuestInstance int64 fields: nothing sends them, Q4 decides, so **no shared struct and no ServerDefine.h change in Q5a**.
  - `UPDATE_QUEST_NOT` stays in `CX2State` (June moved the same loop into this window; behaviour equal; widening the count to short is Q4's struct call). Quests 208020-208060 (premium-manager gate): not in June's FieldQuest, nothing to gate.
- **JUNE_BUGS 341-343:** available-list comparator, story flags never reset, scroll-down at 14 rows.
- **Open points for the review.** `323ccbc0`'s register-lost buttons were assigned by meaning (see Q5a-review). GET_STRING ids 40047 / 43475 may be missing from the string table (empty text then). June state numbers 0xe/0xf/0x15 are mapped to March's `XS_VILLAGE_MAP` / `XS_BATTLE_FIELD`; June's 0x17/0x1a game states have no March name and are left out. The emulator must accept a QUEST_COMPLETE_REQ for a skipped epic (Q4).
- **Play-test fix (user, 2026-10-19).** First test: June's scenario-movie controls (12 `Button_Movie_N` / `NewTexture_N`, `Village`, `g_pStatic_HorizonLine`, `RadioButton_Area`) drew over every tab. June hides them with `323ca6d0`, which I had left with the movie tab. Ported as `JuneHideMovieTab` (called at window creation `323d0ca0` and at the end of every `JuneResetQuestUI`), commit `6785efa9`, `DEPLOY OK` md5 2d6615c5116fa2c18e2bff380dde9080. Re-test: user PASSED (all tabs, accept / complete / give up); log: `[JUNE_QUEST] UI tab 5/6`, quest 11010 completed + relation refresh, 0 EXCEPTION, ErrorLog 0 lines.
- **Hygiene.** `flagcheck JUNE_QUEST_UI --worktree --files ...`: `X2UIQuestNew.h` PASS (+71); `X2UIQuestNew.cpp` OTHER (+170, 0 deleted: two inserts sit inside the Elysion `#ifdef`); `Always.h` OTHER (define); the new `X2UIQuestNewJune.cpp` FAILs only on its first line `#include "StdAfx.h"` (a new file, same shape as `X2GFXEventScene.cpp`). 0 deleted, 0 non-ASCII, 0 EOL mismatches.

### Q5a - Review (2026-10-19, Opus 5.5 high): PASS with 2 fixes; play-test PASSED 2026-10-19 (user: the red developer suffix shows on the detail title)

Same flag `SERV_IRUHADEV_JUNE_QUEST_UI`. Second reading against Ghidra (raw disassembly where the decompile lost registers).
- **Matched, no change.**
  - List builders `323d5650` (vector targets settled by disasm `323d5a60-323d5d70`: epic `+0x4c8`, event `+0x458`, guild `+0x488`, story `+0x498`, ongoing `+0x438`,
    quick `+0x4e8`; an ongoing story quest goes to story and ongoing), `323cf570`, `323d0770`, `323d4910`, `323d4ae0`, `323cab40`, `323ca300`, `323c9250`, `323c9e80`.
    - States 0..7 and the tab-priority flags (`+0x736..+0x739`, `+0x758/759`).
    - The story progress map: insert-if-absent, erase or set to 1.
    - The quick-quest push condition.
    - June skips a completed story quest whose group is missing from the story index. That case never happens, because the index holds every QT_STORY quest by its group (Q0a).
  - `JuneResetQuestList` `323d9400`: all 7 modes, the per-mode lists, the clamp, mode 0 -> 1. `323dfa80`.
  - Slot rows `323d8be0` / `323d80d0`: picture numbers match June's `Static_QuestList0` order (0 usual .. 22 MONTH, 23 Black). The shade colour is 0.7058 and the ellipse offset (150, 50), both read from the exe. The width is 208, because `ELLIPSE_GLOBAL` and `QUEST_LIST_TITLE_CUT_STRING` are both on in US. The done / going marks and the open rules also match.
  - `323ccbc0` buttons from the raw disassembly (`323ccbc0-323cd490`, jump tables `323cd454` / `323cd474`): every assignment is right.
    - Admin is `[esp+0x18]`. The game states for the tooltip are June 9 / 0xb / 0x17 PVE_GAME / 0x1a PHOTOZONE_GAME, and March has no twin for the last two.
    - `323ca880`, and the skip flow `0xa055` / `0xa056` (string 43475, ok id 0xa056, `323d24f0` with an empty map) also match.
  - The in-place edits, each against June:
    - category select `0xa03e` (mode 1 keeps the top row);
    - scroll-down `0xa030` (rows - 1 < 14);
    - GIVE_UP `154` (`323e4aa0`);
    - COMPLETE_ACK 11000 (`323e57e0`);
    - board default (`323d2b40`);
    - tab cases `0xa044/45/53/54`.
  - June state numbers (funcmap row `31d6fd60`): 0xe village, **0xf = June-only `XS_GUILD_AGIT_GAME`**, 0x15 battle field. March's square is not in June's set.
  - Strings: 40047 "Waiting for Action" and 43475 are in the live `general.ess` (45,160 rows).
- **Fix 1: the developer title suffix** (`323ce6f0` @323ce89a, auth >= 2): `#CFF0000(id, N/D/R/W/M, T/F, level, S/A)`. March had it (`SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER`, dev only); Q5a dropped it. Added to `JuneUpdateDetailQuestTitle`.
- **Fix 2: NEW_QUEST_NOT** `323e4cb0` has no forbidden-quest give-up. June calls `IsForbiddenQuest` only from the June-only push `0x27b` `323e05f0` (xrefs).
  March's `SERV_GATHER_GIVE_UP_QUEST` block now sits in the flag's `#else` (`X2UIQuestNew.cpp`).
- **Owners corrected / added.**
  - The story Remind button (`323ccbc0` case 6) needs the story-book singleton `326ab210`. It belongs to **Q10**, not Q3/Q4 (kickoff amended).
  - NEW_QUEST_NOT's epic write `*(g_pData+0x274)+0x70` (@323e4e54) goes to **Q5b** (kickoff amended).
  - The admin complete REQ `323d1fc0` has the same clear movie / transcend guard as `323d24f0` (Q8 / Stage 6). Both also call `CX2UIProfessionManager` `32396dc0` / `3239d810`, which June alone has (Stage 9).
  - `UpdateNewQuestNotice` `323d4390` (Q0b: "port whole") was never compared by Q5a. Q6 (running) adds its gate there; **Q6's review compares the whole body**.
  - The detail case's guild reward `323cdbc0` stays Q1's.
- **Hygiene.** `flagcheck JUNE_QUEST_UI --range 5383d636^..HEAD` showed the expected results:
  - the new file FAILs on `#include "StdAfx.h"` only;
  - `X2UIQuestNew.cpp` is OTHER (inside the Elysion `#ifdef`);
  - 0 deleted lines, 0 non-ASCII, 0 EOL mismatches.

  The review edits give `--worktree`: `X2UIQuestNewJune.cpp` PASS (+43 -2: two comment lines reworded), `X2UIQuestNew.cpp` +5 (the other +7 are Q6's `JUNE_QUEST_GATE`, not in this commit).
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,389,568 B, dated 2026-10-19 14:15:23, md5 5b3e8252d9fd53faffc492745a69ed5f, equal to X2/US_SERVICE/x2.exe (2026-10-19 14:15:15); built at main d3c30f58 + 21 uncommitted code file(s)` (COALESCED: a peer's full build at 14:15:28 compiled these edits; `X2UIQuestNew.obj` / `X2UIQuestNewJune.obj` 14:13 are newer than the 14:03 sources).

## Phase Q5b - Quest UI: scenes, clear window, scene info

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_SCENE`. Sub-tag `SCENE`. Needs june-gfx G3.

- `323cb2e0` and its callers, on G3's `CX2GFXEventScene`.
- The per-class `Quest_Event_Scene_Info` choice.
- The Lua-scene fallback for `m_wstrStartScene` / `m_wstrEndScene`.
- `m_vecAdditionalSceneQuestID`.
- The clear window `323d2c60`.

**Done gate:** accepting Elysion quest 22329 plays `Quest_22329_Start` (GFx). A pre-Elysion quest with a Lua scene still plays that scene.

### Q5b - Done (2026-10-19, Sonnet 5.5 high; play-test PASSED 2026-10-19, review next)

Flag `SERV_IRUHADEV_JUNE_QUEST_SCENE` (Always.h, needs `JUNE_QUEST_UI` + `JUNE_GFX_EVENTSCENE`). Log `[JUNE_QUEST] SCENE`. `DEPLOY OK: X2_offline.exe is 25,414,144 B, dated 2026-10-19 15:19:25, md5 6efbeb9aa7fec6efe7d770e0a1a8168f` (strings in the exe checked). First build failed on a missing `#include "X2GFXEventScene.h"` in `X2UIQuestNew.cpp` (fixed, second build OK).
- **Scene starter** `CX2UIQuestNew::JunePlayQuestScene( quest, mode )` = June `323cb2e0` (read from the disassembly): clears `g_pInstanceData +0xaf8/+0xafc`, templet test, row lookup `323cb190`, then per mode: ment register (`3226ac80` = `CX2GFXEventScene::RegisterMent`, map::insert) -> `Play( name, end frame, 0 )` -> quest BGM, all inside "name non-empty". Mode 1/2 then plays every `m_vecAdditionalSceneQuestID`'s start and end (names / ments cleared, frames not reset, both registered then both played). Row lookup: DEFAULT_* first, then the select unit's TYPE row (`CHAR = UNIT_TYPE.*`) overrides start / end only when its own name is non-empty.
- **Quest BGM** `3232df20` / `3232e000` -> `CX2QuestManager::JunePlayQuestSelectBGM` / `JuneRestoreQuestSelectBGM` (world BGM name saved, quest's played through `CX2World::PlayBGM` = `327fc950`); the restore is called from G3's `End` (`@3226b734`). JUNE_BUGS 353.
- **Call sites** (March Lua scenes stay in `#else`): Remind (June case `0xa046`, mode 2), NEW_QUEST_NOT (mode 0; state 5 defers via the new `CX2InstanceData` pair; every epic quest sets the GFX scene's `+0x70`, JUNE_BUGS 352), QUEST_COMPLETE_ACK and ALL_COMPLETED (mode 1). The deferred scene plays at the end of StateChange's village and battle-field blocks (`31d88bf0` @31d896bd-31d896fb; the tutorial-guide skip has no March object). March has no `ELSWORD_NEW_BEGINNING` here, so "state 5" is the `XS_SERVER_SELECT` slot: whether a NEW_QUEST_NOT ever arrives there offline is for the review.
- **No Lua fallback, re-proved.** `june2017_re/dispsweep.py 1d8 1f4` + the 64 callers of `GetQuestTemplet` (`32322310`): 8 functions have both; each `+0x1f4` is the device manager / a vtable slot / an immediate and each `+0x1d8` a UI member or the parser's own store. `323321f0` stores and `32349b20` copies the strings, nothing reads them. Following June, a quest with no row plays nothing and the log names it.
- **GFX data gap (not a bug).** The shipped scene XMLs name only Rose (439), Ain and the Elysion chain 22329-22346; rows for Elsword..Chung scenes have no GFX movie, so `Play` returns quietly as in June. Testable in March: 22329-22337 (`Quest_223xx_Start/End`).
- **Clear window** `323d2c60` read whole against `OpenQuestRewardWindow`: identical (3 + 3 slots, unit type / class filters, dummy ints id / socket / period, slot-1 auto check) except June's gate on a dialog-manager byte `+0x111` that nothing writes (ctor zeroes it): not ported. No code change; `DLG_UI_NEW_Quest_Clear` = June's `ljconv` (data034 `PACK OK`, backup `kom_backup_stage_june\20261019_150631`). **Not copied yet: ask the user for `/x2-kom-packed copy data034`.**
- **Epic effect hook:** G3 left `m_mapMent` "writer-less" (comment in `X2GFXEventScene.h` stays; the writer is `RegisterMent`).
- **Hygiene** (`flagcheck JUNE_QUEST_SCENE --worktree --files` 11 files): `8 PASS, 1 PASS~, 0 DEFINE, 2 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (Always.h OTHER = another phase's hunk; `X2UIQuestNewJune.cpp` OTHER = inside the `JUNE_QUEST_UI` file). Rows: `funcmap.tsv` 10 `(Quest Q5b)`, JUNE_BUGS 352-353.
- **Save (for the play-test, snapshot `db_backup\els_db_20261019_151944_pre_sql.sql`):** unit 12 reyaa: 22328 ongoing (sub-quest 22337 open), 22330 and 22331 complete.
- **Play-test (user + log, unit 12 reyaa).** Admin-complete is `IGNORED` by the emulator (no `EGS_ADMIN_QUEST_COMPLETE_REQ` handler; pre-existing), so 22328's sub-quest was set cleared in the save and the normal Complete button used: `QUEST_COMPLETE_REQ` -> clear window (June DLG) -> `SCENE quest 22328 mode 1` -> `NEW_QUEST_NOT` 22329 -> `SCENE quest 22329 mode 0` -> `Play Quest_22329_Start ( Elysion7.swf )`, 25 frames, ended cleanly; Remind (mode 2) on 22328 played `Quest_22328_Start` twice (forced End by the next Play). 0 EXCEPTION. Side effect of the save setup: 22331 complete made the emulator auto-open 22332 at login.
- **User observation, not Q5b's:** the software cursor draws IN FRONT of the GFX scene; the user wonders whether June draws it behind. G3 left the cursor to R4f (`CX2Cursor XCS_EVENT_SCENE`, June `3226c250` / `31ce50d0` / `32868250`); the render order of the Scaleform layer vs the cursor has not been read in June. Routed to a new phase, `/x2-phase june-gfx G6` (Opus; map June's draw order, port only if it differs; the `CKTDXCursor` class stays R4f's).

### Q5b - Review (2026-10-19, Opus 5.5 high): PASS, 1 fix, 2 corrections routed

All re-read in Ghidra from the raw disassembly. Rows `(Quest Q5b review)` in `june2017_re/funcmap.tsv`.
- **`323cb2e0`** matches: `+0xaf8 / +0xafc` cleared before the templet test. Each mode block is non-empty test (`31c1c1a0` vs L"") -> RegisterMent ->
  `Play( name, frame, 0 )` -> BGM, inside the branch (the port's unconditional `JunePlayQuestSelectBGM` call is equivalent: it tests the name itself).
  The additional loop matches too: names and ment vectors cleared, frames kept, then both registered, both played, BGM. A missing row still runs it.
- **`323cb190`** matches. The key is the unit templet's first dword (unit type; null templet = 0). DEFAULT_* go out first, then the type row's start triple
  only if its start name is non-empty, and its end triple only if its end name is. Value layout: name, name, vec, vec, int, int (row and DEFAULT alike).
- **`3226ac80`** = lower_bound + `!(key < node)` -> keep, else insert at the hint, so map::insert is right. Other `+0xc0` users in the class:
  erase `3226ade0` (End @3226b807), forced clear `3226ccb0`, GetMentID `3226a2b0` (frame - 1, bounds-checked). G3 had already ported the first two.
  The stale header comment "( no writer in June )" is fixed (`X2GFXEventScene.h`, comment only).
- **BGM** `3232df20` / `3232e000`: `33775870` is `g_pTFieldGame`. The getter `323458d0` copies world `+0x44`, and `327fc950` is March's `PlayBGM`
  (`+0x54` non-empty, same-name skip). Only callers: `323cb2e0` x3 and End @3226b734 (after the cursor reset, before `31ce50d0`; March's order too).
- **Call sites** match: NEW_QUEST_NOT (`templet+8 == 5` = `QT_EPIC` -> `+0x70`, JUNE_BUGS 352), COMPLETE_ACK and ALL_COMPLETED (mode 1, after
  `323ca520` / `323dfa80( 1, 1, 1 )`), and Remind (`+0x750 != -1`, templet, mode 2, then the story test -> Q10).
- **Fix: June's state 5 is `XS_SERVER_SELECT`**, not a beginning state. StateChange case 5 (table `31d89ca4`, index = state - 1) builds `CX2StateServerSelect`
  (0x4b8, ctor `32576ff0`). The code now compares with `CX2Main::XS_SERVER_SELECT` and its comment says so (same value; flag `JUNE_QUEST_SCENE`).
  Offline the deferral never fires: the emulator sends the enter-village NEW_QUEST_NOT at `EGS_FIELD_LOADING_COMPLETE` (`Handlers_Field.cpp`), inside the village.
- **Correction: the deferred reader's blocks.** State 14 (0x5f8 state + 0x630 TFieldGame = village) reaches it @31d8947c. State 15 (0x5f8 + 0x680, June-only,
  no March state) and state 21 (0x5d0 = battle field) reach it @31d896aa. Q5b's row took the 0x680 block for the battle field. March's two sites are still right.
- **Lua scenes dropped: verdict stands.** Re-ran `dispsweep.py` on `1d8 1f4` (69 functions intersect) and on the size / capacity fields `1e8 1ec 204 208`, against
  all 59 named + 5 unnamed callers of `GetQuestTemplet`. The `1d8 / 1f4` hits are the device manager (`g_pKTDXApp +0x1f4`), vtable slot `0x1f4`, a UI
  member (`323d8be0`) or `this` (`32691a80`). The templet copies (`3234xxxx`) only copy.
- **Correction: the clear-window gate has a writer.** `323d2c60` otherwise matches March `OpenQuestRewardWindow` line for line: the 3 + 3 slots, unit type
  `+0xd2` / class `+0xd3` filters with no SlotIndex step, dummy ints id / socket `+0xc` / period `+8` (`QUEST_REWARD_PERIOD` and `_CLASS_TYPE_CHECK` are top-level
  in `Always.h`), the `%d` format, and the slot-1 check. But `+0x111` (`g_pKTDXApp +0x1fc +0x38c`) is written by June's tutorial-guide step `31fc4060`
  (= 1 unless guide `+0x8c == 1`), so June hides the clear window while a guide runs. March has no guide manager, so the gate stays unported. It moves,
  with the deferred scene's guide skip, to `june-dungeons C3z-4t` (kickoff amended).
- **Hygiene.** `flagcheck JUNE_QUEST_SCENE --range 012cee73^..012cee73`: 8 PASS, 1 PASS~, 1 DEFINE, 1 OTHER (`X2UIQuestNewJune.cpp`, the `JUNE_QUEST_UI` file), 0 FAIL;
  0 deleted, RESULT: PASS. Review fix `--worktree --files X2UIQuestNew.cpp X2GFXEventScene.h`: 1 PASS, 1 OTHER (the comment in G3's file), RESULT: PASS. The 3 deleted
  lines are Q5b's own lines, rewritten inside its `#ifdef`.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,415,168 B, dated 2026-10-19 17:08:42, md5 cad1ab82212b7fc4806800ba17607729, equal to X2/US_SERVICE/x2.exe (2026-10-19 17:08:33); built at main 4edc83db + 10 uncommitted code file(s).` The exe also carries peers' uncommitted work.
- **Logs** (the play-test run, 15:41-15:44): `[JUNE_QUEST] SCENE` 22328 mode 1, 22329 mode 0, 22328 mode 2 x2. `[JUNE_GFX] SCENE` Play / End lines. 0 EXCEPTION,
  UNHANDLED = baseline `E_DISABLE_HB_CHECK_REQ`. No new play-test needed: the fix does not change behaviour.

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

### Q6 - Done (2026-10-19, Sonnet 5.5 high)

Flag `SERV_IRUHADEV_JUNE_QUEST_GATE` (`Always.h`, no dependency). Client only; no shared struct, no save change. MODS.md "region quest gates".
- **Parser.** `LocalMapTemplet::m_iRequireCompletedQuestID` (June `+0x70`, `32501bb0` @32502b6a, EnterCondition, default 0). `LocalMapTemplet::IsQuestGateOpen( site )` = June `32322260`
  (`GetUnitCompleteQuest`); it logs each (site, map, state) once: `[JUNE_QUEST] GATE <site>: local map N is open|closed ( quest Q ... )`. The parser logs `local map N requires completed quest Q`.
- **Readers (all seven of Q0a's table):**
  - local-map tab `3293c6f0` @3293cbe0 (`X2LocalMapUI.cpp`; hides the tab, no quest mark; a GM skips it as in June);
  - `IsActiveDungeon` second test `327da190` @327da2ea (`X2DungeonManager.cpp`; row `+0x10` is `m_DungeonID`: the offsets `+0x4c` / `+0x50` match `m_RequireDungeonID` / the extra list);
  - warp item `32173120` @321738e3 (`X2UIInventory.cpp`) and Cobo `324627f0` @32462ffd (`X2StateMenu.cpp`; closed takes the branch that moves the list index);
  - world map `3225d340` @3225d745 (hides the pin) and field map `3225d970` @3225dca3 (`SetEnable( false )`) in `X2WorldMapUI.cpp`;
  - quest-notice map filter `323d4390` @323d44ca (`X2UIQuestNew.cpp`, inside the `RequireClearDungeonID` branch, as June's short-circuit), and the same filter in March's old `X2UIQuestReceive.cpp`.
- **Data.** data035 `DungeonData.lua` (core row, `luasrc.py refresh` ledger PASS): AD Normal `m_iRequireCompleteQuestID = 22218` live; `PACK OK data035` (1 changed: DungeonData.lua).
  `DLG_Map_Local_Elysion.lua` (June bytecode) already carries `RequireCompletedQuestID = 22218`, so no Lua edit there.
- **User decision (2026-10-19): no stand-in** for MapData `REQUIRE_COMPLETE_QUEST_ID` / `_SUB_QUEST_ID` (22218 / 22268) or BattleFieldData `NEED_ONGOING_QUEST_ID` /
  `BATTLE_FIELD_DIRECTION_INFO`. June's client has no reader for them (Q0a Part B). The `-- quest plan:` comments in data036 `MapData.lua` / `BattleFieldData.lua` are left as they are
  (comment-only; a data036 pack was not worth it). The Lanox F04 portal into Atlas Station therefore stays open for every unit.
- **`GetNPCFaceTextureKey`** was already June's (`323cc280`, Q5a: `X2UIQuestNew.cpp:4328` -> `JuneGetNPCFaceTextureKey`); nothing left.
- **Hygiene.** `flagcheck JUNE_QUEST_GATE --worktree --files <10 files>`: files 8 PASS, 1 PASS~ (`X2DungeonManager.cpp`, one blank line outside the flag), 0 DEFINE, 1 OTHER
  (`Always.h`: a peer's uncommitted skill-flag block move, not mine), 0 FAIL, 0 ERROR; 0 deleted lines, 0 non-ASCII, 0 EOL mismatch, 0 bare `#else`/`#endif`; RESULT: PASS.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,382,400 B, dated 2026-10-19 14:04:07, md5 e310d7d4f745f7ce893feec0800881f5, equal to X2/US_SERVICE/x2.exe (2026-10-19 14:03:58); built at main 857a2b74 + 26 uncommitted code file(s).`
  Both strings (`RequireCompletedQuestID`, `GATE %s: local map`) are in the exe. The exe also carries peers' uncommitted work.
- **Pending: `/x2-kom-packed copy data035`** (ask the user first, game closed). `data035_packed.kom` is packed.
- **How to see it.** No unit has 22218. Auth level is 3 in this save and a GM sees every tab (as in June), so the tab check needs `python tools/elsdb.py setting auth_level 0`.
  Play-test hand-off is in the Q6 hand-off message; log `python tools/logs.py --tag JUNE_QUEST --grep "GATE"`.

### Q6 - Review (2026-10-19, Opus 5.5 high): PASS, no findings

No code change, no rebuild; the deployed exe stays Q6's (md5 e310d7d4, play-test PASSED).
- **Parser.** XOR table row `32502b6a 32501bb0 ... RequireCompletedQuestID` (reg 337a3fe8:40) inside EnterCondition after `RequireUnitLevel`; templet `+0x70`, default 0. March parses it in
  the same table, after `RequireUnitLevel`, ctor default 0. Match.
- **Readers, all seven in Ghidra** (each tests `+0x70` through `32322260`):
  - tab `3293c6f0`: the GM branch (`+0x43c +0xf8`) skips level and quest. Otherwise the level test, then `+0x70 < 1` or complete leads to the marks, and closed
    is `vtbl+0x78` (SetShowEnable false, false) with no marks. March sets `bShowRadioButton = false`, which is read only by the mark block. Match.
  - `IsActiveDungeon` `327da190`: after the level + `+0x1cc` quest test, only when `g_pData +0x268` is set, `324f25f0( row +0x10, 0 )` -> `324f2360`, `0 < +0x70` and not complete
    returns 0. March is the same; `WMI_INVALID` = 0. Match.
  - warp item `32173120` (`PUSH 0x2719` = Elysion, level and `+0x70 == 0` or complete) and Cobo `324627f0` (`PUSH 0x2719` @32462fcd, level fails or `+0x70 != 0` and
    not complete, then the list-index switch). Match.
  - world map `3225d340` hides on level or quest. Its third term `cStack_5d` (code event 0x90f and control 40037) is a June-only event and not this flag's.
    Field map `3225d970`: SetEnable( level and quest ). Match.
  - quest notice `323d4390`: `size == 0 || ( clear && level && ( +0x70 < 1 || complete ) )`. March puts the quest test inside the `size > 0` branch. Match.
    `X2UIQuestReceive.cpp` carries the same filter in March's own copy.
- **Data.** data035 `DungeonData.lua` is BOM + CRLF. It has one live `m_iRequireCompleteQuestID = 22218` (AD Normal, :16791) plus the revert comment. `luasrc.py ledger --check`: LEDGER CHECK PASS.
  The pack gate (`luasrc.gate` on data035_kom): 0 refusals, 0 warnings.
- **No stand-in, re-confirmed.** `xorstrings.tsv` (6,241 rows) has `RequireCompletedQuestID` (the positive control). None of these appear in it or among Ghidra's plain strings:
  `NEED_ONGOING_QUEST_ID`, `BATTLE_FIELD_DIRECTION_INFO`, `REQUIRE_COMPLETE_QUEST_ID` / `_SUB_QUEST_ID`, `AddMapData`. They are server keys, so the user's decision stands.
- **flag-off.** Every site keeps March's line in `#else` or is insert-only, so flag-off = the Q5a build.
- **Hygiene.** `flagcheck JUNE_QUEST_GATE --range d3c30f58^..d3c30f58`:
  `files: 8 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`. `git log -S` finds no later commit touching the flag.
- **Left as is (cosmetic):**
  - The `Always.h` block opens with two short `//////` lines and borrows the STANDIN block's closing rule. It is comment-only, and a PCH-wide rebuild is not worth it.
  - The `X2DungeonManager.cpp:744-748` comment (SUITABLE_LEVEL flag) still says March's templet lacks `+0x70`.
- **Q9.** Its section and kickoff cover "gates opening" (22160 -> 22218). Nothing to add.

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

## Phase Q10 - Story book (QT_STORY)

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_STORY`. Sub-tag `STORY`. Added by Q3 (2026-10-19): no earlier phase owned it.

- Map first, read-only: the singleton `326ab210` (map by quest id, lookup `326ab090`, loader `326a8d40` from the quest loader `3258c470`), the `g_pData +0x570` rebuild `32335f50`
  (called from `SetUnitQuest` and `323e57e0`), `CX2UIStoryBook` / `DLG_Story_Book.lua`, the QT_STORY branch of the complete ACK (`328c3750`, `328c3ae0`) and the reader of Q1's story index.
- Then port the book whole: data file, singleton, rebuild, UI (June's `Story_New` / `StoryBook` strings), under Q5a's Story tab.

**Done gate:** completing a story quest adds it to the book; the book opens and lists the completed episodes. 0 EXCEPTION.

**Split (2026-10-19, Q10's map):** Q10 = the data, the rebuild and the Remind button; the player is Q10b, the book UI Q10c, the menu-bar opener Stage 9 (user). The gate above is Q10c's.

### Q10 - Done (2026-10-19, Opus 5.5 high)

**Map (read-only).** The "story book" is three June pieces plus a menu button; the evidence is `june2017_re/quest_plan/q10_story_map.tsv` (85 rows),
`q10_story_keys.tsv` (both parsers, key by key) and `q10_story_assets.tsv` (every file the data and dialogs name, June / March).
- **Data:** `CX2SlideShotScriptedManager` (Lua `g_pSlideShotManager`, singleton `326ab210`, 0x20 B): `MediaInfoScript.lua` (AddMediaInfo `326a8e10`: 24 rows,
  1 category + 23 episode groups, 81 files) and `SlideShotScript.lua` (AddStory `326a9890`: 82 calls, 81 stories, 1,655 cut scenes). Neither file was in March.
- **Player:** an unnamed KTDGUI + particle class at `CX2Data +0x288` (ctor `328c34b0`, `DLG_NewSlideShot.lua`), started by the complete ACK, the Remind
  handler and the book; not GFx. -> **Q10b**.
- **Book:** `CX2UIStoryBook` (`32d75370`, `DLG_Story_Book.lua`), UI-manager menu 0x2f. -> **Q10c**.
- **Opener:** only the `StoryBook` button of June's own menu bar (`SMUCM_STORY_BOOK` 181, `32478e90`) with the `Story_New` mark (`g_pInstanceData +0xabd`).
  **User 2026-10-19: no opener here; Stage 9 owns June's menu bar** (handed in `JUNE_PORT_BACKLOG_ORDER.md` Stage 9). Q10c adds a dev-console opener.
- **Overlap with Q0b / Q5a:** none in code. Q5a's Story tab lists story quests from Q1's `m_mapStoryQuest`; the book is keyed by the same `m_iEpisodeGroupID`
  (June templet `+0x1b4`, = MediaInfo `MediaGroup`). Q5a's empty `QSS_COMPLETE_STORY` Remind case and Q5b's Remind note (@323e293d) are filled here / in Q10b.

**Landed (this phase = the data side; flag `SERV_IRUHADEV_JUNE_QUEST_STORY`, needs `_LOAD` / `_RUNTIME` / `_UI`, all four ON by `flagprobe`).**
- New `X2SlideShotScriptedManager.h/.cpp`: both parsers as June reads them (defaults Title / Speaker / Text -1, Scene_Duration 7.0, IsCategory rows skip
  MediaGroup / MediaFile but are kept, `Cut_Scene%d` from 0 to the first gap, the five time sorts of `326a8d70`, first-wins inserts), `GetStory` (`326ab090`),
  `HasStory` (`326a7d50`), the bindings `AddStory` / `AddMediaInfo` (`X2Main.cpp`, June 31d95080 @31e44599 / @31e446e1).
- Load (`X2Data::ResetQuestManager`, June 3258c470 @32590879..32590885): destroy, make, `MediaInfoScript.lua` then `SlideShotScript.lua`; destroyed in `~CX2Data`.
- `CX2QuestManager::JuneRebuildStoryBookQuest( bNew )` (`32335f50`) -> `CX2Data::m_mapJuneStoryBookQuest` (June `+0x570`) and
  `CX2InstanceData::m_bJuneStoryBookNew` (June `+0xabd`, read only by Stage 9's menu bar); called from `SetUnitQuest` (false, @32337290) and the complete ACK
  for a QT_STORY quest with a story (true, @323e5a45).
- Remind button (`323ccbc0` case 6): a completed story quest with a story shows it (enabled in a village / field, disabled elsewhere, focused); none = hidden.
  June's state 0xf there is the guild agit (March has none), so Q5a's village / field helper is used.
- Lua: June's `MEDIA_TYPE` table added to core `Enum.lua` (data036, `luasrc.py refresh`); `MediaInfoScript.lua` / `SlideShotScript.lua` adopted as June's
  ljconv bytecode (`adopt-june`, data036). `PACK OK data036` (+2 added, Enum changed). JUNE_BUGS 356 (BlackLayoutMove never read), 357 (43430 twice).
- Not in Q10 (owned): the player and its starts, `+0xabc`, `DLG_NewSlideShot.lua`, 69 scene `.dds`, 1 BGM (Q10b); the book, menu 0x2f, `DLG_Story_Book.lua`,
  `UI_STORY_BOOK_CUSTOM_MESSAGE`, Storycut / NoteOfSkill / Button20 textures (Q10c); the menu bar button and `Story_New` (Stage 9).

**Hygiene.** `flagcheck JUNE_QUEST_STORY --worktree --files <12 files>`: `files: 10 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 1 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
The FAIL is `X2SlideShotScriptedManager.cpp`'s `#include "StdAfx.h"` (outside the flag for the PCH, as G3's new file). `luasrc.py ledger --check`: PASS.

**Deploy.** `DEPLOY OK: X2_offline.exe is 25,446,400 B, dated 2026-10-19 18:00:54, md5 36fbf5def5d982244a330e179b7837dd, equal to X2/US_SERVICE/x2.exe (2026-10-19 18:00:46); built at main 14420b7d + 30 uncommitted code file(s).` (a peer's run at 18:00 compiled this phase's last edit; my run found nothing to relink). The exe holds `[JUNE_QUEST] STORY LOAD media rows`, `[JUNE_QUEST] STORY rebuild`, `MediaInfoScript.lua`, `SlideShotScript.lua`, `CX2SlideShotScriptedManager`. First run failed on my `Scene_Delete.Key` read (no `std::string` reader in the client's KLuaManager: read wide, narrowed) and on a peer's half-saved Q4b header (not touched).

**How to see it.** Copy data036 (the game closed), log in with any unit and read `python tools/logs.py --grep "JUNE_QUEST\] STORY"`:
`STORY LOAD media rows 24 ( ... 1 categories, 23 groups, 81 files, 81 with a story ) stories 81 ( AddStory calls 82 ) cut scenes 1655`, then one
`STORY rebuild: ... Story_New mark 0` per SetUnitQuest. A completed story quest in the quest window's Story tab shows the Remind button (the replay is Q10b).
Completing a story quest logs `STORY rebuild ... mark 1`.


## Phase Q10b - Story slide-show player

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER`. Sub-tag `STORY`. Split out of Q10 (2026-10-19): the map showed three classes.

- Port June's player at `CX2Data +0x288` whole (reset `31edee40`, ctor `328c34b0`, Start `328c3750`, OnFrameMove `328c41f0`, next cut `328c3210`, End `328c3ae0`,
  text `328c27d0`, the particle-key map), its starts (complete ACK @323e5a4a, Remind @323e293d), the end message (`g_pInstanceData +0xabc`) and the forced Ends
  (`3243cec0`, `3243d1d0`, `3243d4e0`, `323080c0`). Rows: `q10_story_map.tsv` "manager+player".
- Data: June's `DLG_NewSlideShot.lua` (data034, replaces March's; the tutorial `CX2SlideShot` shares it), the 69 `StoryQuest_*.dds` (data048), one BGM.

**Done gate:** completing a story quest in a village plays its slide show and the end message; Remind on a completed story quest replays it. 0 EXCEPTION.

## Phase Q10c - Story book UI

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_QUEST_STORY_BOOK` (+ `_DEBUG` dev-console opener). Sub-tag `STORY`. Split out of Q10.

- Port `CX2UIStoryBook` whole (`32d73c10`..`32d757a0`) and UI-manager menu 0x2f (`32158070`, `3214f1c0`, `3215a0d0`, `321524d0`, close-all, layer, dispatch).
  Rows: `q10_story_map.tsv` "book UI".
- Data: `DLG_Story_Book.lua` (data034), `UI_STORY_BOOK_CUSTOM_MESSAGE` into core `Enum.lua`, Storycut 01-05 (data057), June's NoteOfSkill_01 (data057) and
  Button20 (data056) sheets.
- The menu-bar button is Stage 9's (user, 2026-10-19).

**Done gate:** the book opens from the dev console, lists the unit's episode groups, and a completed story quest's thumbnail plays its story. 0 EXCEPTION.

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
