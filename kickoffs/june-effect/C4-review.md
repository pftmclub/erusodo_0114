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

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C4 - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
