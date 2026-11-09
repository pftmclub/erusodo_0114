# June character Rena (Stage 7) - Phase CH2a - Skills: Rena (3)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH2a` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1 review (and any Opus `CH2a0` helper phase CH0 wrote). One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-rena/CH2a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Rena and the first jobs (classes 3, 12, 13, 24)" "Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2a - Done" "CH2a - Review"
```
Then CH0's and CH1's Done notes, and the `CH2a` rows of `june2017_re/char_rena/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the skill ids class 3 has (a skill's line decides its phase: class 3 has it -> CH2a). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI
arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`); its effect closure (a name found
only now is staged byte-exact and added to `ch0_assets.tsv`); its natural-force gain / cost lines under `SERV_IRUHADEV_JUNE_CHAR_RENA_NF`. Upgrade ids
reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id (the
Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Rena: CH2a)`.
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

- **Functions: 46** (`awk -F'\t' '$7=="CH2a"' june2017_re/char_rena/ch0_class_map.tsv`; inside 46).
  June addresses: `32c6b2e0` `32c6b3a0` `32c6b410` `32c6b4a0` `32c6b620` `32c6b680` `32c6b690` `32c6b820` `32c6b870` `32c6b880` `32c6b890` `32c6b8a0` `32c6b8b0` `32c6b910` `32c6b920` `32c6b930` `32c6b940` `32c6bc30` `32c6bc80` `32c74a40` `32c74d50` `32c75060` `32c75120` `32c75d80` `32c76070` `32c76280` `32c76490` `32c76560` `32c768c0` `32c76960` `32c769b0` `32c84740` `32c847b0` `32c86b60` `32c86b90` `32c86bc0` `32c86bf0` `32c86c20` `32c86c50` `32c86dc0` `32c86df0` `32c86e20` `32c86e50` `32c934b0` `32c947c0` `32c94e20`.
- **States: 9** (`awk -F'\t' '$8=="CH2a"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_SI_LE_POWER_ATTACK1`, `LESI_SI1_LE_PIERCE_ARROW`, `LESI_SI1_LE_PIERCE_ARROW_HYPER`, `LESI_SI3_LE_PHOENIX_STRIKE`, `LESI_SI1_LE_ASSAULT_KICK`, `LESI_SI2_LE_PERFECT_STORM`, `LESI_SI3_LE_AERO_TORNADO`, `LESI_SI3_LE_AERO_TORNADO_HYPER`, `LESI_SI_A_LE_BACKJUMP_SHOT`.
- **Skills: 14** (part 2 of `ch0_state_skill.tsv`, col 6): 23000 RNSI_SA_RENA_RAIL_STINGER, 23001 RNSI_SA_RENA_PERFECT_STORM, 23002 RNSI_SA_RENA_AERO_TORNADO, 23003 RNSI_SA_RENA_ASSAULT_KICK, 23004 RNSI_SA_RENA_PHOENIX_STRIKE, 23026 RNSI_SA_RENA_RAIL_STINGER_UPGRADE, 23027 RNSI_SA_RENA_PERFECT_STORM_UPGRADE, 23028 RNSI_SA_RENA_AERO_TORNADO_UPGRADE, 23029 RNSI_SA_RENA_ASSAULT_KICK_UPGRADE, 23030 RNSI_SA_RENA_PHOENIX_STRIKE_UPGRADE, 23200 RNSI_A_RENA_BACKJUMP_SHOT, 23201 RNSI_A_RENA_SIEGEMODE, 23220 RNSI_A_RENA_BACKJUMP_SHOT_UPGRADE, 23221 RNSI_A_RENA_SIEGEMODE_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7i L431-L445 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L719 an id of a listed type with no case plays the class's first state (Ren; §7ae L726 types 14-16 / 17 / 18 named in the class switch; §7ai L848 pass June's skip / adjust / level / creator at every ported BFI call.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH2a`): 2; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
- `LESI_SI_LE_POWER_ATTACK1` (the skill switch default) is here.

## Amended by CH1n (2026-11-09)

- **The default is `SetPowerAttackSkillFunc`** (`_CORE`, June `32cb06d0 @32cb2eef`): both June switches' default registers `LESI_SI_LE_POWER_ATTACK1` with March's callbacks (thunk targets `32c84740` / `32c86b60` / `32c74a40`, hyper `32c847b0` / `32c86b90` / `32c74d50`); under `_LIVE` March's `default:` calls it. Port the bodies there.
- **June's id switch takes `GetUpgradeBaseSkillID( eSkillID_ )`** (`32207210`), as the head's hyper switch does. The first CH2 phase that writes June cases into the id switch switches it on the base id under `_LIVE` (March's `switch( eSkillID_ )` in `#else`).
- **Slot state:** Siege (23201) registers `LESI_SIEGE_READY` ( 167 ) by its table name.
