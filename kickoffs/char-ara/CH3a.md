# June character Ara (Stage 7) - Phase CH3a - Skills: Sakra Devanam (118), smallest id < 27012

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH3a` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2d3 review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`: the CH2d3 review retired it, so CH3a's first edit turns it back on, uncommenting its `//#define` in `KTDXLIB/Always.h` with `bytepatch.py`; amended 2026-11-12) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH3d review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH3a - Done"
```

## Task

Scope: the skill ids class 118 has and 7 / 28 lack (a skill's line decides its phase). **CH0 split: this phase is the skills whose smallest id is < 27012 and the states with no skill (CH3a2 has the rest).** For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
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

- **Functions: 34** (`awk -F'\t' '$7=="CH3a"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 33, new 1).
  June addresses: `32b91ff0` `32b92070` `32b92080` `32b921a0` `32b921b0` `32b921d0` `32b92620` `32b926d0` `32b926f0` `32b92be0` `32b9b3c0` `32b9b440` `32b9b490` `32b9b4e0` `32b9b6f0` `32b9c0c0` `32b9c0f0` `32b9c610` `32bac550` `32bae7a0` `32bb6950` `32bb7010` `32bb8850` `32bb9040` `32bb90a0` `32bb94c0` `32bb9510` `32bc1f30` `32bc9000` `32bc9070` `32bc90c0` `32bd2ec0` `32bd3470` `32bd6a20`(new StateChangeSwallowAssaultEscape).
- **States: 8** (`awk -F'\t' '$8=="CH3a"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_SA_ASD_TURBULENT_WAVE_START`, `ASI_SA_ASD_TURBULENT_WAVE_LOOP`, `ASI_SA_ASD_TURBULENT_WAVE_END`, `ASI_SA_ALH_SWALLOW_ASSAULT_READY`, `ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK`, `ASI_SA_ALH_SWALLOW_ASSAULT_FINISH`, `ASI_SA_ALH_SWALLOW_ASSAULT_ESCAPE`, `ASI_SA_ASD_VOID_BREAK`.
- **Skills: 4** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27010 RNSI_SA_ASD_TURBULENT_WAVE, 27011 RNSI_SA_ASD_SWALLOW_ASSAULT, 27051 RNSI_SA_ASD_TURBULENT_WAVE_UPGRADE, 27052 RNSI_SA_ASD_SWALLOW_ASSAULT_UPGRADE.
- **Checklist rows: 10** (`awk -F'\t' '$5 ~ /(^| )CH3a( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L55 numerical-only DAMAGE_DATA table in state ASI_SA_ASD_TURBULE; L55 numerical-only DAMAGE_DATA table in state ASI_SA_ASD_TURBULE; L55 numerical-only DAMAGE_DATA + numerical override live (the 14; L116 APPLIED_SKILL_ID (parsed since 8c); L117 NUMERICAL_DATA_INDEX; L121 "BUFF_FACTOR_ID_ULTIMATE x3 in Ara_Martial_Artist (""to-impl; L178 June-only members of tables March has (SKILL_ID names, ARA_S; L713-L745 an id of a listed type with no case plays the class's first ; L726 types 14-16 / 17 / 18 named in the class switch; L775-L777 ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK EventProcess (@32bc204c, M.
- **Force-power rows: 7** (`awk -F'\t' '$7 ~ /(^| )CH3a( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: differs 5, unchecked 2).
- Swallow Assault (`ASI_SA_ALH_SWALLOW_ASSAULT_*`, 27011; `ATTACK` EventProcess `@32bc204c`, `_FINISH` StateStart `@32bb70ff`) is here and runs on CH2b0's `ProcessLockOn`, passing June's `NULL, &lineIndex` (checklist l.775-776). Split: CH3a = skills with the smallest id < 27012 (and states with no skill), CH3a2 = the rest. `BUFF_FACTOR_ID_ULTIMATE` (l.121): re-check the reader gate.

## Amended by CH1n (2026-11-11)

- **Second skill switch.** Each id of this phase that has a case in `InitAquiredActiveSkillState` (`32bd34c0`, `awk -F'	' '$1=="32bd34c0"' june2017_re/char_ara/ch0_skill_listing_rows.tsv`)
  gets June's case body too, inside March's case under `_SKILL`. June's chain lookup there takes the upgrade-swapped id: CH1n's head declares
  `eUpgradeSwappedSkillID` (under `_LIVE`) for it; March's cases still pass `iSkillID_`.
- `GetSkillChainInfoPtrBySkillID` is June's under `_LIVE` (CH1n, `32d3ed30`: it also matches the chain skill's enhanced id).

## Amended by CH1q (2026-11-11)

- `m_hKillEnemyEffectSet` (`+0x9718`, CH1j) is misnamed: June's berserk kill reward `32bc5bc0` keeps no handle; the only writer is @`32bb6f8c` in `ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK` FrameMove `32bb6950` (reader @`32bb6eb2`), and `StopKillEnemyEffectSet` `32bc5ea0` skips the stop while that state runs. Rename the member and its stop for what Swallow Assault plays (bytepatch, both `.h` / `.cpp` sites) and write it in that FrameMove.

## Amended by the CH1 review (2026-11-11)

The CH1 play-test did not chain a skill, so the `SERV_IRUHADEV_JUNE_CHAR_CHAIN_ANIM_SPEED` check moves here: on unit 98 (118, Fullmoon Slash 27402 learned)
the hand-off has the user chain Pulling Pierce and cast it alone; the chained cast plays faster (June `320506f0` @32051bef, `m_fNowSkillPassiveRate`).

## Amended by CH2b0 (2026-11-11)

June's `ASI_SA_ALH_SWALLOW_ASSAULT_READY` StateStart (`32b9b3c0`) writes the lock-on's `+0x30` with 99999.0 (`33441c60`) before its ranges: call
`m_sLockOnSystemPtr->SetFacingLineRange( 99999.f )` there (CH2b0 added the member; `ProcessLockOn` is June's since CH2b0).
