# June effect completion - C3t-2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3t-2 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3t-2 done (its Done note and commit) and its play-test read ('check logs') |
| Flag | `SERV_IRUHADEV_JUNE_SOUND_SET_PICK` (+ `_DEBUG`, turn it OFF here) |
| Log tag | `JUNE_FX C3t-2` (`python tools/logs.py --grep "JUNE_FX C3t-2"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3t-2 - identify, then port: June's state sound-set pick"
grep -n "(C3t-2)" june2017_re/funcmap.tsv
```

## Task

Review C3t-2's commit against June `32057b80 @3205db96-@3205e70a` (user CommonStateStart). What C3t-2 could get wrong:
- **The roll.** `+0x6dcc` loaded before the key (`@3205dba1`), the key `USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE` (decode `0x3379d5dc` again),
  the `test al, al` / `cmp [esp+0x3e], 0` polarity (a miss or false rolls), vslot `0x1c8` = `3208e0b0` = March `GetRandomInt( 0 )`, `cdq / idiv 100 / inc`,
  and the store on both paths.
- **The rate loop.** Key built as `"SOUND_SET" + n + "_RATE"` (`3207feb0` = int to string? confirm), the rate reset to 0 each pass, `jl` (signed,
  strict), and the miss path (`@3205e086 -> @3205e10f`) leaving n untouched: no fallback to set 0 (JUNE_BUGS 192).
- **The table body.** Both clears (`+0x85c4` via `[esp+0x2c]`, `+0x85d4` via `[esp+0x14]`: re-check that neither slot is rewritten between
  `@3205da87` and `@3205e136`), the HYPER / BERSERK formulas and their order, `32078fd0` = `JuneBeginSoundPlayTable` for `SOUND_PLAY`, the plain
  `STATE_SOUND_PLAY` loop, the four miss defaults, and the EndTable count on each path.
- **Scope.** The NPC roll (`327ac9d0 @327ae165-327ae564`) was routed to C3t-3; the member lives on `CX2GUUser` for now (June: `CX2GameUnit +0x6dcc`).
- **The `#else`.** March's block (three fixed rates, `rand()`, one clear, the plain `SOUND_PLAY` loop) is intact; the flag-off build is the base.

Read the play-test lines first: each `JUNE_FX C3t-2` line gives the roll (new / kept), the rate sum, the set, whether the table opened, the branch and
the two record counts. Turn `SERV_IRUHADEV_JUNE_SOUND_SET_PICK_DEBUG` OFF (comment the `#define` with a dated reason), rebuild and deploy.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SOUND_SET_PICK --range <C3t-2's first commit>^..HEAD --with SERV_IRUHADEV_JUNE_SOUND_SET_PICK_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag
(then `tools/build.py --deploy`); hand design decisions back to the user. Write "C3t-2 - Review" into the plan doc, update the STATUS.md row,
check the next phase's kickoff (`C3t-3.md`), and commit.
