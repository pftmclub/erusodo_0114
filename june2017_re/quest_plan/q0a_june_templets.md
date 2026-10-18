# June 2017 quest templet parsers - transcription (quest-system Q0a)

Source: Ghidra (live) + SCR/gh/dis_*.s. All VAs are June `unpacked_x2.exe` addresses.
"use_va" = first instruction using the XOR-decoded key (from q_parser_keys.txt) or, for plain strings, the `PUSH <string>` address.
Offsets are relative to the object named in the section header. "abort" = jump to the failure tail (323353a8 for AddQuestTemplet; 32331625 for sub-quest).

## 0. Common machinery (read this first)

Reader object `KLuaManager`-like (`[0]=lua_State*`, `[4]=depth`, `[8]=L copy`). Built at function entry by `32fead40(L=[DAT_337676ec+4], 1, 0)`
(323321f0: 32332276-3233228d; 32330ff0: 32331093-323310a9). The ctor copies arg2 into `this[4]` (32fe9711 `MOV [EDI+4],EAX`, EAX=[ESP+0x10]=1),
so **depth starts at 1**. Helpers branch on depth:
- depth < 1: `lua_getfield(L, LUA_GLOBALSINDEX(-10002), key)` (32fe9fd0 first branch)
- depth >= 1: push key, `lua_gettable(L, -2)` i.e. the field of the table currently on top of the Lua stack (the table argument of the `AddXxx_LUA{...}` call).
  So every parsed key is a field of the single table argument; nested tables bump depth.
Result: **optional keys that are nil keep the C++ default listed below; keys are fields of the call's table, not globals** (inferred from ctor depth=1; the thunk 31e6e330 itself was not read - "unresolved: that the table is the top of stack at entry, likely").

| helper | VA | meaning | notes |
|---|---|---|---|
| OpenTable(name, 1) | 32fe9940 | get field `name`; if `lua_type==5` (table) then depth++ and return 1 | else pops, returns 0 |
| OpenTableAt(i, 1) | 32fe99b0 | same by integer index (array element) | |
| CloseTable(1) | 32fe9a10 | depth--, pop | |
| GetInt(key,&dst,1) | 32fe9fd0 | requires `isnumber` (32fe4d70: number or numeric string; Lua boolean fails) then `CVTTSD2SI` (32fe25f0) -> 32-bit store | used for int AND enum AND float-looking ints; **no float reader appears in these 4 functions** |
| GetBool(key,&dst,1) | 32fe9e40 | `isnumber` then `value != 0` -> byte store | a Lua `true` literal is NOT readable (counts as missing) |
| GetWString(key,&dst,1) | 32feae90 | `isstring/number` (32fe4db0), converts UTF-8 (CP 0xfde9) to wstring, then assigns via 31f54a10/31aa9200 | on miss caller assigns L"" (3344f1f4) |
| GetArrayInt(i,&dst,1) | 32fea4e0 | element `i` of the table at current depth, `isnumber`, int | loop idiom: `i=1; while(GetArrayInt(i++))` |
| log(code,msg) | 31bd0f50 | ECX=0x33766788 global logger | codes seen: 0x275f, 0x2760, 0x278d |

Container ops: 322aa950 = `map<int,..>::find` (key compared at node+0xc, head node = `[map+4]`); 31f8b860 = `vector<int>::push_back`; 3207b5d0 = `vector<Item16>::push_back`;
322a9500 + 32345580 = `std::set<int>::insert`; 3286eb20 + 32821f20 = make-node + `_Insert` for `map<int,Templet*>`.

---

## 1. 323321f0 - CX2QuestManager::AddQuestTemplet_LUA

Signature: `__thiscall`, ECX = quest manager (`this`, saved at 32332237 -> [ESP+0x44] at entry, [ESP+0x40] after), **no stack args**, plain `RET` (323357bc), no return value set.
Allocates `new(0x2a8)` templet (32332232) and constructs it with 3234a2e0 (3233225b); EDI = templet for the whole function. Templet size 0x2a8.

### 1.1 Key table (in execution order)

Required = failure writes the shown default then `abort` (log 0x275f "QuestTempletLoadFail_" at 32335433-32335493, templet destroyed 323354d7 and freed 323354dd).
Optional = the default is stored and parsing continues.

