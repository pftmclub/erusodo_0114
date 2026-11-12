# Offline field and dungeon rewards - Phase Z - close

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, medium** effort, then type `/x2-phase field-rewards Z` |
| Plan doc | `OFFLINE_FIELD_REWARDS_PHASES.md` (branch `main`) |
| Depends on | R6 review (R7 may still be blocked) |
| Flag | none |
| Log tag | `[DROP_STANDIN]` (`python tools/logs.py --tag DROP_STANDIN`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py OFFLINE_FIELD_REWARDS_PHASES.md "Rules" "Phase Z - Close"
```

And every `Phase R<n> - Done` / `- Review` note of this plan.

## Task

Close the plan: a `MODS.md` entry (both flags, the generated block in the two server Lua files, how to revert: comment out
`SERV_IRUHADEV_OFFLINE_DROP_STANDIN`, no repack needed); mark backlog Stage 23 done in `JUNE_PORT_BACKLOG_ORDER.md`; the `droptable.py`
row in `tools/README.md` is current; `droptable.py check` PASS on the committed files; move the STATUS block to `STATUS_ARCHIVE.md` and
add one *Closed plans* line. If R7 is still blocked, leave it as the block's one open row in place of archiving it.

No client code, no build ("full build skipped: no play-test").
