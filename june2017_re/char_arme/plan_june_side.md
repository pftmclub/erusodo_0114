# Aisha (Arme) June-side facts. Read-only collection, 2026-10-20. Paths relative to Trunk unless absolute.

## 1. june2017_re/s3_reform_ids_no_state.tsv (2045 lines incl. header)
Columns (20): unit_class_name, unit_class_value, family_cpp_file, skill_id, skill_name, templet_name, skill_type, master_level, row, column,
in_cpp_enum, cpp_case_in_family, verdict, march_value_of_name, march_names_at_same_id, cpp_any_token_in_family, case_in_X2GUUser_cpp,
lookalike_march_name, lookalike_march_value, lookalike_case_in_family.
Aisha rows = lines 17..1560 (non-contiguous, grouped per class), 267 rows total, all family file X2GUArme_VioletMage.cpp.
- verdict: 260 `no_state`, 7 `has_state` (all the same skill: id 900 SI_ETC_WS_COMMON_LOVE, type 12, once per class; in_cpp_enum=y, March case exists).
- ids: 130 distinct (106, 900, 22000..22605); 257 rows are 22xxx, 10 rows are id <1000 (900 and 106 per class).
- Rows per class (UC value): 2 Violet Mage 15 | 14 High Magician 30 | 15 Dark Magician 30 | 23 Battle Magician 30 | 104 Elemental Master 54 | 105 Void Princess 54 | 113 Dimension Witch 54.
- skill_type counts per class {type:n}:
  - 2: {6:2, 14:4, 12:1, 15:4, 16:4}
  - 14/15/23: {6:6, 14:6, 12:1, 15:8, 16:6, 8:1, 5:2}
  - 104/113: {6:11, 14:9, 12:1, 15:13, 16:9, 8:6, 5:2, 13:2, 1:1}
  - 105: {6:11, 14:11, 12:1, 15:11, 16:9, 8:6, 5:2, 13:2, 1:1}
  - totals by type: 15=65, 6=53, 14=51, 16=49, 8=21, 5=12, 12=7, 13=6, 1=3
- Aisha total with no state = **260**.
- 5 samples (all unit 2; `-` = empty cols): 
  - L17: 2, 22216 RNSI_A_AISHA_TELEPORT_UPGRADE "[Enhanced] Teleport" type6 ml1 row0 col0 in_enum=n case=n no_state
  - L18: 22200 RNSI_A_AISHA_TELEPORT "Teleport" type6 ml1 0/0 n n no_state
  - L19: 22000 RNSI_SA_AISHA_CHAIN_FIREBALL "Chain Fireball" type14 ml1 row0 col2 n n no_state
  - L20: 22030 RNSI_SA_AISHA_CHAIN_FIREBALL_UPGRADE type14 ml1 0/2 n n no_state
  - L21: 900 SI_ETC_WS_COMMON_LOVE "Extreme Heavenly Love" type12 ml2 0/4 y y has_state (March value 900)
- Violet Mage lines: 17-31 (e.g. L22 GUST_SCREW t15, L24 GUST_STORM t16, L26 LIGHTNING_BOLT t15, L28 METEOR_CALL t16, L30 BINDING_CIRCLE t14).

## 2. c1a_numerical_blocks.tsv / c1a_skilltemplet_diff.tsv
- Neither file has an `Arme_VioletMage` row. c1a_numerical_blocks.tsv (1914 lines) columns: file, templet, block, skill_id, june_skill_name, lookup, result,
  L1_value, L1_x0.01, L1_pvp_rate, L1_tech_point, L1_hit_add_mp, skill_in_march_templets. Its `file` is DamageEffect*.lua (1510), DamageEffect_Character.lua (394), DamageEffect_Dungeon.lua (9).
- Rows whose skill_id is an Aisha skill id (22xxx/106/900 from s3 list): **110 rows** (lines 4..1754): DamageEffect.lua 106, DamageEffect_Character.lua 4; all `result=ok`,
  all `skill_in_march_templets=False`. Examples: L? AISHA_ELEMENTAL_MASTER_CYCLONE_OUTSIDE 22011 L1 208; DAMAGE_EFFECT_ADW_SCREWDRIVER_Main 22028 2469 (tech 938); DamageEffect_Fate_Space_Attack02 22602 5920; DamageEffect_Lightning_Shower 22012 735.
  Full 110 rows with source line number: `...\scratchpad\aisha_june_c1a_rows.tsv` (companion file, same folder as this one).
