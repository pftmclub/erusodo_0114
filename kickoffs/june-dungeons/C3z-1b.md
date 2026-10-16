# June dungeons - C3z-1b - June's map assets in data101 (Velder / Henir)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons C3z-1b` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` (branch `main`); this phase's text and the earlier Done notes still sit in `JUNE2017_EFFECT_COMPLETION_PHASES.md`, which the commands below read; write the new Done note into `JUNE_DUNGEONS_PHASES.md` |
| Depends on | june-asset-port **P0** (its data101 restore: `kom.py pack data101` = `+0/-0/~0` against live) and C3z-1 |
| Flag | none expected (data only) |
| Log tag | none new; mesh misses are `XEM_ERROR109` / `OpenXSkinMesh` lines in `ErrorLog.txt` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-1 - Done (2026-10-10, Opus 5.5 high; data only, no C++, no flag, no build; 11 koms + 17 music copied)"
head -5 june2017_re/c3z1_data101_pending.tsv
```

## The problem

C3z-1 brought every dungeon's map assets to June's bytes except those in data101 (Velder, Henir Space, and one shared Ranox / Hamel /
Sander texture). data101's kom_contents dir lacked 1,071 live entries, and 134 of its files were stubs smaller than live (111 B where live has 5,657 B), so it could not
be packed. 104 names were known to differ or be missing (`c3z1_data101_pending.tsv`; some only because their dir file is a stub), and
799 live-only entries could not be compared. The C3z-1 review (2026-10-15) fixed a tokenizer blind spot in all four censuses: the
pending list is now **119** names (+15 Velder) and the live-only set **889**; world_asset_collide shows 104 data101 DIFF.
C3x's 2 files (Velder Bridge Hope 3-1 `River_Wave`, `Wall01`) are already staged there.

## Task

1. **Check the restore:** data101's dir name set and sizes equal the live kom's (the C3z-1 Done note shows the check).
   If they do not, stop: P0 is not done.
2. **Re-run** `python june2017_re/dungeon_map_parity.py`. Its `dungeon_map_replace.tsv` now lists every data101 name whose June bytes
   differ or that March lacks. The count can move either way from 104: restored stubs that equal June drop out, and the 799 add.
3. **Stage** with `kom.py stage-june june2017_re/dungeon_map_replace.tsv --plan-out june2017_re/c3z1b_stage_plan.tsv`.
   - Also check for `march-only` names (meshes June merged away), as C3z-1 found in Besma. If there are any, run the
     `c3z1_users.py` users scan and ask the user before removing them (C3z-1's removal needed the user's OK).
4. **Pack data101.** Every `~changed` beyond the staged names must be recompression only (same raw size and algo), proven the
   way C3z-1 did it. Ask before `copy`.
5. **Gate:** every `data101-dir` row of `dungeon_map_parity.tsv` reads 0/0. `world_asset_collide.py` and `npc_mesh_census.py --worlds`
   over `june2017_re/c3z1_world_files.txt` report 0.

## Play-test

Velder Gate stage 1 and Velder Market stage 1: you stand on the ground you see. Logs: 0 EXCEPTION, and no `XEM_ERROR109` for a
Velder map file.
