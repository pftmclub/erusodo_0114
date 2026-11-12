# June damage and stat calculation - Phase D2b - GetFinalDamage from the defence term to the return

| | |
|---|---|
| Start | new conversation on **Opus 5.5, xhigh** effort, then type `/x2-phase june-stat-calc D2b` |
| Plan doc | `JUNE_STAT_CALC_PHASES.md` (branch `main`) |
| Depends on | D2a; not beside another phase that edits `X2GameUnit.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_STATCALC_FINAL_DAMAGE` (D2a's) (+ `_STATCALC_DEBUG`) |
| Log tag | `[JUNE_STATCALC]` (`python tools/logs.py --tag JUNE_STATCALC`) |
| Review | `kickoffs/june-stat-calc/D2-review.md` (D2b writes it; it covers D2a and D2b) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_STAT_CALC_PHASES.md "Rules" "Starting context" "Design: the map and the verification" "Phase D2 - GetFinalDamage: the rest of June's body" "Phase D0 - Done" "Phase D1 - Done" "Phase D2a - Done" "Play-test hand-off"
```

Then `june2017_re/statcalc/gfd_blocks.tsv` rows from `32285ccf` to `32286c48` that are not `June`, the map rows with `phase` D2b, and
funcmap rows tagged `StatCalc S33`. Read June from `disasm_range.py` (Ghidra's listing skips `32286810-3228681a`).

## Task

Port every `gfd_blocks.tsv` range from `32285ccf` on that is `mixed` or `absent`, in June's order, March's code in `#else`:

- `32285dc4`: the level-damage path (DT_PHYSIC fixed part = `damage.fPhysic` x the attack sum; June's PvP mode list).
- `32286126`: the PvP rate behind June's `IsPvpOrPve` idiom (`GetIsNowPVPCorrectionGame()`).
- `322862f6`: the state defence's vslot `0x2ac` factor: identify it (vtable slot 171 base and overrides) before writing.
- `32286416`: the NPC `+0x8fd4` multiplier: identify the member and its writer.
- `32286482`: the custom-event boss route (dungeons 41480-41510, named from `DungeonEnum`).
- `322867e0`: the atomic shield's June formula, with its writer (`SetShieldDamageReduce` callers, Chung): rule 3.
- `3228696a`: the HP-rate gate `322726f0` (Rena's `RNSI_SA_ALP_SUPERSONIC` exception; checklist 7c).
- `322869b7`: the dev defence-ignore overlay.
- `GetEquippingItemByEquipPos` `32869e10`'s third argument: replace March's definition (divergent row) and fix its callers as June's
  (`GetFinalDamage`'s piercing term, `CalcExtraAttackPower`, the rest found by grep).
- Re-read `GetIgnoreDefenceRate` `32274970` (map row `unknown`).
- D1's play-test lead (D1 review): 17 of 154 `HIT` lines show `dmg=0` at a normal `rate=` (0.45-0.65), on every monster kind and on two
  incoming hits (530 / 532 -> the player). The `GFD` lines must show which multiplier gives the 0; if no range of D2a / D2b does, say so.

`GFD` debug lines for each multiplier of this half; one emulation script per pure helper ported. Full build and the phase section's
play-test, which covers D2a's ranges too. Write `D2-review.md` from `kickoffs/TEMPLATE-review.md` (it reviews D2a and D2b).

Gate: no ledger row left that is not `June` or `class: <plan>`; flagcheck and junediverge PASS over D2a's and D2b's commits; `DEPLOY
OK`; the play-test of the phase section (three characters; normal, special active and level-damage hits; normal, secret and boss
monsters) with each `GFD` product equal to the number on screen.

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
