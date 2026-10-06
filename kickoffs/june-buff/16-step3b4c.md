# June buff port - Phase 16 step 3b, part 4c - stage June's socket-effect effect sets (data)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b4c` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b4 review done. Data only; check STATUS for june-effect sessions staging the same EffectSet / DamageEffect files |
| Flag | none expected (data); a code change goes under `SERV_IRUHADEV_JUNE_SOCKET_PARITY` |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | write `16-step3b4c-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-step3b4 -- Done"
```

## Task

User decision 2026-10-06 ("follow June"): stage the June data the SIEE 12-20 sockets play, so the ported cases work
on March's data. The buff factors they apply (20 BFIs missing from the 361 loaded slots, e.g. the aura's
`BFI_BUFF_HERO_DUNGEON_INCREASE_PHYSIC_ATT`, `BFI_BUFF_SOLESS_TITLE_INVINCIBLE`, `BFI_BUFF_CELESTIAL_BLITZ`) are
**Phase 13's** (it adopts all of June's buff data); list them for Phase 13, do not stage buff files here.

1. **Census.** Every `EFFECT_NAME` of June's SIEE 12-20 sockets in `june_src_fixed/data036_kom/OptionItemData.src.lua`
   (32 effect-set names in 16-step3b4's count, plus `EffectSet_Hero_Dungeon_Aura_Buff`). For each: the June EffectSet
   entry (`june_src_fixed/.../EffectSet.src.lua`), every damage effect, particle, mesh and sound it names, recursively
   (`june2017_re/assets.py` / `asset_closure.tsv`), and whether March's live koms already have it.
2. **Stage** the missing entries the way earlier June data steps did: June asset wins over March at the same name, June
   Lua from `june_src_fixed/` only, compile with `elstools compile-batch -v mar-2014`, stage in `march2014/dataNNN/` and
   `kom_contents/dataNNN_kom/` with the manifest, pack with `/elstools pack`, ask before `/x2-kom-packed copy`.
3. **Check** with `elstools inspect` on the live koms and a trial load: no Lua abort, the census names all resolve.

Hand-off: a dungeon run where the sets can be seen needs a socket on the player's gear. The user answered "follow June" to
the open save-edit question on 2026-10-06; before editing `els_db.sql`, still ask once, name the item and the socket IDs,
and back it up to `db_backup/` (3b3's precedent).
