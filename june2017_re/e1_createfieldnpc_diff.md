# CX2TFieldNpc::CreateFieldNpc - June 325ca980 vs March X2Lib/X2TFieldNpc.cpp:1740-2203

Conventions. VAs are from a raw capstone pass (`funcdis.py --func 0x325ca980`, 325ca980..325cca0a, RET 0x24 at 325cc912). The Ghidra decompile
(`dec/325ca980.c`) is mangled (std::codecvt/_Locinfo names, wrong string constants) and was NOT used as evidence.
- Signature: `__thiscall CreateFieldNpc(this = CX2TFieldNpc, CX2World* world [esp0+0x4cc], HouseTemplet* T [esp0+0x4d0], std::wstring name BY VALUE [esp0+0x4d4..0x4f0])`,
  callee pops 0x24. esp0 = esp after the prologue pushes (325ca9ad). `edi` = T, `esi` = this for the whole function (edi is bumped to T+0x120 at 325cc7a0 near the end).
- `this+` = NPC member; `T+` = HouseTemplet member (June 0x618 map in NOTES.md "June HouseTemplet offsets -> key"). NPC member names: c9h5_fieldnpc_onframemove_diff.md Conventions
  (+0x198 m_pHouseTemplet, +0x1b8/1bc/1c0 skin/mesh/aniXET, +0x244 m_pDlgNpcMessage, +0x24c m_iNpcIndex, +0x26c m_pUnitShadow, +0x274.. talk strings, +0x3fc m_strHouseName,
  +0x428 m_bStaticMesh, +0x46c m_vecNPCTalkBox, +0x484.. near-sound, +0x5ad m_bLoaded, +0x5b0 m_NpcHouseID, +0x5b4/5c0/5cc m_vPos/Rot/Scale). Names marked (inferred) come from
  layout or from the NOTES key map; they are not in the doc.
- "wstring assign" = 31f54910 (assign(const wstring&, 0, npos)); "assign(ptr,len)" = 31f54a10. "compare" = 31c1c1a0, a "non-empty" test is `compare(0, size, L"" 3344f1f4, 0) != 0`.
- Verdicts: same / changed / June-only / March-only. March line numbers are file lines in X2Lib/X2TFieldNpc.cpp.
- New NPC members first written here are flagged "June-only +0xNNN". Initial values of members the ctor 325d13e0 sets are quoted where they matter.
- Literal strings in the whole function (xordecrypt.py found 0 XOR strings; no GET_STRING / STR_ID anywhere): L"" 3344f1f4, L"Npc_Pivot" 334acc30, L"Bip01" 3344a594, L"shadow.dds" 334acbbc,
  __FILE__ L"D:\SOURCE\INT\source\X2Lib\X2TFieldNpc.cpp" 334acbd8 (lines 0xb67 = 2919 and 0xb7e = 2942, AddModelXSkinMesh args). No particle name, no effect name.

## Blocks in June order

1. **325ca980-325ca9bb** prologue, SEH frame, stack cookie (0x4a8 locals). March: none (MSVC debug-free). **same** (no logic).
2. **325ca9bb-325caa53** member init.
   - this+0x194 = world (m_pWorld, inferred), +0x198 = T (m_pHouseTemplet), +0x42c = T+4 (Type, March m_eHouseType), +0x5b0 = T+0 (m_NpcHouseID), word +0x1a2 = 0 (m_bTalk, m_bBye = false).
   - +0x5c0..0x5c8 = T+0x168.. (m_vRot, 3 floats), +0x5cc..0x5d4 = T+0x174.. (m_vScale).
   - **June-only** +0x5a4 = 0, +0x5a8 = 0 (the friendship multi-line sequencer line index / flag; ctor 325d1a08 also zeroes both).
   - March 1745-1755. **same** (+ the two June-only stores). March ASSERTs 1742-1743 are release-compiled out: no code.
3. **325caa53-325cabb3** June-only temp backup. Two 0x184-byte "override rows" are default-constructed (`325d2080`: 13 wstrings + vector, at esp0+0x294 = BACKUP, esp0+0x110 = FOUND). Then 10 of T's fields are copied into BACKUP:
   BACKUP+0x24 <- T+0x24 NPCTextureName, +0x40 <- T+0x40 NPCTextureKey, +0x5c <- T+0xac NPCMeshName, +0x78 <- T+0x244 Wait talk, +0x94 <- T+0x2b4 Quest talk,
   +0xb0 <- T+0x260 Make talk, +0xcc <- T+0x308 Exchange talk, +0x04 <- T+0x27c Shop talk, +0xe8 <- T+0x2d0 Bye talk, vector +0x104 <- T+0x394 (TALK_BOX; destination cleared first via 31c33540/31aa9640, then 31c57560).
   Pointers to the ten T fields are kept in locals (esp0+0x20 T+0x24, +0x54 T+0x40, +0x24 T+0xac, +0x50 T+0x244, +0x3c T+0x2b4, +0x40 T+0x260, +0x48 T+0x308, +0x38 T+0x27c, +0x4c T+0x2d0, +0x44 T+0x394).
   March: none. **June-only.**
