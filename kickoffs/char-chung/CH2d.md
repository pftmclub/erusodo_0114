# June character Chung (Stage 7) - Phase CH2d - Skills: Shelling Guardian (27)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH2d` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH2c review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-chung/CH2d-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH2d` rows of `june2017_re/char_chung/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the skill ids class 27 has that class 6 does not. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai
BFI arguments, any 7b block CH0 put here, the numerical-only tables of `plan_june_side.md` §3); its effect closure (a name found only now is staged
byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check
the templet swap for every term that reads the id (the Aisha CH3b review defect). An id with no June `SKILL_ID` name stays a raw id with a
one-line comment (plan Rules). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Chung: CH2d)`.
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

- **Functions: 42** (`awk -F'\t' '$7=="CH2d"' june2017_re/char_chung/ch0_class_map.tsv`; inside 38, new 4).
  June addresses: `32bda750` `32bda760` `32bda770` `32bda7c0` `32bdada0` `32bdadc0` `32bdae40` `32bdae60` `32bdb0d0` `32be6040` `32be65e0` `32be6780` `32be69b0` `32be6a50` `32be6af0` `32be6b30` `32be6bf0` `32be77c0` `32be8140` `32be8df0`(new ?) `32bec100` `32bed4c0` `32bedfc0` `32bee020` `32bee0e0` `32bf35a0` `32c05060` `32c05c90` `32c05e80` `32c06050` `32c073e0` `32c0ede0` `32c0f1f0` `32c0f8c0` `32c0fc30`(new CSI_SI_A_CHG_CHAOS_CANNON_01_FrameMove) `32c10fb0` `32c116e0`(new CreateCannonShotDamageEffect) `32c11980`(new SetLockOnCannonShot) `32c12100` `32c12240` `32c12400` `32c12660`.
- **States: 25** (`awk -F'\t' '$8=="CH2d"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_A_CHG_ELASTIC_BOMB`, `CSI_A_CHG_ELASTIC_BOMB_HYPER`, `CSI_SA_CHG_BIGBANG_STREAM`, `CSI_SA_CHG_BIGBANG_STREAM_HYPER`, `CSI_SA_CHG_DREAD_CHASE_START`, `CSI_SA_CHG_DREAD_CHASE_START_HYPER`, `CSI_SA_CHG_DREAD_CHASE_LOOP`, `CSI_SA_CHG_DREAD_CHASE_END`, `CSI_A_CHUNG_IMPACT_DETONATION_READY`, `CSI_A_CHUNG_IMPACT_DETONATION_LOOP`, `CSI_A_CHUNG_IMPACT_DETONATION_FINAL`, `CSI_A_CSG_MAGNUM_SHOT`, `CSI_SI_A_CHG_CHAOS_CANNON_START`, `CSI_SI_A_CHG_CHAOS_CANNON_START_HYPER`, `CSI_SI_A_CHG_CHAOS_CANNON_WAIT`, `CSI_SI_A_CHG_CHAOS_CANNON_WALK`, `CSI_SI_A_CHG_CHAOS_CANNON_JUMP_READY`, `CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP`, `CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN`, `CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG`, `CSI_SI_A_CHG_CHAOS_CANNON_END`, `CSI_SI_A_CHG_CHAOS_CANNON_GUARD`, `CSI_SI_A_CHG_CHAOS_CANNON_01`, `CSI_SI_A_CHG_CHAOS_CANNON_02`, `CSI_SI_A_CHG_CHAOS_CANNON_03`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 26019 RNSI_SA_CHG_BIGBANG_STREAM, 26020 RNSI_SA_CTT_DREAD_CHASE, 26021 RNSI_SA_CC_IMPACT_DETONATION, 26022 RNSI_SA_CHG_CHAOS_CANNON, 26033 RNSI_SA_CHG_BIGBANG_STREAM_UPGRADE, 26036 RNSI_SA_CTT_DREAD_CHASE_UPGRADE, 26056 RNSI_SA_CC_IMPACT_DETONATION_UPGRADE, 26057 RNSI_SA_CHG_CHAOS_CANNON_UPGRADE, 26212 RNSI_A_CHG_ELASTIC_BOMB_HAND_GRENADE, 26213 RNSI_A_CSG_MAGNUM_SHOT, 26224 RNSI_A_CHG_ELASTIC_BOMB_HAND_GRENADE_UPGRADE, 26236 RNSI_A_CSG_MAGNUM_SHOT_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 15 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §4 L178-181 June-only members of tables March has (SKILL_ID names) fail silently; §7 L280 3201ad10 skill-group power socket rate called from Chung code: 32bec28; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L713-720 an id of a listed type with no case plays the class's first skill stat; §7ae L728 49 rows with no June SKILL_ID name (Chung upgrade ids 26027-26029, 262; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- 7 l.280 call sites `32bec286` (Chaos Cannon) / `32c058a0` (Big Bang Stream) ride with their states.
