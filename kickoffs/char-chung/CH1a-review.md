# June character Chung (Stage 7) - Phase CH1a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1a review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1a done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1a - Done (2026-10-31)"
awk -F'\t' '$7=="CH1a"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

Review the 29 CH1a functions against June in Ghidra (`python june2017_re/charport_adis.py chung <va>...`; the listing, not the decompile, carries the `StateChange` arguments):
- The six real differences from March: Wait's `ET_NONE` emotion test; the +0x8bf9 down-force latch (`m_bDownForcePressedInState`: JumpUp / JumpDown / DashJump / the two Start functions); the `CanUseCannon` inline with `RNSI_P_CIP_OPEN_MIND` in Guard and Guard X; Guard X's `UseCannon( 1 )` gate; Guard X's `IsOnSomethingFuture()` on the walk / dash cancels; DashJump's both-keys branch.
- Spot-check 3 of the "= March" functions end to end (suggest `32bf67e0`, `32bef6f0`, `32bf6d30`): branch order, float compares (`fcomp` / `test ah` idioms), constants.
- The new name `JumpUpStart` (CH0 proposed `CSI_JUMP_UP_StateStart`): CH1m registers it.
- Whether +0x8bf9 belongs in `CX2GUUser` (check other June classes' readers before moving it).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE --range <CH1a commit>^..HEAD`; `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `/D SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1a - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1b.md`, commit with explicit pathspecs.
