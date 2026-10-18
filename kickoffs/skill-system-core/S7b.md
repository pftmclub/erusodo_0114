# June skill system core - Phase S7b - June's skill tree UI (CX2UISkillTreeNew2015)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7b` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7 (the learn path; review not required), S3c, S3b; not alongside S6 / S6b (`X2UISkillTreeNew.*`) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_TREE_UI` (+ `SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET` for the memo REQ / ACK pair if it is its own logical change) |
| Log tag | `[JUNE_SKILL] UI` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7b-review.md` |

> **Split during the phase (user, 2026-10-18):** S7b = June's tree class itself ( window, slots, level rows, portrait, tooltips with the skill-type
> branches, tabs, the carried equip bar, no learn / SP UI ). The memo popups and packets moved to `S7c.md`, the page packets / reset item / HUD refresh to
> `S7d.md`, and the major-skill and command panels ( `SkillCommandDescriptionData.lua`, `ComboTree.lua` ) to `S7e.md`. The items below are kept as
> written; the Done note says which phase owns each.

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S0b - Slots, pages and UI RE" "Phase S7 - Skill tree UI and learn logic" "Phase S7 - Done (2026-10-18, Opus 5.5 high)"
```

## Task

S7 was split (user, 2026-10-18): June has no SP learning, so S7 ported the learn path (server-pushed grant, level-up auto-equip,
the slot-change queue). **S7b is the tree UI**, function by function from `june2017_re/s0b_ui_funcmap.tsv`:

- `CX2UISkillTreeNew2015` against March's `CX2UISkillTreeNew` (103 functions; `DLG_UI_Skill_Tree_New2015.lua`,
  `DLG_UI_Skill_Icon_New2015.lua`): no learn / upgrade / init buttons, the skill-type branches, the major / command tooltips, tabs.
- The memo popups and June's memo packets (S3b carry below), the page packets (handlers or `X2OfflineIgnore` rows), and the
  page UI as June has it.
- `SERV_IRUHADEV_SKILLTREE_NO_LOCK` retires here (S0b user decision; its sites are in March's tree UI). Remove the define, list it in
  `MODS.md`.
- June's new-skill memo buttons in the grant handlers (`328df480` -> `328ded60`): S7 left a comment at
  `CX2SkillTree::Handler_EGS_JUNE_ACQUIRE_SKILL_NOT`; wire them with the class.
- Decide with the user whether March's learn / SP UI path (`EGS_GET_SKILL_REQ`, the emulator's `LearnSkills`, the SP display) goes
  away with the June tree (June has none) or stays as a local convenience. Do not keep it silently.
- `CX2Unit 3235a160` (HUD stat refresh through the June-only singleton `324943c0`, 23 callers) was not ported by S7: say whether it
  belongs to this UI port or needs its own row.
- S7 review: the grant handler calls March's `UpdateUI( false, true, true )` for June's `328e7b70( 0, 1, 0 )` (June has no SP argument:
  `( bEquipped, bSkillTree, bSkillPage )`). When June's `UpdateUI` replaces March's, map the call to it and drop the SP step with the SP UI.

**Play-test:** open the tree on two classes; skill types (S3c's gate item), memo select + activation (S3d's first play-test: the
`[JUNE_SKILL] MEMO` lines), a tree reset with the reset item, the page UI as June has it.

## S0b amendments (2026-10-17)

- **June's tree UI class is `CX2UISkillTreeNew2015`** (RTTI `3373ca34`; vt `334c37fc` holds only the dtor `328ee660`;
  ctor `328e76a0`). There is no `CX2UISkillTreeNew` in June. The class block is `328d98e0`-`328ee680` (103 functions).
  The function map is `june2017_re/s0b_ui_funcmap.tsv`: 6 match, 40 changed, 4 candidate, 53 june-only (about 15 of
  them STL), 18 march-only. It loads `DLG_UI_Skill_Tree_New2015.lua` (`@328ed9c0` in `InitSkillTreeUI 328ed890`) and
  `DLG_UI_Skill_Icon_New2015.lua` (`328db2d0`). The 4 slot-data vtables `334c2e94`-`334c2eb8` are types 1, 2, 5 and 6;
  types 5 and 6 are June-only.
