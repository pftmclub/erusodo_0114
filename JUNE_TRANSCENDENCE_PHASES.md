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
| Z | Close-out: MODS, checklist transcendence section, Stage 7 template rows, backlog note, `_DEBUG` off | Sonnet medium | T8 |

T0 runs now (read-only, beside anything). T2 / T3 share the emulator's class-change code: one after the other. T4 may run beside T2 or T3
when the kickoffs' files do not overlap (`X2OfflineServer.cpp`'s dispatch table and `Always.h` one at a time). T5 and T6 edit `X2GUUser.*`
and `X2GUArme_VioletMage.*`: one code phase at a time there.

## Phase T0 - Transcendence map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate the pulls (x2-phase).

**Scope.** Planning settled the representation; T0 settles everything the code phases need, then corrects their kickoffs.
1. **Readers.** Classify every row of `plan_readers_unit.txt` / `plan_readers_my.txt`, and any reader they miss (other base registers,
   `+0x3c` via `CX2Unit*` getters, `CX2Room` slot units). Classes: engine (T1-T5), Aisha (T6), other class, UI owned by Stage 9.
   - Other-class rows go to a new checklist section (§7an "Transcendence, per class"), one row per class with its June addresses.
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
`Transcend T0`; checklist §7an; T7 opened or closed with the reason.

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

## Phase T2 - Becoming transcended: quest, notify, guide

**Run with:** Sonnet 5.5 high, then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_TRANSCEND_CHANGE`.

**Scope.**
- **Emulator.** `CompleteOneQuest` takes `QR_TRANSCENDENTAL_CLASS` instead of refusing (`Handlers_Quest.cpp:331-337`).
  - It applies June's conditions (second job, level 70 or more; T0's list), saves `transcendence = 1`, and returns the unit info in the ACK.
  - Then it sends June's 0x596 notify (offline event id at the end of its block, behind the flag) with June's two ints.
  - Classes allowed: the user's decision below.
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
  - key 1 by the medal 288318 and the quest's 15-day cube 293722, per the user's decision below;
  - the cool-time / attack slot buffs 843 / 844 if T0 puts them here.
- **UI:** the lock statics (`328dfab0`, `328e18d0`) already read T1's field. A locked-slot click (`SMUCM_DISABLED_TRANSCENDENTAL_*`, 208 / 209)
  opens what June opens (T0).

**Done gate:** on a transcended unit, a skill in A-5 casts from F with its state; B-5 follows key 1. On an untranscended unit both stay
locked with June's message; log `[JUNE_TRANSCEND] SLOT`.

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

Placeholder. T0 writes its scope, or closes it with the reason (`SERV_IRUHADEV_JUNE_SOCKET_PARITY` may already cover `SDT_*_TRANSCEND*`).

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
- Checklist §7an marks.
- The Stage 7 template's transcendence rows (each class port enables its class in T2's allow-list and ports its §7an row).
- A backlog Stage 6 closing note; `_DEBUG` flags off; memory.

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill and slot); per check: where / paste / what it does / look for /
if not. Log tag `[JUNE_TRANSCEND]` with the phase's sub-tag (`python tools/logs.py --grep "JUNE_TRANSCEND <sub>"`).

## Open decisions (for the user; asked at the end of planning)

1. **Who may transcend before their own port.** June lets all 37 second-job classes take Limit Break. A March-kit class would gain hyper-class
   tree rows with no states and March's awakening. Recommendation: an emulator allow-list (Aisha now), refusing other classes with a log
   line; each Stage 7 / 8 port adds its class.
2. **The B-5 / T slot (slot-state key 1).** June opens it only by the Skill Slot Change Medal (Transcendence) or the quest's 15-day cube.
   The emulator keeps March's `SKILL_SLOT_B_FREE` rule for B-1..4. Recommendation: June's rule (item, with expiry) for key 1.
