# June character Raven (Stage 7) - Phase CH0b - Every June Raven asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH0b` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH0; char-elsword CH6's walk passed (two play-tests never share a session); may run before char-elsword Z |
| Flag | none, unless CH0 lists a March reader a June asset breaks: then `SERV_IRUHADEV_JUNE_CHAR_RAVEN_ASSET` |
| Log tag | `[JUNE_CHAR]` (`RAVEN DATA`); `ErrorLog.txt` compared against a pre-CH0b run |
| Review | none (the play-test is the gate); a reader fix gets `CH0b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "Starting context" "Phase CH0b - Every June Raven asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH0b - Done (2026-11-04)"
```
Then CH0's Done note and `june2017_re/char_raven/ch0b_stage_list.tsv`.

## Task

Do "Phase CH0b", Elsword's way (his CH0b Done note is the worked example):
1. Save `data\ErrorLog_pre_CH0b_raven.txt`.
2. `kom.py stage-june --dry-run` then the real run over `ch0b_stage_list.tsv`, byte-exact. June's motions come from June's `data202` (March has
   no `data202.kom`): stage them into the kom CH0 named.
3. Pack each touched kom; ask the user once with the whole kom list; copy what they approve, **except the motion koms: pack them and hold them
   for CH1 final** (March's script plays OT names June's motion lacks), and say so.
4. `python tools/skillicons.py check <Raven's skill-id range>` = PASS; stage what it lists (shared `DLG_SKILL_Passive_*` sheets included).
5. Back up the save, then create the walk units it lacks: classes 4, 16, 17, 25, 107, 115, level 99 with class costume and top gear
   (`createchar.py`). Existing: 23 `ravbe` (106 lv72), 49 `Raven99` (106 lv99).
6. Hand over one play-test: one Raven per job tier, a costume and a weapon, three skills, village and dungeon.

A file the list missed (a missing-file line in the play-test) is staged here and added to `ch0_assets.tsv`. Never keep March's copy to make
something load.

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

## Amended by CH0 (2026-11-05)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7ad L752 stage June Motion_Raven* files with the class script; Raven's OT_Combo.
- **Stage list** `june2017_re/char_raven/ch0b_stage_list.tsv`: 4,825 rows (texture 3,267, mesh 1,249, sound 224, motion 63, other 22), no Lua. CH0 dry run: `STAGE-JUNE DRY RUN: 35 kom(s), 4825 staged (4490 new, 335 replaced, 0 identical), 0 refused`. **One new kom: data202** (June's number, as Eve's data203 / Chung's data204): the 53 June-only emotion motions + `Motion_Standard_Raven.X`.
- **Held for CH1 final:** `Motion_Raven_OT.x` only (`ch1_held_motions.tsv`, data033). March's live script and class code still play `OT_ComboX3Down` / `X4Down` / `X5Down` and `OT_SP1a_ChargedBolt`, which June's OT file renames (`_New`, `Raven_OT_SP1a_ChargedBolt`). The other 9 differing class motions (data033) drop no name and go now (`ch0_motion_question.tsv`). `Motion_Raven.x` is already June's (R12b).
- **Lua is not staged** (June `0x82` bytecode fails the pack gate): adopt June-only `DLG_1ST_Class_Change_Guide_Raven.lua` and `DLG_2ND_Class_Change_Guide_Raven.lua` with `luasrc.py adopt-june` (bytecode, `--dry-run` first; `luabind.py check` each). `DLG_Transcendental_Change_Guide_Raven.lua` and `DLG_Class_Change_Guide_Raven.lua` are already June's bytes. `POPUP_TR_B_MASTER` / `_R_FIST` / `_V_COMMANDER` (.tga + .TET, data215) are in the list.
- **Skill icons:** `skillicons.py check 106 900 24000-24058 24200-24239 24400-24422 24600-24605` = 30 misses in `DLG_SKILL_RavenActive_04` (22), `_05` (4, sheet missing: 24057 / 24058), `DLG_Raven_SKILL_ScreenShot_03` (2), `DLG_SKILL_Passive_12` (2: 24417 / 24421); all four sheets + .tet are in the list: re-run after staging. Effect closure: 68 missing names = 49 sounds (in the list) + 19 June itself does not define (`ch0_effect_closure.tsv`). Save: no unit of 4 / 16 / 17 / 25 / 107 / 115 exists; create them.
