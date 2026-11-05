# June character Raven (Stage 7) - Phase CH3c - Skills: Veteran Commander (115)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-raven CH3c` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH3b review (and any `CH3c0`). One at a time: `X2GURaven.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RAVEN SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-raven/CH3c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Raven and the first jobs (classes 4, 16, 17, 25)" "Phase CH3 - Skills: second jobs (106, 107, 115), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH3c - Done" "CH3c - Review"
```
Then CH0's and CH1's Done notes, and the `CH3c` rows of `june2017_re/char_raven/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-115 skill ids no first job has; `RAVEN_SEVEN_BURST_1/2_HYPER`, `RAVEN_MAXIMUM_CANNON_HYPER` / `_FLY_HYPER`, `EffectSet_RAVEN_VJAVELIN[_LAND]` (7u); the overheat pieces `ch0_overheat.tsv` gives the skills. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables); its effect closure (a name found only now is staged byte-exact and
added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Raven: CH3c)`.
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

- **Functions: 34** (`awk -F'\t' '$7=="CH3c"' june2017_re/char_raven/ch0_class_map.tsv`; inside 25, new 9).
  June addresses: `32c26bf0` `32c27000` `32c27010` `32c27030` `32c27040` `32c271a0` `32c27230` `32c27240` `32c27c40`(new RSI_SA_RVC_FLAME_HELL_Init) `32c27c50`(new RSI_SA_RVC_BURSTING_BLADE_Init) `32c27c80`(new RSI_A_RVC_FLAME_SWORD_ANOTHER_Init) `32c28150`(new RSI_SA_RVC_IFRIT_FLAME_Init) `32c33100` `32c35510` `32c35c50` `32c35c80` `32c35cb0` `32c35cf0` `32c3b720`(new RSI_SA_RVC_FLAME_HELL_EventProcess) `32c3ba30`(new RSI_SA_RVC_BURSTING_BLADE_EventProcess) `32c3c360`(new RSI_A_RVC_FLAME_SWORD_ANOTHER_EventProcess) `32c3c930`(new RSI_SA_RVC_IFRIT_FLAME_EventProcess) `32c3e130`(new ?) `32c3ea10` `32c42370` `32c4a080` `32c4bcd0` `32c4c070` `32c53560` `32c536c0` `32c538a0` `32c5a5d0` `32c5a6c0` `32c60fc0`.
- **States: 12** (`awk -F'\t' '$8=="CH3c"' june2017_re/char_raven/ch0_state_skill.tsv`): `RSI_A_RF_BREAKING_FIST`, `RSI_SA_RWT_GIGA_PROMINENCE`, `RSI_SA_RWT_GIGA_PROMINENCE_LOOP`, `RSI_SA_RWT_GIGA_PROMINENCE_FINISH`, `RSI_SA_RWT_GIGA_PROMINENCE_END`, `RSI_SA_RWT_GIGA_PROMINENCE_OVER_END`, `SI_A_RRF_GROUND_IMPACT`, `RSI_A_RVC_FLAME_SWORD_ANOTHER`, `RSI_SA_RVC_FLAME_HELL`, `RSI_SA_RVC_BURSTING_BLADE`, `RSI_A_RF_BREAKING_FIST_UPGRADE`, `RSI_SA_RVC_IFRIT_FLAME`.
- **Skills: 10** (part 2 of `ch0_state_skill.tsv`, col 6): 24021 RNSI_SA_RVC_GIGA_PROMINENCE, 24039 RNSI_SA_RVC_FLAME_HELL, 24040 RNSI_SA_RVC_BURSTING_BLADE, 24052 RNSI_SA_RVC_GIGA_PROMINENCE_UPGRADE, 24058 RNSI_SA_RVC_IFRIT_FLAME, 24215 RNSI_A_RVC_BREAKING_FIST, 24216 RNSI_A_RVC_GROUND_IMPACT, 24226 RNSI_A_RVC_FLAME_SWORD_ANOTHER, 24238 RNSI_A_RVC_BREAKING_FIST_UPGRADE, 24239 RNSI_A_RVC_GROUND_IMPACT_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L50 numerical-only DAMAGE_DATA table in state RVC_SI_SA_DEADLY_RAID (scrip; §1 L50 numerical-only DAMAGE_DATA table in state RVC_SI_SA_DEADLY_RAID_LOOP (; §1 L50 numerical-only DAMAGE_DATA table in state RVC_SI_SA_DEADLY_RAID_LOOP_E; §1 L50 numerical-only DAMAGE_DATA table in state RVC_SI_SA_DEADLY_RAID_FINISH; §1 L50 numerical-only DAMAGE_DATA table in state RVC_SI_SA_DEADLY_RAID_LOOP_O; §1 L50 numerical-only DAMAGE_DATA table in state RVC_SI_SA_DEADLY_RAID_LOOP_E; §1 L50 numerical-only DAMAGE_DATA table in state RVC_SI_SA_DEADLY_RAID_FINISH; §1 L50 numerical-only DAMAGE_DATA + numerical override live (the 25 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names, RAVEN_STATE_ID); §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L714-722 an id of a listed type with no case plays the class's first state (Rav; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; §7ai L844 pass June's skip / adjust / level / creator at every ported BFI call.
- **Split by CH0:** CH3c = Veteran Commander skills except Ignition Crow and Deadly Raid, which are **CH3c2** (45 functions with their overheat states).
