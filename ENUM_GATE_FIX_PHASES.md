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
