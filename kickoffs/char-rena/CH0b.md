# June character Rena (Stage 7) - Phase CH0b - Every June Rena asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH0b` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH0 |
| Flag | none, unless CH0 lists a March reader a June asset breaks: then `SERV_IRUHADEV_JUNE_CHAR_RENA_ASSET` |
| Log tag | `[JUNE_CHAR]` (`RENA DATA`); `ErrorLog.txt` compared against a pre-CH0b run |
| Review | none (the play-test is the gate); a reader fix gets `CH0b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "Starting context" "Phase CH0b - Every June Rena asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH0b - Done (2026-11-05)"
```
Then CH0's Done note and `june2017_re/char_rena/ch0b_stage_list.tsv`.

## Task

Do "Phase CH0b", Raven's way (his CH0b Done note is the worked example), over CH0's `june2017_re/char_rena/ch0b_stage_list.tsv`:
1. Save `data\ErrorLog_pre_CH0b_rena.txt`.
2. `kom.py stage-june --dry-run` then the real run over every non-Lua row, byte-exact (motions, body / face / hair, equipment, textures, effect meshes,
   sounds, cut-in art, portraits, `POPUP_TR_W_SNEAKER` / `_G_ARCHER` / `_N_ARCHER`, trap NPC meshes). June's motions come from `data201` + `data012` (March
   has no `data201.kom`): stage them into the kom CH0 named. The class-change guide dialogs (`DLG_1ST` / `2ND_Class_Change_Guide_Lena.lua`) go in by
   `adopt-june`, then `luabind.py check`.
3. Pack each touched kom; ask the user once with the whole kom list; copy what they approve. Motion koms CH0 marked held (`ch1_held_motions.tsv`) are
   packed and kept for CH1 final; say so. If CH0 found none, they are copied here.
4. `python tools/skillicons.py check 106 900 23000-23060 23200-23242 23400-23417 23600-23605` = PASS; stage what it lists (shared `DLG_SKILL_Passive_*`
   sheets included); a June-side gap is a `JUNE_BUGS.md` row (Raven's 535).
5. Back up the save (`db_backup`), then create the walk units it lacks (`createchar.py`): classes 3, 12, 13, 102, 114 and a level-99 24, class costume,
   top gear. Existing: unit 12 `reyaa` (103 lv99), unit 35 `TrapRangerT` (24 lv70).
6. Hand over one play-test: one Rena per job tier, village and dungeon, a costume, a weapon and three skills; no new `KEM_ERROR` / `Motion Error` /
   missing texture or mesh against the pre-CH0b log.

A file the list missed is staged here and added to `ch0_assets.tsv`. Never keep March's copy to make something load.

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

## Amended by CH0 (2026-11-07)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7ad L753 stage June Motion_Lire* files with the class script; Lire's ComboX3 / .
- Stage list, motion answer, named items, closure and icons: CH0 Done note, **Assets** paragraph (`ch0b_stage_list.tsv`, `ch0_motion_question.tsv`, `ch1_held_motions.tsv`).
