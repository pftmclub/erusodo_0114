# June character Chung (Stage 7) - Phase CH1n3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1n3 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1n3 done (its Done note and commit) and the user's play-test of CH1n3 |
| Flag | `SERV_IRUHADEV_JUNE_USER_JUMP_UP_LINE` (every class) |
| Log tag | none new (body only) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1n3 - Done (2026-11-02)"
grep -a "^31fd6ce0" june2017_re/funcmap.tsv
grep -a "PhysicProcess_Jumping" tools/data/june_march_divergent.tsv tools/data/june_diverge_allow.tsv
```

## Task

Check CH1n3 against June (read-only; `python june2017_re/charport_adis.py chung 31fd6ce0`, read the listing, not the decompile). In order of risk:
- **The merged REL / ABS case:** the port folds June's two blocks (`31fd7022` type 4, `31fd6d86` type 5) into one. Confirm each side effect and its order
  per type: `m_bOnJumpUpLine` before the state tests; the four damage tests (vslots 0x114 / 0x118 / 0x494 / 0x490 -> 0x1e / 0x1f / 0x26 / 0x25) and
  the jump-down test (0x458 = `GetJumpDownStateID`); REL's direction (+0xe0: 2 = `LDO_RIGHT`, 1 = `LDO_LEFT`) in both branches and its second key-stop
  write outside the dungeon branch only; ABS's x flip when `bIsRight != 1`; no direction on ABS.
- **The repeat-touch guard:** `[0x3377585c] +0x8c == 0xb` (`XS_DUNGEON_GAME`) and `+0xe4 > 0`; `timeGetTime` (`winmm`, IAT `0x344c0178`); a new line
  id (`LineData +0`) stores id and time and returns; the same id: clear `+0xde8` only when key stop `<` 0.001 x elapsed (`test ah, 5; jp` skips on `>=`
  or unordered), then store the time. Both types reach the same tail (`31fd7254` / `31fd729d`).
- **CanDown** `31c6c310`: arguments ( pos, 30.0, &landPosition, &index, &index, 1, 0, 1, 0 ) = March's 4-argument `unsigned short*` overload under
  `SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_SIGNATURE`; y -= 33.0 = `LINE_RADIUS * 1.1f`.
- **Other foot lines:** `+0x88f0 = -1`, `+0x88f4 = 0`, `+0x8fc1 = 0`; airborne or a null line touches nothing. Ctor -1 / 0 (`@3204259a`).
- Scope fence: only `31fd6ce0`'s body; the Lu/Ciel getter bullet in Stage 8 is the one routed branch.

Hygiene: `flagcheck SERV_IRUHADEV_JUNE_USER_JUMP_UP_LINE --range <CH1n3 commit>^..<CH1n3 commit>`, `git diff --patience --numstat`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUUser.cpp`, a one-file probe of `X2GUUser.cpp`. Judge the user's play-test from the hand-off. Write "CH1n3 - Review" into the
plan doc, update the STATUS rows (CH1o becomes ready), check `kickoffs/char-chung/CH1o.md` and commit with explicit pathspecs.
