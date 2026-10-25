# Eve (Nasod) June-side facts. Read-only collection, 2026-10-25. Paths relative to Trunk unless absolute. Model: june2017_re/char_arme/plan_june_side.md

## 1. june2017_re/s3_reform_ids_no_state.tsv (2045 lines incl. header; columns as Aisha's section 1)
Eve rows = family file X2GuEve.cpp, **261 rows**, no row outside it for UC_EVE_*. Lines: UC5 76-90 | UC18 370-398 | UC19 399-427 | UC26 603-631 | UC108 1242-1294 | UC109 1295-1347 | UC116 1667-1719.
- verdict: 254 `no_state`, 7 `has_state` (all id 900 SI_ETC_WS_COMMON_LOVE, type 12, once per class; L80, 374, 403, 607, 1246, 1299, 1671).
- ids: **127 distinct** (106, 900, 25000..25605); 251 rows are 25xxx, 10 rows < 1000 (900 x7, 106 x3 = classes 108/109/116 only). 3rd jobs (UC >= 150): none in the tsv (max UC value 122); nothing to list separately.
- Rows per class (UC value): 5 Eve 15 | 18 Exotic Gear 29 | 19 Architecture 29 | 26 Electra 29 | 108 Code Nemesis 53 | 109 Code Empress 53 | 116 Battle Seraph 53.
- skill_type counts per class {type:n}:
  - 5: {6:6, 14:2, 15:2, 5:2, 16:2, 12:1}
  - 18/19/26: {6:8, 14:6, 15:6, 16:4, 5:2, 8:2, 12:1}
  - 108/109/116: {6:13, 15:11, 14:9, 8:7, 16:7, 5:2, 13:2, 12:1, 1:1}
  - totals by type: 6=69, 15=53, 14=47, 16=35, 8=27, 5=14, 12=7, 13=6, 1=3 (sum 261)
- Eve total with no state = **254** (Aisha 260).
- Samples (unit 5): L76 25200 RNSI_A_EVE_PHOTON_BLINK "Photon Flash" t6; L77 25213 ..._UPGRADE t6; L78 25030 RNSI_SA_EVE_DIMENSION_LINK_SENTRY_UPGRADE t14; L79 25000 RNSI_SA_EVE_DIMENSION_LINK_SENTRY "Dimension Link - Century" t14; L80 900 has_state.

