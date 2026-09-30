# Local modifications

The `import-2014` commit is **not** stock 2014 code. It is stock code plus four
changes that were already applied to the working copy before git existed here,
plus mods added afterward -- see the *Since* column below. No pristine copy
exists on disk, so the import could not be split into a vanilla baseline plus
mods.

This file is the substitute for that baseline. Keep it current.

## Applied mods

The first four rows were already applied to the working copy before git existed
here; later rows are marked with the date they were added, in the *Since*
column.

| Flag | Since | Defined in | Client files | Server files |
|---|---|---|---|---|
| `SERV_IRUHADEV_SKILLTREE_NO_LOCK` | import-2014 | `KTDXLIB/Always.h:2414`, `KncWX2Server/Common/ServerDefine.h:4210` | `X2Lib/X2UISkillTreeNew.cpp:364` (added 2026-08-27), `X2Lib/X2UISkillTreeNew.cpp:5788` | `KncWX2Server/GameServer/UserSkillTree.cpp:1489` |
| `SERV_IRUHADEV_SKILL_SLOT_B_FREE` | import-2014 | `KTDXLIB/Always.h:2421`, `KncWX2Server/Common/ServerDefine.h:4217` | `X2Lib/X2UISkillTree.h:228`, `X2Lib/X2UISkillTreeNew.h:269`, `X2Lib/X2UserSkillTree.h:307`, `X2Lib/X2UserSkillTree.cpp:2078` | `KncWX2Server/GameServer/UserSkillTree.cpp` (lines 10, 766, 978) |
| `SERV_IRUHADEV_BUFF_DURATION_TEXT` | import-2014 | `KTDXLIB/Always.h:2428` | `X2Lib/X2GageUI.{h,cpp}`, `X2Lib/X2BuffTemplet.{h,cpp}`, `X2Lib/X2BuffFinalizerTemplet.h`, `X2Lib/X2GameUnit.cpp`, `X2Lib/X2PremiumBuffManager.cpp` | -- |
| `STATIC_AUTO_LOGIN` | import-2014, **retired 2026-09-09** | -- | -- | -- |
| `SERV_IRUHADEV_QUICK_SLOT_FULL_FREE` | 2026-08-27 | `KTDXLIB/Always.h:2435`, `KncWX2Server/Common/ServerDefine.h:4224` | `X2Lib/X2Unit.h:982-988`, `X2Lib/X2Unit.cpp:104-111`, `X2Lib/X2UIQuickSlot.cpp:1797-1822`, `X2Lib/X2CashShop.cpp:8306-8326` | `KncWX2Server/GameServer/Inventory.cpp:500-534`, `KncWX2Server/GameServer/Inventory.cpp:146-152` |
| `SERV_IRUHADEV_OFFLINE` | 2026-08-31 | `KTDXLIB/Always.h:2446` **and** `X2ServerProtocol/X2ServerProtocol_2010.vcxproj:1444` (`US_SERVICE` `PreprocessorDefinitions`); AI party tuning constants in `X2Lib/X2Define.h:1822` | `X2Lib/Offline/` (the whole directory: 55 sources plus `start_offline.bat`), plus seams in `X2ServerProtocol/Socket/Session.cpp` (5 blocks), `X2ServerProtocol/OfflineHook.h`, `X2Lib/X2Data.cpp:2135`, `X2Lib/X2StateServerSelect.cpp:6982`, `X2Lib/X2DungeonSubStage.cpp:1452`, `X2Lib/X2QuestManager.{h,cpp}`, `X2Lib/X2TitleManager.{h,cpp}`, `X2Lib/X2StateBeginning.cpp:1607`; and, for the AI party, `X2Lib/X2Game.h:611`, `X2Lib/X2Game.cpp:195`, `:6541`, `:6986`, `:8886`, `:8958`, `:13802`, `X2Lib/X2DungeonGame.cpp:182`, `:1304`, `:2104`, `X2Lib/X2Room.h:304`, `:332`, `X2Lib/X2Room.cpp:1087`, `X2Lib/X2GageManager.cpp:3623`, `X2Lib/X2GageUI.h:633`, `X2Lib/X2Data.cpp:2930` | -- |
| `SERV_IRUHADEV_NO_PATCHER_TOKEN` | 2026-09-04 | `KTDXLIB/Always.h:2525` | `X2/X2.cpp:805` | -- |
| `SERV_IRUHADEV_JOBCHANGE_PORTRAIT` | 2026-09-04 | `KTDXLIB/Always.h:2513` | `X2Lib/X2UIQuestNew.cpp:8`, `X2Lib/X2UIQuestNew.cpp:1516`, `X2Lib/X2UIQuestNew.cpp:2043` | -- |
| `SERV_IRUHADEV_MP_REGEN_BOOST` | 2026-09-04 | `KTDXLIB/Always.h:2538` (rate constant in `X2Lib/X2Define.h:1802`) | `X2Lib/X2GUUser.cpp:1829`, `X2Lib/X2GUUser.cpp:3654`, `X2Lib/X2GUUser.cpp:3726`, `X2Lib/X2GageManager.cpp:50` | -- |
| `SERV_IRUHADEV_AIPARTY_PERSIST` | 2026-09-06 | `KTDXLIB/Always.h:2560` (nested under `SERV_IRUHADEV_OFFLINE`) | `X2Lib/X2Game.h:660-675`, `X2Lib/X2Game.cpp:197-199`, `X2Lib/X2Game.cpp:4958-4999`, `X2Lib/X2Game.cpp:7234` (`GetOfflinePartyBotPos`), `X2Lib/X2Game.cpp:7306` (`IsOfflinePartyBotUID`), `X2Lib/X2Game.cpp:7347` (`RepositionOfflinePartyBots`), `X2Lib/X2DungeonGame.cpp:685-707`, `X2Lib/X2DungeonGame.cpp:878-885` | -- |
| `SERV_IRUHADEV_LEVEL_CAP_80` | 2026-09-08 | `KTDXLIB/Always.h:2638` (redefines the studio's `USE_MAXLEVEL_LIMIT_VAL` from `KTDXLIB/OnlyGlobal/Always_US.h:75`) | `X2Lib/X2Game.h:34` and `X2Lib/X2UIPersonalShopBoard.h:12` pick it up by macro expansion; compile-time check in `X2Lib/Offline/Handlers_Room.cpp:2711` | -- |
| `SERV_IRUHADEV_OFFLINE_FETCH_AURA_ALWAYS` | 2026-09-08 | `KTDXLIB/Always.h:2672` (nested under `SERV_IRUHADEV_OFFLINE`) | `X2Lib/Offline/Handlers_Social.cpp:1541` (the force, in `MakePetInfo`), `:1574` (`IsPetPastCrystalStage`), `:1620` (the per-pet log line), `X2Lib/Offline/Handlers_Inventory.cpp:872` (item 500720 refused), `X2Lib/Offline/X2OfflineServer.h:490` | -- |
| `SERV_IRUHADEV_FIX_CHAR_SELECT_DELETE_BUTTON` | 2026-09-11 | `KTDXLIB/Always.h:3684` | `X2Lib/X2StateServerSelect.cpp:11097-11103` (`CreateUnitButtonNew`) | -- |
| `SERV_IRUHADEV_PENDING_DELETE_UNIT_MENU` | 2026-09-11, amended 2026-09-16 (twice) | `KTDXLIB/Always.h:3728` (nested under `SERV_UNIT_WAIT_DELETE`) | `X2Lib/X2StateServerSelect.cpp:1407` (down-state reset), `:2210`, `:3203`, `:3319`, `:3434` (immediate confirm on select), `:11196`, `:11234`, `:11303`; the first 2026-09-16 amendment also touches `X2Lib/Offline/X2OfflineDB.h:401-419,512`, `X2Lib/Offline/X2OfflineDB.cpp:1119-1145,1272-1285`, `X2Lib/Offline/Handlers_Unit.cpp:420-425,552-556` | -- |
| `SERV_IRUHADEV_RANOX_FIRE_GROUND` | 2026-09-19 | `KTDXLIB/Always.h:4016`; damage-rate constant in `X2Lib/X2Define.h` | `KTDXLIB/KTDGLineMap.h` (`LINE_TYPE.LT_FIRE_GROUND` member), `X2Lib/X2GUUser.h` and `X2Lib/X2GUUser.cpp` (`PhysicProcess_FireGround`, called from `PhysicProcess()`) | `KncWX2Server/Common/LineMap/XSLLineMap.h` (inert -- flag not reachable from the server build, kept in sync only) |
| `SERV_IRUHADEV_RANOX_RIM_LIGHT` | 2026-09-19 | `KTDXLIB/Always.h:4032` | `KTDXLIB/KTDGXRenderer.h`/`.cpp` (`CARTOON_TEX_TYPE.CTT_RIM_LIGHT_BLUE` member, `CKTDGXRenderer::SetRimLightTexType` and its 3 `BeginRender` substitution sites), `X2Lib/X2World.h`/`.cpp` (`CX2World::SetRimLightTextureIndex`, forwards to the renderer; destructor resets the override), `X2Lib/X2Main.cpp` (lua_tinker binding) | -- |
| `SERV_IRUHADEV_JUNE_EFFECTSET_PARSE` | 2026-09-25, extended 2026-09-26 | `KTDXLIB/Always.h:4287` | `X2Lib/X2EffectSet.h:168` (new `AnimTimeOnStartData`/`MeshTriggerData`), `:307,381,452` (`EffectData` members, `operator==`, ctor), `:484,505,555,592` (`EffectSetData` members incl. `m_vecSoundData`, `operator==`, `operator=`, ctor); `X2Lib/X2EffectSet.cpp:2267` (templet keys), `:2352,2378,2394,2405,2437,2476,2521` (per-`EFFECT%d` keys and sub-tables), `:2842` (`SOUND%d`) | -- |
| `SERV_IRUHADEV_JUNE_FX_DEBUG` | 2026-09-25, extended 2026-09-26 | `KTDXLIB/Always.h:4298` | `X2Lib/X2EffectSet.cpp:2199,2218` (`OpenScriptFile` load summary), `:2314` (per-templet line), `:2667` (per-effect line), `:2864` (per-set sound line); the last three nested under `SERV_IRUHADEV_JUNE_EFFECTSET_PARSE`; `X2Lib/X2DamageEffect.cpp:1942` (`CE_TEMPLET` per-`CEffect` line, nested under `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE`) | -- |
| `SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME` | 2026-09-26 | `KTDXLIB/Always.h:4318` | `X2Lib/X2EffectSet.h:634` (`EffectInstance::m_vecMeshTriggerData`, stage-3 `m_bGroupChild` / `m_wstrGroupChildBoneName` / `m_vGroupChildOffset`), `:659` (their initialisers), `:760` (`TargetMeshData`, `m_vecTargetMeshData`, `m_bIsRightWhenCreate`), `:856` (index accessors, the two bone-name helpers); `X2Lib/X2EffectSet.cpp:5` (`JuneFxEsCreate`, nested under `SERV_IRUHADEV_JUNE_FX_DEBUG`), `:36` (stage-2 helpers: `TRACE_MATRIX_IN_FIELD`, `SET_OVER_UI`; stage-3 `JuneFxUnsupported` `:225`, `JuneGroupChildPosition` `:249`), `:534` (`GROUP_NAME` loop head; the three stock position / rotation / scale lines are its `#else`), `:701` (damage case: `APPLY_DAMAGE_EFFECT_VELOCITY`, `USE_IS_RIGHT_WHEN_CREATE`), `:805` / `:864` / `:924` / `:974` (mesh cases), `:1027`-`:1256` (particle cases, two sites each), `:1276` (per-effect effect-alpha log), `:1308` / `:1447` (target-mesh registration, damage / mesh), `:1410` (trigger-list copy), `:1551` (group-child fields), `:1566` (loop tail), `:2372` (`OVERLAP`), `:2430` (templet effect-alpha log), `:2479` (`APPLIED_SKILL_ID` log), `:3791` (facing snapshot in the `EffectSetInstance` ctor), `:3855` (`GetEffectPosition` index), `:4114` (`NOT_ALLOW_MEMO` log), `:4128` (`GetEffectRotation`), `:4728` (`SetEffectSetOffset`), `:5175` (accessor bodies; stage-3 `GetEffectPositionByTargetMesh( .., bone, found )` `:5313`, `GetEffectPositionByBoneName` `:5362`); `KTDXLIB/KTDGXMeshPlayer.h:1325` (velocity-modifier methods), `:1575` (`m_vecVelocityModifier`, last member); `KTDXLIB/KTDGXMeshPlayer.cpp:1573` (their bodies), `:3360` / `:3385` (`CXMeshEvent_Velocity` re-apply) | -- |
| `SERV_IRUHADEV_EXTRA_NPC_STAT` | 2026-09-27 | `KTDXLIB/Always.h:4362` | `X2Lib/X2UnitManager.h`/`.cpp` (`EXTRA_NPC_STAT_TYPE` enum, `ExtraNPCStat`/`ExtraNPCFormulaVariable`, loader, `CalculateExtraStatValue`, `ApplyExtraNPCStatResistances`), `X2Lib/X2GUNPC.h`/`.cpp` (`SetHardLevel`, `ReInitAtkAndDef_LUA`), `X2Lib/X2Main.cpp` (Lua bindings), `X2Lib/X2BattleFieldGame.cpp` (monster-grade plumbing) | -- |
| `SERV_IRUHADEV_EXTRA_NPC_STAT_DEBUG` | 2026-09-27, **disabled 2026-09-27** (nested, short-lived) | `KTDXLIB/Always.h:4370` (`#define` commented out; re-enable to diagnose) | `X2Lib/X2GUNPC.cpp`/`.h`, `X2Lib/X2BattleFieldGame.cpp` (`[XSTAT]`/`[NPC_ZERO_HP]` diagnostics) | -- |
| `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY` | 2026-09-27 | `KTDXLIB/Always.h:4385` (nested under `SERV_IRUHADEV_EXTRA_NPC_STAT`) | `X2Lib/X2GUNPC.h`/`.cpp`, `X2Lib/X2Game.cpp` (`SetNpcHardLevel`), `X2Lib/X2BattleFieldGame.cpp`, `X2Lib/X2UnitManager.cpp` (`ApplyExtraNPCStatResistances`) | -- |
| `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE` | 2026-09-26, extended 2026-09-26 (Phase 6a) | `KTDXLIB/Always.h:4347` | `X2Lib/X2DamageEffect.h:611` (new `DamageEffectDataInLua::m_vecDamageDataNextChain`), `:1131` (new `CEffect` members); `X2Lib/X2DamageEffect.cpp:1457` (`APPLIED_SKILL_ID`/`_NOT_AFFECT_MEMO` + master ultimate flag), `:1504` (`MAIN_EFFECT_NAME_ULTIMATE`), `:1572` (`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`), `:1587` (`USE_ADVANCE_COLLISIOIN_CHECK`), `:1593` (`DAMAGE_DATA`/`_ULTIMATE` wiring), `:1609` (`IGNORE_MI_HUGE`), `:1680` (`USE_OWNER_ATTACK_SPEED`), `:1689` (`DAMAGE_DATA_CHANGE_TIME(_GLOBAL_TIME)` array reads), `:1745` (`FOLLOW_LINE_ULTIMATE`/`FOLLOW_LINE_NEW_ULTIMATE`), `:1776` (`FOLLOW_LINE_WITH_CUSTOM_ROTATE`/`FOLLOW_LINE_GRAVITY`/`TRACE_OWNER_BONE(_ONLY_ROTATE)`), `:1829` (`DAMAGE_DATA_NEXT(_ULTIMATE)`/`DAMAGE_DATA_NEXT%d` chain), `:1901` (`INHERIT_CRITICAL_RATE`/`_DAMAGE_RATE`), `:1934` (`RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`), `:2160` (destructor cleanup of the chain vector). Phase 6a additions: `X2Lib/X2DamageEffect.h:98` (new `DAMAGE_EFFECT_DIE_CONDITION_TYPE` enum), `:302` (new `DamageEffectData` fields: `fLoopInterval`, `uiLoopEndCount`, `fLoopEndTime`, `vLoopAddPos`, `bSetDieWhenMasterEffectDie`, `bInheritLifeTime`, `bInheritDamage`, `bInheritLockOnData`, `vScale`), `:448` (new `DieCheckCondition` struct), `:970` (`LoadDieCheckConditionBlock`/`LoadDieCheckCondition` decls), `:1252` (`m_vecDieCheckCondition`); `X2Lib/X2DamageEffect.cpp:1805` (ctor call to `LoadDieCheckCondition`), `:6782` (`LoadDieCheckConditionBlock`: shared `CONDITION_` loop for `AND_OPERATION`/`OR_OPERATION`), `:6837` (`LoadDieCheckCondition`: opens `DAMAGE_EFFECT_DIE_CHECK_CONDITION`), `:7040` (`LoadDieDamageEffect`'s `DIE_DAMAGE_EFFECT_ULTIMATE%d` gate+loop, forced off), `:7078` (`LoadDieDamageEffect`'s new keys), `:7366` (`LoadDamageEffect`'s `LOOP_*`/`SET_DIE_WHEN_MASTER_EFFECT_DIE`/`INHERIT_LIFE_TIME`/`INHERIT_DAMAGE`), `:7472` (`LoadHitDamageEffect`'s `HIT_DAMAGE_EFFECT_ULTIMATE%d` gate+loop, forced off), `:7562` (`LoadHitDamageEffect`'s new keys) | -- |
| `SERV_IRUHADEV_JUNE_DAMAGEDATA` | 2026-09-29 (Phase 8a) | `KTDXLIB/Always.h:4439` | `X2Lib/X2DamageManager.h:582` (new `ExtraDamageData::m_bManaDamageWithNoDisplay`), `:662` (its `Init()` reset), `:735` (its copy); `X2Lib/X2DamageManager.cpp:6` (`JUNE_EDT_MANA_DAMAGE_WITH_NO_DISPLAY` = 67, log include), `:169` (the `EXTRA_DAMAGE_TYPE` remap in `SetDamageDataFromLUA`). Phase 8c additions: `X2Lib/X2DamageManager.h:1121` (eight absent-system `DamageData` members), `:1336` (their `SimpleInit()` defaults), `:1537` (`SetJuneDamageDataTemplet` + `JuneDamageDataTempletScope`), `:1567` (`m_pwszJuneDamageDataTemplet`); `X2Lib/X2DamageManager.cpp:15` (log include, `JuneDamageDataUnsupported`), `:67` (ctor init), `:201` (the eight reads and their rule-5 lines); `X2Lib/X2DamageEffect.cpp:1746` (the CEffect ctor's templet scope) | -- |

What each one does:

- **`SERV_IRUHADEV_SKILLTREE_NO_LOCK`** -- both skills in a 2-choice skill tree
  row can be learned; the unchosen one is no longer locked out. Also hides the
  "AbleChoice" select-indicator picture on the left-hand skill slot, since
  that overlay implied an either/or choice that no longer exists.
- **`SERV_IRUHADEV_SKILL_SLOT_B_FREE`** -- Skill Slot B is open by default and
  permanent. No medal purchase, and the server keeps the permanent-sentinel end
  date rather than expiring it.
- **`SERV_IRUHADEV_BUFF_DURATION_TEXT`** -- draws a remaining-seconds countdown
  over each buff/debuff icon on the status HUD.
- **`STATIC_AUTO_LOGIN`** -- retired during the March 2014 migration (Phase 1,
  2026-09-09). It used to skip the login screen and sign in with a fixed
  account. The studio's own `AUTO_LOGIN_IN_HOUSE` went live under
  `US_INTERNAL` for the first time in that migration and does the same job
  data-driven, by reading a `LoginKey.lua` next to the exe - see *Known
  deviations to clean up* below.
- **`SERV_IRUHADEV_QUICK_SLOT_FULL_FREE`** -- all 6 equipped consumable quick
  slots are open from character creation. No cash ticket, no `LOCK` overlay,
  and the Quick Slot Expansion ticket (item 244560, plus the Ara/Elesis
  variants) is hidden from the cash shop. The server pins inventory category
  11 (`ST_E_QUICK_SLOT`) to exactly 6 slots on load, so a legacy `+3` purchase
  row already sitting in `dbo.GItemInventorySize` no longer stacks on top of
  the new base.
- **`SERV_IRUHADEV_OFFLINE`** -- makes the client playable with no servers
  running. `KSession` hands every outbound packet to an in-process emulator
  (`CX2OfflineServer`) instead of a socket, and the emulator answers on the
  same session at the point the real receive path queues into, so everything
  above the socket layer runs unmodified. All player state persists to a local
  SQLite file, `els_db.sql`, in the game directory. Client-only: no packet
  struct, event ID or shared enum is touched, so the servers do not need
  rebuilding. See `OFFLINE_MODE_PLAN.md` for the design and the phase history,
  and the *Offline mode* section of `CLAUDE.md` for the three seams.

  **It also fills a dungeon party with AI characters, and only through the
  auto-party button.** Pressing auto-party queues, matches and opens the
  dungeon with three of the game's named hero NPCs on the player's team -
  drawn at random, placed on the line map's own party start slots, fighting
  with ally AI, revived when they go down, kept across a stage change, and
  given HP/MP bars, portraits, names and levels in the real party HUD. The
  ordinary start button is untouched and still goes in alone: the entire
  scope guard is that a solo room sends no bot slots, so every client-side
  branch is a no-op on that path. An auto-party run pays exactly the same EXP
  as a solo one. The party members are `CX2GUNPC`s using the PvP-bot slot
  mechanism the studio already shipped, so a bot's moveset is its own scripted
  one rather than a player's skill tree, and no P2P peer is added for it.
  Their difficulty is three named constants in `X2Lib/X2Define.h`, applied in
  `CX2Game::SetUserSummonedNPCInfo`. See `AI_PARTY_PLAN.md` for the design and
  the six phases.

  **This flag has to be defined in two places.** `X2ServerProtocol` does not
  include `KTDX.h`, so `KTDXLIB/Always.h` alone does not reach the socket seam:
  it must *also* be in `X2ServerProtocol_2010.vcxproj`'s `US_SERVICE`
  `PreprocessorDefinitions`. Set only one and nothing fails to compile --
  `g_pX2OfflineHook` is declared in a header whose flag-conditional body the
  two projects then disagree about, and the client goes back to trying to
  reach a real server. Toggle both together, always.
- **`SERV_IRUHADEV_NO_PATCHER_TOKEN`** -- lets `x2.exe` be started directly
  instead of only through a launcher that passes the patcher token. A
  `_SERVICE_` build compares `argv[1]` against `PATCHER_RUN_ONLY` and returns
  0 out of `WinMain` if it does not match, so a bare launch (double-click, a
  debugger, a shortcut) exited instantly with no window, no message box and no
  log line. The flag supplies that same constant at the call site rather than
  reading it out of `argv`; the two tests are left standing and the token is
  still accepted when passed, so a launcher or `start_offline.bat` keeps
  working unchanged. Independent of offline mode, and useful without it.

  Verified by probe, not by reading `#ifdef`s: in `US_SERVICE` this is the
  **only** live `argv` read in the client. The other nine
  (`X2.cpp:756-778`, `X2Main.cpp:1083`, `1292-1301`, `1438-1460`, `1478`) are
  all behind flags that are off -- `LAUNCHER_COMMAND_ARGUMENT`,
  `CLOSE_ON_START_FOR_GAMEGUARD`, `CLIENT_PURPLE_MODULE*`,
  `SERV_CHANNELING_AERIA`, `SERV_COUNTRY_PH`, `_NEXON_KR_`, `ARGUMENT_LOGIN`,
  `SERV_STEAM`. Several of them read `__argv[1]`/`[2]` with no NULL guard, so
  that mattered.
- **`SERV_IRUHADEV_JOBCHANGE_PORTRAIT`** -- refreshes the top-left gage
  portrait when a quest changes the character's class. The quest path resets
  the 3D square unit but never the gage, so the HUD kept drawing the old class
  until the gage was rebuilt on a state change; every other class-change path
  in the client already refreshes it.
- **`SERV_IRUHADEV_AIPARTY_PERSIST`** -- the offline AI party survives a stage
  change instead of being rebuilt at every one of them. Requires
  `SERV_IRUHADEV_OFFLINE` and is defined under it.

  `CX2DungeonGame::StageLoading` calls `DeleteAllNPCUnit()`, which used to
  sweep the three bots away with the stage's monsters; `CreateOfflinePartyBots`
  then had to spawn all three again at `SubStageStart`, which costs a packet
  round trip plus three `CX2GUNPC` constructions and lands after the loading
  curtain has already lifted -- so the player fought alone for a moment at
  every stage and then watched three heroes pop in.

  A real multiplayer party never pays that: `CX2GUUser` units are built once in
  `CX2Game::UnitLoading` and `StageLoading` merely repositions them. This flag
  gives a bot the same lifetime. `DeleteAllNPCUnit` spares a *living* party bot
  while `m_bOfflineKeepPartyBots` is set (only around that one call), the way it
  already spares monster-card summons, and `RepositionOfflinePartyBots` places
  the survivors on the new stage's line map beside the user-unit loop. A dead
  bot is deliberately not spared -- letting it go means the ordinary spawn path
  rebuilds it whole on the new stage, which is a free revive while the stage is
  loading anyway.

  Reverting it restores per-stage respawning, which still works; the placement
  helper `GetOfflinePartyBotPos` is shared by both paths and is not gated.

- **`SERV_IRUHADEV_MP_REGEN_BOOST`** -- quality of life: raises the base MP
  regeneration rate for player units from 1 MP/s to
  `SERV_IRUHADEV_BASE_MP_REGEN_PER_SEC` (50 MP/s), the single tuning knob, in
  `X2Lib/X2Define.h`.

  The base rate is not a constant in the shipped client -- it is
  `MP_CHANGE_RATE` out of each class's Lua, read into
  `CX2GUUser::m_fOriginalMPChangeRate` at both of the two places that load it
  (`InitAndInsertToGame` and `InitComponent`) and then pushed into the gage by
  `ResetMPChangeRate()`. The flag raises the value at those two load points
  only, as a **floor** rather than an assignment, which is what keeps it from
  breaking anything downstream:

  - everything that composes on top of the base still composes -- Aisha's
    Meditation, Rena's charge states, Eve's passives, socket
    `m_fSpeedUpManaGather`, title and drag-and-set mana-recovery options all
    read `GetOriginalMPChangeRate()` and add to or scale it;
  - the PvP play-channel scale (x7 in `ResetMPChangeRate`, /7 in
    `GetNowMPChange`) is unaffected, since it wraps the base either way;
  - a class whose Lua already asks for more than the floor keeps its own
    higher value.

  Two things it deliberately does **not** touch. The socket-item MP option
  (`SetMPChangeRateValue`, `KSocketData::m_iMPChangeValue`) is a separate
  additive path and is left at its item-driven value. And the two absolute
  `ResetMPChangeRate( 3.f )` / `( 10.f )` calls behind Eve's `SMI_EVE_MEMO5`
  skill note (`X2Lib/X2GuEve.cpp:8665`) overwrite the base outright while that
  memo is equipped, so Eve regenerates *slower* than the floor in that one
  state. Both are class/item behaviour rather than the general base rate, which
  is what this flag is scoped to.

  NPC and monster MP is untouched -- `CX2GUUser` is the player-unit class. In
  online play MP is client-simulated per unit and P2P-synced, so a remote
  player running an unmodded client will regenerate at their own rate.

  **The village map needed a second site.** `XS_VILLAGE_MAP` does not regen MP
  through the unit's change rate at all. `CX2GageManager::OnFrameMove` runs a
  fixed 1.0 s timer (`m_ElapsedTimeCheckVillageBuff`) that calls
  `CX2GageSet::UpNowHpAndMpInVillage()`, which does a hardcoded
  `UpNowMp( 1.f )` straight onto the HUD's own `CX2GageData` -- the my-gage-set
  has no game unit attached in town, so `UpdateGageDataFromGameUnit()` degrades
  to a self-copy and the unit's rate never reaches the bar. That is the only
  hardcoded periodic MP tick in the client (every other literal `UpNowMp` is
  hit- or skill-driven), and it is why the first cut of this flag looked like
  it had done nothing while idle in a village but clearly worked in a dungeon.
  The tick is now worth one second of the base rate; since the period is
  exactly 1.0 s the constant goes in unscaled.

  **And dungeons and fields needed a third site, which is the one that
  mattered.** Raising the base rate had no effect there at all, and the reason
  is a stock bug: `CX2GUUser::InitComponent` sets the rate from the class Lua
  near the top, then further down -- in the `default` branch of the game-type
  switch, i.e. everything that is not PvP and not the training room -- does

  ```cpp
  CX2GageManager::GetInstance()->RestoreGageData();
  m_pGageData = CX2GageManager::GetInstance()->GetMyGageData()->GetCloneGageData();
  ResetMaxHP();
  ResetMaxMP();
  SetNowHp( ... ); SetNowMp( ... );
  ```

  The clone replaces the **whole** `CX2GageData`, so the MP change rate set
  minutes earlier in the same function is thrown away. The four calls after it
  restore the maxima and the current values; nothing restores the rate, and the
  clone carries the gage manager's, which no code ever sets -- it is the `1.0f`
  default out of `CX2GageData::Gage::Init()`. So in stock code a class's
  `MP_CHANGE_RATE` is dead in dungeons and fields, and every character
  regenerates at exactly 1 MP/s there regardless of what its Lua asks for. It
  survives only in the PvP and training branches, which do not re-clone.

  The flag re-applies `ResetMPChangeRate( GetOriginalMPChangeRate() )`
  immediately after that `ResetMaxMP()`. With the flag off the line is gone and
  stock behaviour is byte-for-byte unchanged, bug included.

  **This was found by measuring, not by reading.** Three rounds of static
  analysis got it wrong, because every writer of the rate looked correct in
  isolation -- the value was right when set and the thing that destroyed it
  does not mention MP at all. A temporary `SERV_IRUHADEV_MP_REGEN_DEBUG` flag
  logging one `[MPDBG]` line per second into `offline_server.log` settled it in
  one play-test: `InitComponent` logged `rateMp=20.000`, every per-frame sample
  a second later logged `rateMp=1.000` with `orig=20.000` still intact, which
  localises the loss to the gage object rather than the value. That asymmetry
  -- member right, gage wrong -- is also what made the class states look like
  they were working while idle regen was not: Aisha and Rena recompute from
  `GetOriginalMPChangeRate()`, which reads the member the clone never touched.
  See the *Deploying the offline client* section of `CLAUDE.md` for the
  diagnostic pattern.
- **`SERV_IRUHADEV_RANOX_FIRE_GROUND`** -- standing on a `LINE_TYPE.LT_FIRE_GROUND`
  line (new member, `KTDXLIB/KTDGLineMap.h`) deals
  `SERV_IRUHADEV_RANOX_FIRE_GROUND_DAMAGE_PER_SEC` (500/s, `X2Lib/X2Define.h`)
  per second to a player unit, floored at 1 HP so it can't kill by itself --
  the same shape as the existing `m_Fire` burn-debuff tick, but line-triggered
  in `CX2GUUser::PhysicProcess_FireGround` rather than buff-timer-triggered,
  and deliberately not routed through the `ExtraDamageData`/buff system so it
  can't collide with a real Fire-attack debuff on the same unit. Built for
  the Ranox port's PVP GateWay map (`RANOX_PORT_PLAN.md` Phase 15); the flag
  has no effect until the corresponding `LINE_TYPE.LT_FIRE_GROUND` value is
  packed into `data036.kom`'s `Enum.lua`.
- **`SERV_IRUHADEV_RANOX_RIM_LIGHT`** -- adds `CARTOON_TEX_TYPE.CTT_RIM_LIGHT_BLUE`
  (new member, `KTDXLIB/KTDGXRenderer.h`) and a renderer-wide override
  (`CKTDGXRenderer::SetRimLightTexType`) substituted for `CTT_NORMAL` at the
  engine's existing per-object cel-shading sites, so every cel-shaded unit on
  screen picks up a blue tint while a room-wide rim-light is active, without
  overriding any unit's own damage/invincible/petrify tint. Exposed to Lua as
  `g_pWorld:SetRimLightTextureIndex(...)`, which Ranox's Secret Hell boss
  stage calls (`RANOX_PORT_PLAN.md` Phase 15). Currently reuses the
  already-shipped `CartoonSkyBlue.bmp` texture, so the tint looks identical
  to the existing `CTT_SKYBLUE` skill-state tint until a dedicated asset is
  authored.

### Offline fetch aura on every pet

**Defined in** `KTDXLIB/Always.h` -- `SERV_IRUHADEV_OFFLINE_FETCH_AURA_ALWAYS`,
nested under `SERV_IRUHADEV_OFFLINE`. `QUALITY_OF_LIFE.md` #7.

Every pet that is past its crystal reports the fetch aura (the pet item-pickup
skill, `PET_DROP_ITEM_PICKUP`) as already unlocked, so cash item 500720 never
has to be bought. The force is one line in `CX2OfflineServer`'s `MakePetInfo`,
which is the only builder of a `KPetInfo` the offline server ever sends -- the
pet list, the create ACK, the summon ACK and its relayed `_NOT`, and the feed
ACK all pass through it, and all eight client sites that read `m_bAutoLooting`
read one of those.

**The save is not touched.** `unit_pet.auto_looting` keeps whatever was actually
purchased, so undefining the flag restores the bought toggle exactly, with no
migration. Item 500720 is refused while the flag is on
(`Handlers_Inventory.cpp:872`, `ERR_PET_28`, item left in the bag) rather than
consumed to write a column nothing reads any more; it stays listed in the cash
shop, because `X2OfflineCashSeed.h` is a verbatim transcription of
`dbo.EB_Product` and a refusal that names its reason is more readable than a
missing product.

"Except the pet still in crystal" is the game's own test, not a guess:
`PetTemplet.lua`'s `PET_STATUS` is `0` for a crystal step (the loader's own
comment at `X2Lib/X2PetManager.cpp:1337` reads *0: egg, 1: juvenile, 2: other
(adult, perfect form)*), the pet window disables the aura button on the same
entry (`X2UIPetInfo.cpp:2019-2021`), and the live server's `ERR_PET_27` gate
means the same thing. `CX2OfflineServer::IsPetPastCrystalStage` asks
`CX2PetManager::GetPetStatus`, which already folds "no templet" and "step out of
range" into `0`. A pet born fully grown carries `PET_STATUS = { 3 }` and so is
covered, which is the live server's `IsEvolutionExceptionPet` case for free.

To include crystal-stage pets as well, drop the `IsPetPastCrystalStage` test in
`MakePetInfo` -- the pickup itself would work (`X2GUUser.cpp:2874` is not gated
on pet status), but the pet window's aura button stays greyed out for them, so
the UI and the behaviour would disagree.

### Offline "any difficulty" quest and title steps

**Gated by** `SERV_IRUHADEV_OFFLINE`; no new flag. `ISSUES_2.md` #2, phase 30.

Six one-line reads added to two client script parsers, so `m_bUpperDifficulty`
-- the only "any difficulty" mechanism this build has -- is actually loaded:

| file | clear type | what it fixes |
|---|---|---|
| `X2Lib/X2QuestManager.cpp:2453` | `SQT_VISIT_DUNGEON` (26) | 88 of 121 sub-quests |
| `X2Lib/X2QuestManager.cpp:2493` | `SQT_FIND_NPC` (27) | 39 of 47 sub-quests |
| `X2Lib/X2QuestManager.cpp:2574` | `SQT_ITEM_USE` (22) | neither of its 2 sets the flag; parity only |
| `X2Lib/X2TitleManager.cpp:587` | `TMCT_DUNGEON_TIME` | 5 of 17 sub-missions |
| `X2Lib/X2TitleManager.cpp:611` | `TMCT_DUNGEON_RANK` | 9 of 32 sub-missions |
| `X2Lib/X2TitleManager.cpp:634` | `TMCT_DUNGEON_DAMAGE` | none of its 11 sets it; parity only |

`REFORM_QUEST` is on, so `ClearCondition` has no difficulty field and no "any"
sentinel: difficulty is the last digit of the dungeon ID, and "any difficulty"
means `m_bUpperDifficulty == true` plus the Normal ID in `m_setDungeonID`, read
as "this dungeon at difficulty >= that digit". `CX2OfflineQuest::IsExistDungeonInSub`
already honoured the flag correctly -- nothing ever set it, because the client
never needed it. **Offline, the client's parse *is* the server's**, and the
server's `CXSLQuestManager` / `CXSLTitleManager` read the key at exactly these
six places. Each addition matches the server's line and its placement, which
brings the client to the server's own count exactly: 11 reads in the quest
parser and 6 in the title parser, where it had 8 and 3.

Because sub-quest groups are staged (`CheckBeforeGroup`), one stuck group-0
`VISIT_DUNGEON` step also blocks the later `DUNGEON_CLEAR_COUNT` step in the
same quest -- which is why a whole quest read as Normal-only even though its
clear-count branch parsed the flag correctly.

**`LUA_GET_VALUE_RETURN` hard-fails on a missing key** (`luaLib/KLuaManager.h:143`,
`goto error_proc`), which would drop the templet and, for the quest parser, take
every quest with it. The key's presence was therefore proved rather than
assumed, against the exact script the client loads: `FieldSubQuest.lua` and
`SubTitleMission.lua` were XOR-decrypted out of `data036/` and their Lua 5.1
bytecode disassembled and register-simulated back into the 1,672 sub-quest and
640 sub-mission tables they build. All 121 + 47 + 2 records of the three quest
types carry `m_bUpperDifficulty` inside `m_ClearCondition`; the types that do
*not* carry it are exactly the ones whose parser branch does not read it, on
both sides. The title reads use the non-fatal `LUA_GET_VALUE`, as the server's
do. The disassembler was scratch, not committed; it is a Lua 5.1 chunk reader
plus a register simulator over `SETTABLE`/`SETLIST`, which is all it takes to
turn any of these bind-a-table scripts back into readable rows.

`TMCT_NPC_HUNT` looks like a seventh site -- 4 of its 177 sub-missions set the
flag -- but the **server does not read it there either**, so ignoring it is
shipped behaviour and was left alone.

Three diagnostics went in alongside, all inside `X2Lib/Offline/X2OfflineQuest.cpp`:

- `DungeonReqString()` renders a sub-quest's requirement as `30070 (upperDiff=1,
  ...)`, since neither the packed key nor the flag is readable without the other.
- The per-sub-quest census line (`QUEST CHAIN sub`) now prints it, and the
  aggregate line (`QUEST CENSUS clearType=`) now prints how many reachable steps
  carry `upperDiff=1`. A zero there for 26 or 27 means the read has regressed.
- The three dungeon-filter rejections that were a bare `continue`
  (`OnVisitDungeon`, `OnFindNPC`, `OnUseItem`) now log the requirement and the
  key the clear actually arrived with.

Baseline to compare against, from the run before the change: **1,395 templets,
1,395 visible**, clearType 26 = 205 steps, 27 = 47, and 22 absent entirely. A
templet count below 1,395 means a parse is now failing.

### Offline 3x EXP and 3x drop rate

**Defined in** `KTDXLIB/Always.h` -- `SERV_IRUHADEV_OFFLINE_EXP_BOOST` and
`SERV_IRUHADEV_OFFLINE_DROP_BOOST`, both nested under `SERV_IRUHADEV_OFFLINE`.
**Rates** live together at the tail of `X2Lib/X2Define.h`: `_EXP_RATE`,
`_ED_RATE`, `_QUEST_ITEM_RATE` (all `3.0f`) and `_DROP_DRAWS` /
`_STATIC_DROP_DRAWS` (both `3`). That is the one place to retune -- set a rate
to `1.0f` or a draw count to `1` to switch that half off without touching the
flags.

Quality of life for solo play. Offline has none of the live server's rate
bonuses: party, PC bang, premium, event and channel EXP are all either
display-only or not compiled in offline, and `X2OfflineDropTable.h:99` records
that the four multipliers the real server folds into a drop are hard-coded to
1.0 here. So the curve is the full retail one with nothing on top of it.

Covered: monster EXP and ED, dungeon-clear bonus EXP (for free -- it is 30% of
the run total), quest reward EXP and ED, monster and static/place item drops,
and quest collection items. Not covered, deliberately: cube/box contents
(`X2OfflineRandomItem`), and the display-only bonus fields (`m_iPartyEXP`,
`m_iSocketOptEXP`, `m_nPremiumBonusEXP`, ...) -- filling those would make the
EXP bar disagree with the database.

Two things here are not the obvious implementation, and both matter:

- **EXP is multiplied where the reward is *minted*, not where it is stored.**
  `Handlers_Room.cpp` scales `iEXP` immediately after the battlefield factor
  and before `m_kRoom.m_iRewardEXP` accumulates, because every figure the
  player sees and every figure that is stored derives from that one local: the
  `ApplyDungeonReward` write, the `m_EXPList` number the client adds to its own
  bar as `EGS_NPC_UNIT_DIE_NOT` arrives, the clear bonus, and the result
  screen's `m_nOldEXP` / `m_nEXP`. Multiplying inside `ApplyDungeonReward`
  instead -- the one-line change, and the tempting one -- would triple the
  stored total while still telling the client the unboosted number, and the
  result screen's bar can then animate **backwards**.

- **Drops repeat the draw; they do not scale the probability.**
  `CX2OfflineDropTable::Decide` is a weighted *single pick* over one
  accumulated list, not a per-item coin flip. Tripling `m_fProb` would not give
  3x: real rows already sum to ~82% (`DropTable.lua:1686`), so tripling pins
  the total at 100% -- about 1.2x actual -- and silently makes every case
  listed after the accumulator passes 100 unreachable. Repeating the draw is
  what the studio's own drop-rate event does
  (`KncWX2Server/CenterServer/KDropTable.cpp:1284`, under
  `SERV_ITEM_DROP_EVENT`), and it multiplies the expected item count exactly
  while leaving every rarity ratio alone. The **inner group draw stays at one**
  -- a group is a near-uniform selector (group 1 is eight rows of 12.5), not a
  rarity gate, so repeating it would saturate the group and pin every group win
  to its first item.

  The quest collection-item roll is the exception: that one *is* a true
  per-item roll, so it is scaled directly and clamped to 100.

The monster row and the static (place) row have separate draw counts because
they are different kinds of loot -- the monster row is where gear comes from,
the static row is where the ordinary consumables come from (Aqua has a static
row in nearly every dungeon at 5-10%). Drop `_STATIC_DROP_DRAWS` back to `1`
on its own if the potion clutter gets tiresome.

`offline_server.log` gets a `BOOST` line at startup naming all five values. It
is printed unconditionally -- with the vanilla numbers when the flags are off
-- because the question it answers is "did my rebuild actually take", and a
line that only appears when the boost is compiled in cannot tell a disabled
boost apart from a stale PCH.

Client-only; no server rebuild. Nothing under `KncWX2Server/Common/` is
touched, so the wire format is unchanged. Reverting is safe at any time: only
future gains are scaled, nothing migrates, and the character is untouched.


### Level cap 80

**Defined in** `KTDXLIB/Always.h` -- `SERV_IRUHADEV_LEVEL_CAP_80`. It does one
thing: `#undef`/`#define` the studio's own `USE_MAXLEVEL_LIMIT_VAL` macro, which
`KTDXLIB/OnlyGlobal/Always_US.h:75` sets to `const int g_iMaxLevel = 67;` for
US. The redefinition sits at the tail of `Always.h`, i.e. after the
`OnlyGlobal` include at line 2492 and before any `X2Lib` header is parsed, so
both places that expand the macro pick up 80: `X2Lib/X2Game.h:34`
(`_CONST_X2GAME_`) and `X2Lib/X2UIPersonalShopBoard.h:12`
(`_CONST_UIPERSONALSHOPBOARD_INFO_`). Two headers, one edit, and the studio's
line is left in place as the value the flag replaces.

**This is data the build already shipped**, which is why it is only a constant:

- `ExpTable.lua` (packed in `data036.kom`; decrypts to `luac` with the 12-byte
  XOR key) carries rows all the way to `LEVEL = 80`, `TOTAL_EXP = 986793900` --
  comfortably inside `int`.
- `KncWX2Server/ServerResource/US/StatTable.lua` does
  `ReserveMemory( class, 80 )` and has a `SetUnitStat` row at level 80 for every
  player class, and `CX2OfflineStatTable::MAX_LEVEL` was already 80 to match.
- `SkillData.lua`'s `CalcLevelUpIncreaseSkillPoint` is a formula
  (`level / 10 + 4`), not a table, so `CX2OfflineSkill::SkillPointForLevelUp`
  keeps paying out past 67 without any change.

Everything that reads the cap follows automatically -- the full list is
`Handlers_Room.cpp`'s `ApplyDungeonReward` (stops levelling at it),
`Handlers_Inventory.cpp`'s level-up scroll (refuses at it), `X2Game.cpp:8313`
(the EXP bar's max), `X2InstanceData.cpp:80` (the seed for `m_iMaxLevel`) and
`X2UIPersonalShopBoard.cpp`'s level filter. `X2Define.h`'s `LIMIT_MAX_LEVEL`
(65) is dead -- nothing but comments references it -- and is deliberately left
alone.

`Handlers_Room.cpp` carries a compile-time check next to `ApplyDungeonReward`:
a negative array bound if `_CONST_X2GAME_::g_iMaxLevel != 80` while the flag is
defined. That is aimed squarely at the stale-PCH failure mode -- `Always.h` is
inside every project's precompiled header, so a dropped edit would otherwise
show up as a cap that silently stayed at 67.

A **live** server would need more than this: `SiKGameSysVal()->GetLimitsLevel()`
reads `GameSysValTable.lua`'s `MAXLevel`, which is what the comment beside the
studio's `#define` is warning about. Offline has no `GameSysVal` and never sends
`EGS_UPDATE_MAX_LEVEL_NOT`, so `CX2InstanceData::m_iMaxLevel` just keeps its
`g_iMaxLevel` seed and there is nothing else to change.

Client-only; no server rebuild. Nothing under `KncWX2Server/Common/` is touched.
Reverting is safe for a character at or below 67; a character already past it
keeps its stored level and EXP, but the EXP bar clamps and further gains stop.

### Character deletion: restore-only pending-delete flow

**Defined in** `KTDXLIB/Always.h` -- `SERV_IRUHADEV_FIX_CHAR_SELECT_DELETE_BUTTON`
and `SERV_IRUHADEV_PENDING_DELETE_UNIT_MENU`, both introduced 2026-09-11 and
undocumented until now.

Vanilla US deletion is two-stage under the studio's own `SERV_UNIT_WAIT_DELETE`:
deleting a character reserves it rather than removing it -- it stays listed,
greyed, and recoverable -- and two dedicated buttons on the character-select
dialog, `restore_unit` and `final_delete_unit`, restore it or make the deletion
permanent after a wait period. **Neither button exists in this build's shipped
`DLG_UI_Character_Selection_Back_New.lua`** -- confirmed from the running
client's own `ErrorLog.txt`, which logs a failed `GetControl` for both names at
every character-list load. So on this build, true vanilla can reserve a
deletion but can reach neither restore nor final-delete from the UI at all; the
network flow the studio wired (`Handler_EGS_RESTORE_UNIT_REQ`,
`Handler_EGS_FINAL_DELETE_UNIT_REQ`, `X2StateServerSelect.cpp:10343-10385`) is
real and complete, just unreachable.

`SERV_IRUHADEV_FIX_CHAR_SELECT_DELETE_BUTTON` is an unrelated, pure bug fix:
`CreateUnitButtonNew()` never hid slot control index 5 (`ButtonDeleteUnit`,
`DLG_UI_Character_Selection_Slot_New.lua`'s 6th per-slot button), which
intercepted every click before the real select button, silently dropping it.

`SERV_IRUHADEV_PENDING_DELETE_UNIT_MENU` repurposes that now-hidden button as
a grey pending-delete overlay, and gives restore a UI entry point by way of the
slot click. **Its original 2026-09-11 version was wrong**: clicking a pending
slot opened an Ok/Cancel box whose *Cancel* button fired
`SUSUCM_FINAL_DELETE_UNIT`, permanently destroying the character with no
confirmation of its own. Amended 2026-09-16:

- the slot click now shows a single Ok/Cancel box worded as vanilla's own
  restore confirm (`STR_ID_16106`, the exact call vanilla already makes at
  `X2StateServerSelect.cpp:1687`) and sends `SUSUCM_RESTORE_UNIT_CHECK` on Ok;
  Cancel just closes it. Nothing destructive is reachable from this menu.
- there is deliberately **no final-delete UI** -- a pending character stays
  recoverable indefinitely, matching what true vanilla actually permits on
  this build. `SUSUCM_RESTORE_UNIT` and the two vanilla button call sites are
  left in place, unreachable, so either lights up for free if the shipped
  dialog ever gains the missing controls.
- `X2Lib/Offline/X2OfflineDB.cpp`'s `IsNickNameTaken` used to free a pending
  unit's nickname immediately on soft delete (`del_date = reg_date` filter),
  diverging from vanilla, which holds the name until final delete. With
  restore now the only exit from the pending state, that divergence was a real
  hazard -- delete "Bob", create a new "Bob", and the old "Bob" could never be
  restored again. It now takes an `iExcludeUnitUID` parameter so the name stays
  held while pending, except against the pending unit's own row (restore would
  otherwise refuse itself). One consequence of combining "hold the name" with
  "no final-delete UI": the name is held **permanently** once a character is
  deleted and never restored, since nothing releases it. Acceptable at solo-save
  scale; noted here so it isn't mistaken for a bug later.
- the wait period (`DELETE_WAIT_SECONDS`, `X2OfflineDB.h`) is 5 minutes rather
  than vanilla's hardcoded 1 day (`GSGameDBThread.cpp`'s `iDelableDay = 1` for
  every region). With no final-delete UI this only governs the date shown in
  `STR_ID_16103` and the ack field a final-delete packet would check if one
  ever arrived; it is not something a player waits out on a button.
- `CX2OfflineDB::FinalDeleteUnit`'s child-table cascade (`szTables`) was missing
  four tables added after the list was last extended (`unit_pet`,
  `unit_riding_pet`, `unit_skill_unsealed`, `unit_skill_note`). Fixed, though
  currently latent -- nothing in the UI reaches final delete.

**Corrected 2026-09-16, same day, from play-testing:** the first cut of the
single-confirm slot click above looked like it did nothing. Two separate
defects, both in `X2StateServerSelect.cpp`, both pre-dating this flag rather
than introduced by it:

- **The click only registers on a slot that is already selected.**
  `UnitButtonUp( CX2Unit* pUnit )` only reaches the `m_bDeleted` check inside
  its `if( m_pSelectUnit == pUnit )` branch -- the first click on a pending
  slot takes the *other* branch (a plain reselect: `m_pSelectUnit = pUnit`,
  `CreateServerSelectUnitViewerUI( pUnit )`, no dialog), so from the player's
  side a first click looked exactly like "nothing happened, just highlighted."
  A live character has the same two-click structure, but the first click's
  reselect is a real action there (it swaps which character's model is
  shown); for a pending character it wasn't, so the requirement was more
  visible. Fixed by raising the same restore confirm immediately in that
  reselect branch too, gated on `pUnit->AccessUnitData().m_bDeleted`, closing
  any dialog already open first (`SAFE_DELETE_DIALOG( m_pDLGDeleteUnitCheck )`)
  in case a different pending slot's confirm was still up. The original
  second-click path (`if( m_pSelectUnit == pUnit )`) is left in place and
  still fires if a player clicks an already-selected pending slot again --
  harmless, since re-raising the same confirm on a unit already confirmed
  pending is a no-op state-wise.
- **Multiple pending slots could show "pressed" at once**, unlike a live
  slot, where clicking a new one always released the last one. Root cause:
  `SUSUCM_UNIT_BUTTON_UP`'s `FIX_BUTTON_CRASH` reset loop (`Always.h`,
  studio code, unconditionally on) walks every slot and resets control index
  0's `SetDownStateAtNormal` -- but a pending slot's clickable control is
  index 5 (`ChangeUnitButtonInfo`, this flag), which the loop never touches.
  So a pending slot's pressed state, once set, was never cleared by clicking
  a different slot afterward. Fixed by resetting index 5 in the same loop,
  gated on this flag since index 5 is only a real interactive button under
  it.