| # | key | use_va (read call) | dest = templet+ | helper | default / on-miss | notes |
|---|---|---|---|---|---|---|
| 1 | m_iServerGroupID | 323322d9 (323323ed) | +0x00 | GetInt | -1, continue | optional; stored by 323323f6 |
| 2 | m_iID | 32332425 (3233248d) | +0x04 | GetInt | 0, **abort** (32332496) | |
| 3 | m_eQuestType | 323324e1 (323325ba) | +0x08 | GetInt (temp [ESP+0x1c], then stored 323325da) | 0, **abort** (323325c3) | enum |
| 4 | m_eStartNPCID | 3233261b (32332704) | +0x0c | GetInt via temp (32332726) | 0, abort | |
| 5 | m_eEndNPCID | 32332762 (3233282b) | +0x10 | GetInt via temp (32332845) | 0, abort | |
| 6 | m_wstrTitle | 32332882 (3233294a) | +0x14 (wstring, 0x1c) | GetWString | abort (3233295a assigns L"") | |
| 7 | m_iFairLevel | 3233299f (32332a77) | +0x30 | GetInt | 0, **abort** (32332a80) | first read is required |
| 8 | m_eRepeatType | 32332ad3 (32332bbc) | +0x34 | GetInt via temp (32332bda) | 0, abort (32332bc5) | stored only; never compared in this function (no 3/4/5 test) |
| 9 | m_bIsPcBang | 32332c17 (32332cdf) | +0x38 | GetBool | 0, abort (32332ce8) | |
| 10 | m_bIsTimeEvent | 32332d33 (32332e2b) | +0x39 | GetBool | 0, abort (32332e34) | |
| 11 | m_wstrMainText | 32332e7f (32332f77) | +0x74 | GetWString | abort | |
| 12 | m_wstrThanksText | 32332fd6 (323330f1) | +0x90 | GetWString | abort | |
| 13 | m_wstrDissClearText | 3233315e (323332a9) | +0xac | GetWString | abort | |
| 14 | m_bIsAccount | 323332fe (323333d9) | +0x270 | GetBool | 0 (323333e2), continue | stored only (see 1.3) |
| 15 | m_bIsGuild | 3233341e (323334d9) | +0x271 | GetBool | 0 (323334e2), continue | stored only |
| 16 | m_iEpisodeGroupID | 3233352e (32333659) | +0x1b4 | GetInt | 0 (32333662) | |
| 17 | m_iPlayLevel | 323336a3 (3233377e) | +0x1d4 | GetInt | 0 (32333787) | |
| 18 | m_iFairLevel (2nd read, same dest) | 323337c8 (323338a0) | +0x30 | GetInt | 0 (323338a9) | redundant re-read |
| 19 | m_iAfterQuestID (a TABLE) | PUSH 3348cdbc @323338b1 (open 323338ba) | vector<int> at +0x214 | OpenTable + GetArrayInt loop 323338db/32333908, push_back 323338f6 if value > 0 | absent -> empty | closed 32333917 |
| 20 | m_wstrEpisodeGroupTitle | 32333984 (32333b0f) | +0x1b8 | GetWString | L"" (32333b1f), continue | |
| 21 | m_wstrStartScene | 32333b69 (32333c84) | +0x1d8 | GetWString | L"" | |
| 22 | m_wstrEndScene | 32333cdd (32333dd8) | +0x1f4 | GetWString | L"" | |
| 23 | m_bIsCommon | 32333e2c (32333ef7) | +0x210 | GetBool | **1** (32333f00) | optional, default true; stored only |
| 24 | m_iNextVillageID | 32333f48 (32334063) | +0x26c | GetInt | 0 (3233406c) | |
| - | (episode-group index) | 32334072-32334100 | this+0x58 map | see 1.4 | | no key |
| 25 | m_iRandomGroupID | 32334145 (32334264) | +0x268 | GetInt | 0 (3233426d) | used at 32335283 (1.4) |
| 26 | m_iChainID | 323342ad (32334368) | +0x278 | GetInt | 0 (32334371) | used at 32334377 (1.4) |
| 27 | m_vecAdditionalSceneQuestID (TABLE) | PUSH 3348cda0 @32334478 (open 32334481) | vector<int> at +0x27c | GetArrayInt 323344a2/323344d4, push_back 323344c2 if > 0 | empty | closed 323344e3 |
| 28 | QuestCondition (via 32323120) | call 323344f8 | embedded condition at +0xc8 | see section 2 | returns false -> **abort** (323344ff) | |
| - | (validation) | 3233450d-32334617 | | if `cond.m_eUnitType (+0xcc) != 0 && cond.unitClassSet.size (+0xd8) != 0`: log "QuestID: <id>" code 0x278d (3233453c-32334606); **no abort**, continues at 3233462b | | |
| 29 | CHANGE_JOB_TYPE (only if `+0x08 == 2`, CMP 3233462b) | 32334682 (3233478b) | temp, then see 1.4 | GetInt | on read fail the `JMP 323347a5` (32334796) skips the reload of EBX at 32334798 (Ghidra shows index 0) - **unresolved (stale register)** | accepted index 0..6 only (323347a0 `CMP EBX,7; JGE`) |
| 30 | m_vecSubQuest (TABLE) | PUSH 3348cd7c @323347e2 (open 323347eb) | vector<int> at +0x134 | GetArrayInt 3233480c/32334839, push_back 32334827 if > 0 | empty | closed 32334848 |
| 31 | m_vecSubQuest_Group (TABLE of TABLES) | PUSH 3348cd68 @3233484f (open 32334858, OpenTableAt 32334875) | map at +0x144 | see 1.4 | | group index 1.. ; entries == -1 skipped (323348a7) |
| 32 | Reward (via 32330250) | PUSH 3348cd60 @323349dd, call 323349e7 | embedded reward at +0x154 | see section 3 | returns false -> **abort** (323349ee) | args (luaMgr, "Reward", templet) |
| 33 | SelectReward (TABLE) | PUSH 3348cd50 @323349f6 (open 323349ff) | | optional | | |
| 33a | m_iSelectionCount | 32334a56 (32334b81) | +0x1a0 | GetInt | **1** (32334b8a) | inside SelectReward |
| 33b | m_vecSelectItem (TABLE of TABLES) | PUSH 3348cd40 @32334b92 (open 32334b9b, OpenTableAt 32334bb3/32335097) | vector<Item16> at +0x1a4 | per element below | | |
| 33c | m_iItemID | 32334c1b (32334cc8) | local item+0x0 | GetInt | 0 | |
| 33d | m_iQuantity | 32334d1e (32334deb) | local item+0x4 | GetInt | 0 | |
| 33e | m_iPeriod | 32334e34 (32334ee1) | local item+0x8 | GetInt | 0 | |
| 33f | m_sSocketOption | 32334f3d (32335049) | local item+0xc | GetInt | 0 | item pushed (3207b5d0 @3233507f, ECX=+0x1a4) only if ItemID != 0 and Quantity != 0 (32335065/3233506f) |
| 34 | SHOW_QUEST_ITEM (TABLE) | PUSH 3348cd30 @323350c0 | vector<int> at +0x224 | push_back 32335105 if > 0 | empty | |
| 35 | HIDE_QUEST_ITEM (TABLE) | PUSH 3348cd20 @3233512d | vector<int> at +0x234 | 32335172 if > 0 | | |
| 36 | HIDE_QUEST_SKILL (TABLE) | PUSH 3348cd0c @3233519a | vector<int> at +0x244 | 323351e2 if > 0 | | |
| 37 | HIDE_QUEST_BANK_GRADE (TABLE) | PUSH 3348ccf4 @3233520a | vector<int> at +0x254 | 3233525d if > 0 | | |
| 38 | m_fSelectBGM (**a wstring, not a float**) | 32335659 (32335738) | +0x28c | GetWString (32feae90) | L"" (32335748) | read AFTER the duplicate check, only reached on success path |

