# June effect completion - C4c - port June's NPC Fury mode

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C4c` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C4b (done 2026-10-07; its findings are below) |
| Flag | `SERV_IRUHADEV_JUNE_NPC_FURY_MODE` (new, in `KTDXLIB/Always.h`); diagnostics behind the shared `SERV_IRUHADEV_JUNE_FX_DEBUG` |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | `kickoffs/june-effect/C4c-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C4 - Review (2026-10-07, Opus 5.5 high)" "C4b - Done (2026-10-07, Opus 5.5 high; identification only, no code)"
grep -n "Effect C4b\|Effect C4 review" june2017_re/funcmap.tsv
```

`CX2GameUnit::GetFinalDamage` is also C10b-1's: do not run in parallel with it. Peers edit
`X2GameUnit.cpp` often: re-find every anchor by function name and bytes, not by line number.

## C4b findings (what to port, with June addresses)

Every item below is one funcmap row tagged `Effect C4b`. Cite the June address in a comment at each site.

1. **State flag and parse** (`X2GUNPC.h` / `X2GUNPC.cpp` `CX2GUNPC::LoadStateData` = June `32784c00`).
   - Append `FLAG_FURY_MODE` just before `FLAG_MAX`. June's bit is 80, but its enum does not line up with March's (June has 83 flags, March 76), so do not reuse 80.
   - `FURY_MODE`: a bool, default **false**, set with `NOWSTATE_SET_FLAG( FURY_MODE, bTemp )`. It goes right after `FALL_DOWN`, before `// attack box` (June `@3278a088-3278a145`).
   - `FURY_DATA` sub-table (June `@32796366-3279670f`). If `BeginTable( "FURY_DATA" )` succeeds, read `ATTACK_POWER_RATE_FURY` into the new `StateData::m_fAttackPowerRateFury` and `ANIM_SPEED_RATE_FURY` into `m_fAnimSpeedRateFury`, each defaulting to **1.0** on a missing key, then `EndTable()`.
     - Both members are also 1.0 in the `StateData` ctor (June `32393dc0`), so a state without the table keeps 1.0.
     - The keys are **only** read inside `FURY_DATA`, never at the top level.
     - Place the table near the end of `LoadStateData`, but **outside** any `#ifdef` block that could be off in `US_SERVICE`. The `CAMERA_CRASH` block sits inside `X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE`: check that.
2. **The Fury byte** (June NPC `+0x8fb7`). Add `bool m_bFuryMode`, set false in `CX2GUNPC::CommonConstructor` (June `32762670 @327633f2`).
   - `void SetFuryMode( bool )` (`31e4d7c0`) only stores the value. It does not call `ResetAnimSpeed`.
   - `bool GetFuryMode()` (`31e4d7d0`).
   - Bind both in `X2Main.cpp`'s `CX2GUNPC` `class_def` list as `"SetFuryMode"` / `"GetFuryMode"`, after `SetMonsterBuffGroupID` (June binds `@31e0ad15` / `@31e0ae4a`).
   - Nothing else writes the byte. It is never reset on Init, ReInit, state change or death.
3. **`GetFinalDamage` multiply** (`32284aa0 @32285757-322857e8`).
   - When the attacker is a `CX2GUNPC` (unit type `GUT_NPC`, then `dynamic_cast`), `m_bFuryMode` is set and the attacker's `GetNowStateData()` has `FURY_MODE`:
     - multiply the physic damage local by `m_fAttackPowerRateFury` **if that local != 0**;
     - do the same for the magic local.
   - The rate itself is never tested. Place this after the two locals are loaded and **before** C4's `JuneApplyDivideDamageFromRange` call.
4. **June's whole NPC `ResetAnimSpeed`** (`327615d0`). No partial update: port all of it.
   - `v = IsSamef( m_fTimedEventAnimSpeed ) ? m_fAnimSpeed : m_fTimedEventAnimSpeed`.
   - **Fury:** if `m_bFuryMode` and `NOWSTATE_FLAG( FURY_MODE )`, take `r = m_fAnimSpeedRateFury`. If `r != 0.f`, set **`v = r`**. This is a **replacement, not `*=`**: the state's own speed is dropped.
   - Then, in this order:
     - `v *= m_fBornAnimSpeedRate`;
     - `v *= GetAnimSpeedValue()`;
     - `v *= m_fRoseSummonAnimSpeedRate` (June `+0x8b6c`, new member, 1.0 in `CommonConstructor`; no March writer, the Rose writer is checklist 7f);
     - `v *= m_fMultiplicationAnimSpeed` (June `+0x8fdc`, new member, 1.0 in `CommonConstructor`; its June writer is the Hero-dungeon `CreateGUNPC` tail `327ef570`, which is **not** ported because March has no hero dungeons);
     - `if bComeBackState: v *= 4.0f`;
     - `SetPlaySpeed( v )`.
   - The member names are suggestions: name them in March style.
   - June's body **does not** call `m_AdditionalAnimSpeed.UpdateResultValue()` at entry (March's does). Drop it, because item 5 replaces it.
