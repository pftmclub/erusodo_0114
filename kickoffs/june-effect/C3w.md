# June effect completion - C3w - June's CX2DungeonSubStage::ShowBossNameAtStart (port whole)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3w` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3s (the `FORCE_CINEMATIC_STATE` member and parse exist under `SERV_IRUHADEV_JUNE_FORCE_CINEMATIC_STATE`) |
| Flag | one `SERV_IRUHADEV_JUNE_*` per logical change found |
| Log tag | `JUNE_FX C3w` |
| Review | `kickoffs/june-effect/C3w-review.md` if code lands |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3s - Done" "C3w - port: June's ShowBossNameAtStart"
grep -n "(C3s)" june2017_re/funcmap.tsv
```

## Task

C3s found that June's `CX2DungeonSubStage::ShowBossNameAtStart` = `327ef160` (only caller the sub-stage frame move `327f12c0 @327f13a5`;
`funcdis.py --func 327ef160`) differs from March `X2DungeonSubStage.cpp` `ShowBossNameAtStart`. Port the whole function
("no partial June updates"):

- **Game-type gate**: June runs for game vslot `0x10c` type 2 **or** 4 (March: `GT_DUNGEON` only). Name type 4.
- **Forced cinematic state**: an NPC whose `FORCE_CINEMATIC_STATE` is set (`327d65d0` = C3s's `HasJuneForceCinematicState`) shows the
  boss intro exactly when its now-state (`+0x8454`) equals `GetStateID( name )` (`3275aaf0`), **bypassing** `m_bShowBossName` (`+0x8ae9`)
  and the delay (`+0x8e34` vs sub-stage time `+0x28`). With `m_bShowBossName` set and the delay passed, the forced state still gates it.
- **Several boss names**: the boss-name texture (`+0x8af0`, wstring, length `+0x8b00`) goes to `327cc7c0` with a per-call counter while
  the counter `<= 7` (March: one `ShowBossName( tex )`). Name `327cc7c0` and the CX2DungeonGame side it needs.
- The two `BOSS_DANGER` / `_ADD` particles (`0x334bc0fc` / `0x334bc0dc`) and the stores into `+0x68` / `+0x6c`; the sub-boss branch
  (`+0x8aea`) and `+0x24` (`m_bBossNameWasShown`): diff against March line by line.

Data: June `Secret_Mad_Trock_Summoner` is the only script with `FORCE_CINEMATIC_STATE`, and no NPCTemplet (March or June) names it.
Count the scripts that reach the multi-name path to decide the play-test.
