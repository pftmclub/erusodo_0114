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
| `SERV_IRUHADEV_LEVEL_CAP_99` | 2026-10-16 | `KTDXLIB/Always.h` (end; redefines `USE_MAXLEVEL_LIMIT_VAL` after the CAP_80 block) | `X2Lib/Offline/Handlers_Room.cpp` (typedef check, level-up no-row guard), `X2OfflineStatTable.h/.cpp`, `X2OfflineDropTable.h/.cpp`, `X2OfflineDB.h/.cpp` (schema v16), `X2Lib/Offline/X2OfflineCap99Exp.h` | -- |
| `SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX` | 2026-10-16 | `KTDXLIB/Always.h` (end) | `X2Lib/X2StateMenu.cpp` (`UpdateExpGageBar`) | -- |
| `SERV_IRUHADEV_ELYSION_PORT` | 2026-10-17 (Atlas A3, extended A5 / A6) | `KTDXLIB/Always.h` (end) | `X2World.h`, `X2LocationManager.h/.cpp`, `X2LocalMapUI.h`, `X2UnitManager.h`, `X2Main.cpp`, `X2StateMenu.cpp`, `X2UIInventory.cpp`, `X2ChatBox.cpp`, `X2FieldMapBase.cpp`, `X2GUUser.cpp`, `X2BattleFieldGame.cpp`, `X2TFieldGame.cpp`, `X2WorldMapUI.cpp`, `Offline/X2OfflineLuaEnumSeed.h`; shared headers `KncWX2Server/Common/Enum/Enum.h`, `Enum/DungeonEnum.h`, `X2Data/XSLUnitManager.h` (client-side only, offline rule) | -- |
| `SERV_IRUHADEV_ELYSION_SCRIPT_A` | 2026-10-17 (Atlas A4) | `KTDXLIB/Always.h` (end) | `X2DungeonSubStage.h/.cpp`, `X2GUNPC.h/.cpp`, `X2GUUser.h/.cpp`, `X2Main.cpp`, `X2MonsterIndicator.cpp`, `X2NPCAI.h/.cpp`; `KTDXLIB/KTDGLineMap.h/.cpp`, `KTDGUIControl.h/.cpp`, `KTDGUIStatic.cpp`, `KTDXApp.cpp` | -- |
| `SERV_IRUHADEV_ELYSION_DEBUG` | 2026-10-17 (Atlas A4; A13 adds the `SetCameraDistance` wrapper) | `KTDXLIB/Always.h` (end) | `[ELYDBG]` lines in `X2DungeonStage.cpp`, `X2DungeonSubStage.cpp`, `X2GUNPC.cpp`, `X2GUUser.cpp`, `X2Game.cpp`, `X2GameUnit.cpp`, `X2LocalMapUI.cpp`, `X2LocationManager.cpp`, `X2MonsterIndicator.cpp`, `X2QuestManager.cpp`, `X2TFieldGame.cpp`, `X2TFieldNpc.cpp`, `X2TFieldNpcShop.cpp`, `X2TalkBoxManagerImp.cpp`, `X2WorldManager.cpp`, `X2WorldMapUI.cpp`, `X2World.h/.cpp`, `X2Main.cpp`, `KTDXLIB/KTDGLineMap.cpp`, `KTDXLIB/KTDGUIControl.cpp`. Left on at the region close-out (E15, as A13 did). Diagnostic only: undefine to silence | -- |
| `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION` | 2026-10-17 (Atlas A7, E4) | `KTDXLIB/Always.h` (end) | `X2LocalMapUI.h/.cpp`, `X2LocationManager.cpp`, `X2StateMenu.cpp`, `X2UIInventory.cpp`, `X2WorldMapUI.cpp` | -- |
| `SERV_IRUHADEV_JUNE_MOVABLE_LINE` | 2026-10-17 (Atlas E5) | `KTDXLIB/Always.h` (end) | `KTDGLineMap.h/.cpp`, `KTDXApp.cpp`, `X2World.h/.cpp`, `X2Game.h/.cpp`, `X2Main.cpp`, `X2UDPPacket.h`, `X2GUNPC.cpp`, `X2WorldObjectSkinMesh.h` | -- |
| `SERV_IRUHADEV_OFFLINE_BF_JOIN_START_POS` (offline emulator) | 2026-10-17 (Atlas A6) | `KTDXLIB/Always.h` (end) | `X2Lib/Offline/Handlers_Room.cpp` (`EGS_JOIN_BATTLE_FIELD` ACK) | `SERV_IRUHADEV_OFFLINE` |
| `SERV_IRUHADEV_ELYSION_VILLAGE` | 2026-10-17 (Atlas E1a, extended E1b, E1c, E1d) | `KTDXLIB/Always.h` (end) | `X2EffectSet.h/.cpp`, `X2LocationManager.h/.cpp`, `X2Main.cpp`, `X2QuestManager.h/.cpp`, `X2State.cpp`, `X2StateField.h/.cpp`, `X2StateLoading.cpp`, `X2TFieldGame.h/.cpp`, `X2TFieldNpc.h/.cpp`, `X2TFieldNpcShop.h/.cpp`, `X2UIQuestNew.cpp`, `X2UIUnit.h/.cpp`, `X2UIUnitManager.h/.cpp` | -- |
| `SERV_IRUHADEV_ELYSION_SCRIPT_B` | 2026-10-17 (Atlas E7; E10 play-test 2 moved the additional-world clear) | `KTDXLIB/Always.h` (end) | `KTDGManager.h/.cpp`, `KTDSMP3Play.h`, `X2BattleFieldGame.cpp`, `X2DamageEffect.h`, `X2DungeonGame.cpp`, `X2EffectSet.h/.cpp`, `X2GUNPC.h/.cpp`, `X2GUNPCAI.h/.cpp`, `X2GUUser.h/.cpp`, `X2GageUI.cpp`, `X2Game.h/.cpp`, `X2GameUnit.h/.cpp`, `X2Main.cpp`, `X2SlideShot.h/.cpp`, `X2World.h/.cpp`, `X2WorldManager.h/.cpp`, `X2WorldObjectParticle.h` | -- |
| `SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE` | 2026-10-17 (Atlas E6) | `KTDXLIB/Always.h` (end) | `X2GUNPC.h/.cpp`, `X2Game.h/.cpp`, `X2Main.cpp`, `X2TalkBoxManager.h/.cpp`, `X2TalkBoxManagerImp.h/.cpp` | -- |
| `SERV_IRUHADEV_ELYSION_MOVABLE_LINE_END` | 2026-10-17 (Atlas E10 play-test 1; JUNE_BUGS 230) | `KTDXLIB/Always.h` (end) | `KTDXLIB/KTDGLineMap.cpp` (two `m_vLastMove` zeroing blocks; port gap on `JUNE_MOVABLE_LINE`) | -- |
| `SERV_IRUHADEV_ELYSION_INVINCIBLE_ATTACK_TYPE` | 2026-10-17 (Atlas E10 play-test 2) | `KTDXLIB/Always.h` (end) | `X2Lib/X2GUNPC.h/.cpp` (`GetInvincibleByAttackType`, state-start append; two call sites nested in `JUNE_BUFF_NEW_FLAG`) | -- |
| `SERV_IRUHADEV_ELYSION_ATTACK_TYPE_STANDIN` | 2026-10-17 (Atlas E10 play-test 2; user chose it) | `KTDXLIB/Always.h` (end) | `X2Lib/X2GUNPC.h` (stand-in, not June: a user's untagged `AT_NORMAL` hit counts as `AT_COMMAND`; inside `ELYSION_INVINCIBLE_ATTACK_TYPE`) | -- |
| `SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX` | 2026-10-17 (june-engine R1; **exe and data036 `Enum.lua` move together**, ramps in data042) | `KTDXLIB/Always.h` (end) | `KTDXLIB/KTDGXRenderer.h` (June `CARTOON_TEX_TYPE` enum, `m_pCartoonTexList[CTT_END + 1]`), `KTDGXRenderer.cpp` (ctor/dtor loops, 20-ramp load + `[JUNE_ENGINE] CTT` log lines); data: data036 `Enum.lua` `CARTOON_TEX_TYPE` table = June's, data042 +10 June ramps | -- |
| `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_PLAYBACK` | 2026-10-17 (june-engine R11) | `KTDXLIB/Always.h` (end) | `KTDXLIB/KTDGXSkinAnim.h/.cpp` (anim `+0x854` / `+0x855` transition bytes, play type 3 in `OnFrameMove`, `ChangeAnim` bounds check / find-without-insert / `+0x855` write), `X2Lib/X2WorldObjectSkinMesh.h` (`SetTransition_LUA` writes the anim's `+0x854`) | -- |
| `SERV_IRUHADEV_JUNE_RENDER_SKIN_ANIM_SETS` | 2026-10-17 (june-engine R11) | `KTDXLIB/Always.h` (end) | `KTDXLIB/KTDGXSkinAnim.h/.cpp` (`+0x294` backup map, `CleanAnimStructure` release check + KEM 436, `MixAnim` never-cleared name list `+0x864`, KEM 434 log, `BuildCloneFrame` NULL guard), `KTDXLIB/KTDXErrorDefine.h` (`KEM_ERROR433..436`) | -- |
| `SERV_IRUHADEV_JUNE_RENDER_SKIN_BONE_REFRESH` | 2026-10-17 (june-engine R11) | `KTDXLIB/Always.h` (end) | `KTDXLIB/KTDGXSkinAnim.h/.cpp` (`JuneRefreshBones` = `31d29950`), `KTDXLIB/KTDGXMeshPlayer.h/.cpp` (`JuneRefreshScale` = `31c83520`), `X2Lib/X2DamageEffect.cpp` (`SetMainEffectScale`) | -- |
| `SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT` | 2026-10-17 (june-engine R2) | `KTDXLIB/Always.h` (end; needs `JUNE_RENDER_CARTOON_TEX`) | `KTDXLIB/KTDGXRenderer.h/.cpp` (33-entry `EFFECT_PARAMETER` + NULL-skipping `_JuneSet*`, `RenderParam` + 2 ints, device caps, 4 `D3DXMACRO`s, `_JuneBindCartoonRamps`, `_JuneSetUpperStages`, `SetAnisotropy`, `[JUNE_ENGINE] FX` log lines) | -- |
| `SERV_IRUHADEV_JUNE_RENDER_RIM_OPTION` | 2026-10-17 (june-engine R2) | `KTDXLIB/Always.h` (end; needs `JUNE_RENDER_SHADER_CONTRACT`) | `X2Lib/X2GameOption.h/.cpp` (keys `RIM_LIGHT`, `USE_VIEW_INDEPENDENT_EDGE`, `ANISOTROPIC_FILTER_SAMPLE`, `USE_ENV_BOX`: load, save, `ApplyAllOption`), `X2Lib/X2Main.cpp` (`OnFrameMove` copies `RIM_LIGHT` into the renderer) | -- |
| `SERV_IRUHADEV_JUNE_RENDER_WORLD_CARTOON` | 2026-10-17 (june-engine R2) | `KTDXLIB/Always.h` (end; needs `JUNE_RENDER_SHADER_CONTRACT` and `RANOX_RIM_LIGHT`) | `X2Lib/X2World.h/.cpp` (`m_iCartoonTextureIndex` / `m_iRimLightTextureIndex`, `SetCartoonTextureIndex`, `SetRimLightTextureIndex` stores on the world), `X2Lib/X2Main.cpp` (Lua binding), the five `RenderParam` fills in `X2GUUser.cpp`, `X2GUNPC.cpp`, `X2Pet.cpp`, `X2RidingPet.cpp`, `X2SquareUnit.cpp` | -- |
| `SERV_IRUHADEV_JUNE_WORLD_TRIGGER_AUTO_DISABLE` | 2026-10-17 (june-engine W1b; JUNE_BUGS 262-265) | `KTDXLIB/Always.h` (end; needs `WORLD_TRIGGER`) | `KTDXLIB/KTDGLineMap.h/.cpp` (`TRIGGER_DATA_LINE::m_bJuneAutoDisableLine` + `AUTO_DISABLE_LINE` parse, `LineData` flag + user / NPC UID lists, `JuneAutoDisableRelease`), `X2Lib/X2World.cpp` (`DoTriggerLine` tail), `X2Lib/X2GameUnit.h/.cpp` (`m_iJuneAutoDisableLine`, -1), `X2Lib/X2GUUser.cpp` (`Init`, `PhysicProcess` entry snapshot + gate, `CommonFrameMove` bookkeeping), `X2Lib/X2GUNPC.cpp` (`PhysicProcess` entry land snapshot + gate + release); `[JUNE_ENGINE] WORLD auto-disable` lines | -- |
| `SERV_IRUHADEV_JUNE_RENDER_ENVBOX` | 2026-10-17 (june-engine R3; JUNE_BUGS 268-270) | `KTDXLIB/Always.h` (end; needs `JUNE_RENDER_SHADER_CONTRACT`) | new `KTDXLIB/KTDGEnvBoxMap.h/.cpp` (+ `KTDXLIB_2010.vcxproj`, `KTDGManager.h` include), `KTDXLIB/KTDGXRenderer.h/.cpp` (`IsEnvBoxEnable`, `JuneSetEnvBoxLut/Rim`, `[+0x64]`), `KTDXLIB/KTDXApp.cpp` (Lua class), `X2Lib/X2World.h/.cpp` (`m_pEnvBoxMap`, `SetEnvBoxMap`, `JuneRenderEnvBox`, reset / lost / destroy), `X2Lib/X2Main.cpp` (`SetEnvBoxMap` binding), `X2Lib/X2Game.cpp` (render call) | -- |
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
| `SERV_IRUHADEV_JUNE_BUFF_LOADER` | 2026-10-03 (June buff port Phase 3) | `KTDXLIB/Always.h:4701` | `X2Lib/X2BuffTempletManager.h`/`.cpp` (`CX2JuneBuffCapture`; June's `AddBuffTemplet_LUA`/`AddBuffFactor_LUA`/`GetBuffFactorPtr`, new `AddBuffFactorINT`/`PushBuffFactor`/`LoadINTScriptFile`/`LogLoadSummary`, `m_vecBuffFactorPtrINT` and the counters), `X2Lib/X2BuffFactor.h`/`.cpp` (`ParsingBuffFactor` wraps the original, now `ParsingBuffFactorBody`; BBT/BFT `default:` branches), `X2Lib/X2BuffBehaviorTemplet.cpp`, `X2BuffFinalizerTemplet.cpp`, `X2BuffDisplayerTemplet.cpp` (`default:` branches), `KTDXLIB/KTDXCommonFunc.h` (`DisplayErrorMessage` sink), `KTDXLIB/KTDXApp.cpp` (`g_pIruhaDisplayErrorSink`) | -- |
| `SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER` | 2026-10-05 (June buff port Phase 8) | `KTDXLIB/Always.h:4928` | `X2Lib/X2BuffFactor.cpp`, `X2BuffFinalizerTemplet.h`/`.cpp` (BFT 14/15/16), `X2BuffTemplet.h`/`.cpp`, `X2GameUnit.h`/`.cpp`, `X2GUUser.h`/`.cpp`, `X2Game.h`/`.cpp`, **`X2UDPPacket.h`/`.cpp` (P2P wire format)** | -- (P2P only; no server packet changes) |
| `SERV_IRUHADEV_JUNE_BUFF_GROUP` | 2026-10-05 (June buff port Phase 10) | `KTDXLIB/Always.h:4944` | `X2Lib/X2BuffTempletManager.h`/`.cpp` (UnitApplyBuffGroup.lua loader, groups, officially-debuff set), `X2StateStartUp.cpp`, `X2Main.cpp` (bindings), `X2GameUnit.h`/`.cpp`, `X2GUUser.h`/`.cpp`, `X2GUNPC.h`/`.cpp` (`CanApplyBuffToGameUnit(BTI)`, NPC group), `X2Game.cpp`, `X2GUAra.cpp`; data `march2014/update/data036/UnitApplyBuffGroup.lua` (pack into `data036.kom`) | -- |
| `SERV_IRUHADEV_JUNE_BUFF_REACT_APPLY` | 2026-10-05 (June buff port Phase 10) | `KTDXLIB/Always.h:4958` | `X2Lib/X2GameUnit.h`/`.cpp` (June's `ApplyBuffToGameUnitInDamageReact`, pre-apply roll, key check) | -- |
| `SERV_IRUHADEV_JUNE_BUFF_BINDING` | 2026-10-05 (June buff port Phase 10) | `KTDXLIB/Always.h:4969` | `X2Lib/X2Main.cpp`, `X2GameUnit.h`/`.cpp` (`GetBuffAccumulationCountNow`), `X2GameEdit.h`/`.cpp` (`ApplyBuffFactor` cheat), `X2BuffFinalizerTemplet.h` (`SetDurationTime`) | -- |
| `SERV_IRUHADEV_JUNE_BUFF_FACTOR_ID` (+ `_MEMO_DROP`, off) | 2026-10-05 (June buff port Phase 10) | `KTDXLIB/Always.h:4981` | `X2Lib/X2DamageManager.cpp` (June's `ParsingBuffFactorID`) | -- |
| `SERV_IRUHADEV_JUNE_BUFF_DAMAGEDATA_KEY` | 2026-10-05 (June buff port Phase 10) | `KTDXLIB/Always.h:5004` | `X2Lib/X2DamageManager.h`/`.cpp` (`IS_AURA_BUFF`, `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`) | -- |
| `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE` | 2026-10-06 (offline grade Phases 1-3c) | `KTDXLIB/Always.h:5263` | `X2Lib/X2DungeonSubStage.h`/`.cpp` (stage-script grade -> NPCData), `X2Lib/Offline/Handlers_Room.cpp`, `X2OfflineServer.h`, `X2OfflineBattleField.h`/`.cpp`, `X2OfflineDropTable.h`/`.cpp` | -- |
| `SERV_IRUHADEV_OFFLINE_FIELD_QUEST_NO_ROW_DESIGN` (designed, not server/June) | 2026-10-06 (offline grade Phase 3c) | `KTDXLIB/Always.h:5281` | `X2Lib/Offline/Handlers_Room.cpp` (a paid field kill with share >= 0.5 counts for hunt quests without a field drop row; since the Phase 3c review a row-less field monster also still drops its quest items, while its special, static and eater items stay off as on the server) | -- |
| `SERV_IRUHADEV_JUNE_DAMAGE_DATA_TABLE` | 2026-10-07 (June effect completion C9) | `KTDXLIB/Always.h` (end; needs `SERV_IRUHADEV_JUNE_DAMAGEDATA`) | `X2Lib/X2DamageManager.h`/`.cpp` (`JuneDamageDataRecord`, `InsertDamageData_LUA`, `OpenJuneDamageDataScript`, `JuneApplyDamageDataRecord`; ctor load; apply after the `DAMAGE_DATA_KEY_NAME` read), `X2Lib/X2Main.cpp` (`CX2DamageManager` class bind) | -- |
| `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_TABLE` | 2026-10-07 (June effect completion C9c-1) | `KTDXLIB/Always.h` (end; the load line also needs `SERV_IRUHADEV_JUNE_FX_DEBUG`) | `X2Lib/X2UnitManager.h`/`.cpp` (`JuneUnitViewerEffectSet`, `AddUnitViewerEffectSet_LUA`, `GetUnitViewerEffectSet`, `OpenJuneUnitViewerEffectSetTable`), `X2Lib/X2Main.cpp` (`AddUnitViewerEffectSet` bind), `X2Lib/X2Data.cpp` (`ResetUnitManager` load), `X2Lib/X2GUUser.h`/`.cpp` (`m_hJuneEmotionEffectSet`, `PlayEmotion` consumer; C9c-1 review: `m_eJuneEmotionEffectSetEmotion` and the stop sites in `PlayEmotion`, `OnFrameMove` and the dtor) | -- |
| `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER` | 2026-10-07 (June effect completion C9c-2b) | `KTDXLIB/Always.h` (end; needs `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_TABLE`, `SERV_IRUHADEV_JUNE_EFFECTSET_PARSE` and `SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME`; the play and create lines also need `SERV_IRUHADEV_JUNE_FX_DEBUG`) | `X2Lib/X2BaseUnitViewerUI.h`/`.cpp` (the player: entries, Play, Stop, update, create pass, sound pass, Prepare, Draw, `NotifyShowObjectChanged`, ctor, dtor), `X2Lib/X2UnitViewerUI.h`/`.cpp` (re-parent under `CX2BaseUnitViewerUI`, `PlayAnim` / `OnFrameMove` request queue, the base calls, the dtor stops, the land-dirty byte, nine retired hard-codes), `X2Lib/X2EffectSet.cpp` (`JuneRunViewerMeshTriggers`) |
| `SERV_IRUHADEV_JUNE_EMOTION_EFFECT_SET` | 2026-10-07 (June effect completion C9e) | `KTDXLIB/Always.h` (end; the viewer site also needs `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER`) | `X2Lib/X2Unit.h` (`GetJuneEmotionEffectSetName`), `X2Lib/X2GUUser.h`/`.cpp` (`m_hJuneEmotionSetEffectSet`, ctor, `PlayEmotion`, `OnFrameMove`, dtor), `X2Lib/X2SquareUnit.h`/`.cpp` (`m_hJuneEmotionEffectSet`, `StopJuneEmotionEffectSet`, ctor, dtor, `StateStart`, `PlayEmotion`), `X2Lib/X2TFieldGame.cpp` (`RemoveUnit`), `X2Lib/X2UnitViewerUI.cpp` (`PlayEmotion`), `X2Lib/X2BaseUnitViewerUI.cpp` (`JuneViewerFxPlay` NULL-skin return) | -- |
| `SERV_IRUHADEV_OFFLINE_NPC_UID_BASE` (offline design, not server/June) | 2026-10-07 (june-buff Phase 14) | `KTDXLIB/Always.h` (end) | `X2Lib/Offline/X2OfflineServer.h` (room `Clear()`: monster UIDs start at 1,000,000) | -- |
| `SERV_IRUHADEV_JUNE_HP_RATE_DAMAGE` | 2026-10-07 (June effect completion C10b-1) | `KTDXLIB/Always.h` (end; needs `SERV_IRUHADEV_JUNE_DAMAGEDATA`) | `X2Lib/X2GameUnit.h`/`.cpp` (`GetHpRateDamage`, the `GetFinalDamage` call before the `<= 0` clamp), `X2Lib/X2GUUser.cpp` (`DamageReact`, before the shown-damage copy) | -- |
| `SERV_IRUHADEV_JUNE_NPC_FURY_MODE` | 2026-10-07 (June effect completion C4c) | `KTDXLIB/Always.h` (end; the anim half also needs `SERV_IRUHADEV_JUNE_NPC_ANIM_SPEED`) | `X2Lib/X2GUNPC.h`/`.cpp` (`FLAG_FURY_MODE`, StateData rates, `LoadStateData` `FURY_MODE` + `FURY_DATA`, `m_bFuryMode`, `SetFuryMode`/`GetFuryMode`/`JuneGetFuryAttackPowerRate`, the `ResetAnimSpeed` replace), `X2Lib/X2GameUnit.cpp` (`GetFinalDamage`, before the range rate), `X2Lib/X2Main.cpp` (2 binds) | -- |
| `SERV_IRUHADEV_JUNE_NPC_ANIM_SPEED` | 2026-10-07 (June effect completion C4c) | `KTDXLIB/Always.h` (end) | `X2Lib/X2GUNPC.h`/`.cpp` (two 1.0 factors, June's `ResetAnimSpeed` body), `X2Lib/X2GameUnit.h` (virtual `ResetAnimSpeed`), `X2Lib/X2GameUnit.cpp` (`CheckAndUpdateAdditionalStat` anim block) | -- |
| `SERV_IRUHADEV_JUNE_HERO_NPC_ANIM_SPEED` | 2026-10-07 (June effect completion C4d) | `KTDXLIB/Always.h` (end; the `CreateGUNPC` tail and the setter also need `SERV_IRUHADEV_JUNE_NPC_ANIM_SPEED`) | `X2Lib/X2DungeonManager.h`/`.cpp` (row list, `Add_HeroDungeon_ExtraStatForException_LUA`, shared `JuneIsHeroDungeon`), `X2Lib/X2Main.cpp` (bind), `X2Lib/X2DungeonSubStage.cpp` (`CreateGUNPC` tail + one log line), `X2Lib/X2GUNPC.h` (`SetMultiplicationAnimSpeed`), `X2Lib/X2PartyUI.cpp` (its `JuneIsHeroDungeon` calls the shared one) |
| `SERV_IRUHADEV_JUNE_EQUIP_EFFECT_SET` | 2026-10-07 (June effect completion C9f-2) | `KTDXLIB/Always.h` (end; the viewer half also needs `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER`) | `X2Lib/X2ItemManager.h`/`.cpp` (two maps, `AddDivisionItemTemplet_LUA` dispatcher, CEPE parser, lookups, load + log line), `X2Lib/X2Main.cpp` (bind), `X2Lib/X2Data.cpp` (`ResetItemManager` load), `X2Lib/X2Eqip.h`/`.cpp` (handle + timers, release, timer step), `X2Lib/X2GUUser.h`/`.cpp`, `X2Lib/X2SquareUnit.h`/`.cpp` (condition, play, last position; square stop), `X2Lib/X2TFieldGame.cpp` (`RemoveUnit` stop), `X2Lib/X2UnitViewerUI.h`/`.cpp` (group filter, viewer play), `X2Lib/X2BaseUnitViewerUI.h`/`.cpp` (`IsPlaying`) | -- |
| `SERV_IRUHADEV_JUNE_BUFF_INSTANCE_KILL` | 2026-10-07 (june-buff Phase 14b) | `KTDXLIB/Always.h` (end) | `X2Lib/X2GUUser.h`/`.cpp` (`m_bJuneInstanceKill`, `HaveInstanceKillDebuff`, set and kill in `DamageReact`), `X2Lib/X2BuffTempletManager.cpp` (load line) | -- |
| `SERV_IRUHADEV_JUNE_TUTORIAL_MODE` | 2026-10-07 (june-buff Phase 14c) | `KTDXLIB/Always.h` (before the 14b block) | `X2Lib/X2Main.h`/`.cpp` (out-of-line `SetIsPlayingTutorial`, `JuneOnExitTutorialRestoreKeys`, `m_bJuneTutorialKeyLatch`), `X2KeyPad.h`/`.cpp`, `X2DungeonGame.cpp` (Release), `X2GUUser.cpp` (clamp, RT_FLY, StartEnd), `X2Game.cpp`, `X2State.cpp`, `X2UIManager.cpp`, `X2UIQuestNew.h`/`.cpp`, `X2StateDungeonGame.cpp`, `X2SlideShot.cpp`, `X2PetManager.cpp`, `X2PlayGuide.cpp`, `X2UIQuickSlot.cpp`, `X2StateServerSelect.cpp` | -- |
| `SERV_IRUHADEV_JUNE_TOUCH_LAND` | 2026-10-07 (June effect completion C7) | `KTDXLIB/Always.h` (end; needs `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`) | `KTDXLIB/KTDGXMeshPlayer.h`/`.cpp` (`m_vVelocityBefore`, ctor init, per-frame snapshot, `GetGravity` / `GetVelocityBefore`), `X2Lib/X2DamageEffect.h`/`.cpp` (OnFrameMove gate, `JuneTouchLandDamageEffect`, `CreateTouchLandDamageEffect`); C7's other three items extend `SERV_IRUHADEV_JUNE_SPAWN_INHERIT` (`X2GameUnit.h` vslot 0x328, CEffect ctor hook, `JuneApplyCreateDamageEffectRate`) and `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME` (CREATE-loop INHERIT_CRITICAL, `JuneAssignLockOnData`, die INHERITLOCKONDATA) | -- |
| `SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST`, `SERV_IRUHADEV_JUNE_FIX_EFFECTSET_BUFF_NULL` | 2026-10-08 (June effect completion C6c) | `KTDXLIB/Always.h` (end; the list needs `SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME` and `SERV_IRUHADEV_JUNE_ENCHANT_PARITY`, the fix needs the list) | `X2Lib/X2EffectSet.h`/`.cpp` (`BuffFactorListEntry` / `BuffFactorListMod`, `EffectSetInstance::m_vecBuffFactorList`, `AttachBuffFactorToEffectSet`, `ApplyBuffFactorListToDamageEffect`, the call in `OnFrameMove_CreateEffect`), `X2Lib/X2BuffFactor.h` (`JuneSetFinalizerValue`), `X2Lib/X2GUUser.cpp` (SIEE 15 attach in `ProcessSocketEffectEx`) | -- |
| `SERV_IRUHADEV_JUNE_SUPER_ARMOR_CONDITION`, plus the C3c part of `SERV_IRUHADEV_JUNE_DAMAGEDATA` | 2026-10-08 (June effect completion C3c) | `KTDXLIB/Always.h` (before the costume-shop block; the `_DAMAGEDATA` part needs `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`) | `X2Lib/X2GameUnit.h`/`.cpp` (`StateParam::setSuperArmorCondition`, `JuneCollectUnitsAround`, `JuneIsPacketBuffRefused`, the two additions in `SetKBuffFactorPacketToGameUnit`), `X2Lib/X2GUUser.h`/`.cpp` (`JuneIsSuperArmorAgainst`, `CommonStateStart` parse, six `DamageReactStateChange` cases, `Process_ExtraDamage` EDT_TRANSIT_DEBUFF member loop), `X2Lib/X2DamageEffect.h` (friend), `X2Lib/X2DamageManager.cpp` (two rule-5 log blocks off, BOUNDING text) | -- |
| `SERV_IRUHADEV_JUNE_USER_STATE_ID16`, `SERV_IRUHADEV_JUNE_USER_ACTION_ID16` | 2026-10-08 (June effect completion C3d-2) | `KTDXLIB/Always.h` (end) | `X2Lib/X2GUUser.h` (`SyncData::nowState` / `nowAction`, `UserUnitStateData::stateID`, `UserUnitStateDataMap` key, the two `GetGameUnitState*` casts, `m_OldStateId`, `USI_BOUNDING_*`), `X2GameUnit.h` (`UnitCondition::nextState`), `X2UDPPacket.h` (`m_ucNowState` / `m_ucNowAction`, **P2P wire format**); **data036 `Enum.lua` ships with the exe** | -- (P2P only) |
| `SERV_IRUHADEV_JUNE_RESERVED_STATE_IDS`, `SERV_IRUHADEV_JUNE_WAIT_STATE_CHANGE`, `SERV_IRUHADEV_JUNE_USEFUL_STRING`, `SERV_IRUHADEV_JUNE_NOT_SELECT_UNIT_OFFSET` | 2026-10-10 (June effect completion C9h-4g) | `KTDXLIB/Always.h` (end) | `X2Lib/X2GUUser.h/.cpp` (11 reserved user state ids: every class state id +11, `USI_END` 102; USI_WAIT_STATE_CHANGE handler + helper; USEFUL_STRING / NOT_SELECT parses); **data036 `Enum.lua` ships with the exe** (turning RESERVED_STATE_IDS off needs the previous Enum.lua: `kom_backup_tools\data036.kom.20261010_085357`) | -- |
| `SERV_IRUHADEV_JUNE_BOUNDING_STATE` | 2026-10-09 (June effect completion C3d-3) | `KTDXLIB/Always.h` (end) | `X2Lib/X2GUUser.h/.cpp` (RT_BOUNDING case, 5 handlers, `JuneInitBoundingStates`), `X2GUNPC.h/.cpp` (3 types, `NOT_BOUNDING`, RT_BOUNDING case), `X2GameUnit.h/.cpp` (back speed + last attacker), `X2DamageManager.cpp`; no class calls `JuneInitBoundingStates` yet (C3d-4) | -- |
| `SERV_IRUHADEV_JUNE_LIMIT_MAX_G_SPEED` | 2026-10-09 (June effect completion C3g) | `KTDXLIB/Always.h` (end) | `X2Lib/X2GameUnit.h/.cpp` (`m_fJuneLimitMaxGSpeed`, 1.0, `JuneGetMaxGSpeedCap`), `X2Lib/X2GUUser.cpp` (state key `SET_LIMIT_MAX_G_SPEED` in CommonStateStartFuture, the two PhysicProcess fall clamps), `X2Lib/X2GUNPC.h/.cpp` (StateData key, CommonStateStart copy, both adaptive-frame clamp branches); nothing sets the key today | -- |
| `SERV_IRUHADEV_JUNE_DAMAGE_REACT_HOOKS`, `SERV_IRUHADEV_JUNE_SOCKET_COOLTIME_ON_HIT` | 2026-10-08 (June effect completion C10e) | `KTDXLIB/Always.h` (end) | `X2Lib/X2GUUser.h/.cpp` (two empty attacker hooks, socket 2000145 bool + cool-down), `X2Lib/X2GUNPC.cpp` (`DamageReact` hooks and the 3 % cool-time cut), `X2Lib/X2GameUnit.h` (empty slot-0x304 hook), `X2Lib/X2Game.h/.cpp` (`JunePvpAmusementHalveDamage`) | -- |
| `SERV_IRUHADEV_JUNE_EFFECT_ALPHA_OPTION` | 2026-10-09 (June effect completion C10b-2) | `KTDXLIB/Always.h` (end) | `KTDXLIB/KTDXApp.h/.cpp` (three alpha floats, set / get), `KTDGXMeshPlayer`, `KTDGSlashTrace`, `KTDGParticleSystem` (render readers), `X2Lib/X2GameOption.cpp` (`GameOptions.lua` keys), `X2Game.h/.cpp` (category, five tag helpers, two Lua binds), `X2Main.cpp`, `X2GameUnit`, `X2GUUser`, `X2GUNPC` (owned-NPC category, body / weapon fade), `X2EffectSet`, `X2DamageEffect`, `X2BuffTemplet`, `X2BuffDisplayerTemplet`; log `JUNE_FX alpha` under `SERV_IRUHADEV_JUNE_FX_DEBUG`. Revert: undefine the flag (the rule-5 log lines come back); the game dir's `GameOptions.lua` keeps three unread keys | -- |
| `SERV_IRUHADEV_JUNE_PARTICLE_TRACE_INHERIT` | 2026-10-21 (pilot-char-revamp CH3a review; june-engine R9's particle item, done there) | `KTDXLIB/Always.h` (after the CH5 block) | `KTDXLIB/KTDGParticleSystem.h/.cpp` (`m_bJuneOutOfDGManager`, ctor 0; `CreateNewParticle` = June `31cf9570`: trace children take the parent's effect-alpha byte, add rotate, add rotate rel, axis angle = add rotate, show state, DG removal; skin-mesh particles: no collision data, `Rotate( AddRotateRel + AddRotate )`, JUNE_BUGS 388), `X2Lib/X2BaseUnitViewerUI.cpp` (sets the flag where the viewer leaves the DG manager) | Revert: comment out the define, `python tools/build.py --deploy`. No data change. Visible fix: Void Princess Plasma Cutter's line follows Aisha's facing |
| `SERV_IRUHADEV_JUNE_UI_SLOT_ITEM_TEX` | 2026-10-21 (pilot-char-revamp CH3a review, crash fix) | `KTDXLIB/Always.h` (after JUNE_PARTICLE_TRACE_INHERIT) | `KTDXLIB/KTDGUISlot.cpp` (both `SetItemTex` overloads = June `31cd0650` / `31cd0850`: the item texture data is kept and refilled in place by `JuneResetTexureData_` = `31c962c0`; March's bodies in the `#else`; JUNE_BUGS 389). Fixes the crash in `CKTDGUIControl::RenderVertex` from the locked TC slots A5 / B5 (disabled slot drew freed texture data, ErrorEnum 131 `SlotA5` / `SlotB5` every frame) | Revert: comment out the define, `python tools/build.py --deploy`. No data change |
| `SERV_IRUHADEV_OFFLINE_FIRST_SELECT` (offline emulator) | 2026-10-07 (june-buff Phase 14c) | `KTDXLIB/Always.h` (next to the above) | `X2Lib/Offline/Handlers_Unit.cpp` (`PushSelectUnitNotifications`, 5_NOT) | `SERV_IRUHADEV_OFFLINE` |
| `SERV_IRUHADEV_JUNE_BUFF_DEBUG` | 2026-10-04, **disabled 2026-10-07** (june-buff Phase 14) | `KTDXLIB/Always.h:4787` (`#define` commented out; re-enable to diagnose) | ~250 `[JUNE_BUFF] first ...` debug sites across `X2Lib` (buff templets, game units, users, NPCs, `X2Main.cpp`'s `JuneBuffTestNewStat_LUA` console hook) | -- |

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
- **`SERV_IRUHADEV_JUNE_RENDER_CARTOON_TEX`** (june-engine R1, 2026-10-17) -- puts the C++ `CARTOON_TEX_TYPE` on June's numbering (RED 1 -> 4 ..
  SCARLET 9 -> 12, six `CTT_RIM_LIGHT_*` 13-18, `CTT_END` 19) and loads June's 20 ramps. With the flag on, the Ranox stand-in above uses June's
  `CTT_RIM_LIGHT_BLUE = 15`, mapped back to the `CartoonSkyBlue.bmp` ramp (the rim maps are overlays, black as a cel ramp) until R2 binds the rim ramp the June way.
  **Revert both together:** comment out the `#define`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`; and restore data036 `Enum.lua`'s
  `CARTOON_TEX_TYPE` table (`kom_backup_tools\data036.kom.20261017_192311` holds the pre-R1 archive; `/x2-kom-packed copy data036` from a
  repacked kom). data042's 10 added ramps are inert with the flag off. Flag on + old `Enum.lua`, or flag off + new `Enum.lua`, mis-indexes ramps.
- **`SERV_IRUHADEV_JUNE_RENDER_SHADER_CONTRACT`, `_RIM_OPTION`, `_WORLD_CARTOON`** (june-engine R2, 2026-10-17) -- the renderer contract. The shader flag renumbers
  `EFFECT_PARAMETER` to June's 33 entries (setters skip handles the loaded `XRenderer.fx` lacks, so March's file still runs), compiles the effect with June's four
  macros, reads the device caps, adds two ints to `RenderParam`, and ports BeginRender's ramp pick / rim ramp / `g_mView` / `g_vEyeDir` / screen size and DrawFrame's
  texture ops, rim and grading state. It supersedes R1's SkyBlue stand-in (`SetRimLightTexType`'s mapping is bypassed). The option flag adds the four
  `GameOptions.lua` keys (`RIM_LIGHT = TRUE` turns the rim on once June's `XRenderer.fx` is in data041). The world flag stores the world's two ramp indices
  (Lua `SetCartoonTextureIndex` / `SetRimLightTextureIndex`) and copies them into five unit types' `RenderParam`. **Revert:** comment out the three `#define`s
  (the other two need the first), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. Nothing else moves with them (no data was changed).

- **`SERV_IRUHADEV_JUNE_RENDER_ENVBOX`** (june-engine R3, 2026-10-17) -- June's environment-box map, ported whole and **inactive, as in June**. `CKTDGEnvBoxMap`
  (`KTDXLIB/KTDGEnvBoxMap.*`, June `31d09b30` ... `31d0d100`) holds colour-grading LUT textures and oriented boxes (`g_pEnvBoxMap:SetLUT`, `:AddEnvBox_LUA{...}`,
  made by `g_pWorld:SetEnvBoxMap( "File.lua" )`); each frame the world gives it the camera unit's position and it blends the LUTs of the boxes around that point
  (ping-pong passes into two 256x16 targets, technique `BlendLUTTechnique`) and their rim-light values, and writes the result into the renderer (`+0x59` .. `+0x7c`, the
  R2 members). Nothing runs unless a script calls `SetEnvBoxMap` (no June script does) *and* `USE_ENV_BOX = TRUE` in `GameOptions.lua` *and* `XEnvBoxRenderer.fx` is in a kom
  (asset-port P4a). The one-time log line `[JUNE_ENGINE] ENVBOX hook alive: ...` proves the Lua binding. **Revert:** comment out the `#define`, `touch X2Lib/stdafx.cpp`,
  `python tools/build.py --deploy`; the two new files stay in the project and compile to nothing.

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

### Level cap 99 (Atlas Phase A2)

**Defined in** `KTDXLIB/Always.h` -- `SERV_IRUHADEV_LEVEL_CAP_99`, after the CAP_80 block above (which stays).
Full record: `ATLAS_PORT_PHASES.md` "Phase A2 — Done". It goes together with three data changes:
- data036 `ExpTable.lua` is June's, levels 1-100 (`luasrc.py adopt-june`);
- `KncWX2Server/ServerResource/US/StatTable.lua` and `NpcExpTable.lua` gain rows 81-99 (`june2017_re/cap99_gen.py`);
- the save moves onto June's EXP curve once (`CX2OfflineDB` schema v16, `[CAP99] migrated unit` lines).

`SERV_IRUHADEV_EXP_GAUGE_MAX_LEVEL_FIX` (separate flag): the menu EXP gauge reads a full 100.0% at the cap instead of
the 0 / 0 NaN that printed "-1.$%".

**Reverting is not just undefining the flag.**
- The save is at schema v16 with June-curve EXP. An exe without the flag refuses it ("save is schema v16").
- Revert all of these together:
  - restore `db_backup/els_db_20261017_000918_pre_pre_A2_cap99_exp_migration.sql`, which loses play since the migration;
  - put March's ExpTable back (the backup under `kom_backup_stage_june/20261017_001003`, then pack + copy data036);
  - undefine the flag.
- The StatTable/NpcExpTable rows 81-99 are harmless under 80: the loaders drop them.
- `tools/createchar.py` reads `tools/data/june_exptable.tsv`. Point it back at `march_exptable.tsv`.

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
~~**Play-test pending**~~ **Play-tested 2026-10-03** (`JUNE2017_EFFECT_REBUILD_PLAN.md`, dev-console
pass T1: the `DAMAGE_EFFECT_Trap_Fire_Big` loop and `SET_DIE_WHEN_MASTER_EFFECT_DIE` both passed, exe
`cccaf8fb…`). Ask before launching, every time.

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

Built `US_SERVICE` (`touch X2Lib/stdafx.cpp` first; KTDXLIB, X2Lib and X2 all 0 errors, no warnings in the edited files) and **deployed to `X2_offline.exe`, 18,952,704 B**, SHA-256 prefix `dc5c6c4fef65ae2d`, 2026-09-29. The new format string is present in the binary. `census.py`: all 8 rows `unsupported` with a March site and `same` semantics (three by hand in `HAND_SEMANTICS`: register defaults), all 12 checks PASS. ~~**Not play-tested yet.**~~ **Play-tested 2026-10-03** (`JUNE2017_EFFECT_REBUILD_PLAN.md`, dev-console pass T5): all seven expected lines, plus two `APPLIED_SKILL_ID` lines that the repacked `Enum.lua` (June `SKILL_ID`s, `RNSI_SA_RHB_LOCKON_STRATOS = 31012`) now makes reachable.

**What the deployed data can show.**
- `NATURAL_FORCE_TYPE`: five Rena templets (`DOUBLE_JUMP_X_ARROW`, `DOUBLE_JUMP_XX_ARROW`,
  `NATURAL_FORCE_EXPLOSION_BOOM_COMBO`/`_SKILL`, `LESI_COMBAT_RANGER_..._Z_UP_DAMAGEEFFECT`) and
  `DamageEffect_DRYADKENTA_SpacialAttackB_Fire`.
- `NUMERICAL_DATA_INDEX`: `DamageEffect_SA_RHB_Lockon_Stratos_Attack`.
- The deployed `APPLIED_SKILL_ID` names a `SKILL_ID` member March lacks, so it arrives as nil and
  never logs.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEDATA`. This phase is pure addition: 181 lines inserted,
none deleted.

### June2017 effect engine: damage triggers and nearest bone (effect completion C5)

C5 of `JUNE2017_EFFECT_COMPLETION_PHASES.md`, extending `SERV_IRUHADEV_JUNE_DAMAGEDATA` (no new flag),
2026-10-06. Files: `X2Lib/X2DamageManager.{h,cpp}`, `X2GameUnit.{h,cpp}`, `X2EffectSet.{h,cpp}`,
`X2DamageEffect.cpp`. Pure insertions (flagcheck: 0 deleted lines).

- **Parse** (`SetDamageDataFromLUA`, June `32632390 @3263d396-3263db70`, `@3263e8d7-3263ead2`): new `DamageData` members
  `m_vecAdditionalAttackTriggerType` (every value kept, zeros too), `m_vecDamageTriggerData` (`DamageData::JuneDamageTriggerData`
  entries `{ATTACK_TRIGGER_TYPE, BUFF_TEMPLET_ID, POWER_RATE = 1.0, POWER_RATE_ACCUMULATION_MULTIPLIER = 0.0}`) and
  `m_iDamageTriggerCondition` (default -1). Both vectors are emptied on every parse, as June does, so `DAMAGE_DATA_NEXT` does not
  inherit them. `DAMAGE_TRIGGER_CONDITION` is parsed and logged under plan rule 5 (June has no reader).
- **Readers:** `DamageData::JuneIsAttackTrigger( int )` = June `32610fb0`; `JuneFindDamageTriggerData( int )` = `32610e60`.
- **Consumer 1:** `CX2GameUnit::JuneApplyMultipleDamageByDebuff` (`3227c940`), called in `GetFinalDamage` right after the
  `damage <= 0` clamp (`@322869b2`): for trigger type 40 (`DTT_MULTIPLE_DAMAGE_BY_DEBUFF`) the damage is multiplied by POWER_RATE,
  or by `(POWER_RATE - 1) * (n - 1) * MULT + POWER_RATE` when MULT > 0, with n the defender's stack count of the entry's buff (n > 0 only).
- **Consumer 2:** `EffectSetInstance::JuneSetTraceNearestBone` (`321da350`), called in `CEffect::AttackResult`'s
  `RT_CREATE_EFFECT_SET` case after the DELAYTIME block; the new `EffectSetInstance::m_wstrTraceNearestBone` replaces the effect's
  bone in the `ESM_GAME` branch of `GetEffectPosition` and `GetEffectPositionByBoneName` for a trace target (June also reads it in
  the field and pet modes; not ported, nothing can set it there). Name from the new
  `CX2GameUnit::JuneGetNearestCollisionFrameName` (`32278580`).
- **Log lines** (`SERV_IRUHADEV_JUNE_FX_DEBUG`, offline_server.log): `JUNE_FX trigger_parse`, `JUNE_FX trigger_data`, `JUNE_FX trace_nearest_bone`.
- **Inert on today's data:** no March `data036` script sets any of these keys; they act once a June DamageEffect that does is adopted.

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEDATA` (this also reverts every earlier 8a-8c addition), or delete the blocks above.

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
- ~~**Known gap.**~~ **Closed by C8b (2026-10-09).** June's `UnitToUnit` check (`32611970`) inlines the
  same code, so the other four hit paths are switched over too (see "C8b" below).

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
The trace strings are present in the binary. ~~**Not play-tested yet.**~~ **Play-tested 2026-10-03** (dev-console pass T6): `DamageEffect_MAG_CHARGER_Attack_A` logs `COLLIDE defender=0 uid=12 … march=1` against the player, there are no `DIFF` lines, and every `TEST` line has `linked=1` with `c` equal to `mesh`. A level-70 charger's fireball visibly damaged the player (a level-1 `/CM` spawn does 0, which is the level gap, not a defect). A spread of far `TEST` samples is still unexplained; see T6.

Revert: undefine `SERV_IRUHADEV_JUNE_COLLISION`. Pure addition: 331 lines inserted, none deleted.

**C8b (2026-10-09): the other four hit paths (`SERV_IRUHADEV_JUNE_COLLISION`, extended) and the
`SetDamageExceptionProcess` layers (new flag `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION`).**

*The four hit paths.* Every path takes `EffectToUnit`'s pair math (`JuneEffectCollisionDataCheck`, left
untouched) with the advance branch off (`USE_ADVANCE_COLLISIOIN_CHECK` is read in `EffectToUnit` only).
March's calls stay in the `#else`. All four are in `X2Lib/X2DamageManager.cpp`:

| path | June | March site | shape |
|---|---|---|---|
| UnitToUnit | `32611970` | `UnitToUnit`'s call of `CollisionDataCheckFromUnitToUnit` | attack set, sub set, each pair sub set, each a listSet x listSet instance; returns at the first hit or guard-only |
| UnitToEffect | `326413c0` | the lambda in `UnitToEffect` | the same three instances against the defender effect's one list |
| MeshToUnit | `32625670` | `MeshToUnit` | the attacker animation's attack list x the defender's set |
| CollisionDataToUnit | `32628890` | `CollisionDataToUnit` | the defender's set (A) x the DamageData's manual datum (B); no guard call; the reported type is B's, which the caller maps to `m_bGuard` |

- Each listSet x listSet instance runs `JuneEffectCollisionDataCheck` once per attack list. The result is
  the same as June's single pass: a guard seen first is what a later hit reports anyway, and a guard
  alone reports `CT_GUARD`.
- **What changes in game.**
  - Hit and miss: line x sphere and sphere x line pairs only (the clamped closest point), as in
    Phase 9. Sphere x sphere hit and miss is March's. The melee path is live on every hit.
  - Impact point: halfway from one centre to the other, so hit particles and `DEPT_IMPACT_POINT`
    effects can move on line-shaped hitboxes.
- **Trace** (`SERV_IRUHADEV_JUNE_FX_DEBUG`): `JUNE_FX HIT <path> COLLIDE | DIFF ...`, one line a second per
  path and branch, `<path>` = `UNIT_TO_UNIT`, `UNIT_TO_EFFECT`, `MESH_TO_UNIT`, `COLLISION_DATA_TO_UNIT`.
  `DIFF` runs March's test on the same pair. `EffectToUnit`'s COLLIDE / DIFF / TEST lines gain
  `path=EFFECT_TO_UNIT`, and TEST gains `owner=<uid> age=<now>/<max>` (the main mesh's life time).
  6 lines of Phase 9's trace were rewritten for that; they sit inside the flag and the debug flag.
- **Not ported, recorded:** `DamageData +0x62c` (June writes it at each hit; no reader found).

*`SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION`* (`KTDXLIB/Always.h`; `#error` without `_JUNE_DAMAGEDATA`,
`_JUNE_BUFF_NEW_STAT`, `_JUNE_BUFF_CUSTOM_FUNCTION`, whose DamageData members the switch writes). **Every
`fPowerRate` multiply is unchanged, so no damage number moves; on March data the whole flag is inert.**
- **`+0x4108`.** `CX2GameUnit::m_iJuneChangeDamageAttackType` (2 in the ctor `@322a5bf7`, -1 in
  `ResetDamageType`). The user layer applies Raven's damage-type change only when it is -1 or equals the
  hit's attack type (`32045340 @320453a0`). Raven's `ResetDamageType` leaves -1, so March data never
  fails the gate; the `= 2` write belongs to the Raven port.
- **User vslot `0x758` (`3202ac40`), the skill-memo switch.** `CX2GUUser::JuneApplySkillMemoToDamageData`,
  transcribed case by case. Its lookup `31fdbbb0` is a plan-rule-5 stub (`GetJuneSkillMemoEntry`: returns
  none, logs `JUNE_FX unsupported skill memo` once), so the switch never runs. Ported cases: 1/4, 2
  (DamageDistance half), 5, 6, 8, 0xd, 0x11, 0x12; the `APPLIED_MEMO_ID` test reads
  `DamageData::m_iAppliedMemoID`, and C3's "unsupported" log line for that key is compiled out. Skipped, each
  named in a comment: `+0x4f8` (case 0xc), `+0x5d0` (case 2; see the C3 review note: the CEffect scale
  base is not ported, so mapping it would reset a scaled child), `+0x5dc` (case 0x16), `+0x5e4` (cases
  1/4/5/6 tail), cases 0xf/0x10 (`32089ff0` not decoded). Cases 5 and 6 fall into the shared
  tail with v1 (JUNE_BUGS 144).
- **Call order in the user layer** (user attacker only, as June's early-outs): Raven block, `0x758`, the
  socket copy (`_JUNE_SOCKET_EDT`, already in), vslot `0x780`.
- **`0x780`** `CX2GUUser::JuneDamageExceptionClassHook( DD, fPowerRate )`: empty, called last; Ara
  overrides it (Ara port).
- **NPC `32759f30`.** `CX2GUNPC::SetDamageExceptionProcess`: the base, then the owner's `0x758` when the
  owner is a live user whose CX2Unit and unit templet are set with a non-zero type. A non-user owner is
  skipped (June would read its `+0x867c` through the wrong layout; JUNE_BUGS 145).
- **Not ported:** DD `+0x638` (vslot `0x2e0`, always -1); the socket copy to DD `+0x3b4`
  (june-buff 16-step3c's, already in); the eight per-class overrides (checklist 7b, the character ports).

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION` for the layers. The four paths go back to March with
`SERV_IRUHADEV_JUNE_COLLISION`, which also reverts `EffectToUnit` (Phase 9). The functions were found with
`june2017_re/vtslot.py` (vtable slot `0x2b8`); every row is in `funcmap.tsv` (`Effect C8a`).

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
present in the binary. ~~**Not play-tested yet.**~~ **Play-tested 2026-10-03** (dev-console pass T7): no tokenizer MessageBox, no alpha regression, and `MP_CRASH` fires for the Mag Charger fireball (`LT_TIME t=1.264`).

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
independent routes to the reported softlock (**correction, `RANOX_LUA_DECOMPILE_FIX_PLAN.md`'s
Phase 2b calibration, 2026-09-28: the real bytecode-verified count is 12 dropped assignments in
these same 5 functions -- this "six" undercounts by treating one comment that covers two
separate writes as a single instance, 4 times over; see the plan's "Calibration" section for the
full per-write table. The fix below restores all 12, not 6** -- the functional description and
the fix itself were always right, only this summary sentence's count was low):

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

### Ranox Lua decompile fix: tooling (corpus harness, fix, and the 62-file repair)

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
  matches either the exact SHA-256 it was written against or the one `ljd_slotworks.patch`
  produces (already-patched, treated as a no-op, not an error). `june2017_re/ljd_slotworks.patch`
  is the same change as a unified diff for the two vendored ljd copies -- **applied 2026-10-01**
  (the user's call) to both `elstools/vendor/ljd` and the second copy `elstools.exe` runs, so
  `elstools.exe decompile-batch` has the fix directly now too, not just `ljd_jun2017.py`. Both
  copies confirmed `diff -rq` clean against each other after patching, and both match the
  SHA-256 the patch's own header records. One consequence: `--unfixed` no longer produces
  genuinely unfixed output anywhere, since the fix is now in the vendored file itself.
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

### June UI parity, Phase 1: Local Map custom messages

New flag `SERV_IRUHADEV_JUNE_LOCAL_MAP_UI` (`KTDXLIB/Always.h`). Adds three
`LOCAL_MAP_UI_CUSTOM_MSG` members (`X2Lib/X2LocalMapUI.h`) that June's Local Map dialog
needs and March lacks, at June's own values (`JUNE_UI_PARITY_PHASES.md` Phase 0 findings):
`LMUCM_DUNGEON_MODE_SELECTED` (18005), `LMUCM_LOCAL_MAP_ARROW_LEFT` (18009),
`LMUCM_LOCAL_MAP_ARROW_RIGHT` (18010) -- explicit initializers on all three so the
intentional 18006-18008 gap (Elysion-only, not added) can't shift the arrow values
implicitly. Matching Lua side in `march2014/data036/Enum.src.lua`'s
`LOCAL_MAP_UI_CUSTOM_MSG` table, same explicit values. No `LMI_*` content IDs and no
`PARTY_UI_CUSTOM_MSG` members added -- out of this phase's scope, see the doc.

Verified end to end through the real bytecode: compiled the edited `Enum.src.lua` with
`elstools compile-batch -v mar-2014`, decompiled the result back and confirmed all three
names and values round-trip, and confirmed the *previous* committed `Enum.lua` was already
byte-identical to a fresh compile of the pre-edit source (no drift to inherit before
editing). Also confirmed with a temporary `#pragma message` that the flag reaches
`X2LocalMapUI.cpp` through the PCH, not just that the build exits 0.

Built `US_SERVICE`, rc 0, all three projects (`KTDXLIB`, `X2Lib`, `X2`). `x2.exe`:
18,979,840 B, 2026-10-01 09:03. Deployed to `X2_offline.exe`, confirmed by size and mtime.
Not launched -- play-test checklist in `JUNE_UI_PARITY_PHASES.md`'s Phase 1 result.

**Needs packing**: `march2014/data036/Enum.lua` (265,246 B, was 265,151 B), copied into
`kom_contents/data036_kom/Enum.lua`/`.src.lua` too. This repack also carries every other
`data036` enum addition already staged ahead of `kom_contents` before this phase (Ranox
Phase 1, ExtraNPCStat, the June2017 effect rebuild's `HIT_TYPE`/`BUFF_FACTOR_ID` work) --
intentional, but the play-test should expect all of it live together, not just this
phase's three values.

Revert: undefine `SERV_IRUHADEV_JUNE_LOCAL_MAP_UI`; the three enum members become dead
code (nothing reads them yet). The Lua table entries are additive and harmless either way.

### June UI parity, Phase 3: local-map dungeon buttons, Special tab (Henir)

New flag `SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS` (`KTDXLIB/Always.h`), which needs
`SERV_IRUHADEV_JUNE_LOCAL_MAP_UI`. Every March village's local map (and Lanox's)
now uses June2017's per-village luas.
- There are no clear-difficulty stars.
- Henir moved to June's Special tab, `LMI_SPECIAL = 10011`.
- Each secret dungeon icon now names its `_HELL` dungeon and gets a
  `SecretDungeonDesc` lock button.

The C++ that drives those luas is ported whole from June, in
`X2Lib/X2LocalMapUI.{h,cpp}` and `X2Lib/X2LocationManager.h`:
- `UpdateDungeonButtons` and `SecretDungeonDesc`;
- the `LMUCM_DUNGEON_CLICKED` case. Secret dungeons now start as
  `DM_SECRET_HELL`, which is June behaviour and the user's decision of
  2026-10-01;
- the Special tab in `UpdateLocalMapDLG`;
- June's `DT_SECRET` → `DM_SECRET_HELL` mapping in the three handlers that
  reset a party's mode: `X2PartyManager.cpp` (party closed) and
  `X2StateBeginning.cpp` / `X2StateServerSelect.cpp` (unit selected).

Details, the June-only omissions, the pack list and the play-test checklist are
in `JUNE_UI_PARITY_PHASES.md`'s "Phase 3 result".

Built `US_SERVICE`, rc 0. `x2.exe`: 18,981,888 B, 2026-10-01 13:29. Deployed to
`X2_offline.exe` (byte-identical). Not launched.

**Needs packing**: `data034`, `data036`, `data065`, `data215`, `data217`, and a
**new** `data151` archive. Every file is already in `kom_contents/`.

Revert: undefine `SERV_IRUHADEV_JUNE_LOCAL_MAP_ICONS`. The C++ returns to
March's/Phase 2's versions. The June luas still load, but stop working
properly:
- their secret icons carry `_HELL` IDs under March's `DM_SECRET_NORMAL`;
- the hero filter and lock buttons are never hidden;
- the Special tab is counted as a page-2 tab.

So a full revert also means restoring March's own local-map luas: their live-kom originals
are in `...\24965799\kom_contents_backup_june_ui_phase3\` (copy back into `kom_contents`,
drop the Phase 3 manifest entries, repack).

### June UI parity, Phase 5: party/dungeon panel

New flag `SERV_IRUHADEV_JUNE_PARTY_UI` (`KTDXLIB/Always.h`). The party/dungeon panel
follows June2017. The C++ is ported whole from June into `X2Lib/X2PartyUI.{h,cpp}`:
- `OpenPartyMenu`, which loads June's `DLG_UI_PARTY_BOTTON_NEW_20161103.lua`, and
  `UpdateMatchingUI` (the wait panel's new Heroes Assemble line);
- `UpdatePartyDLG`. The region plate is picked by local-map ID, so Lanox gets one;
- the `UpdateDifficultyUI` family plus the new `UpdateAdequateLevel`. A secret
  dungeon now shows no difficulty boxes;
- `SelectDungeonDifficulty`;
- an OK/Cancel confirm before "start with current members"
  (`PUCM_PARTY_GAME_START_OK = 14074`).

June's `DLG_UI_PARTY_LENGTH_NEW.lua` is adopted with its June-only controls dropped.
Three atlases come from June: `Texture_Field_00` and `Texture49_NEW` (data057), and a
new `DLG_UI_Text_05` (data151).

June's PvP union dialog is **not** ported. It sends a multi-mode match request that
March's `KEGS_REQUEST_MATCH_MAKING_REQ` cannot carry, so the PvP side stays March's.

Details, the June-only omissions, the pack list and the play-test checklist are in
`JUNE_UI_PARITY_PHASES.md`'s "Phase 5 result".

Built `US_SERVICE`, rc 0. `x2.exe`: 18,987,008 B, 2026-10-01 17:46. Deployed to
`X2_offline.exe` (byte-identical). Not launched.

**Needs packing**: `data034`, `data057`, `data151`. Every file is already in
`kom_contents/`. Ship `data034` before playing: with March's LENGTH lua still packed,
the difficulty boxes stop updating (logged as `[JUNE_PARTYUI] ... no VSFOUR`).

Revert: undefine `SERV_IRUHADEV_JUNE_PARTY_UI` and rebuild. March's `DLG_UI_PARTY_BOTTON_NEW.lua`
is loaded again and still packed. June's LENGTH lua works under March's C++ apart from
the three extra controls showing at their lua default. A full data revert restores the
originals from `...\24965799\kom_contents_backup_june_ui_phase5\` (README inside).

### June buff port, Phase 3: the buff loader survives June data

New flag `SERV_IRUHADEV_JUNE_BUFF_LOADER` (`KTDXLIB/Always.h`).
- **June's manager functions.** `CX2BuffTempletManager`'s `AddBuffTemplet_LUA`, `AddBuffFactor_LUA` and `GetBuffFactorPtr` are
  June's (`326c3990`/`326c4050`/`326c32a0`). This includes the second factor vector for `_INT` ids 10001+ (`326c3f30`) and loading
  `BattleFieldBuff{Templet,Factor}_INT.lua` after the main files.
- **Placeholders.** A factor that passes the ordinal check but fails to parse becomes a NULL placeholder, so later ids no longer
  shift. It is never applied. This is a deliberate deviation from June and March, which both drop it.
- **No popups while parsing.** Every `DISPLAY_ERROR` raised while a buff entry parses is captured: `KTDXCommonFunc.h`'s
  `DisplayErrorMessage` passes it to a per-thread sink. The entry then writes one `[JUNE_BUFF] unsupported ...` line to `ErrorLog.txt`
  instead of raising a modal MessageBox, and a `[JUNE_BUFF] load summary` line follows the factor files.
- **No behavior change on March's data.** The same 364 factors load, with no placeholders. The two `BUFF_FACTOR_ID invalid` boxes
  every launch used to raise become `ordinal reject` lines.

Design, the expected log lines for both March's files and a June trial, and the trial directory are in
`JUNE_BUFF_PORT_PHASES.md`'s "Phase 3 — Done". The prediction/compare tool is `june2017_re/buff_trial_predict.py`.

Built `US_SERVICE`, rc 0. `x2.exe`: 18,999,808 B, 2026-10-03 03:55. Deployed to `X2_offline.exe` (md5 match). Not launched.

**Needs packing:** nothing. **Revert:** undefine `SERV_IRUHADEV_JUNE_BUFF_LOADER` and rebuild all four projects (it sits in the PCH).

### June buff port, Phase 8: new finalizers and the buff-change sync (changes the P2P wire format)

New flag `SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER` (`KTDXLIB/Always.h`).
- June's finalizers `BFT_AURA` (14), `BFT_STICK_SHAKE` (15) and `BFT_USE_SKILL_BY_SKILL_SLOTTYPE` (16), with their unit counters and
  the sites that move them: left/right alternation while a BFT 15 buff runs, and one count per skill use (any, and per slot type).
- June's buff-change counter (`CX2GameUnit +0x6dd0`) and its resync: a peer whose copy of a unit's counter disagrees for 1 s asks
  that unit's owner for its buff list (new P2P message `XPT_BUFF_INFO_REQ` = 27, answered with the existing `SendBuffInfoNot`).
- **Every peer in a game must run the same build.** The user sync's field flag is now 32 bits (June's width) and carries a new
  DWORD at bit `0x800000`, and message 27 is new. A client built without the flag reads this build's user sync wrongly, and the
  other way round. Offline play has no peers, so it is unaffected. No server code changes.

Details, June addresses and the play-test are in `JUNE_BUFF_PORT_PHASES.md`, "Phase 8 -- Done".

**Needs packing:** nothing. **Revert:** undefine the flag and rebuild all four projects, on every peer.

### Offline monster grade: the emulator ranks monsters as the real server did

Flag `SERV_IRUHADEV_OFFLINE_MONSTER_GRADE` (`KTDXLIB/Always.h`), one flag for all three phases. Plan:
`OFFLINE_MONSTER_GRADE_PHASES.md`.
- **Phase 1, dungeons.** The client parser now keeps a stage script's `MONSTER_GRADE` in `NPCData`, the emulator sends it in
  `KNPCUnitReq::m_cMonsterGrade`, and the NPC's ExtraNPCStat grade follows June's path (`327edb60`/`327ef71d`). This is what wakes
  the June socket boss terms on dungeon bosses with an ExtraNPCStat row (JUNE_BUGS #52 goes live with them).
- **Phase 2, battlefields.** The middle-boss loader reads `SPAWN_GROUP_ID` (before this no middle boss ever spawned offline). Each
  UID keeps its grade, the boss set is tracked by membership, and danger stops rising while a member lives, as on the server.
- **Phase 3, rewards.** EXP and ED take the server's monster type factor (`GetMonsterTypeFactor`): x12 for a boss, x4 for a middle
  boss, x1 otherwise. On dungeons it is `fDungeonFactor`; on fields it multiplies the field EXP/ED factors. The offline 3x boost is
  applied after it. A field middle boss's ED also gets the server's x20. A boss scatters 10-20 coins instead of 2-3, and a coin
  worth more than 1999 ED is the gold bar (127041). Special drops: 70058 for a boss, 70057 at 50 % for a dungeon middle boss, and
  70057 for grade 3 (not for a field middle boss, which the server never gives one). The item lottery is unchanged, as on the
  server. One `[OFFLINE_GRADE]` line per kill in `offline_server.log`.
- **Phase 3b, field middle-boss bonus items.** These come from the server's `FieldBonusDrop.lua`, now in data036: `FieldBonusDrop.src.lua`
  is the server file byte for byte, and `FieldBonusDrop.lua` is its compiled form at algo 2. A field middle boss kill with a drop row
  and some damage from the player drops 70070, 70050, 99988 and one cube 160712. Solo is always the low cube count, as on the server.
- **Phase 3c, die states.** What a death pays follows the server's `IsDropDieState`: KILL_BY_USER, KILL_SELF and BONUS_DROP are
  paid, ALLY and ESCAPE are not, and NO_DROP pays nothing (EXP included). On a field the player must be in the damage map. Dungeon
  hunt quests count every death of a known NPC; field ones need a paid kill with share >= 0.5 and a field drop row. The result kill
  count is the player's own KILL_BY_USER/BONUS_DROP. Mana/blood eaters dying BONUS_DROP drop 70055/70056. Since the review, a field
  monster with no drop row drops no special, static or eater item, as on the server.
- **`SERV_IRUHADEV_OFFLINE_FIELD_QUEST_NO_ROW_DESIGN` (designed, not server/June).** Without a field drop row, a paid kill with share
  >= 0.5 still counts for hunt quests and still drops quest items (Lanox field monsters 5154.. have no row in March data; June's
  server had them). Comment it out for the server's rule, e.g. once a generated drop table supplies the rows.

**Needs packing:** `FieldBonusDrop.lua` in data036 (packed by `kom.py pack data036`; without it the bonus items are off and logged).
**Revert:** undefine the flag and rebuild all four projects (it sits in the PCH). Rewards, coins and drops go back to the flat x1
rates, and no middle boss spawns on fields again. The extra data036 entry is then unused and harmless.

### June effect completion C9: Elsword_DamageData.lua (the `DAMAGE_DATA_KEY_NAME` table)

New flag **`SERV_IRUHADEV_JUNE_DAMAGE_DATA_TABLE`** (needs `SERV_IRUHADEV_JUNE_DAMAGEDATA`). June loads a
107-entry table from `Elsword_DamageData.lua` in the damage manager's ctor (`32620940` from `32622080`),
one record per `DAMAGE_KEY_NAME`, and a `DAMAGE_DATA` block that carries `DAMAGE_DATA_KEY_NAME` has the record
copied over its damage type, PVP rate, tech point, physic/magic, force-down, hit-add-MP, applied skill ID
(only when non-zero) and attack type (`32622220` @`3263edee`). March parsed the key and stored it, with no reader.

| | before | after |
|---|---|---|
| `Elsword_DamageData.lua` | not opened | opened in the ctor; `ErrorLog.txt` gets `JUNE_FX DAMAGE_DATA_LOAD ... rows=N` |
| `CX2DamageManager` in Lua | not bound | bound with `InsertDamageData_LUA` (`g_pDamageManager`) |
| `DAMAGE_DATA_KEY_NAME` | stored, unused | record applied right after the read (miss: logged once per name, nothing applied) |

Data: `Elsword_DamageData.lua` is staged in `kom_contents/data036_kom/` (ljconv route, manifest key added
with `june2017_re/stage_june_fx.py --only Elsword_DamageData.lua --apply`) and packed into `data036.kom`.
Only three June templets use the key, so most play shows no `apply` line.
Revert: undefine the flag; remove `Elsword_DamageData.lua` and its manifest key from `data036_kom` and repack.
`UnitViewerEffectSetTable.lua` is not part of this change: its consumer is not identified (see the plan's C9 Done note).

### June effect completion C9c-1: UnitViewerEffectSetTable.lua (the unit-manager half)

New flag **`SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_TABLE`**. June keeps a per-motion table of "play this effect set" rows on the unit manager
(`3237a1f0` insert, first row wins; `323760f0` lookup with three gates: unit type, unit class, duo) and fills it from
`UnitViewerEffectSetTable.lua` (18 rows) at the end of `CX2Data::ResetUnitManager` (`31ee6100` @`31ee6224`). March had no table.

| | before | after |
|---|---|---|
| `UnitViewerEffectSetTable.lua` | not opened | opened after `CorrelateUnitClassTemplet.lua`, outside `REFORM_SKILL_NOTE_UI`, on the main Lua state; `ErrorLog.txt` gets `JUNE_FX UNIT_VIEWER_FX_LOAD ... loaded=1 rows=18 skipped_duplicate=0 skipped_bad_args=0` |
| `g_pUnitManager:AddUnitViewerEffectSet` | not bound | bound; the handler reads the Lua stack itself (7 arguments, one more than lua_tinker's member functor takes) |
| `CX2GUUser::PlayEmotion` | anim + sound only | looks the emotion motion up and plays the hit on the UI effect-set manager (inert in March: only Rose/Ain rows match) |

Not ported here: June's `SelectVoiceTable.lua` / `AnimationNameTable.lua` opens between Correlate and this file; the `32296f30` tag after the
`PlayEmotion` play (C10b-2's helper); the viewer consumer (`PlayAnim` / `OnFrameMove`, C9c-2b). Deliberate difference from June: a row with a
wrong-typed argument (e.g. an enum name March lacks, which would read as 0 = "any") is dropped and counted in `skipped_bad_args`.
Data: staged with `june2017_re/stage_june_fx.py --only UnitViewerEffectSetTable.lua --apply` (`UnitViewerEffectSetTable` added to its `LUA10`),
packed and copied into `data036.kom`.
Revert: undefine the flag; remove `UnitViewerEffectSetTable.lua` and its manifest key from `data036_kom` and repack.

### June effect completion C9c-2b: the unit-viewer effect player and the retired class effects

New flag **`SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER`**. June's character-select / lobby viewer does not use `CX2EffectSet::PlayEffectSet`: it
runs its own effect player on `CX2BaseUnitViewerUI` (vtable `3347113c`), driven by the C9c-1 table. This phase ports that player, the two
`CX2UnitViewerUI` hooks that feed it, a no-set variant of the mesh trigger runner, and retires the hand-made class effects the table now plays.

| | before | after |
|---|---|---|
| `CX2UnitViewerUI` base | `CKTDGObject` (the empty `CX2BaseUnitViewerUI` only under `MONSTER_TROPHY`, off) | `CX2BaseUnitViewerUI`; the base holds the player (June's own layout). `CX2NPCUnitViewerUI` inherits the base `NotifyShowObjectChanged`, a no-op on its empty lists |
| `PlayAnim` | anim, then hand-made emit-rate switches | anim, clear the request queue, stop group 0, `GetUnitViewerEffectSet( name, class, duo 0 )`, queue the hit |
| `OnFrameMove` | hand-made handles followed their bones | copy the trigger codes and run the base update as the first statements (before the unit test); after the skin frame move, the equip loop and the render-param block: re-arm and drain the queue, then the base update again (order corrected by the C9c-2b review) |
| class effects | MagicKnight, SPK, SBH, ALP, CDC, ETK/EIS (+crescent cut), RTR, RNW made in `SetUnit` / `PlayAnim` | not made; the table sets play them (the Little Devil orb stays) |
| `data036.kom` | `UIEffectSet.lua` 52 sets; `UIMajorParticle.txt` March's (593 KB, 858 sequences) | + the 15 June sets; June's whole `UIMajorParticle.txt` (2719 sequences) + 10 March-only sequences re-appended |

Not changed on purpose: the per-frame, draw, prepare, notify and avatar-capture sites of the retired handles. They test the handle first, and the
handles are never made under the flag, so they do nothing. Not ported: the particle child-sequence draw (`31cf4890`: March chains child sequences
itself), mesh `+0x15c` and sequence `+0x415` / `+0x45a` (unidentified or write-only), `EffectSet_SpaceMoonRabbit_motionFX` (June-only), the
`SetHideSelectUnitWeapon` counterpart (its four callers run right after a unit is set, in a wait motion, when June creates nothing).
Deliberate differences from June: a set missing from the UI effect set is also logged (June skips it silently; corrected by the C9c-2b review); the sound pass skips a set
with no UI-major mesh or particle (June faults); `EFFECT_SET_TOGGLE_VISIBILITY_TIMER` on the viewer trigger path is skipped and logged once (user,
2026-10-07). June quirks kept: JUNE_BUGS rows 79-86.
C9c-2b review (same flag): Play's trace-target tail (`@3295987e`), the land refresh read once per frame (JUNE_BUGS 82 retracted), the target-mesh
branch's frame test, the viewer trigger's non-zero code test, and the frame-move order; all inert on the 15 table sets. Pyro Knight's ready-action
fire is lost (June's row is keyed `SPK_LobbyReadyAction`, March's anim is `SPK_LobbyReadyaction`; user accepted 2026-10-07, checklist 7e).
Data: `UIEffectSet.lua` = March's decompiled source + the 15 June blocks (`kom_contents/data036_kom/UIEffectSet.src.lua`, compiled mar-2014);
`UIMajorParticle.txt` = `14JUN17/data/data036_kom/UIMajorParticle.txt` (2,042,192 B, equal to the June kom entry) with the 10 March-only sequences
before its last brace (`CHUNG_HOWLING01/02`, `CHUNG_STEEL_EDGE02`, `Nasod_King_SpellAttack_Splitlight06`, `PET_PETTE_BERTHE_STEB_3_Attack_C`,
`..._Attack_C_02`, `Pulsewave_Summon_Boss_SpecialAttackB01/02`, `ShockWave_Summon_Boss_SpecialAttackB`, `UI_Eve_Party_Event_StartEffect`).
Revert: undefine the flag, rebuild all four projects (the flag sits in the PCH); restore the two files from `kom_backup_tools/data036.kom.*` and repack.

### June effect completion C9e: June's per-emotion effect sets (inert)

New flag **`SERV_IRUHADEV_JUNE_EMOTION_EFFECT_SET`**. June plays one UI effect set per emotion id (`32355a50`, 11 ids from `0x1c`) from `CX2GUUser::PlayEmotion` (handle `+0x9434`), `CX2SquareUnit::PlayEmotion` (handle `+0x7ac`) and `CX2UnitViewerUI::PlayEmotion` (viewer player, group 1). March's `EMOTION_TYPE` ends at 15, so every path is **inert**; the code is in so the emotion system (`JUNE_PORT_BACKLOG_ORDER.md` Stage 10) only has to add the ids.

| | before | after |
|---|---|---|
| `CX2Unit::GetJuneEmotionEffectSetName( int )` | none | the 11 names, `L""` for any other id |
| GUUser `PlayEmotion` / `OnFrameMove` / dtor | no per-emotion set | plays into `m_hJuneEmotionSetEffectSet`; `OnFrameMove` stops it on every id except June's keep list (28-32, 35, 38-41, 47, 48); dtor stops it |
| SquareUnit `PlayEmotion` / `StateStart` / dtor / `RemoveUnit` | none | plays into `m_hJuneEmotionEffectSet` (no stop before the play); stopped on every `StateStart`, in the dtor and by `StopJuneEmotionEffectSet` from `CX2TFieldGame::RemoveUnit` |
| `CX2UnitViewerUI::PlayEmotion` | anim only | `JuneViewerFxPlay( skin anim, name, 1 )` |
| `JuneViewerFxPlay` (C9c-2b) | a NULL skin anim was not tested | returns at once on NULL, as June `32959640 @32959680` |

Not ported (missing emotion system): the `+0x93dc` / `+0x93ec` / `+0x940c` sit-item vectors, `EmotionData`, `323f34a0` / `323f6fb0`; not identified: `323f56d0` (June `RemoveUnit`). Data: `UIEffectSet.lua` + 11 templets (June source appended, compiled), `UIMajorXMeshPlayer.txt` + 5 entries (before the closing brace); the 5 `.x`/`.X` meshes are not staged. `data036` packed; the copy into the game is the user's decision.
Revert: undefine the flag; remove the 11 `UIEffectSet.src.lua` blocks and the 5 mesh-player entries and repack `data036`.

### June effect completion C6b: the spawner tail, the effect-set stamps and RT_CREATE_EFFECT_SET

Two flags. **`SERV_IRUHADEV_JUNE_SPAWN_INHERIT`** (needs the damage-effect parse and runtime flags) ports June's five spawner-tail callees
as `CEffect` methods (`31f8fdb0` parent-to-child state, `31f649d0` damage rate, `31f64c00` attack-trigger copy, `31f65e40` /
`31f65f90` ignore-defence down the tree) and calls them from the hit, die and CREATE_DAMAGE_EFFECT spawners and RT_CREATE_DAMAGE_EFFECT.
Every gate flag is set only by class code, so in March the visible changes are that hit and CREATE children inherit the parent's
`m_iIndex` and `m_bHyper` (die children already inherited the index), and that every damage effect an effect set makes gets
`m_iIndex` 0 and the owner's state at play time. **`SERV_IRUHADEV_JUNE_CREATE_EFFECT_SET`** (needs `SERV_IRUHADEV_JUNE_DAMAGEDATA`) is
June's RT_CREATE_EFFECT_SET: with a trace unit, the field-boss collision-box retarget gate (unreachable here: March has no raid
object, so the boss unit is a stub that returns NULL); without one, the set plays at the main effect's position, rotation and direction,
and only when the main-effect mesh exists, with no LIFTTIME. `EffectSet_RAVEN_NUCLEAR` and `EffectSet_SP_B_MASK` take that second path:
their sets now appear where the effect is, not on the caster.

| | before | after |
|---|---|---|
| hit / CREATE child `m_iIndex` | -1 | the parent's (Rena's detonations look effects up by it) |
| effect-set damage effect `m_iIndex` | -1 | 0, and `m_iJuneAttackerStateID` = the owner's state when the set was played |
| `ErrorLog.txt` / `offline_server.log` | - | `JUNE_FX CE_HIT ... SPAWN_INHERIT / SPAWN_TAIL / CREATE_EFFECT_SET`, `ES_CREATE ... SPAWN_INHERIT_STAMP` |

Revert: undefine the two flags in `KTDXLIB/Always.h` and rebuild all four projects (they sit in the PCH). JUNE_BUGS rows 74-78 record
the June quirks kept as they are. The save character `NWatcher` (Night Watcher, class 114, level 70, unit 31) was added for the
play-test; the pre-edit backup is `db_backup/els_db_20261007_104747_pre_createchar.sql`.

### June buff port, Phase 14: play-test sweep and close-out

- **`SERV_IRUHADEV_JUNE_BUFF_DEBUG` disabled.** The one-shot `[JUNE_BUFF] first apply/read ...` lines, the per-event
  stick-shake/aura/custom-function lines and the `JuneBuffTestNewStat_LUA` console hook are compiled out. The load-time lines
  (load summary, socket census, rank tables, world buffs) stay. **Revert:** uncomment the `#define` and rebuild all projects.
- **`SERV_IRUHADEV_JUNE_BUFF_STORE_BUFF`** also rewords the stale load line: `IS_STORE_BUFF on 119 templet(s): kept over a game
  change ...` replaces the `unsupported ... IS_STORE_BUFF` text (its consumer was ported in 11b and turned on in 13).
- **`SERV_IRUHADEV_OFFLINE_NPC_UID_BASE` (offline design).** The emulator numbers monsters from 1,000,000. The real server starts
  at 1, which is safe live because character UIDs are large; the save's are 12-30, so a field's 19th monster shared Iivue's UID and
  its clear-all on a field move wiped her buff icons (`CX2GageManager` matches on the UID alone, as June's `32489980` does).
  **Revert:** undefine it (monsters number from 1 again; the icon wipe comes back for any character whose UID a monster reaches).

### June buff port, Phase 14b: IS_INSTANCE_KILL_DEBUFF consumer

- **`SERV_IRUHADEV_JUNE_BUFF_INSTANCE_KILL`.** A player carrying a buff templet with `IS_INSTANCE_KILL_DEBUFF` (in March data
  only the `_INT` Christmas master, BFI 10066) dies at the next damaging hit: June's user `DamageReact` (`320462a0`) sets HP to 0
  after the hit's damage. Monsters are unaffected (June has no NPC reader). The load line now says `... ported`.
  **Revert:** undefine it and rebuild all projects (the debuff becomes a plain slow again).

### June buff port, Phase 14c: the tutorial flag (CX2Main +0x16a)

- **`SERV_IRUHADEV_JUNE_TUTORIAL_MODE`.** June's `CX2Main +0x16a` is March's `m_bIsPlayingTutorial`. This ports June's reads of it
  that March lacks (classification: `june2017_re/c14c_tutorial_reads.tsv`): in the tutorial HP stops at 1, super armor does not
  stop a launch, there is no start fanfare (and outside it the fanfare plays once per unit, `m_bStarted`), no riding key, no
  shortcut keys (March's K-key combo-tree exception is gone), the quick slot / quick quest / menu bar are hidden, the quick quest
  cannot be hidden with TAB, stage changes show the loading dialogs, ESC no longer ends a slide show, no pets, no play guide or
  Go arrow, disabled quick slots are hidden. The first `SetIsPlayingTutorial( true )` swaps in the default keys locally and the
  tutorial's `CX2DungeonGame::Release` puts the player's keys back. Character select starts the tutorial only on the server's
  first-select bool. **Revert:** undefine it and rebuild all projects.
- **`SERV_IRUHADEV_OFFLINE_FIRST_SELECT`.** The emulator sends `m_bFirstSelect` as live does (this character's select count == 0),
  counted in `els_db.sql` `settings` rows `select_count.<unit uid>` (no schema change). **Revert:** undefine it (the bool goes
  back to false, so with the flag above still on the tutorial never auto-starts); the rows can be deleted with
  `DELETE FROM settings WHERE name LIKE 'select_count.%';`.

### June effect completion C4: damage math (range rate, Nasod combo gate, DIVISION window)

- **`SERV_IRUHADEV_JUNE_DAMAGE_MATH`.** Two changes, both dormant on today's data (no March or June script sets the keys they read
  except the Nasod one):
  - *Formula (June `32622780`, from `GetFinalDamage` `@322857f6`).* The two damage locals (physic, magic) are each multiplied by
    `DIVIDE_DAMAGE_FROM_RANGE`'s rate when it is below 1.0: rate = the `DAMAGE_RATE` of the last `RANGE%d` entry whose `RANGE` is not
    above the distance from the effect to the defender's aim point (1.0 before any). It runs before the class damage hook, on the
    locals only. Nothing changes without the key. Parse: `DIVIDE_DAMAGE_FROM_RANGE = { RANGE0 = { RANGE, DAMAGE_RATE }, ... }`.
  - *Gate (June `329f63a0`).* `CX2GUAdd::SetNasodArmorAttackTarget` sets Add's Nasod armor target only for a hit whose DamageData has
    `CAN_ACTIVATE_NASOD_ARMOR_COMBO = TRUE`, or whose skill is `SI_A_AN_PULSE_BULLET` / `SI_A_AN_PHASE_SHIFT`. March's data has no
    such line, so **Add's other hits no longer set that target** until June's DamageEffect data is adopted (user, 2026-10-07).
  **Revert:** undefine it and rebuild all projects (the target is set on every hit again; no range scaling).
- **`SERV_IRUHADEV_JUNE_BUFF_REACT_APPLY` (extended).** `CheckDamageDataKeyName` became `JuneCheckDivisionWindow`: with
  `DIVISION_NUM >= 1` a unit refuses a second buff-factor application from the same (attacker, `DIVISION_NUM`) for `DIVISION_OFF_TIME`
  seconds (window map ticked at the end of `CX2GameUnit::OnFrameMove`, re-armed at the end of `SetDamageDataFromLUA`). Dormant: no
  data sets either key. Reverting it means undefining the whole flag.
- No ignore-defence number changes: those keys are June buff port Phase 5c (`SERV_IRUHADEV_JUNE_BUFF_NEW_STAT`).
- **C4 review (2026-10-07):** `DamageData::SimpleInit` no longer resets `CAN_ACTIVATE_NASOD_ARMOR_COMBO` or clears the
  range map, as June (`31f8ee50` / `31f8f550`): a re-used DamageData keeps earlier `RANGE`s (JUNE_BUGS 91). The ctor sets the
  bool false. Dormant on today's data; reverting is the same flag.

### June effect completion C10b-1: HP-rate damage

- **`SERV_IRUHADEV_JUNE_HP_RATE_DAMAGE`.** A DamageEffect hit with `HP_RATE_DAMAGE_TYPE` 1 / 2 replaces the damage with the defender's
  now HP / max HP times `HP_RATE_DAMAGE_VALUE` (June `32273e80`), while its now HP is above 1.0. It runs on the defender in
  `GetFinalDamage` and again in a user defender's `DamageReact`, as June (JUNE_BUGS 93). March data sets neither key, so nothing
  changes in play; the C10b-1 play-test used a temporary edit of `ARROW_OF_EXPLOSION` in data036 (reverted after the test).
  **Revert:** undefine it and rebuild all projects.

### June effect completion C4c: NPC Fury mode and June's NPC anim speed

- **`SERV_IRUHADEV_JUNE_NPC_FURY_MODE`.** June's raid-boss Fury mode. An NPC state may set `FURY_MODE = TRUE` and a
  `FURY_DATA = { ATTACK_POWER_RATE_FURY, ANIM_SPEED_RATE_FURY }` table (each 1.0 when absent); NPC Lua turns the NPC's Fury byte on
  and off with `SetFuryMode( bool )` / `GetFuryMode()`. While the byte is on and the current state has `FURY_MODE`, the NPC's
  nonzero damage locals are multiplied by the attack rate in `GetFinalDamage` (before the range rate), and the anim rate
  **replaces** the state's speed in `ResetAnimSpeed` (that half also needs the next flag). No March script uses any of it; the C4c
  play-test staged Mars (`Thief_Thin_Zero.lua`, data037; original in `kom_contents_backup_june_fx_c4c`). JUNE_BUGS 94-98.
  **Revert:** undefine it and rebuild all projects (the keys are then ignored and the two Lua calls are unbound).
- **`SERV_IRUHADEV_JUNE_NPC_ANIM_SPEED`.** `CX2GUNPC::ResetAnimSpeed` is June's: no stat update at entry; it multiplies in two
  June factors that stay 1.0 here (Rose summons, checklist 7f; Hero dungeons, phase C4d), and it is now a `CX2GameUnit` virtual
  that `CheckAndUpdateAdditionalStat` calls whenever a buff changes the anim stat (empty for users), so NPC slows and hastes apply
  at once instead of at the next state change. **Revert:** undefine it and rebuild all projects.

### June effect completion C4d: Hero-dungeon NPC anim speed

- **`SERV_IRUHADEV_JUNE_HERO_NPC_ANIM_SPEED`.** June's Hero-dungeon row list (`Add_HeroDungeon_ExtraStatForException`: dungeon ID,
  NPC ID, anim-speed rate) on `g_pDungeonManager`, and June's `CreateGUNPC` tail that gives a matching NPC that rate in a Hero
  dungeon (last match wins, JUNE_BUGS 99). Dormant: March loads no `HeroDungeonData_Client.lua` and has no Hero dungeon, so the
  list stays empty and nothing changes in play. One `JUNE_FX HERO_ANIM` line per run in ErrorLog.txt says so.
  **Revert:** undefine it and rebuild all projects.

### June effect completion C3c: super-armour condition, DamageReactStateChange's gates, ADD_MEMBER_COUNT

- **`SERV_IRUHADEV_JUNE_SUPER_ARMOR_CONDITION`.** A state may set `SUPER_ARMOR_FRONT = TRUE` and a `SUPER_ARMOR_CONDITION = { ... }` list
  (`SAC_FRONT` 0 only from the front, `SAC_NORMAL_ATTACK` 1 not against special attacks, `SAC_SPECIAL_ATTACK` 2 only against them);
  `DamageReactStateChange` asks `JuneIsSuperArmorAgainst` at June's 22 sites and `FORCE_BACK_SPEED` at its 6 tests, so a hit that
  super armour would stop can still give the unit the hit's vertical knock speed. No March script sets either key, so all of it is
  inert **except one change that is live**: a knock-down (RT_DOWN) no longer empties the force-down gage, because June's case has no
  `SetNowForceDown( 0 )` (March's `FIX_FORCE_DOWN_INITIALIZE_BUG` added one). The gage now fills across knock-downs until it overflows
  into the invincible fall. JUNE_BUGS 131. **Revert:** undefine it and rebuild all projects.
- **`SERV_IRUHADEV_JUNE_DAMAGEDATA` (C3c part).** `ADD_MEMBER_COUNT` / `_DISTANCE` now have their reader: an EDT_TRANSIT_DEBUFF hit also
  runs the attacker's debuff transit on each team-mate around the attacker (PvP user defenders only in June's data). The per-factor
  packet buff apply gains June's two extra steps (a per-class refusal hook that is always false in March, and the finalizers learning
  their buff templet id). JUNE_BUGS 132-133. **Revert:** undefine `SERV_IRUHADEV_JUNE_DAMAGEDATA` (this also removes C3's keys).

### June effect completion C9f-2: per-equip effect sets (DITT_CONDITION_EQUIP_PLAY_EFFECT_SET)

- **`SERV_IRUHADEV_JUNE_EQUIP_EFFECT_SET`.** June's division-item-templet loader (`g_pItemManager:AddDivisionItemTemplet_LUA`, its
  dispatcher and only the CEPE parser; the rest of the system is backlog Stage 11), loaded from `RenderDivisionItemTemplet.lua`
  in `ResetItemManager`, and the per-equip effect set it drives: an equip whose displayed item id has a record plays its set on
  the UI effect set from GUUser `CommonFrameMove`, `CX2SquareUnit::OnFrameMove` and the unit viewer (group 1), with June's
  WAIT / NOT_MOVE / ALWAYS conditions, start delay and random switch set; `RemoveUnit` stops it. **Reachable** (was inert
  until june-item-kim K2, 2026-10-08, put June's `item.kim` live: all 5 item ids are in it). Data (data036): `Enum.lua` + June's `DIVISION_ITEM_TEMPLET_TYPE`, `READ_TYPE`,
  `CONDITION_EQUIP_PLAY_EFFECT_SET` (appended verbatim, as Phase 11 did); `RenderDivisionItemTemplet.lua` (June verbatim,
  manifest entry added); `UIEffectSet.lua` + 6 sets; `UIMajorXMeshPlayer.txt` + 2 mesh templets. One `JUNE_FX EQUIP_FX_LOAD`
  line in ErrorLog.txt. JUNE_BUGS 100-102. **Revert:** undefine it and rebuild all projects. The data can stay: with the flag off
  nothing loads `RenderDivisionItemTemplet.lua`, and the extra enum tables and sets are unused.

### June effect completion C9h-2a: June's pet / riding-pet viewer effect sets (viewer and engine half)

- **`SERV_IRUHADEV_JUNE_PET_VIEWER_FX`** (needs `SERV_IRUHADEV_JUNE_UNIT_VIEWER_EFFECT_PLAYER` and `SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME`, `#error`
  guards). `CX2NPCUnitViewerUI` gets June's handle vector (`+0x2a8`), the three base-viewer hooks (`OnFrameMove` / `Prepare` / `Draw` now call
  `CX2BaseUnitViewerUI`'s, as June's do), `IsAnimExist`, the handle stop (stops a copy of each handle, never erases), a `SetScale` that also
  scales the live handles, `ResetPet( id, lv, cVariant = 0 )` (variant branch, group-1 stop, `UNIT_VIEWER_EFFECT_SET_LIST` into group 1) and
  `ResetRidingPet` (viewer list into group 1 with no stop, `RIDINGPET_EFFECT_SET_LIST_BY_UIEFFECTSET` through the new
  `CX2EffectSet::PlayEffectSet( name, skin )`). Engine: the instance's viewer flag (June `+0xdd`) and its three consumers (screen-space bone
  position in `GetEffectPosition` and `GetEffectPositionByBoneName`, over-UI / 2D-plane / resolution-converted particles). `CX2PetManager` gets
  the variant XET suffix helper, `CKTDGXSkinAnim` a find-based `JuneIsAnimSetExist` (March's `GetAnimSet( name )` inserts on a miss).
- **Inert in March data**: none of June's 8 pets / riding pets with these keys is in March, and no March caller passes a variant. The only visible
  change is the base viewer's per-frame player now also running for pet / riding-pet previews. Log: `JUNE_FX PET_VIEWER` in ErrorLog.txt,
  written only when something played.
- **Revert.** Undefine the flag in `KTDXLIB/Always.h` and rebuild all projects. No data changed.

### June effect completion C9g-2: June's PlayByMotionType and AnimationNameTable.lua

- **`SERV_IRUHADEV_JUNE_ANIMATION_NAME_TABLE`.** June replaced March's per-class lobby motion switch in `CX2UnitViewerUI::PlayByMotionType`
  (`X2UnitViewerUI.cpp`, 1,090 lines, kept in the `#else`) with a lookup in `AnimationNameTable.lua` (384 rows): `g_pUnitManager:AddAnimationName`,
  `CX2UnitManager::GetAnimationName`, loaded in `ResetUnitManager`. `UNIT_VIEWER_UI_MOTION_TYPE` is renumbered to June's (HABIT 6, SIT 7-9,
  WAIT_DELETE 6 -> 10); `PlayByMotionType` takes `bool bTransition = true` and runs June's head first (`m_ePlayedEmotion` reset, viewer effect
  group stops 0..4, cash shop 0..2). `CX2SquareUnit::InitWaitMotion` takes its WAIT name from the table. WAIT1/WAIT2 now loop; the table drives the
  Little Devil orb (March's keyed `ALD_lobbyWait` hard-code never fires again) and the Pyro Knight ready-action fire. Log: `JUNE_FX ANIM_NAME_LOAD`
  (`rows=384 inserted=230 skipped_duplicate=154 class_none=160`: 31 June classes are missing from March's Lua `UNIT_CLASS` and load as class 0).
- **Data.** `data036`: `Enum.lua` + `UNIT_VIEWER_UI_MOTION_TYPE`, `AnimationNameTable.lua` (+ manifest entry). `data033`: June's `Motion_EL_Lobby.x`,
  `Motion_ARA_Lobby.x` (1 byte repaired: appended `1`, user's choice), `Motion_ARA_Common.x`, `Motion_EVE_Common.x` replace March's (June renamed 7
  motions: `PR_LobbyDownLanding`, `ALD_LobbyWait`, `AYR_LobbyReadyAction`, `SSK_/SPK_LobbyReadyAction`).
- **Revert.** Undefine the flag and rebuild all projects; and **restore March's four motion files** (backups `%TEMP%/c9g2_backup033`, or the
  kom backup `kom_backup_tools/data033.kom.20261008_042838`): with the flag off March's code asks for `PR_LobbyDownlanding`, `ALD_lobbyWait`,
  `AYR_LobbyReadyAtcion`, `SSK_/SPK_LobbyReadyaction`, which June's files no longer carry. The `data036` additions can stay (unused).

### June effect completion C7: touch-land runtime and three spawner gaps

- **`SERV_IRUHADEV_JUNE_TOUCH_LAND`** (needs `SERV_IRUHADEV_JUNE_SPAWN_INHERIT`). `CXMeshInstance` gets June's previous-frame
  velocity (`m_vVelocityBefore`, snapshotted next to `m_fAniTimeBefore` every frame). `CEffect::OnFrameMove` runs June's bounce gate
  right after the die check: gravity down, falling last frame, rising now. While it holds, June's touch-land spawner (`31f81ba0`)
  spawns every `TOUCH_LAND_DAMAGE_EFFECT%d` entry at `m_BeforePos` and runs the C6b tail. No March DamageEffect has the table, so
  nothing spawns. The gate itself fires on any bouncing effect and logs `JUNE_FX CE_HIT <effect> TOUCH_LAND_GATE entries=0`.
- **`SERV_IRUHADEV_JUNE_SPAWN_INHERIT`, extended.** A new base virtual `CX2GameUnit::JuneOnDamageEffectCreated` (June vslot 0x328, a
  no-op) is called on the owner from every CEffect ctor. RT_CREATE_DAMAGE_EFFECT now also runs `31f6ea20`, which scales the child's
  damage, HIT_ADD_MP and TECH_POINT by `m_fJuneCreateDamageEffectRate`. That rate stays -1 unless class code sets it, so nothing
  changes in March.
- **`SERV_IRUHADEV_JUNE_DAMAGEEFFECT_RUNTIME`, extended.**
  - The CREATE_DAMAGE_EFFECT loop now applies INHERIT_CRITICAL_RATE / _DAMAGE_RATE, as the hit and die spawners do.
  - DIE entries' INHERITLOCKONDATA now copies the parent's LockOnData to the child. March used to parse the key and ignore it.
  - Both lock-on copies use June's assignment (`JuneAssignLockOnData`), which leaves the child's pre-lock-on hit set alone. **Live
    in March:** a CREATE child with lock-on no longer inherits the parent's list of units already hit. The in-range target picks
    (June `322d7670` / `322daa10`) skip the units in that list.
  - March data uses neither INHERIT_CRITICAL nor INHERITLOCKONDATA.

| | before | after |
|---|---|---|
| CREATE child's pre-lock-on hit set | the parent's | the child's own (empty at birth) |
| `offline_server.log` | - | `JUNE_FX CE_HIT ... TOUCH_LAND_GATE / TOUCH_LAND / CREATE_DAMAGE_EFFECT_RATE / CREATE_DAMAGE_EFFECT_INHERIT / INHERITLOCKONDATA / CREATE_LOCKON_COPY` |

Revert: undefine `SERV_IRUHADEV_JUNE_TOUCH_LAND` (KTDXLIB too: the mesh member) and rebuild all four projects. The other two
items go with their own flags. JUNE_BUGS 103 records the TECH_POINT truncation.

### June effect completion C6c: the effect-set buff list

Two flags. **`SERV_IRUHADEV_JUNE_EFFECTSET_BUFF_LIST`** (needs `SERV_IRUHADEV_JUNE_EFFECTSET_RUNTIME` and `SERV_IRUHADEV_JUNE_ENCHANT_PARITY`) ports
June's effect-set buff list: `EffectSetInstance::m_vecBuffFactorList` (June `+0x160`), `CX2EffectSet::AttachBuffFactorToEffectSet` (`321e3650`),
and `ApplyBuffFactorListToDamageEffect` (`321ee480`), which `OnFrameMove_CreateEffect` calls for each damage effect a set creates, right after
C6b's stamps. Per entry: the buff factor by id, a clone at the entry's level, the entry's modifiers (`32d418a0` = the existing
`JuneSetBehaviorValue`; `32d41960` -> `32d40dd0` = the new `CX2BuffFactor::JuneSetFinalizerValue`), then
`DamageData::PushBuffFactor( clone, true )`. The clear flag is inside the loop, so a non-empty list replaces the effect's own buff factors
and only the last entry survives (JUNE_BUGS 59, verbatim). SIEE 15 (`SIEE_ALWAYS_PLAY_EFFECT_SET`) now attaches every flagged
`m_vecConditionalBuff` entry to the set it plays instead of logging "unsupported". **`SERV_IRUHADEV_JUNE_FIX_EFFECTSET_BUFF_NULL`** is the
JUNE_BUGS 60 fix (user, 2026-10-07): an entry whose BFI the buff templet manager does not know is skipped and logged once per BFI
(`JUNE_FX BUFF_LIST skipped BFI n` in `ErrorLog.txt`); off, June's unguarded line (a NULL read) is back.

In March nothing reaches this yet: June's aura set `EffectSet_Hero_Dungeon_Aura_Buff` is not in March's data and no save carries a
Fever socket (SIEE 13-20), so no instance gets a list and every consumer call returns at the empty-list test. Modifiers are filled only
by Rose / Add class code (checklist 7a), so they run inert too.

| | before | after |
|---|---|---|
| SIEE 15 with a flagged buff | `unsupported (effect-set buff list, 321e3650)` log, nothing attached | attached to the set; its damage effects carry the buff |
| effect set with an empty list | - | unchanged (the consumer returns first) |
| `offline_server.log` (`JUNE_FX_DEBUG`) | - | `JUNE_FX ES_CREATE ... BUFF_LIST_ATTACH / BUFF_LIST_APPLY` |

Revert: undefine the two flags in `KTDXLIB/Always.h` (the fix first, it needs the list) and rebuild all four projects (they sit in the PCH).
June's per-class attach variants `321e3730` / `321e3830` and the class callers are in the character-port checklist 7a.

### June item.kim adoption K2: June's 228-byte item record and June's item.kim

- **`SERV_IRUHADEV_JUNE_ITEM_KIM` and June's `item.kim` go together.** The flag compiles June's `KItemFormatTemplet` (228 B, K0 map
  `june_item_kim/k0_record_map.tsv`), `MAX_MODEL_COUNT_A_ITEM` 8, the 7-float stat pool and the 6-byte stat-level pool, flags bit 7 =
  PvP-equip (HideSetDesc moved to flags2 bit 1), June's Extra key remap, `IT_CUSTOM` = 14 (sorts as `ST_SPECIAL`, as June), the grade-6
  name colour, tooltip label (STR 34930 "Legend") and drop colour, a load census and the PvP-emblem NULL guard (June has none; a March
  gap). `data036`'s `item.kim` is June's file (67,557,756 B, 111,428 items, 6,740 sets), manifest value unchanged (`"2"`).
- **`SERV_IRUHADEV_KIM_LAYOUT_CHECK`** (independent, stays on): `DoMemoryNotEncrypt` reads the file's record stride from the data and
  refuses a file that does not match the compiled struct, with a `[JUNE_KIM] layout mismatch` line in ErrorLog.txt. The refusal falls to
  the Lua path, which has no `Item.lua`, so the client then crashes at the first item lookup: the line names the cause. **An exe built
  without both flags (e.g. a branch whose `Always.h` predates K2) reads June's kim at 188 B per record silently. Keep branches in step.**
- **`SERV_IRUHADEV_JUNE_KIM_MINIME_REMAP`** (decision D2, a stand-in, not June): eqip 26 -> 17 and 27 -> 18 so the Mini-me items and
  earrings keep equipping. Backlog Stage 16 ports June's two slots and removes it.
- **Assets (decision D1).** 501 June files that June items name and March lacked, staged into the same-numbered `kom_contents/dataNNN_kom/`
  (manifest `"0"`): 19 existing koms gain files, 6 are new (`data158`, `data159`, `data185`, `data191`, `data192`, `data211`). List and
  short-copy bytes: `june_item_kim/k2_assets_staged.tsv`; 79 names exist in no June kom and stay missing.
- **Revert.** Undefine the three flags in `KTDXLIB/Always.h`, rebuild all four projects, and put March's kim back in
  `kom_contents/data036_kom/item.kim`: `march_data/data036/item.kim.march_patched` (the buff Phase 13 patched copy), then repack and copy
  data036. The staged assets can stay (nothing March names reads them). Pre-K2 koms: `kom_contents/../kom_backup_tools/`.

### June effect completion C3: the straightforward DamageData keys

Flag: `SERV_IRUHADEV_JUNE_DAMAGEDATA` (extended, no new flag). All 19 keys are parsed into new `DamageData` / `ExtraDamageData`
members with June's defaults (`SimpleInit` / `Init`), and each consumer June has is ported at its March site.

| key | June parse | March consumer (June consumer) |
|---|---|---|
| `ATTACK_ONLY_ME` | `+0x50d` | the six hit paths (`X2DamageManager.cpp`: Ground / Area / UnitToUnit: UID == the caller's `attackUnitUID`; Mesh / CollisionData / EffectToUnit: attacker (the effect's owner) UID == defender UID), and both `DamageReact`s' invincible gate |
| `ONYL_ATTACK_USER` | `+0x5fc` | UnitToUnit, MeshToUnit, CollisionDataToUnit, EffectToUnit: non-user defenders are skipped |
| `ADD_CHECK_DAMAGE_UNIT_NUM` | `+0x4d8` | `AreaHit`: result count = count + the key when the loop stopped on `DAMAGE_UNIT_NUM`; feeds the MP drain and the return value |
| `FORCE_BACK_SPEED` | `+0x3ec` | user and NPC `DamageReact`: the back-speed block also runs for a super-armored / dummy (NPC: guarding, frozen) hit |
| `DAMAGE_LIGHT_TIME` | `+0x5ec` | user `DamageReact` (DT_BUFF clears the timer), NPC `DamageReact`, user `AttackResult` (own `m_DamageData`) |
| `EXCEPT_ME_HP_UP` | `+0x50c` | NPC `DamageReact`: no HP heal and no heal effect for a hit from the same NPC (low UID dword); HP heal also needs `GetNowHp() > 0` |
| `DAMAGE_RATE_TO_SUPERAMOR` | `+0x620` | `CX2GameUnit::GetFinalDamage`: x the key when the defender is super armored |
| `PAUSE_NPC_AI_TIME` | `+0x624` | `GetFinalDamage`: `SetTimePauseNPCAI` on an NPC defender when > 0 |
| `HIT_ADD_MP_PERCENT` | `+0x410` | `CEffect::AttackResult` and `CX2GUUser::AttackResult`: new `CX2GameUnit::JuneUpNowMpPerHitOthersPercent` ("MP +%d" in the blue pic char) |
| `IGNORE_ATTACK_SUCCESS` | `+0x628` | `CEffect::AttackResult`: an effect hit sets a user owner's future-frame `unitCondition.bHit` unless the key is set (new `CX2GUUser::JuneSetAttackSuccess`) |
| `CREATE_DAMAGE_EFFECT_NAME_SCALE_X/Y/Z` | `+0x498` | `SetScale` on the RT_CREATE_DAMAGE_EFFECT child (CEffect and user `AttackResult`) and on the random-spawn child |
| `INIT_EXTRA_DAMAGE_BEFORE_LOAD` | local | `ExtraDamageData::Init` (+ release of `m_ptrBuffFactor`) before the EXTRA_DAMAGE table is read |
| `EDT_MANA_DAMAGE_WITH_NO_DISPLAY` | (8a) | both `Process_ExtraDamage`s: the "MP -" text is skipped for the no-display flag |

**Parsed and stored, no behaviour (rule 5 log once per key per templet):** `ADD_MEMBER_COUNT` / `_DISTANCE` (no June reader found),
`BOUNDING_BACK_SPEED_X/Y` (their reader is `DamageReactStateChange`'s bounding case behind the unidentified armour-level check
`31fd86b0`), `APPLIED_MEMO_ID` (its reader is the skill-memo switch, C8b). `FORCE_BACK_SPEED` has the same `DamageReactStateChange`
gate as a second consumer (6 sites, `31fd86b0`); only its `DamageReact` half is ported.

**Live in March today** (the data does not set the new keys, but these paths now differ): an invincible unit takes a DT_BUFF hit
that is `ATTACK_OUR_TEAM`; an NPC no longer heals a dead NPC; a user-owned effect hit marks the owner's future-frame `bHit` (June's
`31fcee10`).

Revert: undefine `SERV_IRUHADEV_JUNE_DAMAGEDATA` and rebuild all four projects (it also turns off the earlier phases that flag owns).

### C3d-2: 16-bit user state id and action id (2026-10-08)

`SERV_IRUHADEV_JUNE_USER_STATE_ID16` widens the user state id from a byte to 16 bits, as June does (sites: `june2017_re/c3d1_state_id_sites.tsv`),
and reserves June's `USI_BOUNDING_DOWN` / `_LANDCRASH` / `_UP` just before `USI_END`: **every class state id shifts by 3** (`USI_END` 88 -> 91,
Chung's last 255 -> 258, `ASI_DASH_COMBO_X` 121 -> 124). `SERV_IRUHADEV_JUNE_USER_ACTION_ID16` widens `SyncData::nowAction` and its packet field.

**The exe and data036 must deploy together.** `Enum.lua`'s `CHUNG_STATE_ID` / `ARA_STATE_ID` are `USI_END`-relative and are read by
`Chung_Iron_Cannon.lua` (guard-state list), `Ara_Martial_Artist.lua` (chain system) and data037 `Npc_Chung_01..05`. A flag-on exe with the old
data036, or the old exe with the new data036, leaves Chung's guard list and Ara's skill-chain ids off by 3. data036's `Enum.src.lua` gained the three
rows at `GUSI_END + 83..85` and `USI_END = GUSI_END + 86`.

Revert: undefine both flags, restore `Enum.src.lua`'s `USI_END = GUSI_END + 83` (drop the three rows), `kom.py compile` + `pack` data036, rebuild all
four projects and deploy exe and kom together.

### C3d-3: June's bounding back-speed state (2026-10-09)

`SERV_IRUHADEV_JUNE_BOUNDING_STATE` ports the engine side of June's RT_BOUNDING reaction (`BOUNDING_BACK_SPEED_X/Y`). **No class registers the three states yet** (June carries their tables inline in each class script; March's are bytecode only: phase C3d-4),
so an RT_BOUNDING hit is ignored and logged once. No data file and no kom change: data036 / data037 pack identical to the live archives.

Revert: undefine the flag and rebuild all four projects.

### June NPC and dungeon Lua via ljconv (2026-10-09)

Data only, no flag, no exe change. Every June NPC / monster script (data037, datai016) and dungeon stage script (data035 `Dungeon_*`,
datai030) ships as June's own bytecode, transcoded to March's opcode numbering by `june2017_re/ljconv.py` (no decompile, no recompile):
3,518 files, 2,145 replacing March or earlier-port files, 1,373 June-only. Per-file list: `june2017_re/june_npc_lua_port_20261009.tsv`.
Packed and copied: `data035` md5 b76d3cb3, `data037` 42e6ccc6, `datai016` 86d9291a, new `datai030` 21dd2fdb.

- **Kept March's:** the registries `NPCTemplet.lua` / `NPCTemplet_1.lua` / `NPCTemplet_LA.lua` and `DungeonData.lua`, pets and riding pets,
  datai016 `UI_*`, data035 `El_Tree_Main_Test.lua`, and every table / map (data036, data040). June-only scripts are therefore
  unreachable until a registry names them.
- **Lanox ports replaced, edits included (user, 2026-10-09):** the 252 Lanox `.src.lua` ports in data035/037/040 had 45 that differ from
  June on purpose (e.g. `RAGE_DRYADKENTA` softlock fix). All in this scope are now June's bytecode; their sidecars moved to the backup below.
  **`march2014/update/data035` and `data037` `.src.lua` are superseded**: do not `kom.py compile` them back over June's files.
- Origin of a regression: a June script stopping at a name March lacks logs `[LUA_SRC] lua error while loading X.lua` in `ErrorLog.txt`.

Revert: copy back the live koms from `24965799\kom_backup_tools\{data035,data037,datai016}.kom.20261009_135512*` and delete `data\datai030.kom`;
for the staging tree, restore `24965799\kom_backup_stage_june\20261009_135231\` (replaced files + moved sidecars) over `kom_contents\`.

### June meshes for the replaced NPC / dungeon scripts (2026-10-09)

Data only, no flag, no exe change. The ljconv scripts above that **replaced** a March script name June-renamed meshes March never
had (e.g. `Thief_Thin_Toto.x` -> `Motion_THIEF_THIN_TOTO.X`). The monster's motion then fails to open and `CX2GUNPC::Init` dereferences
the NULL `m_pMotion` (`+0x775c`): the client crashed entering Ruben field 40000 (Toto). User chose to bring June's files in (option B).
`june2017_re/npc_mesh_census.py` scans the 2,267 replaced `.lua` (stage backups `20261009_135231` and `_141115`) for asset names June has
and March lacks, then follows each mesh to its textures: **143 files, 178 MB** (97 `.x`, 31 `.dds`, 15 `.tga`), list
`june2017_re/npc_mesh_need_replaced.tsv`. Staged with `kom.py stage-june` (June's exact bytes, June's kom number, all 13 exist in March):
data005 +6, data006 +4, data007 +19, data008 +15, data009 +12, data043 +31, data057 +4, data100 +16, data111 +22, data119 +1, data126 +6,
data136 +4, data151 +3. Added only: no March file replaced. (`kom.py status` also lists hundreds of `~changed` entries in these koms:
compressed size only, raw size equal: elstools compresses differently from KOG's packer.) June-only scripts (1,373 new files) name
~1,700 more missing assets; they are unreachable until a registry names them, and stay with june-asset-port P2-P4.

Revert: copy back `24965799\kom_backup_tools\<kom>.kom.<stamp>` for the 13 koms, and delete the 143 names (TSV) from
`kom_contents\dataNNN_kom\` and their `_manifest.json` keys.

### June partial ports replaced (2026-10-09)

Data only, no flag. Every earlier June port that was still a recompiled decompile or a March/June merge is now June's own bytecode via
`ljconv`: 120 files in data034/036/037/039/040/217 (105 kom entries changed; the 121st, datai014's Cobo dialog, was reverted; data036 `BattleFieldData.lua` was reverted too, see below). Per-file list with kind and probe:
`june2017_re/june_partial_port_replace_20261009.tsv`. Kinds: 88 were June's content already (decompile drift removed), 9 dropped small
Lanox March-compat edits, 24 were March files with June parts (now June whole). 110 `.src.lua` sidecars moved to the backup below;
the matching `march2014/` sources are superseded. L0's six data217 ledger rows are marked `restored` (superseded).

**Kept merged on purpose (user, 2026-10-09), with their blockers** (measured by loading June's file over March's enums):
- `Enum.lua`: 454 names differ in value from March's C++ (user / Chung / Raven / Ara state ids, UI message ids), 164 March names gone. Needs the C++ on June's numbering.
- `DungeonEnum.lua`: drops 59 March dungeon names (the 9 battle-field tutorials, Besma Mine/Canyon, secret dungeons).
- `DamageEffect.lua` + `EffectSet.lua`: D1, 1,915 blocks take June's numerical skill data (character ports) and deal 0.
- `DLG_Map_Village.lua`, `DLG_Map_Local.lua`, `DLG_Map_House.lua`: stop loading at the first Elysion / June house id ("table index is nil"); clean once Atlas/Elysion adds the ids.
- `NPCTemplet.lua`, `DungeonData.lua`: registries (also kept by the NPC port above).
- `DLG_Destination_byButton.lua` (datai014, reverted the same day after the play-test): June's drops `Village_1000` / `Village_1002`, and March's `CX2StateMenu::InitWarpList` dereferences them unchecked, so Cobo Express (B) crashed. Comes back with June's `InitWarpList` / `CreateWarpDest` in Atlas A5 step 5.
- `BattleFieldData.lua` (data036, reverted 2026-10-09 after the Ara/Ruben play-test): June's uses `BFT_BOSS_RAID` / `BFT_EVENT` / `BFT_NEW_BOSS_RAID`, which March's `CX2BattleFieldManager` never registers; lua_tinker aborts the script (`can't find 'BFT_BOSS_RAID' class variable`), `OpenScriptFile` fails (ErrorEnum 10002) and the client has no battlefield data. March's merged copy (29,521 B, shipped 2026-10-03) and its `.src.lua` are back in `kom_contents`, core-list row kept-merged; June's copy saved to `kom_backup_stage_june\20261009_battlefielddata_june\`. Returns with June's battlefield types (atlas A6b).

The 20 "uncertain" ones went in (user's call): `WorldData`, `MapData`, `BattleFieldData`, `PVP_Map_List` and 16 map dialogs load, but name
Elysion / Atlas / hero-dungeon ids March lacks (nil to the C++). If a map or world misbehaves, check that file's probe column first.

Revert: live koms from `24965799\kom_backup_tools\*.20261009_1412*`; staging from `24965799\kom_backup_stage_june\20261009_141115\`; set the
six data217 ledger rows back to `live`.

**Rolled back 2026-10-09 (user: "rollback game core lua that got replaced by june lua, not the mobs/dungeon one").** `kom_backup_stage_june\20261009_141115\` restored over `kom_contents\` for data034/036/037/039/040/217 (119 `.lua` + 108 `.src.lua`; BattleFieldData and datai014 were already March's), repacked (104 entries changed). About 88 files only change format back to the recompiled decompile; the 24 merged files get their March parts back and the 9 small-edit files get the Lanox March-compat edits back. The rollback also covers the 14 data037 `RidingPet_*` and the data040 Ranox line maps / `Ranox_*_Stage*.lua`. The data217 L0 ledger rows are `live` again and on the core list (kept-merged: user rollback); for these files the sidecars and `march2014/` sources are current again, not superseded. The NPC/dungeon wave above is untouched. To redo the June wave, re-run the original `stage-june` replace for the TSV's names (per file, the TSV `kind` column), or restore the current koms from `kom_backup_tools\` stamped after this note.

**Two files re-adopted 2026-10-11 (june-effect C3y-1a, user's call):** data034 `DLG_Map_Local_Hamel_Icon_NEW.lua` and `DLG_Map_Local_Hamel.lua` are June's ljconv bytecode again (they bring back the Hamel secret button and its `DungeonIDList` entry). Their March sidecars are in `kom_backup_stage_june\20261011_103645\` and `\20261011_162133\`; to revert, put the sidecar back as `X.src.lua`, `kom.py compile data034`, then pack.

### June maps under the June dungeon scripts (june-effect C3x, 2026-10-10)

Data, plus one log-only flag (`SERV_IRUHADEV_JUNE_WORLD_LOAD_LOG`).
- `Enum.lua` (data036) gains `WI_ELDER_DARK_FOREST_STAGE_0 / _1 / _BOSS_STAGE` (575-577), `WI_ELDER_BENDERS_STAGE_BOSS = 33` and
  `CTT_RIM_LIGHT_BLUE = 10`. World 33 is June's Benders Cave boss stage now; the user chose June parity, and the old name is an alias.
- `WorldData.lua` (data040) is BOM source on the core list, with June's rows for every ID March knows. March keeps the three Ranox
  secret extra rows June lacks.
- 205 June world scripts and line maps replace March's in data040 (`june2017_re/june_adopt_20261010_133232.tsv` and `_134556.tsv`).
- 363 June world meshes and textures are added to data026 / 028 / 029 / 031 / 032 (`june2017_re/world_asset_need.tsv`). data101
  is staged but not packed: its dir is corrupt.

Revert:
- `luasrc.py restore data040 WorldData.lua`.
- Remove the 5 Enum.lua lines tagged `June effect C3x`, then `luasrc.py refresh data036 Enum.lua`.
- Restore the map files and 34 sidecars from `24965799\kom_backup_stage_june\20261010_133232\` and `\20261010_134556\`, and delete the 28 new names and
  the 363 asset names (TSVs) with their manifest keys.
- Repack, or put back the live koms from `kom_backup_tools`.

### June effect Lua re-adopted in full (june-effect C11a, 2026-10-16)

Data only, no flag. The user chose full June re-adoption (route b).
- data036 `DamageEffect.lua`, `EffectSet.lua`, `DamageSound.lua`, `UIEffectSet.lua` and datai015 `DamageEffect_INT.lua`, `EffectSet_INT.lua`,
  `UIEffectSet_INT.lua` are June's own bytecode through `ljconv`, and data036 gains `DamageEffect_Character/_Dungeon/_ETC.lua`
  (`june2017_re/stage_june_fx.py --apply`, plan `june2017_re/stage_plan.tsv`). The two merged BOM sources left the core list
  (`luasrc.py restore`, then June's files over them; core rows `bytecode`, ledger rows `restored`).
- 2,840 June closure assets added (new archive data222) and 86 replaced across 53 koms, all packed and copied (`COPY OK`, data036 md5 31d144bd).
  The packs also carry the 2026-10-12 NPC-effect-closure files that were packed but never copied (`5173c0a5`).
- The merged copies' 87 March-only templets are gone (`june2017_re/c11a_march_only_templets.tsv`; checklist 7u, JUNE_BUGS 200);
  1,913 June blocks deal 0 until the character ports (D1/D7).

Revert:
- Copy everything in `24965799\kom_contents_backup_c11a_20261016\` back into `kom_contents\`: the 93 replaced files (incl. the merged BOM
  `DamageEffect.lua` / `EffectSet.lua`), the 52 manifests, and `UIEffectSet.src.lua.moved` back as `data036_kom\UIEffectSet.src.lua`. Delete
  the 2,843 `add` rows of `june2017_re/stage_plan.tsv` from the dirs, and `data036_kom\{DamageEffect,EffectSet}.src.lua` if present.
  `data222` is a new archive (no manifest to restore): delete `kom_contents\data222_kom\` and the game dir's `data\data222.kom` by hand
  (a repack/copy does not remove it). (C11a review, 2026-10-16.)
  Do not use `stage_june_fx.py --revert`: its backup is the 2026-10-03 one, older than the 2026-10-11/12 merged edits.
- Put the two ledger rows back to `live` and the core rows to `source` (`luasrc.py ledger --check` / `core --check` say what is off), repack, copy.

### June flag index (june-effect C12, 2026-10-16)

Every `SERV_IRUHADEV_JUNE_*` flag that is `#define`d in `KTDXLIB/Always.h` (180) with its Always.h line, date, what it changes and its
dependency guards. Generated from the comment block above each `#define`; the full block (June addresses, JUNE_BUGS ids) is at the listed line.
A flag with a section name in the last column has a longer entry above or below in this file (data revert steps live there).

**Revert, the default:** comment out the `#define` in `KTDXLIB/Always.h`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`.
Flag-off compiles the original code (the `#else` branch). Extra rules:
- A flag listed under *needs* by another flag cannot be turned off alone: the other flag stops at an `#error` guard. Turn the dependent flag off first.
- **Most flags are C++ only.** These are not, and turning them off alone is wrong (read the flag's Always.h block, at the listed line, before touching one):

| flag(s) | revert together with |
|---|---|
| `ITEM_KIM` (+ `KIM_LAYOUT_CHECK`, `KIM_MINIME_REMAP`) | data036's `item.kim`: the exe and June's 228-byte kim ship together. Off with June's kim packed reads it at 188 bytes per record (wrong items, nothing logs); put March's kim back in data036 as well. See "June item.kim adoption K2" |
| `BUFF_ENUM` | data036 `Enum.lua` (the buff Phase 2 file): "ships paired with" it; the BBT / BFT values differ between the two |
| `RESERVED_STATE_IDS`, `USER_STATE_ID16` | data036 `Enum.lua`: USI_END 91 -> 102 and the three bounding ids shift every class state id; the old `Enum.lua` is in the `kom_contents_bytecode` snapshot |
| `ENCHANT_PARITY` | data036 `Enum.lua`: the `EDT_ENCHANT_*` values are renumbered with it |
| `RIDINGPET_UI_FX` | needs data036 `Enum.lua`'s two RESERVED tables when it is on; off is safe |
| `BUFF_FACTOR_WIRE`, `PET_PREVIEW_LOOKUP` | a **second** `#define` at the end of `KncWX2Server/Common/ServerDefine.h` (`CommonPacket.h` / `ClientPacket_Global.h` precede `Always.h`); toggle both, `tools/flagcheck.py` `SECOND_HOMES` |
| `DAMAGE_DATA_TABLE` | data036 `Elsword_DamageData.lua`: flag on without the file logs it and applies nothing; off is safe |
| `LINEMAP_TRIGGER`, `ACTIVE_TRIGGER` | data036 `Enum.lua` carries the same trigger values; `ACTIVE_TRIGGER` also needs `SERV_WORLD_TRIGGER_RELOCATION` (ServerDefine.h) |
| `HITTYPE_EXTEND`, `LOADER_GAPS` | see "June2017 effect engine: enum data + loader-list gaps (Phase 11)" |

- A flag that ships its own data in another kom (a `.txt`, a `.lua`) otherwise leaves that data in place; the old code ignores keys it does not know.
- The flag state in the table is what Always.h `#define`s at top level; it was not re-probed with the preprocessor (CLAUDE.md warns about nested `#ifdef`s in the in-house files, which Always.h does not use).

| flag | line | date | what it changes | needs | section |
|---|---|---|---|---|---|
| `EFFECTSET_PARSE` | 3948 | 2026-09-25 | June2017 effect-engine rebuild, Phase 2 (JUNE2017_EFFECT_REBUILD_PLAN.md). CX2EffectSet::AddEffectSetTemplet_LUA parses the six templet-level keys June2017's parser (FUN_321e48e0) reads and March does not: ... |  | June2017 effect engine: EffectSet templet-level keys (Phase 2) |
| `FX_DEBUG` | 3959 | 2026-09-25 | Short-lived diagnostic shared by every phase of the June2017 effect-engine rebuild (JUNE2017_EFFECT_REBUILD_PLAN.md, plan rule 4). |  | June2017 effect engine: EffectSet templet-level keys (Phase 2) |
| `EFFECTSET_RUNTIME` | 3985 | 2026-09-26 | EffectSet creation-time behaviour transcribed from June's FUN_321ee600 (OnFrameMove_CreateEffect), FUN_321e1e60 (PlayEffectSet) and their helpers. |  | June2017 effect engine: EffectSet creation-time runtime (Phase 4a) |
| `DAMAGEEFFECT_PARSE` | 4008 | 2026-09-26 | June2017 effect-engine rebuild, Phase 5 (JUNE2017_EFFECT_REBUILD_PLAN.md). CX2DamageEffect::CEffect::CEffect parses the CEffect-ctor top-level keys June's FUN_31f7aab0 reads that March does not: USE_ADVANCE_COLLISIOIN_CHECK, ... |  | June2017 effect engine: DamageEffect CEffect ctor keys (Phase 5) |
| `DAMAGEEFFECT_RUNTIME` | 4081 | 2026-09-27 | DamageEffect per-frame behaviour transcribed from June's CEffect::OnFrameMove (FUN_31f88790, the one CEffect virtual) and FollowLine (FUN_31f70ee0): TRACE_OWNER_BONE, USE_OWNER_ATTACK_SPEED (with USE_GLOBAL_TIME's window), ... | `JUNE_DAMAGEEFFECT_PARSE` | June2017 effect engine: DamageEffect per-frame runtime (Phase 7a) |
| `DAMAGEDATA` | 4105 | 2026-09-29 | the DAMAGE_DATA keys March already parses, walked against June's SetDamageDataFromLUA (FUN_32632390) and ParsingBuffFactorID (FUN_32630d20). |  | June2017 effect engine: DamageData, existing keys (Phase 8a) |
| `COLLISION` | 4123 | 2026-09-29 | the damage-effect -> unit hit test. June's EffectToUnit (FUN_3262b6e0) inlines CollisionDataCheck with three changes against March's KTDXCollision.h: sphere x line and line x sphere use the closest point on the segment (clamped to its ... | `JUNE_DAMAGEEFFECT_PARSE` | June2017 effect engine: the damage-effect hit test (Phase 9) |
| `MESHPLAYER` | 4145 | 2026-09-29 | the four XMeshPlayer .txt templet keys June's parser reads and March's does not (ProcessTempletBlock FUN_31c8ae10, tokenizer FUN_31c85840). | `RANOX_FORCE_NOT_CULL` | June2017 effect engine: XMeshPlayer `.txt` templet keys (Phase 10) |
| `PARTICLE` | 4165 | 2026-09-29 | June2017 effect rebuild, Phase 10 follow-up: the particle .txt templet keys June's particle parser reads and March's does not (tokenizer FUN_31cf2250, ProcessEventSequenceBlock FUN_31cfa4a0). |  | June2017 effect engine: particle `.txt` templet keys (Phase 10 follow-up) |
| `DIECREATENPC_DTOR_GUARD` | 4176 | 2026-09-29 | fix R6025 pure virtual function call crash (X2_offline.exe.41620.dmp), hit in Burned Forest (dungeon 33270) when a dungeon sub-stage transition force-deletes an NPC that still has a live effect registered via ... |  |  |
| `LOADER_GAPS` | 4185 | 2026-09-30 | June2017 effect rebuild Phase 11 -- June effect-set data uses particle/lua files this build never opens (GameMajorParticle36.txt/44.txt, DamageEffect_Dungeon.lua; JUNE2017_NAME_CONTRACT.md). |  | June2017 effect engine: enum data + loader-list gaps (Phase 11) |
| `HITTYPE_EXTEND` | 4194 | 2026-10-01 | June2017 effect rebuild Phase 11 -- extends HIT_TYPE/HIT_TYPE_COUNT (X2DamageManager.h) to 69 entries and m_DamageSoundName with it, so June effect data can set the 15 newer hit types without indexing the array out of bounds. |  | June2017 effect engine: enum data + loader-list gaps (Phase 11) |
| `LOCAL_MAP_UI` | 4204 | 2026-10-01 | at June's own numeric values (JUNE_UI_PARITY_PHASES.md Phase 0 findings). Matching Lua side added to march2014/data036/Enum.src.lua. |  | June UI parity, Phase 1: Local Map custom messages |
| `LOCAL_MAP_ICONS` | 4217 | 2026-10-01 | June2017 UI parity Phase 3 (JUNE_UI_PARITY_PHASES.md). Local-map dungeon buttons follow June: whole-function ports of June's CX2LocalMapUI::UpdateDungeonButtons (FUN_3293b5e0) and SecretDungeonDesc (FUN_32939aa0), June's ... |  | June UI parity, Phase 3: local-map dungeon buttons, Special tab (Henir) |
| `PARTY_UI` | 4232 | 2026-10-01 | June2017 UI parity Phase 5 (JUNE_UI_PARITY_PHASES.md). The party/dungeon panel follows June: whole-function ports in X2Lib/X2PartyUI.cpp of June's OpenPartyMenu (FUN_322c3d80, loads DLG_UI_PARTY_BOTTON_NEW_20161103.lua), UpdateMatchingUI ... |  | June UI parity, Phase 5: party/dungeon panel |
| `LINEMAP_TRIGGER` | 4261 | 2026-10-01 | LANOX_REMAINING_PHASES.md Phase 10. Bring the line-map trigger actions Ranox Phase 14 added up to June 2017's contract, so the June linemap rows can be restored verbatim. |  |  |
| `FIELD_MAP` | 4291 | 2026-10-01 | M-key field map follows the character in the Lanox village, fields and rest area (LANOX_REMAINING_PHASES.md Phase 5). |  |  |
| `ACTIVE_TRIGGER` | 4314 | 2026-10-01 | Port June 2017's CX2Game::SetActiveTrigger_LUA so NPC scripts can fire line-map triggers. The Fire Temple boss (FIRE_DEVIL_CHAIN_IPLITAN) fires trigger 0 at the end of its start state and trigger 1 (the floor collapse) when it roars; March ... |  |  |
| `BUFF_ENUM` | 4341 | 2026-10-03 | bring the C++ buff enums to June 2017. X2BuffFactor.h gains BBT 73-112 and BFT 14-17 with explicit values, fixes BTI_SI_SA_EEG_ATOMIC_SHIELD (124 -> 141) and BFI_BUFF_SWORD_ENCHANT_CHARGE_MP (338 -> 337), renames BTI_BUFF_INDURANCE_POWER / ... |  |  |
| `BUFF_LOADER` | 4362 | 2026-10-03 | the buff loader survives June data. CX2BuffTempletManager's AddBuffTemplet_LUA / AddBuffFactor_LUA / GetBuffFactorPtr are June's (326c3990 / 326c4050 / 326c32a0), including the second factor vector for _INT ids 10001+ (326c3f30), and ... |  | June buff port, Phase 3: the buff loader survives June data |
| `BUFF_TEMPLET_PARITY` | 4384 | 2026-10-03 | the buff templet/factor top level and the apply path are June's. CX2BuffTemplet::ParsingScript (3236a9b0) and CX2BuffFactor::ParsingBuffFactor (32d4d850, APPLY_CHECK_CONDITION 32d4d190/32d4d030) read June's keys; ACCUMULATION_MULTIPLIER ... |  |  |
| `BUFF_BEHAVIOR_PARITY` | 4407 | 2026-10-03 | existing buff behavior types brought to the June 2017 client. Factor parses gain June's extra floats (USE_CREATOR_UNIT_DATA, APPLY_TO_SPECIAL_ATTACK, HYPER_COUNT, ADDITIONAL_ATTACK_TYPE, RESET_SKILL_COOL_TIME SWAP_VALUE), the level slice ... |  |  |
| `BUFF_NEW_STAT` | 4435 | 2026-10-04 | the first seven ChangeStat-class behavior types June added, each with its own unit store and June's read sites: 76 CHANGE_HYPER_DURATION (GetHyperModeTime), 85/86 RANGE/MELEE_COMMAND_MP_GAIN_RATE (SetDamageExceptionProcess), 87/96 ... |  | June effect completion C4: damage math (range rate, Nasod combo gate, DIVISION window) |
| `BUFF_NEW_SKILL` | 4467 | 2026-10-04 | the skill- scoped behavior types June added. Complete (class, store, reader and host): 75 DOUBLE_ATTACK_BUFF (both DamageReacts), 91 CHARGE_MP_HIT_BY_SKILL_ID (SetDamageExceptionProcess) and 103 CHANGE_SKILL_COOL_TIME_SPEED ... |  |  |
| `BUFF_NEW_FLAG` | 4493 | 2026-10-04 | the flag and immunity behavior types June added, each with its class, unit store and June's read sites: 73 IGNORE_STATE_GUARD (GetFinalDamage; adds BTI_BUFF_SCHWERT_PLATZEN = 975), 94 IMMUNE_BUFF_DEBUFF and 95 IMMUNE_NON_CONTROL_DEBUFF ... |  |  |
| `BUFF_AWAKE_LOCK` | 4513 | 2026-10-05 | the host of BBT 110 AWAKE_IMPOSSIBLE. June's CX2GUUser::TryHyperModeStateChange (slot 343, 31fe9a90) replaces March's ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE; CanHyperMode (344) and the class hooks 434/435 are virtuals with June's bases; ... |  |  |
| `BUFF_QUICKSLOT_ITEM` | 4538 | 2026-10-05 | June's quick-slot item check. CX2Game::CheckQuickSlotItemSpecialAbility is June's 322fa960 (the per-special-ability check its quick-slot use 32951420 calls); CX2UIQuickSlot::KeyEventProcess routes its special-ability loop through it, ... |  |  |
| `BUFF_START_DELAY` | 4555 | 2026-10-05 | June's start-motion delay (CX2GUUser +0x92cc). -1 in the ctor; StageData reads START_MOTION_DELAYTIME (default 0); CX2DungeonGame::StageLoad stores it on a start-motion stage instead of changing state, and -1 with the (riding) wait state ... |  |  |
| `BUFF_RIDING_KEY_WAIT` | 4569 | 2026-10-05 | June's riding-key skill wait (CX2GUUser +0x92d0), found in the riding-key block 7e's start-motion reader edits. |  |  |
| `BUFF_NEW_FINALIZER` | 4591 | 2026-10-05 | June's finalizers BFT_AURA (14), BFT_STICK_SHAKE (15) and BFT_USE_SKILL_BY_SKILL_SLOTTYPE (16): factor parse, level slice, factory and classes; their unit counters (stick-shake count and reference, skill uses per slot type) and increment ... |  | June buff port, Phase 8: new finalizers and the buff-change sync (changes the P2P wire format) |
| `BUFF_GROUP` | 4607 | 2026-10-05 | June's buff groups and officially-debuff list. UnitApplyBuffGroup.lua loads after the factor files (326c26f0: AddBuffGroup_LUA, AddOfficiallyBuffList_LUA). |  | Applied mods (table row) |
| `BUFF_REACT_APPLY` | 4621 | 2026-10-05 | the can-apply check per factor with its BTI (so buff groups gate damage-data debuffs), the damage-data key check (3227a960; passes until the effect plan's C3 registers DAMAGE_DATA_KEY_NAME), June's pre-apply roll with the RESIST text ... |  | June effect completion C4: damage math (range rate, Nasod combo gate, DIVISION window) |
| `BUFF_BINDING` | 4632 | 2026-10-05 | June buff port Phase 10: June-only script bindings DeleteNonControlDebuff and GetBuffAccumulationCountNow on CX2GameUnit, and the ApplyBuffFactor cheat on g_pGameEdit (3274c880, with CX2BuffTimeFinalizerTemplet::SetDurationTime). |  | Applied mods (table row) |
| `BUFF_FACTOR_ID` | 4644 | 2026-10-05 | APPLY_LEARNED_SKILL_BUFF_FACTOR_ID (+ _RELATIVE_SKILL_ID / _RELATIVE_BUFF_TEMPLATE_ID), BUFF_FACTOR_ID_ULTIMATE(_PVP) behind June's skill-memo gate (31fcf3e0: absent here, off like X2DamageEffect's), and BUFF_FACTOR_ID_PVP in a PvP game.. |  | Applied mods (table row) |
| `BUFF_MEMO_DROP` | 4658 | 2026-10-05 | June buff port Phase 10: drop March's BUFF_FACTOR_ID_MEMO / BUFF_FACTOR_RELATIVE_MEMO_ID branch in ParsingBuffFactorID, as June did (no such key in June's binary). |  |  |
| `BUFF_DAMAGEDATA_KEY` | 4669 | 2026-10-05 | parsed and stored as June's SetDamageDataFromLUA does (32632390). Their readers (the party-only aura gate, the per-unit count) are Phase 10b's. |  | Applied mods (table row) |
| `BUFF_AURA_PARTY` | 4681 | 2026-10-05 | June buff port Phase 10b: IS_AURA_BUFF's party gate. In every hit path (GroundHit, AreaHit, UnitToUnit, MeshToUnit, CollisionDataToUnit, EffectToUnit, UnitToEffect) an aura buff skips this client's own unit when the attacker is not in my ... |  |  |
| `BUFF_AURA_REACT` | 4690 | 2026-10-05 | June buff port Phase 10b: an IS_AURA_BUFF hit plays no hitted-by-buff effect set (CX2GUUser/CX2GUNPC::DamageReact). |  |  |
| `BUFF_GUILD_REVERSE` | 4702 | 2026-10-05 | June buff port Phase 10b: June's guild skill Chance to Reverse check (32610df0). Skipped for ATTACK_OUR_TEAM, IS_AURA_BUFF and DT_BUFF damage, attacker first, and run on a hit after AttackResult as well as on a miss (March: miss only). |  |  |
| `BUFF_PREMIUM` | 4716 | 2026-10-05 | June buff port Phase 11: premium/world buffs. Loads BuffTemplet_INT.lua after BuffTemplet.lua (June 3258c470) with a count line per file; June's world-buff handlers (EGS_UPDATE_BUFF_INFO_IN_ROOM_NOT 326c9030, _NOT 326c9530): a per-unit ... |  |  |
| `BUFF_NOT_USE_IN` | 4743 | 2026-10-05 | June buff port Phase 11: stand-in for June's server-only BuffTemplet keys (user decision 2026-10-05). |  |  |
| `BUFF_CUSTOM_FUNCTION` | 4782 | 2026-10-05 | June buff port Phase 9a: June's custom-function-by-buff engine. CX2CustomFunctionByBuff gets June's four extra hooks (AttackResultByType, NPC final damage, buff applied, erase), CreateAndInsertCustomFunctionByBuff takes June's creator info ... |  |  |
| `BUFF_STORE_BUFF` | 4800 | 2026-10-05 | June buff port Phase 11b: IS_STORE_BUFF's consumers. June's clear-all (3229fb10) takes bKeepStoreBuff: with it set, my unit's buffs go to the gage first, and the gage keeps only the IS_STORE_BUFF factors (32494560 + predicate 32499c10), so ... |  | June buff port, Phase 14: play-test sweep and close-out |
| `BUFF_REPAIR_DISCOUNT` | 4812 | 2026-10-05 | June buff port Phase 11b: June's repair discount (32363f00). The world buffs are read from the Phase 11 record vector; June adds BTI 207 / 10027 / 10038 (and 876, whose June event check has no March home), drops March's 0.1 clamp, and its ... |  |  |
| `BUFF_DURATION_TIME` | 4824 | 2026-10-05 | June buff port Phase 11b: June's buff-icon time text (CX2MyGageUI::SetDurationTime, 32e608f0 + 32e5cb30). |  |  |
| `BUFF_NO_WORLD_BUFF_ID` | 4839 | 2026-10-05 | its two world-buff handlers, ReInit, the skill level, the repair discount and the icon time text read nothing else. |  |  |
| `BUFF_CLEAR_TYPE` | 4852 | 2026-10-05 | June buff port Phase 9c: June's sub-stage clear type (CX2DungeonSubStage +0x48, 5 = none). |  |  |
| `BUFF_FACTOR_WIRE` | 4867 | 2026-10-05 | the creator power rate member (+0x57, default 1.0), a copy ctor that copies the creator UID / NPC flag / power rate, and Put/Get in June's order (320e3950). |  |  |
| `BUFF_DEATH_CLEAR` | 4882 | 2026-10-05 | June buff port Phase 11b review: June clears a dying unit's buffs when its death state ENDS, not in DyingStart. |  |  |
| `SOCKET_PARITY` | 4899 | 2026-10-06 | June buff port Phase 16: June 2017's socket-option system. The loader keys June added (32726850) and the fields they fill, June's ConvertAndAdd body (3207cd70: MP pair, class specializations and 92's maximize as linear terms, no evade, no ... |  |  |
| `ATTACK_RANGE_DESIGN` | 4915 | 2026-10-06 | June buff port Phase 16: DESIGNED, NOT JUNE (user decision 2026-10-04, Q3). June's player attack range came from its server's KStat with a formula the client lacks; here each user's physic / magic range basic is 10% of that user's own ... |  |  |
| `ENCHANT_PARITY` | 4956 | 2026-10-06 | June's attribute-enchant system. Chunk A: EDT_ENCHANT_* renumbered to June's values (a combined type is the sum of its slots' element values; Enum.lua renumbered with it, they ship together), June's compose 32707ce0, decompose 32708080 and ... |  | June effect completion C6c: the effect-set buff list |
| `SOCKET_EDT` | 4971 | 2026-10-07 | June's handlers for the June-only socket EXTRA_DAMAGE_TYPEs (user 32034e50 / NPC 327995c0 Process_ExtraDamage): the effect-set spawns 68-72, 145-147, 149, 152-155, the buff calls 150/159, the 10-stack hit 151, June's second socket list ... | `JUNE_SOCKET_PARITY`, `JUNE_BUFF_BEHAVIOR_PARITY`, `JUNE_BUFF_ENUM` |  |
| `SOCKET_FEVER` | 5004 | 2026-10-07 | the hyper-mode socket type HMET_FEVER (June 3206d480 @3206d6ab). With a party, a game and no fever pass running it calls June's PartyFeverAllUser( 30.0, true ), then starts the socket's cool time. | `JUNE_SOCKET_PARITY` |  |
| `SPAWN_INHERIT` | 5022 | 2026-10-07 | what a June damage effect carries from whatever created it. The five spawner-tail callees as CEffect methods (June 31f8fdb0 parent->child state copy, 31f649d0 damage rate, 31f64c00 attack-trigger copy, 31f65e40 / 31f65f90 ignore defence), ... | `JUNE_DAMAGEEFFECT_PARSE`, `JUNE_DAMAGEEFFECT_RUNTIME` | June effect completion C6b: the spawner tail, the effect-set stamps and RT_CREATE_EFFECT_SET |
| `CREATE_EFFECT_SET` | 5040 | 2026-10-07 | AttackResult's RT_CREATE_EFFECT_SET case as June has it (31f85910 @31f86158-31f8672a). With a trace unit: the field-boss collision-box retarget gate (ported; its boss source is stubbed, March has no raid object). | `JUNE_DAMAGEDATA` | June effect completion C6b: the spawner tail, the effect-set stamps and RT_CREATE_EFFECT_SET |
| `DAMAGE_DATA_TABLE` | 5059 | 2026-10-07 | June's Elsword_DamageData.lua table. Binds CX2DamageManager::InsertDamageData_LUA (June 32621080), loads Elsword_DamageData.lua in the damage manager ctor (June 32620940 from 32622080 @326221da) and applies a record onto a DamageData when ... | `JUNE_DAMAGEDATA` | June effect completion C9: Elsword_DamageData.lua (the `DAMAGE_DATA_KEY_NAME` table) |
| `UNIT_VIEWER_EFFECT_TABLE` | 5086 | 2026-10-07 | June effect completion C9c-1. June's UnitViewerEffectSetTable.lua (18 AddUnitViewerEffectSet calls) is read into a per-motion map on CX2UnitManager (June 3237a1f0 insert, first row wins; 323760f0 lookup), loaded in ... |  | June effect completion C9c-1: UnitViewerEffectSetTable.lua (the unit-manager half) |
| `TUTORIAL_MODE` | 5107 | 2026-10-07 | June buff port Phase 14c (JUNE_BUFF_PORT_PHASES.md). June's CX2Main +0x16a is March's m_bIsPlayingTutorial (setter 31d716c0 = SetIsPlayingTutorial). |  | June buff port, Phase 14c: the tutorial flag (CX2Main +0x16a) |
| `BUFF_INSTANCE_KILL` | 5134 | 2026-10-07 | the consumer of the templet key IS_INSTANCE_KILL_DEBUFF. June's CX2GUUser::DamageReact (320462a0) records, on every damaging hit and before the hit's own buffs apply, whether the user carries a templet with the key (slot 200, 31fe4150) in ... |  | June buff port, Phase 14b: IS_INSTANCE_KILL_DEBUFF consumer |
| `UNIT_VIEWER_EFFECT_PLAYER` | 5151 | 2026-10-07 | Play 32959640, per-frame update 329585f0, create pass 32957980, sound pass 329582b0, stop 329594c0 / 32959250, Prepare 32957260, Draw 32957330, NotifyShowObjectChanged 32957760, dtor 31fae1a0; the CX2UnitViewerUI hooks PlayAnim 31fa3010 ... |  | June effect completion C9c-2b: the unit-viewer effect player and the retired class effects |
| `DAMAGE_MATH` | 5169 | 2026-10-07 | June effect completion C4 (8b damage math). Two DamageData keys that move damage numbers or targeting, each with its June consumer: DIVIDE_DAMAGE_FROM_RANGE (RANGE%d tables, map 3261a150, consumer 32622780 from GetFinalDamage @322857f6) ... |  | June effect completion C4: damage math (range rate, Nasod combo gate, DIVISION window) |
| `EMOTION_EFFECT_SET` | 5186 | 2026-10-07 | June effect completion C9e. June's per-emotion effect sets (32355a50: emotion id -> UIEffectSet name, 11 ids from 0x1c up) are played by CX2GUUser::PlayEmotion into +0x9434 (stopped in OnFrameMove and the dtor), by ... |  | June effect completion C9e: June's per-emotion effect sets (inert) |
| `HP_RATE_DAMAGE` | 5204 | 2026-10-07 | HP-rate damage. CX2GameUnit::GetHpRateDamage (June 32273e80) overwrites the damage with HP_RATE_DAMAGE_TYPE 1 now HP / 2 max HP times HP_RATE_DAMAGE_VALUE, only while now HP > 1.0. |  | June effect completion C10b-1: HP-rate damage |
| `NPC_FURY_MODE` | 5221 | 2026-10-07 | June 2017 effect completion C4c: NPC Fury mode (June raid bosses). State key FURY_MODE (bool, false) and the FURY_DATA table (ATTACK_POWER_RATE_FURY / ANIM_SPEED_RATE_FURY, 1.0) in CX2GUNPC::LoadStateData (June 32784c00); the NPC Fury byte ... |  | June effect completion C4c: NPC Fury mode and June's NPC anim speed |
| `NPC_ANIM_SPEED` | 5235 | 2026-10-07 | June 2017 effect completion C4c: June's NPC anim-speed rework. CX2GUNPC::ResetAnimSpeed = June's whole 327615d0: no m_AdditionalAnimSpeed.UpdateResultValue() at entry, then x the Rose factor (+0x8b6c) and the Hero-dungeon factor (+0x8fdc), ... |  | June effect completion C4c: NPC Fury mode and June's NPC anim speed |
| `HERO_NPC_ANIM_SPEED` | 5251 | 2026-10-07 | June 2017 effect completion C4d: Hero-dungeon NPC anim speed. CX2DungeonManager gets June's row list (g_pData +0x44 vector +0x5c, 12-byte { rate, dungeon ID, NPC ID }) and its Lua bind Add_HeroDungeon_ExtraStatForException (327db2c0); the ... |  | June effect completion C4d: Hero-dungeon NPC anim speed |
| `EQUIP_EFFECT_SET` | 5271 | 2026-10-07 | the one slice of June's division-item-templet system that is ported. CX2ItemManager binds AddDivisionItemTemplet_LUA (dispatcher 32818480, parser 32814da0 only; the rest is backlog Stage 11) and ResetItemManager loads ... |  | June effect completion C9f-2: per-equip effect sets (DITT_CONDITION_EQUIP_PLAY_EFFECT_SET) |
| `TOUCH_LAND` | 5288 | 2026-10-07 | TOUCH_LAND_DAMAGE_EFFECT%d's runtime. CXMeshInstance gets June's previous-frame velocity (m_vVelocityBefore, June mesh +0x54c); CEffect::OnFrameMove runs June's bounce gate (31f88790 @31f8a2a4-31f8a394: gravity.y < 0, previous velocity.y < ... | `JUNE_SPAWN_INHERIT` | June effect completion C7: touch-land runtime and three spawner gaps |
| `ANIMATION_NAME_TABLE` | 5309 | 2026-10-07 | June's AnimationNameTable.lua and CX2UnitViewerUI::PlayByMotionType (31fa7ef0): g_pUnitManager:AddAnimationName (3237a060) + the GetAnimationName lookup (32375ee0), loaded in ResetUnitManager (31ee6100); UNIT_VIEWER_UI_MOTION_TYPE ... |  | June effect completion C9g-2: June's PlayByMotionType and AnimationNameTable.lua |
| `PET_VIEWER_FX` | 5330 | 2026-10-08 | the viewer and engine half of June's pet / riding-pet viewer effect sets. CX2NPCUnitViewerUI (handle vector +0x2a8, the base viewer frame / prepare / draw hooks, IsAnimExist 32e4fe30, the handle stop 32e4fde0, SetScale 32e4fcf0), ResetPet( ... | `JUNE_UNIT_VIEWER_EFFECT_PLAYER`, `JUNE_EFFECTSET_RUNTIME` | June effect completion C9h-2a: June's pet / riding-pet viewer effect sets (viewer and engine half) |
| `ITEM_KIM` | 5352 | 2026-10-08 | June's 228-byte KItemFormatTemplet, field for field from the K0 map (june_item_kim/k0_record_map.tsv; June DoMemoryNotEncrypt 328101f0 checks N*0xe8): MAX_MODEL_COUNT_A_ITEM 8, the 7-float stat pool, the 6-byte StatRelLV pool, flags bit 7 ... |  | June item.kim adoption K2: June's 228-byte item record and June's item.kim |
| `KIM_MINIME_REMAP` | 5380 | 2026-10-08 | a STAND-IN, not June. June puts the Mini-me items on eqip 26 (EP_AC_MINIME) and earrings on 27 (EP_AC_EARRING), slots March does not have; the templet accessor maps 26 -> 17 (EP_AC_FACE1, where March's kim had the Mini-me items) and 27 -> ... | `JUNE_ITEM_KIM` | June item.kim adoption K2: June's 228-byte item record and June's item.kim |
| `PET_PREVIEW` | 5405 | 2026-10-08 | the cash-shop pet preview and the viewer hide sites, as June has them. CX2CashShop::ResetPetViewer = June 32228c90 (type -1 guard, UI-set + group-1 stops first, ResetPet( id, 3, 0 ), the 1.4 scale for pets 35-36, Wait_Transform, June's ... | `JUNE_PET_VIEWER_FX` |  |
| `PET_PREVIEW_LOOKUP` | 5428 | 2026-10-08 | June's pet-preview table whole. The 0x4c row (second id, start / end date, code event), the 6-argument loader 32812ce0 (no _IN_HOUSE_ popup / latch; each skipped row is logged), the lookup 32808770 (dated / code-event / second-id rows, ... |  |  |
| `RIDINGPET_STATE` | 5445 | 2026-10-08 | ANIM_START_TIME, ANIM_NAME_AT_PLAY_END (player 32d286c0), ONGROUND_CHANGE_STATE (check 32d3cff0, called from CX2GUUser::DoFrameMoveFuture; flag +0x620 cleared at the top of CX2GUUser::StateChange), the manager's ten anim-speed rates (June ... |  |  |
| `UNIT_SOUND` | 5483 | 2026-10-08 | the SoundPlayData record gains DELETE_CONDITION_SOUND values (the ctor seeds DCS_NONE) and LOOP; the nation-suffix SOUND_PLAY reader 32078fd0; the five delete-condition sound lists and their stoppers (attach 3227a740; damage react / state ... |  |  |
| `RIDINGPET_UI_FX` | 5508 | 2026-10-08 | the in-game riding pet's UI effect sets, whole. The engine holder (EffectSetInstance +0xd8, PlayEffectSet( name, CX2RidingPetPtr ) 321e2790 and its consumers: GetEffectPosition, GetCombineMatrix, GetEffectPositionByBoneName, ... | `JUNE_EFFECTSET_RUNTIME` |  |
| `UPDOWN_CANCEL` | 5546 | 2026-10-08 | UPDOWN_CANCEL_AFTER (user +0x8ca4, miss 9999.0) read in both CommonStateStart parses, and June's RidingAttackZEventProcess 32031e60 (March's tests + the UPDOWN branch after WALK), which DoFrameMoveFuture 320347d0 @32034872 also runs on the ... | `JUNE_RIDINGPET_STATE` |  |
| `COMMON_CANCEL` | 5571 | 2026-10-08 | June effect completion C9h-4c: USE_COMMON_CANCEL_AFTER (user +0x8ca0 byte, read by CommonStateStartFuture only) and its consumer, a block at the top of June's CommonEventProcess 31fe7610 @31fe7618-31fe7902: with the byte set, the state ... | `JUNE_BUFF_AWAKE_LOCK` |  |
| `CANCEL_CLAMP` | 5591 | 2026-10-08 | June effect completion C9h-4c: the cancel-after clamp. After the keys are read, June's CommonStateStartFuture 320506f0 @320568d3-32056a15 and CommonStateStart 32057b80 @3205fa38- raise every cancel-after float (ATTACK_EX, ATTACK, SKILL, ... |  |  |
| `NPC_SOUND` | 5610 | 2026-10-08 | June's sound records on the NPC and the event scene. NPC: the SOUND_PLAY nation reader (default and SOUND_SET tables), the SOUND_PLAY_ALL_OPTION records (set 0), the TimedEvent player with LOOP and the delete-condition attach, June's ... | `JUNE_UNIT_SOUND` |  |
| `SQUARE_SOUND` | 5630 | 2026-10-08 | June effect completion C9h-4b: the village square unit's sound records. SOUND_PLAY through June's nation reader, June's player 323f17b0 (the riding pet's clock while riding, the record's LOOP and 3D flags, a handle list for DCS_STATE_END ... | `JUNE_UNIT_SOUND` |  |
| `EFFECT_SET_DELETE` | 5656 | 2026-10-08 | June's effect-set delete conditions. Three June-only bytes of the user's LoadEffectSetFromScript (DELETE_EFFECT_SET_ON_STAGE_END, DELETE_EFFECT_SET_OFF_RIDING, DELETE_EFFECT_SET_ON_SKILL_STATE_END) and ... | `JUNE_EFFECTSET_PARSE` |  |
| `SOUND_AUTO_RELEASE` | 5682 | 2026-10-08 | CKTDXDeviceManager::PlaySound 31ce0e10 gets a 4th parameter (default true = OUR choice; June has none recoverable); in the open branch the sound's channel gets an FMOD END callback (31cdd4e0 / 31cdcca0) that queues the device (31cdcbb0); ... |  |  |
| `EFFECTSET_BUFF_LIST` | 5698 | 2026-10-08 | CX2EffectSet::AttachBuffFactorToEffectSet (321e3650) and ApplyBuffFactorListToDamageEffect (321ee480, called from OnFrameMove_CreateEffect @321ef689), the two CX2BuffFactor modifier helpers (32d418a0 reused, 32d41960 / 32d40dd0 new), and ... | `JUNE_EFFECTSET_RUNTIME`, `JUNE_ENCHANT_PARITY` | June effect completion C6c: the effect-set buff list |
| `FIX_EFFECTSET_BUFF_NULL` | 5711 | 2026-10-08 | JUNE_BUGS 60 fix (user, 2026-10-07: guard it). June 321ee480 has no null test on GetBuffFactorPtr( BFI ) and crashes on a BFI the manager does not know. | `JUNE_EFFECTSET_BUFF_LIST` | June effect completion C6c: the effect-set buff list |
| `SUPER_ARMOR_CONDITION` | 5729 | 2026-10-08 | June's directional / typed super armour. CommonStateStart reads SUPER_ARMOR_FRONT and the SUPER_ARMOR_CONDITION int list (SAC_FRONT 0, SAC_NORMAL_ATTACK 1, SAC_SPECIAL_ATTACK 2) into a per-state set next to bSuperArmor (June 32057b80 ... |  | June effect completion C3c: super-armour condition, DamageReactStateChange's gates, ADD_MEMBER_COUNT |
| `RIDINGPET_USER_KEYS` | 5761 | 2026-10-08 | the riding pet's StateChange / StateChangeFuture user and village keys that C9h-3b left as a comment (June 32d2f560 @32d3110d-32d35a05, 32d2c8b0): DISABLE_GRAVITY_TIME_START / _END into the user (after SPEED_TIME, miss 0 / 0), ... | `JUNE_RIDINGPET_STATE`, `JUNE_RIDINGPET_UI_FX`, `JUNE_UNIT_SOUND`, `JUNE_SQUARE_SOUND`, `JUNE_UPDOWN_CANCEL`, `JUNE_EFFECT_SET_DELETE` |  |
| `USEFUL_VALUE` | 5816 | 2026-10-08 | C9h-4f: June's USEFUL_VALUE<n> per-state float table. June reads up to five tables USEFUL_VALUE0..4 of a state ( three floats each, a missing value is 0 ) into two member sets: CommonStateStartFuture 320506f0 @32056a15 fills the Future set ... |  |  |
| `DAMAGEEFFECT_SOUND` | 5831 | 2026-10-08 | C9h-8: June's damage-effect sound play CEffect::OnFrameMove_SoundData 31f6eca0. March opened each SOUND<n> with OpenSound ( closed by name in the dtor, CLOSE_SOUND_TEST ) and played it once, ignoring LOOP. June calls the device PlaySound( ... | `JUNE_SOUND_AUTO_RELEASE`, `JUNE_DAMAGEEFFECT_PARSE` |  |
| `USER_STATE_ID16` | 5854 | 2026-10-08 | C3d-2: June's 16-bit user state id ( C3d-1 sites, june2017_re/c3d1_state_id_sites.tsv ). March kept the user state id in a byte, so Chung's last state ( 255 ) already aliased the -1 sentinel and nothing could go above it. |  | C3d-2: 16-bit user state id and action id (2026-10-08) |
| `USER_ACTION_ID16` | 5864 | 2026-10-08 | C3d-2: June also widened the user action id: SyncData::nowAction is a protected short ( Future +0x7e84, 31fe9a90 @31fe9b0d ) and the P2P sync field is 2 bytes ( 32d5be30 bit 2, the name m_ucNowAction is kept ). |  | C3d-2: 16-bit user state id and action id (2026-10-08) |
| `DAMAGE_REACT_HOOKS` | 5876 | 2026-10-08 | C10e: June's common CX2GUUser / CX2GUNPC::DamageReact steps March lacked. Two empty CX2GUUser attacker hooks ( June vslots 0x4f8 / 0x4fc, called with ( DamageData, damage ) on a user attacker, user DamageReact 320462a0 @320495d8, NPC ... |  | Applied mods (table row) |
| `SOCKET_COOLTIME_ON_HIT` | 5887 | 2026-10-08 | C10e: socket option 2000145 ( text "1% Chance to decrease Cooldown time by 5 seconds", code 3% ). |  | Applied mods (table row) |
| `DAMAGEEFFECT_NOTIFY_OWNER_FIRST` | 5897 | 2026-10-08 | C9h-8 review: June's CEffect dtor 31f78550 @31f78584-31f785cf calls the owner's NOTIFY_TO_OWNER_WHEN_DIE hook ( vslot 0x2c4 ) first, before the DELETE_WHEN_EFFECT_END stopper; March calls DieDamageEffectProc late in the dtor. |  |  |
| `BOUNDING_STATE` | 5919 | 2026-10-09 | Effect completion C3d-3: June's bounding back-speed state. DamageReactStateChange's RT_BOUNDING case ( June 31feabd0 @31feb68f, NPC 32769060 @32769da7 ) stores BOUNDING_BACK_SPEED_X/Y and enters USI_BOUNDING_DOWN ( in the air ) or ... | `JUNE_USER_STATE_ID16`, `JUNE_USEFUL_VALUE`, `JUNE_DAMAGEDATA` | C3d-3: June's bounding back-speed state (2026-10-09) |
| `LIMIT_MAX_G_SPEED` | 5962 | 2026-10-09 | June effect completion C3g (JUNE2017_EFFECT_COMPLETION_PHASES.md). June's state key SET_LIMIT_MAX_G_SPEED: a float read at every user state start (CommonStateStartFuture 320506f0 @32054734, default 1.0) and into the NPC StateData ... |  | Applied mods (table row) |
| `APPLY_MOVEMENT_SPEED` | 5977 | 2026-10-09 | June effect completion C3h. June's APPLY_MOVEMENT_SPEED state key (user CommonStateStartFuture 320506f0 @32054181, a bool read at every state start, miss = false). |  |  |
| `NPC_FORCE_BACK_SPEED` | 5987 | 2026-10-09 | June effect completion C3i (JUNE2017_EFFECT_COMPLETION_PHASES.md). June's NPC DamageReactStateChange (32769240) opens SMALL / BIG / DOWN / UP / FLY / DRAG_UP with the FORCE_BACK_SPEED gate that C3c ported for the user: a super-armoured NPC ... | `JUNE_DAMAGEDATA` |  |
| `NOT_UP_STATE` | 5995 |  | C3i. June's NPC INIT key NOT_UP_STATE (+0x8ac4; read under SERV_IRUHADEV_JUNE_BOUNDING_STATE) in the RT_UP / RT_FLY / RT_DRAG_UP cases (@32769b1a, b72, be9, c41, cad, ced): the launch's nowSpeed.y write is skipped and the reaction state ... | `JUNE_NPC_FORCE_BACK_SPEED`, `JUNE_BOUNDING_STATE` |  |
| `NOT_AFFECT_BLACKHOLE` | 6008 |  | C3i. June's black-hole pull (CEffect::OnFrameMove_BlackHoleData 31f6eea0). The NPC INIT key NOT_AFFECT_BLACKHOLE (InitComponent 327a0fa0 @327a5f0b, +0x8ac5, bool, default false; 36 June NPC scripts) keeps an NPC out of every pull ... |  |  |
| `NO_FALL_DOWN_AT_STATE` | 6013 |  | a fall-down NPC in such a state is not pulled by a black hole (31f6eea0 @31f6f0b2) and is not knocked down when a user resurrects in the dungeon (Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK, @3230ec81).. |  |  |
| `LINE_INDEX16` | 6019 |  | C3e-1. June's 16-bit lastTouchLineIndex of the user sync (SetData 3201f5f0 syncData +0x20 short, SendFrameData 3203d4d0, PhysicProcess 32015b30 @32019dc9, packet bit 0x40 = 2 bytes in Deserialize 32d5be30). |  |  |
| `SYNC_CLASS_FLOAT` | 6023 |  | C3e-1. June's class-skill float slot of the user sync (packet bit 0x200000, 4 bytes after RelativePos 0x8000 and before BuffSyncCount, SetData 3201f5f0 syncData +0x58, protected slot Future +0x7ed8 / Now +0x8178, default 0). |  |  |
| `DAMAGE_EXCEPTION` | 6033 | 2026-10-09 | the layers of SetDamageExceptionProcess that March lacks. CX2GameUnit's ChangeDamageType attack-type member (June +0x4108: 2 in the ctor, -1 in ResetDamageType) and the user layer's gate on it (32045340 @320453a0); the user vslot 0x758 ... | `JUNE_DAMAGEDATA`, `JUNE_BUFF_NEW_STAT`, `JUNE_BUFF_CUSTOM_FUNCTION` | June2017 effect engine: the damage-effect hit test (Phase 9) |
| `EFFECT_ALPHA_OPTION` | 6056 | 2026-10-09 | June effect completion C10b-2 (JUNE2017_EFFECT_COMPLETION_PHASES.md, C10). June's effect-alpha game option engine: CKTDXApp stores three floats (my / my team / other team, default 1.0, set clamped to [0.1, 1.0], 31be2a90 / 31be2b00), ... | `JUNE_EFFECTSET_RUNTIME`, `JUNE_BUFF_CUSTOM_FUNCTION`, `JUNE_UNIT_VIEWER_EFFECT_TABLE` | Applied mods (table row) |
| `EFFECT_ALPHA_PARTY_BOT_AS_PLAYER` | 6073 | 2026-10-09 | so the offline AI-party bots (NAT_ALLY NPCs owned by my unit) faded their whole body and weapon with MY_EFFECT_ALPHA. A real other player is a user unit: only their skills fade, with MY_TEAM_EFFECT_ALPHA, and their body never does. | `JUNE_EFFECT_ALPHA_OPTION`, `AIPARTY_PERSIST` |  |
| `WORLD_RATE_APPLY` | 6090 | 2026-10-09 | the Lua setter CX2World::SetGravityAndSpeedRate ( June 32800800 ) stops being an inline store; in a GT_DUNGEON game it walks the unit list ( 327cab80 ) and calls CX2GameUnit::ApplyJuneWorldGravityRate ( 32296290 ): a one-time PhysicParam ... |  |  |
| `WORLD_RATE_TRIM` | 6099 | 2026-10-09 | June effect completion C3n. June's trims of the world gravity rate: CX2GameUnit::ResetPhysicParam ( June 322886a0 ) scales only the three gravity fields ( March's walk / run / jump / dash-jump x rate are gone ), and ... |  |  |
| `COMMON_RANDOM_STATE` | 6111 | 2026-10-09 | June effect completion C3e-2. June's START / WIN / LOSE variant index N: the roll ( June 31fe22d0, March's InitStateCommonRandom ) also stores each list's picked index in a map ( +0x8aac, keys USI_START / WIN / LOSE ) and clears the ... |  |  |
| `SPEED_TIME` | 6127 | 2026-10-10 | June effect completion Phase C3l: June's user SPEED_TIME additions. Loader 31fe3a60 / parser 31fe2740: TIME_SPEED gains a 5th column, a bool ( bJuneRotateByDirZ ); SPEED_TIME_ULTIMATE is chosen when the Future state's equipped skill ... |  |  |
| `USEFUL_STRING` | 6138 | 2026-10-10 | June effect completion Phase C9h-4g: June's USEFUL_STRING<n> rows, read right after USEFUL_VALUE in CommonStateStartFuture 320506f0 @32056b0e ( Future, +0x8f40 ) and CommonStateStart 32057b80 @32060dda ( Now, +0x8f30 ): a vector of wide ... |  | Applied mods (table row) |
| `NOT_SELECT_UNIT_OFFSET` | 6148 | 2026-10-10 | June effect completion Phase C9h-4g: CUSTOM_OFFSET_NOT_SELECT_UNIT ( two floats, +0x8f54 / +0x8f58 ) and USE_NOT_SELECT_MOTION_DIFFERENCE ( bool +0x8f5c, a miss reads TRUE ), read after USEFUL_STRING by both parses into ONE set ( the Now ... |  | Applied mods (table row) |
| `RESERVED_STATE_IDS` | 6159 | 2026-10-10 | reserves 11 June base user state ids in June's order: USI_EVENT_SNIPER_INIT .. USI_EVENT_GUN_DANCE_BACK_STEP ( 10, before USI_BOUNDING_* ) and USI_WAIT_STATE_CHANGE ( after them ). | `JUNE_USER_STATE_ID16` | Applied mods (table row) |
| `WAIT_STATE_CHANGE` | 6173 | 2026-10-10 | June effect completion Phase C9h-4g: June's USI_WAIT_STATE_CHANGE. Its OnEventProcess 31fc8620 ( state time above USEFUL_VALUE0[0] of the Future set -> USI_WAIT ), the registration helper the character ports call per class ( June registers ... | `JUNE_RESERVED_STATE_IDS`, `JUNE_USEFUL_VALUE` | Applied mods (table row) |
| `FLY_COUNT` | 6213 | 2026-10-10 | June effect completion C3k: June never counts fly hits. Neither DamageReact ( user 320462a0, NPC 327b1f30 ) touches m_FlyCount ( CX2GameUnit +0xdf8 ): no RT_UP / RT_FLY / RT_DRAG_UP increment and no FORCE_FLY reset. |  |  |
| `NPC_CINEMATIC_UI` | 6236 | 2026-10-10 | June effect completion C3o: the NPC state key IS_APPLY_CINEMATIC_UI ( June bit 0x4f, parser 32784c00 @3278a049, default TRUE ). |  |  |
| `NPC_STATE_START_EFFECT_SET` | 6257 | 2026-10-10 | June effect completion C3o: June's NPC CommonStateStart ( 327ac9d0 @327af3c5-327af3d7 ) runs the state's effect-set list once at state start ( CommonFrameMove_EffectSet( GetNowAnimationTime() ), June 32776e10 with vslot 0x3bc ), so ... |  |  |
| `WORLD_COLOR_WINDOW` | 6267 | 2026-10-10 | June effect completion C3o: the state table CHANGE_WORLD_COLOR_DATA { START_TIME, END_TIME } ( CX2GameUnit +0x244 / +0x248 ). |  |  |
| `COMMON_SKILL_STATE` | 6275 | 2026-10-10 | June effect completion C9h-4h: CX2GameUnit::InitSystem ( June 32286cb0 @32287856 ) loads CommonSkillState.lua into every unit's Lua state after the unit script ( users and NPCs ): the USI_EVENT_* state tables. |  |  |
| `EVENT_STATES` | 6285 | 2026-10-10 | June effect completion C9h-4h: the "Heavenly Combat Support Kit" event states USI_EVENT_SNIPER_INIT .. USI_EVENT_GUN_DANCE_INIT ( ids reserved by C9h-4g ), registered for every class like June 3202b110, their handlers, the members ... |  |  |
| `EVENT_KIT_PICKUP` | 6297 | 2026-10-10 | items 100005740..100005743 start the Sniper / Meca Drop / Proton Bomb / Gun Dance state on the picker while its kit latch is open. | `JUNE_EVENT_STATES` |  |
| `CUTIN_MY_UNIT_GUARD` | 6309 | 2026-10-10 | June effect completion C3r: CX2GUUser::GetShowCutInAndChangeWorldColor ( June 32001380 @320013d0-320013f7 ) in a battle field: a user that is not your own unit returns false while the game has no my-unit, instead of reading GetMyUnit()'s ... |  |  |
| `NPC_EFFECT_SET_LIST` | 6319 | 2026-10-10 | June effect completion C3q: June's NPC effect-set list. LoadStateData ( June 32784c00 @32787775-32787d09 ) reads EFFECT_SET_LIST_FINAL = { EFFECT_SET0 = { EFFECT_NAME, START_TIME, CONSUME_MP, DELETE_CONDITION = {...} }, EFFECT_SET1 ... } ... |  |  |
| `FORCE_CINEMATIC_STATE` | 6328 | 2026-10-10 | June effect completion C3s: the NPC INIT_COMPONENT key FORCE_CINEMATIC_STATE ( June 327a0fa0 @327ab5fe, +0x8f8c, a state name ). |  |  |
| `BF_FOCUS_NEEDS_MY_UNIT` | 6336 | 2026-10-10 | June effect completion C3s: CX2BattleFieldGame::UpdateUnitPointer ( June 32d530a0 @32d530a3-32d530b4 ) returns before the base update and the NPC focus walk while the game has no my-unit.. |  |  |
| `WORLD_LOAD_LOG` | 6347 | 2026-10-10 | June effect completion C3x: make a world that fails to load loud ( offline_server.log, JUNE_FX C3x ). |  | June maps under the June dungeon scripts (june-effect C3x, 2026-10-10) |
| `IS_VOTING_LUTO` | 6360 | 2026-10-10 | June effect completion C3z-1 play-test 2: bind June's CX2Game:IsVotingLuto. Ten adopted June gate scripts ( SMALLGATE_WALLY_CASTLE_A, SMALLGATE_BESMA_AIR_SHIP, ... ) ask it before LiveActiveNPCNum; unbound, the call raised "can't find ... |  |  |
| `NPC_CREATE_NPC_REQ` | 6382 | 2026-10-10 | June effect completion C3z-4 batch 1: bind June's NPC-side CreateNPCReq_LUA, CreateNPCReq_LUA2, CreateNPCReqElite, CreateNPCReqAlly and PushCreateNPCReq_Lua on CX2GUNPC ( 113 / 4 / 13 / 4 / 82 dungeons, june2017_re/c3z4_bind_plan.tsv ). |  |  |
| `NPC_STORED_GUUSER` | 6391 | 2026-10-10 | one user observer per NPC ( +0x8d64 ), set and read only for a unit whose game unit type is GUT_USER. 16 linker / shadow master / trock scripts, 37 dungeons.. |  |  |
| `GAME_LUA_INT` | 6401 | 2026-10-10 | ten Lua scratch ints on the game ( +0x7e0, zeroed in the ctor 32306600 ), no bounds check ( JUNE_BUGS ). |  |  |
| `NPC_INIT_STATE_COOL_TIME` | 6410 | 2026-10-10 | the named state's elapsed time is set to its STATE_COOL_TIME, so it is ready at once ( fire devil / dryad kenta / smith stain scripts, 15 dungeons ).. |  |  |
| `GAME_HOST_GAME_UNIT` | 6419 | 2026-10-10 | June effect completion C3z-4 batch 1: bind CX2Game::GetHostGameUnit, which March has and never bound. |  |  |
| `NPC_GET_DYING_START` | 6428 | 2026-10-10 | June effect completion C3z-4 batch 1: bind CX2GameUnit::GetDyingStart on CX2GUNPC, where June binds it ( 31e4ce00, @31df98c5: byte +0xdac, set by DyingStart 32299c80 = March m_bDyingStart ). |  |  |
| `GAME_STAGE_QUERY` | 6437 | 2026-10-10 | June effect completion C3z-4 batch 1: June CX2Game::GetIsLastStage ( 322f36e0, @31dd22b0 ) and GetNowStateID_LUA ( 322cf9b0, @31dd9a6c ). |  |  |
| `GAME_NPC_COUNT` | 6447 | 2026-10-10 | the number of NPCs in the NPC list whose team ( protected +0x278 ) equals the argument, no other test. |  |  |
| `SUBSTAGE_NPC_TEAM` | 6463 | 2026-10-10 | June effect completion C3z-4 play-test 3: read a dungeon-script NPC's TEAM, as June's sub-stage NPC parser 327eb8e0 does ( @327ec9a9-327eca04: NPCData +0x5c = TEAM, 2 = TN_MONSTER when absent ), and have the offline emulator send it as ... |  |  |
| `HYPER_CUT_IN_TIME` | 6506 | 2026-10-10 | June effect C3t. June's user state key SHOW_HYPER_CUT_IN_TIME ( 32057b80 @32060505, user +0x9280 ) and its reader ( CommonFrameMove 32063650 @3206ce0a ): user vslot 0x838 ( 32014fc0 ) shows the unit templet's m_HyperCutIn ( or ... |  |  |
| `DUNGEON_ADEQUATE_MEMBERS` | 6514 | 2026-10-11 | CX2DungeonManager::GetAdequateMemberCount ( 327da980 ), and the party UI's need_user text ( UpdateAdequateMemberCount 322be5c0: "min~max", red below the minimum ).. |  |  |
| `DUNGEON_FIX_MAX_DIFFICULTY` | 6523 | 2026-10-11 | IsFixMaximumDifficulty ( 327da9f0 ), GetDefaultDifficulty ( 327db250 ). A dungeon whose top-difficulty row sets it is played at that difficulty: UpdatePartyDLG switches to it ( 322c2e50 ), the difficulty boxes are hidden ( 322b6600 ), the ... |  |  |
| `DUNGEON_SUITABLE_LEVEL` | 6531 | 2026-10-11 | IsActiveDungeon ( 327da190 ) and the party-list join check ( 322ca040 ) use the row's level slack instead of a hard-coded 2, and a required quest must be complete.. |  |  |
| `HENIR_HARD` | 6538 | 2026-10-11 | m_HardMinLevel / m_HardMaxLevel ( +0x94 / +0x98 ): the party UI shows them for Henir challenge ( 322c2e50, 322c1700, 322beb90 ).. |  |  |
| `DUNGEON_CHECK_CONDITION` | 6547 | 2026-10-11 | June effect C3z-5. DungeonData DUNGEON_CHECK_CONDITION ( +0xc4, std::bitset<15> ) and CX2DungeonManager::IsDungeonCheckCondition ( 327d9df0 ). |  |  |
| `WORLD_ANIM_WAIT` | 6555 | 2026-10-11 | June effect C3y-1b. Binds June's CX2WorldObjectSkinMesh "SetAnimWait" ( name use 31dbffdd in binder 31d95080, body 327f6990: if ( m_pXSkinAnim ) m_pXSkinAnim->Wait(), 31d28d30 = CKTDGXSkinAnim::Wait ). |  |  |
| `MOVIE_DESCRIPTION` | 6565 | 2026-10-11 | the NPC script subtitle. June CX2GUNPC::SetMovieDescriptionData( string ID, seconds ) ( 32758730, bound @31e0419c ) stores both on the now state ( CX2State 31ef7370: +0x1ec / +0x1f0 ), and the state draws it last in its render pass ( ... |  |  |
| `NPC_SIEGE_QUERY` | 6573 | 2026-10-11 | the nearest other live NPC in siege mode ( GetSiegeMode, +0x8aa0 ), from 1e6; June returns the UID's low 32 bits, 0 when none ( Bunker / Elder secret bunker scripts, 10 dungeons ).. |  |  |
| `NPC_ACTIVE_MONSTER` | 6581 | 2026-10-11 | June effect completion C3z-4b: bind CX2GUNPC::IsActiveMonster, which March has and never bound. |  |  |
| `GAME_KEYCODE_NPC_COUNT` | 6589 | 2026-10-11 | NPCs of TN_MONSTER that are not GUSI_DIE, have HP left ( no gage data counts ) and carry the key code ( 3 dungeons ).. |  |  |
| `UNIT_PLAY_2D_SOUND` | 6597 | 2026-10-11 | June's PlaySound_LUA ( 322763c0 ) with the sound core's 3D flag off ( 32276220( name, 0, 0 ) ), same DieLight.ogg effect-option gate ( 7 dungeons ).. |  |  |
| `NPC_CHANGE_MOTION` | 6606 | 2026-10-11 | closes and reopens the motion mesh, drops the anim and both collision / attack list sets, rebuilds the anim ( CreateAndSetAinmXSkinMesh, 3275c950 ), replays the now state's play type / wait / anim speed and rebuilds the after image ( 5 ... |  |  |
| `GAME_WORLD_FADE` | 6613 | 2026-10-11 | the world's FadeOriginWorldColor when GetWorld() is set ( 6 dungeons ).. |  |  |
| `NPC_SHOW_OTHERS` | 6620 | 2026-10-11 | every other NPC's gage UI and object show flag ( an inlined SetShowGage + SetShowObject ), 6 dungeons.. |  |  |
| `GAME_HENIR_QUERY` | 6629 | 2026-10-11 | June effect completion C3z-4b: June CX2Game vslot 0x210 CanPassThisStage / 0x214 GetHenirLimitLevel ( vcall thunks 31e4e7f0 / 31e4e930, bound @31dd9c27 / @31dd9de3 ). |  |  |
| `GAME_RANDOM_USER` | 6637 | 2026-10-11 | a random user that is not GUSI_DIE and has HP left, NULL when none ( Elysion laser scripts, 2 dungeons ).. |  |  |
| `GAME_GO_NEXT` | 6645 | 2026-10-11 | June effect completion C3z-4b: June CX2Game::GetIsGoNextPacketSended_LUA / Send_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ ( 322f14d0 / 322f1550, bound @31dd945f / @31dd9757 ): in a dungeon game, read m_bGoNextPacketSended ( +0xc4c ) / send the ... |  |  |
| `EFFECT_MAIN_POS` | 6652 | 2026-10-11 | the main mesh instance's position, else the effect's own ( 3 dungeons ).. |  |  |
| `NPC_STATE_NAME` | 6659 | 2026-10-11 | the name of the now state ( nowState, +0x8454 ) in the init state data's name map, NULL when absent ( 3 dungeons ).. |  |  |
| `USER_UNIT_TYPE` | 6666 | 2026-10-11 | June effect completion C3z-4b: bind CX2GUUser::GetUnitType, which March has and never bound ( June 31e4d0e0, @31e13a93: m_pUnit's templet unit type ), 3 dungeons.. |  |  |
| `WORLD_MESH_BY_NAME` | 6674 | 2026-10-11 | the first static-mesh world object whose name ( the mesh file SetXMesh_LUA names it after ) matches, case insensitive; NULL when none ( Red Dragon raid, Eltrion scenes, 3 dungeons ).. |  |  |
| `LINEMAP_QUERY` | 6683 | 2026-10-11 | June effect completion C3z-4b: June CKTDGLineMap::GetCenterPositionLineIndex( line ) ( 31c6ad50, @31bedb4c ) and SetEnableLinebyPos( enable, pos, distance, can-down only ) ( 31c6afb0, @31bee5a5 ): a line's centre ( zero for a bad index ); ... |  |  |
| `NPC_IMMUNE_EXTRA_DAMAGE` | 6690 | 2026-10-11 | writes byte +0x4104, whose June readers are March's m_bImmuneToExtraDamage readers ( NOT_EXTRA_DAMAGE key ), 2 dungeons.. |  |  |
| `WORLD_CAMERA_DISTANCE` | 6698 | 2026-10-11 | which March has and never bound. June 31e4ca10 ( @31dbad5e ) writes the four floats June's SetProjection ( 327fb590 ) also writes, as March's SetProjection does ( 1 dungeon ).. |  |  |
| `USER_LAST_TOUCH_LINE` | 6706 | 2026-10-11 | June effect completion C3z-4b: bind CX2GUUser::GetLastTouchLineIndex. June binds the vcall thunk 31e4e960 ( slot 0x12c, @31e03bcd ) on CX2GUUser too; the GUUser slot ( 3208e100: bFuture ? +0x7e54 : +0x80f4 ) is March's override ( 2 ... |  |  |
| `UNIT_BASE_BINDS` | 6714 | 2026-10-11 | June effect completion C3z-4b batch 3: June also binds GetIsRight / GetRotateDegree on CX2GameUnit ( vcall thunks 31e4eaf0 slot 0x170 / 31e4eac0 slot 0x150, @31ded26f / @31dee047 ) and SetIsRight( bIsRight, bFuture ) on CX2GUUser ( ... |  |  |
| `DUNGEON_CANNOT_INPUT` | 6722 | 2026-10-11 | the body of the slide-shot one ( 325c73e0 = March CX2SlideShot::CannotInput ): my unit's can-not-input time, for the dungeon trigger scripts ( 6 dungeons ).. |  |  |
| `ATTACH_SKIN_MESH_SKIP_MISSING` | 6732 | 2026-10-11 | June effect C3y-1b play-test fix. June's NPC init opens an ATTACH_SKIN_MESH's skin mesh first ( 327a8fff-327a9013: 31cdef40 = OpenXSkinMesh( name, L"Bip01" ) ) and skips the whole block when it is NULL ( 327a901e / 327a9020 je 327a9814 ). |  |  |
| `SKIP_START` | 6746 | 2026-10-11 | June effect completion C3p: June's skip-the-start ( ESC ) system. The NPC state key DIRECTION_STATE ( June bit 0x4d, parser 32784c00 @3278bbc7, a miss stores false; 81 June scripts: dungeon openings and boss intros ) arms a list on the ... | `JUNE_BUFF_START_DELAY` |  |
| `ATTACH_SKIN_MESH_ANIM` | 6774 | 2026-10-11 | June 2017 ATTACH_SKIN_MESH%d shell ( Lanox FIRE_DEVIL_* / SECRET_* fighter + berserker ). June's NPC init ( FUN_327a0fa0, 327a8dd9..327a9870 ) opens the block's skin mesh ( OpenXSkinMesh 31cdef40, NULL = skip ), stores SKIN_MESH_RENDERTYPE ... |  |  |
| `FORCE_DOWN_FLUSH` | 6796 | 2026-10-12 | June effect completion C3f-1: June's force-down flush. The user's StateChange ( 320348c0 @3203492b-32034994 ) empties the force-down gage and clears its flag ( UpdateFlushForceDown 32a33dc0 ) when the unit LEAVES DOWN_FRONT / DOWN_BACK / ... |  |  |
| `TIMER_FOCUS_UNIT` | 6819 | 2026-10-12 | June effect completion C3v: June's timed focus unit. CX2Game::SetTimerFocusUnit ( 322d63f0, vslot 0x88 ) stores the unit, time, distance and two new bools ( CanNotInput, tracking camera ) in a struct ( +0x2ec ) instead of March's ... |  |  |
| `HIT_STOP_LISTS` | 6841 | 2026-10-12 | June effect completion C3e-4: June's per-attacker hit-stop. SetStopTime / SetStop2Time take ( float, bool bDamageDataStop, CX2GameUnit* pAttacker, bool bForce ) ( user 31ffe4f0 / 31ffe750, NPC 32772aa0 / 32772c10 ). |  |  |
| `DAMAGE_REACT_ORDER` | 6887 | 2026-10-12 | June effect completion C3e-5: June's CX2GUUser::DamageReact block order ( 320462a0 ). June runs the attacker-front test, the Future copy of it, the revenge / reflect detection and the revenge / reflect / avoidance block BEFORE the ... |  |  |
| `SUPER_ARMOR_HIT_STOP` | 6896 | 2026-10-12 | June effect completion C3e-5: June's super-armor hit-stop in user / NPC DamageReact ( 320462a0 @32046ce3, 327b1f30 @327b3163 ). | `JUNE_HIT_STOP_LISTS` |  |
| `CUTIN_SHOW_OPTION` | 6921 | 2026-10-14 | June effect completion C3t-1: June's CX2GUUser::GetShowActiveSkillShow ( 31fd3180 ). June's cut-in option is a bitmask ( 1 mine, 2 party only, 3 mine or party ); March's enum is mapped to it by name ( ONESELF 1, PARTY_MEMBER 3, HIDE 0 ). |  |  |
| `SHOW_BOSS_NAME_AT_START` | 6942 | 2026-10-14 | June effect completion C3w: June's CX2DungeonSubStage::ShowBossNameAtStart ( 327ef160 ) whole. | `JUNE_FORCE_CINEMATIC_STATE` |  |
| `SOUND_SET_PICK` | 6966 | 2026-10-14 | June effect completion C3t-2: June's user sound-set pick in CX2GUUser::CommonStateStart ( 32057b80 @3205db96-@3205e70a ). | `JUNE_HYPER_CUT_IN_TIME`, `JUNE_UNIT_SOUND` |  |
| `SUMMON_NPC_SOCKET_GATE` | 6992 | 2026-10-14 | June effect completion C3u: June CX2GUUser::SumSocketOption 3204f190 @3204fbc9-3204fc1d. A summon-NPC socket for NPC 4119 / 4120 ( June *_KNIGHT_BASTARD_EVENT_TITLE; June sockets 2000035 / 2000036 on June event titles 100220 / 100230 ) is ... |  |  |
| `SIMPLE_DLG` | 7002 | 2026-10-15 | a script-made dialog on CX2Game ( +0xba8: CreateSimpleDLG 322def10 / DeleteSimpleDLG 322cc520 / GetSimpleDLG 31e4de40, deleted by the game dtor 323080c0 ) with its 12 Lua methods ( binder 31d95080 @31e42615..@31e43fde ), and June's ... |  |  |
| `WORLD_FESTIVAL` | 7017 | 2026-10-15 | June effect completion C3x-1: June's WorldData variants. AddWorldData_LUA ( 32805770 ) keeps the plain dataFileName, swaps in dataFileName_Festival while the clock is before 2016-03-16 04:00, and reads dataFileName_CodeEvent; CreateWorld ( ... |  |  |
| `EL_DASHJUMP_XZ_FIREBALL` | 7393 | 2026-10-17 | Elesis SPK / SBH dash-jump X-Z (and its Z re-attack) fireball, June 32b770e0: CreateInstance of DamageEffect_Elesis_SPK/SBH_DashJump_Combo_XZ at Dummy2_Lhand, scale 1, angle z -45, MP via IsEnoughMPForAttack, instead of the x2 / z+10 effect set (June's data already carries the x2: the fireball drew at 2x, aimed up) ... |  |  |

(Flag names above drop the `SERV_IRUHADEV_JUNE_` prefix; `needs` drops `SERV_IRUHADEV_`.)

Flags **off** (commented out in Always.h; 30): the `_DEBUG` diagnostics, which you turn on only for a play-test and off again after, and `FIX_RIDINGPET_ENTRY_STATE` (turned off by the user 2026-10-08: follow June):

`BUFF_DEBUG`, `RIDINGPET_STATE_DEBUG`, `UNIT_SOUND_DEBUG`, `RIDINGPET_UI_FX_DEBUG`, `UPDOWN_CANCEL_DEBUG`, `CANCEL_CLAMP_DEBUG`, `NPC_SOUND_DEBUG`, `SQUARE_SOUND_DEBUG`, `EFFECT_SET_DELETE_DEBUG`, `SOUND_AUTO_RELEASE_DEBUG`, `RIDINGPET_USER_KEYS_DEBUG`, `FIX_RIDINGPET_ENTRY_STATE`, `FIX_RIDINGPET_ENTRY_STATE_DEBUG`, `USEFUL_VALUE_DEBUG`, `DAMAGEEFFECT_SOUND_DEBUG`, `C9H4G_DEBUG`, `BOUNDING_STATE_DEBUG`, `FLY_COUNT_DEBUG`, `NPC_CINEMATIC_UI_DEBUG`, `IS_VOTING_LUTO_DEBUG`, `C3Z4_DEBUG`, `SKIP_START_DEBUG`, `FORCE_DOWN_FLUSH_DEBUG`, `C3V_DEBUG`, `HIT_STOP_LISTS_DEBUG`, `SUPER_ARMOR_HIT_STOP_DEBUG`, `CUTIN_SHOW_OPTION_DEBUG`, `SHOW_BOSS_NAME_AT_START_DEBUG`, `SOUND_SET_PICK_DEBUG`, `SIMPLE_DLG_DEBUG`.

### June EffectSet_Description.lua adopted (june-effect C12, 2026-10-16)

Data only, no flag. data036 `EffectSet_Description.lua` is June's own bytecode (`luasrc.py adopt-june data036 EffectSet_Description.lua`,
`ljconv`; 25,370 B). It is the effect tool's description map: March's C++ opens no file of that name (`X2EffectSet.cpp:3380`,
`AddEffectSet_Description_LUA`, is `#ifdef EFFECT_TOOL`), so no in-game behaviour changes. Packed (exactly 1 entry changed vs the live kom)
and copied, `COPY OK` data036 md5 deb87a3a.

Revert: copy `24965799\kom_backup_stage_june\20261016_215227\data036_kom\EffectSet_Description.lua` (March's) back into `kom_contents\data036_kom\`,
`python tools/kom.py pack data036`, then ask for `copy`. Or restore the game dir's `data036.kom` from
`24965799\kom_backup_tools\data036.kom.20261016_215439`.

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

### Atlas/Elysion port A3: region enums, Lua + C++ (2026-10-17)

Flag `SERV_IRUHADEV_ELYSION_PORT` (`KTDXLIB/Always.h`; later Atlas/Elysion phases put their region switches under it too).
June's ids at June's values, in both halves, with June's end markers. Ids come from `june2017_re/atlas_a3_ids.tsv`;
`june2017_re/atlas_a3_gen.py` wrote the Lua rows and the bytepatch spec, `june2017_re/atlas_a3_check.py` is the Lua == C++ gate.

- **C++** (each block `#ifdef SERV_IRUHADEV_ELYSION_PORT`, explicit `= N`, before its sentinel): `X2World.h` `WORLD_ID` (+38, `WI_END` 779),
  `KncWX2Server/Common/Enum/Enum.h` `VILLAGE_MAP_ID` (+7; `REST_END` 1009, `VILLAGE_MAP_END` 20011, `BATTLE_FIELD_END` 40045) and
  `LOCAL_MAP_ID` (`LMI_ELYSION`), `DungeonEnum.h` (+21, `DI_END` 41530), `X2LocationManager.h` `HOUSE_ID` (+12) / `LOCAL_MAP_ID` and
  `IsVillageField` (June 20000-20010), `X2UnitManager.h` + `XSLUnitManager.h` `NPC_UNIT_ID` (+193; `NUI_MAX_LIMIT` implicit = 10196),
  `X2LocalMapUI.h` LMUCM 18006-18008, `X2OfflineLuaEnumSeed.h` (3 markers + 7 rows). The sentinels keep their original line in `#else`.
- **Lua** (staged `data036_kom/Enum.lua`, `DungeonEnum.lua`; BOM source, ledger refreshed): the same ids, plus `TAT_LINE_CHANGE_MOVABLE_DATA` 8,
  tables `LINE_MOVE_TYPE`, `DRAMATIC_MESSAGE_TYPE`, `VILLAGENPC_RELATIONDATA_TYPE` (June's values; their C++ comes with E5 / E6 / E1),
  `WI_END` 779, `VMI_BATTLE_FIELD_END` 40045. Needs `data036.kom` repacked and deployed together with the exe.
- **Not added** (quest plan, user 2026-10-16): `NUI_STORY_QUEST` 10142, `NUI_TRANSCEND_QUEST` 10153. `VMI_EN` 20010 has no March enum (June's range includes it).
- **Revert:** remove the `SERV_IRUHADEV_ELYSION_PORT` `#define` from `Always.h` (every block above is dead, the markers return to
  1008 / 20009 / 40033 / 752 / 41311 / 10191). The Lua rows are extra names nothing reads without the flag; to remove them too, `kom.py` the
  pre-A3 `Enum.lua` / `DungeonEnum.lua` out of the live `data036.kom` as it stood on 2026-10-16.

### Atlas/Elysion port A5: Atlas Station, rest area 1008 (2026-10-17)

Flag `SERV_IRUHADEV_ELYSION_PORT` (A3's). June's own functions, cited by address in each block:
- **C++:** `IsValidWarpField` (+1008, 20009, 1501), `LoadFieldImage` (+1008 / 20009 / 1501 art), `OpenFieldName` (picture 15 / 16 / 27 / 28 / 30
  + `SetDisableUnderWindow(false)`), `GetFieldMapDlg` / `UpdateNaviMap` / `ResetCameraMatrix` / `ResetMapPosAndSize` (Elysion cases),
  `WarpByCommand` (positions 15-18, 20), both `InitWarpList` / `CreateWarpDest` pairs (`X2StateMenu.cpp` Cobo, `X2UIInventory.cpp` warp NPC:
  June's whole bodies, March's in `#else`), and the `atlas` chat warp (also under `SERV_IRUHADEV_CHAT_WARP_DEBUG`). 1500 / 25000 omitted (no enum).
- **Lua / data (kom_contents):** BOM source edits to `DLG_Map_Village` (Village[1008], March key names), `DLG_Map_House` (1008 rows + `HI_YUNO`),
  `NPCTemplet` (`NUI_YUNO`), `WorldData` (WI 637), `MapData` (AddMapData + Cobo zone 17), `DLG_UI_World_Map_NEW` (June's Elysion block over the
  Lanox `99999` cloud). June's whole `DLG_Destination.lua`, `DLG_Destination_byButton.lua` (core row lifted), `DLG_UI_Field_Title.lua`,
  `DLG_UI_FIELD_MAP_Elysion_Rest_NEW.lua`, `UI_YUNO.lua`, the scene scripts and 130+ assets (data013/023/040/057/096/151/180-183).
  Server mirror `KncWX2Server/ServerResource/US/MapData.lua`: the same AddMapData + zone 17.
- **Revert:** remove the flag (the C++ falls back to March's bodies; Cobo then needs March's `DLG_Destination_byButton.lua` back, because March's
  `InitWarpList` dereferences `Village_1000` / `1002`: the sidecar is in `kom_backup_stage_june\20261017_012014`). Data: restore the
  koms from `24965799\kom_backup_tools\` stamped before 2026-10-17 01:30, or delete the commented A5 blocks in the BOM sources.

### Atlas/Elysion port A7: local map, Atlas half (2026-10-17)

Flag `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION`. June's own code, cited by address in each block:
- **C++:** `CX2LocalMapUI::UpdateDungeonButtons` June's Elysion page filter (new member `m_iLocalMapPage`, June +0x54, 0 until atlas E4)
  and the hidden `LocalMapMoveUp/Down` buttons (E4 ports the scroll); `UpdateLocalMapDLG` June's event-code 2260 branches (inert offline).
  Stand-ins until atlas E2 / E3 (user, 2026-10-17): `CX2LocationManager::VillageParsing` skips a village with no `Village[]` row with an
  `[ELYDBG]` ErrorLog line (no popup, table closed); the Cobo (`X2StateMenu.cpp`) and warp-NPC (`X2UIInventory.cpp`) lists skip
  `Village_20009` while 20009 has no village templet; `CX2WorldMapUI::UpdateWorldMap` keeps the Elysion town / Field 03 pins hidden
  while their village templet / battlefield data is missing.
- **Lua / data (kom_contents):** BOM source edits to data034 `DLG_Map_Local` (`Local[LMI_ELYSION]`), `DLG_Map_World_Elios` (`LocalIDList`),
  `DLG_UI_Local_Map_NEW` (June's `LocalMapRadioBtn9` + QuestGuide picture), data036 `MapData` (21 `AddLocalMapInfo` rows; also the server
  mirror). June's `DLG_Map_Local_Elysion.lua`, `_Back_NEW.lua`, `_Icon_NEW.lua` (ljconv bytecode), `DLG_New_Region_Elision_01.dds/.tet`
  (data065), `DLG_Common_Button06_NEW.TET/.tga` (data215).
- **Revert:** remove the flag (the C++ falls back to March's; with the Lua still in, the loader then pops "Village OpenScript Error" for 20009,
  so revert the data too): delete the commented A7 blocks in the four BOM sources and the 7 adopted files (all new names, nothing
  was replaced; manifests lose their keys), then repack data034 / 036 / 065 / 215 / 217.

### Atlas/Elysion port E4: local map, Elysion page (2026-10-17)

Same flag, `SERV_IRUHADEV_JUNE_LOCAL_MAP_ELYSION` (log lines behind `SERV_IRUHADEV_ELYSION_DEBUG`). June's own code, cited by address in each block:
- **C++ (`X2LocalMapUI.h/.cpp`):** the page scroll `ScrollLocalMap` (June 329378a0: `Elysion_Local` picture 0, A4's `StartChangeUV`, page written at the
  start), `HideLocalMapPageControls` (329366a0), the tween's end callback `OnLocalMapScrollEnd` (3293c1e0), LMUCM 18006 / 18007 / 18008 cases,
  `OnFrameMove`'s blinking move buttons (3293a960), the guide window `OpenElysionLocalMapGuide` (329386b0, once per install via `checkfile.txt`)
  from `OpenLocalMapDLG`, the guide in the dtor, `ClearLocalMapDLGs`' scroll reset and `m_bFirstPage = true` (329379d0), the code-event 2260 / 2261
  dungeon gates (`HideDungeonByElysionCodeEvent`, 3293b5e0 twice, inert offline), and `UpdateDungeonButtons`' tail re-sync (replaces A7's hidden
  buttons). Members June +0x48 / +0x4c / +0x50 / +0x58.
- **Lua (kom_contents):** June's `DLG_Elysion_LocalMap_Guide.lua` (data034, ljconv bytecode, new name). Its textures were already staged.
- **Revert:** remove the flag (it also takes A7 out), or keep A7 by restoring A7's tail block from commit `6b988787`; delete
  `DLG_Elysion_LocalMap_Guide.lua` from data034 (and its manifest key) and repack data034. `data\checkfile.txt` is June's marker file and can be deleted.

### Atlas/Elysion port E5: moving platforms (2026-10-17)

Flag `SERV_IRUHADEV_JUNE_MOVABLE_LINE` (the `[ELYDBG] movable line` line behind `SERV_IRUHADEV_ELYSION_DEBUG`). June's own code, cited by address in each block:
- **C++ (`KTDGLineMap.h/.cpp`, `KTDXApp.cpp`):** `LINE_MOVE_TYPE`, `TAT_LINE_CHANGE_MOVABLE_DATA = 8`, the `MovableLineData` record and map,
  `AddMovableLineData_LUA` (bound), the movers in `OnFrameMove`, the moving-line vertex refresh in `UpdateLineVB`, `CAMERA_LAND_HEIGHT`, trigger case 8
  parse, and `CanDown`'s June head (carry by the last move; the `CAN_DOWN = FALSE` test now runs before the radius drop on every map).
- **C++ (`X2World`, `X2Game`, `X2Main.cpp`, `X2UDPPacket.h`, `X2GUNPC.cpp`, `X2WorldObjectSkinMesh.h`):** the world ticks the line map, meshes follow
  their line, `DoTriggerLine` case 8, P2P message 28 `XPT_MOVABLE_LINE_SYNC` (host broadcast once a second, guest snap), `GetLineTypeByLineID_LUA`,
  the per-line camera land height at the end of `CX2Game::OnFrameMove`, the NPC hit-stop ride.
- **Lua / koms:** none (A3 staged the enums).
- **Revert:** remove the flag. Nothing else depends on it until E9 / E10 stage the Adrian / Soless line maps, which then load without platforms.

### Atlas/Elysion port A6: Lanox -> Atlas crossing + Fields 01/02 (2026-10-17)

Flags `SERV_IRUHADEV_ELYSION_PORT` (A3's) and `SERV_IRUHADEV_OFFLINE_BF_JOIN_START_POS` (new). June's own code, cited by address in each block:
- **C++ (`ELYSION_PORT`):**
  - `CX2BattleFieldGame::IsNearPortalLineMap` (June `32d53810`): the NULL world / line-map return, which cured the Field 01 crash; plus
    the code-event 2260-2262 gate.
  - `CreatePotalParticle` (`32d51be0`): the gate, a `SHOW_PORTAL` constant until june-engine W2, and June's second particle
    `UI_Portal_Arrow`.
  - `CX2GUUser::PhysicProcess_PortalInBattleField` (`3201a670`): the gate.
  - `CX2Main::FieldWarpByCommand` (`31d71130`): positions 27-30 (Ranox) and 35 / 38 (Fields 01 / 02).

  The gates are inert offline (no code events).
- **C++ (`OFFLINE_BF_JOIN_START_POS`):** the emulator's `EGS_JOIN_BATTLE_FIELD_ACK` echoes the request's start-position mode, as the live
  servers do. A village exit or portal now lands on its `AddStartPos` key, for every field, not on a line of that number.
- **Lua / data (kom_contents):**
  - BOM source edits:
    - data036 `BattleFieldData`: Lanox F04 portal 103 -> 1008, June's `BATTLE_FIELD_DIRECTION_INFO` (inert), and the F01 / F02 rows,
      with F02's village exit commented out until E2.
    - data037 `NPCTemplet`: 26 Elysion field-monster rows.
    - data040 `WorldData`: WI 638 / 639.
    - data034 `DLG_UI_FIELD_MAP_Ranox_04_NEW`: June's right button.
  - June's own files (ljconv): `Field_Elysion_Stage0` / `stage1` + line maps, `Elysion_Field_01/02`,
    `DLG_UI_FIELD_MAP_Elysion_01/02_NEW`.
  - 338 June assets (data046 / 078 / 096 / 179 (new) / 180-182).
  - 3 loose music files copied into `data\music`.
- **Revert:**
  - Remove either flag. The emulator then sends the old zeroed join info; the client falls back to March's portal functions. With the
    data still in, Fields 01 / 02 still load.
  - Data: delete the commented A6 blocks in the four BOM sources and the adopted files (all new names; record
    `june2017_re/june_adopt_20261017_060423.tsv`, asset backups under `kom_backup_stage_june\20261017_060810`), then repack the
    11 koms.

### Atlas/Elysion port A4: Atlas-half script engine (2026-10-17)

Flags `SERV_IRUHADEV_ELYSION_SCRIPT_A` (new) and `SERV_IRUHADEV_ELYSION_DEBUG` (new, `[ELYDBG]`). June's own code, cited by address in each
block; 19 `funcmap.tsv` rows tagged `Atlas A4`:
- **Binds:** `GetAliveActiveNPCCount` (CX2Game, `322d0ae0`), `GetNationFlag` (CX2Main, `31e4e690`).
- **`SetChangeUVPoint`:** June's UV-change tween on `CPictureData` (`31c9a7d0`, `31c95150`, `31c95a60`, `31c95580`, `31c9eea0`) plus the
  `CKTDGUIStatic` frame-move gate (`31cc0950`). Its only caller is the Elysion local-map scroll (atlas E4).
- **NPC keys:** `USE_DEFAULT_DIE_PARTICLE` ("DieLight" once at death, `3277ec60`), `NOT_CHECK_INDICATOR` / `INIT_AI NO_INDICATOR`
  (indicator filter `32f09f05`; AI `+0x34`).
- **State key `CAN_NOT_PASS_SPECIFIED_BONE`:** bone list, `HasCanNotPassSpecifiedBone`, the player push loop (`32010060`), and the bone form of
  `ProcessCanPushUnit` (`31fe8ad0`).
- **Line-map key `CHANGE_LINE_TYPE_AFTER_ALL_ACTIVE_NPC_KILLED`:** line `+0x150` and the retype twin (`327efc90`), run from
  `CheckIfSubStageCleared`.
- **Guards June lacks** (on paths June data never takes): NULL texture in the UV tween, direction > 2 refused; `SHOW_PORTAL` a `true` constant
  until june-engine W2. JUNE_BUGS 205.
- **A13 diagnostic (`ELYSION_DEBUG`):** `CX2World::SetCameraDistance_ELYDBG` is bound as Lua `SetCameraDistance` (the plain binding, from
  june-effect C3z-4b `SERV_IRUHADEV_JUNE_WORLD_CAMERA_DISTANCE`, stays in `#else`). It logs `[ELYDBG] SetCameraDistance call: ...` and then
  calls the setter. It proves Lanox `SECRET_SCENE_DUMMY_SUMMON_TYPE3_STATE_START` reaches C++.
- **Revert:** undefine `ELYSION_DEBUG` to silence every `[ELYDBG]` line (no behaviour change). Undefining `ELYSION_SCRIPT_A` drops the keys and
  binds: June NPC scripts that call `GetAliveActiveNPCCount`, `GetNationFlag` or `SetChangeUVPoint` then stop at the nil method, and the four
  keys are read as unknown. Revert it only together with the Atlas dungeon data (A8-A10).

### Atlas/Elysion port A8-A11: AtlasDevelopment, AtlasCity, Tower, asset closure (2026-10-17)

Data only, no flag and no C++. June's own files are ljconv bytecode; registry rows in BOM sources are copied verbatim from June and marked by phase.
- **A8-A10:** dungeon scripts, line maps and NPC scripts adopted into data040; `DungeonData` (N/H/E rows, Hero rows omitted -> june-dungeons C3y-5),
  `NPCTemplet` and `WorldData` rows; quest key 22218 commented (quest plan). JUNE_BUGS 209 (`WI_TOWER_STAGE3` names a file June ships nowhere).
- **A11:** 28 missing `.TET` companions staged (data057 / 151 / 215); script `june2017_re/atlas_a11_closure.py` and its `atlas_a11_*.tsv`.
- **Revert:** delete the phase's commented blocks in the BOM sources (`DungeonData`, `NPCTemplet`, `WorldData`) and the adopted files named in each
  phase's `june_adopt_*.tsv` record, then repack the koms the Done note lists. Each phase's Done note in `ATLAS_PORT_PHASES.md` lists them.

### Atlas/Elysion port E1a-E1d: village engine (2026-10-17)

Flag `SERV_IRUHADEV_ELYSION_VILLAGE` (`[ELYDBG]` lines behind `SERV_IRUHADEV_ELYSION_DEBUG`). Closed by E15.
- **E1a:** June's whole `HouseParsing` (`324f3260`; ~50 June-only house keys + 6 sub-tables into new `HouseTemplet` members), and the
  village NPC relation data in `CX2QuestManager` (loaders `3233b680` / `32340720`, lookups, `VILLAGENPC_RELATIONDATA_TYPE`, loaded from
  `X2StateLoading.cpp` as June does). Data: June's `DLG_Map_House_NPC_RelationData.lua` (data034, ljconv). JUNE_BUGS 207, 208.
- **E1b:** June's `CX2TFieldNpc` whole except `OnFrameMove` (ctor / dtor, `CreateFieldNpc` `325ca980`, talk / bye steps, show / enable /
  reset, `OnFrameRender_Draw`, `RenderName`, and from the E1b review `ResetNpcType`, `GetHouseType`, `InitQuestMark`), `CX2UIUnitManager`
  `GetUIUnit` / `CreateUIUnit` / `ClearUIUnit` by file, `CX2TFieldNpcShop` 4-arg `SetShopType` (June-only buttons registered but not shown,
  `[ELYDBG] NpcShop` line). The June bodies are in one flagged block at the end of `X2TFieldNpc.cpp`, March's in `#else`. JUNE_BUGS 210-213.
  Visible in March villages: talk / bye motions `Talk` / `Bye` with a `Normal` fallback, JoinTalk sound, names drawn to 2700, March's
  synthesis / web-event / wedding buttons gone (June dropped them).
- **E1c (commit `8ee92c45`):** June's village NPC creation `323ba5f0` (ByQuest, then ByVillage relation lookup), `AddFieldNpc( templet, name )`
  `323b0980`, replace `323b7ff0`, remove `323a9ed0`, restore `323b8970`; the `CX2TFieldGame` NPC loop (`323b5970`: AlwaysShow, effect-set show,
  quest info once a second); both refresh entry points (code event `32344880`, quest complete `32340630`, called from `X2UIQuestNew.cpp`'s
  quest-complete ACKs); `CX2TFieldNpc::OnFrameMove` `325cdfa0` whole; `PlayEffectSet( name, CX2TFieldNpc* )` `321e26d0`. Data: June's postbox /
  billboard meshes and textures (data013) and `UI_NPC_BILLBOARD_ELYSION` / `UI_BILLBOARD_ELYSION` / `UI_NPC_BILLBOARD_SPA` (data039, record
  `june2017_re/june_adopt_20261017_092513.tsv`). JUNE_BUGS 218-225.
- **E1d (commit `292ff32e`):** June's `VillageParsing` `324ff4b0` whole (`CameraModifyDistanceByPortal`, `SPA_INFO`, `START_POS` `IsGuildAgitEnter` /
  `WaitingTime` / `InternalPlaceStartPosID` / `IsSpaEnter` / `IsSpaOut`), the TFieldGame portal block, `MoveToOtherPlace` `3250a800`,
  `MoveToInternalPlace` `323ac300`, `IsInternalPlace` `323aa170`, timer `323aa1b0`, `GetVecLinkedPos` `324f2ff0`. Visible in March villages: Lanox's
  spa entrance (row 146) is no longer a portal (June; spa = backlog Stage 24). JUNE_BUGS 216-217.
- **Revert:** undefine the flag. March's house parser, field-NPC bodies, village NPC creation and `VillageParsing` return; the relation `.lua` and
  the E1c data stay in their koms, unread (delete them per the record and repack data013 / data034 / data039 for a full revert).

### Atlas/Elysion port E2-E3: Elysion village 20009 + Field 03 (2026-10-17)

Flag `SERV_IRUHADEV_ELYSION_PORT` (A3's). Nearly all data:
- **C++:** the `elysion` chat warp (`X2ChatBox.cpp`, also under `SERV_IRUHADEV_CHAT_WARP_DEBUG`); `CX2Main::FieldWarpByCommand` `case 42` ->
  `VMI_BATTLE_FIELD_ELYSION_FIELD_03` (June's table @`31d71550`, commit `22d66551`). `case 43` (Field 04) is left out: June has F04 as an enum only.
- **Lua (BOM source, core list, blocks marked `atlas E2` / `atlas E3`, copied from June):** `DLG_Map_Village` `Village[VMI_ELYSION]` (6 in-village
  warp pairs, 517 -> F03), `DLG_Map_House` (11 houses, Elysion rows in 7 shared houses, June's whole `HI_ADAMS_HOUSE`: Lanox's Adams moves and
  loses make / attribute / shop, as June; `HI_PRIVATE_BANK` JoinArea 700, `HI_CAMILLA_PVP_HOUSE` 500), `NPCTemplet` 11 rows, `WorldData` 636 / 640,
  `MapData` 20009 + Cobo zone 18 (+ server mirror `ServerResource/US/MapData.lua`), `BattleFieldData` F02 exit -> 500 and the F03 entry,
  `DLG_UI_World_Map_NEW` (town + F03 pins shown), `ShopItem` (converted T0 + June's `VMI_ELYSION` block).
- **June's own files (ljconv):** village scene + line map, F03 scene + line map, `Elysion_Field_03`, both field-map dialogs, 12 NPC dialogs
  (records `june_adopt_20261017_103941.tsv`, `_111022.tsv`); loading images, textures, `DLG_UI_Common_Texture_Field_04` (data057).
  JUNE_BUGS 226, 227 (the world map opens at the top-left, as June's does).
- **Revert:** undefine the flag (with A3's enums it takes the whole region out); for the data, delete the `atlas E2` / `atlas E3` blocks and the
  adopted files named in the records, then repack data002 / 013 / 034 / 036 / 037 / 039 / 040 / 057 / 181 / 183.

### Atlas/Elysion port E6: dramatic messages (2026-10-17)

Flag `SERV_IRUHADEV_JUNE_DRAMATIC_MESSAGE` (`[ELYDBG] dramatic message:` behind `ELYSION_DEBUG`). SimpleDLG gauges were already ported by
june-effect C3z-4s (`SERV_IRUHADEV_JUNE_SIMPLE_DLG`); E6 only staged the three Soless / Memory gauge dialogs (data034, ljconv).
- **C++:** loader `322e9e00` (from `CX2GUNPC::CommonStateStart`), remove `322e61c0` / `322e6270` (CommonStateEnd / NPC dtor), player `32306110`, Lua
  `Playing_Dramatic_Message_LUA`, the second talk-box manager `TBMT_DRAMATIC_MESSAGE` = `CX2TalkBoxManagerImpDramaticMessage` (Push `32f218c0`,
  `SetBalloonTip`, `LineBreak`), drawn after the fade; portrait particle / slide-up / spread. The review made Push run the US global line breaker
  (`CLIENT_GLOBAL_LINEBREAK`, max 200), as June US does; June's own `LineBreak` stays in `#else`. RE in `june2017_re/e6_dramatic_re.md`; JUNE_BUGS 237-238.
- **Revert:** undefine the flag. Soless / Memory NPCs then skip their dramatic boxes; nothing else uses it.

### Atlas/Elysion port E7: Elysion-half script engine (2026-10-17)

Flag `SERV_IRUHADEV_ELYSION_SCRIPT_B` (commit `0cc43f29`; 25 `funcmap.tsv` rows tagged `Atlas E7`, all 39 E7 rows of `region_engine.tsv`):
- **Additional world:** WorldData `dataFileName_Additional` (`32805770`), hidden worlds created after the stage world, moved / drawn while shown,
  `SetShowWorld` / `SetShowAdditionalWorld` / `PlayWorldAllSound` / `PlayAdditionalWorldAllSound`, June `StopBGM`. Freed in `CX2Game::Release` before
  the camera, as June `323080c0` does (E10 play-test 2 fix: freeing it after the camera crashed the result screen).
- **Game / unit queries and effects:** random users in range, nearest unit `322d55b0`, user index; colour fades (`CKTDGManager`; a white flash uses
  June's `HQ_WhiteBack.dds` quad), thorns + HP text (HP test only when +0x75e0 is 1, review fix), simple timer, small-gage hide, sync dummy (host
  only), `StartAlphaFade`, stored NPC, state cool time, `VectorUidType`, `IsSceneEnded`, `SetParticleSize`, BlackHole accessors, `StopEffectSetSilent`.
- **Keys:** `CAN_PASS_UNIT_AT_FIRST_FRAME`, `SHOW_GAGE`, `INVINCIBLE_ATTACK_TYPE` (each entry a sub-table, `[1]` read, E10 play-test 3 fix),
  `HEIGHT_CHECK_RANGE`. JUNE_BUGS 234-236.
- **Revert:** undefine the flag, only together with the Elysion dungeon data (E8-E11): their NPC scripts call these methods and stop at the nil.

### Atlas/Elysion port E8-E13: SkyCrossroads, Adrian, Soless, Memory, PvP map, asset closure (2026-10-17)

Mostly data. June's own files are ljconv bytecode; registry rows in BOM sources are June's verbatim, marked `atlas E8` ... `atlas E12`.
- **E8-E11 (data035 `DungeonData` N/H/E rows, data037 `NPCTemplet`, data040 `WorldData` + stage / line-map / NPC scripts):** SkyCrossroads 33870-72,
  Adrian 33880-82, Soless 33890-92 (+ hidden stage, phase-2 additional world), Memory 33900-02; the unlock chain is Normal-keyed (Tower -> SC -> AR
  -> SL -> ME), no quest key in June. Hero rows omitted (june-dungeons C3y-5). Records `june_adopt_20261017_143715` / `_145520` / `_150655` /
  `_172712.tsv`; mesh-internal textures staged after the E10 / E11 play-tests (data002 / 016 / 050 / 181). JUNE_BUGS 239-247.
- **E10 C++ (port gaps found by its play-tests):**
  - `SERV_IRUHADEV_ELYSION_MOVABLE_LINE_END` (`KTDGLineMap.cpp`, commit `30b389d6`): a moving platform's last move is zeroed when it stops, so
    the player no longer sinks after the ride (June's code keeps it, June's game does not show it; JUNE_BUGS 230 fixed).
  - `SERV_IRUHADEV_ELYSION_INVINCIBLE_ATTACK_TYPE` (`X2GUNPC.h/.cpp`, commit `1077e562`): June's state-start list (`327ad91e`, slot 95 `3275bbf0`) makes
    the Stage3 seals take only their attack type. JUNE_BUGS 246.
  - `SERV_IRUHADEV_ELYSION_ATTACK_TYPE_STANDIN` (`X2GUNPC.h`, inside the flag above): **not June**, user's choice. March's class combos are untagged
    `AT_NORMAL`, June tags them `AT_COMMAND`, so a user's untagged hit counts as `AT_COMMAND`. Remove it with the class ports (checklist 7x).
  - `ELYSION_DEBUG`: the `LoadReadyNPC` hole log (`X2DungeonStage.cpp`).
- **E12:** PvP map `PVP_Elysion_Field` + line map (data040), June's `PVP_Map_List` (data036) and `DLG_PVP_Map_List` (data034), `WorldData` 752,
  `DLG_New_PVP_Image09` / `_TITLE` TETs. Load-only (no PvP offline). June's `m_PrivateType` map filter is parked as E12b. JUNE_BUGS 248.
- **E13:** 46 + 5 June files that the relation and list data name (data013 / 039 / 065 / 096 / 181 / 215); scripts `june2017_re/atlas_e13_closure.py`,
  `atlas_e13_sweep.py`. JUNE_BUGS 249-250.
- **Revert:** undefine `ELYSION_ATTACK_TYPE_STANDIN` alone to restore June's strict rule (the white seal then breaks only to tagged hits);
  `ELYSION_INVINCIBLE_ATTACK_TYPE` / `ELYSION_MOVABLE_LINE_END` each alone bring back the bug they fixed. For the data, delete the phase blocks and
  the files named in each `june_adopt_*.tsv` record, then repack the koms each Done note in `ATLAS_PORT_PHASES.md` lists.

### June skill templet loader (skill-system-core S2, 2026-10-18)

Flag `SERV_IRUHADEV_JUNE_SKILL_TEMPLET` (+ short-lived `SERV_IRUHADEV_JUNE_SKILL_DEBUG`), plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S2". **The data is still March's**
(`NewSkillTempletVer2.lua`, `SkillData.lua`); S3 flips it. What changes with the flag on:
- **Loader (June `32216320`, whole).** `CX2SkillTree::AddSkillTemplet_JuneLUA` replaces the body of `AddSkillTemplet_LUA`. Differences from March, each deliberate:
  - `m_SkillCoolTime`, `m_MPConsumption` and `m_RequireCharacterLevel` are **not padded to 30** (March repeated the head of the array). A level past the end reads 0.
  - An **empty `SA_*` table inserts nothing** (March inserted `{ 0.f }`).
  - `IN_HOUSE_TEST_ONLY` drops the templet. A duplicate `m_eID` is not added and the call still returns true (first wins).
  - New fields, stored: `m_DeleteSkillIDByUpgrade` (also fills the upgrade map base -> "[Enhanced]"), `NUMERICAL_DATA<n>`, `m_ExtraIconInfo<n>`, SA ids 143-202, `SPRT_HYPER_CLASS` = 5.
  - `31c59710` (anti-tamper registration) is not ported.
- **Kept March behaviour, on purpose** (S2 only): `AddSkillTreeTemplet_LUA` still reads `m_iPrecedingSkill` (June dropped the read; June's data carries it on 0 unit rows,
  so on June data it is a no-op, and on March data dropping it would remove every prerequisite). `AddSealSkillInfo` reads `m_iItemIDList` and falls back to the scalar
  `m_iItemID` when the list is absent (June's data only has lists). S3 removes both fallbacks when it flips the data.
- **Lookups.** `GetUpgradedSkillID` (`32205c00`), `IsUpgradedSkill` (`32206d00`), `GetUpgradeBaseSkillID` (`32207210`); `CX2UserSkillTree::GetUpgradeSwappedSkillID` (`328905b0`) and
  its use as a trailing `bUpgradeSwap_` argument of `GetSkillLevel` / `GetUserSkillTemplet` / `CX2GUUser::GetEquippedActiveSkillTemplet` (defaults false = March's calls).
  `SkillTemplet::GetSkillPowerRateValue` reads SA `0xad` first for an upgraded skill (`32071720`); `GetSkillAbilityValue` reads a `NUMERICAL_DATA` block named after the ability first
  (`32206e50` / `32206fb0`, `JuneTryGetSkillAbilityValue`, the 3-argument overload carries the PvP flag).
- **The three `_ULTIMATE` gates are live** (they were constant false): `SPEED_TIME_ULTIMATE` (`CommonStateStartFuture`), `BUFF_FACTOR_ID_ULTIMATE` (`ParsingBuffFactorID`),
  and the DamageEffect `_ULTIMATE` keys (`m_bUseUltimateDamageData`). They stay off until a templet with `m_DeleteSkillIDByUpgrade` exists, i.e. until S3.
- **S2 review fixes (same flag):** `JuneTryGetSkillAbilityValue` returns false past the end of an SA table, as June `32206fb0` does, so an upgraded skill's power rate falls back
  to `m_vecPowerRate`; `AddSkillPowerRate_LUA` reads `m_Value` without padding (June `32209d80`; every row in both data sets has 30 entries).
- **Revert:** comment out `SERV_IRUHADEV_JUNE_SKILL_TEMPLET` (and `_DEBUG`) in `KTDXLIB/Always.h`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data, save or kom changes.

### June numerical skill data override (skill-system-core S4, 2026-10-18)

Flag `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL` (+ short-lived `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL_DEBUG`), plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S4", log tag `[JUNE_SKILL] NUM`.
- **What it ports.** June's override at the end of `SetDamageDataFromLUA` (`FUN_32632390 @3263fe41-32640054`), as `JuneApplyNumericalSkillData` in `X2DamageManager.cpp`, called
  after every key (so after the `DAMAGE_DATA_KEY_NAME` record) and before `EndTable` / `SetDamageExceptionProcess`. It replaces the "unsupported" log for `NUMERICAL_DATA_INDEX` / `_NAME`.
  - Gate (any miss skips it all): INDEX > 0 or NAME set; `APPLIED_SKILL_ID` or the new `DAMAGE_SKILL_ID` (`m_iDamageSkillID`, +0x5cc, parsed with default 0) non-zero; skill tree present;
    attacker valid; unit type user (itself) or NPC (its owner); then level `GetSkillLevel( id, 0, 0, swap )` > 0 and `GetEquippedActiveSkillTemplet( id, swap )` non-null.
  - Row: INDEX > 0 gives `JuneGetNumericalRowByIndex` (`320717e0`, 1-based, out of range = zeros), else `JuneGetNumericalRowByName` (`32078060`, first name match, none = zeros); the clamp is S2's
    `JuneGetNumericalRow` (`320716c0`).
  - Writes, unconditional (a zero row zeroes all five): `PHYSIC = value * 0.01f` unless `DT_MAGIC`; `MAGIC = value * 0.01f` unless `DT_PHYSIC`; `PVP_RATE`; `TECH_POINT` (truncated); `HIT_ADD_MP`.
    `SetDamageExceptionProcess` then scales PHYSIC / MAGIC by the power rate, so the override is scaled too.
- **Values that change.** None in today's game. June's `DamageEffect.lua` / `EffectSet.lua` are live (C11a), so the keys are parsed and the override would run. But June's 1,913 blocks
  (`june2017_re/c1a_numerical_blocks.tsv`: file, templet, skill, level-1 value / x0.01 / pvp rate / tech point / hit add MP) all name reform skill IDs, and S3 refuses presses of a reform skill
  that has no March class state (`[JUNE_SKILL] MIG no-state skill ...`). They reach this code once a ported class (Stage 5/7) has the states (S4 review, 2026-10-18).
  Until then the "deal 0" note under "June partial ports replaced" holds. Afterwards a learned skill's blocks deal June's numbers. When they do, June's number replaces March's:
  June's damage scale differs, e.g. Aceldama (26002) 9.50 at level 1 vs March's 2.33, Aiming Shot (26203) 14.82 vs March's 5.63 (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 1).
  Three blocks index past their skill's data and write a zero row (JUNE_BUGS 282-284); the owner's `CX2Unit` is unguarded as in June (JUNE_BUGS 285).
- **June-side power-rate note.** `GetSkillPowerRateValue 32071720` (S2) gives an upgraded skill (`m_DeleteSkillIDByUpgrade` non-zero) its power rate from SA `0xad`
  (`SA_ULTIMATE_STRENGTHENED_SKILL_POWERRATE`), so the damage of an "[Enhanced]" skill carries that factor on top of the numerical row.
- **Self-check (`_DEBUG`).** At skill-tree load `CX2SkillTree::JuneNumericalSelfCheck` compares 40 rows of `c1a_numerical_blocks.tsv` (level 1; every 36th distinct skill / index plus the three zero rows)
  with the templets and logs `[JUNE_SKILL] NUM selfcheck: sample=40 ok=.. mismatch=.. no_templet=.. PASS`. Each distinct real use logs `[JUNE_SKILL] NUM use ...` (first 80); a refused use logs
  `[JUNE_SKILL] NUM skip ...`.
- **Revert:** comment out `SERV_IRUHADEV_JUNE_SKILL_NUMERICAL` (and `_DEBUG`) in `KTDXLIB/Always.h`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data, save or kom changes.

### June skill memo consumers (skill-system-core S3d, 2026-10-18)

Flags `SERV_IRUHADEV_JUNE_SKILL_MEMO_FX`, `_MEMO_DAMAGE`, `_MEMO_BUFF` (needs `_MEMO_DAMAGE`), `_MEMO_MP`, `_MEMO_STATE`, all in `KTDXLIB/Always.h` and all needing S3b's
`SERV_IRUHADEV_JUNE_SKILL_NOTE`; plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S3d"; log tag `[JUNE_SKILL] MEMO` (with `SERV_IRUHADEV_JUNE_SKILL_DEBUG`, throttled per site).
- **What changes.** Nothing until a memo is active (S7 activates them). Every path then multiplies by 1.0 or is skipped. With a memo:
  - `_MEMO_FX`: MI_HUGE scales effect sets with `APPLIED_SKILL_ID`, through the templet, which is never reset (JUNE_BUGS 289). It also scales the user's attack boxes during a
    timed DamageCheck (`JuneMultiplyAttackBoxRadiusScale`). The DamageData fields `+0x5d0` / `+0x5dc` reach the CEffect ctor (1.0 on June's flow, JUNE_BUGS 290).
    `CEffect::SetScale` now multiplies by a per-instance base (1,1,1 by default, so every existing call is unchanged).
  - `_MEMO_DAMAGE`: MI_BRUTAL sets NO_DETONATION. MI_CHECK_AND_KILL rescales the DamageData's buff factors (`32089ff0`, JUNE_BUGS 291). The tail scales the level-damage power rate (`+0x5e4`).
  - `_MEMO_BUFF`: slot 476 rescales skill buff factors (types 0xf / 0x10) from `SetBuffFactorToGameUnit( skill, index )` and `...AtLevel`.
  - `_MEMO_MP`: the MP cost uses the transcendence templet (read from my unit, JUNE_BUGS 292) and the MI_LIGHT / BRILLIANT / CRITICAL rate (vslot 471).
  - `_MEMO_STATE`: CommonStateStart sets the state string only on skill-slot states, with the transcendence skill's name.
- **Revert:** comment out the flag(s) in `KTDXLIB/Always.h` (`_MEMO_BUFF` before `_MEMO_DAMAGE`), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`.
  Each `#else` restores the C8b / buff-port stub it replaced. No data, save or kom changes.

### June skill slots 5 + 5 (skill-system-core S5, 2026-10-18)

Flag `SERV_IRUHADEV_JUNE_SKILL_SLOT_10` (+ short-lived `SERV_IRUHADEV_JUNE_SKILL_SLOT_10_DEBUG`), defined in `KTDXLIB/Always.h` **and** `KncWX2Server/Common/ServerDefine.h` (`SECOND_HOMES` in `tools/flagcheck.py`), plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S5", log tag `[JUNE_SKILL] SLOT` / `MIG`.
- **Wire (`CommonPacket.h/.cpp`).** `EQUIPPED_SKILL_SLOT_COUNT` 5 (static member and the `#define`). `KSkillData` is 7 bytes (id, level, June-only `m_iJuneSkillDataExtra`, meaning unmapped, the emulator sends 0).
  `KUnitSkillData` follows June `320e3670`: `A[0..4]`, `B[0..4]`, `m_vecJuneSkillSlotState` (replaces the B end date / expiration state), passives, guild passives, notes, the two page bytes, then the
  June-only `KJuneSkillExtraData` (`320dc980`, sent empty). `KUserSkillPageData` has 5 + 5.
- **Slot ids.** A 0-3, A-TC 4, B 5-8, B-TC 9 (`CX2UserSkillTree::SKILL_SLOT_*`, `GetSlotIndexAndSlotB`, the gage list A `i`, B `i + 5`). The **pressed-index space** the class files use (A 0-3, B 4-7, `-4` offsets in ~35 class
  sites) is left as it was: only the transcendence (TC) slots have no pressed index, and they stay dormant. The in-game HUD and the tree dialog keep their 4 + 4 controls (loops bounded 8 / 4); S6 owns the 10-slot dialog.
- **Slot state (`CX2UserSkillTree`).** `m_mapJuneSlotState` key -> { state, date } (June `+0x434`, `32892a00` / `32890ea0` / `32890ee0`): key 0 gates B 5-8, key 1 gates B-TC 9; state 1 / 2 = usable.
  `GetEnabledSkillSlotB()` = `IsJuneSlotUsable( 0 )`. The legacy B-expiry setters write key 0. **`SERV_IRUHADEV_SKILL_SLOT_B_FREE` is retired under this flag** (four client sites); the emulator sends key 0 = 2 (permanent), key 1 = 0.
  The TC slots: June locks A-TC on `CX2Unit +0x3c` (transcendence) and B-TC on slot-state key 1. March has no such flag and the emulator sends key 1 = 0, so both stay locked until Stage 6: the emulator refuses an equip into 4 / 9
  and the unit state code skips the fifth slot of a bar. Press path: `CommonSpecialAttackEventProcess` consumes an F press (index 4, no lock check, as June) and a T press only while key 1 is usable; neither selects a skill.
- **States.** `USI_SPECIAL_ATTACK_5` / `_HYPER_5` after the A4 pair and `USI_SPECIAL_ATTACK_SLOTB_5` / `_HYPER_5` after the B4 pair (June `GUSI_END + 53/54` and `63/64`); every later `USI_*` shifts by 2 / 4. Nothing uses numeric USI ids.
  `_CONST_GUUSER_::aNormalSkillStateID*` have 5 entries. Unit state code never sees a TC slot (`ChangeEquippedSkillState`, `SetEquippedSkillFunc*` skip it).
- **Keys.** `GAMEACTION_SKILL5` / `GAMEACTION_SKILL_EX5` (default F / T; Community moves T -> 0, as June `@31c48b2a`), `InputData` groups `*Skill5` / `*SkillEx5`, fed in `CX2Game::KeyProcess` (swapped with the bars), chat key lock. The key map is not persisted
  offline, so the enum insert renumbers nothing saved. F is also the March riding key: both fire on F (TC is locked, so only riding acts).
- **Types.** `GetEquippedSkillSlotType` returns 3 for slots 4 / 9 (June `3214f3d0`): BBT 98's reader and BFT 16's slot-type use count see it; `ApplyResetCoolTime` gets slot type 3 for the TC index.
- **Emulator + save.** `FillUnitSkillData` sends the state vector; `ChangeSkillSlot` refuses 4 / 9; seeds stop before the TC slot. **Save schema v18** (`MigrateSkillSlotsToTen`): `unit_skill.slot` 4-7 -> 5-8, one `[JUNE_SKILL] MIG unit .. skill .. slot a -> b` line per row.
  Forward-only: an exe without the flag refuses the save ("save is schema v18"). Snapshot to `db_backup/` before the first launch.
- **Revert:** comment out `SERV_IRUHADEV_JUNE_SKILL_SLOT_10` (and `_DEBUG`) in `KTDXLIB/Always.h` **and** `KncWX2Server/Common/ServerDefine.h`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`, and put back the pre-S5 save snapshot (`db_backup/*pre_S5*`): the flagless exe refuses a v18 save.

### June 10-slot skill dialog (skill-system-core S6, 2026-10-18)

Flag `SERV_IRUHADEV_JUNE_SKILL_SLOT_UI`, defined in `KTDXLIB/Always.h` after `SERV_IRUHADEV_JUNE_SKILL_SLOT_10` (an `#error` guards the dependency), plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S6". Log tag `[JUNE_SKILL] UI`
- **Data (the exe and the koms move together).** data034 gets June's `DLG_GAME_SKILL_SLOT_NEW_10slot.lua` (`ljconv` bytecode, not on the core list). data036 `Enum.lua` (core list, BOM source, ledger `refresh`) gets
  `SKILL_TREE_UI_CUSTOM_MSG.STUCM_MEMORIZE_SKILL_SLOT_MOUSEIN` = 29059, `STATE_MENU_UI_CUSTOM_MSG.SMUCM_DISABLED_TRANSCENDENTAL_SKILL_SLOT_MOUSE_UP` / `_EXPAND_SKILL_SLOT_MOUSE_UP` = **208 / 209** (June's 182 / 183 are
  March's `SMUCM_WARP_DEST_OK` / `SMUCM_USE_SPIRIT_REWARD_RAGE`; `X2StateMenu.h` carries the same numbers), `GAGE_UI_CUSTOM_MSG.STUCM_CHARACTER_SYSTEM_TOOLTIP_ON` / `_OFF` = 54002 / 54003 and the table
  `UI_TOOLTIP_TYPE` (June's 3 members). data056 + data057: June's `DLG_UI_Button24_NEW`, `DLG_UI_Common_Texture106`, `DLG_UI_Common_Texture92_NEW` (new) and `DLG_UI_Common_Texture63_NEW` (replaced: June's tet is a
  superset, no March key is lost). The other shared sheets the dialog uses (`Texture20`, `Texture64_NEW`, `Texture65_NEW`) already carry every key it names, so they wait for the asset port (June's `Texture64_NEW` drops `EXP`).
- **`CX2UISkillTreeNew` (`X2UISkillTreeNew.cpp`).** The constructor loads the 10-slot dialog. `GetEquippedSlotData` / `GetEquippedSlot` / `InitEquippedSkillSlotData` know `SlotA5` / `SlotB5` (combined ids A 0-4, B 5-9,
  June `328da440`). Every 8-wide loop (equip-slot data, `UpdateSkillCoolTime`, the swap offsets, 10 sites) is 10 wide with `i > 4` / `i - 5`. New `UpdateJuneSkillSlotLockUI` = the lock half of June `328dfab0`: A-TC and B-TC and B
  overlays (button + lock picture), the B key labels' alpha 1 / 0.5, the cool-time text and fade of a locked slot cleared, a skill sitting in a locked slot unequipped, `m_bIsExpandSkillSlot` from slot-state key 0, the Rose glow
  hidden outside a game. A-TC stays locked (`CX2Unit +0x3c` has no March counterpart: Stage 6), B-TC follows key 1 (the emulator sends 0), B follows key 0 (sends 2). `SetOffsetPosJuneTCSlotUI` = the TC placement in
  `328daed0` / `328db100` (type A +-47, type B -200 / 48, guide-text offset 100). The slot pitch is 47 (51 - 4) in the swap offsets, the type B dialog is at (-257, -619) (June `328e1680`; June has no wide-bar y, user 2026-10-18),
  `UpdateSkillSlotString` writes five labels a bar (`GAMEACTION_SKILL5` / `_EX5`), `SetOffsetPosSkillSLotStatic` moves the Rose glow statics.
- **`CX2Game` (`X2Game.cpp`).** The nine 4+4 loops S5 left (`UpdateSkillCoolTime`, `InitExpandSkillCoolTime`, the Elsword / Eve / Night Watcher / Veteran Commander / Code Empress / Add class slot-icon updates) are 5 + 5 / 10 wide.
  The recharge particle of the type B B bar sits 47 (was 51) below its picture (inferred from the swap offset: June's game-side cool-time function is not mapped).
- **`X2StateMenu`.** `SMUCM_DISABLED_TRANSCENDENTAL_*_MOUSE_UP` (a click on a locked TC slot) log one `[JUNE_SKILL] UI click on a locked transcendence slot` line and return: June opens the transcendence guide, March has none.
- **Not ported (routed):** the key-config dialog's rows/icons for `GAMEACTION_SKILL5` / `_EX5` (`X2KeyPad` `UpdateUI`, `X2StateOption::MappingKeyConfigIndex`); the Install / G-Core slot statics (`328e0290`) and the
  Eve / Luciel / Ain / Aisha-memorize statics (class material, hidden by the dialog); the tail of `SetSkillUIType` that moves one `CX2Game` dialog (`+0x8f70`) with the type; June's `SetOffsetPosSkillSLotStatic` TC-lock offsets behind
  the unit-map entry 0xb flag (March has no such map: the TC placement is done by the swap functions instead).
- **Revert:** comment out `SERV_IRUHADEV_JUNE_SKILL_SLOT_UI` in `KTDXLIB/Always.h`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy` (the exe then loads the 4+4 `DLG_GAME_SKILL_SLOT_NEW.lua` again, which still sits in data034). The data034 / data036 /
  data056 / data057 additions can stay: nothing else reads them (data057's replaced `Texture63_NEW` has no March key removed; the pre-S6 files are under `kom_backup_stage_june\20261018_1218*`, `kom_backup_s6_slot_ui\Enum.lua.before_S6`).

### June key-config rows for the fifth skill keys (skill-system-core S6b, 2026-10-18)

Flag `SERV_IRUHADEV_JUNE_SKILL_KEYCONFIG`, defined in `KTDXLIB/Always.h` after `SERV_IRUHADEV_JUNE_SKILL_SLOT_UI` (an `#error` guards the `SLOT_10` dependency), plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S6b". Log tag `[JUNE_SKILL] UI`
- **The dialog is `DLG_UI_KEY_SETTING.LUA` driven by `CX2KeyPad`, not `X2StateOption`** (`MappingKeyConfigIndex` is the gamepad list). Slots, drag/drop, the conflict swap and the save are generic over `GAMEACTION_END`, so F / T were already
  draggable; what was missing was the icon and tooltip. `CX2UseKeySlotData::UpdateUI` and `CX2DisUseKeySlotData::UpdateUI` (`X2KeyPad.cpp`) get cases for `GAMEACTION_SKILL5` (art `SKILL_A5`, string 41915 "Skill Slot 5")
  and `GAMEACTION_SKILL_EX5` (`SKILL_B5`, 43093 "Skill Slot B 5"), June `31f5be70` region / `31f5f040` region; the strings are read out of the live `general.ess` and are passed as numbers (March's `STR_ID_` enums stop at 30534).
- **Art (data063).** June's `DLG_UI_KEYSETTING.TET` (6,080 B; adds `SKILL_A5` / `SKILL_B5`) and `.tga` replace March's (staged with `kom.py stage-june`, data063 packed and copied, md5 97e47f22). Backups `kom_backup_stage_june\20261018_131139`,
  `kom_backup_tools\data063.kom.20261018_141325`.
- **Defaults (judgement call).** March binds RIDING to F and GUILD to G; June's `31f5da70` binds only SKILL5 to F, RIDING (action 40, read at `322f7fe3`) to G, and has no GUILD action (its `GUILD_INFO` 45 = `,`). With RIDING on F the
  dialog's F slot showed riding and SKILL5 could not be dragged at all, so the three key-pad default maps (`DefaultSlotData`, `JuneSetDefaultKeyPadMap`, `InitKeySlotData`) and `CKTDIManager::SetDefaultMap` move RIDING F -> G and GUILD G -> `,`.
  WARP (June `'`) is untouched: nothing else sits on B in March.
- **Revert:** comment out `SERV_IRUHADEV_JUNE_SKILL_KEYCONFIG` in `KTDXLIB/Always.h`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy` (riding / guild go back to F / G; the F / T slots lose their icons). The data063 files can stay (the old
  `.TET` is a subset). Keys are not persisted offline, so no save changes.

### June state buff factors and the state-string timer (skill-system-core S3e, 2026-10-18)

Flags `SERV_IRUHADEV_JUNE_SKILL_STATE_BUFF` (needs `_JUNE_BUFF_BEHAVIOR_PARITY`, `_JUNE_BUFF_GROUP`, `_JUNE_SKILL_TEMPLET`; `#error` guards) and `SERV_IRUHADEV_JUNE_SKILL_STATE_STRING_TIME`, both in `KTDXLIB/Always.h`;
plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S3e"; log tags `[JUNE_SKILL] SBUF` / `SSTR` (with `SERV_IRUHADEV_JUNE_SKILL_DEBUG`, throttled per site).
- **`_STATE_BUFF`.**
  - `CX2GameUnit::SetBuffFactorToGameUnitByBuffFactorID` is June's `322a6960`. Its arguments are `( BFI, level, pCreatorUnit_ = NULL, iAdjustSkillID_ = 0, bSkipAdjust_ = true )`. The defaults are March's behaviour.
  - A user without skip runs slot 476 (the memo rescale) with the adjust id or `+0x9274`, then slot 141. A creator unit stamps its UID.
  - Five engine callers pass June's skip 0: `ApplyBuffFactor_LUA`, the servo-nitro custom function, the emblem EXP buff, and ExtraDamage 788 / 804.
  - `CX2GUUser::ParsingBuffFactorID` / `CommonFrameMove_InsertBuffFactor` are June's whole. The list is an ordered vector, so duplicates are kept and there is no popup.
    - On an upgraded skill the `_ULTIMATE` table is read. March's `_MEMO` table is gone from June.
    - `+0x9274` is set, and the skill level comes from the upgrade swap.
    - User vslot 0x830 (`JuneOnStateInsertBuffFactorApplied`, empty) runs after each frame apply.
  - **`CommonStateEnd` now applies every reserved buff factor whose time was not reached** (June `@32015284`).
  - `SetBuffAccumulationCount` falls back to `+0x9274`.
  - What a player sees today: the reserved buffs fire at state end, and a memo rescale once S7 activates memos.
- **`_STATE_STRING_TIME`.**
  - `CX2Game::SetStateString( str, fShowTime_ = 0 )`. `OnFrameMove` counts a non-zero time down and clears the string.
  - Every March call passes 0, so nothing changes until class code calls `CX2GUUser::JuneShowSkillNameStateString` (3 s, June `3200ab60`).
- **Revert:** comment out the flag(s) in `KTDXLIB/Always.h`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. Each `#else` keeps March's code. No data, save or kom changes.

### June's learn path: server-pushed skills, level-up auto-equip, the slot-change queue (skill-system-core S7, 2026-10-18)

Flag `SERV_IRUHADEV_JUNE_SKILL_ACQUIRE`, defined in `KTDXLIB/Always.h` **and** `KncWX2Server/Common/ServerDefine.h` (`EventID_Client.h` / `ClientPacket.h` precede `Always.h`; `SECOND_HOMES` in
`tools/flagcheck.py`); `#error` guards for `_SLOT_10`, `_SKILL_TEMPLET`, `_SKILL_REFORM`, `_SKILL_TYPE`, `_SOCKET_PARITY`. Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7"; log tag `[JUNE_SKILL] ACQ`.
- **June's client never learns a skill.** The learn REQ (June 0x11c) has no sender and the one-skill reset sender (`3220df10`) no caller; June tree rows have no SP key. The server pushes skills with a
  June-only NOT (`{ UID, vector<KSkillData> }`, handlers `323b0770` / `324631a0` / `32535be0`; June's id is not recoverable), added client-side as `EGS_JUNE_ACQUIRE_SKILL_NOT` at the end of the client event block
  (offline only: it renumbers the global ids after it, harmless since client and emulator share the exe).
- **Client.** `CX2SkillTree::Handler_EGS_JUNE_ACQUIRE_SKILL_NOT` -> `CX2UserSkillTree::AddJuneAcquiredSkills` (June `328924c0`: an "[Enhanced]" skill erases its base from the acquired map). The level-up
  handlers (`CX2TFieldGame`, `CX2StateMenu`) end with `CX2SkillTree::JuneAutoEquipOnLevelUp` (June `32206d30` + `328db990`: below level 11, castables whose required level is the level's multiple of 5 go into free
  A, then B slots). `Handler_EGS_CHANGE_SKILL_SLOT_REQ` is June's `3220e3b0` (checks + queue); `JuneSendChangeSkillSlotReq` (`3220e5f0`) is pumped once a frame from `CX2Data::OnFrameMove` (`322105c0`).
- **Emulator (labelled stand-in, June's rule is server-only).** `CX2OfflineSkill::GrantJuneSkillsByLevel`: each class tree row gets one level per `m_RequireCharacterLevel` entry at or below the unit's level, capped at the
  master level; skipped: no required level (900, the couple skill), `SPRT_HYPER_CLASS` (needs transcendence, Stage 6), born-sealed and not unsealed. An upgraded skill zeroes its base and takes its slot. Run at
  level-up (NOT before the level-up NOT), at job change (NOT), at character select (silent catch-up) and after a whole-tree reset (NOT with every acquired skill).
- **Revert:** comment out both defines, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. Saves keep the extra rows the grant wrote (levelled rows, bases at level 0); they read as learned skills
  under the S3 rules. No data files changed.

### June's skill tree UI (skill-system-core S7b, 2026-10-18)

Flag `SERV_IRUHADEV_JUNE_SKILL_TREE_UI` in `KTDXLIB/Always.h` (`#error` guards: `_SLOT_UI`, `_ACQUIRE`, `_SKILL_NOTE`, `_SKILL_TYPE`, `_SLOT_10`, `_SKILL_TEMPLET`, `_SKILL_REFORM`,
`_SOCKET_PARITY`, `SERV_IRUHADEV_OFFLINE`). Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7b"; log tag `[JUNE_SKILL] UI`.
- **Code.** June's `CX2UISkillTreeNew2015` (class block `328d98e0`-`328ee680`) lives in `X2Lib/X2UISkillTreeNew2015.h/.cpp` under March's class name, so the callers of
  `CX2UIManager::GetUISkillTree()` are unchanged. `X2UISkillTreeNew.h` includes the June header in its `#ifdef` and keeps March's class in the `#else`; `X2UISkillTreeNew.cpp` includes
  the June `.cpp` in its `#ifdef` and keeps March's code in the `#else` / `#ifndef`. The June `.cpp` is not a compile unit of its own (it has no `StdAfx.h`); the vcxproj lists it as `None`.
- **No learn / SP UI** (user, 2026-10-18): June's tree has no learn, upgrade or init buttons and no SP box. March's `EGS_GET_SKILL_REQ` sender, the emulator's `LearnSkills` and the
  SP display stay compiled but nothing reaches them with the flag on. `GetNowLearnSkill()` returns false (June has no learn mode), so the nine class `GetNowLearnSkill` checks never block a skill.
- **Data (they move with the exe).** data034: June's `DLG_UI_Skill_Tree_New2015.lua`, `DLG_UI_Skill_Icon_New2015.lua`, `DLG_UI_Skill_Desc_Video.lua`, `DLG_UI_Command_Desc_Video.lua`
  (adopt-june, ljconv bytecode); data036: June's `SkillTypeColor.lua` (adopt-june) and 58 June `SKILL_TREE_UI_CUSTOM_MSG` members in the merged `Enum.lua` (ledger refreshed);
  data057: June's `DLG_UI_Common_Texture_NewSkill_02` / `_03` `.tga` + `.tet` (stage-june).
- **`SERV_IRUHADEV_SKILLTREE_NO_LOCK` is retired** (user decision, S0b): its `#define` is gone from `Always.h` (a comment marks the spot). Its two sites are in March's tree code, which the
  flag compiles out; with the flag off they read the March lock logic again.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`, and restore data034 / data036 / data057 from `kom_backup_tools/` (the added files are unused by March's
  class, so leaving them in is harmless). Put `#define SERV_IRUHADEV_SKILLTREE_NO_LOCK` back for the old two-choice behaviour.

### June's skill memos (skill-system-core S7c, 2026-10-18)

Flag `SERV_IRUHADEV_JUNE_SKILL_MEMO_PACKET`, defined twice: `KTDXLIB/Always.h` (`#error` guards: `_SKILL_NOTE`, `_SKILL_TREE_UI`, `_ACQUIRE`, `_SLOT_10`, `SERV_IRUHADEV_OFFLINE`)
and `KncWX2Server/Common/ServerDefine.h` (EventID_Client.h / ClientPacket.h precede Always.h; `SECOND_HOMES` in `tools/flagcheck.py`). Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7c";
log tag `[JUNE_SKILL] MEMO`.
- **Client.** The tree's memo buttons, state icons and select popup (`328dda50`, `328e9b30`, UICustomEventProc 29065-29068 / 29076 / 29077), the memo texts in the tooltip
  (`32453420` / `324513e0`, `X2SkillNoteManager`), June's bool `32452150` (`HasJuneMemo`; S3b's `GetJuneMemoValue` is its `#else`), the grant handler's upgraded-skill slot swap
  (`328df480` / `328ded60`), the ACK handlers (`CX2SkillTree`, June `31f1bdb0` / `31f1c1d0`), and the unit-info merge in `SetKUnitInfo` / `SetKRoomUserInfo` (`3235eb20` / `32360090`).
- **Packets.** `EGS_JUNE_ACTIVE_SKILL_MEMO_REQ` / `_ACK`, `EGS_JUNE_DEACTIVE_SKILL_MEMO_REQ` / `_ACK` appended to `EventID_Client.h` (June's 0x554-0x557); the active memos ride in the existing
  `KJuneSkillExtraData::m_mapIntInt` (skill -> memo).
- **Emulator and save.** Two handlers in `Handlers_Skill.cpp` (a labelled stand-in: no June server), schema **v19**: `unit_skill_note` is recreated as (unit, skill, memo); March's notebook
  REQ (`EGS_REG_SKILL_NOTE_MEMO_REQ`) is refused with a log line.
- **Revert:** comment out both defines together, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. **The save is forward-only:** an exe without the flag refuses a v19 save, so restore
  the `db_backup/` snapshot taken before the first v19 launch (`els_db_20261018_195712_pre_manual.sql`) together with the old exe.

### June's skill pages (skill-system-core S7d, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_PAGE`, defined twice: `KTDXLIB/Always.h` (`#error` guards: `_SKILL_TREE_UI`, `_SKILL_MEMO_PACKET`, `_ACQUIRE`, `_SLOT_10`, `SERV_IRUHADEV_OFFLINE`)
and `KncWX2Server/Common/ServerDefine.h` (ClientPacket.h precedes Always.h; `SECOND_HOMES` in `tools/flagcheck.py`). Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7d"; log tag `[JUNE_SKILL] PAGE`.
- **Client.** June's page UI (`UpdateSkillPageUI` `328e6970`: no ED or decide button, the cash button beside the last radio, village only), the one radio case 29049 (`328ec068`: the radio's
  dummy int is the page; busy / cool-down guards; then the decide REQ), the decide REQ / ACK (`3220f160` / `3220f2b0`: the ACK's `KJuneSkillExtraData` vectors are the page's bars, its map
  the page's memos; SP and skill levels are not touched) and the expand ACK (`32214c60`: page count, notice, ED). Local, not June: `CX2UserSkillTree::JuneGrowSkillPages` gives every opened
  page a skill map (March indexes the page in use unchecked).
- **Packet.** `KEGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK` gains June's trailing `KJuneSkillExtraData` (June `320e0620` @+0x6a).
- **Emulator and save** (labelled stand-in, no June server). Handlers for `EGS_GET_NEXT_SKILL_PAGE_ED_REQ` / `EGS_EXPAND_SKILL_PAGE_REQ` (50,000,000 ED, the server's
  `ED_NEEDED_TO_EXPAND_SECOND_SKILL_PAGE`) / `EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ` (swaps the bar, then re-sends the shared skill list); the cash item 127273 (`CI_EXPAND_SKILL_PAGE`)
  is claimed as a page plus an unsolicited `EGS_EXPAND_SKILL_PAGE_ACK`; select unit sends one page entry per open page; a whole-tree reset leaves every bar empty, as June's client shows.
  Schema **v20**: `unit_skill_page_info` (count, page in use) and `unit_skill_page` (bars of the pages not in use); the migration offers 127273 in cash category 33.
- **Revert:** comment out both defines together, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. **The save is forward-only:** an exe without the flag refuses a v20 save, so restore
  the `db_backup/` snapshot taken before the first v20 launch (`els_db_20261019_022520_pre_pre_S7d_pages.sql`) together with the old exe.

### June's skill tree panels and command tab (skill-system-core S7e, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_TREE_PANELS` in `KTDXLIB/Always.h` (`#error` guards: `_SKILL_TREE_UI`, `SERV_IRUHADEV_OFFLINE`). Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7e";
log tag `[JUNE_SKILL] UI`.
- **Client.** The tree's major-skill panel (`328e42d0`), major-command panel (`328e8820` / `328e5290`), command tab lists (`328e7ba0` / `328df4c0` / `328e4bd0` / `328de9a0`), the
  description fit (`328df060`), the description popup (`328e6cd0`, `DLG_UI_Command_Desc_Video.lua`) and UICustomEventProc 29061 / 29062 / 29069-29074 (`X2UISkillTreeNew2015.*`);
  `CX2SkillTree` gains June's readers of `SkillCommandDescriptionData.lua` (`3220bdb0`) and `ComboTree.lua` (`3220d4d0`), the `CommandIconData.lua` map (binder `3220f8d0`, loaded at the
  end of `OpenScriptFile` like June's `32207dd0`) and the icon picture builder (`32207630`); their bodies sit in `X2UISkillTreeNew2015.cpp`, so the CP949 `X2SkillTree.cpp` only gains the call.
  `CKTDGUIControl::GetDummyPosNum()` (June `328edc20`, inlined there).
- **March's combo tree is retired.** June never calls `CX2Data::ResetComboTree` (`31eeaf20`), so `X2StateLoading.cpp`'s call is in the `#else`; the old combo window
  (`CX2UIComboTree`, e.g. from the training game) opens empty and logs why instead of crashing on a NULL `GetComboTree()`.
- **Data (they move with the exe).** data036: June's `SkillCommandDescriptionData.lua`, `CommandIconData.lua` (new) and `ComboTree.lua` (replaces March's; adopt-june, ljconv bytecode;
  backup `kom_backup_stage_june\20261019_051018`), and June's 48 missing `COMBO_TREE_ICON` members appended to the merged `Enum.lua` (ledger refreshed).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`, and restore data036 from `kom_backup_tools/` (March's `ComboTree.lua` is needed again by the
  `#else`'s `ResetComboTree`). No save change.

### June's in-room skill-info notice (skill-system-core S7f, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_MEMO_RESYNC` in `KTDXLIB/Always.h` (`#error` guards: `_SKILL_MEMO_PACKET`, `SERV_IRUHADEV_OFFLINE`). Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7f";
log tag `[JUNE_SKILL] MEMO room`.
- **Client.** `CX2Room::Handler_EGS_CHANGE_SKILL_INFO_IN_ROOM_NOT` (`X2Room.cpp`) is June's `31ed4c80`: the unit from the game user or else the room slot, another user's note list and
  active memos (clear + merge from `m_kJuneSkillExtraData`), the ten slots set without the changed-slot check, then every occupied slot's user re-reads its socket and skill state (`31ed3f90`).
  March's KStat and packet are unchanged. The emulator never sends this NOT, so nothing changes offline.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### Socket sources the S7k aggregate skipped (skill-system-core S7m, 2026-10-19)

Flags `SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET_EDT` and `SERV_IRUHADEV_JUNE_SKILL_PARTY_SOCKET` in `KTDXLIB/Always.h` (both `#error`-guarded on `_UNIT_SOCKET`). Plan
`JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7m"; no log tag (nothing reachable offline).
- **Client.** `CX2Unit::JuneSumSocketOption` copies the socket's single extra damage into the unit aggregate (June `32364790 @32364af3`); `CX2Unit::JuneGetPartySocketOptionSources` /
  `JuneIsPartyShareSocketID` / `JuneGetPartyShareSocketOptions` (June `31ed40c0` / `31d70d30` / the pass both sums inline) add room members' shared sockets in
  `CX2Unit::JuneInitSocketData` and `CX2GUUser::InitSocketData`.
- **Revert:** comment out either define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June's unit socket aggregate and HUD max MP / HP (skill-system-core S7k, 2026-10-19)

Flags `SERV_IRUHADEV_JUNE_SKILL_UNIT_SOCKET` (`#error` guards: `_SOCKET_PARITY`, `_SOCKET_EDT`), `SERV_IRUHADEV_JUNE_SKILL_STAT_GETTERS` (`_SKILL_TEMPLET`, `_ITEM_KIM`) and
`SERV_IRUHADEV_JUNE_SKILL_HUD_MAXHPMP` (needs the other two and `_SKILL_MAX_MP`) in `KTDXLIB/Always.h`. Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7k"; log tag `[JUNE_SKILL] HUD`.
- **Client.** CX2Unit owns June's socket aggregate (`+0xe1c`), filled by `JuneInitSocketData` (June `32365d30`) through `JuneSumSocketOption` (`32364790`); June's
  `GetUnitStat` (`32358b00`) / `GetEqipStat` (`3234e210`) / `CX2Inventory::JuneGetEquipGroup` (`3286a4b0`); the HUD refresh `CX2Unit::JuneResetHudMaxHpMp` (`3235a160`)
  at 15 of June's 23 call sites (skill tree ACKs, inventory slot, repair, pets, titles, riding-pet unsummon, class change, field state ctor).
- **Revert:** comment out `_HUD_MAXHPMP` (the HUD goes back to the emulator's `KGamePlayStatus` maxima), then `_STAT_GETTERS` and `_UNIT_SOCKET` if wanted,
  `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June's max MP (skill-system-core S7j, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_MAX_MP` in `KTDXLIB/Always.h` (`#error` guard: `_SKILL_TEMPLET`). Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7j"; log tag `[JUNE_SKILL] MAXMP`.
- **Client.** `CX2Unit::ResetMaxMp( base, group )` = June `3234b9d0` (SA_MAX_MP_ABS of 19 passives, 17 June-only `RNSI_*`; March read 2402 / 5402 / 5410 / 7402, gone
  from June's skill data); its equipment term `CX2Inventory::JuneGetAddMaxMPValue` = June `32869f80`; `CX2GUUser::ResetMaxMP` = June `31fd2810`.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### The rest of June's emblem manager (skill-system-core S7l, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_EMBLEM_MANAGER` in `KTDXLIB/Always.h` (`#error` guards: `_LEVEL_EMBLEM`, `ADD_PLAY_MUSIC_WHEN_EMBLEM_POP`, `DIALOG_SHOW_TOGGLE`, `FIELD_BOSS_RAID`,
`_RANOX_PORT`, `_SOUND_AUTO_RELEASE`, `_SKILL_STATE_BUFF`); debug `SERV_IRUHADEV_JUNE_SKILL_EMBLEM_DEBUG` (dev console `JuneEmblem( id )`, off since the S7l review, 2026-10-19). Plan
`JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7l"; log tag `[JUNE_SKILL] EMBLEM`.
- **Client.** `X2EmblemManager.*`: `EXPAND_TEXTURE_FILE_NAME` rows become UI particles that follow the camera and hold the next emblem, June's `OnFrameMove` / `InitEmbelmPlayInfo`
  whole (hide-dialog gate in the pick), cases 5 / 7; `X2BossRaidManager.cpp`: the gate-open emblem gets two one-shot rows instead of the 2nd texture, Ranox open textures;
  `X2PremiumBuffManager.cpp`: BFI 10060 in a room plays `EI_ROSE_EVENT_INT`.
- **Data.** data063 + June's `RaidField_Area_0.dds` / `_1.dds` (new; nothing replaced).
- **Revert:** comment out both defines, `python tools/build.py --deploy`; optionally remove the two data063 files and re-pack / copy (harmless if left). No save change.

### June's class-change notice (skill-system-core S7h, 2026-10-19)

Flags `SERV_IRUHADEV_JUNE_SKILL_CLASS_CHANGE` in `KTDXLIB/Always.h` **and** `KncWX2Server/Common/ServerDefine.h` (`ClientPacket.h` precedes `Always.h`; `#error` guards:
`UNIT_CLASS_CHANGE_ITEM`, `_SLOT_10`, `_ACQUIRE`, `_MEMO_PACKET`, `_TREE_UI`, `_HUD_MAXHPMP`, `ADDED_RELATIONSHIP_SYSTEM`) and `SERV_IRUHADEV_JUNE_SKILL_RELATION_UNEQUIP`
(`Always.h` only). Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7h"; log tag `[JUNE_SKILL] CLASS`.
- **Packet.** `KEGS_BUY_UNIT_CLASS_CHANGE_NOT` takes June's layout (Get `320ed4e0`): `short` class, no memo map, a trailing `vector< KSkillData >` (`ClientPacket.h/.cpp`).
- **Client.** `CX2State::Handler_EGS_UNIT_CLASS_CHANGE_CASH_ITEM_NOT` = June `31f276b0` whole: both bars emptied, the page in use takes the packet's acquired list
  (`CX2UserSkillTree::ResetJuneAcquiredSkills` = `328929e0`), the couple skill 900 takes the relation state, no per-page default skills, no skill-note map, June's refresh order.
  `CX2SkillTree::UnequipRelationSkill` = June `3220f740` (a bar-B hit is unequipped as slot index + 5).
- **Emulator.** `Handlers_Shop.cpp`: the class-change NOT carries the unit's whole acquired list (after the S7 grant for the new class) and its unsealed list.
- **Callees June changed (own flags, `Always.h` only):** `SERV_IRUHADEV_JUNE_SKILL_CLASS_POPUP` (`X2StateClassChange.cpp`: June's dialog table `32486e60`, scale 1.4 for
  Chung Tactical Trooper, June's voice `32486c10`, needs `ADD_PLAY_SOUND` + `_SOUND_AUTO_RELEASE`); `SERV_IRUHADEV_JUNE_SKILL_CLASS_ITEM_CHOICE` (`X2CashShop.*`: June's two
  class-change item tables, needs `ADJUST_UNIT_CLASS_CHANGE_ITEM_REPETITION`); `SERV_IRUHADEV_JUNE_SKILL_SQUARE_VIEWER_RESET` (`X2State.cpp`: June's `31efd510` verbatim,
  user 2026-10-19; a class change bought while in the square now reads a NULL field game, as in June: JUNE_BUGS 348).
- **Data.** data095 + June's `RenaVoice_ClassUpFinish01.ogg` (new, nothing replaced; the popup voice names Rena where March named Lena).
- **Revert:** comment out both `CLASS_CHANGE` defines together (and/or `RELATION_UNEQUIP`, `CLASS_POPUP`, `CLASS_ITEM_CHOICE`, `SQUARE_VIEWER_RESET`), `touch X2Lib/stdafx.cpp`,
  `python tools/build.py --deploy`. The data095 file is harmless if left (or remove it, `kom.py pack data095`, ask, copy). No save change.

### June's new-title notice (skill-system-core S7n, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_TITLE_NOTICE` in `KTDXLIB/Always.h` (`#error` guards: `_EMBLEM_MANAGER`, `_LEVEL_EMBLEM`). Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7n";
log tag `[JUNE_SKILL] EMBLEM title`.
- **Client.** `X2TitleManager.*`: earned titles and opened title missions are queued once (June `326f88a0`, sets seeded by `TakeTitle`), `CheckNewTitle` is June's
  `32701960` (level < 5 drops them; emblem 18 `EI_NEW_TITLE_INFO_NOTICE` instead of the info text / chat line / OK box; title 10 asks "Apply title now?"),
  REWARD_TITLE_NOT / NEW_MISSION_NOT / UpdateMission / EQUIP_TITLE_ACK as June has them; `X2State.*`: custom messages 3085 / 3086 (the box's OK / Cancel);
  `X2EmblemManager.*`: map `+0x78` and case 0x12 draw two icon + text rows per pop in `DLG_Get_New_Skill_or_Memo.lua`.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June's level-up new-skill emblem (skill-system-core S7i, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_LEVEL_EMBLEM` in `KTDXLIB/Always.h` (`#error` guards: `NEW_EMBLEM_MANAGER`, `_SKILL_NOTE`, `_SKILL_TEMPLET`, `_SOCKET_PARITY`). Plan
`JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7i"; log tag `[JUNE_SKILL] EMBLEM`.
- **Client.** My unit's level-up (`X2TFieldGame.cpp`, `X2StateMenu.cpp`) feeds `CX2SkillTree::JuneShowNewSkillOrMemoEmblem` (June `31d885e0`), which queues what opens at the level
  in `CX2EmblemManager` and plays emblem 17 `EI_GET_NEW_SKILL_OR_MEMO`: a `LUA_FILE_NAME` dialog opened by June's `PlayEmblemEffect`, faded / closed in `OnFrameMove`, two icon rows per pop.
- **Data.** data036 `Emblem_Manager.lua` (June's), `Enum.lua` (+ EMBLEM_ID 14-34); data217 `DLG_Get_New_Skill_or_Memo.lua`; data053 `DLG_Ui_Guild_01.tga` / `.TET`.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`; restore data036 / data053 / data217 from
  `kom_backup_stage_june\20261019_090743` / `_090747` (Emblem_Manager.lua) and remove the three new files, take EMBLEM_ID 14-34 out of `Enum.lua`, re-pack and copy.
  No save change.

### June's cash shop open-at-item list (skill-system-core S7g, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_CASH_OPEN_ITEM` in `KTDXLIB/Always.h`. Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S7g"; log tag `[JUNE_SKILL] HUD`.
- **Client.** `CX2CashShop` keeps June's open-at-item list (`m_vecJuneOpenAtItemID`, June `+0x338`); `SetOpen( true )` hands the items that have a product to the buy pop-up
  (`JuneOpenAtItemList` = June `3224a5d0`, on March's `BuyItemPopup`) and clears the list. Pushed by the skill tree's page cash button (127273) and the quick-slot expand button (274333).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June's running-state pointer model (skill-system-core S8, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_STATE_PTR` in `KTDXLIB/Always.h` (`#error` guard: `_SKILL_TREE_UI`). Plan `JUNE_SKILL_SYSTEM_PHASES.md` "Phase S8"; log tag `[JUNE_SKILL] PTR`
(with `SERV_IRUHADEV_JUNE_SKILL_DEBUG`).
- **Client.** `CX2GUUser::m_FutureStateData` / `m_NowStateData` (by-value copies) become `m_itFutureStateData` / `m_itNowStateData`, iterators into `m_StateList` (June `+0x7cb0` /
  `+0x7cbc` point into the state vector; the state id is the key); the running power rate is `m_fNowStatePowerRate` (June `+0x7cc8`); June's five-delegate snapshot (`31fd2af0`,
  `+0x9544..+0x9568`) is taken by `JuneSnapshotBeforeSlotRebind` before every `SetEquippedSkillFuncBySkillSlotIndex` call and preferred by DoFrameMoveFuture / DoStateEndStartFuture /
  DoFrameMove / DoStateEndStart until the next state change. Every reader converted under the flag: `X2GUUser.h/.cpp` and the Add, Ara, Arme, Chung, Elsword, Lire, Raven class files.
- **What changes in game:** re-equipping a slot (slot drop, page switch, another user's slot change) while that slot's skill runs now rewrites the running state as June does, except the
  five snapshot delegates; class code's power-rate multiplications go to the running rate, not the state's entry. JUNE_BUGS 327.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June GFx runtime: SDK link and system init (june-gfx G1, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_GFX_RUNTIME` in `KTDXLIB/Always.h` (client only). Plan `JUNE_GFX_RUNTIME_PHASES.md` "Phase G1"; log tag `[JUNE_GFX] RUNTIME`.
- **Client.** `KTDXApp.cpp` links `libgfx`, `libgfx_as2`, `libgfx_as3`, `libgfxrender_d3d9`, `pcre` (`#pragma comment lib`, inside the flag); the CKTDXApp ctor / dtor bring
  `Scaleform::GFx::System` up / down; the first frame writes `[JUNE_GFX] RUNTIME Scaleform 4.2.22 up` to ErrorLog.txt. New `KTDXLIB/KTDGFXRuntime.{h,cpp}` hold the System object and
  the link shims (six cdecl zlib wrappers over the client's zlib 1.2.3, two PNG longjmp functions over libpng 1.2.24; the client's zlib / png / jpeg stay the one copy).
- **Project files** (inert with the flag off): `X2/X2_2010.vcxproj` and `KTDXLIB/KTDXLIB_2010.vcxproj` `LibraryPath` of `US_INTERNAL` / `US_SERVICE` start with
  `$(SolutionDir)\Libs\ExternalLib\Scaleform\Lib\Win32\Msvc10\Release;`; `KTDXLIB_2010.vcxproj` lists `KTDGFXRuntime.cpp` (per-file Scaleform `Src` + `Include` dirs) and `KTDGFXRuntime.h`.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. The libraries then drop out of the link; the project-file hunks may stay.

### June GFx runtime: the GFx manager (june-gfx G2, 2026-10-19)

Flags `SERV_IRUHADEV_JUNE_GFX_MANAGER` (+ `SERV_IRUHADEV_JUNE_GFX_DEBUG`) in `KTDXLIB/Always.h` (client only; the define `#error`s without `JUNE_GFX_RUNTIME` /
`JUNE_RENDER_FRAME`). Plan `JUNE_GFX_RUNTIME_PHASES.md` "Phase G2"; log tag `[JUNE_GFX] MGR`.
- **Client.** New `KTDXLIB/KTDGGFXManager.{h,cpp}`: June's manager ( loader states, kom-first file opener, clipboard, log, AS2 + AS3, image handlers, Win32 fonts,
  `fontconfig.font` "US", D3D9 HAL + Renderer2D, `main.gfx` root movie, the 14 AS wrappers ). New `KTDXLIB/KTDGFXSound.cpp` compiles the SDK's three `Src/Sound` files
  (FMOD renderer on KTDS's FMOD Ex system). Hooks: `KTDXApp.h/.cpp` (member, ctor / dtor, `JuneOpenPerturbationMap` = Init, OnFrameMove dt, OnFrameRender draw, MsgProc
  mouse first / keys after, reset / lost, four `JuneGfx*` methods, the DEBUG Lua binds), `DXUT/DXUT.cpp` (PeekMessage loop), `KTDXStage.h` (no-op virtual
  `JuneGfxSetMovieUI`), `KGCMassFileManager.h/.cpp` (`IsValidFile_MassFileOnly` / `LoadDataFile_MassFileOnly`), `X2Main.cpp` (StateChange message stage),
  `X2StateServerSelect.cpp` (mouse input on).
- **Project files** (inert with the flag off): `KTDXLIB_2010.vcxproj` lists `KTDGGFXManager.cpp` (per-file Scaleform `Src` + `Include`), `KTDGFXSound.cpp` (the same plus
  `Src\Sound` and `$(ProjectDir)FMOD\inc`) and `KTDGGFXManager.h`; both .cpp are empty with the flag off.
- **G2 review (2026-10-19):** `_DEBUG` is off: the `JuneGfx*` dev-console binds and the GFx SDK log mirror are gone. A kom miss in `LoadDataFile_MassFileOnly`
  now logs `KEM_ERROR135` as June's lookup does.
- **Revert:** comment out `JUNE_GFX_MANAGER` (and `_DEBUG`, if it is on again), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`.

### June quest system: enums, parser keys, binders, new quest files (quest-system Q1, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_LOAD` in `KTDXLIB/Always.h`. Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Phase Q1"; log tag `[JUNE_QUEST] LOAD` (offline_server.log).
- **Client.** `X2QuestManager.h/.cpp`: June's enums (`QT_GUILD` 8 / `QT_CHANGE_TRANSCENDENTAL_CLASS` 9 / `QT_STORY` 10 so `QT_END` is 11; `QRT_WEEK` 3 / `QRT_MONTH` 4 /
  `QRT_CHAIN_REPEAT` 5; `CJT_JOB_FOURTH` 6 so `CJT_JOB_END` is 7; `SQT` 37-48 so `SQT_END` is 49); quest keys `m_bIsAccount`, `m_bIsGuild`, `m_iChainID` (chain index),
  `m_vecAdditionalSceneQuestID`, `m_fSelectBGM` (a wstring), `Reward.m_iGuildCoopQuestRewardID`, the QT 9 `m_eChangeUnitClass % 1000` split, the `m_eUnitClass` QR tail scalar, the
  QT_STORY index; clear-condition cases 37-39 and 43-47, case 48 sharing case 8, `m_ePvpTournamentType` (cases 9 / 10), `m_cDifficulty` after the switch, and `m_eDungeonID` keeping 0 (11 sites);
  new `AddChainQuestDays_LUA`, `LoadChainQuestInfo`, `LoadQuestEventSceneInfo` (rows kept on the quest manager, not `g_pData`), `LogLoadCensus`. Guild quests (QT_GUILD / `m_bIsGuild`) are
  parsed and dropped with one log line each. `X2Main.cpp`: binders `AddGuildQuestTemplet_LUA` (same function as `AddQuestTemplet_LUA`), `AddChainQuestDays`, and `AddGuildRewardData` on
  `CX2GuildManager` (`X2GuildManager.*`: second global `g_pGuildCoopQuestRewardTable`, row multimap). `AddGroupChainQuest` is not bound (JUNE_BUGS 320). `X2Data.cpp`: `GuildQuest.lua`
  after `AccountQuest.lua`, then `ChainQuestInfo.lua` and `Quest_Event_Scene_Info.lua`, then the census line; `GuildCoopQuestRewardTable.lua` after `GuildData.lua`.
- **Data (moves with the exe).** data036: June's `GuildQuest.lua`, `ChainQuestInfo.lua`, `GuildCoopQuestRewardTable.lua`, `Quest_Event_Scene_Info.lua` (adopt-june, ljconv bytecode; all new) and
  the merged `Enum.lua` edited as text (the enum members above, `NUI_STORY_QUEST` 10142, `NUI_TRANSCEND_QUEST` 10153, `NUI_EVENT_KEEPER` 4116, `NUI_EVENT_GUILD_EDAN_VILAGE` 4255, the
  `QUEST_REWARD_TRAN_CLASS` and `PVP_TOURNAMENT_TYPE` tables, `PGT_WINNER` / `PGT_1VS1_CHALLENGE_MODE` / `PGT_AMUSEMENT_ITEM`, `PCC_1VS1_PVP_CHALLENGE`; ledger refreshed).
  `NUI_EDEL` stays 10072 (Q7 decides).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`, and restore data036 from `kom_backup_tools/` (`data036.kom.20261019_070244`). No save change.

### June quest system: data flip and old-save migration (quest-system Q2, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_DATA` in `KTDXLIB/Always.h` (needs `JUNE_QUEST_LOAD`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Phase Q2"; log tag `[JUNE_QUEST] MIG`.
- **Data (moves with the exe).** data036: June's `FieldQuest.lua`, `FieldSubQuest.lua`, `AccountQuest.lua` replace March's (adopt-june, ljconv bytecode; March's 257 quests go).
  March's originals are kept in `24965799\kom_contents_bytecode\data036_kom\` and `kom_backup_stage_june\20261019_074358\`.
- **Save v21** (`X2Lib/Offline/X2OfflineDB.*`, `MigrateQuestsToJune`; tables `X2OfflineJuneQuestMig.h` from `june2017_re/quest_plan/q2_gen.py`): in-progress and completed
  quests June has no templet for are deleted; where a dropped epic was in progress, the epic June opens from the same predecessor is accepted; quests whose sub-quests
  changed restart at 0 (user, 2026-10-19). One-way.
- **Client.** `X2UIQuestNew.cpp`: the two complete-ack handlers and four quest-list readers skip a quest id with no templet and log it once.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`, put March's three `.lua` back into `kom_contents\data036_kom\` and repack /
  copy data036, and restore the save from `db_backup\els_db_20261019_075100_pre_pre-quest-q2-v21.sql` (an exe without the flag refuses a v21 save).

### June quest system: quest NPCs and houses (quest-system Q7, 2026-10-19)

No flag, no C++, no build: Lua and assets only (they move with the koms). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q7 - Done".
- **data037 `NPCTemplet.lua`** (core source): June's 6 rows appended in one `--{{ Iruha : 2026-10-19 ... --}}` block (`NUI_STORY_QUEST`, `NUI_TRANSCEND_QUEST`, `NUI_ELSION_EVENT_00/01`,
  `NUI_EVENT_KEEPER`, `NUI_EVENT_GUILD_EDAN_VILAGE`).
- **data034 `DLG_Map_Village.lua`** (core source): `HI_STORY_QUEST`, `HI_TRANSCEND_QUEST` appended to the `HouseIDList` of Ruben, Elder, Besma, Altera, Peita, Velder, Hamel, Sander
  (marked `-- quest-system Q7`).
- **New files (stage-june / adopt-june, June byte-exact):** datai002 `Motion_NPC_Event_Keeper.x` + 3 `.tga`; datai024 `Motion_NPC_Event_EDAN.x`, `HQ_UI_NPC_EVENT_KEEPER.X`,
  `UI_NPC_Event_EDAN.x` + 3 `.tga`; datai016 `UI_NPC_EVENT_KEEPER.lua`, `UI_NPC_EVENT_EDAN.lua` (ljconv bytecode).
- **Revert:** delete the `Q7` block at the end of `NPCTemplet.lua` and the two `quest-system Q7` lines (+ the comma they needed) in each of the 8 village lists, `luasrc.py refresh`
  both, drop the 12 new files from datai002 / datai016 / datai024 (their manifests too), `kom.py pack` the five koms, ask, copy (copy backs the live kom up to `kom_backup_tools\`).

### June quest system: offline completion of every sub-quest type (quest-system Q4, 2026-10-19)

Flags `SERV_IRUHADEV_JUNE_QUEST_OFFLINE` (needs `JUNE_QUEST_LOAD`, `OFFLINE`) and the stand-in `SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN` (needs `_OFFLINE`) in `KTDXLIB/Always.h`.
Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q4 - Done"; log tag `[JUNE_QUEST] OFF` (offline_server.log). Emulator only (`X2Lib/Offline/`): no client UI, no shared struct, no save schema change.
- **`X2OfflineQuest.*`.** SQT 32 `SUITABLE_LEVEL_DUNGEON_CLEAR` driven (room server's band, `IsSuitableLevel`); SQT 8 gets the server's dungeon rules; a `{0}` dungeon list = any
  dungeon (`HasDungeonFilter`, 5 sites); `DAILY_REPEAT_HOUR` 0 (US); `CheckJuneRepeat` (weekly / monthly / chain-day accept refusals); account quests stored under `unit_uid = -user_uid`
  and opened at character select (`CheckAccountOpen`, `SaveKey`); `IsClearTypeUndriven` labels the other June types; census line `SQT coverage`. Stand-in: SQT 48 counts EXP-paying clears.
- **Handlers.** `Handlers_Room.cpp` decides suitable level at loading 100 (`KOfflineRoom::m_bSuitableLevel`, `X2OfflineServer.h`); `Handlers_Quest.cpp` passes it and the run's EXP to
  `OnDungeonClear` and refuses quest 42030's transcendental class change (`ERR_QUEST_18`); `Handlers_Unit.cpp` calls `CheckAccountOpen`.
- **Save.** Account quests write rows with a negative `unit_uid` (`unit_quest`, `unit_subquest`, `unit_quest_complete`). Unit 24 got quests 31110 / 26600 for the play-test
  (backup `db_backup\els_db_20261019_113906_pre_sql.sql`).
- **Revert:** comment out both defines (the stand-in first), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. An old exe ignores the negative-`unit_uid` account rows;
  `DELETE FROM unit_quest WHERE unit_uid < 0` (and the two sibling tables) removes them.
- **Q4 review (2026-10-19), same flag:** `IsQuestCountedDungeon` = `XSLDungeon.cpp` (`IsQuestTutorialDungeon`, the full TC range); field loads skip `IsSuitableLevel`.
- **Q4 review stand-ins (user 2026-10-19), two more flags (both need `_OFFLINE`):**
  - `SERV_IRUHADEV_JUNE_QUEST_STANDIN_TOGETHER`: SQT 44 (35365 in 35363 'The Ultimate Road 4/5') counts a clear that also meets the quest's other dungeon steps
    (`IsTogetherRunMet`). Revert: comment out the define; the quest stops at 4/5 again (progress already saved stays).
  - `SERV_IRUHADEV_JUNE_QUEST_STANDIN_RANOX_SECRET` (needs `RANOX_PORT`): 41270 / 41271 are not training camp in `IsQuestCountedDungeon`. Revert: comment out the define.

### June quest system: accept-time reward items and the exception dungeon list (quest-system Q4b, 2026-10-19)

Flags `SERV_IRUHADEV_JUNE_QUEST_OFFLINE` (accept items) and `SERV_IRUHADEV_JUNE_QUEST_OFFLINE_STANDIN` (exception list), both already in `KTDXLIB/Always.h`. Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q4b - Done"; log tag `[JUNE_QUEST] OFF`.
Two server-only June keys the client parser now reads for the emulator. No shared struct, no save schema change.
- **`X2QuestManager.h/.cpp`.** `QuestTemplet::m_mapAcceptRewardItem` (from `m_vecAcceptRewardItem`, 7 quests) and `ClearCondition::m_setExceptionDungeonID` (from `m_eExceptionDungeonID`, 6 sub-quests; entries are dungeon id + difficulty).
- **`X2OfflineQuest.*`, `Handlers_Quest.cpp`.** `Accept` pays the items last (`PayAcceptItems`: ERR_QUEST_24 held, ERR_QUEST_25 no room); the slots go out in `EGS_NEW_QUEST_NOT` (`TakeAcceptSlots`, REQ and `PushNewQuest`);
  a give-up deletes the items (`RemoveAcceptItems`); `OnDungeonClear` and `IsTogetherRunMet` skip a listed dungeon (`IsExceptionDungeon`).
- **Save.** Unit 21 (eresisu) was given item 111371 to show the ERR_QUEST_24 refusal (backup `db_backup\els_db_20261019_180120_pre_additem.sql`).
- **Revert:** comment out the stand-in define for the exception list, and `SERV_IRUHADEV_JUNE_QUEST_OFFLINE` only if the whole offline completion is going (it also drops Q4); `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. Items already paid stay in the bag.

### June quest system: dungeon result ranks offline (quest-system Q4c, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK` in `KTDXLIB/Always.h` (needs `OFFLINE_MONSTER_GRADE`, `OFFLINE_SECRET_POISON`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q4c - Done"; log tag `[JUNE_QUEST] RANK`.
- **Emulator only.** New `X2Lib/Offline/X2OfflineDungeonRank.h/.cpp` (listed in `X2Lib_2010.vcxproj`) loads the server's `ResultData_new.lua` and binds `ResultProcess` and the two argument objects. `Handlers_Room.cpp`: die counts by monster type, `m_nSec` less loading, ranks + rank bonus EXP in `SendDungeonResultData`, `QuestOnDungeonClear` moved there from `Handler_EGS_END_GAME_REQ`. `Handlers_Quest.cpp`: `bHaveExp` read from `m_mapHaveExpInDungeon`. `X2OfflineServer.h/.cpp`: room counters, declaration, `Release`.
- **Data.** `KncWX2Server/ServerResource/US/ResultData_new.lua` XOR-packed into data036.kom by the user (server-only Lua). Without it every rank is F and the log says so.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. Ranks go back to F, quests back to RT_NONE at end game. No save change (rank bonus EXP already paid stays).
- **Found by its play-test, own flag `SERV_IRUHADEV_OFFLINE_DUNGEON_CONTINUE_END`** (needs `OFFLINE`): a solo dungeon death ends the run as the room did (15 s timer from the die REQ, X = `EGS_SKIP_CONTINUE_DUNGEON_NOT` ends at once, 11 s cash-revive window, a revive cancels). `Handlers_Room.cpp` (die / resurrect / stop-continue / skip handlers, `EndDungeonGame` = `Handler_EGS_END_GAME_REQ`'s body after its ACK), `Handlers_Field.cpp` (the beat), `X2OfflineServer.h/.cpp`. Revert: comment out the define and rebuild; a dead solo run then never ends.

### June skill system: flags without a section above, and the close-out (skill-system-core S3 / S3b / S3c / Z, 2026-10-19)

Plan `JUNE_SKILL_SYSTEM_PHASES.md`; log tag `[JUNE_SKILL]`. The other `SERV_IRUHADEV_JUNE_SKILL_*` flags have their own section above (S2 TEMPLET, S4 NUMERICAL, S3d MEMO_*, S5 SLOT_10, S6 SLOT_UI,
S6b KEYCONFIG, S3e STATE_BUFF / STATE_STRING_TIME, S7 ACQUIRE, S7b TREE_UI, S7c MEMO_PACKET, S7d PAGE, S7e TREE_PANELS, S7f MEMO_RESYNC, S7g CASH_OPEN_ITEM / HUD_MAXHPMP, S7h CLASS_CHANGE / CLASS_POPUP /
CLASS_ITEM_CHOICE, S7i / S7l / S7n emblems and notices, S7j MAX_MP, S7k UNIT_SOCKET / STAT_GETTERS, S7m UNIT_SOCKET_EDT / PARTY_SOCKET, S8 STATE_PTR, S3f STATE_ULTIMATE).
These three had none:
- **`SERV_IRUHADEV_JUNE_SKILL_REFORM`** + **`_REFORM_AUTOLEARN`** (S3, `KTDXLIB/Always.h`; the flip). Loader supersets (`X2SkillTree.cpp`: preceding-skill read, seal-skill scalar item id), June `GetSkillLevel 32890620`
  whole (`X2UserSkillTree.cpp`), save v17 (`X2OfflineDB::MigrateSkillsToJuneTree`: every `unit_skill` / `unit_skill_unsealed` / `unit_skill_note` row goes, SP = June init SP, CSP 0), auto-learn of each class's free
  rows (`X2OfflineJuneSkillAutoLearn.h`, generated; labelled stand-in), emulator seeds on create / job change / reset, level-0 rows neither loaded nor sent, ACK pages clamped to >= 1, a no-state skill slot logs once
  (`MIG no-state skill`) and `StateChange` refuses the blank state. Data: `data036` carries June's `SkillData.lua` + `NewSkillTempletVer2.lua` (ljconv) and the core `Enum.lua` (31 June `UNIT_CLASS` names,
  `SPRT_HYPER_CLASS`). JUNE_BUGS 279. **Revert:** the exe, `data036` and the save move together (K2 rule) and the save is forward-only. Comment out both flags only together with restoring the pre-S3 snapshot
  `db_backup\els_db_20261018_070707_pre_S3_skill_reform.sql` (v16, **loses everything saved since**) and the pre-S3 `data036.kom` (the copy tool's backup of the live archive, game directory); every later skill flag (SLOT_10 v18, MEMO_PACKET / PAGE v19, ...)
  needs REFORM, so they revert with it. No partial revert is supported.
- **`SERV_IRUHADEV_JUNE_SKILL_NOTE`** (S3b). June's skill-note (memo) manager is added because the US build compiles March's out (`Always_Global.h:39`): manager, Lua binders and readers, the user-tree memo multimap,
  June `31fdbbb0` / `31fdbcc0`. JUNE_BUGS 286. Needs TEMPLET. Everything memo-related (`MEMO_*`, `MEMO_PACKET`, `MEMO_RESYNC`, `LEVEL_EMBLEM`) is guarded on it. **Revert:** comment out the define (and the dependants),
  `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`.
- **`SERV_IRUHADEV_JUNE_SKILL_TYPE`** (S3c). `SKILL_TYPE` enumerators 14-18, `RESET_SKILL_ACK` clears the type 14-16 slots (June `322112d0`; unreachable offline), the `[JUNE_SKILL] TYPE` census at load.
  **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data, save or kom changes.

**Retired in Z: `SERV_IRUHADEV_JUNE_SKILL_DEBUG`.** Its define, the `#if ... JUNE_SKILL_DEBUG` blocks (41 in `X2GUUser.cpp/.h`, `X2SkillTree.cpp/.h`, `X2GameUnit.cpp`, `X2DamageEffect.cpp`, `X2EffectSet.cpp`, `X2Game.cpp`,
`X2SkillNoteManager.h`, `X2OfflineSkill.cpp`) and its `#error` guard are gone: no play-test ever showed the S3d `MEMO` lines (they need a class with a state for a memo skill), the S8 `PTR` lines cannot run offline, and
S2's march-vs-june scratch parser compare was noise since S3. Diagnostics only, so no behaviour changed. Where a revert line above says "(and `_DEBUG`)" or a log tag "with `_DEBUG`", read that as history.
Left as empty bodies (still called): `CX2SkillTree::JuneLogShortArray` / `JuneLogShortArrayReach`. S3's `MIG autolearn check` (a `_DEBUG` block) went with it; the generated table was verified by the S3 play-test (`only generated 0, only loaded 0`). `SERV_IRUHADEV_JUNE_SKILL_SLOT_10_DEBUG` and `_NUMERICAL_DEBUG` are separate flags and stay.

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

### June GFx runtime: user skill cut-in as a GFx widget (june-gfx G4, 2026-10-19)

Flags `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN` (+ `SERV_IRUHADEV_JUNE_GFX_TALK_CUTIN_DEBUG`, off since the G4 review) in `KTDXLIB/Always.h` (client only; `#error`s without `JUNE_GFX_EVENTSCENE`). Plan `JUNE_GFX_RUNTIME_PHASES.md` "Phase G4"; log tag `[JUNE_GFX] CUTIN` (ErrorLog.txt).
- **Client.** `X2GUUser.cpp`: `ActiveSkillShow( bonePos, ... )` (June's leaf `31fd95a0`) returns early with June's GFx body: the dragged item goes back (`CX2UIManager::JuneSetNowDragItemReturn`, `X2UIManager.h`),
  then `g_pMain->JunePlayCutInWidget( 0xc, <name>.gfx, facing left, 0 )`; March's `CutIn_Right/Left`, `FocusLine`, `OutLine` and `CutIn_Talk_*` particles stay below it for flag off. NPC cut-ins (`CX2GUNPC`) are untouched.
  `X2GFXEventScene.cpp`: `CX2Main::JuneMakeSimpleWidgetPlayer` (`31d70c60`; also called at the end of `X2StateStartUp.cpp`'s script load, as June's `3259bb20`) and `JunePlayCutInWidget` (`31d76b40`:
  frame + random `CUT_IN_TALK_<R_|L_><1-3>` + the cut-in). `X2Main.h/.cpp`: member `m_pJuneSimpleWidgetPlayer` (June `+0xd7c`), deleted in `~CX2Main`.
  `KTDXLIB/KTDGGFXManager.{h,cpp}`: `CKTDGGFXSimpleWidgetPlayer` (June's `CX2SimpleWidgetPlayer`: listener `simpleWidgetPlayer`, data bindings `SimpleWidgetPlayer.playSimpleWidget` /
  `...clearSimpleWidgetPlayer`), a third binding kind (an object `{ widgetID, fileName, parameter1, parameter2 }`) in G3's adapter table, and the adapter keeps the movie for it.
- **Project files:** none (no new file).
- **What changes in game:** every user skill cut-in is a GFx widget set (frame, talk balloon, cut-in art) instead of the particle cut-in. March's class tables still name `.dds` textures
  (`june2017_re/g4_cutin_names.tsv`: none of the 52 names has a June `.gfx`), so until the character ports rename them the art part of a class cut-in does not show; hyper cut-ins and the
  debug command use June names.
- **Debug (G4 review turns it off):** dev console `JuneGfxCutIn( "HQ_CutIn_Elsword_01" )` runs my unit's `ActiveSkillShow` with a June name.
- **Revert:** comment out `JUNE_GFX_TALK_CUTIN` (and `_DEBUG`), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June's upgraded-skill flag at state start; March's memo path retired (skill-system-core S3f, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_SKILL_STATE_ULTIMATE` in `KTDXLIB/Always.h` (`#error` guards: `_SKILL_STATE_BUFF` in Always.h, `ADD_MEMO_1ST_CLASS` in `X2GUUser.cpp`). Plan `JUNE_SKILL_SYSTEM_PHASES.md`
"Phase S3f"; log tag `[JUNE_SKILL] SULT` (with `SERV_IRUHADEV_JUNE_SKILL_DEBUG`, throttled).
- **Client (`X2GUUser.h/.cpp`).** `CommonStateStart` computes June's one flag at the top (`32057b80 @32057c03`: the now-state skill's equipped templet, upgrade swap, `m_iDeleteSkillIDByUpgrade > 0`) and
  passes it to `LoadEffectSetFromScript`'s new third argument (`32024ff0`; its `_ULTIMATE` lists are checklist row 397), to the `DAMAGE_DATA_ULTIMATE` / `DAMAGE_DATA` pick (`@3205c865`) and to S3e's
  `ParsingBuffFactorID`. `m_bApplyMemo` is always false (June's MemoCondition decides false, JUNE_BUGS 332), so `EFFECT_SET_LIST_MEMO` / `_EX_MEMO` are never picked; `DAMAGE_DATA_MEMO` and
  `DAMAGE_DATA_NEXT_MEMO` are no longer read (June has neither key). `SetDamageData` is June's `31fcd280`: one argument, and a negative `FORCE_DOWN` moves away from 0 by the melee rate's distance from 1.
- **What changes in game:** March class scripts' `APPLY_MEMO` states lose their memo effect sets / damage data (as in June). `DAMAGE_DATA_ULTIMATE` exists only in June's DamageEffect files, so no
  class state picks it yet.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June quest system: quest manager runtime (quest-system Q3, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_RUNTIME` in `KTDXLIB/Always.h` (needs `JUNE_QUEST_LOAD`), debug flag `SERV_IRUHADEV_JUNE_QUEST_RUNTIME_DEBUG`. Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q3 - Done"; log tag `[JUNE_QUEST] RUN`.
- **Client.** `X2QuestManager.h/.cpp`: chain quests (`m_mapChainStartable` / `m_mapChainComplete`, `IsChainQuestInProgress` / `GetChainQuestStartable` / `IsChainQuestAvailable` /
  `RebuildChainQuestStartable` / `ForgetCompletedQuest`; hooks in `SetUnitQuest`, `CreateUnitQuest`, `RemoveUnitQuest`, `RefreshDailyQuest`, `ClearUnitQuest`, `GetAvailableQuest`),
  `IsWeekQuestExpired` / `IsMonthQuestExpired` in `SetUnitQuest` (which also gets June's third argument `bClear_`), `GetStartQuestList` (class-change quests at every NPC, guild gate),
  `GetAvailableQuest` (QT_GUILD drop, chain flag), `GetBoardStartQuestList` (June's village table, early return above March's body), `IsForbiddenQuest` (QR_* tail, QT 9 code events),
  `SubQuestInst::IsComplete` (collection = flag OR count), `QuestInst::IsCompleteVisitQuest`, `MakeSubQuestString` (SQT 37 / 39 / 43, 44 and 48 in the clear-count group, the any-dungeon
  visit text), `GetClassUpQuestNoticeState` (June 32336310; no caller yet). `X2PlayGuide.*`: `StartGoEffectNewbie` (Go effect only below level 3) at the 11005-not-complete site.
- **Not ported (owners in the plan doc):** `32335f50` (story-book rebuild, new phase Q10), `32344ab0` and the class step `31edf870` (Stage 9 class-up notice UI), `31d79ac0` (Q8), the
  server-fed gates of `IsForbiddenQuest`.
- **Revert:** comment out the define (and the `_DEBUG` one), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June quest system: June's quest window (quest-system Q5a, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_UI` in `KTDXLIB/Always.h` (needs `JUNE_QUEST_LOAD`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q5a - Done"; log tag `[JUNE_QUEST] UI`.
- **Client.** New `X2Lib/X2UIQuestNewJune.cpp` (listed in `X2Lib_2010.vcxproj`): June's tabs (Request / Scenario / Event / Guild / Story), list builders, slot rows, buttons incl. skip-epic, detail title, NPC face keys. `X2UIQuestNew.cpp/.h`: dispatches, enums, members.
- **Data.** data034 `DLG_UI_NEW_Quest_List` / `_Detail` / `DLG_UI_Quest_Clear` = June's (backup `kom_backup_stage_june\20261019_115132`); data036 `Enum.lua` + 10 UQNCM ids (core list, `luasrc refresh`).
- **Review (2026-10-19).** June's developer suffix on the detail title (auth >= 2); NEW_QUEST_NOT without March's forbidden-quest give-up (`#else` of the flag in `X2UIQuestNew.cpp`). Same revert.
- **Revert:** comment out the define, restore the three DLGs from `24965799\kom_contents_bytecode\data034_kom\` (or the backup above), drop the 10 `UQNCM_*` lines from `Enum.lua` (`luasrc.py refresh`), `kom.py pack` data034 / data036, ask, copy; `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No save change.

### June quest system: story slide-show player (quest-system Q10b, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_STORY_PLAYER` in `KTDXLIB/Always.h` (`#error`s without `JUNE_QUEST_STORY`, `JUNE_QUEST_SCENE`, `JUNE_GFX_GATES`, `JUNE_RENDER_CAMERA_TRACK`,
`DUNGEONSTAGE_CONTINUOUS_BGM`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q10b - Done"; log tag `[JUNE_QUEST] STORY PLAY`.
- **Client.** New `X2Lib/X2SlideShotScripted.{h,cpp}` (June's player at `CX2Data +0x288`; listed in `X2Lib_2010.vcxproj`, inert with the flag off). `X2Data.h/.cpp`: the member, reset with the
  quest data (`ResetQuestManager`), frame move, dtor, `JuneStartStorySlideShot` / `JuneIsStorySlideShotRunning` / `JuneEndStorySlideShot`. `X2InstanceData.h/.cpp`: `m_bJuneStoryEndMessage`
  (June `+0xabc`). `X2UIQuestNew.cpp`: the complete ACK's start + end message, the Remind replay. Forced Ends: `X2Game.cpp` `Release`, `X2PartyManager.cpp` ready-check and auto-party
  popups. Gates while a show runs: `X2GameUnit.cpp` `PlaySound`, `X2Game.cpp` key input and focus camera (the one `#else` line), `X2TalkBoxManager.cpp` frame move.
- **Data.** data048 + 69 June `StoryQuest_*.dds` (new, `stage-june`); `Tutorial_New_ElswordAisha.ogg` loose in `data\music\` (June's; the other two story BGMs are March's own).
  `DLG_NewSlideShot.lua` unchanged (June's `ljconv` bytes equal March's).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. The textures and the ogg may stay (nothing reads them with the flag off). No save change.

### June quest system: story book data (quest-system Q10, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_STORY` in `KTDXLIB/Always.h` (needs `JUNE_QUEST_LOAD`, `JUNE_QUEST_RUNTIME`, `JUNE_QUEST_UI`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q10 - Done"; log tag `[JUNE_QUEST] STORY`.
- **Client.** New `X2Lib/X2SlideShotScriptedManager.{h,cpp}` (June `CX2SlideShotScriptedManager`, Lua `g_pSlideShotManager`; listed in `X2Lib_2010.vcxproj`, inert with the flag off), bound in `X2Main.cpp`
  (`AddStory`, `AddMediaInfo`), loaded by `X2Data::ResetQuestManager` and destroyed in `~CX2Data`. `X2QuestManager.h/.cpp`: `JuneRebuildStoryBookQuest` (June `32335f50`), called at the end of `SetUnitQuest`.
  `X2Data.h`: `m_mapJuneStoryBookQuest`; `X2InstanceData.h/.cpp`: `m_bJuneStoryBookNew`. `X2UIQuestNew.cpp`: the complete ACK's story rebuild; `X2UIQuestNewJune.cpp`: the story Remind button.
- **Data.** data036 `MediaInfoScript.lua` / `SlideShotScript.lua` = June's `ljconv` bytecode (new); core `Enum.lua` + June's `MEDIA_TYPE` table (`luasrc.py refresh`).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. The two Lua files and `MEDIA_TYPE` may stay (nothing reads them with the flag off). No save change.

### June quest system: region quest gates (quest-system Q6, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_GATE` in `KTDXLIB/Always.h` (no dependency). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q6 - Done"; log tag `[JUNE_QUEST] GATE`.
- **Client.** `X2LocationManager.h/.cpp`: `LocalMapTemplet::m_iRequireCompletedQuestID` (June templet `+0x70`, parser `32501bb0` @32502b6a, 0 when absent) and `IsQuestGateOpen( site )`
  (June `32322260`; logs each site / map / state once). Readers: `X2LocalMapUI.cpp` tab (`3293c6f0`), `X2DungeonManager.cpp` `IsActiveDungeon` second test (`327da190`), `X2UIInventory.cpp` and
  `X2StateMenu.cpp` warp lists (`32173120` / `324627f0`), `X2WorldMapUI.cpp` world and field map (`3225d340` / `3225d970`), `X2UIQuestNew.cpp` and `X2UIQuestReceive.cpp` quest-notice map filter (`323d4390`).
- **Data.** data035 `DungeonData.lua` (core row): AD Normal's `m_iRequireCompleteQuestID = 22218` uncommented. `DLG_Map_Local_Elysion.lua` already carries `RequireCompletedQuestID = 22218` (June's bytecode).
- **No stand-in (user, 2026-10-19):** MapData `REQUIRE_COMPLETE_QUEST_ID` / `_SUB_QUEST_ID` and BattleFieldData `NEED_ONGOING_QUEST_ID` / `BATTLE_FIELD_DIRECTION_INFO` stay verbatim and inert (June's client never read them).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`; comment out `m_iRequireCompleteQuestID = 22218,` in `DungeonData.lua` (data035), `luasrc.py refresh data035 DungeonData.lua`, pack and copy data035.
  No save change.

### June quest system: quest scenes and the clear window (quest-system Q5b, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_SCENE` in `KTDXLIB/Always.h` (needs `JUNE_QUEST_UI` and `JUNE_GFX_EVENTSCENE`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q5b - Done"; log tag `[JUNE_QUEST] SCENE`.
- **Client.** `X2UIQuestNewJune.cpp`: `CX2UIQuestNew::JunePlayQuestScene( quest, mode )` (June `323cb2e0`, row lookup `323cb190`) and `JunePlayDeferredQuestScene`. `X2UIQuestNew.cpp`: the four live `PlayEventScene( m_wstrStartScene / m_wstrEndScene )` sites
  (Remind, NEW_QUEST_NOT, QUEST_COMPLETE_ACK, ALL_COMPLETED) go to the new call; their Lua calls stay in the `#else`. `X2Main.cpp`: StateChange's village and battle-field blocks play the scene NEW_QUEST_NOT deferred in state 5
  (`X2InstanceData.h/.cpp`: `SetJuneDeferredQuestScene`). `X2QuestManager.h/.cpp`: the quest BGM (`3232df20` / `3232e000`). `X2GFXEventScene.h/.cpp`: `RegisterMent` (`3226ac80`) and the BGM restore in `End`.
- **What changes in game.** A quest with no `Quest_Event_Scene_Info` row plays no scene (June has no Lua fallback; the log names the quest). An epic quest sets the GFX scene's new-epic flag. The clear window's DLG is June's.
- **Data.** data034 `DLG_UI_NEW_Quest_Clear.lua` = June's `ljconv` bytecode (backup `kom_backup_stage_june\20261019_150631`).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`; restore the DLG from `24965799\kom_contents_bytecode\data034_kom\` (or the backup above), `kom.py pack data034`, ask, copy. No save change.

### June GFx runtime: event scenes (june-gfx G3, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_GFX_EVENTSCENE` in `KTDXLIB/Always.h` (client only; `#error`s without `JUNE_GFX_MANAGER` and `JUNE_SOUND_AUTO_RELEASE`). Plan `JUNE_GFX_RUNTIME_PHASES.md` "Phase G3"; log tag `[JUNE_GFX] SCENE`.
- **Client.** New `X2Lib/X2GFXEventScene.{h,cpp}`: `CX2GFXEventScene` (June `3226aed0`) on `CX2Data` (`GetGFXEventScene`, `ResetGFXEventScene`, `JuneIsEventScenePlaying` = `3226a010`); data from `VillageName.xml` -> 11 region
  XMLs and `GFXEventSceneInfo.xml`; Play / queue / End / IsPlaying / input / voice / `IsRecentlyPlayed`. `KTDXLIB/KTDGGFXManager.{h,cpp}`: the DataBinding wrapper and `CKTDGGFXEventSceneBridge`
  (June's scene adapter). Lua `PlayEventScene( name )` and `IsRecentlyPlayedEventScene( name )` (`X2Main::RegisterLuabind`). Readers under the flag: StateChange, MsgProc, TFieldGame unit hide, Informer,
  TalkBoxManager frame move, `StateDungeonGame::QuitGame`, `DungeonGame::Release` tail.
- **Project files** (inert with the flag off): `X2Lib/X2Lib_2010.vcxproj` lists `X2GFXEventScene.cpp` / `.h` (no include dirs); KTDXLIB unchanged.
- **What changes in game:** a quest scene named in `GFXEventSceneInfo.xml` plays as a GFx movie (scene `.swf` loose in `data\eventscene\`) instead of nothing; today only the dev console (`PlayEventScene( "Quest_22329_Start" )`)
  and the quest-system phases reach it. The black dimming layer is missing until the asset port brings June's `GameMajorParticle_INT.txt` (data036; March's copy lacks `EventScene_BlackLayOut`).
- **Revert:** comment out `JUNE_GFX_EVENTSCENE` (the dependent `JUNE_GFX_TALK_CUTIN` needs it: comment that out too), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. The scene `.swf` and koms may stay. No save change.

### June GFx runtime: JPEG 8 for Scaleform (june-gfx G3 fix 1, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_GFX_JPEG8` in `KTDXLIB/Always.h` (client only; `#error`s without `JUNE_GFX_RUNTIME`). Plan "Phase G3 - Done", "G3 fix 1".
- **Why.** The SDK's `libgfx.lib` is built on libjpeg 8d; the client links CxImage's 6b `jpeg.lib` under the same symbol names, so the first JPEG inside a `.swf` crashed.
- **Client.** `KTDXApp.cpp` links `sfjpeg.lib` before `libgfx.lib`. `KTDXLIB/sfjpeg.lib` is generated by `tools/sfjpeg.py` (the SDK's libjpeg 8d plus libgfx's `JPEG_Image*` objects with every `_jpeg_*` renamed
  `_Jpeg_*`); it is git-ignored (`*.lib`) and `tools/build.py` regenerates it when missing. CxImage keeps `jpeg.lib`.
- **Project files:** none.
- **Revert:** comment out the define (the pragma drops; the SDK then needs a different JPEG fix, so revert `JUNE_GFX_EVENTSCENE` with it: scenes with JPEGs crash otherwise), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`.

### June GFx runtime: owed gates, ESC and the dialog hide modes (june-gfx G5, 2026-10-19)

Flags `SERV_IRUHADEV_JUNE_GFX_GATES` (+ `SERV_IRUHADEV_JUNE_GFX_GATES_DEBUG`, off since the G5 review) in `KTDXLIB/Always.h` (client only; `#error`s without `JUNE_GFX_MANAGER`, `JUNE_CUTIN_SHOW_OPTION`,
`JUNE_UI_DIALOG_BANDS` and `DIALOG_SHOW_TOGGLE`). Plan `JUNE_GFX_RUNTIME_PHASES.md` "Phase G5"; log tag `[JUNE_GFX] GATE`.
- **Client.** `X2GUUser.cpp` `GetShowActiveSkillShow`: `isOpenUI( 0xb )` term (`31fd3180 @31fd3228`). `X2State.cpp` `HandleMsgByESC`: `closeToTopZorder` first and `31d03a10`'s mode 2-4 rule (`31f1e1a0 @31f1e262`).
  `KTDGUIDialogManager`: `JuneSetShow( bShow, mode )` (`31d05af0`), the five hide presets (`32597410`), the OnFrameMove / render readers, `JuneToggleHide` (`31f02f50`) at the DELETE sites in `X2State.cpp` and
  `X2StatePVPGame.cpp`. `KTDGGFXManager.{h,cpp}`: `isOpenUI` / `closeToTopZorder` callers.
- **Project files:** none.
- **What changes in game:** with the flag, DELETE and the dialog hide go through June's mode table (mode 1 = the same names as before, in June's sorted order); `isOpenUI( 0xb )` stays false until the GFx cash shop exists (Stage 9),
  and the ESC / hide modes 2-5 branches are unproven in game until Stage 9's screens. The AVI movie term `g_pMain +0x80a` is left out: backlog Stage 26. JUNE_BUGS 336.
- **Revert:** comment out `JUNE_GFX_GATES` (and `_GATES_DEBUG`), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June GFx runtime: close-out (june-gfx Z, 2026-10-19)

All `SERV_IRUHADEV_JUNE_GFX_*` flags are on, every `_DEBUG` flag is off (`JUNE_GFX_DEBUG`, `JUNE_GFX_TALK_CUTIN_DEBUG`, `JUNE_GFX_GATES_DEBUG`; off since the G2 / G4 / G5 reviews; no phase kept one on). Flag family, in
dependency order: `JUNE_GFX_RUNTIME` (G1) <- `JUNE_GFX_MANAGER` (G2) <- `JUNE_GFX_EVENTSCENE` (G3) <- `JUNE_GFX_TALK_CUTIN` (G4); `JUNE_GFX_JPEG8` (G3 fix) <- RUNTIME; `JUNE_GFX_GATES` (G5) <- MANAGER.
To switch the whole runtime off, comment out `JUNE_GFX_RUNTIME` last and every dependent before it (the `#error` guards enforce the order). Project-file hunks, all inert with the flags off: `X2/X2_2010.vcxproj` and
`KTDXLIB/KTDXLIB_2010.vcxproj` `LibraryPath` (Scaleform `Lib\Win32\Msvc10\Release`), `KTDXLIB_2010.vcxproj` (`KTDGFXRuntime`, `KTDGGFXManager`, `KTDGFXSound` with per-file Scaleform dirs), `X2Lib/X2Lib_2010.vcxproj`
(`X2GFXEventScene`). Data staged by GA (4,645 June files, 18 koms, 2,383 loose `eventscene\*.swf`) stays regardless.

### June GFx runtime: the scene cursor after the GFx layer (june-gfx G6, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_GFX_CURSOR_ORDER` in `KTDXLIB/Always.h` (client only; `#error`s without `JUNE_GFX_EVENTSCENE` and `JUNE_RENDER_STAGE_SPLIT`). Plan `JUNE_GFX_RUNTIME_PHASES.md` "Phase G6 - Done"; log tag `[JUNE_GFX] SCENE`.
- **Client.** `X2Cursor.{h,cpp}`: after-GFx mode (`SetJuneDrawAfterGFx`, `JuneOnFrameRenderAfterGFx`; the normal `OnFrameRender` calls return while it is on). `X2State.cpp` `OnFrameRenderAfter` draws it. `X2GFXEventScene.cpp` Play / End switch it on / off.
- **Project files:** none.
- **What changes in game:** during a GFx event scene the event-scene cursor is drawn over the movie (June `31be3800 @31be3c7c`) instead of under it. A stand-in for June's `CKTDXCursor` until engine Phase R4f, which retires this flag.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data or save change.

### June quest system: quest clear movies (quest-system Q8, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_MOVIE` in `KTDXLIB/Always.h` (needs `JUNE_QUEST_UI`, `REFORM_ENTRY_POINT`, `MOVIE_TEST_BASE`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q8 - Done"; log tag `[JUNE_QUEST] MOVIE`.
- **Client.** `X2Main.h/.cpp`: June's movie table and play state on `CX2Main` (`AddDungeonClearMovieTemplet_LUA` bound and `DungeonClearMovieTemplet.lua` loaded in the ctor; `JunePlayQuestClearMovie`, `JuneStartClearMovie`, `JuneStopClearMovie`, `JuneSendReservedQuestComplete`, `JuneCheckClearMovieEnd` from `OnFrameMove`). `X2UIQuestNew.cpp`: both complete requests play the quest's movie first and park the request. `X2State.h/.cpp`: `SUCM_CLOSE_DUNGEON_CLEAR_MOVIE` 3032 / `_OK_MSG` 3033 (skip box, string 30615). `X2GUUser.cpp`: no cut-ins while a movie plays. `X2StateDungeonGame.cpp`: the ctor clears the flag.
- **What changes in game.** Completing quest 22336 (Elysion) or 22236 (Atlas) plays its clear movie full screen, sound and music off, UI hidden; the reward request goes out when the movie ends or is skipped (button / ESC -> "stop the video?").
- **Data.** data036 `Enum.lua` (core source): `SUCM_CLOSE_DUNGEON_CLEAR_MOVIE*`, `SCENARIO_MOVIE_CATEGORY`. New June files (ljconv bytecode): data036 `DungeonClearMovieTemplet.lua`, data034 `DLG_Dungeon_Clear_Movie.lua` (record `june2017_re/june_adopt_20261019_174523.tsv`). Same-name rule: June's `DLG_UI_Button20.tga/.TET` (data056) and `DLG_UI_Common_Texture_GUI_FLOW_01.tga/.TET` (data057) replaced March's (backup `kom_backup_stage_june\20261019_174605`). Movie files: June's `data\movie\*.avi` copied loose into the game's `data\movie\` (user).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. Data: the two new Lua can stay (unbound binder = a Lua error in the log only; remove them from `kom_contents` and the manifests to be clean); restore the four textures from the backup above; Enum.lua lines are harmless. `kom.py pack data034 data036 data056 data057`, ask, copy. No save change.

### June quest system: the quest window's Video tab (quest-system Q8b, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_MOVIE_TAB` in `KTDXLIB/Always.h` (`#error`s without `JUNE_QUEST_MOVIE` and `JUNE_QUEST_UI`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q8b - Done"; log tag `[JUNE_QUEST] MOVIE`.
- **Client.** `X2Main.h/.cpp`: row / seen-list helpers, `JunePlayClearMovieByID`, `JuneAddSeenClearMovieByQuest`, `JuneClearSeenClearMovie`. `X2QuestManager.cpp` (end of `SetUnitQuest`), `X2Unit.cpp` (`Reset`), `X2StateMenu.cpp` (`ProcessStateChangeUnitSelectWithOptionWindow`) feed and clear the seen list. `X2UIQuestNew.h/.cpp` + `X2UIQuestNewJune.cpp`: the Video tab (`JuneOpenMovieTab`, `JuneUpdateMovieList`, `JuneGetMovieLockDesc`, play / hover), its branches in the scroll buttons, navi, wheel, `UpdateNavi`, `JuneResetQuestUI`, and the June tab-hide sites; the Q5a log-only stub is the `#else`.
- **What changes in game.** The quest window's Video button lists June's 13 scenario movies by area; seen ones (completed quest / cleared dungeon / played) show their picture and play on click, the rest are locked with a description.
- **Data.** June-only `DLG_UI_Common_Texture_ScenarioMovie_01.tga/.TET` and `_02.tga/.TET` added to data063; June's `DLG_UI_Common_Texture10.tga/.TET` replaced March's in data057 (same-name rule; backup `kom_backup_stage_june61019_210421`).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. The textures can stay (the tab stays hidden and logs its messages). No save change.

### June quest system: the quest window's MsgProc as June's (quest-system Q8b fix, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_MSGPROC` in `KTDXLIB/Always.h` (`#error`s without `JUNE_QUEST_UI`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q8b - Done".
- **Client.** `X2UIQuestNew.cpp` `MsgProc`: no QuestDetail mouse-over test (June `323e5690`); March's `FIXED_QUEST_DLG_IS_NOT_CHECK_FLAG_WHEN_NOT_OVER_MOUSE` test is the `#else`.
- **What changes in game.** The mouse wheel scrolls the quest list and the Video tab (it was dead on every list tab: June's list mode hides the detail dialog). Watch: other windows' clicks while the quest window is open (user chose June as is).
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`.

### June quest system: the random dungeon mission (quest-system RM1, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_RANDOM_MISSION` in `KTDXLIB/Always.h` **and** at the end of `KncWX2Server/Common/ServerDefine.h` (`EventID_Client.h` / `ClientPacket.h` precede `Always.h`; both are toggled together; `tools/flagcheck.py` SECOND_HOMES). `#error`s without `SERV_IRUHADEV_OFFLINE` and `SERV_IRUHADEV_JUNE_SKILL_TYPE`. Plan `JUNE_QUEST_SYSTEM_PHASES.md` "RM1 - Done"; log tag `[JUNE_QUEST] RM`. Map `june2017_re/quest_plan/rm0_map.tsv`.
- **Client.** New `X2Lib/X2RandomMission.h/.cpp` (in `stdafx.h` and `X2Lib_2010.vcxproj`): `CX2RandomMissionManager` (June singleton 337a66b4) and `CX2RandomMission`; the 0x492 handler (`CX2State`), the 0x494 ACK handler (`CX2StateDungeonGame`), `CX2Game::SendRandomMission*Cheat` and their Lua binders in `X2Main.cpp`. Raise sites: `X2ComboManager`, `X2GUNPC` (twice), `X2GUUser` (+2 members), `X2Game` (NPC die, resurrect), `X2DungeonSubStage`, `X2UIQuickSlot`, `X2GageData::UpNowMp`, nine class files (`X2GuEve`, `X2GUAdd`, `X2GUEL`, `X2GUAra`, `X2GUChung`, `X2GURaven`, `X2GULire_ElvenRanger`, `X2GUArme_VioletMage`, `X2GUElsword_SwordMan`). Life cycle: `X2DungeonGame` (start, tick, release), `X2StateDungeonGame` (end, gate, Tab), `X2State` (countdown reset), `X2UIManager::SetShowUI`.
- **Packets.** `EGS_JUNE_RANDOM_MISSION_NOT` / `_COMPLETE_REQ` / `_COMPLETE_ACK` / `_CHEAT_REQ` / `_REWARD_EXP_CHEAT_REQ` / `_REWARD_BUFF_CHEAT_REQ` (June 0x492-0x497; names are ours).
- **Emulator.** `Handlers_Quest.cpp`, `X2OfflineServer.*`, `X2OfflineBuff.h` + `Handlers_Inventory.cpp` (`ActivateBuffByCheat`). Dormant: only the cheat sets a mission.
- **What changes in game.** Nothing in normal play. With auth level 3, the console command `Slash_rm( type, goal, seconds )` sets a mission that starts with the next stage; `Slash_rmexp( n )` / `Slash_rmbuff( n )` grant EXP / a buff.
- **Data.** June `DLG_UI_Random_Mission.lua`, `DLG_UI_Random_Mission_PopUp.lua` added to data034 and **June's `GameEditCommandList.lua` replaced March's in data036** (same-name rule; +119 Slash functions, backup in `kom_backup_stage_june`); June `DLG_UI_Common_Texture83_NEW` / `84_NEW` `.tga/.TET` added to data057. Packed, copy asked.
- **Revert:** comment out both defines, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. The koms can stay (the new dialogs are never created; March's `GameEditCommandList` is in the backup folder). No save change.

### June quest system: the sub-quest "or harder difficulty" flag (quest-system Q4d, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_UPPER_DIFF` in `KTDXLIB/Always.h` (`#error`s without `JUNE_QUEST_OFFLINE`). Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q4d - Done"; log lines `QUEST    VISIT` / `FINDNPC` (they print `upperDiff=`) and the `CENSUS` rows for clear types 26 / 27.
- **Client.** `X2QuestManager.cpp` `LoadClearCondition`: reads the optional key `m_bUpperDifficulty` (default false) for `SQT_VISIT_DUNGEON`, `SQT_FIND_NPC` and `SQT_ITEM_USE`, as `CXSLQuestManager::LoadClearCondition` does (`XSLQuestManager.cpp:1318 / 1353 / 1437`). The Jan-source read of offline phase 30 (`2e63a803`, flag `SERV_IRUHADEV_OFFLINE`) was overwritten by the March snapshot and never re-applied. No `#else`: the original had no read.
- **What changes in game.** 101 of June's 136 `VISIT_DUNGEON` and 41 of its 49 `FIND_NPC` sub-quests carry the flag; they now accept the named dungeon at the same or a harder difficulty (22182 "Enter Burnt Forest" counts Expert). `ITEM_USE`: 0 of 14 carry it, no change. The emulator's `IsExistDungeonInSub` is unchanged.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data, no save change.

### June quest system: visit sub-quests flush the completed-visit list (quest-system Q4e, 2026-10-19)

Flag `SERV_IRUHADEV_JUNE_QUEST_VISIT_FLUSH` in `KTDXLIB/Always.h`. Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q4e - Done"; the proof is the absence of `QUEST    complete <id> refused` after a visit sub-quest ticks.
- **Client.** `X2State.cpp` `Handler_EGS_UPDATE_QUEST_NOT`: a completed `VISIT_FIELD` / `VISIT_DUNGEON` (in game) or `VISIT_VILLAGE` (out of game) sub-quest calls `CX2QuestManager::CompleteQuestToVisitVillage()` (the completed-visit list, one-sub-quest quests only) as June's quest-window handler `323e08c0` does, instead of sending `EGS_QUEST_COMPLETE_REQ` for the whole quest (`#else`). `X2QuestManager.cpp` `CompleteQuestToVisitVillage()`: skips quests of another server group (June `32337520`).
- **What changes in game.** No "completion requirement is not met" box when a visit sub-quest that is not the quest's last one ticks (22182 in 22162). The quest completes at its NPC as before.
- **Revert:** comment out the define, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data, no save change.

### June quest system: UPDATE_QUEST_NOT in-game progress text (quest-system Q5c, 2026-10-20)

Flags `SERV_IRUHADEV_JUNE_QUEST_NOT_CASES` and `SERV_IRUHADEV_JUNE_QUEST_NOT_NO_LINEBREAK` in `KTDXLIB/Always.h`. Plan `JUNE_QUEST_SYSTEM_PHASES.md` "Q5c - Done".
- **Client.** `X2State.cpp` `Handler_EGS_UPDATE_QUEST_NOT`, in-game switch, as June's live handler `323e08c0`: `_CASES` adds a `SQT_DUNGEON_CLEAR_COUNT` progress line (no quest-UI create) and puts the `SQT_ITEM_USE` / `SQT_SKILL_USE` / `SQT_LEARN_NEW_SKILL` lines in the `#else`. `_NO_LINEBREAK` puts the 500 px wrap of the `SQT_NPC_HUNT` line in the `#else`.
- **What changes in game.** A dungeon-clear sub-quest shows its progress line when the dungeon is cleared. The ITEM_USE line of event quest 203070 is gone. Long kill-progress lines are no longer wrapped. Quest progress itself is unchanged.
- **Revert:** comment out either define (they are independent), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data. The play-test put quest 160 into unit 20's save (backup `db_backup/els_db_20261020_080340_pre_sql.sql`).

### June character pilot: Aisha's Violet Mage / High Magician skills (pilot-char-revamp CH2a, 2026-10-21)

Flags `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` and `SERV_IRUHADEV_JUNE_CHAR_EFFECTSET_MODIFIER` in `KTDXLIB/Always.h` (SKILL needs LIVE, CORE and the modifier flag). Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH2a - Done"; log lines `[JUNE_CHAR] ARME SKILL registered ...` and `[JUNE_SKILL] NUM use ...`.
- **Client.** `X2GUArme_VioletMage.cpp` `June_SetEquippedSkillFuncHead` registers June's states for Chain Fireball, Gust Screw, Gust Storm, Lightning Bolt, Meteor Call, Binding Circle, Magic Missile, Meteor Shower (+ its loop and end states), Blizzard Shower, Chain Lightning, Teleport and Fire Road, with their upgrade ids; the `June_*` bodies sit in the CH2a block at the end of the file. A June clone that equals March's body is registered as March's member. `X2EffectSet.cpp/.h`: the effect set's June modifier list (tag 2 applied, other tags log once), used by Chain Fireball. Without the flags the skills fall back to March's `case` labels.
- **What changes in game.** A Violet Mage / High Magician / Void Princess casts the June state of these skills: June's effect sets and damage effects replace March's timed `CreateInstance` calls. Their damage now comes from June's numerical skill data (`NUMERICAL_DATA_INDEX`); the March value is raw, before the skill's power rate. Level 1 values:

| Skill (June id) | March effect = MAGIC | June effect = level-1 value x0.01 |
|---|---|---|
| Chain Fireball (22000) | FIREBALL_CHAIN = 1.88 (twice, 0.3 s and 0.8 s), FIREBALL_CHAIN_DOWN = 2.51 | ChainFireBall_Small = 5.76, ChainFireBall_Big = 5.23 |
| Gust Screw (22001) | GUST_SCREW = 3.36 | GustScrew = 4.68, hyper 4.95 |
| Gust Storm (22002) | GUST_STORM_LOW = 1.57, GUST_STORM_TOWER = 1.26 | LOW = 4.15, TOWER = 3.75 |
| Lightning Bolt (22003) | EffectSet_Aisha_Lightning_Bolt (damage inside the set) | LightningBolt_Fire_Damage = 2.43, hyper 3.11 |
| Meteor Call (22004) | METEO_CALL = 0.69, next hit 1.6 | MeteoCall = 9.61, MeteoCall_Explosion = 5.14 |
| Binding Circle (22005) | no damage | no damage (bind buff, memo 18 force-down) |
| Magic Missile (22006) | GUIDE_FIRE_BALL = 2.66 | MagicMissile = 4.9, hyper 4.9 |
| Meteor Shower (22007) | AISHA_METEO_SHOWER01 = 1.33 and O2 = 2.95 per tick | MeteoShower_Attack = 0.72 (twice per tick), MeteoShower_Explosion = 2.44 |
| Blizzard Shower (22008) | AISHA_BLIZZARD_SHOWER = 2.95, FROZEN_ATTACK = 2.95 | BlizzardShower = 0.62, _Explosion = 7.16; hyper 0.8 / 9.17 |
| Chain Lightning (22009) | DamageEffect_AHM_Chain_Lightning = 5.99 | same name = 6.5 |
| Teleport (22200) | no damage | no damage |
| Fire Road (22201) | ARME_FIRE_ROAD = 3.08 | A_AHM_FireRoad = 9.97 |

- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (and, if wanted, the modifier flag), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change. The play-test added unit 45 `ArmeHM` (High Magician, level 60, eight CH2a skills in slots 0-3 and 5-8) and re-slotted unit 44 `arishiaaa` (Meteor Call, Binding Circle and Lightning Bolt into B slots 5-7); backups `db_backup/els_db_20261021_120150_pre_manual.sql`, `..._120220_pre_createchar.sql` and the two `elsdb x` ones. Slots 4 and 9 (keys F and T) are June's transcendence slots and hold nothing.

### June character pilot: Aisha's Dark Magician / Battle Magician / Dimension Witch skills (pilot-char-revamp CH2b, 2026-10-21)

Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (CH2a's; the modifier flag `SERV_IRUHADEV_JUNE_CHAR_EFFECTSET_MODIFIER` gains tag 1). Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH2b - Done"; log lines `[JUNE_CHAR] ARME SKILL registered ...`.
- **Client.** `June_SetEquippedSkillFuncHead` registers June's states for Dark Cloud, Aging, Death Field, Hell Stone, Magical Makeup, Super Nova, Guillotine Press (+ its loop and end states), Heavy Press, Dark Road, Mana Intake and Energetic Road; the `June_*` bodies sit in the CH2b block at the end of `X2GUArme_VioletMage.cpp`. Dark Cloud, Aging, Guillotine and the Init / Start / End of Death Field and Magical Makeup are March's members (disassembly equal). `X2EffectSet.cpp`: modifier tag 1 (main-effect velocity), used by the upgraded Energetic Road. `June_EndHellStoneWindow` (CH1i) now passes the upgrade swap June passes. **Energy Drain (22213 / 22234) is not registered**: its target search calls June's `322f0e60`, which is not mapped (needs Opus); the skill keeps the default path.
- **What changes in game.** Most June states have no FrameMove of their own: the cut-in, world colour, sounds and effect sets come from June's `Arme_VioletMage.lua`. March's per-skill damage effects are replaced by June's numerical ones.

| Skill (June id) | March effect = MAGIC / PHYSIC | June effect = level-1 value x0.01 |
|---|---|---|
| Dark Cloud (22014) | DARK_CLOUD = 1.83 (hyper 1.83) | DamageEffect_SA_ADM_DarkCloud = 24.14, next hit 1.28 |
| Aging (22015) | ADM_AGING = 3.81, ADM_AGING_THROW = 0.42 | Aging_Throw = 0.71 (hyper 0.91), Aging_Explosion = 3.84 (hyper 4.91) |
| Death Field (22016) | AISHA_DEATH_FIELD_FIRST_IMPACT = 4.95, _EXPLOSION = 8.25 | Death_Field = 20.13, Death_Field_Explosion = 9.83 |
| Hell Stone (22017) | HELL_STONE = 1.99 / 2.79 (hyper the same) | Hell_Stone = 5.04, next 2.68; hyper 5.61, next 3.42 |
| Magical Makeup (22022) | no damage | no damage (transform + buff) |
| Super Nova (22023) | ARME_ACTIVE_SUPERNOVA PHYSIC = 4.88 / 9.76 | Super_Nova = 3.38, next 25.21 |
| Guillotine Press (22024) | ABM_GUILLOTINEPRESS_LAND01 = 8.31, LAND02 = 13.85 PHYSIC | Damage01 = 7.38, Damage02 = 22.17 |
| Heavy Press (22026) | AISHA_ABM_HEAVYPRESS_01 = 2.6, _02 = 7.83 PHYSIC | Heavy_Press_01 = 6.05, _02 = 7.27 |
| Dark Road (22206) | June-only | DamageEffect_A_ADM_DarkRoad = 9.97 |
| Mana Intake (22208) | ARME_MANA_INTAKE = 0 (MP intake value as power rate) | no damage (the MP intake value is the effect set's power rate) |
| Energetic Road (22211) | June-only | DamageEffect_A_ABM_EnergyRoad = 9.97 |

- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (and, if wanted, the modifier flag), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.

### June character pilot: Aisha's Energy Drain (pilot-char-revamp CH2c, 2026-10-21)

Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (CH2a's). Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH2c - Done"; log lines `[JUNE_CHAR] ARME SKILL registered A_ABM_ENERGY_DRAIN` and `[JUNE_CHAR] ARME SKILL Energy Drain SUCCESS / FAIL ( target N )`.
- **Client.** `June_SetEquippedSkillFuncHead` registers Energy Drain (22213; 22234 through its upgrade base) with June's READY / SUCCESS / FAIL states; the `June_*` bodies sit in the CH2c block at the end of `X2GUArme_VioletMage.cpp`. `X2Game.cpp` gains `JuneIsUnitInViewCone` (June `322f0e60`), the view cone READY uses in place of March's `IsFront`.
- **What changes in game.** The target must be inside June's cone (height band, 45 degrees to the side of the facing line) and inside the SCOPE_ABS_Z..Y angle band (June data -45 / 45); the range is 500 in PVP. The MEMO13 double drain is gone (June's memo check is a stub). A type 18 memo adds to the hit's force-down value. JUNE_BUGS 386 (the cone uses tan, not tan^2).

| Skill (June id) | March effect = MAGIC / PHYSIC | June effect = level-1 value x0.01 |
|---|---|---|
| Energy Drain (22213 / 22234) | no damage effect; the SUCCESS state's DAMAGE_DATA MAGIC = 0.34 x skill power rate, PVP_RATE 0.6 | DamageEffect_ABM_Enegry_Drain_Hand_Attack ("Energy Ball", PHYSIC) = 6.46, SUCCESS state ("Impact", NUMERICAL_DATA_INDEX 2, PHYSIC) = 5.85 |
| Energy Drain MP / range (22213, level 1) | SA_MP_GAIN_ON_HIT_REL 15, SA_MP_INTAKE_ABS 0.85, SA_PROJECTILE_RANGE_REL 700 | 20, 2, 700 (22234: 910) |

- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.

### June character pilot: Aisha's Elemental Master / Void Princess skills (pilot-char-revamp CH3a, 2026-10-21)

Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (CH2a's). Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH3a - Done"; log lines `[JUNE_CHAR] ARME SKILL registered ...`.
- **Client.** `June_SetEquippedSkillFuncHead` registers June's states for Blaze Step, Cyclone, Chain Burst, Plasma Cutter, Phantom Breathing, Hell Drop (+ its 9 steering states, FIRE and DYING), Dark Burst, Circle of Elements, Frost Barrage, Petit Angkor's Millions, Ruin (+ its loop and end states), Icicle Spear, Abyss Field, Circle Flame (+ state 186) and Petit Angkor, with their upgrade ids. The `June_*` bodies sit in the CH3a block at the end of `X2GUArme_VioletMage.cpp`; Inits, Phantom Breathing's EventProcess and most Circle Flame callbacks are March's members (equal bodies). **Not done (CH3a2, Opus):** Lightning Shower (22012 / 22052, unmapped `31c69af0`) and Fire Ring / Dark Ring (22224 / 22225, unmapped `31ff5440`): done in CH3a2 (next section).
- **What changes in game.** Most June states have no FrameMove: the cut-in, world colour, sounds and effect sets come from June's `Arme_VioletMage.lua`. Hell Drop always summons the memo UFO (`NUI_UFO_AISHA_DROPPER_MEMO`) and the skill state ends at the animation end; the steering states run on June's key test (the sit key can hold the control). Ruin's tick time, duration and MP cost are read from the skill at the learned level. Circle Flame's ice / energy sets are misspelled in June and play nothing (JUNE_BUGS 387).

| Skill (June id) | March effect = MAGIC | June effect = level-1 value x0.01 |
|---|---|---|
| Blaze Step (22010) | DE_ARME_BLAZE_STEP_01 / 02 / 03 = 0.54 / 0.42 / 0.42 | BlazeStep_Main = 37.07, next 18.54, next 9.27, Sub = 1.61 |
| Cyclone (22011) | AISHA_ELEMENTAL_MASTER_CYCLONE_OUTSIDE = 0.46 | same name = 2.08 |
| Chain Burst (22013) | AISHA_CHAIN_BURST_FIREBALL_BIG / SMALL = 1.18 | ChainBurst_Big / Small = 2.81 |
| Plasma Cutter (22018) | CATASTROPHE_LASER = 1.22, hyper 0.46 | Plasma_Cutter_Main / Sub = 1.08, hyper 0.83 |
| Phantom Breathing (22019) | PHANTOM_BREATHING = 1.21 | Phantom_Breathing_Attack = 2.22 |
| Hell Drop (22020) | AISHA_HELL_DROP_STONE = 4.47, EXPLOSION_INVISIBLE = 4.47 | Hell_Drop_Stone = 0.91, Hell_Drop_Invisible = 2.13 |
| Dark Burst (22021) | June-only | DarkBurst_Big / Small = 2.81 |
| Circle of Elements (22041) | June-only | Lightning 2.8, Fire 2.24, Ice 2.13 |
| Frost Barrage (22042) | June-only | Frozen_Edge_Projectile_Damage = 3.33 |
| Petit Angkor's Millions (22043) | June-only | Bite = 0.45, Explosion = 2.83 |
| Ruin (22044) | June-only | Destruction_BlackHole = 6.15, Destruction_End = 34.31 |
| Icicle Spear (22068) | June-only | Projectile = 37.42, ExplosionFX = 47.78 |
| Abyss Field (22069) | June-only | ABYSS_FIELD_Projectile = 12.91 |
| Circle Flame (22204) | ARME_SI_A_AV_CIRCLE_FLAME_1 / 2 / 3 = 1.89 | DamageEffect_A_AEM_CircleFlame = 1.38, next 4.05 |
| Petit Angkor (22209) | AISHA_SUMMON_BAT_FIRST_BITE / BITE / EXPLOSION = 0.74 | Petit_Angor_Bite / First_Bite = 1.43, Explosion = 3.3 |

- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.

### June character pilot: Aisha's Lightning Shower, Fire Ring, Dark Ring (pilot-char-revamp CH3a2, 2026-10-21)

Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (CH2a's). Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH3a2 - Done"; log lines `[JUNE_CHAR] ARME SKILL registered ...`.
- **Client.** `June_SetEquippedSkillFuncHead` gets June's cases for Lightning Shower (22012 / 22052: normal, loop 221 / 222, end 223), Fire Ring (22224) and Dark Ring (22225). New engine member `CKTDGLineMap::June_GetVerticalCrossLinePos` (June `31c69af0`, `KTDGLineMap.cpp`).
- **What changes in game.** Lightning Shower's bolts land on the ground ( no 850 drop, no +-500 facing shift ), spread ( rand x 3 - 150 ) x 4 around +-250, mirrored when facing right; a point off the line map snaps to the line under it. Its cut-in and world colour come from June's Lua. Fire / Dark Ring play their ring on the nearest enemy in a forward cone ( SA_DEGREE_X / Y / Z, range SA_SCOPE_ABS = 400 at level 1 ); with no target the cast plays no ring.

| Skill (June id) | March effect = MAGIC | June effect = level-1 value x0.01 |
|---|---|---|
| Lightning Shower (22012) | DamageEffect_Lightning_Shower = 3.22 | DamageEffect_Lightning_Shower = 7.35 |
| Fire Ring (22224) | June-only | DamageEffect_A_AEM_FireRing_Explosion = 18.98 |
| Dark Ring (22225) | June-only | DamageEffect_A_AEM_DarkRing_Explosion = 18.98 |

- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (the three skills fall back to March's cases with CH3a's), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.

### June character pilot: Aisha's Dimension Witch skills and hyper actives (pilot-char-revamp CH3b, 2026-10-21)

Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (CH2a's). Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH3b - Done"; log lines `[JUNE_CHAR] ARME SKILL registered ...`.
- **Client.** `June_SetEquippedSkillFuncHead` gets June's cases for Energy Spurt (22025), Distortion (22027), Screw Driver (22028), Impact Hammer (22029, state `AVSI_SI_SA_ADW_MORNING_STAR`), Air Stunner (22045 + END state 248), Crazy Worm (22046), Impact Zone (22070), Worm Hole (22214) and Energy Ring (22226), with their upgrade ids. The hyper actives 22600-22605 were registered earlier (CH1); their Inits `32cb8650` / `32cb89b0` are March's (preload lists equal).
- **What changes in game.** Energy Spurt, Screw Driver, Impact Zone and Crazy Worm have no FrameMove: cut-in, world colour, effect sets and sounds come from June's `Arme_VioletMage.lua`. Distortion's summon is requested by the local unit with no self-destruct call; its power rate follows skill 22027 after the upgrade swap (22065 when Enhanced; CH3b review) and it ends after that skill's SA_EFFECTIVE_TIME (`CX2Game::SetUserSummonedNPCInfo` Distortion case, `June_SetDistortionSummonInfo`). Impact Hammer's hit buff factor takes the skill's memo 0xf / 0x10 rate (new `CX2GUUser::JuneRescaleBuffFactorByMemoRate`). Air Stunner enters its END state when the animation ends on the ground. Energy Ring is Fire Ring's body with `EffectSet_A_ADW_Energy_Ring`.

| Skill (June id) | March damage | June damage = level-1 value x0.01 |
|---|---|---|
| Energy Spurt (22025) | June-only | DamageEffect_SA_ADW_Energy_Spurt = 48.56 |
| Screw Driver (22028) | DAMAGE_EFFECT_ADW_SCREWDRIVER_Main / FINISH / Sub = PHYSIC 17.64 / 0.61 / 0.61 | Main = 24.69, FINISH / Sub = 1.78 |
| Impact Hammer (22029) | DAMAGE_EFFECT_ADW_MORNING_STAR_HIT = PHYSIC 11.02 | same name = 12.46 |
| Air Stunner (22045) | June-only | DamageEffect_SA_ADW_Air_Stunner_Fire = 20.52 |
| Crazy Worm (22046) | June-only | BlackHole = 3.98, BlackHole_Explosion = 7.2 |
| Impact Zone (22070) | June-only | DamageEffect_SA_ADW_IMPACT_ZONE_Projectile = 8.13 |
| Worm Hole (22214) | DamageEffect_Worm_Hole = MAGIC 0.98 | same name = 2.2 |
| Energy Ring (22226) | June-only | DamageEffect_A_AEM_EnergyRing_Explosion = 18.98 |
| Distortion (22027) | NPC summon | NPC summon (no skill damage effect) |
| Elemental Storm (22600) | June-only | ICE 9.51, LIGHTNING 7.35, Gust_Storm 13.84 |
| Abyss Angor (22601) | June-only | ABYSS_ANGOR_BEAM = 10.99 |
| Fate Space (22602) | June-only | Attack01 / 02 / 03 = 44.4 / 59.2 / 40.96 |

- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (the skills fall back to March's cases with CH3a's), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.

### June DoAimingTarget reads the aiming unit's own line (JUNE_AIMING_TARGET_OWN_LINE, 2026-10-21)

Flag `SERV_IRUHADEV_JUNE_AIMING_TARGET_OWN_LINE` (`KTDXLIB/Always.h`). June `31ff5440` = March `CX2GUUser::DoAimingTarget` except one line: the in-front test takes the line direction of the aiming unit's own last-touch line (`X2GUUser.cpp`), March the target's.
- **Reaches:** Aisha's Fire / Dark Ring (CH3a2) and Energy Ring (CH3b), Chung's and Rena's aimed skills (every `DoAimingTarget` caller).
- **Revert:** comment out the flag, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`.

### June's skill unseal NOT: the unsealed skill is learned in the same packet (JUNE_UNSEAL_SKILL_NOT, 2026-10-21)

Flag `SERV_IRUHADEV_JUNE_UNSEAL_SKILL_NOT`, defined in `KTDXLIB/Always.h` **and** `KncWX2Server/Common/ServerDefine.h` (`ClientPacket.h` precedes `Always.h`; `SECOND_HOMES` in `tools/flagcheck.py`); `#error` guard `_JUNE_SKILL_ACQUIRE`.
- **Bug.** A secret manual (Camilla's, 270970-270973, 271311...) only marked the skill unsealed; the level grant skips sealed skills and ran again only at level-up / job change / select, so the skill showed unlocked but unlearned.
- **June.** `KEGS_UNSEAL_SKILL_NOT` is `{ int, vector<KSkillData>, vector<short> unsealed }` (reader `320dd8d0`, the KSkillData vector getter `3211b1e0` = the acquire NOT's). Handler `3220ee30`: `AddJuneAcquiredSkills` (`328924c0`), `AddSkillUnsealed` per id (`32891b40`),
  `JuneReplaceUpgradedSkillSlots` (`328df480`), tree `UpdateUI` when the skill window is shown, then the unsealed dialog for the first base id (upgrade-swapped, `328905b0`) **only when it is acquired** (`328914b0` = IsSkillAcquired, not AddSkillUnsealed as the old comment said),
  `GiveUpForbiddenQuest`, the HUD reset, and in a game my unit's `InitPassiveSkillState` (vslot 420; June has no NULL test on GetMyUnit, the port adds one).
- **Emulator (labelled stand-in, June's server rule is server-only).** `Handler_EGS_USE_ITEM_IN_INVENTORY_REQ` fills the NOT's skill list with `GrantJuneSkillsByLevel` after `UnsealSkill`; an unsealed skill whose level is not yet met comes at its level-up, as before.
  Log: `SKILL    item N unsealed skill S, NOT carries K learned skill(s)` plus the grant's `[JUNE_SKILL] ACQ grant` line. Replaces the same-day `SERV_IRUHADEV_OFFLINE_UNSEAL_GRANT` (two packets), removed.
- **Revert:** comment out both defines, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change; skills already granted stay in the save.

### Offline: a class-change item carries unsealed skills to the new job (OFFLINE_CLASS_CHANGE_UNSEAL_REMAP, 2026-10-21)

Flag `SERV_IRUHADEV_OFFLINE_CLASS_CHANGE_UNSEAL_REMAP` in `KTDXLIB/Always.h`. Labelled stand-in, chosen by the user 2026-10-21.
- **Why.** Unseals are kept by skill id and each job's sealed skill has its own id, so after a class-change item (e.g. Elemental Master -> Void Princess) the new job's counterpart stayed sealed.
  Live remaps through server-only `ClassChangeSkill.lua` (`GetClassChange_NewSkill`, `P_GSkill_Unsealed_UPD_UnitClassChange`); every copy in `KncWX2Server/ServerResource/` uses pre-2013 ids (2403...), none of June's.
- **Stand-in.** `CX2OfflineSkill::RemapUnsealedForClass`, called from the class-change item path in `Handlers_Shop.cpp` after `InitSkillTree`: an unsealed skill the new class's tree has is kept; otherwise it is replaced
  (set + `unit_skill_unsealed` row) by the first skill of the same `AddSealSkillInfo` row (June `SkillData.lua`, the manual's per-class list) that the new class's tree has; no counterpart -> kept. Switching back maps it back.
  Job advancement by quest is untouched (live does not remap there; the set already persists). Log: `SKILL    class change: unsealed skill A -> B for class C`.
- **Revert:** comment out the flag, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. Rows already remapped stay remapped.

### June character pilot: Aisha's Hell Drop and Stone Wall summons (pilot-char-revamp CH3c, 2026-10-22)

Flag `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (CH2a's). Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH3c - Done"; log lines `[JUNE_CHAR] ARME SKILL summon ...`.
- **Client.** `CX2Game::SetUserSummonedNPCInfo`: the Hell Drop UFO cases (`NUI_UFO_AISHA`, `NUI_UFO_AISHA_DROPPER_MEMO`, June `3230c1a0` cases `0x1ef` / `0x627`) call the new `June_SetHellDropSummonInfo`; March's block, which read March's `SI_A_ADM_HELL_DROP`, is the `#else`. The UFO's power rate follows skill 22020 after the upgrade swap (22059 when Enhanced), it is the unit's UFO, plays `EffectSet_SA_AVP_HellDrop_Summon_Loop` and has its move speed scaled by `SA_REMOTE_SPEED_ABS` (both UFOs; March scaled only the first). `CX2GUNPC::ScaleMoveSpeed` accepts NPC 495, 1574 and 1575 as June's `32757560` does. The Stone Wall case (`0x1f5`) adds `June_SetStoneWallSummonInfo` (`AddStoneWall( uid, 1.0 )`); that skill has no June state, so it never runs. June has no bat case in `3230c1a0`.
- **CH3c review.** The UFOs and Distortion (`0x443`) set their stats and power rate as June's `322ece70` does (`June_SetSummonStatAndPowerRate`): the skill power rate scales the five stats, the power rate is socket x buff x `AT_SPECIAL`.
- **Lua binders.** `GetWalkSpeedParam` / `GetJumpSpeedParam` on `CX2GameUnit` (June `31e4cfd0` / `31e4e020`): without them the memo UFO's script (`Ufo_Aisha_Droper_Memo.lua`) errors in every flight state and the UFO never moves.
- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (all CH2a-CH3c Aisha skill work reverts with it), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.

### June character pilot: Aisha's passives from June's InitPassiveSkillState (pilot-char-revamp CH4a, 2026-10-22)

Flags `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE` (needs LIVE and the MP text flag) and `SERV_IRUHADEV_JUNE_MP_CHANGE_TEXT` (needs `ELYSION_SCRIPT_B`) in `KTDXLIB/Always.h`. Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH4a - Done"; log line `[JUNE_CHAR] ARME SKILL passive init ( unit ... )`.
- **Client.** `CX2GUArme_VioletMage::InitPassiveSkillState` calls the new `June_InitPassiveSkillState` (June `32cecd70`): Aisha's passives are read by their June ids (22400-22411, 22415, 22022 / 22023) into the members the June-ported code reads, and the Strong Mind / Dark Deal object is `m_pJuneMpRefund`. March's body (Sprinter, Teleport-up, Freezing Point, Shadow of Disaster, `m_pStrongMindData`) is the `#else`. `June_FlushMpWithRefund` refunds through the new MP text.
- **Engine.** `CX2GameUnit::JuneUpNowMpWithText` / `JuneFrameMoveMpChangeText` (June `32278070`, `3229ecc0`): queued MP changes show as "MP +n" / "MP -n" every 0.2 s, ahead of the HP queue.
- **Save.** Test rows: unit 54 `ArmeVP2` + Dark Deal 22415, unit 53 `ArmeDW` + Advanced Teleportation 22411 (both level 1); backup `db_backup/els_db_20261022_060831_pre_sql.sql`. No schema change.
- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE` (then, if wanted, `SERV_IRUHADEV_JUNE_MP_CHANGE_TEXT`), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.

### June unit delegate lists and Aisha's Open Mind delegate (pilot-char-revamp CH4b, 2026-10-22)

Flags `SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST`, `SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST_DEBUG` (temporary) and CH4a's `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE` in `KTDXLIB/Always.h`. Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH4b - Done"; log lines `[JUNE_CHAR] ARME SKILL delegate run ( ... )`, `... passive init ( unit ...: open mind level ... )`.
- **Engine.** March already has four of June's five lists with June's runners (`322752b0` / `32275280`). New: the area-hit list `m_vecJuneDelegateProcessInAreaHit` (June `+0x66ec`), cleared in `CX2GUUser::InitPassiveSkillState` and `ClearAllDelegateProcess`, run at the end of `CX2DamageManager::AreaHit` for the attacker; and `CX2GUNPC::AttackResult` also runs the attack-result list for a `NAT_NORMAL` NPC (June `@3276a83e`). June registers nothing on the area-hit list.
- **Aisha.** `June_InitPassiveSkillState` registers `June_OpenMindAttackResult` (June `32ce9f90`) when Open Mind 22416 is learned; the callback puts the skill's buff factor on the target of a trigger-17 hit (JUNE_BUGS 397). 22416 is a transcendence passive, so it stays unregistered until Stage 6.
- **Debug.** `..._DEBUG` logs each list's first run per user / NPC and its non-empty runs once a second (`CX2GameUnit::JuneLogDelegateRun`). Off since the CH4b review (2026-10-22): the define is commented out, the code stays.
- **Revert:** comment out `SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST_DEBUG` and `SERV_IRUHADEV_JUNE_UNIT_DELEGATE_LIST` (the Open Mind half reverts with `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE`), `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.

### June character pilot: Aisha's overrides and two user passive hooks (pilot-char-revamp CH4c, 2026-10-22)

Flags `SERV_IRUHADEV_JUNE_USER_PASSIVE_HOOKS` (engine), CH4a's `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE` (now needs the hooks flag) and `SERV_IRUHADEV_JUNE_CHAR_ARME_OVERRIDE_DEBUG` (temporary; off since the CH4c review, 2026-10-22: the define is commented out, the code stays) in `KTDXLIB/Always.h`. Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH4c - Done" and "CH4c -- Review"; log lines `[JUNE_CHAR] ARME SKILL mp consume / cool time / magical staff ... / shadow death ... / cyclone upgrade / transform equip ...`.
- **Aisha.** June's bodies for `SetDamageExceptionProcess` (`32cccc70`), `GetActualMPConsume` (`32ccfe30`), a new `GetActualCoolTime` override (`32ce1cc0`), `SetSpecificValueByBuffTempletID` (`32cddec0`), `TransformStart` / `TransformEnd` (`32cc91b0` / `32cdbd00`), and the transform's equip-change queue (`32cc9950`, which CH1i had left dropping changes). March's bodies are the `#else`; the memo branches are gone because June's memo lookup always answers false.
- **Engine.** Two `CX2GUUser` virtuals, each returning 1.0 in the base. `JuneGetDefenceRateBySkill` (June vslot `0x428`) replaces March's inline Shadow Death block in `CX2GameUnit::GetFinalDamage`. `JuneGetIncreaseMpRateOnDamageReact` (vslot `0x73c`) multiplies the MP a user gains when hit, in `CX2GUUser::DamageReact`.
- **Damage numbers.** Shadow Death 22406 changes damage:
  - **March:** `fDefenceRateBySkill` = `SA_UNFIXED_DEFENSE_REL` of March's own skill id, which no save on June's trees has, so 1.0.
  - **June:** a non-special attack ignores the defence term outright, with chance `SA_EFFECTIVE_RATE` 0.05 / 0.10 / 0.15 / 0.20 (levels 1-4). Unit 52 has level 3 (15 %), unit 54 level 4 (20 %).
- **Other numbers.** None of these change damage:
  - Magical Staff 22409: physical force-down x0.9 to x0.6 (a negative force-down x1.1 to x1.4), hit MP x1.02 to x1.08, MP gained when hit physically x2.02 to x2.08 (JUNE_BUGS 399).
  - Cyclone upgrade 22051: `m_fDecreaseForceDown` 2.
  - Binding Circle 22005: cast in the air adds 5 s to the cool time.
  - Teleport: Advanced Teleportation's MP rate applies only in PvP (JUNE_BUGS 398).
- **Save.** Unit 51 `ArmeEM` carries Cyclone upgrade 22051 in slot 2, where Meteor Shower upgrade 22047 was (backup `db_backup/els_db_20261022_091244_pre_sql.sql`). No schema change.
- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_OVERRIDE_DEBUG`. Then comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE` (CH4a's half goes with it) and `SERV_IRUHADEV_JUNE_USER_PASSIVE_HOOKS`. Then `touch X2Lib/stdafx.cpp` and `python tools/build.py --deploy`. No data change.

### June character pilot: Aisha's memorize HUD (pilot-char-revamp CH4d, 2026-10-22)

Flags `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE` and `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE_DEBUG` (temporary) in `KTDXLIB/Always.h`; slot data type `SDT_JUNE_MEMORIZE` in `KTDXLIB/KTDGUISlot.h`. Plan `JUNE_CHARACTER_AISHA_PHASES.md` "CH4d - Done"; log lines `[JUNE_CHAR] ARME MEMO ...`.
- **HUD.** `CX2Game` gets June's memorize slots of the 10-slot dialog (`322deb80`, statics `322cf350`), the per-frame sparkle pass on slots a charging Aisha can memorize (`322f4740`, `Aisha_Memorise_P01`-`P05`, the `+0xb38` timer and the `+0xb28` particle map), and the memorize sign on equipped slots (`322cf790`, also at a cool-time end while charging). The gate is June's `CX2Main::JuneCanShowClassGuide` (`31d71810`); its last test reads March's own `m_bShowSkillSlot` (June UI manager `+0x89`).
- **Tree UI.** Slot data type 5 `CX2MemorizeSkillSlotData` (`328d9ac0` / `328e0f10`), the `SESAPT_MEMORIZE` picture (`328e0460` case 2), the 29059 mouse-in without June's write over the slot (JUNE_BUGS 400, user 2026-10-22), the tooltip's memorize line (`328e9160 @328e9710`), and a refresh after every slot swap (`328e0ff0`).
- **Callers.** Aisha's two stand-ins (`June_GameRefreshMemoRise`, `June_SetGameFloatB38`) now call CX2Game; the skill-page decide ACK sweeps the list (`3220f4dc`); saving the key settings refreshes the hot-key label (`31f5ea00`).
- **Revert:** comment out `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE_DEBUG`, then `SERV_IRUHADEV_JUNE_CHAR_ARME_MEMORIZE`, `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.
- **No sit on V in dungeons and battlefields** (play-test fix 1, flag `SERV_IRUHADEV_JUNE_STATE_NO_SIT_KEY`, all classes): June's `324d2e20` / `324bc8e0` have no `GA_SIT` block, so March's sit-emotion block in both `ShortCutKeyProcess` is the `#else`. Sitting by chat emotion still works. Revert: comment out the flag and rebuild.

### Dev convenience: free skills for a dev account in June-ported classes (JUNE_DEV_UNLIMITED_SKILL, 2026-10-22)

Flag `SERV_IRUHADEV_JUNE_DEV_UNLIMITED_SKILL` in `KTDXLIB/Always.h`. Not June: `CX2GUUser::JuneIsDevUnlimitedSkillUse()` is March's in-house skip of the skill cool-time / MP checks (`TOGGLE_UNLIMITED_SKILL_USE`, else `#ifndef _SERVICE_` with `XUAL_DEV`), and a ported class's June skill check (Aisha's `June_SpecialAttackLookup`) skips its block when it is true. Standing rule for every class port (`JUNE_PORT_BACKLOG_ORDER.md` "Standing rules").
- **Turn off:** define `_SERVICE_` (turns March's off too), or comment out the flag. Then `touch X2Lib/stdafx.cpp`, `python tools/build.py --deploy`. No data change.

### Offline: hyper skills consume El's Essence (OFFLINE_FINALITY_SKILL, 2026-10-22)

Flag `SERV_IRUHADEV_OFFLINE_FINALITY_SKILL` in `KTDXLIB/Always.h`. The emulator answers `EGS_USE_FINALITY_SKILL_REQ`, which used to be logged UNHANDLED and left the client's ACK pending. `Handlers_Skill.cpp` `Handler_EGS_USE_FINALITY_SKILL_REQ` follows `GSUserInventory.cpp` `IMPL_ON_FUNC( EGS_USE_FINALITY_SKILL_REQ )`:
- the event no-consume request is OK only while item 60008064 is in the inventory;
- otherwise the item must be El's Essence 130208 (`ERR_FINALITY_SKILL_00` / `_01` / `_02`), and one is consumed (`ConsumeOne`, `_03`);
- the ACK carries the updated slot.

Unlike the live server's DB ACK, which reuses `KEGS_ITEM_EXTRACT_ACK`, the emulator sends the real `KEGS_USE_FINALITY_SKILL_ACK`, with `m_bNoConsume` false. Log `SKILL    finality skill ...`.
- **Turn off:** comment out the flag, then `python tools/build.py --deploy`. The packet goes back to UNHANDLED and stones are not consumed. No data change.

### June dungeon result R2: the emulator's answers (flag `SERV_IRUHADEV_JUNE_RESULT_OFFLINE`, 2026-10-22)

- `EGS_RESULT_SUCCESS_REQ` is accepted with no body (June `324d84c0` sends it with SendID); March's struct form still reads.
- Three June-only event IDs at the end of the `EventID_Client.h` block: `EGS_JUNE_DUNGEON_RETRY_REQ` (0x648, no body), `EGS_JUNE_DUNGEON_RETRY_ACK` (0x649, `{ int, wstring }`) and `EGS_JUNE_FAST_SOLO_CLEAR_NOT` (0x1f2, no body, IGNORED in `X2OfflineIgnore.cpp`).
- Retry stand-in (user, 2026-10-22): the emulator reopens the same dungeon, difficulty, mode, loot rule and bots through `OpenDungeonGameRoom` + `SendDungeonGameStartNot` and ACKs OK. Nothing sends the REQ until R3 ports June's sender.
- Defined in `Always.h` and `ServerDefine.h` (`EventID_Client.h` / `ClientPacket.h` precede `Always.h`); `flagcheck.py` `SECOND_HOMES` has the row. Log `[JUNE_RESULT] EMU`.
- **Turn off:** comment out both defines and rebuild. The IDs vanish, March's `ReadReq` form of the success REQ is the only path.
