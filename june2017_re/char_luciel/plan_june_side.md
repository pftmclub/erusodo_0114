# char-luciel: June-side planning evidence (read-only scan, 2026-11-02)

Paths: `E` = `june_src_fixed/data036_kom/Enum.src.lua`; `LIVE` = `...\24965799\kom_contents\data036_kom\Enum.lua` (BOM text, readable); `K` = `...\24965799\kom_contents`.

## 1. Enum ids
UNIT_TYPE (E:312, LIVE:306): `UT_LUCIEL = 10`. March C++ `X2Lib/X2Unit.h:36` already has `UT_LUCIEL = 10, // shares UT_END's value` (June UT_END = 13, E:313).

UNIT_CLASS. Lu/Ciel has ONE base class + THREE first jobs + THREE second jobs (not two + two). Role from UnitTemplet motion suffix (UnitTemplet rows below).
| name | value | E line | LIVE line | role |
|---|---|---|---|---|
| UC_LUCIEL_DUO | 200 | 399 | 395 | base (UnitTemplet row 1) |
| UC_LUCIEL_CHILIARCH | 37 | 354 | 370 | 1st job (motion suffix LC) |
| UC_LUCIEL_ROYAL_GUARD | 38 | 379 | 371 | 1st job (CG) |
| UC_LUCIEL_DIABLURA | 39 | 358 | 372 | 1st job (LDB) |
| UC_LUCIEL_DREAD_LORD | 127 | 359 | 384 | 2nd job (LD) |
| UC_LUCIEL_NOBLESSE | 128 | 361 | 385 | 2nd job (CN) |
| UC_LUCIEL_DEMONIO | 129 | 421 | 386 | 2nd job (LDN) |
| UC_LUCIEL_CATASTROPHE / _INNOCENT / _DIANGELION | 177 / 178 / 179 | 407-409 | not in LIVE | 3rd job: OUT OF SCOPE |
Class ids > 127: yes (128, 129, 200; third jobs 177-179). All 7 in-scope ids are already in `X2Lib/X2Unit.h:167-191` (37,38,39,127,128,129,200). LIVE has exactly 7 `UC_LUCIEL_*` lines (no third jobs).

Other Enum names:
- DUO_TYPE (E:17515-17520): `DT_MINOR = 1, DT_BOTH = 2, DT_MAJOR = 0, DT_KEEP = 3`. LIVE (Enum.lua:13155-13157) has only `DT_MAJOR = 0, DT_MINOR = 1`: DT_BOTH=2 and DT_KEEP=3 are MISSING live, and Luciel_Duo uses both (47 DT_BOTH, 24 DT_KEEP, 153 DT_MAJOR, 166 DT_MINOR, e.g. Luciel_Duo.src.lua:821/854).
- `HIT_ADD_COMBINATION_POINT`: NOT in Enum (no match); it is a damage-data key, Luciel_Duo x6 (first at Luciel_Duo.src.lua:7359 `HIT_ADD_COMBINATION_POINT = 1,`); `CP_AMOUNT` x2 (:8915 `CP_AMOUNT = 0,`).
- CHARACTER_ID (E:17537): `CI_LU = 10` (E:17539), `CI_CIEL = 11` (E:17542); `ATSI_CIEL_PROMOTION = 11` (E:17614); `CCIC_LUCIEL = 11` (E:17820).
- `NESI_MINORUNIT_*` (E:6653-6694) are equipment-slot names (AC_*, FASHION), not Lu/Ciel-specific; `SPT_*_MINOR_UNIT` (E:17488-17497) are voice types. No Lu/Ciel-specific minor-unit enum found.
- Other Lu/Ciel enum members: `WI_TUTORIAL_LUCIEL_STAGE0 = 633` (E:2357), `CTI_ICON_LUCIEL_SWITCHING* ` (E:10796-10877), `SCUUCM_LUCIEL_BUTTON_UP = 36` / `_JUMPING = 50` (E:1283/1295), `BTI_BUFF_P_LUCIEL_OPEN_MIND = 877` (E:12289), `BFI_BUFF_P_LUCIEL_OPEN_MIND = 906` (E:13701), `LUEMCUM_LUCIEL_*` (E:1085-1104).
- NPC ids (NUI_): `NUI_CSM_LU_DIABLA 2282` (E:3795), `NUI_CSM_CIEL_DEMONIO 2283` (E:4530), `NUI_TIME_LU1/2/3 2725/2726/2727`, `NUI_TIME_CIEL1/2/3 2728/2729/2730`, `NUI_TIME_LU_SUMMON 2740`, `NUI_TIME_CIEL_SUMMON 2741`, `NUI_SI_SA_LLD_SUMMON_DARKNESS 1906`, `NUI_SI_A_LCN_SHADOW 1907`, `NUI_SI_SA_LCN_SHADOW_BOLT 1908`, `NUI_SI_SA_LLD_SUMMON_DARKNESS_PVP 1937`, `NUI_SA_LDN_GARDEN_OF_HAURES 2269`, `..._ADD 2270`, `NUI_TUTORIAL_LUCIEL_DP1 2067`, `NUI_TUTORIAL_RUCIEL_OBJECT 2044`, `NUI_RAID_SHADOW_CIEL 2113`, `NUI_EVENT_LUCIEL_DARKNESS 4233`, `NUI_EVENT_RAID_SHADOW_CIEL 4357`.

