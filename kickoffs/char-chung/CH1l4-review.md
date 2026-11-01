# June character Chung (Stage 7) - Phase CH1l4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1l4 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1l4 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` / `_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_USER_NUMERICAL_BY_NAME` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1l4 - Done (2026-11-01)"
grep -a "Char Chung CH1l4:" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH1l4
```

## Task

Check each row against June's listing (read-only). In order of risk:
- **`32c0d570` CommonFrameMove:** the order of the blocks; `+0x8160` = `iClassSyncInt` Now into `32bf3f60`; the shot cancel (`+0x8a48` hold, `+0x9108` grab; only `+0x9108` was matched to a named member); the Chaos Cannon block and the two `+0x9894` tests (JUNE_BUGS #501); the `SA_DELAY` wait (skill 26014, zero for Missile Shower); the `+0x97f8` map walk (JUNE_BUGS #502, node key `+0x10`, handle `+0x14`); the `+0x98d8` USE_CANNON_BALL row loop (`anim +0x214`, erase shifts the tail); the `+0x98d4` tick.
- **`32c058a0` DoCarpetBombing:** the `count x 0.2 <= time` test (March waits 1 s more), `y + 2500` with the old y as the landing y, the power chain (`GetSkillAdditionalPowerRate`, `+0x98a0` from shot 19), skill 26023.
- **`32c080f0` / `32c08010`:** the two cases, the `4000.0` and `15` constants, `SA_FALL_HEIGHT` of 26014, the buff 418 erase and the 863 / 1419 self-destructs, `m_fSummonedLifeTime` no longer reset.
- **`32c09390` / `32c09ad0`:** the claim that both equal March's `DoSateliteRain` / `SateliteRainFrameMove` (counts 1 / 2 / 4 / 3, random keys, big shell timing, power rate on each effect).
- **`GetSkillNumericalByName`** moved from `CX2GUEve` to `CX2GUUser` (same body; Eve's copy is `#ifndef`-ed out).

Hygiene: `flagcheck JUNE_CHAR_CHUNG_LIVE --with JUNE_CHAR_CHUNG_CORE JUNE_ENUM_NAMES --range <CH1l4 commit>^..<CH1l4 commit> --files <the Chung / SkillTree / BuffFactor files>` (Always.h separately: `flagcheck JUNE_USER_NUMERICAL_BY_NAME`), `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.*`, a one-file probe of `X2GUChung.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`. Write "CH1l4 - Review" into the plan doc, update the STATUS rows (CH1m becomes ready), check `kickoffs/char-chung/CH1m.md` and commit with explicit pathspecs.
