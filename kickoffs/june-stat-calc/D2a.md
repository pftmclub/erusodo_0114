# June damage and stat calculation - Phase D2a - GetFinalDamage up to the damage-type switch

| | |
|---|---|
| Start | new conversation on **Opus 5.5, xhigh** effort, then type `/x2-phase june-stat-calc D2a` |
| Plan doc | `JUNE_STAT_CALC_PHASES.md` (branch `main`) |
| Depends on | D0 review, D1 review; not beside another phase that edits `X2GameUnit.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_STATCALC_FINAL_DAMAGE` (+ `_STATCALC_DEBUG`); D2b continues under the same flag |
| Log tag | `[JUNE_STATCALC]` (`python tools/logs.py --tag JUNE_STATCALC`) |
| Review | none of its own: `D2-review.md` (written by D2b) covers D2a and D2b |

D0 split D2 by address range (2026-11-12): 17 `mixed` / `absent` ledger rows plus three helpers were more than one phase carries.

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_STAT_CALC_PHASES.md "Rules" "Starting context" "Design: the map and the verification" "Phase D2 - GetFinalDamage: the rest of June's body" "Phase D0 - Done" "Phase D1 - Done"
```

Then `june2017_re/statcalc/gfd_blocks.tsv` rows from `32284aa0` to `32285ccf` that are not `June`, the map rows with `phase` D2a, and
funcmap rows tagged `StatCalc S33` (`grep -n "StatCalc S33" june2017_re/funcmap.tsv`). Read June from `disasm_range.py`, not Ghidra's
listing: Ghidra's skips the switch bodies `322851b1-322852d7`.

## Task

Port every `gfd_blocks.tsv` range before `32285ccf` that is `mixed` or `absent`, in June's order, March's code in `#else`:

- `32284d76`: the damage-result hook `3229a530` as a new `CX2GameUnit` virtual at slot `0x6c` and its call. Its Raven, Rena and Ara
  blocks port with it (all three classes are ported; D0 corrected planning's "Eve").
- `32284db2-32285055`: the class passives on June's ids (24403 at 0.3, 24408, 22410 without the 0.9 HP gate, with vslot `0x524` /
  `0x784`; identify `0x784` first). March's blocks key March ids (4412 / 2414 / 3411) that June's skill data no longer uses.
- `32285055`: the Sharpen Arrow writer (23411 ability `0x11`); its reader is D1's.
- `3228509f`: the world-mission rate epsilon (June `5e-8`, March's `CalculateIncreasingRate` `5e-5`): a local helper, never an edit of
  `KTDXCommonFunc.h`.
- `32285189`: the socket stat rates in states 9 / 11 / 18 / 21 / 23 (June ids; March names `XS_PVP_GAME`, `XS_DUNGEON_GAME`,
  `XS_TRAINING_GAME`, `XS_BATTLE_FIELD`, and register June's `XS_PVE_GAME` 23 if March lacks it, flag `SERV_IRUHADEV_JUNE_ENUM_NAMES`).
  Note March's own state values differ (`XS_TRAINING_GAME` 17, `XS_BATTLE_FIELD` 24): compare by name.
- `322855a6`: `CalcExtraAttackPower` as June's `3226e530` (no `EDT_EVENT_SMASH_3` case; the discarded roll per `+0x754` record).
- `322857fb`: drop March's 4-arg `ModifyFinalDamageByGameUnitType` call on `AT_NORMAL` once Eve's 4-arg override (`X2GuEve.cpp:43123`)
  is shown to be covered by her ported 3-arg slot.

`GFD` debug lines for each multiplier of this half. One emulation script per pure helper ported, under `june2017_re/statcalc/`.
Probe build only (0 errors): the play-test is D2b's and covers both halves. Done note "full build skipped: no play-test".

Gate: no ledger row before `32285ccf` left that is not `June` or `class: <plan>`; flagcheck and junediverge PASS.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
