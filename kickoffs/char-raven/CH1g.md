# June character Raven (Stage 7) - Phase CH1g - Class core: Weapon Taker line

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH1g` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1f review. One at a time with CH1a-CH1k: they all edit `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-raven/CH1g-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1g - Done" "CH1g - Review"
awk -F'\t' '$7=="CH1g"' june2017_re/char_raven/ch0_class_map.tsv   # column as Elsword's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier Raven CH1 Done notes / reviews (their routed gaps). Elsword's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1g` (Weapon Taker line (9 `RSI_WEAPON_TAKER*` incl. the X-X-X loop, `RWT`, the `RWT_` tables)) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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

## Amended by CH0 (2026-11-05)

- **Functions: 35** (`awk -F'\t' '$7=="CH1g"' june2017_re/char_raven/ch0_class_map.tsv`; inside 32, new 3).
  June addresses: `32c26eb0` `32c26ee0` `32c26ef0` `32c26f00` `32c26f10` `32c26f20` `32c26f30` `32c26f70` `32c26f80` `32c26fa0` `32c26fb0` `32c26fd0` `32c26fe0` `32c26ff0` `32c33c30` `32c33fd0` `32c342c0` `32c345f0` `32c34970` `32c34cc0` `32c35020` `32c35330` `32c35440` `32c42160` `32c4aab0` `32c4b290` `32c4b530` `32c526e0`(new ?) `32c528c0` `32c57150` `32c598c0`(new ?) `32c59b60` `32c5b740` `32c5fd90`(new ?) `32c60230`.
- **States: 11** (`awk -F'\t' '$8=="CH1g"' june2017_re/char_raven/ch0_state_skill.tsv`): `RWT_COMBO_X`, `RSI_WEAPON_TAKER_COMBO_XX`, `RSI_WEAPON_TAKER_COMBO_XXX`, `RSI_WEAPON_TAKER_COMBO_XXX_LOOP`, `RSI_WEAPON_TAKER_COMBO_XXX_FINISH`, `RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZX`, `RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZXX`, `RWT_COMBO_ZZZX`, `RSI_WEAPON_TAKER_COMBO_ZZZXX`, `RSI_WEAPON_TAKER_COMBO_ZZZdownX`, `RSI_WEAPON_TAKER_COMBO_ZZZdownXX`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID).
- `32c5fd90` (Weapon Taker X-X-X loop helper) copies a DamageData by value: `31e9cb70` / `31e9bb70` are `DamageData`'s copy ctor / dtor (funcmap): write a plain local copy. The WT combo states call `CreateOverHeatObject` `32c4dfe0` (CH1g2: declare it, `body in CH1g2`).
- **Amended by CH1d (2026-11-05):** `RWT_COMBO_ZZZX` (June id 304) is already a `RAVEN_STATE_ID` member (`_CORE`, appended without `= value`); March registers the Veteran Commander Z-Z-Z-X under `RSI_COMBO_ZZZX`, June under its own id. CH1d's Z-Z-Z (`32c29fd0`) starts it for class 115 with the front key held, else `RSI_WEAPON_TAKER_COMBO_ZZZdownX`.
