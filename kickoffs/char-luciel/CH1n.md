# June character Lu/Ciel (Stage 8) - Phase CH1n - Class core: Skill head and base callbacks

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-luciel CH1n` |
| Plan doc | `JUNE_CHARACTER_LUCIEL_PHASES.md` (branch `main`) |
| Depends on | CH1m |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_CORE` (compiled, unreferenced) and `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`; no play-test) |
| Review | none of its own (user, 2026-11-10): the CH1o review covers CH1m, CH1n, CH1o; do not write `CH1n-review.md` (any debug define stays on until then) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Rules" "Starting context" "Phase CH1 - Deploy unit: the class core" "Phase CH0 - Lu/Ciel class map (read-only)"
```
Then the planning digests in `june2017_re/char_luciel/` the task names, and the Done notes of the phases this one depends on
(their routed gaps). Verify a digest line before building on it.

## Task

Do the CH1n row: the skill setter `32b46580` (vslot 0x7b4; uncarved in Ghidra unless CH0 carved it: use the capstone dump
`ghidra/disasm_32b46580_SetEquippedSkillFunc_capstone.txt`), every case with the states it registers, and the base callbacks it hands to.
Exact case count in the Done note.

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

## Amended by CH0 (2026-11-02)

- **Functions: 1 to transcribe** (+ 0 thunks) (`awk -F'\t' '$6=="CH1n"' june2017_re/char_luciel/ch0_class_map.tsv`; all new).
  June addresses: `32b46580` SetEquippedSkillFuncBySkillSlotIndex.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L261 dev free-skill gate around the cool-time / MP / hyper-stone block of the skill-u; §7b L325-326 skill-memo entry 31fdbbb0 / class callers of 31fdbcc0; §7i L427-442 SetEquippedSkillFuncBySkillSlotIndex: drop the 31fd2af0 snapshot call, set state; §7ae L729-733 June skill types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTi; §7ag L802-814 pressed-index space 5 + 5: every > 3 / -4 decode; slot id + 1 decode.
- `ch0_skill.tsv` (listing emulator `ch0_skill_listing_emu.py`; the setter is not carved): **64 cases, 112 skill ids**; slot state ids Lu 50-59, Ciel 60-69 (SLOTB). 12 states are not set here: 4 are registered in InitStateCommon (`LSI_SA_LLC_DARK_CRESCENT`, `_RUINOUS_IMPACT`, `LSI_SA_LCG_ESCHATON`, `_FANTASIE_IMPROMPTU_2ND_MOVEMENT`), the transform states 30044-30046 / 30050-30052 via vslot 0x57c `GetTransformedSkillStateID`. Check skill 30023 (Noblesse, state named LCG, linked by tail name).
