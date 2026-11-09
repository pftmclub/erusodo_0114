# June port backlog - Stage 38 planning - June village / field game core

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-field` |
| Output | plan key `june-field` (`kickoffs/june-field/`, a `june-field` STATUS block), plan doc `JUNE_FIELD_PHASES.md` (new) |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 38) |
| Depends on | - (planning is read-only; code waits for its turn in the priority order, after Stage 9) |
| Flag | one family, e.g. `SERV_IRUHADEV_JUNE_FIELD_*` (none in this planning session: no code) |
| Log tag | `JUNE_FIELD` (propose it in the plan) |
| Review | none (a planning session); each planned phase gets its own review kickoff |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 38: June village / field game core (june-census V2, 2026-11-09)" "Stage 24: June spa villages (atlas E1d, 2026-10-17)" "Standing rules (every stage, every kickoff)"
grep -n -E "TFieldGame|SquareUnit" june2017_re/funcmap.tsv
```

and the `S38` rows of `june2017_re/census/exclusions.tsv`.

## Task

The census found June code of the village / field game that no stage or plan names: `CX2TFieldGame` (class 72 + file 49 unowned,
`0x323a4ea0`-`0x323bd4e0`) and `CX2SquareUnit` (46, `0x323ed330`-`0x323fa4b0`; strings `PASSIVE_SPEED_X`, `RELOAD_ACCEL`, `G_ACCEL`).
Atlas (Stage 1) and Stage 24 touched `X2TFieldGame.cpp` for entrance lists only; Stage 18 owns `CX2TFieldNpc`.
1. Map every June function of the two classes against March (`X2Lib/X2TFieldGame.cpp`, `X2Lib/X2SquareUnit.cpp`): June / March / differs.
2. Check the triage gaps next to them once Stage 40 has run (`gap:X2GUNPC..X2TFieldGame@0x32394340` 191, `gap:X2UIQuestNew..X2SquareUnit@0x323e60b0` 103);
   take only what Stage 40 routes here.
3. Plan the port by behaviour (field movement / sync of other units, village state, field UI hooks), offline reach of each piece, and the
   play-test path (a village, the field next to it).

Write the plan in the x2-phase `new-plan` shape, mark this brief's STATUS row done, commit with explicit pathspecs.
