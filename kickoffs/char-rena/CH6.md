# June character Rena (Stage 7) - Phase CH6 - Whole-character walk and gap reruns

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-rena CH6` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH3d, CH4b and CH5 reviews (CH4b's covers CH4a) |
| Flag | none (a defect goes back to its phase's flag) |
| Log tag | `[JUNE_CHAR]` (`RENA *`) |
| Review | none (the user's walk is the gate) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Planning evidence (2026-11-07)" "Phase CH6 - Whole-character walk and gap reruns"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH6 - Done"
```
Then CH0's Done note (the S3 baseline) and Raven's CH6 Done note.

## Task

Do "Phase CH6". Back up the save first. Re-run `census.py`, `charport_scan.py` (short scratch dir), `charport_keys.py Lire_ElvenRanger` and
`s3_no_state.py --family X2GULire_ElvenRanger.cpp --preprocess`; before / after table in the Done note (planning: 260 `no_state` rows), every remaining
`no_state` id explained. Then hand over the walk: one unit per class (CH0b's 3 / 12 / 13 / 24 / 102 / 114 units, `reyaa` for 103), every skill in an A and
a B slot (each cast twice: cool time, MP), a hyper, natural force (fill, spend, gauge, hyper buff), each trap, the passives, one dungeon per job, then
leave the game and return to the village; transcended units for A-5 / B-5; and the becoming-transcended transition on un-transcended units (plan "Phase
CH6": `/tc 1`, `/qdone 42030`, `/additem 288493 1`, one per second job, guide popup picture, skill-tree rows, HUD portrait, relog). Failures go back to
the owning phase's flag.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by CH5 (2026-11-09)

The walk adds the cut-in checks CH5 left to it: on each Rena job cast a skill that reaches `ShowActiveSkillCutInAndLight` and read `[JUNE_CHAR] RENA UI cut-in set <set> index <i> name <gfx>`
(set 0 = jobs 3 / 12 / 13 / 24 with indices 0 / 1 / 2 on the skill's cut-in index; set 1 = 102 `HQ_CutIn_Rena_WS_01`; set 2 = 103 `_GA_01`; set 3 = 114 `_NW_01`), and the awakening hyper on 102 / 103 / 114
(`[JUNE_CHAR] RENA UI hyper cut-in class <c> name HQ_Cutin_R{WS,GA,NW}_Transcendence`, only when the class script's awakening state sets `SHOW_HYPER_CUT_IN_TIME`).

## Amended by CH2a review (2026-11-09)

CH2a's play-test cast only the Enhanced ids on unawakened Rena. The walk adds, on class 3: each of Rail Stinger, Perfect Storm, Aero Tornado, Assault Kick and Phoenix Strike
cast once while awakened (the hyper state: Rail Stinger and Aero Tornado use their own `_HYPER` tables), and one base id (23000-23004 or 23200 Backflip Shot, not the Enhanced one)
slotted and cast: it must bind its own state, not fall into the Power Attack default.

## Amended by CH2b (2026-11-09)

CH2b's play-test casts only the Enhanced ids unawakened. The walk adds, on class 12 (`CombatRangerT`, unit 87): each of Spinning Kick, Crescent Kick, Dive Kick Bombing, Rising Falcon, Back Kick, Low Kick and Assault Impact
cast once while awakened (the hyper state: Crescent Kick and Dive Kick use their own `_HYPER` table names with the normal functions), and one base id (23005-23007, 23202-23206, not the Enhanced one) slotted and cast: it must bind its own state,
not fall into the Power Attack default. Back Kick's base id (23203) must **not** chain to Low Kick on a second press (June tests the Enhanced templet's `m_iDeleteSkillIDByUpgrade`).

## Amended by CH3d review (2026-11-10)

- **Night Watcher bases ( class 114 ).** CH3c's set 2 could not run five bases: at lv 99 the emulator's auto-learn ( `X2OfflineSkill.cpp` @930-965 ) swapped them for their Enhanced ids. Cast each base once on a 114 unit
  below lv 99 ( or one whose save lacks the upgrades ): Furious Engage 23025, Delayed Fire 23217, Seed of Wrath 23219 ( never a `second seed 1` line ), Call of Ruin 23022 ( normal animation speed ), Trapping Arrow - Fungus 23024.
- **Wind Sneaker sets 2 / 3 ( class 102 ).** CH3a's base ids and Spinning Crescent / Gyro Kick / Tempest passed by the user's eye only; one cast each with `python tools/logs.py --grep "RENA SKILL"` is not available any more
  ( `_SKILL_DEBUG` retired ), so read `ES_CREATE` lines and `ErrorLog.txt` ( 0 Motion Error ) instead.
- **CH3d's visual checks** ( the user's verdict was not given ): the transcended pictures on 102 / 103 / 114 ( character list, HUD portrait, gauge: Night Watcher's is `TR_N_ARCHER_STATE` ), the transcendence cut-ins,
  and Spiral Strike's spin ( Rena invisible for its 2.75 s, visible again on landing ).
