# June quest system - Phase Q4e - Auto-complete request refused with a message box

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (Ghidra read first), then type `/x2-phase quest-system Q4e` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q4d built (it made the visit tick, which exposed this); Q3 (ported `IsCompleteVisitQuest`) |
| Flag | decided by the phase (own `SERV_IRUHADEV_JUNE_QUEST_*` flag if code changes) |
| Log tag | `QUEST    complete ... refused` (`python tools/logs.py --grep "complete 22162 refused"`) |
| Review | optional; the phase says |

## Read first

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Rules" "Phase Q4e - Visit-quest auto-complete refusal" "Q4d - Done"
```

## Task

Found by Q9 play-test (2026-10-19). After Q4d the visit sub-quest 22182 ticks, and the same frame the client sends `EGS_QUEST_COMPLETE_REQ` for the whole
quest 22162 (NPC 0, 21 bytes): `X2State.cpp:5044-5051` (`SQT_VISIT_DUNGEON` -> `CompleteQuestToVisitVillage`) gated by `IsCompleteVisitQuest`, which Q3 ported
as June's `3232c750` ("true once that sub-quest is complete"; March required it to be the last sub-quest). 22183 (the clear) is not done, so the emulator
answers `ERR_QUEST_09` (`X2OfflineQuest.cpp:1873`, copied from the March server) and the client shows its "completion requirement is not met" box.
Nothing is lost (the sub-quest stays done, the clear counts later), it is a popup on every first-position visit / field sub-quest.

1. **Read June** (`323e57e0` `Handler_EGS_QUEST_COMPLETE_ACK`, `X2State` visit case in `funcmap.tsv`): does June's client show the box for
   `ERR_QUEST_09` on this auto request, or does it skip it? Is the auto request itself gated differently in June (a caller test)?
2. **Decide by the rules:** if June's client shows the same box, it is a June quirk kept verbatim: `JUNE_BUGS.md` row, no code. If June's client or
   server answers differently (silent, no REQ), port that under its own flag. Do not invent a silent emulator reply the client's pending-packet
   bookkeeping (`DeleteServerPacket`) cannot take; read it first.
3. Proof: Q9 A3 again (22162 Enter Burnt Forest): no box, or the JUNE_BUGS row written.

No new `.lua`, no data change.
