# June character Eve (Stage 7) - Phase CH2b3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2b3 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2b3 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_NPC_LUA_SUMMON_RATE` |
| Log tag | `[JUNE_CHAR]` (`NPC POWER`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2b3 - Done (2026-10-27, Opus 5.5)"
```

Rows: `june2017_re/funcmap.tsv` (grep `CH2b3`).

## Task

Review CH2b3 against June (`python june2017_re/disasm_range.py <va> <end>` for the raw listings):
- `31e8d610` / `31e8d6a0`: the compare-then-store against `SetMultiplyPowerRate_LUA` / `ResetMultiplyPowerRate_LUA` (`X2GUNPC.cpp`). Then `32759630` against `GetPowerRate`, including the float rounding (`fstp` / `fld dword`).
- `32763c10` line by line against `ApplyPassiveSkillOwnerUser_LUA`:
  - the owner gate (`+0x8d5c`, `- 0x1a0`);
  - the `NPCUnitStat` zero / 9999 init and the struct offsets `+8` / `+0xc` / `+0x5c` = `m_fAtkPhysic` / `m_fAtkMagic` / `m_fPercentCritical`;
  - the `0 < +0x4064` test (`test ah,5`; NaN skips) and the `min( crit, 1.0 )` compare;
  - the three `3236ca50` targets / sources = `CopyJuneMaximizeAndRangeFrom`.
- `32758d50` / `32763950` = March's `GetNPCStat` / `SetNPCStat( const NPCUnitStat& )` (funcmap 1432).
- The ctor values of `+0x4068` / `+0x406c` (`322a2700` @`322a4323`-`322a435d`), and that `SkillRelatedData::Init` runs only from the ctor.
- The two store moves in `X2Game.cpp` (Nasod Watch 0x240, Ferdinand 0x260) against `322ece70` @`322ed38c`-`322ed3ae`: product in the factor, `+0x8ce0` untouched. Check that no other March case with June flag 0x200 still stores into the default rate.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_NPC_LUA_SUMMON_RATE --range <first phase commit>^..<commit>`, then `git log -S SERV_IRUHADEV_JUNE_NPC_LUA_SUMMON_RATE <commit>..HEAD`. Paste the summary lines. Explain every OTHER row and every deleted line.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH2b3 - Review" into the plan doc, update the STATUS row, check the next kickoff (`CH2b4.md`), and commit.
