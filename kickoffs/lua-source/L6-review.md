# Lua source - Phase L6 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase lua-source L6 review` |
| Plan doc | `LUA_SOURCE_PHASES.md` (branch `main`) |
| Depends on | Phase L6 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_LUA_SOURCE` |
| Log tag | `[LUA_SRC]` |

## Read first

```sh
python tools/docsection.py LUA_SOURCE_PHASES.md "Established facts" "Phase L6" "Phase L6-review"
```

## Task

Review Phase L6's commits:
- **Threads.** Is `_ALERT` safe when loader threads call it, and does the ErrorLog write lock?
- **Throttle.** Can a per-frame error flood the log?
- **Flag off.** Does the build equal base?
- **Loads.** Is the set of files that load unchanged?
- **Parser guard.** Does it trip as intended?
- **Play-test finding (fix it).** The reason text is misleading for one failure. See the L6 Done note's "Play-test" paragraph:
  `TutorSystem.lua` is a BOM-only (3 B) file that `KLuaManagerProxy` refuses. Its `[LUA_SRC]` reason points at a lua error line
  that never exists. Give an empty buffer its own kind, and make the BOM-text reason cover non-Lua readers.

Hygiene is one command:
`python tools/flagcheck.py SERV_IRUHADEV_LUA_SOURCE --range <first L6 commit>^..HEAD --with SERV_IRUHADEV_LUA_SOURCE_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a
finding.

Fix clear defects under the same flag (then `tools/build.py --deploy`), and hand design decisions back to the user. Write "Phase L6 -
Review" into the plan doc, update the STATUS row, check the L7 kickoff, and commit.
