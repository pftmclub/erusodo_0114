# June effect completion - C9h-4f review - the USEFUL_VALUE<n> float table

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-4f review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4f done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_USEFUL_VALUE` (+ `_DEBUG`) |
| Log tag | `JUNE_FX USEFUL_VALUE` (`python tools/logs.py --grep "JUNE_FX USEFUL_VALUE"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4f - Done"
grep -n "(C9h-4f)" june2017_re/funcmap.tsv
```

## Task

Review the C9h-4f code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- **Row count and shape.** `320506f0 @32056a15-32056b0e` and `32057b80 @32060cd9-32060dd5`: 15 floats zeroed (`+0x8ef4..+0x8f2c`, `+0x8eb8..+0x8ef0`),
  `BeginTable( 33478e24, i, true )` for `i = 0..`, values 1 / 2 / 3 into row `i`, a miss writes 0.0, the loop stops at the first missing table, a table that opens at
  `i == 5` is closed unread. Confirm the March helper `JuneParseUsefulValue` (`X2GUUser.cpp`, before `CommonEventProcess`) does exactly that, and that the key string
  at `33478e24` is `USEFUL_VALUE` with no trailing character (the index is appended by `BeginTable( name, index )`'s `%s%d`).
- **Call sites.** The Future call sits after the cancel-after clamp in `CommonStateStartFuture`, the Now call after the clamp in `CommonStateStart`. June's order
  between the USEFUL_VALUE block and the other keys in between does not matter unless one of them shares the table cursor: confirm no `BeginTable` is left open across the call.
- **Readers.** `funcmap.tsv` rows `(C9h-4f)`: `31fc8620` (`*_WAIT_STATE_CHANGE`), `31fc9a20`, `3200d450`, `3200d1d0` (USI_EVENT_SNIPER_*). Confirm the compare directions
  and the state ids written there (they are the notes the next phase, C9h-4g, will port from), and that `3200d3f0` is not a reader.
- **Data on hand.** March scripts carry no `USEFUL_VALUE`; the parse was proved with a temporary injected table (Done note, "Play-test (3 runs)"). Delete the `SERV_IRUHADEV_JUNE_USEFUL_VALUE_INJECT` blocks and the probe block in `JuneParseUsefulValue` and the `_INJECT` / `_DEBUG` defines' leftovers if you judge them dead.
- **Hygiene:** `python tools/flagcheck.py JUNE_USEFUL_VALUE --with JUNE_USEFUL_VALUE_DEBUG --range <first C9h-4f commit>^..HEAD`.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. `_DEBUG` and `_INJECT` are already off. Write "C9h-4f - Review" into the plan doc, update the phase's STATUS.md row, check the next kickoff (`C9h-4g.md`, `C3d-3.md`), and commit.
