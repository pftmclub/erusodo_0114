# June character Chung (Stage 7) - Phase CH1a - Class core: movement, guard and slot-head states

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH1a` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments); may run beside CH0b. One at a time with CH1b-CH1l4: they all edit `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` (`new` rows, compiled) and `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`inside` rows; `//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | `kickoffs/char-chung/CH1a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
awk -F'\t' '$7=="CH1a"' june2017_re/char_chung/ch0_class_map.tsv
awk -F'\t' '$8=="CH1a"' june2017_re/char_chung/ch0_state_skill.tsv
awk -F'\t' '$5 ~ /CH1a/' june2017_re/char_chung/ch0_checklist_rows.tsv
```
Then CH0's Done note and the earlier CH1 Done notes / reviews (their routed gaps). Eve's matching sub-phase is a worked example of the
reading, not of the naming (hers wrote `June_*` members; Chung never does).

## Task

Transcribe every function the class map gives phase `CH1a` (wait, walk, jump (4), dash (4), charge MP, throw item, power attack, guard (3) and the `CHUNG_STATE_ID_LIST_ABLE_TO_BE_CHANGED_TO_GUARD_STATE` key, common buff, hyper) from Ghidra, **whole**. The plan's "Transcription rules
(CH1a-CH1l4)" apply:
- **`inside` rows:** June's body inside March's `CX2GUChung` function under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, March's body in `#else`,
  `#endif SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`. Add `_LIVE` to `Always.h` as `//#define` if no earlier phase did. Prove the June path compiles: one
  `cl /c` of `X2GUChung.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (the build's own flags), result in the Done note.
- **`new` rows:** a plain name in the class's pattern (CH0 proposed it) under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, compiled in every build,
  referenced by nothing yet. Leave `InitState*` alone (CH1m registers).
- A class-range callee (`0x32bd86f0`-`0x32c25a1f`) owned by a later group gets a declaration with `// June <va>: body in <phase>`; an engine callee
  is called through its March counterpart (`funcmap.tsv`, `junediverge.py --list`); an unmapped engine callee is "this needs Opus": stop and say so.
- June's numbers, order and quirks verbatim (a `JUNE_BUGS.md` row each); `m_fNowStatePowerRate` (7i); June's BFI arguments (7ai,
  `s3e_bybfi_callers.tsv`).
- `X2GUChung.*` are CP949 (`.cpp` CRLF, `.h` mixed): `bytepatch.py` only; check each `#ifdef` branch with `cl /P`.
- Done gate: every class-map row with phase `CH1a` is written (the Done note lists them with June addresses and `inside` / `new`); the build passes
  with `_LIVE` undefined and the one-file probe passes with it defined; `junediverge.py --check` and `enumscan.py --check` on the class files PASS.
  Nothing to play-test: the hand-off says so and names the next phase.

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

## Amended by CH0 (2026-10-31)

- **Functions: 29** (`awk -F'\t' '$7=="CH1a"' june2017_re/char_chung/ch0_class_map.tsv`; inside 28, new 1).
  June addresses: `32bd8930` `32bd8960` `32bd8980` `32bd89b0` `32bd8a50` `32bd8a60` `32bdbcd0`(new CSI_JUMP_UP_StateStart) `32bdca80` `32bdcac0` `32bdcaf0` `32bdcb10` `32bdcb50` `32bdcfc0` `32bdd1d0` `32beb1b0` `32beb260` `32bef2d0` `32bef450` `32bef6f0` `32bf2b50` `32bf5de0` `32bf60e0` `32bf62f0` `32bf65e0` `32bf67e0` `32bf6ae0` `32bf6d30` `32bf7250` `32c03880`.
- **States: 18** (`awk -F'\t' '$8=="CH1a"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_WAIT_STATE_CHANGE`, `CSI_START_INTRUDE`, `CSI_WAIT`, `CSI_WALK`, `CSI_JUMP_READY`, `CSI_JUMP_UP`, `CSI_JUMP_DOWN`, `CSI_JUMP_LANDING`, `CSI_DASH`, `CSI_DASH_END`, `CSI_DASH_JUMP`, `CSI_DASH_JUMP_LANDING`, `CSI_POWER_DOWN_LANDING`, `CSI_THROW_ITEM`, `CSI_COMMON_BUFF`, `CSI_GUARD`, `CSI_GUARD_ATTACK_Z`, `CSI_GUARD_ATTACK_X`.
- **Skills: 1** (part 2 of `ch0_state_skill.tsv`, col 6): 900 SI_ETC_WS_COMMON_LOVE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L253 class code scales the cached state power rate (+0x7cc8).
- Guard key `CHUNG_STATE_ID_LIST_ABLE_TO_BE_CHANGED_TO_GUARD_STATE`: March already reads it (`LoadStateIDCanBeChangedGuardState`, an `inside` helper).
- Cannon-ball counter calls (`32bf3e40`, `32bf5280`, `32c24c80`) are CH1q's: declare them with `// June <va>: body in CH1q`.
