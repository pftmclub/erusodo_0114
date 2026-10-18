# June engine remainder - Phase R4j - Static colour-quad list (June-only)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (RE first, then the port if March can reach it); type `/x2-phase june-engine R4j` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R4e (the static's `OnFrameRender( pass )`) |
| Flag | `SERV_IRUHADEV_JUNE_UI_STATIC_QUADS` (proposed) |
| Log tag | `[JUNE_ENGINE] SQUAD` |
| Review | `kickoffs/june-engine/R4j-review.md` (the phase writes it if it ports code) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Phase R4e -- Done" "Phase R4j - Static colour-quad list (June-only)"
```

## Task

Do "Phase R4j". June's `CKTDGUIStatic` render `31cbec10` walks a vector at `+0x268` in every pass (`@31cbee6a-31cbeed1`), between the
pictures and the strings. March has no such member. Each entry is placed by `31c9b660` and drawn by `31c954a0` -> `31c94e70` (type 2, 4
vertices, untextured).

- Find the writer(s) of `+0x268` (xrefs to the push-back, a `CKTDGUIStatic` Lua `class_def` entry, the XOR string table
  `june2017_re/xorstrings.tsv`) and the entry's type and layout.
- Check whether any June script or dialog uses it (`june_src_fixed/`, `python -I june2017_re/ui_dlg_census.py`).
- If nothing March loads can create an entry, write that down with the evidence and close the phase (no code). Otherwise port the
  member, its filler and the draw, behind the flag. Gate: build + deploy; UI unchanged (or the dialog that uses it shows its quads).
