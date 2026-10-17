# Atlas/Elysion port - Phase E13 review (Elysion asset closure + pack list)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E13 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | E13 done (Done note + commit); the five koms are already copied |
| Flag | none (data phase) |
| Log tag | none; load misses in `ErrorLog.txt` (`python tools/logs.py --grep "OpenTexture\|XEM_ERROR\|result : 47"`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Phase E13 — Done" "Phase A11 — Review"
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

- **The 46 staged files.** Each equals June's strict decode (`junekom.decode`), manifest `"0"`, one archive only, listed by `elstools inspect` in the live kom at the same size; live md5 =
  packed md5 = the `COPY OK` values (data013 caf8ee84, data065 e31dbc70, data096 a2cf0c63, data181 2fef4b1d, data215 6238322c). Spot-read the UTF-16LE keys
  `MAP_SELECT_IMAGE__RESIAM_TRIANGLE` (Image08.TET) and `MAP_SELECT_IMAGE_WINNER_ADRIAN` (Image10.TET).
- **Sweep method** (`june2017_re/atlas_e13_sweep.py`): it covers `.lua` modified on/after 2026-10-17 that the region set does not own. Check it missed none: list every kom_contents `.lua`
  an E-phase staged (ledger rows + `june_adopt_*.tsv`) and confirm each is either region-owned or in the sweep's 31; check the bytecode / source split (`bytecode_consts` for `1B 4B 4C 81`,
  `ASSET_RE` for BOM source) loses no name (compare a BOM-source file's regex hits to its string constants). Also: `ASSET_RE` / the sweep skip `.ogg` sound names embedded in particle
  or effect files, and TET key pairs in the swept files (`SetTex` calls inside registry rows) were not censused: say so or run them through `tet_pairs`.
- **Routed classes.** The 5 `MISSING-june-has` left (event houses, `nui_mul_mai_event`) are not Elysion rows; confirm the referrer rows pre-date the port (`DLG_Map_House` rows
  `HI_EVENT_GUILD_EDAN_VILAGE` / `HI_EVENT_BLESS_OF_SOLACE`). The 7 `koms.unstaged` are known (Tower_Stage3 #209, 5 seasonal scenes, `Movie_Elysion_Clear`).
- **Gate re-run:** `region_census.py --region elysion --limit 0 --no-probe --live` -> `live {"same": 346}`; `atlas_e13_closure.py` -> `PACKLIST ... 0`; `luasrc.py ledger --check` / `core --check` PASS.

Write "Phase E13 — Review" into the plan doc, update E13's STATUS row, check `kickoffs/atlas/E14.md`, and commit.
