# June buff port - Phase 15 review (enchant system parity, steps 2 + 2b)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 15 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 15 step 2 (`835b5ef`, `5d8f827`, `6d304a1`) and step 2b (its commit is in the 2b Done note); the 2b play-test, if the user has run it |
| Flag | `SERV_IRUHADEV_JUNE_ENCHANT_PARITY` (debug lines: `SERV_IRUHADEV_JUNE_BUFF_DEBUG`) |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 15, step 1 — RE (2026-10-04), read-only; step 2 not started" "Phase 15, step 2 — chunks A-C (2026-10-07)" "Phase 15, step 2b — chunk D Done (2026-10-07)"
```

The funcmap rows prefixed "Buff Phase 15 step 2" and "Buff Phase 15 step 2b" are the address map.

## Task

Review Phase 15 (steps 2 and 2b) against June:
- the EDT renumbering and the compose/decompose tables (chunk A);
- the EnchantData loader and the six bodies (B1, C);
- 2b's tick helpers in `X2GameUnit.cpp`. Check the arithmetic and float order of the slow values, the shock r, the part choice (first / second-else-first / user map) at each site, and the rose-flag clear;
- the host sites in `X2GUUser.cpp` / `X2GUNPC.cpp` (the shock debuff placement and the MP-drain gate);
- the GetFinalDamage piercing term and the frozen tiers;
- `USE_OWNERS_ENCHANT`: the `GetWeaponEnchantData` NPC branch and the 2 `X2EnchantItem.cpp` sites. Check that no other March caller of `GetEnchantExtraDamageType()` corresponds to a June slot-111 call that an owner-enchant NPC reaches.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_ENCHANT_PARITY --range 835b5ef^..HEAD --with SERV_IRUHADEV_JUNE_BUFF_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line.

Spot-check in Ghidra or with `june2017_re/disasm_range.py --x87`:
- the slow value chain at `@32067e82..@32067fa5`;
- the shock values at `@32068c22..@32068c93`;
- the piercing branch at `@3228660a..@322866e4`.

If one fails, re-check the whole host. Fix clear transcription errors under the flag, then run `tools/build.py --deploy` and hand design decisions back. If the 2b play-test has run, read it with `python tools/logs.py --tag JUNE_BUFF` (this phase's lines only).
