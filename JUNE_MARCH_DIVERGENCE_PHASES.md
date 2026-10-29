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

### Done (2026-10-28, no flag, no game code, no build)

`tools/junecensus.py` (new, README row added). `python tools/junecensus.py` scans 1,016 files (X2Lib incl. Offline, KTDXLIB, X2, plus the
`KncWX2Server/Common` headers that name June) in ~15 s cold, ~3 s from the `.build/junecensus` cache, and writes `june2017_re/diverge/`:
`names.tsv` (4,176 rows, one per definition), `callees.tsv` (2,731 rows), `census_meta.json`. Flags: `--summary` (default; it rescans, so a
gate is live), `--check-twins`, `--file PATH`, `--gate` (exit 1 while a June-named identifier remains: C5 / D2), `--no-scan`, `--rescan`. The
`#ifdef` handling keeps the studio frames, evaluates `SERV_IRUHADEV_*` against `Always.h`, and restores the brace stack at `#else`.

**Gate (counts vs the Context; this tool / Context):**
| Row | Result |
|---|---|
| prefixed member functions in `.cpp` | 1,510 / ~1,505 (+122 inline in headers). Eve 560 / 568, Aisha 451 / 453, X2GUUser 110 / 108, X2GameUnit, quest, DamageEffect, X2Game, emblem, KTDXApp equal |
| ... with a March twin (`.cpp`) | **740 / ~604 (+22%)**: Eve 401 / 373, Aisha 307 / 200, quest 17, X2GUUser 8 / 9. Cause: the Context's rough script. Hand-checked: `DashJumpAttackX2FrameMove`, `AVSI_SI_SA_ABM_MAGICAL_MAKEUP_FrameMove`, `RidingHyperModeFrameMove`, `JumpUpEventProcess` all exist as March Aisha functions |
| `m_*June*` occurrences in headers | 724 / ~700; X2GUUser.h 87, X2GameUnit.h 74, X2GUNPC.h 48, X2GuEve.h 47: exact |
| locals with a type prefix | **447 distinct, 2,465 uses / ~400, ~2,150**: a raw text scan finds 454 distinct, so the Context under-counted (it missed bodies whose `{` follows an `#endif`) |
| `JUNE_*` constants / phase-tag aliases | 366 / 193 vs ~364 / 195; alias def + use 540 vs 545 (raw text count 540) |
| address-named / Lua-bound / namespaces | 1 = 1; 11 vs 10 (the extra is the registered class `CX2JuneSkillTypeColorTable`); 30 vs 29 `_CONST_*_JUNE_*_` (one more than the Context, cause not found) |
| distinct callees from June-named bodies | 1,272 (1,121 march + 151 ours) / ~1,300; 15,511 sites / ~15,000. Not counted: 145 names with no project definition, 21 constructor names |
| funcmap: with a row / hint / never compared | 422 / 100 / 850 vs 453 / 93 / 847. Row match is by function name and a class related to the owner; hint words: stub, always true/false, returns 0/false/true, no-op, differs, none in March, June form/overload/only, dead, not ported |

**Spot check (hand, against the source):** 13 name rows (member function x3, free function, data member x2, local x2, constant x2, namespace,
Lua name, class) and 8 callee rows (owner, origin, first site) all match; the known slip `X2GUArme_VioletMage.cpp:27702`
(`GetEqippedSkillMemo`, `hint: stub`, funcmap `31fca9e0`) is in `callees.tsv`. Two bugs found while checking, fixed: class rows pointed at the `{`
line; an `#endif` between `)` and `{` hid function bodies (unresolved names fell from 119 to 1).

**Found beyond the Context (all plan subject; D0b rules names, C4 / C5 carry them):**
1. **295 June-named free functions** (54 in `_CONST_*_JUNE_*` namespaces, 241 at file scope, e.g. `JuneEveIsSkillKeyUp`, X2GuEve 29, X2DamageManager 24,
   X2UISkillTreeNew2015 22, X2GUUser 20): not in the ~1,505 and not in any C kickoff yet.
2. **103 June-named types**: 72 structs / classes (17 of them `JuneGfx*`, all in `KTDGGFXManager`; the rest `JuneDamageDataRecord`, `JuneSlotState`,
   `CX2JuneBuffRuntimeScope`, ...), 10 enums (`JUNE_MEMO_ID`, `JUNE_MENU_SLOT`), 5 typedefs, 12 packet structs, 4 data structs. The Context knew `CX2UIQuestNewJune` only.
3. **26 June-named event ids and packet structs** in the shared headers (`EGS_JUNE_*`, `KEGS_JUNE_*`, `KJuneIntPair`, `KJuneSkillExtraData`): same
   rule as C++ names, but they sit in `KncWX2Server/Common`; D0b decides whether the collapse touches them.
4. Two Offline files and namespaces named June (`X2OfflineJuneQuestMig`, `X2OfflineJuneSkillAutoLearn`); three June-named source files in all.
5. `twin_check` is mostly **unverified** (623 of 749): funcmap names no March function for those June addresses, so D0b proves each twin through Ghidra.
   `yes` 95, `conflict` 3 (funcmap maps the address to another function, e.g. `June_CopyModelFrameStructure`), `unmapped` 28.
6. 151 callees are `origin ours` (declared only behind our flags or in `Offline/`); June bodies call them and they are not March.
7. Plus the live branches of defined June flags: 1,961 march-origin callees, 24,728 sites (June code that is not June-named).