SKILL_ID (E:14257-16853). Prefix tokens (derived from motion suffixes): LD = shared base, LLC = Chiliarch, LCG = Royal Guard, LLD = Dread Lord, LCN = Noblesse, LDB = Diablura, LDN = Demonio.
- `SI_` ids: 52 names, 10000-10025 (SA), 10200-10210 (A), 10400-10412 (P), 10600-10601 (HA). e.g. `SI_SA_LD_TRIPLE_SHOT = 10000`, `SI_SA_LLC_DEAD_HANDS = 10006`, `SI_A_LCN_SHADOW = 10210`, `SI_HA_LCN_CASTLE_OF_ABADDON = 10601`. No `SI_*_LDB_*` / `SI_*_LDN_*` names exist.
- `RNSI_` ("reformed") ids: 143 names in 30001-30606 (the whole 30000-30999 block is Lu/Ciel; Rose starts at 31001). e.g. `RNSI_SA_LUCIEL_TRIPLE_SHOT = 30001` (E:14761), `RNSI_SA_LDN_ABYSS_EXTINCTION = 30085`, `RNSI_A_LUCIEL_HEAVY_HIT = 30201`, `RNSI_A_LUCIEL_AURA_PUNISHMENT_ACCEL = 30301`, `RNSI_P_LDN_STABLE_DARKENERGY = 30421`, `RNSI_HA_LDN_DIABOLIC_DISASTER_UPGRADE = 30606`. Blocks: SA 30001-30085, A 30201-30220, A 30301-30315, P 30401-30421, HA 30601-30606.

