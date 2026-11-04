# June character Elsword (Stage 7) - Phase CH1g - Class core: Sheath Knight and Infinity Sword lines

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1g` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1f review, char-chung Z. One at a time with CH1a-CH1k: they all edit `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1g-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1g"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1g` (Sheath Knight (14 `ESSI_ETK*`) and Infinity Sword (11 `ESSI_EIS*`; `PHANTOM_BLADE_*` if CH0 put it here)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

## Amended by CH0 (2026-11-03)

- **Functions: 31** (`awk -F'\t' '$7=="CH1g"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 29, new 2).
  June addresses: `32cf4090` `32cf40b0` `32cf40d0` `32cf40e0` `32cf4100` `32cf4120` `32cf4150` `32cf4170` `32cf4190` `32cf41b0` `32cfc8e0` `32cfc900` `32cfcb70` `32cfce30` `32cfcfe0` `32cfd210` `32cfd540` `32cfd700` `32cfd7a0` `32d054d0`(new ESSI_ETK_DASH_JUMP_ZZZ_EventProcess) `32d0b730` `32d0b7d0` `32d19b80`(new ESSI_ETK_DASH_JUMP_ZZZ_FrameMove) `32d1e200` `32d1e400` `32d1e4d0` `32d1e5a0` `32d1e7a0` `32d1e870` `32d1e940` `32d1ea10`.
- **States: 14** (`awk -F'\t' '$8=="CH1g"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_ETK_START`, `ESSI_ETK_WIN`, `ESSI_ETK_WIN2`, `ESSI_ETK_LOSE`, `ESSI_ETK_LOSE2`, `ESSI_ETK_COMBO_XX`, `ESSI_ETK_COMBO_XXX_LOOP`, `ESSI_ETK_COMBO_XXX_END`, `ESSI_ETK_COMBO_XXXZ`, `ESSI_ETK_DASH_COMBO_ZZ`, `ESSI_ETK_DASH_JUMP_ZZZ`, `ESSI_ETK_COMBO_ZZZ`, `ESSI_ETK_COMBO_ZZZZ`, `ESSI_ETK_COMBO_ZZZZZ`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8); §7c L352 29 alpha-tag sites 32d0d660 .. 32d1ecc0.
- **Split by CH0:** CH1g = Sheath Knight line `ESSI_ETK_*` (31); Infinity Sword `ESSI_EIS_*` and the Compact Counter guard attack are **CH1g2** (16).

## Amended by CH1d2 (2026-11-04)

- `ESSI_ETK_DASH_JUMP_ZZZ` (June script id 258) is already in the `ESSI` enum at its end, under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (dash jump Z2 EventProcess names it). Do not add it again. The state's functions stay yours.
- Reusable from CH1d / CH1d2: `ELSWORD_ELSE_IF_FLAG_STATE_CHANGE`, `ELSWORD_ELSE_IF_FLAG_SET_IN_WINDOW`, `ELSWORD_ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_WITH( bKey_ )`, `IsDataClass`, `IsTempletClass`. The checklist row §7h L420 (`UPDOWN_CANCEL_AFTER`) has `32cf8230` done; see CH1e's amendment for the inline cancel.

## Amended by CH1e (2026-11-04)

- The CH1d helper block (`IsDataClass`, `IsTempletClass`, `ELSWORD_GROUND_BWALK_CANCEL` / `_WALK_` / `_DASH_`, `ELSWORD_ELSE_IF_FLAG_STATE_CHANGE`, `ELSWORD_ELSE_IF_FLAG_SET_IN_WINDOW`, `ELSWORD_ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_WITH`) now sits under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (it was `_LIVE`), so a `new` row can use it; `Always.h` carries `_CORE needs SERV_IRUHADEV_JUNE_UPDOWN_CANCEL`. The dash-combo cancel tail (UPDOWN inline, `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, the three ground macros, the Z / X cancels on `m_fAttackCancelAfter`) is written out in `ESSI_ESK_DASH_COMBO_ZX_EventProcess`; copy that shape for a `new` EventProcess.
