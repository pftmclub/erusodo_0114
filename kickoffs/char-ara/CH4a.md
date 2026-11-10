# June character Ara (Stage 7) - Phase CH4a - Passives, the 0x780 hook and delegates

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH4a` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH3d review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_PASSIVE` (+ `..._PASSIVE_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA PASSIVE`) |
| Review | none of its own: the CH4b review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH4a - Done (2026-11-10)"
```

## Task

Do CH4a: `InitPassiveSkillState` `32bcd9c0` (vslot 0x690) whole on June's ids and Ara's passive ids, their objects and readers; the passive /
attack hook 0x780 `32bca010` whole (checklist l.330; Ara reaches it instead of a 0x2b8 override); the delegate registrations (l.920: `32bcd9c0`,
`32bb0d10`) replacing March's own pushes (l.930); death prevention with 27409 (l.258, the `32bc5a50` test, in `CX2GUUser::UpNowHp` / `SetNowHp`: widen
the existing gate); `GetReduceDebuffDurationRate` `32bd32f0`'s 27415 tail (l.267, the `SERV_IRUHADEV_JUNE_BUFF_GROUP` block); the plan's passive-id
check. Play-test per passive (CH0 names the stat or log line). No review of its own: CH4b's covers it; `_PASSIVE_DEBUG` stays on. You are on Opus:
delegate the reads, probes and builds.

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