**Known limits:** `ptr->F()` has no type information, so `owner` is the root declaring class(es) (`owner_n` > 1 = ambiguous); `uses` of a
class member is by name for the whole tree (two rows of one name share the number); one name stays `unresolved` (`pJuneHealer`,
`X2GUNPC.cpp:11463`: `DamageReact`'s `#ifdef` branches split braces); functions made by macros are not seen.

**Open for the user:** none. **Next:** `/x2-phase june-march-divergence D0b` on Opus 5.5 high; C0 (collapse tool, Sonnet 5.5 high) may start beside it.

## Phase D0b - Rulings: twins, funcmap hints, names (Opus 5.5, high)

**Depends on:** D0a. Read-only plus tsvs. Delegate pulls; keep every June reading.
1. Every twin pair: is `June_X`'s address March `X`'s June counterpart? `merge` or `rename`.
2. Every non-twin name: its new name in the class's pattern (a subagent proposes, Opus approves; ambiguous ones listed in the Done note).
3. The 93 hinted callees plus the Context seeds: `tools/data/june_march_divergent.tsv` (`# name	kind	june_va	verdict	june_does	march_does
   write_instead	march_callers	guard	source`; verdict `replace` / `idiom` / `out` / `stage33`).
4. Write `june2017_re/diverge/collapse_map.tsv` (`# old	new	action	file	class	flag	june_va	guard	note`; action `merge` / `rename` / `enum`
   (alias -> enum member) / `class` / `file` / `namespace` / `lua`), split into the C1-C5 batches, and the D0c split (by callee owner).
**Gate:** every name and every hinted callee has a verdict; the C1-C5 kickoffs list their batches; the D0c kickoffs exist.

### Done (2026-10-28, no flag, no game code, no build)

Wrote `june2017_re/diverge/collapse_map.tsv` (3,934 rows, one per definition of the 4,176 census rows; columns `old new action file
class flag june_va guard note batch kind`), `tools/data/june_march_divergent.tsv` (232 callee rows), `june2017_re/diverge/d0c_areas.tsv`
(1,325 never-compared March callees by area) and the evidence `d0b_naming.tsv` / `d0b_callee_digest.tsv` (subagent drafts, each row
approved or overruled here). Kickoffs C0-C5 carry their batch, D0c-1..3 their area.

**1. Twins (754 pairs).** Proven from our own state tables: June_X registered at the same Lua state and slot as March's X (557); the
Aisha evasion states (7: March registers them at `AVSI_TELEPORT_*` under `UPGRADE_SKILL_SYSTEM_2013`); macro-registered Hell Drop
control states (11); March's X dispatching to June_X under the flag (86); the port's Ghidra comment ("June 329b2f60: March's
DoFrameMoveThousandStar ...", 89). Ghidra spot checks: `31fca9e0` = `return 0`; the census picked a second address from the comment
for `EveElectraEffectData::OnFrameMove` (329c12a0), `DoFrameMoveThousandStar` (329b2f60), `GetActualMPConsume` (3203cf60). All merge
except `June_ENSI_DASH_COMBO_ZX_*` (rename to March's name: March's copy is only under the undefined `EVE_COMBO_TEST_TEST`) and
`JuneParseCommonRandomState` (rename `ParseCommonRandomStateByUnitClass`: March's is a pure virtual per class). Twins the census
missed: Aisha's four vslot overrides, Eve/Aisha copies of the base's `SetSkillBuffFactors` / `HaveSkillBuff` / `GetSkillBuffFactorPtr`,
the `June_X` bodies behind `JuneX` virtual forwarders (4), three copies of June 32007c40, the June/March state tables
`s_akStatesPostEffect` / `akRSICKTDGUIDialog`, `LEVEL_UP_EVENT_ID`, and `m_bUseRenderParam` (+0xa2c is what June's equip glow sets;
March's glow sets March's: same field, a reach point for C4). Signature merges (hand shapes): `SetShow`, `GetActualMPConsume`,
`EnchantAccumulationRateModifier`, `ModifyFinalDamageByGameUnitType`. Aisha's vslots: 0x604 / 0x608 = `HyperModeEnd` /
`RidingHyperModeEnd` overrides (base made virtual in C3), 0x60c = `RidingOffStart`.

**2. Names.** Default = `June` stripped, then a collision check against the class, its bases, the function and (free / global names)
the tree. 197 vague, colliding, offset- or vslot-named rows got context names (subagent, 102 high / 82 med / 13 low; all approved,
low ones marked in the note). 30 debug log helpers / throttles named after effect-phase tags were named from their bodies. `drop`:
five Aisha members only set in the constructor. 211 `enum` rows: each `JUNE_*` alias resolved by value through `cl /P` to one member.
One phase-tag constant is not an alias: `JUNE_CH1L_ENSI_SA_ECE_REASON_PRIDE_ACTION` is 242, the C++ state 196 (C5 note). Base / override
groups go to the base's batch (38 rows moved to C3 / C4), so no override is ever split between two batches. Shared-header event ids and
packets are renamed client-side (offline only). Lua names: dev-console bindings, no `.lua` calls them.

**3. Callees (`june_march_divergent.tsv`).** 208 hinted rows + 24 Context seeds: out 209 (already June's at the definition, a funcmap
row about another class, or owned elsewhere: render R9, transcendence T5, Stage 2, Stage 8 Lu/Ciel, checklist 7j), **replace 10**
(`GetEqippedSkillMemo`, `MemoCondition`, `FlushMp`, `SetNowMp` x2, `SetNowHp`, `GetLockOnNearstTargetInRange`, `UnrefDevice`,
`StartDisplayer`, `CX2GUUser::UpdateEquipCollisionData`; D1 flags `SERV_IRUHADEV_JUNE_DIVERGE_<NAME>`), **idiom 5** (`322cc1a0` PvP-or-PvE,
`322ece70` stat / power split, the hit-stop lists, BBT 80-84, `ParsingBuffFactorID`: D2's idiom list), **stage33 8** (handed to the
Stage 33 brief).

**4. Batches.** C1 592 (Eve), C2 460 (Aisha), C3 577 (16 shared unit files plus the Eve / Aisha overrides of June-named bases), C4 726
(95 UI / data / engine / offline files, classes, files, Lua), C5 1,579 (members, locals, constants, namespaces). D0c areas (default
three kept): D0c-1 unit classes 409, D0c-2 engine / effect / damage / buff / world 513, D0c-3 data / UI / managers 403.

**Hygiene:** no code, no flag, no build. **Open for the user:** none (no name was left unsettled).
**Next:** C0 (Sonnet 5.5 high) and D0c-1..3 (Opus 5.5 high, read-only) can start; they do not overlap.

## Phase D0c - Never-compared callees (Opus 5.5, high; one session per area D0b sets, default three)

**Depends on:** D0b. Read-only; runs beside the C phases. For each callee in its area with no funcmap row: clear by rule (plain getter,
STL, engine accessor June shares, checked once per kind) or find June's version through an already-ported June caller (its address is in
the funcmap) and compare. Each difference becomes a `june_march_divergent.tsv` row; each June function found gets a funcmap row.
**Gate:** every callee in the area is `same (how checked)` or a tsv row.

### D0c-1 Done (2026-10-29, unit classes; no flag, no game code, no build)

All 409 callees of area D0c-1 have a verdict in `june2017_re/diverge/d0c1_callees.tsv` (callee, owner, sites, verdict, June va, how
checked, batch). The gate holds: **42 new `june_march_divergent.tsv` rows** (replace 29, stage33 8, out 5) and **127 funcmap rows**
`(Diverge D0c-1)`.

**Method.** March's live bodies came from `cl /P` of the 27 TUs that define them, so no `#ifdef` was read by eye. June's versions were
found three ways: the June caller's call list (offline capstone, voting over every ported caller), funcmap, and the June vtables
(`334781f4` CX2GUUser, `3351d0b4` Aisha, `334ba584` CX2GUNPC). Nine Sonnet subagents compared 187 callees; Opus re-read the decisive
ones in Ghidra (below). Every other `differs` row is a subagent reading with June addresses, which D1 reads again before it ports.

| Verdict | Count | How |
|---|---|---|
| same by rule | 221 | plain get / ref (June inlines: `32cc5ae0` GetUnit = `[ecx+0x867c]`), set (`329b4b40` @329b4b70), protected value (`322788c0` reads +0x44d8), STL op (`3293b5e0` @3293c177 vector clear), one-line forward (`3208e0d0` = GetRandomFloat, vslot 0x1cc), constant state id (`3208df10` = `USI_WAIT` 0xd; Aisha has no override in June either) |
| same / inlined-same | 131 | decompiled or disassembled against the live body |
| virtual-stub / already-june / dead / no June call | 6 / 5 / 2 / 2 | `EventCheck` is dead (`X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE`); `OnCameraMove` is a state-table name; `SendPacket` / `StateEnd` have no June call |
| tsv row | 42 | below |

**Re-read by Opus in Ghidra (all agree with the subagents):** NPC `StateChange` refuses while `m_vecFreezePtr` is non-empty (`32758fe0`
@32759074, the vector `EraseFreezeByBuff` walks); NPC `GetStateID` returns the first state whose name **contains** the argument
(`3275aaf0`, `StrStrA` @3275ab27; D1 adds a `JUNE_BUGS` row); `LevelCorrectionExtraDamage` adds 0xa140 (@3276044e); absorb MP
only when |sum| > epsilon (@322746df); `MAJOR_` bone prefix (@31fd925e); `GetSkillLevelValue` 0 for a hyper skill before transcendence
(`31fceab0`); `CureExtraDamage`'s March sort stops one short (`X2GameUnit.cpp:9722`); and one **port slip**: `X2BuffBehaviorTemplet.cpp:2012`
/ `:2078` call `IsFullHyperMode` (vslot 0x9c) where June calls vslot 0xa4 (`32e7de0a`) = `JuneIsBerserkByChance` (the comment above it
already says slot 41). It differs only for Chung (not ported).

**Replace (29, D1):** NPC `StateChange` / `StateChange_Reaction_FIRST` / `GetStateID`; user `GetCombineMatrixFromBoneName`,
`CheckSkillUsePVPOfficial`, `CheckSummonSpecialAttack`, `CommonCameraMove`, `DieFrameMove`, `DyingStart` (June's die packet: the
emulator's handler follows), `ForceChangeHyperModeWithoutMotion`, `GetSkillLevelValue`, `PlayAnimationWaitStart`, `SetCustomAnimName` (June's
base is not empty), `SetUnitReForm`, `UpdateNotRenderingOtherEquipList`, `UpdateWeaponEnchantEffectAndExtraDamageType`; `CureExtraDamage`,
`UpNowMp` (with D0b's `SetNowMp` / `FlushMp`); the three weapon enchant-particle functions (66 EDT values, register June's enum names);
`GetNumItemByTID`; riding `GetUserInfo`, `GetRidingOnState`; `ConstructSkillTreeNotMyUnit`, `SetSkillCoolTimeLeft`; and two call-site fixes
(the buff-site vslot 0xa4 above; June's socket sum reads `GetRidingPetTemplet( manager +0x40 )`, the port's `GetMyRidingPet` route is a stand-in).
**Open for D1:** member +0x89e0 (`PlayAnimationWaitStart`), June's riding-hyper state 0x2c vs March's 0x2b, member +0x9200 (`SetCustomAnimName`).

**Stage 33 (8, map rows):** `GetEnchantResist` (June user override `31fd8790`), `LevelCorrectionExtraDamage`, `ResetMPChangeRate`,
`AbsorbEffectAttackProcess`, `CX2Item::GetStat`, `GetEquippingItemByEquipPos` (a `GetFinalDamage` helper), `GetSetIDNPartsNum`,
`ResetIncreaseSkillLevelBySocket`.

**Drift, owned elsewhere (out, 5):** Eve's equipped-item cut-in (Stage 12, as the source already marks it); gage `EraseBuff` / `PushBuff`
(June's third gage set and BuffIcon shape: Stage 9 UI sweep); square-unit `InitWaitMotion` (Stage 10, funcmap 1817); Ara's
`SetFullHyperMode` +0x9700 and `IsFullHyperMode` arm (Ara port, checklist). Each row names its owner; nothing new to plan.

**Batch-data caveats** (for D0c-2 / D0c-3, which can reuse the method): a name-only owner sometimes points at another class's inline
header body, and some callers' addresses are wrong (`33441c60` is a constant). The subagents re-located those from the call site.

**Hygiene:** no code, no flag, no build. **Open for the user:** none.

### D0c-3 Done (2026-10-29, data / UI / managers; no flag, no game code, no build)

All 403 callees of area D0c-3 have a verdict in `june2017_re/diverge/d0c3_callees.tsv` (D0c-1's columns; `batch` = `ruleA` or the comparison
batch). The gate holds: **70 new `june_march_divergent.tsv` rows** (replace 29, out 40, stage33 1; the ledger's 71 count `UpdateQuickQuestDLG` twice)
and **166 funcmap rows** `(Diverge D0c-3)` (41 more June functions found were already mapped).

**Method.** March bodies: a definition index over `X2Lib` / `KTDXLIB`, classified by shape. June: each call site's enclosing function and its June
address (the census cache, `names.tsv`, funcmap), then the call matched in that function's decompile (or `funcdis.py` where Ghidra has no function),
then June's callee decompiled. Six Sonnet subagents compared 252 callees. Opus ruled every `differs` and read the decisive ones in Ghidra.

| Verdict | Count | How |
|---|---|---|
| same by rule | 151 | plain get / set / empty (June inlines: `324cf442` / `324cf452` read and write `CX2Game::m_bCanUseESC` (+0x2ad); `32447c00` reaches the skill tree with no accessor call; `ProcessShowMiniMap`'s empty base = June's no-op `3208ddc0`) |
| same, compared | 166 | decompile or disassembly against the live body |
| same, the tree already runs June's body under a defined flag | 10 | `JUNE_PARTY_UI` (7 party-UI functions), `JUNE_TUTORIAL_MODE`, `JUNE_QUEST_UI` (2) |
| same, ruled | 5 | `GetPartyMemberData` (June returns the leader byte; every caller reads only that), Scaleform SDK calls (`Initialize`, `PushBack`), `SetAlpha` (plain virtual setter), `GetDungeonType` (June DungeonGame vtable `334bb094` slot 0x1c8 = `327c9ef0`) |
| tsv row | 71 | below |

**Re-read by Opus:** the four `CX2InformerManager::Get*Informer` (March's inner `pInformer` shadows the outer one, `X2InformerManager.cpp:37-49`, so the
create branch returns an uninitialised pointer; June `328546a0` returns the new informer); `Handler_EGS_NEW_QUEST_REQ` (`323d2b40`: quests
208020-208060 return without sending while manager `+0x5d` is 0; the tree's `JUNE_QUEST_UI` branch lacks it); `GetDungeonType` (above).

**Replace (29, D1):** `Get{Guild,Skill,Quest,Inven}Informer`, `GetEDString`, `GetKeyString`, `CreateDummyEquip`, `DisplayAddHPInformation` /
`DisplayAddMPInformation`, `SetMusic`, `SetMapDetail`, play guide `SetMouseClickIndex` (June: empty) and `StartGoEffect`; quest `GetQuestColor`,
`GetNeedToClearDungeonList`, `SetHasDungeonQuest`, `Handler_EGS_NEW_QUEST_REQ`, `UpdateRewardItemSlot`, `UpdateNavi`, `UpdateQuickQuestDLG`,
`UdateLocalMapQuestDesc`, `GetHenirDungeonIcnoDesc`, `UIOpened`, `SetNowDragItemReturn`; and five call-site fixes inside June bodies, where June removed
the call: `IsEventValentineDungeon` / `GetEventValentineDungeonIconDesc`, `IsDefenceDungeon`, `GetPartyUI` (`X2LocalMapUI.cpp:283`), and
`GetCloneBuffFactorPtr` (June goes through the unit's getter `3229a3d0` -> `32028af0`, `X2GUArme_VioletMage.cpp:29411`).
**Open for D1:** `SetMapDetail`'s layer offsets are inferred; `UpdateQuickQuestDLG` brings June's three per-state slot setters.

**Stage 33 (1):** `CX2Stat::Stat::SetKStat` (`323571c0`: two 7-value stat sets).

**Drift, owned elsewhere (out 40 = 33 + C4 + 6):** each row names its owner, and each owner has a bullet now: Stage 9 (12: chat box, cursor, `SetShowMenu`,
cash-shop / system-menu toggles, shortcut keys, minimap, event notice, `IsValidPacket`, `GetErrStr`, PvP emblem, advertisement); Stage 28 brief (8:
party dungeon change, game start, the two button setters, boss items, item level, PvP party dlg, hero recruit); Stage 22 (6: Hero / Bestera gates,
names, clear stars, local-map id, boss-raid field, sub-stage packet); Stage 16 (3: item-manager Lua globals, use-item and finality packets); Stage 29
(2: `GetTotalExp`, leave-room packet); Stage 25 brief (2 line breaks); C4 (`SetOffsetPosSkillSLotStatic`: merge `SetOffsetPosJuneTCSlotUI` into it).
Six have no June caller (debug log, our dev gate, a dead `#else`, two emulator functions, `ProcessQuestDesc` dead on US).

**Batch-data caveats** (as D0c-1): eight caller addresses were wrong (a vtable `3348ffe4`, a `std::map` find `326ab090`, comment-picked neighbours).
The subagents re-located each from the call site. Evidence cells cite scratch decompiles (`june/<va>.c`); D1 re-reads June before it ports.

**Hygiene:** no code, no flag, no build. **Open for the user:** none.

### D0c-2 Done (2026-10-29, engine / effect / damage / buff / world; no flag, no game code, no build)

All 513 callees of area D0c-2 have a verdict in `june2017_re/diverge/d0c2_callees.tsv` (callee, owner, sites, verdict, June va, how
checked, batch). The gate holds: **25 new `june_march_divergent.tsv` rows** (replace 19, out 6) and **91 funcmap rows** `(Diverge D0c-2)`.

| Verdict | Count | How |
|---|---|---|
| same by rule | 370 | 238 plain accessors, 26 STL / DXUT / D3D / GFx calls, 106 one-line helpers and forwarders (bodies read). Spot-checked in Ghidra: `31c8f1b0` GetLayer (walks the manager chain = March's recursion), `31d28b10` IsManagedByDGManager, `31cfe4a0` SetShowObject, `31cd2dc0` dialog SetLayer: all = March. `EventTimerOneshot` (281 sites) = `+0x24c < t && 31d28e10` (funcmap 2234, 2437, 3544) |
| same, compared | 98 | June decompile against the live March body (lists A / B / C, three Sonnet subagents) |
| already June / ours / dead | 9 / 9 / 2 | font and edge draws with June's pass argument (`JUNE_RENDER_DRAW_PASS`, `JUNE_UI_CONTROL_PASS`), the R6 skin draw, the RT `OnLostDevice`; 9 names defined only under our flags; `TableBind` compiled out, state-manager `SetRenderTarget` uncalled |
| tsv row | 25 | below |

**Re-read by Opus in Ghidra:** `SetTex` x2 / `SetMouseOverTex` (refill in place, against `SetDownTex` `31cace60`, which still deletes and
recreates like March), `OpenTexture` (FAILED device -> NULL), `GetKeyState` / `GetButtonState`, `EventTimerGlobal` (before-time reset),
`SetManagerObject` (`31d28a00`, pointer only; the subagent had marked it unsure), `SetBuffFactorToDamageDataByBuffFactorID` (= March:
`322b5320( factor, level, false )`), `GetDefenceDungeonIcnoDesc` (`328a0230` = March; the subagent had not found it in `3293d280` case 0x4653).

**Replace (19, D1):** `CPictureData::SetTex` (both), `CKTDGUISlot::SetMouseOverTex`; `OpenTexture` / `OpenXET` / `OpenXSkinMesh` (NULL
for a FAILED device, no resource log: D1 keeps a log line), `OpenSound` (OGG / INT variants); `GetButtonState` (SM_SWAPBUTTON),
`GetKeyState` (allow-list; its setter `31d25490` goes to Stage 9 beside the tutorial guide, so the check is inert until then);
`GetFixedDistancePosition` / `...InAllLine` (a negative distance flips the side), `ApplyFunctionToParticles` (NULL particle system),
`EventTimerGlobal`, `SetManagerObject`; `ApplyExternalFactorToFinalizer` (June's third argument: set or add); the four buff popups June
dropped (`SelectDelegate`, `GetFinalizerFactor`, `ParsingTableForWhile`, `GetBuffFactorPtrFromBuffFactorList`: drop the popup, keep a log);
and one **port slip**, a call-site fix: Aisha's `June_ChangeWeapon` (`32cc8980`) reads the equip's `m_pItemTemplet` (+0x198) where the port
tests and reads `GetItem()` (`X2GUArme_VioletMage.cpp:30955`); C2 fixes it in the merge.

**Out, owned elsewhere (6):** `GetX2UnitClassCompatibility` (the 55 March classes match; 31 cases for classes March lacks: checklist 7ar,
new); `CX2Eqip::SetRenderParam` (`32963c80`, june-engine R6b); `CollisionDataCheck` (June's math is in the hit tests since Effect C8a; the
remaining calls are the JuneFx comparison trace, March on purpose); `LoadDataFile` / `GetMemberFile` (June's kom reader and archive format;
March's loose-first order is the in-house config the dev overlay uses); `CX2AdditionalStatByBuff::Verify` (no June counterpart found).

**Drift:** the `GetKeyState` setter got a sub-bullet under Stage 9's tutorial-guide item (`JUNE_PORT_BACKLOG_ORDER.md`); class
compatibility got checklist 7ar. Nothing else is new.

**Batch-data caveats** (as D0c-1 / D0c-3): harvested addresses for rule rows can belong to a same-named function of another class
(`SetIndex`, `GetRenderParam`, `SetLayer`), so rule rows carry no June va; `def` named the wrong overload for `RenderVertex` /
`CheckControl` and a dead `GetTexUV` (the live one is `CKTDXDeviceTET_Preprocessing::GetTexUV`, `31d15c10`). D1 re-reads June before it ports.

**Hygiene:** no code, no flag, no build. **Open for the user:** none.

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

### Done (2026-10-29, no flag, no game code, no build)

`tools/junecollapse.py` (new, README row added): `apply <batch> [--dry-run] [--files F ...] [--only OLD ...] [--report TSV] [--out-dir DIR] [--base REV]`, `--selftest`.
Byte-level (comments and literals masked, every edit a splice of the original bytes), reuses `flagcheck.strip` / `enc_class` for the per-file checks.
- **merge:** `X(..) { #ifdef F <June body> #else <March body> #endif F }`; June definition (and its attached `//` comment, moved to the top of the June branch)
  and header declaration deleted; emptied `#ifdef <June flag>` frames removed; parameter names read in March's names; plain
  `#ifdef F June_X #else X #endif` wrappers collapsed, other June branches that call a merged name listed as *mixed call site*.
  **The port's own delegating wrapper** (`{ #ifdef LIVE June_X(..); } #else <March body> } #endif`, 37 of Eve's 410) is rewritten in place
  under its LIVE flag, so those merges change no behaviour. Hand shapes (listed, not touched): inline definition in a header, braces split across
  `#if` in any other way, differing parameter types / default arguments / return type, no March twin in the file, a note naming a hand shape.
- **rename:** refuses a name the class, any base class (read from the headers) or the touched files already have, and two rows of a batch that
  rename to the same name. A name carried by rows of several classes is renamed only in the row's own file and header (`name shared` sites reported).
- **enum / namespace / class / lua / file / drop / keep:** matching rewrites; `lua` also rewrites the exact string literal; `file` is `git mv` + project files + `#include`.
- **checks per file:** `flagcheck.strip` with every `SERV_IRUHADEV_JUNE_*` undefined, new vs the base revision with the same renames applied
  (a rename outside every flag is neutral), byte for byte; encoding class unchanged. One failing file writes nothing.

**Gate:** selftest PASS (twin merge, delegating wrapper, rename blocked by `CBase::Taken`, mixed site reported, CP949 bytes kept, header declaration,
member / namespace / lua / enum / drop, a March-side change caught by the flag-off check). Dry run of C1 on `X2GuEve.cpp` + `.h`: merge 410, rename 182 (= map),
checks PASS, ~36 s: merge 390 auto / 20 hand, rename 178 auto / 4 hand.
Hand list (C1): 6 inline definitions in the header, 3 class copies of a base function, 2 namespace copies, 3 default-argument or overload mismatches, 6 "no March
overload with June's parameter types" / missing twin, 1 collision (`ENSI_DASH_COMBO_ZX_EventProcess`), the notes the map already named. `--report` writes one row per map row.

**Other batches, dry run (smoke only):** C2 460 rows (merge 300/314, rename 144/146, checks PASS); C3 577 (rename 548/554, merge 8/23); C4 726 (rename 565/583,
class 70/101, lua 10/11, file 3/3, merge 1/26; 39 "no use found" are the census's header-only or macro rows); C5 1,579 (rename 1,133/1,327, enum 210/210,
namespace 32/32, drop 5/5; 180 files; about 40 rows are locals whose new name exists in the file). No crash in any batch. Check status: C1, C2 PASS; C5 passes in every file except `Always.h` (item 2); the C3 and C4 summary lines were cut off in my smoke run, so their check status is not recorded: C3 and C4 re-run `--dry-run` first.

**For the C phases:**
1. **Reach points.** 354 of C1's 390 automatic merges sit under `_CORE`, not behind a delegating wrapper: March callers of those `X` now run June's body whenever `_CORE` is on
   (the tool prints the count and tags each row `reach point` in `--report`). C1's Done note lists them; Eve is live, so the build already ran June through the `_LIVE` calls.
2. `KTDXLIB/Always.h` is the only file that fails a C5 dry run: the flag-off check cannot evaluate its `#if defined(..) && ..` (line 10007), so C5 either does that file by hand or filters it with `--only`. Cause of the edit not traced.
3. Mixed call sites (C1: ~48, all June branches under `_LIVE` / `_SKILL` / `_CORE` beside a different March branch) are in the report; a June call inside a bigger `#ifdef` alternative is not listed.

## Phase C1 - Collapse Eve (Sonnet 5.5, high)

**Depends on:** C0. `X2GuEve.{h,cpp}` (+ Eve call sites elsewhere). Apply the Eve batch; hand-do the reported shapes; one commit per
step (renames, merges, hand shapes). Build + deploy. Smoke play-test: Eve, every job, a dungeon with each skill slot and the Nasod Core.
**Gate:** no June-named identifier left in Eve's files (`junecensus.py --file`); flag-off equivalence PASS; DEPLOY OK; smoke lines clean.

### Done (2026-10-29, Sonnet 5.5; flags: each body keeps its `SERV_IRUHADEV_JUNE_CHAR_EVE_*`)

Commits: `e79168a8` renames (182 rows), `df2b994d` merges (390 auto), `affc89cd` hand shapes + hoists. `e79168a8` also carries the C2 session's uncommitted
`X2Game.cpp` hunks (Aisha memo renames): `commit -- <pathspec>` takes the whole file; C2 need not re-commit that file.
- **Hand shapes (all 24 map rows done; none left but one):** `June_HaveSkillBuff` / `SetSkillBuffFactors` / `GetSkillBuffFactorPtr` deleted (callers call `CX2GUUser`'s);
  `CallSummonedNPC`, `ApplyAuraAcceleratorBuffToGameUnit`, `GetAdditionalAccelBuffTime`, `SpecialAttackEventProcess` merged under the port's own wrapper flag (March signature kept,
  a one-line prelude makes June's parameter); `SetSelfDestructSummonedNPC` (int signature under CORE, header + definition), `EndElCrystalSystem`, `AddLaserToSystem` (June signature
  beside March's under CORE, header + definition) merged unwrapped; the six header-inline state forwarders became the `...Body( bool )` call inside March's state function
  under SKILL; the three `TryGetAcquiredSkillAbilityValue` copies are one helper (callers pass false / true); `GetComboZStateID` is one line (June's body is March's);
  `ENSI_DASH_COMBO_ZX_*` take March's names (March's are under undefined `EVE_COMBO_TEST_TEST`); `June_ModifyFinalDamageByGameUnitType` is now Eve's override definition
  of the base virtual `JuneModifyFinalDamageByGameUnitType` (the base name goes in C3). Open: `June_Vslot828` (C3, renamed with its base).
- **Tool gap the C2-C5 sessions must know:** the merge moves June bodies up to March's position, above the file-local helpers they call. `junecollapse.py` does not reorder, so the
  file stopped compiling. Fixed by hand: 16 `_CONST_EVE_JUNE_*` namespace blocks and 8 `JUNE_EVE_*` macros now sit above the constructor, each under its own flag
  (`CH1p` stays: it is under `EVE_ELECTRA` and used after its definition). Check `X2Lib` compiles after every apply; a C0 follow-up is to hoist automatically.
- **Reach points** (March's body was still reached and now runs June's): 363 functions, `june2017_re/diverge/c1_reach_points.tsv` (old name, new name, flag). Notable: the six
  state functions above also serve March's registrations (hyper EPR / atomic blaster states now run June's body); `TryGetAcquiredSkillAbilityValue` now reads the real PVP flag
  where the CH1l copy hard-coded false (offline PvE: same value).
- **Hygiene:** every June flag undefined, X2GuEve.cpp / .h against `7a13a1aa` byte-equal (blank lines from the hoist aside), encoding class unchanged; `enumscan.py --check` 0 A/B rows.
  `junecollapse` own checks PASS for each apply. Not run: `flagcheck.py --range` (its per-flag mode cannot judge a file with other June flags).
- **Gate not met yet, by design:** June-named identifiers still in Eve's files are other batches: the 21 `CX2GUUser`-based rows (C3), `JUNE_*` constants and `JUNE_EVE_*` macros
  and `_CONST_EVE_JUNE_*_` namespaces (C4 / C5), `JuneReservedSkillStart` and the other `June*` base overrides (C3).
- Deploy: DEPLOY OK: X2_offline.exe is 25,822,720 B, dated 2026-10-29 07:10:51, md5 0b868d9a80368d71879d54eaf782ef51, equal to X2/US_SERVICE/x2.exe, built at main affc89cd.
  (the first full compile ran inside a peer-shared build; X2Lib compiled with no error in X2GuEve.cpp.)

## Phase C2 - Collapse Aisha (Sonnet 5.5, high)

**Depends on:** C0; may run beside C1 (different files; the build lock queues them). `X2GUArme_VioletMage.{h,cpp}`. As C1, for Aisha
(transcendence included). The `27702` memo call becomes a call to the replaced function in D1; here it is only renamed with its function.

### Done (2026-10-29, no new flag; each body keeps its `SERV_IRUHADEV_JUNE_CHAR_ARME_*` flag)

`python tools/junecollapse.py apply C2` on `X2GUArme_VioletMage.{h,cpp}` (+ callers in `X2Game.cpp`, `X2SkillTree.cpp`, `X2UISkillTreeNew2015.cpp`):
460 map rows, 444 automatic, 16 by hand; flag-off equivalence PASS in all 5 files (every `SERV_IRUHADEV_JUNE_*` off = HEAD with the same renames, byte for byte),
encoding class unchanged (CRLF/LF mix kept; the lines I inserted were set to CRLF). Merged 314 (`X(..) { #ifdef F June body #else March body #endif F }`), renamed 146.
**Hand shapes (16):**
- 7 default-argument mismatches (`ReInit`, `InitEquippedActiveSkillState`, `CommonHyperModeFrameMove`, `ShowActiveSkillCutInAndLight`, `SpecialAttackEventProcess`, `TransformStart`, `TransformEnd`):
  June's declaration got March's defaults, then the tool merged them.
- `June_SetSkillBuffFactors` / `June_HaveSkillBuff`: copies of `CX2GUUser::SetSkillBuffFactors` / `HaveSkillBuff` (same June addresses 31fcac90 / 3229a470); definitions and
  declarations deleted, 8 callers call the base. **Behaviour change:** the base (Eve phase, June-faithful) reads the skill level with `bBasicLevel = true` and tests
  `m_iUnitClassTran`, the copy used `false` and `UnitFlag3c`; the 6 Magical Body / Dark Deal / Time Limit passive calls and the Time Limit tests now follow the base.
- `June_Vslot60c` + `June_RidingOffStart` (a bare jump) -> `RidingOffStart` (new, no March twin; the state registration calls it); `June_RidingOffEnd` -> `RidingOffEnd`
  (hides non-virtual `CX2GUUser::RidingOffEnd`, calls it first); `June_Vslot828` -> `IsNotCommonMoveState` (no callers; base `JuneIsNotCommonMoveState` is renamed in C3).
- The `ARME_LIVE` inline forwarders for `SetShowViewList`, `UpdateEquipViewListInfo`, `UpdateEquipCollisionData` are gone: the three June bodies are the real overrides of the
  `CX2GUUser` virtuals, declared in the LIVE block and defined under `#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE` (CORE alone stays inert).
**Reach points:** 265 of the 300 automatic merges sit under a non-wrapper flag, so March callers of those functions now run June's body whenever the flag is on
(CORE 219, SKILL 45, PASSIVE 1); the per-row list is `--report` output (`apply C2 --dry-run --report`). 24 "mixed call site" notes (a June branch calling the merged name
beside a different March branch under `#else`) are all renames only. No `guards.tsv` row: Aisha is ported. No JUNE_BUGS row (no June quirk touched).
**Order fix the tool does not do (C0 gap, for C1 / C3 / C4):** a merged June body now sits at March's position, earlier in the file than the phase helpers it uses, so the first build failed (101 errors in
`X2GUArme_VioletMage.cpp`). Fixed by hand: the 19 `JUNE_ARME_*` key macros (identical in every redefinition) are defined once at the top and all 53 `#undef`s and repeat `#define`s are gone; the
namespaces `_CONST_ARME_JUNE_CH1a_/CH1b_/CH1h_/CH1i_/CH1k_/CH4a_/CH4c_` moved, with their `using`, into one `#ifdef SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` block after `_CONST_AISHA_`
(CH4a/CH4c also under PASSIVE). C5 renames these anyway. Flag-off equivalence re-run after the move: PASS.
**Build:** `DEPLOY OK: X2_offline.exe is 25,822,720 B, dated 2026-10-29 07:10:51, md5 0b868d9a80368d71879d54eaf782ef51` (built at df2b994d + 6 uncommitted files; it includes the C1 session's uncommitted Eve edits).
**Left for later batches:** 22 Aisha rows whose base is in `CX2GUUser` (`June_Vslot604/608/654/850/860`, `June_RidingHyperModeEnd`, `June_GetSkillLevel`, ...) are C3;
members, locals, `JUNE_CH*` constants and `_CONST_ARME_JUNE_*_` namespaces are C5. `enumscan` not run (code moved, none added).

## Phase C2 - Review (covers C1 and C2) (Opus 5.5, high)

Every merge against its June address (spot-check 10 per class in Ghidra), the hand shapes, the reach list (where March's body was still
reached and now runs June's), flag-off equivalence per flag, the smoke logs.

### Done (2026-10-29, Opus 5.5; reviews `e79168a8`, `df2b994d`, `affc89cd`, `56880121`; fixes keep each body's flag)

**Verdict: C1 and C2 pass.** One naming fix landed here (six offset names, below); nothing else needed a code change.
- **Twins (10 per class, against Ghidra / the raw listing / the funcmap vslot):** Eve: `ENSI_JUMP_LANDING_EventProcess` 32981ba0, `Draw` 32968ee0 (vslot 0x98),
  `ENSI_EBS_DASH_JUMP_COMBO_ZUpX_EndFuture` 329678f0, `ENSI_A_EN_CONVERSION_Start` 329780b0, `CommonEventProcess` 3297c390, `ENSI_JUMP_COMBO_Z_End` 329872a0,
  `AttackResultPlayEffect` 3299cdc0 (vslot 0x4dc), `ENSI_SA_EPR_GIGA_STREAM_FrameMove` 329a7250, `ENSI_A_EBS_TASER_PILUM_Start` 32973f10, `ENSI_SA_EEP_ELECTRONIC_FIELD_FrameMove` 3299ecb0.
  Aisha: `AVSI_A_AVP_Summon_Bat_Heavy_Init` 32cb8450, `ComboZ4upFrameMoveFuture` 32cb7310, `WaitStart` 32cb9a40, `UpdateEquipViewListInfo` 32cc9950 (vslot 0x644),
  `ChargeMPStart` 32cd1540, `DashEventProcess` 32cd0a40, `AVSI_DIMENSION_WITCH_DASH_ATTACK_X2_EventProcess` 32cc16f0, `AVSI_DARK_MAGICIAN_COMBO_X3back_FrameMove` 32cd4650,
  `Lose` 32cc5ca0 (vslot 0x94), `InitState` 32cea0c0 (0x7a4). All 20 are the June counterpart of March's function, and each June branch reads as that address's body (not diffed against the old `June_X` text).
  Six addresses are not carved in Ghidra (no function there); 32cd4650 / 32cc16f0 have no Ghidra xref but a rel32 jump from the state thunks 32cf2670 / 32cf1140.
- **Hand shapes:** C2's `SetSkillBuffFactors` change is a fix: June 31fcac90 @31fcacc4 pushes `GetSkillLevel( id, 1, 0, 1 )` (basic level true), as the base does;
  Aisha's deleted copy passed false. `UnitFlag3c` was the base's `m_iUnitClassTran` test, so that part is unchanged. The 7 default-argument merges change nothing
  (June's declarations had no defaults, so every June caller passes all arguments). `RidingOffEnd` hides the non-virtual base but is reached only through
  Aisha's own `SET_CB_FUNC( CX2GUArme_VioletMage, RidingOffEnd )` (`USI_RIDING_OFF`). Eve's signature merges (`CallSummonedNPC` prelude, `SetSelfDestructSummonedNPC`) read right.
  The four Eve `..._FrameMoveBody` / `_EventProcessBody( bool )` helpers have no March precedent but name what they are; kept.
- **Renames (audit of all 1,045 C1 / C2 rows):** every `old` gone and `new` present except `June_Vslot828` in Eve (left for C3 on purpose). No new name collides
  with or hides a base member. **Fixed here** (six D0b names were object offsets, not meanings; `junecollapse.py apply` with a scratch map, flag-off PASS):
  `EveUnitFlag3c` -> `EveIsTranscendedUnit`, `UnitFlag3c` -> `IsTranscendedUnit`, `IsBlock9c50ValueCPositive` -> `IsPsykerKinesisHeld`, `UpdateBlock9c50Timer` -> `UpdatePsykerKinesisTimer`,
  `SetGameFloatB38` -> `SetGameMemoRiseTimer`, `GameByteC20` -> `IsMPChargeBlockedByGame` (June never writes CX2Game +0xc20; both readers only block or end the MP charge).
  The map now carries them, and its C4 / C5 rows for the same block say what it is: `JuneEveBlock9c50` -> `PsykerKinesisData`, `m_pJuneBlock9c50` -> `m_pPsykerKinesisData`,
  `m_iJuneClock9c44` -> `m_iLastParryClock`, plus five new C5 field rows (`m_bFlag0` -> `m_bReleased`, `m_hEffectSet0` / `1` -> `m_hStartEffectSet` / `m_hAttackBallEffectSet`,
  `m_fValueC` -> `m_fRemainTime`, `m_fValue10` -> `m_fPowerRate`). Offset-style names still in C5 rows are listed in the C5 kickoff.
- **Reach (every SET_CB_FUNC of the 718 merged functions):** 569 have a live registration, all in the state the June slot names. 37 serve several states
  (DIE_FRONT + RIDING_DIE, the dash-attack and teleport pairs, hell-drop directions); each matches June's sharing. `DashComboZ2aStartFuture` also serves
  `AVSI_EM_DASH_COMBO_Z2a`, as `June_InitState` already registered it before C2 (the map note omits it). Eve's `DashStartFuture` is registered only in dead
  `#else` branches and is reached by its virtual. 143 merged functions are not state callbacks; none is unreferenced. Table: scratch only (`reach_registrations.tsv`, not kept).
- **For C3 (in its kickoff):** vslot 0x828 has no caller; both overrides call the base `JuneIsNotCommonMoveState()` unqualified, so the C3 rename must make the
  base `virtual bool IsNotCommonMoveState()` and qualify those two calls `CX2GUUser::` (Aisha `X2GUArme_VioletMage.cpp:34241`, Eve `X2GuEve.cpp:52865`), or Aisha's override recurses.
- **Hygiene** (`flagcheck.py <FLAG> --range e79168a8^..56880121`, all 14 Eve / Aisha flags): RESULT PASS for every flag, 0 FAIL, 0 ERROR. EVE_CORE `files: 1 PASS, 9 OTHER`
  (deleted 1: the X2GUUser.cpp rename line); EVE_PASSIVE `2 PASS, 8 OTHER` (X2GUNPC rename lines); ARME_CORE `1 PASS, 13 OTHER`, 448 deleted lines = the `June_X`
  declarations removed from `X2GUArme_VioletMage.h`; ARME_MEMORIZE `2 PASS, 7 OTHER` (the two caller renames); EVE_TRANSCEND touched no file. Every OTHER row is a
  merge hunk spanning many June flags, which the per-flag mode cannot judge, so the all-flags check carries it: `flagcheck --with` all 374 `SERV_IRUHADEV_JUNE_*`
  flags, `--base e79168a8^ --at 56880121`, PASS in 9 files and PASS~ in `X2GuEve.cpp` (17 blank / marker lines from the hoist). Encoding unchanged everywhere.
  `eol!` 2008 (Eve) / 15 (Aisha) inserted lines take the moved June body's own line ending (the files were already mixed). `junecollapse.py --selftest` PASS.
  This review's renames: EVE_CORE / ARME_CORE `--worktree` RESULT PASS (OTHER = lines under EVE_LIVE / PASSIVE, ARME_LIVE); `enumscan --check` 0 A/B rows.
- **Smoke logs:** none, by the user's 2026-10-29 rule (one smoke at the end of the plan).
- DEPLOY OK: X2_offline.exe is 25,822,720 B, dated 2026-10-29 07:42:56, md5 2a98d4ac1df2d4abc6457f90c660c8d8, equal to X2/US_SERVICE/x2.exe (built at main 6548aebf + this review's 3 code files).

## Phase C3 - Collapse the shared unit code (Opus 5.5, high)

**Depends on:** C2 review. `X2GUUser`, `X2GameUnit`, `X2GUNPC`, `X2Unit`, `X2RidingPet`, `X2DamageEffect`, `X2SkillTree`, `X2Game` (D0b's
list). Opus because merges here reach unported classes: write each `stand-in: <class> not ported` guard (`guards.tsv`). Smoke: one ported
and one unported class (Elsword) through a dungeon. **Review:** `C3 review` (Opus): the guards, every reach point, the smoke logs.

### Done (2026-10-29, Opus 5.5; no new flag, each body keeps its flag)

`python tools/junecollapse.py apply C3`: 577 rows, 555 by the tool in 62 files (merge 8, rename 547), 22 by hand; flag-off equivalence PASS per file.
Tool fix: `verify()` skips the `#if` strip when a file's new bytes equal the base with the same renames (`Always.h` `#error` text was "not evaluable").
**Hand shapes (`bytepatch.py`):**
- Shared-name sites the tool leaves outside each row's own files: the 12 it reports (`GetNearestCollisionFrameName`, `IsPosAlongLastTouchLine` x3,
  `DamageDataTempletScope`, `GetDefenceRateBySkill`, `CopyStateCallbacks` in Eve) and 10 it does not (`pUser->` calls of `CanAllyRallyToMe`, `SummonCriticalBonus`,
  `OverrideEffectAlphaCondition`, `ShownSkillCoolTime` x2, `SummonedNPCInfo` x2, `AfterEquipSlotChange`, `SkillIconOverride`; the two `X2Main.cpp` binding pointers,
  whose Lua strings stay for C4's `lua` rows), found by the first build.
- **Virtuals.** `CX2GUUser::HyperModeEnd` / `RidingHyperModeEnd` become virtual under ARME_CORE (vslots 0x604 / 0x608, March's non-virtual in `#else`); Aisha's
  `June_Vslot604` / `June_Vslot608` are the overrides `HyperModeEnd` / `RidingHyperModeEnd`, `June_RidingHyperModeEnd` (bare jump 32cf0fa0) is deleted and both states
  register `SET_CB_FUNC( CX2GUUser, ... )`. `IsNotCommonMoveState` is virtual (vslot 0x828); Aisha's and Eve's (`June_Vslot828`) overrides call `CX2GUUser::` (Aisha's
  unqualified call would have recursed). Forwarders: Aisha `AfterEquipSlotChange` / `AfterImageFrameMove` / `SummonedNPCInfo`, Eve `SummonedNPCInfo` / `SkillIconOverride`
  / `CanAllyRallyToMe` / `SummonCriticalBonus` / `ShownSkillCoolTime` are the bodies now (the `June_X` copies gone), defined under the forwarder's own flags (LIVE, plus
  the feature flag on Eve's four), so CORE alone stays inert.
- **Signatures.** `GetActualMPConsumeJune` -> a non-virtual 3-arg overload `GetActualMPConsume( id, level, bApplySocketMPCostCut_ )` (the 2-arg virtual and the 10 class
  overrides untouched; Add's one `false` caller is base-qualified). `EnchantAccumulationRateModifier`: one name, `#ifndef ENCHANT_PARITY` March's 3-arg, `#else` June's
  `( DamageData*, ... )`. `ModifyFinalDamageByGameUnitType`: June's `( DamageData*, phys, magic )` is a third overload. `SetSkillAbilityMap` (June's key argument) and
  `ResetLeftSkillCoolTimeAll` (June's mode + list) are overloads of March's name.
- Renames the tool refused (name used elsewhere, different scope): `IsSkillSlotState`, `IsUnitTranscended`, `ParseCommonRandomStateByUnitClass`.
- **Constructor rows rename their struct**, uses in other files did not follow: `ComboRow`, `SkillCommandDesc`, `SocketOptionSource`, `SSkillMemoTemplet` (C4 class row
  run here with `--base $(git stash create)`), and `CX2SkillIDRateByBuff` keeps `typedef ::SkillIDRateBlock SkillIDRateBlock;`.
- **Order (the tool does not move helpers):** `X2GUUser.cpp` CH1k constants hoisted above `StartEventProcess` as `_CONST_GUUSER_CH1k_` (CH1j's anonymous namespace is
  `_CONST_GUUSER_CH1j_`), the cancel-chain macro's `#undef` dropped; `X2SkillTree.cpp` ability-key table hoisted above `AddSkillTemplet_LUA` as `_CONST_SKILLTREE_S2_`.
**Reach points** (March callers now run the June body when the flag is on): `CommonSpecialAttackEventProcess`, `GenericActiveSkillEventProcess`,
`GenericSpecialActiveSkillEventProcess`, `WinStateStart`, `LoseStateStart`, `StartEventProcess` (ARME_CORE), `HyperModeEnd` (EVE_CORE), `AddSkillTemplet_LUA`
(SKILL_TEMPLET; March's already dispatched to it, so unchanged). Win / Lose / Start add June's result invincibility and treasure particles; the Generic* pair adds June's
skill / walk / dash / back-walk cancel chain, which fires only on keys a state's own Lua sets: no class state is touched, no guard.
**Guards** (`june2017_re/diverge/guards.tsv`, 2 rows): `CommonSpecialAttackEventProcess` returns June's slot index (A 0-3, F 4, B 5-8, T 9); every class but Eve and
Aisha reads March's 0-7, so for them F / T give no slot and B reports 4-7. `HyperModeEnd` applies the pending hyper buff; Add's `ApplyHyperModeBuff` skips the base
that clears the flag, so Add is skipped (Ara / Chung / Raven call the base; Elsword / Lire / Elesis use it).
**Found, not ported (D1):** June's riding-hyper end 32006c00 (vslot 0x608) is HyperModeEnd's body; March's `RidingHyperModeEnd` stays March's: row added to
`tools/data/june_march_divergent.tsv` (`replace`, guard: Add as above).
**Hygiene:** all-flags flag-off check (`flagcheck ARME_CORE --base HEAD --with` all 456 `SERV_IRUHADEV_*`, 65 files): RESULT PASS, 0 FAIL, 0 ERROR; `Always.h` changes
are `#error` texts only. `enumscan --check` (5 hand-edited files): 0 A/B rows. Per-flag `--range fb062a6b^..fb062a6b`: ARME_CORE `1 PASS, 65 OTHER, 0 FAIL,
1 ERROR` (36 deleted lines), EVE_CORE `1 PASS, 65 OTHER, 0 FAIL, 1 ERROR` (35 deleted); the ERROR is `Always.h`'s compound `#if` around the renamed `#error` texts;
the deletions are the removed `June_X` declarations / definitions and renamed lines. ARME_LIVE / EVE_LIVE / ENCHANT_PARITY / SOCKET_PARITY / SKILL_TEMPLET left to the review.
DEPLOY OK: X2_offline.exe is 25,816,576 B, dated 2026-10-29 08:27:03, md5 d0fbc7c838bc92c312431ee3c941e377, equal to X2/US_SERVICE/x2.exe (built at main 6c7726f4 + this phase's 67 code files).
**Smoke:** none here (user, 2026-10-29: one smoke at the end of the plan); the end smoke must take Elsword and Eve or Aisha through a dungeon, B-slot skills on both.

## Phase C3 - Review (Opus 5.5, high)

The guards, every merge against its June address, the reach points, flag-off equivalence per flag, the smoke logs.

### Done (2026-10-29, Opus 5.5; reviews `fb062a6b`, `d0b2b8f9`; fixes keep each body's flag)

**Verdict: C3 passes.** Two kinds of fix landed (below); no merge or guard needed a code change.
- **Merges against June (Ghidra / raw listing):** `CommonSpecialAttackEventProcess` 31fcadd0 (key order A S D C Skill5, T on MY unit's tree @g_pData+0x68+0x90,
  the expand-slot test, B 5-8; every exit returns 1 as June does), `HyperModeEnd` 3201b8e0 (= 32006c00 body: vslot 0x7dc end, pending byte, 31ffda00 + vslot 0xc0,
  +0x9544 = 0), `WinStateStart` 320004b0 / `LoseStateStart` 320007e0 (pet, +0x8625, vslot 0x11c with the result-UI time + 5, the 41470 particles), `StartEventProcess`
  3201b390 (code event 0x85f, `StateChange( 13, 0 )`), `GenericActiveSkillEventProcess` 31fcc9d0 and `GenericSpecialActiveSkillEventProcess` 31fcc740 (not carved; raw listing:
  `StateChange( 0x11, 0 )`, then the cancel chain behind 322731a0 / 32070cf0 / 32070d60). All seven are the June counterpart and read as that address's body.
  The hand shapes read right: the 3-arg `GetActualMPConsume` overload (2-arg virtual = June slot 424 with true, Add's one false caller qualified), `EnchantAccumulationRateModifier`
  one name per flag state, the third `ModifyFinalDamageByGameUnitType` virtual (Eve overrides it; Ara's 4-arg override hides it only for calls on an Ara type, none exist),
  Eve's five forwarder bodies declared and defined under the same LIVE + feature flags, `IsNotCommonMoveState` virtual with both overrides calling `CX2GUUser::`,
  Aisha's `HyperModeEnd` / `RidingHyperModeEnd` reached through `SET_CB_FUNC( CX2GUUser, ... )` by the virtual.
- **Guards (`guards.tsv`, both kept as written):** the slot-index guard is needed and minimal: for a March class it returns exactly what March's own branch did (F and T
  give no slot, B reports 4-7, an empty slot sets no index); it drops the March branch's `[JUNE_TRANSCEND] SLOT F / T pressed on a March class` log line. The Add
  guard is needed (`CX2GUAdd::ApplyHyperModeBuff`, `X2GUAdd.cpp:9529`, never reaches `ClearHyperModeBuffPending` in `X2GameUnit.cpp:16774`) and minimal; Ara / Chung / Raven call
  the base first, Elsword / Lire / Elesis use it, Eve and Aisha have no override.
- **Reach points, one correction:** the Generic* pair is registered by unported classes too (`CX2GUUser, Generic*` in Elsword, Lire, Chung, Aisha 65, Eve 12). The cancel
  members default to 9999 (`X2GUUser.cpp:1259-1262`) and are read from each state's Lua (`:17479-17482`), so an Elsword / Lire / Chung skill state whose March Lua sets
  `SKILL_CANCEL_AFTER` / `WALK_` / `DASH_` / `BWALK_CANCEL_AFTER` now cancels as June's base does. C3's "no class state is touched" holds only for states without those keys;
  this is June's base behaviour (rule: June removed it, so it is removed here), for the end smoke. Win / Lose now give every class the result invincibility
  (`GetStateResultUITime()` is the 0 stand-in, so 5 s). `HyperModeEnd` now applies a pending hyper buff when an unported class leaves the hyper state before its own apply.
- **Fixed here: dead dispatches** (`bytepatch.py`). Where March's `X` held `#ifdef F return June_X(); #endif F` (or `#ifdef F June_X(); #else <March body>`), the merge renamed the
  call to `X` inside F's own `#else`: never compiled, but it reads as recursion. Six: `X2SkillTree.cpp` `AddSkillTemplet_LUA` (C3), Aisha `InitPassiveSkillState` (C2), Eve
  `InitPassiveSkillState`, `GetActualMPConsume`, `SetSpecificValueByBuffTempletID`, `UnSetSpecificValueByBuffTempletID` (C1). Each `#ifdef F` / call / `#else F` and its inner
  `#endif F` is gone; preprocessed output is unchanged in every flag state. `tools/junecollapse.py` now reports new ones after an apply (`nested_same_flag`, compared
  with the base so the studio's own nestings are not counted); `--selftest` PASS. Four older nestings outside this plan stay (`Handlers_Unit.cpp:926`, `X2GUUser.cpp:46136`,
  `X2TFieldNpc.cpp:1115` / `1225`).
- **Fixed here: one name.** `Buff7bFirstApply` (buff port Phase 7b) -> `LogFirstBuffBehaviorApply` (`junecollapse.py apply` with a scratch map; map row updated).
- **Rename audit (all 577 rows):** no `old` survives in code (169 mentions, all comments); every `new` is present; no new name overrides or collides with a base member.
  Same-name non-virtual pairs (`DashJumpStart` family and `DamageSmall` / `DamageBig*` in `CX2GUUser` and Add / Ara / Chung / EL / Elsword / Lire / Aisha) bind as before:
  every June caller is `SET_CB_FUNC( CX2GUUser, ... )`. The three `Handler_EGS_*` renames wait for C5's `EGS_JUNE_*` event-id rows (noted in the C5 kickoff).
  The tool-refused names: `IsSkillSlotState` (static, `X2GUUser.cpp:16236`), `IsUnitTranscended` (`_CONST_GUUSER_CH1j_`), `ParseCommonRandomStateByUnitClass` (CX2GUUser member).
- **Routed:** `JUNE_DI_EVENT_2017_TREASURE` (41470) is named in June's `DungeonEnum.lua:573`; its C5 row is now an `enum` row to `SEnum::DI_EVENT_2017_TREASURE`, registered
  in `KncWX2Server/Common/Enum/DungeonEnum.h` first (C5 kickoff). `ResetLeftSkillCoolTimeAll()` (March, 0-arg) stays beside June's mode overload: June's only caller of
  328941a0 is Eve's booster, so the 0-arg's June counterpart is not identified; nothing to do in a C phase.
- **Hygiene, C3** (`flagcheck <FLAG> --range fb062a6b^..fb062a6b`): ARME_CORE `1 PASS, 65 OTHER, 0 FAIL, 1 ERROR` (36 deleted), EVE_CORE `1 PASS, 65 OTHER, 0 FAIL, 1 ERROR` (35),
  ARME_LIVE `0 PASS, 66 OTHER, 0 FAIL, 1 ERROR` (13), EVE_LIVE `0 PASS, 66 OTHER, 1 FAIL, 0 ERROR` (13), ENCHANT_PARITY `1 PASS, 65 OTHER, 1 ERROR` (15), SOCKET_PARITY
  `1 PASS, 65 OTHER, 1 ERROR` (14), SKILL_TEMPLET `0 PASS, 66 OTHER, 1 ERROR` (13), FINAL_DAMAGE_UNIT_HOOK `0 PASS, 66 OTHER, 1 FAIL` (13). Every FAIL / ERROR is
  `KTDXLIB/Always.h` (+13 -13): the identifiers inside 13 `#error ... needs ...` message texts renamed with their functions (`HasJuneForceCinematicState` -> `HasForceCinematicState`, ...),
  which the per-flag mode cannot evaluate in a compound `#if`; no `#define` or condition changed. All-flags (`--with` the other 455, `--base fb062a6b^ --at fb062a6b`): `1 PASS, 65 OTHER,
  0 FAIL, 1 ERROR`, the ERROR again `Always.h`; no code file is non-PASS. Deleted lines are the removed `June_X` declarations / definitions and renamed lines.
  `enumscan --check` (8 files): 2 A/B rows, `X2GUUser.h:2550-2551` (NUI 4342 / 4343), from `f07940fe`, not on a C3 line.
- **Hygiene, this review** (`--worktree --files` the 4 code files): SKILL_TEMPLET `1 PASS, 3 OTHER` (4 deleted), ARME_PASSIVE `1 PASS, 3 OTHER` (4), EVE_PASSIVE `1 PASS, 3 OTHER` (16):
  RESULT PASS each; the deletions are the dead dispatches. BUFF_DEBUG: `1 PASS, 3 OTHER`, RESULT FAIL only for "not #defined" (`Always.h:4788` keeps it commented out; the rename
  sits in that compiled-out block). Encoding unchanged.
- **Smoke logs:** none (user, 2026-10-29: one smoke at the end of the plan). It must take Elsword and Eve or Aisha through a dungeon with B-slot skills on both, end a
  hyper mode early on Elsword, and finish the dungeon (result invincibility).
- DEPLOY OK: X2_offline.exe is 25,816,576 B, dated 2026-10-29 09:03:02, md5 9d9d18bbd44a418f5e7784a7e1ddbeef, equal to X2/US_SERVICE/x2.exe (2026-10-29 09:02:50); built at main d0b2b8f9 + 4 uncommitted code file(s).

## Phase C4 - Collapse UI, data, engine, offline; class, file and Lua names (Sonnet 5.5, high)

**Depends on:** C3 review. Quest (`CX2UIQuestNewJune` -> its March twin or a pattern name, file + vcxproj), skill-tree UI, result adapter,
emblem, `X2Main`, `KTDXApp` and the rest of KTDXLIB, X2Lib/Offline; the 10 Lua-bound names and every core-list Lua that calls them
(`luasrc.py refresh`, `luabind.py check`; pack, ask before copy). Smoke: quest window, skill tree, dungeon result, character info.
**Review:** `C4 review` (Opus).

### Done (2026-10-29, Sonnet 5.5; no new flag, each body keeps its flag)

`python tools/junecollapse.py apply C4`: 726 rows, 184 files written (565 renames, 55 classes, 1 merge, 8 Lua names, 2 file renames by the tool). Three follow-up
maps (`june2017_re/diverge/c4{x,y,z}_followup_map.tsv`, run with `--allow-existing --base $(git stash create)`) finished the rest: **x** 39 shared / half-done names
(`KJuneGfxResult*` / `SceneFrame` / `SceneSymbol`, the 15 packet structs, `CX2JuneBuffCapture` / `BuffRuntimeScope`, `ClearJuneActiveMemo`, the `JuneResult*` state
members, `JuneKimGuard` / `KimLine`, `SJuneVillageNpcRenderOption`, ...), **y** 6 renames + 17 quest twins, **z** the 14 `EGS_JUNE_*` event ids (C5 rows, pulled forward:
`DECL_PACKET( X )` derives the struct `KX`, so a `KEGS_JUNE_*` rename without its id does not compile; C5's rows for them now find nothing). Flag-off equivalence PASS per file.
**Tool fixes** (`tools/junecollapse.py`, `--selftest` PASS): the flag-off check allows for file-name renames in comments (`Always.h`); a `file` row whose `new` is not a name
is a hand row; `--allow-existing` for finishing a half-done rename; `KncWX2Server/Common` is scanned (the packet names live there).
**Hand shapes (`bytepatch.py`):**
- Overloads, not merges: `SetShow( bool, int hideMode )` (`CKTDGUIDialogManager`; `KTDXApp.cpp` Lua binding casts to the 1-argument form), `GetAddMaxMPValue( int )` (`CX2Inventory`).
  `CopyModelFrameStructure`: June's out-of-line body under `SERV_IRUHADEV_JUNE_SKIN_MESH_RENDER_TYPE`, March's `__forceinline` in `#else`. `GetXSkinAnim`: also under `JUNE_MOVABLE_LINE`
  (March's is tool-only). `SetNowDragItemReturn`: June's alias wrapper gone, the March member is public under the flag. `LEVEL_UP_EVENT_ID`: `LUEI_CHANGE_CLASS_GUIDE_TRANSCENDENTAL = 6`
  inside the March enum under the flag. `SkillTypeRate`: global `typedef std::pair<int,float> SkillTypeRate`, the class typedef reads `::SkillTypeRate`.
- Field merge: `m_bJuneUseRenderParam` (+0xa2c, never set here) and `GetJuneUseRenderParam` are gone; the June typed pass reads March's `GetUseRenderParam()`.
- `CX2UserSkillTree::DoIHaveThisSkill`: June's `GetSkillLevel( id, false, 0, true ) > 0` under `SKILL_TREE_UI`, March's learn-now fallback in `#else`.
  `CX2SkillTree::SkillTemplet::GetSkillEffectiveDescValue`: declared under the flag in `X2SkillTree.h`, June's body defined in `X2UISkillTreeNew2015.cpp` (it is a compile unit of
  `X2UISkillTreeNew.cpp`; the two format helpers stay in its anonymous namespace), March's inline in `#else`; the four UI call sites call the member.
- Quest: the whole body of `X2UIQuestNewJune.cpp` was byte-copied into `X2UIQuestNew.cpp` before `#endif SERV_EPIC_QUEST` (`git rm` + vcxproj line), 15 twins merged by the tool, `GetNPCFaceTextureKey`
  (March takes the enum, June's `int` body now sits in it) and `ResetQuestUI` (default arguments differ) by hand, 16 dead dispatches removed (15 quest + `X2StateMenu`), and 5 file-local
  helpers hoisted above their first use into `_CONST_UIQUESTNEW_HELPERS_`. June's NPC ids in that switch stay numbers (moved verbatim, not touched code of this phase's own).
- Files renamed: `X2OfflineQuestMig.h`, `X2OfflineSkillAutoLearn.h` (`git mv`, vcxproj, `#include`s). Lua: none of the 11 bound names is called by any `.lua` in `kom_contents`; no Lua edited, nothing to pack.
**Reach points** (March callers now run the June body when the flag is on): `UpdateMenuString` (`JUNE_BOTTOMBAR_LAYOUT`, `X2StateMenu.cpp`), the 15 quest twins (`JUNE_QUEST_UI`; March callers already
returned into them, unchanged), `DoIHaveThisSkill` (every caller: `X2UserSkillTree.cpp` can-learn / can-reset chains, `X2UIGuildSkillTree`), `GetSkillEffectiveDescValue` (`X2UIGuildSkillTree.cpp` x2),
`GetUseRenderParam` (the June typed pass now collects March's glow meshes).
**Hygiene:** all-flags flag-off check (`flagcheck JUNE_QUEST_UI --base HEAD --with` the other 455 flags, 204 files): `2 PASS, 201 OTHER, 1 FAIL, 1 ERROR`; the FAIL is the deleted
`X2UIQuestNewJune.cpp` (its only content outside the flag was `#include "StdAfx.h"`), the ERROR `Always.h` (a compound `#if` around renamed `#error` texts). `enumscan --check` (9 hand-edited files): 0 A/B rows.
Gone from code: all 667 old C4 names (`leftover` scan, comments stripped), except the `keep` row.
DEPLOY OK: X2_offline.exe is 25,809,920 B, dated 2026-10-29 09:57:16, md5 6a33fb1f07faca0c32b57824b3cacf30, equal to X2/US_SERVICE/x2.exe (built at main 5883066e + 206 uncommitted code files).
EXESTR OK: `ResultOpen` / `ResultClose` present; the old `JuneGfxShow` / `JuneGfxCutIn` / `JuneResultOpen` / `JuneEmblem` / `JuneBuffTestNewSkill_LUA` absent (`GfxShow`, `GfxCutIn`, `Emblem_LUA`,
`BuffTestNewSkill_LUA` are compiled out: dev bindings under flags that are off).
**Smoke:** none here (user, 2026-10-29: one smoke at the end of the plan). It must open the quest window (all tabs, a quest detail, Remind / Skip Epic), the skill tree (a learned skill's
description, a skill with a memo), the dungeon result, the character info, the bottom bar, and wear a glowing (Ara) weapon in a village.

## Phase C4 - Review (Opus 5.5, high)

### Done (2026-10-29, Opus 5.5; reviews `913a6aba`; fixes keep each body's flag)

**Verdict: C4 passes with one missed merge and three missed Lua strings, fixed here.** The rest of the merges, the overloads, the vcxproj edit and the reach points read right.
- **Fixed: a missed twin.** `JuneUpdateMenuPosition` (June `3245bb70`) was renamed to `UpdateMenuPosition` beside March's `UpdateMenuPosistion` (March's spelling), whose
  body forwarded to it under `JUNE_BOTTOMBAR_LAYOUT`; funcmap 4684 maps `3245bb70` to `CX2StateMenu::UpdateMenuPosistion`. Merged by hand (`bytepatch.py`): June's body inside
  March's function under the flag, March's in `#else`; the separate definition, its declaration and the forwarder are gone. `collapse_map.tsv` row now `merge`.
  A scan of every C4 rename against funcmap (June address named as a differently spelled March function of the same class) found no other twin: the rest are pieces
  carved out of a larger June function (`UpdateUnitPointerNPCWalk`, emblem / quest / skill-tree event helpers) or a forwarder after the packet decode (`ResultEndHandler`).
- **Fixed: Lua strings the tool skipped** (their C++ names were renamed in C3, so the `lua` rows found no identifier): `"JuneBuffTestNewStat_LUA"` / `"JuneBuffTestNewSkill_LUA"`
  (`X2Main.cpp`, dev bindings under `JUNE_BUFF_DEBUG`, off) and the class name `"CX2JuneSkillTypeColorTable"` (`X2UISkillTreeNew2015.cpp`, a metatable name; `SkillTypeColor.lua`
  calls only `g_pSkillToolTipManager:AddSkillTypeColor`). Two usage comments (`Emblem( 7 )`, `GfxCutIn( ... )`) and the copy-ready console lines in `kickoffs/june-result/Z.md`,
  `kickoffs/june-gfx/G3.md` and `MODS.md` (3) now name the new bindings; older plan docs are history and keep the old names.
- **Merges read against June:** `DoIHaveThisSkill` = June `32890a30` (`GetSkillLevel( id, false, 0, true ) > 0`), the moved helper body word for word; `GetSkillEffectiveDescValue`
  (`328ee280`) = the old free function with the templet as `this` (the NULL test goes with it); `GetNPCFaceTextureKey` / `ResetQuestUI`: June's had no defaults and every June caller
  already went through March's name; `SetShow( bool )` forwards mode 0 and the Lua binding casts to it; `UpdateMenuString` is the studio pattern. Overloads: no rename shadows a
  base or class member (24 same-name rows, all intended merges); the one file-local clash, `IsHeroDungeon` in `X2PartyUI.cpp`, forwards to the qualified static.
- **Reach point corrected: `GetUseRenderParam`.** The field merge is right (June's draw `31d30380` tests mesh `+0xa2c`, the field June's equip glow `3295b970` sets), but June
  sets it only together with the `+0x980` param. March's Ara cases in `CX2Eqip` (41510 / 41640 / 41700 / 41736 / 41743) set the flag alone, so those weapons now get a second
  pass with the default param (`RT_REAL_COLOR`, opaque) over the cartoon one; before C4 they had no glow pass at all. Cosmetic, Ara only; routed to june-engine R6b (ready),
  which ports both June writers (kickoff note added). The end smoke's Ara weapon check expects this until R6b.
- **vcxproj:** one line deleted (`X2UIQuestNewJune.cpp`), BOM and CRLF kept, no entry names a missing file; the two renamed headers are reached by `#include` only (as before).
  Built from a clean PCH below.
- **Lua names:** 11 C4 `lua` rows = 8 strings the tool renamed + the 2 `BuffTestNew*` + the colour-table class. No `.lua` in `kom_contents` or `june_src_fixed` calls an old or new
  name (`Emblem` appears only as `SetName( "Emblem" )`); no new string is bound twice. Only `ResultOpen` / `ResultClose` are live (`JUNE_RESULT_DEBUG`).
- **Routed to C5** (kickoff): rows 2529-2532 point at the old header paths; `GetNPCFaceTextureKey`'s 55 June NPC ids (moved in C4) all have `NUI_*` names; `s_kJuneSkillTypeColorTable`
  and the two `X2Offline*` namespaces are already C5 rows. No anonymous namespace was added (59 before and after); the C4 helpers sit in `_CONST_UIQUESTNEW_HELPERS_`.
- **Hygiene, C4** (`flagcheck <FLAG> --range 913a6aba^..913a6aba`): QUEST_UI `0 PASS, 204 OTHER, 1 FAIL, 0 ERROR`, SKILL_TREE_UI `3 PASS, 200 OTHER, 1 FAIL, 1 ERROR`,
  SKIN_MESH_RENDER_TYPE `3 PASS, 201 OTHER, 1 FAIL`, MOVABLE_LINE `0 PASS, 204 OTHER, 1 FAIL`, BOTTOMBAR_LAYOUT `1 PASS, 203 OTHER, 1 FAIL`. All-flags (`--base 913a6aba^ --at 913a6aba
  --with` the other 455): `2 PASS, 201 OTHER, 1 FAIL, 1 ERROR`. Every FAIL is the deleted `X2UIQuestNewJune.cpp` (its only flag-off content was `#include "StdAfx.h"`); every ERROR is
  `KTDXLIB/Always.h`, whose 16 changed lines are `#error` texts and comments inside compound `#if`s the tool cannot evaluate. No code file is non-PASS.
- **Hygiene, this review** (`--worktree --files`): BOTTOMBAR_LAYOUT (`X2StateMenu.cpp` / `.h`) `2 PASS`, RESULT PASS (116 deleted lines: the separate definition and the forwarder);
  SKILL_TREE_UI (`X2UISkillTreeNew2015.cpp`) `1 PASS`, RESULT PASS; `X2Main.cpp` / `X2EmblemManager.cpp` sit in `JUNE_BUFF_DEBUG`, `JUNE_GFX_TALK_CUTIN_DEBUG` and
  `JUNE_SKILL_EMBLEM_DEBUG`, which `Always.h` keeps commented out: RESULT FAIL only for "not #defined", the files PASS / OTHER. Encoding unchanged. `enumscan --check` (4 files): 0 A/B rows.
- **Smoke logs:** none (user, 2026-10-29: one smoke at the end of the plan).
- DEPLOY OK (`build.py --deploy --pch force`, every project's PCH rebuilt): X2_offline.exe is 25,809,920 B, dated 2026-10-29 10:30:35, md5 63f32d873a14855c7e00c834d38acfe8, equal to
  X2/US_SERVICE/x2.exe (2026-10-29 10:30:29); built at main 913a6aba + 5 uncommitted code file(s).
  EXESTR OK: `CX2SkillTypeColorTable` / `ResultOpen` / `ResultClose` found; `CX2JuneSkillTypeColorTable` / `JuneBuffTestNew{Stat,Skill}_LUA` absent (the new `BuffTestNew*` strings are compiled out).

## Phase C5 - Members, locals, constants, namespaces (Sonnet 5.5, high)

**Depends on:** C4 review. `m_*June*` members (merge into a March twin of the same type and meaning, else rename), `bJuneMemo`-style
locals, `JUNE_*` constants (an enum alias -> the member), `June32481fa0`, the 29 `_CONST_*_JUNE_*_` namespaces -> `_CONST_<CLASS>_<phase>_`.
Build + deploy; smoke = C1-C4's lines. **Gate:** `junecensus.py --summary` shows 0 June-named identifiers outside flags.
**Review:** `C5 review` (Opus).

### Done (2026-10-29, Sonnet 5.5; no new flag, each name keeps its flag)

**Gate: `junecensus.py --gate` = `0 June-named identifiers remain`** (was 183 after the batch run; locals 0, constants 0, data members 0, namespaces 0 June / 32 rule-form).
- **Batch run** `junecollapse.py apply C5` (1,584 rows, 183 files, checks PASS): 1,379 auto (rename 1,131, enum 211, namespace 32, drop 5), 205 hand. Prerequisite: `DI_EVENT_2017_TREASURE = 41470`
  registered in `DungeonEnum.h` under `JUNE_ENUM_NAMES`; map rows 2529-2532 pointed at the renamed `X2OfflineQuestMig.h` / `X2OfflineSkillAutoLearn.h`.
- **Tool change** (`tools/junecollapse.py`, `--selftest` PASS, README row): `--scope-locals`. The old collision check refused a local whose new name appeared anywhere in the tree
  (115 of the 205 hand rows: `pUnit`, `fValue`, ...). The new check refuses only when the name is used in the same function (column-0 braces) on a branch the old name can meet
  (`#ifdef` / `#else` branches are exclusive), and two renames to one name in one function. Follow-up maps (`june2017_re/diverge/c5{x,y,z,w,v}_followup_map.tsv`, run with
  `--base $(git stash create)`): x 151 rows (123 auto, 28 hand), y 15 (13), z 3, w 2, v 2. Every file's flag-off equivalence check PASS.
- **Hand work (`bytepatch.py`):** `m_iJuneAttackSerial` uses in `X2GUNPC.cpp` / `X2GUUser.cpp` (shared name, two classes) -> `m_iAttackSerial`; `akRSICKTDGUIDialogJune` merged into
  `akRSICKTDGUIDialog` (`KTDGManager.cpp`: June's rows under `JUNE_RENDER_DG_STATES`, March's in `#else`, assignment site collapsed); `s_akStatesJunePostEffect` ->
  `s_akStatesPostEffect` (file-local, `KTDGDeviceFont.cpp`); the duplicate Aisha macro `JUNE_ARME_CH2A_ANY_KEY_ZXC` (same body as `ARME_ANY_KEY_ZXC`) deleted, its one use renamed;
  `JUNE_AT_COMMAND` local -> `CX2DamageManager::AT_COMMAND` (`X2Game.cpp`); `ENSI_JUNE_DASH_COMBO_ZX` -> `ENSI_DASH_COMBO_ZX` (March's twin exists only under the undefined
  `EVE_COMBO_TEST_TEST`); 12 locals given their own names where the function already used the plain one (`pCandidateLineData`, `pEffectInstanceToSilence`, `eSocketUnitType`,
  `iSoundPlayIndex`, `iStateSoundPlayIndex`, `pPiercingEnchantData`, `vUserPos` / `bUserIsRight`, `vFlyDownPos`, `iCanDownLineDataIndex`, `pStageUser`, `pDamageAttackerUnit`,
  `iClearDifficulty`); `m_fJuneCoolTime` -> `m_fIntervalTime` (March's `m_fCoolTime` is another field); `m_fJuneCreatorPowerRate` -> `m_fCreatorPowerRate` (`KBuffFactor`).
- **Found and fixed: the map's `enum` rows matched by value across enum families (D0b's `cl /P` value lookup).** Two groups were wrong and had been applied:
  (1) the ten `JUNE_CH1I_UAI_*` / `JUNE_CH1L3_UAI_*` user-action ids (`nowAction` is `USER_ACTION_ID`) were replaced by `BFI_DEBUFF_MOVE_JUMP_SLOWDOWN_LEG_SHOT`, `BTI_DEBUFF_FIRE`,
  `BTI_DEBUFF_CURSE`, ... (same number, other enum). June's `Enum.lua` names none of them, so they are constants named from use in the Arme / Eve / `X2GUUser` namespaces:
  `UAI_ERASE_SPECTER_WALK` (0x25), `UAI_CREST_OF_TIME_REVIVAL` (0x29), `UAI_MAGNIFYING_THIEF_USE_ITEM` (0x36), `UAI_AMUSEMENT_ITEM_PRESS` / `_MISSILE` / `_WATER_BALLOON` (0x37-0x39);
  the case labels in `UserNoStateChangeActionFrameMove` (Arme, Eve) are back on them. (2) `JUNE_EDT_MANA_DAMAGE_WITH_NO_DISPLAY` became `BFI_DEBUFF_SIDE_EFFECT_MEDICINE` in
  `X2DamageManager.cpp`; June names it `EDT_MANA_DAMAGE_WITH_NO_DISPLAY = 67` (EXTRA_DAMAGE_TYPE), now registered in `X2DamageManager.h` under `JUNE_ENUM_NAMES` and used.
  All other 200 enum rows have a family-matching name. Kept by the rule "March's name when names share a value": `DTT_END` for June's `DTT_PANDEMONIUM_FEAR` (6),
  `BTI_HYPER_MODE` for the hyper skill category (March's own idiom).
- **Names from use** (C2 review list): `ARME_INPUT_8726` -> `ARME_INPUT_SIT`, `STUCM_UNNAMED_29075` -> `STUCM_UNUSED_PLACEHOLDER`; the `UAI_*` set above. `REASON_PRIDE_ACTION_SCRIPT_STATE_ID` (242) is in
  the dead `#else` of `JUNE_CHAR_EVE_SKILL`; the live branch passes `ENSI_SA_ECE_REASON_PRIDE_ACTION` (196), so the name is only March-side text.
- **Quest NPC faces:** `GetNPCFaceTextureKey` cases and `SetNpcFacePicture` use `CX2UnitManager::NUI_*` (58 names, values checked with `june2017_re/enumprobe.py`). C++ lacked six:
  `NUI_EVENT_GUILD_EDAN_ROOM` 4254, `NUI_EVENT_DOLL` 4309, `NUI_EVENT_BLESS_OF_SOLACE` 4346, `NUI_STORY_QUEST` 10142, `NUI_TRANSCEND_QUEST` 10153, `NUI_PANE` 10218 are registered under
  `JUNE_ENUM_NAMES` in `X2UnitManager.h`. 10067 is March's `NUI_EVENT_ADAMS_UI_SHOP` (June's `NUI_EDEL`, but March's `NUI_EDEL` is 10072): March's name, comment names June's.
- **String literal found by EXESTR:** `X2OfflineIgnore.cpp` listed `L"EGS_JUNE_FAST_SOLO_CLEAR_NOT"` by name, so the renamed id would have logged UNHANDLED; now `EGS_FAST_SOLO_CLEAR_NOT`.
  A scan of every C1-C5 old name inside string literals of X2Lib / KTDXLIB / Common found no other. Old names in comments are left (rule).
- **Reach points:** none new; renames and the one `akRSICKTDGUIDialog` merge (June's rows only under `JUNE_RENDER_DG_STATES`, as the old forwarding already did).
- **Hygiene:** all-flags `flagcheck JUNE_QUEST_UI --worktree --with` the other 417 `JUNE_*` flags over 188 files: `2 PASS, 185 OTHER, 0 FAIL, 1 ERROR` (the ERROR is `Always.h` line 10007, the
  compound `#if` around renamed `#error` texts, as in C4). `enumscan --check` (9 hand-edited files): only the new registrations and `Class A` rows on the enum definitions themselves.
  `bytepatch info`: encodings unchanged; `KTDGManager.cpp` lost its 9 CRs to a `sed -i` and got them back (memory note written).
- DEPLOY OK: X2_offline.exe is 25,809,408 B, dated 2026-10-29 11:42:36, md5 1207168dea16aed958a259217dd54e54, equal to X2/US_SERVICE/x2.exe (built at main cd29f6ac + 189 uncommitted code files).
  EXESTR OK: `EGS_RANDOM_MISSION_NOT` / `EGS_FAST_SOLO_CLEAR_NOT` present (wide); `JuneBuff` / `EGS_JUNE_FAST_SOLO` absent.
- **Smoke:** none here (one smoke at the end of the plan). Add to it: Eve and Aisha amusement-item pickups (press / missile / water balloon: the `UserNoStateChangeActionFrameMove` cases),
  a mana-drain hit that shows no number, and the quest window NPC faces (story and transcend quest NPC, Edel).

## Phase C5 - Review (Opus 5.5, high)

### Done (2026-10-29, Opus 5.5; reviews `09107672`; fixes keep each site's flag)

**Verdict: C5 passes. Two enum aliases from another family and one bare skill id on a touched line were fixed here.** Census gate, member merges, names and `--scope-locals` read right.
- **Ruling on "March's name when names share a value":** the rule covers two names for one member of one enum. It does not cover March's count sentinel (`*_END`)
  or a member of another enum, so both C5 "kept" cases were wrong:
  - `DTT_END` stood for June's `DTT_PANDEMONIUM_FEAR` (6) in `CX2CustomFunctionByBuffPandemoniumFear::FunctionInDamageReact` (`X2BuffTemplet.cpp`, both `JUNE_DAMAGEDATA` branches).
    March's `DTT_END` is the trigger count and moves with `EVE_ELECTRA` / `ADD_EVE_SYSTEM_2014` / ... (5 when `ADD_EVE_SYSTEM_2014` is off). `DTT_PANDEMONIUM_FEAR = 6` (June `Enum.lua` 7306) is now
    registered after `DTT_END` in `X2DamageManager.h` under `JUNE_ENUM_NAMES`, and both sites use it.
  - `BTI_HYPER_MODE` (a buff templet id) was compared with `SkillCategory()` (June `32070510`'s 0-6 categories, no Enum entry) in Eve's cool-time reader (`X2GuEve.cpp`). C5 had dropped the constant
    `JUNE_CH4B_SKILL_CATEGORY_HYPER`; it is back as `SKILL_CATEGORY_HYPER = 5` in `_CONST_EVE_CH4b_` (`JUNE_CHAR_EVE_PASSIVE`). Eve's other reader keeps its bare `5` (not touched by C5).
  - Rows checked: all 211 C5 `enum` rows by family (old name's prefix and use site against the new member's enum); `_END` / `_NONE` / `_MAX` targets in every collapse map: only `DTT_END`.
    The 167 prefix mismatches are skill ids whose June constant had no family prefix (`JUNE_CH2A_SA_*` -> `RNSI_SA_*`), plus the UAI / EDT rows C5 fixed. `JUNE_CH1K_P_ECN_NASOD_WEAPON_MEMO` ->
    `RNSI_P_ENS_CHEAT_CODE_DESTROY` (25416) is June's own name for that skill id, so the porter's old name was wrong, not the map.
- **Fixed: `29024` on a C5-touched line** (`X2BuffTemplet.cpp` 2220, `JUNE_SOCKET_PARITY`): `RNSI_SA_ADE_MOONLIGHT_RHAPSODY = 29024` (June `Enum.lua` 16380) registered in `X2SkillTree.h`'s
  `JUNE_ENUM_NAMES` block and used; `enumscan --check` (2 files) 0 A/B rows.
- **UAI constants:** the six `UAI_*` names match their case bodies (specter-walk erase, crest-of-time revival, magnifying-thief effect, the three `BFI_AMUSEMENT_ITEM_*` factors); none collides
  with March's `CX2GUUser::USER_ACTION_ID`, which would win unqualified lookup.
- **Members** (Sonnet audit of 516 `data member` / `global variable` rows, spot-checked): 11 new names already existed at the parent, all in unrelated classes (e.g. `m_iAttackSerial` in
  `CX2GameUnit` and `DamageData`, both int, a deliberate copy). No same-class or base / derived clash. The one in-class clash (`m_fCoolTime` in `InfoByUseHyperMode`) was already avoided
  by c5y's `m_fIntervalTime`. `PsykerKinesisData` `m_fRemainTime` / `m_fPowerRate` match their reads. No new duplicate declaration on two live branches.
- **Names:** no address, phase tag or `June` left in a C5 identifier (namespaces keep the rule-form `_CONST_<CLASS>_<phase>_`). Kept: `s_lSkinKem433` / `SkinAnimKem433`, named after
  March's own `KEM_ERROR433`, which they log; `s_bSocket215434Logged`, after a socket option id; `STUCM_UNUSED_PLACEHOLDER` (the June gap at 29075); `NUI_EVENT_ADAMS_UI_SHOP` for 10067 in
  `GetNPCFaceTextureKey`, by the rule (same family, same value; no C++ reader uses March's `NUI_EDEL` 10072).
- **`--scope-locals`** (`local_conflict` / `exclusive`): sound and conservative. A function ends at a column-0 `}`, and `#ifdef` branches count as exclusive only across one `#else`. Blind spot: a column-0 `}`
  inside a function body narrows the scope. The flag-off equivalence check and the build cover it.
- **Hygiene, C5** (all-flags: `flagcheck JUNE_QUEST_UI --range 09107672^..09107672 --with` the other 417 `JUNE_*` flags):
  `files: 2 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 186 OTHER (other flags), 0 FAIL, 1 ERROR`; `in this flag's files: 150 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched
  inserted line(s), 15 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: FAIL` from that ERROR only. The ERROR is `Always.h` 10007, the `ADMIN_CHEATS` compound
  `#if`, byte-identical before and after. The commit changed 4 `#error` texts there. The 150 deleted lines are 146 renamed lines in `X2UIQuestNew.{cpp,h}` (142/142, 4/4) plus those 4 texts.
  The 186 OTHER rows are `+0` with the change owned by another flag.
- **Hygiene, this review** (`flagcheck JUNE_ENUM_NAMES --worktree --files X2GuEve.cpp X2BuffTemplet.cpp X2DamageManager.h --with JUNE_CHAR_EVE_PASSIVE JUNE_DAMAGEDATA JUNE_BUFF_CUSTOM_FUNCTION
  JUNE_CHAR_EVE_CORE`): `1 PASS, 1 PASS~, 1 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`. The 1 deleted line is the C5 `BTI_HYPER_MODE` line, replaced. `X2SkillTree.h` gets one line
  inside an existing `JUNE_ENUM_NAMES` block. `bytepatch` before / after: encodings and EOLs unchanged.
- **Smoke logs:** none (one smoke at the end of the plan). Add to it: a Pandemonium Fear holder taking a hit (the buff's damage react), and the cool time of a non-hyper skill with Eve's
  Cheat Code: Electronic learned.
- DEPLOY OK: X2_offline.exe is 25,809,408 B, dated 2026-10-29 12:10:52, md5 4cde57bff57b9f54d0ac94d741929b2e, equal to X2/US_SERVICE/x2.exe (2026-10-29 12:10:45); built at main
  09107672 + 4 uncommitted code file(s) (PCH rebuilt for the two headers). No string to check: the fixes are enum names and a constant.

## Phase D1 - Replace divergent March functions at the definition (Opus 5.5, high)

**Depends on:** C5 review and every D0c. Per `replace` row of `june_march_divergent.tsv`: March's definition becomes June's whole body
under `SERV_IRUHADEV_JUNE_DIVERGE_<NAME>`, March's in `#else`, `MODS.md` flag + revert line, guard + `guards.tsv` row if an unported class
would crash, `JUNE_BUGS.md` row if June's answer is a bug. First: `GetEqippedSkillMemo` -> `return false;` (June `31fca9e0`; fixes VioletMage
27702 without touching it). Hand-off: per visible change, one ported and one unported class.
**Review:** `D1 review` (Opus): each body against its listing, `#else` intact, flagcheck per flag, one unported call site per row.

### D1 split (2026-10-29, D1-1 session)

The 88 `replace` rows (87 functions) each need June's whole body read in Ghidra, so D1 runs as five sub-phases by area (tsv line numbers),
each Opus 5.5 high, kickoffs `D1-1.md` (= `D1.md`) .. `D1-5.md`; reviews pair them like C2's: `D1-2 review` (D1-1 + D1-2), `D1-4 review`
(D1-3 + D1-4), `D1 review` (D1-5, closes D1). No per-phase play-test: the smoke runs once at the plan end (user, 2026-10-29).
- **D1-1** unit core: 143, 210, 17 / 50 / 192 / 193 / 260, 54, 122, 203, 234 / 258, 245, 246, 256 (done below).
- **D1-2** user / NPC states and combat: 237, 238, 239, 240, 241, 242, 243, 244, 247, 249, 371, call sites 330, 364.
  (done below, but 242 / 244). **D1-2b** (split in the D1-2 session): 242 `CommonCameraMove`, 244 `DyingStart`; reviewed in `D1-2 review`.
- **D1-3** equipment, enchant, riding pet, items, skill tree: 205, 250, 251, 252, 261, 262, 263, 266, 267, 268, 269, 270, 274, 275, and the
  missed twin `CX2GUUser::GetSkillLevel` (June `31fceab0`, 55 callers) folded into `GetSkillLevelValue` (done below, but 205 / 250 / 251 /
  252 / 263). **D1-3b** (split in the D1-3 session): 252, 250, 251, 205, 263 and June's cool-time rate `3234cc10`; reviewed in `D1-4 review`. D1-3b ported 252 and `3234cc10` and split again: **D1-3c** 250, 205, 263 and two D1-3b leads (the weapon-effect show test in the enchant frame moves, the MP rate sites); **D1-3d** 251 alone. Both reviewed in `D1-4 review`. D1-3c turned the glow on, ported the viewer's glow and the five weapon enchant frame moves, and split again: **D1-3e** the at-hand effect `31ff8510`, the viewer's enchant frame moves and 263; **D1-3f** 250, 205 and the MP rate sites. D1-3d ported 251 and, to fill its groups, June's division group parser `32814170` with the Cobo pairs. D1-3e ported the at-hand effect and split again: **D1-3g** the viewer's whole attribute-enchant family (June's two handle lists); 263 routed to **june-engine R6c** (June-only equip list). D1-3f ported 250 (June's `DefaultBoneInfo.lua` table) and split again: **D1-3h** 205 and the reform bone-restore lead `31fde420`. D1-3h ported 205 (base and Aisha) and the lead (`RestoreHead`); the riding-pet branch went to Stage 17. **D1-3g moved out of this plan (user, 2026-10-29, drift: no `replace` row, cosmetic, a callee family of row 252): it is an add-on to june-charinfo C4 (the unit viewer), brief `kickoffs/june-charinfo/C4-enchant.md`.**
- **D1-4** UI, quest, local map, play guide: 280, 285, 287, 288, 289, 290, 293-297, 299, 300, 301, 302, 306, 307, 314, 318, 326-329, 337, 339, 341-343
  (done below, but 342). **D1-4b** (split in the D1-4 session): 342 `UpdateQuickQuestDLG` with its three slot setters; reviewed in `D1-4 review`.
- **D1-5** KTDXLIB engine and buff popups: 346-363.

### D1-1 Done (2026-10-29, Opus 5.5; nine `SERV_IRUHADEV_JUNE_DIVERGE_*` flags, March's body in every `#else`)

| tsv | March function | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 143 | `CX2GUUser::GetEqippedSkillMemo` | `31fca9e0` | `SKILL_MEMO` | always false; VioletMage 27702's super armour and every March memo test go |
| 210 | `MemoCondition` decide | `31fd4450` | (143) | June's decide is false for every class, which `m_bApplyMemo = GetEqippedSkillMemo()` now gives: no code |
| 17, 50, 192, 193, 260 | gage `FlushMp` / `SetNowHp` / `SetNowMp` / `UpNowMp` | `32a31910`, `32081260`, `322b3010`, `322b33d0`; unit `3208dc70` / `3208dd00` / `3208dd20` | `GAGE_TALLY` | members +0x100..+0x10c (ctor `322b3a70` zeroes, copy `322b24f0` copies); unit `UpNowMp` tracks, `FlushMp` tracks `IsMyUnit()`; random-mission event 9 for any tracked spend (JUNE_BUGS 467) |
| 54 | `CX2Game::GetLockOnNearstTargetInRange` | `322d7670` | `LOCK_ON_IN_RANGE` | June's `LockOnData*` signature; the effect's twin (`X2DamageEffect.cpp`) now calls it; retry result dropped (asm @322d78a6, JUNE_BUGS 468) |
| 122 | `CX2BuffEffectSetDisplayerTemplet::StartDisplayer` | `32eb9c70` | `START_DISPLAYER` | awakening BTIs set June's skip flag: their sets take `AEACBO_NOT_APPLY`; BTI 660 / 661 registered |
| 203 | `CKTDXDevice::UnrefDevice` | `31d4d300` | `UNREF_DEVICE` | true only at 0, no clamp; the CS stands in for the interlock (as `CloseDeviceByPointer`) |
| 246 | `CX2GUUser::GetSkillLevelValue` | `31fceab0` | `SKILL_LEVEL_VALUE` | returns the ported `GetSkillLevel` (one body; hyper-class 0 before transcendence) |
| 256 | `CX2GameUnit::CureExtraDamage` | `3229dd90` | `CURE_EXTRA_DAMAGE` | sorts the whole list |
| 245 | `ForceChangeHyperModeWithoutMotion` | `31ff7300` | `FORCE_HYPER_MODE` | skip test `GetHypetMpdeStateID()` / `USI_RIDING_HYPER_MODE` (June 0x2c: June's USI runs one above March's here, funcmap 4085) |
| 234, 258 | buff `SetFactor` / `SetFactorFromPacket` sites | @32e7de0a | `BERSERK_BY_CHANCE` | `IsBerserkByChance` (vslot 0xa4) is now a public `CX2GameUnit` virtual (base `31e4c550` false); Chung's March berserk no longer forces full hyper |

- **Stand-ins (one-line comments):** `322d7670`'s NPC byte +0x8fb6 skip (member unidentified); `32eb9c70`'s equipped-item effect set
  (user +0x867c) - the awakening effect-set family already waits on Stage 6 (backlog Stage 5 close note).
- **Signatures not taken:** June's unused second `FlushMp` argument (`3208dc70` ret 8) and the gage's unused third; June's
  `SetNowMp( float, bool )` callers' bools are not known per site, so March callers pass false (tallies only; no reader found).
- **Routed:** the device-refcount IAT conflict (`31d4d2f0` R12 "AddRef" calls the decrement slot) -> backlog Stage 2 bullet.
- **Deferred to D1-3:** row 205 (riding-pet branch + unidentified equip +0x15d4 / unit +0x8e91 test).
- **Hygiene:** `flagcheck --worktree --files` per flag: `RESULT: PASS` for all nine (0 FAIL, 0 ERROR, 0 deleted lines, 0 non-ASCII,
  0 EOL mismatches); the multi-flag files pass with `--with` (`X2GUUser.cpp` SKILL_MEMO + SKILL_LEVEL_VALUE + FORCE_HYPER_MODE, `X2GameUnit.h`
  GAGE_TALLY + BERSERK_BY_CHANCE; `X2BuffFactor.h` under `JUNE_ENUM_NAMES` PASS~). `git diff --stat`: 14 code files, 368 insertions, 0 deletions.
  `tok!` rows are the plain `#else` lines (house style). `enumscan --check`: the 2 class-A rows are old lines (`X2DamageEffect.cpp:1705`,
  `X2BuffBehaviorTemplet.cpp:5261`), none in this phase's edits. `cl /P` of `X2GuEve.cpp` passed every new `#error` guard before the build.
- **Build:** first run failed in this phase's own edit (C2248, protected `IsBerserkByChance` through `CX2GameUnit*`), fixed by the base virtual.
  `DEPLOY OK: X2_offline.exe is 25,808,896 B, dated 2026-10-29 13:25:32, md5 b9a34c9fd7625c49f95058295780e64b, equal to X2/US_SERVICE/x2.exe
  (2026-10-29 13:25:25); built at main 997feb24 + 14 uncommitted code file(s).`
- **Smoke (plan end):** Aisha (ported) dash combo Z->Z->Z with memo 5 gives no super armour; Chung (unported) skill with memo 13 acts
  memo-less; Ara (unported) awakening and Add formation sets render undimmed with other-effect alpha lowered; a hyper buff on Chung
  in berserk with 2 orbs no longer forces full hyper.

### D1-2 Done (2026-10-29, Opus 5.5; nine `SERV_IRUHADEV_JUNE_DIVERGE_*` flags; rows 242 / 244 split to D1-2b)

| tsv | March function | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 237, 238 | `CX2GUNPC::StateChange` / `StateChange_Reaction_FIRST` (+ `_Continue`) | `32758fe0` / `327590c0` / `32759250` | `NPC_FREEZE_REFUSAL` | a live NPC holding a freeze effect (`m_vecFreezePtr`) refuses; June's `_Continue` has the same test, so it moved with them |
| 239 | `CX2GUUser::GetCombineMatrixFromBoneName` | `31fd9200` | `BONE_NAME_PREFIX` | `MAJOR_` is stripped and read on the main body; `MINOR_` (second body +0x8e88, Lu/Ciel) is a stand-in |
| 240 | `CheckSkillUsePVPOfficial` | `3202cba0` | `PVP_OFFICIAL_SKILL` | skill 900 refused outside E3 / E5: OFFICIAL / GUILD / EVENT_MODE string 24675, 1VS1 39561, else 28982; `PCC_GUILD` / `_EVENT_MODE` / `_1VS1_PVP_CHALLENGE` registered past `PCC_END` (ClientPacket.h; second home ServerDefine.h, `SECOND_HOMES`); PvP only, unreachable offline |
| 241 | `CheckSummonSpecialAttack` + `IsEventGamePlayable` | `3202daf0`, `3201bd60` (bool branch `322f43e0` / `322f42a0` / `322f4340`) | `SUMMON_SPECIAL_ATTACK` | Add's 6 summon skills always gate, others only if in June's list (`IsSummonSkillBy` = `31fc8200`), else free; refusal string 45041; on a last kill a summon still passes while the stage's GO arrow shows (`IsGoEffectOn`) or off the stage's last sub-stage; 3 Add skill names registered |
| 243 | `CX2GUUser::DieFrameMove` | `31ffcba0` (fade `31fe8220`) | `DIE_FRAME_MOVE` | no Sacrifice of Hero call (2nd float unused), card-monster exceptions + `NUI_CSM_LU_DIABLA` / `_CIEL_DEMONIO` (registered), no Christmas-soccer exception, no Gate-of-Darkness ally block, render type reset always, alpha-object flag left set |
| 249 | `CX2GUUser::SetCustomAnimName` (base) | `31fdaf30` (vslot 0x6ec) | `RIDING_ANIM_NAME` | riding states but DIE take `m_wstrRidingMotionName` (the pet's `RIDING_MOTION`, `Ride_1/2/3`), now applied after `RIDING_ANIM_NAME` (`32051266`, `32057b80`); Add's override calls the base first (`329e73a0`) |
| 371 | `CX2GUUser::RidingHyperModeEnd` | `32006c00` | `RIDING_HYPER_MODE_END` | a pending hyper buff is applied, as `HyperModeEnd`; Add guard (`guards.tsv`) |
| 330 | VioletMage Mana Flow Overdrive (call site) | `32cc5b20` -> `3229a3d0` | `MANAFLOW_BUFF_ID` | `GetSkillBuffTempletID( id, 0, true )`: learned only, at the unit's level |
| 364 | VioletMage `ChangeWeapon` (call site) | `32cc8980` | `CHANGE_WEAPON_TEMPLET` | old templet from the equip (`GetItemTemplet`); only the equip is tested |
| 247 | `PlayAnimationWaitStart` | `31fe1dd0` | none | June's extra branch resumes an emotion that only June's `PlayEmotion` saves (+0x89e0 @`320443f5`); March's has no save, so June's body equals March's (as row 210) |

- **Identified:** user +0x9200 = `m_wstrRidingMotionName` (ctor `32041b5b`, emotion `3249dcc4`, sit wait `31fde33b`); +0x89d8 =
  `m_ePlayedEmotion`, +0x89e0 = June's resume slot, `31fe0bb0` = June's emotion setter; June's riding ids from Add's registration `329b8030`
  (0x48 START .. 0x5c SPECIAL_MOVE, 0x57 DIE, 0x5d SPECIFIC, 0x5e-0x60 ATTACK_X_READY / _READY_2 / _END, none in March); unit +0xdb0 is a
  protected copy of `color.a` (synced by `32299af0`), so June's fade is March's.
- **Stand-ins (one-line comments):** the `MINOR_` bone; June's card-monster list (+0x91cc, filled by CreateNPC @`3230dae4`, killed in
  `DieFrameMove` under event code 2143), not in March.
- **Signatures not taken:** June's `SetCustomAnimName( int stateID, wstring& )` (March's state data carries `stateID`); `DieFrameMove`'s
  second float stays (unused). The old `IsEventGamePlayable` comment ("argument-true branch not ported") is not rewritten.
- **Routed:** the emotion resume (June `PlayEmotion` save, `31fe0bb0`, `31fd7b60`, StageLoad clear `327cda99`, `PlayAnimationWaitStart`
  branch) -> backlog Stage 10 bullet. No `JUNE_BUGS.md` row: no June answer here shows as a bug.
- **Split to D1-2b:** 242 `CommonCameraMove` needs June's `CX2Camera::PlayLuaCamera` time argument (`31fb5920`, wrappers `31fb6ab0` /
  `31fb6af0`, setter `31fae7b0`); 244 `DyingStart` needs June's die packet (3 more fields, emulator Put/Get + handler), the XMAS-2016 hook
  `322e60d0` and the black-out handles. Identification digest in `kickoffs/june-march-divergence/D1-2b.md`.
- **Hygiene:** `flagcheck --worktree --files <the 10 files> --with <the other 8>` per flag: `RESULT: PASS` for all nine (0 FAIL, 0 ERROR,
  0 deleted lines, 0 non-ASCII, 0 EOL mismatches, 0 `#else`/`#endif` without the token); OTHER rows are the sibling flags and the D1-4
  session's `Always.h` block. `JUNE_ENUM_NAMES` on `X2SkillTree.h` / `X2UnitManager.h`: PASS (1 PASS, 1 PASS~). Never list `JUNE_ENUM_NAMES` in
  `--with`: it makes old `Always.h` guards unevaluable (ERROR). `enumscan --check` on the four `.cpp`: 0 A/B rows.
- **Build:** `cl /P` of `X2GUUser.cpp` exit 0 (every new `#error` guard, all nine flags live). `DEPLOY OK: X2_offline.exe is 25,808,896 B, dated 2026-10-29 14:11:25, md5 75eb52b09a14913fc9391810d6b222c6, equal to
  X2/US_SERVICE/x2.exe (2026-10-29 14:11:18); built at main 5e4508ee + 21 uncommitted code file(s).` (the build also carried the
  D1-4 session's uncommitted files.) `EXESTR OK: 1 of 1 found in X2_offline.exe` (`MAJOR_`, wide).
- **Smoke (plan end):** an NPC holding a freeze effect takes no state change; on a riding pet every riding state plays the
  pet's rider pose (`Ride_N`); Add's riding dash too; Aisha (ported) and Elsword (unported) die: body fades and hides, no Sacrifice of Hero
  heal; in a dungeon, Add's summon skill on the last kill of the stage's last sub-stage is refused with string 45041.

### D1-2b Done (2026-10-29, Opus 5.5; three `SERV_IRUHADEV_JUNE_DIVERGE_*` flags, March's body in every `#else`)

| tsv | March function | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 242 (callee) | `CX2Camera::PlayLuaCamera` (3 overloads) + the 6 camera functions it calls | `31fb5920`, `31fb6ab0`, `31fb6af0`; `31fb3640`, `31faee30`, `31fae6d0`, `31fb3c50`, `31faf120`, `31fae740` | `LUA_CAMERA` | anim-time argument (default 0); `TILTANGLE` -> roll `-( deg * 0.017453292 )`; `LINEARMOVING` in both tracking modes: tracking time minus the played time (clamp 0) and linear tracking; parts-look modes need my unit; no bone-attached case (March's was compiled out); linear / roll passed to `SetTracking` / `Point` (engine side = R13c) |
| 242 | `CX2GUUser::CommonCameraMove` | `31ff5df0` | `COMMON_CAMERA_MOVE` | no `SUB_CAMERA_TIMING`: table 1 gets the whole anim time; else `SUB_CAMERA<i>` gets now minus its start; then `SetLinearTrackingTime( clamp0( now - before ) )` (anim `+0x214` / `+0x21c` = `GetNowAnimationTime` / `GetBeforeAnimationTime`); no world / line map -> return; default tracking skipped while the unit's `m_bHeldByTimerFocus` (June `+0x240`) is set |
| 244 | `CX2GUUser::DyingStart` + `KEGS_USER_UNIT_DIE_REQ` | `32014810` | `DYING_START` | attacked-by-user but killed by a non-user: outside `GT_PVE` the max-damage user is the killer, in `GT_PVE` an NPC attacker's UID; `PCC_GUILD` max-damage fallback to the killer; packet + `m_fPveSyncValue1/2`, `m_iPortalValue` (0, 0, -1; Put / Get; emulator reads it through the shared Get, no handler change) |

- **Identified:** `+0x240` = 1 while `CX2Game`'s timed focus holds the unit: writers `322d5f70 @322d6158` (held, after the camera), `@322d61be`
  (expiry), `@322d6229` (cancel), ctor `@322a2922`; reader `31ff5df0 @31ff64b7` (the C3v disp32 sweep missed it). Ported as
  `CX2GameUnit::m_bHeldByTimerFocus` with its three writes in `UpdateUnitPointer`. `32082810` on user `+0x8ad8` = March's
  `m_InfiniteStrength.Init()` (the kickoff digest's "+0x258" was the callee's own offset). `32291750` = March's `ClearStartOfDelayedFiringData`.
  June constants checked from the exe: 200.0 / 0.3 (doubles `0x3346ef50` / `0x3343bd78`), 0.4 (`0x33446fa0`), 50 / 10 / -1 near clip = March.
- **Corrections to the tsv / checklist text:** the index form gets the whole anim time, not `clamp0( now - +0x21c )` (that value goes to
  `SetLinearTrackingTime`); checklist 7af's "unit pointer's low byte" for the `31fae6d0` / `31faee30` byte: the low byte is overwritten by the
  `LINEARMOVING` read (`@31fb6496`, 0 on a miss), so the byte is the key's value.
- **Stand-ins (one-line comments, routed):** game byte `+0xba5` early return in `PlayLuaCamera` (written only by PvP-side `32ee2e00` /
  `324a0470`); in `DyingStart` the XMAS-2016 fail hook `322e60d0` (game `+0xbd8`) and the PvE die fields -> Stage 19 bullet; the `PCC_GUILD`
  killer lookup by user `+0xd74` (PvP only, parked: no PvP offline); black-out handles `+0x9188` / `+0x9194` / `+0x918c` / `+0x9198` and the
  unit list `+0x3b84` (`32274880`) -> Stage 35 "Found by D1-2b", for the D1-2 review to place (with `NomalDirectCamera`'s uncompared
  height-compensation chain). Checklist 7af: class `SUB_CAMERA` callers pass June's time when their class is ported.
- **June bug kept:** `JUNE_BUGS.md` 469 (`31fae6d0` passes roll 0, not its 6th argument; latent, `TILTANGLE` in no file).
- **Signatures:** every new parameter has March's value as its default, so March's other callers (VioletMage / Elsword `SUB_CAMERA` plays,
  `UpdateUnitPointer`, the `_LUA` wrappers) compile unchanged and behave as before.
- **Reach offline:** `LINEARMOVING` exists only in Ain's files (unported), `TILTANGLE` in none, so the visible changes today are the timed-focus
  skip and `DyingStart`'s killer fields; `GT_PVE` / `PCC_GUILD` are never set offline.
- **Hygiene:** `flagcheck --worktree --files <the 10 files> --with <the other two>` per flag: `LUA_CAMERA` 2 PASS, 1 DEFINE, 7 OTHER; `COMMON_CAMERA_MOVE`
  4 PASS, 6 OTHER; `DYING_START` 3 PASS, 1 DEFINE (`ServerDefine.h` second home), 6 OTHER; each `RESULT: PASS`, 0 FAIL, 0 ERROR, 0 deleted lines,
  0 non-ASCII, 0 EOL mismatches, 0 `#else`/`#endif` without the token. OTHER rows are the sibling flags; `X2Game.cpp`'s three writes sit inside
  `JUNE_TIMER_FOCUS_UNIT`'s June body. `tools/flagcheck.py` `SECOND_HOMES` gained `JUNE_DIVERGE_DYING_START`.
  `enumscan --check` on the four `.cpp`: 0 A/B rows. `cl /c /Zs` (no PCH) of `X2Camera.cpp`, `X2Game.cpp`, `X2GameUnit.cpp`, `X2GUUser.cpp`,
  `Offline/Handlers_Room.cpp`, `X2ServerPacket.cpp` (includes `ClientPacket.cpp`): exit 0, no warning in the new blocks.
- **Build:** `DEPLOY OK: X2_offline.exe is 25,809,920 B, dated 2026-10-29 15:01:50, md5 8eb6307629ddb0d4806c729382e916bc, equal to
  X2/US_SERVICE/x2.exe (2026-10-29 15:01:44); built at main d831b6c3 + 12 uncommitted code file(s).` (the two extra are a peer's quest files.)
  `EXESTR OK: 2 of 2 found in X2_offline.exe` (`TILTANGLE`, `LINEARMOVING`).
- **Smoke (plan end):** a skill with `SUB_CAMERA_TIMING` plays its sub-cameras as before; a dungeon death with the party / NPC killer sends
  the request (log `GAME     unitUID=... died`); a timed focus (boss last kill) is not pulled back by the dying unit's default camera.

### D1-2 review Done (2026-10-29, Opus 5.5; reviews `5e4508ee`, `0c054a37`, `25d16ac1`)

**Verdict: D1-1, D1-2 and D1-2b pass; one partial port fixed under `LUA_CAMERA`.**
- **Bodies vs June** (Ghidra decompile + `funcdis.py` asm, all 21 flags): gage `SetNowMp` / `UpNowMp` / `FlushMp` and the unit wrappers
  (skip / track arguments, before-value, tally signs, random mission fed the argument, `ret 0xc`); `322d7670` filters and list modes
  (`NTPLOTT` 1 / 3 / 4, `NCT` 102 / 106); `32eb9c70` awakening BTIs (157, 320, 660, 661, 822, 823, 966-968; `AEACBO_NOT_APPLY` = 1, single-set
  branch only); `31fceab0`, `3229dd90` (March's array is its whole list, 35 vs June's 36), `31ff7300` placement; NPC `32758fe0` / `327590c0` /
  `32759250` (freeze vector `+0x6564`); `3202cba0` (channels 12-14 -> 24675, 15 -> 39561); `3202daf0` + `3201bd60` / `322f43e0` / `322f42a0` /
  `322f4340` / `327f3b60` (sub-stage -1 / 0 cases match); `31ffcba0` + fade `31fe8220`; `31fdaf30` (21 cases, 0x57 / 0x5d left out);
  `32006c00`; `32cc5b20`; `32cc8980` (enchant `+0x194`, templet `+0x198`); `31fd9200`; `32014810` (every killer branch, PCC 13 fallback,
  white-out, die count, delayed-firing clear); `31ff5df0` (stack slots: index form = anim time `[E+0x20]`, sub forms = now - start,
  `SetLinearTrackingTime` = clamp0( now - before )); `31fb5920` tracking path; `31fae6d0` is `ret 0x18` with roll 0 (JUNE_BUGS 469 holds).
- **Finding, fixed:** D1-2b gave `NomalTrackingCamera` (unit) and `NomalDirectCamera` only June's roll. June's `31fb3640 @31fb3788` and
  `31fb3c50 @31fb3cf6` apply the height compensation for game types 2-5 (constants 400 / 800->100 / 900->200 / 1200->300 read from the exe,
  as March). Both conditions now take June's `GT_DUNGEON || GT_BATTLE_FIELD || GT_PVE || 5` (5 has no Enum name) under `LUA_CAMERA`, March's
  (with its `PVP_ZOOM_CAMERA_1` ladder) in `#else`; new `#error` guard `LUA_CAMERA` needs `JUNE_ENUM_NAMES` (defined at `Always.h:10170`).
  No change offline (types 4 / 5 never set). The rest of `31fb3c50` (lookAt +100, eye math, clamps, `m_bRight`) equals March's.
- **Live branches:** `cl /EP` probe (X2Lib US_SERVICE), 57 markers: all 21 flags and every enclosing flag ON (`X2OPTIMIZE_NPC_NONHOST_SIMULATION`
  holds the NPC `_Reaction_*` pair, `HYPER_MODE_FIX`, `SERV_SKILL_NOTE`, `SUMMON_MONSTER_CARD_SYSTEM`, `SERV_RENA_NIGHT_WATCHER`, ...);
  off: `CHRISTMAS_EVENT_2013`, `SERV_GATE_OF_DARKNESS_SUPPORT_EVENT`, `HEAD_ATTACHED_CAMERA_TEST` (March-only lines June drops anyway).
- **Bookkeeping:** `MODS.md` sections for all three; `JUNE_BUGS.md` 467-469; `guards.tsv` Add row for `RIDING_HYPER_MODE_END`; stand-ins named.
- **Branches placed:** black-out screen + the `+0x3b84` list (a timed per-unit mark list: timer `+4` counted down by `32297740`, effect set
  `+0x41ec` via `322919d0`, fed through `Process_ExtraDamage`) -> was phase D1-2c; moved out of this plan (user, 2026-10-29, drift) to backlog **Stage 36**
  (`kickoffs/june-backlog/stage36-extra-damage.md`). Camera chain -> fixed above; March's `ManualTrackingCamera` / `CalcNormalCameraPosition` carry the same 2-type chain,
  June counterparts not in the tsv -> lead in `D1-review.md`. Backlog Stage 35 bullet updated.
- **Hygiene:** `flagcheck <FLAG> --range <c>^..<c>` per flag: all 21 `RESULT: PASS`, 0 FAIL, 0 deleted, 0 non-ASCII, 0 EOL mismatches, 0 encoding
  changes. Shared-file flags rerun `--with` their same-commit siblings: SKILL_MEMO / SKILL_LEVEL_VALUE / FORCE_HYPER_MODE 1 PASS, 13 OTHER each;
  BONE_NAME_PREFIX / DIE_FRAME_MOVE / SUMMON_SPECIAL_ATTACK / RIDING_HYPER_MODE_END 1 PASS, 9 OTHER; PVP_OFFICIAL_SKILL 2 PASS, 1 DEFINE (two homes);
  LUA_CAMERA's one ERROR (the `COMMON_CAMERA_MOVE` guard naming it) clears `--with` (2 PASS, 8 OTHER). OTHER rows are the sibling flags; the `tok`
  counts (D1-1: GAGE_TALLY 5, LOCK_ON 3, ...) are the plain house-style `#else` lines. `git log -S` after each commit: only `25d16ac1` for
  PVP_OFFICIAL_SKILL (shared `ClientPacket.h` / `ServerDefine.h`). This review's edit: `LUA_CAMERA --worktree --with COMMON_CAMERA_MOVE DYING_START`
  2 PASS, 0 FAIL, 0 ERROR, 0 deleted; `enumscan --check X2Camera.cpp` 0 A/B rows.
- **Build:** `DEPLOY OK: X2_offline.exe is 25,809,920 B, dated 2026-10-29 15:31:22, md5 00ce3d231498557f7caf99d45c476bbb, equal to
  X2/US_SERVICE/x2.exe (2026-10-29 15:31:15); built at main 2c4f7e85 + 2 uncommitted code file(s).` (PCH rebuilt for the `Always.h` edit.)
- **Smoke (plan end):** as D1-1 / D1-2 / D1-2b; the camera fix has no offline effect.

### D1-3 Done (2026-10-29, Opus 5.5; eight `SERV_IRUHADEV_JUNE_DIVERGE_*` flags, March's body in every `#else`)

| tsv | March function | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 261, 262 | `Weapon::AppendEnchantParticleForSwordToDeviceList` / `InitEnchantParticleForSword` | `3226ecc0` / `3228cb30` | `ENCHANT_PARTICLE` | 30 more enchant types (triple, double-double: the Property01 sets; quadra masters 4004 / 4044 / 4404: `Property04_M_*`, present in `GameMajorParticle44.txt`) |
| 266 | `CX2Inventory::GetNumItemByTID` | `32869b80` | `ITEM_COUNT_BY_TID` | shared-bank items count; the bank flag also skips June's `ST_COMMON_BANK` (registered = 16); args 4-5 ( damage-range items, time-locked items ), defaults 1 / 0 as 99 of June's 114 call sites |
| 267 | `GetStateID` (call sites) | `3275aaf0` | `STATE_ID_BY_KEYWORD` | the dungeon input lock (C3s) and the boss intro (C3w) match `FORCE_CINEMATIC_STATE` by keyword |
| 268 | `CX2RidingPet::GetUserInfo` + 4 readers | `32d2d7e0`; `32062903`, `3206532c`, `323f2847`, `323f29e3` | `SADDLE_POS` | saddle = bone - rider position; `CommonFrameMoveFuture` / `CommonFrameMove` / the square unit and its viewer add the position back |
| 269 | riding socket sum (call site) | `32365d30` @`323664e6` | `RIDING_SOCKET_TEMPLET` | templet looked up by the pet UID's low dword (JUNE_BUGS 470) |
| 270 | `CX2RidingPetManager::GetRidingOnState` | `321fa590` | `RIDING_ON_STATE` | a `DCC_NOT_SUMMON_RIDING_PET` dungeon (registered = 8, `IsDungeonCheckCondition`) refuses |
| 274 | `CX2UserSkillTree::ConstructSkillTreeNotMyUnit` | `328926f0` | `SKILL_TREE_NOT_MY_UNIT` | no `Reset`: `AddAcquiredSkills` of the in-tree skills, the out-of-tree ones a learned skill links (`SA_ADD_LEARN_SKILL_LIST_1-3`, new `AppendLinkedLearnSkill` = `32892590`), equipped A / B |
| 275 | `CX2UserSkillTree::SetSkillCoolTimeLeft` | `32893880` | `SKILL_COOL_TIME_LEFT` | no relationship / hyper-active override; matches the upgrade-swapped id (3rd arg, default true as 45 of 53 June sites); bar B when key 0 or 1 is usable |
| (twin) | `CX2GUUser::GetSkillLevel` -> `GetSkillLevelValue` | `31fceab0` | D1-1's `SKILL_LEVEL_VALUE` | one function: `int GetSkillLevelValue( const int ) const` holds June's body; 54 calls renamed (VioletMage 26, Eve 24, user 4) |

- **Corrections to the tsv** (rows annotated): 266's `0x10` is June's `ST_COMMON_BANK` (June `Enum.lua` `SORT_TYPE`), not the shared bank, and
  `32882230` is true while item `+0x50` (a 64-bit time from `KItemInfo +0x97`) is still ahead, not "expired"; 267's `3275aaf0` is June's
  `GetStateIDByKeyword` (June's monster-state cheat `32748270` calls it where `X2GameEdit.cpp:495` does), so March's exact `GetStateID` stays
  and only the two call sites change; 250's call goes to `g_pData` (`0x33775860`), not `g_pX2Game`; 252's function equals March's (the
  weapon loop is March's `ADD_UPGRADE_WEAPON_PARTICLE`), the divergence is the +11 / +13 particle family it calls.
- **Method:** 261 / 262 were emulated (unicorn, every value 0-6999): both functions give 66 values, the same names per value. Call-site
  defaults come from the pushes before every June call (`xrefs_to`).
- **Stand-ins (one-line comments):** item `+0x50` time (March's `ItemData` lacks it: `bIncludeTimeLockedItem` skips nothing; Stage 16 bullet);
  square-unit byte `+0x794` cleared on load (`@32d2d8bc`, unidentified); June's guild-agit (15) / photo-zone (26) states in
  `GetRidingOnState` (not in March, as D1-4's play guide); its boss-raid field `32481fa0` and dungeon-id list `32932230` (ids not in this build);
  March's page vector for June's single acquired map in 274. The rate step in 275 stays under `SERV_BALANCE_FINALITY_SKILL_EVENT` as in
  March's body (undefined here; June runs `3234cc10` always -> D1-3b).
- **June bugs kept:** `JUNE_BUGS.md` 470 (riding socket by UID), 471 (bar B's rate step writes the bar-A gauge slot, `@32893dd8`; March has it too).
- **Split to D1-3b** (`kickoffs/june-march-divergence/D1-3b.md`): 252 (the upgrade-weapon particle family, 10 handles), 250 (`31ee1f40`,
  about 970 decompiled lines of per-class bone data), 251 (about 2,200 lines), 205, 263, and `3234cc10`.
- **Leads for the D1-4 review (kickoff):** the Bow enchant pair was not compared; `CX2OfflineInventory`'s server-side count keeps March's
  exclusions (the emulator models the server).
- **No guard:** no unported class can crash on these bodies (the riding and skill-tree paths read only common state).
- **Bookkeeping:** tsv rows 250 / 252 / 266 / 267 annotated; funcmap rows `(Diverge D1-3)` x7; `MODS.md` D1-3 entry; Stage 16 bullet.
- **Hygiene:** `flagcheck --worktree --files` per flag: `RESULT: PASS` for all eight new flags (0 FAIL, 0 ERROR, 0 deleted lines, 0 non-ASCII,
  0 EOL mismatches, 0 `#else`/`#endif` without the token; `Always.h` OTHER = the sibling defines). `SADDLE_POS` and `SKILL_LEVEL_VALUE`
  pass with `--with` each other and `JUNE_USER_SKILL_LEVEL_HELPERS` once `Always.h` is left out of `--files`; with it, one ERROR row is the new
  compound HELPERS guard flagcheck cannot evaluate (as D1-2 review's `LUA_CAMERA`). The fold's deleted lines (old twin definition 13, its
  declaration 1, and the 54 renamed calls) all sit inside June blocks (`CHAR_ARME_*`, `CHAR_EVE_*`, `EFFECT_SET_LIST_VARIANTS`, HELPERS).
  `cl /c /Zs` (no PCH) of the 12 changed `.cpp`: exit 0, no warning in the new lines; it caught one guard of mine that could never pass
  (`ENCHANT_PARTICLE` on `X2OPTIMIZE_GAME_CHARACTER_BACKGROUND_LOAD`, defined in `KTDX.h:258` after `Always.h`), removed before the build.
  The first build then stopped in KTDXLIB on the HELPERS guard's `SERV_ADD_LUNATIC_PSYKER` half (not visible to KTDXLIB): narrowed to
  `SKILL_LEVEL_VALUE` (both defined earlier in `Always.h`), rebuilt.
  `enumscan --check` on the 17 files: 29 A/B rows, all old lines in `X2Unit.cpp` / `X2GUUser.h`, none in this phase's hunks.
- **Build:** `DEPLOY OK: X2_offline.exe is 25,811,968 B, dated 2026-10-29 17:34:24, md5 3229774dc762c74205a1954f4059f099, equal to
  X2/US_SERVICE/x2.exe (2026-10-29 17:34:17); built at main 0d2f5c99 + 17 uncommitted code file(s).` (all 17 are this phase's.)
  `EXESTR OK: 2 of 2 found in X2_offline.exe` (`Weapon_Elsword_Property04_M_Fire`, `_M_Poison2`, wide).
- **Smoke (plan end):** Elsword with a triple-attribute weapon shows its enchant glow; quest items in the shared bank count toward a
  collect quest; riding a pet in a village and a dungeon, the rider sits on the saddle (not at the map origin); Aisha's cool times after
  a skill use; another player's (or a party bot's) passives apply in a dungeon.

### D1-3b Done (2026-10-29, Opus 5.5; two `SERV_IRUHADEV_JUNE_DIVERGE_*` flags, March's body in every `#else`)

| tsv | March function | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 252 | `CX2GameUnit::Weapon::Init` / `Create` / `Process` / `DeleteUpgradeWeaponParticle`, new `SetUpgradeWeaponParticleEmitRate`, the call in `CommonFrameMoveForUser`, new `CX2GUUser::CanShowWeaponEffect` | `32299730` / `32291e00` / `32292610` / `32293620` / `32293870`, `32297d50` @`3229843a`, `31fcf160` | `UPGRADE_WEAPON_PARTICLE` | ten sequences (`m_hSeqUpgradeWeapon5..10`); a +13 weapon adds `13Gang_Weapon_Effect_Spark01` / `_Spark_Particle` / `_Spark_Particle_1` (seqs 5 / 6 / 9 on the main weapon, 7 / 8 / 10 on Eve's and Add's second); the hide test left `Process`: a user that is hidden, riding or playing an avatar emotion gets emit rate 0 on all ten |
| (callee) | `CX2Unit::GetSkillCoolTimeDecreaseRate`; `SetSkillCoolTimeLeft`'s rate step | `3234cc10` | `SKILL_COOL_TIME_RATE` | the step runs always (it sat under the undefined `SERV_BALANCE_FINALITY_SKILL_EVENT`); the hyper-active item (60008065) cuts only in a dungeon game, as the 163-id list's item (60008059) already did |

- **Method:** `32292610` read from its disassembly (stack slots -> handles; the decompile swaps locals); constants 0.5 (`3342ebc8`) and 10.0
  (`3344a51c`) as March's. Frame offsets checked in June's weapon init `3228d660`: `+0x348` / `+0x34c` = `ATTACK_LINE_END0` / `_START0_FRONT`.
  `3234cc10` emulated (unicorn, ids 0-11,999, types 0 and 13): the same 163 ids as March's switch, rate 0.7 (`3344c858`).
- **Inert in this build (found at the deploy check):** `ADD_UPGRADE_WEAPON_PARTICLE` is defined at `Always.h:2013` and `#undef`'d by
  `OnlyGlobal/Always_Global.h:30` (included at `Always.h:3393`), so March's +11 glow and this port are compiled out of US_SERVICE
  (`exestr`: `11Gang_Weapon_Effect_Lightning03` and the six June names absent). D1-3 Done's "March runs the weapon loop" is wrong for this
  build. The 252 body is a transcription not yet compiled; **D1-3c task 1** compares the 77 `ADD_UPGRADE_WEAPON_PARTICLE` sites (8 files)
  with June, re-defines it after the global `#undef` under this flag, and proves it builds. `CanShowWeaponEffect` compiles (outside the block).
- **Assets:** the six June sequence names are in March's `GameMajorParticle44.txt` already (no staging).
- **Stand-ins (one-line comments):** June's usable test `328825c0` / level `32882540` -> March's endurance / infinity test and
  `m_EnchantLevel` (as funcmap S7k); in `CanShowWeaponEffect` June's custom-sit emotion test `323630f0` (CX2CustomizeManager, Stage 12) and
  the byte pair `+0x8f6e` / `+0x8f6f` (unidentified) are skipped, and June's data-driven `IsAvatarEmotion` is March's static one.
- **Not reached here:** June's cases 10-12 (Lu/Ciel, Rose, Ain) in Init and Process: checklist **7as**.
- **June bugs kept:** `JUNE_BUGS.md` 472 (handle 4 built from name 3, March too). 471 (bar B's rate step writes bar A's gauge) is now reached.
- **Guard widening:** `X2Define.h:380`, `X2Unit.h:1247`, `X2Unit.cpp:5787` read `#if defined( SERV_BALANCE_FINALITY_SKILL_EVENT ) ||
  defined( SERV_IRUHADEV_JUNE_DIVERGE_SKILL_COOL_TIME_RATE )` (flag off = March's line). This also compiles `IsInfinityElEssence` and the
  earlier-ported `GetSkillMpDecreaseRate`, whose call sites stay under the event flag (D1-3c task 5).
- **Leads -> D1-3c:** June's eight enchant frame moves (`3228bf60`..`3228c7d0`) and `31ff8510` call `31fcf160` where March calls
  `CanNotShowEnchantedWeaponEffectToRiding`; June calls the MP rate `3234d840` from six class sites.
- **Split:** 250 / 205 / 263 -> D1-3c, 251 -> D1-3d (kickoffs written; tsv rows annotated).
- **Bookkeeping:** tsv 252 annotated; funcmap rows `(Diverge D1-3b)` x8; `MODS.md` D1-3b entry; checklist 7as; JUNE_BUGS 471 / 472.
- **Hygiene:** `flagcheck --worktree --files`: `UPGRADE_WEAPON_PARTICLE` RESULT PASS (4 files, 0 deleted, 0 non-ASCII, 0 EOL mismatch);
  `SKILL_COOL_TIME_LEFT` (`X2UserSkillTree.cpp`) PASS, its 5 deleted lines are D1-3's event-flag lines inside its June block;
  `SKILL_COOL_TIME_RATE` 3 ERROR rows = the three widened `#if defined( A ) || defined( B )` lines (its 3 deleted lines), flag off = March's.
- **Build:** `DEPLOY OK: X2_offline.exe is 25,811,968 B, dated 2026-10-29 18:22:57, md5 b0f4a9e260bff1bb17af0a6f4f029b6b, equal to
  X2/US_SERVICE/x2.exe (2026-10-29 18:22:50); built at main 9bd17f2e + 9 uncommitted code file(s).` (all 9 are this phase's.)
  `EXESTR FAILED: 2 of 2 not found` (`13Gang_Weapon_Effect_Spark_Particle_1`, `_Spark01`, wide): the block is compiled out, see above.
- **Smoke (plan end):** (after D1-3c) Elsword with a +11 sword shows the lightning glow, a +13 one adds the sparks, none while riding or
  in an avatar emotion; a hyper-active skill's cool time with item 60008065 is cut only in a dungeon.

### D1-3c Done (2026-10-29, Opus 5.5; two `SERV_IRUHADEV_JUNE_DIVERGE_*` flags, March's body in every `#else`)

| Item | March | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| task 0 | `ADD_UPGRADE_WEAPON_PARTICLE` (77 sites, `#undef`'d by `OnlyGlobal/Always_Global.h:30`) | has the glow | `UPGRADE_WEAPON_PARTICLE` | re-defined at the end of `Always.h` under the flag: March's +11 sites and D1-3b's weapon port compile |
| task 0 | `CX2UnitViewerUI` Init / Create / Process / Delete, ctor, `OnFrameRender_Draw`, `SetShowAttribEnchant` | `31f92910` / `31f914c0` / `31f9a900` / `31f90d30`, `31fab020`, `31fa00b0`, `31f91f90` | `UPGRADE_WEAPON_PARTICLE` | the viewer runs ten sequences with the +13 sparks, as the weapon does; new `CanShowWeaponEffect` (`31f91230`) hides them |
| task 0 | `CX2UnitViewerUI::CanNotShowEnchantedWeaponEffectToRiding` | `31f90e20` | `UPGRADE_WEAPON_PARTICLE` | the viewed unit's own riding pet (by UID) instead of the local player's; its six viewer enchant callers follow June |
| task 4 | `Weapon::EnchantParticleFrameMoveFor` Eve / Bow / Sword / Ara / Add | `3228bf60` / `3228c050` / `3228c140` / `3228c360` / `3228c450` | `WEAPON_EFFECT_SHOW` | hide test = the owner's `CX2GUUser::CanShowWeaponEffect` (adds the owner's show flag and avatar emotion); bodies otherwise equal |

- **Sites compared and equal (no edit):** `CX2GUUser` `ReInit`, `InitEffect` (`32012760`), `UpdateWeaponEnchantEffectAndExtraDamageType` (`3201bce0`), the
  preload `AppendUpgradeWeaponParticleToDeviceList` (`31fcdb50`, no 13Gang preload in June either) with `CalcWeaponEnchantLevel` (inline in `31fef4d0`),
  the weapon dtor and Ara frame lookup (`3228d660`), `X2TFieldGame` equip notice (`323bc8d0`), viewer `SetUnit` / `UpdateEqip` / dtor / Ara frames in
  `InitWeaponTrace` (`31f9a2a0`). The four `11Gang_Protect.tga` sites in `X2Eqip` sit in `JUNE_EQIP_SKIN`'s `#else` (June's chain is already live).
- **Kept March US:** `CX2Eqip::SetRenderParam`'s UV speed (0.75 at +11 with the studio flag) stays 1.0 under the flag: June rewrote the function
  (`32963c80`, owned by june-engine R6b), and its helper `3295b970` has no per-level speed.
- **Method:** `31f9a900` read from its disassembly (handles 1-10 = EDI, EBX, `[esp+1c]`, `[esp+10]`, EBP, `[14]`, `[18]`, `[28]`, `[24]`, `[20]`);
  `+0xde8` / `+0xdc8` = `TRACE_END[0]` / `TRACE_START[0]`, `+0xecc` / `+0xed0` = `ATTACK_LINE_END0` / `_START0_FRONT`; constants 2.0 (`33437820`),
  0.5 (`3342ebc8`), 10.0 (`3344a51c`) read from the PE. Viewer states 1 / 3 / 6 / 7 = March's members; 9 and 10 are June-only.
- **Stand-ins (one-line comments):** in the viewer's `CanShowWeaponEffect`, June's custom-motion habit test `31f91110` (Stage 12) and the per-emotion
  bytes `+0x1174..+0x1176`; June's usable test `328825c0` is March's endurance / infinity test (as D1-3b). June quirk kept: the Ara spark group also
  needs sequences 7 / 8.
- **Not reached here:** viewer cases 10-12 and the Lu/Ciel trace frames: checklist **7as** (extended). `31f9f8d0` (June-only equip-list rebuild that
  re-inits the glow, UI callers `32195e40` / `321a29a0`) and `31f97960` (vslot 0x10 draw of sequences 1-4, R4a routed) are not in March.
- **June bugs kept:** `JUNE_BUGS.md` 473 (viewer handle 4 from name 3, March too).
- **Split:** `31ff8510` `ProcessEnchantedWeaponEffectAtHand`, the nine viewer enchant frame moves that call `31f91230`, and 263 -> **D1-3e**;
  250 / 205 / the MP rate sites (tasks 1, 2, 5) -> **D1-3f** (kickoffs written; tsv rows annotated). The MP rate sites left D1-3f for Stage 33 (user, 2026-10-29, drift).
- **Bookkeeping:** funcmap rows `(Diverge D1-3c)` x15; tsv 205 / 250 / 252 / 263 annotated; `MODS.md` D1-3c entry; checklist 7as; JUNE_BUGS 473;
  `D1-4-review.md` covers D1-3e / D1-3f and lists three review points (D1-3f no longer has the MP rate sites).
- **Order proof:** `cl /EP` with the US_SERVICE defines on a probe including `KTDX.h`, then one including `X2Lib/StdAfx.h`: `ADD_UPGRADE_WEAPON_PARTICLE`,
  both flags DEFINED (no later `#undef`).
- **Hygiene:** `flagcheck --worktree --files`: `WEAPON_EFFECT_SHOW` RESULT PASS (`X2GameUnit.cpp` +20, 0 deleted, 0 non-ASCII, 0 EOL mismatch);
  `UPGRADE_WEAPON_PARTICLE` 3 PASS (`X2Eqip.cpp` +4, `X2UnitViewerUI.cpp` +321, `X2UnitViewerUI.h` +16, 0 deleted) and 1 ERROR = `Always.h:10793`,
  the `#error` dependency guard `#if defined( ..._WEAPON_EFFECT_SHOW ) && !defined( ..._UPGRADE_WEAPON_PARTICLE )` (not this flag's code).
- **Build:** `DEPLOY OK: X2_offline.exe is 25,826,304 B, dated 2026-10-29 19:13:26, md5 5b1ecaaca973d79954752f539edf98be, equal to
  X2/US_SERVICE/x2.exe (2026-10-29 19:13:19); built at main 107270e7 + 5 uncommitted code file(s).` (all five are this phase's.)
  `EXESTR OK: 3 of 3 found in X2_offline.exe, 25826304 B, dated 2026-10-29 19:13:26, md5 5b1ecaaca973d79954752f539edf98be`
  (`13Gang_Weapon_Effect_Spark_Particle_1`, `_Spark01`, `11Gang_Weapon_Effect_Lightning03`, wide).
- **Smoke (plan end):** Elsword with a +11 sword shows the lightning glow in a dungeon and in the character window (U), a +13 one adds the
  sparks, none while riding or during an avatar emotion; Eve's / Add's glow sits on both weapons.

### D1-3d Done (2026-10-29, Opus 5.5; two `SERV_IRUHADEV_JUNE_DIVERGE_*` flags, March's body in the `#else`)

| tsv | March function | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 251 | `CX2GUUser::UpdateNotRenderingOtherEquipList` (`X2GUUser.cpp:39971`) | `3201c3a0` | `NOT_RENDERING_EQUIP` | June's whole body: the one-piece (`EP_ONEPIECE_FASHION`), face-1 / face-2 and arm accessories hide equips by division group instead of March's `IsOnePieceAvatar*` / `IsNoRender*` id lists; a re-show pass at the end; new member `m_bSDOnePieceAvatar` (June `+0x8fc0`) |
| (loader) | `CX2ItemManager::AddDivisionItemTemplet_LUA`, new `ParseDivisionItemGroup`, `AddCoboDivisionItemTemplet_LUA`; `X2Data.cpp` load; `X2Main.cpp` binding | `32814170`, `3280b550` | `DIVISION_ITEM_GROUP` | every division block outside types 55 / 85 / 89-93 fills its group (`m_eItemID` + `m_eItemID_INT`, plus each listed id's Cobo clone); first block per type wins |

- **Why the loader:** the groups the body tests were empty in March (only CEPE 81 was parsed, C9f-2). June's dispatcher (`32818480`, byte table `32818748`,
  checked) sends every type but 55 / 85 / 89-93 through `32814170`; the Cobo map (`+0x220`) is filled only by `3280b550`, bound by `31d95080 @31db3089`
  (name from `CoboRenderDivisionItemTemplet.lua`: 750 two-int calls; the binder's name bytes are XOR'd). All 28 types the body tests sit once each in
  `RenderDivisionItemTemplet.lua`, which March already loads. `DivisionItemTemplet.lua` and the other parsers stay backlog Stage 11 (bullet added).
- **Body, from the decompile plus disassembly:** 26 group tests, case lists copied per block (the blocks differ: some skip the shown test, some set no
  `bOnePieceAvatarCheck` on the hair case, `..._ONLY_RENDERING_HAIR_AND_FACE` null-checks the equip). Helpers: `31fd5ab0` = `GetEquipPtrFromEquipViewListByEquipPosition`,
  the inline `+0x172` / vslot `0x28` idiom = `SetShowObject( false )`, `31cfe4a0` = `SetShowObject( bShow )`, `+0x9170` = `m_bHideNoRenderable`,
  `+0x9340` = `m_pHairXSkinMesh`, `+0x5a4` / `320708b0` = `GetNormalModel(0)`, `+0x6754` = `m_pAfterImage`. Unit types 12 / 2 = `UT_AIN` / `UT_ARME`
  (Aisha and Ain hide equips that are already hidden); the state hook = `IsNearPortalLineMap` of the dungeon (`XS_DUNGEON_GAME` 0xb, `327ca710`) or
  field (`XS_BATTLE_FIELD` 0x15, `32d53810`) game. June's one mask byte `[esp+0x24]` (5 / |2 / |8, tested `& 0xd`) is two bools here; bit 2 is never read.
  The `NOT_RENDERING_HAIR` (33) test in the `ONE_PIECE_AVATAR` loop discards its result and is left out.
- **Enum names (`JUNE_ENUM_NAMES`):** 35 DITT members in `CX2ItemManager::DIVISION_ITEM_TEMPLET_TYPE`, `CX2Unit::UT_AIN = 12` after `UT_END`. June EP 27 / 28
  (`EP_AC_EARRING` / `EP_AC_RING2`) share March's values, so March's names `EP_AC_TEMP3` / `EP_AC_TEMP4` with a comment; the kim remap still puts
  earrings on 18 (`JUNE_KIM_MINIME_REMAP`, Stage 16), so June's earring cases do not fire yet.
- **Deviation (one):** June calls the game's `IsNearPortalLineMap` without a NULL test; the port tests `g_pX2Game` (March may build users first).
- **Stand-in:** `SetpHairSkinMesh` is March's plain setter; June's `3322a5d0` refs the mesh -> june-engine **R12c** (kickoff, STATUS row).
- **Routed:** the `+0x8fc0` reader `31fdc070` (head bone `Bip01_Spine1` for SD avatars; Ain `32a51670`) -> backlog Stage 11 bullet.
- **Reach points:** every user's equip hiding (Elsword ... Add; Ara's override still calls this base). Groups now live for R14's `_UpdateSkin`
  (6 / 0x31 / 0x36 / 7, every model attaches) and R14b's jiggle (30); these were June ports waiting on Stage 11.
- **Data:** `CoboRenderDivisionItemTemplet.lua` adopted from June data036 (`luasrc.py adopt-june`, bytecode, new); `luabind.py check`: 1 method, 0 unbound,
  PASS; `PACK OK data036` (vs live: +1 added). `COPY OK data036` (user, 2026-10-29): 268 entries, md5 040d4e30772f84d40f8f577e87a1a7a7.
- **June bugs kept:** `JUNE_BUGS.md` 474 (the SET2 pass reuses the SET1 iterator), 475 (UID low dword only).
- **Bookkeeping:** tsv 251 annotated; funcmap `3201c3a0` updated + 5 rows `(Diverge D1-3d)`; `MODS.md` D1-3d entry; backlog Stage 11 bullet; STATUS R12c.
- **Hygiene:** `flagcheck --worktree --files`: `NOT_RENDERING_EQUIP` RESULT PASS (`X2GUUser.cpp` +978, `X2GUUser.h` +3; 0 deleted, 0 non-ASCII, 0 EOL mismatch); `JUNE_ENUM_NAMES` (`X2Unit.h` +3, `X2ItemManager.h` +44 OTHER) PASS; `DIVISION_ITEM_GROUP` `X2Data.cpp` +4 / `X2ItemManager.cpp` +60 / `X2Main.cpp` +3 PASS,
  1 ERROR = the `#if defined( ..._NOT_RENDERING_EQUIP ) && !defined( ..._DIVISION_ITEM_GROUP )` dependency line in `Always.h` (as D1-3b's), so the tool prints RESULT FAIL; 0 deleted lines.
- **Build:** `DEPLOY OK: X2_offline.exe is 25,833,984 B, dated 2026-10-29 23:38:28, md5 e4082e3046b58249a4603358ea98667c, equal to X2/US_SERVICE/x2.exe
  (2026-10-29 23:38:21); built at main b4683fee + 8 uncommitted code file(s).` (all 8 this phase's.) `EXESTR OK: 2 of 2 found` (`AddCoboDivisionItemTemplet_LUA`,
  `m_eItemID_INT`); `EXESTR OK: 1 of 1 found` (`CoboRenderDivisionItemTemplet.lua`, wide).
- **Smoke (plan end):** Aisha with `/additem 180060` (Luto's Illusion, one-piece) equipped: hair, top, bottom, gloves and shoes are hidden and the one-piece
  shows; Eve with `/additem 180063` the same; unequipping brings them back. Optional: a face accessory in group 33 (`NOT_RENDERING_HAIR`) hides the hair.

### D1-3e Done (2026-10-29, Opus 5.5; one site under D1-3c's `WEAPON_EFFECT_SHOW`; viewer family -> D1-3g, 263 -> june-engine R6c)

| Item | March | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| task 1 | `CX2GUUser::ProcessEnchantedWeaponEffectAtHand` (`X2GUUser.cpp:30409`) | `31ff8510` | `WEAPON_EFFECT_SHOW` | the hand sparks hide with `CX2GUUser::CanShowWeaponEffect` (hidden, riding, avatar emotion) instead of the riding manager alone |

- **Task 1, compared whole:** types 1-9 are March's (bones `Dummy1_Rhand` 1 / 2 / 4 / 6, `Dummy2_Lhand` 3 / 5, `Dummy3_Weapon` 7 / 8, `Weapon02` 9; rates
  10 or 15 / 30 / 60 / 25, Eve 10 / 10 / 10 / 25, read with `charport_consts.py`); the hidden path is March's (`ClearAllParticle`, emit 0). Only the test
  differs, so the site swaps the test as D1-3c's frame moves did. Stack slots from the disassembly: sequence 1 = `[esp+0x2c]`, rates `[1c]..[28]`
  (the decompile's `local_18` / `iStack_14` overlap). Types 10-12 (Lu/Ciel by byte `+0x8e90` on the second skin `+0x8e88`, Rose, Ain): checklist **7as**.
- **Task 2, not ported, split to D1-3g:** June's viewer moved its seven attribute-enchant handles into two vectors, the fire list `+0xd90` and the hand
  list `+0xda0`. All nine frame moves read the list (D1-3c's "Add's `+0xd90` list" was the whole viewer's), so their bodies cannot be ported without
  the container, and the container changes about eighteen functions: variadic creators `31f9d1f0` / `31f9d3b0`, the three EDT switches `31f9ea20` /
  `31f9eeb0` / `31f9d570`, delete `31f9da10`, the at-hand `31f961a0` (per-type rate vector; Chung on `Bip01_R_Hand`), ctor / dtor, frame move, render,
  `SetShowAttribEnchant`. Every one has a March twin, but none is a `replace` row, so it left this plan (user, 2026-10-29): june-charinfo C4 add-on, kickoff
  `kickoffs/june-charinfo/C4-enchant.md` (the March / June table there; 16 funcmap rows `(Diverge D1-3e)`). June-only frame moves `31f93050` / `31f93200` / `31f940b0`: checklist 7as.
- **Task 3, routed to june-engine R6c:** 263 is not a `Weapon` function in June. `3295cd30` = `ADD ECX,0x8f0; JMP 3295bf90` on an equip-side list of
  entries (show byte `+0x34`, handle `+0x2c`, system byte `+0x1c`), with writers `3295d960` / `3295ddf0` / `32961c80` in the `CX2Eqip` range R6b owns.
  A June-only container on another class: routed, not half-ported (kickoff `kickoffs/june-engine/R6c.md`, STATUS row; tsv 263 annotated).
- **Bookkeeping:** funcmap `31ff8510` row rewritten + 15 new `(Diverge D1-3e)` rows; tsv 263; checklist 7as; `MODS.md` D1-3e entry. No JUNE_BUGS row
  (no June quirk at this site). `D1-4-review.md` no longer lists D1-3g (moved to june-charinfo C4).
- **Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_DIVERGE_WEAPON_EFFECT_SHOW --worktree --files X2Lib/X2GUUser.cpp`: RESULT PASS (`X2GUUser.cpp` +4, 0 deleted,
  0 non-ASCII, 0 EOL mismatch).
- **Build:** `DEPLOY OK: X2_offline.exe is 25,826,304 B, dated 2026-10-29 22:47:29, md5 b28778a6fa5a8c8c4982417014148748, equal to X2/US_SERVICE/x2.exe
  (2026-10-29 22:47:22); built at main bdac0eb3 + 1 uncommitted code file(s).` (`X2Lib/US_SERVICE/X2GUUser.obj` newer than the edit; no new string,
  so no `exestr` line.)
- **Smoke (plan end):** an Elsword with a wind / light / curse weapon shows the hand sparks in a dungeon, and none while riding or during an avatar emotion.

### D1-3f Done (2026-10-30, Opus 5.5; `SERV_IRUHADEV_JUNE_DIVERGE_UNIT_REFORM`; 205 -> D1-3h)

| Item | March | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 250 | `CX2GUUser::SetUnitReForm` (`X2GUUser.cpp:36885`), `CX2UnitViewerUI::SetUnitReForm` (`X2UnitViewerUI.cpp:6799`) | `31fda0d0`, `31f99b20` -> `31ee1f40` | `UNIT_REFORM` | the per-class bone moves / scales come from June's `DefaultBoneInfo.lua`, not March's hard-coded switch |

- **The table is data in June.** `31ee1f40` (on `g_pData`) reads 14 values from the object at `g_pData +0x474` (`3214d170`), a map filled by
  `g_pViewerSkinMesh:AddBoneInfo{ UNIT_ID, BONE_MODIFY1.. }` (`3214cc60`) from `DefaultBoneInfo.lua` (`3214c5f0`, loaded at start-up `3259afd3`, just
  before `ResetEnchantItem`). It is the X2Viewer tool's `CX2ViewerSkinMesh` loader (March `X2Viewer/X2ViewerSkinMesh.cpp:1341-1415`) moved into the
  client with a map. Ported as `CX2DefaultBoneInfo` (`X2Data.h` / `.cpp`; not `CX2ViewerSkinMesh`, which X2Viewer defines and which includes X2Lib),
  member `CX2Data::m_DefaultBoneInfo`, registered in `CX2Main::RegisterLuabind`, loaded in `X2StateStartUp.cpp`; its `CHARACTER_ID` /
  `MODIFY_BONE_STYLE` enums live in the class (X2Viewer's `dxstdafx.h` has March-era global ones).
- **`CX2Data::SetUnitReForm( skin anim, unit type, bSecondForm, bone name )` = `31ee1f40`, read from the disassembly:** type -> CHARACTER_ID
  (Aisha 1 / DW 2 by flag, Lire `CI_LENA`, Elesis `CI_ELISIS`, Lu/Ciel 10 / 11 by flag, Rose 12, Ain 13); defaults move 0 / scale 1; a bone-name
  filter chain whose every branch falls into the full pass (JUNE_BUGS 477, `Bip01_Pelvis` scales the right foot: 476); full pass = `Bip01` and
  `Bip01_Footsteps` `SetFrameMoveBoneY( y )`, 13 `SetFrameScale( bApplyChild true )`; `Dummy2_Lhand` takes the `Dummy1_Lhand` row, `Bip01_Pelvis`
  gets `( 1, 1, 1 )` (never looked up). `31edfa60` = `_CONST_X2DATA_D13F_::SetFrameScaleByBoneName`. AddBoneInfo quirk: JUNE_BUGS 478.
- **Callers:** user `31fda0d0`: Aisha passes `IsTransformed()` (`+0x8c0c`), others false; the viewer `31f99b20` never passes the transform flag.
  Lu/Ciel passes the second body (`+0x8e88`, viewer `+0xd7c`): **stand-in**, returns early at both sites (2 `guards.tsv` rows). `UT_LUCIEL` (10)
  and `UT_ROSE` (11) registered under `JUNE_ENUM_NAMES` (June's `UNIT_TYPE`; 10 shares March's `UT_END` value).
- **Behaviour change (June removed it):** March's body raised `m_fBasicBoneScale` (transformed Aisha 1.1, Chung 1.08). June reads `+0x3e38` only in
  `CX2GUUser::OnFrameMove` and writes it only in the `CX2GameUnit` ctor (dispsweep), so it stays 1.0. Chung's and the transformed DW's whole-body
  size now follow June. March's `reform.txt` dev read (`#ifndef _SERVICE_`) is gone with March's body.
- **Data:** `luasrc.py adopt-june data036 DefaultBoneInfo.lua` (June bytecode, 8,189 B, new); core-list `Enum.lua` + June's `CHARACTER_ID` and
  `MODIFY_BONE_STYLE` tables (337 B appended, `luasrc.py refresh`; `ledger --check` / `core --check` PASS). `luabind.py check DefaultBoneInfo`:
  RESULT PASS (`AddBoneInfo` bound). `kom.py pack data036`: PACK OK (269 entries, + `DefaultBoneInfo.lua`, ~ `Enum.lua`); `kom.py copy data036`: COPY OK (14,015,398 B, 269 entries, md5 d994becdfaa918b922de2eac46b691b9; user, 2026-10-30).
  Missing file: start-up logs `UNIT_REFORM DefaultBoneInfo.lua NOT LOADED` (ErrorLog, XEM_ERROR142) and every unit gets identity bones.
- **Not ported (leads):** the dev reload `31d8e660` @`31d8f6e5` (auth >= 1, DIK_APOSTROPHE); `31fde420` / `31fdc9d0` (restore a bone, then re-apply
  `Bip01_Head` / `Bip01_Spine1` reform on four bodies; caller `3202ed60`) -> **D1-3h** with 205.
- **Split:** 250 filled the session; **205 -> D1-3h** (kickoff `D1-3h.md`, STATUS row; `D1-4-review.md` covers it, plus three D1-3f review points).
- **Bookkeeping:** funcmap `(Diverge D1-3f)` x10; tsv 250 / 205 annotated; JUNE_BUGS 476-478; guards.tsv x2; `MODS.md` D1-3f entry.
- **Hygiene:** `flagcheck SERV_IRUHADEV_JUNE_DIVERGE_UNIT_REFORM --worktree` (7 files): 6 PASS, 1 DEFINE, 0 FAIL, 0 deleted, 0 non-ASCII, 1 EOL-mismatched
  line (the viewer's closing `#endif` takes its CRLF anchor's EOL inside an LF-mostly function); RESULT PASS. `flagcheck SERV_IRUHADEV_JUNE_ENUM_NAMES
  --worktree --files X2Lib/X2Unit.h`: RESULT PASS. `cl /EP` probe: `MOVE_BONE`, `ELSWORD_SECOND_CLASS_CHANGE`, `JUNE_ENUM_NAMES`,
  `X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING` ON (the `#error` guard's inputs).
  After the commit, `--range ba30bb8a^..ba30bb8a`: 6 PASS, 1 DEFINE, 1 ERROR = `X2Unit.h:1252`, D1-3b's widened `#if defined(
  SERV_BALANCE_FINALITY_SKILL_EVENT ) || defined( ..._SKILL_COOL_TIME_RATE )` (not this phase's; listed because the commit touches `X2Unit.h`);
  `SERV_IRUHADEV_JUNE_ENUM_NAMES --range ... --files X2Lib/X2Unit.h`: RESULT PASS.
- **Build:** `DEPLOY OK: X2_offline.exe is 25,843,200 B, dated 2026-10-30 00:14:39, md5 c3cf407f4c8fa398bdd532c8439d8d6a, equal to X2/US_SERVICE/x2.exe
  (2026-10-30 00:14:33); built at main 3db9b108 + 8 uncommitted code file(s).` `EXESTR OK` for `UNIT_REFORM DefaultBoneInfo.lua NOT LOADED` (wide) and
  `g_pViewerSkinMesh` / `AddBoneInfo` (narrow), md5 c3cf407f.
- **Smoke (plan end, after the data036 copy):** every class's head / legs look as in June in the village and the U-key viewer (Elsword's head is
  smaller than March's 0.93: June 0.87); Aisha DW transformed keeps her adult proportions without the 1.1 body scale; Chung is no longer 8 % larger.

### D1-3h Done (2026-10-30, Opus 5.5; `SERV_IRUHADEV_JUNE_DIVERGE_UPDATE_EQUIP_COLLISION_DATA`, `..._RESTORE_HEAD`; March's body in every `#else`)

| Item | March | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 205 | `CX2GUUser::UpdateEquipCollisionData( vector& )` (`X2GUUser.cpp:5773`), Aisha's override (`X2GUArme_VioletMage.cpp:34206`) | `31fd8c00`, `32cc8d30` | `UPDATE_EQUIP_COLLISION_DATA` | attach-anim equips add collision / attack lists only for Add / Rose items; transformed Aisha = transform weapon + own body |
| lead | `CX2GUUser::RestoreHead` (`X2GUUser.cpp:36791`, big-head buff end) | `31fde420` | `RESTORE_HEAD` | face sphere restored, then `CX2Data::SetUnitReForm` for `Bip01_Head` / `Bip01_Spine1` |

- **Both bodies read from the disassembly** (set targets `+0x200` / `+0x210` checked per call; `322a9500` is RET 4, so the pushed 0 is `326bd250`'s third
  argument). Offsets: vslot `0x200` `GetRidingOn`, `+0x683c` `m_ptrRidingPet`, `+0x8e91` / vslot `0x260` (`+0x8e90`) Lu/Ciel body state, equip `+0x15d4`
  a June-only ctor byte (which body), `+0x198` templet, `+0xd2` its unit type (9 `UT_ADD`, 11 `UT_ROSE`), `+0x544` normal anims, `+0x584` Lu/Ciel's
  minor-unit anims (R14b `3295dfa0`, checklist 7aa). Not hyper (`+0x564`), as funcmap R12b had it.
- **Behaviour change (June's, not a guess):** an equip whose templet unit type is not Add / Rose reads `+0x584`, which only Lu/Ciel fills, so every other
  class's attach-anim equips stop adding hit / attack boxes. March has no `+0x584`, so those items add nothing (one-line comment at both sites).
- **Aisha `32cc8d30`** has no Lu/Ciel filter. Transformed with `m_pXSkinAnim`: `GetTransformWeapon()` (vslot `0x680`) anims + own lists, then Rfoot / Lfoot
  attack boxes off, no equips. A NULL weapon leaves both sets empty (JUNE_BUGS 479). Own lists are inserted once after the loop, not 8 times as June does (a set: same
  result). Supersedes CH1i's PARTIAL body (funcmap 4149). Its old line-34204 comment still describes the CH1i body (not touched).
- **Stand-ins (comments, 4 `guards.tsv` rows):** the riding-pet branch at both sites (pet collision, sphere filter `32d2eb50`, own lists skipped when pet
  `+0x478` != 1) went to Stage 17's "riding pet's collision boxes" bullet, which already holds `ENABLE_USER_COLLISION_BOX` / `ENABLE_RIDING_PET_COLLISION_BOX`
  (likely `+0x478` / `+0x47c`). Also the Lu/Ciel body filter and minor anims, and RestoreHead's three Lu/Ciel bodies.
- **RestoreHead = `31fde420`** (string `33473070` = `COLLISION_SPHERE1_Face`, read from the exe). Callers `3202ed60` x2 = `UnSetSpecificValueByBuffTempletID`,
  the same buff cases as March's. June passes the transform byte for every class (`CX2Data::SetUnitReForm` uses it only for Aisha / Lu/Ciel). It drops
  March's `Bip01_Head` restore: the reform's full pass (JUNE_BUGS 477) sets the head again. `+0x8e88` / `+0x8f84` / `+0x8f9c` are Lu/Ciel skin anims (a
  displacement scan finds them only in `~CX2GUUser`, `CX2GULuciel` and RestoreHead). `31fdc9d0` is not ported because its only callers are those bodies.
- **Lead, not ported:** June's `ScaleHead` `31fd64a0` takes ( bone, head scale, face scale, body ) and has 9 callers; March's `ScaleHead()` is fixed at
  2.0 / 1.8. No tsv row covers it -> `D1-4-review.md`.
- **Bookkeeping:** funcmap `(Diverge D1-3h)` x6; tsv 205 annotated; JUNE_BUGS 479; guards.tsv x4; `MODS.md` D1-3h entry; Stage 17 bullet extended.
- **Hygiene:** `flagcheck ..._UPDATE_EQUIP_COLLISION_DATA --with ..._RESTORE_HEAD --worktree` (3 files): `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL,
  0 ERROR`; `in this flag's files: 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; RESULT PASS.
  Each flag alone: RESULT PASS (the OTHER rows are the sibling flag). `enumscan --check`: 0 A/B rows. The `#error` guards' inputs (`JUNE_ENUM_NAMES`,
  `UNIT_REFORM`) are Always.h flags defined above them; the build compiled through both.
- **Build:** `DEPLOY OK: X2_offline.exe is 25,845,248 B, dated 2026-10-30 00:41:31, md5 812a9c232e729eef1111f9fdf022bba0, equal to X2/US_SERVICE/x2.exe
  (2026-10-30 00:41:25); built at main 728c4d5d + 3 uncommitted code file(s).` (PCH rebuilt for Always.h; no new strings to prove.)
- **Smoke (plan end):** Aisha DW: Magical Makeup (transformed) -> she is hit and her hammer hits, foot kicks do not; Add: dynamo hits still register.
  Any class: drink the big-head potion / soup, let it expire -> the head returns to its reform size with no pop.

### D1-4 Done (2026-10-29, Opus 5.5; fourteen `SERV_IRUHADEV_JUNE_DIVERGE_*` flags, March's body in every `#else`)

| tsv | March function | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 287, 288 | `CX2Game::DisplayAddHPInformation` / `MP` | `322d37d0` / `322d38f0` | `ADD_HPMP_INFO` | nothing drawn while dialogs are hidden; HP+ in green |
| 290 | `CX2World::SetMapDetail` | `327fbeb0` | `MAP_DETAIL` | at LOW a layer with InitShow (+0x294) and ForceShow (+0x1f4) stays shown (offsets confirmed: June's non-LOW branch reads the same +0x294 as March's `GetInitShow`, the object loop the same +0x1f4 as `GetForceShow`) |
| 293-296 | `CX2InformerManager::Get*Informer` | `328545c0` `328546a0` `32854780` `32854860` | `INFORMER` | the created informer is returned (March returned an uninitialised pointer); the fifth twin `GetCashShopInformer` (`32854940`, same bug) too |
| 297 | `CX2ItemManager::CreateDummyEquip` | `3280a170` | `DUMMY_EQUIP` | the unit's type for every item (0 when the unit has no templet) |
| 299 | `CX2KeyPad::GetKeyString( GAME_ACTION )` | `31f5ee50` | `KEY_STRING` | a bound key with no key name gives `GET_STRING( 39162 )` (no `STR_ID` name), so `IsSetKey` at X2KeyPad.cpp `GA_RIDING` reads it as set |
| 301 | `CX2LocalMapUI::GetHenirDungeonIcnoDesc` | `32938ea0` | `HENIR_ICON_DESC` | count line is 27305 (March's live `#else` used 13763), no closing 13762 |
| 302 | `CX2LocalMapUI::UdateLocalMapQuestDesc` | `32935d20` | `LOCALMAP_QUEST_DESC` | `bShow` false hides the mark; no `m_mapLocalMapQuestDesc` insert (write-only map) |
| 306 | `CX2Main::GetEDString` | `31d76440` | `ED_STRING` | no comma before a minus sign |
| 314, 326 | `CX2PlayGuide::SetMouseClickIndex` / `StartGoEffect` | `3289d200` / `3289d090` | `PLAY_GUIDE` | the guide arrows are gone (empty, ret 4); the Go effect and its sound need the UI particle system |
| 318 | `CX2UIQuestNew::Handler_EGS_NEW_QUEST_REQ` | `323d2b40` | `COMEBACK_QUEST` | quests 208020-208060 are not requested unless `IsComeBackUser` (premium buff manager +0x5d, funcmap 1173) |
| 327 | `CX2QuestManager::GetNeedToClearDungeonList` | `3232db00` | `NEED_CLEAR_DUNGEON` | June's `( bool bExceptHeroDungeon_, map& )`; the two June local-map callers pass `!m_bHeroMode` (`3293c6f0` @3293cac4 / @3293cc53) |
| 328 | `CX2QuestManager::GetQuestColor` | `323223b0` | `QUEST_COLOR` | grey from 8 levels above (March 6) |
| 341 | `CX2UIQuestNew::UpdateNavi` | `323c8da0` | `QUEST_NAVI` | non-movie half: NPC-friendship quests left out of count and position (`IsFriendshipQuest`, empty until Stage 18) |
| 343 | `CX2UIQuestNew::UpdateRewardItemSlot` | `323d5160` | `QUEST_REWARD_SLOT` | a guild quest (`m_bIsGuild`) leaves the fixed-reward slots alone |

- **No code, the tsv was wrong:** 285 (June still has the defence-dungeon branches: `327d97b0` = `IsDefenceDungeon` 38300-38302, called in
  `3293d280` with `DefenceDungeonDisable`, `3293b5e0` and `322c2e50`); 307 (June's `31e552c0` = the lazily-creating `GetPartyUI`, then
  `UpdatePartyDLG` `322c2e50`, as March; only the play-guide call goes, and `SetMouseClickIndex` is now empty).
- **No code, March already reads as June:** 280 / 300 (`SERV_EVENT_VALENTINE_DUNGEON_INT` is off here, so the Valentine call is compiled out);
  329 (June's `SetHasDungeonQuest` inlines June's `SubQuestInst::IsComplete` `323225e0`, which March's runs under `JUNE_QUEST_RUNTIME`;
  `SERV_POINT_COUNT_SYSTEM` is off); 289 (June's +0x66 gate is the option `MUTE_ON_DEFOCUS`, which `OpenScriptFile` loads as false when absent
  (@32423a7a), so June's body is March's here); 337 / 339 (June's extra branches test UIManager +0x34 / +0x58 / +0x64, June-only UIs: absent here, June's body acts as March's).
- **Corrections to the tsv text:** 326's second state 0xf is June's `XS_GUILD_AGIT_GAME` (June `Enum.lua`), not `XS_SQUARE_GAME`; March has no
  guild agit, so the port keeps village only. 327 / 328's "class filter" is the quest server-group test (+0 == -1 or `g_pInstanceData` +0x120,
  funcmap 4628), which `GetQuestTemplet` already does. 343's NULL-templet destroy is in March too.
- **Stand-ins (one-line comments):** `CreateDummyEquip` keeps the new'd item (June's lives on its stack and June's `CX2Eqip` ctor `32961280`
  takes two more arguments, 0 and 1; March's ctor keeps the item as `m_pDummyItem`); `UdateLocalMapQuestDesc`'s game-edit operator byte
  (+0xf8) read as `XUAL_OPERATOR`, as `UpdatePartyDLG`'s port; Henir challenge count 0 (`SERV_HENIR_RENEWAL_2013` off: no packet fields);
  reward quantity unscaled (June `323d4eb0` multiplies it for an NPC-friendship code event, Stage 18).
- **Split out:** 342 `UpdateQuickQuestDLG` (June `323dd640`: 6 slots and three per-state slot setters `323da7a0` / `323db830` / `323dc470`,
  about 1,800 decompiled lines) -> sub-phase **D1-4b** (`kickoffs/june-march-divergence/D1-4b.md`).
- **Routed:** to D1-3 (kickoff note): `GetNumItemByTID`'s June arguments 4-5 at the quest callers (June passes 1, 0, 1, 0); `CX2Eqip`'s ctor
  arguments from `CreateDummyEquip`. To backlog Stage 9: the `MUTE_ON_DEFOCUS` option (+0x66, save `3241ef40`, GFx `muteOnDefocus`, app
  +0x1f8 +0x352 active test); UIManager +0x34 (refresh `328eea70`) / +0x58 skill-slot UI (June slot types 3 / 0x1a) in `SetNowDragItemReturn`;
  the +0x64 panel (UI menu 47, show `32d754f0`) in `UIOpened`.
- **No `JUNE_BUGS.md` row, no guard:** none of the ported bodies is a June bug that shows offline; no unported class can crash on them.
- **Hygiene:** `flagcheck --worktree --files` per flag: `RESULT: PASS` for all fourteen (0 FAIL, 0 ERROR, 0 deleted lines, 0 non-ASCII,
  0 EOL mismatches); multi-flag files with `--with` (`X2LocalMapUI.cpp`, `X2QuestManager.cpp` + `.h`, `X2UIQuestNew.cpp`); `Always.h` is
  OTHER (other flags, a peer's D1-2 lines uncommitted beside). `git diff --stat`: 11 code files + `Always.h`, 0 deletions. `enumscan --check`:
  the 4 class-A rows are old lines (`X2ItemManager.cpp:367/372`, `X2QuestManager.cpp:6318/6319`). A `/Zs` compile of the 10 `.cpp` passed
  every new `#error` guard before the build.
- **Build:** first run passed. `DEPLOY OK: X2_offline.exe is 25,808,384 B, dated 2026-10-29 14:22:14, md5 2b21db02c39734460c4fb41147c32d6c,
  equal to X2/US_SERVICE/x2.exe (2026-10-29 14:22:03); built at main 0c054a37 + 12 uncommitted code file(s).`
- **Smoke (plan end):** dungeon with dialogs hidden (the hide key): no HP+ / MP+ text; local map at map detail LOW keeps forced layers;
  the Henir icon tooltip ends after the count line; a key-binding slot for an unnamed key shows string 39162; the quest list's colours grey at
  +8 levels; an unported class's quest reward window still fills its slots.

### D1-4b Done (2026-10-29, Opus 5.5; `SERV_IRUHADEV_JUNE_DIVERGE_QUICK_QUEST`, plus `GetQuestColorString` under D1-4's `QUEST_COLOR`)

| tsv | March function | June | Flag `..._DIVERGE_` | What changes |
|---|---|---|---|---|
| 342 | `CX2UIQuestNew::UpdateQuickQuestDLG` | `323dd640` | `QUICK_QUEST` | quests in the custom-event lists (`IsFriendshipQuest`, new `IsChallengeMissionQuest`, both empty here) take no slot; each Complete button's alpha goes back to 1 (base `SetColor`, June vslot 0x84 = `31c932c0`, `m_Color` +0x74) |
| 342 (twin) | `CX2UIQuestNew::SetUIQuickQuestSlotInfo` | `323da7a0` | `QUICK_QUEST` | also skips `QSS_COMPLETE_STORY` / `QSS_CHAIN_WAIT`; a quest with one sub-quest group holding a `SQT_TOGETHER_SUBQUEST_CLEAR` row lists only those rows; `QTS_STORY` titles red (1,0,0,1); every constant else (colours, 155/180, 200, -4.0, `#CFBAF5D`, `...`) read from June and equal to March's live branch |
| 372 (new) | `CX2QuestManager::GetQuestColorString` | `32323000` | `QUEST_COLOR` (D1-4's flag, same change) | grey from 8 levels above (March 6), as `GetQuestColor`; types `QT_EVENT` / `QT_PCBANG` / `QT_CHANGE_JOB` green; found as the only xref of `L"#CB3B3B3"` |

- **Corrections to row 342:** June keeps Minimize/ExpandAll: `323c96c0` / `323c98c0` are March's `MinimizeAllUIQuickQuest` /
  `ExpandAllUIQuickQuest` with the per-slot Minimize / Expand inlined (same 18.0 / 200.0 / +4.0), so the port calls them by name.
  `323db830` / `323dc470` are not the dungeon / field setters. `g_pX2Game +0xbb8` / `+0xbbc` are event managers that `327c9560` creates only
  in `DI_EVENT_2017_FOOLSDAY` (41460, `@327c9663`) / `DI_EVENT_2017_TREASURE` (41470, funcmap `Effect C10d`). `3267b0b0` is not a
  friendship list: it is `32696fa0` (the custom-event manager) `+0x36c` = `MISSION_QUEST_ID` from `CustomEventScript.lua`
  `AddChallengeELMissionEvent` (loader `3267c270`, table string `334b0b94`).
- **Stand-ins (one-line comments):** the two event setters with their 2-slot cap (managers absent: routed to backlog Stage 19, new bullet);
  the slot-dialog restore in `SetUIQuickQuestSlotInfo` (slot +0x45, set only by those setters, byte-scan of `[reg+0x55d]` writes);
  `IsChallengeMissionQuest` returns false (no custom-event manager in March). June's `GET_STRING` string-id debug form (`31c31060` +0x40)
  is the macro, not ported. The `SERV_IRUHADEV_JUNE_QUEST_DATA` NULL-templet guard stays inside June's body (June would crash).
- **Lead for the D1-4 review (kickoff note):** D1-4's `IsFriendshipQuest` names the same manager's `+0x510` map; its filler is not traced.
  June's `IsStateQuickQueskFilter` `323cd490` adds `XS_PVE_GAME` / `XS_PHOTOZONE_GAME` (states March lacks), so the call by name is equal here.
- **No `JUNE_BUGS.md` row, no guard:** no June bug shows offline; every slot field read exists for every class.
- **Bookkeeping:** tsv row 342 source extended, row 372 added; funcmap rows `(Diverge D1-4b)` x5; `MODS.md` line under the D1-4 entry
  (it went into the D1-2b commit `25d16ac1`, which staged `MODS.md` whole); Stage 19 bullet in `JUNE_PORT_BACKLOG_ORDER.md`.
- **Hygiene:** `flagcheck JUNE_DIVERGE_QUICK_QUEST --worktree --files X2Lib/X2UIQuestNew.cpp KTDXLIB/Always.h`: `files: 1 PASS, 0 PASS~,
  0 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted
  line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (`Always.h` OTHER: the D1-2b peer's lines, since
  committed). `flagcheck JUNE_DIVERGE_QUEST_COLOR --worktree --files X2Lib/X2QuestManager.cpp`: 1 PASS, same zero counts, `RESULT: PASS`.
  `enumscan --check`: `X2UIQuestNew.cpp` 0 rows; `X2QuestManager.cpp` only D1-4's old lines (now 6343/6344). A `/Zs` compile of both `.cpp`
  passed with the new `#error` guard.
- **Build:** `DEPLOY OK: X2_offline.exe is 25,809,920 B, dated 2026-10-29 15:03:15, md5 ba10cad3e070b278444da5faa342ed94, equal to
  X2/US_SERVICE/x2.exe (2026-10-29 15:03:08); built at main d831b6c3 + 12 uncommitted code file(s).`
- **Smoke (plan end):** quick-quest window in a village with 2+ ongoing quests: titles, sub-quest lines and the Complete button as before;
  a story quest's title is red; quest list colours grey from +8 levels in the text form too.

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

## Phase S - The plan-end smoke play-test (Opus 5.5, high)

**Depends on:** D2. Added 2026-10-29 (user): every C and D1 phase deferred its play-test to the plan end, so the plan needs a phase that runs it.
Kickoff `kickoffs/june-march-divergence/Smoke.md`: it merges every Done note's "Smoke (plan end)" line into one copy-ready hand-off (ported characters only, Aisha and Eve: user, 2026-10-29;
unported classes are not played and their checks are listed as "not covered"; dungeon / camera, UI / quests), the user plays it, `logs.py` reads `ErrorLog.txt` and the offline logs, a failure
traced to a plan flag is fixed under that flag, anything else is routed by the backlog scope rule. It writes "Phase S Done". D2 review depends on it.

## Phase D2 - Review and close (Opus 5.5, high)

`/x2-phase june-march-divergence D2 review` (after Phase S): the smoke's Done note, selftest, `--check` over X2Lib / KTDXLIB yourself, every sweep fix and allow-list reason,
the wiring. Close: `MODS.md` section (flags, guards, renames by batch, revert), backlog Stage 35 closing note, STATUS. **The plan closes.**

**Standing rule (user, 2026-10-28: "after this plan complete, make it as a rule so this won't happen again").** The naming half is in
`CLAUDE.md` already ("no `June` prefix", 2026-10-28). The close adds the enforcement: `CLAUDE.md` gets the gate (`junediverge.py --check`
must PASS) and the table rule (a review that finds a new divergent March function adds the `june_march_divergent.tsv` row and the
replacement in the same commit); `JUNE_PORT_BACKLOG_ORDER.md` "Standing rules" and `kickoffs/TEMPLATE.md` get a line each; memory is updated.
