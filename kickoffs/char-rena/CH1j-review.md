# June character Rena (Stage 7) - Phase CH1j review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1j review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1j done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`; `CommonStateStart`, `CommonFrameMoveFuture`, `CommonFrameMove`, `CommonStateEnd`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; 3 functions, 8 members), `..._NF` (undefined; 4 lines), `SERV_IRUHADEV_JUNE_ENUM_NAMES` (7 `RNSI_` ids) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1j - Done"
awk -F'\t' '$7=="CH1j"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1j - Review"
```

## Task

Review the 10 CH1j rows against June (`python june2017_re/charport_adis.py rena --phase CH1j --out <dir>`; `32c83280`'s listing runs on into the next function at `32c83320`, which is not CH1j's). Spot-check at least:
- **`CommonStateStart` `32ca81b0`:** decode the five xor key tables by script from the listing (tables at `0x337ae670` BOW_SKILL_STATE 15 bytes, `0x337ae658` DAMAGEDATA_CHANGE_TIME, `0x337ae640` `...TIME1`, `0x337ae628` `...TIME2`, `0x337ae614` CREATE_TRAP_BLADE 17 bytes) and check the order and the stores ( +0x981d, +0x9698, +0x969c, +0x96a0, +0x981c cleared first ). Then: the `_NF` block ( `m_SPLevel > 0`, skill id != +0x9758, `GetSkillTemplet( id, 1 )`, type != 6 `ST_ACTIVE`, byte of templet +0x10 = low byte of `m_usFPConsumtion` ( every Rena skill row in `NewSkillTempletVer2.src.lua` has 1 / 2 / 3 or 0 ) into `SetNowDamageRelateSkillData` ); Sharp Kick 0x5b68 -> ability 0x28 `SA_FORCE_DOWN_MELEE_REL` on `fForceDownValue` ( +0x790 ) only when `m_bMeleeAttack` ( +0x3cc ) and `attackType` ( +0x3a8 ) != 1; Hunter's Ability 0x5b70 -> ability 0x27 `SA_FORCE_DOWN_REL` only when +0x96a4 == 1; Predator 0x5b79 learned and the upgrade base id of the state skill in { 0x5a01, 0x5a02, 0x59ee }: `GetRandomFloat( 0 )` ( vslot 0x1cc ) -> stage -> multiple, `m_fNowStatePowerRate` ( +0x7cc8 ) `*=` it, +0x97f0 = stage 0 ( the empty `std::wstring::assign` at `32ca8b64-32ca8b7f` is dead and not written ); the tail ( vslot 0x198 `IsMyUnit`, state skill != 0x5a13 -> +0x8d90 = 0, +0x8da4 = 0 ).
- **`32c83280` / `32c6d800`:** abilities 0x73-0x76 = `SA_SKILLDAMAGE_MULTIPLE_01..04` and 0x77-0x79 = `_01_RATE..03_RATE` ( `cl` gave 115-118 and 119 / 120 / 121 / 122 ); the compares are `roll < running sum` ( `fcompp` / `fcomp st(1)` + `test ah, 0x41` + `jne` jumps when `sum <= roll` ).
- **`CommonFrameMove` `32ca8bf0`:** +0x981e = 0 before anything else; the Eradication countdown ( `0 < +0x9814` then `<= 0` -> 0 + `32ca2aa0` ); the 0x5aa7 / 0x5aaa ( = 23207 `RNSI_A_RWS_REFLEX_MAGIC`, 23210 `RNSI_A_RSR_MIND_OF_SHARPSHOOTER` ) upgrade block writing `bSuperArmor` ( +0x7ff4 ) and `bSuperArmorNotRed` ( +0x8040 ); the MP charge A / B / C, friendship, trap blade and prior planned blow blocks = March; 0x59e7 ( 23015 ) test -> `EndGungnirTargeting` ( `32ca7750` ); +0x97e8 minus the frame time ( no clamp ); the map walk at +0x97f4 ( node +0x20 -= frame time ). **Check the inferred map type** ( `std::pair< int, UidType >` -> float; key compare order in `32cb57b0` is the unit type, then the 64-bit uid ).
- **`CommonFrameMoveFuture` `32c87990`:** +0x980c += frame time first; the clamped `+0x959c` ( March's `m_fReAttackZ1Time` ) and `+0x95a4` countdowns; the NF sync. **`CommonStateEnd` `32ca5060`:** +0x981c / +0x981d tests, vslot 0x23c `( 0x5b77, 1, 1 )` = `SetSkillBuffFactors( RNSI_P_RGA_RANDOM_SHOT_PLEASURE, 1, true )`. **`32ca2aa0`:** hit count 0 and the effect set stop ( `push 0` is the stop's second argument, March's default false ).
- **"= March" rows:** `32c6a930` / `32c6a940` ( jmp thunks ), `32c6a950` ( base call, hint returned ).
- **Names:** the 7 `RNSI_` ids against `Enum.src.lua` ( 23022 / 23041 / 23042 / 23400 / 23408 / 23415 / 23417 ); the stand-in camera members and the three kickoff amendments ( CH1k, CH1o, CH3b, CH4a ) say what the Done note says.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE SERV_IRUHADEV_JUNE_CHAR_RENA_NF --range <CH1j commit>^..<CH1j commit>` ( FAIL only for the commented-out `#define` ); `python tools/flagcheck.py SERV_IRUHADEV_JUNE_ENUM_NAMES --range <CH1j commit>^..<CH1j commit>`; `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe ( `census.cl_cmd`, `/c /Zs /EHsc` ) of `X2GULire_ElvenRanger.cpp` off, with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place ( bytepatch only ). Write "CH1j - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1k.md`, commit with explicit pathspecs.
