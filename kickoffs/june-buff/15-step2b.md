# June buff port - Phase 15 step 2b - enchant ticks, piercing term, owner's enchant, June data

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 15-step2b` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 15 step 2 chunks A-C (`835b5ef`, `5d8f827`, and the chunk C/B2 commit named in the step 2 Done note) |
| Flag | `SERV_IRUHADEV_JUNE_ENCHANT_PARITY` |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | `kickoffs/june-buff/15-review.md` (write it when 2b is done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 15, step 1 — RE (2026-10-04), read-only; step 2 not started" "Phase 15, step 2 — chunks A-C (2026-10-07)"
```

The step 1 subsection's "Readers of the new fields" table and "Getter surface" are the input;
the step 2 subsection says what already landed and what moved (state 23, bit 7, the tooltip).

## Task

Finish Phase 15 (chunk D), under `SERV_IRUHADEV_JUNE_ENCHANT_PARITY`, June's whole code at each
host, insert-only, March in `#else`:

1. **The user and NPC ticks.** `CX2GUUser::CommonFrameMove` `32063650` and the NPC hosts
   `327b53b0` / `3276b400` against March's enchant ticks (`X2GUUser.cpp` around 16589-17360, and
   the `CX2GUNPC` equivalents). Decode before porting:
   - blaze MP drain (`@320691de`, NPC `@3276b95d`): `m_fBlazeMpDrain x (1 - resist/500)`;
   - poison / frozen slows (`@32067e9e`, `@3206a3b0`; NPC `@327b901f`, `@327bab4f`): BFI 1001 / 1014
     from the speed lists, re-applied only when absent or `JuneIsEnchantAccumulationRose`
     (already ported, `X2GameUnit.h`). The BFIs are not in March data until Phase 13 (user,
     2026-10-07): June null-checks the factor, so the code lands dormant and logs once;
   - shock debuff BFI 1000 (`@32068bc0`, `@327ba136`): trace the trigger condition and the `x`;
   - every tick reader of `m_Accumulation`: the bodies are now 1-based (1..3), March's
     thresholds (`>= 2`, `>= 1` visual block at 16589-16660, the Slow-percent tiers) are 0-based.
     Port June's thresholds; say whether June still reads the `Slow` percents;
   - the attacker's table comes from the element's `m_optrGameUnit` (stored by chunk C) by its
     class, and its EDT from `32708080` on the attacker's slot 111.
2. **`GetFinalDamage` piercing term** `32284aa0` `@322865ea` / `@322866c3`
   (`m_fPiercingDamageFactor`, the `ENCHANT_STAT_SCALE` gate) and the frozen block `@322855ff`.
   Also identify March's `X2GameUnit.cpp` code near lines 1600-1810 that carries a Henir check
   like the bodies (which function, and does June change it).
3. **`USE_OWNERS_ENCHANT`**: NPC slots 111/112 (`3275aa20`, `3275aa80`) and the key read in
   `32289090` `@32289d44` (only `data037_kom/Lire_Evoke_trap(_Memo)` set it). March's
   `GetWeaponEnchantData` is non-virtual (`X2GameUnit.h`), so say how the NPC override is hosted.
4. **June data.** Stage June's `AttribEnchantItem.lua` (`june_src_fixed/data036_kom/`) in both
   staging dirs (compile with `tools/kom.py compile data036`), add the JUNE_BUGS row for its
   `EDT_ENCHANT_DOUBLE_MASTER_SHOCK_OLD` entry (absent from June's own Enum, so it loads keyed
   EDT_NONE: latent), pack data036, **ask before the copy**, then hand over one play-test of the
   whole phase (blaze tick damage and MP drain, piercing extra damage, snatch "HP -" text, the
   new tooltip, the attribute ED cost from `AttribEnchantRequireED.lua`).
5. **Decided already (user, 2026-10-07): follow June** - armour may take a 3rd attribute; nothing to code.
