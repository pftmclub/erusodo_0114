# June quest system - Phase RM1 - Random mission port

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase quest-system RM1` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | RM0 (and the user's answer), Q4 review |
| Flag | `SERV_IRUHADEV_JUNE_RANDOM_MISSION` |
| Log tag | `[JUNE_QUEST] RM` (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | `kickoffs/quest-system/RM1-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Phase RM1 - Random mission port" "Phase RM0 - Random mission map and source proposal" "RM0 - Done (2026-10-19)"
```

Then read `june2017_re/quest_plan/rm0_map.tsv` (every site, with its June address and March function) and the `(Quest RM0)` rows in
`june2017_re/funcmap.tsv`.

## User's answer in RM0 (2026-10-19): dormant + cheat

- **Client side, whole, as rm0_map.tsv lists it.** This covers:
  - the manager singleton and the mission object;
  - the 0x492 handler;
  - every raise site in a March class;
  - the start / tick / end / reset sites;
  - the UI (both dialogs, the textures and the popup fade);
  - the completion send (0x493 with the mission id; 0x494 as a wait entry);
  - the three cheat binders on `CX2Game` (`SendRandomMissionCheat` / `RewardEXPCheat` / `RewardBuffCheat`, which send 0x495 / 0x496 / 0x497).
  - June bugs stay verbatim with a `JUNE_BUGS.md` row.
- **Normal play never sets a mission.** That is June's absent-value default. The emulator never sends 0x492 on its own, and there is no stand-in schedule.
- **Emulator, cheat path only:**
  - **0x495 { a, b, c }** -> reply with a 0x492. The meaning of a / b / c was server-side. Propose a labelled reading to the user before coding it
    (for example a = event type, b = goal, c = seconds, id 0, no description text id unless June's strings have one per type), then log it.
  - **0x493 (mission id)** -> acknowledge with 0x494 if the client's wait entry needs it (find its consumer first). No reward is paid: log one
    `[JUNE_QUEST] RM` line saying the reward was server-side.
  - **0x496 (n)** -> grant n EXP to the unit, by the emulator's normal EXP path. **0x497 (n)** -> apply buff n by the emulator's buff path. Each logs.
- **Data.** Adopt `DLG_UI_Random_Mission.lua` and `DLG_UI_Random_Mission_PopUp.lua` (June data034) with `luasrc.py adopt-june`. Stage
  `DLG_UI_Common_Texture83_NEW` / `Texture84_NEW` `.tga` + `.TET` (June data057) with `kom.py stage-june`. Check `DLG_Room_Button0.tga` is present.
  `GameEditCommandList`'s `Slash_rm*` functions must reach the console (check March's copy).
- **No March home.** Ain, Rose and Luciel have no March class. Note their 4 skill-use sites (and the Rose / Ain / Luciel gage slot 3) as
  out of scope, with a line in the Done note.
- **Read before porting** (RM0 left these open):
  - what sub-stage `+0x4` is (event 6);
  - the key branch in `ShortCutKeyProcess` (`324d2e20` @324d3e02);
  - the flag test on event 9 in the base gage (skips flag 1) and in Rose (counts only flag 1). Port each as June has it.
- The 13 per-class skill-use sites sit in class files that other ports also edit. Check `git status` and keep to your own hunks. Add's site
  (`X2GUAdd.cpp:2686`) has no `3235a710` port, so place the hook after `SetSkillCoolTimeLeft` as June does.

## Task

Do "Phase RM1" as above. Done gate as in the section: a mission set by the cheat (`Slash_rm`) counts its event, completes, and shows the UI.
