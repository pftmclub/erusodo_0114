# June GFx runtime - Phase GA - GFx assets in, byte-exact

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-gfx GA` |
| Plan doc | `JUNE_GFX_RUNTIME_PHASES.md` (branch `main`) |
| Depends on | G0 item 4 (where the scene `.swf` load from) for the `.swf` part; the koms can go first |
| Flag | none (data only) |
| Log tag | none |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Context" "Rules" "Starting context" "Phase GA - GFx assets in, byte-exact"
```

## Task

Do "Phase GA". Extract with `tools/junekom.py` (byte-exact; never elstools for June algo-2), stage into
`24965799\kom_contents\data194_kom`, `data152_kom` (new kom numbers: check `X2Main.cpp`'s 226-kom mount covers them) and
`data036_kom`; add `_manifest.json` entries in the house format (CLAUDE.md, manifests). A name March already has: stop and ask.

`/elstools pack` each, then ask the user before each `/x2-kom-packed copy`. The scene `.swf` go where G0 says. List every
staged file in the Done note (the asset port's P0 census then counts them as same bytes). Done gate as in the section.
