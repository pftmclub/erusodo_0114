# char-luciel: March-side planning evidence (read-only digest, 2026-11-02)

Scope: what the March 2014 client tree already has for Lu/Ciel (June `UT_LUCIEL` = 10, creation class `UC_LUCIEL_DUO` = 200),
and what a port has to touch. Paths are relative to the Trunk. Line numbers are 1-based, CP949 files read as bytes.
Flag states are read from `KTDXLIB/Always.h` `#define` lines (live = uncommented `#define`).

## 0. Headline findings

1. There is no Lu/Ciel class code in March. Every hit is an enum name (`SERV_IRUHADEV_JUNE_ENUM_NAMES`), a June-parity table, a
   `stand-in: Lu/Ciel not ported` branch, or a comment routing the work to "checklist 7x / Stage 8". No `CX2GULuciel`, no `UT_LUCIEL` case in any factory.
2. `UT_LUCIEL = 10` shares `UT_END`'s value (X2Unit.h:34-36). Everything sized or looped on `UT_END` is one short for Lu/Ciel (section 3).
3. June creates Lu/Ciel directly at class 200 (`june_src_fixed/data036_kom/UnitTemplet.src.lua:1759`, `UC_LUCIEL_DUO`); June's `UNIT_CLASS` has no other base-tier Lu class.
   Class 200 overflows every `char m_cUnitClass` in the shared packet structs (signed char, section 2). Class 127 is the last value that survives; 128-129 and 200-202 wrap negative.
4. Offline emulator: `X2OfflineStatTable::GetUnitStat` has a silent wrong fallback for Lu/Ciel (section 4): `UnitTypeOf(200)` returns 10, which finds `StatTable.lua`'s class-10 rows (Elsword Knight), no warning.
5. `GBASE_ITEM` (starter item table) has rows for classes 1-9 only; class 200 logs "no row for class" and the character starts with no gear or costume. A sibling session already wrote
   `june2017_re/char_luciel/plan_starter_items.tsv` (Lu gear 113300-113304; costume 41890-41894 vs Ciel 41900-41904 ambiguous). Not duplicated here.
6. The create screen's buttons come from a Lua dialog (`DLG_Unit_Create_Unit_Button_New.lua`); March's live copy is March bytecode with no Lu slot, June's source has `LucielCreateUnitSlot`.

## 1. Existing Lu/Ciel code in March (grep `luciel|lu_ciel|UT_LUCIEL|CIEL` over X2Lib / KTDXLIB / X2ServerProtocol / KncWX2Server/Common)

Verdict key: live = compiled and reachable now; inert = compiled, but unreachable because no Lu/Ciel unit exists; comment = comment text only.
Flags named are all defined live in Always.h (lines: ENUM_NAMES 10164, DIVERGE_UNIT_REFORM 10823, CHAR_ARME_CORE 9614, CHAR_EVE_CORE 10111, SKILL_CLASS_POPUP 9271,
TRANSCEND_CHANGE 9980, CHARINFO_VIEW 10978 / SHOW 10950 / STATS 10957, SKILL_TREE_UI 8587, SKILL_TREE_PANELS 8702, BUFF_BEHAVIOR_PARITY 4741,
RENDER_MOTION_CASH_EMOTION 8308, DIVERGE_DIE_FRAME_MOVE 10600). 30 files hit. NetError_def_FR/PL hits are unrelated strings (skipped).

- `X2Lib/X2Unit.h` (`JUNE_ENUM_NAMES`; live, inert): `UT_LUCIEL = 10` :36; `UC_LUCIEL_CHILIARCH 37 / ROYAL_GUARD 38 / DIABLURA 39` :167-169;
  `UC_LUCIEL_DREAD_LORD 127 / NOBLESSE 128 / DEMONIO 129` :181-183; `UC_LUCIEL_DUO 200` :191. Comment :376. (No UC_LUCIEL_CATASTROPHE/INNOCENT/DIANGELION 177-179: third jobs, out of scope.)
- `X2Lib/X2UnitManager.h:2947` `NUI_CSM_CIEL_DEMONIO = 2283` (`JUNE_ENUM_NAMES`, live); used at `X2GUUser.cpp:32697` under `JUNE_DIVERGE_DIE_FRAME_MOVE` + `SUMMON_MONSTER_CARD_SYSTEM` (live; NPC id, not the class).
- `X2Lib/X2SkillTree.h:1728-1729` `RNSI_A_LUCIEL_AURA_PUNISHMENT_ACCEL(_UPGRADE) = 30301 / 30302` (`JUNE_ENUM_NAMES`, live); read at `X2GuEve.cpp:313-316,336` ("BuffAttackCore_Luciel", `CHAR_EVE_CORE`, live; Eve skill, not Lu/Ciel).
- `X2Lib/X2Data.cpp:3907` `case UT_LUCIEL: CI_CIEL : CI_LU` in `CX2Data::SetUnitReForm`; `X2Data.h:44` `CI_CIEL = 11` (`JUNE_DIVERGE_UNIT_REFORM`; live, inert). Rose `CI_ROSE 12`, Ain `CI_AIN 13` beside it.
- `X2Lib/X2UnitViewerUI.cpp:7534` `if (UT_LUCIEL == eUnitType) return;  // stand-in` in `SetUnitReForm` (`ELSWORD_SECOND_CLASS_CHANGE` + `DIVERGE_UNIT_REFORM`; live, inert). Comments :4209, :6265, :8518.
- `X2Lib/X2GUUser.cpp:37556-37557` `case UT_LUCIEL: break; // stand-in` (`SetUnitReForm`; live, inert).
- `X2Lib/X2GUUser.cpp:33444-33539` `GetHyperModeTableName( bool bCiel_ )` (June 31fdd490; `CHAR_ARME_CORE`; live): case 10 `LSI_CIEL_HYPER_MODE / LSI_LU_HYPER_MODE` :33462; class cases 127-129 :33499-33501
  (enum names) and :33537-33539 (literal `#else`). Inert. Declaration `X2GUUser.h:2537` (`RIDING_SYSTEM` + `CHAR_ARME_CORE`).
