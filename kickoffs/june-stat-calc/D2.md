# June damage and stat calculation - Phase D2 - GetFinalDamage: the rest of June's body

| | |
|---|---|
| Start | new conversation on **Opus 5.5, xhigh** effort, then type `/x2-phase june-stat-calc D2` |
| Plan doc | `JUNE_STAT_CALC_PHASES.md` (branch `main`) |
| Depends on | D0 review, D1 review; not beside another phase that edits `X2GameUnit.cpp` |
| Flag | `SERV_IRUHADEV_JUNE_STATCALC_FINAL_DAMAGE` (+ `_STATCALC_DEBUG`) |
| Log tag | `[JUNE_STATCALC]` (`python tools/logs.py --tag JUNE_STATCALC`) |
| Review | `kickoffs/june-stat-calc/D2-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_STAT_CALC_PHASES.md "Rules" "Starting context" "Design: the map and the verification" "Phase D2 - GetFinalDamage: the rest of June's body" "Phase D0 - Done" "Phase D1 - Done" "Play-test hand-off"
```

Then `june2017_re/statcalc/gfd_blocks.tsv` (every row not `June`) and the map's rows for the helpers.

## Task

Work down `gfd_blocks.tsv`: port every range that is `March`, `mixed` or `absent`, so `GetFinalDamage` runs June's body from entry
to return in June's order, as the phase section says. March's code in `#else`; blocks already `June` are re-read against the exe and
left under their own flags; a block of an unported class is guarded (`stand-in: <class> not ported`).

- The variance roll `322788c0` and its seed source are ported only if D0 found its range can be non-zero on live data; otherwise the
  ledger row says why it stays out and who needs it.
- The damage-result hook `3229a530`: the base body and the call site @32284d98. Class blocks inside it go to their plans.
- `GetEquippingItemByEquipPos` `32869e10`'s third argument: replace March's definition (divergent row) and fix its callers as June's.
- `GFD` debug lines: each multiplier of the main path by name, one line per logged hit.
- One emulation script per pure helper ported, under `june2017_re/statcalc/`.

Gate: no ledger row left that is not `June` or `class: <plan>`; the play-test of the phase section (three characters; normal, special
active and level-damage hits; normal, secret and boss monsters) with each `GFD` product equal to the number on screen.

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
