# June effect completion - C11a review - the full June effect-Lua re-adoption

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C11a review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C11a done (its Done note and commit), and the user's C11a play-test read |
| Flag | none (data phase) |
| Log tag | `[JUNE_FX]` (`python tools/logs.py --grep "JUNE_FX <subtag>"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C11a - Done (2026-10-16)"
python tools/docsection.py MODS.md "June effect Lua re-adopted in full (june-effect C11a, 2026-10-16)"
python tools/luasrc.py core --check ; python tools/luasrc.py ledger --check
```
Also: `june2017_re/c11a_march_only_templets.tsv`, `june2017_re/stage_plan.tsv` (the applied plan), checklist section 7u, `JUNE_BUGS.md` row 200.

## Task

Review C11a against the user's decision (full June re-adoption, route b, 2026-10-16). Check that:
1. Every staged effect `.lua` in `kom_contents/data036_kom` and `datai015_kom` is byte-identical to `june2017_re/assets_cache/june_b/<name>`
   (sha1), starts `1B 4B 4C 81`, and has no `X.src.lua` sidecar beside it (the three moved ones are in the backups the Done note names).
2. The packed `data036.kom` / `datai015.kom` in the game dir hold those bytes: `python tools/kom.py status data036 datai015` is in sync.
3. The 87-name table is right: spot-check 5 names with an identifier-bounded search of June's four `DamageEffect*.src.lua` and `EffectSet.src.lua`
   (absent) and of the merged backup (present); spot-check 3 "march-consumer" call sites for the NULL check the Done note claims.
4. The MODS revert steps would work: the backup dir holds the 93 replaced files and 53 manifests.
5. The play-test lines (C11a hand-off) are read and recorded; rows not reached stay "not reached", not failures.
Hand design decisions back to the user. Write "C11a - Review" into the plan doc, update the C11a and C11 STATUS rows, and commit
with explicit pathspecs. C11 closes when this review closes.
