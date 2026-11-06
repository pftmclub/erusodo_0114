# June character Raven (Stage 7) - Phase CH2a - Skills: Raven (4)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH2a` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1 review (and any Opus `CH2a0` helper phase CH0 wrote). One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-raven/CH2a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Raven and the first jobs (classes 4, 16, 17, 25)" "Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH2a - Done" "CH2a - Review"
```
Then CH0's and CH1's Done notes, and the `CH2a` rows of `june2017_re/char_raven/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the skill ids class 4 has. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables); its effect closure (a name found only now is staged byte-exact and
added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Raven: CH2a)`.
- Hand-off: the unit and class, each skill id and its A / B slot (never slot 4 / 9 before transcendence; learn only the bank under test), each
  cast twice (cool time, MP), what June's motion and effect look like.

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

- **Functions: 43** (`awk -F'\t' '$7=="CH2a"' june2017_re/char_raven/ch0_class_map.tsv`; inside 26, new 17).
  June addresses: `32c260a0` `32c26130` `32c261c0` `32c26280` `32c26690` `32c266b0`(new RSI_SHADOW_STEP_Z_Init) `32c27320`(new RSI_SHADOW_BACK_SLIDE_END_UPGRADE_FrameMoveFuture) `32c27350`(new RSI_SHADOW_STEP_X_UPGRADE_Init) `32c279a0` `32c27a20` `32c27ba0`(new RSI_SHADOW_STEP_Z_UPGRADE_Init) `32c2dac0` `32c2db00` `32c2db90` `32c2dbd0`(new RSI_SI2_POWER_ASSAULT_StateEnd) `32c2dc40` `32c2dc90` `32c2dd10` `32c30db0` `32c30e80` `32c30ee0`(new RSI_SHADOW_STEP_Z_EventProcess) `32c33510` `32c378f0`(new RSI_SHADOW_BACK_SLIDE_UPGRADE_StateStartFuture) `32c379b0`(new RSI_SHADOW_BACK_SLIDE_UPGRADE_FrameMove) `32c37a10`(new RSI_SHADOW_BACK_SLIDE_END_UPGRADE_EventProcess) `32c37b80`(new RSI_SHADOW_STEP_X_UPGRADE_EventProcess) `32c39880` `32c398b0` `32c39ab0` `32c39b20` `32c3a690`(new RSI_SHADOW_STEP_Z_UPGRADE_EventProcess) `32c3e7a0` `32c42a30`(new RSI_SHADOW_BACK_SLIDE_UPGRADE_StateStart) `32c49590` `32c4dcc0`(new RSI_SHADOW_STEP_X_UPGRADE_FrameMove) `32c4de60`(new ?) `32c50d50` `32c50e00`(new RSI_SHADOW_BACK_SLIDE_UPGRADE_StateEnd) `32c50ef0`(new ?) `32c52550` `32c54190`(new RSI_SHADOW_BACK_SLIDE_UPGRADE_EventProcess) `32c5d330` `32c5d3f0`.
- **States: 13** (`awk -F'\t' '$8=="CH2a"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_SI1_MAXIMUM_CANNON`, `RSI_SI_A_RF_SHADOW_STEP`, `RSI_SHADOW_BACK_SLIDE_UPGRADE`, `RSI_SHADOW_BACK_SLIDE_END_UPGRADE`, `RSI_SHADOW_STEP_Z`, `RSI_SHADOW_STEP_Z_UPGRADE`, `RSI_SHADOW_STEP_X_UPGRADE`, `RSI_SI2_CANNON_BLADE`, `RSI_SI2_POWER_ASSAULT`, `RSI_A_RWT_FLAME_SWORD`, `RSI_A_RWT_FLAME_SWORD_GROUND`, `RSI_SI3_SEVEN_BURST`, `RSI_A_RWT_FLAME_SWORD_FLY`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 24000 RNSI_SA_RF_MAXIMUM_CANNON, 24001 RNSI_SA_RF_CANNON_BLADE, 24002 RNSI_SA_RF_POWER_ASSAULT, 24003 RNSI_SA_RF_SEVEN_BURST, 24025 RNSI_SA_RF_MAXIMUM_CANNON_UPGRADE, 24026 RNSI_SA_RF_CANNON_BLADE_UPGRADE, 24027 RNSI_SA_RF_POWER_ASSAULT_UPGRADE, 24028 RNSI_SA_RF_SEVEN_BURST_UPGRADE, 24200 RNSI_A_RF_SHADOW_STEP, 24201 RNSI_A_RF_FLAME_SWORD, 24217 RNSI_A_RF_SHADOW_STEP_UPGRADE, 24218 RNSI_A_RF_FLAME_SWORD_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L50 numerical-only DAMAGE_DATA table in state RSI_SHADOW_STEP_Z (script L4; §1 L50 numerical-only DAMAGE_DATA table in state RSI_SHADOW_STEP_Z_UPGRADE (s; §1 L50 numerical-only DAMAGE_DATA table in state RSI_A_RWT_FLAME_SWORD_GROUND; §1 L50 numerical-only DAMAGE_DATA + numerical override live (the 25 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7 L256 class code scales the cached state power rate (+0x7cc8): March Raven A; §7 L256 March RSI_SHADOW_STEP_THRUST_Start X2GURaven.cpp:14727 / :14729 assign; §7 L256 March RVC_SI_P_SHADOW_BUSTER_Start X2GURaven.cpp:20903 / :20905 assign; §7 L256 (d) June class functions that DO write the cache: 32c4b9e0 and 32c5d3f0; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7u L636 March-only effect templets the re-adoption dropped: RAVEN_SEVEN_BURST_; §7ae L714-722 an id of a listed type with no case plays the class's first state (Rav; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; §7ai L844 pass June's skip / adjust / level / creator at every ported BFI call.
- The 5 `RSI_SHADOW_*` states June registers in InitPassiveSkillState are here (their registration lines are CH4a's function). §7 L256: none of March's three power-rate assigners survives; June's `RSI_A_RWT_FLAME_SWORD_FLY` Start `32c5d3f0` does `+0x7cc8 *= 1.2` (skill 24201).
- **Amended by CH1k (2026-11-06):** the constructor `32c3ece0` stores the Shadow Step members' defaults: `+0x9654` (protected float) 0.0, `+0x9660` word 0, `+0x9664` float 1.0, `+0x972c` / `+0x9730` dwords 0 (June has two dwords where March has `m_iShadowBackSlideLevel`, `m_fShadowBusterPowerRate` (0.5f), `m_fShadowBackSlideTimeLeft`, `m_fShadowStepFinishTimeLeft`). `ch1_offsets.tsv` lists them; the phase that reads each decides the March member and sets its default under `_LIVE` (a June read of 0 where March initialises 0.5 or 1.0 changes the numbers). The lookup `32c58c30` lets a Shadow Step chain through with no MP cost and no slot refresh (March charged `FlushMp( 5.f )`).

## Amended by CH1n (2026-11-06)

- `CX2GURaven::SetMaximumCannonSkillFunc` (`_CORE`, `32c641a0` @32c66cd6) is where Maximum Cannon registers: both June switches' default (24000 and 24025
  have no case). It holds March's case until here. Write June's registration inside it under `_SKILL`, March's in `#else`: `RSI_SI1_MAXIMUM_CANNON` on
  both slots, Init `32c68a50` -> `32c260a0`, EventProcess `32c69700` -> `32c2dac0`; the hyper slot gets the same table and the EventProcess only (no
  FrameMove, no `_HYPER` table). The `_LIVE` id-switch `default:` already calls it.

## Amended by CH1p (2026-11-06)

- **Shadow Step's held key is June-only** (March reads no key there). `32c30db0` (`RBM_EMERGENCY_ESCAPE_StartFuture`) ends with `m_kSkillSlotKey.Set( m_iNowSpecialAttack - 1 )`
  `@32c30e73`; `32c52550` (`RSI_SI_A_RF_SHADOW_STEP_EventProcess`) starts `m_kSkillSlotKey.FollowSlotSwap()` `@32c5255b` and gates the back slide on
  `m_bEnableShadowBackSlide` + `m_kSkillSlotKey.IsKeyPressedOnce( m_InputData )` (`31fc8900`) `@32c525f4`. Port both calls with the whole bodies.
