# June character Raven (Stage 7) - Phase CH1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1b review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1b done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` (defined; CH1b adds two functions and two members) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH1b - Done"
awk -F'\t' '$7=="CH1b"' june2017_re/char_raven/ch0_class_map.tsv
python june2017_re/charport_adis.py raven --phase CH1b --out <scratchpad dir>   # one listing per row
```

## Task

Review the 40 CH1b class-map rows (37 `inside` bodies written, 2 `new`, 1 already March's) against June's listings (the listing, not the decompile, carries the `StateChange` arguments):
- **The real differences from March:**
  - `32c28c10` / `32c28dd0` / `32c28f50` (DamageDownFront / Back, AirDownLanding EventProcess): the queued-X stand-up attack goes to `RSI_STANDUP_ATTACK_*_NO_DOWN` when `GetUnit()` is set and `GetSkillLevel( 0x5e88 = RNSI_A_RF_SHADOW_STEP 24200, false, 0, true ) > 0`, else to `RSI_STANDUP_ATTACK_*`; March rolls `GetRandomInt() < m_iNoDownDamageAtStandUpAttackPercent`. Confirm the skill id, the `GetSkillLevel` arguments (`push 1, 0, 0, 0x5e88`) and that Back keeps the double `0.66`.
  - `32c59550` = March's `RBM_EMERGENCY_ESCAPE` (CH0 had it as `new ?`): the cool-time gate (`+0x95ac` above `+0x95a0` returns), `GetSkillLevel( 0x5f5a = RNSI_P_RBM_EMERGENCY_ESCAPE 24410, true, 0, true )`, the roll `GetRandomInt( 0 ) < SA_EFFECTIVE_RATE x 100.0` (a double), the MP check and text, `StateChange( 0xde )` (= March's `RSI_SHADOW_STEP`, script name `RSI_SI_A_RF_SHADOW_STEP`), `UpNowMp( -cost )`, then `+0x95a0 = 0`. Confirm the two fields' roles against `32c42f90` (adds frame time while `+0x95a0 <= +0x95ac`) and `32c57660 @32c57c03` (both set from SA 0x94).
  - `32c290d0` / `32c29130` (`new`, `RSI_DAMAGE_STANDUP_FRONT_StateStart` / `_StateStartFuture`): `CommonStateStart` then, with `RNSI_P_RBM_OPEN_MIND` (0x5f61 = 24417) learned, `ResetAnimSpeed( playSpeed x GetLearnedSkillAbilityValue( SA_ANI_SPEED = 0x67 = 103, id ), true, bFuture )`. Confirm the argument order of `ResetAnimSpeed` (`push 0/1` = bFuture) and that ten states share them.
  - `32c52480` = March's `ClearArcEnemyData` (nothing written).
- **Everything else was written as "= March after the macros":** Die Front / Back (eight functions), the four StandUpRolling EventProcess + End, the four StandUpAttack (front, back, no-down) FrameMove / EventProcess / End, DamageDownFront / Back FrameMove, SmallBack / BigBack Start, `32c33bd0` (`EventTimerOneshot( 1.55f )`, `UpDownCrashCamera( 20, 0.3 )`). Spot-check 3 end to end (suggest `32c55ee0` Die Front Start, `32c29190` Rolling EventProcess, `32c28d90` DamageDownBack FrameMove: `EventTimer( 0.32f ) == true`, not Oneshot).
- Offsets: confirm the two rows CH1b added to `june2017_re/char_raven/ch1_offsets.tsv` (`+0x95a0`, `+0x95ac`).
- The new `SKILL_ID` names (`RNSI_A_RF_SHADOW_STEP` 24200, `RNSI_P_RBM_EMERGENCY_ESCAPE` 24410, `RNSI_P_RBM_OPEN_MIND` 24417) against June `Enum.src.lua` (14579 / 14917 / 16006).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE --range <CH1b commit>^..<CH1b commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `/Zs` probe with `/D SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`census.cl_cmd`). Fix clear transcription errors under the same flag (then `python tools/build.py --deploy`).
Write "CH1b - Review" into the plan doc, update the STATUS row, check `kickoffs/char-raven/CH1c.md`, commit with explicit pathspecs.
