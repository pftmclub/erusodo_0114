# June character Eve (Stage 7) - Phase CH1h review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1h review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1h done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1h - Done"
awk -F'	' '$6=="CH1h"' june2017_re/char_eve/ch0_class_map.tsv      # 11 rows
grep -a "Char Eve CH1h" june2017_re/funcmap.tsv                      # 11 PORTED rows
```

The 11 members are in the eighth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (it starts with the heading comment "Eve's Architecture states"); declarations at the end of the class in `X2Lib/X2GuEve.h`. No new enumerator, field or helper (`ENSI_JUNE_DASH_COMBO_ZX` came with CH1e).

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1h --out <scratchpad dir>` (11 files; the listing now prints full wide strings, CH1h fixed the 12-character cut) and a Ghidra decompile where a function object exists. You decide:

- **Is "= March" right?** `32966320` (Init), `32966350` (End), `3296ecc0` (EventProcess) and the Init `3296c580` (March's switch on the unit class, here the templet class) are March's bodies; check the three mesh names, the two Start / End tail jumps (`0x7c4` CommonStateStart, `0x7dc` CommonStateEnd).
- **XXZ4 FrameMove `32985300`**: the damage effect carries `GetPowerRate() x +0x4064 x +0x97c0`, force-down `+0x97c4`, land 0, range 1.0, the rest default (the push tail -1 / -1.0 / 0 / -1.0 / 0 / 0 equals `CreateInstance`'s defaults); positions (`y +125`, `+/- dir x 60`), the 0.01 / 1.07 timers, the mesh and particle tags (7c: `329854ac`, `329856eb`), no `SetLandPosition` on the 1.07 particle.
- **XXZ4 EventProcess `3296c5d0`**: off line -> JUMP_DOWN and `y -= 45`, then a separate animation-end `if`, then the four cancels as `else if` arms (animation end is first, unlike dash Z-X).
- **Dash Z-X (June-only, script id 180)**: FrameMove `32986d30` plays `EffectSet_EVE_EAT_DashComboZX` with the factored power rate; June also passes `+0x97c4` as a trailing PlayEffectSet float that March's signature lacks (see the Done note, "Drift"); decide whether that gap needs its own phase. EventProcess `3296df00`: chain after the four cancels is two `else if` arms on `m_fAttackCancelAfter` (`+0x8c8c`), pureZ -> vslot `0x4b0` GetComboZStateID, pureX -> `0x4b4` GetComboXStateID, both with the on-line test (March's `ZX_CANCEL_AFTER` has none), then the animation-end `if` after the chain on every path.
- **Dash-jump X-Z FrameMove `329889d0`**: the class switch only wraps the 0.01 block; 0.1 block power factor and `m_vSummonedUnitBonePos` (y +150); 0.15 mesh at `m_vSummonedUnitBonePos`; three tags (`32988afd`, `32988bdd`, `32988e18`).
- **JUNE_BUGS**: none was written; say so if you find one.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1h commit>^..<CH1h commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1h - Review" into the plan doc under the Done note, update the CH1h row in `STATUS.md`, check that `kickoffs/char-eve/CH1i.md` still matches, and commit with explicit pathspecs.
