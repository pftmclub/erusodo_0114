# June character Ara (Stage 7) - Phase CH1m - Registration

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1m` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1k review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` |
| Log tag | `[JUNE_CHAR]` (`ARA CORE`) where the phase logs; otherwise none |
| Review | `kickoffs/char-ara/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1m - Done" "CH1m - Review"
awk -F'\t' '$7=="CH1m"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Registration under `_LIVE`: June's InitState wrapper `32bcf4a0` and body `32bca580` against March's `InitState` (`X2GUAra.cpp:278`),
`InitStateCommon` (`:356-1328`) and `InitStateByUnitClass` (`:1329-1512`), from CH0's registration map (every row passed `ch0c_listing_check.py`). The
hyper-mode table names (`GetHyperModeTableName` `31fdd490`) and the berserk table `31fdd800` (only caller `32bca580`, checklist l.978: port it whole
in `CX2GUUser`, the code only Ara calls), START / WIN / LOSE roll from `INIT_COMMON_RANDOM_STATE`, the base event states (bounding states §7 l.472 /
l.509, `USI_WAIT_STATE_CHANGE` l.428), the `_LIVE` gate. One-file probe with `_LIVE` defined; no full build (no play-test).

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


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
