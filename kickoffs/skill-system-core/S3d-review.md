# June skill system core - Phase S3d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3d review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S3d done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_MEMO_FX`, `_MEMO_DAMAGE`, `_MEMO_BUFF`, `_MEMO_MP`, `_MEMO_STATE` (+ `SERV_IRUHADEV_JUNE_SKILL_DEBUG`) |
| Log tag | `[JUNE_SKILL] MEMO` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Phase S3d - June skill memo consumers"
```
Then the funcmap rows tagged `(S3d)` (`grep "(S3d)" june2017_re/funcmap.tsv`) and JUNE_BUGS rows 289-293.

## Task

Review Phase S3d's commit(s) against June's binary (read-only Ghidra; `june2017_re/disasm_range.py` / `funcdis.py` where Ghidra has no function):
- **Effect sets:** `321e1e60 @321e1f51-321e1f95` (lookup with no zero test, type 2, `*begin`), `321df230 @321df495 / @321df57c`, `321ee600 @321eede4`
  (copy before `321df7e0` and `321dddb0`), `321dddb0 @321dde5f` (the `+0x105` test and the multiply order), the EffectData default (`321f4a20 @321f4ba5`).
- **CEffect:** the ctor step `31f7aab0 @31f7c795-31f7c8db`. Check the `[esp+0x27]` IGNORE_MI_HUGE branch, the life multiply order, the `+0x3bc` / `+0x3c0` /
  `+0x1fc` walk and the `cmp [eax+0x20], ebx` with ebx = 0, which S3d reads as CT_LINE (CT_GUARD = 2 in `32628890`). Then `31f6fdc0`'s base multiply.
  Decide whether the ctor's position in March (after the DAMAGE_DATA parse, `X2DamageEffect.cpp`) matches June's order relative to the main effect's creation.
- **Switch `3202ac40`** cases 2 / 0xc / 0xf / 0x10 / 0x16 and the tail order; `32089ff0` (the Rate fields, the `bApply` test, the finalizer and behavior halves).
- **Slot 476 `32028250`:** the argument order (skill id, factor), the `-1 < id` test, the x87 at `@3202831c-3202833f`, and the two new callers.
- **MP:** `31fd1840` / `3203cf60` with `323511f0` (my unit's), vslot 471 `31fdbe20` (types 3 / 7 / 8), and the multiply order `@3203cfca-3203cfeb`.
- **CommonStateStart `@3205841f-320584bb`:** slot 330 with 1 = the future state. Confirm `GetFutureStateID()` is the right March source at that point.
- **Timed attack `31ff27c0 @31ff2a04-31ff2c4a`** and `32274310` (FLT_EPSILON, both sets, no type test, the name filter).
- `SERV_IRUHADEV_JUNE_SKILL_DEBUG` (its Always.h comment says "remove after the S2 review") now also carries the MEMO lines: decide keep or retire with S7's play-test.

Hygiene is one command per flag, not a fresh script: `python tools/flagcheck.py <FLAG> --range <S3d commit>^..<S3d commit>`
(plus `git log -S<FLAG> <commit>..HEAD`). Paste the `files:` / `in this flag's files:` / `RESULT:` lines and explain every OTHER row.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Phase S3d -- Review" into the plan doc, update the S3d STATUS.md row, check `S3e.md`, and commit. The play-test waits for S7 (memo activation).
