# June character Eve (Stage 7) - Phase CH2a review - Skills: Nasod (5)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2a review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2a done (its Done note and commit) and the user's play-test |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL registered ...`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2a - Done (2026-10-27, Sonnet 5.5)"
```
Then the `CH2a` rows of `june2017_re/funcmap.tsv` (`grep "Char Eve CH2a"`).

## Task

Review CH2a against June `329c8a00` (cases 25000 / 25030, 25001 / 25031, 25002 / 25032, 25200 / 25213, 25203 / 25216 and the Conversion `+0x9c58` / `+0x9c5c` block `@329c9a64`), and the bodies `329663c0` `329665b0` `3298b9b0` `32989b60` `329780b0` `32973150` `32993020` `32007910` (plus `32966910` / `3296f9a0`, inert). Things this phase could get wrong:
- **The "= March" claims** (17 callbacks, funcmap row 2): read each pair in the listing, not the claim. `3296f0e0` registered as `JuneGenericSpecialActiveSkillEventProcess` (Eve's inline copy differs from `31fcc740` only by the `+0x8e90` accessor).
- `32989b60`: the `CreateNPCReq` argument order (key code 1, `NCT_CHANGESTAGE_REMAIN`, gate = `IsMyUnit`), the Rule Break proc direction (`rand < SA_EFFECTIVE_RATE`), the BTI 410 time (`+0x9c4c` x 20 / 60 + `+0x4070`), `ChangeBuffFinalizerTempletPtrList` called even when the list is empty, `SetBuffFactorToGameUnitByBuffFactorID` argument order (June creator / level are March's level / creator).
- `329780b0`: Cheat Code Electronic roll direction, HP x `SA_DAMAGED_REL`, MP x `SA_MP_RETAKE_ABS`, `June_SetSkillBuffFactors( id, 0, true )`; `32007910` upgrade swap and the PvP rate.
- `32993020`: the two `DUMMY_REAL` offsets (+-75, sign per facing), `MI_CHECK_AND_KILL_1` (`0xf`) on `GetNowStateSkillID()`, alpha ladder.
- Hyper tables: Stinger and Guardian use the normal table name (JUNE_BUGS 444).
- Play-test: Eve with Dimension Link, Illusion Stinger, Guardian, Photon Blink, Conversion equipped (A and B) casts June's state; no Motion Error / KEM line.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <CH2a commit>^..HEAD`; `python tools/enumscan.py --check X2Lib/X2GuEve.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH2a - Review" into the plan doc, update the CH2a STATUS.md row, check the next kickoff (`CH2a2`, and the new `CH2a3`).
