# June skill system core - Phase Z - Close-out

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase skill-system-core Z` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S0a-S8 done or routed |
| Flag | none (turns `SERV_IRUHADEV_JUNE_SKILL_DEBUG` off unless a phase kept it) |
| Log tag | `[JUNE_SKILL]` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Investigation ledger (planning, 2026-10-17)" "Phase Z - Close-out"
```

## Task

Do "Phase Z".

- `MODS.md` lines for every `SERV_IRUHADEV_JUNE_SKILL_*` flag, with revert steps (the exe + data036 + save rule).
- A dated note in `JUNE_PORT_BACKLOG_ORDER.md` Stage 3.
- The checklist rows from S3 and S4, confirmed.
- Memory updated.
- `_DEBUG` off; build and deploy if that changes anything.

**`SERV_IRUHADEV_JUNE_SKILL_DEBUG` (carried by the S8 review, 2026-10-19):** the S7e review allowed retiring it only after a play-test had shown the S3d
`[JUNE_SKILL] MEMO effectset` / `damageeffect` / `attack box` lines. At the S8 review they still had not run. First check the logs
(`python tools/logs.py --grep "JUNE_SKILL] MEMO"`). If they still have not run, say so in the Done note, then remove the define and every
`#if defined( SERV_IRUHADEV_JUNE_SKILL_DEBUG ) ...` block (`git grep`). They are diagnostics only, so removing them changes no behaviour.
The S8 `PTR snapshot` lines cannot run offline (S8 Done note).
