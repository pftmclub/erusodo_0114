# June effect completion - C7 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C7 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C7 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_TOUCH_LAND` |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C7 - Touch-land"
```

Also read: `june2017_re/funcmap.tsv` rows the phase cites, and its commits (`git log --oneline --grep="C7"`).

## Task

Review C7's commit(s) against the June binary and the plan's C7 section: the gate 31f83380, m_vVelocityBefore added the March way (no June offset), the spawner 31f81ba0 against ghidra/dis_31f81ba0.txt, and the throttled JUNE_FX TOUCH_LAND line.

Also review the three items the C6b review added to C7 (kickoff "Added by the C6b review"; funcmap rows tagged `Effect C6b review`):
- `31f6ea20` at RT_CREATE_DAMAGE_EFFECT (`@31f86148`), the CEffect `+0xb18` member (-1), and the base vslot `0x328` hook in the ctor (`@31f7f61e-31f7f62b`), under `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`. Check that the `(int)` truncation is kept and has a JUNE_BUGS row.
- The CREATE loop's inlined INHERIT_CRITICAL adds (`@31f89d52-31f89d82`).
- The die spawner's INHERITLOCKONDATA copy (`@31f83110-31f83126`), with the key matrix row corrected.
- Both of the last two go under `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`, so run flagcheck for that flag too.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range <first C7 commit>^..HEAD`
for each flag the phase used. Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C7 - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