Templet fields never written by the parser (constructor-only): +0x3a..+0x73 region, +0x134.. other vector heads, +0x190 (reward item vector, see 3), etc.

Keys the task listed that do NOT occur in June's binary (neither plain strings nor in `xorstrings.tsv` / `q_xor_quest.tsv`): m_vecAcceptRewardItem, m_ePublisher, m_fRandomRate, m_iFairDungeonID, m_bRepeat, m_iEnableField (June has `m_iEnableBattleField` instead, in 32323120). m_eChangeUnitClass / m_iGuildCoopQuestRewardID appear only in 32330250.

### 1.2 (a) Which container the templet goes into

- Single insertion at the end: `ESI=[this+8]` head (32335383), `EDI=this+4` (32335391); `322aa950(this+4, &out, &templet->m_iID)` = find (3233539b); `CMP [EAX],ESI` (323353a0) equal to head => not present => continue (JZ 32335511).
  If a templet with that ID already exists => falls into the failure tail 323353a8 (log 0x275f "QuestTempletLoadFail_" id, title; destroy via 32349b20; free 323354dd).
- Insert: `3286eb20(&pair{id, templet})` + `32821f20` on `this+4` (32335773-3233577e). So **all normal/account/guild quests share ONE map `CX2QuestManager+0x04: map<int questID, QuestTemplet*>`** (head node at this+8).
- The function has **no parameter or flag** distinguishing normal / account / guild: signature is `(this)` only; RET with no immediate; no read of any member flag of `this` selects a different container.
- Bindings in 31d95080 (registration helper 31e75740(L, name, fnptr)): all four push the SAME fnptr 0x323321f0 with identical sequences (`PUSH fn; PUSH name; PUSH ESI; CALL 31e75740; ADD ESP,0xc`):
  - AddQuestTemplet_LUA        name decoded @31daeae9, PUSH 0x323321f0 @31daebfa, call 31daec2d
  - AddAccountQuestTemplet_LUA name decoded @31daef3f, PUSH 0x323321f0 @31daf0c0, call 31daf0f3
  - AddGuildQuestTemplet_LUA   name decoded @31daf1c1, PUSH 0x323321f0 @31daf322, call 31daf355
  - AddSubQuestTemplet_LUA     name @31daecee, PUSH 0x32330ff0 @31daee2e, call 31daee62
  => difference between the three quest bindings is **only the Lua-visible name**. Account/guild-ness comes from the data keys m_bIsAccount (+0x270) / m_bIsGuild (+0x271).
  Registration helper 31e75740 creates a Lua C closure with thunk 31e6e330 and stores the fn pointer in userdata (31e75760-31e75790); same thunk for all.

