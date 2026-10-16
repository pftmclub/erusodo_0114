# June buff port - Phase 7 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 7 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 7 done |
| Flag | SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 1c, item 2" "Shared checklist" "Phase 7 -- group C" "Phase 7 -- Done"
```

The plan doc's "Starting context" predates `tools/`: wherever it (or the task below) says to
build, deploy, check size/mtime, run a fresh unifdef, run `file`/`git diff --stat` for hygiene,
or read logs by hand, use `tools/build.py --deploy`, `tools/flagcheck.py`, `tools/logs.py` and
`tools/bytepatch.py` instead (the x2-phase skill's "Tools" and "Finish" lists).
Never launch X2_offline.exe; ask the user.

Most of the June class functions (`32e7xxxx`) are not carved in Ghidra (`decompile_function` says "No function found").
Read them with `python june2017_re/disasm_range.py <start> <end>` (raw capstone, bounds from the int3 padding), as Phase 7 did.

## Task (verbatim from the plan doc)

Review the Phase 7 commit(s) for types 73, 94, 95, 104 and 105 against item 2: enum
values and the BTI 975 enumerator, identity-store insert/erase, the two stubs filled
with exactly June's body, 94's two verbatim quirks, 95's DeleteNonControlDebuff walk,
104's user and NPC gates, 105's reader and single-line host change, and SERV_IRUHADEV_ /
#else / CP949 hygiene (run `file` on every touched file and check `git diff --stat`).
Spot-check 2-3 types directly in Ghidra (read-only); if a spot-check fails, re-check the
whole phase. Fix clear transcription errors under the same flag; hand design decisions
back to me. Write the findings into the doc as "Phase 7 -- Review", and commit.
