# June engine remainder - Phase W1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1b review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase W1b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE` |
| Log tag | `[JUNE_ENGINE] WORLD` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase W0 -- Done" "Phase W1b - AUTO_DISABLE_LINE" "Phase W1b -- Done"
grep -a "Engine W0\|Engine W1b" june2017_re/funcmap.tsv
```

## Task

Review Phase W1b's commit(s) against its own map (the funcmap rows it raised to `confirmed`) and the June addresses. Check:
- the `LineData` fields with their init / copy / clear;
- the tail's "on this line" test (`31c7bb00` arguments);
- `31c6b8e0`'s three returns;
- the user and NPC bookkeeping (the NPC ignores the return value);
- both physics gates line by line against `@320195be-32019a10` and `@32782d70-32782ede`;
- that `#else` is March's code.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE --range <first phase commit>^..HEAD`. Paste `files:`
/ `RESULT:`; explain every OTHER row and deleted line; a FAIL is a finding.

Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions to the user. Write "Phase W1b -- Review" into the plan doc, update the
STATUS row, check W2's kickoff, and commit.
