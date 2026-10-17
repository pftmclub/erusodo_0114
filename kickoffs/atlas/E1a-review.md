# Atlas/Elysion port - Phase E1a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E1a review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E1a done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_ELYSION_VILLAGE` |
| Log tag | `[ELYDBG]` (and the permanent `[ELYSION]` load line) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E1 — Village engine" "Phase E1 — Split (user, 2026-10-17)" "Phase E1a — Done"
python tools/docsection.py june2017_re/e1_relation_runtime.md "June HouseTemplet (0x618) offsets -> key" "Relation row (0x388; ctor 32347110)" "Lookups (return int; out = { vector<Row> rows; vector<Small> smalls; })"
cat june2017_re/e1_houseparsing_diff.md
```

## Task

Review Phase E1a's commit(s) against June's binary:
- **HouseParsing** (`324f3260`, raw disassembly only: Ghidra cannot decompile it): every June key in June's order, its default
  on a miss, its conversion (string id -> text, enum, table), the sub-tables (CUSTOM_NPC_LOACION, Main_Talk, NPC_COMMON_TALK,
  Talk_Change_Condition, CUSTOM_MOTION_NAME, RenderOption), March keys June dropped, and the failure path.
- **Relation loaders** `3233b680` / `32340720`: the type branch (NOT_SHOW only in ByQuest), each key's default, the >= 0 gates
  before the push, the shared change keys, the RenderOption defaults (the row-default choice is a JUNE_BUGS row: check it).
- **Lookups** `32340320` / `323446c0` / `32340060`: gate, completed-quest test, APPLY_FORCE, the best-quest comparison (smalls
  compare against the best row's quest), which result pushes what into the applied lists.
- The load point (`3258c470 @3259174c`, after PVPData / PVEData) and the bindings.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_ELYSION_VILLAGE --range <first E1a commit>^..<last> --with SERV_IRUHADEV_ELYSION_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line.

Spot-check 2-3 keys and one lookup branch directly in raw disassembly; if one fails, re-check the whole phase. Fix clear
transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Phase E1a — Review" into the plan doc, update the STATUS row, check `kickoffs/atlas/E1b.md`, and commit.
