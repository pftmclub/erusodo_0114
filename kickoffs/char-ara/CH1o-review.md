# June character Ara (Stage 7) - Phase CH1o review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1o review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1o done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (`//#define`), `_CORE` (defined) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CH1o - Done"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1o - Review"
awk -F'\t' '$7=="CH1o"' june2017_re/char_ara/ch0_class_map.tsv
grep "Ara CH1o\|^32610d50\|^3275a410\|^32b92f\|^32bab110\|^32bab6b0" june2017_re/funcmap.tsv
```

## Task

Review CH1o against the exe bytes (`june2017_re/charport_adis.py ara`, `disasm_range.py`, `vtslot.py`, `dispsweep.py`). Spot-check at least:
- **`SetSkillLevelStateData`:** the C++ = `3201f850` (Rena's CH1o review bytes); Ara has no 0x7a8 override and calls `3201f850` directly.
- **0x6b4:** vtable `33505314` + 0x6b4 = `32ba4620`; `32d3f140` = March's `GetTriggerSecretSkillState` (future-state formula, `+0x18` / `+0x1c` / `+0x14`,
  `32d3ff20`, `32d3fd50`, `32070da0`, the `test ah,5; jnp` direction); the moved body's jump table (base 27014, cases 0 / 6 / 10 / 12 / 25) and the
  divergent row. Nothing else calls `ReplaceStateIDBySkill`.
- **0x2fc:** `~DamageData` `31e9bb70` really is the destructor and `32610d50` its first call; `32610d50`'s handle `+0xc` and slot; the NPC vtables that
  point at `3275a410` and its owner `+0x8d5c`; no other `[r+0x2fc]` virtual caller (the sweep found three sites: `32610d79`, `32610d89`, `3275a43a`).
  Judge the teardown note (members destroyed while the observable lives).
- **0x74c:** `3262efa4`-`3262efd7` (owner via `31f8b560`, `+0x264`, CEffect `+0x7ac` = `DamageData` `+0x5ac`, skip target `3262f54e` = next unit) and that
  the March site after `X2DamageManager.cpp:2998` matches June's order; `vtslot.py 0x74c CX2GU` (only Ara overrides).
- **`32bab110` / `32bab6b0`:** line by line (the `+0` store before the ready test, the `> 0` stop, the second loop's `> -1` gate); JUNE_BUGS 601.
- **`32b92fa0` / `32b92fb0`:** the two `DamageReact` call sites and the CH4a amendment.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_ARA_CORE --range <CH1o commit>^..<CH1o commit> --files
X2Lib/X2GUAra.cpp X2Lib/X2GUAra.h X2Lib/X2GUUser.h X2Lib/X2GameUnit.h X2Lib/X2GUNPC.h X2Lib/X2GUNPC.cpp X2Lib/X2DamageManager.h X2Lib/X2DamageManager.cpp`
(FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; `python tools/enumscan.py --check X2Lib/X2GUAra.cpp
X2Lib/X2DamageManager.cpp X2Lib/X2GUNPC.cpp`; one-file `/Zs` probes of `X2GUAra.cpp` off, `_LIVE`, `_LIVE` + `_FP` + `_SPECTER`, and `X2DamageManager.cpp`,
`X2GUNPC.cpp`, `X2GUUser.cpp` off / `_LIVE`. CH1o left no `_DEBUG` define. Fix clear transcription errors in place (bytepatch only). Write "CH1o - Review"
into the plan doc, update the STATUS rows (CH1o done, CH1p ready), check `kickoffs/char-ara/CH1p.md`, commit with explicit pathspecs.

**Nothing to play-test.** Next: `/x2-phase char-ara CH1p` (Opus 5.5 high).
