# June character Elsword (Stage 7) - Phase CH1g2 - Class core: Infinity Sword line (ESSI_EIS*, Compact Counter guard attack)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1g2` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1g review; one at a time with the other `X2GUElsword_SwordMan.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1g2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1g2"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1g2` (Sheath Knight (14 `ESSI_ETK*`) and Infinity Sword (11 `ESSI_EIS*`; `PHANTOM_BLADE_*` if CH0 put it here)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

- **Functions: 16** (`awk -F'\t' '$7=="CH1g2"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 12, new 4).
  June addresses: `32cf4260` `32cf4280` `32cf4470`(new ESSI_EIS_DASH_COMBO_ZX_StateEnd) `32cf4490` `32cfdbe0` `32cfdcb0` `32cfe090`(new ESSI_EIS_DASH_COMBO_ZX_EventProcess) `32cfe280` `32cfe440` `32cfe5e0` `32cfe650` `32d13d20` `32d17550`(new ESSI_EIS_DASH_COMBO_XX_FrameMove) `32d1f130` `32d1f200`(new ESSI_EIS_DASH_COMBO_ZX_FrameMove) `32d1f2d0`.
- **States: 12** (`awk -F'\t' '$8=="CH1g2"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_EIS_START`, `ESSI_EIS_WIN`, `ESSI_EIS_WIN2`, `ESSI_EIS_LOSE`, `ESSI_EIS_LOSE2`, `ESSI_EIS_HYPER_MODE`, `ESSI_EIS_DASH_COMBO_XX`, `ESSI_EIS_DASH_COMBO_XXX`, `ESSI_EIS_DASHJUMP_COMBO_XX`, `ESSI_EIS_DASHJUMP_COMBO_XXX`, `ESSI_EIS_DASH_COMBO_ZX`, `ESSI_SI_A_ETK_COMPACT_COUNTER_GUARD_ATTACK`.
- Scope (CH0 split of CH1g): `ESSI_EIS_*` states and `ESSI_SI_A_ETK_COMPACT_COUNTER_GUARD_ATTACK` (6 callbacks). June registers the latter on id 162 when **21216 `RNSI_A_EIS_COMPACT_COUNTER`** (class 112 only) is learned; March gates on `SI_P_ETK_COMPACT_COUNTER` (the registration is CH1m's).

## Amended by CH1d2 (2026-11-04)

- `ESSI_EIS_DASH_COMBO_ZX` (June script id 224) is already in the `ESSI` enum at its end, under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (dash Z1 EventProcess names it). Do not add it again. The state's functions stay yours. `ESSI_EIS_DASH_COMBO_XX` is March's own member (id 225).
- Reusable from CH1d / CH1d2: `ELSWORD_ELSE_IF_FLAG_STATE_CHANGE`, `ELSWORD_ELSE_IF_FLAG_SET_IN_WINDOW`, `ELSWORD_ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_WITH( bKey_ )`, `IsDataClass`, `IsTempletClass`. The checklist row §7h L420 (`UPDOWN_CANCEL_AFTER`) has `32cf8230` done; see CH1e's amendment for the inline cancel.

## Amended by CH1e (2026-11-04)

- The CH1d helper block (`IsDataClass`, `IsTempletClass`, `ELSWORD_GROUND_BWALK_CANCEL` / `_WALK_` / `_DASH_`, `ELSWORD_ELSE_IF_FLAG_STATE_CHANGE`, `ELSWORD_ELSE_IF_FLAG_SET_IN_WINDOW`, `ELSWORD_ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_WITH`) now sits under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (it was `_LIVE`), so a `new` row can use it; `Always.h` carries `_CORE needs SERV_IRUHADEV_JUNE_UPDOWN_CANCEL`. The dash-combo cancel tail (UPDOWN inline, `SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, the three ground macros, the Z / X cancels on `m_fAttackCancelAfter`) is written out in `ESSI_ESK_DASH_COMBO_ZX_EventProcess`; copy that shape for a `new` EventProcess.
