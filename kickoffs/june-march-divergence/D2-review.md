# March behaviour inside June code - Phase D2 review and close

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence D2 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | Phase D2 done (its Done note and commits) |
| Flag | the flags D2's sweep touched |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase D2 - The idiom gate and the sweep (Sonnet 5.5, high)" "Phase D2 - Review and close (Opus 5.5, high)"
```

and D2's Done note, `tools/data/june_diverge_allow.tsv`.

## Task

Review D2: run `python tools/junediverge.py --selftest` and `--check` over `X2Lib/*.cpp X2Lib/Offline/*.cpp KTDXLIB/*.cpp` yourself; check
every sweep fix and every allow-list reason against its June address; read the wiring lines (x2-phase Start step 5 and Finish step 1, the
Stage 7 template, `tools/README.md`). Hygiene per touched flag:
`python tools/flagcheck.py <FLAG> --range <first D2 commit>^..<last D2 commit>`; paste the summary lines, explain OTHER rows and deletions.

Then close the plan ("Phase D2 - Review and close"): `MODS.md` section, backlog Stage 35 closing note, STATUS, and **the standing rule** the
user asked for (2026-10-28): the `CLAUDE.md` rule section, the `JUNE_PORT_BACKLOG_ORDER.md` "Standing rules" line, the `kickoffs/TEMPLATE.md`
line, and a memory file with its `MEMORY.md` pointer, worded as the plan section gives it. Commit.
