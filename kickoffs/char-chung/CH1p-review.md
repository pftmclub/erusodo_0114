# June character Chung (Stage 7) - Phase CH1p review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1p review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1p done (its Done note and commit); no play-test (every change is under the undefined `_LIVE`) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1p - Done (2026-11-02)"
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7ag. Pressed-index space, the fifth slot and the A5 / B5 states (from June skill system S5)"
grep -a "Char Chung CH1p" june2017_re/funcmap.tsv
```

## Task

Check CH1p against June (read-only; `python june2017_re/charport_adis.py chung <va> ...`; read listings, not decompiles). In order of risk:
- **No reader left behind.** Re-run the `_LIVE` preprocess of `X2GUChung.cpp` (`cl /P` with `/DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`) and list every surviving
  reader of `m_iNowSpecialAttack`, `m_eSpecialAttackKeyPressed`, `m_iChosenSkillSlotIndex`, `m_sCurrentSkillKeyManager`, `CommonSpecialAttackEventProcess`,
  `CountUsedSkill`, `GetPressedSkillState`, `SAKP_*` and the `> 3` / `- 4` decodes. Each must be in June's 0-9 space or in a `#else`. Also check the
  `CX2GUUser` side for paths Chung reaches.
- **The shared reader** (`X2GUUser.cpp` `CommonSpecialAttackEventProcess`): `UT_CHUNG` joins the June set only under `_LIVE`. The `#else` keeps the CH1m line.
  June `31fcadd0` returns A 0-3, F 4, B 5-8, T 9.
- **`SkillSlotKey`:** June's Nova charge `32bdaf00` / `32be7a30` and Rapid Cannon `32bdb130` / `32bee210` call `31fce6d0` / `31fce670` / `31fc89a0` on
  `+0x8b58`. The six converted sites match those call sites. Confirm March's `CSI_A_CDC_RAPID_CANNON_LOOP_StartFuture` (June `32bdb1b0`) does not `Set` either.
- **Artillery Strike control** (`32bfeb80`): jump table `32bff328` (10 entries, `value - 1`, `ja` above 9 = not held) equals `IsKeyReleased`'s order.
  Confirm `+0x872a` / `+0x872e` are `pureSkill5` / `pureSkillEx5`. The held path's `HaveThisBuff( 418 )` gate is routed to CH2c, not dropped.
- **`COMMAND_HIT_ADD_MP_BONUS`:** `32c16be0 @32c16c2c` decodes the protected float at `+0x98c4` and multiplies BBT 85 / 86's passive multiples. No other June
  reader (spot-check the `dispsweep 98c4` hits outside the class range: other classes' own members).

Hygiene: `flagcheck SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE --range <CH1p commit>^..<CH1p commit>` (FAIL only for the `//#define`), `_CORE --range` the same,
`git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUUser.cpp`. Run the one-file probes of
`X2GUChung.cpp` and `X2GUUser.cpp` with `_LIVE` off and on. Write "CH1p - Review" into the plan doc, update the STATUS rows (CH1q becomes ready), check
`kickoffs/char-chung/CH1q.md` and commit.

Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
