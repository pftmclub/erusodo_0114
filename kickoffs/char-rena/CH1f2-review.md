# June character Rena (Stage 7) - Phase CH1f2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1f2 review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1f2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined; 12 enum members, 29 new functions, 2 helpers, 6 Gungnir members), `SERV_IRUHADEV_JUNE_ENUM_NAMES` (`BTI_BUFF_GUNGNIR_SCOPE` 537 in `X2BuffFactor.h`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1f2 - Done"
awk -F'\t' '$7=="CH1f2"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1f - Review"
```

## Task

Review the 37 CH1f2 rows against June (`python june2017_re/charport_adis.py rena --phase CH1f2 --out <dir>`; stop each listing at its first `int3` or at the next class-map start; `32c78f10` runs into `32c78f40`, `32c815a0` into `32c815d0`). Spot-check at least:
- **Class-map corrections.** `32c86890` = March's `ShowActiveSkillCutInAndLight` (not a new helper; the item override `31fd8a50` and the event 0x8b3 pair are left for CH5's cut-in table); `32ca7750` / `32ca9740` named `EndGungnirTargeting` / `PlayGungnirAttackEffect`.
- **Gungnir control `32ca1360` (the hard one).** The two `-750` tests (`@32ca150d`, `@32ca19bb`: `fcomp` then `jp` / `je`, the clamp block `32ca151d`-`32ca1561` dead, the release always fires), the uninitialised `[esp+0x24]` (stand-in 0, `JUNE_BUGS` #567 / #568), the 1500 test on the NPC's y, the nine-way picker (`UP_LEFT` first, gated by gap X >= 0 and gap Y <= 0 or no NPC; then `UP_RIGHT`, `DOWN_LEFT`, `DOWN_RIGHT`, `LEFT`, `RIGHT`, `UP`, `DOWN`, idle; each tests `GetNowStateID()` first), the skill id `GetFutureStateSkillID()` (`[ +0x7cb0 protected ]->+0xc`).
- **Gungnir states.** READY FrameMove `32ca0f80` (0.06 cut-in, 0.001 s oneshot, 120 ahead / 300 up, `CanDown` June shape: in index passed, out `NULL`, 240 above the unit when no line, `CreateNPCReq` 2083, `SetBuffFactorToGameUnitByBuffFactorID( 627, 1, NULL, 0, false )`); READY EventProcess `32ca1240` (0.6 s, 10 s, `IsEventGamePlayable( false )`, the three NPC camera calls with no `IsMyUnit` test); FIRE Start / End / FrameMove / EventProcess; the eight direction StateStarts (identical but the NPC state name); `PlayGungnirAttackEffect` (`+0xe8` of the gage = `GetRemainHyperModeTime()`, y + 4000, `PlayEffectSet` argument order); `EndGungnirTargeting`.
- **LGA / RGA dash-jump line.** `32c78f40` (landing flips `bIsRight` and `dirDegree.y + 180`, X follow-up to 268), `32c815d0` (`oneZ` to 268), `32c81340` = `32c78be0` instruction for instruction (checked with a normalised diff), MP values 2 / 9 / 2 / 8 / 3, arrow names, `Dummy1_Rhand` (ZZXX) against `Dummy2_Lhand` (ZXX).
- **Open for CH1h (constructor).** The new members `m_iGungnirTargetNPCUID` (-1) and `m_hGungnirLoopEffect` (`INVALID_EFFECTSET_HANDLE`) need the constructor's initialisers; confirm `32ca2b60` writes them and that CH1h's kickoff carries it.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1f2 commit>^..<CH1f2 commit>` ( FAIL only for the commented-out `#define` ); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place ( bytepatch only ). Write "CH1f2 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1g.md`, commit with explicit pathspecs.
