# June skill system core - Phase S3d - June skill memo consumers

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3d` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S3b (the memo tables and lookups). The play-test needs an active memo, so it waits for S7 (activation) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_MEMO_FX` (one per logical change if they split; + `SERV_IRUHADEV_JUNE_SKILL_DEBUG`) |
| Log tag | `[JUNE_SKILL] MEMO` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S3d-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S3b - June skill notes (memos)"
```
Then the funcmap rows tagged `Skill S3b`, `MODS.md` lines 2560-2564 (the C8b memo switch) and the `JUNE_FX unsupported` stubs named below.

## Task

Added by S3b (2026-10-18): the engine readers of an active memo that earlier plans stubbed because March had no memo system. S3b made
`CX2GUUser::GetJuneSkillMemoEntry` (31fdbbb0) real, so the C8b switch (3202ac40) already runs; these are the rest
(memory `plan-unowned-june-branches-without-asking`). RE each in Ghidra, then port June's whole function (no partial updates):

1. **Effect-set memo scale.** `321e1e60 @321e1f51-321e1f95`: a type-2 (MI_HUGE) entry writes v0 into the templet's EffectSetData
   `+0x60/+0x64/+0x68` (a persistent templet write: check whether June ever resets it; JUNE_BUGS if not). Copied into EffectData
   `+0x108` `@321eede8`, multiplied in `GetEffectScale 321dddb0` unless NOT_ALLOW_MEMO (`+0x105`). Replace the two
   `JuneFxUnsupported` stubs in `X2EffectSet.cpp` (APPLIED_SKILL_ID `321e1f57`, NOT_ALLOW_MEMO `321dde5f`).
2. **The switch's unported parts** in `JuneApplySkillMemoToDamageData` (`X2GUUser.cpp`): case 2 `+0x5d0`, case 0xc `+0x4f8`, cases
   0xf / 0x10 (`32089ff0`), case 0x16 `+0x5dc`, the tail's `+0x5e4`.
3. **`AdjustBuffFactorByIndex 32028250`** (user slot 476, a stub today): memo types 0xf / 0x10 rescale buff factors (`32d417b0`,
   `32d41910`, `32d42000`, `32d41820`).
4. **vslot 471 `31fdbe20` / `323511f0`** (the alternate templet and MP rate, see the `GetActualMPConsume` comment in `X2GUUser.cpp`).
5. **`32024ff0`'s list variants** (`EFFECT_SET_LIST_EX_TRANS_1 / _2 / _EX_NO_TRANS`, picked by the memo entry): engine or checklist
   material? Checklist row 390 names them; decide and route.
6. **`CommonStateStart 32058444`**: the alternate skill from `GetJuneTranscendenceMemoSkillID` (31fdbcc0, ported by S3b) when non-zero.
   Check March's CommonStateStart port and wire it.
7. Map the other non-class callers of 31fdbbb0 / 31fdbcc0 / 32451250 / 32452150 (`320284c0`, `31ff27c0`, `3230c1a0`, `3220d2b0`,
   `3235b070`, `3235b9e0`) and route each (port here, S7, or checklist).

- **Gate:** each ported site logs once per use under `[JUNE_SKILL] MEMO` (debug flag); flagcheck PASS; build + deploy. The play-test
  (an active MI_HUGE memo scales its effect; a transcendence memo swaps the skill) waits for S7's activation path, or for a dev
  console hook if the user wants one earlier.
- Stop and ask only where copying June breaks March.
