# Atlas/Elysion port - Phase E4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E4 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E4 done (its Done note and commit); best after the E4 play-test |
| Flag | `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION` (+ `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` (`python tools/logs.py --tag ELYDBG`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E4 — Local map, Elysion page" "Phase E4 — Done" "Phase A7 — Review (2026-10-17)"
python tools/docsection.py JUNE_UI_PARITY_PHASES.md 'Function map: `CX2LocalMapUI`'
```

The June rows E4 added are at the end of `june2017_re/funcmap.tsv` (grep `Atlas E4`). Use the ghidra skill (read-only).
Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review E4's commit against June, function by function (the Done note names each address):
- **Scroll `329378a0`:** argument order of `StartChangeUV( dir, from, to )` (pushes @32937959-32937965: to = the arg-1 slot, from = a local, then dir), +0x54 written at the
  start, the end callback bound to `this`; the `UpdateDungeonButtons` tail @3293c146 (`NEG / SBB / ADD 2` = page ? 1 : 2) and `ClearLocalMapDLGs`
  329379d0 (direction 2, then `+0x60 = 1`).
- **`329366a0` / `3293c1e0`:** the ten page strings, `SetShowEnable` args (0,0 vs 1,1), the hero-recruit hide, the 0..10 test that does not return,
  `UpdateDungeonButtons( +0x48 )` then `UpdateHeroRecruitButton`.
- **`OnFrameMove` 3293a960:** +0xc1 = `m_bMouseOver` (dialog OnMouseMove @31cd2457), the branch order (Up hovered, Down hovered, blink), 0.3f and
  the `1.0 <` test; vslot 0x84 = `SetColor`.
- **Guide 329386b0 / 18008:** `checkfile.txt` "rb" / "wb", ctor args (stage, name, 0.07f, layer 0), 0x472 = the dialog-delete message, the dtor
  delete (32937e10).
- **Code-event gates @3293b8c0 / @3293baac:** ranges 33330..33352 (2260) and 33350..33352 (2261, only when 2260 is off), the title static's name
  (334c83ac, the C++ escapes), pictures 0/1 vs 2; the not-enabled path continues either way.
- **Data:** `DLG_Elysion_LocalMap_Guide.lua` is byte-equal to `ljconv(June)` (`june2017_re/june_adopt_20261017_115410.tsv`); its textures and
  `STR_ID_40385` resolve; census `--phase E4` leaves only `_01_A` (JUNE_BUGS 206) and the 3 Hero members on the local-map rows.
- **JUNE_BUGS 228 / 229** are right (Atlas page has no star statics in June's Icon lua; the guide's marker is per install).
- **Play-test result** (the user's report and the `[ELYDBG]` lines): scroll both ways, each page shows only its own controls, guide opens once and
  closes, buttons blink and go solid under the mouse.

Hygiene: `python tools/flagcheck.py JUNE_LOCAL_MAP_ELYSION --range <E4 commit>^..<E4 commit> --with ELYSION_DEBUG --files X2Lib/X2LocalMapUI.cpp X2Lib/X2LocalMapUI.h KTDXLIB/Always.h`.
Explain the deleted lines (A7's own flagged lines). Spot-check 2-3 items in Ghidra; if one fails, re-check the whole phase. Fix clear
transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase E4 — Review"
into the plan doc, update the E4 STATUS row, write `kickoffs/atlas/E5.md` if missing (E5 = moving platforms; Opus 5.5 / high), and commit.
