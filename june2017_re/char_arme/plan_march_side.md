# Aisha (Arme) March-side facts (read-only survey, 2026-10-20)

Paths relative to Trunk. KOM = `F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/`.

## 1. Class source
- `X2Lib/X2GUArme_VioletMage.cpp` 21220 lines; `.h` 1572 lines. `file`: "ISO-8859 text, CRLF, LF" = CP949, mixed line endings -> bytepatch.py only.
- Class `CX2GUArme_VioletMage`. Ctor :96, InitComponent :490, InitEffect :540, InitState :959 (cut-in set index switch :964-:999, calls InitStateID :1010, InitStateCommon :1013, InitStateCommonRandom :1022, InitPassiveSkillState :1614, CX2GUUser::InitState :1618), InitStateID :1623 (only USI_WAIT + AVSI_CHARGE_MP), InitStateCommon :1637 (state table, ~900 lines), InitPassiveSkillState :2550.
- `SetEquippedSkillFuncBySkillSlotIndex` :2842 .. end :4088 (.h:485). Shape:
  - bounds check vs `ARRAY_SIZE(_CONST_GUUSER_::aNormalSkillStateID)` (:2844); picks normal/hyper state id from `aNormalSkillStateID[ / SlotB]` and `aHyperSkillStateID[ / SlotB]` (:2847, :2852).
  - first `switch(pSkillTemplet->m_eType)` :2862: `ST_BUFF` (COMMON_BUFF, :2864), ACTIVE/SPECIAL_ACTIVE/HYPER_ACTIVE (no-op), `ST_RELATIONSHIP_SKILL` (inner `switch(eSkillID_)` :2887, one case SI_ETC_WS_COMMON_LOVE), `default: return` :2903.
  - second `switch(eSkillID_)` :2912 with `default:` :2914 (= AVSI_SI_AV_POWER_ATTACK1 fallback) and **57 `case CX2SkillTree::SI_*` labels** (counted textually, includes both arms of `#ifdef UPGRADE_SKILL_SYSTEM_2013` pairs, so ~45 distinct skills live). Last cases SI_HA_AEM_ELEMENTAL_STORM / SI_HA_AVP_ABYSS_ANGOR :4061 / SI_HA_ADW_FATE_SPACE :4074 (FINALITY_SKILL_SYSTEM block :4046-:4085).
  - skill names present: A_AV_CHAIN_FIRE_BALL, BINDING_CIRCLE, GUST_SCREW, LIGHTNING_BOLT, GUST_STORM, METEO_CALL, A_AEM_CHAIN_BURST, SA_AHM_MAGIC_MISSILE, INFERNAL_WAVE, A/SA_ADM_HELL_STONE, DARK_CLOUD, PLASMA_CUTTER, A_AV_TELEPORT, CIRCLE_FLAME, AHM_BLIZZARDSHOWER, ADM_DEATHFIELD, MANA_INTAKE, AHM_BLAZE_STEP, ADM_AGING, AEM_STONE_WALL, AVP_SUMMON_BAT(_TIME_BOMB/_HEAVY_DOLL), CYCLONE, HELL_DROP, METEOR_SHOWER, AVP_PHANTOM_BREATHING_DARK_FALL/_HOLE, AV_FIRE_ROAD, ABM_* (ENERGY_DRAIN, HEAVY_PRESS, ENERGY_SPURT, GUILLOTINE_PRESS, SUPER_NOVA, MAGICAL_MAKEUP, VITAL_DRAIN), ADW_* (DISTORTION, IMPACT_HAMMER, MORNING_STAR, SCREWDRIVER*, WORM_HOLE), AV_ICICLE_WAVE, ICE_STORM, AEM_AQUA_SHOWER, LIGHTNING_SHOWER, AHM_CHAIN_LIGHTNING. No June-named skills (e.g. no Ain-style new ones); all March SI_ ids.
  - Heavy `#ifdef`s inside: UPGRADE_SKILL_SYSTEM_2013 (live, Always.h:1643), BALANCE_PATCH_20131107, SERV_SKILL_NOTE, SKILL_30_TEST, SKILL_CASH_10_TEST, AISHA_SECOND_CLASS_CHANGE (:3315-:3636, Always.h:2784), NEW_SKILL_2010_11, SERV_ADD_ARME_BATTLE_MAGICIAN (:3639-:3757; ServerDefine.h:1538), SERV_ARME_DIMENSION_WITCH (:3759; ServerDefine.h:3541), ADD_MEMO_1ST_CLASS (:3930, :3939; Always.h:554), FINALITY_SKILL_SYSTEM (Always.h:1434). Verify branch with cl /EP before editing.
