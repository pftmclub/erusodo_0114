# June character Aisha (Stage 5 pilot) - Phase CH3a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH3a review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH3a done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH3a - Done"
grep -a "Char CH3a" june2017_re/funcmap.tsv                                   # the 15 rows the phase wrote
```

The `June_*` members are in the CH3a block at the end of `X2Lib/X2GUArme_VioletMage.cpp` (`//{{ ... Phase CH3a`), the 15 registration cases are in `June_SetEquippedSkillFuncHead`
(after the CH2c case), the ids are the `JUNE_CH3A_*` constants after the CH2b block. Not carved in Ghidra: read with `python june2017_re/funcann.py <va>` (annotated) or `funcdis.py --func <va>`;
`python june2017_re/funccmp.py <a> <b>` proves two bodies equal.

## Task

What the phase could get wrong, in the order of risk. **Ghidra's decompile reuses stack slots here: confirm every value in the listing.**
1. **Hell Drop**: FrameMove `32cd95d0` (CanDown call shape `@32cd97ba`, the CreateNPCReq argument order `@32cd98d5-32cd9925`: key code `( +0x975c == 1 ) ? 0 : -1`, delay 0.3, no drop, team `+0x278`, `NAT_ALLY`,
   UID, the five June trailing arguments against March's defaults; the local-unit gate vslot `0x198`), EventProcess `32cbf2d0` (memo 21 branch only), the control FrameMove `32cd9af0` and the control EventProcess `32cd9d00`
   (the class map missed it: jump table `32cda1b0` order A S D C Q W E R, the hold test with the sit key, the double `1.1000000014901161`, the direction chain, DYING + the `+0x95f4` stop). Registration `@32cef319-32cef609`:
   states 187 .. 197 and which callbacks each carries (the FIRE state `32cf1bc0` / `32cf0e10`, DYING `329cbfc0`).
2. **Ruin**: EventProcess `32cc5940`, the three ability reads (`1`, `0x18`, `0x1b`) into `+0x97ac / b0 / b4`, the registration (EventProcess on normal + hyper, loop `32cf1910`, end `329cba60`).
3. **Catastrophe EventProcess `32cbe9f0`** against the macro chain (the animation-end head is an `if`, not part of the else chain; the order BWALK, WALK, DASH, Z, X; the AWAKE_LOCK form of `SKILL_CANCEL_AFTER`).
4. **Blaze Step / Cyclone / Summon Bat FrameMoves** (`32cd85d0 / 32cd8850 / 32cd9340 / 32cd8ea0`): the early `return` without CommonFrameMove when there is no CX2Unit, the `0.733` / `1.53` / `0.3` timers, the effect-set arguments,
   the handle `+0x977c` stop, hyper = `GetRemainHyperModeTime() > 0`.
5. **Circle Flame**: READY FrameMove `32cd7470` (the `32296d60` alpha tag), state 186's `32cd75f0` (names, element test) and the "= March" claims (`32cbee50`, `32cbeeb0`, `32cb7870`, `32cbef80`, `32cd7800`, `32cb78f0`, `32cb7890`).
6. **"= March" claims in general**: Inits (`32cb77c0 32cb7a60 32cb7ab0 32cb7c00 32cb7c30 32cb7e50`), `32cbfe20` against March's Phantom Breathing EventProcess, the Hell Drop Start / End (`32cb7cb0` / `32cbf320`) against March's.
7. **Routing**: the deferred ids (Lightning Shower 22012 / 22052, Fire Ring 22224, Dark Ring 22225) still fall to March's cases; `31c69af0` and `31ff5440` are in `CH3a2.md`.

**User concern (2026-10-21, after the play-test passed overall): Cyclone looks small; the user remembers the revamped one as big.** CH3a checked and found no cause: the `32cd95a8` CreateInstance call equals March's (the two June extra trailing args are 0, no scale), and `Aisha_Elemental_Master_Cyclone_Outside` (size 0 -> 1, `EM_Cyclone_Effect_Outside.X`), the three particles and the damage effect are byte-equal to June's; the templet has no scope ability; June's description says "mini tornado". Next step if it still looks wrong: the user compares against the June client (`237311JUN17`) and says what differs (width, height, missing swirl), then check the engine side (main-effect scale, hit-box) in a separate task.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ARME_SKILL --range <CH3a commit>^..<CH3a commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines.

Play-test: a High Magician / Elemental Master (104) with Blaze Step, Cyclone, Chain Burst, Circle of Elements, Frost Barrage, Circle Flame and Elemental Edge; a Void Princess (105) with Plasma Cutter,
Phantom Breathing, Hell Drop, Dark Burst, Ruin, Petit Angkor, Millions, Icicle Spear and Abyss Field. The log shows `ARME SKILL registered <name>` per equipped skill.

Spot-check 3 functions directly in the listing; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH3a -- Review" into the plan doc under the Done note, update the CH3a row in `STATUS.md`, and commit with explicit pathspecs.
