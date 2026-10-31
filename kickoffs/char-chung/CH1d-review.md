# June character Chung (Stage 7) - Phase CH1d review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1d review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1d done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1d - Done (2026-10-31)"
awk -F'\t' '$7=="CH1d"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

CH1d is 37 functions, all `inside`. Read them against June (`python june2017_re/charport_adis.py chung --phase CH1d --out <scratch dir>`; the 13 functions Ghidra has not carved are in `june2017_re/funcdis.py --func <va>`; two of them, `32bdda60` and `32bddf30`, are listed together with the carved neighbour that follows: `32bddc20` and `32bde0f0`). The ones where June is not March:
- `ComboZZEventProcess` `32bdd5e0`: the Z Z X flag [3] is Iron Paladin (110) only, Fury Guardian (20) is not tested (JUNE_BUGS 491); Deadly Chaser (111) -> [4]; Tactical Trooper (117) -> [5] with its 0.83 window first; every flag write of the three branches.
- `ComboZZZEventProcess` `32bdd7e0`: the walk cancel is the opposite-direction one, the walk and dash cancels need `IsOnSomethingFuture`, `IF_SKILL_CANCEL_AFTER( 0.5 )` first; no `IsAnimationEnd` test inside the chain.
- `ComboZZZXFinishEventProcess` `32bddc20` / `ComboXXZXFinishEventProcess` `32bde640`: walk (any direction) and dash cancels also need `IsOnSomethingFuture`.
- `ComboXXEventProcess` `32bde240`: the new CFG branch. Check `m_fEventTime[0]` = `+0x8c10` / `+0x8c14` / `+0x8c18` = `keyInputStart` / `keyInputEnd` / `stateChange` (CX2GUUser's `EVENT_PROCESS_TIME` table, `32040d0c` / `320558ca`), the `0x670( 1 )` call (`IsInJobLine`, declared for CH1l), the `bEventFlagList[2]` = `+0x7d87` flag, the `+0x143` = `CSI_CFG_COMBO_XXFRONTZ` state, and that `oneZ` only sets the X X Z flag while that flag is off.
- Everything else is claimed `= March after the macros` (`SKILL_CANCEL_AFTER` / `IF_SKILL_CANCEL_AFTER` / `ELSE_IF_ARROW_ASDC_PRESSED_AFTER` expand to June's code, `+0x24c` timer = `EventTimerOneshot`); spot-check three of them line by line, including every float (each one is a float literal widened to double in June's compare).
- `UpCannonBallCountAtThisAnimationTime` (`32bf3d30`) is March's, called by name; its June body is CH1q's. `CanUseCannon` is June's inline.
- `CSI_CFG_COMBO_XXFRONTZ` enumerator and the `IsInJobLine` declaration (X2GUChung.h, `_CORE`); the CH1e and CH1l kickoffs carry an "Amended by CH1d" note.

Hygiene: `GIT_CONFIG_COUNT=1 GIT_CONFIG_KEY_0=diff.algorithm GIT_CONFIG_VALUE_0=patience python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE --range <CH1d commit>^..<CH1d commit>` (git's default diff mis-aligns 1,000 inserted lines into 35 false deletions; FAIL only because the `#define` is commented out); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with and without `/D SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`june2017_re/census.py` `cl_cmd`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1d - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1d2.md`, commit with explicit pathspecs.
