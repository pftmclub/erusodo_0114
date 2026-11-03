# June character Elsword (Stage 7) - Phase CH4a - Passives

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH4a` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH2d review, and no CH3 phase running (same files). One at a time: `X2GUElsword_SwordMan.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD PASSIVE`) |
| Review | `kickoffs/char-elsword/CH4a-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH4a - Done (2026-11-03)"
```
Then the Elsword lines of the checklist rows named below (`june2017_re/char_elsword/plan_checklist_rows.md` gives the line numbers), the
`CH4a` rows of `ch0_*.tsv`, and Chung's matching CH4 Done note as the worked example.

## Task

`InitPassiveSkillState` `32d1c560` (vslot 0x690) and Elsword's passive ids (types 1 / 8: 3 + 25 rows), their objects and readers; checklist
7am's 8 delegate callbacks registered in `32d1c560`; the three `CreateAndInsertSkillAndSocketBuffFactorToList` calls (`322a25f0`: Spell Chain
21406's bool = Magic Coating 21418 learned, the other two false). The engine hooks they need (Aisha's / Eve's / Chung's CH4 ported the delegate
lists, the user passive hooks and June's base `InitPassiveSkillState` `31fe19d0`): reuse them.

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps
(x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`. Mark rows `(Elsword: CH4a)`. Done gate: each item's effect visible in
game or in a named log line.

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

- **Functions: 18** (`awk -F'\t' '$7=="CH4a"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 2, new 16).
  June addresses: `32d05a00`(new ?) `32d05a50`(new ?) `32d05af0`(new ?) `32d08ba0` `32d0baf0`(new ?) `32d0bc60`(new ?) `32d0c180`(new ?) `32d0c4e0`(new ?) `32d1a370`(new ?) `32d1c560` `32d26a40`(new ?) `32d270a0`(new ?) `32d27160`(new ?) `32d27170`(new ?) `32d27180`(new ?) `32d27190`(new ?) `32d272b0`(new ?) `32d27470`(new ?).
- **Skills: 22** (part 2 of `ch0_state_skill.tsv`, col 6): 106 RNSI_P_COMMON_HYPER_AURA, 21400 RNSI_P_ESK_GET_MANA_UP, 21401 RNSI_P_ESK_AUTO_GUARD, 21402 RNSI_P_ELK_WIND_SLICER, 21403 RNSI_P_ELK_STRONG_BONE, 21404 RNSI_P_ELK_REDUCE_PAIN, 21405 RNSI_P_EMK_IF_PHYSICUP_MAGIC_GET, 21406 RNSI_P_EMK_SPELL_CHAIN, 21407 RNSI_P_ERS_SWORD_ENCHANT, 21408 RNSI_P_ERS_RUNE_MASTERY, 21409 RNSI_P_ERS_MAGIC_RESISTANCE, 21410 RNSI_P_ETK_BRUTAL_SLAYER, 21411 RNSI_P_ETK_SHORTTIME_STRONGATTACK, 21412 RNSI_P_EIS_HARSH_SLAYER, 21413 RNSI_P_EIS_CUTTING_SWORD, 21414 RNSI_P_EIS_LIGHTNING_STEP, 21415 RNSI_P_ELK_OPEN_MIND, 21416 RNSI_P_ELK_FIGHT_GOD, 21417 RNSI_P_ERS_OPEN_MIND, 21418 RNSI_P_ERS_MAGIC_COATING, 21419 RNSI_P_EIS_OPEN_MIND, 21420 RNSI_P_EIS_INFINITY_POTENTIAL.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §2 L124 APPLY_LEARNED_SKILL_BUFF_FACTOR_ID and _RELATIVE_SKILL_ID x1 each (8b ; §4 L178 June-only members of tables March has (SKILL_ID names) fail silently; §7ae L713-721 an id of a listed type with no case plays the class's first skill stat; §7am L901 delegate-list registrations in InitPassiveSkillState: Elsword 32d1c560; §7am L916 March pushes replaced by June's set; §7ao L957 transcend readers: 32d17e90 hyper effect start (vslot 19, slot 0x4c); .
- `LightingStepFrameMove` `32d08ba0` (delegate registered @32d1d410): June's dash threshold is 0.5 s, March's 1.0 s. The hit-list clear `32d270a0` is here. Enum names: 4 of 127 ids are in the C++ enum by name, 28 lookalikes, 95 none; 64 in the live core `Enum.lua` (`ch0_enum_names.tsv`).
