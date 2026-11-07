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
| stage7-plan | Eve closed 2026-10-28; **Chung closed 2026-11-03**; **Elsword closed 2026-11-05**; **Raven planned 2026-11-05** (`JUNE_CHARACTER_RAVEN_PHASES.md` + `kickoffs/char-raven/` + the `char-raven` block). queued: `/x2-phase new-plan char-rena` (asks `ADD_RENA_SYSTEM`), set ready by char-raven Z | Opus 5.5 / high | dep: pilot-char-revamp Z (done) | docs, kickoffs | `stage7-plan.md` |
| stage8-plan | **Lu/Ciel planned 2026-11-02; parked by the user 2026-11-03 until Stage 7 closes** (`JUNE_CHARACTER_LUCIEL_PHASES.md` + `kickoffs/char-luciel/` + the `char-luciel` block). queued: `/x2-phase new-plan char-rose` (then `char-ain`), set ready by char-luciel Z | Opus 5.5 / high | dep: char-luciel Z (NU / DU foundation) | docs, kickoffs | `stage8-plan.md` |

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
| R5 | ready (R0 and R2 done, R2 closed 2026-10-17); R13 added the camera world-mesh-hidden reader (kickoff amendment) | Sonnet 5.5 / high | R0 | unit / NPC / riding-pet render, Always.h, data036 `Enum.lua` | `R5.md`, review `R5-review.md` |
| R6 | built + deployed 2026-10-28 (flag `JUNE_SKIN_MESH_RENDER_TYPE` + short-lived `_DEBUG`; 10 funcmap rows `(R6)`; JUNE_BUGS 455-459): June's whole skin-anim draw 31d30380 (frame copy, hide, RenderParam pass 31d2f000, ...) | **Opus 5.5 / high**, then user play-test | R0, R0b | `KTDGXSkinAnim.h/.cpp`, `KTDXDeviceXSkinMesh.*`, `KTDGXRenderer.cpp`, `KTDGXSkinAfterImage.cpp`, ... | `R6.md`, review `R6-review.md` |
| R6b | ready (R6 review PASS 2026-10-28) (added by R6 2026-10-28): June writers of the mesh fields R6 reads: CX2Eqip mesh-flag switch 32964ae0 (alpha ids, Ara cases gone), equip glow 3295b970 (mesh +0x980 / +0xa2c), ... | Opus 5.5 / high (map, then port), Opus review, then ... | R6 + its review | `X2Eqip.*`, `X2GUUser.cpp`, Always.h | `R6b.md`, review `R6b-review.md` |
| R6c | waiting (routed from june-march-divergence D1-3e, tsv 263): June's equip enchant-particle list `+0x8f0` (3295cd30 / 3295bf90) in place of `Weapon::SetEnchantParticleShow`'s three handles | Opus 5.5 / high (map, then port), Opus review | R6b | `X2Eqip.*`, `X2GameUnit.cpp`, Always.h | `R6c.md` |
| R14c | ready (routed from june-march-divergence Phase S fix 2): June's basic-item collision anim `+0x584` / `+0x5e4` (3295dfa0, 329657d0) for every attach-anim equip; collision sites use the worn item's anim until then; + InitEqip 32024510 and viewer 31f94360 whole (char-raven CH4c) | Opus 5.5 / high, Opus review | R14b | `X2Eqip.*`, `X2GUUser.cpp`, `X2UnitViewerUI.cpp`, `X2GUArme_VioletMage.cpp`, Always.h | `R14c.md` |
| R15 | ready (routed from june-march-divergence D2 review): June's `CX2GameUnit::PlaySound` (32276220) puts looped sounds on the unit's close list too; caller audit first (Chung's `SAFE_CLOSE`) | Opus 5.5 / high, Opus review | - | `X2GameUnit.cpp`, looped callers, Always.h | `R15.md` |
| R12c | waiting (routed from june-march-divergence D1-3d): June's `SetpHairSkinMesh` (3322a5d0) refs the after-image hair mesh and closes the old one; March only stores it | Opus 5.5 / high, Opus review | R12 | `KTDGXSkinAfterImage.*`, Always.h | `R12c.md` |
| R7 | ready (R0b and R2 done) | **Opus 5.5 / high** (map, then port), then user play-test | R0b, R2 | `KTDGXRenderer.cpp`, `KTDXDeviceManager.*`, `KTDXDeviceXSkinMesh.*`, unit face / equip code, ... | `R7.md`, review `R7-review.md` |
| R8 | built + deployed 2026-10-17: all 5 items, 7 flags (`JUNE_RENDER_DG_STATES`, `JUNE_UI_DIALOG_BANDS`, `JUNE_RENDER_DG_LISTS`, `JUNE_RENDER_FRUSTUM_TEST`, `JUNE_RENDER_ALPHA_SORT_KEY`, `JUNE_RENDER_HINT_NO_GROUP`, ...) | **Opus 5.5 / high** (map, then port), then user play-test | R0b; not with R4 (`KTDGManager.*`) or U1 (dialog manager) | `KTDGManager.*`, `KTDGObject.*`, `KTDGFrustum.h`, `KTDGUIDialogManager.*`, `KTDGUIDialog.*`, ... | `R8.md`, review `R8-review.md` |
| R9 | ready (R0b and R2 done); June's `CreateNewParticle` `31cf9570` was done 2026-10-21 in the pilot-char-revamp CH3a review (flag `JUNE_PARTICLE_TRACE_INHERIT`): not R9's to redo | Sonnet 5.5 / high, Opus review, then user play-test | R0b, R2 | `KTDGParticleSystem.*`, `KTDGXRenderer.cpp`, `KTDGEffectStateManager.h`, `KTDXDeviceXSkinMesh.cpp`, ... | `R9.md`, review `R9-review.md` |
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

## char-raven - June character port, Stage 7: Raven
Plan `JUNE_CHARACTER_RAVEN_PHASES.md` · branch `main` · log tag `[JUNE_CHAR]` (lines `RAVEN <sub>`: `DATA`, `CORE`, `RAGE`, `SKILL`, `PASSIVE`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_RAVEN_*` (`_CORE`, `_LIVE`, `_RAGE`, `_SKILL`, `_PASSIVE`, `_UI`, + `_DEBUG`) · kickoffs `kickoffs/char-raven/` · backlog Stage 7 · created 2026-11-05.

**Scope:** Raven (`CX2GURaven`, 7 classes 4 / 16 / 17 / 25 / 106 / 107 / 115) from his March kit to June 2017's, incl. the Nasod core / rage gauge and hyper orbs (live March code), Veteran Commander overheat, Burning Nasod Hand and his transcendence rows. June class `32c25a20`-`32c6a63d` (440 carved + 28 uncarved overrides, 346 script states, no summons). CH0 now; CH0b after CH0; code after CH0 (char-elsword closed 2026-11-05).

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| CH3a | review PASS 2026-11-06, no code change, `_SKILL_DEBUG` retired; logs passed; **user's visual verdict owed** (motions, effects, cool time, MP) | Sonnet 5.5 / high, Opus review, user play-test | CH2d review | `X2GURaven.*` | `CH3a.md` |
| CH3b | review 2026-11-06: one fix (Devastating Strike's 4 s full explosion rate, `CX2GameUnit` timer) + Wild Charge A5 / B5 key; `_SKILL_DEBUG` retired; logs checked (explosions only after the casts); **user's visual verdict owed** | Sonnet 5.5 / high, Opus review, user play-test | CH3a review | `X2GURaven.*`, `X2GameUnit.*` | `CH3b.md`, `CH3b-review.md` |
| CH3d | review PASS 2026-11-07: one stand-in comment (equip enchant list -> june-engine R6c), `_SKILL_DEBUG` retired; logs passed; **user's visual verdict owed** (hyper actives, TR pictures) | Opus 5.5 / high, Opus review, user play-test | CH3c2 review | `X2GURaven.*`, `X2GUUser.*`, `X2Offline*` | `CH3d.md`, `CH3d-review.md` |
| CH4c | built + deployed 2026-11-07: Shadow Step slot MP on June's id 24200, VC slot-icon pass dropped, June's 0x73 icon block; InitEqip / viewer -> R14c, pack -> Stage 36; log check: no line (the emulator puts 24217 in the slot, JUNE_BUGS 559); review next | Opus 5.5 / high, Opus review, user play-test | CH4b review, CH3c2 review | `X2Game.cpp`, `X2UISkillTreeNew2015.cpp` | `CH4c.md`, `CH4c-review.md` |
| CH6 | blocked (CH3d, CH4c reviews; CH5 review PASS): scans rerun + user walk incl. the becoming-transcended transition | Sonnet 5.5 / medium, user walk | CH3d, CH4a-c, CH5 reviews | docs, save | `CH6.md` |
| Z | blocked (CH6): close-out, `MODS.md`, template row, `stage7-plan` -> `char-rena` | Sonnet 5.5 / medium | CH6 | `MODS.md`, backlog, docs | `Z.md` |

Done: CH0 (2026-11-05; read-only map, no build), CH1a (2026-11-05; review PASS, no code change), CH1b (2026-11-05; review PASS, no code change), CH1c (2026-11-05; review PASS, no code change), CH1d (2026-11-05; review PASS after one fix: dash-jump X lowers y, JUNE_BUGS #537 withdrawn), CH1e (2026-11-06; review PASS, no code change), CH1f (2026-11-06; review PASS, no code change), CH1g (2026-11-06; review PASS, no code change), CH1g2 (2026-11-06; review PASS, no code change), CH1h (2026-11-06; review PASS, no code change; one comment fix routed to CH1i), CH1i (2026-11-06; review PASS, no code change), CH1j (2026-11-06; review PASS, no code change), CH1k (2026-11-06; review PASS, one comment fix; two March passive defaults routed to CH4a), CH1m (2026-11-06; review PASS, no code change), CH1n (2026-11-06; review PASS, no code change), CH1o (2026-11-06; review PASS, no code change), CH1p (2026-11-06; review PASS, no code change), CH1q (2026-11-06; review PASS, no code change; `_RAGE` defined by CH1), CH0b (2026-11-05; walk passed, user 2026-11-06), CH1 (2026-11-06; play-test passed; review PASS, `_RAGE_DEBUG` retired), CH5 (2026-11-06; play-test passed; review PASS, no code change), CH2a (2026-11-06; play-test passed; review PASS, no code change, `_SKILL_DEBUG` retired), CH2b (2026-11-06; play-test passed; review PASS, no code change, `_SKILL_DEBUG` retired), CH2c0 (2026-11-06; no play-test; review PASS, no code change), CH2c (2026-11-06; play-test passed; review PASS, no code change, `_SKILL_DEBUG` retired), CH2d (2026-11-06; play-test passed; review one fix: Enhanced Hellfire buff, user check passed; `_SKILL_DEBUG` retired), CH3c (2026-11-06; play-test passed; review PASS, no code change, `_SKILL_DEBUG` retired), CH3c2 (2026-11-07; play-test passed; review PASS, no code change, `_SKILL_DEBUG` retired), CH4a (2026-11-07; play-test passed; review PASS, no code change, `_PASSIVE_DEBUG` retired), CH4b (2026-11-07; log check passed; review PASS, no code change, `_PASSIVE_DEBUG` retired)

## char-luciel - June character port, Stage 8: Lu/Ciel (first new character)
Plan `JUNE_CHARACTER_LUCIEL_PHASES.md` · branch `main` · log tag `[JUNE_CHAR]` (lines `LUCIEL <sub>`: `DATA`, `NEWUNIT`, `CREATE`, `DUO`, `CORE`, `SKILL`, `PASSIVE`, `UI`, `NUM`, `JOB`, `TUTORIAL`) · flags `SERV_IRUHADEV_JUNE_CHAR_LUCIEL_*` (`_CORE`, `_LIVE`, `_DUO`, `_DATA`, `_STARTER_ITEMS`, `_SKILL`, `_PASSIVE`, `_UI`, `_CLASS_CHANGE`, `_TUTORIAL`, + `_DEBUG`) and class-agnostic `SERV_IRUHADEV_JUNE_CHAR_*` (`UNIT_TYPE_RANGE`, `UNIT_CLASS_WORD`, `MINOR_UNIT_TEMPLET`, `CREATE`, `DUO_BODY`, `EQUIP_VIEW`, `NESI_RENUMBER`, `DUO_VIEW`), `SERV_IRUHADEV_JUNE_USER_DAMAGE_STATE_GETTERS` · kickoffs `kickoffs/char-luciel/` · backlog Stage 8 · created 2026-11-02.

**Scope:** Lu/Ciel (`UT_LUCIEL` 10; classes 200 / 37 / 38 / 39 / 127 / 128 / 129) from nothing: new-character foundation (NU), two-body engine (DU), new class `CX2GULuciel` (June `32ae4080`-`32b4e76f`, ~583 functions, vtable `334f06fc`), job change, tutorial, skills, transcendence. **User, 2026-11-02:** maps (CH0, NU0) and CH0b now; every code phase after char-chung Z; **user, 2026-11-03: every remaining phase waits until the March characters are ported (Stage 7 closed)**; 15 starter rows; NESI migration yes; stat rows asked after CH0.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| NU1 | parked (user 2026-11-03: after Stage 7 closes; NU0 done 2026-11-02, kickoff amended): `UT_END` -> June's 13, arrays / loops / script table, `UC_END` guards, flag `UNIT_TYPE_RANGE` | Opus 5.5 / high, Opus review | NU0, char-chung Z | `X2UnitManager.*`, `X2Item*`, `X2State*`, `X2SocketItem.cpp`, Always.h | `NU1.md` |
| NU2 | blocked (NU1 review): unit class as June's word on the wire + emulator, flag `UNIT_CLASS_WORD` (CommonPacket.h exception) | Opus 5.5 / xhigh, Opus review | NU1 review | `CommonPacket.h`, `ClientPacket.h`, `ServerDefine.h`, `Handlers_Unit.cpp`, `X2Unit.cpp`, vcxproj | `NU2.md` |
| NU3 | blocked (NU1 review): Lu/Ciel's 7 `UnitTemplet` rows + parser for the 18 June-only keys | Opus 5.5 / high, Opus review | NU1 review | `UnitTemplet.lua`, `X2UnitManager.*`, data036 | `NU3.md` |
| NU4 | blocked (NU2, NU3 reviews): creation-screen entry (slot, portrait, preview, job buttons) + `SelectLuciel`, emulator create-unit, 15 starter rows, stat fallback fix, `createchar.py` | Opus 5.5 / high, Opus review, user play-test | NU2 review, NU3 review | `X2StateCreateUnit.*`, `X2Offline*`, `tools/createchar.py`, data koms | `NU4.md` |
| DU1 | parked (Stage 7 close; NU3 review): second body in `CX2GUUser`: members, minor-unit model, user sync, not-selected-body physics | Opus 5.5 / high, Opus review | NU3 review, char-chung Z | `X2GUUser.*`, `X2GameUnit.*` | `DU1.md` |
| DU2 | blocked (NU1 review): damage-state getter virtuals + `PhysicProcess_Jumping` / StateChange readers | Opus 5.5 / high, Opus review | NU1 review | `X2GUUser.*` | `DU2.md` |
| CH1a0 | blocked (NU1, DU1 reviews): Opus helpers CH1a calls: Lu/Ciel gauge class, `CX2GUUser` transform link (`31fc9d90` family), hold-key selector, switching guide | Opus 5.5 / high, Opus review | CH0, NU1 review, DU1 review | `X2GUUser.*`, `X2GageData.*`, `X2Game.*`, Always.h | `CH1a0.md` |
| CH1a | blocked (CH1a0 review): new `X2GULuciel.*` skeleton (ctor / dtor / members / own virtuals, LSI enum), flags `_CORE` / `_LIVE` | Opus 5.5 / high, Opus review | CH1a0 review | `X2GULuciel.*` (new), `X2Lib_2010.vcxproj`, `X2UnitManager.h`, Always.h | `CH1a.md` |
| CH1b | blocked (CH1a review): Lu common states (CH1c merged in by CH0), transcription under `_CORE` | Sonnet 5.5 / high, Opus review | CH1a review | `X2GULuciel.*` | `CH1b.md` |
| CH1d | blocked (CH1b review): Ciel common states (CH1e merged in by CH0), transcription under `_CORE` | Sonnet 5.5 / high, Opus review | CH1b review | `X2GULuciel.*` | `CH1d.md` |
| CH1f | blocked (CH1d review): Duo combos part 1, transcription under `_CORE` | Sonnet 5.5 / high, Opus review | CH1d review | `X2GULuciel.*` | `CH1f.md` |
| CH1f2 | blocked (CH1f review): Duo combos part 2 (split by CH0) | Sonnet 5.5 / high, Opus review | CH1f review | `X2GULuciel.*` | `CH1f2.md` |
| CH1g | blocked (CH1f2 review): Chiliarch + Royal Guard states part 1, transcription under `_CORE` | Sonnet 5.5 / high, Opus review | CH1f2 review | `X2GULuciel.*` | `CH1g.md` |
| CH1g2 | blocked (CH1g review): Chiliarch + Royal Guard states part 2 (split by CH0) | Sonnet 5.5 / high, Opus review | CH1g review | `X2GULuciel.*` | `CH1g2.md` |
| CH1h | blocked (CH1g2 review): Dread Lord + Noblesse states, transcription under `_CORE` | Sonnet 5.5 / high, Opus review | CH1g2 review | `X2GULuciel.*` | `CH1h.md` |
| CH1i | blocked (CH1h review): Demonio script states + riding (CH1j merged in by CH0), transcription under `_CORE` | Sonnet 5.5 / high, Opus review | CH1h review | `X2GULuciel.*` | `CH1i.md` |
| CH1k0 | blocked (CH1a0 review): Opus helpers: buff Time-finalizer passes `322a74a0` / `322a7000` / `3227bcb0` | Opus 5.5 / high, Opus review | CH1a0 review | `X2GameUnit.*`, `X2Buff*`, Always.h | `CH1k0.md` |
| CH1k | blocked (CH1i, CH1k0 reviews): overrides band 1 part 1, transcription under `_CORE` | Sonnet 5.5 / high, Opus review | CH1i review, CH1k0 review | `X2GULuciel.*` | `CH1k.md` |
| CH1k2 | blocked (CH1k review): overrides band 1 part 2 (state-id getters), transcription under `_CORE` | Sonnet 5.5 / high, Opus review | CH1k review | `X2GULuciel.*` | `CH1k2.md` |
| CH1l | blocked (CH1k2 review): overrides band 2 part 1 (DU2 getters), transcription under `_CORE` | Sonnet 5.5 / high, Opus review | CH1k2 review | `X2GULuciel.*` | `CH1l.md` |
| CH1l2 | blocked (CH1l review): overrides band 2 part 2 | Sonnet 5.5 / high, Opus review | CH1l review | `X2GULuciel.*` | `CH1l2.md` |
| CH1l30 | blocked (CH1a0 review): Opus helpers: unit scans and the effect aim (`31f75980`, `322f1a90`, `32295360`, `322d7b30`) | Opus 5.5 / high, Opus review | CH1a0 review | `X2Game.*`, `X2GameUnit.*`, `X2DamageEffect.*`, Always.h | `CH1l30.md` |
| CH1l3 | blocked (CH1l2, CH1l30 reviews): overrides band 2 part 3 | Sonnet 5.5 / high, Opus review | CH1l2 review, CH1l30 review | `X2GULuciel.*` | `CH1l3.md` |
| CH1m | blocked (CH1l3 review): registration (InitState `32b43d10`, InitStateCommon `32b3e8b0`, 364 rows; ten delegates declared for CH4a) | Opus 5.5 / high, Opus review | CH1l3 review | `X2GULuciel.*` | `CH1m.md` |
| CH1n | blocked (CH1m review): skill head `32b46580` (0x7b4, uncarved) + base callbacks | Opus 5.5 / high, Opus review | CH1m review | `X2GULuciel.*` | `CH1n.md` |
| CH1o | blocked (CH1n review): virtual wiring, June-only keys, checklist hooks | Opus 5.5 / high, Opus review | CH1n review | `X2GULuciel.*`, `X2GUUser.h` | `CH1o.md` |
| CH1p | blocked (CH1o, DU1, DU2 reviews): in-class duo switch, `DT_BOTH` / `DT_KEEP`, combination points, flag `_DUO` | Opus 5.5 / high, Opus review | CH1o, DU1, DU2 reviews | `X2GULuciel.*`, `Enum.lua` | `CH1p.md` |
| CH1 | blocked (CH1p, NU4 reviews, CH0b): `_LIVE` defined, factory cases, both scripts adopted, play-test | Opus 5.5 / high, Opus review, user play-test | CH1p review, NU4 review, CH0b | `X2GULuciel.*`, `X2Game.cpp`, Always.h, data036 | `CH1.md` |
| DU3 | blocked (CH1 review): June's equip-view rewrite (every class) + item-templet minor-unit fields | Opus 5.5 / high, Opus review, user play-test | CH1 review | `X2GUUser.*`, `X2Eqip.*`, `X2Item*`, class overrides | `DU3.md` |
| DU4 | blocked (DU3 review): NESI renumber + forward-only save migration (user yes 2026-11-02) | Opus 5.5 / xhigh, Opus review, user play-test | DU3 review | `X2Item.h`, `X2OfflineDB.*`, `X2UICharInfo.cpp`, save | `DU4.md` |
| DU5 | blocked (DU4 review): two bodies in the character window / unit viewer, form packets `0x4d9` / `0x4da` | Opus 5.5 / high, Opus review, user play-test | DU4 review | `X2UICharInfo.*`, `X2UnitViewerUI.*`, `Handlers_*` | `DU5.md` |
| CHN | blocked (CH1 review): job change 200 -> 37 / 38 / 39 -> 127 / 128 / 129 (quests, items, seeds, class-change dialogs) | Opus 5.5 / high, Opus review, user play-test | CH1 review | `X2Offline*`, `Handlers_Quest.cpp`, data koms | `CHN.md` |
| TU | blocked (CH1 review): tutorial dungeon 41240, field 39609, world 633, first-entry route | Sonnet 5.5 / high, Opus review, user play-test | CH1 review | `DungeonEnum.*`, `DungeonData.lua`, data koms | `TU.md` |
| CH2a | blocked (CH1 review): Duo (200) skills, flag `_SKILL` | Sonnet 5.5 / high, Opus review, user play-test | CH1 review | `X2GULuciel.*`, `MODS.md` | `CH2a.md` |
| CH2b | blocked (CH2a, CHN reviews): Chiliarch (37) skills | Sonnet 5.5 / high, Opus review, user play-test | CH2a review, CHN review | `X2GULuciel.*` | `CH2b.md` |
| CH2c | blocked (CH2b review): Royal Guard (38) skills part 1 | Sonnet 5.5 / high, Opus review, user play-test | CH2b review | `X2GULuciel.*` | `CH2c.md` |
| CH2c2 | blocked (CH2c review): Royal Guard (38) skills part 2 | Sonnet 5.5 / high, Opus review, user play-test | CH2c review | `X2GULuciel.*` | `CH2c2.md` |
| CH2d | blocked (CH2c2 review): Diablura (39) skills part 1 | Sonnet 5.5 / high, Opus review, user play-test | CH2c2 review | `X2GULuciel.*` | `CH2d.md` |
| CH2d20 | blocked (CH1 review): Opus helpers: the transform HUD (`3200f380`, `31fdc4c0`, `31fe2b80`, `31fd4930`) | Opus 5.5 / high, Opus review | CH1 review | `X2GUUser.*`, Always.h | `CH2d20.md` |
| CH2d2 | blocked (CH2d, CH2d20 reviews): Diablura (39) skills part 2 | Sonnet 5.5 / high, Opus review, user play-test | CH2d review, CH2d20 review | `X2GULuciel.*` | `CH2d2.md` |
| CH3a0 | blocked (CH2d2 review): summoned NPCs (moved from CH2a0): NPCTemplet / Enum.lua rows for 1906 / 1937 / 1908 / 2269 / 2270 | Opus 5.5 / high, Opus review, user play-test | CH2d2 review | `NPCTemplet.lua`, `Enum.lua`, `X2UnitManager.h`, data036 / data037 | `CH3a0.md` |
| CH3a | blocked (CH2d2, CH3a0 reviews): Dread Lord (127) skills | Sonnet 5.5 / high, Opus review, user play-test | CH2d2 review, CH3a0 review | `X2GULuciel.*` | `CH3a.md` |
| CH3b | blocked (CH3a review): Noblesse (128) skills | Sonnet 5.5 / high, Opus review, user play-test | CH3a review | `X2GULuciel.*` | `CH3b.md` |
| CH3c | blocked (CH3b, DU1 reviews): Demonio (129) skills part 1, `CUSTOM_RENDER_STATE` | Sonnet 5.5 / high, Opus review, user play-test | CH3b review, DU1 review | `X2GULuciel.*` | `CH3c.md` |
| CH3c20 | blocked (CH1 review): Opus helpers: `SLockOnSystem::ProcessLockOn` out of `CX2GUAra`, Garden of Haures NPC binders (4 unbound) | Opus 5.5 / high, Opus review | CH1 review | `X2GUAra.*`, `X2GUUser.*`, `X2GUNPC.*`, `X2Game.*` | `CH3c20.md` |
| CH3c2 | blocked (CH3c, CH3c20 reviews): Demonio (129) skills part 2 | Sonnet 5.5 / high, Opus review, user play-test | CH3c review, CH3c20 review | `X2GULuciel.*` | `CH3c2.md` |
| CH3d | blocked (CH3c2 review): hyper actives + transcendence rows, both bodies' cut-ins | Opus 5.5 / high, Opus review, user play-test | CH3c2 review | `X2GULuciel.*`, `X2GUUser.cpp`, data koms | `CH3d.md` |
| CH4a | blocked (CH2d2 review): passive state, delegate lists (9 of InitState's 10), flag `_PASSIVE` | Opus 5.5 / high, Opus review, user play-test | CH2d2 review | `X2GULuciel.*` | `CH4a.md` |
| CH4b | blocked (CH4a review): damage exception + overrides | Opus 5.5 / high, Opus review, user play-test | CH4a review | `X2GULuciel.*`, `X2GUUser.*` | `CH4b.md` |
| CH4c | blocked (CH4b review): memo and item readers (`MemoConditionLuciel`) | Opus 5.5 / high, Opus review, user play-test | CH4b review | `X2GULuciel.*`, `X2BuffTemplet.cpp`, `X2Game.cpp` | `CH4c.md` |
| CH5 | blocked (CH1, DU5 reviews): two-body cut-ins, HUD portraits, bounding-key dialog, combination-point UI | Sonnet 5.5 / high, Opus review, user play-test | CH1 review, DU5 review | `X2GULuciel.cpp`, UI koms | `CH5.md` |
| CH6 | blocked: scans rerun + user walk incl. creation, tutorial, job change and the becoming-transcended transition | Sonnet 5.5 / medium, user walk | CH3d, CH4c, CH5, TU reviews | docs, save | `CH6.md` |
| Z | blocked (CH6): close-out, `MODS.md`, "Template for Stage 8", `stage8-plan` -> `char-rose` | Sonnet 5.5 / medium | CH6 | `MODS.md`, backlog, docs | `Z.md` |

Done: CH0b (2026-11-03, 14,609 + 12 June files and 14 Lua staged, 44 koms packed and copied, skill icons PASS; user select-screen ErrorLog pass owed), CH0 (2026-11-02, read-only class map: 867 functions / 553 tables / 198 ids phased, splits + 5 Opus helper phases, kickoffs amended; stat-row question with the user), NU0 (2026-11-02, read-only engine map: 8 `nu0_*.tsv`, kickoffs amended, DU4 shape written).

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
| june-hud | `JUNE_HUD_PHASES.md` | Amusement / `SetShowUI` / PC-room stubs -> Stage 9; PvP item bar parked (no PvP offline) |
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
| char-chung | `JUNE_CHARACTER_CHUNG_PHASES.md` | Pandemonium Chaos no cut-in = June (`JUNE_BUGS` 519); Velder Market load hang undiagnosed; camera keys -> Stage 9 |
| char-elsword | `JUNE_CHARACTER_ELSWORD_PHASES.md` | cut-in item override -> Stage 12; camera keys -> Stage 9; Wind Slicer icon gap = June (`JUNE_BUGS` 520) |
| june-enum-names | `JUNE_ENUM_NAMES_PHASES.md` | - |
| june-march-divergence | `JUNE_MARCH_DIVERGENCE_PHASES.md` | june-engine R6c / R12c / R14c / R15, june-charinfo C4 add-on, Stage 36, Stage 9 (Korean-sound option) |
| june-charinfo | `JUNE_CHARINFO_PHASES.md` | Customize -> Stage 12, Resonance -> Stage 30, Achievement -> Stage 31, Lu/Ciel and Rose switching -> Stage 8 (STUB clicks); light option, cursor modes -> Stage 9 |
