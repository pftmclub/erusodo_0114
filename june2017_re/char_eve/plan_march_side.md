# Eve March-side facts (read-only survey, 2026-10-25)

Paths relative to Trunk. KOM = `F:/elsword stuff/elsword_2014/els_2014/237311/24965799/kom_contents/`.

## 1. Class source
- `X2Lib/X2GuEve.cpp` 34813 lines (CP949, CRLF+LF mixed -> bytepatch.py only); `X2Lib/X2GuEve.h` 2629 lines. The file name is `X2GuEve` (lowercase u). Class `CX2GUEve : public CX2GUUser` (.h:3).
- UT_EVE = 5 (`X2Unit.h` UNIT_TYPE: NONE0, ELSWORD1, ARME2, LIRE3, RAVEN4, EVE5). Script `EVE_NASOD.lua` (`X2UnitManager.cpp:414` array, `X2GUUser.cpp:42219` `UT_EVE -> L"EVE_NASOD.lua"`); square scripts `X2SquareUnit.cpp:4189-4200` (SQUARE_Eve_Nasod / _Nasod_Exotic / _Nasod_Architecture / _Nasod_Electra; no Nemesis/Empress/Seraph lines in that range). No `SQUARE_Eve*` file exists in kom_contents (not found).
- Landmarks in X2GuEve.cpp: s_SkillCutInSet :48-85, ctor :87, InitStateID :655, InitStateCommon :673, InitState :1605 (cut-in index switch :1612-1640, sub-index :1642-1657), SetEquippedSkillFuncBySkillSlotIndex :2153-3212, InitComponent :3366, SpecialAttackEventProcess :5691..~6201 (next fn SpecialAttackNoStageChange :6203), ActiveSkillShow use :25857-25859.
- Dead flag: `ADD_EVE_SYSTEM_2014` (maneuver gauge, `_CONST_EVE_`) is commented out (`Always.h:58`), so its blocks (:23-45, :6126-6134 ...) are dead. Live: `EVE_SECOND_CLASS_CHANGE` (Always.h:2759), `EVE_ELECTRA` (Always.h:2402), `SERV_EVE_BATTLE_SERAPH` (ServerDefine.h:3544), `UPGRADE_SKILL_SYSTEM_2013` (Always.h:1643, from the Aisha note). Others in the big function: FINALITY_SKILL_SYSTEM, ADDED_RELATIONSHIP_SYSTEM, SKILL_30_TEST, SKILL_CASH_10_TEST, NEW_SKILL_2010_11 (verify the branch with cl /EP before editing).

## 2. UNIT_CLASS (`X2Lib/X2Unit.h:37`)
- :46 `UC_EVE_NASOD // 5` (base)
- :62 `UC_EVE_EXOTIC_GEAR // 18`, :63 `UC_EVE_ARCHITECTURE // 19` (1st jobs)
- :78-80 `#ifdef EVE_ELECTRA  UC_EVE_ELECTRA = 26` (1st job; flag live, Always.h:2402)
- :108 `UC_EVE_CODE_NEMESIS // 108`, :109 `UC_EVE_CODE_EMPRESS // 109` (2nd jobs, unconditional)
- :127-129 `#ifdef SERV_EVE_BATTLE_SERAPH  UC_EVE_BATTLE_SERAPH // 116` (2nd job; flag live, ServerDefine.h:3544)
- No dead/commented Eve class lines. (`UC_ADD_NASOD_RULER` :50 is Add, not Eve.) `X2UnitManager.cpp:172-192` UnitClassNameMap has only 5/18/19/108/109 for Eve (no 26/116 by grep).

## 3. SetEquippedSkillFuncBySkillSlotIndex
- `X2GuEve.cpp:2153` .. closing brace `:3212` (.h:1359). 63 textual `case CX2SkillTree::SI_*` labels (both arms of `#ifdef UPGRADE_SKILL_SYSTEM_2013` pairs counted); first `SI_ETC_WS_COMMON_LOVE` :2200, last `SI_HA_EBS_PSYCHIC_ARTILLERRY` :3197.
- Shape: state ids from `aNormalSkillStateID[ / SlotB]`, `aHyperSkillStateID[ / SlotB]` (:2158, :2163); first `switch(m_eType)` :2173 (relationship inner switch :2198, default :2214); second `switch(eSkillID_)` :2223, `default:` :2225.
- Blocks inside: FINALITY_SKILL_SYSTEM :2189, :3170; EVE_SECOND_CLASS_CHANGE :2607; NEW_SKILL_2010_11 :2701; EVE_ELECTRA :2735; SERV_EVE_BATTLE_SERAPH :2902.
- Skill names are all March SI_* (EN / EEG / EAT / ENS / EEP / EEL / EBS); no June-only names.

