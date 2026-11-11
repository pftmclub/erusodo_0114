# June character Ara (Stage 7) - Phase CH6 - Whole-character walk and gap reruns

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase char-ara CH6` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH4b review, CH5 review, CHNb review (CH5 and CHNb reviews done 2026-11-11) |
| Flag | none (a defect goes back to its phase's flag) |
| Log tag | `[JUNE_CHAR]` (`ARA *`) |
| Review | none (the user's walk is the check) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Phase CH6 - Whole-character walk and gap reruns"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Template for Stage 7"   # the transcendence rows and the transition rule
```

## Task

Do "Phase CH6": back up the save; units of 7 / 28 / 31 / 34 / 118 / 119 / 124 and one transcended per second job. Re-run `census.py`,
`charport_scan.py`, `charport_keys.py Ara_Martial_Artist` and `s3_no_state.py --family X2GUAra.cpp --preprocess`; before / after numbers in the Done note
(0 unexplained `no_state`). Hand the user the walk: every skill in an A and a B slot (each cast twice), a hyper, force power (fill, spend, FP cancel,
gauge) and berserk, a Specter state on 34 / 124, the passives, one dungeon per job, both job changes on fresh units (CHNb's quests; the level-15 and level-35 guides open filled: `CLASS GUIDE 1st` / `2nd` / `2nd skills`), then leave the game
and return to the village; **the becoming-transcended transition** on un-transcended 118 / 119 / 124 by `/tc 1`, `/qdone 42030`, `/additem 288493 1`
(guide popup with the class picture, new skill-tree rows, HUD portrait, relog).


## Amended by CH0 (2026-11-10)

Nothing in the CH0 maps carries this phase's id; the plan section is the scope.

## Amended by CH5 (2026-11-11)

The walk adds the checks CH5 left to it. Cut-ins: on each Ara job cast a skill that reaches the cut-in and read `[JUNE_CHAR] ARA UI cut-in set <set> name <gfx>` (set 0 = jobs 7 / 28 / 31 `HQ_CutIn_Ara_01` / `_11`;
set 1 = 118 `HQ_CutIn_Ara_SD_01` / `_11`; set 2 = 119 `_YR_`; set 3 = 124 `_AS_`; the `_11` entry is the full hyper-mode cut-in), and the awakening hyper / berserk entry on 118 / 119 / 124
(`[JUNE_CHAR] ARA UI hyper cut-in class <c> name HQ_Cutin_AS{D,R}_Transcendence`, only when the class script's awakening state sets `SHOW_HYPER_CUT_IN_TIME`). Creation screen: open it, pick Ara, click
Little Hsien / Sakra Devanam / Little Devil / Yama Raja and read each preview and movie (`Movie_Create_Ara_*`); the 34 / 124 buttons are CHNa's. Lobby: Little Devil's orb shows once (`ALD_LobbyWait`).
