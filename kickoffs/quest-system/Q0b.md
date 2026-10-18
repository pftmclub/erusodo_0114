# June quest system - Phase Q0b - Quest UI, packet and movie map

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase quest-system Q0b` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | - |
| Flag | none (read-only) |
| Log tag | `[JUNE_QUEST]` (none written) (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Phase Q0b - Quest UI, packet and movie map"
```

## Task

Do "Phase Q0b". It is read-only. Delegate the reads and mechanical Ghidra dumps; keep the verdicts.

The planning digest's candidate list for June's `CX2UIQuestNew`:
- builder `323d0ca0` (the List / Detail / Quick DLGs), clear window `323d2c60`;
- `323c8da0`, `323ce6f0`, `323cd570`, `323cdfe0`, `323ccbc0`, `323c9e80`, `323ca520`, `323e5340`;
- `323dfc00`, `323ca6d0`, `323dfa80`, `323d9400`, `323cee10`, `323d4390`;
- the handlers `323e4cb0`, `323e57e0`, `323e5c10`, `323e4930`, `323e4aa0`, `323e05f0`, `323e08c0`, `323e15e0`, `323e1710`, `323caaf0`, `323cf130`;
- the range `323e3e8d`-`323e4cb0`, which Ghidra has not made into a function;
- dispatcher `323e5fb0` (called from `32159c90`);
- scene starter `323cb2e0`;
- list builder `323e1bf0` (called from `3214f7c0`).

March's side:
- `X2Lib/X2UIQuestNew.*` (live, `SERV_EPIC_QUEST`), packet handlers at `:1436-2017`;
- `X2State.cpp:4747-4776`;
- `ClientPacket.h:2072-2176`, `EventID_Client.h:349-375`.

Write `q0b_ui_map.tsv` and funcmap rows `Quest Q0b`. Amend `Q5a.md`, `Q5b.md` and `Q8.md` under "## Amended by Q0b", and `Q4.md`
for any June-only packet the emulator must answer. Name every shared-struct change. Done gate as in the section.