## 4. Pressed-index / slot decode sites (`X2GuEve.cpp`; all assume 4 per set: B = index>3, state = index-4)
- `SpecialAttackEventProcess` :5691; `CommonSpecialAttackEventProcess(..., iPressedSkillSlotIndex)` :5707 (base `X2GUUser.cpp:25439`; its slot-5 branches never set the index); `GetSkillSlotDataMegaElectronBall` :5715, TaserPilum :5730, KugelBlitz :5744; `CountUsedSkill` :5977.
- State change :6151-6169 (`> 3`, `aNormal/aHyperSkillStateID[SlotB][idx-4]`); `m_eSpecialAttackKeyPressed = idx+1` :6174 (EVE_ELECTRA); `m_iNowSpecialAttack = idx+1` :6177.
- Eve's own 4+4 slot encoders (checklist 7ag, flag SERV_IRUHADEV_JUNE_SKILL_SLOT_10, S5 review): InitComponent loops :3391-3400, :3426-3434, :3465-3473 (`if iSlot >= 8 break`, `iSlot > 3` = B, `-4`); Taser Pilum :5529-5537 and Kugel Blitz :5590-5598 (replace `GetSlotIndexAndSlotB` with `>3` / `-4`; `#else` keeps June's 5+5 call).
- Other unflagged slot-id decoders: MegaElectronBall `:5481-5500` (`< EQUIPPED_SKILL_SLOT_COUNT*2`, direct index into aHyper/aNormal[SlotB]) and `:22110`, `:22149-22150`, `:22176`, `:33454`; TaserPilum `:29793`, `:29834-29835`, `:29863`; KugelBlitz `:32358`, `:32399-32400`, `:32420`; key-held checks :28451-28458, :32125-32128; `GetSkillSlotIndexUsed` call :16232.
- Tables: `X2GUUser.h:5008-5025` (5-entry under SKILL_SLOT_10); EQUIPPED_SKILL_SLOT_COUNT 5 (CommonPacket.h:1096).

## 5. Existing SERV_IRUHADEV_ flags in Eve files
- `X2GuEve.h`: none. `X2GuEve.cpp`: 20 lines mention a flag; 9 `#ifdef` sites, 5 distinct flags:
  - SERV_IRUHADEV_JUNE_SKILL_SLOT_10 x5: :3393, :3428, :3467, :5531, :5592 (last two have `#else`)
  - SERV_IRUHADEV_JUNE_BUFF_QUICKSLOT_ITEM :5693-5698
  - SERV_IRUHADEV_JUNE_BUFF_RIDING_KEY_WAIT :5784-5789
  - SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL :6138-6142 (ApplySkillCoolTimeRateByBuff)
  - SERV_IRUHADEV_JUNE_RANDOM_MISSION :6143-6146 (OnSkillUse)
- Absent (vs Aisha): BUFF_BEHAVIOR_PARITY, SKILL_STATE_PTR.

## 6. Lua
- Class script: `data036_kom/Eve_Nasod.lua`, 138560 B, header `1B 4B 4C 81` (bytecode, not BOM source). Not on `tools/data/lua_core_list.tsv` (0 Eve hits) and not in `tools/data/lua_source_ledger.tsv`. Editing it needs `luasrc.py core --add` + convert / `adopt-june --source`. No snapshot in `kom_contents_bytecode/data036_kom` (only LevelUpEvent, SkillLevelUpSocketGroup, UnitTemplet there).
- Other Eve-named Lua: `data119_kom/Eve_AT_Sp2a_SonicWave.lua` 683 B (`1B 4B 4C 81`).
- `data036_kom/UnitTemplet.lua`: BOM source (`EF BB BF`), 1432 lines, core-list member (row "data036 UnitTemplet.lua ... pilot-char-revamp CH5") and ledger row (origin march, tier T1, phase CH5, 2026-10-21, live). Eve rows (`g_pUnitManager:AddUnitTemplet({` .. `})`): Nasod 372-397 (unitClass :380); Exotic Gear 398-423 (:412); Architecture 424-449 (:434); Code Nemesis 664-689 (:672); Code Empress 690-715 (:699); Electra 920-945 (:932); Battle Seraph 1053-1078 (:1062). All `unitType = UNIT_TYPE.UT_EVE`. Motions named: Motion_EVE, _Common, _Field, _Emotion, _Lobby, _EG (Exotic; also Nemesis game1), _AT (Architecture; also Empress game1), _NS (Nemesis game2), _EP (Empress game2), _PR (Electra game1), _BS (Seraph game2). Intro movies Movie_Create_Eve_{Base,CN,CE,BS}.avi.
- `StatTable.lua` (server Lua): `KncWX2Server/ServerResource/US/StatTable.lua` is UTF-8 BOM, 633758 B, `ReserveMemory(c, 99)` for every class. Classes 5, 18, 19, 26, 108, 109, 116 each have exactly 99 `SetUnitStat` rows, levels 1..99, no gaps, no duplicates. The kom copy `data036_kom/StatTable.lua` is bytecode (436432 B), not read; the loose server file is the evidence.

