# June 2017 client: clear-condition loader FUN_32323ad0 (CX2QuestManager, March LoadClearCondition equivalent)

Source: `gh/dis_32323ad0.s` (disassembly; decompile fails), helper bodies via live Ghidra decompile, jump-table targets via Ghidra `xrefs_from` on each table slot.
All addresses are June client VAs. "use_va" = the first instruction that loads the XOR-built key (from q_parser_keys.txt "== 32323ad0"), or the PUSH of the plain key string; "call_va" = the CALL of the read helper (or open-table call).

## Function frame
- `32323ad0`: `SUB ESP,0x740`; two stack args, `RET 8` (3232c55a). arg1 -> ESI (32323aed) = Lua wrapper object (`[ESI]` = lua_State, `[ESI+4]` = table depth). arg2 -> EBP (32323ae5) = clear-condition struct being filled.
- Entry (32323af4-b08): `OpenTable("m_ClearCondition")` (plain key 3348cc04, helper 32fe9940). Fails -> return 0 (3232c577).
- Switch value is the struct's own type field: `MOV EAX,[EBP+0x20]; DEC EAX` (32323b0e-b11), i.e. struct+0x20 = clear type (SQT), assigned before this call by the caller.
- Nothing is read before the switch except the table open, and nothing after it: every case ends in its own CloseTable + return. So there are NO keys common to all types.
- There is no logging anywhere in the function (calls made: 32fe9940, 32fe9a10, 32fe9fd0, 32fe9e40, 32fea370, 32fea4e0, 326a6db0, 31f8b860, 32344f00, and the stack-cookie check 32fe245c only).

## Helper identification (Ghidra decompile)
| addr | meaning |
|---|---|
| 32fe9940 `this,key,1` | OpenTable(key): push field `key` of the current table; succeeds only if Lua type == table (type 5, via 32fe4c40); depth++ ; returns bool |
| 32fe9a10 | CloseTable(): depth--, returns 1 (AL=1) if depth>0 |
| 32fe9fd0 `this,key,&int,1` | GetInt(key,out): gated by lua isnumber (32fe4d70: number OR numeric string); on success `*out = (int)lua_tonumber` (32fe25f0 float->int round); returns bool; out untouched on failure |
| 32fe9e40 | GetBool(key,out): same isnumber gate, `*out = (value != 0)`; a Lua boolean value is NOT accepted (isnumber false) |
| 32fea370 | GetFloat(key,out) |
| 32fea4e0 `this,idx,&int,1` | GetIntByIndex(idx,out) for the currently open array table; returns bool (false when element missing / not a number) |
| 326a6db0 `container,&tmpOut,&val` | std::set<int>::insert(val) (via 322a9500 node alloc + 32345580 red-black insert, key at node+0xc, unique) |
| 31f8b860 `vec,&val` | std::vector<int>::push_back (begin,end,cap at +0,+4,+8) |
| 32344f00 `vec,idx,val` | `if (0<=idx<size && 0<=val<8) vec[idx]=val` (bool) |

## Switch
- Bounds check: `CMP EAX,0x2f; JA 3232c537` (32323b14, 32323b17): type-1 above 47 (type 0, or type > 48) -> default (fail).
- Dispatch: `JMP dword ptr [EAX*4 + 0x3232c57c]` at 32323b1d. Table at 3232c57c, 48 dword entries (type 1..48), range 3232c57c-3232c63b. Entry = table + 4*(type-1).

| type | SQT | case start | | type | SQT | case start |
|---|---|---|---|---|---|---|
| 1 | NPC_TALK | 32323b24 | | 25 | VISIT_FIELD | 3232a67f |
| 2 | NPC_HUNT | 32323cd8 | | 26 | VISIT_DUNGEON | 3232a6fd |
| 3 | ITEM_COLLECTION | 32324383 | | 27 | FIND_NPC | 3232a8c6 |
| 4 | QUEST_ITEM_COLLECTION | 323246e6 | | 28 | PVP_PLAY_ARRANGE | 3232ac31 |
| 5 | DUNGEON_TIME | 32325186 | | 29, 30 | PVP_TAG_COUNT, HYPER_MODE_USE | 3232c537 (default fail) |
| 6 | DUNGEON_RANK | 323257dc | | 31 | CHAR_LEVEL_UP | 3232ad24 |
| 7 | DUNGEON_DAMAGE | 32325fab | | 32 | SUITABLE_LEVEL_DUNGEON_CLEAR | 323265cf (= case 8) |
| 8 | DUNGEON_CLEAR_COUNT | 323265cf | | 33 | PVP_HERO_NPC_KILL | 3232ae43 |
| 9 | PVP_PLAY | 3232714a | | 34 | PVP_HERO_NPC_PLAY | 3232b211 |
| 10 | PVP_WIN | 3232753c | | 35 | POINT_COUNT | 3232c537 (default fail) |
| 11 | PVP_KILL | 32327916 | | 36 | LEARN_NEW_SKILL | 3232baac |
| 12 | WITH_DIF_SERV_USER | 32327cef | | 37 | PVE_WIN | 32326c38 |
| 13 | ITEM_ENCHANT | 32328365 | | 38 | GUILD_JOIN | 3232bf50 |
| 14 | ITEM_SOCKET | 32328660 | | 39 | GUILD_HONOR_POINT_COLLECTION | 3232bcfc |
| 15 | ITEM_ATTRIB | 32328942 | | 40, 41, 42 | PLAYER_WITH_DUNGEON_CLEAR, PGT_1VS1_CHALLENGE_MODE, PGT_WINNER | 3232c537 (default fail) |
| 16 | ITEM_RESOLVE | 323292fc | | 43 | ITEM_EXCHANGE | 3232c0a2 |
| 17 | ITEM_EQUIP_DUNGEON_CLEAR | 323295f3 | | 44 | TOGETHER_SUBQUEST_CLEAR | 3232c268 |
| 18 | USE_SKILL_POINT | 32329dfe | | 45 | PVP_RANK | 3232c428 |
| 19, 20 | FEED_PET, USER_DIE | 3232c537 (default fail) | | 46 | IMMEDIATELY_CLEAR | 3232c56a (true, no keys) |
| 21 | PVP_NPC_HUNT | 3232a047 | | 47 | PLAY_TOGETHER_WITH_TITLE | 3232c56a (true, no keys) |
| 22 | ITEM_USE | 3232b5df | | 48 | HAS_EXP_DUNGEON_CLEAR | 323265cf (= case 8) |
| 23 | SKILL_USE | 3232b8bd | | 24 | VISIT_VILLAGE | 3232a601 |

