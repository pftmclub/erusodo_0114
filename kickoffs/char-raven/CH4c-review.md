# June character Raven (Stage 7) - Phase CH4c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH4c review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH4c done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_PASSIVE` + temporary `_PASSIVE_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN PASSIVE` (`logs.py --grep "RAVEN PASSIVE shadow step"`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH4c - Done (2026-11-07)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH4c - Review"    # the worked example
```
Then funcmap rows `(Raven CH4c)` (`grep -n "Raven CH4c" june2017_re/funcmap.tsv`) and `june2017_re/char_raven/ch0_overheat.tsv` (owner `CH4c`).

## Task

Check CH4c against June (read-only in Ghidra). In order of risk:
- **"Already done" claims.** Each `ch0_overheat.tsv` CH4c row is either ported by the phase the Done note names (CH1g setters, CH1g2 object / icon,
  CH1h ReInit, CH1j CommonFrameMove, CH4a InitPassiveSkillState) or equal to March; spot-check two against the tree.
- **`322e07e0` @`322e1114`:** the 24200-only compare, `+0x9728` = `m_bActiveShadowStepCombo` (`ch1_offsets.tsv`), 10.0 (dword `3344a51c`), and that the
  `else` chain after it is March's `else if` chain ( 0x714f Stasis Field first ). JUNE_BUGS 559.
- **`322de5d0`:** no Veteran Commander pass among its nine callees (the call dropped under the flag); **`328e0aa0` @`328e0c8f`:** the 0x73 block sits
  inside the `+0x130` test, class 115, skill 0x5e88, the two strings. JUNE_BUGS 560.
- **Routed, no code:** `CX2GUUser::InitEqip` `32024510` and the viewer `31f94360` (june-engine R14c, two `replace` rows); the pack's June-only members 40 / 41
  (backlog Stage 36); `MemoConditionRaven` n/a (`32c28100` always false); Specialization B readers taken by CH1q; CTI 76 / 82 equal.
- Play-test result and log lines ( `shadow step slot: combo 1, mp 10.0` / `combo 0, mp 40.0` ), then retire `_PASSIVE_DEBUG` ( `//#define` ).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_PASSIVE --with JUNE_CHAR_RAVEN_PASSIVE_DEBUG --range <CH4c commit>^..<CH4c commit> --files` the three code files,
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2Game.cpp X2Lib/X2UISkillTreeNew2015.cpp`.
No build if the review changes no code (a `//#define` retire is a not-awaited build). Then check the CH6 kickoff (`kickoffs/char-raven/CH6.md`).
