# June character Raven (Stage 7) - Phase CH1d - Class core: Raven ground combos

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH1d` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1c review. One at a time with CH1a-CH1k: they all edit `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-raven/CH1d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1d - Done" "CH1d - Review"
awk -F'\t' '$7=="CH1d"' june2017_re/char_raven/ch0_class_map.tsv   # column as Elsword's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier Raven CH1 Done notes / reviews (their routed gaps). Elsword's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1d` (Raven ground combos (11 `RSI_COMBO*`, the X-X-X combo) and the base dash / jump combos CH0 assigns) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1d` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 38** (`awk -F'\t' '$7=="CH1d"' june2017_re/char_raven/ch0_class_map.tsv`; inside 34, new 4).
  June addresses: `32c25fe0`(new RSI_COMBO_ZZZX_StateStartFuture) `32c25ff0`(new RSI_COMBO_ZZZX_FINISH_StateStartFuture) `32c26000`(new RSI_COMBO_ZZZX_FINISH_FrameMove) `32c26010` `32c26020` `32c26050` `32c26070` `32c26090` `32c297a0` `32c29bb0` `32c29fd0` `32c2a550` `32c2a880` `32c2abc0`(new RSI_COMBO_ZZZX_FINISH_EventProcess) `32c2aef0` `32c2b360` `32c2b6f0` `32c2bbb0` `32c2c070` `32c2c370` `32c2c810` `32c2d450` `32c2d750` `32c2d860` `32c2d8b0` `32c2d8e0` `32c2d930` `32c2da40` `32c2da80` `32c3d800` `32c3d880` `32c3d8f0` `32c3d980` `32c44f90` `32c45260` `32c45f80` `32c46170` `32c5c230`.
- **States: 20** (`awk -F'\t' '$8=="CH1d"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_COMBO_Z`, `RSI_COMBO_ZZ`, `RSI_COMBO_ZZZ`, `RSI_COMBO_ZZZZ`, `RSI_COMBO_ZZZX`, `RSI_COMBO_ZZZX_FINISH`, `RSI_COMBO_ZZX`, `RSI_COMBO_ZZXX`, `RSI_COMBO_X`, `RSI_COMBO_XX`, `RSI_COMBO_XXX`, `RSI_DASH_COMBO_Z`, `RSI_DASH_COMBO_ZZ`, `RSI_DASH_COMBO_X`, `RSI_JUMP_COMBO_Z`, `RSI_JUMP_COMBO_X`, `RSI_DASH_JUMP_COMBO_Z`, `RSI_DASH_JUMP_COMBO_ZZ`, `RSI_DASH_JUMP_COMBO_ZZ_Landing`, `RSI_DASH_JUMP_COMBO_X`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7h L422 Raven 32c2b360: UPDOWN_CANCEL_AFTER (+0x8ca4) up/down cancel branch in.
- 7h: `32c2b360` is `RSI_COMBO_ZZXX` EventProcess (`UPDOWN_CANCEL_AFTER`, +0x8ca4).
