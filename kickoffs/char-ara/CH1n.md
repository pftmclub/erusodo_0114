# June character Ara (Stage 7) - Phase CH1n - Base callbacks and the skill-slot head

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1n` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1m review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` |
| Log tag | `[JUNE_CHAR]` (`ARA CORE`) where the phase logs; otherwise none |
| Review | `kickoffs/char-ara/CH1n-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1n - Done" "CH1n - Review"
awk -F'\t' '$7=="CH1n"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

The `CX2GUUser` base callbacks Ara registers (start from Rena's CH1n triage: only rows earlier characters did not cover);
`SetEquippedSkillFuncBySkillSlotIndex` `32bd09d0` head (bounds, slot tables, the 7i rule, the pre-switches; the slot-state byte stores of checklist
l.444 are dropped: March answers through `IsJuneSkillSlotState`) and the dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`; March's class has
none). Probe with `_LIVE`; no full build.

You are on Opus: delegate the reads, greps, known-address Ghidra dumps, probe compiles and flagcheck runs (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0 (2026-11-10)

- **Functions: 2** (`awk -F'\t' '$7=="CH1n"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 2).
  June addresses: `32bd09d0` `32bd34c0`.
- **Checklist rows: 5** (`awk -F'\t' '$5 ~ /(^| )CH1n( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L263 dev free-skill gate around the cool-time / MP / hyper-stone ; L286 SetSkillLevelStateData hoisted into CX2GUUser; March copy X2; L328 skill-memo entry 31fdbbb0 and the class callers of 31fdbcc0 ; L444 SetEquippedSkillFuncBySkillSlotIndex Ara 32bd09d0: drop the ; L726 types 14-16 / 17 / 18 named in the class switch.
- **Identified callees first needed here: 3** (`awk -F'\t' '$8 ~ /(^| )CH1n( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `31fd8970` `32b4cf30` `32d3ed30`.
- `SetEquippedSkillFuncBySkillSlotIndex` `32bd09d0` switches: pre-switch at `32bd0af5` (V = `0x6a56` / `0x6a6c` -> helper `32babb50` on both states); type switch (type - 5, 12 entries, byte table `32bd282c`, jump table `32bd281c`: 5 -> `ASI_COMMON_BUFF`, 6 / 7 / 13-16 -> id switch, 12 -> `ASI_THROW_ITEM` (id 900), 8-11 exit); id switch (id - 0x6978, 244 entries, byte table `32bd2930`, jump table `32bd2838`, 62 targets, default `32bd271e`); id 27600 by compare (`32bd2685`); hyper switch (id - 0x6bd1, 5 entries, jump table `32bd2a24`). 86 cases hand setup to class helpers `helper( state*, id )`: each helper is `inside` this function with its state's phase.
- **Second switch:** vslot 0x68c `32bd34c0` = `InitAquiredActiveSkillState( skill id )` (March `X2GUUser.h:3039`, `X2GUAra.cpp:16262`; called by `X2ComboAndSkillChainSystem.cpp:98`): 56 states (id - 0x6979, 242 entries, byte table `32bd5150`, jump table `32bd50f4`). Its head is CH1n; its case bodies go with their state phases. `ch0_skill_listing_rows.tsv` (246 rows: 190 from `32bd09d0`, 56 from `32bd34c0`).
