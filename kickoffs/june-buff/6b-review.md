# June buff port - Phase 6b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 6b-review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 6b done |
| Flag | SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 1c, item 1" "Shared checklist" "Phase 6b -- group B" "Phase 6b -- Done"
```

The plan doc's "Starting context" predates `tools/`: wherever it (or the task below) says to
build, deploy, check size/mtime, run a fresh unifdef, run `file`/`git diff --stat` for hygiene,
or read logs by hand, use `tools/build.py --deploy`, `tools/flagcheck.py`, `tools/logs.py` and
`tools/bytepatch.py` instead (the x2-phase skill's "Tools" and "Finish" lists).
Never launch X2_offline.exe; ask the user.

## Task (verbatim from the plan doc)

Review the Phase 6b commit(s): every host June rewrote (DoStateEndStart and the
power-rate cache, the summon helper and its 21 call sites, GetActualMPConsume, the
cooldown helper and its 16 callers) against "Phase 1c, item 1", including what the
flag-off (#else) path does compared with March, any deviation recorded, and
SERV_IRUHADEV_ / CP949 hygiene (run `file` on every touched file and check `git diff
--stat`; X2GUUser.cpp is a 37k-line CP949 source, and a diff of hundreds of lines for a
small change means the file was re-encoded). Spot-check 2-3 host sites directly in Ghidra
(read-only); if a spot-check fails, re-check the whole phase. Fix clear transcription
errors under the same flag; hand design decisions back to me. Write the findings into the
doc as "Phase 6b -- Review", record this session's /cost, and commit.
