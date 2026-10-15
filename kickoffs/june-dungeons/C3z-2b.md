# June dungeons - C3z-2b - drop / EXP / ED rows for the 142 June-only NPCs C3z-2 added

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons C3z-2b` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` (branch: the current working branch); C3z-2's Done note still sits in `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3z-2 (its 142 `NPC_UNIT_ID` values and `NPCTemplet.lua` rows) |
| Flag | none expected (data / server-Lua work); name one `SERV_IRUHADEV_JUNE_*` only if loader code changes |
| Log tag | `JUNE_FX C3z-2b` |
| Review | write `kickoffs/june-dungeons/C3z-2b-review.md` when done |

## Why this exists

C3z-2 added 142 June-only NPCs to `Enum.lua` / `NPCTemplet.lua`. They spawn and fight, but give 0 EXP / ED / drops: the
rows that pay out (drop tables, per-level stat and EXP tables) were server data, and June has no server here. The user
decided 2026-10-16 that this belongs to june-dungeons, not to the C3z-2 review.

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-2 - Done"
```

Also read the CLAUDE.md rules *server-side Lua the client needs is the user's to pack* and *design missing June pieces*
(memory: check what June does where the value is absent; keep June's default; else propose a labelled stand-in first).

## Task

1. **Identify (read-only).** For each of the 142 ids, list which payout tables March's offline loaders read
   (`X2Lib/Offline/X2OfflineDropTable.cpp`, `X2OfflineStatTable.cpp`) and whether a row exists. Group the misses by
   dungeon and by table.
2. **Source check.** Look for a real row in the repo (`KncWX2Server/ServerResource/US/`, June extracts). A row that can
   be read out of the repo and cited is the only permitted fallback; never invent a curve or hardcode a table.
3. **Decide per group.** Real rows exist -> port them (server-Lua files are named for the user to XOR-encrypt and pack,
   never packed here). No rows anywhere -> report the group as "0 payout, no source" and stop; a stand-in needs the
   user's yes first.
4. Finish per x2-phase (Done note, STATUS row, commit); name any server-Lua file the user must pack.