- Slot 5 / "pressed index" tables: `_CONST_GUUSER_::aNormalSkillStateID / aHyperSkillStateID / ...SlotB` in `X2Lib/X2GUUser.h:5008-5025`, each already `#ifdef SERV_IRUHADEV_JUNE_SKILL_SLOT_10` (5 entries) `#else` (4). `EQUIPPED_SKILL_SLOT_COUNT` = 5 in `KncWX2Server/Common/CommonPacket.h:1096` (else 4 :1098; macro :6472/:6474).
- `SpecialAttackEventProcess` :4931 (.h:503). Calls `CommonSpecialAttackEventProcess(tree, pSkillSlotData, iPressedSkillSlotIndex)` :4948 (defined `X2GUUser.cpp:25439`). In X2GUUser.cpp the pressed index is assigned only for A-keys 0..3 (:25465/75/85/95) and B-keys Q/W/E/R 4..7 (:25523/33/43/53). **The June slot-5 branches (`oneSkill5` :25500 and `oneSkillEx5` :25505, flag SKILL_SLOT_10) return the slot data but never set iSkillSlotIndexPressed_, so it stays INVALID (-1) in Aisha's code.** Aisha-side pressed-index decodes (all assume 4 per set, i.e. B = index>3, B state = index-4):
  - `X2GUArme_VioletMage.cpp:5232/5234` (`> 3`, `aNormalSkillStateIDSlotB[idx-4]`), `:5238` (A normal), `:5243/5245` (hyper B), `:5249` (hyper A)
  - `:5072` `CountUsedSkill( iPressedSkillSlotIndex )`; `:5255` `m_eSpecialAttackKeyPressed = idx+1`; `:5257` `m_iNowSpecialAttack = idx+1`; consumer `:16610 switch(m_eSpecialAttackKeyPressed)`
  - base: `X2GUUser.cpp:41536-41556` `GetPressedSkillState` (`> 3`, `-4`; only Ara calls it, `X2GUAra.cpp:3699`).
  - `:14540-14551` `GetSkillSlotIndexUsed(iSkillSlotIndex, bSlotB)` (Meditation/skill-slot lookup), `:371-405` loop over slots A and B for SI_A_AHM_MEDITATION.
  - SpecialAttackEventProcess body: pre-checks :4933-4960; `CheckSkillUsePVPOfficial` ~:4976; cooldown `pSkillSlotData->m_fCoolTimeLeft` :5009; cool-time/mission hooks :5219-:5227; state change :5230-5252; ~28 state-event handlers call `SpecialAttackEventProcess()` (e.g. :5725 ... :20512).
