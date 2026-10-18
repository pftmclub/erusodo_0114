# June skill system core - Phase S6b - Key-config rows for the fifth skill keys

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase skill-system-core S6b` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S6 (reviewed). Not alongside another session editing `X2KeyPad.cpp` / `X2StateOption.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_KEYCONFIG` |
| Log tag | `[JUNE_SKILL] UI` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | none unless the user asks (small, UI only) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S6 - Slot UI"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ah. The 10-slot dialog's class statics and the key-config rows (from June skill system S6)"
```

## Task

Added by S6 (2026-10-18; memory `plan-unowned-june-branches-without-asking`). S5 added the actions `GAMEACTION_SKILL5` / `GAMEACTION_SKILL_EX5` (default F / T) and S6 shows their labels on the 10-slot bar,
but the options dialog cannot rebind them: the key-config list has no row, no icon and no tooltip for them.

- Find June's key-config UI: the option dialog's key rows (`MappingKeyConfigIndex` equivalent, June `X2StateOption`), the `X2KeyPad::UpdateUI` icon switch (March `X2KeyPad.cpp` cases `GAMEACTION_SKILL1-4` /
  `_EX1-4`), the `DLG_UI_KEYSETTING.TGA` art keys (`SKILL_A5` / `SKILL_B5`?) and the dialog Lua (census with `tools/junekom.py`, read June Lua from `june_src_fixed/`; the dialog Lua is adopted with `adopt-june`).
- Port June's rows for the two actions (index, icon, tooltip string, the conflict check on a rebind) so the options dialog lists and rebinds F / T. If the art or the dialog Lua needs a kom, pack it and ask before every copy.
- **Gate:** a rebind of the fifth A key and the fifth B key in the options dialog works and the 10-slot bar's labels follow it; flagcheck PASS; build + deploy.
- Stop and ask only where copying June breaks March.
