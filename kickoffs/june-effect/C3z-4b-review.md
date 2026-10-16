# June effect completion - C3z-4b review - unbound Lua methods, batches 2 and 3

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-4b review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3z-4b done and play-tested (its Done note and commit) |
| Flags | the 21 `SERV_IRUHADEV_JUNE_*` flags listed in "C3z-4b - Done" (debug: `SERV_IRUHADEV_JUNE_C3Z4_DEBUG`) |
| Log tag | `JUNE_FX C3z-4b` (`python tools/logs.py --grep "JUNE_FX C3z-4b"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-4b - Done"
awk -F'\t' '$7=="2" || $7=="3"' june2017_re/c3z4_bind_plan.tsv
```

## Task

Review C3z-4b's commit against the June bodies its Done note cites. The things this phase could get wrong:

- **Mapped readings:** vslot 0x174 read as `GetGameUnitState()` (GUSI_DIE = 4), `unit +0xc04 -> +0x20` as the HP gage
  (`GetNowHp() > 0`, NULL gage data counting), `+0x8aa0` = `GetSiegeMode`, team 2 = `TN_MONSTER`, the line `+0x18` =
  `bCanDown`, world object `+0x194 / +0x19c` = type / `m_wstrObjectName`.
- **ChangeMotion_LUA** (`32761cd0`): the order of close / open / reset / set clears / `CreateAndSetAinmXSkinMesh`, the
  now-state replay (`31e8a990` read as `GetNowStateData`), and the after image rebuilt unconditionally (JUNE_BUGS #174).
- **SetMovieDescriptionData**: June's render arg (the state render pass takes a bool in June; March passes false), the
  font (June id 6 read as `XUF_HEADLINE_30_NORMAL`), and coordinates passed unscaled (March's font scales its output).
- **Henir pair**: the claim that June's level-limit map is always empty (JUNE_BUGS #172).
- **GO_NEXT**: March's handler takes before / now indices where June's takes the sub-stage pointer.
- **Data:** the 8 Enum / NPCTemplet rows against June's values and rows.

Hygiene is one command per flag:
`python tools/flagcheck.py <FLAG> --range <C3z-4b commit>^..HEAD --with JUNE_C3Z4_DEBUG <the other 20 flags>`.
Paste the `files:` / `RESULT:` lines. Explain every OTHER row and deleted line (the only expected deleted line is the
`SERV_IRUHADEV_JUNE_C3Z4_DEBUG` toggle in `Always.h`).

Spot-check 2-3 bodies in Ghidra (read-only); if one fails, re-check the whole batch. Fix clear transcription errors under
the same flag and `tools/build.py --deploy`. Turn `SERV_IRUHADEV_JUNE_C3Z4_DEBUG` off once the play-test has passed. Write
"C3z-4b - Review" into the plan doc, update STATUS.md, check the next kickoff, commit.
