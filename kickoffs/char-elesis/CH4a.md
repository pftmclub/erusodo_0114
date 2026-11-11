# June character Elesis (Stage 7) - Phase CH4a - Passives, the 0x83c hook and delegates

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CH4a` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH3d review. One at a time with CH1-CH4: they all edit `X2GUEL.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_PASSIVE` (+ `..._PASSIVE_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS PASSIVE`) |
| Review | none of its own: the CH4c review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH4a - Done"   # the worked example
cat june2017_re/char_elesis/ch0_passive_ids.tsv
```

## Task

Do CH4a: `InitPassiveSkillState` `32b88300` (vslot 0x690, 4,453 B) whole on June's ids (March `X2GUEL.cpp:350-659`, class arms `:414` / `:479` /
`:533` / `:612`, plus arms for 35 / 125), her passive ids, their objects and readers, and its 7 attack-result delegate pushes replacing March's own
(checklist l.920 / l.930; the Blood Runaway delegate's body `32b87880` is CH4c's: push it under `_BLOOD`); the June-only hook 0x83c `32b732e0`
(skill-level read, buff erase by BTI, weapon extra-damage type; CH1o added the base virtual); 0x29c `32b6fc40` (the `GetReduceDebuffDurationRate`
shape, in the `SERV_IRUHADEV_JUNE_BUFF_GROUP` block); the class float writer `32b73bf0` / reader `32b7d550` (l.496); the plan's passive-id check
(`ch0_passive_ids.tsv`: March 8000s against June 28000s). Play-test per passive (CH0 names the stat or log line). No review of its own: CH4c's
covers it; `_PASSIVE_DEBUG` stays on. You are on Opus: delegate the reads, probes and builds.

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
