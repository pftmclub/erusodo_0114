# June character Aisha (Stage 5 pilot) - Phase CH1j review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1j review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1j done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (the `InitState` gate: `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE`, undefined) |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1 split (2026-10-21, user)" "CH1j - Done (2026-10-21, Opus)"
grep -a "Char CH1j" june2017_re/funcmap.tsv                  # the rows the phase wrote ( confirmed pairs, PORTED, ROUTED stand-ins )
head -5 june2017_re/char_arme/ch1j_reg_by_class.tsv            # per-class registrations from charport_regsim.py
```

The code is the `//{{ ... Phase CH1j` block at the end of `X2Lib/X2GUArme_VioletMage.cpp` ( `June_InitStateCommon`, `June_InitState`,
`June_LoadAngorHammerMesh` ), the gate at the top of `InitState`, and the CH1j block after `JuneInitBoundingStates` in `X2Lib/X2GUUser.cpp`.

## Task

What the phase could get wrong, in the order of risk:
1. **The path simulation** (`june2017_re/charport_regsim.py`). Re-run it on fresh decompiles of `32cea0c0` and `32ce77b0` and diff with
   `ch1j_reg_by_class.tsv`. Check three branch points against the listing: InitStateCommon's templet test `@32ce8eb0` (templet `+4` taken as the
   class, Battle Magician / Dimension Witch take the ABM combos), InitState's class chain `@32ceae8b` / `@32ceae9b` jumping to `32ceb027` (the Void
   Princess shares the Dark Magician ZZupZZ), and the last switch's default `@32ceb6bb` to `32ceb989` (which classes skip its blocks).
2. **Id -> enumerator mapping**: 177 = `AVSI_DASH_COMBO_Z3up`, 237 = the new `AVSI_DIMENSION_WITCH_DASH_ATTACK_X`, 72 = `USI_RIDING_START`; spot-check five
   shared ids (166, 168, 171, 178, 183) against the per-class table.
3. **CX2GUUser pairs**: pick five `confirmed` rows and check the June function against the March member body ( e.g. `31fd23e0` WalkFrameMoveFuture,
   `31fcc0e0` DamageStandUpEventProcess, `329cc1f0` DashStartFuture, `31fc8050` HyperModeStartFuture, `32030580` RidingOnEventProcess ). A pair whose
   bodies differ is a June change March lacks: route it (CH1k if it is a common state, Stage 17 if riding).
4. **The new CX2GUUser members**: `31fd2520` (both keys held counts as forward; no down-force branch), `31fc7f50` (`+0x8bf8` = `m_bDownForce`,
   `+0x314` = `fDownAccel`, `+0x31c` = `fGAccel`), `31fdd490` (unit type 11 shares RSI, the transcended class table, `+0x3c`).
5. **What was left out**: the die states' StateEnd ( `CX2GUUser::StateChange` already runs `3202a7d0` ), the riding / phoenix / dance ids, and the
   13 stand-in or empty slots. Confirm each owner (CH1j2, CH1k, Stage 10, Stage 17).

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1j commit>^..<CH1j commit>`. Paste its `files:` / `in this flag's files:` /
`RESULT:` lines into the review. The OTHER row is the LIVE gate; the 4 deleted lines are CH1h's hoisted members.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear errors under the same flag (then
`tools/build.py --deploy`); hand design decisions back to the user. Write "CH1j -- Review" into the plan doc under the Done note, update the CH1j
row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1j2.md` still matches, and commit with explicit pathspecs.
