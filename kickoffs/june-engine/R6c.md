# June engine remainder - Phase R6c - The equip's enchant-particle list (SetEnchantParticleShow)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort (map, then port), then type `/x2-phase june-engine R6c` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | R6b (same file, `X2Eqip.*`); routed from june-march-divergence D1-3e (tsv row 263) |
| Flag | `SERV_IRUHADEV_JUNE_EQUIP_ENCHANT_PARTICLE` |
| Log tag | `[JUNE_ENGINE]` (add a sub-tag only if a check needs a log line) |
| Review | `kickoffs/june-engine/R6c-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "D1-3e Done (2026-10-29, Opus 5.5; one site under D1-3c's \`WEAPON_EFFECT_SHOW\`; viewer family -> D1-3g, 263 -> june-engine R6c)"
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context"
```

and row 263 of `tools/data/june_march_divergent.tsv` (`sed -n '263p'`).

## Task

March keeps each weapon's attribute-enchant particles as three fixed handles on `CX2GameUnit::Weapon` (`m_ahEnchantWeapon[0..2]`), shown and
hidden by `Weapon::SetEnchantParticleShow` (`X2GameUnit.cpp:10686`; callers `X2GameUnit.cpp:9651` / `:11499`, `X2GUUser.cpp:2266` / `:21033`,
`X2GuEve.cpp:6911`). June keeps them in a list of entries on the owning equip object (`+0x8f0`): `3295cd30` is `ADD ECX,0x8f0; JMP 3295bf90`, and
`3295bf90( bShow, index, -1 = all )` sets each entry's show byte `+0x34`, finds its sequence (`+0x2c`) in the major or minor particle system by the
type byte `+0x1c`, and flips the sequence's show flag `+0x172` (calling vslot `0x28` when the visible state changes, with `31fae370`).

1. **Map** the list: its owner class (the `3295xxxx` range is `CX2Eqip`; confirm from the ctor), the entry struct, its writers (`3295d960`,
   `3295ddf0`, `32961c80`) and their callers, and which March functions create / destroy / move the three weapon handles today.
2. **Port** the container and every reader and writer whole under the flag (no partial updates), March's in `#else`, so
   `Weapon::SetEnchantParticleShow`'s callers (June `329c5bc0` CX2GUEve::CommonFrameMove, `3200a590` the CX2GUUser invisibility setter,
   `329afaa0`) reach June's list. If the map shows the list is the whole attribute-enchant create path, port that path in this phase or split.
3. Close tsv row 263 (`june_march_divergent.tsv`) with this phase's commit; funcmap rows `(R6c)`; `MODS.md`; `JUNE_BUGS.md` rows for June quirks.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md). No `June` prefix (user, 2026-10-28).