### 1.3 (b)/(c) Branches on quest type / flags

Complete list of comparisons against the templet's own fields in this function (grep of all `CMP ... [E.. + 0x8]` and uses of +0x34/+0x210/+0x270/+0x271):
- `m_eQuestType == 2` (CMP 3233462b): reads CHANGE_JOB_TYPE and registers in the change-job index (1.4).
- `m_eQuestType == 0xA` (CMP 32335515): inserts into the episode-group set map at this+0xec (1.4). **0xA is the only "QT_STORY"-like test.**
- 8 (QT_GUILD): **no test anywhere in 323321f0, 32330250, 32323120, 32330ff0.**
- 9 (QT_CHANGE_TRANSCENDENTAL_CLASS): tested only in the Reward reader (32330250 @323306b5, section 3).
- m_eRepeatType (+0x34): written once (32332bda), never read in these parsers -> values 3/4/5 are not special-cased here.
- m_bIsAccount (+0x270), m_bIsGuild (+0x271), m_bIsCommon (+0x210): stored only. No validation, no second container, no cross-check between them.

### 1.4 Secondary indexes built by the parser (all offsets in CX2QuestManager `this`)

| index | condition | container (this+) | key -> value | insts |
|---|---|---|---|---|
| episode-group titles | `+0x1b4 != 0 && +0x1c8(title.size) != 0` (3233407f/32334072) | +0x58, head at +0x5c | EpisodeGroupID -> wstring title; inserted only if key not found (CMP vs [this+0x5c] @323340a8) | find 3265c7f0, pair 31ef1250, insert 3207d560 (323340cd-323340e7) |
| chain | `m_iChainID != 0` (32334377) | +0xfc, head at +0x100 | ChainID -> set<int questID>; if absent create set containing templet.m_iID else insert into existing set (mapped value at node+0x10) | find 3215a530 @32334392, cmp [this+0x100] @3233439f, insert 32345580 @323343ec / @32334471, pair 32347540, insert 32349810 |
| random group | `m_iRandomGroupID (+0x268) > 0` (32335287 `CMP [ECX],0; JLE`) | +0x68, head at +0x6c | RandomGroupID -> vector<int questID>; unconditional push_back (the find loop at 323352d0-323352d9 result is unused: both exits reach 323352db) | find 32076c90 @323352a4, push_back 31f8b860 @323352df/32335313 |
| change-job | `m_eQuestType == 2` and (CHANGE_JOB_TYPE read failed OR 0 <= v < 7) (323347a0) | +0x44, head at +0x48 | key = first element of cond unit-class set (`[[EDI+0xd4]]+0xc`, 323347a5-323347ab, begins at head->_Left; NO emptiness check -> if set empty the key is read from the head node: **unresolved/hazard**); mapped value = array of 7 sets (stride 0x10, `SHL EBX,4` @323347bd); inserts templet.m_iID into set[CHANGE_JOB_TYPE] | operator[] 3234a690 @323347b8, insert 32345580 @323347db |
| sub-quest groups | per element != -1 | templet+0x144 (not manager) | groupIndex(1..) -> vector<int subQuestID>; `find 32076c90`, create-or-push_back | 323348c5-32334986 |
| story group | `m_eQuestType == 0xA` | +0xec, head at +0xf0 | key = templet+0x1b4 (EpisodeGroupID) -> set<int questID> (create-or-insert) | find 32076c90 @32335539, insert 32345580 @32335569 / @323355aa, pair 32828270, node 3265f750, insert 3282b3d0 |
| main | always | +0x04, head at +0x08 | questID -> QuestTemplet* | see 1.2 |

