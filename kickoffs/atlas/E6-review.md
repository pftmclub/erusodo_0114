# Atlas/Elysion port - Phase E6 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase atlas E6 review` |
| Plan doc | `ATLAS_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase E6 done (its Done note and commit); best after the E6 play-test |
| Flag | `SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE` (+ `SERV_IRUHADEV_ELYSION_DEBUG`); SimpleDLG is `SERV_IRUHADEV_JUNE_SIMPLE_DLG` (C3z-4s) |
| Log tag | `[ELYDBG]` (`python tools/logs.py --tag ELYDBG --grep "dramatic message"`) |

## Read first

```sh
python tools/docsection.py ATLAS_PORT_PHASES.md "Starting context" "Phase E6 — SimpleDLG gauges + dramatic messages" "Phase E6 — Done"
```

Then `june2017_re/e6_dramatic_re.md` (the RE notes: row layout, TalkBox offsets, vtable slots) and the E6 rows of `june2017_re/funcmap.tsv`
(`grep -a "Atlas E6" june2017_re/funcmap.tsv`). Use the ghidra skill (read-only). Do NOT consult `NEW_VILLAGE_*.md` or `mods/lanox-offline`.

## Task

Review Phase E6's commit against June:
- the loader 322e9e00: every key's default (raw constants 33436700 / 3344a510 / 33733d74.. / 334395e0), the POSITION ( 0, 0 ) rule, the
  duplicate / skip rule and the missing EndTable (JUNE_BUGS 237), the return value;
- the player 32306110: the team gate, the START_TIME compare, every TalkBox field it fills (stack offsets in the asm 323061f0-323065b0),
  the stacking / SetMove order for both types, the +44 block (g_pMain +0x278 = GetIsSkillUITypeA), the sound, the played flag;
- the remove pair 322e61c0 / 322e6270 and their call sites (CommonStateEnd @3277f9f7, NPC dtor @327a062f), the CommonStateStart site @327af18c;
- the talk box side: the dramatic Push 32f218c0 vs base 32e943d0, SetBalloonTip 32f21380, LineBreak 32f21600, DrawImage 32e92cf0
  (anchor / offset arithmetic, ReverseY, OverUI), Update's slide 32e945c0, SetMove 32e90a30, the +0x100 destroy at each removal site,
  the spread fields in 32e928f0;
- the render order (after FrontUIRender, @3230ad36) and the OnFrameMove order (@323095da, @3230aa21);
- the decision in the Done note to run June's non-global LineBreak (June US ran the global breaker 31d436c0): confirm or hand back.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE --range <E6 commit>^..<E6 commit> --with SERV_IRUHADEV_ELYSION_DEBUG`
(use `git log -S SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE` for later commits; `..HEAD` times out). Paste its `files:` / `in this flag's files:` /
`RESULT:` lines; explain every OTHER row and deleted line.

Spot-check 2-3 items directly in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then
`tools/build.py --deploy`); hand design decisions back to the user. Write "Phase E6 — Review" into the plan doc, update the STATUS.md row,
write or check `kickoffs/atlas/E7.md`, and commit.
