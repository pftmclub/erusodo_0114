# June transcendence system, on Aisha (backlog Stage 6)

Plan key `june-transcend` · branch `main` · log tag `[JUNE_TRANSCEND]` (sub-tags `UNIT`, `WIRE`, `DB`, `CHANGE`, `ITEM`, `SLOT`, `ENGINE`,
`ARME`) · flags `SERV_IRUHADEV_JUNE_TRANSCEND_*` (engine, + `_DEBUG`), `SERV_IRUHADEV_JUNE_CHAR_ARME_TRANSCEND` (Aisha) · kickoffs
`kickoffs/june-transcend/` · planned 2026-10-22 (Opus 5.5, xhigh; brief `kickoffs/june-backlog/stage6-plan.md`).

## Context

June 2017 lets a level-70 second-job character "transcend". The class id stays the same; the unit gains a flag that unlocks:
- the hyper-class skills (`SPRT_HYPER_CLASS`, power-rate type 5; Aisha has 18);
- the A-bar transcendence slot (A-5 / F; the B-5 / T slot needs an item as well);
- the transcended awakening (per-class hyper-mode tables, `Effectset_TRANScendence_WakeUp_FX`, a 1.3 summon factor);
- the transcendence weapon stat row, and the guide popup.

Aisha (pilot-char-revamp, closed 2026-10-22) already carries every piece of this as dormant code behind stand-ins that answer "not
transcended". This plan gives the flag a source and switches each piece on, on Aisha first. Other classes' class-specific readers go to
their Stage 7 / 8 ports through the checklist.

## Rules

- **Standing rules:** `JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules (every stage, every kickoff)". In particular:
  - the scope rule (a branch that is not transcendence and not needed to play it gets an owner and a brief, never a phase here);
  - no partial June updates;
  - June wins, even when it removes a feature;
  - June bugs are kept verbatim with a `JUNE_BUGS.md` row;
  - cite June addresses, never raw offsets.
- **The flag is a unit field, not a class (planning finding).** No new `UNIT_CLASS`, so no `GBase_Item` rows and no live-DB pull.
  The open decision "Starter items for classes the live DB never had" no longer gates Stage 6.
- **The save is the switch.** A unit is transcended when its save row says so. Until a unit's column is 1, every phase's code is inert.
  So T1 can switch all the stand-ins to the real reader at once.
- **Wire changes are offline-only** (CLAUDE.md, user 2026-10-05). A `CommonPacket.h` member is behind its flag in `Always.h` and in
  `ServerDefine.h`, and the flag goes into `SECOND_HOMES` in `tools/flagcheck.py`. `X2ServerProtocol` sees it through `CommonPacket.h:16`;
  prove it with `cl /P` on a file from each project.
- **Saves.** Snapshot `db_backup/` before the schema rung; migrations are forward-only.
- **Shared Lua is per character** (user, 2026-10-20). A multi-class `.lua` (`LevelUpEvent.lua`, `CharacterStatTable.lua` if it holds
  per-class rows) takes only Aisha's rows here, as core-list BOM source; a system file with no per-class rows is adopted whole.
- **Dev free-skill gate** (user, 2026-10-22): any June skill-use check this plan brings in is wrapped with `JuneIsDevUnlimitedSkillUse()`.
- **Opus phases delegate** reads, greps, builds, flagcheck and log reads to subagents (x2-phase, *When the phase is Opus*).

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "<heading>"` |
| Planning evidence | `june2017_re/transcend/` (`plan_*` files, below); funcmap rows `Transcend S6` |
| Aisha's dormant pieces | `python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4a - Done (2026-10-22, Opus 5.5)" "CH4b - Done (2026-10-22, Opus 5.5)" "CH3b - Done (2026-10-21, Sonnet 5.5)"` |
| June Lua (reading) | `june_src_fixed/data036_kom/` `TranscendentalClassItem`, `FieldQuest`, `FieldSubQuest`, `NewSkillTempletVer2`, `SkillCommandDescriptionData`, `CharacterStatTable`, `LevelUpEvent`; `data034_kom/DLG_Transcendental_Change_Guide_Aisha` |
| Ghidra | `ghidra` skill (read-only in T0 and reviews); `june2017_re/disasm_range.py`, `dispsweep.py` for raw ranges |
| Bring June Lua / assets in | `/x2-adopt-june`; `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, then `/x2-kom-packed copy dataNNN` (ask first) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_TRANSCEND <sub>"` |
| Save / test character | `/x2-db`; units 51 `ArmeEM` (104), 52 `ArmeVP` (105), 53 `ArmeDW` (113), 54 `ArmeVP2` (105), 28 `jtVoidPrincess` (105, level 80): all second job, level 70+ |
| Dev console | auth level 3 (`python tools/elsdb.py setting auth_level 3`), END key |

## Planning evidence (2026-10-22)

**How June represents it** (Ghidra; funcmap rows `Transcend S6`):
- `CX2Unit +0x3c` is an int next to the class (`+0x38`), inside `UnitData` (which starts at `CX2Unit +0x28`).
  - Writers: `SetKUnitInfo` `3235eb20`, class a ushort at packet `+0x15`, flag an int at `+0x17`.
  - Writers: the setters `32360090` (Room by offsets: class `+0x21`, flag `+0x23`), `323614c0` (Field: `+0x24` / `+0x26`) and `32361e00` (Square
    by offsets: `+0x4a` / `+0x4c`). A fifth setter, `32360c70`, takes a class at `+0x4a` and no flag.
  - The only other writer is `3234b4a0` (called by `31ef82f0`).
- `31ef82f0` is the state handler (state vtable `+0x180`) for June event **0x596** (dispatch `31f2ab80 @31f2bf11`). Its payload is two ints.
  - It writes the second int into my unit's `+0x3c`. It calls the tree UI (`328ed890`, `328e7b70( 1, 1, 0 )`) and `328d6f40( type, class, flag )`.
  - When the second int is non-zero, it calls `CX2ClassChangePopup::SetShow( 1, 1 )` (`324886b0`, the guide mode).
  - It also calls quest `3232ecd0` and state vslots `0xac` / `0x104`.
- Readers test `!= 0` almost everywhere; `31fceab0`, `32063650` and the quest UI test `== 1`.
- Readers, 48 on the user's unit (`+0x867c`): `june2017_re/transcend/plan_readers_unit.txt`.
  - Engine: `31fceab0`, `31fdd490`, `31fdd800`, `31ffd490`, `3201a8f0`, `32028790`, `32063650`, `32298590`, `322ece70`, `32610cb0`.
  - Aisha: `31fcac90`, `31fcad70`, `31ff1e80`, `32cddcc0`, all ported with stand-ins.
  - The rest are other classes' code (`329e*` / `32a0*` Add, `32a5*`-`32a7*` Ain, `32ab*`-`32d1*` others).
- Readers, 30 on my unit (`g_pData +0x68 +0x90`): `plan_readers_my.txt`. They are the skill tree / slot UI (`328dfab0`, `328e18d0`, `328ed890`, `328de040`),
  `JuneGetSkillListByLevel` `32206d30`, the quest manager (`3232e510`, `32336e80`), the class name / icon `31ee0de0` (from `32ed5180`) and UI structs (`328b3350`).

**How a unit becomes transcended in June:**
- **The quest.** "Limit Break 1/5-5/5", quests 41990 / 42000 / 42010 / 42020 / 42030.
  - All are level 70, at NPC `NUI_TRANSCEND_QUEST` 10153 (house `HI_TRANSCEND_QUEST` 40140), for the 37 second-job classes.
  - 42030 has `QT_CHANGE_TRANSCENDENTAL_CLASS` (9) and the reward `QR_TRANSCENDENTAL_CLASS` 1001 (`m_iChangeUnitClassTran` = 1), plus item 293722
    "[Cobo] Skill Slot Change Medal (Transcendence) Cube (15 Days)".
  - March already parses all of it (quest-system Q1-Q3). The emulator refuses 42030 today with `ERR_QUEST_18` (`Handlers_Quest.cpp:331-337`).
  - Follow-on chains 42040-42080 and 42090-42130 are `QT_SKILL` (their own rewards).
- **The items.** `TranscendentalClassItem.lua` (data036, absent from March) is loaded by `CX2Data::ResetItemManager` `31edda90` with binders
  `AddTranscendentalUnitType` (`31db3c8f`) and `AddTranscendentalClassItem( item, 0|1 )` (`31db3f10`).
  - Arg 0 is used for second-job tickets and "Book of Transcendence"; arg 1 for "Class Change Ticket (Transcendence)", Skill Slot Change Medal
    (Transcendence) 288318, Forbidden Secret Manual, [Cobo] Manual. Aisha: 288323 / 288324 / 288325.
  - Rows are in `plan_tci_rows.tsv`. The meaning of 0 / 1 is not decoded.
  - NetError ids 1675-1727 (`GetErrStr` `3264a950`) carry the item-use refusals: "cannot Transcend", "same rank", "only after being transcendent",
    "no longer use this item after Transcendence", "2nd Job Class over Lv.70".
- **The guide.** `DLG_Transcendental_Change_Guide_<class>.lua` (12, data034; textures `POPUP_TR_*.tga` / `TR_POPUP_IMAGE.tga` in June data215).
  - It is opened by `324886b0` (mode 1), with init `32486790` / `32710710` and the name test `32711490`.
  - It is also opened by June's level-up event `LUEI_CHANGE_CLASS_GUIDE_TRANSCENDENTAL` (6, level 70, `LevelUpEvent.lua`). March has
    `CX2LVUpEventMgr` with FIRST / SECOND only.
- **The dev cheat.** June's `GameEditCommandList` has `/tc bEnable` -> `g_pMain:ChangeTranscendentalClassCheat` (xorstring `31e41e65`).

**What Aisha waits for:** her 18 `SPRT_HYPER_CLASS` skills (`plan_aisha_hyper_class.tsv`; tree rows 14-17):
- actives 22041 / 22042 / 22068 / 22224 (EM), 22043 / 22044 / 22069 / 22225 (VP), 22045 / 22046 / 22070 / 22226 (DW): most already have states (CH3);
- passives 22412-22417.

Also waiting:
- `TRANSCENDENTAL_SKILL_DATA` rows (already read by `JuneLoadSkillCommandDescription`);
- the transcended hyper tables `AVSI_AEM/AVP/ADW_HYPER_MODE` (already registered; same callbacks as `AVSI_HYPER_MODE`);
- cut-ins `HQ_Cutin_AEM/AVP/ADW_Transcendence` (the `.gfx` are in data195);
- the memo row 22209 -> 22217 (VP).

**March stand-ins to switch** (all return "not transcended"):
- `June_UnitFlag3c` (`X2GUArme_VioletMage.cpp:28164`);
- `CX2GUUser::JuneIsUnitTranscended` (`X2GUUser.cpp:31637`);
- `CX2UISkillTreeNew2015::JuneIsMyUnitTranscended` (`.h:226`) and `bTranscendent` (`.cpp:2890`);
- `JuneSelectUnitClassTran()` (`X2UIQuestNewJune.cpp:52`);
- the hyper-class skips (`X2SkillTree.cpp:3295`, `:3350`; `X2OfflineSkill.cpp:884`);
- the slot-4 / 9 skips (`X2GUUser.cpp:40216`, `:6422-6432`, `:6458-6461`);
- the emulator's equip refusal into 4 / 9;
- the 42030 refusal.

**Not transcendence, routed (scope rule):**
- **Awakening effect-set family** (`32011d20`, `31fd66b0`, `32011f50`, `3204b5e0`):
  - `+0x9578` is a wstring holding a set name. It is taken from the applied-customize map on CX2Unit (`+0xa48` key 15) through
    `325d2370( 3 )`, with "_Add" for unit type 9.
  - Owner moved from Stage 6 to **Stage 12** (bullet there), together with `CharCustomData`'s `EFFECT_SET_NAME_TRANSCENDENTAL` (parser `326525d0`).
  - Only `Effectset_TRANScendence_WakeUp_FX` (pushed by `32298590` for a transcended unit) stays here (T5).
- **Resonance** (`3247b460` = level > 98 and CX2Unit byte `+0xaa9`; setter `3234b3e0`; EXP `322f8920`; June `KUnitInfo` tail): a level-99
  progression system, new **Stage 30** (`kickoffs/june-backlog/stage30-plan.md`).
- **Achievements** (`CX2AchievementManager`, `ACT_TRANCENDENTAL_CLASS_COUNT` 14 counts transcended characters): no March system, new **Stage 31**
  (`stage31-plan.md`).

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| T0 | Transcendence map (read-only): readers, wire, event 0x596, quest / item / slot / engine paths, per-class rows | Opus high | - |
| T1 | The flag: `UnitData`, packet structs, save rung, emulator, `elsdb.py` switch, every stand-in on the real reader | Sonnet high, Opus review, user play-test | T0 |
| T2 | Becoming transcended: quest 42030, June's 0x596 notify, the guide popup (mode 1), the level-up guide, the `/tc` cheat | Sonnet high, Opus review, user play-test | T1 review |
| T3 | The transcendence items: `TranscendentalClassItem.lua`, binders, ticket use and refusals, emulator | Sonnet high, Opus review, user play-test | T2 review |
| T4 | The TC slots: A-5 / F for a transcended unit, B-5 / T by slot-state key 1 (medal 288318 / cube 293722), states, emulator, UI | Sonnet high, Opus review, user play-test | T1 review |
| T5 | Transcended combat engine: awakening (`31ffd490`, `32298590` WakeUp FX), `32063650`, `3201a8f0`, aura, summon 1.3, stat table, cut-in names | Opus high, Opus review, user play-test | T4 review |
| T6 | Aisha transcended: her readers live, passives 22412-22417, transcended hyper tables, cut-ins, memo, tree rows 14-17, her guide | Sonnet high, Opus review, user play-test | T5 review |
| T7 | *Opened by T0 only:* transcendence sockets (`SDT_*_TRANSCEND*`) and anything else T0 finds on the subject | per T0 | T6 review |
| T8 | Walk: unit 52 through Limit Break 1-5, units 51 / 53 by `/tc`, every check on Aisha | Sonnet medium, user play-test | T3, T6 (T7) reviews |
| Z | Close-out: MODS, checklist transcendence section (7ao), Stage 7 template rows, backlog note, `_DEBUG` off | Sonnet medium | T8 |

