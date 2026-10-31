# June character Chung (Stage 7) - Phase CH1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1b review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1b done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE`, `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (commented out until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1b - Done (2026-10-31)"
awk -F'\t' '$7=="CH1b"' june2017_re/char_chung/ch0_class_map.tsv
```

## Task

Review the 27 CH1b functions against June (`python june2017_re/charport_adis.py chung --phase CH1b --out <scratch dir>`; the listing, not the decompile, carries the `StateChange` arguments):
- The real differences from March: `EraseTargetingNPC` `32c04140` (new, `_CORE`: the SetNowHp( 0 ) kill, the ignored 3.0 argument, ids 863 / 1419 / 2525; the new enum name `NUI_TARGETING_CHUNG_HEAVY_FIELD` = 2525 in `X2UnitManager.h`) and its three calls in the two Die Start functions; Damage Small / Big without the memo 12 and PvP tests; StandUpCannonAttackEventProcess's walk / dash cancels with `IsOnSomethingFuture()` and the hyper-awakening branch.
- Spot-check 3 "= March" functions end to end (suggest `32bedb80` flag bytes `+0x7d86..0x7d89`, `32bf8c50` effect-set order, `32c033b0`).
- Whether the bounding states (`CSI_DAMAGE_BOUNDING_*`, checklist 7l) really have no Chung-owned function (the three callbacks are thunks to `CX2GUUser` virtuals `31fd0230` / `31fd0260` / `31fd02d0`: CH1m registers, CH1n owns the bodies).
- Whether `EraseTargetingNPC` should also replace March's `SetSelfDestructSummonedNPC` calls outside CH1b (March lines for `32c08010` / `32c0c4e0` / `32c0ca60`'s callers): they belong to the phases that own those June functions.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE --range <CH1b commit>^..<CH1b commit>`; `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `/D SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (`june2017_re/census.py` `cl_cmd`, see the CH1a review). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1b - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1c.md`, commit with explicit pathspecs.