- c1a_skilltemplet_diff.tsv (168 lines) is a per-key census (key, shared_skills_differing, note; e.g. m_EffectiveDesc 310, m_eType 232): no class/skill rows, **no Aisha/Arme rows**.

## 3. june_src_fixed/data036_kom/Arme_VioletMage.src.lua (273,805 B, 13,787 lines)
Declaration style: every state/section is a top-level table assignment `NAME = {` at column 0; no top-level functions, no top-level calls, no `g_pX2Game:` / `AddPublicCommand`
(grep for `^name(` calls = 0; no other top-level statement kinds). So the script is pure data tables.
- Top-level tables: **319** = 6 `INIT_*` (INIT_SYSTEM L1, INIT_DEVICE L12, INIT_MOTION L238, INIT_PHYSIC L241, INIT_COMPONENT L250, INIT_COMMON_RANDOM_STATE L264)
  + 306 `AVSI_*` states (first AVSI_WAIT_STATE_CHANGE L482; AVSI_START L605; per-job blocks AVSI_AHM_/ADM_/ABM_/AEM_/AVP_ START/WIN/WIN2/LOSE/LOSE2; e.g. AVSI_SA_AVP_HELL_DROP L8604)
  + 4 `RNSI_*` (RNSI_A_ADM_DARK_ROAD L12709, RNSI_A_ABM_ENERGETIC_ROAD L12757, RNSI_SA_ADM_DARK_BURST L12805, RNSI_SA_AVM_METEO_CALL_NEW L12881)
  + 2 `ABM_*` (ABM_DASH_COMBO_ZXX L9513, ABM_DASH_COMBO_ZXXX L9564) + 1 `SI_A_AV_ICICLE_WAVE` (L11067). => 313 state tables.
- Key counts (grep -c lines): DAMAGE_DATA 37 (first L4186), NUMERICAL_DATA_INDEX 1 (L9750 `= 2`), TIME_STOP_IMMUNE_STATE 12 (first L8677, `= TRUE`),
  SET_LIMIT_MAX_G_SPEED 1 (L3878 `= -4000`), AT_COMMAND 18 (as `ATTACK_TYPE.AT_COMMAND`, first L4288), DAMAGE_BOUNDING_ 3 (state names AVSI_DAMAGE_BOUNDING_DOWN L3876 / _LANDCRASH L3885 / _UP L3898),
  WAIT_STATE_CHANGE 1 (AVSI_WAIT_STATE_CHANGE L482), SHOW_HYPER_CUT_IN_TIME 3 (L3369, 3485, 3601, each 0.06), APPLIED_SKILL_ID 3 (L4205 RNSI_A_AISHA_TELEPORT, L4250 ..._UPGRADE, L9755 RNSI_A_ABM_ENERGY_DRAIN).
- INIT_DEVICE READY_TEXTURE lists HQ_CutIn_Aisha_1/2/3, _EM_1, _VP_1 .dds (L12-22); sound list L~30-230 (Aisha_*.ogg).

## 4. Other June Aisha Lua (june_src_fixed, name matches arme|aisha; 64 files). March = same-name file in kom_contents/*_kom (sizes: June source / March bytecode)
- data036: Arme_VioletMage 273805 / March data036 136603; Ain_Arme_Thaumaturgy 102284 / **none in March**.
- data037 (NPC/monster scripts, all have March same-name unless noted): NPC_Arme_VioletMage 3113/2527; Aisha_PigBat 4775/3811; Aisha_StoneWall 6809/5527; APINK_ANGEL_02_AISHA; Bloody_Aisha_Gliter(+_Boss,_Hell);
  CSM_Aisha_Dimensionwitch/Elementalmaster/Voidprincess; HAMEL_SHADOW_AISHA(+_ELIT); Npc_Aisha_01..05 (~67.7k each); RAID_SHADOW_AISHA; SECRET_MAD_SHADOW_AISHA; TIME_AISHA1/2/3/_SUMMON; Tutorial_Aisha_boss_verde/_low_pporu/_pporu;
  Tutotial_Aisha_Dp1; Ufo_Aisha 7430/4983; Ufo_Aisha_Droper_Memo 13742/9880; PVP_BOT_AISHA 0/18 (both stubs); **PET_Mystic_ARME_3 35412: no March**.
