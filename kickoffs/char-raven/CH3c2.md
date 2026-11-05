# June character Raven (Stage 7) - Phase CH3c2 - Skills: Veteran Commander Ignition Crow and Deadly Raid (with their overheat states)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH3c2` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3c review; one at a time with the other `X2GURaven.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-raven/CH3c2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Raven and the first jobs (classes 4, 16, 17, 25)" "Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH3c - Done" "CH3c - Review"
```
Then CH0's and CH1's Done notes, and the `CH3c2` rows of `june2017_re/char_raven/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope (CH0 split, below): the six Ignition Crow / Deadly Raid ids only; the rest of the class-115 ids are CH3c. As CH3c: `RAVEN_SEVEN_BURST_1/2_HYPER`, `RAVEN_MAXIMUM_CANNON_HYPER` / `_FLY_HYPER`, `EffectSet_RAVEN_VJAVELIN[_LAND]` (7u); the overheat pieces `ch0_overheat.tsv` gives the skills. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables); its effect closure (a name found only now is staged byte-exact and
added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Raven: CH3c2)`.
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

- **Functions: 45** (`awk -F'\t' '$7=="CH3c2"' june2017_re/char_raven/ch0_class_map.tsv`; inside 43, new 2).
  June addresses: `32c272f0` `32c27300` `32c27310` `32c27b20` `32c36e00` `32c36e50` `32c36e80` `32c37080` `32c37270` `32c37470`(new ?) `32c374c0` `32c376c0` `32c376f0` `32c3a260` `32c3a460` `32c3a490` `32c3e740` `32c3ea40` `32c42690` `32c427b0` `32c42850` `32c428f0` `32c42990` `32c42c30` `32c4cba0` `32c4cf30` `32c4d350` `32c4d770` `32c4db90` `32c538c0` `32c53b10` `32c53d60` `32c53fb0` `32c54780` `32c5af90`(new ?) `32c5b0a0` `32c5b560` `32c5bb40` `32c5be30` `32c5bf80` `32c61fa0` `32c623d0` `32c62800` `32c62c30` `32c634f0`.
- **States: 22** (`awk -F'\t' '$8=="CH3c2"' june2017_re/char_raven/ch0_state_skill.tsv`): `RVC_SI_SA_IGNITION_CROW`, `RVC_SI_SA_IGNITION_CROW_FIRE`, `RVC_SI_SA_IGNITION_CROW_FIRE_OVERHEAT`, `RVC_SI_SA_IGNITION_CROW_OVERHEAT_END`, `RVC_SI_SA_IGNITION_CROW_INCINERATION`, `RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE`, `RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE_OVERHEAT`, `RVC_SI_SA_IGNITION_CROW_INCINERATION_OVERHEAT_END`, `RVC_SI_SA_DEADLY_RAID`, `RVC_SI_SA_DEADLY_RAID_LOOP`, `RVC_SI_SA_DEADLY_RAID_LOOP_END`, `RVC_SI_SA_DEADLY_RAID_FINISH`, `RVC_SI_SA_DEADLY_RAID_END`, `RVC_SI_SA_DEADLY_RAID_LOOP_OVERHEAT`, `RVC_SI_SA_DEADLY_RAID_LOOP_END_OVERHEAT`, `RVC_SI_SA_DEADLY_RAID_FINISH_OVERHEAT`, `RVC_SI_SA_IGNITION_CROW_UPGRADE`, `RVC_SI_SA_IGNITION_CROW_FIRE_UPGRADE`, `RVC_SI_SA_IGNITION_CROW_FIRE_OVERHEAT_UPGRADE`, `RVC_SI_SA_IGNITION_CROW_INCINERATION_UPGRADE`, `RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE_UPGRADE`, `RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE_OVERHEAT_UPGRADE`.
- **Skills: 6** (part 2 of `ch0_state_skill.tsv`, col 6): 24022 RNSI_SA_RVC_IGNITION_CROW_NAPALM, 24023 RNSI_SA_RVC_IGNITION_CROW_INCINERATION, 24024 RNSI_SA_RVC_DEADLY_RAID, 24053 RNSI_SA_RVC_IGNITION_CROW_NAPALM_UPGRADE, 24054 RNSI_SA_RVC_IGNITION_CROW_INCINERATION_UPGRADE, 24055 RNSI_SA_RVC_DEADLY_RAID_UPGRADE.
- Scope (CH0 split of CH3c): Ignition Crow Napalm 24022 / 24053, Incineration 24023 / 24054, Deadly Raid 24024 / 24055: the `RVC_SI_SA_*` tables incl. the `*_OVERHEAT` variants, helpers `32c37470` / `32c5af90`. `RVC_SI_SA_IGNITION_CROW*_OVERHEAT_END_UPGRADE` are dead data. Overheat mode itself (24415) is CH4c's; CH1g2 provides `CreateOverHeatObject`.
