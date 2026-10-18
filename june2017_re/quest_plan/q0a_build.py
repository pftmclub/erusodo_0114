"""Quest Q0a: build q0a_engine_map.tsv.

Every key in q_june_quest_keys.tsv and every type in q_june_quest_types.tsv gets one row (default rule
by status, overridden by KEY / TYPE below); FUNCS holds the June functions, binders, loaders and readers.
Evidence: q0a_june_templets.md, q0a_june_clearcond.md, q0a_june_callers.md, q0a_qm_functions.tsv,
q0a_qm_summary.md, q0a_march_digest.md. Run: python q0a_build.py (writes q0a_engine_map.tsv next to it).
"""
import csv, os

HERE = os.path.dirname(os.path.abspath(__file__))
COLS = ["june_va", "june_name", "march_equivalent", "verdict", "target", "evidence"]

# ---- keys: (table, keypath) -> (june_va, june_name, march, verdict, target, evidence)
QT = "323321f0 AddQuestTemplet_LUA"
CC = "32323ad0 clear condition"
QC = "32323120 QuestCondition"
RW = "32330250 Reward"
KEY = {
    ("quest", "m_fSelectBGM"): ("323321f0 @32335659", "QuestTemplet+0x28c m_fSelectBGM (wstring, not float)", "new", "new member; read after the duplicate check; no reader in 32318000-3234a000", "Q1 (store); reader Q0b/Q5b", "q0a_june_templets.md row 38"),
    ("quest", "m_iChainID"): ("323321f0 @323342ad", "QuestTemplet+0x278 m_iChainID", "new", "new; != 0 -> manager+0xfc map<chain, {set<questID>, AllowFewDays, TestMinute}>", "Q1 (parse + index); Q3 (chain rule 3232caf0); Q4 (emulator)", "q0a_june_templets.md index table; 3232caf0"),
    ("quest", "m_vecAdditionalSceneQuestID"): ("323321f0 @32334478", "QuestTemplet+0x27c vector<int> (> 0)", "new", "new; stored only in the quest manager", "Q1 (store); reader Q0b/Q5b", "q0a_june_templets.md row 27"),
    ("quest", "m_ePublisher"): ("-", "not read by June's client", "not read", "server-only key (no plain or XOR string in the June exe)", "June-only, not ported: client never reads it", "q0a_june_templets.md 'keys that do not exist'"),
    ("quest", "m_fRandomRate"): ("-", "not read by June's client", "not read", "server-only key", "Q4 (emulator: check March UserQuestManager for random-day use)", "q0a_june_templets.md"),
    ("quest", "m_iFairDungeonID"): ("-", "not read by June's client", "not read", "server-only key", "June-only, not ported: client never reads it", "q0a_june_templets.md"),
    ("quest", "m_vecAcceptRewardItem"): ("-", "not read by June's client", "not read", "server-only (accept-time item grant, 7 quests)", "Q4 (emulator pays on accept if March's server does)", "q0a_june_templets.md"),
    ("quest", "m_vecAcceptRewardItem[].m_iItemID"): ("-", "not read by June's client", "not read", "see m_vecAcceptRewardItem", "Q4", "q0a_june_templets.md"),
    ("quest", "m_vecAcceptRewardItem[].m_iQuantity"): ("-", "not read by June's client", "not read", "see m_vecAcceptRewardItem", "Q4", "q0a_june_templets.md"),
    ("quest", "m_bRepeat"): ("-", "not read by June's client", "dead #else in March (cpp:543)", "obsolete key; both read m_eRepeatType", "June-only, not ported: neither client reads it", "q0a_march_digest.md s2"),
    ("quest", "QuestCondition.m_iEnableField"): ("-", "not read; June reads m_iEnableBattleField (+0x58)", "m_setEnableBattleField (cpp:2238)", "data key unread on both sides", "June-only, not ported: neither client reads it", "q0a_june_templets.md QuestCondition row 5"),
    ("quest", "QuestCondition.m_eUnitClass"): (QC + " @32323572", "unit-class set +0x08; value >= 1000 -> cond+0x18 = value % 1000 (QR_NONE 0, QR_TRANSCENDENTAL_CLASS 1)", "m_setUnitClass (cpp:2258)", "changed: QR tail goes to a new scalar", "Q1 (parse); reader Q3 (IsForbiddenQuest 3232e510), consumer Stage 6", "decompile 32323120 lines 175-191"),
    ("quest", "QuestCondition.m_iNotBeforeQuestID"): (QC + " @3232392c", "cond+0x24", "m_iNotBeforeQuestID", "same", "-", "q0a_june_templets.md"),
    ("quest", "Reward.m_eChangeUnitClass"): (RW + " @3233070e / @3233088e", "QT 9: reward+0x38 = value % 1000 (miss -> 0); else reward+0x30 raw", "m_eChangeUnitClass (raw)", "changed: QT 9 split", "Q1 (parse both); Q4 refuses the transcendental change (Stage 6)", "CMP [EBP+8],9 @323306b5"),
    ("quest", "m_eRepeatType"): (QT + " @32332ad3", "QuestTemplet+0x34", "m_eRepeatType", "same key; June adds values 3/4 (SetUnitQuest 32336e80)", "Q3 (week/month expiry)", "q0a_qm_summary.md s2"),
    ("quest", "m_iAfterQuestID"): (QT + " (table)", "vector<int> +0x214", "m_vecAfterQuestID (ours, SERV_IRUHADEV_OFFLINE)", "same as our revival", "-", "q0a_june_templets.md row 19"),
    ("subquest", "m_ClearCondition.m_cDifficulty"): (CC, "cond+0x13c int, missing -> -1 (stub 3232434c), still true", "none live (dead #ifndef REFORM_QUEST)", "new member; read in all 12 dungeon cases; no reader in the quest-manager range", "Q1 (parse); Q3 (find a UI reader before porting one)", "q0a_june_clearcond.md N1; 699 agree / 123 differ with id%10"),
    ("subquest", "m_ClearCondition.m_eDungeonID"): (CC, "set<int> cond+0x40, inserts values >= 0", "m_setDungeonID, inserts > 0 (cpp:2384)", "changed: June keeps 0 (5 June sub-quests list {0})", "Q1 (verbatim >= 0); Q4 (a {0} set matches no dungeon)", "CMP EAX,EBX; JL @32323d09"),
    ("subquest", "m_ClearCondition.m_bExpDungeonAndField"): ("-", "not read by June's client (no plain/XOR string)", "none", "server-only key (36 sub-quests)", "Q4 (stand-in proposal, ask)", "Ghidra strings filter 'ExpDungeonAndField': 0"),
    ("subquest", "m_ClearCondition.m_eExceptionDungeonID"): ("-", "not read by June's client", "none", "server-only key (6 sub-quests)", "Q4 (stand-in proposal, ask)", "Ghidra strings filter 'ExceptionDungeon': 0"),
    ("subquest", "m_ClearCondition.m_ePvpRank"): (CC + " case 45", "cond+0x118 (required)", "none", "new", "Q1 (parse); Q4 undriven (no PvP)", "q0a_june_clearcond.md"),
    ("subquest", "m_ClearCondition.m_ePvpTournamentType"): (CC + " cases 9/10", "optional, default 0", "none", "new", "Q1 (parse); Q4 undriven (no PvP)", "q0a_june_clearcond.md"),
    ("subquest", "m_ClearCondition.m_bIsGuildJoin"): (CC + " case 38", "cond+0x120 (required)", "none", "new", "Q1 (parse); Q4 undriven (guild decision)", "q0a_june_clearcond.md"),
    ("subquest", "m_ClearCondition.m_iCollectionHonorPointCount"): (CC + " case 39", "cond+0x11c (required)", "none", "new", "Q1 (parse); Q4 undriven (guild decision)", "q0a_june_clearcond.md"),
    ("subquest", "m_ClearCondition.m_iExchangeCount"): (CC + " case 43", "cond+0x124 (required)", "none", "new", "Q1 (parse); Q4 (drive or label)", "q0a_june_clearcond.md"),
    ("subquest", "m_ClearCondition.m_vecExchangeSourceItem_ID"): (CC + " case 43 @3232c0cf", "vector<int> cond+0x128; loop exit inverted: a non-empty table pushes nothing", "none", "new; June bug kept (JUNE_BUGS 321)", "Q1 (verbatim); Q4", "TEST AL,AL; JNZ exit @3232c0cf"),
    ("subquest", "m_ClearCondition.m_iCharLevel"): (CC + " case 31", "int (required)", "m_iCharLevel (SERV_ACCOUNT_MISSION_SYSTEM)", "same", "-", "q0a_march_digest.md s3"),
    ("subquest", "m_ClearCondition.m_bUpperDifficulty"): (CC, "cond+0x54 bool, required in 2,4,5-8,12,17,32,37,48; not read in 22/26/27", "m_bUpperDifficulty", "same (June adds cases 37/48)", "Q1", "q0a_june_clearcond.md N1"),
    ("subquest", "m_ClearCondition.m_eDungeonMode"): (CC, "cond+0x50 optional", "m_eDungeonMode", "same; June data has 2 DUNGEON_MODE[\"n\"] string indexes (nil -> 0)", "-", "FieldSubQuest.src.lua:20727, 34518"),
    ("account", "m_bIsAccount"): (QT + " @323332fe", "QuestTemplet+0x270 bool", "none (account = AccountQuest.lua via the same parser, X2Main.cpp:3560)", "new; no reader in the quest-manager range", "Q1 (store); Q4 (account-wide progress)", "q0a_qm_summary.md s4"),
    ("guild", "m_bIsGuild"): (QT + " @3233341e", "QuestTemplet+0x271 bool", "none", "new; no reader in range", "Q1 (parse, log, skip guild quests)", "plan decision 2026-10-17"),
    ("guild", "Reward.m_iGuildCoopQuestRewardID"): (RW + " @32330a0b", "QuestTemplet+0x274", "none", "new", "Q1 (parse, log)", "q0a_june_templets.md reward row 8"),
    ("chain", "AddChainQuestDays:m_iChainID"): ("32322b10 @32322ba7", "map +0xfc key (find only, no insert)", "none", "new binder (bound @31dafded)", "Q1 (bind); Q3 rule", "decompile 32322b10"),
    ("chain", "AddChainQuestDays:m_iAllowFewDays"): ("32322b10 @32322c92", "node+0x20", "none", "new", "Q1; Q3 (3232caf0)", "decompile 32322b10"),
    ("chain", "AddChainQuestDays:m_iTestMinute"): ("32322b10 @32322db5", "node+0x24 (used only with enable-code 0x10)", "none", "new", "Q1; Q3", "decompile 3232caf0"),
    ("chain", "AddGroupChainQuest:m_iAllowFewDays"): ("-", "AddGroupChainQuest is NOT bound", "none", "June bug kept: ChainQuestInfo.lua stops at its 6th call", "Q1 (do not bind; JUNE_BUGS 320)", "31d95080: AddChainQuestDays @31dafcfd is the last CX2QuestManager binding"),
    ("chain", "AddGroupChainQuest:m_iChainGroupID"): ("-", "not bound", "none", "as above", "Q1 (do not bind)", "as above"),
    ("chain", "AddGroupChainQuest:m_iChainIDList"): ("-", "not bound", "none", "as above", "Q1 (do not bind)", "as above"),
}
SCENE = ("31eeb040", "scene-info loader -> g_pData+0x498 map by QuestID; reader 323cb190 (caller 323cb2e0)", "none", "new file", "Q1 (load); Q5b (read)", "decompile 31eeb040 / 323cb190")

