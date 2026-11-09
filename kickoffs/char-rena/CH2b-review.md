# June character Rena (Stage 7) - Phase CH2b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH2b review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2b done (its Done note, commit, and the user's play-test) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL`, `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG` (re-defined for the play-test; this review retires it) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL equip`, `RENA SKILL flush mp once`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH2b - Done (2026-11-09)"
grep -n "Rena: CH2b" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Rena CH2b" june2017_re/funcmap.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH2b - Review"
```
Plus `june2017_re/char_rena/ch0_skill_listing_rows.tsv` (the 14 class-12 ids, 7 tables) and the "Amended by CH0 / CH1n / CH1p / CH2a" sections of `kickoffs/char-rena/CH2b.md`.

## Task

Check CH2b's code against June from the listings (`python june2017_re/charport_adis.py rena --phase CH2b --out <scratch dir>`; Ghidra read-only for a spot-check). The phase wrote
11 functions plus one engine helper and registered 7 tables; 8 functions are "= March" with no code (the funcmap rows say which). Spot-check at least:
- **Registration `32cb06d0`.** Each table name and callback of the 14 class-12 rows (`@32cb1201` Spinning Kick, `@32cb1282` Crescent, `@32cb12ec` Dive Kick, `@32cb2396` Rising Falcon,
  `@32cb0b70` Back Kick, `@32cb1809` Low Kick, `@32cb11a2` Assault Impact, and their hyper rows); the hyper rows share the normal functions except Crescent / Dive Kick (own table name, same functions) and Rising Falcon (no Init); Dive Kick has no FrameMoveFuture; Low Kick has no StateStart.
- **The "= March" claims.** All 8: Spinning Kick FMF `32c6ba10` and EP `32c76630` (14 keys, 1.4 s), Dive Kick EP `32c76800` and FM `32c86ee0` (cut-in `Bip01_R_Foot` 0.14 index 2), Rising Falcon FMF `32c6c790`,
  EP `32c7ae20` (landing state `0xe3`), FM `32cad390` (`DAMAGE_DATA_NEXT` / `_LAST` by `m_fDamageDataChangeTime` / `2`), Landing EP `32c7a6c0` (SKIP_TIME, CHANGE_TIME).
- **Crescent Kick EP `32c76700`.** `GetUsefulValueFuture( 0, 0 ) < time && !foot` -> JUMP_DOWN (the compare is `<`, from `fcompp` + `test ah, 5; jp`), then 14 keys past 2.5 s. The script row is `USEFUL_VALUE0 = 1.9` (`Lire_ElvenRanger.src.lua` 9614).
- **Back Kick EP `32c84ea0` and `FlushMpOnce` (the one new engine piece: Sonnet identified user vslot `0xe8` = `3208dcb0` itself, so this needs your read).** Confirm: the chain order (animation end, flag branch to
  Low Kick `0x127`, key window, ground dash / walk, skill cancel), the condition (templet `+0x178` = `m_iDeleteSkillIDByUpgrade`, `31fce710`, `31fc7b40( 1 ) +0x118` = `bHit`, `FlushMpOnce( GetLearnedSkillAbilityValue( SA_MP_CONSUMPTION = 0x8f ) )`),
  and `+0x7638`'s only other writers: `32050759` (`CommonStateStartFuture`, after the socket early return) and `322a5eb2` (constructor / Init, false). Nothing else reads it (byte search of `.text` found 6 sites: the two writers, `3208dcb4` / `3208dcf2` in `3208dcb0`, and two stack-local `31da36f8` / `31da38f4`). Judge whether keeping the flag in `CX2GUUser` (as `m_bInstanceKill` was) is acceptable.
- **Assault Impact EP `32c7d3a0`.** animation end, dash (`+0x8c9c`), walk (`+0x8c98`), skill (`+0x8c90`), each ground-only; no Z / X branch.
- **Preload lists.** The seven `Init` lists name for name and in order (`32c6b950` / `32c6ba20` / `32c6ba70` / `32c6c710` / `32c6c860` / `32c6cf80`); the names are all in `ch0_assets.tsv` (staged from June by CH0b).
- **Play-test.** Read `logs.py --grep "RENA SKILL"` and the `[JUNE_SKILL]` lines (damage per skill against `MODS.md` "Stage 7 Rena, CH2b"); the Back Kick chain must show one `flush mp once` line per chained cast.

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL --with SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG --range <CH2b commit>^..<CH2b commit>`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`, a one-file `/Zs` probe of the cpp. Retire `_SKILL_DEBUG` (`//#define`; CH2c re-defines it for its lines).
Fix clear errors in place (bytepatch only). Write "CH2b - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH2c.md`, commit with explicit pathspecs.