4. **325cabb3-325cacb2** June-only "completed set" build. `new(0x14)` set head (32fe243e), `3260ea70` clear; if `g_pData (33775860) && g_pData+0x68`: `325d1db0` copies the std::set<int> at (g_pData+0x68)+0xd0 (set copy-ctor 32699370) and assigns it into the local set (swap helper [344c0350]; temp destroyed 32345900 + free 32fe2300).
   Identity of the g_pData+0x68 object and of the ids in that set: **unverified** (c9h5 funcmap rows say g_pData+0x68 -> +0x90 -> unit data, so it is probably the user data object, not the quest manager whose lists NOTES put at +0xcc/+0xdc).
5. **325cacb2-325caf0e** June-only template override lookup.
   - `sub esp,0x10` + set copy by value (32699370), then `325cacb7 ecx = this+0x5b0 (house id)`; `32696fa0` (friendship-manager singleton getter, 0x610 bytes, g_337a66b8) then `326821d0(mgr, houseID, &FOUND, set by value)` RET 0x18.
   - 326821d0 walks mgr rows (vector at mgr+0x4e8..0x4ec, stride 0x184, count via imul 0x151d07eb); a row matches when `row+0x20 == houseID` and `row+0` (dword) is a member of the set (inlined lower_bound); on match 3269ac40 copies the row into FOUND, returns 1 (else 0).
   - If it returned 1 (`325cace2 cmp al,1`), the strings of FOUND overwrite T **in place** when non-empty (each guarded by compare != 0):
     - FOUND+0x24 AND FOUND+0x40 both non-empty (325cacea-325cad4e) -> T+0x24, T+0x40 (NPCTextureName/Key; only as a pair);
     - FOUND+0x5c -> T+0xac (NPCMeshName) @325cad4e-325cad81 (checked at esp+0x17c = FOUND+0x6c);
     - FOUND+0x78 -> T+0x244 Wait, +0x94 -> T+0x2b4 Quest, +0xb0 -> T+0x260 Make, +0xcc -> T+0x308 Exchange, +0x04 -> T+0x27c Shop, +0xe8 -> T+0x2d0 Bye;
     - FOUND vector +0x104 (non-empty) -> T+0x394 (TALK_BOX, erase + 31c57560) @325caeb3-325caf09.
   - FOUND+0x114/+0x130/+0x14c/+0x168 (four more wstrings) are NOT read in this function.
   - Because T is restored in block 21, only what CreateFieldNpc itself consumes (mesh name, talk strings, talk box) is affected; T+0x24/T+0x40 are not consumed by CreateFieldNpc at all (they are only backed up/restored/overridden).
   - March: none. **June-only.** (This is a different mechanism from the qm relation rows / TFieldGame replace(row) in NOTES: that one builds a copied T before calling AddFieldNpc; this one patches the shared T during creation.)
6. **325caf0e-325caf3e** destroy the set copies (32345900 + 32fe2300). **June-only.**
7. **325caf3e-325cb0bb** position / join area / anim speeds / static list. March 1754-1803.
   - Three-way pos choice: T+0x224 (COMMON_NPC_LOCATION) empty and T+0x234 (CUSTOM_NPC_LOCATION) empty -> this+0x5b4..0x5bc = T+0x15c.. (325caf61-325caf85; March 1761-1763 **same**).
   - COMMON empty, CUSTOM non-empty -> `325c95e0(this)` **June-only**: reads g_pTFieldGame (33775870)+0xa8 -> +0x2c worldID, collects T+0x234 entries (stride 0x20, world id at +0) with that world: 0 matches -> pos = T+0x15c (rot stays), 1 -> that entry pos (+4) / rot (+0x10), >=2 -> rand() pick; then Move(skin +0x1b8, pos) if a skin exists.
   - COMMON non-empty (325cafe0-325cb0af): `world != NULL` (June adds the NULL guard; March asserts), worldID = world->[+0x2c]->[0] (null-guarded); first COMMON entry (stride 0x20, world id at +0) matching worldID -> pos = entry+4.., rot = entry+0x10.., and if entry+0x1c (local join area float) > 0 -> this+0x5e4 = (int)(32fe25f0 ftol). March 1765-1787 **same** (LOCAL_NPC_JOIN_AREA live in both).
   - Otherwise this+0x5e4 = T+0x194 (JoinArea; ctor default 0x1f4 = 500) @325caf85-325caf8f. March 1788-1793 **same**; member +0x5e4 is June's m_NPCJoinArea (inferred).
   - **June-only/changed**: anim speeds `fld/fstp` this+0x5d8 = T+0x180 (JoinAnimSpeed), this+0x5dc = T+0x184 (WaitAnimSpeed), this+0x5e0 = T+0x188 (Bye/third speed, name inferred) @325caf95-325cafb3. March: none.
   - Static-mesh list: `jmp table` on (T+0 - 40021) with 6 hits -> `this+0x428 = 1` @325cb0b4: 40021 HI_POSTBOX, 40036 HI_CHRISTMAS_TREE, 40082 HI_AGIT_BANK, 40083 HI_GUILD_MARK, 40130 HI_STORY_QUEST, 40140 HI_TRANSCEND_QUEST (table 325cc940/325cc948, bytes decoded). March 1795-1803: POSTBOX + CHRISTMAS_TREE only. **changed: +4 IDs.** (+0x428 is never reset to 0 by either build; the ctor 325d1989 zeroes it.)
