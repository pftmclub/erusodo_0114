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
- **NetError text.** `X2NetError.cpp` `GetErrStr` returns June's English text for 1687 / 1688 / 1726 (past `ERR_SENTINEL`).
- **Data.** `adopt-june` data034: the 12 `DLG_Transcendental_Change_Guide_*.lua` + `DLG_Class_Change_TranscendentalClass.lua` (13 new, `june2017_re/june_adopt_20261023_080637.tsv`); data036 `Enum.lua` (+3 `LUEMCUM_*`, `LUEI_..._TRANSCENDENTAL` 6, `refresh`) and `LevelUpEvent.lua` (`convert` T0, event 6 with Aisha's row only); `stage-june` 16 files (data215 14: `POPUP_TR_E_MASTER / V_PRINCESS / D_WITCH`, `TR_POPUP_IMAGE`, `DLG_UI_Common_Texture97/98/100_NEW` (+gray, .TET), `DLG_UI_Image_09/10`; data052 `TR_POPUP_TEXT`; data054 `DLG_UI_Common_Texture_jump_event_12`). `ledger --check` PASS. Packed: `PACK OK` data034 / 036 / 052 / 054 / 215; **copy pending** (ask).
- **T0 rows not done, with owner:** `32712e50` is the skill-command panel (S7e), not the level-up manager (funcmap); `328dcba0` is Luciel (unit type 10; checklist); the cash shop's item preselect `+0x334` (June's single member) is replaced by T4's open-at-item list.
- **Hygiene** (`flagcheck --worktree --files` on 27 files): `files: 19 PASS, 5 PASS~, 2 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. (A first spec with repeated `@@ file` sections silently dropped six edits; found by the per-file counts and re-applied.)
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,808,384 B, dated 2026-10-23 08:32:47, md5 551b034e1f3fb3b4428257304904fcdf, equal to X2/US_SERVICE/x2.exe (2026-10-23 08:32:41); built at main 756d936f + 27 uncommitted code file(s).` Strings checked in the exe: the Aisha guide file name, `TR_`, `You cannot Transcend.`.
- **Open for the user:** `data054` has 66 entries that differ from the live archive besides mine (earlier staging never copied); copying it deploys them too. Say which koms to copy.

**How to see it:** auth level 3, `/tc 1` on unit 52 (`ArmeVP`, level 70+): log `CHANGE /tc 1 ... transcendence 0 -> 1` and `CHANGE NOT sent`, the guide popup opens (Aisha's, VP picture, Close button), the skill tree shows rows 14-17, a relog keeps it. `/tc 1` again: NetError 1688 text; a first-job or under-70 unit: 1687; another class: 1726 and a log line. `/tc 0` clears it.

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
- **Hygiene** (`flagcheck JUNE_TRANSCEND_SLOT --worktree --files` on the 7 code files): `files: 3 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 25,794,560 B, dated 2026-10-23 07:26:59, md5 57f3849e8cd8f05cd124be340cf6a467, equal to X2/US_SERVICE/x2.exe (2026-10-23 07:26:52); built at main a6ff8002 + 15 uncommitted code file(s).` The exe also carries a peer's uncommitted T2 work (`SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE`).

**Play-test hand-off** (log tag `[JUNE_TRANSCEND] SLOT`; unit 52 is `ARME_VOID_PRINCESS`, level 99):
1. *Untranscended, locked.* Game closed: `python tools/elsdb.py settranscend 52 0`. Start the game, pick unit 52, open the skill window (K). Look for: A-5 and B-5 locked. Click the locked A-5: the cash shop opens on "Book of Transcendence" (village), or in a dungeon the yellow chat text. Log: `click on a locked A TC slot: shop opens on item 288319`. Click B-5 (expand B first): item 288318.
2. *Transcended, A-5 / F.* Close the game: `python tools/elsdb.py settranscend 52 1`. Start, pick unit 52. Drag any active skill into A-5 (the fifth A slot). Look for: it stays; log `equip TC slot 4 A skill <id>`. Enter a village or dungeon and press **F**: the skill casts with its own animation. If not: `grep "SLOT" offline_server.log`, and the `JUNE_SKILL` blank-state line.
3. *Transcended, B-5 / T.* Expand B, drag a skill into B-5, press **T**: it casts. B-5 unlocked without the medal (key 1 permanent, stand-in). Log: no `refused` line.
4. *Reverting.* Close, `settranscend 52 0`, start: a skill left in A-5 / B-5 is unequipped by the UI (June `328e18d0`) or shows locked.
Then say "check logs".

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
