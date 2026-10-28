# One function per name - Phase D1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence D1 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | D1 done |
| Flag | `SERV_IRUHADEV_JUNE_DIVERGE_*` |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase D1 - Replace divergent March functions at the definition (Opus 5.5, high)"
```

and D1's Done note, `june_march_divergent.tsv`.

## Task

Review D1: each replaced body against its June listing, `#else` byte-intact, the live March branch (`cl /P`), one unported call site
per row, guards, `MODS.md` / `JUNE_BUGS.md` rows, the play-test logs.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out), plus the flag-off
equivalence the C0 tool reports. Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding. Fix clear errors under the same flag (then `tools/build.py --deploy`); hand design
decisions to the user. Write the review section into the plan doc, update STATUS, check the next kickoff, commit.
