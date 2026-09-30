# June2017 effect rebuild -- Phase 11 trial load

Punch list for adopting June's own `EffectSet*`/`DamageEffect*` lua onto the March engine (see
`JUNE2017_EFFECT_REBUILD_PLAN.md`, "End goal" and "Phase 11"). This is **not a keep-or-adopt
verdict** -- it records what blocks adoption and what's already clear.

Status as of 2026-10-01: the Lua-execution-level trial is done and clean, the enum data and
loader-list gaps are built (`x2.exe` 18,979,840 B, 2026-10-01 02:01:48), and the user has decided
every open question below. The client-level trial (real asset pipeline, real play) has not
started -- it needs a throwaway copy of the game directory, built next.

## Done and verified

**1. Enum data.** 968 `(table, member)` values in `march2014/data036/Enum.src.lua`: the original
953 (17 across 7 new tables, 936 into 13 existing tables) plus 15 `HIT_TYPE` members added once
their C++ consumer was widened (see 5 below). Plus 2 renumbered in place (see 6). Detail is in
`JUNE2017_EFFECT_KEY_MATRIX.md`'s "Phase 11 additions and holds" (now carries a `reader` column)
and in `MODS.md`.

Verification, four independent checks on the original 953, repeated for the 15 `HIT_TYPE`
additions and the 2 renumbers:
- Cross-checked the ljd decompile (`june2017_re/june_lua/Enum.src.lua`) against June's `Enum.lua`
  bytecode actually *executed* with the vendored luajit: 0 value mismatches, 0 hallucinated
  entries for any table in this plan's scope. (374 entries the decompile missed entirely are all
  in out-of-scope character-state tables -- `USER_STATE_ID`, `CHUNG_STATE_ID`, `ARA_STATE_ID`.)
- Round-tripped the new March `Enum.src.lua` through the real `mar-2014` compiler
  (`elstools compile-batch`) and executed the result twice (once per round of edits): every
  planned key appears with the right value, nothing unplanned changed.
- Ran all 14 real June effect files (`Enum`, `DamageSound`, `Elsword_DamageData`, `UIEffectSet*`,
  `EffectSet*`, `UnitViewerEffectSetTable`, `DamageEffect*`) through `fxdump.lua` with the new
  March `Enum.lua` substituted for June's own: **every call-site count is identical** to Phase
  10b's baseline (9,035 total calls, 0 differences across all 8 `(file, receiver, method)`
  groups). This is the load-doesn't-abort check the plan calls for -- a missing enum *table*
  aborts the rest of a chunk; a missing *member* just reads nil into that one field.
  - The two builds' `missing_reads` differ only in which pre-existing `SERVER_GROUP_ID` bare-list
    entries (`SGI_GAIA`/`SGI_TEST`/`SGI_WH01`/`SGI_WH02`/`SGI_ZR03` vs. `AET_MAX`/`RWT_END`/
    `SGI_END`) resolve to nil. `SERVER_GROUP_ID` is outside this plan's 40 in-scope tables and
    wasn't touched; both are the same harmless bare-reference pattern Phase 10b already found for
    June's copy (`Enum.src.lua:90-94`), just on March's side of the table instead.
- Audited every extended table's C++ consumer for array-indexing risk, not just Lua-side name
  collisions: `ATTACK_TYPE` (every `.attackType`/`->attackType` site in `X2Lib`, not only
  `X2DamageManager.cpp`), `BUFF_BEHAVIOR_TYPE`/`FINALIZER_TYPE`/`TEMPLET_ID`/`FACTOR_ID`
  (`BOOST_FOREACH` linear scan returning `false`, or `std::map::find`, never a raw array index --
  `GetBehaviorFactor`/`GetFinalizerFactor`, `X2BuffFactor.cpp:46,69`;
  `BuffTempletManager.cpp:112`), `DAMAGE_EFFECT_POSITION_TYPE` (`switch`), `DAMAGE_TRIGGER_TYPE`
  (`m_eDamageTrigger` is stored but has no other reader at all in `X2Lib`/`KTDXLIB`),
  `EXTRA_DAMAGE_TYPE` (`switch`, `GetExtraDamageName`), `UNIT_TYPE` (no reader in this scope at
  all -- the array-sized `CX2Unit::UNIT_TYPE` this could collide with,
  `m_RanderScale`/`m_RanderTemplet`/etc. in `X2Item.h`/`X2UnitManager.h`, is populated by fixed
  `UT_NONE..UT_END` loops untouched by this change, not by anything reading this key),
  `SKILL_ID` (`std::map`/float storage, never indexed; sparse IDs into the thousands rule out an
  array by construction). One exception: `HIT_TYPE`, resolved below rather than held.
  **`ATTACK_TYPE`'s `AT_COMMAND` is memory-safe, not behaviourally complete:** `MODS.md`'s
  `SetDamageExceptionProcess` entry already records that June applies an MP-gain multiplier for
  `attackType == AT_COMMAND (2)` that March's equivalent code doesn't have
  (`SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION`, no code yet). The 784 uses now correctly read
  `AT_COMMAND` instead of defaulting to `AT_NORMAL`, and correctly take neither the `AT_NORMAL`
  nor `AT_SPECIAL` branch anywhere in `X2Lib` -- but that MP-gain multiplier is a separate,
  already-tracked absent-system gap, not something this phase implements.

