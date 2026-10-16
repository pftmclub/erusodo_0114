# June port backlog - Stage 12 planning - character customize system

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-backlog stage12-plan` |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 12), output `JUNE_CUSTOMIZE_SYSTEM_PHASES.md` (new) |
| Depends on | june-effect C9g-2 review (done); planning can start before Stages 10 / 2 / 13 finish, phases that need them say so |
| Flag | one family per phase, e.g. `SERV_IRUHADEV_JUNE_CUSTOMIZE_*` (none in this planning session: no code) |
| Log tag | `JUNE_FX CUSTOMIZE` (propose it in the plan) |
| Review | none (a planning session); each planned phase gets its own review kickoff |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 12: character customize system (june-effect C9g, 2026-10-07)" "Stage 10: emotion system (user, 2026-10-07)" "Standing rules (every stage, every kickoff)"
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9g - Done" "C9g-2 - Done" "C9g-2 - Review"
grep -n "Effect C9g\|Effect C9d" june2017_re/funcmap.tsv
```

## Task

Plan Stage 12 (June's `CX2CustomizeManager`, `CharCustomData.lua`) as a phased plan in the `new-plan` shape of the x2-phase skill.
First RE (read-only Ghidra) what C9g left open:
- the `CX2Unit +0xa44` applied-customize map's insert site;
- `326525d0`'s full key list against `june_src_fixed/` `CharCustomData*.src.lua`;
- `CharCustomPreviewData.lua` (`3264d790`) and `CustomItemHide.lua`;
- the viewer members `+0xf48..+0x11d0`, `ApplyCustomize` `31fa89b0`, `ResetCustomize` `31fa4980`, motion-list load `31fa8230`;
- the custom branches of `31fa5c90`, which C9g-2 left as Stage 12: cases 0/3/6 custom, the 1-2 custom-WAIT shortcut, the sit arrays 7/8/9;
- the HABIT timer `31fa8ce0 @31fa99f1` (`+0xf9c`/`+0xfa0`/`+0xf90`) and `31fa7ef0`'s `+0x11c0` HABIT branch (`326509e0`);
- the square side (`323fad80` custom sets, `323f92f0`/`323f9470`/`323f9680` in `323fa700`);
- the preview dialogs (`3224b340`, `3224d0f0`, `32d77220`, `32d7c7d0`; `32d7c962` is the only PlayByMotionType caller passing bTransition 0).

Census the `Cash1-6_*` motions and the customize items against March data and June's `item.kim` (Stage 13). Mark what needs Stage 10
(sit ids 0x19-0x3b) and Stage 2 (UI).

Output:
- `JUNE_CUSTOMIZE_SYSTEM_PHASES.md`;
- one kickoff per phase in `kickoffs/june-customize/`;
- a STATUS block;
- funcmap rows tagged `Customize S12`.

No code, no build. Open design questions go to the user at the end, each with a recommendation.
