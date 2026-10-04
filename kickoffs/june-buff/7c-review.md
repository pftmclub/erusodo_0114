# June buff port - Phase 7c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 7c review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 7c done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK` (+ `JUNE_BUFF_DEBUG` for the `first read BBT 110` line) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Shared checklist" "Phase 7c -- BBT 110 host" "Phase 7c -- Done"
```

The 7c rows in `june2017_re/funcmap.tsv` ("Buff Phase 7c") hold the RE evidence. Rows 11 and 12 of
`JUNE_BUGS.md` and the Add / slot-510 rows of `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7 came
from this phase.

## Task

Review the Phase 7c commit against June. Phase 7c found that its own kickoff's premise was wrong:
`+0x8d78` is the quick-slot special item wait (armed in `322fa960`), not an awakening lockout. Check
that finding first, then what landed:

1. **The re-attribution.** Confirm that `322faee8`/`322fb40b` belong to `322fa960` and not to
   `322f8920` (`ProcessExpListByNpcUnitDie`). Confirm the 10.0 constant (`3344a51c`), and that case
   18 of the switch is `SAT_SPECIAL_SKILL` (March `CX2UIQuickSlot::KeyEventProcess`, STR_ID_20120).
2. **`TryHyperModeStateChange`** (`31fe9a90`): order and polarity of every test (`oneLCtr`, 345,
   434/435, BlackHole, `timer <= 0`, 344), `StateChange( state )`, and the flag write. The
   `+0x7e84` write was left out on purpose (Add-only branch, member not identified). Say whether
   that holds.
3. **The macro switch.** `ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE` now calls 343 at all 52 live
   March sites. Recount June's slot-343 callers per class against March, including Eve's file
   `X2GuEve.cpp` (lowercase `u`, which the phase's first count missed). Sample one site per
   class and confirm the June state calls 343 rather than inlining.
4. **`SKILL_CANCEL_AFTER` / `IF_SKILL_CANCEL_AFTER`.** These now use June's expansion: the skill
   branch gains `IsOnSomethingFuture()` (`+0x7e1c`); the awakening branch uses `CanHyperMode`, the
   item-wait gate and the flag. Check against 2-3 June expansion sites in different classes
   (CX2GUUser `31fcc350`, Eve `3296a1f0`, one more). Confirm `+0x7e1c` is
   `unitCondition.bFootOnLine`, and that June never tests 345 there (`JUNE_BUGS.md` #11).
5. **Ara's six sites:** gate order, no flag write (`JUNE_BUGS.md` #12), and that slot 430 equals
   March's 3-bead `ASI_BERSERK_MODE` choice.
6. **Member homes.** The ctor, the `THROW_ITEM_StateEnd` reset, the `OnFrameMove` countdown
   (inside `IsLocalUnit`, after `PushFrameData`), and the flag clears in
   `HyperModeEnd`/`RidingHyperModeEnd` (`31fe96c0`/`31ffd490`: confirm which is which).
7. **The `#else` path.** Flag off must give March exactly. The three macros are full
   `#ifdef/#else` copies inside `X2GUUser.h`.

Hygiene is one command:
`python tools/flagcheck.py JUNE_BUFF_AWAKE_LOCK --range <the 7c code commit>^..HEAD --with JUNE_BUFF_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding.

Spot-check items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear
transcription errors under the same flag (then `tools/build.py --deploy`). Hand design decisions
back to the user. **Decision (b), the quick-slot arm, is the user's**: do not port it in the review.
Write "Phase 7c -- Review" into the plan doc, update the 7c review row in STATUS.md, record /cost,
and commit with explicit pathspecs.
