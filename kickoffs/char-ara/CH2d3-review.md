# June character Ara (Stage 7) - Phase CH2d3 review (the CH2 stage review: covers CH2a-CH2d3, incl. CH2dx)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH2d3 review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH2d3 done (its Done note, commit and the user's play-test of units 137 / 138) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SKILL` + `..._SKILL_DEBUG` (the review retires the debug define) |
| Log tag | `[JUNE_CHAR]` (`ARA SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH2a - Done" "CH2b - Done" "CH2b2 - Done" "CH2c - Done" "CH2c2 - Done" "CH2d - Done" "CH2dx - Done" "CH2d2 - Done" "CH2d3 - Done"
```

CH2b0 and CH2c0 have their own reviews (not covered again). Have a subagent do the reading and the mechanical runs (flagcheck per commit, junediverge, logs); you keep the Ghidra spot-checks and the verdict.

## Task

Review the whole CH2 stage against June from the exe bytes (Ghidra read-only). Commits: CH2a `46ad8778`, CH2b `d30d489f`, CH2b2 `a1ede78a`, CH2c `0d8a94c7`, CH2c2 `c9aee9ed`, CH2d `24c5badf`, CH2dx `fc331897`, CH2d2 `2a38867d`, CH2d3 (the commit named in its Done note).
- **Spot-check at least 6 functions across the stage**, at least two from CH2d3: View Between FrameMove `32bbcdc0` (65 / 220 offsets, the `IsRight` sign, power rate and `ScaleDamageByRate( +0x9758 )`), Shake Chaos FrameMove `32bc3260` (two timers 0.135 / 0.309, the first-run spend, the 0.5 repeat factor) and Mana Drain FrameMove / End `32bc3bc0` / `32bc3e40` (0.3 s effect handle, 0.65 s give-back). If one fails, re-check the whole phase it came from.
- **CH2d3 specifics.** Registration (both switches): `32bd09d0` cases 27214 / 27225 (helper `32bce810` x2 + finish 0x121), 27215 / 27239 (`32bce8c0` x2, repeat 0x14d `32bce920`, finish 0x124 `32bc9450`), 27216 / 27240 (`32bce9c0` x2), 27217 / 27241 (table by `31fceab0( 27420 )`, Init on the normal state only); `32bd34c0` @`32bd4b9a`-@`32bd4ddb`. The repeat state (`ASI_A_ALS_SHAKE_CHAOS_REPEAT`, June id 333, unnamed in June) was appended at the end of `ARA_STATE_ID`: check `luabind.py check Ara_Martial_Artist` STATE stays 0 wrong. The stub claim ("`31fca9e0` never holds a memo": the View Between `_MEMO_` effect name, the 0.15 rate lift, Shake Chaos' repeat) is the same one CH2d made for memo 0x9433c: re-read the stub's body once and say so.
- **Base Storm 27021** (owed since CH2dx): unit 137 `LSpecBase45` (lv 45, no ACQ upgrade yet) slot A0: `state <slot> -> 305-equivalent finish -> landing`, no `BLANK`. A failure is CH2d's case (`ASI_SA_ALS_SPECTER_OF_STORM_*`), fixed here.
- **Unit 138 was not played** (play-test 2026-11-12 covered unit 137 only: Storm, View Between, Shake Chaos, Mana Drain, Quick Piercing all clean). Have the user cast 27225 / 27239 / 27240 / 27241 on unit 138 (second-switch registration, chain lookup) and the View Between -> Shake Chaos -> Raid chain (`chain refused`, 0 lines so far) before `_SKILL_DEBUG` is retired.
- **Retire `_SKILL_DEBUG`**: comment out its `//#define` block in `KTDXLIB/Always.h` (guarded code stays), per the standing rule start the build and hand off without awaiting it (the Done note says "not awaited"). The stand-in `IsStateRegistered` guard in `CanUseSpecialAttack` stays while CH3's chain states are unported (Raid, Get Blood, Smash, Low Brandish ...: the View Between -> Shake Chaos -> Raid chain refuses at Raid).
- **Hygiene:** `flagcheck.py JUNE_CHAR_ARA_SKILL --with ..._SKILL_DEBUG --range <c>^..<c>` once per commit above (a range to HEAD times out), `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUAra.cpp`, `skillicons.py check` over the class-34 / 7 / 28 / 31 ids of the stage. Paste the `files:` / `RESULT:` lines.
- **Play-test reading** (`python tools/logs.py --grep "ARA SKILL state"`): every state id entered by the CH2 skills has a callback (no `BLANK`), 0 `EXCEPTION`, 0 `KEM_ERROR` / `Motion Error`.

Fix clear errors in place (bytepatch only), hand design decisions back to the user. Write "CH2d3 - Review" into the plan doc, update the STATUS rows (CH2d3 done, CH3a ready), check `kickoffs/char-ara/CH3a.md`, commit with explicit pathspecs.
