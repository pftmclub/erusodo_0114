# June backlog - Stage 28 planning - June party-manager handlers (CX2PartyManager packet handlers)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-party` |
| Output | `JUNE_PARTY_PHASES.md` + `kickoffs/june-party/` + a `june-party` block in `STATUS.md` |
| Branch | `main` |
| Depends on | nothing. Read-only planning; Ghidra with `unpacked_x2.exe`. |
| Written | 2026-10-19 by quest-system Q10b (scope rule: the story player only needed the forced-End lines; the rest of these handlers is not the quest plan's) |

## Read first

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 28: June party-manager handlers (added 2026-10-19, quest-system Q10b)" "Standing rules (every stage, every kickoff)"
```
Then `june2017_re/quest_plan/q10b_player_map.tsv` rows "forced end", and March's `X2Lib/X2PartyManager.cpp` (handlers ~:334-3257, dispatcher in the class's
packet proc).

## What is known

- June's `CX2PartyManager` packet handler is vtable `33496b80` slot 1, `3243ee30`: event ids 0x133..0x155 through byte table `3243f370` / jump table
  `3243f314`, 0x32f..0x3e8 through `3243f3ec` / `3243f394`, plus direct compares (0x23f, 0x240, 0x32e, 0x48e). June's event numbering differs from
  March's; name each id from its handler body, not from the number.
- `3243d1d0` (June 0x147) = March `Handler_EGS_PARTY_GAME_START_OPINION_CHECK_NOT`, with a June-only branch `3243cec0` on a byte of the packet
  ( dungeon info from the packet, STR 21304 "Li", custom messages 0x36e8 / 0x36e9 ).
- `3243d4e0` (June 0x37d) = March `Handler_EGS_AUTO_PARTY_MAKING_SUCCESS_NOT`, with a June-only tail after the popup: `31d70b00`, `31fc6010`, party
  manager `+0x8a` = 1, now-state `32461a70` and, on the packet byte, `324639a0`.
- Q10b ported only their story-slide-show forced Ends. Offline, the emulator's AI party sends neither packet, so these handlers are undriven: say so
  per handler (`IsClearTypeUndriven`-style labels are not needed; a log line is).

## Task

Diff every case of `3243ee30` against March's handler table (which ids exist only in June, which bodies changed), map the June-only packet fields
against `KncWX2Server/Common/` ( offline-only override: client-side behind the flag ), mark each handler's offline reach, and write the plan
( one flag family `SERV_IRUHADEV_JUNE_PARTY_*`, log tag `[JUNE_PARTY]` ). Mark this STATUS row done.
