# June character Chung (Stage 7) - Phase CH3c2 - Targeting NPC camera does not follow the aim (Heavy Field, Missile Shower)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH3c2` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3c done (commits `86545ce0`, `04cfc94a` and the camera-diagnostic commit). One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`, the camera lines) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL camera`) |
| Review | `kickoffs/char-chung/CH3c2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH3c - Done (2026-11-03)"
```
Code: `X2BattleFieldGame.cpp` `UpdateUnitPointerNPCWalk` (~1162), `X2DungeonGame.cpp` `UpdateUnitPointerNPCWalk` (~2537), `X2Game.cpp` `CX2Game::UpdateUnitPointer` (~2472, the timer focus), `X2GUNPC.cpp` `CommonCameraMove` (~22767, `NomalTrackingCamera` default) and `X2GUChung.cpp` `LogTargetingNpcFocus`, `CSI_SA_CTT_HEAVY_FIELD_LOOP_FrameMove`, `ArtilleryStrikeControl_EventProcess`.

## Task

**Symptom (user play-test, 2026-11-03, unit 18 Freils, battlefield 40001).** Holding C (Heavy Field) or R (Missile Shower) and steering the targeting NPC: the camera sometimes does not follow it, mostly on the first casts of a session.

**What CH3c's logs showed** (`python tools/logs.py --grep "CHUNG SKILL camera"`, exe md5 `e7346915`, lines 10:41-10:42): the NPC always had `FocusCameraForce` = 1 and the camera flag = 1, but in the first two Heavy Field casts the game's focus unit stayed my unit (type 0) for the whole loop (3.57 s, 2.17 s); from the third cast on the focus was the NPC at once. Missile Shower: the first sample of each cast is unfocused, then focused. The camera is therefore not lost by a flag reset: the NPC walk does not select the NPC (or does not run) at those times.

**Round 3 result (exe md5 `04afffb6`, 10:57 logs, battlefield, game type 3): lead found, to be proven by this phase.** All 22 samples `npc focused 0`; the NPC is in the list (`list index 35 of 80`), so the walk reaches it. Heavy Field shows Force 1 (my loop sets it every frame) and Missile Shower shows **Force 0** after its first sample (its start sets it once): something clears `FocusCameraForce` and hands the focus to my unit. The only such code is `CX2BattleFieldGame::UpdateUnitPointerNPCWalk` @1223-1229 (`SERV_BATTLEFIELD_MIDDLE_BOSS`): `if ( pNPC->GetIsMainBoss() && m_pCinematicUI->GetIsCinematicUIActive() ) { SetFocusCameraForce( false ); m_optrFocusUnit = GetMyUnit(); }` with no `return`. `CinematicUI::GetIsCinematicUIActive()` is `!m_bStart && !m_bEnd`, true whenever no fade runs. `CX2GUNPC::m_bIsMainBoss` (`X2GUNPC.h` ~4653) has **no initializer**: the constructor (`X2GUNPC.cpp` ~2420-2430) never writes it; only `X2BattleFieldGame.cpp:998` (field monsters) and `X2GUNPC.cpp:15322/15332` do. A summoned NPC (CreateNPC, `X2Game.cpp` ~8063) therefore carries heap garbage: non-zero = the walk strips the camera every frame, zero = it works. That is why it varied by cast and session. Prove it (log `GetIsMainBoss()` in `LogTargetingNpcFocus`, or initialise and re-test), then fix in the NPC constructor behind the flag (June's NPC constructor: check it zeroes the byte, Ghidra `CX2GUNPC` ctor), and re-check Missile Shower / Quantum Ballista in the dungeon walk too. The same bug hits every summoned NPC that takes the camera in a field (Sniper / Proton Bomb event NPCs).

**Do:** read the next round of `camera` lines (build with the extended diagnostic: NPC list index and list size, start state, game type; one line per 0.4 s) from a fresh session: first Heavy Field cast, steered. Then decide from the data:
- list index `-1` while the NPC acts: the NPC is not in `m_NPCUnitList` yet (background load / `CreateNPCData` queue in `CX2Game::CreateNPC`, `X2Game.cpp` ~8063); fix where it is added or make the walk not depend on it;
- index valid, Force 1, still unfocused: the walk is skipped (`CX2Game::UpdateUnitPointer` returns true while a timer focus on my unit lasts) or another path resets the focus after it (`ResetFocusUnit`, `InitFocusUnit` from a StateEnd, the `SERV_BATTLEFIELD_MIDDLE_BOSS` clause); find it with a log in the walk;
- focused but the camera still stays: `NomalTrackingCamera` / `m_bNpcTrackingCamera` / the camera's own limits (the NPC `CommonCameraMove` default).
June's behaviour wins: compare with June `UpdateUnitPointer` (`322d5f70`, `32d530a0` battlefield, `327cea70` dungeon) before changing March's walk; a June quirk is kept verbatim with a `JUNE_BUGS.md` row. The fix is June's body inside March's function under the flag, March in `#else`.

**Done gate:** from a fresh session the first Heavy Field and Missile Shower casts log `npc focused 1` from the first sample, and the user sees the camera follow. Retire `_SKILL_DEBUG` in the review. Also in this build's tail: the deployed exe of CH3c still has the Mobile Armored Mode block that was moved to CH4a, so this phase's build drops it (`mobile armored mode on` disappears; the orange balls are CH4a's).

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
