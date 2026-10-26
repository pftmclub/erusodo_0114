# June enum names - Phase N5a - Shared: X2GUUser.cpp

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-enum-names N5a` |
| Plan doc | `JUNE_ENUM_NAMES_PHASES.md` (branch `main`) |
| Depends on | N2 review |
| Flag | `SERV_IRUHADEV_JUNE_ENUM_NAMES` |
| Log tag | none |
| Review | none (reviewed in NZ) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_ENUM_NAMES_PHASES.md "Starting context" "Rules" "Phase N5a - Shared: `X2GUUser.cpp`"
```

## Task

Per the phase section: `IsSummonSkillByJune`, the UNIT_CLASS `case`s and the BBT / BTI sites of `X2Lib/X2GUUser.cpp`. Gate: `enumscan.py --check X2Lib/X2GUUser.cpp`. Reviewed in NZ.
