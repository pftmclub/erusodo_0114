# June character Rena (Stage 7) - Phase CH2a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH2a review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2a done (its Done note, commit, and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL`, `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG` (defined for the play-test; this review retires it) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL equip`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH2a - Done (2026-11-09)"
grep -n "Rena: CH2a" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Rena CH2a" june2017_re/funcmap.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2a - Review"
```
Plus `june2017_re/char_rena/ch0_skill_listing_rows.tsv` (14 ids, 7 tables) and the "Amended by CH0 / CH1n" sections of `kickoffs/char-rena/CH2a.md`.

## Task

Check CH2a's code against June from the listings (`python june2017_re/charport_adis.py rena --phase CH2a --out <scratch dir>`; Ghidra read-only for a
spot-check). The phase wrote 18 functions and registered 7 tables; 28 functions are "= March" with no code (the funcmap rows say which). Spot-check at least:
- **Registration `32cb06d0`.** The id switch is `switch( GetUpgradeBaseSkillID( id ) )` under `_LIVE` (June `32207210`); each table name and callback of the 14 rows of
  `ch0_skill_listing_rows.tsv` (`@32cb08b9` .. `@32cb156c`), including that the hyper rows of Assault Kick, Phoenix Strike and Siege share the normal functions,
  Perfect Storm and Aero Tornado use their own hyper functions, and no hyper row has an Init.
- **The "= March" claims.** Pick five (Assault Kick EP `32c75d80` keys + 1.3 s, Aero Tornado EP `32c76490` 2.8 s, Perfect Storm FM `32c86dc0` = March's live part under
  `BALANCE_PATCH_20131107`, Backflip Shot FMF `32c76960`, Phoenix Strike camera `32c947c0`) and confirm them from the listing.
- **Ground cancels.** `32c74a40` (Power Attack EP: walk `+0x8c98`, back-walk `+0x8c94`, dash `+0x8c9c`, Z / X `+0x8c88` at `@32c74c81`, each with the foot test) and `32c76070` (Perfect
  Storm: skill, dash, walk only); the new `RENA_GROUND_ZX_CANCEL_EX` macro.
- **Backflip Shot.** `32c94e20`: 0.361 s, `Dummy2_Lhand`, angle table by count, the 15 arguments of `CreateInstance` (the force-down rate read at `+0x4050` must be
  `m_SkillRelatedData.GetForceDownValueRateOfRenaArrowAttack()`); `32c768c0`: base then upgrade templet, level `max( 1, GetSkillLevel( id, 0, 0, 1 ) )`, ability 30.
- **Preload lists.** The six `Init` lists name for name and in order (`initgen.py`-style read of `32c6b2e0` / `32c6b410` / `32c6b620` / `32c6b820` / `32c6b8b0` / `32c6bc30`); the
  names are all in `ch0_assets.tsv` (checked by the phase: staged as June's bytes).
- **Play-test.** Read `logs.py --grep "RENA SKILL"` and the `[JUNE_SKILL]` lines; damage per skill against `MODS.md` "Stage 7 Rena, CH2a".

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL --with SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG --range <CH2a commit>^..<CH2a commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`, a one-file `/Zs` probe of the cpp. Retire `_SKILL_DEBUG` (`//#define`; CH2b re-defines it for its lines).
Fix clear errors in place (bytepatch only). Write "CH2a - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH2b.md`, commit with explicit pathspecs.
