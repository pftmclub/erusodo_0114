# June port backlog - Stage 39 planning - June game-event systems

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-events` |
| Output | plan key `june-events` (`kickoffs/june-events/`, a `june-events` STATUS block), plan doc `JUNE_EVENTS_PHASES.md` (new) |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 39) |
| Depends on | - (planning is read-only; code waits for its turn in the priority order, after Stage 9) |
| Flag | one family, e.g. `SERV_IRUHADEV_JUNE_EVENT_*` (none in this planning session: no code) |
| Log tag | `JUNE_EVENT` (propose it in the plan) |
| Review | none (a planning session); each planned phase gets its own review kickoff |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 39: June game-event systems (june-census V2, 2026-11-09)" "Standing rules (every stage, every kickoff)"
grep -n -i -E "CustomEvent|LVUpEvent|RandomMission" june2017_re/xorstrings.tsv june2017_re/funcmap.tsv
```

and the `S39` rows of `june2017_re/census/exclusions.tsv`.

## Task

The census found June's date-window event systems with no owner:
- `CX2CustomEventManager` (52 unowned, `0x32682c20`-`0x32691980`; strings `StartDate`, `MaxRewardCount`, `IntervalSec`, `DummyRewardID`,
  `CustomEventScript.lua`, `CustomEventScript_AddChangeNpcInfo.lua`) and `CX2CustomEventItemSlotManager` (gap
  `X2TalkBoxManagerUI..X2PetAI@0x32eec4d0`, 82);
- `CX2LVUpEventMgr` (11, `0x32710980`-`0x32714af0`; `EVENT_ID`, `LEVEL`) and `CX2RandomMission` (3, `0x32676590`-`0x32676870`).
Their scripts may be server Lua (CLAUDE.md "server-side Lua the client needs is the user's to pack"); the standing rule "game events on
from the save DB" (memory, 2026-10-21) says how a date window switches on offline.
1. Identify each system: classes, Lua binders, data files (client kom or server-only), packets, UI, what the emulator must answer.
2. Check the triage gap `X2RandomMission..X2BossRaidManager@0x326797a0` (195; strings name custom events and NPC friendship) once Stage 40
   routes it; take only what it routes here.
3. Plan the port with the emulator handlers and the save-DB switch, and the play-test (an event window opened from `els_db.sql`).

Write the plan in the x2-phase `new-plan` shape, mark this brief's STATUS row done, commit with explicit pathspecs.
