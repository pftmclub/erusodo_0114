# June damage and stat calculation - Phase D0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-stat-calc D0 review` |
| Plan doc | `JUNE_STAT_CALC_PHASES.md` (branch `main`) |
| Depends on | Phase D0 done |
| Flag | none |
| Log tag | `[JUNE_STATCALC]` |

## Read first

```sh
python tools/docsection.py JUNE_STAT_CALC_PHASES.md "Rules" "Design: the map and the verification" "Phase D0 - Damage map (read-only)" "Phase D0 - Done"
```

## Task

Review D0's map and block ledger. No flag, so no flagcheck.

- Re-derive `gfd_blocks.tsv`'s coverage: ranges contiguous from `32284aa0` to the function's last `ret`, none overlapping.
- Spot-check five blocks in the exe (two marked `June`, two `March`, one `absent`): the state must hold against March's live branch
  (resolve the `#ifdef` with `cl /P`, not by reading).
- Spot-check three map rows outside `GetFinalDamage` (one from each `DamageReact`, one helper) and each row's reader list with a grep.
- Check the class hand-offs landed where the Done note says.

If one spot-check fails, re-check that function's rows whole. Fix rows in place; write "Phase D0 - Review", update STATUS, check the
D2 kickoff still fits, commit.
