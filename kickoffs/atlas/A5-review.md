# Atlas/Elysion port - Phase A5 review (Atlas Station, rest area 1008)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas A5 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | A5 done (its Done note and commit; the user's A5 play-test can run before or after) |
| Flag | `SERV_IRUHADEV_ELYSION_PORT` (+ the `atlas` chat warp also under `SERV_IRUHADEV_CHAT_WARP_DEBUG`) |
| Log tag | `[ELYDBG]` (A5 added no log lines; the play-test's evidence is ErrorLog.txt load errors and what the screen shows) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Topology (June)" "Phase A5 — Atlas Station" "A1a reachability summary" "Phase A5 — Done"
awk -F'\t' '$11 ~ /A5/' june2017_re/region_reach.tsv
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review A5's commit against June's functions (addresses in each `#ifdef` block and the Done note). The things A5 could get wrong:

- **The two whole-body ports** (`X2StateMenu.cpp` Cobo `InitWarpList` / `CreateWarpDest` 3245a1e0 / 324627f0, `X2UIInventory.cpp` warp NPC
  32166ae0 / 32173120): row order, which LMI gates which rows, where `row` advances (Cobo 1008 + code event 2260 @32463030-32463056), the
  `m_iCurrentVillageWarpIndex` branch (@32463091-324630f2; A5 mapped June's states to March's `AddWarpList` switch: judge that), the
  unchecked-vs-checked controls of the warp-NPC `InitWarpList`, `SetIndex` placement, and that every `#else` is March's body byte for byte.
- **Values read from the binary:** `ResetCameraMatrix` floats (re-read 2 cases with the float reader), `OpenFieldName` picture indices
  (jump tables @323b1600 / @323b1624 against June's `DLG_UI_Field_Title.lua` `SetIndex`), `WarpByCommand` positions (@31d710d4).
- **Data:** `DLG_Map_Village` Village[1008] against June's 4367-4423 (only the two key renames may differ), the 3 house rows + `HI_YUNO`,
  `NPCTemplet` `NUI_YUNO`, `MapData` (client + server mirror: the mirror is byte-level, mixed EOL; `git diff` must show insert-only),
  the world-map block (June 1842-2081 + the 4 `SetShow(false)` lines), the `DLG_Destination_byButton` core row (`--source --lifted`).
- **Omissions:** each "omitted" comment (1500 / 25000 / Cobo spa rows / quest test / `this+0x3c0`) is still true; nothing else of June's
  functions was dropped silently.

Hygiene: `python tools/flagcheck.py ELYSION_PORT --range <A5 commit>^..<A5 commit> --files <A5's 8 code files>` (`..HEAD` times out). Paste
its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row (X2Main.cpp may carry a peer's `ELYSION_SCRIPT_A` hunks)
and every deleted line; a FAIL or ERROR is a finding. Also `python tools/luasrc.py ledger --check` and `core --check`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the
same flag (then `tools/build.py --deploy`, and repack / ask before copy for data); hand design decisions back to the user. Write
"Phase A5 — Review" into the plan doc, update the A5 STATUS row, check `kickoffs/atlas/A6.md` / `A7.md` exist (write from the plan if not;
A7 must keep the Elysion town / F03 world-map pins hidden until E2 / E3, see the A5 Done note's open item 2), and commit.
