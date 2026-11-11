# June character Elesis (Stage 7) - Phase CH0b - Every June Elesis asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elesis CH0b` |
| Plan doc | `JUNE_CHARACTER_ELESIS_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments); char-ara CH6 walk done (the game and save are in use until then) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELESIS_ASSET` only if CH0 found a March reader a June asset breaks; otherwise none |
| Log tag | `[JUNE_CHAR]` (`ELESIS DATA`) |
| Review | none (the play-test is the check) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md --no-sub "Rules" "Phase CH0b - Every June Elesis asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH0b - Done (2026-11-10)"
cat june2017_re/char_elesis/ch0b_stage_list.tsv | head -5   # then filter by route as the amendment says
```

## Task

Do "Phase CH0b": every non-Lua row of `ch0b_stage_list.tsv` staged byte-exact (`kom.py stage-june`) into the kom CH0 named (held motions packed
and kept for CH1 final), every kom packed (`/elstools pack`) and copied (`/x2-kom-packed copy`; CLAUDE.md 2026-11-10: no ask; if the game runs, ask the
user to close it). The class-change dialogs and guides (`DLG_Class_Change_Elesis_*`, `DLG_1ST` / `2ND_Class_Change_Guide_Elesis.lua`,
`Class_Change_Guide_Elesis.lua`) through `adopt-june` and `luabind.py check`; `LevelUpEvent.lua` is not touched (CHNb), nor is the creation-screen
button dialog (CH5). Save `data\ErrorLog_pre_CH0b_elesis.txt` first. No unit to create: every March class has one (20, 40, 21, 29, 30). Gate:
`python tools/skillicons.py check <Elesis's skill-id range from CH0>` = PASS (stage what it lists; a June-side gap is a `JUNE_BUGS.md` row).

Done gate: the stage list re-runs 0 `missing` / `march-copy`; the play-test (one Elesis per March job tier, village and dungeon) shows no new
`KEM_ERROR` / `Motion Error` / missing texture or mesh against the pre-CH0b log, and the KEM 433 frame warning on her part meshes is gone; a
costume, a weapon and three skills look right. Record each replaced March asset in the Done note and `MODS.md`.

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
