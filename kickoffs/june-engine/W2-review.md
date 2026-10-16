# June engine remainder - Phase W2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W2 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase W2 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_PORTAL_GATE` |
| Log tag | `[JUNE_ENGINE] WORLD` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase W0 -- Done" "Phase W2 - Portal-line markers" "Phase W2 -- Done"
grep -a "Engine W0\|Engine W2" june2017_re/funcmap.tsv
```

## Task

Review Phase W2's commit(s) against its map and the June addresses. Check:
- `322e5820`'s position (0.5 / 3.0), names and handle push;
- the `+0x7bc` lifecycle;
- the `StageLoad` gate (`LT_POTAL`, `SHOW_PORTAL == 1`, `LINE_PORTAL_TYPE != 2`) and the type-2 particles;
- the key names and defaults;
- the `DoTriggerLine` case-7 call;
- that `#else` keeps March's `PORTAL_LINEMAP_TEST` block;
- any routing note to Stage 19.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_WORLD_PORTAL_GATE --range <first phase commit>^..HEAD`. Paste `files:` /
`RESULT:`; explain every OTHER row and deleted line; a FAIL is a finding.

Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions to the user. Write "Phase W2 -- Review" into the plan doc, update the
STATUS row, check Z's kickoff (W1b / W2 are now in Z's "every phase done"), and commit.
