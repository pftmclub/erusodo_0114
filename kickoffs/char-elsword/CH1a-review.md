# June character Elsword (Stage 7) - Phase CH1a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1a review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1a done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final; `_CORE` is defined by the first phase with a `new` row) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1a - Done (2026-11-04)"
awk -F'\t' '$7=="CH1a"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 22 CH1a functions against June in Ghidra (`python june2017_re/charport_adis.py elsword <va>...`; the listing, not the decompile, carries the `StateChange` arguments):
- The real differences from March: Z has no re-attack gate in Wait / Walk / JumpLanding / DashEnd (`m_bReAttackZ1Right`, `m_fReAttackZ1Time` stay March-only; check no other June function reads them); no cliff-climb branch in JumpDown / DashJump (March's is live); DashJump's both-keys branch; Wait's way-of-the-sword branch (vslot 0x674 = `GetWayOfSwordState`, base `31fc8770`, Elsword override `32cf50f0`).
- Spot-check 3 of the "= March" functions end to end (suggest `32d0e410` DashEnd, `32d0e8e0` PowerLanding, `32cfa360` Leap FrameMoveFuture): branch order, float compares (`fcomp` / `test ah` idioms), constants.
- Leap StartFuture keeps June's missing early return (a NULL deref when no NPC is found, identical in March; not a `JUNE_BUGS` row). `GetNearestNPCUnitUID` is March's until CH1o.
- `june2017_re/char_elsword/ch1_offsets.tsv` (seeded from Chung's, class fields dropped): confirm or correct the Elsword class fields read here (+0x95d8 .. +0x9604 Leap, +0x899d `m_bLeaping`, +0x9598 `m_fReDashJumpXTime`).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE --range <CH1a commit>^..<CH1a commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `/D SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE`. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1a - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1b.md`, commit with explicit pathspecs.