Order at the tail: random-group (32335283) -> duplicate check on main (3233537b-323353a0) -> story group (32335511) -> m_fSelectBGM read (32335659) -> main insert (32335773). So a duplicate ID aborts BEFORE the story-group index and before m_fSelectBGM; but the random-group, chain, episode-title, change-job indexes and the sub-quest-group map are already written (they are not rolled back on abort; the templet is freed leaving dangling IDs in those indexes - note for the port).

Failure tail 323353a8-3233550c: builds "QuestTempletLoadFail_" + id + (literal @3344cb5c, content unresolved) + title (+0x14), then 31bd0f50(0x275f, msg), destroys templet (32349b20) and frees it (32fe2300).

---

## 2. 32323120 - quest condition loader (`QuestCondition`)

Signature: `RET 8` (32323ac1); stack args `(KLuaManager* [ESI=arg1], Condition* [EBP=arg2])`; ECX unused; returns AL (bool).
Call site: 323344ec-323344f8: `MOV ECX,this; PUSH EDI+0xc8; PUSH lua; CALL`. Condition object = templet+0xc8.

| # | key | use_va (read call) | dest = cond+ | helper | default / on-miss | notes |
|---|---|---|---|---|---|---|
| 0 | QuestCondition (TABLE) | PUSH 3348cb7c @3232313c (OpenTable 32323143) | - | | **missing -> returns AL=0** (JNZ 32323aad, AL!=1) | closed at the end (32323aa8) |
| 1 | m_iEnableVillage (TABLE) | PUSH 3348cb68 @32323152 | set<int> at +0x38 | GetArrayInt 32323178/323231b0; insert 32345580 @323231a0 if value > 0 | empty | |
| 2 | m_iLevel | 323231f0 (32323265) | +0x00 | GetInt | -1 (32323270), **returns 0 immediately without closing the table** (32323287 RET) | required |
| 3 | m_iLimitLevel | 323232ca (32323383) | +0x68 | GetInt | -1 (3232338c) | optional |
| 4 | m_iEnableDungeon (TABLE) | PUSH 3348cb54 @32323394 | set<int> at +0x48 | insert 323233e2 if > 0 | empty | |
| 5 | m_iEnableBattleField (TABLE; June name, not "m_iEnableField") | PUSH 3348cb3c @32323406 | set<int> at +0x58 | insert 32323454 if > 0 | empty | |
| 6 | m_eUnitType | 323234bd (3232355d) | +0x04 | GetInt into temp [ESP+0x10] (init 0) then `-(ok) & temp` stored 3232356f | 0 | optional |
| 7 | m_eUnitClass (TABLE) | PUSH 3348cb2c @32323572 | see 3 below | GetArrayInt loop i=1..99 (`CMP EBX,0x64` 323235f6) | | element handling below |
| 8 | m_iConditionItemID | 32323652 (3232374c) | +0x1c | GetInt | -1 (32323755), **returns 0** | required |
| 9 | m_iConditionItemNum | 323237c5 (323238cc) | +0x20 | GetInt | -1 (32323755 shared), **returns 0** (JZ 323238d3) | required |
| 10 | m_iNotBeforeQuestID | 3232392c (32323a33) | +0x24 | GetInt | -1 (32323a3c) | optional |
| 11 | m_vecBeforeQuestID (TABLE) | PUSH 3348cb18 @32323a44 | vector<int> at +0x28 | `CMP value,0; JL` => values >= 0 pushed (32323a73/32323a78), push_back 31f8b860 @32323a82 | empty | note >= 0 here but > 0 elsewhere |
| end | close QuestCondition | 32323aa8 | | | returns AL=1 (from 32fe9a10) | |

