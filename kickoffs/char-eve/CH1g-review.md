# June character Eve (Stage 7) - Phase CH1g review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1g review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1g done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1g - Done"
awk -F'	' '$6=="CH1g"' june2017_re/char_eve/ch0_class_map.tsv      # 23 rows
grep -a "Char Eve CH1g" june2017_re/funcmap.tsv                      # 23 PORTED rows
```

The 23 members are in the seventh `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (it starts with the heading comment "Eve's Code: Nemesis states"); declarations at the end of the class in `X2Lib/X2GuEve.h`. New enumerators `ENSI_ECN_COMBO_ZZX_LOOP` / `_END` (June ids 232 / 233); no new fields or helpers.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1g --out <scratchpad dir>` (23 files; full wide strings need a read of the push address, the listing cuts them at 12 characters) and a Ghidra decompile where a function object exists. You decide:

- **Is "= March" right?** Ten of the 23 are March's body verbatim: the two Inits and Start of dash Z-X / Z-X-X, the five FrameMoves (spear effects at 0.27 / 0.1 / 0.001 / six hits at 0.26 0.40 0.53 0.66 0.8 0.93 / 0.36, positions, Queen's Throne power rate) and `329709a0` / `32970b50`. Spot-check the effect names, timers, `y -50`, `x -/+ dir.x * 70`, `x -/+ 40`, speed 900 / 0, 0.46 / 0.56, and that none of the five CreateInstance calls carries more than the default tail (a masked block diff of the six XXZZ8 calls is in the Done note).
- **The cancel chain** added to XXZ (`32970c30`), XXZZ8 (`32970f10`), XXZZ8Z (`32971240`, with `SKILL_CANCEL_AFTER( 0.46 )` first), dash-jump X-Z (`32975bc0`) and ZZX end (`32975eb0`): `+0x8c90..+0x8c9c` = skill / BWALK / WALK / DASH cancel-after, on-line tests, the CH1d macros. Check the position of the animation-end test (before the chain in XXZ / ZZX end, after it in XXZZ8Z, nested `if( IsAnimationEnd() ) { if( on line ) landing }` in dash-jump X-Z) and that `+0x8e90` swap is the 7o second-body item (not ported, as CH1d-CH1f).
- **June-only states**: ids 230 (dash-jump X-Z), 231 / 232 / 233 (Z-Z-X ready / loop / end): the transitions (`0xe8`, `0xe9`, `pureX` release), the Init preload lists (`329685d0`, `32968650`), and that `FrameMove` is `CommonFrameMove` in all four.
- **Enumerators**: `LOOP` / `END` stay as new names; CH1m maps March values to June ids.
- **JUNE_BUGS**: none was written; say so if you find one.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1g commit>^..<CH1g commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1g -- Review" into the plan doc under the Done note, update the CH1g row in `STATUS.md`, check that `kickoffs/char-eve/CH1h.md` still matches, and commit with explicit pathspecs.
