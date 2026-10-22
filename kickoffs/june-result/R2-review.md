# June dungeon result - Phase R2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, medium** effort, then type `/x2-phase june-result R2 review` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | R2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_RESULT_OFFLINE` |
| Log tag | `[JUNE_RESULT] EMU` |

## Read first

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Starting context" "Phase R2 - Offline emulator for June's flow" "R2 - Done"
```

Then the R2 rows of `june2017_re/result_r0_map.tsv` (`awk -F'\t' '$5 ~ /R2/'`).

## Task

Review R2's commit against the R0 map:

1. `EGS_RESULT_SUCCESS_REQ`: June `324d84c0` / `31e57100` sends no body; the emulator accepts the empty body and still reads March's struct.
2. Retry: REQ `0x648` is SendID with no body (`324c9530`); ACK `0x649` is `{ int, wstring }` (`324ddac0` reader, `324c95b0` handler). Check
   Put / Get order in `ClientPacket.cpp` against the reader in Ghidra (read-only), and that the new IDs sit at the end of the
   `EventID_Client.h` block behind the flag.
3. `0x1f2` (`324cf734`): SendID, no body; the ignore row's name matches the enum entry.
4. The retry stand-in (user choice, 2026-10-22: restart the same dungeon, difficulty, mode and party like a fresh start) goes through
   `OpenDungeonGameRoom` + `SendDungeonGameStartNot`. Check what it skips against `Handler_EGS_LEAVE_ROOM_REQ` (buff leave, quest
   hooks) and that bots are carried over.
5. Whether the client can take a second `EGS_PARTY_GAME_START_NOT` while in `XS_DUNGEON_GAME` is **not** R2's: it is R3's play-test
   gate. Do not fail R2 for it; confirm R3's kickoff names it.

Hygiene: `python tools/flagcheck.py JUNE_RESULT_OFFLINE --range <R2 commit>^..<R2 commit>`; paste `files:` / `in this flag's files:` / `RESULT:`.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"R2 - Review" into the plan doc, update the STATUS row, check R3's kickoff, commit.
