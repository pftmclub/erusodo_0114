# June engine remainder - Phase W1e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1e review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase W1e done (its Done note and commits) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_LINK` (+ short-lived `_DEBUG`) |
| Log tag | `[JUNE_ENGINE] WORLD far link` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase W1e - Line links to a far line (Elysion line 2 \`BEFORE_LINE = 130\`)" "Phase W1e -- Done"
grep -a "(W1e)" june2017_re/funcmap.tsv
```

## Task

Review Phase W1e's commit(s) against June's `CKTDGLineMap::CanDown` `31c6c310`. What this phase could get wrong:
- the six gates: each must be `linked line enabled && linked lineType == 7 (LT_MONSTER_ROAD) && bIsMonster` in the loop
  (`@31c6cbcd` higher / start, `@31c6e612` higher / end, `@31c6f61a` lower / start, `@31c6f6cb` lower / end), but the **entry line's own** type in
  the no-hit fallback (`@31c6f7dc`, `@31c6f82a`). Re-read 2-3 in the disassembly and confirm the refused paths land where March's else does
  (`@31c6e597` / `@31c6f53c`: only when `touch` is still false; `@31c6f652` / `@31c6f6ff`: line = i, ret = pointL2);
- the fallback's bounds test (`JS` + unsigned `JBE` @31c6f756 / @31c6f763) and its NULL entry line returning false (`@31c6f77d` -> `@31c6c3dd`);
- the collision skip (`@31c6c57d`: `g_pKTDXApp +0x1c0` = `m_pCollision`, ctor `31c17ed0` order viewport / `new(0x1c)` / device manager);
- that nothing else in the body differs from March (switch on `lineType - 1`, the bounding-box precheck, the `0.0001` start / end tests,
  `pos.y < 0` -> false). The `DROPPABLE_MONSTER_ROAD` case, `param_8` (separate out line index) and the caller map are W1f's, not findings here.

Hygiene is one command:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_LINK --range <first W1e commit>^..<last W1e commit> --with SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_LINK_DEBUG`
(`git log -S SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_LINK` names the commits). Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

If the play-test passed, remove the short-lived `_DEBUG` flag (its `#define`, the `#error` block and the helper + 4 call blocks) and rebuild.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Phase W1e -- Review" into the plan doc, update the W1e STATUS.md row, check `kickoffs/june-engine/W1f.md`, and commit.