- `X2Lib/X2GUUser.cpp:5008` `Motion_Ciel_Emotion_Cash25.X` (`JUNE_RENDER_MOTION_CASH_EMOTION`; live; a Cash25 emotion motion name, the file is not proven present).
- `X2Lib/X2BuffDisplayerTemplet.cpp:1245` class key table `... "ADD", "LUCIEL", "ROSE", "AIN"` (`HAMEL_SECRET_DUNGEON` + `BUFF_BEHAVIOR_PARITY`; live; June 32eb8c30 twelve keys).
- `X2Lib/X2StateClassChange.cpp:335-337,372-374` `DLG_Class_Change_Luciel_{LC,CG,DB,LD,CN,DN}.lua` (classes 37-39, 127-129; `SKILL_CLASS_POPUP`; live); :430 `DLG_Transcendental_Change_Guide_Luciel.lua` case 10
  (`TRANSCEND_CHANGE`; live); :923 sound prefix array with "Luciel" (`ADD_PLAY_SOUND` + `SKILL_CLASS_POPUP`; live). All inert.
- `X2Lib/X2UICharInfo.cpp`: :392 (`CHARINFO_VIEW`) and :1369 (`CHARINFO_SHOW`) test `GetType() == UT_LUCIEL`; :1872 curve names `CalculateFinalLucielSpecialization{A,B}Coefficient` (`CHARINFO_STATS`);
  :2333 / :2362 string ids 36722 / 36723 (`CHARINFO_STATS` inside `#ifndef MODIFY_CHARACTER_STAT_INFO`, which is commented out in `KTDXLIB/InHouse5.h:758`, so the block is live). All inert. Stand-ins :1316, :1912, :3462, :3797, :4096.
- `X2Lib/X2UISkillTreeNew2015.cpp`: :1906 `bLuciel = ( 10 == m_UnitType )`, :1923, :1946-1947 log "[JUNE_SKILL] UI command tab ... Luciel %d" (`SKILL_TREE_UI` + `UPGRADE_SKILL_SYSTEM_2013` + `SKILL_TREE_PANELS`; live; Lu branch false in March).
  :5633-5680 class-name switch by enum name (`ENUM_NAMES`), :5724-5771 same by number (`#else`): `LUCIEL_CHILIARCH ... LUCIEL_DUO` for 37-39, 127-129, 200. Comments :1175, :1465, :1689, :1883, :4795. `X2UISkillTreeNew2015.h:248,:300` `m_iCommandGroup`.
- Item data (`JUNE_ITEM_KIM` K2; live, no consumer): `X2Item_Preprocessing.inl:512` `FLAG_BIT2_IS_FOR_MINOR_UNIT`, `:929` `m_iItemIDForMinorUnit` (Lu item -> Ciel dummy), `:698` + `X2Item_Preprocessing.cpp:405` Extra-key remap
  (type 10 -> 10 Lu / 11 Ciel by bool, 11 -> 12, 12 -> 13); `X2SocketItem.h:1427,:1739` (types 10-12 have no March UNIT_TYPE, any other type returns 0).
- Comment / stand-in only (verdict comment): `X2Eqip.cpp:2044,:2563`; `X2GameUnit.cpp:19199,:19323`; `X2GameUnit.h:3182,:3546,:3553,:3759,:3793`; `X2GUNPC.cpp:17957`; `X2GUNPC.h:2023`;
  `X2GUUser.cpp` :5795, :9372, :11078, :15398, :16126, :16131, :16167, :16837, :22676, :27010, :31421, :33342-33343, :37401, :37447, :43176-43177; `X2GUUser.h` :2313, :2319, :2523, :3163, :3405, :4199;
  `X2GUArme_VioletMage.cpp:32005`; `X2GUChung.cpp:34946`; `X2Unit.cpp:1182,:1436,:1758`; `X2UIManager.cpp:253`; `X2BuffTemplet.h:876`; `X2DamageManager.cpp:424`; `X2EffectSet.cpp:3502`.
- `KTDXLIB/Always.h`: 10 hits, all comments or `#error` text (section 7).
- `X2ServerProtocol/` and `KncWX2Server/Common` (outside NetError): not found.

