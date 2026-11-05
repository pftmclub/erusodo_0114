# June character Raven (Stage 7) - Phase CH1e - Class core: Sword Taker and Blade Master lines

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH1e` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1d review. One at a time with CH1a-CH1k: they all edit `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-raven/CH1e-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1e - Done" "CH1e - Review"
awk -F'\t' '$7=="CH1e"' june2017_re/char_raven/ch0_class_map.tsv   # column as Elsword's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier Raven CH1 Done notes / reviews (their routed gaps). Elsword's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1e` (Sword Taker line (10 `RSI_SOUL_TAKER*`, `RST`) and Blade Master line (`RSI_RBM*`, the `RBM_` tables)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GURaven` function under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of
  `X2GURaven.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*`
  alone (CH1m registers).
- A class-range callee (`0x32c25a20`-`0x32c6a63d`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called
  through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py raven <va>` (Ghidra stays read-only).
- A state that writes the state power rate keeps June's operator (`*=` where June multiplies; March assigns in the Shadow Step / Shadow Buster
  states, checklist §7 l.256).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- `X2GURaven.*` are CP949 (`.cpp` mixed endings, mostly LF; `.h` mostly LF): `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1e` is written (the Done note lists them with June addresses and `inside` / `new`); the build
  passes with `_LIVE` undefined and the one-file probe passes with it defined; `junediverge.py --check` and `enumscan.py --check` on the class
  files PASS. Nothing to play-test: the hand-off says so and names the next phase.

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

## Amended by CH0 (2026-11-05)

- **Functions: 32** (`awk -F'\t' '$7=="CH1e"' june2017_re/char_raven/ch0_class_map.tsv`; inside 20, new 12).
  June addresses: `32c26340` `32c26b80` `32c26b90` `32c26bd0` `32c2cb30`(new RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZ_EventProcess) `32c2cc40`(new RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZZ_EventProcess) `32c2cc70`(new RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZZ_Landing_StateStartFuture) `32c2ccc0`(new RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZZZ_EventProcess) `32c2cfe0`(new RBM_DASH_JUMP_COMBO_X_EventProcess) `32c2d0f0`(new RBM_DASH_JUMP_COMBO_XZ_EventProcess) `32c2dd50` `32c2dec0` `32c2df90`(new RSI_RBM_COMBO_ZZZupZ_FrameMoveFuture) `32c2dfe0`(new RSI_RBM_COMBO_ZZZupZ_EventProcess) `32c2e2f0` `32c2e6a0` `32c2e700` `32c2ea40` `32c32560` `32c32970` `32c32da0` `32c32de0` `32c41330` `32c413e0` `32c41890` `32c41a00` `32c41ef0` `32c45520`(new RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZ_Landing_EventProcess) `32c45710`(new RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZZ_Landing_EventProcess) `32c459f0`(new RBM_DASH_JUMP_COMBO_X_FrameMove) `32c45cc0`(new RBM_DASH_JUMP_COMBO_XZ_FrameMove) `32c49de0`.
- **States: 19** (`awk -F'\t' '$8=="CH1e"' june2017_re/char_raven/ch0_state_skill.tsv`): `RBM_COMBO_XZ`, `RBM_COMBO_XZZ`, `RBM_COMBO_XZZZ`, `RSI_SOUL_TAKER_COMBO_XXZ`, `RSI_SOUL_TAKER_COMBO_XXZZ8`, `RSI_SOUL_TAKER_COMBO_XXZZ8Z`, `RSI_SOUL_TAKER_DASH_COMBO_ZZ`, `RSI_SOUL_TAKER_DASH_COMBO_ZZZ`, `RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZ`, `RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZ_Landing`, `RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZZ`, `RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZZ_Landing`, `RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZZZ`, `RSI_RBM_COMBO_ZZZupZ`, `RBM_COMBO_ZZZX`, `RBM_COMBO_ZZZX_EXPLOSION`, `RBM_COMBO_ZZZXZ`, `RBM_DASH_JUMP_COMBO_X`, `RBM_DASH_JUMP_COMBO_XZ`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID).
- **Amended by CH1d (2026-11-05):** `RSI_RBM_COMBO_ZZZupZ` (June id 299), `RSI_SOUL_TAKER_DASH_JUMP_COMBO_ZZ` (294) and `RBM_DASH_JUMP_COMBO_XZ` (301) are already `RAVEN_STATE_ID` members (`_CORE`, appended without `= value`): do not add them again. CH1d's Z-Z-Z (`32c29fd0`) starts `RSI_RBM_COMBO_ZZZupZ` for class 106 only (`JUNE_BUGS` #536); `32c2d930` (dash-jump X EventProcess) starts `RBM_DASH_JUMP_COMBO_XZ`.
