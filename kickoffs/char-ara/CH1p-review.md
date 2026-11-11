# June character Ara (Stage 7) - Phase CH1p review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1p review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1p done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`//#define`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1p - Done"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH1p - Done"
grep "Ara CH1p" june2017_re/funcmap.tsv
grep "^GetPressedSkillState" tools/data/june_march_divergent.tsv
```

## Task

Review CH1p against the exe bytes (`june2017_re/funcdis.py`, `dispsweep.py`, `disasm_range.py`). Spot-check at least:
- **`31fd3360`:** the hyper test direction (`fcomp; test ah,1; jne` = remaining time > 0 -> hyper), `cmp 4; jle`, and which of the four `.rdata` tables each
  branch indexes (`[idx*4 + base]` with the `- 5` folded into the base) against the C++ in `X2GUUser.cpp` (`GetPressedSkillState`).
- **Shared reader:** in the `_LIVE` `.i` of `X2GUUser.cpp`, `CommonSpecialAttackEventProcess`'s `bMarchSlotIndex` excludes `UT_ARA`; flag-off has no `UT_ARA` there.
- **Sweep:** `dispsweep.py 8ac0 8b58 88bc 8ea0 8ea4 8ea8` over `32b90b70-32bd86e0`: every `+0x8b58` / `+0x88bc` reader is ported or routed (Energy Void here;
  Energy Wave CH2c; Specter of Storm CH2d / CH2d2; `32b93590` dead). No other March Ara site still feeds June's 0-9 to a 4 + 4 decoder (`CurrentSkillKeyManager`,
  `SAKP_*`, `> 3` / `- 4`) in the `_LIVE` `.i`.
- **Energy Void:** `32b93bc0` whole = the StartFuture; `32b9e820`'s two calls; the CH3b amendment's charge numbers (`+0x96c8 >= 5`, `>= 10`, 0.4 at `33487220`).
- **Keys:** the owners in the Done note (CH1j's `CommonStateStart` reads, the C3t-2 sound-set reader, the dead Specter of Blade tables).

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --range <CH1p commit>^..<CH1p commit> --files X2Lib/X2GUAra.cpp X2Lib/X2GUUser.cpp`
(FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GUAra.cpp X2Lib/X2GUUser.cpp`;
one-file `/Zs` probes of `X2GUAra.cpp` and `X2GUUser.cpp` off, `_LIVE`, `_LIVE` + `_FP` + `_SPECTER`. CH1p left no `_DEBUG` define. Fix clear transcription
errors in place (bytepatch only). Write "CH1p - Review" into the plan doc, update the STATUS rows (CH1p done, CH1q ready), check `kickoffs/char-ara/CH1q.md`,
commit with explicit pathspecs.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1q` (Opus 5.5 high).
