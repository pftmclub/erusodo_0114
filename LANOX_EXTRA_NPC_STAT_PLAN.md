# Lanox ExtraNPCStat Port (June2017 `NewNPCStat` system, full June parity)

This plan fixes the Lanox dungeon **"phase clears while monsters are still alive"**
defect. It does so by porting June2017's second NPC stat system, `NewNPCStat.lua` +
`NewNPCStatFormula.lua`, which the March-2014 engine does not have. The port follows
June's own runtime, reverse-engineered in Ghidra, **including June's priority rule**: an
NPC with a NewNPCStat row uses it, even when it also has a legacy `NPCStat.lua` row.

This document is self-contained. To run a phase, paste the **"Starting context"** block
plus that phase's section into a fresh conversation, then paste the phase's **Kickoff
prompt**. Phases are sized for one conversation each and run in order, because each one
uses the previous one's outputs. Do not paste the whole document.

**Fix philosophy (the user's):**
- Transcribe June, don't guess. Every behaviour cites the June function it came from.
- Where Ghidra can't settle a question, read more of the binary. Never pick a plausible
  default. Where an interim value is unavoidable, it is the value the game *already*
  uses today, and it is logged as open.
- Keep June's own names. File names, Lua bindings, the Lua function, the enum and the
  table keys are all June's. Only the `SERV_IRUHADEV_*` flags are new, because
  `CLAUDE.md` requires them.
- Degrade visibly. If the packed data is missing, the feature turns itself off, the log
  says exactly what's missing, and the game behaves as it does today.
- Client Lua data is the user's to pack. Implement against the assumption that it's
  packed, and name the files when reporting done.

**Decision (user, 2026-09-26): full June parity, behind two flags.**

| Flag | What it does | Undefine it and… |
|---|---|---|
| `SERV_IRUHADEV_EXTRA_NPC_STAT` | Loader, evaluator, and formula stats for the **75** NPCs that have *no* legacy stat row (today they spawn at 0 HP). **This alone fixes the stage skip.** | …everything is exactly as today, including the bug |
| `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY` | Formula stats also **override** the **772** NPCs that do have a legacy row. June's `bNoHardLevel` goes into dungeon `SetNpcHardLevel`. `DEF_*` are copied over legacy rows (`DAMAGE_TYPE` is not: June never reads it, see Phase 0 verdict 3). | …old content goes back to today's numbers and the stage-skip fix stays |

Both are defined by default. The parity flag is the one that rebalances older content,
and Phase 5 is its go/no-go.

---

## Model and effort per phase (for the user; do not paste)

Same scheme as `JUNE2017_EFFECT_REBUILD_PLAN.md`. Spend Opus only where a mistake is both
likely and silent.

- **Identification** means deciding what a June function or field *is*, or whether a
  semantic differs. If this is wrong, everything built on it is wrong and still compiles.
  Use **Opus, high**.
- **Transcription** means copying settled behaviour into March code or data, then the
  byte-level edit, build, deploy and doc updates. Use **Sonnet 5, high**.

| Phase | Kind | Model / effort |
|---|---|---|
| 0 — RE close-out | Identification (Ghidra) | **Opus, high**. Max only if a candidate function still won't settle after one high pass. |
| 1 — Data: convert, enum, dry-run, stage | Mechanical, needs care | Sonnet 5, high |
| 2 — Balance before/after table | Analysis, then a user decision | Sonnet 5, high |
| 3 — C++ base flag (+ build/deploy) | Transcription from confirmed addresses | Sonnet 5, high, then an **Opus `/code-review`** of the diff |
| 4 — C++ parity flag | Transcription, semantics-sensitive | Sonnet 5, high, then an **Opus `/code-review`**. Use **Opus, high** for the whole phase if Phase 0 left accuracy/avoidance or `param_4` open. |
| 5 — Play-test verification, parity go/no-go | Log reading | Sonnet 5, high. Switch to **Opus, high** if a phase still skips with no `[NPC_ZERO_HP]` line (that means a new root cause). |
| 6 — Docs, cleanup, memory | Writing | Sonnet 5, high |

Rules:
- **Effort is always high.** Save by switching Opus to Sonnet, never by lowering effort.
  Low effort is where the CP949 check or `git diff --stat` gets skipped.
- **Max only to get unstuck**, never by default.
- **Haiku: not for this plan.**
- **Close every Sonnet phase that changes code with an Opus review** (`/code-review` on the
  phase's diff).
- **Stop a Sonnet session that makes its own identification call.** For example, "this
  June function looks like X" when X isn't recorded in the ledger. Switch that step to
  Opus.
- **One fresh session per phase.** Paste only "Starting context" plus the phase section.
  Send broad searches to a subagent, and decompile address ranges rather than whole huge
  functions.

---

## Starting context (paste this with any phase)

- **Repo root:** `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk`
- **Branch:** `mods/lanox-port`
- **Game directory** (deploy target, logs, save): `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`
- **March kom staging** (the user's): `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\kom_contents\dataNNN_kom\`
- **June2017 reference client** (read only, never modify): `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\dataNNN_kom\`
- **Ghidra:** project `els_jun_2017` (June `x2.exe`, unpacked) with the GhidraMCP HTTP server at
  `http://127.0.0.1:8080`. Drive it with curl as described in `.claude/skills/ghidra/SKILL.md`.
  Read-only unless the user agrees otherwise.
- **Client build** (default `US_SERVICE`; see `CLAUDE.md` *Toolchains*):
  ```sh
  MSBUILD="/c/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe"
  TRUNK="F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk"
  DATA="F:/elsword stuff/elsword_2014/els_2014/237311/24965799/data"
  MP="-m -p:MultiProcessorCompilation=true"
  touch X2Lib/stdafx.cpp      # always, after any header edit (Always.h, X2UnitManager.h, X2GUNPC.h are in the PCH)
  "$MSBUILD" X2ServerProtocol/X2ServerProtocol_2010.vcxproj -p:Configuration=X2TOOL     -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" KTDXLIB/KTDXLIB_2010.vcxproj                   -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2Lib/X2Lib_2010.vcxproj                       -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2/X2_2010.vcxproj                             -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  ```
  - The artifact is `X2/US_SERVICE/x2.exe` (lowercase). Copy it to `$DATA/X2_offline.exe`.
  - Confirm the name, size and mtime programmatically (`python -c "import os;…"`), not by eye.
  - If a flag seems to do nothing, prove it compiled in with a `#pragma message` inside the guard.
- **Standing constraints:**
  - **Flags.** Every client C++ change goes behind a `SERV_IRUHADEV_*` flag appended to
    `KTDXLIB/Always.h`, in the house comment block (`// Author: Iruha` / `// Date:` /
    `// Description:`), using the `#endif SERV_IRUHADEV_FOO` trailing-token style. The
    original code stays in `#else`.
  - **CP949 files.** `X2GUNPC.cpp/.h`, `X2UnitManager.cpp/.h`, `X2Game.cpp` and `X2Main.cpp`
    contain Korean CP949 bytes. **Edit them at the byte level only**: Python `rb`/`wb`,
    anchors read with `cat -A` or Python. Don't use bash heredocs with double backslashes.
    After every edit, run `file <path>` (the encoding must not change) and
    `git diff --stat` (a few lines, and no deletions on a pure insertion).
  - **Launching.** Never launch `X2_offline.exe` without asking first, every time.
  - **Save data.** `els_db.sql` in the game directory is real player data. Never delete it.
  - **Lua packing.**
    - Every shipped `.lua` is compiled `mar-2014` bytecode. For June bytecode use
      `june2017_re/ljconv.py`.
    - `.src.lua`/`.std.luac` never go inside a `.kom`.
    - Pack rule: `.lua` → algo `"2"`, everything else → `"0"` in `_manifest.json`.
    - House `_manifest.json` format: CRLF, 2-space indent, no trailing newline.
    - **The user packs.**
  - **Defender.** Windows Defender may quarantine a fresh `x2.exe` (Bearfoos false positive).
- **Logs** (game directory, flushed per line): `offline_server.log`, `offline_packets.log`,
  `ErrorLog.txt`, `State.log`.
  ```sh
  grep -E "KILL_SELF|NPC_ZERO_HP|XSTAT|EXTRA_NPC_STAT" "$DATA/offline_server.log" | tail -80
  grep -E "UNHANDLED|EXCEPTION" "$DATA/offline_packets.log" | sort | uniq -c | sort -rn
  ```
  `CX2OfflineLog::Server` is **not** open yet while `CX2UnitManager` loads its files at
  startup. Use `ErrorLogMsg` there, or defer the line to first use.

---

## Investigation ledger — proven, do not redo

### The defect

`offline_server.log`, 2026-09-26, Burned Forest Normal (`dungeonID=33270`), level-70 NPCs:

- Stage 0, sub-stage 0: NPC uids 1–10 all `died: KILL_BY_USER`, the last at `09:34.311`.
- **`09:40.392 GAME NPC uid=21 id=1694 lv=70 died: KILL_SELF`**. That's
  `NUI_RANOX_BURNDEDFOREST_GATE`, spawned `INACTIVE` in sub-stage 1, dying as sub-stage 1
  starts.
- `09:45.268 stage 1 loading`. Sub-stage 1's other 11 NPCs (uids 11–20, 22) never died.
  **This is the "phase clears with mobs alive" the user reported.**
- `09:46.630`: four `id=1702` (`NUI_TRAP_FIRE_SMALL`) `died: KILL_SELF`, 0.1 s after
  `stage 1 start`. Same mechanism.

### Root-cause chain

1. **The sub-stage's clear condition is the gate's death.**
   `data035_kom/Dungeon_Ranox_BurnedForest_Normal.src.lua` STAGE0 / SUB_STAGE1:
   `CLEAR_COND0 = { CLEAR_CONDITION.CC_KILL_NPC_TYPE, NPC_ID = { NUI_RANOX_BURNDEDFOREST_GATE }, NEXT_STAGE0 = { CLEAR_TYPE.CT_STAGE, 1, 0, 100 } }`.
   The gate is placed with `ACTIVE = FALSE`, `START_POS = { 20 }`.
2. **The gate is designed to be broken after the mobs die.**
   `data037_kom/RANOX_BURNDEDFOREST_GATE.src.lua`:
   - `START_STATE = RANOX_BURNDEDFOREST_GATE_WAIT_INVINCIBLE` (`INVINCIBLE {0,9999}`).
   - `CF_RANOX_BURNDEDFOREST_GATE_WAIT` moves it to the breakable `WAIT` state when
     `slot1:LiveActiveNPCNum() == 0`.
   - Its script has `INIT_COMPONENT.MAX_HP = 3000`, but that value is overwritten (point 3).
3. **It spawns with 0 HP.**
   - Sub-stage NPCs are created together at sub-stage start
     (`CX2DungeonSubStage::SwapToGameNPCUnit`, `X2DungeonSubStage.cpp:357`), then
     `SetNpcHardLevel` (`:384`) → `CX2GUNPC::SetHardLevel` (`X2GUNPC.cpp:7046`).
   - That computes `HP = (stat.m_fMaxHP + stat.m_HardLevel.m_fHP * level) * fHPScale`.
   - `m_stat` is filled only by `CX2UnitManager::AddNPCStat_LUA` (`X2UnitManager.cpp:679`).
     With no row it stays default-constructed, so HP is 0.
   - `DyingStart` finds no attacker and sends `NDS_KILL_SELF` (`X2GUNPC.cpp:13450`).
   - That satisfies `CC_KILL_NPC_TYPE` and triggers `CT_STAGE`. This is the same mechanism
     as `LANOX_DEFECT_PHASES.md` Phase 7d's field monsters.
4. **It's not a packing omission.**
   - The live `kom_contents/data037_kom/NPCStat.src.lua` is **byte-identical** to June's
     `14JUN17/data/data037_kom/NPCStat.src.lua` (1,087,871 B both).
   - June's copy is complete: it ends at `NUI_NPC_END`, so it isn't truncated.
   - June has no legacy row for any of the missing IDs either.
5. **June's actual source: `data036/NewNPCStat.lua` + `data036/NewNPCStatFormula.lua`.**
   - Found by a byte search of every June file for the missing IDs. The only other hit
     is `datai016_kom/NPCStat_INT.lua`, which has none of them.
   - `NewNPCStat.lua` (476,743 B) holds **1693** `g_pUnitManager:AddExtraNPCStat{…}` rows
     plus **67** `g_pUnitManager:AddExtraNPCFormulaVariable{…}` level bands.
   - `NewNPCStatFormula.lua` (1,834 B) holds `CalculateExtraStatValue` and 9 helpers.
   - Both names are also strings in June's exe.

### Audit counts (settled; re-derivable with the script in "Regenerating the scratch artifacts")

| Count | Meaning |
|---|---|
| 57 | Lanox dungeon NPC IDs (named in `Dungeon_Ranox_*.src.lua`) with **no** legacy stat row: all gates/switch gates/traps/regen points, `NUI_GEYSER_*`, `NUI_FIRE_DEVIL_*`, `NUI_MOUNT_*`, `NUI_EVENT_*_BOSS*`, all `NUI_SECRET_*`. **Every one** has a NewNPCStat row. |
| 0 | Non-Lanox dungeon NPC IDs without a legacy row. The gap is Lanox-only. |
| 1693 / 67 | NewNPCStat rows / formula variables |
| 836 | NewNPCStat IDs that are nil in March's `Enum.lua` (`NPC_UNIT_ID.X` → nil → skipped) |
| ~846 | NewNPCStat IDs with no March `NPCTemplet` entry (overlaps the above) |
| **847** | Rows usable in March (in `Enum.lua` **and** `NPCTemplet`) |
| **75** | …of those, with **no** legacy row: the 0-HP set, fixed by the base flag |
| **772** | …of those, **with** a legacy row: overridden only under the parity flag |
| 8 | `NUI_LOW_GEYSER_*_FD` field monsters. Phase 7d called them "unsourceable"; they are in NewNPCStat. |

FireTemple showed **no** stat gaps, and FinalHall only its two `NUI_EVENT_FINALHALL_BOSS2/3`.
If a FireTemple phase still skips after this port, it's a different defect.

### June runtime (Ghidra `els_jun_2017`; decompiled 2026-09-26)

| June function | Role | How it was established |
|---|---|---|
| `FUN_32374ea0` | **Evaluator** `float (statType, level, value, dungeonID)`. See the pseudo-code below. | decompile |
| `FUN_32373680` | "Does an exception entry exist for this dungeon?". Selects matching on the dungeon key (normal rows have key 0). | called from the evaluator |
| `FUN_32931920` | `return dungeonID == 38100;`, the Henir check | decompile (1 line) |
| `FUN_32931950` | switch over the secret-dungeon IDs (`0x75d0..`, `0x80fc..`, `0xa0b4..`), the Secret check | decompile |
| `FUN_323752f0` | Builds the stat block from one ExtraNPCStat row. Evaluates stat types **1–7 in order** into `+0x80` HP, `+0x84` physical attack, `+0x88` magic attack, `+0x8c` physical defense, `+0x90` magic defense, `+0xd0` accuracy, `+0xd4` avoidance. Copies the 6 element defenses (`+0xb0..+0xc4`). Writes `+0xdc = param_4` = **monster grade** (Phase 0 verdict 3), and scan rate `+0xc8`/near range `+0xcc` = **50/1000** for grade 1 or 2, else **10/500**. Sets `+0xe8 = 1`. **Never touches `HARD_LEVEL` (`+0x94..+0xa4`) or the damage type (`+0xac`).** `__thiscall` on the unit manager: `(block, &row, level, grade, dungeonID)`. | decompile + disassembly |
| `FUN_32384d10` | NPC unit-info fetch, June's widened `GetNPCUnitInfo` (March `X2UnitManager.cpp:416` takes only the ID): `(npcID, level, grade, bNoDrop, difficulty, dungeonID)`, 7 call sites. If `npcID` is in the ExtraNPCStat map (`this+0xb8`/end `+0xbc`), it calls `FUN_323752f0` at `323850dd`, so **extra overrides the legacy stat block**. The block it rewrites is the **shared per-ID unit info** (the map value), so it holds the last fetch's level/grade/dungeon. | decompile + disassembly of every caller's pushes |
| `FUN_32374e70` | `bool HasExtraNPCStat(npcID)`, a lookup in `this+0xbc` | decompile |
| **`FUN_327687b0`** | **`SetHardLevel(level, bResetHP, fHPScale, fAtkScale, bNoHardLevel)`**. `if (bNoHardLevel) stat = block[+0x80..+0x90]; else stat = block + HARD_LEVEL * (level * pvpCorrection)`. Then `SetNPCStat(hp*fHPScale, atkP*fAtkScale, atkM*fAtkScale, defP, defM)`. | decompile; identified via the `CreateBattleFieldNpc` call |
| `FUN_322f0a90` | `SetNpcHardLevel`. Not a dungeon room (`g_pX2Room+8 != 1`): `SetHardLevel(lvl,1,1,1,0)`. Defence dungeons (`g_pX2Room+0xdc - 0x959c < 3`): flag 0. **Default dungeon branch: `bNoHardLevel = g_pData->m_pUnitManager ? m_pUnitManager->HasExtraNPCStat(npcID) : 0`**, with the same difficulty HP scale as March. | decompile + disassembly (`322f0afe: MOV ECX,[EDX+0x38]` is `this` for the `HasExtraNPCStat` call) |
| `FUN_32d528f0` | `CreateBattleFieldNpc` (its error strings name it) → `SetHardLevel(lvl,1,1.0,1.0,0)` (`32d52a54`). So **field NPCs spawned through this path keep the legacy `HARD_LEVEL` term on top of formula stats.** That's June's real behaviour; replicate it, don't "fix" it. The other battlefield path, `FUN_32d539c0`, passes `HasExtraNPCStat` (Phase 0 verdict 5). | decompile + disassembly |
| `FUN_32d539c0` | **`CreateBattleFieldNpcByCreatedInfo`** (March `X2BattleFieldGame.cpp:818`; its struct matches `CNpcCreatedInfo` field for field: `m_uiBossGroupId +0xc`, `m_uiNpcId +0x10`, `m_ucLevel +0x4a`) → `SetHardLevel(lvl,1,1,1, HasExtraNPCStat(npc))` when the unit manager is non-null (`32d53c26-32d53c51`) | decompile + disassembly |
| `FUN_3230c1a0` | ally-summon re-level, March `CX2Game::SetUserSummonedNPCInfo` (`X2Game.cpp:13986`, the `:14002` call) → `SetHardLevel(ownerLvl, 0, 1, 1, 0)` at `3230c27b`, then `GetNPCStat`/`SetNPCStat(struct)` (`FUN_32758d50`/`FUN_32763950`) as March `:14009-14022` | decompile |
| `FUN_31ee6100` | loads `NewNPCStatFormula.lua` (June runs its own 20-byte-key decrypt first; March uses its normal `.kom` loader) | decompile |
| `DAT_33775860` | **`g_pData`**. `FUN_31e46300` does `new(0x5e8)`, stores it at `CX2Main+0x124` and in the global, right after `m_GameP2PPort = rand()%1000+8000`. That's exactly March's `X2Main.cpp:1346-1357` (`m_pData = new CX2Data(); g_pData = m_pData;`). `+0x38` is its unit-manager pointer. | disassembly |
| `FUN_31d95080` | Lua binding registration. `"AddExtraNPCStat"` (use `31db5cc8`), `"AddExtraNPCFormulaVariable"` (use `31db5ea3`), `"Add_HeroDungeon_ExtraStatForException"` (use `31dc3faa`; zero rows in the data, out of scope). | `june2017_re/xorstrings.tsv` rows 1122/1123/1269 |

**The evaluator, `FUN_32374ea0`, as pseudo-code:**
```
range = formulaVars.equal_range(statType)          // std::multimap keyed by stat type, file order kept
useException = FUN_32373680(statType, level, dungeonID)
for var in range (in order):
    if var.minLevel <= level <= var.maxLevel and var.dungeonKey == (useException ? dungeonID : 0):
        lvlArg   = IsHenir(dungeonID) ? 85 : level
        gameType = IsHenir(dungeonID) ? 1 : IsSecret(dungeonID) ? (unitManager[+0x118] ? 3 : 2) : 0
        return Lua CalculateExtraStatValue(statType, var.a, var.b, var.c, level, value, lvlArg, gameType)
return value                                        // no band -> the raw multiplier, unchanged
```
- `var.a/b/c` are **`BASIC_VALUE`/`GRADIENT_VALUE`/`INTERCEPT_VALUE`** (Phase 0 verdict 1).
  The decompile's `+4`/`+0xd`/`+0x10` are dword indices into the multimap node, not row
  offsets. HP, attack and defense don't use them.
- `unitManager[+0x118]` is never set in the port, so Secret is always `gameType 2`
  (Phase 0 verdict 4).
- **Band coverage (checked):** every stat type has bands covering levels 1–110, so HP never
  falls back to the raw multiplier.
- Some types have **overlapping** bands (HP has three for 1–10; `PHYSIC_ATT` has an odd
  `2..11` one). June takes the **first match in file order**. That doesn't matter for
  types whose formula ignores `a/b/c`. `ACCURACY` and `AVOIDANCE` have exactly one band
  each.

**`NewNPCStatFormula.lua`, as June ships it** (decompiled; verbatim):
```lua
function CalculateExtraStatValue(slot0, slot1, slot2, slot3, slot4, slot5, slot6, slot7)
	-- slot0 statType, slot1..3 formula var a/b/c, slot4 level, slot5 row value, slot6 lvlArg, slot7 gameType
	if EXTRA_NPC_STAT_TYPE.ENST_HP == slot0 then
		return math.floor((340 + 60 * slot4) * (9.994 + 0.006 * slot6 * slot6) * slot5 * GetHPFactorByGameType(slot4, slot7))
	elseif EXTRA_NPC_STAT_TYPE.ENST_PHYSIC_ATT == slot0 then
		return math.floor((340 + 60 * slot4) * (0.98 + 0.02 * slot6) * slot5 * GetAttackFactorByGameType(slot7))
	elseif EXTRA_NPC_STAT_TYPE.ENST_MAGIC_ATT == slot0 then
		return math.floor((340 + 60 * slot4) * (0.98 + 0.02 * slot6) * slot5 * GetAttackFactorByGameType(slot7))
	elseif EXTRA_NPC_STAT_TYPE.ENST_PHYSIC_DEF == slot0 or EXTRA_NPC_STAT_TYPE.ENST_MAGIC_DEF == slot0 then
		slot8 = 0
		return math.floor((slot7 == 1 and slot5 * DefFactor(slot4) * HenirDef(slot4) / (DefFactor(slot4) - (slot5 - 1) * HenirDef(slot4)) or slot7 == 2 and slot5 * DefFactor(slot4) * SecretNormDef(slot4) / (DefFactor(slot4) - (slot5 - 1) * SecretNormDef(slot4)) or slot7 == 3 and slot5 * DefFactor(slot4) * SecretLutoDef(slot4) / (DefFactor(slot4) - (slot5 - 1) * SecretLutoDef(slot4)) or slot5 * DefFactor(slot4) * NormalDef(slot4) / (DefFactor(slot4) - (slot5 - 1) * NormalDef(slot4))) * GetDefenseFactorByGameType(slot7))
	elseif EXTRA_NPC_STAT_TYPE.ENST_ACCURACY == slot0 then
		return slot1 * (slot2 * slot4 + slot3) * slot5 + 1
	elseif EXTRA_NPC_STAT_TYPE.ENST_AVOIDANCE == slot0 then
		return slot1 * (slot2 * slot4 * slot4 + slot3) / 1000 * slot5 + 1
	end
	return slot5
end
-- helpers: HenirDef, NormalDef (67.215 + 12.554*L), SecretNormDef, SecretLutoDef, DefFactor (258.65 + 39.35*L),
-- GetHPFactorByGameType (type 2: 0.95*((L-70)/36)^2.5 + 1.05; type 3: ((L-70)/36)^3.5 + 1.5; else 1),
-- GetAttackFactorByGameType (2 -> 1.5, 3 -> 2, else 1), GetDefenseFactorByGameType (2|3 -> 1.2, else 1)
```
The inline comment line on `slot0`..`slot7` is added here for reading; it's not in June's
file.

**Row shape** (`NewNPCStat.lua`; three real rows):
```
NUI_RANOX_BURNDEDFOREST_GATE: HP_VALUE 0.3, PHYSIC_ATT_VALUE 1, MAGIC_ATT_VALUE 1, PHYSIC_DEF_VALUE 0.5, MAGIC_DEF_VALUE 0.5,
                              ACCURACY_VALUE 0, AVOIDANCE_VALUE 0, DEF_RED 350 DEF_BLUE 50 DEF_GREEN 200 DEF_WIND 50 DEF_LIGHT 200 DEF_DARK 350, DAMAGE_TYPE DT_PHYSIC
NUI_MAG_NOM:                  HP_VALUE 1, ATT 1/1, DEF 1/1, ACCURACY_VALUE 1, AVOIDANCE_VALUE 0, (same element defs), DT_PHYSIC
NUI_GEYSER_WORKER:            HP_VALUE 2, ATT 1.5/1.5, DEF 1.2/1.2, ACCURACY_VALUE 2, AVOIDANCE_VALUE 0, DT_PHYSIC
formula var: { STAT_TYPE = ENST_AVOIDANCE, MIN_LEVEL = 1, MAX_LEVEL = 110, BASIC_VALUE = 0.02, GRADIENT_VALUE = 1, INTERCEPT_VALUE = 0 }
```
There's no dungeon-key field in any formula variable, so all are key 0.

**Expected values at level 70 in a normal dungeon** (`gameType 0`, `lvlArg 70`), **before**
`SetNpcHardLevel`'s `fHPScale`. That scale follows **dungeon difficulty** (`X2Game.cpp:18205-18217`):
Normal ×1.0, Hard ×1.2, Expert ×1.5, and Henir/Secret ×1.0. So every figure below is a
**Normal** figure; multiply for Hard or Expert. Party size doesn't enter into it.
`HP = floor(4540 × 39.394 × HP_VALUE)`, i.e. **gate 53,654**, **Mag Nom 178,848**,
Geyser Worker 357,697. `TRAP_FIRE_BIG`'s `HP_VALUE = 500` gives about 89M, an effectively
indestructible hazard (by design).

**`EXTRA_NPC_STAT_TYPE`** (June `data036_kom/Enum.src.lua:17197-17213`; absent from March's
`Enum.lua`):
`ENST_NONE 0, ENST_HP 1, ENST_PHYSIC_ATT 2, ENST_MAGIC_ATT 3, ENST_PHYSIC_DEF 4, ENST_MAGIC_DEF 5, ENST_ACCURACY 6, ENST_AVOIDANCE 7, ENST_BLUE_DEF 8, ENST_DARK_DEF 9, ENST_GREEN_DEF 10, ENST_LIGHT_DEF 11, ENST_RED_DEF 12, ENST_WIND_DEF 13, ENST_END 14`.

### Phase 0 verdicts (settled 2026-09-26; paste this subsection with Phases 1, 3 and 4)

Read-only work: Ghidra `els_jun_2017`, plus capstone over the raw
`14JUN17/data/unpacked_x2.exe` (`.text` VA = raw + `0x31aa0c00`) for code Ghidra has not
defined as functions. Nothing in the Ghidra project was changed.

**1. Formula-variable fields: settled.**
- The `AddExtraNPCFormulaVariable` binding is **`FUN_32382470`**; its pointer is pushed at
  `31db6024` in `FUN_31d95080`.
- It reads these keys, in this order:
  `STAT_TYPE` (int, the multimap key), `BASIC_VALUE` (float), `MIN_LEVEL` (int),
  `MAX_LEVEL` (int), `GRADIENT_VALUE` (float), `INTERCEPT_VALUE` (float), `DUNGEON_ID` (int).
- Each value is a 3-dword protected triple. The pair's copy constructor `FUN_3238d800`
  keeps source order (src bytes `0/0xc/0x18/0x24/0x30/0x3c` → node dwords
  `4/7/10/13/16/19`).
- The evaluator's `+4/+0xd/+0x10` are **dword indices into the `std::multimap` node**, not
  row offsets. The node holds the key at `[3]` and the value from `[4]`. So:
  - **`a = BASIC_VALUE`**, **`b = GRADIENT_VALUE`**, **`c = INTERCEPT_VALUE`**;
  - `MIN/MAX_LEVEL` are `[7]/[10]`, `DUNGEON_ID` is `[19]`.
- The formulas are therefore:
  - accuracy = `BASIC·(GRADIENT·L + INTERCEPT)·value + 1`;
  - avoidance = `BASIC·(GRADIENT·L² + INTERCEPT)/1000·value + 1`.
- **Required keys.** `DUNGEON_ID` is optional (missing → 0). The other six are
  **required**: if any one is missing, `FUN_32382470` drops the whole row without
  inserting it. The port drops such rows too, and counts and logs them. **Never fill in a
  default**; that would be a guess.

**2. Accuracy/avoidance: settled.** They are used raw as the basic stat value, and June
skips the 1.0 reset for NPCs with an extra row.
- **How the value gets in.** `CX2GUNPC::InitComponent` is **`FUN_327a0fa0`** (vtable slot
  `+0x3c` in the `CX2GUNPC` family, e.g. `.?AVCX2GUKiddayNPC@@` vtable `334b8084`).
  - It calls `SetNPCStat(const NPCUnitStat&)` (**`FUN_32763950`**) on
    `m_pNPCUnitInfo(+0x85f8) + 0x7c`, the ExtraNPCStat-built block (`327a0feb-327a0ff7`).
  - That sends stat `+0x54` (block `+0xd0`) to `m_AdditionalAccuracy` (`this+0x4b40`) and
    `+0x58` (block `+0xd4`) to `m_AdditionalAvoidance` (`this+0x4bf4`), both via
    `SetBasicStatValue` (`FUN_3236ca50`, `32763acd-32763aeb`).
  - There's no subtraction and no scaling. This is the same path as March
    (`X2GUNPC.cpp:3996` → `:7157-7158`).
- **The reset.** `CX2GUNPC::Init` is **`FUN_3277c030`**. It calls `InitComponent` first
  (`3277c0a4-3277c0a9`) and `InitAdditionalBasicStatValue` (**`FUN_32271540`**) later
  (`3277c160`).
  - June's reset sets accuracy = avoidance = 1.0 **unless**
    `m_eGameUnitType(+0x264) == GUT_NPC && m_pNPCUnitInfo->[+0xe8]`
    (`32271560-32271595`).
  - `+0xe8` is the flag the builder sets (`32375304`).
- **Today's March behaviour.** March's reset (`X2GameUnit.cpp:12474-12481`) is
  unconditional and runs after `InitComponent` (`X2GUNPC.cpp:2685` → `:2722`). So today
  **every normally spawned NPC has accuracy = avoidance = 1.0**, and the legacy
  `ACCURACY`/`AVOIDANCE` values (1225 of 2052 rows are `0.02`; 1920 avoidance rows are `0`)
  are dead data.
  - The old interim ("legacy for overridden, 0 for extra-only") was wrong about today's
    behaviour.
- **1.0 is neutral.** `X2DamageManager.cpp:2742-2751` computes
  `hit = 1.0·accuracy − (avoidance + extra − 1.0)`. The formula's `… + 1` is on that
  scale, so the worry about feeding "~1.0 into a percent field" had it backwards.
- **What the port needs depends on its route.** The end state to reproduce is that the
  NPC's accuracy/avoidance basic values hold the formula values after spawn.
  - **June's route.** The values sit in the stat block before `InitComponent`, and
    `Init`'s reset runs afterwards (`X2GUNPC.cpp:2685` → `:2722`). This route needs
    **both** the values and the reset guard in `CX2GameUnit::InitAdditionalBasicStatValue`.
    Without the guard, the change is silently undone.
  - **A post-`Init` route**, e.g. setting the values where `SetHardLevel` evaluates the
    formula. The `:2722` reset has already run by then, so this route needs the values
    plus a check that no later reset fires. The known resets are `:2722` `Init`, `:30286`
    `InitCheat`, and `X2GUUser.cpp:1515` (users only).
  - **Caution.** March's `m_pNPCUnitInfo->m_stat` is per-ID, and accuracy depends on the
    level. Storing the values in `m_stat` therefore inherits June's shared-block hazard
    (verdict 3).
- **Who it applies to.** In June `+0xe8` is set for **every** ID with an extra row,
  extra-only and overridden alike. Map that onto the flags: the base flag covers the 75,
  and parity adds the 772.
- The critical-rate reset to 0 stays unconditional in June too.

**3. `FUN_323752f0`'s `param_4`: settled.** It is the NPC's **monster grade**
(`MG_NORMAL_NPC 0 / MG_BOSS_NPC 1 / MG_MIDDLE_BOSS_NPC 2`, `X2GUNPC.h:231-235`), not the
damage type.
- **What the builder does with it** (`323754d0-32375512`):
  - It stores the grade at `+0xdc`.
  - Grade 1 or 2 → scan rate `+0xc8 = 50.0` and near range `+0xcc = 1000.0`
    (`0x3344a510`, `0x33437b40`).
  - Any other grade → `10.0` / `500.0` (`0x3344a51c`, `0x3346a6ac`).
  - `SetNPCStat(struct)` applies the scan fields only when the AI is `NAT_NORMAL`, as March
    does (`X2GUNPC.cpp:7160-7168`).
- **Corroboration.** The fetch looks the same value up as a u16 key in the hero-dungeon
  manager (`g_pData+0x44`, map `+0x4c`, `32384f56`).
  - That manager's binding, `Add_HeroDungeon_ExtraMonsterGradeStat` (**`FUN_327defc0`**,
    registered at `31dc3e4c`), keys it by `MONSTER_GRADE`.
  - Two June damage paths test `unitInfo+0xdc == 1` (`32048845`, `3228650b`). These are
    June-only bonuses and weren't traced further.
- **What each fetch caller passes as the grade:**

  | June caller | March counterpart | Grade passed |
  |---|---|---|
  | `FUN_327ef570`, sub-stage `CreateGUNPC(const NPCData*)` | `X2DungeonSubStage.cpp:2359` | `NPCData+0x44`, filled from `KNPCUnitReq.m_cMonsterGrade` (June `+0x66`, right after `m_cAllyTeam +0x65`) by **`FUN_327edb60`** (`327edc4f-327edc57`). The Lua sub-stage parser `FUN_327eb8e0` never sets it. |
  | `FUN_32d528f0`, `CreateBattleFieldNpc` | `X2BattleFieldGame.cpp:720` | `KNPCUnitReq.m_bShowBossName ? 1 : 0` (June `+0x52`; `32d5292c`) |
  | `FUN_32d539c0`, `CreateBattleFieldNpcByCreatedInfo` | `X2BattleFieldGame.cpp:818` | `CNpcCreatedInfo.m_uiBossGroupId > 0` (`+0xc`; `32d539f5`) |
  | `FUN_3230d2b0`, `CX2Game::CreateNPC` | `X2Game.cpp:6151` | `0` |
  | `FUN_31f0b690`, `FUN_32e8cfe0`, `FUN_32e8d0a0` (unidentified) | — | `0`, with level `0` |

- **The port has to plumb the grade through.** March's non-tool `NPCData` has no grade
  field; `m_MonsterGrade` is `X2TOOL`-only (`X2DungeonSubStage.h`).
- **Don't reuse March struct offsets on June structs.** June's `KNPCUnitReq` is one byte
  shorter than March's between `+0x3f` and `+0x55` (June `m_bIsRight` is `+0x55`, read from `FUN_327edb60`;
  March's is `+0x56`, computed from the `pack(1)` header).
- **`DAMAGE_TYPE` is never applied by June.**
  - June's `AddExtraNPCStat` binding (**`FUN_32380cf0`**, registered at `31db5d99`) reads
    exactly 14 keys, all required: `NPC_ID`, `HP_VALUE`, `PHYSIC_ATT_VALUE`,
    `MAGIC_ATT_VALUE`, `PHYSIC_DEF_VALUE`, `MAGIC_DEF_VALUE`, `DEF_RED`, `DEF_BLUE`,
    `DEF_GREEN`, `DEF_WIND`, `DEF_LIGHT`, `DEF_DARK`, `ACCURACY_VALUE`, `AVOIDANCE_VALUE`.
  - They're stored as 12-byte triples at row `+0x00…+0x90`, in that order.
  - There's no `DAMAGE_TYPE` read, and the builder never writes block `+0xac`. So an
    overridden NPC keeps its legacy damage type, and an extra-only NPC keeps the
    block's default. That default is `DT_PHYSIC` (0) in March's constructor; June's default
    wasn't read.
- **The `DEF_*` chain is confirmed.** Row `+0x3c/+0x48/+0x54/+0x60/+0x6c/+0x78`
  (RED/BLUE/GREEN/WIND/LIGHT/DARK) → block `+0xb0…+0xc4` = March
  `m_DefenseRed…m_DefenseDark`.
- **Hazard (don't copy it).** The fetch rewrites the shared per-ID block, and the level-0
  callers above rebuild it with the raw row multipliers, because no band covers level 0.

**4. UnitManager `+0x118`: settled for the port (always `gameType 2`); what it means in
June is open.**
- **Two writers found.** The image has 14 byte writes to some `+0x118`; the other 12 are
  not attributed.
  - The unit-manager constructor **`FUN_32385e90`** (called from `FUN_31ee6100`) zeroes it
    (`323861c2`).
  - The sub-stage `CreateGUNPC` (`FUN_327ef570`) copies `CX2DungeonGame+0xdd0` into it
    before every fetch when the game type is `GT_DUNGEON` (`327ef6b9-327ef6c8`).
- **Where `CX2DungeonGame+0xdd0` comes from.** The only byte-width writers found (the scan
  covered byte stores only) are in `CX2DungeonGame`'s packet handler: `327cc170`, vtable
  slot `+0x24c` (shared by `CX2PVEGame`), plus its standalone copy `327caa70`.
  - On server event **`0x50a`** the handler deserializes a single bool (`FUN_327d6440`).
    It then sets `+0xdd1 = 1` and `+0xdd0 = bool` (`327cc19d-327cc1b3`).
  - June's client event names aren't in the binary: the name table at `33743420` covers
    only the 21 System IDs. **Don't map `0x50a` to a March event by number.**
- **Port verdict.** March has neither the field nor the packet, and the offline emulator
  has no event to send. So Secret dungeons use **`gameType 2` always**.
  - That's also what June reads whenever the server never sends `0x50a`.
  - **Open:** what June's event `0x50a` flag means ("hell"-style Secret mode is the
    obvious guess, but it's unverified). **Interim = type 2.**

**5. Call sites: settled.** The table is in the Phase 0 section.
- **Correction:** March `X2BattleFieldGame.cpp:987` ↔ **`FUN_32d539c0`**, which passes
  `HasExtraNPCStat` (`32d53c26-32d53c51`), not `false`.
- Only `:782` ↔ `FUN_32d528f0` passes `false` (`32d52a54`).

**Corrections these verdicts make to later phases.** Phases 3 and 4 were patched in place
on 2026-09-26. Phase 1's body was not, so apply the first bullet when running it. The
list is kept as the record.
- **Phase 1 converter and audit.**
  - June drops any `AddExtraNPCFormulaVariable` row missing one of its six required keys,
    and any `AddExtraNPCStat` row missing one of its 14; `DAMAGE_TYPE` is ignored.
  - Count and report the rows June would drop. Never default-fill them.
  - Re-run the band-coverage check (1–110 per stat type) and the 847/75/772 audit on
    **June-loadable rows only**. The ledger's figures were computed from the raw source
    with no required-key filter.
- **Phase 3 `AddExtraNPCStat_LUA`.** Don't copy `DAMAGE_TYPE`. Copy the six `DEF_*` as
  planned. **Phase 4 item 7:** the same, and the scan fields come from the grade (verdict
  3), not the row.
- **Phase 3 `SetHardLevel`/`InitComponent` and Phase 4 item 8.** Handle accuracy/avoidance
  per verdict 2, with both halves (the value **and** the reset guard). "March's value"
  today is 1.0, not 0 and not the legacy row.
- **Phase 3/4 scan fields.** Set scan rate/near range from the grade for every NPC whose
  block comes from an extra row. The grade must be plumbed from `KNPCUnitReq.m_cMonsterGrade`
  (sub-stage), `m_bShowBossName` (`:782`) and `m_uiBossGroupId` (`:987`).
- **Phase 3 `CalculateExtraStatValue`.** For Secret dungeons, `gameType` is 2 (verdict 4).
- **Phase 4 item 5.** `:987` passes `HasExtraNPCStat`; only `:782` stays `false`.

### Naming (June's where recoverable, house-derived otherwise)

- **June's own names:**
  - files `NewNPCStat.lua`, `NewNPCStatFormula.lua`
  - bindings `AddExtraNPCStat`, `AddExtraNPCFormulaVariable`
  - Lua function `CalculateExtraStatValue`
  - enum `EXTRA_NPC_STAT_TYPE` / `ENST_*`
  - row and var keys `HP_VALUE` … `STAT_TYPE`
- **C++ names.** June's binary is stripped, with no symbols or RTTI for this system, so the
  C++ names are derived from those originals using the `AddNPCStat`↔`AddNPCStat_LUA` and
  `NPCUnitStat` patterns:
  - `CX2UnitManager::AddExtraNPCStat_LUA`, `AddExtraNPCFormulaVariable_LUA`
  - structs `ExtraNPCStat`, `ExtraNPCFormulaVariable`
  - `enum EXTRA_NPC_STAT_TYPE`
  - `CalculateExtraStatValue(...)`, named after the Lua function it calls
  - `HasExtraNPCStat(...)`, named for `FUN_32374e70`'s role
  - `SetHardLevel(…, bool bNoHardLevel = false)`, June's extra parameter
  - members named after the Lua keys (`m_fHPValue`, `m_iMinLevel`, `m_fBasicValue`, …)
- **The only new names:** `SERV_IRUHADEV_EXTRA_NPC_STAT`, `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY`,
  and the short-lived `SERV_IRUHADEV_EXTRA_NPC_STAT_DEBUG`.

### Dead ends (do not re-check)

| Hypothesis | Verdict |
|---|---|
| Sub-stage clear logic or `LiveActiveNPCNum` is wrong | No. The clear was `CC_KILL_NPC_TYPE` on a gate that died of 0 HP. |
| June's `NPCStat.lua` extract is truncated and lost the rows | No. It ends at `NUI_NPC_END` and is byte-identical to the live one. |
| June reads a stat-redirect key from `NPCTemplet` | No. June's templet keys are only `NAME DESC NPC_ID CLASS_TYPE IS_NPC MOTION LUA_FILE_NAME LUA_FILE_NAME_UI NPC_FACE_TEXTURE(_KEY)`. |
| June falls back to the unit script's `INIT_COMPONENT.MAX_HP` | No. Real monsters like `GEYSER_WORKER.lua`/`FIRE_DEVIL_FIGHTER.lua` have no `MAX_HP` at all. March reads it (`X2GUNPC.cpp:2919`), but `SetHardLevel` overwrites it. |
| `NPCStat_INT.lua` holds the rows | No. None of the missing IDs are in it. |
| June's `SetHardLevel` reads the `+0xe8` flag itself | No. The **callers** pass `bNoHardLevel`. |
| `DAT_33775860+0x38` is some unknown June toggle | No. It's `g_pData`'s unit-manager pointer, so the gate is a null check. |

### Regenerating the scratch artifacts (the 2026-09-26 scratchpad doesn't persist)

```sh
# Decompiled June files (read only; for reading and diffing, never for shipping)
J="F:/elsword stuff/elsword_2014/els_2014/237311/14JUN17/data/data036_kom"
S="$TEMP/xstat"; mkdir -p "$S"; cp "$J/NewNPCStat.lua" "$J/NewNPCStatFormula.lua" "$S/"
cd june2017_re
for f in NewNPCStat NewNPCStatFormula; do
  ../elstools/elstools.exe decompile-batch -q -i "$S/$f.lua"   # leaves $f.std.luac
  python ljd_jun2017.py "$S/$f.lua"                            # -> $f.src.lua; expect "ok, 20695 / 269 instructions"
done
```
Audit (the counts table): parse `NPC_ID = NPC_UNIT_ID.(\w+)` out of `NewNPCStat.src.lua`,
the live `kom_contents/data037_kom/NPCStat.src.lua`, `…/data037_kom/NPCTemplet.src.lua`,
and the `NPC_UNIT_ID = { … }` table of `…/data036_kom/Enum.src.lua`. Intersect them.
Usable = extra ∩ enum ∩ templet; override = usable ∩ legacy; extra-only = usable − legacy.
For Lanox gaps, collect the IDs named in `data035_kom/Dungeon_Ranox_*.src.lua` that are
not in legacy.

---

## Phase 0 — RE close-out (Ghidra, read-only)

**Status:** done 2026-09-26. Items 1, 2, 3 and 5 are settled. Item 4 is settled for the
port (Secret `gameType` is always 2), but what June's flag means is **open**, with
interim = type 2. The verdicts are in the ledger under "Phase 0 verdicts". The call-site
table is below. The Ghidra project was not modified.

**Paste with:** Starting context + this section.

**Model:** Opus, high. Identification: a wrong answer here compiles fine and corrupts
every stat.

**Goal.** Settle the five questions the ledger leaves open, and write each verdict into
this document's ledger (replacing the question) before any code is written. None of them
blocks the Burned Forest fix; they decide accuracy/avoidance, Secret-dungeon scaling and
exact call-site parity.

**Verdicts.** Each one replaces the question it answers; evidence and addresses are in the
ledger under "Phase 0 verdicts".

1. **Formula-variable field order.** `a/b/c` = `BASIC_VALUE`/`GRADIENT_VALUE`/`INTERCEPT_VALUE`
   (binding `FUN_32382470`, pushed at `31db6024`). Six of its seven keys are required;
   `DUNGEON_ID` is optional.
2. **Accuracy/avoidance.** Used **raw** as the `SetBasicStatValue` basic value, through
   `InitComponent` → `SetNPCStat(struct)`. June then skips `Init`'s 1.0 reset for NPCs
   whose block came from an extra row. 1.0 is the neutral value. March today resets
   every NPC to 1.0, so the legacy values are dead.
3. **`param_4`.** It's the **monster grade** (0/1/2), which picks scan rate/near range
   50/1000 (boss or middle boss) vs 10/500. It isn't the damage type, and **June never
   reads `DAMAGE_TYPE`** from NewNPCStat at all.
4. **UnitManager `+0x118`.** It's copied from `CX2DungeonGame+0xdd0`, which only a June
   server packet (event `0x50a`, one bool) sets. The port uses `gameType 2`. The flag's
   meaning is **open, interim = type 2**.
5. **Call-site table.** Below.

**Call-site table (verdict 5).**
- June `SetHardLevel` = `FUN_327687b0(level, bResetHP, fHPScale, fAtkScale, bNoHardLevel)`.
  It has exactly five call sites: `3230c27b`, `322f0c32`, `322f0cd6`, `32d52a54`,
  `32d53c51`.
- "HasExtra" below means
  `g_pData->m_pUnitManager ? HasExtraNPCStat(m_pNPCUnitInfo->m_templet.m_nNPCUnitID) : 0`.
  `FUN_32374e70` takes its key by reference, and `322f0b0a` passes `m_pNPCUnitInfo`,
  whose first field is the NPC ID.

| March call site | June call site | June arguments |
|---|---|---|
| `X2BattleFieldGame.cpp:782`, `CreateBattleFieldNpc` | `32d52a54` in `FUN_32d528f0` | `(req.m_Level, 1, 1.0, 1.0, 0)` |
| `X2BattleFieldGame.cpp:987`, `CreateBattleFieldNpcByCreatedInfo` | `32d53c51` in `FUN_32d539c0` | `(info.m_ucLevel, 1, 1.0, 1.0, HasExtra)`; no call at all when the unit manager is null |
| `X2DungeonSubStage.cpp:384` → `SetNpcHardLevel` | `327f0248` in `FUN_327f0070` (sub-stage spawn loop: `CreateGUNPC` → `Init(0, m_StartPos)` → `SetNpcHardLevel(npc, m_Level)`) | per the `SetNpcHardLevel` rows |
| `X2Game.cpp:6286`, `CreateNPC` → `SetNpcHardLevel` | `3230d8ff` in `FUN_3230d2b0` | per the `SetNpcHardLevel` rows |
| `X2Game.cpp:14002`, `SetUserSummonedNPCInfo` ally re-level | `3230c27b` in `FUN_3230c1a0` | `(ownerLevel, 0, 1.0, 1.0, 0)` |
| `SetNpcHardLevel` (`X2Game.cpp:18149` = `FUN_322f0a90`): non-dungeon room `:18290`, and in-dungeon `NCT != NCT_NONE` `:18272` | `322f0cd6` | `(createType == NCT_MONSTER_CARD ? GetSummonMonsterCardNPCLevel(…) (FUN_322cea20) : level, 1, 1.0, 1.0, 0)`, no force-down. June tests only `createType != 0`. |
| `:18181`, `CASE_DEFENCE_DUNGEON` (`dungeonID − 38300 < 3`) | `322f0c32` | `(level, 1, 1.0, 1.0, 0)` |
| `:18197` Henir / `:18201` Secret | `322f0c32` | `(level, 1, 1.0, 1.0, HasExtra)` (June dungeon types 2/3 = `DT_HENIR`/`DT_SECRET`) |
| `:18209` / `:18213` / `:18217` Normal / Hard / Expert | `322f0c32` | `(level, 1, 1.0 / 1.2 / 1.5, 1.0, HasExtra)` (`0x3344ce94` = 1.2, `0x33471a1c` = 1.5) |
| `:18221`, other difficulty | `322f0c32` | `(level, 1, 1.0, 1.0, 0)`: June **clears** the flag here |
| none (June-only): hero dungeon | `322f0c32` | `FUN_32932230` → `FUN_32931be0` (hero-dungeon ID ranges) forces `fHPScale 1.0` before the difficulty switch; the flag is kept |
| none (June-only): `GetGameType() == 4` | `322f0c32` | `(level, 1, 1.0, 1.0, HasExtra)`. March's enum has no 4 (`GT_BATTLE_FIELD` is 3), so there's nothing to port. Other non-dungeon types make no call, same as March. |
| force-down tail `:18234-18253` | `322f0c37…` | same 130/195/234/260 by `g_pX2Room+0xac` player count; skipped on the `322f0cd6` path, as in March |

**Watch for.** Don't use Ghidra mutation endpoints (rename, retype, or creating a function
at `0x32763b40`-style undefined code) without asking the user; they change the user's
project. If a needed function is undefined in Ghidra, ask the user to press `F` at the
address.

**Exit criteria.** The ledger's June table has a verdict (or an explicit "open, interim =
today's value") for items 1–5. The call-site table is written into this section.

**Kickoff prompt**:
```
Continuing the Lanox ExtraNPCStat port. Read LANOX_EXTRA_NPC_STAT_PLAN.md
("Starting context", "Investigation ledger", "Phase 0") and
.claude/skills/ghidra/SKILL.md. Branch: mods/lanox-port. Do not re-derive anything
in the ledger.

Your task (Ghidra, read-only, no code): settle Phase 0's five items in June's x2.exe
(project els_jun_2017, GhidraMCP on 127.0.0.1:8080): (1) which of
BASIC/GRADIENT/INTERCEPT sits at formula-var offsets +4/+0xd/+0x10, via the
AddExtraNPCFormulaVariable binding in FUN_31d95080; (2) how June consumes accuracy/
avoidance (+0xd0/+0xd4), given that the formula returns about 1.0; (3) what
FUN_323752f0's param_4 is; (4) what sets UnitManager +0x118 (Secret type 3); (5) the
March<->June SetHardLevel call-site table. Cite addresses. If something won't settle,
write "open, interim = today's value". Don't guess. Don't use Ghidra mutation
endpoints without asking me.

Done when: every item has a verdict written into the plan's ledger and the Phase 0
Status line is updated.
```

---

## Phase 1 — Data: convert, enum, dry-run, stage (you pack it)

**Status:** not started.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high.

**Goal.** Produce March-loadable `NewNPCStat.lua` and `NewNPCStatFormula.lua`, add June's
`EXTRA_NPC_STAT_TYPE` block to the existing `Enum.lua`, prove the whole table loads in a
Lua dry-run, and stage it all for the user to pack into `data036.kom`. No C++ in this
phase.

1. **Verify the sources.**
   - Sources: `14JUN17/data/data036_kom/NewNPCStat.lua` (476,743 B) and
     `NewNPCStatFormula.lua` (1,834 B).
   - Check both sizes against June's live `data036.kom` manifest
     (`elstools inspect -q -i <kom>`). elstools can truncate June algo-2 entries by
     1–255 bytes at the tail. If either file is short, stop and tell the user.
2. **Convert with `june2017_re/ljconv.py`** (bytecode transcode `0x82` → `0x81`, see
   `june2017_re/README.md` "Phase 10b").
   - **Never copy anything into `june2017_re/june_lua/`.** `census.py`, `fxdata.py` and
     `reconcile.py` glob that directory as the effect-rebuild's input set, so stat files
     there would pollute its key census.
   - `ljconv.py` builds its opcode table from the `.std.luac` pairs of the files it's
     given, so **don't run it on these two files alone**. It reads each pair
     stem-relative (`f[:-4] + '.std.luac'`), so explicit paths outside `june_lua/` work.
   - Keep both June files and their `.std.luac` (from `elstools decompile-batch`, see
     "Regenerating the scratch artifacts") in a scratch directory. Then pass **both sets
     explicitly**:
     `python ljconv.py OUTDIR june_lua/<each of the 14 non-.src .lua> <scratch>/NewNPCStat.lua <scratch>/NewNPCStatFormula.lua`.
     That way the table has full evidence. An unmapped opcode is a hard error, never a
     pass-through.
   - Do **not** decompile and recompile `NewNPCStatFormula.lua`. It has nested `and`/`or`
     chains, which is exactly where ljd mis-structures code.
3. **Round-trip check.**
   - Decompile the `mar-2014` outputs (elstools `-v mar-2014` is safe for `0x81`) and
     diff them against the June `.src.lua` (see "Regenerating the scratch artifacts").
   - Expect 1693 `AddExtraNPCStat` blocks and 67 `AddExtraNPCFormulaVariable` blocks, and
     all 10 functions (`CalculateExtraStatValue`, `HenirDef`, `NormalDef`,
     `SecretNormDef`, `SecretLutoDef`, `DefFactor`, `GetHPFactorByGameType`,
     `GetAttackFactorByGameType`, `GetDefenseFactorByGameType`, plus the top chunk).
4. **Add the enum to the existing `Enum.lua`.**
   - Append June's `EXTRA_NPC_STAT_TYPE = { … }` block (values in the ledger) verbatim to
     `march2014/data036/Enum.src.lua`, inside a `--{{ Iruha : <date> // ExtraNPCStat
     port: June2017 Enum.lua:17197` … `--}}` change block.
   - First confirm that this copy and `kom_contents/data036_kom/Enum.src.lua` agree. If
     they differ, patch whichever is the live staging copy, and tell the user.
   - Compile with `elstools compile-batch -v mar-2014`.
   - Preserve the file's existing encoding, BOM and line endings byte for byte.
5. **Load dry-run.** This is the safety net for silent truncation.
   - Run the converted `NewNPCStat.lua` under `elstools/vendor/luajit/luajit.exe` (it
     loads `0x81`), in one Lua state, after the compiled March `Enum.lua`.
   - Stub `g_pUnitManager` so `AddExtraNPCStat`/`AddExtraNPCFormulaVariable` just count
     and record `NPC_ID`. Any other method should raise.
   - **Assert:** 1693 + 67 calls; exactly **847** rows whose `NPC_ID` is non-nil *and* in
     the March templet. Also check every `STAT_TYPE` is non-nil.
   - A nil-table index partway through would stop the chunk silently at a row count
     below 1693. That's the failure this step exists to catch.
   - Also load the converted `NewNPCStatFormula.lua` and call
     `CalculateExtraStatValue(1, 0,0,0, 70, 0.3, 70, 0)`. It must equal **53654** (the
     gate at level 70).
6. **Stage.**
   - New files go in `march2014/update/data036/NewNPCStat.src.lua` + `.lua` and
     `NewNPCStatFormula.src.lua` + `.lua` (the `.src.lua` is the June decompile, for
     reading only), plus the user's `kom_contents/data036_kom/`.
   - Register `"NewNPCStat.lua": "2"` and `"NewNPCStatFormula.lua": "2"` in
     `_manifest.json`, in house format (CRLF, 2-space indent, no trailing newline).
   - `Enum.lua` is already registered; don't duplicate it.
   - Never stage `.std.luac`.

**Exit criteria.**
- Round-trip diff clean.
- Dry-run asserts pass (1693 / 67 / 847 / 53654).
- Files staged and manifests updated.
- The report to the user names the three files to pack into `data036.kom`:
  `NewNPCStat.lua`, `NewNPCStatFormula.lua`, and the updated `Enum.lua`.

**Kickoff prompt**:
```
Continuing the Lanox ExtraNPCStat port. Read LANOX_EXTRA_NPC_STAT_PLAN.md
("Starting context", "Investigation ledger", "Phase 1") and june2017_re/README.md
("Phase 10b"). Branch: mods/lanox-port. Do not re-derive anything in the ledger.

Your task (data only, no C++): convert June's data036 NewNPCStat.lua and
NewNPCStatFormula.lua to mar-2014 with june2017_re/ljconv.py. Pass the 14 june_lua/
files plus these two (kept in a scratch dir) explicitly, never these two alone.
Never copy anything into june_lua/ (other scripts glob it), and never
decompile+recompile the formula file. Round-trip-diff the output. Append June's EXTRA_NPC_STAT_TYPE block to the
existing march2014/data036/Enum.src.lua (house change block, bytes preserved) and
compile it. Then run the Phase 1 step 5 luajit dry-run and assert 1693/67 calls,
847 usable rows, and CalculateExtraStatValue(1,0,0,0,70,0.3,70,0) == 53654. Stage
into march2014/update/data036/ + kom_contents/data036_kom/ with _manifest.json
entries in house format.

Done when: the asserts pass and the files are staged. Tell me exactly which files to
pack into data036.kom. Update the plan's Phase 1 Status line.
```

---

## Phase 2 — Balance before/after table (analysis only)

**Status:** not started.

**Paste with:** Starting context + this section (+ Phase 0's verdicts in the ledger).

**Model:** Sonnet 5, high.

**Goal.** Put numbers on "parity rebalances old content" **before** anything is played.
No game file changes.

1. For each of the **772** override IDs, find the NPC level(s) it spawns at. Use
   `m_NPCLevel` in `data035_kom/DungeonData.src.lua` for every dungeon whose stage file
   names it (`Dungeon_*.src.lua`); for battlefields, use `BattleFieldData.lua`'s level.
2. Compute three things at those levels:
   - **legacy**: `MAX_HP + HARD_LEVEL.HP × lv` (the same for attack and defense);
   - **June-dungeon**: formula, `bNoHardLevel = 1`;
   - **June-field**: formula + legacy `HARD_LEVEL × lv`, `bNoHardLevel = 0`.
   Run June's `CalculateExtraStatValue` itself under the vendored luajit, using the
   Phase 1 outputs; don't reimplement it. Use Phase 0's verdicts for accuracy/avoidance
   and game type.
3. Write `june2017_re/xstat_balance.tsv` (ID, name, dungeon, level, legacy vs June per
   stat, ratio). Also write a short summary into this section: the ratio distribution per
   region (Ruben/Elder/Bethma/Altera/Feita/Velder/Hamel/Sander/Lanox) and the ten largest
   swings each way.
4. Show the summary to the user. **This is input to Phase 5's go/no-go, not a decision
   made here.**

**Exit criteria.** The TSV is written, the summary is in this section, and the user has
seen it.

**Kickoff prompt**:
```
Continuing the Lanox ExtraNPCStat port. Read LANOX_EXTRA_NPC_STAT_PLAN.md
("Starting context", "Investigation ledger", "Phase 2"). Branch: mods/lanox-port.
Do not re-derive anything in the ledger.

Your task (analysis only, change no game files): for the 772 NPCs whose legacy stat row
the parity flag overrides, compute legacy vs June-dungeon (formula, no HARD_LEVEL) vs
June-field (formula + HARD_LEVEL) HP/attack/defense at the levels they actually spawn
at (DungeonData m_NPCLevel / BattleFieldData). Run June's own CalculateExtraStatValue
from Phase 1's converted file under the vendored luajit; don't reimplement it. Write
june2017_re/xstat_balance.tsv plus a per-region ratio summary into Phase 2's section,
and show me the summary.

Done when: the TSV and summary exist and I've seen them. Update the Phase 2 Status line.
```

---

## Phase 3 — C++ base flag: tables, loader, evaluator, extra-only stats (+ build/deploy)

**Status:** not started.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high, then an Opus `/code-review` of the diff.

**Needs:** Phase 1 only. Phase 0's verdicts are applied if present, otherwise the
documented interim values. Phase 2 isn't needed here. **Shortest path to a fixed Burned
Forest: Phase 1, then Phase 3.** Phases 0 and 2 must be done before Phase 4.

**Goal.** Under `SERV_IRUHADEV_EXTRA_NPC_STAT`, load June's two files and give the
**75** NPCs with no legacy row their June formula stats. This fixes the stage skip. The
parity pieces are Phase 4; don't add them here.

**`KTDXLIB/Always.h`.** Append `SERV_IRUHADEV_EXTRA_NPC_STAT` and
`SERV_IRUHADEV_EXTRA_NPC_STAT_DEBUG` (short-lived) in the house comment block.

**`X2Lib/X2UnitManager.h`** (in the PCH). New members stay in X2Lib-only classes, so no
`X2ServerProtocol` translation unit sees a different layout.
- `enum EXTRA_NPC_STAT_TYPE { ENST_NONE = 0, ENST_HP = 1, … ENST_END = 14 };`. Mirror June's
  values exactly, citing `data036/Enum.lua:17197`.
- `struct ExtraNPCStat { NPC_UNIT_ID m_nNPCUnitID; float m_fHPValue, m_fPhysicAttValue,
  m_fMagicAttValue, m_fPhysicDefValue, m_fMagicDefValue, m_fAccuracyValue,
  m_fAvoidanceValue; int m_DamageType; int m_DefenseRed, m_DefenseBlue, m_DefenseGreen,
  m_DefenseWind, m_DefenseLight, m_DefenseDark; };`. The last two follow `NPCUnitStat`'s own
  member names.
- `struct ExtraNPCFormulaVariable { EXTRA_NPC_STAT_TYPE m_eStatType; int m_iMinLevel,
  m_iMaxLevel; float m_fBasicValue, m_fGradientValue, m_fInterceptValue; };`
- `std::map<NPC_UNIT_ID, ExtraNPCStat> m_mapExtraNPCStat;`
  `std::multimap<int, ExtraNPCFormulaVariable> m_mmapExtraNPCFormulaVariable;` (insertion
  order is preserved for equal keys, which gives June's first-match-in-file-order)
  `bool m_bExtraNPCStatReady;`
- On `NPCUnitInfo`: `bool m_bHasLegacyNPCStat;`. Set it in `AddNPCStat_LUA`. "No legacy
  row" must be decided from this, **not** from `m_stat.m_nNPCUnitID`, because Phase 4 may
  write into `m_stat`.
- Methods:
  - `bool AddExtraNPCStat_LUA();`
  - `bool AddExtraNPCFormulaVariable_LUA();`
  - `bool HasExtraNPCStat( NPC_UNIT_ID ) const;`
  - `const ExtraNPCStat* GetExtraNPCStat( NPC_UNIT_ID ) const;`
  - `float CalculateExtraStatValue( EXTRA_NPC_STAT_TYPE eStatType, int iLevel, float fValue, int iDungeonID ) const;`
  - `bool IsExtraNPCStatReady() const;`

**`X2Lib/X2Main.cpp`**, next to `:3598` (`"AddNPCStat"`):
`lua_tinker::class_def<CX2UnitManager>( L, "AddExtraNPCStat", &CX2UnitManager::AddExtraNPCStat_LUA );`
and the same for `"AddExtraNPCFormulaVariable"`.

**`X2Lib/X2UnitManager.cpp`:**
- **`OpenNPCFiles` (`:258`):** after `_OpenNPCStatScriptFile( pStatFile )` succeeds, load
  `L"NewNPCStatFormula.lua"` and then `L"NewNPCStat.lua"`. Use literal names, following
  `X2Data.cpp:1799`'s `L"NpcExtraAbility.lua"` precedent, and the same loading call
  `_OpenNPCStatScriptFile` (`:320`) uses. **A failure here must never make
  `OpenNPCFiles` return false.**
- **Degrade visibly.** Set `m_bExtraNPCStatReady = false` and log exactly which piece is
  missing, plus "pack it into data036.kom", if any of these holds. The game then behaves
  as it does today.
  - The Lua global `EXTRA_NPC_STAT_TYPE` is nil, i.e. `Enum.lua` wasn't repacked. **Check
    this before loading `NewNPCStat.lua`**, whose first row would otherwise raise and
    abort the rest of the chunk.
  - Either file fails to load.
  - The global function `CalculateExtraStatValue` is missing afterwards.
- **One summary line:** `EXTRA_NPC_STAT loaded N rows (skipped: X nil NPC_ID, Y no templet;
  Z extra-only, W with legacy row) and V formula vars`. Expect X≈836, N−X−Y = 847, Z = 75,
  W = 772, V = 67. Use `ErrorLogMsg`, or buffer the line and emit it through
  `CX2OfflineLog::Server` on first use.
- **`AddExtraNPCStat_LUA`.** Mirror `AddNPCStat_LUA` (`:679`: `KLuaManager`,
  `LUA_GET_VALUE`, `LUA_GET_VALUE_ENUM`).
  - Skip `NUI_NONE` and IDs with no `m_mapNPCUnitInfo` entry, and count both.
  - Store the row in `m_mapExtraNPCStat`.
  - For **extra-only** IDs (`!info.m_bHasLegacyNPCStat`), copy the six `DEF_*` into
    `info.m_stat`. The resistance reader at `X2GUNPC.cpp:25994` and `InitComponent` read
    them from there. Leave `m_HardLevel` at 0.
  - **Don't read or copy `DAMAGE_TYPE`.** June's binding never reads it, so `m_DamageType`
    keeps its default `DT_PHYSIC` (Phase 0 verdict 3).
  - A row missing any of the 14 June keys is dropped, counted and logged, as June does.
    Never default-fill it (verdict 3).
- **Scan fields.** For extra-only IDs, set `m_fScanRate`/`m_fScanNearRange` from the
  spawn's monster grade: 50/1000 for grade 1 or 2, else 10/500 (verdict 3, June
  `FUN_323752f0`). The grade has to be plumbed from the caller:
  - `KNPCUnitReq.m_cMonsterGrade` for sub-stage NPCs;
  - `m_bShowBossName ? 1 : 0` at `X2BattleFieldGame.cpp:782`;
  - `m_uiBossGroupId > 0` at `:987`;
  - `0` in `CX2Game::CreateNPC`.

  March's non-tool `NPCData` has no grade field. The values are applied only for
  `NAT_NORMAL` AI.
- **`AddExtraNPCFormulaVariable_LUA`.** Read `STAT_TYPE`, `MIN_LEVEL`, `MAX_LEVEL`,
  `BASIC_VALUE`, `GRADIENT_VALUE`, `INTERCEPT_VALUE` (all required) and `DUNGEON_ID`
  (optional, default 0). Insert into the multimap keyed by stat type. A row missing a
  required key is dropped, counted and logged, as June does (verdict 1).
- **`CalculateExtraStatValue`** is an exact replica of `FUN_32374ea0` (pseudo-code in the
  ledger):
  - First band in `equal_range(statType)` with `min ≤ level ≤ max`. **No band → return
    `fValue`.**
  - `lvlArg = CX2Dungeon::IsHenirDungeon(dungeon) ? 85 : level`. Verify it covers 38100.
  - `gameType` = Henir 1; `CX2Dungeon::IsSecretDungeon` 2 (**always 2**: June's type-3
    flag comes from a server packet March doesn't have, Phase 0 verdict 4); else 0.
  - Then call Lua `CalculateExtraStatValue( statType, a, b, c, level, fValue, lvlArg,
    gameType )`, with `a = BASIC_VALUE`, `b = GRADIENT_VALUE`, `c = INTERCEPT_VALUE`
    (verdict 1).
    - **Don't use `lua_tinker::call`.** KNCSDK's `Include_2010/lua_tinker.h` only has
      overloads up to 6 arguments, and this call needs 8.
    - Use the raw Lua C API on `g_pKTDXApp->GetLuaBinder()->GetLuaState()`:
      `lua_getglobal`, push 8 numbers, `lua_pcall(L, 8, 1, 0)`, read `lua_tonumber`, then
      `lua_pop`. Keep the stack balanced on every path.
    - **If the pcall fails**, log the Lua error once, with a grep-able
      `EXTRA_NPC_STAT` prefix, and return the legacy result for that NPC. Never return a
      silent 0: that would bring back the 0-HP `KILL_SELF` with nothing in the log
      except `[NPC_ZERO_HP]`.
  - Comment that Hero-dungeon exception rows are out of scope (zero rows).

**`X2Lib/X2GUNPC.cpp` — `SetHardLevel` (`:7046`), base flag only.**
- If `IsExtraNPCStatReady() && HasExtraNPCStat(id) && !m_pNPCUnitInfo->m_bHasLegacyNPCStat`,
  compute the five base values with `CalculateExtraStatValue(ENST_HP/PHYSIC_ATT/MAGIC_ATT/
  PHYSIC_DEF/MAGIC_DEF, hardLevel, row.m_f…Value, dungeonID)`.
  - `dungeonID` comes from `g_pData->GetDungeonRoom()->GetDungeonID()` when
    `g_pX2Game->GetGameType() == GT_DUNGEON`, else 0.
  - The per-level term is 0 for these NPCs anyway (`m_HardLevel` is 0).
  - Then `SetNPCStat( hp * fHPScale, atkP, atkM, defP, defM )`, followed by the unchanged
    `bResetHP` tail.
- Otherwise run the original code path unchanged.
- **Accuracy/avoidance** (Phase 0 verdict 2). Also compute `ENST_ACCURACY`/`ENST_AVOIDANCE`
  at `hardLevel`. Put them into `m_AdditionalAccuracy`/`m_AdditionalAvoidance` with
  `SetBasicStatValue`: raw, no transform, since 1.0 is neutral.
  - This route runs after `Init`, so `Init`'s 1.0 reset (`X2GUNPC.cpp:2722`) has already
    happened.
  - Verify that no later reset fires. Known resets: `:2722` `Init`, `:30286` `InitCheat`,
    `X2GUUser.cpp:1515` (users). If one does, gate it under June's guard
    (`GUT_NPC && NPC has an extra row`, June `FUN_32271540`).
  - Don't store these values in the per-ID `m_stat`: accuracy is level-dependent.
  - Today's value for every NPC is 1.0. Undefining the flag must restore exactly that.

**`ReInitAtkAndDef_LUA` (`X2GUNPC.cpp:11461`, `SERV_SECRET_HELL`).** Same switch for attack
and defense at `m_HardLevel`. Otherwise extra-only Secret NPCs drop to 0 attack/defense
when their script calls it.

**`InitComponent` (`:3989`).** No change. Its `SetNPCStat(kNPCUnitStat)` HP/attack/defense
are overwritten by the next `SetHardLevel`. Its accuracy/avoidance are reset to 1.0 by
`Init` (`:2722`) and then set by `SetHardLevel` (see above). It still applies the `DEF_*`
and scan fields from `m_stat`. **Verify** every creation path reaches `SetHardLevel`:
- `SwapToGameNPCUnit` → `SetNpcHardLevel` (`X2DungeonSubStage.cpp:384`);
- battlefield `:782/:987`;
- `CX2Game::CreateNPC` → `SetNpcHardLevel` (`X2Game.cpp:6286`).

**Diagnostics** (`SERV_IRUHADEV_EXTRA_NPC_STAT_DEBUG`), via `CX2OfflineLog::Server`, once
per NPC ID per game:
- `[XSTAT] npc=%d lv=%d dungeon=%d src=extra|legacy noHL=%d hp=%.0f atkP=%.0f atkM=%.0f defP=%.0f defM=%.0f`
- `[NPC_ZERO_HP] npc=%d lv=%d dungeon=%d extra=%d legacy=%d` whenever `GetMaxHp() <= 0`
  after `SetHardLevel`. This catches NPCs summoned at runtime by scripts or triggers,
  which the static audit couldn't see.

**Build, deploy, verify.**
- Build and deploy per Starting context, then **ask before launching**.
- **Pre-pack run** (Phase 1's files not packed yet): the degrade line names the missing
  piece, and behaviour is unchanged.
- **Post-pack:** the summary line shows the expected counts, and Burned Forest is fixed.
  See Phase 5's Burned Forest checklist; run just that part here.

**Exit criteria.**
- Clean build, deployed and confirmed.
- The degrade path is seen in the log.
- After the user packs: the summary counts match, the Burned Forest gate dies
  `KILL_BY_USER` with `[XSTAT] … src=extra … hp=53654` (Burned Forest **Normal**), and stage 1
  loads only after that.
- `MODS.md` is updated.
- The Opus `/code-review` is done.

**Kickoff prompt**:
```
Continuing the Lanox ExtraNPCStat port. Read LANOX_EXTRA_NPC_STAT_PLAN.md
("Starting context", "Investigation ledger", "Phase 3"). Branch: mods/lanox-port.
Do not re-derive anything in the ledger. Phase 1 is done. Where Phase 0 has a verdict
in the ledger, apply it; where it doesn't, use the interim value the plan documents.

Your task: implement the BASE flag only (SERV_IRUHADEV_EXTRA_NPC_STAT, plus the
short-lived _DEBUG flag) exactly as Phase 3 specifies. Add the CX2UnitManager tables and
bindings; load NewNPCStatFormula.lua + NewNPCStat.lua after NPCStat in OpenNPCFiles with
a visible degrade path; add CalculateExtraStatValue as a replica of June FUN_32374ea0;
give extra stats in SetHardLevel and ReInitAtkAndDef_LUA ONLY to NPCs with no legacy row
(m_bHasLegacyNPCStat); add the [XSTAT]/[NPC_ZERO_HP] logs. No parity pieces; that's
Phase 4.

X2UnitManager.cpp/.h, X2GUNPC.cpp, X2Main.cpp are CP949: byte-level edits only, then
run file + git diff --stat after each. touch X2Lib/stdafx.cpp. Build, deploy to
X2_offline.exe, confirm size/mtime, and ASK me before launching.

Done when: the pre-pack degrade line is seen; after I pack data036.kom, the summary
counts match and the Burned Forest gate dies KILL_BY_USER with hp=53654 before stage 1
loads. Update MODS.md and the Phase 3 Status line.
```

---

## Phase 4 — C++ parity flag

**Status:** not started.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high, then an Opus `/code-review`. Use **Opus, high** for the whole
phase if Phase 0 left item 2 (accuracy/avoidance) or item 3 (`param_4`) open.

**Needs:** Phases 0, 2 and 3 done.

**Goal.** Under `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY` (nested inside the base flag), make
the **772** NPCs with a legacy row use formula stats too, exactly as June does.

1. **`KTDXLIB/Always.h`.** Append `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY`.
2. **`SetHardLevel` signature** (`X2GUNPC.h:2326`, `X2GUNPC.cpp:7046`):
   `SetHardLevel( int hardLevel, bool bResetHP, float fHPScale = 1.f, bool bNoHardLevel = false )`.
   - The default keeps every existing call site unchanged.
   - June also has an `fAtkScale`, but every June caller passes 1.0, so omit it and say so
     in a comment.
   - Store the last `bNoHardLevel` in a member, for `ReInitAtkAndDef_LUA`.
3. **Base values.** Drop the `!m_bHasLegacyNPCStat` condition under the parity flag: any
   NPC with an extra row takes formula base values.
4. **Per-level term.** `if( !bNoHardLevel )`, add `m_stat.m_HardLevel.* × (hardLevel ×
   fCorrectionStat)`. That's exactly June `FUN_327687b0`. The PvP correction code is
   unchanged.
5. **`SetNpcHardLevel` (`X2Game.cpp:18149`).**
   - In the **default dungeon branch only** (`:18197–18228`: the Henir/Secret `1.f` calls
     and the difficulty-scaled Normal/Hard/Expert `1.0f/1.2f/1.5f` calls), pass
     `bNoHardLevel = ( NULL != g_pData->GetUnitManager() ) &&
     g_pData->GetUnitManager()->HasExtraNPCStat( pNpcUnit_->GetNPCTemplet().m_nNPCUnitID )`.
   - Defence dungeon (`:18181`) and non-dungeon calls pass `false`, per June's
     `FUN_322f0a90`.
   - The `:18221` "other difficulty" call also passes `false`: June clears the flag there.
   - Other call sites follow Phase 0's call-site table:
     - battlefield `:782` (`CreateBattleFieldNpc`) stays `false`;
     - battlefield `:987` (`CreateBattleFieldNpcByCreatedInfo`) passes `HasExtraNPCStat`
       (June `FUN_32d539c0`);
     - the ally re-level `X2Game.cpp:14002` stays `false`.
6. **`ReInitAtkAndDef_LUA`.** Use the formula base for any NPC with an extra row, plus the
   per-level term unless the stored `bNoHardLevel` is set.
7. **`AddExtraNPCStat_LUA`.** Copy `DEF_*` over legacy rows too (June's `FUN_323752f0`
   overwrites them). **Keep the legacy `DAMAGE_TYPE`**: June never reads it (Phase 0
   verdict 3). Set the scan fields from the spawn's monster grade, exactly as the base flag
   does for extra-only IDs (50/1000 for grade 1 or 2, else 10/500), replacing the legacy
   `SCAN_RATE`/`SCAN_NEAR_RANGE`.
8. **Accuracy/avoidance.** Extend the base flag's accuracy/avoidance handling to overridden
   IDs (verdict 2). Their legacy `ACCURACY`/`AVOIDANCE` values are dead today; the effective
   value is 1.0.
9. **`[XSTAT]`.** Now shows `src=extra noHL=1` for dungeon NPCs with a legacy row. On
   battlefields it shows `noHL=0` for `:782` spawns and `noHL=1` for `:987` spawns with an
   extra row.

**Exit criteria.**
- Clean build, deployed.
- `[XSTAT]` for Mag Nom in Burned Forest **Normal** shows `src=extra noHL=1 hp=178848`. With the parity
  flag undefined, it shows `src=legacy`.
- A battlefield `_FD` monster with a legacy row shows `noHL` matching its creation path:
  `0` via `:782`, `1` via `:987`. Log the path in `[XSTAT]` so the two can be told apart.
- `MODS.md` is updated.
- The Opus `/code-review` is done.

**Kickoff prompt**:
```
Continuing the Lanox ExtraNPCStat port. Read LANOX_EXTRA_NPC_STAT_PLAN.md
("Starting context", "Investigation ledger", "Phase 4"). Branch: mods/lanox-port.
Do not re-derive anything in the ledger. Phases 0-3 are done.

Your task: implement SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY exactly as Phase 4 specifies:
add June's bNoHardLevel parameter to SetHardLevel (June FUN_327687b0); drop the
legacy-row exclusion; in SetNpcHardLevel's default dungeon branch only, pass
HasExtraNPCStat (June FUN_322f0a90); other call sites follow Phase 0's call-site
table (battlefield :782 stays false, :987 passes HasExtraNPCStat). Update
ReInitAtkAndDef_LUA. Copy DEF_* over legacy rows; keep the legacy DAMAGE_TYPE (June
never reads it). Scan fields come from the monster grade and accuracy/avoidance follow
Phase 0 verdict 2, both extended to overridden IDs.

CP949 byte-level edits + file/git diff --stat. touch X2Lib/stdafx.cpp. Build, deploy,
ASK before launching.

Done when: [XSTAT] shows Mag Nom src=extra noHL=1 hp=178848 in Burned Forest, and
src=legacy with the parity flag undefined. Update MODS.md and the Phase 4 Status line.
```

---

## Phase 5 — Play-test verification and parity go/no-go

**Status:** not started.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. Switch to **Opus, high** if a phase still skips with no
`[NPC_ZERO_HP]` line.

**Goal.** Prove the fix in every Lanox dungeon and field, then decide with the user
whether parity stays on. The user plays; you read the logs. Ask before every launch.

**Checklist:**
- **Load line:** the expected counts (847 usable, 75 extra-only, 772 with a legacy row,
  67 vars). Confirm them from the log, not from the fact that the code compiled.
- **Burned Forest Normal:**
  - Stage 0 / sub-stage 1: the gate (1694) is still alive while sub-stage 1's mobs are.
  - It becomes breakable only once they're dead, then `died: KILL_BY_USER`.
  - `stage 1 loading` follows the gate's death, not a `KILL_SELF`.
  - `[XSTAT] npc=1694 lv=70 … hp=53654` on Normal (×1.2 Hard, ×1.5 Expert).
- **Burned Forest stage 1:** traps 1702/1703 are alive after `stage 1 start` (no `KILL_SELF`
  at +0.1 s). Stage 1 sub-stage 2's gate behaves like stage 0's.
- **AshesVillage, Gateway, Geyser, FireTemple, FinalHall, Secret_Hell**, every
  difficulty available:
  - **Zero `[NPC_ZERO_HP]` lines.**
  - Every gate or switch gate dies `KILL_BY_USER` after its phase's mobs.
  - `KILL_SELF` alone proves nothing: some scripted NPCs die that way legitimately.
  - A phase that skips with no zero-HP line is a **new defect**. Open it in
    `LANOX_DEFECT_PHASES.md`, don't patch it here.
- **Lanox fields** (`Ranox_Field_00..03`): the 8 `NUI_LOW_GEYSER_*_FD` survive spawn in
  `Ranox_Field_01`'s zone. This closes Phase 7d's "unsourceable" bucket.
- **Old content (the parity decision):**
  - Play one early dungeon (Ruben/Elder) and one mid dungeon (Velder/Hamel). Compare
    `[XSTAT]` against Phase 2's table, and ask the user how it feels.
  - **Keep parity** or **undefine `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY`**. The stage-skip
    fix survives either way.
  - Record the decision here.

**Exit criteria.** Every item is checked and recorded here with its log excerpt, and the
parity decision is recorded.

**Kickoff prompt**:
```
Continuing the Lanox ExtraNPCStat port. Read LANOX_EXTRA_NPC_STAT_PLAN.md
("Starting context", "Investigation ledger", "Phase 5", and Phase 2's summary).
Branch: mods/lanox-port. Phases 0-4 are done and deployed.

Your task: run Phase 5's checklist with me. I play; you read offline_server.log
([XSTAT], [NPC_ZERO_HP], KILL_SELF, stage loading) after each session and tell me what
to play next. ASK before every launch. Any phase that still skips with no [NPC_ZERO_HP]
line is a new defect: open it in LANOX_DEFECT_PHASES.md, don't patch it here. At the
end, walk me through Phase 2's table vs what I felt in the old dungeons and record my
parity keep/revert decision.

Done when: every checklist item has a log excerpt recorded in Phase 5 and the parity
decision is written down.
```

---

## Phase 6 — Docs, cleanup, memory

**Status:** not started.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high.

1. **`LANOX_DEFECT_PHASES.md`.** Add a phase entry that points here, with a one-paragraph
   summary.
   - Correct Phase 7d: the "8 `NUI_LOW_GEYSER_*_FD` … no source data exists" and "35
     remain" notes are superseded, because the source is June's `NewNPCStat.lua`, not
     `NPCStat.lua`.
   - Update the ledger's "Proven root causes".
2. **`MODS.md`.** Final entry: the flags, what each does, and how to revert (parity alone,
   or both).
3. **Debug flag.** Comment out `SERV_IRUHADEV_EXTRA_NPC_STAT_DEBUG` in `Always.h` (keep the
   block with a "re-enable to diagnose" note), rebuild, and deploy.
4. **Memory.** Update the ExtraNPCStat memory with the final state (flags, parity
   decision).
5. **This document.** Set every Status line, and fold any open Phase 0 items into
   "Out of scope / follow-ups".
6. Commit only if the user asks.

**Kickoff prompt**:
```
Continuing the Lanox ExtraNPCStat port. Read LANOX_EXTRA_NPC_STAT_PLAN.md
("Starting context", "Phase 6", and every phase's Status). Branch: mods/lanox-port.

Your task: close out the port. Update LANOX_DEFECT_PHASES.md (new entry pointing here;
supersede Phase 7d's "Geyser _FD unsourceable" / "35 remain" notes, since the source is
NewNPCStat.lua). Finalize MODS.md. Disable the _DEBUG flag, then rebuild and deploy
(ASK before launching). Update the ExtraNPCStat memory, and set every Status line here.
Don't commit unless I ask.
```

---

## Out of scope / follow-ups

- **Lanox kills grant 0 EXP / 0 ED.** Every Lanox kill logs `+0 exp, +0 ED (key=33270 …) …
  no reward row`. That's a separate reward-table gap in the offline emulator, and it
  needs its own defect entry.
- **Hero-dungeon exception rows** (`Add_HeroDungeon_ExtraStatForException`, June
  `FUN_32373680` path). Zero rows in the data, and March has no hero dungeons.
- **The 836 NewNPCStat IDs March's enum doesn't know.** Skipped by design. They become
  relevant only if those NPCs are ported, and they'll pick up their rows automatically
  once they exist in `Enum.lua` and `NPCTemplet`.
- **Monster grade never reaches dungeon NPCs offline** (deferred by the user,
  2026-09-26).
  - The data has it. The Lanox dungeon scripts carry `MONSTER_GRADE`: 55 entries across
    all 19 `data035_kom/Dungeon_Ranox_*.src.lua`, including `Secret_Hell`. Some of those
    entries are commented out.
  - Live June's server read those entries into `KNPCUnitReq.m_cMonsterGrade`. The
    emulator leaves it at `MG_NORMAL_NPC` (`X2Lib/Offline/Handlers_Room.cpp:1855`),
    because March's client `NPCData` only parses the key under `X2TOOL`.
  - **What that costs, stats side.** Every Lanox boss gets normal-monster scan values
    (10/500 instead of 50/1000; Phase 0 verdict 3). The grade-keyed hero-dungeon table is
    irrelevant, since it has zero rows.
  - **What that costs, reward side (the result screen).**
    - The March server uses the grade for kill rewards: `CXSLUnitManager`'s monster-type
      factor (`MTF_BOSS_NPC`/`MTF_MIDDLE_BOSS_NPC`, `XSLUnitManager.cpp:240-270`), which
      scales EXP and drops.
    - It also uses the grade for the boss/middle-boss special drop (`KDropTable.cpp:1904`,
      items `70058`/`70057`, `SERV_KILL_BOSS_MIDDLE_BOSS_DROP_ITEM`,
      `ServerDefine.h:3236`), and for boss tracking (`DungeonMonsterManager.cpp:55-75`,
      `DungeonRoom.cpp:6016/6530`).
    - The emulator's dungeon kill path currently treats every dungeon NPC as normal. How
      much of this the result screen shows offline hasn't been checked.
  - **Fix.** Have the emulator parse `MONSTER_GRADE` from the dungeon script and set
    `m_cMonsterGrade`, honouring the commented-out entries as absent. Then plumb it into
    the client-side grade (Phase 3's scan fields) and the emulator's reward path.
- **Secret Hell `gameType 3`** (Phase 0 verdict 4, open; deferred by the user, 2026-09-26).
  - The port always uses type 2. At level 70 that means monster HP ×1.05 instead of ×1.5,
    and attack ×1.5 instead of ×2 (defence is 1.2 either way). So
    `Dungeon_Ranox_Secret_Hell` would be about 30% less HP and 25% less damage than June,
    if that dungeon is June's type-3 case.
  - **Next step is an Opus RE pass on June event `0x50a`:**
    - find every reader of `CX2DungeonGame+0xdd0`/`+0xdd1`;
    - find what the client does just before the packet arrives (the `0x508` case goes to
      `327cbb80`).
  - If it proves to be hell mode, set the flag from something March already knows (the
    dungeon ID or mode). Don't try to send a June-only event March doesn't have.
