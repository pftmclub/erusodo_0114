# June character Ara (Stage 7) - Phase CH1o - Virtuals and SetSkillLevelStateData

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1o` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1n review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` |
| Log tag | `[JUNE_CHAR]` (`ARA CORE`) where the phase logs; otherwise none |
| Review | `kickoffs/char-ara/CH1o-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1o - Done" "CH1o - Review"
awk -F'\t' '$7=="CH1o"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

The virtuals under `_LIVE`: June-only and changed user virtuals Ara overrides (0xd0 `32bc19a0` once CH0 has named it, 0x30 `32ba8180`, 0x28
`32b968f0`, the rest CH0 gives this phase); `SetSkillLevelStateData` (`X2GUAra.cpp:2989`) with June's `( 3201ad10( skill ) + 1.0 )` term (§7
l.286); the gaps the CH1a-CH1k reviews routed here. Probe with `_LIVE`; no full build.

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

- **Functions: 4** (`awk -F'\t' '$7=="CH1o"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; new 4).
  June addresses: `32b92fa0`(new ?) `32b92fb0`(new ?) `32bab110`(new TakeFPGainByHitID) `32bab6b0`(new TakeSkillFPGainByHit).
- **Checklist rows: 1** (`awk -F'\t' '$5 ~ /(^| )CH1o( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L286 SetSkillLevelStateData hoisted into CX2GUUser; March copy X2.
- **Force-power rows: 1** (`awk -F'\t' '$7 ~ /(^| )CH1o( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: unchecked 1).
- **Identified callees first needed here: 1** (`awk -F'\t' '$8 ~ /(^| )CH1o( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `31d29310`.
- 2 rows left by the map (unsettled callers). `GetDefenceRateBySkill` 0x428 `32b956a0` is CH4b (l.940).
