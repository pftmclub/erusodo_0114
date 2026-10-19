# June skill system core - Phase S7p - June's character-window socket total ( GetMyEquippedSocketData )

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7p` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7m review ( the party pass `CX2Unit::JuneGetPartyShareSocketOptions` ) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_SKILL_*` |
| Log tag | `[JUNE_SKILL]` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7p-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7p - June's character-window socket total" "Phase S7m - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `(S7m)` and `(S7k)`.

## Task

Added by S7m (2026-10-19, unowned-branch rule). June `3291ba40( out SocketData, group )` is June's form of March's `CX2UICharInfo::GetMyEquippedSocketData()`
(`X2UICharInfo.cpp:2474`, the character window's socket total, read at `:1423`). `3291cee0( out, group )` sums group 2 and then the requested group (only caller `3291cfb0`).
June's version adds the equip-group filter ( `3286a4b0` ), the riding pet, the title, the sets and the party pass ( `31ed40c0 @3291c355`, `31d70d30 @3291c55c` ),
each through `3291b4b0`.

1. Identify `3291ba40`, `3291b4b0`, `3291cee0` and `3291cfb0` whole, and map each to March's char-info code ( the reader at `:1423` and what it shows ).
2. Port June's form whole ( June wins, no partial update ); March's body goes in `#else`. Reuse S7k / S7m helpers where June calls the same thing
   ( the party pass is `CX2Unit::JuneGetPartyShareSocketOptions` ).
   S7m review: `31ed40c0`'s set count `32350b80` filters by the equip group `3286a3e0( uid, 3234d8d0() )`, which reads *my* unit's `+0xe40`; March has no second set,
   so every slot counts. Check whether `3291ba40`'s own set / equipment loops use the same group source or the `group` argument.
3. Note anything that needs June-only item data ( item `+0x8c` / `+0x9c` / `+0xac`: backlog Stage 16 ) instead of porting it.

**Play-test:** the character window's stat / socket view with unit 19 ( or any equipped character ), before / after an equip change; say what should change, if anything.
