# June character Aisha (Stage 5 pilot) - Phase CH3b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH3b review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH3b done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH3b - Done"
grep -a "(CH3b)" june2017_re/funcmap.tsv
grep -a "^| 391 " JUNE_BUGS.md
```

The code is the `//{{ ... Phase CH3b` blocks in `X2Lib/X2GUArme_VioletMage.cpp` / `.h`, `CX2GUUser::JuneRescaleBuffFactorByMemoRate`
(`X2Lib/X2GUUser.cpp`) and the Distortion case of `CX2Game::SetUserSummonedNPCInfo` (`X2Lib/X2Game.cpp`). `june2017_re/funcann.py <va>` gives listings.

## Task

Review CH3b's commit against June, in the listing, not only the decompile:
1. **Registration `32cede00`**: cases `0x5609 0x560b 0x560c 0x560d 0x561d 0x561e 0x5636 0x56c6 0x56d2` (callback slots, the hyper copy: `329cc2c0`
   for Morning Star / Impact Zone, field by field elsewhere, the hyper flag on Air Stunner / Crazy Worm / Energy Ring only); the Air Stunner END
   state `0xf8` carries only `329cba60`.
2. **Distortion FrameMove `32cdda50`** (the `322e67c0` argument list against Hell Drop `32cd95d0`) and **Worm Hole FrameMove `32ce0f50`**.
3. **Morning Star `32ced8b0`**: the instance loop, the `break` placement, the `322b5320` call with the `31fdbfc0` Rate, and the early return on a NULL unit.
   Check `32089ff0` against `JuneRescaleBuffFactorByMemoRate` (time first, then behaviors).
4. **Air Stunner `32cc5aa0`** (`+0x7e1c` = IsOnSomethingFuture) and `StartFuture` `32cc70b0` (`+0x8900` / `+0x8905`).
5. **`3230c1a0` case `0x443`** in `June_SetDistortionSummonInfo` (March keeps its stat line; power rate and end follow skill 22027).
6. The hyper cut-in lines: `logs.py --grep "ARME UI hyper cut-in"` names `HQ_Cutin_AEM / AVP / ADW_Transcendence` for 104 / 105 / 113 (kickoff CH3b, CH5 review).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL --range <CH3b commit>^..<CH3b commit>`; paste the `files:` /
`in this flag's files:` / `RESULT:` lines. A FAIL is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH3b -- Review" into the plan doc, update the STATUS row, check `kickoffs/pilot-char-revamp/CH3c.md`, and commit.
