# June character Eve (Stage 7) - Phase CH1l2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1l2 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1l2 - Done"
awk -F'	' '$6=="CH1l2"' june2017_re/char_eve/ch0_class_map.tsv         # 9 rows
grep -a "Char Eve CH1l2" june2017_re/funcmap.tsv                        # 11 rows: the 9 class-map rows, 322e65c0, 3214f050
```

The members are in the fourteenth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp`; declarations at the end of the class in `X2Lib/X2GuEve.h`. No JUNE_BUGS row.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1l2 --out <scratchpad dir>` (the 9 listings, all short). You decide:

- **`3297c390` (vslot 0x7d4).** The Mega Electronball block (enable `+0x9784`, swap `+0x978c` from `GetUISkillTree()->GetSkillSlotSwap()` via `3214f050` +0x115, slot id `+0x97a0` in 0-9, key by `JuneIsEquippedSkillKeyPressed( 0x6277 )`, `nowAction` `+0x7e84` = 7) and the Taser Pilum block (enable `+0x9a54`, swap `+0x9a5c`, key `0x627c`, `nowAction` = 17, no range test, the `3288fe30` result unused, so left out). That there is no Kugelblitz branch and the dirty-state batch (`3376cdd0`) is only the anti-tamper write of `nowAction`.
- **`32980f90` (vslot 0x7c8).** The two clamped timers, `+0x7ec0` (`iJuneClassSyncInt`) = `+0x9b9c` (is it `m_iManeuverCoreLv`? the block is under `ADD_EVE_SYSTEM_2014`, off today), the Code Recovery cool-time run-down after the base, and the omitted `322e65c0` call (args in its funcmap row; CH4c).
- **`3297a400` (vslot 0x7d8).** The now-state index `( +0x7cbc - +0x7cec ) / 100` against 21 = `USI_DASH_JUMP` and `GetNowStateID()` as its March equal (X2GUUser.cpp `JuneOverrideEffectAlphaCondition` comment).
- **The small ones.** `32965e30` / `32965e50` (`+0x9ae4` = `m_fEffectStartTime`; `+0x9b70` = `m_bIsCheckEndElCrystal`, same order as March's `m_eReserveEndElCrystalBuffID`, `m_bIsCheckEndElCrystal`, which re-labels two ctor-map rows), `32965e70` (hidden RENDER_HINT pointer), `329694a0`, `32976fb0` (name still neutral).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1l2 commit>^..<CH1l2 commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1l2 - Review" into the plan doc under the Done note, update the CH1l2 row in `STATUS.md`, check that `kickoffs/char-eve/CH1l3.md` still matches, and commit with explicit pathspecs.
