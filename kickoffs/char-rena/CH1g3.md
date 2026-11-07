# June character Rena (Stage 7) - Phase CH1g3 - Class core: Night Watcher accel dash and dash-jump states (CH0 split of CH1g)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1g3` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1g2 review. One at a time with the other `X2GULire_ElvenRanger.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1g3-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1g - Done" "CH1g - Review"
awk -F'\t' '$7=="CH1g3"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1g3` (Night Watcher accel dash and dash-jump states) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

- **Functions: 27** (`awk -F'\t' '$7=="CH1g3"' june2017_re/char_rena/ch0_class_map.tsv`; inside 22, new 5).
  June addresses: `32c6b4b0` `32c6b4e0` `32c6b500` `32c6b510` `32c6b530` `32c6b560` `32c6b580` `32c6b590` `32c6b5a0` `32c6b5c0` `32c6b5d0` `32c6b5e0` `32c75b00` `32c75b50` `32c75bc0` `32c75cc0` `32c82850`(new LESI_RNW_DASH_JUMP_XZ_START_EventProcess) `32c828a0`(new LESI_RNW_DASH_JUMP_XXZ_LOOP_EventProcess) `32c82ab0`(new LESI_RNW_DASH_JUMP_XZ_END_EventProcess) `32c84820` `32c848c0` `32c84920` `32c84980` `32c948e0` `32c94ba0` `32ca0f20`(new LESI_RNW_DASH_JUMP_XXZ_LOOP_StateEnd) `32ca7730`(new LESI_RNW_DASH_JUMP_XXZ_LOOP_StateStart).
- **States: 10** (`awk -F'\t' '$8=="CH1g3"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_RNW_ACCEL_DASH`, `LESI_RNW_ACCEL_DASH_JUMP`, `LESI_RNW_ACCEL_DASH_JUMP_LANDING`, `LESI_RNW_ACCEL_DASH_JUMP_Z`, `LESI_RNW_ACCEL_DASH_JUMP_ZZ`, `LESI_RNW_DASH_JUMP_XZ_START`, `LESI_RNW_DASH_JUMP_XZ_END`, `LESI_RNW_DASH_JUMP_XXZ_START`, `LESI_RNW_DASH_JUMP_XXZ_LOOP`, `LESI_RNW_DASH_JUMP_XXZ_END`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7ai L848 pass June's skip / adjust / level / creator at every ported BFI call.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH1g3`): 1; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
