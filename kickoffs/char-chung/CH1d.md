# June character Chung (Stage 7) - Phase CH1d - Class core: Iron Cannon ground combos

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH1d` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1d0 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-chung/CH1d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1d"' june2017_re/char_chung/ch0_class_map.tsv
awk -F'\t' '$8=="CH1d"' june2017_re/char_chung/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1d/' june2017_re/char_chung/ch0_checklist_rows.tsv
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Eve's matching sub-phase is a worked example of the
reading, not of the naming (hers wrote `June_*` members; Chung never does).

## Task

Transcribe every function the class map gives phase `CH1d` (the 20 `CSI_COMBO*` and 4 `CSI_ATTACK*` states) from Ghidra, **whole**. The plan's "Transcription rules
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
- Done gate: every class-map row with phase `CH1d` is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
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

- **Functions: 37** (`awk -F'\t' '$7=="CH1d"' june2017_re/char_chung/ch0_class_map.tsv`; inside 37).
  June addresses: `32bd8ab0` `32bd8b00` `32bd8b20` `32bd8b70` `32bd8b90` `32bd8bc0` `32bd8c10` `32bd8c60` `32bdd420` `32bdd5e0` `32bdd7e0` `32bdda60` `32bddc20` `32bdddf0` `32bddf30` `32bde0f0` `32bde240` `32bde470` `32bde640` `32beb2a0` `32beb330` `32beb450` `32beb5b0` `32bf5360` `32bf75b0` `32bf77c0` `32bf7ae0` `32bf7cd0` `32bf7ef0` `32c00ec0` `32c01070` `32c01220` `32c014d0` `32c01780` `32c01930` `32c03b30` `32c1bad0`.
- **States: 15** (`awk -F'\t' '$8=="CH1d"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_COMBO_Z`, `CSI_COMBO_ZZ`, `CSI_COMBO_ZZZ`, `CSI_COMBO_ZZZZ`, `CSI_COMBO_ZZZX_READY`, `CSI_COMBO_ZZZX_CANNON_BALL_SHOT`, `CSI_COMBO_ZZZX_FINISH`, `CSI_COMBO_ZZdownZ`, `CSI_COMBO_ZZdownZZ`, `CSI_COMBO_X`, `CSI_COMBO_XX`, `CSI_COMBO_XXX`, `CSI_COMBO_XXZ`, `CSI_COMBO_XXZX_CANNON_BALL_SHOT`, `CSI_COMBO_XXZX_FINISH`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L253 class code scales the cached state power rate (+0x7cc8).
- **Depends on CH0's Opus helper phase CH1d0** (`31fd4290`, `31fd4340`).
- Split by CH0 (59 functions): CH1d = `CSI_COMBO_Z*` / `CSI_COMBO_X*`; CH1d2 = dash / jump combos and `CSI_ATTACK_*`.
- Opus helper phase **CH1d0** runs first (`31fd4290`, 9 callers, and its neighbour `31fd4340`).
- `32bf3d30` (UpCannonBallCount) is CH1q's: declare it.
