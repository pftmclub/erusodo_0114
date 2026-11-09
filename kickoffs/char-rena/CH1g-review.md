# June character Rena (Stage 7) - Phase CH1g review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1g review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1g done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; 4 new `LESI_RTR_DOUBLE_JUMP_ZX_*` functions, their 4 declarations) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1g - Done"
awk -F'\t' '$7=="CH1g"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1g - Review"
```

## Task

Review the 46 CH1g rows against June (`python june2017_re/charport_adis.py rena --phase CH1g --out <dir>`; stop each listing at its first `int3` or the next class-map start; `32ca6410` runs into `32ca6640`, `32ca6be0` ends at `32ca6cc2`, `32ca0cd0` is followed by three sibling copies). Spot-check at least:
- **Class-map corrections.** CH0 listed six helpers as `new ?`; they are March functions: `32ca6410` `CreateTrapBlade`, `32c96e50` `DeleteTrapBlade`, `32ca69e0` `SetPriorPlannedBlow`, `32ca6b10` `EndPriorPlannedBlow`, `32ca6be0` `CheckPriorPlannedBlow`, `32c9a5b0` `InsertPriorPlannedBlowEffect`. Confirm June = March for each (thresholds 5 / 14 and levels 9 / 14, 10.0 s, `UpNowHp( max * rate * ( count - 4 ) )`, the `PlayEffectSet` defaults). `CheckPriorPlannedBlow` wraps the whole function (its signature line carries a Korean comment, so the `#else` sits above the signature).
- **The five combo EventProcesses** (`32c79470`, `32c79950`, `32c79be0`, `32c79f50`, `32c7a290`): flag / event-time pairs (ZZZ: 0 `oneZ`+up Z4UP, 1 `oneZ` ZZZZ, 2 `oneX` Z4a; ZZZZ: 0 `oneZ` ZZZZZ), the animation-end test next to the change-time test, and the cancel order (ZZZ / ZZZZ: dash, walk, skill, back-walk; ZZZ_DZ / ZZZZZ / dash Z-X: skill, walk, back-walk, dash), each with the foot test. ZZZZZ's 1.3 s walk exit reads the future animation only.
- **ZZZZZ FrameMove `32cad240`:** force down `-= down * ( count - 4 )` (March `count - 5`), `reActType` big damage, the two flags set, `CheckPriorPlannedBlow` last.
- **New `LESI_RTR_DOUBLE_JUMP_ZX_*`** (`32c6d580`, `32ca7670`, `32c818d0`, `32ca0cd0`): the state enum member already exists (CH1d2, 271); StateEnd deletes the blade before `CommonStateEnd` (June order); EventProcess has no `m_fCanNotAttackTime` and no jump-down branch.
- **Routed to CH1g2 / CH1g3:** the Night Watcher states call `CreateTrapBlade`, `DeleteTrapBlade` and the three PPB helpers; they are written, so those phases call them with June's arguments.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1g commit>^..<CH1g commit>` ( FAIL only for the commented-out `#define` ); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place ( bytepatch only ). Write "CH1g - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1g2.md`, commit with explicit pathspecs.
