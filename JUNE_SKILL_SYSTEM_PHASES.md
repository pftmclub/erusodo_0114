# June skill system core: skill templets, skill tree, numerical data, 5+5 slots, tree UI, running-state pointers

Backlog Stage 3 (`JUNE_PORT_BACKLOG_ORDER.md`). Plan key `skill-system-core`; STATUS block `skill-system-core`; kickoffs
`kickoffs/skill-system-core/`. Planned 2026-10-17 (Opus 5.5 high, read-only; `/x2-phase new-plan skill-system-core`).

## Context

Every character stage (5, 7, 8), transcendence (6) and two buff types (BBT 98, BFT 16) sit on June's skill system. March has the
2013 "upgrade skill system" (`NewSkillTempletVer2.lua`, 521 templets, skill IDs 100-8409, 4+4 slots). June has the skill
reform: its `NewSkillTempletVer2.lua` is one file holding **2,346 templets and 3,360 skill-tree rows for 86 classes**. 3,237 of
the tree rows use reform IDs (21000-32606), and only 123 use March-range IDs. 451 of the 521 shared templets changed
(`june2017_re/c1a_skilltemplet_diff.tsv`). March's class scripts have no states for reform IDs, and no save has learned one.

**User decisions (2026-10-17, this planning session):**
1. **Flip everything in Stage 3.** June's `NewSkillTempletVer2.lua` (and its data closure) is adopted whole, and every save
   moves to June's trees in a forward-only migration: skills reset, SP refunded by June's rule. Unported March characters
   lose most usable skills until their own ports (Stages 5, 7). Their class-side states are character-port material.
2. **3f: adopt June's pointer model** (running state data as pointers into the state vector, with the `31fd2af0` snapshot and
   its four consumers) as the plan's last code phase.

## Rules