T0 runs now (read-only, beside anything). T2 / T3 share the emulator's class-change code: one after the other. T4 may run beside T2 or T3
when the kickoffs' files do not overlap (`X2OfflineServer.cpp`'s dispatch table and `Always.h` one at a time). T5 and T6 edit `X2GUUser.*`
and `X2GUArme_VioletMage.*`: one code phase at a time there.

## Phase T0 - Transcendence map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate the pulls (x2-phase).

**Scope.** Planning settled the representation; T0 settles everything the code phases need, then corrects their kickoffs.
1. **Readers.** Classify every row of `plan_readers_unit.txt` / `plan_readers_my.txt`, and any reader they miss (other base registers,
   `+0x3c` via `CX2Unit*` getters, `CX2Room` slot units). Classes: engine (T1-T5), Aisha (T6), other class, UI owned by Stage 9.
   - Other-class rows go to a new checklist section (§7ao "Transcendence, per class"), one row per class with its June addresses.
   - Output: `june2017_re/transcend/t0_readers.tsv`.
2. **Wire.** June's position of the flag in each struct: `KUnitInfo` (`+0x17`), Room / Field / Square.
   - Identify `32360c70`.
   - List any other June struct or packet that carries it (character list, party, user list, P2P unit sync).
   - Map each to March's struct and Put/Get lines (`CommonPacket.cpp`).
3. **Event 0x596.** Its June name, its payload (int 0 = ?), who sends it in June (quest complete? item use? both?), and `31ef82f0`'s callees
   (`328d6f40`, `328ed890`, `328e7b70`, `3232ecd0`, state vslots `0xac` / `0x104`) with their March counterparts. Propose the offline event id.
4. **Quest path.** June's client side of a `QR_TRANSCENDENTAL_CLASS` complete:
   - what the ACK's unit info carries;
   - what the quest UI does with `m_iChangeUnitClassTran`;
   - `IsForbiddenQuest` `3232e510` / `SetUnitQuest` `32336e80`'s `+0x3c` compares (Q3's port state);
   - `GetClassUpQuestNoticeState` step 3.
   Level-up guide: June's `CX2LVUpEventMgr` entry 6 and its two buttons (1549 accept quest / 1550 buy cash item).
5. **Item path.** What `31db3f10` stores, what arg 0 / 1 mean, the ticket-use REQ / ACK and which NetError each check sends. The `/tc` cheat:
   `ChangeTranscendentalClassCheat` and what it sends.
6. **TC slots.** What June allows in slot 4 / 9 (any skill, or only some), `32024b00` / `31fcadd0` / `3220e3b0` / `328e18d0` rules for a
   transcended unit, B-TC by slot-state key 1 (who sets it: medal 288318, cube 293722, expiry). Also `BFI_TRANSCENDENTAL_SLOT_ATTACK_EFFECT` 843 /
   `_SLOT_COOLTIME_EFFECT` 844 and `BuffBehaviorTemplet m_iSkillSlotType` 3: who applies them.
7. **Engine.** Each engine reader's transcended branch:
   - `31ffd490` -> `31fc8c20`;
   - `32298590`: WakeUp FX and the `vslot 0x21c` skip, `BFI_TRANSCENDENTAL_HYPER_MODE` 845;
   - `32063650` (`== 1`), `3201a8f0` -> `31ff6920`, `32028790`;
   - `322ece70` (`_DAT_33472a10`), the `32610cb0` region, `31fdd800`;
   - `USI_TRANSCENDENTAL_HYPER_MODE` (24) and its users.
   Also the stat table: `328d7890` stores `TRANSCENDENTAL_EQIP_POSITION_RELATIVE_STAT_TABLE` through `31ef0b40`; find its consumer and March's
   (`X2ItemStatCalculator.cpp:20-23`); does June's whole `CharacterStatTable.lua` replace March's?
   Also `TRANSCENDENTAL_HYPER_MODE_PARTICLE` (`31fee090`; no June script has the key), `EffectSet_UI_Transcendence_Open/Loop`
   (`32154f60` key 4, caller `32157c10`) and the hyper cut-in name reader.
8. **Sockets.** What `SDT_*_TRANSCEND*` (190-193, 224-227) need beyond `SERV_IRUHADEV_JUNE_SOCKET_PARITY`. Open T7 only if something is missing.
9. **Sizing.** Count per phase, split any phase over ~40 transcriptions, and rewrite the T1-T8 kickoffs with the facts (file:line, addresses).

**Done gate:** `t0_readers.tsv` with 0 unclassified rows; every T1-T8 kickoff names its June addresses and March sites; funcmap rows
`Transcend T0`; checklist §7ao; T7 opened or closed with the reason.

### T0 - Done (2026-10-23, Opus 5.5)