**A third defect, found the same day after the above two shipped, is what had
actually been silently defeating both of them:** clicking a pending slot did
nothing at all -- no model swap, no dialog, restore and (the still-hypothetical)
delete both unreachable. Diagnosed with a temporary logging pass
(`SERV_IRUHADEV_PENDING_DELETE_DEBUG`, removed once this was confirmed fixed;
see the *Deploying the offline client* diagnostic pattern in `CLAUDE.md`), which
showed the clicked button's own name had silently become `SLOT_BUTTON_<uid>100`
-- three extra digits nobody wrote.

Root cause: `ChangeUnitButtonInfo` (`X2StateServerSelect.cpp`) names
`pSelectUnitButton` and `pDeleteUnitButton` **identically**,
`SLOT_BUTTON_<uid>`, since both represent the same slot and only one is ever
shown. It used `SetName` for both. `SetName` registers the name in the
dialog's `m_ControlsMap` for `GetControl(L"name")` lookups, and
`CKTDGUIDialog::ReNameControl` -- studio code -- silently disambiguates any
collision by appending a counter starting at **100**. Since
`pSelectUnitButton` claims the name first, `pDeleteUnitButton` collided and
was quietly renamed to `SLOT_BUTTON_<uid>100`. `SUSUCM_UNIT_BUTTON_UP`'s
`tempButtonName.substr(12)` then parsed a UID with that spurious suffix,
matching no real character, so `GetUnitByUID()` returned `NULL` and the
click silently did nothing -- on *every* pending slot, unconditionally,
regardless of the two fixes above.

Nothing looks `pDeleteUnitButton` up by name anywhere (only by index,
`GetControl(5)`), so the fix is `SetNameByForce` instead of `SetName` --
same string, but it only sets the control's own name and never touches the
map or its collision check. Not a workaround: `SetNameByForce` is the
correct call for a control nothing ever looks up by name, and it was
available already (`CKTDGUIControl::SetNameByForce`, used elsewhere in the
engine for exactly this reason).

Client-only; no server rebuild -- the offline emulator files above are part of
`SERV_IRUHADEV_OFFLINE`, not `KncWX2Server/Common/`. Reverting
`SERV_IRUHADEV_PENDING_DELETE_UNIT_MENU` returns to the studio's own dead-end OK
box (`STR_ID_30401`) on a pending slot; the character stays recoverable only by
undeleting through direct DB access, since there is still no in-game
final-delete path either way.

### Ranox monster effects: missing effect-set diagnostic + skin-mesh glow attach

Two independent fixes for the Ranox (Lanox) monster family's missing effects and
damage, investigated and planned in full in `RANOX_EFFECT_ENGINE_GAPS.md`
(the standing gap inventory) -- this entry covers only the two C++ flags; the
data-side fix (the 196 effect sets / 92 damage templets / 381 particle
sequences merged into `march2014/data036/EffectSet.src.lua`,
`DamageEffect.src.lua`, `GameMajorXMeshPlayer.txt` and `GameMajorParticle23.txt`)
is a pure content change with no flag.

**`SERV_IRUHADEV_RANOX_EFFECT_DEBUG`** (`KTDXLIB/Always.h`) -- a throttled
diagnostic in `CX2EffectSet::PlayEffectSet` (`X2Lib/X2EffectSet.cpp`), at the
existing `NULL == pEffectSetData` early return that used to fail completely
silently (no log, no assert -- the reason the Mag Charger's death effect and
its damage both simply vanished with no trace to grep for). Logs
`[RANOX_EFFECT_MISSING] EffectSet not found: <name>` to `offline_server.log`
via `CX2OfflineLog::Server`, once per name per second (`static
map<wstring,DWORD>` + `timeGetTime()`). Short-lived by design -- remove once a
play-test's log shows only the handful of effect sets `RANOX_EFFECT_ENGINE_GAPS.md`
already lists as unrecoverable in both clients.

**`SERV_IRUHADEV_RANOX_ATTACH_SKIN_MESH`** (`KTDXLIB/Always.h`,
`X2Lib/X2GUNPC.cpp`, `X2Lib/X2SubEquip.h`, `X2Lib/X2SubEquip.cpp`) -- the
`FIRE_DEVIL_*`/`SECRET_*` fighter and berserker family's Lua carries an
`ATTACH_SKIN_MESH0` block (a second, skinned whole-body shell -- the missing
glow overlay reported for Demon Fighter) that this build's NPC parser never
read at all; only the sibling `ATTACH_MESH` (rigid, bone-anchored) loop
existed. Added a second loop in `CX2GUNPC`'s constructor reading
`ATTACH_SKIN_MESH0`'s `ATTACH_MESH_NAME`/`SKIN_MESH_RENDERTYPE` and reusing
the studio's own pre-existing (unconditionally compiled, not part of this
mod) `ATTACH_SKINMESH` mechanism -- the same skinned-sub-equip machinery
`X2Eqip.cpp` already uses for player equipment -- anchored to the unit's root
frame (`GetCloneFrameRoot()`, since the block gives no bone name). When
`SKIN_MESH_RENDERTYPE == SKIN_MESH_RENDER_TYPE.SMRT_UV_ANIM`, `CX2SubEquip`
now scrolls `texOffsetStage2.y` per frame under `RT_LIGHT_FLOW_LOCAL_Z`
(`CX2SubEquip::SetUVAnimGlow`), the same UV-scroll idiom
`X2Eqip.cpp:412-419` uses for the enchanted-weapon glow, at the same fallback
speed (`1.0f`/sec, `X2Eqip.cpp:414`).

**Known, disclosed limitation, not a bug to chase further**: `CX2SubEquip` has
only one render pass, unlike `CX2Eqip`'s separate `m_EnchantRenderParam` for
its glow -- so the scrolling UV render type *replaces* the attached shell's
normal cel-shaded look rather than layering on top of it, as the enchant glow
does. If that reads wrong in play-test, the documented fallback is to drop the
`SetUVAnimGlow` call and leave the shell on the default `RT_REAL_COLOR` --
still attached and visible, just without the scroll. See
`RANOX_EFFECT_ENGINE_GAPS.md` §4 for the full inference chain, including the
unexplained `VALUE = 0.01` field this mod does not use.

Client-only; no server rebuild -- neither flag touches anything under
`KncWX2Server/Common/`. Reverting either flag independently is safe: the
effect-set fix is a pure logging addition (removing it returns to the old
silent-failure behavior, not a new one), and the skin-mesh fix drops back to
skipping `ATTACH_SKIN_MESH0` entirely, the pre-mod behavior.

### June2017 effect engine: EffectSet templet-level keys (Phase 2)

Phase 2 of `JUNE2017_EFFECT_REBUILD_PLAN.md`. Parse only; nothing reads the new
members yet (the runtime is Phase 4a/4b).

**`SERV_IRUHADEV_JUNE_EFFECTSET_PARSE`** (`KTDXLIB/Always.h`,
`X2Lib/X2EffectSet.h`, `X2Lib/X2EffectSet.cpp`). June2017's EffectSet parser
(`FUN_321e48e0`) reads ten keys at the templet level, before the `EFFECT%d`
loop at `321e5cf8`. March already read four of them, with the same getter and
default. This flag reads the other six into new `EffectSetData` members, with
June's getter types and defaults. Each read carries a comment citing its June
address range.

| Key | Type, default | Consumer |
|---|---|---|
| `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE` | bool, false | Phase 4a |
| `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE` | bool, false | Phase 4b |
| `OVERLAP` | bool, false | Phase 4a |
| `SYNC_SHOW_BY_CREATE_UNIT` | bool, false | Phase 4a |
| `APPLIED_SKILL_ID` | int, 0 | none: unsupported |
| `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` | int, 0 (June keeps only the low byte) | none: unsupported |

The two unsupported keys belong to systems this build lacks (plan rule 5).
They are stored with no behaviour, and a non-zero value logs
`JUNE_FX unsupported <KEY> in <set>: ...` once per templet to `ErrorLog.txt`.
The flag also adds the new members to the dev-script `operator==` and
`operator=`.

**`SERV_IRUHADEV_JUNE_FX_DEBUG`** (`KTDXLIB/Always.h`, `X2Lib/X2EffectSet.cpp`).
This is the rebuild's shared, short-lived diagnostic (plan rule 4).
- For each templet that sets a June key, it writes
  `JUNE_FX ES_TEMPLET <set> <KEY>=<v> ...`.
