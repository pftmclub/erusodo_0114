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