**2. Compiled and ready to pack.** `march2014/data036/Enum.lua` (mar-2014 bytecode, 265,151 B, the
second, post-`HIT_TYPE`/BFI-renumber compile). **The user packs it**, replacing the existing
`Enum.lua` entry in `data036.kom`. `kom_contents/data036_kom/Enum.src.lua` matched the
pre-Phase-11 tracked repo copy byte-for-byte when checked -- replace both `Enum.src.lua` and
`Enum.lua` there. **This exact `Enum.lua` must not be packed into the live archive before
`x2.exe` (2026-10-01 02:01:48 or later) is deployed there** -- the `HIT_TYPE` members it carries
are only safe with the widened `m_DamageSoundName` array (item 5).

**3. Loader-list gaps, corrected and widened.** New flag `SERV_IRUHADEV_JUNE_LOADER_GAPS`
(`KTDXLIB/Always.h`). Two findings changed this from the original 36/44-only version:
- **`GameMajorParticle%d.txt` is not a fixed-count list in June.** Ghidra decompile of
  `FUN_32597410` (`32597b20`+) shows a self-terminating loop: format `GameMajorParticle%d.txt`,
  open, break on the first miss. June ships 2-67 contiguously (checked on disk, no gaps); March's
  three call sites (`X2Game.cpp`, `X2SquareGame.cpp`, `X2StateStartUp.cpp`) stopped at 23. Now
  open 24-67 at all three (`X2Game.cpp`'s fourth, commented-out 1-9 list, lines 1586-1603, is dead
  code -- confirmed by reading the comment markers, not just grepped). Verified both
  `REALTIME_SCRIPT_PATCH` (the guard on the one live `X2Game.cpp` site) and the new flag survive
  `cl /P` under the real `X2Lib` `US_SERVICE` defines -- not dead code sitting in a false-live
  `#ifdef`.
- **`DamageEffect_Character.lua`/`_ETC.lua` were never opened at all**, not just `_Dungeon.lua`.
  `fxdump`'s `globals` census (14-file run) shows 1,022 damage effects defined only in
  `DamageEffect_Character.lua`, 805 only in `_Dungeon.lua`, 431 only in `_ETC.lua` -- next to
  6,456 in `DamageEffect.lua` and 268 in `_INT.lua`. About 25% of June's damage effects would have
  silently never loaded. Fixed at both `X2Game.cpp` sites, in June's own order
  (`FUN_322e2930`: `DamageEffect`, `_Character`, `_Dungeon`, `_ETC`, `_INT` -- `_INT` last, so it
  still wins any name it redefines, same as before).
- **Not opened, on purpose: `Elsword_DamageData.lua`, `UnitViewerEffectSetTable.lua`.** Neither
  file's consumer exists in March at all -- no `InsertDamageData_LUA` binding, no literal
  reference to either name anywhere in `X2Lib`/`KTDXLIB`. Opening them would just define globals
  nothing reads. Real adoption needs C++ work first; out of this phase's scope.
- Built `US_SERVICE` clean (`KTDXLIB`, `X2Lib`, `X2`; 0 errors each).