- After each script file, it writes one summary line,
  `JUNE_FX ES_LOAD <file> loaded=.. templets=.. with_june_templet_key=..`.

Both go to `ErrorLog.txt`, not `offline_server.log`. The effect sets load in
`CX2StateStartUp`, and the offline server only opens its log on first connect.

**Data note.** No set in the current `march2014/data036/EffectSet.src.lua` uses
any of the six keys. The Ranox merge brought none of them in. So the
per-templet line fires only once data carrying these keys is packed; until
then, the `ES_LOAD` line is the evidence that the block ran.

Client-only; no server rebuild. Undefining `SERV_IRUHADEV_JUNE_EFFECTSET_PARSE`
removes the reads and members. Nothing consumes them yet, so no behaviour
changes. Undefining `SERV_IRUHADEV_JUNE_FX_DEBUG` removes only log lines.

### June2017 effect engine: EffectSet per-effect keys and sub-tables (Phase 3)

Phase 3 of `JUNE2017_EFFECT_REBUILD_PLAN.md`. It extends the same two flags;
it is the same logical change, the rest of June's `FUN_321e48e0`
(`321e5cf8`–`321ee2a0`). The plan's Phase 3 section holds the per-key table:
VA range, getter, default and post-processing.

**`SERV_IRUHADEV_JUNE_EFFECTSET_PARSE`** now also reads:

- **Per `EFFECT%d`**, into new `EffectData` members:
  - `GROUP_NAME` (string, binary-only).
  - `ZOFFSET_USE_UNIT_IS_RIGHT`, `TRACE_MATRIX_EXCLUDE_SCALE`, `TRACE_MATRIX_IN_FIELD`,
    `USE_IS_RIGHT_WHEN_CREATE`, `FOLLOW_UNIT_ROTATION`, `FOLLOW_UNIT_ROTATION_CPARTICLE`,
    `FOLLOW_UNIT_VIEWER_SCALE`, `USE_EFFECT_SCALE_IN_UNIT_VIEWER`, `NOT_ALLOW_MEMO`: bools,
    all default false.
  - `BONE_TRACE_TARGET_MESH_INDEX`: int, default -1.
  - `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`: unsupported, like the templet-level one.
    Logged once per templet, naming the first effect that uses it.
- **Sub-tables gated on `EFFECT_TYPE`, as June does:**
  - `ET_DAMAGE_EFFECT` reads `APPLY_DAMAGE_EFFECT_VELOCITY`, a positional
    `{x, y, z, n}`. The floats default to 1. June keeps only `n == 0`.
  - The four `ET_MESH_PLAYER_*` types read `MESH_TRIGGER_DATA_LIST` → `TRIGGER_DATA%d` into a
    `std::vector<MeshTriggerData>`, then the single inline `MESH_TRIGGER_DATA`.
    - Trigger fields: time, motion, play type (default 2), `BOUND_MOTION_TYPE_LIST`,
      stop-tracing/once/block flags, block time, `TRIGGER_EFFECTSET_LIST` names, the
      toggle-visibility timer and its derived flag, `USE_MESH_TRANSITION` (default true
      for list entries), and three `ANIMTIME_ON_START_*` sub-tables.
- **`SOUND%d`** at templet level, after `CAMERA_SHAKE%d`, into
  `EffectSetData::m_vecSoundData`. June's entry is exactly March's
  `CX2DamageEffect::SoundData`, so that struct is reused: event time, file name,
  rate (default 100), use-global-time, loop.

**One read changes an existing member.** `TRACE_MATRIX_EXCLUDE_SCALE = TRUE` also
sets `m_bTraceMatrix`, because June's parser does (`321e760b`–`321e7610`).
`m_bTraceMatrix` is consumed by the stock runtime. This affects only sets that use the
new key, and no deployed set does. Everything else is parse only, with consumers in
Phase 4a/4b.

**`SERV_IRUHADEV_JUNE_FX_DEBUG`** adds three lines, all to `ErrorLog.txt`:

- `JUNE_FX ES_EFFECT <set> EFFECT<n> <KEY>=<v> ...`, for each effect that sets a new key.
- `JUNE_FX ES_SOUND <set> count=.. first=<file>@<time> rate=..`, for each set with
  `SOUND%d`.
- The `ES_LOAD` summary now also has `effects=.. with_june_effect_key=.. sounds=..`.

**Data note.** No deployed set uses any Phase-3 key. That was checked against
`march2014/data036/EffectSet.src.lua` and the deployed `UIEffectSet.lua`. So, as in
Phase 2, only the `ES_LOAD` counts can show the block ran. The per-effect line is
settled at Phase 11, with June's own data. Porting June's trigger sets early
would not work anyway: `BOUND_MOTION_TYPE_LIST` uses `EMOTION_TYPE.*`, which March's
`Enum.lua` lacks.

Revert as for Phase 2. Undefining the flag drops every Phase 2 and 3 read, and
with it the `TRACE_MATRIX_EXCLUDE_SCALE` → `TRACE_MATRIX` rule.


### June2017 effect engine: EffectSet creation-time runtime (Phase 4a)

Phase 4a of `JUNE2017_EFFECT_REBUILD_PLAN.md`, under the new flag
**`SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME`**. The keys Phases 2-3 parse now start to be
consumed. The behaviour is transcribed from June's `FUN_321ee600` (`OnFrameMove_CreateEffect`),
`FUN_321e1e60` (`PlayEffectSet`) and their helpers. The plan's Phase 4a section holds the spec
table (key, June VA range, March site, verdict), and the matrix's *Runtime* section is
generated from the same table (`RUNTIME` in `census.py`). The work is staged. **Stage 1** (this
entry) changes:

- **Target-mesh list.** Every `BONE_TRACE_TARGET_MESH` effect is pushed onto
  `EffectSetInstance::m_vecTargetMeshData`, and `BONE_TRACE_TARGET_MESH_INDEX` picks the mesh.
  - Position, rotation and offset each clamp the index as June does.
  - An effect that is itself a `BONE_TRACE_TARGET_MESH`, with the default index -1, no
    longer traces the first target mesh.
  - Entry 0 is mirrored into the stock `m_hTargetMesh` / `m_eTargetMeshEffectType`, which the
    not-yet-transcribed update and delete code still reads.
  - The damage-effect and mesh-player registrations differ, as they do in June: only the
    mesh path refreshes facing, direction and Z on every push and sets the instance's
    bone-trace scale.
- **Position offset.** In the game-unit branch of `SetEffectSetOffset`:
  - X/Z follow the **trace target's** facing when a valid trace target exists; stock uses
    the owner's.
  - `ZOFFSET_USE_UNIT_IS_RIGHT` subtracts the Z offset while that unit faces left.
  - An effect that traces a target mesh is offset by unit facing, not by the target mesh's
    dir/Z. June's target-mesh branch there is unreachable for any valid index (an inverted
    bound, `321d98c0`).
- **`OVERLAP`.** `_CreateEffectSetInstance` refuses a new instance while one of the same
  templet lives with the same trace target. `PlayEffectSet` then returns an invalid handle.

**`SERV_IRUHADEV_JUNE_FX_DEBUG`** adds `JUNE_FX ES_CREATE <set> <effect> <key> <detail>` to
`offline_server.log`, at most once a second per (set, key, detail).
- It fires on each June branch taken: `BONE_TRACE_TARGET_MESH registered_index=..`,
  `POSITION_OFFSET facing=trace_target`, `BONE_TRACE_TARGET_MESH offset=unit_facing`,
  `ZOFFSET_USE_UNIT_IS_RIGHT z_added/z_subtracted`, `BONE_TRACE_TARGET_MESH_INDEX ..`,
  `OVERLAP refused_new_instance`.
- The position and offset helpers also run per frame for traced effects, so a line can come
  from the update path.

**Play-tested 2026-09-26.**
- 47 `ES_CREATE` lines fired on Ranox Burned Forest and Ashes Village sets:
  `registered_index=0(mesh)` and `offset=unit_facing` on `EffectSet_NUI_RAGE_BABYFIG_Attack_A_Ready`,
  `EffectSet_NUI_SMITH_STAIN_Attack_Ready`, `EffectSet_NUI_RAGE_MOTHERFIG_Attack_B_Ready`, and
  `EffectSet_MOUNT_MAG_STONE_Wait`.
- No unhandled packets or exceptions, and a clean shutdown.
- The damage-effect registration path was confirmed with Rena's Perfect Storm
  (`registered_index=0(damage)`).
- The fire Hamelling's Attack X (`EffectSet_Riding_Red_HAMELLING_Attack_X`) fired
  damage-effect registration and the unit-facing offset of a sibling traced on it. That is
  the visible stage-1 change against stock.

**Stage 2** (2026-09-26) extends the same flag. Each June offset was mapped to a March member
twice: once with a `cl` `offsetof` probe under X2Lib's `US_SERVICE` defines, and once with a
June function whose shape matches a March one. The mapping table is in the plan's Phase 4a
stage-2 notes.
- **`USE_IS_RIGHT_WHEN_CREATE`.** The `EffectSetInstance` ctor records the creator's facing
  (`m_bIsRightWhenCreate`). A damage effect with the key gets it as its follow-line facing,
  `CEffect::SetFollowLineRight` (June CEffect `+0x135`).
- **`APPLY_DAMAGE_EFFECT_VELOCITY`.** The main effect's `m_vVelocity` is added to, or multiplied
  per component. The modifier is also stored on the mesh, and that needs a change outside
  X2Lib: `CKTDGXMeshPlayer::CXMeshInstance` gains `m_vecVelocityModifier` (appended last) with
  `AddVelocityModifier` / `ApplyVelocityModifier`. `CXMeshEvent_Velocity::OnFrameMove` then
  re-applies the list, as June does:
  - in the live `X2OPTIMIZE_..._VELOCITY_ACCUM_BUG_FIX` branch, to `m_vVelocity` for events
    starting after time 0;
  - to `m_vVelocityFinal` for fading events.
- **`TRACE_MATRIX_IN_FIELD`.** All four mesh and all four particle cases get June's
  bone-matrix trace: `SetUseDXMatrix` from the bone's `combineMatrix`, plus `POSITION_OFFSET`,
  or else the bone position. Three June quirks are transcribed as they are:
  - For meshes, the owner branch reads the new mesh's **own** skeleton; for particles it reads
    the set's `m_pXSkinAnim`.
  - The target mesh is always looked up in the UI-major mesh player.
  - The offset is added a second time.
  
  A mesh without a skin anim is skipped with a log line. June would fault there.
- **`MESH_TRIGGER_DATA_LIST`.** The mesh `EffectInstance` keeps its own copy
  (`m_vecMeshTriggerData`) for Phase 4b. June also clears mesh `+0x194`. That member is
  June-only and has no March counterpart, so it is not reproduced.
- **`SET_OVER_UI`.** It now also sets the particle's layer to `XDL_DIRECTING` (500). This is the
  one stage-2 branch deployed data can fire, and it is a visible change: the Ice Heater UI
  particles now draw on layer 500.
  - March's own `UIEffectSet.lua` (x38) sets it on `EffectSet_Ice_Heater_Start` and its
    `_Mini_`, `_Ancient_` and `_All_In_One_` variants, and on the `_Wait` sets.
  - `UIEffectSet_INT.lua` (x8, loaded under `INT_RESOURCE_DEVIDE`) sets it on the Halloween
    variants.
  - The sets play while an Ice Heater opens. That goes through `EGS_OPEN_RANDOM_ITEM_REQ`,
    which the offline emulator answers.

New `JUNE_FX ES_CREATE` details:
- `APPLY_DAMAGE_EFFECT_VELOCITY added/multiplied`;
- `USE_IS_RIGHT_WHEN_CREATE follow_line_right=0|1`;
- `TRACE_MATRIX_IN_FIELD mesh_matrix=own_bone / mesh_matrix=target_mesh_bone / mesh_pos=target_mesh_bone`,
  the `particle_*` equivalents, and `skipped=..`;
- `MESH_TRIGGER_DATA_LIST copied=N`;
- `SET_OVER_UI layer=XDL_DIRECTING`.

The mesh-side re-apply runs in KTDXLIB, which cannot reach `offline_server.log`. It is cited
and proven compiled in (a temporary `#pragma message` in the build), but it is not
log-verified.

No deployed set uses any of the other four keys. All four effect-set files March loads were
checked: `EffectSet.lua`, `UIEffectSet.lua`, `EffectSet_INT.lua` and `UIEffectSet_INT.lua`
(`X2StateStartUp.cpp:164-177`). They are used by June's data: `EffectSet.lua`
(`USE_IS_RIGHT_WHEN_CREATE` x27, `APPLY_DAMAGE_EFFECT_VELOCITY` x6, `TRACE_MATRIX_IN_FIELD` x6) and
`UIEffectSet.lua` (`MESH_TRIGGER_DATA_LIST` x11, `TRACE_MATRIX_IN_FIELD` x6). They fire from
Phase 11, when June's files are adopted.

**Built `US_SERVICE` (KTDXLIB, X2Lib, X2) and deployed to `X2_offline.exe` (SHA-256
`d31b2cb8328d66c1…`). Play-tested 2026-09-26.**
- The Ice Heater (108900 + key 207500) and the Halloween Ice Heater (85003498 + key 85003501)
  were opened. Every open logged `SET_OVER_UI layer=XDL_DIRECTING` on the Wait and Start sets.
- No unhandled packets or exceptions, and a clean shutdown.
- The four items were added to the offline shop for the test, in category 34 (Consume > Etc),
  product numbers 2375-2378. The save was backed up first to
  `db_backup/*.bak-pre-ice-heater-shop-20260926-094553`. Delete those rows to undo.
- Regression, second run: Rena's Perfect Storm and the fire Hamelling Attack X logged exactly
  their stage-1 lines, with no errors and a clean shutdown. The Ranox mesh sets were not re-run.

**Stage 3** (2026-09-26) extends the same flag. The member mapping is in the plan's Phase 4a
"Stage 3 result".
- **`GROUP_NAME`** (particle types only). `OnFrameMove_CreateEffect` now takes rotation and scale
  once per effect, then loops.
  - Without `GROUP_NAME`, the loop body runs once, with the same `GetEffectPosition` call as before.
  - With it, the loop creates one particle per bone `<base>_GROUPCHILD_<n>`, n from 1, where
    `<base>` is the name up to `_GROUP`. It stops at the first bone that neither the holder nor
    the creation target mesh has.
  - The position comes from the new `EffectSetInstance::GetEffectPositionByBoneName` (June
    `321dd590`), and from `GetEffectPositionByTargetMesh( .., bone, found )` (`321dcc20`) on a
    target mesh.
  - Each child `EffectInstance` keeps the flag, the bone and (with `BONE_NAME`) its offset in the
    target mesh's `BONE_NAME` frame. Phase 4b's `UpdateEffect` needs all three.
  - The particle `TRACE_MATRIX_IN_FIELD` helper takes the child's bone.
  - The stock body is the loop body. It is not re-indented, so its bytes are unchanged.
- **Unsupported-key logs** (plan rule 5, not behind the debug flag). `JUNE_FX unsupported <KEY> in
  <templet>: <system> absent (value N, June <VA>)` is written to `offline_server.log`, once per
  key per templet, where June would act on the key:
  - `APPLIED_SKILL_ID` in `PlayEffectSet`, for user owners;
  - `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`, at templet level in `_CreateEffectSetInstance` and per
    effect (` EFFECT<i>`) in `OnFrameMove_CreateEffect`;
  - `NOT_ALLOW_MEMO` in `GetEffectScale`.
- New `JUNE_FX ES_CREATE` details: `GROUP_NAME child=<bone>`, `children=N(321f2581)`,
  `child_bone_on_target_mesh_only`, `offset_in_target_mesh_bone`,
  `offset_not_reproduced=no_target_mesh_bone`.

**No data file uses `GROUP_NAME`.** Neither the four March-loaded effect-set files nor any June
effect lua contains it, so the branch is implemented from the disassembly and cannot be
play-tested. None of the four March files uses the three rule-5 keys either. June's
`EffectSet.lua` does (`APPLIED_SKILL_ID` x170, `NOT_ALLOW_MEMO` x7, effect alpha x27), so those
lines can first fire at Phase 11.

**Built `US_SERVICE` (KTDXLIB, X2Lib, X2) and deployed to `X2_offline.exe` (SHA-256
`7fd69ad01bce53c8…`). Regression play-tested 2026-09-26.** Every effect set now goes through the
restructured loop, and the run logged exactly the stage-1/2 lines: Perfect Storm and the fire Hamelling
Attack X (damage registration, unit-facing offset), both Ice Heaters (`SET_OVER_UI` layer), and the Ranox
Babyfig (mesh registration, unit-facing offset). No `GROUP_NAME` or `unsupported` lines appeared, as expected. No
unhandled packets or exceptions, and a clean shutdown.

**Not done:** June's tag-2 and tag-3 mesh
modifiers (`31c89fb0` on `+0x4ec`/`+0x4f8`, `31c8a720` on `+0x5cc`/`+0x5d0`), which are pushed
only by `31c8aac0` from `32c076e0`, outside the effect code. `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`
is deferred: its consumer is on the unit side.

Client-only; no server rebuild. Undefining `SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME` restores the
stock creation, offset, position and rotation code, which sits intact in each `#else`. The
mesh-player additions are pure additions under the same flag, so a KTDXLIB and X2Lib rebuild
(touch both `stdafx.cpp`) removes them.

### June2017 effect engine: EffectSet delete, update and camera-shake runtime (Phase 4b)

Phase 4b of `JUNE2017_EFFECT_REBUILD_PLAN.md`, extending
**`SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME`**. Transcribed from June's `FUN_321e4020`
(`OnFrameMove_DeleteEffect`), `FUN_321e0080` (`OnFrameMove_UpdateEffect`) and `FUN_321dbcc0`
(`OnFrameMove_StartCameraShake`) and their helpers. The spec table is in the plan's Phase 4b section.
`census.py` now requires a runtime citation for every EffectSet key at every sub-path, and fails
on a leftover `4b@VA` placeholder or `stage N` state. All three stages are in.

**Stage 1**, the part deployed data can fire:
- **Delete.** A set that traces target meshes is deleted only when *no* target-mesh entry holds a
  handle any more (stock tested entry 0 alone). `DELETE_EFFECTSET_AT_TRACE_UNIT_DIE` deletes the set
  when its trace target dies, the same test the two stock instance flags use.
- **Update.**
  - An effect whose target mesh is gone is **hidden** and skipped, per effect. Stock `break`s out of
    the set's loop after moving the first such effect.
  - `GROUP_NAME` children follow their own bone.
  - `TRACE_MATRIX` also applies to UI-major particles, and is skipped while a trigger's
    `STOP_TRACING_ON_TRIGGER` is set.
  - `GetCombineMatrix` gains June's target-mesh index rule, and bone frames for square units and
    pets (stock returned NULL for both).
  - `REVERSE_Y` uses the index rule.
- **Camera shake**: diffed against June, the same; cited only.
- New `CX2DamageEffect::CEffect::SetShowObject` (June `31f75570`): main effect plus start / passive /
  time particles.

**Stage 2** (X2Lib, plus one KTDXLIB method):
- `SYNC_SHOW_BY_CREATE_UNIT`: the set keeps the owner's `GetShowObject()`. New particles take the
  set's show flag at creation.
- `SOUND%d` playback (per instance, `EVENT_TIME` / `USE_GLOBAL_TIME` / `RATE` / `LOOP`). The
  instance's destructor stops and closes the sounds that are still playing.
- `FOLLOW_UNIT_ROTATION(_CPARTICLE)`: meshes `SetRotateDegree`, particles `SetAddRotate`. The new
  `CParticleEventSequence::SetRotateToParticles` (KTDXLIB) sets the rotation on child particles.
- Per-frame `TRACE_MATRIX_IN_FIELD` for UI-major particles, and a per-frame land-height refresh for
  land-using particles.

**Stage 3** (X2Lib and KTDXLIB):
- Mesh triggers: the inline `MESH_TRIGGER_DATA`, and `MESH_TRIGGER_DATA_LIST` entries bound to the
  owner's emotion.
- Every `TRIGGER_DATA%d` key is transcribed: `TRIGGER_ONCE`, `STOP_TRACING_ON_TRIGGER`,
  `TRIGGER_EFFECTSET_LIST`, the toggle-visibility timer, `BLOCK_TRIGGER(_TIME)` and
  `ANIMTIME_ON_START_*`.
- `CXMeshInstance` gains six members, appended last: restore-after-trigger (June `+0x194`), played,
  blocked, block time, restore anim and its play type.
- It also gains `ChangeAnimByTrigger`, a block countdown in `OnFrameMove`, and a restore of the
  previous anim after `RunEvent`.
- `sizeof(CXMeshInstance)` is `0x524` from both an X2Lib TU and a KTDXLIB TU.
- A mesh with a `MESH_TRIGGER_DATA_LIST` now turns restore off at creation. That is the `+0x194`
  store 4a left out.

Documented, not transcribed:
- the alpha push (instance `+0x6c`), which belongs to the absent effect-alpha system;
- June's deferred-stop queue;
- the unit-viewer-only keys (`FOLLOW_UNIT_VIEWER_SCALE`, `USE_EFFECT_SCALE_IN_UNIT_VIEWER`,
  `ANIMTIME_ON_START_UNITVIEWER`), read only by `329585f0`;
- `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE`, still deferred.

**`SERV_IRUHADEV_JUNE_FX_DEBUG`** adds `JUNE_FX ES_UPDATE` / `JUNE_FX ES_DELETE <set> <effect> <key>
<detail>` lines, throttled to one a second. Expected on current data (stage 1):
- `ES_UPDATE ... BONE_TRACE_TARGET_MESH hidden=target_mesh_gone(321e0217)`;
- `ES_DELETE ... BONE_TRACE_TARGET_MESH deleted=no_target_mesh_handle_left(321e4197)` (and
  `kept=entry0_gone_another_holds_a_handle` when a later mesh outlives the first);
- `ES_DELETE ... DELETE_SHAKE_COUNT deleted=shake_limit(321e431e)` (5 sets use the key);
- `ES_UPDATE ... REVERSE_Y facing=unit|target_mesh`;
- `ES_UPDATE ... TRACE_MATRIX matrix=pet|square_unit_viewer`, on a pet or village set.

None of the four March-loaded effect-set files uses a stage-2 or stage-3 key, so those lines can
only fire after Phase 11.

**Built `US_SERVICE` (KTDXLIB, X2Lib, X2) and deployed to `X2_offline.exe` (SHA-256
`3b2cbece6764e1e2…`). Play-tested 2026-09-26.**
- Rena's Perfect Storm and the fire Hamelling's Attack X each logged
  `ES_UPDATE ... BONE_TRACE_TARGET_MESH hidden=target_mesh_gone(321e0217)` immediately followed by
  `ES_DELETE ... BONE_TRACE_TARGET_MESH deleted=no_target_mesh_handle_left(321e4197)` — the stage-1
  delete/hide pair the exit criterion asks for.
- `0` `UNHANDLED`/`EXCEPTION` packets, clean shutdown.
- The Ranox mesh set (Babyfig) registered at creation but its target mesh stayed alive the whole run,
  so no `ES_UPDATE`/`ES_DELETE` fired for it in this pass — not a failure, the damage-effect sets
  already prove the branch.
- **Not testable with current data:** `GetCombineMatrix`'s square-unit and pet branches. No deployed
  effect set combines `TRACE_MATRIX` with a pet or square-unit owner (checked all four effect-set
  files), and no in-build command plays an arbitrary set on a square unit. They stay cited but
  unverified until Phase 11.

### June2017 effect engine: DamageEffect CEffect ctor keys (Phase 5)

