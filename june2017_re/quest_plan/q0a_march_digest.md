# March quest system digest (Q0a) - X2Lib/X2QuestManager.{h,cpp}, X2Lib/Offline quest

Paths relative to TRUNK. `h` = X2Lib/X2QuestManager.h, `cpp` = X2Lib/X2QuestManager.cpp, `off` = X2Lib/Offline/X2OfflineQuest.cpp.
Liveness = checked for a top-level (unconditional) `#define` and no `#undef`; ServerDefine_Global.h is included at ServerDefine.h:4620 under
SERV_GLOBAL_COMMON (defined ServerDefine.h:4608, unconditional); ServerDefine_US.h at :4632; Always_Global.h at Always.h:3393 under CLIENT_GLOBAL_COMMON (Always.h:3391).
Not resolved with `cl /EP`; resolved by reading each define's enclosing #if stack (all top level).

## 0. Flag liveness (US live build)
LIVE (defined at):
- REFORM_QUEST: KTDXLIB/Always.h:2195
- SERV_EPIC_QUEST: KncWX2Server/Common/ServerDefine.h:368
- SERV_REFORM_QUEST: ServerDefine.h:2821
- SERV_DAILY_QUEST: ServerDefine.h:236
- SERV_DAY_QUEST: ServerDefine.h:173
- SERV_SECRET_HELL: ServerDefine.h:229
- SERV_QUEST_CLEAR_EXPAND: ServerDefine.h:789
- SERV_NEW_PVP_QUEST: ServerDefine.h:1194
- SERV_ENTER_FIELD_QUEST_CLEAR: ServerDefine.h:2780
- SERV_ACCOUNT_MISSION_SYSTEM: ServerDefine.h:2318
- SERV_RANDOM_DAY_QUEST: ServerDefine.h:1291
- SERV_INTEGRATION: ServerDefine.h:259
- SERV_SUITABLE_LEVEL_DUNGEON_CLEAR_SUB_QUEST: ServerDefine.h:3451
- SERV_SUB_QUEST_USE_ITEM: OnlyGlobal/ServerDefine/ServerDefine_Global.h:1075
- SERV_SKILL_USE_SUBQUEST: ServerDefine_Global.h:963
- SERV_SUB_QUEST_LEARN_NEW_SKILL: ServerDefine_Global.h:1102
- SERV_PVP_NPC_HUNT_QUEST_EXTEND: ServerDefine_Global.h:244
- EXTEND_SERVER_GROUP_MASK: ServerDefine_US.h:100
- ADD_SERVER_GROUP: Always.h:2807
- PVP_SEASON2: Always.h:2237
- DAY_OF_THE_WEEK_QUEST: Always.h:1960
- QUEST_GUIDE: Always.h:2289
- SERV_EPIC_QUEST_HIDE_QUESTMARK_WHEN_SUBQUEST_COMPLETED: Always.h:3277
- REFORM_NOVICE_GUIDE: Always.h:2197
- FIX_QUEST_DESC_CRASH: Always.h:2278
- DIALOG_SHOW_TOGGLE: Always.h:3270
- INT_SHOW_QUEST_ITEM: KTDXLIB/OnlyGlobal/Always_Global.h:715
- X2OPTIMIZE_AVOID_LUA_RUNTIME_INTERPRETING: KTDX.h:390
- SERV_IRUHADEV_OFFLINE: Always.h:3482
- SERV_IRUHADEV_ELYSION_VILLAGE: Always.h:7449
- PRIVATE_BANK Always.h:2866, GUILD_MANAGEMENT :2848, NEW_VILLAGE_UI :2899, EVENT_SCENE :2660, GOOD_ELSWORD :621,
  SERV_GLOBAL_BILLING ServerDefine.h:4613, SERV_IRUHADEV_JUNE_DUNGEON_SUITABLE_LEVEL Always.h:6871.

DEAD / not defined for US:
- SERV_POINT_COUNT_SYSTEM: not found anywhere live (only `//` in ServerDefine_TH.h:107). So SQT_POINT_COUNT has no loader case (see 4).
- SERV_EVENT_TITLE_SUBQUEST: only ServerDefine_BR.h:561 / _PH.h:500 / _TH.h:116, not US.
- LUA_TRANS_DEVIDE: `//#define` in KTDXLIB/OnlyGlobal/Always_Global.h:181. QuestTrans.lua / SubQuestTrans.lua not loaded, Trans binders not registered.
- EXCEPTION_EPIC_QUEST_TEMPLET: `//#define` ServerDefine.h:370. So the "m_eQuestType >= 5 -> return true" skip (cpp:522-527) is OFF.
- MODIFY_ACCEPT_QUEST: `//#define` Always.h:758 (also InHouse5.h:485). GetBoardStartQuestList takes the #else branch (cpp:1605).
- EVENT_NPC_IN_VILLAGE: only `//` in Always_JP.h:151. So `GetStartQuestList(NUI_EVENT_BENDERS)` (cpp:1697) is live.
- SERV_RECRUIT_EVENT_QUEST_FOR_NEW_USER: `//` ServerDefine_US.h:328. TEMP_MEDAL_EVENT_QUEST: not found.
- SHOW_DAILY_REPEAT_MARK_ON_EVENT_QUEST_FOR_DEVELOPER_FIX: not found. PRINT_INGAMEINFO_TO_EXCEL: `//` InHouseEtc.h:6.
- FIRST_GAME_LOADING_THREAD_TEST: `//` InHouseEtc.h:229 (so X2StateLoading OnFrameMove branch is the live loader).

## 1. Structs / enums (h)
### enums
- QUEST_TYPE (h:31-50, no flag; the `//#ifdef SERV_EPIC_QUEST` is a comment so EPIC/CONTEST always exist):
  QT_NORMAL=0, QT_EVENT=1, QT_CHANGE_JOB=2, QT_SKILL=3, QT_PCBANG=4, QT_EPIC=5, QT_CONTEST=6, QT_CHARACTER=7, QT_END=8.
