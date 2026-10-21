# June character Aisha (Stage 5 pilot) - Phase CH3c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH3c review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH3c done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL summon`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH3c - Done"
grep -a "(CH3c)" june2017_re/funcmap.tsv
grep -a "^| 393 " JUNE_BUGS.md
```

The code is the `//{{ ... Phase CH3c` blocks in `X2Lib/X2GUArme_VioletMage.cpp` / `.h`, the two UFO cases and the Stone Wall case of
`CX2Game::SetUserSummonedNPCInfo` (`X2Lib/X2Game.cpp`) and `CX2GUNPC::ScaleMoveSpeed` (`X2Lib/X2GUNPC.cpp`). `june2017_re/funcann.py <va>` gives listings.

## Task

Review CH3c's commit against June, in the listing, not only the decompile:
1. **`3230c1a0` cases `0x1ef` / `0x627`** (listing `3230c4a1`-`3230c559`): the case skill in `ebx` (`31fcf430( 31fcf520( npc ) )` = 22020, 22059 when Enhanced) reaches the
   level read, the templet and `32007b10( 0x4c, ebx )`; the effect-set name at `3348c6a8` is `..._Summon_Loop`; `323115e0` is SetUFO (`+0x9620`).
2. **`322ece70`** for flags 0: does anything in it other than the power rate (stat scale, hyper mode, life time) differ from March's Hell Drop case once the skill id is June's?
3. **`32757560`** (`ScaleMoveSpeed`): the four fields `+0x324..+0x330` against March's `m_PhysicParam` walk / run / jump / dash jump speeds.
4. **Case `0x1f5`** (`3230c585`-`3230c59a`) and `June_SetStoneWallSummonInfo`; confirm in the listing that no other caller creates NPC 501, and that no June bat case exists.
5. March's `GetEquippedActiveSkillTemplet( ..., true )` second argument with a swapped id (as in CH3b's Distortion case).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL --range <CH3c commit>^..<CH3c commit>`; paste the `files:` /
`in this flag's files:` / `RESULT:` lines. A FAIL is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH3c -- Review" into the plan doc, update the STATUS row, check `kickoffs/pilot-char-revamp/CH4.md`, and commit.
