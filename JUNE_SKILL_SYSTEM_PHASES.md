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
  - Ported: `Handler_EGS_RESET_SKILL_ACK` clears slots for 14-16 as well (June `322112d0`, switch @32211750: 5, 6, 7, 12-16). `Handler_EGS_GET_SKILL_ACK`
    (March-only; June's `UIServerEventProc 32216150` has no case for it) auto-slots a new 14-16 skill next to 7 (June's auto-equip `328db990` takes 5, 6, 7, 14-16).
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
  reset that skill in the tree: its slot empties (before S3c the slot kept the id). Learning a new 14-16 skill with a free slot puts it in the slot.

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

## Phase S6 - Slot UI

**Run with:** Sonnet 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_SLOT_UI`. Sub-tag `[JUNE_SKILL] UI`.

- Adopt June's `DLG_GAME_SKILL_SLOT_NEW_10slot` (and the slot images S0b lists), and port June's slot-UI code per S0b item 4:
  the gage / cooltime per slot and the locked TC frame.
- The exe and the kom move together.
- **Play-test:** the 10-slot bar, cooldowns on all eight usable slots, the TC frame shown locked.

## Phase S7 - Skill tree UI and learn logic

**Run with:** Opus 5.5 high, then an Opus review. Flag `SERV_IRUHADEV_JUNE_SKILL_TREE_UI`. Sub-tag `[JUNE_SKILL] UI`.

- June's tree dialogs and the tree UI class per S0b item 4, function by function, including the learn / reset / purchase windows
  and the client side of learn.
- The page packets get June's handlers or `X2OfflineIgnore` rows (S0b item 3).
- Settle `SKILLTREE_NO_LOCK` / `SKILL_SLOT_B_FREE` as the user decided in S0b.
- **Play-test:** open the tree, learn, level, reset and re-learn on two classes; the page UI as June has it.

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
