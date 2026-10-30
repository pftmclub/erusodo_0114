# One function per name - Phase D2 review and close

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence D2 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | D2 and Phase S (the smoke, `Smoke.md`) done |
| Flag | the flags the sweep touched |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase D2 - The gate and the wiring (Sonnet 5.5, high)" "Phase D2 - Review and close (Opus 5.5, high)" "Phase S Done (2026-10-30, Opus 5.5)"
```

and D2's Done note, `tools/data/june_diverge_allow.tsv`.

## Task

Review D2 and read Phase S's Done note (every smoke failure fixed or routed; check fix 1 `OpenSoundInBackground` and fix 2's collision stand-in against June, and triage its lead: June's `PlaySound` adds to the close list with no `bLoop == false` test): `junediverge.py --selftest` and `--check` over X2Lib / KTDXLIB yourself, every sweep fix and allow-list reason, the wiring.
Then close the plan ("Phase D2 - Review and close"): `MODS.md`, backlog Stage 35 closing note, STATUS, and the standing-rule additions
the section names (CLAUDE.md gate + table rule, backlog "Standing rules", `kickoffs/TEMPLATE.md`, memory).

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out), plus the flag-off
equivalence the C0 tool reports. Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding. Fix clear errors under the same flag (then `tools/build.py --deploy`); hand design
decisions to the user. Write the review section into the plan doc, update STATUS, check the next kickoff, commit.