Phase 5 of `JUNE2017_EFFECT_REBUILD_PLAN.md`. Parse only; consuming these members is
Phase 7. Transcribed from June's `CEffect::CEffect` (`FUN_31f7aab0`), excluding the
sub-loader *calls* at `0x31f7e7b2`-`0x31f7f976` (Phase 6a/6b's), but including the
top-level decrypt sites that sit inside that same address range (`INHERIT_CRITICAL_RATE`,
`INHERIT_CRITICAL_DAMAGE_RATE`, `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`).

**`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE`** (`KTDXLIB/Always.h`, `X2Lib/X2DamageEffect.h`,
`X2Lib/X2DamageEffect.cpp`):

- **A master "read `_ULTIMATE` first" flag, forced false.** June computes one flag near the
  top of the ctor from `APPLIED_SKILL_ID`/`APPLIED_SKILL_ID_NOT_AFFECT_MEMO` (read inside the
  ctor's own `BeginTable("DAMAGE_DATA")`/`EndTable`, before `MAIN_EFFECT_NAME`) and a per-effect
  skill-memo lookup (`FUN_31fcf3e0`, candidate, not identified this phase: reads a "memo"
  pointer at `user+0x867c`). Every `"K"`/`"K_ULTIMATE"` read below (`MAIN_EFFECT_NAME`,
  `FOLLOW_LINE`, `FOLLOW_LINE_NEW`, `DAMAGE_DATA`, `DAMAGE_DATA_NEXT`,
  `DAMAGE_DATA_CHANGE_TIME(_GLOBAL_TIME)`) tries `_ULTIMATE` first only when this flag is set,
  falling back to `K` otherwise. The "applied skill ID" system is absent
  (`JUNE2017_EFFECT_KEY_MATRIX.md`'s *Absent systems*), so both ids are parsed, stored and
  logged only (plan rule 5, `m_bUseUltimateDamageData` forced `false`) -- which reproduces
  today's March behaviour exactly, since every `_ULTIMATE` branch is then unreachable.
- **Plain bool/float/string reads**, June getter and default, no consumer yet:
  `USE_ADVANCE_COLLISIOIN_CHECK` (studio typo, kept), `USE_OWNER_ATTACK_SPEED`, `IGNORE_MI_HUGE`
  (bool, false; when false June also overrides the main effect's scale via `31f6fdc0`, not
  identified this phase -- see the code comment for the Phase 7 handoff), `INHERIT_CRITICAL_RATE`,
  `INHERIT_CRITICAL_DAMAGE_RATE`, `FOLLOW_LINE_WITH_CUSTOM_ROTATE`, `FOLLOW_LINE_GRAVITY`
  (bools, false), `TRACE_OWNER_BONE`, `TRACE_OWNER_BONE_ONLY_ROTATE` (wstrings, `L""`),
  `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT` (float, **-1.f** -- confirmed byte-exact by reading
  the June binary's float constant at `0x334380f0`, which is also `ATT`/`DamageEffect_ATT`'s
  `LIMIT_DISTANCE` default per Phase 3's hand-verified notes).
- **`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`** (int, 0): binary-only at this scope (no in-scope
  June data sets it inside `DAMAGE_DATA` at the CEffect level, only via `SetDamageDataFromLUA`'s
  own table and at the EffectSet level); unsupported, same absent effect-alpha-option system as
  Phase 2's EffectSet-level read.
- **`DAMAGE_DATA`/`DAMAGE_DATA_ULTIMATE`.** March's existing single
  `SetDamageDataFromLUA(&m_DamageData, ..., "DAMAGE_DATA", ...)` call is restated as
  `_ULTIMATE`-then-`DAMAGE_DATA`, gated by the forced-false flag, so it still reduces to exactly
  one call. `m_bUsedDamageDataUltimate` records which table was actually read, for Phase 8.
- **`DAMAGE_DATA_NEXT`/`_ULTIMATE`/`%d` chain.** June's `FUN_31f79c20` (not a thin wrapper) reads
  the unnumbered table as before, then loops reading `DAMAGE_DATA_NEXT2`, `DAMAGE_DATA_NEXT3`,
  ... (or `..._ULTIMATE2`... on the ultimate branch) into new entries, each copied from
  `m_DamageData` (not chained from the previous entry), stopping at the first table that does not
  exist. Numbering starts at **2** -- there is no `..._NEXT1` -- confirmed against
  `june2017_re/june_lua/DamageEffect*.src.lua`. The unnumbered entry stays
  `m_pDamageDataNext` for existing consumers; entries 2.. go into a new
  `DamageEffectDataInLua::m_vecDamageDataNextChain`, freed in the ctor's failure path and the
  destructor.
- **`DAMAGE_DATA_CHANGE_TIME(_GLOBAL_TIME)` -- semantics-differ.** At this scope, June reads the
  key as an indexed float array (`BeginTable(key)` + `GetValue(1..)`, June `FUN_31f64b20`, shared
  by both keys via a bool arg), not the scalar March already reads into
  `m_fDamageDataChangeTime(GlobalTime)`. Added as new `vector<float>` members
  (`m_vecDamageDataChangeTime`, `m_vecDamageDataChangeTimeGlobalTime`), read in parallel with the
  existing scalar reads, which are left untouched for their consumers. Checked against both
  `march2014/data036/` (scalar: `KEY = 0.5`) and `june2017_re/june_lua/` (table: `KEY = { ... }`):
  current March/Ranox data always uses the scalar form, so `BeginTable` fails and these vectors
  stay empty -- additive, no behaviour change until June's own data is adopted (Phase 11).
- **Three census.py findings reported, not corrected in code** (the generated matrix is
  regenerated, never hand-edited):
  - `APPLIED_SKILL_ID`/`_NOT_AFFECT_MEMO` are read by the ctor itself, inside its own
    `BeginTable("DAMAGE_DATA")` -- i.e. at the `DE.DAMAGE_DATA` path, not `DE.top` where the
    matrix places `APPLIED_SKILL_ID`. Doesn't block the exit criterion (`APPLIED_SKILL_ID` is
    `unsupported`, not `to-implement`, either way).
  - `APPLIED_SKILL_ID_NOT_AFFECT_MEMO` is decrypted and read by the ctor (getter `32fe9fd0` at
    `31f7b801`), but the matrix calls it `june-ignores(dead-here)`.
  - Before this phase, `DAMAGE_DATA_CHANGE_TIME(_GLOBAL_TIME)` was already marked
    `n/a (sub-table name)` even though March only had a scalar read at that path -- the census's
    table-vs-scalar check doesn't fire for this key's shape.

**`SERV_IRUHADEV_JUNE_FX_DEBUG`** adds `JUNE_FX CE_TEMPLET <name> <KEY>=<v> ...`, once per
`CEffect` that sets a non-default Phase-5 key, mirroring `ES_TEMPLET`. Runs at actual gameplay
time (a `CEffect` is created per damage-effect instance, well after the offline server's log is
open), so it goes to `offline_server.log` via `CX2OfflineLog::Server`, not `ErrorLog.txt`. The
`APPLIED_SKILL_ID`/`_NOT_AFFECT_MEMO` unsupported line is separate and per-effect-name throttled
(1/second), since `CEffect` instances are created continuously during play.

**Data note.** Checked `march2014/data036/DamageEffect.src.lua` (the only `DamageEffect*.lua` this
build loads) and `DamageEffect_INT.lua`: no Phase-5 `_ULTIMATE`/array-form key is used, but
`DamageEffect_SA_RHB_Lockon_Stratos_Attack`'s `DAMAGE_DATA` sets
`APPLIED_SKILL_ID = SKILL_ID.RNSI_SA_RHB_LOCKON_STRATOS` -- a real, already-packed test case for
the `unsupported` log line. It belongs to Rose, not yet ported, so it could not be triggered this
pass.

**Built `US_SERVICE` (KTDXLIB, X2Lib, X2) and deployed to `X2_offline.exe`. Play-tested
2026-09-26** (multiple dungeon rooms, a field warp, two class changes, cash-shop browsing): clean
shutdown (`wal checkpoint ok`), `0` `UNHANDLED`/`EXCEPTION` lines in `offline_packets.log` --
this phase's ctor changes run on every `CEffect` created and introduced no regression. No
`JUNE_FX` line fired, expected: nothing in this session's `DamageEffect.src.lua` data sets a
Phase-5 key other than the untestable Rose one above. `RNSI_SA_RHB_LOCKON_STRATOS` stays open
until Rose is ported.

Revert as for Phase 2-4b: undefine the flag. Nothing consumes the new members yet (Phase 7), so
undefining it only removes the parse sites and the `_ULTIMATE`/array/chain members -- no
behaviour changes either way.

### June2017 effect engine: DamageEffect sub-loaders I (Phase 6a)

Phase 6a of `JUNE2017_EFFECT_REBUILD_PLAN.md`. Parse only; the loop/condition runtime is
Phase 7c. Transcribes June's `CREATE_DAMAGE_EFFECT%d` loader (`FUN_31f6ac60`), the
die/hit damage-effect loaders (`FUN_31f68f30`/`FUN_31f6ceb0`, each the *full* `"<base><index>"`
loop -- not a "per-entry body" as Phase 0's table said; `funcmap.tsv` corrected) via their
wrappers (`FUN_31f6e7e0`/`FUN_31f6e850`), and the die-condition parser (`FUN_31f66700`).
Extends `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE` (+ `SERV_IRUHADEV_JUNE_FX_DEBUG`'s new
`DDE_TEMPLET`/`DDE_CONDITION` lines).

- **`CREATE_DAMAGE_EFFECT%d` new keys** (`LoadDamageEffect`, `X2DamageEffect.cpp:7366`):
  `LOOP_INTERVAL` (float, 0.f), `LOOP_END_COUNT` (int, 1, getter `32fea210` -- a distinct
  "int" getter from `32fe9fd0`, rounds via `FISTP`), `LOOP_END_TIME` (float, **99999.f** --
  confirmed byte-exact against the June binary's `0x33441c60`, cross-checked against the
  already-verified `-1.0` control; matches March's own dormant `LOOP_CREATE_DAMAGE_EFFECT`-gated
  default, so `same` not `differ`), `LOOP_ADD_POS_X`/`_Y` (float, 0.f; `_Z` binary-only),
  `SET_DIE_WHEN_MASTER_EFFECT_DIE` (bool, false), `INHERIT_LIFE_TIME` (bool, false --
  *not* the float "inherit at this rate" its name suggests), `INHERIT_DAMAGE` (bool, false).
  The existing `LOOP_CREATE_DAMAGE_EFFECT`-gated block right above (dead: that flag is never
  defined under `US_SERVICE`, which is *why* census found these three keys `to-implement`
  despite the dead code already naming them) is left untouched; these are fresh reads under
  our own flag, with distinct member names, so there is no collision if that old flag is ever
  turned on.
- **`DIE_DAMAGE_EFFECT%d` new keys** (`LoadDieDamageEffect`, `:7078`): `INHERITLOCKONDATA`
  (studio spelling, bool, false), `SCALE_X`/`_Y`/`_Z` (float, **1.f** via `FLD1`, not 0.f),
  `INHERIT_LIFE_TIME` (bool, false, same shape as `CREATE_DAMAGE_EFFECT`'s).
- **`HIT_DAMAGE_EFFECT%d` new keys** (`LoadHitDamageEffect`, `:7562`): `SCALE_X`/`_Y`/`_Z`
  (float, 1.f), `INHERIT_LIFE_TIME` (bool, false).
- **`DIE_DAMAGE_EFFECT_ULTIMATE%d`/`HIT_DAMAGE_EFFECT_ULTIMATE%d`, forced off** (`:7040`,
  `:7472`). The wrapper's own inline gate (`31f6e7e3`-`31f6e822`/`31f6e853`-`31f6e892`: an
  owner-unit/memo-pointer null chain, then `FUN_31fcf3e0([+0x7c4])` -- the same unidentified
  skill-memo callee Phase 5's ctor calls to compute `m_bUseUltimateDamageData`, a different call
  site but the same absent system) is reused as `m_bUseUltimateDamageData` rather than
  re-derived, and stays forced false for the identical reason. When it would pass, the wrapper
  tries the `_ULTIMATE` loader first and skips the base loader only if that read at least one
  entry (index 0 existed); today it always falls straight through to the unchanged base loop.
  Duplicates the per-entry key list rather than adding a new method, so the header/PCH did not
  need touching for this half of the phase.
- **The die-condition parser** (`LoadDieCheckCondition`/`LoadDieCheckConditionBlock`,
  `:6837`/`:6782`, called from the ctor at `:1805`, new `X2DamageEffect.h` enum/struct/members
  at `:98`/`:448`/`:970`/`:1252`). `DAMAGE_EFFECT_DIE_CHECK_CONDITION` has no March equivalent
  before this phase (June-only). Opens `AND_OPERATION`/`OR_OPERATION` (a shared helper takes the
  table name and a `bIsOrOperation` flag, since a `BeginTable` on a runtime variable still
  resolves for census's text scanner -- only a *numbered* `BeginTable(var, i)` cannot, which is
  why the `_ULTIMATE` loops above use a literal name instead of a shared helper); each opens a
  numbered `CONDITION_%d` list (the base name is `"CONDITION_"`, since March's
  `BeginTable(name, index)` does a plain `"%s%d"`). Per entry:
  `DAMAGE_EFFECT_DIE_CONDITION_TYPE` (new enum, `DEDCT_NONE`/`DEDCT_DAMAGE_TIME`/
  `DEDCT_LOCK_ON_TARGET`, values from `june2017_re/june_lua/Enum.src.lua:11746-11749`; a failed
  read or `DEDCT_NONE` drops the whole entry, matching June exactly) then, only for
  `DEDCT_DAMAGE_TIME`, `CHECK_DAMAGE_TIME_VALUE` (float, -1.f) and `IS_OVER` (bool, **true**);
  only for `DEDCT_LOCK_ON_TARGET`, `IS_EXIST` (bool, false). Every entry (from either sub-table)
  is tagged `bIsOrOperation` and pushed into one shared `m_vecDieCheckCondition` -- confirmed by
  tracing June's third call argument through the frame to its store into the pushed entry's own
  second field (`README.md`'s Phase 6a hand-verified notes have the byte accounting).
- **census.py's `ROOTS` needed real entries** for the new `DE.DIE_CHECK_CONDITION` scope (its
  March column was `[]`): `LoadDieCheckCondition` added to `CE_LOADERS`, and
  `ce('LoadDieCheckCondition')`/`ce('LoadDieCheckConditionBlock', ())` added to the root's own
  March list. No change to the matching logic itself.
- **Two binary-only rows stay unresolved at this scope, on purpose**:
  `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION` and `DAMAGE_DATA_NEXT_ULTIMATE%d` are ctor-level keys
  that land under `DE.DIE_CHECK_CONDITION` only because `CE` (the ctor) is listed as a June
  reader of this root too; both are already implemented at `DE.top` by Phase 5. Not parsed
  twice.
- **`JUNE_FX DDE_TEMPLET <effect> <TABLE><n> <key>=<v> ...`** (one per `CREATE_DAMAGE_EFFECT%d`/
  `DIE_DAMAGE_EFFECT%d`/`HIT_DAMAGE_EFFECT%d` entry that sets a non-default Phase-6a key) and
  **`JUNE_FX DDE_CONDITION <effect> <AND|OR>_OPERATION<n> type=.. isOr=.. checkTime=.. isOver=..
  isExist=..`** (one per die-condition entry) are both per-effect-name throttled (1/second),
  the same pattern as Phase 5's `APPLIED_SKILL_ID` line -- `LOOP_INTERVAL`/`LOOP_END_COUNT`
  alone are set by ~557 June effects, so an un-throttled line would be written continuously
  during play.

**Built `US_SERVICE` (X2Lib, X2) and deployed to `X2_offline.exe`.** `census.py` shows 0
Phase-6a keys `to-implement` (the two binary-only rows above are the only open items, and both
are a named exception, not work). Play-test target: `DAMAGE_EFFECT_Trap_Fire_Big`
(`CREATE_DAMAGE_EFFECT0..3`, using `LOOP_ADD_POS_X`/`_Y` and
`SET_DIE_WHEN_MASTER_EFFECT_DIE`) is spawned by the `TRAP_FIRE_BIG` NPC placed in the Ranox
Burned Forest dungeon (`Dungeon_Ranox_BurnedForest_{Normal,Hard,Expert}.src.lua`) -- not yet
play-tested this session; ask before launching.

Revert as for Phase 5: undefine the flag. Nothing consumes `m_vecDieCheckCondition` or the new
`DamageEffectData` fields yet (Phase 7c), so undefining it only removes the parse sites -- no
behaviour changes either way.

### June2017 effect engine: DamageEffect sub-loaders II (Phase 6b)

Phase 6b of `JUNE2017_EFFECT_REBUILD_PLAN.md`. Parse only; runtime is Phase 7b/7c. Transcribes
the remaining `CEffect` ctor sub-loaders: the particle table (`FUN_31f66e60`, shared by the four
`START_`/`PASSIVE_`/`TIME_`/`HIT_PARTICLE` wrappers), lock-on (`FUN_31f71930`), sound
(`FUN_31f73f30`), and the three June-only loaders `TOUCH_LAND_DAMAGE_EFFECT%d` (`FUN_31f76a90`),
`HIT_EFFECT_SET%d` (`FUN_31f64c80`) and `DIE_CREATE_NPC%d` (`FUN_31f66230`), none of which had a
March reader before this phase. Extends `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE` (+
`SERV_IRUHADEV_JUNE_FX_DEBUG`'s new `LOD_TEMPLET` line).

- **`LoadParticleTable` new key** (`X2DamageEffect.cpp:5628`): `APPLY_START_PARTICLE_SCALE` (int,
  getter `32fe9fd0`, default 0) -- read unconditionally, regardless of which of the four wrappers
  called this function; despite the name, it is not scoped to `START_PARTICLE` in the binary.
- **`LoadLockOnData`/`LoadBlackHoleData` restructured into a name-parameterised gate plus a body
  helper** (`LoadLockOnDataBlock`/`LoadBlackHoleDataBlock`, `X2DamageEffect.cpp:6699`/`6718`/
  `6962`/`6987`), mirroring June's own `"LOCK_ON_DATA_ULTIMATE"`/`"BLACKHOLE_DATA_ULTIMATE"`-first,
  `"LOCK_ON_DATA"`/`"BLACKHOLE_DATA"`-fallback shape (`FUN_31f7aab0` ctor
  `@31f7e7a4-31f7e7da`/`@31f7e7d3-31f7e7e9`), gated on `m_bUseUltimateDamageData` (forced false,
  same absent system as Phase 5/6a). The original single-shot functions are kept verbatim in the
  `#else` branch. `census.py`'s `ROOTS` needed the two new `Block` helpers added alongside the
  existing `ce('LoadLockOnData')`/`ce('LoadBlackHoleData')` entries (empty prefix, same shape as
  Phase 6a's `LoadDieCheckConditionBlock`), or the keys already read by the un-refactored bodies
  would have shown as regressed to `to-implement`.
- **`LOCK_ON_DATA` new keys** (`LoadLockOnDataBlock`): `RANDOM_SEED` (reuses the existing, until
  now unparsed, `m_iRandomSeed`; int, default -1), `UNLOCK_START_AFTER_ATTACK` (int, default -1 --
  despite the name, not a bool), `RETARGET_COOLTIME_AFTER_ATTACK` (float, -1.f), `Y_DIR_INIT`
  (bool, false), `LOCK_ON_OFFSET_Y_POS` (float, 100.f, binary-only), `LOCK_ON_TARGET_BONE_NAME`
  (wstring, `L""`), `INIT_TARGET` (bool, false -- the runtime effect, an immediate nearest-target
  lookup at ctor time, is Phase 7b's), `NOT_LOCKON_INVINCIBLE_TARGET` (bool, false).
- **`NOT_TARGET_PRE_LOCK_ON_TARGET` semantics-differ**: June also reads this key as an int
  (getter `32fe9fd0`, default 0, `@31f721dd`) alongside March's existing bool
  (`m_bIsNotTargetPreLockOnTarget`, under `BALANCE_DEADLY_CHASER_20130214`). Added as a new,
  parallel `m_iNotTargetPreLockOnTarget` member rather than replacing the bool read, the same
  pattern as Phase 5's `DAMAGE_DATA_CHANGE_TIME`; deciding how the two coexist is Phase 7b's.
- **`LOCK_ON_RANGE_FROM_OWNER` sub-table** (`@31f73202-31f7354b`): June selects only one of
  `AT_DUNGEON`/`AT_FIELD`/`AT_RAID_FIELD`/`AT_PVP` at ctor time by the effect's current game mode,
  read from `*(int*)(DAT_3377585c+0x8c)` -- not identified. Parsed unconditionally here (every
  member read, regardless of mode: `AT_DUNGEON`/`AT_RAID_FIELD`/`AT_PVP` default 99999.f,
  `AT_FIELD` defaults 1000.f, all hand-verified against the raw exe bytes alongside the existing
  `-1.0`/`99999.0` controls); identifying the mode selector and wiring it up is Phase 7b's.
- **`DUNGEON_TARGET_RANGE_MIN_MAX`/`PVP_TARGET_RANGE_MIN_MAX`**: plaintext (unencrypted) sub-table
  names, read positionally (`GetValue(1,..)`/`GetValue(2,..)`), reset to their own defaults
  (150.f/750.f and 150.f/300.f) unless both indices parse and max is strictly greater than min,
  matching June's `fMax<=fMin` reset check byte-for-byte. Getting `census.py` to credit these
  (and `LOCK_ON_RANGE_FROM_OWNER` above) required writing the `BeginTable` call directly inside
  the `if( true == ... )` condition rather than as a separate assignment statement followed by a
  bare `if` on the stored bool -- the census's brace-depth tracker only auto-closes a pushed table
  name when the `BeginTable` call itself sits inside the recognised `if(...)`/`while(...)` shape;
  a bare-statement `BeginTable` needs a same-depth `EndTable()` to close, which an `EndTable()`
  nested one level deeper (inside the `if`'s own block) never satisfies, silently nesting every
  read for the rest of the function under the leaked-open table name. Found by re-running
  `census.py` after the phase's first pass and noticing `DUNGEON_TARGET_RANGE_MIN_MAX` recorded
  at path `('LOCK_ON_RANGE_FROM_OWNER',)` instead of `(top)`.
- **`SOUND%d` new keys** (`LoadSoundData`, `:7556`): `DELETE_WHEN_EFFECT_END` (bool, false),
  `THREE_DIMENSIONAL_SOUND` (bool, **true**).
- **`TOUCH_LAND_DAMAGE_EFFECT%d`, `HIT_EFFECT_SET%d`, `DIE_CREATE_NPC%d`: no March reader before
  this phase (June-only)**. New loaders `LoadTouchLandDamageEffect`/`LoadHitEffectSet`/
  `LoadDieCreateNpc` (`X2DamageEffect.cpp:7899`/`7863`/`7880`), called from the ctor right after
  `LoadHitDamageEffect()` in June's own order (`@31f7e852-31f7e867`).
  - `TOUCH_LAND_DAMAGE_EFFECT%d` reuses the existing `DamageEffectData` struct rather than a new
    one: its decrypted key order (`POSITION_TYPE`, `DAMAGE_EFFECT_NAME`, `BONE_NAME`,
    `APPLY_ROTATE`, `APPLY_ROTATE_ORIGIN`, `ONLY_APPLY_ROTATE`, `APPLY_ROTATED_OFFSET`,
    `ROTATE_X/Y/Z`, `OFFSET_X/Y/Z`, `SCALE_X/Y/Z`, `PASSIVE_PARTICLE_X/Y/Z`,
    `INHERIT_BUFF_FACTOR`, `INHERIT_LIFE_TIME`) matches that struct's fields 1:1, including reusing
    Phase 6a's `vScale` (default 1.f, not `vMainEffectScale`'s 0.f) for `SCALE_X/Y/Z`. No
    `EVENT_TIME` read at this scope. Stored in a new `m_vecTouchLandDamageEffect` vector.
  - `HIT_EFFECT_SET%d` (`X2DamageEffect.h`'s new `HitEffectSetData`: `EVENT_TIME` float -1.f,
    `EFFECT_SET_NAME` wstring `L""`) and `DIE_CREATE_NPC%d` (new `DieCreateNpcData`, a value type:
    `EVENT_TIME` float -1.f, `NPC_ID` int 0) are each a small dedicated struct/vector
    (`m_vecHitEffectSet`, `m_vecDieCreateNpc`) rather than reusing `DamageEffectData`, since their
    key sets don't overlap it. `HIT_EFFECT_SET%d` itself is binary-only: no in-scope June data
    opens the sub-table at all, but it is still parsed per plan rule 3.
  - `census.py`'s `ROOTS` needed the three new roots' March column filled in (was `[]`, no reader
    existed) and the three new function names added to `CE_LOADERS`, so the ctor's own read of
    each top-level table name (`TOUCH_LAND_DAMAGE_EFFECT%d`/`HIT_EFFECT_SET%d`/`DIE_CREATE_NPC%d`
    at `DE.top`) is credited too -- the same two-part registration Phase 6a's
    `LoadDieCheckCondition` needed.
- **`JUNE_FX LOD_TEMPLET <name> <key>=<v> ...`** (one per `LoadLockOnDataBlock` call that sets a
  non-default Phase-6b key) is ctor-time only (one `LoadLockOnData()` call per `CEffect`), so it
  needs no throttling map, unlike the per-frame-adjacent `DDE_TEMPLET`/`ES_TEMPLET` lines.

**Built `US_SERVICE` (X2Lib, X2) and deployed to `X2_offline.exe`.** `census.py` shows 0
Phase-6b keys `to-implement`; two binary-only rows remain, both named exceptions like Phase 6a's:
`AT_RAID_FIELD` (parsed identically to its three siblings, just never observed in live data) and
`HIT_EFFECT_SET%d` (the sub-table itself is parsed, no live data opens it). Not yet play-tested
this session; ask before launching.

Revert as for Phase 6a: undefine the flag. Nothing consumes any of this phase's new members yet
(Phase 7b/7c), so undefining it only removes the parse sites -- no behaviour changes either way.

### June2017 effect engine: DamageEffect per-frame runtime (Phase 7a)

Phase 7a of `JUNE2017_EFFECT_REBUILD_PLAN.md`. New flag **`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`**
(`KTDXLIB/Always.h`, `X2Lib/X2DamageEffect.h`, `X2Lib/X2DamageEffect.cpp`). It needs
`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE`, whose members it consumes, and `Always.h` raises an
`#error` if it is defined without it. It adds `SERV_IRUHADEV_JUNE_FX_DEBUG`'s new
`JUNE_FX CE_FRAME` line.

**Identification first (all in `june2017_re/funcmap.tsv`):**
- June's `CEffect::OnFrameMove` is **`31f88790`**, the one CEffect virtual. Phase 0 had it as the
  deleting destructor: wrong. It ends `RET 0xc` (a `double` plus a `float`), and
  `CX2DamageEffect::OnFrameMove` **`31f79fb0`** (a line-for-line match of `X2DamageEffect.cpp:84-201`)
  calls it as `vtable[0](fTime, fElapsedTime)`.
- June's `~CEffect` (`31f78550`) is non-virtual.
- Ghidra defines no function at `31f88790`, so it was read with capstone.
- The size-based candidate `31f85910` is not OnFrameMove. It hangs off the damage manager's
  `DamageCheck` chain (Phase 7d/9).
- The callees are confirmed by call order plus the member offsets they share with their
  already-confirmed loaders:

  | June VA | Function |
  |---|---|
  | `31f6eea0` | BlackHole |
  | `31f83aa0` | LockOn |
  | `31f6f3d0` | SineMove |
  | `31f6eca0` | Sound |
  | `31f70ee0` | `FollowLine` |
  | `31f70500` | `SetParticleData` |
  | `31f76620` / `31f76710` / `31f76880` | `Set*Particle` |
  | `31f799c0` | `SetDamageDataNext` |

- The unit virtuals the new branches call were matched to March by body, from the RTTI vtables.
  Slots `0x80`, `0x138`, `0x150` and `0x158` are `GetBonePos` (bool form), `GetPos`,
  `GetRotateDegree` and `GetYRotateDegree`, and CX2Game slot `0x98` is `GetMyUnit`.
- `unit+0x264` is `m_GameUnitType`: the NPC and User ctors store 1 and 0 there.
- The attack-speed float is March's `GetAnimSpeedValue()`: June's `ResetAnimSpeed` `31ffe190`
  reads it exactly where March's does.

**Transcribed, each with its June address range in a comment:**
- **`TRACE_OWNER_BONE`** (`CEffect::OnFrameMove`, `X2DamageEffect.cpp:2243`): every frame the main
  effect is moved to the named bone of the owner, or of `g_pX2Game->GetMyUnit()` when there is no
  owner. If the bone isn't found it goes to the unit's position instead.
- **`USE_OWNER_ATTACK_SPEED`**: computed once, and only for a user owner.
  - With owner speed `f > 1`, the attack start becomes `start·(2−f)`, clamped at 0.
  - With `f < 1`, the end becomes `end·(2−f)`; if that comes out negative, the original end is kept.
  - The result is cached in the new `m_fOwnerAttackSpeedAttackTime{Start,End}`, which start at
    −1 as in June.
  - The x87 code was checked by emulating it on concrete inputs, not only by hand.
- **`USE_GLOBAL_TIME`**: the same test as March's, run on that window.
- **`REFRESH_LAND_POS`**: the crash-land update is now skipped when the probe returns `y == −1.0`
  (emulated). The first refresh block is unchanged from March.
- **`FOLLOW_LINE_WITH_CUSTOM_ROTATE`** (`FollowLine`, `:6803`): the line angle keeps the effect's own
  x/z rotation where March uses zeroes.
- **`FOLLOW_LINE_GRAVITY`, nearest-line branch**: with `FOLLOW_ON_LINE` set and no line below, the
  effect snaps to the nearest point of every enabled line (skipping `LT_WALL`/`LT_MONSTER_ROAD`),
  100 higher. Failing that, it goes to `m_BeforePos`.
- **`DAMAGE_DATA_CHANGE_TIME(_GLOBAL_TIME)` arrays**: one shared counter (`m_iDamageDataChangeTimeIndex`)
  steps both arrays through the NEXT chain via the new `SetDamageDataNextByChainIndex` (`:8325`;
  index 0 = `DAMAGE_DATA_NEXT`, 1.. = `NEXT2`..). This path is dormant on the scalar-shaped data
  the tree ships, where March's scalar blocks still run.

**Logged, not transcribed:**
- **Rule 5 (unsupported):**
  - `TRACE_OWNER_BONE_ONLY_ROTATE`: June acts only for a `UT_ROSE` user owner; Rose is absent.
  - `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`: the effect-alpha option is absent.
- **Deferred:**
  - `FOLLOW_LINE_GRAVITY`'s near-line case needs a move-enable flag on `CXMeshInstance` that March
    doesn't have (June mesh `+0x633`). That is Phase 10.
  - `IGNORE_MI_HUGE`: June's ctor branch scales by a `DamageData` field that nothing in
    `SetDamageDataFromLUA` writes, so there is no value to transcribe.
  - `REACT_ANIMSPEED` and `USE_ADVANCE_COLLISIOIN_CHECK` have no reader in OnFrameMove or
    FollowLine (Phases 7d and 9).

**Found, not ported (recorded for later phases):**
- June refreshes the ctor-argument vector `+0x118` from the owner's rotation at the top of every
  frame.
- The first-frame block copies the templet vectors into per-instance lists instead of calling
  `SetCameraData`/`SetScreenData`/`SetSoundData`/`SetDamageEffect`.
- A June `+0xec` "`m_BeforePos` initialised" flag.
- The die-condition evaluator `31f765b0` and touch-land check `31f83380` (Phase 7c).
- The per-frame accumulator `31f75da0`.
- FollowLine's changed line probe (`31c6c310`, `31c5df50`).
- `DamageCheck`'s three extra arguments (Phase 9).

**Built `US_SERVICE` (KTDXLIB, X2Lib, X2) and deployed to `X2_offline.exe`.** `census.py` passes;
the matrix's runtime table has 14 new `7a` rows. The new code was confirmed present by its strings
in `X2DamageEffect.obj`. **Play-tested 2026-09-27:**
- `JUNE_FX CE_FRAME ... USE_GLOBAL_TIME` fired for Rena's Freezing Bolt and Chung's grenade shell,
  dash-jump ZX bomb and Carpet Bombing.
- The June `REFRESH_LAND_POS` guard fired on `CHUNG_CARPETBOMBING_SMALL` (probe `y == -1`, update
  skipped). March would have set the land height to -1 there.
- Clean shutdown, no `UNHANDLED`/`EXCEPTION`, and no visible regressions on ground-following or
  landing projectiles.
- The June-only keys (`TRACE_OWNER_BONE`, `FOLLOW_LINE_GRAVITY`, `USE_OWNER_ATTACK_SPEED`,
  `FOLLOW_LINE_WITH_CUSTOM_ROTATE`) are absent from the deployed `DamageEffect.lua`, so those
  branches get their real test at Phase 11.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`. Every site keeps March's code in the
`#else` branch or is a pure addition, and the ctor only initialises the three new members.

**`/code-review` fixes, 2026-09-27 (Phase 7a plus two pre-existing Phase 6b/Lanox issues it also
found).** Built `US_SERVICE` (X2Lib, X2) clean; `census.py` re-run afterward with no status/phase
changes (only citation line numbers moved).

- **`JuneFxCeOnce`/`JuneFxCeFrame` and the Phase 5 `APPLIED_SKILL_ID` log needed `SERV_IRUHADEV_OFFLINE`.**
  `CX2OfflineLog` is declared only under that flag, but these were gated solely on
  `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`/`_PARSE`. Both flags happen to always be on together
  right now, so this was latent, not live -- but it meant the June flags weren't actually
  independently revertible. Both helpers now no-op (`UNREFERENCED_PARAMETER` on every arg) when
  `SERV_IRUHADEV_OFFLINE` is off; the Phase 5 log block is wrapped the same way.
- **The two per-frame "unsupported" logs in `OnFrameMove` were pure waste.** `APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`
  and `TRACE_OWNER_BONE_ONLY_ROTATE` are ctor-time templet facts that never change, so logging them
  every frame paid a `std::wstring` build and a `std::set` lookup forever for the same answer. Moved
  both into the ctor, beside the existing `CE_TEMPLET` dump.
- **`SetDamageDataNextByChainIndex` duplicated `SetDamageDataNext`'s whole field-carry body**
  (hit list, extra-damage flag, PvP rate, laser-group copy, Rena first-attack/skill data, struct
  copy). `SetDamageDataNext` now calls `SetDamageDataNextByChainIndex( pDamageData, 0 )` instead of
  keeping a second copy that could silently drift when Phase 8 adds a field.
- **`FollowLine`'s nearest-line scan re-validated bounds and null-checked a line already
  guaranteed by the loop that found it.** Simplified to `if( iJuneNearestLine >= 0 )`.
- **`RANDOM_SEED` was not parse-only.** It wrote straight into the live `m_LockOnData.m_iRandomSeed`,
  which `OnFrameMove_LockOnData`'s re-target branch already reads unconditionally (pre-dating Phase
  6b) -- so any `LOCK_ON_DATA` table carrying this key would have changed lock-on targeting the
  moment it was read, despite the phase's own comment calling it parse-only. Now parsed into a new,
  inert `m_iRandomSeedFromLua` member; Phase 7b decides whether/how it feeds the live field.
- **Two issues from earlier phases, unrelated to Phase 7a, that the same review pass caught:**
  - **`~CEffect` leaked `m_vecTouchLandDamageEffect`/`m_vecHitEffectSet`** (Phase 6b, both
    `vector<T*>` allocated with `new` in their `Load*` functions, never freed --
    `m_vecDieCreateNpc` is a value type and needed nothing). Added the same free-and-clear loop
    the dtor already uses for `m_vecDamageDataNextChain`.
  - **`CalculateExtraStatValue`'s "does a dungeon-keyed band exist" scan (Lanox ExtraNPCStat,
    `X2UnitManager.cpp`) ignored level.** A keyed band that didn't cover the queried level would
    still block every generic (`DUNGEON_ID 0`) row, so the function fell through to "no band
    covers this level" and returned the raw `fValue` with `*pbEvalFailed` left false --
    indistinguishable from a real result. Now the keyed-band scan also requires `iLevel` to fall
    inside the row's range; a no-op for the one real keyed band in the shipped data (Henir,
    1-110).
  - **`HasExtraNPCStat` ignored `m_bExtraNPCStatReady`.** A `NewNPCStat.lua` that fails partway
    (elstools' documented algo-2 truncation) leaves rows already committed to
    `m_mapExtraNPCStat` even though the load is marked degraded -- exactly the hazard the comment
    above `AddExtraNPCStat_LUA`'s own map write already flags. `X2Game.cpp`'s and
    `X2BattleFieldGame.cpp`'s `bExtraNPCStatNoHardLevel` called `HasExtraNPCStat` directly with no
    readiness check of their own (unlike `GetExtraNPCStat`'s two callers in `X2GUNPC.cpp`, which
    already gate on `IsExtraNPCStatReady()`). Folded the check into `HasExtraNPCStat` itself,
    fixing both callers at once.
- **Two findings from the same pass were not acted on**, with reasons:
  - **`ReInitAtkAndDef_LUA` (`X2GUNPC.cpp`, `SERV_SECRET_HELL`) duplicates `SetHardLevel`'s
    per-level term without the PvP correction factor.** This asymmetry pre-dates every Lanox/June
    change -- March's own legacy `ReInitAtkAndDef_LUA` body has never applied a PvP correction
    either, only `SetHardLevel`'s does. `SERV_SECRET_HELL` NPCs are PvE-only, so
    `GetGameType() == GT_PVP` is never true at this function's call sites, making the missing
    factor a no-op in practice. Sharing a helper would fix a real duplication but would also erase
    a pre-existing asymmetry in the shipped client for a branch that can't be reached -- not worth
    the risk to already-play-tested spawn code for this pass.
  - **The Phase 7a edits carry no `//{{ author : date // description ... //}}` change-block
    markers.** Every phase of this rebuild (2 through 7a, all already committed) has used the
    `#ifdef SERV_IRUHADEV_JUNE_* ... #endif` block plus an inline `// June2017 FUN_xxx @addr: ...`
    citation as its change marker instead, across `X2EffectSet.cpp` and `X2DamageEffect.cpp` alike
    -- zero `//{{ }}` blocks appear anywhere in either file. Adding them retroactively to only
    Phase 7a, and not the five phases before it in the same files, would make this phase
    inconsistent with its own project rather than with CLAUDE.md's general house style.

**`SERV_IRUHADEV_EXTRA_NPC_STAT` (+ short-lived `SERV_IRUHADEV_EXTRA_NPC_STAT_DEBUG`), 2026-09-27
-- Lanox ExtraNPCStat port Phase 3 (base flag only; see `LANOX_EXTRA_NPC_STAT_PLAN.md`).**
Ports June2017's second NPC stat system, `data036/NewNPCStat.lua` + `NewNPCStatFormula.lua`,
which gives the **75** Lanox NPC IDs with no legacy `NPCStat.lua` row (gates, switch gates,
traps, regen points, geysers, secret bosses, ...) their real stats instead of spawning at 0 HP
and dying `KILL_SELF`, which was letting a sub-stage's `CC_KILL_NPC_TYPE` clear condition fire
while other monsters were still alive -- the Burned Forest gate defect the whole port exists to
fix. NPCs that already have a legacy row are untouched by this flag; overriding them with
formula stats too is `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY`, a separate Phase 4 flag —
**implemented, built, and play-tested** (see that flag's own entry below).

- **`X2Lib/X2UnitManager.h`/`.cpp`:** `EXTRA_NPC_STAT_TYPE` enum, `ExtraNPCStat`/
  `ExtraNPCFormulaVariable` structs (June's own key names; `ExtraNPCFormulaVariable` carries an
  `m_iDungeonID` beyond what Phase 3's original spec listed, per the Phase 2 ledger correction --
  needed for fidelity to June's per-dungeon exception band, though it changes no computed stat
  in the shipped data), `m_bHasLegacyNPCStat` on `NPCUnitInfo` (set in `AddNPCStat_LUA`; "no
  legacy row" is judged from this, never from `m_stat.m_nNPCUnitID`), `AddExtraNPCStat_LUA`/
  `AddExtraNPCFormulaVariable_LUA` (both drop a row missing any required key, matching June,
  and count it rather than default-filling it), `HasExtraNPCStat`/`GetExtraNPCStat`, and
  `CalculateExtraStatValue` -- a replica of June `FUN_32374ea0` using the raw Lua C API (8
  arguments; `lua_tinker::call<>()` tops out at 6), with an out-flag that tells the caller
  "the Lua call itself failed" apart from "no band covers this level" (June's own fall-through),
  so a real failure falls back to the legacy stat path instead of computing a partial/zero mix.
  `OpenNPCFiles` loads `NewNPCStatFormula.lua` then `NewNPCStat.lua` after the legacy stat file,
  degrading (feature off, game behaves as today) rather than failing the load if `Enum.lua`'s
  `EXTRA_NPC_STAT_TYPE` table, either file, or the `CalculateExtraStatValue` global is missing
  afterward. The load summary (counts of nil-`NPC_ID`/no-templet/extra-only/with-legacy rows and
  formula vars, or the degrade reason) goes to `ErrorLogMsg` at load time and is buffered to be
  re-emitted once through `CX2OfflineLog::Server` on the first `SetHardLevel` call, since the
  offline log isn't open yet while `CX2UnitManager` loads its files at startup.
- **`X2Lib/X2GUNPC.h`/`.cpp`:** `SetHardLevel` and `ReInitAtkAndDef_LUA` give formula stats
  (HP/attack/defense, plus accuracy/avoidance per Phase 0 verdict 2 in `SetHardLevel`) to NPCs
  with `HasExtraNPCStat() && !m_bHasLegacyNPCStat`, falling back to the unchanged legacy code
  path otherwise -- including when the Lua evaluation itself fails, so an extra-only NPC never
  ends up with a partial mix of extra and legacy values. Scan rate/near range come from a new
  `m_iExtraNPCStatMonsterGrade` member (June `FUN_323752f0`'s monster-grade param, 0 by default);
  the sub-stage spawn path leaves it at 0, matching June's own sub-stage Lua parser (which never
  sets grade either -- `X2DungeonSubStage.cpp`'s runtime `FetchNPCData` has no `MONSTER_GRADE`
  read, only the `X2TOOL`-only `FetchNPCData4Tool` does); `X2BattleFieldGame.cpp`'s two NPC
  creation paths (`:782`/`:987`) set it from `SHOW_BOSS_NAME`/boss-group membership right before
  calling `SetHardLevel`. `[XSTAT]`/`[NPC_ZERO_HP]` diagnostics (short-lived debug flag) log once
  per NPC ID per game through `CX2OfflineLog::Server`.
- **`X2Lib/X2Main.cpp`:** binds `AddExtraNPCStat`/`AddExtraNPCFormulaVariable` to
  `CX2UnitManager`, next to the existing `AddNPCStat` binding.
- All edits are pure insertions (`git diff --stat`: 7 files, 690 lines, 0 deletions);
  `file`/CP949-byte checks passed on every touched file (`X2GUNPC.cpp/.h`, `X2UnitManager.cpp/.h`,
  `X2Main.cpp`, plus `X2BattleFieldGame.cpp` and `KTDXLIB/Always.h`, neither of which the plan's
  Starting Context names but both carry the same non-ASCII byte class).
- **Opus `/code-review` caught three real bugs, all fixed and rebuilt before this entry was
  written:** (1) `CalculateExtraStatValue` unconditionally reset its shared `*pbEvalFailed`
  output to `false` on every call, so when `SetHardLevel`/`ReInitAtkAndDef_LUA` call it several
  times (once per stat type) into one shared flag, an early failure was silently overwritten by
  a later success -- fixed by only ever setting the flag to `true`, never resetting it, so the
  caller's own pre-loop initialization is what matters. (2) The two `#include
  "Offline/X2OfflineLog.h"` lines (`X2GUNPC.cpp`, `X2UnitManager.cpp`) were unconditional --
  harmless in practice (the header is itself empty unless `SERV_IRUHADEV_OFFLINE` is defined,
  and every actual use is already flag-guarded) but not literally revertible by undefining one
  macro -- now wrapped in `#ifdef SERV_IRUHADEV_EXTRA_NPC_STAT`. (3) `AddExtraNPCStat_LUA` wrote
  extra-only NPCs' `DEF_*`/element-resistance fields into the live `NPCUnitInfo` map as each Lua
  row was read, before the loader had confirmed the whole file loaded; a `NewNPCStat.lua` that
  fails partway through (this project has a documented history of `elstools` truncating large
  packed algo-2 files) would leave some NPCs mutated while the summary line and
  `m_bExtraNPCStatReady` both claim the feature is fully degraded/off. Fixed by moving that
  write-back into a new `ApplyExtraNPCStatResistances()`, called from `OpenNPCFiles` only after
  `m_bExtraNPCStatReady` is confirmed `true` -- `AddExtraNPCStat_LUA` itself now only populates
  the pure-data `m_mapExtraNPCStat` table, no side effects on `NPCUnitInfo`. Two further findings
  were read and deliberately left as documented gaps rather than fixed: the legacy PVP
  attack/defense dampening (`fCorrectionStat`, 0 in free/tournament channels, 0.1 elsewhere) has
  no equivalent in the new extra-stat path, since neither the ledger nor Phase 0's call-site
  table describe what June does for `ExtraNPCStat` in PVP and the 75 extra-only IDs are Lanox
  dungeon/field content not known to spawn there; and the sub-stage creation path never calls
  `SetExtraNPCStatMonsterGrade()`, leaving grade at its default 0 for those NPCs -- deliberate,
  since transcribing June's own sub-stage Lua parser (which never sets grade either) requires
  the same default, not a bug to fix.
- **Built `US_SERVICE` (KTDXLIB, X2Lib, X2) and deployed to `X2_offline.exe`** (confirmed via
  `os.stat`: 18,921,472 bytes, matching source and destination mtimes, after the review-driven
  rebuild). **Play-tested — corrected below, this was stale.** An `offline_server.log` from
  2026-09-27 05:10-05:13 confirms `data036.kom` is packed and the Burned Forest gate fix works
  (see the Phase 4 entry's own corroborating-log note); Phase 5's session 1 (2026-09-27,
  07:10-07:20) re-confirmed it on the actually-deployed post-review exe.
- **Files packed into `data036.kom`** (staged in Phase 1): `NewNPCStat.lua`,
  `NewNPCStatFormula.lua`, and the updated `Enum.lua`. Confirmed packed from the
  `[EXTRA_NPC_STAT] loaded 1693 rows ...` summary line in `offline_server.log`, not from the code
  compiling.

Revert: undefine `SERV_IRUHADEV_EXTRA_NPC_STAT` (and `_DEBUG` if still present). Every call site
falls back to the exact original code path; `AddExtraNPCStat_LUA`/`AddExtraNPCFormulaVariable_LUA`
stop being bound to Lua at all.

**`SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY`, 2026-09-27 -- Lanox ExtraNPCStat port Phase 4
(nested inside `SERV_IRUHADEV_EXTRA_NPC_STAT`; see `LANOX_EXTRA_NPC_STAT_PLAN.md`).**
Makes the **772** NPC IDs that DO have a legacy `NPCStat.lua` row use June's formula stats
too -- June's own priority rule (an `ExtraNPCStat` row always wins, legacy row or not).
Every edit is a pure insertion (`git diff --stat`: 6 files, 242 lines, 0 deletions);
`file`/CP949-byte checks passed on every touched file (`X2GUNPC.cpp/.h`, `X2UnitManager.cpp`,
`X2Game.cpp`, `X2BattleFieldGame.cpp`, `KTDXLIB/Always.h`).

- **`SetHardLevel`** (`X2GUNPC.h`/`.cpp`) gained a 4th parameter, `bool bNoHardLevel = false`
  (June `FUN_327687b0`'s own extra argument; the default keeps every un-migrated call site's
  behavior unchanged). The condition that gates the whole extra-stat block drops its
  `!m_bHasLegacyNPCStat` requirement under parity, so any NPC with an extra row qualifies.
  When `bNoHardLevel` is false, the legacy `m_stat.m_HardLevel.*` rate (times `hardLevel *`
  the existing PvP correction) is added on top of the formula base values before
  `SetNPCStat` -- for the base flag's 75 extra-only NPCs this is always +0, since no legacy
  row ever set their `m_stat.m_HardLevel.*` rates (not to be confused with `CX2GUNPC::m_HardLevel`,
  the spawn level -- that's always set), so base-flag-only behavior is unchanged bit-for-bit.
  The last `bNoHardLevel` a spawn was given is remembered in a new member,
  `m_bExtraNPCStatNoHardLevel`, because `ReInitAtkAndDef_LUA` (`SERV_SECRET_HELL`, attack/
  defense only) has no parameter of its own and needs the same switch.
- **`SetNpcHardLevel`** (`X2Game.cpp:18149`) computes
  `bExtraNPCStatNoHardLevel = HasExtraNPCStat(...)` once per call and passes it only into the
  **default dungeon branch**'s five `SetHardLevel` calls (Henir, Secret, and the
  Normal/Hard/Expert difficulty switch) -- the defence-dungeon case and the "other
  difficulty" fallback keep the default `false`, exactly matching Phase 0's call-site table
  (June clears the flag in both). `X2BattleFieldGame.cpp:999`
  (`CreateBattleFieldNpcByCreatedInfo`) now passes `HasExtraNPCStat` too (June
  `FUN_32d539c0`); `:788` (`CreateBattleFieldNpc`) is untouched, since June `FUN_32d528f0`
  always passes `false` there.
- **`ApplyExtraNPCStatResistances`** (`X2UnitManager.cpp`) drops its "skip if legacy row"
  guard under parity, so `DEF_*`/element resistances are copied onto overridden IDs too
  (June's builder, `FUN_323752f0`, always overwrites them). The legacy `DAMAGE_TYPE` is left
  alone either way -- June's binding never reads that key (Phase 0 verdict 3), and this
  function never touched it to begin with.
- **Parity widens an existing, already-documented cost beyond Lanox.** Monster grade
  (`m_iExtraNPCStatMonsterGrade`) is only ever set on the two battlefield creation paths;
  the dungeon sub-stage spawn path never calls `SetExtraNPCStatMonsterGrade`, so it stays
  at its default 0 there (see the plan's "Monster grade never reaches dungeon NPCs
  offline" out-of-scope item, which previously scoped this to Lanox's 75 extra-only IDs
  only). Under parity, every one of the **772** override IDs' dungeon spawns now goes
  through the same default-0 path too, so any old-content dungeon boss with a legacy row
  gets scan values 10/500 instead of its legacy `SCAN_RATE`/`SCAN_NEAR_RANGE` the moment
  parity is on. Not fixed here; folded into Phase 5's go/no-go inputs.
- **NaN guard, found during this phase, not itemized in the original spec.** Phase 2's
  balance pass had flagged 84 rows (Secret Normal/Hard/Expert below level 70) where June's
  own `GetHPFactorByGameType` formula raises a negative base to a non-integer power --
  real NaN, not a transcription bug -- and warned that parity would be the first flag to
  actually reach them (the base flag's 75 extra-only IDs don't include any of those rows).
  `CalculateExtraStatValue` now treats a non-finite Lua result as an eval failure
  (`_finite() == 0`): it logs a `[EXTRA_NPC_STAT] non-finite result` line and returns the
  raw legacy value with `*pbEvalFailed = true`, so the caller falls back to the legacy stat
  path exactly as for a Lua pcall error, per the plan's fallback rule (today's value, logged
  as open) rather than guessing a Secret-dungeon classifier fix. This still leaves open
  whether June's own Secret switch (`FUN_32931950`) actually reaches those tiers --
  unresolved, tracked in the plan's ledger and "Out of scope" section.
- **`[XSTAT]` diagnostics** (still `SERV_IRUHADEV_EXTRA_NPC_STAT_DEBUG`, still short-lived):
  the extra-branch line now prints the actually-applied `noHL` and dedups on `(id, noHL)`
  instead of `id` alone, so a battlefield ID spawned via both `:788` (`noHL=0`) and `:999`
  (`noHL=1`, when it has an extra row) logs both instead of just the first. A new debug-only
  `m_iExtraNPCStatDebugPath` member (set to `782`/`987` at the two battlefield call sites,
  `0`/unset elsewhere) tags which path produced the spawn. A **new legacy-path `[XSTAT]`
  line** was added (Phase 3 never had one, so `src=legacy` was previously unobservable in
  the log) -- it always prints `noHL=0`, since the legacy path unconditionally adds the full
  `HARD_LEVEL` term regardless of what any caller passed.
- **Byte-fidelity defect found and fixed before any build.** The first pass wrote every
  inserted line with a bare `\n` join, which is correct for `X2UnitManager.cpp` (LF-
  dominant) but produced bare-LF insertions in `X2GUNPC.cpp`/`.h`, `X2Game.cpp` and
  `X2BattleFieldGame.cpp`, all CRLF-dominant. `file` didn't catch it (it samples, not
  scans). Fixed by diffing each file against `HEAD` with `difflib.SequenceMatcher` and
  appending `\r` to every inserted line, then re-confirming the file's bare-LF line count
  matches `HEAD`'s exactly and `git diff --stat` is unchanged (same insertion counts, 0
  deletions).
- **Built `US_SERVICE` (KTDXLIB, X2Lib, X2) clean, 0 errors, with parity ON (defined by
  default) -- and separately rebuilt X2Lib+X2 with `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY`
  commented out**, to actually compile every `#else` branch this phase added (the
  preprocessor only compiles one side of an `#ifdef`, so the parity-on build alone never
  touched them). Both configurations built clean. The parity-off exe was saved off
  (`x2_parity_off.exe`, 18,922,496 bytes) for the second half of the play-test below; the
  flag was then restored, `X2Lib`/`X2` rebuilt a third time, and *that* parity-on exe
  (18,926,080 bytes) is what's deployed to `X2_offline.exe` now (source/destination
  mtimes matched via `os.stat`). **Play-tested and reviewed — corrected below, this was
  stale.** See the play-test and `/code-review` entries further down in this same block.
- **`offline_server.log` from an earlier session (2026-09-27 05:10-05:13, predating this
  phase's code) confirms two things Phase 3's own MODS.md entry left unconfirmed:**
  `data036.kom` **is already packed** (`[EXTRA_NPC_STAT] loaded 1693 rows (skipped: 836 nil
  NPC_ID, 10 no templet, 0 missing other key; 75 extra-only, 772 with legacy row) and 67
  formula vars (skipped 0)` -- the exact expected counts), and **the Burned Forest gate fix
  was in fact play-tested and works** (`npc=1694 ... src=extra noHL=0 hp=53654`, then
  `NPC uid=88 id=1694 lv=70 died: KILL_BY_USER`) -- Phase 3's Status line and this file's
  Phase 3 entry both still say "not yet play-tested," which is stale. Not corrected here;
  flagging it rather than silently fixing a different phase's entry. That same log has
  **no** `[XSTAT]` line for `NUI_MAG_NOM` (id **1680**) at all, which is exactly the gap
  this phase's new legacy-path log line exists to close -- under the base flag alone,
  overridden IDs went through the silent legacy path with nothing logged.
- **Play-tested 2026-09-27 06:19-06:21 on the deployed parity-on exe -- passed.**
  `offline_server.log` shows the expected load line (75/772/67, nothing degraded),
  **`[XSTAT] npc=1680 lv=70 dungeon=33270 src=extra noHL=1 path=dungeon/other hp=178848`**
  (Mag Nom, Burned Forest Normal -- exact match to the done-when line), the gate
  (`npc=1694`, `hp=53654`, unchanged since it has no legacy row) still dying
  `KILL_BY_USER` only after sub-stage 1's other NPCs finish, and `stage 1 loading`
  following correctly. Zero `[NPC_ZERO_HP]` lines, zero `UNHANDLED`/`EXCEPTION` packets.
  Ten `[XSTAT] ... src=extra noHL=1 path=battlefield987` lines (`npc=5154-5164`, a Lanox
  field surge) confirm `:999`'s wiring and the path tag both work -- `path=battlefield782`
  wasn't exercised this session (needs a field load where the local player doesn't already
  have a unit), not a defect.
- **Opus `/code-review` run 2026-09-27, 10 findings. Fixed 5, documented 4 as intended
  behavior/deferred decisions, skipped 1.**
  - **Fixed:**
    1. **Revert hole.** `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY` (and Phase 3's
       `_DEBUG`) were defined unconditionally in `Always.h`, but their call sites use
       APIs (`HasExtraNPCStat`, `m_bHasLegacyNPCStat`) declared only under the base
       flag -- undefining `SERV_IRUHADEV_EXTRA_NPC_STAT` alone, MODS.md's own
       documented one-macro revert, would have failed to compile. Both are now nested
       inside `#ifdef SERV_IRUHADEV_EXTRA_NPC_STAT`.
    2. **Legacy/extra mixing.** `ReInitAtkAndDef_LUA` (`SERV_SECRET_HELL`) re-evaluates
       its own attack/defense independently of `SetHardLevel` -- if a spawn's `SetHardLevel`
       fell back to legacy (e.g. a NaN HP eval, see below), a later `ReInitAtkAndDef_LUA`
       call could still succeed (attack/defense never go NaN) and apply formula atk/def
       on top of legacy HP, mixing the two systems the "never mix" rule was meant to
       prevent. Fixed with a new member, `m_bExtraNPCStatAppliedAtSpawn`, set from
       `SetHardLevel`'s own `bAppliedExtraStat` and checked by `ReInitAtkAndDef_LUA`'s
       PARITY condition.
    3. **`[XSTAT]` dedup gap.** Deduping on `(id, noHL)` alone still collapsed a
       battlefield ID spawned via both `:788` and `:999` when they happened to produce
       the same `noHL` -- the path tag existed but couldn't do what its own header
       comment claimed. Now dedups on `(id, noHL, path)`.
    4. **NaN log spam.** The non-finite guard logged on every affected spawn with no
       throttle -- a Secret wave of 20 mobs would write 20 identical lines. Now throttled
       to once per `(statType, level, dungeonID)` via a static set, matching this
       feature's other diagnostics.
    5. **Comment accuracy.** "`m_HardLevel is always 0`" was wrong -- `m_HardLevel` is
       the spawn level (always set); what's zero for the 75 extra-only NPCs is
       `m_stat.m_HardLevel.*`, the rate fields no legacy row ever populated. Fixed in
       both the code comment and this file's own wording above.
  - **Documented, not fixed (all confirmed as intended behavior or an already-recorded
    deferred decision, not bugs):**
    - **`ApplyExtraNPCStatResistances` overwrites DEF_*/resistances for all 772 override
      IDs, including a spawn whose per-spawn HP/atk/def eval later falls back to legacy.**
      This is spec item 7 working as specified (June's builder does the same), not a
      defect -- but it does mean a "fell back to legacy" spawn isn't byte-identical to
      legacy: HP/attack/defense are legacy, DEF_*/resistances stay June's either way.
      Reflected in the NaN guard's own comment and log line (now says "falling back to
      legacy HP/attack/defense", not "legacy entirely").
    - **The NaN guard could in principle reintroduce the 0-HP stage-skip bug for an
      extra-only NPC if it ever hit a NaN eval** (its "legacy" fallback is an empty,
      unset `m_stat`). Checked against the ledger rather than fixed blind: the audit
      table says 0 non-Lanox dungeon NPC IDs lack a legacy row, and the NaN-affected
      tiers are Altera/Besma/Elder Wally Secret only -- none of Lanox's 75 extra-only
      IDs are reachable there, so this is statically unreachable in the shipped data.
      If that ever turns out wrong, it's loud: both the non-finite log line and
      `[NPC_ZERO_HP]` fire, rather than a silent NaN passing through. Folded into the
      NaN interim the user still has to confirm (see Phase 2's ledger note).
    - **Scan grade (10/500 vs 50/1000) now applies to all 772 override IDs' dungeon
      spawns, not just Lanox's 75.** Already recorded above and in the plan's
      "Monster grade never reaches dungeon NPCs offline" out-of-scope item -- a known,
      deferred cost, not new.
    - **PvP/summon base stats scale with level under parity even in free/tournament
      channels**, where the legacy `fCorrectionStat=0` was meant to zero out the
      `HARD_LEVEL` term specifically. Checked against the ledger's `FUN_327687b0` row:
      the correction only ever scaled the `HARD_LEVEL` term, never the formula base
      (which June's own builder, `FUN_323752f0`, computes independent of PvP context) --
      so a level-scaled base for PvP summons with an extra row is June's own behavior
      under parity, not a bug introduced here.
  - **Skipped:** de-duplicating the `fCorrectionStat`/`HardLevel`-term computation that
    now appears in `SetHardLevel`'s extra branch, its legacy branch, and
    `ReInitAtkAndDef_LUA` -- a shared helper would mean editing the studio's legacy
    block, which breaks the 0-deletions convention these edits otherwise hold to.
  - All five fixes are pure insertions or same-session edits to this phase's own
    uncommitted lines (never to stock code); `git diff --stat` against `HEAD` still
    shows 0 deletions in every source file.
- **Re-verified after the review fixes:** rebuilt parity-off (replacing the earlier
  `x2_parity_off.exe`, now 18,922,496 bytes) and parity-on again, both clean, 0 errors;
  the parity-on rebuild (18,926,080 bytes) is deployed to `X2_offline.exe`, source/
  destination mtimes matched via `os.stat`. The parity-on play-test from 06:19-06:21
  still stands, since none of the five fixes change any value `SetHardLevel` produces for
  a spawn that doesn't hit the NaN/mixing edge cases.
- **Still owed:** the parity-OFF half of the exit criterion (`src=legacy`, on the
  rebuilt `x2_parity_off.exe`, not yet deployed/tested); a `battlefield782` observation.

**Play-tested 2026-09-27 07:10-07:20 (Phase 5 session 1, on the deployed post-review
parity-on exe) -- passed for everything it covered.** Zero `[NPC_ZERO_HP]`, zero
`UNHANDLED`/`EXCEPTION`. Burned Forest Normal full clear (stage 0 through 3), gate
`npc=1694` reused across stage 0 sub 1 and stage 1 sub 2, both times dying `KILL_BY_USER`
only after the sub-stage's other mobs; all four Ranox battlefields visited, including
`Ranox_Field_01.lua`'s Geyser zone (7 of 8 `NUI_LOW_GEYSER_*_FD` observed `src=extra`, no
`KILL_SELF` -- this closes Phase 7d's "unsourceable" bucket, see
`LANOX_DEFECT_PHASES.md` Phase 10); Ashes Village Normal partially cleared (left by
choice, not a skip). **Not yet covered:** Geyser/Gateway/FireTemple/FinalHall/
Secret_Hell dungeons, every Hard/Expert tier, the parity balance comparison against
Phase 2's table, and the keep/revert decision. Full detail and checklist in
`LANOX_EXTRA_NPC_STAT_PLAN.md` Phase 5, whose Status line still reads "in progress" --
this entry summarizes, it does not close, that phase.

**Phase 6 close-out, 2026-09-27.** `SERV_IRUHADEV_EXTRA_NPC_STAT_DEBUG` disabled
(`#define` commented out, `Always.h`, still nested under the base flag with a
re-enable-to-diagnose note) since Phase 5 asked for it removed once verification was
done -- **it wasn't**, so this is a documented deviation, not a completion: re-enable
the one `#define` to resume Phase 5's `[XSTAT]`/`[NPC_ZERO_HP]` logging. Rebuilt
`US_SERVICE` (X2ServerProtocol, KTDXLIB, X2Lib, X2) with the flag off and deployed;
string-count control reading before/after confirms `[XSTAT]`/`[NPC_ZERO_HP]` compiled
out and `[EXTRA_NPC_STAT]` (the always-on load/degrade/NaN lines) still present. Ask
before launching, as always. **The parity keep/revert decision is not made.** Both
flags are defined by default in the deployed exe; the choice is the user's, informed by
Phase 2's balance table and whatever of Phase 5's checklist gets finished.

Revert: undefine `SERV_IRUHADEV_EXTRA_NPC_STAT_PARITY` alone to put overridden NPCs' stats
back to today's legacy numbers while keeping the base flag's stage-skip fix; undefine both
flags together to revert the whole ExtraNPCStat port.

### June2017 effect engine: DamageEffect lock-on runtime (Phase 7b)

Phase 7b of `JUNE2017_EFFECT_REBUILD_PLAN.md`. It extends **`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`**
(no new flag) in `X2Lib/X2DamageEffect.h`, `X2Lib/X2DamageEffect.cpp` and, for one hook,
`X2Lib/X2DamageManager.cpp`. `SERV_IRUHADEV_JUNE_FX_DEBUG` gains a `JUNE_FX CE_LOCKON <effect>
<branch> <detail>` line (throttled to one a second per effect and branch, in `offline_server.log`).
Identification was finished on Opus in the same session. Every function and offset is in
`june2017_re/funcmap.tsv` (rows `31f73a10` … `32481f80/32482650`, 18 of them).

**What June's lock-on runtime turned out to be:**
- `OnFrameMove_LockOnData` `31f83aa0`: jump table `31f858e4`, 8 cases. `VectorLockOn` is
  `31f73a10`.
- The kickoff's `SetLockOnNearstTarget` is `31f758d0`. It is **unchanged** from March `:8760`
  (nearest pick `322d10b0`, then UID and seed).
- The three new pick routines are June `CX2Game` methods with no March counterpart:
  - `322dadd0`: the seeded pick.
  - `322d7670`: the in-range nearest pick, with a target list.
  - `322daa10`: the random in-range pick.

  They are re-expressed as `CEffect` helpers over `g_pX2Game->GetUnitList()` (public, and March
  never takes the unit-list lock), so `X2Game` is untouched.
- The list that `NOT_TARGET_PRE_LOCK_ON_TARGET` works on is **per effect**, and it is filled on
  **hit**. June's `EffectToUnit` `3262b6e0` inserts every unit the effect hits, and nothing else
  does.

**Transcribed, each with its June range in a comment:**

| Key / branch | June | What changes |
|---|---|---|
| `LOCK_ON_RANGE_FROM_OWNER` + `AT_*` | `31f71930@31f73130-31f73540`, gate `31f647c0` | Picked at load by state: `AT_PVP` in `XS_PVP_GAME`; `AT_RAID_FIELD` / `AT_FIELD` in `XS_BATTLE_FIELD`; `AT_DUNGEON` otherwise. With no key, a non-raid battle field gets **1000**, everything else 99999. `VectorLockOn` steers nothing, and the picks drop any candidate, whose aim point is farther from the owner than that range. |
| `LOT_UID_VECTOR` re-target | `31f8407a-31f84489` | June's seeded pick replaces `CX2Game::GetLockOnTarget`. The index is `(seed % 30 + 1) % eligible`, over owner-range-filtered candidates. **March's walk lands one unit off, and at the default seed −1 it finds nobody; June picks index 0.** |
| `NOT_LOCKON_INVINCIBLE_TARGET` | `31f63d80` | Case 4 also re-targets when the live target is invincible. |
| `NOT_TARGET_PRE_LOCK_ON_TARGET` (int type) | `322d7670`, `3262b6e0` | Case 6 uses the effect's own hit list. The deployed `TRUE` is 1 = `NTPLOTT_TARGET_LIST_CLEAR`. <br>On a miss: CLEAR empties the list; SAVE_ONE empties it only when it holds more than one; SAVE_HIT_UNIT also skips its last pick (a studio quirk is kept: its retry result is discarded). <br>Types 2–4 steer one `DirSpeed` ahead on a miss (`31f8497c`). <br>This replaces March's game-wide pick list (`BALANCE_DEADLY_CHASER_20130214`, kept in `#else`). |
| `DUNGEON_/PVP_TARGET_RANGE_MIN_MAX` | case 6 / 8 | Replace March's hardcoded 150/750 and 150/300, which are their defaults. |
| `RETARGET_COOLTIME_AFTER_ATTACK` | `31f83c2c`, set `@31f851c6` | After a hit-count re-target, no case runs until the cooldown ends. |
| `UNLOCK_START_AFTER_ATTACK` | `31f83c0d` | No steering until the effect has hit that many times. |
| `TIME_UNLOCK_FROM_START` | `31f83b70` | As March, plus the ONLY_FRONT origin is refreshed to the effect's own position when it expires. **This reaches live data:** `DamageEffect_ENS_HA_Machine_Gun_Bullet` and `DamageEffect_Oberon650_AngelHeart_(Jump_)Attack_Z` set both keys (checked against the packed Sep 27 `DamageEffect.lua`). |
| `Y_DIR_INIT` | `31f73d77` | The heading's y is negated once (`fmul QWORD −1.0`, verified from raw bytes). |
| `RANDOM_SEED` | `31f7355c` | **One member in June.** The Lua value now feeds `m_iRandomSeed` at load, which reverses Phase 6b's split. Load runs only from the ctor, so a seed set by skill code still wins. |
| `LOCK_ON_TARGET_BONE_NAME` | `31f73109` | Feeds `m_wstrTargetBoneName`, which March never assigned, so `LOT_UID_BONE_POS` always fell back to `GetPos`. |
| `INIT_TARGET` | `31f73732` | An immediate seeded pick at load. |
| `LOT_UID_VECTOR_AFTER_ATTACK_IN_RANGE` (= 8, appended) | `31f844b1-31f84a0e` | New case. It aims at the target's slot-`0x3e4` point seen from `m_Pos`, plus `LOCK_ON_OFFSET_Y_POS`, and re-targets randomly around it. |

Cases 1–3, 5 and 7 are the same as March. The body is re-written under the flag, and March's
original sits whole in `#else`.

**Not ported, recorded:**
- The NPC byte `+0x8fb6` that every June pick also filters on. It is not identified.
- The battle-field tail of the NPC aim point (`3275ae4f-3275b01d`): unidentified mode tests
  `3266ce90`/`3266ce80`.
- June's `XS_PVE_GAME` branch (`+0x9d4`, mesh removal). March has no such state.
- The raid-field test is re-expressed as `GetIsBossRaidCurrentField()`. That mapping is a
  candidate only.

**Bonus identification for Phase 7d:** `31f85910` is almost certainly `CEffect::AttackResult`
(the last call in `EffectToUnit`, the same place as March `X2DamageManager.cpp:1900`).

**Built `US_SERVICE` (full X2Lib rebuild after `touch stdafx.cpp`, because the `LockOnData` layout
changed) and deployed to `X2_offline.exe`, 18,943,488 B.** The new log strings were confirmed in
the exe. `census.py` passes all checks, the matrix gains 24 `7b` runtime rows, and the Phase-6b row
counts are unchanged.
- This run wrote its preprocessor cache to the scratchpad. `june2017_re/pp/X2BuffBehaviorTemplet.i`
  is locked by another process, probably an MCP server's indexer.
- **Play-tested 2026-09-27 15:30-15:32**, battle fields only. The shutdown was clean, and there were
  no `UNHANDLED` or `EXCEPTION` entries.
  - **`Damage_Chung_CDC_Bullet_Blitz`** (field 40025) exercised the whole target-list cycle:
    - `TARGET_LIST_INSERT list=1 uid=4 set=1`. The hit was recorded.
    - `IN_RANGE_PICK … skipped=1 … uid=2 dist=571`. The unit already hit was skipped and the next
      one picked.
    - `IN_RANGE_MISS set=4 skipped=4`, then `TARGET_LIST_MISS_RULE list=1 set=0`. Every candidate
      had been hit, so CLEAR emptied the list.
    - Then the cycle started again (`uid=5` inserted, `uid=3` picked).
  - **`GUIDED_ARROW_MEMO`** (field 40032, `LOT_UID_VECTOR`): 9 `RANGE_FROM_OWNER_REJECT range=1000
    dist=1002..2881` lines. With a lock-on table and no `LOCK_ON_RANGE_FROM_OWNER`, June stops
    steering in a battle field once the target is more than 1000 from the owner. This is the live
    change listed above.
  - **Not exercised:** the `LOT_UID_VECTOR` re-target (`UID_VECTOR_RETARGET` / `SEED_PICK`), which
    needs a guided projectile's target to die mid-flight, and any dungeon run.
- The keys no deployed `LOCK_ON_DATA` carries (`INIT_TARGET`, `Y_DIR_INIT`,
  `UNLOCK_START_AFTER_ATTACK`, `RETARGET_COOLTIME`, bone name, `LOCK_ON_OFFSET_Y_POS`, case 8) get
  their log check at Phase 11.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`. Every site keeps March's code in
`#else` or is a pure addition. The `EffectToUnit` hook is two added calls.

**`/code-review` (high), 2026-09-27.** Ten findings, each checked against the June disassembly.
The three fixes were rebuilt (KTDXLIB, X2Lib, X2) and redeployed to `X2_offline.exe` at 15:53.

Fixed:
- **The `EffectToUnit` hook now checks `IsLiveInstance( pAttacker )` first.** `DamageReact` runs
  before the second site and may remove the effect; the studio's own `AttackResult` call just below
  re-checks for the same reason. June does not check here. The guard changes nothing while the
  effect is alive.
- **The `_IN_HOUSE_` "NOT_TARGET_PRE_LOCK_ON_TARGET is for IN_RANGE only" message box** no longer
  fires for `LOT_UID_VECTOR_AFTER_ATTACK_IN_RANGE`, which reads that key legitimately
  (`322daa10`). This is the one modified line: the condition gains a flag-gated `&&` clause.
- **`Always.h`'s description of the flag** now covers the lock-on work. The kickoff chose to
  extend the 7a flag rather than add one, so reverting it reverts both phases.

Kept, because June does the same:
- Case 8 aims from `m_Pos` (June `+0x100`), which only the ctor and `SetPos` write (`31f73df0`,
  `31f7aec7`).
- The random pick uses CRT `rand()`, as `322daa10` does. That could desync P2P clients; it is
  studio behaviour.
- The forward steer runs after `NOT_FIND_TARGET_MOVE_STOP` has zeroed DirSpeed, so it aims at the
  effect's own position. June reads `+0x5cc` after that same store.
- The case-6 hit-count re-target writes one UID slot without clearing the other, in both June and
  March.

Kept, not changed:
- The per-effect hit list replaces March's game-wide pick list. That is June's design, and the
  Bullet Blitz play-test still spread across targets.

Noted for later:
- The aim point (slot `0x3e4`) is computed for every candidate even at range 99999. It is only
  costly on June data (Phase 11).
- The target-lookup, owner-fallback and DirSpeed blocks are repeated across the cases.

### June2017 effect engine: CREATE_DAMAGE_EFFECT loop + die-condition runtime (Phase 7c)

Phase 7c of `JUNE2017_EFFECT_REBUILD_PLAN.md`. It extends **`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`**
(no new flag) in `X2Lib/X2DamageEffect.h` and `X2Lib/X2DamageEffect.cpp`. Identified by disassembling
June's `CEffect::OnFrameMove` (`31f88790`) with a small capstone script reusing
`june2017_re/xordecrypt.py`'s `data()`/`raw()`/`disasm()` helpers, since Ghidra has no function
defined over that range (Phase 7a's finding still holds). Every address cited below was read from
that disassembly, not guessed from the studio's dead code.

**What June's CREATE_DAMAGE_EFFECT loop turned out to be:** the studio's own dead
`#ifdef LOOP_CREATE_DAMAGE_EFFECT` code at `X2DamageEffect.cpp` (never defined under `US_SERVICE`,
confirmed with `flagprobe.py`) is a byte-for-byte match for June's scheduling and termination logic --
LOOP_END_TIME expiry, `fEventTime + fLoopInterval*uiCreateCount`, and the post-spawn
`++uiCreateCount` / `uiLoopEndCount` compare are all identical. Rather than define that flag (it also
gates a duplicate, unrelated set of Lua reads at the parser), the same logic was re-written under our
own flag in the `#else //LOOP_CREATE_DAMAGE_EFFECT` branches, with `uiCreateCount` as our own
`DamageEffectData` member (the dead flag's field of the same name is unreachable here). What the dead
code never had:

| Key / behaviour | June | What was added |
|---|---|---|
| `LOOP_ADD_POS_X/Y/Z` | `31f89b9d-31f89c1f` | `vPosition += vLoopAddPos * (float)uiCreateCount`: world-space, cumulative through the spawn counter (computed fresh from the base position every spawn, not an accumulated running sum), added after the existing rotated-offset/plain-offset composition and before `CreateInstance`. |
| `SET_DIE_WHEN_MASTER_EFFECT_DIE` | write `31f89e77-31f89e95`, read `31f886c0` | The parent pushes the new child's handle onto its own list, `m_vecDieWithMasterChildEffects` (June `CEffect+0x8d4`). The one reader, `31f886c0`, is called only from June's `DestroyInstanceHandle` `31f83830` @`31f838c5`, after the die block and before the critical-loop test. It copies the list and destroys each child with `DestroyInstanceHandle( child, false )`, so the children play their own die effects. Ported into `CX2DamageEffect::DestroyInstanceHandle` at the same point. **A parent that simply runs out of life does not take its children with it** (June has no other caller); an explicit destroy does, e.g. the trap's Lua `StopEffectSet` every 5 s. |
| `INHERIT_LIFE_TIME` | `31f89dc1-31f89dfc` | Copies the parent main effect's `GetNowLifeTime()` onto the child main effect (`CKTDGXMeshPlayer::CXMeshInstance::SetNowLifeTime`, already public), so the child continues the parent's life clock instead of starting at 0. Both main effects must be live. |
| `INHERIT_DAMAGE` | `31f89cf5-31f89cfb` | Propagates the flag onto the child (`CEffect::m_bInheritDamageFromMaster` / `SetInheritDamageFromMaster`), exactly as June's own binary does at spawn time. *(Corrected by Phase 7d: the flag's readers are not `DamageReact`/`AttackResult`. They are June `31f8fdb0 @31f8feb1-31f8fecc`, which copies the parent's damage onto the child at spawn, and `SetDamageDataNext 31f799c0 @31f79b54-31f79b75`, which carries it across a `DAMAGE_DATA_NEXT` change. Both are now ported; see the Phase 7d section.)* |

**Already correct, no new code (confirmed by re-reading June and citing the live flag):**
`APPLY_ROTATED_OFFSET`/`APPLY_ROTATE_ORIGIN` (the existing rotated-offset branch, always compiled),
`AUTO_DIE` (`ADD_GAME_STAGE_DELETE_DAMAGEEFFECT`, ON), `SET_GROUP_ID`
(`SERV_RAVEN_VETERAN_COMMANDER`, ON), `INHERIT_BUFF_FACTOR` (`ADDITIONAL_MEMO`, ON). Matrix rows
updated with a `runtime 7c: same (existing, flag X ON) (done)` note rather than touching the code.

**Die conditions.** `DAMAGE_EFFECT_DIE_CHECK_CONDITION`'s consumer is `CX2DamageEffect::CEffect::
JuneEvaluateDieCheckCondition()` (June `31f765b0` the combiner, `31f76010` the per-entry check),
wired into `OnFrameMove`'s existing `IsLiveInstanceHandle`/`IsDyingByCrash` die gate as one more
`||` term. An empty `m_vecDieCheckCondition` returns `false` (verified from the binary:
`31f765ba-31f765c6` short-circuits before touching any accumulator), so effects with no
`DIE_CHECK_CONDITION` table are unaffected. Non-empty: AND of the `AND_OPERATION` entries, ANDed
with the OR of the `OR_OPERATION` entries only when at least one exists. `DEDCT_DAMAGE_TIME`
compares `m_DamageTimeNow` to `(int)CHECK_DAMAGE_TIME_VALUE` (`>` when `IS_OVER`, else `<`).
`DEDCT_LOCK_ON_TARGET` checks whether `m_LockOnData`'s unit or NPC UID still resolves to a live
unit via the same `GetUserUnitByUID`/`GetNPCUnitByUID` lookups used everywhere else in this file
(re-expressed instead of re-deriving June's raw `g_pX2Game` vtable call), compared against
`IS_EXIST`.

**Scope decision, recorded rather than dropped:** `funcmap.tsv` row `31f83380` (a `TOUCH_LAND` check
inside the same `OnFrameMove`) is tagged "Phase 7c" from Phase 7a's survey, but the kickoff's named
scope is LOOP_*/`SET_DIE_WHEN_MASTER_EFFECT_DIE`/`INHERIT_*`/`SET_GROUP_ID`/`AUTO_DIE`/rotate
offset-origin/die conditions only. Left unported, alongside `TOUCH_LAND_DAMAGE_EFFECT`'s own
runtime, for a future phase. *(Now Phase 7e, added 2026-09-29.)*

**Named test case:** `DAMAGE_EFFECT_Trap_Fire_Big` (deployed `data036_kom/DamageEffect.lua`, spawned
by the `NPC_TRAP_FIRE_BIG` templet's `EffectSet_Trap_Fire_Big`). All four `CREATE_DAMAGE_EFFECT0-3`
entries already carry `LOOP_INTERVAL=0.05`, `LOOP_END_COUNT`/`LOOP_END_TIME` (5/3, 7/3, 6/3, 6/5),
`LOOP_ADD_POS_X`/`_Y`, `SET_GROUP_ID=1` and `SET_DIE_WHEN_MASTER_EFFECT_DIE=TRUE` -- no Lua packing
needed for this phase. `SERV_IRUHADEV_JUNE_FX_DEBUG` logs one `JUNE_FX CE_LOOP` line per spawn with
the templet name, spawn index/`LOOP_END_COUNT`, the computed position, `LOOP_ADD_POS`, and the three
new flags.

**Built `US_SERVICE`** (full `X2Lib` rebuild after `touch stdafx.cpp`, header layout changed) **and
deployed to `X2_offline.exe`, 18,942,976 B**, 2026-09-27. `X2Lib`, `X2` both built with 0 errors.
**Play-test pending** -- ask before launching, every time.

**`/code-review` (high, Opus), 2026-09-27.** Nine findings, each checked against the June
disassembly. Three fixed, rebuilt and redeployed:
- **Hang: the first version drained the child list from `~CEffect`.** `~CX2DamageEffect`,
  `DestroyAllInstances` and `UnitDeleteProcess` walk `LIST_LIVE` with a saved `iterNext` and
  `SAFE_DELETE` directly, outside the critical loop. A child destroyed from the parent's dtor was
  spliced into `LIST_FREE`; if it was the saved `iterNext`, the walk ran into the circular free list
  and never ended (unit removal, stage change, exit). Finding the June reader (`31f886c0`, a full
  scan of `31f50000-31f90000` for `+0x8d4`) settled it: the drain belongs in `DestroyInstanceHandle`,
  which none of those loops call. `~CEffect` no longer touches the list.
- **Children died silently** (`bSilently = true`). June passes 0 (`31f88716`), so their
  `DIE_DAMAGE_EFFECT`s now play.
- **`INHERIT_LIFE_TIME`** reads the parent clock from the `pMeshInst` already in scope.

Added while fixing: `m_coInstanceHandleList` stores in a `std::vector`, and the children's die
effects can create instances, so after a non-empty drain `DestroyInstanceHandle` finishes through a
fresh lookup (`DestroyInstanceHandle( hInstance, true )`) rather than trust the `info` reference.
The studio's own `SetDieDamageEffect` call just above has the same exposure; that is unchanged.

Kept, because June does the same:
- One spawn per entry per frame (`31f89f27` moves to the next entry after a spawn). An interval
  shorter than a frame falls behind, and `LOOP_INTERVAL = 0` with `LOOP_END_COUNT > 1` spawns once,
  then waits for `LOOP_END_TIME`: the check time never moves. June's timer `31c82d60` is
  `EventTimerGlobal` (the `ELSWORD_SHEATH_KNIGHT` window, `before <= t < global`).
- `DEDCT_LOCK_ON_TARGET` counts a target that still resolves by UID, dying or not (`31f760b2`: a
  null test only).
- The child list only grows (one push per spawn, never pruned); it goes with the parent.

Kept, not changed:
- The duplicated scheduling/counter blocks next to the studio's dead `LOOP_CREATE_DAMAGE_EFFECT`
  ones: merging them would edit studio `#ifdef` lines.
- A child-polls-master design: June has the parent destroy its children.

Noted: `GetInheritDamageFromMaster` had no reader yet. *(Phase 7d: `m_bInheritDamageFromMaster` is now read by `SetDamageDataNextByChainIndex`, and the spawn-time damage copy sits next to `SetInheritDamageFromMaster( true )`. It is not on the hit path.)*

Not a 7c defect, recorded: in Burned Forest (33270) the `NPC_TRAP_FIRE_BIG` (1703) is placed
(stage 1 sub 2, `INACTIVE`) but its `EffectSet_Trap_Fire_Big` never plays, so
`DAMAGE_EFFECT_Trap_Fire_Big` is never created and the named test cannot run there. Other NPC
effect sets play in the same run. Out of this plan's scope; the user chose to skip it.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`. Every site keeps March's existing
behaviour in `#else` (the LOOP scheduling/termination edits nest inside the studio's own dead
`LOOP_CREATE_DAMAGE_EFFECT` `#else`) or is a pure addition (`m_vecDieWithMasterChildEffects`,
`m_bInheritDamageFromMaster`, the die-check `||` term, `JuneEvaluateDieCheckCondition` itself).

### June2017 effect engine: DamageEffect hit-response runtime (Phase 7d)

Phase 7d of `JUNE2017_EFFECT_REBUILD_PLAN.md`. It extends **`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`**
(no new flag) in `X2Lib/X2DamageEffect.cpp` and `X2Lib/X2DamageManager.cpp`; `KTDXLIB/Always.h`
changes only in the flag's description. Diagnostics use a new `JUNE_FX CE_HIT` line, throttled to
one a second per (effect, branch), behind `SERV_IRUHADEV_JUNE_FX_DEBUG`. No header changed.

**Identification** (Ghidra `decompile_function` plus capstone scans; all in `june2017_re/funcmap.tsv`):
- June `CEffect::AttackResult` is `31f85910`. Phase 7b had it as a lead.
- `CEffect::DamageReact` is `31f6f600`. Its one caller is UnitToEffect's per-effect body `326413c0`.
  The body is March's line for line, with no June key and no behaviour change.
- The hit spawner is `31f810c0`, behind `SetHitDamageEffect` `31f831e0`, which AttackResult calls.
- The die spawner is `31f82210`, behind the die wrapper `31f83400`, which `DestroyInstanceHandle`
  calls.
- Each key's reader was found by scanning `.text` for the CEffect offset the ctor stores it at.

| Key | June | What changed |
|---|---|---|
| `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT` | `31f85910 @31f881b6-31f881fe` | In `AttackResult`, right after `m_DamageTimeNow++` and before the `DAMAGE_TIME` die test: when the rate is `>= 0`, the main effect's now-life = max-life × rate. 0 restarts the clock; the default −1 does nothing. The mesh offsets were confirmed against June's `CXMeshInstance` life update (`31c82fbf`) and ctor (`31c8d44d`). |
| `INHERIT_CRITICAL_RATE` / `INHERIT_CRITICAL_DAMAGE_RATE` | `31f8af50`, called `@31f81987` (hit) / `@31f82f36` (die) | In `CreateHitDamageEffect` / `CreateDieDamageEffect`: the child's `m_fCriticalRate` / `m_fApplyCriticalDamage` **gain** the parent's (`+=`). The June DamageData offsets map to `CRITICAL_RATE` / `APPLY_CRITICAL_DAMAGE_PERCENT` (`32632390@3263c197`/`@3263e629`). June does **not** do this for CREATE_DAMAGE_EFFECT loop children or `RT_CREATE_DAMAGE_EFFECT`. It does do it for touch-land children, whose runtime is Phase 7e's. |
| `INHERIT_LIFE_TIME` (hit / die scopes) | `31f810c0 @31f819bd-31f819fe`, `31f82210 @31f82f74-31f82fb5` | The child's main effect takes the parent's now-life, as Phase 7c did for the loop. |
| `INHERIT_DAMAGE` | `31f8fdb0 @31f8feb1-31f8fecc`; `31f799c0 @31f79b54-31f79b75` | Phase 7c's handoff was wrong about where this key is read (corrected in its section and in the code comment). At spawn, right after `SetInheritDamageFromMaster( true )`, the child's `damage.fPhysic` / `fMagic` are set to the parent's. `SetDamageDataNextByChainIndex` then keeps that damage across a `DAMAGE_DATA_NEXT` change. Only the CREATE_DAMAGE_EFFECT loop sets the flag. |
| `ONLY_RELATIONSHIP` | `3262b6e0 @3262ba35-3262ba49` | In `EffectToUnit`, a defender that is not `GUT_USER` is skipped before March's relationship test. March `static_cast`s any defender, NPCs included, to `CX2GUUser` and reads a relationship UID it doesn't have. The rest of the test is March's. |
| Hit/die `SCALE_X/Y/Z` (existing keys) | `31f810c0 @31f81a4f-31f81a90`, `31f82210 @31f82fb5-31f83010` | March parsed each entry's scale into `DamageEffectData::vScale` and never used it. June's `SetScale` (`31f6fdc0`) gets `entry scale × parent hit (or die) damage-effect scale`, per component. Replaces `pEffect->SetScale( m_vHitDamageScale / m_vDieDamageScale )`, which stays in `#else`. |

The `DAMAGE_DATA_NEXT` carry writes into `pDamageDataNext`. That object is per instance, not shared
templet data: every `CEffect` is `new`'d (`X2DamageEffect.cpp:996`/`:1116`), and its own ctor
allocates `m_DamageEffectDataInLua.m_pDamageDataNext` (`:2004`), which its dtor frees. So an
inherited value cannot leak into later instances of the same effect.

**Classified, no code:**
- `REACT_ANIMSPEED`: `same`. `@31f86ad5` is March's `SKILL_CASH_10_TEST` block (ON).
- `USE_ADVANCE_COLLISIOIN_CHECK`: `deferred`. It is read only inside EffectToUnit's inlined
  collision test (`@3262e976`), so it is handed to Phase 9 with what it needs; see Phase 9's section
  of the plan.
- AttackResult's other June differences read DamageData keys: `CREATE_EFFECT_SET_*`,
  `TRACE_NEAREST_BONE`, `MAKE_TARGET`. They are handed to Phase 8b as a table in its section.

**`census.py` now holds Phase 7c's runtime notes.** Phase 7c had typed its 20 matrix notes by hand
into a file that `census.py` regenerates, and the next rerun erased them. They are now `RT_7C`
rows in `RUNTIME`; INHERIT_DAMAGE's became a `7d` row. The status counts are unchanged, and all
checks pass.

**What the deployed data reaches:**
- **The new branches cannot fire on it.** The packed Sep 27 `DamageEffect.lua` (`KL\x81`, plain
  constants) has no `RESET_MAIN_EFFECT_LIFE_TIME_RATE_BY_HIT`, `INHERIT_CRITICAL_*`, `INHERIT_DAMAGE`
  or `INHERIT_LIFE_TIME`.
- **Two changes do reach live data:**
  - `ONLY_RELATIONSHIP`: 1 use.
  - The scale change. **This is a visible change.**
  - **Correction (2026-09-29): 30 entries, not 13.** The first count skipped values like `1.5`
    because its filter only checked that the number started with `1`. Recounted: 8 hit and 22 die
    entries have a non-1 scale.
    - Hit:
      - Elesis `SPK_DashJump_Combo_XZ` and `SBH_DashJump_Combo_XZ` `_Explosion_Air` at 1.5;
      - `SBH_Dash_Combo_X/XX/XXX_Explosion` at 0.75;
      - `SA_APT_Conqueror_Projectile_Shock` at 1.1;
      - the monster `NUI_SMITH_RUNNER_Attack_A_EXPLOSION` at 1.3.
    - Die:
      - `ENS_HA_Scud_Missile_Explosion` at 1.5;
      - Ara `ASD_Dragon_Falling_Explosion` at (2, 1, 2);
      - `SBH_Fire_Works_Dying_Shrapnel` at 0.66;
      - Ara `SA_ALD_WOLF_Break` explosions at 0.5, 1.5 and 3;
      - `SA_APT_Conqueror_Projectile_Shock` at 1.1;
      - the monster `VKC_EXPLOSION` and `NUI_SMITH_ALUMINUM` / `RUNNER` explosions at 1.3.
    - Most are **above** 1, so most of these explosions get bigger, and that includes some
      monster attacks.
  - **March did not draw these at 1×.** It drew them at the parent's hit / die scale, and June
    multiplies the entry scale into it. In the play-test the parent was 2×, so the SPK air
    explosion went from 2× to 3×.
  - The one `ONLY_RELATIONSHIP` user needs a relationship partner, which offline play probably
    lacks.

**Built `US_SERVICE`** (`touch X2Lib/stdafx.cpp` first; X2ServerProtocol, KTDXLIB, X2Lib and X2,
all 0 errors) **and deployed to `X2_offline.exe`, 18,947,072 B**, SHA-256 prefix `bc99dc05a2eb4ebd`,
2026-09-28.

**Play-tested 2026-09-29** (Elesis and Ara skills, battle field, clean shutdown).
- **The hit scale branch fired.** It logged 7 lines, e.g.
  `JUNE_FX CE_HIT DamageEffect_Elesis_SPK_DashJump_Combo_XZ HIT_DAMAGE_EFFECT_SCALE
  …_XZ_Explosion_Air entry=(1.500,1.500,1.500) parent=(2.000,2.000,2.000)`: the child drew at 3×.
- **The die scale branch did not fire.** The Ara skill used was `EffectSet_SA_AYR_ORB_KI_FX_DAMAGE`,
  not Dragon Falling or Wolf Break, so it is untested.
- No `UNHANDLED` / `EXCEPTION` lines.
- Phase 7c's `CE_LOOP` fired as well (13 single-spawn lines, `DAMAGE_EFFECT_ENERGY_BULLET_SOUND`
  from the Ara skill). A multi-spawn LOOP_* case is still untested.
- Whether the 3× air explosion *looks* right was left to the tester.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`.
- The scale change keeps March's `SetScale` in `#else`.
- Everything else is a pure addition: the RESET block, the inherit blocks in both spawners, the
  loop's damage copy, the `DAMAGE_DATA_NEXT` carry and the `GUT_USER` guard.
- The loop's `if( bInheritDamage )` grew braces inside Phase 7c's own flagged block.

### June2017 effect engine: HIT_EFFECT_SET and DIE_CREATE_NPC runtime (Phase 7e)

Phase 7e of `JUNE2017_EFFECT_REBUILD_PLAN.md`. Extends **`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`**
(no new flag) in `X2Lib/X2DamageEffect.{h,cpp}` only -- `KTDXLIB` is untouched this phase (see below).
Diagnostics extend `JUNE_FX CE_HIT` behind `SERV_IRUHADEV_JUNE_FX_DEBUG`. Two new methods:
`CX2DamageEffect::CEffect::JuneSetHitEffectSet()`, `JuneDieCreateNpc()`.

**Two of three sub-tables shipped; `TOUCH_LAND_DAMAGE_EFFECT` did not.** Phase 7d had identified all
three functions (`31f83380`+`31f81ba0` touch-land, `31f70130` hit-effect-set, `31f70330`
die-create-npc) but left them unported. This phase transcribed `HIT_EFFECT_SET` and `DIE_CREATE_NPC`
fully. Touch-land's gate and placement were settled with real evidence, but a first draft then read
two still-unidentified June offsets directly on March's `CXMeshInstance` struct -- a Rule-2 violation
(March's struct does not have June's layout) caught before this was declared done, and the spawner
turned out to be missing several blocks besides. Rather than ship a gate that silently reads the
wrong fields, or a spawner with known-missing behaviour, the touch-land call site, its new
`CEffect` method and its `CXMeshInstance` gate accessor were all removed; `KTDXLIB` is back to its
pristine state. All of the evidence gathered is recorded in `funcmap.tsv` and `census.py`
(status `deferred`, not `done`) so the next pass does not have to re-derive it.

**The touch-land velocity gate's polarity was wrong, and this is worth keeping even though the port
isn't.** `funcmap.tsv` row `31f83380` said "all three [mesh offsets] < 0", explicitly flagged "x87
compare, not yet emulated" -- a hand-trace, not a read. A real x86 decoder (`june2017_re/x87test.py`,
JITs the two comparison idioms and runs them on concrete floats via ctypes) shows the third comparison
uses the opposite mask/branch idiom from the first two, and is the **opposite polarity**: the gate is
`mesh(+0x47c) < 0 && mesh(+0x550) < 0 && m_vVelocity.y(+0x538) > 0`, all strict, and false on NaN.
`disasm_range.py`'s own symbolic tracer had the same bug one level up -- it only special-cased the
*popping* compare forms (`fcomp`/`fcompp`), so a plain non-popping `fcom` (used for the first two
checks) fell through to a generic stack dump with no comparison shown at all. Fixed in the same pass.
The gate's placement (`OnFrameMove @31f8a2a4-31f8a394`, right after March's own die-check,
`X2DamageEffect.cpp:3098-3109`, right before `DAMAGE_DATA_CHANGE_TIME`) and its position source
(`m_BeforePos`, CEffect `+0x10c` -- confirmed by a writer scan: OnFrameMove's only write to it is
later in the same frame, `@31f8a7f8`, so the gate reads last frame's value, matching March's own
read-before-write use of the member) are settled too. **What isn't settled:** `+0x47c` (read by mesh
`OnFrameMove` `31c87b80` at two points, `@31c87ef1`/`@31c8817a`) and `+0x54c`/`+0x550`/`+0x554` (a
plain per-frame copy of `m_vVelocity`, written by the same function `@31c87cac-31c87cc4`) did not
resolve to a named March `CXMeshInstance` member -- the second one looks like a June-only member
March's engine does not have at all, not a rename. The spawner (`31f81ba0`) also has an invoke-rate
skip, a damage-rate multiply, a force-down-rate branch (the `#ifdef INHERIT_FORCEDOWNRATE` arm of
`CreateHitDamageEffect`'s shape, not the `#else` arm a first draft copied), `SetScaleByUnit`, a flag
copy and `SetMaxLifeTime` -- none of which are in `CreateHitDamageEffect`'s own shape, so "same
post-create shape as the hit spawner" was not the full story. Full detail, with addresses, is in
`funcmap.tsv`'s `31f83380` and `31f81ba0` rows.

**`FUN_31f8b010`'s 18 forwarded values were mis-attributed by Ghidra's own decompile**, which split one
call's pushes across two different calls (`31aada50`, a plain `g_pX2Game` fetch with no stack args at
all, and `321e1e60`/`322e67c0`, the real targets). Settled by disassembling the raw pushes directly and
tracking ESP through the interleaved `PUSH`/`SUB ESP` sequence, computing each read's original-frame
offset as `stack-offset - cumulative-bytes-pushed-so-far`. Both fixes are in `funcmap.tsv`; the plan's
own "Phase 7a: x87 hand traces are unreliable, emulate" and "Phase 0: any decrypted name is a lead, not
a fact" rules are exactly what caught these, along with the touch-land findings above.

| Sub-table | June | March target | Notes |
|---|---|---|---|
| `HIT_EFFECT_SET%d` | `31f70130`, from `AttackResult` `31f85910@31f87b1a` | `JuneSetHitEffectSet( pOwnerGameUnit, pDefenderGameUnit )`, called right after `SetHitDamageEffect()` inside March's own `bCreateHitEffect == true` branch | `PlayEffectSet( entry name, owner, defender, false, m_fPowerRate )`, everything else at March's own defaults (fLifeTime=-1, scale=(1,1,1), bCustomPosition=false) -- June passes those explicitly rather than differing. Both units must be non-null (`@31f701f6-31f70200`), matched with a guard. `EFFECT_SET_NAME` parsed; `EVENT_TIME` parsed-not-used. |
| `TOUCH_LAND_DAMAGE_EFFECT%d` | gate `31f83380` + spawner `31f81ba0` | *deferred* | See above and `funcmap.tsv`. |
| `DIE_CREATE_NPC%d` | `31f70330`, paired immediately before `31f83400` (= March's own `SetDieDamageEffect()`, which already calls `CreateDieDamageEffect`) | `JuneDieCreateNpc()`, one call at the top of `SetDieDamageEffect()` | An E8-relative-call scan of the whole June `.text` (not just the decompile) confirms exactly 6 callers of each function, paired 1:1. March calls `SetDieDamageEffect()` from 5 active sites (`DestroyInstanceHandle`, the `CEffect*`-taking `DestroyInstance`, and `OnFrameMove`'s three die branches -- by count, not independently guard-matched site by site against June's three `OnFrameMove` call sites), covering 5 of June's 6. `JuneDieCreateNpc()` fires wherever March calls `SetDieDamageEffect()`, so it is correct by construction regardless. The 6th, `OnFrameMove_LockOnData`'s `31f75e20` prelude, never calls `SetDieDamageEffect()` in March at all. That is a gap Phase 7b left, not this phase's to close; recorded so it isn't lost. `CX2Game::CreateNPCReq`'s 14-parameter overload (`SERV_INSERT_GLOBAL_SERVER` + `SERV_TRAPPING_RANGER_TEST` + `SUMMON_MONSTER_CARD_SYSTEM`+`SERV_NEW_DEFENCE_DUNGEON` all defined here). unitID/vPos(`m_vMainEffectPosition`, CEffect `+0x1f4`, confirmed by writer scan)/iAllyUID(owner `GetUnitUID()`) as expected; level/bActive/bNoDrop/iKeyCode/bElite/eAllyTeam/eCreateMonsterType are all June's own literal constants (0/true/true/-1/false/TN_NONE/NCT_NONE); eTeam is the owner's XOR-protected `+0x278` value read as `GetTeam()`, confirmed three ways: `CX2Room::TN_MONSTER == 2` matches the exact compare inside the packet-fill routine `FUN_322e67c0` (confirmed to run on `this = g_pX2Game`, traced through `FUN_31f8b010`'s unchanged ECX), `TN_NONE == -1` matches the eAllyTeam literal, `CX2GUNPC::NCT_NONE == 0` matches the eCreateMonsterType literal, and `X2GUChung.cpp:17128` has an existing March call in the identical shape. `EVENT_TIME` parsed-not-used. The gate (a vcall on the owner, forwarded as `FUN_31f8b010`'s own first parameter) is re-expressed as `GetOwnerUnit()->IsMyUnit()`, the existing local-ownership check this file already uses elsewhere -- confirmed, not just inferred by position: the vtable slot (GUUser `334781f4+0x198`) has no Ghidra function, and its raw decompile is exactly `g_pX2Game->GetMyUnit() == this`, i.e. March's own `IsMyUnit()`. `bRight` (slot `+0x170`) is a trivial one-byte-field getter by the same method -- the shape `GetIsRight()` has -- though its own byte offset wasn't cross-checked against `GetIsRight()`'s March member. |

**The new branches themselves can't be play-tested on this build's data.** The deployed (packed Sep
27) `DamageEffect.lua` has no use of any of the three sub-tables; June's own data has four users, all
Rose (RMH), per the plan's Phase 7e section. The exit for the two done sub-tables was a clean build
matching the cited branches, confirmed by `census.py` (all checks pass; the
`HIT_EFFECT_SET`/`DIE_CREATE_NPC` rows moved `deferred` → `done`, the `TOUCH_LAND_DAMAGE_EFFECT` rows
stay `deferred` with the corrected evidence attached), **plus a regression play-test** of the
functions both new methods now sit inside: `JuneSetHitEffectSet()` runs every time `AttackResult`
does (essentially every hit landed), and `JuneDieCreateNpc()` runs at the top of
`SetDieDamageEffect()` (whenever any damage effect dies, constant during any fight) -- both loops
just find an empty list and return here, but the surrounding functions are exercised regardless.

**Play-tested 2026-09-29.** Normal combat and NPC kills exercised both call sites repeatedly.
`offline_packets.log` shows no `UNHANDLED`/`EXCEPTION` lines; `offline_server.log` shows ordinary
play (quest/title load, field NPCs placed, several NPC kills, earlier phases' own `JUNE_FX
ES_CREATE`/`CE_FRAME` lines still firing normally) ending in `SHUTDOWN clean: wal checkpoint ok,
els_db.sql.bak written`.

**Built `US_SERVICE`** (`touch X2Lib/stdafx.cpp` first; X2Lib and X2, all 0 errors -- `KTDXLIB` did not
need rebuilding this time, since it ended up untouched. One build hit a self-inflicted byte-splice bug
during editing, a statement glued directly onto the next `#ifdef` with no newline between them,
`error C2014`/`fatal error C1020`; fixed before the build that succeeded) **and deployed to
`X2_offline.exe`, 18,948,096 B**, SHA-256 prefix `f61802c8f21cac8c`, 2026-09-29.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`. Everything here is a pure addition: two
new `CEffect` methods, and one new call each inside `AttackResult` (after `SetHitDamageEffect()`) and
`SetDieDamageEffect()` (at its top).

### June2017 effect engine: DamageData, existing keys (Phase 8a)

Phase 8a of `JUNE2017_EFFECT_REBUILD_PLAN.md`. New flag **`SERV_IRUHADEV_JUNE_DAMAGEDATA`**, in
`X2Lib/X2DamageManager.{h,cpp}` only. June's `SetDamageDataFromLUA` (`FUN_32632390`, 142 decrypted keys)
and `ParsingBuffFactorID` (`FUN_32630d20`) were read key by key, for the keys March already parses,
against March's `SetDamageDataFromLUA` / `ParsingBuffFactorID` as preprocessed under `US_SERVICE`.
Tool: `june2017_re/sdd_walk.py` (one block per key, XOR-decrypt noise filtered out).

**No default changed.** This function is shared by every damage source in the game (skills, NPC
scripts, damage effects), so the result is stated explicitly. For every March-parsed key (top level,
`DAMAGE`, `ATTACKER_GET`, `EXTRA_DAMAGE`, `CUSTOM_HIT_PARTICLE%d`, `ParsingBuffFactorID`'s three),
June's getter type and failure default equal March's:
- **Register-sourced defaults** were resolved by tracking `EBX`/`EDI` back to their last write. All are
  0/false, except `EXTRA_DAMAGE.LEVEL` and `DYNAMO_PARTICLE_RATE_TYPE`, which are -1 (`EDI`).
- **Enum defaults** are June literals: 0, `CAMERA_SHAKE_TYPE` 3, `DYNAMO_PARTICLE_RATE_TYPE` -1. They
  were checked against March's C++ values under `US_SERVICE` by a compile probe: `DT_PHYSIC`,
  `AT_NORMAL`, `HT_DEFAULT`, `RT_NO_DAMAGE`, `WST_NONE`, `DAT_ALL_UNIT`, `DTT_NONE`, `SI_NONE`,
  `EDT_NONE` = 0; `DECT_UP_DOWN` = 3.
- **Surrounding conditions match too:**
  - `ATTACK_ALL_TEAM` is unconditional in both, because `MODIFY_SET_DAMAGE_DATA` is on.
  - `ENABLE_`/`DISABLE_ATTACKBOX` and `CUSTOM_HIT_PARTICLE` have the same loop shape.
  - `ParsingBuffFactor`, then `PushSocketAndTemporaryBuffFactor`, then `ParsingBuffFactorID` run in the
    same order.
  - The skill-level rule is branch-for-branch identical, including `RELATIVE_SKILL_LEARN_CHECK` and the
    NPC master-unit path.
  - The function ends with the same attacker `SetDamageExceptionProcess( pDamageData, fPowerRate )`
    call (vtable `0x2b8`).
- **Protected-type stores** (`ATTACKER_GET`, `EXTRA_DAMAGE` numerics, `HIT_ADD_MP`,
  `HIT_ADD_HP_PERCENT`) are the `KProtectedType` assignment March already uses. They are anti-hack
  plumbing, not semantics.

**The one difference, now fixed: `EXTRA_DAMAGE_TYPE`'s post-read remap.** June
(`@32636204-32636214`) turns `EDT_MANA_DAMAGE_WITH_NO_DISPLAY` (67, June `Enum.lua:7336`) into
`EDT_MANA_DAMAGE` (16) and sets a no-display byte on the extra damage.

| | before (March) | after (`SERV_IRUHADEV_JUNE_DAMAGEDATA`) |
|---|---|---|
| `EXTRA_DAMAGE_TYPE` = 67 | On March's `Enum.lua` this is unreachable, because the table has no member 67 and the read yields `EDT_NONE`. A literal 67 arrives only with June's `Enum.lua`; it is then stored as 67, a gap among March's 112 enumerators. What March does downstream with 67 was not traced. | stored as `EDT_MANA_DAMAGE` (16); `m_ExtraDamage.m_bManaDamageWithNoDisplay = true` |
| any other value | unchanged | unchanged |

- **It cannot fire on today's data.** March's `Enum.lua` has no member 67, so the Lua read yields
  `EDT_NONE`. It starts mattering when June's `Enum.lua`/`DamageEffect*.lua` are adopted (Phase 11).
- **The log line** `JUNE_FX DD EXTRA_DAMAGE_TYPE in <table>: ...` is behind `SERV_IRUHADEV_JUNE_FX_DEBUG`.
- **The flag is carried but not yet consumed.** The new member is copied by `ExtraDamageData`'s
  hand-written copy function, so it survives the hand-off to the defender. Nothing in March reads it:
  June's reader is Phase 8b's to port.
- **The reset is March-side.** `Init()` clears the flag because June's own reset site was not found:
  its ExtraDamageData init (`31f8d4d0`) clears `+0x100` but not `+0x101`.

**Not ported, and why.** None of these changes a March-parsed key's value.
- **`DRAIN_HP_RATE` is mirrored into a second June member** (`DamageData +0x4e4`, `@3263afdd`). The
  layout makes it a new member, not a renamed March one: `+0x4e0` is the rate and `+0x4e8` is the
  `CUSTOM_HIT_PARTICLE` vector. No reader is identified, and rule 2 forbids porting an offset. Lead
  for 8b.
- **June dropped the memo path.** Its `SetDamageDataFromLUA` has no `IsEqippedMemo_` parameter
  (`RET 0x10`), and `ParsingBuffFactorID` never reads `BUFF_FACTOR_RELATIVE_MEMO_ID` /
  `BUFF_FACTOR_ID_MEMO`. **Kept in March:** its own data uses both keys 12 times each, June's only 2.
- **`LAUGHTER_DAMAGE`** is read by March (`PLAY_EMOTION_BY_DAMAGE_EFFECT`) and not by June. Neither
  data set uses it. Kept, since it is harmless. It has no matrix row, because no June data uses it.
- **Not compared: the callee `SetDamageExceptionProcess`.** It is where `fPowerRate` actually reaches
  the damage values: the biggest damage lever left. Assigned to Phase 9 (2026-09-29). The same goes for June's
  `GetSkillLevel`, which takes a fourth argument (`32890620(id, 0, 0, 1)`).

Built `US_SERVICE` (`touch X2Lib/stdafx.cpp` first; all four projects rebuilt because `Always.h`
changed; 0 errors; the new `JUNE_FX DD` string is present in the exe, so the branch compiled in) **and
deployed to `X2_offline.exe`, 18,948,608 B**, SHA-256 prefix `b009e15a487c9370`, 2026-09-29. Exit
criterion: a clean build and `census.py` all-PASS. The census's new `HAND_SEMANTICS` table carries the
per-key verdicts into the matrix: phase 8a has 104 implemented rows and 0 open; semantics-differ
went from 1 to 2 with `EXTRA_DAMAGE_TYPE`. Not play-tested yet; a regression play-test would exercise
every `SetDamageDataFromLUA` call, i.e. every hit.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEDATA`. Pure addition: one member (+ reset, + copy), one
constant, one `if` after the `EXTRA_DAMAGE_TYPE` read.

### June2017 effect engine: DamageData, new keys I (Phase 8b, in progress)

Phase 8b of `JUNE2017_EFFECT_REBUILD_PLAN.md`, extending `SERV_IRUHADEV_JUNE_DAMAGEDATA` (no new
flag) in `X2Lib/X2DamageManager.{h,cpp}`, `X2Lib/X2DamageEffect.cpp` and `X2Lib/X2GameUnit.{h,cpp}`.
Five of the 36 new keys 8a's handoff assigned to this phase (the original "33" and "28" were a miscount,
corrected 2026-09-29), done this session; 31 `to-implement` plus 13 `binary-only` remain (see the
plan's Phase 8b resume point for the grouped list and what each group needs next).

- **`CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME`/`_DELAYTIME`** (`FUN_32632390
  @32633ef5-326341f4`/`@326341f4-32634510`, both bool): new `DamageData::m_bCreateEffectSetMainEffectLifetime`/
  `m_bCreateEffectSetMainEffectDelaytime`. Consumed in `CEffect::AttackResult`'s `RT_CREATE_EFFECT_SET`
  case (`FUN_31f85910 @31f861a1-31f861e2` / `@31f86755-31f86788`): both read this `CEffect`'s own
  main-effect mesh (`m_hMeshInstHandle`, the handle `GetMainEffect()` reads) and compute
  `GetMaxLifeTime() − GetNowLifeTime()`. LIFTTIME overrides the created effect set's `PlayEffectSet`
  `fLifeTime` argument (previously always its own default, `-1.f`); DELAYTIME writes
  `EffectSetInstance::m_fDelayTime` directly, after the set is created.
- **`MAKE_TARGET`** (`FUN_32632390 @3263eaeb-3263ec1a`, bool): new `DamageData::m_bMakeTarget`.
  Consumed in `AttackResult`'s default case (`FUN_31f85910 @31f86a9d-31f86acb`) — unlike every other
  key in this batch, June stores the result onto the **owner** (attacker), not `DamageData` or
  `CEffect`: the defender's `UnitUID` and `GameUnitType` at the moment of the hit. New
  `CX2GameUnit::m_uidMakeTarget`/`m_eMakeTargetGameUnitType` (+ `SetMakeTarget()`/getters,
  `X2GameUnit.h/.cpp`), initialized `-1`/`GUT_USER` in the constructor. **No March reader exists for
  these yet** — the same "carried, not consumed" situation as 8a's `DRAIN_HP_RATE`.
- **`TRACE_NEAREST_BONE`** (`FUN_32632390 @32634b5b-32634cf0`, bool): new
  `DamageData::m_bTraceNearestBone`. **Parsed only.** The call site is identified —
  `FUN_31f85910 @31f8679f-31f867ad` calls `FUN_321da350(pInstance, &mainEffectMesh-position,
  pTraceUnit)` — but `FUN_321da350`'s own bone-lookup callees (`FUN_32278580`, `FUN_31c1c1a0`,
  `FUN_31f54910`) are not identified, and this build already has a separate bone-trace-target
  subsystem (`SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME`'s `TargetMeshData`, Phase 4b) that this key may
  or may not feed into. No runtime behavior was written to avoid guessing which.
- **`DAMAGE_DATA_KEY_NAME`** (`FUN_32632390 @3263ec1a-3263edf3`, string): new
  `DamageData::m_wstrDamageDataKeyName`. **Parsed only.** The registration call is cited
  (`FUN_32622220 @3263edee(name, pDamageData)` on the damage manager, run when the name is non-empty)
  but its purpose and any reader are not identified.

Built `US_SERVICE` (`touch X2Lib/stdafx.cpp` first; X2Lib and X2, 0 errors) **and deployed to
`X2_offline.exe`, 18,949,120 B**, SHA-256 prefix `1c69e9949daa31be`, 2026-09-29. `census.py`: all
five keys moved from `to-implement` to `implemented` (reported under phase `8a`, since a
`DE.DAMAGE_DATA` key with a March parse site always reports there — see `census.py`'s
`phase_of()`), all checks PASS. Not play-tested yet: `CREATE_EFFECT_SET_MAIN_EFFECT_LIFTTIME`/
`_DELAYTIME`/`TRACE_NEAREST_BONE` need a `DamageData` with `m_wstrCreateEffectSetName` set and one
of these flags true, which no deployed March data currently sets (same situation as Phase 2's
`ES_TEMPLET` check — settled at Phase 11 when June's own data is adopted); `MAKE_TARGET` and
`DAMAGE_DATA_KEY_NAME` have no reader to observe yet either. So this round's play-test is a
regression check on the always-executed `RT_CREATE_EFFECT_SET`/default-case paths, not a feature
verification — see the plan's Phase 8b "Play-test checklist" for what to look for.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEDATA`. Pure addition: five new members across three
files (`DamageData` x5, `CX2GameUnit` x2), one new `AttackResult` branch in the
`RT_CREATE_EFFECT_SET` case, one new `if` block at the end of the default case.

### June2017 effect engine: DamageData, absent systems (Phase 8c)

Phase 8c of `JUNE2017_EFFECT_REBUILD_PLAN.md`. It extends `SERV_IRUHADEV_JUNE_DAMAGEDATA` (no new
flag) in `X2Lib/X2DamageManager.{h,cpp}` and `X2Lib/X2DamageEffect.cpp`.

**What changed.** Eight `DAMAGE_DATA` keys whose system this build lacks are now read by
`SetDamageDataFromLUA`, each with June's getter type and default, and stored in new `DamageData`
members. **Nothing reads them** (plan rule 5).

| key | system | new member | default |
|---|---|---|---|
| `APPLIED_SKILL_ID` | applied skill ID | `m_iAppliedSkillID` | 0 |
| `NATURAL_FORCE_TYPE` | natural force (`ADD_RENA_SYSTEM` off) | `m_iNaturalForceType`, only under `#ifndef ADD_RENA_SYSTEM` | -1 (`NFT_NONE`) |
| `HIT_ADD_COMBINATION_POINT` | combination point | `m_iHitAddCombinationPoint` | 0 |
| `HP_RATE_DAMAGE_TYPE` | HP-rate damage | `m_iHpRateDamageType` | 0 (`HRDT_NONE`) |
| `HP_RATE_DAMAGE_VALUE` | HP-rate damage | `m_fHpRateDamageValue` | 0.0 |
| `NUMERICAL_DATA_INDEX` | numerical skill data | `m_iNumericalDataIndex` | 0 |
| `NUMERICAL_DATA_NAME` | numerical skill data | `m_wstrNumericalDataName` | `L""` |
| `ROSE_WEAPON_ATTACK_TYPE` | Rose weapon | `m_iRoseWeaponAttackType` | -1 (`RWAT_NONE`) |

**The log line.** A value other than June's default writes one `offline_server.log` line per key per
templet per session:

```
JUNE_FX unsupported <KEY> in <templet>: <system> absent (value <v>, table <DAMAGE_DATA...>)
```

- The CEffect ctor supplies the templet name through a scoped `CX2DamageManager::JuneDamageDataTempletScope`.
  Other callers (unit, NPC and buff scripts) print `(no templet) <table>`.
- The line is not behind `SERV_IRUHADEV_JUNE_FX_DEBUG`. It needs `SERV_IRUHADEV_OFFLINE`, because
  `CX2OfflineLog` lives there.

**No damage number or default changed.** Every existing read is untouched. March's own
`ADD_RENA_SYSTEM` natural-force read is still what compiles if that flag is ever turned on.

Built `US_SERVICE` (`touch X2Lib/stdafx.cpp` first; KTDXLIB, X2Lib and X2 all 0 errors, no warnings in the edited files) and **deployed to `X2_offline.exe`, 18,952,704 B**, SHA-256 prefix `dc5c6c4fef65ae2d`, 2026-09-29. The new format string is present in the binary. `census.py`: all 8 rows `unsupported` with a March site and `same` semantics (three by hand in `HAND_SEMANTICS`: register defaults), all 12 checks PASS. **Not play-tested yet.**

**What the deployed data can show.**
- `NATURAL_FORCE_TYPE`: five Rena templets (`DOUBLE_JUMP_X_ARROW`, `DOUBLE_JUMP_XX_ARROW`,
  `NATURAL_FORCE_EXPLOSION_BOOM_COMBO`/`_SKILL`, `LESI_COMBAT_RANGER_..._Z_UP_DAMAGEEFFECT`) and
  `DamageEffect_DRYADKENTA_SpacialAttackB_Fire`.
- `NUMERICAL_DATA_INDEX`: `DamageEffect_SA_RHB_Lockon_Stratos_Attack`.
- The deployed `APPLIED_SKILL_ID` names a `SKILL_ID` member March lacks, so it arrives as nil and
  never logs.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEDATA`. This phase is pure addition: 181 lines inserted,
none deleted.

### June2017 effect engine: the damage-effect hit test (Phase 9)

Phase 9 of `JUNE2017_EFFECT_REBUILD_PLAN.md`. New flag **`SERV_IRUHADEV_JUNE_COLLISION`**
(`KTDXLIB/Always.h`; `#error` without `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_PARSE`). Files:
`X2Lib/X2DamageManager.cpp` (all logic), plus one getter in `X2Lib/X2DamageEffect.h`
(`GetUseAdvanceCollisionCheck()`). `KTDXCollision.h` is untouched.

**What changed.** `CX2DamageManager::EffectToUnit` (a damage effect hitting any unit, user or NPC)
now calls `JuneEffectCollisionDataCheck()`. That is a re-expression of the collision test June's
`EffectToUnit` (`FUN_3262b6e0 @3262bc1a-3262eebd`) inlines. March's call stays in the `#else`.
Against March's `CKTDXCollision::CollisionDataCheck( list, listSet )`:

| pair (attack × defender) | hit test | impact point |
|---|---|---|
| sphere × sphere | same (`dist < rA + rB`) | same (midpoint), **unless** `USE_ADVANCE_COLLISIOIN_CHECK` (below) |
| line × line | same (March `LineToLine` inlined branch for branch) | same (point on the attack line) |
| line × sphere | **changed.** The closest point on the attack segment is clamped to its ends. March rejects a sphere behind the line's start (projection < 0) and uses an unclamped projection past its end. | **changed.** Halfway from the sphere centre to that point; March used the projection point on the line. |
| sphere × line | **changed.** The same clamp, on the defender's segment. | **changed.** Halfway from the attack sphere's centre to the closest point; March used the projection on the defender's line. |

- `USE_ADVANCE_COLLISIOIN_CHECK` (read `@3262e976`) changes only the sphere×sphere impact point:
  - defender centre inside the attack sphere → the defender centre;
  - else attack centre inside the defender → the attack centre;
  - else equal radii (`|rA − rB| ≤ FLT_EPSILON`) → the midpoint of the centres;
  - else the midpoint of the two near surface points.

  **Phase 7d's "swept test between the previous and current centre" guess was wrong.** No deployed
  `DamageEffect.lua` sets the key.
- **Unchanged:** the guard rule (`CT_GUARD` wins if seen first; a guard alone reports `CT_GUARD`),
  the radius (`m_fRadius × m_fRadiusScale × combine-matrix X/Y scale`), the `m_bEnable` skips.
  `m_Name` is not filtered in either engine.

**Visible effects of the change.**
- **Hit particles move on capsule defenders.** Wherever a line hitbox meets a sphere, the impact
  point (where `HIT_DAMAGE_EFFECT … DEPT_IMPACT_POINT` and hit particles spawn) shifts from the
  line's projection point to halfway between the sphere centre and the line. Most units' collision
  data are spheres, so this shows only on line-shaped data.
- **Gameplay: some effects that missed now hit.** A sphere just behind a line's start point used to
  be rejected outright (projection < 0). Now its distance to the start point is tested. The reverse
  is possible too: a sphere far beyond a line's end, which March's unclamped projection accepted.
  The `JUNE_FX HIT DIFF` line below shows every such disagreement.
- **Known gap.** June's `UnitToUnit` check (`FUN_32611970`) inlines the same code, so June changed
  the shared header. Here only `EffectToUnit` is switched over. Melee (`UnitToUnit`),
  `UnitToEffect`, `MeshToUnit` and `CollisionDataToUnit` keep March semantics.

**Not ported, recorded:**
- written by June with no identified reader: `DamageData +0x62c` = the hit defender datum's centre,
  and the damage-manager `m_Name` copy;
- `DamageCheck`'s `+0x91e4` early-outs;
- `FUN_32622990`'s extra PvP/field team conditions.

Details are in `june2017_re/funcmap.tsv`.

**Diagnostics** (behind `SERV_IRUHADEV_JUNE_FX_DEBUG`, throttled to one line a second per effect
and branch):

```
JUNE_FX HIT <effect> ENTER   units owner team live main anim attacks hitlist   (traced effects only)
JUNE_FX HIT <effect> TEST    attacks a0{type linked c r} mesh defender uid data near{d rsum} june march   (traced only)
JUNE_FX HIT <effect> COLLIDE defender uid collType impact march                (every effect)
JUNE_FX HIT <effect> DIFF    june march defender uid june_impact march_impact  (every effect)
```

- "Traced" means the name contains `MAG_CHARGER`: the named test case,
  `DamageEffect_MAG_CHARGER_Attack_A` (Mag Charger's fireball, templet `MAG_CHARGER_Attack_A`,
  `Arme_FireballS.X`).
- The `TEST` line prints the attack sphere's centre next to the flying mesh's position. If they
  differ, the hitbox is not following the visual. Neither engine's math would fix that.
- `DIFF` runs March's test on the same pair purely for comparison.

**§4 of `RANOX_EFFECT_RUNTIME_UNKNOWNS.md`** (4.2-4.8) is answered there, each with a June citation.

Built `US_SERVICE` (`touch X2Lib/stdafx.cpp` first; all four projects rc 0, no new warnings) and
**deployed to `X2_offline.exe`, 18,966,528 B**, SHA-256 prefix `e90c673d1c707257`, 2026-09-29.
The trace strings are present in the binary. **Not play-tested yet.**

Revert: undefine `SERV_IRUHADEV_JUNE_COLLISION`. Pure addition: 331 lines inserted, none deleted.

**`SetDamageExceptionProcess` (`SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION`): compared in part, no code
yet.**
- **How the functions were found.** `june2017_re/vtslot.py` resolves vtable slot `0x2b8` from RTTI
  for every class.
- **The base and user layers are compared:** `CX2GameUnit` `322990e0`, and `CX2GUUser` `32045340`
  (also Ara's). **Every `fPowerRate` multiply is March's**: `damage.fPhysic/fMagic`, extra damage
  unless fixed, `EDT_PAIN`. So no damage number moves from these two layers.
- **Their June-only additions are all in systems March lacks, or unidentified:**
  - an MP-gain multiplier for `attackType == AT_COMMAND (2)`, a value March's `ATTACK_TYPE` does
    not have;
  - an MP-gain multiplier and a hook, both keyed on `APPLIED_SKILL_ID` (the skill memo);
  - three per-hit tags with no known reader;
  - an attack-type gate (`+0x4108`) on Raven's damage-type change, whose writer is not identified;
  - one unidentified call.
- **NPC `32759f30`** (June-only; March NPCs inherit the base): the base, then the master unit's
  skill-memo hook (vtable `0x758`), only for an NPC with a live master. On March data a monster hit
  is exactly March's base.
- **Still to compare:** eight per-class overrides (`funcmap.tsv`, rows marked `candidate`). Four
  have March counterparts: Add, Elesis, Lire, Arme. Four are June-only overrides: Eve, Chung,
  Raven, Elsword.

### June2017 effect engine: XMeshPlayer `.txt` templet keys (Phase 10)

Phase 10 of `JUNE2017_EFFECT_REBUILD_PLAN.md`. New flag **`SERV_IRUHADEV_JUNE_MESHPLAYER`**
(`KTDXLIB/Always.h`; `#error` without `SERV_IRUHADEV_RANOX_FORCE_NOT_CULL`, whose member it uses).
Files: `KTDXLIB/KTDGXMeshPlayer.h` and `KTDXLIB/KTDGXMeshPlayer.cpp`.

**June's parser**, all in `june2017_re/funcmap.tsv`:
- `ProcessTempletBlock` `31c8ae10`, with the tokenizer `DetermineTokenType` `31c85840` and
  `EventFactory` `31c85350`.
- Its keywords are plaintext.
- Against March it adds exactly four bool templet keys and nothing else. Every §4.1 key
  (`UseLand`, `CrashLand`, `Gravity`, `ElasticCoeff[_X]`, `NOT_CHECK_LAND_TIME`, `LifeType`,
  `LightPos`, `DrawCount`) was already parsed, with June's semantics.

**Why it matters more than a Lua key.** March's tokenizer has no entry for these four. It throws
`Unexpected "..." in Sequence Block!` (a MessageBox), and **the rest of that `.txt` file is never
loaded**. June's own `GameMajorXMeshPlayer*.txt` use `ALPHABLEND` 25 times and
`SLASH_TRACE_NOT_CULL` 14 times. March's live copies and the merged Ranox templets use none, so
nothing loaded today changes unless it sets one of the keys.

| Key | Default | What it does now | June source |
|---|---|---|---|
| `ALPHABLEND` | **TRUE** | The ctor calls `SetAlphaObject( bAlphaBlend )` after the templet copy. All three render params (skin ctor, `OnFrameMove`, X_MESH `Draw`) take `bAlphaBlend = m_TempletData.bAlphaBlend` in place of `IsAlphaObject()` (the skin ctor's line was commented out in March). | ctor `31c8d140 @31c8d2f6/@31c8dcbd`, OnFrameMove `31c87b80 @31c88cc4`, Draw `31c839c0 @31c83cf9`; default `31c907e0 @31c90b5a` |
| `WIREFRAME` | false | `OnFrameRender_Draw` sets `D3DRS_FILLMODE = D3DFILL_WIREFRAME` after the `NOT_RENDER_EFFECT_MADE_BY_GAME_UNIT` early return. It restores the fill mode cached before it (not SOLID) after both mesh paths. | Draw `31c839c0 @31c839ed`, `@31c83d4b` |
| `SLASH_TRACE_NOT_CULL` | false | `SetIsForceNotCull()` on both slash traces, right after `AddObjectChain`. | ctor `@31c8df76`, `@31c8e09c`; read by the sort loop `31ceb5f0 @31ceb6b7` |
| `POSTEFFECT` | false | **Unsupported** (plan rule 5): parsed and stored. When TRUE it logs `JUNE_FX unsupported POSTEFFECT in <templet>: post-effect render pass absent` to `ErrorLog.txt`. | slot-11 predicate `31c83500`; post-effect pass `31cd7810` mode 1 |

**Visible effects.**
- Only templets that set a key change. Absent keys give March's behaviour: `ALPHABLEND` defaults to
  true, which is March's `SetAlphaObject( true )`.
- **One semantic shift.** A mesh instance's render param no longer follows `IsAlphaObject()`, so a
  later `SetAlphaObject()` on the instance no longer changes how it blends.
  - **X2Lib: none of its 115 calls targets a `CXMeshInstance`.** Each receiver's declared type was
    checked: `CKTDGXSkinAnim`, `CX2SubEquip`, particle sequences, `CX2UIUnit`, or `this` in a
    unit/world/UI class.
  - **Lua: only the readable `.src.lua` sources were checked** (`march2014/`, `kom_contents/`). Their
    callers (`g_pObjectMesh`, `g_pWorldLayer`, `g_pObjectSkinMesh`) are world objects. Compiled-only
    data036 scripts were not checked. A Lua `SetAlphaObject` on a mesh instance would now stop
    changing its blend.
- **`SLASH_TRACE_NOT_CULL` placement differs slightly.** June's sort loop skips the frustum call
  entirely. The Ranox flag short-circuits inside `IsInFrustum`, after the `GetShowObject()` test.
  - The effect is the same: never culled.
  - A hidden trace is still rejected in March, but it would not draw in June either (`Prepare`
    tests it).
- **`POSTEFFECT` does nothing extra.** In June such a mesh draws in the ordinary pass **and** again
  in the post-effect pass. March has only the ordinary pass.

**Not ported:**
- June's post-effect pass itself. It is a DG-manager render mode with its own transforms.
- (June's *particle* tokenizer has `POSTEFFECT`/`WIREFRAME` too. That is now ported: see the next
  section.)

**Diagnostic** (behind `SERV_IRUHADEV_JUNE_FX_DEBUG`): each CrashLand death in `PlayProcess` writes
the following line to `ErrorLog.txt` (KTDXLIB cannot reach `offline_server.log`):

```
JUNE_FX MP_CRASH <templet> <LT_TIME|LT_PLAY_COUNT/X_MESH|LT_PLAY_COUNT/X_SKIN_MESH> t=<time> notCheck=<NOT_CHECK_LAND_TIME> y=<pos.y> land=<land height>
```

It is for `RANOX_EFFECT_RUNTIME_UNKNOWNS.md` §4.1, now answered: June's land and crash code is
March's, and `MAG_CHARGER_Attack_A` is identical in both builds. The line shows whether the Mag
Charger fireball reaches the ground before its target.

**Census.** `june2017_re/census.py --meshplayer` gives 0 to-implement, 1 unsupported, 64
implemented. The full run appends it to `JUNE2017_EFFECT_KEY_MATRIX.md`.

Built `US_SERVICE` (`touch KTDXLIB/stdafx.cpp X2Lib/stdafx.cpp` first, since `XMeshTemplet` changes
size; all four projects rc 0, no warnings from the changed files) and **deployed to `X2_offline.exe`,
18,969,088 B**, SHA-256 prefix `04392c9c65fef4dc`, 2026-09-29. The new keyword and log strings are
present in the binary. **Not play-tested yet.**

Revert: undefine `SERV_IRUHADEV_JUNE_MESHPLAYER`. Pure addition: 154 lines inserted, none deleted.
Undefining it restores March's parse, so a June templet using one of the four keys throws again.

### June2017 effect engine: particle `.txt` templet keys (Phase 10 follow-up)

New flag **`SERV_IRUHADEV_JUNE_PARTICLE`** (`KTDXLIB/Always.h`). Files: `KTDXLIB/KTDGParticleSystem.h`
and `KTDXLIB/KTDGParticleSystem.cpp`.

**June's particle parser** (`funcmap.tsv`): tokenizer `31cf2250`, `ProcessEventSequenceBlock`
`31cfa4a0`, `EventFactory` `31cf1e80`.
- Against March's, the key sets differ by exactly two sequence bools: `POSTEFFECT` and `WIREFRAME`.
  The event names are identical, 16 each.
- Only the key sets were compared. The shared keys' store semantics were not.
- March's tokenizer threw on both words, and the rest of that `.txt` file was never loaded.
  - June's own `*Particle*.txt` use `WIREFRAME` 44 times.
  - March's live copies use neither.

| Key | Default | What it does now | June source |
|---|---|---|---|
| `WIREFRAME` | false | `CParticleEventSequence::OnFrameRender_Draw`, after the latency and particle-list early-outs, sets `D3DFILL_WIREFRAME` for every particle type. It restores the fill mode cached before it (not SOLID) after the switch. Parsed, defaulted, cloned (`Clone`) and compared (`operator==`). | parse `31cfa4a0 @31cfb071`; ctor `31cf78a0 @31cf7c7c`; clone `31cf8af0 @31cf8fef`; Draw `31cf7500 @31cf757c`, `@31cf77dc` |
| `POSTEFFECT` | false | **Unsupported** (plan rule 5). Parsed, stored and cloned; when TRUE, logs `JUNE_FX unsupported POSTEFFECT in <sequence>` to `ErrorLog.txt`. The June consumer is the same post-effect pass as the mesh player's: sequence slot 11 `31cef710`, DG manager `31cd7810` mode 1. | parse `@31cfafef`; clone `@31cf8e8c` |

**Census.** `census.py --meshplayer` now covers both parsers. The `Is*()` helper map is scoped to each
parser's own token class, because both classes have an `IsAniXET` with different keys. Particles:
0 to-implement, 1 unsupported, 60 implemented. A negative control against the pre-change source
reports exactly the two keys.

Built `US_SERVICE` (both PCH sources touched; rc 0, no warnings from the changed files) and **deployed
to `X2_offline.exe`, 18,969,600 B**, SHA-256 prefix `73ab71b6282dca56`, 2026-09-29. **Not play-tested.**

Revert: undefine `SERV_IRUHADEV_JUNE_PARTICLE`. Pure addition: 90 lines inserted, none deleted.

### June2017 effect engine: enum data + loader-list gaps (Phase 11)

**Enum data (no flag -- pure Lua data, not client code).** Added 968 `(table, member)` values to
`march2014/data036/Enum.src.lua`: 17 members across 7 new tables (`APPLY_EFFECT_ALPHA_CONDITION_BY_OPTION`,
`DAMAGE_EFFECT_DIE_CONDITION_TYPE`, `DUO_TYPE`, `EMOTION_TYPE`, `HP_RATE_DAMAGE_TYPE`,
`NOT_TARGET_PRE_LOCK_ON_TARGET_TYPE`, `ROSE_WEAPON_ATTACK_TYPE`), 936 members into 13 existing tables
(`ATTACK_TYPE`, `BUFF_BEHAVIOR_TYPE`, `BUFF_FACTOR_ID`, `BUFF_FINALIZER_TYPE`, `BUFF_TEMPLET_ID`,
`DAMAGE_EFFECT_POSITION_TYPE`, `DAMAGE_TRIGGER_TYPE`, `EXTRA_DAMAGE_TYPE`, `LOCK_ON_TYPE`, `NPC_UNIT_ID`,
`SKILL_ID`, `UNIT_TYPE`), and 15 into `HIT_TYPE` once its C++ consumer was widened to take them safely
(see below). Plus 2 renumbered in place (also below). Values verbatim from June's `Enum.lua`, cross-checked
against the transcoded bytecode actually executed with the vendored luajit (not just the ljd decompile),
and round-tripped through the real `mar-2014` compiler with 0 unintended changes. Compiled sibling
`march2014/data036/Enum.lua` (265,151 B) is ready to pack; **the user packs it**, replacing the existing
`Enum.lua` entry in `data036.kom` -- but **not before** the `x2.exe` below is deployed (the `HIT_TYPE`
members it carries need the widened array; see next paragraph).

**`HIT_TYPE` extended rather than held.** New flag `SERV_IRUHADEV_JUNE_HITTYPE_EXTEND`
(`KTDXLIB/Always.h`, `X2DamageManager.h`/`.cpp`). March's
`m_DamageSoundName[HIT_TYPE_COUNT][HITTED_TYPE_COUNT]` (`X2DamageManager.h:1622`) is indexed directly by
`pDamageData->hitType` (`X2DamageManager.cpp:138`, `GetDamageSound`, `:3157`) with no bounds check, so the
15 members June added past March's `HIT_TYPE_COUNT` (54) needed the array widened before they could be
added to the Lua enum at all. `HIT_TYPE_COUNT` is now 69 (the array is dimensioned by the enum constant, so
it resizes with it), guarded by
`static_assert( HT_STONE == 53 && HT_EXPLOSION_2 == 68 && HIT_TYPE_COUNT == 69, ... )` against the
sequential `#ifdef SERV_9TH_NEW_CHARACTER` numbering drifting silently later. 15 new
`ParseDamageSoundBlock` calls read June's own `DamageSound.lua` table names
(`HT_MAGIC_GLOVE_SLASH`/`_PUNCH`, `HT_MAGIC_CIRCLE`, `HT_MAGIC_BULLET`/`_BIG`, `HT_SOUL_HIT`/`_BIG_HIT`,
`HT_CUT_HIT_1/2`, `HT_PIERCE_HIT_1/2`, `HT_IMPRINT_HIT_1/2`, `HT_EXPLOSION_1/2`) -- confirmed present in
`june2017_re/june_lua/DamageSound.src.lua:522-611`. March's live `DamageSound.lua` doesn't have these 15
tables yet, so they play with no sound until it's adopted too (the same silent default every other
`HIT_TYPE` already falls back to for an unset key, not a new failure mode); `DamageSound.lua` is already
one of the 14 files the Phase 11 Lua-level trial covers.

**`BUFF_FACTOR_ID` renumbered to June's values:** `BFI_BUFF_HAMEL_SECRET_DUNGEON_ARMOR_EVENT` 365->372,
`_WING_EVENT` 366->373 (June moved this pair by +7). Checked clear before editing: 372/373 free in both
March's Lua table and its C++ enum; zero hits for either name, or for `BUFF_FACTOR_ID` generally, anywhere
under `KncWX2Server/Common` or `X2Lib/Offline` (not on the wire, not persisted); the full March-vs-June
`BUFF_FACTOR_ID` diff has 3 other differs (three potion buffs, +119, unrelated and out of this plan's
scope, left untouched). The un-renumbered `BFI_BUFF_HAMEL_SECRET_DUNGEON_ARMOR`/`_WING` (280/281) are
referenced by name in the live merged `DamageEffect.src.lua`, confirming `HAMEL_SECRET_DUNGEON`
(`Always.h:1153`, unconditional) is real content, not dead.

**Loader-list gaps: new flag `SERV_IRUHADEV_JUNE_LOADER_GAPS`** (`KTDXLIB/Always.h`). Files:
`X2Lib/X2Game.cpp` (2 sites), `X2Lib/X2SquareGame.cpp`, `X2Lib/X2StateStartUp.cpp`.

- **`GameMajorParticle%d.txt` turned out not to be a fixed-count list in June** -- Ghidra decompile of
  `FUN_32597410` (`32597b20`+) is a self-terminating loop (format, open, break on first miss); June ships
  2-67 contiguously, March's three call sites stopped at 23. Now opens 24-67 at all three, not just 36 and
  44 (the original, narrower version of this flag). Verified both `REALTIME_SCRIPT_PATCH` (the live
  `X2Game.cpp` site's guard) and this flag survive `cl /P` under `X2Lib`'s real `US_SERVICE` defines.
- **`DamageEffect_Character.lua`/`_ETC.lua` were never opened at all, not just `_Dungeon.lua`.** `fxdump`'s
  globals census shows 1,022 damage effects defined only in `_Character.lua`, 805 only in `_Dungeon.lua`,
  431 only in `_ETC.lua` (next to 6,456 in `DamageEffect.lua`, 268 in `_INT.lua`) -- about 25% of June's
  damage effects would have silently never loaded. Fixed at both `X2Game.cpp` sites, in June's own order
  (`FUN_322e2930`: `DamageEffect`, `_Character`, `_Dungeon`, `_ETC`, `_INT` -- `_INT` stays last).
- **Not opened, on purpose: `Elsword_DamageData.lua`, `UnitViewerEffectSetTable.lua`.** No March consumer
  for either exists at all (no `InsertDamageData_LUA` binding, no literal reference to either name
  anywhere in `X2Lib`/`KTDXLIB`). Needs C++ work first; out of this phase's scope.

None of these files are packed into March's live `data036.kom` yet, so the flag is currently inert in the
live game directory -- `OpenScriptFile` on a file the archive doesn't have logs and continues.

**Particle override set computed in full** (`june2017_re/particle_override_census.py` ->
`particle_override_census.tsv`), not just the 18 C++-literal names `JUNE2017_NAME_CONTRACT.md` could see:
**1,401 names** collide between March's files 1-23 and June's newly-opened 24-67. **1,223 are
byte-identical** (pure reorganization, June just moved them -- a no-op once packed); **178 genuinely
differ** (~117 Add-character skill effects moved into `GameMajorParticle{29,30}.txt` with real content
changes, ~61 event/lobby/property effects into `GameMajorParticle44.txt`). Checked and confirmed
identical, not a concern: `BossRaid_Gate_P01-05,07`, `RaidFieldGate`, the four `HeadMarker*3D` names,
`PremiumSquareShop` -- the live-content names flagged in this section's first draft were false alarms.
The 178 real differs are a play-test item now, not further static analysis; see `JUNE2017_TRIAL_LOAD.md`.

Built `US_SERVICE`, rc 0, all three projects (`KTDXLIB`, `X2Lib`, `X2`), across two rounds (loader gaps,
then `HIT_TYPE` extend + BFI renumber). Final `x2.exe`: 18,979,840 B, 2026-10-01 02:01:48. **Not deployed,
not play-tested** -- the trial-load step (game-directory copy, packed trial `.kom`, launch) needs the
throwaway copy built first; see `JUNE2017_TRIAL_LOAD.md`.

Revert: undefine `SERV_IRUHADEV_JUNE_LOADER_GAPS` and/or `SERV_IRUHADEV_JUNE_HITTYPE_EXTEND`
independently; the enum additions are separate (see above) and are not gated by a flag. The `BUFF_FACTOR_ID`
renumber is a plain value change (365/366 <-> 372/373), not additive -- revert by editing the two lines
back. Otherwise pure addition throughout `KTDXLIB/Always.h`, `X2Lib/X2Game.cpp`, `X2Lib/X2SquareGame.cpp`,
`X2Lib/X2StateStartUp.cpp`, `X2Lib/X2DamageManager.h`/`.cpp`, `march2014/data036/Enum.src.lua`.

### Dryad Kenta (Burned Forest boss): "jumps off the map, never comes back" softlock

Pure content fix, no flag -- `march2014/update/data037/RAGE_DRYADKENTA.src.lua` only. Reported
symptom: mid-fight she leaves the visible map and the encounter never recovers; the only way
out is abandoning the dungeon.

Root cause: elstools' decompile of this Ranox Phase 4 file (`RANOX_PORT_PLAN.md`) repeatedly
drops a real bytecode assignment and inlines the call straight into the `if`/`elseif` condition
that reads it, leaving the reused variable unassigned everywhere else it's read in the same
function. Because none of these ported files declare Lua's `local`, every such `slotN` compiles
to a genuine `_env["slotN"]` global (confirmed by disassembling `compile-batch -v mar-2014`'s own
output -- `GSET`/`GGET` on a named constant, not a per-call stack register), so a "missing"
variable doesn't read as Lua's nil-safe undefined -- it reads as whatever that global last held,
left over from an earlier branch, an earlier call, or (worst case) another script's own bare
`slotN`. Confirmed two ways: function by function against the real June2017 bytecode via
`june2017_re/ljd_jun2017.py` and a raw pseudo-asm dump (`ljd`'s `--asm`), and independently by
extracting and disassembling the live, currently-deployed `data037.kom`'s own `RAGE_DRYADKENTA.lua`
the same way (`elstools extract -v mar-2014`, then `ljd`'s `--asm` on its `.std.luac` -- full AST
decompile fails on this particular compiled-from-broken-text file, so the disassembly is the only
route). The live copy shows the exact fault: the `PlayEffectSetWithCustomPos_LUA` call reads
`_env["slot5"]` -- last written a position vector by an earlier branch, never the `GetEffectSet()`
result computed two instructions earlier -- and errors. Six instances, two of which are
independent routes to the reported softlock:

- **`RAGE_DRYADKENTA_ATTACK_B_WAIT_FRAME_MOVE`** -- confirmed live, see above. Attack B teleports
  her to `(0, -5000, 0)` as its "jump/mark" telegraph, pre-seeding the return-position global with
  her *current* (already off-map) position; the frame-move is supposed to overwrite it with a real
  on-map mark position before the state that reads it back plays. Three dropped assignments in this
  one function meant it called `PlayEffectSetWithCustomPos_LUA` on a stale position vector instead
  of the effect-set object, erroring out before the return-position global was ever overwritten --
  so she gets placed right back at `(0, -5000, 0)` and stays there.
- **`RAGE_DRYADKENTA_ATTACK_A_ATTACK_DASH_STATE_START`** -- a second, independent route to the same
  symptom (not ranked against the one above -- both are real; "jump out of the map" could describe
  either). `COMMON_FRAMEMOVE` forces this dash state at 60% and 20% HP on a cooldown. The dash's
  target-position global was left unassigned, and the dash only ends within 200 units of it, so an
  unset (or stale, non-position) target lets her dash indefinitely and run off the map. Also
  restored a second, pre-existing instance of the same bug in this function's first line
  (`GetLastTouchLineIndex()`'s result), present since the original Phase 4 port and unrelated to
  the softlock but the same class of bug.
- **`RAGE_DRYADKENTA_COMMON_FRAMEMOVE`** -- the HP-ratio division was never stored, so once the
  60% phase had fired once, the 20% phase's own condition read that global as unset and errored
  (Lua's `<=` isn't nil-safe the way `==` is) every WAIT/DAMAGE_FRONT/DAMAGE_BACK frame; the 20%
  dash never triggered. Not a softlock by itself, fixed for correctness.
- **`RAGE_DRYADKENTA_SPECIAL_ATTACK_FIRE_FRAME_MOVE`**, **`..._FIRE_EMOTION_FRAME_MOVE`** -- same
  pattern on `slot1:GetLineMap()`'s result, breaking the Special Attack A projectile-trail effect
  calls. Not a softlock, fixed for correctness.

The restored logic's line-map dependency was checked too: Attack B and both Special Attack A
functions read `GetLineMap():GetStartPosition(1..13)`, and
`march2014/update/data040/Ranox_BurnedForest_StageBoss_LineMap.src.lua` defines
`AddStartPos(1..13, ...)` -- no missing index, so the fix doesn't just move the bug to a
different undefined lookup.

Verified with a small read-before-write lint (flags any `slotN` read in a function before that
function assigns it -- these files never use Lua's `local`, so a dropped assignment is otherwise
invisible on a read-through) and `elstools compile-batch -v mar-2014` on a scratch copy (the
compiler overwrites its input). Both are clean on the fixed file.

**Needs packing**: `march2014/update/data037/RAGE_DRYADKENTA.src.lua` still needs recompiling to
KOG bytecode and repacking into `data037.kom` before a play-test can confirm it in-game -- that
step is the user's, per this repo's own convention for `march2014/`.

**Not fixed here at the time, later fixed properly:** the same lint found the identical bug
pattern elsewhere across the Ranox-ported `data037/` corpus (dozens of files, roughly 1 to a few
hundred hits each depending on file size -- `SECRET_MAGI_BOSS.src.lua` and
`SECRET_SCENE_DUMMY.src.lua` stood out at 379 and 251). That was a quick heuristic (flags a
`slotN` read with no prior `slotN = ...` assignment anywhere earlier in the same function body),
not verified instruction-by-instruction the way Kenta's six were -- **superseded by the bytecode
harness below**, which turned "worth auditing" into 62 files / 123 functions, bytecode-confirmed
and repaired.

### Ranox Lua decompile fix: tooling (corpus harness, fix, and the 61-file repair)

No client flag -- this is tooling under `june2017_re/` plus content-only edits to
`march2014/**/*.src.lua` (the same `SERV_IRUHADEV_RANOX_PORT` comment convention as the Kenta
entry above, not a `KTDXLIB/Always.h` flag; no `X2`/`X2Lib`/`KTDXLIB` C++ changed).

The Dryad Kenta fix above was done by hand, before any of this existed. `RANOX_LUA_DECOMPILE_FIX_PLAN.md`
(9 phases, 2026-09-27 to 2026-10-01; full methodology and every phase's result live there) then
built the general version:

- **Root cause, one defect at four sites**, identified in `elstools/vendor/ljd`'s
  `slotworks.py`: it inlines a temporary whenever its *narrow, per-pass* view of the value's
  live range counts exactly one read, even when the value is actually read again outside that
  view. See `RANOX_LUA_DECOMPILE_BUG.md`'s "Root cause" for the full causal chain and upstream
  status (this is a still-open bug in third-party code, `gitlab.com/znixian/luajit-decompiler`
  issue #32, not something introduced by this port).
- **The fix**: `june2017_re/ljd_fixed.py` (design in `ljd_slotworks_fix.py`), a run-time
  monkey-patch into the vendored ljd that refuses to install unless the vendored `slotworks.py`
  matches the exact SHA-256 it was written against. `june2017_re/ljd_slotworks.patch` is the
  same change as a unified diff for the two vendored ljd copies -- **generated, not applied**;
  applying it is the user's call, not made by this work.
- **`june2017_re/ljd_jun2017.py`** (the June decompiler entry point) **installs the fix by
  default** as of this phase; `--unfixed` opts out. `june2017_re/ljd_jun2017.py --check FILE.lua`
  runs the bytecode harness against one file's own June original in one command (learn + trace +
  diff, 32 seeds, fails on `clobber` or an unexplained `dropped-assignment`) -- the fast way to
  verify a file without reading a printed decompile by eye.
- **A harness (`june2017_re/ljdiff/`) and an engine-free oracle (`june2017_re/ljd_oracle/`)**
  prove the fix: run the real June bytecode and a decompile-recompile round trip side by side
  under a mock Lua environment and diff their behaviour, rather than trusting that the printed
  Lua reads correctly (it always does -- that's what makes this class of bug dangerous).
- **The corpus sweep (Phases 5/7/8) covers all 7,328 June `.lua` files**, not just the port:
  `june2017_re/decompile_verdicts.tsv` carries one verdict row per (file, function). Zero
  unexplained `dropped-assignment` anywhere in the corpus -- the fix holds. What's left besides
  clean files is all harmless-in-engine (call-argument evaluation-order reordering, mostly) or a
  separate, already-known, still-unfixed ljd defect (the "ljd caveat" deepcopy-hoist
  mis-structuring); see `RANOX_LUA_DECOMPILE_BUG.md`'s "Residual ljd defects" for the full
  breakdown.
- **62 ported files, 123 functions, repaired** (Phase 6a-6c, this repo's commits
  `8a141a5`/`6809a33`/`2bbd358`): every real instance the sweep found in `march2014/`, restoring
  the dropped assignment ahead of its reads, same convention as Kenta's fix. 61 of the 62 are in
  `march2014/update/data037/` (the same Ranox monster/boss AI scripts as Kenta); the other is
  `march2014/data036/MapData.src.lua`. Full per-file list in `RANOX_LUA_DECOMPILE_BUG.md`'s
  "Scope" section.
- **The `local slotN` question was asked and answered, not assumed.** Bare globals stay (the
  port's existing no-`local` convention); a real census of nested-frame clobber risk found only 3
  live instances across all 276 mapped port files, fixed case by case rather than by a blanket
  `local` rewrite. See `RANOX_LUA_DECOMPILE_FIX_PLAN.md`'s "Phase 5 result" for the evidence.

**Needs packing**: all 62 repaired files listed in `RANOX_LUA_DECOMPILE_BUG.md`'s "Scope" table
(same `march2014/update/data037/` directory as Kenta, plus `data036/MapData.src.lua`) need
recompiling to KOG bytecode and repacking into their `.kom` before an in-game play-test can
confirm them -- the user's step, same as Kenta above.

## Reverting to stock

Comment out every `#define` listed in the *Defined in* column and rebuild.
No git operation is needed -- the flag system is the revert switch. That is the
main reason a reconstructed vanilla baseline was not worth building.

Reverting `SERV_IRUHADEV_QUICK_SLOT_FULL_FREE` specifically can move player
data: any item sitting in quick slot 4, 5, or 6 falls outside the stock 3-slot
category size, so on next login `KInventory::Init()` routes it through the
same `mapWrongPosItem` path stock code already uses for any item found outside
its category's current bounds -- it is not deleted, just relocated out of the
quick-slot bar.

## Rebuild requirements

`SERV_IRUHADEV_SKILLTREE_NO_LOCK`, `SERV_IRUHADEV_SKILL_SLOT_B_FREE`, and
`SERV_IRUHADEV_QUICK_SLOT_FULL_FREE` all touch
`KncWX2Server/Common/ServerDefine.h`, which compiles into **both** sides.
Toggling any of them requires rebuilding the VS2010 client *and* the five
servers, with the flag set consistently in both. Rebuild only one side and the
wire format desynchronizes silently at runtime instead of failing to compile.

The remaining five -- `SERV_IRUHADEV_BUFF_DURATION_TEXT`,
`SERV_IRUHADEV_OFFLINE`, `SERV_IRUHADEV_NO_PATCHER_TOKEN`,
`SERV_IRUHADEV_JOBCHANGE_PORTRAIT` and `SERV_IRUHADEV_MP_REGEN_BOOST` -- are
client-only. None of them touches anything under `KncWX2Server/Common/`, so
the servers never need rebuilding for any of them. Two caveats:

- `SERV_IRUHADEV_OFFLINE` needs **both** of its definition sites toggled
  together, and all three client libs plus the exe rebuilt (`KTDXLIB`,
  `X2ServerProtocol`, `X2Lib`, then `X2`).
- Toggling anything in `KTDXLIB/Always.h` -- which is every
  `SERV_IRUHADEV_*` flag -- means rebuilding **all four** projects, because
  `Always.h` sits inside each one's precompiled header. msbuild does not
  always notice, and a stale PCH makes the flag simply absent at the call
  site with no error: the `#else` branch compiles and the change appears to
  have done nothing. If in doubt, prove it rather than assume it -- put a
  `#pragma message` inside the `#ifdef` at the call site, rebuild that one
  project, and read the compiler output.

## Known deviations to clean up

**Resolved 2026-09-09 (March migration, Phase 1):** `STATIC_AUTO_LOGIN` used
to break the project rule in CLAUDE.md two ways - it wasn't
`SERV_IRUHADEV_`-prefixed, and it lived in `KTDXLIB/OnlyGlobal/Always_US.h`
(pulled in only for `CLIENT_COUNTRY_US`, so the change was silently US-only)
instead of `KTDXLIB/Always.h` - and it stored a plaintext account password in
a header that compiled into the shipped executable. Rather than fix those
three problems, the flag was retired: the studio's own `AUTO_LOGIN_IN_HOUSE`
went live under `US_INTERNAL` for the first time in this migration and does
the same job (skip the login screen, sign in as a fixed account) by reading
an external `LoginKey.lua`, which has none of the three problems. See
`MARCH_2014_MIGRATION.md`'s "What Phase 1 established" for the full reasoning.
