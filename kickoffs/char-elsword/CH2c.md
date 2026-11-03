# June character Elsword (Stage 7) - Phase CH2c - Skills: Magic Knight (11)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH2c` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH2b review (and any Opus `CH2c0` helper phase CH0 wrote). One at a time: `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-elsword/CH2c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Sword Man and the first jobs (classes 1, 10, 11, 22)" "Phase CH3 - Skills: second jobs (100, 101, 112), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
```
Then CH0's and CH1's Done notes, and the `CH2c` rows of `june2017_re/char_elsword/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the ids class 11 adds, incl. his type-2 row. For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, the numerical-only tables, alpha-tag sites in its functions); its effect closure (a name found only now is staged byte-exact
and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Elsword: CH2c)`.
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

## Amended by CH0 (2026-11-03)

- **Functions: 18** (`awk -F'\t' '$7=="CH2c"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 17, new 1).
  June addresses: `32cf3b90` `32cf3bd0`(new ESSI_SI1_ES_WIND_BLADE_StateStartFuture) `32cf3c00` `32cf3c70` `32cf3f30` `32cf45c0` `32cf4710` `32cfa040` `32cfa080` `32cfa230` `32cfa270` `32cfa320` `32cfae00` `32cfef60` `32cffcf0` `32cffd60` `32d086f0` `32d123f0`.
- **States: 7** (`awk -F'\t' '$8=="CH2c"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_SA_EMK_BIG_BURST`, `ESSI_SI2_ES_RISING_SLASH`, `ESSI_SI1_ES_WIND_BLADE`, `ESSI_SI1_ES_WIND_BLADE_ULTIMATE`, `ESSI_A_ES_SWORD_WAVE`, `ESSI_SI_A_ERS_CRITICAL_SWORD`, `ESSI_SI3_ES_STORM_BLADE`.
- **Skills: 12** (part 2 of `ch0_state_skill.tsv`, col 6): 21012 RNSI_SA_EMK_RISING_SLASH, 21013 RNSI_SA_EMK_WIND_BLADE, 21014 RNSI_SA_EMK_STORM_BLADE, 21032 RNSI_SA_EMK_RISING_SLASH_UPGRADE, 21035 RNSI_SA_EMK_WIND_BLADE_UPGRADE, 21048 RNSI_SA_EMK_STORM_BLADE_UPGRADE, 21207 RNSI_A_EMK_BIG_BURST, 21208 RNSI_A_EMK_SWORD_WAVE, 21209 RNSI_A_EMK_CRITICAL_SWORD, 21220 RNSI_A_EMK_BIG_BURST_UPGRADE, 21229 RNSI_A_EMK_SWORD_WAVE_UPGRADE, 21230 RNSI_A_EMK_CRITICAL_SWORD_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SA_EMK_BIG_BURST (scrip; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 20 tables ab; §1 L37-105 blocks that index past their skill data need a JUNE_BUGS row; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names) fail silently; §7 L254 class code scales the cached state power rate (+0x7cc8); §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L713-721 an id of a listed type with no case plays the class's first skill stat; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.
- `c1a` zero row: **21013 `RNSI_SA_EMK_WIND_BLADE`** (l.1274, `DamageEffect_SA_EMK_WindBlade_Ultimate_Projectile` INDEX 2): verbatim + `JUNE_BUGS.md` row.
