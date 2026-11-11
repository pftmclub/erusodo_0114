# June character Add (Stage 7) - Phase CH1p - Pressed-index space, hooks and June-only keys

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-add CH1p` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH1o review. One at a time with CH1-CH4: they all edit `X2GUAdd.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ADD_LIVE` |
| Log tag | `[JUNE_CHAR]` (`ADD CORE`) where the phase logs; otherwise none |
| Review | `kickoffs/char-add/CH1p-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Rules" "Phase CH0 - Add map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "CH1p - Done" "CH1p - Review"   # the worked example, once written; else Ara's
awk -F'\t' '$7=="CH1p"' june2017_re/char_add/ch0_class_map.tsv
```

## Task

7ag pressed-index space (his decode, `CountUsedSkill`, every reader together; `X2GUUser.cpp`'s "Elesis, Add not ported" set); hooks 7h / 7l / 7o / 7p / 7x; the June-only script keys CH0 gives an owner here (the `INIT_COMPONENT` keys not read by InitComponent, `POSSIBLE_NASOD_BARRIER` if CH0 puts its loader here). One-file probe with `_LIVE` defined; no full build.

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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-add <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