## 2. Class scripts (june_src_fixed/data036_kom)
Top-level state prefix is `LSI_` (not CSI_). Duo includes Demonio: `Luciel_Duo.src.lua:22107 IncludeLua("Luciel_Demonio.lua")`.
- `Luciel_Duo.src.lua`: 22,847 lines, 448,921 B, 406 top-level tables = 6 INIT (`INIT_SYSTEM:1`, `INIT_DEVICE:24`, `INIT_MOTION:566`, `INIT_PHYSIC:570`, `INIT_COMPONENT:579`, `INIT_COMMON_RANDOM_STATE:597`; NO `INIT_PERSONAL_INFO`) + 400 `LSI_*`. Groups: LSI_LU_ 83, LSI_CIEL_ 79 (DAMAGE/DASH/COMBO/STANDUP/JUMP/PEPPER/DANCE/DIE/AMUSEMENT per partner), LSI_SA_ 63, LSI_A_ 36, LSI_LCG_ 29, LSI_LLC_ 28, LSI_RIDING_ 27, LSI_LLD_ 17, LSI_LCN_ 17, LSI_LDN_ 7, LSI_LDB_ 5, LSI_START_ 2, LSI_HA_ 2, WAIT_STATE_CHANGE 1, LSI_WIN/WIN2/LOSE/LOSE2 1 each; per-job START/WIN/WIN2/LOSE/LOSE2 sets for LLC/LCG/LLD/LCN/LDB/LDN. First LSI table `LSI_WAIT_STATE_CHANGE:815`, last `LSI_SA_LCN_RAIN_OF_CHAOS:22760`.
- `Luciel_Demonio.src.lua`: 7,524 lines, 144,912 B, 158 top-level tables, no INIT: LSI_SA_ 94 (LDN 56, LDB 36), LSI_BOUNDING_ 30, LSI_LDB_ 19, LSI_A_ 14 (LDN 8, LDB 6), LSI_HA_LDN_ 1. First `LSI_BOUNDING_TARGET_LU_STRIKER:1`, last `LSI_SA_LDN_ABYSS_EXTINCTION:7429`.
- Special keys: `DUO_TYPE` (Duo x390, Demonio x158), `MINOR_UNIT_ANIM_NAME` (247/108), `MINOR_UNIT_SPEED_TIME0-4`, `MINOR_UNIT_SPEED_X`, `MINOR_UNIT_AFTER_IMAGE`, `MINOR_UNIT_MOTION_FILE_NAME` (INIT_MOTION, x1), `NOT_SHOW_UNIT_BY_DUOTYPE` (Duo:3920, :4598), `HIT_ADD_COMBINATION_POINT` x6, `CP_AMOUNT` x2, `DUNGEON_SPEECH_LUCIEL` (INIT_SYSTEM, Duo:1-22, keys `LU`/`CIEL` x4 with STR_ID_33342-33349), `DEFAULT_UNSELECTED_UNIT_MP_CHARGE_VALUE_PER_SECOND(_AT_PVP)` (INIT_COMPONENT), `ENABLE_ATTACKBOX` x17, `DECREASE_FORCE_DOWN`, `DAMAGE_RATE_TO_SUPERAMOR`, `SUPER_ARMOR_FRONT`, `CUSTOM_STATE_DELETE_SOUND`. Demonio-only: `USE_NOT_SELECT_MOTION_DIFFERENCE` 15, `USE_PHYSICPROCESS_NOT_SELECT_UNIT` 15, `USE_NOT_SELECT_MOTION_BEFORE_POS` 13, `CUSTOM_OFFSET_NOT_SELECT_UNIT` 10, `USE_NOT_SELECT_UNIT_ANIM_TIME_IN_SPEED` 3, `SUPER_ARMOR_CUSTOM_COLOR` 49, `AFTER_IMAGE_COLOR` 5. Raw word counts: Demonio `NOT_SELECT` 56, `TRANSFORM` 64; Duo `COMBINATION` 18, `SECOND` 14, `SWITCH` 2, `NOT_SELECT` 0.
- No script key in UnitTemplet names the class script; script names are `Luciel_Duo` / `Luciel_Demonio` by file only (C++ side not scanned).
- NPC / summon scripts (all already present as .lua in `K/data037_kom`, plus tutorial in data035/data040): `CSM_Lu_Diabla` 236 lines, `CSM_Ciel_Demonio` 168, `TIME_LU1/2/3` 1458/1468/1432, `TIME_LU_SUMMON` 155, `TIME_CIEL1/2/3` 1651/1645/1629, `TIME_CIEL_SUMMON` 192, `SA_LDN_GARDEN_OF_HAURES` 382, `SA_LDN_GARDEN_OF_HAURES_ADD` 154, `SI_SA_LCN_SHADOW_BOLT` 379, `SI_SA_LLD_SUMMON_DARKNESS` 561, `SI_SA_LLD_SUMMON_DARKNESS_PVP` 561, `RAID_SHADOW_CIEL/RUCIEL` 1463 each (event: `datai016_kom/EVENT_LUCIEL_DARKNESS` 575, `EVENT_RAID_SHADOW_CIEL` 1463, `PET_Mystic_LUCIEL_3` 1912, `datai016 PET_Mana_Eater_LuCiel` 1881). No script file exists for `NUI_SI_A_LCN_SHADOW` (1907): `find june_src_fixed -name 'SI_A_LCN*'` = none.
- Present in March kom_contents (ls): all above NPC scripts, `Dungeon_Tutorial_Luciel.lua` (data035), `Tutorial_Luciel_*` (data037), `DLG_Transcendental_Change_Guide_Luciel.lua` (data034). NOT present: `Luciel_Duo.lua`, `Luciel_Demonio.lua` (data036), `Class_Change_Guide_Luciel*`, `DLG_1ST/2ND_Class_Change_Guide_Luciel`, `DLG_LUCIEL_BOUNDING_KEY`, `DLG_Luciel_CombinationPoint`, `DLG_LUCIEL_TRANSFORM_SKILL_SLOT`, `DLG_UI_JUMPING_EVENT_LUCIEL`, `DLG_Unit_Create_ClassChange_Button_Luciel` (only the Transcendental one listed in data034_kom), `data217 DLG_Class_Change_Luciel_{CG,CN,DB,DN,LC,LD}`, `data040 Tutorial_Luciel_Stage0*` (K/data217_kom and K/data040_kom exist but hold no luciel file); `datai016_kom/EVENT_LUCIEL_DARKNESS.lua` IS present. UI sizes: DLG_Luciel_CombinationPoint 169 lines, DLG_LUCIEL_BOUNDING_KEY 29, DLG_LUCIEL_TRANSFORM_SKILL_SLOT 293.