(SQT names for 12/17/21/33/34/46 etc. are from june_src_fixed/data036_kom/Enum.src.lua lines 7075-7124.)

## Clear-condition struct offsets used (June)
+0x20 type (read, not written here); +0x24 set KillNPCID; +0x34 KillNum; +0x38 CollectionItemID; +0x3c CollectionItemNum; +0x40 set DungeonID; +0x50 DungeonMode; +0x54 UpperDifficulty(byte); +0x58 DungeonRank; +0x5c DungeonClearTime; +0x60 TalkNPCID; +0x68 PVPType; +0x6c PVPPlay; +0x70 PVPWin; +0x74 PVPKill; +0x78 float QuestItemDropRate; +0x7c DungeonDamage; +0x80 DungeonClearCount; +0x84 EnchantLevel; +0x88 SocketCount; +0x8c vector<int> AttribEnchant[4]; +0x9c AttribCountType; +0xa0 ResolveCount; +0xa4 SkillID; +0xa8 SpUseCount; +0xac PvpChannelClass; +0xb0 set VillageID; +0xc0 set FieldID; +0xd0 FindNPCID; +0xd4 CharLevel; +0xd8 vector UseItemID; +0xe8 UseItemCnt; +0xec UseSkillCount; +0xf0 set SkillID (SKILL_USE); +0x100 LearnNewSkillCount; +0x104 set SkillID (LEARN_NEW_SKILL); +0x118 PvpRank; +0x11c HonorPointCount; +0x120 IsGuildJoin(byte); +0x124 ExchangeCount; +0x128 vector ExchangeSourceItem_ID; +0x138 PvpTournamentType; +0x13c Difficulty.

