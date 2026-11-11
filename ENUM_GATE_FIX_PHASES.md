# Enum gate fixes for the ported classes - phases

## Context

char-ara CH1 (2026-11-11) shipped Ara's June class script while the merged data036 `Enum.lua` (core-list BOM source) lacked June members
it reads: the file died at load and her `CHAIN_SYSTEM` popped `StateID`. The fix added an ENUM gate and a STATE gate to
`tools/luabind.py check` (tool docstring). Run over the June class scripts already ported, the gate lists five silent nils in four classes
(`JUNE_CHARACTER_ARA_PHASES.md`, "CH1 - Done", *Found in the ported classes*):

| Class | Script line (june_src_fixed) | Reference | Read by | June value | C++ |
|---|---|---|---|---|---|
| Elsword | `Elsword_SwordMan.src.lua:16950` | `SKILL_ID.RNSI_A_ERS_ENCHANT_BREAK` | `APPLIED_SKILL_ID` | 21223 | `X2SkillTree.h:2584`, same value |
| Elsword | `Elsword_SwordMan.src.lua:5185` | `SKILL_ID.RNSI_P_ELK_FIGHT_GOD` | `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_SKILL_ID` | 21416 | `X2SkillTree.h:2628`, same value |
| Raven | `RAVEN_FIGHTER.src.lua:15522` | `SKILL_ID.RNSI_A_RBM_POINTED_STILETTO` | `APPLIED_SKILL_ID` | 24222 | `X2SkillTree.h:2765`, same value |
| Chung | `Chung_Iron_Cannon.src.lua:619` | `CHUNG_STATE_ID.CSI_CFG_COMBO_XXFRONTZ` | `INIT_PERSONAL_INFO` list (l.563) | `USI_END + 171` | `X2GUChung.h:284`, `USI_END + 168` (probe) |
| Aisha | `Arme_VioletMage.src.lua:4293, 4364, 4408, 4457` | `DAMAGE_TRIGGER_TYPE.DTT_AISHA_STICK_ATTACK` | `ATTACK_TRIGGER_TYPE` | 17 | **no member** (`X2DamageManager.h` DTT enum; 17 is free) |

## Rules

CLAUDE.md and the standing rules hold: every client change behind a `SERV_IRUHADEV_` flag in `KTDXLIB/Always.h`, June's values verbatim,
no hardcoded enum literals, June bugs kept with a `JUNE_BUGS.md` row, one-line comments. `Enum.lua` is edited as BOM + CRLF text (keep both),
one-line `-- enum-gate-fix F1` comments, then `python tools/luasrc.py refresh data036 Enum.lua`; `core --check` / `ledger --check` PASS.
A Lua state-id value must equal the C++ enum, never June's number when the two differ. Pack and copy yourself (CLAUDE.md, 2026-11-10).

## Starting context

- The gate: `python tools/luabind.py check Elsword_SwordMan RAVEN_FIGHTER Chung_Iron_Cannon Arme_VioletMage` (~15 s with the STATE probe).
- Staged file: `<kom_contents>\data036_kom\Enum.lua` (`tools/luasrc.py` / `kom.py` `KOM_ROOT`). Pack `kom.py pack data036`, copy `kom.py copy data036`.
- C++ values: `june2017_re/enumprobe.py` (items file, one constant expression per line, run from `june2017_re/`).
- Build, flag hygiene, logs: the x2-phase skill's tools.

## Phase F1 - Fill the five gaps

**Run with:** Opus 5.5 high (the Aisha trigger needs June's reader read in Ghidra), user play-test.
**Read list:** this doc; `JUNE_CHARACTER_ARA_PHASES.md` "CH1 - Done (2026-11-11)" (the *Play-test fix* and *Guard* bullets only);
`python tools/luabind.py --help`.

1. **Elsword / Raven (data):** add the three `SKILL_ID` members to `Enum.lua` with June's values (C++ already agrees).
2. **Chung (data):** add `CHUNG_STATE_ID.CSI_CFG_COMBO_XXFRONTZ = USER_STATE_ID.USI_END + 168` (the C++ value). Confirm the STATE gate then
   compares it equal; check what reads `INIT_PERSONAL_INFO` in March (`X2GUChung.cpp`) to say what the fix changes in game.
3. **Aisha (C++ + data):** find June's reader of `DTT_AISHA_STICK_ATTACK` (17): the damage-trigger switch / Aisha's `AttackResultByType` or
   damage-react override in June's Arme class (`char-arme` funcmap rows; `grep "Arme" june2017_re/funcmap.tsv`). Then:
   - register `DTT_AISHA_STICK_ATTACK = 17` in the C++ `DAMAGE_TRIGGER_TYPE` enum the way its June neighbours are (`X2DamageManager.h:57-125`,
     explicit value, one-line comment) under a new flag `SERV_IRUHADEV_ENUM_GATE_AISHA_TRIGGER`, and the same member in `Enum.lua`;
   - if June's reader is already ported (the case exists in March's switch), done; if not, port it whole under the same flag when it is
     one function's case, otherwise route it under the scope rule (a bullet in the char-arme follow-ups / Stage 7 backlog) and say so.
