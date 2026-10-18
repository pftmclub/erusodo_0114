# June skill system core - Phase S3e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3e review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S3e done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_STATE_BUFF`, `SERV_IRUHADEV_JUNE_SKILL_STATE_STRING_TIME` (+ `SERV_IRUHADEV_JUNE_SKILL_DEBUG`) |
| Log tag | `[JUNE_SKILL] SBUF` / `SSTR` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Phase S3e - The state buff-factor path and the state-string timer"
```
Then the funcmap rows tagged `(S3e)` (`grep "(S3e)" june2017_re/funcmap.tsv`) and `june2017_re/s3e_bybfi_callers.tsv` (part 1: engine sites).

## Task

Review Phase S3e's commit against June's binary (read-only Ghidra; `june2017_re/disasm_range.py` where Ghidra has no function):
- **`322a6960`:**
  - The argument order (BFI, creator, level, adjust, skip). S3e read it from the push order at `@322a6a1a` / `@322a6a4e` and the 5-push call shape.
  - The user test `+0x264`, the skip byte, and slot 476's argument order (`@322a6a5c-322a6a72`: the first push is the reserved ptr slot).
  - Slot 141 runs for every user, skip or not.
  - The creator stamp: -1 / true without a creator.
  - Confirm `322a6b60` (`...AndCreatorID`) really is unchanged.
- **The defaults.** S3e kept March's `( BFI, level )` order and defaulted skip to **true** (March's behaviour). Check the five engine sites it changed to skip 0 against
  `s3e_bybfi_callers.tsv`, including the inferred `ebx` = 0 in `32034e50`. Spot-check two rows it left at the defaults.
- **`31fe32d0` vs the new `ParsingBuffFactorID`:**
  - the `_ULTIMATE` / base table fallback;
  - the row parse (EndTable before the 0 check);
  - `+0x9274`'s two writes;
  - `BUFF_FACTOR_RELATIVE_SKILL_ID`'s -1 rule, `32890620( id, 0, 0, 1 )`, and the learn check (`32079d60` clear and return);
  - the time-0 loop with skip 0.
  Also check the caller's flag (`32057b80`: `31fca8e0( 0x524, 1 )` at `@32057c03`, `+0x178 > 0` at `@3205ad70`). S3e tracked `[esp+0x40]` by hand across a big function, so confirm it is the value pushed at `@32060693`.
- **`31fdf4c0`** (time <= anim +0x214, vslot 0x830 after 0x21c) and **CommonStateEnd `@32015284-32015304`** (every row, no time test, before `JuneClearStateSounds` `@320153c7`).
- **Timer:**
  - `32079be0` writes the timer first;
  - `32308c20 @3230a7d1-3230a87b` is the x87 order (strictly > 0 to count; <= 0 to clear);
  - its placement after March's `GS_PLAY` block (the only bracket is the `+0x2d8` early return `@32308dd3`);
  - the ctor init `@323076e6`;
  - the helper `3200ab60` (3.0f `@3347209c`).
- **Not ported, decide:** the NPC `ParsingBuffFactorID` / frame apply (`X2GUNPC.cpp`; June `32761b40` applies its time-0 rows inside the frame move). Were they changed in June? If so, give them a phase.

Hygiene is one command per flag, not a fresh script: `python tools/flagcheck.py <FLAG> --range <S3e commit>^..<S3e commit>`
(plus `git log -S<FLAG> <commit>..HEAD`). Paste the `files:` / `in this flag's files:` / `RESULT:` lines and explain every OTHER row.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Phase S3e -- Review" into the plan doc, update the S3e STATUS.md row, and commit. Read the play-test log first if the user has run it (`python tools/logs.py --tag JUNE_SKILL`).
