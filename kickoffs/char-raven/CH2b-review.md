# June character Raven (Stage 7) - Phase CH2b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH2b review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2b done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` + temporary `_SKILL_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN SKILL` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH2b - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2a - Review (2026-11-06)"    # the worked example
```
Then `june2017_re/char_raven/ch0_skill_listing_rows.tsv` (the CH2b ids), and the Ghidra listings of the 27 CH2b addresses
(`cd june2017_re && python charport_adis.py raven --phase CH2b --out <dir>`).

## Task

Check CH2b against June (read-only in Ghidra; floats with `june2017_re/charport_consts.py --fn <va>`). In order of risk:
- **Registration.** The 14 listing rows of `32c641a0` for the 12 ids: slot for slot (Berserker Blade Init + EP, hyper EP only; Hypersonic Stab Init + FMF + EP, hyper FMF + EP with its own
  table; Shockwave five callbacks on both slots; One Flash Init / Start / EP on both; Bleeding Slicer Init / Start / EP on both plus the GROUND (274) / FLY (275) states; Armor Break Init + EP
  on both). Upgrade ids reach the same case (raw-id switch); `RNSI_*` values against `Enum.src.lua`.
- **Shockwave end `32c26820`.** The helper `3229ea50` (CH0 "candidate"): confirm `(skill 0x5f62 = 24418, index 0, learned-only 1)` = `GetSkillBuffFactorPtr` (vslot 0x238), the find by the factor's
  `+0x28` and `EraseBuffTempletFromGameUnitByIterator`, and that `GetSkillBuffTempletID` + `EraseBuffTempletFromGameUnit` is the same effect. JUNE_BUGS 547.
- **One Flash `32c33280`** (`SA_DAMAGE_TIME` = 0x4e = 78, `[this+0x4118]` = `ChangeDamageTimes`), **Bleeding Slicer** starts `32c5ccc0` / `32c5cd80` (the `322b5320` call), the cancels
  `32c37dc0` / `32c38030` / `32c3aab0` (key order, which time member each compare reads, the foot test, no walk cancel in Armor Break), **Hypersonic Stab** `32c30af0` (1300, 0.7 one-shot, `>= 1.0`).
- **"= March" rows** (15) and the preload lists (Berserker 10, Hypersonic 12, Shockwave 13, One Flash 6 names): equal, name for name and in order.
- **Open from the phase:** `m_fEDTBleedingTime` is still read through March's `SI_A_RVC_BLEEDING_SLICER` in the unit init (X2GURaven.cpp, CH4a's passive-id list row 51); no reader uses it yet.
- Play-test result and the `RAVEN SKILL` log lines (`equip skill`, `one flash skill ... hits`, `shockwave end counter buff`), then retire `_SKILL_DEBUG` (`//#define`).

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --range <CH2b commit>^..<CH2b commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`, one-file probe of `X2GURaven.cpp` (0 errors). No build if the review changes no code (a `//#define` retire is a not-awaited build).
Then check the CH2c0 kickoff.
