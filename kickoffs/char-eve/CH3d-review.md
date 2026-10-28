# June character Eve (Stage 7) - Phase CH3d review - Hyper actives, memo states, transcendence rows

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH3d review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH3d done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`, `SERV_IRUHADEV_JUNE_CHAR_EVE_TRANSCEND` |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL registered`), `[JUNE_TRANSCEND]` (`IMAGE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH3d - Done (2026-10-28, Sonnet 5.5)"
```
Then the `Char Eve CH3d` rows at the end of `june2017_re/funcmap.tsv`.

## Task

Review the CH3d port against the listings (`python june2017_re/funcdis.py --func <va>`, Ghidra for carved functions). Where it could be wrong:
- **The 15 "= March" claims.** Init lists were compared by string; re-check the EventProcess cancel orders (Hornet / Spit WALK, SKILL, DASH; Psychic / Illusion Ray SKILL, BWALK, WALK, DASH) and the Illusion Ray `UAI` 14 / 15 against `UAI_ELECTRA_PLANE_ANGLE_UP / DOWN`.
- **Link Overcharge `32997940`.** The `31f8b010` argument mapping (read from raw pushes; compare the CH3b Electronic Field call), `322a6960` arguments ( 438, 0, level, 0, 0 ), `GetEquippedActiveSkillTemplet( 25601, true )`, the early `return` without CommonFrameMove, the 300 offset as a double.
- **Hornet `32997350`.** Order cut-in / world colour, `CreateInstance` last argument for the mesh, the three particle names and the damage effect position (bone vs `GetPos`).
- **Spit Grenade `329a9df0`.** Five set names, `bHyper` on all five, `0x52` = `SA_SIZE_ABS`, tag 15 `bAdd` false, `GetNowStateSkillID` = vslot 0x524.
- **Psychic Artillery `329abb90`.** The two trailing wstrings of `June_AddLaserToSystem`, bone names, `false` flag, laser 10000 / 0.3 / 1.0.
- **Ferdinand binds.** `32757ea0` / `32294460` (AI type 1, UID >= 0, unit list match on UID and type) and `31e4d140`; whether `CX2GUUser` binding is reachable from `GetOwnerGameUnit` in March (play-test: Ferdinand follows, no "can't find" in ErrorLog).
- **Transcendence.** Allow-list 108 / 109 / 116, `LevelUpEvent.lua` TYPE1, the picture rows (and that June's 160by160 table really has no Eve pair: read `333988f0` / `3339ead0` like T6c).
- **Hand-off check:** units 59 / 60 / 61 and the log lines the hand-off lists.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --with JUNE_CHAR_EVE_TRANSCEND --range <first CH3d commit>^..HEAD`. Fix clear transcription errors under the same flags (then `tools/build.py --deploy`); hand design decisions to the user. Next phase kickoff `CH4a.md` exists.
