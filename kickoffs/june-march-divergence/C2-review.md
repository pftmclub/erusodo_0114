# One function per name - Phase C2 review (covers C1 and C2)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence C2 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | C1 and C2 done |
| Flag | the Eve and Aisha flags the batches touched |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase C1 - Collapse Eve (Sonnet 5.5, high)" "Phase C2 - Collapse Aisha (Sonnet 5.5, high)" "Phase C2 - Review (covers C1 and C2) (Opus 5.5, high)"
```

and the C1 / C2 Done notes, `collapse_map.tsv` batches `C1` / `C2`.

## Task

Review C1 and C2: every merge is a true twin (spot-check 10 per class against the June address in Ghidra), every hand shape, every
rename against the class pattern and the base classes, the reach lists, the smoke logs.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out), plus the flag-off
equivalence the C0 tool reports. Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding. Fix clear errors under the same flag (then `tools/build.py --deploy`); hand design
decisions to the user. Write the review section into the plan doc, update STATUS, check the next kickoff, commit.
