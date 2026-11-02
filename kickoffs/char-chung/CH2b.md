# June character Chung (Stage 7) - Phase CH2b - Skills: Fury Guardian (20)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH2b` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2b0 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-chung/CH2b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH2b` rows of `june2017_re/char_chung/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the skill ids class 20 has that class 6 does not. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai
BFI arguments, any 7b block CH0 put here, the numerical-only tables of `plan_june_side.md` §3); its effect closure (a name found only now is staged
byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check
the templet swap for every term that reads the id (the Aisha CH3b review defect). An id with no June `SKILL_ID` name stays a raw id with a
one-line comment (plan Rules). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Chung: CH2b)`.
- Hand-off: the unit and class, each skill id and its A / B slot (never slot 4 / 9 before transcendence), what June's motion and effect look like.

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

## Amended by CH0 (2026-10-31)

- **Functions: 39** (`awk -F'\t' '$7=="CH2b"' june2017_re/char_chung/ch0_class_map.tsv`; inside 38, new 1).
  June addresses: `32bd94c0` `32bd94f0` `32bd9820` `32bd98e0` `32bd98f0` `32bd9900`(new CSI_SI_A_CFG_LEAP_ATTACK_READY_StateStartFuture) `32bd9930` `32bd9950` `32bd9970` `32bd9990` `32bd99b0` `32bd9ec0` `32bda4d0` `32bda530` `32bda540` `32bda590` `32be0e90` `32be15b0` `32be15e0` `32be1620` `32be1650` `32be1690` `32be16c0` `32be16f0` `32be1720` `32be1760` `32be2f10` `32be4da0` `32be4e70` `32be4f50` `32beb890` `32bee840` `32bf0b20` `32bf1000` `32bfa4a0` `32bfb6e0` `32bfb8f0` `32c003c0` `32c007e0`.
- **States: 19** (`awk -F'\t' '$8=="CH2b"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_SI_A_CFG_LEAP_ATTACK_READY`, `CSI_SI_A_CFG_LEAP_ATTACK_JUMP`, `CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN`, `CSI_SI_A_CFG_LEAP_ATTACK_LANDING`, `CSI_SI_A_CIP_LEAP_ATTACK_LANDING`, `CSI_SI_SA_CIP_BURST_WOLF`, `CSI_SI_SA_CIP_BURST_WOLF_HYPER`, `CSI_SI_SA_CFG_PANDEMONIUM`, `CSI_SI_SA_CFG_PANDEMONIUM_HYPER`, `CSI_SI_SA_CHUNG_BRUTAL_SWING`, `CSI_SI_SA_CHUNG_BRUTAL_SWING_HYPER`, `CSI_SI_A_CIP_SUDDEN_BURSTER`, `CSI_SI_SA_CFG_LUNATIC_FURY_READY`, `CSI_SI_SA_CFG_LUNATIC_FURY_READY_HYPER`, `CSI_SI_SA_CFG_LUNATIC_FURY_RUN`, `CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK`, `CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING`, `CSI_SI_SA_CFG_LUNATIC_FURY_END`, `CSI_SI_SA_CFG_LUNATIC_FURY_END_HYPER`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 26003 RNSI_SA_CIP_BURST_WOLF, 26004 RNSI_SA_CFG_PANDEMONIUM_CHAOS, 26005 RNSI_SA_CC_BRUTAL_SWING, 26006 RNSI_SA_CFG_LUNATIC_FURY, 26031 RNSI_SA_CIP_BURST_WOLF_UPGRADE, 26034 RNSI_SA_CFG_PANDEMONIUM_CHAOS_UPGRADE, 26043 RNSI_SA_CC_BRUTAL_SWING_UPGRADE, 26044 RNSI_SA_CFG_LUNATIC_FURY_UPGRADE, 26204 RNSI_A_CFG_LEAP_ATTACK, 26205 RNSI_A_CIP_SUDDEN_BURSTER, 26222 RNSI_A_CFG_LEAP_ATTACK_UPGRADE, 26229 RNSI_A_CIP_SUDDEN_BURSTER_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L53 numerical-only DAMAGE_DATA table CSI_SI_SA_CIP_BURST_WOLF (script L833; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_SA_CIP_BURST_WOLF_HYPER (scrip; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_SA_CHUNG_BRUTAL_SWING (script ; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_SA_CHUNG_BRUTAL_SWING_HYPER (s; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_A_CIP_SUDDEN_BURSTER (script L; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_SA_CFG_LUNATIC_FURY_RUN (scrip; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 15 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178-181 June-only members of tables March has (SKILL_ID names) fail silently; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L713-720 an id of a listed type with no case plays the class's first skill stat; §7ae L728 49 rows with no June SKILL_ID name (Chung upgrade ids 26027-26029, 262; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- **Depends on CH0's Opus helper phase CH2b0** (`31fcf430`, `329cc340`, `31fc8900`).
- Opus helper phase **CH2b0** runs first (`31fcf430` master-ultimate memo lookup, `329cc340` vslot 0x524 predicate, `31fc8900`).
- Brutal Swing (26005, numerical-only x2) is class 20 / 110: here.

## Amended by CH2b0 (2026-11-02)

The three helpers are ported (flag `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, funcmap rows `(CH2b0)`); call them by these March names:
- `31fcf430` -> `CX2GUUser::GetUpgradeSwappedSkillID( eSkillID )` (the upgrade swap; no unit -> the id). It is not a memo lookup.
- `329cc340` -> `CX2GUUser::IsNowStateSkillUpgraded()` (= `IsEquippedSkillUpgraded( GetNowStateSkillID() )`).
- `31fc8900` (ECX = `+0x8b58`, arg `+0x86da`) -> `m_kSkillSlotKey.IsKeyPressedOnce( m_InputData )`; `31fce670` at the top of `32be1760` is `m_kSkillSlotKey.FollowSlotSwap()`.
