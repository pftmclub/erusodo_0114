# Chung (Iron Cannon) June-side facts. Read-only collection, 2026-10-31. Paths relative to Trunk unless absolute. Model: june2017_re/char_eve/plan_june_side.md

## 1. june2017_re/s3_reform_ids_no_state.tsv (2045 lines incl. header; columns as Eve's section 1)
Chung rows = family file X2GUChung.cpp, **261 rows** (same total as Eve), none outside it for UC_CHUNG_*. Lines (each block contiguous): UC6 91-105 | UC20 428-456 | UC21 457-485 | UC27 632-660 | UC110 1348-1400 | UC111 1401-1453 | UC117 1720-1772.
- verdict: 254 `no_state`, 7 `has_state` (all id 900 SI_ETC_WS_COMMON_LOVE, type 12, once per class; L95, 432, 461, 636, 1352, 1405, 1724).
- ids: **127 distinct** (106, 900, 26000..26064 [26030 and 26052 absent], 26200..26238, 26400..26420, 26600..26605). Bands: 26000s, 26200s, 26400s, 26600s. 3rd jobs (UC >= 150): none (max UC value in the tsv is 122).
- **49 rows have skill_name `not found`** (no June SKILL_ID name; checklist L728: Chung upgrade ids 26027-26029, 26216-26219 and others, name them from the templet).
- Rows per class (UC value): 6 Iron Cannon 15 | 20 Fury Guardian 29 | 21 Shooting Guardian 29 | 27 Shelling Guardian 29 | 110 Iron Paladin 53 | 111 Deadly Chaser 53 | 117 Tactical Trooper 53.
- skill_type counts per class {type:n}:
  - 6: {6:8, 14:2, 15:2, 16:2, 12:1}
  - 20/21/27: {6:12, 14:6, 15:4, 16:4, 8:2, 12:1}
  - 110/111/117: {6:17, 14:9, 15:9, 8:7, 16:7, 13:2, 12:1, 1:1}
  - totals: 6=95, 14=47, 15=41, 16=35, 8=27, 12=7, 13=6, 1=3 (sum 261)
- Chung total with no state = **254** (Eve 254). Samples: L91 26000 RNSI_SA_CC_SCARE_CHASE t14; L92 26027 (not found) t14; L93 26200 RNSI_A_CC_BACK_BLAST t6; L94 26216 (not found) t6; L95 900 has_state.

