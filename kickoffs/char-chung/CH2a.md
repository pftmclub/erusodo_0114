# June character Chung (Stage 7) - Phase CH2a - Skills: Iron Cannon (6)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-chung CH2a` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1 review (and any Opus `CH2a0` helper phase CH0 wrote). One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-chung/CH2a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Iron Cannon and the first jobs (classes 6, 20, 21, 27)" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have" "7ae. June skill-tree ids with no March class state (from June skill system S3)"
```
Then CH0's and CH1's Done notes, and the `CH2a` rows of `june2017_re/char_chung/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the skill ids class 6 has. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai
BFI arguments, any 7b block CH0 put here, the numerical-only tables of `plan_june_side.md` §3); its effect closure (a name found only now is staged
byte-exact and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check
the templet swap for every term that reads the id (the Aisha CH3b review defect). An id with no June `SKILL_ID` name stays a raw id with a
one-line comment (plan Rules). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Chung: CH2a)`.
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

- **Functions: 41** (`awk -F'\t' '$7=="CH2a"' june2017_re/char_chung/ch0_class_map.tsv`; inside 35, new 6).
  June addresses: `32bd9160` `32bd91d0` `32bd9260` `32bd9510` `32bd95c0` `32bd95d0` `32bd95e0` `32bd9600` `32bd96e0` `32bda6c0`(new CSI_SI_A_SIEGE_SHELLING_READYLOOP_StateStart) `32be0d50` `32be0da0` `32be0ed0` `32be0f10` `32be0f50` `32be0fb0` `32be5a80` `32be5b40` `32be5d00` `32be5f20` `32bea9c0`(new CSI_A_CC_BACK_BLAST_EventProcess) `32beb7f0` `32beba80` `32bebc40` `32bedf60` `32bf0a90` `32bf2e10` `32bf3430` `32bf4180` `32bf44a0` `32bfa2c0` `32bfa670` `32bfa860` `32bfafa0` `32c0a230`(new CanConsumeCannonBallForShot) `32c0e530` `32c0e9b0` `32c11410`(new PlayCannonShotEffectSet) `32c11e10` `32c24360`(new -) `32c244a0`(new -).
- **States: 16** (`awk -F'\t' '$8=="CH2a"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_A_CC_BACK_BLAST`, `CSI_SI_SA_CHUNG_SCARE_CHASE`, `CSI_SI_SA_CHUNG_SCARE_CHASE_HYPER`, `CSI_SI_A_CHUNG_RELOAD_CANNON`, `CSI_SI_A_SIEGE_SHELLING_READY`, `CSI_SI_A_SIEGE_SHELLING_READYLOOP`, `CSI_SI_A_SIEGE_SHELLING_X`, `CSI_SI_A_SIEGE_SHELLING_X_FINISH`, `CSI_SI_A_SIEGE_SHELLING_Z_FINISH`, `CSI_SI_SA_CHUNG_LUNATIC_BLOW_START`, `CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_HYPER`, `CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION`, `CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH`, `CSI_SI_SA_CHUNG_ACELDAMA`, `CSI_SI_SA_CHUNG_ACELDAMA_HYPER`, `CSI_SI_A_CHUNG_AIMMING_SHOT`.
- **Skills: 14** (part 2 of `ch0_state_skill.tsv`, col 6): 26000 RNSI_SA_CC_SCARE_CHASE, 26001 RNSI_SA_CC_LUNATIC_BLOW, 26002 RNSI_SA_CC_ACELDAMA, 26027 not found, 26028 not found, 26029 not found, 26200 RNSI_A_CC_BACK_BLAST, 26201 RNSI_A_CC_SIEGE_SHELLING, 26202 RNSI_A_CC_RELOAD_CANNON, 26203 RNSI_A_CC_AIMMING_SHOT, 26216 not found, 26217 not found, 26218 not found, 26219 not found.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L53 numerical-only DAMAGE_DATA table CSI_A_CC_BACK_BLAST (script L6455); §1 L53 numerical-only DAMAGE_DATA table CSI_SI_A_CHUNG_RELOAD_CANNON (script ; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_A_SIEGE_SHELLING_READY (script; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_A_SIEGE_SHELLING_Z_FINISH (scr; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSIO; §1 L53 numerical-only DAMAGE_DATA table CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSIO; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 15 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178-181 June-only members of tables March has (SKILL_ID names) fail silently; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L713-720 an id of a listed type with no case plays the class's first skill stat; §7ae L728 49 rows with no June SKILL_ID name (Chung upgrade ids 26027-26029, 262; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- Siege Shelling (26201: `CSI_SI_A_SIEGE_SHELLING_*`, 2 numerical-only tables) and Back Blast (26200) are class 6 in June's tree: here, not CH2c.
- The 7 unnamed ids (26027-26029, 26216-26219; 49 tree rows) have no SKILL_ID name in either Enum (`ch0_unnamed_ids.tsv`; 26027-26029 collide only with `XCCUM_MGR_GROUP_*`): raw id + one-line comment with the templet's name.
