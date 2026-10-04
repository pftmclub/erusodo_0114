# June buff port - Phase 15 step 2 onward - enchant system parity

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 15-step2` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase 15 step 1 (done, read-only RE) |
| Flag | SERV_IRUHADEV_JUNE_ENCHANT_PARITY |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | 15-review (write it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 4b-i, part 4" "Phase 15 -- Enchant system parity"
```

The plan doc's "Starting context" predates `tools/`: wherever it (or the task below) says to
build, deploy, check size/mtime, run a fresh unifdef, run `file`/`git diff --stat` for hygiene,
or read logs by hand, use `tools/build.py --deploy`, `tools/flagcheck.py`, `tools/logs.py` and
`tools/bytepatch.py` instead (the x2-phase skill's "Tools" and "Finish" lists).
Never launch X2_offline.exe; ask the user.

## Task (verbatim from the plan doc)

Continue Phase 15 at **step 2**. Step 1 (RE) is done and recorded in "Phase 15, step 1 -- RE"
(inside the "Phase 15" section above): do not redo it. Start from that subsection's
"Where this stopped" inputs (the EDT values the hosts compare, starting from 32708080's
decompose table) and its "Corrections to the Phase 15 text above", which override the
original scope where they disagree.

Before any code, put the subsection's **"User decisions to collect (step 2 onward)"** to the
user (EDT renumbering, IG_LEGEND in the shared XSLItem.h, the six buff entries now or with
Phase 13, USE_OWNERS_ENCHANT, the pack list) and stop on any whose answer changes the work.

Then, as the original Phase 15 kickoff says: port the EnchantData fields, the loader,
32272d90 and the process bodies (and the five reader hosts step 1 found) under
SERV_IRUHADEV_JUNE_ENCHANT_PARITY: June's whole functions, byte-level CP949 edits,
insert-only, flag-off check per file. Stage June's AttribEnchantItem.lua (and whatever the
pack-list decision adds; compile -v mar-2014, both staging dirs, manifest) and name it for
the user to pack. Build, deploy, commit; record where you stopped.