## 2. c1a_numerical_blocks.tsv
Chung skill ids (the 127 above) match **145 rows** (Eve 209): DamageEffect.lua 142, DamageEffect_Character.lua 3, Dungeon 0. All `result=ok`, all `skill_in_march_templets=False`, lookup INDEX 1:72, 2:44, 3:15, 4:11, 5:3.
- Block kinds: DAMAGE_DATA 135, _NEXT 3, _NEXT2 3, _NEXT3 2, _NEXT4 1, _ULTIMATE 1.
- Rows by id band: 26000-26099 103 | 26200-26299 31 | 26400-26499 2 | 26600-26699 9. **58 distinct skill ids** have rows (26000-26026, 26037-26042, 26062-26064, 26201, 26203-26215, 26226-26228, 26412, 26413, 26600-26602); 69 of the 127 have none (incl. 106, 900). No 26xxx id outside the s3 set appears in the tsv.
- Rows with source line numbers: `june2017_re/char_chung/plan_numerical_rows.tsv` (header + 145 rows, CRLF; first column = line in c1a_numerical_blocks.tsv, header = line 1; otherwise Eve's columns).

## 3. june_src_fixed/data036_kom/Chung_Iron_Cannon.src.lua (388,259 B, 19,376 lines; March data036 bytecode 204,340 B; no other `Chung*` file there)
Pure data: every non-indented line is a `NAME = {` table, a closer or a comment; no top-level functions or calls, so no binders to fail.
- Top-level tables: **395** = 7 `INIT_*` (INIT_SYSTEM L1, INIT_DEVICE L12, INIT_MOTION L265, INIT_PHYSIC L268, INIT_COMPONENT L277, INIT_COMMON_RANDOM_STATE L310, INIT_PERSONAL_INFO L563) + `PROMOTION_ITEM_ID_LIST` L698 + 2 `XXX_CSI_CFG_COMBO_DASH_JUMP_ZXX` / `..ZXXLanding` (L7987, L8068) + **385 `CSI_*`**. First state CSI_WAIT_STATE_CHANGE L528. No CHUNG_*/RNSI_*/SI_* table (Eve had 295 ENSI_*).
- CSI_ groups: SI 117, SA 28, RIDING 27, CSG 21, DAMAGE 21, A 21, COMBO 20, CTT 18, CIP 16, CFG 14, CDC 13, CHG 13, STANDUP 8, PEPPER 5, JUMP 4, DASH 4, ATTACK 4, DANCE 3, GUARD 3, HA 3, WAIT/DIE/START/AMUSEMENT/P 2 each, WIN WIN2 LOSE LOSE2 WALK POWER HYPER WALL THROW COMMON CHARGE 1 each.
- DAMAGE_DATA **79** (first L4413): 64 carry a literal DAMAGE, **15 numerical-only** (NUMERICAL_DATA_INDEX, no DAMAGE; checklist L53 `Chung_Iron_Cannon | 15 | 74`):
  CSI_A_CC_BACK_BLAST L6455 | CSI_SI_A_CHUNG_RELOAD_CANNON L6735 | CSI_SI_A_SIEGE_SHELLING_READY L6785 | CSI_SI_A_SIEGE_SHELLING_Z_FINISH L6922 | CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION L7115 | ..._EXPLOSION_FINISH L7185 | CSI_SI_SA_CIP_BURST_WOLF L8339 | ..._HYPER L8491 | CSI_SI_SA_CHUNG_BRUTAL_SWING L8931 | ..._HYPER L9062 | CSI_SI_A_CIP_SUDDEN_BURSTER L9204 | CSI_SI_SA_CFG_LUNATIC_FURY_RUN L9303 | CSI_SI_SA_CTT_CANNON_STRIKE L14312 | ..._HYPER L14452 | CSI_SA_CIP_CRACK_CRASHER L18752.
  NUMERICAL_DATA_INDEX 15 (L6458, 6739, 6789, 6927, 7118, 7188, 8342, 8494, 8936, 9066, 9209, 9309, 14317, 14457, 18755).
- APPLIED_SKILL_ID 24 (15 distinct SKILL_ID.RNSI_*): SA_CC_LUNATIC_BLOW 4; SA_CC_SCARE_CHASE, A_CC_SIEGE_SHELLING, SA_CIP_BURST_WOLF, SA_CC_BRUTAL_SWING, SA_CHG_CANNON_STRIKE, SA_CC_DETONATION 2 each; A_CC_BACK_BLAST, A_CC_RELOAD_CANNON, A_CIP_SUDDEN_BURSTER, SA_CFG_LUNATIC_FURY, A_CIP_ARTILLERY_NOVA, SA_CC_IMPACT_DETONATION, A_CDC_GATLING_SHOT, SA_CIP_CRACK_CRASHER 1 each.
- AT_COMMAND 53 (`ATTACK_TYPE.AT_COMMAND`, first L4430); TIME_STOP_IMMUNE_STATE 14 (L11196, 13917, 13936, 13956, 13975, 13994, 14013, 14032, 14051, 14070, 14089, 14550, 18004, 19333); SHOW_HYPER_CUT_IN_TIME 3 (L3468, 3645, 3822); DAMAGE_BOUNDING_ 3 (L4148, 4157, 4170); SET_LIMIT_MAX_G_SPEED 1 (L4150); WAIT_STATE_CHANGE 1 (L528).
- Other: SHOW_SKILL_CUT_IN_TIME 15, EFFECT_SET_LIST 82, CHANGE_WORLD_COLOR 15, COMBO_GUIDE 36, SUPER_ARMOR 33, IMMUNITY_LIST_AT_THIS_STATE 46 (+ _EDT_ 46), HYPER_EFFECT_SET 2 (L7439, 16368), USEFUL_VALUE 3 (L541, 4164, 10111), HEAD_IK 9, AFTER_IMAGE 16, BUFF_TEMPLET_ID 3, BUFF_FACTOR_ID_AND_APPLY_TIME 1, CHUNG_STATE_ID_LIST_ABLE_TO_BE_CHANGED_TO_GUARD_STATE 1.
- **Summon / CreateNPC keys: none.** No NPC, SUMMON, CREATE or NUI key and no NPC id in the script (the `PET_BOMBING` hits are inside CSI_SA_CHG_CARPET_BOMBING, L13207 / L13337). Summon ids come from C++: March X2GUChung.cpp names NUI_TARGETING_CHUNG and NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER (10 refs each) and NUI_REMOTE_MORTAR (1). June's CreateNPC call sites are Ghidra work for CH1, not collected here.

## 4. Other June Chung Lua (june_src_fixed sizes; March same-name present?)
- data036 Chung_Iron_Cannon 388259 / March 204340 [class script].
- data037: Targeting_Chung 11753 / March 8356 [skill NPC of the targeting summon]; TIME_CHUNG_SUMMON 3970 [NPC copy]; PET_Mystic_CHUNG_3 35413 (**no March**) [Stage 17]; Tutorial_Chung_object / _object2 / _shadow_master, Tutotial_Chung_Dp1/Dp2/Dp3 [tutorial, june-dungeons].
- Dialogs: DLG_1ST_Class_Change_Guide_Chung 16377 and DLG_2ND_Class_Change_Guide_Chung 12521 (**no March**); DLG_Transcendental_Change_Guide_Chung 7206 (March has) [Stage 6]; DLG_Chung_Cannonball(_NEW), DLG_Class_Change_Guide_Chung, DLG_UI_JUMPING_EVENT_CHUNG, DLG_Unit_Create_ClassChange_Button_Chung.
- NPC copies (owned elsewhere): Npc_Chung_01..05, HAMEL_SHADOW_CHUNG(+_ELIT), RAID_SHADOW_CHUNG, SECRET_MAD_SHADOW_CHUNG, TIME_CHUNG1/2/3, APINK_ANGEL_06_CHUNG, PVP_BOT_CHUNG; Dungeon_ElTutorial_Chung, Dungeon_Tutorial_Chung.

## 5. June Chung motions (June koms read with tools/junekom.py Kom(); March = kom_contents/data*_kom)
- `Motion_Chung*` (case-insensitive `Motion_chung_` / `Motion_chung.`): June **70 files** = data204 69 + data012 1 (Buff_NPC); March **17** = data033 16 + data012 1. Only in March: **0**. In both: 17 = 7 same size (Motion_CHUNG.x [already June's in the tree], Buff_NPC, Emotion_APINK, _APINK_LOVE, _BIGBANG, _CRAYONPOP_Sorted, _DeepBow) + 10 differ (June/March bytes): CDC 2817639/1632684; CFG 3171606/1817203; CHG 3122550/2266865; CIP 2526389/1674097; Common 5817217/1120364; CSG 2894234/1685496; CTT 2878158/1655598; Emotion 1130155/814107; Field 423793/241172; Lobby 1246184/920996. Only in June: 53 Emotion variants (Cash1..Cash43, 100YEAR, 2015Chuseok/xmas, 2016Halloween/Maid/Seol, 2017Maid, April_Fools_Day, B1A4, BbaBbaBba, Bonny, BTS, Cobo, FM, GFRIEND x4, Keroro, Larva, Memorial, MIKEY, SeeU, Space_Moon_Rabbit, Strawberry, TWICE_OOH_AHH, Uhee, VIXX).
- s2_model_core.tsv: 31 Chung rows (L184-214): motions 11 differ (all shipped class motions) / 6 same; meshes 4 differ (Basic_Face .X and .XET, CSG_Basic_Hair, CDC_Basic_Hair) / 10 same; bone_parent_map identical. Animation-name deltas March->June: CHUNG 99->103 (+4) | Common 16->37 (+21) | Emotion 9->9 | Field 7->9 (+2) | Lobby 11->11 | CDC 16->20 (+4) | CSG 27->37 (+10) | CHG 32->33 (+1) | CTT 19->26 (+7) | CIP 20->22 (+4, **-2 dropped**) | CFG 30->40 (+11, **-1 dropped**). Dropped names are not listed in s2 (not found).
- **r12b_anim_loss.tsv: 1 Chung row**: SI_SA_CFG_IRON_HOWLING in Motion_CHUNG_CFG.x (matches the CFG -1). The 2 CIP drops have no r12b row; not reconciled here.

## 6. Per-class tsv hits
- c11a_march_only_templets.tsv: L26 DE CHUNG_BIGBANGSTREAM_MEMO (march-consumer, X2BuffTemplet.cpp); L27 DE DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell (X2GUChung.cpp); L44 ES EffectSet_CSI_CIP_LAND_DEMOLISHER_AFTER_EFFECT (X2GUChung.cpp); L45 ES EffectSet_CSI_CSG_COMBO_XFRONTX_FIRE (consumer Chung_Iron_Cannon.lua); L55 DE CHUNG_BIGBANGSTREAM_EXPLOSION_MEMO (no consumer).
- g4_cutin_names.tsv L18-20: HQ_CutIn_Chung_Gura / _Rage / _normal (June `.gfx` = no); June candidates HQ_CutIn_Chung_01 / 11 / Cash_16 / Cash_17 / DC_01 / DC_11 / IP...
- c10a_alpha_sites.tsv: 0 Chung rows (not found). w1f_candown_callers.tsv: 1 Chung-text row. s3e_bybfi_callers.tsv, w1c: none by text.
- funcmap.tsv: 41 lines match "chung" (case-insensitive), 13 contain `CX2GUChung`, 12 rows have a June address inside the class range. Key rows: L200 32bf28f0 SetDamageExceptionProcess (June-only; USED_SKILL_ID 26402), L841 32bec310 ApplyRenderParamOnEmptyRenderParamByBuff, L896 32c16be0 InitPassiveSkillState, L897 32bf4900 InitComponent reader (COMMAND_HIT_ADD_MP_BONUS), L902 32c0c4e0 ~CX2GUChung, L1018 32bf4880 ChangeMaxCharacterAddResource (slot 201), L1060 32bd8790 SpecialAttackEventProcess, L2023 32c1f530 SetEquippedSkillFuncBySkillSlotIndex, L2441 32c24b90 vslot 0xa4 berserk-by-chance (June-only), L5683 32c24b50 IsFullHyperMode.

## 7. UnitTemplet.src.lua (2483 lines) Chung rows vs March kom_contents/data036_kom/UnitTemplet.lua (BOM source)
Seven `AddUnitTemplet` rows, June / March line ranges: IRON_CANNON 745-769 / 723-746 | FURY_GUARDIAN 770-795 / 747-771 | SHOOTING_GUARDIAN 796-821 / 772-796 | SHELLING_GUARDIAN 822-847 / 797-821 | IRON_PALADIN 848-876 / 822-847 | DEADLY_CHASER 877-905 / 848-873 | TACTICAL_TROOPER 1129-1157 / 1089-1114.
- June adds `m_fLobbyRotationY` to all 7 (10, 10, 20, 1, 20, 30, 25) and, for CIP / CDC / CTT, `m_BerserkCutIn` ("HQ_Cutin_CIP_1_Transcendence", "HQ_Cutin_CDC_1_Transcendence", "HQ_Cutin_CTT_1_Transcendence") and `m_HyperCutIn` ("HQ_Cutin_CIP_Transcendence", "HQ_Cutin_CDC_Transcendence", "HQ_Cutin_CTT_Transcendence"). No key removed.
- Value change: UC_CHUNG_TACTICAL_TROOPER `name` STR_ID_9739 (March) -> STR_ID_17435 (June). Others same. No UC >= 150 row.

## 8. Ghidra: June CX2GUChung in unpacked_x2.exe (read-only; xrefs_from per vtable slot; list_functions = 55,556 functions)
- **Vtable 3350967c**, 544 slots (0x0-0x87c; Eve 549), 438 distinct targets. xrefs_to 3350967c: FUN_32bf1040 (`@32bf1094`, Body 32bf1040-32bf28e4 = **constructor**) and FUN_32c0c4e0 (`@32c0c50d`, ~CX2GUChung body; slot 0 scalar deleting dtor = 32c25880).
- RTTI: `.?AVCX2GUChung@@` not found; strings show only `.?AVMemoConditionChung@CX2GUChung@@` (3373d650).
- Overrides inside the class block: **72 slots / 72 distinct targets**, first **0x32bd86f0** (slot 0x48 InitEffect, Body 32bd86f0-32bd8710), last 0x32c25880 (slot 0, Body 32c25880-32c2589d).
- **Class range 0x32bd86f0 - 0x32c25a1f.** Precedes: Ara (vtable 33505314; in-block 32b90b90..32bd34c0, tiny tail overrides 32bd85d0-32bd8680). Follows: Raven (vtable 33511454; first override 0x32c25a20, not a defined Ghidra function; then 32c25a60, 32c25aa0 ...).
- Carved functions: **441** in [32bd86f0, 32c25880]; **442** in [32bd86f0, 32c25a20) (adds 32c25a10).
- Slot 0x7b4 SetEquippedSkillFuncBySkillSlotIndex = **32c1f530** (Body 32c1f530-32c2224f; slot address 33509e30). Slot 0x7a4 InitState = **32c22540**. Slot 0x3c InitComponent 32bf4900. Slot 0x2b8 = **32bf28f0**, the June-only SetDamageExceptionProcess override (funcmap L200: none in March). Slot 0xa4 = 32c24b90 (June-only vslot), 0x9c IsFullHyperMode 32c24b50, 0xa0 32c24b60. Slot 509 (0x7f4) = 32bd8790 SpecialAttackEventProcess, slot 510 (0x7f8) = 32c17830.