def key_row(r):
    t, k = r["table"], r["keypath"]
    if (t, k) in KEY:
        return KEY[(t, k)]
    if t == "scene":
        return SCENE
    if t in ("account", "guild") and ("quest", k) in KEY:
        return KEY[("quest", k)]
    if t == "guild":
        return ("323321f0 (shared parser)", k, "AddQuestTemplet_LUA", "guild file, same parser", "Q1 (parse, log, skip)", "plan decision 2026-10-17")
    if t == "account":
        return ("323321f0 (shared parser)", k, "AddQuestTemplet_LUA (X2Main.cpp:3560)", "same parser; June has 5 account quests, March's file is empty", "Q2 (data flip); Q4 (account quests)", "q0a_march_digest.md s2")
    if r["status"] == "both" and r["key_read_by_march_cpp"] == "1":
        return ("parser of the table", k, "same key", "same", "-", "q_june_quest_keys.tsv")
    return ("?", k, "?", "UNMAPPED", "?", "")

# ---- types: name -> row
TYPE = {
    "QT_GUILD": ("323321f0 / 3232fbd0", "8: listed only if **(g_pData+0x2ec) != 0", "March QT_END = 8 (collision)", "new", "Q1 (enum, QT_END -> 11); Q3 (guild gate); quests skipped (decision)", "q0a_qm_summary.md s2"),
    "QT_CHANGE_TRANSCENDENTAL_CLASS": ("3232e510 / 32336310 / 32330250", "9: gated by code events 0x920/0x921/0x923 + job record; reward % 1000", "none", "new", "Q1 (enum); Q3 (IsForbiddenQuest, class-up notice); Stage 6 owns the class change", "q0a_qm_summary.md s2"),
    "QT_STORY": ("323321f0 / 3232de60 / 323d5650", "10: indexed at manager+0xec by m_iEpisodeGroupID; exempt from chain removal; UI sort 8", "none", "new (91 quests)", "Q1 (enum); Q3 (index, chain exemption); Q5a (UI row 323d5aec)", "q0a_qm_summary.md s2"),
    "QRT_WEEK": ("32322050 (in SetUnitQuest 32336e80)", "3: expires after the next Wednesday 00:00 local", "none", "new (32 quests)", "Q1 (enum); Q3 (SetUnitQuest); Q4 (emulator reset)", "q0a_qm_summary.md s2"),
    "QRT_MONTH": ("32321d10 (in SetUnitQuest 32336e80)", "4: expires after the 1st of next month 00:00", "none", "new", "Q1 (enum); Q3; Q4", "q0a_qm_summary.md s2"),
    "CJT_JOB_FOURTH": ("323321f0 @323347a0", "6: change-job index accepts 0 <= v < 7", "March CJT_JOB_END = 6 sizes m_setQuestID[CJT_JOB_END]", "new; array grows", "Q1 (enum + ChangeJobQuest array)", "q0a_june_templets.md index table"),
    "QR_NONE": (QC + " / " + RW, "1000 -> 0 after % 1000", "none", "new table QUEST_REWARD_TRAN_CLASS", "Q1 (Enum.lua + C++)", "see QuestCondition.m_eUnitClass"),
    "QR_TRANSCENDENTAL_CLASS": (QC + " / " + RW, "1001 -> 1 after % 1000", "none", "new", "Q1; Stage 6 consumer", "see Reward.m_eChangeUnitClass"),
    "SQT_PVE_WIN": (CC + " case 37; MakeSubQuestString 32337db0 case 0x25", "dungeon keys + m_iDungeonClearCount", "March SQT_END = 37 (collision)", "new (15)", "Q1 (enum, SQT_END -> 49); Q3 (string); Q4 (drive or label)", "q0a_june_clearcond.md"),
    "SQT_GUILD_JOIN": (CC + " case 38", "m_bIsGuildJoin", "none", "new", "Q1; Q4 undriven (guild)", "q0a_june_clearcond.md"),
    "SQT_GUILD_HONOR_POINT_COLLECTION": (CC + " case 39; string case 0x27", "m_iCollectionHonorPointCount", "none", "new", "Q1; Q3 (string); Q4 undriven (guild)", "q0a_june_clearcond.md"),
    "SQT_ITEM_EXCHANGE": (CC + " case 43; string case 0x2b", "m_iExchangeCount, m_vecExchangeSourceItem_ID", "none", "new", "Q1; Q3 (string); Q4 (drive or label)", "q0a_june_clearcond.md"),
    "SQT_TOGETHER_SUBQUEST_CLEAR": (CC + " case 44; string case 0x2c", "m_iDungeonClearCount", "none", "new", "Q1; Q3; Q4 (party-only: label or stand-in, ask)", "q0a_june_clearcond.md"),
    "SQT_PVP_RANK": (CC + " case 45", "m_ePvpRank", "none", "new", "Q1; Q4 undriven (no PvP)", "q0a_june_clearcond.md"),
    "SQT_PLAY_TOGETHER_WITH_TITLE": (CC + " case 47 (no keys, returns true)", "-", "none", "new", "Q1; Q4 (party-only: label or stand-in, ask)", "q0a_june_clearcond.md"),
    "SQT_HAS_EXP_DUNGEON_CLEAR": (CC + " case 48 (= case 8/32 body)", "dungeon clear count keys", "none", "new", "Q1; Q3 (string); Q4 (drive as clear count + m_bExpDungeonAndField stand-in)", "q0a_june_clearcond.md"),
    "SQT_SUITABLE_LEVEL_DUNGEON_CLEAR": (CC + " case 32 (= case 8 body)", "same keys as clear count", "SQT 32 (case shared with 8, cpp:2641)", "same", "Q4 (undriven today: decide a driver)", "q0a_march_digest.md s7"),
    "DL_HELL": ("-", "nil in June's Enum: m_cDifficulty read fails -> -1", "nil", "June data bug kept", "Q1 (keep nil; JUNE_BUGS 322)", "q_june_quest_types.tsv"),
    "DL_NOMAL": ("-", "nil (typo of DL_NORMAL)", "nil", "June data bug kept", "Q1 (keep nil; JUNE_BUGS 322)", "q_june_quest_types.tsv"),
    "NUI_EDEL": ("Enum", "June 10067", "March 10072 (Enum.lua + X2UnitManager.h:2700); June 10072 = NUI_STEEL_AND_STEING", "value differs", "Q7 (keep 10072 unless NUI_STEEL_AND_STEING is needed; then ask)", "Enum.src.lua:5346 vs kom Enum.lua:4398"),
    "NUI_": ("-", "empty name in dev sub-quest 205680 -> nil; required m_eTalkNPCID fails, June drops it too", "-", "June data, kept", "-", "FieldSubQuest.src.lua:54602"),
    "DUNGEON_MODE[0]": ("-", "DUNGEON_MODE[\"0\"] string index -> nil -> optional 0", "-", "June data, kept", "-", "FieldSubQuest.src.lua:34518"),
    "DUNGEON_MODE[1]": ("-", "DUNGEON_MODE[\"1\"] -> nil -> 0", "-", "June data, kept", "-", "FieldSubQuest.src.lua:20727"),
}

