# June effect completion - C3j-1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3j-1 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3j-1 done (its Done note and commit) and its play-test read |
| Flag | `SERV_IRUHADEV_JUNE_BOUNDING_STATE` (+ `SERV_IRUHADEV_JUNE_BOUNDING_STATE_DEBUG`, turn it OFF here) |
| Log tag | `JUNE_FX C3j-1` (offline_server.log; `logs.py --grep "JUNE_FX C3j-1"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3j-1 - Done"
grep -n "327992a2-327992fd" june2017_re/funcmap.tsv
grep -n "^| 154 " JUNE_BUGS.md
```

## Task

Review C3j-1's commit against June (Ghidra read-only, `june2017_re/disasm_range.py`, `funcdis.py`):

- **The three fallbacks** in `CX2GUNPC::InitState` (`327989e0 @327992a2-327992fd`): order DOWN, LANDCRASH, UP; each a `== 0` test on the
  value just stored; DOWN and LANDCRASH's last step read `+0x4098` (March `m_CommonState.m_DownDamageAir`), LANDCRASH's middle step reads
  the type byte `+0x14f` (`m_acStateType[ STATE_TYPE_DOWN_DAMAGE_AIR_LANDING ]`, 0x13; not a member), UP reads `+0x8690` (`m_UpDamage`).
  Confirm both members are filled before the fallbacks in March (they are, a few lines above) and that nothing after them in InitState
  rewrites `m_UpDamage` / `m_CommonState.m_DownDamageAir` (the `GetStateID` copies at ~`X2GUNPC.cpp:7033/7043` are in InitState's dead
  `#else X2OPTIMIZE_NPC_NONHOST_SIMULATION`).
- **The reworded comments** (render gate, RT_BOUNDING case, CommonStateStart tail, `Always.h` flag block) say what the code now does.
- **The play-test result** in the Done note: the `ids` lines' up = the NPC's UP_DAMAGE state, and the `overwrite` line's speed after = (0,0).
- **Turn `SERV_IRUHADEV_JUNE_BOUNDING_STATE_DEBUG` off** (comment out the `#define`, add a dated "OFF" line to its block), then
  `tools/build.py --deploy`.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_BOUNDING_STATE --range <C3j-1 commit>^..HEAD --files X2Lib/X2GUNPC.cpp` and the same
with `SERV_IRUHADEV_JUNE_BOUNDING_STATE_DEBUG --files X2Lib/X2GUNPC.cpp KTDXLIB/Always.h`. (`Always.h` under the main flag gives an ERROR
on C3i's pre-existing `#if defined(...)` dependency line, 6336, which flagcheck cannot evaluate; that is not a finding.) Paste the
`files:` / `in this flag's files:` / `RESULT:` lines; explain every OTHER row and deleted line.

Write "C3j-1 - Review" into the plan doc, update the STATUS row, and commit with explicit pathspecs.
