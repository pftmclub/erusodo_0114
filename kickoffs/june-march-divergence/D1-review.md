# March behaviour inside June code - Phase D1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence D1 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | Phase D1 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_DIVERGE_*` |
| Log tag | none (D1's, if it added one) |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase D1 - Replace at the definition (Opus 5.5, high)" "Phase D1 - Review (Opus 5.5, high)"
```

and D1's Done note (`--list`, then its "Done" section), `tools/data/june_march_divergent.tsv`.

## Task

Review D1's commits: each replaced body against its June listing in Ghidra, the `#else` bodies byte-intact, the live March branch of each
definition (`cl /P`), one unported March call site per row (still compiles into the replaced function, nothing crashes on the June answer),
the `MODS.md` lines and `JUNE_BUGS.md` rows, and the play-test logs.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit of that flag>^..<its last commit>` (a `..HEAD` range can time out).
Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); a row that cannot stay `replace` moves to `site` with
its reason, for D2. Write "Phase D1 - Review" into the plan doc, update STATUS, check `D2.md`, commit.
