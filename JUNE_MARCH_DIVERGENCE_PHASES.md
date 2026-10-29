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
