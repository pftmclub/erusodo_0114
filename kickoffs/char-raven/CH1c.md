# June character Raven (Stage 7) - Phase CH1c - Class core: riding, pepper run, dance

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH1c` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1b review. One at a time with CH1a-CH1k: they all edit `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-raven/CH1c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1c - Done" "CH1c - Review"
awk -F'\t' '$7=="CH1c"' june2017_re/char_raven/ch0_class_map.tsv   # column as Elsword's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier Raven CH1 Done notes / reviews (their routed gaps). Elsword's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1c` (riding (27 `RSI_RIDING*`), pepper run (5), dance (3)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

## Amended by CH0 (2026-11-05)

- **Functions: 2** (`awk -F'\t' '$7=="CH1c"' june2017_re/char_raven/ch0_class_map.tsv`; inside 1, new 1).
  June addresses: `32c4f350`(new ?) `32c511a0`.
- **States: 37** (`awk -F'\t' '$8=="CH1c"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_PEPPER_RUN_READY`, `RSI_PEPPER_RUN`, `RSI_PEPPER_RUN_END`, `RSI_PEPPER_RUN_JUMP_UP`, `RSI_PEPPER_RUN_JUMP_DOWN`, `RSI_AMUSEMENT_ITEM_PHOENIX_RUN`, `RSI_AMUSEMENT_ITEM_PHOENIX_RUN_END`, `RSI_DANCE_Z`, `RSI_DANCE_X`, `RSI_DANCE_WAIT`, `RSI_RIDING_START`, `RSI_RIDING_ON`, `RSI_RIDING_OFF`, `RSI_RIDING_WAIT_HABIT`, `RSI_RIDING_WAIT`, `RSI_RIDING_WALK`, `RSI_RIDING_JUMP_UP`, `RSI_RIDING_JUMP_DOWN`, `RSI_RIDING_JUMP_LANDING`, `RSI_RIDING_DASH`, `RSI_RIDING_DASH_END`, `RSI_RIDING_DASH_JUMP`, `RSI_RIDING_DASH_JUMP_LANDING`, `RSI_RIDING_DAMAGE_FRONT`, `RSI_RIDING_DAMAGE_FRONT`, `RSI_RIDING_ATTACK_Z`, `RSI_RIDING_ATTACK_X`, `RSI_RIDING_JUMP_ATTACK_Z`, `RSI_RIDING_ATTACK_SPECIAL`, `RSI_RIDING_DIE`, `RSI_RIDING_SPECIAL_MOVE`, `RSI_RIDING_HYPER_MODE`, `RSI_RIDING_SPECIFIC`, `RSI_RIDING_ATTACK_X_READY`, `RSI_RIDING_ATTACK_X_READY_2`, `RSI_RIDING_ATTACK_X_END`, `RSI_RIDING_DAMAGE_BACK`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID).
- Only 2 class functions: the riding / pepper / dance states use `CX2GUUser` callbacks. `RSI_RIDING_JUMP_READY` is dead data; `RSI_RIDING_DAMAGE_BACK` is registered with no script table.
