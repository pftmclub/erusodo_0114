# June character Eve (Stage 7) - Phase CH2b4 - A dying summon drops its owner's summon buff

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase char-eve CH2b4` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2b2 review; not beside another `X2GUNPC.*` phase |
| Flag | `SERV_IRUHADEV_JUNE_SUMMON_UNIQUE` (same logical change as CH2b2: the summon's lifecycle against its owner's summon buff) |
| Log tag | `[JUNE_SUMMON]` |
| Review | `kickoffs/char-eve/CH2b4-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2b2 - Done (2026-10-27, Opus 5.5)" "Phase CH2b4 - A dying summon drops its owner's summon buff (added 2026-10-27 by CH2b2, Sonnet)"
```

Rows: `june2017_re/funcmap.tsv` rows `327b53b0 @327bcdfd (CH2b2)` and `32778b10`.

## Task

Port June's `CX2GUNPC::CommonFrameMove` `327b53b0` block `@327bcdfd..327bcfe0` whole, from the raw listing (`python june2017_re/charport_adis.py eve 327b53b0`, then read that range):
- Where it sits: inside the DYING_END block (`@327bca9e` flag 0x800000), after the self-resurrection test, on the path where the animation has **not** ended
  (`@327bcac9..327bcad6`). March's counterpart is the `NOWSTATE_FLAG( DYING_END )` block of `CX2GUNPC::CommonFrameMove` (`X2GUNPC.cpp`, the Ferdinand
  `ResetLinkOverChargeIllusion` lines sit in the animation-end branch there; compare and say which June keeps).
- Owner `+0x8d5c` (the owner observer), `+0x8b40` = `m_bSelfDestruction` must be false. Ferdinand 0x5ae / Apocalypse 0x644 -> `31fc8a90( owner's unit class )`
  (identify it; read 32997c10 / `June_ResetLinkOverChargeIllusion` for comparison) and set `m_bSelfDestruction`. Other ids: `CX2GUUser::JuneGetSummonBuffFactorID`
  (CH2b2) -> `GetBuffFactorPtr` -> unless `32778b10` (another live ally NPC of my id and my owner; port it as a `CX2GUNPC` member) erase the owner's buff templet
  (`3229e160`, March's `EraseBuffTempletFromGameUnit`) and set `m_bSelfDestruction`.
- Names, not numbers (`NUI_SI_HA_FERDINAND`, `NUI_SI_HA_APOCALYPSE`); one `[JUNE_SUMMON]` log line when the buff is erased.

Play-test hand-off: Eve (unit 19 `Iivue`, slot 0) casts Surface Cuting once and lets the Oberon die (enemy hits, or wait out its life): the Oberon buff icon goes with it
and a `[JUNE_SUMMON]` erase line appears; cast twice quickly: the replaced Oberon's death keeps the buff (the new one is alive).

CP949 sources: `bytepatch.py` only. No hardcoded enum literals (`enumscan --check`). Done gate: the block ported whole, funcmap rows updated to PORTED, flagcheck PASS,
`DEPLOY OK`, play-test hand-off.
