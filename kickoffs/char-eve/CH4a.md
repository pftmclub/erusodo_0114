# June character Eve (Stage 7) - Phase CH4a - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH4a` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2c2 review, and no CH3 phase running (same files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`EVE PASSIVE` / `EVE UI`) |
| Review | `kickoffs/char-eve/CH4a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)"
```
Then the Eve lines of the checklist rows named below (`june2017_re/char_eve/plan_checklist_rows.md` gives the line numbers), the `CH4a` rows of `ch0_*.tsv`, and Aisha's matching CH4 Done note as the worked example.

## Task

Scope: `InitPassiveSkillState` and Eve's passive ids (types 1 / 5 / 8), their objects and readers; the engine hooks they need (Aisha's CH4a-CH4c already ported the unit delegate lists and `JUNE_USER_PASSIVE_HOOKS`: reuse them). June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`. Mark rows `(Eve: CH4a)`. Done gate: each item's effect visible in game or in a named log line.

## Amended by CH0 (2026-10-26)

- **Functions: 7** (`awk -F'\t' '$6=="CH4a"' june2017_re/char_eve/ch0_class_map.tsv`; helper 3, override 4).
  June addresses: `32978370` `3297a440` `3297f290` `329b3b50` `329cd810` `329ce7b0` `329ce830`.
- **Skill ids: 24** (`awk -F'\t' 'NF==7 && $6=="CH4a"' june2017_re/char_eve/ch0_state_skill.tsv`): 106 RNSI_P_COMMON_HYPER_AURA, 25201 RNSI_A_COMMON_AURA_ADDATK_ACCEL, 25214 RNSI_A_COMMON_AURA_ADDATK_ACCEL_UPGRADE, 25400 RNSI_P_EEG_REFINED_STEP, 25401 RNSI_P_ECN_NASOD_WEAPON, 25402 RNSI_P_ECN_ENERGY_OF_CONCENTRATION, 25403 RNSI_P_ECN_QUEEN_OF_VENGEANCE, 25404 RNSI_P_ECN_QUEEN_OF_DESTROY, 25405 RNSI_P_EAT_QUEENS_POTENTIAL, 25406 RNSI_P_ECE_QUEENS_CONTROL, 25407 RNSI_P_ECE_ELECTRON_BOOSTER, 25408 RNSI_P_ECE_GRACE_OF_QUEEN, 25409 RNSI_P_ECE_CHARGING_BOOSTER, 25410 RNSI_P_EEL_SPECTRUM_DYNAMICS, 25411 RNSI_P_ECB_HYPER_OPTICAL_RESEARCH, 25412 RNSI_P_ECB_PHOTON_BOOSTER, 25413 RNSI_P_ECB_HIGHPOWERED_ELECTRICS, 25414 RNSI_P_ECB_ADVANCED_EL_ENERGY_REACTOR, 25415 RNSI_P_ENS_OPEN_MIND, 25416 RNSI_P_ENS_CHEAT_CODE_DESTROY, 25417 RNSI_P_EEP_OPEN_MIND, 25418 RNSI_P_EEP_CHEAT_CODE_RULE_BREAK, 25419 RNSI_P_EBS_OPEN_MIND, 25420 RNSI_P_EBS_CHEAT_CODE_ELECTRONIC.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7ae L690-722 (254 tree rows with no state).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `322de5d0` CX2Game::UpdateSkillSlotUI( bool ) (X2Game.cpp:13345) (confirmed); `320017d0` user (candidate); `3202ed60` user (candidate).
- `InitPassiveSkillState` `3297f290` also sets the June-only Nasod Core members `+0x9bf8` / `+0x9bfc` / `+0x9c00` that CH1q's AttackManeuverCore reads; keep them consistent with CH1q.
- 106 `RNSI_P_COMMON_HYPER_AURA`: no case; imm32 in class code at `329673c9`.

## Amended by CH1d (2026-10-26)

- `InitPassiveSkillState` `3297f290` writes `m_fJuneElectronBallBigRate` (+0x97bc, CH1d; ctor default 0) from skill 25407 (`0x633f`) ability `0x49` (@3297fd5a): the chance of the BIG fire ball in `June_ENSI_COMBO_X / XX_FrameMove`. The same skill's ability `0x1f` scales `m_fElectronBallRangeRate` (+0x97ac) and, protected, `m_fMPConsumeRateElectronBall` (+0x97b0) is a `KProtectedType<float>` in June (March: plain float).

## Amended by CH1k (2026-10-26)

- CH1k declared `m_fJuneVengeanceMpRate` (`+0x9bf8`, read by `June_DamageReact`: `UpNowMp( GetMaxMp x it x HP fraction lost )` while alive), `m_fJune9bfc` and `m_fJune9c00` (`+0x9bfc` / `+0x9c00`, ctor 0). `InitPassiveSkillState` `3297f290` writes them from skills `0x633b` / `0x633c`; use these names and drop any duplicate.
- `3297f290` also calls `329ce7b0` (a protected-bool setter); CH1k keeps the bool as `m_bJune89fc` (`+0x89fc`, write-only so far). Set it there the same way.

## Amended by CH1q (2026-10-27)

- `AttackManeuverCore` (`329ac4a0 @329aca06-329aca3d`) has one June branch CH1q left as a comment: `m_fJune9c00` ( `+0x9c00` ) x `m_iManeuverCoreLv` > 0 and `m_pUnit` -> `32894e40( f, false )` on the unit's `CX2UserSkillTree` ( `+0x494` ). `32894e40` (funcmap row, CH1q) is June-only: both bars, the force / ignore lists and type 12 / 13 filter of its sibling `32893e00` ( `ReduceLeftSkillCoolTimeByRateJune` ), left > 0 -> rate ? left x f : max( 0, left - f ), then it clears the reset lists. Port it whole as a `CX2UserSkillTree` member and make the call live in the same phase that fills `+0x9c00` ( the comment line sits after the Queen of Vengeance MP block, under `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE` ).
- `+0x9bfc` ( Queen of Vengeance MP per core level ) is already read there ( `UpNowMp` + `MP+ %d` text ); filling it is enough.

## Amended by CH2a (2026-10-27)

- `3297f290 @3297f73a` writes `m_fJune4070` (ability `SA_EFFECTIVE_TIME` of Grace of Queen 25408 `RNSI_P_ECE_GRACE_OF_QUEEN`, 0 when absent; `@3297f722` writes `+0x406c` from ability `0x33`). The member exists since CH2a (`CX2GUEve::m_fJune4070`, default 0) and is read by Dimension Link's FrameMove (`32989b60`: sentry time `+0x9c4c` x 20 / 60 + it) and by `3299b914` / `3299bb74` (Electronic Field, CH3b). Write it here.

