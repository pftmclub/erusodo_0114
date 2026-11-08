# June character Rena (Stage 7) - Phase CH1d2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1d2 review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1d2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; 8 enum members), `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (`//#define`; three lines in X3's arrows) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1d2 - Done"
awk -F'\t' '$7=="CH1d2"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1d - Review"
```

## Task

Review the 36 CH1d2 rows against June (`python june2017_re/charport_adis.py rena --phase CH1d2 --out <dir>`; stop each listing at its first `int3`). Spot-check at least:
- **Flag / event-time pairs and targets** (`ELSE_IF_STATE_CHANGE_ON_EX_STRUCT_OR_ANIMATION_END_`; event-time struct is 12 bytes from `+0x8c10`, so `[i].stateChange` is `+0x8c18 + 12 i`):
  - Dash Z1: 0 Z -> `LESI_DASH_COMBO_Z2` (June id 186), 1 X (class 24 / 114) -> `LESI_DASH_COMBO_RTR_ZX` (222), 2 X (class 13 / 103) -> `LESI_RSR_DASH_ZX` (267); direction clear covers flags 0-1 only (`CANCEL_COMBO_ON_DIRECTION_CHANGE( 2 )`, a word write).
  - Dash attack X: 0 Z (class 114) -> `LESI_RNW_DASH_XZ` (272). Double-jump landing: 0 Z (12 / 102) -> `LESI_COMBAT_RANGER_LESI_DOUBLE_JUMP_ATTACK_Z_UP`, 1 X (24 / 114) -> `LESI_RTR_DOUBLE_JUMP_ZX` (271).
  - Dash-jump X1: 0 X -> X2; 1 Z (102) -> `LESI_RWS_DASH_JUMP_XXZ_START` (261); 2 Z (103) -> `LESI_RGA_DASH_JUMP_XXZ` (269); 3 Z (114) -> `LESI_RNW_DASH_JUMP_XZ_START` (277). Dash-jump X2: 0 X -> X3; 1 Z (102) -> id 264 (`..._START_FROM_X2`, same script table as 261); 2 Z (114) -> `LESI_RNW_DASH_JUMP_XXZ_START` (280).
- **The cancel chain** in Dash Z1 / Z2 / dash attack X (skill, walk, back-walk, dash, foot test each; no Z / X cancel), and `m_fCanNotAttackTime` 0.2 in Dash Z1 only.
- **The arrow functions**: dash attack X (0.3 s, MP 4 / 9 for a Night Watcher, `posR.y -= 20`, `angle.y += 180`, `DamageEffect_Rena_Dash_Combo_X_Arrow`), jump attack X (0.21 s, MP 4, `DamageEffect_Rena_Jump_Combo_X_Arrow`), dash-jump X1 (0.23 s, MP 4; Night Watcher at `Bip01` + 30 / + 20, Sniping Ranger / Grand Archer `_RSR_`, else `_Rena_`), X2 (0.2 s, MP 6), X3 (0.233 s, MP 8, three arrows `degree.z`, `-20`, `-20`, NF on each, not-enough-MP `-25`); the `ARROW_DEGREE_Z` key (`xorstrings.tsv`), the `GetMainEffect` max-life tail, Night Watcher `+0xb04` / `+0xb00` order (type, then index).
- **The two FrameMoveFuture changes**: jump attack X (`y = 0` for the whole first 0.26 s, `x = 0` from 0.13 s, walk key only after 0.26 s) and dash-jump X3 (a bare thunk).
- **Enum**: eight `_CORE` members appended after `LESI_RCR_COMBO_XXZ_FROM_X2` (June script ids 261 / 264 / 267 / 269 / 271 / 272 / 277 / 280; `ch0_state_enum_names.tsv` has no `Enum.lua` value for any); CH1e / f / g own the states, CH1m maps the ids.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1d2 commit>^..<CH1d2 commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place (bytepatch only). Write "CH1d2 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1e.md`, commit with explicit pathspecs.
