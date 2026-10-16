# Atlas/Elysion port - Phase A6 review (Lanox -> Atlas crossing + Fields 01/02)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A6 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A6 done (its Done note and commit; the user's A6 play-test can run before or after) |
| Flags | `SERV_IRUHADEV_ELYSION_PORT` (portal gates, `FieldWarpByCommand`, the NULL-world guard), `SERV_IRUHADEV_OFFLINE_BF_JOIN_START_POS` (emulator join ACK) |
| Log tag | `[ELYDBG]` (A6 added no log lines; evidence is `offline_server.log` `ROOM battlefield` / `FIELDNPC` lines, `State.log`, and the screen) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Topology (June)" "Phase A6 — Lanox→Atlas crossing + Fields 01/02" "Phase A6 — Done"
awk -F'\t' '$11 ~ /A6/' june2017_re/region_reach.tsv
python -I june2017_re/region_census.py --line FLD     # the gate; A6's Done note routes every open row
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review A6's commit against June (addresses in each `#ifdef` block and the Done note). The things A6 could get wrong:

- **Portal gates** (code events 2260 / 2261 / 2262, inert offline):
  - `IsNearPortalLineMap` `32d53810`, including June's NULL world / line-map early return.
  - `CreatePotalParticle` `32d51be0`: Ghidra's `32d51960` carve is wrong, so use `funcdis.py`. Check the world 637-or-(608, 103) test,
    the `continue` that skips the particle, the 2261 branch, and the second particle `UI_Portal_Arrow` from `g_pData +0x7c` at +50.
  - `PhysicProcess_PortalInBattleField` `3201a670`.
  - Judge which-is-which for `3201a2b0` / `3201a670` (A6: `3201a670` is the battlefield twin, only the teleport effect). Also judge
    A6's routing of the dungeon twin's other June changes to june-engine W2.
- **`FieldWarpByCommand`** jump table @31d71550:
  - positions 27-30 / 35 / 38 against the decoded ids;
  - the omissions: 31 = 40028; 36 / 37 / 39-41 raid; 42 / 43 deferred to E3, because a field with no row loads no world.
- **Emulator join ACK** (`Handlers_Room.cpp`): echoing the flag + position value matches the live servers
  (`GSUserBattleField.cpp:466`, `BattleFieldRoom.cpp:1065`).
  - Check what it changes for March / Lanox fields: start-pos key vs line index.
  - Check that the 3-arg "custom position" REQ still gets its `false`.
- **Data:**
  - `BattleFieldData.lua`: the F04 portal and direction block (June :1621-1626, :1641-1650), the F01 / F02 rows (June :2061-2157), and
    F02's L142 omission comment.
  - `NPCTemplet.lua`: the 26 rows, against June's line ranges in the block header.
  - `WorldData.lua` 638 / 639.
  - `DLG_UI_FIELD_MAP_Ranox_04_NEW.lua`: June's text plus one comment.
  - The 8 adopted June Lua files and the 338 assets (stage-june record `june2017_re/june_adopt_20261017_060423.tsv`; asset plan in the
    Done note).

Hygiene:
- `python tools/flagcheck.py ELYSION_PORT --range <A6 commit>^..<A6 commit> --files <A6's code files>`, and the same for
  `OFFLINE_BF_JOIN_START_POS`. `..HEAD` times out. Paste the `files:` / `in this flag's files:` / `RESULT:` lines.
- `python tools/luasrc.py ledger --check` and `core --check`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the
same flag (then `tools/build.py --deploy`, and repack / ask before copy for data); hand design decisions back to the user. Write
"Phase A6 — Review" into the plan doc, update the A6 STATUS row, and commit.
