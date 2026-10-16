# June effect completion - C3t - June's user SHOW_HYPER_CUT_IN_TIME (identify, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3t` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3r (found there). Not with a phase editing the user CommonStateStart key block or the CommonFrameMove cut-in lines |
| Flag | `SERV_IRUHADEV_JUNE_HYPER_CUT_IN_TIME` |
| Log tag | `JUNE_FX C3t` |
| Review | `kickoffs/june-effect/C3t-review.md` if code lands |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3r - Done" "C3t - identify, then port: June's user SHOW_HYPER_CUT_IN_TIME"
grep -n "(C3r)" june2017_re/funcmap.tsv
```

## Task

June's `CX2GUUser::CommonStateStart` (`32057b80`) reads a state key March does not have, right after `CHANGE_WORLD_COLOR_DATA`:
`SHOW_HYPER_CUT_IN_TIME` (XOR-built @32060505, `xordec2.py` on the function) into user `+0x9280` (a miss stores -1, `@32060676`).
`CommonFrameMove` (`32063650 @3206ce0a-3206ce42`) reads it right after March's `SHOW_SKILL_CUT_IN_TIME` line: when it is not -1
(`31c7b640` = IsSamef), user vslot `0x838` ( time, false ). Slot `0x834` is March's `ShowActiveSkillCutInAndLightByScript`; resolve
`0x838` on a concrete class (`vtslot.py 0x838 CX2GUElsword`: `CX2GUUser`'s own slot `0x834` reads a stub `33093ad0`), then diff it against
March (`32014fc0` on `CX2GUUser`'s primary vtable calls `32001380` = `GetShowCutInAndChangeWorldColor` @32015057, so it is likely a hyper
cut-in twin). Port the key, the member, the reader and whatever the slot needs, at June's positions.

Data: every June class script sets the key (17 files, the hyper-active states); March's class scripts do not, so the port is dormant until
the character ports bring June's class scripts (hand that note to `JUNE2017_CHARACTER_PORT_CHECKLIST.md`, no question).
