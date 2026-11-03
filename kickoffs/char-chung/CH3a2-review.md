# June character Chung (Stage 7) - Phase CH3a2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH3a2 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3a2 done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`, retired by this review) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH3a2 - Done (2026-11-03)"
grep -a "Char Chung CH3a2" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH3a2 --out <short scratch dir>
```

## Task

Check CH3a2 against June (read-only; read the listings, not the decompiles). In order of risk:
- **G.U.A.R.D. first strike `32bfbc10` / `32be1ba0`.** `EventTimerOneshot( 0.05 )`, the ball / berserk gate (`+0x110` of the gage = now cannon ball count), `UseCannon( 1 )`; the event process branch order
  (cancel chain, then `bEventFlagList[0]` with `+0x8c18` / animation end -> state 0x12a, then the `+0x8c10 <= anim < +0x8c14` key window, then animation end -> WAIT) and the `jnp` / `jp` reads of the `fcompp` pairs.
  Check the Lua `EVENT_PROCESS_TIME0 = { 0.55, 0.6, 0.6 }` order (start, end, state change) against `m_fEventTime[0]`.
- **Registration `32c1f530`.** G.U.A.R.D.: normal and hyper both the FIRST_STRIKE table with `StartFuture` / `FrameMove` / `EventProcess` / `End`; the five reg states take only their own callbacks
  (`UserUnitStateData::Init` before each, so no copy from the first strike). Iron Howling: hyper reads the normal table (`0x3350f3e0` twice), copies `+0x1c..+0x48` but not `Init`; LOOP 321 / END 322.
  Confirm Caladbolg and Wonder Wall need only the labels.
- **Iron Howling `32be11a0` / `32be1260` / `32be12c0` / `32be1390` / `32be13b0`.** `32007ce0( 0x1b, id, 1, &out )` argument order (default 10.0), `vslot 0xe4` = `FlushMp`, the `setge` loop test (`>= 5`),
  `+0x6890` = `SetNoDetonation`, the END chain and the JUMP_DOWN / WAIT pick.
- **Wonder Wall `32c156c0` / `32be6400`.** `g_pMain +0x8c == 9` = `XS_PVP_GAME` (indexes 0 / 1 or 2 / 3), the size > 3 test, the unit-null early return (no `CommonFrameMove`), user slot 476 once per ally, `160000.0`.
- **Damage event process `32bf2f70`.** The oneZ / oneX / key-up / cancel order, `0x672f` = Open Mind 26415, `jnp 32bf3009` on the random pair.
- **The "= March" rows** (funcmap `Char Chung CH3a2`): confirm 4 against the listing, e.g. `32be1ff0`, `32be2f60`, `32bf4280`, `32bd9760`.
- **Play-test results:** the `iron howling start ends` / `loop N` / `loop ends` lines (five loops at most, MP drops by the cost per loop), `guard first strike, balls now N`, `wonder wall buff, level L, allies N`,
  `ES_CREATE EffectSet_CFG_A_Guard_First_Strike`; the missing-sound check of JUNE_BUGS 505's kind (ErrorLog).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_SKILL --range <CH3a2 commit>^..<CH3a2 commit> --with JUNE_CHAR_CHUNG_SKILL_DEBUG`, `git diff --patience --numstat`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`, `ch1m_regcheck.py`. Retire `_SKILL_DEBUG` (comment out the define, note it in the review). Write "CH3a2 - Review" into the plan doc,
update the STATUS rows (CH3b unblocks) and commit.
