# Atlas/Elysion port - Phase E1d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E1d review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E1d done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_ELYSION_VILLAGE` (+ `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E1 — Split (user, 2026-10-17)" "Phase E1d — Done"
grep -a "Atlas E1d" june2017_re/funcmap.tsv
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review Phase E1d's commit against June's binary (raw disassembly, not only Ghidra's decompile):
- **VillageParsing** `324ff4b0`: every June-only key's offset and miss default (CameraModifyDistanceByPortal @324ffa18, SPA_INFO
  @324ffa22-324ffcc7 incl. a row kept without LineIdx, the five START_POS keys @3250046d-32500d72), the duplicate-StartPosId free
  @32500db1, and whether anything else in June's body differs from March's (the `NAME` branch @3250167d is noted as not ported:
  check it is display-only).
- **Portal block** in `323b5970` (@323b6d1d-323b6fd5): the -1 reset, the 2000 / camera / 1000 / 100 tests (`a < b != (a == b)`
  is `<=`), the spa test on the location manager's current village, the place ranges, and the timer write (323aa1b0).
- **MoveToOtherPlace** `3250a800`: return values on every path (internal place returns 323ac300's AL; the IsWaitingServerPacket
  path skips +0x22f but keeps the sound), the packet (0x21f = `EGS_STATE_CHANGE_FIELD_REQ`, `m_bUseWarpItem` false), the 3.0 ACK
  time, `GetIsRight` 324f2030.
- **323ac300 / 323aa170 / 323aa1b0** and the ctor writes (+0x2f0 = -1, +0x358 from templet +0x58).
- **GetVecLinkedPos** `324f2ff0`: the six flags and the code 2319 / 40037 gate.
- March's other readers of the old bool (`X2TFieldGame.cpp` market block, `X2UISkillNote.cpp:544`, the four
  `SetNearPortalToBattleField( false )` sites): confirm June's twins of the two `CX2State` JOIN_BATTLE_FIELD sites also clear the id
  (the phase confirmed only the two StateField ones, 325091d0 / 32509660).

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_ELYSION_VILLAGE --range <E1d commit>^..<E1d commit> --with SERV_IRUHADEV_ELYSION_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line.

Spot-check 2-3 items directly in raw disassembly; if one fails, re-check the whole phase. Fix clear transcription errors under
the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase E1d — Review" into the
plan doc, update the STATUS row, and commit.
