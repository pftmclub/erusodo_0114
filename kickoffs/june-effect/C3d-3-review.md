# June effect completion - C3d-3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3d-3 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3d-3 done (its Done note and commits). There is no play-test to wait for (C3d-4) |
| Flag | `SERV_IRUHADEV_JUNE_BOUNDING_STATE` |
| Log tag | `JUNE_FX C3d-3` (`python tools/logs.py --grep "JUNE_FX C3d-3"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3d-3 - Done"
grep -n "(C3d-3)" june2017_re/funcmap.tsv
grep -n "^| 140 \|^| 141 " JUNE_BUGS.md
```

## Task

Review C3d-3's commit(s) against the June binary (Ghidra read-only; `june2017_re/disasm_range.py` for ranges Ghidra has not carved).

1. **The five user handlers** `31fd0230`, `31fd0260`, `31fc9d20`, `31fd02d0`, `31fd0340` and the RT_BOUNDING case `31feabd0 @31feb68f-31feb71a`:
   state ids by name, `StateChange` second argument (0 in the handlers, 1 in the case), the speed fields (`+0x304` / `+0x308`) and which anim the
   LANDCRASH test reads. Re-settle the two x87 compares (`31fd0291`, `31fd0354`) with `june2017_re/x87test.py`.
2. **The NPC half** `32769060 @32769da7-32769e75`, `3275a070`, `3275d900`: the type order (June 0x21..0x23), `NOT_BOUNDING` @327a678e, and the facing
   test (the NPC compares its own facing, the user case compares both). Confirm `+0x8ac4` is never written on an NPC (sweep `lea` forms too) and
   `322f8920`'s reader at `[ebp+0x8ac4]` is a user.
3. **No data.** The first pass's `June_Bounding_States.lua` and the data037 Mars staging were removed (user rule: no invented Lua). Confirm `kom.py pack data036 data037` shows `~0 changed` and `grep -rn June_Bounding_States X2Lib KTDXLIB` finds nothing.
4. **Last attacker** `320462a0 @32046f06-32046f27`, `327b1f30 @327b2be6`: the stored type is the attacker's game-unit type (+0x264), the UID the virtual getter.
5. **Twins checked and left alone** (the Done note gives the evidence; re-check one): March's dead `DamageReactStateChange_CheckOnly` NPC case
   (`X2OPTIMIZE_NPC_NONHOST_SIMULATION_ANALYSIS` is OFF) and the `m_FlyCount++` RT_BOUNDING case (`BOUNDING_COMBO_TEST`, OFF). **Open from the Done note:**
   whether June's DamageReact (`32045dc0`) still counts RT_BOUNDING hits in its fly count (not located by pattern). Find June's counter and decide.
6. **Play-test legs** (registration log lines, a bounding hit) are not part of this review. C3d-4 (user, 2026-10-09) left the registration to the
   character ports, so no class registers the states yet and they run with the first port (checklist 7l). This review is code-only and deploys only if item 1-5 finds a fix.
7. **Hygiene.** `python tools/flagcheck.py JUNE_BOUNDING_STATE --range <first C3d-3 commit>^..HEAD`. Explain the nested `#else`/`#endif X2OPTIMIZE_*` rows.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`). Write "C3d-3 - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