- `SetDamageExceptionProcess` :19822 (.h:525): calls base then only the ABM Fitness (SI_P_ABM_FITNESS) force-down/MP-gain scaling, under SERV_ADD_ARME_BATTLE_MAGICIAN. No June hooks in it.
- Script load: class has no own Lua-load; the unit script name comes from `X2UnitManager.cpp:411` (`L"ARME_VIOLETMAGE.LUA"`, array index = unit type) and `X2GUUser.cpp:41521` (`SetScriptFileName`, REALTIME_SCRIPT_PATCH). Also `X2SquareUnit.cpp:4167` `SQUARE_Arme_VioletMage.lua`. State tables are looked up by name via `m_LuaManager.MakeTableReference("AVSI_...", stateID)`.
- memorize: `grep -i memoriz` in the .cpp/.h = **no hits**. The "memo" system is flag `ADD_MEMO_1ST_CLASS` (:3930, :3939, :4931+ `GetSkillUseCondition`) plus `SI_A_AHM_MEDITATION` memo (cooldown reduce) at :371-:417.
- Existing `SERV_IRUHADEV_` flags in these files (previous phases' hooks; .h has none):
  - SERV_IRUHADEV_JUNE_BUFF_QUICKSLOT_ITEM :4933-:4938
  - SERV_IRUHADEV_JUNE_BUFF_RIDING_KEY_WAIT :4978-:4983
  - SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL :5219-:5223 (ApplySkillCoolTimeRateByBuff)
  - SERV_IRUHADEV_JUNE_RANDOM_MISSION :5224-:5227 (OnSkillUse)
  - SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY :5485-:5502 (Teleport MP override dropped)
  - SERV_IRUHADEV_JUNE_SKILL_STATE_PTR :11299-:11303, :11362-:11366 (`m_itNowStateData->second.m_SPLevel` vs `m_NowStateData.m_SPLevel`)
  Count of SERV_IRUHADEV_JUNE_SKILL_SLOT_10 in the Arme .cpp: 0 (it is handled only in X2GUUser).
- Cut-in index selection in InitState: UC_ARME_VIOLET_MAGE/HIGH/DARK/BATTLE -> set 0; ELEMENTAL_MASTER -> 1; VOID_PRINCESS -> 2; DIMENSION_WITCH -> 3 (:964-:987); SubIndex 1 HM, 2 DM, else 0.

## 2. Lua files
- Class script: `KOM data036_kom/Arme_VioletMage.lua`, 136603 B, header `1B 4B 4C 81` = ljconv'd/March LuaJIT bytecode (not BOM source). Looked up as "ARME_VIOLETMAGE.LUA" (case-insensitive).
- `tools/data/lua_core_list.tsv`: **not present** (0 hits for arme/aisha). `tools/data/lua_source_ledger.tsv`: **not present**. Core list data036 members: Enum, DungeonEnum, DamageEffect, EffectSet, BattleFieldData, MapData, ShopItem. So an edit of Arme_VioletMage.lua needs `luasrc.py core --add` + `adopt-june --source` / convert first (the effects it loads are DamageEffect/EffectSet = already core-list BOM source).
- Other Aisha-named Lua (name/kom/size, all `1B4B4C81` bytecode): data034 DLG_Class_Change_Aisha_DW 1969, _EM 2003, _VP 1982, DLG_Class_Change_Guide_Aisha 2734, DLG_PVP_Game_Damaged_Arme 989 (+_DM, _HM 989), DLG_UI_JUMPING_EVENT_AISHA 3940, DLG_Unit_Create_ClassChange_Button_Arme 5022, DLG_Unit_Select_Name_Arme 694, DLG_Unit_Select_Slot_Arme 2981; data035 Dungeon_Battle_Field_Tutorial_Aisha 4458, Dungeon_ElTutorial_arme 33481, Dungeon_Training_arme_0..1 / Arme_2..6 (14272,10257,13243,2089,7896,7804,10564), Dungeon_Tutorial_Aisha 13695; data037 NPC_Arme_VioletMage 2527, CSM_Aisha_Dimensionwitch 2640, _Elementalmaster 2456, _Voidprincess 3254, PVP_BOT_AISHA 18, Npc_Aisha_01..05 (~47.5k each), Aisha_PigBat 3811, Aisha_StoneWall 5527, Ufo_Aisha 4983, Ufo_Aisha_Droper_Memo 9880, Bloody_Aisha_Gliter(+_Boss/_Hell) ~27.6k, HAMEL_SHADOW_AISHA 18909 (+_ELIT 21993), RAID_SHADOW_AISHA 20973, SECRET_MAD_SHADOW_AISHA 18436, TIME_AISHA1/2/3 (24473/24264/24152), TIME_AISHA_SUMMON 4110, APINK_ANGEL_02_AISHA 15052, Tutorial_Aisha_* (30185,11398,9466), Tutotial_Aisha_Dp1 4023; datai016 Event_Doppelganger_Arme 21882, EVENT_*_AISHA* (5 files ~19-22k). Others (skills' DamageEffect/EffectSet/ComboTree/state tables) are not Aisha-named; Aisha state tables (AVSI_*) live inside Arme_VioletMage.lua.

## 3. Mesh / motion files (KOM)
- Motion (all data033_kom unless noted): Motion_Arme.x 3442962; _BM 2009215; _Common 2005409; _DM 1505385; _DW 1792867; _EM 1625101; _HM 1675220; _VP 1576895; _Lobby 1597467; _Field 232673; _Emotion 1253531; Emotion_APINK 937093 / _APINK_LOVE 272847 / _BIGBANG 1020541 / _CRAYONPOP_Sorted 682288 / _DeepBow 412712. Other: data012 Motion_Aisha_Buff_NPC.X 426559; data111 MOTION_HAMEL_SUMMON_DUMMY_ARME.x 1344451; data165 Motion_TUTORIAL_Aisha_DP1.X 1816178; data225 Motion_Time_Aisha.x 2097905 (+_Weapon 10210); datai027 Motion_Event_Halloween_Ginger_Aisha.x 1332016.
- data002_kom effects: ArmeBasicEff.x 322860, Arme_ComboX3Up-1/2/3.X (1722/1925/1909), Arme_Evasion.X 11677, Arme_EvasionAttack.X 1981, Arme_FireballB.X 6488, Arme_FireballS.X 6364, Arme_SpecialAttack_LightningBolts2a-0.X 1974, Arme_Thunder_Attack_Dummy.X 918, Mesh_PC_Arme_Stand.X 26366. (Referenced by Arme class cpp :4373-:4389.)
- data015_kom: ~hundreds of Mesh_Arme_* equipment .X / .xet (Basic, AVP, AEM, ADW, BM class meshes, cash, ED sets); 57 Lua / 1405 .x total match arme|aisha|violet across koms.

## 4. Offline emulator (X2Lib/Offline/)
- Aisha ids only appear in tables: `X2OfflineClassChangeSeed.h` (items 214430/214440/217920/217930/241980..242180/242320/252650 -> classes 14,15,104,105,23,113; parent table :258 {2,2}...{113,23}; tier list :313), `X2OfflineLuaEnumSeed.h:63-107` (UC_ARME_* values 2/14/15/23/104/105/113), `Handlers_Room.cpp:121` (UC_ARME_VIOLET_MAGE -> "Lime", AI-party names).
- Job change handlers (both **class-generic**, no per-class branch):
  1. Quest route `Handlers_Quest.cpp:555-590`: `GetChangeUnitClass(questID)` (X2OfflineQuest.cpp:3320, QT_CHANGE_JOB) -> `SaveUnitClass` -> `CX2OfflineSkill::Load` -> `SeedJuneAutoLearn(uid, newClass, level)` :576 (flag SERV_IRUHADEV_JUNE_SKILL_REFORM_AUTOLEARN, else SeedDefaultSkills) -> `PushJuneSkillGrant(...)` under SKILL_ACQUIRE.
  2. Cash-ticket route `Handlers_Shop.cpp:~400-795` (EGS_BILL_GET_PURCHASED_CASH_ITEM -> `EGS_BUY_UNIT_CLASS_CHANGE_NOT`, :749-793; `m_sNewUnitClass` under SERV_IRUHADEV_JUNE_SKILL_CLASS_CHANGE; `GrantJuneSkillsByLevel` + `GetJuneAcquiredSkillData`).
  - Also `Handlers_Unit.cpp:278` (char create) and `X2OfflineSkill.cpp:628` (InitSkillTree) call `SeedJuneAutoLearn`. Impl `X2OfflineSkill.cpp:789`; data `X2OfflineJuneSkillAutoLearn.h` rows {class, skill, reqLevel, type}: Aisha class rows = 2:6, 14:12, 15:12, 23:12, 104:22, 105:22, 113:22 (by text-regex count of `{class,...}` rows; header says 1347 rows/86 classes, GENERATED by june2017_re/s3_autolearn_gen.py).

## 5. StatTable
`KncWX2Server/ServerResource/US/StatTable.lua` (UTF-8 BOM, CRLF, 633758 B): 49 `ReserveMemory` + 4950 `SetUnitStat` = 50 classes x 99. Classes 2, 14, 15, 23, 104, 105, 113 each have **exactly 99 rows, levels 1..99, no gaps**.

## 6. Cut-in tables
`X2Lib/X2GUArme_VioletMage.cpp:10` `static const CX2GUUser::SkillCutInSet s_SkillCutInSet[]` (3 SkillCutIn each): [0] HQ_CutIn_Aisha_1/2/3.dds (VM/HM/DM/BM); [1] HQ_CutIn_Aisha_EM_1.dds x3; [2] HQ_CutIn_Aisha_VP_1.dds x3; [3] (SERV_ARME_DIMENSION_WITCH) HQ_CutIn_Aisha_DW_1.dds x3. Plus `s_SkillCutInSetFoolsDay` :6 (HQ_CutIn_Aisha_Gura.dds, SERV_APRIL_FOOLS_DAY). Use site `ActiveSkillShow(...)` :19015-:19025. Index chosen in InitState :964-:987 (m_iSkillCutInSetIndex).
(Other classes: X2GUAdd.cpp:92, X2GUAra.cpp:36, etc. each have their own static table.)

## 7. X2Unit.h UNIT_CLASS (`X2Lib/X2Unit.h:37`)
:43 `UC_ARME_VIOLET_MAGE, // 2`; :58 `UC_ARME_HIGH_MAGICIAN, // 14`; :59 `UC_ARME_DARK_MAGICIAN, // 15`; :69-70 `#ifdef SERV_ADD_ARME_BATTLE_MAGICIAN` `UC_ARME_BATTLE_MAGICIAN, // 23`; :104 `UC_ARME_ELEMENTAL_MASTER, // 104`; :105 `UC_ARME_VOID_PRINCESS, // 105`; :118-119 `#ifdef SERV_ARME_DIMENSION_WITCH` `UC_ARME_DIMENSION_WITCH, // 113`. UT_ARME at :24. Both flags are defined in `KncWX2Server/Common/ServerDefine.h` (:1538, :3541).

## Not found / caveats
- No "memorize" code in Aisha class (see section 1).
- Arme_VioletMage.lua origin (March vs June ljconv) not determined; compare with `24965799\kom_contents_bytecode\data036_kom\` snapshot if needed.
- Case-label count 57 is textual (both arms of #ifdef pairs).
