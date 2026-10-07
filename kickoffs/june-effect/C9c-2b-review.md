# June effect completion - C9c-2b review (also re-verifies C9c-2a)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9c-2b review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9c-2b done (its Done note, commits and play-test) |
| Flag | `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER` (with `SERV_IRUHADEV_JUNE_FX_DEBUG` for the play and create lines) |
| Log tag | `JUNE_FX` in ErrorLog.txt; the C9c lines are unbracketed, so read them with `grep JUNE_FX ErrorLog.txt`, not `logs.py --tag` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9c-2b - Done" "C9c-2a - Done"
```

Also read: `june2017_re/funcmap.tsv` rows tagged `Effect C9c-2a` and `Effect C9b` (`grep "Effect C9" june2017_re/funcmap.tsv`),
`june2017_re/c9c2a_hardcode_map.tsv`, and the phase's commits (`git log --oneline --grep="C9c-2b"`).

## Task 1: C9c-2a had no review of its own, so re-verify it in Ghidra

Check every funcmap row tagged `Effect C9c-2a`, **starting with the three `candidate` rows**:
- `viewer+0x1d8` = `m_ePlayedEmotion` (written by `31fa8ce0 @31fa8d46`; +0xe5c is zeroed by `PlayByMotionType` `31fa5c90 @31fa5cd1`).
  C9c-2b copies `m_ePlayedEmotion` into it each frame, so a wrong identity changes which trigger code the (empty) trigger lists see.
- `31f99f70` = the viewer's move, which sets the land-dirty byte `+0x1d4` (C9c-2b sets it in `SetPositionOnScr`, `X2UnitViewerUI.cpp`).
- `31fa4980` = enchant / weapon-effect reset (no code was ported from it).

Then the verdicts in `june2017_re/c9c2a_hardcode_map.tsv`:
- the **11 live rows** count (`SPK_LobbyReadyAction` has no March motion; `ALD_LobbyWait` vs March's `ALD_lobbyWait`: confirm both in the
  motion files, `Motion_EL_Lobby.x` and `Motion_ARA_Common.x`);
- the **draw ownership** claim: a managed viewer's shown effects are reached by both the DG manager and `32957330`. If the play-test shows
  doubled effects, the Done note says so; add a JUNE_BUGS row.

## Task 2: C9c-2b against the binary

- **Player**: Play `32959640` (entries, sound list, the `+0x190` write), Stop `329594c0` / `32959250` (group 5 = all), create pass `32957980`
  (START_TIME gate, position, ROTATE, SCALE state test, un-chaining of an unmanaged viewer's particles, the trace-target links), update
  `329585f0 @detail` (three trace branches, FOLLOW_UNIT_VIEWER_SCALE, show), sound pass `329582b0`, Prepare `32957260`, Draw `32957330`,
  Notify `32957760`, ctor `31fae120`, dtor `31fae1a0`. Say whether the port's deliberate differences hold: a missing set is logged and skipped;
  a failed create is retried every frame (as June) with the log written once; the sound pass is skipped without a skin.
- **Hooks**: `PlayAnim` `31fa3010` (clear, stop group 0, lookup, push), `OnFrameMove` `31fa8ce0` (the code copy and the first base update at
  `@31fa8d4c`, the re-arm `@31fa9271`, the drain `@31fa93f6`, the second base update `@31fa9639`). The port puts the first update before
  the skin frame move and the second after the equip loop: check that against the addresses.
- **Trigger variant** `321dfc40 @viewer`: first non-zero folded code, ANIMTIME_ON_START_UNITVIEWER, the skipped STOP_TRACING, the guarded
  toggle (JUNE_BUGS row 86).
- **Retired sites**: confirm in the diff that only creation and the `m_bShow*` / crescent-cut writes went behind the flag, and that every other
  site of a retired handle tests the handle first (so it is a no-op). The Little Devil orb must be untouched.
- **`SetHideSelectUnitWeapon`**: the port records no counterpart (its four callers run right after a unit is set, in a wait motion, when June
  creates nothing at `SetUnit`). Ghidra was not reachable in C9c-2b: check June's callers and say whether anything else hides these meshes.
- **Data**: `UIEffectSet.lua` (March's 52 sets + 15 June blocks, 37,072 B compiled) and `UIMajorParticle.txt` (June's file + the 10 March-only
  sequences before the last brace). Check the play-test log for particle parse errors: `CHUNG_HOWLING01` is the last sequence in the file, so
  if it resolves nothing before it was dropped. The orb's `SA_AYR_ORB_KI_Main_S_P01` lost March's `Zenable = false` line (June has none).
- JUNE_BUGS rows 79-86: confirm each is real in the code that was ported; row 85 (the re-arm) is "not counted" until the log shows two
  `UNIT_VIEWER_FX_PLAY EffectSet_SBH_LobbyWait` lines back to back.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER --with JUNE_FX_DEBUG --range <first C9c-2b commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines and explain every OTHER row and deleted line.

Spot-check 3 sites in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then
`tools/build.py --deploy`); hand design decisions back to the user. Write "C9c-2b - Review" under the phase in the plan, update the STATUS.md
rows, write or check the next kickoff (C10b-3 / C11 are listed in STATUS), and commit. Never launch X2_offline.exe; ask the user.