8. **325cb0bb-325cb0d4** `this+0x5ad (m_bLoaded) = 0` @325cb0c2, then branch on `this+0x5ac` (m_bBackgroundLoad, ctor 325d1786). March 1805-1807. **same.**
9. **325cb0ce-325cb403** background-load path (`+0x5ac == 1`). March 1808-1825.
   - Mesh 1: OpenXSkinMeshInBackground `31cdfe60(devmgr = [g_pKTDXApp 337676e8]+0x1f4, wstring T+0xac, priority 1 (PRIORITY_HIGH), pivot)` -> this+0x1bc (325cb138 non-static; 325cb1ed static). Pivot = L"Npc_Pivot" 334acc30 if `+0x428 == 0`, else L"Bip01" 3344a594 (the March default arg). March 1810/1812 **same** (pivot only explicit for non-static; same value as March's).
   - **June-only** Mesh 2: same call with T+0xe4 (NPCMeshName2nd) and the same pivot -> this+0x1c8 (325cb25a).
   - If mesh 1: `31d28b60 DeleteKTDGObject(this+0x1b8)`, +0x1b8 = 0; aniXET: OpenXETInBackground `31ce0a90(T+0xc8 NPCAniTex, 1, 0)` -> +0x1c0 (325cb2dc); **June-only** MultiTex: `31ce0a90(T+0x5b0, 1, 0)` -> **this+0x1ec** (325cb354); SetCenter: this+0x150..0x158 = this+0x5b4.. (325cb360). March 1815-1823 **same** + MultiTex.
   - **June-only** if mesh 2: DeleteKTDGObject(+0x1c4), +0x1c4 = 0 (325cb38d), `31ce0a90(T+0x100 NPCAniTex2nd, 1, 0)` -> **+0x1cc** (325cb3f8); then `jmp 325cba1e`.
   - Empty name -> device opens return NULL (31cdfe60 @31cdfebc, 31ce0a90 @31ce0aec test the wstring size). m_bLoaded stays 0 (completed in OnFrameMove block 2).
10. **325cb403-325cba1e** foreground-load path. March 1826-1863.
   - 10a opens (325cb427-325cb620): `31cdef40(devmgr, wstring T+0xac, wstring pivot)` = OpenXSkinMesh -> +0x1bc (325cb49e/325cb58b). Pivot wstring built from L"Npc_Pivot"(len 9) for non-static, L"Bip01"(len 5) for static (March: static used the default). **June-only** mesh 2: OpenXSkinMesh(T+0xe4, same pivot) -> +0x1c8 (325cb620). **same + mesh 2.**
   - 10b (325cb626-325cb80e) if mesh 1 non-NULL: `31d28b60` DeleteKTDGObject(+0x1b8); skin = new CKTDGXSkinAnim (pool `31d2f930(0x874)` + ctor `31d32040`, i.e. an inlined CreateSkinAnim) -> +0x1b8 (325cb66e). If skin: aniXET `31cdf880(T+0xc8, false)` = OpenXET -> +0x1c0 (325cb6d9); `skin+0x2b4 = 0` (SetApplyMotionOffset(false)); `31d32750(skin, mesh, xet, 0)` SetAnimXSkinMesh (3rd arg new); `31d2f3f0(skin, mesh, xet, 0,0,0, 1, __FILE__, 2919)` AddModelXSkinMesh (multiTex args = 0, five new args); Show(false) (inlined: 31fae370 test, byte +0x172, vslot +0x28); Move(this+0x5b4) `31c5aff0`; RotateDegree(this+0x5c0) `31cfdcd0`; Scale(this+0x5cc) `31c5b0d0`; each matrix op after `31c19d90` when skin+0xc. March 1839-1852 **changed: signature extras only, same sequence, MultiTex is NOT used on the foreground path (this+0x1ec stays the ctor's 0).**
   - 10c (325cb80e-325cb9cb) **June-only** skin 2: if mesh 2 -> DeleteKTDGObject(+0x1c4), new skin -> +0x1c4; if skin: `31cdf880(T+0x100 via wstring 31aa1670, false)` -> +0x1cc; same SetAnimXSkinMesh(mesh2, xet2, 0) / AddModelXSkinMesh(..., 2942) / Show(false) / Move / RotateDegree / Scale.
   - 10d (325cb9cb-325cba1e) still inside the "mesh 1" gate: this+0x150..0x158 = pos (SetCenter); radius = mesh1[+0x87c] * this+0x5d0 (scale.y) -> this+0x15c (SetBoundingRadius); this+0x5ad (m_bLoaded) = 1. March 1857-1861 **same** (radius = m_pMesh->GetBoundingRadius()*m_vScale.y).
11. **325cba1e-325cbaad** NPC id and name. March 1865-1912.
   - non-static (`+0x428 == 0`): this+0x24c = `*T+0x214` (NPC_ID_LIST[0], no empty-vector guard, same as March's original lines 1904; the Ranox guard flag exists only in our fork); name argument non-empty (compare(0, name.size, L"", 0) != 0 @325cba4a): **this+0x250 (m_strNpcName) = name** (31f54910 @325cba69). Else `GetNPCUnitTemplet` (`323737a0`, [g_pData+0x38], npcIndex @325cba7b) and, if non-NULL, this+0x250 = templet+8 (m_Name). NULL templet -> name left untouched (March fork guard shape; unguarded in March original).
   - static: this+0x24c = -1, this+0x250 = L"" (assign(L"",0) @325cbaa8); the name argument is ignored. March 1892-1893/1909-1910 **same**.
   - **changed: name override added; same fallback.**
12. **325cbaad-325cbc0c** flag copies (byte). March 1914-1973. **changed: set differs.**
   - this+0x5fb = T+0x1e8 (EVENT_NPC), +0x5e8 = T+0x198 PVP, +0x5ea = T+0x199 MAKING, +0x5eb = T+0x19a TRAINING, +0x5ec = T+0x19b FREE_TRAINING, +0x5ed = T+0x19c ENCHANT, +0x5ee = T+0x19d ATTRIBUTE,
     +0x5ef = T+0x1a0 GUILD, +0x5f0 = T+0x1a1 RANKING, +0x5f1 = T+0x1a2 EXCHANGE, +0x5f2 = T+0x1a3 PRIVATE_BANK, +0x5f3 = T+0x1a4 PERSONAL_SHOP, +0x5f4 = T+0x1ab AGENCY_REG, +0x5f5 = T+0x1ac AGENCY_RECV,
     +0x5e9 = T+0x19f SELL (March m_bShopNpc = m_bSell, ADD_HOUSE_ID_TO_MANUFACTURE live), +0x5f6..0x5f9 = T+0x1a5..0x1a8 EVENT1-4,
     June-only: +0x606 = T+0x429 PHOTOZONE, +0x607 = T+0x42a FRAGMENT_EXCHANGE, +0x608 = T+0x42b ROCKSCISSORSPAPER, +0x609 = T+0x1a9 OLD_ENCHANT, +0x60a = T+0x1aa OLD_ADVANCED_ENCHANT,
     +0x5fd = T+0x3a5 (bExchangeNewItem), +0x5fe = T+0x3e8 HENIR_EQUIP_UPGRADE, +0x5ff = T+0x408 GUILD_AGIT_SERVICE.
   - March-only: m_bWebEvent, m_bSynthesisNpc (no June fields). **T+0x19e REPAIR is not read anywhere in this function.**
13. **325cbc0c-325cbcb6** **June-only** HI_MU special: gate `g_pData && g_pData+0x2ec && [g_pData+0x2ec] != 0 && g_pMain(3377585c)+0x8c == 0xf && T+0 == 0x9c6e (40046 HI_MU)`; copies a std::map (3215cbf0) of g_pData+0x2ec, `32822360` finds key 0: value (node+0x14) 0 -> this+0x600 = 0, 0x601 = 1, 0x602 = 1; value 1 -> all 0; not found -> this+0x600 = 1; temp dtor 3289c4f0. Those three bytes are the third SetShopType arg of types 2 / 0xf / 4 in block 16 (SHOP: +0x600==1, EXCHANGE: +0x601==1, MAKE: +0x602==1). Meaning of the map/state 0xf: **unverified.** (ctor zeroes +0x5fd..0x604 as dwords.)
14. **325cbcb6-325cbcde** this+0x603 = T+0x409 GUILD_ENTER, +0x604 = T+0x40a EXTRACTION, +0x605 = T+0x428 TRADE. **June-only.**
15. **325cbcde-325cbe1e** **June-only** motion-name map this+0x1f4 (std::map<int,wstring>, head 0x1f8 allocated by ctor): `325d2370(map, key)` operator[] + assign: key 1 = T+0x4b0 WAIT01 (325cbcf2), 2 = T+0x4cc WAIT02 (325cbd27), 3 = T+0x4e8 WAIT03 (325cbd50), 4 = T+0x520 TALK (325cbd79), 0xa = T+0x504 TALKLOOP (325cbda2), 5 = T+0x53c TALKWAIT (325cbdcb), 6 = T+0x558 BYE (325cbdf4), 0 = T+0x574 NORMAL (325cbe19). Always assigned, even if the templet name is empty. March: none (motion names are literals in OnFrameMove).
16. **325cbe1e-325cbe77** **June-only** custom motion queue: this+0x204 vector (elements 0x20 bytes = wstring + flag) cleared (323bfe70 + 31e71d00 when begin != end), then `323c0610(this+0x204, &T+0x608)` = vector operator= copying CUSTOM_MOTION_NAME. March: none.
17. **325cbe77-325cc607** NPC dialog block, gate `this+0x244 != 0 && +0x428 == 0` (March 1976 gate **same**). March 1976-2123.
   - 17a (325cbe91-325cbede) `32eca610(dlg, T)` CreateHouseNPCName (inferred), this+0x3fc = T+8 (m_strHouseName), `32ec8fb0(dlg, this+0x24c, copy of m_strHouseName)` SetNpcId. March 1979-1982 **same**.
   - 17b (325cbede-325cc281) `32ecd610` SetShopType(type, 1, X, 0) (4-arg June form; March 1-arg) for each flag, in this order: +0x5e8 -> 3 PVP; +0x5e9 -> 2 SHOP (X = +0x600 == 1); +0x5ea -> 4 MAKE (X = +0x602 == 1); +0x5eb -> 6 TRAINING; +0x5ec -> 5 FREETRAIN; +0x5ed -> 0xc ENCHANT; +0x5ee -> 0xd ATTRIBUTE; +0x5ef -> 0x11 GUILD; +0x5f0 -> 0xe RANKING; +0x5f1 -> 0xf EXCHANGE (X = +0x601 == 1); +0x5f2 -> 0x10 PRIVATE_BANK; +0x5f3 -> 0x13 PERSONAL_SHOP; +0x5f4 -> 0x14; +0x5f5 -> 0x15; +0x5f6..9 -> 0x1c..0x1f EVENT1-4; +0x5fd -> 0x17 EXCHANGE_NEW_ITEM; (==1) +0x5fe -> 0x18; +0x606 -> 0x1b; +0x607 -> 0x20; +0x608 -> 0x21; +0x609 -> 0x22; +0x60a -> 0x23; then the 2204 block; (==1) +0x5ff -> 0x19; +0x603 -> 0x24; +0x604 -> 0x25; +0x605 -> 0x1a.
     June ids 3..0x15 equal March NSBT ids 3..21 (March enum read); ids >= 0x17 are June's renumbered set (c9h5 block 18/25 note).
     March-only: WEDDING_EVENT (SERV_RELATIONSHIP_SYSTEM_LAUNCHING_EVENT), WEB_EVENT, SYNTHESIS. **changed.**
   - 2204 block (325cc17c-325cc219): `[33775864 (g_pInstanceData per NOTES) +0xa18]` std::map find of key 0x89c (322aa950); skip to 325cc219 if found and byte +0x10 == 1; otherwise `this+0x24c - 10004` jump table: NPC ids 10004 NUI_ECHO, 10009 NUI_CHACHABUK, 10015 NUI_AMOS, 10020 NUI_ALLEGRO, 10028 NUI_GRAIL, 10041 NUI_DENKA, 10057 NUI_VAPOR, 10069 NUI_FACEOFF -> 325cc1d3, which repeats the find and calls SetShopType(0x18, 0, 1, 0) only if found AND byte == 1. As decoded this body is unreachable (it requires the exact value the first test just skipped on). Whether the two byte tests are the same field: **unverified**. June-only.
   - 17c (325cc281-325cc332) talk strings (assign from the already-overridden T fields): +0x274 = T+0x244 Wait, +0x290 = T+0x260 Make, +0x2ac = T+0x27c Shop, +0x2c8 = T+0x298 Pvp, +0x2e4 = T+0x2b4 Quest, +0x300 = T+0x2d0 Bye, +0x31c = T+0x2ec Ranking, +0x338 = T+0x308 Exchange, +0x354 = T+0x324 PrivateBank. March 2085-2095 **same** (offsets match the doc names).
   - 17d (325cc332-325cc500) **June-only** loop over the std::map at T+0x430 (head T+0x434): per node (key at node+0xc, value struct: vector at +0x10, wstrings at +0x20/+0x3c/+0x58/+0x74/+0x90) copy into a local struct and insert (`325d25f0` make_pair, `32506dc0`, `325d2950`) into the std::map at **this+0x524**; destroy local (325d2170). Meaning of the T+0x430 table: **unverified** (NOTES: "Main_Talk / NPC_COMMON_TALK container?").
   - 17e (325cc500-325cc5fc) talk strings: **June-only** +0x550 = T+0x45c, +0x588 = T+0x494, +0x56c = T+0x478, +0x534 = T+0x440, +0x4b4 = T+0x3cc Upgrade, +0x4d0 = T+0x3ec Service, +0x4ec = T+0x40c Trade; and the March-twin +0x370 = T+0x340 GuildAD, +0x38c = T+0x35c PersonalShop, +0x3a8 = T+0x1b0 Register, +0x3c4 = T+0x1cc Receive, +0x3e0 = T+0x378 ExchangeNewItem (March 2101-2120). **same + June-only talks.** March-only: m_strSynthesisTalk.
   - 17f (325cc5fc-325cc607) `32ec90b0(dlg)` SetKeyEvent. March 2122 **same**.
18. **325cc607-325cc6f5** shadow. March 2150-2164 gate `m_NpcHouseID != HI_BILLBOARD`; June gate: T id not in {0x9c5c = 40028 HI_BILLBOARD, 0x9c9c = 40092 HI_YUNO, 0x9cd3 = 40147 HI_SPA_PPORU}. `322a8d10(200.0f [33457460])` CreatePlanShadow -> this+0x26c; if non-NULL: shadow+0x164 = 0xe (XL_EFFECT_0 = 14 in both enums), SetShadowTexture(L"shadow.dds" via 31aa1670 + 3322b660), UpdatePosition(pos, pos, rot) `3322b4d0`, AddObjectChain `31ce9a60` on [g_pKTDXApp]+0x1fc. **changed: +2 excluded ids.**
19. **325cc6f5-325cc70c** `325c8250(this, T+0x3a4, 0)` SetCanTalkNpc(bCanTalkNpc, false); `325c81a0(this)` GetQuestInfo. March 2168/2172 **same**.
20. **325cc70c-325cc735** TALK_BOX: push_back each of T+0x394 (post-override) into this+0x46c (`31aad250`). March 2174-2182 **same** (but reads the overridden vector).
21. **325cc735-325cc79d** near-sound: `T+0x3a8` non-empty and devmgr ([g_pKTDXApp]+0x1f4) non-NULL -> this+0x484 = 1, this+0x488 = name, this+0x4a4 = this+0x4a8 = T+0x3c4, this+0x4ac = T+0x3c8. March 2184-2201 **same.**
22. **325cc79d-325cc7b2** **June-only**: `edi += 0x120`; this+0x1d0 = T+0x120 (UI file name) via assign, unconditionally (also for static houses). No default; L"" when the templet has none.
23. **325cc7b2-325cc8a5** **June-only** restore: the ten backed-up values are assigned back from BACKUP to T (T+0x24, +0x40, +0xac, +0x244, +0x2b4, +0x260, +0x308, +0x27c, +0x2d0; vector T+0x394 erased and re-copied). Normal path only (no exception cleanup of the T mutation seen).
24. **325cc8a5-325cc912** dtors (`325d1de0` on FOUND and BACKUP, name wstring free if cap >= 8), SEH unwind, cookie check, `ret 0x24`. **same shape.** 325cc915-325cc93f is the operator-new failure path (std::bad_alloc throw).

## Settled questions

1. **name arg (4th param)**: never copied to a templet field. Used once (325cba1e-325cbaad): non-static + name non-empty -> this+0x250 (m_strNpcName, inferred from the layout between +0x24c m_iNpcIndex and +0x26c m_pUnitShadow) = name (assign @325cba69). Name empty -> this+0x250 = GetNPCUnitTemplet(T+0x214[0])->m_Name (323737a0 @325cba7b, +8). Static houses ignore it (+0x24c = -1, +0x250 = L"").
2. **T+0x120 UI file name** -> this+0x1d0 (assign @325cc7ad, unconditional, after near-sound, no default).
3. **Consumption in this function**:
   - T+0x11c/0x11d/0x11e, T+0x18c, T+0x190, T+0x5a0/0x5a1, T+0x5a4..0x5ac (UseRenderOption, UseAlphaBlend, blend modes), T+0x5cc.., T+0x5e8 EFFECT_SET_NAME, T+0x604, T+0x19e REPAIR, T+0x590, T+0x13c/0x14c, T+0x42c: **not read** by 325ca980.
   - Their consumer found on the way: the per-skin render-param routine 325d1040 (cond. on this+0x5fa and this+0x194; reads T through this+0x198): T+0x190 @325d113b, T+0x11c @325d1151 (+ T+0x18c -> param+0x14 @325d1173), RenderOption T+0x5a4 @325d118b with +0x5a5/+0x5a8/+0x5ac/+0x5cc/+0x5d0/+0x5d4/+0x5d8..0x5e4, T+0x11d @325d1334, T+0x11e @325d1356 (this+0x1f0 multiplier). T+0x5e8 is read by OnFrameMove block 4.
   - T+0x180/0x184/0x188 -> this+0x5d8/0x5dc/0x5e0 (floats, 325caf95/325cafa1/325cafad); T+0x194 -> this+0x5e4.
   - MultiTex T+0x5b0 -> this+0x1ec via OpenXETInBackground (31ce0a90) at 325cb354, **background path only**; the foreground path never writes +0x1ec (ctor 325d1a14 = 0).
   - 8 motion names -> this+0x1f4 map, keys 1,2,3,4,0xa,5,6,0 (325cbcf2..325cbe19); CUSTOM_MOTION_NAME T+0x608 -> this+0x204 (323c0610 @325cbe72, vector cleared first).
4. **Device opens** (devmgr = [g_pKTDXApp 337676e8]+0x1f4): background: 31cdfe60 (mesh1 T+0xac -> +0x1bc, mesh2 T+0xe4 -> +0x1c8), 31ce0a90 (aniXET T+0xc8 -> +0x1c0, MultiTex T+0x5b0 -> +0x1ec, aniXET2 T+0x100 -> +0x1cc); foreground: 31cdef40 (mesh1/mesh2), 31cdf880 (aniXET T+0xc8 -> +0x1c0 inside the skin gate, aniXET2 T+0x100 -> +0x1cc inside the skin-2 gate). Pivot L"Npc_Pivot" (non-static) / L"Bip01" (static). **Particle creation: none** in 325ca980 (the +0x230 NpcTalkParticle is made elsewhere, 325c876f; no effect-set either).
5. **Literals / STR ids**: only the five literals listed in Conventions; no STR id, no XOR string, no particle/effect name.

## Callees (VA, funcmap name or "unnamed", role)

Funcmap has an exact-start row only for 32696fa0 ("June NPC friendship manager singleton getter") and 31d03920; every other VA below is **unnamed in funcmap.tsv**, roles are from this pass, the c9h5 doc, or shape (inferred).
- 31aa1670 unnamed wstring(const wchar*) ctor (inferred) x2 | 31aa9640 unnamed vector<wstring> range destroy x3 | 31aad250 unnamed vector<wstring>::push_back (talk box)
- 31c19d90 unnamed June matrix accessor (c9h5 doc, likely) x6 | 31c1c1a0 unnamed std::wstring::compare (funcmap C2 rows) x11 | 31c33540 unnamed vector erase-range move x3 | 31c57560 unnamed vector<wstring>::operator= x4
- 31c5aff0 unnamed CKTDGMatrix::Move | 31c5b0d0 unnamed Scale | 31cfdcd0 unnamed RotateDegree | 31d28b60 unnamed DeleteKTDGObject x4 | 31d2f3f0 unnamed AddModelXSkinMesh x2 | 31d32750 unnamed SetAnimXSkinMesh x2
- 31d2f930 unnamed pooled new(0x874) x2 | 31d32040 unnamed CKTDGXSkinAnim ctor x2 | 31fae370 unnamed manager-show test (inlined SetShowObject) x2
- 31cdef40 unnamed OpenXSkinMesh (funcmap C3y text) x3 | 31cdf880 unnamed OpenXET x2 | 31cdfe60 unnamed OpenXSkinMeshInBackground x3 | 31ce0a90 unnamed OpenXETInBackground x3 | 31ce9a60 unnamed AddObjectChain (null -> E_FAIL)
- 31e71d00 / 323bfe70 unnamed destroy / erase for vector<{wstring,flag}> | 323c0610 unnamed vector<{wstring,flag}>::operator=
- 31f54910 unnamed wstring::assign(const wstring&,0,npos) x67 | 31f54a10 unnamed wstring::assign(const wchar*,len) x15
- 3215cbf0 / 32822360 / 3289c4f0 unnamed map copy / find / dtor (HI_MU block) | 322a8d10 unnamed CreatePlanShadow(float) | 3322b4d0 unnamed UpdatePosition | 3322b660 unnamed SetShadowTexture | 322aa950 unnamed map::find (funcmap rows)
- 32345900 unnamed set _Erase | 3260ea70 unnamed set::clear | 32699370 unnamed set copy-ctor | 325d1db0 unnamed copy of set<int> at arg+0xd0 | 32696fa0 friendship-manager singleton getter | 326821d0 unnamed override-row finder (this blocks 4-5)
- 323737a0 unnamed UnitManager::GetNPCUnitTemplet | 32eca610 unnamed dlg CreateHouseNPCName | 32ec8fb0 unnamed dlg SetNpcId | 32ec90b0 unnamed dlg SetKeyEvent (c9h5 doc) | 32ecd610 unnamed dlg SetShopType x30
- 325c81a0 unnamed GetQuestInfo (funcmap C9h-5 text) | 325c8250 unnamed SetCanTalkNpc | 325c95e0 unnamed CUSTOM_NPC_LOCATION chooser (decompile) | 325d2080 ctor / 325d1de0 dtor of the 0x184 override row | 325d2170 dtor of the T+0x430 value struct
- 325d2370 unnamed motion-map operator[] x8 | 325d25f0 / 32506dc0 / 325d2950 unnamed map insert helpers (NPC+0x524)
- CRT: 32fe243e new, 32fe2300 free, 32fe245c cookie check, 32fe25f0 _ftol2, 32fe2444 throw, [0x344c0350] and [0x344c00d0] unnamed thunks.

## NPC members written (June offset: value / source, VA)

Prologue 325ca9cb: +0x194 world, +0x198 T, +0x42c T+4, +0x5b0 T+0, +0x1a2(word) 0, +0x5c0..0x5c8 T+0x168.., +0x5cc..0x5d4 T+0x174.., +0x5a4 0, +0x5a8 0.
Position: +0x5b4..0x5bc (plain T+0x15c / COMMON / CUSTOM), +0x5c0..0x5c8 (COMMON or CUSTOM rot), +0x5e4 (T+0x194 or (int)local join area), +0x5d8/+0x5dc/+0x5e0 (T+0x180/184/188), +0x428 = 1 (6 ids).
Load: +0x5ad 0 then 1 (fg only), +0x1bc mesh1, +0x1c8 mesh2, +0x1b8 skin1 (0 or new), +0x1c4 skin2, +0x1c0 aniXET, +0x1cc aniXET2, +0x1ec MultiTex xet (bg only), +0x150..0x158 center = pos, +0x15c radius (fg only).
Identity: +0x24c = NPC_ID_LIST[0] or -1, +0x250 name, flags +0x5e8..+0x60a (block 12/14), +0x600/0x601/0x602 (HI_MU only), +0x1f4 map, +0x204 vector (+0x208 end updated), +0x3fc = T+8, talk strings (17c/17e), +0x524 map, +0x26c shadow, +0x46c talk box, +0x484/+0x488/+0x4a4/+0x4a8/+0x4ac near sound, +0x1d0 UI file name.
ctor 325d13e0 initial values: +0x1b8..0x1cc = 0, +0x1ec = 0, +0x5ac = BG flag, +0x428 = 0, +0x5e4 = 0x1f4, +0x5d8/5dc/5e0 = float (fld1), +0x5a4/+0x5a8 = 0, +0x5fd..0x604 = 0.

## March-only
- ASSERT( pWorld/pHouseTemplate != NULL ) (compiled out); NSBT_WEDDING_EVENT, m_bWebEvent / NSBT_WEB_EVENT, m_bSynthesisNpc / NSBT_SYNTHESIS, m_strSynthesisTalk; HI_BILLBOARD-only shadow gate (replaced by a 3-id gate); in-line `m_pXSkinAnim = NULL` is present in June too (BG path).

## Unverified
- Identity of g_pData+0x68, ids in its set at +0xd0, the mgr+0x4e8 row loader/Lua keys and meaning of FOUND+0x114..0x168; HI_MU (40046) map and state 0xf; T+0x430 table semantics; whether 325cc214's SetShopType(0x18,0,1,0) can ever run; role names marked (inferred); 325d1040's caller.
