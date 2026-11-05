# June character Elsword (Stage 7) - Phase CH3a - Skills: Lord Knight (100)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH3a` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH2d review (and any Opus `CH3a0` helper phase CH0 wrote). One at a time: `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `..._SKILL_DEBUG`, still defined: the CH2d review kept it on for CH2d's play-test; leave it on) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-elsword/CH3a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Sword Man and the first jobs (classes 1, 10, 11, 22)" "Phase CH3 - Skills: second jobs (100, 101, 112), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
```
Then CH0's and CH1's Done notes, and the `CH3a` rows of `june2017_re/char_elsword/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the ids class 100 adds (`LORD_KNIGHT_SAND_STORM`, a March-only templet, 7u). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, the numerical-only tables, alpha-tag sites in its functions); its effect closure (a name found only now is staged byte-exact
and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Elsword: CH3a)`.
- Hand-off: the unit and class, each skill id and its A / B slot (never slot 4 / 9 before transcendence), what June's motion and effect look like.
- From the CH1 review: Counter 21206 is the first skill that reaches `ESSI_REVENGE_ATTACK` (CH1b, multiplied damage, `NUMERICAL_DATA_INDEX = 1`).
  The play-test shows the counter strike hitting for a non-zero amount (unit 25 has 21206 at level 0, unslotted: learn and slot it in A / B 0-3).

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

## Amended by CH0 (2026-11-03)

- **Functions: 38** (`awk -F'\t' '$7=="CH3a"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 23, new 15).
  June addresses: `32cf3650` `32cf3670` `32cf3b70` `32cf3d70` `32cf3da0`(new ESSI_SA_ESK_WINDMILL_START_StateStartFuture) `32cf3dd0` `32cf3e20`(new ESSI_SA_ESK_WINDMILL_FrameMoveFuture) `32cf3e90` `32cf4e80`(new ESSI_SA_ELK_GRAND_CROSS_START_Init) `32cf4e90`(new ESSI_SA_ELK_GRAND_CROSS_START_StateStart) `32cf4ec0`(new ESSI_SA_ELK_GRAND_CROSS_START_StateStartFuture) `32cf4ef0`(new ESSI_SA_ELK_GRAND_CROSS_CHARGE_FrameMove) `32cf4f60`(new ESSI_SA_ELK_GRAND_CROSS_CHARGE_EventProcess) `32cf4fe0`(new ESSI_SA_ELK_GRAND_CROSS_CHARGE_StateEnd) `32cf6290` `32cf9f60` `32cfa000` `32cfa910` `32cfa970` `32cfa9a0` `32cfaa30` `32cfc050` `32cfc120` `32cfc150` `32cfc1f0` `32cfc5d0` `32d05f00`(new ESSI_SA_ELK_SONIC_BLADE_CRESCENT_EventProcess) `32d061a0`(new ESSI_SA_ELK_GRAND_CROSS_START_EventProcess) `32d061d0`(new ESSI_SA_ELK_GRAND_CROSS_END_EventProcess) `32d06550`(new ESSI_SA_ELK_GRAND_CROSS_FULL_CHARGE_EventProcess) `32d087a0` `32d0eaf0` `32d11cb0` `32d11fd0` `32d12210` `32d19dd0`(new ?) `32d1b930`(new ESSI_SA_ELK_GRAND_CROSS_END_FrameMove) `32d1b980`(new ESSI_SA_ELK_GRAND_CROSS_FULL_CHARGE_FrameMove).
