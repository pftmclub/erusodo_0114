# June character Chung (Stage 7) - Phase CH3b0 - Chung's summoned NPCs

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH3b0` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1 review (placed by CH0 before CH2c, the first skill phase that spawns a summon). One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SUMMON_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`) |
| Review | `kickoffs/char-chung/CH3b0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Context" "Phase CH3 - Skills: second jobs (110, 111, 117), hyper actives, transcendence"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Phase CH2b2 - How June spawns a summon (added 2026-10-27, Opus)" "CH2b2 - Done (2026-10-27, Opus 5.5)" "CH2b3 - Done (2026-10-27, Opus 5.5)" "CH2b4 - Done (2026-10-27, Sonnet 5.5)"
```
Then CH0's summon map (item 7) and `june_src_fixed/data037_kom/Targeting_Chung.src.lua`.

## Task

From CH0 item 7: how June's class spawns and drives its NPCs: the targeting NPC (`NUI_TARGETING_CHUNG`, `NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER`,
used by Artillery Strike and its relatives) and `NUI_REMOTE_MORTAR`. For each: the spawn call and its rule (one per owner or many), the owner
link, the NPC's own script (`Targeting_Chung.lua` adopted as June's bytecode, or confirmed already June's in the ledger), `luabind.py check` PASS
(an unbound method is bound whole from June or listed in `tools/data/lua_unbound_known.tsv` with the reason), and any summon buff or death path
June has (Eve's CH2b4). If CH0 found more than one spawn rule, split (`CH3b02`) with a kickoff and STATUS row. The skills that use them stay with
their skill phase; this phase makes the NPC behave as June's when spawned.

You are on Opus: delegate reads and known-address dumps. CP949 through `bytepatch.py`. Done gate: the play-test spawns each NPC from a March
skill (or the dev console) and the `ES_CREATE` / state lines the hand-off lists appear.

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

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7 L252 IsSummonSkill (54 June skill ids) and SetUserSummonedNPCInfo summon ca; §7 L277 3230c1a0 SetUserSummonedNPCInfo / 322ece70 per-case flags.
- **Placed before CH2c** (Depends: CH1 review; CH2c waits for this review). Spawn sites (`ch0_summons.tsv`): `32bfea4d` (via `31f8b010` CreateNPCReq forwarder, 863 `NUI_TARGETING_CHUNG`) and `32bfe9ae` (via `322e67c0`, 1419 `_NOT_SHOW_OTHER_PLAYER`) in `32bfe730`; `32c0b97d` (via `322e67c0`, 2525 `NUI_TARGETING_CHUNG_HEAVY_FIELD`) in `32c0b7f0` Heavy Field READY; `32c10f99` (1100 `NUI_REMOTE_MORTAR`) in `32c10d20`, called from Remote Mortar FrameMove `32c12470`. One spawn rule, two entry points: map both.
- **2525 is June-only:** its NPCTemplet row (June `NPCTemplet.src.lua:21708`, LUA_FILE_NAME `Targeting_Chung.lua`), the `NPC_UNIT_ID` member in the core `Enum.lua` and the C++ enum (`X2UnitManager.h`, under `JUNE_ENUM_NAMES`) are this phase's.
- March sites: CreateNPCReq at X2GUChung.cpp:17202 / 17205 / 22611; self-destruct uses of 863 / 1419 at 286, 375, 5605, 7187, 7235, 17176 (both arms of `BALANCE_PATCH_20131107`: `cl /P`), 23731; X2Game.cpp:9488 / 18190 / 18238 / 18490; X2GUUser.cpp:26427.