def type_row(r):
    p, n = r["prefix"], r["name"]
    name = n or p
    if name in TYPE:
        return TYPE[name]
    if p in ("True", "False", "NONE") or n == "DI_NONE":
        return ("-", name, "-", "Lua global / placeholder", "-", "q_june_quest_types.tsv")
    if r["in_march_Enum"] == "1" and r["june_value"] == r["march_value"]:
        return ("Enum", "%s.%s = %s" % (p, n, r["june_value"]), "same value", "same", "-", "q_june_quest_types.tsv")
    if r["in_march_Enum"] != "1":
        tgt = "Q1 (Enum.lua)" if p != "NPC_UNIT_ID" else "Q1 (Enum.lua); Q7 (NPC templet)"
        return ("Enum", "%s.%s = %s" % (p, n, r["june_value"]), "missing in March", "new", tgt, "q_june_quest_types.tsv")
    return ("Enum", "%s.%s" % (p, n), "march %s" % r["march_value"], "UNMAPPED", "?", "")

# ---- functions, binders, loaders, readers
FUNCS = [
    ("31d95080 @31dae93e..@31dafded", "quest-manager Lua bindings", "X2Main.cpp:3549-3569", "June adds AddGuildQuestTemplet_LUA, AddChainQuestDays; Account/Guild/Quest push the same 323321f0", "Q1", "xorstrings.tsv; PUSH 0x323321f0 @31daebfa / 31daf0c0 / 31daf322"),
    ("31edbf70", "CX2Data::ResetQuestManager(field, sub, account, guild)", "X2Data.cpp:1918 (3 files)", "June adds GuildQuest; new 0x14c manager", "Q1", "decompile 31edbf70"),
    ("3258c470 @3258f852", "loader: _LA switch on g_pMain+0x708 (argv[1] == Axeso5)", "X2StateLoading.cpp:803", "US = plain FieldQuest / FieldSubQuest / AccountQuest / GuildQuest; AccountQuest_LA never loaded", "Q1 (US branch only)", "funcmap 31e46300; disasm @3258f852-3258f899"),
    ("3258c470 @3258fa7b", "AccountQuestDate.lua (31ee0a80: clear, open)", "X2StateLoading.cpp:809", "same; June's file has 0 AddAccountQuestDate calls", "-", "AccountQuestDate.src.lua"),
    ("3258c470 @3258fc67", "ChainQuestInfo.lua (31edceb0)", "none", "new file", "Q1", "disasm"),
    ("3258c470 @32590853", "Quest_Event_Scene_Info.lua (31eeb040)", "none", "new file", "Q1; Q5b", "disasm"),
    ("3258c470 @32591ff5", "ChainCashItem.lua (ChainItem:AddChainCashItem)", "none", "cash-shop chain items, not quests", "June-only, not ported here: backlog Stage 16 (item-side)", "ChainCashItem.src.lua"),
    ("3232f070", "CX2QuestManager::OpenScriptFile (void; a Lua error ends that file)", "X2QuestManager.cpp:46", "same", "-", "decompile"),
    ("32337720", "CX2QuestManager ctor (0x14c)", "X2QuestManager.cpp:11", "June adds maps +0x9c, +0xec, +0xfc, +0x10c, +0x11c", "Q1 (members); Q3", "q0a_qm_summary.md layout"),
    ("32336830", "~CX2QuestManager", "X2QuestManager.cpp:19", "frees the new maps", "Q1/Q3", "q0a_qm_functions.tsv"),
    ("323321f0", "AddQuestTemplet_LUA", "X2QuestManager.cpp:497", "new keys + chain / story indexes", "Q1", "q0a_june_templets.md"),
    ("32330ff0", "AddSubQuestTemplet_LUA", "X2QuestManager.cpp:851", "same shape; error 0x2760", "Q1", "q0a_june_templets.md"),
    ("32323120", "LoadQuestCondition", "X2QuestManager.cpp:2192", "QR tail scalar (cond+0x18)", "Q1", "q0a_june_templets.md"),
    ("32323ad0", "LoadClearCondition (48-way jump table @3232c57c)", "X2QuestManager.cpp:2345", "June cases 37-39, 43-45, 47, 48; m_cDifficulty; >= 0 dungeon ids; defaults 19,20,29,30,35,40-42 false (as March)", "Q1", "q0a_june_clearcond.md"),
    ("32330250", "LoadReward", "X2QuestManager.cpp:2304", "QT 9 split, guild coop id", "Q1", "q0a_june_templets.md"),
    ("3232e070", "AddAccountQuestDate_LUA -> map this+0x9c", "X2QuestManager.cpp:988", "same", "-", "q0a_qm_functions.tsv"),
    ("32322b10", "AddChainQuestDays", "none", "new", "Q1", "decompile"),
    ("31eeb040", "Quest_Event_Scene_Info loader", "none", "new", "Q1", "decompile"),
    ("323cb190", "GetQuestEventSceneInfo(quest, ...) (only reader of g_pData+0x498)", "none", "new", "Q5b", "decompile; callers 323cb2e0 x2"),
    ("32322260", "IsCompleteQuest", "GetUnitCompleteQuest X2QuestManager.cpp:1339", "same (renamed)", "-", "q0a_june_callers.md"),
    ("323225e0", "SubQuestInst::IsComplete", "X2QuestManager.cpp:3203", "changed: SQT 3/4 complete on flag OR count; no POINT_COUNT", "Q3", "q0a_qm_summary.md s3"),
    ("3232c640", "QuestInst::IsComplete", "X2QuestManager.cpp:3146", "inlines 323225e0", "Q3", "q0a_qm_functions.tsv"),
    ("3232c750", "QuestInst::IsCompleteVisitQuest", "X2QuestManager.cpp:3158", "changed: no last-sub-quest rule", "Q3", "q0a_qm_summary.md s3"),
    ("32337db0", "MakeSubQuestString", "X2QuestManager.cpp:3277", "new cases 0x25, 0x27, 0x2b, 0x2c; 0x2d-0x2f empty", "Q3", "q0a_qm_summary.md s3"),
    ("32344f30", "find sub-quest record by id", "inline in MakeSubQuestString", "same", "-", "q0a_qm_functions.tsv"),
    ("32321ee0", "GetTimeToRefreshQuest (local midnight)", "X2QuestManager.cpp:1092", "same (US hour 0)", "-", "q0a_qm_functions.tsv"),
    ("32321d10", "monthly expiry", "none", "new", "Q3", "q0a_qm_summary.md s2"),
    ("32322050", "weekly expiry (next Wednesday 00:00)", "none", "new", "Q3", "q0a_qm_summary.md s2"),
    ("32336e80", "SetUnitQuest (daily / weekly / monthly purge, chain times)", "X2QuestManager.cpp:1022", "changed", "Q3", "q0a_qm_functions.tsv"),
    ("3232f830", "RefreshDailyQuest (+ chain refresh 3232ef70)", "X2QuestManager.cpp:1118", "changed", "Q3", "q0a_qm_functions.tsv"),
    ("323229d0", "chain quest in progress?", "none", "new", "Q3", "q0a_qm_functions.tsv"),
    ("32322ad0", "chain startable flag (map +0x10c)", "none", "new", "Q3", "q0a_qm_functions.tsv"),
    ("3232caf0", "IsChainQuestAvailable: AllowFewDays -> local midnight of (last completion + N days) <= now; enable-code 0x10 + TestMinute -> minutes", "none", "new", "Q3; Q4 (emulator mirror)", "decompile 3232caf0"),
    ("3232ef70", "rebuild chain startable map", "none", "new", "Q3", "q0a_qm_functions.tsv"),
    ("3232de60", "forget a completed quest (chain, not QT_STORY)", "none", "new", "Q3", "q0a_qm_functions.tsv"),
    ("32331830", "CreateUnitQuest (+ chain refresh)", "X2QuestManager.cpp:1179", "changed", "Q3", "q0a_qm_functions.tsv"),
    ("32331fb0", "RemoveUnitQuest (chain completion time into +0x11c)", "X2QuestManager.cpp:3125", "changed", "Q3", "q0a_qm_functions.tsv"),
    ("32331dd0", "ClearUnitQuest (also +0x10c / +0x11c)", "X2QuestManager.cpp:3107", "changed", "Q3", "q0a_qm_functions.tsv"),
    ("32331a30", "UpdateUnitQuest", "X2QuestManager.cpp:1276", "same", "-", "q0a_qm_functions.tsv"),
    ("3232e310", "CheckUpdateSubQuestInst", "X2QuestManager.cpp:1200", "same", "-", "q0a_qm_functions.tsv"),
    ("3232e510", "IsForbiddenQuest (+ QT 9 code events 0x920/0x921/0x923)", "X2QuestManager.cpp:5030", "changed", "Q3", "q0a_qm_summary.md s2"),
    ("3232ecd0", "GiveUpForbiddenQuest", "X2QuestManager.cpp:5122", "same", "-", "q0a_qm_functions.tsv"),
    ("3232fbd0", "GetStartQuestList (QT 2/9 at any start NPC; QT 8 guild gate)", "X2QuestManager.cpp:1350", "changed", "Q3", "q0a_qm_functions.tsv"),
    ("3232fcd0", "GetEndQuestList", "X2QuestManager.cpp:1385", "same", "-", "q0a_qm_functions.tsv"),
    ("3232fd70", "GetBoardStartQuestList (location state switch, June villages)", "X2QuestManager.cpp:1588", "changed", "Q3", "decompile 3232fd70"),
    ("323357c0", "GetAvailableQuest (chain flag, QT 8 drop)", "X2QuestManager.cpp:1708", "changed", "Q3", "q0a_qm_functions.tsv"),
    ("3232fff0", "GetCompletableQuest", "X2QuestManager.cpp:2043", "same", "-", "q0a_qm_functions.tsv"),
    ("32330120", "GetOnGoingQuest", "X2QuestManager.cpp:2073", "same", "-", "q0a_qm_functions.tsv"),
    ("3232fa40", "GetCompletableQuestEpic", "X2QuestManager.cpp:1234", "same", "-", "q0a_qm_functions.tsv"),
    ("32337320", "GetNewQuest", "X2QuestManager.cpp:1410", "same", "-", "q0a_qm_functions.tsv"),
    ("32331ca0", "GetCompleteQuest", "X2QuestManager.cpp:1551", "same", "-", "q0a_qm_functions.tsv"),
    ("32336310", "class-up quest notice (QT 2 / QT 9 by class step)", "none", "new", "Q3 (QT 9 half to Stage 6)", "q0a_qm_functions.tsv"),
    ("32344ab0", "class-up notice message", "none", "new", "Q3", "q0a_qm_functions.tsv"),
    ("32322490", "in-progress ids by type + start NPC (0 callers)", "none", "dead in June", "June-only, not ported: no caller", "q0a_qm_functions.tsv"),
    ("32331c00", "GetOnGoingQuest count (0 callers)", "none", "dead in June", "June-only, not ported: no caller", "q0a_qm_functions.tsv"),
    ("3232cf70", "HasCompletableQuest (0 callers)", "X2QuestManager.cpp:2025", "same", "-", "q0a_qm_functions.tsv"),
    ("32322f80", "id-table probe from 42500 (0 callers)", "none", "dead in June", "June-only, not ported: no caller", "q0a_qm_functions.tsv"),
    ("32322290", "clear-condition predicate (cond+0x48 == 1, set +0x40 has no 0)", "none", "unresolved field", "Q3 (resolve with MakeSubQuestString)", "q0a_qm_functions.tsv"),
    ("32335f50", "rebuild g_pData+0x570 from completed quests", "none", "unresolved", "Q3 (identify)", "q0a_qm_functions.tsv"),
    ("32337520", "vector of quest ids -> UI add (not a quest-manager this)", "none", "unresolved", "Q0b / Q5a", "q0a_qm_functions.tsv"),
    # IsCompleteQuest callers
    ("3293c6f0 @3293cbf1", "CX2LocalMapUI::UpdateLocalMapDLG: tab hidden while local-map +0x70 quest not complete", "X2LocalMapUI.cpp:850 (comment :1024)", "missing in March", "Q6", "q0a_june_callers.md row 1"),
    ("323d5650 @323d5a5a", "CX2UIQuestNew::UpdateOngoingQuestInfo epic row", "X2UIQuestNew.cpp:4777", "same", "-", "row 2"),
    ("323d5650 @323d5aec", "UpdateOngoingQuestInfo QT_STORY row", "none", "new", "Q5a", "row 3"),
    ("323cab40 @323caba3", "CX2UIQuestNew::CanIAcceptQuest (before-quest list)", "X2UIQuestNew.cpp:3013/3028", "same", "-", "row 4"),
    ("327da190 @327da29f", "IsActiveDungeon: DungeonData +0x1cc", "X2DungeonManager.cpp:750", "same (ported)", "Q6 (uncomment AD Normal 22218 only)", "row 5"),
    ("327da190 @327da2ea", "IsActiveDungeon: local-map +0x70", "X2DungeonManager.cpp:744 (comment)", "missing", "Q6", "row 6"),
    ("3226b6c0 @3226bb88", "CX2GFXEventScene::End: Quest_11005 + code event 2362 popup", "none", "new", "Q5b (june-gfx G3 owns the class)", "row 7"),
    ("31f14e40 @31f1514f", "server event 0x2db handler: hint quest 26600 (lv 35-70)", "none", "new; packet unresolved", "Q0b", "row 8"),
    ("31d79ac0 @31d79afb", "unlock quest movies (g_pMain+0x7f8 table)", "none", "new", "Q8", "row 9"),
    ("32173120 @321738e3", "CX2UIInventory::CreateWarpDest: Elysion rows", "X2UIInventory.cpp:13996", "missing", "Q6", "row 10"),
    ("3225d340 @3225d745", "CX2WorldMapUI::UpdateWorldMap region gate", "X2WorldMapUI.cpp:424 (level only)", "missing", "Q6", "row 11"),
    ("3225d970 @3225dca3", "CX2WorldMapUI::UpdateFieldMap button gate", "X2WorldMapUI.cpp:1036 (level only)", "missing", "Q6", "row 12"),
    ("323d4390 @323d44ca", "CX2UIQuestNew::UpdateNewQuestNotice local-map gate", "X2UIQuestNew.cpp:2825", "missing", "Q6", "row 13"),
    ("3289dd50 @3289dd68", "CX2PlayGuide Go effect: 11005 not complete and level < 3", "X2PlayGuide.cpp:485 (no level test)", "changed", "Q3", "row 14"),
    ("324627f0 @32462ffd", "CX2StateMenu::CreateWarpDest: Elysion rows", "X2StateMenu.cpp:6230 (comment :6242)", "missing", "Q6", "row 15"),
    ("32f16d50 @32f16e4e", "CX2InformerQuest::CheckHouseQuestInfo", "X2InformerQuest.cpp:98", "same", "-", "row 16"),
    ("31fbde90 @31fbdea1", "CX2TutorialGuideManager::CheckToNeedTutorialGuide (11000)", "none", "new (no March tutorial-guide manager)", "June-only, not ported: TutorialGuide system not in this plan (backlog)", "row 17"),
    # readers
    ("32501bb0 @32502b6a", "CX2LocationManager::OpenScriptFile: EnterCondition RequireCompletedQuestID -> local-map +0x70", "X2LocationManager.cpp:285-308 (not parsed)", "missing; set only by data217 DLG_Map_Local_Elysion.lua:6 (22218)", "Q6", "q0a_june_callers.md part B"),
    ("327e0f80 @327e4f01", "AddDungeonData_LUA m_iRequireCompleteQuestID -> +0x1cc", "X2DungeonManager.cpp:241", "same; data line still commented (data035 DungeonData.lua)", "Q6", "part B"),
    ("-", "MapData REQUIRE_COMPLETE_QUEST_ID / _SUB_QUEST_ID: no June client reader (AddMapData unbound)", "X2OfflineMapData.cpp:155 (no-op)", "server-only; March's server XSLMapData does not read them either", "Q6 (emulator stand-in, propose first)", "Ghidra strings 'REQUIRE_COMPLETE' / 'AddMapData': 0"),
    ("-", "BattleFieldData NEED_ONGOING_QUEST_ID / BATTLE_FIELD_DIRECTION_INFO: no June client reader", "not parsed", "server-only (data036 BattleFieldData.lua:1642-1651, Lanox F04)", "Q6 (emulator stand-in, propose first)", "Ghidra strings 'NEED_ONGOING' / 'DIRECTION_INFO': 0"),
    ("31fbe4a0 @31fbf1a4", "CX2TutorialGuideManager::LoadTutorialGuideInfo CONDITION_GOING/NEW/COMPLETE_QUEST_ID", "none", "new system; evaluator unresolved", "June-only, not ported: TutorialGuide system not in this plan (backlog)", "part B"),
]

def main():
    rows = []
    with open(os.path.join(HERE, "q_june_quest_keys.tsv"), encoding="utf-8") as f:
        for r in csv.DictReader(f, delimiter="\t"):
            v = key_row(r)
            rows.append(("key:%s:%s" % (r["table"], r["keypath"]),) + v)
    with open(os.path.join(HERE, "q_june_quest_types.tsv"), encoding="utf-8") as f:
        for r in csv.DictReader(f, delimiter="\t"):
            v = type_row(r)
            rows.append(("type:%s:%s" % (r["prefix"], r["name"]),) + v)
    for v in FUNCS:
        rows.append(("func",) + v)
    bad = [r for r in rows if r[4] == "UNMAPPED"]
    with open(os.path.join(HERE, "q0a_engine_map.tsv"), "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", lineterminator="\n")
        w.writerow(["item"] + COLS)
        w.writerows(rows)
    print("rows %d (keys+types+funcs), unmapped %d" % (len(rows), len(bad)))
    for b in bad:
        print("UNMAPPED", b[0])

if __name__ == "__main__":
    main()
