# June skill system core - Phase S7j review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7j review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S7j done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_MAX_MP` |
| Log tag | `[JUNE_SKILL] MAXMP` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7j - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7j`.

## Task

Review Phase S7j (item 1 only; items 2-4 moved to S7k by the user) against Ghidra (read-only):
- `CX2Unit::ResetMaxMp( base, group )` / `JuneGetMaxMpSkillValue` (`X2Unit.cpp`) against `3234b9d0`: the 19 ids and their order (raw pushes, not the decompile),
  the item term's place after the 7th skill, the getter arguments `32890620( id, 1, 0, 1 )` / `32206450( id, 1 )` / `32206e50( 0x15, level, 0, 0.0 )` vs March's
  `GetSkillLevel( id, true, 0, true )` / `GetSkillTemplet( id )` / `GetSkillAbilityValue( SA_MAX_MP_ABS, level, false )`, and the `31d717d0` (PVE state) skip.
- `CX2Inventory::JuneGetAddMaxMPValue` against `32869f80`: the list (`+0xf4` = `m_ItemMap[ST_E_EQUIP]`? check the inventory layout), the slot-id switch (1 / 3 / 5 / 7 / 9;
  the `short` set 0x14..0x22, 0x24, 0x26, 0x28, 0x2a), the `( param != 0 ) != in-set` skip, item `+8` / templet `+0xc0` / `+0x28`.
- `CX2GUUser::ResetMaxMP` against `31fd2810` (raw `disasm_range.py 31fd2810 31fd2900`): the x87 order ( r + ( r x rate + add ) ), `- 300.0` (`334577c8`), the
  `m_AdditionalMaxMp` calls, `SetMaxMp`; `32358970( 5 )` taken as 0.
- The other callers of `3234b9d0`: `3235a160` (S7k) and `32b0fa90` (type-10 override, no March class).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SKILL_MAX_MP --range <first phase commit>^..<that commit>`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S7j -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file (S7k), and commit.

**`SERV_IRUHADEV_JUNE_SKILL_DEBUG`:** the retire note lives in `S8-review.md`; leave it there.
