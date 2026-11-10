# June character Ara (Stage 7) - Phase CH1b review (covers CH1a-CH1b)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1b review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1a and CH1b done (their Done notes and commits) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`//#define`; no `_CORE` row so far) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1a - Done" "CH1b - Done"
awk -F'\t' '$7=="CH1a" || $7=="CH1b"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Review the 47 functions of CH1a and CH1b against June (`python june2017_re/charport_adis.py ara <va>...`; the listing carries the `StateChange` arguments and the `fcomp` / `test ah` direction). Spot-check at least:
- **`32b96f70` / `32b97390`** (DamageDownFront / Back EventProcess): flags 5 / 6 / 7 fire on `stateChange < time || IsAnimationEnd()` (the file-local `ELSE_IF_STATE_CHANGE_ON_EX_OR_ANIMATION_END_` from CH1a), the time tests run before the key test, state ids per listing (quick stand FRONT_FRONT / FRONT_BACK / ATTACK and BACK_FRONT / BACK_BACK / ATTACK), the no-down roll, the Back key arms.
- **`32bab2d0`** (StandUpRolling End): the memo block is unconditional with `( BFI_BUFF_BREAKING_MEMO, 1, NULL, 0, false )` (7ai; row `32bab361`), and `GetEqippedSkillMemo` is the June-returns-false one (`SERV_IRUHADEV_JUNE_DIVERGE_SKILL_MEMO`).
- **`32b96b30`** (Die StartFuture): the riding pet dying-state stand-in for `32d278f0`.
- **CH1a:** AirialDash FrameMove `32b97900` / Start `32b97880` (`+0x9628` / `+0x9629`), the hyper-entry `GetHypetMpdeStateID()` calls in Wait / HyperWait / Walk / JumpLanding, JumpAttackZ `32b9a0b0`, DoubleJumpFront `32ba7820`.
- `june2017_re/char_ara/ch1_offsets.tsv` (CH1a and CH1b rows) against the listings.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --range <CH1a commit>^..<CH1b commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `/Zs` probe of `X2GUAra.cpp` off and with `/D SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`. Fix clear transcription errors in place (bytepatch only). Write "CH1b - Review" into the plan doc, update the STATUS rows (CH1b done, CH1c ready), check `kickoffs/char-ara/CH1c.md`, commit with explicit pathspecs.
