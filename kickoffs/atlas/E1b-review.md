# Atlas/Elysion port - Phase E1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E1b review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E1b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_ELYSION_VILLAGE` (+ `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E1 — Split (user, 2026-10-17)" "Phase E1b — Done"
cat june2017_re/e1_createfieldnpc_diff.md
cat june2017_re/e1_talkbye_diff.md
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review Phase E1b's commit(s) against June's binary (raw disassembly: `june2017_re/funcdis.py --func`, `disasm_range.py`):
- **CreateFieldNpc** `325ca980`: the three-way position choice and join area (325caf3e-325cb0bb), the 6-id static list, the
  background / foreground mesh paths (skin 2 only inside the mesh-1 gate; MultiTex only on the background path), the name
  override, the flag copies and the SetShopType order (17b), the 2204 block, the motion map keys, the 3-id shadow gate.
- **Talk / bye steps** `325ccf60` / `325ca2f0`: the +0x1a6 guard, the motion block (static meshes fall back to NORMAL, non-static
  do not), the anim speeds, ClearUIUnit( id ) in bye, June's UpdatePartyMenu call (322c5980) in bye.
- **OnFrameRender_Draw** `325d1040` (vtable `334acc6c` +0xc): the RenderParam field mapping (June +0x68 = cullMode, +0xa4 =
  bAlphaTestEnable, +0x4c..+0x60 = texOffsetStage0-2), bApplyAlphaTest skipping the RenderOption, the 2700 / 40147 name gate,
  skin 2's AlphaBlend2ndMesh / NoEdge2ndMesh.
- **Helpers**: SetCanTalkNpc `325c8250` (4 houses, DeActive plays only when the anim exists), SetShowField `325c9130`,
  SetShowObject `325c9000`, ResetNpc `325c8ce0`, 325ca920, SetEnable `325ca950`, 325c95e0, NpcPlaySound `325c9c00`,
  RenderName `325ccb40`, ctor `325d13e0` / dtor `325cd7c0`, InitParticle `325c84b0`.
- **325ca0b0 / 325ca1b0**: the row test (STATE == arg, text non-empty, TYPE 0), GetUnitQuest / GetSubQuestTemplet, and whether
  June's sub-quest flag byte +6 really is March's `m_bIsSuccess` (+5): find June's SubQuestInst layout (its
  `SetKSubQuestInstance` or `IsComplete`).
- **UIUnitManager** `32806b30` / `32806910` / `32806800` and `32ee37a0`.
- Not diffed by the phase (check whether June changed them): GetQuestInfo `325c81a0`, ResetNpcType, ChangeType,
  UICustomEventProc, InitQuestMark.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_ELYSION_VILLAGE --range <first E1b commit>^..<last> --with SERV_IRUHADEV_ELYSION_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line.

Spot-check 2-3 items directly in raw disassembly; if one fails, re-check the whole phase. Fix clear transcription errors under
the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase E1b — Review" into the
plan doc, update the STATUS row, check `kickoffs/atlas/E1c.md`, and commit.
