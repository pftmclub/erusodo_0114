# June character Chung (Stage 7) - Phase CH3b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH3b review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3b done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`, retired by this review) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`, `SUMMON`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH3b - Done (2026-11-03)"
grep -a "Char Chung CH3b" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH3b --out <short scratch dir>
```

## Task

Check CH3b against June (read-only; read the listings, not the decompiles). In order of risk:
- **Shooting Star `32beb940` @32beb944-32beb9e1 / `32bfc310`.** The play count: base 12, 15 when `gage +0xe8` (hyper time) > 0, 18 when `GetTranscendenceMemoSkillID( 26015 )` is the same-or-upgrade
  skill of `0x65ae` (26030, `31edf580` = `CX2UISkillTreeNew::IsSameOrUpgradeSkill( memo skill, 26030 )`), then `+ (int) 32007b10( 0x1e, 26418 )` when `31fceab0( 26418 ) > 0`
  (`0x1e` = `SA_PROJECTILE_FIRE_COUNT`). In the frame move: the shell offsets (`+- dir x 70`, y + 130, `GetZVector x 40`), the Endless Chase block (`32007b10( 0x1d, 26418 )` scales
  `+0x970 / +0x974` and `+0x978 / +0x97c` = the dungeon / PvP target range min / max), `SetLockOn( effect, playCount, 4, max )` after it, and no memo 23 branch.
- **Bullet Blitz `32bffd20` @32c002b3-32c00385, `32be3f50`, `32be3fe0`.** One function for both tables; shell `Damage_Chung_CDC_Bullet_Blitz`, first hit `..._Mini_Gun_Hit_Skill`; the Endless Chase tail
  (main effect `+0x44c` x `32007b10( 1, 26418 )` = `SA_EFFECTIVE_TIME`, `+0x970..+0x97c` x `32007b10( 0x1d, 26418 )`); the start: `GetEquippedActiveSkillTemplet( 0x65a2, 1 )`, level `GetSkillLevel( id, 0, 0, 1 )`
  min 1, `32206e50( 2, level, 0, 0.0 )`, hyper `+ 5`. JUNE_BUGS 511 (`+0x9700` unread) needs a reader search over the exe.
- **Rapid Cannon `32c079f0` / `32bee210` / `32bee140`.** `edx = 2 x sete + 6` (FM, fires while `fire <= edx`) and `+ 5` (EP, `edx <= fire` -> FINAL 0x13f); `0x6663` = 26211 and `0x666d` = 26221;
  the inlined ball count (`ecx x 8 - ecx + now`) in all three; minimum `fire >= 2` with `JuneSkillSlotKey::IsKeyReleased`. JUNE_BUGS 510 (x 7 vs `32bed560`).
- **Rumble Shot `32c0dfe0`.** `UseCannon( 1 )` at 0.01 and the single `DAMAGE_CSI_SI_A_CSG_RUMBLE_SHOT_Shell` at 0.24 (no memo 13 branch).
- **Shooting Bind `32bdba90` / `32bdbab0`, Burning Punisher `32bdbb40`.** `+0x98d4` = 10.0 / `32007b10( 0x94, 0x65b7 )` (`SA_COOLTIME_1` = June 148); the 11 preloads in order and their kind (texture / skin mesh).
- **Registration `32c1f530`.** The labels (Heavy Railgun 26016 / 26053, Shooting Star 26015 / 26051, Rumble Shot 26210 / 26233, Comet Crasher 26017 / 26054, Bullet Blitz 26018 / 26055, Rapid Cannon 26211 / 26234,
  Missile Shower 26025 / 26060) and the four script-only blocks (`GenericSpecialActiveSkillEventProcess` + `CopyStateCallbacks`); `ch1m_regcheck.py`.
- **The "= March" rows** (funcmap `Char Chung CH3b`, 26 rows): confirm 5 against the listing, e.g. `32bd9c70`, `32be4070`, `32bfd2b0`, `32be8360`, `32bf3900` (the listing runs into the next function).
- **Play-test results:** `shooting star, max plays N`, `bullet blitz endless chase, life ..., range ...`, `rapid cannon fire N, limit 6`, `shooting bind start / end`, the `ES_CREATE` lines of the effect sets,
  `ErrorLog.txt` (no missing mesh / particle / sound of JUNE_BUGS 505's kind).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_SKILL --range <CH3b commit>^..<CH3b commit> --with JUNE_CHAR_CHUNG_SKILL_DEBUG`, `git diff --patience --numstat`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`, `ch1m_regcheck.py`. Retire `_SKILL_DEBUG` (comment out the define, note it in the review). Write "CH3b - Review" into the plan doc,
update the STATUS rows (CH3c unblocks) and commit.
