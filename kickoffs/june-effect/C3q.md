# June effect completion - C3q - June's NPC effect-set list: EFFECT_SET_LIST_FINAL and the CONSUME_MP gate (identify, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3q` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3o (found there) |
| Flag | `SERV_IRUHADEV_JUNE_NPC_EFFECT_SET_LIST` |
| Log tag | `JUNE_FX C3q` |
| Review | `kickoffs/june-effect/C3q-review.md` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3o - Done" "C3q - port: June's NPC effect-set list (EFFECT_SET_LIST_FINAL, CONSUME_MP)"
grep -n "(C3o)\|^32007570\|(C9h-4d)" june2017_re/funcmap.tsv
```

## Task

June's `CX2GUNPC::CommonFrameMove_EffectSet` (`32776e10`; C3o ported only its new call at state start) differs from March's
(`X2GUNPC.cpp` `CommonFrameMove_EffectSet`): entries are 0x3c bytes in the state data's list at `+0x1e4`, an entry with `+0x30 > 0`
plays only when NPC vslot `0xe4( +0x30, 0 )` returns true (the CONSUME_MP gate), the hyper bit is state flag `0x200000`, and the
delete lists are picked by state flags `0x8000 / 0x4000 / 0x10000 / 0x20000` (`32a2dec0`), then `3227a570( handle, entry +0x20 )`.
The entries come from June's NPC state loader (`32784c00`), which reads `EFFECT_SET_LIST_FINAL` = { `EFFECT_SETn` = { `START_TIME`,
`EFFECT_NAME`, `CONSUME_MP`, `DELETE_CONDITION` } } next to March's `EFFECT_SET_LIST` / `_EX`: find that block, its order against the
March keys and what wins when a state has both.

Data: 10 shipped NPC scripts use `EFFECT_SET_LIST_FINAL` (`PVP_HERO_ANNONA / BIX / DEAR / EOR / HYSLEY / IONIS / KANOS / LEAH / VINTON /
ZENITH`, data037); today March loads no effect set for those states. They are PvP heroes, not the AI-party bots (`CSM_PVP_HERO_*`, none
of which has the table), and the offline client has no PvP, so the play-test is a regression unless a reachable spawn is found.
Port the loader and the player together under one flag (the user side's version is `32007570`, C9h-4 / 4d, for comparison); vslot
`0xe4` is the NPC's MP check-and-consume: name it.