4. **Gate:** `luabind.py check` over the four scripts = `RESULT: PASS` (`enum nil in June` lines stay). Pack + copy data036.

**Done gate:** the gate PASS line; the C++ probe value of `DTT_AISHA_STICK_ATTACK` = 17; `flagcheck` PASS; build + deploy only if C++
changed (play-test). **Play-test** (dev console first): Elsword Enchant Break / Fight God passive, Raven Pointed Stiletto, Chung (Fury
Guardian) X X front Z, Aisha stick-attack combo hits; ErrorLog shows no new `[LUA_SRC]` error. No review of its own unless step 3 ports a
reader (then `F1-review.md`, Opus).

### Phase F1 - Done (2026-11-11)

**Landed.** Flag `SERV_IRUHADEV_ENUM_GATE_AISHA_TRIGGER` (`KTDXLIB/Always.h`) and five data036 `Enum.lua` members.
- **Elsword / Raven (data):** `SKILL_ID.RNSI_A_ERS_ENCHANT_BREAK = 21223`, `RNSI_P_ELK_FIGHT_GOD = 21416`, `RNSI_A_RBM_POINTED_STILETTO = 24222`
  (June `Enum.src.lua:14971 / 15672 / 15991`; C++ `X2SkillTree.h` same values). What they change: Enchant Break's and Pointed Stiletto's
  damage data carry `NUMERICAL_DATA_INDEX = 1`, which `ApplyNumericalSkillData` (`X2DamageManager.cpp:72`) only applies when
  `APPLIED_SKILL_ID` is set, so both skills now take their skill-level numbers. Armor Break's (`ESSI_A_ESK_ARMOR_BREAK`)
  `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID` (`BFI_DEBUFF_ARMOR_BREAK_OF_FIGHT_GOD`) was skipped while its relative skill id read nil
  (`X2DamageManager.cpp:5178`); it now applies when Fight God is learned and the attacker has `BTI_SI_A_ES_ENDURANCE`.