- **`UICustomEventProc 328eab00`** switches on `wParam - 29000`. 29000-29058 have March's numbering, and June adds
  29059-29079. It handles 28 shared values and 19 June-only ones (memo, tabs, major / command tooltips, transform
  slot). It drops 26 March values, all of the learn / upgrade / reset group among them.
- **There is no learn and no reset REQ in the June tree class.** The memo cases 29067 and 29068 send `0x554` / `0x556`
  through the memo singleton `324550a0` (`32453060`, `324530d0`). 29016 only toggles the remove mode (`+0xa9`). Before
  porting, find where June learns and levels skills, if anywhere: grep the senders of March's `EGS_GET_SKILL_REQ` shape.
  This touches S0a's learn / SP rules and S3's SP refund, so if June has no SP learning, stop and tell the user.
- **Pages:** all three `Skill_Page` radios send 29049, which sets tree `+0x4b8`, refreshes, and sends the decide REQ
  `3220f160` (REQ `0x47a` / ACK `0x47b`). The ACK handler is `31f1fd90`, and June's ACK carries a whole `KUnitInfo`
  (funcmap). Expand-by-ED sends its REQ directly. Cash sets the shop category to `0x11`. `BUY_CONFIRM` ->
  `322149e0` when ED >= `+0x12c` (candidate expand REQ). `BuySkillPageDialog` is `328dd1f0`. The emulator still has
  no handler or ignore row for the three page packets.
- **User decisions (S0b, 2026-10-17):** `SERV_IRUHADEV_SKILLTREE_NO_LOCK` **retires with S7**. June has no two-choice
  blind (`SetBlindSelectSkill`, `GetAnotherSelectSkillID` and `IsLearnBeforeSelectSkill` are march-only rows), so once
  the June tree replaces the March one its sites are gone. Remove the define in S7's commit and list it in `MODS.md`.
  `SERV_IRUHADEV_SKILL_SLOT_B_FREE` is **kept via the emulator**: S5 retires its four client sites and the emulator
  sends slot-state key 0 = 2.
- The census of June skill DLGs against March data034 is in `june2017_re/s0b_skill_dlg_census.md`. The tree needs the
  June-only `STUCM_*` / `SMUCM_*` enum members in the merged `Enum.lua`, and four `STUCM_RESET_*` values that come
  from June's C++.

## Carried from the S3 review (2026-10-18)

- March's tree UI reads the required character level at index 2 of `m_vecRequireCharacterLevel`, but June's data has
  1-entry arrays (S2: no 30-entry padding), so the S3 log shows `LOAD reach ... -> 0`. Port June's reader (how
  `CX2UISkillTreeNew2015` gets a row's required level) rather than padding the data.

## Carried from S3c (2026-10-18)

- **User decision (2026-10-18): the tree UI's skill-type readers are S7's.** S3c added `ST_SPECIAL_ACTIVE_GRADE_1..3` (14-16),
  `ST_CORE` (17) and `ST_CORE_PASSIVE` (18) to `CX2SkillTree::SKILL_TYPE` (`SERV_IRUHADEV_JUNE_SKILL_TYPE`) but left
  `X2UISkillTreeNew.cpp` alone. Until S7, June's special actives (1,148 templets, all types 14-16; June has none of type 7) look
  and act like passives in March's tree: passive background, `[Passive]` tooltip, not draggable, no right-click equip, no skill
  level bonus. June's handling, per `june2017_re/s3c_skill_type_readers.tsv`: right-click equips 5, 6, 7, 12-16 (`328e6550`);
  the slot-data `UpdateUI` (`328dcd50`), `CreateSlotUIPreset` (`328db2d0`), `ShowSkillDesc` (`328e9160`) and `GetSkillSubDesc`
  (`328e2900`, STR 40577-40585) each have their own type branches. Port them with the class.
