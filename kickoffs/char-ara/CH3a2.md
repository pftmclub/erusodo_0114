# June character Ara (Stage 7) - Phase CH3a2 - Skills: Sakra Devanam (118) skills with smallest id >= 27012 (CH0 split of CH3a)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH3a2` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH3a. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH3d review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH3a - Done"
```

## Task

Scope: the skill ids class 118 has and 7 / 28 lack (a skill's line decides its phase). **CH0 split: this phase is the skills whose smallest id is >= 27012 (CH3a has the smallest id < 27012 and the states with no skill).** For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
whole (Ghidra, the map's addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (March's in `#else`) or a new case; its attached rows
(7u names, 7ai BFI arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`, `SPEED_TIME` rows
l.522-526); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`); its force-power gain / cost lines on CH1q's
code. Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that
reads the id. A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Ara: <phase>)`.
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
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0 (2026-11-10)

- **Functions: 35** (`awk -F'\t' '$7=="CH3a2"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 24, new 11).
  June addresses: `32b92460` `32b92480`(new ASI_A_ALH_BREAKING_ATTACK_StateStart) `32b924e0` `32b92510` `32b92530`(new ASI_A_ALH_BREAKING_AVOIDANCE_StateStart) `32b928e0` `32b929c0` `32b929e0` `32b92b20` `32b92b70` `32b92b90` `32b92ba0` `32b952a0`(new ASI_SA_ASD_LINK_FLOWER_SLASH_1st_StateEndFuture) `32b952c0`(new ASI_SA_ASD_ADVENT_DRAGON_Init) `32b9bd00` `32b9bde0` `32b9c3b0` `32b9c510` `32ba3ea0`(new ASI_SA_ASD_LINK_FLOWER_SLASH_1st_EventProcess) `32ba40a0`(new ASI_SA_ASD_LINK_FLOWER_SLASH_2nd_EventProcess) `32ba42a0`(new ASI_SA_ASD_LINK_FLOWER_SLASH_LAST_EventProcess) `32ba7ad0` `32bab370`(new ASI_SA_ASD_LINK_FLOWER_SLASH_1st_StateStartFuture) `32bab3c0`(new ASI_SA_ASD_LINK_FLOWER_SLASH_LAST_StateStartFuture) `32bac5e0` `32bac760` `32bad910`(new ASI_SA_ASD_CONTINUITY_STRIKE_THROWING_START_EventProcess) `32badac0`(new ASI_A_ASD_SWALLOW_ASSAULT_TEETH_START_EventProcess) `32bb03d0` `32bb84e0` `32bb8680` `32bba8f0` `32bbaab0` `32bc9230` `32bc92a0`.
