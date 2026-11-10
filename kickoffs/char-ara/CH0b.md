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


## Amended by CH0 (2026-11-10)

- **Checklist rows: 1** (`awk -F'\t' '$5 ~ /(^| )CH0b( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L760 stage June Motion_ARA* files with the class script; skill an.
- **Specter / tail rows: 2** (`awk -F'\t' '$7 ~ /(^| )CH0b( |$)/' june2017_re/char_ara/ch0_specter.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- **CH-N (class path) rows: 1** (`awk -F'\t' '$7 ~ /(^| )CH0b( |$)/' june2017_re/char_ara/ch0_classn.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- Stage list `june2017_re/char_ara/ch0b_stage_list.tsv` 5,886 rows; dry run `STAGE-JUNE DRY RUN: 34 kom(s), 5886 staged (5562 new, 324 replaced, 0 identical), 0 refused`; one new kom **data205** (56 June-only motions: `Motion_Ara_LS.x`, `Motion_Ara_AA.x`, `Motion_Standard_Ara.X`, 53 emotions): pack it and copy it as a new kom (Rena's data201 precedent). Differing class motions -> data033; `Motion_Ara_Tail_03.x` / `_06.x` -> data131.
- Held for CH1 final, not in the list (`ch1_held_motions.tsv`): `Motion_Ara_LD.x` (drops `SI_SA_ALD_ENERGY_WAVE_Start` / `_End`: June's file has `_START` / `_END`, March's lookup is case-sensitive) and `Motion_Ara_SD.x` (drops `SI_SA_ASD_FallingBlossom_Start` / `_Finish`). `_ARA`, `_Field`, `_Emotion`, `_LH`, `_YR` go now.
- `adopt-june` (bytecode, then `luabind.py check`): the 7 `CH0b-adopt` rows of `ch0_assets.tsv` (col 3 owner/phase): `DLG_Class_Change_Ara_LH` / `_LD` / `_LS` / `_SD` / `_YR` / `_AS`, `DLG_Class_Change_Guide_Ara`; plus `DLG_1ST_Class_Change_Guide_Ara.lua` / `DLG_2ND_Class_Change_Guide_Ara.lua` (data034, absent in March; CHNb wires them through LevelUpEvent).
- `POPUP_TR_S_DEVANAM` / `_Y_RAJA` / `_ASURA` (+ `.TET`, data215) are in the list. Movies (`Movie_Create_Ara_AA/_Base/_SD/_YR.avi`) are loose and identical: nothing to stage. All 41 cut-in `.gfx` are already June's; 31 `HQ_CutIn_Ara_*` textures are in the list.
- Icon gate: `python tools/skillicons.py check 106 900 27000-27066 27200-27244 27400-27420 27600-27605`. Before staging: 74 misses on 5 sheets, all 10 sheet files in the list. One June gap: 27414 piece `SI_P_AA_SPIRITUAL_EYE` absent from June's own `DLG_ARA_SKILL_ScreenShot_04.TET` -> `JUNE_BUGS.md` row + `skillicons_june_gaps.tsv` row.
- Effect closure `ch0_effect_closure.tsv` 7,600 rows: 169 unique missing, 136 files all in the list; 33 are names June itself lacks (dangling).
- Walk units to create (none exist): classes 7, 28, 118, and a level-99 31 (`createchar.py`, after a `db_backup`).
