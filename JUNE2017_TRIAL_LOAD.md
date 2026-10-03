# June2017 effect rebuild -- Phase 11 trial load

Punch list for adopting June's own `EffectSet*`/`DamageEffect*` lua onto the March engine (see
`JUNE2017_EFFECT_REBUILD_PLAN.md`, "End goal" and "Phase 11"). This is **not a keep-or-adopt
verdict** -- it records what blocks adoption and what's already clear.

Status as of 2026-10-03: the Lua-execution-level trial is done and clean. The enum data and the
loader-list gaps are built and packed. **June's effect data is staged in the user's
`kom_contents` and waits for the user to pack it.** The client-level trial runs on the live game
directory once it is packed; see *Client-level trial* below. (2026-10-01 had planned a throwaway
copy of the game directory. On 2026-10-03 the user decided to stage straight into `kom_contents`
instead, because it gets packed anyway.)

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
2. ~~Trial format~~: superseded 2026-10-03. Staged into `kom_contents`; see *Client-level trial*.
3. **Full asset trial** (user's decision): all 3,622 June-only files from
   `JUNE2017_ASSET_CLOSURE.md`, not a narrower Lua-only pass. Staged 2026-10-03.
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

## Client-level trial (2026-10-03): staged in `kom_contents`, waiting for the pack

### What was staged

The tool is `june2017_re/stage_june_fx.py` (dry run by default, `--apply` to write). Its full
per-file plan, with the source path, the destination and the reason for each file, is
`june2017_re/stage_plan.tsv`.

- **The 10 June effect lua files March opens**, converted with `ljconv.py` (Phase 10b, route
  (b)). Each output is byte-identical to Phase 10b's `assets_cache/june_b/` copy.
  - Into `data036_kom`, replacing March's copy: `EffectSet.lua`, `UIEffectSet.lua`,
    `DamageEffect.lua`, `DamageSound.lua`.
  - Into `data036_kom`, new: `DamageEffect_Character.lua`, `DamageEffect_Dungeon.lua`,
    `DamageEffect_ETC.lua`.
  - Into `datai015_kom`, replacing March's copy: `EffectSet_INT.lua`, `UIEffectSet_INT.lua`,
    `DamageEffect_INT.lua`.
- **Files deliberately not staged:**
  - June's `Enum.lua`. The Phase 11 March `Enum.lua` stays. June's would undo the three potion
    `BFI_*` values that were left alone on purpose, and would change the character-state tables.
  - `EffectSet_Description.lua`, which only `EFFECT_TOOL` reads.
  - `Elsword_DamageData.lua` and `UnitViewerEffectSetTable.lua`, which March has no consumer for
    (item 3 above).
- **All of June's particle and mesh-player `.txt` files**, under the "June wins at the same
  filename" rule. Each one differs from March's.
  - Replaced: `GameMajorParticle.txt` and `GameMajorParticle2-8` and `10-23`,
    `GameMajorXMeshPlayer.txt`, `GameMinorParticle.txt`, `GameMinorXMeshPlayer.txt`, and the four
    `UI{Major,Minor}{Particle,XMeshPlayer}.txt` files.
  - `GameMajorParticle9.txt` was already identical.
  - New: `GameMajorParticle24-67.txt`, which were not staged before. All of these are in
    `data036_kom`.
- **Assets from the closure.**
  - 3,622 June-only files are added. Each goes into June's own archive number, which is the
    Ranox/Lanox convention.
  - 59 `differs` files are replaced in the kom March already ships them in.
  - 543 of the added and replaced files are elstools-short June extracts, 1-255 bytes short.
    The user has confirmed these work in the March client.
- **Totals.** 3,669 files added and 95 replaced, across 60 koms. Each `_manifest.json` was
  extended in its own style: LF, 2-space indent, no trailing newline, and new keys appended at
  the end. Values: `"2"` for `.lua` **and `.txt`**, `"0"` for everything else. The live
  `data036.kom` stores every particle and mesh-player `.txt` as algo 2. The 44 new
  `GameMajorParticle24-67.txt` entries were first written as `"0"` and corrected on 2026-10-03.
  Twelve koms are new archives the client already mounts (`KOM_FILE_COUNT_226`):
  `data158`, `159`, `180`, `182`, `185`, `189`, `197`, `208`, `211`, `212`, `218`, `222`.
- **Moved out of `kom_contents`:** `data036_kom/EffectSet.src.lua` and `DamageEffect.src.lua`,
  the Ranox-merged sources. They are in `24965799/kom_contents_backup_june_fx/data036_kom/`,
  together with a copy of every file that was replaced (25 MB).
  - **Why they were moved:** `elstools compile-dir` rebuilds `X.lua` from any `X.src.lua` beside
    it and overwrites the existing `X.lua`. This was tested on scratch files. A `.lua` with no
    `.src.lua` beside it is left alone. If these two stayed, the next pack would silently put the
    merged copies back.
  - The git copies in `march2014/data036/` are untouched. Retiring those is step 4 and waits for
    the user.
- **Not staged: the 15 `truncated-in-march` files.** March's live copy is already the same
  truncated bytes that elstools extracts from June. Replacing them changes nothing until June's
  archive is extracted by a tool that does not truncate.

### Undoing it

`python june2017_re/stage_june_fx.py --revert`. A `--revert --check` run first shows what it
would do; its 2026-10-03 output was: 95 restored, 3,282 deleted with their manifest keys, 12 new
directories removed, and 2 `.src.lua` moved back. The revert asserts that every manifest returns
to its exact pre-staging key list.

### Interaction with other ports

- **Any pack of a staged kom now ships this adoption.** That is all of `data036` and 59 other
  koms, whatever the pack is for. If another port needs a `data036` pack before the trial is
  wanted, run `--revert` first, or pack that port's kom from a clean extract.
- **If a sync copies `march2014/data036/*.src.lua` into `kom_contents`, the merged copies come
  back** at the next `compile-dir`, until step 4 retires them.
- **The buff port's tooling is unaffected.** `buff_census.py` reads only
  `BattleFieldBuff*`/`BuffTemplet.lua` from `kom_contents`. The `DamageEffect.src.lua` line
  numbers in `JUNE_BUFF_PORT_PHASES.md` refer to `march2014/data036/`, which is untouched.
  - **Once packed, the buff port's play-tests run on June's `DamageEffect` `BUFF_FACTOR` blocks.**
    That is the data the buff port targets, but the baseline changes.
- **`census.py`'s mesh-player and particle side** reads `kom_contents/data036_kom` and
  `datai015_kom` (`MP_MARCH_DIRS`). After staging, its "March" `.txt` rows are June's files, so
  the Phase 10 counts change when it is rerun.
- **The shared save now has four `jt*` test characters** (below). Anyone testing another port
  will see them at character select.

### What the staging checked so that it does not collide with other work

- **No overwriting of unpacked work.** A file is replaced only where the `kom_contents` copy
  matches the live `.kom` manifest size. The same goes for any name staged but not yet packed,
  in `kom_contents` or in `march2014/update/`. The only collisions were the 15 truncated files
  above.
- **No duplicates across koms.** Under `_IN_HOUSE_`, a name that exists in two koms raises a
  MessageBox at mount (`KGCMassFileManager::AddMassFile`). Every replacement stays in March's
  kom, and every added name is in no live manifest. After staging, no staged name appears in
  more than one `kom_contents` directory.
- **Mounting.** Every destination kom falls within the mounted ranges `data001`-`226` and
  `datai001`-`032` (`X2Main.cpp:799-845`).
- **Unrelated, already present:** 1,084 manifest keys in `data101_kom` (1,071) and `data023_kom`
  (13) have no file beside them. None of them are from this staging.

### For the user to pack

- Repack every kom listed below from `kom_contents`. A number is the count of added files,
  `~N` the count of replaced files, and `NEW` marks an archive that does not exist yet:

  data002 (+1356 ~5), data005 (~1), data006 (~2), data009 (+1), data011 (+1 ~1), data012 (+22 ~1),
  data013 (+5), data016 (+21), data017 (+1), data018 (+2), data023 (+8), data036 (+47 ~33),
  data046 (+1), data048 (+25 ~3), data050 (+36), data069 (+48), data070 (+24), data078 (+9),
  data100 (~2), data106 (+2), data116 (+28 ~2), data117 (+10 ~1), data118 (+97 ~3),
  data119 (+31), data120 (+43 ~1), data121 (+62 ~3), data125 (+5 ~1), data129 (+4), data131 (~1),
  data132 (+128 ~7), data135 (+4), data136 (~18), data140 (+31 ~2), data141 (+9), data142 (+102 ~2),
  data143 (+2), data146 (+2), data147 (+172 ~2), data148 (~1), data155 (+7), data157 (+322),
  data158 (+7 NEW), data159 (+9 NEW), data180 (+10 NEW), data181 (+24), data182 (+2 NEW),
  data184 (+386), data185 (+11 NEW), data189 (+5 NEW), data197 (+12 NEW), data208 (+1 NEW),
  data211 (+1 NEW), data212 (+324 NEW), data218 (+1 NEW), data222 (+4 NEW), data223 (+190),
  datai001 (+9), datai007 (+4), datai015 (~3), datai017 (+1).
- The 10 lua files are already mar-2014 bytecode. They are not XOR'd yet; packing does that.
  `compile-dir` on `data036_kom` is now harmless, because no `EffectSet`/`DamageEffect`
  `.src.lua` is left there.
- `Enum.lua`: `kom_contents/data036_kom/Enum.lua` is byte-identical to
  `march2014/data036/Enum.lua` (314,915 B, md5 `81f0b944...`). `data036.kom` was repacked after
  it was staged. The `ES_LOAD`/`KEM_ERROR` lines below confirm that it is live.

### Packed and deployed (2026-10-03, at the user's request)

- **Compile.** `elstools compile-dir -v mar-2014` on `data036_kom` rebuilt its 13 `.src.lua`.
  - The 10 June effect `.lua` came out byte-identical.
  - `Enum.lua` matched the copy already packed.
  - `datai015_kom` has no sources, so nothing ran there.
  - Pass an absolute `-d`: a relative path failed on all 13 and wrote nothing.
- **Pack.** `elstools pack -v mar-2014` built all 60 koms to `kom_contents/dataXXX_packed.kom`,
  0 failed.
  - Read back with `elstools inspect`: every kom holds exactly the files in its folder, at their
    on-disk sizes, with the manifest's algorithm.
  - Do not drive `pack` from a bash `while read` loop: it produced nothing that way. Use a loop
    that gives each process its own stdin.
- **Deploy.**
  - Copied to `data/dataXXX.kom`; each copy's SHA-1 matches its source.
  - The 48 live koms these replace were moved to `24965799/kom_backup_pre_june_fx/` first.
  - 12 koms are new.
  - To undo: move the backups back, and delete the 12 new koms from `data/`.

### Test characters (added to `els_db.sql` 2026-10-03, at the user's request)

The tool is `june2017_re/phase11_test_units.py`. It mirrors `CX2OfflineDB::CreateUnit`: the
`unit` row plus the default `inventory_size` rows, copied from level-1 unit 20. All four are
level 80 with 600 SP and 1,000,000 ED. Learn skills and set hotkeys in game. The backup is
`db_backup/els_db_20261003_225109_pre_phase11_units.sql`.

| uid | class | name | covers |
|---|---|---|---|
| 25 | 100 Lord Knight | `jtLordKnight` | Sand Storm -> `LORD_KNIGHT_SAND_STORM` |
| 26 | 101 Rune Slayer, **to become 112 Infinity Sword** | `jtRuneSlayer` -> `jtInfinitySword` | Double Slash, awakened -> `EffectSet_ELSWORD_DOUBLE_SLASH_HYPER`. This is a Sheath Knight skill, so 101 was the wrong class. The switch waits for the game to be closed |
| 27 | 109 Code Empress | `jtCodeEmpress` | dash combo Z -> `EVE_EP_DASH_COMBO_Z` |
| 28 | 105 Void Princess | `jtVoidPrincess` | Death Field (`AVSI_SA_ADM_DEATHFIELD`) -> `EffectSet_Aisha_DeathField_*` |

Existing characters cover the rest:
- `reyaa` (103, Grand Archer): Arc Shot, Cryotron Bolt.
- `Freils` (111, Deadly Chaser): Leg Shot.
- `ravbe` (107, Reckless Fist): Maximum Cannon and Seven Burst in hyper mode, OT combo XXfrontX,
  Valkyrie's Javelin, and the `INIT_PHYSIC` check below.

Add is not in this build.

### Static verdicts, before play

- **Name-contract call sites (Phase 10b, 15 names): none crashes.**
  - Each damage-effect site either null-checks the result of `CreateInstance` or discards it.
  - `PlayEffectSet` returns `INVALID_EFFECTSET_HANDLE`, and both Aisha sites test for it.
  - On adoption, then, a missing name means a **missing effect, not a crash**.
  - A missing effect set logs `[RANOX_EFFECT_MISSING] EffectSet not found: <name>` in
    `offline_server.log` (`X2EffectSet.cpp`, `PlayEffectSet`), whatever the caller is.
  - **A missing damage effect logs nothing.** `CreateInstance` returns NULL silently. Those
    names are checked by eye.
- **Script users (new, not in Phase 10b):** 20 names (first counted as 17) that March's own character and NPC scripts
  use. The merged effect files define them; June's do not. Found by scanning every non-effect
  `.lua` in `kom_contents` for exact constants.
  - The first scan checked that the byte *after* a name was not an identifier character. In
    LuaJIT bytecode that byte is the next constant's length, so it can be a letter, and the scan
    missed 3 names. It now matches the `ULEB128(len+5)` length prefix plus the bytes. The miss
    surfaced in play, when `..._GUARDIAN_STRIKE_HYPER` logged. The particle and mesh-player scan
    was redone the same way, and its result did not change.
  - **14 where June changed the script that uses them.** These break unless the script is
    adopted too. These are character and NPC scripts, outside this plan's census; they belong to
    the character ports (`JUNE2017_CHARACTER_PORT_CHECKLIST.md`), not to this phase:
    - Raven `EffectSet_RAVEN_OVER_TAKER_GUARDIAN_STRIKE`, `_MEMO`, `_HYPER`, `_HYPER_MEMO`,
      `EffectSet_RAVEN_VJAVELIN_LAND`, `Effectset_A_RWT_FLAME_SWORD_MEMO` (`RAVEN_FIGHTER.lua`);
    - Rena `Effectset_Rena_DiveKickBombingA`/`B`/`B_Memo` (`Lire_ElvenRanger.lua`);
    - Aisha `EffectSet_AHM_Blaze_Step_Damage_MEMO`, `EffectSet_Aisha_Blaze_step_MEMO`
      (`Arme_VioletMage.lua`);
    - PvP-hero NPCs `EffectSet_PVP_BOT_EEP_DashAttackZ` (`*_APPLE.lua`) and
      `EffectSet_RAVEN_VJAVELIN` (`*_BALAK.lua`).
  - **6 that June's own scripts still use but June's effect files never define**, so they are
    dead in June too:
    - `EffectSet_CSI_CSG_COMBO_XFRONTX_FIRE`, `_PULL_SUBWEAPON` (Chung);
    - `AISHA_SUMMON_BAT_BITE`, `ANCIENT_PPORU_BREATH`, `BOMB`,
      `DAMAGE_EFFECT_MAGIC_TENTACLE_MAGIC_ATTACK`;
    - `DamageEffect_RRF_ExplosiveHand_Impact`. **Seven ANU monster scripts use this one.**
  - Note: a Lua script that indexes a missing damage-effect global gets nil, so the script's own
    handling decides what happens.
- **Particle and mesh-player templets lost by swapping the `.txt` files: none that March data
  uses.**
  - March defines 128 templets that June's `.txt` files do not. Outside the effect files being
    replaced, the only users are two names that `datai015_kom/GameMajorXMeshPlayer_INT.txt`
    defines itself, so they stay.
  - Two junk test names, `111` and `tt`, match unrelated strings.
- **`INIT_PHYSIC.WALK_SPEED`: probably an adoption blocker.**
  - `DamageEffect.lua` reads it in `DamageEffect_RF_DASH_COMBO_Z` (`.src.lua:351020`), and 147
    top-level templets follow it.
  - `CX2DamageEffect` runs its chunk in a `KLuaManager` with its own environment
    (`X2DamageEffect.cpp:134`, `bInNewEnv=true`; `KLuaManager::AssignNewLuaState` makes a fresh
    globals table that falls back to the shared one through `__index`).
  - Unit scripts assign `INIT_PHYSIC` in their own private environments (`X2GameUnit.cpp:14`, the
    same flag). So the read most likely gets nil, raises an error, and every templet from
    `DamageEffect_RF_DASH_COMBO_Z` to the end of the file (`DamageEffect_Erda_Mid_All_Super_Armor`
    is the last) is never defined.
  - Whether June's own engine has the same hole is not known.

### Pass criteria, once packed (read the live game directory's logs)

**At startup (`ErrorLog.txt`):**
- **Exe under test:** the deployed `X2_offline.exe`, 19,015,168 B, SHA-256 `cccaf8fb28e7826d...`.
  It contains the loader-gap literals (`DamageEffect_Character`/`_Dungeon`/`_ETC.lua`,
  `GameMajorParticle67.txt`) and `JUNE_FX ES_LOAD`. If the exe has changed, check those again
  first.
- `JUNE_FX ES_LOAD EffectSet.lua loaded=1 templets=8242 ... effects=43817`, then `UIEffectSet.lua`
  287 / 1,546, `EffectSet_INT.lua` 138 / 762, `UIEffectSet_INT.lua` 12 / 40.
  - `templets` counts `AddEffectSetTemplet_LUA` calls (`++s_iJuneFxTempletCount`, once per parsed
    templet).
  - `effects` counts `EFFECT%d` up to the first gap, because the client loops
    `while BeginTable( "EFFECT", i )`.
  - Both expected values come from `assets_cache/june_fx.json`. March's merged figure was 3,134
    templets.
- No `KEM_ERROR73` for any of the 10 lua files. The one predicted exception is `DamageEffect.lua`,
  if the `INIT_PHYSIC` read fails.
- **Deferred Phase 2 checks:** `JUNE_FX ES_TEMPLET EffectSet_KILL_ENEMY_WHEN_FULL_HYPER_MODE`
  carrying `SYNC_SHOW_BY_CREATE_UNIT=1`, and `JUNE_FX ES_TEMPLET EffectSet_ALH_LINK_FLOWER_DeBuff`
  carrying `OVERLAP=1`. Both sets have exactly these June keys in June's data.
- No duplicate-file MessageBox at mount.
- A file-not-found line for an asset is expected only if its name is one of the 156
  `nowhere/nowhere` rows of `june2017_re/asset_closure.tsv`, which June's own data names but
  neither build ships. Any other such line is a real finding.
- No `Unexpected "..." in Sequence Block!` MessageBox from the new `.txt` files.

**In play (`offline_server.log`):**
- `grep -a "RANOX_EFFECT_MISSING" offline_server.log | sort | uniq -c` after a session. Every line
  must be one of the names above.
- `INIT_PHYSIC`: as `ravbe`, dash combo Z. If it plays, the chunk ran past line 351020.
  Confirm from the console:
  `jDE:CreateInstance_LUA(jU, "DamageEffect_Erda_Mid_All_Super_Armor", jP, jY)`.
  Something should appear.
- The 15 name-contract call sites, with the characters above. Each should be missing, not crash.
- Also owed here: the 178 changed particles (item 1 above), and the lock-on keys (item 4) on any
  lock-on skill.

### Startup result (session 2026-10-03 23:28, read from `ErrorLog.txt`)

- **PASS: `ES_LOAD`.** Each file loaded (`loaded=1`) with the expected templet count:

  | file | templets | effects |
  |---|---|---|
  | `EffectSet.lua` | 8,242 | 43,775 |
  | `UIEffectSet.lua` | 287 | 1,540 |
  | `EffectSet_INT.lua` | 138 | 762 |
  | `UIEffectSet_INT.lua` | 12 | 40 |

  The `effects` figures are 42 and 6 below the earlier expectation for `EffectSet` and
  `UIEffectSet`. June's own data has exactly 42 and 6 `EFFECT%d` entries with no `EFFECT_TYPE` or
  an empty `NAME`, and the parser rejects those by design (`goto effect_error_proc`,
  `X2EffectSet.cpp:3713/3718`). So the counts match June's data to the entry.
- **PASS: deferred Phase 2 checks.** Both lines appear:
  - `JUNE_FX ES_TEMPLET EffectSet_KILL_ENEMY_WHEN_FULL_HYPER_MODE SYNC_SHOW_BY_CREATE_UNIT=1`
  - `JUNE_FX ES_TEMPLET EffectSet_ALH_LINK_FLOWER_DeBuff OVERLAP=1`, plus `_Explosion`.
- **Not ours:**
  - Two `ErrorEnum(72)` lines with an empty file name. These are the buff port's
    `BattleFieldBuff{Templet,Factor}_INT.lua`, which are not packed yet (its own log line says it
    expects them in Phase 13).
  - `ErrorEnum(73) TutorSystem.lua`. That is a loose local file, not loaded from a kom.
- **Open: `BOSSRAID_ABYSS_SMOG.DDS` not found** (`ErrorEnum(70)`, twice, 23:28:51).
  - June ships it as `BossRaid_Abyss_smog.dds` (and `_smog2`).
  - It is not in the effect closure.
  - No staged `.txt`, `.xet` or plain-text file names it. The likely source is a compressed mesh
    staged here.
  - The fix is to add those two textures. Find the referencing file before packing them.
- **Results of the same session, in game (logs read at 23:35 and 23:38):**
  - **`INIT_PHYSIC` is not a blocker.** `CE_FRAME` lines were logged for
    `DamageEffect_RF_DASH_COMBO_Z` (the templet that reads it) and
    `DamageEffect_Erda_Mid_All_Super_Armor` (the last templet in the file), so the chunk runs past
    `.src.lua:351020` to the end. The static prediction above was wrong: a shared-global
    `INIT_PHYSIC` must exist when the chunk runs. Where it comes from is not pinned down.
  - `DamageEffect_CIP_3_HyperActive_Last_Bomb_01` also logged a `CE_FRAME` line, so the
    `DamageEffect_Character` file loaded.
  - No `ErrorEnum(73)` for any `DamageEffect*.lua`.
  - The `_ETC` and `_INT` last templets carry no June key, so they log nothing. The user's
    observation is the only check for them.
  - Missing effect sets (`RANOX_EFFECT_MISSING`), all expected:
    - `EffectSet_RAVEN_VJAVELIN` and `_LAND` (Valkyrie's Javelin);
    - `EffectSet_RAVEN_OVER_TAKER_GUARDIAN_STRIKE_HYPER`.
    - No crash.
  - The normal Maximum Cannon and Seven Burst hit. The `*_HYPER` paths (awakened) are still owed.
  - The `PVP_HERO_APPLE` NPC spawned, and its damage effects run (`CE_LOOP`/`HIT`). Its
    `EffectSet_PVP_BOT_EEP_DashAttackZ` has not logged yet.
  - The buff port should know: `[JUNE_BUFF] BFI 653` and `BFI 1129` were requested but are not
    loaded. June damage effects name buff factors this build lacks, so nothing is applied.
  - `offline_packets.log`: no `EXCEPTION`. The only `UNHANDLED` is `E_DISABLE_HB_CHECK_REQ`.
- **Previously owed:** the `DamageEffect*.lua` load. Damage effects open when a game starts, not at the
  title screen, so their lines come with the first dungeon.

### Play-test steps

**Before you start**
- Close the game. Test character 26 was inserted as Rune Slayer (101) by mistake: Double Slash is
  a Sheath Knight skill (`ESSI_SA_ESK_DOUBLE_SLASH_HYPER`). It is to become `jtInfinitySword`
  (112), which needs the game closed. Until then, skip step 5.
- After every step: if the client crashes, stop and note the step, the skill and the time.

**1. First dungeon: `DamageEffect` load and `INIT_PHYSIC`**
1. Log in with `ravbe` (Reckless Fist) and enter any dungeon.
2. Quit to the title screen or desktop, then check:
   - `ErrorLog.txt` has no `ErrorEnum(73)` naming `DamageEffect.lua`, `DamageEffect_Character.lua`,
     `DamageEffect_Dungeon.lua`, `DamageEffect_ETC.lua` or `DamageEffect_INT.lua`.
   - **If `DamageEffect.lua` has a 73, the `INIT_PHYSIC` problem is confirmed.** Steps 2-3 still
     run, but every templet after `DamageEffect_RF_DASH_COMBO_Z` is missing.
3. Re-enter the dungeon. Press **END** for the console, then run these five lines one at a time:
   ```lua
   jU = g_pX2Game:GetMyUnit()
   jES = g_pX2Game:GetEffectSet()
   jDE = g_pX2Game:GetDamageEffect()
   jP = jU:GetPos(false)
   jY = jU:GetLandPosition_LUA(false).y
   ```
4. Then:
   ```lua
   jDE:CreateInstance_LUA(jU, "DamageEffect_RF_DASH_COMBO_Z", jP, jY)
   jDE:CreateInstance_LUA(jU, "DamageEffect_Erda_Mid_All_Super_Armor", jP, jY)
   ```
   - `DamageEffect_RF_DASH_COMBO_Z` is the templet that reads `INIT_PHYSIC`.
     `DamageEffect_Erda_Mid_All_Super_Armor` is the last templet in the file.
   - **Pass:** something visible appears for both.
   - **Fail:** nothing appears for either. That means the chunk stopped there.
5. Also check one templet from each new file, which proves each one loaded:
   ```lua
   jDE:CreateInstance_LUA(jU, "DamageEffect_CIP_3_HyperActive_Last_Bomb_01", jP, jY)
   jDE:CreateInstance_LUA(jU, "DamageEffect_SpaceMRabbit_AttackX", jP, jY)
   jDE:CreateInstance_LUA(jU, "DamageEffect_Riding_Ceta_AttackC_Whale2", jP, jY)
   ```
   These are the last templets of `DamageEffect_Character`, `_ETC` and `_INT`.
   - A failed line prints only the generic invalid-command message. If one shows nothing, retry
     it once standing on flat ground.

**2. Raven: `ravbe` (Reckless Fist, 107)**
- Learn and hotkey **Maximum Cannon**, **Seven Burst** and **Valkyrie's Javelin**. Use SP if
  needed.
- Steps 2-3 need **awakening (hyper mode)**. The `*_HYPER` code paths run only when the skill is
  used while awakened, so fill the awakening gauge on monsters first. `/CM "RAGE_BABYFIG" 4 1`
  spawns five.

| do | name being tested | expect |
|---|---|---|
| Valkyrie's Javelin (normal, then awakened) | `EffectSet_RAVEN_VJAVELIN` (C++), `EffectSet_RAVEN_VJAVELIN_LAND` (script) | the ground-impact effect is missing; the skill works; no crash; two `RANOX_EFFECT_MISSING` lines |
| Maximum Cannon, awakened | `RAVEN_MAXIMUM_CANNON_HYPER`, `RAVEN_MAXIMUM_CANNON_FLY_HYPER` | the cannon shot is missing or does no damage; no crash; no log line (damage effects do not log) |
| Seven Burst, awakened | `RAVEN_SEVEN_BURST_1_HYPER`, `_2_HYPER` | the burst projectiles are missing; no crash |
| combo X X then (forward + X) | `RAVEN_OVER_TAKER_XX_FRONT_X` | the last hit's damage effect is missing; no crash |
| Guardian Strike, if learnable | `EffectSet_RAVEN_OVER_TAKER_GUARDIAN_STRIKE` (script) | effect missing; `RANOX_EFFECT_MISSING` line |

**3. Rena: `reyaa` (Grand Archer, 103)**

| do | name | expect |
|---|---|---|
| **Arc Shot** | `DamageEffect_LGA_Arc_Shot` (x5 arrows) | the arrows are missing; no crash |
| **Cryotron Bolt** (hyper active; only if the tree lets you learn it) | `DamageEffect_Cryotron_Bolt_GUIDED_ARROW` (June renamed it `..._Guided_Arrow`) | the guided arrow is missing; no crash |
| any skill or combo named Dive Kick Bombing | `Effectset_Rena_DiveKickBombingA/B/B_Memo` (script) | effect missing; `RANOX_EFFECT_MISSING` |

**4. Chung: `Freils` (Deadly Chaser, 111)**

| do | name | expect |
|---|---|---|
| **Leg Shot** | `DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell` | the mini-gun shell is missing; no crash |

**5. Elsword: `jtInfinitySword` (after the class fix) and `jtLordKnight` (100)**

| char | do | name | expect |
|---|---|---|---|
| `jtInfinitySword` | **Double Slash**, awakened | `EffectSet_ELSWORD_DOUBLE_SLASH_HYPER` | effect missing; `RANOX_EFFECT_MISSING`; no crash |
| `jtLordKnight` | **Sand Storm** | `LORD_KNIGHT_SAND_STORM` (June has `DamageEffect_ELK_Sand_Storm_Main`) | the storm does nothing; no crash |

**6. Eve: `jtCodeEmpress` (109)**

| do | name | expect |
|---|---|---|
| dash (double-tap a direction) then **Z** | `EVE_EP_DASH_COMBO_Z` | the dash-Z projectile is missing; no crash |

**7. Aisha: `jtVoidPrincess` (105)**

| do | name | expect |
|---|---|---|
| **Death Field**, normal and awakened, near a monster (within about 400 units) | `EffectSet_Aisha_DeathField_FirstImpact`, `EffectSet_Aisha_DeathFieldBomb` | both effects missing; the Death Sentence debuff still applies; 2 `RANOX_EFFECT_MISSING` lines; no crash |

**8. Monsters that use names June dropped** (any character, in a dungeon, one at a time; `/ak`
clears them)

| command | name | expect |
|---|---|---|
| `/CM "PVP_HERO_APPLE" 0 1` | `EffectSet_PVP_BOT_EEP_DashAttackZ` | when it dash-attacks: effect missing, `RANOX_EFFECT_MISSING` |
| `/CM "PVP_HERO_BALAK" 0 1` | `EffectSet_RAVEN_VJAVELIN` | the same |
| `/CM "ANU" 0 1` | `DamageEffect_RRF_ExplosiveHand_Impact` | dead in June too: the attack has no effect; **watch for a crash** |
| `/CM "MAGIC_TENTACLE" 0 1` | `DAMAGE_EFFECT_MAGIC_TENTACLE_MAGIC_ATTACK` | dead in June too: no attack effect; watch for a crash |

**9. General look-around** (5-10 minutes, any characters)
- Play two or three dungeons with characters whose skills changed in June, such as Add-era
  skill effects. Watch for effects that are now opaque, see-through, oddly coloured or missing.
  These are the 178 changed particles.
- Any lock-on skill counts too (e.g. Rena's guided arrows, Chung's homing missiles):
  `offline_server.log` should gain `JUNE_FX CE_LOCKON` lines.

**After the session**
- Quit normally. I then read:
  - `grep -a "RANOX_EFFECT_MISSING" offline_server.log | sort | uniq -c`. Every name must be in the
    tables above. Any other name is a new finding.
  - `grep -a "ErrorEnum(7[023])" ErrorLog.txt`: any missing file or failed lua load.
  - `grep -a "JUNE_FX CE_LOCKON\|JUNE_FX unsupported" offline_server.log`.
  - `offline_packets.log` for `EXCEPTION`.
- Tell me which steps you could not do (skill not learnable, monster would not spawn). Those stay
  open in this list.

### Play-test result: console route and monster spawns (2026-10-03, 23:42-23:47)

The per-character skill steps (2-7) were replaced by the console route. The missing names are
already proven absent, by executing June's data. All 15 C++ call sites were read and handle a
missing name. `PlayEffectSet_LUA` reaches the same `PlayEffectSet` that logs
`RANOX_EFFECT_MISSING` (`X2EffectSet.cpp:5254-5260`). So the console exercises exactly the lookup
that fails, with no class, skill or awakening needed.

- **PASS: all 18 effect-set names logged `RANOX_EFFECT_MISSING`, no crash.** These are the 5 C++
  names plus the 13 script names not already hit in play.
- **PASS: control.** `EffectSet_Rena_Perfect_Storm` played (`ES_CREATE` / `ES_UPDATE` /
  `ES_DELETE`) and was not logged as missing.
- **PASS: monster scripts that name dropped effects.** `PVP_HERO_APPLE`, `PVP_HERO_BALAK`, `ANU`
  and `MAGIC_TENTACLE` were spawned and allowed to attack, with no crash and no Lua error in
  `ErrorLog.txt`.
  - Apple's real dash attack logged `EffectSet_PVP_BOT_EEP_DashAttackZ`.
  - Balak's javelin raised the `RAVEN_VJAVELIN`/`_LAND` count to 5 each.
- **PASS: the repack lost nothing.** All 48 replaced koms, old (`kom_backup_pre_june_fx/`) against
  new (`elstools inspect`): 0 entries dropped.
- **Not ours:**
  - Texture-open failures for Ruben village and field textures (`RUBEN_VILLAGE_*.DDS`,
    `WORLDLAYER_FIELD_RUBEN.TGA`, ...). They are in no live kom, before or after the change.
  - `BFI 512` requested but not loaded: the buff port.
  - `MP_CRASH` lines for `MAG_CHARGER_Attack_A` and Apple's `Eve_EP_ComboX2DX_Bomb*`. These are the
    Phase 9/10 diagnostics (T6 data).
  - No `EXCEPTION` in `offline_packets.log`.
- **Bonus:** `DDE_TEMPLET` fired on `DamageEffect_EMK_SwordFire_Ignitor_HYPER`
  (`LOOP_INTERVAL=0.066 LOOP_END_COUNT=9 LOOP_END_TIME=5.000`). This is Phase 6a's line, on June
  data.
- **Damage-effect names (15 C++, 4 script) are covered statically only.** `CreateInstance` does
  not log a miss. Their absence is certain, and every C++ site was read null-safe. The four
  monster scripts survived in play.

### Adoption punch list (as of 2026-10-03)

| # | item | blocks adoption? |
|---|---|---|
| 1 | `BOSSRAID_ABYSS_SMOG.DDS` not found. June ships `BossRaid_Abyss_smog.dds` / `_smog2.dds`; the referencing file, probably a compressed mesh, is not yet found | no: one missing texture |
| 2 | 15 C++ call sites name effects June dropped or renamed; they are now silently missing. June changed those call sites too | no, under "June behaviour wins" (user's standing rule). Following June's renames (e.g. `..._Guided_Arrow`) is optional C++ work |
| 3 | 14 script names June changed (Raven Guardian Strike x4, VJavelin landing, Flame Sword memo, Rena Dive Kick Bombing x3, Aisha Blaze Step memo x2, PvP-hero Apple/Balak) | no: the effect is missing, no crash. Fixed by adopting June's character and NPC scripts (`JUNE2017_CHARACTER_PORT_CHECKLIST.md`) |
| 4 | 6 names dead in June too (`BOMB`, `ANCIENT_PPORU_BREATH`, ...) | no: parity |
| 5 | Lock-on runtime lines (`CE_LOCKON`, Phase 7b) | no: never observed; needs a real lock-on skill |
| 6 | 178 changed particles: visual check | no: optional |
| 7 | Retire `march2014/data036/EffectSet.src.lua` and `DamageEffect.src.lua` (the merged copies) | waits for the user's OK |
