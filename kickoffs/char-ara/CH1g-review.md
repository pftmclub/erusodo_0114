# June character Ara (Stage 7) - Phase CH1g review (covers CH1e-CH1g)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1g review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1e, CH1f and CH1g done (their Done notes and commits) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`//#define`), `_CORE` (defined), `_FP` (`//#define`), `SERV_IRUHADEV_JUNE_USER_LANDING_PREDICTION` (defined by CH1g) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1e - Done" "CH1f - Done" "CH1g - Done"
awk -F'\t' '$7=="CH1e" || $7=="CH1f" || $7=="CH1g"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Review the functions of CH1e (13), CH1f (20) and CH1g (28) against June (`python june2017_re/charport_adis.py ara <va>...`; the listing carries the `StateChange` arguments and the `fcomp` / `test ah` direction; `je` after `cmp byte, 1` jumps when the byte IS 1). Spot-check at least:
- **CH1g EventProcess shapes:** `32b9edd0` (FP-cancel lines beside CH1d's `32b99890`), `32b9f5b0` (X as Asura -> AA ZXX, class 0x7c), `32b9fce0` (the Loop's count `+0x9738` < 5, flag 0 on Z, flag 1 + LoopX on X as Asura, no Z / X attack cancel), `32ba53a0` / `32ba54d0` / `32ba5600` (the dead walk / dash cancels, JUNE_BUGS 590).
- **CH1g FrameMoves:** the `0.f < m_fBuffHyperModeDamageRate` rate rule (`fcomp [esi+0x9744]` + `jp`), `IsHyperState()` as the fourth `PlayEffectSet` argument (the byte at `[state data + 0x10]`), the black hole handle `+0x9734` stored at `32bbc590` and stopped at `32bbcb91`, the 11 effect-set names and times against the string table.
- **`32bc0bb0`** (`CreateDashJumpXZBullet`): MP 4, the finger bones, no passive scaling, the 23 `CreateInstance` arguments against March's defaults (the CH1f `32bbfde0` listing has the same push sequence).
- **The `LandingPrediction` hoist** (`SERV_IRUHADEV_JUNE_USER_LANDING_PREDICTION`): `X2GUUser.cpp` body = Arme's old body; Arme's declaration and definition guarded by `#ifndef`; Arme's `AVSI_ABM_DASH_JUMP_ZX_StartFuture` still compiles and calls the base.
- **CH1e / CH1f:** the `_CORE` / `_LIVE` macro pairs, JUNE_BUGS 588 / 589, the `CreateEnergeBullet` force-down assignment.
- `june2017_re/char_ara/ch1_offsets.tsv` rows `0x9734` / `0x9738` / `0x9744` against the listings; the three new `_CORE` members of `X2GUAra.h` (CH1h places them and confirms June's ctor values).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_CORE SERV_IRUHADEV_JUNE_CHAR_ARA_FP --range <CH1e commit>^..<CH1g commit>` (FAIL only for the commented-out `#define`); the same for `SERV_IRUHADEV_JUNE_USER_LANDING_PREDICTION`; `python tools/junediverge.py --check-all`; one-file `/Zs` probe of `X2GUAra.cpp` off and with `/D SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`. Fix clear transcription errors in place (bytepatch only). Write "CH1g - Review" into the plan doc, update the STATUS rows (CH1g done, CH1h ready), check `kickoffs/char-ara/CH1h.md`, commit with explicit pathspecs.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1h` (Sonnet 5.5 high).