- data034 (dialogs): DLG_Class_Change_Guide_Aisha, DLG_PVP_Game_Damaged_Arme(+_DM,_HM), DLG_UI_JUMPING_EVENT_AISHA, DLG_Unit_Create_ClassChange_Button_Arme, DLG_Unit_Select_Name_Arme, DLG_Unit_Select_Slot_Arme have March; **no March**: DLG_1ST_Class_Change_Guide_Aisha 16375, DLG_2ND_Class_Change_Guide_Aisha 12521, DLG_Transcendental_Change_Guide_Aisha 7186.
- data035: Dungeon_ElTutorial_arme, Dungeon_Training_Arme_2..6 and arme_0/1, Dungeon_Tutorial_Aisha (all have March).
- data040: Tutorial_Aisha_Stage0 19767, Tutorial_Aisha_Stage0_LineMap 8173: **no March**.
- data217: DLG_Class_Change_Aisha_DW/EM/VP (March same name lives in data034_kom).
- datai014: DLG_UI_JUMPING_SYSTEM_AISHA_1ST_CLASS 5553, _2ND_CLASS 5517: **no March**. datai016: EVENT_HALLOWEEN_AISHA_GINGERBREAD, EVENT_RAID_SHADOW_AISHA, EVENT_SHADOW_AISHA_ELIT, EVENT_SHADOW_AISHA_FD: have March.
- March-only (no June same-name): data035_kom/Dungeon_Battle_Field_Tutorial_Aisha.lua; datai016_kom/Event_Doppelganger_Arme.lua.
- Not in june_src_fixed but relevant: Aisha skill-affecting globals live in DamageEffect*.lua / EffectSet.lua (core-list, data036).

## 5. June Aisha motions (read-only `tools/junekom.py list <kom>` over all 260 June koms in 237311/14JUN17/data; `tools/kom.py stage-june` NOT run)
All 69 `Motion_Arme*` June files are in **data200** (V.0.4 kom). Base set + sizes (June / March data033_kom):
Motion_Arme.x 5285943 / 3442962; _BM 2966301 / 2009215; _Common 6148830 / 2005409; _DM 2292934 / 1505385; _DW 3145404 / 1792867; _EM 2738878 / 1625101; _HM 2788996 / 1675220; _VP 2870210 / 1576895;
_Field 371832 / 232673; _Emotion 1803707 / 1253531; _Lobby 1597467 / 1597467 (identical). Emotion files June-only (~50): 100YEAR, 2015Chuseok, 2015xmas, 2016Halloween/Maid/Seol, 2017Maid, April_Fools_Day, B1A4, BTS, Bonny, Cash1..43 (many), Cobo,
FM, GFRIEND_*, Keroro, Larva, MIKEY, Memorial, SeeU, Space_Moon_Rabbit, Strawberry, TWICE_OOH_AHH, Uhee, VIXX, ARME_Emotion_BbaBbaBba. Identical in both: APINK, APINK_LOVE, BIGBANG, CRAYONPOP_Sorted, DeepBow.
March has 16 Motion_Arme* (data033_kom). Other June kom hits for the arme|aisha name filter appear in many koms (data015 5809, data067 1811, data191 1198, datai004 812 ...: meshes, textures, sounds); raw listing: /tmp/aisha_june_list.txt (git-bash path).
- june2017_re/s2_model_core.tsv: 38 Aisha rows (lines 44..81). Motion .x differs: Motion_Arme_BM L44 (anim_sets 23->28, +4 names), _HM L45 (15->26, +11), _DW L46 (13->20, +7), _DM L47 (18->21, +3), _VP L48 (11->19, +8), _Field L49 (7->9, +2),
  _Emotion L50 (10->10, +0), Motion_Arme.x L51 (66->74, +8). All: KSM frames 76->76 (BM 75), bone_parent_map identical, mesh_container_frames identical, 0 dropped names. Counts: base_motion 10 differs/7 same; base_mesh 12 differs/9 same (Mesh_Arme_Basic_* UpBody/LowBody/Hand/Foot/Wand and AVP hair identical).
