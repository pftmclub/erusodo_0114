# June effect completion - C4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C4 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C4 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_DAMAGE_MATH` |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C4 - 8b, damage math"
```

Also read: `june2017_re/funcmap.tsv` rows the phase cites, and its commits (`git log --oneline --grep="C4"`).

## Task

Review C4's commit(s) against the June binary and the plan's C4 section: every formula against June's asm (ignore-defence, division, divide-from-range, Nasod armor combo), PvE vs PvP branches, and that MODS.md lists every formula change.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range <first C4 commit>^..HEAD`
for each flag the phase used. Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

## What C4 changed against the plan text (read before reviewing)

- **The ignore-defence keys were already June buff port Phase 5c's** (`SERV_IRUHADEV_JUNE_BUFF_NEW_STAT`). C4 changes no ignore-defence
  number; do not look for them under `_DAMAGE_MATH`. The census rows were `implemented` before C4 started.
- **Flags:** `SERV_IRUHADEV_JUNE_DAMAGE_MATH` (range rate, Nasod gate) and, per the user's 2026-10-06 decision, **`SERV_IRUHADEV_JUNE_BUFF_REACT_APPLY`**
  for the DIVISION window (`JuneCheckDivisionWindow` = renamed `CheckDamageDataKeyName`, tick, re-arm). Run flagcheck for both.
- Check against June (Ghidra, read-only): the `DIVIDE_DAMAGE_FROM_RANGE` parse (`32632390 @3263575f-32635b0f`: "%s%d" names `RANGE0`.. from 0,
  defaults 99999 / 1.0), `32622780` (aim point, walk, `rate < 1.0`), its call site `@322857f6` and the placement before the class hook,
  `3227a960` / `322749f0` / `32275da0` / `32622ad0`, and `329f63a0`'s top switch + gate. The x87 results are in funcmap rows
  `Effect C4` (harness `c4_x87.py` in the C4 session scratchpad; re-run `june2017_re/x87test.py`'s idiom if in doubt).
- **Judgement calls to confirm:** (1) the Nasod gate passes March's `SI_A_AN_PULSE_BULLET` / `SI_A_AN_PHASE_SHIFT` in place of June's four
  reform IDs; (2) user decision 2026-10-07: Add's other hits set no Nasod target until DamageEffect data is adopted; (3) the re-arm loop
  iterates `GetUnitList()` directly (June copies it first; the body changes no list).
- **Found, not owned:** `GetFinalDamage @32285757-322857e8`, an NPC-attacker block before the DIVIDE call (funcmap row, status `candidate`). Identify
  it (Opus) and decide whether it is C4's neighbour or a character-port item.
- No data sets any of the three keys, so there are no before/after damage numbers; the play-test is a regression check plus the logs.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C4 - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
