# char-luciel: Ghidra evidence (June 2017 `unpacked_x2.exe`, image base 31aa0000, GhidraMCP at 127.0.0.1:8080)

Raw dumps are in `ghidra/`. Ghidra was read-only. The program is the June binary: `.text` is 31aa1000-337b9fff, the ctor disassembly matches
the bytes of `...\14JUN17\data\unpacked_x2.exe` (file offset = va - 31aa0c00), and Chung's ctor `32bf1040` is defined. Ghidra's function
database is incomplete: 85 of the 119 Lu/Ciel override targets are not defined as functions (including 0x7b4 `32b46580`, 0x114 `32b4d810`,
0x60 `32ae4080`), so `decompile_function` fails there. For those I used capstone on the exe bytes (marked "capstone" below).

## 1. Class, vtable, ctor, dtor
- Class `CX2GULuciel`. The only RTTI-style string is `.?AVMemoConditionLuciel@CX2GULuciel@@` (3373d574); the class's own `.?AV` string is
  not in the plugin string list. Source path string `D:\SOURCE\INT\source\X2Lib\X2GULuciel.cpp` (334f0660, xref from 32b162e6 / 32b165e1).
- Vtable **334f06fc** (found from the cited overrides: 32b4d810 at 334f0810 = slot 0x114, 32aeb410 at 334f0e3c = 0x740, each minus its slot
  offset). `[334f06fc - 4]` = 3361d244 (RTTI locator). Secondary tables the ctor writes: 334f06f0 (this+0x190) and 334f06e8 (this+0x1a0).
- **Slot count 547 (0x0-0x888), not 544**: slots 0x0-0x87c are CX2GUUser's 544; 0x880 = 32b14db0, 0x884 = 32b3acf0, 0x888 = 32ae6130 are
  three Lu/Ciel-only virtuals; 0x88c starts a wide string. Full table: `ghidra/vtable_334f06fc_all.tsv` (base = CX2GUUser 334781f4).
- **ctor `32b1bd90`** (Body 32b1bd90-32b1c87b; writes 334f06fc at 32b1bde2; calls the CX2GUUser ctor `3203e850` with the constant 0x251).
  Sole xref: `3230f470` (operator new 0x98a0 + ctor). Object size 0x98a0.
- **dtor `32b39380`** (Body 32b39380-32b39790; rewrites 334f06fc at 32b393ac). Scalar deleting dtor `32b4e660` = vslot 0x0, referenced only
  from the vtable.

## 2. Range and neighbours
- Range **32ae4080-32b4e76f** (capstone + vtable): first override = vslot 0x60 DamageReact `32ae4080`; last = the thunk trailer 32b4e680-32b4e764
  (jmp thunks into Lu/Ciel functions) after the scalar deleting dtor 32b4e660. `32b4e770` / `32b4e780` are shared `ret` stubs (callers in base
  code 31fca204 / 31fede50), not class members.
- Before: **Rose** (vtable 334e8b5c; her scalar deleting dtor is `32ae4010`; 32ae4030 is a Rose helper). After: the **EL (Elesis)** class
  (vtable 334fa61c, first override `32b4e790`). Order by vtable: Add, Ain, Rose, Lu/Ciel, EL, Ara, Chung, Lire, Elsword.
