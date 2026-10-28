# One function per name - Phase C3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence C3 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | C3 done |
| Flag | the flags batch C3 touched |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase C3 - Collapse the shared unit code (Opus 5.5, high)"
```

and C3's Done note, `guards.tsv`.

## Task

Review C3: each merge against its June address, each guard (needed, minimal, one line, in `guards.tsv`), the reach points, the smoke logs.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out), plus the flag-off
equivalence the C0 tool reports. Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding. Fix clear errors under the same flag (then `tools/build.py --deploy`); hand design
decisions to the user. Write the review section into the plan doc, update STATUS, check the next kickoff, commit.
