# One function per name - Phase C4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence C4 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | C4 done |
| Flag | the flags batch C4 touched |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase C4 - Collapse UI, data, engine, offline; class, file and Lua names (Sonnet 5.5, high)"
```

and C4's Done note.

## Task

Review C4: merges and renames, the vcxproj edit (builds from a clean PCH), the Lua names against every caller, the smoke logs.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out), plus the flag-off
equivalence the C0 tool reports. Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding. Fix clear errors under the same flag (then `tools/build.py --deploy`); hand design
decisions to the user. Write the review section into the plan doc, update STATUS, check the next kickoff, commit.