## 2. c1a_numerical_blocks.tsv / c1a_skilltemplet_diff.tsv
- Eve skill ids (the 127 above) match **209 rows** of c1a_numerical_blocks.tsv (Aisha 110): DamageEffect.lua 191, DamageEffect_Character.lua 17, DamageEffect_Dungeon.lua 1. All `result=ok`, all `skill_in_march_templets=False`, all lookup `INDEX n`
  (1:89, 2:49, 3:42, 4:19, 5:8, 6:2), no zero L1 value. Block kinds: DAMAGE_DATA 149, _NEXT 45, _NEXT2 11, _ULTIMATE 3, _NEXT_ULTIMATE 1.
  Rows by id band: 25000-25099 145 | 25200-25299 45 | 25600-25699 19. 50 distinct skill ids have rows; 77 of the 127 have none. ids 106 and 900: no row.
  No 25xxx id outside the s3 set appears in the tsv. Rows with source line numbers: `june2017_re/char_eve/plan_numerical_rows.tsv` (same columns as Aisha's, CRLF).
- c1a_skilltemplet_diff.tsv: per-key census, no per-class rows (same as Aisha).

## 3. june_src_fixed/data036_kom/Eve_Nasod.src.lua (276,395 B, 14,137 lines; March data036 bytecode 138,560 B)
Pure data: every section is a top-level `NAME = {`; no top-level functions, no top-level calls, no other statement kinds (checked), so no binders to fail.
- Top-level tables: **301** = 6 `INIT_*` (INIT_SYSTEM L1, INIT_DEVICE L12, INIT_MOTION L243, INIT_PHYSIC L246, INIT_COMPONENT L255, INIT_COMMON_RANDOM_STATE L279) + **295 `ENSI_*`**. No `EVE_*`, `RNSI_*`, `SI_*` tables (Aisha had AVSI 306 + RNSI 4 + ABM 2 + SI 1).
  First state ENSI_WAIT_STATE_CHANGE L497. Name groups after ENSI_: SA 37, A 28, RIDING 27, DAMAGE 21, DASH 17, COMBO 15, JUMP 10, NS 10, EBS 10, SI1 10, SI2 10, EP 9, SI3 8, HA 4, ECN 5, ECE 4, ECB 2, EEG/EG/EPR/PR/AT/BS (job blocks).
- Key counts (grep lines): DAMAGE_DATA **28** (first L4378): 25 carry a literal DAMAGE (PHYSIC 1-4.31 / MAGIC 1.5), **3 numerical-only** (ENSI_A_EBS_ELECTRONIC_MOVE_FRONT L13608 / _UP L13718 / _DOWN L13794; matches checklist L57 `Eve_Nasod | 3 | 25`).
  NUMERICAL_DATA_INDEX 3 (L13612, 13722, 13798; each `= 1`), APPLIED_SKILL_ID 3 (L13622, 13732, 13808, all `SKILL_ID.RNSI_A_EBS_ELECTRONIC_MOVE`),
  AT_COMMAND 23 (as `ATTACK_TYPE.AT_COMMAND`, first L4385), TIME_STOP_IMMUNE_STATE 3 (L12422 ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION, L12575 ENSI_HA_ENS_LUNATIC_SCUD, L12696 ENSI_HA_EBS_PSYCHIC_ARTILLERY),
  SHOW_HYPER_CUT_IN_TIME 3 (L3463 ECN, L3643 ECE, L3823 ECB hyper modes, each 0.04), DAMAGE_BOUNDING_ 3 (ENSI_DAMAGE_BOUNDING_DOWN L4093 / _LANDCRASH L4102 / _UP L4115),
  SET_LIMIT_MAX_G_SPEED 1 (L4095 `= -4000`), WAIT_STATE_CHANGE 1 (ENSI_WAIT_STATE_CHANGE L497).
- Other keys worth a look (1-tab-level counts): SHOW_SKILL_CUT_IN_TIME 43, EFFECT_SET_LIST 49 / _EX 26 / _FINAL 5 / _FINAL_ULTIMATE 5, CHANGE_WORLD_COLOR 44, COMBO_GUIDE 29, IMMUNITY_LIST_AT_THIS_STATE 7 + _EDT_ 7, HYPER_EFFECT_SET 4, USEFUL_VALUE# 4, HEAD_IK 8, AFTER_IMAGE 19, SUPER_ARMOR 14.
- INIT_DEVICE READY_TEXTURE L13-22: HQ_CutIn_Eve_1/2/3.tga (L14-16) + 7 effect textures.

## 4. Other June Eve Lua (june_src_fixed; sizes June source / March bytecode). Classification in brackets.
- data036: **Eve_Nasod** 276395 / 138560 [class script]. Add_NasodRuler is Add's (not Eve).
- Skill NPC / summon scripts, data037 (all have a March same-name file): Atomic_Shield_Eve 6485/5200; Guardian_Eve 7265/5650; Guardian_Sp1_Eve 13075/9743 (+_Memo 13397/10041); Guardian_Sp2_Eve 11797/9013 (+_Memo 12080/9235);
  Nasod_Watch_Eve 41168/28851 (+_Memo 35926/25775); Nasod_Element_Fire_Eve 13360/10211; Wally9th_Eve 15016/11136; SI_HA_FERDINAND 21959/15084; SI_SA_EEP_ELECTRONIC_FIELD 19762/13339 [skill-NPC/summon; names from X2GuEve.cpp NUI_* refs]. data119 Eve_AT_Sp2a_SonicWave 930/683 [skill effect script].
  Not found by name: Moby/Rabby (no hit; Riding_Nasod_Mobi is a mount mesh only), Oberon/Ophelia/Ehemer have no own Lua (appear only as DamageEffect names, see section 2).
- Monster/NPC copies of Eve (owned elsewhere, all have March): Npc_Eve_01..05 (~70.3k), HAMEL_SHADOW_EVE(+_ELIT), RAID_SHADOW_EVE, SECRET_MAD_SHADOW_EVE, TIME_EVE1/2/3/_SUMMON, APINK_ANGEL_05_EVE, CSM_HERO_CODE_Q_PROTO_00_PROFESSION, HENIR_CODE_Q_PROTO_00, PVP_BOT_EVE (0/18 stubs) [NPC scripts -> JUNE NPC/dungeon adoption, already ljconv].
  datai016: EVENT_HALLOWEEN_EVE_GINGERBREAD, EVENT_RAID_SHADOW_EVE, EVENT_SHADOW_EVE_ELIT, EVENT_SHADOW_EVE_FD, PVP_EVE, Battle_Seraph_Event, UI_NPC_EVENT_Battle_Seraph (all have March) [events].
- Tutorial [-> june-dungeons]: data035 Dungeon_ElTutorial_Eve, Dungeon_Tutorial_Eve; data037 Tutorial_Eve_9th_type2/_boss_driller/_object/_thin_defect, Tutotial_Eve_Dp1; data040 Tutorial_Eve_Stage0 21837, Tutorial_Eve_Stage0_LineMap 8254 (**no March**).
  There are no Dungeon_Training_Eve_* in June (Aisha had Dungeon_Training_Arme_*).
- Dialogs [-> Stage 9 unless noted]: data034 DLG_Class_Change_Guide_Eve, DLG_PVP_Game_Damaged_Eve(+_CA,_CE), DLG_UI_JUMPING_EVENT_EVE, DLG_Unit_Create_ClassChange_Button_Eve (all have March); **no March**: DLG_1ST_Class_Change_Guide_Eve 16369, DLG_2ND_Class_Change_Guide_Eve 12515;
  DLG_Transcendental_Change_Guide_Eve 7207 (March 4496) [-> Stage 6]; data217 DLG_Class_Change_Battle_Seraph / _CodeArchitecture / _Electra / _Eve_CE / _Eve_CN (March same names live in data034_kom); datai014 DLG_UI_JUMPING_SYSTEM_EVE_1ST_CLASS 5525 / _2ND_CLASS 5493 (**no March**).
- Pets [-> Stage 17]: data037 PET_Mystic_EVE_3 35411 (**no March**).
- March-only (no June same-name): data035_kom/Dungeon_Battle_Field_Tutorial_Eve.lua; datai016_kom/Event_Doppelganger_Eve.lua.

## 5. June Eve motions (all June koms listed with tools/junekom.py Kom(); March = kom_contents/data*_kom)
- `Motion_Eve*` (prefix `Motion_Eve_`/`Motion_Eve.`): June **70 files** = data203 69 + data012 1 (Motion_Eve_Buff_NPC); March **17** = data033 16 + data012 1. Names only in March: **0**. Both: 17 (7 identical size, 10 differ). Only in June: 53 = Emotion variants in data203 (25 Cash*, 100year, 2015Chuseok/xmas, 2016Halloween/Maid/Seol, 2017Maid, April_Fools_Day, B1A4, BBaBBaBBa, Bonny, BTS, Cobo, FM, GFRIEND x4, Keroro, Larva, Memorial, MIKEY, SeeU, Space_Moon_Rabbit, Strawberry, TWICE_OOH_AHH, Uhee, VIXX).
- Differ (June / March bytes): Motion_Eve 6102656/4315889; _AT 2260878/1652348; _BS 2636906/1691814; _EG 2641577/1907285; _EP 2690935/1371947; _NS 3326557/2016900; _PR 2621401/1836432; _Emotion 1270526/961642; _Field 449459/441322; _Lobby 1822735/1822129.
  Same: _Common (6062982 both; already June's in March tree), Buff_NPC, Emotion_APINK, _APINK_LOVE, _BIGBANG, _CRAYONPOP_Sorted, _DeepBow. No Nasod-named Eve motion (Motion_Nasod_Element_Fire_Eve / Nasod_Watch_Eve in data011 are NPC motions, identical).
- june2017_re/s2_model_core.tsv: 32 Eve rows (L152-183): base_motion 11 differs / 6 same, base_mesh 7 differs / 8 same. All: KSM frames 77->77, bone_parent_map identical, mesh_container_frames identical. Animation-name deltas (unique_names, March->June):
  _EVE 79->84 (+5) | _NS 17->25 (+9, **-1 dropped**) | _PR 17->19 (+2) | _BS 18->21 (+4, **-1 dropped**) | _EP 14->22 (+8) | _AT 15->18 (+3) | _EG 16->18 (+2) | _Common 16->37 (+22, **-1 dropped**; s2 recorded a 1578032 B March file, the tree now holds 6062982) | _Emotion 9->9 | _Field 7->9 (+2) | _Lobby 14->14.
  The 3 dropped names are not listed in s2 (not found). Meshes: Mesh_Eve_Cash_Hair/Face (+XET), ENS_Base_Hair, EEP_Base_Hair, EG_Cash_Hair, AT_Cash_Hair differ; Basic_* UpBody/LowBody/Hand/Foot/Weapon_A/B, ET_Cash_Hair, EBS_Base_Hair identical.
- **r12b_anim_loss.tsv: no Eve row** (classes listed: ADD, ARA, CHUNG, EL, LIRE, RAVEN; header + 43 rows). Checklist L729 also says Eve loses none. The s2 "-1 dropped" x3 vs r12b "none" are not reconciled here (r12b = names March code uses, per its columns class/anim/march_file).

## 6. Per-class tsv hits
- c10a_alpha_sites.tsv: **167 Eve rows** (class_region CX2GUEve, all disposition `checklist 7c`, note `class code (D8 standing rule)`), L27-193; June call va 32982623..329c84f8; helper 32296d60 particle ptr 119, 32296ee0 mesh instance 41, 32296da0 handle major/minor 4, 32296e40 handle array 3; 15 rows have containing_function `-`. Checklist L337 says 167 sites in 0x32965e30-0x329cf260.
- c11a_march_only_templets.tsv Eve rows (7): L23 ES EffectSet_PVP_BOT_EEP_DashAttackZ (june-consumer only, datai016 PVP_HERO_APPLE scripts); L31 DE EVE_EP_DASH_COMBO_Z (march-consumer, X2GuEve.cpp, checklist 7u); L59/L60 DE DamageEffect_Heavens_Fist_Sweeper(_MEMO) (no consumer); L64/L65 DE EVE_EP_ELECTRONIC_FIELD_HYPER / _NO_DAMAGE_HYPER (no consumer); L83 ES EffectSet_ENS_Assualt_Spear (no consumer). Checklist L624 lists only EVE_EP_DASH_COMBO_Z.
- g4_cutin_names.tsv L33-39 (7 rows, June `.gfx` exists = no for all, March class file X2GuEve.cpp): HQ_CutIn_Eve_1, _2, _3, _BS_1, _EP_1, _Gura, _NM_1. June candidates: HQ_CutIn_Eve_01/02/03/BS_01/Cash_16/Cash_17/EP_01/NM_01.
- s3e_bybfi_callers.tsv: no Eve text; address-matched (0x3298-0x329b) L150-160 (11 rows): 329830e2 in 32982fb0, 3298a547 in 32988e40, 32990c1a in 329903e0, 329936ab in 32993020, 329979ef in 32997940, 3299b7e6 in 3299b790, 3299b8b0 in 3299b810, 3299bb10 in 3299ba70, 329b2e8f in 329b1e20, 329b58c4 / 329b5a1f in 329b57a0. Region membership by address, unverified. Checklist L237: Eve caller 3299b7f9 (BFI 812).
- Others: w1c_land_position_callers.tsv L23, 24, 25, 84 (Dimension Link FrameMoves, all `true (March)`); w1f_candown_callers.tsv 18 address rows in 3298-3299 (L39-50, 95-96 shown, all "March shape: forwarder"); c9h4f_useful_value_readers.tsv L93-94 (3299a9d0, 329ae4f0); c3e3_stop_time_callers.tsv none.
- funcmap.tsv: 4 `CX2GUEve` rows: L197 329bc560 SetDamageExceptionProcess (none in March, June-only), L842 32978420 ApplyRenderParamOnEmptyRenderParamByBuff, L1059 32969300 SpecialAttackEventProcess (slot 509; slot 510 = 329b5af0), L2016 329c8a00 SetEquippedSkillFuncBySkillSlotIndex (vslot 0x7b4, RTTI vtable 334cf45c).
- Checklist (JUNE2017_CHARACTER_PORT_CHECKLIST.md) Eve lines: 57 (3/25 table), 237, 248 (32976fe0 PSYKER_KINESIS 25235 cooldown), 277, 285 (32978ec2 tutorial flag), 297 (3299a9d0 Extreme_Virtual, 329c12a0, 329a3040/3760 Thousand Star), 320, 337, 411 (329ba9c0), 427, 477, 624, 662, 729, 788-790 (slot id 4+4 vs June 5+5), 823-824 (El Crystal icon, UpdateEveElectraSkillSlotIcon), 866, 876, 890 (GetCharacterSpecializationB 329b407b), 922 (32994710).
