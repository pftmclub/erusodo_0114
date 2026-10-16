# June skill system core - Phase S0b - Slots, pages and UI RE

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S0b` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | - (read-only; runs alongside anything, incl. S0a and S1) |
| Flag | none (read-only, no code, no build) |
| Log tag | none (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | none (RE phase; the S5-S8 reviews check its map) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Investigation ledger (planning, 2026-10-17)" "Phase S0b - Slots, pages and UI RE"
```

## Task

Do "Phase S0b" items 1-6:

- June's `KUnitSkillData` / `KUserSkillPageData` wire layout, as the exact S5 struct spec.
- Every June slot-index reader (`3288fff0`, `3214f3d0`, `32890070`, `3288fe00`, `32896600`, quick keys, gage), and how TC is locked.
- Skill pages in June.
- The slot and tree UI: June's DLG names, the UI class function list, and the framework methods they need against june-engine U0 / U1.
- The pointer model, for S8.
- A keep / retire recommendation for `SKILLTREE_NO_LOCK` / `SKILL_SLOT_B_FREE`. Ask the user.

Output:

- `june2017_re/s0b_slot_sites.tsv`: every March slot site, its June equivalent and the S5 action;
- funcmap rows tagged `Skill S0b`;
- amendments in `S5.md` to `S8.md`.
