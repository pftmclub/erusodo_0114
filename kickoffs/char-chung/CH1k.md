# June character Chung (Stage 7) - Phase CH1k - Class core: overrides below vslot 0x600

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH1k` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1k0 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-chung/CH1k-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1k"' june2017_re/char_chung/ch0_class_map.tsv
awk -F'\t' '$8=="CH1k"' june2017_re/char_chung/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1k/' june2017_re/char_chung/ch0_checklist_rows.tsv
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Eve's matching sub-phase is a worked example of the
reading, not of the naming (hers wrote `June_*` members; Chung never does).

## Task

Transcribe every function the class map gives phase `CH1k` (lifecycle, render and reaction virtuals; DamageReact `32c0cf90` (vslot 0x60) with the 7p hit-stop (`@32c0d18c` / `d19c` / `d1b4`); the ctor `32bf1040` / dtor `32c0c4e0` offset map (every member June's ctor sets, March pair or new member)) from Ghidra, **whole**. The plan's "Transcription rules
(CH1a-CH1l4)" apply:
- **`inside` rows:** June's body inside March's `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one
  `cl /c` of `X2GUChung.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** a plain name in the class's pattern (CH0 proposed it) under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, compiled in every build,
  referenced by nothing yet. Leave `InitState*` alone (CH1m registers).
- A class-range callee (`0x32bd86f0`-`0x32c25a1f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee
  is called through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai,
  `s3e_bybfi_callers.tsv`).
- `X2GUChung.*` are CP949 (`.cpp` CRLF, `.h` mixed): `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1k` is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
  with `_LIVE` undefined and the one-file probe passes with it defined; `junediverge.py --check` and `enumscan.py --check` on the class files PASS.
  Nothing to play-test: the hand-off says so and names the next phase.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by CH0 (2026-10-31)

- **Functions: 38** (`awk -F'\t' '$7=="CH1k"' june2017_re/char_chung/ch0_class_map.tsv`; inside 32, new 6).
  June addresses: `32bd86f0` `32bd8730` `32bdbdd0` `32bdbe10` `32bdc700` `32bdc850` `32bdc890`(new GetMetabolismBoostValue) `32beaf70` `32beaf90` `32beb050` `32bec310` `32bed3d0`(new GetDefenderDamageRate) `32bed7e0`(new ?) `32bed840`(new ?) `32bed9a0` `32bedb40` `32bee5b0` `32bee680` `32beeb90`(new ?) `32beec40`(new ?) `32beece0` `32beeed0` `32beefb0` `32bf0e90` `32bf4880` `32bf4900` `32bf50e0` `32c06220` `32c06270` `32c063d0` `32c06750` `32c0c4e0` `32c0c8f0` `32c0ca60` `32c0cf90` `32c0feb0` `32c24b80` `32c25880`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §5 L184-188 keys COMMAND_HIT_ADD_MP_BONUS, SIEGE_ANGLE, MOBILE_SIEGE_ANGLE (INIT_C; §7 L246 32bf4900 InitComponent reader (COMMAND_HIT_ADD_MP_BONUS 1.15, SIEGE_AN; §7p L546 DamageReact 32c0cf90 (vslot 0x60) hit-stop: attacker SetStopTime( 0.5,.
- **Depends on CH0's Opus helper phase CH1k0** (`31d28b30`, `3226e020`, `3226f870`, `31ff1950`, `32497d80`, `32a37130`).
- Opus helper phase **CH1k0** runs first (`31d28b30`, `3226e020` / `3226f870` weapon render pulse, `31ff1950`, `32497d80`, `32a37130`).
- InitComponent `32bf4900` (slot 0x3c) is written whole here, with its key reads: `COMMAND_HIT_ADD_MP_BONUS` (+0x98c4, consumer CH1p), `SIEGE_ANGLE` (+0x261a..d) / `MOBILE_SIEGE_ANGLE` (+0x261e..21) (consumers in the siege skill states), `EXTRA_CANNON_BALL_DAMAGE_RATE` (+0x2626 / 7, CH1q).
- The ctor `32bf1040` is kind `not-Chung` in the map only because its caller (`3230f409`) is outside the range: it is Chung's ctor and the source of the ctor / dtor offset map. Dtor body `32c0c4e0`, slot 0 `32c25880`.
- 7p: DamageReact `32c0cf90` with the hit-stop.

## Amended by CH1g (2026-10-31)

- **Three `_CORE` members stand in for June members the ctor map must place:** `m_fRemodelRifleDamageRate` (June `+0x98ac`, protected float, ctor 1.0; written by CH4a) and `m_iCountbyCsgComboDashZXXMiniGun` (June `+0x9894`, int, 0); both are in March's ctor initializer list since CH1g. `CHUNG_MAJOR_MII_CSG_COMBO_DASH_ZX_MINI_GUN_L` / `_R` were appended before `CHUNG_MAJOR_MII_END` for June's handle slots 0x11 / 0x12 (`+0x960c` / `+0x9610`). June's own slot order differs from March's after 0x0f (Bullet Blitz's left gun is slot 0x10 in `32bffd20`): settle `InitializeMajorMeshArray` / `DeleteMajorMesh` / DamageReact `32c0cf90` (which destroys 0x11 and 0x12) against it.

## Amended by CH1h (2026-11-01)

- **Deadly Chaser mini gun handle slots are placed by June's number, written by March's enumerator.** June frees / reads slot 9 (CDC X, left), 10 / 11 (CDC X Z, left / right) and 13 / 14 (CDC Z Z X X, left / right); slot 12 (`CHUNG_MAJOR_MII_CDC_COMBO_XZZ_MINI_GUN_L`) is never used. March's enumerators sit one higher (10, 11, 12, 14, 15) because June has no slot for Head Shot between dash-jump landing (8) and the CDC block. CH1h wrote the March names with the June slot in the comment, so the June bodies work only if `InitializeMajorMeshArray` / `DeleteMajorMesh` clear all of `CHUNG_MAJOR_MII_END`: settle that together with the CH1g2 slot note above.
