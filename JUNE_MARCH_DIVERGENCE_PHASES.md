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
