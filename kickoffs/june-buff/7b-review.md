# June buff port - Phase 7b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 7b-review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 7b done |
| Flag | SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 1c, item 2" "Shared checklist" "Phase 7b -- group C" "Phase 7b -- Done"
```

The plan doc's "Starting context" predates `tools/`: wherever it (or the task below) says to
build, deploy, check size/mtime, run a fresh unifdef, run `file`/`git diff --stat` for hygiene,
or read logs by hand, use `tools/build.py --deploy`, `tools/flagcheck.py`, `tools/logs.py` and
`tools/bytepatch.py` instead (the x2-phase skill's "Tools" and "Finish" lists).
Never launch X2_offline.exe; ask the user.

## Task (verbatim from the plan doc)

Review the Phase 7b commit(s) for the types that landed (74, 77, 97, 101, 102, 110)
against item 2: the m_bNoDetonation split (every write site of the two bytes, the
charge-zeroing behavior change in DetonationEventProcess), the 77 getter and
CommonFrameMove block and what was decided about the 12 direct bInvincible reads, 97's
base virtual and per-class overrides, 101's virtual HP setters and their extra gates, 110's
rewritten consumer, and SERV_IRUHADEV_ / #else / CP949 hygiene (run `file` on every
touched file and check `git diff --stat`; X2GUUser.cpp and X2GUNPC.cpp are large CP949
sources, and a diff of hundreds of lines for a small change means the file was
re-encoded). Because every one of these touches a state machine every unit runs, read the
#else branches and confirm the flag-off path equals March. Spot-check 2-3 types
directly in Ghidra (read-only); if a spot-check fails, re-check the whole phase. Fix clear
transcription errors under the same flag; hand design decisions back to me. Write the
findings into the doc as "Phase 7b -- Review", record this session's /cost, and commit.