- **States: 15** (`awk -F'\t' '$8=="CH3a2"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_A_ALH_BREAKING`, `ASI_A_ALH_BREAKING_ATTACK`, `ASI_A_ALH_BREAKING_AVOIDANCE`, `ASI_SA_ALH_PEERLESSNESS_SLASH_START`, `ASI_SA_ALH_PEERLESSNESS_SLASH_END`, `ASI_A_ASD_LOW_BRANDISH_SLICE`, `ASI_A_ASD_LOW_BRANDISH_KICK`, `ASI_A_ASD_SWALLOW_ASSAULT_TEETH_START`, `ASI_A_ASD_SWALLOW_ASSAULT_TEETH_END`, `ASI_SA_ASD_LINK_FLOWER_SLASH_1st`, `ASI_SA_ASD_LINK_FLOWER_SLASH_2nd`, `ASI_SA_ASD_LINK_FLOWER_SLASH_LAST`, `ASI_SA_ASD_CONTINUITY_STRIKE_THROWING_START`, `ASI_SA_ASD_CONTINUITY_STRIKE_THROWING_END`, `ASI_SA_ASD_ADVENT_DRAGON`.
- **Skills: 10** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27012 RNSI_SA_ASD_PEERLESSNESS_SLASH, 27043 RNSI_SA_ASD_LINK_FLOWER_SLASH, 27044 RNSI_SA_ASD_CONTINUITY_STRIKE_THROWING, 27053 RNSI_SA_ASD_PEERLESSNESS_SLASH_UPGRADE, 27064 RNSI_SA_ASD_ADVENT_DRAGON, 27206 RNSI_A_ASD_LOW_BRANDISH, 27207 RNSI_A_ASD_BREAKING, 27226 RNSI_A_ASD_SWALLOW_ASSAULT_TEETH, 27232 RNSI_A_ASD_LOW_BRANDISH_UPGRADE, 27233 RNSI_A_ASD_BREAKING_UPGRADE.
- **Checklist rows: 8** (`awk -F'\t' '$5 ~ /(^| )CH3a2( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L55 numerical-only DAMAGE_DATA table in state ASI_A_ALH_BREAKING; L55 numerical-only DAMAGE_DATA table in state ASI_A_ASD_LOW_BRAN; L55 numerical-only DAMAGE_DATA table in state ASI_A_ASD_LOW_BRAN; L55 numerical-only DAMAGE_DATA + numerical override live (the 14; L116 APPLIED_SKILL_ID (parsed since 8c); L117 NUMERICAL_DATA_INDEX; L178 June-only members of tables March has (SKILL_ID names, ARA_S; L852-L855 "pass June's skip / adjust / level / creator at every ported.
- **Identified callees first needed here: 2** (`awk -F'\t' '$8 ~ /(^| )CH3a2( |$)/' june2017_re/char_ara/ch0_unmapped_identified.tsv`, col 8 = earliest caller phase; none needs Opus): `320726a0` `322720d0`.
- Split rule (key = the smallest skill id of the state's skills): CH2c SA / CH2c2 A (>= 27200); CH2d ids < 27024 and states with no skill / CH2d2 27024-27199 / CH2d3 A; CH3c SA / CH3c2 A. CH2a (48, an SA / A split would leave 6), CH2b (45), CH3b (44), CH2d (49) stay whole. The CH2 stage review moves to CH2d3; the CH3 stage review stays at CH3d.
- 7w `+0x8e64` writers in CH2d (`32ba1d2d` / `32ba1ddb`) and CH2d2 (`32bbe885` / `32bbec3f`): the same labelled stand-in as CH3c's Fox Phantom. Reserved-buff path (`BUFF_FACTOR_ID_AND_APPLY_TIME`, checklist l.868): first Ara use in CH2d / CH3b / CH3c.

## Amended by CH1n (2026-11-11)

- **Second skill switch.** Each id of this phase that has a case in `InitAquiredActiveSkillState` (`32bd34c0`, `awk -F'	' '$1=="32bd34c0"' june2017_re/char_ara/ch0_skill_listing_rows.tsv`)
  gets June's case body too, inside March's case under `_SKILL`. June's chain lookup there takes the upgrade-swapped id: CH1n's head declares
  `eUpgradeSwappedSkillID` (under `_LIVE`) for it; March's cases still pass `iSkillID_`.
- `GetSkillChainInfoPtrBySkillID` is June's under `_LIVE` (CH1n, `32d3ed30`: it also matches the chain skill's enhanced id).

## Amended by CH3a (2026-11-12)

- **Void Break play-test lands here.** CH3a ported `ASI_SA_ASD_VOID_BREAK` (state 249) but cannot reach it: it is the secret skill of the Turbulent Wave chain
  (`Ara_Martial_Artist` `CHAIN_SYSTEM` chain 1: Rock Crash 27202, Tiger Claw, Low Brandish 27206, Turbulent Wave 27010; trigger = the loop state past 0.45 s). Low Brandish
  is this phase's, so the hand-off chains all four on unit 98 (118) and reads `[JUNE_CHAR] ARA SKILL state 249` plus `DAMAGE_EFFECT_VOID_BREAK_BUFF` / `EffectSet_ASD_Void_Break`.
- `_SKILL_DEBUG` is defined (CH3a turned it back on); the CH3d review retires it. Swallow Assault, Turbulent Wave and Void Break are done: do not re-port them.
- Unit 98 `SakraDevanamT` (118, lv 99): CH3a left bank A slot 0 Turbulent Wave Enhanced 27051, slot 1 Swallow Assault Enhanced 27052, slot 2 Pulling Pierce Enhanced 27030, slot 3 Falling Dragon Enhanced 27031.
