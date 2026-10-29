# STATUS - what is running, what is next

One board for every phased plan. **Updated by the session that finishes a phase or review, in
the same commit as its Done note** (the x2-phase skill's Finish list). If a row and `git log`
disagree, `git log` wins; fix the row.

**How to start anything below:** open a new conversation, pick the model and effort in the row,
type `/x2-phase <plan> <phase>` (or `... review`). `/x2-phase status` answers "what's next" and
"what can run together" from this file.

**Running two sessions at once** is fine when (a) neither depends on the other, (b) at most one
of them edits code, or their "Touches" do not share a file, and (c) both commit with explicit
paths. Builds queue through one lock. Read-only steps (RE, reviews
that find nothing, planning) can always run alongside.

**One branch: `main`** (CLAUDE.md rule).

States: `done` · `review` (done, review pending) · `play-test` (waiting on the user) ·
`ready` (can start) · `queued` (could start, but the user ordered another plan first) ·
`blocked` (dependency not done) · `decision` (waiting on a user decision).

**Keep this board small.** A row is one line: state, date, commit, what the user owes (about 200 characters). Evidence goes
in the Done note. When a plan closes, move its block to `STATUS_ARCHIVE.md` and add one line to *Closed plans*.
Done rows of open plans from before 2026-10-29 are in `STATUS_ARCHIVE.md` (grep it by phase id).

---

## june-dungeons - June dungeons, worlds and their map / entry content
Plan `JUNE_DUNGEONS_PHASES.md` (phase text and earlier Done notes of these phases still sit in `JUNE2017_EFFECT_COMPLETION_PHASES.md`) · current branch · log tag `JUNE_FX <phase>` (ErrorLog.txt; use `logs.py --grep "JUNE_FX C3y-1c"`) · flags `SERV_IRUHADEV_JUNE_*` (one per logical change, named in each kickoff) · kickoffs `kickoffs/june-dungeons/` · created 2026-10-11 by splitting these rows out of june-effect · ...

Done: C3y-1b.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| C3y-1c | queued: the Sander secret dungeon: `DI_SANDER_SECRET_HELL` 41141 (DungeonEnum), 2 DungeonData rows, 32 NPC ids (Enum + NPCTemplet, unit scripts already staged), meshes, `SanderSecret` map button (June's own icon file by ...) | Opus 5.5 / high | C3y-1b, C3y-1a; one at a time with the port halves of C3y-2 / 3 / 4 (same ...) | data036 Enum.lua + DungeonEnum.lua, data035 DungeonData.lua, data040 WorldData.lua, data037 ... | `C3y-1c.md` |
| C3y-2 | ready (C3y): identify, then port June's class tutorial dungeons (9 March classes, 41150-41290, worlds 624-632); identify June's entry route first and stop if it replaces March's new-character flow (Phase 3B ...) | Opus 5.5 / high | C3y; identify half runs with anything; port half one at a time with C3y-1b / ... | read-only first; port half: data036 Enum.lua + DungeonEnum.lua, data035 DungeonData.lua, data040 ... | `C3y-2.md` |
| C3y-3 | **ready** (C3y): identify, then port Bestera 1-4 + `DI_BESTERA_START` (150000-150040, worlds 736-743, 25-29 NPCs each, item 78988); June `IsActiveDungeon` `327da190` Bestera branch | Opus 5.5 / high | C3y; identify half runs with anything; port half one at a time with C3y-1b / ... | read-only first; port half: data036 Enum.lua + DungeonEnum.lua, data035 DungeonData.lua, data040 ... | `C3y-3.md` |
| C3y-4 | ready (C3y): Space of Thought 1-3 + `_DUMMY` (41480-41510, worlds 771-773, 30 June-only `NUI_PVP_HERO_*`); restore the commented Special-map line (data217 `DLG_Map_Local_Special.lua`, core); `m_iLimitPlayCount` ... | Opus 5.5 / high | C3y; **one at a time with the port halves of C3y-1b / 1c / 2 / 3** | data036 Enum.lua + DungeonEnum.lua, data035 DungeonData.lua, data040 WorldData.lua, data037 ... | `C3y-4.md` |
| C3y-5 | **ready** (C3y): identify (read-only) June's real route into the Hero tier (191 ids; the Mode Button has no callers), then write the port phases | Opus 5.5 / **xhigh** | C3y; runs with anything | read-only (Ghidra), plan doc | `C3y-5.md` |
| C3z-1b | waiting (june-asset-port P0's data101 restore): re-run `dungeon_map_parity.py` once data101's dir equals live, then stage `c3z1_data101_pending.tsv` (104 Velder / Henir names; grows with the 799 entries the broken ...) | Opus 5.5 / high | june-asset-port P0, C3z-1 | data101 (kom_contents, game dir) | `C3z-1b.md` |
| C3z-5b | ready (routed by C3z-5): DCC 8 riding pet (live on Henir; 31fcf040, 321f9480, 321f99e0, 321fa590, 3204ea20), DCC 2 vs March's IsEventDungeon lists, DCC 1/9/10/12/13/14 readers, June's GetLastClearDungeonID ... | Opus 5.5 / high | C3z-5; best before/with C3y | X2RidingPetManager / X2GUUser, X2Dungeon(.h/Manager), X2Unit, X2PartyUI, the DCC sites, Always.h | `C3z-5b.md` |
| C3z-2b | ready, scope first (user 2026-10-16, from C3z-2): the 142 June-only NPCs C3z-2 added give 0 EXP / ED / drops because their payout rows were server data; identify which tables lack them, port only rows that can be ... | Opus 5.5 / high | C3z-2 | data / server Lua (named for the user to pack), `X2Offline*Table.cpp` only if a loader gap shows | `C3z-2b.md` |
| C3z-4t | ready, scope first (user 2026-10-10, from C3z-4): June's CX2TutorialGuideManager (GetTutorialGuideManager / CheckToNeedTutorialGuide / OnBeginGuideNo; 9 Battle Field tutorial dungeons via benders_actor.lua); report ... | Opus 5.5 / high | C3z-4 | X2Main (embedded manager), new manager files, bindings | `C3z-4t.md` |
| C3z-8 | ready, scope first (user 2026-10-19, routed from skill-system-core S7o): June's boss-raid systems - `CX2BossRaidLongTimeEvent` ( `g_pMain +0xd80`: enter UI `32161070` / `DLG_BossRaid_Enter.lua`, state + rank ...) | Opus 5.5 / high | skill S7o (done); port halves: A6b ( battlefield types ) where June's ... | read-only (Ghidra), plan doc, funcmap | `C3z-8.md` |
| W3 | ready (moved from june-engine by its scope audit 2026-10-20, same ID): census every village world script + line map in data040 against June, stage the closure, adopt June's (ljconv, no edit), pack data040, ask, ... | Sonnet 5.5 / high (census, then adopt), then user play-test | - ; one at a time with the port halves of C3y-1b / 1c / 2 / 3 / 4 and any ... | `kom_contents/data040_kom/` village world + line-map `.lua`, staged closure, ... | `W3.md` (text: `JUNE_ENGINE_REMAINDER_PHASES.md` "Phase W3") |

## costume-shop - every June costume in the offline cash shop
Plan `COSTUME_SHOP_PHASES.md` · branch `main` · log tag `[COSTUME_SHOP]` · flag `SERV_IRUHADEV_COSTUME_SHOP` · kickoffs `kickoffs/costume-shop/` · created 2026-10-08 (user request after june-item-kim K4).

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| S0 | **built + deployed, awaiting play-test** (38,844 rows written into els_db.sql by costume_shop/insert_products.py, no seeder; client file check built; user owes the play-test, then "check logs") | Sonnet 5.5 / high, then user play-test | june-item-kim K4 (done) | `X2Lib/Offline/X2OfflineCashShop.cpp/.h`, `X2OfflineDB` (read/insert only), `KTDXLIB/Always.h`, ... | `S0.md` |

## june-asset-port - every remaining June non-lua asset in the March client
Plan `JUNE_ASSET_PORT_PHASES.md` · branch `main` · log tag `[KOM_MOUNT]` · flag `SERV_IRUHADEV_KOM_MOUNT_DEBUG` (P1, short-lived) · kickoffs `kickoffs/june-asset-port/` · backlog Stage 15 · created 2026-10-08.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| P0 | ready (`data118_kom` `blur_16.dds` already restored from the live kom, 2026-10-10, C3s mesh copy) | Sonnet 5.5 / high | - | `tools/kom.py`, `tools/README.md`, new `june2017_re/asset_port_census.py` + `p0_*.tsv`, ... | `P0.md` |
| P1 | waiting (P0) | Sonnet 5.5 / high, then user play-test | P0 | `KTDXLIB/KGCMassFileManager.cpp`, `KTDXLIB/Always.h`, `tools/logs.py` | `P1.md` |
| P1 review | waiting (P1) | Sonnet 5.5 / high | P1 | review (P1 files only if a fix) | `P1-review.md` |
| P2 | waiting (P1 review) | Sonnet 5.5 / high, then user play-test | P1 review | new `kom_contents/data152,179,214,224,i027_kom`, game dir koms | `P2.md` |
| P3 | waiting (P2) | Sonnet 5.5 / medium, then user play-test | P2 | 61 new `kom_contents/dataNNN_kom`, game dir koms | `P3.md` |
| P4a | waiting (P3) | Sonnet 5.5 / medium, then user play-test | P3 | `kom_contents/data001-075_kom`, game dir koms | `P4a.md` |
| P4b | waiting (P4a) | Sonnet 5.5 / medium, then user play-test | P4a | `kom_contents/data076-150_kom`, game dir koms | `P4b.md` |
| P4c | waiting (P4b) | Sonnet 5.5 / medium, then user play-test | P4b | `kom_contents/data151-226_kom`, `datai*_kom`, game dir koms | `P4c.md` |
| P5 | waiting (P0; can run alongside P2-P4) | Opus 5.5 / high | P0 | new `june2017_re/asset_keys.py`, `p5_*.tsv`, `p6/p7/p8_wave.tsv`, owners' docs ... | `P5.md` |
| P5 review | waiting (P5) | Opus 5.5 / high | P5 | review | `P5-review.md` |
| P6 | waiting (P4c, P5 review) | Sonnet 5.5 / high, then user play-test | P4c, P5 review | koms on `p6_wave.tsv` | `P6.md` |
| P7 | waiting (P6) | Sonnet 5.5 / high, then user play-test | P6 | koms on `p7_wave.tsv` | `P7.md` |
| P8 | waiting (P7) | Sonnet 5.5 / high, then user play-test | P7 | koms on `p8_wave.tsv` | `P8.md` |
| P9 | waiting (P0; independent of the kom waves) | Sonnet 5.5 / high, then user play-test | P0 | game dir `music/`, `movie/`, `eventscene/` | `P9.md` |
| P10 | waiting (P4a). june-engine R2 (reviewed 2026-10-17) already copied June's `XRenderer.fx` into data041 (`COPY OK` md5 027c8a01): P10 = gates + play-test only | Sonnet 5.5 / high, then user play-test | P4a, june-engine R2 review (done) | `kom_contents/data041_kom` (`XRenderer.fx`), game dir data041 | `P10.md` |
| P11 | conditional (P2/P4 measurements) | Opus 5.5 / high | P2 | `X2/X2_2010.vcxproj` (user approval first) | `P11.md` |
| P12 | waiting (P3, P4a-c, P6-P10) | Sonnet 5.5 / medium | all | census, STATUS, backlog | `P12.md` |

## lua-source - core client Lua as proven plaintext source, the rest as June's bytecode
Plan `LUA_SOURCE_PHASES.md` · branch `main` · log tag `[LUA_SRC]` · flag `SERV_IRUHADEV_LUA_SOURCE` (+ `_DEBUG`, L6 only) · kickoffs `kickoffs/lua-source/` · skills `x2-lua`, `x2-adopt-june` (built here) · created 2026-10-09 (user request).

Done: L0, L1, L2, L2 review, L3, L4, L6, L6 review, L7.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| L5 | **ready, user decides** (L3 named 6 core files failing T1: the 2 hoist files plus 4 with an instruction-count difference; `NewSkillTempletVer2` and `ELYSION_SOLESS` (ljd timeout) are off the core list) | Opus 5.5 / **xhigh** | L3 | `june2017_re/` ljd hooks | `L5.md` |

## dev-overlay - loose Lua for development, pack at plan close-out and release
Plan `DEV_OVERLAY_PHASES.md` · branch `main` · log tag `[DEV_OVERLAY]` (`data\dev_overlay.log`) · flag `SERV_IRUHADEV_DEV_OVERLAY` · kickoffs `kickoffs/dev-overlay/` · tool `tools/overlay.py` + skill `x2-overlay` (built here) · created 2026-10-09 (user request) · **`.lua` only** (user, 2026-10-09). Non-Lua assets keep the usual kom flow.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| O0 | ready | Sonnet 5.5 / high | - | `tools/data/overlay_baseline.tsv` (new); may pack/copy pending koms | `O0.md` |
| O1 | waiting (O0) | Opus 5.5 / high, then user play-test | O0 | `KTDXLIB/KGCMassFileManager.cpp/.h`, `KTDXLIB/KTDXDeviceManager.cpp`, `KTDXLIB/Always.h` | `O1.md` |
| O1 review | waiting (O1) | Opus 5.5 / high | O1 | review (O1's files if a fix is needed) | `O1-review.md` |
| O2 | waiting (O0) | Sonnet 5.5 / high | O0 (O1 for `served`) | `tools/overlay.py` (new), `tools/logs.py`, `.claude/skills/x2-overlay/` (new), `tools/README.md` | `O2.md` |
| O3 | waiting (O1 review, O2) | Sonnet 5.5 / high, then user play-tests | O1 review, O2 | overlay test files only (all dropped at the end) | `O3.md` |
| O4 | waiting (O3, lua-source L2 committed) | Opus 5.5 / high, then user play-test | O3, lua-source L2 | `tools/overlay.py`, `tools/luasrc.py` (produce/commit split), `tools/data/overlay_promotions.tsv` ... | `O4.md` |
| O5 | waiting (O4) | Opus 5.5 / high | O4 | CLAUDE.md, `.claude/skills/` (x2-lua, x2-phase, elstools, x2-kom-packed, x2-check-logs), ... | `O5.md` |
| O5 review | waiting (O5) | Opus 5.5 / high | O5 | docs (fixes only) | `O5-review.md` |

## june-backlog - backlog stage planning briefs (`JUNE_PORT_BACKLOG_ORDER.md`)
Branch `main` · plan: `JUNE_PORT_BACKLOG_ORDER.md` · each row is a read-only planning session (Ghidra read-only, no code, no build) that writes a new phased plan and its own STATUS block; each row is started with `/x2-phase new-plan <plan key>` (the key is in the row); the skill reads the row's brief first.

Done: stage2-plan, stage3-plan, stage4-plan, stage29-plan, stage5-plan, stage6-plan, stage34-plan, stage35-plan.

| Phase | State | Model | Needs | Touches | Kickoff |
|---|---|---|---|---|---|
| stage12-plan | **ready** (start: `/x2-phase new-plan june-customize`): character customize system (`CX2CustomizeManager`, `CharCustomData.lua`) -> `JUNE_CUSTOMIZE_SYSTEM_PHASES.md` + `kickoffs/june-customize/` | Opus 5.5 / high | dep: june-effect C9g-2 review (done); runs with anything (read-only) | docs, funcmap rows (`Customize S12`) | `stage12-plan.md` |
| stage23-plan | ready (start: `/x2-phase new-plan field-rewards`): offline field rewards stand-in for Lanox + Elysion fields (no server drop rows past 40025; found by atlas A6) -> `OFFLINE_FIELD_REWARDS_PHASES.md` + ... | Opus 5.5 / high | dep: atlas A6 (done); runs with anything (read-only) | docs | `stage23-plan.md` |
| stage25-plan | ready (start: `/x2-phase new-plan june-fonts`): June fonts and text rendering (June's `data196` font set, June's FreeType setup; user: March text looks un-anti-aliased) -> `JUNE_FONTS_PHASES.md` + ... | Opus 5.5 / high | dep: -; runs with anything (read-only) | docs, funcmap rows (`Fonts S25`) | `stage25-plan.md` |
| stage26-plan | ready (start: `/x2-phase new-plan june-movies`, after quest-system Q8 review): June AVI movie system remainder - dungeon first-clear / intro movies, the field / battle-field starts, the other `+0x80a` readers, ... | Opus 5.5 / high | dep: quest-system Q8 review | docs, kickoffs, funcmap | `stage26-plan.md` |
| stage27-plan | ready (start: `/x2-phase new-plan june-lua-scenes`): June Lua event scenes - adopt `EventScene.lua` / `EventSceneObject.lua` with June's DUO keys (`325b1f40` / `325b0020` / `325b0c70`), find the Lua-scene callers ... | Opus 5.5 / high | - | docs, kickoffs, funcmap | `stage27-plan.md` |
| stage28-plan | ready (start: `/x2-phase new-plan june-party`): June party-manager handlers - diff June's `CX2PartyManager` packet handler `3243ee30` against March's (the ready-check's June-only branch `3243cec0`, the auto-party ...) | Opus 5.5 / high | - | docs, kickoffs, funcmap | `stage28-plan.md` |
| stage30-plan | ready (start: `/x2-phase new-plan june-resonance`): June's level-99 resonance system (`KUnitInfo` tail -> CX2Unit `+0xa99`..`+0xaa9`, gate `3247b460`, EXP `322f8920`; found by june-result R0, identified by Stage 6 ...) | Opus 5.5 / high | dep: -; runs with anything (read-only); code after Stage 9 | docs, funcmap rows (`Resonance S30`) | `stage30-plan.md` |
| stage31-plan | ready (start: `/x2-phase new-plan june-achievement`): June's achievement system (`CX2AchievementManager`, `AchievementData` / `AchievementConditionData.lua`, `DLG_Achievement_System_UI`; none in March) -> ... | Opus 5.5 / high | dep: -; runs with anything (read-only); code after Stage 9 | docs, funcmap rows (`Achievement S31`) | `stage31-plan.md` |
| stage33-plan | ready (start: `/x2-phase new-plan june-stat-calc`): June's damage and character stat calculation. Map every stat / damage formula as June / March / mixed, then replace each mixed one with June's whole function ... | Opus 5.5 / xhigh | dep: -; runs with anything (read-only); code after the character ports, Stages ... | docs, `june2017_re/statcalc/`, funcmap rows (`StatCalc S33`) | `stage33-plan.md` |
| stage7-plan | Eve planned 2026-10-25, closed 2026-10-28 (Z) (`JUNE_CHARACTER_EVE_PHASES.md` + `kickoffs/char-eve/` + the `char-eve` block; next planning session: `/x2-phase new-plan char-chung`). ready (one planning session ...) | Opus 5.5 / high | dep: pilot-char-revamp Z (done) | docs, kickoffs | `stage7-plan.md` |

## june-engine - June engine remainder: renderer contract, render types, UI framework, world timeline
Plan `JUNE_ENGINE_REMAINDER_PHASES.md` · branch `main` · log tag `[JUNE_ENGINE]` (sub-tags `CTT`, `FX`, `ENVBOX`, `SHIMMER`, `CRS`, `SMRT`, `UI`, `WORLD`) · flags `SERV_IRUHADEV_JUNE_RENDER_*` / `_JUNE_UI_*` / `_JUNE_WORLD_*` · kickoffs `kickoffs/june-engine/` · backlog Stage 2 · created 2026-10-16.

Done: R0, R0b, R1, R2, R3, R4, R4e, R4f, R10, R11, R12, R12b, R14, R14b, R13, R13b, R13c, R13d, U0, U1, W0, W1, W1b, W1g, W3.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| R4a | built + deployed 2026-10-18 (flags `JUNE_RENDER_HEAT_SHIMMER` + `JUNE_RENDER_PASS1`; 15 funcmap rows `(R4a)`; JUNE_BUGS 273-274): heat-shimmer object (new `KTDGHeatShimmer.*`, made / reset / lost / deleted by the DG ...) | **Opus 5.5 / high**, Opus review, then user play-test | R4; not with R8-style DG / dialog-manager work or U1 | `KTDGManager.*`, `KTDGObject.*`, `KTDGXMeshPlayer.*`, `KTDGParticleSystem.*`, ... | `R4a.md`, review `R4a-review.md` |
| R4b | built + deployed 2026-10-18 (flags `JUNE_RENDER_TARGET` + `JUNE_RENDER_FRAME`; 17 funcmap rows `(R4b)`; JUNE_BUGS 280-281): frame `31be3800` + the CKTDXStage pass slots (March classes still draw in OnFrameRender), ... | **Opus 5.5 / high**, Opus review, then user play-test | R4a; not with another `KTDXApp.cpp` / `X2GameOption.*` editor | `KTDXApp.h/.cpp`, `KTDXStage.h`, `KTDXDeviceRenderTargetTexture.*`, `KTDXDeviceManager.*`, ... | `R4b.md`, review `R4b-review.md` |
| R4c | built + deployed 2026-10-18 (flag `JUNE_RENDER_STAGE_SPLIT`; 8 funcmap rows `(R4c)`): `CX2State` split into June's `0x8` / `0xc` / `0x10` / `0x18` (`31ef75b0` / `75f0` / `7640` / `76b0`) + StartUp, Login ... | **Opus 5.5 / high**, Opus review, then user play-test | R4b | `X2State*.h/.cpp`, Always.h | `R4c.md`, review `R4c-review.md` |
| R4c2 | waiting (added by R4c 2026-10-18): game and room state split (DungeonRoom, PVPRoom, DungeonGame, BattleField, PVPGame, Field, Square, Wedding + `CX2TFieldGame` / `CX2SquareGame` / `CX2WeddingGame`), removes R4c's ... | **Opus 5.5 / high**, Opus review, then user play-test | R4c + its review | `X2State*.h/.cpp`, `X2TFieldGame.*`, `X2SquareGame.*`, `X2WeddingGame.*`, Always.h | `R4c2.md`, review `R4c2-review.md` |
| R4h | waiting (added by R4c 2026-10-18; split by the scope audit 2026-10-20: the notice text manager `g_pMain +0x444` + `CX2NoticeManager` port moved to backlog Stage 9): the pass argument of the slot callees (text ...) | Opus 5.5 / high (map, then port), Opus review, then ... | R4c; not with another `X2TextManager.*` / `X2State.cpp` editor | `X2TextManager.*`, `X2GameEdit.*`, `X2Cursor.*`, `X2State.cpp`, Always.h | `R4h.md`, review `R4h-review.md` (R4h writes it) |
| R4i | ready (R4e review PASS 2026-10-18; added by R4e 2026-10-18): `CKTDGObject` vslot 0x10 / 0x2c overrides of `CX2UnitViewerUI` (`31f97960` / `31fae370`), `CX2Equip` (`3295caa0` / `3295bd50`), `CX2TalkBoxManager` ... | Opus 5.5 / high (map, then port), Opus review, then ... | R4e + its review | `X2UnitViewerUI.*`, `X2Equip` (X2Item*), `X2TalkBoxManager*.*`, Always.h | `R4i.md`, review `R4i-review.md` (R4i writes it) |
| R4j | ready (R4e review PASS 2026-10-18; added by R4e 2026-10-18): June-only `CKTDGUIStatic` list `+0x268` (untextured quads, every pass; filler unknown): RE the filler, port only if March can reach it | **Opus 5.5 / high** (RE first) | R4e | `KTDGUIStatic.*` (+ the filler it finds), Always.h | `R4j.md` |
| R4d | waiting (added by R4): game render split | **Opus 5.5 / high**, Opus review, then user play-test | R4c2 | `X2Game.*`, `X2DungeonGame.*`, `X2PVPGame*.*`, `X2BattleFieldGame.*`, Always.h | `R4d.md`, review `R4d-review.md` |
| R4g | waiting (added by R4b 2026-10-18): render-target call sites take June's arguments (opens `31cdf310`: env box / LastKillShot / minimap / MoveData; 4-arg BeginRender callers) + the last-kill shot while the frame is in the ... | Opus 5.5 / high (map, then port), Opus review, then ... | R4b + its play-test; not with R4d / R4e on the same files | `KTDGEnvBoxMap.cpp`, `X2Game.cpp`, `X2MiniMap.cpp`, `KTDGUIControl.cpp`, `X2Main.cpp`, Always.h | `R4g.md`, review `R4g-review.md` (R4g writes it) |
| R5 | waiting (R0, after R2); R13 added the camera world-mesh-hidden reader (kickoff amendment) | Sonnet 5.5 / high | R0 | unit / NPC / riding-pet render, Always.h, data036 `Enum.lua` | `R5.md`, review `R5-review.md` |
| R6 | built + deployed 2026-10-28 (flag `JUNE_SKIN_MESH_RENDER_TYPE` + short-lived `_DEBUG`; 10 funcmap rows `(R6)`; JUNE_BUGS 455-459): June's whole skin-anim draw 31d30380 (frame copy, hide, RenderParam pass 31d2f000, ...) | **Opus 5.5 / high**, then user play-test | R0, R0b | `KTDGXSkinAnim.h/.cpp`, `KTDXDeviceXSkinMesh.*`, `KTDGXRenderer.cpp`, `KTDGXSkinAfterImage.cpp`, ... | `R6.md`, review `R6-review.md` |
| R6b | ready (R6 review PASS 2026-10-28) (added by R6 2026-10-28): June writers of the mesh fields R6 reads: CX2Eqip mesh-flag switch 32964ae0 (alpha ids, Ara cases gone), equip glow 3295b970 (mesh +0x980 / +0xa2c), ... | Opus 5.5 / high (map, then port), Opus review, then ... | R6 + its review | `X2Eqip.*`, `X2GUUser.cpp`, Always.h | `R6b.md`, review `R6b-review.md` |
| R7 | waiting (after R2) | **Opus 5.5 / high** (map, then port), then user play-test | R0b, R2 | `KTDGXRenderer.cpp`, `KTDXDeviceManager.*`, `KTDXDeviceXSkinMesh.*`, unit face / equip code, ... | `R7.md`, review `R7-review.md` |
| R8 | built + deployed 2026-10-17: all 5 items, 7 flags (`JUNE_RENDER_DG_STATES`, `JUNE_UI_DIALOG_BANDS`, `JUNE_RENDER_DG_LISTS`, `JUNE_RENDER_FRUSTUM_TEST`, `JUNE_RENDER_ALPHA_SORT_KEY`, `JUNE_RENDER_HINT_NO_GROUP`, ...) | **Opus 5.5 / high** (map, then port), then user play-test | R0b; not with R4 (`KTDGManager.*`) or U1 (dialog manager) | `KTDGManager.*`, `KTDGObject.*`, `KTDGFrustum.h`, `KTDGUIDialogManager.*`, `KTDGUIDialog.*`, ... | `R8.md`, review `R8-review.md` |
| R9 | waiting (after R2); June's `CreateNewParticle` `31cf9570` was done 2026-10-21 in the pilot-char-revamp CH3a review (flag `JUNE_PARTICLE_TRACE_INHERIT`): not R9's to redo | Sonnet 5.5 / high, Opus review, then user play-test | R0b, R2 | `KTDGParticleSystem.*`, `KTDGXRenderer.cpp`, `KTDGEffectStateManager.h`, `KTDXDeviceXSkinMesh.cpp`, ... | `R9.md`, review `R9-review.md` |
| W1c | built + deployed 2026-10-17 (flag `JUNE_LINEMAP_LAND_MONSTER_ROAD` + short-lived `_DEBUG`): June's `31c7bb00` = March's body + the `LT_MONSTER_ROAD` skip unless `bMonster`; March's 3-arg signature forwards `true` ... | Opus 5.5 / high (map, then port), then user play-test | W1b; not with W2 / atlas E5 | `KTDGLineMap.*`, its callers, Always.h | `W1c.md`, review `W1c-review.md` |
| W1d | built + deployed 2026-10-18 (flag `JUNE_PET_AI_ROAD_RESEAT` + short-lived `_DEBUG`): June `CX2PETAI::Moving` `32ef68b0` = March's + one block `@32ef69b2-32ef6b08` (field / square game: the pet's own position landed, ...) | Opus 5.5 / high (map, then port), then user play-test | W1c | `X2PetAI.cpp`, Always.h | `W1d.md`, review `W1d-review.md` |
| W1e | built + deployed 2026-10-18 (flag `JUNE_LINEMAP_CANDOWN_LINK` + short-lived `_DEBUG`): June's `CanDown` `31c6c310` takes a `BEFORE_LINE` / `NEXT_LINE` only for a monster onto an enabled `LT_MONSTER_ROAD` (4 loop ...) | Opus 5.5 / high (map, then port), then user play-test | W1c; not with W2 | `KTDGLineMap.*`, Always.h | `W1e.md`, review `W1e-review.md` |
| W1f | built + deployed 2026-10-18 (flag `JUNE_LINEMAP_CANDOWN_SIGNATURE`, no `_DEBUG`): June's 9-arg `CanDown` `31c6c310` (in index read only at the head / fallback, separate out index `@31c6f86c`, `bDroppable` with the ...) | Opus 5.5 / high (map, then port) | W1e; not with W2 | `KTDGLineMap.*`, `X2DamageEffect.cpp`, `X2DropItemManager.cpp`, Always.h | `W1f.md`, review `W1f-review.md` |
| W2 | ready (W1b review PASS 2026-10-17; W1c review PASS 2026-10-18) | Opus 5.5 / high (map, then port), then user play-test | W0, june-effect C12; not with W1 / W1b / atlas E5 | `X2DungeonGame.cpp`, `X2Game.*`, `X2World.cpp`, `KTDGLineMap.*`, Always.h | `W2.md`, review `W2-review.md` |
| Z | waiting (all, C11) | Sonnet 5.5 / medium | every phase done or routed; june-effect C11 | plan doc, backlog, MODS.md | `Z.md` |

## quest-system - June quest system: engine, data, offline completion, quest UI, gates, random mission
Plan `JUNE_QUEST_SYSTEM_PHASES.md` · branch `main` · log tag `[JUNE_QUEST]` (sub-tags `LOAD`, `MIG`, `RUN`, `OFF`, `UI`, `SCENE`, `GATE`, `NPC`, `MOVIE`, `RM`) · flags `SERV_IRUHADEV_JUNE_QUEST_*` (+ `_DEBUG`) and `SERV_IRUHADEV_JUNE_RANDOM_MISSION` · kickoffs `kickoffs/quest-system/` · backlog Stage 4 · created 2026-10-19.

Done: Q0a, Q0b, RM0, Q1, Q2, Q3, Q4, Q4b, Q4d, Q4e, Q5a, Q5b, Q5c, Q5c review, Q6, Q7, Q8, Q8b, RM1, Q10, Q10b, Q10c, Q11.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| Q4c | built + deployed 2026-10-19 (flag `SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK`; DEPLOY OK md5 b304f506): March's room-server ranks from its own `ResultData_new.lua` (new `X2OfflineDungeonRank`, server getters bound by ...) | Opus 5.5 / high, then review | Q4 review; not with Q4b / RM1 (`X2Lib/Offline/`) | `X2Lib/Offline/Handlers_Room.cpp` (result data, end game), new rank module, Always.h, data036 (user ...) | `Q4c.md`, review `Q4c-review.md` |
| Q9 | paused at A3, waiting on Q4d (A1 / A2 passed: 22160 auto-opened in Lanox and completed on arrival; 22209 also opened and completed out of order, a Q4 question for after the walk). Before that: prepared 2026-10-19, ... | Sonnet 5.5 / medium, then user play-test | Q3, Q4, Q5b, Q6, Q7, Q8 | save (snapshot), checklist | `Q9.md` |
| Z | waiting (all; Q10c routed to Stage 9) | Sonnet 5.5 / medium | all except Q10c | `MODS.md`, backlog, memory, Always.h (`_DEBUG` off) | `Z.md` |

## june-result - June dungeon result screen: the in-state GFx result UI
Plan `JUNE_DUNGEON_RESULT_PHASES.md` · branch `main` · log tag `[JUNE_RESULT]` (sub-tags `MAP`, `WIDGET`, `EMU`, `FLOW`, `LEGACY`) · flags `SERV_IRUHADEV_JUNE_RESULT_*` (+ `_DEBUG`) · kickoffs `kickoffs/june-result/` · backlog Stage 29, split out of Stage 9 (user, 2026-10-21) · created 2026-10-21.

Done: R0, R1, R2, R3, R3b, R3c, R3d, R4.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| Z | blocked (waits for the R4 review) | Sonnet 5.5 / medium | R3 review (+ R4 review if run) | `MODS.md`, backlog, memory | `Z.md` |

## june-charinfo - June character info window (U key): dialog, header, stat panels, viewer, actions
Plan `JUNE_CHARINFO_PHASES.md` · branch `main` · log tag `[JUNE_CHARINFO]` · flags `SERV_IRUHADEV_JUNE_CHARINFO_*` (`_DLG`, `_SHOW`, `_STATS`, `_VIEW`, `_ACTIONS`, + `_DEBUG`) · kickoffs `kickoffs/june-charinfo/` · evidence `june2017_re/charinfo_functions.tsv` (C0) · backlog Stage 34, split out of Stage 9 (user, 2026-10-28) · planned on Opus 2026-10-28.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| C0 | **ready** (start: `/x2-phase june-charinfo C0`; read-only: function verdicts, the six packets, asset list, stub owners) | Opus 5.5 / high | - | `june2017_re/`, funcmap | `C0.md` |
| C1 | waiting on C0 | Sonnet 5.5 / high | C0 | data034 Lua + texture koms, `Enum.lua` (core list), `X2UICharInfo.h`, `X2UIManager.cpp`, `Always.h` | `C1.md` |
| C2 | waiting on C1 | Sonnet 5.5 / high | C1 | `X2UICharInfo.{h,cpp}` | `C2.md` |
| C3 | waiting on C2 | Opus 5.5 / high | C2 | `X2UICharInfo.{h,cpp}` | `C3.md` |
| C4 | waiting on C3 | Sonnet 5.5 / high | C3 | `X2UICharInfo.{h,cpp}`, `X2UIManager.cpp` | `C4.md` |
| C5 | waiting on C4 | Sonnet 5.5 / high | C4 | `X2UICharInfo.cpp`, `X2Lib/Offline/Handlers_*.cpp`, `X2OfflineDB` | `C5.md` |
| C5 review | waiting on C5 (covers C1-C5) | Opus 5.5 / high | C5 | - | `C5-review.md` |
| Z | waiting on the review | Sonnet 5.5 / medium | C5 review | `MODS.md`, backlog, memory | `Z.md` |

## june-march-divergence - One function per name: June merged into March's functions, no `June` names, no March behaviour in June code
Plan `JUNE_MARCH_DIVERGENCE_PHASES.md` · branch `main` · log tag none · flags: the collapse keeps each body's existing flag; D1 uses `SERV_IRUHADEV_JUNE_DIVERGE_<NAME>` · kickoffs `kickoffs/june-march-divergence/` · evidence `june2017_re/diverge/` · tables `tools/data/june_march_divergent.tsv`, `june2017_re/diverge/collapse_map.tsv` · backlog Stage 35 · planned on Opus 2026-10-28. Runs after the Eve port is ...

Done: D0a, D0b, C0.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| D0c-1..3 (never-compared callees) | D0c-1 done 2026-10-29 (409 callees in `d0c1_callees.tsv`; 42 tsv rows: replace 29, stage33 8, out 5; 127 funcmap rows; one port slip: buff sites call `IsFullHyperMode` for June vslot 0xa4). D0c-3 done 2026-10-29 ... | Opus 5.5 / high | D0b; beside the C phases | `june_march_divergent.tsv`, funcmap rows | `D0c-1.md`..`D0c-3.md` |
| C1 (Eve) | code done + deployed 2026-10-29 (`e79168a8` 182 renames, `df2b994d` 390 merges, `affc89cd` 24 hand shapes + hoist of 16 June namespaces / 8 macros above first use; 363 reach points in ...) | Sonnet 5.5 / high, then smoke | C0, D0b | `X2GuEve.{h,cpp}` | `C1.md` |
| C2 (Aisha) | code done 2026-10-29, smoke deferred to the end of the plan (user) (444 of 460 rows by the tool, 16 by hand; flag-off equivalence PASS; macros and 7 helper namespaces hoisted above the merged bodies; ...) | Sonnet 5.5 / high, then smoke | C0, D0b; beside C1 | `X2GUArme_VioletMage.{h,cpp}` | `C2.md` |
| C2 review (C1 + C2) | done 2026-10-29 (C1 + C2 pass; six offset names renamed; C3 kickoff: vslot 0x828 base turns virtual; ...) | Opus 5.5 / high | C1, C2 | as C1 / C2 | `C2-review.md` |
| C3 (shared unit code) + review | C3 + review done 2026-10-29, smoke deferred to plan end (merges and 2 guards pass; 6 dead dispatches from C1-C3 removed, tool now reports them; DEPLOY OK md5 9d9d18bb) | Opus 5.5 / high, then smoke | C2 review | `X2GUUser`, `X2GameUnit`, `X2GUNPC`, `X2Unit`, `X2RidingPet`, `X2DamageEffect`, `X2SkillTree`, ... | `C3.md`, `C3-review.md` |
| C4 (UI, data, engine, offline, class / ...) | C4 + review done 2026-10-29, smoke deferred to plan end (review: UpdateMenuPosistion twin merged, 3 Lua strings renamed; Ara glow double pass routed to june-engine R6b) | Sonnet 5.5 / high, Opus review, then smoke | C3 review | UI / quest / result / emblem / `X2Main` / KTDXLIB / Offline, `X2Lib_2010.vcxproj`, core-list Lua | `C4.md`, `C4-review.md` |
| C5 (members, locals, constants, ...) + review | C5 + review done 2026-10-29, smoke deferred to plan end (review: `DTT_END` / `BTI_HYPER_MODE` aliases replaced by `DTT_PANDEMONIUM_FEAR` / a category constant; skill 29024 named) | Sonnet 5.5 / high, Opus review | C4 review | every touched header and file | `C5.md`, `C5-review.md` |
| D1-1 (replace divergent: unit core) | done 2026-10-29, smoke at plan end (19 tsv rows, 9 `JUNE_DIVERGE_*` flags; D1 split into D1-1..D1-5, see "D1 split") | Opus 5.5 / high | C5 review, every D0c | `X2GUUser`, `X2GameUnit`, `X2GageData`, `X2Game`, buff displayer / behaviour, `KTDXDevice` | `D1.md` |
| D1-2 (user / NPC states, combat) | done 2026-10-29, smoke at plan end (11 tsv rows, 9 `JUNE_DIVERGE_*` flags; 242 / 244 split to D1-2b; 247 no code, routed to Stage 10) | Opus 5.5 / high | D1-1 | `X2GUUser.cpp`, `X2GUNPC.cpp`, `X2GUArme_VioletMage.cpp` | `D1-2.md` |
| D1-2b (camera move, dying start) + D1-2 review (D1-1 + D1-2 + D1-2b) | D1-2b + review done 2026-10-29, smoke at plan end (21 flags pass; camera height chain fixed under LUA_CAMERA; black-out + mark list -> D1-2c) | Opus 5.5 / high | D1-2 | `X2GUUser.cpp`, `X2Camera`, `ClientPacket.{h,cpp}`, `Offline/Handlers_Room.cpp` | `D1-2b.md`, `D1-2-review.md` |
| D1-2c (Process_ExtraDamage: black-out screens, timed mark list) | waiting | Opus 5.5 / high | D1-2 review; not beside D1-3d / D1-3e / D1-3f | `X2GUUser.cpp`, `X2GUNPC.cpp`, `X2GameUnit` | `D1-2c.md` |
| D1-3 (equipment, enchant, riding pet, items, skill tree) | done 2026-10-29, smoke at plan end (8 `JUNE_DIVERGE_*` flags, 9 rows + GetSkillLevel fold; 267 = call sites; 205 / 250 / 251 / 252 / 263 split to D1-3b) | Opus 5.5 / high | D1-2 review | `X2GameUnit`, `X2Inventory`, `X2RidingPet*`, `X2UserSkillTree`, `X2GUUser`, VioletMage / Eve calls | `D1-3.md` |
| D1-3b (upgrade-weapon glow, event cool-time rate) | done 2026-10-29, smoke at plan end (2 flags: cool-time rate `3234cc10` live; tsv 252 glow transcribed but inert, `ADD_UPGRADE_WEAPON_PARTICLE` is undefined here -> D1-3c; 250 / 205 / 263 -> D1-3c, 251 -> D1-3d) | Opus 5.5 / high | D1-3 | `X2GameUnit`, `X2GUUser`, `X2Unit`, `X2Define.h`, `X2UserSkillTree` | `D1-3b.md` |
| D1-3c (turn the +11 glow on, unit-viewer glow, weapon enchant show test) | done 2026-10-29, smoke at plan end (2 flags: `ADD_UPGRADE_WEAPON_PARTICLE` re-defined, viewer family ported, 5 frame moves; 31ff8510 / viewer moves / 263 -> D1-3e; 250 / 205 / MP sites -> D1-3f) | Opus 5.5 / high | D1-3b | `Always.h`, `X2UnitViewerUI`, `X2GameUnit`, `X2Eqip` | `D1-3c.md` |
| D1-3e (at-hand enchant effect 31ff8510, viewer enchant frame moves, SetEnchantParticleShow tsv 263) | waiting | Opus 5.5 / high | D1-3c; not beside D1-2c / D1-3d / D1-3f | `X2GUUser`, `X2UnitViewerUI`, `X2GameUnit` | `D1-3e.md` |
| D1-3f (reform table tsv 250, equip collision tsv 205, MP rate sites) | waiting | Opus 5.5 / high | D1-3c; not beside D1-2c / D1-3d / D1-3e | `X2GUUser`, `X2Data`, `X2UnitViewerUI`, class MP sites | `D1-3f.md` |
| D1-3d (UpdateNotRenderingOtherEquipList, tsv 251) | waiting | Opus 5.5 / high | D1-3b; not beside D1-2c / D1-3e / D1-3f | `X2GUUser`, `X2ItemManager` | `D1-3d.md` |
| D1-4 (UI, quest, local map, play guide) | done 2026-10-29, smoke at plan end (20 rows under 14 `JUNE_DIVERGE_*` flags; 7 rows need no code, 2 tsv verdicts wrong; 342 split to D1-4b) | Opus 5.5 / high | D1-1 | `X2LocalMapUI`, `X2UIQuestNew`, `X2QuestManager`, `X2PlayGuide`, informers, `X2Game`, `X2World`, `X2KeyPad`, `X2Main` | `D1-4.md` |
| D1-4b (quick-quest window, tsv 342) | done 2026-10-29, smoke at plan end (`QUICK_QUEST` flag; twin `SetUIQuickQuestSlotInfo` + `GetQuestColorString` ported; event-dungeon setters -> Stage 19) | Opus 5.5 / high | D1-4; beside D1-2 / D1-3 (UI files only) | `X2UIQuestNew` | `D1-4b.md` |
| D1-4 review (D1-3 + D1-3b-f + D1-4 + D1-4b + D1-2c) | waiting | Opus 5.5 / high | D1-3d, D1-3e, D1-3f, D1-4b, D1-2c | as D1-3 / D1-4 | `D1-4-review.md` |
| D1-5 (KTDXLIB engine, buff popups) + D1 review (closes D1) | waiting | Opus 5.5 / high | D1-4 review | `KTDXDeviceManager`, `KTDGUI*`, `KTDXInput`, `KTDGLineMap`, `X2BuffFactor`, buff templets | `D1-5.md`, `D1-review.md` |
| D2 (gate + wiring) + review / close | waiting | Sonnet 5.5 / high, Opus review | D1 review | `tools/junediverge.py`, allow-list, idiom sites, x2-phase skill, Stage 7 template, `CLAUDE.md` | `D2.md`, `D2-review.md` |

## Other plans (no STATUS rows)

| Plan | Doc | Note |
|---|---|---|
| backlog | `JUNE_PORT_BACKLOG_ORDER.md` | The order of every stage; planning briefs are the `june-backlog` rows above. |
| lanox | `LANOX_REMAINING_PHASES.md` + `JUNE_UI_PARITY_PHASES.md` | Check the doc for what landed; its Lua rule is superseded by `LUA_SOURCE_PHASES.md` L7. |
| char-port | `JUNE2017_CHARACTER_PORT_CHECKLIST.md` | Checklist, not phased. |

## Closed plans

Full rows in `STATUS_ARCHIVE.md`.

| Plan | Doc | Leftovers (owned elsewhere or parked) |
|---|---|---|
| june-buff | `JUNE_BUFF_PORT_PHASES.md` | - |
| june-effect | `JUNE2017_EFFECT_COMPLETION_PHASES.md` | C3z-4s-pvp parked (no PvP offline); C1b / C8c / C10b-3 handed to the checklist / Stage 9 |
| offline-grade | `OFFLINE_MONSTER_GRADE_PHASES.md` | - |
| june-item-kim | `JUNE_ITEM_KIM_PHASES.md` | - |
| june-asset-extract | `JUNE_ASSET_EXTRACT_PHASES.md` | - |
| atlas | `ATLAS_PORT_PHASES.md` | A6b queued (battlefield types), E12b parked (needs PvP) |
| lanox-fix | `LANOX_REMAINING_PHASES.md` | - |
| skill-system-core | `JUNE_SKILL_SYSTEM_PHASES.md` | S7p routed to Stage 9 |
| june-gfx | `JUNE_GFX_RUNTIME_PHASES.md` | - |
| pilot-char-revamp | `JUNE_CHARACTER_AISHA_PHASES.md` | - |
| june-transcend | `JUNE_TRANSCENDENCE_PHASES.md` | - |
| june-bottombar | `JUNE_BOTTOMBAR_PHASES.md` | - |
| char-eve | `JUNE_CHARACTER_EVE_PHASES.md` | - |
| june-enum-names | `JUNE_ENUM_NAMES_PHASES.md` | - |
