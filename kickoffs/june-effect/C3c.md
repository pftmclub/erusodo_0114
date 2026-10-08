# June effect completion - C3c - port the super-armour condition, DamageReactStateChange's gates and ADD_MEMBER_COUNT's reader

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C3c` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3b (done 2026-10-08). Not in parallel with C9h-4f or C8b: same `X2GUUser.cpp` areas (CommonStateStart, DamageReact) |
| Flag | new `SERV_IRUHADEV_JUNE_SUPER_ARMOR_CONDITION` (items 1-2); extend `SERV_IRUHADEV_JUNE_DAMAGEDATA` (item 3). C3b's kickoff said only `_DAMAGEDATA`; the parse + check + 22-site rewrite is a logical change of its own |
| Log tag | `JUNE_FX C3c` (`python tools/logs.py --grep "JUNE_FX C3c"`) |
| Review | `kickoffs/june-effect/C3c-review.md` (write it when done) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3b - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build)" "C3c - port: super-armour condition, DamageReactStateChange's gates, ADD_MEMBER_COUNT"
grep -n "(C3b)" june2017_re/funcmap.tsv
cat june2017_re/c3b_damage_react_sites.tsv
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context"
```

The rebuild plan's Starting context predates `tools/`. Ghidra is read-only (`ghidra` skill). `X2GUUser.cpp`, `X2GameUnit.cpp` and
`X2DamageManager.cpp` are CP949: `tools/bytepatch.py` only.

## Task

Every item below is a funcmap row tagged `(C3b)`. Cite June addresses in every comment, keep March's code in `#else`, and name new
members after the key in March's style. June's StateChange constants are June enum values. They differ from March's
by a varying offset (`USI_DAMAGE_SMALL_FRONT` June 0x1a / March 0x19, `USI_RIDING_DAMAGE_FRONT` June 0x55 / March 0x50), so never
compute one: map each **by name** (`june_src_fixed/data036_kom/Enum.src.lua`).

1. **Super-armour condition** (rows `32057b80 @320588e5-32058d38 (C3b)`, `31fd86b0 (C3b)`).
   - **Parse.** In `CX2GUUser::CommonStateStart` (`X2GUUser.cpp:14857`), keep June's order: `SUPER_ARMOR`, `SUPER_ARMOR_FRONT`, clear
     the set, the `SUPER_ARMOR_CONDITION` int list from index 1, then FRONT (sets `bSuperArmor`, inserts `SAC_FRONT` 0), then
     `SUPER_ARMOR_NOT_RED`.
   - **The set's home.** It is per state, next to `stateParam.bSuperArmor`. Check how March copies `stateParam` between the Now and
     Future frames before choosing where the set lives.
   - **`m_bStateSuperArmor`.** Check whether June's counterpart takes FRONT's `true` (read the stores after SUPER_ARMOR_NOT_RED
     `@32058d38..`).
   - **The check.** `bool JuneIsSuperArmorAgainst( bool bAttackerFront_, const CX2DamageManager::DamageData* pDamageData_ ) const` on
     `CX2GUUser` = `31fd86b0`. It is virtual in June (vslot `0x81c`), but no class overrides it; Add's caller is class code (checklist 7l).
2. **DamageReactStateChange** (`X2GUUser.cpp:12066`, June `31feabd0`, rows `31feabd0 (C3b)` and the tsv).
   - **The 22 sites.** Replace each case-top `IsSuperArmor()` with June's 22 `0x81c` calls and 6 `m_bForceBackSpeed` tests, case by
     case as in the tsv, including the FORCE block `31feb28f` (`nowSpeed.y = backSpeed.y`, no state change) and the post-speed
     breaks in UP / FLY / DRAG_UP.
   - **FLY's tutorial block.** Buff 14c's block at `X2GUUser.cpp:12421-12430` (`JUNE_TUTORIAL_MODE`) gains June's `&& !FORCE` term, and
     its comment ("+0x3ec ... reads as false") is corrected. Both flags must compile on and off (`flagcheck --with`).
   - **DOWN.** June has no `SetNowForceDown( 0 )` (March `:12359`, `FIX_FORCE_DOWN_INITIALIZE_BUG` ON). First identify June's force-down
     member and **all its writers** (`dispsweep.py`; March's `SetNowForceDown` body names the member). If June moved the reset elsewhere
     (state start, landing), port it there. Only if it is gone, follow June: drop it under the flag and add a JUNE_BUGS row (June
     removed a March fix). **This is the one live change.**
   - **Left out.** The pre-switch `+0x9175` block and the RT_BOUNDING case (C3d-3) are not ported.
3. **ADD_MEMBER_COUNT reader** (rows `32034e50 @3203681f-32036c81 (C3b)`, `32294e30 (C3b)`, `322a8240 (C3b)`).
   - **Where.** `CX2GUUser::Process_ExtraDamage`, case `EDT_TRANSIT_DEBUFF` (`X2GUUser.cpp:11634`). After March's
     `ProcessTransitDebuff` call, port the member loop.
   - **The collector.** Transcribe `32294e30` whole as a `CX2GameUnit` method. Settle its argument order from the call
     `@32036bee-32036c11`, and check each skip test against March names.
   - **Polarities.** Settle the distance test `@3203686d` (`<= 0` or NaN -> 1000.0) and the collector's x87 tests with
     `june2017_re/x87test.py`.
   - **ProcessTransitDebuff.** Diff `322a8240` against `X2GameUnit.cpp:12328`. June applies the factors one by one (`322a6d00`, buff 9c row
     1247) and lets a target with no gage through. If that differs from March's vector call in behaviour, port June's form (no partial
     updates).
   - **JUNE_BUGS.** The reader always uses the DD's main `m_ExtraDamage` count, whichever ED is processed.
   - **The NPC case stays March's** (June's twin is the same).
4. **Logs.**
   - Under the flags, drop the C3 rule-5 lines for `ADD_MEMBER_COUNT` / `_DISTANCE` (`X2DamageManager.cpp` ~494-500), because they are
     consumed now.
   - Change BOUNDING's system text (~311) to `bounding user states (C3d-3)`.
   - Add `JUNE_FX C3c` once-per-session lines on the TRANSIT member loop and on the first SAC filter that changes a result.
5. **Census.** Run `june2017_re/census.py`; the `ADD_MEMBER_COUNT*` and `FORCE_BACK_SPEED` rows leave "parsed only"
   (`census_completion.KEY_NOTES`).

Not ported, by C3b's verdicts: `31fcee40`'s counter (no reader in June), the bounding state (C3d-1..3, D10 (a)), and the class rows (checklist 7l).

**Done means:**
- flagcheck PASS for both flags;
- the new strings are in the deployed exe;
- a regression play-test: get knocked down (DOWN), launched (UP / FLY) and dragged up in a dungeon. Look for force-down / detonation
  after DOWN, which is the live change;
- 0 EXCEPTION;
- STATUS row updated; `C3c-review.md` written.

Stop and ask the user only if copying June would break March.