## 3. UnitTemplet rows (`UnitTemplet.src.lua`, 87 rows total; Lu/Ciel = 7)
Row starts (`g_pUnitManager:AddUnitTemplet`): 1723 DUO, 1765 CHILIARCH, 1807 ROYAL_GUARD, 1849 DIABLURA, 1891 DREAD_LORD, 1935 NOBLESSE, 1979 DEMONIO (`unitClass =` at 1759/1801/1843/1885/1929/1973/2019). LIVE `K/data036_kom/UnitTemplet.lua` (BOM text) has 0 `UC_LUCIEL`: March has no Lu/Ciel row.
| class | GameMotion1 | GameMotion2 | MinorGameMotion1/2 | intro movie | HyperCutIn (major / minor) |
|---|---|---|---|---|---|
| DUO | "" | "" | "" / "" | Movie_Create_Luciel_Base.avi | none |
| CHILIARCH | Motion_Lu_LC.x | "" | Motion_Ciel_LC.x / "" | Movie_Create_Luciel_LD.avi | none |
| ROYAL_GUARD | Motion_Lu_CG.x | "" | Motion_Ciel_CG.x / "" | Movie_Create_Luciel_CN.avi | none |
| DIABLURA | Motion_Lu_LDB.x | "" | Motion_Ciel_LDB.x / "" | Movie_Create_Luciel_LN.avi | none |
| DREAD_LORD | Motion_Lu_LC.x | Motion_Lu_LD.x | Motion_Ciel_LC.x / Motion_Ciel_LD.x | Movie_Create_Luciel_LD.avi | HQ_Cutin_LLD_LU_Transcendence / HQ_Cutin_LLD_CIEL_Transcendence |
| NOBLESSE | Motion_Lu_CG.x | Motion_Lu_CN.x | Motion_Ciel_CG.x / Motion_Ciel_CN.x | Movie_Create_Luciel_CN.avi | HQ_Cutin_LCN_LU_/_CIEL_Transcendence |
| DEMONIO | Motion_Lu_LDB.x | Motion_Lu_LDN.x | Motion_Ciel_LDB.x / Motion_Ciel_LDN.x | Movie_Create_Luciel_LN.avi | HQ_Cutin_LDN_LU_/_CIEL_Transcendence plus `_Trans` pair |
Common: `m_GameMotion0 Motion_Lu.x`, `m_MinorUnitGameMotion0 Motion_Ciel.x`, Lu Field/Common/Lobby/Emotion = `Motion_Lu_{Field,Common,Lobby,Emotion}.x`, Ciel = `Motion_Ciel_*.x`. Description ids: DUO STR_ID_33020, LC 33626, CG 33627, LDB 39441, LD 33868, CN 33872, LDN 39445. DEMONIO row (1979-2025): name STR_ID_39442, majorUnitName 39443, minorUnitName 39444, basic*/promotion* item ids 42103-42107 / 42111-42115 (minor).
Keys no other class row has (computed over the 80 non-Lu rows): `m_HyperCutIn_Trans`, `m_MinorUnitHyperCutIn`, `m_MinorUnitHyperCutIn_Trans`, `m_MinorUnitCommonMotion`, `m_MinorUnitEmotionMotion`, `m_MinorUnitFieldMotion`, `m_MinorUnitLobbyMotion`, `m_MinorUnitGameMotion0/1/2`, `m_MinorUnitPromotion{Body,Foot,Hand,Leg,Weapon}ItemID`, `m_fMinorUnitLobbyRotationY`, `majorUnitName`, `minorUnitName` (18 keys). Lu rows lack `m_BerserkCutIn` and `m_RavenLeftArmItemID`.

