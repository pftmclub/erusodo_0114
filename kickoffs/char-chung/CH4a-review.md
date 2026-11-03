# June character Chung (Stage 7) - Phase CH4a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH4a review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH4a done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE` (+ `_DEBUG`), `SERV_IRUHADEV_JUNE_USER_MP_GAIN_ON_HIT` |
| Log tag | `[JUNE_CHAR]` (`CHUNG PASSIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH4a - Done (2026-11-03)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH4a - Done (2026-10-28, Opus 5.5)"
```

## Task

Check CH4a against June (read-only; read the listings, not only the decompiles). In order of risk:
- **`InitPassiveSkillState` `32c16be0`** block by block against `X2GUChung.cpp`'s `_PASSIVE` branch: the skill id, the GetSkillLevel arguments
  (`0,0,1` for 26201 and the swapped 26202, `1,0,1` for the rest), each SA type -> member pairing (the Done note's offset table), the resets
  before each block (values and which members are *not* reset), the `rate - 1.0 + 5e-8` sites ( Remodel Rifle, Guard Mastery / Tussle anim speed,
  Tussle force down ), and 26413's `32071720` power rate with the `-1` reset (JUNE_BUGS 515).
- **The two callbacks** `32c12910` (DamageData `+0x470` defender, `+0x5c4` applied skill: the five ids, vslot `0x238( 26416, 0, 1 )`, defender vslot
  `0x224`) and `32bdba10` (`+0x20` hit type 0x21 / 0x22, `+8` == 2, vslot `0x23c( 26417, 0, 1 )`), and the two pushes on `+0x66ac` `@32c177e3` / `@32c1780f`.
- **`SetSpecificValueByBuffTempletID` `32bec1a0`** (not carved: `june2017_re/funcdis.py --func 32bec1a0`): BTI 45 / 54 bodies, the pulse arguments, the
  `3201ad10 + 1.0` factor; `32bda7d0` = March's live UnSet (no change).
- **User MP gain on hit**: `32012b50 @32013355-320134bd` (four ids, AT_SPECIAL skip, `+0x93b8` add) against `X2GUUser.cpp`'s block, `31fc94d0`'s `>= 1.0`
  compare, ctor `@32041d30`, base reset `@31fe1db8`.
- **Play-test results** (units 18 / 43 / 50): the three `CHUNG PASSIVE init` lines per load, `guardian force`, `open mind`, `land demolisher`,
  `sharpshooter syndrome` lines, CH3c's `extra 6` on unit 18, `ErrorLog.txt`.

Retire `SERV_IRUHADEV_JUNE_CHAR_CHUNG_PASSIVE_DEBUG` (comment out its `#define`) if the logs were read.
Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_PASSIVE --with JUNE_CHAR_CHUNG_PASSIVE_DEBUG --range <CH4a commit>^..<CH4a commit> --files KTDXLIB/Always.h X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h X2Lib/X2SkillTree.h`,
the same for `JUNE_USER_MP_GAIN_ON_HIT` on `X2Lib/X2GUUser.*` (the tool cannot evaluate `X2GUUser.cpp`'s compound `#if` when it checks another flag),
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUUser.cpp`. Write "CH4a - Review" into the plan doc, update the STATUS rows
(CH4a2 and CH4b unblock) and commit.
