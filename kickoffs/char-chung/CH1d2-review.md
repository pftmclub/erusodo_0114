# June character Chung (Stage 7) - Phase CH1d2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1d2 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1d2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1d2 - Done (2026-10-31)"
awk -F'\t' '$7=="CH1d2"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1d2 is 21 functions, all `inside`. Read them against June (`python june2017_re/charport_adis.py chung --phase CH1d2 --out <scratch dir>`). The ones where June is not March:
- `AttackDashXEventProcess` `32bdea80`: the X X branch tests class 111 (Deadly Chaser) only (JUNE_BUGS 492); 110 -> `CSI_CIP_COMBO_DASH_XX` on Z at 0.53333, 117 -> `CSI_CTT_DASH_COMBO_XX` on X at 0.53333; the Deadly Chaser dash cancel (`m_fDashCancelAfter`, `+0x8c9c`) needs `IsOnSomethingFuture` and sits in the else-if chain after the two time tests. The jump table of `AttackDashXFrameMove` `32bf80e0` (`32bf87cc` / `32bf87dc`: classes 21, 111 -> CSG, 110 -> CIP, 117 -> CTT, else the plain set).
- `AttackDashXFrameMove` `32bf80e0` / `AttackDashJumpXFrameMove` `32bf8840`: skill `0x672f` (`RNSI_P_CIP_OPEN_MIND`, `GetSkillLevelValue` = June `31fceab0`) learned plays the effect set with no `UseCannon` and no onomatopoeia; `AttackDashJumpXStartFuture` `32bee7c0` skips the 1/4 speed when it is learned.
- `ComboDashJumpZEventProcess` `32bdece0`: four flag + `m_fEventTime[0..3]` pairs (`+0x8c10`-`+0x8c3c`, stride 12), their state ids (`0xba`, `0xc3`, `0x10e`, `0x14d`), the job-line numbers (1, 3, 2), "stateChange < t || anim end" on the flag side and "t < keyInputEnd && keyInputStart <= t" on the window side.
- `AttackDashJumpXEventProcess` `32befa80`: the Deadly Chaser 0.36 X branch (class 111 only), the two new windows (`CSI_CIP_COMBO_DASHJUMP_XX` 0x145 for class 110, `CSI_CTT_DASH_JUMP_COMBO_XX` 0x148 for class 117), the in-air X only for classes 6 / 20 / 27.
- `ComboDashZEventProcess` `32bde810`, `ComboDashJumpZZLandingCannonBallShotEventProcess` `32bdf160`: the any-direction walk (to `USI_WAIT`) and the dash cancel need `IsOnSomethingFuture`; the ZXASDC arrow test does not; Z's arrow goes to `USI_WALK`, the landing's to `USI_WAIT`.
- Everything else is claimed `= March after the macros`; spot-check three line by line, including every float.
- New enumerators `CSI_CIP_COMBO_DASHJUMP_XX`, `CSI_CTT_DASH_JUMP_COMBO_XX`, `CSI_CSG_COMBO_DASH_JUMP_ZX_READY` (X2GUChung.h, `_CORE`, appended without `= value` like CH1d's `CSI_CFG_COMBO_XXFRONTZ`; C++ values are March's append order, not June's script ids). The CH1f / CH1g2 / CH1j kickoffs carry an "Amended by CH1d2" note.

Hygiene: `GIT_CONFIG_COUNT=1 GIT_CONFIG_KEY_0=diff.algorithm GIT_CONFIG_VALUE_0=patience python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE --range <CH1d2 commit>^..<CH1d2 commit>` (FAIL only because the `#define` is commented out); `python tools/junediverge.py --check-all`; one-file `cl /Zs /EHsc` probe with and without `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`june2017_re/census.py` `cl_cmd`; from Git Bash use `-D`, a leading `/D` is turned into a path). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1d2 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1e.md`, commit with explicit pathspecs.
