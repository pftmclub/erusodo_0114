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
