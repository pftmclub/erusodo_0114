# June port backlog - Stage 30 planning - June resonance system

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-resonance` |
| Output | plan key `june-resonance` (`kickoffs/june-resonance/`, a `june-resonance` STATUS block) |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 30), output `JUNE_RESONANCE_PHASES.md` (new) |
| Depends on | - (planning is read-only; code waits for its turn in the priority order, after Stage 9, unless it blocks play) |
| Flag | one family, e.g. `SERV_IRUHADEV_JUNE_RESONANCE_*` (none in this planning session: no code) |
| Log tag | `JUNE_RESONANCE` (propose it in the plan) |
| Review | none (a planning session); each planned phase gets its own review kickoff |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 30: June resonance system (june-transcend planning, 2026-10-22)" "Standing rules (every stage, every kickoff)"
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "R0 - Done (2026-10-22, Opus 5.5 high; read-only, no code, no build)"
grep -n "Transcend S6" june2017_re/funcmap.tsv
```

## Task

Identify June's resonance system (found by june-result R0 and june-transcend planning) and plan its port. Do the RE read-only in Ghidra.
- **The unit side.** June's `KUnitInfo` ends with five June-only fields. `SetKUnitInfo` `3235eb20` copies packet dwords `0xc9..0xcd` into
  CX2Unit `+0xa99` (int), `+0xa9d` / `+0xaa1` (an int64 EXP), `+0xaa5` (int) and the byte `+0xaa9`.
- **The setter.** `3234b3e0`, called from `3260c140` / `3260c5d0` / `3260ca30`: name those packets.
- **The gate.** `3247b460` = unit level > 98 and byte `+0xaa9`.
- **The EXP.** `322f8920` adds to the int64 (callers `3230b7b0`, `32d53530`).
- **The readers.**
  - the dungeon result `324d60e0` (`resonanceLevel`, the EXP bars);
  - `324b0f80`, `32529f90`, `32473810`, `32476050`, `3260a360`;
  - quest `3232e510` (reads `+0xaa9`);
  - UI `328b3810`, `3291df90`, `31d8e280`, `32468c70`, `32356640`.
- **The rest.** What resonance gives (stats? a buff? points to spend?), its data file(s), its UI, and the emulator / save it needs (a
  `unit` column rung, `KUnitInfo` fields offline-only). There is no March counterpart.

Output:
- `JUNE_RESONANCE_PHASES.md` (map phase first, Opus review each);
- one kickoff per phase in `kickoffs/june-resonance/`;
- a STATUS block;
- funcmap rows tagged `Resonance S30`.

Play-test on the level-99 units (51-54 Aisha, 48 Ara, 49 Raven, 50 Chung). Open design questions go to the user at the end, each with a
recommendation.