## 4. Skill tree / templet
- `s3_reform_ids_no_state.tsv` (2,044 rows, header `unit_class_name ... skill_id ...`): NO Lu/Ciel rows (the tsv only lists classes with a March family cpp file). Not found. `s0a_skilltree_tu_map.tsv` is a June-exe function map, no per-class rows.
- June source: `june_src_fixed/data036_kom/NewSkillTempletVer2.src.lua` (299,582 lines; 2,346 `AddSkillTemplet_LUA`, 3,360 `AddSkillTreeTemplet_LUA`). Tree rows with `m_eUnitClass = UC_LUCIEL_*` (first at line 282612) = 301 total:
  DUO 19 (ids 900..30302), CHILIARCH 35 (900..30402), ROYAL_GUARD 35 (900..30407), DIABLURA 35 (900..30412), DREAD_LORD 59 (106..30603), NOBLESSE 59 (106..30604), DEMONIO 59 (106..30606); every class's rows are all-distinct ids.
  Templets: 52 with ids 10000-10699, 143 with ids 30001-30999 (195 Lu/Ciel-specific); 137 templets are reachable from Lu/Ciel tree rows (incl. shared ids). Example templet `m_eID = 10210` Shadow (line ~131509).
- `NewSkillTempletVer2_LA.src.lua` has the same 301 `UC_LUCIEL` hits. `SkillLevelUpSocketGroup.src.lua` 419, `SkillNoteTemplet.src.lua` 7 hits.
- Other per-class data with Lu/Ciel rows: `CorrelateUnitClassTemplet.src.lua:138-145` (6), `SelectVoiceTable.src.lua` (67), `AnimationNameTable.src.lua` (35), `CharCustomData.src.lua` (22), `FieldQuest.src.lua` / `FieldQuest_LA` (130 each), `CharacterStatTable.src.lua`.

## 5. Motions / assets
Source: `june2017_re/assets_cache/inspect/14JUN17_data_*.kom_*.json` manifests joined to `ls` of K.
- Motion files, 125 total: **data208 = 62** (`Motion_Lu*.x`, `Motion_CSM_Lu_LDB.x`) and **data218 = 63** (`Motion_Ciel*.x`, `Motion_CSM_Ciel_LDN.X`).
- Core (non-emotion) list: data208 `Motion_Lu.x`(9,270,265 B) `_CG` `_CN` `_Common` `_Emotion` `_Field` `_LC` `_LD.X` `_LDB` `_LDN` `_Lobby` `_LDB_Evil.x` `Motion_CSM_Lu_LDB.x`; data218 `Motion_Ciel.x` `_CG` `_CN` `_Common` `_Emotion` `_Field.X` `_LC` `_LD` `_LDB.X` `_LDN.X` `_Lobby` `_LDN_Evil.X` `Motion_CSM_Ciel_LDN.X`. The rest are `*_Emotion_<name>.x` (Cash1..43, event, K-pop): 49 in each kom.
- March already has the same name for exactly 4: `K/data208_kom/{Motion_CSM_Lu_LDB.x, Motion_Lu_LDB_Evil.x}` and `K/data218_kom/{Motion_CSM_Ciel_LDN.X, Motion_Ciel_LDN_Evil.X}` (staged by earlier phases; `asset_closure.tsv` rows `Motion_Ciel_LDN_Evil.X` / `Motion_Lu_LDB_Evil.x` = `june-only missing`). The other 121 motion names are absent from every `K/*_kom`. data208 / data218 are new koms (only those 2 files each present in `K/data208_kom`, `K/data218_kom`; `data/data208.kom`, `data218.kom` exist in the game dir).
- Wider `Mesh_/Motion_ (Lu|Ciel)` name match: 8,487 June entries (data158 3,817, data159 4,008 incl. costume mesh/tga/xet, datai028/datai029 ~250 each, plus data016/021/157), of which 70 names already exist in K.
- `ga_staged_files.tsv`: HQ cut-in gfx/dds for Lu/Ciel staged in data152 / data195, state `new` (e.g. `HQ_CutIn_LuCiel_01.gfx data152 1521 new`); the six `HQ_Cutin_L{LD,CN,DN}_{LU,CIEL}_Transcendence.gfx` (+ `_Trans` for LDN) are present in `K/data195_kom` (ls).

