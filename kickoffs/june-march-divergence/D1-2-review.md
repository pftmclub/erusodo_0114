# One function per name - Phase D1-2 review (covers D1-1, D1-2 and D1-2b)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence D1-2 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | D1-2b done |
| Flag | the `SERV_IRUHADEV_JUNE_DIVERGE_*` flags of D1-1, D1-2 and D1-2b |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Rules" "D1 split (2026-10-29, D1-1 session)"
```

and the Done notes of D1-1, D1-2 and D1-2b (`docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md --list` names them), `june_march_divergent.tsv`.

## Task

Review D1-1, D1-2 and D1-2b: each replaced body against its June listing (Ghidra / `funcdis.py`), `#else` byte-intact, the live March branch (`cl /P`),
one unported call site per row, guards, `MODS.md` / `JUNE_BUGS.md` rows, stand-ins named and routed.
Also place the three branches D1-2b found (`JUNE_PORT_BACKLOG_ORDER.md` Stage 35, "Found by D1-2b"): the black-out `EDT_BLIND` / `DARK_CLOUD`
cases, the unit list `+0x3b84`, `NomalDirectCamera`'s height-compensation chain: a D1 row each, or their own stage.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out). Paste the `files:` /
`in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding. Fix clear errors
under the same flag (then `tools/build.py --deploy`); hand design decisions to the user. No play-test (the smoke runs at the plan end).
Write the review section into the plan doc, update STATUS, check `D1-3.md`, commit.
