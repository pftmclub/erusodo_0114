# June engine remainder - Phase W1d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1d review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase W1d done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_PET_AI_ROAD_RESEAT` (+ short-lived `_DEBUG`) |
| Log tag | `[JUNE_ENGINE] WORLD pet re-seat` (`python tools/logs.py --grep "WORLD pet re-seat"`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase W1d - Pet re-seat on the master's road type" "Phase W1d -- Done"
grep -a "(W1d)" june2017_re/funcmap.tsv
grep -a "^| 278 " JUNE_BUGS.md
```

March code: the flag's block in `CX2PETAI::Moving` (`X2Lib/X2PetAI.cpp`, after `bool bAIIsRight = false;`).

## Task

Review Phase W1d's commit against June's `CX2PETAI::Moving` (`32ef68b0`). What this phase could get wrong:
- the block (`@32ef69b2-32ef6b08`): field before square game, the master lookup (`323a8dd0` / `323a4dd0` on `+0xd8/+0xdc`), the pet line from
  `+0x3c` and the master line from the square unit's byte `+0x3d4`, the land position from the **pet's** position (`m_pPet +0x42c`, not the
  master's), `bMonster` true and radius 30 (`@33441b9c`), `GetNearOtherLine` radius 200 (`@33457460`) and its argument order, the type compare
  (`LineData +4`), and `SetPosition( out, +0x40 m_bIsRight )`;
- the claim that the rest of `32ef68b0` is March's `Moving` (random jump, the fly / walk path-finder calls and their arguments, the move-state
  switch, the fly and walk / dash intervals, the send); re-read at least the jump block and the walk / dash block;
- the claim that June's `GetNearOtherLine` (`31c688c0`) and `CX2PET::SetPosition` (`326eb090`) equal March's (`KTDGLineMap.cpp:2733`,
  `X2Pet.cpp:3129`);
- JUNE_BUGS 278: both quirks really are June's (no found-line type test; a master line index out of range is read through NULL).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_PET_AI_ROAD_RESEAT --range <W1d commit>^..<W1d commit> --with SERV_IRUHADEV_JUNE_PET_AI_ROAD_RESEAT_DEBUG`
(`git log -S SERV_IRUHADEV_JUNE_PET_AI_ROAD_RESEAT` names the commit). Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

If the play-test passed, remove the short-lived `_DEBUG` flag (its `#define` and block) in the same pass and rebuild.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Phase W1d -- Review" into the plan doc, update the W1d STATUS.md row, and commit.
