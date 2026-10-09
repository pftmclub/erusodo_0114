# Dev overlay - Phase O1-review - review of the engine flag

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase dev-overlay O1-review` |
| Plan doc | `DEV_OVERLAY_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | O1 |
| Flag | `SERV_IRUHADEV_DEV_OVERLAY` |
| Log tag | `[DEV_OVERLAY]` (`python tools/logs.py --tag DEV_OVERLAY`; before O2, read `data\dev_overlay.log` directly) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py DEV_OVERLAY_PHASES.md "Established facts" "Design" "Phase O1" "Phase O1-review"
```

## Task

Do "Phase O1-review": an adversarial review of O1's diff (`git log --grep "Dev overlay Phase O1"`, then `git show`). Check:
- flag off = base, every `#else` intact (`flagcheck`);
- a quiet TET path;
- the served-set's thread safety;
- the index is built before any loader thread;
- the ExternFileMapping name is looked up;
- truncation matches the kom path;
- the `DeleteFile` guard compares normalised full paths.

Fix what is wrong. If code changed, rebuild and repeat play-test (b). Design questions go to the user.
