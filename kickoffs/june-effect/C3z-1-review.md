# June effect completion - C3z-1 review - June's map assets at the same filename

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-1 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3z-1 done (its Done note and commit); best after the user's play-test |
| Flag | `SERV_IRUHADEV_JUNE_IS_VOTING_LUTO` (play-test 2; `_DEBUG` is off) |
| Log tag | none new; mesh misses are `XEM_ERROR109` / `OpenXSkinMesh` lines in `ErrorLog.txt` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z - old dungeons on June's data (user, 2026-10-10)" "C3z-1 - Done (2026-10-10, Opus 5.5 high; data only, no C++, no flag, no build; 11 koms + 17 music copied)"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-1 - play-test 1 (2026-10-10 14:40): crash at Air Ship stage 1 -> 199 NPC assets" "C3z-1 - play-test 2 (2026-10-10): gates stay invincible -> IsVotingLuto bound; 37 more unbound methods -> C3z-4" "C3z-1 - play-test 3 (2026-10-10 15:20): PASSED"
grep "^| 166 |" JUNE_BUGS.md
```

## Task

Review C3z-1's census, staging and removals, the 199 NPC assets of play-test 1 and the `IsVotingLuto` port of play-test 2.

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
5. **Play-test logs** (if the user played): 0 EXCEPTION. Every `XEM_ERROR109` / `XEM_ERROR110` / `KEM_ERROR136` / `OpenXSkinMesh`
   line names a file in `c3z1_june_dropped.tsv` or one of Velder Bridge Hope 3-1's 4 data101 meshes (`River_Wave`, `Wall01`,
   `Back_Gate`, `Gate_Window`). Any other name is a finding.

6. **NPC assets (play-test 1):** `python june2017_re/npc_mesh_census.py --all-npc` needs 0. Spot-check 5 rows of
   `june2017_re/c3z1_npc_stage_plan.tsv` in their live koms.
7. **`IsVotingLuto` (play-test 2):** `python tools/flagcheck.py SERV_IRUHADEV_JUNE_IS_VOTING_LUTO --range 65ef74f^..HEAD --with
   SERV_IRUHADEV_JUNE_IS_VOTING_LUTO_DEBUG`, and paste its summary lines. In Ghidra / `disasm_range.py`, re-check `327c94a0`
   (3 bytes), `330e7c10` (returns 1), the vtable slot `0x244` in `334bb094`, `334c1f34` and `3348bf94`, and the ctor zeroing
   `@327cf464`. Confirm that no March class between `CX2Game` and the dungeon game needs its own override: March's subclasses
   are `CX2DungeonGame`, `CX2BattleFieldGame` and `CX2PVPGame` (no `CX2PVEGame`). The last two keep the base `true`: check that
   June's battle-field and PvP vtables also hold `330e7c10` at `0x244`.

Write "C3z-1 - Review" into the plan doc, update the C3z-1 STATUS row, check `kickoffs/june-effect/C3z-1b.md`, and commit.
