# June character Raven (Stage 7) - Phase CH2a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH2a review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH2a done (its Done note, commit and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_SKILL` + temporary `_SKILL_DEBUG` (the review retires it) |
| Log tag | `[JUNE_CHAR] RAVEN SKILL`, `[JUNE_SKILL] NUM use` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH2a - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH2a - Review (2026-11-05)"    # the worked example
```
Then `june2017_re/char_raven/ch0_skill_listing_rows.tsv` (the CH2a ids), `ch0_reg.tsv` (lines for 222, 305-309) and the Ghidra listings of the 43 CH2a addresses
(`cd june2017_re && python charport_adis.py raven --phase CH2a --out <dir>`).

## Task

Check CH2a against June (read-only in Ghidra; floats with `june2017_re/charport_consts.py --fn <va>`). In order of risk:
- **Registration.** The 14 listing rows of `32c641a0` for the 12 ids: slot for slot (Max Cannon Init + EP, hyper EP only; Power Assault five callbacks on both slots;
  Seven Burst hyper FMF + EP; Shadow Step hyper without Init; Flame Sword Init / Start / EP on both and the GROUND (278) / FLY (279) states). `SetMaximumCannonSkillFunc`
  = `@32c66ce8` / `@32c66d13`. Upgrade ids reach the same case (raw-id switch).
- **Shadow Step.** `32c52550`: the `+0x178` branch (`m_iDeleteSkillIDByUpgrade`), the Z block that does NOT clear `m_bActiveShadowStepCombo` in the upgraded branch but does in the
  plain one, `0xc9 / 0xe1 / 0xe2` = THRUST / THRUST_ENHANCE / PUNISHER (Enum.lua id + 69), `0x131-0x135`. `32c50ef0` (no `FlushMp`), `32c4de60` (no `-= level`, swap on the cool
  time and the buff rate), `32c30db0` (speed x1.3 unconditional), `32c49590` (`GetActualCoolTime`, null-unit early return), `32c33510` (`GetSkillTemplet`, level 0 -> 1).
- **Chain states** `32c378f0` / `32c42a30` / `32c379b0` / `32c54190` / `32c50e00`, `32c3a690` / `32c37b80` (walk / dash order differs), `32c4dcc0`, `32c30ee0`, `32c37a10`.
- **Power Assault `32c3e7a0` / `32c2dbd0`.** The stand-in for CRS 8 (`+0x8e64`, UNIT_BLACK) and `+0x164` = layer (0x10 = `XL_EFFECT_2`, 0xb = `XL_UNIT_0`); `31fc9570`'s four stores
  (`+0xc34` outline alpha, `+0xc44` colour alpha, `+0x8e64`, `+0xc74` blend). Decide whether the stand-in should stay.
- **Flame Sword.** `32c5d330` / `32c5d3f0` (x1.2 on `+0x7cc8`, the buff push with the memo rate: `31fdbfc0` + `322b5320`), `32c398b0` / `32c39b20` (foot-gated cancels; the FLY skill cancel's
  dangling `CommonEventProcess` body is June's).
- **"= March" rows:** Max Cannon / Power Assault / Seven Burst / Cannon Blade EP and FMF, Shadow Step FM / End / Init, Flame Sword Init / Start / EP / FLY FMF.
- Play-test result and log lines (`RAVEN SKILL equip`, `shadow step cool time`, `NUM use` against the tsv x0.01 at level 1), then retire `_SKILL_DEBUG` (`//#define`) and, if the
  `NUM use` lines matched, check whether the engine's `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG` is already retired.

Hygiene: `flagcheck.py JUNE_CHAR_RAVEN_SKILL --with JUNE_CHAR_RAVEN_SKILL_DEBUG --range <CH2a commit>^..<CH2a commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h`, one-file probes of `X2GURaven.cpp` / `X2GUUser.cpp` (0 errors). No build if the review changes no code (a `//#define`
retire is a not-awaited build). Then check the CH2b kickoff.
