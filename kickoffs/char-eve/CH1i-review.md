# June character Eve (Stage 7) - Phase CH1i review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1i review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1i done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1i - Done"
awk -F'	' '$6=="CH1i"' june2017_re/char_eve/ch0_class_map.tsv      # 22 rows
grep -a "Char Eve CH1i" june2017_re/funcmap.tsv                      # 22 PORTED rows
```

The 22 members are in the ninth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (heading comment "Eve's Code: Empress states"); declarations at the end of the class in `X2Lib/X2GuEve.h`. One new enumerator, `ENSI_ECE_COMBO_ZZXX` (script id 235), after `ENSI_ECE_COMBO_ZZX`; no new field or helper.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1i --out <scratchpad dir>` (22 files) and a Ghidra decompile where a function object exists. You decide:

- **"= March" pair bodies**: the four Inits (`32966f00` `32966f30` `32966f60` `32966fa0`) and the four EventProcess (`329715a0` `32971700` `329717f0` `32971820`): strings in order, the 0.46 / 0.466667 / 0.6 / 0.666667 compares, the key lists (ZZ is ZXCAS with LCtr, the others ZXC), the unit-condition order.
- **Effect-set FrameMoves** `3298e860` `3298ea30` `3298eff0`: one `EventTimerOneshot( 0.01 )` + `PlayEffectSet` with the factored power rate; June's trailing force-down float (`+0x97c4`) has no March parameter (CH1o owns it). The three differ from the CH1h dash Z-X listing (`32986d30`) only by the effect-set name length.
- **Jump Z-X `3298ec00` and Z-Z-X-X `329998f0`**: the same listing with 0.06 / 3 MP and 0.4 / 4 MP; `FlushMp( n, 0 )` (vslot `0xe4`, March has one argument); failure path = `CreateNotEnoughMPEffect` (`32007d80`) at the mid-point of `Dummy1_Rhand` / `Dummy2_Lhand`, angles `0, 0, GetRotateDegree().z` (re-derive the stack offsets); success path = two `PlayEffectSet` calls in order JumpZX, JumpZX_Grenade.
- **Z-Z-X `329994e0`** is op-for-op `32985300` (CH1h); **EventProcess `32976110`**: animation end first, then flag 0 with `m_fEventTime[0]` (`+0x8c10` start, `+0x8c14` end, `+0x8c18` state change), the condition order (class 109 through the unit data `+0x38`, then `oneX`), target `ENSI_ECE_COMBO_ZZXX` (`0xeb`), then the four cancels.
- **Dash X-X `32999ce0`**: target selection (`+0x9c24`, else Eve) for the 0.01 and 0.1 blocks, the **y +200 then +290** on the particle (the listing reads the slot it just raised; decide whether it is a `JUNE_BUGS.md` row), y +350 on `m_vSummonedUnitBonePos`, `SetLandPosition` from Eve, no class switch; End `329687c0` clears `+0x9c24`.
- **Alpha tags (7c)**: five sites `3299968c` `329998cb` `32999dda` `32999ed4` `3299a119`.
- **JUNE_BUGS**: none was written; say so if you find one.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1i commit>^..<CH1i commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1i - Review" into the plan doc under the Done note, update the CH1i row in `STATUS.md`, check that `kickoffs/char-eve/CH1j.md` still matches, and commit with explicit pathspecs.