ROSE / AIN hit counts per file (regex `UT_ROSE|UC_ROSE|\bRose\b|\bROSE\b|_ROSE_` and `UT_AIN\b|UC_AIN|\bAin\b|\bAIN\b|_AIN_`; approximate; `X2GUEL.cpp`'s Rose hits are probably skill-name words):
- ROSE: X2GUEL.cpp 17, X2GUUser.cpp 14, X2BuffTemplet.cpp 13, X2UISkillTreeNew2015.cpp 12, Always.h 11, X2Unit.h 10, X2GUUser.h 9, X2GameUnit.h 6, X2GUEl.h 6, X2UnitViewerUI.cpp 5, X2UICharInfo.cpp 4,
  X2PremiumBuffManager.cpp 4, X2GameUnit.cpp 4, X2DamageEffect.cpp 3, X2BuffTemplet.h 3, X2BuffFactor.h 3, then 8 files with 1-2.
- AIN: X2GUUser.cpp 11, X2Unit.h 8, X2UISkillTreeNew2015.cpp 8, X2GUUser.h 7, X2BuffTemplet.cpp 7, X2BuffDisplayerTemplet.cpp 4, X2UICharInfo.cpp 3, X2GameUnit.cpp 3, X2BuffFactor.h 3, Always.h 3, then 12 files with 1-2.

## 2. Unit type / class enums and packet storage

`X2Lib/X2Unit.h` `UNIT_TYPE` (:17-40): `UT_NONE 0, ELSWORD 1, ARME 2, LIRE 3, RAVEN 4, EVE 5, CHUNG 6, ARA 7, ELESIS 8, ADD 9, UT_END` (= 10), then under `SERV_IRUHADEV_JUNE_ENUM_NAMES`:
`UT_LUCIEL = 10  // shares UT_END's value`, `UT_ROSE = 11`, `UT_AIN = 12` (:34-39). June has `UT_END = 13` (Enum.src.lua:305-319).
`UNIT_CLASS`: base 1-9 (`UC_ADD_NASOD_RULER 9`), jobs 10-32 (`UC_ADD_PSYCHIC_TRACER = 32`), 100-122 (`UC_ADD_LUNATIC_PSYKER = 122` is March's last), `UC_VALIDE_END` :154 (= 123), `UC_END` :158 (= 124; offline Lua seed
`X2Lib/Offline/X2OfflineLuaEnumSeed.h:117-118`), then June names after `UC_END` under `ENUM_NAMES` (:162-195): 33-46 (Lu 37-39, Rose 40-43, Ain 44-46), 123-136 (Lu 127-129, Rose 130-133, Ain 134-136),
`UC_LUCIEL_DUO 200`, `UC_ROSE_GUNNER 201`, `UC_AIN_ANGEL 202`. June's `UNIT_CLASS` (Enum.src.lua:321-437) has no Lu class below 37; `UnitTemplet.src.lua:1759` gives `UT_LUCIEL`'s entry `UC_LUCIEL_DUO`.
Server copy `KncWX2Server/Common/X2Data/XSLUnit.h:10-30`: own `UNIT_TYPE` ending `UT_ADD` + `UT_END`; no Lu/Ciel or June names. The client does not use it.

Shared packet structs with `char m_cUnitClass` (signed on MSVC; values >= 128 wrap negative). `KncWX2Server/Common/CommonPacket.h` (struct, line):
KUnitInfo 1457 (next member `int m_iUnitClassTran` :1459 under `JUNE_TRANSCEND_UNIT`), KRoomUserInfo 2279, KHenirRankingInfo 3006, KDungeonRankingInfo 3109, KPvpRankingInfo 3127,
DBE_QUEST_COMPLETE_REQ / _ACK `m_cChangeUnitClass` 4033 / 4091, KConnectionUnitInfo 4216, KSquareUserInfo 4329, KPartyUserInfo 4584 and 4683, KPartyListUserInfo 4741, KFieldUserInfo 6137,
KCommunityUserInfo 6298, KGuildMemberInfo 6947, KGuildMemberMessageInfo 6979, KApplyJoinGuildInfo 7420, KRecommendUserInfo 7463, KMatchUserInfo 8016, KPvpMatchUserResultLog 8076,
EVENT_DATA `m_cUnitType` 8784 + `m_cUnitClass` 8785, KAutoPartyUserInfo 9007, KLocalRankingUnitInfo 9441.
`ClientPacket.h`: EGS_DELETE_UNIT_ACK 136, EGS_UPDATE_PARTY_USER_INFO_NOT 2857, EGS_ADMIN_CHANGE_UNIT_CLASS_REQ / _ACK 4290 / 4296, EGS_CHANGE_USER_INFO_NOT 6217,
EGS_SEARCH_TRADE_BOARD_REQ `m_cUnitType` / `m_cUnitClass` 6624 / 6625, EGS_COUPLE_PROPOSE_AGREE_NOT `m_cAcceptUnitClass` 9774, EGS_RELATIONSHIP_INFO_ACK 9837 / 9844,
EGS_CHANGE_MY_UNIT_INFO_NOT 10632; `m_cAttUnitType` in EGS_NPC_UNIT_DIE_REQ / _NOT 1506 / 1566. (`OnlyGlobal/Packet/*_Global.h` copies are other regions: ignore.)
Client readers cast plain: `X2Unit.cpp:4342,:4586,:4753,:4845` `m_UnitClass = (UNIT_CLASS)data.m_cUnitClass`; compares at `X2Unit.cpp:175,:189`. Offline writers cast `(char)`:
`Handlers_Unit.cpp:251,:263,:411,:499`, `Handlers_Room.cpp:186`, `X2OfflineServer.cpp:466` (parameter `char cUnitClass`). `KUnitInfo` carries the character list, so class 200 needs the member widened
(client-side behind an `Always.h` flag; `CommonPacket.h` also needs the `ServerDefine.h` twin and `flagcheck.py SECOND_HOMES`, per CLAUDE.md) or a parallel int. Not decided here.
DB side is fine: `unit.unit_class INTEGER NOT NULL` (`X2OfflineDB.cpp:65`), `CreateUnit` binds an int (:2086-2111).

## 3. GU class factory, unit viewer, UnitTemplet loading

Factory switch over `CX2Unit::UT_*`: `X2Game.cpp:21276` `CX2Game::CreateGUUser` (ELSWORD 21290, ARME 21296, LIRE 21302, RAVEN 21308, EVE 21314, CHUNG 21322 (`NEW_CHARACTER_CHUNG`), ARA 21331
(`ARA_CHARACTER_BASE`), ELESIS 21339 (`NEW_CHARACTER_EL`), ADD 21346 (`SERV_9TH_NEW_CHARACTER`), `default:` returns NULL). Second copy `X2Game.cpp:2141-2199` in `AddUserUnitIntruder`
(`CX2GUAdd::CreateGUAdd` :2201). Each class has a static `CreateGU<Class>( slotIndex, team, script_info, frameBufferNum, pUnit )` (ex `X2GUAdd.cpp:109`). Callers of `CreateGUUser`: `X2Game.cpp:2075`, `X2BattleFieldGame.cpp:1604`.

Every other `case CX2Unit::UT_ADD:` site (60 in X2Lib/*.cpp, found by scan; each needs a Lu/Ciel decision unless already a stand-in):
X2ChatBox.cpp:6924 PlayEmotionSound; X2Data.cpp:3906 SetUnitReForm; X2EnchantItem.cpp:2337 GetBaseExtraDamageType; X2EventScene.cpp:1723 GetPlayerTagName;
X2GUUser.cpp: 2550 AppendDeviceToDeviceList, 4225 AppendComponentToDeviceList, 4651 AppendEffectToDeviceList, 5003 AppendMotionToDeviceList, 30828 ProcessEnchantedWeaponEffectAtHand,
37997 SetUnitReForm, 45219 SetScriptFileName (`REALTIME_SCRIPT_PATCH`), 45271 AppendUpgradeWeaponParticleToDeviceList, 49623 PlaySoundRevive;
X2Game.cpp: 2199, 21346; X2GameUnit.cpp: 11909, 19367, 19465, 19559, 19774 (Weapon:: enchant and upgrade particles); X2MemoryHolder.cpp:531 GetCommonDLG; X2SquareUnit.cpp:220;
X2StateClassChange.cpp:995 PlaySoundChangeClass; X2StateCreateUnit.cpp: 207, 853, 3166 ChangeCreateUnitTypeChangeInfoDlg, 3321 ChangeCreateUnitStandMeshInstance, 3582 GetUnitClassCreateInfoTextureNameByUnitType, 4033 PlaySoundCreateUnit;
X2StateDungeonGame.cpp: 887, 2547, 5574, 6116; X2StatePVPGame.cpp: 440, 2147; X2StatePVPResult.cpp:2326; X2StateServerSelect.cpp: 2969 CreateUnitButton, 7568, 10868 ChangeSelectUnitStandMeshInstance,
11222 SetBackgroundImageBySelectUnitType, 11412 ChangeUnitButtonInfo, 12197 PlaySoundSelect; X2UICharInfo.cpp: 1871, 2332, 2361; X2UIManufacture.cpp:1580; X2UIPersonalShopBoard.cpp:2201;
X2UIQuestNew.cpp:9096; X2UISkillTreeNew.cpp:5564 (CreateSkillTreePortrait); X2Unit.cpp: 3641 GetAvatarEmotion, 4053 GetAvatarEmotionID;
X2UnitViewerUI.cpp: 916 and 960 (OnFrameMove enchant weapon), 6255, 6348, 7938 SetUnitReForm, 8559, 8681, 8795, 9027.
Unit viewer / character-select model: built from the unit templet plus the per-type switches above (`X2UnitViewerUI.cpp`; `X2StateServerSelect.cpp:10868 / 11222 / 11412`); there is no separate per-type model table.

Hard limits on unit type (all one short for type 10, since `UT_END` = 10):
- `X2UnitManager.h:3442` `boost::array<UnitTypeLuaTemplet, UT_END> m_aUnitTypeLuaTemplet`; `:3444` `boost::array<const UnitTypeTemplet*, UT_END> m_aUnitTypeTemplet`.
- `X2UnitManager.cpp:405-427` `CreateUnitLuaSpace`: `apszScript[]` = NULL + 8 script names + `ADD_NASODRULER.LUA` (`SERV_9TH_NEW_CHARACTER`); `ASSERT( ARRAY_SIZE( apszScript ) == UT_END )` :425; loop `iUnitType < UT_END` :427;
  getters :667 and :679 (`unitType < UT_END`, else NULL / `s_default`). The per-type class script (`InitInit`) loads here, so Lu/Ciel needs the array and the assert widened.
- `X2Item.h:208-209` `m_RanderScale[UT_END]`, `m_RanderRotate[UT_END]`; :347 loop `UT_NONE..UT_END`; `X2Item_Preprocessing.inl:991-992,:1035` `BOOST_STATIC_ASSERT( UT_END <= 256 )`; `X2ItemStatCalculator.cpp:195`;
  `X2ItemManager.cpp:547` and `X2ItemManager_Preprocessing.cpp:683` (item templet unit type >= UT_END is skipped; census line `X2ItemManager.cpp:594-595`); `X2OfflineCashShop.cpp:139-233` (costumes >= UT_END offered unchecked).
- `UT_END - 1` loops (9 iterations; character select / create statics `StaticUnit_Create_Unit%d`): `X2BeginningGame.cpp` 1233, 1287, 1338, 1395, 1447, 1502; `X2StateBeginning.cpp:1967`;
  `X2StateCreateUnit.cpp` 1376, 1510, 1642, 1771, 1900, 2035, 2179, 2313, 2457; `X2StateDungeonRoom.cpp:2908`; `X2StatePVPResult.cpp:2258`; `X2StatePVPRoom.cpp:3814`; `X2StateServerSelect.cpp:3393`;
  `X2BuffDisplayerTemplet.cpp:1257` `iMaxUnitNum = UT_END - 1` (the twelve-key table at :1245 is longer than this bound).
- Range guards `eUnitType >= UT_END -> return`: `X2GUUser.cpp:49576`, `X2StateClassChange.cpp:943`, `X2StateCreateUnit.cpp:3985`.
- `UC_END` (124) guards: `X2ItemManager.cpp:549`, `X2ItemManager_Preprocessing.cpp:688`, `X2SocketItem.cpp:2897` (class >= UC_END rejected: June classes 127+ and 200-202 all fall out), `X2GameEdit.cpp:1643`;
  `BOOST_STATIC_ASSERT( UC_END <= 256 )` `X2Item_Preprocessing.inl:1036`; `X2ItemStatCalculator.cpp:203` loops `UC_ELSWORD_LORD_KNIGHT..UC_VALIDE_END`.
- UnitTemplet storage has no limit: `X2UnitManager.cpp:690-700` `GetUnitTemplet( unitClass )` is a `std::map` find; :843 reads `unitType` through `LUA_GET_VALUE_RETURN_ENUM`; :269 uses it.
  March `UnitTemplet.lua` (kom_contents/data036_kom, BOM source, core list `tools/data/lua_core_list.tsv:30`, "edit: pilot-char-revamp CH5 per-character rows") has no Lu/Ciel rows (0 hits).
  June rows: `june_src_fixed/data036_kom/UnitTemplet.src.lua` Luciel entries at :1758-1759 (Duo 200), :1800 (Chiliarch 37), :1842 (Royal Guard 38), :1884 (Diablura 39), :1928 (Dread Lord 127), :1972 (Noblesse 128), Demonio 129 after.
- `X2OfflineLuaEnumSeed.h`: UNIT_CLASS seed stops at 124; June classes are absent there.

## 4. Character creation (client UI and offline emulator)

Client state `X2Lib/X2StateCreateUnit.cpp` (4050 lines; `REFORM_ENTRY_POINT` live, Always.h:472): dialogs `DLG_Unit_Create_Back_New.lua` :94, `DLG_Unit_Create_Front_New.lua` :97, server-select popup :871
`DLG_Unit_Create_Front_Server_Select_New.lua`. One `Select<Char>()` per type: SelectElSword 1266, Arme 1407, Lire 1541, Raven 1673, Eve 1802, Chung 1936, Ara 2074, Elesis 2213, Add 2346 (each sets `m_SelectUnitType`,
calls `ChangeCreateUnitTypeChangeInfoDlg`, `ChangeCreateUnitStandMeshInstance`, `ChangeCreateUnitClassChangeInfoDlg( type, UCT_BASIC_CLASS, ... )`, and assigns the base class to `m_SelectUnitClass`).
Entry switch :195-215 maps type -> `Select<Char>()` plus a radio button found by control name (`AddCreateUnitSlot` :207-210, `Elesis_Slot`, `Ara_Slot`, ...). Class-change button dialog by type :3133-3167
(`DLG_Unit_Create_ClassChange_Button_<Char>.lua`; `default: ASSERT(!L"Wrong Unit Type")`).
What decides which characters are offered: (a) the Lua dialog `DLG_Unit_Create_Unit_Button_New.lua` (one `CreateRadioButton` per character, included by `DLG_Unit_Create_Front_New.lua`). March's live copy
(`24965799/kom_contents/data034_kom`, `1B 4B 4C 81` bytecode) has 0 "Luciel" strings. June's source `june_src_fixed/data034_kom/DLG_Unit_Create_Unit_Button_New.src.lua:10,88-97` has `Pos_Luciel = 8` and
`LucielCreateUnitSlot` (textures `BT_LUCIEL_SELECT_N/O`); June also has `DLG_Unit_Create_ClassChange_Button_{Luciel,Rose,Ain}.src.lua`, which March lacks. (b) the C++ `Select*` and per-type switches.
(c) `SERV_LIMIT_UNIT` + `_CONST_LIMIT_UNIT_::iLimitType` (`X2StateCreateUnit.cpp:106-175`, `X2UIPersonalShopBoard.cpp:635-686`): no `#define SERV_LIMIT_UNIT` found in Always.h / ServerDefine.h / X2Define.h and no
`_CONST_LIMIT_UNIT_` declaration in any header, so that block is dead. No other per-type button list in C++. Create packets `KEGS_CREATE_NEW_UNIT_REQ` / `KEGS_CREATE_UNIT_REQ` carry only nickname + `m_iClass` (int).

Emulator handler: `X2Lib/Offline/Handlers_Unit.cpp:311` `Handler_EGS_CREATE_UNIT_REQ`, :333 `Handler_EGS_CREATE_NEW_UNIT_REQ`, both -> `BuildCreateUnitAck` :222-308. No class validation (no range or enum check).
Order: nickname check -> `IsNickNameTaken` -> `CountLiveUnits >= m_iUnitSlots` (ERR_CREATE_UNIT_05) -> `CX2OfflineDB::CreateUnit` (`X2OfflineDB.cpp:2086`; level 1, exp 0, ed 0, sp 1) -> `SeedAutoLearn` :278
(`SERV_IRUHADEV_JUNE_SKILL_REFORM_AUTOLEARN`) -> `SeedPromotionItems` :292 -> `SeedBeginnerGear` :298 -> `MakeUnitInfoFromRow` (casts class to `char`).
Starter item table: `X2Lib/Offline/X2OfflineInventory.cpp:1187-1236` (file-local anonymous namespace; old code, not rewritten): `struct KBaseItemSet { int m_iUnitClass; int m_aiGear[5]; int m_aiCostume[5]; }` :1219 and
`const KBaseItemSet GBASE_ITEM[]` :1226 with rows 1-9, e.g. `{ 1, { 131641, 111094, 111095, 111096, 111097 }, { 128000, 128001, 128002, 128003, 128004 } },	///< Elsword` :1228 ... `{ 9, { 112991, ... }, { 41757, ... } }` :1236.
Order weapon, body, leg, hand, foot; gear = PT_ENDURANCE, endurance 10000 (`GEAR_ENDURANCE` :1262), slots NESI 9/1/3/5/7; costume = PT_INFINITY, endurance -1, slots 10/2/4/6/8. `FindBaseItemSet( iUnitClass )` :1269 is an
exact match on the unit's class, so class 200 finds nothing: `SeedBeginnerGear` :1318 and `SeedPromotionItems` :1394 log "dbo.GBase_Item has no row for class %d" and return false (no gear, no costume).
A class 1-5 fallback (`class*1000000 + 2000 + offset`) exists only for the gear half. Declarations `X2OfflineInventory.h:208,:242`.
Other class-keyed offline tables: `X2OfflineSkillAutoLearn.h:35-43` `ROWS` (class, skill, require level, type) already covers classes 1-46, 100-136, 200-202 (Lu/Ciel free skills seed; `tools/data/june_skill_autolearn.tsv` same classes);
`X2OfflineClassChangeSeed.h:249-306` `PARENT_ROWS` (ends at class 119; no June classes, no class 8/9/29/30/32 row) and `SIDE_STEP_CLASSES`.
`X2OfflineStatTable.cpp`: `UnitTypeOf` :271-286 (templet type, else identity if class < `UT_END`), `CharAbilTypeOf` :289-313 (by unit type; default `CAC_NONE`; only Elsword, Chung, Ara special),
`GetUnitStat` :327-: looks up `m_mapStat[class]`; if absent, retries with `UnitTypeOf(class)`; if still absent logs `STAT     WARNING no rows for class=%d - using the synthetic curve`.
Data file `StatTable.lua` (:27; server Lua, XOR + pack into data036; `KncWX2Server/ServerResource/US/StatTable.lua` has `SetUnitStat` rows for classes 1-8, 10-31, 100-119 only, 99 levels each except 110/111 with 118).
Consequence for Lu/Ciel (class 200 with a templet of type 10): no class-200 rows -> `UnitTypeOf(200)` = 10 -> finds class 10 = Elsword Knight's rows: wrong stats, no warning. With no templet row: type 0 -> synthetic curve with the warning.
Every June class without rows in this `StatTable.lua` (123+, 200+, plus Add 9 / 32) takes the synthetic or wrong-type path today.

## 5. Tools that enumerate classes

- `tools/createchar.py:31-55` `CLASSES = {1: 'ELSWORD_SWORDMAN', ..., 9: 'ADD_NASOD_RULER', 10: ..., 31: 'ARA_LITTLE_DEVIL', 32: 'ADD_PSYCHIC_TRACER', 100: ... 122: 'ADD_LUNATIC_PSYKER'}` (March only;
  `resolve_class` :57-70 dies on any other id, so `200` / `uc_luciel_duo` is rejected). `PARENT` :131-141 (job -> base; ends `32: 9, 122: 32`); `PROMO` :151-160 (base classes 1-9 five-piece costumes);
  `COSTUME_SLOTS = (10, 2, 4, 6, 8)` :163; `GEAR_POSITIONS` :165. `tools/elsdb.py:386` calls `createchar.resolve_class`, :358 shells `createchar.py`; elsdb has no table of its own.
  Skill seeding reads `tools/data/june_skill_autolearn.tsv` (already has 200-202).
- `june2017_re/charport_adis.py:23` `CLASS_RANGE = {'eve': (0x32965dd0, 0x329cf86f), 'chung': (0x32bd86f0, 0x32c25a1f)}` (marks a call as `(class fn)` when inside the class's address range; `.get(key, (0, 0))` :80,
  so a `luciel` key has no range until added; Lu/Ciel vtable is `334f06fc`, class function addresses seen in funcmap run `32b0e670..32b4dbe0`).
- `june2017_re/charport_closure.py:63` `AISHA = dict(script='Arme_VioletMage', lo=0x32cb6e00, hi=0x32cf2da0)` is only the default argument; it takes a script name and `--range`, so no edit needed.
- `charport_keys.py` (arg = class script name, resolved in `june_src_fixed/data036_kom/<name>.src.lua`), `charport_scan.py` (one scratch-dir arg), `charport_states.py` (`init` / `skill`), `charport_consts.py`: no class table.
  `charport_regsim.py:8` "CLASSES below are Aisha's" (comment; `CLASSEXPR` :136 is a regex list). `census.py` has no class list (Luciel appears only as SKILL_ID rows :2392-2393, :2827-2832).
- Existing per-character scratch dirs `june2017_re/char_arme`, `char_chung`, `char_eve`; `char_luciel` now exists (`ghidra/`, `plan_starter_items.tsv`) from another session; this file sits beside them.
- `tools/skillicons.py check <A-B ...>` takes id ranges (no class table). Lu/Ciel skill ids in June Enum: `RNSI_SA_LUCIEL_*` 30001-30029, `RNSI_A_LUCIEL_*` 30201-30302.

## 6. Plan-doc digests

### 6a. `JUNE2017_CHARACTER_PORT_CHECKLIST.md` (78 matching lines for `-i "lu/ciel|luciel|ciel"`, not 76)
Format: line | section | gist.
3 | intro | read before porting any June character; Lu/Ciel named among new characters
54 | 1 Blocker | numerical-override census: `Luciel_Duo` 15 tables / 67 rows need June's numerical table
58 | 1 Blocker | same census: `Luciel_Demonio` 2 tables / 11 rows
119 | 2 DamageData keys | `HIT_ADD_COMBINATION_POINT` unsupported (parsed, logged); `Luciel_Duo` uses it 6 times
126 | 2 DamageData keys | `DAMAGE_RATE_TO_SUPERAMOR` implemented in effect C3; `Luciel_Duo` uses it 2 times
151 | 3 Systems | Combination point absent; reader `CX2GULuciel` 32b14bc0 (DD +0x594 > 0, then 32b14860); new-character system
153 | 3 Systems | Lu/Ciel duo absent (no `DUO_TYPE` enum) plus CharCustomData, EmotionData, EventScene*, UnitViewerEffectSetTable
166 | 4 Enum tables | `CUSTOM_RENDER_STATE` (17473) table missing in March; row lists Luciel-related entries
167 | 4 Enum tables | `DUO_TYPE` (17515) DT_MAJOR 0 / MINOR 1 / BOTH 2 / KEEP 3; `Luciel_Duo` 390 uses, `Luciel_Demonio` 158
225 | 7 Buff hooks | `UT_LUCIEL` and `ST_SPECIAL_ACTIVE_GRADE_1-3` enum members needed by class-script buff ids
238 | 7 Buff hooks | 32b1e8e0 (vslot 16): Luciel writes 0 at init
240 | 7 Buff hooks | 322a7660 / 322a8a70 set-stack-count callers include Luciel
244 | 7 Buff hooks | bone-rotation second pair +0x8e84 / +0x8e88 exists only for unit type 10
251 | 7 Buff hooks | skill-use function Luciel 32b1eef0 (vtable +0x800): sets the cooldown (Add, Luciel, Ain, Rose)
253 | 7 Buff hooks | `IsSummonSkill` (54 ids) and summon cases of 3230c1a0 that March lacks (NPC id list)
255 | 7 Buff hooks | unit vslot 201 `ChangeMaxCharacterAddResource`: Luciel override 32b14cd0 (BBT 97)
256 | 7 Buff hooks | death-prevention 320072d0 / 3202b9a0 from UpNowHp / SetNowHp overrides; all users
257 | 7 Buff hooks | slot 251 3204dcd0 + Ain 32a3d220: Luciel Soul Collector (BTI 462, RNSI_P_LCN_TRIBUTE 30409)
260 | 7 Buff hooks | every class's vslot 510 skill lookup; Luciel 32b3a310
263 | 7 Buff hooks | vslot 510 copies; Luciel @32b3a61f after CheckSkillUsePVPOfficial
274 | 7 Buff hooks | last-hit serial +0x40dc read at 31fd0311 for Luciel (unit type 10)
275 | 7 Buff hooks | `SocketData_DuoUnit` (vt 33478ab0, 0x488) built by user ctor for type 10; read in 32b0fa90
282 | 7 Buff hooks | `CX2GULuciel` 32b0e670 (vtable 334f06fc) overrides vslot 485 called by SumSocketOption
308 | 7a Effect hooks | Luciel vslot 0x248 / 0x2e0 overrides 32b0efc0 / 32b0efe0; counter +0x988c; DD +0x638 readers
368 | 7e Viewer effect rows | duo minor model +0xd7c skin anim, flags +0xd80-0xd82, DT_MINOR row LDN_LobbyWait -> EffectSet_LDN_Start_Ciel
379 | 7d Damage math | DamageReact 320483e4 / 327b33cf: type-10 attacker with USED_SKILL_ID 30205 (RNSI_A_LLC_BACKSTAB) forces ignore
407 | 7g Item fields | record +48 `m_ItemIDForMinorUnit`: 429 Lu items map to Ciel dummy (30344 -> 200030344)
408 | 7g Item fields | flags2 bit 2 `m_bIsForMinorUnit` on 2,582 Ciel dummies; K2 added accessor only
409 | 7g Item fields | Extra accessors 32867660..32867960: type 10 -> key 10 (Lu) or 11 (Ciel) by bool; 11 -> 12, 12 -> 13
421 | 7h Unit-state readers | EFFECT_SET_LIST_EX_TRANS_1/2 variants picked by skill-memo entry 31fdbbb0
423 | 7h Unit-state readers | SOUND_PLAY_EX / STATE_SOUND_PLAY / CUSTOM_STATE_DELETE_SOUND in Add, Elesis, Luciel, Arme, Aisha scripts
442 | 7i Re-equip snapshot | Luciel 32b46580 is June-only among 12 classes' skill-slot re-equip calls
454 | 7j DamageReact steps | inline step 320462a0 + 32aebaf0: type-10 unit absorbs damage x 0.2 (shadow shield)
469 | 7l Super-armour | `SUPER_ARMOR_FRONT` states: Rose_Ranger 4, Luciel_Duo 4; works once parsed
470 | 7l Bounding | USI_BOUNDING_DOWN / LANDCRASH / UP registered in every class's InitState (12)
471 | 7l Bounding | USI_BOUNDING_UP StateStart 31fd02d0: last attacker type 0xa -> 32b38cb0 (class code)
474 | 7l Bounding | Luciel virtual 32b34680 (slot 0x568): target check via vslots 0x2f4 / 0x2f0
496 | 7n User-sync float | Rose Neil the Sniper float; Ain and Lu/Ciel drive it too
507 | 7o Fall-speed | `<prefix>DAMAGE_BOUNDING_DOWN` -4000 in all 12 classes (13 tables: Lu and Ciel one each)
511 | 7o Minor unit | second PhysicParam +0x36c, MINOR_UNIT_PASSIVE_SPEED_X/Y, APPLY_MOVEMENT_SPEED
513 | 7o Minor unit | comes with Lu/Ciel (Stage 8); June builds the limit then ignores it (JUNE_BUGS 142), keep verbatim
519 | 7o Minor unit | walk-speed rescale uses main unit; comes with Lu/Ciel (funcmap C3h rows)
530 | 7o Minor unit | Stage 8: MINOR_UNIT_SPEED_TIME(_ULTIMATE) twins (+0x8604, 262 June rows), list swap on second body
551 | 7q Down getters | heading: Luciel's down-state getters (effect C3f)
555 | 7q Down getters | Luciel overrides all four; make virtual (or test the class) when ported
557 | 7q Down getters | table: June vslot / base (12 classes) / Luciel / Luciel returns
583 | 7s Dungeon registry | `BOSS_DROP_ITEM.LUCIEL`, `ENDING_SPEECH<n>.LUCIEL` (101 tables); speeches are `{ LU, CIEL }` pairs, June 327e0f80
586 | 7t Hyper cut-in | heading: class hooks that pick berserk / Lu/Ciel names (C3t)
602 | 7t Hyper cut-in | Luciel 32b4dbe0 overrides `JuneIsHyperModeState` (state 0x17 or a Luciel state)
603 | 7t Hyper cut-in | in 32014fc0: second body + `m_MinorUnitHyperCutIn`; type 10 / class 0x81 (DEMONIO) branch
611 | 7t Hyper cut-in | sound sets `SOUND_SET_PICK`: three overrides incl. Ara / Luciel
649 | 7v Minor slot key | heading: Lu/Ciel minor-unit slot key (engine U0)
652 | 7v Minor slot key | read in `X2SlotItem.cpp:53-65` LuaGetValue; used by character-window and closet dialogs
653 | 7v Minor slot key | only the Lu/Ciel port needs it (DLG_Magical_Closet, UI_CHARACTER_NEW_2/3/4)
662 | 7w CUSTOM_RENDER_STATE | `Luciel_Demonio` among CRS_INVISIBLE users (with Ain, Rose, RidingPet_Rakshasa)
680 | 7y GetLandPosition | Rose and Luciel pass bMonster true; Lu/Ciel minor-body block with the class
683 | 7z Skin equipment | heading: Lu/Ciel skin equipment attaches every model (R14)
686 | 7z Skin equipment | unit type 10, eqip ctor flag +0x15d4 clear: the Lu/Ciel port adds the flag and the term
688 | 7z Skin equipment | related: 7v slot key, 7o minor physics
690 | 7aa Attach-anim | heading: Lu/Ciel minor-unit attach-anim models (R14b), pair +0x584 / +0x5e4
700 | 7aa Attach-anim | UpdateEquipCollisionData part is not Lu/Ciel-only; belongs to engine plan
774 | 7ac CanDown | `ProcessLockOn` is shared with Luciel
777 | 7ac CanDown | landing prediction 32021390 has Add / Luciel / Arme callers
778 | 7ac CanDown | Luciel states @32b2fd79, @32b33fb7 use June's call shape
827 | 7ah 10-slot dialog | class statics `Static_LucielSwitchingGuide` / `g_pStatic_LucielCooltime`
862 | 7aj Tree-UI | slot data type 6 (328d9b00 / 328da050): Luciel transform slot, caller 3200f380
864 | 7aj Tree-UI | type 6 and slot 29078 are Luciel's: Stage 8
865 | 7aj Tree-UI | Luciel portrait 328dcba0 / 328db7a0: form flag tree +0x8c from unit +0xe40
870 | 7aj Tree-UI | command group: `JuneSetCommandGroup` 328e8cd0 ported; only caller is unit form change 3234d930
871 | 7aj Tree-UI | Luciel `_MINOR` command lists 328e7ba0 and COMMAND_DATA_MINOR 3220bdb0 need the class port
872 | 7aj Tree-UI | June's minor flag = unit +0x3c; needs March's Luciel unit type and that flag
907 | 7am Delegate lists | Lu/Ciel 32b43d10 (slot 0x7a4 of 334f06fc): ten A / C delegate entries
927 | 7an Passive hooks | `GetCharacterSpecializationB` readers: Lu/Ciel 32b14950 / 32b3794d
964 | 7ao Transcendence | Lu/Ciel 32b1f0a0 (hyper effect start, vslot 19), 32b397a0 (CommonFrameMove, vslot 499)
977 | 7ap CH1o2 slots | Lu/Ciel 0x56c 32b16220, 0x6a8 32aea8c0
994 | 7as Glow | heading: upgrade-weapon glow for Lu/Ciel, Rose, Ain (D1-3b)
1007 | 7as Glow | viewer 31f961a0 cases 10-12; Lu/Ciel frame move 31f93050 (+0xd80 trace)
1036 | 7av State anim rate | Lu/Ciel future anim = `+0x8e90 ? +0x8e84 : m_pXSkinAnimFuture` (dual body)

### 6b. `JUNE_BUFF_PORT_PHASES.md` (27 matching lines)
Policy: :29 and :2133 "Port June's full functions now, including the Rose, Ain and Lu/Ciel cases. They stay dormant" (user decision); :2134 only Rose, Ain and Lu/Ciel are absent from March;
:3354 Phase 9 includes the Rose / Ain / Lu/Ciel `CreateAndInsertCustomFunctionByBuff` cases. Rows by file / flag:
- :207 displayer per-class keys `ADD/AIN/LUCIEL/ROSE` (2 each, `BDT_EFFECT_SET_ONLY_MY_CLASS`) -> `X2BuffDisplayerTemplet.cpp:1245`, `JUNE_BUFF_BEHAVIOR_PARITY`.
- :294 class files Luciel (Demonio, Duo); :621, :2741, :2769 BBT 97 `CHANGE_MAX_CHARACTER_ADD_RESOURCE` -> unit vslot 201 (empty base `3208ddc0`; Luciel override 32b14cd0 left to the class; `X2GameUnit.cpp:19199`).
- :2490, :2492 Phase 6b: cool-time helper 3235a710; Luciel skill-use virtual 32b1eef0 (vtable +0x800) mapped, not ported.
- :2882, :2995, :3055, :3160 Phases 7c / 7d / 7e: vslot 510 prefix and slot 545 (`32b3acf0`) for Add / Ain / Rose / Luciel go with their ports.
- :3452 Phase 9b: Luciel cases of `3229e1a0` have their classes; :4028 Phase 11b slot 251 `3204dcd0` Luciel Soul Collector (checklist 7 row).
- :4259, :4293 Phase 13: `UT_LUCIEL = 10` added to Enum.lua (3 displayer entries; `BDT_RENDER_PARAM_BY_UNIT_TYPE` store is inert) and `ST_SPECIAL_ACTIVE_GRADE_1/2/3 = 14/15/16`.
- :5034, :5188, :5243, :5250 Phase 16: `SocketData_DuoUnit` allocated by user ctor `3203e850` (@32042060) when unit type 10; vslots 3/4 `32091b10` / `32091b60`; `32b0fa90`; "DuoUnit goes to section 7, March has no Lu/Ciel"
  (flag `JUNE_SKILL_UNIT_SOCKET`, `X2Unit.cpp:1182`).
- :5227 `CalculateLinear<Class>Specialization{A,B}Value` for 12 classes incl. Luciel (-> `X2UICharInfo.cpp:1872` curve names).
- :6008 OptionItemData: 1,829 rows name classes March lacks (Rose, Luciel, Ain, Add / Ara / Elesis jobs); :6074 Luciel `32b0e670` ids 30044-30046 / 30050-30052; :6077 hook `33093ad0` is a bare ret, Lu/Ciel and Ara overrides to checklist 7.

### 6c. `june2017_re/funcmap.tsv` (columns: june_va, march_symbol, status, evidence; there is no plan/phase column, so rows are grouped by the evidence prefix)
108 rows match `lu/ciel|luciel|ciel|\blu\b` (107 without the bare "Lu"). By evidence prefix: Effect C* 39, Char (CH*) 20, Buff Phase 15, june-march-divergence (Diverge / Phase S) 11, CharInfo 8, Skill S* 7, Transcend 3, Engine 2, Quest 2, KIM 1.
Status: confirmed 69, ported / PORTED 25, candidate 4, `differs: Stage 8` 3, not ported / NOT PORTED 2, partial / noted / same 5.
Rows that look like Lu/Ciel class functions (June VA, note, status):
32b1e8e0 Luciel-region init (DLG_LUCIEL_BOUNDING_KEY) candidate; 32b14bc0 `CX2GULuciel` add HIT_ADD_COMBINATION_POINT on hit candidate; 32b0efc0 / 32b0efe0 `CX2GULuciel` vslots 0x248 / 0x2e0 confirmed;
32b46580 `CX2GULuciel::SetEquippedSkillFuncBySkillSlotIndex` (vslot 0x7b4) confirmed; 32aebaf0 shadow-shield absorb confirmed (BTI 478 SHADOW_LU / 479 SHADOW_CIEL);
32b4d810 / 32b4d850 / 32b4d990 / 32b4d9d0 down-state getters confirmed; 32b34680 bounding-target check (vslot 0x568) confirmed; 32b3713d Bluff IsSuperArmor(0) caller confirmed;
32b0fa90 type-10 `ResetMaxMP` override noted; 32091730 `SocketData_DuoUnit` ctor confirmed; 32070cf0 future-anim accessor (second body) confirmed; 32071db0 / 31fe9e40 dual-body active skin anim confirmed;
31fdc9d0 RestoreHead on another anim (not ported); 31fac140 `CX2Unit` is-type-10 confirmed; 31fd48a0 vslot 0x570 skin anim of a form confirmed; 31e4c550 `IsClassCommonMoveState` base, Luciel 32b15340 override (states 424-433);
31fd2f00 / 31ffe990 / 3201b930 equip view-list rewrite `differs: Stage 8`; 3291d360 CharInfo viewer reset (only Lu/Ciel stubs call it) not ported; 3234b8c0 sets the Lu/Ciel form byte +0xe40 confirmed;
32015b30 `PhysicProcess` minor-unit blocks (@3201745a, @32016996, @32019f9b) NOT PORTED; 31fdd490 `GetHyperModeTableName(bCiel)` PORTED; 3200d3f0 / 3200d900 Neil the Sniper (Rose, shared).
Phases named in the evidence of those rows: Buff Phase 4b / 7 / 9 / 11b / 16, Effect C2 / C3b-C3l / C6a / C6a2 / C9h-6 / C10d, Char CH1d-CH1l, CharInfo C0-C4, Skill S7e / S7j.

## 7. `KTDXLIB/Always.h` existing defines (grep -i `luciel|lu/ciel|lu_ciel|NEW_CHARACTER|minor.unit|ciel`)

No Lu/Ciel-named define, no minor-unit define. Hits:
- :1480 `//#define EVENT_NEW_CHARACTER_EL` (commented), :1521 `#define NEW_CHARACTER_EL`, :3306 `#define NEW_CHARACTER_CHUNG`. `SERV_9TH_NEW_CHARACTER` is in `KncWX2Server/Common/ServerDefine.h:4323`, not Always.h.
- :6309, :6459, :6481, :8190, :11146: comments naming the Lu/Ciel minor unit / second body as not ported, routed to checklist 7o / 7aa / Stage 8.
- :10602 `#error SERV_IRUHADEV_JUNE_DIVERGE_DIE_FRAME_MOVE needs SERV_IRUHADEV_JUNE_ENUM_NAMES ( NUI_CSM_LU_DIABLA / NUI_CSM_CIEL_DEMONIO )`.
- :10825 `#error SERV_IRUHADEV_JUNE_DIVERGE_UNIT_REFORM needs SERV_IRUHADEV_JUNE_ENUM_NAMES ( UT_LUCIEL / UT_ROSE / UT_AIN ), MOVE_BONE ( SetFrameMoveBoneY ) and ELSWORD_SECOND_CLASS_CHANGE ( SetUnitReForm )`.
- Flags a Lu/Ciel plan leans on (all live): `SERV_IRUHADEV_JUNE_ENUM_NAMES` :10164, `SERV_IRUHADEV_JUNE_DIVERGE_UNIT_REFORM` :10823 (description :10820), `MOVE_BONE` :2827, `ELSWORD_SECOND_CLASS_CHANGE` :2833,
  `ARA_CHARACTER_BASE` :2132, `NEW_CHARACTER_EL` :1521, `NEW_CHARACTER_CHUNG` :3306.
- Per-character flags exist for Eve (`SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` :10111), Arme (`..._CHAR_ARME_CORE` :9614) and Chung (`..._CHAR_CHUNG_LIVE` / `_CBALL` per the git log); none for Lu/Ciel.
