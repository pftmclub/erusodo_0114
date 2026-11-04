# June character Elsword (Stage 7) - Phase CH1f - Class core: Magic Knight and Rune Slayer lines

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1f` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1e review, char-chung Z. One at a time with CH1a-CH1k: they all edit `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1f-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1f"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1f` (Magic Knight (11 `ESSI_EMK*`) and Rune Slayer (18 `ESSI_ERS*`)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GUElsword_SwordMan` function under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of
  `X2GUElsword_SwordMan.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*`
  alone (CH1m registers).
- A class-range callee (`0x32cf2da0`-`0x32d2763f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called
  through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py elsword <va>` (Ghidra stays read-only).
- An alpha-tag site (7c, `c10a_alpha_sites.tsv`) inside one of these functions is written here.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- `X2GUElsword_SwordMan.*` are CP949 (`.cpp` mixed endings, `.h` mostly LF): `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1f` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

## Amended by CH0 (2026-11-03)

- **Functions: 36** (`awk -F'\t' '$7=="CH1f"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 21, new 15).
  June addresses: `32cf3790` `32cf3fa0` `32cf3fc0` `32cf3ff0` `32cf4d50`(new ESSI_ERS_XZ_CHARGE_StateStart) `32cf8840` `32cf8a00` `32cf97a0` `32cf97e0` `32cfb900` `32cfb970` `32cfbdb0` `32cfbe10` `32cfbe80` `32cfbee0` `32d03250`(new ESSI_EMK_DASH_JUMP_ZX_EventProcess) `32d035d0`(new ESSI_EMK_DASH_JUMP_ZXX_EventProcess) `32d03880`(new ESSI_EMK_COMBO_XZ_EventProcess) `32d03d00`(new ESSI_EMK_COMBO_XZ_CHARGE_EventProcess) `32d040c0`(new ESSI_EMK_COMBO_XZZ_EventProcess) `32d04440`(new ESSI_EMK_COMBO_FULL_CHARGE_XZZ_EventProcess) `32d047c0`(new ESSI_ERS_XZ_CHARGE_EventProcess) `32d04b80`(new ESSI_ERS_FULL_CHARGE_XZZ_EventProcess) `32d04f00`(new ESSI_ERS_COMBO_ZZZ_EventProcess) `32d05250`(new ESSI_ERS_COMBO_ZZZZ_EventProcess) `32d07f40` `32d0f640` `32d0fc00` `32d12550` `32d128b0` `32d12c50` `32d131d0` `32d19040`(new ESSI_EMK_COMBO_XZ_FrameMove) `32d19300`(new ESSI_EMK_COMBO_XZZ_FrameMove) `32d195c0`(new ESSI_EMK_COMBO_FULL_CHARGE_XZZ_FrameMove) `32d19880`(new ESSI_ERS_FULL_CHARGE_XZZ_FrameMove).
- **States: 28** (`awk -F'\t' '$8=="CH1f"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_EMK_START`, `ESSI_ERS_START`, `ESSI_EMK_WIN`, `ESSI_EMK_WIN2`, `ESSI_ERS_WIN`, `ESSI_ERS_WIN2`, `ESSI_EMK_LOSE`, `ESSI_EMK_LOSE2`, `ESSI_ERS_LOSE`, `ESSI_ERS_LOSE2`, `ESSI_ERS_HYPER_MODE`, `ESSI_EMK_COMBO_XZ`, `ESSI_EMK_COMBO_XZ_CHARGE`, `ESSI_EMK_COMBO_XZZ`, `ESSI_EMK_COMBO_FULL_CHARGE_XZZ`, `ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3`, `ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a`, `ESSI_EMK_DASH_JUMP_ZX`, `ESSI_EMK_DASH_JUMP_ZXX`, `ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a`, `ESSI_ERS_COMBO_ZZZ`, `ESSI_ERS_COMBO_ZZZZ`, `ESSI_ERS_XZ`, `ESSI_ERS_XZ_CHARGE`, `ESSI_ERS_XZZ`, `ESSI_ERS_FULL_CHARGE_XZZ`, `ESSI_ERS_DASH_JUMP_XZ`, `ESSI_ERS_DASH_JUMP_XZZ`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8); §7h L420 Elsword 25 functions 32cf8230..32d06c50 (e.g. 32d01040): UPDOWN_CANCEL.

## Amended by CH1d (2026-11-04)

- `ESSI_ERS_COMBO_ZZZ` (June script id 260) and `ESSI_EMK_COMBO_XZ` (251) are already in the `ESSI` enum at its end, under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (CH1d's Z2 / X1 EventProcess name them). Do not add them again; add the other June-only ids the same way. The states' functions stay yours.
