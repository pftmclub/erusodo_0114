# June dungeons - phases

Created 2026-10-11 (user): the dungeon / world / map-entry ports that `june-effect` (C3y, C3z) collected are not effect-engine work,
so they run as their own plan. Engine fixes the dungeons need (NPC/game queries, intro skip, SimpleDLG) stay in
`JUNE2017_EFFECT_COMPLETION_PHASES.md`.

## Context

- Goal: every June dungeon and world that March lacks or has half of is playable offline, with June's rows, worlds, NPCs, meshes,
  map buttons and entry rules. Offline client only (shared-header changes flag-gated, server desync not a concern).
- **Phase IDs were kept** (C3y-1b ... C3z-5b, C3z-4t) so commit messages, JUNE_BUGS rows, census files and `funcmap.tsv` tags still match.
  New phases of this plan (for example the ones C3y-5 writes) take the next free ID in the same style, for example `C3y-5a`.
- Done phases that stay in june-effect as history: C3x (world load, 170 map files), C3y (census), C3y-1a (Hamel secret),
  C3z-1 (map assets), C3z-2 (old dungeon NPCs), C3z-3 (registry parity), C3z-5 (DungeonData keys), C3z-6 (relative level, poison).
- Commit message prefix: `June dungeons Phase <id>: <what>`. Log tag stays `JUNE_FX <id>` (existing code prints it).
- Where the text lives: every moved phase's task text and every earlier Done note is still in `JUNE2017_EFFECT_COMPLETION_PHASES.md`
  (section names below). **New Done notes go into this file**, under `## <id> - Done`, so the old doc stops growing.

## Rules

- All of CLAUDE.md and the standing memory rules: one `SERV_IRUHADEV_JUNE_*` flag per logical change in `KTDXLIB/Always.h`, original
  code in `#else`; June bugs kept verbatim with a `JUNE_BUGS.md` row; no new `.lua` files; June `.lua` arrives through
  `luasrc.py adopt-june`, core-list rows added by the phase that edits them; a missing June asset or key is a finding, never a hand patch.
- **Opus phases delegate the mundane work to subagents** (x2-phase skill, "When the phase is Opus").
- The port halves of C3y-1b / 1c / 2 / 3 / 4 share the core registries: one at a time.
- W3 packs `data040`, which those port halves also pack (`WorldData.lua`): one at a time with them.

## Starting context

- Tools, not hand commands: `python tools/build.py --deploy`, `flagcheck.py`, `logs.py --grep "JUNE_FX <id>"`, `kom.py`, `luasrc.py`.
- Censuses and gates: `june2017_re/c3y_unreached_dungeon_census.tsv` (+ `_world_` and `c3y_june_only_dungeon_ids.tsv`),
  `dungeon_npc_parity.tsv`, `dungeon_map_parity.py`, `dungeon_registry_census.py`, `c3z5_key_readers.tsv`.
- Method for a dungeon: DungeonEnum / DungeonData rows (data036 / data035, core) -> worlds (Enum `WORLD_ID`, data040 WorldData, files)
  -> NPC ids (`Enum.lua` + data037 `NPCTemplet.lua`, stat tables) -> meshes (`npc_mesh_census.py`) -> map button (June's icon file
  by `adopt-june`) -> emulator check. C3z-2's and C3y-1a's Done notes show it done.

## Phases

| Phase | Scope | Text lives in (JUNE2017_EFFECT_COMPLETION_PHASES.md) |
|---|---|---|
| C3y-1b | Henir Space's 5 missing boss worlds + the other Sander secret worlds (13 worlds); play-test + review owed | "C3y-1b - port: ..." and "C3y-1b - Done" |
| C3y-1c | the Sander secret dungeon (41141, 32 NPCs, map button, BEHEMOTHSOUL effect data) | "C3y-1c - port: the Sander secret dungeon" |
| C3y-2 | June's class tutorial dungeons (9 classes; identify the entry route first) | "C3y-2 - identify, then port: ..." |
| C3y-3 | Bestera 1-4 + `DI_BESTERA_START` | "C3y-3 - identify, then port: Bestera 1-4" |
| C3y-4 | Space of Thought 1-3 + `_DUMMY`, the Special-map line | "C3y-4 - port: Space of Thought 1-3" |
| C3y-5 | identify June's route into the Hero tier (read-only), then write the port phases | "C3y-5 - identify: ..." |
| C3z-1b | June's data101 map assets (Velder / Henir); waits for june-asset-port P0 | kickoff `C3z-1b.md`; "C3z-1 - Done" |
| C3z-4t | June's `CX2TutorialGuideManager` (the 9 Battle Field tutorial dungeons via `benders_actor.lua`); scope first: report the system's size before porting | kickoff `C3z-4t.md`; "C3z-4 - Done (batch 1)" |
| C3z-5b | the DungeonData readers C3z-5 routed (DCC bits, GetLastClearDungeonID, m_fExtraValue, June-only-row keys) | kickoff `C3z-5b.md`; "C3z-5 - Done" |
| C3z-8 | June's boss-raid systems (`CX2BossRaidLongTimeEvent` enter / state UI, `CX2NewBossRaidManager` + raid phases, June's `CX2BossRaidManager`); scope first, report size and offline reach, then write port phases. Routed from skill-system-core S7o (user 2026-10-19) | kickoff `C3z-8.md`; `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7o - Done" |
| W3 | June's village world Lua: census every village world script + line map in data040, stage the closure, adopt June's (ljconv), pack, ask, copy. Moved from june-engine by its scope audit (2026-10-20), same ID; Sonnet 5.5 high | `JUNE_ENGINE_REMAINDER_PHASES.md` "Phase W3 - June village world Lua"; kickoff `W3.md` |

Run with: Opus 5.5 high (C3y-5: xhigh). Kickoffs: `kickoffs/june-dungeons/<id>.md`. Board: `STATUS.md`, block `june-dungeons`.

## Order (recommendation, 2026-10-11)

1. C3y-1b play-test (user) and review; 2. C3y-1c; 3. C3y-2 / 3 / 4 one at a time (identify halves can run earlier);
4. C3z-5b best before or with them; C3z-4t (scope first) pairs with C3y-2, the class tutorials; 5. C3y-5 any time (read-only); 6. C3z-1b when P0 has restored data101.
7. C3z-8 (added 2026-10-19): its scope half is read-only and runs with anything; port halves wait for the user's look at the scope and, where they need June's
   `BattleFieldData.lua`, for atlas A6b.
8. W3 (added 2026-10-20, from june-engine): any time no other session packs `data040`.