- Functions: Ghidra-defined in the range **499**. Capstone + vtable + call-target estimate **583** (84 undefined in Ghidra; list
  `ghidra/range_function_candidates.tsv`). The range is 0x6A6F0 bytes (Chung's 0x4D330).
- Vtable slots pointing into the range: **122** (119 overrides of base slots 0-0x87c, including the 0x0 dtor and 0x87c = 32b172b0, plus the 3
  new slots 0x880-0x888); all distinct, none points outside.

## 3. InitState, SetEquippedSkillFuncBySkillSlotIndex
- 0x7a4 InitState = **32b43d10** (Body 32b43d10-32b45764; Ghidra decompile `ghidra/decomp_32b43d10_InitState.c`). It switches on the unit class
  (`[this+0x867c]+0x38`) and sets `this+0x8ac8`: 0x25 / 0x26 / 0x27 / 200 -> 0; 0x7f -> 1; 0x80 -> 2; 0x81 -> 3 (Enum.src.lua: CHILIARCH 37,
  ROYAL_GUARD 38, DIABLURA 39, DUO 200; DREAD_LORD 127; NOBLESSE 128; DEMONIO 129). Then `+0x7d04 = +0x4088 = 0xd`, `+0x408c = 0x28`, and it
  calls **InitStateCommon = 32b3e8b0** (Body 32b3e8b0-32b43587, one call site 32b43dbc, 294 calls to UpdateUserUnitStateData `31fd89b0`),
  then `3202b110` (event states), `31fe22d0` (InitStateCommonRandom, 3 strings), `32078bc0`.
- 0x7b4 = **32b46580** (capstone; not a Ghidra function). Frame 0x1e8; the slot index must be 0..4. The skill switch is a nested
  compare / jump-table tree on the skill id returned by `32207210`: jump table 0x7532-0x760c (219-byte index table at 32b4a920, 52 targets,
  **65 ids**), jump table 0x778a-0x778e (5 ids, 3 distinct bodies), compares at 0x7553, 0x765d, 0x765e, 0x765f, 0x7660, 0x7661, plus a
  12-wide table on `[obj+4]-5` (a different value, 8 ids). Roughly **75-85 skill cases**; exact count not derived. Disassembly:
  `ghidra/disasm_32b46580_SetEquippedSkillFunc_capstone.txt`.

## 4. The five cited overrides (all in vtable 334f06fc)
32b4d810 = slot 0x114; 32b4d850 = 0x118; 32b4d9d0 = 0x490; 32b4da10 = 0x494; 32b4d5b0 = 0x458. Also 32b4d990 = 0x48c. Slots 0x434-0x4a4 (29
overrides) all sit in 32b4d3c0-32b4dbe0, the end of the class.

## 5. Unit-type factory
- ctor xref: `3230f470` only. Its xrefs: `322ceeb4` in **`322ced00`** (decompile `ghidra/decomp_322ced00_unitfactory.c`) and `322d5d8f` in an
  undefined-in-Ghidra function **`322d5bf0`** (capstone: same switch; its address is stored in 11 vtables: 3348bfec, 334bb0ec, 334c1b8c, 334c1f8c,
  3352781c, 33527c9c, 33538d9c, 335397b4, 3353a2dc, 3353a68c, 3353a924, so it is a virtual of the game classes).
- In both, the switch value is `**( (*(param_1+0x50)) + 0xaf0 )` (CX2Unit unit type) and **case 10 = UT_LUCIEL** (Enum.src.lua:312) calls
  `3230f470`. Siblings: cases 1-6 = 3230f130 / 1b0 / 230 / 2b0 / 330 / 3b0, 7 = 32bc7970, 8 = 32b72430, 9 = 32a244e0, 0xb = 3230f4f0,
  0xc = 32a6a990.
- Callers of `322ced00`: `322d8bd0` in `322d8bc0`, and `32d51e90` in `32d51e80` (unit creation; funcmap: copies CX2Unit `+0xe40` into `+0x8e90`
  through `3204e030`).
- Unit viewer / character select: **not found**. No other call or pointer reaches the ctor, so the viewer does not build a CX2GULuciel.

## 6. Strings (plain in this binary, not XOR-encrypted)
- Script table: `LUCIEL_DUO.LUA` at 333c1c28, in the 12-name list from 333c1bec (`AIN_ANGEL.LUA`, `ROSE_GUNNER.LUA`, `LUCIEL_DUO.LUA`,
  `ADD_NASODRULER.LUA`, ...). Referenced only as data (337341e0, 33721b88, 337322c8), not by the class code.
- Referenced by the class range: `Motion_Ciel_LDN_Evil.X` (334f0050), `Motion_Ciel_LDN_Evil.x` (334f06b4), `Motion_Lu_LDB_Evil.x` (334f0024),
  `Motion_HAURES1_Ciel_LDN.x` (334efe44), `Motion_HAURES2_Ciel_LDN.x` (334efe10), `__Motion_HAURES2_Ciel_LDN.X` (334efe78),
  `DLG_LUCIEL_BOUNDING_KEY.lua` (334f0fd0, from 32b1ea23 in 32b1e8e0). 1,060 distinct strings are referenced: `ghidra/range_string_refs.tsv`.
  The body motions and meshes (`Motion_Ciel.x` 3345a394, `Motion_Ciel_Emotion*.x`, `Mesh_Lu_Base_SD.X`, `Mesh_Ciel_Base_SD.X`) have no code
  xref in the range: they come from a data table / UnitTemplet.lua.

## 7. Duo members +0x8e88 / +0x8e90 / +0x8e91
Not written in Lu/Ciel's ctor (`ghidra/disasm_32b1bd90_ctor.txt` has none of the offsets) nor anywhere in the class range (capstone: 26 / 872 / 28
reads, 0 writes). They are CX2GUUser members written in the base ctor `3203e850`: `+0x8e88` = 0 at 32040e4e and 32042433 (a refcounted pointer; the
count at +8 is released first); `+0x8e90` byte = `[[this+0x867c]+0xe40]` at 32042471; `+0x8e91` word = 0 at 32042479 (`+0x8e93` byte = 0). Other
writers (capstone whole-image scan, some may be misdecoded): `+0x8e88` 31fe633a, 32762670; `+0x8e90` 3204e07a (setter `3204e030`), 3276331b;
`+0x8e91` 32050e1a, 32092a04.
