# June character Rena (Stage 7) - Phase CH2c review ( folded into the CH2d review )

STATUS.md and `CH2d0.md` say CH2c has no review of its own: the **CH2d review reads this list too** and covers CH2c's code, its play-test logs and retires `_SKILL_DEBUG` once (CH2d re-uses it for its own lines). Nothing starts from this file by itself.

| | |
|---|---|
| Start | with the CH2d review ( `/x2-phase char-rena CH2d review`, Opus 5.5 high ) |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2c done (its Done note, commit, and the user's play-test) and CH2d |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL`, `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG` (re-defined for the play-test; this review retires it) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL equip`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH2c - Done (2026-11-10)"
grep -n "Rena: CH2c" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Rena CH2c" june2017_re/funcmap.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2c - Review"
```
Plus `june2017_re/char_rena/ch0_skill_listing_rows.tsv` (the 14 class-13 ids, 8 tables), `JUNE_BUGS.md` row 576 and the "Amended by CH0 / CH2a / CH2b review" sections of `kickoffs/char-rena/CH2c.md`.

## Task

Check CH2c's code against June from the listings (`python june2017_re/charport_adis.py rena --phase CH2c --out <scratch dir>`; Ghidra read-only for a spot-check). The phase wrote
26 functions (19 inside March's, 7 new: Rapid Shot's `StateStart` / `FrameMoveFuture` / `FrameMove` / `UpdateRapidShotAimRotation` and Hunter's Steadiness's `Init` / `EventProcess` / `FrameMove`)
and registered 8 tables; 13 functions are "= March" with no code (the funcmap rows say which). Spot-check at least:
- **Registration `32cb06d0`.** Each table name and callback of the 14 class-13 rows (`@32cb13b7` Guided Arrow, `@32cb17a5` Wind Wall, `@32cb1446` Crazy Shot, `@32cb0c29` Shooting Magnum,
  `@32cb0c87` Arc Shot, `@32cb0bcf` Hunter's Steadiness, `@32cb0cf9` Rapid Shot and its hyper rows). Hunter's Steadiness's hyper row has no direct stores: June calls `329cc2c0` (`CopyStateCallbacks`),
  so the port copies all callbacks; the Wind Wall hyper takes Start / FM / EP / End but no Init; Crazy Shot's hyper has no Init; Rapid Shot's attack / finish states (`@32cb0ded` / `@32cb0e33`, ids `0xf0` / `0xf1`) carry the ready state's FrameMoveFuture.
- **The "= March" claims.** All 13: the thunks `32c6b7e0` / `32c6b7f0` / `32c6b800` / `32c6b810`, Guided Arrow EP `32c75ed0` / `32c75fa0` (14 keys past 1.8 s), Crazy Shot EP `32c76840` / `32c76880`, Wind Wall `32c76dd0` / `32c76e30` / `32c6bef0`, Arc Shot Init `32c6c9c0`, Rapid Shot READY EP `32c7c950`.
- **Rapid Shot aim.** FMF `32c88050` (id `0x5b77` = `RNSI_P_RGA_RANDOM_SHOT_PLEASURE`, ±50 / 0 into the Future class-skill float), helper `32c86620` (aim += elapsed x Now float, clamp ±40, `SetRotateBone( true, Bip01_Spine1, ( 0, 0, -1 ), ( 0, 0, aim ) )`),
  attack FM `32c9d250` (time after start, arrow at 0.034 s, `y += RandomFloat( -20, 20 )`, `z += aim`), finish FM `32c9d420` (past 0.08 s the Now float -50 above 3, +50 below -3, else 0 and aim 0; arrow at 0.054 s). The Now-slot writes are `syncData +0x8178`.
- **Arc Shot FM `32c9cc10`.** Five arrows at +30 / +15 / 0 / -15 / -30; with the equipped templet `+0x178` (vslot `0x524` = `GetNowStateSkillID`) non-zero: -45, then an absolute 45 (kept verbatim).
- **Wind Wall FM `32c953d0`.** `EffectSet_SA_RSR_WindWard`, hyper false, `GetPowerRate()`, ability 1 (`SA_EFFECTIVE_TIME`) as the life time, ability `0x20` (`SA_PROJECTILE_SIZE`) three times; the unit-null case skips the effect set only.
- **Hunter's Steadiness `32c83220`.** `CommonFrameMove` first, then at 0.71 s `SetBuffFactorToGameUnit( templet of 23210 ( swap first ), 0 )` (user slot 138, `0x228`).
- **Preload lists.** The seven `Init` lists, name for name and in order (`32c6b720` / `32c6bb20` / `32c6be30` / `32c6c970` / `32c6ca70` / `32c6d690`); all names are `june-bytes` in `ch0_assets.tsv`.
- **Icon gap.** `skillicons.py check 23012-23048 23208-23236` passes with two `KNOWN_JUNE_GAP` rows (23210 / 23234 image piece): confirm June's own TET lacks the piece (the staged sheet is byte-identical to June's).
- **Play-test.** Read `logs.py --grep "RENA SKILL"` and the `[JUNE_SKILL]` lines (damage per skill against `MODS.md` "Stage 7 Rena, CH2c").

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL --with SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG --range <CH2c commit>^..<CH2c commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`, a one-file `/Zs` probe of the cpp. Retire `_SKILL_DEBUG` once, in the CH2d review (`//#define`).
Fix clear errors in place (bytepatch only). Write the CH2c findings under "CH2d - Review" in the plan doc.
