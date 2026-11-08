# June character Rena (Stage 7) - Phase CH1g - Class core: Trapping Ranger line (`RTR`)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1g` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1f2 review (CH0 split). One at a time with CH1a-CH1k: they all edit `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1g-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1g - Done" "CH1g - Review"
awk -F'\t' '$7=="CH1g"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1g` (Trapping Ranger line: `RTR`) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
- **`inside` rows:** June's body inside March's `CX2GULire_ElvenRanger` function under `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one `cl /c` of
  `X2GULire_ElvenRanger.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** the plain name CH0 proposed, under `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE`, compiled in every build, referenced by nothing yet. Leave `InitState*`
  alone (CH1m registers).
- A March block under `ADD_RENA_SYSTEM` inside a function you transcribe stays as it is (dead) until CH1q: the June body's natural-force lines go under
  `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (an `#ifdef` inside the `_LIVE` body), never under the studio macro.
- A class-range callee (`0x32c6a8d0`-`0x32cb6c6d`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee is called
  through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- Uncarved functions are read with `python june2017_re/charport_adis.py rena <va>` (Ghidra stays read-only).
- A state that writes the state power rate keeps June's operator (checklist §7 l.256); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai).
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each). `X2GULire_ElvenRanger.*` are CP949 (`.cpp` mixed endings, mostly LF; `.h` mostly LF):
  `bytepatch.py` only; check each `#ifdef` branch with `cl /P` (`ADD_RENA_SYSTEM`, `SERV_RENA_NIGHT_WATCHER`, `SERV_TRAPPING_RANGER_TEST`, ...).
- Done gate: every class-map row with phase `CH1g` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

## Amended by CH0 (2026-11-07)

- **Functions: 46** (`awk -F'\t' '$7=="CH1g"' june2017_re/char_rena/ch0_class_map.tsv`; inside 36, new 10).
  June addresses: `32c6c560` `32c6c570` `32c6d580`(new LESI_RTR_DOUBLE_JUMP_ZX_StateEndFuture) `32c79470` `32c79950` `32c79be0` `32c79f50` `32c7a220` `32c7a290` `32c7a510` `32c7a610` `32c7a640` `32c7a680` `32c818d0`(new LESI_RTR_DOUBLE_JUMP_ZX_EventProcess) `32c84d90` `32c96e50`(new ?) `32c96eb0` `32c96f10` `32c96f70` `32c96fd0` `32c97030` `32c97090` `32c970f0` `32c97150` `32c9a5b0`(new ?) `32ca0cd0`(new LESI_RTR_DOUBLE_JUMP_ZX_StateEnd) `32ca6410`(new ?) `32ca6640` `32ca6670` `32ca6690` `32ca66c0` `32ca6700` `32ca6730` `32ca6780` `32ca67b0` `32ca69e0`(new ?) `32ca6b10`(new ?) `32ca6be0`(new ?) `32ca7670`(new LESI_RTR_DOUBLE_JUMP_ZX_StateStart) `32ca9620` `32ca9640` `32ca9660` `32ca9680` `32ca9700` `32ca9720` `32cad240`.
- **States: 16** (`awk -F'\t' '$8=="CH1g"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_COMBO_RTR_ZZZ_DZ`, `LESI_COMBO_RTR_ZZZ`, `LESI_COMBO_RTR_ZZZZ`, `LESI_COMBO_RTR_ZZZZZ`, `LESI_DASH_COMBO_RTR_ZX`, `LESI_JUMP_ATTACK_RTR_Z`, `LESI_RTR_DOUBLE_JUMP_ZX`, `LESI_DASH_JUMP_ATTACK_RTR_Z`, `LESI_DASH_JUMP_ATTACK_RTR_ZZ`, `LESI_RTR_START`, `LESI_RTR_WIN`, `LESI_RTR_WIN2`, `LESI_RTR_LOSE`, `LESI_RTR_LOSE2`, `LESI_SA_RTR_FATALITY_SUCCESS`, `LESI_SA_RTR_FATALITY_FAIL`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH1g`): 6; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
- **Amended by CH1d2 (2026-11-09):** `LESI_RTR_DOUBLE_JUMP_ZX` (271), `LESI_RNW_DASH_XZ` (272), `LESI_RNW_DASH_JUMP_XZ_START` (277) and `LESI_RNW_DASH_JUMP_XXZ_START` (280) already exist as `_CORE` enum members in `X2GULire_ElvenRanger.h`; CH1d2's double-jump landing, dash attack X and dash-jump X1 / X2 EventProcess change into them. Do not add them again.
