# Chung March-side facts (read-only survey, 2026-10-31)

Paths relative to Trunk. KOM = `F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/`.
Flag state method: uncommented line-start `#define` in `KTDXLIB/Always.h`, `KncWX2Server/Common/ServerDefine.h`, `KTDXLIB/*.h`, `X2Lib/*.h`; no `cl /EP` probe run (nested #if not resolved).

## 1. Class source
- `X2Lib/X2GUChung.cpp` 24461 lines (CP949, pure CRLF -> bytepatch.py only); `X2Lib/X2GUChung.h` 1946 lines (ISO-8859, CRLF+LF mixed). Class `CX2GUChung : public CX2GUUser` (.h:10).
- UT_CHUNG = 6 (`X2Unit.h:28`, follows UT_EVE :27; value by position). Script `CHUNG_IRON_CANNON.lua` (`X2UnitManager.cpp:415` array, `X2GUUser.cpp:44633`). Square scripts `X2SquareUnit.cpp:4247-4291` (SQUARE_Chung_Iron_Cannon / Fury_Guardian / Shooting_Guardian / Shelling_Guardian / Iron_Paladin / Deadly_Chaser / TacticalTrooper). No `SQUARE_Chung*` in KOM (find maxdepth 2): not found.
- Landmarks in X2GUChung.cpp: s_SkillCutInSetFoolsDay :60, s_SkillCutInSet :63-120, ctor :131, InitStateID :769, InitState :779 (cut-in switch on GetUnitClass :781-838, ends :884), InitStateCommon :890, SetEquippedSkillFuncBySkillSlotIndex :3184-4831, SetEquippedSkillLevel :4838, InitComponent :5181, SpecialAttackEventProcess :5815 (next fn SpecialAttackNoStageChange :6105), ActiveSkillShow use :6438-6446.
- SetEquippedSkillFuncBySkillSlotIndex: 79 textual `case CX2SkillTree::SI_*` labels; first `SI_ETC_WS_COMMON_LOVE` :3231, last `SI_HA_CTT_SATELITE_RAIN` :4812. `switch(m_eType)` :3204, inner `switch(eSkillID_)` :3229, main `switch(eSkillID_)` :3255, end :4830.

## 2. UNIT_CLASS (`X2Lib/X2Unit.h`)
- :52 `UC_CHUNG_IRON_CANNON // 6` (base); :69 `UC_CHUNG_FURY_GUARDIAN // 20`; :70 `UC_CHUNG_SHOOTING_GUARDIAN // 21` (unconditional)
- :87-88 `#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN  UC_CHUNG_SHELLING_GUARDIAN = 27` (flag live, ServerDefine.h:1746)
- :115-117 `#ifdef CHUNG_SECOND_CLASS_CHANGE  UC_CHUNG_IRON_PALADIN // 110, UC_CHUNG_DEADLY_CHASER // 111` (live, Always.h:2499)
- :135-136 `#ifdef SERV_CHUNG_TACTICAL_TROOPER  UC_CHUNG_TACTICAL_TROOPER // 117` (live, ServerDefine.h:3545)
- NEW_CHARACTER_CHUNG (Always.h:3306) and CHUNG_FIRST_CLASS_CHANGE (:3307) live. `X2UnitManager.cpp` UnitClassNameMap: no Chung rows found (only Chung hit in the file is :415). Other UC_CHUNG_* case lists (class-change chains, first-class mapping) are in `X2Lib/X2Unit.cpp`-area lines ~1882-1990 and ~2540-2610 from an unlabeled grep; file not confirmed, recheck before citing.

## 3. Feature flags (X2GUChung.cpp/.h)
- Defined (live `#define` found): CHUNG_SECOND_CLASS_CHANGE (66 `#ifdef` + 5 commented variants, 2 `#ifndef`), SERV_ADD_CHUNG_SHELLING_GUARDIAN (36), SERV_CHUNG_TACTICAL_TROOPER (33+2+1), CHUNG_FIRST_CLASS_CHANGE (5), BALANCE_DEADLY_CHASER_20130214 (28 ifdef, 7 ifndef), UPGRADE_SKILL_SYSTEM_2013 (~100), SKILL_CANCEL_BY_HYPER_MODE (42), FINALITY_SKILL_SYSTEM (8), BALANCE_PATCH_20131107 (21), BALANCE_PATCH_20120329 (19), SKILL_BALANCE_20110728 (22), BALANCE_PATCH_20110303 (5), RIDING_SYSTEM (14), ADDITIONAL_MEMO (14), ADD_MEMO_1ST_CLASS (11), FIX_CANNON_BALL_SYNC_CALCULATION (2), X2OPTIMIZE_* (205 NPC adaptive frame move), FIX_CHUNG_GAGE_UI_UPDATE_BUG (Always.h:912).
- No live `#define` found (dead or compiler-set): ALWAYS_SCREEN_SHOT_TEST (4), SERV_APRIL_FOOLS_DAY (3), SERV_BALANCE_FINALITY_SKILL_EVENT (3), SERV_SKILL_USE_SUBQUEST, TOGGLE_UNLIMITED_SKILL_USE, `_SERVICE_` (known undefined).
- DORMANT 2014 SYSTEM: none found. Commented `#define` with CHUNG/2014 in Always.h / ServerDefine.h: only `FIX_ADD_EVE_SYSTEM_2014` (Always.h:11) and `ADD_EVE_SYSTEM_2014` (:58), both Eve. No "2014" text in X2GUChung.cpp/.h.
- Existing SERV_IRUHADEV_ flags in Chung files: JUNE_BUFF_NEW_FLAG (.cpp :455-467, :626-630, :663-667; .h :762-764), JUNE_BUFF_QUICKSLOT_ITEM :5817, JUNE_BUFF_RIDING_KEY_WAIT :5858, JUNE_BUFF_NEW_SKILL :6044, JUNE_RANDOM_MISSION :6049, JUNE_SKILL_STATE_PTR x11 pairs (:13138 .. :19547, with #else). Absent: JUNE_SKILL_SLOT_10, BUFF_BEHAVIOR_PARITY.
- Chung/CHUNG mention line counts per file (all flags, not dormant-only): X2GUUser.cpp 203, X2GUUser.h 44, X2GageUI.cpp 66, X2GageManager.cpp 32, X2DamageManager.cpp 30, X2DamageManager.h 42, X2GUNPC.cpp 49.

## 4. Class resource
- `ChangeMaxCharacterAddResource`: base no-op `X2GameUnit.h:3790`; Chung override decl `X2GUChung.h:763`, def `X2GUChung.cpp:461` under SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG (June 32bf4880, buff Phase 7b, BBT 97: adds (int)fRate cannon balls unless IsFullHyperMode). Already the ported June function. Caller `X2BuffBehaviorTemplet.cpp:5520-5528`.
- Gauge: `CX2ChungMyGageUI : CX2MyGageUI` `X2GageUI.h:406`; `CX2ChungGageData` (used `X2GUChung.h:577`; its home not located by grep in X2GageManager.h); `CCannonBallUI` X2GageManager.h:555 inside NEW_CHARACTER_CHUNG block :552-629; `PGUWO_ORB_CHUNG` X2GageUI.h:49.

## 5. Cut-ins (`X2GUChung.cpp:63-120`, 3 SkillCutIn each)
- [0] Iron Cannon (and UC_NONE): HQ_CutIn_Chung_normal.dds / _Rage.dds / _Rage.dds. [1] Fury Guardian, [2] Shooting Guardian: same files. [3] Iron Paladin: DDS_Chung_CIP_Skill_Normal/_Rage. [4] Deadly Chaser: DDS_Chung_CDC_Skill_Normal/_Rage. [5] Shelling Guardian: HQ_CutIn_Chung_normal/_Rage. [6] Tactical Trooper: DDS_Chung_CTT_Skill_Normal/_Rage.
- InitState (:779-838) assigns m_iSkillCutInSetIndex: UC_NONE 0, Iron Cannon 0, Fury 1, Shooting 2, Shelling 5, Paladin 3, Chaser 4, TT 6 (:786-832). FoolsDay `HQ_CutIn_Chung_Gura.dds` :60.

## 6. SKILL_ID (`X2Lib/X2SkillTree.h`)
- Chung ids at lines 536-1925 mixed with other classes; line-start counts by prefix. A: CC 6, CFG 5, CHG 7, CSG 8, CDC 6, CIP 5, CTT 5, CHUNG 4. SA: CC 4, CFG 10, CHG 8, CSG 8, CDC 5, CIP 6, CTT 7, CHUNG 6. P: CFG 8, CHG 6, CSG 5, CDC 7, CIP 4, CTT 4, CHUNG 2. HA: CDC 1, CIP 1, CTT 1. About 139 plus shared COMMON ids. Anchors: SI_SA_CHUNG_BRUTAL_SWING = 4000 (:1210), SI_SA_CC_LUNATIC_BLOW = 6000 (:536), SI_A_CIP_ARTILLERY_NOVA = 6210 (:572), SI_SA_CTT_DREAD_CHASE = 6020 (:556).
- RNSI_*: style exists (Aisha :1559); `RNSI_P_CIP_IRON_WILL = 26402` (:1925) is the one Chung-class RNSI hit seen; full Chung RNSI set not enumerated.

## 7. Pressed-index / slot decode (4+4 model)
- No JUNE_SKILL_SLOT_10 flag in Chung. `SpecialAttackEventProcess` :5815; `iPressedSkillSlotIndex` :5826 -> `CommonSpecialAttackEventProcess` :5832; decode `>3` / `-4`: :5923-5924, :6064-6081 (aNormal/aHyperSkillStateID[SlotB]); `CountUsedSkill` :5979; `m_eSpecialAttackKeyPressed = idx+1` :6087; `m_iNowSpecialAttack = idx+1` :6090; `InitCurrentSkillKeyManager(m_iNowSpecialAttack-1)` :22981, :23478; `switch(m_eSpecialAttackKeyPressed)` :17264. No `GetSlotIndexAndSlotB` or `EQUIPPED_SKILL_SLOT_COUNT` in the file. InitComponent (:5181) loops not inspected.

## 8. Lua
- `data036_kom/Chung_Iron_Cannon.lua` and `CHUNG_IRON_CANNON.lua` (204340 B, header `1B 4B 4C 81` bytecode). Core list: only the Enum.lua row mentions Chung; no Chung class script row. Ledger not checked.
- `data036_kom/UnitTemplet.lua` (core-list BOM source): 7 `UT_CHUNG` rows.
- `KncWX2Server/ServerResource/US/StatTable.lua`: classes 6, 20, 21, 27, 117 each exactly 99 `SetUnitStat` rows. Classes 110 and 111 have 118 rows each: levels 81-99 appear twice (levels 1..99 all present; first 110 row :2906; cause not checked).

## 9. Offline emulator
- Seed tables only: `X2OfflineClassChangeSeed.h` (tickets 229460/229470/235120/235130, 242100/242110/242120/242280/242290/242300/242360, 252690; parent table :262-301: 20,21,27 -> 6; 110 -> 20; 111 -> 21; 117 -> 27; tier list :317 has 27), `X2OfflineLuaEnumSeed.h:67-111`, `Handlers_Room.cpp:125` (AI party "Speka", IRON_CANNON), `X2OfflineQuest.cpp:901` (DI_TUTORIAL_CHUNG), `X2OfflineInventory.cpp:1207` (comment). No Chung logic.

## 10. Save (`elsdb.py q "select ... from unit"`)
- uid 18 Freils (117 TACTICAL_TROOPER, lv99, transcendence 0); uid 42 RapidT (111, lv70, 0); uid 43 NovaT (110, lv70, 0); uid 50 Chung99 (111, lv99, 0). No class 6, 20, 21 or 27 unit.

## Not found
- Dormant Chung 2014 flag; SQUARE_Chung* in KOM; Chung rows in UnitClassNameMap; origin of Chung_Iron_Cannon.lua; CX2ChungGageData definition site.
