# June character Elsword (Stage 7) - Phase CH1c - Class core: riding and fun states

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1c` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1b review, char-chung Z. One at a time with CH1a-CH1k: they all edit `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1c"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1c` (riding (27 `ESSI_RIDING*`), pepper run (5), dance (3), `MACHINE` (7) unless CH0 placed it elsewhere) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

## Amended by CH0 (2026-11-03)

- **Functions: 2** (`awk -F'\t' '$7=="CH1c"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 1, new 1).
  June addresses: `32d18d80`(new ?) `32d1b910`.
- **States: 43** (`awk -F'\t' '$8=="CH1c"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_PEPPER_RUN_READY`, `ESSI_PEPPER_RUN`, `ESSI_PEPPER_RUN_END`, `ESSI_PEPPER_RUN_JUMP_UP`, `ESSI_PEPPER_RUN_JUMP_DOWN`, `ESSI_AMUSEMENT_ITEM_PHOENIX_RUN`, `ESSI_AMUSEMENT_ITEM_PHOENIX_RUN_END`, `ESSI_DANCE_Z`, `ESSI_DANCE_X`, `ESSI_DANCE_WAIT`, `ESSI_RIDING_START`, `ESSI_RIDING_ON`, `ESSI_RIDING_OFF`, `ESSI_RIDING_WAIT_HABIT`, `ESSI_RIDING_WAIT`, `ESSI_RIDING_WALK`, `ESSI_RIDING_JUMP_UP`, `ESSI_RIDING_JUMP_DOWN`, `ESSI_RIDING_JUMP_LANDING`, `ESSI_RIDING_DASH`, `ESSI_RIDING_DASH_END`, `ESSI_RIDING_DASH_JUMP`, `ESSI_RIDING_DASH_JUMP_LANDING`, `ESSI_RIDING_DAMAGE_FRONT`, `ESSI_RIDING_DAMAGE_FRONT`, `ESSI_RIDING_ATTACK_Z`, `ESSI_RIDING_ATTACK_X`, `ESSI_RIDING_JUMP_ATTACK_Z`, `ESSI_RIDING_ATTACK_SPECIAL`, `ESSI_RIDING_DIE`, `ESSI_RIDING_SPECIAL_MOVE`, `ESSI_RIDING_HYPER_MODE`, `ESSI_RIDING_ATTACK_X_READY`, `ESSI_RIDING_ATTACK_X_READY_2`, `ESSI_RIDING_ATTACK_X_END`, `ESSI_MACHINE_GUN_RIDE`, `ESSI_MACHINE_GUN_WAIT`, `ESSI_MACHINE_GUN_WALK`, `ESSI_MACHINE_GUN_ATTACK`, `ESSI_MACHINE_GUN_LEAVE`, `ESSI_MACHINE_GUN_JUMP_UP`, `ESSI_MACHINE_GUN_JUMP_DOWN`, `ESSI_RIDING_DAMAGE_BACK`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8).
- Only 2 class functions: the 34 riding / pepper / dance / `MACHINE` states use `CX2GUUser` callbacks (Chung's CH1n2 already replaced the base ones). `ESSI_THROW_WOODEN_PIECE` is a dead table in June (no registration).
