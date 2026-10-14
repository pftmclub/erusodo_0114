# June effect completion - C3z-7 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-7 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3z-7 done (no code, verdict + census) and its Altera play-test; best after the C3p review (C3z-7 rests on C3p's port) |
| Flag | none of its own: `SERV_IRUHADEV_JUNE_SKIP_START` (C3p) carries the skip |
| Log tag | `JUNE_FX C3p` (`python tools/logs.py --grep "JUNE_FX C3p"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-7 - skipping a boss intro cinematic (user, 2026-10-10)" "C3z-7 - Done (2026-10-15, Opus 5.5 high)"
cat june2017_re/c3z7_boss_intro_skip.tsv
```

## Task

C3z-7 found no code to write: June's boss-intro skip is C3p's skip-the-start chain. Review the verdict, not a diff.

- **ESC is the only way in.** The verdict relies on C3p's raw sweep of game `+0xb40..+0xb48`. Spot-check in Ghidra (read-only) that no
  other June reader changes an event NPC out of a `DIRECTION_STATE` START state on input, e.g. the dungeon-game key handling around
  `CannotInput` `327d20b0` and the `IsSkipKeyPressed` (`@31dc90c7`) callers. If one exists, it is a port item: write it up for a phase.
- **Release parity.** Confirm that June has no extra release on the skip path that the natural `SCT_MOTION_END` end lacks (camera /
  `STOP_AT_CURRENT_STATE` reader, cinematic UI), i.e. that `322f11d0`'s `StateChange( WAIT_STATE, false )` is the whole job.
- **Census.** Re-run `python june2017_re/c3z7_boss_intro_census.py` and spot-check 2 rows against `june_src_fixed`. Read the rows from
  `NPC_GROUP`, not `READY_NPC`. Check that the `no_direction` row (`EVENT_FINALHALL_BOSS2`) really sets no `DIRECTION_STATE`.
- **Play-test read.** Read the Altera Battle Air Ship (30240) STAGE3 run: `JUNE_FX C3p arm` names `Event_NPC_Battle_Air_Ship.lua`; ESC ->
  `skip ... -> WAIT_STATE`; `end ... input unlocked`; Raven fights (hits on his UID afterwards); 0 EXCEPTION.

There is no C3z-7 code, so there is no flagcheck range. If the review finds a gap, port it under a new `SERV_IRUHADEV_JUNE_BOSS_INTRO_SKIP`
(`#else` = March), then `tools/build.py --deploy`. Hand design decisions back to the user. Write "C3z-7 - Review" into the plan doc, update
the C3z-7 STATUS row, and commit with explicit pathspecs.
