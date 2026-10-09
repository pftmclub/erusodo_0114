# June effect completion - C3l review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3l review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3l done (its Done note and commit). The play-test is a regression only (no March data uses the additions), so the review need not wait for it. **Not with** a phase that edits `CX2GUUser::CommonStateStartFuture` or `CommonFrameMoveFuture` |
| Flag | `SERV_IRUHADEV_JUNE_SPEED_TIME` |
| Log tag | `JUNE_FX C3l` (`python tools/logs.py --grep "JUNE_FX C3l"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3l - Done"
grep -n "(C3l)" june2017_re/funcmap.tsv
grep -n "^| 153 " JUNE_BUGS.md
```

## Task

Review C3l's commit against the June binary (Ghidra read-only; `june2017_re/disasm_range.py` for ranges, `vtslot.py` for slots).

1. **Parser** `31fe2740`: the five columns, their miss values (x 0, y 0, z -1.0 at `334380f0`, iFlag 0, bool false), the "%s%d" names from
   index 0, and the return value (`@31fe28cd` true / `@31fe28d1` false). Check that `32fe9ed0` reads a bool the way March's
   `KLuaManager::GetValue( int, bool& )` does (`LuaCommon/KLuaManager.cpp:696`).
2. **Loader** `31fe3a60`: the main / minor list choice on `+0x8e90`, the fallback order (main ULTIMATE empty -> SPEED_TIME; the minor list
   independently). Re-check the gate's key: templet `+0x178` = `m_DeleteSkillIDByUpgrade`, including the EBP = templet - 4 calibration
   (`32216320 @32217cbb` vs `32071720`) and the `+0x20` map insert (`@322186ca`) / reader (`32205c00`). If the calibration is wrong, the key is.
3. **Consumer** `32061bf0 @320621f4-3206236a`: the bool test (`== 1`), the angle's source (`+0x7e0c` = Future `dirDegree.z`; Now getter
   `3208dfd0`), the x87 order (float roundings at `@32062217`, `@32062224`, `@32062238`, `@32062251`), cos / sin (`32fe2b4e` / `32fe2b54`),
   and JUNE_BUGS 153 (`y = x cos sin`). Run the tail `@32062255-32062263` with a popping x87 harness to settle it.
4. **iFlag 11 / 12 / 13**: the jump table (`32063618`, index bytes `32063634`), and vslot `0x778` = `3208e1e0` (identity) in every user class
   vtable but Rose's (`32a7ab60`). Check that the March virtual is not shadowed by a class with the same name.
5. **Not ported, check the routes:** the minor list and its consumer pass, the second-body anim clock (checklist 7o); Rose's override (7o);
   `m_DeleteSkillIDByUpgrade` and the upgrade swap (backlog Stage 3c).
6. **Hygiene.** `python tools/flagcheck.py SERV_IRUHADEV_JUNE_SPEED_TIME --range <C3l commit>^..HEAD`.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`). Write "C3l - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