## 6. Tutorial dungeon
- `DungeonEnum.src.lua:101  DI_DUNGEON_TUTORIAL_LUCIEL = 41240,` and `:56 DI_BATTLE_FIELD_TUTORIAL_LUCIEL = 39609,`. LIVE `K/data036_kom/DungeonEnum.lua` has only `:396 DI_BATTLE_FIELD_TUTORIAL_LUCIEL = 39609,` (41240 missing live).
- `data035_kom/DungeonData.src.lua:29878` row: `dataFileName = "Dungeon_Tutorial_Luciel.lua"`, `dungeonID = DUNGEON_ID.DI_DUNGEON_TUTORIAL_LUCIEL`, `dungeonName = STR_ID_35437`, `m_LoadingScreenFileName = "HQ_Loading_TUTORIAL_LUCIEL_01.tga"` / `_02`, `m_NPCLevel = 1`, checks `DCC_TUTORIAL_DUNGEON, DCC_NOT_DECREASE_SPIRIT`. LIVE `K/data035_kom/DungeonData.lua` has 0 hits for `Dungeon_Tutorial_Luciel`.
- Script `data035_kom/Dungeon_Tutorial_Luciel.src.lua` 2,244 lines; already in K as June bytecode (`K/data035_kom/Dungeon_Tutorial_Luciel.lua`, 58,663 B, `1B 4B 4C 81`). World `WI_TUTORIAL_LUCIEL_STAGE0 = 633` (`Tutorial_Luciel_Stage0.src.lua` 658 lines + `_LineMap` 294, data040).
- `c3y_june_only_dungeon_ids.tsv:38`: `DI_DUNGEON_TUTORIAL_LUCIEL	41240	-	-	Dungeon_Tutorial_Luciel.lua	unreached-script	kc/june	backlog Stage 8`.
- `c3y_unreached_dungeon_census.tsv:101`: `data035	Dungeon_Tutorial_Luciel.lua	june	DI_DUNGEON_TUTORIAL_LUCIEL=41240	WI_TUTORIAL_LUCIEL_STAGE0	1	0	5	NUI_TUTORIAL_BOSS_KARIS,NUI_TUTORIAL_DUMY1,NUI_TUTORIAL_DUMY2,NUI_TUTORIAL_ICOOS,NUI_TUTORIAL_RUCIEL_OBJECT	june-bytecode	no	backlog Stage 8`.
- `dungeon_enum_census.tsv:148`: `DI_DUNGEON_TUTORIAL_LUCIEL		41240		june-only`. `dungeon_registry_census.tsv`: no row for 41240 (0 matches).
- Tutorial NPC scripts: `Tutorial_Luciel_boss_karis` 678, `_icoos` 973, `_object` 227, `Tutotial_Luciel_Dp1` 197.

## 7. Stat rows
- `KncWX2Server/ServerResource/US/StatTable.lua` (5,182 lines): `StatTable:ReserveMemory(<classId>, 99)` / `SetUnitStat(<classId>, ...)` keyed by numeric class id. Ids present: 1-8, 10-31, 100-119 only. Class ids 37, 38, 39, 127, 128, 129, 200 are ABSENT (also 9, 32-36, 120-126 absent). No `LUCIEL`/`UC_` text in the file.
- Packed `K/data036_kom/StatTable.lua` is bytecode (`1B 4B 4C 81`), not parsed. Game-dir `offline_server.log` shows `STAT     WARNING no rows for class=32 / 122 - using the synthetic curve`: missing classes fall back to `X2OfflineStatTable::SyntheticStat` (`X2Lib/Offline/X2OfflineStatTable.cpp:316-340`). No Lu/Ciel warning because no Lu/Ciel character exists yet.
- Client `CharacterStatTable.src.lua`: `UNIT_TYPE.UT_LUCIEL` row (`:167`, values 430,98,410,390,100,530,115,10500) and second-job relative rows `UC_LUCIEL_DREAD_LORD :417`, `NOBLESSE :425`, `DEMONIO :433`. LIVE `CharacterStatTable.lua` (bytecode) already contains `UT_LUCIEL`, `UC_LUCIEL_DEMONIO`, `UC_LUCIEL_NOBLESSE`, `UC_LUCIEL_DREAD_LORD` strings.
- `ExpTable` / `NpcExpTable`: not per class (levels / NPC level only; `ExpTable.src.lua` rows are `TOTAL_EXP, LEVEL, NEED_EXP`; no `UC_` in `NpcExpTable.lua`). Nothing Lu/Ciel to add. `june2017_re/xstat_balance.tsv` is NPC-only (no Lu/Ciel row).

