# June character Ara (Stage 7) - Phase CH2b0 - Lock-on: ProcessLockOn (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH2b0` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1 review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LOCKON` |
| Log tag | none unless the phase logs |
| Review | `kickoffs/char-ara/CH2b0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ac. CanDown's June call shape in class code (from June engine W1f)"
cat kickoffs/char-luciel/CH3c20.md
```

## Task

Port June's `SLockOnSystem::ProcessLockOn` `3200fa20` whole inside March's (`X2GUAra.cpp:5366-5476`, declared `X2GUAra.h`), March's in `#else`:
June's `CanDown( pos, r, pOutPos, NULL, &lineIndex, ... )` shape at `@3200ff98` (the 9-arg overload from world W1f), the candidate-loop `CanDown` at
`@3200fcad` that clears a flag when it fails (March has none), and the Ara landing helper `32bc0420` (new, plain name). June's function is shared with
Lu/Ciel (callers `32b07a40`, `32b15290`): keep it where March has it and amend `kickoffs/char-luciel/CH3c20.md` to "hoist only: Ara's CH2b0 holds
June's body" (one line, reason in this Done note). Funcmap rows `Char Ara CH2b0`. Probe compile; no full build (CH2b's play-test exercises it).
You are on Opus: delegate the reads and probes. CP949 sources: `bytepatch.py` only.

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