- **States: 18** (`awk -F'\t' '$8=="CH3a"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_A_ELK_ROLLING_SMASH_READY`, `ESSI_A_ELK_ROLLING_SMASH`, `ESSI_A_ELK_ROLLING_SMASH_LANDING`, `ESSI_SI3_ES_ARMAGEDDON_BLADE`, `ESSI_SA_ELK_SAND_STORM`, `ESSI_SA_ESK_WINDMILL_START`, `ESSI_SA_ESK_WINDMILL`, `ESSI_SA_ESK_WINDMILL_END`, `ESSI_REVENGE_WAIT`, `ESSI_SA_ELK_SONIC_BLADE`, `ESSI_A_ELK_POWER_SMASH`, `ESSI_SA_ELK_SONIC_BLADE_CRESCENT`, `ESSI_SA_ELK_IMPERIAL_CRUSHER`, `ESSI_SA_ELK_GRAND_CROSS_START`, `ESSI_SA_ELK_GRAND_CROSS_CHARGE`, `ESSI_SA_ELK_GRAND_CROSS_END`, `ESSI_SA_ELK_GRAND_CROSS_FULL_CHARGE`, `ESSI_SA_ELK_SAND_STORM_UPGRADE`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 21008 RNSI_SA_ELK_ARMAGEDON_BLADE, 21009 RNSI_SA_ELK_SAND_STORM, 21010 RNSI_SA_ELK_WINDMILL, 21011 RNSI_SA_ELK_SONIC_BLADE, 21037 RNSI_SA_ELK_SONIC_BLADE_CRESCENT, 21038 RNSI_SA_ELK_IMPERIAL_CRUSHER, 21044 RNSI_SA_ELK_ARMAGEDON_BLADE_UPGRADE, 21045 RNSI_SA_ELK_SAND_STORM_UPGRADE, 21046 RNSI_SA_ELK_WINDMILL_UPGRADE, 21047 RNSI_SA_ELK_SONIC_BLADE_UPGRADE, 21058 RNSI_SA_ELK_GRAND_CROSS, 21205 RNSI_A_ELK_ROLLING_SMASH, 21206 RNSI_A_ELK_COUNTER, 21222 RNSI_A_ELK_POWER_SMASH, 21227 RNSI_A_ELK_ROLLING_SMASH_UPGRADE, 21228 RNSI_A_ELK_COUNTER_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L51 numerical-only DAMAGE_DATA table in state ESSI_A_ELK_ROLLING_SMASH (sc; §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SI3_ES_ARMAGEDDON_BLADE; §1 L51 numerical-only DAMAGE_DATA table in state ESSI_REVENGE_ATTACK (script ; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 20 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names) fail silently; §7 L254 class code scales the cached state power rate (+0x7cc8); §7h L420 Elsword 25 functions 32cf8230..32d06c50 (e.g. 32d01040): UPDOWN_CANCEL; §7h L425 USEFUL_VALUE rows and getters; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7u L635 March-only effect templets: X2GUElsword_SwordMan.cpp LORD_KNIGHT_SAND_; §7ae L713-721 an id of a listed type with no case plays the class's first skill stat; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim; §7ai L839 pass June's skip / adjust / level / creator at every ported BFI call.
- March-only templet `LORD_KNIGHT_SAND_STORM` at `X2GUElsword_SwordMan.cpp:15450` (7u: take June's caller, drop the name).

## Amended by CH1p (2026-11-04)

- Held-key skills read `CX2GUUser::m_kSkillSlotKey` (June `+0x8b58`; `Set` `31fce6d0`, `IsKeyReleased` `31fc89a0`, `X2GUUser.h`, `_EVE_CORE`): Windmill start / loop StartFuture `32cf3da0` and EventProcess `32d11fd0` (inside March's `ESSI_SA_ESK_WINDMILL_EventProcess`), Grand Cross start / charge / end StartFuture `32cf4ec0` and charge EventProcess `32cf4f60`. March has no key reader there: port June's calls whole.

## Amended by CH2c review (2026-11-05)

- State 204 `ESSI_A_ELK_ROLLING_SMASH_LANDING` (21205 / 21227): its `ch0_skill_listing_rows.tsv` row has no callbacks because the extractor lost the register name (as state 262 in CH2c, which froze the character). June sets only its EventProcess, `329cbfc0` = a jump to `31fcc740` = `CX2GUUser::GenericSpecialActiveSkillEventProcess` (`32d223e0` @32d232a1). Register it.