## 8. Transcendence
- `data034_kom/DLG_Transcendental_Change_Guide_Luciel.src.lua` (203 lines; one of 12 per-character guides); `K/data034_kom/DLG_Transcendental_Change_Guide_Luciel.lua` already present.
- `LevelUpEvent.src.lua` rows: `:158 UC_LUCIEL_DUO` (TYPE9, LEVEL 15, `DLG_1ST_Class_Change_Guide_Luciel.lua`, `LucielVoice_ClassUpReady01.ogg`), `:381-383` CHILIARCH/ROYAL_GUARD/DIABLURA, `:627-629` DREAD_LORD/NOBLESSE/DEMONIO; `datai015_kom/LevelUpEvent_A.src.lua:158` DUO.
- Cut-in names (UnitTemplet 1891-2019): `HQ_Cutin_LLD_{LU,CIEL}_Transcendence`, `HQ_Cutin_LCN_{LU,CIEL}_Transcendence`, `HQ_Cutin_LDN_{LU,CIEL}_Transcendence` + `_Trans`; gfx present in `K/data195_kom` (ga_staged_files rows, state `new`). No separate Berserk cut-in key for Lu/Ciel.

## 9. Gap scan
- `python charport_keys.py Luciel_Duo` and `Luciel_Demonio` both RAN (no error; class unknown to March is not an error here). Output saved: `june2017_re/char_luciel/plan_charport_keys.txt` (55 lines).
- Duo: `top-level tables: 406 (INIT_*: 6)   distinct keys: 230   key occurrences: 11463`; `verdicts: june-only=23  neither=3  march-only-site=4  both=200`. june-only: BBT_CHANGE_NOW_HP_PER_SECOND, BFT_TIME, CIEL, CP_AMOUNT, DEFAULT_UNSELECTED_UNIT_MP_CHARGE_VALUE_PER_SECOND(_AT_PVP), DUNGEON_SPEECH_LUCIEL, DUO_TYPE, FORCE_DYNAMIC_CAMERA_INDEX, IMMUNE_TO_SUPER_ARMOR_DOWN, IMMUNITY_EDT_LIST_AT_THIS_STATE, LU, MINOR_UNIT_AFTER_IMAGE, MINOR_UNIT_ANIM_NAME, MINOR_UNIT_MOTION_FILE_NAME, MINOR_UNIT_SPEED_TIME0-4, MINOR_UNIT_SPEED_X, NOT_SHOW_UNIT_BY_DUOTYPE, RETAIN_CAMERA_STATE. neither: HEAD_IK, HEAD_IK_MAX_ANGLE, MINOR_SPEED_TIME0.
- Demonio: `top-level tables: 158 (INIT_*: 0)   distinct keys: 127   key occurrences: 3525`; `verdicts: june-only=12  neither=2  march-only-site=3  both=110`; adds SUPER_ARMOR_CUSTOM_COLOR, USE_NOT_SELECT_MOTION_BEFORE_POS, USE_NOT_SELECT_UNIT_ANIM_TIME_IN_SPEED, USE_PHYSICPROCESS_NOT_SELECT_UNIT; neither: CUSTOM_RENDER_STATE, STOP_TIME_ATK. (Note: the tool does not list USE_NOT_SELECT_MOTION_DIFFERENCE / CUSTOM_OFFSET_NOT_SELECT_UNIT / AFTER_IMAGE_COLOR as june-only, so those count as `both` / March-read.)
- charport_scan output quotes (`char_chung/plan_charport_scan_20261031.txt`, identical in `char_arme/..._20261020.txt:11-12,37` and `char_eve/..._20261025.txt`):
  `:11  Luciel_Demonio               {'no NUMERICAL_DATA_INDEX': 11, 'no DAMAGE table': 2}`
  `:12  Luciel_Duo                   {'no NUMERICAL_DATA_INDEX': 67, 'no DAMAGE table': 15}`
  `:37  Luciel_Demonio               {'CUSTOM_RENDER_STATE': 3}`

## Gaps / not found
- s3 skill-tree tsv rows for Lu/Ciel: not found (use NewSkillTempletVer2.src.lua counts above).
- Stat rows for Lu/Ciel in server StatTable.lua: not found (classes 37/38/39/127/128/129/200 absent).
- Script name binding (which C++ call loads `Luciel_Duo.lua`): not looked for (June side only).
- LIVE Enum.lua lacks DUO_TYPE members DT_BOTH=2 / DT_KEEP=3 and live DungeonEnum lacks DI_DUNGEON_TUTORIAL_LUCIEL=41240; neither added (read-only job).
