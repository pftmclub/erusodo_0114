# June character Elsword (Stage 7) - Phase CH1d - Class core: Sword Man ground combos

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1d` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1c review, char-chung Z. One at a time with CH1a-CH1k: they all edit `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1d"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1d` (Sword Man ground combos (9 `ESSI_COMBO*`, 12 `ESSI_A*`) and the `KNIGHT` / `SWORD` / `MAGIC` common blocks CH0 assigned) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

## Amended by CH0 (2026-11-03)

- **Functions: 28** (`awk -F'\t' '$7=="CH1d"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 28).
  June addresses: `32cf36d0` `32cf36f0` `32cf3700` `32cf3710` `32cf3720` `32cf3730` `32cf3740` `32cf3750` `32cf3760` `32cf37d0` `32cf37e0` `32cf6440` `32cf67c0` `32cf6d80` `32cf7260` `32cf7340` `32cf73e0` `32cf7810` `32cf7b00` `32cf7bd0` `32cf9050` `32cf9090` `32d07c40` `32d07cc0` `32d07d30` `32d07db0` `32d080f0` `32d08180`.
- **States: 11** (`awk -F'\t' '$8=="CH1d"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_COMBO_Z1`, `ESSI_COMBO_Z2`, `ESSI_COMBO_Z3`, `ESSI_COMBO_Z4`, `ESSI_COMBO_Z4a`, `ESSI_COMBO_X1`, `ESSI_COMBO_X2`, `ESSI_COMBO_X3`, `ESSI_COMBO_X3a`, `ESSI_JUMP_ATTACK_Z`, `ESSI_JUMP_ATTACK_X`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8).
- **Split by CH0:** CH1d = ground combos `ESSI_COMBO_*` and `ESSI_JUMP_ATTACK_X/Z` (28); the dash and dash-jump combos are **CH1d2** (28).

## Amended by CH1a review (2026-11-04)

- ComboZ3 EventProcess `32cf6d80` has no re-attack writes: June drops March's `m_bReAttackZ1Right = bIsRight; m_fReAttackZ1Time = 0.2f;` (`.cpp:8269`). No June function reads or writes those members.
