# June character Aisha (Stage 5 pilot) - Phase CH1 - Deploy unit: June script, motions and the class core

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH0, CH0b (Aisha's assets are already June's when CH1 starts), CH1a-CH1i and their reviews (amended by CH0) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (+ `SERV_IRUHADEV_JUNE_CHAR_DEBUG` if the phase logs per frame) |
| Log tag | `[JUNE_CHAR]` (`python tools/logs.py --tag JUNE_CHAR`; lines `ARME DATA` / `ARME CORE`) |
| Review | `kickoffs/pilot-char-revamp/CH1-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Starting context" "Phase CH0 - Aisha map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
```
Then CH0's Done note and the `ch1` rows of `june2017_re/char_arme/ch0_class_map.tsv`, `ch0_state_skill.tsv`, `ch0_checklist_rows.tsv`,
`ch0_assets.tsv`. Read each checklist row CH0 assigned to CH1 by its heading (`docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`).

## Task

Do "Phase CH1". Her assets are June's since CH0b, and the script and the C++ go live together:
1. Build the code.
2. Adopt the script, pack and ask before each copy.
3. Hand the play-test over only once both the exe and the koms are deployed.

- `X2GUArme_VioletMage.*` and `X2GUUser.*` are CP949: edit them with `bytepatch.py` only.
- Check each `#ifdef` branch you edit with `cl /P` (`SetEquippedSkillFuncBySkillSlotIndex` carries `UPGRADE_SKILL_SYSTEM_2013` pairs).
- After the phase, append `(Aisha: CH1, <commit>)` to each checklist row it ported.
- The play-test hand-off names: an Aisha save and class, combos / dash / jump / a bounding hit, and the `ErrorLog.txt` lines to compare
  against a pre-CH1 run.

## Amended by CH0

Written by CH0 (2026-10-20). **CH1 now depends on CH1a-CH1i and their reviews**: those Sonnet phases transcribe the ~340 class functions as
inert `June_*` members; CH1 (Opus) wires them, adopts the script and hands over the play-test.

Filters: `awk -F'\t' '$6=="CH1"' ch0_class_map.tsv` (29 functions: the registrars and what only they call), `awk -F'\t' 'NF==8 && $8 ~ /^CH1/'
ch0_state_skill.tsv` (the 204 states CH1a-CH1i serve), `awk -F'\t' '$5 ~ /CH1( |$)/' ch0_checklist_rows.tsv`.

What CH1 itself does:
1. **Registration.** Port June's `InitState` `32cea0c0` whole (vslot 0x7a4): cut-in set index `+0x22b2` / `+0x22b3`, `InitStateCommon`
   `32ce77b0`, `3202b110` (base event states), the START / WIN / LOSE roll `31fe22d0`, `AVSI_WAIT_STATE_CHANGE` (id 0x82), the per-job blocks
   (switch on unit class 2 / 14 / 15 / 23 / 104 / 105 / 113), `31fd8f00`, `32ce1ba0` (AVP Angor hammer mesh). Every callback points at the
   `June_*` member; March's registration stays in the `#else`. March-only states that June drops (6: `AVSI_DARK_MAGICIAN_DASH_COMBO_Z3up`,
   `_SA_ADM_DEATHFIELD_HYPER`, `_SI1_AV_BINDING_CIRCLE_HYPER`, `_SI1_AV_CHAIN_BURST_HYPER`, `_SI3_AV_GUST_STORM_HYPER`,
   `_SI_A_AV_CIRCLE_FLAME_MEMO`) are not registered. `AVSI_RIDING_DAMAGE_BACK` is registered by June with no script table: keep it, plus a
   `JUNE_BUGS.md` row. 23 script tables no June C++ names (Ice Storm, Icicle Wave, Aqua Shower, Vital Drain, ...) stay dead data.
2. **`SetEquippedSkillFuncBySkillSlotIndex` head** (`32cede00`): bounds, the two slot tables (`3351b05c` / `3351b184`), drop `31fd2af0` (7i),
   `+0x11 = 1`, the transcendence-memo branch (`31fdbcc0`, only `0x56c9`; inert until Stage 6), the type switch (case 5 COMMON_BUFF, 0xc
   THROW_ITEM for id 900, the hyper-active inner switch, `default:` Power Attack). Skill cases stay March's until CH2a-CH3b.
3. **Virtuals.** Each override of CH1h / CH1i becomes the virtual's body under the flag (March body in `#else`); March-only virtuals June
   lacks (`ParseCommonRandomState`, `ApplyAuraAcceleratorBuffToGameUnit`) are dropped under the flag.
4. **Pressed-index space (7ag).** Convert the class decode (`:5232-5257`, `:5072`, consumer `:16610`) to June's A 0-4 / B 5-9, including
   June's 4 (F) / 9 (T) presses: that is the slot-5 gap at `X2GUUser.cpp:25500-25510`, inert until Stage 6 unlocks the TC slot.
   `CommonSpecialAttackEventProcess` is shared by every class: decide how Aisha alone gets June's space (an Opus design call; write it down).
5. **Hooks:** `JuneInitWaitStateChange`, `JuneInitBoundingStates( "AVSI_" )` (7l), and the 7 June-only script keys of `ch0_keys.tsv`
   (`FORCE_DYNAMIC_CAMERA_INDEX`, `RETAIN_CAMERA_STATE`, `EFFECT_SET_LIST_ULTIMATE_EX`, `IMMUNITY_EDT_LIST_AT_THIS_STATE`, `SPECIAL_OPTION_VALUE`,
   `BBT_CHANGE_NOW_HP_PER_SECOND`, `BFT_TIME`): confirm each against March's preprocessed reader, port the missing reader or route it.
6. **Script:** `luasrc.py adopt-june data036 Arme_VioletMage.lua` (June bytecode); then `charport_scan.py` and `charport_keys.py
   Arme_VioletMage` show nothing new.

## From the CH1a review (2026-10-20)

- Wiring CH1a's `June_WaitEventProcess` / `June_ChargeMPEventProcess` is safe only after CH1i retires `m_iJune9770` (June's rule: below 3 MemoRise
  entries the wait state enters the charge, and stays in it, on a full MP bar). The constructor must set `m_hJuneMemoRiseLoop` to
  `INVALID_EFFECTSET_HANDLE` (`32cc99d0 @32ccb01c`).
- `June_GameByteC20()` (CX2Game `+0xc20`) is June's value (false), not a pending stand-in.
