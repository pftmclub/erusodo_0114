# Ranox Lua decompile bug: dropped assignments become stale globals

Tracking document for a decompiler defect found 2026-09-27 while fixing Dryad Kenta's
"jumps off the map, never comes back" softlock. This is a **known-issues / verdict-summary
document**, not a phase plan — the investigation, the fix, and the corpus sweeps that produced
the numbers below all live in `RANOX_LUA_DECOMPILE_FIX_PLAN.md`; that plan is the source of
record for methodology, and this document is the outward-facing summary.

**Status as of Phase 9 (2026-10-01): fixed, shipped as the default, swept across the whole
June corpus.** `june2017_re/ljd_jun2017.py` installs the fix (`june2017_re/ljd_fixed.py`) by
default now; `--unfixed` opts out. All 62 ported files carrying the bug have been repaired
(Phase 6a–6c) and are believed correct pending the user's recompile/repack/play-test pass. See
"Scope: every ported file with this pattern" below for the real, bytecode-verified numbers,
which supersede the original lint-based triage this document shipped with.

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

**`march2014/update/data037/RAGE_DRYADKENTA.src.lua`** — 6 instances by the original comment
count (**corrected by the plan's Phase 2b calibration: 12 dropped assignments across the same 5
functions** — the "6" undercounts by treating 4 comments that each cover two separate writes as
one instance apiece; see "Calibration" in `RANOX_LUA_DECOMPILE_FIX_PLAN.md` for the per-write
table), 2 of which independently caused the reported softlock (Attack B's off-map "jump" never
computing a valid return position; the 60%/20% HP dash's target position left unset, so the dash
never ends). Full writeup, per-function detail, and the exact fix are in `MODS.md`'s "Dryad
Kenta (Burned Forest boss)" entry — confirmed both against the real June2017 bytecode and,
independently, against the raw disassembly of the *currently deployed* `data037.kom`'s own
compiled `RAGE_DRYADKENTA.lua`. Phase 6c's later re-check confirmed 0 residual
`dropped-assignment`. Still needs recompiling and repacking into `data037.kom` (the user's step)
and an in-game play-test before it can be called done.

## Detecting more instances

**The authoritative check is now bytecode-level, not the lint.** `june2017_re/ljdiff` runs the
real June bytecode and a decompile-recompile round trip side by side and diffs their runtime
behaviour; `june2017_re/ljd_jun2017.py --check FILE.lua` is the one-command entry point (learn +
trace + diff against FILE's own June original, 32 seeds by default, fails on any `clobber`
verdict or any `dropped-assignment` read not covered by the same deepcopy-hoist exemption the
Phase 5/7/8 aggregators use — see "Residual ljd defects" below — so `EffectSet.lua`/
`DamageEffect*.lua` correctly pass despite carrying that separate, still-open defect). `regen.sh`
runs it automatically on every file in `june_lua/`. It does **not** replace a full corpus sweep:
it only sees what the seeds actually executed, not the coverage-independent static must-define
pass the sweeps also run.

`june2017_re/lint_decompile_slots.py` flags any `slotN` read in a function before that same
function ever assigns it — the signature this bug leaves behind, since a dropped assignment
is otherwise invisible on a read-through (the printed Lua still parses and reads as
plausible). It's a **heuristic, regex-based scan, not a bytecode-level check** — still useful
as a fast pre-check (no bytecode/luajit round trip needed), but every hit needs verifying
against `--check` or the sweep tooling before being trusted as a real defect. It has already
had one round of false-positive fixing (`for slotN = a, b do` and `for slotN in ... do` declare
their own loop variable; the first version of this scanner didn't know that and over-reported).

**Calibrated precision and recall** (`RANOX_LUA_DECOMPILE_FIX_PLAN.md`'s "Calibration", Phase
2b — unit is a unique (line, slot) site on the same text):

| Text | True sites | Lint hits | TP / FP / FN | Precision | Recall |
|---|---:|---:|---|---:|---:|
| Kenta round trip | 15 | 11 | 11 / 0 / 4 | 1.00 | 0.73 |
| Kenta pre-fix port | 15 | 13 | 13 / 0 / 2 | 1.00 | 0.87 |
| MapData round trip | 1 | 1 | 1 / 0 / 0 | 1.00 | 1.00 |

A lint hit has never been a false positive in this corpus (precision 1.00 across all three
texts), so a hit is trustworthy without further checking. **A lint zero is not** — it misses
roughly a quarter of sites and a third of instances, specifically every "written on one arm,
read after the merge" shape and everything with no unwritten read at all (a stale value written
earlier in the same call, which only the dynamic harness can see). Use the lint to triage
quickly; use `--check` or the sweep tooling to clear a file.

```sh
python june2017_re/lint_decompile_slots.py <file.src.lua> [more.src.lua ...]
python june2017_re/ljd_jun2017.py --check <file.lua>   # bytecode-level; a COPY of the June
                                                        # original with its .std.luac sibling --
                                                        # never the pristine 14JUN17\ tree itself,
                                                        # decompile() writes next to its input
```

**How to verify a candidate** (same process used for all 6 confirmed `RAGE_DRYADKENTA`
instances; steps 1-3 predate `--check` and stay useful for reading *why*, but
`ljd_jun2017.py --check <june original>.lua` now gives a pass/fail answer in one command --
on a **copy** of the June original plus its `.std.luac`, same as step 2 below, never the
pristine `14JUN17\` tree directly):

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
   no-`local` bare-global convention. **Don't introduce `local`** — this is a decision, not the
   safety argument an earlier pass of this doc made. That argument ("safe *within* one
   function's own sequential statements, since nothing else runs between them") is **refuted**:
   `RANOX_LUA_DECOMPILE_FIX_PLAN.md`'s Phase 1 oracle found a same-file recursive call clobbering
   a shared global (`deepcopy_global`'s `global/nested` case), and Phase 5's real-corpus sweep
   found 3 live instances across 276 port files (`SECRET_MAGI_BOSS.src.lua` ×2,
   `SMITH_REGIEON_POINT.src.lua` ×1 — the same one already fixed in Kenta's sibling function).
   The user chose to accept that residual risk and fix known instances case by case rather than
   adopt `local` everywhere; see the plan's "Phase 5 result" and "Decisions taken" for the
   evidence and the full record. Verify with the lint (should drop to zero for that function),
   `elstools compile-batch -v mar-2014` on a **scratch copy** (the compiler overwrites its
   input in place), and `ljd_jun2017.py --check` on the June original (should come back clean).
5. Note the `SERV_IRUHADEV_RANOX_PORT (<date>): ...` comment convention already used
   throughout `RAGE_DRYADKENTA.src.lua` for these fixes — cite the bytecode evidence, not
   just "this looked wrong."

## Scope: every ported file with this pattern

**Superseded the original lint-based triage below on 2026-09-28/2026-10-01.** The heuristic
table this section originally carried (61 files, 774 raw lint hits, "kind" guessed from
filename) is gone. `RANOX_LUA_DECOMPILE_FIX_PLAN.md`'s Phase 5 mapped and bytecode-diffed all
277 `march2014/**/*.src.lua` files against their June originals; Phase 6a–6c then repaired
every real instance found. What follows is that real, per-function, bytecode-verified count —
one row per repaired function, `scope=ported` / `verdict=repaired` rows in
`june2017_re/decompile_verdicts.tsv` — not a lint guess.

**62 files, 123 functions, all repaired (Phase 6a–6c, committed).** Recompile + repack into the
listed `.kom` and an in-game play-test are still the user's step (see `MODS.md`'s Dryad Kenta
entry for the worked example of that last mile). 61 of the 62 files are in
`march2014/update/data037/` (the Ranox monster/boss AI scripts) plus `data036/MapData.src.lua`
— the same shape the original heuristic scope predicted (branching control flow feeding a later
shared statement, which per-frame AI state machines are full of and plain data tables mostly
aren't), now with an exact count instead of a guess.

| Functions repaired | File (relative to `march2014/`) |
|---:|---|
| 12 | `update/data037/SECRET_MAGI_BOSS.src.lua` |
| 9 | `update/data037/GEYSER_MULSAYRENA.src.lua` |
| 7 | `update/data037/SECRET_SCENE_DUMMY.src.lua` |
| 7 | `update/data037/SMITH_SCIENCE.src.lua` |
| 5 | `update/data037/FIRE_DEVIL_IGNIA.src.lua` |
| 4 | `update/data037/LOW_SMITH_SCIENCE_FD.src.lua` |
| 4 | `update/data037/SAMETEAM_IGNIA.src.lua` |
| 3 | `update/data037/FIRE_DEVIL_CHAIN_BALL.src.lua` |
| 3 | `update/data037/FIRE_DEVIL_SKA.src.lua` |
| 3 | `update/data037/GEYSER_MULLOCK.src.lua` |
| 3 | `update/data037/TRAP_FIRE_BIG.src.lua` |
| 2 | `update/data037/EVENT_FINALHALL_BOSS2.src.lua` |
| 2 | `update/data037/FIRE_DEVIL_BERSERKER_BOSS.src.lua` |
| 2 | `update/data037/FIRE_DEVIL_BERSERKER_BOSS_FINALHALL.src.lua` |
| 2 | `update/data037/FIRE_DEVIL_IGNIA_BOSS_FINALHALL.src.lua` |
| 2 | `update/data037/GEYSER_MULLOCK_CALL.src.lua` |
| 2 | `update/data037/GEYSER_MULSAYRENA_CALL.src.lua` |
| 2 | `update/data037/LOW_GEYSER_MULLOCK_CALL_FD.src.lua` |
| 2 | `update/data037/LOW_GEYSER_MULLOCK_FD.src.lua` |
| 2 | `update/data037/LOW_MOUNT_BERSERKER_BOSS.src.lua` |
| 2 | `update/data037/MOUNT_FIREBIRD_FIRETEMPLE.src.lua` |
| 2 | `update/data037/SMITH_REGIEON_POINT.src.lua` |
| 2 | `update/data037/SMITH_RUNNER.src.lua` |
| 1 | `data036/MapData.src.lua` |
| 1 | `update/data037/EVENT_FINALHALL_BOSS.src.lua` |
| 1 | `update/data037/EVENT_GATEWAY_BOSS.src.lua` |
| 1 | `update/data037/EVENT_GAYSER_BOSS.src.lua` |
| 1 | `update/data037/FIRETEMPLE_REGIEON_POINT.src.lua` |
| 1 | `update/data037/FIRE_DEVIL_BERSERKER.src.lua` |
| 1 | `update/data037/FIRE_DEVIL_BERSERKER_FIRETEMPLE.src.lua` |
| 1 | `update/data037/FIRE_DEVIL_BERSERKER_INCHANT.src.lua` |
| 1 | `update/data037/FIRE_DEVIL_SNIKER.src.lua` |
| 1 | `update/data037/FIRE_DEVIL_SNIKER_INCHANT.src.lua` |
| 1 | `update/data037/GEYSER_ALUMINUM.src.lua` |
| 1 | `update/data037/GEYSER_MULMAI.src.lua` |
| 1 | `update/data037/GEYSER_SCIENCE.src.lua` |
| 1 | `update/data037/LOW_FINALHALL_BERSERKER_FD.src.lua` |
| 1 | `update/data037/LOW_FINALHALL_FIREBIRD_FD.src.lua` |
| 1 | `update/data037/LOW_FINALHALL_SNIKER_FD.src.lua` |
| 1 | `update/data037/LOW_GEYSER_ALUMINUM_FD_BOSS.src.lua` |
| 1 | `update/data037/LOW_GEYSER_MULMAI_FD.src.lua` |
| 1 | `update/data037/LOW_GEYSER_SCIENCE_FD.src.lua` |
| 1 | `update/data037/LOW_MOUNT_BERSERKER_FD.src.lua` |
| 1 | `update/data037/LOW_MOUNT_FIREBIRD_FD.src.lua` |
| 1 | `update/data037/LOW_SMITH_ALUMINUM_FD_BOSS.src.lua` |
| 1 | `update/data037/MOUNT_FIREBIRD.src.lua` |
| 1 | `update/data037/MOUNT_POINT_STAGE_BOSS.src.lua` |
| 1 | `update/data037/SECRET_BERSERKER.src.lua` |
| 1 | `update/data037/SECRET_CRUEL_BERSERKER.src.lua` |
| 1 | `update/data037/SECRET_CRUEL_SNIKER.src.lua` |
| 1 | `update/data037/SECRET_FIREBIRD.src.lua` |
| 1 | `update/data037/SECRET_MAGI_SUMMON.src.lua` |
| 1 | `update/data037/SECRET_SCENE_1_1.src.lua` |
| 1 | `update/data037/SECRET_SNIKER.src.lua` |
| 1 | `update/data037/SECRET_STAGE1_CHAIN1.src.lua` |
| 1 | `update/data037/SECRET_STAGE1_CHAIN2.src.lua` |
| 1 | `update/data037/SECRET_SUMMON_BERSERKER.src.lua` |
| 1 | `update/data037/SECRET_SUMMON_FIREBIRD.src.lua` |
| 1 | `update/data037/SECRET_SUMMON_SNIKER.src.lua` |
| 1 | `update/data037/SMITH_ALUMINUM.src.lua` |
| 1 | `update/data037/SMITH_CART.src.lua` |
| 1 | `update/data037/SMITH_STAIN.src.lua` |

`RAGE_DRYADKENTA.src.lua` (Kenta, the file this bug was originally found on) is not in this
table: it was fixed first, by hand, before the sweep tooling existed (see "Confirmed and fixed"
above), and Phase 6c's re-check confirmed 0 residual `dropped-assignment` against the fixed
decompile — it is done, not merely absent from the count.

## Root cause

Identified 2026-09-28 in Phase 3 of `RANOX_LUA_DECOMPILE_FIX_PLAN.md`. The fix shipped the
same day in Phase 4, opt-in: `june2017_re/ljd_fixed.py` (`ljd_jun2017.py --fixed`).
- It is one reachability check, made where a write is actually deleted. It closes all four
  sites below.
- It also closes a fifth, found in review: `_eliminate_mass_assignments`, after
  `_remove_invalid_references` drops `id == -1` reads.
- The plan's "Phase 4 result" has the design and the acceptance numbers.
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

## Residual ljd defects (Phase 7, data037_kom corpus sweep)

Ran 2026-09-29/30. `june2017_re/phase7_worker.py` + `phase7_aggregate.py` compared every
`data037_kom` June original (3,105 files, the AI monster/boss/dungeon scripts future ports
draw from) against its **fixed** round trip only — no `march2014/` port side, since most of
this folder has never been ported. Fixed decompiles are persisted at
`june_src_fixed/data037_kom/<name>.src.lua`; raw verdicts are `scope=corpus` rows in
`june2017_re/decompile_verdicts.tsv` (31,347 rows total, alongside Phase 5's 666 `scope=ported`
rows, which this sweep did not touch).

**Zero unexplained `dropped-assignment`. The Phase 4 fix holds across the whole folder** —
confirmed by `phase7_aggregate.py`'s exit code (0) both dynamically (32 seeds) and via the
coverage-independent static must-define pass, the same two-check design Phase 5 used. This is
not a bare "no regressions found": two real false positives surfaced and were run down rather
than dismissed, both the same *already-catalogued* deepcopy-hoist class (`main.py`'s
constructor-hoist logic — a different code path from Phase 4's `slotworks.py` fix) manifesting
in shapes Phase 5's smaller ported-file census hadn't happened to see:
- **`ipairs(...)` and numeric `for slotN = a, b do` loop headers**, not only `for … in
  pairs(...)`. Found on `Treasure_Summon{1,2,3}.lua`'s `TREASURE_SUMMONn_CREATE_CHEST`: ljd
  prints `slot7 = { [slot11 - slot3] = slot11 }` immediately followed by the loop that actually
  computes it, `for slot11 = slot3, slot4 do end` (empty — its real body is the constructor line
  hoisted above it).
- **A hoisted constructor entry reading more than one register on its line**, where only some of
  those registers are loop variables. Found on `SECRET_MAD_BEHEMOTHSOUL.lua`'s
  `SECRET_MAD_BEHEMOTH_SOUL_MAGIC_ATTACK_C_FRAME_MOVE`: `[slot10] = slot1:GetDist_LUA(slot11:GetPos(), slot6)`
  — `slot10`/`slot11` are recognized loop variables, but `slot6` is a plain, later-assigned local
  read in that same hoisted expression. Confirmed against the orig trace: `slot6`'s real write
  (`slot2:GetPos()`) executes early in the true bytecode order, not where ljd printed it.

Both fixes are in `june2017_re/phase5_aggregate.py`'s `deepcopy_exempt_at_line()` (shared by the
Phase 5 and Phase 7 aggregators), re-verified against the full sweep afterward (exit 0, no
unexplained rows left). **Not** counted as Phase 4 regressions — they're the same pre-existing,
separate ljd defect the "ljd caveat" note above already names, just with a classifier that
hadn't seen every shape it takes yet.

**Verdict breakdown, 31,347 `scope=corpus` rows** (one per file+function, plus one `<main>` row
per file):

| Verdict | Rows | Meaning |
|---|---:|---|
| `clean` | 28,374 | matches June exactly |
| `reorder` | 2,140 | same events as a multiset, same outcome, different order — see below |
| `other-ljd` | 706 | events differ (usually a reorder-driven early stop) — see below |
| `clobber` | 61 | a nested same-file call frame writes a register the caller was still relying on |
| `other-ljd (deepcopy, exempt)` | 55 | the separate, already-catalogued deepcopy-hoist defect above |
| `upvalue` / `TIMEOUT` | 4 each | a captured local prints as `uvN`, not a `slotN` global / the fixed hook ran too long — see below |
| `STEP_ERROR` | 3 | the fixed hook itself failed — see below |

**1,989 of 3,105 files are byte-for-behavior clean** (every function and `<main>` pass);
1,109 more have at least one `reorder`/`other-ljd`/`clobber`/`upvalue` row (harmless-in-engine
per Phase 4's own read of the equivalent classes on Kenta) but no dropped-assignment; 7 files
produced no verdict at all (below — 2 more were retried and recovered, see that table). **Zero
`unreached-by-harness` functions** (every function the June trace calls, the fixed round trip's
own driver also called) and **zero `unverifiable-truncated`** (Phase 0 already proved this
corpus has no truncated originals — this sweep didn't need to re-check it, and didn't find any
either).

**The dominant residual shape, by far, is call-argument evaluation order — the same `order`
class Phase 4's calibration already named (BR-O/FI-O on Kenta) and left untouched on purpose.**
Clustering every `reorder`/`other-ljd`/`clobber`/`upvalue` row by (verdict, labels, first
differing event, with numbers/mock strings/register names masked) gives 293 distinct textual
shapes, but they collapse into a handful of real causes:

| Cause | Rows | Example |
|---|---:|---|
| Pure reorder: two argument-evaluating calls (or a call and a field read) swap position, same values, same outcome | 2,001 | `a2:GetPos()` and `a2:GetBonePos_LUA(...)` swap; `GetTimerElapsedTime`/`GetVector_LUA` swap; `IsRightTarget_LUA`'s own arg-evaluation order (`GetIsRight()` vs `GetPos()`) swaps |
| A reorder shifts whether/when the function hits an early `return` (a reordered condition takes a different branch shape but the same final `outcome` bucket) | 399 | e.g. `OUTCOME RET` appearing one or more events earlier or later once two calls swap position |
| A reorder shifts WHERE the call tree hits a genuine Lua error (`attempt to index a nil value`, from a mock returning `nil`) — same eventual error, different event count before it | 341 | `SPRIGGAN_BIG_ATTACK_D_STATE_START`: orig evaluates 4 args before erroring on the 3rd's result; side (reordered) errors one call sooner |
| A reorder shifts where the harness's own runaway-call cutoff (`BUDGET`, 2,000,000 VM instructions) lands inside a long loop — a harness artifact, not a decompile defect | 170 | `SECRET_BUFF_DUMMY_WAIT_STATE_START`: an 887-iteration buff-removal loop crosses the instruction budget on one side one iteration earlier/later than the other |
| `clobber`: a nested same-file call writes a register the caller still needs | 61 (21 files) | `SECRET_MAGI_BOSS.lua`, `SMITH_REGIEON_POINT.lua` — the same two instances Phase 5's ported-file clobber census already found and Phase 4 already named; also `RAID_ELTRION_MK2_PHASE{1,2}.lua`, `KARIS_JIN.lua`, `Ran.lua`, `TARGETING_HELLPUTT.lua`, `HENIR_RAN.LUA`, `SA_LDN_GARDEN_OF_HAURES.lua`, `SECRET_EXTRA_MAGI_BOSS.lua` and their `_HENIR` twins — not individually verified beyond the two already-known instances; flagged as a lead, not confirmed |
| `upvalue`: a captured local prints as `uvN` | 4 (2 functions, 4 file variants — `WALLY_9TH_ARMOR*`'s `ATTACK_C_FRAME_MOVE`/`Wally9th_Armor*`'s `MISSILE_FRAME_MOVE`) | expected ljd behavior for a genuinely captured closure variable, not a defect |

None of this is fixed here, per the plan. The reorder/error-position/budget-position classes are
inherited, harmless-in-engine defects (same call, same values, same final outcome except where a
mock's random `nil` happens to matter) — exactly the `order` class Phase 4's own Kenta
calibration (BR-O, FI-O) already decided not to chase. The clobber rows are a lead for whoever
next ports one of those 12 files, not a finding this plan resolves.

**9 files initially failed; 2 were retried and recovered (a transient artifact, not a real
defect), leaving 7 with a genuine, reproducible tooling limitation in the fixed hook itself —
not a correctness question:**

First, the retry that separated real failures from noise: `Low_Rash_Bugi_Trock_FD.lua` and
`Defence_Soldier_Benders.lua` both initially failed with `luajit.exe: not enough memory`. Both
were an orphaned `ljd_fixed.py` subprocess (left over from a `Spriggan_Small.lua` timeout kill,
found still running and consuming 11+ GB, starving the machine mid-sweep) — not a property of
these files. Once that process was killed and the two files re-run in isolation, both decompiled
and traced clean (only harmless `reorder`/`other-ljd` rows, zero `dropped-assignment`); their
real verdicts are folded into the table and counts above, not listed separately here.

**7 files still have no verdict, confirmed on a clean retry (900s budget, no memory
contention) — a real limitation of the fixed hook, distinct from the correctness fix itself:**

| File | Failure | Cause |
|---|---|---|
| `Erda_Portal.lua`, `Henir_Three_Way.lua`, `Low_Miner_Big_Defect_FD.lua` | `STEP_ERROR`: `RecursionError: maximum recursion depth exceeded` | the fixed hook's per-elimination flow rebuild (see "Four refinements the checks forced" above) uses `copy.deepcopy` on the function's AST; on some deeply-nested shapes Python's C-stack-bounded recursion limit is hit. Raising `sys.setrecursionlimit` was tried and does **not** help — it only trades a clean `RecursionError` for an unbounded `MemoryError` after exhausting available RAM (confirmed: one such attempt consumed 11+ GB before being killed) |
| `Spriggan_Small.lua`, `SPRIGGAN_SMALL_HENIR.LUA`, `Raid_Red_Dragon_Phase1.lua`, `ELYSION_SOLESS.lua` | `TIMEOUT` (`TIMEOUT-INTERNAL` on the retry, same underlying cause) | the fixed hook's decompile step alone exceeds `ljdiff.py`'s own hard-coded 600s internal subprocess timeout (`decompile()`'s `run(..., 600)`, independent of `corpus_run.py`'s `--budget` — confirmed by retrying at `--budget 900` and still hitting the same 600s internal cap) on these files. Not obviously size-correlated with the rest of the corpus — `Raid_Red_Dragon_Phase1.lua` (101 KB) and `ELYSION_SOLESS.lua` (105 KB) are unremarkable in size next to the 14 files over 1 MB Phase 0 already sails through; this looks like a specific AST-nesting shape, same family as the `RecursionError` cases above, not a simple size problem |

All 7 are genuinely new June originals (none has a `march2014/` port yet, confirmed against
Phase 5's 276-file map), so nothing currently shipped depends on these decompiling. If and when
one is ported, its fixed decompile will need either a bigger `ljdiff.py` internal timeout (for
the 4 `TIMEOUT` files) or a real fix to the flow-rebuild's recursion depth (for the 3
`RecursionError` files) — both out of scope here, and worth a follow-up plan item if the user
wants these unblocked before then.

## Residual ljd defects (Phase 8, the remaining ~4,200 files)

Ran 2026-09-30. `june2017_re/phase8_worker.py` + `phase8_aggregate.py` compared every June
original **outside** `data037_kom` (4,223 files across 20 folders — line maps, dialog/UI
tables, item/skill/effect data, event scripts) against its **fixed** round trip, the same
no-port-side comparison Phase 7 used. Fixed decompiles are persisted at
`june_src_fixed/<dir>/<name>.src.lua`; raw verdicts are `scope=corpus` rows in
`june2017_re/decompile_verdicts.tsv`, which together with Phase 7's rows now cover the **entire
7,328-file corpus** (41,809 `scope=corpus` rows total; Phase 5's 666 `scope=ported` rows
untouched).

**Zero unexplained `dropped-assignment` across all 4,223 files — the Phase 4 fix holds for the
whole non-`data037_kom` corpus too**, confirmed the same two-check way (dynamic + static)
`phase8_aggregate.py` inherits from Phase 5/7.

**A main-chunk-only fast path, confirmed safe rather than assumed.** 3,715 of the 4,223 files
define zero nested function prototypes (`decompile_corpus.tsv`'s `protos_complete == 1`), split
out via `june2017_re/phase8_build_targets.py`. For those, `ljdiff.lua`'s `entry_points()` — which
only discovers functions reachable from main-chunk globals — is provably empty, so the
per-function/per-seed driver is already a no-op with or without `--fast`; the ONLY thing
`--fast` skips is `learn()`'s extra main-chunk execution(s) and its `static_learn()` type-mining
pass. Before sweeping, this was checked on 20 files spread across 8 folders (sizes 1 KB–5.2 MB):
fast and full produced byte-identical JSON on 19/19 that completed (the 20th hit the same
compile-batch defect described below, on both sides identically — not a fast/full disagreement).
Critically, **none of the 20 samples exercised `learn()`'s type-mining path at all** — traced to
a structural reason, not a sampling gap: `static_learn()` only fires on a `CALL` immediately
followed by a comparison against a constant, and a direct grep of all 19 successfully-decompiled
samples found **zero `==`/`~=` occurrences anywhere** in any of them. Fast-path files are pure
declarative data (no `if`, no comparisons at all), so there is structurally nothing for
`learn()` to teach on this population regardless of sample size. The fast batch (3,715 files)
then ran 8-way parallel in 614s (0.17s/file avg) — confirmed safe because `save_types()` is only
ever called from inside `learn()`, which `--fast` skips, so a fast-path worker never writes
`june2017_re/ljdiff/types.tsv`, only reads it.

**Verdict breakdown, Phase 8's 4,223 files** (one row per file+function, plus one `<main>` row
per file):

| Verdict | Rows | Meaning |
|---|---:|---|
| `clean` | 10,013 | matches June exactly |
| `reorder` | 272 | same events as a multiset, same outcome, different order |
| `other-ljd` | 147 | events differ, usually a reorder-driven early stop |
| `upvalue` | 19 | a captured local prints as `uvN`, not a `slotN` global |
| `clobber` | 2 | a nested same-file call frame writes a register the caller was still relying on |
| `other-ljd (deepcopy, exempt)` | 1 | the pre-existing deepcopy-hoist class, see Phase 7 above |
| `STEP_ERROR` / `TIMEOUT` | 5 / 3 | see below |

3,958 of 4,223 files are byte-for-behavior clean; 257 more have at least one harmless
`reorder`/`other-ljd`/`clobber`/`upvalue` row but no dropped-assignment; 8 produced no verdict at
all (below). Clustering every non-clean row the same way Phase 7 did (293→a handful of causes
there; 86 shapes here, same dominant pattern) confirms the same story: pure call-argument
reorder, a reorder shifting an early return/error/budget-cutoff position, and two more `clobber`
instances (`data034_kom/DLG_GAME_SKILL_SLOT_NEW*.lua`-style skill-slot files) — nothing new in
kind, just more instances of what Phase 7 already named.

**8 files got no verdict — three distinct tooling-limitation shapes, two of them new (not seen
in Phase 7's data037_kom sweep):**

| File(s) | Failure | Cause |
|---|---|---|
| `SQUARE_Unit.lua`, `PET_Sadfox_Biho_1.lua`, `PET_Sadfox_Biho_2.lua` | `TIMEOUT` | same class as Phase 7's 4 `TIMEOUT` files: the fixed hook's decompile step exceeds `ljdiff.py`'s internal cap. One `TIMEOUT` in this same batch, `GameEditCommandList.lua` (228 functions), *did* recover on a retry at `--decompile-timeout 900`/`--budget 900` (474s of that was the decompile step alone) — these 3 did not, confirmed on the same retry |
| `PET_Sadfox_Biho_3.lua` | `STEP_ERROR`: `MemoryError` in ljd itself (not a Python recursion limit) | new shape, not one of Phase 7's 3 `RecursionError` files — the fixed hook's flow rebuild runs out of memory rather than hitting a recursion cap on this file's AST shape |
| `DamageEffect_Dungeon.lua`, `Rose_StormTrooper.lua` | `STEP_ERROR`: `compile-batch` syntax error, `'}' expected ... near '('`/`'slot3'` | **new class**: ljd prints an anonymous function used as an immediately-invoked expression without the parentheses Lua's grammar requires around it — `function (slot0) ... end(ARGS)` instead of `(function (slot0) ... end)(ARGS)`. Both examples are the deepcopy-hoist IIFE itself (`slot1 = function(slot0) ... end(orig_table)`, the same `main.py` constructor-hoist defect as the "ljd caveat"), so this looks like the same root cause growing an extra symptom under a shape Phase 7's `data037_kom` folder didn't happen to contain: not just wrong *values* (the hoist) but invalid *syntax* around the hoisted IIFE's call |
| `NewSkillTempletVer2.lua`, `NewSkillTempletVer2_LA.lua` | `STEP_ERROR`: `compile-batch` syntax error, `'}' expected ... near ']'` | **new class**: ljd prints a multi-line string as a plain `[[...]]` long-bracket literal without checking whether the string's own content contains the substring `]]`. Confirmed by reading the produced text directly: a skill-description string ending in combo notation (`...XX↓X[X]]]`) closes the long bracket three characters early, corrupting the enclosing table constructor. Elsword's skill/combo text routinely uses bracket notation (`[X]`), so this is plausible in any file with translated skill descriptions, not just these two — both are `NewSkillTempletVer2*` variants, i.e. the same skill-description table under two locales |

None of these 8 has a `march2014/` port yet (checked against every `.src.lua` filename stem
under `march2014/`), so nothing currently shipped depends on any of them decompiling. The two new
classes are both **decompile-stage failures, not semantic ones** — they produce no comparable
output at all rather than a wrong one, so they can't hide a `dropped-assignment`-shaped defect;
if and when a file needing either shape is ported, the fix is in ljd's printer (add the missing
parens around a called function literal; pick a `]=*]` bracket level that doesn't collide with
the string's own content), not in the slotworks elimination fix this plan already made.

**Corpus-wide totals (all 7,328 files, Phase 7 + Phase 8 combined):**

| | Count |
|---|---:|
| Files with a verdict | 7,313 / 7,328 |
| Fully clean files (every function + `<main>` clean) | 6,001 |
| Files with only harmless residuals (reorder/other-ljd/clobber/upvalue, no dropped-assignment) | 1,312 |
| Files with no verdict (tooling limitation, not a correctness question) | 15 (7 from Phase 7, 8 from Phase 8) |
| Files with an unexplained `dropped-assignment` (Phase 4 regression) | **0** |

## Status

**Everything this section used to list as outstanding is done, as of Phase 9
(`RANOX_LUA_DECOMPILE_FIX_PLAN.md`, 2026-10-01):**

- **The root cause inside `elstools/vendor/ljd` (`ljd/ast/slotworks.py`) is identified and
  fixed.** See "Root cause" above, one defect at four sites (a fifth found in review).
  - The fix is a run-time hook, `june2017_re/ljd_fixed.py`, and it is now the **default**:
    `june2017_re/ljd_jun2017.py` installs it unless `--unfixed` is passed. `regen.sh` uses the
    default.
  - `june2017_re/ljd_slotworks.patch` is the same change as a unified diff for the two vendored
    ljd copies. **Applied 2026-10-01** (the user's call): both `elstools/vendor/ljd` and the
    second copy `elstools.exe` runs now have the fix baked in statically, so
    `elstools.exe decompile-batch` gets it directly, not just `ljd_jun2017.py`. Both copies'
    `slotworks.py` are still `diff -rq` clean against each other after patching.
    `june2017_re/ljd_fixed.py`'s `install_fix()` recognizes an already-patched file (matching
    `ljd_slotworks.patch`'s own recorded post-patch SHA-256) and treats it as a no-op rather than
    refusing — it used to only recognize the pre-patch SHA, which broke outright the moment the
    patch was applied; caught and fixed in the same pass.
    **Consequence: `--unfixed` no longer means genuinely unfixed.** Since the fix is now in the
    vendored file itself, every path (`ljd_jun2017.py` with or without `--unfixed`, `ljd_fixed.py`
    directly, `elstools.exe`) produces fixed output. True pre-fix behavior would need a fresh,
    unpatched checkout of the two vendored `ljd` directories.
- **Every ported file has a verdict, and every real instance is repaired.** Phase 5 mapped and
  bytecode-diffed all 277 `march2014/**/*.src.lua` files; Phase 6a–6c repaired all 62 files /
  123 functions found (see "Scope" above) and the repairs are committed. Recompile, repack, and
  an in-game play-test remain the user's step.
- **The fix holds across the entire 7,328-file June corpus**, not just the ported subset —
  Phases 7 and 8 swept every other file (data037_kom's AI scripts, then everything else) and
  found zero unexplained `dropped-assignment`. See "Residual ljd defects" above for what's left
  (all harmless-in-engine order/reorder classes, a separate already-catalogued deepcopy-hoist
  defect, and a handful of tooling-limitation failures with no port depending on them).
- **The lint is not the only check anymore.** `ljd_jun2017.py --check FILE.lua` runs the
  bytecode-level harness in one command (see "Detecting more instances" above); `regen.sh` runs
  it automatically on every reference file it regenerates. The lint is still useful as a fast,
  no-bytecode pre-check, and its calibrated precision/recall are recorded above.

See also: `MODS.md`'s Dryad Kenta entry (the full fix, as a worked example) and its tooling
entry for this plan; `RANOX_LUA_DECOMPILE_FIX_PLAN.md` (the full investigation, every phase's
methodology and result); and `RANOX_PORT_PLAN.md`'s Phase 4 section (carries a correction note
pointing back here, since its original write-up reasoned about one instance of this bug and got
it half right).
