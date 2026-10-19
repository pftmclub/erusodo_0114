# June skill system core - Phase S7h - June's class-change notice

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7h` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7g (the HUD singleton `324943c0` this handler calls); S7 (`AddJuneAcquiredSkills`) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_SKILL_*` (a packet-struct change also goes in `ServerDefine.h` + `SECOND_HOMES`) |
| Log tag | `[JUNE_SKILL] CLASS` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7h-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7f - Done (2026-10-19, Opus 5.5 high)" "Phase S7 - Done (2026-10-18, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7f`.

## Task

Added by S7f (2026-10-19, unowned-branch rule). June's handler `31f276b0` (dispatcher `31f2ab80` @31f2b91b) reads, by layout, March's
`EGS_BUY_UNIT_CLASS_CHANGE_NOT` (`ClientPacket.h:7769`, handler `CX2State::Handler_EGS_UNIT_CLASS_CHANGE_CASH_ITEM_NOT`, `X2State.cpp:8957`).

1. **Confirm the packet** (the event id behind the dispatcher case) and port June's layout (Get `320ed4e0`): UID, a `short` class (+8, `33051f40`), four vectors
   (+0xa unsealed skills -> `32891560`; +0x1a items -> the inventory update; +0x2a / +0x3a quests -> `32336e80`), KUnitInfo (+0x4a), and a trailing vector (+0x3bf,
   `3211eb10`) handed to `328929e0` = clear + `AddJuneAcquiredSkills` (`328924c0`). June has no `m_mapChangeMemo`.
2. **Port the handler whole** (`SetKUnitInfo` path `32362730`, the memo merge @31f2791a, `32895c40( 0, 1, 0, 0 )`, the tree / page / quest / HUD refreshes, `3235a160`).
3. **The emulator** (`X2Lib/Offline/Handlers_Shop.cpp:749`) must fill the acquired-skill vector, or the client's clear empties the tree: send the unit's acquired list
   (the S7 stand-in's source), logged.

**Play-test:** buy a class-change item on a first-job character; the tree, slots and active memos are right after the change and after a relog.
