# Atlas/Elysion port - Phase E12 review (Elysion field PvP map)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E12 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | E12 done (Done note + commit); the data034 / 036 / 040 / 065 / 215 copy can come before or after |
| Flag | `SERV_IRUHADEV_ELYSION_PORT` names the Lua block; E12 changed no C++ |
| Log tag | load errors in `ErrorLog.txt` (`python tools/logs.py --grep "WorldData\|PVP_Map_List\|PVP_Elysion\|DLG_PVP_Map_List"`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Phase E12 — Elysion field PvP map (optional)" "Phase E12 — Done"
python tools/docsection.py RANOX_PORT_PLAN.md "Phase 11 findings (executed 2026-09-19)"
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

- **WorldData block:** diff the `atlas E12` block against June `june_src_fixed/data040_kom/WorldData.src.lua` :3700-3704 (no allowed difference); one hit for `WI_ELYSION_FIELD_PVPMAP` as a worldID.
- **Adopted files = `ljconv(June)`:** `june_adopt_20261017_174813.tsv` (scene + linemap), `_174816` (`PVP_Map_List`), `_174905` (`DLG_PVP_Map_List`); manifest `"2"`;
  the staged `DLG_New_PVP_Image09.TET` / `DLG_New_PVP_Image_TITLE.TET` equal June's bytes (sizes 344 / 2048).
- **Superseded March data:** diff March's old `PVP_Map_List.src.lua` (backup dir `kom_backup_stage_june\20261017_174816`) against June's: only the Ranox comment,
  the Elysion row and the two `m_MapSize` swaps. Same for the old data034 `DLG_PVP_Map_List.lua`: no March-only map row lost (its `AddMapInfo_LUA` list vs June's).
- **Consumers:** `X2PartyUI.cpp:4051` (`PVP_Map_List.lua`) and `X2StatePVPRoom.cpp:4457` (`DLG_PVP_Map_List.lua`) read the new fields the same way as the old rows (`m_bHot` / `m_bNew`, `m_MapNameTexKeyName`);
  `SetTex( tex, key )` finds `MAP_SELECT_ELISION` in the Image09 / TITLE TETs (key present, case-insensitive).
- **Scene / linemap:** every enum token and method bound (E12 checked 16 + 44), no `LT_FIRE_GROUND`; re-run `june2017_re/atlas_a11_closure.py` if the PvP names are not in its region set, else say so for E13.
- **Lanox note:** `STR_ID_44260/44261` resolve in `General.ess` (the old Lanox risk is closed); `MAP_SELECT_RANOX` now resolves too.
- **Play-test (if run):** PvP room map window lists Ranox + Elysion with art; Elysion loads (WI 752), music `Field_Elysion001`, wheels turn, rain layer. Offline has no PvP: record what was reachable.

Hygiene: `luasrc.py ledger --check` and `core --check`; after the user copies, nothing else is owed.
