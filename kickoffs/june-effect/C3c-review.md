# June effect completion - C3c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3c review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3c done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_SUPER_ARMOR_CONDITION` (new) and `SERV_IRUHADEV_JUNE_DAMAGEDATA` (extended) |
| Log tag | `JUNE_FX C3c` (`python tools/logs.py --grep "JUNE_FX C3c"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3c - Done (2026-10-08, Sonnet 5.5 high)"
grep -n "(C3c)" june2017_re/funcmap.tsv
cat june2017_re/c3b_damage_react_sites.tsv
```

## Task

Review C3c's commit against the June binary. The phase ported the super-armour condition, `DamageReactStateChange`'s gates and the ADD_MEMBER_COUNT reader.

1. **The 22 sites, case by case.** Diff the six June case blocks in `X2GUUser.cpp` (`#ifdef SERV_IRUHADEV_JUNE_SUPER_ARMOR_CONDITION` before each case body) against the decompile of `31feabd0` and the tsv:
   - the gate order (`SAC( false )` then FORCE, then the FORCE block);
   - which unit-condition frame is read (`+0x7e2d` = Future `bAttackerFront`);
   - the UP / FLY / DRAG_UP shape: the speed is taken before the `0x81c` check, and `ResetOnSomethingFuture` sits inside it;
   - the hold / grap state ids in FLY and DOWN.
   Spot-check at least `@31feae21`, `@31feb330` and `@31feb499` in Ghidra.
2. **DOWN's dropped reset (the one live change).** Confirm from the xrefs of `32081520` (SetNowForceDown) that June's four writers are the ones the funcmap row `31feabd0 (C3c)` lists, and that nothing re-adds the reset on the RT_DOWN path. JUNE_BUGS 131.
3. **Parse.** `CommonStateStart`: June's order, the local `bSuperArmor` set by FRONT feeding `m_bStateSuperArmor` and the NotRed block (`@32058f05`, `@32059000`).
4. **Collector `32294e30`.** Check the argument order against the call `@32036bee-32036c11`, every skip test, the NPC class / size term, the angle test (`GetDirVecToDegree` = `31c19930`, component z) and the sort direction. The x87 polarities were settled by reading the masks, not by `x87test.py`: re-run `june2017_re/x87test.py` on `@32295114`, `@32295177` and `@3203686d` if there is any doubt.
5. **Per-factor packet apply `322a6d00`.** The port is the two additions in `SetKBuffFactorPacketToGameUnit` (the `0x3e0` refusal hook, the finalizer `SetBuffTempletID` loop). Confirm nothing else in `322a6d00` differs from March's body.
6. **Hygiene.** `python tools/flagcheck.py JUNE_SUPER_ARMOR_CONDITION --range <first C3c commit>^..HEAD` and the same for `JUNE_DAMAGEDATA`. Explain every OTHER row and deleted line.

Write "C3c - Review" under the phase in the plan, update the STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
