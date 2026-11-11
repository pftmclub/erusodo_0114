# June character Elesis (Stage 7) - Phase CH1m - Registration

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH1m` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH1k review. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_LIVE` |
| Log tag | `[JUNE_CHAR]` (`ELESIS CORE`) where the phase logs; otherwise none |
| Review | `kickoffs/char-elesis/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH0 - Elesis map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1m - Done" "CH1m - Review"   # the worked example (Ara's matching sub-phase)
awk -F'\t' '$7=="CH1m"' june2017_re/char_elesis/ch0_class_map.tsv   # CH0 confirms the column
```

## Task

Registration under `_LIVE`: June's InitState wrapper `32b87a60` and its parts `32b81be0` (111 state names) and `32b864f0` (57, the second-job
combo states) against March's `InitState` (`X2GUEL.cpp:247-310`), `InitStateCommon` (`:2004-2743`) and `InitStateByUnitClass` (`:2744-2970`), from
CH0's registration map (every row passed `ch0c_listing_check.py`); the wrapper's class switch (`sub eax,8 ; cmp eax,0x75`, 5 targets) with arms for
35 / 125 by name. The hyper-mode table names (`GetHyperModeTableName` `31fdd490`), the START / WIN / LOSE roll (`31fe22d0`), the base event states
(`3202b110`; bounding states §7 l.472 / l.509, `USI_WAIT_STATE_CHANGE` l.428 with her own `ESI_WAIT_STATE_CHANGE`), the June-only `EL_STATE_ID`
members CH1a-CH1k declared (registered by their June script names), the `_LIVE` gate. If June's `ReInit` tests game type 1 only, add the
`CX2GUEL::ReInit` `GT_PVP` allow row with the June address, as Ara's. One-file probe with `_LIVE` defined; no full build (no play-test).

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
