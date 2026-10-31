# June character Chung (Stage 7) - Phase CH0b - Every June Chung asset, whole, first

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH0b` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH0 |
| Flag | none, unless CH0 lists a March reader a June asset breaks: then `SERV_IRUHADEV_JUNE_CHAR_CHUNG_ASSET` |
| Log tag | `[JUNE_CHAR]` (`CHUNG DATA`); `ErrorLog.txt` compared against a pre-CH0b run |
| Review | none (the play-test is the gate); a reader fix gets `CH0b-review.md` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0b - Every June Chung asset, whole, first"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH0b - Done (2026-10-26)"
```
Then CH0's Done note and `june2017_re/char_chung/ch0b_stage_list.tsv`.

## Task

Do "Phase CH0b", Eve's way (her CH0b Done note is the worked example):
1. Save `data\ErrorLog_pre_CH0b_chung.txt`.
2. `kom.py stage-june --dry-run` then the real run over `ch0b_stage_list.tsv`, byte-exact.
3. Pack each touched kom; ask the user once with the whole kom list; copy what they approve (`data204` is a new archive). **If CH0 found a
   dropped animation March plays,** pack but hold the motion koms for CH1 final and say so.
4. Back up the save, then create the walk units it lacks: Iron Cannon (6), Fury Guardian (20), Shooting Guardian (21), Shelling Guardian (27),
   level 99 with class costume and top gear (`createchar.py`). Existing: 18 Freils (117), 42 RapidT (111), 43 NovaT (110), 50 Chung99 (111).
5. Hand over one play-test: one Chung per job tier, a costume and a weapon, three skills, village and dungeon.

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

## Amended by CH0 (2026-10-31)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7ad L745 L751-755 stage June Motion_<Class>* files with the class script; Motion_CHUNG.x.
- **Stage list `june2017_re/char_chung/ch0b_stage_list.tsv`: 5,093 files** (4,788 missing in March, 305 March copies; 8,518 more are already June bytes). `kom.py stage-june --dry-run`: 35 koms, 5,093 staged (4,789 new, 304 replaced, 0 identical), **0 refused**; one new kom, `data204` (54 files). Routes a / b / c unioned in `ch0_assets.tsv` (13,947 rows); owner rules keep NPC copies (25), Stage 9 UI (11), Stage 17 pet (10) and june-dungeons tutorials (181) out of the list.
- **Hold the class motions for CH1 final (checklist 7ad).** March's script plays all three animations June dropped (`ch0_motion_question.tsv`): `SI_SA_CFG_IRON_HOWLING` (gone from every June file; June's own `CSI_SI_SA_CFG_IRON_HOWLING_HYPER` still names it: JUNE_BUGS row in CH1e), `SI_A_CIP_SUDDEN_BURSTER` and `SI_SA_CIP_BURST_WOLF` (moved CIP -> June `Motion_CHUNG_CFG.x`). Stage and pack everything, but leave the 10 differing class motions (`Motion_CHUNG_CDC / CFG / CHG / CIP / Common / CSG / CTT / Emotion / Field / Lobby .x`) out of the copy until CH1 final; the 53 June-only emotion motions and the rest go now. Say in the hand-off which kom holds them back.
- **Skill icons:** `skillicons.py check` over 106, 900, 26000-26064, 26200-26238, 26400-26420, 26600-26605 = FAIL (133 skills, 22 misses) until `DLG_SKILL_ChungActive_05` (absent), `DLG_SKILL_ChungActive_04` and `DLG_Chung_SKILL_ScreenShot_04` (March `.tet` smaller than June's) are copied: all in the list. Transcendence passives sit on `DLG_SKILL_Passive_13` (26415-26420) and `_12` (106), already June bytes. Gate: the check PASSes after the copy.
- **In the list too:** `POPUP_TR_I_PALADIN / D_CHASER / T_TROOPER` (+ `.tet`, June data215, names from `DLG_Transcendental_Change_Guide_Chung`), `DDS_Chung_CTT_Skill_Normal / _Rage`, `HQ_CutIn_Chung_Basic`, `HQ_CutIn_Chung_CASH_*`. The six `HQ_Cutin_{CIP,CDC,CTT}[_1]_Transcendence.gfx` are already June bytes.
- Effect closure `ch0_effect_closure.tsv`: 7,263 rows, 1,945 names, 64 missing (50 sounds), 15 June never defines.
- The builder is `june2017_re/char_eve/ch0_build_assets.py chung` (CHARS entry `chung`, new optional keys `extra_lua` / `skill_icon_ranges`).
