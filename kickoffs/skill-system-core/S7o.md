# June skill system core - Phase S7o - June's boss-raid enter UI ( g_pMain +0xd80 )

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7o` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7h review ( found there ); the port half may need `A6b` ( June's `BFT_BOSS_RAID` battlefield types ) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_SKILL_*` |
| Log tag | `[JUNE_SKILL]` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7o-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7h -- Review (2026-10-19, Opus 5.5 high)"
```
`STATUS.md` row `A6b` (atlas plan); `june2017_re/funcmap.tsv` rows tagged `Skill S7l` ( boss-raid fields `32482650` ).

## Task

The S7h review found that June's class-change NOT `31f276b0` @31f27c17 calls `32161070` on `g_pMain +0xd80` ( getter `31d70d70` ), and
that the S7h Done note's "emblem 16, S7l" owner does not hold: S7l did not take it and no STATUS row owns it. `32161070` rebuilds
`DLG_BossRaid_Enter.lua` ( `g_pStatic_Enter_UI`, `Active_Enter_Button` / `Deactive_Enter_Button`; flags `+0x124` / `+0x125`, `+0x11c` /
`+0x120` ). March has no such object, dialog or caller. Callers: `31f08290` ( 0x43b handler, S7d candidate ), `31f276b0` @31f27c17 ( state
0xe / 0xf / 0x15 and my class 10-199 ), `324bbe50`, `32509d30`.

1. **Identify ( Opus ):** the `+0xd80` object's class, ctor / owner, every member `32161070` reads, who sets `+0x124` / `+0x125` /
   `+0x11c` / `+0x120`, the four callers, and the dialog's data ( is `DLG_BossRaid_Enter.lua` in a June kom; is it in March's ).
   Funcmap rows tagged `Skill S7o`.
2. **Reach:** can the offline client show it ( boss-raid fields need `A6b`'s `BFT_BOSS_RAID` and a server event )? If the setters
   are server-driven and nothing offline sets them, say so and stop at identification ( a `not ported` funcmap row naming the blocker ).
3. **Port** ( if reachable ): June's object and `32161070` whole, then the S7h call at @31f27c17 in
   `CX2State::Handler_EGS_UNIT_CLASS_CHANGE_CASH_ITEM_NOT` and the other callers that have a March home. Assets through
   `kom.py stage-june` ( June wins at the same name ).

Gate: every `32161070` caller is ported or has a funcmap row with its blocker; if a port lands, a play-test line shows the dialog
built ( or the log says why it is off ).
