# June character Rena (Stage 7) - Phase CH2d - Skills: Trapping Ranger (24)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH2d` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2d0 review (and any `CH2d1`). One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-rena/CH2d-review.md` (Opus 5.5 high; the phase writes it when done). **It covers CH2c + CH2d** (user, 2026-11-10): both Done notes, `flagcheck --range` over both phases, `--check-all`, and it retires `_SKILL_DEBUG` |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Rena and the first jobs (classes 3, 12, 13, 24)" "Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2d - Done" "CH2d - Review"
```
Then CH0's and CH1's Done notes, and the `CH2d` rows of `june2017_re/char_rena/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-24 skill ids earlier classes lack: Entangle `:15884`, Evoke trap, Trap Blade `:17797` states, on CH2d0's spawn rules (`SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP`). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI
arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`); its effect closure (a name found
only now is staged byte-exact and added to `ch0_assets.tsv`); its natural-force gain / cost lines under `SERV_IRUHADEV_JUNE_CHAR_RENA_NF`. Upgrade ids
reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id (the
Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Rena: CH2d)`.
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

- **Functions: 47** (`awk -F'\t' '$7=="CH2d"' june2017_re/char_rena/ch0_class_map.tsv`; inside 46, new 1).
  June addresses: `32c6b6a0` `32c6b6e0` `32c6b6f0` `32c6b700` `32c6b710` `32c6c5b0` `32c6c600` `32c6c610` `32c6c640` `32c6c650` `32c6c680` `32c6c690` `32c6c6a0` `32c6c6b0` `32c6c6c0` `32c6c6e0` `32c6c6f0` `32c6cf10` `32c6cf30` `32c6cf50` `32c6cf60` `32c6cf70` `32c75e50` `32c75e90` `32c7a9c0` `32c7a9f0` `32c7aae0` `32c7ad50` `32c7b060`(new ?) `32c7d160` `32c7d360` `32c86d00` `32c86d30` `32c87090` `32c971b0` `32c97210` `32c98360` `32c987f0` `32c98c80` `32c9e0d0` `32c9e440` `32ca67f0` `32ca6810` `32ca6860` `32ca68b0` `32ca6f90` `32ca6fb0`.
- **States: 8** (`awk -F'\t' '$8=="CH2d"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_SI1_LE_HUMMING_WIND`, `LESI_SI1_LE_HUMMING_WIND_HYPER`, `LESI_SA_RTR_EVOKE`, `LESI_A_RTR_EXPLOSION_TRAP`, `LESI_SA_RTR_KARMA`, `LESI_SA_RTR_FATALITY`, `LESI_A_RNW_ANGER_OF_ELF`, `LESI_A_RNW_ROSEBUSH_TRAP`.
- **Skills: 14** (part 2 of `ch0_state_skill.tsv`, col 6): 23019 RNSI_SA_RTR_FATALITY, 23020 RNSI_SA_RTR_HUMMING_WIND, 23021 RNSI_SA_RTR_KARMA, 23033 RNSI_SA_RTR_FATALITY_UPGRADE, 23036 RNSI_SA_RTR_HUMMING_WIND_UPGRADE, 23053 RNSI_SA_RTR_KARMA_UPGRADE, 23214 RNSI_A_RTR_EXPLOSION_TRAP, 23215 RNSI_A_RTR_ROSEBUSH_TRAP, 23216 RNSI_A_RTR_ANGER_OF_ELF, 23218 RNSI_A_RTR_EVOKE, 23224 RNSI_A_RTR_EXPLOSION_TRAP_UPGRADE, 23238 RNSI_A_RTR_ROSEBUSH_TRAP_UPGRADE, 23239 RNSI_A_RTR_ANGER_OF_ELF_UPGRADE, 23241 RNSI_A_RTR_EVOKE_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7 L256 class code scales the cached state power rate (+0x7cc8); §7 L256 March Rena RTR_FATALITY m_fNowStatePowerRate *= m_sFatalityData.GetPow; §7i L431-L445 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L719 an id of a listed type with no case plays the class's first state (Ren; §7ae L726 types 14-16 / 17 / 18 named in the class switch; §7ai L848 pass June's skip / adjust / level / creator at every ported BFI call.

## Amended by CH2c (2026-11-10)

- **Review:** this phase's review also covers CH2c (no review of its own): read `kickoffs/char-rena/CH2c-review.md` with it, write CH2c's findings in the same review note, and retire `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG` there (CH2c left it defined; CH2d may use it for its own lines).
- **Shapes from CH2c:** a hyper row with no direct stores that calls `329cc2c0` = `CopyStateCallbacks( hyperStateData, normalStateData )`; the dash / walk cancels of a skill state with the foot test are `RENA_GROUND_DASH_CANCEL` / `RENA_GROUND_WALK_CANCEL` + `SKILL_CANCEL_AFTER` (June's macro tests the foot); the fifth slot's keys are `pureSkill5` / `pureSkillEx5`; `CX2GUUser::GetSkillLevelValue( id )` is June `31fceab0`.

## Amended by CH2d0 (2026-11-10)

- **Spawn rules are in** (`SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP`): `CreateEvoke` (`32c99690`), `CreateAngerOfElfTrap` (`32c9e250`) and `CreateEntangleTrap` are June's (owner's client spawns; Evoke's memo trap 1025 = the upgraded skill; Anger of Elf's key code = the skill's `SA_EFFECTIVE_TIME`, which the trap script reads as its life), and `X2Game.cpp` has June's 1020 / 1025 / 1420 / 1421 cases (Hunter's Ability rate, list add). The states only call them: June's Evoke hyper FrameMove `32ca6860` calls `CreateEvoke` (March's six inline `CreateNPCReq` calls go to `#else`), Anger of Elf's FrameMove `32ca6fb0` calls `CreateAngerOfElfTrap`. Pass June's count argument to `CreateEvoke` from the listing.
- **CH0 correction:** June keeps `NUI_EVOKE_TRAP_LIRE_MEMO` (1025) and `NUI_RENA_ANGER_OF_ELF_DELAY` (1421). `EntireExplosion` (March `:21072`, reads the delay id) is not March-only: compare it with June's body.
- **Hand-off adds the trap check:** each trap skill cast twice near enemies; `python tools/logs.py --grep "RENA TRAP"` must show per cast `RENA TRAP spawn npc <1020|1025|1420|1421> ...`, then `RENA TRAP linked npc ... owner <unit uid> ...` (Anger of Elf: `key <seconds>` > 0), the `[JUNE_CHAR] SUMMON npc ...` line (flags `0x1`), and when the trap ends (Anger of Elf after `key` seconds; Evoke when exploded or replaced past the count) `RENA TRAP removed ... uid <same uid>`. In game: the trap stays for its time and hits, it does not vanish at once.
- **Review:** CH2d's review retires `SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP_DEBUG` together with `_SKILL_DEBUG`.
- **CH2d0 review (2026-11-10): passed, no code change**; the spawn rules above are confirmed against June (plan doc "CH2d0 - Review").
