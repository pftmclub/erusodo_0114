# June character Chung (Stage 7) - Phase CH1l2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1l2 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1l2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` / `_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1l2 - Done (2026-11-01)"
grep -a "Char Chung CH1l2:" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH1l2 --out <scratch>     # June listings, 9 rows
```

## Task

Check each CH1l2 transcription against June in Ghidra (read-only). In order of risk:
- **`CommonStateStart` `32bf3990`:** the 12 skill ids behind the jump table (`32bf3c4c` dwords, `32bf3c54` byte index, base 26011, 217 entries; index 0 = the 12 ids, 1 = default); that the id compared with `+0x98ec` is the raw state skill id and the switch is on `32207210`'s result; the call is `SetSkillBuffFactors( 26417, 1, true )` (pushes `1, 1, 0x6731`). Is `+0x8b10` = `+0x9814` really the passive critical rate (`31fd19b0` adds `+0x8b10` to the critical rate) and `+0x3cc` / `+0x790` = `m_bMeleeAttack` / `fForceDownValue`? State 163 = `CSI_GUARD`? `32c24c80` = the used-count setter of `m_CannonBallSystem`?
- **The `USE_CANNON_BALL<n>` loop** (`@32bf3ae1`-`@32bf3c34`): key `"%s%d"` with n before the increment, `[1]` float and `[2]` int, the skipped `EndTable` on a failed read (JUNE_BUGS #499), `clear()` = `end = begin`.
- **`CommonFrameMoveFuture` `32bf2a40`:** now `+0x110` plus 7 x extra `+0x128` only for class 0x75 with `+0x97a0 == 1`; the destination is `+0x7ec0` (`iClassSyncInt`, Future), not the wire field; the base call comes first.
- **The four bare jumps** (`32bd8740` `32bd8750` `32bd8760` `32bd8770`): targets `320506f0` `31fdd8e0` `32015250` `31fd07b0`.
- **Routing:** the `GetTotalCannonBallCount` row (`32bed560` is not March's), CH1q / CH1l4 amendments.

Hygiene: `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --range <CH1l2 commit>^..<CH1l2 commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUChung.*`, both one-file probes (`-D`, not `/D`, from Git Bash). Write "CH1l2 - Review" into the plan doc, update the STATUS row,
check `kickoffs/char-chung/CH1l30.md` and commit with explicit pathspecs.
