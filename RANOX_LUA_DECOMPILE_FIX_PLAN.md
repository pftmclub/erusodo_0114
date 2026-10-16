# Ranox Lua decompile fix: differential harness, root cause, corpus sweep

This plan finds and fixes the ljd defect described in `RANOX_LUA_DECOMPILE_BUG.md`.
- In that defect, ljd drops a real register assignment and inlines the computing call into
  the condition that reads it.
- The dropped `slotN` then becomes a stale `_env` global in these no-`local` ported files.
- The plan then checks the whole June2017 Lua corpus against the fixed decompiler.

This document is self-contained. To run a phase, paste the "Starting context" block below
together with that phase's section into a fresh conversation, then paste its **Kickoff
prompt**. Each phase is sized for one conversation. Run them in order, because each one
consumes the previous one's outputs.

**Why this exists.** The bug caused a real boss softlock: Dryad Kenta, fixed in `857dfd7`.
It is invisible on a read-through, because the printed Lua parses and reads as plausible.
The only detector so far is `june2017_re/lint_decompile_slots.py`, a regex heuristic that
has only been run on `march2014/`. Six Kenta instances were checked against bytecode by hand.
That doesn't scale to 7,328 June files. This plan builds two things:
1. A **semantic** detector: run the original bytecode and its decompile-recompile round trip
   side by side, and diff their behaviour.
2. A **general** fix: correct ljd's elimination pass, so that every future decompile comes
   out right.

