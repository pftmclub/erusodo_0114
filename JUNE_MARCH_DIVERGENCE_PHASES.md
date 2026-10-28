# One function per name: June's code merged into March's, no `June` names, no March behaviour left inside June code

Backlog Stage 35 (user, 2026-10-28). Plan key `june-march-divergence`, branch `main`, no log tag (a phase may add one), flags
`SERV_IRUHADEV_JUNE_DIVERGE_<NAME>` for D1's replaced functions (the collapse keeps each body's existing flag), kickoffs
`kickoffs/june-march-divergence/`, evidence `june2017_re/diverge/` (D0a writes it), tables `tools/data/june_march_divergent.tsv` and
`june2017_re/diverge/collapse_map.tsv`. **Runs after the Eve port is complete (user, 2026-10-28)**: D0a waits for char-eve Z.

## Context

The user (2026-10-28): "i just dont want the march related issue keep happening on june's logic or work", then "i wanna go for our entire
changes so far so there is no June prefixed function and the future function naming are without prefix -- so the codebase look really
updated and not a duplicated code mess", and "if march doesnt have the function that june needs, dont worry to create a new function but
always see how the project pattern for naming (without june prefix of course)", and names must be readable in context, not
`June<address>`.

**Two problems, one cause.**
1. **March behaviour inside June code.** A June body calls March functions by name; where June's namesake is a stub or differs, the port
   keeps March's behaviour and nothing fails. char-eve CH4a review (`b86fc6d6`): `GetEqippedSkillMemo` vs June's always-false `31fca9e0`.
   The same kind: `f4f26c25` (`322cc1a0`), `012ca1f0` (`322ece70`), `04448382`, `3785fd55`, `01e83fcf`, `214dec90`, `51837124`, `c3010acc`, `9c4e7307`.
2. **Duplicated code.** The ports wrote June's functions as `June_X` / `JuneX` copies beside March's `X`, so the tree carries two versions
   of hundreds of functions, and a copy that calls the old namesake is exactly problem 1.

**The studio's own pattern is the fix.** KOG changed a function in place under a per-update flag and callers kept the name (March's
`GetEqippedSkillMemo`, `X2GUUser.cpp:6765-6781`, sits whole inside `#ifdef SERV_SKILL_NOTE`; June's `31fca9e0` is that function after a
later update). The rule is in `CLAUDE.md` from 2026-10-28 ("no `June` prefix; June's body goes into March's own function") for all new
code; this plan brings the code written before then into line, and audits every March function June code calls.

