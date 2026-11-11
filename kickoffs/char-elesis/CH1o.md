# June character Elesis (Stage 7) - Phase CH1o - Virtuals under _LIVE

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH1o` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH1n review. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` |
| Log tag | `[JUNE_CHAR]` (`ELESIS CORE`) where the phase logs; otherwise none |
| Review | `kickoffs/char-elesis/CH1o-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH0 - Elesis map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1o - Done" "CH1o - Review"   # the worked example (Ara's matching sub-phase)
awk -F'\t' '$7=="CH1o"' june2017_re/char_elesis/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

The virtuals under `_LIVE`: June-only and changed user virtuals Elesis overrides that no transcription phase owns (the June-only hook 0x83c
`32b732e0` (base `31f3f070` = `ret`: add the empty base virtual and its call site from June), 0x670 `32b64290`, the rest CH0 gives this phase);
`SetSkillLevelStateData` with June's `( 3201ad10( skill ) + 1.0 )` term (§7 l.286) if her June class has it; the gaps the CH1a-CH1k reviews
routed here, incl. every function a Sonnet phase marked "this needs Opus". Probe with `_LIVE`; no full build.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-elesis <phase>` comment (at the start of an existing one-line comment, never a second line).
