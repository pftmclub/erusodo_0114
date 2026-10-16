# June buff port - Phase 9c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 9c review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase 9c done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION`, `SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE` (also in `ServerDefine.h`) and `SERV_IRUHADEV_JUNE_BUFF_CLEAR_TYPE` (+ `_DEBUG`) |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 9b — Review (2026-10-05, Opus 5.5 high)" "Phase 9c — Done (2026-10-05)"
```

`june2017_re/funcmap.tsv` rows tagged "Buff Phase 9c" hold the RE. `kickoffs/june-buff/9c.md` is the task 9c answered.

## Task

Review Phase 9c's commit(s) against June. What this phase could get wrong:

1. **`KBuffFactor` (`CommonPacket.h/.cpp`, flag `_FACTOR_WIRE`).** First confirm the flag really reaches `CommonPacket.h`: its second define is in `ServerDefine.h`, because `CommonPacket.h` precedes `Always.h` (`X2Main.h:4`). A clean link proves it, since the member exists only under the flag. Then check these against June:
   - the new member's default (ctor `3211ea50`);
   - the copy ctor;
   - Put/Get against Get `320e3950`: uid after the identity, NPC flag before the factor ID, power last. The two June-only fields `+0x35`/`+0x39` are deliberately not carried.

   Find June's **Put** and confirm it mirrors Get (9c did not locate it). Also check whether anything in June's client reads or writes `+0x35`/`+0x39`. If neither happens, they are a value June always sends as 0: add a JUNE_BUGS row.
2. **The templet's creator fields.** Check each against June:
   - SetFactor `3236d400`: `+0xb0/+0xb8` are stored after the lookup, creator found or not;
   - GetFactor `32369f50 @3236a009`: unconditional, inside not-finished;
   - SetFactorFromPacket `3236c440 @3236c4f0`: after the displayers;
   - copy ctor `322ad9cd..`;
   - ctor `326c499b` (-1 UID), `326c49a7` (0).
3. **ABI.** Through `ServerDefine.h`, `X2ServerProtocol` now compiles the same `KBuffFactor` layout as X2Lib. Confirm that with the symbol check on `X2Lib/X2ServerProtocol.lib` (`KBuffFactor@@`, `KBuffInfo@@`, `KUnitBuffInfo@@`, `KNpcUnitBuffInfo@@`). Also check for any other `SERV_IRUHADEV_*` guard in a header that comes before `KTDX.h` (the same dead-flag trap).
4. **Sinister Mind.** The divide is June's (`3236e506-3236e516`), and the `_DEBUG` line runs before it.
5. **Death Sign / clear type.** Check the ported pieces against June:
   - the set in `ClearSubStage`: before the host test, as `327f03c0` (the NOT) stores it on every client;
   - the reset before `PrepareClearSubStage` (`327f127c`);
   - the reset in the ACK before `ClearSubStage` (`327eec80`);
   - the reader's null chain and `GT_DUNGEON` test (`3206f8c9-3206f975`).

   Then decide whether the unported state machine (`+0x30`, June's NOT, the CT_GAME / CT_FAILURE work in `327eec80`) changes when `+0x48 == 4` holds in a way the port misses. One known divergence: March can re-enter the clear check for a `CT_STAGE_FORCE` condition after a `CT_GAME` clear, and June's state 4 blocks that.

Hygiene, one command per flag (strip all four together; each alone FAILs the files the others share):
- `python tools/flagcheck.py <FLAG> --range <first 9c commit>^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_CUSTOM_FUNCTION JUNE_BUFF_CLEAR_TYPE JUNE_BUFF_FACTOR_WIRE`, for each of the three flags
- `python tools/flagcheck.py JUNE_BUFF_CLEAR_TYPE --range <first 9c commit>^..HEAD --with JUNE_BUFF_DEBUG --files X2Lib/X2DungeonSubStage.h X2Lib/X2DungeonSubStage.cpp X2Lib/X2DungeonGame.cpp`
- Also review the `SECOND_HOMES` addition to `tools/flagcheck.py`.

Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and deleted line.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase 9c — Review" into the plan doc, update the
phase's STATUS.md row, check `kickoffs/june-buff/13.md` still gates on 9c, and commit.