Legend for the table: "REQUIRED" = helper failure jumps to a fail stub and the function returns false (struct field left at the stub's value); "OPTIONAL" = failure is ignored or defaulted and loading continues. Fail stubs: 3232c537 = CloseTable, return false; 3232c56a = CloseTable, return true; 3232434c = [+0x13c]=-1 then CloseTable, return true; 323246db = [EDI]=0 then false; 3232420b = [EDI]=EBX(0) then false; 3232c41d = [EDI]=-1 then false; 32324aa0 / 32324029 = byte[EDI]=0 then false; 32327253 = [+0x68]=0 then false; 32327cc3 = [+0xac]=-1 then false. Every `JMP 3232c542` success path sets AL=1 first (verified for all of them).

## Key table

| case | sqt | key | offset | helper | default / on-missing | use_va | call_va |
|---|---|---|---|---|---|---|---|
| 1 | NPC_TALK | m_eTalkNPCID | +0x60 | int 32fe9fd0 | temp 0; missing -> [+0x60]=0, RETURN FALSE (32323c4b-4e) | 32323b67 | 32323c42 |
| 1 | NPC_TALK | m_eVillageID | +0xb0 | OpenTable 32fe9940 (plain key @3348cbf4) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (32323c90 CMP,32323c94 JLE); table absent: skipped, no failure | 3348cbf4 | open 32323c66, insert 32323ca6 |
| 2 | NPC_HUNT | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (32323d0b JL); table absent: skipped, no failure | 33457ed8 | open 32323ce1, insert 32323d1e |
| 2 |  | m_eDungeonMode | +0x50 | int 32fe9fd0 | optional; temp 0, value masked by call success (NEG/SBB/AND) -> missing = 0 (store 32323e9d) | 32323d8b | 32323e85 |
| 2 |  | m_bUpperDifficulty | +0x54 | bool 32fe9e40 (gated by isnumber 32fe4d70) | REQUIRED: missing/non-number -> byte=0, RETURN FALSE (32324025 TEST,32324027 JNZ -> 32324029) | 32323ee9 | 32324020 |
| 2 | NPC_HUNT | m_eFieldID | +0xc0 | OpenTable 32fe9940 (plain key @3348cbe8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (32324060 CMP,32324064 JLE); table absent: skipped, no failure | 3348cbe8 | open 32324039, insert 32324076 |
| 2 | NPC_HUNT | m_eKillNPCID | +0x24 | OpenTable 32fe9940 (plain key @3348cbd8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (323240d7 CMP,323240d9 JLE); table absent: skipped, no failure | 3348cbd8 | open 323240a1, insert 323240ec |
| 2 |  | m_iKillNum | +0x34 | int 32fe9fd0 | REQUIRED: missing -> [+0x34]=0, RETURN FALSE (32324209 -> 3232420b) | 3232414b | 32324202 |
| 2 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 32324259 | 32324343 |
| 3 | ITEM_COLLECTION | m_iCollectionItemID | +0x38 | int 32fe9fd0 | REQUIRED: missing -> [+0x38]=0, RETURN FALSE (32324523 JZ 323246db) | 323243d6 | 3232451c |
| 3 |  | m_iCollectionItemNum | +0x3c | int 32fe9fd0 (shared stub 323246be-323246d3) | REQUIRED: missing -> [+0x3c]=0, RETURN FALSE (323246db) | 32324578 | 323246ce |
| 4 | QUEST_ITEM_COLLECTION | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (32324719 JL); table absent: skipped, no failure | 33457ed8 | open 323246ef, insert 3232472c |
| 4 | QUEST_ITEM_COLLECTION | m_eFieldID | +0xc0 | OpenTable 32fe9940 (plain key @3348cbe8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (32324780 CMP,32324784 JLE); table absent: skipped, no failure | 3348cbe8 | open 32324757, insert 32324796 |
| 4 |  | m_eDungeonMode | +0x50 | int 32fe9fd0 | optional; temp 0, value masked by call success (NEG/SBB/AND) -> missing = 0 (store 32324910) | 32324803 | 323248fd |
| 4 |  | m_bUpperDifficulty | +0x54 | bool 32fe9e40 (gated by isnumber 32fe4d70) | REQUIRED: missing/non-number -> byte=0, RETURN FALSE (32324a9e -> 32324aa0) | 32324961 | 32324a97 |
| 4 | QUEST_ITEM_COLLECTION | m_eKillNPCID | +0x24 | OpenTable 32fe9940 (plain key @3348cbd8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (32324ae4 TEST,32324ae6 JLE); table absent: skipped, no failure | 3348cbd8 | open 32324ab1, insert 32324af9 |
| 4 |  | m_iCollectionItemID | +0x38 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (32324cbb JZ 323246db) | 32324b6e | 32324cb4 |
| 4 |  | m_iCollectionItemNum | +0x3c | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (32324e6d JZ 323246db) | 32324d10 | 32324e66 |
| 4 |  | m_fQuestItemDropRate | +0x78 | float 32fea370 | REQUIRED: missing -> 0.0f (FLDZ/FSTP 32325022-24), RETURN FALSE | 32324ec2 | 32325019 |
| 4 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 32325072 | 3232515c |
| 5 | DUNGEON_TIME | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (323251b9 JL); table absent: skipped, no failure | 33457ed8 | open 3232518f, insert 323251c5 |
| 5 |  | m_eDungeonMode | +0x50 | int 32fe9fd0 | optional; temp 0, value masked by call success (NEG/SBB/AND) -> missing = 0 (store 32325346) | 32325239 | 32325333 |
| 5 |  | m_bUpperDifficulty | +0x54 | bool 32fe9e40 (gated by isnumber 32fe4d70) | REQUIRED: missing/non-number -> byte=0, RETURN FALSE (323254d4 -> 32324aa0) | 32325397 | 323254cd |
| 5 |  | m_iDungeonClearTime | +0x5c | int 32fe9fd0 | REQUIRED: missing -> [+0x5c]=0, RETURN FALSE (3232567b JZ 3232420b) | 3232552d | 32325674 |
| 5 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 323256c8 | 323257b2 |
| 6 | DUNGEON_RANK | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (32325816 JL); table absent: skipped, no failure | 33457ed8 | open 323257e5, insert 32325822 |
| 6 |  | m_eDungeonMode | +0x50 | int 32fe9fd0 | optional; temp 0, value masked by call success (NEG/SBB/AND) -> missing = 0 (store 323259a6) | 32325899 | 32325993 |
| 6 |  | m_bUpperDifficulty | +0x54 | bool 32fe9e40 (gated by isnumber 32fe4d70) | REQUIRED: missing/non-number -> byte=0, RETURN FALSE (32325b35 -> 32324029) | 323259f7 | 32325b2e |
| 6 |  | m_eDungeonRank | +0x58 | int 32fe9fd0 | REQUIRED: missing -> [+0x58]=0, RETURN FALSE (32325c8a-8d) | 32325b86 | 32325c81 |
| 6 |  | m_iDungeonClearCount | +0x80 | int 32fe9fd0 | OPTIONAL here: missing -> [+0x80]=0 (32325e4e), continues | 32325ceb | 32325e45 |
| 6 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 32325e97 | 32325f81 |
| 7 | DUNGEON_DAMAGE | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (32325fe6 JS); table absent: skipped, no failure | 33457ed8 | open 32325fb4, insert 32325ff5 |
| 7 |  | m_eDungeonMode | +0x50 | int 32fe9fd0 | optional; temp 0, value masked by call success (NEG/SBB/AND) -> missing = 0 (store 32326177) | 3232606a | 32326164 |
| 7 |  | m_bUpperDifficulty | +0x54 | bool 32fe9e40 (gated by isnumber 32fe4d70) | REQUIRED: missing/non-number -> byte=0, RETURN FALSE (32326305 -> 32324aa0) | 323261c8 | 323262fe |
| 7 |  | m_iDungeonDamage | +0x7c | int 32fe9fd0 | REQUIRED: missing -> [+0x7c]=-1, RETURN FALSE (3232646e JZ 3232c41d) | 32326350 | 32326467 |
| 7 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 323264bb | 323265a5 |
| 8 | DUNGEON_CLEAR_COUNT | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (32326606 JL); table absent: skipped, no failure | 33457ed8 | open 323265d8, insert 32326615 |
| 8 |  | m_eDungeonMode | +0x50 | int 32fe9fd0 | optional; temp 0, value masked by call success (NEG/SBB/AND) -> missing = 0 (store 32326793) | 32326686 | 32326780 |
| 8 |  | m_bUpperDifficulty | +0x54 | bool 32fe9e40 (gated by isnumber 32fe4d70) | REQUIRED: missing/non-number -> byte=0, RETURN FALSE (32326921 -> 32324aa0) | 323267e4 | 3232691a |
| 8 |  | m_iDungeonClearCount | +0x80 | int 32fe9fd0 | REQUIRED: missing -> [+0x80]=-1, RETURN FALSE (32326ad7 JZ 3232c41d) | 32326976 | 32326ad0 |
| 8 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 32326b24 | 32326c0e |
| 32, 48 | SUITABLE_LEVEL_DUNGEON_CLEAR / HAS_EXP_DUNGEON_CLEAR | (identical to case 8: table entries 3232c5f8 and 3232c638 both -> 323265cf) |  |  | same keys, offsets and defaults as case 8 | 323265cf |  |
| 9 | PVP_PLAY | m_ePVPType | +0x68 | int 32fe9fd0 | REQUIRED: missing -> [+0x68]=0, RETURN FALSE (32327253) | 3232718f | 3232724a |
| 9 |  | m_iPVPPlay | +0x6c | int 32fe9fd0 | REQUIRED: missing -> [+0x6c]=-1, RETURN FALSE (3232735d JZ 3232c41d) | 3232729f | 32327356 |
| 9 |  | m_ePvpTournamentType | +0x138 | int 32fe9fd0 | optional; missing = 0 (mask, store 3232752a), RETURN TRUE | 323273b9 | 32327513 |
| 10 | PVP_WIN | m_ePVPType | +0x68 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (32327643 JZ 32327253) | 32327581 | 3232763c |
| 10 |  | m_iPVPWin | +0x70 | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (32327737 JZ 3232c41d) | 32327689 | 32327730 |
| 10 |  | m_ePvpTournamentType | +0x138 | int 32fe9fd0 | optional; missing = 0 (store 32327902) | 32327793 | 323278ed |
| 11 | PVP_KILL | m_ePVPType | +0x68 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (32327a1d JZ 32327253) | 3232795b | 32327a16 |
| 11 |  | m_iPVPKill | +0x74 | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (32327b25 JZ 3232c41d) | 32327a67 | 32327b1e |
| 11 |  | m_ePvpChannelClass | +0xac | int 32fe9fd0 | REQUIRED: missing -> [+0xac]=-1, RETURN FALSE (32327cc3) | 32327b80 | 32327cba |
| 12 | WITH_DIF_SERV_USER | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (32327d26 JL); table absent: skipped, no failure | 33457ed8 | open 32327cf8, insert 32327d35 |
| 12 |  | m_eDungeonMode | +0x50 | int 32fe9fd0 | optional; temp 0, value masked by call success (NEG/SBB/AND) -> missing = 0 (store 32327eb7) | 32327da6 | 32327e9f |
| 12 |  | m_bUpperDifficulty | +0x54 | bool 32fe9e40 (gated by isnumber 32fe4d70) | REQUIRED: missing/non-number -> byte=0, RETURN FALSE (3232804e -> 32324aa0) | 32327f11 | 32328047 |
| 12 |  | m_iDungeonClearCount | +0x80 | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (32328204 JZ 3232c41d) | 323280a3 | 323281fd |
| 12 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 32328251 | 3232833b |
| 13 | ITEM_ENCHANT | m_iEnchantLevel | +0x84 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (323284be JZ 323246db) | 323283ae | 323284b7 |
| 13 |  | m_iCollectionItemID | +0x38 | int 32fe9fd0 (shared tail 3232a01e-3232a042) | OPTIONAL: missing -> [+0x38]=0 (3232a035), RETURN TRUE | 32328517 | 3232a024 (JMP 3232a022 @3232865b) |
| 14 | ITEM_SOCKET | m_iSocketCount | +0x88 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (323287a4 JZ 323246db) | 323286a4 | 3232879d |
| 14 |  | m_iCollectionItemID | +0x38 | int 32fe9fd0 (shared tail) | OPTIONAL: missing -> 0, RETURN TRUE | 323287fd | 3232a024 (JMP 3232a01e @3232893d) |
| 15 | ITEM_ATTRIB | m_eAttribCountType | +0x9c | int 32fe9fd0 | REQUIRED: missing -> [+0x9c]=0, RETURN FALSE (32328adc-e2) | 32328999 | 32328ad3 |
| 15 |  | m_iCollectionItemID | +0x38 | int 32fe9fd0 | OPTIONAL: missing -> 0 (32328ca1), continues | 32328b52 | 32328c98 |
| 15 |  | m_eAttribEnchant0 | +0x8c vector<int>[0] via setter call 32328e40 | int 32fe9fd0, then setter 32344f00(vec, idx=0, val) | OPTIONAL: missing -> val 0; setter stores only if 0<=val<8 and idx<size, else ignored | 32328cf9 | 32328e20 |
| 15 |  | m_eAttribEnchant1 | +0x8c vector<int>[1] via setter call 32328fcd | int 32fe9fd0, then setter 32344f00(vec, idx=1, val) | OPTIONAL: missing -> val 0; setter stores only if 0<=val<8 and idx<size, else ignored | 32328e8f | 32328fb7 |
| 15 |  | m_eAttribEnchant2 | +0x8c vector<int>[2] via setter call 3232915a | int 32fe9fd0, then setter 32344f00(vec, idx=2, val) | OPTIONAL: missing -> val 0; setter stores only if 0<=val<8 and idx<size, else ignored | 3232901c | 32329144 |
| 15 |  | m_eAttribEnchant3 | +0x8c vector<int>[3] via setter call 323292e5 | int 32fe9fd0, then setter 32344f00(vec, idx=3, val) | OPTIONAL: missing -> val 0; setter stores only if 0<=val<8 and idx<size, else ignored | 323291a9 | 323292d1 |
| 16 | ITEM_RESOLVE | m_iResolveCount | +0xa0 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (32329455 JZ 323246db) | 32329345 | 3232944e |
| 16 |  | m_iCollectionItemID | +0x38 | int 32fe9fd0 (shared tail) | OPTIONAL: missing -> 0, RETURN TRUE | 323294ae | 3232a024 (JMP 3232a01e @323295ee) |
| 17 | ITEM_EQUIP_DUNGEON_CLEAR | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (32329626 JL); table absent: skipped, no failure | 33457ed8 | open 323295fc, insert 32329639 |
| 17 |  | m_eDungeonMode | +0x50 | int 32fe9fd0 | optional; temp 0, value masked by call success (NEG/SBB/AND) -> missing = 0 (store 323297b8) | 323296a6 | 323297a0 |
| 17 |  | m_bUpperDifficulty | +0x54 | bool 32fe9e40 (gated by isnumber 32fe4d70) | REQUIRED: missing/non-number -> byte=0, RETURN FALSE (32329941 -> 32324aa0) | 32329804 | 3232993a |
| 17 |  | m_iDungeonClearCount | +0x80 | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (32329af6 JZ 3232c41d) | 32329996 | 32329aef |
| 17 |  | m_iCollectionItemID | +0x38 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (32329c9d JZ 3232420b) | 32329b4f | 32329c96 |
| 17 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 32329cea | 32329dd4 |
| 18 | USE_SKILL_POINT | m_iSpUseCount | +0xa8 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (32329f2e JZ 323246db) | 32329e3e | 32329f27 |
| 18 |  | m_iSkillID | +0xa4 | int 32fe9fd0 (inline tail 3232a011-3232a042) | OPTIONAL: missing -> [+0xa4]=0, RETURN TRUE | 32329f6b | 3232a024 |
| 21 | PVP_NPC_HUNT | m_ePVPType | +0x68 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (3232a14e JZ 32327253) | 3232a08c | 3232a147 |
| 21 |  | m_iPVPWin | +0x70 | int 32fe9fd0 | OPTIONAL: missing -> -1 (3232a243), continues | 3232a194 | 3232a23a |
| 21 |  | m_iPVPKill | +0x74 | int 32fe9fd0 | OPTIONAL: missing -> -1 (3232a342) | 3232a283 | 3232a339 |
| 21 |  | m_iPVPPlay | +0x6c | int 32fe9fd0 | OPTIONAL: missing -> -1 (3232a442) | 3232a382 | 3232a439 |
| 21 |  | m_ePvpChannelClass | +0xac | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (3232a5de JZ 32327cc3) | 3232a49d | 3232a5d7 |
| 22 | ITEM_USE | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (3232b616 JL); table absent: skipped, no failure | 33457ed8 | open 3232b5e8, insert 3232b62c |
| 22 | ITEM_USE | m_eFieldID | +0xc0 | OpenTable 32fe9940 (plain key @3348cbe8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (3232b680 CMP,3232b684 JLE); table absent: skipped, no failure | 3348cbe8 | open 3232b657, insert 3232b699 |
| 22 | ITEM_USE | m_listUseItemID | +0xd8 vector<int> (push_back 31f8b860 @3232b705) | OpenTable 32fe9940 (plain key @3348cbc8) + GetValueByIndex loop | push only if >0 (3232b6f0 CMP,3232b6f4 JLE); temp init -1; table absent: skipped | 3232b6bd (PUSH 3348cbc8) | open 3232b6c4 |
| 22 |  | m_iUseItemCnt | +0xe8 | int 32fe9fd0 with PLAIN key 3348cbb8 | REQUIRED: missing -> [+0xe8]=0, RETURN FALSE (3232b742 JZ 3232420b) | 3232b734 (PUSH 3348cbb8) | 3232b73b |
| 22 |  | m_cDifficulty | +0x13c (via saved this at [ESP+0x10]) | int 32fe9fd0 | missing -> [+0x13c]=-1 (EBP set to -1 at 3232b6c9; store 3232b88a), RETURN TRUE | 3232b78f | 3232b879 |
| 23 | SKILL_USE | m_iUseSkillCount | +0xec | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (3232ba22 JZ 3232c41d) | 3232b902 | 3232ba1b |
| 23 | SKILL_USE | m_vecSkillID | +0xf0 | OpenTable 32fe9940 (plain key @3348cba8) + GetValueByIndex 32fea4e0 loop -> std::set<int> (despite 'vec' name) insert | insert only if >0 (3232ba60 CMP,3232ba65 JLE); table absent: skipped, no failure | 3348cba8 | open 3232ba31, insert 3232ba7a |
| 24 | VISIT_VILLAGE | m_eVillageID | +0xb0 | OpenTable 32fe9940 (plain key @3348cbf4) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (3232a636 CMP,3232a63b JLE); table absent: skipped, no failure | 3348cbf4 | open 3232a60a, insert 3232a64d |
| 25 | VISIT_FIELD | m_eFieldID | +0xc0 | OpenTable 32fe9940 (plain key @3348cbe8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (3232a6b4 CMP,3232a6b9 JLE); table absent: skipped, no failure | 3348cbe8 | open 3232a688, insert 3232a6cb |
| 26 | VISIT_DUNGEON | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (3232a736 JL); table absent: skipped, no failure | 33457ed8 | open 3232a706, insert 3232a749 |
| 26 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 3232a7b2 | 3232a89c |
| 27 | FIND_NPC | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (3232a8f9 JL); table absent: skipped, no failure | 33457ed8 | open 3232a8cf, insert 3232a90c |
| 27 | FIND_NPC | m_eFieldID | +0xc0 | OpenTable 32fe9940 (plain key @3348cbe8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (3232a960 CMP,3232a964 JLE); table absent: skipped, no failure | 3348cbe8 | open 3232a937, insert 3232a976 |
| 27 |  | m_iFindNPCID | +0xd0 | int 32fe9fd0 | REQUIRED: missing -> [+0xd0]=0, RETURN FALSE (3232aabe-c4) | 3232a9da | 3232aab5 |
| 27 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 3232ab1d | 3232ac07 |
| 28 | PVP_PLAY_ARRANGE | m_iPVPPlay | +0x6c | int 32fe9fd0 (shared stub 323246c8) | REQUIRED: missing -> 0, RETURN FALSE (323246db) | 3232ac6b | 323246ce (JMP 3232ad1f -> 323246cc) |
| 31 | CHAR_LEVEL_UP | m_iCharLevel | +0xd4 | int 32fe9fd0 (shared stub 323246c8) | REQUIRED: missing -> 0, RETURN FALSE (323246db) | 3232ad6c | 323246ce (JMP 3232ae3e -> 323246c8) |
| 33 | PVP_HERO_NPC_KILL | m_ePVPType | +0x68 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (3232af4a JZ 32327253) | 3232ae88 | 3232af43 |
| 33 |  | m_iPVPKill | +0x74 | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (3232b052 JZ 3232c41d) | 3232af94 | 3232b04b |
| 33 |  | m_ePvpChannelClass | +0xac | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (3232b1ee JZ 32327cc3) | 3232b0ad | 3232b1e7 |
| 34 | PVP_HERO_NPC_PLAY | m_ePVPType | +0x68 | int 32fe9fd0 | REQUIRED: missing -> 0, RETURN FALSE (3232b318 JZ 32327253) | 3232b256 | 3232b311 |
| 34 |  | m_iPVPPlay | +0x6c | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (3232b420 JZ 3232c41d) | 3232b362 | 3232b419 |
| 34 |  | m_ePvpChannelClass | +0xac | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (3232b5bc JZ 32327cc3) | 3232b47b | 3232b5b5 |
| 36 | LEARN_NEW_SKILL | m_iLearnNewSkillCount | +0x100 | int 32fe9fd0 | REQUIRED: missing -> -1, RETURN FALSE (3232bc70 JZ 3232c41d) | 3232bb00 | 3232bc69 |
| 36 | LEARN_NEW_SKILL | m_vecSkillID | +0x104 | OpenTable 32fe9940 (plain key @3348cba8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >0 (3232bcb0 CMP,3232bcb5 JLE); table absent: skipped, no failure | 3348cba8 | open 3232bc7f, insert 3232bcca |
| 37 | PVE_WIN | m_eDungeonID | +0x40 | OpenTable 32fe9940 (plain key @33457ed8) + GetValueByIndex 32fea4e0 loop -> std::set<int> insert | insert only if >=0 (0 is stored) (32326c76 JS); table absent: skipped, no failure | 33457ed8 | open 32326c41, insert 32326c85 |
| 37 |  | m_bUpperDifficulty | +0x54 | bool 32fe9e40 (gated by isnumber 32fe4d70) | REQUIRED: missing/non-number -> byte=0, RETURN FALSE (32326e36 -> 32324aa0) | 32326cf9 | 32326e2f |
| 37 |  | m_iDungeonClearCount | +0x80 | int 32fe9fd0 | REQUIRED: missing -> [+0x80]=-1, RETURN FALSE (32326fec JZ 3232c41d) | 32326e8b | 32326fe5 |
| 37 |  | m_cDifficulty | +0x13c | int 32fe9fd0 | temp 0; missing -> [+0x13c]=-1 (stub 3232434c: 32324350), still RETURN TRUE | 32327036 | 32327120 |
| 38 | GUILD_JOIN | m_bIsGuildJoin | +0x120 | bool 32fe9e40 | REQUIRED: missing -> byte=1 (3232c09a), RETURN FALSE | 3232bf94 | 3232c08d |
| 39 | GUILD_HONOR_POINT_COLLECTION | m_iCollectionHonorPointCount | +0x11c | int 32fe9fd0 | REQUIRED: missing -> [+0x11c]=0x989680 = 10,000,000 (3232bf45), RETURN FALSE | 3232bd5f | 3232bf38 |
| 43 | ITEM_EXCHANGE | m_vecExchangeSourceItem_ID | +0x128 vector<int> (push_back 31f8b860 @3232c0e5) | OpenTable 32fe9940 (plain key @3348cb8c) + GetValueByIndex loop | push only if >0 (3232c0d3 CMP,3232c0d8 JLE). LOOP TEST IS INVERTED: body runs only when the getter returns 0 (3232c0cf TEST AL,AL / JNZ exit; 3232c0fa/fc JZ loop) - see note N5 | 3232c0a4 (PUSH 3348cb8c) | open 3232c0ab |
| 43 |  | m_iExchangeCount | +0x124 | int 32fe9fd0 (shared stub 323246cc) | REQUIRED: missing -> [+0x124]=0, RETURN FALSE (323246db) | 3232c14c | 323246ce (JMP 3232c263 -> 323246cc) |
| 44 | TOGETHER_SUBQUEST_CLEAR | m_iDungeonClearCount | +0x80 | int 32fe9fd0 | REQUIRED: missing -> -1 (3232c41d), RETURN FALSE | 3232c2b7 | 3232c410 |
| 45 | PVP_RANK | m_ePvpRank | +0x118 | int 32fe9fd0 | REQUIRED: missing -> [+0x118]=0 (3232c531), falls into 3232c537, RETURN FALSE | 3232c46d | 3232c528 |
| 46, 47 | IMMEDIATELY_CLEAR / PLAY_TOGETHER_WITH_TITLE | (no keys) |  |  | table entries 3232c630/3232c634 -> 3232c56a: close table, RETURN TRUE |  |  |
| 19,20,29,30,35,40,41,42 and any other value | FEED_PET, USER_DIE, PVP_TAG_COUNT, HYPER_MODE_USE, POINT_COUNT, PLAYER_WITH_DUNGEON_CLEAR, PGT_1VS1_CHALLENGE_MODE, PGT_WINNER | (default) |  |  | table entries -> 3232c537 (also JA 3232c537 at 32323b17 for type-1 > 0x2f): close table, RETURN FALSE, no log | 3232c537 |  |

## Notes

### N1. Q3: dungeon-type cases, m_eDungeonID and m_cDifficulty
- Dungeon-type cases = 2 (NPC_HUNT), 4 (QUEST_ITEM_COLLECTION), 5, 6, 7, 8/32/48, 12, 17, 22 (ITEM_USE), 26 (VISIT_DUNGEON), 27 (FIND_NPC), 37 (PVE_WIN). In every one of them `m_eDungeonID` (plain key string at 33457ed8) IS read, as the first or an early read of the case, and `m_cDifficulty` IS read in the same case as the last key. Open-table call addresses are in the key table (e.g. case 2: 32323ce1, case 8: 323265d8, case 26: 3232a706).
- `m_eDungeonID` is read as an ARRAY TABLE, not a scalar: OpenTable 32fe9940 accepts only Lua type 5, then GetIntByIndex 32fea4e0 for index 1,2,3... until it returns false; each value `>= 0` is inserted into the std::set<int> at struct+0x40 (CMP/JL or TEST/JS, e.g. 32323d09-0b, 32325fe4-e6; so 0 is inserted, negatives dropped). A scalar `m_eDungeonID = 5` in the Lua is not a table -> OpenTable returns false -> silently skipped, set stays empty, no failure and no log.
- There is NO check that m_eDungeonID is present or nonzero or that the set is non-empty, and no fallback when it is missing: the "table absent" branches (e.g. JNZ 32323d40 path at 32323cea) just continue with the next key. The function returns true for a dungeon-type quest with an empty DungeonID set as long as the required keys (m_bUpperDifficulty always; plus m_iKillNum / m_iDungeonClearCount / etc. per case) are present.
- m_cDifficulty (+0x13c): int, temp starts 0, if the key is missing it is set to -1 (stub 3232434c: `MOV [EBP+0x13c],-1` at 32324350) and the function STILL returns true. Cases without any m_cDifficulty read: 1, 3, 9-11, 13-16, 18, 21, 23-25, 28, 31, 33, 34, 36, 38, 39, 43-47.
- m_bUpperDifficulty (bool, +0x54) is REQUIRED in all of 2,4,5,6,7,8/32/48,12,17,37 but NOT read in 22/26/27; since the bool helper is isnumber-gated, a Lua boolean `true`/`false` fails the read and makes the whole load return false (the data must hold a number, e.g. 1/0).
- m_eDungeonMode (+0x50) is read (optional, default 0) in 2,4,5,6,7,8/32/48,12,17 only; not in 22, 26, 27, 37.

### N2. Return value paths
- false (AL=0): entry OpenTable(m_ClearCondition) failed (3232c577); type out of range (32323b17); default cases (19,20,29,30,35,40,41,42 -> 3232c537); every REQUIRED key failure listed in the table (all end at 3232c537 after their stub, 3232c53b closes the table, 3232c540 `XOR AL,AL`).
- true (AL=1): every normal end of a case (`CALL CloseTable; MOV AL,1; JMP 3232c542` style, and 3232c56a/3232c573), cases 46/47 with no key reads, and the m_cDifficulty-missing path (3232434c, AL=1 at 3232435f).
- Failure leaves the struct partially filled; fields are not reset beyond the stub value.

### N3. Common keys requested in the brief
- m_eDungeonID, m_eKillNPCID, m_eFieldID, m_eVillageID, m_listUseItemID, m_vecSkillID, m_vecExchangeSourceItem_ID, m_iUseItemCnt, m_iCharLevel are all INSIDE cases (see the table: DungeonID in 12 cases; FieldID in 2,4,22,25,27; VillageID in 1,24; KillNPCID in 2,4; UseItemID/UseItemCnt in 22; vecSkillID in 23 and 36; ExchangeSourceItem_ID in 43; CharLevel in 31). None is common/pre-switch/post-switch.
- m_bExpDungeonAndField and m_eExceptionDungeonID: UNRESOLVED. They appear in June quest Lua (q_june_quest_keys.tsv: 36 and 6 uses) but this function does not read them, and no decoded string of that name exists in june2017_re/xorstrings.tsv or in the "== ..." parser lists of q_parser_keys.txt (0 hits). Either another function with an undecoded key table reads them, or June ignores them.

### N4. Containers
std::set<int> (unique, sorted): +0x24 KillNPCID, +0x40 DungeonID, +0xb0 VillageID, +0xc0 FieldID, +0xf0 and +0x104 SkillID. std::vector<int>: +0xd8 UseItemID, +0x128 ExchangeSourceItem_ID, +0x8c AttribEnchant (fixed slots, set via 32344f00 with 0<=val<8). Insert thresholds: DungeonID `>=0`; Village/Field/KillNPC/UseItem/SkillID/Exchange `>0`.

### N5. Suspected June bug, case 43 (ITEM_EXCHANGE) array loop
At 3232c0cf-c0d1 the loop reads index 1 and does `TEST AL,AL; JNZ 3232c0fe` (exit to CloseTable) when the getter SUCCEEDS; the body (3232c0d3-c0e5, push_back) runs only when the getter returns 0, and the loop repeats while it returns 0 (3232c0fa/fc `JZ 3232c0d3`). Every other array loop in this function uses `CMP AL,1; JNZ exit`. Consequences as compiled: a table of numbers is never pushed (vector stays empty); an empty table or non-numeric first element makes the loop spin through indices (temp stays -1, never pushed) until some index yields a number, possibly forever. Not verified at runtime; the disassembly is unambiguous (3232c0ca-c0fe).

### N6. Other observations
- m_eDungeonMode / m_ePvpTournamentType use the idiom `MOVZX; NEG; SBB; AND [temp]` = `success ? value : 0` (temp preset to 0), so they are optional and default 0.
- Cases 13/14/16 jump to the shared inline tail 3232a01e/3232a022 (read int into [EDI], missing -> 0 and still true); case 18's m_iSkillID falls into the same tail.
- Cases 3 (ItemNum), 28, 31, 43 (ExchangeCount) use the shared stub 323246c8/323246cc (read int into [EDI]=struct+X, else [EDI]=0 and false).
- XOR key strings use the usual one-shot static-init guard (TEST/OR of 337a0d7c, 337a0910 ...), not data dependent.
- Case 45 (PVP_RANK) key m_ePvpRank, case 38/39: March has no equivalent keys (june-only per q_june_quest_keys.tsv).

Note: for plain (non-XOR) keys the use_va column holds the address of the key string (33457ed8 m_eDungeonID, 3348cbf4 m_eVillageID, 3348cbe8 m_eFieldID, 3348cbd8 m_eKillNPCID, 3348cbc8 m_listUseItemID, 3348cbb8 m_iUseItemCnt, 3348cba8 m_vecSkillID, 3348cb8c m_vecExchangeSourceItem_ID, 3348cc04 m_ClearCondition) and the call_va column gives the open-table / insert call addresses.
