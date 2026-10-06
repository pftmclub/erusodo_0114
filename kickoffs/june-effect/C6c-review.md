# June effect completion - C6c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C6c review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C6c done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST` and `SERV_IRUHADEV_JUNE_FIX_EFFECTSET_BUFF_NULL` (user chose the guard, 2026-10-07) |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C6a2 - Done" "C6c - Done"
grep "Effect C6a2" june2017_re/funcmap.tsv
```

Also read C6c's commits (`git log --oneline --grep="C6c"`).

## Task

Review C6c's commits against funcmap rows `321ee480`, `321e3650`, `EffectSetInstance+0x160`, `32d41960` and `32d40dd0`.
These are the things the port could get wrong:
- `PushBuffFactor( clone, true )` is called **once per entry, inside the loop**: JUNE_BUGS 59, verbatim;
- the clone is taken at the entry's level;
- the mod kinds are not swapped, and the finalizer helper's double gate holds (type match **and** BFT_TIME, values 2..3, index 0 skipped);
- the consumer runs on the instance list only, with no EffectData `+0x118` call;
- the GetBuffFactorPtr null handling is what the user chose;
- the SIEE 15 attach keeps the old log line in `#else`, nested under `SERV_IRUHADEV_JUNE_SOCKET_PARITY`;
- no June offset is ported.

Hygiene is one command per flag: `python tools/flagcheck.py <FLAG> --range <first C6c commit>^..HEAD`. Paste its `files:` /
`in this flag's files:` / `RESULT:` lines, and explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check `321ee480 @321ee4dd-321ee58e` and `32d40dd0` in Ghidra or with `june2017_re/funcdis.py` (read-only). If one fails,
re-check the whole phase. Fix clear transcription errors under the same flag, then run `tools/build.py --deploy`. Hand design
decisions back to the user. Write "C6c - Review" under C6 in the plan, update the STATUS.md row, and commit. Never launch
X2_offline.exe; ask the user.
