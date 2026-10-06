# June effect completion - C8c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C8c review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C8c done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION` |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C8a - Done" "C8c - Done"
```

Also read:
- the funcmap rows tagged `Effect C8a`;
- the D8 row;
- C8c's commits (`git log --oneline --grep="C8c"`).

## Task

Review C8c's commit(s) block by block against the June overrides its Done note cites. Check:
- skill IDs, ability types (`SA_*` numbering);
- DamageData fields and protected-value writes;
- the `+0x53c` / `32610fb0` trigger numbers;
- that the D8 blocks follow the user's answer;
- that Add and Elesis's Way of the Sword were left alone.

**Hygiene** is one command: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION --range <first C8c commit>^..HEAD`.
- Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
- Explain every OTHER row and deleted line.
- A FAIL or ERROR is a finding.

**Spot-checks and fixes:**
- Spot-check 2-3 blocks directly in Ghidra (read-only). If one fails, re-check the whole phase.
- Fix clear transcription errors under the same flag, then run `tools/build.py --deploy`.
- Hand design decisions back to the user.

**Finish:** write "C8c - Review" under the phase, update the STATUS.md row, and commit. Never launch X2_offline.exe: ask the user.
