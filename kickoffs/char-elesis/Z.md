# June character Elesis (Stage 7) - Phase Z - Close-out

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-elesis Z` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH6 |
| Flag | none |
| Log tag | none |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Phase Z - Close-out"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"
```

## Task

Do "Phase Z": the `MODS.md` section `June character: Elesis` (flags, files, data, revert order: exe + data036 + motion koms together;
`SERV_IRUHADEV_JUNE_EL_DASHJUMP_XZ_FIREBALL` recorded as superseded by her `_LIVE` body; any user-packed `StatTable.lua` rows); the stale "flag off"
comments about `SERV_NEW_CHARACTER_EL` corrected if a phase touched those files; no `_DEBUG` flag left defined; checklist rows marked `(Elesis: CHn)`;
the Aisha plan's "Template for Stage 7" gets an Elesis paragraph (what she took against this plan; what CH-N's second run took; how the
listing-only half went); backlog Stage 7 note; the `stage7-plan` STATUS row pointing at `char-add` (if char-add is already planned: set its code
rows ready instead); the `char-elesis` block moved to `STATUS_ARCHIVE.md` with a *Closed plans* row.

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