1. **June wins; no partial updates.** Where June changed a function March has, port June's whole function in the same pass.
   June bugs are kept verbatim with a `JUNE_BUGS.md` row (e.g. C1a's three numerical blocks that index past their data).
2. **Flags.** One `SERV_IRUHADEV_JUNE_SKILL_*` flag per logical change in `KTDXLIB/Always.h`, original code in `#else`,
   `#endif FLAG`. **The slot-count flag guards `CommonPacket.h`/`.cpp`, so it is also `#define`d at the end of
   `KncWX2Server/Common/ServerDefine.h`** and added to `SECOND_HOMES` in `tools/flagcheck.py` (CLAUDE.md, `CommonPacket.h`
   exception). That one define reaches both projects. `X2ServerProtocol` reads `CommonPacket.h` through
   `X2ServerProtocolLib.h:68` -> `SystemPacket.h:5` -> `CommonPacket.h:16` -> `ServerDefine.h`. `X2Lib` reads it through
   `X2Main.h:4`, before `KTDX.h`. **Prove both with `cl /P`** (S5 gate): `june2017_re/flagprobe.py` covers X2Lib only, so S5
   adds an X2ServerProtocol probe.
3. **Data.** June's `.lua` arrives through `luasrc.py adopt-june` as `ljconv` bytecode (not core-list). Only a file this plan
   must edit gets a `core --add` row and `--source`. **No new `.lua` files.** `/elstools pack`, then ask before every
   `/x2-kom-packed copy`. **The exe, data036 and the save move together** (the K2 rule): deploy, copy and migrate in the same
   hand-off, play-test only when all three are live, and revert all three together.
4. **Saves.** `els_db.sql` is the user's only copy. Each migration phase takes a `db_backup/` snapshot before the migrating
   launch, adds one rung to `CX2OfflineDB::Migrate()` above v16 (Atlas A2), and is forward-only: an exe without the flag
   refuses the newer save, as v16 does. Keep `SetUsingPage`'s rule: page numbers in every ACK are `>= 1`, never 0
   (`MARCH_2014_MIGRATION.md:1068-1100`).
5. **Character material goes to the checklist** (memory `hand-char-port-material-to-char-ports`): class-script states for
   reform IDs, 7i's slot-493 call, the per-class `IsSummonSkillByJune`-style lists. This plan ports the engine and the data
   files that are not class scripts.
6. **Offline only.** Shared-header changes are flag-gated and the emulator (`X2Lib/Offline/`) is the only other side. No server
   code, and no desync questions.
7. **Opus phases delegate** reads, greps, builds and log reads (memory `opus-phases-delegate-mundane-work`).
8. **Never launch `X2_offline.exe`**: ask the user.

Flag family `SERV_IRUHADEV_JUNE_SKILL_*` (debug: `SERV_IRUHADEV_JUNE_SKILL_DEBUG`). Log tag **`[JUNE_SKILL]`**, with a sub-tag per
phase (`LOAD`, `MIG`, `NUM`, `SLOT`, `UI`, `PTR`): `python tools/logs.py --tag JUNE_SKILL`. Funcmap tag `Skill <phase>`.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "<heading>"` |
| June Lua (read) | `june_src_fixed/data036_kom/*.src.lua`. **`NewSkillTempletVer2(_LA).src.lua` are there since S1 (2026-10-17), proven** (`--check` clean, T1). The old `tmp/decompile_sweep/` copy is obsolete: it does not compile |
| June kom reader | `python tools/junekom.py list / extract` (never by hand) |
| Bring June Lua in | `python tools/luasrc.py adopt-june data036 X.lua` (x2-adopt-june) |
| Ghidra | `ghidra` skill (read-only for RE phases) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Save | `python tools/elsdb.py` (x2-db); snapshot to `db_backup/` before a migrating launch |
| Flag probe | `python june2017_re/flagprobe.py FLAG` (X2Lib); the `cl /EP` recipe at `MARCH_2014_MIGRATION.md:634-650` |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `build.py --deploy` (background), `logs.py --tag JUNE_SKILL` |

## Investigation ledger (planning, 2026-10-17)

Facts re-checked against the backlog's Stage 3 facts 1-3 (measured 2026-10-03):

- **Fact 1 (templets not in `june_src_fixed/`): holds for `NewSkillTempletVer2(_LA)` only.** Their decompile is `clean/complete`
  (`june2017_re/decompile_corpus.tsv:1634-1635`, 5,208,709 / 5,232,904 B) but fails the compile round-trip
  (`decompile_verdicts.tsv:42457-42458`, STEP_ERROR; L3: "`slot3` global; a `}` expected"). **Corrected:** the companion tables
  are already in `june_src_fixed/data036_kom/` (`OptionItemData(_LA)`, `PetTemplet(_LA)`, `TitleTable(_LA)`, `FieldQuest`,
  `FieldSubQuest`, `AccountQuest`, `GuildQuest`, `ChainQuestInfo`, `Quest_Event_Scene_Info`, `TitleMission`, ...). Shipping does
  not need the decompile: the file is off the core list, so it arrives as `ljconv` bytecode.
- **Fact 2 (override callees unknown): corrected.** Effect C1a confirmed them: override `3263fe41` (`SetDamageDataFromLUA`, March
  site before `EndTable`, `X2DamageManager.cpp:605`); getters `320717e0` (by index, templet `+0x17c`), `32078060` (by name);
  clamp `320716c0`; parse `32216320 @322186f5` (funcmap 1302-1306). Rules: checklist section 1 (lines 37-93). March parses the
  keys and logs them as unsupported (`X2DamageManager.cpp:268-275`).
- **Fact 3 (slots): holds, with three corrections.**
  - The `#define EQUIPPED_SKILL_SLOT_COUNT 4` is at `CommonPacket.h:6411` (not `:6387`). It is unconditional and shadows the
    static member at `:1051`.
  - **Two structs carry the slot arrays:** `KUnitSkillData` (`:1050-1105`, Put/Get `CommonPacket.cpp:760-818`) and
    `KUserSkillPageData` (`:6425-6452`, `SERV_SKILL_PAGE_SYSTEM`, Put/Get `:5890-5920`). Both Put/Gets hardcode indices 0-3.
  - About 60 uses sit across 15 X2Lib files: `X2Game.cpp` (9), `X2UISkillTree.cpp` (13), `X2UISkillTreeNew.cpp` (13),
    `X2SkillTree.cpp` (5), `X2GUUser.*`, `X2GuEve.cpp`, `X2GageManager.cpp`, `X2Room.cpp`, `X2UserSkillTree.cpp`,
    `X2UIGuildSkillTree.cpp`, `X2OfflineSkill.h:54-61`. The uses in `X2GUUser.h:4887-4890` are four-element state-id arrays.
  - **The save** is `unit_skill( unit_uid, skill_id, level, slot DEFAULT -1, csp )` (`X2OfflineDB.cpp:86-91`, `:151`). `slot` is
    0-3 for A and 4-7 for B (`X2OfflineSkill.h:46-48`). Writes go through `SetSkillSlot` (`:2358`).
  - Skill pages and the B-slot expiry are **not stored**. The emulator sends one page (`Handlers_Unit.cpp:782-801`) and a
    permanent B slot (`X2OfflineSkill.cpp:276-281`).
  - The page packets (`EGS_EXPAND_SKILL_PAGE`, `EGS_DECIDE_TO_USE_THIS_SKILL_PAGE`, `EGS_GET_NEXT_SKILL_PAGE_ED`) have no
    handler and no ignore row.
- **June slot model** (buff Phase 6, `JUNE_BUFF_PORT_PHASES.md:783-803`):
  - `FindEquippedSlotIndex 3288fff0` returns 0-9: five A rows at `+0x48` and five B rows at `+0x23c`, stride 100.
  - `SlotIndexToSlotType 3214f3d0` maps 0-3 to A, 5-8 to B, and 4 and 9 to TC (no funcmap row yet).
  - So June's "5+5" is 4 A + 1 TC and 4 B + 1 TC. March's B slots 4-7 become June's 5-8.
- **Dormant code waiting here.**
  - BBT 98: `SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL` (Always.h:4806). Reader `32039e00`; March's use is at `X2GUUser.cpp:25244`.
  - BFT 16: `SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER` (Always.h:4930). `CountReservedSkillUse` is at `X2GUUser.cpp:39096-39125`.
  - `GetEquippedSkillSlotType` (`X2UserSkillTree.cpp:1425-1445`) never returns TC (3).
- **The forced-off `_ULTIMATE` gates** (effect C3l). They switch on with S2:
  - `SPEED_TIME_ULTIMATE` (`SERV_IRUHADEV_JUNE_SPEED_TIME`, stub at `X2GUUser.cpp:15249-15253`);
  - `BUFF_FACTOR_ID_ULTIMATE` (`SERV_IRUHADEV_JUNE_BUFF_FACTOR_ID`, `31fcf3e0`);
  - DamageEffect's `_ULTIMATE` keys (`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE`).
- **March loader.** `X2StateLoading.cpp:722` calls `ResetSkillTree( L"NewSkillTempletVer2.lua", L"SkillData.lua" )`, which runs
  `CX2SkillTree::OpenScriptFile` (`X2SkillTree.cpp:102`). It loads `SkillData.lua`, then the templet file, then
  `NewSkillTempletVer2Trans.lua` (`:123`, `LUA_TRANS_DEVIDE`; the file is absent from March's data036).
  - Bindings are at `X2Main.cpp:5150-5170`: `AddSkillTemplet_LUA`, `AddSkillTreeTemplet_LUA`, `AddSkillPowerRate_LUA`,
    `AddDefaultSkill_LUA`, `AddSealSkillInfo`, `AddSkillTempletTrans_LUA`, `AddGuildSkillTreeTemplet_LUA`.
  - The struct is `CX2SkillTree::SkillTemplet` (`X2SkillTree.h:2005-2275`).
  - June's file calls only `AddSkillTemplet_LUA` (2,346), `AddSkillTreeTemplet_LUA` (3,360), `AddGuildSkillTreeTemplet_LUA` (12)
    and `OpenScriptFilePostProcess` (1). June's `SkillData.lua` holds `CalcLevelUpIncreaseSkillPoint` and `AddSealSkillInfo`
    over reform IDs.
- **Existing local flags in the area:**
  - `SERV_IRUHADEV_SKILLTREE_NO_LOCK` (Always.h:3447; `X2UISkillTreeNew.cpp:408-414`, `:6355-6417`);
  - `SERV_IRUHADEV_SKILL_SLOT_B_FREE` (Always.h:3454; four sites);
  - `SERV_IRUHADEV_JUNE_COMMON_SKILL_STATE` (Always.h:6614).

  S0b decides whether the first two survive June's tree and slot rules: both are user conveniences, not June behaviour.
- **UI.** The slot dialog is `DLG_GAME_SKILL_SLOT_NEW.lua` (`X2UISkillTreeNew.cpp:1003`, controls `SlotA1..SlotB4`), and the tree
  is `DLG_UI_Skill_Tree_New.lua` (`:2187`). June's slot dialog is `DLG_GAME_SKILL_SLOT_NEW_10slot` (`SlotA1..A5`/`B1..B5`).
  The backlog's `DLG_SkillTree_*` / `DLG_Skill_Tree_Main` names are unconfirmed: S0b finds June's real ones.

## Phases

| Phase | What | Run with | Code | Play-test |
|---|---|---|---|---|
| S0a | RE: June's templet and tree loader, `SkillData`, learn / reset / SP / default-skill rules, data closure (3b, 3c) | Opus 5.5 high | no | no |
| S0b | RE: June's slots (A/B/TC), skill pages, slot and tree UI, emulator packets (3d, 3e) | Opus 5.5 high | no | no |
| S1 | Extract `NewSkillTempletVer2(_LA)` as checked source into `june_src_fixed/` (3a) | Opus 5.5 **xhigh** | tools | no |
| S2 | Templet loader and `CX2SkillTree` / `CX2UserSkillTree` to June; `m_DeleteSkillIDByUpgrade`, `+0x20` map, upgrade swap, `_ULTIMATE` gates on (3c) | Sonnet 5.5 high + Opus review | yes | yes (March data) |
| S3 | **The flip:** June's skill data closure in data036, save migration v17 (skills reset, SP refund), emulator learn / reset / init to June's rules | Opus 5.5 high + review | yes | yes |
| S4 | Numerical skill data override (3b) | Sonnet 5.5 high + Opus review | yes | log self-check |
| S5 | Slots 4+4 -> 5+5 (A/B/TC): shared structs, all slot users, BBT 98 / BFT 16 wired, TC locked, emulator, save migration v18 (3d) | Sonnet 5.5 high + Opus review | yes | yes |
| S6 | Slot UI: `DLG_GAME_SKILL_SLOT_NEW_10slot` and June's slot-UI code (3d) | Sonnet 5.5 high + Opus review | yes | yes |
| S7 | Skill tree UI and learn logic to June, plus the emulator's page packets as June has them (3e) | Opus 5.5 high + review | yes | yes |
| S8 | Running state data: June's pointer model, `31fd2af0` snapshot, four consumers (3f) | Opus 5.5 high + review | yes | yes |
| Z | Close-out: `MODS.md`, backlog notes, checklist hand-offs, memory | Sonnet 5.5 medium | no | no |

**Model choice.**
- **Opus for RE and new mechanisms** (S0a, S0b, S7, S8): a wrong reading there costs a whole port.
- **Opus for S3**: it rewrites the user's only save, and it decides what a June-tree character starts with.
- **Opus xhigh for S1**: decompiler defects in a 5 MB file, the same class of work as lua-source L5.
- **Sonnet high for transcription from a written map** (S2, S4, S5, S6), each with a scoped Opus review: the slot change is
  wide and a single missed site corrupts a packet silently.

**Order and parallelism.**
- **S0a, S0b and S1 can run now, in parallel.** They are read-only or tools-only.
- **Code:** S2 -> S3 -> S4, and S3 -> S5 -> S6. S7 needs S3 and S0b, and never runs alongside S6 (both edit
  `X2UISkillTreeNew.*`). S8 needs S5. All code phases are one at a time on `main`, because of the build lock and the shared
  `Always.h` block.
- **The UI phases (S6, S7) also need june-engine U1** (the UI framework port), because June's DLGs may use its methods.
- **Relaxation of the backlog brief, recorded here.** The brief wants "Stage 2 done" before Stage 3 code. Only the UI phases
  depend on Stage 2, so S2-S5 and S8 do not wait for it.
- **Not alongside Atlas E1 / E5 / E6 / E7.** S2 edits `X2Main.cpp`'s binding block, which those phases also edit.

## Phase S0a - Templet and tree RE

**Run with:** Opus 5.5 high, read-only (Ghidra, no code, no build). **Read list:** this doc's Context, Rules and Investigation
ledger; `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 1 (lines 37-93); `JUNE2017_EFFECT_COMPLETION_PHASES.md` C3l Done and
review (lines 2292-2408); funcmap rows tagged C1a, C3l and Buff P10 (`32216320`, `32206450`, `32890720`, `328905b0`, `32071720`,
`31fca8e0`, `320717e0`).

Map, from the binary, with June addresses (funcmap rows tagged `Skill S0a`):
1. **`AddSkillTemplet_LUA 32216320`, every key.** For each key give: name (XOR string), type, default, store offset, March
   equivalent or "new", and its readers. Keys for absent characters are stored and logged, never dropped. Produce
   `june2017_re/s0a_templet_keys.tsv`. Do the same for `AddSkillTreeTemplet_LUA`, `AddGuildSkillTreeTemplet_LUA` and
   `OpenScriptFilePostProcess`. Note what March parses that June no longer does: `AddSkillPowerRate_LUA`, `AddDefaultSkill_LUA`,
   the Trans file.
2. **`CX2SkillTree` and `CX2UserSkillTree`, June vs March, function by function** (no-partial rule). Cover the `+0x20` old->new
   map (`32205c00`, insert `@322186ca`), the upgrade swap `328905b0`, `GetSkillPowerRateValue 32071720` and its `32206fb0( 0xad,
   level )` table, and how June resolves "is this skill learned / equipped" for an upgraded skill.
3. **Which files June's US build loads, and in what order.** Settle whether June loads `_LA`, `SkillData.lua` and a Trans file.
   Also give the data closure: every data036 / data034 file keyed by skill ID that must flip with the templets
   (`SkillNoteTemplet`, `SkillLevelUpSocketGroup`, `SkillCommandDescriptionData`, `SkillTypeColor`, `ComboTree`, the
   `DamageEffect` / `EffectSet` blocks already June's, item templets that grant skills or SP). Output
   `june2017_re/s0a_skill_data_closure.tsv`.
4. **Learn rules for S3:** SP per level (`CalcLevelUpIncreaseSkillPoint`), the default skills a new or reset unit gets, master
   level and row / column gating, CSP, seal and unseal over reform IDs, and skill-note items. Base this on June's own client
   code. For server-only values, follow the memory rule `design-missing-june-pieces` (June's default where the value is absent,
   else a labelled stand-in proposed to the user).
5. **The migration's input:** a census of the save's learned skills per unit (`elsdb.py`), and what each March ID maps to in
   June (same ID kept, upgraded via `+0x178`, or gone). S3 resets them anyway, but the census decides what the hand-off says.
6. **Numerical override (S4's map):** confirm C1a's rows and write the exact transcription spec. That covers the condition,
   the row pick by learned level, the clamp, x0.01, PVP_RATE / TECH_POINT / HIT_ADD_MP defaults, and the three past-the-end
   blocks for `JUNE_BUGS.md`.

**Done gate:** the two tsvs; funcmap rows; a Done note under ~60 lines with S2 / S3 / S4 amendments written into their kickoffs;
and open questions for the user. Expected: SP stand-ins, if June's client has no SP table.

**Done note (2026-10-17, Opus 5.5 high): S0a done.** Read-only: no flag, no build, no deploy, no pack, no save write.
Sonnet subagents did the March `cl /P` key list, the save census, the closure survey and the TU decompile map; the parsers, the
override and every row below were read in the binary by Opus. Data counts were taken from the unproven decompile and re-checked on
S1's proven `june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua` (2,346 / 3,360 / 12 / 1).
- **Item 1.** `june2017_re/s0a_templet_keys.tsv`, 260 rows: the templet parser (37 keys + 202 SA tables), tree, guild tree,
  power rate, default skill, seal, type colour, post-process. June's SA 1-142 equal March's; 143-202 are new.
  - Behaviour changes for S2: no 30-entry padding (cool time, MP, required level); empty SA table not inserted;
    `IN_HOUSE_TEST_ONLY` drops the templet; tree rows lose `m_iPrecedingSkill`; seal takes `m_iItemIDList`.
  - The Lua `OpenScriptFilePostProcess` is a bare `RET` (`31f3f070`).
  - `GetSkillAbilityValue 32206e50` reads a `NUMERICAL_DATA` block named after the SA type first (1,056 blocks).
- **Item 2.** `june2017_re/s0a_skilltree_tu_map.tsv`, 415 rows (Sonnet candidate map, `confidence` column; the rows S2 needs
  were checked here).
  - Upgrade swap `328905b0`: a base id resolves to its "[Enhanced]" skill once that one is learned. It is applied inside
    `GetSkillLevel` and `GetUserSkillTemplet`.
  - June's CX2SkillTree has no GET_SKILL / RESET_SKILL_REQ handler: the learn path is S7's (candidate).
- **Item 3.** `june2017_re/s0a_skill_data_closure.tsv`, 35 rows. US order: `SkillData.lua`, `NewSkillTempletVer2.lua`
  (`_LA` only for argv `Axeso5`, `@3259033f`), then `CommandIconData.lua` (S7). No Trans file. S3 flips SkillData + the templet
  file; `SkillCommandDescriptionData` / `SkillTypeColor` / `ComboTree` / `CommandIconData` go to S7; `SkillNoteTemplet` to the
  new **S3b**.
  - **Blocker for S3:** March's core `Enum.lua` lacks `SPRT_HYPER_CLASS` (223 templets), 9 `BUFF_FACTOR_ID` names and
    39 `UNIT_CLASS` names. Under ljconv they read nil and default silently (S3 amendment 2).
- **Item 4.**
  - **SP needs no stand-in:** `GetCalcInitSkillPoint 32205790` and `CalcLevelUpIncreaseSkillPoint` are identical in both builds.
  - June data: 3,267 of 3,360 tree rows cost 0 SP, 3,039 have master level 1, and 1,389 "[Enhanced]" rows need level 71-98.
  - June has **no default skills**: `AddDefaultSkill_LUA` is called 0 times (March 52).
- **Item 5.** Census: 113 learned skills, kept 0 / upgraded 0 / gone 113, because no saved id is on a June tree row. 31 unsealed
  rows and 3 note rows are March ids and go too. Per unit (uid class: rows): 12 103: 20, 18 117: 15, 19 109: 8, 20 8: 2,
  21 30: 13, 22 5: 2, 23 107: 6, 24 119: 9, 25 100: 7, 26 101: 8, 27 109: 5, 28 105: 4, 29 120: 7, 30 121: 2, 31 114: 3,
  42 111: 1, 43 110: 1; units 32-41: none.
- **Item 6.** C1a re-read at `3263fe41-32640054`: confirmed. The transcription spec, the three past-the-end blocks and the
  self-check are in S4's amendments.
- Funcmap: 26 rows tagged `Skill S0a`. Kickoffs `S2.md`, `S3.md`, `S4.md` carry "S0a amendments"; `S3b.md` is new.

**Open questions for the user** (each has a labelled stand-in in S3's amendments):
1. **Default skills.** June's client defines none. **Recommended:** the migration and `createchar` grant every free (0 SP,
   master 1, unsealed, non-upgrade) tree skill at or below the unit's level, under its own flag `..._REFORM_AUTOLEARN`.
   The June default is to grant nothing.
2. **Upgrade.** Does learning an "[Enhanced]" skill remove its base? **Recommended:** keep the base. June's swap works either
   way; S7 settles it from June's learn UI.
3. **Unseals.** March unseals do not carry over. **Recommended:** none are carried; units unseal again with June's items.

## Phase S0b - Slots, pages and UI RE

**Run with:** Opus 5.5 high, read-only. **Read list:** Investigation ledger; `JUNE_BUFF_PORT_PHASES.md` lines 783-803 and
1279-1301 (98 reader, BFT 16 counter); `MARCH_2014_MIGRATION.md` lines 1068-1100 (`SetUsingPage`); effect C9h-6 Done
(`JUNE2017_EFFECT_COMPLETION_PHASES.md:8163-8200`).

Map:
1. **June's `KUnitSkillData` / `KUserSkillPageData` layout.** Give the June Put/Get order (from the deserializers) and whether
   TC slots are on the wire. Note any field June added (page counters, TC unlock state). Settle the S5 struct spec exactly.
2. **Every June reader of slot indices:** `FindEquippedSlotIndex 3288fff0`, `SlotIndexToSlotType 3214f3d0`,
   `GetSkillEquippedSlotIndex 32890070`, `GetSkillSlot 3288fe00`, `ResetLeftSkillCoolTime 32896600`, the gage / cooltime UI,
   the quick keys (which key fires slot 4 / 9), `ChangeEquippedSkillState`, and the slot-type count for BFT 16. How the TC
   slot is locked (S5 keeps it locked until Stage 6).
3. **Skill pages in June:** do pages still exist, and how many; the page packets and their ACK fields; the expand-page cost.
   The emulator gets handlers or ignore rows in S7.
4. **UI:**
   - June's slot dialog (`DLG_GAME_SKILL_SLOT_NEW_10slot`) and its code (S6).
   - June's tree dialog names and the tree UI class, function by function against `X2UISkillTreeNew` (S7).
   - The learn / reset / purchase windows. Census their June DLG files against March's data034 / data036
     (`june_src_fixed/` names), and the UI framework methods they need (june-engine U0 / U1 list).
5. **Pointer model (S8's map):** June's state vector `+0x7cec` (100-byte entries), `+0x7cb0` / `+0x7cbc`, the `31fe6a40` store,
   every reader of Now / Future state data in `CX2GUUser` and `CX2GameUnit`, and the snapshot `31fd2af0` with its four
   consumers. List March's by-value copies (`X2GUUser.cpp:6585`, `:6644`, `operator=` at `X2GUUser.h:1024`) and every holder of a
   reference that could dangle when the vector reallocates.
6. **`SKILLTREE_NO_LOCK` / `SKILL_SLOT_B_FREE`:** say what June does at each of their sites, and recommend keep (as a local
   convenience on top of June) or retire. The user decides.

**Done gate:** funcmap rows `Skill S0b`; `june2017_re/s0b_slot_sites.tsv` (every March `EQUIPPED_SKILL_SLOT_COUNT` / slot-index
site with its June equivalent and the S5 action); S5-S8 kickoff amendments; a Done note under ~60 lines.

### S0b - Done (2026-10-17, Opus 5.5 high; read-only: no code, no flag, no build)

There are 21 funcmap rows tagged `Skill S0b`. New files in `june2017_re/`:
- `s0b_slot_sites.tsv`: 2,357 March sites, with `june_equivalent` / `phase` / `action` columns. 1,303 are S5, 336 are S6,
  and 717 are checklist rows (per-class `SAKP_`, state tables and slot-493 code).
- `s0b_ui_funcmap.tsv`: 121 rows for the June tree UI.
- `s0b_skill_dlg_census.md`.
- `s0b_state_ptr_sweep.tsv`: 790 hits.
- `s0b_march_state_ptr_map.md`.

The kickoffs `S5.md`-`S8.md` each carry an "S0b amendments" section.

- **Item 1 (wire).**
  - **`KSkillData` is 7 bytes in June** (`320ef0e0`): id, level, and a June-only 4-byte int with no mapped reader.
  - **`KUnitSkillData`** (`320e3670`, 200 B) has `A[5]` and `B[5]`, so the TC slots are on the wire. June dropped the
    B end-date / expiry fields for a `vector<{key, state, date}>`, then kept passive, guild passive, note and the two page
    bytes, and added a June-only struct (`320dc980`).
  - **`KUserSkillPageData`** (`320dc120`) has `A[5]`, `B[5]`, SP, CSP and the skill vector.
  - The exact spec is in S5's amendments.
- **Item 2 (readers).**
  - Index split `3288fe30`: B is 5-9.
  - State ids `0x32-0x45` are 10 (normal, hyper) pairs (`32890140`).
  - The slot ACK `31ed4c80` -> `32895d20` + `ChangeEquippedSkillState 31fd84c0` (4-arg `31fd8ff0`). Bar re-bind
    `31fcd660`. Gage `i + 5`.
  - Keys: pressed slot `31fcadd0`. Slot 4 = `GAMEACTION_SKILL5` (F), 9 = `SKILL_EX5` (T). June's enum inserts both, so
    every later action id shifts.
  - BFT 16 counts keys 0-3 (any, A, B, TC) through `3214f3d0`.
  - **TC lock.** Slot 4 needs `CX2Unit +0x3c` (transcendence), which March lacks, so it stays locked until Stage 6. Slot 9
    needs slot-state key 1, and B 5-8 need key 0 (map `+0x434`, `32892a00` / `32890ee0`: state 1 or 2 usable).
- **Item 3 (pages).**
  - June keeps 3 pages (SP/CSP `USHORT[3]` at tree `+0x4a8` / `+0x4ae`).
  - Decide: REQ `3220f160` (`0x47a` / `0x47b`). The ACK handler is `31f1fd90`, and its ACK carries a whole `KUnitInfo`
    (March's does not). Expand is by ED or cash. The cost is server-sent, so there is no client table.
- **Item 4 (UI).**
  - June's only tree class is **`CX2UISkillTreeNew2015`** (`328d98e0`-`328ee680`, 103 functions: 6 match, 40 changed,
    53 June-only, 18 March-only).
  - Its dialogs are `DLG_UI_Skill_Tree_New2015` + `DLG_UI_Skill_Icon_New2015`, and the slot dialog is
    `DLG_GAME_SKILL_SLOT_NEW_10slot` (the 4+4 file is not loaded).
  - **The June tree sends no learn and no reset REQ.** The learn / upgrade / init cases are gone. The memo select sends
    `0x554` / `0x556`. This agrees with S0a item 2 ("no GET_SKILL / RESET_SKILL handler"); S7 finds the learn path first.
  - All 100 widget methods the June skill DLGs use are bound in March. What is missing is **enum members** in the merged
    `Enum.lua` (`PVE_GAME_UI_CUSTOM_MSG`, `UI_TOOLTIP_TYPE`, June `STUCM_*` / `SMUCM_*`), so S6 may not need june-engine U1.
- **Item 5 (pointers).**
  - March's `m_StateList` is a `std::map` (node based, no erase), so the "dangling after a vector reallocation" risk does
    not exist in March.
  - The real hazards are the 7 `m_fPowerRate *=` sites, which would compound, and `Init()` on a running entry during
    re-equip.
  - The copies are now at `X2GUUser.cpp:6697` / `:6761` (the ledger's 6585 / 6644 have moved).
  - June readers: about 50 user / game-unit functions and about 318 class functions (sweep tsv).
- **Item 6 (user, 2026-10-17).**
  - `SKILL_SLOT_B_FREE` is **kept via the emulator**: it sends key 0 = permanent, and S5 retires the 4 client sites.
  - `SKILLTREE_NO_LOCK` **retires with S7**: June has no two-choice blind (`SetBlindSelectSkill` and its tier logic are
    March-only), so the June tree has nothing to unlock.
- **Open for later phases.**
  - The meaning of `KSkillData +3`.
  - The `KUnitInfo +0x30c` map and `320dc980`'s consumer.
  - June's event-id names for `0x554` / `0x556` / `0x735`.
  - Whether `GAMEACTION` renumbering touches the emulator's stored key map (S5).

## Phase S1 - Extract NewSkillTempletVer2

**Run with:** Opus 5.5 xhigh. Tools only, no game code. **Read list:** `LUA_SOURCE_PHASES.md` L3 Done note and Phase L5;
`RANOX_LUA_DECOMPILE_BUG.md` "Residual ljd defects".

- Fix the round-trip failure for `NewSkillTempletVer2.lua` and `_LA.lua` as **tracked hooks** (like `june2017_re/ljd_fixed.py`),
  never by editing the vendored ljd silently. Known causes: the `]]`-inside-a-long-string printer defect, and L3's "`slot3`
  global; a `}` expected".
- Verify with `ljd_jun2017.py --check` (and `luasrc.py check --ref june` for T1). Write the files to
  `june_src_fixed/data036_kom/NewSkillTempletVer2(_LA).src.lua`.
- If T1 cannot be reached, ship nothing to `june_src_fixed/`. Name the residual defect, and leave S0a / S2 on the unproven
  decompile, cited as such.
- Not a shipping step: the game gets June's bytecode via `adopt-june` in S3. Re-run the decompile corpus row so
  `decompile_verdicts.tsv` records the pass.

**Done gate:** `--check` pass (or the named blocker); the two `.src.lua` files; the tool change committed with its self-test.

**Done note (2026-10-17, Opus 5.5 xhigh): S1 done.** Tools only: no flag, no build, no deploy, no kom pack or copy, no save.

**One defect, not two.** Both files fail only on ljd's long-bracket printer. L3's "`slot3` global" message belongs to
`DamageEffect_Dungeon` (IIFE parens, still open). The failure was 12 skill descriptions per file ending in combo notation; the first
is `m_eID = 7004`, "Dragon Stance 4: Moonlight Slash", `...XX↓X[X]]` printed as `[[...[X]]]`.

**The fix: `june2017_re/ljd_writer_fix.py` (new).**
- It is a run-time hook on `ljd.lua.writer.Visitor.visit_constant`. It refuses unless `writer.py` has SHA-256 `001f3364...`, which
  both vendored copies have. The vendored ljd is not edited.
- `ljd_fixed.install_fix()` installs it after the slotworks fix, so every fixed-ljd caller gets it (`ljd_jun2017.py`,
  `ljdiff --fixed`, `luasrc.py decompile` / `adopt-june --source`). The summary line ends in `writer fix active (...)`;
  `--unfixed` stays plain ljd.
- It changes only strings plain ljd gets wrong: `]]` inside or `]` at the end gets the lowest `[=*[` level that does not collide;
  a value with a `\r` gets ljd's own quoted form. A long string folds `\r` to `\n`, so this third case was a silent value change.
- `python june2017_re/ljd_writer_fix.py --selftest`: **PASS**, 13 cases through luajit. The 5 cases plain ljd already got right print
  the same bytes; the 8 it got wrong are repaired, and each one is proven to fail under plain ljd.

**Results** (June bytes from `tools/junekom.py`; sizes = the manifest sizes; T1 = `luasrc.py check --ref june`):

| file | `--check` (32 seeds) | T1 | leveled / requoted | corpus row | `june_src_fixed` sha1 |
|---|---|---|---|---|---|
| `NewSkillTempletVer2.lua` | clean | T1, 145,088 ins | 12 / 0 | `STEP_ERROR` -> `clean` | `353dd76d` (6,818,275 B) |
| `NewSkillTempletVer2_LA.lua` | clean | T1, 145,088 ins | 12 / 0 | `STEP_ERROR` -> `clean` | `43c22e55` (6,836,680 B) |
| `PetTemplet.lua` (side find) | - | T1, 13,246 ins (old copy: FAIL, 19 diffs) | 0 / 129 | `other-ljd` -> `clean` | `239f47a3` (603,909 B) |

- **What the fix changed.** In both templet files the only lines that differ from the pre-fix decompile are the opening and closing
  lines of the 12 re-levelled strings (24 lines).
- **Text format.** The files are plain ljd text, the format every `june_src_fixed/` file has. Phase 8's `corpus_run.py` +
  `phase8_worker.py --fast` wrote them. They are byte-identical to `ljd_jun2017.py`'s output.
- **Table order.** 2,358 tables store their pairs in another order; T1 ignores that by rule. `luasrc.py decompile` adds the
  layout repair, which brings this to 0, if a later phase ever needs the file as source.
- **Corpus rows.** `decompile_verdicts.tsv` rows are re-run with `phase8_aggregate.py`: 3 replaced, 42,472 kept, "Phase 4 fix holds".
- **PetTemplet backup.** The old `PetTemplet.src.lua` is backed up at `.build/s1/backup_june_src_fixed/`. A scan of all 7,313
  `june_src_fixed/` files found no other CR inside a long string.
- **Regression checks.** `luasrc.py selftest` PASS 517/517, `ledger --check` PASS, `core --check` PASS.
- **Docs updated.** `RANOX_LUA_DECOMPILE_BUG.md` Phase 8 section, `LUA_SOURCE_PHASES.md` L5 (its long-bracket item is done), and
  `june2017_re/README.md`.

**Not done (out of scope):**
- No patch was applied to either vendored ljd, so `elstools.exe decompile-batch` still prints the old way.
- No ledger or core row was added: `NewSkillTempletVer2` is off the core list, and S3 ships June's bytecode through `adopt-june`.

**For S0a / S2:** read `june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua`. It is proven; `tmp/decompile_sweep/` is not.

## Phase S2 - Templet loader and skill tree to June

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_TEMPLET`. Sub-tag `[JUNE_SKILL] LOAD`.

- Port June's `AddSkillTemplet_LUA` whole, from S0a's tsv: every key, stored. Do the same for `AddSkillTreeTemplet_LUA`, the
  guild tree and the post-process.
  - Keys for absent characters and for systems not yet ported are stored and logged once per key name.
  - `SkillTemplet` gains June's fields: `m_DeleteSkillIDByUpgrade` (`+0x178`) and the `NUMERICAL_DATA<n>` vector (`+0x17c`, for
    S4).
- Port June's `CX2SkillTree` / `CX2UserSkillTree` functions per S0a item 2: the `+0x20` map, the upgrade swap `328905b0`, and
  `GetSkillPowerRateValue 32071720`.
- Switch on the three `_ULTIMATE` gates: replace the constant-false stubs (`X2GUUser.cpp:15249-15253` and the other two flags'
  sites) with June's test under this flag.
- **Data stays March's in this phase.** The ported parser must load March's `NewSkillTempletVer2.lua` with no behaviour change.
  That is the play-test: every character's tree, learned skills and damage as before; one `[JUNE_SKILL] LOAD` summary line
  (templets, tree rows, keys seen).
- Also prove the June file parses before S3 flips it. On a scratch load, under `SERV_IRUHADEV_JUNE_SKILL_DEBUG`, log the
  counts June's file produces (2,346 / 3,360 / 12).

**Done gate:** March data loads unchanged (log counts equal the pre-port counts); the June-file scratch counts match; flagcheck
PASS; `DEPLOY OK`.

**Done note (2026-10-18, Sonnet 5.5 high): S2 built + deployed, play-test pending.** Flag `SERV_IRUHADEV_JUNE_SKILL_TEMPLET` (+ short-lived `_DEBUG`), tag `[JUNE_SKILL] LOAD`.
No kom, data or save change. Revert and the full list of changes: `MODS.md` "June skill templet loader". Funcmap: 7 rows `(S2)`.
- **Loader.** `AddSkillTemplet_LUA` returns `AddSkillTemplet_JuneLUA()` (March's body stays as dead code, reachable only by the debug compare). Every amendment-1 point is in:
  no padding (fill 0), empty SA table not inserted, `IN_HOUSE_TEST_ONLY` drop, duplicate keeps the first and returns true, `m_ExtraIconInfo<n>`, `m_DeleteSkillIDByUpgrade` + upgrade map,
  `NUMERICAL_DATA<n>` (rows `{value, pvp, tech, hit_add_mp}`, side arrays clamped), SA 143-202 (60 enumerators, `SA_END` = 203, `C_ASSERT`s pin 142/173/202/203), `SPRT_HYPER_CLASS` = 5.
  The SA tables run from a table copied from `s0a_templet_keys.tsv`; `SA_TEAM_ATK_MAGIC_REL` -> 13 kept (JUNE_BUGS 272).
- **Two deliberate supersets of June (flagged here, S3 removes them with the data flip).** (1) `AddSkillTreeTemplet_LUA` keeps reading `m_iPrecedingSkill`: June dropped the read, but June's data
  has it on 0 unit rows (no-op on June data) while March's data would lose every prerequisite. (2) `AddSealSkillInfo` reads `m_iItemIDList` and falls back to the scalar `m_iItemID`.
  The amendment-8 "exactly four points" therefore holds for the templet parser; these two are the only other places March data meets a June rule.
- **Already identical to March, no change:** guild tree parser, `AddSkillPowerRate_LUA` / `SetSkillPowerRate` (30-entry rule), `AddDefaultSkill_LUA`, OpenScriptFile's load order (no Trans file: `LUA_TRANS_DEVIDE` is off).
  Lua `OpenScriptFilePostProcess` is bound to a no-op; `IsMyClassSkillID_LUA` and `GetMaxLimitedSkillLevel_LUA` were already bound.
- **Lookups and gates.** `GetUpgradedSkillID` (32205c00), `GetUpgradeSwappedSkillID` (328905b0, current page's acquired map) with a trailing `bUpgradeSwap_` (default false) on `GetSkillLevel`,
  `GetUserSkillTemplet`, `CX2GUUser::GetEquippedActiveSkillTemplet`; `IsEquippedSkillUpgraded` (32071e70), `IsUpgradedSkillLearned` (31fcf3e0); `GetSkillPowerRateValue` reads SA 173 for an upgraded skill;
  `GetSkillAbilityValue` is numerical-first (+ a 3-argument overload with the PvP flag). Gates: `SPEED_TIME_ULTIMATE`, `ParsingBuffFactorID`, DamageEffect ctor (owner user or NPC master, id = `APPLIED_SKILL_ID` else `_NOT_AFFECT_MEMO`).
  The other ~35 June callers of the swap are class / UI / gage code (S5, S7, character ports) and keep the default.
- **Debug (`_DEBUG`).** After the real load: (A) March's file through March's own parser into a throw-away tree, compared field by field with the real (June-parser) tree; (B) June's `june_src_fixed/.../NewSkillTempletVer2.src.lua`
  read from disk into another throw-away tree. Plus per-skill `pad` lines at load and `reach` lines when play reads a level past a short array.
- **Hygiene (`flagcheck --worktree --files` on the 9 code files):** `files: 9 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,872,256 B, dated 2026-10-18 03:21:50, md5 ce79dccff14191fdfb02268e1cd90a81, equal to X2/US_SERVICE/x2.exe (2026-10-18 03:21:45); built at main 60eac05c + 29 uncommitted code file(s).`
  (another session's build compiled this tree first; the exe holds the `[JUNE_SKILL] LOAD` strings.)
- **How to see it:** `python tools/logs.py --grep "JUNE_SKILL"` after a launch to the character screen or any town. Expected, in `offline_server.log`: `LOAD templets=521 ...`, `LOAD rules: ...`, `LOAD pad id=...` lines,
  `LOAD cmp march-parser vs june-parser ... unexpected=0`, `LOAD cmp tree rows N / N ( row diffs 0 )`, `LOAD june-file templets=2346 tree_rows=3360 guild_rows=12`.
- **Gate for the review:** `unexpected=0`; the `pad` / `reach` lines decide whether March data needs the "June's rule applied to March data" wording (amendment 8). Checks that need a class skill in play are unchanged paths (no templet has `+0x178`), so one dungeon run is enough.
- **Open for the review:** the exact June order of the `AddSkillTemplet` scalar keys relative to the upgrade-map insert is by tsv, not by the binary; the CEffect-ctor gate's owner rule was read from the disassembly (`@31f7b3f3-31f7b81f`).
- **Play-test result (2026-10-18, log, user launched twice).** First run found two defects in the debug compare only (stat floats are `KProtectedType`, compared by bytes; the `june_src_fixed` file has no BOM, which the binder requires); fixed, redeployed (`DEPLOY OK ... md5 3f645dcc4ba83fb4987a9738e5f0983e`). Second run: 0 UNHANDLED beyond the usual ignored `E_DISABLE_HB_CHECK_REQ`, 0 EXCEPTION, 0 ErrorLog lines.
  - `LOAD templets=521 tree_rows=1371 (classes=52) guild_rows=12 seal_items=4 upgrade_map=0`; `LOAD cmp march-parser vs june-parser ... both 521, only march 0, only june 0, unexpected=0; padded arrays=1, empty SA tables=0`; tree rows `1371 / 1371 (row diffs 0)`, guild 12 / 12, power-rate 5 / 5, default-skill classes 52 / 52, seal items 4 / 4.
  - The one padding difference in March data: id 900 (`SI_ETC_WS_COMMON_LOVE`) `m_SkillCoolTime` has 2 entries; no `reach` line, so play never read it past the end. March data needs no "June's rule applied to March data" wording beyond that single array.
  - June's file through June's parser: `templets=2346 guild_rows=12 upgrade_map=615 numerical_blocks=3859 extra_icons=19` (S0a: 615 upgrade rows, 19 extra-icon tables: equal). **`tree_rows=2697`, not 3360, and that is the S0a blocker, not a loader fault:** June writes the tree class as `UNIT_CLASS.UC_*`, 31 of its 86 class names are absent from March's core `Enum.lua`, they read nil -> `UC_NONE`, and 1,316 rows collapse onto 663 duplicate (class, skill) keys. Counting the file with unknown names mapped to one class gives exactly 2,697. S3 (amendment 2) adds the names to `Enum.lua`; the S3 gate re-runs this count and must read 3,360.
  - June-file short arrays (cool time / MP / required level): 1,386 / 1,259 / 1,634 with 1-29 entries: June's no-padding rule is live for them after S3 (S2 review checks which callers pass a level above the array).

### Phase S2 -- Review (2026-10-18, Opus 5.5 high)

**Verdict: S2 passes with two transcription fixes and one S3 amendment.** Commits reviewed: `c91ce6c6`, `28c47773`.
A Sonnet subagent checked the 260 rows of `s0a_templet_keys.tsv` key by key; Opus read every finding and spot-checked six functions in Ghidra.
- **Hygiene, per commit (`--range c^..c --with SERV_IRUHADEV_JUNE_SKILL_DEBUG`).**
  - `c91ce6c6`: `files: 9 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  - `28c47773`: `files: 1 PASS, 0 FAIL, 0 ERROR`; `2 deleted line(s)`, both inside `JuneCompareTrees` (`_DEBUG`, this flag's own block: the byte `memcmp` of the stat vector, replaced by a value compare); `RESULT: PASS`.
- **Keys.** 257 of 260 rows match.
  - The templet parser matches on all 239 rows (name, getter, default, store, no-fill flags).
  - The SA table matches on all 202 rows: id, key and order, with 12 -> 13 kept and `C_ASSERT`s at 142/173/202/203.
  - Read order: `m_eType` is read before `m_eID` and the duplicate test. That only sets a byte on a templet that is then deleted, so there is no effect.
  - The three mismatches: (1) the tree row's `m_iPrecedingSkill`, a documented superset that S3 removes; (2) **power-rate `m_Value` padded**, fixed below; (3) `AddSkillTypeColor`, which belongs to S7 (amendment 9).
  - "Logged once per key name": every June key is parsed and stored, so no key is left unported to log. Nothing is owed.
- **Ghidra spot-checks.** Five match the port:
  - CEffect ctor gate `@31f7b3b0-31f7b81f`: unit type 0 = the user, 1 = `32759f00` master, other types skip; APPLIED_SKILL_ID, else _NOT_AFFECT_MEMO; `31fcf3e0`.
  - NUMERICAL_DATA rows `@32218ac1-32218b8a`: one row per `m_Value` entry; pvp 1.0, tech and hit 0 when empty; side arrays clamp to their last entry.
  - Row pick `320716c0`; `GetSkillPowerRateValue 32071720`; SPEED_TIME gate `31fe3aaf` (`31fca8e0( state skill, 1 ) +0x178`).
  - Swap `328905b0`.
  - Post-process `32206050` and power-rate copy `32207430`: March's shape, so the Done note's "stays as in March" holds.
- **Fix 1 (`X2SkillTree.cpp` `JuneTryGetSkillAbilityValue`).** June `32206fb0` returns **false** with the output untouched when the SA table exists but the level is past its end (`@322070a6-322070e3`). The port returned true with 0.
  - Only `GetSkillPowerRateValue` sees the difference: an upgraded skill with a short SA 173 table now falls back to `m_vecPowerRate`, as June does. `GetSkillAbilityValue` still gives 0.
  - No effect on March data, where no templet has `+0x178`.
- **Fix 2 (`AddSkillPowerRate_LUA`).** June `32209d80` reads `m_Value` with fill flag 0 (`XOR EBX,EBX @32209e53`, pushed `@32209e8c`). The port kept March's padding; it now has `#ifdef` June / `#else` March.
  - All six June rows and all five March rows have 30 entries, so nothing changes in play. The S2 Done note's "identical" was wrong on this point.
- **S3 amendment 9 (`kickoffs/skill-system-core/S3.md`): June's `GetSkillLevel 32890620` differs beyond the swap.**
  - Guild skills read map `+0x28`: bonus total, cap 30, plus a `31d717d0` zero test.
  - Every other skill reads the acquired map `+0x4a8`: the stored level byte (0 -> 1), with no bonus, no `bBasic`, no page and no cap.
  - S0a's TU row and the buff 4b-i row described only the guild half.
  - On March data this would remove +skill-level bonuses, so it moves with the flip. On June data it is what keeps the 1-entry cool-time and MP arrays from reading 0 at bonus levels.
- **Short arrays (amendment 1).** The three getters (cool time, MP, required level) have about 90 call lines: class code, the tree UI and `X2UserSkillTree`.
  - All of them pass a level from `GetSkillLevel`. Past the end both builds read 0, so June's no-padding rule is safe only together with amendment 9.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,873,280 B, dated 2026-10-18 06:34:55, md5 6e1b71e8bdb7cbe25ec57d44d4c11aa3, equal to X2/US_SERVICE/x2.exe (2026-10-18 06:34:50); built at main 68e3302f + 6 uncommitted code file(s).` A peer's build compiled this tree first; `X2SkillTree.obj` (06:31) is newer than the last fix (06:16).
- **Play-test:** none owed. Neither fix changes March data (no upgraded templet; every power-rate row has 30 entries). The S2 play-test result stands.

## Phase S3 - The flip: June skill data and the save migration

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_REFORM`. Sub-tag `[JUNE_SKILL] MIG`.

- **Data.** `adopt-june data036 NewSkillTempletVer2.lua SkillData.lua` and the rest of S0a's closure tsv (ljconv bytecode).
  Adopt `_LA` only if S0a says the US build loads it. `/elstools pack data036`, ask, `/x2-kom-packed copy data036`.
- **Loader.** Load the files in June's order (S0a item 3); the Trans file goes if June dropped it.
- **Save migration v17** (`Migrate()` rung `iFrom < 17`, after v16):
  - Every unit's `unit_skill` rows go. The unit gets June's default skills for its class (S0a item 4).
  - SP is recomputed by June's rule for the unit's level, and CSP is refunded.
  - `unit_skill_unsealed`, `unit_skill_note` and any quick-slot reference to a removed skill follow S0a's census.
  - Log one line per unit: `[JUNE_SKILL] MIG unit <uid> L=<level> skills <n> -> <defaults>, SP <old> -> <new>`.
  - Snapshot `db_backup/els_db_<date>_pre_S3_skill_reform.sql` (with `-wal` / `-shm`) before the migrating launch, and ask
    the user first.
- **Emulator.** `Handler_EGS_GET_SKILL_REQ` / `RESET` / `INIT_SKILL_TREE` to June's rules (master level, row / column, SP), still
  echoing the request's page number (never 0). `createchar.py` / `setchar` learn June's defaults.
- **Checklist.** Add a section listing, per March class, the reform IDs on its June tree with no state in its March class
  script. Each one is a character-port row (rule 5).
- **Play-test.**
  - Every save character loads, with June's tree in the March UI.
  - SP equals June's rule.
  - Learning a skill whose state the class script has works (one per class from the census).
  - A skill with no state logs once and does not crash.
  - The `MIG` lines appear for every unit.

**Done gate:** migration lines for every unit; the snapshot path recorded; `COPY OK data036`; `DEPLOY OK`; flagcheck PASS; the
checklist section written.

**Done note (2026-10-18, Opus 5.5 high): S3 built, packed; copy, migrating launch and play-test pending.** Flags `SERV_IRUHADEV_JUNE_SKILL_REFORM` and
`..._REFORM_AUTOLEARN` (labelled stand-in), short-lived lines under `_DEBUG`; tag `[JUNE_SKILL] MIG`. User answers (2026-10-18): flip now; Q1 auto-learn
free rows; Q2 keep the base when an [Enhanced] skill is learned; Q3 no June unseals.
- **Data.** `adopt-june data036 SkillData.lua NewSkillTempletVer2.lua` (ljconv; record `june2017_re/june_adopt_20261018_064900.tsv`). Core `Enum.lua`
  (BOM source, `luasrc.py refresh`, ledger PASS): 31 June `UNIT_CLASS` names (33-46, 123-136, 200-202; the S2 count, not 39) and
  `SPRT_HYPER_CLASS = 5` with `SPRT_END` 5 -> 6. The nine `BUFF_FACTOR_ID` names are missing from **June's** Enum too: kept, `JUNE_BUGS.md` 279.
  `PACK OK data036` (242 entries; vs live: ~3 changed: Enum.lua, NewSkillTempletVer2.lua, SkillData.lua). Not adopted: `_LA`, CommandIconData (S7).
- **Loader.** No change: `OpenScriptFile` already loads SkillData.lua, then NewSkillTempletVer2.lua; the Trans load is dead (`LUA_TRANS_DEVIDE` off).
  S2's two supersets go under the flag: the tree row's `m_iPrecedingSkill` read and AddSealSkillInfo's scalar `m_iItemID` fallback (`X2SkillTree.cpp`).
- **June `GetSkillLevel 32890620` whole** (amendment 9, `X2UserSkillTree.cpp`). The guild branch is March's (the `31d717d0` test is state 0x17 / 0x18,
  June's PVE states, which March lacks). Every other skill reads the current page's map: the stored level, 0 reads 1, no bonus / bBasic / page / cap.
  A page argument other than the page in use logs once.
- **Save v17** (`X2OfflineDB::MigrateSkillsToJuneTree`, one-way; an exe without the flag refuses v17). All `unit_skill`, `unit_skill_unsealed` and
  `unit_skill_note` rows go (one log line per memo; orphan rows counted). SP = June `GetCalcInitSkillPoint(level)` (L80 604, L99 842); CSP 0.
  AUTOLEARN grants each class's 0-SP, master-level-1, not born-sealed, non-[Enhanced] rows up to the unit's level; the first 4 castable go in A
  slots. Unit 12: 20 at L85, 22 at L99; unit 39: 2. One `[JUNE_SKILL] MIG unit` line per unit. June data comes from the generated
  `X2OfflineJuneSkillAutoLearn.h` (`june2017_re/s3_autolearn_gen.py`, `--check` SAME x3; 1,347 rows, 86 classes), because Open() runs before the tree loads.
  `_DEBUG` checks it against the loaded tree (`MIG autolearn check`).
- **Emulator.** Create / job change / full reset seed June's free rows (`SeedJuneAutoLearn`); level-0 rows are neither loaded nor sent (June reads 0 as 1;
  June's `SetSkillLevelAndCSP 32897240` erases below 1); GET / RESET / INIT ACK pages clamp to >= 1; the `UNIT_CLASS` seed merges into Enum.lua's table
  instead of replacing it (it would have nil'd the 31 names for later loads). `createchar.py` (SP = 1 + June init SP) and `elsdb.py setchar` (level / class)
  grant the same rows (`tools/data/june_skill_autolearn.tsv`, `june_skill_init_sp.tsv`).
- **No-state skills** (`X2GUUser.cpp`). Every reform id lacks a March state. Types BUFF / ACTIVE / SPECIAL_ACTIVE fall into the class switch's
  `default:` (the class's first skill state, March's own behaviour). June types 14-16 hit `default: return`, which leaves a blank state that would hold
  the unit forever. Such a slot logs once (`MIG no-state skill`), and `StateChange` refuses the blank state. MP and the cool time are still spent: the
  class charges them first.
- **Checklist** 7ae (`JUNE2017_CHARACTER_PORT_CHECKLIST.md`) + census `june2017_re/s3_reform_ids_no_state.tsv`: 2,044 rows of March's 55 classes, 1,989
  with no state (only SI_ETC_WS_COMMON_LOVE has one). **New phase S3c** (unowned, planned per rule): March's engine readers of June types 14-18.
- **Side finds.** `SkillLevelUpSocketGroup.lua` / `AnimationNameTable.lua` now resolve the 31 names (both loaders safe: stored or bound-checked; the
  socket loader still drops classes >= `UC_END` 124, which March cannot play).
- **Snapshot:** `db_backup\els_db_20261018_070707_pre_S3_skill_reform.sql` (v16, taken before any v17 exe could run).
- **Hygiene** (`flagcheck JUNE_SKILL_REFORM --worktree --with JUNE_SKILL_REFORM_AUTOLEARN JUNE_SKILL_DEBUG --files` the 12 code files): 7 PASS, 3 PASS~
  (blank lines outside the flag), 1 OTHER (`X2GUUser.cpp`: a peer's R12b lines, since committed in `74dd25e6`), 0 FAIL; 0 deleted lines.
  Range `--range d483753d^..d483753d`: `files: 9 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII,
  0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Code commit `d483753d`.
- **Play-test 2026-10-18: PASSED (log, user launched once).** v16 -> v17 at 07:34:28: 27 `MIG unit` lines, 3 memo lines, `MIG done` (4 orphaned unseal rows of
  unit 17); `LOAD templets=2346 tree_rows=3360 ( classes=86 )`; `autolearn check: generated 1347 ... loaded tree 1347; only generated 0, only loaded 0,
  require-level diffs 0`; `UNIT_CLASS seed merged ... 2 added, 0 differ`. Every character loaded (27 `SKILL loaded` lines). reyaa learned 23048 / 23234 /
  23235 in the tree (GET_SKILL HANDLED, 0 SP, 842 left; in the save after exit, v17, 25 skills, bar 23200 / 23000 / 23001 / 23002). Dungeon runs as
  Lord Knight, Grand Archer, Tactical Trooper, Grand Master and Lunatic Psyker: one `no-state skill` line per blank slot, the type-6 slots of Elsword /
  Lire / Chung fell into their class's default state (no line), Elesis's and Add's type-6 slots logged as blank. 0 EXCEPTION, UNHANDLED = baseline
  (`E_DISABLE_HB_CHECK_REQ`), no crash. Not exercised: a paid learn (Extreme Heavenly Love, 2 SP). For the review: (1) S2's `_DEBUG` march-vs-june parser
  compare now runs on June's file and reports `unexpected=1586` (March's parser lacks June's SA keys): debug noise, remove with `_DEBUG`; (2) a level-1
  unit's SP goes 1 -> 0 (June init SP(1) = 0 drops the creation point; the kickoff rule, `createchar.py` gives 1 + init SP); (3) the tree UI reads
  required level 2 of 1-entry arrays (`LOAD reach ... -> 0`), June's no-padding rule as S2 planned.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,942,400 B, dated 2026-10-18 07:20:29, md5 0818bfebcaf0f51a0fdf4836d1634a0b, equal to X2/US_SERVICE/x2.exe
  (2026-10-18 07:20:24); built at main 74dd25e6 + 38 uncommitted code file(s).` A peer's build compiled this phase first; all six S3 strings are in the exe.
  `COPY OK data036: ... data036.kom 13486307 bytes, 242 entries, md5 c485fe6f654085d82dda3570ee90da86` (user OK 2026-10-18).
- **How to see it:** the first launch migrates (save v16 -> v17): `logs.py --tag JUNE_SKILL` shows 27 `MIG unit` lines, 3 memo lines, `MIG done`,
  `schema upgraded to v17`, the `UNIT_CLASS seed merged` line, and (debug) `MIG autolearn check ... only generated 0, only loaded 0, require-level diffs 0`.
  In game, the skill tree shows June's rows; SP is June's (reyaa L99: 842).
- **Gate to read in the log:** the real load's `[JUNE_SKILL] LOAD templets=2346 tree_rows=3360` (was 2,697 before the 31 class names; S0a amendment 2).

### Phase S3 -- Review (2026-10-18, Opus 5.5 high)

**Verdict: PASS, no code change** (so no rebuild; S3's `DEPLOY OK` md5 0818bfeb and `COPY OK data036` md5 c485fe6f stand). A Sonnet subagent ran the
mechanical checks. Opus read the diff of `d483753d` and did the Ghidra spot-checks.
- **Hygiene** (`flagcheck JUNE_SKILL_REFORM --range d483753d^..d483753d --with JUNE_SKILL_REFORM_AUTOLEARN JUNE_SKILL_DEBUG`):
  `files: 9 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
  `RESULT: PASS`. The PASS~ rows are blank lines in `X2OfflineDB.cpp`, `X2OfflineSkill.cpp` and `X2OfflineSkill.h`. DEFINE is `Always.h` (+33).
  `git log -S` finds no later commit that touches the flag.
- **Closure.** The two S3 flips in `s0a_skill_data_closure.tsv` are adopted: `SkillData.lua` 4469 B sha1 `030f8761...` and `NewSkillTempletVer2.lua`
  5208709 B sha1 `7f6986ca...`. Both equal the adopt record and ljconv(June). Every other closure row is owned elsewhere: `_LA` (argv `Axeso5` only),
  `NewSkillTempletVer2Trans` (dead), `SkillNoteTemplet` (S3b), `CommandIconData` / `ComboTree` / `SkillCommandDescriptionData` / `SkillTypeColor` (S7),
  `TranscendentalClassItem` (not skill-keyed), and `SkillLevelUpSocketGroup` (already June). `ledger --check` PASS and `core --check` PASS.
- **Load order.** `OpenScriptFile` under `UPGRADE_SKILL_SYSTEM_2013` loads SkillData.lua, then the templet file. The Trans load sits behind `LUA_TRANS_DEVIDE`
  (off). This is June's US order (S0a item 3).
- **v17 rung.** `iFrom > SCHEMA_VERSION` refuses a newer save (`X2OfflineDB.cpp:844`), so a flag-off exe refuses v17. Every table S0a named is
  cleared (`unit_skill`, `_unsealed`, `_note`; orphans counted). SP = `INIT_SKILL_POINT[level]`, CSP 0. Defaults: `s3_autolearn_gen.py --check` printed SAME x3
  (1,347 rows, 86 classes), and the play-test's `autolearn check` was 0 / 0 / 0. The snapshot `els_db_20261018_070707_pre_S3_skill_reform.sql` exists and
  reads `user_version` 16.
- **SP rule settled** (Done note item 2). The live server's own reset, `GSUserGameCommon.cpp` `EGS_INIT_SKILL_TREE_REQ` ->
  `CXSLSkillTree::GetCalcInitSkillPoint` (`XSLSkillTree.cpp:821`), also returns init(level) with no creation point. So the migration and the emulator's
  reset follow the server, and `createchar.py` (1 + init) models a unit that has never been reset. All three are right; no change.
- **Pages.** All three ACKs (`Handlers_Skill.cpp:61`, `:252`, `:318`) clamp the page to >= 1. SELECT_UNIT sends no explicit page.
- **Level-0 rows.** They are filtered at load (`X2OfflineSkill.cpp:178`), in the acquired list (`:326`) and in the passive list (`:370`, `:387`). The only
  zero levels that still go out are the slot arrays (`:349` / `:352`) of an empty slot, which is correct. Stale comment: `X2OfflineSkill.h:83` still says
  "Level-0 rows go out too". It is a comment only, left for S5 (which touches that file).
- **Ghidra spot-checks (read-only), all match:**
  - `GetSkillLevel 32890620` @32890620-3289071b. The guild byte selects the branch. `31d717d0` returns 0. In the guild map, bBasic (arg 2) gives the
    level, otherwise the level plus two bonus fields, capped at 0x1e (= `MAX_LIMITED_SKILL_LEVEL` 30). Otherwise: an empty map (`+0x4b0`) gives 0; arg 4
    selects `328905b0`; a found byte `+0x10` gives max(1, byte); a missing skill gives 0.
  - `GetCalcInitSkillPoint 32205790`: the sum of `CalcLevelUpIncreaseSkillPoint(2..L)`, identical to March and to the server.
  - `SetSkillLevelAndCSP 32897240`: a level < 1 is not inserted, and an existing entry is erased (`32898f50`). This backs the level-0 filter.
- **Blank-state guard.** `StateChange` refuses only a slot state (A/B, normal/hyper) whose `StateStart`, `OnFrameMove` and `OnEventProcess` are all
  empty. Every class `SetEquippedSkillFuncBySkillSlotIndex` calls `Init()` before its switch (e.g. `X2GUElsword_SwordMan.cpp:2893`). A ported state
  always binds `OnEventProcess`, so the guard cannot refuse one. Its check sits after June's random-state rebind, which June places before every early
  return. Not June code: a March safety, judged sound.
- **UNIT_CLASS merge.** The merge fills only missing names and keeps Enum.lua's values (log: 2 added, 0 differ). Without a table it falls back to the old publish.
- **Flag combinations.** REFORM without AUTOLEARN: `SeedDefaultSkills` reads the loaded tree's default map, which June's SkillData leaves empty. Units
  then start empty, as the define's comment says.
- **Carried, not S3's:** (1) S2's `_DEBUG` parser compare reports `unexpected=1586` on June's file (March's parser lacks June's SA keys). It is debug
  noise and goes when Z turns `_DEBUG` off. (2) The tree UI reads required level index 2 of the 1-entry arrays (`LOAD reach ... -> 0`). That is June's
  no-padding rule meeting March's UI; it is noted in `kickoffs/skill-system-core/S7.md`.

## Phase S3b - June skill notes (memos)

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_NOTE`. Sub-tag `[JUNE_SKILL] NOTE`. Added 2026-10-17 by S0a
(unowned June branch). Task and gate: `kickoffs/skill-system-core/S3b.md`.

**Done note (2026-10-18, Opus 5.5 high): S3b built, packed; copy and play-test pending.** Funcmap: 16 rows tagged `(S3b)`.
- **Finding: March's skill-note manager is not in the US build.** `OnlyGlobal/Always_Global.h:39` `#undef`s `REFORM_SKILL_NOTE_UI`
  (Always.h:526 defines it first), so March's `CX2SkillNoteManager`, its Lua binding (`X2Main.cpp:5523`) and its load
  (`X2StateLoading.cpp:778`) were compiled out and `SkillNoteTemplet.lua` was never read. S0a's "March binds AddSkillNoteTemplet" held for
  the source only. June's US build has the manager, so S3b declares June's class under the flag (`X2SkillNoteManager.h`, `#error` if the
  March class is ever on too), with its own include (`X2Main.h`), binding, load and destroy beside March's.
- **June's model (RE).** A memo is a `MEMO_ID` type (1-22, `MI_ENHANCE` .. `MI_LONGSHOT`), not an item. Each class lists rows
  { memo, skill, required unit level, five values }. A row opens at the unit's level and is switched on per skill in the tree UI
  (REQ `0x554` / `0x556`). The ACKs write the user tree's `multimap<skill, memo>` (+0x458), and the unit-info refresh also merges it.
  Readers ask "the active row for this skill" (`31fdbbb0`, `31fdbcc0`).
- **Ported (flag `JUNE_SKILL_NOTE`):**
  - `CX2SkillNoteManager`: loader `32452270`; `AddSkillNoteTemplet 32453160` (March's body; June's data calls it 0 times);
    `AddNewSkillNoteTemplet 32454840` with June's parent inheritance (first job / base rows inserted at the front; the class tables
    `31eca1d0` / `32368880` / `31eca340` and the `337a1660` table read off its initializer's stores); `AddMemoIDMatchItemID 32452120` (multimap);
    `AddRemovableMemoItemID 32450f20` (set). Readers `324511a0`, `324510e0`, `32451140`, `32451250`, `32452150`, `32451ff0`, `32450f50`,
    `32451210`, `32455150`.
  - `CX2UserSkillTree`: `IsJuneMemoActive 32890f90`, `GetJuneActiveMemoList 32891e10`, `Add 328923e0`, `Remove 32891050`, `Clear 32892050`,
    `Merge 32892410`.
  - `CX2GUUser::GetJuneSkillMemoEntry` (`31fdbbb0`) is real; it was C8b's null stub. The C8b memo DamageData switch (`3202ac40`) now runs
    whenever a memo is active. `GetJuneTranscendenceMemoSkillID` (`31fdbcc0`) is new; S3d wires its caller.
- **Data.** `adopt-june data036 SkillNoteTemplet.lua` (ljconv, 86176 B; record `june2017_re/june_adopt_20261018_081934.tsv`). Core `Enum.lua`
  (BOM source, `luasrc.py refresh`, ledger + core check PASS) gains June's `MEMO_ID` verbatim, plus the 80 June `SKILL_ID` names the file uses
  that the table lacked, with June's values and no value collisions. Without them the rows read nil, and the whole file stops at `MEMO_ID`.
  `PACK OK data036` (242 entries, 13504762 bytes; vs live ~2: Enum.lua, SkillNoteTemplet.lua).
- **JUNE_BUGS 286:** a second job holds its base's rows twice (first-job list + base list, both at the front). Guarded, not June:
  `32454840` reads a missing parent's map node without an end() test; June's data never has one (`parent_missing` in the load line).
- **Routed (no owner before):**
  - **S7:** the memo UI (`328dda50`, `328e9b30`, `328ded60`, the text builders `32453420` / `324513e0`), the packets `0x554`-`0x557`
    with their ACK handlers (`31f1bdb0`, `31f1c1d0`), the emulator handlers, and the save rows. These move to S7, not S3b as the
    kickoff planned, because nothing can send the REQs before S7's UI exists.
  - **S5:** the unit-info merge (`31ed4c80`, `3220f2b0`).
  - **New S3d:** the stubbed consumers (EffectSet memo scale `321e1e60` / `321dddb0`, the switch's unported cases, `32028250`, vslot 471,
    `32024ff0` variants, CommonStateStart's `31fdbcc0` use).
- **Hygiene:** `flagcheck JUNE_SKILL_NOTE --worktree` (10 files): `files: 7 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
  `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- **Deploy:** S3b's own builds were blocked by peer guards (fixed by their owners), so the exe came from a peer's build:
  `X2_offline.exe` 19973120 B, 2026-10-18 09:18:28, md5 `d4eef83688b181e6dbdb7d59656cc4a8`. Checked: it carries both `[JUNE_SKILL] NOTE` format strings and the
  three binder names. data036: `PACK OK`, copy pending the user's OK.
- **How to see it in game:** the first loading screen writes two `[JUNE_SKILL] NOTE` lines to `offline_server.log`. Expect
  `AddNewSkillNoteTemplet=86 AddMemoIDMatchItemID=23 AddRemovableMemoItemID=7 AddSkillNoteTemplet=0`, then `classes=86`,
  `memo0_dropped=0 skill0=0 parent_missing=0 class_dup=0`. No memo can be switched on until S7, so nothing changes in play.
  No class script is ported to June yet (Stages 5 / 7), so per the kickoff the load counts are the gate.
- **Open questions:** none.
- **Play-test 2026-10-18: PASSED (log, user launched once).** `COPY OK data036` (13504762 B, md5 `57a7da4c...`, user OK); exe md5 `d4eef836`. Lines 115-116:
  `AddNewSkillNoteTemplet=86 AddMemoIDMatchItemID=23 AddRemovableMemoItemID=7 AddSkillNoteTemplet=0`, `classes=86 rows=3236 ( own=1132 inherited=2104 )
  match_pairs=23 removable_items=7 | memo0_dropped=0 skill0=0 parent_missing=0 class_dup=0`. 1132 = June's 1155 MEMO_ID uses minus the 23 match lines. 0 EXCEPTION.

### Phase S3b -- Review (2026-10-18, Opus 5.5 high)

**Verdict: PASS. No code change, no rebuild.** Code commit 53713bc9 checked against June's binary, read-only.
- **AddNewSkillNoteTemplet `32454840`** matches. Tier test `9 < class < 200`, then `99 < class` gives 2 (base classes 200-202 are tier 0).
  - Tier 2 inserts the first job's rows (`3234b390( 31eca1d0, 1, 31eca340 )`) at the front through `32455e30`, then jumps to `LAB_32454a0b`,
    which inserts the base's rows (`32368880( 31eca1d0 )`) at the front. Tier 1 runs the base insert only.
  - Row defaults match: memo 0 and skill 0 (the `-(ok) & value` masks), level -1, and five values from `334380f0` at Lua indices 4-8.
  - A row is pushed only when memo != 0. The class goes in through unique insert `32457140`.
  - June looks up the parent with `32076c90` and does not test it against end(). The port guards that case and counts it
    (`parent_missing=0` in the play-test log). That keeps behaviour identical on June's data, so it is not a finding.
- **Class tables.** The initializer stores at `3234b050-3234b37f` were re-read against `JuneClassAt`. The table is `[type][tier][line]`,
  laid out as 0x50 / 0x14 / 4 bytes from `337a1660`.
  - Tiers 1 and 2 match for types 1, 7, 8, 9, 10, 11 and 12.
  - That includes type 11's order 41 / 40 / 43 / 42 and 131 / 130 / 133 / 132, and type 10's empty line 4.
  - The `31eca340` line numbers agree with that order: 41 is line 1, 40 is line 2.
- **Readers** match.
  - `32451250` maps the skill through `32207210` and tests each filter field for equality, with 0 matching anything.
    Level is tested by equality, not as a minimum. Unlike Add, it does test the class against end().
  - `32451ff0` takes no base mapping. It accepts types 0x13 and 0x14 when `v0` or `v1` truncates (`32fe25f0`) to the argument;
    an index past the end reads 0. It returns the row's raw skill id.
  - `32455150` maps once, then `324510e0` maps again, as the port does (the double map is idempotent). It returns 0 at `324551dd` when
    the row's level is above the unit's.
  - It then requires `g_pData +0x68` and `+0x90` to be non-null, and calls `32890f90` on `+0x494` with the mapped id: 2 when active, 1 otherwise.
  - `IsJuneMemoActive`, `Add` and `Remove` take the id as given. `GetJuneActiveMemoList` maps the id.
- **`31fdbbb0` / `31fdbcc0`** match.
  - Both return null or 0 when `+0x867c` is null. Both build the filter `{ 32207210(id), 0, unit +0x38, 0 }` and test `32890f90( base, memo )`.
  - In `31fdbcc0`, the value index is `31fca8e0( id as given, 1 ) +0x178 != 0`. That is March's `IsEquippedSkillUpgraded`
    (`X2GUUser.cpp:6535`, `m_iDeleteSkillIDByUpgrade`). The type test is `0x12 < t < 0x15`.
- **The REFORM_SKILL_NOTE_UI finding is confirmed with `cl /P`** (US_SERVICE defines and includes from `X2Lib_2010.vcxproj`).
  - `KTDXLIB/OnlyGlobal/Always_Global.h:39` (the commit said `OnlyGlobal/`; it lives under `KTDXLIB/`) `#undef`s it after `Always.h:526`.
  - `X2SkillNoteManager.i` has exactly one `class CX2SkillNoteManager`, June's (`.h:108-168`).
  - `X2StateLoading.i` keeps only the flag's pair (source `:784-785`). `X2Main.i` keeps only the flag's destroy (`:2205`) and its binders (`:5528-5532`).
  - Both `[JUNE_SKILL] NOTE` success lines (`X2SkillNoteManager.cpp:290`, `:292`) survive preprocessing.
- **Hygiene:** `flagcheck JUNE_SKILL_NOTE --range 53713bc9^..53713bc9`, with no later commit touching the flag (`git log -S`):
  - `files: 7 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
  - `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
  - `RESULT: PASS`
  - The two PASS~ rows are the one blank line before each file's flag block.
- **Deploy:** none (no code change). The live exe, md5 `d4eef836`, already carries S3b; the play-test passed on it.
- **Next:** `S3d.md` checked; it is current. S3d can start now, but its play-test waits for S7.
- **Open questions:** none.

## Phase S3c - June skill types 14-18 in March's engine

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_TYPE`. Sub-tag `[JUNE_SKILL] TYPE`. Added 2026-10-18 by S3
(unowned June branch). June's `SKILL_TYPE` has 14-16 `ST_SPECIAL_ACTIVE_GRADE_1..3`, 17 `ST_CORE`, 18 `ST_CORE_PASSIVE`; March's C++ enum stops at 13,
and June's data gives most special actives type 14-16. Add the enumerators, then port June's handling in every engine reader of `m_eType`
(class GU files are checklist 7ae; UI readers S7 rewrites go to S7).

**Done gate:** a `[JUNE_SKILL] TYPE` census at load; a grade-1 special active shows its type in the tree UI and tooltip; flagcheck PASS; `DEPLOY OK`.

**Done note (2026-10-18, Opus 5.5 high): S3c built; play-test pending.** Flag `SERV_IRUHADEV_JUNE_SKILL_TYPE` (needs `_TEMPLET`); tag `[JUNE_SKILL] TYPE`.
**User decision (2026-10-18): the tree UI's type readers stay with S7**, so the gate's "tree UI and tooltip" item moved to S7 (`kickoffs/skill-system-core/S7.md`,
"Carried from S3c"). Until S7, June's special actives look and act like passives in March's tree (passive background, `[Passive]`, no drag, no right-click equip).
- **Enum** (`X2SkillTree.h`): `ST_SPECIAL_ACTIVE_GRADE_1..3` = 14-16, `ST_CORE` = 17, `ST_CORE_PASSIVE` = 18, explicit values from June `Enum.lua`.
- **Readers.** The census subagent listed ~60 March sites (none in KTDXLIB). On the June side, a capstone sweep of every call to `GetSkillTemplet 32206450` /
  type-by-id `32205ab0` followed by a type compare or a jump table found 45 functions. Every one is in `june2017_re/s3c_skill_type_readers.tsv`
  with its March reader and the action. June has **no templet of type 7, 17 or 18**: its 1,148 special actives are 14-16, and wherever June names them it groups them with 7.
  - Ported: `Handler_EGS_RESET_SKILL_ACK` clears slots for 14-16 as well (June `322112d0`, switch @32211750: 5, 6, 7, 12-16).
  - **Withdrawn after the play-test:** the `Handler_EGS_GET_SKILL_ACK` auto-slot cases. That switch is in `UPGRADE_SKILL_SYSTEM_2013`'s `#else`, dead in
    this build; the live branch auto-slots no learned skill of any type. Removed in the follow-up commit (no compiled change); June's auto-equip `328db990` is S7's.
  - No change needed: guild NOT handlers `3220a940` / `3220ad40` and the page apply step `3220b440` switch on passives only, in both. The cool-time filters
    (`328922f0`, `32894490`) single out only 12 / 13. March's HYPER-only tests (`X2Unit`, `X2Game`, `X2SocketItem`, `X2GUUser`) have no 14-16 branch in the June functions already ported.
  - Routed, not ported. Tree UI (`328e6550`, `328dcd50`, `328db2d0`, `328e9160`, `328e2900`): S7. The rest of `322112d0`: the slot loop is S5's (its kickoff
    also asks it to identify `3235a160`), and the page SP writes and UI refresh are S7's. `ResetLeftSkillCoolTimeBySkillType 32895390` matches a request
    for 7 against 7 and 14-16, but no non-class March caller asks for 7 (`X2Game.cpp:2073` asks for 13), so it went to checklist 7ae with class code. The guild UI
    readers (`32926490`, `32f189d0`, `32f1bf80`) are unreachable: June's 12 guild skills (500-511) are types 9-11.
  - Unidentified June-only readers with no March counterpart: `31fcd3d0` (a guide event by type), `32622bd0` / `32622e80` (a DamageManager rate from the
    applied skill's +0x130 array x a 14 / 15 / 16 factor). The review identifies and routes them.
- **Census** in `JuneLogLoadSummary` (`X2SkillTree.cpp`), one line per real load. From June's data, expect
  `TYPE templets per type 0-18: 4 10 3 0 0 | 35 622 0 | 408 | 1 1 10 | 1 103 | 372 467 309 | 0 0 out=0` (2,346 = S3's `LOAD templets`).
- **Not changed:** `JuneSkillCategory` (`X2GUUser.cpp`, the socket flag) already maps 14-16 by number; its comment that they "never come back" is stale since S3 and is left for its owner.
- **Hygiene** (`flagcheck JUNE_SKILL_TYPE --worktree --files` Always.h, X2SkillTree.h, X2SkillTree.cpp): `0 deleted line(s), 0 non-ASCII inserted line(s),
  0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`, `RESULT: PASS` (3 OTHER = peers' S4 / S3b lines then
  in the same files). Range line: see the follow-up commit. All S3c code is insert-only `case` labels / a census block; no `#else` needed.
- **Deploy** (R4e's build, built at HEAD with S3c `a2b20c4c` in it; the exe carries the TYPE string): `DEPLOY OK: X2_offline.exe is 19,973,120 B, dated
  2026-10-18 09:18:28, md5 d4eef83688b181e6dbdb7d59656cc4a8, equal to X2/US_SERVICE/x2.exe`. Range: `flagcheck JUNE_SKILL_TYPE --range a2b20c4c^..a2b20c4c`:
  `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token,
  0 encoding change(s)`; `RESULT: PASS`.
- **How to see it:** `python tools/logs.py --grep "JUNE_SKILL] TYPE"` shows the census above. In game, as a character with a grade-1 special active in a slot,
  reset that skill in the tree: its slot empties (before S3c the slot kept the id). Not reachable offline today: every grade special active has master
  level 1, and the emulator (as the live server) refuses to reset a slotted level-1 skill (`ERR_RESET_SKILL_04` / `_05`).
- **Play-test 2026-10-18 (log, exe d4eef836):** `[JUNE_SKILL] TYPE templets per type 0-18: 4 10 3 0 0 | 35 622 0 | 408 | 1 1 10 | 1 103 | 372 467 309 | 0 0
  out=0` = June's data exactly. reyaa learned 23050 `[Enhanced] Wind Blast` (type 15; `SKILL skill 23050: level 0 -> 1`, 0 SP) and no slot change
  followed, which is March's live behaviour (see Withdrawn). The two `MIG no-state skill` lines (23000 type 14, 23001 type 15) are S3's guard. 0 EXCEPTION,
  UNHANDLED = baseline (`E_DISABLE_HB_CHECK_REQ`). **Gate: census PASS; the reset path is code-read only; the UI item is S7's.**

### Phase S3c -- Review (2026-10-18, Opus 5.5 high): PASS, no code change, no rebuild

**Verdict: PASS.** Commits a2b20c4c + 8a842484 checked against June's binary, read-only. Net code: 33 inserted lines, nothing deleted.
- **Enumerators** match June `Enum.lua` `SKILL_TYPE` (`june_src_fixed/data036_kom/Enum.src.lua:7126-7146`): 14-16 `ST_SPECIAL_ACTIVE_GRADE_1..3`,
  17 `ST_CORE`, 18 `ST_CORE_PASSIVE`.
- **`Handler_EGS_RESET_SKILL_ACK`.** June `322112d0`'s switch @32211750: passives 1-4 and 8; slot clear for 5, 6, 7, 12, 13, 14, 15, 16.
  March's group now has the same members (`flagprobe`: `ADDED_RELATIONSHIP_SYSTEM`, `FINALITY_SKILL_SYSTEM` ON). The case is live:
  no `#if` encloses `X2SkillTree.cpp:2396-2410`.
  - The withdrawn `GET_SKILL_ACK` cases sat under `#else // UPGRADE_SKILL_SYSTEM_2013` (`:2076`, flag ON). 8a842484 removed exactly
    the 6 lines a2b20c4c added (`git diff a2b20c4c^ 8a842484`: only the RESET cases, the census and the enum remain).
  - The census block sits inside `JUNE_SKILL_TEMPLET` / `OFFLINE` / `JUNE_SKILL_TYPE`, all ON; the play-test printed it.
- **Census.** I recounted it from `june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua` (`m_eType` per templet):
  `4 10 3 0 0 35 622 0 408 1 1 10 1 103 372 467 309 0 0`, total 2,346. That equals the logged line.
- **Sweep re-run.** I added the script to the tree as `june2017_re/s3c_typesweep.py`. Its output is byte-identical to S3c's (46 lines).
  - S3c's Done note said all 45 functions were in the tsv, but the tsv had 22 rows. The review adds the other 30
    (`s3c_skill_type_readers.tsv`, now 52 rows). Every sweep line has a row; `3220a350` / `32210c70` are function-start artefacts of the
    rows `3220a940` / `322112d0`.
  - **No engine reader with a March counterpart was missed.** Three engine readers group 14-16, and March's copies already match June:
    - `3234fbd0` is the socket cool time; `32070510` (called from `31fcef00`) is the now-state category. Both map 14 to 2, 15 to 3 and
      16 to 4, as `JuneSkillCategory` (`X2GUUser.cpp:15722`) does.
    - `32039e00` (BBT 98) excludes only 13, as `GetSkillDamageRateBySlotType` does.
    - `3201ad10` uses the type only for an ability term that is June-only and 0 in March.
  - The other engine hits test only 12 / 13, or switch on a skill id or an argument rather than the type.
  - The rest are class code: the 9 `SetEquippedSkillFuncBySkillSlotIndex` blocks (7ae), and `32c5c080` Raven's `SetDamageExceptionProcess`,
    already a checklist row.
  - March's own engine `ST_SPECIAL_ACTIVE` readers: the two `X2SkillTree` handlers above, the guild NOT handlers (`:3290`, `:3528`;
    June passives only) and the emulator's `SeedDefaultSkills` (`X2OfflineSkill.cpp:725`). The last is unreachable: `default-skill classes 0 / 0` in the log.
- **The three unidentified readers, identified and routed:**
  - `31fcd3d0` is **June's random mission system**. It is a 16-byte singleton `337a66b4` (`32676150`), with dialogs `RANDOM_MISSION_UI_02/03`
    (`326791c0`). While a mission runs, a skill use counts as mission event 10 (type 6), 11 (types 7 and 14-16) or 12 (type 13).
    13 per-class skill-use functions (vtable `+0x800`) call it. No STATUS row owned it, so it goes to the quest-system planning brief
    (`kickoffs/june-backlog/stage4-plan.md`, "Carried in").
  - `32622bd0` / `32622e80` are **Ain only**. Every caller gates on unit type 12 (`UT_AIN`): `AttackResult` @32013519, `DamageReact`
    @32049af5, and `CX2GUAin` `32a76f00`. They charge a gauge from the hit's damage, with a 14 / 15 / 16 factor, and go to checklist 7ae (Stage 8).
    The tsv's "game-state 0xc gate" was this unit-type test.
- **Hygiene:** `flagcheck JUNE_SKILL_TYPE --range a2b20c4c^..a2b20c4c`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
  `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  - `--range 8a842484^..8a842484`: `files: 1 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `7 deleted line(s)`, 0 of everything else;
    `RESULT: PASS`. Of the 7 deletions, 6 are the withdrawn block and 1 is the Always.h description line, rewritten.
  - `git log -S` finds one other commit: S3's d483753d, which names the flag only in the plan doc and `S3c.md`. No other code touches it.
- **Deploy:** none (no code change). The live exe, md5 `d4eef836`, carries S3c.
- **Next:** `S3d.md` checked; it is current. **Open questions:** none.

## Phase S3d - June skill memo consumers

**Run with:** Opus 5.5 high, then an Opus review. Added 2026-10-18 by S3b (unowned: the engine readers of an active memo that earlier plans
stubbed). Task and gate: `kickoffs/skill-system-core/S3d.md`. Sub-tag `[JUNE_SKILL] MEMO` (with `SERV_IRUHADEV_JUNE_SKILL_DEBUG`).

**Done note (2026-10-18, Opus 5.5 high): S3d built and deployed; play-test waits for S7.** Five flags, one per change. Funcmap: rows tagged `(S3d)`.
- **`JUNE_SKILL_MEMO_FX`: the size memos MI_HUGE (2) and MI_LONGSHOT (22).**
  - Effect sets: user `APPLIED_SKILL_ID` lookup -> type 2 writes the templet's memo scale (`321e1e60 @321e1f51`, no reset, JUNE_BUGS 289).
  - The instance copies it (`321df230 @321df584`, 0,0,0 first), each effect takes it (`321ee600 @321eede8`).
  - `GetEffectScale` multiplies by it unless `NOT_ALLOW_MEMO` (`321dddb0 @321dde5f`). Both stubs are replaced in `#else`.
  - DamageData `+0x5d0` / `+0x5dc` (memo cases 2 / 0x16). Their one reader is the CEffect ctor (`31f7aab0 @31f7c795-31f7c8db`):
    `SetScale(s)` and the scale base, the main mesh's life x `+0x5dc`, and the attack list's CT_LINE boxes x `s`. June tests type 0: JUNE_BUGS 290.
  - `CEffect::SetScale` multiplies by the base `+0xb24` (`31f6fdc0`).
  - User attack boxes: radius x v0 before and x 1 / v0 after the timed DamageCheck (`31ff27c0` -> `32274310`), as `JuneMultiplyAttackBoxRadiusScale`.
- **`JUNE_SKILL_MEMO_DAMAGE`: the rest of switch `3202ac40`.**
  - Case 0xc sets `m_bNoDetonation` (DD `+0x4f8` = NO_DETONATION: both DamageReacts copy it to `+0x6892`).
  - Cases 0xf / 0x10 run `32089ff0` (decoded; x87 settled; JUNE_BUGS 291).
  - The tail's third multiply: DD `+0x5e4`, which GetFinalDamage applies to the level-damage power rate (`32284aa0 @32285eef`).
- **`JUNE_SKILL_MEMO_BUFF`: slot 476 `32028250`.** June passes (skill id, factor): type 0xf rescales the time finalizer, 0x10 the behaviors.
  It is now called from `SetBuffFactorToGameUnit( skill, index )` (`3229af40 @3229b036`) and `...AtLevel` (`3229b0a0 @3229b162`).
- **`JUNE_SKILL_MEMO_MP`.** GetActualMPConsume takes the transcendence alternate templet (`323511f0`; it reads **my** unit: JUNE_BUGS 292).
  It is also multiplied by vslot 471 (`31fdbe20`: types 3 / 7 / 8 give v0, else 1.0), June `31fd1840` / `3203cf60`.
- **`JUNE_SKILL_MEMO_STATE`.** CommonStateStart's state string only on a skill-slot state (slot 330 on the future state) and with the
  `31fdbcc0` skill (`32057b80 @3205841f-320584bb`).
- **Routed:**
  - **New S3e:** `322a6960` (June merged ByBuffFactorID with the creator-ID variant; user block = slot 476 with the `+0x9274` fallback + slot
    0x234), the `+0x9274` writer (June's ParsingBuffFactorID `31fe32d0`), and June's X2Game state-string timer (`32079be0` float `+0xb3c`, `32308c20`).
  - **Checklist:** `320284c0 @3202858e` (skills 31301 / 31321) and `3230c1a0 @3230c742` (summon of skill 22027, type 0xf); row 396 (`32024ff0` lists) stays.
  - **S7:** `3220d2b0` (item -> skills / memo rows, item tooltip) and `3235b070` / `3235b9e0` (memo text, under `328e9b30`).
  - Noted, not memo: June's SetScale also stores `+0xa74` / `+0xa80` (time / start particle scale). March has neither (APPLY_START_PARTICLE_SCALE parse-only).
- **What a memo does now (once S7 can activate one):** MI_HUGE bigger effect sets, attack boxes and DamageDistance. MI_LONGSHOT and the CEffect
  path are 1.0 on June's own data flow (JUNE_BUGS 290). MI_BRUTAL no detonation; MI_CHECK_AND_KILL buff rescales; MI_LIGHT / BRILLIANT / CRITICAL MP rate.
  TRANSCENDENCE: the MP templet and the state name. Without an active memo every path multiplies by 1.0 or is skipped.
- **Hygiene:** `flagcheck <each flag> --worktree --files <12 files>`: RESULT PASS x5; `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched,
  0 #else/#endif without the flag token, 0 encoding change(s)` each. OTHER rows = files that mix the five flags (and the peer's S5 hunks).
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,991,040 B, dated 2026-10-18 11:01:20, md5 ee639661bb8fe55323ac62c8d2e7068c` (built at c8b47c9b + 33 uncommitted files: it also carries the peer's in-progress S5, save v18). All eight `[JUNE_SKILL] MEMO` format strings are in it.
- **How to see it in game:** after S7 (memo activation). Each use writes a throttled `[JUNE_SKILL] MEMO <site>` line to `offline_server.log`.
- **Open questions:** none.

### Phase S3d -- Review (2026-10-18, Opus 5.5 high): PASS, no code change, no rebuild

**Verdict: PASS.** Commit 5ac08da9 was checked against June's binary, read-only. Every site in the kickoff matches. I found no transcription error.
- **Effect sets.**
  - `321e1e60`: the user branch is `+0x264 == 0` (@321e1ed2). `31fdbbb0( [ebx+0x5c] )` is called with no zero test (@321e1f51), and type 2 writes v0 from `*begin` into `+0x60..+0x68` (@321e1f8c).
    This happens before `321e1bf0` (@321e1fcf). March skips an empty value list, where June would read past the end (noted in S3d).
  - Instance `321df230`: the fldz stores come first (@321df495), then the templet copy (@321df584).
  - `321ee600`: the copy @321eede8-321eee0e comes before `321df7e0` / `321dddb0`.
  - `321dddb0`: the `+0x105` test, then `+0x108.. x scale` (@321dde5f-321dde89).
  - Defaults: EffectData `0,0,0` (the fldz from @321f4b15 is still on the stack at @321f4ba5) and templet `1,1,1` (@321f3f90).
  - March's other `GetEffectScale` caller (`X2EffectSet.cpp:1053`) sits in the RUNTIME-off `#else`, which MEMO_FX's guard excludes.
- **CEffect ctor `31f7aab0`.**
  - The skip on `[esp+0x27] != 0` (IGNORE_MI_HUGE, default 0 when GetValue fails) matches.
  - `SetScale( s,s,s )` comes before the base store `+0xb24` (@31f7c80a). Life = `+0x7dc x mesh+0x44c`, in that order.
  - The `+0x3bc` / `+0x3c0` / `+0x1fc` walk matches. For radius, March's order `s x +0x8c` matches June's.
  - ebx is 0 for the whole ctor (`xor ebx,ebx` @31f7ab46, never written again). So `cmp [eax+0x20], ebx` tests type 0 = CT_LINE (March `KTDXCollision.h`: CT_LINE 0, CT_GUARD 2).
  - March's own CollisionData layout puts `m_CollisionType` at +0x20, `m_fRadiusScale` at +0x8c and `m_bUseRadiusScaleY` at +0x94 (EVE_ELECTRA on), the offsets June uses.
  - **Order is the same.** June stores the main-effect handle `+0x1e4` (@31f7bde3, with `+0x44c` = MAIN_EFFECT_TIME @31f7bdcc) before the step (@31f7c795).
    March creates the main effect at `X2DamageEffect.cpp:1718-1760`, before the step at `:1846`. Neither ctor calls SetScale again afterwards.
  - The step's `SetScale(1,1,1)` now runs in every damage effect. That is June's behaviour, and harmless in March: mesh `m_vScale` defaults to 1 and is only set by SetScale. The template size is the separate `m_vSize`.
  - `31f6fdc0` multiplies by the base first. March's vector overload forwards to the float one.
- **Switch `3202ac40`.**
  - Tail: `fMagic`, `fPhysic`, then `+0x5e4 x s` (@3202acd9-3202acf3).
  - Case 2: `+0x5d0` (fst @3202ad07) before the DamageDistance multiply.
  - Case 0xc: `+0x4f8 = bl`, with ebx = 1 (@3202ac89). `+0x4f8` is NO_DETONATION: SetDamageDataFromLUA reads the key @3263b439 into `[ebp+0x4f8]` (@3263b4ec).
  - 0xf / 0x10: Rate `{v0,1,1}` / `{1,v0,1}`, factor 0 (@3202af87-3202afee).
  - 0x16: `+0x5dc` (@3202b0a6).
- **`32089ff0`.** The fields are `bApply == 1`, each factor in `+0x40..+0x44`, and the `32d417b0` first BFT_TIME finalizer with size > 1, then `32d41910( 1, v1 x fTime, 0 )`.
  - `32d41910` ignores its type argument and tests 1 itself, as March's `JuneSetTimeFinalizerValue` does.
  - The behaviors run over a copy. `ftol( value[0] )` (fld @3208a0f1) == 3 gives `(v1-1) x |fValue-1| + v1`, with |.| rounded to float (@3208a10a-3208a136); otherwise `v1 x fValue`.
  - The result goes to `32d41820`: value[1] of the first behavior of that type with size > 1 only. That is `JuneSetBehaviorValue` op 2, which returns after the first match.
  - March's clone (`X2BuffFactor.h:787/798`) deep-copies `BUFF_FACTOR`, so slot 476 rescales only the clone. The DamageData list is a vector of pointers in both builds.
- **Slot 476 `32028250`.** The arguments are (skill id, factor) and `-1 < id` is tested. v0 is 0 for an empty list. Type 0xf takes the time half; 0x10 the behavior half, whose x87 @3202830b-3202833f is identical to `32089ff0`'s (v0 and fld1 reloaded per entry).
  Callers: `3229af40` and `3229b0a0 @3229b14d-3229b17a` test the user (`+0x264 == 0`) and call vslot `0x770` with `templet+8` after can-apply and before `32d4d670`. March's base virtual is empty, so NPCs are unchanged.
- **MP.**
  - `31fd1840` and `3203cf60` both swap the templet for `323511f0`'s id when it is > 0 and has a templet (@3203cf92 / @3203cfa7).
  - `323511f0` is a CX2Unit method: class `+0x38`, with `31fdbcc0` doing the same walk on the user's own `+0x867c`. So `g_pData +0x68 +0x90` is my unit, and JUNE_BUGS 292 stands.
    It takes the base skill's rows, active test, the upgraded test on the original id (`32206d00`), `0x12 < type < 0x15`, and truncated value[i].
  - `31fd1840` takes the level from the alternate id (@31fd189b); March's `GetSkillLevel( pSkillTemplet->m_eID )` does too.
  - Rate = vslot `0x75c` (471) x BBT 63 (@3203cfca-3203cfeb), MP multiplied last (@3203d09c).
  - vslot 471 `31fdbe20`: types 3 / 7 / 8 give v0 (0 when empty), else fld1.
  - Phase 6b's "stays out" note at `X2GUUser.cpp:25344` meant "March has no memo list". S3b added the list, so S3d may port these reads.
- **CommonStateStart @3205841f-320584bb.** Slot `0x528` (330) is called with 1. `3208e430`: arg 1 reads `+0x7cb0`, the future state data (funcmap, Buff 16-step3b), and byte `+0x11`.
  Then `31fdbcc0( slot 329 )`, or slot 329 when that is 0; `32206450( id, 1 )`, name `+0x14`, `32079be0( name, 0.f )`.
  `GetFutureStateID()` (`m_FutureStateData.stateID`) is March's twin of June's future pointer. June asks for the future side explicitly.
- **Timed attack `31ff27c0`.**
  - Scale 1.0 (@31ff2a06), slot 329, memo type 2. Empty values give 0 (@31ff2b27). Scale only when 0 < v0 (`fcomp`, `test ah,5; jp` @31ff2a41-31ff2a4c).
  - Then DamageCheck `3262f5a0` (@31ff2abc), and `1 / v0` (`fdivrp`) when scale > 0, hit or miss (@31ff2bf8-31ff2c4a).
  - `32274310`: `|s - 1| > FLT_EPSILON` (`334490f8` = 1.1920929e-07; `334355b0` = 1.0 double). It walks both sets `+0x214` / `+0x224` and every non-null box with no type test, applying the `31c1c1a0` name compare when the name is non-empty.
- **Hygiene** (`flagcheck <FLAG> --range 5ac08da9^..5ac08da9`):
  - `RESULT: PASS` x5. Each run: `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`.
  - `files:` FX `5 PASS, 7 OTHER`; DAMAGE `1 PASS, 11 OTHER`; MP `1 PASS, 11 OTHER`; BUFF and STATE `0 PASS, 12 OTHER`; 0 FAIL / 0 ERROR in all five.
  - Every OTHER row is a file that mixes the five MEMO flags (plus Always.h's defines and X2SkillNoteManager.h's SKILL_DEBUG helper).
    BUFF and STATE own no file outright: their blocks sit in X2GameUnit.cpp / X2GUUser.cpp next to the others. I read those blocks in the diff (X2GameUnit.cpp:13336 / :13379, X2GUUser.cpp AdjustBuffFactorByIndex and CommonStateStart).
  - `git log -S<FLAG> 5ac08da9..HEAD`: none for the five MEMO flags. SKILL_DEBUG: e30eec8d (the S5 review, not S3d code).
- **Nits, not fixed (no behaviour change; a rebuild for them would cost every peer's queue):**
  - `JuneGetFirstFinalizer` / `JuneCopyBehaviorFactors` sit inside `JUNE_EFFECTSET_BUFF_LIST` (`X2BuffFactor.h:959`), but MEMO_DAMAGE's Always.h guard does not name that flag.
    Undefining it alone fails at compile time, not silently. Add the `#error` the next time Always.h is edited.
  - The X2GUUser.cpp comments at :25344 (BUFF_NEW_SKILL) and :25367 (SOCKET_PARITY) ("not written here: the alternate templet ... March lacks") predate S3d.
    They are stale under `JUNE_SKILL_MEMO_MP`.
- **`SERV_IRUHADEV_JUNE_SKILL_DEBUG`: keep.** It now carries the MEMO lines, and S3e uses it too. S7's play-test is the first one where a memo is active. Retire it in the S7 review, which now says so (`kickoffs/skill-system-core/S7-review.md`); its Always.h comment "remove after the S2 review" is out of date.
- **S3e kickoff** checked: it matches the routed rows (`322a6960`, `31fe32d0`, `32079be0` / `+0xb3c`).
- **User owes:** nothing for S3d. The play-test waits for S7.

## Phase S3e - The state buff-factor path and the state-string timer

**Run with:** Opus 5.5 high, then an Opus review. Added 2026-10-18 by S3d (unowned June-changed functions next to the memo consumers). Task and gate: `kickoffs/skill-system-core/S3e.md`.
Sub-tags `[JUNE_SKILL] SBUF` (state buffs, including the slot-476 call) and `SSTR` (timer), with `SERV_IRUHADEV_JUNE_SKILL_DEBUG`. Funcmap: rows tagged `(S3e)`. Revert: MODS.md "June state buff factors and the state-string timer".

**Done note (2026-10-18, Opus 5.5 high).** Two flags.
- **Correction to S3d's routing.** June did **not** merge `ByBuffFactorID` with `...AndCreatorID`. `322a6b60` (vslot 0x220) still exists, unchanged except the BTI already ported.
  `322a6960` (vslot 0x21c) gained three arguments: `( BFI, creator unit, level, adjust skill id, skip )`.
- **`JUNE_SKILL_STATE_BUFF`: `SetBuffFactorToGameUnitByBuffFactorID` (June `322a6960`, whole).**
  - Signature: March's `( BFI, level )`, then `pCreatorUnit_ = NULL, iAdjustSkillID_ = 0, bSkipAdjust_ = true`. The defaults are March's behaviour.
  - Body:
    - The factor, and can-apply on its BTI; then the levelled clone.
    - User, no skip: slot 476 with the adjust id, or `+0x9274` when it is 0 (`@322a6a47-322a6a72`). Then slot 141 with 0 for every user.
    - The creator stamp: the creator's UID / is-user, or -1 / true (`32d4d670`). Then slot 137.
  - Callers: the June scan of every `+0x21c` call is `june2017_re/s3e_bybfi_callers.tsv`. Part 1 is the engine sites; part 2 is all 231 June calls, the class map.
    - Five live engine sites pass June's skip 0: `X2GameEdit` `ApplyBuffFactor_LUA`, `X2BuffTemplet` servo nitro (0x2ea), `X2EmblemManager` EXP buff (0x9a), and `X2GameUnit` ExtraDamage 788 / 804.
      For ExtraDamage, `ebx` = 0 was proven by `cmp [unit+0x264], ebx` in `32034e50`.
    - Every other matched engine site passes skip 1, no creator and adjust 0, which are the defaults. No engine site passes a creator or an adjust id.
    - The pet aura (`X2Pet` DoCheer) has no June `+0x21c` call: June builds the factor and calls slot 137 directly. It is left as is (not in scope).
  - `SetBuffAccumulationCount`: slot 476 falls back to `+0x9274` when the index is 0 (`322a7660 @322a777b`).
- **`JUNE_SKILL_STATE_BUFF`: `ParsingBuffFactorID` (June `31fe32d0`, whole).**
  - The list is a vector at `+0x865c`, in script order with duplicates kept (March: a map, and an in-house duplicate popup that June dropped).
  - The caller's flag is June's "equipped version of the state's skill is upgraded": `31fca8e0( NowStateSkillID, swap )` with `+0x178` > 0 (`32057b80 @32057c03`, `@3205ad70`).
    With the flag, `BUFF_FACTOR_ID_AND_APPLY_TIME_ULTIMATE` is read, falling back to the base table when absent. March's `_MEMO` table is not in June.
  - `+0x9274` = the now-state skill id, then `BUFF_FACTOR_RELATIVE_SKILL_ID` (missing / -1: the now-state skill).
  - The level comes from `GetSkillLevel( id, 0, 0, upgrade swap )` (`32890620`); March had no swap. The learn check is kept.
  - Rows with time 0 apply at once with skip 0. June's frame apply `31fdf4c0` adds user vslot 0x830 (`JuneOnStateInsertBuffFactorApplied`, empty; Rose `32ad9a40` -> checklist 7ai).
  - **June's `CommonStateEnd` first applies every row still waiting, whatever its time** (`@32015284-32015304`), and S3e ports that.
    This is the one change a player can see before S7: a state's delayed self-buff now lands even if the state ends early.
- **`JUNE_SKILL_STATE_STRING_TIME`.**
  - `CX2Game::SetStateString( str, fShowTime_ = 0 )` (June `32079be0`; the timer is written first). The member `m_fJuneStateStringTime` is June's `+0xb3c` (ctor 0, `@323076e6`).
  - `OnFrameMove` counts it down and clears the string at <= 0 (`32308c20 @3230a7d1-3230a87b`). June gates it on nothing but the `+0x2d8` early return, so it sits after March's `GS_PLAY` block.
  - All June engine sets pass 0, and June's `CommonStateEnd` inlines `("", 0)`; March's calls default to 0, so they match.
  - The 3 s caller is June's static `3200ab60`, ported as `CX2GUUser::JuneShowSkillNameStateString`. Its callers are class code only (checklist 7ai), so the timer is dormant in March today.
- **Hygiene.** `flagcheck <flag> --worktree --files ...`: RESULT PASS for both. Each shows `0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`.
  The OTHER rows are files that mix the two flags, plus a peer's S6 hunks (`Always.h`, `X2Game.cpp`).
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,998,720 B, dated 2026-10-18 13:04:05, md5 f1645b0f4434d266725a25f2ad3e8533, equal to X2/US_SERVICE/x2.exe (2026-10-18 13:03:59); built at main fc09cddd + 10 uncommitted code file(s).`
  The build includes a peer's uncommitted `X2Game.cpp` S6 hunks. The first build failed in S3e's own edit (the new getter sat in a protected section; fixed to `public:` in the same block).
  All six `SBUF` / `SSTR` format strings and `BUFF_FACTOR_ID_AND_APPLY_TIME_ULTIMATE` are in the exe.
- **Bookkeeping note:** the S3e `MODS.md` section and checklist **7ai** were committed inside the peer's S6 commit `eae671fd`, which took the whole files. They are S3e's text.
- **Play-test (2026-10-18, log).**
  - Run: exe `c0952f40`, a peer's rebuild of `80e19ca1`, which contains `c6ca2d33`; the run started 14:18 and the game was still open when the log was read.
  - Check 1 PASSED: `[14:21:59.423] [JUNE_SKILL] SBUF bybfi BFI 143 level 1 -> slot 476 with skill 0 (+0x9274, the state's) (322a6a65)`.
    The console call went through June's user block with skip 0, and the fallback is the idle state's skill 0.
  - Check 2, the skill smoke test, could not run on ElesisGM. Its four equipped skills (28000 / 28200 / 28001 / 28002) are all S3 no-state reform skills (`MIG no-state skill ...`, presses refused): a hand-off error, not a defect.
    The run also visited Eve Code Empress (unit 19, skills 25200 / 25201 have states).
  - 0 EXCEPTION. 4 UNHANDLED: the older `E_DISABLE_HB_CHECK_REQ`, unrelated. No `SSTR` line, as expected.
  - User (2026-10-18): the super armor buff showed after the console call, and the Eve skill names looked normal.
- **How to see it in game:**
  - Item 1: the dev console's `g_pGameEdit:ApplyBuffFactor(143, 5)` now passes June's skip 0, so it writes `[JUNE_SKILL] SBUF bybfi BFI 143 level 1 -> slot 476 with skill <the state's> (+0x9274 ...)`, and super armor shows.
  - Item 2 cannot be triggered by a skill today. The only user script that has the table, March's Elesis `ELESIS_KNIGHT.lua` (Endurance Power, Flame Sword, the Endurance memo), keys its states on March ids
    (8217), while the save holds June's (28210). S3 refuses reform skills with no state, so the `SBUF parse` / `frame` / `state end` lines arrive with the Elesis port (as S4's `NUM use` does).
    The play-test is a smoke test: every state runs the new parser and state end.
  - Item 3: no engine caller sets a time, so the state string looks as before. An `SSTR` line appears only once a class port calls the helper.
- **Open questions:** none. Routed: the class calls' June arguments, Rose's vslot 0x830 and the 3 s helper's callers go to checklist 7ai. The NPC `ParsingBuffFactorID` / frame apply were not in scope and are not changed.

### Phase S3e -- Review (2026-10-18, Opus 5.5 high)

**Verdict: PASS. No code change, no rebuild.** Code commit c6ca2d33 checked against June's binary, read-only (Ghidra and `disasm_range.py`).
- **`322a6960`** matches. Five stack args (`RET 0x14`): BFI `[esp+0x2c]` (factor `326c32a0`), creator `+0x30`, level `+0x34` (clone `32089f60`),
  adjust `+0x38`, skip byte `+0x3c`.
  - User test `cmp [edi+0x264], 0` (GUT_USER = 0). Skip set jumps to `@322a6a74`, so slot 141 `( ptr, 0 )` runs for every user. Non-users skip both.
  - Slot 476 `@322a6a5c-322a6a72`: the first `push ecx` reserves the by-value ptr slot (overwritten with the clone), the second pushes the id
    (adjust, or `+0x9274` when 0). The port's `AdjustBuffFactorByIndex( clone, id )` passes the same two values.
  - Creator stamp: `EDX:EAX` = -1 and the bool = 1 without a creator; else vslot 21 UID and `[creator+0x264] == 0`, then `32d4d670`, slot 137.
  - `322a6b60` is unchanged: `( BFI, UID lo, UID hi, isUser, level )`, no slot 476 / 141. It is March's `...AndCreatorID`.
  - `SetBuffAccumulationCount` `322a7660 @322a776c-322a7799` has the same user test and `+0x9274` fallback.
- **The engine sites** (push order skip / adjust / level / creator / BFI) match `s3e_bybfi_callers.tsv` part 1:
  - skip 0: `3274c954` (`ebx` zeroed `@3274c8b4`), `3236997f` (`0,0,1,0,0x2ea`), `3244c599` (`0,0,1,0,0x9a`), `320392ab` (0x314 = 788) and
    `32039624` (0x324 = 804).
  - In `32034e50`, `ebx` is reassigned in other cases, but the 788 path null-tests with `cmp esi, ebx; jz` (`@3203921d`) and the 804 path with
    `cmp [eax+0x264], ebx` / `cmp esi, ebx` / `cmp ecx, ebx` (`@32039526-3203954a`). The compiler emits those only with `ebx` = 0.
  - Spot checks left at the defaults: `31feb12c` (`1,0,1,0,0xc7`), `322d6fc8` (`1,0,1,0,eax`), `32060cb8` (`1,0,level,0,BFI`), `32298ad7` (`1,ebp,1,ebp,1`).
  - No override of the virtual exists (one declaration in `X2GameUnit.h`), so the longer signature hides nothing.
- **`31fe32d0` vs the new `ParsingBuffFactorID`** matches.
  - `+0x9270` = 0 and the vector `+0x865c` cleared. With the flag, `_ULTIMATE` is opened; if it is absent or the flag is off, the base table is used.
  - Row: index 1 (int) and 2 (float), `EndTable` (`32fe9a10`), then the 0 test. The outer `EndTable` runs only when a table opened.
  - `+0x9274` is written twice: vslot 0x524 (`@31fe3551`), then, on a non-empty list, `BUFF_FACTOR_RELATIVE_SKILL_ID` (XOR string, 29 bytes),
    falling back to vslot 0x524 on a miss or -1 (SI_INVALID = -1 in March).
  - Level `32890620( id, 0, 0, 1 )` only when id != 0 (SI_NONE = 0) and `+0x867c` is set. `RELATIVE_SKILL_LEARN_CHECK` (26 bytes) with
    level < 1 calls `32079d60` on `edi = +0x865c` (an erase of the whole range) and returns.
  - Time-0 rows: slot 0x21c `( BFI, 0, max(level, 1), 0, 0 )`, then erase.
- **The caller's flag.** `@32057c03` pushes 1 and then calls vslot 0x524 with no args, so it is `31fca8e0( GetNowStateSkillID(), 1 )`; the templet
  goes to `[esp+0x34]`. `@3205ad70-3205ad82` sets byte `[esp+0x40]` when `+0x178` (`m_iDeleteSkillIDByUpgrade`) is > 0 (`jle`).
  - In between, the only writes at `+0x40` sit under pending pushes: `@3205ba4d` (one push: the base `+0x3c` bool) and `@3205d1d7` (three: the `+0x34` slot).
    `@32060693` reads the same byte, so the port's flag is right.
  - **New finding:** the same byte also goes to `32024ff0` (LoadEffectSetFromScript, `@3205ad87-3205ad90`) and picks `DAMAGE_DATA_ULTIMATE` over
    `DAMAGE_DATA` (`@3205c865-3205c883`, `31fcd280`). March still uses `APPLY_MEMO` / `m_bApplyMemo` there (`X2GUUser.cpp:16212-16221`, `:16432-16441`;
    `DAMAGE_DATA_NEXT_MEMO` `:43433`). No phase owned it -> **new Phase S3f** (the effect-set list choice itself stays checklist row 397).
- **`31fdf4c0`**: `(t < a) == (t == a)` advances, so a row applies when t <= anim `+0x214`; slot 0x21c then vslot 0x830, both with max(level, 1).
  June's anim getter `32071d80` picks `+0x8e88` only for the June-only second body (`+0x8e90`); March's `m_pXSkinAnim` is `+0x87d4`.
- **CommonStateEnd `@32015284-32015304`**: every row, no time test, front first, slots 0x21c / 0x830. Then the second-body block `@32015306-320153c5`
  (character material, C9h-4g), then `32009ae0` `@320153c7`. The port's order is right.
- **Timer.** `32079be0` stores the float `[entry+0x20]` (after the 0x1c-byte wstring, `RET 0x20`) to `+0xb3c` before the string.
  - `@3230a7d1`: `fcom` + `test ah, 5; jp` counts only when t > 0. After `t -= +0x2d0` (elapsed, stored `@32308c6b`), `test ah, 0x41; jp` clears
    only when t <= 0, through `g_pX2Game` (`+0xb3c` = 0, then `""`).
  - Only `@32308dd3` (`+0x2d8 == 1`, March's `m_bGameWait` return) branches past it; the jump `@3230a742` returns into the loop. The placement is right.
  - Ctor `@323076e6` stores 0. Helper `3200ab60`: game alive, skill tree `g_pData +0x50`, `32206450( id, 1 )`, name `+0x14`, `3.0f` (read from `3347209c`).
- **NPC (decided: no phase).** June's parser `3275ee70` = March's live `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` parser (`X2GUNPC.cpp:33668`: row parse,
  `< 0` skipped, stable sort `327c55d0`). Frame apply `32761b40` = March's cursor loop with the defaults `( BFI, 0, 1, 0, 1 )`, plus a loop over
  `+0x9038`, the absolute-monster list. Its only writer, `3277cf10`, is the known unported Buff 16-step3b gap (funcmap row added).
  June's NPC parser drops `_IN_HOUSE_`'s duplicate popup, a build-config difference, not a June change.
- **Hygiene** (`flagcheck <flag> --range c6ca2d33^..c6ca2d33`; `git log -S` shows no later commit for either flag):
  - `JUNE_SKILL_STATE_BUFF`: `files: 5 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR`.
  - `JUNE_SKILL_STATE_STRING_TIME`: `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 8 OTHER (other flags), 0 FAIL, 0 ERROR`.
  - Both: `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`;
    `RESULT: PASS`. The OTHER rows are `Always.h`, plus each flag counting the other flag's files.
- **Play-test:** already PASSED (S3e Done note, exe `c0952f40`); nothing new to read. **Open questions:** none.

## Phase S3f - CommonStateStart's upgraded-skill flag: the state's damage data and the effect-set argument

**Run with:** Opus 5.5 high, then an Opus review. Added 2026-10-18 by the S3e review (an unowned June branch next to S3e's `ParsingBuffFactorID` caller).
Task and gate: `kickoffs/skill-system-core/S3f.md`. Flag `SERV_IRUHADEV_JUNE_SKILL_STATE_ULTIMATE`, sub-tag `[JUNE_SKILL] SULT` with
`SERV_IRUHADEV_JUNE_SKILL_DEBUG`. Funcmap: rows tagged `(S3f)`.
- June's CommonStateStart `32057b80` computes "the equipped version of the now-state skill is upgraded" once (`@32057c03-32057c1d`, `@3205ad70-3205ad82`)
  and passes it to `LoadEffectSetFromScript` `32024ff0` (`@3205ad90`), to the `DAMAGE_DATA_ULTIMATE` / `DAMAGE_DATA` choice (`@3205c865-3205c883`, `31fcd280`),
  and to `ParsingBuffFactorID` (`@32060693`, ported by S3e).
- March still reads `APPLY_MEMO` into `m_bApplyMemo` and `DAMAGE_DATA_MEMO` / `DAMAGE_DATA_NEXT_MEMO` (`X2GUUser.cpp:16212-16221`, `:16432-16441`, `:43433-43442`).
- The effect-set list choice inside `32024ff0` stays checklist row 397; S3f ports the argument, the damage-data choices and the memo path's retirement.

## Phase S4 - Numerical skill data override

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL`. Sub-tag `[JUNE_SKILL] NUM`.

- Port `3263fe41`'s override at March's site before `EndTable` in `SetDamageDataFromLUA` (`X2DamageManager.cpp:605`), from
  S0a item 6.
  - The getters `320717e0` / `32078060` and the clamp `320716c0` go on the templet.
  - Replace the "unsupported" log at `X2DamageManager.cpp:268-275` under the flag.
  - The three past-the-end blocks get a `JUNE_BUGS.md` row each.
- **Every value that changes lands in `MODS.md`.** June's damage scale differs from March's, so record both values (checklist
  section 1).
- **Verification is a log self-check, because no ported class script uses it yet.** Under `SERV_IRUHADEV_JUNE_SKILL_DEBUG`,
  at load, compute the override for a fixed sample of (skill, level) pairs from `june2017_re/c1a_numerical_blocks.tsv` and log
  them against the tsv. Also log one line per real use (throttled), ready for the pilot character.

**Done gate:** self-check lines all equal the tsv; flagcheck PASS; `DEPLOY OK`.

### Phase S4 -- Done (2026-10-18, Sonnet 5.5 high)

Flag `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL` (+ short-lived `_DEBUG`), code commit `9eae42db`. Log tag `[JUNE_SKILL] NUM`. Revert and the full behaviour list: `MODS.md` "June numerical skill data override".
- **Override** (`X2DamageManager.cpp`, `JuneApplyNumericalSkillData`, called just before `EndTable` in `SetDamageDataFromLUA`, which is June's order: after the
  `DAMAGE_DATA_KEY_NAME` record, before `SetDamageExceptionProcess`, which applies the power rate). Transcribed from `3263fe41-32640054` (disassembly read in Ghidra this
  session; the `@` addresses are in the code comments):
  - gate in June's order, any miss skips the whole block: INDEX > 0 or NAME set; `APPLIED_SKILL_ID` (+0x5c4) or `DAMAGE_SKILL_ID` (+0x5cc) non-zero (USED_SKILL_ID is not a
    trigger); `g_pData` and its skill tree; the attacker's observer valid; unit type user (itself) or NPC (`GetOwnerGameUnit 32759f00`), else skip;
  - skill = APPLIED else DAMAGE; level = `GetSkillLevel( id, 0, 0, swap )` on the owner's `CX2Unit` skill tree (`32890620`, no null test), templet =
    `GetEquippedActiveSkillTemplet( id, swap )` (`31fca8e0`); skip unless level > 0 and templet;
  - row = INDEX > 0 ? `JuneGetNumericalRowByIndex` (`320717e0`; index 0 or past the count = four zeros, checked in the disassembly: `count < index` unsigned, `index <= 0`)
    : `JuneGetNumericalRowByName` (`32078060`, first `wstring::compare` match, none = zeros); both over S2's clamp `JuneGetNumericalRow` (`320716c0`);
  - writes, unconditional: PHYSIC unless `DT_MAGIC`, MAGIC unless `DT_PHYSIC` (value * 0.01f; `DT_MIX`, `DT_WEAPON_*`, `DT_BUFF` write both), `PVP_RATE`, `TECH_POINT`
    (`_ftol2` truncation = `static_cast<int>`), `HIT_ADD_MP` (assigned only when different).
- **`DAMAGE_SKILL_ID`** was not parsed in March at all (only the record column of `Elsword_DamageData.lua`). New `DamageData::m_iDamageSkillID` (+0x5cc), June getter
  `32fe9fd0` default 0 read at `32632842`, reset in the DamageData init. The "unsupported" log for `NUMERICAL_DATA_INDEX` / `_NAME` is replaced under the flag (original in `#else`).
- **Verification (log self-check, no class script uses the keys yet).** `CX2SkillTree::JuneNumericalSelfCheck`, `_DEBUG`, runs at skill-tree load: 40 rows of
  `june2017_re/c1a_numerical_blocks.tsv` (every 36th distinct ok (skill, index) + the 3 zero rows), level 1, tolerance 1e-4; plus a by-name cross-check where the element has a
  name. Logs `[JUNE_SKILL] NUM selfcheck: sample=40 ok=.. mismatch=.. no_templet=.. PASS`. Each distinct real use logs `[JUNE_SKILL] NUM use ...` (first 80), a refused use
  `[JUNE_SKILL] NUM skip ...` (first 40 skills). **Not yet run: the log is the gate.**
- **JUNE_BUGS 282-285.** 282-284 the three past-the-end blocks (Mortal Blow 21006 INDEX 2 of 1 element; Flick Shot 30003 INDEX 5 of 4; Wind Blade 21013 INDEX 2 of 1; element counts read from
  June's `NewSkillTempletVer2.src.lua`), kept verbatim: zero damage once the skill is learned. 285: the owner's `CX2Unit` is dereferenced unguarded (latent: only the "my slot data is null" error branch in
  `X2GUUser.cpp` clears `m_pUnit`).
- **MODS.md** records June's values: 1,913 blocks in `c1a_numerical_blocks.tsv`; no value in today's game changes (no March class script sets the keys). The two June-vs-March pairs the
  checklist cites (Aceldama 9.50 vs 2.33, Aiming Shot 14.82 vs 5.63) and the June-side `GetSkillPowerRateValue 32071720` note (an "[Enhanced]" skill's damage also carries SA `0xad`) are there too.
- **Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_SKILL_NUMERICAL --worktree --files` the 5 code files: `RESULT: PASS`. Range `--range 9eae42db^..9eae42db --with SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG`:
  `files: 3 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s),
  0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- **Deploy.** My own `build.py --deploy` was blocked twice: first by the build lock, then by a peer's uncommitted S3b edits (`SERV_IRUHADEV_JUNE_SKILL_NOTE`, `CX2SkillNoteManager` undeclared; my two
  files compiled without error in that run). The deployed exe came from the peer's build at 08:09 (`last_deploy.json`: `X2_offline.exe` 19,944,960 B, dated 2026-10-18 08:09:08, md5
  `00703adcea5d79dfdd6c61b040922363`, from `X2/US_SERVICE/x2.exe` 08:09:03, built at `7a43b7a3` + uncommitted files incl. both S4 sources). All three `[JUNE_SKILL] NUM selfcheck / use / skip` strings
  are in that exe (UTF-16 scan). No kom, no save change.
- **How to see it:** launch the offline client once and close it at character select. `python tools/logs.py --tag JUNE_SKILL --grep "NUM"` shows `NUM selfcheck: sample=40 ok=40 mismatch=0 no_templet=0 ... PASS`.
  The `NUM use` lines appear once a ported class's DamageEffect with `NUMERICAL_DATA_INDEX` hits (none today).
- **Open for the review (Opus):** the sample is 40 of 1,913 rows at level 1 only; the level clamp (`min( level - 1, count - 1 )`) is S2's and was read, not re-run on a level > 1 row here.

### Phase S4 -- Review (2026-10-18, Opus 5.5 high): PASS, no code change, no rebuild

- **Gate met.** The run of 09:50 (exe `d4eef836`, built from `eb5280c7`, which carries `9eae42db`) logged
  `[JUNE_SKILL] NUM selfcheck: sample=40 ok=40 mismatch=0 no_templet=0 ( by-name cross-check ok=19 mismatch=0 ) PASS`. The 40 sample rows in `X2SkillTree.cpp`
  were re-read against `c1a_numerical_blocks.tsv` by script: 40/40 equal the tsv's level-1 columns (37 `ok` rows plus the 3 "index out of range" zero rows). All 1,913 blocks use `INDEX`; none use `NAME`.
- **Ghidra, read-only (4 functions):**
  - `3263fe41-32640054`: the gate order, the type 0 / 1 branch with the null test on the owner, and `32890620( id, 0, 0, 1 )` on `[+0x867c]+0x494` all match.
    So do `31fca8e0( id, 1 )`, `level <= 0` signed then templet null, and INDEX > 0 choosing `320717e0`, else `32078060`.
  - The writes match: PHYSIC unless `[EBP]==1`, MAGIC unless `[EBP]==0`, `+0x538`, `_ftol2` -> `+0x34`, and HIT_ADD_MP written when not equal
    (`TEST AH,0x44 / JNP`; NaN writes, as C++ `!=`).
  - `0x334436a0` read from `unpacked_x2.exe` is `0.009999999776482582` = `(double)0.01f`. A float x float product is exact in double, so `value * 0.01f` rounds the same as June's x87 path.
  - `320717e0`: the element count is `(end - begin) / 0x48`, `JC` count < index, `JLE` index, and element `index - 1`. All match.
  - `32078060`: the first `wstring::compare` match, else zeros. This matches `JuneFindNumericalData`.
  - `320716c0` (S4's open item, now closed): `idx = level - 1`, `JNC` -> last row (unsigned min), and an empty vector gives zeros. This matches `JuneGetNumericalRow`.
- **March side.** `GUT_USER 0` / `GUT_NPC 1` (`X2GameUnit.h:79-80`); `CX2GUNPC::GetOwnerGameUnit` returns `CX2GUUser*` (`X2GUNPC.h:3038`, live under
  `X2OPTIMIZE_UNITTYPE_BUG_FIX`); `GetSkillLevel( id, bBasic, usPage, bUpgradeSwap )` (`X2UserSkillTree.h:227`); `SKILL_BALANCE_20110728` defined unconditionally
  (`Always.h:2491`), so the PVP_RATE write compiles in. Every `SetDamageDataFromLUA` caller (DamageEffect ctor `:1521` -> `:1807`, NEXT chains by copy, NPC `:16504`,
  user `:16285`, reflect, third blade) sets `optrAttackerGameUnit` before the parse, so the override can run wherever June's does. `DamageData`'s copy is memberwise, so `m_iDamageSkillID` is copied.
- **JUNE_BUGS 282-285**: the rows agree with the tsv and the disassembly (285: no test at `3263feec`). **MODS.md**: the values are right (Aceldama 950 -> 9.50, Aiming Shot 1482 -> 14.82).
  One reason was wrong and is corrected in this commit: June's `DamageEffect.lua` / `EffectSet.lua` are live (C11a), so the keys *are* parsed today. Nothing changes in play
  because S3 refuses presses of reform skills that have no March class state (`MIG no-state skill ...` lines in the same run). The first `NUM use` line therefore waits on a character port.
- **Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_SKILL_NUMERICAL --range 9eae42db^..9eae42db --with SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG`:
  `files: 3 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s),
  0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The PASS~ is `X2DamageManager.cpp`'s 2 blank lines next to the call.
  `git log -S SERV_IRUHADEV_JUNE_SKILL_NUMERICAL 9eae42db..HEAD` touches code in no other commit.
- **`_DEBUG` stays on**, although `Always.h` says "remove after the S4 review". The `NUM use` / `skip` lines are the pilot character's evidence (plan text above), and no use has happened yet.
  Retire it in the first character port whose play-test logs a `NUM use` line (checklist row added to section 1).
- **User owes:** nothing for S4.

## Phase S5 - Slots 4+4 -> 5+5

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_SLOT_10`, defined in `Always.h` **and**
`ServerDefine.h` (Rule 2; `SECOND_HOMES`). Sub-tag `[JUNE_SKILL] SLOT`.

- **Shared structs to S0b item 1's spec:**
  - the count goes to 5, in the static member and the `:6411` macro;
  - `KUnitSkillData` and `KUserSkillPageData` Put/Get go to June's order;
  - prove the flag with `cl /P` in X2Lib **and** X2ServerProtocol (the struct's `sizeof` must agree).
- **Every site in S0b's `s0b_slot_sites.tsv`:**
  - the arrays, `index > 3` slot-B tests, `X2GUUser.h:4887-4890` state arrays, `X2OfflineSkill::SLOT_COUNT_PER_SET`;
  - June's `FindEquippedSlotIndex` and `SlotIndexToSlotType`;
  - `GetEquippedSkillSlotType` returning TC.
- **Wire in BBT 98 and BFT 16** (the dormant code): the 98 reader and the BFT 16 slot-type count see June's slot types.
- **The TC slots (4, 9) exist and stay locked until Stage 6**, as June locks them.
- **Emulator:** `ChangeSkillSlot` takes 0-9, refuses TC while locked, and keeps the B-slot rule.
- **Save migration v18:** slot 4-7 -> 5-8. Snapshot first and ask. One `MIG` line per moved row.
- **Play-test:** equip a skill in each of A1-A4 and B1-B4 and use it from each key; TC refuses; B slots survive a relog.

**Done gate:** both `cl /P` probes show the flag; flagcheck PASS (both homes); `DEPLOY OK`; the v18 lines.

### Phase S5 -- Done (2026-10-18, Sonnet 5.5 high)

Flag `SERV_IRUHADEV_JUNE_SKILL_SLOT_10` (+ short-lived `_DEBUG`), defined in `KTDXLIB/Always.h` **and** `KncWX2Server/Common/ServerDefine.h` (`SECOND_HOMES`). Log tag `[JUNE_SKILL] SLOT` / `MIG`.
Revert and the full behaviour list: `MODS.md` "June skill slots 5 + 5". Site ledger: `june2017_re/s5_slot_sites_done.tsv` (1,304 rows; the disposition is derived from each row's shape and was read
against the code, not proved line by line: the review spot-checks it).
- **Hygiene:** `flagcheck --worktree --files <25 files>`: `files: 22 PASS, 1 PASS~ (blank/marker only), 2 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `3 deleted line(s)` (the three `GAMEACTION_COMMUNITY` lines of
  `X2KeyPad.cpp`, kept verbatim in `#else`), `6 EOL-mismatched inserted line(s)` (`X2GuEve.cpp` / `X2SkillTree.cpp`, mixed-EOL files, inserts follow their anchor), `0 #else/#endif without the flag token`.
- **Probes (`cl /P`):** `june2017_re/flagprobe.py` (X2Lib): `ON SERV_IRUHADEV_JUNE_SKILL_SLOT_10`. New `june2017_re/flagprobe_sp.py` (X2ServerProtocol, X2TOOL): `ON SERV_IRUHADEV_JUNE_SKILL_SLOT_10`, `EQUIPPED_SKILL_SLOT_COUNT = 5`.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,992,064 B, dated 2026-10-18 11:12:15, md5 99b5211ec587d8c0bd2caab63493e7a9, equal to X2/US_SERVICE/x2.exe` (an earlier coalesced build, md5 `ee639661`, already carried the code without the census lines).
- **What landed**
  - *Wire* (`CommonPacket.h/.cpp`): count 5 (static member and the `#define`); `KSkillData` 7 B (`m_iJuneSkillDataExtra`, unmapped, sent 0); `KUnitSkillData` in June's Get order (`A[0..4]`, `B[0..4]`,
    slot-state vector in place of the B end date / state, passives, guild passives, notes, page bytes, `KJuneSkillExtraData` = `320dc980`, sent empty); `KUserSkillPageData` 5 + 5.
  - *Slot ids* A 0-3 + A-TC 4, B 5-8 + B-TC 9 (`SKILL_SLOT_*` enum, `GetSkillSlot`'s combined branch, gage list `i + COUNT`, `GetEquippedSkillSlotType` 0-3 A / 5-8 B / 4, 9 = type 3, `ApplyResetCoolTime` type 3).
    BBT 98's reader and BFT 16's counter need nothing more: both already take the type from `GetEquippedSkillSlotType` / a `std::map`.
  - *Slot state* `m_mapJuneSlotState` (June `+0x434`; `32892a00` / `32890ea0` / `32890ee0`), fed from the vector's first two entries; `GetEnabledSkillSlotB()` = key 0 usable; the legacy setters write key 0.
    The four `SKILL_SLOT_B_FREE` client sites (tree `.h` / `.cpp`, both tree UIs) are retired under the flag; the emulator sends key 0 = 2 (permanent), key 1 = 0.
  - *States* `USI_SPECIAL_ATTACK_5` / `_HYPER_5` (`GUSI_END + 53 / 54`) and `_SLOTB_5` / `_HYPER_5` (`+ 63 / 64`) = June's numbers; the four state arrays have 5 entries; `GetSkillSlotIndexUsed` and
    `GetSkillIDByEquipSlotID` know them.
  - *Keys* `GAMEACTION_SKILL5` / `SKILL_EX5` (F / T), Community T -> 0 (June `@31c48b2a`), `InputData` groups `*Skill5` / `*SkillEx5` fed in `KeyProcess` (swapped with the bars), chat key lock,
    `CommonSpecialAttackEventProcess` consumes an F press (index 4, no lock check: JUNE_BUGS 288) and a T press only while key 1 is usable.
  - *Emulator + save* state vector, `ChangeSkillSlot` refuses 4 / 9 (`ERR_SKILL_09`), seeds stop before the TC slot, **schema v18** (`MigrateSkillSlotsToTen`: slot 4-7 -> 5-8, a `MIG unit .. skill .. slot a -> b` line per row).
  - *Small fixes the widening forced:* `ToggleSkillSlotAB` temp buffer (was `[4]`: a stack overflow at 5), `GetSkillSlot`'s `>= 4` (would have mapped A-TC to B1), the reset-skill and page-ACK loops' decode, Eve's three slot-id loops,
    the learn-ACK auto-equip no longer lands in the empty TC slot (`GetSkillEquippedSlotIndex`), the v17 rung's bar size (`MAX_SKILL_SLOT / 2` would now be 5).
- **Decisions the plan did not fix (judgement calls, say if you disagree)**
  - **Pressed-index space stays March's** (A 0-3, B 4-7) in `CommonSpecialAttackEventProcess` / `CountUsedSkill` / `GetPressedSkillState` / `CurrentSkillKeyManager`: about 35 class sites read it as `> 3` / `-4`, and S0b put class files in the
    checklist. Slot ids, wire, DB and gage use June's numbering. Conversion at class-port time: checklist **7ag**.
  - **The HUD and both tree dialogs keep 4 + 4 controls** (their loops are bounded 8 / 4; `GetPicture( 4 )` would be NULL). S6 builds the 10-slot dialog. The key-config dialog (`X2KeyPad` `UpdateUI`, `X2StateOption`) has no icons for
    the new actions: they take the default branch (S6).
  - **TC is dormant, not half-live:** unit state code skips the fifth slot (`SetEquippedSkillFunc*`, `SetEquippedSkillLevelCommon`, `ChangeEquippedSkillState`), so no class code sees a TC skill.
  - F is also the March riding key (both fire on F); only riding acts because TC is empty.
  - `3235a160` (called after the SkillDown sound in June's reset ACK `322112d0`) is a unit stat / HP / MP recalculation, not slot work: not ported here (S7 owns the reset path). The reset loop's 0-9 decode is ported.
- **Out of scope, noted:** `KUnitInfo +0x30c` map (no consumer), `KSkillData +3` meaning, the `m_mapSkillSlotVector` option packet (page-option, not the skill slots), cash-item packets that still carry the B end date.
- **Play-test (pending; no console command reaches slots, so it is the skill window + logs).** Character `SaberT` (Elesis, Saber Knight, L70, unit 40). The save now holds the old-numbering B bar (skill 28003 Sonic Assault: Sting at slot 4,
  28004 Unlimited Blade at slot 5) and `user_version` 17, so the next launch runs the v18 rung on real rows. Snapshots: `db_backup/els_db_20261018_104349_pre_pre_S5_skill_slots.sql` (before any S5 launch) and
  `..._111246_pre_sql.sql`. Look for in `python tools/logs.py --tag JUNE_SKILL`: `MIG unit 40 skill 28003 slot 4 -> 5`, `skill 28004 slot 5 -> 6`, `MIG v18 done: 2`, `SLOT sent unit 40: A 28000/28200/28001/28002/0 B 28003/28004/0/0/0 ... key 0 = 2, key 1 = 0`,
  `SLOT census select unit: ... key0 state 2 usable 1, key1 state 0 usable 0, enabledB 1, slots per bar 5`; after moving a skill into B3 and a relog the census shows it in B[2].

### Phase S5 -- Review (2026-10-18, Opus 5.5 high): PASS with one fix (Eve slot id)

- **Hygiene** (`flagcheck SERV_IRUHADEV_JUNE_SKILL_SLOT_10 --range dc4e0c0c^..dc4e0c0c --with SERV_IRUHADEV_JUNE_SKILL_SLOT_10_DEBUG`; the later S5 commits touch only STATUS.md):
  `files: 22 PASS, 1 PASS~ (blank/marker only), 2 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 3 deleted line(s), 0 non-ASCII inserted line(s), 6 EOL-mismatched inserted line(s),
  0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. The 3 deleted lines are the blank line above `GAMEACTION_COMMUNITY` in `X2KeyPad.cpp`'s three default maps, re-inserted after the new F / T lines; `= KBI_T` itself is untouched in `#else`; PASS~ = 2 blank lines in
  `X2OfflineDB.cpp`. (The kickoff's `--with SERV_IRUHADEV_JUNE_SKILL_DEBUG` names the wrong debug flag: with it, the `_SLOT_10_DEBUG` blocks in `X2StateBeginning.cpp` / `X2UserSkillTree.cpp` show as OTHER.)
- **Wire** = S0b's spec. Ghidra `320e3670`: A[0..4] (+0, +7 inline, then `320ef0e0` x8 to +0x3f), slot-state vector `32138800` +0x46, two `vector<KSkillData>` +0x56 / +0x66, `vector<int>` +0x76, UCHAR +0x86 / +0x87,
  `32124f30` +0x88: the same order as `CommonPacket.cpp`'s Put and Get. `320ef0e0` = short, UCHAR, int (7 B). `KUserSkillPageData` 5 + 5. Both `cl /P` probes re-run: X2Lib `ON` (and `_DEBUG` `ON`), X2ServerProtocol
  `ON`, `EQUIPPED_SKILL_SLOT_COUNT = 5`.
- **Slot numbering, locks, keys** match June: `3214f3d0` (0-3 -> 1, 5-8 -> 2, 4 / 9 -> 3) = `GetEquippedSkillSlotType`; `3288fff0` scans A[i] then B[i] per index, as March's loop does; `3288fe30` (> 4 is B,
  minus 5) = `GetSkillSlot`'s combined branch; `32890ee0` (state 1 or 2) = `IsJuneSlotUsable`; `32892a00` (insert with the `2000-01-01` default, else overwrite) = `SetJuneSlotState`; `31fcadd0` reads F with no lock and
  T through `32890ee0( 1 )` (JUNE_BUGS 288). June also sets the pressed index to 4 / 9 there; the port does not (pressed space stays March's, checklist 7ag), which changes nothing while TC is empty.
- **Sites:** all 1,304 S5 rows of `s0b_slot_sites.tsv` are in `s5_slot_sites_done.tsv` (joined on file / line / function / kind; 0 unmatched); 17 dispositions spot-checked against HEAD, all hold. 34 rows cited a
  `s5_slot_sites_done.md` that was never written; they now cite the Done note and checklist 7ag.
- **Emulator / save:** `ChangeSkillSlot` refuses 4 / 9 before any write; seeds stop before TC; the v18 rung moves `slot 4-7 -> +1` inside its own transaction from `Open()`. Snapshot
  `els_db_20261018_104349_pre_pre_S5_skill_slots.sql` is v17 and pre-S5 (the `111246` copy is already v18); the live save is v18 with unit 40's B rows at 5 / 6 / 7. The key map is not stored by the emulator
  (`Handlers_Unit.cpp:81` sends an empty map, `Handlers_Login.cpp:322` does not persist), so June's `GAMEACTION` renumbering touches no saved data: S0b's open item is closed.
- **Fix (under the same flag): `X2GuEve.cpp`.** S5 moved the three `InitEquippedActiveSkillState` encoder loops (Mega Electronball, Taser Pilum, Kugelblitz) to June's slot ids. Every reader stayed in March's
  4 + 4 space: the key switches (`case 4: oneQ` ...), `iSkillSlotIndexPressed_ = id`, the decoders `GetSkillSlotData*` (`> 3`, `-4`). Only the two `GetSlotIndexAndSlotB` readers followed, because that splitter is now 5 + 5.
  A B-bar skill was therefore mis-decoded. The fix puts the encoders back on March's split (`if( iSlot >= 8 ) break;`) and gives the two `GetSlotIndexAndSlotB` readers a March split. `flagcheck --base dc4e0c0c^ X2Lib/X2GuEve.cpp`:
  PASS, 0 deleted lines. It is latent today: S3's June data names these skills `RNSI_*` (e.g. 25207), so March's 5204 / 5220 / 5221 match no equipped skill. Checklist 7ag's Eve bullet is corrected.
  `DEPLOY OK: X2_offline.exe is 19,992,064 B, dated 2026-10-18 12:04:52, md5 09273198f5c63ddd466a01423d549e37, equal to X2/US_SERVICE/x2.exe (2026-10-18 12:04:46); built at main dc54a8b3 + 1 uncommitted code file(s).`
- **Noted, not changed:** `GetSkillSlot( int, bool )`'s guard (`> ARRAY_SIZE - 1`) rejects combined ids 5-9, so its combined branch is unreachable. That is March's own behaviour (ids 4-7 were rejected too); every caller passes
  (index, bSlotB), and it is routed to S6. `JuneBuffTestNewSkill_LUA`'s debug dump loops `GetSkillSlot( 0..7, false )`, so it never lists the B bar (debug only). The comment at `X2GUUser.cpp:25757` still says "4 + 4".
- **User owes:** an optional smoke test (below). No new behaviour is reachable.

## Phase S6 - Slot UI

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_SLOT_UI`. Sub-tag `[JUNE_SKILL] UI`.

- Adopt June's `DLG_GAME_SKILL_SLOT_NEW_10slot` (and the slot images S0b lists), and port June's slot-UI code per S0b item 4:
  the gage / cooltime per slot and the locked TC frame.
- The exe and the kom move together.
- **Play-test:** the 10-slot bar, cooldowns on all eight usable slots, the TC frame shown locked.

### Phase S6 -- Done (2026-10-18, Sonnet 5.5 high)

Flag `SERV_IRUHADEV_JUNE_SKILL_SLOT_UI` (needs `SLOT_10`, `#error` guard), `KTDXLIB/Always.h`. Log `[JUNE_SKILL] UI`. Revert and the full list: `MODS.md` "June 10-slot skill dialog". June bugs kept: `JUNE_BUGS.md` 294 / 295. Class material: checklist **7ah**.
- **Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_SKILL_SLOT_UI --worktree --files <6 code files>`: `files: 4 PASS, 1 PASS~ (1 blank line after the new helper block), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. `cl /P` probe: `ON SERV_IRUHADEV_JUNE_SKILL_SLOT_UI`.
- **Deploy:** `DEPLOY OK: X2_offline.exe is 19,995,648 B, dated 2026-10-18 12:42:23, md5 f6b4b19e3ace7cd8d7e12dfaa1d21951, equal to X2/US_SERVICE/x2.exe (2026-10-18 12:42:18); built at main f8ffdbbd + 14 uncommitted code file(s).` (the peer's S3e hunks in `Always.h` / `X2Game.cpp` / `X2GUUser.*` are built in.) Strings in the exe: the dialog name, `[JUNE_SKILL] UI slot locks`, `LOCK_SLOT_B_TC`, `Button_Buy_Guide_Slot_A_TC`.
- **Koms:** `PACK OK` + `COPY OK` (user said yes) data034 (+`DLG_GAME_SKILL_SLOT_NEW_10slot.lua`, June `ljconv`), data036 (`Enum.lua` +8 members, md5 92de31e4), data056 (+`Button24_NEW` tga/tet), data057 (+`Texture106`, `Texture92_NEW`; `Texture63_NEW` replaced by June's superset). Backups `kom_backup_tools\*.20261018_1257*`, `kom_backup_stage_june61018_1218*`, `kom_backup_s6_slot_ui\Enum.lua.before_S6`.
- **What landed:** the 10-slot dialog (5 + 5 slot controls, June ids A 0-4 / B 5-9) driven by `CX2UISkillTreeNew`: `SlotA5` / `SlotB5` data, 10-wide loops (equip data, cool time, swap offsets) and the nine `CX2Game` loops S5 left (cool time, `InitExpandSkillCoolTime`, six class slot-icon updates); June's lock rules `328dfab0` as `UpdateJuneSkillSlotLockUI` (A-TC locked until Stage 6, B-TC from slot-state key 1, B from key 0, label alpha, cool-time clear, unequip of a skill in a locked slot); TC placement `328daed0` / `328db100` (`SetOffsetPosJuneTCSlotUI`), pitch 47, type B dialog x -257; five key labels a bar; the TC buy-guide clicks (`SMUCM_..._TRANSCENDENTAL_*` = 208 / 209) log and return.
- **Judgement calls (say if you disagree):** (1) June's enum values 182 / 183 are taken by March, so the two new `SMUCM_` members sit after March's last (208 / 209). (2) The type B dialog y keeps March's `INT_WIDE_BAR` rule (-599); only June's x (-257) is taken. (3) The type B recharge particle offset 51 -> 47 is inferred from the swap pitch (June's game-side cool-time function is not mapped). (4) `Texture20` / `Texture64_NEW` / `Texture65_NEW` are not replaced (every key the dialog names exists; June's `Texture64_NEW` drops `EXP`): asset port. (5) The `SetSkillUIType` tail that moves a `CX2Game` dialog (`+0x8f70`, `31fe2b10`) is not ported (unmapped June HUD element). (6) `STR_ID_39713` / `39707` (TC guide texts) are defined in `StringID_def.lua`; whether the string table has text for them was not checked.
- **Not done, routed:** the options dialog's rows/icons for F / T (`GAMEACTION_SKILL5` / `_EX5`) -> new **S6b** (`kickoffs/skill-system-core/S6b.md`); class statics of the dialog -> checklist 7ah.
- **Play-test PASSED (2026-10-18, log + user):** `UI slot dialog ...: 10 of 10 slot controls found`, `UI slot locks: A-TC locked, B-TC locked ( key 1 state 0 ), B open ( key 0 state 2 )`, two clicks on the B-TC lock logged msg 209, 0 EXCEPTION, clean exit (the 2 `E_DISABLE_HB_CHECK_REQ` UNHANDLED lines are older and unrelated). The run was a village visit: the dungeon cool-time check on all eight slots was not logged, the user saw the bar and said good. Expected lines were: `[JUNE_SKILL] UI slot dialog DLG_GAME_SKILL_SLOT_NEW_10slot: 10 of 10 slot controls found` and `[JUNE_SKILL] UI slot locks: A-TC locked ..., B-TC locked ( key 1 state 0 ), B open ( key 0 state 2 )` in `python tools/logs.py --tag JUNE_SKILL`.

### Phase S6 -- Review (2026-10-18, Opus 5.5 high): PASS with one repository fix (no code change, no rebuild)

- **Finding (fixed): the S6 commit put its `X2Game.cpp` hunks in the wrong places, and its Done note too.** `eae671fd` was staged as HEAD + S6 hunks at the working tree's line numbers, which the peer's then-uncommitted S3e insertions (19 lines at `:259` / `:3843`) had moved. All 20 `X2Game.cpp` runs landed about 19 lines low, for example the `for i<10` header inside `if( true == bSlotB )` (`UpdateElswordSkillSlotEffect`) and the `UpdateSkillCoolTime` `for i<5` above the type-B check, and the Done note landed under "Play-test hand-off". The **working tree was right all along**, so exe `f6b4b19e` and every later build compiled the correct placement; only the repository was wrong, and S3e `c6ca2d33` committed on top of it. Fix: commit the working-tree `X2Game.cpp` (HEAD vs worktree = the same 61 lines moved, multiset-equal, nothing else) and move the Done note under S6. The tree is unchanged, so there was no rebuild. `flagcheck --range` cannot see a mis-anchored insert (the bad commit passed it), so for partial commits of a shared file, diff the staged blob against the worktree.
- **Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_SKILL_SLOT_UI --range eae671fd^..eae671fd --with SERV_IRUHADEV_JUNE_SKILL_DEBUG`: `files: 4 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. The PASS~ is the blank line after the helper block in `X2UISkillTreeNew.cpp`. The fix commit's own range result is in its commit message.
- **DLG byte-exact:** `adopt-june data034 DLG_GAME_SKILL_SLOT_NEW_10slot.lua --dry-run` gives `1 identical`, and the staged file's sha1 `e883626b` = ljconv(June), as in `june_adopt_20261018_121830.tsv`. Every control the 5-wide loops index has 5 pictures or strings in June's DLG: `Static_SKILL_SLOT_A/B`, `Static_Skill_Cool_Time(_B)`, `_Fade(_B)`, `Static_EL_SKILL_*`, `Static_RoseGlow(_B)`, `SkillHotKey(Ex)`. `LOCK` keeps 4 and is never looped. The TC buttons send `SMUCM_DISABLED_TRANSCENDENTAL_(EXPAND_)SKILL_SLOT_MOUSE_UP`, which are 208 / 209 in `Enum.lua` and `X2StateMenu.h`.
- **Ghidra (read-only), all match.** Float constants read from `unpacked_x2.exe` through `disasm_range.va_to_raw`.
  - `328dfab0`: A-TC on `CX2Unit +0x3c`, B-TC on key 1, B on key 0; `SkillHotKeyEx` in all three (JUNE_BUGS 295); `_DAT_3343777c` = 0.5; cool-time clears A 4, B 4, B 0-3; first-found unequip with `break` (JUNE_BUGS 294); slot id `+5` = `GetSlotID()` with `EQUIPPED_SKILL_SLOT_COUNT` 5; RoseGlow hidden outside a game.
  - `328db100` (type A): slots -47 / 47, statics 47, TC (-47, 47).
  - `328daed0` (type B): slots -200 / 47 swapped, 0 / -150 unswapped (null-slot deref kept as March); statics 47 or -200 plus 200; TC (-200, 48) or (0, -200); guide offset 100.
- **Subagent lead closed:** the class slot-icon loops pass the raw 0-9 index to `GetEquippedSlot( i )`. `GetUISkillTree()` is `CX2UISkillTreeNew` (`UPGRADE_SKILL_SYSTEM_2013`, `X2UIManager.h:248`), whose split S6 moved to 0-4 / 5-9, so it is correct. The old `CX2UISkillTree` is not built.
- **Flag off:** every hunk is insert-only with the S5 code in `#else` (0 deleted lines), so the 4+4 `DLG_GAME_SKILL_SLOT_NEW.lua` and 8-wide loops come back.
- **Judgement call 2, decided (user, 2026-10-18: "take -619"):** June `328e1680` puts the type B dialog at (-257, **-619**). -619 is March's own non-`INT_WIDE_BAR` value, so June dropped the wide-bar rule; S6 had kept March's -599 (`INT_WIDE_BAR` is on at `Always_Global.h:240`). `SetSkillUIType` now sets y = -619 after the `INT_WIDE_BAR` block, under the same flag. Flagcheck `--worktree`: PASS, 1 deleted line (S6's own x-line comment reworded). `DEPLOY OK: X2_offline.exe is 20,000,768 B, dated 2026-10-18 14:15:27, md5 c0952f40deaaf46db29d4537aaa20aaf, equal to X2/US_SERVICE/x2.exe (2026-10-18 14:15:22); built at main 80e19ca1 + 4 uncommitted code file(s).` (the other 3 are the peer's S6b). Judgement call 3 (the particle offset 47) stays inferred: June's game-side cool-time function is unmapped.

### Phase S6b -- Done (2026-10-18, Sonnet 5.5 high)

Flag `SERV_IRUHADEV_JUNE_SKILL_KEYCONFIG` (needs `SLOT_10`, `#error` guard), `KTDXLIB/Always.h`. Log `[JUNE_SKILL] UI keys:`. Revert: `MODS.md` "June key-config rows for the fifth skill keys". No JUNE_BUGS row (nothing kept verbatim).
- **Finding.** The dialog is `DLG_UI_KEY_SETTING.LUA` under `CX2KeyPad`, not `X2StateOption` (`MappingKeyConfigIndex` is the gamepad list). Slots, drag/drop, conflict swap and save are generic over `GAMEACTION_END`; only the icon and tooltip of the two actions were missing.
- **What landed.** `CX2UseKeySlotData::UpdateUI` / `CX2DisUseKeySlotData::UpdateUI` cases for `GAMEACTION_SKILL5` (`SKILL_A5`, string 41915) and `_EX5` (`SKILL_B5`, 43093): June `31f5be70` / `31f5f040` regions (the strings are in the live `general.ess`: "Skill Slot 5" / "Skill Slot B 5"; passed as numbers, March's `STR_ID_` enums end at 30534).
  data063 gets June's `DLG_UI_KEYSETTING.TET` (adds the two keys) + `.tga` (`stage-june`, `PACK OK`, `COPY OK data063` md5 97e47f22 with the user's yes; backups `kom_backup_stage_june\20261018_131139`, `kom_backup_tools\data063.kom.20261018_141325`).
  One log line a key-map write: `[JUNE_SKILL] UI keys: SKILL5 'F' SKILL_EX5 'T' RIDING 'G' GUILD ','`.
- **Judgement call (say if you disagree): two March defaults move.** March binds RIDING to F, so the dialog's F slot showed riding and SKILL5 could not be dragged. June (`31f5da70`) binds F to SKILL5 only and RIDING (action 40, read at `322f7fe3`) to G; it has no GUILD action (`GUILD_INFO` 45 = `,`, `ACTION1` takes B).
  Under the flag RIDING F -> G and GUILD G -> `,` in the three key-pad default maps and `CKTDIManager::SetDefaultMap`. WARP stays B. The key map is not saved offline, so each launch starts from these defaults.
- **Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_SKILL_KEYCONFIG --worktree --files X2Lib/X2KeyPad.cpp KTDXLIB/KTDIManager.cpp KTDXLIB/Always.h`: `files: 1 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 20,000,768 B, dated 2026-10-18 14:14:48, md5 ef215f60ca5ec94458f91e82a0d31ee3, equal to X2/US_SERVICE/x2.exe (2026-10-18 14:14:43); built at main 80e19ca1 + 3 uncommitted code file(s).` `SKILL_A5` and the log string are in the exe.
- **Play-test (pending).** Options (O) -> Key setting: F and T show the two icons (tooltips "Skill Slot 5" / "Skill Slot B 5"); drag one to another key, save; the in-game bar's fifth labels follow. Log: `python tools/logs.py --tag JUNE_SKILL`.

## Phase S7 - Skill tree UI and learn logic

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_TREE_UI`. Sub-tag `[JUNE_SKILL] UI`.

- June's tree dialogs and the tree UI class per S0b item 4, function by function, including the learn / reset / purchase windows
  and the client side of learn.
- The page packets get June's handlers or `X2OfflineIgnore` rows (S0b item 3).
- Settle `SKILLTREE_NO_LOCK` / `SKILL_SLOT_B_FREE` as the user decided in S0b.
- **Play-test:** open the tree, learn, level, reset and re-learn on two classes; the page UI as June has it.

### Phase S7 - Done (2026-10-18, Opus 5.5 high)

**Split (user, 2026-10-18).** June has no SP learning, so the user chose June's model: S7 = the learn path, the tree UI = new **S7b**
(`kickoffs/skill-system-core/S7b.md` carries every UI item). Flag `SERV_IRUHADEV_JUNE_SKILL_ACQUIRE` (`KTDXLIB/Always.h` + `ServerDefine.h`,
`SECOND_HOMES`; `#error` guards `_SLOT_10`, `_SKILL_TEMPLET`, `_SKILL_REFORM`, `_SKILL_TYPE`, `_SOCKET_PARITY`). Log `[JUNE_SKILL] ACQ`. Revert: `MODS.md`
"June's learn path". Funcmap rows `(S7)` (14). JUNE_BUGS 296-297.
- **Finding (RE).** June's ids 0x11c-0x12d are March's contiguous block (`RESET_SKILL_ACK` 0x128, `UNSEAL_SKILL_NOT` 0x12d). A byte scan of every
  `push` / `mov` of 0x11c (GET_SKILL_REQ) finds string ids and sizes, no send; the one-skill reset sender `3220df10` has no call, jump or data ref.
  June tree rows have no SP key. Skills arrive from the server: a June-only NOT `{ UID, vector<KSkillData>, vector<3 int> }` (handlers `323b0770` /
  `324631a0` / `32535be0`; its id is not recoverable). The only reset is the item-driven `INIT_SKILL_TREE_REQ` (`3220e000`); its ACK (`3220ea50`)
  empties the acquired map (June has no default skills). June's `328924c0` erases a base when its "[Enhanced]" skill arrives: that supersedes S0a's
  "keep the base" stand-in (Q2).
- **Client.** `EGS_JUNE_ACQUIRE_SKILL_NOT` (end of the client event block) + `KEGS_JUNE_ACQUIRE_SKILL_NOT` (`ClientPacket.h/.cpp`, the unread
  second vector is not carried). One handler in `CX2SkillTree::UIServerEventProc` (reached from every state) instead of June's three state copies:
  `AddJuneAcquiredSkills` (`328924c0`), tree redraw when the skill window is shown (`32535be0`), charinfo `ResetStat`. The level-up handlers
  (`CX2TFieldGame`, `CX2StateMenu`) end with `JuneAutoEquipOnLevelUp` = `32206d30` + `328db990` (below level 11: castables whose required level
  is the level's multiple of 5 go into free A slots, then B; no TC). `Handler_EGS_CHANGE_SKILL_SLOT_REQ` = June's `3220e3b0` (gates + queue),
  `JuneSendChangeSkillSlotReq` = `3220e5f0`, pumped from `CX2Data::OnFrameMove` (`322105c0`).
- **Emulator - labelled stand-in (June's grant rule is server-only).** `GrantJuneSkillsByLevel`: each class tree row gets one level per
  `m_RequireCharacterLevel` entry at or below the unit's level, capped at the master level; skips no-level rows (900, the couple skill),
  `SPRT_HYPER_CLASS` (Stage 6) and unsealed-sealed rows; an upgrade zeroes its base and takes its slot. Runs before the level-up NOT, at job change,
  at character select (silent catch-up) and after a tree reset (NOT with every skill). Each run logs one `ACQ grant` line with its counts.
- **Not ported, owner named.** The rest of June's three level-up handlers (only the closing block); `CX2Unit 3235a160` (HUD refresh through the
  June-only singleton `324943c0`) -> S7b; the memo buttons `328df480` -> S7b. Pre-existing, untouched: after a tree reset the emulator re-slots
  castables (S3 seed) while the client's Reset empties the bar until relog.
- **Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_SKILL_ACQUIRE --worktree --files <21 code files>`: `files: 9 PASS, 10 PASS~ (blank/marker only), 2 DEFINE,
  0 OTHER (other flags), 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without
  the flag token, 0 encoding change(s)` / `RESULT: PASS`. `cl /P` (flagprobe): the enum, the packet struct and every new member are in X2Lib's stdafx expansion.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 20,008,960 B, dated 2026-10-18 15:00:33, md5 5fc80b2a89f842386033ef6fdb420f8c, equal to X2/US_SERVICE/x2.exe
  (2026-10-18 15:00:28); built at main 041f852d + 21 uncommitted code file(s).` All six `ACQ` strings are in the exe.
- **Save (for the play-test).** `elsdb setchar`: unit 44 arishiaaa (Aisha, class 2) L5 -> 9, EXP 31000 (L10 at 31100); unit 39 Tut14c (Elsword,
  class 1) L2 -> 4, EXP 6150 (L5 at 6200). Backups `db_backup\els_db_20261018_145226_pre_setchar*.sql`.
- **How to see it.** Select unit 44: `ACQ grant ... 1 row(s) raised` (22001 Gust Screw, required L5). Kill a monster -> L10: `ACQ push (level-up) ... 1 skill(s)`,
  `ACQ NOT`, `ACQ level-up L=10`, `ACQ auto-equip ... queued A 1`: Gust Storm (22002) appears in a free A slot. Same on unit 39 at L5 with Flame Geyser (21001).
- **Play-test PASSED 2026-10-18** (user: all checks; log, exe 5fc80b2a, 0 EXCEPTION, clean exit). Unit 44 L9 -> 10: `ACQ grant ... L=10: 1 row(s) raised` -> `ACQ push (level-up)` -> `ACQ NOT ... 1 skill(s) added` -> `ACQ level-up L=10: 1 skill(s) at band 10` -> `ACQ auto-equip ... queued A 1` -> one `CHANGE_SKILL_SLOT_REQ`/`ACK`; save: 22002 in slot 3. Unit 39 L4 -> 5: the same chain, 21001 in slot 2. Unit 19 (Code Empress, L80) select: `9 row(s) raised, 5 base(s) replaced; skipped 1 no-level, 7 hyper-class, 9 sealed`. Unit 44's select raised 0 rows, not the 1 predicted: 22001 was already in the save when this run started (3 rows at the 15:09:21 list load, so an earlier launch's select had granted it). The 4 UNHANDLED `E_DISABLE_HB_CHECK_REQ` are outside this phase.

### Phase S7 -- Review (2026-10-18, Opus 5.5 high)

**PASS. No code change, no rebuild** (exe stays 5fc80b2a).
- **Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_SKILL_ACQUIRE --range a5f970d2^..HEAD`: `files: 9 PASS, 10 PASS~ (blank/marker only), 2 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` /
  `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` /
  `RESULT: PASS`. Defines `Always.h:8582` + `ServerDefine.h:4692`. No OTHER rows, no deleted lines.
- **Finding re-checked.** Ghidra `xrefs_to 3220df10`: none (no code or data reference); the push-scan of 0x11c is not re-run (the dead sender and
  the absent SP key agree with S0a / S0b).
- **Ghidra vs the port (all match).** `328924c0`: found -> level byte at node +0x10; else, only with the skill tree loaded (`g_pData +0x50`), erase
  templet +0x178's row, then insert with the level byte. `32206d30`: class tree map, `+0x174 == 5` skipped unless unit +0x3c, `(level / 5) * 5` in
  +0x150. `328db990`: A list (no TC), B list only if `32890ee0(0)`; the doubled level test is June's own (`32890620( id, 0, 0, 1 )` when a unit is
  selected, then `32890a30` = the same call > 0); types 5 / 6 / 7 / 0xe-0x10; A first then B + 5; `Skill_Equip.ogg` per equip. `3220e3b0`: 1 s timing,
  `+0x191` -> 1, game alive + my unit + `3221c560` false + vslot 0x20c > 10 -> 0, B 5-8 key 0 / slot 9 key 1 unless skill 0, dedup -> 1, push.
  `3220e5f0`: 0x11f waiting -> 0, REQ 0x11e, AddServerPacket 0x11f, +0x90 = 1, my unit +0xde8 = `_DAT_3344a51c`. `322105c0`: send front, pop
  only on true. `32535be0`: UID test vs my unit +0x28/+0x2c, `328924c0`, then (UI manager +0x2cc, tree UI) memo buttons + `328e7b70( 0, 1, 0 )` if
  UI 0x15 shown, charinfo `3214ef80` if 0x13 shown.
- **`UpdateUI` arguments.** June's `328e7b70` is `( bEquipped, bSkillTree, bSkillPage )` with no SP argument (funcmap), so June's `( 0, 1, 0 )` is a
  tree redraw. March's `UpdateUI( false, true, true )` = that redraw plus March's `UpdateSPInfo` (bSP): a correct mapping while March's SP box
  exists. Routed to S7b (June's `UpdateUI` has no SP step).
- **Stand-in.** `GrantJuneSkillsByLevel` logs one `ACQ grant` line per run with the rule in the text ("stand-in rule: one level per
  m_RequireCharacterLevel entry <= L") and the skip counts; `PushJuneSkillGrant` logs every push, including "NOT not sent". The select
  catch-up is silent only on the wire (no NOT), not in the log. Skips = Done note (no level, `SPRT_HYPER_CLASS`, sealed and not unsealed, a base whose
  upgrade is held). The base swap moves the slot (`Persist` writes `GetSlotOfSkill`), zeroes the base, and drops a base raised earlier in the same
  pass from the NOT; both map orders (base before or after its upgrade) end the same.
- **Routing.** `CX2State::UIServerEventProc` (`X2State.cpp:2110`, `NEW_SKILL_TREE` on at `Always.h:3022`) hands packets to the skill tree; every state
  override chains to it except `CX2StateJoin` (no unit). **Level-up sites:** `CX2StateField`'s override (`X2StateField.cpp:2029`) forwards to
  `CX2TFieldGame`'s (patched); `CX2StateBattleField` / `CX2StateDungeonGame` inherit `CX2StateMenu`'s (patched, inside its
  `Handler_EGS_CHAR_LEVEL_UP_NOT` at `:4871`). Every March level-up path runs the block. June's `325346e0` sits in one vtable (`334a4398`), `32473f70`
  in 13 (the base), `323ae130` is called once.
- **Nits, no change.** The Done note did not name `CX2StateField`'s forwarder (above). `SERV_IRUHADEV_JUNE_SKILL_DEBUG` retires in the S7b review now
  (`S7b-review.md`), not here.
- **User owes:** nothing for S7. Next: `S7b` (or `S8`).

## Phase S7b - June's skill tree UI

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_TREE_UI`. Sub-tag `[JUNE_SKILL] UI`. Split out of S7 (user,
2026-10-18): `CX2UISkillTreeNew2015` function by function (`june2017_re/s0b_ui_funcmap.tsv`), the memo popups and memo packets (S3b carry), the page
packets and page UI, `SKILLTREE_NO_LOCK` retired, and the user's call on March's learn / SP UI path. Kickoff `kickoffs/skill-system-core/S7b.md`.
**Play-test:** the tree on two classes, skill types (S3c), memo select + activation (S3d's first play-test), a reset with the item, the page UI.

### Phase S7b - Done (2026-10-18, Opus 5.5 high)

**Split (user, 2026-10-18).** S7b = June's tree class; memo -> new **S7c**, pages / reset item / HUD refresh -> **S7d**, and the June-only major-skill and command panels
(`SkillCommandDescriptionData.lua`, `ComboTree.lua`) -> **S7e** (unowned-branch rule). **SP path (user):** removed with the June tree: no learn / upgrade / init buttons, no SP box;
March's `EGS_GET_SKILL_REQ` sender and the emulator's `LearnSkills` stay compiled but unreachable. Flag `SERV_IRUHADEV_JUNE_SKILL_TREE_UI` (`Always.h`, nine `#error` guards).
Revert: `MODS.md` "June's skill tree UI". Funcmap rows `Skill S7b` (38). JUNE_BUGS 298-304. Checklist 7aj.
- **Shape.** June's `CX2UISkillTreeNew2015` is `X2Lib/X2UISkillTreeNew2015.h/.cpp` under March's class name (callers of `GetUISkillTree()` unchanged; `cl /Zs` of
  X2UIManager / X2SkillTree / X2State clean). `X2UISkillTreeNew.h` includes it in its `#ifdef`; `X2UISkillTreeNew.cpp` `#include`s the June `.cpp` (no `StdAfx.h` of its own; vcxproj `None`).
  March's class is the `#else`. `SERV_IRUHADEV_SKILLTREE_NO_LOCK` retired (define gone, comment left; `MODS.md`).
- **Ported** (each `/// June <addr>`): slot data 1 / 2 (`328dcd50`, `328dbcd0` incl. the June glow picture 6, `328e18d0`, `328e1830`, `328e0460`); ctor / dtor / MsgProc / wheel / SetShow /
  SetLayer; InitSkillTreeUI (`DLG_UI_Skill_Tree_New2015.lua`); CreateSkillTree: slots by (tier, col) with the hyper-class and unsealed-upgrade rules (`328de040`), `CreateSlotUIPreset`
  (`DLG_UI_Skill_Icon_New2015.lua`, skill-type background `32207560`), level rows Lv.1/5/../70/80/90/95/100 (`328dc680`), portrait CIT_160by160 + class name (`328e8e90`); UpdateUI (March's
  4-arg signature, bSP ignored); UpdateSkillTree; new-skill marks; tabs (`328d9fb0`, `328e89f0`); acquisition dialog; right-click / drop / unequip (`328e6550`, `328d9ea0`, `328d9cf0`,
  equip check `32896c70`); ShowSkillDesc (`328e9160` + closure `328ee680`: `DLG_UI_Skill_Desc_Video.lua`, width 0x1a4, font 1 -> 0 at 0x300) and the text builders Title / Sub
  (STR 40572-40588 labels + `SkillTypeColor.lua` colours, `3220baf0`) / Effective / UnSealed (drafted by a Sonnet subagent, reviewed); UICustomEventProc's shared cases + tabs.
  The equip bar (S6) and JuneAutoEquipNewSkills (S7) are carried verbatim.
- **Skill types (S3c carry).** Types 14-16 now get the special-active background, the `[Special Active Grade n]` label, drag and right-click equip; type 13 / 12 as June.
- **Required level (S3 carry).** June's tree has no per-row level reader: the level column is the tier (`328dc680`); the tooltip's next-level preview reads `GetSkillRequireLevelValue( level + 1 )`.
- **Left for later, owner named.** Memo popups / buttons / text (`/// S7c:`), page radios still run March's page cases (`JuneSkillPageCustomEventProc`, `/// S7d:`), panels and command tab
  (`/// S7e:`), class pieces (checklist 7aj: `328e0aa0`, `328e0290`, slot types 5 / 6, Luciel portrait, Aisha memorize, transform slot). HUD refresh `3235a160` (singleton `324943c0`) is not tree
  UI: June's tree-rebuild handler `31f08290` drives it, so it goes with the reset item to S7d. June's notice branch of the acquisition popup (`31d72cc0`, g_pData `+0x290`) has no March system.
- **Data.** data034: June's 4 tree dialogs (adopt-june, bytecode); data036: `SkillTypeColor.lua` (adopt-june) + 58 June `STUCM_*` members in `Enum.lua` (ledger refreshed, check PASS);
  data057: `DLG_UI_Common_Texture_NewSkill_02` / `_03` (stage-june). PACK OK and COPY OK (user OK): data034 md5 cbbf994b, data036 f3ea3181, data057 0bbaa80a.
- **Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_SKILL_TREE_UI --worktree --files <5 files>`: `files: 4 PASS, 0 PASS~, 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` / `0 deleted line(s), 0 non-ASCII
  inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. The OTHER row: `Always.h` -1 is the retired NO_LOCK define.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 19,958,784 B, dated 2026-10-18 16:49:34, md5 e31d2624d9ed4db27338fc36ce7ac6f4, equal to X2/US_SERVICE/x2.exe (2026-10-18 16:49:29); built at main b07e3bc4 + 7 uncommitted code file(s).` The four strings ( `SkillTypeColor.lua loaded`, the two 2015 dialog names, `g_pSkillToolTipManager` ) are in the exe.
- **How to see it.** Unit 25 (Lord Knight): the 2015 tree window with Lv rows, coloured type backgrounds, no learn buttons / SP; 21000 (grade 1) shows `[Special Active ...]` in the tooltip and
  drags; the type colours load once (`[JUNE_SKILL] UI SkillTypeColor.lua loaded N colour row(s)`). Unit 41 (Electra) the same on another class.

### Phase S7b - Play-test fixes (2026-10-18, Opus 5.5 high)

**First play-test (17:25 run, exe e31d2624):** layout, type labels, second class and tabs passed; equip FAILED, the background looked stacked, the TC slot looked wrong,
the tooltip showed `@1%` / `@2%`. Log: every drop was undone 50 ms later (`SKILL slot N <- skill X`, then `slot N <- skill 0`), and unit 41's four equipped skills were all
taken off on field entry (the save was changed; she needs re-equipping). 0 EXCEPTION, clean exit, `SkillTypeColor.lua loaded 16 colour row(s)`.
- **Equip (cause of both).** `328914b0` is the **acquired-map** check `(id, bUpgradeSwap)`, not IsSkillUnsealed (the unsealed set is `32890a00`, user tree `+0x38`). The equipped
  slot's refresh (`328e1d38`) takes off a skill that is not *acquired*; S7b's port tested March's unsealed set, so every refresh unequipped. All four call sites now use June's test:
  `JuneIsSkillAcquired` (= `GetUserSkillTemplet( id, swap )`, the same find); the tree lock (`328dd042`) and the desc unseal line (`328e96b1`) keep June's order. Also June passes the
  upgrade swap at `32890a30`, `328e1caf` and `328e6602`, which S7b dropped. Funcmap rows "Skill S7b play-test fix".
- **`@1` / `@2`.** June's effect text (`328ee280`) fills `@N` from the templet's NUMERICAL_DATA rows (`32215e80` + `32214fc0`: `@101+i` = value * pvp rate, `@1+i` = value; letters
  I / L / S / e / f / h / i); March's getter has only the `GetSkillEffectiveDescExpand` special cases. Ported whole as `JuneGetSkillEffectiveDescValue` (the 4 calls of `328e5e60`).
  JUNE_BUGS 305-308. The other caller, `32929770` (from `3292a640`), is a June UI no phase owns yet.
- **Textures.** The tree's page radios and cash button name `NewSkill_01` pieces March's sheet lacks (`BT_1..3_*`, `BT_CASHBUY_*`); a missing piece draws the wrong area. A census of all 72
  (texture, piece) pairs of the 5 dialogs + the C++ found only those. Staged June's copies (stage-june; every March piece kept, except `ABLECHOICE` / `BG_TITLE_RESKILL`, read only by
  March's removed learn path and `DLG_UI_Skill_Choice.lua`, which nothing loads): data057 `NewSkill_01`, `Texture02`, `Texture20`, `Texture21_NEW.tga`, `Texture65_NEW`; data056 `Button14`.
  **Held back:** `DLG_UI_Common_Texture64_NEW` (the slot bar's sheet; same coordinates for SKILL_SLOT / LOCK / Glow): June's drops `EXP`, which March's live
  `DLG_Menu_Button_NEW.lua` (SERV_RECOMMEND_LIST_EVENT is off) draws the EXP gauge with; June's menu uses `EXP_Y`. Adopting it needs June's menu dialog with it (June asset plan).
  The A-TC slot shows locked by design (transcendence is Stage 6; B-TC follows slot-state key 1 = 0).
- **Hygiene.** `flagcheck --worktree --files X2Lib/X2UISkillTreeNew2015.cpp`: `1 PASS ... 0 FAIL` / `18 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched` / `RESULT: PASS`
  (the 18 are S7b's own lines in a file that is wholly inside the flag).
- **Deploy.** `DEPLOY OK: X2_offline.exe is 19,962,368 B, dated 2026-10-18 17:47:39, md5 9f2dc90916b2449eaab6b74254b0f501, equal to X2/US_SERVICE/x2.exe (2026-10-18 17:47:33); built at main ad0d41b7 + 1 uncommitted code file(s).`
  `X2UISkillTreeNew.obj` rebuilt 17:47:23 (after the edit), `JuneReplaceSkillDescValues` in x2.map. PACK OK and COPY OK (user OK) data056 / data057 (data057 md5 0ba792cb).
- **Play-test 2 (17:52 run, exe 9f2dc909):** equip passed (9 slot changes, none undone; relog kept the bar; unit 41 kept hers), `@N` filled, 0 EXCEPTION.
  The user's screenshot still showed a doubled window: "Primary Skill" overprinted by "Primary\nCommand" (STR 35502 / 35501) and a second panel edge at y 292 / 303
  with an inner lined panel from x 107. That is June's own `Command_Back_Black` + `SkillTree_MajorCommand` (OpenScriptFile.log: only the 2015 dialogs open, no March
  tree script). June registers them in the command group +0x18 (`328e8820` -> `328ded30`, group 2) and `328e89f0` hides that group on the skill tab (`328da3c0`); S7b
  had deferred both to S7e, so the dialog's default "shown" stayed. Ported the group halves of `328e42d0` / `328e8820`, `328ded30`, `328da3c0`, +0x8c, and the +0x18
  clear in InitSkillTreeUI (`@328ed964`); the portrait's missing-control return no longer skips the panels (`@328e8f64`). S7e kickoff updated
  (also: `Command_Description` overlaps `Skill_Description`, June never hides it). flagcheck (2 files) RESULT: PASS (20 deleted = S7b's own lines).
  `DEPLOY OK: X2_offline.exe is 19,964,928 B, dated 2026-10-18 18:13:57, md5 8cc9c75eae4bc8f4465ca5d501f64526, equal to X2/US_SERVICE/x2.exe (2026-10-18 18:13:52); built at main 9be863cb + 2 uncommitted code file(s).`
- **Play-test 3 PASSED** (user, 2026-10-18; 18:15 run on exe 8cc9c75e: the 2015 tree opened, 0 EXCEPTION, clean exit). S7b is done; its review is next.

### Phase S7b -- Review (2026-10-18, Opus 5.5 high)

**Verdict: PASS with one transcription fix.** Commits `0c02e7be` .. `70c9d2e2`, flag `SERV_IRUHADEV_JUNE_SKILL_TREE_UI`.
- **Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_SKILL_TREE_UI --range 0c02e7be^..HEAD --with SERV_IRUHADEV_JUNE_SKILL_DEBUG`:
  `files: 7 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 1 OTHER (other flags), 0 FAIL, 0 ERROR` /
  `in this flag's files: 38 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
  OTHER = `Always.h` +43 -1: the -1 is the retired `#define SERV_IRUHADEV_SKILLTREE_NO_LOCK`. The 38 deleted lines are S7b's own lines in `X2UISkillTreeNew2015.cpp`, which is new and
  sits wholly inside the flag: 18 in the play-test fix `4cf5840d` (unsealed -> acquired, `@N`), 20 in `2347581f` (the portrait block rewritten around the group code).
- **Ghidra spot-checks** (read-only). `328e2900` GetSkillSubDesc: the type switch (STR 0x9e7c-0x9e8c, passive set for 1-4 / 8-11, the default's `+0x120` byte), the base-templet MP gate,
  the UT_ADD DP / Rose FP / other unit lines and the cool-time tail with the alternate templet: **match**. `328de040` CreateSkillTree: (tier, index) key, hyper-class `+0x174 == 5` needs
  unit `+0x3c`, an upgrade (`+0x178`) replaces only when `328914b0( id, 0 )`, 8 inserts (indices 1, 2, 3, 6, 7, 9, 10, 0 of the 11-dword set), max row `+0x118`, `SetIndex( 0, 0, 1, 1 )`: **match**.
  `32896c70` the equip check: both loops, SA 190 against the equipped skill's own id, the B loop's 0-4 index (JUNE_BUGS) and message 0x9b55 = 39765 match, but **a skill with no templet
  returns false in June** (`32896cd1 JZ 32896f5f: XOR AL,AL`); the port returned true. **Fixed** under the same flag (one line, `/// June @32896cd1`).
- **Re-check after the failed spot-check** (every ported function that returns a value or gates on one): `328e6550` right-click equip (gate, STR 35567, types 5-7 / 12-16, TC kind 3,
  expanded B `iFree + 5`, closed B -> slot 9 when `32890140( 0x44 )` = B slot 4 `+0x3cc` is empty), `328d9ea0` drop, `328d9cf0` unequip, `322074e0` same-or-upgrade (miss = -1 as March's
  `GetUpgradedSkillID`), `328ea4e0` MsgProc (the 9 messages, drag guide `+0x114`, layer 0x15), `328d9b60` wheel (`31d41d90` = the handle test, = `NULL == m_pDLGUISkillTree`): **match**.
  Only difference left: `328d9ea0` dereferences its own slot data without a test on the type-1 path; the port's `NULL` guard returns false there (unreachable; kept).
- **Carried code.** The S6 bar and S7's `JuneAutoEquipNewSkills`: 22 of 23 functions identical to `X2UISkillTreeNew.cpp`'s `JUNE_SKILL_SLOT_UI` branch (difflib, comments stripped);
  `SetSkillUIType` keeps a dead `vSlotPos.y = -599.f;` (the old `INT_WIDE_BAR` branch, now unguarded) that the next line overwrites with -619: same result, left as is.
- **Markers.** `/// S7c:` 6 (memo popups, the two memo text builders, the left / right memo buttons), `/// S7e:` 4 (video popup, ComboTree lists, command map, Skill_Description text),
  `/// S7d:` 0 in the code (the page cases are `JuneSkillPageCustomEventProc`, named in S7d's kickoff), checklist 5 (`328e0aa0`, `328e0290`, Luciel group, `+0x170`, Aisha / Luciel slots),
  stand-ins 2 (the memo-name / memo-effect builders return empty until S7c ports `32453420` / `324513e0`; S7c's kickoff owns them).
- **`SKILLTREE_NO_LOCK` retired:** no define; its two use sites in `X2UISkillTreeNew.cpp` (`:416`, `:6840`) sit inside the tree flag's `#else` and fall to their own `#else` too.
- **Data.** Game == packed: data034 cbbf994b, data036 f3ea3181, data056 41f03dee, data057 0ba792cb; manifests complete (963 / 243 / 84 / 923). The 4 data034 dialogs
  (incl. `DLG_UI_Command_Desc_Video.lua`) and `SkillTypeColor.lua` are `1B 4B 4C 81` with adopt rows. `Enum.lua`: one hunk in `SKILL_TREE_UI_CUSTOM_MSG`, 58 members 29055-29114
  (one comma added to the previous last member, nothing else changed). `LEDGER CHECK PASS`, `CORE CHECK PASS`. The staged textures are byte-identical to June's; `Texture64_NEW` is still March's.
- **Fix hygiene.** `flagcheck --worktree --files X2Lib/X2UISkillTreeNew2015.cpp`: `1 PASS ... 0 FAIL` / `1 deleted line(s)` (S7b's `return true;`) / `RESULT: PASS`.
- **Deploy.** `DEPLOY OK: X2_offline.exe is 19,964,928 B, dated 2026-10-18 18:23:13, md5 5744a8fd8d0322eb0df7fb8ea56880a2, equal to X2/US_SERVICE/x2.exe (2026-10-18 18:23:08); built at main 70c9d2e2 + 1 uncommitted code file(s).`
  `X2UISkillTreeNew.obj` rebuilt 18:22:57 (after the edit). No play-test needed: the path is reached only by a skill id with no templet.
- **User owes:** nothing. Next: `/x2-phase skill-system-core S7c` (S7c / S7d / S7e one at a time: they share the tree class).

## Phase S7c - Skill memos: popups, packets, emulator, save

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET`. Sub-tag `[JUNE_SKILL] MEMO`. Split out of S7b (user, 2026-10-18).
June's memo popups on the tree ( `328dda50`, `328e9b30`, `328ded60`, `328df480`, cases 29065-29068 / 29076 / 29077 ), the memo packets 0x554-0x557 on client and emulator,
the save rung for June's memo rows, the grant-handler memo buttons and the memo text builders ( `32453420`, `324513e0` ). Kickoff `kickoffs/skill-system-core/S7c.md`.
**Play-test:** select a memo, use the skill ( the `[JUNE_SKILL] MEMO` lines ), relog, remove it with the removal item.

## Phase S7d - Skill pages, the reset item's UI, the HUD refresh

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_PAGE`. Sub-tag `[JUNE_SKILL] PAGE`. Split out of S7b (user, 2026-10-18).
June's page radios / expand / buy ( 29049 .. 29056, `328e6970`, `328dd1f0` ), the page packets ( handlers or `X2OfflineIgnore` rows ), `RESET_SKILL_ACK`'s page fields, the reset item's
tree rebuild ( `31f08290` ) and the HUD refresh `CX2Unit 3235a160` / singleton `324943c0`. Kickoff `kickoffs/skill-system-core/S7d.md`.
**Play-test:** switch pages, buy a page with ED, use the reset item.

## Phase S7e - Description panels: major skills and the command tab

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS`. Sub-tag `[JUNE_SKILL] UI`. Split out of S7b (2026-10-18, unowned June branch).
The major-skill panel ( `328e42d0` from `SkillCommandDescriptionData.lua` via `CX2SkillTree 3220bdb0` ) and the command tab ( `ComboTree.lua` through `328e7ba0`, `328e4bd0`, `328de9a0`,
`328e5290`, `328e8820`, the video popup `328e6cd0`, cases 29061 / 29062 / 29069-29074 ). Kickoff `kickoffs/skill-system-core/S7e.md`.
**Play-test:** the major-skill icons and hover text on two classes; the command tab and a video popup.

## Phase S8 - Running state data: June's pointer model

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_STATE_PTR`. Sub-tag `[JUNE_SKILL] PTR`.

- `m_NowStateData` / `m_FutureStateData` become pointers into the state vector, with June's store `31fe6a40`.
- Port the `31fd2af0` snapshot (user `+0x9544..+0x9568`) and its four consumers (`320347d0`, `31fe6a40`, `31fd6b80`, `3203c3a0`),
  and the gate `31edf580` -> `322074e0`.
- Every March reader from S0b item 5 is converted, and no reference may outlive a vector reallocation.
- The class-side slot-493 call stays checklist 7i.
- **Play-test:** re-equip a slot while its skill is running, at the three points C9h-6 names (null delegate, new entry's skill
  id / hyper byte, a second re-equip). The `PTR` lines show the snapshot taken and consumed.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium.
- Write the `MODS.md` lines for every flag, with revert steps.
- Add a dated note in `JUNE_PORT_BACKLOG_ORDER.md` (Stage 3 done, facts 1-3 corrected).
- Make sure the checklist rows from S3 and S4 are in place.
- Update memory.
- Turn `SERV_IRUHADEV_JUNE_SKILL_DEBUG` off, unless a phase recorded a reason to keep it.

## Open questions (planning, 2026-10-17)

Decided in planning (above): flip everything in Stage 3; adopt June's pointer model; S2-S5 and S8 do not wait for Stage 2.

Open for the user:
1. **When S3 runs relative to Atlas play-tests.** After the flip, unported characters lose most usable skills. That weakens
   any Atlas / Elysion play-test still to come (A6 onward).
   **Recommendation:** run S0a, S0b, S1 and S2 now. Hold S3 until the user says the Atlas play-tests can run on the flipped
   save, or until a pause in them.
2. **Server-only skill values** S0a may find (an SP table, expand-page costs): June's default where absent, else a labelled
   stand-in. S0a proposes each one.

## Play-test hand-off (every code phase)

Dev console first (auth level 3, END key). Give copy-ready commands with real values, and name the character and job from
`els_db.sql`, with the skill name and slot. For each check: where / paste / what it does / look for / if not. The log tag is
`[JUNE_SKILL]` plus the phase's sub-tag.
