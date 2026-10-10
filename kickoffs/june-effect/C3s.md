# June effect completion - C3s - June's UpdateUnitPointer (dungeon and battle field focus camera) (identify, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3s` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3o (found there) |
| Flag | one `SERV_IRUHADEV_JUNE_*` per logical change found |
| Log tag | `JUNE_FX C3s` |
| Review | `kickoffs/june-effect/C3s-review.md` if code lands |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3o - Done" "C3s - identify, then port: June's UpdateUnitPointer"
grep -n "(C3o)" june2017_re/funcmap.tsv
```

## Task

C3o found the two readers of `m_bIsApplyCinematicUI` (`+0x8e39`): `CX2DungeonGame::UpdateUnitPointer` = `327cea70` (no Ghidra function;
`funcdis.py --func 327cec78`) and its battle-field twin `32d530a0` (March `X2DungeonGame.cpp` / `X2BattleFieldGame.cpp`
`UpdateUnitPointer`). June's shape differs from March's: a per-NPC byte `+0x8e38` gate, animation-name compares through
`31c40570` / `3275aaf0` against `+0x8f8c` (string `0x3342f604`), state flags `0x2000000` (word `+4`) and `0x20000000` (word `+0`),
`322cef70( bool )`, focus handles through `+0x2e4` / `31e56b80` / `31e56830`, and the cinematic UI hide by `g_pMain` state id
(`+0x8c - 1 <= 0x19`, jump tables `0x327ceda0` / `0x327cedc4`) setting `g_pMain +0x90 -> +0x3e8`. Name each member and key (the
state-data parser `32784c00` sets the flags), diff both functions against March line by line, and port what differs at June's positions.
Shipped data that drives it (focus-camera states, the 6 `IS_APPLY_CINEMATIC_UI = FALSE` scripts) makes this a positive play-test if a
reachable dungeon opening uses a key the port adds.