**Planning census (2026-10-28, rough scripts; D0a's tool replaces them):**

| What | Count |
|---|---|
| June-prefixed member function definitions | ~1,505 (Eve 568, Aisha 453, `X2GUUser` 108, result adapter 37, `X2GameUnit` 36, quest 35, skill-tree UI 35, `X2DamageEffect` 27, `X2Game` 23, emblem 20, `X2Main` 20, `X2SkillTree` 14, `KTDXApp` 14, ...) |
| ... with a March twin in the same class (merge) | ~604 (Eve 373, Aisha 200, quest 17, `X2GUUser` 9, ...) |
| ... without one (rename to the class's pattern) | ~900 |
| `m_*June*` data-member mentions in headers | ~700 (`X2GUUser.h` 87, `X2GameUnit.h` 74, `X2GUNPC.h` 48, `X2GuEve.h` 47, ...) |
| locals with `June` after a type prefix (`bJuneMemo`) | ~400 distinct, ~2,150 uses |
| `JUNE_*` constants (phase-tag aliases such as `JUNE_CH1I_SA_AHM_METEOR_SHOWER`: 195 distinct, 545 uses) | ~364 |
| address-named identifiers | 1 (`June32481fa0`, VioletMage) |
| `_CONST_*_JUNE_*_` namespaces / Lua-bound `June*` names / June-named classes | 29 / 10 / `CX2UIQuestNewJune` (+ file) |
| June bodies (approx.) and the distinct March functions they call | ~1,500 bodies, ~1,300 callees, ~15,000 call sites |
| ... callees with a funcmap row / of those hinting a difference / never compared | 453 / 93 / 847 |

**Known slips in today's tree:** `X2GUArme_VioletMage.cpp:27702` (`June_AVSI_ABM_DASH_COMBO_Z2a_FrameMove`, June `32cdc0f0`) calls
`GetEqippedSkillMemo( SMI_AISHA_MEMO5 )` and grants super armor June never gives; GT_PVP-only tests where June calls `322cc1a0`
(VioletMage 34837 / 35032 / 35720, `X2GUUser.cpp:29462`), whose "March has no GT_PVE" comments are stale (`X2Game.h:72` `GT_PVE = 4`,
`X2Game.h:884` `GetIsNowPVPCorrectionGame()`).

**Seed divergent candidates** (`june2017_re/funcmap.tsv` line: June address, March name, what June does). D0b rules each; the full 93 come from D0a.
1182 `31fca9e0` GetEqippedSkillMemo, returns 0 · 3545 `31fd4450` MemoCondition, always false · 4491 `328df840` memorize-sign test, always
false · 985 `327089e0` GetAttribEnchantRequireED, always true · 946 `32001570` CanApplySocketDataByGameType, GT_PVE rejects all · 1129
`32630d20` ParsingBuffFactorID · 1166 `323644a0` GetIncreaseSkillLevelByBuff · 945 `3204f190` SumSocketOption · 2281 `322d0350` (+3)
StopOther / StopAll / Stop2* · 2434 `327ef160` ShowBossNameAtStart · 1161 `326c9030` UPDATE_BUFF_INFO_IN_ROOM_NOT · 504 `32d4ce60` BBT
80-84 · 2489 `320462a0` DamageReact reflect · 1495 `3204d440` EraseDebuffAndAddMP · 1699 `3208ddc0` CheckAndUpdateAdditionalStat (no-op) ·
2221 `31d411c0` Update3DPosition · 3006 `31d43ef0` GetStrByLineBreakColor · 2386 / 2393 / 3030 NPC CommonStateStart / UpdateUnitPointer /
OnFrameRender_Prepare · 328 / 1473 / 1474 buff-factor / reduced-damage helpers (Stage 33) · 2069 / 2923 / 160 base-class stubs ·
idioms `322cc1a0` (PvP or PvE) and `322ece70` (stat / power split flags).

## Rules

- Branch `main`; commit with explicit pathspecs; other sessions share the tree. X2Lib / KTDXLIB sources are CP949: `bytepatch.py` or the C0
  tool (byte-level, ASCII identifiers only), never the Edit tool.
- **Merge = the studio pattern.** March's `X` becomes `{ #ifdef <the June body's flag> June's body #else March's body #endif <flag> }`;
  the `June_X` definition, its declaration and its `#ifdef F June_X() #else X() #endif` call-site wrappers go away; callers call `X`.
  With every June flag off the tree is byte-for-byte March's again (flagcheck `--base`).
- **Merge only true twins.** `June_X` merges into `X` only if its June address is the June counterpart of March's `X` (funcmap). Otherwise
  it is a different June function and is renamed.
- **Names follow the class's own pattern** (CLAUDE.md, user 2026-10-28): state functions as the class names them, `Get` / `Set` / `Is` /
  `Can` accessors, Hungarian members and locals, `CX2*` classes, `_CONST_<CLASS>_<phase>_` namespaces. A name says what the thing is in
  context; no `June`, no Ghidra address, no phase tag in any identifier. Flags keep `SERV_IRUHADEV_JUNE_*`. A rename must not collide
  with, hide or silently override a base-class member (`CX2GUUser`, `CX2GameUnit`, ...): the C0 tool checks the hierarchy.
- **June removed it, so it is removed here (user, 2026-10-28).** A merge or replacement changes unported March callers too (about 350
  memo calls alone). That is the point, not a question.
- **Merge anyway, guard it (user, 2026-10-28).** A merge that would crash a class that is not ported yet (June state that class never sets
  up) is merged, with the missing-state check inside the June body and a one-line `stand-in: <class> not ported` comment. Each guard is a
  row in `june2017_re/diverge/guards.tsv`; that character's port removes it.
- **Replace = June's whole body** (no partial June updates); June quirks verbatim with a `JUNE_BUGS.md` row.
- **Stage 33 owns stat / damage formulas.** A divergent candidate inside its map gets verdict `stage33` and stays there. A June-prefixed
  function inside its map is still renamed or merged here (naming only; Stage 33 judges the formula).
- No hardcoded enum literals (`enumscan.py --check`). A `JUNE_CH*` alias of an enum member is replaced by the member, never renamed.
- **Behaviour-neutral by default.** C phases change names and move bodies; a rename alone changes nothing, and a merge changes behaviour
  only where March's `X` was still reached in the June build. Each C phase lists those reach points in its Done note.
- Drift goes by the backlog scope rule, never into this plan.

## Starting context

Tools: `python tools/build.py --deploy` (background), `flagcheck.py` (`--worktree --files`, `--range`, `--base`), `bytepatch.py`,
`docsection.py`, `enumscan.py`, `luabind.py`, `luasrc.py` (core-list Lua that calls a renamed Lua-bound name), `logs.py`, `elsdb.py`.
New here: `tools/junecensus.py` (D0a), `tools/junecollapse.py` (C0), `tools/junediverge.py` (D2).
June: Ghidra (`.claude/skills/ghidra`), `june2017_re/funcmap.tsv`, `june2017_re/funcdis.py`. March: `cl /P` for the live branch, never
`#ifdef` reading. Region walker to reuse: `enumscan.scan_file` (`tools/enumscan.py:437-666`: `#ifdef` / `#ifndef` / `#if defined` frames,
`DEFINED_JUNE` from `Always.h`, function-name tracking :558-574); allow-list reader: `luabind.known()` (`tools/luabind.py:41-48`).
Projects: `X2Lib/X2Lib_2010.vcxproj` (+ `.filters`) for the class / file rename.

## Phase D0a - Census tool (Sonnet 5.5, high)

**Run with:** `/x2-phase june-march-divergence D0a`. **Depends on:** char-eve Z. No game code, no build.
`tools/junecensus.py` writes `june2017_re/diverge/`:
- `names.tsv`: every identifier with `June` / `JUNE_` (not `SERV_IRUHADEV_JUNE_*`), a June address or a phase tag, in X2Lib / KTDXLIB /
  X2Lib/Offline `.cpp` / `.h` and their Lua binders: kind (member function / free function / data member / local / constant / namespace /
  class / file / Lua name), file, class, enclosing function, flag at the definition, June address from the comment or funcmap, the March
  twin if one exists (same class, prefix stripped), use count.
- `callees.tsv`: every distinct March function called from June code (June-named bodies and the ON branch of a defined
  `SERV_IRUHADEV_JUNE_*`, never `#else`), with call sites, owner class, and its funcmap rows (`same` / `hint: <word>` / `none`).
- `--summary` prints the census table of the Context. `--check-twins` lists the twin pairs.
**Gate:** the counts land within a few percent of the Context's (explain any gap); spot-check 10 rows by hand.

## Phase D0b - Rulings: twins, funcmap hints, names (Opus 5.5, high)

**Depends on:** D0a. Read-only plus tsvs. Delegate pulls; keep every June reading.
1. Every twin pair: is `June_X`'s address March `X`'s June counterpart? `merge` or `rename`.
2. Every non-twin name: its new name in the class's pattern (a subagent proposes, Opus approves; ambiguous ones listed in the Done note).
3. The 93 hinted callees plus the Context seeds: `tools/data/june_march_divergent.tsv` (`# name	kind	june_va	verdict	june_does	march_does
   write_instead	march_callers	guard	source`; verdict `replace` / `idiom` / `out` / `stage33`).
4. Write `june2017_re/diverge/collapse_map.tsv` (`# old	new	action	file	class	flag	june_va	guard	note`; action `merge` / `rename` / `enum`
   (alias -> enum member) / `class` / `file` / `namespace` / `lua`), split into the C1-C5 batches, and the D0c split (by callee owner).
**Gate:** every name and every hinted callee has a verdict; the C1-C5 kickoffs list their batches; the D0c kickoffs exist.

## Phase D0c - Never-compared callees (Opus 5.5, high; one session per area D0b sets, default three)

**Depends on:** D0b. Read-only; runs beside the C phases. For each callee in its area with no funcmap row: clear by rule (plain getter,
STL, engine accessor June shares, checked once per kind) or find June's version through an already-ported June caller (its address is in
the funcmap) and compare. Each difference becomes a `june_march_divergent.tsv` row; each June function found gets a funcmap row.
**Gate:** every callee in the area is `same (how checked)` or a tsv row.

## Phase C0 - Collapse tool (Sonnet 5.5, high)

**Depends on:** D0a (format), D0b (map) for the selftest data.
`tools/junecollapse.py apply <batch> [--dry-run]` applies `collapse_map.tsv` rows of one batch:
- **rename:** byte-level, whole-identifier, ASCII-only, across X2Lib / KTDXLIB `.cpp` / `.h` and binders; refuses a name that already exists
  in the class or any base class (reads the class hierarchy from the headers).
- **merge:** moves June's body into March's definition under the June body's flag (the Rules shape), deletes the `June_X` definition and
  declaration, collapses the plain `#ifdef F June_X(...) #else X(...) #endif` call-site wrappers; lists every other shape (mixed flags, a
  wrapper with more code, a virtual) for the phase to do by hand.
- **enum / namespace / lua / class / file:** the matching rewrite (class/file: `git mv` + `X2Lib_2010.vcxproj` and `.filters`).
- After each file: `flagcheck` flag-off equivalence against the batch's base revision, and an encoding check.
- `--selftest` on fixtures: a twin merge, a rename blocked by a base-class name, a mixed-flag call site reported, a CP949 byte kept.
**Gate:** selftest PASS; a dry run of C1's batch on `X2GuEve.cpp` reports counts that match the map.

## Phase C1 - Collapse Eve (Sonnet 5.5, high)

**Depends on:** C0. `X2GuEve.{h,cpp}` (+ Eve call sites elsewhere). Apply the Eve batch; hand-do the reported shapes; one commit per
step (renames, merges, hand shapes). Build + deploy. Smoke play-test: Eve, every job, a dungeon with each skill slot and the Nasod Core.
**Gate:** no June-named identifier left in Eve's files (`junecensus.py --file`); flag-off equivalence PASS; DEPLOY OK; smoke lines clean.

## Phase C2 - Collapse Aisha (Sonnet 5.5, high)

**Depends on:** C0; may run beside C1 (different files; the build lock queues them). `X2GUArme_VioletMage.{h,cpp}`. As C1, for Aisha
(transcendence included). The `27702` memo call becomes a call to the replaced function in D1; here it is only renamed with its function.

## Phase C2 - Review (covers C1 and C2) (Opus 5.5, high)

Every merge against its June address (spot-check 10 per class in Ghidra), the hand shapes, the reach list (where March's body was still
reached and now runs June's), flag-off equivalence per flag, the smoke logs.

## Phase C3 - Collapse the shared unit code (Opus 5.5, high)

**Depends on:** C2 review. `X2GUUser`, `X2GameUnit`, `X2GUNPC`, `X2Unit`, `X2RidingPet`, `X2DamageEffect`, `X2SkillTree`, `X2Game` (D0b's
list). Opus because merges here reach unported classes: write each `stand-in: <class> not ported` guard (`guards.tsv`). Smoke: one ported
and one unported class (Elsword) through a dungeon. **Review:** `C3 review` (Opus): the guards, every reach point, the smoke logs.

## Phase C4 - Collapse UI, data, engine, offline; class, file and Lua names (Sonnet 5.5, high)

**Depends on:** C3 review. Quest (`CX2UIQuestNewJune` -> its March twin or a pattern name, file + vcxproj), skill-tree UI, result adapter,
emblem, `X2Main`, `KTDXApp` and the rest of KTDXLIB, X2Lib/Offline; the 10 Lua-bound names and every core-list Lua that calls them
(`luasrc.py refresh`, `luabind.py check`; pack, ask before copy). Smoke: quest window, skill tree, dungeon result, character info.
**Review:** `C4 review` (Opus).

## Phase C5 - Members, locals, constants, namespaces (Sonnet 5.5, high)

**Depends on:** C4 review. `m_*June*` members (merge into a March twin of the same type and meaning, else rename), `bJuneMemo`-style
locals, `JUNE_*` constants (an enum alias -> the member), `June32481fa0`, the 29 `_CONST_*_JUNE_*_` namespaces -> `_CONST_<CLASS>_<phase>_`.
Build + deploy; smoke = C1-C4's lines. **Gate:** `junecensus.py --summary` shows 0 June-named identifiers outside flags.
**Review:** `C5 review` (Opus).

## Phase D1 - Replace divergent March functions at the definition (Opus 5.5, high)

**Depends on:** C5 review and every D0c. Per `replace` row of `june_march_divergent.tsv`: March's definition becomes June's whole body
under `SERV_IRUHADEV_JUNE_DIVERGE_<NAME>`, March's in `#else`, `MODS.md` flag + revert line, guard + `guards.tsv` row if an unported class
would crash, `JUNE_BUGS.md` row if June's answer is a bug. First: `GetEqippedSkillMemo` -> `return false;` (June `31fca9e0`; fixes VioletMage
27702 without touching it). Hand-off: per visible change, one ported and one unported class.
**Review:** `D1 review` (Opus): each body against its listing, `#else` intact, flagcheck per flag, one unported call site per row.

## Phase D2 - The gate and the wiring (Sonnet 5.5, high)

**Depends on:** D1 review. `tools/junediverge.py`:
- `--check <files>`: (a) any new June-named / address-named / phase-tag identifier outside a flag name = FAIL; (b) the `idiom` rows inside
  June code (June bodies are now March's functions: the region is the ON branch of a defined `SERV_IRUHADEV_JUNE_*`, never `#else`);
  (c) a call to a `replace`-row function that is not yet replaced. Allow-list `tools/data/june_diverge_allow.tsv` (`# file	function	name
  reason`). `RESULT: PASS` / `RESULT: FAIL, N hit(s)`, exit 1.
- `--list` (names to avoid and what to write instead), `--selftest` (pinned revisions: `d4e8edc6:X2Lib/X2GUArme_VioletMage.cpp` hits the
  GT_PVP idiom and the June-named rule; a March `#else` does not hit; an `#ifndef` fixture hits in its `#else`).
- Sweep the idiom hits (GT_PVP-only -> `GetIsNowPVPCorrectionGame()` where June is `322cc1a0`) under the owning flag; build + deploy.
- Wiring: x2-phase Start step 5 (`--list` before transcribing) and Finish step 1 (`--check <files>` must PASS, beside luabind / skillicons),
  the Stage 7 template, `tools/README.md`.

## Phase D2 - Review and close (Opus 5.5, high)

`/x2-phase june-march-divergence D2 review`: selftest, `--check` over X2Lib / KTDXLIB yourself, every sweep fix and allow-list reason,
the wiring. Close: `MODS.md` section (flags, guards, renames by batch, revert), backlog Stage 35 closing note, STATUS. **The plan closes.**

**Standing rule (user, 2026-10-28: "after this plan complete, make it as a rule so this won't happen again").** The naming half is in
`CLAUDE.md` already ("no `June` prefix", 2026-10-28). The close adds the enforcement: `CLAUDE.md` gets the gate (`junediverge.py --check`
must PASS) and the table rule (a review that finds a new divergent March function adds the `june_march_divergent.tsv` row and the
replacement in the same commit); `JUNE_PORT_BACKLOG_ORDER.md` "Standing rules" and `kickoffs/TEMPLATE.md` get a line each; memory is updated.
