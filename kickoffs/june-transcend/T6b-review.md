# June transcendence - Phase T6b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-transcend T6b review` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | Phase T6b done (its Done note and commits) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_TRANSCEND_SLOTBUFF` (+ `_DEBUG`, the proof lines) |
| Log tag | `[JUNE_TRANSCEND] SLOTBUFF` (`python tools/logs.py --grep "JUNE_TRANSCEND] SLOTBUFF"`) |

## Read first

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Phase T6b - The transcendence slot buffs" "T6b - Done"
grep -n "Transcend T6b" june2017_re/funcmap.tsv
```

## Task

Review T6b's commit(s) against June:
- `322d8100` (`CX2Game::June_TranscendSlotGuideFrameMove`): the early `return` on `IsJuneSlotUsable( 1 )` / B / `m_iUnitClassTran`, the type A (`+0x116`) / swap (`+0x115`) bar test,
  the skill-type test (12 / 13 against March's `ST_RELATIONSHIP_SKILL` / `ST_HYPER_ACTIVE_SKILL` values), `GetRemainHyperModeTime() > 0`, `HaveUnfinishedBuff( 820 )`,
  `JuneCanShowClassGuide( 4 )`, the position (half size, swap +-47) and the move test (> 0.1).
- The UI manager table (`32154f60` type 4 names and order, period 9999; `321536e0` timer reset; `32151fa0` timer erase; `32155eb0 @3215631c` `<= 0`; `32153880` dialog test and the
  skipped game-type-5 branch).
- Retire `SERV_IRUHADEV_JUNE_TRANSCEND_SLOTBUFF_DEBUG` and its blocks once the play-test logs prove the buffs (as T5's review did); retire T6's two temporary ARME lines if T6's review has not.

Hygiene: `python tools/flagcheck.py JUNE_TRANSCEND_SLOTBUFF --range <first T6b commit>^..<last T6b commit> --with JUNE_TRANSCEND_SLOTBUFF_DEBUG`, then
`git log -SJUNE_TRANSCEND_SLOTBUFF <last T6b commit>..HEAD`. Paste the `files:` / `in this flag's files:` / `RESULT:` lines. A FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`);
hand design decisions back to the user. Write "T6b - Review" into the plan doc, update the T6b STATUS row, check T7 / T8's kickoffs, and commit.
