# Atlas/Elysion port - Phase A7 review (local map, Atlas half)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A7 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A7 done (its Done note and commit; the user's A7 + A5 play-test can run before or after) |
| Flag | `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION` (`[ELYDBG]` first-use lines behind `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` (`python tools/logs.py --tag ELYDBG`; the VillageParsing line is in ErrorLog.txt if the mount-time log survives) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Elysion half: local map (Elysion page)" "Phase A7 — Local map, Atlas half" "Phase A7 — Done"
awk -F'\t' '$11 ~ /A7/' june2017_re/region_reach.tsv
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review A7's commit against June (addresses in each `#ifdef` block and the Done note). The things A7 could get wrong:

- **June transcriptions:** the page filter in `UpdateDungeonButtons` (@3293bc66-3293bd65: strings "0" @33458f54 / "1" @33489510, the
  compare `31cda210` returns true when the strings differ, its position after the custom-event check); the event-code 2260 branches in
  `UpdateLocalMapDLG` (@3293c8d9-3293c91f "Elysion" control, @3293c97e-3293c9b7 tab + the `goto 3293cde3` that skips the rest of the loop
  for every tab); where June calls the scroll (@3293c146-3293c16a). Judge whether hiding `LocalMapMoveUp/Down` until E4 is the right stand-in.
- **The user's stand-ins (2026-10-17, guard + gate):** `VillageParsing`'s degrade closes exactly the table it opened; the Cobo / warp-NPC
  `Village_20009` skip keeps `row` consistent; the world-map pin gate shows the locked art like the level gate does, and goes idle once
  E2 / E3 build 20009 / Field 03. Look for any other reader of LMI_ELYSION's `m_VillageID` (20009) that dereferences a NULL village templet.
- **Data:** `DLG_UI_Local_Map_NEW` Btn9 + QuestGuide picture equal June's (JF data217 :520-556, :761-772); `DLG_Map_Local` / `DLG_Map_World_Elios`
  entries; the 21 `MapData` rows (client + server mirror) equal June's :576-596; the three June files are unedited ljconv bytecode; the two
  atlases equal June's sizes. JUNE_BUGS 206.
- **Omissions:** Hero rows / Hero dummies, LMI_EVENT, the guide window, the quest test (decision 2): each still true.

Hygiene: `python tools/flagcheck.py JUNE_LOCAL_MAP_ELYSION --range <A7 commit>^..<A7 commit> --files <A7's 7 code files>` (`..HEAD` times
out). Paste its `files:` / `in this flag's files:` / `RESULT:` lines; every OTHER row and deleted line explained. Also
`python tools/luasrc.py ledger --check` and `core --check`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the
same flag (then `tools/build.py --deploy`, and repack / ask before copy for data); hand design decisions back to the user. Write
"Phase A7 — Review" into the plan doc, update the A7 STATUS row, check `kickoffs/atlas/A8.md` exists (write it from the plan if not), and commit.