- **Chung (data):** `CHUNG_STATE_ID.CSI_CFG_COMBO_XXFRONTZ = USER_STATE_ID.USI_END + 168` (C++ value, probe; June's file says + 171).
  Reader: `CHUNG_STATE_ID_LIST_ABLE_TO_BE_CHANGED_TO_GUARD_STATE` (`INIT_PERSONAL_INFO`, `LoadStateIDCanBeChangedGuardState`
  `X2GUChung.cpp:11135`): the `{ nil, 0, 0.9 }` row was dropped (state CSI_BASE); Fury Guardian can now guard out of X X front Z in 0-0.9 s.
  June's `CSI_CFG_COMBO_XXFRONTZZ` (June + 172; C++ member `X2GUChung.h:288`) is not read by the script and was not added.
- **Aisha (C++ + data):** `DTT_AISHA_STICK_ATTACK = 17` in `CX2DamageManager::DAMAGE_TRIGGER_TYPE` (`X2DamageManager.h`, after the CH1h2
  block) and in `Enum.lua`. June's reader, read in Ghidra: `32ce9f90` (Open Mind 22416 attack-result delegate) `@FUN_32610fb0( 0x11 )` =
  `IsAttackTrigger( 17 )`. **Already ported** (char-arme CH4a, `OpenMindAttackResult`), with a bare `17`: that literal is now the member
  under the flag, `#else` keeps it. No reader to port, so no F1 review. Script states carrying it: `AVSI_COMBO_Z1 / Z2 / Z3 / Z3up`.
- C++ probe (`june2017_re/enumprobe.py`): `DTT_AISHA_STICK_ATTACK = 17`, `CSI_CFG_COMBO_XXFRONTZ - USI_END = 168`, `DTT_END = 6` (unchanged).

**Gates.**
- `luabind.py check Elsword_SwordMan RAVEN_FIGHTER Chung_Iron_Cannon Arme_VioletMage`: ENUM 0 missing in all four; `STATE: 27 state-id
  reference(s) compared with the C++ enum, 0 wrong` (26 before: XXFRONTZ now compared); `RESULT: PASS`. `enum nil in June` lines stay
  (`HIT_TYPE.HT_METEL_PUNCH_HIT`, `HT_SLASH`, `SKILL_ID.RNSI_SA_CC_DETONATION`).
- `Enum.lua`: BOM kept, 13,471 CRLF / 0 bare LF; `luasrc.py refresh data036 Enum.lua` REFRESHED; `CORE CHECK PASS`; `LEDGER CHECK PASS`.
- `PACK OK data036` (270 entries, ~1 changed: Enum.lua); `COPY OK data036 ... md5 afc0f04d2a18ec851118dbe6502dcc3c`,
  backup `kom_backup_tools/data036.kom.20261111_130625`.
- `flagcheck SERV_IRUHADEV_ENUM_GATE_AISHA_TRIGGER --worktree`: 1 PASS, 1 PASS~ (marker lines), 1 DEFINE, 0 OTHER, 0 FAIL, 0 deleted,
  0 non-ASCII, 0 EOL mismatch; RESULT: PASS.
- `junediverge.py --check` (3 files): RESULT: PASS. `enumscan.py --check`: 0 A/B rows.
- `DEPLOY OK: X2_offline.exe is 26,225,152 B, dated 2026-11-11 13:17:37, md5 5d3ea84f8c8496830324e63150e58685, equal to X2/US_SERVICE/x2.exe (2026-11-11 13:17:30); built at main 85ea22bd + 3 uncommitted code file(s).`
- `EXESTR OK: 1 of 1 found in X2_offline.exe, 26225152 B, dated 2026-11-11 13:17:37, md5 5d3ea84f8c8496830324e63150e58685` (`ARME SKILL open mind`, wide).

**Save (play-test set-up, backup `db_backup/els_db_20261111_130831_pre_sql.sql`).** Unit 26 jtRuneSlayer: 21223 Enchant Break in slot 0
(21231 unslotted). Unit 25 jtLordKnight: learned 21201 Endurance (slot 0) and 21204 Armor Break (slot 1), 21043 / 21218 unslotted.
Unit 49 Raven99: 24222 Pointed Stiletto in slot 0 (24025 unslotted). Unit 53 ArmeDW (transcended, Open Mind 22416 lv 1) and
unit 63 ChungFury unchanged.

**How to see it in game.** Enchant Break / Pointed Stiletto hit with their level numbers; Armor Break under Endurance adds the Fight God
debuff icon on the target; ChungFury X X front Z can be cancelled into guard; ArmeDW's Z combo logs
`[JUNE_CHAR] ARME SKILL open mind ( unit 53: level 1, buff factor to ... )` per hit. No new `[LUA_SRC]` line in ErrorLog.

**Not ported / routed.** Nothing: all five gaps closed in this phase. MODS.md section added.

**Play-test PASSED (user, 2026-11-11; logs 13:20-13:26 on md5 `5d3ea84f`, 0 EXCEPTION, known `E_DISABLE_HB_CHECK_REQ` UNHANDLED, `AddGroupChainQuest` = quests, no new `[LUA_SRC]` line).**
- Aisha: 16 `[JUNE_CHAR] ARME SKILL open mind ( unit 53: level 1, buff factor to ... )` lines from ArmeDW's Z combo, several targets.
- Raven: Rend (= `RNSI_A_RBM_POINTED_STILETTO`; the hand-off used June's name) cast 4 times, damage normal (user).
- Chung: ChungFury blocks when hit during X X front Z (user): the guard-on-hit list now holds the state.
- Elsword: no buff icon is expected on a monster (`CX2GageManager::PushBuff` draws only my / party / PvP-team gauges). Enchant Breaker's
  `BTI_DEBUFF_ERS_ELEMENTA_BRAKE` has no displayer; God of War's `BTI_DEBUFF_ARMOR_BREAK_OF_FIGHT_GOD` shows `EffectSet_ARMOR_BREAK`, the same
  set as plain Armor Break's debuff, logged on monster 1000016 at 13:23:00 one second after Enhanced Iron Body (21218). The user declined a
  debug line: both checked in code only. The hand-off's base Iron Body (21201) row on unit 25 was redundant (21218 learned) and was removed.
- Side note: `ARMOR_BREAK_IMPACT01.XET` fails to open on Armor Break; June has no such file (`stage-june` dry run: not in any June kom).
