# June CX2QuestManager Q0a: range 0x32318000-0x3234a000 (244 functions)

Raw Ghidra output for every function: `scratchpad/qm/dec_<va>.txt`, `dis_<va>.txt`, `xr_<va>.txt`. Table: `june_qm_functions.tsv`.
Totals: 88 quest logic + 6 Elysion row helpers (kind `b-*`), 65 STL/container helpers (`a`), 85 non-quest (`c`: P2P senders 0x32318fd0-0x3231f310, XOR-protected setters, SHA-1, UI hints).
The quest manager proper starts at 0x32322260. Everything before it (0x32318000-0x32322250) is CX2Game / engine code, not quest code.

Object layout recovered (this = g_pData(33775860)+0x4c, 0x14c bytes). Offsets are from the decompiles:
- +4/+8 templet map (value +0x10), +0x18 sub-quest templet map, +0x28 in-progress map (QuestInst*), +0x34/+0x38 completed map<int,int>, +0x58/+0x5c epic group title map<int,wstring>, +0x68/+0x6c episode-group vector map, +0x78 `m_mapTodayRandomQuest`, +0x98 `m_bHasDungeonQuest`, +0x9c/+0xa0 account-quest-date map<int,int>, +0xac..+0xdc Elysion relation rows, +0xec/+0xf0 story (QT 10) index keyed by templet+0x1b4, +0xfc/+0x100 chain config, +0x10c/+0x110 chain startable map<int,bool>, +0x11c/+0x120 chain completion map.
- QuestTemplet (June): +0 server group, +4 id, +8 quest type, +0xc start NPC, +0x10 end NPC, +0x14 title (wstring 0x1c), +0x30 fair level, +0x34 repeat type, +0x38 pcbang, +0x39 time-event, +0x1b4 episode group id, +0x264 start-time-event, +0x268 random group id, +0x270 m_bIsAccount, +0x271 m_bIsGuild, +0x278 m_iChainID (new in June).
- SubQuestInst is 8 bytes: id +0, short ucClearData +4, bool bIsSuccess +6.
- `FUN_322aa950` = `std::map<int,*>::find` (ECX = map object, end sentinel = map+4); `FUN_31edbdb0` = `GetServerCurrentTime()` (CTime base +0x2a0); `FUN_31edca50` = `GetServerCurrentTime64()` (+0x294); `FUN_32164dc0` = `CTimeSpan(days,hours,mins,secs)` (verified by disassembly).

## 1. Who reads the chain map (this+0xfc/+0x100) and what rule applies