- june2017_re/r12b_anim_loss.tsv: no Arme row (header only for Arme; checklist L719 says Elsword, Arme, Eve lose no animation names).

## 6. Per-class tsv hits (verbatim, file:row)
- c10a_alpha_sites.tsv (22 Aisha rows, L214-L235; cols june_call, helper_va, helper, containing_function, class_region, disposition, note), first 15:
  L214-221: 32ccdf92/dfa6/dfba/dfce/dfe2/dff6/e00a/e01e | 32296da0 | tag particle (handle, major/minor) | FUN_32ccdb70 | CX2GUArme_VioletMage | checklist 7c | class code (D8 standing rule)
  L222 32cce1c2 ... FUN_32cce0f0; L223 32cce2fd ... FUN_32cce230; L224 32cce43d ... FUN_32cce370 (same helper/disposition)
  L225 32cd2853 | 32296ee0 | tag mesh instance | FUN_32cd2740 | CX2GUArme_VioletMage | checklist 7c; L226 32cd29bf ... FUN_32cd2870; L227 32cd4f03 ... FUN_32cd4df0; L228 32cd5033 ... FUN_32cd4f20
  (remaining 7 rows L229-235 not shown).
- c11a_march_only_templets.tsv (header comment L1: templets March's merged DamageEffect/EffectSet held that June's lack):
  L4 `DE	AISHA_SUMMON_BAT_BITE	june-consumer only (JUNE_BUGS 200)	data037_kom/Aisha_PigBat.lua;EffectSet.src.lua`
  L5 `DE	AISHA_SUMMON_BAT_FIRST_BITE	june-consumer only (JUNE_BUGS 200)	EffectSet.src.lua`
  L40 `ES	EffectSet_AHM_Blaze_Step_Damage_MEMO	march-consumer (checklist 7u)	data036_kom/Arme_VioletMage.lua`
  L41 `ES	EffectSet_Aisha_Blaze_step_MEMO	march-consumer (checklist 7u)	data036_kom/Arme_VioletMage.lua`
  L42 `ES	EffectSet_Aisha_DeathFieldBomb	march-consumer (checklist 7u)	X2GUArme_VioletMage.cpp`
  L43 `ES	EffectSet_Aisha_DeathField_FirstImpact	march-consumer (checklist 7u)	X2GUArme_VioletMage.cpp`
- g4_cutin_names.tsv L4-10 (7 rows; col2 = "June <name>.gfx exists" = no for all; March class file X2GUArme_VioletMage.cpp; candidates HQ_CutIn_Aisha_01/02/03/Cash_16/Cash_17/DW_01/EM_01/VP_01):
  HQ_CutIn_Aisha_1, _2, _3, _DW_1, _EM_1, _Gura, _VP_1.
- c3e3_stop_time_callers.tsv L23: `22	X2GUArme_VioletMage.cpp AVSI_SI1_AV_BINDING_CIRCLE_HYPER_FrameMove	CX2GUArme_VioletMage (fastcall FrameMove)	32cd5d60	@32cd6152	SetStopTime on each unit in range	( 3.0 [33436700], false, NULL, true ): the only bForce = true caller	checklist 7p`
- w1f_candown_callers.tsv: Aisha-keyword rows L15 (32021390 June-only landing-prediction helper, callers Add/Luciel/Ara-EL/Arme -> checklist 7ac) and
  L87 `32cd97ba 32cd95d0 ptr ptr NULL 1 0 1 0 in ptr, out NULL DIFFERS ... CX2GUArme AVSI_SA_AVP_HELL_DROP FrameMove | X2GUArme_VioletMage.cpp:16482 | no change (equivalent) | out NULL; March's iNewLineIndex only reset after; June always creates NUI_UFO_AISHA_DROPPER_MEMO (0x627) -> checklist 7ac note`;
  plus address-matched rows in 32cc-32cf range, all "March shape: forwarder": L82-86, L88-91, L264-268... (>=17 rows; callers 32cd0200, 32cd21d0, 32cd4830, 32cdebd0, 32cdf240, 32cd0490, 32cd07e0, 32cd0a40, ...).