**4. `GameMajorParticle` override set computed in full, not just the 18 C++-literal names.**
`june2017_re/particle_override_census.py` -> `particle_override_census.tsv`: every name March's
files 1-23 define, checked against every name June's newly-opened 24-67 define (both parsed with
`fxdata.py`'s own `.txt` parser). **1,401 names collide** (not 18 -- the name contract only sees
names March's C++ *code* asks for by literal, and most particle references are *data*, from
EffectSet/DamageEffect). Of those, **1,223 are byte-identical** between March's copy and June's
(pure reorganization, June just moved them to a later file -- a no-op once packed) and **178
genuinely differ** (see `particle_override_census.tsv`, `verdict` column; concentrated in two
clusters: ~117 Add-character skill effects moved `GameMajorParticle{19,20,21,22,23}.txt` ->
`GameMajorParticle{29,30}.txt` with real content changes, and ~61 event/lobby/property effects
moved into `GameMajorParticle44.txt`). Checked specifically and **confirmed identical** (not a
concern): `BossRaid_Gate_P01-05,07`, `RaidFieldGate`, `HeadMarkerBlue3D`/`BlueMy3D`/`Red3D`/
`RedMy3D`, `PremiumSquareShop` -- the live-content names flagged in the first pass of this
document turned out to be false alarms. The 178 real differs are a play-test item (visual
comparison), not further static analysis -- see the open item below.

