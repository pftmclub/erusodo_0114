# Atlas/Elysion port - Phase E5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E5 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E5 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_MOVABLE_LINE` (+ `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` (`python tools/logs.py --tag ELYDBG`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Elysion half: engine gaps" "Phase E5 — Moving platforms" "Phase E5 — Done"
grep -n "(E5)" june2017_re/funcmap.tsv
grep -n "atlas E5" JUNE_BUGS.md
```

Use the ghidra skill (read-only). The June strings are XOR'd: `june2017_re/xorstrings.tsv` rows for 31c75470 / 31c705c0 / 31c77e20.
Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review E5's commit against June in raw disassembly, function by function (the funcmap `(E5)` rows name each address):
- **Record 0xb0** (ctor `31c7f2b0`, Reset `31c7f170`, dtor `31c7edc0`): every offset the port names, the defaults (LOOP 1, IS_RIGHT 1).
- **`AddMovableLineData_LUA` 31c75470:** key order and defaults, which misses return silently and which log, the uint LINE_ID / follower test,
  the type branches (@31c75a8e = SPIN, @31c7615e = PATROL, @31c7660a = TARGET_MESH), the mesh/bone empty test (31c7d460 / 31c7cfb0 on
  3344f1f4 = ""), `31c7e560` emptying both names before the follower copies, map insert (no overwrite).
- **Movers `31c6fbb0`:** the patrol `w > time` test and the overshoot drop, LOOP false -> UINT_MAX, the spin angle's float truncation points
  (@31c6fd15-31c6fd3d), the RotationAxis / TransformCoord argument order, +0x64 = the new centre. `31c751b0`: yaw source (31c19930 `.y`),
  FLT_EPSILON test, RotationY, w sign. The D3DX names come from the June `d3dx9_43.dll` export table (base 0x61200000).
- **CanDown head (31c6c310):** the June order (carry test before the radius drop) changes every `CAN_DOWN = FALSE` line's out position in March
  maps too; confirm in June that the drop really comes after, and that no other June code path compensates.
- **World (32802140, 32801ab0, 327fe640, 328018c0, 32801fb0, 327fca00):** the per-object call site and its "shown or not" scope, +0x1f8 =
  `m_XMeshName` / `m_XSkinMeshName` (+0x19c = the object name), +0x218 / +0x220 (anim / motion mesh), host test, 1.0 s timer, packet layout
  (0x14 bytes), the type filter on receive.
- **Game / NPC:** P2P case 0x1c @322edf99 (no world = false), `GetLineTypeByLineID_LUA` 322d0800 and its binding, camera land height 322cfe20
  (camera +0x3c = `m_fLand`, g_pKTDXApp +0x1fc = the DG manager), the NPC hit-stop branch @3277e75b (+0x8470 sync line, +0x8ae4 =
  `m_fAnimWaitTime`, 0.1f).
- **JUNE_BUGS 230-233** are right, and 230 is a June bug rather than a port gap (decide what E10's play-test must look at).
- **Data audit:** every key the five June line maps (Adrian Stage2, Soless Stage0 / 1 / 1_Hide / 3) pass to `AddMovableLineData_LUA`, trigger
  case 8 and `CAMERA_LAND_HEIGHT` is read by the port (E5 checked it by script; re-run it).
