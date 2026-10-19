# June skill system core - Phase S7j - June's max MP / max HP: ResetMaxMp, the unit's socket aggregate and the HUD refresh

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7j` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7g (it identified the pieces); S3 (June skill data); not alongside S8's review if it touches `X2GUUser.cpp` `ResetMaxMP` |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_SKILL_*` |
| Log tag | `[JUNE_SKILL] HUD` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7j-review.md` (the phase writes it when done) |

**Split by the user (2026-10-19, during S7j):** S7j ports item 1 only. Items 2-4 (the unit aggregate, the HUD refresh `3235a160`, its 23 callers) are
**S7k** (`kickoffs/skill-system-core/S7k.md`).

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7g - Done (2026-10-19, Opus 5.5 high)" "Phase S7g -- Review (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7g`, and the Buff Phase 16 rows for `32091080` (SocketData), `32364790` (the CX2Unit aggregate +0xe1c),
`3204b4c0` / `3204f190` (the user-side sum this mirrors).

## Task

Added by S7g (2026-10-19, unowned-branch rule). Port whole, in this order; design questions go to the user.

1. **June's `CX2Unit::ResetMaxMp` `3234b9d0`** (March `X2Unit.cpp:932`). June sums `GetSkillAbilityValue( SA_MAX_MP_ABS, level )` over 19 skills, 17 of them
   June-only `RNSI_*` ids (`june_src_fixed/data036_kom/Enum.src.lua`); March still reads 2402 / 5402 / 5410 / 7402, which June's skill data (S3) no longer has, so
   **in-game max MP from those passives is lost today**. June's item term `32869f80( param )` (inventory +0x2a4, list +0xf4, item kinds 1/3/5/7/9 always, a
   param-selected kind group otherwise) replaces March's `GetAddMaxMPValue`, and is skipped in June's PVE states (`31d717d0`, absent in March: always summed).
   Find June's in-game caller (the `CX2GUUser::ResetMaxMP` counterpart) and the param it passes.
2. **The CX2Unit socket aggregate** (`+0xe1c`, a `CX2SocketItem::SocketData`, allocated @32363d53), filled by CX2Unit vtable slots 1 / 2 (`32365d30` /
   `323652b0`) with a mode ( 2, then `+0xe40 == 1` ); its getters vslot 0xc / 0x10 ( `32091320` / `32091310` ) are the MP rate and the added MP.
3. **The HUD refresh `3235a160`**: max MP = `ResetMaxMp( 3344f348 ) x ( 1 + rate + ability(5) x 0.01 ) + added` -> `CX2GageManager` June SetMaxMp `3248a800( uid, mp, 0 )`
   (vslot 0x14 on the gage data; idx 1 only for unit type 10, absent in March); max HP = the stat sum ( `32358b00` unit, `3234e210` equip, `32890230` skill,
   `328902f0` guild, `326f8080` title, aggregate stat ) + min( sum x `3342ebc8`, the aggregate's first stat float ) -> June SetMaxHp `3248d3a0( uid, hp )` ( `31e8c940` ).
   `32358970` (the ability map) is June-only: 0, as in Buff Phase 16. Compare each stat getter with March's ( `GetUnitStat` / `GetEqipStat` / `GetSkillStat` /
   `GetGuildSkillStat` ) and port June's where it differs.
4. **The 23 callers of `3235a160`** (xrefs in Ghidra): the INIT ACK `3220ea50`, the grant NOT `323b0770`, the RESET ACK `322112d0`, `3220ee30` x2, the 0x735 handler
   `31f1fd90`, the class-change NOT `31f276b0` (S7h), ... Map each to March and add the call.

**Play-test:** a character with a max-MP passive (e.g. an Elemental Master with Unlimited Mana learned): in a village the HUD MP reads "x/300+bonus" and the 100/200
guide marks move; learn or reset the passive and the HUD changes at once; in a dungeon the max MP matches.