- QUEST_REPEAT_TYPE (h:55-60, SERV_DAILY_QUEST live): QRT_NORMAL=0, QRT_DAY=1, QRT_REPEAT=2.
- CHANGE_JOB_TYPE (h:65-77): CJT_JOB_NONE=0, CJT_JOB_FIRST_OLD=1, CJT_JOB_SECOND_OLD=2, CJT_JOB_FIRST=3, CJT_JOB_SECOND=4, CJT_JOB_THIRD=5, CJT_JOB_END=6.
- SUB_QUEST_TYPE (h:79-153): SQT_NONE=0, NPC_TALK=1, NPC_HUNT=2, ITEM_COLLECTION=3, QUEST_ITEM_COLLECTION=4, DUNGEON_TIME=5, DUNGEON_RANK=6,
  DUNGEON_DAMAGE=7, DUNGEON_CLEAR_COUNT=8, PVP_PLAY=9, PVP_WIN=10, PVP_KILL=11, WITH_DIF_SERV_USER=12, ITEM_ENCHANT=13, ITEM_SOCKET=14,
  ITEM_ATTRIB=15, ITEM_RESOLVE=16, ITEM_EQUIP_DUNGEON_CLEAR=17, USE_SKILL_POINT=18, FEED_PET=19, USER_DIE=20, PVP_NPC_HUNT=21, ITEM_USE=22,
  SKILL_USE=23, VISIT_VILLAGE=24, VISIT_FIELD=25, VISIT_DUNGEON=26, FIND_NPC=27, PVP_PLAY_ARRANGE=28, PVP_TAG_COUNT=29, HYPER_MODE_USE=30,
  CHAR_LEVEL_UP=31, SUITABLE_LEVEL_DUNGEON_CLEAR=32, PVP_HERO_NPC_KILL=33, PVP_HERO_NPC_PLAY=34, POINT_COUNT=35,
  LEARN_NEW_SKILL=36 (inside `#ifdef SERV_SUB_QUEST_LEARN_NEW_SKILL`, live, h:149-151), SQT_END=37. All other values are unguarded (flags are in comments).
- QUEST_MAPID_TYPE (h:156-161): QMT_NONE=0, QMT_VILLAGE=1, QMT_DUNGEON=2.
- VILLAGENPC_RELATIONDATA_TYPE (h:685, SERV_IRUHADEV_ELYSION_VILLAGE): VRT_NONE=-1, VRT_CHANGE=0, VRT_NOT_SHOW=1.

