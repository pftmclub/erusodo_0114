# Atlas/Elysion port - Phase E8 review (SkyCrossroads line)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E8 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | E8 done (its Done note and commit; the data035 / 037 / 040 copy can come before or after) |
| Flag | none new (data-only phase; `SERV_IRUHADEV_ELYSION_PORT` names the Lua blocks) |
| Log tag | `[ELYDBG]`; load errors in `ErrorLog.txt` (`python tools/logs.py --grep "DungeonData\|NPCTemplet\|WorldData"`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context (paste with any phase)" "Phase E8 — SkyCrossroads" "Per-line checklist" "Phase E8 — Done"
python -I june2017_re/region_census.py --line SC --limit 0 --no-probe
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review E8 (data only) against June. Things E8 could get wrong:

- **Registry rows, verbatim:** diff the three appended blocks against `june_src_fixed`:
  - `data035_kom/DungeonData.lua` `atlas E8` block against June data035 :32268-32508 (no allowed difference; June has no quest key on SkyCrossroads);
  - `data037_kom/NPCTemplet.lua` 18 rows against the ranges in the block header (June data037);
  - `data040_kom/WorldData.lua` against June :3270-3284.
  Check no SkyCrossroads row existed elsewhere in a file (duplicate `dungeonID` / `NPC_ID` / `worldID`).
- **Hero omission:** the four Hero ranges in the block header (`HERO_NORMAL :32509-32670`, `HARD :32671-32714`, `EXPERT :32715-32887`,
  `HELL :38436-38598`) are right and still C3y-5's.
- **Row choice:** the 4 Emperor rows added ahead of E9 (only Adrian / Soless spawn them), and the 4 left out (`EMPEROR_FEAR`, `SET_ELYSION_HERNA_BOW`,
  `SET_ELYSION_EMPEROR_BOW`, `HERNA_LANCE_CARD`: no March NUI, no June spawn). Confirm no SC / AR / SL / ME script names the left-out four.
- **Unlock chain:** `EXTRA_REQUIRE_DUNGEON_ID = DI_TOWER_NORMAL` against the Tower clear key (`unit_dungeon` 33350); `IsActiveDungeon`
  (`X2DungeonManager.cpp:750`) lets level 83+ in. SC's icon on the local map's Elysion page turns live once the row exists (E4).
- **Adopted files:** the six data040 files (Stage0-2, 3 line maps) equal `ljconv(June)` (sha1 in `june2017_re/june_adopt_20261017_143715.tsv`), manifest `"2"`.
- **June bug:** `CAINVINCIBLE` in `ELYSION_WATERCANNON_OBJECT.LUA` (JUNE_BUGS #239): confirm June's NPC state parser `32784c00` reads only
  `INVINCIBLE` (@3278f50c) and the key is a typo June never reads.
- **DUMMY_TITLE / FAKE_OBJECT:** the interplay in the Done note against `ELYSION_FAKE_OBJECT` (June source :255-280) and the two placements per difficulty.
- **Premises:** NPC readiness of the 18 rows (Enum / NPCTemplet / NewNPCStat / Motion meshes), the 3 boss-face pieces in `DLG_BossState_02.TET`,
  the buff census and the effect closure. Spot-check 2-3 of each.
- **Routed, not open:** 7 `assets.absent` (June ships none), `keys.file` CT_ / CF_ / state names, `ANIM_SPPED`, `CAINVINCIBLE`, `keys.registry` (checklist step 9).
- **Checklist step 11** (Elysion notes E8 added): accurate and enough for E9-E11.

Hygiene: `luasrc.py ledger --check` and `core --check`; census `--line SC` shows only the routed classes. If the user has copied, check
the live koms (`region_census.py --line SC --live`).

Fix clear transcription errors (repack, ask before copy); hand design decisions back to the user. Write "Phase E8 — Review" into the plan
doc, update the E8 STATUS row, write `kickoffs/atlas/E9.md` from the template if missing (E9's model: Sonnet 5.5 high if
`region_census.py --line AR` shows zero open engine gaps, else Opus 5.5 high), and commit.
