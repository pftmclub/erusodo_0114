# Dev overlay - Phase O6-review - doc sweep review + close-out

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase dev-overlay O6-review` |
| Plan doc | `DEV_OVERLAY_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | O6 |
| Flag | none |
| Log tag | `[DEV_OVERLAY]` (`python tools/logs.py --tag DEV_OVERLAY`; before O2, read `data\dev_overlay.log` directly) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py DEV_OVERLAY_PHASES.md "Rules" "Phase O6" "Phase O6-review + close-out"
```

## Task

Do "Phase O6-review + close-out":
- **Review** O6's changed sections for contradictions and stale pack/copy or loose-copy instructions, and fix them.
- **Dry run:** `overlay.py promote --plan dev-overlay --dry-run` must list nothing.
- **Memory:** finalise the entries.
- **STATUS:** mark the block done.
