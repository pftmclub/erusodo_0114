# June character Rena (Stage 7) - Phase CH2d review ( covers CH2c + CH2d )

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH2d review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH2d done (its Done note, commit and the user's play-test + "check logs"); CH2c's play-test logs are already in its Done note |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL`, `..._SKILL_DEBUG` and `SERV_IRUHADEV_JUNE_CHAR_RENA_TRAP_DEBUG` (both debug defines retire here: `//#define` in `Always.h`) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL equip`, `RENA TRAP ...`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH2c - Done (2026-11-10)" "CH2d - Done (2026-11-10)"
grep -n "Rena: CH2c\|Rena: CH2d" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Rena CH2c\|Char Rena CH2d" june2017_re/funcmap.tsv
```
and `kickoffs/char-rena/CH2c-review.md` (its spot-check list for the class-13 code: do it too, and write its findings in the same review note).
Plus `june2017_re/char_rena/ch0_skill_listing_rows.tsv` (the 14 class-24 ids, 14 table rows) and `MODS.md` "Stage 7 Rena, CH2d".

## Task

Check CH2d's code against June from the listings (`python june2017_re/charport_adis.py rena --phase CH2d --out <scratch dir>`; a listing runs on into the next function: stop at the `int3`; Ghidra read-only for a spot-check).
The phase wrote 21 function bodies inside March's (`#ifdef SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` June `#else` March) and seven cases (both ids each); the other 26 of the 47 class-map functions are "= March" with no code (the funcmap rows say which).
Spot-check at least:
- **Registration `32cb06d0`.** Each table name and callback of the 14 class-24 rows: Humming Wind `@32cb1328` / `@32cb1372`, Fatality `@32cb2269` / `@32cb22d4` (hyper FrameMove `32c987f0`), Karma `@32cb21d5` / `@32cb221d` (the hyper uses the normal FrameMove), Explosion Trap `@32cb2108` / `@32cb2168`, Briar Trap `@32cb10e7` / `@32cb113a`, Eldrasil's Fury `@32cb107b` / `@32cb10c3`, Evoke `@32cb2076` / `@32cb20c0` (EventProcess `329cba60` = `31fcc9d0` = the shared `GenericActiveSkillEventProcess`, which CH1k made June's under `JUNE_CHAR_ARME_CORE`).
- **Explosion Trap FrameMove `32c97210`.** Hunter's Ability (`0x5b70`) level through `GetSkillLevel( id, 0, 0, 1 )`, the templet by plain id, `SA_DAMAGE_REL` (`0xf`); `pos.y += 200`; the count (`SA_EFFECTIVE_RATE` = 5 through `32007c40` with the swap flag; `fRate >= GetRandomFloat( 0 )`, so one more trap); per trap the delay mode makes `EXPLOSION_TRAP_FLY_DELAY` with `m_iIndexNum` (wrap at 100) x unit index and type 5, else `EXPLOSION_TRAP_FLY`; `vRotateDegree.z += 15` after each. March's `FAKE` names are dropped.
- **Briar Trap FrameMove `32c9e440`** (same Hunter's Ability block; `ROSEBUSH_TRAP_FLY_MEMO` + `_Upgrade` through `IsEquippedSkillUpgraded( GetNowStateSkillID() )`; a 20 % second one at `pos.x += 100`, unconditional) and its three thunks `32c6cf50` / `32c6cf60` / `32c6cf70` (common calls only: March's gravity switches are not June's), EventProcess `32c7d360` (no 0.36 s timer).
- **Fatality.** `SetFatalityData` `32c7b060` (23019 at the swapped level, the templet of `GetUpgradeSwappedSkillID`, abilities `0x77`-`0x7a` = `SA_SKILLDAMAGE_MULTIPLE_01_RATE`..`04_RATE`, power `0x73 + rating`; rating order LUCKY 0 / NORMAL 1 / POOR 2 / FAIL 3, jump table `32c987dc`); FrameMove `32c98360` / `32c987f0` (the `_MEMO` suffix on the damage set is unconditional; the handle pushed to `m_vecEffectSetToDeleteOnStateEnd` is the damage set's: `[esp+0x44]` = the first call's return slot); Start `32ca68b0` and EventProcess `32c7ad50` = March.
- **Karma EventProcess `32c7aae0`** (end test, then `SKILL_CANCEL_AFTER`, `RENA_GROUND_WALK / BWALK / DASH_CANCEL` in the listing's order `0x8c90`, `0x8c98`, `0x8c94`, `0x8c9c`) and Start `32c6c690` (no memo speed); **Eldrasil's Fury EventProcess `32c7d160`** (dash `0x8c9c`, walk `0x8c98`, then the skill cancel).
- **Evoke FrameMove `32ca6810` / `32ca6860`** (`CreateEvoke( 1 )` at 0.25 s, no cut-in); **Humming Wind** `32c86d00` / `32c86d30` / `32c75e50` / `32c75e90` and the new `Init` list `32c6b6a0`.
- **The "= March" claims.** Every funcmap row marked `= March`; in particular the `DeleteTrapBlade` bodies inlined in `32c971b0` / `32c98c80` / `32c9e0d0` (the `+0x220` set clear, `m_hTrapBlade`, `m_bUseTrapBlade`) and `EntireExplosion` (`32c9af70`, CH1k's row: counts NPC 1421).
- **Templet swap.** Every term that reads a class-24 skill id in the new code reads it through `GetNowStateSkillID()` + the swap flag, never a base id alone.
- **Play-test.** Read `logs.py --grep "RENA SKILL"` (four `equip` lines of set 1: 23241 / 23239 / 23224 / 23238 in states 49 / 51 / 53 / 55; set 2: 23036 / 23033 / 23053), `--grep "RENA TRAP"` (per cast: `spawn npc`, `linked npc ... owner`, `[JUNE_CHAR] SUMMON`, `removed ... uid`), `ErrorLog.txt` (0 Motion Error / missing mesh or particle), and the `[JUNE_SKILL]` lines against `MODS.md` "Stage 7 Rena, CH2d". CH2d0 left a question open: a trap that "vanishes at once" is a finding.

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL --with SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG --range <CH2c commit>^..<CH2d commit>`, the same for `..._TRAP` with `..._TRAP_DEBUG` over CH2d0, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`, a one-file `/Zs` probe of the cpp. Retire `_SKILL_DEBUG` and `_TRAP_DEBUG` together (`//#define`; a build with the only change a commented `_DEBUG` define starts but is not awaited).
Fix clear errors in place (bytepatch only). Write "CH2d - Review" in the plan doc (CH2c's findings in it), update the STATUS rows (CH2d done, CH3a ready), and check `CH3a.md`.
