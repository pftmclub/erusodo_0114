# June character Eve (Stage 7) - Phase CH1 - Final: LIVE, the script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1q review; CH0b done (its koms copied, or held for this phase if CH0 found a dropped animation March plays) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (defined here), `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | `[JUNE_CHAR]` (`EVE DATA`, `EVE CORE`, `EVE NCORE`) |
| Review | `kickoffs/char-eve/CH1-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
```
Then the CH1a-CH1q Done notes and reviews.

## Task

1. Define `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` in `Always.h`; `flagcheck.py JUNE_CHAR_EVE_LIVE`.
2. Adopt `Eve_Nasod.lua` (`luasrc.py adopt-june data036 Eve_Nasod.lua`, `--dry-run` first) and the skill NPC / summon scripts CH0 marked CH1 (planning list: `Atomic_Shield_Eve`, `Guardian_Eve`, `Guardian_Sp1_Eve`, `Guardian_Sp2_Eve`, `Nasod_Watch_Eve`, `Nasod_Element_Fire_Eve`, `Wally9th_Eve`, `SI_HA_FERDINAND`, `SI_SA_EEP_ELECTRONIC_FIELD`, and their `_Memo` files). A `[JUNE_CHAR] EVE DATA` line logs the script load. Re-run `charport_scan.py` and `charport_keys.py Eve_Nasod`: nothing new.
3. Build, pack, ask before each copy (with CH0b's held koms if any), then hand over the play-test: an Eve save per job tier; combos, dash, jump, riding, a bounding hit, damage, awakening with the Nasod Core; the `ErrorLog.txt` lines to compare against a pre-CH1 run.
- Check that March's `CommonStateStart` accepts June's empty `ANIM_NAME` on the wait-state-change table (7h).
- Mark the ported checklist rows `(Eve: CH1)`.

## Amended by CH0 (2026-10-26)

- **Skill ids: 1** (`awk -F'\t' 'NF==7 && $6=="CH1"' june2017_re/char_eve/ch0_state_skill.tsv`): 900 SI_ETC_WS_COMMON_LOVE.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §4 L155-178 (missing enum tables); §7h L407 (effect-set list variants); §7o L484-518 (SET_LIMIT_MAX_G_SPEED -4000); §7p L531 (TIME_STOP_IMMUNE_STATE); §7t L575 (SHOW_HYPER_CUT_IN_TIME); §7t L600 (sound sets SOUND_SET3_RATE+); §7x L649-657 (AT_COMMAND tags).
- **Lua to adopt:** `Eve_Nasod.lua` only. Her 14 skill NPC / summon scripts are June's `ljconv` bytecode already (`ch0_assets.tsv` owner `char-eve/done-elsewhere`): check them, do not re-adopt.
- **CH0b holds no koms** (the motion question in CH0b's amendment); copy the 11 base motions together if CH0b has not.
- The script's 3 numerical-only tables belong to Electronic Move (CH3c).

## Amended by CH1k (2026-10-26)

- Wire list for the lifecycle group ( June vslot -> member; nothing is registered yet ): `0x3c` `June_InitComponent`, `0x44` `June_ReInit`, `0x48` `June_InitEffect`, `0x4c` `June_DeleteMinorParticle`, `0x60` `June_DamageReact`, `0x90` / `0x94` / `0x98` `June_Win` / `June_Lose` / `June_Draw`, `0xc4` `June_HyperModeBuffEffectStart`, `0x27c` `June_AddUnitSlashData`, `0x314` `June_ApplyRenderParamOnEmptyRenderParamByBuff`, `0x3a8` `June_ApplyRenderParam`, `0x3ac` `June_ModifyFinalDamageByGameUnitType` ( `( DamageData*, float&, float& )`, not March's four-reference form ), `0x4b0` `June_GetComboZStateID`, `0x4dc` `June_AttackResultPlayEffect`, `0x4e0` `June_AttackResultByType`, `0x540` / `0x548` `June_CreateMajorParticleArray` / `June_CreateMinorParticleArray`, `0x54c` `June_InitEnchantWeaponEffectUnitSpecific`, `0x56c` `June_Vslot56c`, `0x584` `June_Vslot584`, `0x5f0` `June_COMMON_BUFF_FrameMove`, `0x5fc` `June_DashStartFuture`; `June_ENSI_RIDING_HYPER_MODE_FrameMove` ( `329ac330` ) for `ENSI_RIDING_HYPER_MODE`. The constructor body is `June_ConstructorInit()` ( call it at the end of the March constructor ) and the destructor body `June_Destructor()` ( first line of `~CX2GUEve` ).
