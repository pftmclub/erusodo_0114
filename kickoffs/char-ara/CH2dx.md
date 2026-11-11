# June character Ara (Stage 7) - Phase CH2dx - Fix: Storm and Restraints leave the unit stuck (CH2d play-test)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH2dx` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`), "CH2d - Done" |
| Depends on | CH2d (commit `24c5badf`). One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`) |
| Review | none of its own: the CH2d3 review covers it |

## Report (user, 2026-11-11, CH2d play-test on unit 135 `LSpecter99`)

Specter of Thunder works (find / landing, 4 casts in the log). **Specter of Storm and Restraints Specter: after the cast the character is stuck** (no further input; the user relogged).

## What the log already says (do not re-derive)

- The game equipped the **enhanced ids**, not the ones the save slotted: `[JUNE_CHAR] ARA SKILL equip skill 27036 slot 0 / 27041 slot 1 / 27042 slot 2` (`offline_server.log`, 22:12-22:14). So the test ran 27036 (Storm), 27041 (Restraints), 27042 (Thunder); the user cannot avoid it.
- **Storm (27036):** CH2d registered 27036's case with the main state and the landing state only. Its EventProcess (`ASI_SA_ALS_SPECTER_OF_STORM_UPGRADE_EventProcess`, June `32bc9c70`) changes to `ASI_SA_ALS_SPECTER_OF_STORM_FINISH` (and to `_DEGREE` / `_ULTIMATE_DEGREE`), none registered for 27036 (the base case registers `_FINISH`, the enhanced one does not: June registers the ultimate finish through helper `32bc9f50`, CH2d2's). That alone explains the stuck unit.
- **Restraints (27041):** the cast reached the damage ( `DamageEffect_RESTRAINTS_SPECTER_Damage` HIT at 22:13:54.588 ) and its effect set was deleted at 22:13:55.122, then no more lines until the relog at 22:14:12. 27041 shares 27022's states ( `ASI_SA_ALS_RESTRAINTS_SPECTER_CreateState` + finish ) in both switches, so the cause is not yet known. `ErrorLog.txt`: 0 lines.

## Task

1. **Find why Restraints sticks.** Read the CH2d code against June's listings ( `charport_adis.py ara 32bc4410 32bad5e0 32bbd840 32ba2460` ) and Ghidra: the finish state's registration ( `32bc9540` ), whether the finish state is reached and left ( the animation end test, `StateChangeOrFPCancel( ASI_SA_ALS_RESTRAINTS_SPECTER_FINISH )` and the FP-cancel path `m_iUsedSkillID == m_iChosenSkillID`, the `m_bFPCancel` / `m_vFPCancel` window ), the second-switch case ( `InitAquiredActiveSkillState`, state ids 302 / 303 set twice with a chain pointer ), `ASI_WAIT` 0xd vs March's `USI_WAIT`, and the finish FrameMove's `DestroyInstanceHandle`. A temporary diagnostic is a `CX2OfflineLog::Server` line behind `..._SKILL_DEBUG` ( state ids at StateChange ); say in the hand-off which lines to read.
2. **Make Storm 27036 safe.** Either port CH2d2's slice that the unit needs now ( the enhanced Storm's `_FINISH` registration `32bc9f50` and its three functions `32ba7c90` / `32ba61d0` / `32bc0e30` / `32ba61f0`, plus the `_DEGREE` / `_ULTIMATE_DEGREE` states if the key-hold path can reach them ), or, if that is too much, register the base finish as a labelled stand-in under the flag. Decide with the scope rule: the degree states are CH2d2's unless the unit is unplayable without them ( it is unplayable now ). Amend `CH2d2.md` for what moves.
3. Check Thunder 27042 and the three base ids ( 27021 / 27022 / 27023 ) still work; the hand-off slots them and says plainly that the game swaps to the enhanced ids.
- CP949: `bytepatch.py` only. Same gates as every phase ( `flagcheck`, `junediverge`, `enumscan`, a full build: it hands off a play-test ). `JUNE_BUGS.md` row for anything June does that looks wrong.
- Hand-off: unit 135 `LSpecter99`, each skill cast twice, what must happen after the cast ( back to wait, input works ), the log lines you will read.
