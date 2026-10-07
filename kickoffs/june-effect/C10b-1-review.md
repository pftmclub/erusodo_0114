# June effect completion - C10b-1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C10b-1 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C10b-1 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_HP_RATE_DAMAGE` |
| Log tag | `JUNE_FX hp-rate` in `offline_server.log` (unbracketed: read with `python tools/logs.py --grep "JUNE_FX hp-rate"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C10 - Decided"
```

Also read: the `june2017_re/funcmap.tsv` rows tagged `Effect C10a` that `kickoffs/june-effect/C10b-1.md` lists, and the
phase's commits (`git log --oneline --grep="C10b-1"`).

## Task

Review C10b-1's commits against the June binary and its kickoff: GetHpRateDamage (`32273e80`: the two types, the strict now-HP > 1.0 gate, overwrite not add) and both sites (GetFinalDamage on the defender with the >= 0 clamp; user DamageReact after the BBT 78 block), the JUNE_BUGS row for the double overwrite, and the SUPERSONIC gate left to checklist 7c.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_HP_RATE_DAMAGE --range <first C10b-1 commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and explain every OTHER row and deleted line; a FAIL or
ERROR is a finding. Check that `census.py` now passes the HP-rate item-1 line.

C10b-1's test vehicle is a TEMPORARY edit of `ARROW_OF_EXPLOSION` in `kom_contents/data036_kom/DamageEffect.src.lua`
(two `HP_RATE_DAMAGE_*` lines, marked `C10b-1 TEST`). If the play-test has passed and the edit is still there, revert it
(the comment says how), `python tools/kom.py compile data036`, `/elstools pack data036`, and ask before the copy.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C10b-1 - Review" under C10 in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
