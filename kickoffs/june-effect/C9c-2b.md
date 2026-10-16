# June effect completion - C9c-2b - the unit-viewer effect player, hooks, templets, hard-code retirement

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9c-2b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C9c-1 (its table lookup and load) and C9c-2a (done 2026-10-07) |
| Flag | new `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER` (needs C9c-1's `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_TABLE`) |
| Log tag | `JUNE_FX` in ErrorLog.txt (the C9c lines are unbracketed: read them with `grep JUNE_FX ErrorLog.txt`, as C9c-1 found) |
| Review | `kickoffs/june-effect/C9c-2b-review.md` (the phase writes it when done; it also reviews C9c-2a, which had no review of its own) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9c-2a - Done" "C9b - Done" "C9c-1 - Done"
```

Also read:
- `june2017_re/funcmap.tsv`: every row tagged `Effect C9c-2a` (`grep "Effect C9c-2a" june2017_re/funcmap.tsv`), plus `31fa3010`,
  `31fa8ce0`, `32959640`, `329594c0`, `329585f0`, `321dfc40`, `3264ca10`;
- `june2017_re/c9c2a_hardcode_map.tsv` (every March site, its table row and the verdict: retire / KEEP / stay);
- March `X2Lib/X2BaseUnitViewerUI.h/.cpp`, `X2Lib/X2UnitViewerUI.h` (class head :1-60, handle members :260-395) and the
  `X2UnitViewerUI.cpp` lines the `.tsv` names;
- `X2Lib/X2EffectSet.cpp:302-400` (`JuneRunMeshTriggers`) and `:130-220` (`JuneOnFrameMoveSound`), the game-side versions to adapt;
- `june_src_fixed/data036_kom/UIEffectSet.src.lua` at the 15 `NAME` lines the `.tsv` gives.

## Task

One phase, so nothing regresses between the player arriving and the hard-code leaving. Everything goes behind the new flag; the original
code stays reachable in `#else`.

1. **Where the state lives: `CX2BaseUnitViewerUI`** (decided in C9c-2a). June has a real base class (RTTI, vtable `3347113c`), shared with
   `CX2NPCUnitViewerUI`.
   - Re-parent `CX2UnitViewerUI` insert-only: `#ifdef <flag>` + `class CX2UnitViewerUI : public CX2BaseUnitViewerUI` + `#else`, then the
     whole existing `MONSTER_TROPHY` block untouched, then `#endif`.
   - Give the base June's members: the 0x40 entry (`32959640 @detail`), the four vectors (`31fae120`), the `+0x190` show byte, the `+0x1d4`
     land-dirty byte, and the two trigger codes `+0x1d8` / `+0x1dc`.
   - Port these functions:
     - Play `32959640`, create pass `32957980`, per-entry update `329585f0 @detail`, sound pass `329582b0`;
     - stop group `329594c0` and stop sounds `32959250`;
     - land position `32957170`, plus the lowest-bone helper `31d2b080`, which has no March counterpart;
     - the overrides `OnFrameMove` (= `329585f0`), `OnFrameRender_Prepare` (`32957260`), `OnFrameRender_Draw` (`32957330`) and
       `NotifyShowObjectChanged` (`32957760`);
     - ctor `31fae120` and dtor `31fae1a0`: the dtor stops group 1 and then every sound (group 5).
   - **Draw** (`32957330`) is March's own fixed-mode pattern, applied to every live entry whose `GetShowObject()` is true:
     `CKTDGStateManager::PushStates( GetRenderStateID() )`, then `OnFrameRender_Draw()`. Meshes come first, then particles.
     - A particle also draws its particles' child sequences (`31cf4890`). Find March's equivalent, or record it.
     - Who draws what:
       - March chains every mesh and every particle into the DG manager at creation.
       - June un-chains only the particles of an unmanaged viewer (`!IsManagedByDGManager()` -> `RemoveObjectChain`).
       - So a managed viewer's shown effects are reached by both the DG manager and this draw. Meshes are reached by both in either case,
         as March's `m_bFixed` meshes already are.
     - Keep June's behaviour. If the play-test shows doubled effects, add a JUNE_BUGS row.
   - `CX2NPCUnitViewerUI` overrides `OnFrameMove`, `OnFrameRender_Prepare` and `OnFrameRender_Draw` but not `NotifyShowObjectChanged`.
     That one becomes the base's, which is a no-op on its empty lists; June's NPC vtable inherits it the same way. Nothing else changes for
     it. Say so in the Done note.
2. **The viewer trigger variant.**
   - March's `JuneRunMeshTriggers` dereferences the set on its first line, so add a no-set variant for `321dfc40 @viewer`. It takes the
     skin anim, the two viewer codes (folded with March's `JuneFoldTriggerEmotion`, first non-zero wins) and ANIMTIME_ON_START_UNITVIEWER.
     The trigger struct already has `m_AnimTimeOnStartUnitViewer`.
   - STOP_TRACING is skipped without a set, as June does.
   - **User decision 2026-10-07:** guard EFFECT_SET_TOGGLE_VISIBILITY_TIMER. June writes it through the null set (`@321dff8f`). Skip it,
     log it once under `JUNE_FX`, and add a `JUNE_BUGS.md` row.
   - Codes: `+0x1d8` = `m_ePlayedEmotion` (candidate, `viewer+0x1d8` row); `+0x1dc` = 0. June's motion types 7 and 8 do not exist in March.
3. **`CX2UnitViewerUI` hooks, in June's order.**
   - `PlayAnim` (`31fa3010`): after ChangeAnim/Play, clear the request vector and stop group 0. Then call C9c-1's lookup `GetUnitViewerEffectSet` with
     ( name, class, duo 0 ); on a hit push the request { set, UNIT_TYPE, UNIT_CLASS, start time, loop }.
   - `OnFrameMove` (`31fa8ce0`):
     - copy the two codes, then call the base update at entry (`@31fa8d4c`);
     - re-arm: when the anim time is exactly 0.0, look up the current anim name and push it again if `loop` (`@31fa9271`);
     - drain: start 0 or start <= anim time -> Play( skin, name, 0 ), then erase (`@31fa93f6`);
     - call the base update **again** after the equip show loop (`@31fa9639`). June runs it twice per frame; keep both.
   - `NotifyShowObjectChanged` calls the base one first (`31f96ad0 @31f96afd`). The Prepare and Draw bodies call the base ones as June
     does (`31f974b0` first, `31fa00b0 @31fa2ef7` last).
   - The dtor stops groups 0..4 (`31fab920 @31fabc53`).
   - `31f99f70`: set the land-dirty byte where `SetPositionOnScr` moves the matrices (`:6191-6196`).
4. **Templets.**
   - `kom.py status data036` showed the staged `UIEffectSet.lua` in sync with the live kom on 2026-10-07. Re-check that, since C9c-1
     packs data036 too.
   - Decompile it with `elstools decompile-batch` **in the scratchpad**. C9c-2a proved the mar-2014 round trip clean: same bytecode size,
     only table-key order differs, 52 sets.
   - Copy the `.src.lua` beside it, never the `.std.luac`: the packer refuses unlisted files.
   - Append the 15 June templets verbatim, then `kom.py compile data036`, `/elstools pack data036`, and ask before `/x2-kom-packed copy`.
   - `EffectSet_SBH_*` exist in March only in the game `EffectSet.lua`; append June's to the UI file anyway.
   - **User decision 2026-10-07: adopt June's whole `UIMajorParticle.txt`** (data036), from `14JUN17\data\data036_kom\`.
     - March drove six of these particles from code; under June's player they need June's own values:
       - `MagicKnightHandFire` has no emit rate in March (June: 50);
       - `SBH_LobbyWait01-03` and `ADD_LP_LobbyWait01-02` never stop in March (trigger time 99999/9999; June: 0.7/0.4/0.9).
     - C9c-2a checked June's file: 2,042,192 B, 2719 sequences, CRLF, braces balanced (2720/2720), ends `}\r\n`. So it is not the
       truncated-extract case, but compare its size with the June kom entry before trusting it.
     - **10 March sequences are not in June's file:** `CHUNG_HOWLING01/02`, `CHUNG_STEEL_EDGE02`, `Nasod_King_SpellAttack_Splitlight06`,
       `PET_PETTE_BERTHE_STEB_3_Attack_C(_02)`, `Pulsewave_Summon_Boss_SpecialAttackB01/02`, `ShockWave_Summon_Boss_SpecialAttackB`,
       `UI_Eve_Party_Event_StartEffect`. Append March's blocks for them verbatim, so no name March reads goes missing, and list them in
       the Done note.
     - The manifest algo stays as it is. Pack, and ask before copy.
5. **Retire the hard-code** under the flag, site by site from the `.tsv`, with `#else` intact.
   - **User decision 2026-10-07: KEEP the ALD orb** (`m_hSeqEnergyBullet1-3`, `SERV_ARA_CHANGE_CLASS_SECOND`).
   - The reason: June's row is keyed `ALD_LobbyWait`, but March's motion file and calls say `ALD_lobbyWait`, so the row never matches here.
   - Stage its templet anyway (inert), and add a `JUNE2017_CHARACTER_PORT_CHECKLIST.md` §7 row: the Ara port adopts June's
     `Motion_ARA_Common.x` and the spelling together, then retires these three handles.
   - Only the `m_bShow*` writes and the handles go behind the flag. `SetHideSelectUnitWeapon`'s mesh cases become no-ops with the handles
     gone: June's counterpart is unidentified, so check it in Ghidra (callers of the June UICharInfo / UIWatch equivalents) or record it.
6. **JUNE_BUGS rows** (verbatim, inert on the 15 sets):
   - TRACE_MATRIX entries add POSITION_OFFSET to their own position every frame (cumulative);
   - the sound pass has no null test on its skin anim;
   - the dead mesh-to-mesh link store (`@32957bfc`);
   - only one land particle is refreshed per dirty flag;
   - `+0x190` is set by Play and never cleared;
   - TRACE is not read, so every entry follows its bone;
   - the exact-0.0 re-arm (C9b check).

   Add only the ones that are real in the code you port.

**Done gate:**
- the strings are in the deployed exe;
- a load or first-play line, `JUNE_FX UNIT_VIEWER_FX_PLAY <set> group=0 meshes=<n> particles=<n>`, with a `_DEBUG` flag for per-frame lines;
- the hand-off lists the 11 live rows by class, for the user to look at in character select or the lobby: MagicKnight, Pyro Knight wait,
  Blazing Heart, Lunatic Psyker, Deadly Chaser, Sheath Knight, Infinity Sword, Trapping Ranger and Night Watcher ready actions. Say which
  of these the save has (`els_db.sql` unit classes; add a character with `/x2-create-char` if needed);
- the Little Devil orb still shows exactly as before;
- 0 EXCEPTION.

**When writing `C9c-2b-review.md`:** C9c-2a had no review of its own, so the review kickoff must also re-verify C9c-2a in Ghidra, not just
check this phase's transcription. Have it check:
- every funcmap row tagged `Effect C9c-2a`, starting with the `candidate` rows (`viewer+0x1d8` = `m_ePlayedEmotion`, `31f99f70`,
  `31fa4980`);
- the verdicts in `june2017_re/c9c2a_hardcode_map.tsv`, including the 11-live-rows count (`SPK_LobbyReadyAction` has no March motion;
  `ALD_LobbyWait` vs `ALD_lobbyWait`);
- the draw ownership claim (a managed viewer's effects are reached by both the DG manager and `32957330`).
