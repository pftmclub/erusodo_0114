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

Filters: `awk -F'\t' '$6=="CH1"' ch0_class_map.tsv` (28 functions after CH0c: the registrars, what only they call, and 32cb7110), `awk -F'\t' 'NF==8 && $8 ~ /^CH1/'
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

## From CH0c (2026-10-20)

CH0c corrected the registration map; wire every state from `ch0_state_skill.tsv` (june_registration, june_callbacks), never from the CH1a / CH1b
Done notes, whose "Registration notes for CH1" were written from the old map. What changed for CH1 itself:
- **Every registration row now matches the listing** (172 rows: 163 read from the name push along the branch path, all 33 per-class `~` rows among them; 9 from the call back: 4 std::string names and the 5 of `3202b110`).
  A state gets exactly the slots its row lists; no state shares a callback it was not registered with.
- **USI_HYPER_MODE** (`32ce77b0` id 0x17, `@32ce8674`): its name is the std::string `31fdd490( job )` returns, so the four hyper-mode rows
  now carry Init `32cb7110` (state-fn, CH1: yours), StartFuture `329caf10`, Start `329caef0`, FrameMove `32bd5f20`, CameraMove `32cd1470`
  (`June_HyperModeCameraMove`, ported in CH1b), EventProcess `329caf00`, End `329cc5a0`.
- **USI_START / USI_WIN / USI_LOSE** (`32cea0c0` ids 9 / 0xb / 0xc): the rolled names of `31fe22d0`; callbacks are CX2GUUser's (funcmap
  `32cea0c0 @32cea2f5`), now on the rolled `_START` / `_WIN` / `_LOSE` rows.
- Ported functions whose state set shrank keep their phase and member: `32cc8800` (Init of the two TELEPORT_STANDUP_ATTACK states, CH1a),
  `32cbb8d0` / `32cbba10` (COMBO_X4_CHARGE only), `32cb7530` / `32cc6ca0` / `32cd5360` (DASH_JUMP_ATTACK_Z only), the riding hyper set
  `32cb8a50` / `32ce6f20` / `32ce1590` / `32cf0fa0` (AVSI_RIDING_HYPER_MODE only), `32cd1470` (CH1b). Point only those states at them.

## From CH1c (2026-10-20)

- **Constructor / destructor:** `m_pJuneMpRefund` (June `+0x96ac`, `JuneMpRefundData*`) = NULL and `m_iJuneBallElement` (June `+0x9630`) = 0 in the constructor (June `32cc99d0 @32cca662`, `@32ccb794`); the destructor deletes `m_pJuneMpRefund` (June `32ce4647`, inside the destructor that follows `32ce4180`).
  CH4 fills both (the passive object, the element) and replaces `JuneMpRefundData` with the real layout.
- **Shared state ids:** the job variants share one id (0xaa = `AVSI_COMBO_X3front` / `AVSI_AEM_XXfrontX`, 0xab = `AVSI_COMBO_X3down` / the High and Dark Magician `X3down`, 0xa8 = X2 / the High and Dark Magician X2 ...). CH1c's `StateChange` calls name the base state; the numbering decision is yours.
- `AVSI_AEM_ZZX` (233) exists as an enum member; its state functions are CH1e's. The CH1c functions' registered states are in `ch0_state_skill.tsv` (`phase` CH1c).

## From CH1e (2026-10-20)

- **Constructor:** `m_fJuneZZXXLoopTime` (June `+0x9784`) = 0 (`32ccb02e`), `m_hJuneDashJumpChargeEffect` (`+0x9788`) = `INVALID_EFFECTSET_HANDLE` (`32ccb034`); the destructor does not touch them.
- **`SPECIAL_OPTION_VALUE` reader (item 5):** June `CommonStateStart` `32057b80` clears the user float list at `+0x93a8` (`31f8b7e0( 0 )` @320608a9-320608c1), then, if the state has the key (`@320608c8`), pushes its values from index 1 on (`31fac820` per value).
  CH1e's `June_AVSI_AEM_XZZ_FrameMove` reads it through `June_GetSpecialOptionValue( 0..5 )` over the stand-in `m_vecJuneSpecialOptionValue` (Arme class); CH1 parses the key (CX2GUUser, `CommonStateStart`) and points the getter at the user list.
- **Enum numbering:** six June-only ids were added after `AVSI_AEM_DASH_JUMP_X3`: `AVSI_AHM_DASH_JUMP_ZXX_START / _CHARGE / _FINISH` (229 / 230 / 231), `AVSI_AEM_ZZXX_START / _LOOP / _END` (234 / 235 / 236). CH1 decides the final numbers like every other June-only id.
- The CH1e states' registered callbacks are in `ch0_state_skill.tsv` (`phase` CH1e); the three `EventProcess` of the `ZXX` / `ZZXX` families chain several `StateChange` calls in one frame on purpose (June structure).
