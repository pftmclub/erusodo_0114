# June character Chung (Stage 7) - Phase CH1q review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1q review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1q done (its Done note and commit); no play-test (the class bodies are under the undefined `_LIVE`, the outside sites under the undefined `_CBALL`) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CBALL` (`//#define`, CH1 defines both), `_CORE`, `_CBALL_DEBUG` |
| Log tag | `[JUNE_CHAR] CHUNG CBALL` (no line can appear before CH1 defines `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1q - Done (2026-11-02)"
grep -a "(CH1q)" june2017_re/funcmap.tsv
grep -a "char-chung CH1q" tools/data/june_march_divergent.tsv
```

## Task

Check CH1q against June (read-only; `python june2017_re/charport_adis.py chung <va> ...`; read listings, not decompiles). In order of risk:
- **The awakening refill.** `ConsumeCannonBallCount` `32bf3e40` and `CannonBallGaugeTimerFrameMove` `32bf4680`: every x87 compare direction (`0 <= t`,
  `t <= 0`, `-10 <= t`, `0 < hyper time`), the `+0x6844` = `GetHyperModeCountByBuff()` reading, vslot 0x524 = `GetNowStateSkillID()`, and the -100 / 0.3
  values. Confirm the only writers of `+0x98b8` are the ctor (-1), these two and the berserk end (-100, CH1l2 / CH1l4).
- **`UseCannon` `32bf5280`:** `+0x6d04` is `GetCharacterSpecializationB()` (Eve CH1q's DoParrying read the same) and the roll is taken before the
  hyper test; the used count adds the reduced count only on the reload branch.
- **`LoadPersonalInfo` `32bf5430`:** the four xor keys (decrypt them), their defaults (6 / 0 / 0 / 0), that `32bef230` (the heavy-stance setter) truly has
  no caller (calls, jumps, pointers), and the GUARD_BUG_TEST block.
- **Outside the class (`_CBALL`):** the play status factor 13 in `32a32390` / `32a347e0` and the max-before-now order; `UpdateDataFromGameUnit` `32a34720`
  copying no `+0x141`; InitUI `32e6d6c0` calling vslot 0x54 (= InitWakeOrb `32e5b3a0`) before the dialog; the DamageEffect block `31f7cae4`
  (`+0x9864` stored with no multiply, unit type 6 test) against March `X2DamageEffect.cpp` ~1928.
- **The guarded extra count:** `GetExtraCannonBallCount` / `SetExtraCannonBallCount` (`32c24d00`, `32c24d40`) and every March caller reached under `_LIVE`
  (`SetExtraCannonBallCount( GetMaxCannonBallCountEx() )` sites at mode entry: is `m_bMobileArmoredModeEnable` set first?).

Hygiene: `flagcheck` for `JUNE_CHAR_CHUNG_LIVE`, `JUNE_CHAR_CHUNG_CBALL` (both FAIL only for the `//#define`), `_CORE` and `_CBALL_DEBUG` with
`--range <CH1q commit>^..<CH1q commit>`, `git diff --patience --numstat`, `junediverge.py --check-all`, `enumscan.py --check` on the six files. Run the
one-file probes of `X2GUChung.cpp`, `X2GageData.cpp`, `X2GageUI.cpp`, `X2DamageEffect.cpp` with `_LIVE` + `_CBALL` off and on. Keep `_CBALL_DEBUG` until
CH1's review (its lines can only appear in CH1's play-test). Write "CH1q - Review" into the plan doc, update the STATUS rows (CH1 waits on CH0b only), check
`kickoffs/char-chung/CH1.md` and commit.

Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
