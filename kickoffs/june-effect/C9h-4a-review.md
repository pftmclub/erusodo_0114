# June effect completion - C9h-4a review - June's unit sound records, delete conditions and players

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-4a review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4a done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_UNIT_SOUND` (+ `_DEBUG`) |
| Log tag | `JUNE_FX UNIT_SOUND` (`python tools/logs.py --grep "JUNE_FX UNIT_SOUND"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4a - Done" "C9h-4 - Done"
grep -n "Effect C9h-4a" june2017_re/funcmap.tsv
```

## Task

Review the C9h-4a code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- **The decoded keys** of `SOUND_PLAY_ALL_OPTION` (`@3205d1cc-3205da78`). The Done note decoded `SOUND_NAME`, `PLAY_TIME` and `RATE` byte by byte; re-decode the
  other five (`ONLY_IF_MY_UNIT`, `MAX_SOUND_DISTANCE`, `ENABLE_3D_SOUND`, `LOOP`, `DELETE_CONDITION`) from the XOR buffers at `0x3379d650`.. and check each
  getter's type (`32fe9e40` bool, `32fea370` float, `32fe9fd0` int) against the record offset it stores into.
- **`DELETE_CONDITION` replaces the seed:** read `31ecace0` and the vector writes at `@3205d9b4-3205da23` to confirm the list is cleared before it is filled
  (the port clears it), not appended to.
- **`SOUND_PLAY_EX`:** rows are 1-based, `[3..]` are the conditions, the record has no seed, and an empty name leaves the row open (JUNE_BUGS 117).
- **The stop sites' places in March:** DamageReact (`@3204a71f`, June's guard wider than March's), the shock block (`@32068a3c`), CommonStateEnd
  (`32276ca0` between `3228a200` and `3228a120`; `32009ae0` before the avatar-emotion stop), CommonStateStart (`32009ae0` early, `CUSTOM_STATE_DELETE_SOUND`
  after the effect-set one, `32278200` late), ReInit, the dtor.
- **The players:** `31fea120` erases after play (also on a null handle) and keeps a record whose roll fails; the STATE player never erases, forces loop off and 3D
  on, and pushes null handles.
- **`+0x9108` = `m_bGrap`:** confirm from another reader that it is the grab flag and not something else.
- **Regression:** on March data every attach is `DCS_NONE`, so the lists stay empty; the only change a March script can see is the player erasing a record (Chung).

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_UNIT_SOUND --range <first C9h-4a commit>^..HEAD --with JUNE_UNIT_SOUND_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "C9h-4a - Review" into the plan
doc, update the phase's STATUS.md row, check the next kickoff (`C9h-4b.md`), and commit.
