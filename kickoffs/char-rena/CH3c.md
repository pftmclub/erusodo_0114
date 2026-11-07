# June character Rena (Stage 7) - Phase CH3c - Skills: Night Watcher (114)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH3c` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH3c0 review (CH0 helper phase). One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-rena/CH3c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Rena and the first jobs (classes 3, 12, 13, 24)" "Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH3c - Done" "CH3c - Review"
```
Then CH0's and CH1's Done notes, and the `CH3c` rows of `june2017_re/char_rena/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-114 skill ids earlier classes lack (the `RNW` / `LNW` skill states; her traps' Night Watcher variants on CH2d0's rules). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI
arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`); its effect closure (a name found
only now is staged byte-exact and added to `ch0_assets.tsv`); its natural-force gain / cost lines under `SERV_IRUHADEV_JUNE_CHAR_RENA_NF`. Upgrade ids
reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id (the
Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Rena: CH3c)`.
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

## Amended by CH0 (2026-11-07)

- **Functions: 46** (`awk -F'\t' '$7=="CH3c"' june2017_re/char_rena/ch0_class_map.tsv`; inside 33, new 13).
  June addresses: `32c6b600` `32c6b610` `32c6c7a0` `32c6c7b0` `32c6c7c0` `32c6c7e0` `32c6c7f0` `32c6c800` `32c6c810` `32c6cc30` `32c6cda0` `32c6ce40` `32c6cec0` `32c6cef0` `32c6d060`(new LESI_SA_RNW_SEED_OF_RUIN_POLE_Init) `32c6d070`(new LESI_SA_RNW_RAGE_FUNGUS_Init) `32c6d0a0`(new LESI_A_RNW_BUTTERFLY_Init) `32c6d910`(new LESI_SA_RNW_ERADICATION_START_Init) `32c75cf0` `32c7ada0` `32c7adc0` `32c7ae60` `32c7af00` `32c7af40` `32c7cde0` `32c7ce00` `32c7d000` `32c7d0c0` `32c7d0f0` `32c7e3d0`(new LESI_SA_RNW_SEED_OF_RUIN_POLE_EventProcess) `32c7e6e0`(new LESI_SA_RNW_RAGE_FUNGUS_EventProcess) `32c7f010`(new LESI_A_RNW_BUTTERFLY_EventProcess) `32c83480`(new LESI_SA_RNW_ERADICATION_START_EventProcess) `32c834b0`(new LESI_SA_RNW_ERADICATION_ATTACK_EventProcess) `32c84e20`(new LESI_SA_RTR_CALL_OF_RUIN_StateStartFuture) `32c98ce0` `32c98d50` `32c990d0` `32c99280` `32c9dc40` `32c9eac0`(new LESI_A_RNW_BUTTERFLY_StateEnd) `32ca6970` `32ca7000`(new LESI_A_RNW_BUTTERFLY_StateStart) `32ca7810`(new LESI_SA_RNW_ERADICATION_ATTACK_StateStart) `32ca95f0` `32caca80`.
- **States: 11** (`awk -F'\t' '$8=="CH3c"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_SA_RTR_CALL_OF_RUIN`, `LESI_SA_RNW_FURIOUS_ENGAGE`, `LESI_A_RNW_START_OF_DELAYED_FIRING`, `LESI_SA_RNW_GLIDING_STRIKE`, `LESI_SA_RTR_TRAPPING_ARROW_FUNGUS`, `LESI_P_RNW_SEED_OF_RUIN`, `LESI_A_RNW_BUTTERFLY`, `LESI_SA_RNW_SEED_OF_RUIN_POLE`, `LESI_SA_RNW_RAGE_FUNGUS`, `LESI_SA_RNW_ERADICATION_START`, `LESI_SA_RNW_ERADICATION_ATTACK`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 23022 RNSI_SA_RNW_CALL_OF_RUIN, 23023 RNSI_SA_RNW_GLIDING_STRIKE, 23024 RNSI_SA_RNW_TRAPPING_ARROW_FUNGUS, 23025 RNSI_SA_RNW_FURIOUS_ENGAGE, 23041 RNSI_SA_RNW_SEED_OF_RUIN_POLE, 23042 RNSI_SA_RNW_RAGE_FUNGUS, 23054 RNSI_SA_RNW_CALL_OF_RUIN_UPGRADE, 23055 RNSI_SA_RNW_GLIDING_STRIKE_UPGRADE, 23056 RNSI_SA_RNW_TRAPPING_ARROW_FUNGUS_UPGRADE, 23057 RNSI_SA_RNW_FURIOUS_ENGAGE_UPGRADE, 23060 RNSI_SA_RNW_ERADICATION, 23217 RNSI_A_RNW_DELAYED_FIRE, 23219 RNSI_A_RNW_SEED_OF_RUIN, 23227 RNSI_A_RNW_BUTTERFLY, 23240 RNSI_A_RNW_DELAYED_FIRE_UPGRADE, 23242 RNSI_A_RNW_SEED_OF_RUIN_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L56 numerical-only DAMAGE_DATA table in state LESI_SA_RNW_FURIOUS_ENGAGE (; §1 L56 numerical-only DAMAGE_DATA_NEXT table in state LESI_SA_RNW_FURIOUS_ENG; §1 L56 numerical-only DAMAGE_DATA_THIRD table in state LESI_SA_RNW_FURIOUS_EN; §1 L56 numerical-only DAMAGE_DATA_LAST table in state LESI_SA_RNW_FURIOUS_ENG; §1 L56 numerical-only DAMAGE_DATA + numerical override live (the 12 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7i L431-L445 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L719 an id of a listed type with no case plays the class's first state (Ren; §7ae L726 types 14-16 / 17 / 18 named in the class switch; §7ai L848 pass June's skip / adjust / level / creator at every ported BFI call.
- `LESI_SA_RNW_FURIOUS_ENGAGE` FrameMove `32caca80` reads `DAMAGE_DATA_THIRD` / `DAMAGE_DATA_LAST`. Depends on CH3c0 (the Night Watcher engine hook).
