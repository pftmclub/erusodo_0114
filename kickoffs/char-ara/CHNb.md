# June character Ara (Stage 7) - Phase CHNb - CH-N (b): the job-change route to Little Specter and Asura

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CHNb` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CHNa review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_CLASS` (+ `..._CLASS_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA CLASS`), `[ADMIN_CHEAT]` |
| Review | `kickoffs/char-ara/CHNb-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Context" "Rules" "Phase CH-N - Ara's June-only class path: Little Specter (34) and Asura (124)" "Open decisions (for the user; asked at the end of planning)"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Phase CH-N - A June-only later class path (template; not run for Aisha)"
python tools/docsection.py JUNE_CHARACTER_LUCIEL_PHASES.md "Open decisions (for the user)"   # Lu/Ciel's stat-row answer (user, 2026-11-02): the precedent
cat june2017_re/char_ara/ch0_classn.tsv
```

## Task

Do CHNb from `ch0_classn.tsv`: June's quests 35560-35563 (Martial Artist level 15 -> Little Specter, `CJT_JOB_THIRD`) and 35564-35568 (Little
Specter level 35 -> Asura) in March's `FieldQuest.lua` (if March lacks them, adopt June's rows by the core-list rule, `luasrc.py core --add ...
--why "edit: char-ara CHNb"`); the reward's `m_eChangeUnitClass` through `Handlers_Quest.cpp:586-620`; `X2OfflineClassChangeSeed.h` rows for 34 / 124
(promotion items 41794 / 41796 and 41803 / 41805, parent rows 34 -> 7 and 124 -> 34); `DLG_Class_Change_Ara_LS` / `_AS` and the class-change guide
dialogs (staged by CH0b) with `luabind.py check`. Full build and deploy; play-test on fresh units: a level-15 Martial Artist takes 35560-35563
(`/qdone`) and becomes Little Specter; a level-35 Little Specter takes 35564-35568 and becomes Asura; both survive a relog; no other class changed.
You are on Opus: delegate the reads, greps, probes and builds. CP949 sources: `bytepatch.py` only.

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
