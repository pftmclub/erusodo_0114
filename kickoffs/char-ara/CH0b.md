# June character Ara (Stage 7) - Phase CH0b - Every June Ara asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH0b` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH0 (and its amendments); char-rena CH6 walk done (the game and save are in use until then) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_ASSET` only if CH0 found a March reader a June asset breaks; otherwise none |
| Log tag | `[JUNE_CHAR]` (`ARA DATA`) |
| Review | none (the play-test is the check) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0b - Every June Ara asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH0b - Done (2026-11-07)"
cat june2017_re/char_ara/ch0b_stage_list.tsv | head -5   # then filter by route as the amendment says
```

## Task

Do "Phase CH0b": every non-Lua row of `ch0b_stage_list.tsv` staged byte-exact (`kom.py stage-june`) into the kom CH0 named (held motions packed
and kept for CH1 final), every kom packed (`/elstools pack`) and copied (`/x2-kom-packed copy`; CLAUDE.md 2026-11-10: no ask; if the game runs, ask the
user to close it). The class-change dialogs through `adopt-june` and `luabind.py check`. Save `data\ErrorLog_pre_CH0b_ara.txt` first. After a
`db_backup`, create the walk units the save lacks: classes 7, 28, 118 and a level-99 31 (`createchar.py`; 34 / 124 wait for CHNa). Gate: `python
tools/skillicons.py check <Ara's skill-id range from CH0>` = PASS (stage what it lists; a June-side gap is a `JUNE_BUGS.md` row).

Done gate: the stage list re-runs 0 `missing` / `march-copy`; the play-test (one Ara per March job tier, village and dungeon) shows no new
`KEM_ERROR` / `Motion Error` / missing texture or mesh against the pre-CH0b log; a costume, a weapon and three skills look right. Record each
replaced March asset in the Done note and `MODS.md`.


## Amended by CH0

(CH0 fills this: the filter with real column numbers, the function / state / skill counts and June addresses, checklist rows, any split.)