- s3e_bybfi_callers.tsv: no Arme/Aisha text; address-matched L309 `32cd5bc9 32cd57c0 FUN_32cd57c0 0|0|1|ebx|0x49`, L310 `32cd6308 32cd5d60 FUN_32cd5d60 0|0|1|esi|0x4a`, L311 `32cd7f23 32cd7bf0 FUN_32cd7bf0 ebx|ebx|1|esi|0x4b`, L312 `32cfc2f0 32cfc1f0 FUN_32cfc1f0 0|0|eax|0|0x151`.
  (L312 is in 32cf range but may be Elsword's family, unverified.) Checklist L230 also says Aisha caller `32cc5c16` (BFI in a register).
- c9h4f_useful_value_readers.tsv, w1c_land_position_callers.tsv: no Arme/Aisha and no 32cc-32cf address rows.
- funcmap.tsv (3729 lines): 31 rows mention Arme/Aisha or lie in 0x32cc0000-0x32cfffff; only 5 are in range. First 25 (line: va | symbol | status):
  203: 32cccc70 CX2GUArme_VioletMage::SetDamageExceptionProcess | confirmed (Phase 9: slot 0x2b8 of 3351d0b4; March X2GUArme_VioletMage.cpp:19787; not yet compared)
  645: 322a7660 none in March: set existing buff stack count by BFI | confirmed (Aisha caller 32cc5c16)
  662: 32e7cef0 CX2BuffChangeConsumeMpRate::SetFactor | confirmed;  675: same, supersedes (reader 32039a50, GetActualMPConsume 3203cf60; disable March's Aisha Te...)
  707: 3203cf60 CX2GUUser::GetActualMPConsume | slot 423 of 13 user vtables incl. Arme_VioletMage
  789: 32eb8c30 CX2BuffEffectSetOnlyMyClassDisplayerTemplet::ParsingDisplayer (key AISHA among 12)
  840: 32cc1c50 CX2GUArme_VioletMage::ApplyRenderParamOnEmptyRenderParamByBuff | confirmed (Buff Phase 4b-iii: vslot 197; +0x6840 return, transform time +0x96c8)
  1042: 32d1bd10 CX2GUElsword_SwordMan slot 510 (mentions Arme list) ; 1057: 32cf3290 CX2GUElsword_SwordMan::SpecialAttackEventProcess (slot 509)
  1064: 322f11d0; 1072: 322f4c60; 1076: 329cc370 (riding-key wait; each class's slot 510); 
  1077: 32cebe80 CX2GUArme_VioletMage slot 510 (the skill lookup) | confirmed (vtable 3351d0b4+0x7f8; holds June's 329cc370 call @32cec385)
  1356: 3208e430 state-data +0x11; 1446: 31fd1840 GetActualMPConsume (slot 424); 1626: 321dfc40 mesh trigger runner
  2026: 32cede00 CX2GUArme_VioletMage::SetEquippedSkillFuncBySkillSlotIndex (vslot 0x7b4) | confirmed (slot 493; normal/hyper state id tables)
  2117: 32552860 CX2StateCreateUnit::SelectElSword; 2173: USI_BOUNDING_DOWN/_LANDCRASH/_UP (user states GUSI_END+115..117, every class registers); 2384: 327ac9d0; 2452: 322d8010; 2455: 322f11d0; 2459: 32305be0
  3269: 328e0460 CX2EquippedSkillSlotData::ShowSlotPicture | ported (case 2 AISHA_MEMORIZE_SIGN -> checklist 7aj)
  (rows 3296 328e9160 ShowSkillDesc [Aisha memorize line], 3459, 3508, 3545, 3555, 3637, 3718 are the remaining 7 hits, not read.)
  => In-range 32cc-32cf rows are only 203, 840, 1057, 1077, 2026 (the Arme class has just these mapped functions; most of 32cc.. region is unmapped).

## 7. JUNE2017_CHARACTER_PORT_CHECKLIST.md (830 lines) lines mentioning Aisha / Arme / 32cc-32cf / memoriz / unit type 2
L4 "...or a June update of a March one (Elsword, Aisha, Rena, Raven, Eve, Chung, Ara, Elesis, Add)."
L61 table row `Arme_VioletMage | 1 | 36` (script count per ... table; 36 = 36 items in that scan)
L230 `322a7660` set-stack-count: callers incl. Aisha (`32cc5c16`, BFI in a register)
L249 vslot 510 skill lookup row (Add 32a20e70, Ain, Rose, Luciel; Arme 32cebe80 per funcmap)
L264 Spec A readers `32cc7300`, `32ccda90`, `32ce7480` (option member +0x6bb0)
L272 SetSkillLevelStateData callers: `32c886b0`, `32ccb830`, ...
L310 per-class passive blocks in funcmap rows incl. `32cccc70` (Aisha) keyed on June RNSI_* IDs; brief kickoffs/june-effect/C8c.md; needs C5
L311 D8: "Aisha's Fitness 2413 block replaced by Magical Staff 22409" (hand off: March blocks stay until this port)
L329 `22 sites (32ccdb70 .. 32ce0010) | Aisha | as above (tag particles/meshes with alpha category) | inside the class code being ported`
L396 per-class EventProcess cancel branches (`32cf8230`.. are Elsword)
L398 SOUND_PLAY_EX / STATE_SOUND_PLAY / CUSTOM_STATE_DELETE_SOUND data: Add, Elesis, Luciel, Arme, Aisha (18/2/2 June scripts): "work once the class scripts are adopted"
L417 `32cede00` Arme: SetEquippedSkillFuncBySkillSlotIndex; sets state byte +0x11 on normal and hyper entries
L523 Aisha binding circle hyper FrameMove `32cd5d60` @32cd6152: SetStopTime( 3.0, false, NULL, true ), forced stop overrides immune state
L552 X2DungeonManager.cpp:252-552 reads COMMON/ELSWORD/AISHA/RENA/... keys
L612 `Aisha | X2GUArme_VioletMage.cpp: EffectSet_Aisha_DeathField_FirstImpact, EffectSet_Aisha_DeathFieldBomb | Arme_VioletMage.lua: EffectSet_AHM_Blaze_Step_Damage_MEMO, EffectSet_Aisha_Blaze_step_MEMO` (March-only EffectSet names)
L691 default: plays class's first skill state (... Arme Power Attack ...)
L719 r12b: Elsword, Arme and Eve lose no animation names; L723 stage June's Motion_<Class>* with the class script (pattern ^motion_(elsword|arme|...)); keep script and motions in the same pack
L745-747 Hell Drop @32cd97ba (X2GUArme_VioletMage.cpp:16482) passes in-index, NULL out-index; June always creates NUI_UFO_AISHA_DROPPER_MEMO (0x627); helper 32021390 callers Add/Luciel/Arme
L763 X2GUArme_VioletMage.cpp:6865 etc.
L788-789 UI: memorize slots Static_Memorize_Skill_Slot_TypeA1-3 / TypeB1-3, Memorize_Slot1-3_TypeA/B, UseMemorizedSkillHotKey_A/B (Aisha; needs STUCM_MEMORIZE_SKILL_SLOT_MOUSEIN = 29059, already in Enum.lua)
L805 `32bc84b0`, `32d1ddf0`, `32ccfb92`-`32ccfc91` helper range (port of each calls it where June does)
L815 Slot data types 5/6 (328d9ac0/328e0f10 Aisha memorize slots, ctor caller 322deb80); equipped slot SESAPT_MEMORIZE picture (328e0460 case 2, AISHA_MEMORIZE_SIGN, 328df840 via vslot 0x714); memorize mouse-in case 29059 (328ec719)
L818 Aisha's memorize line in tooltip (328e9160 @328e9710): unit type 2, templet +0x170 == 3 (32205af0)
L829 "Unit type 2 (Aisha): 32ce1770( &true ) on the game unit (@3220f4dc), a walk over the unit's class list (32cf1b10 / 32a78b90)."
(Line numbers are from grep -n; some quotes shortened. No other lines contain "unit type 2"/"memoriz".)
