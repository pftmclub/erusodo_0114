# June character Chung (Stage 7) - Phase CH1j review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1j review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1j done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1j - Done (2026-11-01)"
awk -F'	' '$7=="CH1j"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1j is 41 functions: 36 `inside` and 5 `new` (the Tactical Trooper mobile-armor line, Z Z X / Z Z X X / dash X X, and the dash jump X X state). Read them against June (`python june2017_re/charport_adis.py chung --phase CH1j --out <scratch dir>`; the listings run on past a function's end). Check these:
- **The nine that differ** (Done note table): the Loop / Forward / Backward EventProcesses (`32bece90` / `32bec410` / `32bec950`: MP drain x `SA_MP_CONSUME_REL` of `RNSI_P_CTT_OPEN_MIND` 26419, flag index and `m_fEventTime` index of each of the four flags, the animation-end term, `SKILL_CANCEL_AFTER( +0x8c90 )`, the key tests of the three states), Ready EP `32be6fe0` and Z Z X EP `32be6c50` (animation-end term), X StartFuture `32bdac90` (0.8), Loop FM `32bf07b0` and X FM `32c108d0` (spine aim, extra cannon ball, `CreateCannonBulletDamageEffect( 2, power x +0x97a8 )`).
- **The five `new` rows**: `32bda860` (`m_PhysicParam.passiveSpeed.x` = unit +0x30c x `SA_MOVE_SPEED`), `32bf00c0` (arguments, folds, `+0x8a68`; against CH1j0's review), `32c09e40` (types 0 / 1 / 2, bone, range rate clamp to >= 1, the rotation test: `|x|`, `|y|` <= `FLT_EPSILON` and `IsSamef( 0, z )`), `32bf02c0`, `32c0a5b0`.
- **Open item for the review to settle**: Loop FM's gate `g_pMain +0x1a0 == 0` (June `32bf07dc`): the port calls `ApplySpineAimRotation` always. `g_pMain +0x1a0` is inside the `CX2GameOption` (+0x180, data block +4, so block +0x1c: a dword the option constructor `3242fbd0` zeroes; the option layout there differs from March's `OptionList`). Name the option if it can be named; else leave the stand-in.
- The `GetExtraCannonBallCount()` inline guard (unit class 117 and `+0x97a0`) is cannon-ball territory (CH1q); confirm nothing here needs it.
- Spot-check the "= March" bodies for any float or order difference, in particular Z FM `32c104d0`, dash X X FM `32c06b00`, Z Z X FM `32c0ff80`.

Hygiene: `git -c diff.algorithm=patience` runs of `python tools/flagcheck.py JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --range <CH1j commit>^..<CH1j commit>` (FAIL only because the `#define` is commented out), `junediverge.py --check-all`, `enumscan.py --check`, both one-file probes (`-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` on and off).
Write "CH1j - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1k0.md`, commit with explicit pathspecs.