5. **Make `ResetAnimSpeed` virtual and call it from `CheckAndUpdateAdditionalStat`** (June slot `0x3f8`, base `31f3f070` = empty).
   - Add `virtual void ResetAnimSpeed() {}` to `CX2GameUnit`. The NPC's existing `ResetAnimSpeed()` then overrides it.
   - `CX2GUUser::ResetAnimSpeed( float, bool, bool, float )` hides the name in the user class. That is fine: June's user slot is empty too. Check it compiles, and do not make users call the base.
   - In `CX2GameUnit::CheckAndUpdateAdditionalStat` (June `32290920 @322909a2-322909fd`), replace the plain anim line with: `if ( m_AdditionalAnimSpeed.GetMustChangeResultValue() ) { m_AdditionalAnimSpeed.CheckAndUpdateResultValueWithOptionData( ...same args... ); ResetAnimSpeed(); }`.
   - June's following slot `0x3fc(1)` call is a no-op in every unit class (`3208ddc0` = `ret 4`). Skip it with a comment.
   - March's three direct callers stay as they are: `CommonStateStart`, and `CommonFrameMove_TimedEvent` twice.
6. **JUNE_BUGS rows** in the same commit:
   - The Fury anim rate replaces the base speed instead of multiplying it.
   - An anim rate of 0 is skipped, but the attack rate is never tested, so `ATTACK_POWER_RATE_FURY = 0` zeroes the damage.
   - A NaN rate is stored or multiplied.
   - `SetFuryMode` does not refresh the speed until the next `ResetAnimSpeed`.
   - The byte survives every state, death and ReInit.
7. **Census / key matrix / MODS:**
   - `FURY_MODE`, `FURY_DATA`, `ATTACK_POWER_RATE_FURY` and `ANIM_SPEED_RATE_FURY` become `implemented` (if the census tracks NPC state keys; if not, say so).
   - Add a MODS row for the flag.

**Not ported, and why:**
- The Rose writer (`32ab0560`) is checklist 7f.
- The Hero-dungeon table, its Lua bind `Add_HeroDungeon_ExtraStatForException` and the `CreateGUNPC` tail: March has no hero dungeons.
- June's `IS_APPLY_CINEMATIC_UI` and `DAMAGE_RATE_OF_PART` neighbours belong to other systems and are not Fury.

## Task

Port items 1-7 under `SERV_IRUHADEV_JUNE_NPC_FURY_MODE`, with the original code in `#else` wherever behaviour is replaced (items 4 and 5). Keep June's quirks verbatim.

**Diagnostics** go behind `SERV_IRUHADEV_JUNE_FX_DEBUG`, as `CX2OfflineLog::Server` / `ErrorLogMsg` lines:
- `[JUNE_FX] FURY set npc=<uid> on=<0/1>` in `SetFuryMode`;
- `[JUNE_FX] FURY dmg rate=<r> phys <a>-><b> magic <c>-><d>`, throttled to about 1/s;
- `[JUNE_FX] FURY anim state=<id> v=<v>` when the Fury branch replaces the speed.

**Play-test data.** Nothing in March reaches Fury: only five June raid scripts use the keys, and none is in March. So stage a test NPC in its `kom_contents/dataNNN_kom/` script, then pack it and ask before the copy (CLAUDE.md):
- pick one common monster from the save's usual first dungeon;
- give its main attack state `FURY_MODE = TRUE` and `FURY_DATA = { ATTACK_POWER_RATE_FURY = 2.0, ANIM_SPEED_RATE_FURY = 2.0 }`;
- give its spawn or wait state's `STATE_START` function a `pNPC:SetFuryMode( true )` (the Lua variable name follows that script).

Keep the original values in comments so the staging can be reverted.

**Hand-off checks.**
- The monster's attack animates visibly faster.
- The damage taken from that attack roughly doubles, compared with a run on the previous exe or the unstaged script.
- The `[JUNE_FX] FURY` lines appear.
- A regression run on an unstaged dungeon shows no `FURY` lines and no anim-speed change. This catches a broken `CheckAndUpdateAdditionalStat` anim update, which would show up as buff slows or haste no longer applying to NPCs.
