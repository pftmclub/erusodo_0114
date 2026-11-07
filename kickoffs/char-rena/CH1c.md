# June character Rena (Stage 7) - Phase CH1c - Class core: riding, pepper run, dance

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH1c` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1b review. One at a time with CH1a-CH1k: they all edit `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-rena/CH1c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1c - Done" "CH1c - Review"
awk -F'\t' '$7=="CH1c"' june2017_re/char_rena/ch0_class_map.tsv   # column 7 = phase (CH0 checked)
```
Then CH0's Done note and the earlier Rena CH1 Done notes / reviews (their routed gaps). Raven's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1c` (riding (27 `LESI_RIDING*`), pepper run (5), dance (3)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1c` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 3** (`awk -F'\t' '$7=="CH1c"' june2017_re/char_rena/ch0_class_map.tsv`; inside 2, new 1).
  June addresses: `32c6d420` `32c9f4f0`(new ?) `32ca7040`.
- **States: 37** (`awk -F'\t' '$8=="CH1c"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_PEPPER_RUN_READY`, `LESI_PEPPER_RUN`, `LESI_PEPPER_RUN_END`, `LESI_PEPPER_RUN_JUMP_UP`, `LESI_PEPPER_RUN_JUMP_DOWN`, `LESI_AMUSEMENT_ITEM_PHOENIX_RUN`, `LESI_AMUSEMENT_ITEM_PHOENIX_RUN_END`, `LESI_DANCE_Z`, `LESI_DANCE_X`, `LESI_DANCE_WAIT`, `LESI_RIDING_START`, `LESI_RIDING_ON`, `LESI_RIDING_OFF`, `LESI_RIDING_WAIT_HABIT`, `LESI_RIDING_WAIT`, `LESI_RIDING_WALK`, `LESI_RIDING_JUMP_UP`, `LESI_RIDING_JUMP_DOWN`, `LESI_RIDING_JUMP_LANDING`, `LESI_RIDING_DASH`, `LESI_RIDING_DASH_END`, `LESI_RIDING_DASH_JUMP`, `LESI_RIDING_DASH_JUMP_LANDING`, `LESI_RIDING_DAMAGE_FRONT`, `LESI_RIDING_DAMAGE_FRONT`, `LESI_RIDING_ATTACK_Z`, `LESI_RIDING_ATTACK_X`, `LESI_RIDING_JUMP_ATTACK_Z`, `LESI_RIDING_ATTACK_SPECIAL`, `LESI_RIDING_DIE`, `LESI_RIDING_SPECIAL_MOVE`, `LESI_RIDING_HYPER_MODE`, `LESI_RIDING_SPECIFIC`, `LESI_RIDING_ATTACK_X_READY`, `LESI_RIDING_ATTACK_X_READY_2`, `LESI_RIDING_ATTACK_X_END`, `LESI_RIDING_DAMAGE_BACK`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE.

## Amended by CH1b (2026-11-07)

- `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` is now defined (`Always.h`; `_LIVE needs _CORE` guard added). Its first member is `CX2GULire_ElvenRanger::KillMySummonedNPC( iNPCID, float )` (June `32ca2010`): a later phase with a `new` row adds to the same `#ifdef`.
- A stand-up flag that also fires on animation end uses the file-local macro `ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_` (Rena `.cpp` top, `_LIVE`), the same expansion as Chung's.
- CH0's `new ?` rows can be March functions: `32ca6e90` was `ClearStartOfDelayedFiringData`, `32cb6310` / `32cb61e0` its lambda instantiations. Read the callee's listing before naming a `new` row.
