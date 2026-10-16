# June backlog - Stage 4 planning - quest system

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan quest-system` |
| Output | `JUNE_QUEST_SYSTEM_PHASES.md` + `kickoffs/quest-system/` + a `quest-system` block in `STATUS.md` |
| Branch | `main` |
| Depends on | **Stage 1 closed** (atlas A13 and E15 done); skill-system-core **S1** done (June's tables as checked source); june-gfx **G0** done (the event-scene map). Not before. |
| Written | 2026-10-17, by the first `new-plan quest-system` session, which stopped at the preconditions and split the Scaleform runtime out as `june-gfx` |

## Decisions already made (user, 2026-10-17)

- **Guild quests: parse and log.** Load `QT_GUILD`, `AddGuildQuestTemplet_LUA` and `GuildCoopQuestRewardTable` so the scripts
  parse; skip guild quests with a log line. No guild stand-in in the emulator.
- **Scaleform: a runtime, as its own plan** (`JUNE_GFX_RUNTIME_PHASES.md`, key `june-gfx`). Its G3 ports `CX2GFXEventScene` and
  June's `PlayEventScene` binding. **The quest-UI callers of the scene (`323cb2e0`, from `323e1bf0`, `323e4cb0` and others) are
  this plan's**, built on G3. Scope there is event scenes + talk cut-ins; June's GFx quest-related screens, if any, are Stage 9.
- Scene reach: the scene XMLs key 493 scenes by quest ID; of those, only the Elysion quests 22329-22340 are reachable by current
  characters (the rest are Rose / Ain: Stage 8).

## Read first

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 4: quest system (including the deferred region quests)" "Open decisions that gate stages" "Standing rules (every stage, every kickoff)" "Stages 3–9 (short)"
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Context" "Phase G3 - Event scenes"
```
Then the Stage 4 inline brief's own read list (ATLAS_PORT_PHASES.md decision 2 and the quest hand-offs in the A / E Done notes,
`X2Lib/X2QuestManager.*`, the offline quest module, JUNE_UI_PARITY_PHASES.md's IsActiveDungeon / RequireCompletedQuestID
follow-up, S1's extracted tables), plus `june2017_re/g0_gfx_map.tsv`'s rows marked quest-system.

## Task

The backlog's "Stage 4, quest system" brief (in "Stages 3–9 (short)"), minus its "first ask" step: both decisions are above.
Re-check the preconditions against `git log` first; if one is not met, stop and say which.
