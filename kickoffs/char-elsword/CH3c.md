# June character Elsword (Stage 7) - Phase CH3c - Skills: Infinity Sword (112)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-elsword CH3c` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH3b review (and any Opus `CH3c0` helper phase CH0 wrote). One at a time: `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (+ `..._SKILL_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |
| Review | `kickoffs/char-elsword/CH3c-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH2 - Skills: Sword Man and the first jobs (classes 1, 10, 11, 22)" "Phase CH3 - Skills: second jobs (100, 101, 112), hyper actives, transcendence"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "1. Blocker: June damage numbers come from a table this build does not have"
```
Then CH0's and CH1's Done notes, and the `CH3c` rows of `june2017_re/char_elsword/ch0_state_skill.tsv` / `ch0_checklist_rows.tsv`.
If a checklist heading differs, `--list` and pull it by its current name.

## Task

Scope: the ids class 112 adds (`PHANTOM_BLADE_*` if CH0 put it here). For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole (Ghidra, the map's
addresses), inside March's case under `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_SKILL` (March's in `#else`) or a new case; its attached rows (7u names, 7ai BFI arguments, 7h
UPDOWN cancels, the numerical-only tables, alpha-tag sites in its functions); its effect closure (a name found only now is staged byte-exact
and added to `ch0_assets.tsv`). Upgrade ids reach their base through the templet's `m_iDeleteSkillIDByUpgrade` (June `32071720`): check the
templet swap for every term that reads the id (the Aisha CH3b review defect). A function the map does not settle (new mechanism, unmapped
callee) is "this needs Opus": stop.
- `NUM use` lines for these skills; March vs June damage per skill in `MODS.md`.
- CP949: `bytepatch.py` only. Mark the ported checklist rows `(Elsword: CH3c)`.
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

- **Functions: 26** (`awk -F'\t' '$7=="CH3c"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 23, new 3).
  June addresses: `32cf4210` `32cf4230` `32cf4240` `32cf42a0` `32cf42f0` `32cf4450` `32cf44b0`(new ESSI_SI_SA_EIS_SWORD_BLASTING_StateEnd) `32cf5000`(new ESSI_SA_EIS_INFINITY_STRIKE_Init) `32cfd910` `32cfd990` `32cfdb30` `32cfdd60` `32cfddc0` `32cfe000` `32cfea40` `32cfea80` `32d06c50`(new ESSI_SA_EIS_INFINITY_STRIKE_EventProcess) `32d089b0` `32d13940` `32d13be0` `32d13cd0` `32d13e20` `32d170e0` `32d177a0` `32d1ecc0` `32d1f470`.
- **States: 12** (`awk -F'\t' '$8=="CH3c"' june2017_re/char_elsword/ch0_state_skill.tsv`): `ESSI_SI_SA_ETK_HARSH_CHASER`, `ESSI_SI_SA_ETK_HARSH_CHASER_PULL`, `ESSI_SI_SA_ETK_PHANTOM_SWORD`, `ESSI_SI_SA_EIS_RAGE_CUTTER`, `ESSI_SI_SA_ETK_MAELSTORM_RAGE`, `ESSI_SI_A_ETK_COMPACT_COUNTER_ATTACK`, `ESSI_SI_SA_EIS_SWORD_BLASTING`, `ESSI_A_EIS_ANOTHER_SWORD`, `ESSI_SA_EIS_INFINITY_CHASER`, `ESSI_SA_EIS_BLADE_CIRCLE`, `ESSI_SA_EIS_INFINITY_STRIKE`, `ESSI_SI_SA_EIS_SWORD_BLASTING_UPGRADE`.
- **Skills: 16** (part 2 of `ch0_state_skill.tsv`, col 6): 21022 RNSI_SA_ETK_PHANTOM_SWORD, 21023 RNSI_SA_EIS_RAGE_CUTTER, 21024 RNSI_SA_EIS_MAELSTROM_RAGE, 21025 RNSI_SA_EIS_SWORD_BLASTING, 21041 RNSI_SA_EIS_INFINITY_CHASER, 21042 RNSI_SA_EIS_BLADE_CIRCLE, 21054 RNSI_SA_ETK_PHANTOM_SWORD_UPGRADE, 21055 RNSI_SA_EIS_RAGE_CUTTER_UPGRADE, 21056 RNSI_SA_EIS_MAELSTROM_RAGE_UPGRADE, 21057 RNSI_SA_EIS_SWORD_BLASTING_UPGRADE, 21060 RNSI_SA_EIS_INFINITY_STRIKE, 21215 RNSI_A_EIS_HARSH_CHASER, 21216 RNSI_A_EIS_COMPACT_COUNTER, 21224 RNSI_A_EIS_ANOTHER_SWORD, 21235 RNSI_A_EIS_HARSH_CHASER_UPGRADE, 21236 RNSI_A_EIS_COMPACT_COUNTER_UPGRADE.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SI_A_ETK_COMPACT_COUNTE; §1 L51 numerical-only DAMAGE_DATA_NEXT table in state ESSI_SI_A_ETK_COMPACT_C; §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SI_SA_EIS_SWORD_BLASTIN; §1 L51 numerical-only DAMAGE_DATA table in state ESSI_A_EIS_ANOTHER_SWORD (sc; §1 L51 numerical-only DAMAGE_DATA_NEXT table in state ESSI_A_EIS_ANOTHER_SWOR; §1 L51 numerical-only DAMAGE_DATA table in state ESSI_SI_SA_EIS_SWORD_BLASTIN; §1 L37-105 numerical-only DAMAGE_DATA + numerical override live (the 20 tables ab; §2 L116 APPLIED_SKILL_ID (parsed since 8c); §2 L117 NUMERICAL_DATA_INDEX; §4 L178 June-only members of tables March has (SKILL_ID names) fail silently; §7 L254 class code scales the cached state power rate (+0x7cc8); §7c L352 29 alpha-tag sites 32d0d660 .. 32d1ecc0; §7h L420 Elsword 25 functions 32cf8230..32d06c50 (e.g. 32d01040): UPDOWN_CANCEL; §7i L436-437 class code multiplies m_fNowStatePowerRate, never the entry's; §7ae L713-721 an id of a listed type with no case plays the class's first skill stat; §7ae L729-733 types 14-16 / 17 / 18 named in the class switch; ResetLeftSkillCoolTim.

## Amended by CH3b (2026-11-05)

- **Read the registration `32d223e0` raw for every state**, not only the listing rows: the tool loses register names and an empty hyper row means `329cc2c0` copied every delegate (Init included).
- **Ground-only cancels already have macros** in `X2GUElsword_SwordMan.cpp`: `ELSWORD_GROUND_BWALK_CANCEL`, `_WALK_CANCEL`, `_DASH_CANCEL`, `_ATTACK_CANCEL` (Z / X), with `SKILL_CANCEL_AFTER` and the up-down branch written out as in `ESSI_ERS_COUNTER_GUARD_END_EventProcess`. Compare two EventProcess listings with an address-stripped diff before writing both (CH3b: `32d05b80` = `32d068d0`).
- **`m_fSkillKeyHoldTime`** (June `+0x96f4`) is the shared hold-time member; a held-key state sets `m_kSkillSlotKey` in StartFuture and reads `FollowSlotSwap` / `IsKeyReleased` in EventProcess.
