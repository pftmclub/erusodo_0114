# June port backlog - Stage 7 planning - the remaining March characters

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan char-eve` (the first in the order below; later: `char-chung`, `char-elsword`, `char-raven`, `char-rena`, `char-ara`, `char-elesis`, `char-add`) |
| Output | plan key `char-<name>` (`JUNE_CHARACTER_<NAME>_PHASES.md`, `kickoffs/char-<name>/`, a `char-<name>` STATUS block); one character per planning session |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 7); template: `JUNE_CHARACTER_AISHA_PHASES.md` "Template for Stage 7" |
| Depends on | pilot-char-revamp Z (done 2026-10-22). Stage 6 does not block a character's non-transcendence phases; each plan lists its transcendence rows as waiting on `june-transcend` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_<PREFIX>_*` (none in this planning session) |
| Log tag | `[JUNE_CHAR] <PREFIX>` |
| Review | none (a planning session) |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7" "Phases" "CH1 split (2026-10-21, user)" "CH4 split (2026-10-22, Opus)" "Phase CH-N - A June-only later class path (template; not run for Aisha)" "CH6 - Done (2026-10-22, Sonnet 5.5)"
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 7: remaining March characters" "Standing rules (every stage, every kickoff)"
```
Then the character's own checklist rows (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7, grep the class name) and the `june2017_re/charport_scan.py` output.

## Order and why (set by Z, 2026-10-22; a planning session may move a row with a stated reason)

| # | Character | Plan key | Why here |
|---|---|---|---|
| 1 | Eve | `char-eve` | Smallest gap (3) and no blocker: a second character proves the template without new decisions. Its extras (167 alpha tags, 7ag slot encoders, El Crystal statics) are the 7c / 7ag / 7ah traps Aisha already walked. |
| 2 | Chung | `char-chung` | Gap 15, no June-only class, no decision; `ChangeMaxCharacterAddResource` is ported. |
| 3 | Elsword | `char-elsword` | Gap 20, no blocker, but `DAMAGE_SKILL_ID` x103 in his damage data is a large table job: plan its data phase first. |
| 4 | Raven | `char-raven` | Gap 25, the biggest plain character; after three clean runs the template should be tight. |
| 5 | Rena | `char-rena` | Gap 12 would put her second, but she needs the **`ADD_RENA_SYSTEM` decision** (natural force, 77 uses). Ask it when she is scheduled; if the user answers before Eve closes, move her to #2. |
| 6 | Ara | `char-ara` | Gap 14; first run of **CH-N** (classes 34, 124), the stat-row decision, `CUSTOM_RENDER_STATE`, 2 new motion files. After the plain characters, so CH-N is written once from the most mature template. |
| 7 | Elesis | `char-elesis` | Gap 20; CH-N for 35 / 125 (reuses Ara's), first live reserved-buff path (7ai). |
| 8 | Add | `char-add` | Skeleton March kit, 4 June-only classes (33, 36, 123, 126), no stat rows for 9 / 32 / 122, 4 new motion files: last, with everything proven. |

Gap alone did not predict effort (Aisha's gap of 1 took about 96 commits); the order follows blockers and decisions first, gap second.

## Task (per planning session)

Plan one character as `JUNE_CHARACTER_<NAME>_PHASES.md` in the Aisha plan's shape, using its corrected template:
- re-run `census.py`, `charport_scan.py` and `charport_keys.py <Class>`; run `ch0c_listing_check.py` on the new registration map in CH0 (step 0);
- name the CH1 / CH2 / CH3 / CH4 sub-phases up front from the map (more than ~25 functions or more than one engine piece = split), each with model, flag and review;
- list every `unmapped` callee per skill and give each an Opus sub-phase;
- include CH-N only when the character has a templet-backed June-only class;
- put the character's transcendence rows in a "waits for Stage 6" list, not in a phase;
- the character's checklist section-7 rows go to the phase that owns them, with the `(<Name>: CHn)` mark made by that phase.
- skill-level reads in the character's CH1 kickoffs call `CX2GUUser::June_GetSkillLevel` / `June_GetLearnedSkillAbilityValue` (Eve CH1a2, flag `SERV_IRUHADEV_JUNE_USER_SKILL_LEVEL_HELPERS`) and never copy them into the class.
- skill-use reads in the character's CH1 kickoffs call the seven `CX2GUUser` helpers `June_UserNoStateChangeActionFrameMove` / `June_MarkSkillSlotUsed` / `June_ReserveSkillUse` / `June_StateChangeBySkillSlot` / `June_SkillMemoMpRefund` / `June_CheckSkillUseNotPve` / `June_CheckSkillUseNotDungeon` and the fields `m_iJune8ea0` / `m_iJune8ea4` / `m_fJune8ea8` / `m_fJune8eb0` / `m_abJune90d3` / `m_abJune90d0` (Eve CH1l5, flag `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`) and never copy them into the class.

Output: the phase doc, `kickoffs/char-<name>/`, a STATUS block. No code, no build. Design questions go to the user at the end, each with a recommendation.
