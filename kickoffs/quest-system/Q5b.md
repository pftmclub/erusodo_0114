# June quest system - Phase Q5b - Quest UI: scenes, clear window, scene info

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase quest-system Q5b` |
| Plan doc | `JUNE_QUEST_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Q5a review, june-gfx G3 review |
| Flag | `SERV_IRUHADEV_JUNE_QUEST_SCENE` |
| Log tag | `[JUNE_QUEST] SCENE` (`python tools/logs.py --tag JUNE_QUEST`) |
| Review | `kickoffs/quest-system/Q5b-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_QUEST_SYSTEM_PHASES.md "Context" "Rules" "Starting context" "Phase Q5b - Quest UI: scenes, clear window, scene info" "Phase Q0b - Quest UI, packet and movie map"
```

## Task

Do "Phase Q5b" from Q0b's rows targeted Q5b. Build on june-gfx G3's `CX2GFXEventScene`. For its API, read
`python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Phase G3 - Event scenes"` and G3's Done note. The scene-info store is Q1's.
Done gate as in the section.

## Amended by Q0b

Rows: `june2017_re/quest_plan/q0b_ui_map.tsv`, target `Q5b`.
- **`323cb2e0( quest, mode )`**: 0 start, 1 end, 2 both. Callers: NEW_QUEST_NOT (0), QUEST_COMPLETE_ACK (1), ALL_COMPLETED (1),
  UICustomEventProc `0xa046` replay (2), and `31d8942f`. It replaces March's `PlayEventScene( m_wstrStartScene / m_wstrEndScene )` at
  `X2UIQuestNew.cpp:938/939, 1524, 1901, 2065` (1660 is the dead `#else`).
  - `323cb190` looks the quest up in the scene-info map (`g_pData +0x498`).
  - The selected unit's class picks a sub-row; the class row's name wins when it is non-empty, else `DEFAULT_*`.
  - Each name goes to GFx lookup `3226ac80` + Play `3226b130` (G3).
  - Templet `+0x29c` -> `3232df20` (a field-game sound, flag `+0x148`; read it).
  - End or both also plays every `m_vecAdditionalSceneQuestID`'s start and end.
  - NEW_QUEST_NOT in state 5 defers the scene through `g_pInstanceData +0xaf8 / +0xafc`. Find that reader and port it.
- **No Lua fallback in June (likely).** `323cb2e0` and `3226b130` never call the Lua scene (`g_pData+0x278`). No `GetQuestTemplet` caller
  reads templet `+0x1d8` / `+0x1f4`, where the parser (`323321f0`) stores `m_wstrStartScene` / `m_wstrEndScene`.
  So 780 of the 968 June quests that name a Lua scene play nothing in June.
  **Before removing March's Lua calls**, re-prove it with a whole-image sweep for `[reg + 0x1d8]` / `[reg + 0x1f4]` reads on a quest
  templet. Then follow June (June wins), with a log line naming the quest when no scene-info row exists.
  **This replaces the done gate's second half:** "a pre-Elysion quest with a Lua scene still plays that scene" becomes
  "a quest with no scene-info row plays no scene and logs it".
- **The clear window** `323d2c60` matches `OpenQuestRewardWindow` (`:2331`). Port it whole, and adopt June's `DLG_UI_NEW_Quest_Clear` (`ljconv`, data034) with it.

## Added by the Q5a review (2026-10-19)

- NEW_QUEST_NOT `323e4cb0` @323e4e48-323e4e54: for **every** epic quest (templet type 5) June sets `*(g_pData +0x274) +0x70 = 1` before it
  opens the detail mode. March calls `NotifyNewEpicQuest()` only inside its start-scene branch (`X2UIQuestNew.cpp`, NEW_QUEST_NOT). Name the
  `+0x274` object (likely the event scene, `m_bNotifyNewEpicQuest`) and port the write with the scene calls.
