# June character Elesis (Stage 7) - Phase CHNa - CH-N (a): Dark Knight and Crimson Avenger load and play

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elesis CHNa` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH5 review, char-ara CHNa review (the class-generic flag), Open decision 1; may run beside a CH2 phase when no `X2GUEL.*` hunk overlaps |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_CLASS` (her rows) and Ara's `SERV_IRUHADEV_JUNE_CLASS_PATH` where a class-generic test needs widening (+ `..._CLASS_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELESIS CLASS`) |
| Review | `kickoffs/char-elesis/CHNa-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Context" "Rules" "Phase CH-N - Elesis's June-only class path: Dark Knight (35) and Crimson Avenger (125)" "Open decisions (for the user; asked at the end of planning)"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CHNa - Done" "CHNa - Review" "CHNb - Done" "CHNb - Review"   # the first CH-N run: what is already class-generic
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Phase CH-N - A June-only later class path (template; not run for Aisha)"
cat june2017_re/char_elesis/ch0_classn.tsv
```

## Task

Do CHNa from `ch0_classn.tsv` (rows marked `Elesis`; the rows marked `covered by Ara CHNa` are only verified for 35 / 125: the socket-loader
bound, `GetClassLevel`, the optional log sites). **The gauge create first:** `X2GageManager.cpp:194-209` gets the 35 / 125 arms (without them
`CX2GUEL::CreateGageData` `X2GUEL.cpp:1970` / `GetELGageData` cast a plain gauge to `CX2ElesisGageData`: a crash). Then
`GetX2UnitClassCompatibility` (`X2Unit.h:2057-2123`, June `31ed6500`: read June's arms for 35 and 125; divergence row 365); the third-line arms of
`GetFirstLineElesisClassTypeByUnitClassType` / `GetSecondLine...` and their callers (`X2Unit.h:2685-2750`, assert-only today); the class switches
inside `X2GUEL.cpp` (`:213`, `:249`, `:411`, `:2747`, `:9581`) and the class compares; `X2Data.cpp:2872-2892`; `X2ComboTree.cpp:302`;
`tools/createchar.py` (`CLASSES` / `PARENT`). **Stat rows** per Open decision 1 (if (a): no rows, add the log line on the type fallback in
`X2OfflineStatTable.cpp`; if (b) / (c): the rows into `KncWX2Server/ServerResource/US/StatTable.lua`, server Lua: name the file in the hand-off, the
user XORs and packs it into data036). `Motion_EL_DK.x` / `MOTION_EL_CA.x` reach the unit.

**Creation screen, third line (user, 2026-11-10 and 2026-11-11).** Dark Knight and Crimson Avenger on the creation screen: the helper arms above,
`GetUnitStatInfomationChart` arms (`X2StateCreateUnit.cpp:3870-3885`), the previews and `Movie_Create_Elesis_CA`; remove CH5's hide lines if it
added any.

The units: after a `db_backup`, `createchar.py` a 35 (level 20), a 125 (level 40) and a level-80 125. Full build and deploy; play-test: both load
in the village and a dungeon, combos and the Way of the Sword gauge work, the job name shows, a socketed weapon keeps its socket, and the creation
screen shows all six Elesis entries with previews.

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
