# June skill system core - Phase S7m - June-only socket sources the S7k aggregate skipped

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7m` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7k review ( the unit aggregate `CX2Unit::JuneSumSocketOption` / `JuneInitSocketData` ) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_SKILL_*` |
| Log tag | `[JUNE_SKILL]` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7m-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7k - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7k` (`31ed40c0`, `3208c500`, `32365d30`, `32364790`) and Buff Phase 16 rows `3206d770` / `3204f190`.

## Task

Added by S7k (2026-10-19, unowned-branch rule). Three June-only socket sources that both June sums read ( the user's `3204f190` / `3206d770` and the unit's
`32364790` / `32365d30` ) and March has no data for. Identify each in Ghidra first; port where March can carry it, otherwise say what blocks it.

1. **The socket templet's `+0x324` block** ( 0x1c bytes: an int type, two bytes, a refcounted pointer, two floats ). Copied into the aggregate's `+0x32c` by `3208c500`;
   the user side ( `3204f190 @3204f5fc` ) branches on the type ( 1 / 3 / other ) when `fAtkMagic == 0` ( a protected value, decompile only: check the raw code ) and
   otherwise scales by `fAtkMagic`; the unit side writes `templet +0x334 x fAtkMagic` into `+0x344`. Find the loader key ( `32726850` or a helper it calls ), the
   readers of `+0x32c` / `+0x344`, and what it does in game.
2. **Party members' shared sockets** `31ed40c0` ( with `g_pX2Room` 0x33775868 and the game: each member's lists through `325bb840`, filtered by `31d70d30` to ids
   2000139-2000141 / 2000144 / 2000145, deduplicated ), summed by `3206d770 @3206df3e` and `32365d30 @323667bd`.
3. **The item socket lists `+0x8c` / `+0x9c` and the set-gated `+0xac`** ( `32365d30 @323660ee / @32366105 / @323661ad`, `3206d770 @3206dc10..3206dd30` ): what fills
   them ( the item data `Get`, or the templet ), and whether March's `KItemInfo` carries them.

The customize map `+0xa44` ( `32365d30 @32366528` ) belongs with June's customize system ( Effect C9d / C9g ); note it only.

**Play-test:** whatever the identification makes reachable offline; if nothing is, say so (no play-test owed).
