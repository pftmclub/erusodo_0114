# June effect completion - C3z-1 review - June's map assets at the same filename

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-1 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3z-1 done (its Done note and commit); best after the user's play-test |
| Flag | none (C3z-1 changed no C++) |
| Log tag | none new; mesh misses are `XEM_ERROR109` / `OpenXSkinMesh` lines in `ErrorLog.txt` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z - old dungeons on June's data (user, 2026-10-10)" "C3z-1 - Done (2026-10-10, Opus 5.5 high; data only, no C++, no flag, no build; 11 koms + 17 music copied)"
grep "^| 166 |" JUNE_BUGS.md
```

## Task

Review C3z-1's census, staging and removals. No C++ changed, so there is no flagcheck range.

1. **Re-run the gate:** `python june2017_re/dungeon_map_parity.py`.
   - Expect `to stage (kom): 0`, no differing music, and 24 rows not 0/0, each naming only `data101-dir`.
   - Anything else means a peer changed a kom dir or world script since the commit: say which.
2. **The tokenizer reads what the client reads.** Pick 3 world scripts and check the names `names_in` returns against the
   `SetXMesh` / `AddXSkinModel` / `SetBGM` calls in `june_src_fixed/data040_kom/<world>.src.lua`:
   - one with glued constants, e.g. `Hamel_Frozen_Water_Hall_Stage1.lua`'s `shadow1-3`;
   - one Velder world;
   - one mesh closure, e.g. `Altera_Battle_Air_Ship_s1_ground2.y`'s `lightcore_01.dds`.
   Also list `unresolved`: every name there must be in neither build.
3. **The removals are June's state.** Take 5 names from `june2017_re/c3z1_june_dropped.tsv` and check each:
   - it is in no June kom (`junekom` index);
   - a June world script still names it;
   - no live March kom has it now (`kom.py status data028 data029`).
   Also re-check that `Besma_Dragon_Road_Stage4.lua` (KidDay) is the only reachable March-side user (`c3z1_users.tsv`).
4. **The live koms hold June's bytes:** decode 10 of the 404 staged names from the live game-dir kom and compare them with junekom's
   strict June extract. Check that the 17 `data\music\` files md5-equal June's `14JUN17\data\music\`.
5. **Play-test logs** (if the user played): 0 EXCEPTION. Every `XEM_ERROR109` / `OpenXSkinMesh` line names a file in
   `c3z1_june_dropped.tsv` or the 2 Velder Bridge Hope 3-1 data101 meshes; any other name is a finding.

Write "C3z-1 - Review" into the plan doc, update the C3z-1 STATUS row, check `kickoffs/june-effect/C3z-1b.md`, and commit.
