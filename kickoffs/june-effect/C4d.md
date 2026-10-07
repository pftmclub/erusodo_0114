# June effect completion - C4d - Hero-dungeon NPC anim speed (`Add_HeroDungeon_ExtraStatForException`), dormant

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C4d` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C4c (it adds the NPC member this phase writes: June `+0x8fdc`, the `MULTIPLICATION_ANIM_SPEED` factor; C4c named it `CX2GUNPC::m_fMultiplicationAnimSpeed`, under `SERV_IRUHADEV_JUNE_NPC_ANIM_SPEED`, with no setter: add one here) |
| Flag | `SERV_IRUHADEV_JUNE_HERO_NPC_ANIM_SPEED` (new, in `KTDXLIB/Always.h`) |
| Log tag | `[JUNE_FX]` in ErrorLog.txt (`python tools/logs.py --tag JUNE_FX`) |
| Review | none unless the port grows past the three pieces below (the C4c review can take a look) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C4b - Done (2026-10-07, Opus 5.5 high; identification only, no code)"
grep -n "Effect C4b" june2017_re/funcmap.tsv
grep -n "^327defc0\|^32931be0\|^32373680" june2017_re/funcmap.tsv
```

## Why this phase exists

C4b found that June's `CX2DungeonSubStage::CreateGUNPC` (`327ef570`) gained a tail at `@327ef852-327ef8af` that sets each new NPC's anim-speed factor from a Hero-dungeon table. C4c ports the factor and its multiply in `ResetAnimSpeed`. This phase ports what fills it: June changed a March function, and no partial updates are allowed.

The phase is **dormant in March**. March has no hero dungeons, `HeroDungeonData_Client.lua` is not loaded, and the table stays empty. That is fine: follow June, and make the empty case log once.

## Task

Under `SERV_IRUHADEV_JUNE_HERO_NPC_ANIM_SPEED`, port June's three pieces:

1. **Storage.** `CX2DungeonManager` (June `g_pData +0x44`) holds a vector at `+0x5c`/`+0x60`. Each entry is 12 bytes: `{ float rate, int dungeonID, int npcID }`. Name the struct and members after the Lua keys `MULTIPLICATION_ANIM_SPEED`, `DUNGEON_ID` and `NPC_ID`.
2. **Lua bind `Add_HeroDungeon_ExtraStatForException`.**
   - Find its June body first. The `LANOX_EXTRA_NPC_STAT_PLAN.md` table names the use `31dc3faa` in `31d95080` and the `FUN_32373680` path; it sits near `Add_HeroDungeon_ExtraMonsterGradeStat` (`327defc0`).
   - Transcribe how each row is read: the table walk, defaults, and dedupe or append.
   - Bind it on `g_pDungeonManager` the way March binds that object's other functions.
   - Check whether `HeroDungeonData_Client.lua` would load in March at all: it uses `DUNGEON_ID.DI_*_HERO_*` and NPC IDs March's enums lack. If it would not, do **not** stage it; the table simply stays empty (log once).
3. **`CreateGUNPC` tail** (`@327ef852-327ef8af`).
   - Gate: `IsHeroDungeon( current dungeon ID )`, June `32931be0`: true for 100000..100532, 110001..110310, 120000 and 120010. March already has `JuneIsHeroDungeon` (static, in `X2PartyUI.cpp`). Expose it or share it rather than writing a second copy.
   - Then walk the vector. On a dungeon and NPC templet ID match, set the NPC's factor to `rate`.
   - The walk does **not** break, so the **last match wins**: a JUNE_BUGS row.

Cite June addresses at each site and record the funcmap rows (`Effect C4d`). This phase stages no data. The play-test is a regression run: a normal dungeon shows no change, and the log has the one "table empty / not loaded" line.
