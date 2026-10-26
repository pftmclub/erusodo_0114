# June enum names - Phase N5c - Shared: units, NPC, game

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-enum-names N5c` |
| Plan doc | `JUNE_ENUM_NAMES_PHASES.md` (branch `main`) |
| Depends on | N2 review |
| Flag | `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none |
| Review | none (reviewed in NZ) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENUM_NAMES_PHASES.md "Starting context" "Rules" "Phase N5c - Shared: units, NPC, game"
```

## Task

Per the phase section: `X2GameUnit.cpp`, `X2GUNPC.cpp`, `X2Game.cpp`. Gate: `enumscan.py --check` on the three files. Reviewed in NZ.
