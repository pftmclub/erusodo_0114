# June skill system core - Phase S7g - The HUD stat singleton and the cash shop's open-at-item list

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7g` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7d (it found both pieces); not alongside S7e / S7f if they touch the same handlers |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_*` |
| Log tag | `[JUNE_SKILL] HUD` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7g-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7d - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7d` (`3235a160`, `321cd390`, `31f08290`) and the C9h-3 row for `324943c0`.

## Task

Added by S7d (2026-10-19, unowned-branch rule). Two June-only systems S7d found and did not port. Identify first, then port; design
questions go to the user.

1. **The HUD stat singleton `324943c0`.** `CX2Unit 3235a160` (23 callers: the INIT ACK `3220ea50`, the grant NOT `323b0770`, the `0x735`
   handler `31f1fd90`, ...) writes stat totals keyed by unit uid into it (`3248a800( uid, value, idx )`, `3248d3a0( uid, value )`); the C9h-3
   row says it is 0xd4 bytes, has no vtable and sits beside `CX2GageManager` code (`323fea90` reads rate floats through its `+0`
   pointer). Name it from its readers (who reads what `3248a800` stores, and what the screen shows from it), say whether March has the
   same display somewhere else, and port it or record why not.
2. **The cash shop's open-at-item list (`+0x338`).** `321cd390` pushes an item id into a June-only `vector<int>` of `CX2CashShop` before
   `ToggleCashShop`, from six callers (`328ec3ff` the page button with 127273, `3239814b`, `3247840a`, `324a6fb1`, `3260d7f7`, `3260dfe7`).
   Find the reader (the shop's open path), say what the player sees (the shop opening on that item?), and port it with all six call
   sites that March has.

**Play-test:** whatever the identification shows. For item 2, press the skill tree's page button in a village and check the shop opens on
"Skill Tree Page Expansion".
