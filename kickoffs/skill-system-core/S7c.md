# June skill system core - Phase S7c - Skill memos: popups, packets, emulator, save

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7c` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7b (June's tree class, which hosts the memo popups); not alongside S7d (both edit the tree class and `Handlers_Skill.cpp`) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET` (packets, handlers, emulator, save rung); the popup code inside the tree class sits under `SERV_IRUHADEV_JUNE_SKILL_TREE_UI` too |
| Log tag | `[JUNE_SKILL] MEMO` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7c-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S3b - June skill notes (memos)" "Phase S3d - June skill memo consumers" "Phase S7b - Done (2026-10-18, Opus 5.5 high)" "Phase S7c - Skill memos"
```
`june2017_re/s0b_ui_funcmap.tsv` rows for the addresses below.

## Task

Split out of S7b (user, 2026-10-18: S7b = tree class, S7c = memo, S7d = pages). Port June's memo UI and the memo packets onto S7b's
`CX2UISkillTreeNew2015` port.

**Carried from S3b (2026-10-18).** S3b ported June's memo tables (`CX2SkillNoteManager`) and the user tree's active-memo multimap
(`CX2UserSkillTree::Add/Remove/IsJuneMemoActive`), flag `SERV_IRUHADEV_JUNE_SKILL_NOTE`. Nothing activates a memo yet; that is this phase's job:
- **UI.** The memo popup builder `328dda50` (left / right memo per skill, state from `CX2SkillNoteManager::GetJuneMemoState`
  (32455150): 0 locked below the row's level, 1 open, 2 active), its helpers `328d9cc0` / `328dcae0`, the select popup `328e9b30`
  (UICustomEventProc 29065 / 29066, 29067 / 29068), the desc buttons `328ded60` and `328df480`, and the manager's two text builders
  `32453420` / `324513e0` (not ported by S3b). S7b leaves those UICustomEventProc cases and the memo calls in `UpdateSkillTree` as
  named stubs; replace them.
- **Packets** (June event ids, none in March's `EventID_Client.h`; name them from June's table and add them client-side behind the flag):
  REQ `0x554` { base skill (32207210), memo } (`32453060`) -> ACK `0x555` { ok, skill, memo, byte } (`320aaae0`; handler `31f1bdb0`:
  popup STR 0x8b98, `AddJuneActiveMemo`, then the A/B slot buff refresh `31fd84c0` and the tree UI). REQ `0x556` (3 dwords, `324530d0`)
  -> ACK `0x557` (`320e7eb0`, with an inventory vector; handler `31f1c1d0`: inventory update `3286d590`, `RemoveJuneActiveMemo`, same refresh).
  Read the sender `328ed5b2` for what 0x556 carries (a removal item from `AddRemovableMemoItemID`'s list, e.g. Tome of Amnesia 135667?).
- **Emulator and save.** Handlers for both REQs; `unit_skill_note` (March notebook pages, emptied by v17) becomes June's
  (unit, skill, memo) rows in a new rung (the next free number after S5's v18; check `CX2OfflineDB::Migrate()`); the active list goes
  out with the unit info (S5's carry).
- **March's notebook path is dead in June's data** (0 `AddSkillNoteTemplet` calls; `REFORM_SKILL_NOTE_UI` is off in the US build):
  `EGS_REG_SKILL_NOTE_MEMO_REQ` and the page-expand items can retire with this phase.
- **New-skill memo buttons** in the grant handler (`328df480` -> `328ded60`): S7 left a comment at
  `CX2SkillTree::Handler_EGS_JUNE_ACQUIRE_SKILL_NOT`; wire it to the S7b class.

**Carried from S3d (2026-10-18).**
- S3d made the engine consumers of an active memo real (flags `SERV_IRUHADEV_JUNE_SKILL_MEMO_*`). This phase's play-test is their first:
  an active MI_HUGE memo scales its skill's effect sets and attack boxes; a transcendence memo renames the state string and swaps the MP
  templet; each use writes a `[JUNE_SKILL] MEMO` line (`JUNE_SKILL_SYSTEM_PHASES.md` "Phase S3d", "What a memo does now").
- Two more UI readers of the memo tables: `3220d2b0` (caller `31d885e0`: the skills an item is used by, memo rows included, in the item
  description) and `3235b070` (caller `328e9b30`) / `3235b9e0` (memo text from `GetJuneMemoValue` 32452150).

**Play-test:** open the tree, select a memo (popup, ACK, the `[JUNE_SKILL] MEMO` lines when the skill is used), relog (the memo persists),
remove it with the removal item.
