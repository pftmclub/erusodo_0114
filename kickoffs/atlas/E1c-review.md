# Atlas/Elysion port - Phase E1c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E1c review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E1c done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_ELYSION_VILLAGE` (+ `SERV_IRUHADEV_ELYSION_DEBUG`) |
| Log tag | `[ELYDBG]` |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E1 — Split (user, 2026-10-17)" "Phase E1c — Done"
cat june2017_re/e1_relation_runtime.md
cat june2017_re/c9h5_fieldnpc_onframemove_diff.md
```

Do NOT consult `NEW_VILLAGE_PLAN.md`, `NEW_VILLAGE_RANOX_IDENTIFIERS.md` or the `mods/lanox-offline` branch, and tell any subagent the same.

## Task

Review Phase E1c's commit against June's binary (raw disassembly: `june2017_re/funcdis.py --func`, `disasm_range.py`):
- **TFieldGame relation side**: village NPC creation `323ba5f0` (relation lookup order, result 0 / 1 / other, the code-event,
  comeback and spa-sign gates and their house ids), AddFieldNpc( templet, name ) `323b0980`, replace `323b7ff0` (every row ->
  templet offset, the two verbatim bugs JUNE_BUGS 208 / 218, rot / scale "any part non-zero", the unconditional RenderOption
  copy, the +0x444 push and its free in the dtor `323b8ba0` @323b92a1), remove `323a9ed0`, restore `323b8970` (type 0 rows
  rebuilt from an empty row, state-1 rows re-enabled or built).
- **The NPC loop** in TFieldGame OnFrameMove `323b5970` @323b70cb-323b7475: the !m_bEnable skip, AlwaysShow, the effect-set
  show / hide on `inst +0xdc`, the once-a-second quest refresh and +0x239, the hide-dialog / join-area tests. Was March's
  per-frame GetQuestInfo the only other thing the loop did?
- **Entry points**: the code-event refresh after the two code-event sets (31f00970 @31f00a61) and the per-quest refresh after
  each `RemoveUnitQuest( q, true )` (323e57e0 @323e5903, 323e1710 @323e1864).
- **OnFrameMove** `325cdfa0`, block by block against the diff file: the load gate / fail paths (2-8), the effect-set play (4),
  block 14's exclusions, 15 / 18 (the 4-arg SetShopType args 1, b, 0, 0), 19's friendship gate, 20's texture houses and
  bOnlyClickable, 22's ApplyQuestConditionMessage( 0 ), 25's states 3-8 / 1 and 27's state 2, the motion jump table
  (325d1024 / 325d1034: ids 1, 2/3/10, 11, default), the static TalkWait / Normal path (Play 0 / 2), block 42's id choice.
- **Effect set**: `PlayEffectSet( name, CX2TFieldNpc* )` `321e26d0` (no hyper / power / life writes) and the four readers
  (@321dd1a3 GetEffectPosition, @321dd9dc ByBoneName, @321dc8af GetCombineMatrix, @321eebf3 create gate), each in its else-chain
  position and without SetEffectSetOffset.
- **Settle**: premium buff manager +0x5d is read as `IsComeBackUser()` (buff port Phase 11 review), not E1a's "PC bang" guess.
  Confirm from 326c7a10 / its callers which one it is.

Hygiene is one command: `python tools/flagcheck.py SERV_IRUHADEV_ELYSION_VILLAGE --range <E1c commit>^..<E1c commit> --with SERV_IRUHADEV_ELYSION_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line.

Spot-check 2-3 items directly in raw disassembly; if one fails, re-check the whole phase. Fix clear transcription errors under
the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase E1c — Review" into the
plan doc, update the STATUS row, write `kickoffs/atlas/E2.md` if it is missing, and commit.
