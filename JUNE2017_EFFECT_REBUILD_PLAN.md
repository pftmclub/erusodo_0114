# June2017 EffectSet / DamageEffect / DamageData Rebuild

This plan updates the March-2014 client's effect engine to the June2017 one. It covers
`CX2EffectSet`, `CX2DamageEffect::CEffect` and all of its sub-blocks, and
`CX2DamageManager::SetDamageData` plus the hit path. The source is the June2017 client
binary, **read instruction by instruction in Ghidra**, not inferred from the data files.

This document is self-contained. To run a phase, paste its section together with the
"Starting context" block below into a fresh conversation, then paste its **Kickoff
prompt**. Phases are sized for one conversation each and are meant to run in order,
because each one consumes the previous one's outputs.

**Why this exists.** The Ranox/Lanox port ships June2017 content on a March-2014 engine.
Every engine gap so far was closed by inference, because `RANOX_EFFECT_ENGINE_GAPS.md` §6
said the June `x2.exe` was packed and had "zero hits". Two examples:

- Phase C's `ATTACH_SKIN_MESH` reconstruction, including a `VALUE=0.01` nobody can explain.
- The open "projectiles render but hit nothing" questions in `RANOX_EFFECT_RUNTIME_UNKNOWNS.md` §4.

On 2026-09-23 it turned out that an **unpacked** dump of that binary exists and is loaded
in Ghidra. The missing strings were never missing: they are XOR-encrypted at compile time,
and they decrypt mechanically. So the June engine can now be read rather than guessed.

**Fix philosophy (the user's, and this plan's whole point):** *no more guessing code.*

- Every behaviour added here is transcribed from a specific June function and cites it.
- Where the binary cannot settle a question, the answer is to read more of the binary. It
  is never a plausible default.
- Where June depends on a system this build does not have at all (Rose weapon, memo,
  natural force, numerical skill data), the key is **parsed, stored and logged as
  unsupported**. No behaviour is faked for it.
- The standing CLAUDE.md rules still hold: no hardcoded tables, no invented curves, and
  server-side Lua is the user's to pack.

**End goal (the user's, stated 2026-09-25).**
- The March engine should become as close to the June engine as possible, so that
  later June features port with little friction. This plan is the first subsystem done
  that way: a trial of the approach, on effects.
- For effects, the target is to **adopt June's own files**: `EffectSet*`/`DamageEffect*`
  lua and the assets they reference. The Ranox-merged March copies are a stopgap to be
  retired, not maintained.
- Consequences for every phase:
  - Match June's behaviour, not just what the current merged data exercises.
  - **Character ports are out of this plan's census.** Before porting or updating any June character
    script, read `JUNE2017_CHARACTER_PORT_CHECKLIST.md` (numerical skill data, keys only character
    scripts use, enum tables that truncate a character file).
  - A log check that cannot fire on the merged data is settled at Phase 11, where
    June's data carries the keys. Phase 2's `ES_TEMPLET` check is the first case.

---

## Model and effort per phase (for the user; do not paste)

Decided 2026-09-26. The goal is to spend Opus only where a mistake is both likely and
silent. Each remaining phase also carries a one-line **Model:** note under its
**Paste with:** line.

Work in this plan is one of two kinds:

- **Identification.** Deciding which June function is which, by call graph and
  decrypted strings, or deciding whether an existing key's semantics changed. If this is
  wrong, everything built on it is wrong and still compiles. Use **Opus, high**.
- **Transcription.** Copying keys, defaults and branches out of a function that is
  already `confirmed` in `funcmap.tsv`, then doing the byte-level edit, build, deploy
  and doc updates. This is mechanical but needs care. Use **Sonnet 5, high**.

| Phase | Kind | Model / effort |
|---|---|---|
| 5: CEffect ctor keys | Transcription (addresses given) | Sonnet 5, high |
| 6a, 6b: sub-loaders | Transcription (roles mapped in Phase 0) | Sonnet 5, high |
| 7a step 1: identify OnFrameMove + callees | Identification | **Opus, high** |
| 7a step 2, 7b, 7c | Transcription, once 7a has confirmed the functions | Sonnet 5, high, then an Opus review |
| 7d: DamageReact, AttackResult | Behaviour is subtle | **Opus, high** |
| 7e: touch-land, hit effect set, die-create NPC | Transcription (identified in 7d), one argument mapping | Sonnet 5, high, then Opus review |
| 8a: existing keys, June semantics | Did the semantics change? | **Opus, high** |
| 8b: new keys, systems exist | Transcription | Sonnet 5, high |
| 8c: absent systems, enum tables | Deciding how to stand up an absent system | **Opus, high** |
| 9: collision / hit path | Identification plus root cause | **Opus, high**, whole phase |
| 10: MeshPlayer | Confirm the parser, then plaintext keys | Opus, high to confirm; then Sonnet 5, high |
| 12: docs and memory | Writing | Sonnet 5, high |

Rules:

- **Effort is always high.** Do not use medium or low on either model. Save by
  switching from Opus to Sonnet, not by lowering effort. Low effort is where the CP949
  check, `git diff --stat` or a default gets skipped without anyone noticing.
- **Max only to get unstuck, never by default.** Use it when an identification still
  won't settle after one high pass (for example, two candidate functions both look
  plausible), or when Phase 9's "renders but hits nothing" is still unexplained after a
  high pass. Go back to high once that one question is answered.
- **Haiku: not for this plan.**
- **Split mixed phases within one session.** Start on Opus. Do the identification step
  without writing code, and write the verdicts into `funcmap.tsv` (`confirmed`) and the
  matrix. Then run `/model` to switch to Sonnet in the same session for transcription.
  The findings are on disk and in context.
- **Close every Sonnet phase with an Opus review.** Run `/code-review` on the phase's
  diff on Opus. This is cheaper than an Opus implementation, and more independent
  because the reviewer did not write the code.
- **Stop a Sonnet session that makes its own identification call.** If it treats a
  function as "looks like the same one" and that function is not already `confirmed` in
  `funcmap.tsv`, switch to Opus for that step.
- **Context size costs as much as model choice**, because every turn re-reads the whole
  conversation:
  - Use one fresh session per phase.
  - Paste only "Starting context" plus the phase section, never the whole plan.
  - Ask Ghidra to decompile address ranges rather than dump whole-function disassembly.
  - Send broad searches to a subagent.
  - Run `/compact` when a phase moves from identification to transcription.

---

## Dev-console verification pass (for the user; do not paste)

Added 2026-10-03. This section runs every play-test the plan still owes, from the in-game
developer console. The console sets up each scene, so a test no longer depends on finding the
right class, dungeon or monster. It is a checklist, not a phase, and it changes no code.

**What it covers.** These phases are built and deployed, but their play-test is still owed:

| test | phase | what is still owed |
|---|---|---|
| T0 | — | calibration: proves the raw-Lua route reaches the same code as a real skill |
| T1 | 6a, 7c | `DAMAGE_EFFECT_Trap_Fire_Big`: the `DDE_TEMPLET` line, a multi-spawn `LOOP_*`, `SET_DIE_WHEN_MASTER_EFFECT_DIE` |
| T2 | 7b | the `LOT_UID_VECTOR` re-target (`UID_VECTOR_RETARGET` / `SEED_PICK`) |
| T3 | 7d | the die-scale branch (`DIE_DAMAGE_EFFECT_SCALE`) |
| T4 | 8b | the regression run of the restructured `AttackResult` |
| T5 | 8c | the `JUNE_FX unsupported` lines that deployed data can produce |
| T6 | 9 | the Mag Charger fireball: the `JUNE_FX HIT … TEST` trace (diagnostic) |
| T7 | 10 | the mesh/particle tokenizer startup check, `MP_CRASH`, and alpha regressions |

Phases 2, 3, 4a, 4b stage 1, 5, 7a and 7e are already play-tested and are not repeated here.
T0 also serves as their regression check. What still cannot be tested is listed under "Not
testable, even with the console" below.

### Setup

1. **Turn the console on.** See `DEVELOPER_COMMANDS.md`, "Turning the console on". With the game
   closed, back up `els_db.sql` into `db_backup/`, then set
   `INSERT OR REPLACE INTO settings( name, value ) VALUES( 'auth_level', 3 );`. Level 3 is required
   because level 2 cannot run raw Lua lines. Log in, enter a dungeon or field, and press **END**.
2. **Record which exe you are testing.** On 2026-10-03 the deployed `X2_offline.exe` was
   19,015,168 B, SHA-256 `cccaf8fb28e7826d…` (PE timestamp `6ac0cf4d`), built on
   `mods/june-buff-port`. This line first said `1429d05783def6a2…`, which was wrong; the size was
   right. The matching `X2/US_SERVICE/X2.map` resolves crash addresses for this exe.
   - It carries this plan's `SERV_IRUHADEV_JUNE_*` flags and also the June buff-port flags
     (`SERV_IRUHADEV_JUNE_BUFF_*`). So a regression is not automatically this plan's.
   - The *Starting context* names `mods/lanox-port`, which is where these phases were built.
   - Write the hash into each result.
3. **Know which log each line goes to.** All three logs are in the game directory.

   | log | holds |
   |---|---|
   | `offline_server.log` | X2Lib run-time lines: `ES_*`, `CE_*`, `DDE_TEMPLET`, `JUNE_FX HIT`, `JUNE_FX unsupported` from DamageData |
   | `ErrorLog.txt` | load-time lines (`ES_LOAD`), and everything from KTDXLIB (`MP_CRASH`, mesh-player/particle `unsupported`) |
   | `offline_packets.log` | `UNHANDLED` / `EXCEPTION` |

4. **After every test** (`DATA` as in *Starting context*):
   ```sh
   cd "$DATA"
   grep -E "UNHANDLED|EXCEPTION" offline_packets.log | sort | uniq -c
   grep -a "JUNE_FX" offline_server.log | tail -60
   grep -a "JUNE_FX" ErrorLog.txt | tail -20
   grep -a "SHUTDOWN" offline_server.log | tail -1     # after quitting: expect "SHUTDOWN clean"
   ```

### Console toolkit

**Slash commands** (from `DEVELOPER_COMMANDS.md`, whose entries were not play-tested when it was
written). Strings must be in double quotes.

| command | use here |
|---|---|
| `/CM "NAME" count level` | spawns `NUI_<NAME>` monsters at your position. The loop is inclusive, so `count` 0 gives one monster. In game only |
| `/monhp n` | sets every *living* NPC's HP to n. Spawn first, then set |
| `/ai false` / `/ai true` | freezes / unfreezes all NPC AI |
| `/ak` | kills all NPCs |
| `/invin 0` | turns invincibility **on**. `/invin 1` turns it off |
| `/nocooltime` | removes skill cooldowns. It cannot be turned back off |
| `/fwarp 0` | prints the battle-field table. Then `/fwarp n` warps there |

**Raw Lua.** At auth 3, any console line that does not start with `/` runs as Lua.
- Each line is its own chunk, so `local` does not carry over to the next line. Use globals.
- A failed line prints only the generic invalid-command message. If a long line fails, split it
  to find the failing part.
- Every test below uses these five lines first:

```lua
jU = g_pX2Game:GetMyUnit()
jES = g_pX2Game:GetEffectSet()
jDE = g_pX2Game:GetDamageEffect()
jP = jU:GetPos(false)
jY = jU:GetLandPosition_LUA(false).y
```

- **Run all five again after every warp, stage change or new dungeon.** The first three hold raw
  pointers into the current game, and a stale one can crash the client. `jP`/`jY` hold the
  position where you stood, so re-run those two whenever you want an effect to start somewhere
  else.
- **A stale pointer is the likely cause of T3's 2026-10-03 crash.** It is not confirmed until the
  user says whether the five lines were re-run after entering the dungeon. CrashRpt's bundle
  (`%TEMP%\X2_*.tmp`, a zip holding `X2_offline.dmp`, `X2_offline.xml` and that session's logs)
  shows an access violation at RVA `0xCD3AD0`, which is LuaJIT's `index2adr`, reading address `0x8`
  with `idx = LUA_GLOBALSINDEX` and a NULL `lua_State`. The stack, read through `X2.map`, is
  `CX2GameEdit::ExecCommand` → `KLuabinder::DoString` → `CreateInstance_LUA` →
  `CX2DamageEffect::CreateInstance` (+0x180, its first `m_LuaManager.BeginTable( name )`) →
  `KLuaManager::BeginTable` → `lua_getfield`. That fault comes before any templet-specific code, so
  the `CX2DamageEffect` itself was bad. `State.log` shows the session went from the battle field
  (21:49) into Burnt Forest (21:53), and nothing between T1 step 1 and T3 needed raw Lua. Each
  `CX2Game` owns its own `m_pDamageEffect`, so a `jDE` set in the field points at freed memory in
  the dungeon. The "not responding" afterwards is CrashRpt trying to send the report to KOG's
  server. **From T2 on, use `jC` below instead of the cached pointers.**
- **`jC`: create a damage effect with nothing cached.** Globals, including functions, persist
  between console lines, so define this once per session:

  ```lua
  function jC(n) local g = g_pX2Game local u = g:GetMyUnit() return g:GetDamageEffect():CreateInstance_LUA(u, n, u:GetPos(false), u:GetLandPosition_LUA(false).y) end
  ```

  Then `jC("DAMAGE_EFFECT_NAME")` looks up the game, your unit, its position and the land height
  each time it runs, at your current position. It returns the effect, so `jE = jC("…")` lets a
  later call on the same console line act on it. **Not yet tried.** The console's line-length
  limit is unknown. If the definition fails, the per-call form is
  `g_pX2Game:GetDamageEffect():CreateInstance_LUA(g_pX2Game:GetMyUnit(), "NAME", jP, jY)`, with
  `jP`/`jY` re-run where you stand.
- If the `jY` line fails, use `jY = jP.y` instead, standing on the ground. Keeping the position in
  its own lines also tells a binding failure apart from a templet failure: if `jP`/`jY` worked
  and a create line fails, the templet is at fault.
- The two calls used below:
  - play an effect set: `jES:PlayEffectSet_LUA("EffectSet_NAME", jU)`
  - create a damage effect at `jP`:
    `jDE:CreateInstance_LUA(jU, "DAMAGE_EFFECT_NAME", jP, jY)`
- **Where these come from.**
  - The bindings are `X2Main.cpp:3851`/`:3852`/`:3877` (`GetDamageEffect`, `GetEffectSet`,
    `GetMyUnit`), `:4074` (`GetPos`), `:4594` (`GetLandPosition_LUA`, the user form), `:4664`
    (`CreateInstance_LUA`) and `:4688` (`PlayEffectSet_LUA`).
  - The call shapes match the shipped NPC scripts: `GEYSER_SCIENCE.lua` for the effect set, and
    `SECRET_MAGI_SUMMON.lua` for the damage effect.
  - `g_pX2Game:GetMyUnit()` from the console is proven: the 2026-10-03 buff-factor test used it.
  - **The rest is unverified.** T0 proves the effect-set call and T1's first step proves the
  damage-effect call; until then, treat any snippet that logs nothing as a possible binding
  problem, not a result.
- **Ownership.** An effect you create this way belongs to you, so it hits monsters, not you. An
  effect that belongs to another class may sit in an odd place, because a missing bone falls back
  to another position. The log lines are what count.

### T0: calibrate the console route (do this first)

**At launch, before entering a game**, check `ErrorLog.txt` for the Phase 2-3 load lines. This
re-confirms those phases on the current exe, which has changed since they passed, and confirms
you are reading the right `ErrorLog.txt` before T7 depends on it:
- `JUNE_FX ES_LOAD EffectSet.lua loaded=1 templets=3134 … effects=18528 …`;
- `UIEffectSet.lua` 52 / 211, `UIEffectSet_INT.lua` 6 / 29, `EffectSet_INT.lua` 46 / 196
  (templets / effects, Phase 3's table).

Then, in any dungeon or field, run the five setup lines, then:
```lua
jES:PlayEffectSet_LUA("EffectSet_Rena_Perfect_Storm", jU)
```
- **Expect** (`offline_server.log`) the line the real skill logged on 2026-09-26:
  `JUNE_FX ES_CREATE EffectSet_Rena_Perfect_Storm PERFECT_STORM BONE_TRACE_TARGET_MESH registered_index=0(damage)`.
- When the set ends, Phase 4b's pair should follow:
  - `ES_UPDATE … hidden=target_mesh_gone(321e0217)`;
  - `ES_DELETE … deleted=no_target_mesh_handle_left(321e4197)`.
- **Pass:** the `ES_CREATE` line appears. The console route then reaches the same code as the
  skill, and T1's first step does the same check for damage effects.
- **Fail:** stop using raw Lua. Run only the slash-command and real-skill parts below, and note the
  failure here.
- Result: **passed 2026-10-03** (user report, exe `cccaf8fb…`). The console route reaches the
  effect-set code. The client wrote its logs over between sessions, so the lines themselves were
  not kept. The crash bundle's `ErrorLog.txt` does keep the four `ES_LOAD` lines with Phase 3's
  counts (3134 / 18528, 52 / 211, 6 / 29, 46 / 196).

### T1: Phases 6a and 7c, `DAMAGE_EFFECT_Trap_Fire_Big`

**Step 1: the loop, from the console.** Run:
```lua
jDE:CreateInstance_LUA(jU, "DAMAGE_EFFECT_Trap_Fire_Big", jP, jY)
```

Expect these lines in `offline_server.log`:
- `JUNE_FX DDE_TEMPLET DAMAGE_EFFECT_Trap_Fire_Big CREATE_DAMAGE_EFFECT<n> LOOP_ADD_POS_X=…`. This
  is Phase 6a's exit line. It is throttled to one per second per templet, so seeing only `<n>`=0 is
  normal.
- `JUNE_FX CE_LOOP DAMAGE_EFFECT_Trap_Fire_Big CREATE_DAMAGE_EFFECT DAMAGE_EFFECT_Trap_Fire_Small spawn=i/N pos=(…) addPos=(…) dieWithMaster=1 …`.
  This is Phase 7c's exit line. Per entry, from the deployed `DamageEffect.lua`:

  | entry | spawns N | `addPos` |
  |---|---|---|
  | `CREATE_DAMAGE_EFFECT0` | 5 | (200, 0, 0) |
  | `CREATE_DAMAGE_EFFECT1` | 7 | (−200, 0, 0) |
  | `CREATE_DAMAGE_EFFECT2` | 6 | (150, 23, 0) |
  | `CREATE_DAMAGE_EFFECT3` | 6 | (−200, 40, 0) |

**Pass:**
- Each entry logs N lines, all with `dieWithMaster=1`.
- Within one entry, consecutive `pos` values differ by exactly `addPos`. June computes base +
  `addPos` × spawn counter (`MODS.md`, Phase 7c).
- The four entries interleave, because June spawns once per entry per frame.

**Step 2: `SET_DIE_WHEN_MASTER_EFFECT_DIE`.** This step needs the trap NPC.
- Only an explicit destroy takes the children with it, and that destroy is the trap's own Lua
  `StopEffectSet`, every 5 s. A console instance never gets one.
- Run `/invin 0`, then `/CM "TRAP_FIRE_BIG" 0 1`. Or use the real trap in Ranox Burned Forest.
- Expect the step-1 `CE_LOOP` lines on every cycle.
- No log line marks the drain itself. The evidence is `dieWithMaster=1` plus what you see: the
  small flames go out at the same moment as the big one, instead of burning on to their own end.

**Not covered:** the die conditions (`DIE_CHECK_CONDITION`). No deployed templet uses them (0 hits
on 2026-10-03), so they wait for Phase 11.

**Record in:** the Phase 6a status ("Not yet play-tested this session") and `MODS.md`'s Phase 7c
"Play-test pending" line. Phase 7c has no Status line in this plan yet; add one with the result.
Result: **step 1 passed, step 2 passed, 2026-10-03** (user report, exe `cccaf8fb…`). Step 1 also
proves the console's damage-effect call. Recorded in the Phase 6a status, the new Phase 7c status
line and `MODS.md`. The log lines were not kept (see T0).

### T2: Phase 7b, the UID-vector re-target

**The 2026-10-03 run could not have passed, and the reason is in the skill code, not Phase 7b.**
- Both skills this test used to name *change the lock-on type after creating the effect*:
  - Rena's Guided Arrow (`LESI_SI2_LE_GUIDED_ARROW`, `CreateGuideArrow`, under
    `BALANCE_GRAND_ARCHER_20121213`, defined at `Always.h:2140`) creates 3 `GUIDED_ARROW`s (4 with
    the memo), then calls `SetLockOnType( LOT_NEARST_UID_VECTOR )` on each.
  - Chung's Scare Chase (`X2GUChung.cpp:13423`) passes `LOT_NEARST_UID_VECTOR` to `SetLockOn`.
- So their templets' `LOT_UID_VECTOR` never runs. They take `LOT_NEARST_UID_VECTOR`, which
  Phase 7b left as March's and which logs nothing.
- The templet list is now checked: 69 deployed templets mention `LOT_UID_VECTOR`, including both of
  these. The skill code, not the templet, decides.
- The re-target also needs the target **still in the unit list at 0 HP** while the effect flies.
  If the NPC is already gone, the branch exits before its dead check.

**Console route** (not yet tried). `CEffect:SetLockOnNPCUID`, `CX2Game:GetNPCUnit`,
`CX2GUNPC:GetUID` and `CX2GameUnit:SetNowHP` are all bound (`X2Main.cpp:4974`, `:3896`, `:4273`,
`:4081`). `CX2GUNPC` inherits the `CX2GameUnit` methods in Lua (`class_inh`, `:4196`). One console line can therefore create a `GUIDED_ARROW`, aim it at a monster and drop
that monster to 0 HP before the arrow's next frame.
1. In a dungeon, define `jC` (*Console toolkit*), then `/CM "RAGE_BABYFIG" 2 70` (three monsters, level
   70 so that a stray hit does not kill them first)
   and `/ai false`. **Then walk about one screen-width away, facing them.** `GUIDED_ARROW` has
   `TIME_UNLOCK_FROM_START = 0.3`, and lock-on processing (the re-target included) returns early
   until that runs out (`X2DamageEffect.cpp:3563`). An arrow that reaches its target in under
   0.3 s never gets to the dead check. Do not walk so far that the monsters leave the June pick's
   owner range (1000 on a non-raid battle field, effectively unlimited in a dungeon).
2. Run, as **one** line:
   ```lua
   jID = g_pX2Game:GetNearNPCUnitUID_LUA(g_pX2Game:GetMyUnit():GetPos(false), 1683, 9999) jN = g_pX2Game:GetNPCUnitByUID(jID) jE = jC("GUIDED_ARROW") jE:SetLockOnNPCUID(jID) jN:SetNowHP(0)
   ```
   - 1683 is `NUI_RAGE_BABYFIG`'s NPC ID (`offline_server.log`, `died` lines). `GetNearNPCUnitUID_LUA`
     (`X2Game.cpp:13741`) returns the nearest living NPC of that ID, or 0.
   - **Corrected 2026-10-03.** The first version used
     `GetNPCUnit(GetNPCUnitListSize() - 1)`, which is wrong. `m_NPCUnitList` is a fixed table of
     `MAX_GAME_NPC_UNIT_NUM` slots, mostly NULL, so the last index is an empty slot. `jN` came back
     nil, and the line failed with "Wrong Command!" after the arrow had already spawned.
3. Repeat two or three times, re-spawning when the pack runs out.

**Expect** in `offline_server.log`:
- `JUNE_FX CE_LOCKON GUIDED_ARROW UID_VECTOR_RETARGET reason=dead seed=<s> -> uid=<n> user=0`.
  On a console instance the seed is the templet's own value (`m_iRandomSeedFromLua`,
  `X2DamageEffect.cpp:8059`), because no skill code calls `SetLockOnRandomSeed`.
- then `… SEED_PICK seed=<s> key=<k> eligible=<m> index=<i> -> uid=<n>`.
- With no other monster left, `eligible=0 … -> uid=-1` still proves the branch ran.

**Pass:** at least one such pair. Record in the Phase 7b status ("Not exercised yet").
Result: **not reached 2026-10-03.**
- First run: Rena's Guided Arrow switches to `LOT_NEARST_UID_VECTOR` (see above).
- Second run (console route, 22:39, exe `cccaf8fb…`): the one-line create, lock-on and `SetNowHP(0)`
  worked. Four arrows hit (`COLLIDE` on NPC uids 26, 27, 1 and 28), but there was no `CE_LOCKON`
  line.
  - The user stood close, as the old step 1 said, so each arrow hit within the 0.3 s
    `TIME_UNLOCK_FROM_START` window, before lock-on runs. Each target died at the arrow's hit
    (`died: KILL_BY_USER` about 30 ms after `COLLIDE`), not at `SetNowHP(0)`.
  - Retry from about a screen-width away (step 1 is now fixed).
- Third run (22:44-22:45): no `GUIDED_ARROW` line at all.
  - The three level-1 test monsters (uids 23-25) all died at 22:44:57.5, right after a
    `DAMAGE_EFFECT_SIEE_POISON_BURST` hit. That was before any arrow, so the line had nothing to
    aim at.
  - Next try: spawn them at level 70 (`/CM "RAGE_BABYFIG" 2 70`) so a stray hit cannot kill them,
    and do not attack before running the line.
- Fourth run: "Wrong Command!", because the old line's `GetNPCUnit(size - 1)` returns an empty slot
  (corrected above).
- **Fifth run: PASSED 2026-10-03** (22:57-22:58, exe `cccaf8fb…`, `GetNearNPCUnitUID_LUA` line):
  ```
  [22:57:49.179] GAME     NPC uid=5 id=1683 lv=70 died: KILL_SELF
  [22:57:49.487] JUNE_FX CE_LOCKON GUIDED_ARROW SEED_PICK seed=-1 key=0 eligible=12 index=0 -> uid=1
  [22:57:49.487] JUNE_FX CE_LOCKON GUIDED_ARROW UID_VECTOR_RETARGET reason=dead seed=-1 -> uid=1 user=0
  [22:57:51.635] JUNE_FX HIT GUIDED_ARROW COLLIDE defender=1 uid=1 collType=1 impact=(1381,548,-0) march=0
  [22:57:51.637] JUNE_FX CE_LOCKON GUIDED_ARROW SEED_PICK seed=-1 key=0 eligible=11 index=0 -> uid=2
  [22:57:51.637] JUNE_FX CE_LOCKON GUIDED_ARROW UID_VECTOR_RETARGET reason=dead seed=-1 -> uid=2 user=0
  ```
  - Five re-target pairs in all, with `eligible` falling 12 → 8 as monsters died.
  - The nearest ID-1683 NPC was one of the dungeon's own babyfigs (uid 5), not a `/CM` one, and
    `SetNowHP(0)` killed it (`KILL_SELF`).
  - **Every pick went to the dungeon's own NPCs** (uids 1, 2, 3), never to the `/CM` spawns. This is
    expected for a console instance. Its seed is the templet default `-1`, giving `key=0` and
    `index=0`, so the June pick always takes the first eligible NPC in list order, and the dungeon's
    NPCs occupy the low slots. Real skills call `SetLockOnRandomSeed( GetRandomInt() + offset )`, so
    their picks spread out.
  - Phase 9 side note: the `GUIDED_ARROW` hits mostly logged `DIFF june=1 march=0` again. One logged
    `DIFF june=0 march=1` (uid 25, 22:58:02.463), with a garbage `june_impact`: the June impact
    vector is printed uninitialised when June misses. That is cosmetic, but the case of June missing
    where March hits is new and worth a look if arrows ever pass through a monster in play.
- **Side finding for Phase 9:** all four hits logged `DIFF june=1 march=0`. June's hit test hits
  where March's would have missed (`march_impact=(0,0,0)`). This is the first real-data case of the
  port changing an outcome. It matches the sphere×line / line×sphere change. Not a defect unless a
  hit looks wrong in play.

### T3: Phase 7d, die scale

**2026-10-03: the client crashed before this test ran.** The crash is in the console's
`CreateInstance` call, before any templet code (see the stale-pointer note in *Console toolkit*).
So it says nothing about the die-scale branch, and nothing about `DamageEffect_SBH_Fire_Works`.
Its assets are all in the March data: mesh, three `EffectSet_A_SBH_FIREWORKS0n`, both shrapnel
children and the sound and explosion templets. Its skill-bound damage data (`BUFF_FACTOR_ID` with
`RELATIVE_SKILL_LEARN_CHECK`, `WAY_OF_SWORD_TYPE`) is guarded for a non-Elesis owner.

From the console (define `jC` first, in the place you are testing):
```lua
jC("DamageEffect_SBH_Fire_Works")
```
- **Expect** when it dies:
  `JUNE_FX CE_HIT DamageEffect_SBH_Fire_Works DIE_DAMAGE_EFFECT_SCALE DamageEffect_SBH_Fire_Works_Dying_Shrapnel entry=(0.660,0.660,0.660) parent=(…)`.
  It is throttled to one per second.
- If nothing logs, face a monster or a wall: some projectiles die only on contact.
- **Alternatives**, all in the deployed data with a non-1 die scale:
  - `DamageEffect_ENS_HA_Scud_Missile` (1.5);
  - `DamageEffect_SA_ALD_WOLF_Break3` (0.5);
  - `DamageEffect_ASD_Dragon_Falling_DOWN` (2, 2, 1);
  - or let `/CM "SMITH_RUNNER" 0 1` / `/CM "SMITH_ALUMINUM" 0 1` attack you (1.3).
- **If it crashes again even through `jC`**, the stale pointer was not the cause. Before
  relaunching, copy the newest `%TEMP%\X2_*.tmp` (the CrashRpt zip) somewhere safe; it holds the
  minidump. To read it, unzip it and run `python tools/mdscan.py X2_offline.dmp
  X2/US_SERVICE/X2.map`.
- **Pass:** a `DIE_DAMAGE_EFFECT_SCALE` line, and the child drawn at entry × parent.
- Record in the Phase 7d status ("Still owed: the die-scale branch").
  Result: first run **not run** (console crash, stale `jDE` suspected). Second run **passed
  2026-10-03** (22:41, exe `cccaf8fb…`, through `jC`, no crash):
  `JUNE_FX CE_HIT DamageEffect_SBH_Fire_Works DIE_DAMAGE_EFFECT_SCALE DamageEffect_SBH_Fire_Works_Dying_Shrapnel entry=(0.660,0.660,0.660) parent=(1.000,1.000,1.000)`
  appeared twice. The same templet that "crashed" ran cleanly through `jC`, which supports the
  stale-pointer diagnosis.

### T4: Phase 8b, regression

**What changed.** Phase 8b touched code that runs on every hit, but it changes nothing you can see
on this data. All five new keys default to false, and no deployed templet sets one. So this test
only checks that nothing broke, and **no log line is expected from it.** The two hot spots:
- the `RT_CREATE_EFFECT_SET` hit case: a damage effect that sticks an effect set onto whatever it
  hits;
- the `default:` hit case: every ordinary hit.

**Part A: an effect set created on hit.** 15 deployed templets use `RT_CREATE_EFFECT_SET`, and each
names its set in its own `CREATE_EFFECT_SET_NAME`, so a console spawn reaches the changed call.
1. In a dungeon, define `jC`, then `/CM "RAGE_BABYFIG" 0 1` (one monster) and `/ai false`. Stand
   a short step away from it, facing it.
2. Run `jC("AISHA_SUMMON_BAT_FLY")`. This is Aisha's bat, which flies forward into whatever is in
   front of you. Its set is `EffectSet_Aisha_Summon_Bat_Stick`.
3. Run `jC("ROSEBUSH_TRAP_GROUND")`. This is Rena's rose-bush trap, and its set is
   `EffectSet_ROSEBUSH_TRAP_ATTACH`. It is not known whether it reaches a monster standing still
   beside you, so if nothing happens, skip it.
4. Watch the monster each time. **Pass** if the stuck-on effect (the bat, or the rose bush) appears
   **on the monster's body**, not at your feet or somewhere else, and goes away by itself after a
   few seconds. **Fail** if it appears somewhere other than the monster, never goes away, or
   vanishes the instant it appears.
   - If nothing at all appears, step closer and retry before calling it a fail: the effect has to
     touch the monster.
   - If your Rena has the Rose Bush Trap skill, casting it on a monster checks the same thing
     through the real skill.

**Part B: ordinary combat.** Unfreeze with `/ai true`, `/CM "RAGE_BABYFIG" 4 1`, `/nocooltime`.
Then fight normally for a minute: your Z and X combos, a dash attack, a jump attack, and three or
four different skills, all landing on monsters. **Pass** if all of these hold:
- hit sparks appear where you hit;
- damage numbers pop as before;
- monsters flinch, get knocked down and get launched as before;
- your MP rises when you hit;
- nothing freezes.

**After both parts:** `grep -E "UNHANDLED|EXCEPTION" offline_packets.log` should print nothing new.
Report pass, or say what looked different and in which part.

- Record in the Phase 8b "Play-test checklist". Result: **passed 2026-10-03** (22:40, exe
  `cccaf8fb…`, user report plus log).
  - Part A: `AISHA_SUMMON_BAT_FLY` hit (`COLLIDE`). Then
    `JUNE_FX ES_CREATE EffectSet_Aisha_Summon_Bat_Stick Aisha_Summon_Bat_Stick POSITION_OFFSET facing=trace_target(321d9847)`
    was created on the monster, and the monster died at 22:40:11.468.
  - `ROSEBUSH_TRAP_GROUND` produced no line, which T4 allows (optional step).
  - Part B: normal combat looked as before.
  - No `EXCEPTION`. The only `UNHANDLED` is the usual `E_DISABLE_HB_CHECK_REQ`.

### T5: Phase 8c, unsupported lines

Run one line at a time:
```lua
jDE:CreateInstance_LUA(jU, "DamageEffect_DRYADKENTA_SpacialAttackB_Fire", jP, jY)
jDE:CreateInstance_LUA(jU, "NATURAL_FORCE_EXPLOSION_BOOM_COMBO", jP, jY)
jDE:CreateInstance_LUA(jU, "NATURAL_FORCE_EXPLOSION_BOOM_SKILL", jP, jY)
jDE:CreateInstance_LUA(jU, "LESI_COMBAT_RANGER_LESI_DOUBLE_JUMP_ATTACK_Z_UP_DAMAGEEFFECT", jP, jY)
jDE:CreateInstance_LUA(jU, "DOUBLE_JUMP_X_ARROW", jP, jY)
jDE:CreateInstance_LUA(jU, "DOUBLE_JUMP_XX_ARROW", jP, jY)
jDE:CreateInstance_LUA(jU, "DamageEffect_SA_RHB_Lockon_Stratos_Attack", jP, jY)
```
- **Expect:**
  - `JUNE_FX unsupported NATURAL_FORCE_TYPE in <templet>: … absent (value …)` for each of the
    first six;
  - `JUNE_FX unsupported NUMERICAL_DATA_INDEX in DamageEffect_SA_RHB_Lockon_Stratos_Attack: numerical skill data absent …`.
- Each line logs **once per key and templet per session**. Grep the whole log. To see a line
  again, restart the client.
- Keep the Rose templet (`Lockon_Stratos`) last. It sits on bones (`Dummy01`/`Dummy02`) that your
  class may not have, so it is the line most likely to misbehave.
- ~~Its `APPLIED_SKILL_ID` producing **no** line is expected.~~ **Wrong since `Enum.lua` was
  repacked 2026-10-03.** The buff port's `Enum.lua` carries June's `SKILL_ID`s, including
  `RNSI_SA_RHB_LOCKON_STRATOS = 31012`, so the Rose templet also logs two `APPLIED_SKILL_ID` lines
  (see the result).