### m_eUnitClass element handling (32323586-323235fb)
- value <= 0: ignored (323235ac `JLE`).
- value < 1000 (`CMP ECX,0x3e8; JL` 323235ae): inserted in set<int> at **cond+0x08** (ECX=EBP+8, 322a9500 @323235e2, 32345580 @323235f0).
- value >= 1000: **stored in a separate scalar `cond+0x18 = value % 1000`** (magic-divide 0x10624dd3 @323235b6-323235cf, store 323235cf). Not inserted into the set. Last such element wins. There is no comparison against exactly 1000/1001; any value >= 1000 takes this path. (So 1001 -> 1, 1000 -> 0.)
- The 0x18 slot is written only here; no other field name in this function is read for it.

Condition layout recovered: +0x00 level, +0x04 unitType, +0x08 set unitClass (head +0x0c, size +0x10), +0x18 transcendental/promotion class index, +0x1c itemID, +0x20 itemNum, +0x24 notBeforeQuestID, +0x28 vector beforeQuestID, +0x38 set village, +0x48 set dungeon, +0x58 set battlefield, +0x68 limitLevel.

---

## 3. 32330250 - Reward loader

Signature: `RET 0xc` (32330fb6); prologue 3233025e-32330268: stack args `(KLuaManager* [ESI], const char* tableName [EAX], QuestTemplet* [EBP])`. `if (EBP==0) return AL=0` (3233026c-32330282). ECX unused for state.
Only caller: 323349e7 (`tableName = "Reward"` 3348cd60). Reward block = **templet+0x154** (`LEA EDI,[EBP+0x154]` @32330289; saved [ESP+0x18]/[ESP+0x10]). Returns AL: 0 if table missing (JNZ 32330fa2 with AL!=1) or if m_bEnable missing; else 1 (from final CloseTable).
Table is opened by name at 32330296 (OpenTable). If `Reward` is missing the quest load aborts (323349ee).

| # | key | use_va (read call) | dest | helper | default / on-miss | notes |
|---|---|---|---|---|---|---|
| 1 | m_bEnable | 323302d5 (32330356) | reward+0x00 (=templet+0x154) | GetBool | **1 and return AL=0** (3233035f; table left open) | required |
| 2 | m_iEXP | 323303a4 (32330401) | reward+0x04 (+0x158) | GetInt | 0 (3233040a) | |
| 3 | m_iED | 32330439 (3233048a) | reward+0x08 (+0x15c) | GetInt | 0 (32330493) | |
| 4 | m_wstrName | 323304d0 (32330566) | reward+0x0c (+0x160, wstring) | GetWString | L"" (32330578) | |
| 5 | m_iBuff | 323305b2 (3233061d) | reward+0x28 (+0x17c) | GetInt | 0 (32330626) | |
| 6 | m_iSP | 32330658 (323306aa) | reward+0x2c (+0x180) | GetInt | 0 (323306b3) | |
| 7a | m_eChangeUnitClass #1 | 3233070e (32330809) | **reward+0x38 (+0x18c) = value % 1000** | GetInt into [ESP+0x14] | missing -> `ECX=0x3e8` (3233080e) so stored value is 1000 % 1000 = **0** | only when `templet.m_eQuestType == 9` (CMP [EBP+8],9 @323306b5; JNZ 32330840 else) |
| 7b | m_eChangeUnitClass #2 | 3233088e (32330989) | **reward+0x30 (+0x184) = raw value** (`-(ok) & v`, 32330991-3233099d) | GetInt | 0 | when quest type != 9 |
| 8 | m_iGuildCoopQuestRewardID | 32330a0b (32330b64) | **templet+0x274** (not in the reward block; `LEA EDI,[EBP+0x274]` 32330b3f) | GetInt | 0 (32330b6d) | stored only, no other use here |
| 9 | m_vecItem (TABLE of TABLES) | PUSH 3348cc54 @32330b71 (OpenTable 32330b78, OpenTableAt 32330b8e/32330f83) | vector<Item16> at reward+0x3c (= templet+0x190), push_back 3207b5d0 @32330f6f | | absent -> empty | per element: |
| 9a | m_iItemID | 32330be6 (32330c6d) | item+0x0 | GetInt | 0 | |
| 9b | m_iQuantity | 32330cb9 (32330d5a) | item+0x4 | GetInt | 0 | |
| 9c | m_iPeriod | 32330d9d (32330e24) | item+0x8 | GetInt | 0 | |
| 9d | m_sSocketOption | 32330e75 (32330f4a) | item+0xc | GetInt | 0 | pushed only if ItemID != 0 && Quantity != 0 (32330f57/32330f5d) |