**Decisions taken 2026-09-27 (the user's):**
- **Where the fix lives.** It is a tracked runtime hook in `june2017_re/`, the same approach
  `ljd_jun2017.py` already uses for the opcode table.
  - A `.patch` for the two vendored ljd copies is also produced. Applying it is optional and
    the user's call.
  - Third-party ljd is never edited in place.
- **`local slotN`.** This is **decided after the harness has produced evidence.** Phase 5
  gathers the evidence and asks. Until then, keep the existing no-`local` convention.
  - **Decided 2026-09-28 (Phase 5, the user's answer): keep bare globals.** Phase 5's clobber
    census found only 3 real nested-frame clobber sites across all 276 mapped port files (2 in
    `SECRET_MAGI_BOSS.src.lua`, 1 in `SMITH_REGIEON_POINT.src.lua` — the same one Phase 4
    already found), all same-file direct calls, 0 measured engine re-entry. Repairs restore
    dropped assignments only; known clobbers are fixed case by case (Phase 6). `local` was not
    adopted — see "Phase 5 result" for the full evidence and `RANOX_LUA_DECOMPILE_BUG.md` for
    the recorded rationale.
- **Scope.** All 7,328 June files, in stages:
  1. Kenta
  2. the ported `march2014/` files
  3. `data037_kom`
  4. everything else

---

## Model and effort per phase (for the user; do not paste)

The same policy as `JUNE2017_EFFECT_REBUILD_PLAN.md`'s table, applied here:
- Use Opus where a wrong call is likely and silent.
- Use Sonnet where the work is mechanical.
- Effort is always high. Max is only for getting unstuck. Haiku is not used for this plan.

| Phase | Kind | Model / effort |
|---|---|---|
| 0: corpus inventory, input integrity | Mechanical | Sonnet 5, high |
| 1: synthetic oracle | Identification (does a minimal case reproduce?) | **Opus, high** |
| 2a: harness runtime | Design-heavy build (mock semantics decide what counts as a divergence) | **Opus, high** |
| 2b: diff and calibration | Identification (is each divergence real?) | **Opus, high** |
| 3: root cause | Identification | **Opus, high**. Use max only if two code paths both explain the repro |
| 4: fix | Fix to third-party logic; a wrong fix looks *more* plausible | **Opus, high**, then Opus `/code-review` |
| 5: sweep, ported files | Mechanical + verdicts | Sonnet 5, high. Switch to Opus for any divergence the harness can't classify |
| 6a–6c: repair ported files | Triage (port edit vs bug) + byte edits | Sonnet 5, high, then Opus `/code-review` |
| 7, 8: corpus sweeps | Mechanical, long-running | Sonnet 5, high |
| 9: wire-in, docs, memory | Writing | Sonnet 5, high |

Rules carried over:
- **Split mixed phases within one session.** Do the identification step on Opus and write
  the verdicts to disk, then `/model` to Sonnet.
- **Stop a Sonnet session that is making its own identification call.** For example, if it
  starts deciding "this divergence is probably a mock artefact" without evidence.
- **Keep context small.** Paste only "Starting context" plus the phase section. Send broad
  searches to a subagent. Run the long sweeps in the background and read their TSVs rather
  than their stdout.

---

## Starting context (paste this with any phase)

- **Repo root:** `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk`
  (called `<Trunk>` below).
- **Branch:** `main`.
- **June2017 reference client:** `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\`.
  - Scripts sit in `dataNNN_kom\` / `dataiNNN_kom\` folders, next to their `.kom` archives.
  - **Never write into `14JUN17\`.** Copy files out first.
- **Scratch: `<Trunk>/tmp/`** (the user's choice, 2026-09-27).
  - It is already gitignored by `.gitignore`'s `/*` allowlist, so it needs no `.gitignore`
    edit. Create it if it is missing.
  - The path is about 122 characters, so it is also the MAX_PATH workaround. Python's raw
    `io.open` fails past 260 characters with a `FileNotFoundError` on a file that exists.
  - Keep filenames flat there. Put all scratch files there: scratch copies for
    `compile-batch`, `.std.luac` pairs, and harness traces.
  - Don't use `C:\tmp\` or the per-session scratchpad, which is itself close to MAX_PATH.
- **The June corpus** (counted 2026-09-27; re-verify, don't trust):
  - **7,328** `.lua` files across **21** `*_kom` folders: `data037_kom` 3,105, `data040_kom`
    1,562, `data034_kom` 1,137, `datai016_kom` 380, `data035_kom` 328, `data036_kom` 258,
    `datai014_kom` 215, `data217_kom` 162, `data039_kom` 105, `datai015_kom` 54, and 11 more
    folders with 1–7 files each.
  - Total size is 160 MB. The median file is 9 KB, 14 files are over 1 MB, and the largest
    is 5.97 MB.
  - 273 `.src.lua` and 275 `.std.luac` siblings already exist, left behind by earlier work.
  - **Every file is header `1b 4b 4c 82` with flags byte `0x02` (`BCDUMP_F_STRIP`).** There is
    no debug info and no variable names anywhere, so there is no embedded ground truth for
    which registers were real locals. Reading the June binary's `lj_bc_mode[]` (which fixed
    the opcode bug) cannot help here. Ruled out; don't retry.
  - **Out of scope:** `data036_kom/item.kim` and `item_LA.kim`. They are a KOG `KIM ` item
    container, not Lua. Confirmed; don't re-check.
  - Kenta's June original: `data037_kom/RAGE_DRYADKENTA.lua`. `RAGE_DRYADKENTA_HENIR.lua`
    sits next to it.
- **Truncation hazard.** elstools 1.0.0 extracts about 18.7k June algo-2 `.kom` entries 1–255
  bytes short. Lua files are algo 2, and the `14JUN17\*_kom` copies have the problem.
  - A LuaJIT dump writes its **main chunk last**, so a short `.lua` can lose exactly the
    prototype that defines everything.
  - Phase 0's `decompile_corpus.tsv` says which files are affected. A truncated original is
    `unverifiable`, never "clean".
- **Two ljd copies.**
  - `elstools/` (with `elstools.exe`, `vendor/ljd`, `vendor/luajit`) is **gitignored**.
  - `elstools/elstools_config.json` points `elstools.exe` at a *second* copy,
    `F:\elsword stuff\elstools\elstools\vendor\ljd`. It was identical to the in-repo copy on
    2026-09-27 (`diff -rq` clean).
  - Editing either copy changes nothing git can see. The fix is a hook; see the decisions.
- **Lua tooling:**
  - **June input needs the opcode fix.** elstools mis-maps KOG opcodes `0x2f`–`0x37`, so
    `decompile-batch` dies on any `for … in`, and `compile-batch -v jun-*` emits broken
    bytecode. `june2017_re/ljd_jun2017.py` corrects the opcodes while ljd reads them. See
    `june2017_re/README.md`, "June LuaJIT opcode table". `-v mar-2014` is unaffected.
  - `june2017_re/ljconv.py` transcodes June `0x82` to `mar-2014` `0x81`. mar-2014 is stock
    LuaJIT 2.0.2 plus a header. Its opcode table is injective, checked against
    `lj_bc_mode[]`, and a hard error on any unmapped opcode.
  - `elstools/vendor/luajit/luajit.exe` (KOG LuaJIT 2.0.2) **runs `0x81` only**. Its `0x82`
    table has the same swap bug.
  - No `lua`/`luajit` is on `PATH`. Python is 3.14.
  - `elstools compile-batch -v mar-2014 -i <file>` **compiles in place.** Only ever run it on a
    copy in `tmp/`.
  - `ljd main.py --asm -f <file>.std.luac` gives a raw pseudo-asm listing. It still works when
    the full AST decompile asserts.
  - `june2017_re/fxdump.lua` is the precedent for executing lua against recording stubs
    (`missing_reads`, stub methods, one shared environment).
- **Process rules:**
  1. **Never kill processes by image name.** Other `python.exe` processes (MCP servers) run
     on this machine. Bound long scripts with `faulthandler.dump_traceback_later` and a
     per-file timeout, and kill by PID only.
  2. **Long runs are resumable.** Append one TSV row per file as it finishes, and support
     `--resume`. Run them with `run_in_background`, and read the TSV, not stdout.
  3. **A divergence is a confirmed defect only when the harness can name it.** It names the
     function, the seed, and the first differing event on both sides. "Unreached" (the
     harness never executed the flagged code) is reported as its own verdict, and it never
     counts as clean.
  4. **Keep the no-`local` convention** in `march2014/` repairs. Decided (Phase 5, 2026-09-28):
     bare globals stay; known clobbers are fixed case by case.
  5. **`march2014/**/*.src.lua` files are ASCII with CRLF.** Preserve CRLF. After any edit,
     check `file <path>` and `git diff --stat`. A few changed lines should not produce
     hundreds, and there must be no deletions on a pure insertion.
  6. **Repacking into `.kom` is the user's step.** Name the files that need it when you report
     done. Don't block on it.
  7. **Cite bytecode evidence in repairs.** Use the `SERV_IRUHADEV_RANOX_PORT (<date>): ...`
     comment convention from `RAGE_DRYADKENTA.src.lua`, naming the harness verdict, not
     "this looked wrong".
  8. **Ask before launching the game exe**, every time.

---

## Investigation ledger — what is already proven (do not redo)

### The defect and why it is dangerous here

- **The printed shape** is `if slot1:GetEffectSet() ~= nil then slot5:Play...(...) end`,
  where the bytecode is `slot5 = slot1:GetEffectSet(); if slot5 ~= nil then slot5:Play... end`.
- **Why it's dangerous here.** Ported files never declare `local`, so `slotN = …` compiles to
  `GSET`/`GGET` on a named global. This was confirmed by disassembling `compile-batch`
  output. A dropped write reads as whatever that global last held, not as `nil`.
  - `==`/`~=` then give silently wrong results.
  - `<`/`<=`/`>`/`>=` raise and abort the calling function.
- **The global model is a second, untested hazard.**
  - `MODS.md` calls the global model "safe within one function's own sequential statements,
    since nothing else runs between them".
  - That is **not proven** when a function calls a same-file Lua function, or an engine
    method that re-enters Lua, and the callee writes the same `slotN`.
  - Phase 2a gives the harness an explicit **nested-frame clobber** signal to test this.
    Don't assume it in either direction.

### Ground truth for calibration

- **Kenta.**
  - Pre-fix port file: `git show 857dfd7^:march2014/update/data037/RAGE_DRYADKENTA.src.lua`.
  - Fixed file: the working tree. It carries 8 `SERV_IRUHADEV_RANOX_PORT (2026-09-27)`
    comment sites, at lines 545, 564, 580, 786, 809, 987, 1038 and 1434.
  - `MODS.md` counts **6 instances** in 5 functions:
    - `..._ATTACK_B_WAIT_FRAME_MOVE` (3 dropped writes in one function)
    - `..._ATTACK_A_ATTACK_DASH_STATE_START` (2)
    - `..._COMMON_FRAMEMOVE`
    - `..._SPECIAL_ATTACK_FIRE_FRAME_MOVE`
    - `..._FIRE_EMOTION_FRAME_MOVE`
  - The comment-site count, the "6", and the per-function counts don't obviously agree.
    **Reconciled by Phase 2b: 12 dropped assignments in 5 functions.** See "Calibration"
    below.
- **MapData.** `march2014/data036/MapData.src.lua`'s `ComputeCOBOExpressTicketCost` reads an
  unassigned `slot6` in a travel-ticket cost formula. It was hand-verified as real.
- **Triage scale.** `RANOX_LUA_DECOMPILE_BUG.md`'s table lists 61 flagged `march2014/` files
  with 774 lint hits; 60 of those files are in `data037/`. It is heuristic and unverified
  beyond the two items above.

### ljd internals (read, not yet tested)

- **Pipeline** (`elstools/vendor/ljd/main.py:355-450`):
  1. `rawdump.parser.parse`
  2. `ast.builder.build`
  3. `mutator.pre_pass`
  4. `locals.mark_locals`
  5. **`slotworks.eliminate_temporary(identify_slots=True)`** (`:380`)
  6. `unwarper.unwarp`
  7. `mark_local_definitions`
  8. `mutator.primary_pass`
  9. a second `mark_locals` (`alt_mode`)
  10. an "unsafe extra pass" (`:415-448`), which calls `eliminate_temporary(safe_mode=False)`
- **The unsafe extra pass cannot fire on June input.** It is gated on
  `node._debuginfo.variable_info`, and every June file is stripped. Phase 3 confirms this
  with a log line rather than assuming it.
- **Elimination policy** (`slotworks.py:329-348`). A slot is inlined only when it has
  exactly one simple reference, or two with `safe_mode=False`. With more, it is
  `unsafe` and gets rechecked.
- **Leads for Phase 3** (hypotheses, ordered by how directly they produce the observed shape):
  1. **`_fill_simple_refs:286`.** Under `ignore_ambiguous`, references with
     `identifier.id == -1` are **skipped** and never counted.
     - A register read twice, once unambiguously in the `if` condition and once
       "ambiguously" in the body after a branch merge, then looks single-use and is folded
       into the condition.
     - The body's read survives as a bare `slotN`.
     - This matches Kenta exactly.
  2. **`_recheck_unsafe_cases:175-177`.** `del new_slots[idx]` inside `enumerate(new_slots)`
     skips the element after every deletion. The `safe_mode` filter meant to keep only
     previously-unsafe slots therefore leaks others into elimination.
  3. **`_SlotsCollector`'s slot-id assignment across blocks** (`_find_slot_assignments:724`,
     `leave_block:888`). This is what produces the `-1` "ambiguous" ids in the first place.
     If it splits one live range into two ids, the reference counting in lead 1 is wrong
     upstream of it.
- **Lineage.** `vendor/README.md` gives the lineage as NightNord/ljd → forks. `slotworks.py`'s
  comments cite `gitlab.com/znixian/luajit-decompiler` issue #19. Later upstream commits may
  already fix this.
- **A separate, known ljd defect** (`june2017_re/README.md`, "ljd caveat"): ljd mis-structures
  a recursive `deepcopy`, hoisting `slot1[k]=uv0(v)` into a bogus `return {…}`.
  - The harness will see this too.
  - It is **not** the dropped-assignment class. Classify it separately and don't let it
    count toward or against the fix.

### Dead ends and traps (do not repeat)

- **Binary-derived ground truth.** Tracing the June client binary the way the opcode bug was
  fixed doesn't apply: the bytecode is stripped, and nothing in the file or the binary says
  which registers were locals.
- **The lint is a lead.**
  - Zero hits doesn't prove a function correct.
  - A hit doesn't confirm a defect.
  - It is regex-based and doesn't handle multi-line statements or nested nameless functions.
- **Feeding `0x82` files to the vendored `luajit.exe`.** Convert with `ljconv.py` first.
- **Running `compile-batch` on a tracked file.** It overwrites its input.

### Calibration (Phase 2b, 2026-09-28)

Every number here comes from `python june2017_re/ljdiff/calibrate.py` (about 7 minutes; exit 0
iff every criterion holds; report in `tmp/ljcal_report.txt`). The harness is the
post-fix one (see "Phase 2b result", harness fixes 1–2). Sweeps use **32 seeds**; the fix
loop uses **128**.
- **Type oracle:** `types.tsv` had 45 rows, `types_h32` `e9e20f92`.
- Types are keyed by name and the table grows with every new file, so later counts can
  shift for reasons unrelated to the decompile.
- Phase 4's "no new `other-ljd`" comparison must run under this oracle, or re-baseline the
  unfixed side first.

**Kenta: the true count is 12 dropped assignments.** All 12 were derived from the original
bytecode listing (`bcfn.lua`), restored in `fixes/RAGE_DRYADKENTA.json`, and confirmed by
the fix-rerun loop at 128 seeds.
- **Every one is necessary.** With all fixes applied but that one, its function diverges.
- **Together they account for all 15 unique static read sites**: 14 owned outright, and
  `slot5@L559`, which is unwritten only when BW-2 *and* BW-4 are both missing.

| Function | Dropped writes (fix id: what) | Static sites (fresh decompile) | 09-27 comment sites | MODS.md | Earlier prediction |
|---|---|---|---|---|---|
| `ATTACK_B_WAIT_FRAME_MOVE` | 4. BW-1 `slot6 = GetRandVal()%3`; BW-2 `slot5 = GetNearestUserUnit_LUA(…)`; BW-3 `slot4 = slot5:GetPos()` (folded into the field-set target); BW-4 `slot5 = GetEffectSet()` (the softlock call) | 3: `slot6@549`, `slot5@555`, `slot5@559` (joint BW-2/BW-4). BW-3 has none | 3 (545; 564 covers BW-2 and BW-3; 580) | 3 | Phase 1: 4 |
| `ATTACK_A_ATTACK_DASH_STATE_START` | 3. DS-1 `slot3 = GetLastTouchLineIndex()`; DS-2 `slot4 = GetTargetUser()`; DS-3 `slot5 = GetLineGroupStartPos()` | 4: `slot3@749`, `slot4@757`, `slot4@758`, `slot5@762` | 2 (786; 809 covers DS-2 and DS-3) | 2 | Phase 1/2a: 3 |
| `COMMON_FRAMEMOVE` | 1. CF-1 `slot3 = GetNowHP()/GetMaxHP()` | 1: `slot3@1337` | 1 (1434) | 1 | — |
| `SPECIAL_ATTACK_FIRE_FRAME_MOVE` | 2. FI-1 and FI-2: `slot3 = GetLineMap()`, once per timer block (1.2, 1.35) | 3: `slot3@919`, `@929`, `@930` | 1 (987, "every `slot3:GetStartPosition`") | 1 | — |
| `SPECIAL_ATTACK_FIRE_EMOTION_FRAME_MOVE` | 2. EM-1 and EM-2: the same, blocks 0.3 and 1.2 | 4: `slot3@959`, `@960`, `@970`, `@971` | 1 (1038) | 1 | — |
| **Total** | **12** | **15 sites (18 GGET rows)** | **8** | **"6"; its bullets sum to 8** | |

- **How the counts relate.** The 8 comment sites equal MODS.md's per-function figures
  exactly (3+2+1+1+1). MODS.md's headline "6" matches neither.
- **Where the extra 4 are.** Each is a comment that covers two writes: 564, 809, 987 and 1038.
- **The fixed port restores all 12.** At 128 seeds (`calibrate.py` section 4b) its 5
  functions show no `dropped-assignment`, and `ATTACK_B_WAIT_FRAME_MOVE` passes on 128/128
  seeds, BW-3's path included. It has 0 static candidates. At 32 seeds, BW-3's path can go
  unexercised, so the 32-seed table below can't show this.
- **Two further restorations are not dropped writes** (kind `order`, other-ljd evaluation
  order). Both are necessary for exact equality:
  - BR-O, `ATTACK_B_READY_STATE_START`. `GetVector_LUA(0)` moved after the second
    `GetLineMap()`, and a `.y` read moved before `GetLandPosition_LUA()`.
  - FI-O, the FIRE timer-1.2 `Play` call. `GetStartPosition(8)` and `GetRotateDegree()` are
    swapped.
- **How each write shows, fix left out, at 128 seeds.**
  - 9 as `poison` and 1 as `fatal poison` (CF-1).
  - 2 as diverge-only, with no unwritten read at all:
    - BW-4, 27/128 seeds: stale `slot5` from BW-2's path.
    - BW-3, 4/128 seeds: stale `slot4`.
  - Ladder order changes these labels, but not the count. The ladder runs bottom-up within a
    function, because a restored write can turn a later unwritten read into a stale one.

**Three comparisons at 32 seeds.** Verdicts are counted per (function, seed). Functions not
listed pass on every side.

| Function | orig vs rt (fresh) | orig vs pre (`857dfd7^`) | orig vs fix (working tree) |
|---|---|---|---|
| `ATTACK_A_ATTACK_DASH_STATE_START` | dropped-assignment 31, pass 1 | dropped-assignment 32 | other 7, pass 25 |
| `ATTACK_B_WAIT_FRAME_MOVE` | dropped-assignment 11, other-ljd 1, reorder 1, pass 19 | same (other 1) | pass 32 |
| `COMMON_FRAMEMOVE` | dropped-assignment 8, other-ljd 4, reorder 4, pass 16 | dropped-assignment 8, other 8, pass 16 | other 7, pass 25 |
| `SPECIAL_ATTACK_FIRE_FRAME_MOVE` | dropped-assignment 27, reorder 5 | same | reorder 13, pass 19 |
| `SPECIAL_ATTACK_FIRE_EMOTION_FRAME_MOVE` | dropped-assignment 27, reorder 5 | same | pass 32 |
| `ATTACK_B_READY_STATE_START` | other-ljd 5, reorder 18, pass 9 | same | same |

- **(a) and (b) pass.** All five known functions show `dropped-assignment` on both rt and pre.
- **Pre vs rt, by (function, slot).** Every unwritten read and its seed count is identical
  except DASH's `slot4`: 32/32 seeds in pre vs 25/32 in rt.
  - The pre-fix port had also deleted the `GetStoredGUUser()` fallback, which is the only
    other write of `slot4`.
  - That +7 is the whole poison-total gap (pre 139 vs rt 132; in 2a's old-hash run it was 143
    vs 137).
  - COMMON's pre `other` 8 vs rt `other-ljd` 4 + `reorder` 4: the pre port drops
    `InitStateCoolTime`, so the 4 reorder seeds also miss a call.
- **(c) passes.** No `dropped-assignment` remains. Four residuals are left, each named:
  1. DASH other 7. **Port edit** (09-18, fixed file L800): the `GetStoredGUUser()` fallback
     was removed because the method is unbound in March. These are the 7 seeds where
     `GetTargetUser()` returned nil.
  2. COMMON other 7. **Port edit** (09-18, L1425): both `InitStateCoolTime` calls were
     removed for the same reason. These are the 7 seeds that took either dash branch.
  3. B_READY. **Not a port edit.** It is BR-O, inherited from the decompile and untouched by
     the port.
  4. FIRE reorder 13. **Not a port edit.** It is FI-O, likewise inherited.
  - Items 3 and 4 involve only getters, so they are probably harmless in-engine. They are
    still real divergences, and Phase 5/6 should decide them. None of the 8 comments covers
    them.

**MapData: reproduced.**
- `ComputeCOBOExpressTicketCost` shows `dropped-assignment` on 32/32 seeds, reading
  `slot6@L120`. `CheckCOBOExpressTicketMapID` passes on 32/32.
- The fix loop (`fixes/MapData.json`) confirms 1 dropped write, MD-1:
  `slot6 = CheckCOBOExpressTicketMapID(slot0)`.
- It also confirms 1 separate `structure` defect, MD-S. ljd printed the
  `if slot6 == 0 or slot7 == 0 … else …` as `(A and B or C) and D`, which always yields `D`.
  - It diverges on 32/32 seeds once MD-1 is in, and is other-ljd.
  - A text lint cannot see it.

**Noise check: clean.** Each file is zero divergences at 32 seeds, with 0 static rows, 0
`SHADOW_READ` and 0 lint hits.

| File | Calls | What it exercises |
|---|---|---|
| `data034_kom/DLG_Map_Select_Window_Instance.lua` | 64 pass, main pass | The main chunk writes and reads back its own `slot3` temp. That is the main-chunk exemption's actual role (Phase 1 correction 7). |
| `datai016_kom/EVENT_HALLOWEEN_RESPAWNER_SAND.lua` | 64 pass, main pass | Functions read and update file-scope state (`CreateNPCCount`, `posArray`), through deep restore |
| `data038_kom/SQUARE_GM_Ariel.lua` | 416 pass, main pass | 13 functions read `INIT_PHYSIC` |

- **Reinterpretation of the criterion.** The phase asked for "main-chunk locals read by its
  functions". That cannot be clean, for the reason Phase 1 correction 7 gives: ljd prints a
  captured file-scope local as `uvN`.
  - It was confirmed on `data034_kom/DLG_LUCIEL_TRANSFORM_SKILL_SLOT.lua`: it is lint-clean
    and has main-chunk locals `slot0`..`slot4`, yet it gives `upvalue` on 32/32 seeds.
  - That is a real round-trip break, not noise. The check was therefore split across the
    three files above.
- **No single clean file does both.** In the scanned subset (`tmp/find_noise.py`: ≤ 40 KB, ≥ 2
  functions, `data037_kom` excluded), no lint-clean file has both a main-chunk `slotN` temp
  *and* functions reading file-scope state.
  - The one candidate, `DLG_Tournament_MatchList.lua`, has a real ljd main-chunk hoist (see
    "Leads").
- **Phase 4 carries all three.** Its "noise-check file is byte-identical" check applies to all
  three.

**Lint precision and recall.** The unit is a unique (line, slot) on the same text.

| Text | Truth | Lint hits | TP / FP / FN | Precision | Recall | Misses |
|---|---|---|---|---|---|---|
| Kenta rt | 15 (fix-loop sites = static pass, all confirmed) | 11 | 11 / 0 / 4 | 1.00 | 0.73 | `slot5@555`, `@559`, `slot4@757`, `@758` |
| Kenta pre | 15 (confirmed static sites) | 13 | 13 / 0 / 2 | 1.00 | 0.87 | `slot5@555`, `@559` |
| MapData rt | 1 | 1 | 1 / 0 / 0 | 1.00 | 1.00 | — |

- **By instance, on Kenta rt.**
  - The lint flags a read of 8 of the 12 dropped writes, and misses BW-2, BW-3, BW-4 and DS-2.
  - The static pass flags 11 of 12.
  - BW-3 has no unwritten read anywhere, so only the dynamic harness sees it.
- **What the lint misses** is every "written on one arm, read after the merge" site. The lint
  counts any earlier textual assignment as a write.
  - On pre it catches DASH's `slot4` only because the port deleted that textual assignment.
- **What no text lint can see:** the order and structure defects (BR-O, FI-O, MD-S) and the
  stale-value instances (BW-3, and BW-4 once BW-2 is in).
- **So:** a lint hit is trustworthy (0 false positives in 3 texts). A lint zero is not: about
  a quarter of the sites and a third of the instances were missed.

**Choosing N.**

| Measure (Kenta) | N=8 | 16 | 32 | 64 | 128 |
|---|---|---|---|---|---|
| Known functions detected (of 5) | 5 | 5 | 5 | 5 | 5 |
| Round-trip lines executed, all 13 functions | 92 | 93 | 95 | 95 | 96 |
| Seeds reaching COMMON's `elseif` body | 1 | 3 | 3 | 5 | 10 |
| Distinct path signatures, DASH | 6 | 12 | 25 | 47 | 84 |

- **Line coverage saturates by 32** in 12 of 13 functions. The exception is B_READY's
  `slot3 = 2` line, which needs two mock numbers to be equal.
- **Path signatures never saturate.** A signature is the call's events and outcome with every
  number and mock string masked. Independent 1/6 nil returns multiply the error paths, so
  "raising N stops finding new traces" is unattainable by construction. Line coverage is the
  saturation measure used instead.
- **Only one function has coverage 1:** `MovePos`, a straight-line vector helper.
- **N=32 for sweeps.** Every function is detected at 8, and lines saturate at 32.
- **N=128 for the fix loop.** Leave-one-out has to hit rare paths. BW-3 shows on ~3% of seeds,
  so 32 seeds miss it about a third of the time (it did miss on one 32-seed run).
- **A clean 32-seed sweep is not proof against stale-value instances.** "Every function
  detected at 8" holds on Kenta only because each defective function there also has an
  unwritten read.
  - A function whose only defect is BW-3-shaped would pass a 32-seed sweep. Such a defect
    has a stale value and no unwritten read, shows on ~3% of seeds, and neither the static
    pass nor the lint can see it.
  - Any function a sweep cares about gets the 128-seed fix loop.
  - Phase 4's "Kenta has no `dropped-assignment`" check must also run at 128 seeds, or it
    says nothing about BW-3.

---

## Phase 0 — Corpus inventory and input integrity

**Paste with:** Starting context + this section.

**Model:** Sonnet 5, high.

**Goal.** Before any harness exists, know which of the 7,328 originals are trustworthy
inputs, and make the conversion and decompile path work for every one of them without
depending on `.std.luac` siblings.

**Work.**
1. **The runner.** Build `june2017_re/corpus_run.py`, a generic resumable per-file runner that
   every later sweep reuses. It has:
   - a file list from a glob or TSV
   - a per-file subprocess with a timeout
   - `faulthandler`
   - kill by PID
   - one TSV row appended per file
   - `--resume`, which skips rows already present
2. **Enumerate** every non-`.src.lua`, non-`.std.luac` `.lua` under `14JUN17\data\*_kom\`,
   excluding `item.kim`/`item_LA.kim`. Recount; don't reuse this plan's numbers.
3. **Truncation, two independent checks.** Record both.
   - **Manifest.** Compare each file's size to its `.kom` manifest entry. The
     `elstools inspect` manifests and cache live under `june2017_re/assets_cache/inspect/`
     (`fxdata.py`). Generate any June archive's manifest that is missing with
     `elstools inspect`.
   - **Structure.** Walk the dump with `ljconv.instruction_offsets`' walker. A file whose
     prototype walk doesn't end exactly at a zero-length terminator at EOF is truncated,
     whatever the manifest says.
   - Record **bytes short** and **how far the dump walk got**: the last complete prototype
     index and the offset.
   - **Split two subclasses:**
     - **`terminator-only`:** every prototype walks intact and only the trailing zero-length
       terminator (1 byte) is missing. The file is provably complete; append the byte in a
       `tmp/` copy, and it's a normal input.
     - **`unverifiable-truncated`:** anything else.
     - About 25% of June algo-2 entries are short, so this split may bring a large slice of
       the corpus back into scope.
   - **Don't invent any other repair.** elstools truncates with every `-v`. List the affected
     files. Report the count per subclass, the dirs, and whether any `march2014/` port file
     came from one.
4. **Opcode coverage.**
   - Collect the set of KOG opcodes used across all files.
   - `ljconv.py`'s table was built from only the 14 `june_lua/` pairs, so any opcode those
     files lack is currently unmapped.
   - For each missing opcode, get elstools' translation from a real `.std.luac` pair: run
     `decompile-batch` on one file that uses it, in `tmp/`. Check it against
     `lj_bcmode.py`'s mode table.
   - An opcode neither source settles is a hard stop. Record it and ask. Never guess.
5. **Decouple from `.std.luac`.**
   - Persist the complete table as `june2017_re/ljconv_table.tsv`.
   - Add a mode that turns a June file into (a) `mar-2014` bytecode and (b) stock
     `1b 4c 4a 01` bytecode for ljd, with no sibling needed.
   - **Check (b)** against the 275 existing `.std.luac` pairs. The bytes must be identical
     except exactly at elstools' wrong FIX-slot opcode positions, which are expected to
     differ. Anything else is a bug in the new mode.
6. **Decompile status.** Run the corrected decompile on every file through `corpus_run.py`,
   writing output under `tmp/`, with a timeout. The 14 files over 1 MB may need a longer
   budget, or a `timeout` verdict. Record per file:
   - ok
   - assert, with its type (for example the `deepcopy` class)
   - timeout
   - unverifiable-truncated

**Outputs.**
- `june2017_re/corpus_run.py`
- `june2017_re/ljconv_table.tsv`
- `ljconv.py`'s new mode
- `june2017_re/decompile_corpus.tsv`, with one row per file: dir, name, size, manifest size,
  truncated (manifest/structure), opcode set hash, ljconv ok, decompile verdict, and
  wall time
- A short "Phase 0 result" section appended to this plan, with the counts.

**Exit criteria.**
- Every file has a row.
- Kenta's and MapData's June originals are present, not truncated, convert, and decompile.
- The `.std.luac` equivalence check passes on all 275 pairs.

**Kickoff prompt**:
```
Starting the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md at the repo
root ("Starting context", "Investigation ledger", "Phase 0"), then RANOX_LUA_DECOMPILE_BUG.md,
june2017_re/README.md's "June LuaJIT opcode table" and "Phase 10b" sections, and
june2017_re/ljconv.py and ljd_jun2017.py. Branch: main.

Your task is Phase 0: build june2017_re/corpus_run.py (resumable per-file runner: timeout,
faulthandler, kill by PID only, --resume); inventory every June .lua under
14JUN17\data\*_kom\ into june2017_re/decompile_corpus.tsv with truncation (manifest AND
dump-walk, bytes short, walk depth; split terminator-only from unverifiable), opcode coverage, ljconv and decompile status; extend ljconv's table to every
opcode in the corpus (hard stop and ask on any opcode not settled by an elstools pair plus
lj_bcmode.py); and add a no-.std.luac mode verified byte-for-byte against the 275 existing
pairs (differences allowed only at FIX-slot opcodes). Scratch goes in <Trunk>/tmp/. Never
write into 14JUN17\. Never kill python by image name. Append a "Phase 0 result" section to
the plan with the counts.
```

---

## Phase 0 result

Ran 2026-09-27. All exit criteria met: every one of the 7,328 files has a row, Kenta's and
MapData's June originals are present/untruncated/converting/decompiling, and the `.std.luac`
equivalence check passes on all 275 pairs.

**Corpus recount: 7,328, confirmed exactly** (per-folder counts too), across 21 `*_kom`
folders. The raw `*.lua` glob returns 7,601 — the extra 273 are the pre-existing `.src.lua`
siblings, excluded. Total size 159.67 MB, median 9,112 bytes. All 7,328 have header
`1b 4b 4c 82`, flags `0x02` (`BCDUMP_F_STRIP`) — confirmed for every file, not spot-checked.

**Truncation: zero, both ways — this corrects the plan's Starting Context.** A new tolerant
dump-walker (`ljconv.instruction_offsets_tolerant`, alongside the existing strict one — never
raises, reports a status plus how far it got) cross-checked against `elstools inspect`
manifests for all 21 `dataNNN.kom` archives: **all 7,328 files match their manifest size
exactly and walk cleanly to a terminator at EOF.** Kenta and MapData included. The plan's
"elstools 1.0.0 extracts ~18.7k June algo-2 entries short, and the `14JUN17\*_kom` copies have
the problem" does not apply to this corpus: every `.lua` entry here is algo `3`, not algo `2`.
The algo-2 bug is real in these same 21 archives (checked: of 4,317 algo-2 entries, 895 are
short on disk vs. their manifest) — it just never lands on a `.lua` file. `corpus_run.py` and
later sweep phases can skip a truncation-repair path entirely.

**275 `(.lua, .std.luac)` pairs exist, all with a matching original; 3 have no `.src.lua`**
(`DamageEffect.lua`, `SECRET_MAGI_BOSS.lua`, `SECRET_SCENE_DUMMY.lua` — all three contain a
FIX-block opcode per `decompile_corpus.tsv`'s `n_fix_hits`, i.e. elstools' own decompile-batch
failed on all three for the same documented `for...in` swap, not three separate causes).
Conversely `Enum.src.lua` has no `.std.luac` sibling — origin unclear, not investigated
further (out of scope). `272` true pairs `+ 1` orphan `= 273`; `275 − 3 = 272`.

**Opcode coverage: 66 distinct KOG opcodes in the corpus; 71 now mapped, all consistent with
June's `lj_bc_mode[]`.** The 275 pairs covered 63 (incl. all 9 FIX-block opcodes
unconditionally). 8 needed new evidence — `0x02`/`0x06`/`0x12`/`0x20`/`0x22`/`0x29`/`0x51`/
`0x2d` (ISLE/ISEQS/IST/SUBNV/DIVNV/POW/LOOP/CALLMT) — each settled from one real corpus file's
`elstools decompile-batch` output, the same method as the original 14 (evidence file per
opcode recorded in `ljconv_table.tsv`'s new `evidence` column). No opcode was unsettled; the
hard-stop-and-ask path never triggered. The corpus's full opcode set is a verified subset of
the table (checked programmatically — this is how the one opcode missed on a first manual
pass, `0x2d`, was actually caught). One recorded, non-blocking limit: `lj_bc_mode`'s
operand-type-plus-metamethod encoding can't distinguish every same-shape pair (e.g.
`ISLT`/`ISGE` share both); for every opcode outside the documented FIX block, "elstools' own
translation" remains the sole source of truth per the plan's own methodology, `lj_bc_mode` is
only a coarse secondary check, and this was already true before Phase 0.

**No-sibling mode added to `ljconv.py`:** `transcode_stock()` (June → stock `1b 4c 4a 01`
bytecode directly, no `.std.luac` needed) plus `verify_stock_mode()`/`--verify-stock`.
Verified byte-for-byte against all 275 pairs (set equality: differs from elstools' own
`.std.luac` at exactly the FIX-slot instruction bytes, nowhere else, and every FIX-slot byte
does differ). Decompile-path validation: the 14 `june_lua/*.lua` reference files, run through
`transcode_stock` + completely unmodified vendored `ljd` (no hook), reproduce the existing
`ljd_jun2017.py`-hook-produced `.src.lua` references byte-for-byte, all 14.

**`corpus_run.py`** — generic resumable runner, one subprocess per file (ljd keeps
per-process state). Workers arm their own `faulthandler.dump_traceback_later(budget-margin,
exit=True)` via `arm_faulthandler()`; the parent's backstop kill is `taskkill /F /T /PID <pid>`
(never by image name). `--resume` truncates a dangling partial last line off the file itself
before appending (an earlier version only skipped it in memory, which would have glued a new
row onto the fragment — caught before the real sweep, by inspection, not by hitting it).

**`decompile_worker.py`** — per file: fast local truncation check, opcode-set hash, table
lookup, `transcode_stock`, in-process unmodified-`ljd` decompile. Verdicts: `ok` / `assert`
(with exception type + innermost `ljd/file.py:line`) / `unverifiable-truncated` /
`unmapped-opcode` / `TIMEOUT` (the worker's own faulthandler firing first, detected from its
`.hang.txt` starting `Timeout (` — this is the common case, not the parent's backstop) /
`CRASH` (anything else unexplained, with stderr saved to a `.err.txt`). A dry run at
`--timeout 60` on `DamageEffect.lua` initially surfaced as `CRASH` with the real cause sitting
unread in the hang log; `run_one` now checks hang-log content before falling back to `CRASH`.

**The full sweep: 7,328 / 7,328 files, `ljconv_ok=yes`, `decompile_verdict=ok`.** Two batches:
13 files over 2^20 bytes at `--timeout 300 --workers 2` (largest, `EffectSet.lua` 5.97 MB,
took 106s), the remaining 7,315 at `--timeout 90 --workers 8` (max observed 26.9s, well inside
the 85s faulthandler deadline). By the plan's decimal-MB reckoning that split is 14/7,314 —
`BattleFieldBuffFactor.lua` at 1,011,108 bytes sits between the two thresholds and landed in
the 90s batch instead, finishing in 21s with room to spare either way. Zero truncation, zero
unmapped opcodes, zero asserts, zero timeouts, zero crashes. `ok` means **ljd returned without
raising, not that the printed Lua is semantically correct** — `DamageEffect*`/`EffectSet`
still carry the known `deepcopy` mis-structure (README.md, "ljd caveat") and are still `ok`;
that class of defect is Phase 3/4's problem, not something a clean Phase 0 run rules out.
Confirmed non-empty output for all but 19 files, and those 19 are legitimately empty 18-byte/
1-instruction stub scripts (verified via `decompile_corpus.tsv`'s own `size`/`n_instructions`
columns), not silently-dropped output.

**Outputs:** `june2017_re/corpus_run.py`, `decompile_worker.py`, `truncation_census.py`,
`build_decompile_corpus.py` (the last two make `decompile_corpus.tsv` regenerable from tracked
code, not just from scratch files in `tmp/`), `ljconv_table.tsv` (71 opcodes, with an
`evidence` column and a shrink-guard on `--build-table`), `ljconv.py`'s new mode, and
`decompile_corpus.tsv` (7,328 rows: `dir, name, size, manifest_size, manifest_short_by,
truncated, walk_status, protos_complete, last_complete_end, opcode_set_hash, ljconv_ok,
decompile_verdict, wall_seconds, detail, n_instructions, n_fix_hits, n_distinct_opcodes,
exit_code, timed_out`).

**Not done here, by design:** independently re-deriving same-shape opcode pairs beyond
trusting elstools' translation (a harder question than "is every opcode settled"); mapping
`march2014/**/*.src.lua` to June originals (Phase 5's job, and has no open dependency on this
phase now that truncation is a non-issue).

---

## Phase 1 — Engine-free synthetic oracle

**Paste with:** Starting context + this section + "Phase 0 result".

**Model:** Opus, high.

**Goal.** Reproduce the defect from Lua source you wrote yourself, with no engine, no mock
and no June data involved. This is the smallest possible repro for Phase 3, and a regression
suite for Phase 4 that can't be confused by mock noise.

**Work.**
1. Write `june2017_re/ljd_oracle/cases/*.lua`. Each file is a handful of small functions over
   plain values, with deterministic inputs, returning or logging their results. Cover these
   shapes:
   - **The Kenta shape.** `x = f(); if x ~= nil then x:g() end`, where `x`'s register was
     written earlier by an unrelated branch.
   - **The same shape inside an `elseif` chain.**
   - **A value read by an `if` condition and again after the `if`/`else` merges.** This tests
     lead 1's ambiguous-reference theory directly.
   - **The HP-ratio case.** A division result stored, then compared with `<=` in two separate
     later `if`s.
   - **Controls ljd *should* inline.** Genuine single-use temporaries (`f(g(x))`, a method
     call on a fresh result). These measure over-conservatism, which matters in Phase 4.
   - **The known `deepcopy` shape.** Kept as a labelled second-defect control.
2. **Check what `luajit.exe -b` actually emits first.** `vendor/README.md` says the KOG build
   writes `0x81` *or* `0x82` headers, and its `0x82` opcode table is the swapped one.
   - Confirm the version byte is `0x81` before swapping the header to stock `1b 4c 4a 01`.
   - If it's `0x82`, find the switch that selects `0x81`, or build with stock semantics.
   - Otherwise the oracle feeds ljd mis-opcoded input and measures the wrong bug.
3. Build each case two ways with the vendored `luajit.exe`:
   - **Stripped.** `-b` strips by default. Confirm that from the output's flags byte.
   - **With debug info (`-g`).**
   - The `-g` variant is the control. ljd with variable names should *not* hit the defect.
     If it does, the theory about stripping is wrong. Record that.
4. **Round trip.**
   1. Swap the header to stock `1b 4c 4a 01`.
   2. Run ljd, unmodified.
   3. Recompile the output with `elstools compile-batch -v mar-2014` on a copy in `tmp/`,
      matching the real port pipeline.
5. **Execute original vs round trip** in `luajit.exe` with a tiny driver.
   - **Poison the `slotN` globals before each call** with a sentinel. This makes a dropped
     assignment deterministic instead of dependent on leftovers.
   - The sentinel is the same one Phase 2a uses. Its rules are there: it logs and
     continues, and it skips names the main chunk wrote.
   - Diff the results.
6. **If no hand-written case reproduces after a genuine attempt**, minimise from real data.
   - Take one confirmed Kenta function (`..._COMMON_FRAMEMOVE` is the smallest).
   - Delete statements from its source until the defect disappears, then keep the last
     failing version as the case.
   - This is still source-level work; don't hand-edit bytecode.

**Outputs.**
- `june2017_re/ljd_oracle/`, containing the cases, `run_oracle.py` (one command runs
  everything and prints a pass/fail table per case per build), and the expected-results file
- A "Phase 1 result" section in this plan, recording:
  - which shapes reproduce
  - whether `-g` suppresses the defect
  - the minimal repro, named

**Exit criteria.**
- At least one case reproduces the dropped assignment in the stripped build, showing
  `POISON_READ` on the round-trip side only.
- Every control passes.
- `run_oracle.py` is deterministic across two runs.

**Kickoff prompt**:
```
Continuing the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", "Investigation ledger", "Phase 0 result", "Phase 1") and RANOX_LUA_DECOMPILE_BUG.md.
Branch: main.

Your task is Phase 1: build june2017_re/ljd_oracle/, an engine-free oracle. Write small Lua
cases covering the Kenta shape, elseif chains, a read-after-branch-merge, the stored-then-
compared-twice HP-ratio shape, genuine single-use controls, and the known deepcopy shape.
Compile each stripped and with -g using elstools/vendor/luajit/luajit.exe (first confirm it
emits version byte 0x81, not 0x82), swap to the stock header, decompile with unmodified ljd, recompile with compile-batch -v mar-2014 on a scratch
copy in <Trunk>/tmp/, then execute original vs round trip with slotN globals poisoned before
each call (log-and-continue sentinel, main-chunk-written names exempt; see Phase 2a). Done when at least one case reproduces (POISON_READ on the round trip
only), controls pass, and run_oracle.py is deterministic. If nothing reproduces, minimise from
Kenta's COMMON_FRAMEMOVE at source level. Record a "Phase 1 result" section in the plan.
Do not modify ljd.
```

---

## Phase 1 result

Ran 2026-09-28. All exit criteria met: the defect reproduces in the stripped build with
`POISON_READ` on the round-trip side only (26 calls across 5 case files), every control
passes, and `python june2017_re/ljd_oracle/run_oracle.py --twice` is byte-identical
across two clean runs (84 files: bytecode, ljd text, traces), in about 6 s per run.
ljd was not modified.

**Layout.** `june2017_re/ljd_oracle/`:
- `run_oracle.py` runs everything and exits 0 iff every row matches.
  - `--case NAME` runs one case.
  - `--twice` runs the determinism check.
  - `--expect fixed` checks against the Phase 4 target.
  - `--observed F.tsv` dumps every verdict for review.
- `driver.lua` is the runtime.
- `expected.tsv` has 189 rows.
- `cases/<name>.lua` holds 7 cases, each with a `cases/<name>.calls.lua`: the call
  scenarios, loaded as plain source and never decompiled.
- Scratch goes to `tmp/ljoracle_*`, which `run_oracle.py` deletes and rebuilds on every run.
  Don't give other files that prefix.

**Toolchain facts, confirmed:**
- `elstools/vendor/luajit/luajit.exe -b` emits **`1b 4b 4c 81`**, flags `0x02`
  (stripped). `-bg` emits flags `0x00`. `run_oracle.py` asserts both on every build.
- `-b` needs the cwd to be `vendor/luajit/`, or `LUA_PATH` pointing at its `jit/`.
  Otherwise it fails with "unknown luaJIT command or jit.* modules not installed".
- Header-swapping to `1b 4c 4a 01` is all ljd needs.
- `compile-batch -v mar-2014` gives `0x81` back.
- ljd's stripped output has **no `local` at all**. The round trip's `slotN` are real
  globals (`GGET "slot3"` in `-bl`), exactly as in the port files.

**Verdicts** (per call, strongest first):
1. `HARNESS`: the *original* read an unknown global.
2. `REPRO_POISON`: the round trip read a `slotN` it never wrote in this call.
3. `REPRO_GLOBAL`: another unknown name was read, e.g. a `-g` local.
4. `UPVALUE`: a `uvN` was read.
5. `DIVERGE`: the traces differ with no unknown read.
6. `PASS`.

The first differing line is only the detail. The fold also reorders calls (the inlined
call now runs *after* the condition's other operands and is short-circuited), so a
`CALL` line usually differs before the poisoned read does.

**What reproduces.** "Folds" means ljd dropped the assignment and inlined its right-hand
side into the condition.

| Shape | Function (`cases/`) | Stripped | `-g` |
|---|---|---|---|
| COMMON_FRAMEMOVE: ratio in the leading if's `and`, re-read in the `elseif` | `kenta::hp_ratio_elseif` | folds. `REPRO_POISON` (fatal `<=`) on the elseif path; `DIVERGE` (reorder only) at ratio 0.5 | PASS |
| DASH_STATE_START: `or` chain | `kenta::or_chain` | folds | PASS |
| DASH_STATE_START: `== nil` test with fallback, then `~= nil` | `kenta::fallback_user` | folds. The printed text is identical to June's | PASS |
| DASH_STATE_START: value as a call argument in the condition | `kenta::cond_call_arg` | folds | PASS |
| FIRE_FRAME_MOVE: two locals in one `and` | `kenta::and_two_locals` | first operand (`lm`) folds, second (`es`) kept | PASS |
| ATTACK_B_WAIT, full | `kenta::effectset_stale` | all 3 sites fold, printed exactly as June's | PASS |
| Leading `if r == 1` then `elseif r == 2 …` | `elseif::elseif_number` | folds | PASS |
| First read in an **elseif** condition | `elseif::elseif_method`, `elseif_deep`, `elseif_reused_reg` | **kept**, PASS | PASS |
| If-condition read, then read after the merge | `merge::merge_number`, `merge_nil_check`, `merge_both_arms` | folds | PASS |
| Same, but read as the **second** operand of `and` | `merge::merge_and_cond` | **kept**, PASS | PASS |
| Stored ratio, two separate `if r <= …` | `hp::hp_two_ifs` | folds | PASS |
| Same, with `flag == false and r <= …` | `hp::hp_two_ifs_and` | **kept**, PASS | PASS |
| Controls | `controls::*` (6 functions) | PASS | PASS |
| deepcopy (second defect) | `deepcopy::copy_it`, `deepcopy_global` | text identical to DamageEffect's hoisted `return { [slot5] = uv0(slot6) }` | PASS |

**Notes on the table:**
- **The ATTACK_B_WAIT softlock itself is reproduced.** In `bwait/lm-rand1` the round trip
  calls `Play` on the `v0` vector mock (the stale register), errors, and never reaches
  `SetVector(4, …)`. That is Kenta's failure exactly.
  - Its verdict is `DIVERGE`, not `REPRO_POISON`. The stale value was written earlier *in
    the same call*, so no unwritten read exists.
  - This is why strict mocks (an unknown member raises) are required, and why `DIVERGE`
    must never be read as "probably noise".
- **Two wrinkles for Phase 3 to explain:**
  - `hp_ratio_elseif` folds a second-operand `and` read, while `hp_two_ifs_and` and
    `merge_and_cond` keep the same position.
  - The difference is whether the value's *other* read is in an `elseif` or in a separate
    `if`.

**Whether `-g` suppresses it: yes, completely.** All 67 `-g` call rows PASS, including
deepcopy, and ljd prints the real `local` names. The theory that the defect depends on
stripping holds.

**The minimal repro is `cases/minimal.lua::minimal`:**

```lua
function minimal(o)
	local x = o:Get()
	if x then
	end
	return x
end
```

- ljd prints `if slot0:Get() then … end; return slot1`.
- The `if` body can be empty, and the value can be a plain field read (`minimal_field`).
- The paired control `minimal_ctl` is identical except that the condition tests another
  value. It keeps its assignment. That isolates the trigger: the register's first read is
  the leading `if` condition, and reads after the `if` aren't counted.
- This matches ledger lead 1's shape. Phase 3 decides whether lead 1 is the mechanism.

**Controls, and what expected.tsv pins:**
- **Behaviour:** all 14 control calls PASS on both builds.
- **Assignment counts:** ljd's printed count per function (`kind=text`) is pinned too:
  - 0 for the five genuine single-use shapes when stripped
  - 1 for `ctl_two_uses_straight`
  - under `-g`, `ctl_single_use_local` keeps its named `local t`
- **`expect_fixed` column** (for Phase 4):
  - every non-deepcopy call must be `PASS`
  - the control text counts must not change, which catches an over-conservative fix
  - the defect functions' text counts are unchecked (`-`), since a correct fix changes them
  - `run_oracle.py --expect fixed` currently fails on exactly the 29 non-PASS,
    non-deepcopy calls, and on no text row

**`expect_fixed` is reachable with a no-`local` fix.** This was checked once and is not part
of `run_oracle.py`. The script is `tmp/handfix_check.py`; run it after `run_oracle.py`.
- The dropped writes were hand-restored in ljd's stripped text, the way Kenta was fixed:
  bare `slotN` globals, each at its bytecode position.
  - 9 writes across the 6 `kenta` functions, and 2 in `minimal`.
- Recompiled with `compile-batch`, then scored with `run_oracle`'s own verdict against the
  original traces.
- **Every `kenta` and `minimal` scenario is PASS.** The harness therefore doesn't flag
  legitimate in-call `slotN` globals, and a correct Phase 4 output can reach `PASS`.
- `ATTACK_B_WAIT`'s shape needed **4** restored writes, not 3. ljd's
  `slot3:GetPos().y = slot3:GetLandPosition().y` also hides a dropped
  `slot2 = slot3:GetPos()`. The field-set target is where the value was folded into.
  (Phase 2b count lead.)

**Corrections Phase 2a must adopt** (its Design section describes the weaker version):
1. **Detect unwritten reads with the environment's `__index`. Don't store sentinels.**
   - Lua 5.1 runs no `__eq` for table-vs-number or table-vs-nil. A sentinel stored in the
     env is therefore invisible to `slot6 == 1` and `slot5 ~= nil`.
   - Those are the RandVal, GetEffectSet and fallback-user sites. The stored-sentinel
     design would have scored them clean.
   - `driver.lua` leaves unwritten names absent, logs the read, and *then* returns a
     log-and-continue sentinel.
2. **Track writes explicitly.**
   - `slot2 = nil` deletes a global's key, so a plain env table makes the next read look
     unwritten.
   - This produced a real false positive on the first run (`elseif::method/a2-nil`).
   - `driver.lua`'s env is an always-empty proxy with `__newindex` and a `written` set.
     One consequence: `pairs(_G)`-style iteration from inside a chunk won't see globals.
3. **Poison every unknown name, not only `slotN`.** `slotN`, `uvN` and any other name each
   get their own class. Otherwise the `-g` control is vacuous, because a dropped named
   local reads as its own name.
4. **Call `jit.off()` in the driver.** Count hooks don't fire inside compiled traces.
5. **Cap events per call.**
   - With 2000 events, `EVENTS_TRUNCATED` is logged, while structure and outcome lines
     always print.
   - The deepcopy round trip recursed on a sentinel until `BUDGET` and wrote a 580 KB trace
     before the cap.
6. **The deepcopy class also yields `REPRO_POISON`** (`slot1`, `slot5`, `slot6` unwritten
   in `deepcopy_global`).
   - Phase 2b can't separate it from the dropped-assignment class by verdict alone.
   - It needs a structural signature: the hoisted `return { [k] = f(v) }` whose table
     register is never assigned.
7. **Correct the rationale for the main-chunk exemption.**
   - Phase 2a's Design says to exempt main-chunk-written names because the original's
     file-scope locals become main-chunk `slotN` globals, which functions then read
     legitimately.
   - That's not what ljd does. A function reading a captured file-scope local prints it
     as **`uvN`**, not `slotN`. `copy_it` prints `return uv0(slot0)`.
   - In the round trip that is a real break (a call on nil), not a naming nuisance.
   - So the exemption only protects the main chunk's own reads. `UPVALUE` hits in the June
     sweep are real round-trip defects to triage, not noise.

**Leads recorded for later phases (not acted on):**
- **Kenta count, for Phase 2b.** The June decompile of `ATTACK_A_ATTACK_DASH_STATE_START`
  has **3** folded registers (`slot3` index, `slot4` target user, `slot5` start pos), and
  the fixed port file restores all three. `MODS.md` says 2.
  - The fixed file also no longer has June's `GetStoredGUUser()` fallback for `slot4`.
    Whether that is a deliberate port edit is for Phase 2b/6.
- **`uvN` is latent in the port.** Zero `.src.lua` files under `march2014/` contain a
  `uvN` name. The `UPVALUE` class matters for the June sweep (DamageEffect), not the
  existing port.
- **The first named nested-frame clobber, for Phase 2a/5.**
  - `deepcopy_global` with its writes restored the no-`local` way (`slot1 = {}` …
    `return slot1`) still fails on a nested input: `global/nested` returns
    `{["c"]=2}`, the *inner* table.
  - The recursive call re-sets the global `slot1`, and the outer frame's later
    `return slot1` reads the callee's value.
  - `global/flat`, which doesn't recurse, is PASS.
  - This is concrete evidence against `MODS.md`'s "safe within one function's own
    sequential statements" whenever a function calls a same-file function that writes the
    same `slotN`. It is the first data point for the `local slotN` decision, not a census.

---

## Phase 2a — Differential harness: runtime

**Paste with:** Starting context + this section + "Phase 0 result" + "Phase 1 result".

**Model:** Opus, high.

**Goal.** Run any June file's original bytecode and its round trip under identical,
deterministic, recorded conditions.

**Design.** Everything runs in the vendored `luajit.exe` on `0x81` bytecode.
- **Inputs, per file.**
  - **Original:** June → `mar-2014` via Phase 0's `ljconv` mode.
  - **Round trip:** June → corrected decompile (`ljd_jun2017.py`, or Phase 0's no-sibling
    path) → `compile-batch -v mar-2014` on a copy in `tmp/`.
  - **Optional third input:** any `.src.lua` compiled the same way. That covers a
    `march2014/` port file, or the pre-fix Kenta from `git show`.
- **Sandbox.** Each chunk runs under `setfenv` in its own recording environment.
  - Running the main chunk defines the file's functions as globals.
  - Every global write the main chunk makes is recorded, so table-only files are covered by
    this step alone.
  - Any engine global the chunk reads (`g_pX2Game`, enum tables, `D3DXVECTOR3`, …) is served
    by the mock, and its read is recorded.
- **Driver.** For each file-defined function, and for each seed 1..N:
  1. Restore a snapshot of the environment.
  2. **Poison the `slotN` globals.** Two limits apply:
     - Poison only names the **main chunk did not write**. In the round trip, the
       original's file-scope locals become main-chunk `slotN` globals, so a legitimate
       read of them must not be flagged.
     - Reads of `uvN` names get their own class, `UPVALUE_READ`. ljd may render a captured
       upvalue that way, which makes it a global in the round trip that the mock would
       otherwise serve silently.
  3. Call the function with mock arguments. Get the arity from `jit.util.funcinfo(f).params`.
  4. Record the outcome: return values, error message (normalised), `POISON_READ`, or
     `BUDGET`.
  - A `debug.sethook` count hook bounds runaway loops and ends the call with `BUDGET`.
- **Superseded in part by "Phase 1 result", "Corrections Phase 2a must adopt".** Detect
  reads through the env's `__index` with explicit write tracking, not stored sentinels.
  Reuse `ljd_oracle/driver.lua`'s env and sentinel.
- **The poison sentinel logs and continues.** It does not raise, so later dropped reads in
  the same call are logged too. One Kenta function has 3.
  - On `__index`/`__call`/arithmetic, the sentinel logs `POISON_READ slotN` and returns a
    mock value. Lua 5.1 does try arithmetic metamethods on mixed operands.
  - Order comparisons (`<`, `<=`, …) against a number **cannot** be intercepted. Lua 5.1
    raises before any metamethod runs. A sentinel read in that context ends the call. Log it
    as `POISON_READ slotN (fatal)`.
  - Phase 2b's fix-rerun loop recovers the reads hidden behind a fatal one.
- **Mock objects.** An object's `__index` returns a method closure. The closure records
  `(receiver path, method, args)` and returns a **deterministic** value, a function of
  `(seed, receiver path, method, per-method call count)`.
  - The value is chosen by the type oracle below: a mock object, number, boolean, string,
    or `nil`.
  - `nil` is allowed with some probability, so that `~= nil` branches get explored.
  - `math.random` and engine random calls are seeded the same way.
- **Type oracle.** Lua 5.1 raises on table-vs-number comparison and arithmetic, so a mock
  object returned where a number belongs aborts every `GetNowHP() <= x`.
  - **Learn types adaptively, on the original side only.** When the original raises
    "attempt to compare/perform arithmetic on a table value", attribute the error to the
    last mock value involved, retype that method to number, and retry.
  - **Persist the learned types** to `june2017_re/ljdiff/types.tsv`, shared across files.
  - **Freeze them, then run both sides with the same table.** Types learned on one side only
    would put noise into the diff.
  - Methods whose names match a `lua_tinker::class_def` registration in `X2Lib`/`KTDXLIB`
    (about 1,630 sites) can be spot-checked against the C++ return type. That is a
    cross-check, not the source.
- **Nested-frame clobber signal.**
  - Wrap every file-defined function so the harness keeps a frame stack.
  - On the round-trip side, where `slotN` is a global, record any `slotN` a callee frame
    writes that its caller reads afterwards without rewriting it first.
  - Emit it as `CLOBBER caller callee slotN`. This is the evidence for the `local slotN`
    decision.
  - **Engine re-entry is the realistic case, and a plain mock can't see it**, because the
    mock never calls back into Lua.
    - Find which `class_def`-bound methods reach a Lua call in `X2Lib`/`KTDXLIB`
      (`lua_tinker::call`, `DoString`/`DoMemory`, a state-function dispatch such as a
      `StateChange` that runs the new state's Lua start function).
    - List them in `june2017_re/ljdiff/reentrant.tsv`, with the C++ site for each.
    - Have the mock re-enter for those methods by calling the named file-defined function,
      when the argument names one.
    - If that proves too large for this phase, build only the direct-call signal. Then write
      **"the clobber census covers direct calls only; engine re-entry is not simulated"**
      into the Phase 2a result. Phase 5 must repeat that caveat to the user, so the
      `local slotN` decision isn't made on an undercount.

**Outputs.**
- `june2017_re/ljdiff/ljdiff.lua` (the runtime)
- `june2017_re/ljdiff/ljdiff.py` (orchestrator: builds all inputs into `tmp/`, runs them,
  writes one JSON trace per side per file into `tmp/`)
- `types.tsv`

**Exit criteria.**
- Both sides of `RAGE_DRYADKENTA.lua` and one table-heavy file (a `data036_kom` registry) run
  end to end with **no harness errors**. The script's own errors are recorded outcomes, not
  harness errors.
- Two identical runs produce byte-identical traces.
- `types.tsv` exists and needs no hand edits.

**Kickoff prompt**:
```
Continuing the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", "Investigation ledger", "Phase 0 result", "Phase 1 result", "Phase 2a"), and
june2017_re/fxdump.lua as the stub-harness precedent. Branch: main.

Your task is Phase 2a: build june2017_re/ljdiff/ (ljdiff.lua runtime + ljdiff.py
orchestrator) per the phase's Design: original (ljconv mar-2014) vs round trip (corrected
decompile -> compile-batch -v mar-2014 on a tmp/ copy), sandboxed with setfenv, a recording
mock with deterministic seeded returns, a type oracle learned on the ORIGINAL side only then
frozen and shared (types.tsv), per-call slotN poisoning (log-and-continue; exempt names the
main chunk wrote; uvN reads are their own class), a count-hook BUDGET, and a nested-frame
CLOBBER signal including simulated engine re-entry (reentrant.tsv) - or, if that is too
large, record that the census covers direct calls only. Done when RAGE_DRYADKENTA.lua and one data036_kom table file run
both sides with no harness errors and two runs give byte-identical traces. Diffing is 2b's
job, not this phase's. Scratch in <Trunk>/tmp/.
```

---

## Phase 2a result

Ran 2026-09-28. All exit criteria met:
- These files run both sides with **zero harness errors**:
  - `RAGE_DRYADKENTA.lua`
  - `data036_kom/MapData.lua` (the registry: 224 engine calls plus two functions, one carrying
    the hand-verified `slot6` defect)
  - `data038_kom/SQUARE_Unit.lua`, the only June file where simulated engine re-entry fires
- `python june2017_re/ljdiff/ljdiff.py --twice`, started from a deleted `types.tsv`, converges
  in 2 learn rounds, and both runs produce **byte-identical** traces (6 traces, a few seconds
  a run).
- `types.tsv` is generated (38 rows, none of them `compare-ambiguous` or `conflict`) and was
  never hand-edited.
- `--selftest` passes all 24 checks.
- **The main-chunk path is proved by a mutation run,** which `--twice` can't prove.
  - Kenta's decompile was copied with `UNIT_HEIGHT` 350 changed to 351, and
    `RAGE_DRYADKENTA_START.SOUND_PLAY0[1]` changed from 0.035 to 0.036.
  - Traced as `--extra mut=…`, it differs from `rt` in `main_values` at exactly
    `INIT_SYSTEM` and `RAGE_DRYADKENTA_START`, and nowhere else. Every call and main event is
    identical.
  - `orig` and `rt` agree on all 56 `main_values`.

**Layout.** `june2017_re/ljdiff/`:
- `ljdiff.lua` is the runtime. It takes one chunk per process, in `learn` or `trace` mode.
- `ljdiff.py` is the orchestrator:
  - `ljdiff.py [FILES]` defaults to Kenta + MapData + SQUARE_Unit. Paths are under
    `14JUN17\data\`.
  - `--seeds N` defaults to 32.
  - `--extra NAME=PATH.src.lua` adds a third side, compiled the same way. That covers a port
    file or the pre-fix Kenta.
  - `--twice` runs the determinism check.
  - `--selftest` runs 21 checks on `selftest/selftest.lua`, with its own types file and
    `selftest/reentrant.tsv`. The checks cover CLOBBER (had/unset), both re-entry kinds, the
    `unsim` count, poison, both learners, the seeded `__eq`, and table-held entry points.
- `types.tsv` is the shared, frozen type oracle.
- `reentrant.tsv` is the engine re-entry census (see below).
- Scratch goes to `tmp/ljdiff_*`, which is deleted and rebuilt on every run. **Don't give other
  files that prefix.** The traces are `tmp/ljdiff_<dir>__<name>.<side>.json`, with sides `orig`,
  `rt` and any `--extra` names.
- **The round trip is unmodified ljd** (Phase 0's `transcode_stock` path) followed by
  `compile-batch -v mar-2014`. Phase 4 has to point `build()` at the fixed decompiler.

**Trace format** (one JSON object per side per file; events one per line, so they diff as
text):
- Header: `label`, `side`, `seeds`, `types_h32`, `load_error`, `restore`, `main_writes`,
  `main_values`, `entries` (`name/arity`, `nested` for table-held functions).
  - **`main_values`** has one line per global the main chunk wrote, taken after the main
    chunk: `NAME #<hash> = <text>`.
    - The text is `fmt`'s rendering, capped at 256 entries per level and depth 8.
    - The hash is FNV-1a over the **uncapped** rendering, so a difference past the cap still
      shows.
    - This is what covers `X = { … }` tables (`INIT_*`, NPC state tables, most of
      `data036`). Registries that go through engine calls show up as main-chunk `CALL`
      events instead.
  - `types_h32` is a hash of the `types.tsv` it ran with. **2b must refuse to diff two sides
    whose `types_h32` differ.**
- `main` and each `calls[i]`: `fn`, `seed`, `arity`, `args`, `outcome`, `fatal`, `poison`,
  `upvalue`, `clobber`, `harness`, `unsim`, `reenter`, `nevents`, `truncated`, `events`.
  - `unsim` is the set of re-entrant methods called that were **not** simulated (see
    "Engine re-entry").
  - `outcome` is `RET …`, `ERROR <normalised message>`, or `BUDGET`.
  - The `poison`, `clobber` and `harness` sets are kept outside the per-call event cap (4,000),
    so a truncated call still reports them.
- Event vocabulary:
  - Mock activity: `CALL recv:M(args) -> ret`, `GET`, `SET`, `GREAD name` (first read of an
    engine global in a call), `EQ <a> <b> -> bool`, `RANDOM`.
  - Unknown reads: `POISON_READ slotN`, `POISON_USE`, `UPVALUE_READ uvN`.
  - Frames and re-entry: `FCALL`/`FRET`/`FERR` (a nested file-function frame),
    `CLOBBER caller callee slotN had|unset`, `REENTER method -> fn`, `REENTER_ERROR`.
  - `HARNESS_ERROR`.

**How it departs from the Design above, and why:**
1. **Compare attribution is exact, not "the last mock value".**
   - Arithmetic, `unm`, `concat` and node-vs-node `<` run the node's own metamethod, so the node
     is known.
   - Node-vs-number `<` raises before any metamethod. For those, the `xpcall` handler reads the
     erroring frame's registers (`debug.getlocal` works on stripped frames) and scans its
     `ISLT/ISGE/ISLE/ISGT` instructions (`jit.util.funcbc`) for the node-vs-number operand pair.
   - One candidate is recorded as `compare`. Several are `compare-ambiguous`, and the newest
     wins. An ambiguous guess later indexed or called as a number is pinned back to `object`
     (`conflict`).
   - The self-test proves the probe's shape: `a = o:A(); b = o:B(); if a <= 3` retypes `A` and
     leaves `B` alone.
2. **Equality needed two mechanisms the Design didn't have.** `==` never raises, so the error
   oracle can't learn from it. The first 12-seed run reached neither arm of `COMMON_FRAMEMOVE`,
   a known defect site.
   - **Node == node:** a seeded `__eq`. It answers true with probability 1/3 from hash(seed,
     sorted pair of paths), is logged as `EQ`, and is never learned from. `NMT.__call` uses
     `rawequal` for the `a:M()` versus `a.M()` test. Otherwise `__eq` would label a call on a
     stale receiver as a method call on the right object.
   - **`x:M() == false/true/7/"s"`:** a static pass over the *original's* bytecode (learn mode
     only). It looks for `TGETS/GGET rA "M"; CALL rA; … ISEQP/ISNEP` (boolean), `ISEQN/ISNEN`
     (number) or `ISEQS/ISNES` (string) on `rA`, stopping at any write to `rA`.
     - The evidence is recorded as `static-iseqp/n/s`.
     - Names the chunk defines itself are skipped.
   - Encodings were checked empirically on a probe: `ISEQP` D is 0 nil, 1 false, 2 true; the
     `TGETS` name is `funck(f, -C-1)`; the `GGET` name is `funck(f, -D-1)`.
3. **Arguments are typed too** (`arg FN#I`). MapData's `math.abs(slot1 - slot0)` needed it.
   - Key kinds: `ret NAME`, `field NAME`, `arg FN#I`, `global NAME`.
   - Types: `number`, `string`, `boolean`, `object`.
   - Keys are **name-only**, shared across files.
4. **Deep restore.** Before every call, the post-main state is deep-copied (functions and nodes
   shared, node state reset), so a call can't leak into the next.
   - Above 400k cells it falls back to shallow restore, recorded in the trace's `restore` field.
   - Both files here are `deep`.
5. **Also built:**
   - functions held in main-chunk tables are entry points too, re-found by key chain
   - `math.random` is seeded and logged
   - the sandbox's `pcall`/`xpcall` re-raise harness control errors
   - nil is returned for 1/6 of untyped method returns (never for args, fields or globals)
6. **Seeds default to 32.** COMMON_FRAMEMOVE's arm 1 needs several independent draws to line
   up. At 12 seeds the original never reached it; at 32 it does (seeds 15 and 28). Runtime is
   unchanged.

**What the traces show.** These are leads for 2b, not verdicts.
- The reorder that the fold causes makes most rows differ before the poison read. Those
  diagnosis rules come from Phase 1.
- Outcomes are the same on both sides unless the table says otherwise.

| Kenta function (32 seeds) | Seeds that differ | Round trip read an unwritten `slotN` | Fatal | Names |
|---|---|---|---|---|
| `…_ATTACK_A_ATTACK_DASH_STATE_START` | 32 | 32 | 0 | `slot3`, `slot4`, `slot5` |
| `…_ATTACK_B_WAIT_FRAME_MOVE` | 10 | 10 | 0 | `slot5`, `slot6` |
| `…_COMMON_FRAMEMOVE` | 12 | 4 | 4 | `slot3` |
| `…_SPECIAL_ATTACK_FIRE_FRAME_MOVE` | 32 | 28 | 0 | `slot3` |
| `…_SPECIAL_ATTACK_FIRE_EMOTION_FRAME_MOVE` | 32 | 28 | 0 | `slot3` |
| `…_ATTACK_B_READY_STATE_START` | 27 | 0 | 0 | - |
| the other 7 (`CF_*`, `MovePos`, 5 `…_STATE_START`) | 0 | 0 | 0 | - |

- **`DASH_STATE_START` reads all three registers** that Phase 1's count lead named (`slot3`,
  `slot4`, `slot5`), not the 2 in `MODS.md`.
- **The softlock itself is visible.** On `B_WAIT_FRAME_MOVE` seeds 3, 27 and 30, the round
  trip calls `PlayEffectSetWithCustomPos_LUA` on `a2:GetVector_LUA()#2` (the stale vector)
  instead of `a1:GetEffectSet()#1`.
  - That call involves no poison read of its own: it is a receiver-path difference. The same
    calls also carry an unrelated `POISON_READ slot6` from the `RandVal` elseif.
  - On the `GetNearestUserUnit` path (seeds 16, 25, 32), `slot5` is a `POISON_READ` instead,
    and every use of it is logged.
- **COMMON_FRAMEMOVE.** Arm 1 is reached on the original, and the round trip's `elseif` is
  fatal on `slot3 <= 0.2`.
  - **The original's `elseif` body is unreached at 32 seeds.** It needs flag1 true, flag2 false
    and a ratio ≤ 0.2 at once, about 2–3% per seed.
  - Report that path as unreached. Don't add exploration heuristics for it.
- **`B_READY_STATE_START` is a reorder-only divergence:** the same events and the same outcome.
  - ljd's rebuilt `GetLineMap():GetStartPosition(GetVector_LUA(0).x).y == …` evaluates
    `GetVector_LUA(0)` after the second `GetLineMap()`.
  - 2b needs a reorder-only class for this, kept apart from the dropped-assignment class.
- **MapData `ComputeCOBOExpressTicketCost`:** the round trip reads `slot6` on all 32 seeds, and
  every one then errors with `bad argument #1 to 'abs'` where the original returns a number.
  That is the hand-verified defect. `CheckCOBOExpressTicketMapID` never differs.
- **CLOBBER is 0 in both files** because neither file has a file function that calls another.
  The self-test proves the signal:
  - `CLOBBER outer_had inner slot1 had`
  - `CLOBBER outer_unset inner slot1 unset`, where the callee's write masks what would have
    been a poison read
  - `CLOBBER user cb slot3 had` through a simulated re-entry

**SQUARE_Unit.lua (32 seeds, 55 entry points):**
- Simulated re-entry fires **375 times** (`StateChange_LUA -> FUNC_STATE_START_*`).
- The round trip reads the unwritten **`uv0` in `FUNC_FRAME_MOVE_RIDING_DASH`** on all 32
  seeds. This is a captured file-scope local printed as `uv0` (`RUN_SPEED * 1.3 * uv0`), the
  class Phase 1 predicted. It is a 2b lead.
- It reads the unwritten **`slot4` in `FUNC_EVENT_PROCESS_EMOTION`** on 15 seeds, also a
  2b lead.
- **CLOBBER is 0 even with re-entry live.** The callers change state last and read no
  `slotN` afterwards.

**Engine re-entry: `june2017_re/ljdiff/reentrant.tsv`.**
- **How the census was built.** It is a census of `X2Lib`/`KTDXLIB` preprocessed with `cl -E`
  under the US_SERVICE flags, so disabled `#ifdef` blocks and comments are excluded: 76 live
  `lua_tinker::call` sites, 1,438 compiled `class_def` bindings, and 5 `def` bindings.
  - Caller chains came from a name-based graph, 4 levels deep, and every chain was then read
    by hand.
  - It has 120 rows: **41 synchronous, 24 deferred, 12 unclear, 43 not-bound**, each with its
    C++ sites.
- **The `sim` column says how the mock handles each synchronous row. Exactly one is
  simulated.**
  - **`CX2SquareUnit::StateChange_LUA`** runs the new state's `FUNC_STATE_START`
    synchronously, and the old state's `FUNC_STATE_END` first (`X2SquareUnit.cpp:2320`,
    called as `(g_pKTDXApp, g_pMain, game, this)`).
  - `CX2GUNPC` binds the **same Lua name** and defers it: it only sets `nextState`
    (`X2GUNPC.cpp:8437`).
  - The mock can't see the receiver's class. So it resolves the target through the state
    table's `FUNC_STATE_START` field, and stays silent when the field is absent.
  - Of the 7,328 June files, only `data038_kom/SQUARE_Unit.lua` has that field, and no file
    has both it and `LUA_STATE_START_FUNC`. NPC scripts therefore never re-enter, and Kenta's
    traces show 0.
  - `FUNC_STATE_END` runs only when this call already changed the receiver's state, because the
    state at call entry is unknown.
- **The other 40 synchronous rows are `none:<why>`.** Calls to them land in the per-call
  `unsim` set:
  - **24 `file-chunk`:** another file runs. This covers:
    - `UI_Control_Sound.lua` on every `Create*` control and scroll-bar init
    - message-box dialog files
    - `IncludeLua`, `SetLineMap`, `SetWorldCamera`, `PlayWorldCamera` and `SetSlotData`
  - **13 `stored-ref`:** the `CKTDGUIButton` setters, which fire a stored `SetCustomFunc*` name
    only while the button is hovered or pressed.
  - **1 `own-lua-thread`:** `PetActionCheat`. Pet state functions run on the pet's own
    `m_LuaManager` thread (`X2Pet.cpp:2742`).
  - **1 `cross-file`:** `ReInitAtkAndDef_LUA` calls `CalculateExtraStatValue` in
    `NewNPCStatFormula.lua`. That is our `SERV_IRUHADEV_EXTRA_NPC_STAT` code, not the studio's.
  - **1** is the `FUNC_STATE_END` row, folded into the simulated row.
- **The caveat Phase 5 must repeat to the user.** Say this, not "direct calls only":

  > The clobber census covers direct calls between same-file functions, and engine re-entry
  > through `CX2SquareUnit::StateChange_LUA`. It does **not** cover:
  > - re-entry that runs another file: the 24 `file-chunk` rows, including a dialog file's
  >   main chunk, whose round-trip `slotN` are globals in the same state
  > - stored-ref callbacks
  > - the 12 `unclear` rows (combo/slider/check-box event handlers, `SendGameMessage` with
  >   its 4th argument true, direct `CX2GUUser::StateChange`, text `.tet`/`.xet` loads)
  >
  > The traces' `unsim` sets name which of these each call reached, the `unclear` rows
  > included. They are keyed by method name alone, so they **overcount**: `SetString` and the
  > texture setters are bound on more classes than `CKTDGUIButton`.

  All three files here have `unsim` = 0.
- **Phase 5 lead.** `ReInitAtkAndDef_LUA` → `CalculateExtraStatValue` is the one synchronous
  cross-file path in the user's own build (ExtraNPCStat is on by default).
  `march2014/update/data036/NewNPCStatFormula.src.lua:9` writes a no-`local` `slot8 = 0`,
  which is a global write in the shared state, made in the middle of the NPC script's call.

**Limitations 2b and the sweeps must carry:**
- **Name-only type keys.** Two classes with a same-named method that returns different types
  collide. The first learned type wins.
- **Booleans are learned only from `x:M() == true/false`** within 64 instructions of the call.
  - A plain truthiness test (`if x:IsFoo() then`) gets a node (truthy) or nil (1/6).
  - Node-vs-number `==` is always false unless the static pass typed the method.
- **`compare-ambiguous` is a heuristic.** Its rows are labelled in `types.tsv`.
- **Only global functions are wrapped.** A table-held function called through its table from
  script gets no frame and no CLOBBER. As an entry point it is called directly.
- **One chunk per run.** Cross-file interaction isn't simulated in either direction:
  - calls into another file's functions
  - another file's main chunk running mid-call
  - the shared-state effects of the client loading every script into one `lua_State`
- **Determinism is shown for these two files only.** `pairs()` over a table with table keys
  hashes by address, which can vary between processes. If a later `--twice` differs, look
  there first.
- **The per-call event cap is 4,000; the main chunk's is 400,000.** Beyond them, `truncated`
  is set, and the poison, clobber and harness sets stay complete.

---

## Phase 2b — Differential harness: diff and calibration

**Paste with:** Starting context + this section + "Phase 1 result" + "Phase 2a" (Design only).

**Model:** Opus, high.

**Goal.** Turn two traces into verdicts, and prove on the known cases that the verdicts are
right before anything is swept.

**Work.**
1. **`ljdiff.py diff`.** Per function and per seed, report the first differing event on each
   side, with context. Classify each divergence:
   - **`dropped-assignment`**: a `POISON_READ` on the round-trip side only
   - **`other-ljd`**: any other semantic divergence, for example the `deepcopy` hoist
   - **`clobber`**: from 2a's signal
   - **`unreached`**: the lint flags the function, or it has a known site, but no seed
     executed that code
2. **Coverage.** Report distinct traces per function over N seeds. Choose N by showing that
   raising it stops finding new traces on Kenta. Report every function whose coverage is 1
   trace, so a single-path function isn't mistaken for a verified one.
3. **Fix-rerun loop.** A `(fatal)` poison read hides every later read in the same call.
   1. Patch that one site in a `tmp/` copy of the round-trip source by restoring the
      assignment the June trace shows.
   2. Recompile and rerun.
   3. Repeat until the function has no `POISON_READ`.
   - Each iteration is one confirmed instance. The count from this loop is the one
     calibration uses.
4. **Static complement: a must-define pass over the recompiled bytecode.** In the round
   trip, every `slotN` is a named global. So a CFG pass over its bytecode can list every
   `GGET "slotN"` that some path reaches without a prior `GSET "slotN"` in the same
   function. Reuse `ljconv`'s dump walker.
   - The result is exact per-path candidates that don't depend on coverage.
   - It turns `unreached` into a checkable list. A static candidate the harness never
     reached is `static-only`; one it did reach and confirmed is `dropped-assignment`.
   - Excluded: names the main chunk writes, and `uvN`.
5. **Calibrate on Kenta, three comparisons:**
   - **June original vs fresh round trip.** Every one of the 5 known functions must show a
     `dropped-assignment` divergence.
   - **June original vs the pre-fix port** (`git show 857dfd7^:…`). The same sites must
     diverge.
   - **June original vs the fixed port** (working tree). Those sites must be gone.
     Everything left is an intentional port edit; list each one.
   - Then **reconcile the count**: the 8 comment sites vs MODS.md's "6 instances" vs the
     per-function figures. Use the fix-rerun loop's count and the static pass's list, not
     first-divergence alone. Write down the true number and where each one is.
6. **Calibrate on MapData.** `ComputeCOBOExpressTicketCost` must show `dropped-assignment`.
7. **Noise check.** Pick a file where the lint reports zero hits and its decompile looks
   trivially correct. The harness must report zero divergences on it. Otherwise the mock or
   the type oracle is producing noise: fix the harness, not the verdict.
   - The file **must have file-scope state**: main-chunk locals read by its functions. That
     way the check exercises the poison rule's main-chunk exemption.
8. **Differential lint check.** Compare the lint's hits on Kenta and MapData with the
   harness's verdicts, and record precision and recall. This says how far the lint alone
   can be trusted when the harness isn't available.

**Outputs.**
- The `diff` mode.
- A **calibration table** added to the Investigation ledger of this plan, covering the
  Kenta reconciliation, MapData, the noise check, lint precision and recall, and the chosen N.

**Exit criteria.** All three Kenta comparisons behave as stated, MapData reproduces, and the
noise check is clean.

**Kickoff prompt**:
```
Continuing the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", "Investigation ledger", "Phase 1 result", "Phase 2a" Design, "Phase 2b") and
MODS.md's "Dryad Kenta (Burned Forest boss)" entry. Branch: main.

Your task is Phase 2b: add a diff mode to june2017_re/ljdiff/ljdiff.py that classifies
divergences (dropped-assignment / other-ljd / clobber / unreached / static-only) with
per-function coverage, a fix-rerun loop for fatal poison reads, and a static must-define pass
over the recompiled bytecode (GGET slotN reachable without GSET), then calibrate: Kenta's June original vs (a) fresh round trip, (b) the pre-fix port
from `git show 857dfd7^:march2014/update/data037/RAGE_DRYADKENTA.src.lua`, (c) the fixed
working-tree port; reconcile the true instance count (8 comment sites vs MODS.md's "6") from
the fix-rerun loop and the static pass;
reproduce MapData's ComputeCOBOExpressTicketCost; prove zero divergences on a trivially clean
file that has file-scope state; measure lint precision/recall. If the harness is noisy, fix the harness, never the
verdict. Write the calibration table into the plan's Investigation ledger.
```

---

## Phase 2b result

Ran 2026-09-28. All exit criteria met: all three Kenta comparisons behave as stated, MapData
reproduces, and the noise check is clean. The numbers are in the Investigation ledger's
**"Calibration"** section. `python june2017_re/ljdiff/calibrate.py` regenerates all of them
and exits 0. `--twice` is still byte-identical, and `--selftest` passes 42 checks.

**Layout added to `june2017_re/ljdiff/`:**
- `ljdiff.py`
  - `--diff`: after tracing, diffs orig against every other side and writes
    `tmp/ljdiff_<flat>.<side>.diff.txt`.
  - `diff A.json B.json [--src S]` diffs two existing traces.
  - `fixloop FILE --fixes F.json [--seeds N]` runs the fix-rerun loop.
  - `--lines` adds per-call line coverage.
  - `--no-twin` traces the stripped chunk.
  - A run now cleans only the `tmp/ljdiff_<flat>.*` files of the files it was given, not
    every `ljdiff_*` file.
- `calibrate.py`: the whole calibration, with its own report.
- `bcfn.lua`: `luajit bcfn.lua CHUNK FN…` lists named file functions' bytecode (cwd
  `elstools/vendor/luajit`). Every fix cites this listing.
- `fixes/RAGE_DRYADKENTA.json`, `fixes/MapData.json`: the curated restorations, each with
  its bytecode evidence (pc ranges from the original).

**What the diff does:**
- **Verdicts** are assigned per (function, seed), strongest first: `harness`,
  `dropped-assignment`, `clobber`, `upvalue`, `reorder`, then `other-ljd` (rt) or `other`
  (an `--extra`, which may be a port edit), then `pass`.
  - `dropped-assignment` = a `POISON_READ` or a `SHADOW_READ` on the side.
  - `reorder` = the same event multiset and outcome, in a different order.
  - Any unknown read on the *original* side is `harness`, never a verdict about the
    decompile.
- **Diff refuses to run on a `types_h32` mismatch.**
- **Main-chunk values.** The side's main-chunk `slotN` globals are listed apart as
  `slot_globals`: they are its own locals, not a difference.
- **Coverage:**
  - `cov_o`/`cov_s` count distinct path signatures. A signature is the call's events and
    outcome with numbers and mock strings masked.
  - The report lists every function with coverage 1.
  - Line coverage (`--lines`) is the saturation measure.
- **Static must-define pass** (`ljdiff.lua`, trace mode, chunks with line info):
  - Per prototype: a basic-block CFG from `jit.util.funcbc`, then a forward
    intersection-of-predecessors analysis over `GSET "slotN"`.
  - A row is written per `GGET "slotN"` that some path reaches unwritten, and per
    `GGET "uvN"`.
  - Rows are tagged `main-written`, `nested` or `upvalue`, never dropped.
  - Diff matches rows to the dynamic read sites as `confirmed`, `static-only (executed)` or
    `static-only (unreached)`. It also lists dynamic sites the pass missed; there were none in
    any calibration file.
- **Line info comes from a `luajit -bg` twin** of each compiled side. compile-batch's output
  is byte-identical to `luajit -b`'s (checked).
  - `twin_check()` proves every prototype's header, bytecode, upvalues and constants equal
    the stripped chunk's on every build.
  - The calibration shows the stripped and twin traces are identical once the `@L`
    annotations are dropped.
- **The fix-rerun loop**, one step per fix in the curated list:
  1. Apply the fix to the fresh decompile and rerun both sides (the ladder).
  2. Record what the harness showed before the fix.
  3. Fail if any *other* function's line-free per-seed trace hash moved.
  - Then **leave-one-out**, from the all-fixed text: revert each fix alone. A fix is an
    instance iff its function diverges without it. The static sites left in that state are
    the ones it owns.
  - The loop must end with every fixed function `pass` on every seed, and the dropped fixes
    accounting for every static site.
  - Fixes are derived from the original's bytecode, **never from the port file**. The fixed
    port carries port edits.

**Harness fixes found by calibration.** The rule was "fix the harness, never the verdict".
1. **The mock's values were not independent** (`ljdiff.lua` `hval`).
   - FNV-1a keeps parity through its odd-prime multiply, and `path()#c|c` carries the count
     twice. So every call to one boolean method gave the same answer within a seed, and the
     same 1/6 nil draw.
   - COMMON_FRAMEMOVE's `elseif` needs flag 1 true and flag 2 false. It was therefore
     unreachable at **any** N: 0 of 128 seeds. 2a's "~2–3% per seed" was really 0%.
   - Values now go through murmur3's `fmix32`, checked against a Python reference.
   - A selftest (`two_flags`) passes with the fix and fails on a copy of the runtime without
     it.
   - Afterwards the branch is reached on 3/32 and 10/128 seeds.
   - **Every per-seed count in the "Phase 2a result" table predates this fix and is
     superseded** by the ledger's. The functions and names it lists are unchanged.
2. **A nil key on a mock table was a harness error.** `ENUM[nil]` should read nil, as in
   Lua. It hit `EVENT_SHADOW_MIRROR_FD.lua` (91 errors while learning).
   - Reads now give nil, and writes raise Lua's own message.
   - Covered by selftest `nil_key`.
3. **`SHADOW_READ` was added.** A call's read of a `slotN` that only the main chunk wrote is
   a dropped write masked by a main-chunk temp. It is `dropped-assignment`.
   - Not seen in any calibration file. Covered by selftest.

**Leads for later phases (not acted on):**
- **Phase 3.** Kenta adds two shapes Phase 1's cases don't have. Both should be pinned as
  oracle cases before the fix:
  - BW-3: the write is folded into a **field-set target**. `slot5:GetPos().y = …` hides
    `slot4 = slot5:GetPos()`.
  - FI-1 and friends: the first operand of an `and`, **computed before** a second
    assignment statement, is folded past it (`slot4 = GetEffectSet()` then
    `if slot3 ~= nil`).
- **Phase 4.** The fix must not break the `order` and `structure` classes, and it isn't
  expected to repair them:
  - FI-O and BR-O are single-use temps inlined past another call.
  - MD-S is an `if`/`else` rebuilt as `(A and B or C) and D`, which evaluates to `D`
    whenever `A and B` is false.
  - Acceptance should count them separately: "no `dropped-assignment`" is achievable, while
    "no `other-ljd`" may not be. MD-S is a real value bug in the round trip, not a reorder.
- **Phase 5/6.** The fixed Kenta port still carries FI-O and BR-O (getter-only reorders;
  probably harmless in-engine) and the two 09-18 port edits. MODS.md's "Six instances" is
  wrong. The true count is 12 in 5 functions (8 comment sites). Correct MODS.md when Phase 6
  touches this file.
- **Phases 7/8.**
  - `data034_kom/DLG_Tournament_MatchList.lua`'s **main chunk** is `other-ljd`. ljd hoisted
    `OUTLINE_COLOR = D3DXCOLOR(0,0,0,0)` into `slot3 = …` ahead of the table constructor, so
    the two `D3DXCOLOR` calls swap. It is harmless in-engine, and it is the first main-chunk
    divergence seen.
  - `EVENT_SHADOW_MIRROR_FD.lua`'s `CheckAbleSummon` has a real `static-only (executed)`
    candidate: `slot3` read after a loop that may not run.
  - `DLG_LUCIEL_TRANSFORM_SKILL_SLOT.lua` is a lint-clean `upvalue` break on every seed.
- **The deepcopy-hoist separation (Phase 1 correction 6) is not built.**
  - Neither Kenta nor MapData exercises it.
  - The sweeps must carry it as a known classifier gap. A deepcopy-shaped `POISON_READ`
    currently scores as `dropped-assignment`.

---

## Phase 3 — Root cause in `slotworks.py` (identification only)

**Paste with:** Starting context + this section + "Phase 1 result" + the calibration table.

**Model:** Opus, high. Use max only if two code paths both fully explain the repro.

**Goal.** Name the exact code path that drops the assignment, and explain why stripping
matters, using the minimal repro from Phase 1.

**Work.**
1. **Instrument a copy.** Copy ljd into `tmp/` and never touch the vendored copies. Log every
   elimination: `slot#slot_id`, the reference count *including skipped `id == -1` references*,
   the holder, `safe_mode`, and which caller made the decision.
2. **Find the decision.** Run the minimal repro through the copy and find the one elimination
   that removes the flagged assignment.
3. **Test the three leads in the ledger**, in order. Also confirm with a log line that the
   unsafe extra pass (`main.py:415-448`) never runs on stripped input.
4. **Explain the stripped vs `-g` difference** from Phase 1 in terms of the found path.
5. **Check upstream.** Find the ljd lineage's later history (the `znixian/luajit-decompiler`
   fork cited in `slotworks.py`, and its successors). Record whether this path was changed
   upstream and how. Reading public repos is fine. Don't vendor anything in this phase.
6. **Prove causality with a scratch experiment.**
   - Make the smallest edit to the `tmp/` copy that makes the minimal repro pass.
   - Run `run_oracle.py` against it, and the Kenta comparison.
   - This is evidence, **not the fix**. Record it, including anything it breaks, such as
     controls that are no longer inlined.

**Outputs.**
- A "Root cause" section in `RANOX_LUA_DECOMPILE_BUG.md`, covering:
  - file and line
  - mechanism
  - why only stripped bytecode is affected
  - the minimal repro
  - the upstream status
  - the scratch-experiment result
- A "Phase 3 result" pointer in this plan.

**Exit criteria.**
- A causal chain from a specific line to the repro's `POISON_READ`.
- The scratch edit flips the repro.

**Kickoff prompt**:
```
Continuing the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", "Investigation ledger" incl. the calibration table and the three slotworks leads,
"Phase 1 result", "Phase 3"), then elstools/vendor/ljd/main.py:355-450 and
elstools/vendor/ljd/ljd/ast/slotworks.py. Branch: main.

Your task is Phase 3, identification only: copy ljd into <Trunk>/tmp/, instrument every
elimination decision (incl. skipped id==-1 refs), and find the exact path that drops the
assignment in Phase 1's minimal repro. Test the ledger's three leads in order; confirm by log
that main.py's unsafe extra pass never runs on stripped input; explain why -g suppresses it;
check the upstream fork history. Prove causality with a scratch edit in tmp/ against
run_oracle.py and Kenta, but do NOT ship a fix. Never edit either vendored ljd copy. Write a
"Root cause" section into RANOX_LUA_DECOMPILE_BUG.md.
```

## Phase 3 result

Ran 2026-09-28. Both exit criteria are met:
- The causal chain from `slotworks.py:285-287` (fed by `:857-861`) to the minimal repro's
  `POISON_READ` is logged step by step.
- A scratch edit flips the repro.

The full write-up is in **`RANOX_LUA_DECOMPILE_BUG.md`, "Root cause"**. No fix was shipped,
and neither vendored ljd copy was touched: both are still `diff -rq` clean against each other.
The only tracked files changed are this plan and the bug doc; `types.tsv` is untouched.

What Phase 4 must take from it:
- **One defect, four sites.** Each site is a collection narrower than the value's live range:
  - (a) the `:286` skip of cross-block (`id == -1`) references in the `main.py:380` pass
  - (b) `_recheck_unsafe_cases`, which re-collects one block at a time
  - (c) `unwarper.py:2521` `_cleanup_ast` over structured code, where an unresolvable read is
    dropped silently (`reaching=[]`)
  - (d) the same call on a sub-tree, whose outside reads are never visited
- **Closing one site hands the value to the next.**
  - Fixing only `:286`, or only reverting upstream bcb0aff9, leaves the minimal repro
    unchanged: (b) re-inlines it.
  - Scratch E1+E2+E3, which covers (a)–(c), leaves BW-3 to (d).
- **Attribution.** In the unfixed decompile, 11 of Kenta's 12 are dropped at site (a) and CF-1
  at site (c).
  - No other inline in the file shows either signature `tmp/p3_folds.py` looks for: skipped
    `id == -1` references, or reads lost within the same collection.
  - **That detector is blind to site (d)**, where the other reads are never visited. So it is
    not a complete census. The harness is what found nothing beyond the 12.
- **Scratch numbers.**
  - Oracle `--expect fixed` misses fall from 29 to 1 (`bwait/nolm-near` = BW-3), with no
    control, `-g` or deepcopy row changed.
  - Kenta at 128 seeds: `dropped-assignment` 406 → 0, static candidates 18 → 0, `other-ljd`
    60 → 33, and the text restores exactly 11 writes. BW-3 remains, as `other-ljd` on 4/128.
- **The ledger's other claims:**
  - The `del new_slots[idx]` bug is inert: 0 leaks in 91 recheck blocks.
  - The unsafe extra pass never runs, on stripped input or under `-g`.
  - `_remove_invalid_references` never fires.
- **Upstream has no fix.** The vendored copy equals znixian master, whose last commit is
  2020-05-09. The maintainer's issue #32, "work across blocks", is still open. No fork
  changes these sites.
- **Pin before fixing (Phase 2b lead, still not done).**
  - Add a BW-3 field-set-target case and an FI-1 "computed before a second assignment" case
    to `ljd_oracle`.
  - BW-3 is the case that tells an (a)–(c) fix apart from a complete one.
  - The "Kenta has no `dropped-assignment`" acceptance check doesn't see BW-3. It shows as
    `other-ljd` (stale value, no unwritten read), so Phase 4 must also check
    `ATTACK_B_WAIT_FRAME_MOVE` for `pass` on 128/128.

---

## Phase 4 — The fix, as a tracked hook

**Paste with:** Starting context + this section + "Phase 3 result" (and the Root cause
section) + the calibration table.

**Model:** Opus, high, then Opus `/code-review` on the diff.

**Goal.** A correct fix that the whole toolchain can use, with no third-party file edited.

**Work.**
1. **`june2017_re/ljd_fixed.py`.**
   - `install_fix()` monkey-patches the `slotworks` function(s) identified in Phase 3.
   - It **refuses to install** if the vendored `slotworks.py`'s SHA-256 differs from the one
     it was written against, so drift in the vendored copy fails loudly.
   - Callable from `ljd_jun2017.py` (new `--fixed` opt-in; the default stays unfixed until
     Phase 9) and from a thin stock-bytecode entry point for mar-2014 files.
2. **`june2017_re/ljd_slotworks.patch`.** The same change as a unified diff against the
   vendored `slotworks.py`. It must apply cleanly to both copies. **Don't apply it**;
   whether to is the user's call, noted in the report.
3. **Acceptance.** All five must hold. Together they reject a fix that just inlines nothing:
   - **Oracle.** Every `run_oracle.py` case passes, the controls included. A control that
     stops being inlined is a regression unless Phase 3 showed that inlining it was itself
     wrong.
   - **Kenta.** June original vs *fixed* round trip has no `dropped-assignment` divergence.
     The `other-ljd` count doesn't rise.
   - **Bounded churn.**
     - Re-decompile the ~273 June files that already have `.src.lua` siblings, into `tmp/`,
       both unfixed and fixed.
     - Every textual difference must sit in a function the harness or lint flagged.
     - Total statement-count growth must stay within a bound you state and justify, for
       example "≤ 1 new assignment per flagged site".
   - **Detection still works.** The unfixed decompile still reproduces the Kenta and MapData
     divergences. This proves the oracle can detect the defect and isn't passing vacuously.
   - **Clean files are unchanged.** Phase 2b's noise-check file decompiles byte-identically
     with the fix.

**Outputs.**
- `ljd_fixed.py`
- `ljd_slotworks.patch`
- `ljd_jun2017.py --fixed`
- A "Phase 4 result" section with the acceptance numbers.

**Exit criteria.** All five acceptance checks pass, and the `/code-review` findings are
resolved.

**Kickoff prompt**:
```
Continuing the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", "Investigation ledger" incl. calibration table, "Phase 3 result", "Phase 4") and
RANOX_LUA_DECOMPILE_BUG.md's "Root cause" section. Branch: main.

Your task is Phase 4: implement the Phase 3 fix as june2017_re/ljd_fixed.py (monkey-patch,
refuses to install if vendored slotworks.py's SHA-256 changed), add an opt-in --fixed to
ljd_jun2017.py, and write june2017_re/ljd_slotworks.patch for the two vendored copies without
applying it. Pass all five acceptance checks in the phase (oracle incl. controls; Kenta has no
dropped-assignment divergence and no new other-ljd; bounded churn over the ~273 existing
.src.lua re-decompiled in tmp/; the unfixed decompile still diverges; the noise-check file is
byte-identical). Then run /code-review on the diff and resolve its findings.
```

## Phase 4 result

Ran 2026-09-28. All five acceptance checks pass. The `/code-review` findings are resolved
(see the end of this section). Neither vendored ljd copy was edited, and both are still
`diff -rq` clean against each other. `types.tsv` is unchanged: 45 rows, `types_h32` `e9e20f92`.

**What shipped** (all in `june2017_re/`):
- **`ljd_slotworks_fix.py`: the fix itself.** It is written as `slotworks.py` code and is not a
  module. One text serves both the hook and the patch, so they are the same change by
  construction.
- **`ljd_fixed.py`.**
  - `install_fix()` execs the fix inside `ljd.ast.slotworks`. It refuses to install unless the
    vendored `slotworks.py` has SHA-256 `464b2c45…`. The error says so if the file is the
    patched one.
  - `python ljd_fixed.py <main.py args>` is the stock-bytecode entry point. It prints one
    `ljd_fixed: slotworks fix active …` line to stderr.
  - `--write-patch` and `--check-patch` maintain the patch.
- **`ljd_slotworks.patch`: not applied.**
  - It is generated, not hand-written: 13 hunks. Twelve are small edits inside six replaced
    functions, and the thirteenth is an appended block. Its only deletion is one `elif` that
    becomes `if`.
  - `patch -p1` applies it cleanly to fresh copies of both vendored trees. The patched file
    hashes `2235d0a1…`, which the patch header records.
  - Whether to apply it is the user's call. **Later applied — see "Phase 9 result"'s
    "Post-close update" for when, and the `install_fix()` fix it required.**
- **`--fixed`** in `ljd_jun2017.py` (CRLF preserved), `ljd_oracle/run_oracle.py` and
  `ljdiff/ljdiff.py`. The two harnesses fail a step if a `--fixed` run doesn't report the fix
  active, so a hook that silently failed to install can't pass. The default everywhere stays
  unfixed until Phase 9.
- **New oracle cases.** `expect_now` comes from unmodified ljd; the pins and `sites` all
  reproduce there.
  - `cases/pin.lua`, the two pins Phase 3 asked for: BW-3's field-set target, and FI-1/FI-2's
    two timer blocks.
  - `cases/valuectl.lua`: value-producing `and`/`or` controls, whose assignment counts are
    pinned.
  - `cases/sites.lua`: the three review findings (see the end of this section).
  - `cases/andor.lua` and `cases/gaps.lua`: the second review's regression control and its
    three gaps.
  - The fixed assignment counts of `pin` and `sites` are pinned. On stripped input they equal
    the `-g` build's counts, which is exactly what a correct fix keeps.

**The design: one veto instead of four per-site patches.**
- **Where it acts.** Just before `_eliminate_simple_cases` deletes `slotN = <expr>`, the fix
  asks one question. Does this definition reach, with no write of slotN on the way, a read of
  slotN that the collection didn't account for? If so, the assignment is kept.
- **The whole function.** The question is answered over the enclosing function as it stands at
  that moment, not over the collection's view. That covers:
  - warped blocks, with FORI's zero-trip edge, FORL's index write on the body edge only, and
    ITERC's writes on both edges;
  - structured `if`/`elseif`/`while`/`repeat`/`for`/`break`/`return`;
  - the mix the unwarper leaves between its steps.
- **"Accounted"** means the references `_fill_simple_refs` actually considered. The `:286` skip
  of `id == -1` is therefore exactly what the veto catches.
- **Why this closes all four sites together.** Phase 3's (a) main pass, (b) recheck,
  (c) `_cleanup_ast` over structured code, and (d) a collection rooted at a sub-tree all end at
  this one check.
- **Four refinements the checks forced.** Each is commented in the code.
  1. **Transient states (found by the oracle).** ljd's inline chains pass through wrong
     intermediate states. `obj:Make():Go()` inlines `slot2 = slot1` past `slot1 = slot1.Go`, and
     the method-call conversion makes it right again later. So:
     - an expression an elimination moved is read where it was computed, while that assignment
       is still in the flow;
     - identifiers no longer in the tree read nothing (a deleted method self argument, for
       instance);
     - flows are rebuilt per elimination batch.

     Without this, `ctl_method_on_fresh` came out as `fresh.Go(<function>, 1)`.
  2. **Which function (found by the oracle).** It is the collector's `info.function`, or else
     the collection root's own function. A recheck collects over one block. A root can also sit
     in an enclosing function, since collections descend into nested ones.
  3. **Eliminated assignments still write (found by the churn run).** Every elimination passed
     the check, and its table-constructor refs stay in place until they are folded in. Without
     this there was one false veto (`DLG_UI_RANOX_SECRET_FEVER_GAGE`, `slot13` across a loop
     back-edge), which a later pass happened to undo.
  4. **`unwarper.py:570`'s temporary block is exempt.** Its blocks are deleted right after the
     call, so keeping the assignment there would keep nothing. It occurred 0 times in any run.
- **Fails closed.** The assignment is kept in any of these cases:
  - the check can't place the assignment in the flow;
  - it reaches a stale warp target;
  - it meets a node type it doesn't know.

  A kept assignment is always semantically safe. All three counters were 0 in every run.
- **The mass-assignment rename is guarded too.** `_eliminate_mass_assignments` goes through the
  same check. This fifth site was found in review; see the end of this section.
- **Also modelled (second review):**
  - the ISTC/ISFC copy, as a write on the jump edge only;
  - a named `-g` local reusing the register, which counts as a write;
  - a closure created while it captures the register (`_upvalues`), which counts as a read.
    ljd still prints the captured local as `uvN`, a separate class, so the kept write alone
    doesn't make such a round trip pass.
- **The one deliberate exception: an and/or value part.** `slotN = <part>` ends its block, the
  block's warp tests slotN itself, and every fall-through path rewrites slotN in
  expression-shaped blocks before the join. There the inline into the test is how ljd carries
  the value: its expression unwarper rebuilds `slotN = a and <part> or b` from the conditions,
  and refuses a part block that also holds an assignment (`unwarper.py:899-903`). Counted as
  `andor`. See review 2, finding 1.

**Acceptance.**

1. **Oracle.**
   - `run_oracle.py --fixed --expect fixed`: ALL EXPECTED. That covers every call row,
     including all controls, every `-g` row, both pins, all four value controls, all
     three `sites` shapes, the `andor` controls and the three `gaps`.
     `bwait/nolm-near` (BW-3) is included; Phase 3's scratch edits did not close it. The pinned
     text facts for controls and valuectl are unchanged.
   - The unfixed `--expect now` run is also ALL EXPECTED.
2. **Kenta** (`ljdiff.py --diff --seeds 128`, under `types_h32` `e9e20f92`, read from both traces):

   | | dropped-assignment | other-ljd | reorder | pass | static |
   |---|---|---|---|---|---|
   | unfixed | 406 | 60 | 138 | 1060 | 18 |
   | fixed | **0** | **29** | 132 | 1503 | **0** |

   - The unfixed row equals Phase 3's baseline exactly.
   - All five known functions pass 128/128, including `ATTACK_B_WAIT_FRAME_MOVE`, so BW-3 is
     closed.
   - All 29 remaining other-ljd are `ATTACK_B_READY_STATE_START` (BR-O). The 63 reorder in FIRE
     are FI-O. Both are the known `order` class and are untouched.
   - The fixed text restores all 12 writes, each in its own function. This was checked by
     script against `fixes/RAGE_DRYADKENTA.json`.
   - MapData: 128 dropped-assignment → 256 pass, with MD-1 restored in place. MD-S caveat
     below.
3. **Bounded churn.**
   - **Scope.** 272 June files were re-decompiled, unfixed and `--fixed`, through
     `ljd_jun2017.py` into `tmp/p4c_*`. There are 273 `.src.lua` siblings; `data036_kom/Enum`
     has no `.std.luac`.
   - **Results.** 0 decompile failures on either side. 213 files are byte-identical; 59 changed,
     in 102 functions.
   - **Every changed function is flagged.** 98 are flagged by the lint. The other 4 are flagged
     `dropped-assignment` by the harness on the unfixed side at 128 seeds:

     | Function (file) | Unfixed `dropped-assignment` seeds |
     |---|---|
     | `GetUser` (FIRE_DEVIL_SKA) | 109 |
     | `GEYSER_MULLOCK_ATTACK_B_RUN_STATE_START` | 106 |
     | `ConsumeNPCSummomLimitNum` | 111 |
     | `SMITH_REGIEON_POINT_SUMMON_FRAME_MOVE` | 27 |

     - All four are the lint's known blind spot: a value written on one arm and read after the
       merge.
     - All four pass fixed, except the clobber below.
     - Every changed function also has at least one definition in the veto log. No text changed
       without a veto.
   - **The bound, stated and met.** A changed function's assignment statements grow by exactly
     the number of distinct definitions the fix kept in it: 180 against 180, with no function
     over.
     - Justification: a veto keeps one assignment ljd would have deleted, and adds nothing else.
     - Counting: any statement-level `=` counts. A restored BW-3-shaped write turns the
       uncounted `f().y = v` into `slotN.y = v`, which the oracle's narrower regex would have
       miscounted as growth.
   - **Fix totals.** 408,035 inlines checked, 300 vetoed; exempt, unlocated, stale-target and
     unknown-node all 0.
   - **Neither review's fixes add churn.** After both, a clean solo run is identical: the same
     59 files and 102 functions, 180 = 180, 300 vetoes. The mass-rename guard, fold simulation,
     KNIL, closure-capture and fail-closed paths veto nothing in these 272 files, and the and/or
     exemption fires 0 times. They close the holes `sites` and `gaps` pin, and keep `andor`
     correct.
   - **One run was tainted and discarded.** Two acceptance runs once overlapped in `tmp/`; every
     number here is from the clean solo run after that.
   - **Cost** (clean solo run).
     - On the churn set the largest files are 1.3–1.9× slower: StringID_def 15.4 → 19.6 s,
       NPCTemplet 9.6 → 17.9 s.
     - The corpus's largest file, `data036_kom/EffectSet.lua` (5.97 MB, outside the churn set),
       goes 67.5 → 207.7 s, which is **3.1×**. It gets 2 vetoes, which the harness hasn't
       checked; that is Phase 8's.
     - The flow is rebuilt per elimination batch. Caching per-point read/write sets would
       recover most of this, and Phase 7/8's sweep will want it. It isn't needed for
       correctness.
4. **Detection still works.** The unfixed decompile still diverges:
   - Kenta: `dropped-assignment` 406 with 18 confirmed static sites.
   - MapData: 128 with 1 site.
   - The oracle's `expect_now` rows all still reproduce.
5. **Clean files are unchanged.** All three noise-check files are byte-identical fixed vs
   unfixed, with 0 vetoes in each.

**Hook ≡ patch.** The hooked and patched ljd give byte-identical output on 31 inputs: the 24
oracle inputs and 7 June files (the three noise files, Kenta, MapData, FIRE_DEVIL_IGNIA and
SMITH_REGIEON_POINT). This was re-measured on the final patch, `2235d0a1…`.

**What Phase 5 onward needs from this.**
- **`other-ljd` can rise per file, and that is unmasking.** `dropped-assignment` outranks every
  other verdict, so fixing it exposes what it was hiding.
  - FIRE_DEVIL_IGNIA's other-ljd goes 17 → 150. Of the 150:
    - 97 are a swapped comparison: `GetDist(…EndPos) < GetDist(…StartPos)`. It is already
      present in the unfixed text, and is order class.
    - 36 are seeds where both sides run into the 2M-instruction `BUDGET`, at different events.
      The no-`local` round trip spends more instructions per loop.
    - 17 are unchanged from unfixed.
  - Kenta's "other-ljd doesn't rise" holds. A per-file sweep must classify the rest rather than
    compare totals.
- **First evidence for the `local slotN` decision.** `SMITH_REGIEON_POINT_SUMMON_FRAME_MOVE`
  shows `clobber` on 27/128 seeds when fixed, and passes otherwise.
  - It correctly writes `slot3`, then calls `ConsumeNPCSummomLimitNum`, which now correctly
    writes its own `slot3` global.
  - That is the nested-frame hazard in the ledger, which the dropped write used to hide.
  - It is not a decompile defect.
- **Correction to Phase 2b's MD-S evidence.**
  - "MD-S diverges on 32/32 seeds once MD-1 is in" was measured with MD-1's *text* fix. That fix
    restored the write but left `CheckCOBOExpressTicketMapID(slot0)` folded into the condition,
    so the function called it twice.
  - MD-S itself (the always-`D` `(A and B or C) and D`) is a real structure defect and is still
    in the fixed text.
  - The harness can't reach it: `CheckCOBOExpressTicketMapID` returns ≥ 1 on every seed, so the
    `== 0` branch never runs. MapData's fixed "pass" is therefore vacuous for MD-S.
- **Kept assignments can change the printed structure.**
  - Kenta BW-2: `elseif <call> ~= nil` becomes `else slot5 = <call>; if slot5 ~= nil`.
  - A condition like `A and <call> ~= nil` becomes nested `if`s when the call's result is now
    stored.
  - Both are correct. Expect them in Phase 6 diffs.
- **Where the evidence lives** (scratch, gitignored, may be gone): `tmp/p4_all.sh` reruns
  everything, with one caveat. It restores `types.tsv` from `tmp/p4_types_snapshot.tsv` at the
  end; a run on new files would otherwise grow the tracked table (it learned 7 rows here).
  - Veto logs: `tmp/p4c_*.veto.log`.
  - Per-function churn table: `tmp/p4c_units.tsv`.
  - Kenta/MapData reports: `tmp/p4_{unfixed,fixed}_*.rt.diff.txt`.

**`/code-review` (high) and how each finding was resolved.** Every acceptance check above was
re-run after these changes, with identical results.

1. **Fixed: the mass-assignment rename was a fifth site.** `_fill_massive_refs`'s
   `_remove_invalid_references` drops trailing `id == -1` reads the same way the `:286` skip
   does. `_eliminate_mass_assignments` then renamed the call's destination away.
   - Reproduced by `sites/massive_reuse`: `return slot2, slot4` read an unwritten `slot2`.
   - It is now guarded by the same check.
   - A vetoed rename puts back the `_ids` entries that `_remove_invalid_references` removed,
     since a later pass removes them again. Without that, a later pass crashes on
     `list.remove`.
   - `_eliminate_iterators` is not guarded. Its slots are the for-in generator registers, which
     compiled Lua never reads outside the loop header.
2. **Fixed: an unfoldable constructor element.** `insert_table_record(..., False)` refuses an
   array index that already holds a value. That left `slotN[k] = v` reading a table that had
   been inlined away.
   - Reproduced by `sites/ctor_refold`.
   - The check now simulates the folds (`_ljdfix_folds`, which mirrors the helper's only
     refusing branch) and keeps the assignment if one won't fold.
3. **Fixed: duplicate holders.** A reference the duplicate-holder skip drops is no longer
   counted as consumed. `_replace_node` rewrites only one identifier.
   - Reproduced by `sites/dup_holder`: `t[t]` printed as `slot1[slot0:Get()]`.
4. **Fixed: fails closed.** The assignment is kept when the check can't place it, reaches a
   stale target, or meets an unknown node type (previously it deferred to ljd). All three
   counters are 0 in every run.
5. **Fixed: stderr.** `ljd_fixed.py` prints its stats line only on success, and prints a
   string `SystemExit`. A failed decompile's last stderr line, which is what both harnesses
   report, is ljd's own error again.
6. **Fixed: the origin chain.** A moved expression now keeps every origin, oldest first, and is
   read at the oldest one still in the flow. Before, it was read at the first origin even after
   that one was cleaned up, which counted it twice.
7. **Rejected: an `EndWarp` in a nested list as function exit.**
   - `_set_end` puts an `EndWarp` on the last block of an if/else body. It means "leave this
     body", so it continues after the `if`.
   - A real function end is a `Return` statement, and it has no successors in the flow.
   - Treating the nested `EndWarp` as a function exit would drop real paths.
8. **Fixed: the stale patch hash in this section.** The patch was regenerated after the fixes
   above. The hook-vs-patch comparison was re-run on the final patch (`2235d0a1…`).
9. **Fixed: the pins' fixed assignment counts are pinned** in `expected.tsv`.
10. **Noted, not changed: cost.** See "Cost" under check 3.

**Second `/code-review` pass (high), scoped to the code the first pass added.** Every acceptance
check was re-run afterwards (numbers above are from that run).

1. **Fixed: a regression the fix itself introduced.** `c and <value> or d` (a closure, call or
   constructor as the middle part) came out as an `if` that drops `d`; stock ljd gets it right.
   - ljd carries such a part's value in its block's `IST` test. The expression unwarper
     rebuilds `slotN = c and <value> or d` from the conditions and refuses a part block that
     also holds an assignment (`unwarper.py:899-903`). Keeping the assignment broke the
     expression, and the if-unwarper then mis-structured the CFG.
   - Fixed by the and/or exemption (`_ljdfix_andor_value`, counted `andor`). It needed the
     ISTC/ISFC copy modelled as a jump-edge write, and a named `-g` local counted as a
     register write, or it missed the `-g` build.
   - Pinned by `cases/andor.lua`: it passes unfixed and fixed, stripped and `-g`, and prints as
     stock does.
2. **Fixed: a slot key that a later inline makes an integer** (`gaps/gap_slot_key`,
   `t[k] = v` with `k = 1`). The fold simulation now resolves a register key to its definition
   in the same block, and stays conservative only if it can't.
   - The first version assumed the worst for every register key. That falsely vetoed computed
     data-table keys (`t[NPC_UNIT_ID.X] = ...`) in Dungeon_Ranox_Secret_Hell and FIRE_DEVIL_SKA,
     where ljd then hoisted the kept tables. The churn run caught it, and the resolution removed
     it.
3. **Fixed: KNIL.** `slotA, slotB = nil` deletes every destination, so each destination is now
   checked (`gaps/gap_knil`).
4. **Fixed: closure capture.** A closure created while it captures the register counts as a read
   (`gaps/gap_closure`). ljd still prints the capture as `uvN`, the separate class. Stripped,
   `gap_closure` passes because the harness restores the global it reads.
5. **Fixed: the exemption's scope.** The `unwarper.py:570` exemption now applies only to the
   root's own function, not to functions nested in the deleted blocks.
6. **Rejected: the put-back after the recheck.** The recheck can pin a removed read's id only
   through `_get_slot`, i.e. to a definition earlier in that read's own block. That definition
   is the read's real reaching definition, so the pinned attribution is correct.
7. **Fixed: untested claims.** Both apply routes are now tested on a fresh copy:
   - `git apply` gives the same patched hash as `patch -p1`;
   - the SHA refusal was exercised both ways (patched file: the "already applied" message; one
     byte changed: the generic refusal).

---

## Phase 5 — Sweep stage 1: the ported `march2014/` files, and the `local slotN` decision

**Paste with:** Starting context + this section + "Phase 4 result".

**Model:** Sonnet 5, high. Switch to Opus for any divergence the classifier can't name.

**Goal.** Get a verdict for every ported file, and collect the evidence the user needs to
decide on `local slotN`.

**Work.**
1. **Map originals.** Recount the `.src.lua` files under `march2014/` (277 on 2026-09-27).
   Map each one to its June original by name, across all 21 folders.
   - Record `no-june-original` when there's no match.
   - Record `ambiguous` when there's more than one match, for example a `_HENIR` variant.
   - Use `unverifiable-truncated` from Phase 0 where it applies.
2. **Run through `corpus_run.py` + `ljdiff`, two comparisons per file:**
   - June original vs **fixed** round trip. This validates the fix on real data; it should
     have no `dropped-assignment`.
   - June original vs **the port file** as it stands.
3. **Write `june2017_re/decompile_verdicts.tsv`.** One row per (file, function), with:
   - the verdict (`clean` / `dropped-assignment` / `other-ljd` / `clobber` / `unreached` /
     `static-only` / `port-divergence` / `unverifiable-*`)
   - lint hits
   - coverage
   - first-divergence summary
4. **Clobber census.** Count how many `clobber` events occur, in which files and function
   pairs, and whether any is reachable through an engine callback rather than a direct call.
   - If Phase 2a recorded that engine re-entry is not simulated, **say so to the user
     alongside the numbers**. In that case the census is a lower bound.
5. **Ask the user for the `local slotN` decision.** Present the census with 2–3 concrete
   examples, then use AskUserQuestion.
   - **Keep bare globals:** repairs restore assignments only; clobbers get fixed case by case.
   - **Emit `local`:** repairs declare `local slotN` per function, removing both hazards.
   - Record the answer in this plan's decisions block and in `RANOX_LUA_DECOMPILE_BUG.md`.
6. **Re-split Phase 6.** Base the split on the real `dropped-assignment` + `clobber` volume
   per file, not on lint hit counts. Update 6a–6c's file lists here.

**Outputs.** `decompile_verdicts.tsv`, the clobber census, the recorded decision, and the
revised 6a–6c split.

**Exit criteria.**
- Every ported file has a verdict row.
- The fixed round trip shows zero `dropped-assignment` across the set. Any exception is a
  Phase 4 regression: stop and report it.
- The decision is recorded.

**Kickoff prompt**:
```
Continuing the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", "Investigation ledger", "Phase 4 result", "Phase 5") and RANOX_LUA_DECOMPILE_BUG.md.
Branch: main.

Your task is Phase 5: map every march2014/**/*.src.lua to its June original, and run
corpus_run.py + ljdiff on each: June vs FIXED round trip (must show zero dropped-assignment
anywhere, else stop: Phase 4 regression) and June vs the port file as-is. Write
june2017_re/decompile_verdicts.tsv (per file+function). Produce the clobber census (state plainly if it
covers direct calls only, per the Phase 2a result), then present it and ask me the `local slotN` decision with AskUserQuestion; record my answer in
the plan and in RANOX_LUA_DECOMPILE_BUG.md. Re-split Phase 6a-6c by real verdict volume.
Do not edit any march2014 file in this phase.
```

---

## Phase 5 result

Ran 2026-09-28. Exit criteria met with one caveat resolved below, not a stop: the fixed round
trip shows **zero unexplained `dropped-assignment`** across the whole set, both dynamically (32
seeds) and via the coverage-independent static must-define pass (which a 32-seed sweep alone
cannot prove; see the calibration's "a clean 32-seed sweep is not proof against stale-value
instances"). `march2014/**/*.src.lua` was not edited (`git status --short march2014`: clean).

**Mapping.** 277 `.src.lua` files recounted (matches Phase 0's prior count), matched to their
June original by exact basename (`decompile_corpus.tsv`'s 7,328 rows have 7,328 *distinct*
names, checked, not assumed — so a name match is unambiguous). **276 mapped, 1
`no-june-original`**: `march2014/data040/Field_Sander_Stage3_LineMaps.src.lua` (a Ranox-original
zone with no June counterpart — expected, not investigated further). `ambiguous` and
`duplicate-port-basename` both occurred 0 times. Outputs:
`june2017_re/phase5_map.py`, `phase5_files.tsv` (the mapping, one row per port file),
`phase5_targets.tsv` (the 276 mapped `(dir, name)` pairs `corpus_run.py` swept).

**The sweep.** `june2017_re/phase5_worker.py` (a `corpus_run.py` WORKER: per June original,
builds `orig`/`rt`(fixed)/`port` via `ljdiff` as a library — see the file's own docstring for
why per-file process isolation is about OS-level timeout/resume here, not `ljd`'s module state,
since `ljdiff.py` already shells out to `ljd`/`luajit`/`compile-batch` as subprocesses) plus
`june2017_re/phase5_aggregate.py` (turns the per-file JSON into `decompile_verdicts.tsv`).
- Split into a 267-file batch (<200 KB) and a 9-file batch (≥200 KB, up to `EffectSet.lua`
  5.97 MB / `DamageEffect.lua` 5.42 MB), the latter at `--timeout 1800`. Both **complete, zero
  crashes, zero timeouts, zero harness errors.**
- **A real concurrency bug, found and fixed twice.** `--workers 8` on the first small-batch run
  raced 8 processes' `learn()`/`save_types()` against the one shared `types.tsv` (never
  previously run concurrently — `ljdiff.py`'s own multi-file runs are one process). 27/267
  files hit `ljdiff.py`'s own `types_h32` mismatch guard (`diff_traces` refuses to diff two
  traces built from different type snapshots) and reported `STEP_ERROR`, never a wrong verdict
  — the guard did its job. Rerun those 27 at `--workers 1`: clean.
  - **A second, narrower window the guard can't see**, raised in review: another worker's
    `save_types()` landing between file A's own learn-phase save and its first trace() call
    would make A's `orig`/`rt`/`port` traces mutually consistent (same guard-satisfying
    `types_h32`) but built from a snapshot that skipped A's *own* newly-learned types — a
    silent-verdict risk, not a crash. Handled by discarding the entire 240-file parallel result
    and **rerunning all 267 at `--workers 1`** (the 9-file batch was already serial throughout).
    Diffed old vs new, sorted: **2 lines differ, in one function's coverage count only**
    (`FIRE_DEVIL_SNIKER`/`FIRE_DEVIL_SNIKER_INCHANT`'s `ATTACK_B_READY_JUMP_TO_TARGET`,
    `cov_orig`/`cov_side` 17/14 vs 20/17) — **no verdict changed, the clobber census is
    byte-identical.** The race was real but didn't reach a wrong answer here; `--workers 1` is
    now how this sweep must be run, and is what produced the numbers below.
  - `types.tsv` grew from the calibration's 45 rows to 97, all still keyed by name and shared;
    `types_h32` is no longer `e9e20f92` (that hash is calibration-specific and superseded, as
    "Calibration"'s own MD-S note already warned).

**Verdicts: `june2017_re/decompile_verdicts.tsv`, 666 rows** (one per file+function, plus one
`<main>` row per mapped file so a pure-data file with zero named functions still gets a verdict
— `EffectSet`/`Enum`/`EnumString`/etc. would otherwise be invisible in this table):

| Verdict | Rows | Meaning |
|---|---:|---|
| `clean` | 466 | matches June, fixed round trip and port agree |
| `dropped-assignment` | 121 | the **port** file still has the bug — Phase 6's real worklist |
| `port-divergence` | 55 | port differs from June for a reason that isn't a dropped write (edit, order, or — for `<main>` on table-only files — enum/content coverage gaps already tracked by `JUNE2017_EFFECT_REBUILD_PLAN.md`, not this plan's job) |
| `not-exercised` | 13 | function present in the June/rt trace but never matched against the port (DamageEffect's 13 upvalue-closure helpers — its March/Ranox merge restructures them, not a decompile question) |
| `reorder` | 6 | inherited order-only defects (BR-O/FI-O-shaped), harmless in-engine per Phase 4 |
| `other-ljd (deepcopy, exempt)` | 2 | the **separate**, already-catalogued ljd deepcopy defect (see below) |
| `static-only` | 1 | a static must-define candidate no seed reached |
| `other-ljd` | 1 | DamageEffect's `<main>`, `upvalue`-dominant (captured-closure prints as `uvN`, expected for that file's shape) |
| `no-june-original` | 1 | `Field_Sander_Stage3_LineMaps` |

**62 distinct files carry at least one real `dropped-assignment`** (61 in `data037/` plus
`MapData.src.lua`) — this reproduces the old heuristic lint's 61/774-hit scope almost exactly
(one more file: the lint never flagged `MapData.src.lua` by itself since that was hand-verified
separately), now with bytecode-confirmed, per-function, per-seed detail instead of a regex
guess. Per-file counts are in `decompile_verdicts.tsv`; the two heaviest are `SECRET_MAGI_BOSS`
(11 functions) and `GEYSER_MULSAYRENA` (9).

**The `other-ljd (deepcopy, exempt)` rows are not a Phase 4 regression — verified, not
assumed.** Two rt-side sightings (`SECRET_MAGI_BOSS`/`SECRET_SCENE_DUMMY`'s shared `TableCopy`
helper, and a third, anonymous instance in `EffectSet.lua`'s `<main>`) print the exact
`README.md` "ljd caveat" shape: `slot1 = { [slot5] = <call>(slot6) }` hoisted *before* the
`for slot5, slot6 in pairs(...)` loop that is the only place those registers are actually
assigned. That is `main.py`'s constructor-hoist logic, a different code path from the
`slotworks.py` elimination Phase 4 fixed, and it was already flagged in Phase 1's oracle
(`deepcopy::copy_it`) and Phase 2b's leads as something the sweep would hit and must not
misclassify. `phase5_aggregate.py`'s `deepcopy_exempt_at_line()` confirms the signature
per-site (by line, not by function name, so it also catches the anonymous `EffectSet` instance)
and only then reclassifies away from `dropped-assignment`; every rt-side `dropped-assignment`
that *doesn't* match gets called out as a regression candidate and the script exits 1 — it did,
twice, while this classifier was still being built (see the aggregator's own commit history),
and exits 0 now. **Correction to an assumption made mid-phase:** the port files' own `TableCopy`
is *not* the raw ljd output — both were manually reconstructed on 2026-09-18 (`SERV_IRUHADEV_
RANOX_PORT` comments citing the pseudo-asm evidence) as a plain non-recursive `local`-based
shallow copy. So this defect exists only in the **fixed decompile of not-yet-ported June
content** (relevant to Phases 7/8), never in the live port text — the two named-function
`dropped-assignment` sightings this exemption also covers (`CreateMagiLockOnEffect`,
`SECRET_MAGI_SPECIAL_ATTACK_READY_FRAME_MOVE_FUNC`, both calling into `TableCopy`) show `pass`/
`static-only` on the **port** side for exactly this reason.

**A second regression check, coverage-independent.** Per the calibration's own warning, a
32-seed dynamic sweep cannot rule out a stale-value defect (BW-3 showed on ~3% of seeds).
`phase5_aggregate.py` also walks every rt-side static must-define row with no `main-written`/
`nested`/`upvalue` tag — the coverage-independent list, present whether or not any of the 32
seeds actually took that path — through the same deepcopy signature check. **Zero additional,
unexempted rows** across all 276 files. Both checks (dynamic + static) agree: the Phase 4 fix
holds across this corpus.

**Clobber census: `june2017_re/phase5_clobber_census.tsv`, 3 distinct clobber events across 2
files**, all direct same-file calls (no engine re-entry — this sweep never touched
`data038_kom/SQUARE_Unit.lua`, the only file where Phase 2a's simulated re-entry fires):

| File | Caller | Callee | Slot | port | rt (fixed round trip preview) |
|---|---|---|---|---|---|
| `SECRET_MAGI_BOSS.src.lua` | `SECRET_MAGI_ATTACK_D_READY_START_FUNC` | `GetRandomUserIndexList` | `slot3` | `had`, 20/32 seeds | `had`, 20/32 seeds |
| `SECRET_MAGI_BOSS.src.lua` | `SECRET_MAGI_SPECIAL_ATTACK_COMBO_FRAME_MOVE_FUNC` | `CreateLavaGround` | `slot3` | `unset`, 22/32 seeds | *(not in rt's function set — port-only)* |
| `SMITH_REGIEON_POINT.src.lua` | `SMITH_REGIEON_POINT_SUMMON_FRAME_MOVE` | `ConsumeNPCSummomLimitNum` | `slot3` | `unset`, 3/32 seeds | `had`, 8/32 seeds |

- **`had` vs `unset` matters.** `had` means the caller's own write to the slot survives to be
  clobbered by the callee — the genuine nested-frame hazard. `unset` means the caller never
  wrote the slot at all in that seed, so what looks like "no clobber" is really **a dropped
  write being masked by the callee's own write** — i.e. still Phase 6's problem, not a clobber
  decision at all, until the dropped write is restored. The `SMITH_REGIEON_POINT` row's
  port→rt shift (3 `unset` → 8 `had`) is exactly Phase 4's own finding restated: once the
  caller's own write is present (as it is in the fixed decompile), the clobber becomes real.
  **This is the strongest concrete evidence for the decision below**, and it's the same
  instance Phase 4 already named — this sweep didn't find a new one, it confirmed that one
  is real and quantified it.
- **The census is a lower bound — stated plainly, per Phase 2a's own caveat, quoted, not
  paraphrased:**

  > The clobber census covers direct calls between same-file functions, and engine re-entry
  > through `CX2SquareUnit::StateChange_LUA`. It does **not** cover: re-entry that runs another
  > file (24 `file-chunk` rows); stored-ref callbacks; the 12 `unclear` rows (combo/slider/
  > check-box handlers, `SendGameMessage` with its 4th arg true, direct `CX2GUUser::StateChange`,
  > text `.tet`/`.xet` loads). The traces' `unsim` sets name which of these each call reached,
  > and overcount (keyed by method name alone).

  **Measured, not left abstract:** aggregated `unsim` across every trace in this sweep (all
  276 files, both `rt` and `port` sides) — **zero un-simulated re-entrant calls fired, in any
  file.** So for exactly this corpus, the stated lower bound and the measured floor happen to
  coincide; that is a fact about this sweep's call coverage, not a claim that no such re-entry
  exists in the game.
- **The one cross-file lead Phase 2a assigned here, chased and closed.**
  `ReInitAtkAndDef_LUA` → `CalculateExtraStatValue` (`march2014/update/data036/
  NewNPCStatFormula.src.lua:9`, our own `SERV_IRUHADEV_EXTRA_NPC_STAT` code) writes a bare
  `slot8 = 0` mid-function. Read the file: **`slot8` is never read anywhere in that function or
  the file.** It's a dead write, not a live hazard — checked, not just asserted.

**Two informational notes, out of Phase 6's scope:**
- **`<main>` `port-divergence` on data-only files** (`Enum.src.lua`: 135 values differ from
  June, 139 June-only, 3 port-only) is expected, pre-existing content drift already tracked by
  `JUNE2017_EFFECT_REBUILD_PLAN.md` Phase 11 ("add every June enum table/member... supersedes
  the 'no reader, not added' rule"), not a decompile defect for this plan to fix.
- **`DamageEffect.src.lua`'s `<main>`** is `other-ljd` (`upvalue`-dominant): its 13 helper
  functions are captured closures in June's original but restructured in the March/Ranox merge,
  so they never match up as callable-by-name entry points on the port side (`not-exercised`,
  13 rows). Expected for a file `JUNE2017_EFFECT_REBUILD_PLAN.md` already calls a "Ranox-merged
  copy," not a literal port.

**Outputs:** `june2017_re/phase5_map.py`, `phase5_worker.py`, `phase5_aggregate.py`,
`phase5_files.tsv`, `phase5_targets.tsv`, `decompile_verdicts.tsv`, `phase5_clobber_census.tsv`.
Scratch (gitignored): `tmp/phase5_out/*.json` (per-file detail), `tmp/phase5_verdicts_{small,
big}.tsv` (the corpus_run.py sweep logs).

**The `local slotN` decision:** asked via `AskUserQuestion` in this same conversation; the
answer and its rationale are recorded immediately below and in `RANOX_LUA_DECOMPILE_BUG.md`.

---

## Phase 6a–6c — Repair the ported files

**Re-split 2026-09-28 from `decompile_verdicts.tsv`'s real `dropped-assignment` row count per
file** (function-level, not lint hits — see "Phase 5 result"), balanced to ~40 writes per
sub-phase rather than by file size or folder. **62 files, 121 writes total.** The decision
above is "keep bare globals" — no `clobber`-class repairs are added to this split; the 3 known
clobber sites (`SECRET_MAGI_BOSS.src.lua` ×2, `SMITH_REGIEON_POINT.src.lua` ×1, already in their
buckets below by their `dropped-assignment` count) are fixed as part of the same case-by-case
pass, not a separate pass.

- **6a (6 files, 42 writes — the heaviest, do these first and alone):**
  `SECRET_MAGI_BOSS.src.lua` (11), `GEYSER_MULSAYRENA.src.lua` (9), `SMITH_SCIENCE.src.lua` (7),
  `SECRET_SCENE_DUMMY.src.lua` (6), `FIRE_DEVIL_IGNIA.src.lua` (5),
  `LOW_SMITH_SCIENCE_FD.src.lua` (4). All in `march2014/update/data037/`.
- **6b (15 files, 36 writes):** `SAMETEAM_IGNIA.src.lua` (4); at 3 each:
  `FIRE_DEVIL_CHAIN_BALL.src.lua`, `FIRE_DEVIL_SKA.src.lua`, `GEYSER_MULLOCK.src.lua`,
  `TRAP_FIRE_BIG.src.lua`; at 2 each: `EVENT_FINALHALL_BOSS2.src.lua`,
  `FIRE_DEVIL_BERSERKER_BOSS.src.lua`, `FIRE_DEVIL_BERSERKER_BOSS_FINALHALL.src.lua`,
  `FIRE_DEVIL_IGNIA_BOSS_FINALHALL.src.lua`, `GEYSER_MULLOCK_CALL.src.lua`,
  `GEYSER_MULSAYRENA_CALL.src.lua`, `LOW_GEYSER_MULLOCK_CALL_FD.src.lua`,
  `LOW_GEYSER_MULLOCK_FD.src.lua`, `LOW_MOUNT_BERSERKER_BOSS.src.lua`,
  `MOUNT_FIREBIRD_FIRETEMPLE.src.lua`. All in `march2014/update/data037/`.
- **6c (41 files, 43 writes — the 1- and 2-write long tail, plus MapData and a Kenta
  recheck):** at 2 each: `SMITH_REGIEON_POINT.src.lua`, `SMITH_RUNNER.src.lua`; at 1 each (all
  `march2014/update/data037/` unless noted): `EVENT_FINALHALL_BOSS.src.lua`,
  `EVENT_GATEWAY_BOSS.src.lua`, `EVENT_GAYSER_BOSS.src.lua`, `FIRETEMPLE_REGIEON_POINT.src.lua`,
  `FIRE_DEVIL_BERSERKER.src.lua`, `FIRE_DEVIL_BERSERKER_FIRETEMPLE.src.lua`,
  `FIRE_DEVIL_BERSERKER_INCHANT.src.lua`, `FIRE_DEVIL_SNIKER.src.lua`,
  `FIRE_DEVIL_SNIKER_INCHANT.src.lua`, `GEYSER_ALUMINUM.src.lua`, `GEYSER_MULMAI.src.lua`,
  `GEYSER_SCIENCE.src.lua`, `LOW_FINALHALL_BERSERKER_FD.src.lua`,
  `LOW_FINALHALL_FIREBIRD_FD.src.lua`, `LOW_FINALHALL_SNIKER_FD.src.lua`,
  `LOW_GEYSER_ALUMINUM_FD_BOSS.src.lua`, `LOW_GEYSER_MULMAI_FD.src.lua`,
  `LOW_GEYSER_SCIENCE_FD.src.lua`, `LOW_MOUNT_BERSERKER_FD.src.lua`,
  `LOW_MOUNT_FIREBIRD_FD.src.lua`, `LOW_SMITH_ALUMINUM_FD_BOSS.src.lua`,
  `MOUNT_FIREBIRD.src.lua`, `MOUNT_POINT_STAGE_BOSS.src.lua`, `SECRET_BERSERKER.src.lua`,
  `SECRET_CRUEL_BERSERKER.src.lua`, `SECRET_CRUEL_SNIKER.src.lua`, `SECRET_FIREBIRD.src.lua`,
  `SECRET_MAGI_SUMMON.src.lua`, `SECRET_SCENE_1_1.src.lua`, `SECRET_SNIKER.src.lua`,
  `SECRET_STAGE1_CHAIN1.src.lua`, `SECRET_STAGE1_CHAIN2.src.lua`,
  `SECRET_SUMMON_BERSERKER.src.lua`, `SECRET_SUMMON_FIREBIRD.src.lua`,
  `SECRET_SUMMON_SNIKER.src.lua`, `SMITH_ALUMINUM.src.lua`, `SMITH_CART.src.lua`,
  `SMITH_STAIN.src.lua`; plus `march2014/data036/MapData.src.lua` (1: `MD-1`, and MD-S — the
  `(A and B or C) and D` structure defect — is unreachable by the harness but still real, see
  the calibration's MD-S correction) and a final re-check of Kenta (`update/data037/
  RAGE_DRYADKENTA.src.lua`) against the fixed decompile (0 `dropped-assignment` expected —
  it's already fixed; this is a confirm-not-regressed pass, not new repair work).

**Paste with:** Starting context + this section + "Phase 5 result" (including the decision)
+ the sub-phase's file list.

**Model:** Sonnet 5, high, then Opus `/code-review` on each sub-phase's diff.

**Goal.** Every `dropped-assignment` (and, per the decision, `clobber`) in a ported file is
fixed, and every intentional port edit is kept.

**Work, per flagged function.**
1. **Compare three ways:** the June bytecode (via harness trace), the fixed decompile, and
   the port file.
   - **Port equals the unfixed decompile at the site, and June agrees with the fixed
     decompile:** that's the bug. Fix it.
   - **Port differs from both:** that's a port edit. Find its reason in `MODS.md` /
     `RANOX_PORT_PLAN.md` / its own comment, and keep it. If you find no reason, ask; don't
     silently revert it.
2. **Make the minimal edit**, as Kenta's fix did: restore the assignment ahead of its reads.
   Follow the recorded `local` decision. Don't splice whole decompiled bodies over port edits.
   - Add a `SERV_IRUHADEV_RANOX_PORT (<date>): ...` comment citing the harness verdict:
     function, seed, first-divergence event.
   - Preserve CRLF. Run `file` and `git diff --stat` after each file.
3. **Verify.**
   - Run the lint on each repaired function: it should show zero hits.
   - Compile with `compile-batch -v mar-2014` on a `tmp/` copy: it should be clean.
   - Run the harness June vs the repaired port. Only the listed port-edit divergences
     should remain.

**Outputs.**
- The repaired `.src.lua` files.
- Rows in `decompile_verdicts.tsv` updated to `repaired`.
- A per-sub-phase list of **files needing recompile + repack** into which `.kom`, handed to
  the user.

**Exit criteria.**
- The sub-phase's files are harness-clean apart from the listed port edits.
- The review findings are resolved.
- The repack list is delivered.
- In-game play-testing waits for the user's repack. **Ask before launching.**

**Kickoff prompt** (substitute 6a/6b/6c and its file list):
```
Continuing the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", "Investigation ledger", "Phase 5 result" incl. the local-slotN decision, "Phase
6a-6c") and MODS.md's Dryad Kenta entry as the worked example. Branch: main.

Your task is Phase 6<x>: repair <file list> using decompile_verdicts.tsv. Three-way compare
per flagged function (June trace / fixed decompile / port file): fix bugs with the minimal
edit, keep port edits (find their reason; ask if none). Follow the recorded local decision.
Cite the harness verdict in a SERV_IRUHADEV_RANOX_PORT (<date>) comment. CRLF preserved; run
`file` + `git diff --stat` per file. Verify with lint, compile-batch -v mar-2014 on a tmp/
copy, and the harness (only listed port edits may remain). Hand me the recompile+repack list.
Then /code-review. Ask before launching the game.
```

---

## Phase 7 — Sweep stage 2: `data037_kom` (the AI scripts)

**Paste with:** Starting context + this section + "Phase 4 result" + "Phase 0 result".

**Model:** Sonnet 5, high.

**Goal.** A trustworthy fixed decompile, with a verdict, for every file in the folder future
monster/boss ports draw from.

**Work.**
1. **Run the sweep.** Run `corpus_run.py` over all `data037_kom` originals (3,105 on
   2026-09-27), comparing June vs the fixed round trip, in the background with `--resume`.
2. **Write the decompiles** to `<Trunk>/june_src_fixed/data037_kom/<name>.src.lua`.
   - That location is persistent and gitignored by the `/*` allowlist.
   - It is not `tmp/`, because later ports read from it.
   - It is not `14JUN17\`, which stays pristine.
3. **Record verdicts** in `decompile_verdicts.tsv`, using a `corpus` scope column so they are
   kept separate from ported files.
4. **Catalogue `other-ljd` divergences** by shape into a "Residual ljd defects" section of
   `RANOX_LUA_DECOMPILE_BUG.md`. Examples: the `deepcopy` class, and anything new. Don't fix
   them here. A new class that hits many files becomes a follow-up plan item for the user
   to schedule.

**Exit criteria.**
- Every file has a verdict.
- Zero `dropped-assignment` with the fix. Any exception is a Phase 4 regression: stop and
  report it.
- The `unreached` and `unverifiable-*` counts are reported, not hidden.

**Kickoff prompt**:
```
Continuing the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", "Phase 0 result", "Phase 4 result", "Phase 7"). Branch: main.

Your task is Phase 7: sweep every June data037_kom .lua with corpus_run.py + ljdiff (June vs
FIXED round trip), in the background with --resume, writing fixed decompiles to
<Trunk>/june_src_fixed/data037_kom/ and verdict rows (scope=corpus) to
decompile_verdicts.tsv. Stop and report on any dropped-assignment (Phase 4 regression).
Catalogue other-ljd divergences by shape into a "Residual ljd defects" section of
RANOX_LUA_DECOMPILE_BUG.md without fixing them. Report unreached and unverifiable counts.
Never kill python by image name.
```

---

## Phase 7 result

Ran 2026-09-29/30. All exit criteria met: every one of the 3,105 `data037_kom` files has a
verdict (3,098 `ok`, 7 tooling-limitation failures — see below), zero unexplained
`dropped-assignment` (Phase 4 fix holds; `phase7_aggregate.py` exits 0), and the `unreached`/
`unverifiable` counts are reported as zero rather than hidden.

**Tooling.** `june2017_re/phase7_worker.py` (a `corpus_run.py` WORKER, modeled on
`phase5_worker.py` but with no `port` side — `LD.build(path, table, [], twin=True)` — since most
of `data037_kom` has no `march2014/` port yet), `phase7_aggregate.py` (writes `scope=corpus` rows
into the shared `decompile_verdicts.tsv`, preserving Phase 5's `scope=ported` rows), and
`phase7_cluster_residuals.py` (groups the non-`clean` rows by first-divergence shape for the bug
doc, fixes nothing). Fixed decompiles persist at `june_src_fixed/data037_kom/<name>.src.lua`
(gitignored, persistent — read by later ports, unlike `tmp/`). Ran at `--workers 1` per Phase 5's
own concurrency-race finding (a shared `types.tsv` under concurrent `learn()`/`save_types()`).

**Two real false positives in the regression check itself, found and fixed, not dismissed.**
The known deepcopy-hoist exemption (`phase5_aggregate.py`'s `deepcopy_exempt_at_line()`, shared
by both aggregators) only recognized `for … in pairs(...)` loop headers and single-register
hoisted reads. The much larger `data037_kom` corpus surfaced two shapes it missed:
`ipairs(...)`/numeric `for` loop headers (`Treasure_Summon{1,2,3}.lua`), and a hoisted
constructor entry reading more than one register on its line where only some are loop variables
(`SECRET_MAD_BEHEMOTHSOUL.lua`, confirmed against the orig trace: the non-loop-var register's
real write executes early in true bytecode order). Both fixed in `deepcopy_exempt_at_line()`;
re-verified against the full sweep afterward (exit 0, nothing left unexplained). Full detail in
`RANOX_LUA_DECOMPILE_BUG.md`'s "Residual ljd defects" section.

**One incident during the sweep, not a tooling defect:** a diagnostic aside (testing whether
raising Python's recursion limit would rescue a `RecursionError` file) was run manually against
the SAME shared `june2017_re/ljdiff/types.tsv` the background sweep was using — exactly the
concurrency hazard Phase 5's own finding warns about. It also ran away to 15+ GB and had to be
killed by PID (never by image name, per the standing rule), which had briefly starved the
sweep's own `luajit.exe` calls into two spurious "not enough memory" `STEP_ERROR`s. Confirmed:
raising the recursion limit does not help — it only trades a clean `RecursionError` for an
unbounded `MemoryError`. Lesson for future phases: never run a second `ljdiff`/`ljd_fixed`
invocation by hand while a sweep using the same `types.tsv` is running in the background.

**Residual defects, none fixed here per the plan.** 31,347 `scope=corpus` rows: 28,374 `clean`,
2,140 `reorder`, 706 `other-ljd`, 61 `clobber`, 55 `other-ljd (deepcopy, exempt)`, 4 `upvalue`.
Clustering the non-clean rows by first-divergence shape (293 distinct textual shapes) collapses
into: pure call-argument-evaluation-order reordering (the great majority — same `order` class
Phase 4's Kenta calibration already named, BR-O/FI-O, and deliberately left unfixed), a reorder
shifting where a genuine Lua error or the harness's own runaway-call budget lands, and 61
`clobber` rows across 21 files (2 already known from Phase 5's ported-file census; 10 more
flagged as an unconfirmed lead for whoever next ports one of those files).

**7 files got no verdict — a real, reproducible limitation of the fixed hook, not a correctness
regression.** 2 more (`Low_Rash_Bugi_Trock_FD.lua`, `Defence_Soldier_Benders.lua`) initially
failed the same way but were the orphaned-diagnostic-process artifact above; retried clean once
that process was killed, and folded into the verdict counts above. The remaining 7: 3
`RecursionError` (the fixed hook's `copy.deepcopy`-based flow rebuild hits Python's recursion
limit on some deeply-nested ASTs — confirmed not fixable by raising the limit) and 4 `TIMEOUT`
(the fixed hook's decompile step exceeds `ljdiff.py`'s own hard-coded 600s internal subprocess
timeout, independent of `corpus_run.py`'s `--budget` — confirmed by retrying at `--budget 900`
and still hitting the same 600s internal cap). None of the 7 has a `march2014/` port yet, so
nothing currently shipped depends on them. Full per-file detail in
`RANOX_LUA_DECOMPILE_BUG.md`'s "Residual ljd defects" section.

**Outputs:** `june2017_re/phase7_worker.py`, `phase7_aggregate.py`, `phase7_cluster_residuals.py`,
`phase7_targets.tsv` (3,105 rows), `june_src_fixed/data037_kom/` (3,098 fixed decompiles),
`decompile_verdicts.tsv` (32,013 rows total: 666 `scope=ported` + 31,347 `scope=corpus`), and the
`deepcopy_exempt_at_line()` fix in `phase5_aggregate.py` (shared by both aggregators).

---

## Phase 8 — Sweep stage 3: the remaining folders

**Paste with:** Starting context + this section + "Phase 7 result".

**Model:** Sonnet 5, high.

**Goal.** The same as Phase 7, for the other roughly 4,200 files. Most of them are data
tables (`data040_kom` line maps, `data034_kom`, `datai*`).

**Work.**
1. **Run the same pipeline**, writing to `june_src_fixed/<dir>/`.
2. **Take the fast path for files that define no functions.** The main chunk's recorded
   calls and final globals are the whole comparison, so skip the per-function driver.
   Confirm the fast path's verdicts agree with the full path on 20 sampled files first.
3. **Handle the 14 files over 1 MB** on their own list, with a larger budget. A `timeout`
   verdict is acceptable if it is reported.

**Exit criteria.**
- Every one of the corpus files has a verdict.
- The corpus-wide totals are written into `RANOX_LUA_DECOMPILE_BUG.md`.

**Kickoff prompt**:
```
Continuing the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", "Phase 7 result", "Phase 8"). Branch: main.

Your task is Phase 8: run the Phase 7 pipeline over every remaining June folder, writing to
<Trunk>/june_src_fixed/<dir>/. Use the main-chunk-only fast path for files that define no
functions, after checking on 20 sampled files that it agrees with the full path. Put the 14
files >1 MB on a separate larger-budget list. Stop on any dropped-assignment. Write
corpus-wide totals into RANOX_LUA_DECOMPILE_BUG.md.
```

---

## Phase 8 result

Ran 2026-09-30. All exit criteria met: every one of the 4,223 non-`data037_kom` files has a
verdict (4,215 `ok`, 8 tooling-limitation failures — see below), and the corpus-wide totals
(Phase 7 + Phase 8, all 7,328 files) are written into `RANOX_LUA_DECOMPILE_BUG.md`'s new
"Residual ljd defects (Phase 8...)" and "Corpus-wide totals" sections.

**Tooling.** `june2017_re/phase8_build_targets.py` split the 4,223 files (reading
`decompile_corpus.tsv`, not re-deriving anything) into three target lists: 494 files that define
at least one nested function (`protos_complete > 1`, run full — `learn()` populates
`types.tsv`), 3,715 that define none (`protos_complete == 1`, run `--fast`), and the 14 files
over 1 MB on their own list (further split 12 fast / 2 full by the same `protos_complete` test).
`june2017_re/phase8_worker.py` is `phase7_worker.py` generalized to: (a) a `--fast` flag that
skips `LD.learn()` entirely, (b) `--decompile-timeout`, threading a previously-hard-coded 600s
`ljd -f` subprocess timeout in `ljdiff.py`'s `decompile()`/`build()` into a real parameter
(default unchanged at 600, only the big-file batches pass more), and (c) writing the fixed
decompile to `<src-outdir>/<kom_dir>/<name>.src.lua` instead of Phase 7's flat
`<src-outdir>/<name>.src.lua`, since Phase 8's targets span 20 folders. `june2017_re/phase8_aggregate.py`
is `phase7_aggregate.py` generalized the same way, with one correctness-critical
difference: it preserves existing `scope=corpus` rows by **exact `(kom_dir, kom_name)` pair**,
not by `kom_dir` alone — see "One real bug, caught before it mattered" below.

**The fast path: proven safe before the sweep ran, not assumed.** See
`RANOX_LUA_DECOMPILE_BUG.md`'s Phase 8 section for the full account — the short version is that
`entry_points()` is structurally empty for every `protos_complete == 1` file (confirmed by
reading `ljdiff.lua` directly: `run_main()` executes once regardless of `MODE`/seed count, and
the per-entry-point seed loop below it never iterates over an empty list), so `--fast` only
removes `learn()`'s extra work, and a 20-file fast-vs-full check (byte-identical JSON on all 19
that completed) plus a direct grep confirming **zero `==`/`~=` anywhere** in any of the 19
decompiled samples (the only thing `learn()`'s `static_learn()` could have taught from a
main-chunk-only file) closed the loop. `--fast` never calls `save_types()`, confirmed both by
code reading (`save_types()` has exactly one call site, inside `learn()`) and empirically (all
20 calibration runs left `types.tsv` untouched) — which is what let the 3,715-file fast batch run
8-way parallel (614s total, 0.17s/file) instead of serial.

**Batch order, and why:** functions batch first (serial, `--workers 1`, populates `types.tsv`),
then the fast batch (parallel, reads but never writes `types.tsv`), then the two big-file
sub-batches (serial, larger budget). Never overlapped — Phase 7's own concurrency-race finding
(concurrent `learn()`/`save_types()` on the shared `types.tsv`) still applies to any batch that
isn't `--fast`.

**One real bug, caught before it mattered.** A first version of `phase8_aggregate.py` preserved
existing `scope=corpus` rows by `kom_dir` alone (mirroring `phase7_aggregate.py`, which only ever
had one folder to worry about). Phase 8 splits files *within* a folder across its three batches —
8 folders appear in more than one target list, confirmed programmatically (0 shared `(dir, name)`
pairs, but 8 shared `dir` values) — so aggregating the fast batch second silently dropped the
functions batch's own rows for every one of those 8 folders (a row-count drop against the
committed baseline caught this immediately after the second aggregation run, before any further
phase work built on it). Fixed to scope by exact `(dir, name)` pair; re-verified by restoring
`june2017_re/decompile_verdicts.tsv` to its committed baseline and re-running all four
aggregations in order, confirming Phase 5's 666 `scope=ported` rows and Phase 7's 31,347
`data037_kom` rows both survive byte-for-byte.

**A second, unrelated bug from an early misconfigured run, also caught and repaired.** The very
first `corpus_run.py` invocation passed `--worker june2017_re/phase8_worker.py`; `corpus_run.py`
runs its worker subprocess with `cwd` set to its own directory (`june2017_re/`), so the relative
path doubled (`june2017_re/june2017_re/phase8_worker.py`) and all 494 targets came back `CRASH`.
The immediate retry fixed the worker path but still passed `--outdir`/`--hang-dir` as relative
paths, which resolve against the *worker's* cwd (`june2017_re/`), not the caller's — so a few
files' real output landed in a stray, untracked `june2017_re/tmp/` before this was caught and all
three path arguments (`--outdir`, `--hang-dir`, `--src-outdir`) were made absolute for every later
invocation. One second-order effect of chaining `--resume` through this: 65 files that had already
run (with `status=ok`) against the wrong, now-deleted `june2017_re/tmp/` output got silently
skipped by `--resume` on the corrected re-run, since `_read_done()` only checks that a `(dir,
name)` row exists, not that its `out_path` is reachable — `phase8_aggregate.py` reported these as
`not-run` (json missing) and named all 65, which is what surfaced the problem; they were
identified from the aggregator's own `not-run` rows, stripped out of the run TSV, and re-run clean
(110s for 65 files) before final aggregation.

**Two genuinely new tooling-limitation shapes surfaced, beyond Phase 7's 3 already-known
classes** (`RecursionError`, `TIMEOUT`, and the deepcopy-hoist value-level defect): ljd omitting
the parentheses Lua's grammar requires around an immediately-invoked function literal
(`compile-batch` then can't even parse the output), and ljd choosing a `[[...]]` long-bracket
string delimiter without checking whether the string's own content contains `]]`, closing the
literal early. Both are decompile-stage failures (no output at all, not a wrong one), documented
with the exact `compile-batch` errors and affected files in `RANOX_LUA_DECOMPILE_BUG.md`, not
fixed here per the plan.

**Corpus-wide, all 7,328 files:** 6,001 fully clean, 1,312 more with only harmless
reorder/other-ljd/clobber/upvalue residuals, 15 with no verdict (a tooling limitation, not a
correctness question), and **zero** files with an unexplained `dropped-assignment` — the Phase 4
fix holds across the entire June corpus, not just the files that happened to get ported or the
`data037_kom` folder.

**Outputs:** `june2017_re/phase8_worker.py`, `phase8_aggregate.py`, `phase8_build_targets.py`,
`phase8_targets_{functions,fast,big,big_fast,big_full}.tsv`, `ljdiff.py`'s
`decompile_timeout`/`build(..., decompile_timeout=...)` parameter (default unchanged),
`june_src_fixed/<dir>/` for 20 more folders (4,215 fixed decompiles), and `decompile_verdicts.tsv`
grown to 41,809 `scope=corpus` rows (covering the full 7,328-file corpus) plus Phase 5's untouched
666 `scope=ported` rows.

---

## Phase 9 — Wire it in, document, close out

**Paste with:** Starting context + this section + every "Phase N result".

**Model:** Sonnet 5, high.

**Work.**
1. **Tooling.**
   - Flip `ljd_jun2017.py` to use the fixed hook by default, and keep `--unfixed` for
     comparison.
   - Update `regen.sh` to match.
   - Add a `--check` that runs `ljdiff` on the output and exits non-zero on any
     `dropped-assignment` or `clobber`. Keep the lint as the fast pre-check.
2. **`RANOX_LUA_DECOMPILE_BUG.md`.**
   - Replace the heuristic triage table with the verdict summary.
   - Keep the lint's calibrated precision and recall.
   - Link this plan.
3. **`june2017_re/README.md`.** Add a section for `corpus_run.py`, `ljdiff/`, `ljd_oracle/`,
   `ljd_fixed.py`, `ljconv_table.tsv` and `june_src_fixed/`.
4. **`MODS.md`.** Add a tooling entry. There is no client flag, since no client code
   changed. Cross-reference the Phase 6 repairs.
5. **Memory.**
   - Update `elstools-decompile-drops-assignments-becomes-globals.md`: the root cause, the
     fix hook, and "use `--fixed`/the default".
   - Retire the "never trust a read-through" advice only for output of the fixed decompiler
     with a clean `--check`.
6. **Ask the user** whether to apply `ljd_slotworks.patch` to the two vendored copies, which
   would also give `elstools.exe decompile-batch` the fix.

**Exit criteria.**
- A fresh `regen.sh` run uses the fix, and `--check` passes on the `june_lua/` set.
- Docs and memory are consistent with each other and with the verdict TSV.

**Kickoff prompt**:
```
Closing out the Ranox Lua decompile fix. Read RANOX_LUA_DECOMPILE_FIX_PLAN.md ("Starting
context", every "Phase N result", "Phase 9"). Branch: main.

Your task is Phase 9: make ljd_jun2017.py/regen.sh use the fixed hook by default (keep
--unfixed), add a --check that runs ljdiff and fails on dropped-assignment/clobber; rewrite
RANOX_LUA_DECOMPILE_BUG.md's triage table as the verdict summary; document the new tools in
june2017_re/README.md; add a MODS.md tooling entry; update the
elstools-decompile-drops-assignments-becomes-globals memory. Then ask me whether to apply
ljd_slotworks.patch to the two vendored ljd copies.
```

---

## Phase 9 result

Ran 2026-10-01. Both exit criteria met, with one caveat surfaced and handled rather than
routed around (see "What the 14-file check actually found").

**Tooling.**
- `june2017_re/ljd_jun2017.py` installs the Phase 4 fix **by default**; `--unfixed` opts out
  (was `--fixed` opt-in before this phase). `decompile(lua_path, fixed=True)`'s own default
  changed to match, for any future library caller.
- **New `--check FILE.lua`**: runs `june2017_re/ljdiff` (`LD.build`/`learn`/`trace`/`diff_files`,
  32 seeds) against `FILE`'s own June bytecode and fails if the `rt` side shows any `clobber`,
  or any `dropped-assignment` read site not covered by `phase5_aggregate.py`'s
  `deepcopy_exempt_at_line()` — the same exemption the Phase 5/7/8 aggregators use for the
  separate, still-open deepcopy-hoist defect. Without this exemption, `EffectSet.lua` produces a
  false failure (confirmed empirically before the exemption was wired in: its main chunk's
  hoisted `deepcopy` prints as `dropped-assignment,upvalue,other-ljd`, structurally identical to
  the already-catalogued "ljd caveat," not a Phase 4 regression).
- **Exit codes, not just pass/fail**: `0` clean, `1` a real clobber or unexplained
  dropped-assignment, `2` the harness itself couldn't run (`ljdiff.StepError`, e.g. a
  `compile-batch` syntax error from ljd's own printer). Per the plan's own process rule 3
  ("unreached"/unverifiable is its own verdict, never "clean"), `2` is reported, never silently
  treated as either a pass or a correctness finding. The multi-file CLI dispatch combines child
  exit codes by priority (`1` wins over `2` wins over `0`), not `|=` or `max()` — an early
  version used `|=`, which would have silently turned `1` and `2` together into `3`, a code
  neither one means.
- **A real bug found and fixed during this phase's own verification, not shipped**: the first
  `check()` only inspected `LD.summary_row(res)` (which sums `res['fns']` only) for
  `dropped-assignment`, and only fell through to the per-site exemption walk when that sum was
  nonzero. A main-chunk-**only** `dropped-assignment` (no function-level one at all — exactly
  `EffectSet.lua`'s shape) was therefore never walked, never exempted, and never flagged: it
  would have silently reported "clean" for the wrong reason. Fixed by always walking `rt`'s
  sites (function-level and main-chunk) regardless of the summary total. Caught by actually
  running `--check` against `EffectSet.lua` twice — once before the fix (correctly failed, given
  a design that didn't yet exempt the known deepcopy defect) and once after adding the exemption
  but before this second fix (incorrectly passed, for the wrong reason: the gate skipped the
  check entirely rather than the check correctly exempting it) — and only trusted after a third,
  clean run with both fixes in place.
- `regen.sh`: unchanged decompile line now runs fixed by default (no `--fixed` needed);
  `REGEN_UNFIXED=1` opts out. Added a `--check` pass over all 14 `june_lua/` files after
  decompiling, which reports (not fails the build on) any per-file exit code `2` and only fails
  the whole run on a real exit code `1`.

**What the 14-file check actually found — run individually, one file at a time, and confirmed
with `types.tsv` untouched afterward (`git diff --stat` clean)**: 13 of 14 clean, 1
(`DamageEffect_Dungeon.lua`) exit code `2` — **this is not a Phase 4 regression**, it's the same
already-documented Phase 8 finding (`RANOX_LUA_DECOMPILE_BUG.md`'s "Residual ljd defects (Phase
8...)"): ljd prints the deepcopy-hoist IIFE without the parentheses Lua's grammar requires around
it, so `compile-batch` rejects the `rt` side before the harness can even trace it. This file was
already known-unverifiable before this phase (it's in Phase 8's 8-file no-verdict table); Phase
9 didn't introduce the gap, it made `--check`/`regen.sh` report it honestly instead of either
silently passing or hard-failing the whole regen. **The literal exit criterion ("`--check` passes
on the `june_lua/` set") holds for 13/14 file-level passes and for the whole-run exit code**
(`regen.sh` exits 0, since `DamageEffect_Dungeon.lua`'s `2` is reported, not fatal) — it does not
mean all 14 individually returned exit `0`, and this document says so rather than rounding up.

**`RANOX_LUA_DECOMPILE_BUG.md`**: triage table replaced with the real, bytecode-verified verdict
summary (62 files / 123 functions, from `decompile_verdicts.tsv`'s `scope=ported`/`repaired`
rows, superseding the old 61-file/774-hit lint guess); "Detecting more instances" now leads with
`--check` and keeps the lint's calibrated precision/recall (1.00 / 0.73-1.00); "Not done in this
pass" rewritten as "Status" now that Phases 5-9 are all done; the stale "6 instances" Kenta count
corrected to the real 12 (Phase 2b calibration) in both this doc and `MODS.md`'s Kenta entry,
matching what the `dryad-kenta-jump-softlock` memory already claimed had been corrected — it
hadn't been, until this phase actually did it. Fixed two spots that invited pointing `--check`
at the pristine `14JUN17\` tree directly (the exact mistake this phase's own early testing made
and had to clean up, see below) to say "a copy with its `.std.luac`" instead.

**`june2017_re/README.md`**: new "Ranox Lua decompile fix: corpus tooling" section documenting
`corpus_run.py`, `decompile_corpus.tsv`/`ljconv_table.tsv`, `ljdiff/`, `ljd_oracle/`,
`decompile_verdicts.tsv`, the `phase5_*`/`phase7_*`/`phase8_*` sweep drivers, and
`../june_src_fixed/` — caught mid-write that the last of these lives at the Trunk root, not
under `june2017_re/`, before it shipped wrong. "ljd dropped-assignment fix" section updated for
the new default and `--check`.

**`MODS.md`**: new tooling entry right after the Dryad Kenta entry (no client flag — content
and tooling only), cross-referencing the Phase 6a-6c repair commits (`8a141a5`/`6809a33`/
`2bbd358`) and listing the still-outstanding recompile/repack/play-test step.

**Memory**: `elstools-decompile-drops-assignments-becomes-globals.md` rewritten for the new
default, `--check`, and the corpus-wide sweep result; the refuted "safe within one function's
own sequential statements" claim it used to carry replaced with the real, cited evidence (3
clobber instances, Phase 5) and the user's actual decision (keep bare globals, fix clobbers case
by case). `dryad-kenta-jump-softlock.md` updated to point at the finished plan instead of
"phases 0-2b done."

**One incident during this phase's own work, caught and cleaned up, not just noted**: an early
manual test ran `elstools decompile-batch` and `ljd_jun2017.py --check` directly against
`RAGE_DRYADKENTA.lua`'s path under the pristine `14JUN17\` tree (decompile() writes its output
next to its input), producing exactly the write-into-`14JUN17\` mistake the project's own
standing rule warns about. Caught immediately, the two stray files
(`RAGE_DRYADKENTA.src.lua`/`.std.luac`) deleted, confirmed `14JUN17\data\data037_kom\` back to
only its two original `.lua` files. All later testing used scratch copies in `<Trunk>/tmp/`.

**Post-close update, same day: the user was asked, and said to apply `ljd_slotworks.patch`.**
Applied to both `elstools/vendor/ljd` and the second copy `elstools.exe` runs
(`F:\elsword stuff\elstools\elstools\vendor\ljd`), verified `diff -rq` clean against each other
afterward and matching the patch header's own recorded post-patch SHA-256
(`2235d0a1...`) on both. This immediately surfaced a real bug the phase's own testing hadn't
hit: `ljd_fixed.py`'s `install_fix()` only recognized the *pre*-patch SHA and refused outright
once the vendored file no longer matched it — breaking `ljd_jun2017.py`'s own new default path.
Fixed by making `install_fix()` also recognize the file `ljd_slotworks.patch` produces (reading
its recorded SHA from the patch header) and treat that as a no-op instead of an error, re-tested
against a small file (clean) and against `DamageEffect.lua`/`EffectSet.lua` again (both still
clean). **Consequence, stated plainly, not left implicit: `--unfixed` no longer produces
genuinely unfixed output anywhere** (`ljd_jun2017.py --unfixed`, `ljd_fixed.py` run directly, and
`elstools.exe` all now hit the statically-patched vendored file regardless of the hook) — true
pre-fix behaviour would need a fresh, unpatched checkout of both vendored `ljd` trees. Every doc
that said "not applied" (`RANOX_LUA_DECOMPILE_BUG.md`'s Status, `MODS.md`'s tooling entry,
`june2017_re/README.md`'s "ljd dropped-assignment fix") was updated to match.
