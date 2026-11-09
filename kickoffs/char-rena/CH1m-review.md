# June character Rena (Stage 7) - Phase CH1m review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1m review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1m done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`; `InitStateCommon` and `InitState` bodies, six new `#error` guards in `Always.h`), `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (nested: seven `ADD_RENA_SYSTEM`-only callbacks and the natural-force buff in the tail), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (enum member `LESI_RNW_DASH_JUMP_XZ_LOOP`, June id 278) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1m - Done"
awk -F'\t' '$7=="CH1m"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1m - Review"
```

## Task

Review the registration against June, from fresh decompiles of `32ca9dc0` (InitStateCommon) and `32cadc90` (InitState) and the listing where
the decompile is unclear. Spot-check at least:
- **Simulation.** `june2017_re/char_rena/ch1m_regsim.py` over the fresh decompiles equals `ch1m_reg_by_class.tsv` row for row with no
  `UNKNOWN COND` except the NF count test @1705. The simulator folds two class reads ( unit `+0x38`, templet `+0xaf0` -> `+4` ) and takes
  the unit / templet null tests as not null: confirm both reads are the unit class. `ch1m_regcheck.py`: `matched 992 registrations, 0 problems; ids mapped 172`.
- **Head, from the exe bytes.** Cut-in index `+0x8ac8` ( 3 / 12 / 13 / 24 -> 0, 102 -> 1, 103 -> 2, 114 -> 3, no default write ), sub-index
  `+0x8acc` ( 12 -> 1, 13 -> 2, else 0 ), the inlined `InitStateID` ( `+0x4088` = 0xd, `+0x7d04` = 0xa0 ) = March's `InitStateID`, `31fc93d0` not called.
- **START / WIN / LOSE.** The three OUT strings of `31fe22d0`; WIN's compare @32cadfac runs on the WIN string ( `[esp+0x88]` at the call )
  and sets byte `+0x9714` = `m_bShowTrapBladeToWinMotion`; `0x82` @32cae131 -> `InitWaitStateChange( "LESI_WAIT_STATE_CHANGE" )`.
- **Class tests.** The nine tests ( decompile @306 / @857 / @1031 / @1095 / @1160 / @1220 / @1254 / @1345 / @1384 ) against the listing
  ( shared-call tails ); the C++ has one `switch( eClass )` per block in June's order.
- **Ids.** `ch1m_idmap.tsv`: one-to-one both ways; 213 = `LESI_LGA_DASH_JUMP_Z` ( the Combat Ranger Z1 table, as March ); 278 = the new member;
  253 / 257, 254 / 258, 255 / 259, 256 / 260, 261 / 264, 262 / 265, 263 / 266, 269 / 270 share tables with their own members.
- **InitStateCommon.** Id 23 = `GetHyperModeTableName( false )` ( `31fdd490( ..., 0 )` @898 ), FrameMove `32adf320` = vslot 0x600, End
  `329cc0e0` = vslot 0x604 ( not overridden: `CX2GUUser::HyperModeEnd` ). LESI_DASH's class arm ( 114: `32c6e0c0` / `32c6e0f0` ).
  Bounding 120-122 at June's place; riding 93-96, phoenix 123-126, dance 127-129 left out with owner comments.
- **`_NF` callbacks.** `WalkStart`, `DashStart`, `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX_StateStart` / `_FrameMove` / `_EventProcess`,
  `DashJumpComboX3StateStart`, `LESI_LGA_ZZXX_StateStart` ( CH0's `inside (ADD_RENA_SYSTEM)` rows ) with March's flag-off pair in `#else` where one exists.
- **Tail.** 0x690, 0x684( 0 ), `31fd8f00`, `32c96d60` = `StateInitTrapBlade` ( = March ), then `32c85200( 1 )` when the count > 0 ( `_NF` ).
- **JUNE_BUGS 571** ( `LESI_RIDING_DAMAGE_BACK` has no table; FRONT defined twice ).

Hygiene: `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --range <CH1m commit>^..<CH1m commit> --files X2Lib/X2GULire_ElvenRanger.cpp KTDXLIB/Always.h`
( FAIL only for the commented-out `#define` ), `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range ... --files X2Lib/X2GULire_ElvenRanger.h`,
`junediverge.py --check-all`, `enumscan.py --check` on the class files, one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off and with
`-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`. Fix clear transcription errors in place ( bytepatch only ). Write "CH1m - Review" into the plan doc,
update the STATUS row, check `kickoffs/char-rena/CH1n.md`, commit with explicit pathspecs.