## 7. Motions (kom_contents; Eve-specific only; `Motion_Nasod_*` are Nasod monsters, not Eve)
- data033_kom: Motion_EVE.x 4315889, _AT 1652348, _BS 1691814, _Common 6062982, _EG 1907285, _EP 1371947, _Emotion 961642, _Emotion_APINK.X 785814, _APINK_LOVE.X 313114, _BIGBANG.X 986181, _Field 441322, _Lobby 1822129, _NS 2016900, _PR 1836432, Motion_Eve_Emotion_CRAYONPOP_Sorted.X 699973, Motion_Eve_Emotion_DeepBow.X 458669.
- Other: data012 Motion_Eve_Buff_NPC.X 426516; data011 Motion_Nasod_Element_Fire_Eve.x 272062, Motion_Nasod_Watch_Eve.xet 140, Motion_Wally9th_Eve.x 346063; data111 MOTION_HAMEL_SUMMON_DUMMY_EVE.x 1362306; data119 Motion_Guardian_Eve.x 408189; data165 Motion_EVENT_TUTORIAL_EVE_STAGE_Direct.x 1271393, Motion_Tutorial_Eve_object.x 37309; data225 Motion_Time_Eve.x 2675225 (+_Weapon_A.X 12689, _Weapon_B.X 11578); datai024 MOTION_PVP_EVE.x 1362220; datai027 Motion_Event_Halloween_Ginger_Eve.x 1359264; datai002 MOTION_EVENT_Battle_Seraph.x 1266003.

## 8. Cut-ins
- `X2GuEve.cpp:48-85` `s_SkillCutInSet[]` (3 SkillCutIn each): [0] HQ_CutIn_Eve_1/2/3.dds (Nasod, Exotic, Architecture; Electra has no case in InitState :1612-1640, so it keeps the ctor default 0, `X2GUUser.cpp:687`); [1] HQ_CutIn_Eve_NM_1.dds x3 (Nemesis, under EVE_SECOND_CLASS_CHANGE); [2] HQ_CutIn_Eve_EP_1.dds x3 (Empress); [3] HQ_CutIn_Eve_BS_1.dds x3 (Seraph, under SERV_EVE_BATTLE_SERAPH). `s_SkillCutInSetFoolsDay` :20 = HQ_CutIn_Eve_Gura.dds. Sub-index: Exotic 1, Architecture 2, else 0 (:1642-1657). Use site :25857-25859.
- Files: data044_kom HQ_CutIn_Eve_1/2/3, _BS_1, _EP_1, _Gura, _NM_1 (.dds, 262272 B each); data152_kom has June-style `.gfx` HQ_CutIn_Eve_01..03, _BS_01, _EP_01, _NM_01, many _CASH_nn (+ _I*.dds); datai019_kom HQ_CutIn_Eve_1/2/3.tga.

## 9. Offline emulator
- Job change is class-generic (Handlers_Quest.cpp ~555-590 quest route, Handlers_Shop.cpp ~704-795 ticket route). Eve ids appear only in tables: `X2OfflineClassChangeSeed.h` (tickets 214490/214500/217970/217980; 242070/242080/242090/242250/242260/242270; 242350 Electra; 252680 Seraph; parent table :261-300: 18->5, 19->5, 26->5, 108->18, 109->19, 116->26; tier list :316 has 26), `X2OfflineLuaEnumSeed.h:66-110` (UC_EVE_*), `Handlers_Room.cpp:124` (AI-party name "Noah", UC_EVE_NASOD only), `X2OfflineInventory.cpp:1232` (beginner gear row for unit type 5), `X2OfflineQuest.cpp:900/906` (DI_TUTORIAL_EVE, DI_BATTLE_FIELD_TUTORIAL_EVE). No Eve-specific logic.

## 10. Existing save (read-only `elsdb.py chars`)
Eve units: uid 19 Iivue (109 EMPRESS, lv80), uid 22 weevwe (5 NASOD, lv99), uid 27 jtCodeEmpress (109, lv80), uid 41 ElectraT (26 ELECTRA, lv90). No class 18, 19, 108 or 116 unit exists in the save.

## Not found / caveats
- No SQUARE_Eve*.lua in kom_contents. Origin (March vs June ljconv) of Eve_Nasod.lua not determined. A regex count of StatTable `SetUnitStat` gave 4988 vs 50 x 99 = 4950 (cause not checked); the per-class counts above are exact.
