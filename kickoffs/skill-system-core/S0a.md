# June skill system core - Phase S0a - Templet and tree RE

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S0a` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs alongside anything) |
| Flag | none (read-only, no code, no build) |
| Log tag | none (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | none (RE phase; the S2 and S3 reviews check its map) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Investigation ledger (planning, 2026-10-17)" "Phase S0a - Templet and tree RE"
```

## Task

Do "Phase S0a" items 1-6.

- Map June's `AddSkillTemplet_LUA 32216320`: every key goes to `june2017_re/s0a_templet_keys.tsv`. Also map the tree, guild-tree and post-process parsers.
- Compare `CX2SkillTree` / `CX2UserSkillTree`, June vs March, function by function.
- Give June's US load list and the skill-keyed data closure (`june2017_re/s0a_skill_data_closure.tsv`).
- Give the learn / SP / default-skill rules S3 needs, the save census, and S4's numerical transcription spec.

Funcmap rows are tagged `Skill S0a`. The phase is read-only: Ghidra, greps, `elsdb.py q` (no writes). Write the amendments into `kickoffs/skill-system-core/S2.md`, `S3.md` and `S4.md`, each under an "S0a amendments" section.

A server-only value with no June client source gets June's default where the value is absent. Otherwise propose a labelled stand-in to the user as an open question, never a guess. The Done gate is as the section says.
