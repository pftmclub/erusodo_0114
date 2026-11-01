# June character Chung (Stage 7) - Phase CH1m review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1m review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1m done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1m - Done (2026-11-01)"
grep -a "Char Chung CH1m:" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung 32c22540
```

## Task

Check CH1m's registration against June (read-only in Ghidra). In order of risk:
- **The simulation.** Fresh decompiles of `32c12a30` / `32c1d1d0` through `june2017_re/char_chung/ch1m_regsim.py` must reproduce
  `ch1m_reg_by_class.tsv` row for row; its condition overrides (`0x867c` unit present, `iVar4 != 0` Back Blast, the shared_ptr releases) are the
  places it could be wrong. `ch1m_regcheck.py` must print `0 problems`.
- **The gates of `32c1d1d0`, against the listing:** the first switch's targets (20 joins the default's tail at `code_r0x32c1f09d`; 27 / 117 jump to
  `LAB_32c1f0af` with ids 0x115 / 0x134), the 0xb6 tail (table per class: `CSI_ATTACK_DASH_X` / `CSI_CSG_COMBO_DASH_X` / `CSI_CIP_COMBO_DASH_X`), and the
  second switch's fallthroughs 110 -> 20, 117 -> 27 (21 / 111 together, the rest default).
- **`32c22540` (not carved):** cut-in bytes `32c22af8` / targets `32c22ad8`, the inlined InitStateID (`+0x7d04` 0xa2, `+0x4088` 0xd), the call order
  (`32c12a30`, `3202b110`, `32c1d1d0`, `31fe22d0`; C++ calls InitStateByUnitClass before InitStateCommonRandom, which runs 3202b110 at its head), the four
  registrations' slots (stateData at `esp+0x14`), the absence of WIN2 / LOSE2 flag tests, the tail vslots 0x690 / 0x684( 0 ) / `31fd8f00`.
- **Ids by name:** `ch1m_idmap.tsv` one-to-one both ways; June 72 = `GetRidingStartStateID()`, 181 = `CSI_COMBO_DASH_Z` for the four dash-Z tables.
- **Callbacks June takes from `CX2GUUser`** where March registered Chung's own: `DamageBigStart` (same body) and `DashJumpFrameMoveFuture` (June's base, no 1.6x).
- **Left out / routed:** phoenix run 123-126, dance 127-129, riding 93-96 and the empty slots (`3202a7d0`, `32002f70`); Back Blast 335 and the siege-shelling
  StateStart `32bda6c0` are in CH2a's kickoff.
- **Always.h:** the `_LIVE` block's new `#error` guards (SCALE_HEAD, ARME_CORE, EVE_CORE, HYPER_MODE_FIX, COMMON_RANDOM_STATE, EVENT_STATES,
  WAIT_STATE_CHANGE, BOUNDING_STATE) each name a flag the registration needs.

Hygiene: `flagcheck JUNE_CHAR_CHUNG_LIVE --range <CH1m commit>^..<CH1m commit> --files X2Lib/X2GUChung.cpp KTDXLIB/Always.h` (the default diff misaligns
the long insertion: confirm with `git diff --patience --numstat`, 1812 / 0), `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUChung.cpp`, a
one-file probe of `X2GUChung.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE`. Write "CH1m - Review" into the plan doc, update the STATUS rows
(CH1n becomes ready), check `kickoffs/char-chung/CH1n.md` and commit with explicit pathspecs.
