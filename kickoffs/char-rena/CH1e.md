# June character Rena (Stage 7) - Phase CH1e - Class core: Combat Ranger and Wind Sneaker lines

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1e` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1d2 review (CH0 split). One at a time with CH1a-CH1k: they all edit `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1e-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1e - Done" "CH1e - Review"
awk -F'\t' '$7=="CH1e"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1e` (Combat Ranger line (`COMBAT`, `LCR` / `RCR`) and Wind Sneaker line (`LWS` / `RWS`)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

## Amended by CH0 (2026-11-07)

- **Functions: 29** (`awk -F'\t' '$7=="CH1e"' june2017_re/char_rena/ch0_class_map.tsv`; inside 22, new 7).
  June addresses: `32c6bf40` `32c6bfa0` `32c6c120` `32c6e2e0` `32c6e390` `32c72850` `32c72bc0` `32c73d10` `32c73e10` `32c73f10` `32c770a0` `32c77370` `32c776d0` `32c77710` `32c77980` `32c779c0` `32c77c50` `32c782f0` `32c78610` `32c78e40` `32c7feb0`(new LESI_RCR_COMBO_XXZ_EventProcess) `32c80200`(new LESI_RCR_COMBO_XXZZ_EventProcess) `32c80480`(new LESI_RCR_COMBO_XXZZ_LANDING_EventProcess) `32c807c0`(new LESI_RCR_COMBO_XXZZZ_EventProcess) `32c80a40`(new LESI_RWS_DASH_JUMP_XXZ_START_EventProcess) `32c80c70`(new LESI_RWS_DASH_JUMP_XXZ_LOOP_EventProcess) `32c80e80`(new LESI_RWS_DASH_JUMP_XXZ_END_EventProcess) `32c84510` `32c84590`.
- **States: 31** (`awk -F'\t' '$8=="CH1e"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_LCR_START`, `LESI_LCR_WIN`, `LESI_LCR_WIN2`, `LESI_LCR_LOSE`, `LESI_LCR_LOSE2`, `LESI_LWS_START`, `LESI_LWS_WIN`, `LESI_LWS_WIN2`, `LESI_LWS_LOSE`, `LESI_LWS_LOSE2`, `LESI_RCR_COMBO_XXZ`, `LESI_RCR_COMBO_XXZZ`, `LESI_RCR_COMBO_XXZZ_LANDING`, `LESI_RCR_COMBO_XXZZZ`, `LESI_COMBAT_RANGER_DASH_COMBO_Z1`, `LESI_COMBAT_RANGER_DASH_COMBO_Z2`, `LESI_COMBAT_RANGER_LESI_DOUBLE_JUMP_ATTACK_Z`, `LESI_COMBAT_RANGER_LESI_DOUBLE_JUMP_ATTACK_Z_UP`, `LESI_COMBAT_RANGER_DASH_JUMP_COMBO_Z1`, `LESI_COMBAT_RANGER_DASH_JUMP_COMBO_Z2`, `LESI_COMBAT_RANGER_DASH_JUMP_COMBO_Z3`, `LESI_LWS_ZZfrontZ`, `LESI_LWS_ZZfrontZZ`, `LESI_LWS_ZZfrontZZZ`, `LESI_LWS_ZZfrontZZZ_Landing`, `LESI_LWS_DASH_XZ`, `LESI_LWS_DASH_XZZ`, `LESI_RWS_DASH_JUMP_XXZ_START`, `LESI_RWS_DASH_JUMP_XXZ_LOOP`, `LESI_RWS_DASH_JUMP_XXZ_END`, `LESI_SA_LCR_LOW_KICK`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH1e`): 10; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
