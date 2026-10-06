# June buff port - Phase 16 step 3b, part 4 - review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b4 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b4 done (see its STATUS row for the commits) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (`_DEBUG` for the first-fire lines) |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 8 -- Done" "Phase 16-step3b4 -- Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3b4" carry the decodes.

## Task

Review 16-step3b4 against June `32028ba0` and its callers:
- **The function.** Case by case: the random offset (1-11 = 10..19; 12/13/18 = 0x18/0x19/0x1c + index; 14/15/16 = argument +
  index; 20 = 0x1f; 17/19 none), the order of the roll and the cool-time test (7: cool time first; 10, 11, 12-16, 18: roll
  first), which paths `break` and which `continue` (13's apply path continues, its limit paths break; 17 breaks on HP <= 0),
  the return value per case, and the socket-data offsets in the function's brief (`+0x1e8` is `m_fProbability`, not the
  interval the 3b4 kickoff named). Re-read case 20's state choice (`+0x7e1c`/`+0x7e2d`, riding) and the June -> March state
  mapping (June USI = GUSI_END + n, March = n - 1).
- **The store.** `31fd8b70` / `31fe0b90` / `31fd7bd0` and the tick's place in OnFrameMove (after the bone shield).
- **The callers.** 12 after 11 in DamageReact; slot 252 at the end of AttackResult (`@32014204`) with its two early returns
  and the defender-UID offset; 15 in OnFrameMove after the riding-key wait; 17 at the top of CommonFrameMove; 18 in the die
  states' StateEnd; 19 in UpNowHp / SetNowHp (death gate) and the `+0x8e78` knock-down in CommonEventProcess; 20 at the top
  of CommonStateStartFuture through slot 334.
- **HasSocketAuraFor.** The loop against `3204e380`.

Hygiene is one command:
`python tools/flagcheck.py JUNE_SOCKET_PARITY --range <first 3b4 commit>^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_NEW_FINALIZER JUNE_BUFF_NEW_FLAG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR
is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors
under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user (the Done note's open items:
the effect-set buff list, staging June's SIEE effect sets and buffs, the other users of the cool-time store). Write
"Phase 16-step3b4 -- Review" into the plan doc, update the STATUS row, check `16-step3b5.md`, and commit.