Readers of the chain config map (all inside the range; the map's `find` 0x3215a530 has the callers 3232caf0 x2, 32322b10, 323321f0, and other callers using different maps (+0xd4, +0x74; 3244adb0 not checked)):
- **0x3232caf0** `IsChainQuestAvailable(chainID)` (1140 B, sole caller 3232ef70) does the keyed lookup.
- **0x3232ef70** (254 B, 4 callers: RefreshDailyQuest 3232f830, CreateUnitQuest 32331830, RemoveUnitQuest 32331fb0, SetUnitQuest 32336e80) iterates the whole map @3232ef8a..3232f057 and rebuilds the derived map this+0x10c (chainID -> bool, default true; false when 0x323229d0(chain) says a quest of the chain is in progress, or 3232caf0 returns 0). It first calls `31d06e90` on +0x10c (presumed clear).
- The derived flag is read by **0x32322ad0** (find in +0x10c; missing -> false; only caller 323d5650, the quest-list UI: state 7 when flag==0 and the quest is not in progress) and by **0x323357c0** GetAvailableQuest @32335dd9-32335e3b (drops a quest when its chain (templet+0x278) has flag 0). Cleared in 0x32331dd0 (ClearUnitQuest).
- Writers of the chain data: 0x32322b10 (config), 0x323321f0 (loader), 0x32331fb0 (RemoveUnitQuest with bComplete: inserts (questID, `GetServerCurrentTime()`) into the chain completion map this+0x11c), 0x32336e80 SetUnitQuest (builds the same map from the completed list; includes the weekly/monthly expiry), 0x3232de60 (removes a quest from the +0x11c list; skipped for QT 10).

Rule in 0x3232caf0 (decoded from disassembly 3232caf0-3232cf50; returns AL):
1. node = chain map[chainID]; no node or `AllowFewDays` (node+0x20) <= 0 -> true (available).
2. T = latest completion time64 (list node +0x18/+0x1c) among entries of this+0x11c[chainID] whose templet exists, has server group -1 or == g_pMain+0x120, and templet+0x278 == chainID. No entries -> T = 0.
3. now = `GetServerCurrentTime()` (31edbdb0).
4. **Test branch** (g_pMain+0xa18 enable-code map has key 0x10 with byte +0x10 == 1, and node+0x24 `TestMinute` > 0): available iff `T + TestMinute*60 <= now`.
5. **Normal branch**: `D = T + AllowFewDays*86400` (CTimeSpan(days)); take the local calendar day of D, build mktime64(year, month, mday, 0:0:0); available iff that midnight `<= now`. So the chain unlocks at local 00:00 of the day AllowFewDays days after the completion day (AllowFewDays=1 -> next midnight). A defensive else-branch (localtime fails) uses midnight(T) + (AllowFewDays-1) days.
6. Additionally 0x323229d0 blocks the chain while any in-progress quest has templet+0x278 == chainID.

## 2. Handlers of QRT_WEEK(3) / QRT_MONTH(4) / QRT_CHAIN_REPEAT(5) / QT 8, 9, 10

Repeat type is templet+0x34 (piVar[0xd]). Tests found in the range: `==1` (3232f830, 32336e80), `==2` (32331ca0, 32337320, 323357c0), `==3` and `==4` only in **0x32336e80** SetUnitQuest. **No function in the range tests repeat type 5**; the chain rule is keyed on templet+0x278 != 0, not on repeat type (0x323321f0 stores the type unvalidated).
- Daily (QRT_DAY=1) **0x32321ee0**: returns local midnight of server time; completed records before it are skipped in SetUnitQuest; 0x3232f830 purges type 1 at local midnight (caller 31ef7970 runs when the day changes). Same value as March US (`HOUR_TO_REFRESH_DAILY_QUEST = 0`, X2Define.h:198).
- Weekly (3) **0x32322050**(completeDate64) -> bool "expired": d = days from the completion weekday to the next Wednesday (`tm_wday+1 < 4 ? 4-(wday+1) : 11-(wday+1)`, completed on Wednesday -> 7); expired iff `midnight(completeDate) + d*86400 <= GetServerCurrentTime64()`. Reset time = Wednesday 00:00 local.
- Monthly (4) **0x32321d10**(completeDate64) -> bool "expired": expired iff mktime(1st of the next month 00:00 local, December wraps to January +1 year) `<= GetServerCurrentTime64()`.
- Weekly/monthly are evaluated only in SetUnitQuest (login, 3 callers: 31f276b0, 324112e0, 32563270). 0x3232f830 does not purge types 3/4 at midnight.
- QT 8 (GUILD): **3232fbd0** GetStartQuestList lists it only if `**(g_pData+0x2ec) != 0`; **323357c0** removes type-8 quests in its non-repeat branch (`if (templet[2] == 8)` after the completed/random-group tests); **32330250** carries `m_iGuildCoopQuestRewardID`; loader flag m_bIsGuild at templet+0x271 (no reader in range).
- QT 9 (CHANGE_TRANSCENDENTAL_CLASS): **3232e510** IsForbiddenQuest gates (code event 0x920 with job record [0]==0xc; 0x921 with [1]==0x87; 0x923 with [1]==0x88); **3232fbd0** lists type 2/9 quests at any NPC that has a start NPC; **32330250** reads `m_eChangeUnitClass` when type==9; **32336310** (caller 3245abd0, CX2StateMenu class-up button) picks type 2 for class step 1-2 and type 9 for step 3 on NPC 0x2714; **32344ab0** class-up notice (type 2/9).
- QT 10 (STORY): **323321f0** @32335511 indexes quests of type 10 by templet+0x1b4 into the map at this+0xec (no reader in range, reader unresolved); **3232de60** excludes type 10 when removing a quest from the chain list.
- QT 5 is a plain EPIC test in 3232d290/d560/d830/db00, 3232fbd0.

## 3. Per-SQT completion check and progress string

- **0x323225e0** = `SubQuestInst::IsComplete` (March X2QuestManager.cpp:3203). Inventory count (`32869b80`) vs the clear-condition count at SubQuestTemplet+0x3c for SQT 3/4; everything else uses the server flag at inst+6. Inlined copies in 0x3232c640 (QuestInst::IsComplete :3146), 0x3232c750 (:3158), 0x3232c840 (:3185), 0x3232d290 family. Differences from March: June counts a collection quest done when `flag==1 || count>=need` (March ignores the flag), flag lives at +6, no SQT_POINT_COUNT branch, IsCompleteVisitQuest has no "must be last sub-quest" rule. The client never compares progress to counts for other SQTs; completion comes from the server flag.
- **0x32337db0** = `MakeSubQuestString` (March :3277), 13225 B, 19 callers. Signature (out wstring*, bAuto, SubQuestTemplet*, QuestInst*, bShowCount). Progress = `(short)subInst+4` (found by 0x32344f30) against the templet count (+0x34 for SQT 2, +0x6c, +0x74, +0x80, +0x100, +0x11c, +0x124, +0x3c for 3/4), printed with a wostringstream as `desc (cur/need)`. Switch labels run to 0x30: bodies for 0x25 (count +0x80), 0x27 (+0x11c), 0x2b (+0x124), 0x2c (shares 8/0x20/0x30 body, +0x80), 0x2d/0x2e/0x2f share the empty body of 0x1b/0x26; 0x28-0x2a have no label (default empty).
- Callers/users: 0x3233b290 = CheckItemCollection (:4916), 0x3232cff0 = LineBreakSubQuestString (:3240).

## 4. Account-quest container / date data

- 0x3232e070 (`AddAccountQuestDate`, keys AccountQuestID, DayOfWeek) writes `map<int,int>` at **this+0x9c** (end +0xa0) via 0x32257cf0 @3232e2c7.
- Reader in the range: **0x323222d0** (`GetQuestDayByQuestID`, March :1006; caller 323ca200). Also freed by 0x32336830 (dtor). No other function in the range touches +0x9c.
- The templet flag m_bIsAccount (templet+0x270) and m_bIsGuild (+0x271) are written by 0x323321f0 and not read anywhere in the range: readers unresolved (outside, UI).

## March gaps this implies (not in March today)
Chain system (+0xfc/+0x10c/+0x11c maps, 32322b10, 3232caf0, 3232ef70, 323229d0, 32322ad0, templet+0x278, hooks in Create/Remove/Refresh/Clear/SetUnitQuest/GetAvailableQuest/quest-list UI), QRT_WEEK/MONTH expiry (32322050, 32321d10), QT 8/9/10 handling above, story index +0xec, class-up quest helpers (32336310, 32344ab0), `3232de60` completed-quest reset.

## Unresolved
32322290 exact field, 32322f80 (0 callers), 32335f50 (post-SetUnitQuest rebuild into g_pData+0x570 via singleton 326ab090), 32337520 (non-QM this), the reader of the +0xec story map, readers of templet+0x270/+0x271, 32322490 / 32331c00 / 3232cf70 (0 callers).
