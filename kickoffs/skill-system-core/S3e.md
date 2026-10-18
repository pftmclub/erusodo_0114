# June skill system core - Phase S3e - The state buff-factor path and the state-string timer

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3e` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S3d (slot 476 is real). The memo part of the play-test waits for S7 (activation) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_STATE_BUFF` (+ one for the state-string timer if it splits; `SERV_IRUHADEV_JUNE_SKILL_DEBUG`) |
| Log tag | `[JUNE_SKILL] MEMO` / a new sub-tag for the timer (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S3e-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S3d - June skill memo consumers"
```
Then the funcmap rows tagged `(S3d)` whose status is `S3e` (`grep "(S3d)" june2017_re/funcmap.tsv | grep S3e`).

## Task

Added by S3d (2026-10-18): June-changed functions next to the memo consumers that S3d found and no phase owned
(memory `plan-unowned-june-branches-without-asking`). RE each in Ghidra and port June's whole function (no partial updates):

1. **`322a6960` SetBuffFactorToGameUnitByBuffFactorID.** June merges March's `SetBuffFactorToGameUnitByBuffFactorID` and
   `...AndCreatorID` into one function: (BFI, creator unit, level, adjust skill id, skip flag). For a user, unless skip, it calls slot 476 with
   the adjust id, or with unit `+0x9274` when that id is 0 (`@322a6a47-322a6a72`), then slot 0x234 (March `OnBuffAccumulationFactorReady`,
   with 0). Then `32d4d670` with the creator's UID / is-user, and slot 137. March has 88 call sites of the two functions: decide the
   signature (default arguments, or June's single function) and keep every caller's behaviour.
2. **`31fe32d0`** (June's `ParsingBuffFactorID`, called by CommonStateStart `@3206069b`). Compare it with March's (`X2GUUser.cpp`,
   `BUFF_FACTOR_ID_AND_APPLY_TIME`); it writes `+0x9274` = slot 0x524 (`GetNowStateSkillID`) at `@31fe3551` / `@31fe3796`, the fallback in item 1
   and in `322a7660` (March `SetBuffAccumulationCount`'s callers pass it today). Port the member and June's whole function.
3. **June's state-string timer.** `32079be0` (CX2Game, string + float) stores the float at `+0xb3c` (callers `3200ab60 @3200ac34`,
   CommonStateStart `@320584bb` with 0.0). `32308c20` reads and counts it down (`@3230a7d3-3230a85d`); `32306600 @323076e6` sets it too. Find what the
   timer does to the state-string display, then port the member, the setter's second argument and the readers.

- **Gate:** each ported site logs once per use (debug flag); flagcheck PASS; build + deploy. Play-test the state string after a skill
  (item 3) and a state buff factor (item 2); the memo fallback of item 1 waits for S7.
- Stop and ask only where copying June breaks March.
