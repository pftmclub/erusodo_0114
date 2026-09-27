# Ranox Lua decompile bug: dropped assignments become stale globals

Tracking document for a decompiler defect found 2026-09-27 while fixing Dryad Kenta's
"jumps off the map, never comes back" softlock. This is a **known-issues / triage list**,
not a phase plan — nothing here is scheduled, most of it is unverified.

## The bug

`elstools`' vendored decompiler (`elstools/vendor/ljd`, third-party MIT-licensed code, not
KOG's or this project's) sometimes drops a real bytecode assignment and inlines the
computing call straight into the `if`/`elseif` condition that reads it. Printed example
(from `RAGE_DRYADKENTA.src.lua` before the fix):

```lua
-- what the decompile prints (looks like valid Lua, reads fine):
if slot1:GetEffectSet() ~= nil then
    slot5:PlayEffectSetWithCustomPos_LUA(...)
end

-- what the real bytecode actually does:
slot5 = slot1:GetEffectSet()
if slot5 ~= nil then
    slot5:PlayEffectSetWithCustomPos_LUA(...)
end
```

The printed text calls `GetEffectSet()` once, uses the result only for the nil check, and
then the body reads `slot5` — whatever it happened to hold from an earlier, unrelated
statement.

This is far worse here than in a typical decompiled codebase because **none of the
Ranox-ported `.src.lua` files declare Lua's `local`.** A bare `slotN = ...` at function
scope compiles under `elstools compile-batch -v mar-2014` to a literal Lua **global**
(`_env["slotN"]` — confirmed by disassembling the compiled output with `ljd`'s `--asm`
mode), not a per-call stack register. So a dropped assignment doesn't read as Lua's
relatively safe `nil` (`==` against nil doesn't error) — it reads as whatever that global
last held: an earlier branch's leftover value, a stale value from a previous frame's call to
the same function, or in the worst case another script's own bare `slotN`. Comparisons other
than `==`/`~=` (`<=`, `<`, `>`, `>=`) *do* error against a genuinely unset global, so the
failure mode ranges from silently wrong data to a hard Lua error that aborts the calling
function mid-statement.

## Confirmed and fixed

**`march2014/update/data037/RAGE_DRYADKENTA.src.lua`** — 6 instances, 2 of which
independently caused the reported softlock (Attack B's off-map "jump" never computing a
valid return position; the 60%/20% HP dash's target position left unset, so the dash never
ends). Full writeup, per-function detail, and the exact fix are in `MODS.md`'s "Dryad Kenta
(Burned Forest boss)" entry — confirmed both against the real June2017 bytecode and,
independently, against the raw disassembly of the *currently deployed* `data037.kom`'s own
compiled `RAGE_DRYADKENTA.lua`. Still needs recompiling and repacking into `data037.kom`
(the user's step) and an in-game play-test before it can be called done.

## Detecting more instances

`june2017_re/lint_decompile_slots.py` flags any `slotN` read in a function before that same
function ever assigns it — the signature this bug leaves behind, since a dropped assignment
is otherwise invisible on a read-through (the printed Lua still parses and reads as
plausible). It's a **heuristic, regex-based scan, not a bytecode-level check** — every hit
still needs verifying against real bytecode before being trusted as a real defect. It has
already had one round of false-positive fixing (`for slotN = a, b do` and
`for slotN in ... do` declare their own loop variable; the first version of this scanner
didn't know that and over-reported).

```sh
python june2017_re/lint_decompile_slots.py <file.src.lua> [more.src.lua ...]
```

**How to verify a candidate** (same process used for all 6 confirmed `RAGE_DRYADKENTA`
instances):

1. Re-run the lint on the specific file to confirm it's still flagged and see the exact
   line(s).
2. Find the file's June2017 original under
   `F:\...\237311\14JUN17\data\dataNNN_kom\<name>.lua` (folders suffixed `_kom`) and
   decompile it with `june2017_re/ljd_jun2017.py` (this handles the *separate* June
   opcode-table bug documented in `june2017_re/README.md`; needs a `.std.luac` sibling from
   `elstools decompile-batch` first).
3. To check what's actually packed and deployed rather than re-deriving from June source:
   `elstools extract -v mar-2014` the live `.kom`, then run
   `elstools/vendor/ljd/main.py --asm -f <name>.std.luac` directly on the extracted file's
   `.std.luac`. This raw pseudo-asm mode works even when full AST decompile fails, which it
   can on a file that was itself compiled from already-broken text (happened on the live
   `RAGE_DRYADKENTA.lua`).
4. Fix by restoring the real assignment ahead of its reads, matching the file's own
   no-`local` bare-global convention (don't introduce `local` — the global-write model is
   safe *within* one function's own sequential statements, since nothing else runs between
   them; it's only unsafe across statements that never actually execute, which is the bug
   itself). Verify with the lint (should drop to zero for that function) and
   `elstools compile-batch -v mar-2014` on a **scratch copy** (the compiler overwrites its
   input in place).
5. Note the `SERV_IRUHADEV_RANOX_PORT (<date>): ...` comment convention already used
   throughout `RAGE_DRYADKENTA.src.lua` for these fixes — cite the bytecode evidence, not
   just "this looked wrong."

## Scope: every ported file with this pattern

Ran across all 277 `.src.lua` files under `march2014/` on 2026-09-27. **61 files flagged, 60
of them inside `march2014/update/data037/`** — the Ranox monster/boss AI scripts, same
category as Kenta. That's consistent with the bug's shape: it only shows up in Lua with
branching control flow feeding a later shared statement, and `data037/`'s per-frame AI state
machines are full of that; almost everything else in the port (UI dialogs, dungeon/line-map
data, enum tables) is closer to a plain table literal and doesn't trip it.

**"Kind" below is a guess from the filename, not structural analysis** — useful for
prioritizing, not authoritative. Nothing in this table has been bytecode-verified except
`RAGE_DRYADKENTA.src.lua` (already fixed, so it no longer appears) and
`MapData.src.lua`'s `ComputeCOBOExpressTicketCost` (verified by hand, confirmed real: reads
an unassigned `slot6` in a travel-ticket cost formula).

| Hits | File | Kind (guessed) |
|---:|---|---|
| 275 | `update/data037/SECRET_MAGI_BOSS.src.lua` | Boss |
| 216 | `update/data037/SECRET_SCENE_DUMMY.src.lua` | Non-combat (scene) |
| 41 | `update/data037/FIRE_DEVIL_IGNIA.src.lua` | Mob |
| 23 | `update/data037/SMITH_SCIENCE.src.lua` | Mob |
| 21 | `update/data037/GEYSER_MULSAYRENA.src.lua` | Mob |
| 19 | `update/data037/SAMETEAM_IGNIA.src.lua` | Mob (already noted once in `RANOX_PORT_PLAN.md`, independent of this doc) |
| 16 | `update/data037/FIRE_DEVIL_SKA.src.lua` | Mob |
| 15 | `update/data037/FIRE_DEVIL_CHAIN_BALL.src.lua` | Mob/object |
| 14 | `update/data037/FIRETEMPLE_REGIEON_POINT.src.lua` | Non-combat (region trigger) |
| 13 | `update/data037/LOW_SMITH_SCIENCE_FD.src.lua` | Mob (field-difficulty variant) |
| 10 | `update/data037/FIRE_DEVIL_IGNIA_BOSS_FINALHALL.src.lua` | Boss |
| 9 | `update/data037/SMITH_RUNNER.src.lua` | Mob |
| 8 | `update/data037/LOW_FINALHALL_SNIKER_FD.src.lua` | Mob |
| 6 | `update/data037/TRAP_FIRE_BIG.src.lua` | Non-combat (trap) |
| 6 | `update/data037/SMITH_STAIN.src.lua` | Mob |
| 6 | `update/data037/EVENT_FINALHALL_BOSS2.src.lua` | Boss |
| 5 | `update/data037/MOUNT_FIREBIRD_FIRETEMPLE.src.lua` | Mob |
| 4 | `update/data037/MOUNT_POINT_STAGE_BOSS.src.lua` | Boss |
| 3 | `update/data037/SMITH_ALUMINUM.src.lua` | Mob |
| 3 | `update/data037/LOW_SMITH_ALUMINUM_FD_BOSS.src.lua` | Boss |
| 3 | `update/data037/LOW_GEYSER_ALUMINUM_FD_BOSS.src.lua` | Boss |
| 3 | `update/data037/GEYSER_MULLOCK.src.lua` | Mob |
| 3 | `update/data037/GEYSER_ALUMINUM.src.lua` | Mob |
| 2 | `update/data037/SECRET_SUMMON_SNIKER.src.lua` | Mob (summon) |
| 2 | `update/data037/SECRET_SUMMON_FIREBIRD.src.lua` | Mob (summon) |
| 2 | `update/data037/SECRET_SNIKER.src.lua` | Mob |
| 2 | `update/data037/SECRET_SCENE_1_1.src.lua` | Non-combat (scene) |
| 2 | `update/data037/SECRET_FIREBIRD.src.lua` | Mob |
| 2 | `update/data037/SECRET_CRUEL_SNIKER.src.lua` | Mob |
| 2 | `update/data037/MOUNT_FIREBIRD.src.lua` | Mob |
| 2 | `update/data037/LOW_MOUNT_FIREBIRD_FD.src.lua` | Mob |
| 2 | `update/data037/LOW_MOUNT_BERSERKER_BOSS.src.lua` | Boss |
| 2 | `update/data037/LOW_FINALHALL_FIREBIRD_FD.src.lua` | Mob |
| 2 | `update/data037/FIRE_DEVIL_SNIKER_INCHANT.src.lua` | Mob (enchant variant) |
| 2 | `update/data037/FIRE_DEVIL_SNIKER.src.lua` | Mob |
| 2 | `update/data037/FIRE_DEVIL_BERSERKER_BOSS_FINALHALL.src.lua` | Boss |
| 2 | `update/data037/FIRE_DEVIL_BERSERKER_BOSS.src.lua` | Boss |
| 1 | `update/data037/SMITH_CART.src.lua` | Non-combat (object) |
| 1 | `update/data037/SECRET_SUMMON_BERSERKER.src.lua` | Mob (summon) |
| 1 | `update/data037/SECRET_STAGE1_CHAIN2.src.lua` | Non-combat (trigger) |
| 1 | `update/data037/SECRET_STAGE1_CHAIN1.src.lua` | Non-combat (trigger) |
| 1 | `update/data037/SECRET_MAGI_SUMMON.src.lua` | Mob (summon) |
| 1 | `update/data037/SECRET_CRUEL_BERSERKER.src.lua` | Mob |
| 1 | `update/data037/SECRET_BERSERKER.src.lua` | Mob |
| 1 | `update/data037/LOW_MOUNT_BERSERKER_FD.src.lua` | Mob |
| 1 | `update/data037/LOW_GEYSER_SCIENCE_FD.src.lua` | Mob |
| 1 | `update/data037/LOW_GEYSER_MULMAI_FD.src.lua` | Mob |
| 1 | `update/data037/LOW_GEYSER_MULLOCK_FD.src.lua` | Mob |
| 1 | `update/data037/LOW_GEYSER_MULLOCK_CALL_FD.src.lua` | Mob (call/summon variant) |
| 1 | `update/data037/LOW_FINALHALL_BERSERKER_FD.src.lua` | Mob |
| 1 | `update/data037/GEYSER_SCIENCE.src.lua` | Mob |
| 1 | `update/data037/GEYSER_MULSAYRENA_CALL.src.lua` | Mob (call variant) |
| 1 | `update/data037/GEYSER_MULMAI.src.lua` | Mob |
| 1 | `update/data037/GEYSER_MULLOCK_CALL.src.lua` | Mob (call variant) |
| 1 | `update/data037/FIRE_DEVIL_BERSERKER_INCHANT.src.lua` | Mob (enchant variant) |
| 1 | `update/data037/FIRE_DEVIL_BERSERKER_FIRETEMPLE.src.lua` | Mob |
| 1 | `update/data037/FIRE_DEVIL_BERSERKER.src.lua` | Mob |
| 1 | `update/data037/EVENT_GAYSER_BOSS.src.lua` | Boss |
| 1 | `update/data037/EVENT_GATEWAY_BOSS.src.lua` | Boss |
| 1 | `update/data037/EVENT_FINALHALL_BOSS.src.lua` | Boss |
| 1 | `data036/MapData.src.lua` | Shared registry data (outside `data037/`, hand-verified real) |

All paths are relative to `march2014/`.

## Not done in this pass

- Nothing in the table above has been fixed except `RAGE_DRYADKENTA.src.lua` (which is why
  it no longer appears in the table).
- No attempt made to fix the root cause inside `elstools/vendor/ljd` itself (its temporary-
  elimination pass, `ljd/ast/slotworks.py`) — not investigated closely enough yet to know
  whether it's a small or large surgery, and a wrong fix there risks producing decompiles
  that look even more plausible while still being wrong, which is worse than the current
  known failure shape.
- The lint has not been wired into the decompile workflow (`ljd_jun2017.py` /
  `elstools decompile-batch`) as an automatic guard — right now it's a standalone script you
  have to remember to run.

See also: `MODS.md`'s Dryad Kenta entry (the full fix, as a worked example), and
`RANOX_PORT_PLAN.md`'s Phase 4 section (carries a correction note pointing back here, since
its original write-up reasoned about one instance of this bug and got it half right).
