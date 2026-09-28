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

## Root cause

Identified 2026-09-28 in Phase 3 of `RANOX_LUA_DECOMPILE_FIX_PLAN.md`. No fix has shipped.
Line numbers are for the vendored `elstools/vendor/ljd`, which is byte-identical to upstream
master (see "Upstream status").

**In one sentence.** `slotworks` inlines a temporary when it counts exactly one read of it.
Every collection it runs, however, sees only part of the value's live range. A read it cannot
see, or cannot attribute, is not counted. So a value read once inside the view and again
outside it counts as single-use, and it is folded into the one read that was counted. The
upstream maintainer describes this class in an issue that is still open (see "Upstream
status").

### Minimal repro and causal chain

`june2017_re/ljd_oracle/cases/minimal.lua::minimal`:

```lua
function minimal(o)          -- ljd, stripped:  function minimal(slot0)
	local x = o:Get()        --                    if slot0:Get() then
	if x then                --                        -- Nothing
	end                      --                    end
	return x                 --                    return slot1      <- never written
end                          --                end
```

The bytecode is two basic blocks: block 0 is `CALL 1`, `ISF 1`, `JMP`; block 1 is `RET1 1`.

1. **Stripped bytecode leaves `x` as a slot.** `mark_locals` finds no variable info, so
   register 1 stays `T_SLOT`.
2. **The main pass sees the read in block 0.** `main.py:380` calls
   `eliminate_temporary(identify_slots=True)`. The collector sees `slot1 = slot0:Get(slot0)`
   and the `ConditionalWarp` reading `slot1` in the same block, and gives both id `#2`.
   `leave_block` (`slotworks.py:907-913`) commits the info with those 2 references, then
   clears `known_slots`.
3. **The read in block 1 is attributed correctly but left ambiguous.** For `return slot1`,
   `_get_slot` misses, and `_find_slot_assignments` (`:724-754`) walks warp predecessors to
   the one reaching definition, `slot1#2`. `visit_identifier` (`:857-861`) then computes
   `update_ids = True` but passes a literal `False`. The reference is appended to `slot1#2`'s
   list with `id == -1`, and `2` goes only into `_ids`.
   - Log: `COLLECT cross-block read slot1 id=-1 fn=_env.minimal identify=True
     reaching=['slot1#2'] (update_ids computed=True, NOT passed)`
4. **The ambiguous read is not counted.** `_eliminate_temporary` passes
   `ignore_ambiguous=True` (`:100`), and `_fill_simple_refs` `:285-287` skips every `id == -1`
   reference. Only the condition is counted, so the policy at `:344-346` puts the slot in
   `data.simple`.
   - Log: `FILL slot1#2 fn=_env.minimal src=slot1#1(slot2#0) refs=2 counted=1 skipped=1
     ... -> SIMPLE`, with `ref slot1#-1{2} holder=Return SKIPPED(id==-1, :286)`
5. **The assignment is removed and its value moved into the `if`.** `_eliminate_simple_cases`
   (`:367-426`) invalidates the assignment and substitutes `slot0:Get()` into the condition.
   The `Return` keeps `slot1`.
   - Log: `ELIMINATE slot1#2 ... into holder=ConditionalWarp`
6. **The round trip reads an unset global.** With no `local`, `slot1` compiles to
   `GGET "slot1"` and nothing wrote it, so the round trip logs `POISON_READ slot1`.

**What triggers it.** The trigger is a definition with **exactly one read in its own basic
block**, plus any number of reads elsewhere.
- **The paired control.** `minimal_ctl` tests another value. At `leave_block` its definition
  has only its assignment reference, so `_commit_info` (`:756-762`) files it as `unused`.
  Later cross-block reads are appended to an info that is never processed, so it is kept.
- **Other shapes that escape the main pass the same way.** A read first in an `elseif`
  condition, or as the second operand of an `and`, is in another block. The main pass never
  considers those slots.
- **Phase 1's wrinkle.** It escapes the main pass in `hp_ratio_elseif`, `hp_two_ifs_and` and
  `merge_and_cond` alike, and the difference comes later, at site (c) below:
  - In `hp_ratio_elseif`, the elseif-condition read (`path=.../If/Block/If/Block/If/...`) is
    lost, so the value is folded.
  - In the other two, both reads are found (`counted=2`, UNSAFE), so the value is kept.

### Why only stripped bytecode

- **Under `-g` the collector never sees the variable.** `locals.py:73-74` renames every
  register that has variable info and sets it to `T_LOCAL`. The collector ignores anything
  that isn't `T_SLOT` (`slotworks.py:799`, `:846`), so `x` is never a candidate.
  - Census after `mark_locals` for `minimal`: 10 `T_SLOT`/0 `T_LOCAL` stripped, against
    4 `T_SLOT`/6 `T_LOCAL` under `-g`.
- **The unsafe extra pass plays no part.** It is gated on `_debuginfo.variable_info`
  (`main.py:421`).
  - On stripped input it is skipped for every top-level assignment. That is 56 in Kenta's
    June file, every function included, plus every function of every stripped oracle case.
  - Under `-g` the gate passes, but no slot assignments are left, so `eliminate_temporary`
    is never called. `UNSAFE-EXTRA RUNS` is 0 on every input logged.

### One defect, four sites

Phase 4 needs this more than anything else in the section: **closing one site hands the value
to the next pass, which drops it again.** Each site is a collection whose view is narrower than
the value's live range:

| Site | Where | What that collection can't see | Where it drops a value |
|---|---|---|---|
| (a) | `slotworks.py:285-287` and `:857-861`, in the `main.py:380` pass | Cross-block reads: attributed, but left at `id == -1` and skipped | Oracle: `minimal`, `minimal_field`, `merge_number`, `merge_nil_check`, `merge_both_arms`, `elseif_number`, `hp_two_ifs`, `or_chain`, `cond_call_arg`, `fallback_user`, `and_two_locals`, and 4 slots in `effectset_stale`. Kenta: **11 of 12** (all except CF-1) |
| (b) | `_recheck_unsafe_cases`' dirty callback (`:166-184`), run per block (`:189-190`) | Anything outside the one block it re-collects | `minimal`, once (a) is closed: `refs=1`, re-inlined |
| (c) | `unwarper.py:2521` (`_cleanup_ast` → `eliminate_temporary(blocks[0], False)`), over already-structured code | A read after a nested block closes. The collector models only a flat, warped block list: `leave_block` clears `known_slots` and never restores `state.block`, and `_find_slot_assignments` follows warp edges only. The read gets `reaching=[]`, and `visit_identifier`'s `if assignments:` (`:858`) drops it **silently** (not even as ambiguous) | `hp_ratio_elseif`, Kenta **CF-1**. With (a)+(b) closed, it also drops `elseif_number` and `merge_number` again |
| (d) | The same call on a sub-tree root. `unwarper.py:570` and `:612-616` are the same family, but eliminated nothing in any log here, the scratch-fixed runs included | Reads that lie outside the traversed root, which are never visited | **BW-3**, with (a)–(c) closed. The collection's root is a `Block` with no enclosing function, `slot2#155` has `refs=1`, and no other read of `slot2` is logged in it. (Inferred from that absence.) |

**The ledger's three leads:**
1. **`_fill_simple_refs:286`: confirmed, but not sufficient alone.** It is the first drop for
   the minimal repro and for 11 of Kenta's 12 instances, BW-3's first drop included.
   - Removing the skip changes nothing on the minimal repro, because site (b) re-inlines.
   - Reverting upstream's bcb0aff9 on its own changes nothing either, for the same reason.
2. **`del new_slots[idx]` in `enumerate` (`:175-177`): real code, but inert.** The filter
   leaked 0 slots in all 91 recheck blocks logged (Kenta unfixed 10, Kenta scratch-fixed
   27, oracle 54).
   - The function is still implicated, through its per-block scope: site (b).
3. **The collector: implicated, but not as hypothesised.** The "splits one live range into two
   ids" hypothesis is refuted, because the cross-block read *is* attributed to the right
   single definition. The collector is implicated in two other ways:
   - It discards `update_ids`, which makes a correct attribution ambiguous.
   - It drops unresolvable reads silently: site (c).

**The other `id == -1` discard,** `_fill_massive_refs._remove_invalid_references`
(`:219-229`, for massive and iterator slots), was logged too. It never fires on the minimal
repro, on Kenta, or on any oracle case.

### Upstream status

- **The vendored copy is unmodified upstream.** The vendored `slotworks.py` is byte-identical
  (ignoring CRs) to `gitlab.com/znixian/luajit-decompiler` master. That repository's last
  commit is 2020-05-09.
- **Issue #32 names this class.** "Rewrite the slotworks gathering to work across blocks"
  was opened by the maintainer on 2020-05-06 and is **still open**. Its example
  (`slot_block_gathering`) is exactly this shape: a value used once, then used again inside
  an `if` block, is eliminated after the first use.
- **bcb0aff9 turned off the id update.** In "Don't update the id for 'warped' references"
  (DorentuZ, 2019-08-29), `visit_identifier` changed from passing `update_ids` to passing
  `False`. That is what leaves a single-definition cross-block read at `id == -1`.
  - Judging by the commit titles only (their diffs weren't read), the id and ambiguity
    machinery itself came in DorentuZ's August 2019 series (`0340231e`, `0604c5a5`).
  - Reverting bcb0aff9 would not fix the defect. With (b)–(d) still open, it doesn't even
    change the output. It also can't handle a read with two reaching definitions
    (`fallback_user`).
- **No fork has fixed it.** All three sites are unchanged in:
  - GitLab forks: igromanru (active 2026-03), 6123451 (2024) and q353b (2021), each identical
    to master; DorentuZ (last commit 2023-01-21, on the pre-`RefsProcessData` layout).
  - GitHub: `AzurLaneTools/ljd` (2021), which only renames nodes.

### Scratch experiment (evidence, not the fix)

Four switchable edits went into an instrumented copy, `tmp/ljd_p3`:
- **E1:** don't skip `id == -1` references at `:286`.
- **E1b:** pass `update_ids` at `:861`, i.e. revert bcb0aff9.
- **E2:** a deliberately over-broad probe, not a fix design. When the collector finds no
  reaching definition, it attaches the read to every definition of that slot the function
  has seen.
- **E3:** the recheck skips any slot whose original references span more than one block.

**Minimal repro.** E1, E1b, E2 or E3 alone leaves the text unchanged. **E1+E3 flips it:** the
output is `slot1 = slot0:Get()` / `if slot1 then` / `return slot1`, and `minimal_field` flips
the same way. E1b+E3 flips it too.

**`run_oracle.py --expect fixed`** (rows that miss `expect_fixed`, out of 29 today):

| Edits | Missing | What is left |
|---|---|---|
| none | 29 | — |
| E1+E3, or E1b+E3 | 13 | `elseif_number`, `merge_number`, `hp_ratio_elseif`, and the rest of `bwait` and `fallback`: sites (c) and (d) |
| **E1+E2+E3** | **1** | `bwait/nolm-near`: BW-3, site (d) |
| E1b+E2+E3 | 2 | the above, plus `fallback/target` (a read with two reaching definitions) |

No control row changed under any combination, in either behaviour or printed assignment count.
No `-g` row and no deepcopy row changed either. Text counts grew only in the defect functions.

**Kenta** (`ljdiff.py --diff --seeds 128`, June original vs round trip). The types oracle was
unchanged and `types.tsv` untouched.

| | dropped-assignment | other-ljd | reorder | pass | static candidates |
|---|---|---|---|---|---|
| unfixed | 406 | 60 | 138 | 1060 | 18 |
| E1+E2+E3 | **0** | 33 | 132 | 1499 | **0** |

- **Text.** Exactly 11 assignments are restored: BW-1, BW-2, BW-4, DS-1–3, FI-1, FI-2, EM-1,
  EM-2 and CF-1.
  - Each is the statement text that `fixes/RAGE_DRYADKENTA.json` restores, placed in the
    right function immediately before the condition it had been folded into. This was checked
    by script, not by name, and none of the 11 is in the unfixed text.
  - Nothing outside the five known functions changed.
  - BW-2's restoration also turns `elseif <call> ~= nil` into `else slot5 = <call>; if slot5
    ~= nil`, which is the correct structure.
  - **BW-3 is not restored.** `slot5:GetPos().y = …` remains, by design of the experiment.
- **Verdicts.** The five functions pass on every seed except `ATTACK_B_WAIT_FRAME_MOVE`, which
  is `other-ljd` on 4 of 128 seeds. That is BW-3's stale `pos`, matching the calibration's
  "BW-3, 4/128".
  - Unchanged from unfixed: `ATTACK_B_READY_STATE_START` (BR-O) and FIRE's `reorder` (FI-O,
    63 of 128). These are the known `order` class, and these edits don't touch them.
- **So the scratch edits restore 11 of 12, not all 12.** A real fix also has to close
  site (d).

**How to reproduce.** `tmp/` is scratch and gitignored, so this may be gone later.
- `tmp/ljd_p3/` is the instrumented copy, with its log helper in `ljd/ast/p3log.py`.
  - `P3_FIX=e1,e1b,e2,e3` selects the edits. With none set, its output is byte-identical to
    the vendored copy (checked on the minimal repro and on Kenta).
  - `LJD_P3_LOG=<file>` writes one line per collection, reference, decision and elimination,
    tagged with its caller.
- `python tmp/p3_run.py tmp/ljd_p3 oracle|ljdiff <args>` runs either tracked script against
  the copy. It overrides only the script's module-level `LJD`; the scripts are not edited.
- Summaries: `tmp/p3_summarize.py`, `p3_folds.py` and `p3_cmp.py`.

## Not done in this pass

- Nothing in the table above has been fixed except `RAGE_DRYADKENTA.src.lua` (which is why
  it no longer appears in the table).
- The root cause inside `elstools/vendor/ljd` (`ljd/ast/slotworks.py`) is now identified.
  See "Root cause" above: one defect, four sites. It is still unfixed. The fix is Phase 4 of
  `RANOX_LUA_DECOMPILE_FIX_PLAN.md`, and a wrong fix there risks decompiles that look even
  more plausible while still being wrong.
- The lint has not been wired into the decompile workflow (`ljd_jun2017.py` /
  `elstools decompile-batch`) as an automatic guard — right now it's a standalone script you
  have to remember to run.

See also: `MODS.md`'s Dryad Kenta entry (the full fix, as a worked example), and
`RANOX_PORT_PLAN.md`'s Phase 4 section (carries a correction note pointing back here, since
its original write-up reasoned about one instance of this bug and got it half right).