### The two m_eChangeUnitClass reads (task item 3)
- The only branch separating them is `templet.m_eQuestType == 9` (32330250: CMP dword ptr [EBP+8],0x9 at 323306b5; type 9 -> falls into read #1 at 323306c3-32330809, others JNZ 32330840 -> read #2 at 3233084b-32330989). They are mutually exclusive (read #1 ends with JMP 323309a0 at 3233083b).
- **No comparison against 1000/1001 (0x3e8/0x3e9) as separate values.** In #1 the only 0x3e8 is the missing-key default (3233080e) and the `% 1000` divisor (magic 0x10624dd3 @3233081b). QR_NONE/QR_TRANSCENDENTAL_CLASS-style handling is therefore not here; both are collapsed by `% 1000` (1000 -> 0, 1001 -> 1).
- Second destination offset: yes, +0x38 (type 9, modded) vs +0x30 (other types, raw). The parser writes nothing at +0x34.
- m_iGuildCoopQuestRewardID: templet+0x274 (outside the reward block), default 0.

---

## 4. 32330ff0 - CX2QuestManager::AddSubQuestTemplet_LUA

Signature: `__thiscall`, ECX saved to EBP (32331032) = quest manager; no stack args; plain `RET` (32331820); returns AL: 1 success (323317f7), 0 failure (323317ad). SubQuest templet size 0x144 (`new(0x144)` 3233102d), constructed inline: wstring at +4 emptied (32331059-32331063), embedded clear-condition object at +0x24 built by 32346bc0 (32331072), `[+0]=-1`, `[+0x20]=0`, `[+0x140]=1` (32331077-32331080).

| # | key | use_va (read call) | dest = subTemplet+ | helper | default / on-miss | notes |
|---|---|---|---|---|---|---|
| 1 | m_iID | 323310e1 (32331145) | +0x00 | GetInt | 0 (3233114e), **error path** 32331625 | initial value -1 |
| 2 | m_wstrDescription | 3233119b (323312c4) | +0x04 (wstring, size @+0x14, cap @+0x18) | GetWString | L"" (323312d5), error path | |
| 3 | m_eClearType | 32331321 (323313fe) | +0x20 | GetInt via temp [ESP+0xf4], stored 32331416 | 0 (32331407), error path | enum, indexes the 48-entry jump table in 32323ad0 (`[EBP+0x20]-1`, `CMP EAX,0x2f; JA`, `JMP [EAX*4+0x3232c57c]` @32323b0e-32323b1d) |
| 4 | m_bAutomaticDescription | 32331476 (32331602) | +0x140 (bool) | GetBool | 1 (3233160b), error path | note: miss => true AND error path |
| 5 | (call) 32323ad0 | 32331616-32331618 | `thiscall(ECX=this)`, stack: (luaMgr [EDX=ESP+0x1c], subTemplet [EDI]) @32331610-32331618 | | returns AL | see below |

### After the call to 32323ad0 (323317 area)
- 32323ad0 opens table `m_ClearCondition` (PUSH 3348cc04 @32323af6, OpenTable 32323b01); `JNZ 3232c577` => **returns AL=0 if missing**; otherwise switches on `m_eClearType-1` over 48 cases (>0x2f -> falls to 3232c537: close table, **return 0**); each case reads its own keys into the embedded condition at subTemplet+0x24.. and returns 1 (3232c573) or 0 on a required-key miss (3232c537-3232c540).
  Key list (names only, dests not transcribed here): m_eTalkNPCID m_eDungeonMode m_bUpperDifficulty m_iKillNum m_cDifficulty m_iCollectionItemID m_iCollectionItemNum m_fQuestItemDropRate m_iDungeonClearTime m_eDungeonRank m_iDungeonClearCount m_iDungeonDamage m_ePVPType m_iPVPPlay m_ePvpTournamentType m_iPVPWin m_iPVPKill m_ePvpChannelClass m_iEnchantLevel m_iSocketCount m_eAttribCountType m_eAttribEnchant0..3 m_iResolveCount m_iSpUseCount m_iSkillID m_iFindNPCID m_iCharLevel m_iUseSkillCount m_iLearnNewSkillCount m_iCollectionHonorPointCount m_bIsGuildJoin m_iExchangeCount m_ePvpRank + plain: m_eKillNPCID(3348cbd8) m_eFieldID(3348cbe8) m_eVillageID(3348cbf4) m_listUseItemID(3348cbc8) m_iUseItemCnt(3348cbb8) m_vecSkillID(3348cba8) m_vecExchangeSourceItem_ID(3348cb8c) (full per-case transcription is outside Q0a scope).
- Caller handling of the return value: `CALL 32323ad0` @32331618, `TEST AL,AL`, `JNZ 323317b1` @3233161f.
  - **AL == 0 (failure)**: falls into 32331625: builds message "SubQuestTempletLoadFail_" (3348cc60) + id (`[EDI]`) + literal @3344cb5c (content unresolved) + description (EDI+4) (323316a4-323316d6), then **`31bd0f50(0x2760, msg)`** @3233170f (ECX=0x33766788); then destroys the embedded condition (32346020 @3233175b on EDI+0x24), frees the description string and the templet (32fe2300 @32331780), returns AL=0 (323317ad). No dup check, nothing inserted.
  - **AL != 0 (success)** @323317b1: `EAX=[EDI]` (id); builds pair {id, templet} on stack ([ESP+0xfc]=id, [ESP+0x100]=templet) and inserts via `3286eb20` (ECX = EBP+0x14) @323317cd then `32821f20` (ECX = EBP+0x14) @323317de. So **sub-quests live in `CX2QuestManager+0x14: map<int subQuestID, SubQuestTemplet*>`** (head at this+0x18), separate from the quest map at this+0x04. No duplicate-ID test (a duplicate insert would be rejected by the map and leak the new templet - same code shape as the quest map but without the preceding find). Returns AL=1 (323317f7).
- No chain/group index and no type-dependent branching in the sub-quest parser itself besides the jump table in 32323ad0.

---

## 5. Unresolved / hazards (explicit)

1. Whether the argument table is the top of the Lua stack at entry: inferred from depth=1 in the ctor (32fead61 -> 32fe9711) and the depth>=1 branch of the helpers; the generic thunk 31e6e330 was not decompiled.
2. CHANGE_JOB_TYPE read miss (32334796 -> 323347a5): EBX not reloaded; index used is whatever EBX held (Ghidra's decompile shows 0). Treat as "unresolved; use 0".
3. Change-job index: `[[cond+0xc]]+0xc` read with no empty-set guard (323347a5-323347ab).
4. String literal at 3344cb5c (separator in the load-fail messages) not decoded.
5. Container types for this+0xec (story group) vs this+0xfc (chain): both store set<int> values via 32345580, but the node/pair helpers differ (32828270/3265f750/3282b3d0 vs 32347540/32347e90/32349810), i.e. different template instantiations; treat as map<int, set<int>> each, exact value types unconfirmed.
6. 32323ad0 per-case destinations were not transcribed (only its entry/exit/return contract).
7. Early-return paths in 32323120 (m_iLevel miss) and 32330250 (m_bEnable miss) do not close the Lua table (depth stays raised); harmless because the caller aborts and destroys the reader.
