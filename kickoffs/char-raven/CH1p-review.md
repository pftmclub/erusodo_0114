# June character Raven (Stage 7) - Phase CH1p review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1p review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1p done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`//#define` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH1p - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1p - Review (2026-11-04)"   # the worked example
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1p - Review (2026-11-02)"     # the SkillSlotKey conversion
```

## Task

Check CH1p against June (read-only in Ghidra; bytes with capstone over `unpacked_x2.exe`, `charport_adis.py raven <va>`). In order of risk:
- **The 12 held-key tests.** For each June function in the Done note, confirm the call is `31fce710` with the future state data skill id (`+0x7cb0` protected
  pointer -> `+0xc`) and `m_InputData` (`+0x86da`), and that it guards the same branch as March's `SAKP_*` test it replaced (overheat count, loop, fire). Redo
  `dispsweep.py 8ac0`: no Raven reader. Judge whether any June function's structure around the call differs enough that the `_LIVE` test alone misbehaves before
  its skill phase (note it for that phase; do not port bodies here).
- **Limit Crusher.** `32c27860` = `Set( m_iNowSpecialAttack - 1 )` + vslot 0x7c0; `32c38f70` `FollowSlotSwap` first, then end-of-animation OR ( time >= 0.45 AND
  `IsKeyReleased` ) -> revenge attack, = the converted condition.
- **Shared reader.** `UT_RAVEN` in `bMarchSlotIndex` only under `_LIVE`; the `#else` is Elsword's block unchanged; the `Always.h` guard (`_LIVE` needs `_ELSWORD_LIVE`) is
  right and fires nothing in the `_LIVE` probe.
- **Left alone:** the two Revolver Cannon HE EventProcesses (June-dead states, old skill id 4017) and Magnum Blaster's `GetSkillSlotIndexUsed`; Shadow Step routed to CH2a
  (`32c30db0 @32c30e73`, `32c52550 @32c5255b` / `@32c525f4`).
- **Sweep:** `cl /P` of `X2GURaven.cpp` with `_LIVE`: no `CountUsedSkill`, `GetPressedSkillState`, `CurrentSkillKeyManager` or `SAKP_` outside the two HE states.

Hygiene: `flagcheck JUNE_CHAR_RAVEN_LIVE --range <CH1p commit>^..<CH1p commit> --files X2Lib/X2GURaven.cpp X2Lib/X2GUUser.cpp KTDXLIB/Always.h` (numstat 76 / 0, 5 / 0, 3 / 0),
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GUUser.cpp`, one-file probes of `X2GURaven.cpp` and `X2GUUser.cpp` with and without
`-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. No build is needed if the review changes no code. Then check the CH1q kickoff.
