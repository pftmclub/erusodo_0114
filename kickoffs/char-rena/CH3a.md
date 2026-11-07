# June character Rena (Stage 7) - Phase CH3a - Skills: Wind Sneaker (102)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-rena CH3a` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2d review. One at a time: `X2GULire_ElvenRanger.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-rena/CH3a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH2 - Skills: Rena and the first jobs (classes 3, 12, 13, 24)" "Phase CH3 - Skills: second jobs (102, 103, 114), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH3a - Done" "CH3a - Review"
```
Then CH0's and CH1's Done notes, and the `CH3a` rows of `june2017_re/char_rena/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the class-102 skill ids classes 3 / 12 / 13 / 24 lack. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI
arguments, 7h UPDOWN cancels, §7 l.256 power-rate operator, the numerical-only tables of `plan_numerical_rows.tsv`); its effect closure (a name found
only now is staged byte-exact and added to `ch0_assets.tsv`); its natural-force gain / cost lines under `SERV_IRUHADEV_JUNE_CHAR_RENA_NF`. Upgrade ids
reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the templet swap for every term that reads the id (the
Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Rena: CH3a)`.
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

- **Functions: 44** (`awk -F'\t' '$7=="CH3a"' june2017_re/char_rena/ch0_class_map.tsv`; inside 35, new 9).
  June addresses: `32c6bca0` `32c6bcf0` `32c6bd00` `32c6bdf0`(new LESI_SA_LCR_SHARPFALL_StateStart) `32c6be20` `32c6bfb0` `32c6c060` `32c6c070` `32c6c080` `32c6c0c0` `32c6c0d0` `32c6c0e0` `32c6c100` `32c6c110` `32c6c150` `32c6c1e0` `32c6c2e0` `32c6c350` `32c6c410` `32c6d020`(new LESI_SA_RWS_SPINNING_CRESCENT_Init) `32c6d030`(new LESI_SA_RWS_GYRO_KICK_Init) `32c6d080`(new LESI_A_RWS_PAIN_Init) `32c6d880`(new LESI_SA_RWS_TEMPEST_Init) `32c769f0` `32c76ce0` `32c76d10` `32c76d50` `32c76d90` `32c77170` `32c77240` `32c77270` `32c772a0` `32c79060` `32c79280` `32c7d790`(new LESI_SA_RWS_SPINNING_CRESCENT_EventProcess) `32c7daa0`(new LESI_SA_RWS_GYRO_KICK_EventProcess) `32c7e9f0`(new LESI_A_RWS_PAIN_EventProcess) `32c86f70` `32c86fa0` `32c86fd0` `32c87ec0`(new LESI_SA_LCR_SHARPFALL_LANDING_StateStart) `32c96560` `32c96910` `32c96aa0`.
- **States: 18** (`awk -F'\t' '$8=="CH3a"' june2017_re/char_rena/ch0_state_skill.tsv`): `LESI_SI_A_LE_REFLEX_MAGIC_READY`, `LESI_SI_A_LE_REFLEX_MAGIC`, `LESI_SA_LCR_SHARPFALL`, `LESI_SA_LCR_SHARPFALL_LANDING`, `LESI_SA_LCR_SHARPFALL_HYPER_LANDING`, `LESI_SA_LCR_VIOLENT_ATTACK_READY`, `LESI_SA_LCR_VIOLENT_ATTACK`, `LESI_SA_LCR_VIOLENT_ATTACK_LANDING`, `LESI_SA_LWS_SLIDE_KICK`, `LESI_SA_LWS_SLIDE_KICK_PASS_THROUGH`, `LESI_SA_LWS_SLIDE_KICK_FINISH`, `LESI_SA_LWS_SLIDE_KICK_CANCEL`, `LESI_A_LWS_NATURE_FORCE`, `LESI_SA_LWS_AIRELINNA`, `LESI_A_RWS_PAIN`, `LESI_SA_RWS_SPINNING_CRESCENT`, `LESI_SA_RWS_GYRO_KICK`, `LESI_SA_RWS_TEMPEST`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 23008 RNSI_SA_RWS_VIOLENT_ATTACK, 23009 RNSI_SA_RWS_SHARPFALL, 23010 RNSI_SA_RWS_AIRELINNA, 23011 RNSI_SA_RWS_SLIDE_KICK, 23037 RNSI_SA_RWS_SPINNING_CRESCENT, 23038 RNSI_SA_RWS_GYRO_KICK, 23044 RNSI_SA_RWS_VIOLENT_ATTACK_UPGRADE, 23045 RNSI_SA_RWS_SHARPFALL_UPGRADE, 23046 RNSI_SA_RWS_AIRELINNA_UPGRADE, 23047 RNSI_SA_RWS_SLIDE_KICK_UPGRADE, 23058 RNSI_SA_RWS_TEMPEST, 23205 RNSI_A_RWS_NATURE_FORCE, 23207 RNSI_A_RWS_REFLEX_MAGIC, 23225 RNSI_A_RWS_PAIN, 23230 RNSI_A_RWS_NATURE_FORCE_UPGRADE, 23232 RNSI_A_RWS_REFLEX_MAGIC_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L56 numerical-only DAMAGE_DATA table in state LESI_SA_LCR_SHARPFALL (scrip; §1 L56 numerical-only DAMAGE_DATA + numerical override live (the 12 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names, LIRE_ELVENRANGE; §7 L256 class code scales the cached state power rate (+0x7cc8); §7i L431-L445 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L719 an id of a listed type with no case plays the class's first state (Ren; §7ae L726 types 14-16 / 17 / 18 named in the class switch; §7ai L848 pass June's skip / adjust / level / creator at every ported BFI call.
- **Natural-force / combo-revamp rows** (`ch0_nf.tsv` col 8 = `CH3a`): 1; March blocks under `ADD_RENA_SYSTEM` inside your functions stay dead, June's NF lines go under `_NF`.