**5. `HIT_TYPE` extended rather than held** (user's decision). New flag
`SERV_IRUHADEV_JUNE_HITTYPE_EXTEND` (`KTDXLIB/Always.h`, `X2DamageManager.h`/`.cpp`):
- 15 new `HIT_TYPE` enumerators, explicit values 54-68 (verbatim from June's `Enum.lua`), before
  `HIT_TYPE_COUNT` (now 69, and `m_DamageSoundName[HIT_TYPE_COUNT][HITTED_TYPE_COUNT]` with it --
  the array itself is dimensioned by the enum constant, no separate resize needed).
  `static_assert( HT_STONE == 53 && HT_EXPLOSION_2 == 68 && HIT_TYPE_COUNT == 69, ... )` guards
  against the sequential `#ifdef SERV_9TH_NEW_CHARACTER` numbering silently drifting later.
- 15 new `ParseDamageSoundBlock` calls reading June's own `DamageSound.lua` table names
  (`HT_MAGIC_GLOVE_SLASH`, `HT_MAGIC_GLOVE_PUNCH`, `HT_MAGIC_CIRCLE`, `HT_MAGIC_BULLET`,
  `HT_MAGIC_BULLET_BIG`, `HT_SOUL_HIT`, `HT_SOUL_BIG_HIT`, `HT_CUT_HIT_1/2`, `HT_PIERCE_HIT_1/2`,
  `HT_IMPRINT_HIT_1/2`, `HT_EXPLOSION_1/2`) -- confirmed present in
  `june2017_re/june_lua/DamageSound.src.lua:522-611`, same `HTD_*` shape March's existing entries
  use. **March's currently-live `DamageSound.lua` doesn't have these 15 tables yet** (the asset
  closure never walked `DamageSound.lua`, since it isn't an effect templet) -- until it's adopted
  too, these 15 hit types play with no sound, the same silent default every other `HIT_TYPE`
  already falls back to for an unset `HTD_*` key, not a new failure mode. `DamageSound.lua` is
  already one of the 14 files in the Lua-level trial (`fxdata.py`'s `JUNE_LUA_ORDER`), so adopting
  it is in scope for the client-level trial below, not a separate piece of work.
- Built `US_SERVICE` clean.

**6. `BUFF_FACTOR_ID` renumbered to June's values** (user's decision): `_HAMEL_SECRET_DUNGEON_
ARMOR_EVENT` 365->372, `_WING_EVENT` 366->373. Checked before editing, all clear:
- 372 and 373 are free in both March's Lua `BUFF_FACTOR_ID` table and its C++ enum
  (`X2BuffFactor.h`) -- nothing to displace.
- The full March-vs-June shared-name diff for `BUFF_FACTOR_ID` (502 shared names) has 5 differs
  total, not 2: the two HAMEL names (+7) and three unrelated potion buffs (+119,
  `BFI_BUFF_THE_ESSENCE_OF_WEAK_HERETIC_POTION`/`_HERETIC_POTION`/`RURIEL_MANA_ENERGIZE_POTION`).
  The potion three are outside this plan's 40-table/1327-member scope (no in-scope effect data
  references them) and were left untouched, on purpose.
- Grepped `KncWX2Server/Common`, `X2Lib/Offline` (including `X2OfflineLuaEnum.cpp`) and the two
  exact names generally: zero hits outside `Enum.src.lua`/`Enum.lua` themselves. Not on the wire,
  not persisted by the offline DB. Plan rule 6 doesn't apply.
- The base (non-`_EVENT`) `BFI_BUFF_HAMEL_SECRET_DUNGEON_ARMOR`/`_WING` (280/281, unaffected by
  this renumber) are referenced by name in the live merged `march2014/data036/DamageEffect.src.lua`
  -- confirms `HAMEL_SECRET_DUNGEON` (`KTDXLIB/Always.h:1153`, unconditional) is real, live content,
  not dead.

**7. census.py fixed and current.** The enum-census table only lists rows that are still missing
or differ, so the Phase 11 additions would have silently vanished from
`JUNE2017_EFFECT_KEY_MATRIX.md` once added. Added a "Phase 11 additions and holds" section with a
`reader` column (sourced from the same `enum_rows` data the main table uses, not a separate
count), covering 968 additions (0 currently held) and the 2 renumbers. `PHASE11_ENUM_ADDED`/
`PHASE11_ENUM_HELD` are literal lists near the top of `write()`, since `census.py` has no
git-history awareness of its own. All existing checks still pass.

## Open, needs the client-level trial or the user's play-testing

1. **The 178 real particle differs (item 4).** Not a static question -- needs a visual check once
   the trial client is running, comparing March's current look against June's for a sample of the
   two clusters (Add-character skills, event/lobby effects).
2. **Trial format: a packed trial `data036.kom`** (user's decision), plus a `data121`-equivalent
   archive (or loose, inside the trial copy only) for `GameMajorParticle{24..67}.txt`,
   `DamageEffect_{Character,Dungeon,ETC}.lua` and `DamageSound.lua`'s June version.
3. **Full asset trial** (user's decision): all 3,622 June-only files from
   `JUNE2017_ASSET_CLOSURE.md`, not a narrower Lua-only pass.
4. **Deferred `JUNE_FX` checks that need June's real data to fire**, beyond Phase 2's
   `ES_TEMPLET` lines (`SYNC_SHOW_BY_CREATE_UNIT` on `EffectSet_KILL_ENEMY_WHEN_FULL_HYPER_MODE`,
   `OVERLAP` on `EffectSet_ALH_LINK_FLOWER_DeBuff`): the `DE.LOCK_ON_DATA` family in `census.py`
   (`LOCK_ON_TYPE` case 8 `LOT_UID_VECTOR_AFTER_ATTACK_IN_RANGE`, `RANDOM_SEED`, `INIT_TARGET`,
   `RETARGET_COOLTIME_AFTER_ATTACK`, `UNLOCK_START_AFTER_ATTACK`, `Y_DIR_INIT`,
   `NOT_LOCKON_INVINCIBLE_TARGET`, `LOCK_ON_TARGET_BONE_NAME`) -- all tagged "log: Phase 11"
   because no deployed `LOCK_ON_DATA` block exercises them on merged March data. Needs a
   character/effect that actually uses lock-on re-targeting during play-testing.
5. **Play-test Phase 10b's name-contract punch list** (17 names: 7 dropped, 7 rename candidates,
   1 case change, 2 dead-in-June-too).
6. Whether `KTDGXMeshPlayer`/`KTDGParticleSystem` actually resolve every mesh/particle reference
   June's effect data makes, once real files are in place (the Lua-level trial only proves the
   effect/damage-effect *lua* loads cleanly, not that every asset it points at resolves).
7. `INIT_PHYSIC.WALK_SPEED` load-order dependency (Phase 10b): needs a character script loaded
   first, in the real startup order, to confirm it resolves rather than reading nil.
8. Whether the studio typos June's own data reads as nil (`TURE`, `FLASE`, `FASLE`, `d`,
   `RVK_FlashBang_Ulti_Scale`) produce any visible effect difference in play, or are as harmless
   here as Phase 10b found them to be at the data level.

## Deploying the trial (user always launches -- never offered, always asked or handed off)

The user does the Defender exclusion; when the throwaway copy's path is fixed, it'll be named
here so that can happen before launch. The build to deploy is `X2/US_SERVICE/x2.exe`,
18,979,840 B, 2026-10-01 02:01:48 -- confirm size/mtime against whatever's actually in the copy
before trusting a play-test result, per the standing "a build that did not land is indistinguishable
from a change that did not work" rule.
