# June skill system core - Phase S8 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S8 review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S8 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_STATE_PTR` (+ `_DEBUG`) |
| Log tag | `[JUNE_SKILL] PTR` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S0b - Slots, pages and UI RE" "Phase S8 - Running state data: June's pointer model"
```
Then the phase's Done note (`python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md --list` names it).

## Task

Review Phase S8's commit(s) against the S0a / S0b map it used: every Now / Future reader converted, no pointer held across a vector reallocation, the snapshot members and four consumers against June, the gate; the flag-off path is March's by-value copy.

Added by S8 (2026-10-19): the commits are `387f4d85` and `7ee961df` (the ctor seed: March's `Init()` of the copies sits in `ReInit`, June seeds in both
`3203e850` and `32022380`). **The snapshot could not be play-tested** (user accepted): June's busy-state guards refuse every mid-skill re-equip, so the
`PTR snapshot taken / consumed` lines never ran. Compare `JuneSnapshotRunningStateFunc` with `31fd2af0`, the four consumers with `320347d0` / `31fe6a40` /
`31fd6b80` / `3203c3a0` (order: snapshot, entry delegate, Common; the clear points) instruction by instruction, and confirm the engine-side call
(`JuneSnapshotBeforeSlotRebind`) picks the same entry as June's slot-493 functions. Also check every other early reader: nothing may read the iterators
before the ctor seed, and every `ReInit` caller keeps June's reset.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first phase commit>^..HEAD --with SERV_IRUHADEV_JUNE_SKILL_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "Phase S8 -- Review" into the plan doc, update the
phase's STATUS.md row, write or check the next phase's kickoff file, and commit.

**`SERV_IRUHADEV_JUNE_SKILL_DEBUG` (carried from the S7e review, 2026-10-19):** retire it only once a play-test has shown the S3d
`[JUNE_SKILL] MEMO effectset` / `damageeffect` / `attack box` lines (a class whose memo skill plays). Then remove its define and every
`#if defined( SERV_IRUHADEV_JUNE_SKILL_DEBUG ) ...` block (`git grep`). If they still have not run, say so and leave it to Phase Z.
