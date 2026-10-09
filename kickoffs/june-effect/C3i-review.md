# June effect completion - C3i review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3i review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3i done (its Done note and commit). The play-test can come before or after the review |
| Flags | `SERV_IRUHADEV_JUNE_NPC_FORCE_BACK_SPEED`, `SERV_IRUHADEV_JUNE_NOT_UP_STATE`, `SERV_IRUHADEV_JUNE_NOT_AFFECT_BLACKHOLE`, `SERV_IRUHADEV_JUNE_NO_FALL_DOWN_AT_STATE` |
| Log tag | `JUNE_FX C3i` (`python tools/logs.py --grep "JUNE_FX C3i"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3i - Done"
grep -n "(C3i)" june2017_re/funcmap.tsv
grep -n "^| 143 " JUNE_BUGS.md
```

## Task

Review C3i's commit against the June binary (Ghidra read-only; `june2017_re/disasm_range.py` for ranges Ghidra does not carve).

1. **The function start.** Confirm `32769240` (ret 8) is the NPC `DamageReactStateChange` and `32769060` (ret 4) is not. Then confirm
   the citation fixes (funcmap C3b / C3d-3 / C3d-3 review rows, the RT_BOUNDING comment in `X2GUNPC.cpp`, the C3i / C3j kickoffs).
2. **The FORCE gate**, case by case (`@327696cf` SMALL, `@327698ba` BIG, `@32769a12` DOWN, `@32769ae9` UP, `@32769bb8` FLY, `@32769c7c`
   DRAG_UP): the gate order, each forced-path body (SMALL's `+700` and x push by `+0x844d`; the m_FlyCount division in UP and FLY only),
   and that the forced path changes no state. Settle `+0x844d` = `unitCondition.bAttackerFront` with `offsetprobe.py`.
3. **The six NOT_UP_STATE tests.** In the forced path the hit does nothing. In the normal path only the `nowSpeed.y` write is skipped
   (not zeroed) and the state still starts. Check the `if / else` chains the port inserted before `if( m_FlyCount > 2 )` and before
   DRAG_UP's speed line still bind the way June branches.
4. **The black hole** (`31f6eea0`): the NPC checks' order, `_isnan` and `GetInvincible` skipping only the move (a user is still
   added), and the reasons the PvE owner filter (`@31f6f1a6`) is not ported. Re-run the range / clamp compares with `--x87` if in doubt.
5. **NO_FALL_DOWN_AT_STATE**: the key read (`32784c00 @327960e6-3279625d`, after HINT_NO_PERFRAME_EVENT, default false), its two readers
   (`@31f6f0b2`, `@3230ec81`) and no third (byte sweep of `+0x401`, which `dispsweep.py` misses: use a raw search like the Done note's).
6. **Routed:** the NPC FlushForceDown guard (`@3276a18d-3276a1f3`) went to C3f item 3; check the C3f kickoff names it.
7. **Hygiene.** `python tools/flagcheck.py <each flag> --range <C3i commit>^..HEAD`. Explain the OTHER rows and the one deleted line
   (the RT_BOUNDING comment, under `SERV_IRUHADEV_JUNE_BOUNDING_STATE`).

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`). Write "C3i - Review" under the phase in the plan,
update the STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
