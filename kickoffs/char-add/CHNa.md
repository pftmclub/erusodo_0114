# June character Add (Stage 7) - Phase CHNa - CH-N (a): Arc Tracer, Time Tracer, Mastermind and Diabolic Esper load and play

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-add CHNa` |
| Plan doc | `JUNE_CHARACTER_ADD_PHASES.md` (branch `main`) |
| Depends on | CH5 review, char-elesis CHNa review (`SERV_IRUHADEV_JUNE_CLASS_PATH` as Ara and Elesis left it); may run beside a CH2 phase (no `X2GUAdd.*` edit unless CH0 says so) |
| Flag | `SERV_IRUHADEV_JUNE_CLASS_PATH` (widened, never copied) and `SERV_IRUHADEV_JUNE_CHAR_ADD_CLASS` (Add rows) (+ `..._CLASS_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ADD CLASS`) |
| Review | `kickoffs/char-add/CHNa-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md --no-sub "Context" "Rules" "Phase CH-N - Add's June-only class paths: Arc Tracer (33) -> Mastermind (123), Time Tracer (36) -> Diabolic Esper (126)" "Open decisions (for the user; asked at the end of planning)"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Phase CH-N - A June-only later class path (template; not run for Aisha)"   # as Ara's Z rewrote it
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CHNa - Done" "CHNa - Review"
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "CHNa - Done" "CHNa - Review"
cat june2017_re/char_add/ch0_classn.tsv
```

## Task

Do CHNa from `ch0_classn.tsv`. `UC_VALIDE_END` is 123 = `UC_ADD_MASTERMIND` and `UC_END` is 124, so Mastermind fails every `< UC_VALIDE_END` test
and Diabolic Esper (126) every `< UC_END` test: each test CH0 lists (minus those Ara and Elesis already fixed) gets June's bound, read from June's own
test, never a guessed end value. The gauge create for all four (`X2GageManager.cpp`); `GetX2UnitClassCompatibility` cases (checklist l.1009, June
`31ed6500`); the second- and third-line helper arms and their callers; the class switches inside `X2GUAdd.cpp`; `X2OfflineLuaEnumSeed.h` names;
`tools/createchar.py` (`CLASSES` / `PARENT` / `PROMO`). **Stat rows** for 33 / 36 / 123 / 126 per Open decision 1 (answered: derive; CH0 item 12
has the rows) into `KncWX2Server/ServerResource/US/StatTable.lua` as a labelled stand-in (name the file in the hand-off; the user packs it).
`Motion_ADD_AT.x` / `_MM.x` / `_TT.x` / `_DE.x` reach the unit. **The creation screen's June-only entries** (user, 2026-11-10 / 2026-11-11): the four
class buttons CH0c left harmless become live, with their previews and `Movie_Create_Add_*`. The units: after a `db_backup`, `createchar.py` a 33 and a
36 (level 20), a 123 and a 126 (level 40), and a level-99 123 and 126 for CH3. Full build and deploy; play-test: all four load in the village and a
dungeon, combos and the DP gauge work, the job name shows, a socketed weapon keeps its socket, the creation screen shows all seven entries.

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

## Amended by char-ara CHNa (2026-11-11)

- What Ara's CHNa landed and you reuse: `SERV_IRUHADEV_JUNE_CLASS_PATH` (socket-group bound 202, `GetClassLevel` = June). Your own flag carries the Add rows.
- **Lines 2 / 3** (June table `337a1660` Add block): first job 33 (`337a194c`) / 36 (`337a1950`), second job 123 (`337a1960`) / 126 (`337a1964`).
- **Stat chart** (June `3254ee40`): 33 / 123 = 2,3,3; 36 / 126 = 3,2,3.
- **Portraits** (`CX2Data::GetCharacterImageName`, `X2Data.cpp`): a class March lacks needs a new index in all nine positional arrays + the switch (Ara took 55 / 56;
  take the next free ones after the last landed port). Rows from June `31ee0de0`: `python june2017_re/char_image_rows.py 33:0x37 123:0x38 36:0x3d 126:0x3e` prints file / piece per CIT type
  (`--check` first: it must reproduce March's Little Devil / Yama Raja rows). Check each piece in the March TET (keys are stored uppercase UTF-16) and stage the June
  textures it names (Ara's were all June's already except data215 `Texture101_NEW` / `_GRAY`).
- **Creation-screen texture (the user saw it blank on Ara):** the class name, story text, radar chart and job text are pieces of
  `DLG_UI_Common_Texture_GUI_character_info_09` (`name_class_<type>_<line>`, `text_class_2_<line>`, `star_graph_class_2_<line>`, `text_class_under_2_<line>`).
  March's copy differs from June's (`.tga` and `.TET` both differ; CH0c stages it first; `kom.py stage-june --dry-run` 2026-11-11): stage June's `.tga` + `.TET`, pack data057, copy. Ara's March copy showed a
  Korean placeholder name and empty text / radar for the third line until June's went in.
- **Stat rows: none.** The user's flat StatTable (2026-11-11) already has every June class id (`statflat.py check`); the per-class rows and the type-fallback log in this kickoff are moot.
