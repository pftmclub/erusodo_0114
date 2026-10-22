# June dungeon result - Phase R3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-result R3 review` |
| Plan doc | `JUNE_DUNGEON_RESULT_PHASES.md` (branch `main`) |
| Depends on | Phase R3 done (play-tested 2026-10-22) and Phases R3b + R3c done (their fixes are reviewed here too; R3c: `kickoffs/june-result/R3c.md`) |
| Flag | `SERV_IRUHADEV_JUNE_RESULT_FLOW` (debug lines under `SERV_IRUHADEV_JUNE_RESULT_DEBUG`) |
| Log tag | `[JUNE_RESULT] FLOW` (`python tools/logs.py --tag JUNE_RESULT`) |

## Read first

```sh
python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "Scope fence" "Phase R3 - The in-state result flow" "R3 - Done (2026-10-22, Opus 5.5 high; flag \`SERV_IRUHADEV_JUNE_RESULT_FLOW\`, log \`[JUNE_RESULT] FLOW\`)"
grep -a "(Result R3)" june2017_re/funcmap.tsv
```

Then `kickoffs/june-result/R3.md` ("Amended by R0 / R2 / R1"), and JUNE_BUGS rows 408-410.

## Task

Review Phase R3's commit and R3b's fixes (plan section "Phase R3b - R3 play-test fixes (user, 2026-10-22)" and its Done note) against June. The things this phase could get wrong:
- **The mode machine `324cf3d0`:** the mode-1 order (0.1 s wait as a double compare, tutorial branch, UI 8 open, no info, the once flag
  `+0x7cc`, the per-tick hide and quick-slot show); the mode-3 countdown (`_ftol` of limit - time, the `fcompp` sense) and the
  speed-clear block; mode 4's order. March adds one degrade (no GFx manager / `main.gfx` -> confirm): check it is the only addition.
- **`0x1f2` = `EGS_REPORT_HACK_USER_NOT`** (the R3 correction of R0 / R2): spot-check June's drop manager `3273b950 @3273b9e3` and
  `g_pKTDXApp +0x218` = `GetFindHacking`; `CX2User +0x54` = `hackingUserType`, `+0x2c` = auth. Decide whether R2's now-unused
  `EGS_JUNE_FAST_SOLO_CLEAR_NOT` id and its ignore row go (that is R2's flag: a review fix or a Z item).
- **The exit box `31f1dd70` / `31f17e60` and the custom msgs 0xbdb / 0xbfe / 0xbff (`31f23e70`):** which box for party / solo, the OK /
  Cancel ids, the flagged-only mode (+0x110) on at create and off at Cancel / state destructor, the text ids by state.
- **The ACK `324d4cd0`:** the `+0x22d` / `+0x22e` / `+0x5db` order; game vslot 0x12c = `Handler_EGS_LEAVE_ROOM_REQ` and the reason
  0x72 = `NOT_LEAVE_ROOM_REASON_00`; `324334d0` = `Handler_EGS_LEAVE_PARTY_REQ`.
- **Retry `324d8600`:** MT 9 = `MT_PARTY_READY_TO_PLAY_DUNGEON_REQUEST` (needs `SERV_PVP_NEW_SYSTEM` on; check March's enum), the
  ok / cancel / time-out ids 14 / 15 / 15, June's extra `( 230, 200 )` argument to `31d94a20` that March's `AddTimedMessagePopup` lacks.
- **The `#else` path:** `CX2DungeonGame::Handler_EGS_STATE_CHANGE_RESULT_NOT` and the state end handler; March's `StateChangeResultReq`
  timer stays (R0).

Hygiene is one command: `python tools/flagcheck.py JUNE_RESULT_FLOW --range <R3 commit>^..<R3 commit>` (memory
`flagcheck-range-to-head-times-out`). Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row
(Always.h's peer block if present) and every deleted line; a FAIL or ERROR is a finding.

Read the play-test lines: `python tools/logs.py --tag JUNE_RESULT`. Spot-check 2-3 items directly in Ghidra (read-only). Fix clear
transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Decide whether the
`_DEBUG` flag and the console opener retire now (R1 review kept them until this review). Write "R3 - Review" into the plan doc,
update the R3 row in STATUS.md, check the R4 kickoff, and commit.
