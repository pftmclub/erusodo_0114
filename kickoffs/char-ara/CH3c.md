# June character Ara (Stage 7) - Phase CH3c - Skills: Asura (124), SA tier

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-ara CH3c` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH3b, CHNb review (the 124 unit). One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | none of its own: the CH3d review covers it (its `_DEBUG` define stays on) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Martial Artist and the first jobs (classes 7, 28, 31, 34)" "Phase CH3 - Skills: second jobs (118, 119, 124), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH3c - Done"
```

## Task

Scope: the skill ids class 124 has and 7 / 34 lack (a skill's line decides its phase). **CH0 split: this phase is the SA tier, skill ids < 27200 (CH3c2 has the A tier).** For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions
whole (Ghidra, the map's addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` (March's in `#else`) or a new case; its attached rows
(7u names, 7ai BFI arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`, `SPEED_TIME` rows
l.522-526); its effect closure (a name found only now is staged byte-exact and added to `ch0_assets.tsv`); its force-power gain / cost lines on CH1q's
code. Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that
reads the id. A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `ASI_SA_ASR_FOX_PHANTOM`'s `CUSTOM_RENDER_STATE` (checklist 7w): june-engine R5 has not landed, so write a labelled stand-in in Raven's Power Assault shape (`X2GURaven.cpp`, `_SKILL`) with a one-line comment; when R5 lands it writes the buff override `+0x8e64` instead.
- Test on CHNa's 124 unit.
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

- **Functions: 43** (`awk -F'\t' '$7=="CH3c"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 5, new 38).
  June addresses: `32b91390`(new ASI_SA_ASR_SHADOW_RISING_JUMP_Init) `32b914a0`(new ASI_SA_ASR_SHADOW_RISING_LANDING_Init) `32b915c0`(new ASI_SA_ASR_SHADOW_RISING_END_Init) `32b94600`(new ASI_SA_AA_NECROMANCY_Init) `32b94690`(new ASI_SA_AA_NECROMANCY_StateStart) `32b94cd0`(new ASI_SA_ASR_SPECTER_OF_BLADE_Init) `32b94d70`(new ASI_SA_ASR_SPECTER_OF_BLADE_StateStart) `32b94d90`(new ASI_SA_ASR_SPECTER_OF_BLADE_FINISH_Init) `32b94e40`(new ASI_SA_AA_GET_BLOOD_Init) `32b94ec0`(new ASI_SA_AA_GET_BLOOD_FINISH_Init) `32b94f00`(new ASI_SA_AA_FIRE_FOX_Init) `32b94ff0`(new ASI_SA_AA_FIRE_FOX_StateStart) `32b95010`(new ASI_SA_AA_FIRE_FOX_StateEnd) `32b95210`(new ASI_SA_AA_SMASH_Init) `32b95460`(new ASI_SA_ASR_FOX_PHANTOM_Init) `32b9ab00`(new ASI_SA_ASR_SHADOW_RISING_JUMP_EventProcess) `32b9ab40`(new ASI_SA_ASR_SHADOW_RISING_END_EventProcess) `32ba1690`(new ASI_SA_AA_NECROMANCY_EventProcess) `32ba2aa0`(new ASI_SA_ASR_SPECTER_OF_BLADE_FINISH_EventProcess) `32ba2af0`(new ASI_SA_AA_GET_BLOOD_StateStart) `32ba2b30`(new ASI_SA_AA_GET_BLOOD_FINISH_EventProcess) `32ba2e50`(new ASI_SA_AA_FIRE_FOX_EventProcess) `32ba3820`(new ASI_SA_AA_SMASH_StateStartFuture) `32ba3860`(new ASI_SA_AA_SMASH_EventProcess) `32baabf0` `32bac240`(new ASI_SA_ASR_SHADOW_RISING_LANDING_EventProcess) `32bad790`(new ASI_SA_ASR_SPECTER_OF_BLADE_EventProcess) `32bad880`(new ASI_SA_AA_GET_BLOOD_EventProcess) `32bb50c0`(new ASI_SA_ASR_SHADOW_RISING_JUMP_FrameMove) `32bb5310`(new ASI_SA_ASR_SHADOW_RISING_LANDING_FrameMove) `32bb5600`(new ASI_SA_ASR_SHADOW_RISING_END_FrameMove) `32bbdf00`(new ASI_SA_ASR_SPECTER_OF_BLADE_FINISH_FrameMove) `32bbe060`(new ASI_SA_AA_GET_BLOOD_FrameMove) `32bbe2a0`(new ASI_SA_AA_GET_BLOOD_StateEnd) `32bbe2f0`(new ASI_SA_AA_GET_BLOOD_FINISH_FrameMove) `32bbe500`(new ASI_SA_AA_GET_BLOOD_FINISH_StateEnd) `32bbf500`(new ASI_SA_AA_SMASH_FrameMove) `32bc4b90`(new ASI_SA_ASR_SPECTER_OF_BLADE_FrameMove) `32bc4d50`(new ASI_SA_AA_FIRE_FOX_FrameMove) `32bc9680` `32bc96f0` `32bc97b0` `32bcebc0`.
- **States: 11** (`awk -F'\t' '$8=="CH3c"' june2017_re/char_ara/ch0_state_skill.tsv`, part 1, col 8 = phase): `ASI_SA_AA_NECROMANCY`, `ASI_SA_AA_GET_BLOOD`, `ASI_SA_AA_GET_BLOOD_FINISH`, `ASI_SA_AA_FIRE_FOX`, `ASI_SA_AA_SMASH`, `ASI_SA_ASR_SHADOW_RISING_JUMP`, `ASI_SA_ASR_SHADOW_RISING_LANDING`, `ASI_SA_ASR_SHADOW_RISING_END`, `ASI_SA_ASR_SPECTER_OF_BLADE`, `ASI_SA_ASR_SPECTER_OF_BLADE_FINISH`, `ASI_SA_ASR_FOX_PHANTOM`.
- **Skills: 9** (`ch0_state_skill.tsv` part 2, col 6 = the class-level phase; a split half takes the skills whose states (col 5, then part 1 col 8) are its own): 27026 RNSI_SA_ASR_GET_BLOOD, 27027 RNSI_SA_ASR_FIRE_FOX, 27028 RNSI_SA_ASR_NECROMANCY, 27047 RNSI_SA_ASR_SHADOW_RISING, 27048 RNSI_SA_ASR_SPECTER_OF_BLADE, 27061 RNSI_SA_ASR_GET_BLOOD_UPGRADE, 27062 RNSI_SA_ASR_FIRE_FOX_UPGRADE, 27063 RNSI_SA_ASR_NECROMANCY_UPGRADE, 27066 RNSI_SA_ASR_FOX_PHANTOM.
- **Checklist rows: 6** (`awk -F'\t' '$5 ~ /(^| )CH3c( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L166 "CUSTOM_RENDER_STATE enum table: Add x7, Ara x1 (""statement; L178 June-only members of tables March has (SKILL_ID names, ARA_S; L664 CUSTOM_RENDER_STATE class users: Ara_Martial_Artist among th; L713-L745 an id of a listed type with no case plays the class's first ; L726 types 14-16 / 17 / 18 named in the class switch; L868 "BUFF_FACTOR_ID_AND_APPLY_TIME ""appears only in Elesis's sc.
- `ASI_SA_ASR_FOX_PHANTOM`'s `CUSTOM_RENDER_STATE`: labelled stand-in (7w).

## Amended by CH1n (2026-11-11)

- **Second skill switch.** Each id of this phase that has a case in `InitAquiredActiveSkillState` (`32bd34c0`, `awk -F'	' '$1=="32bd34c0"' june2017_re/char_ara/ch0_skill_listing_rows.tsv`)
  gets June's case body too, inside March's case under `_SKILL`. June's chain lookup there takes the upgrade-swapped id: CH1n's head declares
  `eUpgradeSwappedSkillID` (under `_LIVE`) for it; March's cases still pass `iSkillID_`.
- `GetSkillChainInfoPtrBySkillID` is June's under `_LIVE` (CH1n, `32d3ed30`: it also matches the chain skill's enhanced id).