### QuestCondition (h:166-204), live branch = SERV_EPIC_QUEST + REFORM_QUEST
int m_iLevel (168); CX2Unit::UNIT_TYPE m_eUnitType (169); std::set<UNIT_CLASS> m_setUnitClass (170); int m_iConditionItemID (171);
int m_iConditionItemNum (172); int m_iNotBeforeQuestID (173); std::vector<int> m_vecBeforeQuestID (174);
[SERV_EPIC_QUEST h:176 + REFORM_QUEST h:177] std::set<int> m_setEnableVillage (178), std::set<int> m_setEnableDungeon (179),
std::set<int> m_setEnableBattleField (180), int m_iLimitLevel (181). [#else of REFORM_QUEST, dead: int m_iEnableVillage (184)].
Ctor defaults h:189-203: m_iLevel=-1, UT_NONE, ItemID=-1, ItemNum=-1, NotBefore=-1, m_iLimitLevel=-1.

### ClearCondition (h:206-368), live branch = REFORM_QUEST + all of SERV_SECRET_HELL/QUEST_CLEAR_EXPAND/NEW_PVP_QUEST/ENTER_FIELD/ACCOUNT_MISSION/SUB_QUEST_USE_ITEM/SKILL_USE/LEARN_NEW_SKILL
(order as declared):
std::set<NPC_UNIT_ID> m_setKillNPCID (209); int m_iKillNum (211); int m_iCollectionItemID (213); int m_iCollectionItemNum (214);
[REFORM_QUEST h:216] std::set<SEnum::DUNGEON_ID> m_setDungeonID (217). [dead #else h:219-220: SEnum::DUNGEON_ID m_eDungeonID, char m_cDifficulty. NOT in the live struct.]
[SERV_SECRET_HELL] CX2Dungeon::DUNGEON_MODE m_eDungeonMode (224); bool m_bUpperDifficulty (228); CX2DungeonRoom::RANK_TYPE m_eDungeonRank (230);
int m_iDungeonClearTime (231); NPC_UNIT_ID m_eTalkNPCID (233); bool m_bTalkNPC (234); CX2PVPRoom::PVP_GAME_TYPE m_ePVPType (236); int m_iPVPPlay (237);
int m_iPVPWin (238); int m_iPVPKill (239); float m_fQuestItemDropRate (240); int m_iDungeonDamage (241); int m_iDungeonClearCount (242);
[SERV_QUEST_CLEAR_EXPAND] int m_iEnchantLevel (245); int m_iSocketCount (246); KItemAttributeEnchantInfo m_kAttribEnchantInfo (247);
CX2EnchantItem::ATTRIB_COUNT_TYPE m_eAttribCountType (248); int m_iResolveCount (249); int m_iSkillID (250); int m_iSpUseCount (251);
[SERV_NEW_PVP_QUEST] KPVPChannelInfo::PVP_CHANNEL_CLASS m_ePvpChannelClass (255);
[SERV_ENTER_FIELD_QUEST_CLEAR + REFORM_QUEST] std::set<int> m_setVillageMapID (260), std::set<int> m_setBattleFieldID (261), int m_iFindNPCID (262)
 [dead #else: int m_iVillageMapID, int m_iBattleFieldID (264-265)];
[SERV_ACCOUNT_MISSION_SYSTEM] int m_iCharLevel (270);
[SERV_EVENT_TITLE_SUBQUEST, DEAD] int m_iUseTitleID (274);
[SERV_SUB_QUEST_USE_ITEM] std::vector<int> m_vecUseItemID (278), int m_iUseItemNum (279);
[SERV_POINT_COUNT_SYSTEM, DEAD] int m_iPointCount (283);
[SERV_SKILL_USE_SUBQUEST] int m_iUseSkillCount (287), std::set<int> m_setSkillID (288);
[SERV_SUB_QUEST_LEARN_NEW_SKILL] int m_iLearnNewSkillCount (292).
Ctor h:295-367 zero/-1 defaults (m_iFindNPCID=0, m_iCharLevel=0, m_eDungeonMode=DM_INVALID, m_eDungeonRank=RT_NONE, m_eTalkNPCID=NUI_NONE).

### ITEM_DATA (h:370-384): int m_iItemID, m_iQuantity, m_iPeriod, m_iSocketOption1.
### Reward (h:387-411): bool m_bEnable (389); int m_iEXP (390); int m_iED (391); std::wstring m_wstrName (392, title); int m_iBuff (393); int m_iSP (394);
CX2Unit::UNIT_CLASS m_eChangeUnitClass (395); std::vector<ITEM_DATA> m_vecItem (396). Ctor: m_bEnable=true, rest 0 / UC_NONE.
### SelectReward (h:414-424): int m_iSelectionCount; std::vector<ITEM_DATA> m_vecSelectItem.
### SubQuestTemplet (h:426-441): int m_iID (428); std::wstring m_wstrDescription (430); SUB_QUEST_TYPE m_eClearType (431);
ClearCondition m_ClearCondition (432); bool m_bAutomaticDescription (433). Defaults: m_iID=-1, SQT_NONE, auto=true.
### QuestTemplet (h:443-575), declaration order (live branch):
[ADD_SERVER_GROUP, live] int m_iServerGroupID (446); int m_iID (448); QUEST_TYPE m_eQuestType (450); NPC_UNIT_ID m_eStartNPCID (452), m_eEndNPCID (453);
std::wstring m_wstrTitle (454); int m_iFairLevel (455); [SERV_DAILY_QUEST] QUEST_REPEAT_TYPE m_eRepeatType (459) [dead #else: bool m_bRepeat 461];
bool m_bIsPcBang (466); bool m_bIsTimeEvent (469); wstring m_wstrDissConditionText (472), m_wstrGreetingText (473), m_wstrMainText (474),
m_wstrThanksText (475), m_wstrDissClearText (476); QuestCondition m_Condition (477); std::vector<int> m_vecSubQuest (478);
[REFORM_QUEST] std::map<int, std::vector<int>> m_mapSubQuestGroup (480, "<index, subquest ids>"); Reward m_Reward (483); SelectReward m_SelectReward (484);
[SERV_EPIC_QUEST] int m_iEpisodeGroupID (487); wstring m_wstrEpisodeGroupTitle (488); int m_iPlayLevel (489); wstring m_wstrStartScene (490);
wstring m_wstrEndScene (491); int m_iAfterQuestID (492; leftover int, never filled); [SERV_IRUHADEV_OFFLINE] std::vector<int> m_vecAfterQuestID (508, ours; the Lua key m_iAfterQuestID is a table);
vector<int> m_vecShowItemID (513), m_vecHideItemID (514), m_vecHideSkillID (515); vector<CX2Inventory::MEMBERSHIP_PRIVATE_BANK> m_vecHideBankGrade (516);
[SHOW_DAILY_REPEAT_MARK..., DEAD] bool m_bStartTimeEvent (519); [SERV_RANDOM_DAY_QUEST] int m_iRandomGroupID (523); int m_iNextVillageID (525).
Other structs: SubQuestInst (h:577: int m_iID, UCHAR m_ucClearData, bool m_bIsSuccess), QuestInst (h:600: int m_iID, UidType m_OwnorUnitUID, vector<SubQuestInst>),
ChangeJobQuest (h:661: std::set<int> m_setQuestID[CJT_JOB_END]).

## 2. Lua parsers (cpp:497-1005). Macro LUA_GET_VALUE* = (luaManager, "key", member, default). `_RETURN` variants run `goto LoadFail` / `return false` when the key is missing.
### AddQuestTemplet_LUA (cpp:497-849) keys in order
- m_iServerGroupID -> m_iServerGroupID (cpp:508, ADD_SERVER_GROUP; the >= SGI_END reject at :511 is compiled OUT because EXTEND_SERVER_GROUP_MASK is live, :509-510).
- m_iID -> m_iID (516, required); m_eQuestType -> m_eQuestType (520, required); [EXCEPTION_EPIC_QUEST_TEMPLET dead, :522-527].
- m_eStartNPCID (529), m_eEndNPCID (530) required; m_wstrTitle (533) req; m_iFairLevel (537) req; m_eRepeatType (541, SERV_DAILY_QUEST) req [dead m_bRepeat 543];
  m_bIsPcBang (548) req; m_bIsTimeEvent (551) req; m_wstrMainText (555), m_wstrThanksText (560), m_wstrDissClearText (565) req.
- SERV_EPIC_QUEST block (569-613): m_iEpisodeGroupID (571), m_iPlayLevel (572), m_iFairLevel (573 again), m_wstrEpisodeGroupTitle (575), m_wstrStartScene (576),
  m_wstrEndScene (577) all optional; [SERV_IRUHADEV_OFFLINE 579-601] table m_iAfterQuestID -> m_vecAfterQuestID (entries > 0); m_iNextVillageID (603);
  then episode title is inserted into m_mapEpicGroupTitle (605-611).
- m_iRandomGroupID -> m_iRandomGroupID (616, SERV_RANDOM_DAY_QUEST).
- LoadQuestCondition (619) failure -> LoadFail. Warning ErrorLogMsg(XEM_ERROR125) if both m_eUnitType and m_setUnitClass are set (624-630).
- If m_eQuestType==QT_CHANGE_JOB: key CHANGE_JOB_TYPE (637) -> m_mapChangeJobQuest[first class].m_setQuestID[type] (645).
- table m_vecSubQuest -> m_vecSubQuest (650-662, ids > 0).
- [REFORM_QUEST 668-699] table m_vecSubQuest_Group: array of arrays of sub ids (-1 skipped) -> m_mapSubQuestGroup[index 1..]. (group i = i-th inner table.)
- LoadReward("Reward") (701); table SelectReward (704): m_iSelectionCount (706), m_vecSelectItem[] each m_iItemID, m_iQuantity, m_iPeriod, m_sSocketOption -> m_iSocketOption1 (708-717; kept when ItemID and Quantity != 0).
- tables of ints (> 0): SHOW_QUEST_ITEM -> m_vecShowItemID (735; the QT_EVENT reject at :744-751 is compiled out, INT_SHOW_QUEST_ITEM live);
  HIDE_QUEST_ITEM -> m_vecHideItemID (761); HIDE_QUEST_SKILL -> m_vecHideSkillID (776); HIDE_QUEST_BANK_GRADE -> m_vecHideBankGrade (790).
- SERV_RANDOM_DAY_QUEST registers id in m_mapRandomQuest[groupID] (804-831). Duplicate id in m_mapQuestTemplet -> LoadFail (834). Insert at 839.
- LoadFail (842-848): ErrorLogMsg(XEM_ERROR79, "QuestTempletLoadFail_<id>_<title>"), delete, return false.
### LoadQuestCondition (cpp:2192-2300), root table "QuestCondition" (a missing table silently returns true, defaults kept)
- m_iEnableVillage: TABLE of ints > 0 -> m_setEnableVillage (2198-2213; SERV_EPIC_QUEST + SERV_REFORM_QUEST both live). [dead scalar variant 2215]
- m_iLevel -> m_iLevel (2218, REQUIRED, `return false`).
- [REFORM_QUEST] m_iLimitLevel -> m_iLimitLevel (2220, opt, -1); tables m_iEnableDungeon -> m_setEnableDungeon (2222-2237, > 0); m_iEnableBattleField -> m_setEnableBattleField (2238-2253, > 0).
- m_eUnitType -> m_eUnitType (2255, opt); table m_eUnitClass (up to 99, UC_NONE skipped) -> m_setUnitClass (2258-2272);
  m_iConditionItemID (2274) and m_iConditionItemNum (2275) REQUIRED (`return false`); m_iNotBeforeQuestID (2276, opt, -1);
  table m_vecBeforeQuestID (entries >= 0) -> m_vecBeforeQuestID (2279-2294).
### LoadReward (cpp:2304-2341) keys in table "Reward": m_bEnable (req, true) m_bEnable; m_iEXP; m_iED; m_wstrName -> m_wstrName; m_iBuff; m_iSP; m_eChangeUnitClass; table m_vecItem[] of {m_iItemID, m_iQuantity, m_iPeriod, m_sSocketOption}.
### AddSubQuestTemplet_LUA (cpp:851-882): m_iID (860, req) -> m_iID; m_wstrDescription (862, req); m_eClearType (863, req, enum SUB_QUEST_TYPE); m_bAutomaticDescription (864, req, default true);
then LoadClearCondition (867) -> LoadFail on false. Insert into m_mapSubQuestTemplet (871) (no duplicate check). LoadFail (875-881): ErrorLogMsg(XEM_ERROR80, "SubQuestTempletLoadFail_<id>_<desc>"), delete, return false.
### Other parsers
- AddQuestTempletTrans_LUA / AddSubQuestTempletTrans_LUA (cpp:885, 947): inside LUA_TRANS_DEVIDE = DEAD (QuestTrans.lua / SubQuestTrans.lua text overlay; XEM_ERROR105 on id mismatch).
- AddAccountQuestDate_LUA (cpp:988-1005, DAY_OF_THE_WEEK_QUEST live): keys AccountQuestID, DayOfWeek (UINT, 1=Sunday..7=Saturday per cpp:1008-1011 comment) -> m_mapDayOfTheWeekQuestList[id]=day; either missing (-1) -> returns false.
- There is NO separate AddAccountQuestTemplet parser: the Lua name AddAccountQuestTemplet_LUA is bound to CX2QuestManager::AddQuestTemplet_LUA (X2Main.cpp:3560, SERV_ACCOUNT_MISSION_SYSTEM).
- June-only (SERV_IRUHADEV_ELYSION_VILLAGE): AddVillageNpcRelationDataByQuest_LUA (cpp:185) / ByVillage (cpp:229), LoadVillageNpcRelationData (cpp:169).

## 3. LoadClearCondition (cpp:2345-3098)
Root = table "m_ClearCondition" (cpp:2347). If that table is ABSENT the function falls out to `return false;` (cpp:3092) -> XEM_ERROR80 for ANY sub-quest lacking it.
Helper pattern "TBL(key)->set" = `BeginTable(key)`, loop GetValue(index, buf), `if buf > 0` insert (missing table is silently skipped = empty set). All dungeon reads below are `m_eDungeonID` TABLE -> m_setDungeonID (REFORM_QUEST live);
**m_cDifficulty is never read** in the live build (every read sits in `#ifndef REFORM_QUEST`, cpp:2398, 2498, 2550, 2586, 2626, 2666, 2733, 2812). m_eDungeonMode read with LUA_GET_VALUE_ENUM (optional, DM_INVALID) wherever noted. m_bUpperDifficulty is read `_RETURN` (key required, default false) where noted.
Per case (all inside the live flags unless said):
- SQT_NPC_TALK (2353): m_eTalkNPCID (REQ enum) -> m_eTalkNPCID; REFORM_QUEST: TBL m_eVillageID -> m_setVillageMapID. (m_bTalkNPC read is commented out, 2356.)
- SQT_NPC_HUNT (2375): TBL m_eDungeonID -> m_setDungeonID; m_eDungeonMode (opt); m_bUpperDifficulty (REQ); [SERV_ENTER_FIELD_QUEST_CLEAR+REFORM] TBL m_eFieldID -> m_setBattleFieldID; TBL m_eKillNPCID -> m_setKillNPCID; m_iKillNum (REQ, default 0) -> m_iKillNum.
- SQT_ITEM_COLLECTION (2445): m_iCollectionItemID (REQ), m_iCollectionItemNum (REQ). Nothing else (dungeon reads commented out 2447-2450).
- SQT_QUEST_ITEM_COLLECTION (2456): TBL m_eDungeonID -> m_setDungeonID; TBL m_eFieldID -> m_setBattleFieldID; m_eDungeonMode (opt); m_bUpperDifficulty (REQ); TBL m_eKillNPCID -> m_setKillNPCID; m_iCollectionItemID (REQ); m_iCollectionItemNum (REQ); m_fQuestItemDropRate (REQ, float) -> m_fQuestItemDropRate.
- SQT_DUNGEON_TIME (2527): TBL m_eDungeonID; m_eDungeonMode (opt); m_bUpperDifficulty (REQ); m_iDungeonClearTime (REQ).
- SQT_DUNGEON_RANK (2563): TBL m_eDungeonID; m_eDungeonMode (opt); m_bUpperDifficulty (REQ); [SERV_EVENT_TITLE_SUBQUEST dead m_iUseTitleID]; m_eDungeonRank (REQ enum RANK_TYPE) -> m_eDungeonRank; m_iDungeonClearCount (opt, 0).
- SQT_DUNGEON_DAMAGE (2603): TBL m_eDungeonID; m_eDungeonMode (opt); m_bUpperDifficulty (REQ); m_iDungeonDamage (REQ, -1).
- SQT_DUNGEON_CLEAR_COUNT (2639) + SQT_SUITABLE_LEVEL_DUNGEON_CLEAR (2641, SERV_SUITABLE_LEVEL_DUNGEON_CLEAR_SUB_QUEST): TBL m_eDungeonID; m_eDungeonMode (opt); m_bUpperDifficulty (REQ); m_iDungeonClearCount (REQ, -1). (dead extras: m_iUseTitleID, m_eDungeonRank under SERV_EVENT_TITLE_SUBQUEST 2676-2679)
- SQT_PVP_PLAY (2683): m_ePVPType (REQ enum) -> m_ePVPType; m_iPVPPlay (REQ). SQT_PVP_WIN (2690): m_ePVPType; m_iPVPWin (REQ). SQT_PVP_KILL (2697): m_ePVPType; m_iPVPKill (REQ); [SERV_NEW_PVP_QUEST] m_ePvpChannelClass (REQ enum).
- SQT_WITH_DIF_SERV_USER (2709, SERV_INTEGRATION live): TBL m_eDungeonID; m_eDungeonMode (opt); m_bUpperDifficulty (REQ); m_iDungeonClearCount (REQ).
- [SERV_QUEST_CLEAR_EXPAND 2750-2832] SQT_ITEM_ENCHANT: m_iEnchantLevel (REQ), m_iCollectionItemID (opt). SQT_ITEM_SOCKET: m_iSocketCount (REQ), m_iCollectionItemID (opt).
  SQT_ITEM_ATTRIB: m_eAttribCountType (REQ enum), m_iCollectionItemID (opt), m_eAttribEnchant0/1/2 (opt, cast char) -> m_kAttribEnchantInfo.m_cAttribEnchant0/1/2.
  SQT_ITEM_RESOLVE: m_iResolveCount (REQ), m_iCollectionItemID (opt). SQT_ITEM_EQUIP_DUNGEON_CLEAR (2786): TBL m_eDungeonID; m_eDungeonMode (opt); m_bUpperDifficulty (REQ); m_iDungeonClearCount (REQ); m_iCollectionItemID (REQ).
  SQT_USE_SKILL_POINT: m_iSpUseCount (REQ), m_iSkillID (opt).
- SQT_PVP_NPC_HUNT (2836, SERV_NEW_PVP_QUEST): m_ePVPType (REQ); [SERV_PVP_NPC_HUNT_QUEST_EXTEND live] m_iPVPWin, m_iPVPKill, m_iPVPPlay (all optional, -1); m_ePvpChannelClass (REQ).
- [SERV_ENTER_FIELD_QUEST_CLEAR 2853-2951] SQT_VISIT_VILLAGE (2854): TBL m_eVillageID -> m_setVillageMapID. SQT_VISIT_FIELD (2875): TBL m_eFieldID -> m_setBattleFieldID.
  SQT_VISIT_DUNGEON (2902, REFORM_QUEST): TBL m_eDungeonID -> m_setDungeonID ONLY (no m_bUpperDifficulty read here; see off:259 comment, the array holds dungeon*10+difficulty).
  SQT_FIND_NPC (2918): TBL m_eDungeonID -> m_setDungeonID; TBL m_eFieldID -> m_setBattleFieldID; m_iFindNPCID (REQ enum NPC_UNIT_ID) -> m_iFindNPCID.
- SQT_POINT_COUNT (2953): inside SERV_POINT_COUNT_SYSTEM = DEAD (no case compiled).
- SQT_PVP_PLAY_ARRANGE (2961, PVP_SEASON2): m_iPVPPlay (REQ, 0). SQT_CHAR_LEVEL_UP (2967, SERV_ACCOUNT_MISSION_SYSTEM): m_iCharLevel (REQ, 0).
- SQT_PVP_HERO_NPC_KILL (2975, SERV_PVP_NPC_HUNT_QUEST_EXTEND): m_ePVPType, m_iPVPKill, m_ePvpChannelClass (all REQ). SQT_PVP_HERO_NPC_PLAY (2982): m_ePVPType, m_iPVPPlay, m_ePvpChannelClass (all REQ).
- SQT_ITEM_USE (2991, SERV_SUB_QUEST_USE_ITEM): TBL m_eDungeonID -> m_setDungeonID; TBL m_eFieldID -> m_setBattleFieldID; TBL m_listUseItemID -> m_vecUseItemID (> 0); m_iUseItemCnt (REQ) -> m_iUseItemNum.
- SQT_SKILL_USE (3040, SERV_SKILL_USE_SUBQUEST): m_iUseSkillCount (REQ, -1); TBL m_vecSkillID -> m_setSkillID.
- SQT_LEARN_NEW_SKILL (3061, SERV_SUB_QUEST_LEARN_NEW_SKILL): m_iLearnNewSkillCount (REQ, -1); TBL m_vecSkillID -> m_setSkillID.
- default (cpp:3081-3085), quoted:
  ```
  default:
      {
          luaManager.EndTable();
          return false;
      }
  ```
  Types with NO case in the live build, hence default -> false: SQT_NONE(0), SQT_FEED_PET(19), SQT_USER_DIE(20), SQT_PVP_TAG_COUNT(29), SQT_HYPER_MODE_USE(30), SQT_POINT_COUNT(35), SQT_END.
- Caller (AddSubQuestTemplet_LUA cpp:867-881): `if( LoadClearCondition(...) == false ) goto LoadFail;` -> `ErrorLogMsg( XEM_ERROR80, L"SubQuestTempletLoadFail_<id>_<desc>" )`, SAFE_DELETE, return false. The sub-quest is NOT inserted, so a quest listing it will hold a dangling id (offline log prints "NO TEMPLET", off:~207).
- error_proc (3095): EndTable, return false (reached by any REQ key failure).

## 4. Where the Lua is bound and loaded
- Bindings: CX2Main::RegisterLuabind, X2Lib/X2Main.cpp:3549-3569: class_add CX2QuestManager (3549); AddQuestTemplet_LUA (3550); AddSubQuestTemplet_LUA (3551);
  [SERV_IRUHADEV_ELYSION_VILLAGE] AddVillageNpcRelationDataByQuest_LUA / ByVillage_LUA (3554-3555 area); [SERV_ACCOUNT_MISSION_SYSTEM] AddAccountQuestTemplet_LUA -> &AddQuestTemplet_LUA (3560);
  [LUA_TRANS_DEVIDE, DEAD] Trans binders (3564-3565); [DAY_OF_THE_WEEK_QUEST] AddAccountQuestDate -> AddAccountQuestDate_LUA (3569).
- Loader: CX2QuestManager::OpenScriptFile (cpp:46-50) = lua_tinker::decl "g_pQuestManager" this + g_pKTDXApp->LoadLuaTinker( file ).
- CX2Data::ResetQuestManager( quest, subquest, accountquest ) X2Lib/X2Data.cpp:1918-1932 (SERV_ACCOUNT_MISSION_SYSTEM live branch): OpenScriptFile on the three names in that order, then QuestTrans/SubQuestTrans only under LUA_TRANS_DEVIDE (dead). Declared X2Data.h:184.
- Call sites: X2Lib/X2StateLoading.cpp:803 `ResetQuestManager( L"FieldQuest.lua", L"FieldSubQuest.lua", L"AccountQuest.lua" )` in CX2StateLoading::OnFrameMove, live (#else of FIRST_GAME_LOADING_THREAD_TEST, inside SERV_ACCOUNT_MISSION_SYSTEM);
  :809 `g_pData->OpenDayOfWeekQuestScript( L"AccountQuestDate.lua" )` (DAY_OF_THE_WEEK_QUEST; X2Data.cpp:1951 clears the map then OpenScriptFile).
  X2StateLoading.cpp:1347 (DropItemLoading) is in the DEAD FIRST_GAME_LOADING_THREAD_TEST branch.
- Which kom holds the March FieldQuest.lua / FieldSubQuest.lua / AccountQuest.lua: not checked here (June copies exist under june_src_fixed/data036_kom/{FieldQuest,FieldSubQuest,AccountQuest,AccountQuestDate}.src.lua; also *_LA variants).

## 5. IsCompleteQuest
- `IsCompleteQuest` does NOT exist in March: no definition anywhere in X2Lib, X2/, KTDXLIB. Only hit is a comment about June's function 32322260: X2Lib/X2StateMenu.cpp:6242 (inside CX2StateMenu::CreateWarpDest, SERV_IRUHADEV_ELYSION_PORT).
- March equivalent: `bool CX2QuestManager::GetUnitCompleteQuest( int iQuestID )` decl h:798, def cpp:1339-1348:
  `mit = m_mapCompleteQuest.find( iQuestID ); if( mit == m_mapCompleteQuest.end() ) return false; return true;` (m_mapCompleteQuest = map<int,int> id -> completion count, h:894; daily QRT_DAY entries older than the 6 AM refresh are dropped on load, cpp:1033-1056.)
  Companion: GetMapCompleteQuest() h:815 (const map<int,int>&).
- Callers of GetUnitCompleteQuest (file:line, function, id, flags):
  - X2CashShop.cpp:3631 and :4040, CX2CashShop::Handler_EGS_BUY_CASH_ITEM_REQ, id g_iBankQuestID = 7035 (X2UIPrivateBank.h:8); :3631 in SERV_GLOBAL_BILLING+PRIVATE_BANK (live), :4040 in the #else of SERV_GLOBAL_BILLING (dead).
  - X2DungeonManager.cpp:751, CX2DungeonManager::IsActiveDungeon, id pkDungeonData->m_iRequireCompleteQuestID, SERV_IRUHADEV_JUNE_DUNGEON_SUITABLE_LEVEL (live).
  - X2EventScene.cpp:1560, CX2EventScene::NoviceGuideTrigger, id 11000, `#ifndef REFORM_NOVICE_GUIDE` (DEAD).
  - X2InformerQuest.cpp:98, CX2InformerQuest::CheckHouseQuestInfo, id tempQuestID (each entry of vecAccecptableQuest), unflagged (live).
  - X2NoviceGuide.cpp:86 (fn not resolved), :304 SetNotifyGuide, :1200 DoGuideStep2, :1584 DoGuideStep7: id 11010; all `#ifndef REFORM_NOVICE_GUIDE` (DEAD).
  - X2PlayGuide.cpp:485, CX2PlayGuide::ResetNoviceGuide, id TQI_CHASE_THIEF = 11005 (X2PlayGuide.h:8), unflagged (live).
  - X2QuestManager.cpp:1944, CX2QuestManager::GetAvailableQuest, id MAGIC_EVENT_QUEST_B = 3055 (cpp:1933), `#ifdef TEMP_MEDAL_EVENT_QUEST` (DEAD).
  - X2TFieldNpcShop.cpp:1694, CX2TFieldNpcShop::OnFrameMove, id 11000, `#ifndef REFORM_NOVICE_GUIDE` (DEAD).
  - X2UIQuestNew.cpp:354/361/377/383/397/403/404/419/426/445/452, CX2UIQuestNew::OnFrameMove, ids 11000 and 11010, inside `#ifdef SERV_EPIC_QUEST` but in the `#else` of REFORM_NOVICE_GUIDE (DEAD).
  - X2UIQuestNew.cpp:3028, CX2UIQuestNew::CanIAcceptQuest, id m_Condition.m_vecBeforeQuestID[iC] (SERV_EPIC_QUEST, live).
  - X2UIQuestNew.cpp:4777, CX2UIQuestNew::UpdateOngoingQuestInfo, id kQuestInfo.m_iID (SERV_EPIC_QUEST, live).
  - X2UIQuestReceive.cpp:1544 (UpdateQuestUI) and :3037 (CanIAcceptQuest): id QuestList.m_QuestID / m_vecBeforeQuestID[iC]; both `#ifndef SERV_EPIC_QUEST` (DEAD).
  - X2UIShop.cpp:5056, CX2UIShop::CanBuyItemToCreateGuild, id GUILD_QUEST_ID = 7630 (X2Define.h:1246), NEW_VILLAGE_UI+GUILD_MANAGEMENT (live).
  - GetMapCompleteQuest users (not asked, for completeness): X2QuestManager.cpp:1728, X2UIQuestNew.cpp:2835/3060/4688, X2UIQuestReceive.cpp:2697/3063, X2CashShop (commented).

## 6. Chain / Story / Week / Month / Account / Guild / Day / Daily / Repeat in X2QuestManager.h/.cpp
- Chain: none by name. "Chain" exists only as our offline vector m_vecAfterQuestID (h:508) + comment h:493-509; March client never read m_iAfterQuestID (h:492 leftover int).
- Story: none. Episode grouping = m_iEpisodeGroupID / m_wstrEpisodeGroupTitle / m_mapEpicGroupTitle (h:487-488, :907), GetEpicGroupTitle (h:808, cpp:5189).
- Week: DAY_OF_THE_WEEK_QUEST: AddAccountQuestDate_LUA (h:679, cpp:988), GetQuestDayByQuestID (h:865, cpp:1006, returns 0 if absent, 1=Sun..7=Sat), ClearDayOfTheWeekQuest (h:864), m_mapDayOfTheWeekQuestList (h:923).
- Month: none.
- Account: SERV_ACCOUNT_MISSION_SYSTEM: SQT_CHAR_LEVEL_UP=31 (h:135), ClearCondition::m_iCharLevel (h:270), AccountQuest.lua through the same AddQuestTemplet_LUA (X2Main.cpp:3560); AddAccountQuestDate_LUA (above).
- Guild: none in the manager (guild quest id is X2Define.h:1246 GUILD_QUEST_ID used in X2UIShop.cpp:5056).
- Day / Daily: QRT_DAY (h:57), GetTimeToRefreshQuest (h:783, cpp:1092, refresh hour HOUR_TO_REFRESH_DAILY_QUEST, 6 AM), RefreshDailyQuest (h:784, cpp:1118, erases QRT_DAY ids from m_mapCompleteQuest),
  SetUnitQuest daily overload (h:782, cpp:1022; drops completes older than the refresh time), SetTimedEventQuest (h:839, cpp:5012, SERV_DAY_QUEST), m_bIsTimeEvent (h:469), m_iRandomGroupID / SetRandomQuestInfo / CheckGiveUpRandomQuest (h:523, 847-848, cpp:5207, 5212; SERV_RANDOM_DAY_QUEST).
- Repeat: QUEST_REPEAT_TYPE/m_eRepeatType (h:55, 459), QRT_REPEAT=2 (complete -> can be taken again at once); GetNewQuest/GetCompleteQuest out-params nRepeat (h:825, 827).
- Other relevant: SetHasDungeonQuest (h:861, cpp:5540), GetNeedTo{TalkNPC,VisitVillage,VisitField,ClearDungeon}List (h:856-859, QUEST_GUIDE), CompleteQuestToVisitVillage / InsertCompletedVisitQuestID (h:852-853, cpp:5264/5293), IsForbiddenQuest/GiveUpForbiddenQuest (h:844-845), HasSkillUseSubQuest (h:881).

## 7. Offline (X2Lib/Offline/X2OfflineQuest.cpp, Handlers_Quest.cpp): sub-quest dungeon matching
- Matcher: `CX2OfflineQuest::IsExistDungeonInSub( pSub, iDungeonID )` off:637-669 (copy of server CXSLQuestManager::IsExistDungeonInSubQuest:2147). Argument is always `iDungeonID + (int)cDifficulty` (dungeon id is a multiple of 10, difficulty 0..2).
  - m_bUpperDifficulty == false: `m_setDungeonID.find( id ) != end()` (exact dungeon+difficulty). off:643-648.
  - m_bUpperDifficulty == true: compare halves: `id/10 == (*sit)/10 && (*sit)%10 <= id%10` for any member (off:655-662).
  - EMPTY set -> returns false in both modes (loop never runs / find on empty). The function itself has no empty special-case.
- Callers add the "empty = anywhere" guard `false == m_setDungeonID.empty() && false == IsExistDungeonInSub(...)` -> skip:
  - OnNpcDie (off:1378, SQT_NPC_HUNT, in a dungeon): guard at off:1423-1424. Battlefield branch uses m_setBattleFieldID with the same empty = anywhere (off:1415-1421).
  - OnDungeonClear (off:1458; DUNGEON_TIME/RANK/CLEAR_COUNT/DAMAGE; every type): guard at off:1482-1486 ("dungeon filter first, for every type").
  - OnFindNPC (off:1720, SQT_FIND_NPC in dungeon): guard off:1752-1753, logs "wants dungeon ... no match". Battlefield: m_setBattleFieldID, empty = anywhere.
  - OnUseItem (off:2072, SQT_ITEM_USE): off:2104-2108; dungeon set non-empty wins over field filter, "else if" field set non-empty.
  - GetQuestItemDrops (off:2311, SQT_QUEST_ITEM_COLLECTION drops): guard off:2363-2364 (else branch of bBattleField).
  - **OnVisitDungeon (off:1664, SQT_VISIT_DUNGEON) has NO empty guard**: off:1689 `if( false == IsExistDungeonInSub(...) ) continue;` so an empty m_setDungeonID NEVER matches for VISIT_DUNGEON.
- DI_NONE (0): the live loader drops every value <= 0 (`if( buf > 0 )`, cpp:2384 etc.), so a script `m_eDungeonID = { 0 }` or no table both leave m_setDungeonID EMPTY. Result: matches ANY dungeon for NPC_HUNT / clears / ITEM_USE / FIND_NPC / quest-item drops; matches NONE for VISIT_DUNGEON.
  (The scalar `m_eDungeonID` / DI_NONE default and m_cDifficulty exist only in the dead #else branches, h:219-220 and cpp:2391 etc.) m_eDungeonMode is parsed but NOT compared by any offline matcher (grep of off for m_eDungeonMode: none).
- The offline log helper DungeonReqString (off:608-635) prints "<no dungeon named>" for an empty set.
- Call chain (Handlers_Quest.cpp): QuestOnNpcDie 913 -> OnNpcDie with m_kRoom.m_kInfo.m_iDungeonID (924); QuestOnDungeonClear 942 -> OnDungeonClear (951); OnVisitDungeon calls at :144 and :1024; QuestOnFindNPC 1031 -> OnFindNPC (1044). Triggers from Handlers_Room.cpp 1159, 2609, 2985 (FindNPC), 3396/3408 (NpcDie), 3606/3893 (GetQuestItemDrops), 4476 (clear).
- IsClearTypeUndriven (off:541-613) labels these undriven (returns a reason string; NULL = driven):
  - "no PvP offline": SQT_PVP_PLAY, PVP_WIN, PVP_KILL, PVP_NPC_HUNT, PVP_PLAY_ARRANGE, PVP_TAG_COUNT, PVP_HERO_NPC_KILL, PVP_HERO_NPC_PLAY.
  - SQT_WITH_DIF_SERV_USER: needs a party member from another server group.
  - "shipped GameServer never ticks this type either": SQT_FEED_PET, SQT_USER_DIE, SQT_HYPER_MODE_USE.
  - SQT_ITEM_ATTRIB: item attribute system compiled out. SQT_ITEM_RESOLVE: disassembly not implemented offline.
  - SQT_SUITABLE_LEVEL_DUNGEON_CLEAR: decided by the room server. SQT_ITEM_EQUIP_DUNGEON_CLEAR: server's own check derefs end().
  - (ITEM_ENCHANT and ITEM_SOCKET removed from the list, driven by OnEnchantItem/OnSocketItem.)

## 8. Progress string builder
- `std::wstring CX2QuestManager::MakeSubQuestString( bool bAuto, const SubQuestTemplet*, const QuestInst*, bool bShowCount = true )` decl h:822, def cpp:3277-4913 (big switch on m_eClearType, case list starts cpp:3294; `return wstrmQuestCond.str()` at cpp:4912).
  Companion: LineBreakSubQuestString h:823 / cpp:3240.
- Live (REFORM_QUEST) SQT_NPC_HUNT: the auto-text block (cpp:3301-3384) is `#ifndef REFORM_QUEST` = DEAD; live is `m_wstrDescription << " " << m_ucClearData << "/" << m_iKillNum` (cpp:3385-3398).
- Other "x/y" lines: ITEM_COLLECTION/QUEST_ITEM_COLLECTION uses inventory count / m_iCollectionItemNum (cpp:3413-3416, 3518-3520); DUNGEON_RANK x/m_iDungeonClearCount (3722-3731); DUNGEON_CLEAR_COUNT (3911-3933);
  PVP (4141-4278); ENCHANT/SOCKET/RESOLVE/SKILL_POINT (4284-4640); PVP_NPC_HUNT (4655); VISIT_VILLAGE/DUNGEON show names or description (4668-4730); FIND_NPC/VISIT_FIELD description only (4703, 4730);
  ITEM_USE x/m_iUseItemNum (4743); POINT_COUNT (dead); PVP_HERO (4799, 4814); SKILL_USE (4857); LEARN_NEW_SKILL (4901).
- Callers: X2UIQuestNew.cpp 3823, 3876, 6345, 6398; X2UIQuestReceive.cpp:3177; X2UIQuest.cpp 1582, 1832, 1880; X2State.cpp 4836-5022; X2QuestManager.cpp:4985, 5646, 5696, 5704.
- Sub-quest completion rule (SubQuestInst::IsComplete cpp:3203-3238): item collection types compare bag count with m_iCollectionItemNum; every other type uses m_bIsSuccess. QuestInst::IsComplete cpp:3146 = all subs complete.

## 9. GetBoardStartQuestList / BoardVillageID
- `bool CX2QuestManager::GetBoardStartQuestList( std::vector<int>& vecQuestID )` decl h:807 (SERV_EPIC_QUEST), def cpp:1587-1703. Only caller: GetAvailableQuest cpp:1718 (SERV_EPIC_QUEST).
- MODIFY_ACCEPT_QUEST is DEAD so cpp:1605-1606 is used: `CurrentVillageID = g_pData->GetLocationManager()->GetCurrentVillageID()` (the #if branch at 1590-1604 would add the XS_BATTLE_FIELD return-village case).
- `BoardVillageID` (cpp:1608) defaults to SEnum::VMI_RUBEN; switch(CurrentVillageID) at cpp:1610-1673 maps village + its DUNGEON_GATE / DUNGEON_LOUNGE / BATTLE_FIELD_*_REST_00 ids to the hub:
  RUBEN (Ruben, gate, lounge), ELDER (+rest_00), BESMA, ALTERA, VELDER, PEITA (lounge, rest_00; no gate), HAMEL, SANDER (+rest_00). Any other id (incl. VMI_INVALID, e.g. Elysion/Atlas) falls through to RUBEN.
- Then (cpp:1677-1687): for every house in GetVillageMapTemplet(BoardVillageID)->m_HouseList, for every NPC in the house GetStartQuestList( npc, vecQuestID ) (no NULL check on the templet).
- Extra (cpp:1690-1699): if CurrentVillageID != VMI_PEITA also GetStartQuestList for NUI_GLAVE, NUI_LUTO_VILLAGE, NUI_EVENT_DAO and (EVENT_NPC_IN_VILLAGE undefined) NUI_EVENT_BENDERS. Returns true always.
- Note: no June village (Elysion/Atlas) is in the switch, so on those villages the board lists Ruben's NPC quests (BoardVillageID = RUBEN).
