# June enum names - Phase NZ - Close

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-enum-names NZ` |
| Plan doc | `JUNE_ENUM_NAMES_PHASES.md` (branch `main`) |
| Depends on | N3, N4, N5a, N5b, N5c done |
| Flag | `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none |
| Review | - |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENUM_NAMES_PHASES.md "Starting context" "Rules" "Phase NZ - Close (Opus)" "Play-test hand-off (once, at NZ)"
```

## Task

Close the plan per the phase section. Write the standing rule (user said yes, 2026-10-26: no hardcoded enum literals) into `CLAUDE.md`, `kickoffs/TEMPLATE.md` and the x2-phase skill, wording in the plan doc; then amend char-eve's Rules line so the paused plan resumes under it. Delegate the scanner re-run, `flagcheck --range`, build + deploy and the log read to subagents; keep the review of N2..N5c against the tsv. Finish with the play-test hand-off in the plan doc.

## Carried from N2 (user, 2026-10-26)

- **Pack and copy `data036` at NZ, before the smoke test.** N2 added 19 lines to the live `Enum.lua` (14 `SKILL_ID`, 5 `NPC_UNIT_ID`) that are not packed yet. NZ runs `/elstools pack data036` itself, asks before `/x2-kom-packed copy data036`, and puts the 19 names in the play-test hand-off (a shipped `.lua` now indexes them: `NPCStat.lua` data037, `NPCStat_INT.lua` datai016, `NewSkillTempletVer2.lua`, `Arme_VioletMage.lua`, `PVP_HERO_HINO.lua`). Look for a spawn or skill-load error in the first dungeon.
