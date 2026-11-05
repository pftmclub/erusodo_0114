# June character Raven (Stage 7) - Phase CH1a - Class core: movement and slot-head states

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH1a` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments), char-elsword Z. One at a time with CH1a-CH1k: they all edit `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-raven/CH1a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1a - Done" "CH1a - Review"
awk -F'\t' '$7=="CH1a"' june2017_re/char_raven/ch0_class_map.tsv   # column as Elsword's; CH0's amendment corrects it
```
Then CH0's Done note and the earlier Raven CH1 Done notes / reviews (their routed gaps). Elsword's matching sub-phase is the worked example.

## Task

Transcribe every function the class map gives phase `CH1a` (wait (2) + wait-state-change, walk, jump (6), dash (12), throw item, common buff, hyper, `SUPER` (3) unless CH0 places it elsewhere) from Ghidra, **whole**. The plan's "Transcription rules (CH1a-CH1k)" apply:
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
- Done gate: every class-map row with phase `CH1a` is written (the Done note lists them with June addresses and `inside` / `new`); the build
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

- **Functions: 19** (`awk -F'\t' '$7=="CH1a"' june2017_re/char_raven/ch0_class_map.tsv`; inside 19).
  June addresses: `32c25c90` `32c25cc0` `32c25d60` `32c25d90` `32c28aa0` `32c28b80` `32c3d2b0` `32c3d3b0` `32c3d3f0` `32c40dd0` `32c40ea0` `32c40f50` `32c43f90` `32c441c0` `32c443f0` `32c446a0` `32c448a0` `32c44b60` `32c44d80`.
- **States: 16** (`awk -F'\t' '$8=="CH1a"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_WAIT_STATE_CHANGE`, `RSI_THROW_ITEM`, `RSI_WAIT`, `RSI_WALK`, `RSI_JUMP_READY`, `RSI_JUMP_UP`, `RSI_JUMP_DOWN`, `RSI_JUMP_LANDING`, `RSI_DASH`, `RSI_DASH_END`, `RSI_DASH_JUMP`, `RSI_DASH_JUMP_LANDING`, `RSI_DASH_JUMP_POWER_LANDING`, `RSI_HYPER_MODE`, `RSI_SI1_COMMON_BUFF`, `RSI_SI1_COMMON_BUFF_HYPER`.
- **Skills: 1** (part 2 of `ch0_state_skill.tsv`, col 6): 900 SI_ETC_WS_COMMON_LOVE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7h L428 USI_WAIT_STATE_CHANGE registered in every class InitState.
- The three `RSI_SUPER_DASH*` tables are dead data in June (no registration): nothing for `SUPER` here. `RSI_COMMON_BUFF` is here (skill type 12).
