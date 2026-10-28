# June character Eve (Stage 7) - Phase CH3a2 review - Eve summon setup: June's `SetUserSummonedNPCInfo` cases

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH3a2 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH3a2 done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (no debug flag in CH3a2) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL summon npc`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH3a2 - Done (2026-10-28, Sonnet 5.5)"
```
Then the `3230c1a0 ... (CH3a2)` rows at the end of `june2017_re/funcmap.tsv`.

## Task

Review the CH3a2 port of June's `3230c1a0` Eve cases and `322ece70` against the raw listings (`curl .../disassemble_function?address=0x3230c1a0` and `0x322ece70`). Where it could be wrong:
- **Jump tables.** The port reads 411 / 520 (0x19b / 0x208) and 2091 (0x82b) as the switch default (flags 0). Confirm from the byte table at `3230ced4` / dword table `3230cec0` (index = id - 0x180, 0x87 entries) and the second switch around `3230cbb8` that these ids really reach `caseD_181` and not a listed case; also 0x655 / 0x656 reach `caseD_180` (flags 0x240) and 0x5ae is its own case (0x260).
- **`322ece70` bit paths used:** 0x20 (skips the `+0x4064` attack multiply, inside the templet-found block), 0x40 (the PvE ids 0x14 / 7 / 8 / 9 / 10, the PvP ids 0xab / 0xa1 / 0xa2 / 0xa3 / 0xa4, chosen by game vslot 0x10c in {1, 4}; a found ability replaces the factor, JUNE_BUGS 454), 0x200 (`31e8d610` factor instead of `+0x8ce0`, `+0x8800` = `3201ad10( skill ) + 1.0`), `+0x8e74` = level. `CX2GUEve::June_SetSummonHelperInfo` also has the flag-less path for flags 0.
- **Ferdinand:** `31e57960( user )` on the NPC is `SetNearestGUUser`; March's `SaveFerdinandNPCUID` is kept for CH3d (June's version of that store is not in this function).
- **Skill ids:** the member swaps with `GetUpgradeSwappedSkillID` (`31fcf430`) on the map ids of `summon_npc_skill_map.tsv` (25000, 25013, 25014, 25015, 25020, 25601).
- **Equal-code claim:** `June_SetSummonHelperInfo` with 0x40 must give the same stats as `June_SetAtomicShieldSummonInfo` (same body; Atomic Shield was left as CH3a review wrote it).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <first CH3a2 commit>^..HEAD`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH3a2 - Review" into the plan doc, update the STATUS.md row, check the CH3b kickoff, and commit.