- **S3c's gate item "a grade-1 special active shows its type in the tree UI and tooltip" is S7's play-test now.**
- June auto-equips new skills of types 5, 6, 7, 14-16 into free slots (`328db990`, UI class block, no 12 / 13). March's live
  `Handler_EGS_GET_SKILL_ACK` (the `UPGRADE_SKILL_SYSTEM_2013` branch) auto-slots nothing; its old switch is in the dead `#else`.
  Port `328db990` and its three callers (`323ae130`, `32473f70`, `325346e0`) with the learn path.
- June's `Handler_EGS_RESET_SKILL_ACK` (`322112d0`) also writes the page's SP / CSP into tree `+0x4a8` / `+0x4ae` and refreshes
  the UI through `3214f050` / `328e7b70`. Those are page and UI work; port them with S7's page packets (S5 owns the slot loop).

## Carried from S3b (2026-10-18)

S3b ported June's memo tables (`CX2SkillNoteManager`) and the user tree's active-memo multimap (`CX2UserSkillTree::Add/Remove/
IsJuneMemoActive`), flag `SERV_IRUHADEV_JUNE_SKILL_NOTE`. Nothing activates a memo yet; that is this phase's, with the tree UI that sends it:
- **UI.** The memo popup builder `328dda50` (left / right memo per skill, state from `CX2SkillNoteManager::GetJuneMemoState`
  (32455150): 0 locked below the row's level, 1 open, 2 active), the select popup `328e9b30` (UICustomEventProc 29065 / 29066), the
  desc buttons `328ded60`, and the manager's two text builders `32453420` / `324513e0` (not ported by S3b).
- **Packets** (June event ids, none in March's `EventID_Client.h`; name them from June's table and add them client-side behind a flag):
  REQ `0x554` { base skill (32207210), memo } (`32453060`) -> ACK `0x555` { ok, skill, memo, byte } (`320aaae0`; handler `31f1bdb0`:
  popup STR 0x8b98, `AddJuneActiveMemo`, then the A/B slot buff refresh `31fd84c0` and the tree UI). REQ `0x556` (3 dwords, `324530d0`)
  -> ACK `0x557` (`320e7eb0`, with an inventory vector; handler `31f1c1d0`: inventory update `3286d590`, `RemoveJuneActiveMemo`, same refresh).
  Read the sender `328ed5b2` for what 0x556 carries (a removal item from `AddRemovableMemoItemID`'s list, e.g. Tome of Amnesia 135667?).
- **Emulator and save.** Handlers for both REQs; `unit_skill_note` (March notebook pages, emptied by v17) becomes June's
  (unit, skill, memo) rows in a new rung (agree the number with S5's v18); the active list goes out with the unit info (S5's carry).
- **March's notebook path is dead in June's data** (0 `AddSkillNoteTemplet` calls; `REFORM_SKILL_NOTE_UI` is off in the US build):
  `EGS_REG_SKILL_NOTE_MEMO_REQ` and the page-expand items can retire with this phase.

## Carried from S3d (2026-10-18)

- S3d made the engine consumers of an active memo real (flags `SERV_IRUHADEV_JUNE_SKILL_MEMO_*`). This phase's play-test is their first:
  an active MI_HUGE memo scales its skill's effect sets and attack boxes; a transcendence memo renames the state string and swaps the MP
  templet; each use writes a `[JUNE_SKILL] MEMO` line (`JUNE_SKILL_SYSTEM_PHASES.md` "Phase S3d", "What a memo does now").
- Two more UI readers of the memo tables: `3220d2b0` (caller `31d885e0`: the skills an item is used by, memo rows included, in the item
  description) and `3235b070` (caller `328e9b30`) / `3235b9e0` (memo text from `GetJuneMemoValue` 32452150).
