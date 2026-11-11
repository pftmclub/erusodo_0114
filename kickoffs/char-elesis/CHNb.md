# June character Elesis (Stage 7) - Phase CHNb - CH-N (b): the job-change route and the class-change dialogs

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CHNb` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CHNa review |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_CLASS` (+ `..._CLASS_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS CLASS`), `[ADMIN_CHEAT]` |
| Review | `kickoffs/char-elesis/CHNb-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Context" "Rules" "Phase CH-N - Elesis's June-only class path: Dark Knight (35) and Crimson Avenger (125)" "Open decisions (for the user; asked at the end of planning)"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CHNa - Done" "CHNa - Review" "CHNb - Done" "CHNb - Review"   # the first CH-N run: what is already class-generic
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Phase CH-N - A June-only later class path (template; not run for Aisha)"
cat june2017_re/char_elesis/ch0_classn.tsv
```

## Task

Do CHNb from `ch0_classn.tsv`: June's quests 35569-35572 (Knight level 15 -> Dark Knight, `CJT_JOB_THIRD`; reward items 109999 + 293720) and
35573-35577 (Dark Knight level 35 -> Crimson Avenger; 110850 + 293721) in the shipped `FieldQuest.lua` (CH0 decoded them; if a row is missing,
adopt June's by the core-list rule); the reward's `m_eChangeUnitClass` through `Handlers_Quest.cpp`; `X2OfflineClassChangeSeed.h` parent rows only if
June's cash route needs them (a generated file: regenerate, never hand-edit; no Elesis row today means "do not refuse").

**The class-change dialogs (user, 2026-11-11).** Today a level-15 Knight gets March's old `Class_Change_Guide_Elesis.lua` with two paths.
`LevelUpEvent.lua`: the level-15 row for `UC_ELESIS_KNIGHT` opens `DLG_1ST_Class_Change_Guide_Elesis.lua` (three paths, with June's voice lines),
the level-35 row lists Saber Knight, Pyro Knight **and Dark Knight** and opens `DLG_2ND_Class_Change_Guide_Elesis.lua`
(`june_src_fixed/data036_kom/LevelUpEvent.src.lua` L107-116, L324-335; core-list rule: `luasrc.py core --add ... --why "edit: char-elesis CHNb"` if
the file is not source yet). The per-class dialogs `DLG_Class_Change_Elesis_DK` / `_CA` and June's `_SK` / `_PK` / `_GM` / `_BH` (staged by CH0b),
each through `luabind.py check`; every June function the dialogs' buttons call is bound or ported whole.

Full build and deploy; play-test on fresh units: a level-15 Knight sees the three-path dialog, takes 35569-35572 (`/qdone`) and becomes Dark
Knight; a level-35 Dark Knight sees the second dialog, takes 35573-35577 and becomes Crimson Avenger; a second level-15 Knight reaches Saber Knight
and a third Pyro Knight from the same dialog; all survive a relog; no other class changed.

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