- **Pass:** ~~seven~~ nine lines. Record in the Phase 8c status ("not play-tested yet").
  Result: **passed 2026-10-03** (exe `cccaf8fb…`, `offline_server.log` 22:00). All seven expected
  lines, plus:
  - `JUNE_FX unsupported APPLIED_SKILL_ID/_NOT_AFFECT_MEMO in DamageEffect_SA_RHB_Lockon_Stratos_Attack: applied-skill-ID system absent (31012/0)`
    (Phase 5's constructor line, `X2DamageEffect.cpp:1637`);
  - `JUNE_FX unsupported APPLIED_SKILL_ID in DamageEffect_SA_RHB_Lockon_Stratos_Attack: applied-skill-ID system absent (value 31012, table DAMAGE_DATA)`
    (Phase 8c's DamageData line, `X2DamageManager.cpp:211`).

  The Rose templet created no crash and no `EXCEPTION`. Three `CE_FRAME … USE_GLOBAL_TIME` lines
  (the two `NATURAL_FORCE_EXPLOSION_BOOM_*` and the Lesi double-jump templet) re-confirm Phase 7a.

### T6: Phase 9, the Mag Charger fireball (diagnostic, not pass/fail)

**Do not use `/invin`.** The test is whether the fireball hits you.
1. Run `/CM "MAG_CHARGER" 0 1`. Or walk to its own field, Ashen Land (`Ranox_Field_00`). `/fwarp 0`
   prints a table that runs only from Ruben to Sander (`DEVELOPER_COMMANDS.md`), so Ashen Land is
   probably not in it.
2. Stand in its line of fire and let it shoot several times.

**Capture:**
- three or four `JUNE_FX HIT DamageEffect_MAG_CHARGER_Attack_A TEST attacks=… a0{type=… linked=… c=(…) r=…} mesh=(…) defender=… near{d=… rsum=…} june=… march=…`
  lines (`offline_server.log`, one per second);
- any `JUNE_FX MP_CRASH …` lines (`ErrorLog.txt`);
- whether you were actually hit.

**What they settle:**
- whether the attack sphere follows the flying mesh: `linked=1` and `c` close to `mesh`;
- whether `d < rsum` when it visibly touches you;
- whether it crash-lands before it reaches you.

The collision math is already known not to be the cause, so read these lines rather than
re-reading code. Paste them into the Phase 9 status.

Result, **2026-10-03** (exe `cccaf8fb…`, a Ranox battle field whose meshes are `Field_Ranox_Stage2_*`,
`offline_server.log` 22:01-22:03):
- **The fireball hits the player.**
  - `JUNE_FX HIT DamageEffect_MAG_CHARGER_Attack_A COLLIDE defender=0 uid=12 collType=1 impact=(13073,3192,1033) march=1`
    at 22:01:48.191, and again at 22:01:59.726 (`impact=(13556,3160,1058)`).
  - `defender=0` is `GUT_USER` (`X2GameUnit.h:79`). Unit UID 12 is the player's character
    (`SELECT 'reyaa' (unitUID=12, class=103, lv=80)`).
  - The dying blast `DAMAGE_EFFECT_MAG_CHARGER_Dying` also hit (`june=1 march=1`, `d=191.6 < rsum=225.2`).
- **June and March agree on every call.** There is no `DIFF` line, and every `COLLIDE` says
  `march=1`. On this map, Phase 9's port did not change the outcome, and the original "renders but
  hits nothing" defect did not reproduce.
- **The hitbox follows the mesh.** Every `TEST` line has `linked=1` and `c` equal to `mesh`, with
  `r=26.0`.
- **It crash-lands on schedule.** `ErrorLog.txt` has
  `JUNE_FX MP_CRASH MAG_CHARGER_Attack_A LT_TIME t=1.264 notCheck=0.300 y=2788.0 land=2788.0`, two
  per shot, with identical values every time. `REFRESH_LAND_POS crash-land probe y == -1, skipped`
  follows each `TEST` line about 0.17 s later.
- **Unexplained, open:** the `TEST` samples come in two groups with the same x but different
  heights.
  - Near: `c=(12818,3064,1022)`, `d=183.6`, and `(13318,2864,1022)`, `d=289.4`.
  - Far: `c=(12818..12842, 1263..1526, 1846..1888)`, `d` 1543-2056.
  - The far group sits about 1700 lower and about 850 deeper than the near group.
  - The trace samples once a second per effect name, so these could be fireballs from a second
    Mag Charger in a lower or background layer, or one fireball's position late in its flight.
    It has not been settled.
  - `ENTER units=32` means 32 units were in the game.
  - Asked the user how many Mag Chargers were visible and whether any fireball fell away into the
    background.
- Phase 9's done criterion, "the Ranox projectile hits in a play-test", is **met, confirmed
  2026-10-03 23:0x**. A `/CM "MAG_CHARGER" 0 70` charger's fireball damaged the player, with
  invincibility off.
  - A `/CM "MAG_CHARGER" 0 1` (level 1) charger did 0 damage to the level-80 character even though
    `COLLIDE … march=1` logged (23:02:21, 23:02:23). That is the level gap, not a defect.
  - The fireball's own data is a normal attack (`DAMAGE = { MAGIC = 1 }`, `RT_BIG_DAMAGE`,
    `BACK_SPEED_X = 800`).
  - **For any damage test, spawn with `/CM … 0 70`, not level 1.** `Field_Ranox_Stage2` is the
  mesh-name prefix in `State.log`, not a battle-field ID.

### T7: Phase 10, tokenizer and alpha regression

No deployed `.txt` uses `ALPHABLEND`, `WIREFRAME`, `SLASH_TRACE_NOT_CULL` or `POSTEFFECT`. This
was checked on 2026-10-03 across `kom_contents/*_kom` and `march2014/update/`. So the new keys
cannot fire, and this is a regression check only.

**Pass:**
- No `Unexpected "…" in Sequence Block!` MessageBox appears at launch.
- `ErrorLog.txt` has no `JUNE_FX unsupported POSTEFFECT`.
- Through T0-T6, effect meshes look as they did before.
  - `ALPHABLEND` defaults to true and now decides alpha for every mesh-player templet, in place
    of `IsAlphaObject()`.
  - An effect that has turned opaque or see-through is the thing to report.
- T6's `MP_CRASH` lines also cover Phase 10's diagnostic.

Record in the Phase 10 status ("play-test pending"). Result: **passed 2026-10-03** (user report,
exe `cccaf8fb…`). There was no MessageBox, no `unsupported POSTEFFECT` and no alpha regression
seen through T0-T6. `MP_CRASH` fires; see T6.

### Not testable, even with the console

Do not count these as passed. Do not build synthetic templets from the console to reach them,
for example with `AddEffectSetTemplet_LUA`: that is guessing, and the user decided these wait for
June's data.
- **Waits for Phase 11, because no deployed data uses the keys:**
  - Phase 2's `ES_TEMPLET` and Phase 3's `ES_EFFECT`;
  - 4a stage 2 (`USE_IS_RIGHT_WHEN_CREATE`, `TRACE_MATRIX_IN_FIELD`,
    `APPLY_DAMAGE_EFFECT_VELOCITY`, `MESH_TRIGGER_DATA_LIST`);
  - 4b stages 2-3;
  - 6b's `LOD_TEMPLET` (re-checked in 7b);
  - 7b's `INIT_TARGET` group;
  - 7c's die conditions;
  - 7e's `HIT_EFFECT_SET` / `DIE_CREATE_NPC`;
  - the behaviour of the five 8b keys.
- **`GROUP_NAME`: never.** No March or June file uses it.
- **4b's pet and square-unit `GetCombineMatrix` branches.**
  - `PlayEffectSet_LUA` takes a `CX2GameUnit*`, and pets and square units are not game units.
  - No deployed set with `TRACE_MATRIX` plays on them.
- ~~**Phase 5's `APPLIED_SKILL_ID` line: never on this data, with or without Rose.**~~
  **Corrected 2026-10-03.** That held only while March's `SKILL_ID` lacked
  `RNSI_SA_RHB_LOCKON_STRATOS`. The `Enum.lua` packed for the buff port defines it as 31012, and T5
  logged both the Phase 5 and the Phase 8c `APPLIED_SKILL_ID` lines. Only the *system* behind the
  key, the applied-skill-ID lookup, is still absent.
- **7a's June-only branches.** Their users are Ain and Rose skills, and those classes are absent.
- **Phase 10c / touch-land:** not built.

---

## Starting context (paste this with any phase)

- Repo root: `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk`
- Branch: `mods/lanox-port`
- Game directory (deploy target, logs, save):
  `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`
- **June2017 reference client:** `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\`
  - Binary: `unpacked_x2.exe`, 43,801,600 B, SHA-256
    `f2376a7210a6245b68ec772afbf0dd3d89a9f52a5b66f47bc739a7ad1c462af1`. Imagebase
    `0x31aa0000`; for `.text`, VA = raw + 0xc00 + imagebase. **Not** `x2.exe` next to it,
    which is the packed original.
  - Scripts: `data036_kom/` (XOR'd and compiled; decompile with elstools, see below).
    `GameMajorXMeshPlayer*.txt` / `UIMajorXMeshPlayer*.txt` in the same directory are
    plaintext.
- **RE scratch directory: `june2017_re/`.** Read its `README.md` first. It holds:
  - The confirmed/candidate June function map.
  - The XOR-string idiom and its hand-verified controls.
  - The prototype decryptors, with their known flaws.
  - The method notes (dead ends).
  - `regen.sh`, which rebuilds the gitignored Ghidra dumps and the decompiled June lua.
    Run it with `sh regen.sh`, because the exec bit isn't kept.
- **Ghidra** (`.claude/skills/ghidra/SKILL.md`, driven with curl, no MCP):
  - Project `F:\elsword stuff\els_jun_2017_RE\els_jun_2017.gpr`. The user must have it
    open with the GhidraMCP server running.
  - Health check:
    ```sh
    curl -s --noproxy '*' -m 10 "http://127.0.0.1:8080/methods?limit=1"
    ```
  - **Quote the `'*'`.** Unquoted, the shell globs it and you get a Cloudflare 302 instead
    of Ghidra. Inside a variable run as `$C`, quotes don't help. The skill file therefore
    uses `--noproxy 127.0.0.1`, since Phase 0.
  - If the check fails, stop and ask the user to start Ghidra and the server. Don't loop.
  - `decompile_function` fails on large functions (the CEffect ctor and up). Ask the user
    to raise Edit → Tool Options → Decompiler → timeout and max payload. Until then,
    `disassemble_function` always works.
  - `get_function_by_address` needs an entry point. To find the function that contains an
    arbitrary address, use `xrefs_to` on it.
  - **Mutations** (rename, comment, retype) edit the user's Ghidra database. Say what will
    change and get an OK first.
- **Lua tooling:** `elstools/elstools.exe`.
  - **Check `june_src_fixed/data###_kom/<name>.src.lua` first.** A large share of the
    June tree has already been decompiled with the fixed decompiler (below) and is sitting
    there as plain `.src.lua` — only fall back to running the tooling yourself for a file
    that isn't already present.
  - `decompile-batch -i <file.lua>` writes `<file>.src.lua` next to the input. Copy June
    files into `june2017_re/june_lua/` first, so the June directory is not littered.
  - **For June (`KL\x82`) files, use `june2017_re/ljd_jun2017.py`** on the `.std.luac`
    that `decompile-batch` leaves behind. elstools 1.0.0 mis-maps KOG opcodes
    `0x2f`–`0x37` in both directions, so its decompile fails on any `for … in` loop, and
    `compile-batch -v jun-*` emits broken bytecode. `mar-2014` is unaffected. See
    `june2017_re/README.md`, "June LuaJIT opcode table". This is the same fixed decompiler
    `june_src_fixed/` was generated with; see `RANOX_LUA_DECOMPILE_FIX_PLAN.md`.
  - `compile-batch -v mar-2014 -i <file.lua>` compiles **in place**.
- **Client build** (default `US_SERVICE`, see CLAUDE.md *Toolchains*):
  ```sh
  MSBUILD="/c/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe"
  TRUNK="F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk"
  DATA="F:/elsword stuff/elsword_2014/els_2014/237311/24965799/data"
  MP="-m -p:MultiProcessorCompilation=true"
  "$MSBUILD" X2ServerProtocol/X2ServerProtocol_2010.vcxproj -p:Configuration=X2TOOL      -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" KTDXLIB/KTDXLIB_2010.vcxproj                   -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2Lib/X2Lib_2010.vcxproj                       -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  "$MSBUILD" X2/X2_2010.vcxproj                             -p:Configuration=US_SERVICE -p:Platform=Win32 "-p:SolutionDir=$TRUNK/" $MP
  ```
  - `touch X2Lib/stdafx.cpp` after any header edit, or the PCH silently discards it.
  - Deploy `X2/US_SERVICE/x2.exe` (lowercase) to `$DATA/X2_offline.exe`, and confirm the
    size and mtime programmatically.
  - **Ask the user before launching it, every time.**
- **Rules specific to this plan**, on top of CLAUDE.md, which auto-loads:
  1. **Cite the source.** Every new code path carries a comment naming the June function
     and address range it was transcribed from, e.g.
     `// June2017 FUN_31f6ac60 @ 31f6b2e0-31f6b3a4: LOOP_ADD_POS_* read as float, default 0`.
  2. **Re-express, don't transplant.**
     - Implement June semantics on March's classes, members and helpers.
     - June member offsets (`[ECX+0x2a4]`) describe June's struct layout. They say nothing
       about March's. Never port an offset.
     - Name new members after the Lua key, in the March class's own naming style.
  3. **A key name needs two independent sources.** Use a key only when the decryptor output
     *and* the plaintext key in a decompiled June `.lua` agree. A misspelled
     `PLUA_GET_VALUE` key compiles and parses nothing, silently.
     - Studio typos are real and must be kept verbatim if both sources agree, e.g.
       `USE_ADVANCE_COLLISIOIN_CHECK`, `ONYL_ATTACK_USER`,
       `CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME`.
     - A key the binary reads but no June data file uses is still implemented, marked
       binary-only in the matrix.
  4. **Flags.**
     - One `SERV_IRUHADEV_JUNE_<SUBSYSTEM>` per logical change, appended to
       `KTDXLIB/Always.h` after the last entry. At plan time that was
       `SERV_IRUHADEV_RANOX_EFFECT_DEBUG2` at line 4267; check for newer ones first.
     - Diagnostics go behind one shared, short-lived `SERV_IRUHADEV_JUNE_FX_DEBUG`. Emit
       them with `CX2OfflineLog::Server( L"JUNE_FX ..." )`: grep-able prefix, throttled
       to about once a second for anything per-frame.
     - `CX2OfflineLog::Server` is **not** open during every loading path; see
       `LANOX_DEFECT_PHASES.md`'s note. If a load-time line doesn't land, use
       `ErrorLogMsg` and say so.
  5. **Unsupported systems.** Parse and store the key, then log once per key per templet:
     `JUNE_FX unsupported <KEY> in <templet>: <system> absent`. No behaviour. The phase
     that introduces it records it in the key matrix as *unsupported*, with the missing
     system named.
  6. **Stop at the wire.** If a June feature needs a change under `KncWX2Server/Common/`
     (packet struct, event ID, shared enum), stop and record it. The servers cannot be
     built here (VS2003 is not installed).
  7. **Encoding.** Every engine file this plan touches is ISO-8859/CP949:
     - `X2Lib/X2EffectSet.{h,cpp}`, `X2DamageEffect.{h,cpp}`, `X2DamageManager.{h,cpp}`,
       `X2Main.cpp`, `KTDXLIB/KTDGXMeshPlayer.{h,cpp}`.
     - Edit at byte level only: Python `rb`/`wb`, anchored on exact bytes read with
       `python`/`cat -A`, not `cat`.
     - After every edit, check `file <path>` (encoding unchanged) and `git diff --stat`.
       A few lines of change should not produce hundreds, and there must be no deletions
       on a pure insertion.
  8. **Keep the key matrix current.** Every phase from 2 on updates
     `JUNE2017_EFFECT_KEY_MATRIX.md` (created in Phase 1): status changes from
     `to-implement` to `implemented` or `unsupported`, with the flag and the June citation.
     Update `MODS.md` for each new flag.
- **The census is the exit test.** There is no test suite. Phase 1 produces
  `june2017_re/census.py`, which re-derives "June keys minus implemented parse sites" from
  the source tree. A parser phase is done when its keys leave the `to-implement` bucket.
  A runtime phase is done when its cited branches show up in `JUNE_FX` log lines during a
  play-test.

---

## Investigation ledger — what is already proven (do not redo)

### The binary is readable

- `unpacked_x2.exe` is an unlicense-unpacked dump with intact RTTI and `__FILEW__` paths,
  e.g. `D:\SOURCE\INT\source\X2Lib\X2EffectSet.cpp` at `0x3347e460`.
- `RANOX_EFFECT_ENGINE_GAPS.md` §6 ("packed, zero hits") describes the *packed* `x2.exe`.
  It is outdated for this purpose; Phase 12 rewrites it.
- The Lua key literals and lua_tinker bind names are XOR-encrypted per string. The idiom,
  byte encodings and two hand-verified controls are in `june2017_re/README.md`:
  - buffer `0x3379c56c` → `MAIN_EFFECT_NAME%d`
  - buffer `0x3379c2b8` → `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`. Its last byte is
    initialised from a register and decrypted out of order, which is exactly the case the
    prototype gets wrong.
- About 7,086 strings were recovered by the prototype whole-binary scan
  (`june2017_re/xorstrings.tsv`), but the prototype has known flaws:
  - It drops the last character.
  - It merges adjacent buffers.
  - It garbles a few characters.
- Phase 0 replaces it. Until then, treat any decrypted name as a lead, not a fact.

### June function map

The full table, with evidence, is in `june2017_re/README.md`. Summary:

| June VA | March equivalent | Status |
|---|---|---|
| `321f28f0` | `CX2EffectSet::OnFrameMove` | confirmed (calls match `X2EffectSet.cpp:70-73`) |
| `321ee600` | `OnFrameMove_CreateEffect` | confirmed (`__FUNCTION__` string) |
| `321e4020` / `321e0080` / `321dbcc0` | `OnFrameMove_DeleteEffect` / `UpdateEffect` / `StartCameraShake` | confirmed (call order) |
| `321e48e0` | `AddEffectSetTemplet_LUA` (39 KB) | confirmed (bind table in `FUN_31d95080`) |
| `31f7aab0` | `CEffect::CEffect` (20 KB) | confirmed |
| `31f7fc60`, `31f7ff60` | `CreateInstance*` | confirmed (only callers of the ctor) |
| `31f71930` | `LoadLockOnData` (`LOCK_ON_DATA`) | confirmed (called by ctor) |
| `31f637e0` | `LoadBlackHoleData` (`BLACKHOLE_DATA`; its keys are `DRAGGING_*`) | confirmed (called by ctor) |
| `31f64200` | `LoadSinMoveData` (`SIN_MOVE_DATA`) | confirmed (called by ctor) |
| `31f73f30` | `LoadSoundData` (`SOUND%d`) | confirmed (called by ctor) |
| `31f6ac60` | `LoadDamageEffect`: the **`CREATE_DAMAGE_EFFECT%d`** loader (`LOOP_*`, `SET_DIE_WHEN_MASTER_EFFECT_DIE`, `INHERIT_*`) | confirmed (called by ctor) |
| `31f76a90` | **`TOUCH_LAND_DAMAGE_EFFECT`** loader (June-only) | confirmed (called by ctor) |
| `31f74a80` | `LoadAfterImage` (`AFTER_IMAGE`) | confirmed (called by ctor) |
| `31f66700` | die-condition parser (`DAMAGE_EFFECT_DIE_CHECK_CONDITION`, `CONDITION_%d`) | confirmed (called by ctor) |
| `31f68f30` | `LoadDieDamageEffect` per-entry body, via wrapper `31f6e7e0` (`DIE_DAMAGE_EFFECT`) | confirmed (ctor → `31f6e7e0` @`31f7e7f7`) |
| `31f6ceb0` | `LoadHitDamageEffect` per-entry body, via wrapper `31f6e850` (`HIT_DAMAGE_EFFECT`) | confirmed (ctor → `31f6e850` @`31f7e84d`) |
| `31f66e60` | `LoadParticleTable`, via `START_`/`PASSIVE_`/`TIME_`/`HIT_PARTICLE` wrappers | confirmed |
| `32632390` | `CX2DamageManager::SetDamageDataFromLUA` (11.9k disasm lines; this plan calls it "SetDamageData") | confirmed |

**Phase 0 corrected this table** (2026-09-24). Sub-table names turned out to be plaintext, and
each loader's role is pinned by the one it opens. `31f6ac60` is the `CREATE_DAMAGE_EFFECT`
loader, not the die loader. `31f68f30` is the **die** body, not the hit one. `31f76a90` is
`TOUCH_LAND_DAMAGE_EFFECT`, not passive particles. `june2017_re/funcmap.tsv` is
authoritative, with 46 rows of evidence; this table only summarises it.
| `31f88790` | **`CEffect::OnFrameMove`**, CEffect's **only** virtual (Phase 0 said deleting dtor: wrong, it ends `RET 0xc`) | confirmed (Phase 7a) |
| `31f79fb0` | `CX2DamageEffect::OnFrameMove` (calls `31f88790` as `vtable[0]`) | confirmed (Phase 7a) |
| `31f85910` | **`CEffect::AttackResult`**, not OnFrameMove: called at the end of EffectToUnit `3262b6e0` | confirmed (Phase 7d) |
| `31f6f600` | `CEffect::DamageReact` (March's line for line) | confirmed (Phase 7d) |
| `31f83aa0` | `CEffect::OnFrameMove_LockOnData` | confirmed (Phase 7a) |
| `31c8ae10` | KTDGXMeshPlayer `ProcessTempletBlock` (the `.txt` templet parser; tokenizer `31c85840`, events `31c85350`). Neither Phase 0 candidate: `31c8d140` is the `CXMeshInstance` ctor, `31c895d0` is probably `ChangeXSkinMesh` | confirmed (Phase 10) |
| `31fbe4a0` | — | **not ours**: its strings are UI-guide keys. Excluded |

Because CEffect's only virtual is the destructor, its runtime methods cannot be found
through a vtable. Find them from the call graph of
`CX2DamageEffect::OnFrameMove` (March `X2DamageEffect.cpp:84`), which loops over
instances calling `CEffect::OnFrameMove`.

### Keys June reads that March does not (partial, from the prototype decryptor)

These lists are **leads** until Phase 0 and Phase 1 confirm them against June lua. They
are here so the size of each phase is understood.

- **CEffect ctor:**
  - `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`, `ONLY_RELATIONSHIP`
  - `INHERIT_CRITICAL_DAMAGE_RATE`, `INHERIT_CRITICAL_RATE`
  - `TRACE_OWNER_BONE`, `TRACE_OWNER_BONE_ONLY_ROTATE`
  - `FOLLOW_LINE_GRAVITY`, `FOLLOW_LINE_WITH_CUSTOM_ROTATE`
  - `REACT_ANIMSPEED`, `REFRESH_LAND_POS`, `USE_GLOBAL_TIME`, `USE_OWNER_ATTACK_SPEED`
  - `IGNORE_MI_HUGE`, `USE_ADVANCE_COLLISIOIN_CHECK`
  - `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`
  - `APPLIED_SKILL_ID`, `APPLIED_SKILL_ID_NOT_AFFECT_MEMO`
- **`CREATE_DAMAGE_EFFECT%d` (`31f6ac60`):**
  - `LOOP_INTERVAL`, `LOOP_END_COUNT`, `LOOP_END_TIME`, `LOOP_ADD_POS_X/Y/Z`
  - `SET_DIE_WHEN_MASTER_EFFECT_DIE`
  - `INHERIT_LIFE_TIME`, `INHERIT_DAMAGE`, `INHERIT_BUFF_FACTOR`
  - `SET_GROUP_ID`, `AUTO_DIE`, `APPLY_ROTATED_OFFSET`, `APPLY_ROTATE_ORIGIN`
- **EffectSet parser (`321e48e0`):**
  - Templet level: `SYNC_SHOW_BY_CREATE_UNIT`, `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`,
    `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`, `GROUP_NAME`, `APPLIED_SKILL_ID`,
    `OVERLAP…`, `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, `NOT_ALLOW_MEMO`
  - Per effect: `POSITION_OFFSET_USE_UNIT_IS_RIGHT`, `TRACE_MATRIX_EXCLUDE_SCALE`,
    `TRACE_MATRIX_IN_FIELD`, `FOLLOW_UNIT_ROTATION`, `FOLLOW_UNIT_ROTATION_C…PARTICLE`,
    `USE_IS_RIGHT_WHEN_CREATE`, `FOLLOW_UNIT_VIEWER_SCALE`,
    `USE_EFFECT_SCALE_IN_UNIT_VIEWER`, `BONE_TRACE_TARGET_MESH_INDEX`
  - Trigger sub-block: `TRIGGER_TIME`, `MOTION_NAME`, `PLAY_TYPE`, `TRIGGER_ONCE`,
    `STOP_TRACING_ON_TRIGGER`, `BLOCK_TRIGGER(_TIME)`,
    `EFFECT_SET_TOGGLE_VISIBILITY_TIMER`, `USE_MESH_TRANSITION`, …
  - Sound and camera-shake sub-blocks.
- **SetDamageData (`32632390`):** about 60 new keys. Examples:
  - `DIVISION_NUM/OFFSET`, `DAMAGE_SKILL_ID`, `REACT_TYPE`
  - `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z`, `CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME/DELAYTIME`
  - `TRACE_NEAREST_BONE`, `TRACE_UNIT_DIE_DELETE_EFFECT_SET`, `PVP_RATE`
  - `IGNORE_DEFENCE_*`, `STOP_TIME_ATT/DEF`, `HIT_ADD_MP_PERCENT`, `BUFF_TEMPLET_ID`
  - `DAMAGE_TRIGGER_CONDITION`, `DAMAGE_DATA_KEY_NAME`, `ONYL_ATTACK_USER`
  - `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`, `PAUSE_NPC_AI_TIME`
  - For absent systems: `NATURAL_FORCE_TYPE`, `ROSE_WEAPON_ATTACK_TYPE`, `HP_RATE_DAMAGE_*`,
    `NUMERICAL_DATA_*`, `APPLIED_MEMO_ID`
- **June-only enum tables** (`RANOX_EFFECT_ENGINE_GAPS.md` §2):
  `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, `DAMAGE_EFFECT_DIE_CONDITION_TYPE`,
  `HP_RATE_DAMAGE_TYPE`, `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE`, `ROSE_WEAPON_ATTACK_TYPE`.
  The same document's §3 also lists 413 June-only members of existing enums.

`RANOX_EFFECT_ENGINE_GAPS.md` §1's "zero unread EffectSet keys" was measured over the 196
Ranox blocks only. The keys above prove it does not hold for June's data as a whole, so do
not let it shrink scope.

### Dead ends and traps (do not repeat)

- The Ghidra `strings` endpoint only lists strings Ghidra already defined. It will
  **never** show an encrypted key, so it cannot prove absence.
- Plain grep for a key in `X2Lib` is not proof of absence either. Stringizing macros
  generate Lua key names (`CASE_BEHAVIOR_FACTOR`, `X2BuffFactor.cpp:9-22`). Use a
  bare-identifier or stem search.
- Parse `xorstrings.tsv` with a tolerant regex over `latin1`. Some decoded rows contain
  raw tab and newline bytes.
- Printing decoded UTF-16 through the cp1252 console raises. Sanitize first.
- March code landmarks, for orientation:
  - `X2EffectSet.cpp`: parser `:2209`; `OnFrameMove_*` `:77/:964/:1261/:1586`;
    `PlayEffectSet` `:1801`; `GetEffectPosition` `:2772`.
  - `X2DamageEffect.cpp`: ctor `:1240`; `CEffect::OnFrameMove` `:1836`; lock-on `:2752`;
    `DamageReact` `:3400`; `AttackResult` `:3485`; `CreateHitDamageEffect` `:5378`;
    `CreateDieDamageEffect` `:5626`; `Load*` `:6342…`.
  - `X2DamageManager.cpp`: `CollisionDataCheck` `:1740`.
  - `X2Main.cpp`: binds `:4655-4672`.

---

## Phase 0 — RE tooling: a decryptor you can trust, and the function map as data

**Status: done 2026-09-24.** Outputs are in `june2017_re/`: `xordecrypt.py` →
`xorstrings.tsv` (6,180 of 6,240 buffers `ok`; three controls, one of them wide),
`ljd_jun2017.py` (all 14 June files decompiled), `reconcile.py` → `reconcile.md`, and
`funcmap.tsv`. Read `june2017_re/README.md` for what changed in the function map.

**Paste with:** Starting context + this section.

**Goal.** Turn the prototype into a tool whose output later phases can cite.

**Deliverables** (all in `june2017_re/`):

1. **`xordecrypt.py`, a new decoder that works on raw bytes, not Ghidra text.**
   - Use a real x86 decoder: `capstone`, installed with `pip install capstone` after the
     user OKs it; Python 3.14 is on this machine and capstone is not installed. If the
     user declines, write a hardened pattern emulator of the idiom instead.
   - Track the guard, the init stores into the static buffer, and the per-byte (or
     per-word, for wide strings) load → XOR/NOT → store sequence. Emit one row per
     **buffer**, not per contiguous address run. Buffer boundaries come from each init
     block's store set, which fixes the "adjacent buffers merge" flaw.
   - Init stores can come **from registers** (`MOV [buf+n],AL` after `MOV AL,imm`), and
     the decrypt can be out of order, with stack-slot offsets shifted by interleaved
     `PUSH`es. The README's worked example of `0x3379c2b8` shows all three. Track register
     values through the init block, and order output bytes by `ESP`-adjusted destination
     slot, not by instruction order.
   - The terminator is the zero-register store. Do not include it, and do not drop the
     last real character.
   - Input: a function address list, or `--all` for a whole-`.text` sweep.
   - Output `xorstrings.tsv` with columns `use_va`, `func_va`, `buf_va`, `kind`
     (`ascii`/`wide`), `len`, `plaintext`.
2. **Controls.** The tool's first act is to decrypt both of these byte-exact. On any
   mismatch it exits non-zero.
   - buffer `0x3379c56c` → `MAIN_EFFECT_NAME%d`
   - buffer `0x3379c2b8` → `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT` (39 chars)

   Hand-verify one more control of your own: a **wide** string, from any function. Record
   it in the README the same way.
3. **Reconciliation.**
   - Decompile the June effect lua into `june2017_re/june_lua/` (copy first, then run
     `elstools decompile-batch`):
     - `DamageEffect*.lua`, `EffectSet*.lua`, `UIEffectSet*.lua`, `UnitViewerEffectSetTable.lua`
     - `Elsword_DamageData.lua`, `DamageSound.lua`, `Enum.lua`
   - Extract every table key those files use.
   - For each parser function in the function map (`321e48e0`, `31f7aab0`, the
     sub-loaders, `32632390`), compare its decrypted key set against the data-file keys.
   - Write `reconcile.md`, which lists the intersection, the binary-only keys and the
     data-only keys. Explain every data-only key: nested sub-table, key read by a
     different function, or decoder miss.
4. **`funcmap.tsv`** with columns `june_va`, `march_symbol`, `status`
   (`confirmed`/`candidate`), `evidence`. Seed it from `README.md`. Extend it by walking
   the CEffect ctor's sub-loader call block (`0x31f7e7b2`–`0x31f7f976`, in call order) and
   the four particle wrappers.
5. **Skill fix, with OK.** Quote `--noproxy '*'` in `.claude/skills/ghidra/SKILL.md`.
6. **Ghidra annotation, optional, with explicit OK.** Rename the confirmed functions
   (`June_CX2EffectSet_OnFrameMove`, …) and set plate comments with the decrypted keys at
   each decrypt site. Later phases' disassembly then reads by name. List the renames and
   get approval before posting any.

**Exit criteria.**

- The three controls pass.
- Every buffer's decoded length equals the number of bytes its init block stored
  (immediate *and* register stores), minus the terminator.
- `reconcile.md` shows no *unexplained* data-only keys for any parser function.
- No data-only key in `reconcile.md` is a one-character extension of a binary-only key
  (e.g. data `FOO_BAR` vs. binary `FOO_BA`). That is the signature of the
  dropped-last-character bug.
- Do **not** test for "no key is a prefix of another". Real pairs like
  `TRACE_OWNER_BONE` / `TRACE_OWNER_BONE_ONLY_ROTATE` exist.

**Kickoff prompt**:
```
Starting the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md in the
repo root in full -- "Starting context", "Investigation ledger", and "Phase 0" -- and then
june2017_re/README.md. Branch: mods/lanox-port.

Goal: no more guessing code. This phase builds the tooling every later phase cites.

First: check the June dump's size and SHA-256 against the plan, then run the Ghidra health
check with --noproxy '*' QUOTED. If Ghidra isn't reachable, stop and ask me to start it.

Your task is Phase 0. Build june2017_re/xordecrypt.py, a raw-byte XOR-string decoder with
per-buffer boundaries. Ask me before pip installing capstone; if I decline, write a
hardened emulator of the idiom instead. It must decrypt the two controls byte-exact
before anything else. Then decompile the June effect lua into june2017_re/june_lua/ (copy
the files first, never decompile in place in the 14JUN17 directory), reconcile the
decoder's per-function key sets against the keys the data actually uses, and write
reconcile.md and funcmap.tsv. Ask before editing .claude/skills/ghidra/SKILL.md, and ask
(with the full list) before writing any rename or comment into my Ghidra project.

No client code changes in this phase. Commit june2017_re/ when done, with my OK.
```

---

## Phase 1 — Key census and gap matrix

**Status: done 2026-09-25.** Outputs: `june2017_re/census.py` (about 3 minutes, all checks
pass, deterministic) → `JUNE2017_EFFECT_KEY_MATRIX.md` + `june2017_re/census.tsv`. Read
`june2017_re/README.md`, "census.py", before running it. Things the next phases should know:

- **Phase 0's CEffect "378 of 378" was a matcher bug.** The CEffect top level reads 58 of its
  378 data keys directly, and `reconcile.py` is fixed.
- **Keys are placed by the level June reads them at.** Many top-level CEffect keys (`HIT_GAP`,
  `ATTACK_TYPE`, …) and templet-level EffectSet keys (`START_TIME`, `ROTATE_Z`, …) are
  misplaced data that June ignores (`june-ignores`). Don't implement them at that level.
- **There is a fifth status, `june-ignores`.** It covers data June never reads: typos, dead or
  misplaced keys, positional entries, and keys under a table June never opens.
- **The census verifies absent-system verdicts:**
  - Nasod armor combo is compiled in, so its key is 8b work, not 8c.
  - Natural force is gated off (`ADD_RENA_SYSTEM`).
  - Skill memo is **on** under `US_SERVICE` (`ADD_MEMO_1ST_CLASS`).
- **Scopes no phase owns** are listed in the matrix's "Open questions": nested `BUFF_FACTOR`,
  `Elsword_DamageData.lua`, and the EFFECT_TOOL-only `EffectSet_Description.lua`.

**Paste with:** Starting context + this section.

**Goal.** Make one authoritative table of every June key in scope, and a script that
re-derives it, so each later phase has a measurable finish line.

**Deliverables.**

1. **`june2017_re/census.py`.** For each June key it records:
   - The June data files and templets that use it, with a count.
   - The June parse function and decrypt-site VA, from `xorstrings.tsv`.
   - The March parse site, if any, in `X2Lib`/`KTDXLIB`. Find it by quoted-literal *and*
     bare-identifier/stem search, because of stringizing macros.
   - The sub-table it lives in: `EFFECT%d`, `DAMAGE_DATA`, `DIE_DAMAGE_EFFECT%d`, trigger,
     sound, and so on. The same key name in two sub-tables is two rows.
   - A status: `implemented`, `to-implement(phase N)`, `unsupported(system)`, or
     `binary-only`.

   It must be re-runnable. Later phases use it as their exit check, so it must read the
   current source tree, not a cached list.
2. **`JUNE2017_EFFECT_KEY_MATRIX.md`** in the repo root. Generate it from `census.py`,
   group it by subsystem and sub-table, and assign each `to-implement` key to its phase
   below.
   - Where one March key's June *semantics* differ (default value, type, units), add a
     `semantics-differ` row. Phase 8a and the parser phases own those.
3. **Enum census.** For every June enum table and member that the in-scope data uses,
   record whether `march2014/data036/Enum.src.lua` has it. Also record which phase adds
   the C++ reader that makes the enum member meaningful. Enum members without a reader
   are *not* added. Phase 11 owns the enum data change.
4. **Absent systems list.** Collect the keys that belong to systems this build lacks
   entirely (Rose weapon, natural force, memo, numerical data, HP-rate damage, and any
   others the census turns up). Name the system for each key. For each system, record
   the evidence that it is absent, e.g. "no `CX2RoseWeapon*` class, no enum, no packet".

**Exit criteria.**

- Every key in the in-scope June data has exactly one status.
- `census.py --summary` prints the counts per status and per phase.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context", "Investigation ledger", "Phase 1") and june2017_re/README.md,
reconcile.md, funcmap.tsv (Phase 0's outputs). Branch: mods/lanox-port.

Your task is Phase 1: write june2017_re/census.py and generate
JUNE2017_EFFECT_KEY_MATRIX.md in the repo root. The census has to find March parse sites
by bare identifier / stem as well as quoted literal (X2Lib uses stringizing macros; a
zero-hit literal grep is not proof a key is unread). Classify every in-scope June key as
implemented / to-implement(phase N) / unsupported(system) / binary-only, assign phases
per the plan's phase table, and flag semantics-differ where a key March already reads is
typed, defaulted, or scaled differently in June's parser.

No client code changes in this phase. Commit the census script and the matrix, with my OK.
```

---

## Phase 2 — EffectSet parser, templet level

**Status: done 2026-09-25**, verified in a play-test. `ErrorLog.txt` showed:
- `JUNE_FX ES_LOAD EffectSet.lua loaded=1 templets=3134 with_june_templet_key=0`. 3134
  equals the count from `march2014/data036/EffectSet.src.lua`, so the packed `data036.kom`
  matches the tree.
- `UIEffectSet.lua` 52, `UIEffectSet_INT.lua` 6 and `EffectSet_INT.lua` 46, all loaded,
  none with a June key.
- No `ES_TEMPLET` or `unsupported` lines, as expected.
- The `ES_TEMPLET` check for data that actually uses the keys moves to Phase 11 (see
  below).
`SERV_IRUHADEV_JUNE_EFFECTSET_PARSE` (+ `SERV_IRUHADEV_JUNE_FX_DEBUG`). Things the next
phases should know:

- **The templet level is ten keys, not the list in *Scope* below.** June reads
  `NAME`, `EVENT_TIMER_TYPE`, `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`,
  `AFFECTED_BY_UNIT_SCALE`, `DELETE_SHAKE_COUNT`, `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`,
  `OVERLAP`, `APPLIED_SKILL_ID`, `SYNC_SHOW_BY_CREATE_UNIT` and
  `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`. `GROUP_NAME` and `NOT_ALLOW_MEMO` are not
  read here: `NOT_ALLOW_MEMO` is Phase 3's.
- **No deployed set uses any of the six new keys.** The 196 Ranox blocks carry none of
  them, and `march2014/data036/EffectSet.src.lua` has no occurrence at all. The three
  June sets that do use one and also exist in March
  (`EffectSet_KILL_ENEMY_WHEN_FULL_HYPER_MODE`, `EffectSet_ANCIENT_BONE_DRAGON_SLEEP`,
  `EffectSet_Wedding_Heart_06`) are March's own versions, without the key.
  - So the "`ES_TEMPLET` line for a Ranox set" criterion cannot fire on current data.
    **The user's decision: it is settled in Phase 11**, where June's own `EffectSet.lua`
    uses all six keys.
  - A `JUNE_FX ES_LOAD` summary line per script file shows that the block ran.
  - Don't port June's `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` sets to test it:
    `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION.AEACBO_*` indexes an enum table March's
    `Enum.lua` lacks (Phase 11), and the nil index would abort the whole chunk.
- **Load-time `JUNE_FX` lines go to `ErrorLog.txt`.** `EffectSet.lua` loads in
  `CX2StateStartUp::LoadData`, before the offline server opens `offline_server.log`
  (it opens on first connect).
- **census.py changed** so rule-5 parse sites don't count as evidence that a system
  exists; see `june2017_re/README.md`.

**Paste with:** Starting context + this section.

- **June source:** `FUN_321e48e0` (`AddEffectSetTemplet_LUA`), from its entry to the start
  of the per-`EFFECT%d` loop.
- **March target:** `CX2EffectSet::AddEffectSetTemplet_LUA` (`X2EffectSet.cpp:2209`) and
  `EffectSetData` in `X2EffectSet.h`.

**Scope.** Every templet-level key the matrix assigns to Phase 2. Examples:
`SYNC_SHOW_BY_CREATE_UNIT`, `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`,
`STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`, `GROUP_NAME`, `OVERLAP…`,
`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, `APPLIED_SKILL_ID`, `NOT_ALLOW_MEMO`.

**Method.**

1. Walk the June parser's templet section instruction by instruction. For each key, record:
   - the getter it calls (int, float, bool, string, or enum — find the getter function by
     its shape);
   - the default value (the value stored before or instead of the call);
   - the destination (June member offset, **for grouping only**);
   - any post-processing: clamp, scale, lookup.
2. Add March members named after the keys and parse them with March's own `LUA_GET_VALUE`
   family, with June's defaults.
3. Parse only in this phase. Runtime use belongs to Phase 4a/4b. A member that is parsed
   but not yet consumed gets a `// consumed in Phase 4x` note.
4. Behind `SERV_IRUHADEV_JUNE_FX_DEBUG`, log one summary line per templet load,
   `JUNE_FX ES_TEMPLET <name> <key>=<v> ...`, only for templets that set a non-default
   June key.

**Exit criteria.**

- `census.py` shows 0 Phase-2 keys still `to-implement`.
- A clean `US_SERVICE` build.
- After a deploy, and after asking before launch, `JUNE_FX ES_TEMPLET` lines appear for a
  named Ranox set that uses one of these keys. Pick one from the matrix.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" -- especially its numbered rules --, "Investigation ledger", "Phase 2"),
june2017_re/README.md, and the Phase-2 rows of JUNE2017_EFFECT_KEY_MATRIX.md.
Branch: mods/lanox-port.

Your task: transcribe the templet-level keys of June's EffectSet parser (FUN_321e48e0,
entry up to the per-EFFECT loop) into CX2EffectSet::AddEffectSetTemplet_LUA
(X2EffectSet.cpp:2209). For each key record getter type, default, and post-processing
from the disassembly, and cite the June address range in a comment next to the new code.
Parse only -- runtime use is Phase 4. Put it behind a new SERV_IRUHADEV_JUNE_EFFECTSET_PARSE
flag in KTDXLIB/Always.h, with the original code in #else where behaviour is replaced.
X2EffectSet.{h,cpp} are CP949: byte-level edits only, then check file + git diff --stat.

Build US_SERVICE, deploy to X2_offline.exe, then ASK me before launching. Done when
census.py shows no Phase-2 keys left and a JUNE_FX ES_TEMPLET line shows up for a Ranox
set that uses one. Update the matrix and MODS.md.
```

---

## Phase 3 — EffectSet parser, per-effect data and sub-blocks

**Status: done 2026-09-26**, verified in a play-test. Everything was done in one session,
so there is no resume VA.

`ErrorLog.txt` showed:

| File | `loaded` | `templets` | `effects` | `with_june_effect_key` | `sounds` |
|---|---|---|---|---|---|
| `EffectSet.lua` | 1 | 3134 | 18528 | 0 | 0 |
| `UIEffectSet.lua` | 1 | 52 | 211 | 0 | 0 |
| `UIEffectSet_INT.lua` | 1 | 6 | 29 | 0 | 0 |
| `EffectSet_INT.lua` | 1 | 46 | 196 | 0 | 0 |

- Every `templets=` count equals Phase 2's, so the new sub-table reads leave the parse of
  later templets intact.
- No `ES_EFFECT`, `ES_SOUND` or `unsupported` lines appeared.
- `18528` is exact, not one short of the source's 18,529 `EFFECT<n> = {` entries:
  - `EffectSet_PET_SNOWBUMP_Attack_C` defines `EFFECT16` twice
    (`march2014/data036/EffectSet.src.lua:126666` and `:126674`).
  - Lua keeps the second, so the set really has 25 effects.
  - This is a studio data quirk, and the stock game has it too.

**The user's decisions (2026-09-26):**
- **The `ES_EFFECT` criterion is settled in Phase 11,** as Phase 2's `ES_TEMPLET` was.
- **The three `ES.Description` rows** (`VERSION`, `DESC`, `NAME`) **are out of scope for Phase
  3 and settled later.**
  - March reads `EffectSet_Description.lua` only under `EFFECT_TOOL`, and the game never
    loads it.
  - Open lead: the census matched those keys to June `32378e20`, `32680da0` and
    `324ff4b0` by name alone. A short Ghidra look at them would show whether June's game
    client loads the file. If it does, those rows need an owning phase.
- The code went in under `SERV_IRUHADEV_JUNE_EFFECTSET_PARSE`, extending Phase 2's flag,
  with `SERV_IRUHADEV_JUNE_FX_DEBUG` for the log lines.
- `census.py` shows Phase 3 at 0 `to-implement` and 0 open `binary-only`. The 5
  binary-only rows are now implemented.
- 1 `unsupported` remains: the per-effect `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`.
- Every new read's `semantics` is `same`, with three exceptions, all `unchecked`:
  - `RATE`: its default is a register store, `ESI=0x64`. It was hand-verified as 100.
  - The two binary-only `ANIMTIME_ON_START_SQUARE/_UNITVIEWER` sub-table names.
- The only EffectSet-family rows left `to-implement` are `ES.Description`
  (`VERSION`/`DESC`/`NAME`). They are `unassigned` and EFFECT_TOOL-only (plan Phase 1 *Open
  questions*), the named exception to this phase's exit criterion.
- The clean `US_SERVICE` build is deployed to `X2_offline.exe`.

Things the next phases should know:
- **The `ES_EFFECT` criterion cannot fire on current data,** exactly as in Phase 2. No deployed
  set uses any Phase-3 key: `march2014/data036/EffectSet.src.lua` and the deployed
  `UIEffectSet.lua` were both checked.
  - The play-test evidence is the `ES_LOAD` line's new `effects=` count. It shows the
    per-effect block ran; `with_june_effect_key=0` and `sounds=0` are expected.
  - The per-effect line is settled at Phase 11, as Phase 2's `ES_TEMPLET` was.
- **One parse rule changes a consumed member:** `TRACE_MATRIX_EXCLUDE_SCALE` → `TRACE_MATRIX`.
- **Scope text below corrected:** the key is `ZOFFSET_USE_UNIT_IS_RIGHT`, not
  `POSITION_OFFSET_USE_UNIT_IS_RIGHT`. Camera shake needed nothing: all 19 of its keys were
  already implemented with matching semantics.
- **New helper VAs** are in `funcmap.tsv`: `321f4a20`, `321f3be0`, `321f2a60`, `321f3690`,
  `321f2db0`, `32fea4e0`, `32fea820`, `32fe99b0`.

### June transcription (`FUN_321e48e0` @ `321e5cf8`–`321ee2a0`)

Read instruction by instruction from `ghidra/dis_0x321e48e0.txt`. Getters: `32fe9e40` bool,
`32fe9fd0` int, `32fea370` float, `32feae90` string, `32fea4e0` int by index,
`32fea820` float by index. `[334380f0]` = -1.0, `[334350e8]` = 0.0 (double),
`[33436704]` = 0.05. "Default" is what June stores when the getter fails, so it holds
whatever the ctor set.

**Per `EFFECT%d`, new keys** (the rest match March; see the matrix):

| key | VA range | getter | default | post-processing |
|---|---|---|---|---|
| `GROUP_NAME` (binary-only) | `321e6261`–`321e632b` | string | `""` | none; read after `BONE_NAME` |
| `ZOFFSET_USE_UNIT_IS_RIGHT` | `321e6f61`–`321e7115` | bool | false | none. The plan's old "POSITION_OFFSET_USE_UNIT_IS_RIGHT" is wrong; the decryptor and June data both say `ZOFFSET_…` |
| `TRACE_MATRIX_EXCLUDE_SCALE` | `321e7445`–`321e7610` | bool | false | **if true, `TRACE_MATRIX` = true** (`321e760b`–`321e7610`) |
| `TRACE_MATRIX_IN_FIELD` | `321e766e`–`321e77e2` | bool | false | none |
| `USE_IS_RIGHT_WHEN_CREATE` | `321e8114`–`321e82bb` | bool | false | none; read after `APPLY_UNIT_ROTATION_MIRROR` |
| `FOLLOW_UNIT_ROTATION` | `321e830c`–`321e846f` | bool | false | none |
| `FOLLOW_UNIT_ROTATION_CPARTICLE` | `321e84da`–`321e86dd` | bool | false | none |
| `FOLLOW_UNIT_VIEWER_SCALE` | `321e8738`–`321e88dc` | bool | false | none |
| `USE_EFFECT_SCALE_IN_UNIT_VIEWER` | `321e894f`–`321e8b63` | bool | false | none |
| `BONE_TRACE_TARGET_MESH_INDEX` | `321e9aa9`–`321e9c8d` | int | **-1** | none; read before `BONE_TRACE_TARGET_MESH` |
| `NOT_ALLOW_MEMO` | `321ea221`–`321ea326` | bool | false (`XOR EBX,EBX` @`321ea308`, then `MOV [ESI],BL`) | none; read after `LIMIT_DISTANCE` |
| `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` | `321ea3ac`–`321ea634` | int | 0 (`NEG/SBB/AND`, low byte kept) | unsupported (same as the templet level) |

`LIMIT_DISTANCE` (`321ea0b2`–`321ea1dd`) was hand-checked: float, default -1.0, stored
squared only when `> 0.0`. That is exactly March's `X2EffectSet.cpp` read, so the matrix's
`unchecked(post-processing)` is settled as same.

**Type gate** (`321ea63a`–`321ea64b`): after the keys above, `EFFECT_TYPE` == 1
(`ET_DAMAGE_EFFECT`) reads `APPLY_DAMAGE_EFFECT_VELOCITY`. Types 2..5 (the four
`ET_MESH_PLAYER_*`) read `MESH_TRIGGER_DATA_LIST`, then `MESH_TRIGGER_DATA`. Every other type
reads neither.

**`APPLY_DAMAGE_EFFECT_VELOCITY`** (`321ebf7c`–`321ec080`): a positional table.
- `[1]`, `[2]`, `[3]` are floats, default 1.0 each. `[4]` is an int, default 0.
- One 32-byte entry is built (ctor `321f2a60`: `+0`=1, `+8..+0x10`=1.0) and pushed onto a
  vector in EffectData (`321f3690` is that vector's `push_back`).
- The entry holds `+0` = (`[4]` == 0) as a bool, `+4` = 1 (a type tag), and `+8/+0xc/+0x10` =
  `[1]`/`[2]`/`[3]`.
- Every June use is `{1.5, 1, 1, 1}`. What `[4]` and the tag mean is Phase 4a's to read at
  the consumer.

**`MESH_TRIGGER_DATA_LIST` → `TRIGGER_DATA%d`** (`321ea682`–`321ebbec`):
- Each entry is a stack struct (ctor `321f3be0`), pushed into a local vector, then assigned
  into EffectData @`321ebbec`.
- Order: `TRIGGER_TIME` float -1.0; `MOTION_NAME` string `""`; `PLAY_TYPE` int **2**
  (`XAP_ONE_WAIT`, binary-only).
- Then `BOUND_MOTION_TYPE_LIST`: int by index from 1 until the first miss, each pushed
  (`321eaa27`–`321eaa99`; the data uses `EMOTION_TYPE.*`, which March's `Enum.lua` lacks).
- Then `STOP_TRACING_ON_TRIGGER` bool false; `TRIGGER_ONCE` bool false; `BLOCK_TRIGGER` bool
  false (`XOR BL,BL` @`321eaed6`); `BLOCK_TRIGGER_TIME` float -1.0.
- Then `TRIGGER_EFFECTSET_LIST`: `BeginTable(int)` from 1, `EFFECT_SET_NAME` string `""`, and
  only non-empty names are pushed (`321eb09c`–`321eb2f3`).
- Then `EFFECT_SET_TOGGLE_VISIBILITY_TIMER` float -1.0. **If the read succeeds and the value
  is not -1.0, a flag is set** (`321eb5c7`–`321eb5de`); the ctor clears it.
- Then `USE_MESH_TRANSITION` bool **true** (`321eb78a`).
- Then `ANIMTIME_ON_START_GAME` / `_SQUARE` / `_UNITVIEWER` (the last two binary-only):
  each is `ONCE` bool false, then `TIME` float 0.
- The ctor also presets, with no Lua read, a second byte after each `ONCE` to 1, plus a
  second wstring and a few dwords: runtime state for Phase 4a.

**`MESH_TRIGGER_DATA`** (`321ebbf1`–`321ebf61`), stored inline in EffectData: `TRIGGER_TIME`
float -1.0, `MOTION_NAME` string `""`, `PLAY_TYPE` int **2** (binary-only).

**`SOUND%d`** (`321edd29`–`321ee2a7`), at templet level after `CAMERA_SHAKE%d`:
- A stack entry is filled and pushed onto a templet vector, with no rejection path:
  `EVENT_TIME` float 0, `FILE_NAME` string `""`, `RATE` int **100** (binary-only),
  `USE_GLOBAL_TIME` bool false, `LOOP` bool false.
- Byte `+0x27` of the entry is preset to 1 with no Lua read: runtime, Phase 4a.
- This is field for field March's own `CX2DamageEffect::SoundData`.

**For Phase 4, not parsed here:** after pushing each effect, June also pushes it onto a
second templet list when `BONE_TRACE_TARGET_MESH` is set (`321ec09a`–`321ec0ab`).
`ONCE_PER_CYCLE` is in the data but June never reads it (`june-ignores`).

**Paste with:** Starting context + this section.

- **June source:** `FUN_321e48e0`, from the per-`EFFECT%d` loop to its end, including the
  trigger, sound and camera-shake sub-blocks.
- **March target:** the same function; `EffectData` and its sibling structs in
  `X2EffectSet.h`.

**Scope.** Per-effect keys:
- `ZOFFSET_USE_UNIT_IS_RIGHT` (was mis-listed as `POSITION_OFFSET_USE_UNIT_IS_RIGHT`), `TRACE_MATRIX_EXCLUDE_SCALE`, `TRACE_MATRIX_IN_FIELD`
- `FOLLOW_UNIT_ROTATION`, `FOLLOW_UNIT_ROTATION_C…PARTICLE`, `USE_IS_RIGHT_WHEN_CREATE`
- `FOLLOW_UNIT_VIEWER_SCALE`, `USE_EFFECT_SCALE_IN_UNIT_VIEWER`, `BONE_TRACE_TARGET_MESH_INDEX`

Then the trigger, sound and camera-shake sub-block keys, with the exact names from the
matrix.

**The parser is 39 KB, so split the session if needed.** Do the per-effect keys first,
then the sub-blocks. If the conversation is running long, commit after the per-effect
keys, and record in this section exactly where the next session should resume (a June
VA).

**Exit criteria.**

- `census.py` shows 0 EffectSet keys `to-implement`; the exceptions (unsupported or
  binary-only) are named in the matrix.
- A clean build.
- A `JUNE_FX ES_EFFECT` summary line appears for one Ranox set.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 3"), june2017_re/README.md, and
the Phase-3 rows of JUNE2017_EFFECT_KEY_MATRIX.md. Phase 2 is done: the templet-level keys
are parsed under SERV_IRUHADEV_JUNE_EFFECTSET_PARSE. Extend that flag, since it's the same
logical change. Branch: mods/lanox-port.

Your task: transcribe the per-EFFECT%d keys and the trigger / sound / camera-shake
sub-blocks of June's FUN_321e48e0 into CX2EffectSet's parser and EffectData. Parse only;
runtime is Phase 4. Cite June address ranges. Per-effect keys first; if the session runs
long, commit and write the resume VA into Phase 3's section of the plan.

CP949 byte-level edits only. Build US_SERVICE, deploy, ASK before launching. Done when
census.py shows 0 EffectSet keys to-implement. Update the matrix and MODS.md.
```

---

## Phase 4a — EffectSet runtime: creation

**Status: in progress (2026-09-26).** The spec below was written before any code, from the
June disassembly, with the decompile used only as a map. The work is split into three stages.
Resume at the first stage not marked done.

| stage | scope | state |
|---|---|---|
| 1 | Offset, position, rotation, the target-mesh vector, `ZOFFSET`, `OVERLAP`, debug lines | **done** 2026-09-26: clean `US_SERVICE` build, deployed to `X2_offline.exe` (SHA-256 `33d1eb57d282fae6…`); **play-tested 2026-09-26**, see below |
| 2 | `USE_IS_RIGHT_WHEN_CREATE`, `TRACE_MATRIX_IN_FIELD`, trigger-list copy + mesh `+0x194`, `APPLY_DAMAGE_EFFECT_VELOCITY`, `SET_OVER_UI` layer | **done** 2026-09-26: clean `US_SERVICE` build (KTDXLIB, X2Lib, X2), deployed to `X2_offline.exe` (SHA-256 `d31b2cb8328d66c1…`); **play-tested 2026-09-26** (Ice Heater `SET_OVER_UI`), see "Stage 2 play-test result" |
| 3 | `GROUP_NAME` (`321dd590`, the `_GROUPCHILD_` loop), unsupported-key logging | **implemented** 2026-09-26: clean `US_SERVICE` build (KTDXLIB, X2Lib, X2), deployed to `X2_offline.exe` (SHA-256 `7fd69ad01bce53c8…`), see "Stage 3 result". No data file, March's or June's, uses `GROUP_NAME`, so that branch can never be play-tested; **regression play-tested 2026-09-26**, see "Stage 3 regression result" |

### June layout used for reading (never ported)

These are offsets from reading June. They are not March members; see rule 2.

- **EffectData** (ctor `321f4a20`, size `0x1e8`). The layout was derived mechanically from
  every getter's destination in `321e48e0`.
  - `+0` type; `+4` `NAME`; `+0x20` `HYPER_NAME`; `+0x3c` `START_TIME`.
  - `+0x40` `BONE_NAME`; `+0x5c` `GROUP_NAME`; `+0x78` `WEAPON_BONE_NAME`; `+0x94`/`+0x98`
    weapon/model index; `+0x9c` `RIDING_PET_BONE_NAME`.
  - `+0xb8..0xc0` `POSITION_OFFSET`.
  - Bytes `+0xc4..0xcb`: `ZOFFSET_USE_UNIT_IS_RIGHT`, `TRACE`, `TRACE_MORE`, `TRACE_MATRIX`,
    `_EXCLUDE_SCALE`, `_IN_FIELD`, `REVERSE_Y`, `USE_LAND_POSITION`.
  - `+0xcc..0xd4` `ROTATE`; `+0xd8`/`+0xd9`/`+0xda` `APPLY_UNIT_ROTATION` / `_MIRROR` /
    `APPLY_INSTANCE_SCALE`; `+0xdc..0xf0` `SCALE` and `SCALE_*_ON_LEFT`.
  - Bytes `+0xf4..0xf7`: `START_WHEN_TRACE_TARGET_DEAD`, `PASSIVE_EFFECT`,
    `USE_SUB_ATTACK_LIST_SET`, `SET_OVER_UI`.
  - `+0xf8` `BONE_TRACE_TARGET_MESH_INDEX`; `+0xfc` `BONE_TRACE_TARGET_MESH`; `+0xfd`
    `TRACE_USER_TARGET_FORCE`; `+0x100` `LIMIT_DISTANCE`.
  - `+0x104` `FOLLOW_UNIT_ROTATION`; `+0x105` `NOT_ALLOW_MEMO`; `+0x114`/`+0x115` the two
    unit-viewer scale keys.
  - `+0x128` the `APPLY_DAMAGE_EFFECT_VELOCITY` vector; `+0x138` `USE_IS_RIGHT_WHEN_CREATE`;
    `+0x13c` the inline `MESH_TRIGGER_DATA`; `+0x1d4` `MESH_TRIGGER_DATA_LIST`.
  - `+0x1e4` `FOLLOW_UNIT_ROTATION_CPARTICLE`; `+0x1e5` the per-effect effect-alpha option.
  - The parser never fills `+0x108..0x110` or `+0x118`. See `NOT_ALLOW_MEMO` below.
- **EffectSetData**:
  - `+0x1c` the effects; `+0x2c` the second list (effects with `BONE_TRACE_TARGET_MESH`).
  - `+0x4c` `EVENT_TIMER_TYPE`; `+0x50` `STOP_CREATE_…`; `+0x51` `AFFECTED_BY_UNIT_SCALE`;
    `+0x54` `DELETE_SHAKE_COUNT`; `+0x58` `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`; `+0x59`
    `OVERLAP`; `+0x5c` `APPLIED_SKILL_ID`.
  - `+0x60..0x68` the memo scale; `+0x6c` `SYNC_SHOW_BY_CREATE_UNIT`; `+0x6d` the templet
    effect-alpha option; `+0x70` `SOUND%d`.
- **EffectSetInstance** (ctor `321df230`):
  - `+0` mode (1 game, 2 field, 3 pet); `+0xc` unit; `+0x14` trace target; `+0x1c` square
    unit; `+0x21` custom position.
  - `+0x5c` delay; `+0x60`/`+0x64` power rate / scale; `+0x70` scale; `+0x7c` damage-effect
    scale; `+0x80` EffectSetData; `+0x98` effect instances.
  - `+0xc0` elapsed; `+0xc4` use animation timing; `+0xd4` pet; `+0xe4` local rotation.
  - `+0xf0` the **target-mesh vector**: `{handle, EFFECT_TYPE, scale}`, `0x14` bytes each.
  - `+0x100` is-target-mesh; `+0x101` target-mesh right; `+0x104` target-mesh dir; `+0x110`
    target-mesh Z; `+0x124` bone-trace target-mesh scale.
  - `+0x150` memo scale; `+0x180` **facing when the set was created** (`321df230`
    @`321df528`-`321df576`: `unit->GetIsRight()`, square `+0x3d5`, pet `+0x439`).

### Creation-time keys: spec

Verdicts:
- `new`: June-only behaviour, transcribed.
- `semantics-differ`: March reads the key, and June's behaviour differs.
- `same`: diffed, no change.
- `4b @VA`: the only reader is in Phase 4b's functions.
- `unsupported`: the input system is absent (rule 5).

| key / behaviour | June branch | March site | verdict | stage |
|---|---|---|---|---|
| `ZOFFSET_USE_UNIT_IS_RIGHT` | `321d9820` @`321d9b36`-`321d9c2a`: in the game-unit branch, a nonzero Z offset is added along `GetZVector()` when the key is false. When the key is true it is added if `GetIsRight()`, subtracted otherwise | `SetEffectSetOffset` | new | 1 |
| `POSITION_OFFSET_X`/`_Z` facing unit | `321d9820` @`321d9847`-`321d9889`: the facing unit is the **trace target** when valid, else the owner. March always uses `m_optrGameUnit` | `SetEffectSetOffset` | semantics-differ | 1 |
| target-mesh offset, game unit | `321d9820` @`321d98a3`-`321d98c2`: `CMP idx,count; JBE` skips the target-mesh branch for every in-range index. It is reachable only past the end of the vector (an out-of-bounds read, not reproduced). June therefore always offsets by unit facing. March used the target mesh's dir / Z | `SetEffectSetOffset` | semantics-differ | 1 |
| target-mesh offset, no unit / square / pet | `321d9820` @`321d9e6a`-`321d9ea6`: `idx = max(INDEX,0)`, requires `idx < count`, handle valid and `!TRACE_USER_TARGET_FORCE`, then the plain add | `SetEffectSetOffset` | same (plus index) | 1 |
| `BONE_TRACE_TARGET_MESH_INDEX`, position | `321dcd30` @`321dcd9d`-`321dce07`: `idx = BONE_TRACE_TARGET_MESH ? INDEX : max(INDEX,0)`; then `321dcac0` (in range, handle ≠ -1, type ≠ 0, `!FORCE`; a dead mesh invalidates its entry) | `GetEffectPosition`, `GetEffectPositionByTargetMesh` | new | 1 |
| `BONE_TRACE_TARGET_MESH_INDEX`, rotation | `321df7e0` @`321df80c`-`321df860`: `idx = max(INDEX,0)`; valid only if not (`INDEX < 0` and `BONE_TRACE_TARGET_MESH`), in range, handle ≠ -1 | `GetEffectRotation` | new | 1 |
| `BONE_TRACE_TARGET_MESH`, registration | Damage `321f1f76`-`321f20f4`; mesh `321f21f7`-`321f2414`. Each such effect is **pushed** onto the target-mesh vector (`{handle, type, SCALE or 1,1,1}`).<br>Damage: is-target-mesh / right / dir / Z are set only when the count becomes 1; the instance's scale is not touched.<br>Mesh: is-target-mesh when the count is 1; right (set-only) / dir / Z on every push when the unit is valid; instance scale on every push.<br>March registered only the first and set scale in both cases | creation switch tail | semantics-differ | 1 |
| `BONE_TRACE_TARGET_MESH`, as a position source for itself | `321dcd30` as above: `INDEX` -1 with the key set means the effect does not trace any mesh | `GetEffectPosition` | semantics-differ | 1 |
| `OVERLAP` | `321d9f30`, from the factory `321e1bf0` @`321e1c68`-`321e1c77`: when set, and a live instance of the same templet has the same trace target (both null counts), no instance is created | `PlayEffectSet` → `_CreateEffectSetInstance` | new | 1 |
| `SET_OVER_UI` (particles) | `321f0f49`-`321f0f63` (the same in the other three particle cases): `+0x170`=1 (`m_bOverUI`) **and** `+0x164`=500 (500 = `XDL_DIRECTING`). Stage 2 confirmed `+0x164` = `m_Layer` from June's `CKTDGObject` ctor `31d28a20`, whose stores follow March's field order (`KTDGObject.cpp:6-89`), rather than from a `LAYER` store | creation, particle cases | semantics-differ | 2 |
| `USE_IS_RIGHT_WHEN_CREATE` | `321ef6bc`-`321ef6d5` (damage effect only): CEffect `+0x135` = instance `+0x180` | damage case | new | 2 |
| `TRACE_MATRIX_IN_FIELD` | mesh / particle cases, e.g. `321ef8ee`, `321efe84`, `321f03b6`: bone frame from the owner (`TRACE_MATRIX`, no target mesh) or from the target mesh. Matrix copied to the instance; position = frame + offset | creation, mesh / particle cases | new | 2 |
| `MESH_TRIGGER_DATA_LIST`, creation part | `321f2130`-`321f2166`: the EffectInstance copies the list (`+0x4c`); a non-empty list sets mesh `+0x194`=0 | mesh case | new | 2 |
| `APPLY_DAMAGE_EFFECT_VELOCITY` | `321ef692`-`321ef6a0` → `321db1b0` tag 1: main effect velocity `+0x534`; add if `[4]`==0, else multiply | damage case | new | 2 |
| `GROUP_NAME` | `321eee45`-`321eeeb7`, loop `321eeec0`-`321ef0ce`, `321f2483`-`321f24f1`, `321f2550`-`321f257c`, helper `321dd590`: particle types only. Base = `GROUP_NAME` up to `_GROUP`; one particle per bone `<base>_GROUPCHILD_<n>`, n from 1 | particle cases | new | 3 |
| `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` (effect / templet) | `321f1ec8`-`321f1ee4`, factory `321e1dd7`-`321e1e0c`: effect byte, or else the instance's option (`+0x1bc`) → damage `+0xb32`, mesh `31c83620`, particle `+0x1b0` | — | unsupported | 3 |
| `APPLIED_SKILL_ID` | `321e1e60` → `31fdbbb0` (user `+0x867c` memo) → EffectSetData `+0x60` → instance `+0x150` | — | unsupported | 3 |
| `NOT_ALLOW_MEMO` | `321dddb0` @ its tail: unless set, scale ×= memo scale. June copies it through EffectData `+0x108` @`321eede8` (a write into the shared templet, not reproduced) | `GetEffectScale` | depends on `APPLIED_SKILL_ID`; no effect in March | 3 (log) |
| `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE` | `321da070` (marks every effect created), reached only from unit `3228a120` over the handle list `unit+0x3e90` on state change (callers `32015250`, `3277ee10`). The code that pushes into that list is outside the EffectSet region and **not yet located** | unit side | deferred: unit-side registration | — |
| `SYNC_SHOW_BY_CREATE_UNIT` | getter `321f2b00`, only caller UpdateEffect @`321e077d`. At creation only particles get `SetShowObject(inst+0xdc)` (`321f251d`) | — | 4b @`321e077d` | — |
| `SOUND%d` | `321dee80`, only caller UpdateEffect @`321e07f7` | — | 4b @`321e07f7` | — |
| `FOLLOW_UNIT_ROTATION`, `_CPARTICLE`, `REVERSE_Y`, inline `MESH_TRIGGER_DATA` | read only in `321e0080` | — | 4b | — |
| `FOLLOW_UNIT_VIEWER_SCALE`, `USE_EFFECT_SCALE_IN_UNIT_VIEWER` | no reader in `0x321d0000`-`0x32208000` except the parser | — | not creation; owner open | — |
| `LIMIT_DISTANCE` | `321eedad`-`321eeddf` | creation | same | — |
| `START_WHEN_TRACE_TARGET_DEAD`, timing / `EVENT_TIMER_TYPE` | `321ee7e4`-`321eec83` | creation | same | — |
| `USE_SUB_ATTACK_LIST_SET` | `321f219f`-`321f21f2` | mesh case | same | — |
| `TRACE_MATRIX` scale division | e.g. `321ef4f5`-`321ef58f` | every case | same | — |
| `GetUnitRotation` | `321ddea0`: the target-mesh lookup result is discarded, as in March | `GetUnitRotation` | same | — |

**Keyless June changes, not transcribed** (no key and no March caller; no dead members added):
- Position: the instance extra offset `+0x1a4` and the instance override bone `+0x130`.
- Holders: `+0xd8` (riding pet), `+0xe0`, and `+0xdd` (UI viewer).
- Damage-effect copies:
  - `+0x14c` (the `fForceDownValueRate` argument, default 1.0 = March's default);
  - `+0x11c`/`+0x120` (first attack and relate skill; `ADD_RENA_SYSTEM` is off);
  - `+0x15c`, from unit vfunc `0x10c`;
  - `+0x1b8`, and CEffect `+0xb64` = hyper.
- Modifier lists: `321ee480` over EffectData `+0x118` and instance `+0x160`, and `321db1b0`
  over instance `+0x170`.
- The particle option gate `3241d2a0` (`[3377585c]+0x180`, `+0x2c == 2` skips particles).
- Unit vfunc `0x248`, called from `PlayEffectSet`, is an empty stub in every class.

**Exit-test set.** No deployed set uses a June-only key, as in Phases 2-3. The branches that
fire on Ranox content are the semantics-differ ones:
- `BONE_TRACE_TARGET_MESH` registration: 11 of the 196 Ranox sets in
  `march2014/data036/EffectSet.src.lua`.
  - Damage-effect and mesh registrations: `EffectSet_NUI_RAGE_MOTHERFIG_Attack_A_Fire`,
    `EffectSet_LOW_RAGE_FATHERFIG_FD_BOSS_Attack_A`.
  - Damage effect only: `EffectSet_NUI_RAGE_DRYADKENTA_AttackA_Fire` / `_SpacialAttackA_Fire`.
  - Mesh only: `EffectSet_NUI_SMITH_STAIN_Attack_Ready` / `_Attack_C_Fire` /
    `_SpecialAttackA_Ready`, `EffectSet_NUI_RAGE_MOTHERFIG_Attack_A_Ready` / `_Attack_B_Ready`,
    `EffectSet_NUI_RAGE_BABYFIG_Attack_A_Ready`, `EffectSet_MOUNT_MAG_STONE_Wait`.
- The X/Z offset facing: 48 sets with a nonzero `POSITION_OFFSET_X`.

**Stage 1 play-test** (any difficulty). The NPC scripts are in `march2014/update/data037/`, and
the dungeons were found by grepping `Dungeon_Ranox_*` for the NPC names.
- **Ranox Burned Forest**: Rage Motherfig (`RAGE_MOTHERFIG`), Rage Dryadkenta
  (`RAGE_DRYADKENTA`), Rage Babyfig (`RAGE_BABYFIG`).
- **Ranox Ashes Village**: Smith Stain (`SMITH_STAIN`).
- Expected in `offline_server.log`:
  - `JUNE_FX ES_CREATE EffectSet_NUI_RAGE_MOTHERFIG_Attack_A_Fire <effect> BONE_TRACE_TARGET_MESH registered_index=0(mesh)`,
    and likewise `(damage)` for the Dryadkenta sets and `(mesh)` for the Smith Stain sets.
  - Where those sets offset a traced effect: `BONE_TRACE_TARGET_MESH offset=unit_facing(321d98c0)`.
- Not expected on current data, and not a failure:
  - `OVERLAP`, `ZOFFSET_USE_UNIT_IS_RIGHT` and `BONE_TRACE_TARGET_MESH_INDEX`: no deployed set
    uses them.
  - `facing=trace_target`: it needs a set played with a trace target that is not its owner.
- Watch for BTTM effects that move or rescale:
  - a second target-mesh effect in a set no longer sits on the first mesh;
  - a damage-effect BTTM set with `TRACE_MATRIX` siblings now divides by (1,1,1).
- Revert: undefine `SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME`.

**Stage 1 play-test result (2026-09-26).** `offline_server.log` has 47 `JUNE_FX ES_CREATE`
lines, all on Ranox sets:

| set | `registered_index=0(mesh)` | `offset=unit_facing(321d98c0)` |
|---|---|---|
| `EffectSet_NUI_RAGE_BABYFIG_Attack_A_Ready` | 15 | 15 |
| `EffectSet_NUI_SMITH_STAIN_Attack_Ready` | 5 | 5 |
| `EffectSet_MOUNT_MAG_STONE_Wait` | 4 | - |
| `EffectSet_NUI_RAGE_MOTHERFIG_Attack_B_Ready` | 1 | 2 |

- No `(damage)` registration fired in that run. Dryadkenta's teleport soft-locks the stage, so
  her sets could not be waited out.
  - A second run covered the damage path with a player skill:
    `EffectSet_Rena_Perfect_Storm PERFECT_STORM BONE_TRACE_TARGET_MESH registered_index=0(damage)`
    (x2), with no unhandled packets or exceptions.
  - A Hamelling Attack X in that run logged nothing. Only `EffectSet_Riding_Dark_/Red_HAMELLING_Attack_X`
    carry `BONE_TRACE_TARGET_MESH`; `_ICE_` does not.
  - A third run used the fire (Red) Hamelling, and `EffectSet_Riding_Red_HAMELLING_Attack_X` fired
    both branches in one set:
    - `DAMAGE_EFFECT_RIDING_RED_HAMELLING_ATTACK_X_MAIN … registered_index=0(damage)` (x3);
    - its traced sibling `…Attack_X04 … offset=unit_facing(321d98c0)` (x6).
  - That sibling is the case where stage 1 moves an effect relative to stock: March offset it
    along the damage effect's own facing.
- No `UNHANDLED`/`EXCEPTION` packets, and the shutdown was clean.
- The only `RANOX_EFFECT_MISSING` line is `EffectSet_PVP_BOT_Armageddon_Blade` (x5), an AI-party
  set with no templet. Its lookup fails before any Phase 4a code runs.
- So the stage-1 exit criterion (a `JUNE_FX ES_CREATE` line on a named Ranox set) is met.

**Create and update share the helpers**, so the facing change is consistent frame to frame.
June's `321dcd30` is called by CreateEffect @`321ef23a` and by UpdateEffect @`321e0964`
(plus `321df750`). March's `GetEffectPosition` is called by CreateEffect and UpdateEffect
(`X2EffectSet.cpp:1428`); its other `SetEffectSetOffset` caller is `EFFECT_TOOL`-only.

**Runtime coverage is mechanical.** `census.py` checks that every key March reads at the top of
`ES.templet`, `ES.EFFECT` and `ES.SOUND` has a `RUNTIME` citation. It passes: 69 keys are cited,
the existing ones as `same` with the June VA of their read.

**Stage 2 result (2026-09-26).** Member identification came first, as planned. Each June offset
has two independent sources:
- a March `offsetof` probe, compiled with `cl` under X2Lib's `US_SERVICE` defines. It reuses
  `census.py`'s `cl_cmd` with `#define private public`, compiles with `/Zs`, and reads each
  offset back out of a deliberate `C2440` on `char (*)[N]`;
- a June function whose shape matches a March one.

| June | March member | probe | shape match |
|---|---|---|---|
| CEffect `+0x135` | `m_bFollowLineRight` | `0xf5`, after `m_bFollowLine` `0xf4` = June `+0x134`, the `FOLLOW_LINE` store @`31f7dbb4` | ctor @`31f7b056` owner vfunc `0x170` / @`31f7b0d7` pet `+0x439` = `X2DamageEffect.cpp:1330` / `:1339`; flip @`31f71865` = `:6286`; left-facing negate @`31f826ad` = `:5796` |
| mesh `+0x534` / `+0x540` | `m_vVelocity` / `m_vVelocityFinal` | `0x3fc` / `0x408` | `31c8a1b0` = `CXMeshEvent_Velocity::OnFrameMove` (`KTDGXMeshPlayer.cpp:3297-3316`) |
| mesh `+0x374` / `+0x378` | `m_bUseDXMatrix` / `m_DXMATRIX` | `0x244` / `0x248` | `31f8ad90` = `SetUseDXMatrix` (`KTDGXMeshPlayer.h:1153`) |
| mesh `+0x3c0`, `+0x3f0`, `+0x510` | `m_pXSkinAnim`, `m_vPos`, `m_vScale` | `0x290`, `0x2c0`, `0x3d8` | `321f2990` = `GetBonePos`; `31c1a370` = `SetPos` |
| particle `+0x164` / `+0x170` | `m_Layer` / `m_bOverUI` (`CKTDGObject`) | `0x30` / `0x3c` | June's `CKTDGObject` ctor `31d28a20` stores every field in March's order |
| particle `+0x1f8` / `+0x1fc`, `+0x2f8` | `m_bUseDXMatrix` / `m_DXMATRIX`, `m_Position` | `0xcc` / `0xd0`, `0x1cc` | `321f2940`, `31c19f20`, `31cf1a40` = `SetPosition( pos, false, false )` |
| EffectSetInstance `+0x8` | `m_pXSkinAnim` | `0x8` | ctor order (`X2EffectSet.cpp` ctor init list) |
| mesh `+0x194` | **none** | March's first `CXMeshInstance` member, `m_Handle`, is June `+0x198` | set to 1 in the init list of the ctor `31c8d140` @`31c8d18e`, ahead of the body's `SetAlphaObject(true)` (`+0x144`) and `m_Handle = -1`. The only other store is `321f2166`; no reader was found in KTDXLIB, the effect regions or the whole `.text` byte scan. June-only, not reproduced |

Offsets move as a block between the two builds: mesh `-0x130` up to `m_vPos`, `-0x138` from
`m_vScale`; particle `-0x12c`; `CKTDGObject` `-0x134` from `m_Layer`. That is consistent, but the
table rests on the shape matches, not on the shifts.

What the two calls after the velocity write do (`31c8f310`, `31c8a9b0`):
- `31c8f310` builds a 0x14-byte modifier `{add, tag, v3}`.
- `31c8a9b0` merges it into, or pushes it onto, the mesh's list at `+0x568`.
- The velocity event `31c8a1b0` re-applies tag 1 through `31c89d60`:
  - to `m_vVelocity` when the event starts after time 0 (a time-0 event already ran in the
    ctor's `RunEvent()`);
  - to `m_vVelocityFinal` on fading events.

That is transcribed on `CXMeshInstance` in KTDXLIB. `sizeof(CXMeshInstance)` is `0x4fc` from both
an X2Lib TU and a KTDXLIB TU, and the list is appended last, at `0x4ec`. The live velocity-event
variant (`X2OPTIMIZE_PARTICLE_AND_ETC_SIMULATION` + `_VELOCITY_ACCUM_BUG_FIX`) was settled with a
flag probe, and a temporary `#pragma message` proved both re-apply sites compile. Tags 2 and 3
(`31c89fb0`, `31c8a720`) are pushed only from `32c076e0`, outside the effect code, and are not
transcribed.

`TRACE_MATRIX_IN_FIELD` is transcribed with three June quirks left in place, each with its own
`JUNE_FX` detail:
- The mesh owner branch reads the **new mesh's own** skeleton. The particle owner branch reads
  the set's `m_pXSkinAnim`.
- The target mesh is always looked up in `g_pData`'s UI-major player (`[g_pData+0x8c]` in all
  eight cases).
- `POSITION_OFFSET` is added a second time.

A mesh with no skin anim is skipped with a log line, because June would fault there. The
particle bone is the `GROUP_NAME` child bone when that is set (`@321f0d10`); stage 3 owns that.

**Which stage-2 keys the deployed data uses.** All four effect-set files March loads were checked
(`X2StateStartUp.cpp:164-177`; the `_INT` pair is loaded because `INT_RESOURCE_DEVIDE` is on):
- `march2014/data036/EffectSet.src.lua`;
- `UIEffectSet.lua` from `24965799/kom_contents/data036_kom`;
- `EffectSet_INT.lua` and `UIEffectSet_INT.lua` from `datai015_kom`.

The last three were decompiled `mar-2014` in a scratch directory.
- `SET_OVER_UI` has 38 uses in `UIEffectSet.lua` and 8 in `UIEffectSet_INT.lua`, all on Ice
  Heater UI sets:
  - `EffectSet_Ice_Heater_Start`, `_Mini_`, `_Ancient_`, `_All_In_One_` and `_Halloween_`
    variants;
  - their `_Wait` sets.
  
  This is the only stage-2 branch a play-test can fire today.
  - The sets play while an Ice Heater opens (`X2UIInventory.cpp:655-663`), after
    `EGS_OPEN_RANDOM_ITEM_REQ`, which the offline emulator answers (`X2OfflineServer.cpp:850`).
  - Expect `JUNE_FX ES_CREATE EffectSet_Ice_Heater_Start <effect> SET_OVER_UI layer=XDL_DIRECTING(321f0f59)`.
  - It is a visible change: those particles now draw on layer 500.
- `USE_IS_RIGHT_WHEN_CREATE`, `TRACE_MATRIX_IN_FIELD`, `APPLY_DAMAGE_EFFECT_VELOCITY`,
  `MESH_TRIGGER_DATA_LIST`: **no deployed set uses them.** June's data does:
  - `EffectSet.lua`: 27 / 6 / 6 / 0;
  - `UIEffectSet.lua`: `TRACE_MATRIX_IN_FIELD` 6, `MESH_TRIGGER_DATA_LIST` 11.
  
  Their branches can only be exercised after Phase 11 adopts June's files. The Rena / Hamelling
  / Ranox runs will not show them. That is expected and does not count as a test.
- The mesh-side velocity re-apply runs in KTDXLIB, which has no route to `offline_server.log`. It
  is cited and proven compiled, but not log-verified.

**Stage 2 play-test** (pending): Rena Perfect Storm and the fire Hamelling Attack X for damage
effects, and Ranox Burned Forest / Ashes Village for meshes. Check them for regressions against
stage 1 (the same `registered_index` / `unit_facing` lines, no `UNHANDLED`/`EXCEPTION`). Open an
Ice Heater to see the new `SET_OVER_UI` layer line.

**Stage 2 play-test result (2026-09-26).** Nothing in the offline cash shop sold an Ice Heater,
so four products were added to `cash_product` in `els_db.sql`, after a backup to
`db_backup/*.bak-pre-ice-heater-shop-20260926-094553`. All four went into category 34
(Consume > Etc), at quantity 1 and price 1, as product numbers 2375-2378:
- 108900 Sealed Ice Sculpture and its key, 207500 Ice Burner;
- 85003498 Halloween Monster's Soul and its key, 85003501 Soul Tester.

How they were chosen:
- the cube IDs are on the client's hardcoded Ice Heater lists (`X2Define.h:1167-1190`);
- each cube/key pair comes from `RandomItemTable.lua`;
- all four IDs, and every reward in their groups, 502070 (122 rows) and 402110 (150 rows), are in
  this install's `item.kim`.

What the run's `offline_server.log` shows:
- `CASH catalog: 2081 product(s) from 2378 cash_product row(s)`: the four new rows arrived.
- The four purchases were claimed into the bag.
- Three `CUBE opened item 108900 (single draw, key 207500)` and one
  `CUBE opened item 85003498 (... key 85003501)`.
- On each open, both `EffectSet_Ice_Heater_Wait` / `_Start` (or the Halloween pair) fired
  `JUNE_FX ES_CREATE ... SET_OVER_UI layer=XDL_DIRECTING(321f0f59)`: 4 Wait and 6 Start lines,
  plus 1 Halloween Wait and 3 Halloween Start lines.
- No `UNHANDLED`/`EXCEPTION`, and the shutdown was clean.
- One `CUBE refused - no room for 1 x item 91610` was the bonus charm meeting a full bag. The cube
  stayed in the bag, as designed. It is not a Phase 4a issue.

A second run (2026-09-26, same exe) repeated two stage-1 checks. Both fired exactly the stage-1
lines and nothing new, which is expected because neither set uses a stage-2 key:
- Rena's Perfect Storm: `EffectSet_Rena_Perfect_Storm PERFECT_STORM BONE_TRACE_TARGET_MESH registered_index=0(damage)`.
- The fire Hamelling Attack X: `…_MAIN registered_index=0(damage)` (x2) and `…Attack_X04 offset=unit_facing(321d98c0)` (x4).

That run had no `UNHANDLED`/`EXCEPTION` and shut down cleanly. So stage 2 did not change the
stage-1 damage-effect path. The Ranox mesh sets were not re-run. The other four stage-2 keys stay
implemented but unexercised until Phase 11.

**Stage 3 result (2026-09-26).** Members were mapped as in stage 2: a March `offsetof` probe
(`offsetprobe.py`) plus one June function whose shape matches a March one.

| June | March member | probe | shape match |
|---|---|---|---|
| EffectData `+0x5c` / `+0x6c` | `m_wstrGroupName` (its size) | `0xe4` | the `GROUP_NAME` getter destination in `321e48e0` (Phase 3) |
| EffectData `+0x40` / `+0x50` | `m_PositionBoneName` (its size) | `0x40` | `321dcd30` reads it where March's `GetEffectPosition` reads `BONE_NAME` |
| EffectData `+0x105`, `+0x1e5` | `m_bNotAllowMemo`, `m_iApplyEffectAlphaConditionByOption` | `0x10c`, `0x110` | their parser reads `321ea221`-`321ea326` / `321ea3ac`-`321ea634` (Phase 3) |
| EffectSetData `+0x5c`, `+0x6d` | `m_iAppliedSkillID`, `m_iApplyEffectAlphaConditionByOption` | `0x4c`, `0x54` | their parser reads `321e5675`-`321e579e` / `321e5a5d`-`321e5cec` (Phase 2) |
| EffectInstance `+0`..`+0x14` | `m_bAlive`, `m_pEffectData`, the three handles, `m_optrGameUnit` | `0`, `4`, `8`/`0xc`/`0x10`, `0x14` (`CX2GameUnitoPtr` is 8 bytes) | the ctor `321f4c30` stores `1, 0, -1, -1, -1, 0` in March's init-list order |
| EffectInstance `+0x20` / `+0x2c` / `+0x30` | **none**. March's members end at `0x1c`, where stage 2's `m_vecMeshTriggerData` (June `+0x4c`) sits | - | cleared by `321f4c30`; written only by the `GROUP_NAME` loop (`@321f2483`-`321f24f1`); read by UpdateEffect `321e0080` @`321e0825`-`321e0986` and `@321e0d49`. Added as `m_vGroupChildOffset` / `m_bGroupChild` / `m_wstrGroupChildBoneName`, for 4b |
| pet `+0x2a4` | `CX2PET::m_pXSkinAnim` | `0x160` | `31e4d9e0` = `CX2PET::GetBonePos` (`X2PET.h:720`); `321f2b80` is its bool form |
| square unit `+0x2ec` | `CX2SquareUnit::m_pUnitViewer` | `0x1a0` | `321f2f20` = `CX2SquareUnit::GetBonePos` (`X2SquareUnit.h:318`); `321f3020` is its bool form |
| unit vfunc `0x80` / `0xfc` | `GetBonePos( name, out )` / `GetHeadBonePos` | - | `321dcd30`'s owner branch = `GetEffectPosition`'s `MODIFY_GET_BONE_POS` call with the head-bone fallback (`X2EffectSet.cpp`, `GetEffectPosition`) |
| mesh `+0x3c0` | `m_pXSkinAnim` | `0x290` (stage 2) | `321f29e0` = `GetBonePos( name, out )` (`KTDGXMeshPlayer.h:1344`) |

What was transcribed:
- **`321dd590` → `EffectSetInstance::GetEffectPositionByBoneName`.** This is `GetEffectPosition` with the
  bone passed in and a "bone found" flag. **`321dcc20`** is the same variant of `GetEffectPositionByTargetMesh`.
  - `321dd590` has two callers, the loop (`@321ef040`) and UpdateEffect (`@321e0933`). Both pass
    `<base>_GROUPCHILD_<n>`, so its empty-name branches (weapon bone, riding pet, position) are unreachable
    and are not transcribed.
  - Every March `GetBonePos( name, out )` override returns false for a missing bone
    (`X2GUNPC.h:1841`, `X2GUUser.cpp:33996`, `KTDGXSkinAnim.h:292`; `MODIFY_GET_BONE_POS` is on). So the
    loop terminates.
- **The loop, in `OnFrameMove_CreateEffect`.**
  - Rotation and scale are now taken once, before the position, as June does (`@321eee07`-`321eee21`).
    Without `GROUP_NAME` the body runs once, with the same position call as before.
  - With `GROUP_NAME` on a particle type:
    - The base is the name up to its first `_GROUP` (`3347e55c`).
    - Each pass builds `<base>_GROUPCHILD_<n>` (`3347e540`, `%d` at `3346a6b0`), n from 1, and creates one
      particle at that bone.
    - A bone the holder lacks still counts when the creation target mesh's skeleton has it
      (`@321ef069`-`321ef0c9`). The position is then what `321dd590` returned, e.g. the owner's head bone.
      This is transcribed as it is.
    - The loop ends at the first missing bone (`@321f2581`). Nothing is created at `BONE_NAME` itself.
  - With `BONE_NAME` set, the child position is also taken into the target mesh's `BONE_NAME` frame
    (`@321ef0ce`-`321ef1ef`), using the inverse of `combineMatrix`, then `D3DXVec3Transform`, then / w.
    - The two D3DX calls are named by shape, `(out, NULL, M)` and a 4-float result, because their IAT slots
      are bound in the dump.
    - When the mesh or bone is missing, June still copies an uncomputed stack vector (`@321f24ac`). That
      is not reproduced: the offset stays at (0,0,0), with a `JUNE_FX` detail.
  - The flag, bone and offset go onto the particle `EffectInstance` for 4b.
  - The particle `TRACE_MATRIX_IN_FIELD` helper now takes the child's bone (`@321f0d10`-`321f0d3b`).
- **Rule-5 run-time lines.** `JUNE_FX unsupported <KEY> in <templet>: <system> absent (value N, June <VA>)`
  is written to `offline_server.log`, once per key per templet, at June's consumer site:
  - `APPLIED_SKILL_ID` in `PlayEffectSet`'s user branch (`@321e1f57`);
  - the templet `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` in `_CreateEffectSetInstance` (`@321e1dd7`);
  - the per-effect one, with ` EFFECT<i>` in the templet field, in `OnFrameMove_CreateEffect` (`@321f1ec8`);
  - `NOT_ALLOW_MEMO` in `GetEffectScale` (`@321dde5f`).

  They are not behind `SERV_IRUHADEV_JUNE_FX_DEBUG`. The system wording matches the parse-time lines.
- Two June facts about the effect-alpha option, for whoever builds that system:
  - When the templet value is 0, the factory derives the option from the game options (`322d4d10`) for
    **every** set. So March differs from June on every set, not only on sets that use the key.
  - A per-effect value is used only when it is exactly 1 (`CMP CL,1` @`321f1ed9`). Any other value falls
    back to the instance's.

**Which data can fire it.** All four effect-set files March loads, and every June effect lua in
`june2017_re/june_lua/`, were searched for `GROUP_NAME`, `APPLIED_SKILL_ID`, `NOT_ALLOW_MEMO` and
`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`:
- The March files have none of the four.
- June's `EffectSet.lua` has `APPLIED_SKILL_ID` x170, `NOT_ALLOW_MEMO` x7 and the effect-alpha key x27;
  `UIEffectSet.lua` has the effect-alpha key x2. These rule-5 lines can fire from Phase 11.
- **No June file uses `GROUP_NAME` at all.** The binary reads it and no data exercises it, now or after
  Phase 11. The branch is implemented from the disassembly and is **untestable**, not merely untested.
  The only evidence that it is in the build is static: the deployed `X2_offline.exe` contains the UTF-16
  literals `_GROUPCHILD_`, `child_bone_on_target_mesh_only(321ef0c9)` and the rule-5 format string.

**Matrix diff against HEAD** (line numbers normalized): the five `RUNTIME` rows and the matching
`runtime 4a: … (stage 3)` → `(done)` notes change. The raw-literal, bare-id and rule-5-hit **lead counts**
of `APPLIED_SKILL_ID`, `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` and the memo / applied-skill /
effect-alpha system rows also rise, because the new log calls contain those names. No status, phase or
semantics cell changed, and the summary counts are identical.

**Stage 3 regression result (2026-09-26, exe `7fd69ad01bce53c8…`).** `offline_server.log` shows exactly the
stage-1/2 lines:
- Perfect Storm: `registered_index=0(damage)` x1.
- Fire Hamelling Attack X: `…_MAIN registered_index=0(damage)` x2, `…Attack_X04 offset=unit_facing(321d98c0)` x4.
- Ice Heater and Halloween Ice Heater: `SET_OVER_UI layer=XDL_DIRECTING(321f0f59)` on the Wait and Start sets (7 lines).
- Ranox Burned Forest Babyfig (`EffectSet_NUI_RAGE_BABYFIG_Attack_A_Ready`): `registered_index=0(mesh)` x9,
  `offset=unit_facing(321d98c0)` x11. This covers the mesh cases, which were not re-run after stage 2.

As expected, no `GROUP_NAME` or `JUNE_FX unsupported` line appeared. There were no `UNHANDLED`/`EXCEPTION`
lines, and the shutdown was clean. So the restructured creation loop left the damage, mesh and particle paths
unchanged.

**For Phase 4b:** the group-child members `EffectInstance::m_bGroupChild` / `m_wstrGroupChildBoneName` /
`m_vGroupChildOffset` are written but not read yet. UpdateEffect reads them @`321e0825`-`321e0986`
(position through `321dd590`, or the offset through the matrix of `321dc550`, a `GetCombineMatrix`
candidate) and @`321e0d49` (the `TRACE_MATRIX_IN_FIELD` bone).

The play-test for stage 3 is therefore a **regression** run: the same stage-1/2 lines as before
(Perfect Storm, the fire Hamelling Attack X, an Ice Heater `SET_OVER_UI`). This matters because every effect set
now runs through the restructured loop.

**Stage 2 kickoff prompt** (a new conversation):
```
Continuing the June2017 effect-engine rebuild, Phase 4a stage 2. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", and all of "Phase 4a": the stage table, the June
layout, the spec table, and the stage-1 play-test notes), june2017_re/README.md, funcmap.tsv, and the
"Runtime (Phase 4a on)" section of JUNE2017_EFFECT_KEY_MATRIX.md. Stage 1 is done and play-tested
under SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME; extend that flag. Branch: mods/lanox-port.

Your task: transcribe the stage-2 rows of the spec table into X2EffectSet.cpp's
OnFrameMove_CreateEffect:
- USE_IS_RIGHT_WHEN_CREATE (321ef6bc-321ef6d5, and the instance facing snapshot in 321df230).
- TRACE_MATRIX_IN_FIELD (the mesh and particle cases, e.g. 321ef8ee, 321efe84, 321f03b6).
- The creation part of MESH_TRIGGER_DATA_LIST (321f2130-321f2166).
- APPLY_DAMAGE_EFFECT_VELOCITY (321ef692-321ef6a0 -> 321db1b0, tag 1).
- The SET_OVER_UI layer (321f0f49-321f0f59).

Member identification comes first, and it is most of the job. June offsets still unmapped:
- CEffect +0x135;
- main-effect velocity +0x534, plus the calls 31c8f310 and 31c8a9b0 after it (read both before
  writing velocity);
- mesh +0x194 and +0x374/+0x378;
- particle +0x164.
Map each one with two independent sources: a March offsetof probe compiled with cl under X2Lib's
US_SERVICE defines (reuse census.py's cl setup; "#define private public" is fine for a probe),
and one June function whose shape matches a March function. Layout alone is not enough. If a member
has no March counterpart (CEffect +0x135 may be a Phase 5/7a dependency), record it and do not
invent one.

Cite June address ranges for every branch; re-express on March's members, never June's offsets. Add a
throttled JUNE_FX ES_CREATE line per branch. Update the RUNTIME table in census.py (state 'stage 2' ->
'done'), rerun census.py, and check that its matrix diff against HEAD only adds runtime notes. Update
MODS.md and the plan's stage table.

CP949 byte-level edits only (Python rb/wb with exact anchors; file + git diff --stat after). Build
US_SERVICE, deploy, ASK before launching. Test targets: Rena Perfect Storm and the fire (Red)
Hamelling Attack X for damage effects; the Ranox Burned Forest / Ashes Village monsters for meshes.
Search EffectSet.src.lua for sets using each stage-2 key; if none uses a key, say so rather than
calling it tested.
```

**Stage 3 kickoff prompt** (a new conversation, once the stage-2 play-test is recorded):
```
Continuing the June2017 effect-engine rebuild, Phase 4a stage 3. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", and all of "Phase 4a", including the stage-2
result and its member table), june2017_re/README.md, funcmap.tsv, and the "Runtime (Phase 4a on)"
section of JUNE2017_EFFECT_KEY_MATRIX.md. Stages 1-2 are done under
SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME; extend that flag. Branch: mods/lanox-port.

Your task: the stage-3 rows of the spec table.
- GROUP_NAME: 321eee45-321eeeb7, the loop 321eeec0-321ef0ce, 321f2483-321f24f1, 321f2550-321f257c,
  helper 321dd590. Particle types only; one particle per <base>_GROUPCHILD_<n> bone. The particle
  TRACE_MATRIX_IN_FIELD helper (JuneTraceMatrixInFieldParticle) must then take the group child's
  bone, as June does @321f0d10-321f0d3b.
- Rule-5 logging for APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION (effect / templet), APPLIED_SKILL_ID and
  NOT_ALLOW_MEMO: "JUNE_FX unsupported <KEY> in <templet>: <system> absent", once per key per templet.

Map every June member to a March one with the stage-2 method (offsetof probe + one shape-matching
June function) before writing. Cite June ranges; re-express on March members. Update census.py's
RUNTIME table, rerun it, and check the matrix diff against HEAD (line numbers normalized) adds only
runtime notes. Update MODS.md, funcmap.tsv and the stage table.

CP949 byte-level edits only (Python rb/wb with exact anchors; file + git diff --stat after). Build
US_SERVICE, deploy, ASK before launching. Search all four effect-set files March loads
(EffectSet, UIEffectSet, EffectSet_INT, UIEffectSet_INT; X2StateStartUp.cpp:164-177) for sets that
use GROUP_NAME; if none does, say so rather than calling it tested.
```

**Paste with:** Starting context + this section.

- **June source:** `FUN_321ee600` (`OnFrameMove_CreateEffect`, 17 KB, confirmed). Also
  `PlayEffectSet`'s June counterpart, which you will find by walking up from `321ee600`'s
  callers and down from the EffectSet lua binds. Add it to `funcmap.tsv`.
- **March target:** `CX2EffectSet::OnFrameMove_CreateEffect` (`X2EffectSet.cpp:77`),
  `PlayEffectSet` (`:1801`), `GetEffectPosition` (`:2772`).

**Method.**

- Diff June's control flow against March's for every Phase-2/3 key consumed at creation:
  position offset by facing, trace matrix, follow rotation, bone-trace mesh index,
  sync-show, overlap/group rules, stop-create-on-state-change.
- Also diff existing behaviour. Where June changed how a March key behaves, that is in
  scope here too: record it as `semantics-differ` and transcribe it.
- Each new branch gets a citation.
- Behind the debug flag, one throttled `JUNE_FX ES_CREATE <set> <effect> <key>` line per
  June branch taken.

**Exit criteria.** Every creation-time key has a cited branch, and a play-test log shows
at least one June branch firing on a named Ranox set.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 4a"), june2017_re/README.md,
funcmap.tsv, and JUNE2017_EFFECT_KEY_MATRIX.md. Phases 2-3 are done: all EffectSet keys are
parsed under SERV_IRUHADEV_JUNE_EFFECTSET_PARSE but not yet consumed.
Branch: mods/lanox-port.

Your task: transcribe creation-time behaviour from June's OnFrameMove_CreateEffect
(FUN_321ee600, confirmed) and PlayEffectSet (find its June VA and add it to funcmap.tsv)
into X2EffectSet.cpp:77 / :1801 / :2772, under a new SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME
flag. Cite June address ranges for every branch. Re-express June's logic on March's
members, never June's struct offsets. Where June changed how an existing key behaves,
transcribe that too and mark it semantics-differ in the matrix.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when every
creation-time key has a cited branch and a JUNE_FX ES_CREATE line fires on a named Ranox
effect set. Update the matrix and MODS.md.
```

---

## Phase 4b — EffectSet runtime: delete, update, camera shake

**Status: in progress (2026-09-26).** The spec below was written from the June disassembly
before any code, as in 4a, with the decompile used only as a map. The work is split into
three stages. Resume at the first stage that is not marked done. The phase is not done while
any `RUNTIME` row in `census.py` still has the state `stage N`.

| stage | scope | state |
|---|---|---|
| 1 | Delete: target-mesh any-handle, `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`. Update: target-mesh-gone hide, `GROUP_NAME` child position, June `GetCombineMatrix` (index rule, square unit, pet), `TRACE_MATRIX` stop-tracing gate and UI-major particle, `REVERSE_Y` index rule. Camera shake cited `same` | **done** 2026-09-26 (built, deployed `56e3eb6c9645fe75…`) |
| 2 | `SYNC_SHOW_BY_CREATE_UNIT` (plus the creation-time particle show @`321f251d`), `SOUND%d` playback and its dtor cleanup, `FOLLOW_UNIT_ROTATION(_CPARTICLE)`, per-frame `TRACE_MATRIX_IN_FIELD`, the particle `+0x31c`/`+0x320` write | **done** 2026-09-26 |
| 3 | Mesh triggers (`MESH_TRIGGER_DATA`, `MESH_TRIGGER_DATA_LIST` and every `TRIGGER_DATA%d` key), new `CXMeshInstance` members and its OnFrameMove countdown/restore, the play-anim wrapper, `EFFECT_SET_TOGGLE_VISIBILITY_TIMER`, the `+0x194` store at creation that 4a skipped | **done** 2026-09-26: all three stages built clean (`US_SERVICE` KTDXLIB, X2Lib, X2) and deployed to `X2_offline.exe` (SHA-256 `3b2cbece6764e1e2…`). **Play-test pending**; see "Result" |

**Which deployed data can fire each stage.** The four files March loads (`X2StateStartUp.cpp:164-177`)
were decompiled `mar-2014` into a scratch directory and counted by set:
- `EffectSet.src.lua` (3134 sets): `BONE_TRACE_TARGET_MESH` 159, `REVERSE_Y` 70, `DELETE_SHAKE_COUNT` 5.
- `EffectSet_INT` (60): `BONE_TRACE_TARGET_MESH` 7. `UIEffectSet` (52): none. `UIEffectSet_INT` (6):
  `REVERSE_Y` 1, `BONE_TRACE_TARGET_MESH` 1.
- None of them uses `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`, `SYNC_SHOW_BY_CREATE_UNIT`, `SOUND%d`,
  `FOLLOW_UNIT_ROTATION*`, `TRACE_MATRIX_IN_FIELD`, `MESH_TRIGGER_DATA*`, `GROUP_NAME` or
  `BONE_TRACE_TARGET_MESH_INDEX`, and no UI-major particle has `TRACE_MATRIX`.
- So only stage 1 can fire in a play-test today: the target-mesh delete/hide on the BTTM sets (Ranox,
  Perfect Storm, the fire Hamelling), `REVERSE_Y`, and `TRACE_MATRIX` on a pet or square-unit set.
  Stages 2 and 3 are exercised only after Phase 11 adopts June's files.

### June layout used for reading (never ported)

- **EffectSetInstance** (ctor `321df230`), on top of 4a's list:
  - `+0x54` stop-tracing flag (ctor clears it; set only by `STOP_TRACING_ON_TRIGGER`, `321dff3c`).
  - `+0x55` / `+0x58` toggle-visibility pending / timer (set by the trigger, `321dff8f`; **the ctor
    does not initialise `+0x55`**, March's member starts false).
  - `+0x6c` float, 1.0 in the ctor: an **alpha multiplier** (see "Documented, not transcribed").
  - `+0xc8` current delete-shake count; `+0xcc` an extra delete-shake count, 0 in the ctor.
  - `+0xd0` / `+0xd1` March's `m_bCreateEffectSetTraceUnit` / `m_bTraceUnitDieDeleteEffectSet`.
  - `+0xdc` the set's show flag (`321de4f0` stores it); `+0x181` / `+0x184` / `+0x194` sound state.
  - `+0x1b0` the `StopEffectSet( h, bool )` flag handed to every effect by the dtor `321e3a80`.
- **Trigger entry** (`MESH_TRIGGER_DATA_LIST/TRIGGER_DATA%d`, ctor `321f3be0`, `0x98` bytes). The
  parser's destinations line up with these after one 4-byte push:
  - `+0` `MOTION_NAME`; `+0x1c` `TRIGGER_TIME`; `+0x20` `PLAY_TYPE`; `+0x29` `USE_MESH_TRANSITION`.
  - Runtime: `+0x2c` the anim name when it last fired; `+0x48` the skin's play count then; `+0x4c`
    armed; `+0x72` fired-once.
  - `+0x50` `BOUND_MOTION_TYPE_LIST`; `+0x60` `TRIGGER_EFFECTSET_LIST`; `+0x70` toggle flag;
    `+0x71` `TRIGGER_ONCE`; `+0x73` `STOP_TRACING_ON_TRIGGER`; `+0x74` `BLOCK_TRIGGER`; `+0x78`
    `BLOCK_TRIGGER_TIME`; `+0x7c` `EFFECT_SET_TOGGLE_VISIBILITY_TIMER`.
  - `+0x80`/`+0x84` `ANIMTIME_ON_START_GAME` `ONCE`/`TIME`, `+0x81` its still-first byte (ctor 1);
    `+0x88`.. `_SQUARE`; `+0x90`.. `_UNITVIEWER`.
- **CKTDGXSkinAnim**, by field order against March's header: `+0x214` `m_fAnimCurrTime`, `+0x21c`
  `m_fAnimBeforeTime`, `+0x220` `m_fAnimMaxTime`, `+0x228` `m_NowAnimName`, `+0x248` `m_NowPlayCount`,
  `+0x2a8` `m_PlayType`, `+0x2ac` `m_bAnimEnd`, `+0x2b0` `m_fTransitionTime`.
- **CXMeshInstance**, June-only members after March's last one: `+0x634` inline trigger played,
  `+0x635`/`+0x638` trigger blocked / time left, `+0x63c` the anim to restore, `+0x658` its play
  type. `+0x4a4` alpha multiplier (June-only). `+0x194` restore-after-trigger (see corrections).

### Spec

| key / behaviour | June branch | March site | verdict | stage |
|---|---|---|---|---|
| `BONE_TRACE_TARGET_MESH` (delete) | `321e4020` @`321e4140`-`321e419d`: with is-target-mesh set, the set is deleted only when **every** entry's stored handle is -1 (`CMP [entry],-1`; a handle becomes -1 only when a position lookup invalidates it) | `OnFrameMove_DeleteEffect` | semantics-differ (March tested entry 0's mirror) | 1 |
| `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE` | `321e41a2`-`321e4231`: the templet flag **or** the instance's two trace-unit flags; then deleted when the trace target is null or its HP (`+0xc04`→`+0x20`, decoded) is not above 0 | `OnFrameMove_DeleteEffect` | new | 1 |
| `DELETE_SHAKE_COUNT` | `321e42ec`-`321e43ee`: limit = templet + instance `+0xcc`; a user trace target (`+0x264` = `GUT_USER`) whose facing (vfunc `0x170`) differs from its previous facing (vfunc `0x4f0`) adds one; deleted at the limit | `OnFrameMove_DeleteEffect` | same (`+0xcc` has no writer in March, so it is 0) | 1 (cite) |
| `PASSIVE_EFFECT` | `321e47c5`-`321e47d7`: an alive effect keeps the set alive unless it is passive | `OnFrameMove_DeleteEffect` | same | 1 (cite) |
| target mesh gone (update) | `321e0080` @`321e0170`-`321e021c`: per effect, **before** the trace test: is-target-mesh, `!TRACE_USER_TARGET_FORCE`, no live entry under the index rule, and not (`BONE_TRACE_TARGET_MESH` with `INDEX` < 0) → `EffectInstance::SetShowObject( false )` (`321de0c0`), then the next effect | `OnFrameMove_UpdateEffect` | semantics-differ (March `break`s the set's loop, and only for traced effects) | 1 |
| `TRACE`, `TRACE_MORE` | `321e07fc`-`321e081f`: an effect with none of `TRACE`/`TRACE_MORE`/`TRACE_MATRIX`, or not alive, is not moved; particles get `SetPosition( pos, TRACE_MORE, TRACE_MORE )` (`321e0cb7`-`321e0cca`) | `OnFrameMove_UpdateEffect` | same | 1 (cite) |
| `GROUP_NAME` (update) | `321e0825`-`321e0986`: a group child with `BONE_NAME` takes its stored offset through `GetCombineMatrix` (`321dc550`, `D3DXVec3Transform`, then / w); without `BONE_NAME`, `GetEffectPositionByBoneName( child bone )`. A null matrix leaves June's stack position uncomputed (@`321e0845`); not reproduced, the effect is not moved | `OnFrameMove_UpdateEffect` | new | 1 |
| `TRACE_MATRIX` (update) | `321e0fa4`-`321e146d`: skipped while the instance's stop-tracing flag is set; `SetUseDXMatrix` on the damage main effect, meshes 2-5 and particles 6 **and** 8 (March: 8 only). `GetCombineMatrix` `321dc550` adds the target-mesh index rule and returns bone frames for a **square unit** (viewer skin) and a **pet** (`GetXSkinAnim()`), where March returned NULL | `OnFrameMove_UpdateEffect`, `GetCombineMatrix` | semantics-differ | 1 |
| `REVERSE_Y` | `321e146d`-`321e159a`: type 8 with a billboard; the target mesh's facing when the index rule gives a live entry and `!TRACE_USER_TARGET_FORCE`, else the EffectInstance unit's | `OnFrameMove_UpdateEffect` | same (plus the index rule) | 1 |
| `CAMERA_SHAKE%d` and its 19 keys | `321dbcc0` @`321dbda3`-`321dc05a` (timing: `START_TIME` + delay, custom timer or pet / riding-pet / set skin anim time), `321dc060`-`321dc160` (done bit, then `LIMIT_DISTANCE` against `GetMyUnit()`'s position; -1 ignores it), `321dc16b`-`321dc403` (`SHAKE_TYPE` 1-9 → the nine `CKTDGCamera` calls with March's arguments; `31d014c0` writes Up's block and `31d01490` Down's, matching `DECT_UP`=1 / `DECT_DOWN`=2 and March's member order) | `OnFrameMove_StartCameraShake` | same | 1 (cite) |
| `SYNC_SHOW_BY_CREATE_UNIT` | `321e077d`-`321e07e9`: per effect, the owner's `GetShowObject()` against the set's show flag → `SetShowObject` on the whole set (`321de4f0`). At creation, particles take the set's flag (`321f251d`) | `OnFrameMove_UpdateEffect`, creation | new | 2 |
| `SOUND%d` (`EVENT_TIME`, `FILE_NAME`, `RATE`, `USE_GLOBAL_TIME`, `LOOP`) | `321e07ee`-`321e07f7` → `321dee80`, called per effect instance (a set with no created effect plays nothing): per-instance copy on first call; `USE_GLOBAL_TIME` against elapsed, else `EventTimer`; `rand()%100 < RATE`; played at the pet / owner / square position; `LOOP` re-arms on `m_bAnimEnd`, otherwise the entry is dropped; stopped sounds are released; the dtor stops the rest (`321da200`) | `OnFrameMove_UpdateEffect`, dtor | new | 2 |
| `FOLLOW_UNIT_ROTATION`, `FOLLOW_UNIT_ROTATION_CPARTICLE` | `321e159a`-`321e1b05` (inside the trace block): `GetUnitRotation` → mesh rotation (`31c1a3a0`) for the damage main effect and meshes; particles 6/8 add it to their own and set it (`31c1a030`), `_CPARTICLE` also on the child particles (`31cf1df0`) | `OnFrameMove_UpdateEffect` | new | 2 |
| `TRACE_MATRIX_IN_FIELD` (update) | `321e0d1a`-`321e0f9b`, UI-major particles only: the child or `BONE_NAME` bone frame (owner's set skin anim, or the target mesh) → `SetUseDXMatrix` + `POSITION_OFFSET` | `OnFrameMove_UpdateEffect` | new | 2 |
| `MESH_TRIGGER_DATA` (`TRIGGER_TIME`, `MOTION_NAME`, `PLAY_TYPE`) | `321e02fc`-`321e0409` → `321dfc40` @`321dffd0`-`321e0033`: only with an empty list; `TRIGGER_TIME` >= 0, not already played (mesh `+0x634`), anim time reached → play the motion at the mesh's default speed; the mesh restores the previous anim when it ends (`31c87cf2`-`31c87d48`) | `OnFrameMove_UpdateEffect`, `CXMeshInstance` | new | 3 |
| `MESH_TRIGGER_DATA_LIST`, `TRIGGER_DATA%d` keys | `321dfc40` @`321dfce0`-`321dffc4`: the owner's emotion (`user+0x89d8` / square `+0x608`, normalised by `3264ca10`) in `BOUND_MOTION_TYPE_LIST` arms an entry; at `TRIGGER_TIME` the motion plays (`PLAY_TYPE`, speed 1, `USE_MESH_TRANSITION`, start at `ANIMTIME_ON_START_*` `TIME` on its first use); then `TRIGGER_ONCE`, `STOP_TRACING_ON_TRIGGER`, `TRIGGER_EFFECTSET_LIST` (`PlayEffectSetByMeshPlayer`, type 2 always), toggle timer, `BLOCK_TRIGGER(_TIME)` | `OnFrameMove_UpdateEffect`, `CXMeshInstance` | new | 3 |
| `EFFECT_SET_TOGGLE_VISIBILITY_TIMER` | set by the trigger (`321dff89`-`321dff96`); `321e0105`-`321e0140` counts it down and flips the set's show flag | `OnFrameMove_UpdateEffect` | new | 3 |
| `ANIMTIME_ON_START_UNITVIEWER` | the unit-viewer branch of `321dfc40` (instance null) runs only from the unit viewer `329585f0` @`32958b12` | - | outside EffectSet | - |
| `FOLLOW_UNIT_VIEWER_SCALE`, `USE_EFFECT_SCALE_IN_UNIT_VIEWER` | read only by the unit viewer: `329585f0` @`32958a11`, `329590c7`; `32957980` @`32957e8e` | - | outside EffectSet | - |
| `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE` | unchanged from 4a: the unit-side registration is not located | unit side | deferred | - |

**Documented, not transcribed** (keyless, or no March caller):
- The alpha push, `321e033e`-`321e0353` and the other cases: the instance's `+0x6c` goes into mesh
  `+0x4a4` (June's mesh render setup multiplies both colour alphas by it, `31c87b80`
  @`31c88c05`-`31c88c1a`), particle `+0x1ac` and the damage effect (`31f75af0`). Nothing in the
  EffectSet region writes `+0x6c` but the ctor. It belongs to the effect-alpha system, which is
  absent (rule 5; `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` is already logged as unsupported).
- The deferred-stop queue: `OnFrameMove_DeleteEffect` first drains `CX2EffectSet+0x30`
  (`{handle, bool}` pairs, copied by `321f38a0` @`321e4051`, drained @`321e4091`-`321e40b6`) through `StopEffectSet( h, bool )` (`321e3ee0`),
  whose bool reaches every effect through `+0x1b0` in the dtor. There is no key, and March has no
  caller.
- Instance `+0xcc`, the extra delete-shake count: no writer in the EffectSet region.
- `CKTDGCamera`'s June setters skip the shake while a camera flag (`+0x19c`) is set: camera side,
  applies to every caller, not EffectSet.
- June's `CParticleEventSequence::SetShowObject` (`31cf1cd0`) also hides each particle, and June's
  `CEffect::SetPos` (`31f73df0`) also moves its start/passive/time particles: KTDXLIB particle
  (Phase 10) and `CEffect` (Phase 7a) differences, not EffectSet.

**Corrections to earlier records.**
- Mesh `+0x194` **has** a reader: June's mesh OnFrameMove `31c87b80` @`31c87d0e` restores the anim
  after an inline trigger only while it is 1. 4a's stage-2 member table and the funcmap row for
  `31c8d140` said no reader was found; the byte scan missed it. Stage 3 transcribes it.

### Result (2026-09-26)

**Member mapping** (the offsetof probe plus one June shape match, as in 4a):

| June | March | probe | shape match |
|---|---|---|---|
| CEffect particle lists `+0x864`/`+0x874`/`+0x884`, ParticleData `+0x94` | `m_StartParticleDataList` / `m_PassiveParticleDataList` / `m_TimeParticleDataList`, `m_hSeq` | `0x614`/`0x624`/`0x634`, `0x90` | the same `0x10` spacing; `31f75570` treats them alike |
| mesh `+0x5b4` (`31c1a3a0`) | `m_vRotate` (`SetRotateDegree`) | `0x460` | the rotate event `31c84e10` writes `+0x5a8`/`+0x5c0` as March writes `m_vRotateLocal`/`m_vRotateFinal` |
| particle `+0x3e0`/`+0x3ec` (`31c1a030`) | `m_AddRotate` / `m_AddRotateRel` | `0x2b4`/`0x2c0` | `31cf9570` adds both into the new particle's `+0x10c` as March's creation does |
| CParticle `+0x10c` | `m_vRotate` | `0x108` | the same site |
| particle `+0x31c`/`+0x320` | `m_bUseLand` / `m_fLandPosition` | `0x1f0`/`0x1f4` | creation `321f0fa6`-`321f0fbf` = March's `SetLandPosition( GetLandPos().y )` |
| mesh `+0x3d4` | `m_fAniPlaySpeed` | `0x2a4` | the speed the mesh already plays at |
| skin `+0x214`/`+0x228`/`+0x248`/`+0x2a8`/`+0x2ac` | `m_fAnimCurrTime`/`m_NowAnimName`/`m_NowPlayCount`/`m_PlayType`/`m_bAnimEnd` | `0xf0`/`0x104`/-/`0x174`/`0x178` | `31d28d00` = `Play( type )` (state, type, play count 0), `31d28e10` = `EventTimer( float )` |
| user `+0x89d8`, square `+0x608` | `GetNowEmotionId()` | - | both are compared with the sitting emotions and their cash variants (`0x19`/`0x1a`/`0x36`.. @`3249ccff`, `323a3474`), the values `3264ca10` folds |
| camera Up/Down (`31d014c0`/`31d01490`) | `UpCrashCamera`/`DownCrashCamera` | `m_fUpCrashGap` `0x94`, `m_fDownCrashGap` `0x88` | gap = 0, then speed and accel, in March's member order |

**What deployed data can show.** Stage 1 only: the target-mesh hide / delete on the 167 BTTM sets, the
5 `DELETE_SHAKE_COUNT` sets, `REVERSE_Y` (71), and `TRACE_MATRIX` on pets and village square units.
The play-test targets are the 4a ones: Rena's Perfect Storm and the fire Hamelling Attack X (damage
BTTM), the Ranox Burned Forest / Ashes Village monsters (mesh BTTM). A pet or the village exercises
the new `GetCombineMatrix` holders.

**Deviations, each commented in the code:**
- A `GROUP_NAME` child whose matrix is missing is not moved. June moves it to an uncomputed position.
- A non-user game-unit trigger owner has emotion `ET_NONE`. June reads the user field regardless.
- June's `TRIGGER_EFFECTSET_LIST` always passes type 2; that is reproduced as `ET_MESH_PLAYER_UI_MAJOR`.
- Mesh triggers skip a mesh with no skin anim.

**Exit status.** `census.py` passes every check, including the two new ones: no `4b@` placeholder, and
no `stage N` state. The matrix diff against HEAD, with line numbers normalized, changes only the
runtime table, the runtime notes and the checks.

**Play-test result (2026-09-26, exe `3b2cbece6764e1e2…`).** `offline_server.log` shows both lines the
exit criterion asks for:
- `JUNE_FX ES_UPDATE EffectSet_Rena_Perfect_Storm PerfectStorm02 BONE_TRACE_TARGET_MESH hidden=target_mesh_gone(321e0217)`,
  immediately followed by `JUNE_FX ES_DELETE EffectSet_Rena_Perfect_Storm - BONE_TRACE_TARGET_MESH deleted=no_target_mesh_handle_left(321e4197)`
  — five times over the run, one per Perfect Storm cast.
- The same pair for `EffectSet_Riding_Red_HAMELLING_Attack_X` (the fire Hamelling's Attack X), once.
- `0` `UNHANDLED`/`EXCEPTION` packets (`offline_packets.log`), and a clean `SHUTDOWN`.
- The Ranox mesh set (`EffectSet_NUI_RAGE_BABYFIG_Attack_A_Ready`) registered at creation
  (`registered_index=0(mesh)`, `offset=unit_facing`) but logged no `ES_UPDATE`/`ES_DELETE` in this
  run — its target mesh must have stayed alive throughout, which is not a failure; the delete/hide
  branch is proven by the damage-effect sets instead.

**A claim made in chat while planning this play-test turned out to be wrong and is recorded here so
it isn't repeated.** `GetCombineMatrix`'s square-unit and pet branches (new in stage 1) were offered
as a third test target ("a pet or village square-unit set with `TRACE_MATRIX`"). Checking the four
deployed effect-set files found no such combination exists:
- Only three effect sets ever play on a pet (`EffectSet_Summon_Pet`, `EffectSet_Evolution_Pet`,
  `EffectSet_Cheer_Pet`), none with `TRACE_MATRIX`.
- The only literal `PlayEffectSet(name, pSquareUnit)` call sites are `EffectSet_LevelUp` and
  `EffectSet_LevelUp_LandPosition`, neither with `TRACE_MATRIX`. The relationship/couple effects take
  their name as a runtime variable and could not be resolved to a literal.
- No dev/cheat command in this build plays an arbitrary effect set on a square unit; the one
  Lua-callable binding (`PlayEffectSet_LUA`) only accepts a `CX2GameUnit*`.

So those two branches are untestable with current March data, same as the several other stage-2/3
branches already marked "fires from Phase 11" above. The branches are still cited and transcribed;
they just have no play-test evidence, and none is possible until June's own files are adopted.

**Paste with:** Starting context + this section.

- **June source:** `FUN_321e4020` (delete), `FUN_321e0080` (update, 7 KB), `FUN_321dbcc0`
  (camera shake), all confirmed by call order.
- **March target:** `X2EffectSet.cpp:964/:1261/:1586`.
- **Inputs left by Phase 4a** (see its stage 2/3 results):
  - `EffectInstance::m_vecMeshTriggerData`;
  - the `GROUP_NAME` members `m_bGroupChild` / `m_wstrGroupChildBoneName` / `m_vGroupChildOffset`, read
    by UpdateEffect @`321e0825`-`321e0986` and @`321e0d49`;
  - `GetEffectPositionByBoneName` (`321dd590`), which exists for those reads;
  - `321dc550`, a `GetCombineMatrix` candidate.

**Scope.**
- Delete-time keys: `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE`, and the trigger block's
  `STOP_TRACING_ON_TRIGGER`, `BLOCK_TRIGGER*`, visibility toggle.
- Per-frame update keys: follow rotation, viewer scale, trace-matrix-in-field.
- Camera-shake sub-block semantics.
- Plus any existing-key semantics June changed.

**Exit criteria.** As 4a, with `JUNE_FX ES_UPDATE` and `ES_DELETE` lines.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 4b"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phase 4a is done (creation-time behaviour under
SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME; extend that flag). Branch: mods/lanox-port.

Your task: transcribe June's OnFrameMove_DeleteEffect (FUN_321e4020), UpdateEffect
(FUN_321e0080) and StartCameraShake (FUN_321dbcc0) differences into X2EffectSet.cpp:964 /
:1261 / :1586, covering every remaining EffectSet key and any changed semantics of
existing keys. Cite June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when the matrix has
no EffectSet key without a runtime citation (or a documented reason it has none), and
JUNE_FX ES_UPDATE / ES_DELETE lines fire. Update the matrix and MODS.md.
```

---

## Phase 5 — DamageEffect CEffect constructor keys

**Status: done 2026-09-26**, play-tested. `census.py` shows 0 Phase-5 keys `to-implement`
(all 17, `unsupported`, and both `binary-only` rows now `implemented`, semantics `same`
against June for every checked key). `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE` (+
`SERV_IRUHADEV_JUNE_FX_DEBUG`'s `CE_TEMPLET` line). Full detail, including three
`census.py` classification quirks found and reported rather than fixed, is in `MODS.md`
and `june2017_re/README.md`'s "Hand-verified (Phase 5)".

Things the next phases should know:

- **June computes one master "read `_ULTIMATE` first" flag** from `APPLIED_SKILL_ID`
  (read inside the ctor's own `BeginTable("DAMAGE_DATA")`) and a skill-memo lookup
  (`FUN_31fcf3e0`, **candidate, not identified**). It gates every `_ULTIMATE` read in the
  ctor (`MAIN_EFFECT_NAME`, `FOLLOW_LINE(_NEW)`, `DAMAGE_DATA`, `DAMAGE_DATA_NEXT`,
  `DAMAGE_DATA_CHANGE_TIME(_GLOBAL_TIME)`). Forced `false` this phase (system unsupported),
  so today's behaviour is unchanged; identifying `31fcf3e0` and wiring the flag for real is
  Phase 7/8's, not a fresh Phase 5-style key hunt.
- **`DAMAGE_DATA_CHANGE_TIME(_GLOBAL_TIME)` is a real semantics-differ**: June reads it as
  an indexed float array at this scope, not March's scalar. The array read was added
  alongside the scalar (new `vector<float>` members, additive on current data). Phase 7 or
  8 needs to decide how the two forms coexist once June's own array-shaped data lands.
- **`DAMAGE_DATA_NEXT` chain**: numbered continuations start at **2**, not 0 or 1, and are
  a new `DamageEffectDataInLua::m_vecDamageDataNextChain` alongside the existing single
  `m_pDamageDataNext`. Nothing consumes the chain yet (Phase 7).
- **`IGNORE_MI_HUGE`** (false) gates a scale override via June `31f6fdc0`
  (**not identified**) that overwrites the main effect's `(1,1,1)` scale — a Phase 7 item.
- **The one available test case, `APPLIED_SKILL_ID` on
  `DamageEffect_SA_RHB_Lockon_Stratos_Attack`, is Rose's and could not be triggered**: Rose
  isn't ported yet. The `unsupported` log line is coded and reads real packed data
  (confirmed by the play-test's clean run and lack of regressions), but has not been seen to
  fire. Worth a follow-up play-test once Rose exists.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **June source:** `FUN_31f7aab0` (20 KB, 4,457 disassembly lines), **excluding** the
  sub-loader call block at `0x31f7e7b2`–`0x31f7f976`, which Phase 6 owns.
- **March target:** `CX2DamageEffect::CEffect::CEffect` (`X2DamageEffect.cpp:1240`) and
  `CEffect` in `X2DamageEffect.h`.

**Scope.** Every top-level CEffect key the matrix assigns here: the list in the ledger,
plus anything Phase 1 added. For keys March already reads, check June's default, type and
post-processing, and fix any that differ (`semantics-differ`).

**Watch for:**
- `FUN_31f8ba80`, June's `"%s"`/`"%s_ULTIMATE"` GetValue wrapper. Some keys have an
  `_ULTIMATE` variant read through it; find out whether March has the same mechanism
  before adding one.
- The two `SetDamageData` calls at `31f7c63f`/`31f7c66c`. Record which `DAMAGE_DATA` keys
  they pass, as a handoff to Phase 8.

**Exit criteria.** `census.py` shows 0 CEffect top-level keys `to-implement`, a clean
build, and a load-time `JUNE_FX CE_TEMPLET` line for a named Ranox damage effect.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 5"), june2017_re/README.md,
funcmap.tsv, and the Phase-5 rows of the matrix. Branch: mods/lanox-port.

Your task: transcribe the top-level keys of June's CEffect constructor (FUN_31f7aab0,
excluding the sub-loader call block 0x31f7e7b2-0x31f7f976 that Phase 6 owns) into
CX2DamageEffect::CEffect::CEffect (X2DamageEffect.cpp:1240), under a new
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE flag. Cite June address ranges. Check existing keys
for changed defaults/types too. Parse only; runtime is Phase 7. Note what the two
SetDamageData calls at 31f7c63f/31f7c66c pass, for Phase 8.

X2DamageEffect.{h,cpp} are CP949: byte-level edits only, then check file + git diff --stat.
Build, deploy, ASK before launching. Done when census.py shows no Phase-5 keys left.
Update the matrix and MODS.md.
```

---

## Phase 6a — Sub-loaders I: CREATE_DAMAGE_EFFECT, die and hit damage effects, die conditions

**Status: done 2026-09-26.** `census.py` shows 0 Phase-6a keys `to-implement` (two `binary-only`
rows remain, deliberately: `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` and
`DAMAGE_DATA_NEXT_ULTIMATE%d`, already implemented at `DE.top` by Phase 5, land under
`DE.DIE_CHECK_CONDITION` too only because `CE` is listed as a June reader of that root -- not
parsed twice). `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE` extended (+
`SERV_IRUHADEV_JUNE_FX_DEBUG`'s new `DDE_TEMPLET`/`DDE_CONDITION` lines, throttled). Full detail
in `MODS.md` and `june2017_re/README.md`'s "Hand-verified (Phase 6a)".

Things the next phases should know:

- **`FUN_31f68f30`/`FUN_31f6ceb0` are the full `"<base><index>"` loop, not a "per-entry body"**
  as Phase 0's table and this plan's text both said (`funcmap.tsv` corrected). The wrapper
  (`FUN_31f6e7e0`/`FUN_31f6e850`) passes the base name as the *only* argument; the callee formats
  `"%s%d"` itself and returns whether index 0 existed. The wrapper tries
  `"DIE_DAMAGE_EFFECT_ULTIMATE"`/`"HIT_DAMAGE_EFFECT_ULTIMATE"` first when its own inline gate
  (the same unidentified `FUN_31fcf3e0` skill-memo callee Phase 5's `m_bUseUltimateDamageData`
  uses) passes, falling back to the base name only if that found nothing. March reuses
  `m_bUseUltimateDamageData` (still forced false) rather than re-deriving the gate.
- **`INHERIT_LIFE_TIME` is a bool (default false) in all three scopes** (`CREATE_DAMAGE_EFFECT`,
  `DIE_DAMAGE_EFFECT`, `HIT_DAMAGE_EFFECT`) -- not the float "inherit at this rate" its name
  suggests.
- **The die-condition parser has no March equivalent before this phase.** A shared helper
  (`LoadDieCheckConditionBlock`) opens `AND_OPERATION`/`OR_OPERATION` by a runtime table-name
  parameter and loops a numbered `CONDITION_%d`, tagging every parsed entry `bIsOrOperation` and
  pushing all of them into one flat `m_vecDieCheckCondition` (confirmed by tracing June's third
  call argument into the pushed entry's own field). `DAMAGE_EFFECT_DIE_CONDITION_TYPE`'s values
  come from `june2017_re/june_lua/Enum.src.lua`. Parse only; combining entries by
  `bIsOrOperation` into an actual die decision is Phase 7c's.
- **census.py's `ROOTS` needed real registrations** for this new scope (`LoadDieCheckCondition`
  added to `CE_LOADERS`; `ce('LoadDieCheckCondition')`/`ce('LoadDieCheckConditionBlock', ())`
  added to `DE.DIE_CHECK_CONDITION`'s March list) -- its March column was `[]` before, since no
  March reader existed yet. No change to the matching logic (`march_reads`/`path_match`) itself.
- **Play-test target for `DAMAGE_EFFECT_Trap_Fire_Big`** (`LOOP_ADD_POS_X`/`_Y`,
  `SET_DIE_WHEN_MASTER_EFFECT_DIE`, inside `CREATE_DAMAGE_EFFECT0..3`): the `TRAP_FIRE_BIG` NPC,
  placed in the Ranox Burned Forest dungeon at every difficulty. ~~Not yet play-tested this
  session.~~ **Play-tested 2026-10-03** (dev-console pass T1, both steps passed, exe
  `cccaf8fb…`).

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **June source** (roles corrected in Phase 0; see `funcmap.tsv`):
  - `FUN_31f6ac60`: the **`CREATE_DAMAGE_EFFECT%d`** loader. It opens plaintext
    `CREATE_DAMAGE_EFFECT` @`31f6acc5` and reads `LOOP_*`, `SET_DIE_WHEN_MASTER_EFFECT_DIE`,
    `INHERIT_*`, `SET_GROUP_ID`, `AUTO_DIE`, `APPLY_ROTATED_OFFSET`, `APPLY_ROTATE_ORIGIN`.
  - `FUN_31f68f30`, reached through wrapper `FUN_31f6e7e0`: the **die** damage-effect
    loader (`DIE_DAMAGE_EFFECT[_ULTIMATE]`). The ctor calls the wrapper @`31f7e7f7`.
  - `FUN_31f6ceb0`, reached through wrapper `FUN_31f6e850`: the **hit** damage-effect loader
    (`HIT_DAMAGE_EFFECT[_ULTIMATE]`). The ctor calls the wrapper @`31f7e84d`.
  - `FUN_31f66700`: the die-condition parser. The `DAMAGE_EFFECT_DIE_CONDITION_TYPE` enum
    values come from the June `Enum.lua`, and the comparison semantics come from this
    function.
- **March target:** `LoadDamageEffect` (`:6822`, `CREATE_DAMAGE_EFFECT`, which fills
  `m_vecCreateDamageEffect`), `LoadDieDamageEffect` (`:6624`), `LoadHitDamageEffect`
  (`:6900`), the `DamageEffectData` struct, and a new die-condition structure.

**Named test case.** `DAMAGE_EFFECT_Trap_Fire_Big` uses `LOOP_ADD_POS_X/Y` and
`SET_DIE_WHEN_MASTER_EFFECT_DIE`, all inside `CREATE_DAMAGE_EFFECT0..3`
(`june_lua/DamageEffect.src.lua:300710`), so it is `31f6ac60`'s. It is one of the two
blocks listed in `RANOX_EFFECT_ENGINE_GAPS.md` §1. After this phase it must parse
completely.

**Exit criteria.**

- The census is clean for these three sub-tables.
- A load-time `JUNE_FX DDE_TEMPLET DAMAGE_EFFECT_Trap_Fire_Big LOOP_ADD_POS_X=...` line
  appears.
- The runtime (the loop spawn itself) belongs to Phase 7c. Note that in the code.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 6a"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phase 5 is done (CEffect top-level keys under
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE; extend that flag). Branch: mods/lanox-port.

Your task: transcribe June's CREATE_DAMAGE_EFFECT%d loader (FUN_31f6ac60), die
damage-effect loader (FUN_31f68f30, via wrapper FUN_31f6e7e0), hit damage-effect loader
(FUN_31f6ceb0, via wrapper FUN_31f6e850) and die-condition parser (FUN_31f66700) into
X2DamageEffect.cpp's LoadDamageEffect (:6822) / LoadDieDamageEffect (:6624) /
LoadHitDamageEffect (:6900) and their structs. Take the DAMAGE_EFFECT_DIE_CONDITION_TYPE
values from June's Enum.lua. Parse only; the loop and condition runtime is Phase 7c. Cite
June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when
DAMAGE_EFFECT_Trap_Fire_Big parses completely (JUNE_FX DDE_TEMPLET line) and the census
is clean for these sub-tables. Update the matrix and MODS.md.
```

---

## Phase 6b — Sub-loaders II: particle, lock-on, sound, after-image, black hole, sine move and the small loaders

**Status: done 2026-09-27.** `census.py` shows 0 Phase-6b keys `to-implement` (two binary-only
rows remain, named exceptions like Phase 6a's two: `AT_RAID_FIELD`, parsed identically to its
three `AT_*` siblings but never observed in live data, and `HIT_EFFECT_SET%d`, whose sub-table is
parsed but never opened by any in-scope June data). `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE`
extended (+ `SERV_IRUHADEV_JUNE_FX_DEBUG`'s new `LOD_TEMPLET` line). Full detail in `MODS.md` and
`june2017_re/README.md`'s "Hand-verified (Phase 6b)". Built `US_SERVICE` and deployed to
`X2_offline.exe`; not yet play-tested this session.

Things the next phases should know:

- **`LoadLockOnData`/`LoadBlackHoleData` are now name-parameterised gates over a body helper**
  (`LoadLockOnDataBlock`/`LoadBlackHoleDataBlock`), mirroring the `_ULTIMATE`-first,
  base-fallback shape the die/hit loaders got in Phase 6a. `m_bUseUltimateDamageData` stays
  forced false, so only the base table is read today.
- **`census.py`'s brace-depth tracker needs `BeginTable` written inside the `if( true == ... )`
  condition itself**, not as a separate assignment statement before a bare `if` on the stored
  bool -- the latter leaks the pushed table name open for the rest of the function (no same-depth
  `EndTable()` closes it), silently nesting every later read under it. Found this the hard way on
  `LOCK_ON_RANGE_FROM_OWNER`/`DUNGEON_TARGET_RANGE_MIN_MAX`/`PVP_TARGET_RANGE_MIN_MAX`; re-running
  `census.py` and checking a key's recorded path against expectation caught it.
- **`LOCK_ON_RANGE_FROM_OWNER`'s four `AT_*` members are parsed unconditionally**, all four,
  regardless of game mode. June itself selects only one at ctor time from
  `*(int*)(DAT_3377585c+0x8c)` (values seen: 9, 0x15/21 with a further raid-field sub-check, 0x17
  with no lua read at all, else) -- not identified. Wiring the actual selection is Phase 7b's.
- **`INIT_TARGET` is parsed but not acted on.** June's post-read behaviour (an immediate
  nearest-target lookup at ctor time, storing into the lock-on UID fields) is runtime, Phase 7b's.
- **`NOT_TARGET_PRE_LOCK_ON_TARGET` semantics-differ**: June also reads this key as an int
  alongside March's existing bool. New parallel `m_iNotTargetPreLockOnTarget` member; Phase 7b
  decides how the two coexist.
- **`TOUCH_LAND_DAMAGE_EFFECT%d` reuses `DamageEffectData`** (confirmed by matching June's
  decrypted key order 1:1 against that struct's fields), rather than a new struct.
  `HIT_EFFECT_SET%d`/`DIE_CREATE_NPC%d` each got a small dedicated struct instead, since neither's
  key set overlaps `DamageEffectData`.
- **Not play-tested this session.** The plan's named lock-on case
  (`DamageEffect_SA_RHB_Lockon_Stratos_Attack`) is Rose's and can't fire before Rose is ported, as
  Phase 5 already noted. The matrix's own examples for the new keys (`GUIDE_FIRE_BALL` for
  `LOCK_ON_RANGE_FROM_OWNER`/`DUNGEON_TARGET_RANGE_MIN_MAX`, `DamageEffect_A_RMH_KS_83_Grenade`
  for `TOUCH_LAND_DAMAGE_EFFECT%d`, ...) are real live data, but which of them is reachable without
  Rose was not checked this session -- worth a follow-up play-test to confirm a `JUNE_FX
  LOD_TEMPLET` line actually fires.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **June source** (all identified in Phase 0; see `funcmap.tsv`):
  - Particle: `FUN_31f66e60` and its four wrappers. `31f6a7b0` is `START_PARTICLE`,
    `31f6a940` is `PASSIVE_PARTICLE`, `31f6aad0` is `TIME_PARTICLE`, and `31f6cd00` is
    `HIT_PARTICLE`.
  - Touch-land damage effect: `31f76a90` (`TOUCH_LAND_DAMAGE_EFFECT`, June-only; no March
    loader). *Not* passive particles, as this section used to say. `31f6ceb0` moved to
    Phase 6a, because it is the hit damage-effect body.
  - Lock-on: `31f71930`, with the `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE` enum and
    `AT_PVP/AT_FIELD/AT_RAID_FIELD/AT_DUNGEON`.
  - Sound: `31f73f30` (`SOUND%d`).
  - After-image: `31f74a80` (`AFTER_IMAGE`).
  - Black hole: `31f637e0` (`BLACKHOLE_DATA`; its keys are `DRAGGING_*`, hence the old
    "drag" label).
  - Sine move: `31f64200` (`SIN_MOVE_DATA`).
  - The small loaders:
    - `31f650f0`: `CAMERA%d`, March `LoadCameraData`.
    - `31f65730`: `SCREEN%d`, March `LoadScreenData`.
    - `31f64c80`: `HIT_EFFECT_SET%d`, June-only.
    - `31f66230`: `DIE_CREATE_NPC%d`, June-only.
- **March target:** `LoadParticleTable`, `LoadLockOnData` (`:6342`), `LoadBlackHoleData`
  (`:6410`), `LoadSinMoveData` (`:6436`), `LoadCameraData` (`:6753`), `LoadScreenData`
  (`:6777`), `LoadSoundData` (`:6801`), `LoadAfterImage` (`:7019`), plus new loaders for
  the June-only sub-tables.

**Named test case.** `DamageEffect_SA_RHB_Lockon_Stratos_Attack` for lock-on. Its
`NUMERICAL_DATA_INDEX`/`APPLIED_SKILL_ID` are unsupported systems: parse, store and log.

**Exit criteria.** `census.py` shows 0 DamageEffect keys `to-implement`, with named
exceptions.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 6b"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phases 5 and 6a are done (extend
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE). Branch: mods/lanox-port.

Your task: transcribe the remaining CEffect sub-loaders -- particle (FUN_31f66e60 + its 4
wrappers), TOUCH_LAND_DAMAGE_EFFECT (31f76a90), lock-on (31f71930), sound (31f73f30),
after-image (31f74a80), black hole (31f637e0), sine move (31f64200), CAMERA%d (31f650f0),
SCREEN%d (31f65730), HIT_EFFECT_SET%d (31f64c80) and DIE_CREATE_NPC%d (31f66230); roles
and evidence are in funcmap.tsv -- into X2DamageEffect.cpp's Load* family. Parse only. Unsupported-system keys (numerical data,
memo, skill-ID-driven systems) get parse + store + one JUNE_FX unsupported log line. Cite
June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when census.py
shows 0 DamageEffect keys to-implement. Update the matrix and MODS.md.
```

---

## Phase 7a — DamageEffect runtime: identify, then OnFrameMove, trace and follow-line

**Status: done 2026-09-27, play-tested.** `JUNE_FX CE_FRAME ... USE_GLOBAL_TIME` fired for Rena's
Freezing Bolt and Chung's grenade launcher, dash-jump ZX bomb and Carpet Bombing. The June-only
`REFRESH_LAND_POS` crash-land guard fired too (`CHUNG_CARPETBOMBING_SMALL`, probe `y == -1`,
skipped). Clean shutdown, no `UNHANDLED`/`EXCEPTION`, no visible regressions. New flag `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`. Full detail is in
`MODS.md`; identification is in `funcmap.tsv`; the matrix has 14 `7a` runtime rows.

Things the next phases should know:

- **Identification.** June's `CEffect::OnFrameMove` is `31f88790`: the one vtable slot, which Phase
  0 had as the deleting dtor. Ghidra has no function there, so disassemble it with capstone
  (`june2017_re` has no helper; a 20-line script over `unpacked_x2.exe` works, `VA - 0x31aa0c00`).
  - `CX2DamageEffect::OnFrameMove` is `31f79fb0`.
  - Callees, all confirmed: BlackHole `31f6eea0`, LockOn `31f83aa0`, SineMove `31f6f3d0`, Sound
    `31f6eca0`, `FollowLine` `31f70ee0`, `SetDamageDataNext` `31f799c0`.
  - **Phase 7b's `OnFrameMove_LockOnData` is `31f83aa0`.** It is called with `fElapsedTime` @`31f8a668`.
- **Unit and game virtuals are mapped by RTTI vtable** (`funcmap.tsv`):

  | Slot | Function |
  |---|---|
  | GUUser `334781f4` `0x80` | `GetBonePos` (bool form) |
  | `0x138` | `GetPos( bFuture )` |
  | `0x150` | `GetRotateDegree` |
  | `0x158` | `GetYRotateDegree` |
  | CX2Game `3348bf94` `0x98` | `GetMyUnit` |

  - `unit+0x264` = `m_GameUnitType`.
  - The typed `g_pX2Game` fetch is `31aad500`/`31aada50` with the CX2Game type descriptor
    `33722a70`.
- **x87 hand traces are unreliable; emulate.** A small capstone-driven x87 emulator settled the
  `USE_OWNER_ATTACK_SPEED` formula and the `REFRESH_LAND_POS` `-1.0` guard. On its first run it
  also caught its own bug: capstone prints the popping form `DE E9` as `fsubp st(1)`, which means
  `st1 = st1 - st0`, then pop.
- **Deferred (not transcribed):**
  - **`FOLLOW_LINE_GRAVITY`'s near-line case, Phase 10.** Within 30 of the line, June adds 20 to y
    and clears a `CXMeshInstance` move-enable flag (June mesh `+0x633`). June's mesh OnFrameMove
    `31c87b80` then zeroes velocity (`+0x534`), `+0x558` and `+0x5a8` before integrating. March
    has no such flag.
  - **`IGNORE_MI_HUGE`.** The ctor branch `@31f7c795` scales by `m_DamageData+0x5d0`, which
    nothing in `SetDamageDataFromLUA` writes. Find the writer first (Phase 8a?).
- **Found, not ported, owned elsewhere:**

  | Item | June | Owner |
  |---|---|---|
  | Die-condition evaluator | `31f765b0` @`31f888e5` | Phase 7c |
  | Touch-land check | `31f83380` @`31f8a38b` | Phase 7e (7c left it) |
  | `CREATE_DAMAGE_EFFECT` loop (child effect via `31f8ff00`, `IGNORE_MI_HUGE`-style scale `31f6fdc0`, `INHERIT_*`, `AUTO_DIE`, `SET_GROUP_ID`, loop counters `+0x90`/`+0x98`) | `31f89312`–`31f89fa4` | Phase 7c |
  | First-frame copy of templet vectors into per-instance lists | `31f8a4b1`–`31f8a63c` | 7c |
  | Per-frame owner-rotation refresh of `+0x118` | `31f887a8` | — |
  | `+0xec` flag | — | — |
  | Accumulator | `31f75da0` | — |
  | FollowLine's changed line probe | `31c6c310`, `31c5df50` | — |
  | `DamageCheck`'s extra args `(0, -1, -1)` | — | Phase 9 |

- **Play-test reachability.** The June-only branches are mostly unreachable on this build's data
  and classes:
  - `USE_OWNER_ATTACK_SPEED`'s users are all Ain skills, and Ain is absent.
  - `FOLLOW_LINE_WITH_CUSTOM_ROTATE`'s are Rose's, plus a PvP-hero NPC.
  - `FOLLOW_LINE_GRAVITY`'s include Rena Trapping Ranger (Karma Spark, Thorn Trap), but the merged
    data may not carry the key.
  - The `CE_FRAME` line that should fire on any data is `USE_GLOBAL_TIME`: any damage effect using
    global time, when its window opens.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

**Identification comes first, and nothing is transcribed until it is done.**

- Find June's `CX2DamageEffect::OnFrameMove` (March `:84`). It loops over instances
  calling `CEffect::OnFrameMove`, then deletes the dead ones.
- Go down from it to `CEffect::OnFrameMove`, then to its callees:
  `OnFrameMove_SoundData`, `BlackHoleData`, `LockOnData`, `SineMoveData`, `Trace`,
  `FollowLine`.
- `31f85910` (11.7 KB, 248 calls) is the leading candidate for `CEffect::OnFrameMove` and
  `31f83aa0` a secondary one. **Confirm them or replace them.**
  - Evidence that counts: the call structure matching March, and the decrypted strings.
  - Evidence that does not count: size alone.
- Record everything in `funcmap.tsv` as `confirmed`.

**Then transcribe** the per-frame semantics for the Phase-5 keys:
- `TRACE_OWNER_BONE*`, `FOLLOW_LINE_GRAVITY`, `FOLLOW_LINE_WITH_CUSTOM_ROTATE`
- `REFRESH_LAND_POS`, `USE_GLOBAL_TIME`, `USE_OWNER_ATTACK_SPEED`, `IGNORE_MI_HUGE`
- `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`

Also transcribe any changed semantics in existing per-frame code.

**Exit criteria.**
- The runtime functions are confirmed in `funcmap.tsv`.
- Each key has a cited branch.
- A `JUNE_FX CE_FRAME` line fires in a play-test.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 7a"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phases 5-6 are done: all DamageEffect keys are parsed but not
consumed. Branch: mods/lanox-port.

Your task, step 1 (no code): identify June's CX2DamageEffect::OnFrameMove and
CEffect::OnFrameMove and its per-frame callees by call graph. FUN_31f85910 is only a
size-based candidate -- confirm it or replace it with evidence, then record everything in
funcmap.tsv. Step 2: transcribe the per-frame behaviour of the Phase-5 keys (trace owner
bone, follow-line gravity/rotation, refresh land pos, global time, owner attack speed,
alpha-by-option, ...) and any changed semantics into X2DamageEffect.cpp:1836 and its
helpers, under a new SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME flag. Cite June address
ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when each key has
a cited branch and JUNE_FX CE_FRAME fires. Update the matrix and MODS.md.
```

---

## Phase 7b — Runtime: lock-on

**Status: done 2026-09-27, play-tested in battle fields** (clean shutdown, no `UNHANDLED`/`EXCEPTION`):
- `Damage_Chung_CDC_Bullet_Blitz` logged the whole list cycle: `TARGET_LIST_INSERT` → `IN_RANGE_PICK
  skipped=1` → `IN_RANGE_MISS set=4` → `TARGET_LIST_MISS_RULE` CLEAR → `set=0`.
- `GUIDED_ARROW_MEMO` logged `RANGE_FROM_OWNER_REJECT range=1000`, the battle-field default.
- Not exercised yet: the `LOT_UID_VECTOR` seed re-target (`UID_VECTOR_RETARGET`/`SEED_PICK`).
  - **2026-10-03: no real skill reaches it.** Guided Arrow (`CreateGuideArrow`) and Scare Chase
    (`X2GUChung.cpp:13423`) both switch their effects to `LOT_NEARST_UID_VECTOR` at runtime, even
    though their templets say `LOT_UID_VECTOR`.
  - Use the dev-console pass's T2 console route instead: `GUIDED_ARROW` plus `SetLockOnNPCUID`
    plus `SetNowHP(0)`.
  - **Exercised 2026-10-03 (T2 passed):** five `UID_VECTOR_RETARGET reason=dead` / `SEED_PICK`
    pairs.

`/code-review` (high) is done, with 3 fixes rebuilt and redeployed; see `MODS.md`:
- a live-instance guard on the `EffectToUnit` hook;
- the in-house message box no longer fires for type 8;
- the `Always.h` flag description covers the lock-on work.

The other 7 findings are June's own behaviour (`m_Pos` for case 8, `rand()`, the forward steer at
DirSpeed 0, the one-slot UID write), the chosen design, or deferred. Identification
was finished on Opus in the same session. `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME` is extended
(`X2DamageEffect.{h,cpp}`, plus one hook in `X2DamageManager.cpp`), and `JUNE_FX CE_LOCKON` lines
are added. Full detail is in `MODS.md`, the functions and offsets are in `june2017_re/funcmap.tsv`
(18 new rows), and the matrix has 24 `7b` runtime rows. Things the next phases should know:

- **June's lock-on functions:**

  | June VA | Function |
  |---|---|
  | `31f83aa0` | `OnFrameMove_LockOnData` (jump table `31f858e4`) |
  | `31f73a10` | `VectorLockOn` |
  | `31f647c0` | the `LOCK_ON_RANGE_FROM_OWNER` gate |
  | `31f8cc80` | the `LockOnData` ctor, with the full `+0x920` layout |
  | `31f758d0` | `SetLockOnNearstTarget`, unchanged from March |

  The kickoff named `SetLockOnNearstTarget` as a June source, but it is not on any of the June
  paths above.
- **The three June picks are `CX2Game` methods with no March counterpart:**
  - `322dadd0`: seeded.
  - `322d7670`: in-range nearest, with the target list.
  - `322daa10`: random in-range; used only by case 8.

  They are re-expressed as `CEffect` helpers over `GetUnitList()`.
  - `322d10b0` is March's `GetLockOnNearstTarget`.
  - An early note in this section called `322daa10` the in-range nearest pick. That was wrong.
- **`NOT_TARGET_PRE_LOCK_ON_TARGET` is an int type**, `NTPLOTT_*`; March's `TRUE` = 1 = CLEAR.
  - The list is per effect and is filled on **hit**, by `EffectToUnit` `3262b6e0`.
  - It replaces March's game-wide, filled-on-pick set, which is kept in `#else`.
- **Live behaviour changes on deployed data:**
  - The `LOT_UID_VECTOR` re-target index differs from March's.
  - At seed −1 June finds a target where March found none.
  - In battle fields, lock-ons that have a table are limited to 1000 from the owner.
  - The `IN_RANGE` ranges now come from data; their defaults equal March's constants.
  - `ONLY_FRONT` together with `TIME_UNLOCK_FROM_START` now re-bases the ONLY_FRONT origin when
    the delay ends. `DamageEffect_ENS_HA_Machine_Gun_Bullet` and the two
    `Oberon650_AngelHeart_*_Z` attacks set both keys.
  - None of the Phase-6b keys appears in the packed Sep 27 `DamageEffect.lua` either (re-checked).
- **Found for Phase 7d:** `31f85910` is almost certainly `CEffect::AttackResult`, the call that
  ends `EffectToUnit`.
- **Not ported:**
  - The NPC filter byte `+0x8fb6`, which is not identified.
  - The NPC aim point's battle-field tail.
  - The `XS_PVE_GAME` branch.
  - The raid-field mapping, which is a candidate.
- **The log check for keys with no deployed user moves to Phase 11:** `INIT_TARGET`, `Y_DIR_INIT`,
  `UNLOCK_START_AFTER_ATTACK`, `RETARGET_COOLTIME_AFTER_ATTACK`, the bone name,
  `LOCK_ON_OFFSET_Y_POS` and case 8.
- **The play-test that settles this phase:**
  - Chung (Deadly Chaser) Bullet Blitz in a dungeon with several monsters. It is `IN_RANGE` with
    `NOT_TARGET_PRE_LOCK_ON_TARGET = TRUE`, so expect `TARGET_LIST_INSERT` / `IN_RANGE_PICK` /
    `IN_RANGE_MISS`.
  - A `LOT_UID_VECTOR` skill such as Chung's Scare Chase or Rena's Guided Arrow, killing its
    target mid-flight. Expect `UID_VECTOR_RETARGET` + `SEED_PICK`.
  - Optionally, a battle field for `RANGE_FROM_OWNER_REJECT`.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **June source:** June's `OnFrameMove_LockOnData`, `SetLockOnNearstTarget` and
  `VectorLockOn` (identified in 7a; if not, identify them first).
- **March target:** `X2DamageEffect.cpp:2752`, `:7341`, `:6465`.

**Scope.** The Phase-6b lock-on keys:
- `NOT_LOCKON_INVINCIBLE_TARGET`, `INIT_TARGET`, `RANDOM_S…`
- the per-game-type `AT_*` flags
- `LOCK_ON_TARGET_BONE_NAME`, `LOCK_ON_OFFSET_Y_*`, `NOT_FIND_TARGET_MOVE_STOP`
- the `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE` semantics

**Named test case.** `DamageEffect_SA_RHB_Lockon_Stratos_Attack`.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 7b"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phase 7a is done (extend
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME). Branch: mods/lanox-port.

Your task: transcribe June's lock-on runtime (OnFrameMove_LockOnData,
SetLockOnNearstTarget, VectorLockOn -- VAs in funcmap.tsv, or identify them first) into
X2DamageEffect.cpp:2752 / :7341 / :6465 for every Phase-6b lock-on key, including the
NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE semantics. Cite June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when a play-test
shows JUNE_FX CE_LOCKON lines on DamageEffect_SA_RHB_Lockon_Stratos_Attack (or another
named lock-on effect from the matrix). Update the matrix and MODS.md.
```

---

## Phase 7c — Runtime: hit/die damage-effect creation, LOOP_*, die conditions

**Status: `LOOP_*` and `SET_DIE_WHEN_MASTER_EFFECT_DIE` play-tested 2026-10-03** (dev-console pass
T1: the console `DAMAGE_EFFECT_Trap_Fire_Big` loop, then the real trap NPC's drain, both passed,
exe `cccaf8fb…`). The die conditions still wait for Phase 11, because no deployed templet uses them.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **June source:** June's `CreateHitDamageEffect` / `CreateDieDamageEffect`, found from the
  sub-effect data consumers, plus the die-condition evaluation site: whatever reads the
  structure `31f66700` filled. **`LOOP_*` is not on the die path.** Phase 0 found it in
  `CREATE_DAMAGE_EFFECT%d` (`31f6ac60`), so its runtime is June's counterpart of March's
  `CREATE_DAMAGE_EFFECT` chain: `LoadDamageEffect` (`:6822`) → `m_vecCreateDamageEffect`
  → `SetDamageEffect` (`:5139`) → `m_DamageEffectDataList`, spawned at run time. Find the
  consumer of what `31f6ac60` fills.
- **March target:** `X2DamageEffect.cpp:5378`, `:5626`, `SetDieDamageEffect` (`:4931`), and
  `SetDamageEffect` (`:5139`) with the per-frame consumer of `m_DamageEffectDataList`.

**Scope.**
- `LOOP_INTERVAL/END_COUNT/END_TIME/ADD_POS_X/Y/Z`: how June schedules repeated spawns and
  steps their position (per spawn? cumulative? in facing space?). **Read it; don't
  assume.**
- `SET_DIE_WHEN_MASTER_EFFECT_DIE`, `INHERIT_*`, `SET_GROUP_ID`, `AUTO_DIE`,
  `APPLY_ROTATED_OFFSET`, `APPLY_ROTATE_ORIGIN`.
- The die conditions.

**Named test case.** `DAMAGE_EFFECT_Trap_Fire_Big`. The log must show N spawns at the
stepped positions, and the N and the step must match the Lua values through June's
formula.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 7c"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phases 6a and 7a are done (extend
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME). Branch: mods/lanox-port.

Your task: find June's CreateHitDamageEffect / CreateDieDamageEffect and the
die-condition evaluation site (the consumer of what FUN_31f66700 parses), then transcribe
the LOOP_* spawn scheduling and position stepping, SET_DIE_WHEN_MASTER_EFFECT_DIE,
INHERIT_*, SET_GROUP_ID, AUTO_DIE, the rotated-offset/origin keys, and the die conditions
into X2DamageEffect.cpp:5378 / :5626 / :4931. Read the loop math exactly -- per-spawn vs
cumulative, local vs world space -- and cite June address ranges.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when
DAMAGE_EFFECT_Trap_Fire_Big logs JUNE_FX CE_LOOP spawns whose count and positions match
the Lua values through June's formula. Update the matrix and MODS.md.
```

---

## Phase 7d — Runtime: DamageReact, AttackResult

**Status: done 2026-09-28, built and deployed; play-tested 2026-09-29.** The hit-scale branch fired
(`JUNE_FX CE_HIT … HIT_DAMAGE_EFFECT_SCALE`, Elesis SPK dash-jump XZ: entry 1.5 × parent 2 = 3×).
The die-scale branch was not reached. No errors, clean shutdown. `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`
is extended in `X2DamageEffect.cpp` and `X2DamageManager.cpp`. New `JUNE_FX CE_HIT` lines sit behind
`SERV_IRUHADEV_JUNE_FX_DEBUG`. The matrix has 16 `7d` runtime rows. Phase 7c's 19 notes, which had
been hand-written into the generated matrix, now live in `census.py` too, so a rerun keeps them. The
functions are in `funcmap.tsv` (4 rows updated, 9 added); full detail is in `MODS.md`.

**None of the new branches can fire on the deployed data.** A constant scan of the packed Sep 27
`DamageEffect.lua` finds no `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`, `INHERIT_CRITICAL_*`,
`INHERIT_DAMAGE` or `INHERIT_LIFE_TIME`. Only two 7d changes reach live data:
- the hit/die `SCALE_*` change: **8 hit and 22 die entries** have a non-1 scale. The first count
  said 13 because of a bad filter; the list is in `MODS.md`. Most are above 1, some are monster
  attacks. March drew them at the parent's scale, so the change is a multiply on top of that;
- the `ONLY_RELATIONSHIP` guard: 1 use in the data.

Things the next phases should know:

- **Identification.**

  | June VA | Function | Evidence |
  |---|---|---|
  | `31f85910` | `AttackResult` | decompiled; the `reActResult` switch and tail match March |
  | `31f6f600` | `DamageReact` | its one caller is the UnitToEffect body `326413c0` (via `32622280` → `32649c80`); the body is March's line for line and reads no June key |
  | `31f831e0` / `31f810c0` | `SetHitDamageEffect` / `CreateHitDamageEffect` | reached from AttackResult |
  | `31f83400` / `31f82210` | die wrapper / `CreateDieDamageEffect` | reached from `DestroyInstanceHandle` |
  | `31f81ba0` | touch-land spawner | reached from `31f83380` |
  | `31f8af50` | INHERIT_CRITICAL_* helper | its three callers are the spawners above |
  | `31f8fdb0` | parent → child state copy | its callers are listed in `funcmap.tsv` |
  | `31f6fdc0` | June `SetScale` | writes main, passive, hit and die scales |

  `funcmap.tsv` also records June's `DamageEffectData` field layout.
- **Transcribed:**
  - `RESET_…`: AttackResult `@31f881b6-31f881fe`. The gate is `>= 0`, so 0 restarts the clock.
  - `INHERIT_CRITICAL_*`: `+=` onto hit and die children only. June also does it for
    touch-land children, whose runtime is Phase 7e's.
  - `INHERIT_LIFE_TIME` in the hit and die scopes.
  - `INHERIT_DAMAGE`:
    - Its reader is **not** DamageReact/AttackResult, as 7c said.
    - `31f8fdb0 @31f8feb1-31f8fecc` copies the parent's `damage` at spawn.
    - `31f799c0 @31f79b54-31f79b75` carries it across `DAMAGE_DATA_NEXT`.
    - Only the CREATE_DAMAGE_EFFECT loop sets the child's flag.
  - `ONLY_RELATIONSHIP`: June skips non-user defenders. March `static_cast`s every defender.
  - Hit/die `SCALE_X/Y/Z`: March parsed them and never applied them. June sets the child's
    scale to `entry * parent hit|die scale`.
- **Classified, no code:**
  - `REACT_ANIMSPEED`: `same`. March's `SKILL_CASH_10_TEST` block, `@31f86ad5`.
  - `USE_ADVANCE_COLLISIOIN_CHECK`: `deferred` to Phase 9 (handoff in Phase 9's section).
  - AttackResult also forwards `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` (`+0xb32`) into the
    effect sets it creates. That key is already `unsupported`, so this is noted only.
- **AttackResult's other June differences read DamageData keys.** They are handed to Phase 8b
  (table in its section).
- **The three June-only sub-tables' runtime is Phase 7e's** (added 2026-09-29):
  `TOUCH_LAND_DAMAGE_EFFECT%d`, `HIT_EFFECT_SET%d`, `DIE_CREATE_NPC%d`. Phase 6b parsed them, and
  7c and 7d left them unported. Phase 7e's section has the identified June functions
  (`31f83380`/`31f81ba0`, `31f70130`, `31f70330`) and what each does.
- **Left without a runtime row, by design:**
  - `DE.top`'s sub-table names: their keys carry their own rows.
  - The `_ULTIMATE` variants: `m_bUseUltimateDamageData` is still forced false.
  - `APPLIED_SKILL_ID`: `unsupported`.
- **Play-test (2026-09-29).**
  - **Hit scale: seen.** Elesis SPK dash-jump XZ logged 7 lines; its air explosion now draws at 3×
    (entry 1.5 × parent 2) where March drew it at 2×.
  - **Still owed: the die-scale branch.** No `DIE_DAMAGE_EFFECT_SCALE` line appeared. (The
    dev-console pass's T3, 2026-10-03: the first run crashed in the console's `CreateInstance`
    call through a stale `jDE`.) **Reached 2026-10-03 on the second run (T3 passed):** two
    `DIE_DAMAGE_EFFECT_SCALE … entry=(0.660,0.660,0.660) parent=(1.000,1.000,1.000)` lines from
    `DamageEffect_SBH_Fire_Works`. It can be
    reached through:
    - Ara Dragon Falling finisher (`EffectSet_ASD_DRAGON_FALLING_FINISH`, `Ara_Martial_Artist.lua`):
      explosion at (2, 1, 2);
    - Ara Wolf Break (`DamageEffect_SA_ALD_WOLF_Break3(_Special)`);
    - Elesis fireworks (`EffectSet_A_SBH_FIREWORKS_FireBall`, `ELESIS_KNIGHT.lua`): shrapnel at 0.66;
    - the monster explosions in `MODS.md` (`NUI_SMITH_*`, `VKC`).
  - The one `ONLY_RELATIONSHIP` user needs a relationship partner, which offline play probably
    lacks, so its guard is unlikely to be exercised.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **June source:** June's `DamageReact` / `AttackResult`, found from the collision
  consumers or from CEffect's own call graph.
- **March target:** `X2DamageEffect.cpp:3400`, `:3485`.

**Scope.**
- `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`, `REACT_ANIMSPEED`, `ONLY_RELATIONSHIP`
- `INHERIT_CRITICAL_*`, `USE_ADVANCE_COLLISIOIN_CHECK`
- any changed semantics of the existing hit-response code

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 7d"), june2017_re/README.md,
funcmap.tsv, and the matrix. Phases 7a-7c are done (extend
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME). Branch: mods/lanox-port.

Your task: identify June's CEffect::DamageReact and AttackResult, then transcribe the
hit-response keys (RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT, REACT_ANIMSPEED,
ONLY_RELATIONSHIP, INHERIT_CRITICAL_*, USE_ADVANCE_COLLISIOIN_CHECK -- the typo is the
studio's) and changed semantics into X2DamageEffect.cpp:3400 / :3485. Cite June address
ranges. If USE_ADVANCE_COLLISIOIN_CHECK reaches into the DamageManager collision path,
record what it needs and hand it to Phase 9 rather than widening this phase.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when every CEffect
key in the matrix is implemented or classified. Update the matrix and MODS.md.
```

---

## Phase 7e — Runtime: TOUCH_LAND_DAMAGE_EFFECT, HIT_EFFECT_SET, DIE_CREATE_NPC

**Status: `HIT_EFFECT_SET` and `DIE_CREATE_NPC` done 2026-09-29, built and deployed.
`TOUCH_LAND_DAMAGE_EFFECT` deferred.** `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME` extended with
`JuneSetHitEffectSet()` and `JuneDieCreateNpc()` (`X2Lib/X2DamageEffect.cpp` only -- `KTDXLIB` is
untouched). Full detail, citations and the per-sub-table March target table are in `MODS.md`;
`funcmap.tsv` and `census.py`/the matrix are updated (the `HIT_EFFECT_SET`/`DIE_CREATE_NPC` rows
moved `deferred` → `done`; `TOUCH_LAND_DAMAGE_EFFECT`'s stay `deferred`, with the corrected
evidence attached rather than left as Phase 7d's placeholders). `census.py` all-PASS.

**`TOUCH_LAND_DAMAGE_EFFECT` was implemented, then reverted, on review.** A first pass wrote
`JuneCreateTouchLandDamageEffect()` and a `CXMeshInstance::JuneIsTouchLandVelocityGate()` that read
two still-unidentified June mesh offsets (`+0x478`, `+0x54c`) directly on March's `CXMeshInstance`
struct -- a violation of this plan's own Rule 2 ("never port an offset"), since those are June's
layout offsets and March's struct does not share them. The spawner (`31f81ba0`) also turned out to
be missing several blocks the "same shape as the hit spawner" finding had missed: an invoke-rate
skip, a damage-rate multiply, the `#ifdef INHERIT_FORCEDOWNRATE` arm rather than the `#else` one
copied, `SetScaleByUnit`, a flag copy, and `SetMaxLifeTime`. Rather than ship a gate on the wrong
fields or an incomplete spawner, the call site and both new methods were removed; `KTDXLIB` is back
to its pristine, unmodified state. Everything that *was* settled with real evidence is kept in
`funcmap.tsv` (the `31f83380` and `31f81ba0` rows) so the next pass starts from there rather than
from Phase 7d's original placeholders:

- **The gate's polarity was wrong, and this correction stands even though the port doesn't.** The
  three velocity comparisons were hand-traced (Phase 7c/7d) as "all three `< 0`" and explicitly
  flagged "not yet emulated". A ground-truth x87 emulator (`june2017_re/x87test.py`, JITs the
  actual compare+branch idiom and runs it on concrete floats via ctypes, rather than hand-deriving
  C0/C2/C3 flag tables) shows the third comparison uses a different mask/branch idiom and is the
  **opposite polarity**: `mesh(+0x47c) < 0 && mesh(+0x550) < 0 && m_vVelocity.y(+0x538) > 0`, all
  strict, false on NaN. `june2017_re/disasm_range.py`'s own symbolic tracer had a matching bug
  (only the *popping* x87 compare forms were special-cased, so the non-popping form used for two
  of the three checks showed no comparison at all) and was fixed in the same pass.
- **The gate's placement and position source are also settled**: `OnFrameMove @31f8a2a4-31f8a394`,
  right after March's own die-check and right before `DAMAGE_DATA_CHANGE_TIME`; position seeds from
  `m_BeforePos` (confirmed by a writer scan of `+0x10c`).
- **What still blocks the port**: `+0x478` and `+0x54c` (a plain per-frame copy of `m_vVelocity`,
  looking like a June-only `CXMeshInstance` member March lacks entirely, not a rename of an
  existing one) need a writer scan of mesh `OnFrameMove` (`31c87b80`) resolved to a named member,
  or an honest new March member added for it -- and the spawner's missing blocks (above) need
  transcribing from `dis_31f81ba0.txt`, which is on disk and fully read.

**`FUN_31f8b010`'s argument list, cited by Phase 7d as "not mapped ... yet", turned out to be
mis-attributed by Ghidra's own decompile:** it split one call's pushes across two different
callees. Settled by disassembling the raw pushes and tracking ESP through the interleaved
`PUSH`/`SUB ESP` sequence by hand, then confirmed a second, independent way: three of the eighteen
forwarded values match March enum constants exactly (`CX2Room::TN_MONSTER == 2`, `TN_NONE == -1`,
`CX2GUNPC::NCT_NONE == 0`), and a fourth way, an existing March `CreateNPCReq` call in the identical
shape (`X2GUChung.cpp:17128`). An E8-relative-call scan across all of June's `.text` also confirmed
the die-path pairing (`31f70330`→`31f83400`) at exactly 6 sites each, matching 1:1 -- stronger
evidence than the disassembly-by-hand this row started from.

**The new branches themselves can't be play-tested**: the deployed `DamageEffect.lua` uses none of
the three sub-tables (June's own data has four users, all Rose/RMH, listed below), so no
`JUNE_FX HIT_EFFECT_SET` or `JUNE_FX DIE_CREATE_NPC` line can fire on this build's data. The exit
criterion for the two done sub-tables was therefore a clean build matching the cited branches, plus
a **regression play-test** of the surrounding functions both new methods now sit inside:
`JuneSetHitEffectSet()` runs every time `AttackResult` does (i.e. on essentially every hit landed),
and `JuneDieCreateNpc()` runs at the top of `SetDieDamageEffect()` (i.e. whenever any damage effect
dies, which is constant during any fight) -- both loops just find an empty list and return on this
data, but the surrounding functions are exercised on every hit and every effect death regardless.

**Play-tested 2026-09-29.** Normal combat and NPC kills exercised both call sites repeatedly.
`offline_packets.log` shows no `UNHANDLED`/`EXCEPTION` lines; `offline_server.log` shows ordinary
play (quest/title load, field NPCs placed, several NPC kills, existing `JUNE_FX ES_CREATE`/`CE_FRAME`
lines from earlier phases still firing normally) ending in `SHUTDOWN clean: wal checkpoint ok,
els_db.sql.bak written`.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high, then an Opus review. Identification is done, and the roles are
recorded in `funcmap.tsv`. Switch to Opus only if one of the open questions below needs a new
function identified.

**Nothing can be play-tested on this build's data.**
- The deployed `DamageEffect.lua` (packed Sep 27) has no use of any of the three sub-tables.
- June's own data has four users, all Rose (RMH):
  - `DamageEffect_A_RMH_KS_83_Grenade`: `TOUCH_LAND_DAMAGE_EFFECT` → `…_Grenade_Landing`.
  - `DamageEffect_A_RMH_All_Bomb_Ultimate_AddMeca` and `DamageEffect_SA_RMH_Meca_Drop_Drop`:
    `DIE_CREATE_NPC`, `NUI_P_RMH_MECHANIC_MASTERY`.
  - `DamageEffect_A_RMH_Meca_Drop_RX_78_Summon_Land_Runner`: `DIE_CREATE_NPC`,
    `NUI_SI_SA_RG_RX78_LAND_RUNNER`, `EVENT_TIME = 0`.
- `HIT_EFFECT_SET` is binary-only: no June data uses it.
- So the exit is a clean build plus code that matches the cited branches. The log check waits
  for Phase 11 (June data) or Rose.

**June source, identified in Phase 7d.** Scans for the three templet lists' offsets found one
runtime reader each.

| Sub-table | Templet list (CEffect) | Loader (6b) | Runtime | Called from |
|---|---|---|---|---|
| `TOUCH_LAND_DAMAGE_EFFECT%d` | `+0xa8` (`DamageEffectData*`, stride `0xb8`) | `31f76a90` (`add ebx,0xa8` @`31f76b36`) | check `31f83380`, then spawner `31f81ba0` | OnFrameMove `31f88790` @`31f8a38b` only |
| `HIT_EFFECT_SET%d` | `+0xb8` (`HitEffectSetData*`) | `31f64c80` (`add esi,0xb8` @`31f64d10`) | `31f70130` | AttackResult `31f85910` @`31f87b1a` only |
| `DIE_CREATE_NPC%d` | `+0xc8` (value entries, stride 8) | `31f66230` (`add esi,0xc8` @`31f662a7`) | `31f70330` | every die path: OnFrameMove @`31f88847`/`31f8aa28`/`31f8aa7b`, `DestroyInstanceHandle` `31f83830` @`31f838b6`, `31f83960` @`31f839e3`, and the lock-on prelude `31f83aa0` @`31f83c59` (when `31f75e20` returns true, before the die wrapper `31f83400` and the handle destroy) |

The only other readers of these offsets in `31f60000`–`31f90000` are the ctor and `~CEffect`
(`31f78550`, which frees `+0xb8` at `31f793cb`–`31f7945a`). Hits outside that range are other structs.

**What each runtime does** (decompiled: `june2017_re/ghidra/dec_0x31f83380.c`, `dec_0x31f70130.c`,
`dec_0x31f70330.c`):

- **Touch-land: `31f83380`, with its gate in OnFrameMove `@31f8a300-31f8a38b`.**
  - **The gate.**
    - It sits after a `GetLandPosition( m_BeforePos )` probe.
    - It needs all three main-mesh y velocity components below the comparison value: `+0x47c`,
      then `+0x550` (triple `+0x54c`) `@31f8a31f`, then `+0x538` (triple `+0x534`) `@31f8a354`.
    - Read that comparison value exactly. It is the x87 value left on the stack before
      `31f8a318`. **Emulate it** rather than hand-trace it; see Phase 7a's emulator note.
  - The probe is recorded in `funcmap.tsv`'s `31f83380` row.
  - **The body.**
    - For each templet entry, `31f8e750( entry )` copies it into the per-call scratch list
      `+0xa38`.
    - `31f81ba0( +0xa38 )` then spawns the children, and the scratch list is freed.
  - **The spawner `31f81ba0`** has the same post-create shape as the hit spawner `31f810c0`
    (see Phase 7d and the `funcmap.tsv` row). After `CreateInstance` (`31f8ff00`) it calls:
    - `31f8af50`, the INHERIT_CRITICAL_* helper, `@31f81ffd`;
    - `31f8fdb0`, the child-state copy, `@31f82005`.
  - **Open question:** whether it can fire more than once per effect. `31f83380` has no once-flag
    of its own. Check whether the effect dies on landing, or whether the gate re-arms.
  - **March target.** A new `CEffect::JuneTouchLandDamageEffect()` called from `OnFrameMove` at
    the same point: after the `DamageCheck` calls (`@31f8a1b3`/`@31f8a1fc`), right before the
    `DAMAGE_DATA_CHANGE_TIME` block (`@31f8a394`). Build it on
    `CreateHitDamageEffect`'s shape over `m_DamageEffectDataInLua.m_vecTouchLandDamageEffect`, and
    reuse Phase 7d's INHERIT_CRITICAL_* / INHERIT_LIFE_TIME / scale blocks. Read `31f81ba0`'s
    position switch and scale lines instead of assuming they match the hit spawner's.
- **HIT_EFFECT_SET: `31f70130( a, b )`.**
  - AttackResult calls it right after `SetHitParticle` (`31f76970`, candidate) and
    `SetHitDamageEffect` (`31f831e0`). That puts it inside March's `bCreateHitEffect == true`
    branch (`X2DamageEffect.cpp`, after `SetHitDamageEffect()`).
  - The two pushed args are `EDI` / `EBP` at `@31f87b16-31f87b17`. They are probably the owner
    and the defender unit. **Confirm which is which** from AttackResult's register use; both
    must be non-null.
  - For each entry it calls `PlayEffectSet( EFFECT_SET_NAME (+0x4), a, b, … )` through the typed
    `g_pX2Game` fetch → `GetEffectSet` (`322ceae0`) → `321e1e60`. It passes power rate
    `+0x1d8` and scale `(1,1,1)`.
  - **`EVENT_TIME` (entry `+0x0`) is not read**; only `+0x4` is. Record it as parsed-not-used
    rather than invent a delay.
  - March target: a new `CEffect::JuneSetHitEffectSet( pOwner, pDefender )` over
    `m_vecHitEffectSet`, using the `PlayEffectSet` overload March already calls in
    `RT_CREATE_EFFECT_SET`.
- **DIE_CREATE_NPC: `31f70330`.**
  - It runs only with `g_pX2Game` and a live owner (`+0x1dc`).
  - For each entry it calls `31f8b010( NPC_ID (+0x4), …, pos = CEffect +0x1f4/+0x1f8/+0x1fc, … )`.
    The other arguments come from the owner:
    - the XOR-protected value at owner `+0x278..+0x280` (probably the level: identify it);
    - `GetUnitUID` (slot `0x54`);
    - slot `0x170`, and slot `0x198` (a bool; AttackResult uses it as "is local unit").
  - `31f8b010` has 18 callers across the NPC/AI code (`3298xxxx`–`32bfxxxx`), so it is a general
    create-NPC request. Its March counterpart is `CX2Game::CreateNPCReq` (`X2Game.cpp:6610`/
    `:6628`). Map its argument list onto one of those two overloads; that is this phase's one
    identification step.
  - **`EVENT_TIME` (entry `+0x0`) is not read here either.**
  - **Not a wire change.** `CreateNPCReq` sends `EGS_NPC_UNIT_CREATE_REQ`, which the offline
    emulator already answers (`X2OfflineServer.cpp:642`). Also check that only the local owner
    sends it, or every peer will create a copy.
  - `funcmap.tsv` used to call `31f70330` "SetDieDamageEffect?". That was wrong. June's die
    damage effect is `31f83400`, the call right after it on every die path.
  - March target: a new `CEffect::JuneDieCreateNpc()`, called just before the existing
    `SetDieDamageEffect()` on each March die path. These are OnFrameMove's die gates and
    `CX2DamageEffect::DestroyInstanceHandle`; match June's six call sites.

**Scope.** Every key in the matrix's `DE.TOUCH_LAND_DAMAGE_EFFECT`, `DE.HIT_EFFECT_SET` and
`DE.DIE_CREATE_NPC` sections, plus `DE.top`'s `TOUCH_LAND_DAMAGE_EFFECT%d` / `DIE_CREATE_NPC%d`
rows. The `census.py` `RUNTIME` rows for them say `7e` / `deferred`; move each to `done` as it is
transcribed. The flag is `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME` (extend it). Log with
`JuneFxHit` (`JUNE_FX CE_HIT`), throttled.

**Exit criteria.**
- Each key's `RUNTIME` row is `done`, or `-` with a reason (e.g. `EVENT_TIME` not read).
- The build is clean.
- `JUNE_FX CE_HIT TOUCH_LAND` / `HIT_EFFECT_SET` / `DIE_CREATE_NPC` lines are coded. Seeing them
  fire waits for Phase 11 or Rose; say so in MODS.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 7d", "Phase 7e"),
june2017_re/README.md, funcmap.tsv, and the matrix. Phases 7a-7d are done (extend
SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME). Branch: mods/lanox-port.

Your task: transcribe the runtime of the three June-only sub-tables, which Phase 7d already
identified: TOUCH_LAND_DAMAGE_EFFECT (gate in OnFrameMove @31f8a300-31f8a38b, check 31f83380,
spawner 31f81ba0), HIT_EFFECT_SET (31f70130, from AttackResult @31f87b1a) and DIE_CREATE_NPC
(31f70330, on every die path; map its callee 31f8b010 onto CX2Game::CreateNPCReq first).
Emulate the touch-land gate's x87 compare rather than hand-trace it. Cite June address
ranges; move the 7e RUNTIME rows in census.py to done and rerun it.

CP949 byte-level edits only. Build, deploy, ASK before launching. No deployed data uses
these sub-tables: done when every 7e row is done or classified and the build is clean.
Update the matrix and MODS.md.
```

---

## Phase 8a — SetDamageData I: existing keys, June semantics

**Status: done 2026-09-29, built and deployed.** New flag `SERV_IRUHADEV_JUNE_DAMAGEDATA`
(`X2Lib/X2DamageManager.{h,cpp}`). Evidence is in `june2017_re/README.md`, "Hand-verified (Phase
8a)", and the per-key verdicts are in `census.py`'s `HAND_SEMANTICS`. The tool is
`june2017_re/sdd_walk.py`. `MODS.md` has the before/after table.

- **No default changed.** Every March-parsed key has June's getter type and failure default:
  - register-sourced defaults resolved by their last write;
  - enum defaults checked against March's C++ values with a compile probe.

  Surrounding conditions match too: `ATTACK_ALL_TEAM`, the attack-box and custom-particle loops, the
  buff-factor call order, `ParsingBuffFactorID`'s level rule, and the tail
  `SetDamageExceptionProcess` call.
- **One semantics difference, fixed: `EXTRA_DAMAGE_TYPE`'s post-read remap** (`@32636204-32636214`).
  - `EDT_MANA_DAMAGE_WITH_NO_DISPLAY` (67) becomes `EDT_MANA_DAMAGE` (16), and a no-display flag is set
    (`ExtraDamageData::m_bManaDamageWithNoDisplay`).
  - It cannot fire on March's `Enum.lua`, which has no member 67. The exit test is a clean build.
  - **The flag's reader is 8b's.** Search from March's `EDT_MANA_DAMAGE` consumer (the case that shows
    the damage number) to its June counterpart, and look for the `+0x151` test there. A displacement
    scan is noise.
- **Not ported, recorded for later:**
  - the `DRAIN_HP_RATE` mirror at `+0x4e4`, which has no known reader;
  - June's `GetSkillLevel` fourth argument;
  - June's dropped memo path. March keeps it, because its data uses both memo keys 12 times each.
- **Not compared: `SetDamageExceptionProcess`** (attacker vtable `0x2b8`, `@3264050d`). It is where
  `fPowerRate` reaches the damage values, the biggest damage lever left. **Assigned to Phase 9**
  (user decision, 2026-09-29); see that section.

**Handoff to 8b/8c: every new read, in June order.** `32632390` comes first, then `ParsingBuffFactorID`
(`32630d20`, rows 56-61, called at `@32635d10`, row 18), then the no-key tail.
- Ranges run from a key's XOR guard to the next key's guard, so a range can include the next sub-table's
  `BeginTable`.
- Every open (`to-implement` / `unsupported` / `binary-only`) `DE.DAMAGE_DATA` row in `census.tsv`
  appears exactly once.
- The only entries with no census row are `RANGE%d`'s inner `RANGE` / `DAMAGE_RATE`, because no June
  data uses them.
- `(no key)` rows are behaviour without a Lua key; they belong to 8b.

| # | key | scope | June VA range | census | phase | note |
|---|---|---|---|---|---|---|
| 1 | `APPLIED_SKILL_ID` | (top) | `32632405-32632588` | unsupported | 8c | int getter; system: Applied skill ID (absent) |
| 2 | `APPLIED_MEMO_ID` | (top) | `32632588-326326e4` | binary-only | 8b | int getter |
| 3 | `DAMAGE_SKILL_ID` | (top) | `326326e4-32632860` | to-implement | 8b | int getter |
| 4 | `DIVISION_NUM` | (top) | `32632860-3263298e` | binary-only | 8b | int getter |
| 5 | `DIVISION_OFF_TIME` | (top) | `3263298e-32632b1a` | binary-only | 8b | float getter |
| 6 | `CREATE_DAMAGE_EFFECT_NAME_SCALE_X` | (top) | `32633698-32633967` | binary-only | 8b | float getter |
| 7 | `CREATE_DAMAGE_EFFECT_NAME_SCALE_Y` | (top) | `32633967-32633c20` | binary-only | 8b | float getter |
| 8 | `CREATE_DAMAGE_EFFECT_NAME_SCALE_Z` | (top) | `32633c20-32633ef5` | binary-only | 8b | float getter |
| 9 | `CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME` | (top) | `32633ef5-326341f4` | to-implement | 8b | bool getter |
| 10 | `CREATE_EFFECT_SET_MAIN_EFFECT_DELAYTIME` | (top) | `326341f4-32634510` | to-implement | 8b | bool getter |
| 11 | `TRACE_NEAREST_BONE` | (top) | `32634b5b-32634cf0` | to-implement | 8b | bool getter |
| 12 | `NATURAL_FORCE_TYPE` | (top) | `3263534f-326354f2` | unsupported | 8c | int getter; system: Natural force (gated-off) |
| 13 | `CAN_ACTIVATE_NASOD_ARMOR_COMBO` | (top) | `326354f2-32635773` | to-implement | 8b | bool getter |
| 14 | `DIVIDE_DAMAGE_FROM_RANGE` | (top) | `326358d4-32635b14` | binary-only | 8b | plaintext sub-table; per RANGE%d entry (RANGE default 99999.0, DAMAGE_RATE default 1.0) calls 3261a150 on DamageData +0x5b0 @32635ae5 |
| 15 | `RANGE%d` | DIVIDE_DAMAGE_FROM_RANGE | `326358ea-32635b0b` | binary-only | 8b | numbered from 0 via 31c7b980 |
| 16 | `RANGE` | DIVIDE_DAMAGE_FROM_RANGE/RANGE%d | `32635918-326359bd` | (no census row) | 8b | float getter |
| 17 | `DAMAGE_RATE` | DIVIDE_DAMAGE_FROM_RANGE/RANGE%d | `326359bd-32635b39` | (no census row) | 8b | float getter |
| 18 | `(call) ParsingBuffFactor / PushSocketAndTemporaryBuffFactor / ParsingBuffFactorID` | (top) | `32635cf6-32635d10` | - | - | same as March; ParsingBuffFactorID (32630d20) new tables listed at the end |
| 19 | `INIT_EXTRA_DAMAGE_BEFORE_LOAD` | (top) | `32635dea-32636073` | to-implement | 8b | bool getter |
| 20 | `ADD_MEMBER_COUNT` | EXTRA_DAMAGE | `326375a9-32637711` | to-implement | 8b | int getter |
| 21 | `ADD_MEMBER_COUNT_DISTANCE` | EXTRA_DAMAGE | `32637711-32637bc4` | to-implement | 8b | float getter |
| 22 | `BOUNDING_BACK_SPEED_X` | (top) | `326381bd-32638389` | to-implement | 8b | float getter |
| 23 | `BOUNDING_BACK_SPEED_Y` | (top) | `32638389-32638555` | to-implement | 8b | float getter |
| 24 | `FORCE_BACK_SPEED` | (top) | `32638555-326386c0` | to-implement | 8b | bool getter |
| 25 | `HIT_ADD_MP_PERCENT` | (top) | `32638e2e-32639047` | binary-only | 8b | float getter |
| 26 | `ADD_CHECK_DAMAGE_UNIT_NUM` | (top) | `3263ab94-3263adad` | to-implement | 8b | int getter |
| 27 | `EXCEPT_ME_HP_UP` | (top) | `3263bba7-3263bd01` | to-implement | 8b | bool getter |
| 28 | `ATTACK_ONLY_ME` | (top) | `3263bd01-3263be4b` | to-implement | 8b | bool getter |
| 29 | `IGNORE_DEFENCE_ON_PVP` | (top) | `3263c6df-3263c8ad` | to-implement | 8b | bool getter |
| 30 | `IGNORE_DEFENCE_RATE` | (top) | `3263c8ad-3263ca5b` | to-implement | 8b | float getter |
| 31 | `IGNORE_DEFENCE_RATE_ON_PVP` | (top) | `3263ca5b-3263cc83` | to-implement | 8b | float getter |
| 32 | `IGNORE_DEFENCE_DECLINE` | (top) | `3263cc83-3263ce70` | to-implement | 8b | bool getter |
| 33 | `(call) clear ADDITIONAL_ATTACK_TRIGGER_TYPE vector` | (top) | `3263d396-3263d39c` | - | - | 3221db80 on DamageData +0x544 |
| 34 | `ADDITIONAL_ATTACK_TRIGGER_TYPE` | (top) | `3263d3a1-3263d400` | to-implement | 8b | plaintext sub-table; ints from index 1 via 32fea4e0, push_back 31f8b860 into +0x544 |
| 35 | `DAMAGE_TRIGGER_DATA` | (top) | `3263d405-3263db6d` | to-implement | 8b | plaintext sub-table; 321f3650 clears +0x554 @3263d40f; entries by 32fe99b0 (i-th table from 1); each entry pushed by 3207b5d0 @3263db40 |
| 36 | `ATTACK_TRIGGER_TYPE` | DAMAGE_TRIGGER_DATA/[] | `3263d446-3263d603` | to-implement | 8b | int getter |
| 37 | `BUFF_TEMPLET_ID` | DAMAGE_TRIGGER_DATA/[] | `3263d603-3263d75e` | to-implement | 8b | int getter |
| 38 | `POWER_RATE` | DAMAGE_TRIGGER_DATA/[] | `3263d75e-3263d86e` | to-implement | 8b | float getter |
| 39 | `POWER_RATE_ACCUMULATION_MULTIPLIER` | DAMAGE_TRIGGER_DATA/[] | `3263d86e-3263db72` | to-implement | 8b | float getter |
| 40 | `HIT_ADD_COMBINATION_POINT` | (top) | `3263df79-3263e18f` | unsupported | 8c | int getter; system: Combination point (absent) |
| 41 | `DAMAGE_TRIGGER_CONDITION` | (top) | `3263e8d7-3263eaeb` | binary-only | 8b | int getter |
| 42 | `MAKE_TARGET` | (top) | `3263eaeb-3263ec1a` | to-implement | 8b | bool getter |
| 43 | `DAMAGE_DATA_KEY_NAME` | (top) | `3263ec1a-3263edf3` | to-implement | 8b | string getter |
| 44 | `DAMAGE_LIGHT_TIME` | (top) | `3263edf3-3263ef73` | to-implement | 8b | float getter |
| 45 | `IS_AURA_BUFF` | (top) | `3263ef73-3263f093` | to-implement | 8b | bool getter |
| 46 | `IGNORE_ATTACK_SUCCESS` | (top) | `3263f093-3263f256` | to-implement | 8b | bool getter |
| 47 | `HP_RATE_DAMAGE_TYPE` | (top) | `3263f256-3263f415` | unsupported | 8c | int getter; system: HP-rate damage (absent) |
| 48 | `HP_RATE_DAMAGE_VALUE` | (top) | `3263f415-3263f5ca` | unsupported | 8c | float getter; system: HP-rate damage (absent) |
| 49 | `ONYL_ATTACK_USER` | (top) | `3263f5ca-3263f733` | to-implement | 8b | bool getter |
| 50 | `DAMAGE_RATE_TO_SUPERAMOR` | (top) | `3263f733-3263f932` | binary-only | 8b | float getter |
| 51 | `PAUSE_NPC_AI_TIME` | (top) | `3263f932-3263fadb` | to-implement | 8b | float getter |
| 52 | `NUMERICAL_DATA_INDEX` | (top) | `3263fadb-3263fc8f` | unsupported | 8c | int getter; system: Numerical skill data (absent) |
| 53 | `NUMERICAL_DATA_NAME` | (top) | `3263fc8f-32640064` | binary-only | 8c | string getter; system: Numerical skill data (absent) |
| 54 | `ROSE_WEAPON_ATTACK_TYPE` | (top) | `32640064-3264026d` | unsupported | 8c | int getter; system: Rose weapon (absent) |
| 55 | `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT` | (top) | `3264026d-32640501` | to-implement | 8b | int getter |
| 56 | `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_SKILL_ID` | (top) | `32630e40-326311cd` | to-implement | 8b | ParsingBuffFactorID 32630d20; int getter @326311b4 (decrypt @32630efe), default 0; read only with a live attacker |
| 57 | `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_BUFF_TEMPLATE_ID` | (top) | `326311cd-326315a4` | binary-only | 8b | 32630d20; int getter @3263159f (decrypt @3263127f), default 0 |
| 58 | `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID` | (top) | `326315a4-326316ff` | to-implement | 8b | 32630d20; user attacker, skill learned (31fceab0 @32631602 > 0) and buff template 0 or present (3227baf0 @32631612); ids from index 1 (32fea6e0); marks "read" so the tables below are skipped |
| 59 | `BUFF_FACTOR_ID_ULTIMATE_PVP` | (top) | `326316ff-32631771` | binary-only | 8b | 32630d20; only when 31fcf3e0(APPLIED_SKILL_ID) @32630e05 is true (the absent skill-memo/ultimate callee, cf. Phase 5/6a) and in PvP (322cc1a0 @326316ff) |
| 60 | `BUFF_FACTOR_ID_ULTIMATE` | (top) | `32631771-326317d4` | to-implement | 8b | 32630d20; same gate, non-PvP or no _PVP table, and nothing read yet |
| 61 | `BUFF_FACTOR_ID_PVP` | (top) | `32631838-326318b6` | to-implement | 8b | 32630d20; PvP (322cc1a0 @32631838) and nothing read yet; falls back to BUFF_FACTOR_ID (@326318b8, March-read) |
| 62 | `(no key) NUMERICAL_DATA override` | (top) | `3263fe41-32640054` | - | - | when NUMERICAL_DATA_INDEX > 0 or NAME set, and APPLIED_SKILL_ID/DAMAGE_SKILL_ID set: overwrites PHYSIC/MAGIC (x0.01), PVP_RATE, TECH_POINT and HIT_ADD_MP from the numerical skill table |
| 63 | `(no key) DAMAGE_DATA_KEY_NAME registration` | (top) | `3263edd7-3263edee` | - | - | 32622220(name, pDamageData) on the damage manager when the name is non-empty |
| 64 | `(existing call) SetDamageExceptionProcess` | (top) | `3264050d-32640546` | - | - | attacker vtable 0x2b8 (pDamageData, &fPowerRate): where fPowerRate reaches the damage values. Same call shape as March; the callee was NOT compared. Assigned to Phase 9 |
| 65 | `(no key) +0x5a8 word copy` | (top) | `32640572-326405b1` | - | - | user attacker only: DamageData +0x5a8 = unit +0x9289 (the "(no key) +0x5a4/+0x5a8" row of Phase 8b's AttackResult table) |
| 66 | `(no key) 32622ad0 call` | (top) | `326405bf-32640608` | - | - | damage manager 32622ad0(attacker GetUnitUID (vtable 0x54), ...), unidentified |

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **June source:** `FUN_32632390` (11,918 disassembly lines). **Only** the keys March
  already parses.
- **March target:** `CX2DamageManager::SetDamageDataFromLUA` (`X2DamageManager.cpp:67`; the plan's
  "SetDamageData"), and `ParsingBuffFactorID` (`:3483`), whose June counterpart is `32630d20`.

**Method.**
- For each March-parsed `DAMAGE_DATA` key, find its June read: default, getter type,
  post-processing, and conditions around it such as a PvP branch.
- Record each key as `same` or `semantics-differ` in the matrix.
- Fix the differences, with citations.
- Hand off the list of *new* keys to 8b/8c in their exact June read order, with their
  June VA ranges.

This function has 9 callers across the binary, so it is shared (skills, NPC scripts). A
semantic change here moves every damage number in the game. Log every changed default in
`MODS.md` with the before and after values.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 8a"), june2017_re/README.md, and
the DamageData rows of the matrix. Branch: mods/lanox-port.

Your task: walk June's SetDamageData (FUN_32632390, ~12k disasm lines) and, ONLY for the
keys March already parses, compare default / type / post-processing / surrounding
conditions against CX2DamageManager::SetDamageDataFromLUA. Mark each same or semantics-differ in
the matrix, and fix the differences under a new SERV_IRUHADEV_JUNE_DAMAGEDATA flag with
June citations. This function is shared by every damage source in the game: list every
changed default in MODS.md with before/after values. Also produce the ordered list of NEW
keys with their June VA ranges, for Phases 8b/8c.

X2DamageManager.{h,cpp} are CP949: byte-level edits only. Build, deploy, ASK before
launching. Update the matrix and MODS.md.
```

---

## Phase 8b — SetDamageData II: new keys whose systems exist

**Status: in progress (2026-09-29).** Five keys done this session, extending
`SERV_IRUHADEV_JUNE_DAMAGEDATA` (no new flag). Built `US_SERVICE` clean (X2Lib + X2, 0 errors) and
deployed to `X2_offline.exe`, 18,949,120 B, SHA-256 `1c69e9949daa31be…`. `census.py` confirms all
five moved to `implemented` (phase label `8a`, since a `DE.DAMAGE_DATA` key with a March site always
reports there — see `census.py`'s `phase_of()`). 31 `to-implement` keys remain in `census.tsv`'s
phase `8b`, plus 13 `binary-only` (corrected 2026-09-29: this line said 28; the census had 36 after 8a,
not 33); resume with the grouping below.

- **`CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME`/`_DELAYTIME`** (`FUN_32632390
  @32633ef5-326341f4`/`@326341f4-32634510`, bool getters): consumed in `CEffect::AttackResult`'s
  `RT_CREATE_EFFECT_SET` case (`FUN_31f85910 @31f861a1-31f861e2`/`@31f86755-31f86788`). Both read
  this `CEffect`'s own main-effect mesh (`m_hMeshInstHandle`, the handle `GetMainEffect()` reads) and
  compute `MaxLifeTime − NowLifeTime`; LIFTTIME overrides the created set's `PlayEffectSet` `fLifeTime`
  argument (default is already March's own `-1.f`), DELAYTIME writes `EffectSetInstance::m_fDelayTime`
  directly. `X2DamageEffect.cpp`, `AttackResult()`.
- **`MAKE_TARGET`** (`FUN_32632390 @3263eaeb-3263ec1a`, bool): consumed in `AttackResult`'s default
  case (`FUN_31f85910 @31f86a9d-31f86acb`) — stores the defender's `UnitUID`/`GameUnitType` onto the
  *owner* (attacker), not onto `DamageData` or `CEffect`. New `CX2GameUnit` members
  `m_uidMakeTarget`/`m_eMakeTargetGameUnitType` (+ `SetMakeTarget`/getters), `X2GameUnit.h/.cpp`. No
  March reader exists for these yet — same "no known reader" situation as 8a's `DRAIN_HP_RATE`.
- **`TRACE_NEAREST_BONE`** (`FUN_32632390 @32634b5b-32634cf0`, bool): **parsed only.** The call site
  is identified — `FUN_31f85910 @31f8679f-31f867ad` calls `FUN_321da350(pInstance, &pMeshInst-position,
  pTraceUnit)` — but `FUN_321da350`'s own bone-lookup helpers (`FUN_32278580`, `FUN_31c1c1a0`,
  `FUN_31f54910`) are not identified, and this build already has a bone-trace-target subsystem
  (`SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME`'s `TargetMeshData`/`BONE_TRACE_TARGET_MESH_INDEX`, Phase 4b)
  that this key may or may not feed into. Implementing further here would be inventing behavior.
  **Next step:** identify those three functions and how (or whether) they interact with
  `TargetMeshData` — an identification step, Opus per the plan's own model table.
- **`DAMAGE_DATA_KEY_NAME`** (`FUN_32632390 @3263ec1a-3263edf3`, string): **parsed only.** The
  registration call is cited (`FUN_32622220 @3263edee(name, pDamageData)` on the damage manager, run
  when the name is non-empty) but its purpose (a name → `DamageData` lookup for something?) and any
  reader are not identified. **Next step:** find `FUN_32622220`'s own definition.
- **Carried over from 8a, not resolved this session: the `EDT_MANA_DAMAGE` no-display reader.** The
  natural March consumer is `CX2GUUser::Process_ExtraDamage`'s `EDT_MANA_DAMAGE` case — the two
  `DrawText( L"MP -…" )` calls at `X2GUUser.cpp:10409-10444` — but its June counterpart was not
  found: the binary exports no C++ names to search by, and a `+0x151` displacement scan is noise
  (README). **Next step:** identify `Process_ExtraDamage`'s June counterpart by call graph from a
  `confirmed` caller (Opus, high — this is identification, not transcription).

**Resume point — 31 `to-implement` + 13 `binary-only` rows left (`census.tsv`, phase `8b`), grouped by what each
needs next.** Groups 1-4 cover the 31; group 5 (added 2026-09-29) covers the `binary-only` rows they missed.
**Read `JUNE2017_CHARACTER_PORT_CHECKLIST.md` first:** June's character scripts use keys the census cannot
see, including two of group 5's.

1. **Damage-math** (the plan's own priority order). All of these need `CX2GameUnit::GetFinalDamage`'s
   June counterpart identified first — it is not in `funcmap.tsv`, and no `Process_ExtraDamage`-style
   string lead was found for it either in this session: `IGNORE_DEFENCE_ON_PVP`, `IGNORE_DEFENCE_RATE`,
   `IGNORE_DEFENCE_RATE_ON_PVP`, `IGNORE_DEFENCE_DECLINE`, `DIVISION_NUM`, `DIVISION_OFF_TIME`,
   `DIVIDE_DAMAGE_FROM_RANGE`/`RANGE%d`/`RANGE`/`DAMAGE_RATE` (`FUN_3261a150` on `DamageData +0x5b0`,
   8a's handoff row 14), `CAN_ACTIVATE_NASOD_ARMOR_COMBO`, `DAMAGE_SKILL_ID`.
2. **`ParsingBuffFactorID` family.** Needs `FUN_31fcf3e0`/`FUN_3227baf0`'s March counterparts
   identified — do not assume they are `IsEqippedMemo_`/the memo path by resemblance (the README
   already notes 1-3 of `ParsingBuffFactorID`'s order don't exist on March's data, but the *functions*
   still need real identification before a reader is written): `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID`,
   `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_SKILL_ID`, `BUFF_FACTOR_ID_ULTIMATE`,
   `BUFF_FACTOR_ID_PVP`.
3. **`DAMAGE_TRIGGER_DATA` family.** Parse (new sub-table + vector) is straightforward; the consumer
   needs finding wherever March iterates `m_eDamageTrigger`/`DAMAGE_TRIGGER_TYPE`:
   `DAMAGE_TRIGGER_DATA` and its `ATTACK_TRIGGER_TYPE`/`BUFF_TEMPLET_ID`/`POWER_RATE`/
   `POWER_RATE_ACCUMULATION_MULTIPLIER`, `ADDITIONAL_ATTACK_TRIGGER_TYPE` (cleared by `FUN_3221db80`,
   pushed by `FUN_31f8b860`, 8a's handoff rows 33-34).
4. **Everything else — likely the same shape as this session's five.** For each: try xrefs on the
   parsed member's write inside an already-`confirmed` function first (`AttackResult`, `DamageReact`,
   `EffectToUnit`); only escalate to a fresh June-function identification if that comes up empty.
   `INIT_EXTRA_DAMAGE_BEFORE_LOAD`, `ADD_MEMBER_COUNT`/`_DISTANCE`, `BOUNDING_BACK_SPEED_X/Y`,
   `FORCE_BACK_SPEED`, `ADD_CHECK_DAMAGE_UNIT_NUM`, `EXCEPT_ME_HP_UP`, `ATTACK_ONLY_ME`,
   `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`, `DAMAGE_LIGHT_TIME`, `IS_AURA_BUFF`, `IGNORE_ATTACK_SUCCESS`,
   `ONYL_ATTACK_USER`, `PAUSE_NPC_AI_TIME`.

5. **`binary-only` rows no group above names** (plan rule 3: implement anyway). **Two are used by June
   character scripts** (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 2), so 8b is not done without them:
   `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_BUFF_TEMPLATE_ID` (`Elsword_SwordMan`; do it with group 2)
   and `DAMAGE_RATE_TO_SUPERAMOR` (`Luciel_Duo`). No June script at all uses the other seven: `APPLIED_MEMO_ID`,
   `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z`, `DAMAGE_TRIGGER_CONDITION`, `HIT_ADD_MP_PERCENT`,
   `BUFF_FACTOR_ID_ULTIMATE_PVP`. (The other four `binary-only` rows, `DIVISION_NUM`, `DIVISION_OFF_TIME`,
   `DIVIDE_DAMAGE_FROM_RANGE` and `RANGE%d`, are already in group 1; no June script uses them either.)

**Not this phase's job.** The four `unassigned` `BUFF_FACTOR/*` rows (`BFT_AURA`, `BBT_INVINCIBLE`,
`BBT_IMMUNE_NON_CONTROL_DEBUFF`, `BUFF_FACTOR/BEHAVIOR/BBT_NEVER_MOVE/OFFSET_POSITION`) are the
matrix's own "nested BUFF_FACTOR" open question (Phase 1's scope-not-owned list), not 8b's — leave
them `unassigned`.

**Play-test checklist (pending as of 2026-09-29; passed 2026-10-03 as the dev-console pass's T4).** None of this session's five keys can produce an
*observable* gameplay difference yet — no currently-loaded March data sets any of them true (deferred
to Phase 11, same as Phase 2's `ES_TEMPLET` situation). So this round is a **regression check on a
shared, always-executed path**, not a feature verification:

- **What actually changed in a hot path:** `CEffect::AttackResult`'s `RT_CREATE_EFFECT_SET` case (any
  damage effect that creates an effect set on hit — impact VFX, on-hit procs) now goes through a
  restructured `PlayEffectSet` call and a new `pInstance->m_fDelayTime` check every time that case
  fires; the `default:` case (every "normal" hit) now also evaluates a new
  `if( m_bMakeTarget && pDefenderGameUnit )` block. Both are no-ops with the new bool flags defaulting
  `false`, but the code is newly reached on every such hit.
- **General combat regression:** normal attacks and a variety of skills, in a dungeon and/or PVP room.
  Nothing should look, feel, or behave differently than before this session.
- **Specifically exercise skills/effects that create an effect set on hit** (on-hit status VFX, proc
  effects) — these hit the changed code. Confirm they still spawn, are positioned correctly, and have
  an unchanged lifetime/delay.
- **No crashes or exceptions.** After playing, check `offline_packets.log` for
  `*** EXCEPTION - ROLLED BACK ***`, `offline_server.log` for anything new/unusual, and `ErrorLog.txt`
  for new entries.
- **Character state sane afterward:** HP/MP, buffs, damage numbers look normal; nothing hangs.

A clean pass confirms the refactor is a safe no-op; genuine verification of these 5 keys' behavior
still has to wait for Phase 11's June data.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

**Scope.** The 8a handoff list, filtered to systems this build has. Examples, to be
verified against the census:
- `DIVISION_NUM/OFFSET`, `DAMAGE_SKILL_ID`, `REACT_TYPE`
- `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z`, `CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME/DELAYTIME`
- `TRACE_NEAREST_BONE`, `TRACE_UNIT_DIE_DELETE_EFFECT_SET`, `PVP_RATE`
- `IGNORE_DEFENCE_ON_PVP/_RATE/_RATE_ON_PVP/_DECLINE`, `IGNORE_STATE_DEFENCE`
- `STOP_TIME_ATT/DEF`, `HIT_ADD_MP_PERCENT`, `BUFF_TEMPLET_ID`
- `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`, `PAUSE_NPC_AI_TIME`, `DAMAGE_TRIGGER_CONDITION`
- `DAMAGE_DATA_KEY_NAME`, `ONYL_ATTACK_USER`, `NO_BUFF`, `ONLY_BASIC`

**The authoritative list is Phase 8a's handoff table** (June order, VA ranges, census status). It also
carries 8a's one leftover consumer: port the reader of
`ExtraDamageData::m_bManaDamageWithNoDisplay` (June `DamageData +0x151`). Phase 8a already sets it.

**Parse and consume both, in this phase.** Each key's consumer is usually in
`X2DamageManager.cpp` (hit resolution) or in the unit's damage-taken path. Find June's
consumer with `xrefs` on the member's write site, and transcribe it.

**Consumers found in AttackResult (`31f85910`) by Phase 7d, not ported.** The DamageData offsets
were mapped to keys through `SetDamageDataFromLUA`'s getter destinations. CEffect `+0x200` is
`m_DamageData`.

| Key (DamageData offset) | Read in AttackResult | What June does |
|---|---|---|
| `CREATE_EFFECT_SET_TRACE_UNIT` (`+0x4c0`) | `@31f86173`, `@31f867bf` | case `0xc` (RT_CREATE_EFFECT_SET): trace-unit choice (March has it); also copied into the created set |
| `CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME` (`+0x4c1`) | `@31f861a1` | case `0xc`: the set's life = main effect max − now life (mesh `+0x44c` − `+0x448`) |
| `CREATE_EFFECT_SET_MAIN_EFFECT_DELAYTIME` (`+0x4c2`) | `@31f86755` | case `0xc` tail: set instance `+0x5c` = main effect max − now life |
| `TRACE_NEAREST_BONE` (`+0x4c3`) | `@31f8678b` | case `0xc` tail: calls `321da350` when a trace unit exists |
| `TRACE_UNIT_DIE_DELETE_EFFECT_SET` (`+0x4c4`) | `@31f867b2` | case `0xc` tail: copied into the set (March `TRACE_UNIT_DIE_DELETE_EFFECTSET`) |
| `MAKE_TARGET` (`+0x5c1`) | `@31f86a9d` | default case: stores the defender's UID and type into the owner (`+0x6898..+0x68a0`) |
| (no key) `+0x5a4` / `+0x5a8` | `@31f860fd`, `@31f867cb` | copied into RT_CREATE_DAMAGE_EFFECT children and created sets (`+0x7a4`/`+0x7a8` in CEffect); also copied by the hit/die spawners |

The MP/soul block's gates at `+0x6dc`/`+0x704`/`+0x708`/`+0x788` are March's existing `NO_DAMAGE`,
`HP_PERCENT_UP`, `MP_PERCENT_UP` and `INCREASE_MP`. They are not new. The block's own formula has
more June-only terms (`31f86937`–`31f877xx`) that were not compared field by field. Read them
when a key here needs it.

**Split rule.** If one conversation can't hold it all, do the damage-math keys first
(ignore-defence, PvP rate, division), commit, and write the resume point here.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 8b" -- including its "Status: in
progress" resume point, if one is there), june2017_re/README.md, funcmap.tsv, and the
matrix. Phase 8a is done (extend SERV_IRUHADEV_JUNE_DAMAGEDATA). Branch: mods/lanox-port.

If Phase 8b already has a resume point, start from its numbered groups in order rather than
re-deriving scope from census.tsv yourself. Otherwise: for each new DamageData key whose
system exists in this build, transcribe both June's parse (FUN_32632390 range from 8a's
list) and June's consumer (find it via xrefs on the June member write inside an
already-confirmed function first), citing both. Damage-math keys first. A key whose
consumer needs identifying a NOT-YET-confirmed June function is an identification step, not
transcription -- per the plan's own model-switch rule, flag it and stop rather than guessing
by resemblance. If the session runs long, commit and write the resume point into Phase 8b's
section, grouped by what each remaining key needs next (mirror the existing resume point's
shape).

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when census.py
shows no DamageData keys to-implement OR binary-only in phase 8b, except Phase 8b's
own explicitly-deferred (parsed-only) keys; see JUNE2017_CHARACTER_PORT_CHECKLIST.md section 2. Update the matrix and MODS.md.
```

---

## Phase 8c — SetDamageData III: absent systems, and the enum tables so June lua loads

**Status: done 2026-09-29, built and deployed (`US_SERVICE`, 0 errors, `X2_offline.exe` 18,952,704 B, SHA-256 `dc5c6c4fef65ae2d…`; ~~not play-tested yet~~ play-tested 2026-10-03, dev-console pass T5: nine lines, including two `APPLIED_SKILL_ID` lines that the repacked `Enum.lua` now makes reachable).** Extends
`SERV_IRUHADEV_JUNE_DAMAGEDATA` (no new flag): `X2Lib/X2DamageManager.{h,cpp}`,
`X2Lib/X2DamageEffect.cpp`. The exit test is a new `census.py` check: every phase-8c
`DE.DAMAGE_DATA` row has a March parse site. It passes, and all 8 rows are `unsupported`, with
their system named and their March site shown.

**Scope, as the census settled it.** Eight `DE.DAMAGE_DATA` keys across six systems.
- The plan's list also named `APPLIED_MEMO_ID` and `CAN_ACTIVATE_NASOD_ARMOR_COMBO`. Their systems
  exist, so they stay with 8b under this phase's own rule:
  - skill memo: `ADD_MEMO_1ST_CLASS` is ON;
  - Add's Nasod-armor combo: `X2GUAdd`, under `SERV_9TH_NEW_CHARACTER`, is ON.
- The `BUFF_FACTOR/APPLIED_SKILL_ID` row is `unassigned`. It is a data path artifact: June's only read
  is the top-level one. Left alone.

**Absent-system verdicts, re-checked.** Stem search over `X2Lib`, `KTDXLIB`, `X2ServerProtocol` and
`KncWX2Server/Common` (classes, shared enums, `EventID_*`/packet headers). `X2Lib/Offline/` and this
rebuild's own blocks are excluded.
- **Rose weapon: absent.**
  - `CX2Unit::UNIT_TYPE` ends at `UT_ADD`.
  - The only `UT_ROSE` is a Phase 5 comment.
  - Every other `ROSE` stem hit is a skill or NPC name (`SI_A_SBH_SCALET_ROSE`, `NUI_ROSEANG`, …).
- **HP-rate damage, numerical skill data, combination point: absent.** Zero identifiers anywhere,
  packets included.
- **Natural force: gated-off.**
  - March's own read is `LUA_GET_VALUE_ENUM( "NATURAL_FORCE_TYPE", m_NaturalForceType, …, NFT_NONE )`,
    with the member and the C++ enum all under `ADD_RENA_SYSTEM`, which is off under `US_SERVICE`.
  - The new read sits under `#ifndef ADD_RENA_SYSTEM`, so turning that flag on hands the key back to
    the studio read.
- **Applied skill ID: absent.**
  - June reads DamageData `+0x5c4` in two places: `ParsingBuffFactorID`'s `_ULTIMATE` gate
    (`31fcf3e0`, still unidentified, 8b group 2) and the numerical-data override. March has neither.
  - `DamageData::m_eUsedSkillID` (`SERV_ARA_CHANGE_CLASS_SECOND`) is a C++-set skill ID, not this key.
    It is a lead for 8b's `DAMAGE_SKILL_ID`, nothing more.
  - This verdict depends on `31fcf3e0`'s identification. If 8b finds it is March's memo path, revisit
    it.

**June's parse, key by key** (`sdd_walk.py`; defaults read at the store after `test al,al; jne`):

| key | June VA range | getter @ call | default | June destination | logged when |
|---|---|---|---|---|---|
| `APPLIED_SKILL_ID` | `32632405-32632588` | int @`32632561` | 0 (`EBX`) | `+0x5c4` | ≠ 0 |
| `NATURAL_FORCE_TYPE` | `3263534f-326354f2` | int @`326354d2` | −1: `EDI`, set @`3263532a` (`or edi,-1`) and not rewritten before the store @`326354db` (checked with capstone) | `+0x56c` | ≠ −1 |
| `HIT_ADD_COMBINATION_POINT` | `3263df79-3263e18f` | int @`3263e17b` | 0 | `+0x594` | ≠ 0 |
| `HP_RATE_DAMAGE_TYPE` | `3263f256-3263f415` | int @`3263f3f7`, enum shape `-(ok) & temp` | 0 (`HRDT_NONE`) | `+0x5f4` | ≠ 0 |
| `HP_RATE_DAMAGE_VALUE` | `3263f415-3263f5ca` | float @`3263f5b8` | 0.0 (`fldz`) | `+0x5f8` | ≠ 0 |
| `NUMERICAL_DATA_INDEX` | `3263fadb-3263fc8f` | int @`3263fc7d` | 0 | stack local | > 0 (June's own gate @`3263fe41`) |
| `NUMERICAL_DATA_NAME` | `3263fc8f-3263fe41` | string @`3263fe27` | `L""` (`0x3344f1f4`) | stack local | not empty (same gate) |
| `ROSE_WEAPON_ATTACK_TYPE` | `32640064-3264026d` | int @`32640246` | −1 (`RWAT_NONE`) | `+0x600` | ≠ −1 |

- **Not ported, by rule 5:** the numerical-data override (`@3263fe41-32640054`, 8a handoff row 62).
  The two `NUMERICAL_DATA_*` keys feed only that override, so June never stores them in DamageData.
  March does, so the log has something to print.

**Where the templet name comes from.** `SetDamageDataFromLUA` does not know its templet, and it has
19 call sites.
- The rule-5 line is emitted inside `SetDamageDataFromLUA`, so no caller parses these keys silently.
- The templet name comes from `CX2DamageManager::JuneDamageDataTempletScope`, an RAII local the CEffect
  ctor sets around all of its `DAMAGE_DATA*` reads. It restores the previous name, so nested templets
  unwind correctly.
- Unit, NPC and buff callers set no name. Their lines read `(no templet) <table>`.
- The dedup is a static set, once per (key, templet) for the session, not the 1-second throttle Phase
  5 used. This is the same unguarded-static pattern as `JuneFxCeOnce`.
- It is not behind `SERV_IRUHADEV_JUNE_FX_DEBUG`: a rule-5 line is not a diagnostic.
- `CX2OfflineLog` is included under `SERV_IRUHADEV_JUNE_DAMAGEDATA` + `SERV_IRUHADEV_OFFLINE`.

**What a missing enum table does in this build.** It is not `LUA_GET_VALUE_ENUM`'s doing.
- The macro (`LuaCommon/KLuaManager.h:152`, the header X2Lib actually compiles, via `KTDXLIB`) reads the
  already-built table through `GetValue( key, int& )`. On `Get` failing or `lua_isnumber` false it
  returns false, and the macro stores the default. It never touches the enum table.
- The enum table is dereferenced by the Lua chunk itself, while the templet's table constructor runs.
  Each DamageEffect templet is a global assignment (`NAME = { … }`), and each EffectSet templet is a
  `g_pEffectSet:AddEffectSetTemplet_LUA({ … })` statement.
- **A missing table** (`ROSE_WEAPON_ATTACK_TYPE.RWAT_…` with no global `ROSE_WEAPON_ATTACK_TYPE`) raises
  "attempt to index a nil value":
  - `lua_tinker::dobuffer` (`LuaCommon/lua_tinker.cpp:184`) catches it in `lua_pcall` and returns false.
  - `LoadAndDoMemory` logs `KEM_ERROR73` with the file name.
  - `CX2DamageEffect::OpenScriptFile` returns silently.
  - Every statement before the failing one has run, and **every templet from the failing one to the
    end of the file is never defined**. The later "templet not found" looks unrelated.
- **A missing member of an existing table** (`SKILL_ID.RNSI_…` absent from March's `SKILL_ID`) is plain
  nil. The key is absent from the built table, so `GetValue` fails and the default is used, **silently**.
  The matrix's *Enum census* lists every such member (1,006 missing from March).

**Handoff to Phase 11: the June-only tables** (from `june2017_re/june_lua/Enum.src.lua`; March's
`Enum.src.lua` + `DungeonEnum.src.lua` have none of them). Add them whole, not only the members 8c's
keys use. Any one missing table truncates a whole file, whichever phase owns the key that references it.

| table | June `Enum.src.lua` line | members (exact values) | uses in June `DamageEffect*`/`EffectSet*` |
|---|---|---|---|
| `NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE` | 7168 | `NTPLOTT_NONE = 0`, `NTPLOTT_TARGET_LIST_CLEAR = 1`, `NTPLOTT_TARGET_LIST_NOT_CLEAR = 2`, `NTPLOTT_TARGET_LIST_SAVE_ONE = 3`, `NTPLOTT_TARGET_LIST_SAVE_HIT_UNIT = 4` | 110 |
| `DAMAGE_EFFECT_DIE_CONDITION_TYPE` | 11746 | `DEDCT_NONE = -1`, `DEDCT_DAMAGE_TIME = 0`, `DEDCT_LOCK_ON_TARGET = 1` | 2 |
| `ROSE_WEAPON_ATTACK_TYPE` | 17643 | `RWAT_NONE = -1`, `RWAT_REVOLVER = 0`, `RWAT_AUTOGUN = 1`, `RWAT_MUSKET = 2`, `RWAT_HANDCANNON = 3`, `RWAT_REVOLVER_CHAIN = 4`, `RWAT_REVOLVER_BLADE = 5`, `RWAT_END = 6` | 38 |
| `HP_RATE_DAMAGE_TYPE` | 17855 | `HRDT_NONE = 0`, `HRDT_NOW_HP = 1`, `HRDT_MAX_HP = 2` | 8 |
| `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` | 18145 | `AEACBO_NONE = 0`, `AEACBO_NOT_APPLY = 1`, `AEACBO_MY_EFFECT = 2`, `AEACBO_MY_TEAM_EFFECT = 3`, `AEACBO_ANOTHER_TEAM_EFFECT = 4` | 27 |

`NATURAL_FORCE_TYPE` (June line 634) is already in March's `Enum.src.lua:510`, with identical values.
The deployed `kom_contents/data036_kom/Enum.src.lua` is byte-identical to `march2014/data036/`'s.

How much of each June file survives a load on today's Enum: the first templet that indexes a missing
table, and everything after it, is lost.
- The scan was widened to any `UPPER.member` whose table is not in March's `Enum`/`DungeonEnum`.
- It found only these five tables, plus false positives that are file names inside string literals
  (`"RWT_COMBO_X_IMPACT.ogg"`).
- So the table below is exact, not a lower bound.

| June file | templets | first failing templet | lost |
|---|---|---|---|
| `DamageEffect` | 6,206 | #322 `DamageEffect_AHM_Chain_Lightning` (`NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE`) | 5,885 |
| `DamageEffect_Character` | 1,027 | #89 `DamageEffect_SA_RCR_Punishment_Attack1_FX` (`ROSE_WEAPON_ATTACK_TYPE`) | 939 |
| `DamageEffect_Dungeon` | 807 | #9 `DamageEffect_PVP_HERO_ARUS_Bullet_Blitz` (`NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE`) | 799 |
| `DamageEffect_ETC` | 431 | #29 `DamageEffect_WaterBalloon_Hit_AttackDummy` (`HP_RATE_DAMAGE_TYPE`) | 403 |
| `DamageEffect_INT` | 358 | #197 `DamageEffect_Riding_Hellhound_Skill_C_GHOST1` (`NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE`) | 162 |
| `EffectSet` | ~8,475 top-level statements | statement #2,536, `EffectSet_HA_LDN_DIABOLIC_DISASTER` (`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`) | the rest |
| `EffectSet_INT`, `EffectSet_Description` | | none | 0 |

- **Two members whose values differ** are a Phase 11 hazard, because they are silent:
  `BUFF_FACTOR_ID.BFI_BUFF_HAMEL_SECRET_DUNGEON_ARMOR_EVENT`/`_WING_EVENT` are March 365/366 and June
  372/373 (matrix *Enum census*).
- **The deployed data today** references none of the five missing tables, so no templet is being lost
  now. It does carry six `NATURAL_FORCE_TYPE`, one `NUMERICAL_DATA_INDEX = 1` and one `APPLIED_SKILL_ID`
  inside `DAMAGE_DATA` (`kom_contents/data036_kom/DamageEffect.src.lua`).
  - `APPLIED_SKILL_ID` is `SKILL_ID.RNSI_SA_RHB_LOCKON_STRATOS`, which March's `SKILL_ID` lacks. It
    arrives as nil and is never logged.
  - The other two can log. Expected lines:
    - `JUNE_FX unsupported NATURAL_FORCE_TYPE in DamageEffect_DRYADKENTA_SpacialAttackB_Fire` (the Dryad
      Kenta boss);
    - five Rena templets: `NATURAL_FORCE_EXPLOSION_BOOM_COMBO`, `NATURAL_FORCE_EXPLOSION_BOOM_SKILL`,
      `LESI_COMBAT_RANGER_LESI_DOUBLE_JUMP_ATTACK_Z_UP_DAMAGEEFFECT`, `DOUBLE_JUMP_X_ARROW` and
      `DOUBLE_JUMP_XX_ARROW`;
    - `NUMERICAL_DATA_INDEX in DamageEffect_SA_RHB_Lockon_Stratos_Attack`.
  - Each needs that templet to be instantiated in play.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

**Scope.**
- `ROSE_WEAPON_ATTACK_TYPE`, `NATURAL_FORCE_TYPE`, `HP_RATE_DAMAGE_VALUE/TYPE`,
  `NUMERICAL_DATA_INDEX/NAME`, `APPLIED_MEMO_ID`, `APPLIED_SKILL_ID`,
  `CAN_ACTIVATE_NASOD_ARMOR_COMBO`, `HIT_ADD_COMBINATION_POINT`, and whatever else the
  census marked `unsupported`.
- Each gets parse and store, one `JUNE_FX unsupported` log line per key per templet, and
  **no behaviour**.
- Confirm the "system absent" verdict before logging it. Search for the system's classes,
  enums and packets. If the system turns out to exist, move the key to 8b's rules instead.

**Enum tables.** Whether a key is parsed as an enum or an int, the June data references
`ROSE_WEAPON_ATTACK_TYPE.*`, `HP_RATE_DAMAGE_TYPE.*` and so on. Without the table the Lua
chunk errors on nil indexing, or reads nil silently.
- Decide by reading how `LUA_GET_VALUE_ENUM` behaves on a nil table in this build.
- Hand the table additions to Phase 11 with exact values from June's `Enum.lua`. Do not
  edit `Enum.src.lua` here.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 8c"), june2017_re/README.md, and
the matrix. Phases 8a-8b are done (extend SERV_IRUHADEV_JUNE_DAMAGEDATA).
Branch: mods/lanox-port.

Your task: for every DamageData key the census marked unsupported(system), first confirm
the system really is absent from this build (classes / enums / packets). Then transcribe
June's parse and store it, with one JUNE_FX unsupported log line per key per templet and
no behaviour. Work out from this build's LUA_GET_VALUE_ENUM what a missing enum table
does, and hand exact table values from June's Enum.lua to Phase 11 -- don't edit
Enum.src.lua in this phase.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when census.py
reports 0 unclassified DamageData keys. Update the matrix and MODS.md.
```

---

## Phase 9 — Collision and hit path (answers RANOX_EFFECT_RUNTIME_UNKNOWNS §4)

**Status: collision half built and deployed 2026-09-29 (`SERV_IRUHADEV_JUNE_COLLISION`, commit
`e2e078f`), ~~play-test pending~~ **play-tested 2026-10-03** (dev-console pass T6: the Mag Charger
fireball `COLLIDE`s with the player and a level-70 charger's fireball does real damage. June and
March agree, and the hitbox follows the mesh. One open question about the trace's far samples is
recorded under T6). `SetDamageExceptionProcess`
half compared in part, no code.**
- **Found.** June's `CollisionDataCheck` is inlined into `EffectToUnit` `3262b6e0 @3262bc1a-3262eebd`,
  and the same code is in UnitToUnit's `32611970`. `CheckCollisionGuard` is `32640770`, March's
  live `#else` branch. The frame parser is `31d4f8b0`.
- **What differs from March:**
  - sphere×line and line×sphere clamp to the segment and move the impact point halfway to the
    sphere centre;
  - `USE_ADVANCE_COLLISIOIN_CHECK` moves only the sphere×sphere impact point. **The handover
    below guessed a swept test; that was wrong.**

  Sphere×sphere hit/miss, line×line, the guard rule, the radius formula and the zero-radius drop
  (x87-emulated) are March's.
- **Where the answers are.** Every §4 item (4.2-4.8) is answered with a citation in
  `RANOX_EFFECT_RUNTIME_UNKNOWNS.md`; 4.6's premise was wrong. Full detail, the visible-change list
  and the known gap (only `EffectToUnit` is switched over) are in `MODS.md`.
- **The named test.** `DamageEffect_MAG_CHARGER_Attack_A` (Mag Charger fireball, Ashen Land field
  `Ranox_Field_00`). Its mesh has a real attack sphere, radius ≈43 before `Size` 0.6. Since
  sphere×sphere is unchanged, the port alone may not make it hit. The `JUNE_FX HIT … TEST` trace
  prints its sphere centre next to its mesh position, to settle whether the hitbox follows the
  visual.
- **`SetDamageExceptionProcess`.** `vtslot.py` resolved slot `0x2b8` for every class. The base
  `322990e0` and `CX2GUUser` `32045340` are compared: the `fPowerRate` multiplies equal March's, and
  every June addition sits in an absent system (`AT_COMMAND`, applied skill ID) or is unidentified.
  The NPC override `32759f30` is the base plus the master's skill-memo hook. The eight other
  per-class overrides are `candidate` rows in `funcmap.tsv`, still to compare.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **June source:** June's `CX2DamageManager::CollisionDataCheck` and
  `CheckCollisionGuard`, plus the attack-sphere/line construction.
  - Find them via `SetDamageData`'s neighbours in the `0x3263xxxx` region, the
    `X2DamageManager.cpp` `__FILEW__` string if present, and the callers of June's
    `KTDGXSkinAnim` collision helpers (March `KTDGXSkinAnim.cpp:556-568`).
- **March target:** `X2DamageManager.cpp:1740` and the chain documented in
  `RANOX_EFFECT_RUNTIME_UNKNOWNS.md`: `KTDGXSkinAnim.cpp`, and
  `KTDXDeviceXSkinMesh.cpp:770-878`, which builds attack data from the
  `ATTACK_LINE_START/END` and `ATTACK_SPHERE` frames.

**Scope.** Answer each open item with a June citation, and implement the differences:
- §4.2: the `CollisionDataCheck`/`CheckCollisionGuard` bodies.
- §4.3: whether the radius scales with instance scale.
- §4.4: what happens to a zero-radius attack datum.
- §4.5: `FIX_EFFECTTOUNIT01`.
- §4.6: the direction of the effect-to-NPC test.
- §4.7: the units of `ATTACK_TIME_START/END`.
- §4.8: whether `USE_CANNONBALL` is user-only.
- `USE_ADVANCE_COLLISIOIN_CHECK`: **handed over by Phase 7d.** What it needs:
  - The ctor stores it at CEffect `+0xb31` (`31f7aab0@31f7c4d3`).
  - Its only reader is EffectToUnit `3262b6e0`. That function inlines CollisionDataCheck over the
    main effect's attack list × the defender's collision set.
  - At `@3262bc1c` it is loaded into the stack local `[esp+0x43]`.
  - The only real read of that local is `@3262e976`, inside the per-sphere geometry. When set, it
    takes a segment-style branch (`3262e981`–`3262ec5d`: two distance compares, else `31c7b640`).
    It is presumably a swept test between an attack sphere's previous and current centres. Verify
    that; it is not settled.
  - **Trap:** the same slot is reused as the miss flag after DamageReact (`@3262f1ad` / `@3262f1c0`,
    read `@3262f212`). Only the read before `3262f000` is the key's.
  - March's `CKTDXCollision::CollisionDataCheck` has no such variant, so this needs a new collision
    entry point.
  - No deployed `DamageEffect.lua` uses the key (Sep 27 packed file).

**Named test case.** The Ranox projectile that "renders but hits nothing". Pick it from
`RANOX_EFFECT_RUNTIME_UNKNOWNS.md`.

**Also in scope: `SetDamageExceptionProcess`** (handed over by Phase 8a; assigned here by the user
on 2026-09-29).
- It is the last call in June's `SetDamageDataFromLUA`: attacker vtable `0x2b8`,
  `(pDamageData, &fPowerRate)`, `32632390@3264050d-32640546`. March makes the same call
  (`X2DamageManager.cpp`, the `MODIFY_SET_DAMAGE_DATA` block after `EndTable`).
- It is where `fPowerRate` reaches the damage values, which makes it the biggest damage lever not yet
  compared. Phase 8a compared only the call's shape, not the callee.
- **Method.**
  1. Resolve slot `0x2b8` in the `CX2GUUser` vtable (`334781f4`, see the README's RTTI note) and in
     the `CX2GUNPC` one. The overrides may differ per class, so compare each against March's
     `SetDamageExceptionProcess` implementations.
  2. Record every difference: which damage fields are scaled, by what, and under which unit or skill
     condition.
- Any change moves every damage number in the game, so list each one in `MODS.md` with before/after
  values, as Phase 8a did. Put the changes under their own flag, `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION`,
  not `..._JUNE_COLLISION`, so they can be reverted separately.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 9"), RANOX_EFFECT_RUNTIME_UNKNOWNS.md
in full, june2017_re/README.md, and funcmap.tsv. Branch: mods/lanox-port.

Your task: identify June's CollisionDataCheck / CheckCollisionGuard and the attack
sphere/line construction, then answer each open item in RANOX_EFFECT_RUNTIME_UNKNOWNS.md
section 4 (4.2-4.8) with a June citation, and transcribe the differences into
X2DamageManager.cpp:1740 and the chain it documents, under a new
SERV_IRUHADEV_JUNE_COLLISION flag. Also finish USE_ADVANCE_COLLISIOIN_CHECK if Phase 7d
handed it over. Update RANOX_EFFECT_RUNTIME_UNKNOWNS.md with the answers.
Then compare SetDamageExceptionProcess (attacker vtable 0x2b8, per unit class) against
March under SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION -- see the phase section.

CP949 byte-level edits only. Build, deploy, ASK before launching. Done when every
section-4 item is answered with a citation and the Ranox projectile named there hits in a
play-test (JUNE_FX HIT lines). Update the matrix and MODS.md.
```

---

## Phase 10 — KTDGXMeshPlayer templet keys

**Status: built and deployed 2026-09-29 (`SERV_IRUHADEV_JUNE_MESHPLAYER`); ~~play-test pending~~
**play-tested 2026-10-03** (dev-console pass T7 passed; `MP_CRASH` seen firing in T6). It can share
one play-test with Phase 9.**
- **Found.** The parser is `ProcessTempletBlock` `31c8ae10`, neither candidate.
  - `31c8d140` is the `CXMeshInstance` ctor.
  - `31c895d0` has the shape of `ChangeXSkinMesh`; it stays a candidate.
  - The chain is Compile `31c8cd30` → `31c8bf00` → `31c8ae10`, with the tokenizer `31c85840` and
    the event factory `31c85350`.
  - Keywords are plaintext.
- **Census.** `census.py --meshplayer` (and every full run, as a matrix section) reports **0
  to-implement**, 1 unsupported, 64 implemented. A negative control (the March side run over the
  pre-phase source) reports exactly the four keys below.
- **June-only keys**, all bools in the tokenizer's `SeqBoolProp` list. March's tokenizer threw on
  each one and the rest of the `.txt` file was never loaded.
  - `ALPHABLEND` (default **true**): `SetAlphaObject()` and all three render params' `bAlphaBlend`,
    in place of `IsAlphaObject()`.
  - `WIREFRAME`: draws in `D3DFILL_WIREFRAME`, then restores the previous fill mode.
  - `SLASH_TRACE_NOT_CULL`: both slash traces skip frustum culling, via the Ranox `m_bForceNotCull`.
  - `POSTEFFECT`: unsupported. June's DG manager has a post-effect render pass (`31cd7810` mode 1)
    and March has none.
- **§4.1 answered:** land and crash semantics are identical (`31c8842f`, `31c82f00`), and so is the
  Mag Charger templet. See `RANOX_EFFECT_RUNTIME_UNKNOWNS.md`.
- **Phase 7e handoff, identified, not transcribed** (`funcmap.tsv`). It is `X2DamageEffect`
  runtime, outside this phase's exit.
  - `mesh+0x478` is the existing `m_Gravity`.
  - `mesh+0x54c` is a June-only per-frame copy of `m_vVelocity`.
  - So the touch-land gate is `m_Gravity.y < 0 && m_vVelocityBefore.y < 0 && m_vVelocity.y > 0`,
    a bounce off land.
- **Particle parser, done the same day** (`SERV_IRUHADEV_JUNE_PARTICLE`). June's particle parser
  (`31cf2250`/`31cfa4a0`/`31cf1e80`) differs by exactly `POSTEFFECT` and `WIREFRAME`. `WIREFRAME` is
  ported (sequence Draw); `POSTEFFECT` is unsupported. The census covers it, with 0 to-implement.
- **Touch-land runtime: moved to Phase 10c** (below). June's three spawners (hit, die, touch-land)
  share a post-create tail that March's existing `CreateHitDamageEffect`/`CreateDieDamageEffect`
  lack. Shipping touch-land at hit-spawner parity would leave three incomplete spawners.

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **June source:** the KTDGXMeshPlayer templet parser. Candidates are `31c895d0` and
  `31c8d140`, which are only known to reference the `KTDGXMeshPlayer.cpp` `__FILEW__`
  string. Confirm which one parses the `.txt` templet.
  - Its key literals may be plaintext, because the `.txt` format predates the Lua
    encryption. Check `xorstrings.tsv` and plain strings both.
- **March target:** `KTDXLIB/KTDGXMeshPlayer.cpp`.
- **Data:** June's plaintext `GameMajorXMeshPlayer*.txt`, `GameMinorXMeshPlayer.txt` and
  `UIMajorXMeshPlayer*.txt`, against March's own copies.

**Scope.**
- `RANOX_EFFECT_RUNTIME_UNKNOWNS.md` §4.1: `UseLand`, `CrashLand`, `Gravity`,
  `ElasticCoeff`, `ElasticCoeff_X`, `NOT_CHECK_LAND_TIME`, `LifeType`, `LightPos`,
  `DrawCount`.
- Every other key June's `.txt` files use that March's parser does not read.
- Extend `census.py` with a MeshPlayer mode.
- **Handed off from Phase 7e: two `CXMeshInstance` member identities**, needed to close
  `TOUCH_LAND_DAMAGE_EFFECT`'s gate (deferred there, `funcmap.tsv`'s `31f83380` row):
  - **`+0x478`** is read by mesh `OnFrameMove` (`31c87b80`) at two points, `@31c87ef1` and
    `@31c8817a`. Both reads fall inside the `+0x633` move-enable ranges Phase 7a already
    handed to this phase (`31c87eea-31c87fe5`, `31c88167`) -- start there rather than a fresh
    scan.
  - **`+0x54c`/`+0x550`/`+0x554`** is written by the same function, `@31c87cac-31c87cc4`, as a
    plain (non-accumulating) per-frame copy of `m_vVelocity`, inside a larger "snapshot the
    previous frame's state" block (`@31c87c8a-31c87cdc`) that also looks like it stores March's
    own `m_fGlobalTimeBefore = m_fGlobalTime` pattern (`KTDGXMeshPlayer.cpp` has that exact
    assignment at `:1711`/`:2179` -- check whether either of those sites is missing the velocity
    copy). If March's block has no matching field, this is a **June-only member to add**, not a
    rename of an existing one; name it for what it is (a velocity snapshot), not for the touch-land
    feature that happens to read it.
  - Once both are named, `CX2DamageEffect::CEffect::JuneCreateTouchLandDamageEffect()` and the
    `TOUCH_LAND_DAMAGE_EFFECT%d` runtime can be finished: the gate condition itself is already
    settled (`funcmap.tsv`'s `31f83380` row, x87-emulated) and its placement in
    `CX2DamageEffect::CEffect::OnFrameMove` is known (right after the die-check, right before
    `DAMAGE_DATA_CHANGE_TIME`). The spawner (`31f81ba0`) still needs its own missing blocks
    transcribed too -- an invoke-rate skip, a damage-rate multiply, the
    `#ifdef INHERIT_FORCEDOWNRATE` arm of `CreateHitDamageEffect`'s shape (not the `#else` arm),
    `SetScaleByUnit`, a flag copy, and `SetMaxLifeTime` -- all read and cited in `funcmap.tsv`'s
    `31f81ba0` row, from `dis_31f81ba0.txt`.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context" rules, "Investigation ledger", "Phase 10"), RANOX_EFFECT_RUNTIME_UNKNOWNS.md
section 4.1, june2017_re/README.md, and funcmap.tsv. Branch: mods/lanox-port.

Your task: confirm which of FUN_31c895d0 / FUN_31c8d140 (or another function) is June's
KTDGXMeshPlayer .txt templet parser, extend census.py with a MeshPlayer mode over June's
plaintext *XMeshPlayer*.txt files, and transcribe every key March's
KTDXLIB/KTDGXMeshPlayer.cpp doesn't read -- parse and its runtime use -- under a new
SERV_IRUHADEV_JUNE_MESHPLAYER flag, with June citations. KTDGXMeshPlayer.cpp is CP949:
byte-level edits only.

Build (KTDXLIB changes need the full chain), deploy, ASK before launching. Done when the
census shows 0 MeshPlayer keys to-implement. Update the matrix and MODS.md.
```

---

## Phase 10c — Spawner post-create tail, then the touch-land runtime

**Status: identification started 2026-09-29, no code.** Found while finishing Phase 10's touch-land
handoff. Under the user's standing rule, a June function March already has must be brought fully
to June's version, not left partial. That makes this a correction to the existing
`CreateHitDamageEffect` and `CreateDieDamageEffect` as well as new touch-land code.

**Paste with:** Starting context + this section.

**Model:** Opus, high, for step 1 (identification). Then Sonnet 5, high.

**The gap.** After `CreateInstance`, June's hit (`31f810c0`), die (`31f82210`) and touch-land
(`31f81ba0`) spawners all make the same calls, in this order:
- `31f8af50` (ported in 7d: `INHERIT_CRITICAL_*`);
- `31f8fdb0`;
- the buff-factor / life-time / damage-rate / `SetScale` blocks (ported);
- `31f76100` (`SetScaleByUnit`: March has it under `GIANT_UNIT_GIANT_EFFECT_TEST`);
- **`31f649d0`, `31f64c00`, `31f65e40`, `31f65f90`**.

None of the unmarked or bold calls is in March. Each is described in its `funcmap.tsv` row:
- **`31f8fdb0`** is the parent→child state copy, gated by flags `+0xb44`–`b47`, `+0xb5c`/`b60` and
  `+0xb64`–`b66`.
- **`31f649d0`** propagates a per-entry damage rate (entry `+0xb0`) through `DAMAGE_DATA_NEXT` and
  every child list.
- **`31f64c00`** copies `ADDITIONAL_ATTACK_TRIGGER_TYPE` (DD `+0x544`) and `ATTACK_TRIGGER_TYPE` (DD
  `+0x53c`).
- **`31f65e40`** / **`31f65f90`** push `IGNORE_DEFENCE[_ON_PVP]` (DD `+0x528`/`+0x529`) and
  `IGNORE_STATE_DEFENCE` (DD `+0x541`) down the whole child tree, via entry flags `+0xb4`/`+0xb5`.

`CEffect.m_DamageData` is at CEffect `+0x200`. The pairs are CEffect `+0x238` ↔ DamageData `+0x38`
and `+0x728` ↔ `+0x528`, and the invoke rate is `+0x720` ↔ DD `+0x520`, per `sdd_walk.py`.

**Step 1: identify, writing each verdict into `funcmap.tsv` as it lands.**
- **CEffect flags `+0xb00`, `+0xb04`, `+0xb32`, `+0xb44`–`b47`, `+0xb5c`/`b60`, `+0xb64`–`b66`.**
  - Known so far:
    - `+0xb32` is `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` (ctor `@31f7c276`).
    - `+0xb5c` may be `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT` (`@31f7f98b`, a `BL` store: check it is
      the getter's destination and not an init).
  - The rest are written from about 25 functions across unit, skill and EffectSet code (`31b09080…`,
    `321db1b0`, `321ee600`, `329fc3f0`, …). So they are runtime state that callers set, not only
    ctor keys.
  - Find each setter. Name the March method it corresponds to, or record the system as absent
    (rule 5).
- **Entry fields `+0xb0` (float), `+0xb4`, `+0xb5` (bools) of the 0xb8-byte `DamageEffectData`.**
  Find which loader writes them, if any, and the `DamageEffectData` ctor defaults.
- **DamageData `+0x5fd`, `+0x5a4`/`+0x5a8`.**
  - `+0x5a8` is `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`'s neighbourhood (`sdd_walk.py`).
  - The spawners copy `+0x7a4`/`+0x7a8`. Do not assume it is `m_eFirstAttack`/`m_RelateSkillData`.
- **The invoke-rate gate.**
  - The touch-land spawner runs the invoke-rate skip only when CEffect `+0xb04 != 0`.
  - March's hit spawner gates it on `DET_ARROW_OF_EXPLOSION != GetType()`.
  - Check June's hit spawner and what `+0xb04` is. If they differ, that is a hit-spawner change too.

**Step 2: port the tail** as CEffect methods mirroring the five callees, called from all three
spawners. Give it its own flag (`SERV_IRUHADEV_JUNE_SPAWN_INHERIT`): it changes existing hit and die
behaviour.

**Step 3: the touch-land runtime** (`SERV_IRUHADEV_JUNE_TOUCH_LAND`).
- **The gate**, `31f83380`, called from CEffect OnFrameMove `@31f8a2a4-31f8a394`, right after the
  die-check:
  - the condition is `m_Gravity.y < 0 && m_vVelocityBefore.y < 0 && m_vVelocity.y > 0`, all strict;
  - it has no once-flag;
  - it first runs a `GetLandPosition` probe whose result nothing reads.
- **The mesh-side prerequisites:**
  - `m_Gravity` is `mesh+0x478` and already exists.
  - `m_vVelocityBefore` is `mesh+0x54c`. It is new and needs the member, a ctor init, the copy in
    March's `KTDGXMeshPlayer.cpp:1709` snapshot block, and a getter. Also check that
    `GetGravity()` exists.
- **The spawner**, `31f81ba0`, fully read in `ghidra/dis_31f81ba0.txt`:
  - The position is always `m_BeforePos`.
  - Rotation: `vAxis = vRotate = m_AngleDegree` when `APPLY_ROTATE_ORIGIN` is set, otherwise
    zero. The entry's `ROTATE`/`POSITION_TYPE` are unused.
  - `APPLY_ROTATED_OFFSET` works as in the hit spawner, else the offset is added.
  - `CreateInstance` gets `m_fLandPos` directly, and the `INHERIT_FORCEDOWNRATE` force-down rate.
  - Then the shared tail.
  - Then `SetInheritForceDownRate`, and max life (mesh `+0x44c`) from the entry's life time when
    `> 0`. Confirm March's `SetMaxLifeTime` does nothing more.
- Give the gate a `JUNE_FX` line, throttled.

---

## Phase 10b — Adoption audits: asset closure, name contract, script conversion

**Status: done 2026-09-25.** Tools and method notes are in `june2017_re/README.md`, section
"Phase 10b". Outputs: `JUNE2017_ASSET_CLOSURE.md`, `JUNE2017_NAME_CONTRACT.md`, and their TSVs in
`june2017_re/`.

**Result 3: Phase 11 converts with route (b), direct transcoding (`june2017_re/ljconv.py`).**

- **What `mar-2014` is, from source, not from elstools.** The March client's LuaJIT,
  `luajitLib/LuaJIT-2.0.2`, differs from stock only in the dump header. `lj_bcdump.h`, under
  `LUAJIT_MODIFIED_BYTECODE_HEADER`, sets `1b 4b 4c` and version `0x81`. `lj_bc.h`'s `BCDEF` is
  stock order. A diff against upstream's `v2.0` branch shows nothing else but the header, an
  `X2OPTIMIZE` guard, and later upstream fixes. `lj_bcread.c` rejects any other version, so June's
  `0x82` files must be converted.
- **The table.** 44 KOG opcodes are mapped: elstools' translation plus `FIX`. It is injective, and
  every slot's operand types match June's `lj_bc_mode[]`. The 14 files use 38 distinct opcodes; an
  unmapped one is a hard error.
- **The plan's disassembly check passes.** `luajit -bl` of each transcoded file against its
  `.std.luac`:
  - The 9 files without FIX-slot opcodes are byte-identical past the header.
  - The other 5 differ only on their FIX-slot lines, exactly 22 / 3 / 3 / 3 / 7 of them.
  - This check is close to tautological, because the table is read back from `.std.luac`. The
    evidence that discriminates is next.
- **Execution** (`june2017_re/fxdump.lua`: run against stub engine objects, dump what gets
  defined):
  - **Route (b)** loads all 14 files through the `0x81` loader and runs every one to completion.
    Its stub-call counts equal the source's call sites: `AddEffectSetTemplet_LUA` 8242 / 287 /
    138 / 12, descriptions 230, damage data 107, unit viewer 18.
  - **Route (a)** loses data three ways:
    - `DamageEffect_Dungeon` does not compile: ljd emits `function … end(…)`, `.src.lua:3910`.
    - `EffectSet.lua` aborts after 2,925 of 8,242 templets.
    - `DamageEffect.lua` aborts with 837 damage effects never defined. Both crash in the bogus
      `deepcopy` (`attempt to call global 'uv0'`) and leak `slot0`..`slot5` as globals.
  - **Where both run, they agree exactly:** every captured call, and 7,869 of 7,870 shared
    globals. The one difference is `DamageEffect_Gunner_Combo_ZZZX_Grenade_Explosion`, which
    `DamageEffect.lua` defines twice (lines 328616 and 340940); route (a) crashed before the
    second definition.
- **Not a third route: the vendored `luajit.exe`.** It loads `0x82`, but with the same
  `0x2f`–`0x37` swap as elstools: June's `deepcopy` lists as `USETN`/`USETP`/`USETV`.
- **For Phase 11: how the converted files are packed.** This is the user's workflow, with one step
  changed.
  - The user normally decompiles June lua to `name.src.lua` and puts it in
    `kom_contents/dataNNN_kom/`. `elstools compile-dir` then produces `name.lua`, and only
    `"name.lua"` is registered in `_manifest.json`.
  - For the effect files, `ljconv.py`'s output replaces the decompile and compile steps: it
    already is a compiled `name.lua`.
  - Manifest values follow the user's rule: `.lua` = `"2"`, everything else = `"0"`.
  - Not yet checked: that `elstools pack` takes a compiled `.lua` as it is.
  - **A stale source would silently undo the conversion.** `kom_contents/data036_kom` holds
    `EffectSet.src.lua` and `DamageEffect.src.lua`, the Ranox-merged sources. `elstools compile-dir`
    recompiles every `name.src.lua` into `name.lua`, so running it after dropping in the converted
    files puts the merged copies back. Nothing reports it. Move those two `.src.lua` files out
    first, or skip `compile-dir` for that directory.
- **For Phase 11: the chunks depend on run-time state.**
  - `DamageEffect.lua:351018` reads `INIT_PHYSIC.WALK_SPEED`. No effect file defines it; every
    character script assigns it globally. In a Lua state where it is absent, the rest of that
    chunk never runs. Check the load order.
  - Globals read as nil, as the client also sees them: studio typos `TURE`, `FLASE`, `FASLE`, `d`,
    and the undefined `RVK_FlashBang_Ulti_Scale`.
  - Also read as nil, and harmless: `AET_MAX`, `RWT_END` and `SGI_END` in June's `Enum.lua`. They
    are bare names written as list entries (`SERVER_GROUP_ID = { SGI_END, SGI_GAIA = 1, … }`,
    `Enum.src.lua:98`), so they only add a nil slot. March's enum has the same `SGI_*` pattern.
    The engine registers nothing for them to find.

**Result 1: asset closure (`JUNE2017_ASSET_CLOSURE.md`).**

- **Scope.** 17,816 roots reach 7,392 file nodes, through every particle and mesh-player templet
  and the textures inside every mesh and `.xet`. The completeness gate is clean: 0 unexplained
  strings, and 6 `NAME` keys that June's particle loader never reads.
- **Verdicts:**
  - 3,159 same;
  - 364 same-prefix;
  - 59 differ, 0 of them atlases (no effect texture has a `.tet` partner);
  - 15 live March copies that are truncated ports;
  - 12 pending pack;
  - 3,622 June-only: 3,081 extract byte-exact; 541 extract 1–255 bytes short (see next point);
  - 156 in neither build.
- **Found on the way: elstools 1.0.0 extracts about 18.7k June algo-2 entries 1–255 bytes short.**
  - The lost bytes are the file's tail: a DDS's smallest mips, and, in the meshes sampled,
    mostly binary `.x` closing-brace tokens. Occasionally it is list data.
  - The user reports such copies work in the March client when packed as lua = algo 2, everything
    else = algo 0. So they are usable, but not byte-identical to June.
  - 51 `march2014/update/` files are such short copies, 49 of them live. Table in the closure
    document. Nothing was changed.

**Result 2: name contract (`JUNE2017_NAME_CONTRACT.md`).**

- **Scope.**
  - All 432 X2Lib and KTDXLIB translation units, `cl /P` under `US_SERVICE`, 0 failures.
  - 48,591 string literals, wide and narrow: `CreateInstance_LUA` takes a `char*` name.
  - A literal counts when March's own data defines it as an effect set, damage effect, particle
    or mesh-player templet.
  - Seven `StringCchPrintf` format strings (e.g. `L"Effectset_Swallow_Assault_Shadow0%d"`,
    `X2GUAra.cpp:11166`) were matched against every defined name, for their run-time names.
  - 1,802 names in total.
- **Loaders.** Every `OpenScriptFile` / `Reset*Particle` / `Reset*XMeshPlayer` call opens only
  the standard particle and mesh-player files. `data121`'s `rena_A_trapingArrow_Particle.txt` /
  `_Xmesh.txt` ship in both builds, and neither build's code opens them.
- **Verdicts:**
  - 1,727 kept.
  - 58 kept only in files March does not load yet: 50 in `GameMajorParticle44.txt`, 7 in
    `GameMajorParticle36.txt` (the `BossRaid_Gate_P0%d` family), 1 in `DamageEffect_Dungeon.lua`.
    Phase 11 has to extend March's loader lists.
  - The punch list, 17 names. Each is also checked against **June's own binary** (plain strings
    plus `xorstrings.tsv`), a second, independent source:
    - **15 are gone from June's code: June changed those call sites.** On adoption, the March
      call site has to change too. They are:
      - 7 dropped: `EffectSet_Aisha_DeathFieldBomb`, `EffectSet_Aisha_DeathField_FirstImpact`,
        `EffectSet_ELSWORD_DOUBLE_SLASH_HYPER`, `EffectSet_RAVEN_VJAVELIN`,
        `CHUNG_BIGBANGSTREAM_MEMO`, `DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell`,
        `DamageEffect_LGA_Arc_Shot`;
      - 7 rename candidates, same references (mostly Raven `*_HYPER` damage effects). Only
        `EVE_EP_DASH_COMBO_Z` has candidates June's binary asks for (`EVE_DASH_COMBO_Z`,
        `EVE_AT_DASH_COMBO_Z*`);
      - 1 case change, confirmed by the binary: `DamageEffect_Cryotron_Bolt_GUIDED_ARROW`
        became `…_Guided_Arrow`, and June asks for the new spelling.
    - **2 are still asked for in June's binary but defined nowhere: dead in June too**, so inert
      for parity. They are `BOMB` (`X2GUNPCFunc.cpp:2711/2759`) and
      `EffectSet_CSI_CIP_LAND_DEMOLISHER_AFTER_EFFECT`.
- **13 names already dead in March** (effect call sites, no March definition).
  - Bone names and parser keywords are excluded.
  - June defines 4 of them, so adoption brings them to life: the `X2GUAdd.cpp` Reverse Reactor /
    Psychic Storm / Dust Anatomy damage effects.
- **Not reachable by construction:** names assembled only from variables (`%s_%s`).

**Paste with:** Starting context + this section.

**No engine dependency.** These are static checks over data and source. They can run any
time before Phase 11, and running them early sizes the data side of adoption. No client
code changes in this phase.

1. **Asset closure → `june2017_re/assets.py` → `JUNE2017_ASSET_CLOSURE.md`.** Start from
   every June `EffectSet*`/`DamageEffect*` templet and follow each reference to its end:
   - the effect set / damage effect → mesh-player and particle templet names
     (`GameMajorXMeshPlayer*.txt`, `GameMajorParticle*.txt`, and their UI/Minor
     siblings);
   - those templets → meshes (`.X`), textures (`.dds`/`.tga`, and `.TET` atlases),
     sounds (`.ogg`/`.wav`), and any nested templet.

   For each leaf, record where it is found: in June's `.kom`s, in March's live `.kom`s, or
   in `march2014/update/`. Also record whether a March copy differs from June's, by size
   and hash.
   - "Differs" is the dangerous bucket. Swapping a shared atlas breaks every other swatch
     in it (see `RANOX_PORT_PLAN.md`, the `DLG_UI_Button17` / Texture71 merges). Flag
     those; don't resolve them.
   - The output is a list for the user. **The user packs.** Nothing is copied into the
     game directory.
2. **Name contract → `june2017_re/names.py`.** Collect every effect-set, damage-effect,
   mesh-player and particle name that March's C++ asks for by literal. Examples:
   `PlayEffectSet( L"..." )`, `CreateInstance( ..., L"..." )`, and `GetMajorXMeshPlayer`
   lookups, in `X2Lib`/`KTDXLIB`, preprocessed under `US_SERVICE` as `census.py` does.
   Diff that set against the names June's files define.
   - A March-only name is an effect that **silently disappears** on adoption. For each
     one, record whether June renamed it (by content), moved it, or dropped it, and
     which March call site uses it.
   - Do not fix any of them here. They are the adoption punch list.
3. **Script conversion method.** June's lua is KOG LuaJIT `KL\x82` bytecode. Two routes
   to `mar-2014`:
   - **(a) decompile + recompile:** `ljd_jun2017.py`, then
     `elstools compile-batch -v mar-2014`. **Known risk:** ljd mis-structures the
     code-built tables in `DamageEffect.lua` (README, "ljd caveat").
   - **(b) direct bytecode transcoding:** the header byte, plus KOG→stock opcode
     renumbering. The README says `mar-2014` uses stock numbers and differs only in the
     header. That is **unverified** as a claim about the whole format, so test it
     first. For each of the 14 June files, transcode, then disassemble both the
     transcoded file and the `decompile-batch` `.std.luac`, and require them to be
     identical.

   Write the result into this section. Name the route Phase 11 uses and give the evidence.

**Exit criteria.** `JUNE2017_ASSET_CLOSURE.md` exists and every leaf has a location
verdict. The name-contract list is complete, with a verdict on each name. There is a
decided, evidenced conversion route.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context", "End goal", "Phase 10b"), june2017_re/README.md (especially "June
LuaJIT opcode table" and the ljd caveat), and RANOX_PORT_PLAN.md's atlas-merge notes.
Branch: mods/lanox-port.

Your task: the three adoption audits, static only, no client code changes. (1) Asset
closure from every June EffectSet*/DamageEffect* templet down to meshes/textures/sounds,
with a location + differs verdict per leaf -> JUNE2017_ASSET_CLOSURE.md (flag shared
atlases, don't resolve them; I pack, nothing goes into the game directory). (2) Name
contract: every effect/mesh/particle name March's C++ requests by literal vs. the names
June's files define; verdict per March-only name. (3) Decide the June->mar-2014 script
conversion route: test direct bytecode transcoding against the decompile/recompile route
and record the evidence. Commit with my OK.
```

---

## Phase 11 — Enums, June-data trial load, then adoption

**Paste with:** Starting context + this section. Needs Phase 10b's outputs.

**The destination is adoption, not a trial for its own sake** (see *End goal*). The trial
in a throwaway copy is the safety step before June's files replace the merged March
copies in the live data.

1. **Enum data.** Add to `march2014/data036/Enum.src.lua` every June enum table and
   member that June's adopted files reference. The Phase 1 enum census lists them.
   - **This supersedes Phase 1's "no reader, not added" rule for adoption.** A missing
     enum name makes June's lua index nil, which aborts the rest of that chunk. A member
     with no C++ reader is still added, and marked reader-less in the matrix, so it is
     visibly inert.
   - Take the values verbatim from June's `Enum.lua`.
   - Preserve the UTF-8 BOM and CRLF byte for byte.
   - Add a comment naming this plan.
   - Then compile with `elstools compile-batch -v mar-2014`. **The user packs it.** Name
     the exact file(s) when reporting.
2. **Trial load.** Point the client at June's own `EffectSet*.lua` and
   `DamageEffect*.lua`, converted to `mar-2014` by the route Phase 10b chose, instead of
   the merged March copies. Include the assets Phase 10b's closure found missing.
   - Run it from a **separate copy of the game directory**, never `$DATA`. Copy
     everything except `els_db.sql*`, and give the copy its own save.
   - CLAUDE.md forbids loose copies in the game directory. That rule exists because a
     loose file masks a failed repack, and it applies to the real `$DATA`. In the
     throwaway copy, loose-file precedence is acceptable, but only because the copy is
     deleted afterwards. That precedence exists under `US_SERVICE` too for now, since it
     still defines `_IN_HOUSE_` and not `MASS_FILE_FIRST`.
   - Do not build `US_INTERNAL` unless the user asks for it.
   - Alternatively, pack a trial `data036.kom` for the copy. Ask the user which they
     prefer.
   - Record every load error in `OpenScriptFile.log`/`ErrorLog.txt` and every unread key
     the census still reports. Delete the copy when done, after asking.
   - **Settle the deferred log checks here.** Phase 2's `JUNE_FX ES_TEMPLET` lines must
     appear, for example `SYNC_SHOW_BY_CREATE_UNIT` on
     `EffectSet_KILL_ENEMY_WHEN_FULL_HYPER_MODE`, and `OVERLAP` on
     `EffectSet_ALH_LINK_FLOWER_DeBuff`. The same goes for any later phase's check that
     could not fire on merged data.
   - Play-test the name-contract punch list from Phase 10b: every March call site whose
     name June renamed or dropped.
3. **Report.** Write `JUNE2017_TRIAL_LOAD.md` with the results. It is a punch list of what
   blocks adoption, not a keep-or-adopt verdict.
4. **Adopt**, once the punch list is clear or each remaining item is accepted by the user:
   - **The user packs** June's converted files, the enum update and the missing assets
     into the live archives. Name every file.
   - Retire the Ranox-merged copies in `march2014/data036/`. Ask before removing them;
     they stay in git history.
   - Verify from the live game directory's logs, not the copy's.

**Kickoff prompt**:
```
Continuing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Starting context", "Phase 11") and JUNE2017_EFFECT_KEY_MATRIX.md. Branch:
mods/lanox-port.

The goal is adopting June's effect files, not keeping the merge (see "End goal").

Your task: (1) add every June enum table/member that June's effect files reference to
march2014/data036/Enum.src.lua (reader-less ones too, marked inert in the matrix), values
verbatim from June's Enum.lua, BOM and CRLF preserved, compiled with elstools
compile-batch -v mar-2014 -- I pack it; name the exact file for me. (2) Trial-load June's
own EffectSet*/DamageEffect* lua, converted by Phase 10b's chosen route, plus the missing
assets from JUNE2017_ASSET_CLOSURE.md, from a SEPARATE COPY of the game directory (never
the live one, never my els_db.sql). Ask me whether to use loose files or a trial .kom in
that copy, don't build US_INTERNAL unless I ask, and ask before deleting the copy
afterwards. Record every load error and every key still unread; confirm the deferred
JUNE_FX checks (Phase 2's ES_TEMPLET lines) fire; play-test Phase 10b's name-contract list.
(3) Write JUNE2017_TRIAL_LOAD.md as an adoption punch list. (4) When it's clear, name
every file I need to pack for adoption, and ask before retiring the merged March copies.
Ask before launching the client, every time.
```

---

## Phase 12 — Docs and memory

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high. See "Model and effort per phase".

- **`RANOX_EFFECT_ENGINE_GAPS.md`:**
  - Rewrite §6: the packed `x2.exe` vs. `unpacked_x2.exe`, the XOR-string encryption, and
    a pointer to `june2017_re/`.
  - Close the §1 items (both named blocks) and the §2 enum tables with the phase that
    closed each.
  - Revisit the Phase C `ATTACH_SKIN_MESH` inference and its `VALUE=0.01` against June's
    code, if Phase 3 or 4 reached it. Otherwise open a follow-up.
- **`RANOX_EFFECT_RUNTIME_UNKNOWNS.md`:** confirm that every §4 item carries its Phase 9
  answer.
- **`MODS.md`:** every `SERV_IRUHADEV_JUNE_*` flag, what it changes, and how to revert it.
  Remove `SERV_IRUHADEV_JUNE_FX_DEBUG` if the user agrees.
- **`CLAUDE.md`:** a short pointer to this plan and `june2017_re/`, in the architecture
  section next to *Offline mode*, with the user's OK.
- **Memory:** record the non-obvious facts:
  - the June binary is readable (dump path and hash);
  - the XOR idiom;
  - CEffect's runtime methods are not virtual;
  - `SetDamageData` is shared, so a semantic change there moves every damage number.

**Kickoff prompt**:
```
Finishing the June2017 effect-engine rebuild. Read JUNE2017_EFFECT_REBUILD_PLAN.md
("Phase 12"), JUNE2017_EFFECT_KEY_MATRIX.md, and MODS.md. Branch: mods/lanox-port.

Your task: bring the docs in line with the code. Rewrite RANOX_EFFECT_ENGINE_GAPS.md
section 6 and close sections 1-2 with the phases that closed them; revisit the Phase C
ATTACH_SKIN_MESH inference against June's code; confirm RANOX_EFFECT_RUNTIME_UNKNOWNS.md
section 4 is fully answered; make MODS.md list every SERV_IRUHADEV_JUNE_* flag with revert
steps (ask me before removing SERV_IRUHADEV_JUNE_FX_DEBUG); ask before adding the CLAUDE.md
pointer; save the non-obvious facts to memory.
```