Read-only: no code, no flag, no build or deploy. Outputs:
- `june2017_re/transcend/t0_readers.tsv`: 82 rows (the planning scans' 80 functions plus `3291df90` and `31ee0de0`), 0 unclassified.
  Owners: T1 11, T2 4, T3 2, T4 2, T5 5, T6 5 (`31fceab0` also T5), class code 35, Stage 9 UI 17, Stage 29 1.
- funcmap: 17 rows `Transcend T0`. Checklist: **7ao** "Transcendence, per class" (7an was already CH4c's; the plan, T6 and Z now say 7ao).
- Kickoffs T1-T6 rewritten with file:line, June addresses and struct offsets. T7 closed. No phase is over ~40 transcriptions; none split.
- An extra scan (unit getters, `UnitData +0x14`, class / flag read pairs) found one more reader, `3291df90` (character info), and nothing else.

Findings, by kickoff item:
1. **Readers.** Engine readers are few. The four base skill helpers gate hyper-class skills: `31fcac90`, `31fcad70`, `31ff1e80` test `!= 0`,
   `31fceab0` tests `== 1`. Then `HyperModeStart`, `ApplyHyperModeBuff`, the aura, the summon factor and the name-tag icon. Each class's
   hyper-effect start returns at once for a transcended unit. Elsword's 21416-21418 passives are read from engine code (7ao).
2. **Wire.** June reads the class as a word and the flag as an int right after it, in four structs: `KUnitInfo` +0x15 / +0x17, Room +0x21 / +0x23,
   Field +0x24 / +0x26, Square +0x4a / +0x4c. `32360c70` is a flag-less Square-prefixed struct in the field game. No other packet carries the flag.
3. **Event 0x596** is the NOT of `/tc`'s pair: REQ 0x594 (one int), ACK 0x595. Its payload is two ints; the client uses only the second.
   `31ef82f0`'s callees are mapped in its funcmap row: the item-stat calculator key `328d6f40`, the popup on `g_pData +0x30c`, the tree UI on the
   UI manager, the cash-shop close, the quest refresh `3232ecd0` and the class-up notice vslot `0x104`. June has no event names; T2 labels ours.
4. **Quest.** `IsForbiddenQuest` is already ported with the flag fixed at 0 (`X2QuestManager.cpp:6188`): T1 switches it. `SetUnitQuest`'s compare
   (`@3233743b`) is missing: T2. Level-up guide: June's `UICustomEventProc` `32712790` maps 1549 to quest manager `( 9, 0 )` and 1550 to the cash shop.
   March's `LevelUpEvent.lua` is bytecode with FIRST / SECOND only.
5. **Items.** `AddTranscendentalClassItem` is `3280b6c0`, a map from item to type with getter `32807fe0`. Type 0 is usable only before transcendence
   (NetError 1727); type 1 only after (1686). `/tc` is `31d809c0`: auth level 3 or more, `arg % 1000`. The NetError texts are decoded (funcmap).
6. **TC slots.** A-TC needs the flag; B-TC needs slot-state key 1 only. Only `31fc8c20` applies 843 / 844: on awakening, a 0.5 roll, timer 20.0.
   They act on `SKILL_SLOT_TC` (type 3) skills, which March cannot resolve yet (T4).
   - Possible June quirk for T4 to confirm: the in-game cool-time loop `322d8100` `break`s at a locked A-TC.
   - The locked-slot click is June `CX2StateMenu` case 182 (March 208).
7. **Engine.** Summon factor 1.2 -> 1.3. WakeUp FX + BFI 845 replace the hyper buff. `CharacterStatTable.lua` is per class: June adds one global
   table, `TRANSCENDENTAL_EQIP_POSITION_RELATIVE_STAT_TABLE` (the weapon row differs). The consumer is `CalculateItemStat`, keyed by `328d6f40`.
   `TRANSCENDENTAL_HYPER_MODE_PARTICLE` has no June data.
8. **Sockets.** T7 is closed. `SOCKET_PARITY` already has the 8 types and their stats, and no June socket code reads the flag.

**Routed (scope rule):**
- the June UI readers (HUD portrait, character info, class icon, inventory, GFx cash-shop filter, world-map quest marks) -> Stage 9 bullet;
- June's 2-byte class on the wire -> Stage 8 bullet;
- the result screen's class image `324ce260` -> Stage 29 note.

All three are in `JUNE_PORT_BACKLOG_ORDER.md`.

No questions for the user. T1 is next.

## Phase T1 - The flag: unit, wire, save, emulator, switches

**Run with:** Sonnet 5.5 high, then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_TRANSCEND_UNIT` (also in `ServerDefine.h`,
`SECOND_HOMES`).

**Scope.**
- `CX2Unit::UnitData` gains June's int (June `+0x3c`), with a getter and June's setter `3234b4a0`. The four (T0: five) setters copy it.
- The packet structs T0 lists gain the member, with Put / Get in June's order.
- **Save rung v22:** `unit.transcendence INTEGER NOT NULL DEFAULT 0` (`X2OfflineDB::Migrate`, `ReadUnitRow`, `KOfflineUnitRow`).
  - `MakeUnitInfoFromRow` and the room / field / square builders fill it.
  - `tools/elsdb.py` gets `settranscend <unit> <0|1>` (the x2-db skill text too).
- **Every stand-in reads the field** (stand-in body kept in `#else`): the list in *Planning evidence*.
  - The class code keeps its own call sites; only the stand-ins' bodies change.
  - The slot-4 / 9 skips and the emulator's equip refusal stay for T4; the 42030 refusal stays for T2.
- Log: `[JUNE_TRANSCEND] UNIT <uid> class <c> transcendence <n>` once per unit build (select, room, field).

**Done gate:** all units select with `transcendence 0`. After `settranscend 52 1`:
- unit 52 logs `transcendence 1`;
- its skill tree shows the hyper-class rows 14-17 and the A-TC slot drawn unlocked;
- unit 52 awakens into `AVSI_AVP_HYPER_MODE` (CH1j's registration);
- `settranscend 52 0` restores it.
No crash with the flag on before T4-T6. Prove the struct size with `cl /P` in `X2Lib` and `X2ServerProtocol`.

### T1 - Done (2026-10-23, Sonnet 5.5)

Flag `SERV_IRUHADEV_JUNE_TRANSCEND_UNIT` (`Always.h` + `ServerDefine.h`, `SECOND_HOMES` entry; `Always.h` carries an `#error` guard that it needs `JUNE_QUEST_DATA`, the v21 rung). Save snapshot `db_backup/..._pre_T1_before_v22.sql` taken first.
- **Unit.** `UnitData::m_iUnitClassTran` (June `+0x3c`), `GetUnitClassTran`, `SetUnitClassTran` (June `3234b4a0`), cleared in `Init`, copied by `SetKUnitInfo` / `SetKRoomUserInfo` / `SetKSquareUserInfo` / `SetKFieldUserInfo`.
- **Wire.** `int m_iUnitClassTran` right after `m_cUnitClass` in `KUnitInfo`, `KRoomUserInfo`, `KSquareUserInfo`, `KFieldUserInfo`, Put and Get in that place (March's class is a `char`; June's 2-byte class is Stage 8's). Ctors zero it (`KUnitInfo`, `KRoomUserInfo`); the Square and Field copy operators copy it. `KCommunityUserInfo` (`:6300`) and June's flag-less `32360c70` struct untouched.
  - `cl /P` proof (subagent, VS2010, probe script in the scratchpad): the member is inside all four structs, directly after the class, in both an `X2Lib` TU (`X2Unit.cpp`) and an `X2ServerProtocol` TU (`Event.cpp`). Put / Get come only through `X2Lib/X2ServerPacket.cpp:15` (`#include "CommonPacket.cpp"`; `X2ServerProtocol` does not compile `CommonPacket.cpp`): `Put(m_iUnitClassTran)` follows `Put(m_cUnitClass)` and Get mirrors it, in all four.
- **Save v22.** `unit.transcendence INTEGER NOT NULL DEFAULT 0` (`iFrom < 22` rung, one `ALTER`); `UNIT_COLUMNS` +`transcendence` (column 23), `KOfflineUnitRow::m_iTranscendence`; `MakeUnitInfoFromRow` and `MakeRoomUserInfo` fill the member. The emulator builds no Square / Field info, so those two only carry what the packets hold. `tools/elsdb.py settranscend <unit> <0|1>` (refuses until the v22 exe has migrated the save) and the x2-db skill line.
- **Stand-ins now real** (old body in `#else`): `June_UnitFlag3c` (Arme; `!= 0`, equal to `31fceab0`'s `== 1` for a 0 / 1 flag), `JuneIsUnitTranscended`, `CX2UISkillTreeNew::JuneIsMyUnitTranscended` (now defined in `X2UISkillTreeNew2015.cpp`) and the A-TC `bTranscendent`, `JuneSelectUnitClassTran`, `IsForbiddenQuest` (`-1 != tail && my != tail && not QT_EVENT`, June `3232e510 @3232ea22`), the two hyper-class skips in `X2SkillTree.cpp` (local `JuneTreeIsMyUnitTranscended`) and the emulator's `GrantJuneSkillsByLevel` skip (reads the unit row). `X2UISkillTreeNew.cpp:2826` left (dead `#else`).
- **Log.** `[JUNE_TRANSCEND] UNIT <uid> class <c> transcendence <n> ( select|room|square|field )`, once per unit, source and value (not per refresh); `[JUNE_TRANSCEND] DB MIG v22 done` at the rung.
- **Hygiene** (`flagcheck --worktree --files` on the 18 code files): `files: 16 PASS, 0 PASS~, 2 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. `git diff --stat`: 247 insertions, 0 deletions.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,792,000 B, dated 2026-10-23 05:28:42, md5 e05dfadba9932ea0127ebf4c2db25df6, equal to X2/US_SERVICE/x2.exe (2026-10-23 05:28:35); built at main 60b60c88 + 18 uncommitted code file(s).`
- **Not touched (as the kickoff says):** slot-4 / 9 skips and the emulator's equip refusal (T4), the 42030 refusal (T2), `X2StateMenu.cpp:1885` (T4). With a unit at 1 the T4-T6 code does not exist yet, so a transcended unit shows the tree rows 14-17 and an unlocked A-TC slot but nothing else changes.

**Play-test PASSED (2026-10-23, user + logs):** all 38 units `transcendence 0`; unit 52 logged `1` (select, room) and showed tree rows 14-17, the unlocked A-TC slot and the hyper-mode awakening; `settranscend 52 0` reverted it to `0`; no ErrorLog lines, no exceptions.

**How to see it:** start the game once (the exe migrates the save to v22; log `DB MIG v22 done`), select a character: every unit logs `transcendence 0`. Close the game, `python tools/elsdb.py settranscend 52 1`, start again: unit 52 logs `1`.

### T1 - Review (2026-10-23, Opus 5.5): PASS, no code change

Read-only against June in Ghidra; `5081c06b` is the port.
- **Unit.** `3234b4a0` is `*(this + 0x3c) = *arg` = `SetUnitClassTran`. The four packet setters each read a 2-byte class then the int: `3235eb20` (+0x15 / +0x17), `32360090` (+0x21 / +0x23), `32361e00` (+0x4a / +0x4c), `323614c0` (+0x26). They store the int at UnitData slot 5, right after the class (slot 4), so the member sits beside the class (CX2Unit +0x3c). March copies it in all four setters.
- **Wire.** In all four structs the member follows `m_cUnitClass`, and Put and Get sit in the same place. The `KSquareUserInfo` / `KFieldUserInfo` copy ops (`CON_COPYCON_ASSIGNOP`) copy it. Those two have no default ctor in March, so the member is left unset like their other members and is filled only by Get. The emulator builds neither.
  - Every emulator `KUnitInfo` goes through `MakeUnitInfoFromRow` (select list, select, quest / room / shop / unit updates), and room infos go through `MakeRoomUserInfo`. A later refresh therefore never resets the flag. The bot and create-unit infos rely on the ctor's 0.
- **Rung v22.** `SCHEMA_VERSION` 22 replaces the v21 branch wholesale (`DEFAULT_RESURRECTION_STONE` kept). The `iFrom < 22` rung has one `ALTER ... DEFAULT 0` and sits after v21. `UNIT_COLUMNS` puts `transcendence` 24th, which is column 23 in `ReadUnitRow`. The unconditional `resurrection_stone` is safe because v16 already `#error`s without `OFFLINE_RES_STONE`. Both unit SELECTs use `UNIT_COLUMNS`, and the unit INSERT leaves the column to its default.
- **Stand-ins.** Each `#else` keeps the old body.
  - `IsForbiddenQuest` = `3232e510 @3232ea0e-ea2b` (`tail != -1`, `tail != unit +0x3c`, `type != 1`).
  - The hyper-class skips test `!= 0`, as June does (`32206d30 @32206d9e`, `3220d2b0 @3220d340`, `31fdd490 @31fdd57f`). So does the A-TC lock (`328e18d0 @328e195e`).
  - Only `31fceab0` tests `== 1`. `June_UnitFlag3c` uses `!= 0` for all six Arme readers, which is the same result while the save holds only 0 / 1: `elsdb.py` refuses other values, and T2's writers set 0 / 1. Not changed (no rebuild for an unreachable value).
- **Not touched, as planned:** the slot-4 / 9 skips (`X2GUUser.cpp:6422`), the emulator equip refusal (`X2OfflineSkill.cpp:674`), the 42030 refusal (`Handlers_Quest.cpp:327`) and `X2StateMenu.cpp:1885` are still there for T2 / T4.
- **Hygiene** (`flagcheck JUNE_TRANSCEND_UNIT --range 5081c06b^..5081c06b`): `files: 16 PASS, 0 PASS~ (blank/marker only), 2 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. No later commit touches the flag.
- **Kickoffs.** T2 reads right. In T4, `SetEquippedSkillFunc`'s loop moved 4 lines because of T1's `X2GUUser.cpp` insert; it now reads `:40218-40224`. No build, since no code changed; the deployed exe is still T1's (md5 e05dfadb).

## Phase T2 - Becoming transcended: quest, notify, guide

**Run with:** Sonnet 5.5 high, then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE`.

**Scope.**
- **Emulator.** `CompleteOneQuest` takes `QR_TRANSCENDENTAL_CLASS` instead of refusing (`Handlers_Quest.cpp:331-337`).
  - It applies June's conditions (second job, level 70 or more; T0's list), saves `transcendence = 1`, and returns the unit info in the ACK.
  - Then it sends June's 0x596 notify (offline event id at the end of its block, behind the flag) with June's two ints.
  - Classes allowed: an allow-list, Aisha's EM / VP / DW now (user decision 1); a refused class gets a log line.
- **Client.**
  - Port June's handler `31ef82f0` whole.
  - Port `CX2ClassChangePopup::SetShow( bShow, bTranscendGuide )` mode 1 (`324886b0`'s guide branch, `32486790` init, `32711490`, viewer `+0xd84`,
    scale / voice).
  - The quest UI's transcend pieces T0 lists.
- **Level-up guide.** June's `LUEI_CHANGE_CLASS_GUIDE_TRANSCENDENTAL` entry in `CX2LVUpEventMgr` and its accept-quest / buy buttons (1549 / 1550),
  with Aisha's `LevelUpEvent.lua` rows (shared-Lua rule: core-list BOM source, Aisha's rows).
- **Data.** Adopt the 12 `DLG_Transcendental_Change_Guide_*.lua` and `DLG_Class_Change_TranscendentalClass.lua` (data034, `adopt-june`). Stage
  `POPUP_TR_*.tga`, `TR_POPUP_IMAGE.tga` and the other guide textures (June data215, `stage-june`).
- **Dev cheat.** June's `/tc` (`ChangeTranscendentalClassCheat`), answered by the emulator (dev auth only).

**Done gate:** completing 42030 on a test unit (or `/tc 1`) logs `[JUNE_TRANSCEND] CHANGE`, saves `transcendence 1`, opens Aisha's
guide popup, and refreshes the tree; a relog keeps it; a non-second-job / under-70 unit is refused with June's error.

### T2 - Done (2026-10-23, Sonnet 5.5)

Flag `SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE` (`Always.h` + `ServerDefine.h`, `SECOND_HOMES`; `#error` guard needs `_UNIT`, `SERV_IRUHADEV_OFFLINE`, `SERV_IRUHADEV_JUNE_QUEST_OFFLINE`). Log `[JUNE_TRANSCEND] CHANGE`.
- **Events.** June 0x594 / 0x595 / 0x596 at the end of the client block: `EGS_ADMIN_CHANGE_TRANSCENDENTAL_CLASS_REQ` (int) / `_ACK` (int, June `31ef82a0` reads one int) / `EGS_CHANGE_TRANSCENDENTAL_CLASS_NOT` (two ints, the first unknown = 0). Names are ours.
- **Emulator** (`Handlers_Quest.cpp`). `CompleteOneQuest` takes `QR_TRANSCENDENTAL_CLASS` (42030): `JuneTranscendCheck` (class 100-199 and level 70, else NetError 1687; already transcended 1688; allow-list Aisha EM 104 / VP 105 / DW 113, else 1726, user decision 1), refusal before anything is spent, save `transcendence = 1`, unit info in the ACK, the NOT after the ACK (`m_bTranscended` in `KQuestAfter`). `/tc` REQ handler: `n != 0` transcends with the same checks, `0` clears (stand-in).
- **Client NOT** (`X2State.cpp`, June `31ef82f0` whole): flag, `InitSkillTreeUI`, guide popup when != 0, `UpdateUI`, item-stat key (`SetUnitClassTran`, T5 reads), cash shop closed, `GiveUpForbiddenQuest` (`3232ecd0`). Not ported: the class-up notice buttons (vslot `0x104`, Stage 9); one log line says so. `/tc`: `CX2Main::ChangeTranscendentalClassCheat` (`31d809c0`, auth 3), June's `Slash_tc` already ships in `GameEditCommandList`.
- **Popup** (`X2StateClassChange.*`). `SetShowTranscendGuide` = `324886b0` mode 1 (a separate name: March's `SetShow( bool, int )` takes `HPT_SHARE_BANK` = 1); dialog by unit type 1-12; `JuneInitTranscendGuide` = `32486790` / `32710710`; `JuneIsTranscendGuideName` = `32711490`. The popup's `+4 / +8` members (image static, 1.5) are not ported: no reader.
- **Level-up guide** (`X2LVUpEventMgr.*`). Event 6, cases 1537 / 1549 / 1550 (`32712790`), `JuneCloseChangeClassGuides` (`32710b60`), the name test in `CheckLevel` (`327141e0`); 1549 -> `CX2QuestManager::JuneGuideAcceptClassUpQuest( 9, 0 )` (`32344ab0`, JUNE_BUGS 412) + `JuneGetClassUpStep` (`31edf870`); 1550 -> menu type 0x1d + `JunePushOpenAtItemID( 288319 )` (`3222a4f0( 0 )`; `X2CashShop.cpp` `case 0x1d` joins the T4 tab-3 case). March has no 1541-1548 (the kickoff's "copy March's 1541 case" has nothing to copy).
- **Quest.** The compare is in `GetNewQuest` (June `32337320 @3233743b`; T0's row said `SetUnitQuest 32336e80`, funcmap corrected).
- **Image.** `GetCharacterImageName` 5-argument overload. I read the 870 image rows off June's static initializer (ctor `31ef0ef0`, four wstrings per 0x70-byte row, the second pair is the transcended file / piece) with a scratch script; Aisha's three classes get the pair for 160by160, gray, 50by50, 20by20, gauge, party. Dungeon clear is the same picture, class initial has none, the unit slot (type 9) is Stage 9. `CreateSkillTreePortrait` passes `bTranscended_`.
- **Gauge portrait (play-test finding, user).** The top-left HUD portrait stayed the normal one. `CX2MyGageUI::SetCharacterImage` and the two berserk-end redraws (Chung, Ara) in `X2GageUI.cpp` now pass my unit's flag to the 5-argument `GetCharacterImageName` (`CIT_MyGage`); the NOT handler redraws the gauge once (June's NOT does not; a relog would). The party bar, character info, inventory, character select and result windows still use the 4-argument form: Stage 9. flagcheck for the two files: `1 PASS, 1 PASS~`, `9 EOL-mismatched` (the helper at the top of a CRLF-headed LF file).
- **Guide popup fix (play-test finding, user).** The popup was created but drew nothing and held the cursor: the 8 `.TET` piece files of the staged textures (`POPUP_TR_*`, `TR_POPUP_IMAGE/TEXT`, `DLG_UI_Image_09/10`, `DLG_UI_Common_Texture_jump_event_12`) had not been staged. Staged now (data215 6, data052 1, data054 1) and repacked. The X / OK button (1537) sends a message that closes only event 6's own dialogs, so the popup the NOT opens could not be closed: `CX2ClassChangePopup::CloseTranscendGuide` is called from the 1537 case (labelled gap, June's close path for this popup is not in the decompile I read). Deploy md5 `8dce3170c123504c20e02e991ccf9321`. **The five koms must be copied again (data052, data054, data215).**
- **Passive skill icons (play-test finding, user).** Icon census (scratch script over `NewSkillTempletVer2`, texture + `.TET` piece lookup in `kom_contents`): all 132 Aisha skills have their texture except the six transcendence passives 22412-22417, which sit in `DLG_SKILL_Passive_12.tga`; the same sheet holds every class's transcendence passives (16 pieces) and the common "Awakened One" (106, `RNSI_P_COMMON_HYPER_AURA`). `stage-june` DLG_SKILL_Passive_12.tga + .TET into data063, `PACK OK data063`; **copy pending**. No code change.
- **Quest list face box (play-test finding, user; the empty square beside "Power of Transcendance 1/5").** It is the quest NPC's face (`JuneGetNPCFaceTextureKey( 10153 )` = `Trans`); March's `DLG_UI_Npc_Face01.TET` (53 pieces) has no `TRANS` / `STORY`. `stage-june` replaced `DLG_UI_Npc_Face01.tga` + `.TET` in data063 with June's (70 pieces; March's copy is in `kom_backup_stage_june61023_195822`), `PACK OK data063`; **copy pending**. My earlier reading of the type label (`DLG_UI_Text_05` `TRANSCENDENCE`) was the wrong box: that art was already present.
- **NetError text.** `X2NetError.cpp` `GetErrStr` returns June's English text for 1687 / 1688 / 1726 (past `ERR_SENTINEL`).
- **Data.** `adopt-june` data034: the 12 `DLG_Transcendental_Change_Guide_*.lua` + `DLG_Class_Change_TranscendentalClass.lua` (13 new, `june2017_re/june_adopt_20261023_080637.tsv`); data036 `Enum.lua` (+3 `LUEMCUM_*`, `LUEI_..._TRANSCENDENTAL` 6, `refresh`) and `LevelUpEvent.lua` (`convert` T0, event 6 with Aisha's row only); `stage-june` 16 files (data215 14: `POPUP_TR_E_MASTER / V_PRINCESS / D_WITCH`, `TR_POPUP_IMAGE`, `DLG_UI_Common_Texture97/98/100_NEW` (+gray, .TET), `DLG_UI_Image_09/10`; data052 `TR_POPUP_TEXT`; data054 `DLG_UI_Common_Texture_jump_event_12`). `ledger --check` PASS. Packed: `PACK OK` data034 / 036 / 052 / 054 / 215; **copy pending** (ask).
- **T0 rows not done, with owner:** `32712e50` is the skill-command panel (S7e), not the level-up manager (funcmap); `328dcba0` is Luciel (unit type 10; checklist); the cash shop's item preselect `+0x334` (June's single member) is replaced by T4's open-at-item list.
- **Hygiene** (`flagcheck --worktree --files` on 27 files): `files: 19 PASS, 5 PASS~, 2 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. (A first spec with repeated `@@ file` sections silently dropped six edits; found by the per-file counts and re-applied.)
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,809,408 B, dated 2026-10-23 18:54:21, md5 e8928f5a6fe209dfc63dea5f6cc0d6ef, equal to X2/US_SERVICE/x2.exe (2026-10-23 18:54:15); built at main 03b0e6de + 2 uncommitted code file(s).` (first deploy md5 551b034e; rebuilt after the gauge fix below). Strings checked in the exe: the Aisha guide file name, `TR_`, `You cannot Transcend.`.
- **Open for the user:** `data054` has 66 entries that differ from the live archive besides mine (earlier staging never copied); copying it deploys them too. Say which koms to copy.

**How to see it:** auth level 3, `/tc 1` on unit 52 (`ArmeVP`, level 70+): log `CHANGE /tc 1 ... transcendence 0 -> 1` and `CHANGE NOT sent`, the guide popup opens (Aisha's, VP picture, Close button), the skill tree shows rows 14-17, a relog keeps it. `/tc 1` again: NetError 1688 text; a first-job or under-70 unit: 1687; another class: 1726 and a log line. `/tc 0` clears it.

### T2 - Review (2026-10-23, Opus 5.5): six fixes under the same flag

Ghidra (read-only) and raw capstone (`disasm_range.py`) against T2's commits `4c82e1ff..e60435c0`. New June rows: funcmap `Transcend T2 review`.
- **Matches June:** NOT `31ef82f0` call for call (flag from the second int, `InitSkillTreeUI`, popup when != 0, `UpdateUI( 1, 1, 0 )` = March
  `( true, true, true )` as S7d settled, `328d6f40`, shop close via vslot `0xac`, `3232ecd0`; vslot `0x104` stays Stage 9's). Class-to-picture
  tables of `32486790` / `32710710`, the popup statics, the four buttons, the unit-type file table. Level-up cases (the function starts at
  `327127a0`; 1537 returns 0, 1549 `32344ab0( 9, 0 )`, 1550 `3222a4f0( 0 )`), `32710b60` events 2 / 4 / 6, `31edf870`, the msgbox at
  ( 250, 300 ). `GetNewQuest @3233743b` class tail. Aisha's six transcended pieces are in the `.TET` files the image table names. NetError
  texts are verbatim; slots `337379b8 + id x 4` confirm 1687 / 1688 / 1726. Emulator checks, the quest ACK-then-NOT order and `/tc` (`% 1000`, auth 3) match.
- **Fix 1, popup close.** June has the close path: `32488c50` (the popup's `UICustomEventProc`, called after the level-up manager as in March)
  deletes the dialog on 0x601. Ported there; T2's `CloseTranscendGuide` and its static are removed. `m_bShow` stays 1 (JUNE_BUGS 413).
- **Fix 2, the image fade.** T2 said the popup's `+4 / +8` had no reader. It has one: `32486aa0` -> `324869d0` fades `Static_Transcendental_Image`
  picture 0's alpha over the 1.5 s timer. The level-up event's `+0x2c / +0x30` get the same fade (`327108b0`, from `32714ad0` after
  `CheckLevel`; `DeleteDialog` clears the image). Ported as `JuneFadeTranscendImage`, with the timer and image filled by `JuneInitTranscendGuide`.
- **Fix 3, the 1550 shop toggle.** `3222a4f0` toggles only outside the non-menu states. T2 cast any state to `CX2StateMenu` (UB in dungeon result). Now
  June's list by March name, plus `XS_TRAINING_RESULT` / `XS_ARCADE_RESULT` (March's other non-`CX2StateMenu` states).
- **Fix 4, server group, not unit type.** Templet `+0` in `GetNewQuest` and `32344ab0` is `m_iServerGroupID` (Q4e), which March's `GetQuestTemplet`
  already filters. T2's added unit-type tests are removed.
- **Fix 5, item-stat key.** June's `CX2Unit::Reset` (`@32362b2a`) and server select (`@3257b72a`) pass unit `+0x3c` to `328d6f40` too. March's two
  `SetUnitData` sites now call `SetUnitClassTran`, so T5's reader is right after a relog.
- **Fix 6.** The guide-name test is case-insensitive like `32714b90` (`_wcsicmp`). No behaviour change for the shipped file names.
- **Hygiene.** T2 range `--range 4c82e1ff^..e60435c0`: `files: 23 PASS, 6 PASS~, 2 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted,
  9 EOL-mismatched` (the `X2GageUI.cpp` helper, Done note); `RESULT: PASS`. The OTHER is T4's `_SLOT` shop-product block, which went into the same commit
  in `X2OfflineDB.cpp`. Review edits (`--worktree --files`, 7 files): `7 PASS, 0 FAIL`; `26 deleted line(s)`, all T2's own flagged lines replaced above; `RESULT: PASS`.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,817,600 B, dated 2026-10-23 20:27:39, md5 bc7ef818d400b61727ab20e78d5e2947, equal to X2/US_SERVICE/x2.exe (2026-10-23 20:27:32); built at main a112fb31 + 7 uncommitted code file(s).`
- **T3 kickoff:** checked; now says to extend T2's `GetErrStr` switch and reuse `JuneTranscendCheck` / `JuneSendTranscendNot`.

**How to see it:** `/tc 0` then `/tc 1` on unit 52: the guide popup's large image flashes in and fades out within about 1.5 s. The OK / X button
closes the popup, and the log shows no `CloseTranscendGuide` line (that stand-in is gone).

## Phase T3 - The transcendence items

**Run with:** Sonnet 5.5 high, then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_TRANSCEND_ITEM`.

**Scope.**
- Adopt `TranscendentalClassItem.lua` (data036; system data, whole).
- Bind `AddTranscendentalUnitType` / `AddTranscendentalClassItem` in `CX2ItemManager` (June `31db3c8f` / `31db3f10`). Load it where June does
  (after `EventSealItemData`, before `UnlockSocketItemData`, `X2Data.cpp` `ResetItemManager`).
- The ticket-use request / refusals as T0 maps them: client checks and NetErrors 1675-1727. The emulator applies a transcendence ticket (class
  change within the transcended rank) and refuses a second-job ticket on a transcended unit (1727).
- The Skill Slot Change Medal (Transcendence) itself is T4's.

**Done gate:** each Aisha ticket (288323-288325, 242160-242180) behaves as June (T0's table), with the error text June shows; log
`[JUNE_TRANSCEND] ITEM`.

### T3 - Done (2026-10-23, Sonnet 5.5)

Flag `SERV_IRUHADEV_JUNE_TRANSCEND_ITEM` (`Always.h`; `#error` guard needs `_CHANGE` and `_SLOT`). Log `[JUNE_TRANSCEND] ITEM`.
- **Map and data.** `CX2ItemManager`: `AddTranscendentalUnitType` `328189d0`, `AddTranscendentalClassItem` `3280b6c0`, getter `32807fe0`, and `IsTranscendentalUnitType` `3280a090` (the T0 row for it was missing: class 0 / empty set -> true, code-event clauses 0x920 / 0x921 / 0x923, then the set lookup). Bound in `X2Main.cpp`. `TranscendentalClassItem.lua` adopted (`adopt-june`, June's bytecode, data036) and loaded at the end of `ResetItemManager`: March has no `EventSealItemData` / `UnlockSocketItemData` load to sit between. One `ErrorLog` line `JUNE_TRANSCEND ITEM load=1 288319=0 288323=1 288318=1` proves the load.
- **Client check.** `X2UIInventory.cpp` `case 288493` (June `321aa5d0 @321bf5ef-321bf88c`): type map miss -> 1726, type 0 and transcended -> 1727, type 1 and not transcended -> 1675 (June's `0x68b`, JUNE_BUGS 414: dead for 288493, which is type 0), else the generic use request. The job-ticket cases 163200 / 163316 / 163323 / 163325 / 161854 have no March case (tickets are claimed from the cash shop, never right-clicked): not ported, routed.
- **NetError.** `X2NetError.cpp`: June's texts for 1675 / 1676 / 1677 / 1684 / 1686 / 1727 (strings read from `unpacked_x2.exe` 0x1950398-0x19508e0).
- **Emulator (stand-ins).**
  - Book 288493 (use from the bag, `Handlers_Inventory.cpp`) and 288319 (claim, `Handlers_Shop.cpp`): transcended -> 1727 first, else `JuneTranscendCheck` (1687 / 1688 / 1726), then flag 1 + `JuneSendTranscendNot`. The medal June mails with it is not given.
  - Claim path: type 0 item on a transcended unit -> 1727; type 1 on an untranscended one -> 1686 (covers the 242130-242300 second-job tickets and 288318).
  - Tickets 288323-288325 (type 1, class from item.kim): same class 1676, another character 1675, off the allow-list through `JuneTranscendCheck` on the target class; otherwise the existing class-change path, transcendence kept.
  - Medal 288318 untranscended: 1686 (T4's refusal, new id). Three shop products 288323-288325 in category 33 (`X2OfflineDB::Open`, idempotent).
- **Routed (scope rule).** `32869940` (T0's `32869900` was the region start) is `CX2Inventory`'s job-ticket kind helper, read by the inventory ticket confirm `@32190c09` and `CX2CashShop::MouseLRButtonUpEquip` `@3290d692`; with NetError 1737 / 1738 and the right-click ticket cases it is a bullet under Stage 9's "Transcendence in the June UI" (`JUNE_PORT_BACKLOG_ORDER.md`). The cash shop's type filter (`3222a840` ...) was already routed there by T0, so the shop still lists every ticket.
- **Data.** data036 packed (`PACK OK`, +1 `TranscendentalClassItem.lua`); **copy pending** (ask). Revert: delete the file, its manifest key, repack.
- **Hygiene** (`flagcheck --worktree --files` on 10 files): `files: 5 PASS, 4 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,824,768 B, dated 2026-10-23 20:59:04, md5 e4689c70e8cf3aec59c9d65e0d9b28c3, equal to X2/US_SERVICE/x2.exe (2026-10-23 20:58:57); built at main 223eeec9 + 10 uncommitted code file(s).`
- **How to see it:** auth level 3; unit 52 (`ArmeVP`, 105). `python tools/elsdb.py settranscend 52 0`, then `/additem 288493`: right-click it in the bag -> the unit is transcended, the guide popup opens (log `ITEM book 288493 used`). A second `/additem 288493` right-click -> pop-up "You can no longer use this item after Transcendence." In the cash shop (category 33): 288323 on VP -> class changes to EM, transcendence kept; 288324 on VP -> 1676; 242160 on a transcended unit -> 1727; 288323 on an untranscended unit -> 1686.
- **Open for the user:** copy data036 (the file above is the only new entry). No design question.
- **Item images (play-test finding, user).** 288493 / 288319 / 288318 showed no picture: `HQ_Shop_Item_<id>.dds` is a June-only file in none of March's koms. June's item pictures in general are Stage 15's (`JUNE_ASSET_PORT_PHASES.md` waves, P2-P8), not T3's. For the transcendence items, `stage-june` took the 84 `HQ_Shop_Item_*.dds` of `plan_tci_rows.tsv` + the cube 293722 (79 found in June; 51 new, 1 replaced, 27 identical; refused: 288493 / 288509 / 288510 / 294780 / 294781 have no file of their own, 288493 uses 288319's): data130 (+40, ~1), data192 (+6), data198 (+7), packed; **copy pending** (ask). Backup `kom_backup_stage_june/20261023_210729`.

### T3 - Review (2026-10-23, Opus 5.5): two fixes under the same flag

Ghidra (read-only) and raw capstone (`disasm_range.py`) against `32df3d2f`. New June rows: funcmap `Transcend T3 review`.
- **Matches June:** `3280a090` (map miss -> false; class 0 or empty set -> true; `g_pInstanceData` `33775864` non-null, then 0x920 / else-if 0x921 /
  else-if 0x923 through `IsEnableCode` `31e5a860`; then `set.find`), `3280b6c0` (insert-if-absent), `32807fe0` (-1 when absent). The load: `31edda90`
  loads it unconditionally with `3280d3c0` between `EventSealItemData` and `UnlockSocketItemData`; March has neither, and the maps are independent,
  so the end of `ResetItemManager` is equivalent. The three tests T3 ported (`@321bf5ef-321bf88c`: getters `31e4c370` / `31e4c380` = templet type / class,
  type 0 and `+0x3c != 0` -> 0x6bf, type 1 and `+0x3c != 1` -> 0x68b, else `32172190` = the use request). All nine texts (1675-1677, 1684, 1686-1688,
  1726, 1727) equal June's English table `337379b8` (the Spanish one at `337397c0` is chosen by `g_pMain +0x708`). Job-ticket cases: no March case
  (grep), routing to Stage 9 right. Emulator stand-ins read as the Done note says, and are labelled in `MODS.md`.
- **Fix 1, the case starts earlier.** June's case 288493 is `@321bf35f` (jump `@321ba228`), and runs three checks before T3's: `GetItemTemplet( 288493 )`
  null -> 0x1f4 (`ERR_USE_ITEM_IN_INVENTORY_00`, same text "The item cannot be used."); my level (`31edefe0`) < 70 -> 1687; the class step
  (`31ef05e0`: 2 only for classes 100..199) < 2 -> 1687. Ported in that order; a first-job or under-70 unit is now refused by the client with no packet.
- **Fix 2, the binder's class argument.** `328189d0` takes ( type, class ) and inserts the class into the type's set when it is non-zero. T3 bound a
  one-argument function. June's Lua passes only the type, so the class reads 0 (lua_tinker) and nothing changes in play; the body is now whole.
- **Hygiene.** T3 range `--range 32df3d2f^..32df3d2f`: `files: 5 PASS, 4 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted,
  0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Review edits (`--worktree --files`, 3 files): `3 PASS, 0 FAIL`;
  `6 deleted line(s)`, all T3's own flagged lines replaced above; `RESULT: PASS`.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,824,768 B, dated 2026-10-23 21:28:18, md5 db3ace7d0f827a8181ef22e3d96efd85, equal to X2/US_SERVICE/x2.exe (2026-10-23 21:23:13); built at main 4dcf54e7.`
- **Data.** data036 is in the game (`data036.kom` md5 `96b15430` = `data036_packed.kom`); the 21:01 run logged `load=1 288319=0 288323=1 288318=1`.
- **Kickoffs.** `T4-review.md`: notes the medal's 1686 inside `_SLOT`'s block. `T5.md`: still right (T3 touches none of its files).
- **Open (no question):** the bag-use path refuses only the medal and the book by type; other type-1 items (288509 / 288510 / 288355 / 288357 / 288320-2)
  used from the bag are not gated, because June's server is not readable. They are claimed from the shop, which is gated.

**How to see it:** `/additem 288493` on unit 45 (`ArmeHM`, lv 60) or unit 32 (`MagicKnightT`, 1st job): right-click -> "Transcendence can only be
reached by 2nd Job Class that are over Lv.70.", and no `EGS_USE_ITEM_IN_INVENTORY_REQ` in `offline_packets.log`.

**Play-test PASSED 2026-10-23 (user + logs, md5 db3ace7d):** `/additem 288493` on units 45 and 32, no use request sent (1687 popup); unit 54 `ITEM book 288493 used: unit 54 transcendence 0 -> 1`, NOT and guide popup; a second book on 54 sent nothing (client 1727); `ITEM ticket 288324 ... unit 52 ... NetError 1676`. 0 EXCEPTION.

## Phase T4 - The TC slots (A-5 / F and B-5 / T)

**Run with:** Sonnet 5.5 high (Opus if T0 finds a new mechanism), then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_TRANSCEND_SLOT`.

**Scope.**
- `CX2GUUser::SetEquippedSkillFunc` / `ChangeEquippedSkillState` drop the index-4 skip for a transcended unit (A) and for slot-state key 1 (B),
  as June `32024b00` / `31fcadd0` do. `USI_SPECIAL_ATTACK_5` / `_SLOTB_5` get their states.
- **Emulator:**
  - lift the equip refusal into 4 / 9 per June's rule;
  - key 1: permanent for a transcended unit (user decision 2, a labelled stand-in); the medal 288318 and the quest's cube 293722 keep June's behaviour;
  - the cool-time / attack slot buffs 843 / 844 if T0 puts them here.
- **UI:** the lock statics (`328dfab0`, `328e18d0`) already read T1's field. A locked-slot click (`SMUCM_DISABLED_TRANSCENDENTAL_*`, 208 / 209)
  opens what June opens (T0).

**Done gate:** on a transcended unit, a skill in A-5 casts from F with its state; B-5 follows key 1. On an untranscended unit both stay
locked with June's message; log `[JUNE_TRANSCEND] SLOT`.

### T4 - Done (2026-10-23, Sonnet 5.5)

Flag `SERV_IRUHADEV_JUNE_TRANSCEND_SLOT` (`Always.h`; `#error` guard needs `_TRANSCEND_UNIT`, `_SKILL_SLOT_UI`, `_SKILL_CASH_OPEN_ITEM`). Stand-ins are in `MODS.md`.
- **Skips gone** (`X2GUUser.cpp`; old body in `#else`): `ChangeEquippedSkillState` x2 (the dormant return; a `[JUNE_TRANSCEND] SLOT equip TC slot` line replaces the dormant log), `SetEquippedSkillLevelCommon` and `SetEquippedSkillFunc` loop all five slots. June `32024b00` loops five slots with no flag test; the lock is `3220e3b0` (equip) and `328e18d0` (unequip), both already ported. The state arrays, `GetSkillSlotIndexUsed` and `GetSkillIDByEquipSlotID` already had the fifth slot; Aisha's `June_SetEquippedSkillFuncHead` takes index 0-4 and her `JuneCommonSpecialAttackEventProcess` is `31fcadd0` (F -> 4 with no lock test, T -> 9 only on key 1), so a skill in A-5 / B-5 casts for her with no new class code.
- **March classes.** They read pressed indexes 0-7 (checklist 7ag); F / T would give a non-NULL slot with index -1 and index the state arrays at -1. `CommonSpecialAttackEventProcess` now returns with no slot for F / T (log `SLOT F / T pressed on a March class`, first 5). Each class's TC case comes with its port (checklist 7ao).
- **Click on a locked slot** (`X2StateMenu.cpp`, June `32476050` 182 / 183): unit flag 0 (A) or `32890ea0( 1 )` 0 (B); in a game outside the battle field the chat text STR_ID_16478 (the quick-slot text), else `m_iMenuTypeCallingCashShop = 0x32` and `JunePushOpenAtItemID` 288319 (A, Book of Transcendence) / 288318 (B, medal), `ToggleCashShop`. `X2CashShop.cpp` maps 0x32 to the consume tab (June `3224a680`: 0x32 shares tab 3 with 0x1d / 0x26 / 0x30 / 0x31; the tab-3 = consume mapping is by analogy with `UI_SKILL_NOTE`).
- **Emulator.** `CX2OfflineSkill::IsUnitTranscended()` reads the save. The equip refusal for ids 4 / 9 applies only to an untranscended unit; slot-state key 1 is sent PERMANENT for a transcended unit (user decision 2). Medal 288318 (`Handlers_Inventory.cpp`): untranscended -> refused, item kept; transcended -> consumed, log `medal 288318 used - slot-state key 1 is already permanent`. There is no expiry store: for a transcended unit the medal and the cube 293722 change nothing (key 1 is already permanent), and the cube's grant is T2's quest reward. NetError 1684 / 1686 for the client side is T3.
- **In-game cool-time loop (kickoff question).** `UpdateSkillCoolTime` already runs all ten slots (S6) with no TC skip. `322d8100` is not a cool-time loop: it is the guide-4 TC-slot particle (hyper-mode time, BTI 0x334, `31d71810( 4 )`), so there is no `break` to keep and no `JUNE_BUGS.md` row. Routed to Stage 9 (`JUNE_PORT_BACKLOG_ORDER.md`, "The TC-slot class-guide effect").
- **BFI 843 / 844** (`m_iSkillSlotType` 3): T4 added nothing; `GetSkillSlotIndexUsed` already returns index 4 for the TC states and `IsTranscendenceSlotIndex` resolves 4 / 9. T5 applies the buffs.
- **Follow-up (play-test, 2026-10-23).** The locked-slot click opened the shop with no item: the buy pop-up keeps only items with a product (`JuneOpenAtItemList`) and 288318 / 288319 were not in `cash_product`. `X2OfflineDB::Open` now adds both in category 33 once, idempotent, no schema change (stand-in: June's billing DB is not readable; 288319 gives an inert book until T3). It rode in commit `4c82e1ff` (T2's, whole-file stage). Deploy: `DEPLOY OK: X2_offline.exe is 25,808,384 B, dated 2026-10-23 08:32:47, md5 551b034e1f3fb3b4428257304904fcdf`. Log `[JUNE_TRANSCEND] SLOT shop products 288318 / 288319 ensured`.
- **Hygiene** (`flagcheck JUNE_TRANSCEND_SLOT --worktree --files` on the 7 code files): `files: 3 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,794,560 B, dated 2026-10-23 07:26:59, md5 57f3849e8cd8f05cd124be340cf6a467, equal to X2/US_SERVICE/x2.exe (2026-10-23 07:26:52); built at main a6ff8002 + 15 uncommitted code file(s).` The exe also carries a peer's uncommitted T2 work (`SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE`).

**Play-test hand-off** (log tag `[JUNE_TRANSCEND] SLOT`; unit 52 is `ARME_VOID_PRINCESS`, level 99):
1. *Untranscended, locked.* Game closed: `python tools/elsdb.py settranscend 52 0`. Start the game, pick unit 52, open the skill window (K). Look for: A-5 and B-5 locked. Click the locked A-5: the cash shop opens on "Book of Transcendence" (village), or in a dungeon the yellow chat text. Log: `click on a locked A TC slot: shop opens on item 288319`. Click B-5 (expand B first): item 288318.
2. *Transcended, A-5 / F.* Close the game: `python tools/elsdb.py settranscend 52 1`. Start, pick unit 52. Drag any active skill into A-5 (the fifth A slot). Look for: it stays; log `equip TC slot 4 A skill <id>`. Enter a village or dungeon and press **F**: the skill casts with its own animation. If not: `grep "SLOT" offline_server.log`, and the `JUNE_SKILL` blank-state line.
3. *Transcended, B-5 / T.* Expand B, drag a skill into B-5, press **T**: it casts. B-5 unlocked without the medal (key 1 permanent, stand-in). Log: no `refused` line.
4. *Reverting.* Close, `settranscend 52 0`, start: a skill left in A-5 / B-5 is unequipped by the UI (June `328e18d0`) or shows locked.
Then say "check logs".

### T4 - Review (2026-10-23, Opus 5.5): one fix under the same flag

Ghidra (read-only) against `c70dc0db` and `03b0e6de` (and the `cash_product` rows that rode in `4c82e1ff`). New June rows: funcmap `Transcend T4 review`.
- **Skips: match June.** `32024b00` loops `0..4` (`while < 5`) with no flag test; its callers are vtable slots only (13 class vtables). `3220e3b0` gates
  slots 5-8 on key 0 and slot 9 on key 1, never slot 4: A-TC is locked by the UI (`328dfab0`) and the emulator's refusal. March's four loops now run five.
- **Press.** `31fcadd0`: F (`+0x872b`) -> slot 4 with no lock test; T (`+0x872f`) -> slot 9 only when `32890ee0( 1 )`, else on to the expand test and
  the B keys. Aisha's `JuneCommonSpecialAttackEventProcess` follows that order. Only `X2GUUser.cpp` reads `oneSkill5` / `oneSkillEx5` (grep over
  `X2Lib` / `KTDXLIB`: the March guard, Aisha's press, the input clear, `X2Game.cpp`'s key map), so no March class indexes -1.
- **Fix: T tests my unit's key 1.** June calls `32890ee0` on `g_pData +0x68 +0x90 +0x494` (my select unit's tree, `@31fcaf87`), not on the pressing
  unit's. Both March sites used `cUserSkillTree_`. Now `JuneIsMyUnitSlotUsable( 1 )`, kept verbatim (JUNE_BUGS 415). No change for the player's own unit.
- **Click: matches June.** Cases 0xb6 / 0xb7: `+0x3c == 0` / `32890ea0( 1 ) == 0` (March `GetJuneSlotState( 1 )`, `SSBES_EXPIRED` = 0); `32530a80(
  &g_pX2Game, 0 )` = `g_pX2Game != NULL`; state 0x15 = `XS_BATTLE_FIELD` (the quick-slot and skill-slot cases use the same test); STR_ID_16478 in
  `#CFFFF00`; else `+0x31c = 0x32`, push 288319 / 288318, `ToggleCashShop`. **Tab:** `3224a680` passes March's own `CASH_SHOP_CATEGORY` numbers to
  `32e41110` (the GFx `cashShop_selectCategory`): 1 / 2 -> 4 (`CSC_INSTALL`, as March has for quick / skill slot), 0x32 -> 3 = `CSC_CONSUME`, so the
  mapping is exact, not an analogy. The `CSSC_CONSUME_ETC` sub-tab comes from March's `UI_SKILL_NOTE` body (June's shop is GFx, Stage 9).
- **`322d8100`: confirmed** a slot particle on 4 / 9 (type 4 through `321536e0` / `32151fa0`), not a cool-time loop; the Stage 9 routing stands.
- **Emulator: matches the Done note.** `IsUnitTranscended` reads the loaded unit; the refusal for 4 / 9 and key 1 follow it; the medal block runs
  before every route-A check and then reaches `ConsumeOne`. `JuneItemHasNoModel` gets a templet already null-checked above it.
- **Stand-in labels.** Key 1 permanent and the medal were in `MODS.md`; the two `cash_product` rows and the model-less file-check exemption were
  not, and are now.
- **Hygiene.** `--range c70dc0db^..c70dc0db`: `files: 3 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token,
  0 encoding change(s)`; `RESULT: PASS`. `--range 03b0e6de^..03b0e6de`: `1 PASS, 0 FAIL`, `RESULT: PASS`. Review edit (`--worktree --files
  X2Lib/X2GUUser.cpp`): `1 PASS, 0 FAIL`; `1 deleted line(s)`, T4's own guard line inside `_SLOT`, replaced; `RESULT: PASS`.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,825,280 B, dated 2026-10-23 21:39:12, md5 2d6ffafeb0b8cb221799112babcc8862, equal to X2/US_SERVICE/x2.exe (2026-10-23 21:39:05); built at main 6289e3e0 + 1 uncommitted code file(s).`
- **Kickoffs.** `T5.md`: still right; its `HyperModeStart` line ref is now `X2GUUser.cpp:32181`.
- **Open (no question):** T4's untranscended checks (locked slots, the shop click) were never played; the hand-off below covers them.

**How to see it:** unit 52 transcended: B-5 / T casts as before. Untranscended: the locked A-5 / B-5 clicks open the shop on 288319 / 288318.

**Review play-test PASSED 2026-10-23 (user + logs, md5 2d6ffafe):** unit 52 `transcendence 0` on select and room; `SLOT click on a locked A TC slot: shop opens on item 288319` (21:42:38) and `... B ... 288318` (21:43:03) in the village; no click line during the 21:43:17 dungeon visit (the chat branch logs nothing); 0 EXCEPTION, 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` (not this phase). The transcended T press was not in this run.

## Phase T5 - Transcended combat engine

**Run with:** Opus 5.5 high, then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_TRANSCEND_ENGINE` (+ `_DEBUG`).

**Scope (T0's list is the work list).**
- The transcended branches of `HyperModeStart` `31ffd490` and `ApplyHyperModeBuff` `32298590`.
  - `32298590` plays `Effectset_TRANScendence_WakeUp_FX` into `+0x9f8`, stopped by `32296cb0`.
  - `BFI_TRANSCENDENTAL_HYPER_MODE` 845.
  - Port June's whole function wherever March has it.
- `CommonFrameMove` `32063650`'s `== 1` branch, `3201a8f0`, the aura `32028790`, `322ece70`'s 1.3 summon factor, the `32610cb0` region, and
  `USI_TRANSCENDENTAL_HYPER_MODE` if an engine path enters it.
- **Stat table:** June's `TRANSCENDENTAL_EQIP_POSITION_RELATIVE_STAT_TABLE` (weapon row 0.08 / 0.08) read by June's loader `328d7890` and used
  for a transcended unit (wrong combat numbers otherwise). `CharacterStatTable.lua` is adopted as T0 decides.
- **Hyper cut-in names:** a transcended class uses `UnitTemplet`'s `HQ_Cutin_*_Transcendence` (`.gfx`, june-gfx G4's widget).

**Done gate:** a transcended unit's awakening plays the WakeUp FX, its weapon stat follows June's row, its summons get the 1.3 factor while
awake; each branch has a `[JUNE_TRANSCEND] ENGINE` line (the `_DEBUG` ones retire in the review).

### T5 - Done (2026-10-23, Opus 5.5)

Flag `SERV_IRUHADEV_JUNE_TRANSCEND_ENGINE` (+ `_DEBUG`), commit: this one. Funcmap rows `Transcend T5` (13). JUNE_BUGS 416. MODS entry "T5".
Hygiene: `flagcheck --worktree`: 7 PASS, 1 OTHER (the `Always.h` define block), 0 FAIL, 0 deleted, RESULT PASS; the 6 EOL notes are the
`RenderRank` declaration beside an LF-only studio line in `X2GUUser.h`.
`DEPLOY OK: X2_offline.exe is 25,828,352 B, dated 2026-10-23 22:22:24, md5 b6f6d8bbf4b61dadcbd878621222a646` (the four T5 strings are in the exe).

What landed (all inert until a unit's `transcendence` is non-zero):
1. **Awakening.** `HyperModeStart` (`31ffd490 @31ffd57a`) -> `JuneApplyTranscendSlotBuffs` (`31fc8c20`): 20 s timer, `GetRandomFloat( 0 ) < 0.5`
   -> BFI 843 + 844. `OnFrameMove` (`3206eaf0 @3206fed1`) re-rolls every 20 s while `JuneIsTranscendHyperMode` (`31fce5f0`: BTI 822 / 823).
2. **ApplyHyperModeBuff** (`32298590`): a transcended user gets BFI 845 instead of BFI 1 and plays `Effectset_TRANScendence_WakeUp_FX`, tagged
   `32296f30( h, 1, 0 )`. **Correction:** June keeps no handle for it; `+0x9f8` is written only by the untranscended customize branch (`@32298c4d`).
3. **RenderName** (`3201a8f0`, whole): rank returns bool and shifts the edge 24; `ICON_TR` (`31ff6920`); dungeon 41450 colours; no couple line.
4. **Summons**: awake factor 1.3 for a transcended user (`322ece70`, `X2Game.cpp`). **Buff factors**: `31fceab0`'s `== 1` hyper-class gate in
   `ParsingBuffFactorID`.
5. **Stat table**: `TRANSCENDENTAL_EQIP_POSITION_RELATIVE_STAT_TABLE` loaded (`328d7890`); used when transcended and the item's use level
   (`+0xd1`) is >= 70 (`328d6c90`). June's condition has the level test; the plan text did not.

Findings and corrections:
- **Aura `32028790` has no transcendence test** (T0 matched `[esp + 0x3c]`). Its June-form change (users only) goes to checklist 7ao's note.
- **`USI_TRANSCENDENTAL_HYPER_MODE`**: no engine path or class script enters it (June `Enum` only); BFI / BTI 845 / 822 carry the awakening.
- **Cut-ins need no code**: `AVSI_AEM/AVP/ADW_HYPER_MODE` carry `SHOW_HYPER_CUT_IN_TIME`, C3t reads `m_HyperCutIn` (March's `UnitTemplet` has the
  three `HQ_Cutin_*_Transcendence` names), G4's `ActiveSkillShow` plays the `.gfx` (data195).
- `TRANSCENDENTAL_HYPER_MODE_PARTICLE`: not ported (no June data). `EffectSet_UI_Transcendence_*` (`32157c10`, a UI object) -> Stage 9 bullet.
- `CommonFrameMove 32063650` / `32610cb0` are Elsword's passives (T0: class Elsword), untouched.

Routed (scope rule): Raven 24419 in `HyperModeStart`, Rena's type-3 branch in `ApplyHyperModeBuff`, Add / Ain's `+0x9330` countdown, the aura's
June form -> checklist 7ao note; the customize-set names in `ApplyHyperModeBuff` -> Stage 12 (already there); `ICON_TR`'s texture
(`DLG_Common_Emblem00.tet`, June's lacks keys March's dungeon map reads) and `32157c10` -> Stage 9 bullet + Stage 15 P5.

**Data (done after the user's yes, 2026-10-23).** `Enum.lua` gained `UC_150 = 150,` (core-list BOM source, `luasrc.py refresh`, ledger PASS):
June's stat table names it in its second class row, and March's loader stops at an unknown row. data036 `CharacterStatTable.lua` is June's ljconv
bytecode (`adopt-june`; `convert` refused March's file: no T0 source, decompile not T1). All 37 March rows and both formulas equal June's, so no
March character's numbers change. `PACK OK data036` (~2 changed: those two files); `COPY OK data036 ... md5 ffa8e4236d99603357b4f6f2c65f40cf`.

Open questions: none for design.

**Play-test 2026-10-23 (logs, unit 52 ArmeVP).** Table `loaded 5 row(s)`; weapon 163451 / 163453 (level 95) took 0.08 / 0.08; the
transcendence cut-in `HQ_Cutin_AVP_Transcendence`; WakeUp FX playing; rolls 0.586 (none) then 0.063 (843 + 844) 20.5 s apart; Hell Drop summons
x1.3; name mark logged `ICON_TR missing` (expected). **One defect:** the awakening line fired twice per awakening (0.6 s apart), so the FX played
twice. Cause: Aisha's state-end (`June_Vslot604`, June `32ccda90`) tested the CH1i stand-in `JuneIsHyperModeState()` (state id), which is true
on both state-end paths. June reads the `+0x952c` byte instead: `CommonStateStart` sets it from vslot `0x51c` (`@320619a6`) and
`ApplyHyperModeBuff` clears it (`32083190( 0 ) @32298d08`), so June applies once. **Fixed** under the same flag: `m_bJuneHyperModeBuffPending`
(set in `CommonStateStart`, cleared in `ApplyHyperModeBuff`, read by `June_Vslot604`). The base readers `32006c00` / `3201b8e0` (other classes'
hyper state-ends) come with their class ports. `DEPLOY OK: X2_offline.exe is 25,828,352 B, dated 2026-10-23 22:47:06, md5 19d37c292b7c6226c510f847321eaf99`; flagcheck PASS (4 files). **Re-check PASSED 22:52:** one `ENGINE awakening` line, rolls 0.682 / 0.079 20.5 s apart, summon x1.3.

### T5 - Review (2026-10-23, Opus 5.5)

**Verdict: PASS, 1 fix**, and `_DEBUG` is retired. Ghidra read-only; constants read from `unpacked_x2.exe` (`charport_consts.read_va`).
- **Awakening.** `31ffd490` calls `31fc8c20` only in the gauge <= 0 branch, after vslot `0xa8( 0 )` (= March's `else SetHyperModeCount( 0 )`).
  `31fc8c20`: `+0x9328` = 20.0 (`33438a8c`), `+0x932c` = 0, vslot `0x1cc( 0 )` (GetRandomFloat) < 0.5 (`3343777c`), vslot `0x21c( 0x34b / 0x34c,
  0, 1, 0, 1 )` = `SetBuffFactorToGameUnitByBuffFactorID( BFI, 1 )` with March's defaults. `3206eaf0 @3206fefc`: `fldz / fcom [+0x9328] / test ah,5
  / jp` rolls when 0 >= timer, else `timer -= dt` (`[ebp+0x10]`): as ported.
- **Fix (transcription).** `31fce5f0` tests BTI 822 / 823 and Ain's 966-968 with `3227b860` (on and **not finished**, `+0x65`), not HaveThisBuff
  (`3227baf0`). T5 used HaveThisBuff, so a finished BTI 822 still in the list kept the re-roll alive until it was erased. Now `HaveUnfinishedBuff`
  (March's `3227b860`, `SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY`, a top-level define, now in the ENGINE guard). Funcmap row corrected.
- **ApplyHyperModeBuff `32298590`.** NPC (`+0x264`): BFI 1, no pending clear. Transcended: vslot `0x21c( 0x34d, 0, 1, 0, 1 )` `@322988d3`; name =
  customize 3 when not `L""` (`31cb2510` vs `3344f1f4`, Stage 12), else `Effectset_TRANScendence_WakeUp_FX` (`33488078`); `321e1e60` with March's
  defaults; the handle is read back from its out slot (`[esp+0x14] @32298ab4`) into `32296f30( h, 1, 0 )` and never stored. `+0x9f8` is written only
  `@32298c4d` (untranscended customize 4): T5's correction stands. Users: `3206d480` on a socket list, `32083190( 0 ) @32298d08`.
- **RenderName `3201a8f0`.** Survival colours when the PvP room type is 2, or state `0xb` (= `XS_DUNGEON_GAME`, `X2Main.h:764` counted) in dungeon
  `0xa1ea` (41450). Rank flag = `g_pMain +0x21b`, cleared by `+0x21f` and vslot `0x198` (`m_bPvpRank` / `m_bNothing` / `IsMyUnit`); left = `ftol( x )`
  - width / 2; a drawn rank -> `fild / fsub 24.0 (33455c6c) / ftol @3201abd5`; `+0x3c != 0` -> `31ff6920( left, top )`. `31ff6920`: game type != 3
  returns; `ICON_TR`; `Draw( x/sx - 22/sx, y/sy - 2/sy, 25/sx, 20.0, lt.x, lt.y, rb.x - lt.x, rb.y - lt.y )` (22 / 2 / 25 are doubles `33474d68` /
  `33437820` / `33474890`). After the rank June draws only the `+0x96c` guild line (y - 22, `ff96ff00` / `ff062507`): **no couple line**, confirmed.
- **Not ported, routed:** June's `RenderRank` body (`31ff6aa0`) draws a season emblem when `3241d290` == 2 and skips `PVPRANK_NONE` /
  `RANK_ARRANGE`; March's draws any rank. Battle-field PvP emblem -> appended to the Stage 9 T5 bullet.
- **Numbers.** `322ece70`: 1.2 / 1.3 (`3344ce94` / `33472a10`, read). `32630d20 @32631602` -> `31fceab0`: templet `+0x174 == 5` (`32205590`,
  `SPRT_HYPER_CLASS` = 5) and `+0x3c != 1` -> 0. `328d6c90 @328d6d34`: transcendence != 0, use level >= `0x46`, `[ebx+0x30]` find against `[ebx+0x24]`
  (JUNE_BUGS 416 right); caller `328d6d70` passes `+0x58`, templet `+0xd5` / `+0xd1`. Loader `328d7890`: same five keys and slots as the normal table.
- **Data.** `adopt-june --dry-run CharacterStatTable.lua`: `same` (= ljconv June, manifest `2`); `Enum.lua` is BOM source with `UC_150 = 150`;
  `ledger --check` PASS. `core --check`'s one FAIL is data036 `ResultData_new.lua` (quest plan Q4c), not T5's.
- **Play-test.** The 22:52 run (after the 22:47 deploy) is the re-check: one awakening line, 5 rows loaded, the weapon row 0.08 / 0.08, x1.3.
- **`_DEBUG` retired:** the define and its four blocks (name mark, roll, summon, stat row). The awakening and `loaded N row(s)` lines stay.
- Left as is: `June_Vslot604`'s header comment still names `JuneIsHyperModeState()` (Aisha's flag, outside this one).

Hygiene: T5 range `flagcheck --range 52eead16^..727bd27f --with _DEBUG`: `files: 12 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
`in this flag's files: 2 deleted line(s)` (727bd27's rewrite of T5's own guard), `6 EOL-mismatched` (T5's `X2GUUser.h` note); `RESULT: PASS`.
Review edit, `--worktree` (4 files): `files: 3 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (Always.h: the _DEBUG define), 0 FAIL, 0 ERROR`;
`in this flag's files: 26 deleted line(s)` (T5's own `_DEBUG` blocks and the two `HaveThisBuff` lines), `0 EOL-mismatched`; `RESULT: PASS`.
`DEPLOY OK: X2_offline.exe is 25,827,328 B, dated 2026-10-23 23:12:21, md5 cbdab12a4de69940e94a06b0129d1c72` (built at `78bfac32` + these 4 files).
The user owes one optional look: unit 52 awakens once (the `ENGINE awakening` line); nothing else changed in play.

## Phase T6 - Aisha transcended

**Run with:** Sonnet 5.5 high, then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_TRANSCEND`.

**Scope.**
- Aisha's readers run on the real field (T1). Check each one on a transcended EM / VP / DW:
  - Dark Deal / Open Mind (CH4a / CH4b), `June_HyperModeBuffEffectStart`'s early return, `JuneGetHyperModeTableName`.
- Port what T0 finds still missing:
  - passives 22412 / 22413 / 22414 / 22417 if `32cecd70` does not read them;
  - a hyper-class active with no state;
  - the transcendence memo 22217 (VP);
  - the TC-slot registration in `June_SetEquippedSkillFuncHead`;
  - her guide-dialog row and cut-ins.
- `MODS.md` damage table for any number that changes.

**Done gate:** on units 51 / 52 / 53 with `transcendence 1`:
- every hyper-class skill learns, equips and casts;
- the passives log non-zero values;
- awakening uses `AVSI_AEM/AVP/ADW_HYPER_MODE` with the transcended cut-in;
- 0 Aisha `no_state` rows.
Log `[JUNE_TRANSCEND] ARME`.

### T6 - Done (2026-10-24, Sonnet 5.5)

Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_TRANSCEND` (log only), commit: this one. Funcmap rows `Transcend T6` (6). Checklist 7ao Aisha row closed. MODS entry "T6".
Hygiene: `flagcheck --worktree --files`: 1 PASS, 1 DEFINE, 0 FAIL; 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 encoding change; RESULT PASS.
`DEPLOY OK: X2_offline.exe is 25,828,352 B, dated 2026-10-24 05:08:45, md5 95a5d71a9febcd394728476555a7a7c4` (the `ARME passives` string is in the exe).

**Finding: no Aisha code was left to port.** Each kickoff item was already June's:
- Readers `31fcac90` / `31fcad70` / `31fceab0` (`== 1`) / `31ff1e80` / `32cddcc0` read `m_iUnitClassTran` since T1; `31fdd490` names `AVSI_AEM/AVP/ADW_HYPER_MODE` for 104 / 105 / 113 (all three tables are in data036 `Arme_VioletMage.lua`).
- Passives: 22412 (CH1i), 22413 (CH1i, CH2a), 22414 (CH4c), 22415 (CH4a), 22416 (CH4b), 22417 (CH1i, CH4c) all have readers. The census `no_state` rows are exactly these eight passives (22410-22417): data, not states.
- Actives: all twelve `SPRT_HYPER_CLASS` actives have a `June_SetEquippedSkillFuncHead` case with a `registered` log line (CH3a / CH3a2 / CH3b).
- Memo 22217: `GetJuneTranscendenceMemoSkillID` (`31fdbcc0`, S3b) and the Summon Bat Heavy branch (CH1k). TC slots: T4. Cut-ins and awakening: T5 (`HQ_Cutin_AVP_Transcendence` seen in its play-test); the three `HQ_Cutin_AEM/AVP/ADW_Transcendence` `.gfx` are in data195.
- Guide dialog: T2 (popup played). The only edit is a proof log, `[JUNE_TRANSCEND] ARME passives ( unit N: transcended, hyper table, open mind EM / magical body / open mind VP / dark deal / open mind DW / time limit with levels and values )`, written after the CH4a / CH4b passive-init lines.
- The earlier T5 log already showed unit 52 `dark deal 0.500 / 0.500` (non-zero when transcended; 0 before, CH4a).

No damage number changes, so no MODS damage row. No June branch found outside the subject; nothing routed.

**Save set up for the play-test** (backups `db_backup/els_db_20261024_045823_pre_sql.sql` and the one after): unit 53 `transcendence 1`; 22413 on unit 51 and 22417 on unit 53 (level 1);
unit 52 learns 22209 (slot 2, memo 19 = `MI_TRANSCENDENCE_1`); hyper actives equipped: unit 51 22041 (slot 1), 22224 (2), 22042 (6), 22068 (7); unit 52 22043 (1), 22225 (3), 22069 (6), 22044 (7), 22058 (4);
unit 53 22045 (0), 22046 (6), 22070 (7), 22226 (8).

**In game.** Open `offline_server.log`: each transcended unit logs one `ARME passives` line per spawn. Unit 52 casts Summon Bat (22209, slot 2): with the memo it becomes Summon Bat Heavy (22217).
Open questions: none.

**Play-test PASSED 2026-10-24 (user + logs).** All three units cast their hyper-class skills, awakened with the transcended table and cut-in, and the passives logged non-zero
(unit 52 `open mind VP 1 scope 600 mp 0.8 cool 0.8, dark deal 1`; unit 53 `open mind DW 1`). One refusal, a **save-setup error of mine, not a code bug:** unit 52 had the enhanced
Petit Angkor 22231 learned and I had equipped the base 22209, so `GetEquippedActiveSkillTemplet( 22209, true )` swapped it to 22231 and June's slot check (`32cebe80 @32cebf9c`,
`templet id != slot id`, string 0x8bae) refused the cast with "This skill cannot be used. Please equip another skill". Fixed in the save (22231 equipped, memo 19 on base 22209); the
cast then worked and the heavy summon played. Two temporary log lines stay for the review to retire: `ARME info text` (in `June_PushInfoText`) and `ARME cast lookup` (in `June_SpecialAttackLookup`).
Lesson for set-ups: equip the skill the unit has learned (the enhanced one when it is learned). Deploy for the diagnostic build: `DEPLOY OK: X2_offline.exe is 25,828,864 B, dated 2026-10-24 05:28:41, md5 d3e9ef527f6885c8b54ef80628440ad3`.

### T6 - Review (2026-10-24, Opus 5.5): PASS, 1 fix

T6's finding holds: no Aisha transcendence code was left unported. One transcription fix, under `SERV_IRUHADEV_JUNE_CHAR_ARME_TRANSCEND`.

- **Fix: `June_GetSkillLevel` tests `== 1`.** June's `31fceab0 @31fceaef` is `cmp [unit + 0x3c], 1`, but March's copy used `June_UnitFlag3c` (`!= 0`). The new line is
  `1 != m_iUnitClassTran`; the old test stays in `#else`. It feeds the Dark Deal / Open Mind DW init (`32cecd70` calls it for 22415 / 22416) and the 22416 attack result.
  Equal for the 0 / 1 save, so nothing changes in play. `X2DamageManager.cpp:5130` already had `1 !=` (T5).
- **Readers.** These match March's copies, read from the raw disassembly where Ghidra has no function:
  - `31fcac90` (`June_SetSkillBuffFactors`) and `31fcad70` / `31ff1e80` test `!= 0`.
  - `32cddcc0` returns at `32cddeb6` for a transcended unit, then on `31fd66b0` (the effect-set test).
  - `31fdd490` names `AVSI_AEM/AVP/ADW_HYPER_MODE` for 0x68 / 0x69 / 0x71.
  - Not fixed, no effect: `31fcac90` asks the level with `bBasic` 1 where March passes `false`. That argument only matters for guild skills (`32890620`).
- **No missed reader.** T0's `+0x867c -> +0x3c` sweep lists no other Aisha function. A new sweep of June's `.text` for 22412-22417, 22041-22046, 22068-22070,
  22224-22226 and 22217 as immediates finds only ported Aisha functions:
  - `32cc26f0` / `32cc2740` / `32cc2790`, `32ccf4e0`, `32ccf9f0`, `32ce1cc0`, `32ce9f90`, `32cea0c0`, `32cecd70`, `32cede00`, `32cdfb70`, `32cc99d0`.
  - Noise: string id 22412 in three login-dialog functions (`3249e3c0` / `324a5370` / `325477e0`, `GET_STRING` 31c30d30), xorstring bytes in `3237aa40`, stack offsets near
    `31e42fd1`, and the Lua enum table `31d95080`.
- **`June_SetEquippedSkillFuncHead`.** In `32cede00`, the `31fdbcc0` memo id registers `AVSI_A_AVP_SUMMON_BAT_HEAVY` only when its base id is 0x56c9 (22217), else jumps
  past the switch: no state for another memo-turned skill. March matches. `31fdbcc0` (types 19 / 20, value `[upgraded ? 1 : 0]`) matches `GetJuneTranscendenceMemoSkillID`.
  All twelve hyper-class actives have one `case` each.
- **Play-test values.** The 07:26 log has unit 52 with `open mind VP 1 scope 600.000 mp 0.800 cool 0.800, dark deal 1`. That is June's 22414 row
  (`SA_SCOPE_ABS` 600, `SA_MP_CONSUME_REL` 0.8, `SA_COOLTIME_1` 0.8). T5 logged 22415 as 0.5 / 0.5 (`SA_EFFECTIVE_RATE` / `SA_MP_RETAKE_ABS`). The `registered`, memo and
  cut-in lines were in T6's own run, since overwritten; the code paths behind them are confirmed above.
- **Temporary lines.** Neither `ARME cast lookup` nor `ARME info text` is in the tree.
- Hygiene: `flagcheck --range 819c96a6^..819c96a6`: `files: 1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII
  inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  The fix: `flagcheck --worktree --files X2Lib/X2GUArme_VioletMage.cpp`: 1 PASS (+6), 0 deleted, RESULT PASS.
- `DEPLOY OK: X2_offline.exe is 25,837,568 B, dated 2026-10-24 07:48:35, md5 6151839baa1624e2b7621a023f60e447`.
- No June branch outside the subject; nothing routed. Open questions: none.

## Phase T6b - The transcendence slot buffs

**Run with:** Opus 5.5 high (RE of the UI guide-particle table first). Kickoff `kickoffs/june-transcend/T6b.md`. Flag `SERV_IRUHADEV_JUNE_TRANSCEND_SLOTBUFF`. Added 2026-10-24 (user).

**Scope.** After a transcended awakening rolls BFI 843 / 844 (T5): the A-5 / B-5 slots glow while BTI 820 holds, the TC-slot skill does x1.2 damage and its cool-time is reset (BTI 821, `PERCENT` 0),
and the buff ends after 10 s or one TC-slot cast. Prove the buff behaviours act on slot type 3 (log), and port June's slot 4 / 9 guide-type-4 particle (`322d8100`, UI manager `321536e0` / `32151190` / `32151fa0`).

**Done gate.** The glow shows and ends as above; the log shows the 1.2 factor and the cool-time reset on the TC cast.

### T6b - Done (2026-10-24, Opus 5.5)

Flag `SERV_IRUHADEV_JUNE_TRANSCEND_SLOTBUFF` (+ `_DEBUG`: the proof lines, for the review to retire), commit: this one. Funcmap rows `Transcend T6b` (8). MODS entry "T6b".
Hygiene: `flagcheck --worktree --files` (7 files): 6 PASS, 1 DEFINE, 0 OTHER, 0 FAIL; 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 encoding change; RESULT PASS.
`DEPLOY OK: X2_offline.exe is 25,839,104 B, dated 2026-10-24 06:24:19, md5 b565a19a691c409674e2123e25cce39d` (the SLOTBUFF strings and `Transcendence_Loop_P1` are in the exe).

**No split needed.** The UI manager's guide table is small. `32154f60` registers types 1 / 2 (`Luciel_SwitchingP01-04` / `-05`, users `322dd0f0` / `32212d50`), 4 (`Transcendence_Loop_P1-P5`, then
`Transcendence_Open_P1-P5`, period 9999 s) and 5 / 6 / 7 (`PT_AIN_Eid_Switching_Red / Yellow / Blue_01-04`, user `322d8590`). All except type 4 use a 2 s period, and all except type 4 are left to their class ports (Lu/Ciel, Ain; Stage 7).

What landed:
1. **The glow** (`X2Game.cpp` `June_TranscendSlotGuideFrameMove` = `322d8100`, called `@3230906f`). For slots 4 / 9 it applies June's bar-visibility test and the TC-key early return.
   The slot skill must not be relationship / hyper-active, the hyper time must be above 0, `HaveUnfinishedBuff( 820 )` must be true and `JuneCanShowClassGuide( 4 )` must pass.
   Position: the centre of the `Static_Skill_Cool_Time_Fade(_B)` picture, +-47 when swapped. When the condition fails the slot's entry is removed.
2. **The table** (`X2UIManager.*`), type 4 only: add `321536e0`, find `32151190`, remove `32151fa0`, the timer at the end of `OnFrameMove` (`32155eb0 @3215631c`), respawn `32155e20`, spawn `32153880`
   (shown `DLG_GAME_SKILL_SLOT`, `CreateSequenceHandle` with defaults, `SetOverUI`). The particles are in data036's `UIMajorParticle.txt` (June's; `kom.py status data036` in sync), so nothing was staged.
3. **Buffs, by reading:** BFI 843 -> BBT 98 (`GetSkillDamageRateBySlotType`, slot type 3 for index 4 of either bar) and BFT 16. BFI 844 -> `ResetLeftSkillCoolTimeByBuff` /
   `ApplyResetCoolTime`, `BCT_PERCENT` = cool time x 0 on slot type 3. Both transcriptions are June's. No fix; the `_DEBUG` lines prove them in play.

Not ported: `32153880`'s skip for game type 5 with hidden dialogs (March has no game type 5); `3288fe00`'s unused result in `322d8100`. Routed: nothing new (types 1 / 2 / 5-7 named above).
Open questions: none.

**In game** (`python tools/logs.py --grep "JUNE_TRANSCEND] SLOTBUFF"`). Awaken a transcended unit. On a roll: `applied BFI 843 / 844`, then `BTI 820 on`.
On the next UI frame, one `glow add` and one `glow spawn ... 10 of 10` per TC slot. Casting the A-5 skill gives `damage: skill N in the TC slot, slot-type rate 1.20`, and the slot then logs `BTI 820 off ... ended by use` and
`glow remove`. With no cast it ends `by time` after about 10 s. A cool-time line (`X -> 0.00`) appears when BTI 821 lands while the TC skill is cooling. The dev free-skill gate (auth 3) skips the cool-time
check on cast, so the cool-time cannot be seen in the slot UI itself, only in that log line.

**Play-test PASSED 2026-10-24 (user + logs, unit 52 ArmeVP, exe md5 b565a19a).**
- **Roll 0.093 (06:34:42):** BTI 820 / 821 on at once, then glow add + spawn `10 of 10` on slots 4 (582, 730) and 9 (582, 683). Phantom Breathing 22058 cast at `slot-type rate 1.20`; 820 off after 3.3 s `ended by use`, both glows removed.
- **Roll 0.469 (06:35:22):** 22058's cool-time `2.84 -> 0.00`; 820 off after 10.0 s `ended by time`, glows removed.
- **Outside the buff:** TC casts at 1.00. 0 ErrorLog lines, 0 exceptions. The two `E_DISABLE_HB_CHECK_REQ` UNHANDLED are not this phase's.

### T6b - Review (2026-10-24, Opus 5.5)

**Verdict: PASS, no fix**, and `_DEBUG` is retired. Ghidra was read-only. Constants were read from `unpacked_x2.exe`'s bytes.

Checked against June:
- **`322d8100`.** The loop is `i` 0-9 with `i + 1 == 5 / 10`, and B is set at `i = 9` and never cleared, as in the port. The early return sits at `322d81d3-322d8207`: the select unit
  `+0x494` skill tree's `32890ee0( 1 )` is false and ( B, or unit `+0x3c == 0` ). Type A is `+0x116` and swap is `+0x115`, with the same bar test. The templet type is not `0xd` / `0xc`; March's enum
  gives `ST_RELATIONSHIP_SKILL` = 12 and `ST_HYPER_ACTIVE_SKILL` = 13 (with `FINALITY_SKILL_SYSTEM`). `GetRemainHyperModeTime` is compared with `FCOMP float [3342e84c]` = 0.0, then
  `HaveUnfinishedBuff( 0x334 )` and `JuneCanShowClassGuide( 4 )` (`31d71810`). Position: picture pos + dialog (`CX2Game +0x714`) pos + size x 0.5 (`3342ebc8` = 0.5); with swap, B +47 / A -47
  (`3348a580` = 47.0). The move test is `> 0.1` (`334395e8`). Add / move-and-store / remove are the same.
- **UI manager.** `32154f60`'s type 4 period is `3346ede8` = 9999.0 (the others `33437778` = 2.0). `321536e0` removes an existing entry, inserts it, and sets the timer to 0 (inserting it if missing).
  `32151fa0` erases every slot match in the type's range, then the timer when the type has no entry left. The timer at `32155eb0 @32156388` is `-= dt`, then `FLDZ; FCOMP` (respawn when `<= 0`), then
  period `+4`. `32153880` checks the game-type-5 skip, skill tree `+0x3c` / dialog `+0x10c` valid and shown (`+0x6e`), destroys the handles and clears the vector. It creates each name with
  `( NULL, name, x, y, 0, -1, -1, -1, -1, -1.0, true )` (`334380f0` = -1.0, which equals March's defaults), sets `+0x170 = 1` and pushes the handle even when invalid.

Port extras, kept: null checks on the fade control and its picture (June dereferences them). Neither changes behaviour when the dialog is loaded.

**Retired:**
- `SERV_IRUHADEV_JUNE_TRANSCEND_SLOTBUFF_DEBUG`: the define and its 7 blocks in `X2Game.cpp`, `X2GUUser.cpp` (2), `X2UIManager.cpp` (4, with the log include) and `X2UserSkillTree.cpp`. 70 lines, no `[JUNE_TRANSCEND] SLOTBUFF` line is left.
- T6's `ARME cast lookup` line (`June_SpecialAttackLookup`, 3 lines; T6's review had not run). `ARME info text` was never committed. `kickoffs/june-transcend/T6-review.md` now says so.

**Hygiene:**
- T6b range `c66d52fa^..c66d52fa --with _DEBUG`: `files: 6 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  `git log -SJUNE_TRANSCEND_SLOTBUFF c66d52fa..HEAD`: nothing.
- Retirement `--worktree`: `4 PASS, 1 DEFINE, 0 FAIL`; the 70 deleted lines are the `_DEBUG` blocks; `RESULT: PASS`. `JUNE_CHAR_ARME_TRANSCEND --worktree` (VioletMage): `1 PASS`, 3 deleted (the lookup line), `RESULT: PASS`.

`DEPLOY OK: X2_offline.exe is 25,836,544 B, dated 2026-10-24 06:52:05, md5 d6c695c7d93ca62d5d7244880d7eeb3c` (built at `08a45a91` plus these files). `SLOTBUFF` and `ARME cast lookup` are absent from the exe; `Transcendence_Loop_P1` is present.

T8's kickoff now depends on this review and checks the glow by eye.

## Phase T6c - Transcended class pictures

**Run with:** Sonnet 5.5 high. Kickoff `kickoffs/june-transcend/T6c.md`. Flag `SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE`. Added 2026-10-24 (user), taken from the Stage 9 bullet "Transcendence in the June UI".

**Scope.** A transcended unit's class picture: the character select slot (June `325659c9`, `3256ed1b`, type 0xa at `3256ebe7`), the HUD portrait, character info, inventory, class name / icon.
Add the unit-slot (and 0xa) rows for Aisha EM / VP / DW, stage June's pictures, pass `unit +0x3c` at the March sites. Evidence: `june2017_re/transcend/t6c_image_callers.tsv`.

**Done gate.** ArmeEM / ArmeVP / ArmeDW show a transcended picture on the select screen, HUD portrait and character info; the log shows the lookups.

### T6c - Done (2026-10-24, Sonnet 5.5)

Flag `SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE` (needs `JUNE_TRANSCEND_CHANGE`, `OFFLINE`), commit: this one. Funcmap rows `Transcend T6c` (12). MODS entry "T6c".
Hygiene: `flagcheck --worktree --files` (8 files): 7 PASS, 1 DEFINE, 0 OTHER, 0 FAIL; 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 encoding change; RESULT PASS.
`DEPLOY OK: X2_offline.exe is 25,837,568 B, dated 2026-10-24 07:19:18, md5 d6de4714369599c1227aa719421f69d8, equal to X2/US_SERVICE/x2.exe (2026-10-24 07:19:11); built at main 63b97762 + 8 uncommitted code file(s).`
Koms: `PACK OK data057` and `PACK OK datai021` (2 entries changed each). **Not copied: `/x2-kom-packed copy data057 datai021` waits for the user's yes.**

What landed:
1. **June's `0xa` is the gray unit slot** (`3256d6e0 @3256ebe7`): the slot refresh tests unit `+0x9d0 == 1` (March `m_bDeleted`, waiting delete) and takes type 0xa, else type 9. Both paths use point ( 23, 11 ), size 38 x 38 and colour 1.0
   (March tinted the waiting-delete slot 0.5; the select-screen site now follows June). `CIT_UnitSlotGray` = `list_<CLASS>_GRAY` in `..._character_01_gray.tga`, derived from March's row (every March unit-slot row is `character_01.tga`).
2. **Aisha rows:** unit slot `list_TR_E_MASTER` / `V_PRINCESS` / `D_WITCH` and `_GRAY` (June's TET, data215, carries all 37 `LIST_TR_*` pieces and their `_GRAY` twins).
3. **Sites that pass `1 == m_iUnitClassTran`:** select screen create (`32564d50 @325659c9`) and refresh (`3256d6e0`), character info (`3291df90 @3291e505`), dungeon result party rank, both live and dead pictures
   (`32513d30 @32515156`; the scan saw one call, the code pushes the flag on both), user list popup (`328b89c0 @328b8e6f`), other-player gauge (`324924a0 @324939f9`; the live `SERV_PVP_NEW_SYSTEM` copy only).
   The HUD portrait was already June's from T2 (`JuneIsMyGageTranscended`).
4. **Data (replace rule, byte-exact):** June's `DLG_UI_Common_Texture_GUI_character_01.TET/.tga` into data057 and `..._gray.TET/.tga` into datai021 (the tga differ from March's: June has the transcended art).
   Backup `kom_backup_stage_june/20261024_070830`.
5. **Log:** `[JUNE_TRANSCEND] IMAGE class N type N -> file / piece`, once per class and type.

**Not ported, with owners** (rows in `funcmap.tsv`; Stage 9 bullet updated): `321a9da0 @321aa423` is the June-only Jumping-system mail dialog (the Stage 9 bullet called it inventory; March has no counterpart);
`324ce260 @324ce67b` dungeon clear (June's rows have no transcended pair, the flag changes nothing); `32601f80` / `32888660` achievements (Stage 31); `325da870` / `325dacb0` / `325db6f0` `DLG_UI_Character_Icon` fills
(friend / field / guild lists: March's structs carry the class only, and offline has no other player); class picker `32ed5180` (no March caller of that shape); `X2StateArcadeResult.cpp:957` / `:968` (same shape as the dungeon result, no June function matched).
**Honest gap for the review:** the table strings of the Aisha unit-slot rows were not read off the static initializer `31ef0ef0` (a scripted read failed because the compiler reuses string temporaries between rows).
They are `list_TR_` + the T2 prefix, and the pieces exist in June's TET; the first play-test log line names the piece actually used.
Open questions: none.

**In game** (`python tools/logs.py --grep "JUNE_TRANSCEND] IMAGE"`): after the two koms are copied, the select screen shows a transcended Aisha slot with a different picture from an untranscended one.

### T6c - Review (2026-10-24, Opus 5.5): PASS, no fix

Reviewed `6ec4a7f4` against June in Ghidra (read-only) and the exe's bytes; no code changed, so no build.
1. **`31ee0de0`**: type switch as ported. Type 9 base `337962e0`, 0xa base `33798ce0`, stride 0x70, pair at +0x38 / +0x54, taken only on `param_5 == 1` and a non-zero +0x48 length.
   Aisha's June row indices: EM 0x13, VP 0x14, DW 0x25. March's `CHARACTER_IMAGE_TYPE` numbering matches June's types 1-9, and `CIT_UnitSlotGray` = 10 = 0xa.
2. **The gap the Done note left is closed.** The row ctor `31ef0ef0( file, piece, trFile, trPiece )` writes +0 / +0x1c / +0x38 / +0x54. The rows are built in `333988f0` (type 9) and `3339ead0` (type 0xa), and their pushed strings read:
   - type 9: `list_AISHA_E_MASTER` / `V_PRINCESS` / `D_WITCH` with `list_TR_E_MASTER` / `V_PRINCESS` / `D_WITCH`, all in `character_01.tga`;
   - type 0xa: the same names + `_GRAY`, in `character_01_gray.tga`.
   This matches the port and the play-test log lines (07:26 run, classes 104 / 105 / 113, type 9).
   DW's two rows write the extension as `.TGA`. The lookup is upper-cased (`KGCMassFileManager.cpp:292`), and March's own DW base row already uses `.TGA`, so it is not a fix.
3. **The gray derivation (March row + `_GRAY` in `_gray.tga`) is exact.** Paired over all 86 June rows, the gray base piece is the type 9 piece + `_GRAY` everywhere except Rose (68-76) and Ain (82); March has neither class.
   All 50 March unit-slot pieces have a `_GRAY` twin among June's strings.
   June's gray TR pieces for other classes are not type 9 + `_GRAY` (e.g. `list_TR_ELSWORD_SW_KNIGHT_GRAY`). The port returns before reaching them (prefix switch, Aisha only), so they matter only to the later class ports, which read their rows from `3339ead0`.
4. **`3256d6e0`**: `cmp byte [unit+0x9d0], 1` then type 0xa (`@3256ebe7`), else type 9 (`@3256ed1b`). Both push `[unit+0x3c]`, both use point ( 23.0, 11.0 ) and size 38.0 (read from the exe, `3349d46c` / `3345abf4` / `33498d18`), and both colour with `FLD1` x4.
5. **Sites**: each pushes `[+0x3c]` as the flag, and each image type matches its March call:

   | June site | Type | Matches March |
   |---|---|---|
   | `32564d50 @325659c9` | 9 | yes |
   | `3291df90 @3291e505` | 5 | yes |
   | `32513d30 @32515156` | 3 and 2, one call with both branches pushing | yes |
   | `328b89c0 @328b8e6f` | 5 | yes |
   | `324924a0 @324939f9` | 4 | yes |

   Ghidra has no function covering `@324939f9`, so it was read from raw bytes: `8b 4f 3c 51 6a 04 ... e8` -> `31ee0de0`.
6. **Data**: `kom.py stage-june --dry-run` reports `identical` for all four (`character_01.TET` / `.tga` in data057, `_gray.TET` / `.tga` in datai021). `kom.py status data057 datai021`: in sync with the live koms, so they were copied.
   ErrorLog of the 07:26 run has no miss on any `character_01` / `list_TR_` name.

Hygiene (`--range 6ec4a7f4^..6ec4a7f4`): `files: 7 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
`in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
`git log -SJUNE_TRANSCEND_IMAGE 6ec4a7f4..HEAD`: empty. Deployed exe unchanged (md5 d6de4714).
Noted, not changed: T2's comment above the prefix switch (`X2Data.cpp:2995-2997`) still says the unit slot is Stage 9's (the comment rule does not ask for old comments to be rewritten).
Open questions: none. The user owes an optional look at the select screen (ArmeEM / ArmeVP / ArmeDW slots); the log already shows the lookups.

## Phase T7 - Transcendence sockets (opened by T0 only)

**Closed by T0 (2026-10-23).** `SERV_IRUHADEV_JUNE_SOCKET_PARITY` already declares and parses all eight `SDT_*_TRANSCEND*` types
and their stats; June's data uses only the four WEAPON types; no June socket code reads the transcendence flag. See `kickoffs/june-transcend/T7.md`.

## Phase T8 - Walk

**Run with:** Sonnet 5.5 medium, user play-test. No code.

**Scope.** Snapshot the save first.
- Unit 52 takes Limit Break 1/5-5/5 from the Transcend house as June has it (the kickoff lists dev commands that shorten the clears).
- Units 51 and 53 transcend by `/tc`.
- Check the guide, the tree, A-5 / B-5, awakening, passives, items, relog.
- Failures go back to the owning phase's flag.

**Done gate:** the user's walk passes; no new `KEM_ERROR`; 0 `UNHANDLED` / `EXCEPTION` in the walk's logs.

### T8 - Set-up (2026-10-24, Sonnet 5.5)

No code, no build (exe md5 6151839b as deployed by the T6 review). Save snapshots: `db_backup/els_db_20261024_075559_pre_T8_walk_start.sql`, then the two
`elsdb` backups of the edits below. Edits: `transcendence 0` on `ArmeVP` (52), `ArmeEM` (51) and `ArmeDW` (53); `ArmeVP2` (54) stays at 1 for the ticket check; the five
`unit_quest_complete` rows 41990-42030 of `ArmeVP` deleted so the chain can be taken again (quest data untouched).
Shortcuts for the clears (the emulator does not check sub-quests or collected items, `Handlers_Quest.cpp` `/qdone`): `/qdone 41990` ... `/qdone 42030` in order.
The walk's result goes here after "check logs".

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium.

**Scope.**
- `MODS.md` section and revert lines.
- Checklist §7ao marks.
- The Stage 7 template's transcendence rows (each class port enables its class in T2's allow-list and ports its §7ao row).
- A backlog Stage 6 closing note; `_DEBUG` flags off; memory.

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill and slot); per check: where / paste / what it does / look for /
if not. Log tag `[JUNE_TRANSCEND]` with the phase's sub-tag (`python tools/logs.py --grep "JUNE_TRANSCEND <sub>"`).

## Open decisions (for the user; asked at the end of planning; both answered 2026-10-22)

1. **Who may transcend before their own port.** June lets all 37 second-job classes take Limit Break. A March-kit class would gain hyper-class
   tree rows with no states and March's awakening.
   **Answered (user, 2026-10-22): an allow-list.** The emulator lets only ported classes transcend (Aisha's EM / VP / DW now) and refuses others
   with a log line. Each Stage 7 / 8 port adds its class. This is a stand-in, not June: label it as one (T2).
2. **The B-5 / T slot (slot-state key 1).** June opens it only by the Skill Slot Change Medal (Transcendence) or the quest's 15-day cube.
   The emulator keeps March's `SKILL_SLOT_B_FREE` rule for B-1..4.
   **Answered (user, 2026-10-22): free for a transcended unit.** The emulator sends key 1 as permanent once a unit is transcended, extending
   the B-free rule. The medal and cube stay usable as June has them. This is a stand-in, not June: label it in `MODS.md` (T4).
