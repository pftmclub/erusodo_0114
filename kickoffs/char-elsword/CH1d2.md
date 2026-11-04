# June character Elsword (Stage 7) - Phase CH1d2 - Class core: Sword Man dash and dash-jump combos (shared by every line)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1d2` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1d review; one at a time with the other `X2GUElsword_SwordMan.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1d2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1d2"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1d2` (the dash, dash-attack and dash-jump combos and the shared knight dash combos; see the CH0 amendment below) from Ghidra, **whole**.
CH1d left `ELSWORD_GROUND_BWALK_CANCEL` / `_WALK_` / `_DASH_`, `ELSWORD_ELSE_IF_FLAG_STATE_CHANGE` / `_SET_IN_WINDOW` and `IsDataClass` / `IsTempletClass` (`_CONST_ELSWORD_CH1D_`) under `_LIVE` before `//ESSI_COMBO_Z1`: reuse them where June's listing has the same shape. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1d2` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 28** (`awk -F'\t' '$7=="CH1d2"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 28).
  June addresses: `32cf3770` `32cf3780` `32cf37f0` `32cf3800` `32cf3810` `32cf3840` `32cf3860` `32cf3870` `32cf38a0` `32cf38d0` `32cf7cb0` `32cf8110` `32cf8230` `32cf8770` `32cf8b30` `32cf90d0` `32cf9220` `32cf9380` `32cf9400` `32cf9440` `32cf9530` `32cf9580` `32cf95e0` `32cf9610` `32d07e20` `32d07eb0` `32d08060` `32d0fa10`.
- **States: 12** (`awk -F'\t' '$8=="CH1d2"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_DASH_COMBO_Z1`, `ESSI_DASH_COMBO_Z2`, `ESSI_KNIGHT_DASH_COMBO_Z3`, `ESSI_DASH_ATTACK_X`, `ESSI_DASH_JUMP_COMBO_Z1`, `ESSI_DASH_JUMP_COMBO_Z2`, `ESSI_DASH_JUMP_COMBO_Z3`, `ESSI_DASH_JUMP_COMBO_Z3_LANDING`, `ESSI_DASH_JUMP_COMBO_X1`, `ESSI_DASH_JUMP_COMBO_X2`, `ESSI_DASH_JUMP_COMBO_X2_LANDING`, `ESSI_KNIGHT_DASH_COMBO_Z2`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7h L420 Elsword 25 functions 32cf8230..32d06c50 (e.g. 32d01040): UPDOWN_CANCEL.
- Scope (CH0 split of CH1d): `ESSI_DASH_ATTACK_X`, `ESSI_DASH_COMBO_*`, `ESSI_DASH_JUMP_COMBO_*` (incl. `_LANDING`), and the shared `ESSI_KNIGHT_DASH_COMBO_Z2/Z3` / `ESSI_DASH_JUMP_COMBO_X2` (gated for several job lines; CH0 hand rule). 7h: `32cf8230` is here.
