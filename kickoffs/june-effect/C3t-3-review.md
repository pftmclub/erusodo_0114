# June effect completion - C3t-3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3t-3 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3t-3 done (its Done note and commit) and its play-test read ('check logs') |
| Flag | `SERV_IRUHADEV_JUNE_SOUND_SET_PICK` (+ `_DEBUG`, turned back ON by C3t-3: turn it OFF here) |
| Log tag | `JUNE_FX C3t-3` (`python tools/logs.py --grep "JUNE_FX C3t-3"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3t-3 - identify, then port: June's NPC sound-set roll"
grep -n "(C3t-3)" june2017_re/funcmap.tsv
```

## Task

Review C3t-3's commit against June `327ac9d0 @327ae165-@327ae5a8` (NPC CommonStateStart) and the dispatcher `32768ac0`. What C3t-3 could get wrong:
- **The key.** Decode `0x337aa5a8..0x337aa5d0` again from `327ac9d0`'s own disassembly (Ghidra now has the function; the tail bytes are built in
  AL / BL / CL / DL, so `xordec2.py` alone prints 40 characters). It must be `USE_PREVIOUS_STATE_SOUNT_SET_RANDOM_VALUE`, NUL at buffer + 41.
- **The roll.** `edi = +0x6dcc` (`@327ae16c`), getter `32fe9e40( key, &[esp+0x14], 1 )` polarity (miss or false rolls), vslot `0x1c8` = `32755a00` =
  March `CX2GUNPC::GetRandomInt( 0 )`, `cdq / idiv 100 / lea edi,[edx+1]`, the store `@327ae564` on both paths, and that `iRand` feeds March's pick.
- **The pick is March's.** `@327ae56a-@327ae5a8` compares against StateData `+0x1f8 / +0x1fc / +0x200` and falls back to set 0 (`@327ae5a3`):
  confirm, so no NPC copy of C3t-2's rate loop or of JUNE_BUGS 192 is owed.
- **The open table.** `32768ac0 @32768c1b` opens the new state by reference before vslot `0x448`, as March's `DoStateEndStart` does. The play-test's
  `live rate0` should equal the pre-parsed `rates` first value with `found 1` (that is the live proof).
- **Member placement.** An NPC twin on `CX2GUNPC` (CommonConstructor 0), not a move to `CX2GameUnit`: check the reason in the Done note holds.
- **The `#else`.** March's `int iRand = rand()%100 + 1;` is intact; the non-adaptive `#else` roll (`X2GUNPC.cpp`, `m_usSoundSet`) is not compiled and untouched.

Read the play-test lines first. Turn `SERV_IRUHADEV_JUNE_SOUND_SET_PICK_DEBUG` OFF (comment the `#define` with a dated reason), rebuild and deploy.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SOUND_SET_PICK --range <C3t-3's commit>^..<C3t-3's commit> --with SERV_IRUHADEV_JUNE_SOUND_SET_PICK_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag
(then `tools/build.py --deploy`); hand design decisions back to the user. Write "C3t-3 - Review" into the plan doc, update the STATUS.md row, and commit.
