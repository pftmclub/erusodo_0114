# June character Elsword (Stage 7) - Phase CH1e - Class core: Sword Knight and Lord Knight lines

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH1e` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1d2 review (char-chung Z done 2026-11-03). One at a time with CH1a-CH1k: they all edit `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-elsword/CH1e-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Phase CH0 - Elsword map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1e"' june2017_re/char_elsword/ch0_class_map.tsv   # column as Chung's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Chung's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1e` (Sword Knight (10 `ESSI_ESK*`) and Lord Knight (13 `ESSI_ELK*`, incl. the X-X-X combos)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

## Amended by CH0 (2026-11-03)

- **Functions: 30** (`awk -F'\t' '$7=="CH1e"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 16, new 14).
  June addresses: `32cf38f0` `32cf3f70` `32cf3f80` `32cf3f90` `32cf4d30`(new ESSI_ESK_DASH_COMBO_ZX_StateStartFuture) `32cf4d40`(new ESSI_ESK_DASH_COMBO_ZXX_StateStartFuture) `32cf8920` `32cf9670` `32cf96c0` `32cf9720` `32cf9830` `32cfaed0` `32cfafa0` `32cfb420` `32cff880` `32cff8d0`(new ESSI_SI_P_ESK_AUTO_GUARD_SMALL_EventProcess) `32cff920`(new ESSI_SI_P_ESK_AUTO_GUARD_END_EventProcess) `32d01040`(new ESSI_ESK_DASH_COMBO_ZX_EventProcess) `32d014c0`(new ESSI_ESK_DASH_COMBO_ZXX_EventProcess) `32d017f0`(new ESSI_ESK_DASH_COMBO_ZChargeX_EventProcess) `32d01b70`(new ESSI_ESK_DASH_JUMP_ZZZ_EventProcess) `32d01ba0`(new ESSI_ESK_DASH_JUMP_ZZZ_LANDING_EventProcess) `32d01f10`(new ESSI_ELK_DASH_COMBO_XChargeX_EventProcess) `32d02290`(new ESSI_ELK_DASH_COMBO_XX_EventProcess) `32d02710`(new ESSI_ELK_DASH_COMBO_XXX_EventProcess) `32d02a90`(new ESSI_ELK_XXXXX_EventProcess) `32d02ed0`(new ESSI_ELK_XXXXXX_EventProcess) `32d07fd0` `32d0c740` `32d180a0`.
- **States: 31** (`awk -F'\t' '$8=="CH1e"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_ESK_START`, `ESSI_ELK_START`, `ESSI_ESK_WIN`, `ESSI_ESK_WIN2`, `ESSI_ELK_WIN`, `ESSI_ELK_WIN2`, `ESSI_ESK_LOSE`, `ESSI_ESK_LOSE2`, `ESSI_ELK_LOSE`, `ESSI_ELK_LOSE2`, `ESSI_ELK_HYPER_MODE`, `ESSI_KNIGHT_DASH_COMBO_Z3a`, `ESSI_SI_P_ESK_AUTO_GUARD`, `ESSI_SI_P_ESK_AUTO_GUARD_END`, `ESSI_SI_P_ESK_AUTO_GUARD_SMALL`, `ESSI_SI_P_ESK_AUTO_GUARD_END_SMALL`, `ESSI_ELK_XXX`, `ESSI_ELK_XXXX`, `ESSI_ELK_XXXXX`, `ESSI_ESK_DASH_COMBO_ZChargeX`, `ESSI_ESK_DASH_COMBO_ZX`, `ESSI_ESK_DASH_COMBO_ZXX`, `ESSI_ESK_DASH_JUMP_ZZZ`, `ESSI_ESK_DASH_JUMP_ZZZ_LANDING`, `ESSI_ELK_ZZZfrontZ`, `ESSI_ELK_DASH_COMBO_XChargeX`, `ESSI_ELK_DASH_COMBO_XX`, `ESSI_ELK_DASH_COMBO_XXX`, `ESSI_KNIGHT_DASH_JUMP_COMBO_X2`, `ESSI_KNIGHT_DASH_JUMP_COMBO_X3`, `ESSI_ELK_XXXXXX`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L254 class code scales the cached state power rate (+0x7cc8); §7h L420 Elsword 25 functions 32cf8230..32d06c50 (e.g. 32d01040): UPDOWN_CANCEL.
