# June buff port - Phase 9b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 9b review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase 9b done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION` (+ `_DEBUG`) |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 1c, item 5" "Phase 9a — Done (2026-10-05)" "Phase 9b — Done (2026-10-05)"
```

`june2017_re/funcmap.tsv` rows tagged "Buff Phase 9b" hold the RE. `kickoffs/june-buff/9b.md` is the task 9b answered.

## Task

Review Phase 9b's commit(s) against June (`3229e1a0` and every class it creates). What this phase could get wrong:

1. **Switch arguments.** Each case's ctor arguments and gates (`3229e1a0`, disassembly): 767's two `32007b10` calls run on the creator, not `this` (ability 2 = `SA_EFFECTIVE_COUNT`, `0x94` = `SA_COOLTIME_1` per June's parser `32216320 @3221a37a`); 891 passes the factor by pointer; 952's gate is `+0x264 == 1`; 1010 passes the creator info's power rate.
2. **Destructors.** Servo Nitro `3236f390`, Summon NPC Target `3236f7c0`, Chain Viper `3236f9f0`, Geist Herrschaft `32370000`, Moder `32370300`, Sinister Mind `3236f480`. Each one's guards and calls.
3. **Sinister Mind** (`3236e770`, `3236e490`, power `32369bd0`): the step `ftol( max / 3.0 )`, the never-stored level (JUNE_BUGS #32), the zero-step guard (#33, a deviation the user is asked about), slot 136's arguments `( 794, uid, 1, 1 )`, the dummy reActType on FrameMove's effect only.
4. **Moonlight Rhapsody** `32370430`: trigger 7, the last-nonzero-count scan, the erase only when the count is above 0, the effect-set arguments (hyper from the attacker, custom position = impact point).
5. **New fields.** DamageData `+0x5d4` / `+0x5e0` / `+0x5e8` (SimpleInit values, the two critical reads, the `SetDamageExceptionProcess` tail); unit `+0x40d8` / `+0x40dc` (the serial wrap, both DamageReact stores inside `IsDamagedReaction`); `+0x258` (the `OnFrameMove` placement after the socket cool-time calls, the `ReInit` reset).
6. **`KBuffFactor` copy ctor** (`CommonPacket.h`): member order, and that nothing outside `X2Lib` compiles the header with the flag.
7. **Hooks.** Every Rose / Ain / summon-target call is an empty `CX2GUUser` virtual with June's argument order, and each has its `JUNE2017_CHARACTER_PORT_CHECKLIST.md` §7 row.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_BUFF_CUSTOM_FUNCTION --range <first 9b commit>^..HEAD --with JUNE_BUFF_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check at least three classes directly in Ghidra / `rawfn.py` (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user (the Done note's three open questions stay the user's). Write "Phase 9b — Review" into the plan doc, update the
phase's STATUS.md row, check the next kickoff (`13.md` is the next phase that depends on 9), and commit.
For a play-test, regression only: the BTIs in March's data (29, 56, 62, 223, 241) and ordinary combat (the per-hit serial / state / critical bookkeeping runs on every hit); the new classes are dormant.
