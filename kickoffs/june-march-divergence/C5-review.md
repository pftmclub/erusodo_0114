# One function per name - Phase C5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence C5 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | C5 done |
| Flag | existing flags |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase C5 - Members, locals, constants, namespaces (Sonnet 5.5, high)"
```

and C5's Done note.

## Task

Review C5: member merges (same type and meaning), names against context, enum aliases, the census at 0, the smoke logs.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out), plus the flag-off
equivalence the C0 tool reports. Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding. Fix clear errors under the same flag (then `tools/build.py --deploy`); hand design
decisions to the user. Write the review section into the plan doc, update STATUS, check the next kickoff, commit.
