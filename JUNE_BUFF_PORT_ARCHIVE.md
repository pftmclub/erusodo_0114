# June 2017 Buff/Debuff Port — archive of finished phase notes

> Moved here from `JUNE_BUFF_PORT_PHASES.md` on 2026-10-04, verbatim and in their original
> order, so the live plan stays small. Each heading below still exists in the plan as a stub,
> and `python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "<heading>"` prints the text from here.
> These are history: kickoff prompts and line numbers inside them are not maintained.

### Phase 4b-i, part 1 — Done (2026-10-03), launch check pending

The scope was too large for one session, so 4b is split three ways:
- **4b-i part 1 (this session):** the behavior parse layer, plus the classes that consume its new floats.
- **4b-i part 2:** the remaining behavior classes, the unit stores and the read sites.
- **4b-ii:** finalizers and displayers, including the Phase 4a review's `BFT_TIME` hand-offs.

All changes are under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY` (`KTDXLIB/Always.h`). Every file is insert-only, the encodings are unchanged, and the March bodies are kept in `#else`. The R3 note (TEMPLET_PARITY needs LOADER) is now in `Always.h`.

**RE method.**
- June's behavior vtables were re-scanned (`buffre/vt.py CX2Buff`) and compared slot by slot with the base `CX2BuffBehaviorTemplet` (`33534be4`).
- The June vtable has 14 slots where March has 8: `[0]` dtor, `[1]` OnFrameMove, `[2]` SetFactor, `[3]` new, `[4]` GetClonePtr, `[5]` GetFactor, `[6]` DoFinish, `[7]` accumulation, `[8]` checksum, `[9]–[11]` new, `[12]` ParsingBehavior, `[13]` SetFactorFromPacket.
- Uncarved slots were read with `rawfn.py`; Ghidra stayed read-only.
- The decompiler hid two pushes. Both `APPLY_TO_SPECIAL_ATTACK` and `USE_CREATOR_UNIT_DATA` push **-1.0** when false, not 0.0 (checked in the asm). Absorb's level slice really does push a leading `1.0` (`FLD1` @`32d47cab`, into the output vector).

**Type-to-function maps (types 1–72; all three match March one-to-one).**
- Factor parse `32d4bbf0`: the same parse function family as March's case list.
- Templet factory `32e80450`: 24 classes, and the type-to-class mapping equals March's `CASE_BEHAVIOR_TEMPLET`.
- SGU `32d4d670`: the same type lists as March (finalizer SGU has an extra case 5, left to 4b-ii).

**Worksheet.** Status values: *same* = diffed, no difference; *ported* = June's version is in; *pending* = not diffed yet, or diffed and not ported, with the reason given.

| Item | June | Status |
|---|---|---|
| Parse: ChangeStat group (6–21, 24–26, 36–42, 44, 52, 72) | `32d445d0` | same |
| Parse: NOW_MP 2, ENCHANT/ATTRIBUTE rate 54/66–71, DOUBLE_ATTACK 56, REFLECT 62, CONSUME_MP 63, EFFECT_SET_WITH_DAMAGE 45, ADD_SKILL_LEVEL 59, ABSORB 23, CUSTOM 43, empty | `32d44320`, `32d46550`, `32d46800`, `32d46d20`, `32d472b0`, `32d45e60`, `32d46aa0`, `32d45570`, `32d45c50`, `32d45b60` | same (June drops some default-branch `DISPLAY_ERROR`s; Phase 3 already captures those at load) |
| Parse: NOW_HP 1 | `32d42030` | **ported**: appends `USE_CREATOR_UNIT_DATA` 1/-1 |
| Parse: CREATE_BUFF_FACTOR 22 | `32d45040` | **ported**: appends `APPLY_TO_SPECIAL_ATTACK` 1/-1 |
| Parse: CHANGE_HYPER_MODE 27 | `32d45860` | **ported**: `HYPER_COUNT` when > 0 |
| Parse: RESET_SKILL_COOL_TIME 34 | `32d46fc0` | **ported**: adds the `SWAP_VALUE` case |
| Parse: CHANGE_ATTACK_BY_TYPE 53 | `32d46000` | **ported**: appends `ADDITIONAL_ATTACK_TYPE` (int, -1); no popup |
| Level slice, whole | `32d47620` | **ported**, every June case including the Phase 5–7 types, so **those phases have no level-slice step left**. The March differences were 1 (an off-by-one plus the creator value), the ChangeStat relation slice (March dropped the relation type), 24–26 (now in the ChangeStat group), 22/53 (March used the default branch and lost values) and 23 (`[1.0, v]`). |
| SGU `SetGameUnitBuffFactor` | `32d4d670` | **ported**: the default branch also records `IsUserUID` |
| SGU NOW_HP | `32d4cc90` | **ported**: creator-unit stat when index 4 > 0 |
| SGU ChangeStat / HyperMode | `32d41700` / `32d41a20` | same |
| Class NowHp `SetFactor` | `32e798d0` | **ported**: no `BCT_PERCENT` restack branch |
| Class NowHp OnFrameMove/GetFactor/packet/ParsingBehavior | `32e72d80`/`32e7d4c0`/`32e79a80`/`32e72a00` | pending (not diffed) |
| Class HyperMode SetFactor/OnFrameMove/packet/DoFinish | `32e7dbb0`/`32e7dae0`/`32e7ddb0`/`32e7de90` | **ported**, except `32b4dd50` (GetFactor `32e7dcf0` same). `32b4dd50` writes a June-only `CX2GameUnit` int at `+0x6844`, set to the count (or 1) at start and 0 at finish. It is **not** March's `SetHyperModeCount`, which sets the gage's orb count, so mapping it there would wipe the player's orbs. Its readers are unmapped: pending, part 2. |
| Class Create SetFactor/GetFactor/packet | `32e860a0`/`32e7d930`/`32e862c0` | **ported**; DoFinish same |
| Class Absorb SetFactor/packet | `32e768a0`/`32e76a00` | **ported** (index 1); GetFactor and DoFinish same |
| **Damage-manager reader of both buff-factor lists** | `326309f0` | **ported in part 2** (see "Phase 4b-i, part 2"; the skipped skill is hyper-active, not type 5). Was: **pending, part 2.** June makes both lists (`+0xe24` permanent, `+0xe34` temporary) `{ptr, bool}`. It skips them on `AT_SPECIAL` attacks unless the bool is set, and when it is set it still skips them for skill type 5. The permanent list's inserts (`322a25f0`, called from `3204f190` with 0 and from `32d1c560` ×3) still need tracing. Until that is done, Create stores and sends the flag, but nothing reads it. |
| Class AttackByType SetFactor/packet/DoFinish | `32e77070`/`32e74740`/`32e747b0` | **ported in part 2.** Was: **pending, part 2.** The unit store `322818d0` gains the additional type (4 args), and its reader is not mapped. The level slice puts the new value last, so March's class still reads indices 0–2 correctly. |
| Class NeverMove ParsingBehavior + slots 1/2/5/6/13 | `32e7a4a0` (keys `IS_GRAVITY`, `CAN_DOWN`, `OFFSET_POSITION`, `DELETE_BUFF_WHEN_DIE`), `32e77000`, `32e744c0`, `32e7e330`, `32e74550`, `32e76e70` | **ported in part 2** |
| Class ResetSkillCoolTime ParsingBehavior + slots 1/2/5 | `32e78e90` (keys `BUFF_CHANGE_TYPE`, `SKILL_TYPE`, `RELATIVE_ALL_SKILL`, `FORCE_SET`, `DRAW_TEXT`, `SKILL_SLOT_TYPE`, `APPLY_ONCE`, plain `FORCE_SKILL_ID`/`IGNORE_SKILL_ID`), `32e85fe0`, `32e76aa0`, `32e7dee0` | **ported in part 2** except DRAW_TEXT's text (no packet override in June) |
| Classes NowMp (new slots 3, 9, 10), ChangeStat (`SetFactor`, slots 3 and 11; does June keep the `ADJUST_BUFF_ACCMULATION_CALCULATE` branch?), EnchantAttackRate (slot 3) | see `vt_buff` | pending, part 2 |
| Classes ReverseLR, Identity, UnitScale, EffectSetWithDamage, Stun, FinishOther, Freeze, CanPass, DoubleAttack, AttackSphereScale, AddSkillLevel, Reflect, ConsumeMpRate, Custom | slots listed by `vt.py` | pending, part 2 (decompiles cached in scratch only; regenerate with `compact.py` / `rawfn.py`) |
| Hand-offs | 1c items 1/2, the 105 note, 1b | pending, part 2: type 56 in both `DamageReact`s; the DASH_IMPOSSIBLE NPC AI gate (`3276e442` switch); the CHANGE_ENCHANT_ATTACK_RATE host math (`32274df0`/`32272d90`); the BBT 7/8 second members plus the variance roll `322788c0` (read 1c item 3's 92 entry first: the second members' basics come from two new stat-block fields, `+0xc`/`+0x10`, via `CX2GameUnit` slots 119/120 `32281690`/`322814a0`) |

**Behavior changes on March's current data.**
- **Skill-level slices change for March data that hits a fixed March bug:**
  - ChangeStat `BCT_RELATION_VALUE` (the relation type now survives);
  - CHARGE_MP_HIT/HITTED with level tables;
  - CREATE_BUFF_FACTOR and ATTACK_BY_TYPE skill factors (the second and third values are no longer lost);
  - NOW_HP relation (index).
  Each of these now gives June's output.
- **HyperMode** now takes the gage's remaining time back every frame, and checks the full-orb flag only on the first `SetFactor`, as June does. June's `+0x6844` write is not ported (see the worksheet).
- **NowHp** stacking for `BCT_PERCENT` now uses the multiplier formula, not March's `ADJUST_BUFF_ACCMULATION_CALCULATE` formula.
- **CreateBuffFactor** now also inserts for NPCs, at the stack count.
- **Live-creator buffs cast by NPCs now last.** `SetGameUnitBuffFactor`'s default branch now records `IsUserUID = false` for an NPC, as June does (`32d4d670`).
  - Before, the default `true` made `CX2BuffLiveCreatorFinalizerTemplet::SetFactor` (`X2BuffFinalizerTemplet.cpp:1379`) look up a *user* with the NPC's UID. The lookup failed, so the buff ended on its first check.
  - Now the buff lasts while the NPC lives.
- **CreateBuffFactor on NPCs, without June's reader.** NPCs now receive created factors, but `326309f0` (which skips them on `AT_SPECIAL`) is not ported. So both NPC-created and user-created factors apply on every attack type until part 2 lands the reader. That matches neither March nor June exactly. It was kept because it is closer to June than March's "never for NPCs".
- **Absorb packet quirk, June's, transcribed verbatim:** `SetFactorFromPacket` reads index 1 while `GetFactor` sends one value, so a packet-built copy gets 0. The packet path is wider than UDP:
  - `X2GameUnit.cpp:14514` restores the player's own buffs from the gage manager (stage change), so **the local player's absorb drops to 0 after a stage change**;
  - `:10654` debuff transit;
  - `X2Game.cpp:17774/17791` join sync.
  - March data has one Absorb entry, in the factor file (none inline). Its normal apply path is fine, because index 1 holds the value both in the level slice `[1.0, v]` and in the raw `[count, v1..]`.
  - **The review must decide** whether to keep this June quirk or restore index 0 for the packet path only.
- **NOW_HP single-level relation slice, June's, transcribed verbatim:** for a skill factor with `BCT_RELATION_VALUE` and one multiplier, June's slice reads 2, 4, 5, 6.
  - Index 3 (the attack value the getter `32e86d20` multiplies, `[3]*[1]*(1-resist)`) becomes `USE_CREATOR_UNIT_DATA`'s +1.0 or -1.0. The relation type is lost, so the SGU never overwrites it.
  - March's own slice gave 0 per tick there. June's gives +/-multiplier, **negative (damage) when the key is absent**.
  - The getters themselves are March's (`SelectDelegate` `32e78e00`: FIX/PERCENT `32e86d80` = `[1]*(1-r)`, RELATION `32e86d20`). The only difference in `SelectDelegate` is that June raises no popup for an unknown type (not ported; pending with the class).
  - Whether any March or June skill factor hits this path has not been counted. **The review must count it** and decide.

**Census.** `buff_census.py --phase 4b` takes the in-type pairs from **41 to 19**:
- **Ported:** `USE_CREATOR_UNIT_DATA`, `APPLY_TO_SPECIAL_ATTACK`, `HYPER_COUNT`, `ADDITIONAL_ATTACK_TYPE` and `SWAP_VALUE`.
- **Excluded:** 26 type/origin/key rows go to the new `NOT_READ_BY_JUNE_TYPE` table, each citing the June reader for that origin (for example `BUFF_RELATION_TYPE` ×206 under DASH_IMPOSSIBLE sits on the empty parse `32d45b60`).
- **Remaining:**
  - behaviors: NEVER_MOVE (`CAN_DOWN`, `DELETE_BUFF_WHEN_DIE`, `OFFSET_POSITION`) and RESET_SKILL_COOL_TIME (`FORCE_SKILL_ID`, `DRAW_TEXT`, `FORCE_SET`, `IGNORE_SKILL_ID`, `SKILL_SLOT_TYPE`), all templet-level and all part 2;
  - finalizers and displayers (4b-ii).

The report now prints each pair's origin split `(templet/factor/inline)`.

**funcmap.tsv:** 23 rows ("Buff Phase 4b").

**Build and deploy.**
- `KTDXLIB`, `X2Lib` and `X2` were built at `US_SERVICE` with rc=0 and 0 errors; both `stdafx.cpp` were touched first. `X2ServerProtocol` was unchanged, so it was not rebuilt.
- A second, incremental `X2Lib` + `X2` pass took out a mapping that was wrong:
  - The first pass mapped June's `32b4dd50` to `SetHyperModeCount`. On `CX2GUUser` that sets the gage orb count, so it would have wiped orbs.
  - The `.obj` (11:44:40) is newer than the source edit (11:40:01).
- `x2.exe` is 19,009,536 B, linked at 11:45:23, md5 `218167fb31d751502c5b8f1fccc90d63`. It contains `USE_CREATOR_UNIT_DATA`, `APPLY_TO_SPECIAL_ATTACK`, `HYPER_COUNT` and `ADDITIONAL_ATTACK_TYPE`.
- It was copied to the game directory as `X2_offline.exe` (name read programmatically) and the md5 matches. **Not launched.**
- **Pairing:** no Lua changes, so this exe needs no pack.
- **Launch check, expected with March data:** the load summary is unchanged from Phase 4a's (`templets 320 loaded, ... ordinal rejects 5`), because no March factor carries the new keys.
- **Things to watch in play:**
  - awakening: HyperMode now re-reads the gage time each frame;
  - recast refresh of percent HP-regen buffs (NowHp stacking);
  - skill buffs whose level slice changed.
- **Flag-off check:** with the flag stripped, `X2BuffBehaviorTemplet.cpp/.h` reduce byte-for-byte to `HEAD`. `X2BuffFactor.cpp` reduces to `HEAD` plus 3 blank lines outside the guards.
- **Review:** required (Opus 5.5), not yet run. Spot-check suggestions:
  - `32d47620`'s ChangeStat relation slice and case 1 (indices 2, 4, 5, 6);
  - the -1.0 pushes in `32d45040`/`32d42030`;
  - `32e7dbb0`'s count/full-orb order.

**Kickoff for 4b-i part 2** (Opus 5.5 high):

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "June buff function map", "Phase 4b" and "Phase 4b-i,
part 1". Branch: mods/june-buff-port. Never launch X2_offline.exe without asking first.

Your task (Phase 4b-i part 2): continue the 4b-i worksheet from its first pending row
(NeverMove), under SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY: the remaining behavior classes,
the 326309f0 damage-manager reader (trace the +0xe24 inserts first), the AttackByType
unit store, and the four hand-offs. Port June's whole function wherever it differs, keep
GetFactor/SetFactorFromPacket order (append only), byte-level CP949 edits, insert-only.
buff_census.py --phase 4b must show no behavior pairs left. Build, deploy, commit;
record where you stopped if the session runs out. Also pick up the 4b-i part 1 review's
hand-offs: re-verify who creates the unit +0x9f4/+0x9f8 effect-set handles that 32296cb0
stops (HyperMode OnFrameMove at 0), and note in Always.h that BEHAVIOR_PARITY needs
SERV_IRUHADEV_JUNE_BUFF_ENUM and ADD_LIVE_CREATOR_FINALIZER (review R2).
```

### Phase 4b-i part 1 — Review (2026-10-03, Opus 5.5 high)

Independent review of `3f51f2b` and `905d514` (`SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`). It used fresh scratch scripts only; none of the 4b session's scripts (`buffre/`, `rawfn.py`, `compact.py`) or the earlier `disasm_range.py` were reused. They were:
- Ghidra decompiles and disassembly, read-only;
- a new PE-section-mapped capstone reader for the functions Ghidra has not carved (`32e798d0`, `32e768a0`, `32e76a00`, `32e7de90`) and for constants and jump tables;
- a `.text` displacement scan for unit `+0x6844/+0x6848/+0x684c` and `+0x9f4/+0x9f8`;
- a new byte-level unifdef for this one flag;
- a fresh `elstools decompile-batch` of March's live `BattleFieldBuffFactor.lua` (353,105 B, equal to the live `data036.kom` entry) and of `Henir_Cube_L.lua`, evaluated under the bundled LuaJIT with stub enums, next to `june_src_fixed`'s factor files.

**Verdict: one code defect (Low), fixed, rebuilt and redeployed.** HyperMode's full-orb check was nested inside the first-`SetFactor` branch, where June and March both run it on every `SetFactor` (R1). Everything else matches June. The three decisions handed to the review are measured below; none of them changes code.

| # | Check | Result |
|---|---|---|
| 1a | `32d47620` level slice | **Pass.** I decoded the jump table myself (`32d48264` bytes → `32d4822c` dwords, `type-1`, 0x68 entries) and mapped the port's labels to numeric values from `X2BuffFactor.h`. All 33 ChangeStat labels (6–21, 24–26, 36–42, 44, 52, 72, 76, 78, 92, 93), the 7 copy-whole types (27, 88, 90, 91, 94, 95, 105) and every other case match June. EBX is zeroed at the loop head (`32d47666`), so each `push ebx` is index 0. Every `31fac820` push has `ECX = esp+0x20` after the argument push, which is the per-iteration output vector at `esp+0x1c`. Constants: `3342e850` = 1.0f, `33439c68` = 30.0, `334355b0` = 1.0, `33437820` = 2.0, `334355a8` = 2^32 (the unsigned fix-up). **Case 1:** type == 2 → count by FISTP under a chop control word, `cmp ebx,1 / jbe` → `[2],[4],[5],[6]`; otherwise `[lvl+1],[n+2],[n+3],[n+4]`. Non-relation: `[2]`, or `[lvl+1]` when count > 1. **ChangeStat:** relation → `[1]`, `[ebx=2]`, then `[3] <= 1.0` (`test ah,0x41`) → `[4]`, else `[lvl+3]`; otherwise `[1] <= 1.0` → `[2]`, else `[lvl+1]`. **0x17:** `FLD1` @`32d47cab` goes into the output vector, then size < 30 → `[0]`, else `[lvl]`. **0x35:** `[1] < 1.0` → 0.0, else `[lvl+1]`; then `[trunc([1]+1)+1]` and `[trunc([1]+2)+1]`. **Default:** `int([0])`, then `[1] < 30` → `[0]`, else `[lvl+1]` (0.0 past the end). June's `32071230` and March's `KBuffBehaviorFactor::operator[]` (`CommonPacket.h:1234`) both return 0.0 past the end on an unsigned compare. |
| 1b | -1.0 pushes | **Pass, in the asm.** `32d42030` @`32d425bb` and `32d45040` @`32d454ea` both `FLD [334380f0]` (= -1.0f) when `32fe9e40` fails or the bool is false, and jump to `FLD1` only on `== 1`. ECX is the factor's own vector (cleaned up from the same slot afterwards). I decrypted the key bytes of `32d42030` independently; they read `USE_CREATOR_UNIT_DATA`, which agrees with `xorstrings.tsv`. Its relation branch reads `MULTIPLIER`, `BUFF_RELATION_TYPE`, a 0, then the flag; FIX and PERCENT read their table only. |
| 1c | `32e7dbb0` HyperMode `SetFactor` | **Count-only-when-0: pass. Full-orb order: FAIL (R1, fixed).** `CMP [ESI+0x10],0 / JNZ 32e7dc30` skips only the count read, and lands on `CMP [ESI+0x10],2 / JA` → slot 40(1), else slot 41 → slot 40(1) if true. So the check runs on **every** `SetFactor`, and slot 41 is asked only when count ≤ 2 (`PGUWO_ORB3` = 2). The port nested it in the `0 == m_uiHyperModeCount` block. **`32b4dd50` is not March's `SetHyperModeCount`: confirmed.** It is a direct, non-virtual call on the unit (`MOV ECX,EDI`). March's `SetHyperModeCount` is pure virtual on `CX2GameUnit`, and `CX2GUUser`'s writes the gage orb count and `syncData`. `32b4dd50` writes one protected int at `+0x6844` (XOR pair `+0x6848/+0x684c`, checksum registration) and nothing else. Besides the buff (`32e7dc69`, `32e7de36`, `32e7ded7` = 0), its callers are `32b1e8e0` (Luciel region, `DLG_LUCIEL_BOUNDING_KEY`; writes 0 at init) and `32bc8030`/`32bc7eae`. Its other `.text` users are the `CX2GameUnit` ctor/dtor region (`322a0f9f`, `322a4e20`) and character-region readers (`32ba6dc1`: `== 1` decides state/skill `0x336`; also `32badd88`, `32bae1c5`, `32bb2980`, `32bc9b3b`, `32bf479b`). It is not `m_iLastStateHyperModeCount` either, which is a plain `int` on `CX2GUUser`. Leaving it out stands; its readers are part 2's. |
| 2 | The rest vs June | **Pass.** **`32d46fc0`:** SWAP 0 / FIX 1 / MULTIPLIER 2 / PERCENT 3 → `ParsingTableForWhile`, anything else fails. March's PERCENT case is behind `ADDED_BBT_ALL_EQUIP_SKILL_SET_OR_RESET_COOLTIME`, which is unconditional at `Always.h:1489`. **`32d46000`:** type (PERCENT → table), `ATTACK_TYPE` required, `ADDITIONAL_ATTACK_TYPE` int, -1 when absent. **`32d45860`:** both keys optional (failure → 0), each pushed only when > 0.0 (`334350e8` = 0.0). **`32d4d670`:** a negative creator UID takes the unit's UID and `+0x264 == 0` (`GUT_USER` = 0), as ported; the lists equal March's for existing types (see correction 2). **`32d4cc90`:** prologue `FLDZ` vs `[4]`, `test ah,5 / jp` = proceed only when `0 < [4]`; game slot 50 `(uid, !isUser)`, falling back to the unit; stat words 7/8 = `m_fAtkPhysic`/`m_fAtkMagic` (after `ExtraStat`'s 6 and `m_fBaseHP`), MIX × 0.5 (`3342ebc8`); BRT 1/2/3 = PHYSIC/MAGIC/MIX. **`32e798d0`** (hand-traced x87 stack): resist = slot 195 / 500.0 (`33458fa8` = `EAR_MAX_VALUE`); started with count c > 0 → `old/(1+m(c-1))`, keep the new one only if `|new| > |that|`, × `(1+mc)`; c == 0 → larger absolute value; not started → new. **`32e7dae0/ddb0/de90`:** take the gage time when > 0, subtract, clamp at 0, set, slot 51, `32296cb0` at 0; the packet reads `[0]` count (with the same full-orb check), and `[1]` when size ≥ 2; DoFinish = my gage 0, unit gage 0, slot 50, `32b4dd50(0)`. **`32e860a0/d930/862c0`:** BFI 0 → false; DidStart → `322a06e0(templet+0x34)`; level = `32369da0` or 1; a user with `GetUnit()` null → false, and `SKILL_ID != 0` → skill level (≥ 1); flag = `size ≥ 3 && [2] > 0`; GetFactor `[BFI, SKILL, ±1]`; the packet uses count `+0x34` or 1, and a user always takes the skill level. **`32e768a0/76a00/7da30`:** started and count > 1 → `+= [1]`, else `= [1]`, then update or insert; the packet reads `[1]`, and GetFactor sends `+0x10` alone. **Wire layout:** only CreateBuffFactor's `GetFactor` changed, and it appends 2 values after March's one; HyperMode, Absorb and NowHp are unchanged. |
| 3 | Census | `buff_census.py --phase 4b` rc 0: **19 pairs, 8 of them behavior** (NEVER_MOVE 3 + RESET_SKILL_COOL_TIME 5). Exclusions spot-checked against every key each cited function reads (XOR strings plus `s_` literals): `32d42030` reads only CHANGE_TYPE/RELATION/USE_CREATOR + FIX/MULTIPLIER/PERCENT, so no `DRAW_TEXT`/`TIME_INTERVAL`. `32d46fc0` has no `SKILL_TYPE`. `32d44320` and `32d46550` have no `BUFF_RELATION_TYPE`. `32e72e20` (NowMp templet) reads CHANGE_TYPE/TIME_INTERVAL/DRAW_TEXT, so no RELATION. `32d45b60` only opens the table, pushes and closes it (callees `32fe9940`/`32129000`/`32fe9a10`). **All hold.** |
| 4 | Hygiene | With the flag undefined, `X2BuffBehaviorTemplet.cpp/.h` reduce byte-for-byte to `3f51f2b^`, and so does the R1-fixed working file. `X2BuffFactor.cpp` differs only by 3 CRLF blank lines. `--numstat 3f51f2b^` shows 0 deletions in every code file (after R1: `X2BuffBehaviorTemplet.cpp` 251/0). All inserted lines are ASCII and CRLF (620 across the code files at `3f51f2b`; the R1 fix moves 2 and adds 3); `file` signatures are unchanged. The `#define` is only at `KTDXLIB/Always.h:4739` and in none of the `.vcxproj`s. |
| 5 | Deploy | Before the fix, both exes were 19,009,536 B, md5 `218167fb31d751502c5b8f1fccc90d63`, with all 5 key strings. After R1 see "Build and deploy" below. |
| 6 | Decisions | See below. |

**Findings**
- **R1, Low (code defect, fixed): HyperMode's full-orb check ran only on the first `SetFactor`.**
  - June (`32e7dc30`) and March (`X2BuffBehaviorTemplet.cpp`'s `#else`) both check every time. The fix moves `if ( PGUWO_ORB3 < count || IsFullHyperMode() ) SetFullHyperMode( true );` below the count block, inside the flag's own inserted code.
  - **Observable effect on March: practically none.** The flag is cleared only in the per-class `HyperModeBuffEffectEnd` (Ara/Chung/Raven/Eve), i.e. at DoFinish, so a re-application while the buff runs finds it already set. It is fixed because it is cheap and the port was the only one of the three that differed.
  - The `SetFactorFromPacket` port already checked every time.
- **R2, Low: flag dependencies.** `BEHAVIOR_PARITY` does not compile without `SERV_IRUHADEV_JUNE_BUFF_ENUM` (the level-slice labels for 73+ are declared in its block of `X2BuffFactor.h`) or without `ADD_LIVE_CREATOR_FINALIZER` (`Set/GetIsUserUID`, `X2BuffFactor.h:772`). Add this to the flag's `Always.h` comment the next time that header is touched; editing it now would force a full PCH rebuild.
- **Info: `32296cb0`.** It stops the unit's effect-set handles at `+0x9f4/+0x9f8`. The writers of those slots (`3204ba71`, `3204bb7c`, `32298c4d`, …) create effect sets with no readable name nearby. **The claim "only June code creates them" was not re-verified.** If part 2 finds that a March effect maps onto those handles, HyperMode's OnFrameMove-at-0 call needs porting.

**Decisions (measured, with a recommendation; no code changed)**
- **6a: Absorb packet path reads `[1]` while `GetFactor` sends one value. Recommendation: keep June's quirk.**
  - **Reach, measured in March.** `FinishAndClearAllBuff` (`X2GameUnit.cpp:12991`) saves your own buffs through `GetFactor` into the gage manager when a stage ends. `SetKBuffFactorFromGageManagerToGameUnit` (`:14514`) restores them through `SetFactorFromPacket` when the next game starts (`X2Game.cpp:1995/2101`, `X2BattleFieldGame.cpp:1511`), in every game type except PvP.
  - **Effect.** A restored Absorb holds `0.0`. It **still absorbs**: `AbsorbEffectAttackProcess` (`X2GameUnit.cpp:12502`) still nulls a reflectable magic hit (`RT_DUMMY_DAMAGE`, `m_bNoDamage`). Only the MP gain becomes `MP+ 0`.
  - **Data.** March has **2** Absorb factors (`BFI_SI_SA_EMK_PHOENIX_TALON`, `BFI_SHINING_BODY`; correction 3). June has 8: Shining Body ×4, Phoenix Talon ×2, and Nichts Feld ×2, whose value is already 0 and so is unaffected.
  - **Not verified.** I did not locate June's own stage-change restore, so it is unconfirmed that June also routes it through `SetFactorFromPacket`. The class slots themselves are confirmed.
  - **Alternative.** Reading `kFactor_[0]` on the packet path only would be a one-line change in `CX2BuffAbsorbEffectAttackBehaviorTemplet::SetFactorFromPacket`, at the cost of a known deviation from June.
- **6b: NOW_HP single-multiplier relation slice. No action needed: no data reaches it.**
  - March's live factor file has 366 factors and 39 NOW_HP `BCT_RELATION_VALUE` entries. Every `MULTIPLIER` has 30 (34 entries) or 33 (5 entries) elements.
  - June has 1,130 factors with 87 such entries (78 × 30, 9 × 33), and `_INT` has 69 factors with 1 entry (× 30). None has a single element.
  - The factor file is the only level-sliced source. Damage-data inline factors parse with `bIsSkill = false` (`X2DamageManager.cpp:3835`) and go straight onto the damage data without a level clone.
  - Only one June entry sets `USE_CREATOR_UNIT_DATA = TRUE` (`BFI_DEBUFF_BLEEDING_REVOLVER_MASTERY`).
  - **No tick turns negative.** Every real multiplier in both files is negative (DoTs), so the hypothetical `-1.0 ×` slice would have produced a small *heal*, not damage (correction 4).
- **6c: CreateBuffFactor inserting for NPCs while `326309f0` is unported. Recommendation: keep it.**
  - **Unreachable on March data.** March has 12 CREATE_BUFF_FACTOR factors: 9 player skills (`SKILL_ID` 4006/1011/1217) and 3 with `SKILL_ID = 0`. Of those three, `PROTECTION_OF_PTAH` comes only from `BuffTemplet.lua` (world buffs), and `HENIR_FIRE/WATER` come only from `Henir_Cube_L.lua`, which targets `GetUserUnit(0..3)` (decompiled). No March script puts any of them on an NPC.
  - **For users nothing changed.** March already applied created factors on every attack type.
  - **Precondition for Phase 13:** part 2's `326309f0` reader must land before June data is adopted. June has 18 CREATE entries, including `MS_DA_UP_PER_GROUP2/3` and `FEAR_OF_ABYSS` (`SKILL_ID` 0 or 210, which look monster-side) and two with `APPLY_TO_SPECIAL_ATTACK = TRUE` (`REASON_PRIDE_OBERON/OPHELIA`).

**Doc corrections**
1. **Worksheet row "Class HyperMode …" and "Behavior changes", HyperMode bullet:** "checks the full-orb flag only on the first `SetFactor`, as June does" is wrong. June checks on every `SetFactor` (R1, now fixed).
2. **Worksheet "SGU … same type lists as March":** true for existing types only. June's `32d4d670` also sends 76, 78, 92 and 93 to the ChangeStat SGU `32d41700`, and 80–84 to `32d4ce60`. Phases 5–7 checklist step 4 has been rewritten to cite that list instead of "if the type uses `BCT_RELATION_VALUE`".
3. **"March data has one Absorb entry, in the factor file":** there are two (Phoenix Talon, Shining Body).
4. **"June's gives +/-multiplier, negative (damage) when the key is absent":** with real (negative) multipliers it would be a heal. No entry in March or June reaches that slice (6b).
5. **"`32b4dd50` … Its readers are unmapped":** its callers and readers are now listed in check 1c; mapping their March equivalents is still part 2.

**funcmap.tsv:** 11 rows added ("Buff Phase 4b-i review"). 8 are confirmed: `32071230`, `32e7da30`, `32e7dcf0`, `31e8c9d0`, `322a06e0`, `32281820`, `32283910` and `32b4dd50`'s readers (`32ba6dc1`). 3 are candidates: `32e78e00`, `32890620` and `32b1e8e0`.

**Build and deploy (R1).**
- Only `X2Lib/X2BuffBehaviorTemplet.cpp` changed (a `.cpp`, so no PCH touch). `X2Lib` and then `X2` were built at `US_SERVICE`: rc 0, 0 errors.
- `.obj` 12:35:16 and `x2.exe` 12:35:39 are both later than the edit (12:34:44).
- `x2.exe` is 19,009,536 B, md5 **`82bfdc8532245b35b5412d25653eb9e7`** (the pre-fix build was `218167fb…`). It was copied to the game directory as `X2_offline.exe`; the names were read programmatically, the md5 matches and all 5 key strings are present.
- **Not launched.** There are no Lua changes, so no pack is needed.
- `253581c`/`5850872` (Phase 1c item 4, committed by another session during this review) touch only the doc and `funcmap.tsv`, so this exe contains no other code change.

### Phase 4b-i, part 2 — progress (2026-10-03), launch check pending

Ran on Opus 5.5 high, under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`. Commits: `05f5230` (NeverMove), `22d63b3` (ResetSkillCoolTime), and the damage reader + AttackByType commit that carries this section. Every code file is insert-only against studio bytes, and the encodings are unchanged.

The flag-off check uses a byte-level unifdef of this flag:
- `X2BuffBehaviorTemplet.h/.cpp` and `X2BuffFactor.h` reduce to `3f51f2b^`;
- `X2GUUser.h/.cpp`, `X2Game.cpp`, `X2BuffTemplet.h`, `X2GameUnit.h/.cpp`, `X2UserSkillTree.h/.cpp` and `X2DamageManager.cpp` reduce to `66a1593`.

The only deletions in `--numstat` are three lines of part 1's own flagged CreateBuffFactor code: the two inserts now pass the flag, and one comment changed.

**The census gate passes.** `buff_census.py --phase 4b` goes from 19 pairs to **11, with no behavior pairs left**. What remains is finalizers and displayers (4b-ii).

**This session found two RE corrections, both in the doc's part 1 text:**
1. `326309f0`'s "skill type 5" is not `ST_BUFF`. The reader passes the skill templet through `32070510`, which maps `m_eType` to a category: 5→6, 6→1, 7/14→2, **13→5**, 15→3, 16→4, otherwise 0. So the excluded skill is `ST_HYPER_ACTIVE_SKILL`.
2. The `+0x9f4/+0x9f8` handles (part 1 review, Info) are **June's data-driven awakening effect sets**. Their writers:
   - `CX2GameUnit` vslot 48, `32298590`, plays `Effectset_TRANScendence_WakeUp_FX` into `+0x9f8`;
   - `CX2GUUser` vslot 147, `3204b5e0`, plays names from data, with `_Add` appended when `GetUnit()+0xaf0` is type 9;
   - `32011f50`, which every character class calls, plays the June-only name member `+0x9578`.

   March plays and stops its awakening effects in the per-class `HyperModeBuffEffectStart/End`, on its own members. No March effect lands in these handles, so HyperMode's OnFrameMove-at-0 call to `32296cb0` has nothing to stop. It stays unported until June's awakening-effect system arrives with the character port.

**Worksheet rows done in this session**

| Item | June | Status |
|---|---|---|
| Class NeverMove, whole | `32e7a4a0`, `32e744c0`, `32e7e330`, `32e76e70`, `32e77000` (new OnFrameMove), `32e74550` | **ported** |
| NeverMove unit store and read sites | `3227abb0`, `31fd8b40` (CX2GUUser vslot 469), `322cc270` (CX2Game), `320506f0`/`320194df` (IGNORE_NEVER_MOVE) | **ported** |
| Class ResetSkillCoolTime, whole | `32e78e90`, `32e76aa0`, `32e85fe0`, `32e7dee0` (GetFactor same) | **ported**, except DRAW_TEXT's text `32e857d0` (logged once; see below) |
| RSCT unit side | slots 160–162 (`31fcd810`, `31fdf340`, `31fdf400`; NPC `321f5590`/`322b55e0`), skill tree `32896600` + `328922f0` + `32894880` | **ported** |
| Damage-manager reader of both lists | `326309f0`, inserts `322a25f0` | **ported** (traced first: `3204f190` passes false; `32d1c560` is Elsword, and only Spell Chain passes "has Magic Coating", June skill 21418, which March lacks) |
| AttackByType class, store and reader | `32e745b0`, `32e77070`, `32e7e440`, `32e74740`, `32e747b0`; `322818d0`; `32274d60` | **ported** |
| `+0x9f4/+0x9f8` re-verification | `32296cb0` | **done**: June-only (correction 2) |
| `Always.h` R2 note | – | **done** in `05f5230` |

**How each port sits on March**

- **NeverMove.**
  - The anchor is `GetPos(true)` + `OFFSET_POSITION`, read as a `D3DXVECTOR3` userdata through `LUA_GET_USER_DEFINED_TYPE_VALUE`.
  - `GetFactor` appends CAN_DOWN and the offset after March's four values.
  - `CX2NeverMoveByBuff` gains June's "can down" byte. **June's store always writes it as false**: `3227abb0` writes `+0x1d = 0` and both callers push 0. So CAN_DOWN is parse-and-send only. `CX2GUUser::CanDownUnderNeverMove` reads the byte, and `CX2Game::KeyProcess` then drops `m_InputData.oneDown` while my unit is anchored.
  - `IGNORE_NEVER_MOVE` is read in `CommonStateStartFuture` (default false). In such a state, `PhysicProcess` moves the anchor to the unit instead of clamping the unit. Only June's `Elsword_SwordMan.lua` uses the key.
- **ResetSkillCoolTime.**
  - It goes through new `CX2GameUnit` virtuals: `ResetLeftSkillCoolTimeByBuff`, `SetResetCoolTimeIgnoreSkillID` and `SetResetCoolTimeForceSkillID`.
  - `CX2UserSkillTree` gets `ResetLeftSkillCoolTimeByBuff`, `IsResetCoolTimeTarget` and `ApplyResetCoolTime`, plus the two lists. March's own `ResetLeftSkillCoolTimeBySkillType` is untouched; only the buff stops calling it.
  - June's 5-slot bars become March's 4 (gage index `i` / `i+4`). `SKILL_SLOT_TYPE` 1/2 match bar A/B; 3 (transcendence) never matches.
  - June's remap of a slot's skill ID (`328905b0`) is not ported; March's `GetUserSkillTemplet` does the lookup.
- **Damage reader.**
  - June keeps `{ptr, bool}` per list entry. Each entry already owns its clone (`GetClonePtr`), so the bool is a new `CX2BuffFactor` member, `m_bApplyToSpecialAttack`. It starts false in all three constructors, and the new insert overloads set it: `CreateAndInsertSkillAndSocketBuffFactorToList(ptr, bool)` and `CreateAndInsertTemporaryBuffFactor(..., bool)`.
  - CreateBuffFactor's `SetFactor` and packet path now pass `APPLY_TO_SPECIAL_ATTACK`.
  - The reader's skill test reads `DamageData::m_iAppliedSkillID`, so **BEHAVIOR_PARITY now also needs `SERV_IRUHADEV_JUNE_DAMAGEDATA`**. That is not yet in the `Always.h` comment: add it the next time that header is touched.
- **AttackByType.**
  - `CX2ChangeAttackByType` gains `m_iAdditionalAttackType` (-1 when none), with a 4-argument constructor and insert.
  - `GetAdditionalAttackByType` is June's whole function.
  - The restack is NowHp's formula, checked in the asm at `32e771a6`–`32e7721f`.
  - `GetFactor` appends the additional type as a fourth value.

**Behavior changes on March data (the review should measure these)**

- **RSCT change types mean different things in June** (`32894880`, raw switch, `cmp 3`):

  | Type | June | March |
  |---|---|---|
  | SWAP | sets the value; without FORCE_SET, only when that lowers it | – |
  | FIX | adds the value, clamped to [0, the skill's cool time] | set (FORCE_SET or lower only) |
  | RELATION | adds cool time × rate | added the value |
  | PERCENT | sets cool time × rate | set cool time × rate |

  Only SWAP and FIX report a change. March's slot-B PERCENT gage index bug (index without `+4`) is fixed by the port.

  **Measured on March's live data (fresh `elstools decompile-batch` of the live `BattleFieldBuffTemplet.lua`/`BattleFieldBuffFactor.lua`, factor file 353,105 B): this is a regression until Phase 13.** June renumbered the types *in its data* alongside the code. For the same BTI, March FIX became June SWAP, and March RELATION became June FIX. So the new exe reads March's numbers with June's meanings:

  | March templet (BFI value) | March type | Meaning with March data on the new exe | June data for the same BTI |
  |---|---|---|---|
  | `BTI_SI_SA_RBM_GIGA_DRIVE` (FIX 1) | FIX | adds 1 s instead of setting 1 s | SWAP |
  | `BTI_BUFF_HARSH_SLAYER_ACTIVE` / `_SPECIAL` (MULTIPLIER -1) | RELATION | the value becomes a rate, so it subtracts the full cool time (no clamp), instead of 1 s | FIX |
  | `BTI_SET_SKILL_COOLTIME_ONCE2` (`BFI_BUFF_FIRE_BLOSSOMS_APPLY2`, -2) | RELATION | subtracts 2 × the cool time instead of 2 s | FIX |
  | `BTI_DEBUFF_EMP_SHOCK` (`_NEAR` 5–13.7, `_FAR` 2.5–6.85) | RELATION | adds 2.5–13.7 × the cool time instead of that many seconds | FIX |
  | `BTI_SET_SKILL_COOLTIME_ONCE` (PERCENT 1 / 0) | PERCENT | unchanged | PERCENT |

  Five of the six March RSCT templets therefore misbehave until June's templet and factor files are adopted (Phase 13), and must ship together with this exe. **Decided (user, 2026-10-03): keep June's semantics as deployed.** The mismatch closes in Phase 13, so no March-semantics switch was added.

  ATTACK_BY_TYPE: March has one factor (`BFI_BUFF_PROTECTION_OF_LAHEL`, 1.2), so the clamp moving from 1 to 0 changes nothing on March data.
- **Every attack type now takes the attacker's buff-factor lists.**
  - March took them only for `AT_NORMAL`. June takes `AT_COMMAND` too, and `AT_SPECIAL` only for flagged entries on a non-hyper skill.
  - So socket, skill and title factors now also apply to `AT_COMMAND` hits. No March damage data uses `AT_COMMAND` until June data is adopted.
  - **This closes 6c's precondition** for Phase 13.
- **AttackByType's product is now clamped at 0, not at 1.** A factor below 1.0 used to do nothing; now it lowers damage of that type. March has none (measured below).
- **AttackByType packets need all four values**, as June's `32e74740` does. A 3-value packet is ignored.
- **NeverMove packets add the offset a second time.** This is June's quirk, like Absorb's: the stage-change restore (`X2GameUnit.cpp` `SetKBuffFactorFromGageManagerToGameUnit`) re-anchors 2× offset away. March data has no OFFSET_POSITION, so it is unaffected today.

**Not ported**
- **RSCT DRAW_TEXT** (`32e857d0`): June floats string ids `0x978f`/`0x9790` in the formats `%s -%d%s`, `%s %d%s` or `%s%d%s` (by change type) at the unit, via `33228fe0`.
  - It is parsed and stored. When it would fire, one `[JUNE_BUFF] unsupported (stored, consumer not ported)` line goes to `ErrorLog.txt`.
  - Porting it needs those two strings in March's string table and the draw call identified.

**Where part 2 resumes (pending, in this order)**
1. The HyperMode `+0x6844` readers (part 1 review 1c: `32ba6dc1` and the others), mapped to their March equivalents.
2. The undiffed NowHp slots: OnFrameMove `32e72d80`, GetFactor `32e7d4c0`, packet `32e79a80`, ParsingBehavior `32e72a00`, and `SelectDelegate`'s missing popup.
3. NowMp (new slots 3, 9, 10), ChangeStat (`SetFactor`, slots 3 and 11), EnchantAttackRate (slot 3).
4. The 14 remaining classes in the worksheet row (ReverseLR … Custom). Their slots are in `vt_buff`; regenerate it with `buffre/vt.py CX2Buff`.
5. The four hand-offs: type 56 in both `DamageReact`s; the DASH_IMPOSSIBLE NPC AI gate; the CHANGE_ENCHANT_ATTACK_RATE host math; the BBT 7/8 second members plus `322788c0`.
6. RSCT DRAW_TEXT (above).

**funcmap.tsv:** 30 rows ("Buff Phase 4b-i part 2"). They supersede the `326309f0` "NOT PORTED" row and the `32296cb0` candidate row.

**Build and deploy.**
- Three passes, each at `US_SERVICE` with rc 0 and 0 errors:
  - NeverMove: KTDXLIB + X2Lib + X2, because `Always.h` changed;
  - RSCT: X2Lib + X2;
  - reader + AttackByType: X2Lib + X2.
- `stdafx.cpp` was touched before each pass.
- Final `x2.exe`: 19,014,144 B, linked 13:40, md5 `ca1bed66e31b3d1e9004661ac71ddbd9`. It contains `IGNORE_NEVER_MOVE`, `FORCE_SKILL_ID`, `ADDITIONAL_ATTACK_TYPE` and `APPLY_TO_SPECIAL_ATTACK`.
- Copied to the game directory as `X2_offline.exe`. The name was read programmatically and the md5 matches. **Not launched.**
- No Lua changed, so no pack is needed.
- **Launch check, expected:** the load summary is unchanged from part 1. **Runtime is not unchanged**: with March data, 5 of the 6 cool-time-reset buffs misbehave (see "Behavior changes"). Giga Drive, Harsh Slayer and Fire Blossoms are player skills; EMP Shock is a debuff.
- **Things to watch in play:**
  - skills that reset cool times (RSCT change-type semantics);
  - any March factor below 1.0 on ATTACK_BY_TYPE;
  - never-move debuffs: the down key is blocked while anchored.

**Play-test (2026-10-03, user):** NeverMove passes on the exe deployed with `c67ccab` (same code plus the offline auth-level setting). `BFI_DEBUFF_ENTANGLE` (69) at level 1 was applied through the dev console: the unit was rooted, the down key did nothing while rooted, and down worked again after it wore off. Level 30 raised two "PERCENT value cannot be 0" popups. That was a data artifact, not the port: Entangle's defence PERCENT table reaches 0 from level 21.

**Kickoff for 4b-i part 3** (Opus 5.5 high):

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "June buff function map", "Phase 4b", "Phase 4b-i, part 1"
and "Phase 4b-i, part 2". Branch: mods/june-buff-port. Never launch X2_offline.exe
without asking first.

Your task (Phase 4b-i part 3): resume the part 2 list "Where part 2 resumes" from item 1
(the HyperMode +0x6844 readers), under SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY: then the
undiffed NowHp slots, NowMp/ChangeStat/EnchantAttackRate, the 14 remaining classes and the
four hand-offs. Port June's whole function wherever it differs, keep
GetFactor/SetFactorFromPacket order (append only), byte-level CP949 edits, insert-only,
flag-off unifdef check per file. The next Always.h touch must add SERV_IRUHADEV_JUNE_DAMAGEDATA
to BEHAVIOR_PARITY's dependency note. Build, deploy, commit; record where you stopped.
```

**Review of part 2:** required (Opus 5.5). Suggested spot-checks:
- `32894880`'s jump table, `32894c3c`: are the cases SWAP/FIX/RELATION/PERCENT in raw order?
- The bool pushed at each `322a25f0` call site.
- `32274d60`'s clamp at 0.
- Count March's RSCT templets by change type (behavior changes, first bullet).

### Phase 4b-i part 2 — Review (2026-10-03, Opus 5.5 high)

Independent review of `05f5230` (NeverMove), `22d63b3` (ResetSkillCoolTime) and `014c8e1` (damage reader + AttackByType), all under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`. `c67ccab` (offline auth level) was checked for hygiene only. The review was read-only on code; nothing was built or deployed.

Fresh tools only:
- Ghidra, read-only. Ghidra carves only the prologue of `32894880` (its body ends at `328948ba`), so the rest was read with a new PE-section-mapped capstone reader.
- A new `.text` scan for `E8` calls to `322a25f0`, `32274d60` and `322818d0`.
- A new byte-level unifdef of the one flag.
- A fresh `elstools decompile-batch` of March's live `BattleFieldBuffTemplet.lua` (152,564 B) and `BattleFieldBuffFactor.lua` (353,105 B). Both sizes equal the live `data036.kom` manifest entries.

**Verdict: no code defect, so there is no numbered fix for part 3.** Every spot-check matches June. One measured behaviour is much worse than the part 2 text says, so it goes back to the user as decision **D1**. It is a data/exe pairing problem, not a port error.

| # | Check | Result |
|---|---|---|
| 1 | `32894880` jump table `32894c3c` | **Pass.** It reads `cmp eax,3 / ja 32894b9c` (default), then `[eax*4+32894c3c]` = `328948f6`, `3289499f`, `32894ab4`, `32894b74`. Raw change types 0–3 are used with no remap, so the order is SWAP/FIX/RELATION/PERCENT (March `BCT_*` 0–3, `X2BuffFactor.h:205`). **Prologue:** `3214f3d0(gage index)` gives the slot type (0–3 → 1, 5–8 → 2, 4/9 → 3, otherwise 0). If `SKILL_SLOT_TYPE` (arg 8) is non-zero and differs, it returns false before anything else. **SWAP:** `FORCE_SET == 1` skips the guard. Otherwise `now <= value` (`test ah,0x41`) returns false and the gage is not touched. Then it writes the slot only if the value differs, writes the gage always, and returns **true even when the value is equal**. **FIX:** `now + value`, clamped below at 0.0 (`334350e8` = 0.0, `test ah,5 / jp`), then clamped to max = `3220f7a0(templet, 32890620(id,0,0,1))`. It returns `new != now`. **RELATION:** `max × rate + now`, no clamp, returns false. **PERCENT:** `max × rate`, returns false. **Default:** 0.0, returns false. `ApplyResetCoolTime` matches every branch. It takes the bar's slot type as an argument (A = 1, B = 2) instead of deriving it from the index; that is the same answer for March's 4-slot bars. |
| 1b | Rest of the RSCT path | **Pass.** **`32e76aa0` SetFactor** (jump table `32e76b74`): 0/1 → cool time `+0x18`; 2 → rate `+0x1c`; 3 → rate, then the 0..1 range check. **`32e85fe0` OnFrameMove:** checks DidStart; slots 161/162 run *before* the APPLY_ONCE gate; slot 160's arguments are `+0x10` type, `+0x18` cool time, `+0x21` FORCE_SET, `+0x14` change type, `+0x1c` rate, `+0x20` RELATIVE_ALL, `+0x48` slot type, in the port's order; DRAW_TEXT runs only on `+0x46` and a true result. **`328922f0` filter:** the force list (`+0x478`) returns true first. Then the type must match unless RELATIVE_ALL (arg 3) is set. Types 12/13 need FORCE_SET (arg 4). The ignore list (`+0x468`) excludes. **`32896600`:** 5 slots per bar; a slot whose ID ≤ 0 is skipped; A then B; the results are ORed; both lists are cleared at the end. **`31fcd810`** guards only on `GetUnit()` (`+0x867c`), as March's own `CX2GUUser::ResetLeftSkillCoolTimeBySkillType` does, so in both versions a reset run on another user's unit would write *my* gage indices (parity, not ported behaviour; not checked whether that path is reached). **`32890620` settled:** arg 2 is "base level only" (`cmp byte [esp+0x10],1`), and the RSCT callers pass 0 (base + bonus). Arg 4 is used only as scratch for the map lookup, and the result is capped at 30 (`0x1e`). That is March's `GetSkillLevel(id)` with `bBasic = false`, so the port's lookup is exact. |
| 2 | `322a25f0` call sites | **Pass.** The `.text` scan finds exactly the four callers Ghidra lists. The bool is arg 2 (`[esp+0x2c]` in `322a25f0`), stored beside the pointer into the `+0xe24` vector (`322b52a0`). MSVC interleaves it: it is pushed before the two arguments of `32089f60`, which pops 8 (`ret 8`), so it survives as `322a25f0`'s arg 2. Values: `3204f8c3` `push 0`; `32d1c8c6` `setg` of `32890620(0x53aa = 21418, 1, 0, 1) > 0`, i.e. the *base* level of Magic Coating; `32d1d291` `push 0` (`32d1d269`); `32d1d2d9` `push 0` (`32d1d2af`). So every March insert on the one-argument overload (false) is right. |
| 3 | `32274d60` clamp | **Pass.** It starts at `FLD1`. Per entry: `cmp edi,[e+0x10] / je` multiply, else `cmp edi,[e+0x20] / jne` skip; it multiplies the protected float at `+0x14`. Tail: `FLDZ / FCOM / test ah,5 / jp`. A product `< 0` returns 0.0; otherwise it returns the product. The port is identical. Store `322818d0` is called only from `32e747a0`, `32e77174` and `32e772d0` (packet, first SetFactor, restack). |
| 4 | NeverMove packet `32e76e70` | **Pass.** `[0..2]` → position. `0 < [3]` → gravity (`+0x1c`). `[4] > 0` → can-down (`+0x1d`, `test ah,0x41`). `size < 7` skips the offset. Otherwise `[5]`, `[6]`, and `[7]` (0.0 when `size == 7`) go into `+0x20/+0x24/+0x28` and are added to the position. Then `3227abb0(identity, pos, gravity, 0)` is called. The port matches, including the size-7 edge and the double offset. |
| 5 | Damage reader `326309f0` (extra) | **Pass.** The attacker is `DamageData+0xc` minus `0x1a0`; null → return. The permanent list `+0xe24` is skipped when reAct `[+0x24]` is 0 or 7 (`RT_NO_DAMAGE`/`RT_DUMMY_DAMAGE`, `X2DamageManager.h:232/239`). **Per entry:** templet = `32206450(APPLIED_SKILL_ID [+0x5c4], 1)`. If the flag is set, the templet is non-null and `32070510(templet) != 5`, the entry is taken. Otherwise it is taken unless `attackType == 1` (`AT_SPECIAL`). The temporary list `+0xe34` uses the same test with no reAct gate. `JuneBuffCanPushAttackerBuffFactor` is the same test, including the null-templet fallthrough. |
| 6 | March RSCT count | **6 templets.** FIX 1 (Giga Drive); RELATION 4 (Harsh Slayer active/special, ONCE2, EMP Shock); PERCENT 1 (ONCE). This matches the part 2 table. **Finalizers:** Giga Drive is `BFT_TIME`, `NORMAL_TIME` 20 at every level, with `FIX_VALUE` 1 at every level. The other five are `BFT_IMMEDIATE`. |
| 7 | Hygiene | **Pass.** **Unifdef:** `X2BuffBehaviorTemplet.cpp/.h` and `X2BuffFactor.h` reduce to `3f51f2b^` byte for byte. `X2GUUser.h/.cpp`, `X2Game.cpp`, `X2BuffTemplet.h`, `X2GameUnit.h/.cpp`, `X2UserSkillTree.h/.cpp` and `X2DamageManager.cpp` reduce to `66a1593` byte for byte. `X2BuffFactor.cpp` is unchanged since `66a1593` (part 1's 3 blank lines remain). **`--numstat 66a1593..c861ebc`:** the only deletions are `014c8e1`'s 3 lines of part 1's own flagged code. Every inserted line is ASCII. Line endings follow the host file: CRLF in CRLF files, and bare LF in the LF-native `X2UserSkillTree.cpp/.h` (20/3002 and 7/537 lines CRLF) and `X2DamageManager.cpp` (278/4216). **`file`** signatures are unchanged for all 17 code files. The flag is defined only at `Always.h:4746` and in no `.vcxproj`. **`c67ccab`:** the flag is at `Always.h:4758`. The unifdef gives `c67ccab^` byte for byte for `Handlers_Login.cpp`, `Handlers_Room.cpp` and `X2OfflineServer.cpp`. `X2OfflineDB.cpp`/`.h` each keep one blank LF line outside the guards (after the `#endif` before `GetCashBalance`, `.cpp:2729`; and before the `////` rule, `.h:764`). These are our own files, so it is benign. The original `XUAL_NORMAL_USER` assignments are kept in each `#else`. |

**D1 — Giga Drive on March data: decision for the user (not a code defect).**
- The part 2 table says "adds 1 s instead of setting 1 s". The real effect is the opposite of the skill's intent:
  - `BTI_SI_SA_RBM_GIGA_DRIVE` lasts **20 s** (`BFT_TIME`).
  - RSCT applies **every frame** while the buff runs (`32e85fe0`; this data has no APPLY_ONCE).
- **March:** each frame lowered every equipped `ST_ACTIVE` cool time above 1 s to 1 s. Actives were effectively on a 1 s cool time for 20 s.
- **New exe:** June's FIX adds 1 s **per frame**, clamped to the skill's maximum.
  - Every equipped active, including ones that were ready, reaches its full cool time within a few dozen frames.
  - It stays pinned there for 20 s.
  - So **Giga Drive locks out every equipped active skill for 20 s.**
- The other five are one-shot (`BFT_IMMEDIATE`), so the table's wording stands for them.
- The user's "keep June semantics" decision was made against the softer description, so it should be re-confirmed.
- **Options:**
  - **(a) Keep it.** Giga Drive stays broken until Phase 13.
  - **(b) Data-side remap.** In March's `BattleFieldBuffTemplet.lua` and `BattleFieldBuffFactor.lua`, give these entries June's own change type *and* value key. Keep the originals in comments. No code change. The edits:
    - Giga Drive: `BCT_FIX_VALUE`/`FIX_VALUE` → `BCT_SWAP_VALUE`/`SWAP_VALUE`.
    - Harsh Slayer ×2, `FIRE_BLOSSOMS_APPLY2` (ONCE2) and `EMP_SHOCK_NEAR`/`_FAR`: `BCT_RELATION_VALUE`/`MULTIPLIER` → `BCT_FIX_VALUE`/`FIX_VALUE`.
    - Templets and factors: recompile with `-v mar-2014` and stage both. The user packs both together (pairing rule).
    - This restores March's behaviour on the new exe, with two exceptions that are June's own: FIX now clamps at the skill's maximum, and the 12/13 exclusion in correction 2.
  - **Recommendation: (b).** It is a 5-entry data edit that June itself made, and Raven's Giga Drive is a normal player skill.

**Doc corrections**
1. **Part 2, RSCT table, Giga Drive row** ("adds 1 s instead of setting 1 s"): the buff lasts 20 s and re-applies every frame. Every equipped active is pinned at its maximum cool time (D1).
2. **Part 2, RSCT table, `BTI_SET_SKILL_COOLTIME_ONCE` "unchanged"** is incomplete.
   - June's filter `328922f0` skips relationship (12) and hyper-active (13) skills unless FORCE_SET is set.
   - So the three RELATIVE_ALL_SKILL resets (ONCE, ONCE2, EMP Shock) no longer touch them.
   - It is reachable: March equips both types in `m_aEquippedSkill`/`m_aEquippedSkillSlotB`, and its own other resets already skip them (`X2UserSkillTree.cpp:2489-2499`).
   - It is **permanent June behaviour**, not a Phase 13 regression, because June's data has no FORCE_SET on any of these BTIs.
   - Two further March templets (Harsh Slayer active/special) are type-scoped, not RELATIVE_ALL. June makes the active one `ST_NONE` + RELATIVE_ALL in its data.
3. **The `SERV_IRUHADEV_JUNE_DAMAGEDATA` hand-off is already done.** `c67ccab` added the note to `BEHAVIOR_PARITY`'s `Always.h` comment. The part 2 "Damage reader" bullet ("not yet in the Always.h comment") and the part 3 kickoff line ("The next Always.h touch must add …") are stale; part 3 should skip it.
4. **`32890620`** moves from candidate to confirmed (check 1b). `funcmap.tsv` has a superseding row.
5. **SWAP reports a change even when the value is unchanged** (`bl = 1` on every SWAP that passes the guard). The port already does this; the part 2 phrase "Only SWAP and FIX report a change" is right but should read "SWAP always, FIX when the value moved".

**funcmap.tsv:** 1 row added ("Buff Phase 4b-i part 2 review"): `32890620`, confirmed.

### Phase 4b-i, part 3 — progress (2026-10-03)

Ran on Opus 5.5 high, under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`. This section records the RE verdicts first (committed before any code); the code, build and stop point follow below.

**Item 1: the HyperMode `+0x6844` readers.**
- A `.text` scan of all three protected-int words (`+0x6844`, `+0x6848`, `+0x684c`) finds no reader in the `CX2GUUser` (`0x3200xxxx`) or `CX2GameUnit` (`0x322xxxxx`) regions. Every reader is character code: Ara (8), Chung (1), Add (1), plus the Luciel writer.
- **June kept March's `m_iHyperModeCountUsed` as a separate member.** `CX2GUUser` vslot 44 (`3208df20`, `GetLastStateHyperModeCount`) reads `+0x8d44`. Both of June's `HyperModeStart` copies (`31fe9706`/`31fe971c`, `31ffd4b8`/`31ffd4ce`) then write the same register into `+0x8d48`. June's Reactive Armor (`32bed453`, skill `0x672e`) switches on `+0x8d48`: 1/2/default → abilities `0x88/0x89/0x8a`, which is March's `X2GUChung.cpp:22663`.
- So `+0x6844` is a new **buff-driven** count. It also covers awakening granted by `HYPER_COUNT` data, which never passes through `HyperModeStart`.
- **Port:** a new `CX2GameUnit` member (`m_iHyperModeCountByBuff`, with `Set/GetHyperModeCountByBuff`). June writes it in three places:
  - SetFactor `@32e7dc69`: `count < 1 ? 1 : count`, after the full-orb check and before `HyperModeBuffEffectStart`. Both orb paths reach the call (`je 32e7dc59`).
  - The packet path `@32e7de36`, the same way.
  - DoFinish `@32e7ded7`: 0, after `HyperModeBuffEffectEnd`.
- The readers are listed in `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7. Ara's vslots 49/50 (`HyperModeBuffEffectStart/End`) exist in March and were reworked in June; they are character-port work.

**Item 2: the NowHp slots.** OnFrameMove `32e72d80`, GetFactor `32e7d4c0`, packet `32e79a80` and ParsingBehavior `32e72a00` are the same as March (each missing key returns false, as March's `LUA_GET_VALUE_RETURN` does). `SelectDelegate` `32e78e00` and the packet path's inlined copy drop March's `DISPLAY_ERROR` for an unknown change type. **Not ported; open decision for the review** (see "Runtime popups" below).

**Item 3: NowMp, ChangeStat, EnchantAttackRate.**
- **The new vtable slots 3, 9, 10 and 11 are reached only from June-only content. Not ported in part 3; they are to be ported dormant** (the user's 2026-10-02 decision: port June's full functions now, absent characters included). A `.text` scan for the call shapes found exactly two callers:
  - `3227c360` `(op, BTI, index, value)` calls vslot 9 (`+=`), 10 (`*=`) or 11 (`=`) on a buff's N-th behavior. Its only callers are Rose's (`32ab2f02`, `32ab2f6d`, `32ae305b`, `32ae30ac`).
  - `3236a430` (`CX2BuffTemplet`) calls vslot 3 on every behavior and then re-runs the displayers. Its only caller is `322a7660` `(BFI, count, level, x)`, which sets an existing buff's stack count (wrapper `322a8a70`). Those are called by Eve, Elesis, Rose, Luciel and Aisha, and by `CX2GUUser` code. Every constant BFI pushed (523, 812, 820, 877, 910, 913, 929) lies in Phase 2's June-only block of `Enum.src.lua`. Aisha's `32cc5c16` passes its BFI in a register (unresolved).
  - Neither is a Lua binding: there are no pointer references.
  - The buff-engine side belongs to this port, to be ported dormant: the overrides, the base defaults, `3227c360`, `3236a430`, `322a7660` and `322a8a70`. It is listed under "Where part 3 stopped". `322a7660` needs the June-only user vslots 476 and 141 mapped first. Only the **callers** go to `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7.
  - For the record: NowMp vslot 3 `32e7c680` recomputes the value from the factor at a given count (`(1 - r) * v[1]`, × `1 + mul * (count - 1)`); vslots 9/10 `32e893a0`/`32e89430` do `+=`/`*=`; ChangeStat vslot 11 `32e86500` does `=`; EnchantAttackRate vslot 3 is `32e775e0`. Base vslot 3 `32e864d0` returns true; base vslots 9–11 `3208ddc0` are empty.
- **ChangeStat `SetFactor` `32e78410`: same as March's `ADJUST_BUFF_ACCMULATION_CALCULATE` branch** (`Always.h:639`). That answers part 1's question: June kept ADJUST in ChangeStat. The x87 selects were traced: both keep the larger absolute value, PERCENT is `1 + (new - 1)(c + 1)` clamped at 0, and the tail updates (`3226fd70`) or inserts (`32282700`).
  - Differences: no popups; a zero multiplier, a change-type mismatch or a relation mismatch returns false silently.
  - June also calls vslot 15 `SetBuffOwnerUID( factor creator UID +0x30/+0x34 )` `@32e784b8`. It is a no-op except in the June-only `_HaveBuffOwner` subclass (BBT 93). Under the no-partial rule it still belongs to this function's port, not to Phase 5: add vslots 14/15 to ChangeStat (`32ec5980` returns 0, `33093ad0` is empty) and the call. It is listed under "Where part 3 stopped".
- **ChangeStat `GetValue` `32e764f0`: ported.** FIX with option data uses `min( level, 90 )` (`cmp eax,0x5a` `@32e7655d`); March does not clamp. Constants 1.5 and 9.5 (`334396c8`/`334b9c80`) are unchanged.
  - The option-type set is a byte table at `32e7664c`: types 11–16, 20, 21 and 72. That equals March's `MustUseOptionData()`, but the port transcribes the set literally.
  - SWAP, RELATION and PERCENT are the same as March.
  - ChangeStat's ParsingBehavior `32e73640`, GetFactor `32e7d740`, packet `32e764a0` and DoFinish `32e73940` are the same.
- **NowMp `SetFactor` `32e7c380`: ported.** June has no `ADJUST_BUFF_ACCMULATION_CALCULATE` PERCENT branch. Every change type restacks with `m / (1 + mul(c - 1))`, keeps the larger absolute value, then multiplies by `1 + mul·c`. It never clamps at 0. This is the same finding as NowHp's in part 1. NowMp's OnFrameMove `32e731a0`, GetFactor `32e7d570`, packet `32e7c8a0` and ParsingBehavior `32e72e20` are the same.
- **EnchantAttackRate: `SetFactor` `32e77370` and packet `32e74b50` ported.**
  - June's per-type attribute table (`32e775c4`: 66..71 → 1, 2, 3, 4, 5, 6) equals March's `BALANCE_PATCH_20131107` switch.
  - When started, March inserts a second entry. June instead restacks: with a count `c`, a zero multiplier returns false; otherwise it erases (`32279c70`) and re-inserts at `v[1]·(1 + mul·c)`, with no resist. With no count, it keeps the larger absolute value, erasing and re-inserting only when the new one is larger.
  - The packet path erases, then inserts with the templet's attribute and change type. March's packet path used the 2-argument insert and lost both. There is no per-type override on the packet path.
  - `32279c70` is a `remove_if` whose predicate (`322a95c0`) matches identity (8 bytes) **and** attribute (`+0x1c`) **and** change type (`+0x28`). It is ported as a new `EraseChangeEnchantAttackRate( identity, attribute, change type )` overload.
  - ParsingBehavior `32e74820`, GetFactor `32e7e520` (`[ct, rate]`) and DoFinish `32e74bc0` are the same.

**Item 4: the 14 remaining classes** (slots dumped from `vt_buff`, `rawfn.py` where Ghidra has not carved them).

| Class | Verdict |
|---|---|
| ReverseLeftRight `32e73370`/`32e733b0`/`32e73400`/`32e73210`/`32e72870` | same |
| Identity `32e73420`/`32e73490`/`32e73470` | same |
| UnitScale `32e73ab0`/`32e73f70`/`32e73bb0`/`32e73b20` | same (the update branch is March's `POSSIBLE_ADD_CHANGE_SCALE_BUFF_VALUE`, `Always.h:2103`) |
| EffectSetWithDamage `32e854a0`/`32e85120`/`32e7c980`/`32e85580` | same in buff logic. Two side notes: `GetPowerRate` is unit vslot 130 with the factor's BTI, which forwards to vslot 129 and drops the BTI on every class except Ain (`32a3d1f0`); and `PlayEffectSet` (`32e851b0`) uses June's wider effect-set signature, which is the effect port's |
| Stun `32e74000`/`32e74090`/`32e74060` | same (`+0x91e5` is March's `SetChangeGroggyState( true )`, `NOT_CANCEL_BBT_STUN_BY_HIT`, `Always.h:2066`) |
| FinishOther `32e7a280`/`32e79ba0` | same |
| **Freeze** `32e740b0`/`32e74120`/`32e76d10`/`32e74170`/`32e76cc0` | **DoFinish ported.** After the erase and `SetStateToNowAnimation( XAS_PLAYING )`, June resumes the animation only if (no gage data, **or** the HP gage exists and `NowHp > 0`) **and** `GetGameUnitState() != GUSI_DIE`; otherwise it plays `XAP_ONE_WAIT`. A gage with a null HP gage takes the `XAP_ONE_WAIT` path. The down transition is unchanged. The other slots are the same. |
| **CanPass** `32e74260`/`32e742b0`/`32e72890`/`32e74320` | **ported.** SetFactor saves the unit's current can-pass flag (unit vslot 68 with 0: the user's `+0x7fc4` copy, which is `m_FrameDataNow` because June's `AnimPlay` clears super-armor at `+0x7ff4` in the same block, as March does in `m_FrameDataNow`). It saves on **every** SetFactor, with no started guard, so a restack saves the buff's own value: June's quirk, transcribed. DoFinish restores it (vslot 233 `SetCanPassUnit`). March had no DoFinish. |
| DoubleAttack `32e77860`/`32e74e00`/`32e74c20`/`32e74db0` | same, except popups. March's SetFactor runs off the end of a `bool` function on a change-type mismatch, where June returns false (listed under popups below). |
| AttackSphereScale `32e79ce0`/`32e79dd0`/`32e7cb70`/`32e79d70` | same |
| AddSkillLevel `32e77970`/`32e74e60`/`32e74e20` | same |
| ReflectMagic `32e77a50`/`32e750a0`/`32e74ed0`/`32e75060` | same |
| **ConsumeMpRate** `32e7cef0`/`32e7e870`/`32e75290`/`32e75100`/`32e7d180` | **differs; pending.** June stores a rate, `1 + v[2]` (restack `+= v[2]`), for users only; it looks up the skill level and then ignores it. March stores an absolute MP cost, `init + init·v[2]` with `init = GetActualMPConsume`. March's only reader, Aisha Teleport (`X2GUArme_VioletMage.cpp:5471`, `FIX_SKILL_BALANCE_AISHA_LENA`), **replaces** the cost with `v[2]`. Porting the class alone would therefore make the Teleport debuff cost `1 + v` MP. June's reader is not in `3203cf60`/`3203a250` (no BBT 63 compare). Lead: map the 30 `.text` hits of skill `0x89a` (June's Teleport ID) to classes, and widen the window in Aisha's (the type test may be a switch). |
| Custom `32e79b00`/`32e73fe0`/`32e73f90` | **Phase 9 hand-off.** SetFactor and the packet path pass June's extra struct (creator UID, is-user, templet `+0xbc`) to `CreateAndInsertCustomFunctionByBuff` (`3229e1a0`), Phase 9's function. Both call sites change with it. |

**Runtime popups: an open decision for the review.** Part 1 ruled popup-only differences "same" because Phase 3 captures them at load. These popups fire at **runtime**, which Phase 3 does not cover, so they are listed rather than ruled:
- NowHp `SelectDelegate` (unknown change type);
- the `SetFactor` mismatch and zero-multiplier popups of ChangeStat, DoubleAttack, ReflectMagic and EnchantAttackRate;
- every class's "GetBehaviorFactor didn't work".

June raises none of them. Functions rewritten whole in this part (NowMp `SetFactor`, EnchantAttackRate `SetFactor`/packet) drop them, as June does.

**Not started in part 3:** item 5 (the four hand-offs: type 56 in both `DamageReact`s, the DASH_IMPOSSIBLE NPC AI gate, the CHANGE_ENCHANT_ATTACK_RATE host math `32274df0`/`32272d90`, the BBT 7/8 second members plus `322788c0`) and item 6 (RSCT DRAW_TEXT).

**Code (`061eb71`).** Everything is under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`, insert-only, with the March bodies kept in `#else`.
- `X2GameUnit.h/.cpp`:
  - `m_iHyperModeCountByBuff` (`KProtectedType<int>`, default 0) with `Set/GetHyperModeCountByBuff`;
  - the 3-argument `EraseChangeEnchantAttackRate`;
  - two new virtuals, `AnimPlayByPlayType` (default empty) and `GetCanPassUnitNow` (default false).
- `X2GUUser.h/.cpp`:
  - `AnimPlay()` replays each skin anim's own play type, where March forced `XAP_LOOP`;
  - `AnimPlayByPlayType` is new;
  - `GetCanPassUnitNow` reads `m_FrameDataNow.stateParam.bCanPassUnit`.
  - Freeze is March's only `AnimPlay` caller (grep of `X2Lib`/`KTDXLIB`/`X2`, all file types, plus the bind tables and Lua).
- `X2GUNPC.h`: `AnimPlay()` now plays `m_pXSkinAnim` (March's was empty); `AnimPlayByPlayType` and `GetCanPassUnitNow` (`m_NPCFrameData`) are new.
- `X2BuffBehaviorTemplet.h/.cpp`:
  - the HyperMode wiring (part 1's three "not ported" comments become calls);
  - NowMp `SetFactor`;
  - ChangeStat `GetValue`;
  - CanPass `SetFactor`, plus the new `DoFinish` and `m_bCanPassUnitBeforeBuff`;
  - Freeze `DoFinish`;
  - EnchantAttackRate `SetFactor` and `SetFactorFromPacket`.
- `Always.h` was not touched. The part 2 review had already added `SERV_IRUHADEV_JUNE_DAMAGEDATA` to the flag's dependency note.

**Hygiene.**
- **Flag-off unifdef** (a fresh byte-level script for this one flag): each file reduces byte-for-byte to its base. `X2BuffBehaviorTemplet.cpp/.h` reduce to `3f51f2b^`; `X2GameUnit.h/.cpp` and `X2GUUser.h/.cpp` to `66a1593`; `X2GUNPC.h` to `b553529`.
- **Diff shape:** `--numstat` shows 322 insertions. The 7 deletions are part 1's own flagged comments: the two copies of the 3-line "not ported" note, and the DoFinish note.
- **Encodings:** the `file` signatures are unchanged.

**Build and deploy.**
- `X2Lib` (with `stdafx.cpp` touched) and `X2` were built at `US_SERVICE`, rc 0, 0 errors.
- The only warning in the touched files is the existing C4018 at `X2GUUser.cpp:17854`, above the insertion.
- A second incremental `X2Lib` + `X2` pass followed a whitespace-only move of one blank line inside the guard.
- `x2.exe` is 19,015,168 B, linked 16:47:58, md5 `57f7cf7475883d957a0e37a251ffbfd7`.
- It was copied to the game directory as `X2_offline.exe` (the name was read programmatically); the size and md5 match. **Not launched.**
- No Lua changed, so no pack is needed.
- **Census:** `buff_census.py --phase 4b` is unchanged. There are no behavior pairs; only finalizers and displayers remain (4b-ii).

**How far each port reaches on March's live data.** Counted on the earlier session's `elstools decompile-batch` of the live `BattleFieldBuffTemplet.lua` (factor file 353,105 B).
- **EnchantAttackRate: 7 templets.**
  - BBT 54: `BTI_BUFF_PROTECTION_OF_ZACHIEL`, `BTI_COMMON_AURA_EL_DEFENCE_ACCEL` (FIX), `BTI_BUFF_ANNIHILATION_WAY_OF_SWORD`, `BTI_BUFF_INCREASE_ENCHANT_ATTACK_RATE`.
  - Attribute types: `BTI_BUFF_SWORD_ENCHANT_FIRE/ICE/POISON` (FIX, 66/67/68).
  - All have `ACCUMULATION_LIMIT = 1`.
  - Re-applying any of them while it runs used to insert a second store entry. `DoFinish` erases only the first entry with that identity, so March kept a stale enchant-rate bonus after the buff ended.
  - The stack count only increments when `ACCUMULATION_LIMIT > 1` (`X2BuffTemplet.cpp:218`), so these recasts take the no-count path. Now the larger rate replaces the entry. Sword Enchant re-casts are the case to watch in play.
- **Freeze: 6 templets.** `WATER_HOLD`, `FROZEN`, `SHADOW_KNOT`, `SHOCK` and `SLESHU_ONE_SLASH` have `IS_DOWN = TRUE`; `BIND` has `IS_DOWN = FALSE`. A unit that dies while frozen now holds its last frame (`XAP_ONE_WAIT`) instead of looping. A live user resumes the play type it had, not `XAP_LOOP`.
- **CanPass: 1 templet,** `BTI_NPC_ALCHEMYST_SECRET_BOSS_TRANSFORM_SCALE`. Its pass-through flag is now restored at finish, where March left it until the next state change.
- **NowMp: no reach.** 28 templets, 5 of them PERCENT: `MANA_BURN_REL` (limit 0), `DARK_POISON`, `RELATION_SHIP_SKILL`, `MAGIC_POINT_IMMEDIATELY_CHANGE_ONCE`, `IMMEDIATELY_CHANGE_HP_MP` (limit 1). The stack count only increments when `ACCUMULATION_LIMIT > 1` (`X2BuffTemplet.cpp:218`, unconditional, no Phase 4a variant), so the removed ADJUST branch, which needs a stack count, is never reached on March data.
- **ChangeStat `GetValue`: only units above level 90.** That is above March's cap, but it matters for the Atlas/Elysion cap-99 worktree.
- **HyperMode store: 2 templets** (`BTI_HYPER_MODE`, `BTI_FORMATION_MODE`). Nothing in March reads the value.
- **ConsumeMpRate (not ported): 1 templet,** `BTI_DEBUFF_CHANGE_CONSUME_MP_RATE` (PERCENT), the Teleport debuff.

**Things to watch in play:** recasting Sword Enchant (Fire/Ice/Poison) and the BBT 54 buffs; deaths while frozen; the Alchemyst secret boss transform; awakening, whose effects are unchanged but the new count is written.

**Play-test (2026-10-03, user): all five checks pass** on the exe deployed with `061eb71` (md5 `57f7cf74…`). Each buff was applied with the dev console, `g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( <BFI>, 1 )`, inside a dungeon:
1. Sword Enchant Fire `163` re-applied while running, then Ice `162`: no duplicate, popup or crash; procs stop after expiry, and Ice replaces Fire.
2. Bind `73` on self, and Ice-frozen monsters: the animation resumes after thaw; a monster killed while frozen holds its pose.
3. Alchemyst transform `28`: pass-through while it runs, collision back after it ends.
4. Awakening, normal and potion `66`: unchanged.
5. One full dungeon clear: no new popup or crash.

**Where part 3 stopped / what resumes (in this order).**
1. Item 5, the four hand-offs:
   - type 56 in both `DamageReact`s;
   - the DASH_IMPOSSIBLE NPC AI gate (`3276e442` switch);
   - the CHANGE_ENCHANT_ATTACK_RATE host math (`32274df0`/`32272d90`; `32272d90` is the function with the `0x89a` compare at `32272e48`);
   - the BBT 7/8 second members plus `322788c0` (read Phase 1c item 3's 92 entry first).
2. Item 6, RSCT DRAW_TEXT (part 2, "Not ported").
3. ConsumeMpRate: find June's reader of the rate, then port the class and Aisha's Teleport reader together.
4. The runtime-popup decision above. **It is the user's:** the no-partial rule says to port June's removals, while part 1's precedent ruled popup-only differences "same".
5. Port dormant: behavior vslots 3/9/10/11 (base defaults and the NowMp, ChangeStat and EnchantAttackRate overrides), `3236a430`, `322a7660` (map user vslots 476 and 141 first), `322a8a70` and `3227c360`.
6. ChangeStat vslots 14/15 and the `SetBuffOwnerUID` call in `SetFactor` (`@32e784b8`).

**Review of part 3:** required (Opus 5.5). Suggested spot-checks:
- `32e77370`'s `c == 0` compare direction (`test ah,0x41` after `fcompp`);
- `32e76d10`'s two null tests (gage, then HP gage);
- `32e7c380`'s restack (no PERCENT branch);
- `32e74260` saving on every `SetFactor`;
- whether the slot-3 chain really has no March-data caller (Aisha's register-passed BFI at `32cc5c16`).

**Kickoff for 4b-i part 4** (Opus 5.5 high):

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "June buff function map", "Phase 4b", "Phase 4b-i, part 1",
"Phase 4b-i, part 2" and "Phase 4b-i, part 3". Branch: mods/june-buff-port. Never launch
X2_offline.exe without asking first.

Your task (Phase 4b-i part 4): resume part 3's list "Where part 3 stopped" from item 1 (the
four hand-offs: type 56 in both DamageReacts, the DASH_IMPOSSIBLE NPC AI gate, the
CHANGE_ENCHANT_ATTACK_RATE host math, the BBT 7/8 second members plus 322788c0), then RSCT
DRAW_TEXT, ConsumeMpRate's June reader, the dormant vslot 3/9/10/11 chain and ChangeStat's
vslot 14/15 owner-UID hook (items 5-6), under SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY.
Port June's whole function wherever it differs, keep GetFactor/SetFactorFromPacket order
(append only), byte-level CP949 edits, insert-only, flag-off unifdef check per file.
Another session may have uncommitted edits in the doc: stage only your own hunks. Build,
deploy, commit; record where you stopped.
```

### Phase 4b-i part 3 — Review (2026-10-03, Opus 5.5 high)

Independent review of `b553529`, `061eb71`, `06521c3`, `00583b2` and `67bbb82`. The code is `061eb71`, under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`; no code file changed after it. The review was read-only on code: nothing was edited, built or deployed.

Fresh tools only (part 3's `bp.py`, `p3_code.py` and `unifdef_check.py` were not reused):
- Ghidra, read-only (DATA xrefs and function lookup).
- A new PE-section-mapped capstone reader for every function body below. New `.text` scans: `E8`/`E9` calls to the deferral functions, `push 0x3f`, `cmp r32,0x3f`, byte stores at `+0x8040`/`+0x8080`, and `mov r,[r+0x264]` vslot loads.
- A new byte-level unifdef of the one flag.
- A fresh `elstools extract -v mar-2014` of the live `data036.kom`, then `decompile-batch` of `BattleFieldBuffTemplet.lua` (152,564 B) and `BattleFieldBuffFactor.lua` (353,105 B). Both sizes equal the live manifest entries.

**Verdict: no code defect.** Every spot-check and every item-2 function matches June in the asm. One comment in the port names the wrong offset (R1). It was not fixed, because a comment edit would re-link a new exe for no behaviour change. Part 3's RE text has three errors, all in the deferrals and none in code (see "Doc corrections"). **June's ConsumeMpRate reader is found** (check 5). The runtime-popup question is still the user's and is not decided here.

| # | Check | Result |
|---|---|---|
| 1a | `32e77370` EnchantAttackRate `SetFactor` | **Pass.** The change type is `ftol(v[0])` against `+0x10`; a mismatch returns false. Attribute jump table `32e775c4` (index type − 0x42): 66→1, 67→2, 68→3, 69→4, 70→5, 71→6, i.e. `BEAT_BLAZE`…`BEAT_SNATCH`. The port's labels resolve to 66–71 in `X2BuffFactor.h`. Type 54 leaves `+0x18` alone. Both branches then require change type 3 or 1. **Not started:** `m_fRate = v[1]` (0.0 when size ≤ 1), then insert `32281990(identity, rate, attr, ct)`. **Started, count c > 0:** `31c7b640(m, 0.0)` is `\|a − b\| <= FLT_EPSILON` (`334490f8`, the same test as March's `IsSamef`). True → return false: that is the zero-multiplier exit. Next comes erase `32279c70(identity, attr, ct)`, **before** the new value is computed; then `v[1] × (1 + m·c)` (count with the unsigned fix-up; `334355b0` = 1.0), store, insert. **c == 0:** st0 = \|new\|, st1 = \|old\|; `fcompp / test ah,0x41 / jne` leaves when \|new\| ≤ \|old\| (C0 or C3). So only a strictly larger \|new\| erases, stores and inserts. The port matches in every branch. It tests FIX/PERCENT once before `DidStart()`, where June tests in each branch; the result is the same. |
| 1b | `32e76d10` Freeze `DoFinish` | **Pass.** Sequence: `3227a470(identity)` (erase), then vslot 238(1) = `SetStateToNowAnimation( XAS_PLAYING )`. **Gage data** `+0xc04` null → straight to the state test. **Non-null:** HP gage `+0x20` null → `XAP_ONE_WAIT`. Otherwise the protected NowHp is decoded and tested with `fldz / fcomp / test ah,1 / je`: `0 < NowHp` continues, anything else → `XAP_ONE_WAIT`. **State test:** vslot 93 `== 4` (`GUSI_DIE`) → `XAP_ONE_WAIT`, else vslot 231 (`AnimPlay`). `XAP_ONE_WAIT` is vslot 232 with 2, and `XAP_ONE_WAIT` = 2 (`KTDGXSkinAnim.h`). **Down:** `+0x14 == 1` and unit `+0x264 == 0` (`GUT_USER`); vslot 128 (riding) → `StateChange(0x55, 1)`, else `(0x1e, 1)`, March's. The port's `GetNowHp()` returns 0.0 for a null HP gage (`X2GageData.h:200`), so that case takes `XAP_ONE_WAIT`, as in June. |
| 1c | `32e7c380` NowMp `SetFactor` | **Pass.** There is no PERCENT branch: the only change-type tests are the mismatch exit and the delegate bind (FIX(1) → `32e86f10`, PERCENT(3) → `32e86f20`, others unbound). There is no clamp: no `fldz` compare on the result anywhere. The value is `(1 − r)·v[1]` for every type, with r = vslot 195 / 500.0, which is March's `GetValueOrPercentPerSecond`. **Not started** → the value. **Started, c == 0** → `\|new\| > \|old\| ? new : old` (`test ah,5 / jp`; a tie keeps old). **c > 0** → `old / (1 + m(c − 1))`, the same larger-absolute pick, then `× (1 + m·c)`. The XOR writes are the protected float plus the anti-tamper registry `3376cdd0`, which is not ported, as in earlier parts. |
| 1d | `32e74260` CanPass `SetFactor` | **Pass.** `GetBehaviorFactor` fails → false. Otherwise, for a non-null unit, `+0x11 = vslot 68(0)` and return true. There is no `DidStart` test, so it saves on every call. DoFinish `32e72890`: for a non-null unit, vslot 233 (`SetCanPassUnit`) with `+0x11`. **Vslot 68's argument 0 is the now frame.** User `3208e580` reads `+0x7fc4` for 0 (`+0x7d24` for 1). `+0x7fc4 + 0x30` = `+0x7ff4`, the protected `bSuperArmor` that `AnimPlay`/`AnimStop` write. March's `StateParam` puts `bSuperArmor` exactly 0x30 after `bCanPassUnit` (`KProtectedType<bool>` 0xc + `PROTECT_VECTOR2` 0x18 + 0xc), and March's `AnimPlay` writes `m_FrameDataNow`. **NPC:** vslot 68 `32755c50` ignores the argument and reads `+0x8344`, the field NPC vslot 233 `32755e90` writes. So `m_NPCFrameData.stateParam.bCanPassUnit` is right. |
| 2a | HyperMode's three `32b4dd50` calls | **Pass.** SetFactor `@32e7dc69`: `mov eax,[esi+0x10] / cmp eax,1 / jae` → the count, else 1 (unsigned). It comes after the full-orb block and before the `v[0]` time add, the gage write and slot 49. Packet `@32e7de36`: the same, before `[1]` is read. DoFinish `@32e7ded7`: `push 0`, after slot 50. The port calls at the same three points. `m_iHyperModeCountByBuff` is a `KProtectedType<int>`, whose default constructor zeroes it. |
| 2b | `32e764f0` ChangeStat `GetValue` | **Pass.** An outer jump table `32e76634` dispatches change types 0–3. **SWAP:** `v[1]`. **FIX:** byte table `32e7664c`, indexed by type − 11 over 62 entries. It picks `32e76644[0]` (the option path) for 11–16, 20, 21 and 72, and `[1]` (`v[1]·(1 − r)`) for the rest; types outside 11..72 also take `[1]`. **Option path:** vslot 131 (unit level), `cmp eax,0x5a / jle` (signed), else 90. Then `(L − 1)` as unsigned (`jns` plus the 2^32 fix-up) × 1.5 + 9.5 (doubles `334396c8`/`334b9c80`) × `v[1]` × (1 − r). **RELATION:** `v[3]` (0.0 when size ≤ 3) × `v[1]` × (1 − r). **PERCENT:** `v ≥ 1` → v; `v ≤ 0` → v; otherwise `1 − (1 − v)(1 − r)`. **Above 3:** 0.0. The port's labels resolve to 11–16, 20, 21 and 72; `FO_VALUE`/`FO_MULTIPLIER` are 1 and 3. |
| 2c | `32e74b50` EnchantAttackRate packet | **Pass.** `size < 2` → return. `+0x10 = ftol(v[0])`, `+0x14 = v[1]`. Then erase `32279c70(kBuffFactor + 0x20, +0x18, ct)` and insert `32281990(identity, rate, +0x18, ct)`. The attribute is whatever the templet holds: ParsingBehavior `32e74820` writes `+0x18` from `ENCHANT_ATTRIBUTE_TYPE`, as March's `:3327` does. No per-type switch runs on this path. |
| 2d | 3-argument erase vs `32279c70`/`322a95c0` | **Pass.** `32279c70` builds a 16-byte predicate {identity (8 bytes), attribute, change type} and calls `322aaa30`. That is `find_if` (`322a95c0`) followed by `322a9450` on the tail, i.e. `remove_if`; `3207b850` then erases `[result, end)`. So **every** match is erased, not just the first. `322a95c0` compares entry `+4`/`+8` (BTI and unique number, as March's `CX2BuffIdentity::IsSame`), the protected `+0x1c` (attribute) and `+0x28` (change type). The port's loop erases every match. |
| 2e | User/NPC `AnimPlay` | **Pass, with R1 (comment).** **User vslot 231 `31fe8130`:** plays `+0x87d0`/`+0x87d4` with each anim's own play type (`[anim+0x2a8]`) and the June-only `+0x8e84`/`+0x8e88`. It then clears the protected `+0x7ff4` and the plain bytes `+0x8080` and `+0x8040`. Vslot 232 `31fe8050` is the same with the argument. **NPC `32757b40`/`32757b20`:** `+0x8604` only, with no state writes. **`+0x8040` is `bSuperArmorNotRed`:** `.text` has 69 immediate byte stores to `+0x8040` across character code, where March writes `bSuperArmorNotRed` (Ara, Aisha, Chung, …). `+0x8080` is written only by the three Anim functions and read only by user vslot 153. So the port's two clears are exactly June's March-side writes, and the skipped writes are June-only members. **Callers:** Freeze `DoFinish` is still the only `AnimPlay` caller (grep of `X2Lib`, `KTDXLIB` and `X2`), so NPC `AnimPlay` turning from a no-op into a play reaches only frozen NPCs. |
| 3 | Three "same" classes, wire layout | **Pass, all three.** Vtables come from Ghidra DATA xrefs (UnitScale `33534ddc`, DoubleAttack `33534f80`, ReflectMagic `33534ff8`); slot 5 is GetFactor and slot 13 the packet path. **UnitScale:** `32e7df70` pushes `{type}` with no values; packet `32e73b20`, when not started, calls `32281bb0(identity, +0x10 scale)`. **DoubleAttack:** `32e7e5d0` makes exactly two `31fac820` pushes, `(float)+0x10` then `+0x14`; packet `32e74db0` requires `size ≥ 2`, sets ct = `v[0]` and rate = `v[1]`, then calls `32281a50(identity, rate)`. **ReflectMagic:** `32e7e7c0` sends `[(float)ct, value]`; packet `32e75060` reads `v[0]`/`v[1]` with no size check and calls `322822e0(identity, value)`. All three equal March (`X2BuffBehaviorTemplet.cpp:2280/2290`, `:3598/3612`, `:3884/3897`). There is no hidden push. |
| 4 | Deferrals reached only from June-only content | **Pass on reach.** The part 3 list was incomplete (correction 1). **Scan** (`E8`/`E9` plus raw pointer search): `3227c360` has 4 callers, all pushing BTI 703 (`BTI_BUFF_OVERSTRIKE_REINFORCE`, June-only); `3236a430` has 1 (`322a77ba`, inside `322a7660`); `322a7660` has 13; `322a8a70` has 9. None of the four has a pointer reference. **Constant BFIs:** 517, 523, 548, 549, 737, 763, 812, 820, 841, 877, 910, 913, 923, 929 and 946. None exists by name or value in March's original Enum (`83ce6bc`) or in the pre-Phase-2 Enum (`2068485^`). `322a8a70`'s argument 1 is a BFI (it calls `326c32a0`). **Aisha `32cc5c16`, resolved.** The BFI is the first entry of skill templet `+0xb0` for skill `0x578a` = 22410, `RNSI_P_ADW_MANAFLOW_OVERDRIVE`. That is June's renewal skill; March's Manaflow Overdrive is `SI_P_ABM_MANAFLOW_OVERDRIVE` = 2414. 22410 is in neither March Enum, so `32206450` returns null and the function exits before the call. The BFI's name is not resolved: `NewSkillTempletVer2.lua` is not in `june_src_fixed`, and its bytecode yields no plain string. The other skill-keyed sites use 25417 (`RNSI_P_EEP_OPEN_MIND`) and 30015 (`RNSI_SA_LLD_PHANTOM_BLADE`), also June-only. **Not re-derived:** part 3's call-shape claim that vslots 3/9/10/11 have no caller besides `3236a430`/`3227c360`. Only those two functions' own callers were re-scanned. |
| 5 | ConsumeMpRate | **Leaving it unported was right. June's reader is found.** See "ConsumeMpRate: June's reader" below. |
| 6 | Hygiene | **Pass.** **Unifdef:** with the flag undefined, `X2BuffBehaviorTemplet.cpp/.h` reduce to `3f51f2b^`, `X2GameUnit.h/.cpp` and `X2GUUser.h/.cpp` to `66a1593`, and `X2GUNPC.h` to `b553529`, all byte for byte. Each raw file differs from its base, so the check is not vacuous. **`--numstat 2fc0ebb 061eb71`:** the code totals 322/7. All 7 deletions are in `X2BuffBehaviorTemplet.cpp`, and all are part 1's own flagged comments: the 3-line "not ported" note twice, and the DoFinish note. **Inserted lines:** 322, all ASCII and all CRLF. **`file`** signatures are unchanged for all seven files. **Deploy:** the game directory holds `x2.exe` and `X2_offline.exe` (names read programmatically). `X2_offline.exe` is 19,015,168 B, dated 16:48:59, md5 `57f7cf7475883d957a0e37a251ffbfd7`, equal to `X2/US_SERVICE/x2.exe` (16:47:58). |
| 7 | March-data reach | **Reproduced; every number matches part 3.** 321 templets. **EnchantAttackRate 7:** BBT 54 ×4 (`PROTECTION_OF_ZACHIEL` PERCENT, `COMMON_AURA_EL_DEFENCE_ACCEL` FIX, `ANNIHILATION_WAY_OF_SWORD` PERCENT, `INCREASE_ENCHANT_ATTACK_RATE` PERCENT) plus Sword Enchant Fire/Ice/Poison (66/67/68, FIX); all limit 1; none for 69–71. **Freeze (`BBT_FROZEN`) 6:** `WATER_HOLD`, `FROZEN`, `SHADOW_KNOT`, `SHOCK` (limit 0) and `SLESHU_ONE_SLASH` have `IS_DOWN` TRUE; `BIND` has FALSE. **CanPass 1.** **NowMp 28, 5 of them PERCENT:** `MANA_BURN_REL` (limit 0); `DARK_POISON`, `RELATION_SHIP_SKILL`, `MAGIC_POINT_IMMEDIATELY_CHANGE_ONCE` and `IMMEDIATELY_CHANGE_HP_MP` (limit 1). No NowMp templet has a limit above 1, so the removed branch's zero reach holds. **HyperMode 2. ConsumeMpRate 1** (PERCENT). |

**ConsumeMpRate: June's reader (check 5)**
- **`32039a50(skillID, BBT)` → float, default 1.0.**
  - It builds the unit's `KBuffFactor` list with `GetBuffFactor` (`3229fd80`: every live templet at `+0xe14`, through `GetFactor`).
  - It walks each behavior factor of the given type and takes `v[2]` when `int(v[1]) == 32207210(skillID)`, or when `v[1] == 0` and `32070580(skill templet)` holds. The last match wins.
  - `32207210` returns the skill templet's `+0x178`, else the ID itself. `32070580` is "skill type ≠ 12 and ≠ 13", so `v[1] == 0` means every skill except relationship and hyper-active.
- **What `v[2]` is.** June's `GetFactor` `32e7e870` sends `[(float)ct, skill, rate]`, where rate is the stored `1 + v`. So `v[2]` is the multiplier.
- **Callers** (each pushes `0x3f`):
  - `3203cfe1` in `CX2GUUser::GetActualMPConsume` `3203cf60` (confirmed in Phase 1c). It multiplies vslot 471's skill rate by `32039a50(skill, 63)` for **every** skill, not only Teleport.
  - Two character-code sites, `329a823d` and `329b20c4`.
  - Part 3 ruled `3203cf60` out because it has no literal BBT 63 compare. The type is an argument to the helper.
- **Why the class alone would be wrong.** March's Teleport reader (`X2GUArme_VioletMage.cpp:5471`, `FIX_SKILL_BALANCE_AISHA_LENA`) *replaces* the cost with `v[2]`. With June's class, `v[2]` would be 1.5, so Teleport would cost 1.5 MP.
- **Port bundle (part 4).** Port these together:
  - the class: `SetFactor` `32e7cef0`, `GetFactor` `32e7e870`, and the packet, DoFinish and ParsingBehavior slots;
  - `32039a50`, with `3229fd80`'s filter;
  - the multiply in March's `GetActualMPConsume`;
  - March's Teleport override, disabled under the flag.

  March has no `+0x178` field, so `32207210` falls back to the skill ID. The two character sites go to `JUNE2017_CHARACTER_PORT_CHECKLIST.md`.
- **Effect on March data.** There is one factor, `BFI_DEBUFF_TELEPORT_CONSUME_MP`: SKILL_ID 2202, PERCENT 0.5 at every level.
  - March: the cost becomes 1.5 × the cost computed when the debuff started.
  - June: 1.5 × the live cost, on every call.

**Findings**
- **R1, Low (comment only; not fixed).** `X2GUUser.cpp`'s `AnimPlay` comment says June "clears a third state byte (+0x8040)".
  - In fact `+0x8040` is `bSuperArmorNotRed`, which the port does clear. June's extra byte is `+0x8080`. The code is correct.
  - The same slip is in part 3's `funcmap.tsv` row for `31fe8130`, which now has a superseding row.
  - Fix the comment the next time that file is touched. Fixing it now would re-link a new exe for no behaviour change.
- **Info, hand-off to part 4: June's `AnimStop` marker.**
  - **The marker.** June's `AnimStop` (user vslot 230, `31fe7f70`) waits the two June-only anims too. When super armor was off, it sets `+0x8080 = 1`; then it sets `bSuperArmor` and `bSuperArmorNotRed`.
  - **The reader.** User vslot 153, `31fcd7b0(bool)`:
    - a non-empty list at `+0x42a0` → true;
    - argument 0 → `bSuperArmor`;
    - argument 1 → `bSuperArmor && !+0x8080`, i.e. super armor that `AnimStop` did not grant.
  - **Why it matters.** Freeze calls `AnimStop` (`X2BuffBehaviorTemplet.cpp:2691/2727/2790`). So in June, a frozen unit's super armor is visible to vslot 153(0) but hidden from vslot 153(1). The port does not model that.
  - **Size.** `.text` has 276 `mov r,[r+0x264]` vslot loads. About 88 of them reach an indirect call within 6 instructions:
    - 56 pass 0;
    - 1 passes a constant 1 (`3229a669`, CX2GameUnit region);
    - 7 pass a register;
    - 5 pass a pointer constant (`33766aac` ×3, `33722a70` ×2), so other classes also use a slot at that offset;
    - 19 had no push the scan could identify.
  - **Next step:** map `3229a669` and March's equivalent of vslot 153 before `AnimStop` is ported. This was outside part 3's scope, so it is not a defect.

**Doc corrections**
1. **Part 3, item 3, the BFI list ("523, 812, 820, 877, 910, 913, 929")** is incomplete.
   - There are 15 constants: add 517, 548, 549, 737, 763, 841, 923 and 946. All are June-only.
   - `3227c360`'s four sites all pass BTI 703.
   - Aisha's `32cc5c16` is resolved (check 4).
   - The conclusion stands: the chain is reached only from June-only content.
2. **Part 3, ConsumeMpRate row ("June's reader is not in `3203cf60`/`3203a250` (no BBT 63 compare)")** is wrong for `3203cf60`. It calls `32039a50( skill, 0x3f )` (check 5).
   - "Where part 3 stopped", item 3 is resolved: what remains is the port bundle above.
   - Part 3's `funcmap.tsv` row for `32e7cef0` ("NOT PORTED until June's reader is found") now has a superseding row.
3. **Part 3, `Code` section, the `X2GUUser` comment** has the R1 offset slip (`+0x8040`). That byte is `bSuperArmorNotRed`; the June-only one is `+0x8080`.

**funcmap.tsv:** 18 rows added ("Buff Phase 4b-i part 3 review").
- 17 are confirmed: `32039a50`, `32207210`, `32070580`, `322aaa30`, `322a95c0`, `31c7b640`, `31fe7f70`, `32755e90`, `32e7e870`, the six UnitScale/DoubleAttack/ReflectMagic GetFactor/packet slots, and superseding rows for `31fe8130` and `32e7cef0`.
- 1 is a candidate: `31fcd7b0`.

**Build and deploy:** none. There is no code change, and the deployed exe is still `57f7cf74…`.

**Kickoff for 4b-i part 4** (Opus 5.5 high). This supersedes the one in part 3's section: it adds this review and replaces the ConsumeMpRate reader search, which this review settled.

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "June buff function map", "Phase 4b", "Phase 4b-i, part 1",
"Phase 4b-i, part 2", "Phase 4b-i, part 3" and "Phase 4b-i part 3 -- Review". Branch:
mods/june-buff-port. Never launch X2_offline.exe without asking first.

Your task (Phase 4b-i part 4): resume part 3's list "Where part 3 stopped" from item 1 (the
four hand-offs: type 56 in both DamageReacts, the DASH_IMPOSSIBLE NPC AI gate, the
CHANGE_ENCHANT_ATTACK_RATE host math, the BBT 7/8 second members plus 322788c0), then RSCT
DRAW_TEXT; ConsumeMpRate as the part 3 review's port bundle (the class, the reader
32039a50, the multiply in GetActualMPConsume, March's Aisha Teleport override disabled
under the flag); the dormant vslot 3/9/10/11 chain and ChangeStat's vslot 14/15 owner-UID
hook; and the review's AnimStop hand-off (map 3229a669 and March's equivalent of user
vslot 153 before porting AnimStop's +0x8080 marker). Fix R1's comment in X2GUUser.cpp if
you touch that file. Under SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY: port June's whole
function wherever it differs, keep GetFactor/SetFactorFromPacket order (append only),
byte-level CP949 edits, insert-only, flag-off unifdef check per file. Another session may
have uncommitted edits in the doc: stage only your own hunks. Build, deploy, commit;
record where you stopped.
```

### Phase 4b-i, part 4 — progress (2026-10-04), launch check pending

Ran on Opus 5.5 high, under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`. The session followed the part 3 review's superseding kickoff, which is a superset of the earlier part 3 one: it does not search for ConsumeMpRate's reader again (`32039a50` was already found), and it adds the AnimStop hand-off and the R1 comment fix. Code commit `b9999a2`. Every code file is insert-only, and each one reduces byte-for-byte to `HEAD` with the flag undefined.

**Verdicts, in the stop-list order**

| Item | June | Verdict |
|---|---|---|
| Type 56 in both `DamageReact`s | `320462a0` @`32048d86`, `327b1f30` @`327b3a91` | **ported**, with June's Shade check |
| DASH_IMPOSSIBLE NPC AI gate | `3276d980` @`3276e3e6`/`@3276e532` | **ported** |
| CHANGE_ENCHANT_ATTACK_RATE host math | `322a0510`, `32274df0`, `32272d90` | **same as March for the buff part**; `32272d90` is not a buff function (hand-off below) |
| BBT 7/8 second members | `32282700`, `3226fd70`, `322830f0`, `32290950` | **ported** (store only) |
| Damage-variance roll | `322788c0` | **not ported**: decision D2 plus unmapped seed |
| RSCT DRAW_TEXT | `32e857d0` | **not ported**: needs a June-only pic-char |
| ConsumeMpRate bundle | `32e7cef0`, `32039a50`, `3203cf60` | **ported** |
| Runtime popups | – | still the user's decision (part 3) |
| Dormant vslot 3/9/10/11 chain | `322a7660`, `3236a430`, `322a8a70`, `3227c360` | **not ported**: three unit vslots and a June UI tail are unmapped |
| ChangeStat vslots 14/15 + `SetBuffOwnerUID` | `32e78410` @`32e784b8` | **ported** |
| AnimStop `+0x8080` marker | `31fe7f70`, caller `3229a669` | **unresolved, not ported**: the one constant-1 caller is June-only content; 7 register-passed callers are not traced |
| R1 (part 3 review) | – | **fixed**: the `X2GUUser.cpp` comment now names `+0x8080` |

**How each port sits on March**

- **Type 56 and Shade the Illusion** (`X2GUUser.cpp`, `X2GUNPC.cpp`).
  - BBT 56 on the defender's own list now applies to every attack type except `AT_SPECIAL`. March used `AT_NORMAL` only.
  - The user's test register is EDI, which is 1 on every path into the block (`MOV EDI,0x1` @`320487fb`, joined @`32048800`), so the user test is `!= AT_SPECIAL`.
  - The NPC also requires `!DD.m_bNoDamage` (`+0x4dc`) for 56 and 75, but not for Shade.
  - Shade: when `m_eUsedSkillID` (`+0x590`) is 8021, 8025 or 8026 (`SI_SA_SDK_BRUTAL_CUTTER`, `SI_SA_SCA_CRIMSON_BREAK`, `SI_SA_SCA_DREAD_WEAPON`, Lu/Ciel skills March lacks) and the attacker has `BTI_BUFF_SHADE_THE_ILLUSION` (430, a new C++ enumerator under the flag), the hit doubles. June calls `HaveThisBuff` (`3227baf0`) on the attacker without a null test, and that function dereferences `this` (`+0xe14`). The port adds a NULL test as a crash guard.
  - March's storage is kept: `m_bDoubleAttack` on the user, and on the NPC both branches of `X2OPTIMIZE_USER_DAMAGEEFFECT_SHOW_BY_GAMEOPTION`.
  - BBT 75 sits between 56 and Shade in June. Its place is marked with a comment; Phase 6 owns it.
  - **Not ported:** June's multiply just above the block (`m_eUsedSkillID == 30205` × a June-only member `+0x9600`, @`32048d64`/@`327b3a6f`). March has neither the skill nor the member.
- **DASH_IMPOSSIBLE NPC gate** (`X2GUNPC.cpp`). June's case values equal March's `SCT_AI_*` enum (11 DASH, 12 JUMP, 14 JUMP_DIR, 15 DOWN_DIR). DASH and JUMP_DIR share the gate @`3276e3e6`, and DOWN_DIR has its own @`3276e532`. While `m_vecDashImpossiblePtr` is non-empty the message is neither acted on nor cleared, as in June. March fills that list for every unit type (`CreateAndInsertBuffIdentity`, `X2GameUnit.cpp:13294`). JUMP (12) gates on list 104, which is Phase 7's.
- **Enchant host math: nothing to port for the buff side.**
  - `322a0510` computes `fix + pct · ( DD.m_fRateModifier · ( GetEnchantAttack + accumulation rate ) )` and returns false when that is below the roll. That is March's formula with `CHUNG_MEMO_01` (the modifier applied unconditionally).
  - `32274df0( edt, ct )` returns the PERCENT product (0 → 1.0) or the FIX sum. Its attribute match (0 all; 1..6 = blaze 1001, frozen 1011, poison 1101, piercing 1543, shock 1654, snatch 1765) equals March's `BEAT_*` order. So it is March's `GetChangeEnchantAttackRate`, called once per change type.
  - **Correction to part 3:** `32272d90`'s `0x89a` compare is EDT 2202, a poison-master tier in June's renumbered EDT enum, not the Teleport skill.
- **BBT 7/8 second ("range") members** (`X2GameUnit.h/.cpp`).
  - `m_AdditionalPhysicAttackRange` and `m_AdditionalMagicAttackRange`, with getters.
  - June inserts the same factor into both the main and the range member (`32282700` @`3228289e`/`@322828d6`), updates or inserts both (`3226fd70`), erases both (`322830f0`) and refreshes both (`32290950`). The port does the same at March's four sites.
  - June's basic value comes from two stat-block fields (`+0xc`/`+0x10`, slots 119/120) that March's stat lacks, so the basic stays 0. June's `Verify` and passive init do not touch these members, so the port doesn't either.
  - **Reach on March data: none.** The result is `basic + option + fix + relation + basic·(mult − 1)` (`X2BuffTemplet.cpp:749`). All 103 BBT 7/8 entries in March's live factor file are `BCT_PERCENT` (fresh `elstools decompile-batch` of the 353,105 B live file), so the range result stays 0.
  - This unblocks Phase 5b's 80 and 81, which also insert into the range members.
- **ConsumeMpRate bundle** (`X2BuffBehaviorTemplet.cpp`, `X2GUUser.h/.cpp`, `X2GUArme_VioletMage.cpp`).
  - Class `SetFactor` (`32e7cef0`): for users only, `rate = 1 + v[2]` on the first apply and `+= v[2]` on a restack. There is no change-type test and no MP lookup; June looks up the skill level and discards it. NPCs return true and store nothing.
  - ParsingBehavior (`32e75100`), packet (`32e7d180`), DoFinish (`32e75290` = `EraseTemporaryBuffFactor`) and GetFactor (`32e7e870`) equal March's, apart from the load-time popup.
  - Reader `CX2GUUser::GetSkillRateByBuffBehavior( skill, BBT )` = `32039a50`. It walks `GetBuffFactor()` and takes `v[2]` when `(int)v[1]` is the skill, or when `v[1]` is 0 and the skill is neither `ST_RELATIONSHIP_SKILL` nor `ST_HYPER_ACTIVE_SKILL`. The last match wins; the default is 1.0. June's `+0x178` ID remap (`32207210`) has no March field, so the ID is compared as is.
  - `CX2GUUser::GetActualMPConsume` returns its cost × that rate for every skill. June's own function takes a different signature (`(float mp, skill, bool)`, `ret 0xc`), but its result is a pure product, so multiplying March's return value is equivalent.
  - Aisha's Teleport override (`X2GUArme_VioletMage.cpp`, `FIX_SKILL_BALANCE_AISHA_LENA`) is disabled under the flag, because it would replace the cost with the rate.
- **ChangeStat vslots 14/15.** `GetBuffOwnerUID()` returns 0 and `SetBuffOwnerUID( UidType )` is empty, as June's base (`32ec5980`/`33093ad0`). `SetFactor` calls 15 with `BuffFactor_.GetUidGameUnit()` (June factor `+0x30`/`+0x34`) after both type checks. This is a no-op until Phase 5b's `_HaveBuffOwner` subclass (BBT 93) overrides it.

**Behaviour changes on March data**
- **NPCs under `BTI_DEBUFF_SILENCE`, `BTI_DEBUFF_WOUND` or `BTI_DEBUFF_LEG_SPASM`** (March's 3 DASH_IMPOSSIBLE templets) no longer dash, jump-dir or down-dir from AI messages while the debuff lasts.
- **`BTI_DEBUFF_CHANGE_CONSUME_MP_RATE`** (Aisha Teleport, SKILL_ID 2202, PERCENT 0.5): the cost is now 1.5 × the live cost on every cast, where March froze 1.5 × the cost at apply time. The live cost includes Aisha's `m_fMPConsumeRateTeleport` (a passive `SA_MP_CONSUME_REL` multiplier set at skill init, `X2GUArme_VioletMage.cpp:2611/3162`, not a chain counter), which March's frozen value also carried through the virtual `GetActualMPConsume` at apply time; the Skill Note ×1.4 still applies after it in both. The reader now looks at buffs too, not only debuffs. A restack adds 0.5 to the rate.
- **`BTI_DEBUFF_DOUBLE_ATTACK`** (March's only BBT 56 templet): `AT_COMMAND` hits also double now (no March damage data uses `AT_COMMAND`), and NPC no-damage hits no longer flag a double. Neither is visible on March data.
- Range members, the ChangeStat hook and the Shade check: no reach.

**Not ported, with what each needs**
- **D2, damage-variance roll `322788c0`: goes to Phase 5c (user, 2026-10-04); RE notes for that phase.**
  - Decoded: MSVC `mt19937` (the 4996-byte engine object), seeded with `defender->vslot 113( 0 ) & 0xffff` (user word `+0x80f8`, NPC word `+0x8474`, unmapped counters). It draws `normal( base · vslot121( arg ) + range · maxi, range · 0.5 )` and clamps to `[ base − range + 2 · range · maxi, base + range ]`. It fires only when range > 0.
  - Still open: the seed counters (they must agree between P2P peers), the two `GetFinalDamage` call sites (`@32284c1f`/`@32284c2f`), slot 121 per class, and the summoned-NPC copies (`3230ca8c`, `32763d6e`, which set the summon's basic from the owner's results).
  - Dormant on March data (range 0). **Decided (user, 2026-10-04): port it in Phase 5c, with 92 MAXIMIZE.** Phase 5c owns the roll, the seed counters, both `GetFinalDamage` call sites and the summoned-NPC copies.
- **RSCT DRAW_TEXT `32e857d0`.**
  - **Correction to part 2:** the formats are `%s = %d%s` (SWAP), `%s +%d%s` and `%s %d%s` (FIX, value ≥ 0 / < 0), not `-%d`. STR 38799 "COOLDOWN" and 38800 "SEC" read the same in March's and June's `General.ess`, so no data change is needed.
  - The draw is `g_pData+0x17c->DrawText( text, unit vslot 63 position, vslot 91 direction, AT_CENTER, 0.0, 0, 0 )`. `+0x17c` is **`PicCharYellowSmall`** (created by `31edd860`), a pic-char March's `CX2Data` lacks. The `PicCharYellowSmall` sequence is present in the staged `UIMajorParticle.txt`.
  - Porting it means adding that pic-char to `CX2Data` (ctor arguments from `31edd860`) and checking that its glyphs cover letters.
- **Dormant vslot 3/9/10/11 chain.** `322a7660` calls unit vslots `+0x210` (132), `+0x770` (476) and `+0x234` (141), none mapped yet (Phase 1c item 5 also left them open). `3236a430` re-runs the displayers through a June UI tail (`322ab560`, `324943c0`, `32489810`, `32580100`). Map those first. The behavior-side overrides are already decoded (part 3).
- **AnimStop `+0x8080` marker.** June's vslot 153 with argument 0 is March's `IsSuperArmor()`. The only constant-1 caller, `3229a669`, gates on `APPLIED_SKILL_ID == 24057` (`RNSI_SA_RRF_DEVASTATING_STRIKE`, after a 24222 `RNSI_A_RBM_POINTED_STILETTO` check). Both are June renewal skills, so that caller is June-only. **Unresolved:** the 7 sites that pass the argument in a register (part 3 review's scan) are not traced, so whether a March-equivalent path asks for the variant is still open.
- **Enchant accumulation rework `32272d90`: no owning phase.** June replaces March's "×2 at accumulation 1–2" with `+ 1× / 2×` the attacker's `EnchantData+0x3c` (`m_fAttribReiterationRate`), and adds a fourth tier per element (4004/4044/4404). June's `EnchantData` loader `32708f00` also reads `m_fBlazeMpDrain`, `m_fPiercingDamageFactor`, `m_iShockDebuffTime` and `m_fShockDebuffDamageFactor`. This is an enchant-system port (with June's EDT renumbering), not a buff behaviour. **Decided (user, 2026-10-04): it gets its own phase, Phase 15.**
- **BBT 6 HP-ratio rescale: orphaned hand-off.** Phase 1c item 3 (and the Phase 5 text) hands June's BBT 6 insert/erase rescale (`32282723`/`32283113`, `31e4f900`/`31e551a0`, gage `32081260`) to 4b, but no 4b stop list carried it. It is listed below.

**Hygiene.** A fresh byte-level unifdef of the flag: all 9 code files reduce byte-for-byte to `db3d003`, the `HEAD` before `b9999a2`. Each raw file differs from it, so the check is not vacuous. `--numstat` shows 203 insertions and 1 deletion, the deletion being our own R1 comment line. All inserted lines are ASCII, the line endings follow each host file (all CRLF here), and the `file` signatures of all 9 files are unchanged.

**Build and deploy.**
- `X2Lib` (with `stdafx.cpp` touched) and `X2` at `US_SERVICE`, twice (after batch 1, then after all batches): rc 0, 0 errors. The only warning in the touched files is the existing C4018 (`X2GUUser.cpp:17876`, shifted).
- `x2.exe` is 19,015,680 B, linked 00:56:35, md5 `537ed2747b4492848b4ae57f57f6d704`. `X2.map` carries `GetSkillRateByBuffBehavior` and the owner-UID virtuals.
- Copied to the game directory as `X2_offline.exe` (name read programmatically); the md5 matches. **Not launched.**
- No Lua changed, so no pack is needed.
- **Things to watch in play:** monsters under Silence, Wound or Leg Spasm should not dash; Aisha's Teleport under the Lena debuff should cost 1.5× its normal (passive-adjusted) cost and should not drop to 1.5 MP; the double-attack debuff on monsters.

**funcmap.tsv:** 24 rows ("Buff Phase 4b-i part 4").

**Play-test (2026-10-04, user): all four checks pass.** The exe deployed with `b9999a2` (md5 `537ed274…`) passed checks 3 and 4. Checks 1 and 2 needed a debug build, `16a6d78` (`SERV_IRUHADEV_JUNE_BUFF_DEBUG`, md5 `ce13bcb3…`), and were settled from its `offline_server.log` lines.
1. **NPC dash gate: pass.**
   - The first attempt was inconclusive. Leg Spasm (227) lands only 70% of the time and monster resist cuts that further, so in Feita nothing applied. The gate only stops a monster *starting* a run: a DASH state has no `SCT_AI_DASH` entry and leaves only on walk, wait, jump or attack. So a monster that was already running kept running (Burning Forest). June uses the same gate and reads `+0x4260` nowhere else (`.text` scan: the user `SetInputData`, the identity insert/erase, the ctor/dtor, and the two NPC sites).
   - Retest with `BFI_DEBUFF_SI_A_RST_CUT_TENDON` (115, level 30; rate 1, resist ignored, about 10 s) in the first Feita dungeon. `DASH_IMPOSSIBLE applied: BTI 31 on npc uid 1..9` (all 9 monsters, twice), then `held back SCT_AI_DASH on Gliter_Archer.lua uid 5`.
   - The Glitter Charger's thrust never appears among the held-back messages, so it is an attack state, not a dash message, and is correctly not blocked (the same in June).
2. **Teleport cost: pass.** One Teleport cost 20.00 (rate 1.00). A chain cost 30, 40, 50, 60, 70 (rate 1.50 → 3.50). Each Teleport re-applies its own debuff (`X2GUArme_VioletMage.cpp:7686-7697`), and the class adds 0.5 per restack (June `32e7cef0`, `+= v`). March's additive restack (`result += init · v`) gives the same 30, 40, 50… on the cost frozen at the first cast, so the two agree whenever the base cost does not change mid-chain. The old override would have read 1.5 MP; it is gone.
3. Power Accel (7) on self: damage up, no popup, clean expiry. Pass.
4. One full dungeon clear: no new popup or crash. Pass.

**The debug build is still deployed.** Revert `16a6d78` (and rebuild KTDXLIB, X2Lib and X2, since `Always.h` changes) before the next play-test that should not log. Part 5 can do it first.


**Where part 4 stopped / what resumes (in this order)**
0. Revert the temporary debug commit `16a6d78` and rebuild (see "Play-test" above).
1. BBT 6 HP-ratio rescale (above).
2. RSCT DRAW_TEXT: `CX2Data` `PicCharYellowSmall` plus the draw call. **Data too (part 4 review, R1):** March's `UIMajorParticle.txt` (staged and live, 592,530 B) has no `eventsequence "PicCharYellowSmall"`, only `"PicCharYellow"`. June's `14JUN17/data/data036_kom/UIMajorParticle.txt` (2,042,192 B) and `GameMajorParticle44.txt` have it. Adopting June's file (June asset wins) needs a user pack, shipped together with the exe that adds the pic-char. First check that March's particle parser accepts every key in June's file (an unknown particle `.txt` key aborts the rest of the file), or carry only the `PicCharYellowSmall` block.
3. The dormant chain: map unit vslots 132/141/476 and `3236a430`'s UI tail, then port.
4. The AnimStop `+0x8080` marker: trace the 7 register-passed vslot 153 callers; port the marker only if a March-equivalent path passes 1.
5. **User decision still open:** the runtime popups (part 3). Decided 2026-10-04: D2, the roll goes to Phase 5c; the enchant rework is Phase 15.
6. Then 4b-ii (finalizers and displayers).

**Review of part 4:** required (Opus 5.5). Suggested spot-checks:
- EDI = 1 at `32048d89` on every path;
- the NPC `m_bNoDamage` placement (56 and 75 only, not Shade);
- `32e7cef0`'s `1 + v` and `+= v`;
- `32039a50`'s last-match-wins and the 12/13 filter;
- the four range-member sites against `3226fd70`/`322830f0`.
- `32e784b8`: is June's factor `+0x30`/`+0x34` the same UID March's `CX2BuffFactor::GetUidGameUnit()` returns?

**Kickoff for the part 4 review** (Opus 5.5 high). It runs on its own (no other session editing code). It is read-only unless it finds a code defect, which it then fixes, rebuilds and deploys, as the part 1 review did.

```
Continuing the June buff/debuff port. Run this session on Opus 5.5 at high effort. Read
JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", the "June buff function
map", "Phase 4b-i part 3 -- Review" and "Phase 4b-i, part 4" (including its play-test).
Branch: mods/june-buff-port. Never launch X2_offline.exe.

Your task: an independent review of Phase 4b-i part 4. The code is b9999a2 (under
SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY); 16a6d78 is a temporary debug commit (hygiene
only); the doc commits are 6b0f0c9, 06868a2, a1e1422, 0f4ca0b and 16e057c. Use fresh
scratch tools; do not reuse part 4's patch or unifdef scripts.

No other session is editing code. The review is read-only unless it finds a code defect.
Run the flag-off unifdef on the committed blobs of b9999a2 against db3d003, and take
numstat from git diff db3d003 b9999a2. The deployed X2_offline.exe is the 16a6d78 debug
build (md5 ce13bcb3...); confirm that, and leave the debug flag in place (part 5 reverts
it).

Check, in Ghidra (read-only) and the asm, the part 4 section's suggested spot-checks, plus:
whether each "same as March" and "not ported" verdict holds (32274df0, 32272d90, 322788c0,
32e857d0 / PicCharYellowSmall at g_pData+0x17c, 322a7660's vslots, 3229a669), and whether
the play-test reasoning holds (the DASH-state event table, the Teleport chain 30..70 vs
32e7cef0's restack). Count March-data reach where the doc claims none.

Report numbered findings (R1, R2...) with a severity. If you find a code defect, fix it
under the same flag (insert-only, byte-level CP949 edits, flag-off unifdef per file),
rebuild X2Lib and X2 at US_SERVICE (KTDXLIB too if Always.h changes; touch stdafx.cpp
after a header edit), deploy as X2_offline.exe, record the md5, and ask before launching.
Add confirmed funcmap.tsv rows. Another
session may have uncommitted edits in the doc: stage only your own hunks (build the index
blob from HEAD plus your edits; git hash-object -w, git update-index --cacheinfo). Commit.
Record the review cost line as in earlier reviews.
```

**Kickoff for 4b-i part 5** (Opus 5.5 high):

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "June buff function map", "Phase 4b", "Phase 4b-i, part 3",
"Phase 4b-i part 3 -- Review" and "Phase 4b-i, part 4". Branch: mods/june-buff-port. Never
launch X2_offline.exe without asking first.

Your task (Phase 4b-i part 5): resume part 4's list "Where part 4 stopped" from item 0 (revert the debug commit 16a6d78), then the
BBT 6 HP-ratio rescale (June 32282723/32283113), RSCT DRAW_TEXT (CX2Data PicCharYellowSmall
from 31edd860, then the 32e857d0 draw; the PicCharYellowSmall particle sequence is not in March's
UIMajorParticle.txt -- stage June's data036_kom/UIMajorParticle.txt and name it for the user
to pack after checking March's particle parser accepts its keys, or carry only that block; see the
part 4 review R1), and the dormant vslot 3/9/10/11 chain (map unit
vslots +0x210/+0x234/+0x770 and 3236a430's UI tail first), then trace the 7 register-passed
IsSuperArmor (vslot 153) callers for AnimStop's +0x8080 marker. Do not port the damage roll
322788c0: it belongs to Phase 5c (user, 2026-10-04). Under SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY: port
June's whole function wherever it differs, keep GetFactor/SetFactorFromPacket order (append
only), byte-level CP949 edits, insert-only, flag-off unifdef check per file. Another session
may have uncommitted edits in the doc: stage only your own hunks. Build, deploy, commit;
record where you stopped.
```

### Phase 4b-i part 4 — Review (2026-10-04, Opus 5.5 high)

Independent review of code commit `b9999a2` (under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`), debug commit `16a6d78` (hygiene only), and doc commits `6b0f0c9`, `06868a2`, `a1e1422`, `0f4ca0b` and `16e057c`. Read-only on code: nothing was edited, built or deployed.

Fresh tools only; part 4's patch and unifdef scripts were not reused:
- Ghidra, read-only (decompile, function lookup).
- A new PE-section-mapped capstone reader, plus a raw-displacement scanner for `.text`.
- A new byte-level unifdef.
- A fresh `elstools extract -v mar-2014` of the live `data036.kom`, `data037.kom` and `datai016.kom`, then `decompile-batch` of `BattleFieldBuffFactor.lua` (353,105 B), `BattleFieldBuffTemplet.lua` (152,564 B) and `Gliter_Archer.lua`.

**Verdict: no code defect.** All six suggested spot-checks pass in the asm, and so do the extra checks. One doc claim is wrong and changes part 5's scope (R1). The rest are Info.

| # | Check | Result |
|---|---|---|
| 1 | EDI = 1 at `32048d89` | **Pass.** `32048d86: mov eax,[ebx+8]; sub eax,edi; je` skips BBT 56 on `AT_SPECIAL` (March `AT_SPECIAL` = 1). The last EDI write before the block is `mov edi,1` @`320487fb`. The only outside entry into `(320487fb, 32048d86]` is `3204864c → 32048800`, with EDI = 1 from `@32048640`. That one is reached by fallthrough or from `32048485` (EDI = 1 from `@32048441`, no other entry). Of the function's 9 switch tables, only `@32048b2e`'s targets the region, and it sits inside it. The block also matches the port: 30205 (`0x75fd`) × `+0x9600` above it; BBT 56 on the defender (`32274f60`); BBT 75 on the attacker with a null test (`32275050`); then Shade: `== 0x1f55`, or `> 0x1f58` and `<= 0x1f5a`, i.e. 8021, 8025 or 8026, then `HaveThisBuff( 0x1ae )` with no null test. |
| 2 | NPC `m_bNoDamage` placement | **Pass.** `327b3a91`: BBT 56 requires type ≠ 1 and `+0x4dc == 0`. BBT 75 @`327b3ac7` has the same two tests plus the attacker null test. Shade @`327b3aea` has neither the `+0x4dc` test nor a null test. **The offsets are proven from the loader, not assumed.** In `SetDamageDataFromLUA` (`32632390`), the decrypted `NO_DAMAGE` key (`xorstrings.tsv` `ok`, @`3263ade6`) stores through `lea edi,[ebp+0x4dc]` / `mov [edi],al` @`3263ae98`. `USED_SKILL_ID` (@`3263de71`) stores `[ebp+0x590]` @`3263df73`. |
| 3 | `32e7cef0`'s `1 + v` and `+= v` | **Pass.** `GetBehaviorFactor` fails → false. `+0x14 = ftol( v[1] )` (0 when size ≤ 1), for every unit. A non-user (`+0x264 ≠ 0`) → true. Then the user's `+0x867c` is null → false. The skill-level lookups (`32890620`) are discarded. Started (`+0xc`) → `old + v[2]` via `32071230( 2 )`. Not started → `v[2] + 1.0` (`334355b0`). The write is skipped when the value is unchanged. The class enum is `{ CHANGE_TYPE 0, SKILL_ID 1, VALUE 2 }`, so the port reads the right slots. Both sides read through bounds-checked getters (0.0 past the end). |
| 4 | `32039a50`, last match and the 12/13 filter | **Pass.** Default `fld1`. It walks `GetBuffFactor` (`3229fd80`: skips templets whose bytes `+0x65`/`+0x66` are set, the same shape as March's `!GetDidFinish() && !IsWorldBuff()`). It compares `m_uiType` to the argument, then `chop( v[1] )` against `32207210( skill )`. When the IDs differ, `v[1] == 0` plus a non-null templet plus `32070580` (type ≠ 13 and ≠ 12) is needed. A match stores `v[2]` and keeps walking, so the last match wins. March has `ST_RELATIONSHIP_SKILL` = 12 and `ST_HYPER_ACTIVE_SKILL` = 13. (R5: June reads `v[1]`/`v[2]` raw.) |
| 5 | Four range-member sites | **Pass.** Insert `@3228289e`/`@322828d6` (main `+0x4384`/`+0x4438`, then range `+0x44ec`/`+0x45a0`, via `3236d110`). Update `@3226fe28…fe79` / `@3226fe97…fee1` (update-or-insert on the main member, then on the range member, via `3236d370`/`3236d110`). Erase `@3228326c`/`@32283294` (`3236c610`). Refresh `@32290976`/`@32290981` (`3236d350`). **All 42 `.text` references to `+0x44ec`/`+0x45a0`/`+0x458c..94`/`+0x4640..48` are accounted for:** the four sites; BBT 80/81's update cases in the same switch (handlers `322708d8`/`32270947`, Phase 5b's); the ctor and dtor; the two basic setters (below); the roll `@32278924`/`@32278943`; the summon copies `3230ca9a`/`32763d7c`; and EH unwind. |
| 6 | `32e784b8` UID | **Pass.** `32d4d670` (`SetGameUnitBuffFactor( unit, creatorUID, isUser )`): a creator UID other than −1 goes to `+0x30/+0x34` with `isUser` at `+0x38`; otherwise vslot 21 (`GetUnitUID`) → `+0x30/+0x34`. That is March's `SetUidGameUnit` choice (`X2BuffFactor.cpp:766/773/777`), so `+0x30` is `m_uidGameUnit`. `@32e784a9…b8` pushes `[factor+0x34]`, `[factor+0x30]` and calls vslot 15 (`+0x3c`). |
| 7 | `32274df0` "same as March" | **Holds.** It walks `+0x6594`: attribute 0 → all; 1→1001, 2→1011, 3→1101, 4→1543, 5→1654, 6→1765. June's Enum names those EDTs `ENCHANT_BLAZE`, `_FROZEN`, `_POISON`, `_PIERCING`, `_SHOCK` and `_SNATCH`, and March's `BEAT_*` are 1–6 in that order. Change type 3 is a product (0 → 1.0) and 1 is a sum; anything else returns 1.0. That equals March's `GetChangeEnchantAttackRate` (`X2GameUnit.cpp:12720`), which returns both values in one struct. |
| 8 | `32272d90` | **Holds.** `0x89a` = `EDT_ENCHANT_POISON_MASTER` (2202, June `Enum.src.lua:7373`). It is compared on the EDT register, beside 1101 POISON, 3303 and 4404 `QUADRA_POISON_MASTER`. |
| 9 | `322788c0` not ported | **Holds.** Callers are only `32284c1f`/`32284c2f`. A 0x13dc-byte frame (the engine). It reads both range results (`+0x4640`, `+0x458c`). Phase 5c's. |
| 10 | `32e857d0` / `g_pData+0x17c` | **The pic-char holds; the data claim does not (R1).** `31edd860` pushes wide string `0x3345ac54` = `PicCharYellowSmall` and stores the result at `[esi+0x17c]` (@`31edd8f1`). `32e857d0` loads `[g_pData]+0x17c` @`32e85f4c`. March's `X2Lib`/`KTDXLIB`/`X2` have no `PicCharYellowSmall`. **Not re-derived:** part 4's format strings and the `General.ess` 38799/38800 claim. |
| 11 | `322a7660`'s vslots | **Hold.** `@322a773a` `+0x210`, `@322a778c` `+0x770`, `@322a77a6` `+0x234` (indirect), then `3236a430` @`322a77ba`. Unmapped, as part 4 says. |
| 12 | `3229a669` | **Holds.** It is in `3229a530`. For a user attacker whose `CX2Unit+0xaf0` pointer points at the value 4, it calls the *defender's* vslot 153( 1 ). Only a true result together with `[DD+0x5c4] == 0x5df9` (24057) acts: `EffectSet SA_ENW_Predator_Lucky`. The 24222 block above it only scales two outputs. So the call's result matters only for June-only skills. The 7 register callers stay part 5's. |
| 13 | DASH-state event table | **Holds, with a detail (R3).** Live `Gliter_Archer.lua`: `GLITER_ARCHER_DASH` has no `SCT_AI_DASH` entry. WAIT and WALK have one, so the gate only stops a monster starting a run. `+0x4260` readers in `.text`: user `SetInputData` (`31fe83a0`), identity erase (`32279d70`), insert (`@3227adff`), ctor (`322a2700`), dtor (`322a0730`), EH unwind, and the two NPC sites. That is exactly part 4's list. Both gates jump to `@3276f03a` without clearing `+0x863c`/`+0x8640`. JUMP (12) gates on `+0x42c0`. |
| 14 | Teleport chain 30..70 vs restack | **Holds.** Each cast re-applies the debuff (`SetBuffFactorToGameUnit`, `X2GUArme_VioletMage.cpp:7694-7704` in HEAD, the doc's 7686-7697 before `16a6d78`'s +8 lines). The rates go 1.0, 1.5, … 3.5. Times 20, that gives 20, 30 … 70. March's `#else` is `result = I0(1 + v)`, then `+= I0·v`, i.e. `I0(1 + nv)` on the frozen first cast. Aisha's Teleport path is all products (passive, Skill Note ×1.4, decrease rate), and the `< 0` clamp never fires. So ordering changes nothing on March data (but see R2). |
| 15 | Hygiene | **Pass.** **Unifdef:** `strip( b9999a2 ) == strip( db3d003 )` for all 9 code files, with the flag undefined. The raw blobs differ; 22 new blocks. `db3d003` already carries parts 1–3's blocks of the same flag (R4). **numstat:** 203/1; the deletion is the R1 comment. Every added line is ASCII and CRLF. **`file`** signatures are unchanged on all 9 files. **`16a6d78`:** with `SERV_IRUHADEV_JUNE_BUFF_DEBUG` undefined, `X2GUArme_VioletMage.cpp`, `X2GUNPC.cpp` and `X2GameUnit.cpp` reduce to `16a6d78^`, and the flag is defined in `Always.h`. **Deploy:** the game directory holds `x2.exe` and `X2_offline.exe` (names read programmatically). `X2_offline.exe` is 19,016,704 B, dated 05:41:07, md5 `ce13bcb3bc7565b5f0bf0cd2b20c52ba`, equal to `X2/US_SERVICE/x2.exe`. The debug build stays deployed; part 5 reverts it. |
| 16 | ConsumeMpRate consumers | **Pass.** `X2Lib` (including `Offline/`) reads the class's `[2]` in three places only: Aisha's override (disabled under the flag), `GetSkillRateByBuffBehavior`, and the `16a6d78` debug line. So no other site replaces a cost with what is now a rate. |

**March-data reach, counted where the doc claims none**
- **Range members: 0.** In the live files, BBT 7/8 has 103 factor entries (51/52) and 95 templet entries (47/48), all `BCT_PERCENT`. No reader of the range members exists outside `X2GameUnit`, and no C++ calls `Insert`/`UpdateAdditionalStat` with BBT 7/8 directly.
- **Shade: 0.** BTI 430 has no templet in the live files. Skills 8021/8025/8026 are absent from March's Enum. BTI 430 matches across the C++ enumerator, March's staged Enum and June's.
- **`AT_COMMAND`: 0.** The only occurrence among 144 + 1,910 + 159 live scripts (`data036`/`data037`/`datai016`) is its definition in `Enum.lua`, and no C++ assigns it.
- **ChangeStat hook: 0.** There is no override.
- **Confirmed counts:** 3 DASH_IMPOSSIBLE templets, 1 BBT 56 templet, 1 ConsumeMpRate templet.

**Findings**
- **R1, Medium (doc; it changes part 5's scope).** Part 4 says "the `PicCharYellowSmall` sequence is present in the staged `UIMajorParticle.txt`". It is not.
  - March's staged and live `UIMajorParticle.txt` (592,530 B) have only `eventsequence "PicCharYellow"`.
  - `"PicCharYellowSmall"` is only in June's `14JUN17/data/data036_kom/UIMajorParticle.txt` (2,042,192 B) and `GameMajorParticle44.txt`.
  - So DRAW_TEXT is not exe-only: it needs June's particle file staged and packed by the user, shipped with the exe.
  - "Where part 4 stopped" item 2 and the part 5 kickoff are amended. A superseding `31edd860` row is added.
- **R2, Info (ConsumeMpRate position; no reach on March data).** Shown in the asm:
  - In June, `3203cf60` is slot 423 of all 13 user vtables, with no per-class override. RTTI: `CX2GUUser`, plus Eve, Add, Ain, Rose, Luciel, EL, Ara, Chung, Raven, Lire_ElvenRanger, Arme_VioletMage and Elsword_SwordMan. The other `CX2GU*` vtables are NPC classes.
  - Inside it, `vslot 471( skill ) × 32039a50( skill, 63 )` is the last factor. It multiplies the `mp` argument after `3203a250` (BBT 99) and the `bool` branch.
  - In the port, the rate multiplies March's base cost. Each March override then applies its additive Skill-Note offsets (e.g. Raven −20/−50, Chung −20/−15/+10, Eve −20/−30) and the `< 0` clamp after the rate.
  - Not traced: where June's callers get `mp`, so whether June also applies those offsets first is open.
  - vslot 471 (`31fdbe20`, a per-skill rate lookup) is a second multiplier the port does not model.
  - Reach is 0 today: the one March factor targets Teleport, whose path is all products. It matters once June data with `v[1] = 0` or other skill IDs arrives. Hand-off: Phase 6b, which owns `3203a250`, the same function.
- **R3, Info (doc).** Play-test item 1 says a DASH state "leaves only on walk, wait, jump or attack".
  - `GLITER_ARCHER_DASH` actually leaves on WAIT (→ `DASH_END`), WALK, JUMP, JUMP_DIR, DOWN, DOWN_DIR and foot-off-line, and has no attack entry.
  - Under the debuff, its JUMP_DIR and DOWN_DIR exits are gated as well. The conclusion stands.
- **R4, Info (doc wording).** "All 9 code files reduce byte-for-byte to `db3d003`" holds as `strip( b9999a2 ) == strip( db3d003 )`. `db3d003` itself contains parts 1–3's blocks of the same flag.
- **R5, Info.** `GetSkillRateByBuffBehavior` reads `v[1]`/`v[2]` through the bounds-checked `operator[]`. June reads them raw (`[esi+4]` @`32039b09`, `[edx+8]` @`32039b73`). Every factor from `GetFactor` has 3 values, so the results agree. No change.

**funcmap.tsv:** 7 rows ("Buff Phase 4b-i part 4 review").
- 5 are confirmed: superseding `31edd860` and `3203cf60`, plus `32d4d670` (UID layout), `3229a530` and `32632390` (`+0x4dc`/`+0x590`).
- 2 are candidates: `31fdbe20`, and the basic-setter sites `32281560`/`322816f8`.

**Build and deploy:** none, because there is no code defect. The deployed exe is still the `16a6d78` debug build (`ce13bcb3…`).

### Phase 4b-i, part 5 — Done (2026-10-04), play-tested

Ran on Opus 5.5 high. Commits: `d34de09` reverts the part 4 debug commit `16a6d78`; `7ea72b2` (items 1-2) and `3bbf7d2` (item 3) are the code, under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`; `30aa0cb` and this section's later commit are the doc.

**Verdicts, in the stop-list order**

| Item | June | Verdict |
|---|---|---|
| 0. Revert `16a6d78` | – | **done** (`d34de09`). The 4 files equal `16a6d78^` byte for byte; `file` signatures unchanged; KTDXLIB rebuilt for `Always.h` |
| 1. BBT 6 HP-ratio rescale | `32282700` @`32282723`, `322830f0` @`32283113` | **ported** |
| 2. RSCT DRAW_TEXT | `32e857d0`, pic-char `31edd860` | **ported**; needs the staged `UIMajorParticle.txt` packed |
| 3. Dormant vslot 3/9/10/11 chain | `322a7660`, `3236a430`, `322a8a70`, `3227c360` | **ported dormant** (`3bbf7d2`); slot 476 logged and skipped, the icon-count tail not ported |
| 4. AnimStop `+0x8080` marker | callers of user vslot 153 | **resolved: not ported**. No March-equivalent path passes 1 |

**1. BBT 6 (`X2GameUnit.cpp`, `InsertAdditionalStat`/`EraseAdditionalStat`, case `BBT_CHANGE_MAX_HP`).**
- Helpers, from the asm: `31e4f900` = unit `GetNowHp` (no gage → 1.0, no HP gage → 0.0); `31e551a0` = unit `GetMaxHp` (same shape); `31e8c940` = gage `SetMaxHp`; `32081260` = gage `SetNowHp`. June's `SetNowHp` is March's clamp (`< 0` → as is, `> max` → max). It also writes a June-only previous-HP member (gage `+0x108`), which is not ported.
- **Insert:** factor insert, result update, then `ratio = now / max`, read *before* the gage's max changes. Then `SetMaxHp( result )` and `SetNowHp( result × ratio )`. With no gage data (`+0xc04` null), neither write happens; March dereferenced the gage unguarded.
- **Erase:** factor erase, result update, then with gage data: read `oldMax` and `now`, `SetMaxHp( result )`, and, if `now != 0` (`FUCOMPP` @`3228317d`, so a NaN still rescales), `SetNowHp( (newMax / oldMax) × now )`. Insert has no zero test.
- **Unchanged:** the update path (`3226fd70` case 6 updates or inserts the factor only) and the refresh `32290950`. Neither writes the gage in June or in March. March's `UpdateAdditionalStat` fallback is member-level (`CreateAndInsertStatFactor`), as June's is, so the rescale does not fire there.
- **Reach on March data: 5 templets** (live `BattleFieldBuffTemplet.lua`, 152,564 B): `BTI_BUFF_BELSSING_OF_CRONOS`, `BTI_BUFF_RALLY_OF_HERO_LEVEL_1/2/3` and `BTI_BUFF_PET_AURA_SKILL`. When one starts, the current HP now scales up with the max, so a unit at 50% stays at 50%. When it ends, the current HP scales down. March kept the current HP and never clamped it on the way down: `SetMaxHp` does not clamp.
- **Watch: game transitions.** `X2Game.cpp:1995/2101` run `Init()`, which resets max HP to base, and then `SetKBuffFactorFromGageManagerToGameUnit`, which re-applies saved buffs through the packet path into `InsertAdditionalStat`. A unit at full HP therefore comes back at full *buffed* HP, where March left it at the base value. June's own transition order was not traced; it is listed for the play-test, not ruled.
- **Hand-off to Phase 5b:** BBT 84 (`…_MAX_HP_BY_CONDITION`) must get its own case. Phase 1c item 3 says 84's blocks do not rescale, so it must not fall into case 6.

**2. DRAW_TEXT (`X2BuffBehaviorTemplet.cpp/.h`, `X2Data.cpp/.h`).**
- **The call** (`32e85fe0` @`32e86090`): `DrawText( unit, m_eChangeType +0x14, m_fCoolTime +0x18, m_fCoolTimeRate +0x1c )`. It fires only on `+0x46` (DRAW_TEXT) and a true reset result. The rate is passed but never read.
- **The text:** `StringCchPrintfW` (`31aa55f0`) into 20 wide chars, with STR 38799 and 38800. Formats (read from the PE): SWAP `%s = %d%s` (`33535608`); FIX `%s +%d%s` (`3353561c`) when the value is ≥ 0 or NaN (`test ah,5 / jp`), else `%s %d%s` (`33535630`). Any other change type draws the zeroed buffer, i.e. an empty string, which the port reproduces. The value is truncated to int (`_ftol2`).
  - The deployed March `data\general.ess` (XOR-16 UTF-16, loaded loose with `_wfopen`) holds 38799 = `COOLDOWN` and 38800 = `SEC`. A missing ID returns an empty string (`GetGeneralString`).
  - **Not ported:** June's string-ID debug view (`[S%1%]%2%`, behind a June-only toggle).
- **The draw:** `g_pData+0x17c` → `DrawText( text, vslot 63, vslot 91, AT_CENTER, 0.0, 0, 0 )`. User vslot 63 is `31fcac20` = `GetHeadBonePos` (head frame, y + 70.0, no further offset); vslot 91 is `3208dfe0` = `GetDirVector` (`+0x80b0`). The two trailing zeros are June-only parameters. With a zero gap time, March's `CKTDGPicChar::DrawText` emits particles at once, so no `OnFrameMove` or `Clear` is needed. June's X2Game pic-char loops (`322e5000`, `32308480`, `32309fb0`) indeed skip `+0x17c`; a `.text` scan finds no other reader of `g_pData+0x17c`.
- **The pic-char** (`31edd860`): `new CKTDGPicChar( m_pPicChar, UIMajor->CreateSequence( NULL, L"PicCharYellowSmall", 0,0,0, 0,0 ) )`, then width 18.0 (`33458e68` into pic-char `+0x1c`, `m_fWidth`). June's own `PicCharYellow` (`31edd470`) has the same shape with width 30.0 (`33441b9c`), so the mapping is exact. June creates it from the loading state (`3258c470` @`3258da73`) among its June-only pic-chars (`+0x170`..`+0x180`). The port calls it from `LoadPicChar5`, which covers both March loading paths (`X2StateLoading.cpp:533/1090`) and `LoadPicCharAll_LUA`. The ctor nulls it and the dtor deletes it. A missing sequence leaves it NULL and writes one `[JUNE_BUFF] unsupported (data not packed)` line to `ErrorLog.txt`; the draw then does nothing.
- **Glyphs:** the copy constructor shares the base `PC_` group (ASCII 32–124), which already draws letters for March's `"HP +%d"`.
- **Data, for the user to pack.** `kom_contents/data036_kom/UIMajorParticle.txt` now carries June's `eventsequence "PicCharYellowSmall"` block (818 B, copied byte for byte from `14JUN17/data/data036_kom/UIMajorParticle.txt`). It sits right after `PicCharYellow`, with no separator line, which matches the surrounding blocks.
  - Before: 592,530 B, md5 `03d57782df6aa386f0ccf160cce00b50`, equal to the live `data036.kom` entry (the part 4 review's fresh extract). After: 593,348 B, md5 `52e4308cee235b5a549566a622b47f52`. CRLF throughout; the file's 48 non-ASCII bytes are untouched.
  - **Parser check:** every identifier token in the block already occurs in March's file, except the sequence name. So March's particle parser accepts it.
  - The manifest already lists the file, so it is unchanged. Its value is `"2"`, which looks like it breaks the non-lua → 0 packing rule, but the live archive settles it: `elstools inspect --input data\data036.kom` lists `UIMajorParticle.txt` at algo 2, and so are all 32 `.txt` entries in `data036.kom` (only its 2 `.xml`, 1 `.ini` and 1 `.lua` are algo 0). So `"2"` is right, and the rule has this exception.
- **Why only the block, not June's whole file.** 10 of March's 857 sequence names are absent from June's `UIMajorParticle.txt`. One of them, `UI_Eve_Party_Event_StartEffect`, is read by `X2GUUser.cpp:25262`; June keeps it in `UIMajorParticle_INT.txt`. March also loads `_INT` (`X2StateStartUp.cpp:216`, `INT_RESOURCE_DEVIDE`), so a wholesale adopt must bring June's `UIMajorParticle.txt` and `_INT` together, checked as a pair. **Open item for the June UI parity work.** The other 9 missing names (`CHUNG_HOWLING01/02`, `CHUNG_STEEL_EDGE02`, `Nasod_King_SpellAttack_Splitlight06`, `PET_PETTE_BERTHE_STEB_3_Attack_C`/`_C_02`, `Pulsewave_Summon_Boss_SpecialAttackB01/02`, `ShockWave_Summon_Boss_SpecialAttackB`) have no C++ reader; Lua readers were not checked.
- **Reach on March data: none until June data arrives.** March's RSCT factors carry no `DRAW_TEXT` key (part 2's census).

**3. The dormant chain: ported dormant (`3bbf7d2`).** The map first, then the port.
- **`322a7660( BFI, count, level, creatorUID )`**, "set a buff's stack count":
  1. It gets the factor (`326c32a0`) and finds the unit's templet with that BTI (`322ae0a0` over `+0xe14`).
  2. With count < 1 it erases the buff (`3229d320`).
  3. Otherwise vslot 132 must return 1. A levelled factor copy is then built (`32089f60`, level).
  4. For users only (`+0x264 == 0`), it calls vslot 476( factor, creatorUID or `+0x9274` ), then vslot 141( factor, 0 ).
  5. Finally `3236a430( count, unit, factor )`.
- **Unit vslots** (from all 19 `CX2GU*` vtables, found through RTTI):

  | Slot | Base / user / NPC | March counterpart |
  |---|---|---|
  | 132 (`+0x210`) | `3229a2e0` / `32045dc0` / `327bdb60` | June's `CanApplyBuffToGameUnit( BTI, factor )`. With the null factor `322a7660` passes, the user body skips its June-only templet-flag checks (`+0xa9`/`+0xaa`, creator/party) and runs the base check `3229a2e0`, then "not `DIE_FRONT`/`DIE_BACK`" (vslots 280/281). That is March's `CX2GUUser::CanApplyBuffToGameUnit()` (`X2GUUser.cpp:5553`) |
  | 141 (`+0x234`) | `322b5630` everywhere | empty (it only releases its argument). Ain (`32a6aa10`) and Rose (`32ac8f10`) override it: a dormant hook |
  | 476 (`+0x770`) | users only: `32028250` in all 13 user classes; base and NPC vtables end before it | June-only: adjusts the levelled factor by an index (`31fdbbb0`, types 0xf/0x10) taken from the creator-UID argument or unit `+0x9274`. March has neither. Port as a logged no-op |

- **`3236a430( count, unit, factor )`** = `CX2BuffTemplet` "set the stack count":
  1. Every behavior's vslot 3 runs with `( factor, unit, templet, count )`; a true result sets its start flag (`+0xc`).
  2. If the stack-count entry exists (`+0x54` map, key 0 = March's `UCM_ACCUMULATION_COUNT_NOW`), it is set to `(float)count`, and every displayer restarts (displayer slot 1, then start flag).
  3. With `+0xa8` set, an icon refresh with the count follows (`322ab560`, `324943c0`, `32489810`, `32580100`). That is the `32489810` tail Phase 4a already left unported, because March's gage icon shows no count (`RestartDisplayerForAccumulationDecrease`).
  - `32369da0` = `GetAccumulationCountNow` (the rounded map value).
- **Behavior slot overrides** (all 38 behavior vtables, through RTTI). Defaults: 3 = `32e864d0` (true), 7 = `32e72980`, 9/10/11 = `3208ddc0` (empty).
  - Slot 3 is overridden by ChangeStat (`32e786d0`, also on `_HaveBuffOwner`), NowMp (`32e7c680`) and EnchantAttackRate (`32e775e0`).
  - Slots 9/10 are overridden by NowMp (`32e893a0` `+=`, `32e89430` `*=`), slot 11 by ChangeStat (`32e86500` `=`).
  - Slot 7 is overridden by ChangeStat (`32e73500`, ported in 4a as `OnAccumulationCountDecreased`) and **ChangeSkillDamage** (`32e7c020`, Phase 6's June-only class).
  - Every body was read in raw asm (none but `32e7c680`/`32e893a0` is carved in Ghidra). All slot-3 overrides take `( factor, unit, templet, count )` (`ret 0x10`; offsets traced through each push).
- **The port, March names.**
  - Behavior base (`X2BuffBehaviorTemplet.h`): `OnAccumulationCountSet` (slot 3, true), `AddValueByOperation` / `MultiplyValueByOperation` / `SetValueByOperation` (slots 9/10/11, empty).
  - **NowMp slot 3** (`32e7c680`): change-type check, `SelectDelegate()` for FIX/PERCENT, `v = (1 - resist) * v[1]` (`GetValueOrPercentPerSecond`), and when started with a stack count `v *= 1 + m * (count - 1)`; no larger-absolute pick. Slots 9/10 (`32e893a0`/`32e89430`): `m_fResultValue += v` / `*= v`.
  - **ChangeStat slot 3** (`32e786d0`): `v = GetValue( factor, unit, resist )`. Not started: `count * v`, then `InsertAdditionalStat`. Started with no stack count: `count * v`. With one: multiplier ~ 0 returns false; FIX/RELATION `count * v`; PERCENT `1 + (v - 1) * count`, clamped at 0 (`fcom / test ah,0x41`); SWAP keeps its value. Then `UpdateAdditionalStat` (`3226fd70`), so BBT 6 gets no HP rescale on this path, as item 1 found. Slot 11 (`32e86500`): a plain store to `m_fResultValue`.
  - **EnchantAttackRate slot 3** (`32e775e0`): change-type check, the per-type attribute, FIX/PERCENT only. Not started: insert `v[1]`. Started with a count: multiplier ~ 0 returns false, else erase and insert `v[1] * (1 + m * (count - 1))`. Started with no count: erase and insert `v[1]`.
  - `CX2BuffTemplet::SetAccumulationCountNow( count, unit, factor )` (`3236a430`): every behavior's slot 3, `SetStart( true )` on success; then, only if `UCM_ACCUMULATION_COUNT_NOW` exists, the count is stored and each displayer runs `StartDisplayer` + `SetStart( true )` (displayer slot 1 + `+0xc`, the same pair Phase 4a's `3236a190` uses). The icon-count tail is not ported (Phase 4a's ruling). The behaviors run before the count is stored, as in June, so NowMp and ChangeStat test the *old* stored count while scaling by the new one. `GetBehaviorPtrList` is June's `3236c8e0` (copy of the behavior list).
  - `CX2GameUnit::SetBuffAccumulationCount( BFI, count, level, x )` (`322a7660`): factor by BFI; the unit's first templet with its BTI; `count < 1` erases at once through `EraseBuffTempletFromGameUnitByIterator` (`3229d320`: counters, slot 148 `UnSetSpecificValueByBuffTempletID`, finish calls, icon erase, vector erase); otherwise `CanApplyBuffToGameUnit()` (slot 132 with no factor; base `3229a2e0` = March's base check exactly, NPC `327bdb60` = March's NCT_BASIC test plus a June-only per-BTI step on NPC `+0x8f70` that is not ported), `new CX2BuffFactor( *factor, level )` (`32089f60`), for users slot 476 then slot 141, then `SetAccumulationCountNow`.
  - `AddBuffAccumulationCount( BFI, delta, level, x )` (`322a8a70`): the count from `322a0090` (first `GetBuffFactor()` entry with that BTI, its `m_ucAccumulationCountNow`, else 0) plus `delta`.
  - `OperateBuffBehaviorValue( op, BTI, index, value )` (`3227c360`): the first templet with that BTI (null templets skipped), behavior `index` (unsigned bound check), op 0/1/2 = slot 9/10/11 (`sub eax,0 / dec / dec`), other ops nothing.
  - Slot 476 is `CX2GameUnit::AdjustBuffFactorByIndex` (empty) with a `CX2GUUser` override that logs `[JUNE_BUFF] unsupported (June-only system)` once; it is called only for `GUT_USER`, as June does. Slot 141 is `OnBuffAccumulationFactorReady` (empty, Ain/Rose override it in June).
  - `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7 already listed every caller; its two rows now name these March functions and the `x` argument. Its `AnimPlay` row had part 3's `+0x8040` slip, now `+0x8080`.
- **Reach: none on March data** (part 3 review: every caller of `322a7660`, `322a8a70` and `3227c360` passes a June-only BFI or BTI).

**4. AnimStop's `+0x8080` marker: no March path asks for it.** A fresh `.text` scan finds 73 `+0x264` sites. 9 are data loads of the unit-type field (the base register is not loaded from a vtable). Of the 64 vtable calls, 57 push 0, 1 pushes 1 (`3229a669`, June-only, part 4), and 6 push a register. None of the 6 is a unit `IsSuperArmor` with 1:
- `324bf210` and `324dc170` push four arguments, so this is another class's slot at that offset.
- `3252c84e` calls through `dynamic_cast<CX2PVPGame*>( CX2Game )` (`__RTDynamicCast` `32fe2daa`, type descriptors `33722a70`/`33732770`): the object is a game, not a unit.
- `327c92b0` is `CX2DungeonGame`/`CX2PVEGame` slot 159, calling the game's own slot 153.
- `3276f4b9` is NPC code. NPC slot 153 (`32758440`) ignores its argument: list `+0x42a0`, else `+0x8374`.
- `32b3727c` is reached by a jump from `32b3713d`, which pushes 0, in Lu/Ciel's Bluff (`EffectSet_A_LLD_Bluff_*`); the scan saw an unrelated push before the jump.

So by part 4's rule the marker is not ported: nothing in March could observe it.

**Hygiene.**
- A fresh byte-level unifdef of the flag: all 5 code files reduce byte-for-byte to `d34de09`, and each raw file differs from it.
- `--numstat` shows 111 insertions and 9 deletions. Every deletion is our own flagged text: part 2's DRAW_TEXT log stub (6 lines, plus 2 doc-comment lines) and one member comment.
- Inserted lines are ASCII, and line endings follow each host line (`X2Data.h` is mixed; its insertions are LF, as their neighbours are). The `file` signatures of all 5 files are unchanged.
- **Chain (`3bbf7d2`):** 8 code files, 330 insertions and 0 deletions; each reduces byte-for-byte to `30aa0cb` with the flag undefined (one leading blank line in `X2GUUser.cpp` was first outside the guard and was moved inside before the build). `file` signatures unchanged.

**Build and deploy.**
- KTDXLIB (for `d34de09`'s `Always.h`), X2Lib (with `stdafx.cpp` touched) and X2, at `US_SERVICE`: rc 0, 0 errors. No warning in a touched file.
- A whitespace-only fix in `X2Data.cpp` (one blank line moved inside the guard) was forced through a recompile and a relink.
- `x2.exe` is 19,018,752 B, linked 07:16:09, md5 `88e67754d472c1b37f34e528b732a8b3`. `X2.map` carries `LoadPicCharYellowSmall`, `GetPicCharYellowSmall` and the RSCT `DrawText`.
- Copied to the game directory as `X2_offline.exe` (the name was read programmatically); size and md5 match. The debug build is gone.
- **After the chain (`3bbf7d2`):** X2Lib (with `stdafx.cpp` touched) and X2 again, rc 0, 0 errors; X2 rebuilt its PCH and `X2.cpp`. The only warning in a touched file is the existing C4018 (`X2GUUser.cpp:17894`, shifted). `x2.exe` is 19,023,360 B, linked 07:43:19, md5 `b7379faae99d540c76870169cf912880`; `X2.map` carries the new symbols. Deployed as `X2_offline.exe`, size and md5 matching. **This is the deployed exe. Not launched.**
- The `CX2Data` layout changed (a new member mid-class), so X2's own PCH had to rebuild with it: it did (`dxstdafx.cpp`, `X2.cpp`, the only `g_pData` user in `X2/`), so no stale object reads shifted offsets.
- **Pack:** `kom_contents/data036_kom/UIMajorParticle.txt` into `data036.kom`. Until then the exe logs the missing sequence once and draws no DRAW_TEXT; nothing else depends on it.

**Things to watch in play:**
- Blessing of Cronos, the Rally of Hero buffs (battle field) and the pet aura: current HP should keep its ratio when they start and end.
- Entering a new game while one of them is active.
- `ErrorLog.txt`: one `[JUNE_BUFF] unsupported (data not packed)` line until the pack, and none after it.

**Play-test (2026-10-04, user): passed** on the deployed exe `b7379faa…` (`3bbf7d2`). The user reported a pass without per-check detail, so this does not record which of the four checks under "Things to watch in play" were run, or whether `UIMajorParticle.txt` had been packed by then.

**Where part 5 stopped / what resumes (in this order)**
Part 5's list is done: items 0-4 above. 4b-i (behaviors) is complete apart from the open user decision.
1. **Runtime popups: decided (user, 2026-10-04): follow June, but always log.** No modal popup where June just returns false; every such failure (missing buff, unreadable factor, change-type mismatch, zero multiplier, unknown type) writes a `[JUNE_BUFF]` line to `ErrorLog.txt`. Planned as 4b-ii's first item: wrap the runtime entry points (apply, packet apply, per-frame) in a `CX2JuneBuffCapture` and report, deduped per templet and message so a per-frame error cannot flood the log. It covers behaviors, finalizers and displayers alike.
2. 4b-ii: finalizers and displayers, including the Phase 4a review's `BFT_TIME` hand-offs (Phase 4b's scope).
3. Open, owned elsewhere: the damage roll `322788c0` (Phase 5c), the enchant rework `32272d90` (Phase 15), BBT 84's own case (Phase 5b), a wholesale `UIMajorParticle.txt` + `_INT` adopt (June UI parity).

**Review of part 5:** required (Opus 5.5). Suggested spot-checks:
- `32283181`'s `FUCOMPP`/`test ah,0x44`/`jnp` reading (now == 0 → no rescale);
- insert's ratio taken before `31e8c940`;
- `32e85899`'s sign test;
- whether `32e857d0`'s other change types really draw an empty string (the `LAB_32e85f47` path with the zeroed buffer);
- the slot-153 site list (73 sites; the 9 non-vtable loads);
- `32e786d0`'s PERCENT clamp and SWAP fall-through, and `32e775e0`'s started-with-no-count path (erase + insert with no larger-absolute pick);
- `3236a430`'s order (behaviors before the stored count) and its displayer step only when the count entry exists.

**funcmap.tsv:** 35 rows ("Buff Phase 4b-i part 5"): 20 with the items 1-2 commit, 15 with the chain.

**Kickoff for the part 5 review** (Opus 5.5 high). It runs on its own; it is read-only unless it finds a code defect, which it then fixes, rebuilds and deploys.

```
Continuing the June buff/debuff port. Run this session on Opus 5.5 at high effort. Read
JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", the "June buff function
map", "Phase 4b-i part 4 -- Review" and "Phase 4b-i, part 5". Branch: mods/june-buff-port.
Never launch X2_offline.exe.

Your task: an independent review of Phase 4b-i part 5. Code: 7ea72b2 and 3bbf7d2 (under
SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY); d34de09 reverts the part 4 debug commit; the doc
commits are 30aa0cb and 5e5adef, plus the manifest note after them. Use fresh scratch tools; do not reuse part 5's
patch or unifdef scripts. Run the flag-off unifdef on the committed blobs against d34de09,
and take numstat from git diff d34de09 3bbf7d2.

Check, in Ghidra (read-only) and the raw asm, the part 5 section's suggested spot-checks,
plus: the staged kom_contents/data036_kom/UIMajorParticle.txt equals the pristine live copy
with only June's PicCharYellowSmall block inserted (md5s in the section); every "same as
March" claim in the dormant-chain map (3229a2e0, 327bdb60's null-factor path, 3229d320,
32089f60); and the March-data reach of BBT 6 (5 templets) on a fresh extract.

Report numbered findings (R1, R2...) with a severity. Fix a code defect under the same flag
(insert-only, byte-level CP949 edits, flag-off unifdef per file), rebuild X2Lib and X2 at
US_SERVICE (touch stdafx.cpp after a header edit), deploy as X2_offline.exe, record the md5,
and ask before launching. Add confirmed funcmap.tsv rows. Another session may have
uncommitted edits in the doc: stage only your own hunks. Commit. Record the review cost line.
```

**Kickoff for 4b-ii** (Opus 5.5 high):

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger", the "June buff function map",
"Phase 4b" and every 4b-i part and review after it (parts 1-5). Branch: mods/june-buff-port.
Never launch X2_offline.exe without asking first.

Your task (Phase 4b-ii): first, the runtime-popup decision ("Where part 5 stopped" item 1:
no popup as in June, always a deduped [JUNE_BUFF] line in ErrorLog.txt, via CX2JuneBuffCapture
around the buff runtime entry points). Then bring every EXISTING March buff finalizer (14 BFT) and displayer
(14 BDT) to June parity, including the Phase 4a review's BFT_TIME hand-offs (the full
duration as an appended third packet value, 32ec5290/32ec53d0, and the debuff-duration
multiplier in 32ec27d0). Per type, diff the factor parse case, the level-slice case
(32d4cb00), the templet class virtuals and the unit-side stores against June, and port
June's whole function wherever it differs. Keep GetFactor/SetFactorFromPacket order (append
only). Under SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY: byte-level CP949 edits, insert-only,
flag-off unifdef check per file. buff_census.py --phase 4b must show the finalizer and
displayer pairs cleared. Another session may have uncommitted edits in the doc: stage only
your own hunks. Build, deploy, commit; record where you stopped.
```

### Phase 4b-i part 5 — Review (2026-10-04, Opus 5.5 high)

Independent review of code commits `7ea72b2` and `3bbf7d2` (under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`), the revert `d34de09`, and doc commits `30aa0cb`, `5e5adef` and `5bc9586` (the manifest note). Read-only on code: nothing was edited, built or deployed.

Fresh tools only; part 5's patch and unifdef scripts were not reused:
- Ghidra, read-only (one decompile, `32e7c680`).
- A new capstone reader over a hand-parsed PE section map (`pefile` is not installed), plus a raw-byte scanner for `.text`. `.text` also holds rdata, so nothing relies on a linear sweep.
- A new byte-level unifdef. It resolves only the one flag, reads `git cat-file` blobs, and has a negative control.
- A fresh `elstools extract -v mar-2014` and `inspect` of the live `data036.kom`, then `decompile-batch` of `BattleFieldBuffTemplet.lua` (152,564 B), `BattleFieldBuffFactor.lua` and `BuffTemplet.lua`.

**Verdict: no code defect.** Every suggested spot-check passes in the asm, and so do the extra checks. R1-R8 are Low or Info: doc wording, dormant-path notes and play-test evidence.

| # | Check | Result |
|---|---|---|
| 1 | `32283181` `FUCOMPP` | **Pass.** `fld now; fldz; fucompp` compares 0 with now. `test ah,0x44 / jnp 322837d6` leaves only on equal (C3 alone has odd parity). Unequal (`0x00`) and NaN (`0x44`) have even parity, so both rescale. Then `GetMaxHp() / oldMax` (`fdiv [esp+0x10]`, before the `push`) × now. The port's `0.0f != fNowHp` behaves the same, NaN included. |
| 2 | Insert's ratio before `31e8c940` | **Pass.** `31e4f900` (now) @`3228274d` and `31e551a0` (max) @`32282758`; `fdivr` = now / max, kept at `[esp+0x18]`. Only then come the `+0xc04` null test, `SetMaxHp` @`32282795` and `SetNowHp( result × ratio )` @`322827c2`. June computes the ratio even without gage data; the port computes it inside the guard, with the same effect. **Helpers:** both getters return 1.0 with no gage and 0.0 with a gage but no HP block (`[gage+0x20]`). March's `GetNowHp` returns 1 with no gage; March's `GetMaxHp` has no guard, but the port calls it only inside one. |
| 3 | `32e85899`'s sign test | **Pass.** `fcomp qword [334350e8]` (= 0.0) / `test ah,5` / `jp 32e85acc`. It falls through only when the value is < 0 (C0 alone), to `33535630` `"%s %d%s"`. A value ≥ 0, or NaN, goes to `3353561c` `"%s +%d%s"`. Type 0 goes to `32e85d12` and `33535608` `"%s = %d%s"`. Every branch passes `( buf, 0x14, fmt, STR 0x978f, _ftol2( v ), STR 0x9790 )`, i.e. COOLDOWN, the int, SEC, as ported. March `BCT_SWAP_VALUE` = 0, `BCT_FIX_VALUE` = 1. |
| 4 | Other change types draw an empty string | **Pass.** `sub eax,0 / je` takes 0; `dec / jne 32e85f47` takes anything but 1. So type ≥ 2 reaches `32e85f47` with the 20-wchar buffer at `esp+0x4c8`, zeroed at entry (`32e85822…69`, 40 B). The draw: `[g_pData]+0x17c` (null → skip), then `33228fe0( buf, head pos, *vslot 91, 2, 0.0, 0, 0 )`. The head position comes from vslot 63 at entry (@`32e8587a`), before the text is built. March `AT_CENTER` = 2. User vslot 63 (`31fcac20`) is the head frame's position + 70.0 (`33472990`), which is March's `CX2GUUser::GetHeadBonePos` (`X2GUUser.cpp:13059`). |
| 5 | Slot-153 site list | **Pass; one count differs (R1).** A raw-byte scan for `64 02 00 00` behind a mod-10 ModRM finds 658 `+0x264` operands in `.text`, none of them a direct `call/jmp [reg+0x264]`. 74 are `mov r,[b+0x264]` followed by a call within 8 instructions. In 64 of them `b` was loaded from `[obj]` (a vtable). Of those, 57 push 0, 1 pushes 1 (`3229a669`), and 6 push a register (`324bf210`, `324dc170`, `3252c84e`, `3276f4b9`, `327c92c0`, `32b3727c`). That is the doc's split exactly. **`32b3727c`:** its block starts at `32b37274`, and the instruction before that is a `jmp`. Within ±0x10000 the block has exactly one predecessor, `jne` @`32b3713d`, with `push 0` @`32b37137`, and no absolute reference. |
| 6 | `32e786d0` PERCENT clamp, SWAP | **Pass.** Not started (@`32e78815`): `count × v`, then `InsertAdditionalStat` (`32282700`). Started with count 0 (`32369da0`): `count × v`. Started with a count: `IsSamef( 32369f10(), 0 )` returns false. Types ≤ 0 or > 3 keep the value, so SWAP falls through to the update. Types 1-2 take `count × v`. Type 3 takes `(v − 1)·count + 1`, then `fldz / fcom st(1) / test ah,0x41 / jne` keeps it when 0 < r, r = 0 or r is NaN. Only r < 0 stores 0, which is the port's `< 0.0f`. Then `3226fd70` (update). The multiplier only gates here: FIX and PERCENT never scale by it, while NowMp and EnchantAttackRate do. That asymmetry is June's own, ported as is. `ret 0x10`; the count is `fild dword`. |
| 7 | `32e775e0`, started with no count | **Pass.** The attribute table @`32e777d8` maps 66→1, 67→2, 68→3, 69→4, 70→5, 71→6 (March BBT 66-71, `BEAT_*` 1-6). Types other than 1 and 3 return false. Not started: insert `v[1]` (0 past the end) through `32281990( identity, rate, attr, type )`. Started with count 0 (@`32e7771d`): erase through `32279c70( identity, attr, type )`, then insert `v[1]`, with no larger-absolute pick. Started with a count: a zero multiplier returns false before the erase; otherwise erase, then insert `v[1] × ((count − 1)·m + 1.0)` (`334355b0` = 1.0). |
| 8 | `3236a430`'s order | **Pass.** Behaviors first: slot 3 gets `( factor, unit, templet, count )`, and `+0xc = 1` on true. Then, only if the map is non-empty and key 0 is found, `fild count` is stored at `[node+0x10]`, and every displayer gets slot 1 `( unit, templet )` and `+0xc = 1`. The `+0xa8` icon tail comes last (not ported). So the behaviors test the old count, as the doc says. |
| 9 | Staged `UIMajorParticle.txt` | **Pass.** The staged file equals the live entry (593,348 B, `52e4308c…`): the user has packed it. Cutting `[29845, 30663)` (818 B) leaves 592,530 B with md5 `03d57782…`, the pristine copy. The block equals June's bytes `[35258, 36076)`. In both files it follows `PicCharYellow`; next comes `ScoreBonusPicChar`. It has 24 CRLF lines, no bare LF and no non-ASCII byte; the file's other 48 non-ASCII bytes are untouched. The block's only string value is its name: there is no texture key, because the pic-char supplies the glyphs. |
| 10 | "Same as March": `3229a2e0` | **Holds.** If byte `+0x4104` is set (March `m_bImmuneToExtraDamage`), it returns false. With gage data, it continues only when the now HP > 0 (`fcomp`/`test ah,5`/`jp`; 0 and NaN fail; no HP block fails). If vslot 67 == 4 (March `GUSI_DIE` = 4), it returns false. With no gage the HP test is skipped (March's `GetNowHp` returns 1). The three tests and their order are the same as March's `CX2GameUnit::CanApplyBuffToGameUnit`. |
| 11 | `327bdb60`, null-factor path | **Holds; one wording fix (R3).** It returns false if the NPC templet's class (`+0x85f8` → `+4`) is not 0 (March: not `NCT_BASIC` → false). Otherwise it calls the base `3229a2e0( BTI, null )`. Then comes the June-only `+0x8f70` step. |
| 12 | `3229d320` | **Holds for March data (R4).** In order: the counters (`+0x4235` debuff / `+0x4236` buff, chosen by `+0x64`); vslot 148 with the BTI; `3236a630` = every behavior's `DoFinish( unit, templet )` (slot 6); `3236a660` = every displayer's slot 3, then `+0xc = 0`; `3236a6a0` = the BFT 15 hook; the icon erase on `+0xa8`; the vector erase (`322b4b00`). That is March's order, plus the BFT 15 hook. |
| 13 | `32089f60` | **Holds.** `new 0x70`, refcount 0, then `320844a0( src+8, level )` (the levelled `BUFF_FACTOR` copy), then `+0x68`/`+0x6c` are copied. Those are the 8 B after the 0x60-B `BUFF_FACTOR`, whose BTI sits at `+0x28`: the identity, which March copies as `m_DataBuffIdentity`. `m_bApplyToSpecialAttack` is June's per-list-entry bool and is not part of this object, so the port's `false` is right. |
| 14 | The rest of the chain | **Pass.** `322a7660` ends in `ret 0x10`: four dwords, so the port's `int` 4th argument has the right width (but see R2). `322a8a70`: the pushes of `322a7660`'s level and `x` precede the call to `322a0090` (`ret 4`), so the call is `322a7660( BFI, (UCHAR)count + delta, level, x )`. `3227c360`: the first templet with the BTI (`+0x34`), null templets skipped, an unsigned `jb` bound check; ops 0/1/2 → `+0x24`/`+0x28`/`+0x2c`. NowMp `32e7c680` (decompiled): the delegate is rebound for types 1 and 3; `(1 − resist/EAR_MAX)·v[1]` is March's `GetValueOrPercentPerSecond`; `(m(count − 1) + 1)` applies when started with a count. |
| 15 | Hygiene | **Pass.** **Unifdef:** `strip( 3bbf7d2 ) == strip( d34de09 )` for all 10 code files, and `strip( 7ea72b2 ) == strip( d34de09 )` for its 5. The raw blobs differ, and a wrong-flag control fails. **`numstat d34de09..3bbf7d2`:** code 441/9 (`X2BuffBehaviorTemplet.cpp` 181/8, `.h` 24/1, the rest insert-only), which is part 5's 111 + 330. The 9 deletions are part 2's DRAW_TEXT stub and comments. All 441 added lines are ASCII and CRLF, except `X2Data.h`'s 9, which are LF like its host's 678 LF lines. `file` signatures are unchanged on all 10. **`d34de09`** equals `16a6d78^` byte for byte on its 4 files. **Deploy:** `X2_offline.exe` = `X2/US_SERVICE/x2.exe`, 19,023,360 B, md5 `b7379faae99d540c76870169cf912880`. The game directory holds only `x2.exe` and `X2_offline.exe` (names read programmatically). |

**Runtime traps, checked because DRAW_TEXT and the chain never run on March data**
- `GET_STRING` is `GetGeneralString( id ).c_str()` on a `const wstring&`. Both macro forms yield a `const wchar_t*`, and a missing ID returns `m_wstrEmpty`. Passing it through `StringCchPrintf`'s `...` is safe.
- `CreateSequence` returns NULL for a missing name, with no log and no default sequence, so the degrade path really fires.
- `LoadPicChar5` runs once per load. The Lua re-entry `LoadPicCharAll_LUA` calls it only while `m_pPicCharYellow` is NULL. So "logged once" holds.

**March-data reach (fresh extract)**
- **BBT 6: 5 templets,** counted by behavior list in the decompile: `BTI_BUFF_BELSSING_OF_CRONOS`, `BTI_BUFF_RALLY_OF_HERO_LEVEL_1/2/3` and `BTI_BUFF_PET_AURA_SKILL`, all `BCT_PERCENT`.
- The file has 321 templet entries, which matches `ErrorLog.txt`'s "320 loaded, 1 duplicate". 19 factor entries carry BBT 6.
- `BuffTemplet.lua` (the premium-buff manager's file) has none.
- In C++, every `Insert`/`EraseAdditionalStat` call is ChangeStat's (`X2BuffBehaviorTemplet.cpp:1101/1142/1151/1213`). Nothing calls them with `BBT_CHANGE_MAX_HP` directly.

**Findings**
- **R1, Info (count).** My filter finds 10 non-vtable `+0x264` loads followed by a call; the doc says 9. None of them is a slot-153 call: each of those calls goes through `+0x54` or `+0x428`. The AnimStop verdict stands.
- **R2, Low (dormant; affects the slot-476 log only).** `322a7660` falls back to unit `+0x9274` when its 4th argument **equals 0** (`test ecx,ecx / jne` @`322a777b`), not when it is negative.
  - The port passes the argument straight to `AdjustBuffFactorByIndex`, which logs only for ≥ 0. Slot 476 is a logged no-op, so nothing else differs.
  - Whoever ports slot 476 (the character port) must treat 0 as "use the unit's `+0x9274`". `JUNE2017_CHARACTER_PORT_CHECKLIST.md` row `322a7660` already says so; only the part 5 section and the code comment leave it out.
  - Recorded in a superseding funcmap row. No code change.
- **R3, Info (doc wording).** Part 5 calls NPC `327bdb60` "March's NCT_BASIC test plus a June-only per-BTI step".
  - When NPC `+0x8f70 > 0` and the BTI is > 0, `326c31c0`'s result *replaces* the base result (`mov bl,al` @`327bdbe1`). So June can return true for an NPC that the base check rejects.
  - It is not ported, and March has no `+0x8f70`, so there is no effect.
- **R4, Info (the hand-off already exists).** `3229d320` calls the BFT 15 finish hook `3236a6a0`; March's erase lacks it. `SetBuffAccumulationCount` now reaches that erase as well, and Phase 8's item (`EraseBuffTempletFromGameUnitByIterator` + `3236a6a0`, under "Phase 8") covers both callers.
- **R5, Info (dormant popup).** March's `GetAccumulationMultiflier` raises `DISPLAY_ERROR` when key 1 is missing; June's `32369f10` silently returns 0.0.
  - The new slot-3 overrides call it only when key 0 exists. Both templet parsers (`X2BuffTemplet.cpp:46-47`, `:128-131`) insert keys 0 and 1 together, and the packet path (`:262`) only updates key 0 on a parsed clone. So data cannot reach the popup.
  - It belongs on 4b-ii's popup sweep anyway.
- **R6, Info (doc).** `32369da0` truncates (`or 0xc00` = round toward zero); the doc says "rounded". March's `static_cast<UINT>` truncates too, so no change is needed.
- **R7, Info (play-test evidence).** The part 5 play-test ran on the packed particle file:
  - `data036.kom` is dated 07:56. `ErrorLog.txt`'s only session starts at 07:56:40 and has no `data not packed` line. The live entry equals the staged file.
  - That settles the open point in part 5's play-test note. It does not show a DRAW_TEXT on screen: no March factor carries the key.
- **R8, Info (manifest note confirmed).** Fresh `inspect` of `data036.kom`: 180 entries. Algo 2: 143 `.lua`, all 32 `.txt` and 1 `.kim`. Algo 0: 2 `.xml`, 1 `.ini` and 1 `.lua` (`PvPDropTable.lua`, the known exception). `UIMajorParticle.txt` is algo 2 at 593,348 B.

**funcmap.tsv:** 8 rows ("Buff Phase 4b-i part 5 review").
- 3 are new: `32281990`, `32d41780` and `32fe25f0`.
- 5 supersede or refine rows: `322a7660` (R2), `327bdb60` (R3), `32369da0` (R6), `32369f10` and `3229d320` (R4).

**Build and deploy:** none, because there is no code defect. The deployed exe is still `b7379faa…` (`3bbf7d2`).

### Phase 4b-ii — RE (2026-10-04)

Recorded before any 4b-ii code. Ghidra read-only; uncarved slots read with a fresh capstone reader over the PE (scratch only).

**Factor side, all 14 BFT: same as March.**
- Parse switch `32d4c750`: 1 `32d42da0`, 2 `32d430f0`, 3 `32d433d0`, 5 `32d436b0`, 6 `32d43b00`, 7 `32d43e90`; 4 and 8–14 the empty parse `32d44230`. That is March's `CASE_FINALIZER_FACTOR(_SKILL)` list.
- The six parses read the same keys in the same order and push the same floats: Time `[type, table]` (NORMAL_TIME / RANDOM_TIME / FORCE_TIME by type 1/2/3, anything else fails); Hit/Hitted `[type, COUNT table, 0.0]`; DamageValue `[type, FIX_VALUE table]` or `[2, 0.0, BUFF_RELATION_TYPE, MULTIPLIER table]`, other types fail; PassHp/PassMp `[PASS_UP 1/0, BUFF_CHANGE_TYPE, CRITERION table]`.
- Level slice `32d4cb00`: 1 `32d482d0`, 2 `32d48640`, 3 `32d487f0`, 5 `32d489a0`, 6 `32d48b90`, 7 `32d48d20`, 15/16 (Phase 8), default `{type}` with no values. Each equals March's `PushFinalizer*ToLevel`, including the HYPER_MODE_COUNT quirk (`[level+1] > 0` gates the push in both branches) and Hit/Hitted dropping BUCT 3/4.
- Finalizer SGU (second loop of `32d4d670`): 1 `32d419b0` (HYPER: `SetForceHyperModeTime(last)` + `GetHyperModeTime()`), 2 `32d412c0` / 3 `32d413b0` (`v[2] = v[1]` or `v[1] × GetAppliedBuffCount(false/true)`), 5 `32d414a0` (`UpdateValueByRelationType`). March's `SetGameUnitFinalizer` has the same four. **Correction to part 1:** "finalizer SGU has an extra case 5" — March already has `BFT_DAMAGE_VALUE` there.

**`BFT_RIDING_ON_OR_NOT` `FINISH_WHEN_RIDING_ON`: not read by June either (census exclusion, both origins).**
- The key's only reader is `32ebfd60`, which has no `E8`/`E9` caller and no pointer reference: it is the class's hiding, non-virtual `ParsingFinalizerTemplateMethod`, as in March (`X2BuffFinalizerTemplet.cpp`, `RIDING_SYSTEM`).
- Factory `32ec36f0` case 12 calls the base template `32ec0580` like every other case: `BeginTable(name)`, vtable `+0x30` (slot 12), `EndTable`. Riding's slot 12 is the base `32ec5990` (true). So `m_bFinishRidingOn` stays false in June as in March. The factor-level copies sit on the empty parse `32d44230`.

**`CX2BuffTimeFinalizerTemplet` (vt `33537e1c`, 16 slots).** Members: protected duration `+0x1c`, full duration `+0x28`, frozen `+0x2c`, FROZEN_TIME `+0x2d`, duration type `+0x30`, BTI `+0x34`, notified `+0x38`, last shown second `+0x3c`; three base floats `+4` (shake decreased so far), `+8` (SHAKE_DECREASE_MAX_TIME), `+0xc` (SHAKE_DECREASE_TIME_AT_ONCE). The copy ctor `32ec65a0` copies every one of them (base refcount and reserve reset, as March's base copy ctor), so March's implicit copy ctor is right.

| Slot | June | Verdict |
|---|---|---|
| 1 OnFrameMove | `32ec2670` | March's body (tick, 5 s notify, per-second `SetDurationTime`; `BUFF_ICON_UI` and `DISPLAY_BUFF_DURATION_TIME` are on), all inside `if ( !frozen )` |
| 2 SetFactor | `32ec27d0` | March's three duration branches (resist = unit slot 194 with the templet = March's `GetEnchantResist(regist)/EAR_MAX`; random = slot 114 `GetRandomInt(0)`), then `GetReduceDebuffDurationRate` (slot 167), then **new:** for a debuff templet (`+0x64`) that is not a world buff (`+0x66`), `× (1 − r)` when the unit's additional-stat result at `+0x7428` is > 0; `+0x28 = duration`; FROZEN_TIME hook; BTI and notified as March. No popups |
| 4 DidFinish | `32ebef20` | same (`reserve || duration <= 0`) |
| 5 GetFactor | `32ec5290` | appends `+0x28` as a third value (Phase 4a review R1) |
| 6 SetBuffTempletID | `32ec6840` | same |
| 8 restart | `32ec6b30` | ported in 4a |
| 11 new | `32ec3610` | shake decrease, below |
| 12 ParsingFinalizer | `32ebe8b0` | `BUFF_DURATION_TIME_TYPE` (required), `FROZEN_TIME` (bool, default false; also copied to `+0x2c`), `SHAKE_DECREASE_TIME_AT_ONCE` → `+0xc`, `SHAKE_DECREASE_MAX_TIME` → `+8` (floats, default 0) |
| 13 packet | `32ec53d0` | March's, then `+0x28 = size > 2 ? v[2] : 0` |
| 14 / 15 | `32ec3580` / `32ebef60` | set / get duration; no caller outside the vtable found |

- **Debuff multiplier (`+0x7428`).** It is `M+0xa0` of an additional-stat member at `+0x7388` (the next one is `+0x743c`, 0xb4 apart). No `InsertAdditionalStat` case writes it; the only seeds are `InitAdditionalStatOptionValue` (`@322711e9`: `m_SocketData +0x2dc`, a June socket-option field March's socket data lacks), the basic/passive initialisers and the refresh. **Port: dormant**, a `CX2GameUnit` value that stays 0.0 with no March writer, applied at June's point.
- **FROZEN_TIME.** At `SetFactor`, a frozen-capable finalizer sets `+0x2c = 1` and, for a user, `31fe2a80( BTI )` pushes `{ BTI, (WORD) protected +0x81fc }` into a user vector `+0x9450` (or updates that BTI's word). `CX2GUUser::CommonStateStart` (`@320606b0` → `3200bde0`) unfreezes and erases every entry whose word differs from user vslot 329 (`3208e410`: field `+0xc` of the record the protected state pointer `+0x7cbc` points at), through `3227beb0( BTI, false )` (each finalizer of that buff that `dynamic_cast`s to the Time class gets `+0x2c = false`). An NPC with the key is never unfrozen. **Not portable on March:** `+0x81fc` is a class-gated protected word (its other readers test `GetUnit()+0xaf0` type 10 = `UT_LUCIEL` and compare it to 1) and vslot 329 has no identified March counterpart. The only data is Rose's passive `BTI_BUFF_HEAVYFIREARMS_MASTERY` (`RNSI_P_RSR_HEAVYFIREARMS_MASTERY`). **Port: parse and store; never freeze; log once per BTI.** Freezing without the unfreeze would make the buff permanent.
- **SHAKE_DECREASE.** Slot 11 `32ec3610`: if `max > done`: `d = min( atOnce, max − done )`, `done += d`, `duration −= d`, return true; else false (base slot 11 `31e4c550` returns false). `CX2BuffTemplet` `3236c990` runs slot 11 on a copy of the finalizer list and returns true if any did. Its caller is `OnFrameMoveBuff` (`@3229d744`), per templet, only for my user unit and only when `CX2Game +0xbae` is set; a true result bumps the `+0x6dd0` sync counter (Phase 8). `+0xbae` is written once per frame in the `CX2Game` input routine `322f7470` (`@322f74ed`), the same block that feeds BFT 15's counter (`31fc9fa0`), from `322cfc60` = input object (`[g_pKTDXApp+0x1f8]`) action states `0x25` and `0x26` both set (`31c488c0`: byte `+0x4d + n`). **Port: the data side dormant** (keys, members, slot 11, the `CX2BuffTemplet` helper); the trigger goes to Phase 8 with the BFT 15 input block and the counter. One `[JUNE_BUFF] unsupported` line per BTI when such a finalizer starts. Data: only `BTI_AMUSEMENT_ITEM_WATER_BALLOON_IN_DAMAGE` (max 2, at once 0.1).

### Phase 4b-ii — Done (2026-10-04), launch check pending

Ran on Opus 5.5 high, under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`. The RE is the section above (`2faeaf7`); the code is `159fb91`.

**Hygiene.**
- 12 code files. `--numstat 146ce5a` shows 411 insertions and 31 deletions. The 31 deletions are parts 1–5's own flagged `return false;` lines, each turned into a logged exit (item 1).
- Every added line is ASCII and CRLF, as its host lines are, and the `file` signatures are unchanged.
- A fresh byte-level unifdef of the flag reduces all 12 files byte-for-byte to `146ce5a`. Each raw file differs from it, and a wrong-flag control fails.
- The first pass left 8 blank lines outside the guards; they were moved inside before the final build.

**1. Runtime popups (the user's decision: follow June, no popup, always log).**
- **`CX2JuneBuffCapture::RuntimeError( wide / narrow )`:**
  - Inside a capture (the Phase 3 load captures), the text is collected as before.
  - Otherwise it raises no MessageBox. Once per (BTI, text) it writes `StateLog` and `[JUNE_BUFF] runtime error, BTI <n>: <text> (June raises no popup here; written once per buff and text)` to `ErrorLog.txt`. `StateLog` sits inside the dedupe too: without the popup that used to throttle it, a per-frame error would otherwise write `State.log` every frame (fixed before the final build, `256c26d`).
  - The narrow overload exists because `DISPLAY_ERROR` is also called with table names.
- **`CX2JuneBuffRuntimeScope`:** a thread-local BTI. It is set at the top of every `CX2BuffTemplet` runtime entry: `SetFactor`, `SetFactorFromPacket`, the three `OnFrameMove*`, `DoFinishBehavior`, `DoFinishDisplayer`, `SetFactorToOnlyFinalizer` and `SetAccumulationCountNow`. Every behavior, finalizer and displayer runtime call passes through one of those, so a line names its buff. Outside them it prints `BTI 0`.
- **`DISPLAY_ERROR` is redirected** to `RuntimeError` (with the original's trailing `;`) in the five buff runtime files, and in `X2BuffTempletManager.cpp` (`256c26d`), where `GetBuffFactorPtrFromBuffFactorList`'s "uiBuffFactorId Invalid" runs during item and skill templet loading (`X2ItemManager.cpp:663`, `X2SkillTree.cpp:663/979`), outside any capture. The five runtime files are `X2BuffBehaviorTemplet.cpp`, `X2BuffFinalizerTemplet.cpp`, `X2BuffDisplayerTemplet.cpp`, `X2BuffTemplet.cpp` and `X2BuffFactor.cpp`. This covers every class popup part 3 listed (NowHp `SelectDelegate`, the mismatch and zero-multiplier popups, every "GetBehaviorFactor didn't work") and the part 5 review's R5 (`GetAccumulationMultiflier`).
- **`X2GameUnit.cpp`'s four buff runtime popups** are converted: "SkillTemplet Invalid", and "Behavior Type Invalid" in Insert/Update/`EraseAdditionalStat`.
- **The silent exits in parts 1–5's June bodies** now log: 31 `return false;` become `return DISPLAY_ERROR( L"<Class>::<method>: <reason>" )`. They cover "GetBehaviorFactor didn't work", "BUFF_CHANGE_TYPE MISS MATCH", "ACCUMULATION_MULTIPLIER is 0", "BUFF_FACTOR_ID is 0", "the user has no CX2Unit", "unknown BUFF_CHANGE_TYPE" and "BUFF_CHANGE_TYPE is neither FIX nor PERCENT". The four `ParsingBehavior` exits were left alone, because the Phase 3 capture already reports a failed parse.
- **Left as they are:**
  - `X2BuffFactor.h`'s inline `GetRate`/`SetRate` "Rate is Empty!". The next statement's `.at( 0 )` throws on an empty vector anyway, and the file is a PCH header.
  - `X2BuffTempletManager.cpp`'s other sites run inside the Phase 3 captures, where the redirect hands the text over exactly as before; `GetBuffFactorPtr` already logs once per id.

**2. `BFT_TIME` (`X2BuffFinalizerTemplet.h/.cpp`, `X2BuffTemplet.h/.cpp`, `X2GameUnit.h`).**
- **`GetFactor`** appends `m_fFullDurationTime` as a third value (`32ec5290`); `[0]`/`[1]` keep their place.
- **`SetFactorFromPacket`** sets it from `v[2]`, or **0 when absent** (`32ec53d0`). That closes the Phase 4a review's R1.
  - Decision: June's fallback, because "June wins". Nothing builds a 2-value `BFT_TIME`: the offline emulator builds no buff factors, and the only other `GetFactor` reader (Aisha's Death Sentence, `X2GUArme_VioletMage.cpp:15638`) reads `[0]`. So every packet now carries `[2]`.
- **The debuff multiplier** (`32ec27d0` @`32ec33bd`) is applied at June's point: after `GetReduceDebuffDurationRate`, only for a debuff that is not a world buff, `× (1 − r)` when `r > 0`.
  - `r` is `CX2GameUnit::GetDebuffDurationDecreaseRate()`, which is 0.0. June's only feed is a socket-option field March lacks.
- **FROZEN_TIME** is parsed and stored (`m_bFrozenTime`). `SetFactor` logs it once per buff and does not freeze.
  - `OnFrameMove` has June's gate on `m_bTimeFrozen`, which nothing sets: dormant until someone ports the unfreeze.
  - Reach: Rose's `HEAVYFIREARMS_MASTERY` only.
- **SHAKE_DECREASE_*** has its data side:
  - three members;
  - finalizer slot 11 `DecreaseDurationByShake` (base false, Time `32ec3610`);
  - `CX2BuffTemplet::DecreaseDurationByShake` (`3236c990`), with no caller yet.
  - A Time finalizer with a max > 0 logs once per buff that the trigger is Phase 8's. Reach: the water balloon only.

**3. Displayers (`X2BuffDisplayerTemplet.h/.cpp`).**
- **Sound:** `ParsingDisplayer` (`32eb7460`) reads `ONLY_MY_UNIT`. `StartDisplayer` (`32eb9940`) returns without a unit; with `ONLY_MY_UNIT` it plays only on a user unit that is `g_pX2Game`'s my unit (or when there is no game or my unit). Then `PlaySound( name, true, false )`, which is March's default arguments. Both are June's whole bodies, with March's in `#else`.
- **EffectSet:**
  - `ParsingDisplayer` reads `ONLY_MY_UNIT` and `NOT_DELETE_EFFECT_SET_WHEN_FINISH` after `REPLAY_EFFECT_SET_ACCUMULATION_LIMIT1` (`32eb82b0`).
  - `StartDisplayer` runs June's `ONLY_MY_UNIT` gate first: my unit only, never an NPC. The rest of June's body is March's (`BALANCE_PATCH_20131107` branch). Its play flag is `GetRemainHyperModeTime() > 0`, the same thing (June `31e4f890` = gage `+0xe8`).
  - `DoFinish` leaves the sets playing when `NOT_DELETE` is set (`32eba380`).
  - **Not ported from June's `StartDisplayer`:**
    - an equipped item's effect set replacing the awakening buffs' (BTI 157, 320, 660, 661, 822, 823, 966–968; `32076c90`/`3264ec50`/`325d2370`; these BTIs skip the next step);
    - a per-instance flag taken from the buff's creator (`CX2Game` slot 50, `322d4d10`, instance `+0x1bc`);
    - June's wider `PlayEffectSet` (the effect port's).
- **OnlyMyClass:** `ParsingDisplayer` is June's `32eb8c30`, twelve keys in `UNIT_TYPE` order: ELSWORD, AISHA, RENA, RAVEN, EVE, CHUNG, ARA, ELESIS, ADD, LUCIEL, ROSE, AIN. A key that is absent stores "", and the parse holds when all twelve are stored.
  - March read nine keys, and its ninth was "NEW_CHARACTER" (`SERV_9TH_NEW_CHARACTER`), so it never read ADD.
  - `StartDisplayer` drops March's invalid-handle popup (`32ebab00` has no check).
  - Reach on March data: the one templet, `BTI_BUFF_HAMEL_SECRET_DUNGEON_WEAPON`, uses only ELSWORD … ELESIS (bytecode constants of the staged `BattleFieldBuffTemplet.lua`, 152,564 B). It parses the same entries; Add and later classes stay "".
- **CreateGap:** `StartDisplayer` creates the sets and their gap timers only while the timer list is empty (`32eba690`). March added another set of handles and timers on every start.
- **New displayer slot 5** (`32eb9310` EffectSet, `32eb9ab0` CreateGap, `32eb9ba0` OnlyMyClass) pushes a unit flag (`31fae370`) into June's effect-set instances (`321de4f0`). It is June-only effect-system work: not ported.

**4. Finalizer verdicts (all 14 BFT; the factor side of every one is "same", see the RE).**

| BFT | Class slots (June) | Verdict |
|---|---|---|
| 1 TIME | see the RE table | **ported** (item 2) |
| 2 HIT_COUNT | `32ec0620`/`32ec1a90`/`32ec0660`/`32ec4240`/`32ec06b0`, GetNewCount `32ec0760`, parse `32ec2300` | same, except June's optional `ATTACK_TYPE` set: the parse's table reads into `+0x28`, and the count then comes from per-attack-type unit counters (`32274800`, unit `+0x4232..`). No June data uses it. **Logged at parse** ("BFT_HIT_COUNT ATTACK_TYPE ... every attack type is counted"), not ported; it would need Phase 8-style counters |
| 3 HITTED_COUNT | `32ebf1c0`/`32ec1b50`/`32ebf1f0`/`32ec4300`/`32ec0920`, GetNewCount `32ec09c0` | same (popups only) |
| 4 STATE_CHANGE | `32ebf220`/`32ebf290`/`32ec43c0`/`32ec0b70` | same, except a user also stores unit `+0x89d8` and finishes when it changes. `+0x89d8` is written by `31fe0bb0` (six user callers; it plays an effect set) and compared against class-specific constants elsewhere: a June class mode with no March counterpart, which never changes on March classes. **Not ported**; character-port item. Wire layout unchanged (`[state]`) |
| 5 DAMAGE_VALUE | `32ec1bf0`/`32ec1c50`/`32ebf630`/`32ec4470`/`32ec1d60` | same (max-pick and finish test traced in the x87, NaN included) |
| 6 PASS_HP / 7 PASS_MP | `32ec21b0`/`32ebf8f0`/`32ec4530`; Mp the same shape | same |
| 8 IMMEDIATE | `32ebfc70`/`32ebe7b0`/`32ec4690` | same |
| 9 DUNGEON_STAGE_CHANGE | `32ec5600`/`32ec5710` | same for every March game type; June also accepts its game type 4 (a `CX2PVEGame`, June-only) |
| 10 LIVE_CREATOR | `32ec1010`/`32ec4df0`/`32ec5840`/`32ec47d0`/`32ec4f60` | same; slot 10 `GetCreatorUID` (`32ec60c0`) has no caller (Phase 1c item 4), not ported |
| 11 FINISH_HYPER | `32ec1140`/`32ebfca0`/`32ebfd10`/`32ec4920`/`32ec1100` | same |
| 12 RIDING_ON_OR_NOT | `32ebfd30`/`32ebe7c0`/`32ec49d0` | same; `FINISH_WHEN_RIDING_ON` unread by June too (census exclusion) |
| 13 X2STATE_CHANGE | `32ebff20`/`32ebff70`/`32ec4a60`/`32ec1320` | same |

Time's slots 14/15 (set/get duration) have no caller outside the vtable and are not ported.

**5. Displayer verdicts (BDT 1–13).**

| BDT | Verdict |
|---|---|
| 1 RENDER_PARAM, 2 RENDER_PARAM_BY_UNIT_TYPE | same (start inserts by the templet's identity when not started, finish erases) |
| 3 EFFECT_SET | **ported** (item 3), minus the June-only parts listed there |
| 4 UNIT_SLASH_TRACE | same |
| 5/6/12 IMPACT_POINT / HITTED / NORMAL_ATTACK | same at the slot level (start when not started, finish by type 5/6/12) |
| 7 WEAPON_PARTICLE | same at the slot level (start by BTI, particle destroy, frame-move type 1/2) |
| 8 AFTER_IMAGE, 10 WEAPON_AFTER_IMAGE | same |
| 9 SOUND | **ported** (item 3) |
| 11 CREATE_GAP | **ported** (item 3, the first-start guard); its OnFrameMove is March's (`GS_PLAY`, per-timer replay) |
| 13 ONLY_MY_CLASS | **ported** (item 3) |

**Not diffed byte for byte (for the review):**
- the displayer parses other than Sound, EffectSet, OnlyMyClass and CreateGap (their keys are census-clean, but defaults and required-ness were not traced);
- ImpactPoint's `32eb78f0`, WeaponParticle's `32eb9400`/`32eb9600`/`32eb9770`, DamageValue's `32ec0c40`/`32ec0ca0`;
- PassMp (taken as PassHp's mirror);
- the Hit/Hitted/DamageValue/PassHp/PassMp class parses beyond their key lists.

**Behavior changes on March data.**
- **No buff popup at runtime.** Each failure is one `[JUNE_BUFF] runtime error, BTI n: ...` line per buff and text.
- **`BFT_TIME` packets carry three values.** A stage-change restore (`SetKBuffFactorFromGageManagerToGameUnit`) or a peer copy now restarts an accumulation decrease at the full duration. No March templet has `ACCUMULATION_DECREASE_WHEN_FINISH`, so there is no reach until Phase 13.
- **CreateGap** no longer stacks another set of effect handles and gap timers each time the displayer restarts.
- **OnlyMyClass, Sound, EffectSet:** no reach until June data. March data has none of the new keys, and the OnlyMyClass templet parses the same entries.

**Census.** `buff_census.py --phase 4b`: **0 type/key pairs** (was 11). `FINISH_WHEN_RIDING_ON` is in `NOT_READ_BY_JUNE_TYPE` for both origins.

**funcmap.tsv:** 39 rows ("Buff Phase 4b-ii").

**Build and deploy.**
- `X2Lib` (with `stdafx.cpp` touched) and `X2` at `US_SERVICE`. Two earlier passes failed: the narrow `DISPLAY_ERROR( const char* )` overload was missing (C2664, fixed by adding it), and then the first pass, still generating code, held the PDB (C1033). The final passes returned rc 0 with 0 errors, and no warning is in a touched file. `KTDXLIB` and `X2ServerProtocol` are unchanged (no `Always.h` edit).
- `x2.exe` is 19,033,088 B, linked 09:29:05, md5 `529ff0c9fe279e8ee3138eea1c817943`. It contains `runtime error, BTI %d: %s`, `SHAKE_DECREASE_TIME_AT_ONCE`, `FROZEN_TIME`, `NOT_DELETE_EFFECT_SET_WHEN_FINISH`, `ONLY_MY_UNIT` and `LUCIEL`, plus the wide `BFT_HIT_COUNT ATTACK_TYPE` and `ACCUMULATION_MULTIPLIER is 0` texts.
- Copied to the game directory as `X2_offline.exe` (the names, read programmatically, are `x2.exe` and `X2_offline.exe`); the size and md5 match. **Not launched.**
- No Lua changed, so no pack is needed.
- **After the two fixes (`256c26d`):** `X2Lib` rebuilt (only `X2BuffTempletManager.cpp` recompiled) and `X2` relinked, rc 0, 0 errors. `x2.exe` is 19,033,088 B, linked 09:34:14, md5 **`806e22235c591139b6963ae22bfe2fb5`**. It was redeployed as `X2_offline.exe` (names read programmatically) and the md5 matches. **This is the deployed exe. Not launched.**
- **Flag dependencies (add to `BEHAVIOR_PARITY`'s `Always.h` note the next time that header is touched):** it now also needs `SERV_IRUHADEV_JUNE_BUFF_LOADER` (`CX2JuneBuffCapture`) and `SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY` (`m_fFullDurationTime`).

**Things to watch in play:**
- `ErrorLog.txt`: any `[JUNE_BUFF] runtime error` line names a buff whose code used to raise a popup or failed silently. Each should appear once.
- The Hamel secret dungeon weapon buff (OnlyMyClass).
- Any create-gap effect buff re-applied while it runs.

**Play-test (2026-10-04, user): passed** on the deployed exe `806e2223…` (09:39–09:46). The user ran the planned checks: the Hamel secret-dungeon weapon buff (BFI 282), the Blazing Bomb elixir (187) re-applied while running, the Weapon Break debuff's sound (42), a timed buff carried across a stage change, and a full dungeon clear. All worked. Logs, read after the session:
- `ErrorLog.txt`: the `[JUNE_BUFF]` lines are exactly the Phase 4a baseline (duplicate BTI 87, both `_INT` not-found lines, ordinal rejects 481/482/483/372/373, `templets 320 loaded, 0 failed, 1 duplicate; factors 361 loaded … ordinal rejects 5`). There are **no** `runtime error` lines. The session's other entries (local-map control lookups, `ErrorEnum(72)`) predate this phase.
- `State.log`: 443 lines for the whole session, so no per-frame flood.
- The offline logs: a clean shutdown. The only `UNHANDLED` packet is `E_DISABLE_HB_CHECK_REQ` (×2), which is unrelated to buffs.

**Where 4b-ii stopped / hand-offs.** The class slots of every BFT and BDT are diffed and the census gate is met. Phase 4b is **not** complete: item 7 below (the unit-side stores and read sites behind the displayer and counter slots) was not diffed. None of these items blocks Phase 5.
1. **Added Phase 8 scope.** The shake trigger: `CX2Game +0xbae` set from input actions `0x25`/`0x26` in the `322f7470` block, and `OnFrameMoveBuff` calling `CX2BuffTemplet::DecreaseDurationByShake` for my user unit, bumping `+0x6dd0`. It sits with BFT 15's input block and the sync counter. Phase 8 must first map June's action codes `0x25`/`0x26` (June's `GAME_ACTION` has more skill slots than March's; LEFT/RIGHT is the likely pair) and what `31c488c0`'s byte array is.
2. **Character port** (`JUNE2017_CHARACTER_PORT_CHECKLIST.md`): FROZEN_TIME's record/unfreeze (`31fe2a80`, `3200bde0`, `3227beb0`; `+0x81fc` and vslot 329 to map) with Rose; StateChange's `+0x89d8` (`31fe0bb0`).
3. **Effect port:** EffectSet's awakening item override, the creator visibility flag, displayer slot 5.
4. **Socket port:** the debuff-duration decrease (`m_SocketData +0x2dc` → additional-stat member `+0x7388`).
5. **HIT_COUNT `ATTACK_TYPE`:** per-attack-type hit counters, if June data ever uses it (none does).
7. **Unit-side stores and read sites not diffed (4b scope, resume here).** The displayer slots match March, but the June unit functions they call were not compared with March's: render param insert/erase `3227ac80`/`32279b20` and by-unit-type unit vslot 154; after-image `3227b220`/`3227a2f0`; weapon after-image `3227b300`/`3227a3a0`; impact point / hitted / normal attack erase `3227a180`/`3227a220`/`3227a4c0` (and their inserts from `32eb78f0`); the slash-trace field `+0x4238`; and where each store is read. The same goes for the finalizer counters: hit `+0x4231`, hitted `+0x4230` and their increment sites (the March hit/hitted sites are listed in the ledger's runtime map).
6. **Review of 4b-ii:** required (Opus 5.5). Suggested spot-checks:
   - `32ec27d0` @`32ec33bd` (the `+0x64`/`+0x66` test order and the `> 0` gate);
   - `32ec3610`'s two x87 compares;
   - `32eb9c70`'s gate (`+0x264` 0 and 1 only);
   - (`32eb8c30`'s keys are settled: AISHA and RENA, the two `xorstrings.tsv` lacked, were decrypted from the function's own init constants, and ELSWORD/EVE decode the same way as a control);
   - the "not diffed byte for byte" list above;
   - whether any March runtime path reaches a buff `DISPLAY_ERROR` outside the five redirected files.

**Kickoff for the 4b-ii review** (Opus 5.5 high). It runs on its own; it is read-only unless it finds a code defect, which it then fixes, rebuilds and deploys.

```
Continuing the June buff/debuff port. Run this session on Opus 5.5 at high effort. Read
JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", the "June buff function
map", "Phase 4b-ii -- RE" and "Phase 4b-ii -- Done". Branch: mods/june-buff-port. Never
launch X2_offline.exe.

Your task: an independent review of Phase 4b-ii. Code: 159fb91 and its fix 256c26d (under
SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY); the RE commit is 2faeaf7; the doc commits are
fefc838, 54117d0, 8ae7d73 and 149bbec (play-test passed). Use fresh scratch tools; do not
reuse the session's patch or unifdef scripts. Run the flag-off unifdef on the committed
blobs of 256c26d against 146ce5a, and take numstat from git diff 146ce5a 256c26d. The
deployed X2_offline.exe should be md5 806e2223...; confirm that.

Check, in Ghidra (read-only) and the raw asm, the section's suggested spot-checks and its
"not diffed byte for byte" list; whether every buff runtime DISPLAY_ERROR on a March path now
logs instead of popping up (and nothing outside the five redirected files was missed); and the
March-data reach claims (OnlyMyClass on the staged BattleFieldBuffTemplet.lua, CreateGap,
BFT_TIME's third value through the gage-manager restore).

Report numbered findings (R1, R2...) with a severity. Fix a code defect under the same flag
(insert-only, byte-level CP949 edits, flag-off unifdef per file), rebuild X2Lib and X2 at
US_SERVICE (touch stdafx.cpp after a header edit), deploy as X2_offline.exe, record the md5,
and ask before launching. Add confirmed funcmap.tsv rows. Another session may have
uncommitted edits in the doc: stage only your own hunks. Commit. Record the review cost line.
```

### Phase 4b-ii — Review (2026-10-04, Opus 5.5 high)

Independent review of code commits `159fb91` and `256c26d` (under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`), the RE commit `2faeaf7`, and doc commits `fefc838`, `54117d0`, `8ae7d73` and `149bbec`. Read-only on code: nothing was edited, built or deployed.

Fresh tools only; the session's patch and unifdef scripts were not reused:
- Ghidra, read-only (decompiles only; no renames or comments).
- A new capstone reader over a hand-parsed PE section map. Vtables were named from their RTTI complete-object locators.
- A new byte-level unifdef that resolves only the one flag on `git cat-file` blobs, with a flag-on control and a wrong-flag control.
- A fresh `elstools extract -v mar-2014` of the live `data036.kom`, then `decompile-batch` of `BattleFieldBuffTemplet.lua` and `BuffTemplet.lua`.

**Verdict: no code defect.** Every suggested spot-check passes in the asm, and so does every item on the "not diffed byte for byte" list. R1-R6 are Low or Info: doc wording, one June-only system the WeaponParticle verdict left out, scope notes and an address.

| # | Check | Result |
|---|---|---|
| 1 | `32ec27d0` @`32ec33bb` | **Pass.** `cmp byte [ebp+0x64],1 / jne`, then `cmp byte [ebp+0x66],0 / jne`, so debuff first, then not-world-buff. `ebp` is the templet: its `+0x34` (BTI) is what vslot 167 (`+0x29c`, `GetReduceDebuffDurationRate`) is called with just before. `3236a9b0` stores `IS_WORLD_BUFF` at `+0x66` and `IS_DEBUF` at `+0x64` (`xorstrings.tsv` rows `3236ab81` / `3236ac79`). Gate: `fldz; fcomp r; test ah,5; jp skip`, so it applies only when `0 < r`; 0, a negative value and NaN skip. Then `duration × (1 − r)` (`fld r; fld1; fsubrp`). The equal-value test after it (`fucompp / test ah,0x44 / jnp`) only skips the protected-value store when nothing changed. The port's `0.0f < r` and `*= 1.0f - r` behave the same. |
| 2 | `32ec3610`'s two x87 compares | **Pass.** First: `fld done; fld max; fcompp; test ah,0x41; jp` goes on only when max > done or NaN; otherwise `xor al,al`. Second: `d = atOnce; fcompp max vs (d + done); test ah,0x41; jp` keeps `d` when max > d + done or NaN, and otherwise `d = max − done` (equal included). Then `done += d` (`+4`) and the protected duration (`+0x1c`) `−= d`. **Both** paths end in `mov al,1`. The port's `<=` tests give the same results, NaN included. |
| 3 | `32eb9c70`'s gate | **Pass.** No game: return (`32eb9d19`). `ONLY_MY_UNIT` (`+0x31`) set and a unit: `[edi+0x264]` is a **field** (the unit type, not a vtable slot). 1 (NPC) returns; 0 (user) compares `CX2Game` vslot 38 (`+0x98`, my unit) by its 64-bit UID (unit vslot 21, `+0x54`; `eax:edx`) and returns on a mismatch. No my unit, or any type ≥ 2, plays. March `GUT_USER` = 0, `GUT_NPC` = 1. That is the port's gate. |
| 4 | Sound `32eb9940` | **Pass.** No unit: return. With `+0x2c` set: a type ≠ 0 returns; an invalid game or no my unit plays; a UID mismatch returns. Then `32276220( name, 1, 0 )`. That is the port. |
| 5 | The displayer parses not diffed | **Pass.** Read from the decompiles, with every `ret` path's `al` checked in the asm (Ghidra types them `void`). UnitSlashTrace `32eb7250`: `SLASH_TRACE_CONDITION`, 0 when missing (`STC_NONE` = 0), always true. AfterImage `32eb7b90` / WeaponAfterImage `32eb7c70`: `COLOR`, (1,1,1,1) when missing, true. RenderParam `32eb7d50`: `RENDER_TYPE`, `CARTOON_TEX_TYPE`, `OUT_LINE_WIDE`, `COLOR_OUT_LINE`, `COLOR`, `ALPHA_BLEND`, `FLICKER_COLOR_GAP`, each optional, true. March reads the last one under `SERV_ADD_LUNATIC_PSYKER`, which `ServerDefine.h:4368` defines unconditionally. RenderParamByUnitType `32ebad40`: `BeginTable(i)` loop; `UNIT_TYPE` (0 when missing) plus the same six keys; a duplicate insert leaves with `xor al,al`; the result is `find( 0 ) != end` (`setne al`). Those are both of March's failure rules. ImpactPoint `32eb7410`: index 1 required (`xor al,al` otherwise); 2 (sound) and 3 (gap, float getter) optional; true. WeaponParticle `32eb8a40`: March's index loop with a handle of −1 each, false when it read nothing. |
| 6 | The finalizer parses not diffed | **Pass.** Hitted `32ebf000`: `BUFF_USE_COUNT_TYPE` required. DamageValue `32ebf330`: `BUFF_CHANGE_TYPE`, then `BUFF_RELATION_TYPE` (7 on failure), both required. PassHp `32ebf6a0` / PassMp `32ebf970`: `PASS_UP` (`+0x34`), then `BUFF_CHANGE_TYPE` (`+0x2c`), both required. Every failure path returns the getter's `al` (0) with no reassignment. That matches March's `LUA_GET_VALUE_RETURN(_ENUM)` required-ness, without the popup. |
| 7 | ImpactPoint `32eb78f0` / finish `32eb7890` | **Pass.** `32eb78f0` is reached by a tail-jump from slot 1 `32eb7a80` (`!started`), which is why Ghidra carves no function there. It switches on the displayer type: 5 → `3227b0a0`, 6 → `3227b160`, 12 → `3227b540`, each with ( identity `+0x34`, set name, sound or `""` when empty, gap `+0x48` ). Finish `32eb7890` (started only): 5/6/12 → `3227a180` / `3227a220` / `3227a4c0` with the identity. That is March's `StartDisplayerByType` / `DoFinishByType` (`X2BuffDisplayerTemplet.cpp:536-592`). The unit-side inserts stay item 7's. |
| 8 | WeaponParticle `32eb9400` / `32eb9600` / `32eb9770` | **Frame moves pass; the start differs (R2).** `32eb9600`: three handles (`0xc` bytes); `TRACE_START0`, `TRACE_END0`, then mid = (start − end) × 0.5 + end. `32eb9770`: eight (`0x20`); start, start, end, end, then mid ×4. Both are March's SwordFire / SwordEnchant. `32eb9400`: BTI 3 → type 1; 163-165 → type 2; anything else → type 0 with no error (March raises "Empty WeaponDisplayer OnFrameMove", which the redirect now logs). Then, beyond March, R2. Then slot 4 `( unit, 0.0 )`. March `BTI_SI_SA_EMK_SWORD_FIRE` = 3; `SWORD_ENCHANT_ICE..POISON` = 163..165. |
| 9 | DamageValue `32ec0c40` / `32ec0ca0` | **Pass.** `32ec0c40` is `GetValue`: `v[1]` (0 with fewer than 2); type 1 returns it; type 2 returns `v[3] × v[1]` (0 with fewer than 4); else 0. That is March's `FO_VALUE` = 1, `FO_MULTIPLIER` = 3 and `BCT_FIX` = 1, `BCT_RELATION` = 2, with `KBuffFinalizerFactor::operator[]`'s range check. `32ec0ca0` is `ModifyDamageValue`: now HP is 1.0 with no gage and 0.0 with no HP block; `now <= old` subtracts `old − now`, else it adds `now − old`. That is March's branches, which agree at equality. |
| 10 | PassMp as PassHp's mirror | **Pass.** All 14 slots compared instruction by instruction with addresses normalised. Slots 2-5, 7 and 13 are identical except two call targets: SelectDelegate `32ec1fa0` / `32ec1dc0` and the copy ctor. Slot 12 differs only in its XOR-string constants. The Mp delegates: `CheckMpByUp` `32ec5b00` = `GetNowMp (unit vslot 54) > c`; `CheckMpByDown` `32ec5b40` = `< c` (NaN false in both); by-percent `32ec5930` = `c × GetMaxMp` (vslot 55); by-value `32ec5920` (shared with Hp) = `c`. SelectDelegate: `PASS_UP` chooses up/down; type 1 → by value, 3 → by percent, else by value. All as March (`X2BuffFinalizerTemplet.cpp:1234-1300`). |
| 11 | Runtime `DISPLAY_ERROR` outside the redirected files | **Pass (R4).** A sweep of every `X2Lib` `.cpp`. Then a callee pass: 143 distinct unit/game/effect methods the five files call, each body grepped. The only hits are the three `Insert`/`Update`/`EraseAdditionalStat` sites, which are already the `#else` of 4b-ii's conversion. `X2GameUnit.cpp:11341` is `TEMPLET_PARITY`'s `#else`. Buff headers: only `X2BuffFactor.h`'s two "Rate is Empty!" (already listed). `CX2JuneBuffCapture::UnsupportedType` calls `DisplayErrorMessage` directly, but all six of its call sites are `ParsingCombination*` parsers, which only run inside a capture (`ParsingBuffFactor` opens its own, `X2BuffFactor.cpp:514`). |
| 12 | "Every runtime line names its buff" | **Pass (R5).** Of `CX2BuffTemplet`'s methods that reach a behavior, finalizer or displayer, the unscoped ones are `GetFactor`, `GetRemainDurationTime`, `ReserveToFinish`, `IsNonControlDebuff` and `DecreaseDurationByShake`. None of their callees has a `DISPLAY_ERROR`. 4a's `ReapplyBehaviorForAccumulationDecrease` / `RestartDisplayerForAccumulationDecrease` are called only from `OnFrameMoveFinalizerAndCheckFinish` (`X2BuffTemplet.cpp:432-435`), which is scoped. Unit-side `OperateBuffBehaviorValue` (slots 9-11) and the finalizer-list getters (Aisha, Eve) reach nothing that errors. |
| 13 | OnlyMyClass reach | **Holds.** The staged `BattleFieldBuffTemplet.lua` equals the live entry (152,564 B, md5 `730531f5…`, and the same for `BattleFieldBuffFactor.lua`). It has two `ONLY_MY_CLASS` lines, i.e. one templet (`BTI_BUFF_HAMEL_SECRET_DUNGEON_WEAPON`), with ELSWORD … ELESIS. `NEW_CHARACTER`, `ADD`, `LUCIEL`, `ONLY_MY_UNIT`, `NOT_DELETE_EFFECT_SET_WHEN_FINISH`, `FROZEN_TIME`, `SHAKE_DECREASE_*`, `ACCUMULATION_DECREASE_WHEN_FINISH` and `ATTACK_TYPE` appear in neither it nor `BuffTemplet.lua`. March `UT_ADD` = 9, so `"ADD"` is index 8, where `StartDisplayer` (`:1188`, type − 1, bounds-checked) looks. |
| 14 | CreateGap reach | **Holds (R3).** Five March templets: the four elixirs (Blazing Bomb, Cradle of Little Fairy, Ice Ball of Deniph, Flame Ring of Rosso) and `BTI_BUFF_ALCHEMYST_BOSS_SHIELD`. Blazing Bomb has `ACCUMULATION_LIMIT = 1` and no `BUFF_REPLACE_DETAIL_OPERATION`, so it takes the parse default `BRDO_ADD_TIME` (`X2BuffTemplet.cpp:46-47`). A re-apply therefore runs `SetFactor` on the buff already on, and March's `SetFactor` starts every displayer unguarded (`:233-240`). An elixir used from the quick slot (`X2UIQuickSlot.cpp:1013`) goes through `CX2Game::ApplyBuffFactorToGUUser` (`X2Game.cpp:18153`) → `SetBuffFactorToGameUnit`, not the packet path. So the play-test's re-apply exercised exactly the changed path. |
| 15 | `BFT_TIME`'s third value through the gage restore | **Holds.** `CX2GageManager::GetBuffFactorFromGameUnit` (`X2GageManager.cpp:3539`) → the unit's `GetBuffFactor` → each templet's `GetFactor` (three values). `SetKBuffFactorFromGageManagerToGameUnit` (`X2GameUnit.cpp:14763`; not in PvP) → `SetKBuffFactorPacketToGameUnit` → a fresh clone's `SetFactorFromPacket`, which takes `v[2]`. `m_fFullDurationTime`'s only reader is `RestartForAccumulationDecrease` (`X2BuffFinalizerTemplet.h:131`). June `32ec53d0`: `cmp ecx,2 / jbe` → `+0x28 = v[2]`, else 0. `32ec5290` pushes the third value from `+0x28`. |
| 16 | Hygiene and deploy | **Pass (R1).** See R1 for the unifdef and numstat. All 423 added lines are ASCII and CRLF, and `file` signatures are unchanged on all 12. `X2/US_SERVICE/x2.exe` and the game directory's `X2_offline.exe` are both 19,033,088 B, md5 **`806e22235c591139b6963ae22bfe2fb5`**. |

**Findings**
- **R1, Info (hygiene wording).** "A fresh byte-level unifdef … reduces all 12 files byte-for-byte to `146ce5a`" is literally true for 6 of them: the two displayer files, the two finalizer files and the two manager files. The other 6 already carry this flag in `146ce5a` (parts 1-5). The property that holds for all 12 is strip(`256c26d`) == strip(`146ce5a`). The flag-on control and the wrong-flag control both fail on every file, as they should. `numstat 146ce5a 256c26d`: code **423/31**. The doc's 411/31 is `159fb91` alone; `256c26d` adds 14/2 in `X2BuffTempletManager.cpp`, and its 2 deletions are `159fb91`'s own lines.
- **R2, Low (doc verdict; hand-off 3).** BDT 7 WEAPON_PARTICLE is not "same at the slot level". After creating the handles, June's start `32eb9400` passes each one, for a non-null unit, through unit `32296da0( handle, false )`. That sets the particle instance's `+0x1b0` to `CX2Game` `322d4d10( unit )` (major system for false, minor for true). It is the same June-only per-instance visibility flag as EffectSet's `+0x1bc`, here computed from the buff's **target** unit (the displayer's unit argument). It is not ported and has no March counterpart. It belongs with hand-off 3 (effect port) beside the EffectSet creator flag. No code change.
- **R3, Info (CreateGap: parity on the restart path too).** June's CreateGap `DoFinish` `32eba8b0` stops the `+0x10` handles and leaves the `+0x30` timer vector, as March's does. 4a's `RestartDisplayerForAccumulationDecrease` calls `StartDisplayer` with no `DoFinish` first. So `32eba690`'s empty-timer gate behaves as June on a re-apply, on an accumulation restart and after a finish. A restarted set is never replayed at start, and its gap timers keep running.
- **R4, Info (popups outside the buff code; the user's call).** `X2GUNPC.cpp:30976/31038` and `X2GUUser.cpp:36463/36501` raise "BuffFactor Duplication Error!" from `ParsingBuffFactorID`. That is `_IN_HOUSE_`-only validation of a unit state script's `BUFF_FACTOR_ID` list, run when the script is parsed, not buff runtime. `_IN_HOUSE_` is on under `US_SERVICE`, so it would still be a MessageBox. No March data has raised it in any play-test. `X2GUNPC.h:2915`'s inline `SetCriticalRate` popup has no buff caller. Both are left as they are; say if the sweep should cover them.
- **R5, Info (Phase 8 note).** `CX2BuffTemplet::DecreaseDurationByShake` opens no `CX2JuneBuffRuntimeScope`. That is harmless today: it has no caller, and no slot 11 raises anything. If Phase 8 calls it from `OnFrameMoveBuff` and slot 11 ever logs, add a scope there or the line will print `BTI 0`.
- **R6, Info (address).** The spot-check address `@32ec33bd` falls inside an instruction. The debuff test is `cmp byte [ebp+0x64],1` at `32ec33bb` (to `33be`), with its `jne` at `33bf`. The 4b-ii doc and two code comments (`X2BuffFinalizerTemplet.cpp`'s debuff-multiplier block and `X2GameUnit.h`'s `GetDebuffDurationDecreaseRate`) all say `33bd`. The comments are left as they are, because changing them would cost a rebuild for no behavior change.

**funcmap.tsv:** 39 rows ("Buff Phase 4b-ii review"), all new:
- CreateGap `DoFinish`.
- WeaponParticle's start and two frame moves, plus unit `32296da0`.
- ImpactPoint's start-by-type and `DoFinish`.
- DamageValue `GetValue` / `ModifyDamageValue`.
- PassHp/PassMp: both `SelectDelegate`s, the seven delegates (Hp up/down/by-percent, Mp up/down/by-percent, and the shared by-value), and each class's five slots (`SetFactor`, `GetClonePtr`, `DidFinish`, `GetFactor`, `SetFactorFromPacket`).
- The seven displayer parses and four finalizer parses diffed above.

**Build and deploy:** none, because there is no code defect. The deployed exe is still `806e2223…` (`256c26d`).

### Phase 4b-iii — Done (2026-10-04), play-test pending

Ran on Opus 5.5 high, under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`. The pre-session commit is `6312f1d`; the code is `2cb90fa`.

**Method.**
- Ghidra was used read-only, for decompiles only. Uncarved bodies were read with `buffre/rawfn.py` and `disasm_range.py`.
- Every store's readers come from a `dispxref.py` sweep of each member's displacement:
  - render param `+0x669c/+0x66a0`;
  - after-image `+0x6744/+0x6748`;
  - weapon after-image `+0x675c/+0x6760`;
  - impact point, hitted and normal attack `+0x66fc`, `+0x670c`, `+0x677c` (and each `+4`);
  - slash trace `+0x4238`;
  - hitted `+0x4230`, hit `+0x4231`.
- Each hit was mapped to its function through `funcs.txt`. Every function on the list is in the verdict table, except the ctor/dtor clears (`322a2700`, `322a0730`) and the finalizer readers already diffed in 4b-ii.
- Unit vslots were resolved per class with `vtslot.py`. March's reach was measured on a fresh `elstools decompile-batch` of the staged `BattleFieldBuffTemplet.lua` (152,564 B, the live size) and `BuffTemplet.lua`.

**Verdicts.**

| Item | June | Verdict |
|---|---|---|
| Render param insert / erase | `3227ac80` / `32279b20` | same shape (copy into `+0x669c`, find-by-identity erase). The struct they copy differs: see the next three rows |
| `StBuffRenderParam` | copy ctor `322ad270` | **ported.** June holds each of the six keys in a one-element slot that stays empty when the script lacks the key. March gets `m_bHas*` flags (struct, copy ctor, `Initializer`). The value members keep March's defaults |
| RenderParam / ByUnitType parses | `32eb7d50` / `32ebad40` | **ported.** Each key is stored only when present. `FLICKER_COLOR_GAP` is a plain float, read only by RenderParam and 0 on a miss, as in March. This corrects 4b-ii's BDT 1/2 "same", which compared required-ness, not storage |
| `ApplyRenderParam` | `32275140` | **ported.** After the `+0x6840` (`m_bPassDash`) return, vslot 197 (`ApplyRenderParamOnEmptyRenderParamByBuff`) always runs first. With an empty list, vslot 199 (`ApplyWorldLightColor`) follows, as in March. Otherwise the newest buff render param overrides only its set keys. Colour flickers when the gap is > 0.0 (`334350e8`) |
| vslot 197 base + overrides | `3226e190`; Ara `32ba6e50`, Aisha `32cc1c50`, Chung `32bec310`, Eve `32978420`, Raven `32c3deb0` | same. The override set is exactly March's five classes and the bodies match (constants read: 0.8/0.6/0.2, 0.694/0.737). They now run every frame, also while a buff render param is active, as in June |
| vslot 154 (by unit type) | user `31fd8600`, NPC `3275bba0` | same (own type, else `UT_NONE`; NPC `UT_NONE`) |
| `GetFlickerColorByBuff` | `322722e0` | same |
| After-image insert | `3227b220` | same, apart from June-only parts: `EnableAfterImage(3)`, then `+0x7c80 = 1` |
| `EnableAfterImage` | `32275a30` | **ported (colour).** Enable needs option `== 0` (`OL_HIGH` = 0) and `+0x3e59 == 0` (absolute invisibility), as in March. Colour = the newest buff colour, **else the state colour `+0x6880`**; March left the colour as it was. The mode argument (0/1/3 → `+0x6754`/`+0x6758`/vslot 152) is June-only and always the first after-image on March |
| `DisableAfterImage` | `322720f0` | **ported.** Same condition (force, empty list, or option `== 2` = `OL_LOW`), but **no `SetAfterImageColor( 0xffcccccc )`** |
| After-image erase | `3227a2f0` | same as March's erase plus the ported `DisableAfterImage`. June-only parts: `+0x7c80 = 0`, and the after-images' `+0x1d1 = 0` |
| State colour `+0x6880` | ctor `@322a5e05`; `CommonStateStart` `32057b80` `@3205b70d` | **ported.** A new member, (0.8, 0.8, 0.8, 1) from the ctor. `CX2GUUser::CommonStateStart` reads the state's `AFTER_IMAGE_COLOR` (decoded from the function's own XOR bytes at `3379d708`; read through `31c9eba0`, June's user-type getter), with the same default when it is missing. Nothing else writes it, and no NPC path does |
| Weapon after-image insert / Enable / erase | `3227b300` / `32275b00` / `3227a3a0` | same. Pulse is (r,g,b,1)→(r,g,b,0.1), 0.5, 3600, `PWCI_UNDEFINED` (1). The erase's inline zeroing of `+0x416c..+0x4188` is March's `SetEnableWeaponRenderEffect(false)`, field for field |
| ImpactPoint / Hitted / NormalAttack insert and erase | `3227b0a0`/`3227b160`/`3227b540`, `3227a180`/`3227a220`/`3227a4c0` | same (ctor `322aba40`: name, sound, gap; cool time 0) |
| `PlayEffectSet*ByBuff` (unit) | `322752f0` / `32275910` / `32275cf0` | same |
| Element players | `3236ea50` / `3236ebd0` / `3236ed60` | same, except **NormalAttack's gate is `type != 1` (`AT_SPECIAL`)**; March has `== AT_NORMAL`. **Ported**: `AT_COMMAND` (2, Phase 2) now plays too |
| Cool-time tick | `323697b0`, from `3229ecc0 @3229f971..` | same (three lists, `cool < gap` → `+= dt`) |
| Slash trace `+0x4238` | readers `3227af80` → `3227a0a0`; writer the displayer `32eb7460` | same. March's reader is compiled in (`UNIT_SLASH_TRACE_MANAGER_TEST`, `Always.h:2857`). June's missing-type default 5 is `STT_NONE` |
| Hitted `+0x4230`, user / NPC | `320462a0 @320489c6` / `327b1f30 @327b4b82` | **ported.** June counts only when `DamageData +0x56c != 4` (`NFT_RESULT_BOMB`, March's `m_iNaturalForceType`, since `ADD_RENA_SYSTEM` is off) **and `+0x0 != 6` (`DT_BUFF`)**. March counted every hit |
| Hit `+0x4231`, user | `32012b50 @320141ae` | same for the March counter. The per-attack-type `+0x4232+type` is hand-off 5 |
| Hit, NPC | `3276a320 @3276ae07` | **ported:** the NPC's owner (`32759f00` = `GetOwnerGameUnit`, observer `+0x8d5c`) also gets `++hit`. Per-type counters: hand-off 5 |
| Hit, damage effect | `31f85910 @31f87a60` | same |
| Sync write / apply | `32061bf0 @32062f82` / `32063650 @3206c1ee` | same for hit, hitted and debuff. June also syncs the per-type counters (`+0x8104..`): hand-off 5 (wire layout). The apply's `!IsMyUnit` gate was not re-traced |

**June-only, handed off (not ported).**
- **Alternate after-image (character port).**
  - Unit vslot 152 (`3208e160`) returns user byte `+0x8e90`; base and NPC (`31e4c550`) return false.
  - `+0x8e90` is copied from `CX2Unit +0xe40` when the user unit is created (`32d51e80`), and set by `3204e030`.
  - It selects between a second after-image `+0x6758` (created at `@31feed3d`) and a second head frame (`31fcac20`, the `+0x4218`/`+0x4200` choice in funcmap row 714).
  - Its second `AFTER_IMAGE` table in `CommonStateStart` fills `+0x852c`.
  - What the flag means was not identified.
- **`+0x7c80` (character port).** "After-image on by buff": set by insert, cleared by erase and by `31ff27c0`. Its only reader is `CommonFrameMove @32065c06`, which re-enables the after-image when a June-only `+0x941c` show/hide block ends.
- **After-image `+0x1d1` (character / effect port).** A saved-enable byte on `CKTDGXSkinAfterImage`, used by a suspend/restore pair (`31fd8250` saves and disables, then restores; `322727c0` sets it on both after-images). Erase clears it.
- **Per-attack-type hit counters `+0x4232+type`.** Incremented next to `+0x4231` at the user and NPC hit sites (and for the NPC's owner), and synced at `+0x8104..+0x8106`. This is 4b-ii hand-off 5 (HIT_COUNT `ATTACK_TYPE`), now with its increment sites and wire fields located.

**Behavior changes on March data.**
- **Render params (reach: all 37 `BDT_RENDER_PARAM` blocks and the 9 by-unit-type entries).** Each omits at least one of the six keys (`COLOR` ×10, `CARTOON_TEX_TYPE` ×26, `OUT_LINE_WIDE`/`COLOR_OUT_LINE`/`ALPHA_BLEND` ×27; by-unit-type: `COLOR` ×9).
  - March replaced each missing key with its default: outline width 0, white outline and colour, `CTT_NORMAL`, no alpha blend.
  - June keeps the unit's own value.
  - Example: line 1645 of the decompiled templet sets only `RENDER_TYPE = RT_CARTOON_BLACK_EDGE` and a red `COLOR`. March drew it with a 0-width outline; it now keeps the unit's outline.
- **After-image colour.** With no buff after-image, an enabled after-image is painted with the state colour, which is 0.8 gray on all March data: no March bytecode has the exact key `AFTER_IMAGE_COLOR`. Before, it was painted gray only on disable.
  - **NPC custom colours (`AFTER_IMAGE_COLOR_R/G/B/A`, `ChangeAfterImageColor`): no practical change.** June's NPC window (`327b53b0 @327b6887`) calls `EnableAfterImage(3)`, which repaints the state colour. But March's `DisableAfterImage` already repainted gray on every frame outside the window, and every state without an `AFTER_IMAGE` table calls it at its start. So a custom colour survived in March only in a window reached before any disable.
  - **Ran (`Ran.lua`, `Ran_Event.lua`; checked, not a loss).** It calls `SetEnableAfterImageAlways(true)` and `ChangeAfterImageColor` in phases 2 and 3. Neither March's nor June's `Ran.lua` has any state `AFTER_IMAGE` table, so its after-image is never enabled except by a buff after-image, whose colour wins in both builds.
    - The always flag is dead in both: March never reads `m_bAfterImageAlways`. June's (`+0x8cd4`) is written by its setter `31e4d6f0` (bound at `31e0876c`), the ctor `@327627c0` and the NPC init `@327a2c5c`, and `dispxref` finds no reader.
    - June's `ChangeAfterImageColor` (`327582d0`) writes the after-image directly, as March's does.
  - June scripts that set `AFTER_IMAGE_COLOR`: `ELESIS_KNIGHT`, Ain, Luciel, Rose, `PVP_HERO_SHAY`/`TAV`. They reach March only when adopted.
- **Hitted count.** `DT_BUFF` damage (damage over time) no longer advances `BFT_HITTED_COUNT`.
- **Hit count.** A summoned NPC's hits now also advance its owner's `BFT_HIT_COUNT`.
- **Normal-attack effect sets** also play on `AT_COMMAND` hits. No March data uses `AT_COMMAND` yet.

**Hygiene.**
- Seven code files: `X2BuffDisplayerTemplet.h/.cpp`, `X2GameUnit.h/.cpp`, `X2GUUser.cpp`, `X2GUNPC.cpp`, `X2BuffTemplet.cpp`.
- `numstat 6312f1d 2cb90fa`: 216 insertions, 0 deletions. Every added line is ASCII and CRLF, and every `file` signature is unchanged.
- A fresh one-flag byte-level unifdef (flag off) gives strip(`2cb90fa`) == strip(`6312f1d`) on all seven. The flag-on control and a wrong-flag control both fail.
- The first pass left one blank line outside a guard in `X2BuffDisplayerTemplet.cpp`. That file was restored and re-patched before the build.
- The shared render-param reader is a public static member of `CX2BuffRenderParamDisplayerTemplet`, not a file-static function. The census follows only class methods, so with a free function it reported the six keys as unread.

**Census.** `buff_census.py --phase 4b`: **0 type/key pairs**.

**funcmap.tsv:** 52 rows ("Buff Phase 4b-iii"). All are `confirmed` except `3200a590` (`likely`).

**Build and deploy.**
- `X2Lib` (with `stdafx.cpp` touched) and `X2` at `US_SERVICE`, rc 0 and 0 errors. The only warning in a touched file is a pre-existing C4018 at `X2GUUser.cpp:17918`.
- `KTDXLIB` and `X2ServerProtocol` are unchanged (no `Always.h` edit).
- `x2.exe` is 19,032,576 B, linked 11:34:50, md5 **`a03acb224cb0c23221183083988a8544`**. It contains the new exact string `AFTER_IMAGE_COLOR`.
- Copied to the game directory as `X2_offline.exe` (names read programmatically: `x2.exe`, `X2_offline.exe`); the size and md5 match. **Not launched.**
- No Lua changed, so no pack is needed.

**Flag dependencies** (add to `BEHAVIOR_PARITY`'s `Always.h` note the next time that header is touched): the NPC owner increment also needs `DUNGEON_RANK_NEW` (`Always.h:2521`, `GetOwnerGameUnit`) and `X2OPTIMIZE_UNITTYPE_BUG_FIX` (`KTDX.h:355`, its `CX2GUUser*` return). The hitted gate reads `DamageData::m_iNaturalForceType`, which is `SERV_IRUHADEV_JUNE_DAMAGEDATA`'s, already a listed dependency.

**Excluded from the table:** `322a0730` is `CX2GameUnit`'s destructor. Its only callers are the scalar-deleting wrapper `322b5690` and the user and NPC destructors (`3203a620`, `3279fb60`).

**Things to watch in play** (ask before launching). The templet names are from March's decompiled `BattleFieldBuffTemplet.lua`:
- **Outline kept under a colour debuff.** These set `RENDER_TYPE` and `COLOR` but no `OUT_LINE_WIDE` / `COLOR_OUT_LINE` / `CARTOON_TEX_TYPE` / `ALPHA_BLEND`. March drew them with no outline; now the unit keeps its own:
  - `BTI_DEBUFF_FIRE` and `BTI_DEBUFF_POISON` (burn or poison from monsters or an elemental weapon);
  - the elixir debuffs `BTI_DEBUFF_ELIXIR_FLAME_RING_OF_ROSSO` and `BTI_DEBUFF_ELIXIR_ICE_BALL_OF_DENIPH`, on the monsters they hit.
- **No visible change expected:** `BTI_BUFF_SUPER_ARMOR` and `BTI_BUFF_STONE_APPLE` set everything but `COLOR`, so the unit keeps its own colour where March forced white.
- **After-image buff:** `BTI_BUFF_ELIXIR_FEATHER_OF_VENTUS` (or `BTI_BUFF_SPRINTER_MEDICINE`, `BTI_BUFF_GALE_APPLE`). On, in its colour, then off; an attack state's own after-image afterwards is gray.
- **Logs:** `ErrorLog.txt` should show no new `[JUNE_BUFF] runtime error` lines.

**Play-test (2026-10-04, user): passed** on the deployed exe `a03acb22…`. The checks were run from the dev console with `SetBuffFactorToGameUnitByBuffFactorID_LUA( BFI, 1 )`:
- fire (152) and poison (168) on the player: red tint, outline kept;
- fire (152) on every NPC: tint, outline kept;
- Super Armor (143) and Stone Apple (201): the unit's own colour kept;
- Sprinter Medicine (59): after-image in the buff's colour, off when the buff ends.

Logs, read after the session (launched 11:49):
- `ErrorLog.txt`: only the Phase 4a baseline `[JUNE_BUFF]` loader lines (duplicate BTI 87, both `_INT` not-found lines, ordinal rejects 481/482/483/372/373, the load summary) and **no `runtime error` line**.
- `State.log`: 101 lines, so no flood.
- The only `UNHANDLED` packet is `E_DISABLE_HB_CHECK_REQ` (×2), which is unrelated.

Not exercisable from the console, so left to the review: the `DT_BUFF` hitted gate (only a few monster attacks deal `DT_BUFF`, e.g. `DamageEffect_Summon_Bat_Heavy_Debuff_Bite`), the NPC owner hit count, and the `AT_COMMAND` normal-attack gate (no March data uses it).

**Phase 4b is done when — status:**
1. Verdict table: **met** (above).
2. Every March-reachable difference ported whole, March in `#else`: **met** (render param, after-image colour, normal-attack gate, hitted gate, owner hit).
3. Census 0: **met**.
4. Unifdef, build, deploy, md5 and the user's play-test: **met**.
5. Review with no open code defect: **met** (see "Phase 4b-iii — Review" below).

**Kickoff for the 4b-iii review** (Opus 5.5 high):

```
Reviewing the June buff/debuff port, Phase 4b-iii (fresh Opus 5.5 session, high effort).
Read JUNE_BUFF_PORT_PHASES.md -- "Starting context", "Phase 4b", "Phase 4b-iii" and
"Phase 4b-iii -- Done". Branch: mods/june-buff-port. Never launch X2_offline.exe.

Code: 2cb90fa (under SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY); pre-session commit 6312f1d.
Use fresh scratch tools; do not reuse the session's patch or unifdef scripts. Run the
flag-off unifdef on the committed blobs of 2cb90fa against 6312f1d and take numstat from
git diff 6312f1d 2cb90fa. Confirm the deployed X2_offline.exe is md5 a03acb22....

Independently re-check the verdict table in the raw asm (Ghidra read-only), especially:
32275140 (only set slots override; vslot 197 always first), 32eb7d50/32ebad40 (push only
when present), 32275a30/322720f0 (colour on enable, none on disable), 3236ed60's
"!= 1" gate, the hitted gates @320489c6/@327b4b82 (+0x56c != 4, +0x0 != 6) and the NPC
owner increment @3276ae3a. Look for readers of each store the dispxref sweep missed
(e.g. an indexed or lea-based access). Check that every copy of StBuffRenderParam goes
through the copy ctor (CX2RenderParamByBuff) so m_bHas* survive. Re-check the NPC
after-image colour conclusion: no reader of June's AFTER_IMAGE_ALWAYS byte +0x8cd4, and no
AFTER_IMAGE state table in March's or June's Ran.lua.

Report numbered findings (R1, R2...) with a severity. Fix a code defect under the same
flag, rebuild X2Lib and X2 at US_SERVICE (touch stdafx.cpp after a header edit), deploy,
record the md5, and ask before launching. If nothing is open and the user's play-test has
passed, mark Phase 4b complete. Add confirmed funcmap.tsv rows. Stage only your own hunks.
Commit. Record the review cost line.
```

### Phase 4b-iii — Review (2026-10-04, Opus 5.5 high)

Independent review of code commit `2cb90fa` (under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`) against the pre-session commit `6312f1d`, and of doc commits `4878e34`, `648817e` and `de252fa`. Read-only on code: nothing was edited, built or deployed.

Fresh tools only; the session's patch, unifdef and `dispxref` scripts were not reused:
- Ghidra, read-only (two decompiles; no renames or comments).
- A new capstone reader over a hand-parsed PE section map, with a whole-`.text` linear sweep (resync on a decode failure). It reports every instruction whose memory displacement **or** immediate equals a target. That covers `lea`, indexed (`[base+idx+disp]`) and `add reg, imm` accesses as well as plain loads and stores.
- A new one-flag byte-level unifdef over `git cat-file` blobs, with a flag-on control and a wrong-flag control (`SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY`).

**Verdict: no code defect.** Every item on the kickoff's "especially" list matches June's asm. R1-R5 are all Info.

| # | Check | Result |
|---|---|---|
| 1 | `ApplyRenderParam` `32275140` | **Pass.** `+0x6840` set → return. Then vslot 197 (`call [vt+0x314]`) runs unconditionally. `+0x669c == +0x66a0` (empty) → vslot 199 (`+0x31c`) and return. Otherwise `back()` (`[end-4]`); a null pointer skips. Each key is gated on its slot's begin ≠ end: render type `+0x10`, cartoon `+0x20`, outline width `+0x30`, outline colour `+0x40`, colour `+0x50`. Alpha blend `+0x60` is a `vector<bool>`, gated on bit count `+0x70` ≠ 0 and read from bit 0. Inside the colour gate, `fld gap(+0x74); fcomp [334350e8]` (a double 0.0); `test ah,0x41; jne` takes the plain colour when gap ≤ 0, otherwise `322722e0`. That is the port, ordering included. |
| 2 | Parses `32eb7d50` / `32ebad40` | **Pass.** `32eb7d50`: `RENDER_TYPE` (`33449c38`), `CARTOON_TEX_TYPE`, `OUT_LINE_WIDE`, `COLOR_OUT_LINE`, `COLOR` (`3343aa84`) and `ALPHA_BLEND` are each read with `cmp al,1 / jne`, which skips the slot's push. So a key the script lacks stays empty. `FLICKER_COLOR_GAP` (XOR-decoded here) is stored at `+0x74`, with `fldz` on a miss. Return `al = 1`. `32ebad40`: the same six keys with the same `cmp al,1 / jne`. Its two colours inline `GetUserDefinedTypeValue`: `Get` `32fe98c0`, then `isuserdata(-1)` `32fe4de0`; on failure it pops and skips the push. That is `LuaCommon/KLuaManager.h:1082` exactly, which the port calls. No flicker read there. |
| 3 | `EnableAfterImage` `32275a30` / `DisableAfterImage` `322720f0` | **Pass.** Enable: option `[0x3377585c]+0x1ac == 0` and `+0x3e59 == 0`, then `+0x194 = 1`. The colour is `back()+0x10..0x1c` when `+0x6744 != +0x6748`, **else** `+0x6880..+0x688c`. Disable: force, **or** an empty list, **or** option `== 2` → `+0x194 = 0`. There is no colour write on any path. The port matches both. |
| 4 | `3236ed60` gate | **Pass.** `mov eax,[esp+0x48]` is the second argument, the attack type; `dec eax; je skip`, so only type 1 skips. The cool gate `fld cool; fld gap; fcompp; test ah,0x41; jp skip` proceeds when cool ≥ gap. That is the port's `AT_SPECIAL != type`. The impact-point sibling `3236ea50` has the same prologue. |
| 5 | Hitted gates | **Pass.** User: vslot 102 (`+0x198`, `IsMyUnit`) at `320489c6`, then `cmp [ebx+0x56c],4 / je` at `320489d7` and `cmp [ebx],6 / je`, then `++[ebp+0x4230]`. NPC: `cmp byte [esp+0x32],1` (`bLocalUnit`) at `327b4b82`, then the same two compares at `327b4b91`. March: `DT_BUFF` = 6, `NFT_RESULT_BOMB` = 4, and `m_iNaturalForceType` defaults to −1 (`X2DamageManager.h:1371`). Read uncompressed, both port blocks sit inside their `IsMyUnit()` / `bLocalUnit` braces, and the unifdef shows the `#else` is March's code. |
| 6 | NPC owner increment | **Pass.** `3276ae07`: `reActResult(+0x3c8) != 7` (`RT_DUMMY_DAMAGE` = 7) and vslot 103 (`+0x19c`) → `++hit`, `++[+0x4232+type]`. Then `32759f00` (call at `3276ae3c`) is null-checked, called again, and `++owner+0x4231` plus the owner's per-type counter, all inside the same branch. `32759f00` is observer `+0x8d5c` → observable − 0x1a0, i.e. March's `GetOwnerGameUnit`. |
| 7 | Readers the `dispxref` sweep might miss | **None missed.** Displacements swept: `669c/66a0/66a4`, `66fc/6700/6704`, `670c/6710/6714`, `6744/6748/674c`, `675c/6760/6764`, `677c/6780/6784`, `4230`, `4231`, `4238`, `6880..688c`, `8cd4`, `7c80`. Then a second sweep for overlapping-width or neighbouring access: `8cd0..8cd3`, `8cd5`, `422c..422f`, `4234`, `4236`, `6740`, `687c`, `6890`. Every hit outside the session's table is one of these: `esp`-relative stack locals in four large functions (`31d95080`, `321abd20`, `32da1700`, `32e102f0`); a known writer; a ctor/dtor clear; or EH unwind funclets in `33251dd0` (`mov ecx,[ebp-0x10]; add ecx,disp; jmp dtor`). `+0x8cd0` is a separate float with its own users. `+0x4234`/`+0x4236` are other counters (the per-type ones are hand-off 5). |
| 8 | `+0x8cd4` (AFTER_IMAGE_ALWAYS) | **No reader.** Three writers: setter `31e4d6f0` (one byte store, bound at `31e088dc` under the name decoded at `31e0876c`), the ctor `@327627c0`, and NPC init `327a0fa0 @327a2c5c` (`AFTER_IMAGE_ALWAYS`, `xorstrings.tsv` row `327a2b50`, through bool getter `32fe9e40`, 0 on a miss). |
| 9 | Copies of `StBuffRenderParam` | **All through the copy ctor.** `X2OPTIMIZE_REMOVE_UNNECESSARY_SHARED_PTR` is unconditional (`KTDX.h:340`), so `operator=` is private and undefined, and an assignment cannot compile. The copy paths: both `GetClonePtr`s copy-construct the displayer (June `32eb8020` calls `322ad270` on `+0x10`); `CX2RenderParamByBuff`'s ctor; and both `CreateAndInsertRenderParamByBuffUnitType` overrides pass `*ptr` to `CreateAndInsertRenderParamByBuff`. The by-unit-type map holds intrusive pointers. The new members are declared after `m_fFlickerColorGap` and initialised in the same order. |
| 10 | NPC after-image colour | **Holds.** Neither March's nor June's `Ran.lua` / `Ran_Event.lua` has a bare `AFTER_IMAGE` state key. June's source has only the init keys. In March's bytecode the only bare-looking match is `USE_AFTER_IMAGE`. No March script among the 5,143 plain `.lua` files in `kom_contents` has the exact key `AFTER_IMAGE_COLOR`. June's NPC window `@327b6887` calls `EnableAfterImage(3)` / `DisableAfterImage(0,3)`, and no NPC path writes `+0x6880`. So June also repaints an NPC's init colour (`AFTER_IMAGE_COLOR_R/G/B/A`, `X2GUNPC.cpp:4124-4137`) to 0.8 gray on enable, and the port is June-equal. |
| 11 | `AFTER_IMAGE_COLOR` read | **Pass.** The XOR bytes at `3379d708` decode to `AFTER_IMAGE_COLOR`. The read is at `3205b857..` via `31c9eba0` into `+0x6880`, with the 0.8 default (`3343b240`) on a miss. It comes after both `AFTER_IMAGE` branches join at `3205b70d`, and the else branch's `DisableAfterImage` (`322720f0`) runs first. That is the port's placement. |
| 12 | Other "same" rows spot-checked | **Pass.** vslot 197 Aisha override `32cc1c50`: transform fade into `m_RenderParam` (`+0xc10` type 0x11, colour alpha), then base `3226e190`, as `X2GUArme_VioletMage.cpp:19744`. Damage-effect hit `31f87a60`: `!= RT_DUMMY_DAMAGE`, then the owner non-null, then owner vslot 102, then `++owner+0x4231`, as `X2DamageEffect.cpp:5460-5462`. March has no counter site beyond the ones in the table (grep of all `X2Lib`). `EnableWeaponAfterImage` `32275b00`: an empty list returns, with no else-colour branch, as March. |
| 13 | Sync apply gate (the session's open item) | **Closed.** `3206c1a2`: vslot 102 (`IsMyUnit`) true → skip. Otherwise HP, then hit `+0x4231 = +0x80fa`, the per-type counters from `+0x8104..+0x8106`, hitted `+0x4230 = +0x80fb`, and debuff `+0x4235 = +0x80fc`. That is March's `if( IsMyUnit() == false )` block (`X2GUUser.cpp:17501-17514`) plus hand-off 5's fields. |
| 14 | Hygiene and deploy | **Pass.** `numstat 6312f1d 2cb90fa`: 216/0 over the seven files. All 216 added lines are ASCII and CRLF, and `file` signatures are unchanged. The fresh unifdef gives strip(`2cb90fa`) == strip(`6312f1d`) flag-off on all seven; the flag-on and wrong-flag controls differ on all seven. `X2/US_SERVICE/x2.exe` and the game directory's `X2_offline.exe` are both 19,032,576 B, md5 **`a03acb224cb0c23221183083988a8544`** (names read programmatically). |

**Not re-checked here** (the session's verdicts stand on its own evidence): slash trace `+0x4238`, the ImpactPoint/Hitted/NormalAttack inserts and erases, `PlayEffectSet*ByBuff`, the cool-time tick, vslot 154, the after-image insert and erase, and the vslot-197 overrides other than Aisha's.

**Findings**
- **R1, Info (open item closed).** The sync apply's `!IsMyUnit` gate is traced (row 13); it is the same as March.
- **R2, Info (no missed reader).** Rows 7-8. The sweep method and its extra displacements are recorded above, so a later phase can extend it rather than redo it.
- **R3, Info (kickoff addresses).** `@320489c6` is the `IsMyUnit` vcall; the hitted gate's first compare is `320489d7`. `@3276ae3a` is `mov ecx,esi`; the owner call is `3276ae3c`. The code comments already cite `@320489d7` and `@327b4b91`. No change.
- **R4, Info (outside buff scope).** In the sync apply, after `SetNowHp` (vslot 61), June calls vslot 404 (`+0x650`) with no argument where March calls `SetNowMp( syncData.fNowMP )`. That function was not identified. It is not a buff store, so it is left for whichever phase owns sync parity.
- **R5, Info (NPC init colour).** The Done note's "no practical change" covers both `ChangeAfterImageColor` and the init `AFTER_IMAGE_COLOR_R/G/B/A`. Row 10 confirms June repaints the init colour too, so the port is June-equal there, not just equal in practice.

**funcmap.tsv:** 7 rows ("Buff Phase 4b-iii review"), all `confirmed` and all new:
- `32eb8020` RenderParam `GetClonePtr`;
- `31e4d6f0` `SetEnableAfterImageAlways`;
- the by-name `KLuaManager::GetValue` int / float / bool (`32fe9fd0` / `32fea370` / `32fe9e40`);
- `GetUserDefinedTypeValue<D3DXCOLOR>` `31c9eba0`;
- `KLuaManager::Get` `32fe98c0`.

**Build and deploy:** none, because there is no code defect. The deployed exe is still `a03acb22…` (`2cb90fa`). The user's play-test on it passed (above).

**Phase 4b: complete.** All five "done when" items are met. Phase 5 starts next.

### Phase 5 — Done (2026-10-04), play-tested (user)

**Result.** All seven types are ported under `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT`; none is Opus-only. `buff_census.py --phase 5` no longer lists any of them (March "handles 79", was 72). Client build `US_SERVICE` rc=0 for KTDXLIB, X2Lib (PCH invalidated with `touch X2Lib/stdafx.cpp`) and X2. `git diff --stat`: 351 insertions, 0 deletions; `file` on every touched file unchanged. Deployed `X2/US_SERVICE/x2.exe` as `X2_offline.exe` (names read programmatically: `x2.exe`, `x2.exe.bak`, `x2.fx`, `X2_offline.exe`): 19,040,256 B, md5 **`407067dcf1f31a0ca858f39c1db3eb4d`**, byte-identical to the build output. The exe contains all seven `BBT_*` key strings (ASCII and UTF-16) and the debug strings `first apply BBT`, `first read BBT`, `console hook BBT`, `JuneBuffTestNewStat_LUA`. **Not launched.** The four read sites (fixed-damage path and main path of `GetFinalDamage`, melee and range branches of `SetDamageExceptionProcess`) were each proven compiled in under `US_SERVICE` with a temporary `#pragma message` and a one-file compile (removed again; `git status` clean afterwards).

**Play-test (2026-10-04, user): all four console checks passed** on the deployed exe `407067dc…`. The user reports the Hyper mode badge went from 30 s to 60 s with `JuneBuffTestNewStat_LUA( 76, 1, 100, 1 )`. `ErrorLog.txt` agrees, and holds no `runtime error` line:
- 108: `first apply BBT 108, change type 3, value 2`, then `first read BBT 108: 19085.9 * 9542.96` (the sum is exactly 2x the base 9542.96); erase logged.
- 109: `first apply BBT 109 … value 2` (once per monster), `first read BBT 109: 9578.55 * 4789.28` (2x).
- 76: `first read BBT 76: hyper time percent 101.5 (socket alone 1.5)`, i.e. the socket bonus plus the +100, so the option-value seed works.
- 85/86/87/96: four `first apply` lines, no error. Their read sites remain unexercised (no `AT_COMMAND` attack in March data).
Not covered by this test, as stated above: the parse, level slice and templet path (needs June data, Phase 13/14).

**Flags.** `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT` (requires `_ENUM` for the enumerators and `AT_COMMAND`, and `_BEHAVIOR_PARITY` for the level slice and the ChangeStat templet parity) and `SERV_IRUHADEV_JUNE_BUFF_DEBUG`, both appended to `KTDXLIB/Always.h`. `DEBUG` had been listed since Phase 3 but never defined; it is defined now so the step-8 first-application lines compile in. Undefine it and no debug string remains. Every edit is a pure insertion inside the flag; nothing needed an `#else` except `CX2GUUser::GetHyperModeTime`, where the original line sits in the `#else`.

**Per type** (June members are `CX2GameUnit` offsets; the port uses new named members and never an offset):

| Type | Member (basic) | June insert | Parse / slice / SGU | Read site (March) |
|---|---|---|---|---|
| 76 CHANGE_HYPER_DURATION | `m_AdditionalHyperDuration` (0.0) `+0x64a0` | `32282ddc` | parse both lists; slice already in (ChangeStat group); **SGU case added** | `CX2GUUser::GetHyperModeTime` (`X2GUUser.cpp`): `fTotal += fTotal * GetHyperDurationPercent() * 0.01f`. Option value seeded in `InitAdditionalStatOptionValue` from `m_SocketData.m_fHyperModeTime` |
| 85 RANGE_COMMAND_MP_GAIN_RATE | `m_AdditionalRangeCommandMpGainRate` (1.0) `+0x68d4` | `32282ee6` | parse both lists; default slice; no SGU | `SetDamageExceptionProcess`, range branch (`@3229947c`) |
| 86 MELEE_COMMAND_MP_GAIN_RATE | `m_AdditionalMeleeCommandMpGainRate` (1.0) `+0x6988` | `32282ec3` | same | `SetDamageExceptionProcess`, melee branch (`@3229935a`) |
| 87 MELEE_COMMAND_DAMAGE_RATE | `m_AdditionalMeleeCommandDamageRate` (1.0) `+0x6a3c` | `32282f09` | same | `GetFinalDamage` main path, `@32286270` |
| 96 RANGE_COMMAND_DAMAGE_RATE | `m_AdditionalRangeCommandDamageRate` (1.0) `+0x6af0` | `32282f2c` | same | `GetFinalDamage` main path, `@322862a7` |
| 108 FINAL_DAMAGE_MULTIPLIER_NPC | `m_AdditionalFinalDamageMultiplierNpc` (1.0) `+0x5ce4` | `32282f72` | same | `GetFinalDamage`, fixed-damage path (`@32285fd9`) and main path (`@32286218`) |
| 109 FINAL_DAMAGE_MULTIPLIER_USER | `m_AdditionalFinalDamageMultiplierUser` (1.0) `+0x5d98` | `32282f95` | same | same two paths (`@32285fba`, `@3228620a`) |

All seven also got: templet-factory case (`CX2BuffChangeStatBehaviorTemplet`), Insert / Update-or-insert / Erase cases, a plain `CheckAndUpdateResultValue()` in `CheckAndUpdateAdditionalStat`, `SetBasicStatValue`, `InitAdditionalPassiveMultipleAndSum`, and the `Verify()` chain. UDP layout needs nothing (the factor is still type + `vector<float>`, `GetFactor`/`SetFactorFromPacket` are the ChangeStat ones).

**What was settled by reading June (all inside the addresses the map names):**
- **Refresh variant.** June's update-all `32290950` refreshes all seven members through the plain result update `3236d350` (`+0x64a0`, `+0x68d4`, `+0x6988`, `+0x6a3c`, `+0x6af0`, `+0x5ce4`, `+0x5d98`), not the option-data variant. So 76's result is `basic 0 + option (socket %) + FIX + RELATION`, and with no buff it equals `m_fHyperModeTime` exactly, as before the port. `SetOptionValue` marks the member dirty and `CX2GameUnit::OnFrameMove` refreshes every frame, as for `m_AdditionalHyperCharge`.
- **Read-site decode.** In `GetFinalDamage` `32284aa0`: `[ESI+0x264]` is the defender's unit type (0 = `GUT_USER`); zero picks `+0x5d98` (109), non-zero `+0x5ce4` (108). `EBP` is the attacker. `[EBX+0x8] == 2` is `attackType == AT_COMMAND` and `[EBX+0x2c]` is `m_bMeleeAttack`; melee picks `+0x6a3c` (87), else `+0x6af0` (96). In `SetDamageExceptionProcess` `322990e0`, `[EDI+8] == 2` gates both; `+0x6988` (86) sits in the first block, `+0x68d4` (85) in the second. This corrects nothing in the map; it confirms every melee/range and NPC/user pairing.
- **Order of terms.** June computes `inc2 = base * (member - 1)` from the same base as the final-defence term and the existing melee/range term, and sums all four (`base + decrease + increase + inc2`). 87/96 then multiply that sum, before the damage-reduce step. March's fixed-damage path gets only 108/109. Float order in the port is `(base + inc2) + increase` rather than `base + (inc2 + increase)`; algebraically equal.
- **Option reset branch.** June's `InitAdditionalStatOptionValue` `32270dd0` is `if ( unitType == user ) { seed from the socket block } else { zero everything }` (the `@3227143d` zero is the NPC branch). March's function has no such branch (it seeds every unit from `m_SocketData`), so only the seed line was added; an NPC's `m_SocketData` is zero.

**Where 85/86's block landed (for Phase 6, type 91).** `CX2GameUnit::SetDamageExceptionProcess`, in the `if ( pDamageData_->m_bMeleeAttack ) { … } else { … }` pair that follows the `#endif` of `BALANCE_PATCH_20120329`. Each branch ends with `pDamageData_->fHitAddMP = pDamageData_->optrAttackerGameUnit->GetChargeMpHit{Melee,Range}Attack();`. The `#ifdef SERV_IRUHADEV_JUNE_BUFF_NEW_STAT` block (`if ( AT_COMMAND == attackType ) fHitAddMP *= GetMeleeCommandMpGainRate()` in the melee branch, `GetRangeCommandMpGainRate()` in the range branch) is inserted directly after that assignment line in each branch. 91 scales the same `fHitAddMP` unconditionally on attack type: add it after these two blocks, or after the whole if/else (both are equivalent as it does not depend on the branch).

**Open items and decisions for the review / the user.**
1. **Default level slice: checked, closed.** June's jump table sends 85, 86, 87 and 96 to case 13 = `32d4812a`, and 108/109 are above the bound `0x68`, so all six use the default (already ported by 4b-i): `[ (int)factor[0], factor[1] < 30 ? factor[0] : factor[level+1] ]`. The factor file parses with `bIsSkill = true` (`ParsingBuffFactor` default), so the layout is counted, `[ct, count, v1..vN]`, and `factor[1]` is N. Measured in `june_src_fixed/data036_kom/BattleFieldBuffFactor.src.lua`: **every** `PERCENT` / `FIX_VALUE` table of all 18 rows carrying these six types plus 76 has exactly 30 entries (BFI 355, 506, 626, 822, 1109, 1110, 1111, the ERDA ones and the rest). `30 < 30` is false, so the default takes `factor[level+1]`, the per-level value. It would hand back the change type only for a table shorter than 30; none exists in June's data. (Transcription re-checked in the disassembly: `PUSH EBX` with `EBX = 0` is `factor[0]`.) The debug line `first apply BBT <n>, change type <c>, value <v>` still prints the value the store actually received, so Phase 14 can confirm it on real data.
2. **Console reach (user's request).** The shipped March data cannot reach these types through `SetBuffFactorToGameUnitByBuffFactorID_LUA`. I added a DEBUG-only hook, `JuneBuffTestNewStat_LUA( type, changeType, value, insert )` (`CX2GameUnit`, bound in `X2Main.cpp` beside `SetBuffFactorToGameUnitByBuffFactorID_LUA`, `#if defined( NEW_STAT ) && defined( DEBUG )`). It calls `InsertAdditionalStat` / `EraseAdditionalStat` with a made-up identity (BTI `900000 + type`, unique 1). It exercises the stores, the refresh and the read sites, **not** the parse, the level slice or the templet. Remove with `DEBUG`.
3. **85/86/87/96 readers are not reachable in game yet.** They only act when `attackType == AT_COMMAND`, and no March damage data sets it (the 4b-iii review said the same). Only their stores can be checked until a June attack that carries `ATTACK_TYPE.AT_COMMAND` is loaded.
4. **Not ported here, as the map says:** the class overrides of the 76 reader (Ain `32a4fe50`, `329e5180`/`329e56c0`) and Chung's virtual `32c16be0` (sets 85/86's passive values). Those classes are not in March and ride with their classes. *Corrected by the Phase 5 review: only Ain is absent. `329e5180`/`329e56c0` are `CX2GUAdd`, and the review ported that read (R1). `32c16be0` is `CX2GUChung::InitPassiveSkillState`, and it is handed to the user (R2).*

**How to see it in game (for Phase 14).** June BFI/BTI numbers from `june_src_fixed/data036_kom/Enum.src.lua`; the data comes with Phase 13's June `BattleFieldBuffFactor`/`Templet`. For a unit to *have* the buff the factor must be applied, e.g. `SetBuffFactorToGameUnitByBuffFactorID_LUA( BFI, 1 )` once June data is loaded.

| Type | June BFI (BTI) | Where it comes from | What to watch |
|---|---|---|---|
| 76 | `BFI_BUFF_ERDA_RESOURCE` 506 (`BTI_BUFF_ERDA_RESOURCE` 447); also `BFI_BUFF_DISPARATE_FORCE_POWER_OF_EL`, `BFI_BUFF_DESTROY_SEED_OF_CHAOS_RECREATION` | an item buff (ledger bucket "item") | the Hyper-mode timer after activating is longer by the buff's FIX % (10 at level 1) |
| 85 | `BFI_BUFF_MIND_OF_MARKMAN` 626 (`BTI_BUFF_MIND_OF_MARKMAN` 536); also `BFI_BUFF_NATURAL_FORCE` 355 | a skill | MP gain on ranged *command* attacks (needs an `AT_COMMAND` attack) |
| 86 | `BFI_BUFF_MIND_OF_MARKMAN` 626 | a skill | MP gain on melee command attacks |
| 87 | `BFI_BUFF_MIND_OF_MARKMAN` 626; `BFI_AMUSEMENT_ITEM_SUPER` 822 (`BTI_AMUSEMENT_ITEM_SUPER` 797) | a skill, an amusement item | damage of melee command attacks |
| 96 | `BFI_AMUSEMENT_ITEM_SUPER` 822 | an amusement item | damage of ranged command attacks |
| 108 | `BFI_SPELL_OF_DESTROY` 1109 (`BTI_SPELL_OF_DESTROY` 1028); also `BFI_ERDA_SPELL_OF_SOURCE` 1111 | a spell buff | your hits on monsters deal more |
| 109 | `BFI_SPELL_OF_BATTLE` 1110; also `BFI_ERDA_SPELL_OF_SOURCE` 1111 | a spell buff | monsters' (or enemy players') hits on you deal more |

**Play-test hand-off (dev console, END key; `settings.auth_level` = 3 is already set).** Every line below is **checked from source only and never run**: the hook is bound with `lua_tinker::class_def<CX2GameUnit>` exactly like `SetBuffFactorToGameUnitByBuffFactorID_LUA`, `GetNPCUnit` returns `nil` for an empty slot, and `CX2GUNPC` inherits the method (the doc's earlier note). The `value` is a multiplier for `change type 3` (percent) and a plain percent for `1`. Undo any check by repeating its line with the last argument `0`.

1. **Your hits on monsters double (type 108).**
   - Where: any field or dungeon, next to a monster you can hit.
   - Paste:
     ```
     g_pX2Game:GetMyUnit():JuneBuffTestNewStat_LUA( 108, 3, 2.0, 1 )
     ```
   - What it does: makes everything you hit a monster with deal twice the damage.
   - Look for: damage numbers on your next hits about twice what they were; `ErrorLog.txt` gets `console hook BBT 108`, `first apply BBT 108, change type 3, value 2` and `first read BBT 108`.
   - If not: damage unchanged, or no `first read BBT 108` line, means the fixed-damage/main path did not pick the member up. Tell me "nothing happened" and send the `[JUNE_BUFF]` lines.
   - Undo: `g_pX2Game:GetMyUnit():JuneBuffTestNewStat_LUA( 108, 3, 2.0, 0 )`

2. **Monsters hit you for double (type 109).**
   - Where: a field or dungeon with monsters on the map, you not yet engaged.
   - Paste:
     ```
     for i = 0, g_pX2Game:GetNPCUnitListSize() - 1 do local n = g_pX2Game:GetNPCUnit( i ) if n ~= nil then n:JuneBuffTestNewStat_LUA( 109, 3, 2.0, 1 ) end end
     ```
   - What it does: gives every monster currently on the map a double-damage-against-players bonus.
   - Look for: a monster hit takes about twice as much HP as before; log gets `first apply BBT 109, change type 3, value 2` and `first read BBT 109`.
   - If not: HP loss unchanged means the user branch was not taken. Tell me, or send a screenshot of the damage number.
   - Undo: the same line with the last `1` changed to `0`.

3. **Hyper mode lasts longer (type 76).**
   - Where: any field, with a character that has Hyper mode available.
   - Paste:
     ```
     g_pX2Game:GetMyUnit():JuneBuffTestNewStat_LUA( 76, 1, 100, 1 )
     ```
   - What it does: adds 100% to the Hyper mode duration.
   - Look for: activate Hyper mode **after** pasting; it should last about twice as long as normal. Log gets `first apply BBT 76, change type 1, value 100` and `first read BBT 76: hyper time percent 100 (socket alone 0)` (the second number is your equipment's own bonus, so it may be larger than 0 and the first number then equals it plus 100).
   - If not: if you see the `first apply` line but no `first read BBT 76`, hyper mode is not reading that getter; tell me "no first read". A `first read BBT 76` line that does not show your +100 means it fired early (it is a one-shot shared by every unit, so a bot with its own socket bonus can trip it); send it and I will look.
   - Undo: `g_pX2Game:GetMyUnit():JuneBuffTestNewStat_LUA( 76, 1, 100, 0 )`

4. **The four command-attack stores accept and release a value (85, 86, 87, 96).**
   - Where: any field or dungeon.
   - Paste:
     ```
     local u = g_pX2Game:GetMyUnit() u:JuneBuffTestNewStat_LUA( 85, 3, 2.0, 1 ) u:JuneBuffTestNewStat_LUA( 86, 3, 2.0, 1 ) u:JuneBuffTestNewStat_LUA( 87, 3, 2.0, 1 ) u:JuneBuffTestNewStat_LUA( 96, 3, 2.0, 1 )
     ```
   - What it does: stores a x2 for the four "command attack" types on you; they only act on command attacks, which no current attack is.
   - Look for: nothing visible in play: normal attacks keep their damage and MP gain; no popup or crash; `ErrorLog.txt` gets four `first apply BBT 85/86/87/96` lines and no `runtime error` line.
   - If not: a popup, crash or a `Behavior Type Invalid` line means a store is missing; send the lines.
   - Undo: the same line with each last `1` changed to `0`.

**Log lines to read afterwards** (this phase's own, `ErrorLog.txt` in the game directory): `[JUNE_BUFF] console hook BBT …`, `[JUNE_BUFF] first apply BBT …`, `[JUNE_BUFF] first read BBT …`. Nothing else is in scope. Per the standing rule these are all `[JUNE_BUFF]` lines and no popup is raised.

### Phase 5 — Review (2026-10-04, Opus 5.5 high)

Independent review of code commit `d899606` (against its parent `d0aa29c`) and doc commits `b5d3b06` and `98ce6f9`. The review used Ghidra read-only (no renames or comments) and fresh tools: a new byte-level unifdef over `git show` blobs that resolves `#ifdef`, `#ifndef` and `#if defined( A ) && defined( B )`, and the existing `buffre/dispxref.py` sweep, bucketed by function start (the full Ghidra function list, bisected).

**Verdict: the transcription is correct.** Every value, member pairing, gate and term order the commit ported matches June's asm. One reader was missed because the plan said its class was not in March, but it is. The review ported it (R1). A second reader in a March class needs a design decision (R2).

| # | Check | Result |
|---|---|---|
| 1 | Enum values | **Pass.** C++ `X2BuffFactor.h` 76/85/86/87/96/108/109, `X2DamageManager.h` `AT_COMMAND = 2`, March `Enum.src.lua` and June `Enum.src.lua` all agree. |
| 2 | Parse (float/key order) | **Pass.** All seven are in both macro lists on `ParsingChangeStatBehavior` (June `32d445d0`, the same layout as March's siblings). The factory has seven `CX2BuffChangeStatBehaviorTemplet` cases. No new type belongs in `GetValue` / `MustUseOptionData` (June's option-data set `32e764f0` is 11-16, 20, 21, 72 only). |
| 3 | Level slice and SGU | **Pass.** 76 is in 4b-i's ChangeStat group, and the other six fall to the default slice. Only 76 is in the SGU ChangeStat group. `X2BuffFactor.cpp:2877` is the March `#else` of the slice, unreachable with `BEHAVIOR_PARITY`. |
| 4 | Store and basic value (spot-check, asm) | **Pass.** The insert cases `32282ddc…32282f95` take `lea ecx,[esi+M]` with M = `64a0 / 68d4 / 6988 / 6a3c / 6af0 / 5ce4 / 5d98` for 76/85/86/87/96/108/109, so every pairing is right. In `32271540`, 76 gets `fldz` and the other six get `fld1`. `322718e0` gives each member `SetPassiveMultiple(1)` (`3236ccc0`) and `SetPassiveSum(0)` (`3236cbf0`). The refresh `32290920` calls the plain `3236d350` on all seven. Update `3226fd70` and erase `322830f0` touch all seven. |
| 5 | 76 seed and reader (spot-check) | **Pass.** `32270dd0` reads the socket block `+0x18` (HyperCharge, `+0x63ec`) and then `+0x1c` (76, `+0x64a0`); `X2SocketItem.h:373-374` has `m_fHyperModeChargeRate` then `m_fHyperModeTime`. `31fd1c00` is `fTotal * result(+0x6540) * 0.01 + fTotal` on the same base as March. |
| 6 | 87/96/108/109 in `GetFinalDamage` (spot-check) | **Pass.** Fixed path: `base + base·(melee/range − 1) + base·(R − 1)` with R = `+0x5e38` (109) when defender `[+0x264] == 0` (user), else `+0x5d84` (108), then clamped at 1. Main path: `(base + decrease) + (inc2 + increase)`, then `× (m_bMeleeAttack ? +0x6adc (87) : +0x6b90 (96))` only when `[EBX+8] == 2`. `ESI` is the defender and `EBP` the attacker. This is the port, term for term. |
| 7 | 85/86 in `SetDamageExceptionProcess` (spot-check) | **Pass.** The melee branch multiplies by `+0x6a28` (86) and the range branch by `+0x6974` (85), each gated on `[EDI+8] == 2`. The `jnz 32299510` skips only the multiply and store. |
| 8 | Whole-phase reader sweep | **Two misses, both in March classes (R1, R2).** `dispxref` over the seven member bases and their result and XOR words gave 218 hits in 61 functions. Every hit is one of these: the known insert, erase, update, option, basic, passive or refresh code; the two read sites; the ctor `322a0730` and dtor `322a2700` (March builds members by declaration); stack locals in `31d95080`, `321abd20` or `32da1700`; byte-misdecodes (`add byte [ecx+0x98c4], al`); constants used as IDs (`cmp/push 0x6978`, `0x68f4`); or the four 76 readers `31fd1c00`, `329e5180`, `329e56c0` and Ain's `32a4fe50`; or Chung's `32c16be0`. |
| 9 | Hygiene | **Pass.** `numstat d0aa29c d899606`: 351/0 over seven files. `file` signatures are unchanged on all eight touched files (`X2GUAdd.cpp` included). All 359 added lines (Phase 5 plus R1) are ASCII and CRLF. The flag-off unifdef (`NEW_STAT` and `DEBUG` both off) equals the parent byte for byte on `X2BuffBehaviorTemplet.cpp`, `X2BuffFactor.cpp`, `X2GUUser.cpp`, `X2GameUnit.h` and `X2GUAdd.cpp`; the residue on the other two is R3. Controls: flag-on and the wrong flag (`TEMPLET_PARITY`) differ from the parent on every file. `Always.h` adds only the two define blocks. |

**Findings**
- **R1, fixed (missed read site, March class).** June `329e5180` is `CX2GUAdd::GetEffectiveDecreaseDPConsume`: `(1 − m_fUsingDPValueChangeRate) + result(76) · 0.01`. `329e56c0` is the factored-out body of `CX2GUAdd::UpNowDPValue`, which inlines the same term. March has both functions (`X2GUAdd.cpp:9474`, `9086`) and still reads `GetSocketData().m_fHyperModeTime · 0.01f`. Phase 1 left the class unattributed, and the Phase 5 scope deferred it as "not in March". **Where the fix sits.** It is in `UpNowDPValue`'s consume branch only, under `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT`, with March's line in the `#else`: `( 1.f − m_fUsingDPValueChangeRate ) + GetHyperDurationPercent() · 0.01f`, in June's order.
- `GetEffectiveDecreaseDPConsume` and `GetUseDPByFormationMode` stay March. A raw `.text` scan finds no `call`/`jmp` to `329e5180` and no pointer to it, so it is dead in June.
- June's formation-mode entry (`32a08580`) computes its cost as `[+0x9620] × [+0x95f0]` and calls `UpNowDPValue( −cost, 1.0 )` (vslot `0x2a4`). It no longer adds the decrease term inside the cost, so the 76 store reaches formation entry only through `UpNowDPValue`, exactly as the fix does.
- A first placement inside `GetEffectiveDecreaseDPConsume` would also have fed the formation cost. It was built and deployed (`63ed5dcb…`, committed as `60d8eb4`), then replaced before handing over.
- With no buff the value is unchanged, because the member's result is then exactly the socket value.
- **R2, decision for the user (Chung).** June `32c16be0` is `CX2GUChung::InitPassiveSkillState` (March `X2GUChung.cpp:2659`). After the base call it does `SetPassiveMultiple( passiveMultiple × K )` on both 85 (`+0x68d4`) and 86 (`+0x6988`). K is a Chung protected float `+0x98c4`, read by the Chung script reader `32bf4900` from a **new init-script key, `COMMAND_HIT_ADD_MP_BONUS`** (default 1.0; `xorstrings.tsv` row `32bf4b82`). June's `Chung_Iron_Cannon.src.lua:284` sets it to **1.15**. March's Chung has no such member or key, and the rest of June's `InitPassiveSkillState` is a rewrite (June-only skill IDs `0x6659`–`0x6731`). Porting it means a new `CX2GUChung` member, a script read, and either June's `Chung_Iron_Cannon.lua` or a hand edit of March's. It only changes MP gain on `AT_COMMAND` hits, and no March attack has that type. **Options:** (a) port the member, the key read and the two `SetPassiveMultiple` lines now, under `NEW_STAT`, and take the key from June's Chung script when Chung's data is adopted; (b) leave it to a Chung-class port, as the plan said. No code was changed for R2.
- **R3, Info (whitespace).** The flag-off strip leaves three blank lines that the commit added just outside its guards: one after the `JuneBuffTestNewStat_LUA` binding in `X2Main.cpp`, and two around the debug helpers above `GetFinalDamage` in `X2GameUnit.cpp`. They change no code. Left as is.
- **R4, Info (rule 9).** Rule 9 upgrades Phases 5 and 6 to Opus "if their reviews find value or layout errors". None was found: R1 and R2 come from the map's scope line (an unattributed class marked "not in March"), not from the Sonnet transcription. In this review's reading, rule 9 is not triggered. The user decides.
- **R6, decision for the user (Add DP, outside buff scope).** Tracing R1 showed that June changed Add's DP code in places that have nothing to do with buffs. They are not ported here; they belong to an Add-class pass.
  - June's formation-mode cost is `m_fUseDPByFormationMode × m_fDPValueRateByActiveFormationMode`, with no `+ GetEffectiveDecreaseDPConsume()` (March `X2GUAdd.cpp:9443`).
  - June's Fantasy Tracer MP refund on formation entry (`32a085ac..`, gate `[+0x9640]`) is computed from the DP actually spent: the DP before minus the DP after.
  - June's `UpNowDPValue` (`329e5b00`) multiplies every consumption by `1 − result(+0x6bb0)`, an additional-stat member that no buff inserts into (it is absent from the `32282700` switch), probably an Add passive.
- **R2 and R6, resolved (user, 2026-10-04): both go to the character port.**
  - Add on this branch is incomplete, and the user will port it from June rather than extend March's Add. The existing March characters were revamped in June and will be ported whole too.
  - R2 would need code in March's `CX2GUChung::InitComponent` and `InitPassiveSkillState`. June rewrote both: March reads none of June's `INIT_COMPONENT` keys, `SIEGE_ANGLE`, `MOBILE_SIEGE_ANGLE`, `X_EXTRA_CANNON_BALL_DAMAGE_RATE` or `COMMAND_HIT_ADD_MP_BONUS`. A graft would be a partial update.
  - Both are now rows in `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7, each naming the buff-side store the port must wire. That buff side (the 85/86 and 76 stores, `SetPassiveMultiple`, `GetHyperDurationPercent`) is already in.
  - R1's March edit stays (June-exact, gated) until the Add port replaces `X2GUAdd.cpp`.
- **R5, Info (new map rows).** `329e5140` is `CX2GUAdd::GetEffectiveIncreaseDPValue`, the HyperCharge sibling, which March already ports the same way (`X2GUAdd.cpp:9458`). Twelve `funcmap.tsv` rows were added ("Buff Phase 5 review"). Confirmed: `329e5140`, `329e5180`, `329e56c0`, `329e5b00` (`CX2GUAdd::UpNowDPValue`), `32c16be0`, `3236ccc0`, `3236cbf0`. Candidates: `32a08580` (Add formation-mode entry), `32bf4900` (March symbol not found), `322a0730` / `322a2700` (`CX2GameUnit` ctor/dtor) and `32c0c4e0` (`CX2GUChung` dtor).

**Build and deploy (R1, final placement).** `X2Lib` and `X2`, `US_SERVICE`, both rc=0. `X2GUAdd.cpp` recompiled; no header changed, so the PCH was not invalidated. `X2GUAdd.cpp` against the pre-review `98ce6f9`: 8/0, CRLF, ASCII, `file` unchanged. The flag-off unifdef equals `98ce6f9` and the flag-on strip differs. Deployed `X2/US_SERVICE/x2.exe` as `X2_offline.exe` (names read programmatically: `x2.exe`, `x2.exe.bak`, `x2.fx`, `X2_offline.exe`): 19,040,256 B, md5 **`9269dbc645699f894a0f1479bc3973e2`**, identical to the build output. It replaces `407067dc…` and the interim `63ed5dcb…`. **Not launched.**

**Play-test hand-off (R1 only; the four Phase 5 checks already passed).** This line is checked from source and has not been run. It is the same `JuneBuffTestNewStat_LUA` hook that the four Phase 5 checks ran successfully.

1. **Add's DP skills cost less with a hyper-duration buff (type 76).**
   - Where: a field or dungeon, **playing an Add character**, in Dynamo Configuration mode (the mode where Add's skills spend DP). Cast one DP-spending skill first and note how much of the DP bar it takes.
   - Paste:
     ```
     g_pX2Game:GetMyUnit():JuneBuffTestNewStat_LUA( 76, 1, 50, 1 )
     ```
   - What it does: lowers the DP each of Add's skills costs.
   - Look for: the same skill now takes about half as much DP as before (a little less than half if your gear or passives already lower DP cost). Entering Dynamo Configuration mode should also cost less, but a little less than half off (entry goes through the same DP step; March's own entry formula keeps its socket term, which June dropped, see R6). `ErrorLog.txt` gets `console hook BBT 76, change type 1, value 50, insert` and `first apply BBT 76, change type 1, value 50`.
   - If not: if the DP cost is unchanged but both log lines are there, tell me "DP cost unchanged". Before/after screenshots of the DP bar help. A popup or a crash: send the `[JUNE_BUFF]` lines.
   - Undo: `g_pX2Game:GetMyUnit():JuneBuffTestNewStat_LUA( 76, 1, 50, 0 )`
   - The value is 50, not Phase 5's 100, on purpose: +100 takes the DP cost rate to zero or below, so skills would cost nothing or even *give* DP. **If you have no Add character, this check cannot be reached:** the change only touches Add's DP code, and no console line makes another class use it. Say so and I will record it as unverified.

**Log lines to read afterwards:** `[JUNE_BUFF] console hook BBT 76 …` and `[JUNE_BUFF] first apply BBT 76 …`. The Add path has no debug line of its own; the DP bar is the check.

### Phase 5b — Done (2026-10-04), play-tested (user, logs checked)

**Play-test (2026-10-04, user, deployed exe `b981a4c1…`): all four console checks ran; `ErrorLog.txt` (read after the run) agrees and holds no runtime error, popup or `[JUNE_BUFF]` failure line other than the paste noise below.**
- **79:** `first apply BBT 79 (force down per second): interval 1, value 50`, then `first read BBT 79: force down gage 0 + 50 (interval 1)`. So the parse (change type 1, `FIX_VALUE`), the level slice (level 1 gave 50), the templet, the SetFactor and the tick all ran on a real buff. The `duplicate BTI 990079` and `ordinal reject BFI 362` lines are the definition pasted a second time in the same session (as the hand-off warned); the first definition was kept.
- **80–84:** `first SetFactor BBT 80..84 (by condition): change type 3, calculate type 0, v[3..5] = 1.06 0.35 1` (80, 81) and `1.06 0.07 1` (82, 83, 84), values 2.11 / 2.11 / 1.27 / 1.27 / 1.27, each followed by its `first apply`. This confirms the SGU appended the user count (the `1`), the value rule, and the stores for all five. The in-game max HP / damage change and the revert after 30 s were not reported back by the user; the logs show the stores received the right values.
- **93:** `first apply BBT 93, change type 1, value -20` (parse, ChangeStat slice, the `_HaveBuffOwner` class and the shared store of 42).
- **106 / 107:** both hook inserts logged, and `first read BBT 107: force down decrease 0 - 200` after a hit. The melee read (`first read BBT 106`) did not fire because the hit that landed was ranged; the two branches are one expression, so this is not a gap. No visible effect, as predicted (users take force down only in PvP).
- Not covered: the packet path (`GetFactor` / `SetFactorFromPacket`, 93's index bug) needs a second client; the stack/restack branches need an accumulating templet.

**Result.** All nine types are ported under `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT`; none is Opus-only. `buff_census.py --phase 5b` lists none of 79, 80–84, 93, 106 and 107 (it still lists 78, 88 and 92, Phase 5c). Code commit `9b79255`. `X2Lib` (PCH invalidated with `touch X2Lib/stdafx.cpp`) and `X2`, `US_SERVICE`, both rc=0; no new warning in the touched files. Deployed `X2/US_SERVICE/x2.exe` as `X2_offline.exe` (names read programmatically: `x2.exe`, `x2.exe.bak`, `x2.fx`, `X2_offline.exe`): 19,053,568 B, md5 **`b981a4c1ac9453422108325b8699a862`**, identical to the build output. It replaces `9269dbc6…` (the Phase 5 review exe). **Not launched.** The exe contains the nine `BBT_*` key strings (ASCII and UTF-16), `BUFF_VALUE_CALCULATE_TYPE`, and the debug strings `first apply BBT 79 (force down per second)`, `first read BBT 79: force down gage`, `first SetFactor BBT %d (by condition)` and `first read BBT %d: force down decrease`. The last one sits in `CX2GUUser::DamageReact` inside the flag, so it proves the reader compiled in.

**Tools.** Ghidra was read at `32d42650` and `32d42910` (the two parse functions; the doc gave their layouts, the decompile settled the failure paths). The tails that Ghidra cannot carve, `32e77f40`, `32290950`, `32271540`, `322718e0` and `32282bf2…c35` were read with `june2017_re/disasm_range.py` and `buffre/rawfn.py` (raw capstone, same binary). Five rows were added to `funcmap.tsv`. Nothing outside the doc's addresses was searched.

**Per type** (June members are `CX2GameUnit` offsets; the port uses named members and never an offset):

| Type | Factor parse | Level slice | SGU | Templet class | Store (basic) | Reader |
|---|---|---|---|---|---|---|
| 79 NOW_FORCE_DOWN_PER_SECOND | new `ParsingChangeNowForceDownPerSecondBehavior` (`32d42650`) | already in (4b-i, `32d478aa`) | none | new `CX2BuffChangeNowForceDownPerSecondBehaviorTemplet` (`32e86e90`) | none: the tick writes the unit's force-down gage | the class itself |
| 80 / 81 PHYSIC / MAGIC_ATTACK_BY_CONDITION | new `ParsingChangeStateByConditionBehavior` (`32d42910`) | already in (`32d48063`, whole `[ct, vct, n, v1..vn]`) | new `SetGameUnitChangeStateByConditionBehavior` (`32d4ce60`) | new `CX2BuffChangeStateByConditionBehaviorTemplet` (`32e86c00`) | store of 7 / 8, **main and range member** (shared case labels) | March's readers of 7 / 8 |
| 82 / 83 PHYSIC / MAGIC_DEFENCE_BY_CONDITION | same | same | same | same | store of 9 / 10 | March's readers of 9 / 10 |
| 84 MAX_HP_BY_CONDITION | same | same | same | same | max HP store; **own** insert and erase blocks (insert, refresh, `SetMaxHp`; no HP-ratio rescale), update shares 6 | March's readers of 6 |
| 93 EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER | `ParsingChangeStatBehavior` | already in (ChangeStat group) | joined the ChangeStat SGU group (`32d4d6ef`) | new `CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner` (`32e86740`), a ChangeStat subclass | the six attribute-defence stores of 42 (shared case labels) | March's readers of 42 |
| 106 / 107 FORCE_DOWN_MULTIPLIER_MELEE / RANGE_ATTACKED | `ParsingChangeStatBehavior` | default slice (above June's bound 105) | none | `CX2BuffChangeStatBehaviorTemplet` | new `m_AdditionalForceDownMultiplierMeleeAttacked` (+0x5b7c, insert `32282bf2`) and `…RangeAttacked` (+0x5c30, `32282c15`), basic 0.0 | `CX2GUUser::DamageReact`, users only |

All nine are in the factor-parse switch of **both** macro lists and in the templet factory (`CASE_BEHAVIOR_TEMPLET`). The new members of 106/107 got `InsertAdditionalStat` / `UpdateAdditionalStat` / `EraseAdditionalStat` cases, the plain refresh in `CheckAndUpdateAdditionalStat` (`32290950`), `SetBasicStatValue( 0 )` (`32271540`, `fldz`), `InitAdditionalPassiveMultipleAndSum` (`322718e0`) and the `Verify()` chain; a raw read of the three June functions confirms both members are in all of them. UDP layout: `GetFactor` / `SetFactorFromPacket` are in the classes (79 and 80–84 `[ m ]`, 93 below); 106/107 use ChangeStat's.

**What was settled by reading (and corrections to the map).**
- **79's parse.** A change type other than `BCT_FIX_VALUE` makes June return false at `32d4283b` with nothing pushed (`AL` = 0). The map's "pushes no factor" therefore means the factor is rejected. Ported as such. The change type is also checked in `SetFactor` (`32e855f0`), which is March's NowHp restack with regist 0.
- **80–84 value rule `32e77f40`.** It tests the class member `+0x14` (the templet's `BUFF_VALUE_CALCULATE_TYPE`), not the factor's `v[1]`; the result is `v[3] + v[4] * ( 4.0 - v[5] )` for `BVC_RALLY_OF_HERO`, else 0. Missing elements read 0. `v[5]` is the user count that the SGU appended.
- **80 / 81 second member.** Phase 4b-i part 2 ported it (store only), so 80 and 81 are wired through the same case labels as 7 and 8: main and range member, as June. On March data the range result stays 0 (only `BCT_PERCENT` data), and 80/81 are `BCT_PERCENT` in June's data as well.
- **`GT_PVE` halving of the user count.** March's `GAME_TYPE` stops at `GT_BATTLE_FIELD`, so the branch cannot run and is commented, not ported (the doc said to do exactly that). It comes with `GT_PVE`.
- **93's index bug, verbatim.** `GetFactor` writes `[ m, uid & 0xffff, ( uid >> 16 ) & 0xffff, ( uid >> 32 ) & 0xffff, uid >> 48 ]`; the packet inserts `m = v[0]` (ChangeStat's own) and rebuilds the owner from `v[4]..v[7]`. June reads past the end of a five-value factor; the port reads 0 there (`KBuffBehaviorFactor::operator[]` is range-safe). The owner aggregator `3227c620` is unreachable in June (nothing calls `CX2GUNPC` slot 183) and is not ported; the comment in `X2BuffBehaviorTemplet.h` says so.
- **106 / 107 reader.** Ported at the March line `fForceDownValue -= pDamageData->m_fDecreaseForceDown;` (`SERV_SKILL_NOTE` block), which stays reachable in the `#else`. `DECREASE_FORCE_DOWN` and `MELEE_ATTACK` are both parsed by March (`X2DamageManager.cpp:502`, `m_bMeleeAttack`), so nothing was missing. **Finding for the play-test:** `CX2GUUser::AddForceDownDelay` (the consumer of this value) only acts in **PvP** (`g_pX2Game->GetGameType() == GT_PVP`, both `BALANCE_PATCH_20110303` branches). In a field or dungeon the reader runs but changes nothing visible, so 106/107 can only be shown there by the log line.
- **Crash guards the port added** (June does not test): missing gage data in 79's tick and in 84's insert / erase (`NULL` test). Neither can fire on a normal unit.
- **Level slices** for all nine were already in from 4b-i and were checked, not added again. No new unit-store member besides 106/107.

**Behaviour on March data: none.** March's shipped data has no factor with any of these types, so the new code runs only when June data (Phase 13) or the console lines below put one in.

**Hygiene.** `git show --stat 9b79255`: 711 insertions, 1 deletion over eight files (706 insertions over the seven source files, plus 5 rows in `funcmap.tsv`). The one deletion is the last line of `X2BuffBehaviorTemplet.cpp` (`#endif //FIX_SKILL_BALANCE_AISHA_LENA`), which had no newline at the end of the file and got one when the new block was appended after it. `file` is unchanged on all seven files; all added lines are ASCII and CRLF. A flag-off unifdef (`SERV_IRUHADEV_JUNE_BUFF_NEW_STAT` off, so `DEBUG` blocks go with it) against `6f4f3d7` differs only in the `//{{ Iruha … //}}` marker comments, blank lines outside the guards, and that EOF newline: no code differs. Every replaced line (`X2GUUser.cpp`) keeps its original in the `#else`.

**Open items.**
1. **80 and 81 and the range members:** with June data the range result stays 0 because the data is `BCT_PERCENT`. Nothing to do; recorded so Phase 5c's 92 (which fills those members) knows 80/81 are already wired.
2. **106 / 107 and PvP.** If the user wants the effect shown, it needs a PvP match. Not required for the phase.
3. **Console reach for 79 and the real path.** The shipped March data has no factor with these types, so the lines below define a throw-away templet and factor from the console (numeric ids `990079`, `990080`, `990093`; BFI `362`, `363`, `364`). The loader reports a wrong ordinal in `ErrorLog.txt` (`ordinal reject BFI n: … the next id must be N`). These lines go through the real parse, level slice, SGU, templet, store and finish paths.

**How to see it in game (for Phase 14).** June numbers from `june_src_fixed/data036_kom/Enum.src.lua` and the factor file (BFI = ordinal). The data comes with Phase 13.

| Type | June BFI (BTI) | Where it comes from | What to watch |
|---|---|---|---|
| 79 | `BFI_DEBUFF_JUDGEMENT_FIRE` 292 (BTI 291; `TIME_INTERVAL` 0, `FIX_VALUE` −30, ended by `BFT_IMMEDIATE`), `BFI_DEBUFF_SHADOW` 547 (BTI 480; interval 1), `BFI_DEBUFF_DECREASE_FORCE_DOWN_IMMEDIATELY` 555 (BTI 486) | damage effects of June monsters and skills | the unit's force-down gage moves by the value on every tick; a negative value (−30) makes the target harder to knock down |
| 80–84 | `BFI_BUFF_RALLY_OF_HERO_LEVEL_1`/`_2`/`_3` 99 / 100 / 101 (BTI 118 / 119 / 120), `…_FOR_HERODUNGEON` 748 (BTI 722), `BFI_BUFF_RESERVATION_FOR_HERODUNGEON` 750 (BTI 724) | an item buff (a week long, `NORMAL_TIME` 604800) | level 1: attack ×( 1.06 + 0.35 × ( 4 − players ) ), defences and max HP ×( 1.06 + 0.07 × ( 4 − players ) ); with one player that is ×2.11 and ×1.27 |
| 93 | `BFI_DECREASE_ENEMY_RESIST` 787 (BTI 761, 10 s, `FIX_VALUE` −1…−30 by level), `BFI_DECENT_TO_SHADOW` 1118 (BTI 1037) | a monster skill (`CX2GUNPC` slot 183 is its only caller and is dead) | the six element defences drop by the level's value |
| 106 / 107 | `BFI_BUFF_PVP_ACCESSORY_SET_FORCE_DOWN` 1101 (BTI 1020, `FIX_VALUE` 0.6, 7 s), one factor carrying both | PvP accessory set | in PvP only: the force down of hits taken changes by the value |

**Play-test hand-off (dev console, END key; `settings.auth_level` = 3 is already set).** Every line below was **syntax-checked** with a Lua parser and, for the three definition lines, run against a mock manager to confirm the tables build and carry the ids named; none has run in the game. The factor numbers rely on `ErrorLog.txt`'s last load summary (`factors 361 loaded`, so the next free factor id is 362): if you changed or repacked the buff Lua since, the ids in the lines may need shifting by the same amount (the log will say). Paste the *definition* line once per game session; pasting it twice only logs a duplicate.

1. **Monsters' force-down gage is pushed up each second (type 79).**
   - Where: a field or dungeon with ordinary monsters on screen (not a boss).
   - Paste, first (defines a 20-second buff that adds 50 force down every second):
     ```
     local t = {} local v = {} local r = {} for i = 1, 30 do t[i] = 20 v[i] = 50 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990079, IS_WORLD_BUFF = FALSE, IS_DEBUF = TRUE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 79 }, BBT_CHANGE_NOW_FORCE_DOWN_PER_SECOND = { TIME_INTERVAL = 1 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 362, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990079, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 79 }, BBT_CHANGE_NOW_FORCE_DOWN_PER_SECOND = { BUFF_CHANGE_TYPE = 1, FIX_VALUE = v } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - Paste, second (puts it on every monster on the map):
     ```
     for i = 0, g_pX2Game:GetNPCUnitListSize() - 1 do local n = g_pX2Game:GetNPCUnit( i ) if n ~= nil then n:SetBuffFactorToGameUnitByBuffFactorID_LUA( 362, 1 ) end end
     ```
   - What it does: every monster's "about to fall over" meter fills by 50 each second for 20 seconds.
   - Look for: ordinary monsters are knocked down within a few seconds; `ErrorLog.txt` gets `first apply BBT 79 (force down per second): interval 1, value 50` and `first read BBT 79: force down gage … + 50 (interval 1)`.
   - If not: nothing falls and there is no `first apply` line. Look for a `[JUNE_BUFF] ordinal reject BFI 362 … the next id must be N` line (tell me N), or a `[JUNE_BUFF]` line naming `BBT_CHANGE_NOW_FORCE_DOWN_PER_SECOND` or `templet dropped`; send the lines. Monsters that resist knock-down (bosses) will not fall; the log lines still count.

2. **The five "by condition" stats on yourself (types 80–84).**
   - Where: any field or dungeon.
   - Paste, first (defines a 30-second buff with June's real rally-of-hero numbers):
     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 30 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990080, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 80, 81, 82, 83, 84 }, BBT_CHANGE_PHYSIC_ATTACK_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0 }, BBT_CHANGE_MAGIC_ATTACK_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0 }, BBT_CHANGE_PHYSIC_DEFENCE_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0 }, BBT_CHANGE_MAGIC_DEFENCE_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0 }, BBT_CHANGE_MAX_HP_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 363, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990080, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 80, 81, 82, 83, 84 }, BBT_CHANGE_PHYSIC_ATTACK_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0, VALUE = { 1.06, 0.35 } }, BBT_CHANGE_MAGIC_ATTACK_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0, VALUE = { 1.06, 0.35 } }, BBT_CHANGE_PHYSIC_DEFENCE_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0, VALUE = { 1.06, 0.07 } }, BBT_CHANGE_MAGIC_DEFENCE_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0, VALUE = { 1.06, 0.07 } }, BBT_CHANGE_MAX_HP_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0, VALUE = { 1.06, 0.07 } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - Paste, second:
     ```
     g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 363, 1 )
     ```
   - What it does: for 30 seconds gives you the Rally of Hero bonus: more attack, more defence and more max HP, larger the fewer players are in the game.
   - Look for: your max HP number rises by about a quarter (×1.27 when you are the only player; your current HP does not rise, so the bar looks emptier) and your hits deal about double (×2.11); after 30 seconds both go back. `ErrorLog.txt` gets one `first SetFactor BBT 80 (by condition): change type 3, calculate type 0, v[3..5] = 1.06 0.35 1, value 2.11` line for each of 80 to 84 (the last number of `v[3..5]` is the player count the game saw; 84 shows `1.06 0.07 1, value 1.27`), then `first apply BBT 80 … 84`.
   - If not: no `first SetFactor` line means the factor was not applied (a `[JUNE_BUFF] ordinal reject BFI 363 … must be N` line says why; if 362 was rejected earlier, 363 shifts by one too). If the line shows `v[3..5] = 0 0 0`, the VALUE table was not read, tell me. A popup or crash: send the `[JUNE_BUFF]` lines.
   - Undo: wait 30 seconds; nothing else to do.

3. **Attribute defences drop, owner subclass (type 93).**
   - Where: any field or dungeon.
   - Paste, first:
     ```
     local t = {} local v = {} local r = {} for i = 1, 30 do t[i] = 20 v[i] = -20 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990093, IS_WORLD_BUFF = FALSE, IS_DEBUF = TRUE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 93 }, BBT_CHANGE_EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER = { BUFF_CHANGE_TYPE = 1, BUFF_RELATION_TYPE = 7 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 364, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990093, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 93 }, BBT_CHANGE_EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER = { BUFF_CHANGE_TYPE = 1, BUFF_RELATION_TYPE = 7, FIX_VALUE = v } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - Paste, second:
     ```
     g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 364, 1 )
     ```
   - What it does: for 20 seconds lowers all six element defences of your character by 20.
   - Look for: nothing you can see in play unless your status window lists element defences (then each shows 20 lower for 20 seconds). `ErrorLog.txt` gets `first apply BBT 93, change type 1, value -20` (every `FIX_VALUE` entry is −20 here, so level 1 takes −20).
   - If not: no `first apply BBT 93` line: look for `ordinal reject BFI 364 … must be N` (tell me N) or a `[JUNE_BUFF]` line naming `BBT_CHANGE_EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER`; send it.

4. **Force-down multipliers, read site (types 106 and 107).**
   - Where: a field or dungeon, next to a monster that hits you.
   - Paste (puts 200 on both stores through the Phase 5 console hook):
     ```
     local u = g_pX2Game:GetMyUnit() u:JuneBuffTestNewStat_LUA( 106, 1, 200, 1 ) u:JuneBuffTestNewStat_LUA( 107, 1, 200, 1 )
     ```
   - What it does: gives you a large "force down taken" bonus, melee and ranged, so the new read site has something to report.
   - Look for: **nothing visible** in a field or dungeon (the game applies the force down of hits taken to a player only in PvP). Let a monster hit you once; `ErrorLog.txt` gets `console hook BBT 106 …`, `first apply BBT 106, change type 1, value 200` (and 107), and after the hit `first read BBT 106: force down decrease 0 - 200` (or `107` for a ranged hit).
   - If not: no `first read` line after being hit means the read site did not run; tell me "no first read". The force-down effect itself can only be seen in a PvP match, which is not needed here.
   - Undo:
     ```
     local u = g_pX2Game:GetMyUnit() u:JuneBuffTestNewStat_LUA( 106, 1, 200, 0 ) u:JuneBuffTestNewStat_LUA( 107, 1, 200, 0 )
     ```

**Log lines to read afterwards** (this phase's own, `ErrorLog.txt` in the game directory): `[JUNE_BUFF] first apply BBT 79 / 80–84 / 93 / 106 / 107 …`, `first SetFactor BBT 80–84 …`, `first read BBT 79 …`, `first read BBT 106/107 …`, and any `[JUNE_BUFF]` line naming one of the nine types or the ids `362`, `363`, `364`. Nothing else is in scope.

### Phase 5b — Review (2026-10-04, Opus 5.5 high)

Independent review of code commit `9b79255` (against its parent `6f4f3d7`) and doc commits `7235480` and `bd44d6b`.
- Ghidra was used read-only (a health call only). Most of the addresses are not carved into functions, so their bodies were read with `june2017_re/disasm_range.py` (raw capstone on the same binary). Jump tables and constants were decoded straight from the PE.
- Hygiene was re-run with fresh tools: a byte-level unifdef, `file` at both commits, and `git diff --numstat`.

**Verdict: the transcription is correct. No code changed.** All three spot-checks, the SGU and level-slice jump tables, and 79's tick match June's asm. One claim about 93's packet bug is wrong in the doc and in one code comment (D1); the code itself is right. The exe deployed in 5b (`b981a4c1…`) stays.

| # | Check | Result |
|---|---|---|
| 1 | Enum values | **Pass.** These three agree: C++ `X2BuffFactor.h:117-122/131/144-145` with `BVC_RALLY_OF_HERO = 0`; June `Enum.src.lua` (the BBT block, and `BUFF_VALUE_CALCULATE_TYPE` at `:11906`); and the staged March `Enum.src.lua` (`:8360-8387`, `:10859`). |
| 2 | Parse and factory | **Pass.** All nine types are in both macro lists and in the factory. 93, 106 and 107 use `ParsingChangeStatBehavior`; 79 and 80–84 use their own parsers. The non-skill list is the `#else` of `UPGRADE_SKILL_SYSTEM_2013`, so it is unreachable in this build. |
| 3 | SGU (`32d4d670`; table `32d4d78c` → `32d4d774`, index type−1) | **Pass.** 76, 78, 92 and 93 go to ChangeStat `32d41700` (with 6–21, 36–42, 44, 52 and 72); 80–84 go to `32d4ce60`; 79 goes to default. 106 and 107 are above the bound (`0x5c`) and also go to default. The port's two SGU edits are exactly this. |
| 4 | Level slice (`32d47620`; table `32d48264` → `32d4822c`) | **Pass.** 79 → `32d478aa`; 80–84 → `32d48063`; 93 → the ChangeStat group `32d47957`. 106 and 107 are above June's bound (type−1 > `0x68`), so they fall to default `32d4812a`. The port's `GetBehaviorFactorToLevel` routes all nine the same way. |
| 5 | **Spot-check: 93** `GetFactor` `32e7d7f0`, packet `32e76710` | **Pass, with doc correction D1.** `GetFactor` pushes m (`+0x18`), then `uid & 0xffff`, `(uid >> 16) & 0xffff` (`shrd`), `(uid >> 32) & 0xffff` (`32fe3030`), and `movzx word [+0x26]`, which is an unsigned `uid >> 48`. The packet inserts `m = v[0]` through `32282700`. It then reads v[4], v[5], v[6] and v[7], **each behind its own size test** (`cmp size, 4/5/6/7`, `fldz` when the element is absent). It converts each with `32fe2626` (`__ftol2`, float to int64) and stores `v4 | v5<<16 | v6<<32 | v7<<48` at `+0x20`/`+0x24`. The port does the same; its `int` conversion is equal for 16-bit words. |
| 6 | **Spot-check: 80–84** `GetValue` `32e77f40`, `SetFactor` `32e78860` | **Pass.** See "80–84 `SetFactor` in detail" below. |
| 7 | **Spot-check: 106/107** reader `3204a118`–`3204a243` | **Pass, term for term.** `dec = DD+0x514`. If the defender `optr` at `DD+0x470` is set (`31e55680`), `dec −= DD+0x2c ? 32071bb0 (+0x5c1c, 106) : 32071bd0 (+0x5cd0, 107)`. Then `fForceDown = DD+0x3f0 − dec`, followed by the penetrate (`DD+0x4c5`) clamp. |
| 8 | 79 tick `32e89800` | **Pass.** `if ( sum(+0x18) >= target(+0x14) ) { sum = 0; gage(+0xc04)+0x5c.Increase( m, 0 ) }`. This equals March's `CheckAndResetElapsedTime` (`KTDXTimer.h:47-52`) plus `UpNowForceDown` (`X2GageData.h:354`). |
| 9 | Stores | **Pass.** See "Stores in detail" below. |
| 10 | Hygiene | **Pass.** See "Hygiene in detail" below. |

**80–84 `SetFactor` in detail (check 6).**
- `GetValue`'s gate is the class member `+0x14 == 0`, i.e. the templet's calculate type. The Done note's correction of the map is right.
- Every element is size-tested before it is read. The result is `( 4.0 − v5 ) · v4 + v3`, with 4.0 at `33437838`.
- `SetFactor`:
  - It returns false when `GetBehaviorFactor` fails, when `ftol( v[0] ) != +0x10`, or when `+0x10 == 2` (`BCT_RELATION_VALUE`).
  - The regist is computed as resist / 500.0 (`33458fa8`) and passed, but never used.
  - A zero multiplier returns false (`31c7b640`, `IsSamef`).
  - PERCENT: `m = 1 + ( fNew − 1 )·( c + 1 )`, then `m < 0 → 0` with no popup.
  - FIX: `c ≤ 1 ? m : m / ( 1 + mul·( c − 1 ) )`, using 1.0 from `334355b0`. The larger magnitude is kept.
- The port matches. Its extra `DISPLAY_ERROR`s go through the 4b-ii redirect, which writes one `[JUNE_BUFF] runtime error` line and raises no popup. So June's "no popups" holds.

**Stores in detail (check 9).**
- 80 and 81 share the case labels of 7 and 8 in insert, update and erase. In all three, the shared body also writes the range member (under `BEHAVIOR_PARITY`); June's update gives 80 and 81 their own blocks (`322708d8`, `32270947`) of the same shape.
- 82 and 83 share the stores of 9 and 10.
- 84 has its own insert and erase blocks, and shares 6's update. In June, case 84 of update `3226fd70` (index type−6) is case 6's block `3226fd94`: update-or-insert on `+0x42d0`, then return, with no `SetMaxHp` (see R3).
- 93 shares all three of 42's blocks.
- 106 and 107 are in insert, update, erase, refresh, basic value (0.0), passive and `Verify`.

**Hygiene in detail (check 10).**
- `numstat 6f4f3d7 9b79255`: 303/1, 110/0, 117/0, 15/0, 22/0, 133/0 and 6/0 over the seven source files, plus 5/0 in `funcmap.tsv`.
- `file` gives the same answer at both commits for all seven source files: CP949 (`ISO-8859` or `Non-ISO extended-ASCII`) with CRLF. The two headers' "CRLF, LF" was already there. HEAD equals `9b79255` on all seven.
- **The one deletion is the old last line regaining its line end.** A line-level diff of `X2BuffBehaviorTemplet.cpp` gives exactly one `replace`: the old last line, `#endif //FIX_SKILL_BALANCE_AISHA_LENA` with no line end, becomes the same bytes plus `\r\n`, followed by the appended block. No byte was dropped.
- **Flag-off strip** (`NEW_STAT` and `DEBUG` undefined, applied to both sides):
  - `X2GUUser.cpp` and `X2GameUnit.h` are byte-identical to the parent.
  - The other five differ only by blank lines, the `//{{ Iruha … //}}` markers and that EOF newline.
  - The wrong-flag control (`TEMPLET_PARITY`) differs on every file.
  - The replaced line in `X2GUUser.cpp` is in the `#else`.

**Findings**
- **D1, doc correction (93's packet bug): June does not read past the end of the factor.**
  - Each of v[4..7] is size-tested and reads 0 when it is absent, exactly like the port's `KBuffBehaviorFactor::operator[]` (`CommonPacket.h:1234`).
  - With `GetFactor`'s five values, a peer gets owner = `uid >> 48` (the top word alone), not "garbage".
  - The wrong wording is in three places: item 3 ("peers get a garbage owner"), the 5b Done note ("June reads past the end; the port reads 0 there", which is right about the port but not about June), and the comment at `X2BuffBehaviorTemplet.cpp:4524`. The doc lines stay as written, so the history is readable, and this note supersedes them.
  - The comment is not edited now: a comment-only change would re-link a new exe for no behaviour change (the 4b-i part 3 review's R1 precedent). Correct it with the next code edit to that file. `funcmap.tsv` has the corrected row.
- **R1, Info (flag dependency).** Two pieces of 5b's behaviour sit under `BEHAVIOR_PARITY`, not `NEW_STAT`: 80/81's range-member writes, and the call that gives 93 its owner (ChangeStat `SetFactor`'s `SetBuffOwnerUID`, `X2BuffBehaviorTemplet.cpp:1051-1054`).
  - `Always.h`'s `NEW_STAT` comment already says it requires `BEHAVIOR_PARITY`, but nothing enforces that. With `NEW_STAT` on and `BEHAVIOR_PARITY` off, the build still compiles: 80/81 write the main member only, and 93 sends owner 0.
  - Both flags are on today. **User's call:** add an `#error` pair like the `DAMAGEEFFECT` one (`Always.h:4421-4422`). Not done.
- **R2, Info (the templet and the factor can disagree).**
  - The SGU tests the factor's `v[1]`; `GetValue` tests the templet's member.
  - If a templet says `BVC_RALLY_OF_HERO` (0) and its factor says something else, no user count is appended and the value is `v3 + 4·v4`.
  - This is June's behaviour, transcribed. June's data uses 0 on both sides, so it cannot happen with June data.
- **R3, Info (a second stack of 84 does not move the gage's max HP).**
  - 84's update shares case 6, which never calls `SetMaxHp`. So a second stack changes the store's result, but the HP bar keeps the first stack's max HP until the next max-HP insert or erase.
  - This was read in June above (`3226fd94`), so it is June's behaviour, not a port gap. The play-test below shows it.
- **Not covered:**
  - The packet path at runtime needs a second client. The Ghidra read above is the only evidence for 93's layout.
  - The default level slice that 106/107 use (ported wholesale in 4b-i) was not re-read in asm. Check 1 below tests it with a level-dependent value.

**`funcmap.tsv`:** three rows were added ("Buff Phase 5b review"): `32e76710` (the range tests and the corrected owner), `32fe2626` (`__ftol2`, candidate), and `3226fd94` (update case 6/84, no `SetMaxHp`).

**Play-test result (2026-10-04, user; exe `b981a4c1…`; `ErrorLog.txt` session from the 15:25:43 load summary, `factors 361 loaded`). All checks pass.**
- **Setup:** both definitions were accepted. There is no `ordinal reject` line and no `[JUNE_BUFF]` line naming `990106`, `990081`, `362` or `363`.
- **Check 1 (106/107 through a real factor): pass.**
  - `first apply BBT 106, change type 1, value 101` and `first apply BBT 107, change type 1, value 201` prove the real factor path end to end: both macro-list parses, ChangeStat `SetFactor` and the stores. The default level slice picked level 1 out of a table that differs at every level.
  - After a ranged hit (a Mag Charger fireball, per the `JUNE_FX` lines beside it): `first read BBT 107: force down decrease 0 - 201`.
  - The melee branch (`first read BBT 106`) did not fire because no melee hit landed, the same as in 5b. It is the other arm of one expression, verified in asm (check 7), so it is not a gap.
- **Check 2 (80/81/84):**
  - The first paste logged `first SetFactor` / `first apply` for 80 and 81 (`v[3..5] = 1.06 0.35 1, value 2.11`) and for 84 (`1.06 0.07 1, value 1.27`).
  - No `runtime error` line for BTI `990081` follows. So if the second paste ran, the restack raised no zero-multiplier or mismatch error.
  - The second stack and the expiry write no log line, so the user checked them on screen. **The user reports both exactly as predicted:**
    - Damage rose again on the second paste, to about 3× the original. That is the PERCENT restack `1 + ( fNew − 1 )·( c + 1 )` through 80/81's update blocks.
    - Max HP returned to normal about 20 s after the second paste. That is 84's own erase block plus `SetMaxHp`, and `BFT_TIME` restarting on the re-apply.
  - **Check 2: pass.**
- **The Phase 5b review is complete.** Every check passed. Nothing is owed except D1's comment fix (carried into 5c's scope) and R1 (the user's call). Phase 5c is next.
- Out of scope and unchanged from earlier sessions: the `JUNE_FX MP_CRASH` lines (effect port), and the missing-texture and missing-control lines.

**Play-test hand-off (review).** These checks cover what 5b's four checks did not reach:
- **106/107 through a real factor.** 5b's check 4 used the console hook, which writes straight into the store and skips the factor parse, the default level slice, the factory and ChangeStat's `SetFactor`.
- **A second stack of 80/81/84** (the accumulation branch).
- **84's expiry** (its own erase block), which was not reported back.

How the lines were checked:
- **Syntax:** checked with `elstools`' `luajit.exe`.
- **Tables:** the two definition lines were run against a mock manager, which confirmed the tables and ids they build.
- **Not run in the game:** none of the lines has been run there yet. Each is the same shape as the 5b lines that did run, and each is shorter than the longest of those (1,125 and 1,310 characters, against 1,740).
- **Ids:** the factor ids assume a **fresh launch**. The last load logged `factors 361 loaded`, so the next free id is 362. If you already pasted 5b's definitions in this session, restart the game first; otherwise both ids are rejected (`ordinal reject BFI 362 … the next id must be N`).

0. **Setup (once, right after loading in).**
   - Where: any field or dungeon, after a fresh launch of the game.
   - Paste, first (defines the force-down test buff, factor 362):
     ```
     local t = {} local r = {} local a = {} local b = {} for i = 1, 30 do t[i] = 60 r[i] = 1 a[i] = 100 + i b[i] = 200 + i end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990106, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 106, 107 }, BBT_CHANGE_FORCE_DOWN_MULTIPLIER_MELEE_ATTACKED = { BUFF_CHANGE_TYPE = 1, BUFF_RELATION_TYPE = 7 }, BBT_CHANGE_FORCE_DOWN_MULTIPLIER_RANGE_ATTACKED = { BUFF_CHANGE_TYPE = 1, BUFF_RELATION_TYPE = 7 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 362, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990106, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 106, 107 }, BBT_CHANGE_FORCE_DOWN_MULTIPLIER_MELEE_ATTACKED = { BUFF_CHANGE_TYPE = 1, BUFF_RELATION_TYPE = 7, FIX_VALUE = a }, BBT_CHANGE_FORCE_DOWN_MULTIPLIER_RANGE_ATTACKED = { BUFF_CHANGE_TYPE = 1, BUFF_RELATION_TYPE = 7, FIX_VALUE = b } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - Paste, second (defines the stacking rally buff, factor 363):
     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 20 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 2, ACCUMULATION_MULTIPLIER = 1, BUFF_TEMPLET_ID = 990081, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 80, 81, 84 }, BBT_CHANGE_PHYSIC_ATTACK_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0 }, BBT_CHANGE_MAGIC_ATTACK_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0 }, BBT_CHANGE_MAX_HP_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 363, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990081, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 80, 81, 84 }, BBT_CHANGE_PHYSIC_ATTACK_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0, VALUE = { 1.06, 0.35 } }, BBT_CHANGE_MAGIC_ATTACK_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0, VALUE = { 1.06, 0.35 } }, BBT_CHANGE_MAX_HP_BY_CONDITION = { BUFF_CHANGE_TYPE = 3, BUFF_VALUE_CALCULATE_TYPE = 0, VALUE = { 1.06, 0.07 } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - What it does: teaches the game two test buffs; nothing changes on screen yet.
   - Look for: no popup. `ErrorLog.txt` gets no `ordinal reject BFI 362` or `363` line.
   - If not: an `ordinal reject … the next id must be N` line means the ids are off. Tell me N, or restart the game and paste the two lines again.

1. **Force-down bonus through a real buff (types 106 and 107).**
   - Where: a field or dungeon, next to a monster that hits you up close, ideally one that also shoots.
   - Paste:
     ```
     g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 362, 1 )
     ```
   - What it does: for 60 seconds, gives you two different "force down taken" values: 101 for melee hits and 201 for ranged hits.
   - Look for:
     - Nothing visible; this only matters in PvP.
     - `ErrorLog.txt` gets `first apply BBT 106, change type 1, value 101` and `first apply BBT 107, change type 1, value 201`. The 101/201 prove the buff's level 1 was picked from a table that is different at every level.
     - After you are hit: `first read BBT 106: force down decrease 0 - 101` for a melee hit, or `first read BBT 107: … - 201` for a ranged hit. Get hit by both kinds if you can.
   - If not:
     - Values other than 101/201: send me the two lines.
     - No `first apply` line: send any `[JUNE_BUFF]` line naming BTI `990106`, BFI `362`, or one of the two `BBT_CHANGE_FORCE_DOWN_MULTIPLIER_…` names.
     - No `first read` line after a hit: tell me "no first read" and what hit you.

2. **The rally buff stacks, then wears off (types 80, 81 and 84).**
   - Where: any field or dungeon, on your own (one player), next to a monster you can hit safely. Before pasting:
     - Note your **max HP** number.
     - Hit the monster a few times with the same basic attack and note the **typical damage number**.
   - Paste (once now, then a second time a few seconds later):
     ```
     g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 363, 1 )
     ```
   - What it does: the first paste gives the Rally of Hero bonus; the second stacks it once more. The buff runs out 20 seconds after your last paste. *(Checked in source: on a re-apply, `CX2BuffTimeFinalizerTemplet::SetFactor`, `BDTT_NORMAL_TIME`, sets the time left to the larger of the time left and 20 s, so the second paste restarts the 20 seconds.)*
   - Look for:
     - **After the first paste:** max HP rises to about 1.27 times what you noted, and the same attack hits for about 2.1 times as much.
     - **After the second paste:** the attack hits for about 3.2 times the original. Max HP does **not** change again; that is June's behaviour (R3).
     - **About 20 seconds after the second paste:** both go back to what you noted.
     - `ErrorLog.txt` gets `first SetFactor BBT 80 (by condition): … v[3..5] = 1.06 0.35 1, value 2.11` (the same for 81), and `… BBT 84 … 1.06 0.07 1, value 1.27`. These come from the first paste only; the second stack writes no log line, so the damage numbers are the check.
   - If not:
     - Damage did not rise on the second paste: tell me "no stack".
     - Max HP did not return after the buff ended: tell me "max HP stuck" with the before/after numbers.
     - A `[JUNE_BUFF] runtime error` line naming BTI `990081`: send it.
     - Screenshots of the damage numbers help.

**Log lines to read afterwards** (this review's checks only, `ErrorLog.txt`):
- `[JUNE_BUFF] first apply BBT 106` / `107`
- `first read BBT 106` / `107`
- `first SetFactor BBT 80` / `81` / `84` and their `first apply` lines
- any `[JUNE_BUFF]` line naming `990106`, `990081`, `362` or `363`

### Phase 5c — Done (2026-10-04): 78 and 88 ported, 92 deferred

Ran on Opus 5.5 high, under `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT`. 78 and 88 are ported, with their June host rewrites. **92 is deferred.** It needs three decisions from the user, listed under "92 — deferred" below.

**Result.**
- **Census.** `buff_census.py --phase 5b` lists only 92 (`BBT_CHANGE_MAXIMIZE`, 20 templets and 21 factors), the deferred type. March now handles 90 behavior types; it was 88. `IGNORE_DEFENCE_RATE` has left the unread-key list.
- **Build.** `X2Lib` (with `stdafx.cpp` touched first, since four headers changed) and `X2`, both at `US_SERVICE`: rc 0, 0 errors. The only warning in a touched file is the existing C4018 in `X2GUUser.cpp` (line 17966 now). `KTDXLIB` was not rebuilt: `Always.h` is unchanged.
- **Deploy.** `X2/US_SERVICE/x2.exe`, 19,058,176 B, linked 16:14:07, md5 **`d555b21c8cce59c80e85480195096d0a`**, copied to the game directory as `X2_offline.exe`. The names were read programmatically (`x2.exe`, `x2.exe.bak`, `x2.fx`, `X2_offline.exe`), and the copy is byte-identical to the build. It replaces `b981a4c1…`. The exe contains `BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER` and `BBT_IGNORE_DEFENCE` (ASCII and UTF-16), the four `IGNORE_DEFENCE_*` keys, and the debug strings `first apply BBT 88 (ignore defence)`, `first read BBT 88: piercing defence factor`, `first read BBT 78 (user hit)` and `first read BBT 78 (npc hit)`. The last three sit inside the host blocks, so they prove the hosts compiled in. As expected, it has no `BBT_CHANGE_MAXIMIZE` string.
- **Not launched.**

**Flags.** Everything sits under `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT`. The debug lines also need `SERV_IRUHADEV_JUNE_BUFF_DEBUG`. `Always.h` is unchanged. Every replaced March line stays in its `#else`.

**78 `BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER`.**

Where each piece sits:

| Piece | June | March |
|---|---|---|
| Factor parse | `ParsingChangeStatBehavior` (`32d445d0`) | both macro lists |
| Level slice | ChangeStat group (`32d47957`) | already in (4b-i); checked, not added |
| SGU | ChangeStat group (`32d4d6ef`) | case added |
| Templet class | `CX2BuffChangeStatBehaviorTemplet` | factory case added |
| Store | member `+0x53b0`; insert `32282dff`, update `@322708a9`, erase `@32283685` | new member `m_AdditionalTypeAdditionalAttackMultiplier`, getter `GetTypeAdditionalAttackMultiplier()` |
| Basic and passive | basic 1.0 (`@32271648`, `fld1`); passive multiple 1, sum 0 (`@32271b26`) | the same |
| Refresh | plain result update (`@32290bc6`) | the same |
| Option value | 0 in both branches of `32270dd0` | `SetOptionValue( 0 )` |
| `Verify` chain | – | added, as for its siblings |

**The host is the additional-attack block of both `DamageReact`s.**
- User: `@32049405`–`32049561`. NPC: `@327b4214`–`327b43be`.
- 78 is read on **the unit taking the hit**, not on the attacker. It multiplies the attacker's additional-attack damage (vslot 149) by `k`.
- One block up, June gates the additional-defence reduction on `!IGNORE_DEFENCE_DECLINE` (DamageData `+0x534`; user `@32049412`, NPC `@327b4221`). Both gates are ported. The key itself is 88's (below).

**Recorded deviation (allowed by the kickoff).**
- June's `k` is `attacker->Opt(+0x5248) + this->78 + this->Opt(+0x5194)`.
  - The user block then subtracts `defender->Opt(+0x5ac8) × 0.01` when that option is above 0.
  - The NPC block subtracts it whenever a defender exists (no `> 0` test).
  - `k` is then clamped at 0.
- The three `Opt` terms are option values. June seeds them from socket-data fields March's socket data lacks (`32270dd0`: `+0xcc`, `+0xc8`, `+0xd4`).
- So the port uses `k = max( 78, 0 )`. This is exact on March data. With no buff, `k` = 1.0, which is March's behaviour.

**What June's data does with 78.** All three of June's 78 factors are `PERCENT` reductions of additional damage taken:
- `BFI_BUFF_DEVIL_CHAIN_BALL` 553 (BTI 484): 0.0001, a monster made nearly immune to additional damage.
- `BFI_BUFF_ELDER_SECRET_SET_NORMAL` 567 (BTI 494): 0.5.
- `BFI_BUFF_ELDER_SECRET_SET_UPGRADE` 568 (BTI 494): 0.0001 at level 1.

**88 `BBT_IGNORE_DEFENCE`.**

Where each piece sits:

| Piece | June | March |
|---|---|---|
| Factor parse | `32d49210`: `[ IGNORE_DEFENCE_RATE ]`, 1.0 when the key is absent; no change type, no count | new `ParsingIgnoreDefenceBehavior`, in both macro lists |
| Level slice | copy whole | already in; checked |
| SGU | none | none |
| Templet class | vt `33535558` | new `CX2BuffIgnoreDefenceBehaviorTemplet` |

Class slots, all from June:
- `SetFactor` `32e7d260`: on the first apply only, rate = `v[0]` (0 when the factor is empty), then the insert. A restack changes nothing.
- Packet `32e7d3f0`: the same insert, with no first-apply test.
- `GetFactor` `32e7ec40`: `[ rate ]`.
- `DoFinish` `32e75b00`: erase.
- A `GetBehaviorFactor` failure returns false with no popup, as June does.

**Store.**
- New `m_vecIgnoreDefencePtr`, a `vector<CX2StatFactorByBuffPtr>`. March's `CX2StatFactorByBuff` has the same shape as June's 0x1c element (`322afc00`: identity plus a protected rate).
- `InsertIgnoreDefence` (`32282390`) pushes with no duplicate test.
- `EraseIgnoreDefence` (`3227a8c0`) removes **the first match only**:
  - `322a9090` is a `find_if` on BTI and unique number, and `3207b700` is called here as a single-element erase.
  - That is March's `FindSameBuffIdentity` plus `erase`.
  - Phase 1c item 2 called `3207b700` a range erase; that is corrected in `funcmap.tsv`.
- `GetIgnoreDefenceRate` (`32274970`) returns the sum of the rates, capped at 1.0.

**Host: June's rewrite of `fPiercingDefenceFactor`** (`GetFinalDamage` `@3228568a`–`32285735`), ported whole:
```
bIgnore = GT_PVP game ? IGNORE_DEFENCE_ON_PVP : IGNORE_DEFENCE;
if ( penetrate || bIgnore ) factor = 0;
else factor = max( 1 - this->GetIgnoreDefenceRate() - ( IsPvpOrPve ? IGNORE_DEFENCE_RATE_ON_PVP : IGNORE_DEFENCE_RATE ), 0 );
```
- `this` is the defender (June calls `32274970` on `ECX`).
- **`IsPvpOrPve`** (`322cc1a0`) is game type 1 or 4. March's `GAME_TYPE` has no `GT_PVE` (4), so in this build it can only match `GT_PVP`. That half is commented, the same way 5b handled `GT_PVE`. It is not a design stop.
- The frozen-debuff resist scaling just above the block is **not** ported, per the scope: it is not a buff change.

**The four DamageData keys.**
- New members `m_bIgnoreDefenceOnPvp`, `m_fIgnoreDefenceRate`, `m_fIgnoreDefenceRateOnPvp` and `m_bIgnoreDefenceDecline`, with `SimpleInit` defaults. `DamageData` copies memberwise, so nothing else needed changing.
- They are parsed in `SetDamageDataFromLUA` right after `IGNORE_DEFENCE`, in June's order (`32632390` stores: `@3263c863`, `@3263ca14`, `@3263cc40`, `@3263ce2b`).
- **Defaults (verified in the asm).**
  - Each `_ON_PVP` key takes its sibling's value when absent (`@3263c8a0` copies `+0x528`; `@3263cc7e` copies `+0x52c`). The port passes the already-read sibling as the `LUA_GET_VALUE` default; that macro assigns the default only on a miss, so the behaviour is the same.
  - `IGNORE_DEFENCE_RATE` defaults to 0.
  - `IGNORE_DEFENCE_DECLINE` defaults to false.
- **Cross-plan note.** `JUNE2017_EFFECT_KEY_MATRIX.md` listed all four keys as "to-implement, 8b". This phase implements them, and the four matrix rows now say so (follow-up commit), so the effect plan's 8b must not parse them again.

**March's runtime writers of `m_bIgnoreDefence`, checked against June.** PvP now reads the `_ON_PVP` twin, so each writer was checked for whether June also sets it:
- **Rena, Powerful Bowstring** (`X2GULire_ElvenRanger.cpp`, two branches): June skill 23405 sets both bytes (`@32cada25`: `mov word [+0x528], 0x101`). The port now sets the twin in both March branches.
- **Phantom Sword third blade, memo 26** (`X2BuffTemplet.cpp`): June's branch (`@323710c6`, memo `600002`) writes `+0x528` and the 0.1 critical-damage field only. March's line is unchanged, which matches June.
- **Add** (`X2GUAdd.cpp:10777`): June (`329e27e2`, `329e4aa0`) writes `+0x528` (and `+0x541`) only. Unchanged, which matches June.
- **Consequence:** in a PvP match, Add's ignore-defence and the memo-26 third blade's no longer ignore defence. That is June's behaviour.

**92 `BBT_CHANGE_MAXIMIZE` — deferred. Three decisions for the user.**

The kickoff's two blockers do not fire: Phase 4b-i part 4 landed the second ("range") physic/magic members, and March's `GetFinalDamage` has no variance roll. Tracing the rest found three further dependencies. The store's value depends on the first of them, so 92 is not wired at all: no parse, no factory, no store. A half-wired 92 would have failed into "Behavior Type Invalid" and shown up in the census as handled.

**Q1. June rewrote `CX2AdditionalStatByBuff::CheckAndUpdateResultValueWithOptionData` (`320839e0`), a shared host function. Port it?**
- 92's member refreshes through the option-data variant (`@32290ec2`), not the plain one.
- June's formula, checked in the asm:
  - `base' = basic + 1` when `basic ≤ 0`, otherwise `basic`.
  - `c = curve( option + FIX + RELATION, level )`; `c *= modifier` when `PERCENT ≥ 1`.
  - `t = base'·(1 + c) − [basic ≤ 0]`.
  - `P = (passiveSum + 1)·passiveMultiple·PERCENT`; when `P > 0`, `t *= ( PERCENT < 1 ? P : (P − 1)·modifier + 1 )`.
  - The result is `max( t, 0 )`. A SWAP factor overrides all of this.
- March (`X2BuffTemplet.h`, `UpdateResultValueWithOptionData`) subtracts the `1` *after* multiplying by the passive/PERCENT product, and applies the modifier to the whole product (under `ADJUST_BUFF_CALCULATE`, which `Always.h:161` defines).
- **Where the two formulas disagree on March's option-data members:**
  - the basic-0 members, whenever a `PERCENT` factor ≠ 1 (or a passive) is present: critical rate, additional attack, additional defence and hyper charge;
  - the 0.5-modifier members (walk, dash, jump and dash-jump), whenever a socket bonus and a `PERCENT > 1` factor are both present (for example 1.155 against 1.16).
  - Everywhere else the two agree.
- **For 92 itself:** under June's formula, a `PERCENT`-only factor on the basic-0 member gives **0**. That covers 18 of June's 21 factors, so they are inert unless a `FIX` or option term is present. Under March's formula they would give `PERCENT − 1`.
- **Options:**
  - (a) Port June's template wholesale. This is a `BEHAVIOR_PARITY` item, outside 5c, and it changes the members listed above.
  - (b) Use June's formula for 92 only. That is a partial update of a function June changed.
  - (c) Leave March's formula. 92 then gives values June never would.
- **Recommendation: (a).** The standing rule is no partial June updates: when June changed a function March has, port June's whole version. So the real question is whether you accept its reach on March's existing option-data members, listed above. On March's shipped data that reach is small: every disagreement needs a `PERCENT` factor ≠ 1 on a basic-0 member, or a socket bonus together with `PERCENT > 1` on a move/jump member. It would land under `BEHAVIOR_PARITY` with a census of the March factors it changes. 92 would then follow, once Q2 and Q3 are settled.

**Q2. 92's option curve is a Lua function March's data lacks. Adopt June's `SocketOptionForm.lua`?**
- The curve is `CalculateFinalDamageMaximizeRate( value, level )` (`322a9e30` → `320723b0`, string `334877dc`), at June `SocketOptionForm.src.lua:557`. It calls `CalculateIntermediateDamageMaximizeRate` and `NewDecrease`.
- March's shipped `SocketOptionForm.lua` (`kom_contents/data036_kom`) has neither, and no `NewDecrease` at all. It is an older curve set: June's `CalculateFinalAnitEvadePercent` returns 0, for example.
- **Options:**
  - Adopt June's file wholesale ("June asset wins"). This changes every socket-option curve.
  - Add only the three maximize functions to March's file.
- Either way the user packs it. The code would log `[JUNE_BUFF]` and keep 92 off while the function is missing.
- The option value also comes from a socket field March lacks: `m_fDamageMaximizeRate`, which the socket loader `32726850` reads into socket `+0x13c` and `32270dd0` (`@322710e9`) seeds into 92. That makes it 0 on March, as with 78's terms. Porting it is a socket-system item.

**Q3. Where should the player's attack range come from?**
- June's range basics come from **two extra ints in the server's `KStat`** (`322814a0`: `{ float hp; int atkP, atkM, rangeP, rangeM, defP, defM }`; March's `KStat` in `KncWX2Server/Common/CommonPacket.h` has five fields). `UnitData::m_Stat` and `m_GameStat` both come from those packets.
- The server's formula is not in the client binary.
- NPCs get no range in June either:
  - `CX2GUNPC::SetNPCStat` (`32763770`) writes stat indices 6, 7, 8, 11 and 12 only, then calls slot 119.
  - `ExtraNPCStat` fills stat types 1–7, none of them a range.
  - Summons copy their owner's range (`SetUserSummonedNPCInfo` tail, `3230ca6d`).
- All of June's BBT 7/8 data is `PERCENT` (129 and 127 entries), so no buff makes the range non-zero either.
- So until a range exists, the roll never fires (it needs range > 0), and 92 has no effect even when fully ported.
- **Options:**
  - Extend `KStat` and have the offline emulator fill it. That touches `KncWX2Server/Common`, which this plan forbids, and the formula would have to come from somewhere.
  - Accept range 0 for players, which makes 92 dormant.

**Resolved (user, 2026-10-04): "follow June, so no half-baked port."** It applies to all three questions. Tracing what that means turned up a fourth dependency, so 92 now moves into a new phase, **Phase 16 — Socket-option parity** (below). It does not land in 5c.
- **Q1 → port June's template, as part of Phase 16.**
  - A diff of every `CX2AdditionalStatByBuff` method found that only the option-data template changed. Insert `3236d110`, update `3236d370` (it still returns 1 when found), erase `3236c610`, the plain refresh `3236d350`/`3236d200`, the per-change-type update `3236cd90` and the four setters all equal March's.
  - A member-by-member diff of June's refresh `32290950` against March's `CheckAndUpdateAdditionalStat` found the same refresh variant and modifier everywhere except three members. The MP-gain members **24, 25 and 26** (`ChargeMpHitMelee`/`Range`, `ChargeMpHitted`) moved from the plain refresh to option-data, with the new curves `CalculateFinalMpIncPercentByAttack` (`322a99e0`) and `CalculateFinalMpIncPercentByAttacked` (`32073ba0`).
  - Every June curve reads only `(value, level)` (`[esp+4]`/`[esp+8]`), so March's call shape is enough.
  - The template's inputs are the socket option values. Porting the formula while March's seeds stay March's would itself be a partial update.
- **Q2 → adopt June's `SocketOptionForm.lua` wholesale, as part of Phase 16.** It must ship with the exe that reads the new curves.
- **Q3 → no decision possible; it is a missing fact.**
  - June's client has no range source: no range key in any June client string, and none in June's `CharacterStatTable.lua` (the same five stat columns as March), `ExtraNPCStat` or the NPC stat table.
  - So following June ends at the server's `KStat`.
  - Extending `KStat` is not done. The plan forbids touching `KncWX2Server/Common`, and with no formula the emulator could only send 0.
  - Players' range stays 0, so the roll stays dormant until a source exists. **Question for the user:** do you have any June-era server binary, database or `ServerResource` that computes the attack range?
- **The fourth dependency: June's socket system itself.**
  - June's socket-option loader `32726850` reads 140 keys; March's `X2SocketItem.cpp` reads 111. **41 are June-only**, including `m_fDamageMaximizeRate` (92's seed), 78's three terms, the MP-gain return rates, the boss and low-HP damage rates, skill cool-time cuts and `m_fDecrease_MPCostRate`. **12 are March-only**, among them `m_fAntiEvadePercent` and `m_fEvadePercent`, which June no longer seeds.
  - June's option-seed function `32270dd0` feeds about 30 members from the new socket aggregate (unit `+0x87b4`).
  - Phase 16 lists all of this.

**Already decoded for 92, so the next session does not redo it** (all in `funcmap.tsv`, "Buff Phase 5c"):
- **Member sites.** Member `+0x6d18`:
  - insert `32282f4f`, update `@32270acc`, erase `@32283795`;
  - basic 0 (`@322717f0`), and **no** passive init;
  - option seed from socket `+0x13c`; the NPC branch zeroes it (`@32271470`);
  - option-data refresh with the curve above.
  - Note: `buffre/rawfn.py` stops after 200 instructions. That hid every case past it until a full disassembly found them.
- **The roll `322788c0`.**
  - It runs only when range > 0.
  - `maxi` = the attacker's 92 result × attacker vslot 121 (DamageData). This corrects Phase 1c item 3, which said slot 121 multiplies base.
  - The engine is a `boost::mt19937`, built fresh on every call and seeded with `this->vslot113( 0 ) & 0xffff`. `31ec9d90` is boost 1.44's seed, the version X2Lib already includes at `X2Main.h:606`.
  - `s = variate_generator<mt19937&, normal_distribution<double>>( base + range·maxi, range·0.5 )()`; `322ab740` and `322ab1c0` are boost's classic Box–Muller.
  - The result is `(L < s ? s : L)`, capped at `base + range`, where `L = base − range + 2·range·maxi`.
  - The physic and magic calls share one seed, so each takes the same first (cos-branch) draw.
  - Call sites: `GetFinalDamage` `@32284c1f` (physic) and `@32284c2f` (magic), right where March reads `fPhysicAttack`/`fMagicAttack` from the attacker.
- **Slot 113** = the synced random-table index plus the offset: user `+0x80f8` is March's `m_FrameDataNow.syncData.m_usRandomTableIndex`, and NPC `+0x8474` is `GetNPCSyncData().m_usRandomTableIndex`. Its siblings, slots 114 and 115, are `GetRandomInt` and `GetRandomFloat` on the same word. The base is pure.
- **Slot 121** returns 1.0 for every class except Rose (`32a7ece0`).
- **Add/Ain `InitPassiveSkillState`** (vslot 420) set 92's passive sum from skill ability `0xc4`.
- **Summon copy:** the tail of `SetUserSummonedNPCInfo` (`3230ca6d`) copies the owner's 92 and range results into the summon. The second copy, `32763d4f`, is inside the June-only binding `ApplyPassiveSkillOwnerUser_LUA` (`32763c10`).
- Two rows were added to `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7: Add/Ain's passive sum, and Rose's slot 121.

**Corrections to earlier text.**
1. **Phase 1c item 3, 92 roll:** slot 121 multiplies MAXIMIZE, not base (`@32278981`–`3227899c`).
2. **Phase 1c item 2, `3207b700`:** in the identity and ignore-defence erases it is a single-element erase after a `find_if`, not a range erase.
3. **5b review D1:** the `X2BuffBehaviorTemplet.cpp` comment is corrected in this commit. A comment line was added; the old one was kept, so the edit stays insert-only.

**Reader sweep (after the build).**
- **The 88 list.** A full `.text` displacement sweep of the list (`+0x67dc`/`+0x67e0`) finds only the insert `32282390`, the erase `3227a8c0`, the reader `32274970` and the unit ctor/dtor (`322a11f3`–`322a124c` frees it; `322a4dae` zero-initialises it). June has no wholesale clear on death or on a stage change, and March clears its sibling `CX2StatFactorByBuff` list nowhere either. So nothing else needed porting.
- **78's getter.** `32071b50` has exactly two callers (raw `E8` scan), both in the user `DamageReact` (`@320494ad`, `@320494ed`). The NPC reads the member inline.
- No code change came from the sweep.

**Hygiene.**
- **Flag-off unifdef.** With `NEW_STAT` and `DEBUG` undefined, the strip of both the working file and `HEAD` is byte-identical for 10 of the 11 code files. `X2GameUnit.cpp` differs by one blank line just outside a guard. Every raw file differs from `HEAD`.
- **Encodings and line endings.** `file` signatures are unchanged on every touched file. Every added line is ASCII. The line ending follows the host line: LF in the LF stretches of `X2DamageManager.cpp`/`.h` and `X2GULire_ElvenRanger.cpp`, CRLF elsewhere.
- **Diff.** `git diff --stat`: 310 insertions and 0 deletions over 11 code files (`X2GameUnit.cpp` 102, `X2BuffBehaviorTemplet.cpp` 64, `X2BuffFactor.cpp` 34, `X2BuffBehaviorTemplet.h` 26, `X2GUUser.cpp` 26, `X2GUNPC.cpp` 24, `X2DamageManager.h` 15, `X2GameUnit.h` 9, `X2DamageManager.cpp` 8, `X2GULire_ElvenRanger.cpp` 6, `X2BuffFactor.h` 1). The docs add 27 `funcmap.tsv` rows (tag "Buff Phase 5c") and 2 rows in checklist section 7.

**How to see it in game (for Phase 14).** June numbers come from `june_src_fixed/data036_kom/Enum.src.lua`; the BFI equals the factor ordinal. The data arrives with Phase 13.

| Type | June BFI (BTI) | Where it comes from | What to watch |
|---|---|---|---|
| 78 | `BFI_BUFF_DEVIL_CHAIN_BALL` 553 (BTI 484); `BFI_BUFF_ELDER_SECRET_SET_NORMAL` 567 / `_UPGRADE` 568 (BTI 494) | a monster buff; an item-set buff | The unit wearing it takes almost no additional damage (0.0001), or half (0.5). Your additional-damage numbers on a Devil Chain Ball monster vanish. |
| 88 | `BFI_DEBUFF_CUTTING_SWORD_IGNORE_DEFENCE` 630 (BTI 540; `IGNORE_DEFENCE_RATE` 1, ended by `BFT_HITTED_COUNT`) | a skill debuff | Hits on the debuffed unit ignore its whole defence until its hit count runs out. |
| 92 | deferred. Factors include `BFI_SI_P_EIS_LIGHTNING_STEP` 81, `BFI_BUFF_PHANTOM_CIRCLE` 783, `BFI_SPA_STRONG_BUFF` 949 (`FIX` 10) and `BFI_ALWAYS_SPA_BUFF_2` 1029 (`FIX` 5) | skills, items | nothing until Q1–Q3 are answered |

**Play-test (2026-10-04, user): both checks pass.**
- **88: pass.** On the first try, hits were visibly harder. The log has `first apply BBT 88 (ignore defence): BTI 990088, rate 1`, then `first read BBT 88: piercing defence factor 0 (buff rate 1, damage data rate 0)`.
- **78: pass after a retest; the first "no change" was a display artefact.**
  - The user watched the **chat-box damage log**. That line is the in-house feature `ADD_DAMAGE_LOG` (from `InHouse5.h`). It prints `CX2GUUser::GetCurrentDamage()`, which the attacker's `AttackResult` sets from `GetFinalDamage()` (`X2GUUser.cpp:12659`) before the defender's `DamageReact` adds the additional damage. So the chat line never included additional damage, in March either.
  - 88 shows up there because it acts inside `GetFinalDamage`. 78 acts on the additional part, which only the damage number over the monster shows.
  - A temporary per-hit diagnostic (`531c8ae`, reverted in `2406d67`) logged the same basic attack in three states:
    - unbuffed: `damage 15424.5 + additional attack 2875.89 x 1 = 18300.4`;
    - with a ×10 test buff (factor 362, BTI 990178): `… x 10 = 44183.4`;
    - after the buff expired: back to `18300.4`.
  - The on-screen number is drawn from that total. The user also reported the damage felt higher.
- **Retest note.** The first ×10 retest never ran: the log has `duplicate BTI 990078`, so the original setup line had been pasted again instead of the ×10 one.
- **Deployed exe** (diagnostic removed, same code as `a3db272`): 19,058,176 B, md5 `fedca4823fac169630c037e71971f67f`.
- Out of scope, seen in the log: `BFI 512 requested but not loaded` (not one of this phase's ids).

**Play-test hand-off (dev console, END key; `settings.auth_level` = 3 is already set).**

How the lines were checked:
- **Syntax and tables:** all five lines were run under `elstools`' bundled LuaJIT against a mock manager. The definitions build their tables with the ids named.
- **Not run in the game.** None of them has run in the game yet. Each has the same shape as lines from 5b and its review that did run.
- **Ids assume a fresh launch.** The last load summary logged `factors 361 loaded`, so the next free factor ids are 362 and 363. If you have pasted any other test definition this session, restart the game first; otherwise you will see `ordinal reject BFI 362 … the next id must be N`.
- **The hook.** `JuneBuffTestNewStat_LUA` is the Phase 5 debug hook. It writes straight into a stat store.

0. **Setup (once, right after loading in).**
   - Where: any field or dungeon, after a fresh launch.
   - Paste, first (defines a 60-second "additional damage taken ×3" buff, factor 362):
     ```
     local t = {} local p = {} local r = {} for i = 1, 30 do t[i] = 60 p[i] = 3 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990078, IS_WORLD_BUFF = FALSE, IS_DEBUF = TRUE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 78 }, BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER = { BUFF_CHANGE_TYPE = 3, BUFF_RELATION_TYPE = 7 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 362, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990078, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 78 }, BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER = { BUFF_CHANGE_TYPE = 3, BUFF_RELATION_TYPE = 7, PERCENT = p } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - Paste, second (defines a 60-second "ignore all defence" debuff, factor 363; the same shape as June's Cutting Sword debuff):
     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990088, IS_WORLD_BUFF = FALSE, IS_DEBUF = TRUE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 88 }, BBT_IGNORE_DEFENCE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 363, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990088, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 88 }, BBT_IGNORE_DEFENCE = { IGNORE_DEFENCE_RATE = 1 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - What it does: teaches the game the two test buffs. Nothing changes on screen yet.
   - Look for: no popup, and no `ordinal reject BFI 362` / `363` line in `ErrorLog.txt`.
   - If not: an `ordinal reject … the next id must be N` line means the ids are off. Tell me N, or restart and paste both again.

1. **Monsters take triple additional damage (type 78).**
   - Where: a field or dungeon with ordinary monsters you can hit safely.
   - Paste, first (gives your character a large additional-damage bonus, so there is something for 78 to multiply):
     ```
     g_pX2Game:GetMyUnit():JuneBuffTestNewStat_LUA( 20, 1, 5000, 1 )
     ```
   - Hit a monster a few times with the same basic attack and note the typical damage number.
   - Paste, second (puts the ×3 buff on every monster on the map):
     ```
     for i = 0, g_pX2Game:GetNPCUnitListSize() - 1 do local n = g_pX2Game:GetNPCUnit( i ) if n ~= nil then n:SetBuffFactorToGameUnitByBuffFactorID_LUA( 362, 1 ) end end
     ```
   - What it does: for 60 seconds, the extra "additional damage" part of each of your hits on those monsters counts three times.
   - Look for:
     - Your damage numbers on the monsters rise; the additional part triples, so the rise depends on how big that part is.
     - `ErrorLog.txt` gets `console hook BBT 20, change type 1, value 5000, insert`, then `first apply BBT 78, change type 3, value 3`.
     - After the next hit it gets `first read BBT 78 (npc hit): additional attack A x 3`. A is your additional damage for that hit, before the ×3.
   - If not:
     - No `first apply BBT 78` line: send any `[JUNE_BUFF]` line naming BTI `990078`, BFI `362` or `BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER`.
     - A `first apply` line but no `first read BBT 78` after several hits: tell me "no first read". Your additional damage may be 0; send the `console hook BBT 20` line.
   - Undo the bonus afterwards:
     ```
     g_pX2Game:GetMyUnit():JuneBuffTestNewStat_LUA( 20, 1, 5000, 0 )
     ```

2. **Monsters lose all their defence (type 88).**
   - Where: a field or dungeon with ordinary monsters. Hit one a few times first and note the typical damage number.
   - Paste:
     ```
     for i = 0, g_pX2Game:GetNPCUnitListSize() - 1 do local n = g_pX2Game:GetNPCUnit( i ) if n ~= nil then n:SetBuffFactorToGameUnitByBuffFactorID_LUA( 363, 1 ) end end
     ```
   - What it does: for 60 seconds, every monster on the map takes your hits as if it had no defence at all.
   - Look for:
     - The same attack hits noticeably harder, and returns to normal after 60 seconds.
     - `ErrorLog.txt` gets `first apply BBT 88 (ignore defence): BTI 990088, rate 1`.
     - After a hit it gets `first read BBT 88: piercing defence factor 0 (buff rate 1, damage data rate 0)`.
   - If not:
     - No `first apply BBT 88`: send any `[JUNE_BUFF]` line naming `990088`, `363` or `BBT_IGNORE_DEFENCE`.
     - A `first read` line whose factor is not 0: send it.
     - Damage unchanged although both lines are there: tell me "no visible change". Monsters with very low defence show little difference.

**Not reachable from the console in this build:**
- `IGNORE_DEFENCE_DECLINE` and the `_ON_PVP` / `IGNORE_DEFENCE_RATE` DamageData keys: no March damage data sets them, so only their defaults run. That is March's behaviour.
- The PvP paths (they need a PvP match).
- The packet paths (they need a second client).

**Log lines to read afterwards** (this phase's own, in `ErrorLog.txt`):
- `[JUNE_BUFF] console hook BBT 20 …`
- `first apply BBT 78 …`, `first read BBT 78 (npc hit) …`
- `first apply BBT 88 …`, `first read BBT 88 …`
- any `[JUNE_BUFF]` line naming `990078`, `990088`, `362` or `363`

### Phase 5c — Review (2026-10-04, Opus 5.5 high)

**Verdict: the transcription is correct. No code changed.** 78 and 88 match June in every host, store, class and key default checked. 92 landed no code, so it was out of scope; the two Ghidra spot-checks were 78 and 88, and both passed, so a whole-phase re-check was not needed.

**What was reviewed.**
- The code commit is `a3db272`. The play-test diagnostic `531c8ae` was reverted in `2406d67`, and `git diff a3db272 HEAD -- X2Lib KTDXLIB` is empty.
- The deployed `X2_offline.exe` is still the play-tested build: 19,058,176 B, md5 `fedca4823fac169630c037e71971f67f`, byte-identical to `X2/US_SERVICE/x2.exe` (read programmatically).
- **92 left nothing half-wired.** `BBT_CHANGE_MAXIMIZE` appears only in its enum (`X2BuffFactor.h:130`), the 4b-i level-slice case (`X2BuffFactor.cpp:2789`) and a comment. There is no parse, factory or store.

**Spot-check 1 — 88, in Ghidra (read-only).**
- **Host, `GetFinalDamage` `@3228568a`–`32285735`.**
  - The flag comes from CX2Game vslot 67 (`+0x10c`). Type 1 reads `+0x529`; any other type reads `+0x528`.
  - `m_bPenetrate` (`+0x4c5`) or that flag zeroes the factor.
  - Otherwise `r = 322cc1a0() ? +0x530 : +0x52c`, and the factor is `1 − 32274970(this) − r`, kept only when `0 ≤` it, else 0.
  - `322cc1a0` is `type == 1 || type == 4`.
  - The port matches. Using `GT_PVP` for both selections equals June on March's `GAME_TYPE` (`GT_PVP` = 1, `X2Game.h:62`; no 4). June calls `32274970` twice; it has no side effects.
- **Key defaults, `SetDamageDataFromLUA` `32632390`.**
  - `IGNORE_DEFENCE`: miss → 0 (`@3263c6d8`, `AL` = 0 after the failed read).
  - `_ON_PVP`: miss → the byte at `[esp+0x14]` = `+0x528` (`@3263c8a0`–`a6`).
  - `IGNORE_DEFENCE_RATE`: miss → `fldz` (`@3263ca51`).
  - `_RATE_ON_PVP`: miss → copies `+0x52c` (`@3263cc7e`).
  - `IGNORE_DEFENCE_DECLINE`: miss → `BL` after `xor ebx,ebx` (`@3263ce49`/`65`) = 0.
  - The order is `+0x528`, `+0x529`, `+0x52c`, `+0x530`, `+0x534`, as ported. `LUA_GET_VALUE` (`luaLib/KLuaManager.h:118`) assigns its default only on a miss, so passing the already-read sibling is exact.
- **Factor parse `32d49210`.** `fld1` is stored before the read and again on a miss, so `IGNORE_DEFENCE_RATE` defaults to 1.0, as ported.

**Spot-check 2 — 78, in Ghidra (read-only).**
- **User `DamageReact` `@32049405`–`32049565`.**
  - The `m_bLvDamage` (`+0x57c`) gate comes first. Inside it, the additional-defence reduction is skipped when `+0x534` is set (`@32049412`), which is the ported decline gate.
  - Then: attacker non-null; defender = `DD+0x470`.
  - `k = attacker.32071b10 + this.32071b50 (78) + this.32071af0`. When the defender exists and its `32071b90` is above 0, `k −= 32071b90 × [0x33478b70]`.
  - `k = max( k, 0 )` (`@32049509`–`1c`), then `× vslot 149` (`+0x254`).
  - Then the `v > 0` add and the double-attack add are March's.
- **NPC `DamageReact` `@327b4214`–`327b43be`.**
  - The same gates (decline `@327b4221`; the additional defence is read inline from `+0x50cc`).
  - The same inline `k`: `this+0x5450` (78's result), `attacker+0x526c`, `this+0x51b8`. The defender term `+0x5aec` is subtracted whenever the defender exists, with no `> 0` test.
  - Then the clamp and `× vslot 149`.
- **The port matches.** The recorded deviation `k = max( 78, 0 )` drops only the three option terms, and all three are 0 on March data. The March context around both blocks (`if( m_bLvDamage == false )`, the attacker null test, the double-attack add) is unchanged.

**Source checks (no Ghidra needed).**
- **88 store.** `CX2StatFactorByBuff( identity, float )` / `GetValue()` (`X2BuffTemplet.h:812`/`816`). The erase is `FindSameBuffIdentity` plus a single `erase`. The reader is the sum, capped at 1.0. All as item 3 and the Done note say.
- **`g_pX2Game` in the 88 host.** `GetFinalDamage` already returns early when it is null (`X2GameUnit.cpp:2099`), so the new `GetGameType()` call is safe.
- **`DamageData` copies.** The copy ctor is `*this = rhs` (`X2DamageManager.h:1441`), there is no user `operator=`, and `Init()` routes through `SimpleInit()`. The only `"IGNORE_DEFENCE"` parser is `X2DamageManager.cpp:511`. So the four new members need nothing beyond the declaration, `SimpleInit` and the parse, as the Done note claims.
- **Runtime writers of `m_bIgnoreDefence`.** A full `X2Lib` grep finds exactly the four the Done note checked: memo 26 (`X2BuffTemplet.cpp:1416`), Add (`X2GUAdd.cpp:10777`) and Rena ×2. Only Rena sets the twin, which matches June.

**Hygiene.**
- **`file`.** Signatures are unchanged on all 11 touched code files (parent against `a3db272`).
- **Diff.** `git diff --stat a3db272^ a3db272 -- X2Lib`: **315 insertions, 0 deletions**. No added line has a non-ASCII byte.
- **Flag-off build path.** A strip of `NEW_STAT` and `DEBUG` (taking every `#else`) of parent and commit is byte-identical for 10 files. `X2GameUnit.cpp` differs by one blank line just outside the new guard (after its `#endif`), which is cosmetic and was already in the Done note.
  - Read directly, the three `#else` branches are March's lines verbatim: both `DamageReact`s' `if ( 0.0f < GetAdditionalDefence() )` and `fAdditionalAttackValue = …GetAdditionalAttackDamage( pDamageData )`, and `GetFinalDamage`'s penetrate/ignore block.
  - Every other edit is a pure insertion (keys, members, store cases, Rena's twin).

**Corrections.**
1. **Done note, Hygiene:** "310 insertions" should read **315**. Its own per-file numbers add up to 315.

**Not reachable from the console, and why** (in addition to the Done note's list):
- **78's user-side host** (a hit taken by the player) cannot fire offline. `CX2GUNPC::GetAdditionalAttackDamage` returns 0 unless the NPC has an owner (`X2GUNPC.cpp:30611`), and the base returns 0. A player only takes additional damage from another player or an owned summon, which means PvP. It is the same block as the NPC side, which the play-test proved.

**Play-test hand-off (optional regression; dev console, END key).** The review changed no code, and the exe in the game directory already passed both of the Done note's checks. One path that play-test did not show is 88 **expiring**, i.e. the erase. It has no log line, so this check is visual only.

1. **The no-defence debuff wears off (type 88).**
   - **Where:** a field or dungeon with ordinary monsters, **after a fresh launch**. Hit one a few times and note the typical damage number.
   - **Paste,** first (the Done note's two definitions, unchanged; both ran in the game on 2026-10-04):
     ```
     local t = {} local p = {} local r = {} for i = 1, 30 do t[i] = 60 p[i] = 3 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990078, IS_WORLD_BUFF = FALSE, IS_DEBUF = TRUE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 78 }, BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER = { BUFF_CHANGE_TYPE = 3, BUFF_RELATION_TYPE = 7 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 362, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990078, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 78 }, BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER = { BUFF_CHANGE_TYPE = 3, BUFF_RELATION_TYPE = 7, PERCENT = p } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990088, IS_WORLD_BUFF = FALSE, IS_DEBUF = TRUE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 88 }, BBT_IGNORE_DEFENCE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 363, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990088, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 88 }, BBT_IGNORE_DEFENCE = { IGNORE_DEFENCE_RATE = 1 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - **Paste,** then (puts the 60-second debuff on every monster on the map):
     ```
     for i = 0, g_pX2Game:GetNPCUnitListSize() - 1 do local n = g_pX2Game:GetNPCUnit( i ) if n ~= nil then n:SetBuffFactorToGameUnitByBuffFactorID_LUA( 363, 1 ) end end
     ```
   - **What it does:** for 60 seconds the monsters take your hits as if they had no defence. After that they should be back to normal.
   - **Look for:**
     - **Read the damage line in the chat box**, not the number over the monster. The chat line is the result 88 changes, without the additional-damage part the floating number adds. Compare non-critical hits of the same attack.
     - Hits are clearly harder right away.
     - **More than 60 seconds later**, the same attack is back to the number you noted first.
     - `ErrorLog.txt` has `first apply BBT 88 (ignore defence): BTI 990088, rate 1` and `first read BBT 88: piercing defence factor 0 …`, as before.
   - **If not:**
     - Still hitting hard well after 60 seconds: tell me "88 did not wear off", and how long you waited.
     - An `ordinal reject BFI 362`/`363` line: the game was not freshly launched. Restart and paste again.

**Log lines to read afterwards** (`ErrorLog.txt`): `first apply BBT 88 …`, `first read BBT 88 …`, and any `[JUNE_BUFF]` line naming `990088` or `363`.

**Play-test (2026-10-04, user): pass.**
- **On screen (user):** the 88 debuff hit harder, then wore off after 60 seconds.
- **Log, launch of 17:07** (`ErrorLog.txt`; the exe is still md5 `fedca482…`):
  - `17:7:37 first apply BBT 88 (ignore defence): BTI 990088, rate 1`
  - `17:7:40 first read BBT 88: piercing defence factor 0 (buff rate 1, damage data rate 0)`
- **No reject of 362 or 363,** and no other `[JUNE_BUFF]` line after load.
- **The load summary is the known baseline:** `factors 361 loaded`, ordinal rejects 5 (481–483, 372, 373), as recorded in Phase 3. Nothing from this phase.

### Phase 6 — Done (2026-10-04), play-tested (user checks 1-2; logs checked for 3 and the clean check 4)

**Play-test (2026-10-04, user, deployed exe `cb9d9015…`; `ErrorLog.txt` read afterwards, no `runtime error` or `[JUNE_BUFF]` failure line besides the 5 startup ordinal rejects of other BFIs).**
- **1 (75):** user passed. `first apply BBT 75 … rate 1`, then `first read BBT 75: the attacker's double-attack roll succeeded`.
- **2 (103):** user passed. `first apply BBT 103 … 4 change(s), first type 5 change type 3 value 2`, then `first read BBT 103: skill type 5 cool time runs x 2` (the host and the lazy rebuild ran on a real parsed factor).
- **3 (89, 90, 91, 98, 99, 100), judged from the log:** all six `first apply` lines appeared, so parse, level slice, templet and `SetFactor` worked on real factors. The user's four equipped skills (3014, 3003, 3000, 3216) are all skill type 7. Readers: 89 = 1.5 on all four; 98 = 1.2 on all four (slot A row); 99 = 0.5; 100 = 0.9 with `applied 1`; 91 skill 31012 = 0 (and its one-shot `first read BBT 91` fired). 90's ×1.3 did not show because none of the equipped ids is in 31011–31014 (the 90 line printed the same 1.5 as 89), so 90's reader is **not yet shown numerically**: the clean check 4 below does it.
- **4:** run within 60 s of check 3, so it was contaminated exactly as the hand-off warned (the live 365 buff holds 91 at 0; the live 363 buff holds 89 at 1.5 and the test wrote type 6 while the skills are type 7; the expired 369 buff left the 103 dirty flag set, so the hook's own read-back rebuilt the table to 1.0). Not a failure; 103's store and read are proven by check 2. Clean rerun (fresh launch, no setup lines; types 7 and skill 3014 match this character):
  ```
  local u = g_pX2Game:GetMyUnit() u:JuneBuffTestNewSkill_LUA( 89, 7, 1.5, 1 ) u:JuneBuffTestNewSkill_LUA( 89, 7, 1.5, 0 ) u:JuneBuffTestNewSkill_LUA( 90, 3014, 1.3, 1 ) u:JuneBuffTestNewSkill_LUA( 90, 3014, 1.3, 0 ) u:JuneBuffTestNewSkill_LUA( 91, 31012, 0.5, 1 ) u:JuneBuffTestNewSkill_LUA( 91, 31012, 0.5, 0 ) u:JuneBuffTestNewSkill_LUA( 103, 7, 2, 1 ) u:JuneBuffTestNewSkill_LUA( 103, 7, 2, 0 )
  ```
  Expect: 89 = 1.5 on all four skills, then 1; 90 = 1.3 on slot 0 (skill 3014) only, then 1; 91 = 0.5 then 1; 103 type 7 = 2 then 1.
- **4, clean rerun (fresh launch, 18:25 log): passed.** 89 = 1.5 on all four skills, then 1; 90 = 1.3 on slot 0 (skill 3014) only, 1 on the others, then 1 on all; 91 = 0.5, then 1; 103 type 7 = 2, then 1 (types 5, 6 and 13 stayed 1). So the store, reader and erase of 89, 90, 91 and 103 are shown, and 90's reader is no longer open. Nothing is owed from this phase except its Opus review.

**Original Done note (written before the play-test).**

**Result.** All eight types are ported under `SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL`; none is Opus-only. 75, 91 and 103 are complete (class, store, reader, host). 89, 90, 98, 99 and 100 have the class, store and reader only, and are **inert until Phase 6b**: no host function was touched (`DoStateEndStart`, the summon-NPC helper, `GetActualMPConsume` and the cooldown setters are as they were). `buff_census.py --phase 6` lists none of the eight types (March "handles 98", was 90). `US_SERVICE` build rc=0 for KTDXLIB, X2Lib (PCH invalidated with `touch X2Lib/stdafx.cpp`) and X2 (a new virtual on `CX2GameUnit`, so a full X2Lib rebuild). Deployed `X2/US_SERVICE/x2.exe` as `X2_offline.exe` (names read programmatically): 19,089,920 B, md5 **`cb9d90155b72a02fb4e417fb55ea249b`**, byte-identical to the build output. **Not launched.** The exe contains the eight `BBT_*` key strings (ASCII and UTF-16), `SKILL_CATEGORY_%d`, `MULTIPLY_MP_CHARGE_RATE`, and the debug strings `first apply BBT 75 (double attack buff)`, `first apply BBT 90` / `91` / `98` / `103`, `first read BBT 75` / `91` / `103`, `reader BBT 89 x 90` and `JuneBuffTestNewSkill_LUA`; the `first read` strings sit in `IsDoubleAttackBuffByBuff`, `GetChargeMpRateBySkillID` and `GetSkillCoolTimeSpeedBySkillType`, inside the flag, so they prove the readers compiled in.

**Flags.** `SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL` (requires `_ENUM`, `_TEMPLET_PARITY` for 89's slot 7, and `_BEHAVIOR_PARITY` for the level-slice cases, the no-popup `DISPLAY_ERROR` and the type-56 block that 75 completes), appended to `KTDXLIB/Always.h`. The step-8 lines and the console hook sit under `SERV_IRUHADEV_JUNE_BUFF_DEBUG`. Every edit is a pure insertion inside the flag except two kinds: the two cooldown subtractions in `CX2UserSkillTree::OnFrameMove` are wrapped (the original line sits in the `#else`), and the two "Phase 6 owns it" placeholder comments Phase 4b-i part 2 left in the DamageReacts were reworded to "just below" (these are the 2 deleted lines in the diff stat).

**What the doc's type-56 question turned out to be.** Phase 4b-i part 2 had already landed June's 56 changes in both `DamageReact`s (`!= AT_SPECIAL` on the defender's own list, the NPC's `!m_bNoDamage`, and the 8021/8025/8026 + `BTI_BUFF_SHADE_THE_ILLUSION` rule) and left a placeholder at `@32048da7` / `@327b3ac7`. Nothing about 56 was changed here; 75 only fills the two placeholders, as its own `if` with its own random roll (June makes two `IsDoubleAttack*` calls), using the attacker pointer the block already declared. The NPC side adds `!m_bNoDamage`, the user side does not, as June.

**Per type** (June members are `CX2GameUnit` / `CX2GUUser` offsets; the port uses named members and never an offset):

| Type | Factor parse | Level slice | SGU | Class | Store | Reader / host |
|---|---|---|---|---|---|---|
| 75 DOUBLE_ATTACK_BUFF | March's `ParsingDoubleAttackByBuffBehavior` (June `32d46800`, shared with 56) | already in (4b-i): default slice | none | new `CX2BuffDoubleAttackBuffBehaviorTemplet` (vt `33535070`) | new `CX2GameUnit::m_vecDoubleAttackBuff` (June `+0x65b4`; `CX2DoubleAttack` elements, June has an identical `CX2DoubleAttackBuff`) | `IsDoubleAttackBuffByBuff` (`32275050`); **host** both `DamageReact`s on the attacker's list |
| 89 CHANGE_SKILL_DAMAGE | new `ParsingChangeSkillDamageBehavior` (`32d49460`) | already in (`32d47a9e`) | none | new `CX2BuffChangeSkillDamageBehaviorTemplet` (vt `33535378`), **with slot 7** `OnAccumulationCountDecreased` (`32e7c020`) | new `CX2GUUser::m_vecSkillDamageByBuff` (June `+0x92d4`) of `CX2SkillTypeRateByBuff` | `CX2GUUser::GetSkillDamageRateByBuff` (`31fd7cc0`); host is 6b |
| 90 CHANGE_SKILL_DAMAGE_BY_SKILL_ID | new `ParsingChangeSkillDamageBySkillIDBehavior` (`32d49810`) | copy as is | none | new `CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet` (vt `33535594`) | new `m_vecSkillDamageBySkillIDByBuff` (`+0x92e4`), last matching block wins | the same `GetSkillDamageRateByBuff` (the 90 loop sits outside its skill-templet test); host is 6b |
| 91 CHARGE_MP_HIT_BY_SKILL_ID | new `ParsingChargeMpHitBySkillIDBehavior` (`32d49da0`) | copy as is | none | new `CX2BuffChargeMpHitBySkillIDBehaviorTemplet` (vt `335355d0`) | new `m_vecChargeMpHitBySkillIDByBuff` (`+0x92f4`), last matching block wins | `GetChargeMpRateBySkillID` (`31fd7df0`) behind a new virtual `CX2GameUnit::GetChargeMpRateByBuff( const DamageData* )` (base 1.0, `CX2GUUser` override = slot 109 `31fd7ea0`); **host** `SetDamageExceptionProcess`, after the whole melee/range `if / else`, on the same `fHitAddMP`, for every attack type |
| 98 CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE | new `ParsingChangeSkillDamageBySkillSlotTypeBehavior` (`32d4a370`) | already in (`32d47e66`) | none | new `CX2AddDamageOfSkillSlotType` (vt `33535160`) | none (reads live factors) | `CX2GUUser::GetSkillDamageRateBySlotType` (`32039e00`) with a new `CX2UserSkillTree::GetEquippedSkillSlotType`; host is 6b |
| 99 CONSUME_MP_RATE_BY_SKILLTYPE / 100 COOLTIME_RATE_BY_SKILLTYPE | new `ParsingChangeRateBySkillTypeBehavior` (`32d4aa30` / `32d4b020`, byte-identical) | already in (`32d47b85`) | none | **one** class `CX2BuffChangeRateBySkillTypeBehaviorTemplet` for both (June's two are byte-identical; type from `GetType()`) | none | `CX2GUUser::ApplySkillTypeConsumeMpRate` (`3203a250`), `ApplySkillTypeCoolTimeRate` (`3203a420`) over one shared loop; hosts are 6b |
| 103 CHANGE_SKILL_COOL_TIME_SPEED | new `ParsingChangeSkillCoolTimeSpeedBehavior` (`32d4b610`) | already in (`32d47b85`) | none | new `CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet` (vt `3353542c`), **no `SetFactorFromPacket`** (June slot 13 is the empty base) | new `CX2GUUser::m_mapSkillCoolTimeSpeed` + dirty flag (June `+0x931c`, `+0x9314`); rebuild `32039fb0` | `GetSkillCoolTimeSpeedBySkillType` (`31fd8bc0`); **host** `CX2UserSkillTree::OnFrameMove`, both slot rows: `left -= GetSkillCoolTimeSpeedByBuff( skillId ) * dt` (`32896850`) |

99, 100 and 103 share one body in a base class (`CX2BuffSkillTypeChangeBehaviorTemplet`: the triples, `GetFactor`, and the first-apply / restack arithmetic; 103 merges BCT_PERCENT only). All eight are in the factor-parse switch of **both** macro lists and in the templet factory. UDP layout is through `GetFactor` / `SetFactorFromPacket`, as in the table (the 103 reader walks `GetBuffFactor`, so 103's `GetFactor` emits the triples).

**What was settled by reading (the parse addresses are in the doc's per-type map; Ghidra was opened at them and nowhere else).**
- **A failing parse returns false with nothing pushed.** June's `32d49460`, `32d49810`, `32d49da0`, `32d4a370`, `32d4aa30`, `32d4b020` and `32d4b610` all end a failed read at an `XOR AL,AL` return and push the factor only after the loop. The doc's "stops at the first missing or zero `SKILL_TYPE`" and "an entry is skipped" are therefore a **whole-factor reject**; the port does the same (and also ends the open category table, which June leaves open).
- **The count is always written.** 89, 99, 100 and 103 call `ParsingTableForWhile( …, 1 )` with a literal 1 (99 and 100 take `bIsSkill` but never pass it on; 89, 90, 91, 98 and 103 take none), so the factor is always the counted layout, whichever macro list calls it. `VALUE` is read for change types 0, 1, 3 (99, 100) or 3 (103); another change type leaves a block as `[SKILL_TYPE, change type]` with no count, which June's level slice then reads past. The port reproduces that.
- **90 / 91 key rules.** The rate key is required (`MULTIPLY_DAMAGE_RATE` for 90, `MULTIPLY_MP_CHARGE_RATE` for 91). 90 needs rate > 0, 91 accepts 0 (`TEST AH,1` against `TEST AH,0x41`). `SKILL_ID_LIST` is read from index 1 while a number ≥ 0 is there; an empty or missing list rejects the factor.
- **98.** `SKILL_SLOT_TYPE` is read with its result ignored (so a missing key means 0) and a negative value rejects; the rate must be ≥ 0. June also rejects a negative slot type in `SetFactor`; both are in.
- **`GetBuffFactor` filter.** The 98, 99, 100 and 103 readers walk `CX2GameUnit::GetBuffFactor`. March's skips finished buffs and **world buffs**; June's `3229fd80` is described as "every live, unfinished buff", with no world-buff mention. No June 98/99/100/103 buff is a world buff, so nothing differs in practice.
- **103 is rebuilt lazily.** June rebuilds in `CommonFrameMove` when the dirty flag is set; the port rebuilds when the table is read, which gives the same table. Both `DoFinish` paths in March run to completion (the templet is erased in the same call) before the next read, so a finished buff cannot be seen by a rebuild.
- **98's slot rows.** June has 5 + 5 slots (0–3 A, 5–8 B, 4 and 9 transcendence, `3214f3d0`); March has 4 + 4 and no transcendence slot, so the port maps the **row** to the slot type (`CX2UserSkillTree::GetEquippedSkillSlotType`: 0 none, 1 A, 2 B). The same applies to 103's host: it walks March's own two rows.
- **Include order (a compile error found and fixed).** `X2Main.h` includes `X2BuffBehaviorTemplet.h` (line 144) **before** `X2BuffTemplet.h` (147), so the behavior classes could not name `CX2SkillTypeRateByBuff::…`. The two plain data types now live in `X2BuffBehaviorTemplet.h` (`JuneSkillTypeRate`, `JuneSkillIDRateBlock`) and the element classes in `X2BuffTemplet.h` alias them.
- **DamageData keys.** The Phase 6 readers use `APPLIED_SKILL_ID` (91) and `NO_DAMAGE` (75's NPC side). Both are parsed (`m_iAppliedSkillID` by 4b's DAMAGEDATA parity, `m_bNoDamage` by March); `USED_SKILL_ID` (the 56 rule) was already in. Nothing was missing.
- **Level slice of 75.** Measured on `june_src_fixed/data036_kom/BattleFieldBuffFactor.src.lua`: all nine 75 factors (BFI 466, 532, 591, 593, 779, 889, 955, 1041, 1042) carry `BCT_PERCENT` with a 30-entry table, so the default slice takes the per-level value (0.1 to 0.5 as the chance of a double hit). The 89, 100 and 103 tables are 1, 4 or 30 entries (all ≥ 1), so `min( level, count )` is always a real value.
- **June DamageData reach of 91.** The only March data that sets `APPLIED_SKILL_ID` is one `DamageEffect` (`DamageEffect.lua`: `APPLIED_SKILL_ID = RNSI_SA_RHB_LOCKON_STRATOS`, skill 31012, an `AT_SPECIAL` hit). Every other March hit has `m_iAppliedSkillID` 0, so the 91 host reads 1.0 for it; the live read site can only be shown on that one attack until June damage data arrives. The reader itself is shown by the console hook below.

**Findings for later phases.**
1. **Phase 13 needs two Enum tables.** The staged Enum has neither `SKILL_SLOT_TYPE` (`SKILL_SLOT_None` 0, `_A` 1, `_B` 2, `_TC` 3) nor June's `ST_SPECIAL_ACTIVE_GRADE_1..3` (14–16), `ST_CORE` and `ST_CORE_PASSIVE` in `SKILL_TYPE`. June's 89, 100 and 103 data use `SKILL_TYPE.ST_SPECIAL_ACTIVE_GRADE_n` as the category key and 98's data uses `SKILL_SLOT_TYPE.SKILL_SLOT_TC`; with those names `nil`, the category's `SKILL_TYPE` is missing and the parse rejects the **whole** factor. The C++ `CX2SkillTree::SKILL_TYPE` also stops at `ST_HYPER_ACTIVE_SKILL` (13); only the console test needs numbers, the code compares ints, but the grade types need adding with the characters.
2. **Phase 6b** still owns the five inert hosts. The readers it will call are: `GetSkillDamageRateByBuff` and `GetSkillDamageRateBySlotType` (power rate cache at state start; 98 first, as June), `ApplySkillTypeConsumeMpRate` (June calls it at `@3203d024`) and `ApplySkillTypeCoolTimeRate` (the cooldown helper). June skill ids use `+0x178` (the skill memo / base id), which March lacks, so 6b must say it passes plain ids.
3. **Character-port checklist:** nothing added. No Phase 6 reader has a class override in a character function; 75, 89, 90, 91, 98, 99, 100 and 103 are all in common code.

**Behaviour on March data: none for the five inert types, and 75, 91, 103 only when a factor carries them.** March's shipped data has no factor with any of the eight types, so the new code runs only when June data (Phase 13) or the console lines below put one in.

**Hygiene.** `git diff --stat`: 1,874 insertions and 2 deletions over 15 files (`X2BuffBehaviorTemplet.cpp` 621, `X2GUUser.cpp` 455, `X2BuffFactor.cpp` 252, `X2BuffBehaviorTemplet.h` 226, `X2BuffTemplet.h` 72, `X2GameUnit.cpp` 56, `X2UserSkillTree.cpp` 55, `X2GUUser.h` 45, `Always.h` 19, `X2BuffFactor.h` 12, `X2GameUnit.h` 12, `X2GUNPC.cpp` 8, `X2UserSkillTree.h` 8, `X2Main.cpp` 3, and 8 `funcmap.tsv` rows tagged "Buff Phase 6", the parse functions). **Commit note:** commit `54eaef6` also carries 24 `funcmap.tsv` rows tagged "Buff Phase 16" that a second session had left uncommitted in the same working tree when `git add -A` ran; they are not Phase 6's and history was not rewritten. The deployed exe was built on top of `5dcf460`; the commits between the session's start (`99b2dcf`) and `5dcf460` touch only `JUNE_BUFF_PORT_PHASES.md` and `funcmap.tsv`, so no Phase 15/16 source is in it. `file` is unchanged on every touched source (all added lines ASCII, CRLF where the file is CRLF; `X2UserSkillTree.cpp` and `.h` are mixed CRLF/LF and each edit follows the lines around it). Every deleted line is one of the five replaced lines named under *Flags* and keeps its original in an `#else` or is a comment. *(Review correction: the source diff has 2 deleted lines, both the placeholder comments; the two cooldown subtractions are wrapped, so their originals stay as context and are not deletions.)*

**How to see it in game (for Phase 14).** June numbers from `june_src_fixed/data036_kom/Enum.src.lua` and the factor file (BFI = ordinal in the file). The data comes with Phase 13; for a unit to *have* the buff the factor must be applied, e.g. `SetBuffFactorToGameUnitByBuffFactorID_LUA( BFI, 1 )` once June data is loaded. 89, 90, 98, 99 and 100 are **inert until 6b**: the console read-out in the hand-off shows their readers, nothing changes in play.

| Type | June BFI (BTI) | Where it comes from | What to watch |
|---|---|---|---|
| 75 | `BFI_BUFF_BRUTAL_CUTTER` 466 (BTI 429), `BFI_BUFF_BLOODLUST` 779 (BTI 753), `BFI_BUFF_SCHWERT_PLATZEN` 1041 (BTI 975), `BFI_BUFF_SOCKET_BERSERKER` 532 (item) | skill and item buffs of June classes | your non-special hits on a monster double (red damage number); the chance is the level's `PERCENT` (0.1 to 0.5) |
| 89 | `BFI_BUFF_2013_DEFENSE_BUFF` 195 (BTI 202), `BFI_BUFF_OVER_STRIKE` 707 (BTI 698), `BFI_BUFF_HEAVYFIREARMS_MASTERY` 721 (BTI 705, decays by `ACCUMULATION_DECREASE_WHEN_FINISH`, 4 levels of rate) | item and skill buffs | **inert until 6b**; later: the power rate of skills of the listed skill types |
| 90 | `BFI_SI_A_LWS_NATURE_FORCE` 23 (BTI 23), `BFI_BUFF_RBM_COUNTER` 867, `BFI_BUFF_CDC_OPEN_MIND` 883 | skill buffs | **inert until 6b**; later: ×1.3 on the listed skill ids |
| 91 | `BFI_DEBUFF_SI_A_LWS_NATURE_FORCE_MEMO` 123, `BFI_DEBUFF_SI_A_LWS_NATURE_FORCE` 950 (both BTI 137) | a skill debuff | MP gained from hits of the listed skill ids is scaled (0.8 in June's data) |
| 98 | `BFI_TRANSCENDENTAL_SLOT_ATTACK_EFFECT` 843 (BTI 820), slot type `SKILL_SLOT_TC` | the transcendence slot (March has none) | **inert until 6b**; and needs a transcendence slot, so it cannot show in March |
| 99 | none (June data does not use it) | – | **inert until 6b**; no data |
| 100 | `BFI_BUFF_OVER_STRIKE_RSR_TRANSCENDENCE` 918 (BTI 888, `VALUE` −0.1) | a skill buff | **inert until 6b**; later: cooldowns 10 % shorter for the three grade types |
| 103 | `BFI_BUFF_RAPID_GUARDIAN` 991 (BTI 936, `VALUE` 2) | a skill buff | your skill cooldown timers run twice as fast for the grade skill types (`ACTIVE`, `GRADE_1..3`) |

**Play-test hand-off (dev console, END key; `settings.auth_level` = 3 is already set).**

How the lines were checked:
- **Syntax and tables:** all eight setup lines were run under `elstools`' bundled LuaJIT against a mock manager. They build the tables with the ids named (templets 990075, 990089, 990090, 990091, 990098, 990099, 990100, 990103; factors 362 to 369).
- **Not run in the game.** The C++ they reach has compiled but never run. The setup lines have the same shape as the 5b and 5c lines that did run.
- **Ids assume a fresh launch.** The last load summary logged `factors 361 loaded`, so the next free factor ids are 362 to 369. If you pasted any other test definition this session, restart the game first; otherwise you will see `ordinal reject BFI 362 … the next id must be N`.
- **The hook.** `JuneBuffTestNewSkill_LUA( type, a, b, mode )` is this phase's debug hook on your unit. Mode 2 only reads; it logs what the type's reader answers (so a buff applied through the real parse can be read back), and modes 1 / 0 write or remove one entry straight into the store. It writes to `ErrorLog.txt`, nothing on screen.
- **Why the real path.** The shipped March data has no factor with these types, so the setup lines teach the game eight throw-away buffs through the real loader. Applying one then runs the parse, the level slice, the templet, `SetFactor` and the store or reader on a real factor.

0. **Setup (once, right after loading in; paste all eight, in this order).**
   - Where: any field or dungeon, after a fresh launch.
   - What they define (all last 60 seconds and are applied to you by the checks below): 362 double hits (100 % chance), 363 +50 % power for skill types 6 and 7, 364 ×1.3 for skill ids 31011 to 31014, 365 no MP gain from hits of skill 31012, 366 ×1.2 for skills in the slot A row, 367 half MP cost for skill types 6 and 7, 368 10 % shorter cooldown for skill types 6 and 7, 369 cooldowns run twice as fast for skill types 5, 6, 7 and 13.

   - Paste, factor 362 (type 75):

     ```
     local t = {} local r = {} local p = {} for i = 1, 30 do t[i] = 60 r[i] = 1 p[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990075, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 75 }, BBT_DOUBLE_ATTACK_BUFF = { BUFF_CHANGE_TYPE = 3 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 362, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990075, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 75 }, BBT_DOUBLE_ATTACK_BUFF = { BUFF_CHANGE_TYPE = 3, PERCENT = p } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```

   - Paste, factor 363 (type 89):

     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990089, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 89 }, BBT_CHANGE_SKILL_DAMAGE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 363, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990089, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 89 }, BBT_CHANGE_SKILL_DAMAGE = { SKILL_CATEGORY_0 = { SKILL_TYPE = 6, MULTIPLY_DAMAGE_RATE = { 1.5 } }, SKILL_CATEGORY_1 = { SKILL_TYPE = 7, MULTIPLY_DAMAGE_RATE = { 1.5 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```

   - Paste, factor 364 (type 90):

     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990090, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 90 }, BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_ID = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 364, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990090, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 90 }, BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_ID = { SKILL_CATEGORY_0 = { MULTIPLY_DAMAGE_RATE = 1.3, SKILL_ID_LIST = { 31011, 31012, 31013, 31014 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```

   - Paste, factor 365 (type 91):

     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990091, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 91 }, BBT_CHARGE_MP_HIT_BY_SKILL_ID = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 365, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990091, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 91 }, BBT_CHARGE_MP_HIT_BY_SKILL_ID = { SKILL_CATEGORY_0 = { MULTIPLY_MP_CHARGE_RATE = 0, SKILL_ID_LIST = { 31012 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```

   - Paste, factor 366 (type 98):

     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990098, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 98 }, BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 366, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990098, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 98 }, BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE = { MULTIPLY_DAMAGE_RATE = 1.2, SKILL_SLOT_TYPE = 1 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```

   - Paste, factor 367 (type 99):

     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990099, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 99 }, BBT_CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 367, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990099, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 99 }, BBT_CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE = { SKILL_CATEGORY_0 = { SKILL_TYPE = 6, BUFF_CHANGE_TYPE = 3, VALUE = { -0.5 } }, SKILL_CATEGORY_1 = { SKILL_TYPE = 7, BUFF_CHANGE_TYPE = 3, VALUE = { -0.5 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```

   - Paste, factor 368 (type 100):

     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990100, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 100 }, BBT_CHANGE_COOLTIME_RATE_BY_SKILLTYPE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 368, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990100, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 100 }, BBT_CHANGE_COOLTIME_RATE_BY_SKILLTYPE = { SKILL_CATEGORY_0 = { SKILL_TYPE = 6, BUFF_CHANGE_TYPE = 3, VALUE = { -0.1 } }, SKILL_CATEGORY_1 = { SKILL_TYPE = 7, BUFF_CHANGE_TYPE = 3, VALUE = { -0.1 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```

   - Paste, factor 369 (type 103):

     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990103, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 103 }, BBT_CHANGE_SKILL_COOL_TIME_SPEED = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 369, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990103, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 103 }, BBT_CHANGE_SKILL_COOL_TIME_SPEED = { SKILL_CATEGORY_0 = { SKILL_TYPE = 5, BUFF_CHANGE_TYPE = 3, VALUE = { 2 } }, SKILL_CATEGORY_1 = { SKILL_TYPE = 6, BUFF_CHANGE_TYPE = 3, VALUE = { 2 } }, SKILL_CATEGORY_2 = { SKILL_TYPE = 7, BUFF_CHANGE_TYPE = 3, VALUE = { 2 } }, SKILL_CATEGORY_3 = { SKILL_TYPE = 13, BUFF_CHANGE_TYPE = 3, VALUE = { 2 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```

   - What it does: teaches the game the eight test buffs. Nothing changes on screen yet.
   - Look for: no popup, and no `ordinal reject BFI 36x` line in `ErrorLog.txt`.
   - If not: an `ordinal reject … the next id must be N` line means the ids are off. Tell me N, or restart and paste all eight again.

1. **Normal hits on monsters double (type 75, complete).**
   - Where: a field or dungeon with ordinary monsters you can hit safely. Hit one a few times with your plain attack first and note the typical damage number.
   - Paste:
     ```
     g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 362, 1 )
     ```
   - What it does: for 60 seconds every plain (non-skill) hit you land counts twice.
   - Look for: your plain-attack damage numbers on monsters about double, with the red "extra" look. Skill hits are unchanged. `ErrorLog.txt` gets `first apply BBT 75 (double attack buff): BTI 990075, rate 1`, and after a hit `first read BBT 75: the attacker's double-attack roll succeeded (rate 1)`.
   - If not: a `first apply` line but no doubling and no `first read BBT 75` means the new DamageReact block did not run; tell me "no first read". No `first apply BBT 75` line: send any `[JUNE_BUFF]` line naming BTI `990075` or `BBT_DOUBLE_ATTACK_BUFF`.
   - Undo: wait 60 seconds.

2. **Skill cooldowns run twice as fast (type 103, complete).**
   - Where: a field or dungeon, with at least one skill equipped and enough MP to use it.
   - Paste, first:
     ```
     g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 369, 1 )
     ```
   - Use one of your skills and watch its cooldown on the slot (compare with a normal use if you remember the time).
   - Paste, second (writes what the game now holds into the log):
     ```
     g_pX2Game:GetMyUnit():JuneBuffTestNewSkill_LUA( 103, 0, 0, 2 )
     ```
   - What it does: for 60 seconds your skill cooldowns tick down twice as fast; the second line lists the speed the game has for skill types 5, 6, 7 and 13.
   - Look for: the cooldown overlay of the skill you used empties in about half the time. `ErrorLog.txt` gets `first apply BBT 103 (skill cool time speed): BTI 990103, 4 change(s), first type 5 change type 3 value 2`, (the one-shot `first read BBT 103: skill type N cool time runs x 2` line also appears, N being the type of the skill you used) and from the second line four `reader BBT 103: skill type 5/6/7/13 cool time speed = 2` lines (the extra fifth line repeats type 0 as 1).
   - If not: no `first apply` line: send any `[JUNE_BUFF]` line naming BTI `990103` or `BBT_CHANGE_SKILL_COOL_TIME_SPEED`. A `first apply` line but speed 1 in the reader lines means the table was not rebuilt; tell me. A `first apply` line, speed 2, but a normal cooldown means the host did not run, or your skill's type is none of 5, 6, 7 and 13; send the `first read` line (or its absence) and the skill's name.
   - Undo: wait 60 seconds.

3. **MP gain by skill id (type 91, complete) and the inert readers (89, 90, 98, 99, 100).**
   - Where: any field or dungeon, with your skills equipped.
   - Paste, first (puts all six buffs on you):
     ```
     local u = g_pX2Game:GetMyUnit() for _, f in ipairs( { 363, 364, 365, 366, 367, 368 } ) do u:SetBuffFactorToGameUnitByBuffFactorID_LUA( f, 1 ) end
     ```
   - Paste, second (asks every reader what it answers):
     ```
     local u = g_pX2Game:GetMyUnit() u:JuneBuffTestNewSkill_LUA( 89, 0, 0, 2 ) u:JuneBuffTestNewSkill_LUA( 90, 0, 0, 2 ) u:JuneBuffTestNewSkill_LUA( 91, 31012, 0, 2 ) u:JuneBuffTestNewSkill_LUA( 98, 0, 0, 2 ) u:JuneBuffTestNewSkill_LUA( 99, 0, 0, 2 ) u:JuneBuffTestNewSkill_LUA( 100, 0, 0, 2 )
     ```
   - What it does: gives you six test buffs and then writes into the log what each of the new "by skill" readers says for every skill you have equipped. **Nothing changes on screen**: 89, 90, 98, 99 and 100 are not wired into the game yet (Phase 6b), and 91's only live attack is one monster-side effect (skill 31012).
   - Look for: one `first apply BBT n` line each for 89, 90, 91, 98, 99 and 100, then the `reader` lines. Per equipped skill (`type` is its skill type):
     - `reader BBT 89 x 90 … GetSkillDamageRateByBuff`: 1.5 for a skill of type 6 or 7, ×1.3 more for skill ids 31011 to 31014, else 1.
     - `reader BBT 98 … GetSkillDamageRateBySlotType`: 1.2 for a skill in the slot A row (the first four), 1 for slot B skills and hyper skills.
     - `reader BBT 99 … MP cost rate from 1`: 0.5 for type 6 or 7, else 1.
     - `reader BBT 100 … cool time rate from 1`: 0.9 and `applied 1` for type 6 or 7, else 1 and `applied 0`.
     - `reader BBT 91: skill 31012 GetChargeMpRateBySkillID = 0`.
   - If not: a missing `first apply BBT n` means that factor was not applied (look for `ordinal reject`, or a `[JUNE_BUFF]` line naming the type or its BTI 990nnn); a `reader` value of 1 where this list says otherwise is the one thing to send back, with the skill name and the line. A popup or crash: send the `[JUNE_BUFF]` lines.
   - Undo: wait 60 seconds.

4. **Direct store writes (types 75, 89, 90, 91, 103), optional.**
   - Where: any field or dungeon, **after a fresh launch and before the setup lines**, or at least 60 seconds after check 3. A live test buff from check 2 or 3 changes the answers (89 would read 2.25 instead of 1.5, and an expiring 369 makes the game rebuild the 103 table, which wipes the written entry), and the one-shot `first read` lines are used up by whichever read comes first; judge this check by the `reader` lines only.
   - Paste (writes one entry into each store and reads it back, then takes it out again):
     ```
     local u = g_pX2Game:GetMyUnit() u:JuneBuffTestNewSkill_LUA( 91, 31012, 0.5, 1 ) u:JuneBuffTestNewSkill_LUA( 91, 31012, 0.5, 0 ) u:JuneBuffTestNewSkill_LUA( 103, 6, 2, 1 ) u:JuneBuffTestNewSkill_LUA( 103, 6, 2, 0 ) u:JuneBuffTestNewSkill_LUA( 89, 6, 1.5, 1 ) u:JuneBuffTestNewSkill_LUA( 89, 6, 1.5, 0 )
     ```
   - What it does: stores and then removes one entry per type, with no buff involved.
   - Look for: `console hook BBT 91, a 31012, b 0.5, insert` followed by `reader BBT 91: skill 31012 GetChargeMpRateBySkillID = 0.5`, then the same with `erase` and `= 1`; likewise 103 (`skill type 6 cool time speed = 2`, then `= 1`) and 89 (`GetSkillDamageRateByBuff = 1.5` on type-6 skills, then 1).
   - If not: a `= 1` after an insert, or an `= 0.5` after an erase, means the store or the erase is wrong; send the lines.

**Log lines to read afterwards** (this phase's own, `ErrorLog.txt` in the game directory): `[JUNE_BUFF] first apply BBT 75 / 89 / 90 / 91 / 98 / 99 / 100 / 103 …`, `first read BBT 75`, `first read BBT 91`, `first read BBT 103`, `console hook BBT …`, the `reader BBT …` lines, and any `[JUNE_BUFF]` line naming one of the eight types or the ids 362 to 369. Nothing else is in scope.

### Phase 6 — Review (2026-10-04, Opus 5.5 high)

**Verdict: the transcription matches June except in three places, now fixed. The map (item 1) was wrong in one of them. The port had restructured June's order in the other two.** All three fixes sit in code `SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL` already owns, so there is no new flag and no `#else` (the replaced lines are this phase's own). Rebuilt and deployed, **not launched**. Fix 1 reproduces a June bug, so the user can veto it (below).

**Your call (veto point): fix 1 copies a June bug on purpose.**
- In June, a 90 or 91 factor with two or more `SKILL_CATEGORY` blocks gives every skill id it lists the rate of the **last** block. The reason: June never clears the id list between blocks, and the getter takes the last match.
- The port now does the same, under the standing "June behaviour wins" rule.
- It changes nothing on June's own data: all five 90/91 factors in `june_src_fixed/data036_kom/BattleFieldBuffFactor.src.lua` have exactly one block. Only custom or test factors with several blocks see it.
- To undo it: put back the line `kBlock = CX2SkillIDRateByBuff::SkillIDRateBlock();` in `JuneBuffReadSkillIDRateBlocks` (`X2BuffBehaviorTemplet.cpp`).
- **Decision (user, 2026-10-04): keep it.** June's behaviour stays: in a multi-block 90/91 factor, the last skill block wins.

**Spot-checks (Ghidra read-only; uncarved functions read with `buffre/rawfn.py` and `disasm_range.py`).**
1. **89, `SetFactor` `32e7a9e0` and slot 7 `32e7c020`: arithmetic matches.**
   - At the cap it returns true before the user gate.
   - `m` is 1 when `lim < 2` or the multiplier is 0.
   - The restack merge is `old += v·m − m`.
   - Slot 7 is `base = (c·m + v) / (c·m + 1.0)` (constant `[0x334355b0]` = 1.0, read from the PE), then `c − 1` passes of `res += base·m − m`, and `c < 2` keeps `base`.
   - One difference: June collects unmatched pairs in a temporary vector and appends them **after** the loop. That became fix 2.
2. **99, `SetFactor` `32e7b0d0`, and the readers `3203a250` / `3203a420`: match.**
   - The restack merges only on the same (skill type, change type) pair, and only for FIX_VALUE or PERCENT. A triple with no match is **dropped**, unlike 89.
   - `applied` starts at 0, becomes 1 on a skill-type match, and drops back to 0 on an unknown change type, so the last matching triple decides.
   - The result is floored at 0 only when the skill has a templet.
3. **90, `SetFactor` `32e849c0`: failed.**
   - The erase fires only when `count == 0` (unsigned `ja` skips on > 0), as mapped.
   - The `-1.0` test is exact (`fucompp`, `jp`), as mapped.
   - The id list is **not** reset between blocks. That became fix 1, so the whole phase was re-checked.

**Whole-phase re-check against the binary.** Every function the phase ports was read; only the three fixes below differ.
- **75:**
  - Parse `32e75360` (`BUFF_CHANGE_TYPE`, default 0).
  - `SetFactor` `32e77be0` (`ct == m_eChangeType && ct == 3`, else false with no popup; erase before re-insert on a restack; `rate = size>1 ? v[1] : 0`).
  - `GetFactor` `32e7e9f0` `[ct, rate]`; packet `32e754f0`; `DoFinish` `32e75540` (no user gate).
  - Insert `32281b00` (no duplicate test), erase `32279d20`, reader `32275050`.
- **89:**
  - `GetFactor` `32e7ecf0` `[type, rate]*`.
  - Packet `32e7af60` (appends, then inserts for a user); `DoFinish` `32e75b70`.
  - Insert `31fe3b30` (only when absent), update `31fe3c10` (erase, then insert).
  - Getter `32369a30` (first match, floored at 0, else 1); reader `31fd7cc0`.
- **90 / 91:**
  - `SetFactor` `32e849c0` / `32e84d70`; packets `32e84b90` / `32e84f40`.
  - `GetFactor` `32e7ef50` / `32e7f2e0` (identical shape: rate `fld`, ids `fild`, then `-1.0`).
  - `DoFinish` `32e75b90` / `32e75bb0` (user gate); inserts `3200ae70` / `3200af50` (same shape, lists `+0x92e4` / `+0x92f4`).
  - Getters `32369aa0` / `32369b10`: byte-identical, and the **last** block that lists the id wins.
  - 91 reader `31fd7df0`; slot 109 `31fd7ea0` (`pDD ? 31fd7df0( pDD->+0x5c4 ) : 1`); base `3208dd70` (`fld1`).
- **98:**
  - Parse `32e75d20`.
  - `SetFactor` `32e78080` (a negative slot type returns false; `rate = size>1 ? v[1] : 0`; no unit call).
  - `GetFactor` `32e7f890`; packet `32e78170`; `DoFinish` `32e75cc0` (erases from 89's list).
  - Reader `32039e00` (skill type 13 returns 1; `ROUND(bf[0])`).
  - Slot map `3214f3d0`; lookup `3288fff0`. The lookup's order is fix 3.
- **99 / 100:** `SetFactor`, `GetFactor` and packet are byte-identical pairs, and the shared `DoFinish` `33093ad0` is empty.
- **103:**
  - `SetFactor` `32e7bb50`: user only, merges PERCENT only, and sets the dirty flag on every success path.
  - `DoFinish` `32e75ea0`.
  - Rebuild `32039fb0`: every key reset to 1.0, then multiply or insert per PERCENT triple.
  - Reader `31fd8bc0`: the map entry or 1.0. Local-player wrapper `32896850`.
  - Host `32896930`: `left -= 32896850( id ) * dt` on both rows.
- **Parses:**
  - `32d49460` / `32d49810` / `32d49da0` / `32d4a370` / `32d4aa30` / `32d4b020` / `32d4b610` have the shapes the Done note gives.
  - 90's was read in full: rate > 0; ids read while ≥ 0; an empty list rejects. The id vector **is** reset per category at load, so the accumulation is runtime-only.
- **Hosts:**
  - User `DamageReact` @`32048da7`: `attackType != 1 && attacker && 32275050`.
  - NPC `DamageReact` @`327b3ac7`: the same, plus `!+0x4dc` (`m_bNoDamage`).
  - Each block is its own roll, after 56's and before the 8021/8025/8026 rule. 56 itself was landed by 4b-i part 2 and is unchanged here.
  - 91 host @`32299510`: vtable `+0x1b4` (slot 109), reached from the non-command branch (`jne` at `32299460`), so it applies to every attack type. March's function returns early on a null attacker, so the port's call is guarded.

**Fixes (all `SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL`).**
1. **90 / 91 ids accumulate across blocks** (`X2BuffBehaviorTemplet.cpp`, `JuneBuffReadSkillIDRateBlocks`, used by both `SetFactor`s and both packet readers). The `kBlock` reset after a `-1.0` is gone.
2. **89 restack append order** (`CX2BuffChangeSkillDamageBehaviorTemplet::SetFactor`). New pairs are matched only against the pairs held before the restack; unmatched ones are appended after the loop. Before the fix, a later pair of the same factor could merge into a pair appended earlier in the same call. June keeps both, and the getter returns the first.
3. **98 slot lookup order** (`CX2UserSkillTree::GetEquippedSkillSlotType`). It now checks A[i] and then B[i] for each i, as `3288fff0` does. This decides only when one skill sits in both rows. 98 stays inert until 6b.

**Recorded, not changed.**
- **Rounding of factor ints.** June converts factor floats with `_ftol` (`32fe25f0`, truncation) in the 99/100 readers and the 103 rebuild, and with `ROUND` in 98's reader. The port adds 0.5 and casts. They agree on the exact integers every factor carries (written by `static_cast<float>( int )`).
- **Packet readers don't clear the member.** June's 89/90/91/99/100 packet readers append to the member without clearing it; the port clears. The packet path always runs on a fresh clone (`X2GameUnit.cpp` `ptrCloneBuffTemplet->SetFactorFromPacket`), so nothing differs.
- **Parse failures.** June's parsers do not call `EndTable` on a failure path; the port does. This was already in the Done note.

**Review-list items.**
- **Enums:** `X2BuffFactor.h` 75, 89, 90, 91, 98, 99, 100, 103; BCT 0 / 1 / 3.
- **`GetFactor` / packet float order:** matches for every type (above).
- **103:** declares no `SetFactorFromPacket` (base is empty, as June's slot 13). The lazy rebuild gives the same table as June's per-frame one.
- **91's host:** unconditional (above).
- **No 6b host was edited.**
  - The commit's source hunks are only: the `CX2GUUser` ctor; both `DamageReact` placeholders; `SetDamageExceptionProcess`; `CX2UserSkillTree::OnFrameMove`; and pure insertions of new functions.
  - `X2Game.cpp`, `DoStateEndStart`, `GetActualMPConsume`, `GetActualCoolTime` and the per-class cooldown setters are untouched.
  - The five inert readers have no callers except each other and the DEBUG console hook.
- **Census at HEAD:** `buff_census.py --phase 6` lists none of the eight, and March handles 98 behavior types.

**Hygiene.**
- **Phase 6 commit `54eaef6`:**
  - `file` is identical before, after and in the working tree on all 14 touched sources.
  - 1,842 insertions and 2 deletions (the placeholder comments); 0 non-ASCII bytes added.
  - The 63 LF-only added lines are all in the LF regions of `X2UserSkillTree.cpp` / `.h`.
  - The Done note's "five replaced lines" is corrected in place.
- **Review fix:**
  - `X2BuffBehaviorTemplet.cpp` +10/−2, `X2UserSkillTree.cpp` +5/−7.
  - Patched at the byte level; `file` unchanged on both (CP949 + CRLF, and LF in the skill-tree region); 0 non-ASCII bytes.

**Build and deploy.**
- `X2Lib` and `X2` at `US_SERVICE`, rc 0, with both edited `.cpp` files in the compile list. `KTDXLIB` was not rebuilt: no header changed.
- The one C4018 in `X2UserSkillTree.cpp(503)` is pre-existing and outside the edit.
- Deployed `X2/US_SERVICE/x2.exe` as `X2_offline.exe`: 19,090,432 B, md5 **`f413b8bf1cb07b51ea3cdd6887e99638`**, 18:41:33. The names were read programmatically (`x2.exe`, `X2_offline.exe`), and the copy is byte-identical to the build. The replaced exe was `cb9d9015…`. **Not launched.**

**Not shown in play (read-verified only).**
- Fix 2 needs an accumulating 89 templet restacked by a different factor.
- Fix 3 needs one skill equipped in both rows, and 98 is inert until 6b.
- 91's live host path (`SetDamageExceptionProcess` → the slot-109 virtual) is read-verified. No March player attack carries a non-zero `APPLIED_SKILL_ID`, so it can only be seen on the one monster-side effect; the review's check below shows the reader and fix 1. A DEBUG one-shot inside the override would prove the dispatch; that is the user's call, and it was not added.
- 75's user side (a monster's hit on you) was not covered by the Phase 6 play-test, which only had you hitting monsters. Check 1 below covers it.

**Play-test (2026-10-04, user; exe `f413b8bf…`, run started 18:47:56 after the 18:41 deploy; `ErrorLog.txt` and `offline_server.log` read afterwards).**
- **Load:** the baseline as before: `factors 361 loaded`, 5 ordinal rejects (481–483, 372, 373), 1 duplicate BTI (87). No `ordinal reject BFI 362` or `363`.
- **1 (75, the player side): passed** (user). `ErrorLog.txt:174` `first apply BBT 75 … BTI 990075, rate 1`, then `:175` `first read BBT 75: the attacker's double-attack roll succeeded (rate 1)` after a monster's hit.
- **2 (91, fix 1): passed** (logs). `:176` `first apply BBT 91 … 2 block(s), first rate 0.5`; `:179` `skill 31012 … = 2` and `:181` `skill 31013 … = 2`. Before the fix, 31012 would have read 0.5. Re-read at 18:54:44: both still 2.
- **2b (spawn the 31012 effect): ran, not conclusive.**
  - The effect was spawned twice (18:54:36, :39). As predicted, the mesh is missing: `ErrorLog.txt:182` `OpenXSkinMesh … RHB_LOCKON_STRATOS_ATTACK.X`, and `offline_server.log:120-127` `no XMesh templet` / `effect not live`. No crash.
  - The one-shot `first read BBT 91` had already fired at 18:49:13 from the console read (`:178`), which was pasted first. So the spawn could not print it, and the live host path is still read-verified only. It is neither shown nor failed.
  - To show it: fresh launch, paste the 363 setup line, apply 363, then spawn, with no read line before the spawn.
- **2b rerun (fresh launch 18:57:10; the 91 factor renumbered to BFI 362, since 362 is the first free id; no read line pasted): passed.**
  - `ErrorLog.txt:173` `first apply BBT 91 … 2 block(s), first rate 0.5`.
  - At 18:57:40 the spawn: `:174` `OpenXSkinMesh … RHB_LOCKON_STRATOS_ATTACK.X`, then `:175` `first read BBT 91: skill 31012 MP gain x 2`, in the same second.
  - No console read ran in this launch, so that line came from the live path: `SetDamageDataFromLUA` → `SetDamageExceptionProcess` → the slot-109 virtual `GetChargeMpRateByBuff` → the store. 91's host is now shown, not only read-verified.
  - No ordinal reject for 362; the load summary is the usual baseline.
  - The `JUNE_FX unsupported APPLIED_SKILL_ID … (31012/0)` line at `offline_server.log:125` is a diagnostic from the effect port (`X2DamageEffect.cpp:1637`). It logs the effect's own copy of the key and clears nothing in `m_DamageData`.

**Play-test hand-off (dev console, END key; `settings.auth_level` = 3 is already set).**

How the lines were checked:
- **Syntax and values.** All five lines below were run under `elstools`' `luajit.exe` against a mock manager. They define templets 990075 and 990091 and factors 362 and 363 with the values named; the monster loop skips an empty slot.
- **Not run in the game.** The setup lines are the Phase 6 hand-off's 362 and 365 lines; only the 91 factor's id is changed (363) and a second block is added. The monster loop has run before (Phase 5b's 79 check).
- **Fresh launch required.** The last load summary logged `factors 361 loaded`, so the free factor ids are 362 and 363. Both one-shot log lines (`first apply BBT 75` / `91`, `first read BBT 75` / `91`) are also used up by whichever path runs first after launch.

0. **Setup (once, right after loading in; paste both, in this order).**
   - **Where:** a field or dungeon with ordinary monsters, after a fresh launch.
   - **Paste,** factor 362 (monsters double their plain hits for 60 s):
     ```
     local t = {} local r = {} local p = {} for i = 1, 30 do t[i] = 60 r[i] = 1 p[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990075, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 75 }, BBT_DOUBLE_ATTACK_BUFF = { BUFF_CHANGE_TYPE = 3 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 362, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990075, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 75 }, BBT_DOUBLE_ATTACK_BUFF = { BUFF_CHANGE_TYPE = 3, PERCENT = p } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - **Paste,** factor 363 (a two-block MP-gain factor: ×0.5 for skill 31012, ×2 for skill 31013):
     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990091, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 91 }, BBT_CHARGE_MP_HIT_BY_SKILL_ID = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 363, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990091, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 91 }, BBT_CHARGE_MP_HIT_BY_SKILL_ID = { SKILL_CATEGORY_0 = { MULTIPLY_MP_CHARGE_RATE = 0.5, SKILL_ID_LIST = { 31012 } }, SKILL_CATEGORY_1 = { MULTIPLY_MP_CHARGE_RATE = 2, SKILL_ID_LIST = { 31013 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - **What it does:** teaches the game the two test buffs. Nothing changes on screen yet.
   - **Look for:** no popup, and no `ordinal reject BFI 362` or `363` line in `ErrorLog.txt`.
   - **If not:** an `ordinal reject … the next id must be N` line means the ids are off. Tell me N, or restart and paste both again.

1. **Monsters' plain hits on you double (type 75, the player side).**
   - **Where:** the same field or dungeon. Let a monster hit you once or twice first and note the usual damage number. Keep your HP comfortable; the hits will double for a minute.
   - **Paste:**
     ```
     for i = 0, g_pX2Game:GetNPCUnitListSize() - 1 do local n = g_pX2Game:GetNPCUnit( i ) if n ~= nil then n:SetBuffFactorToGameUnitByBuffFactorID_LUA( 362, 1 ) end end
     ```
   - **What it does:** for 60 seconds every monster on the map lands each plain (non-special) hit on you twice as hard.
   - **Look for:** the damage numbers of monster hits on you are about double, in the red "extra" look.
     - `ErrorLog.txt` gets `first apply BBT 75 (double attack buff): BTI 990075, rate 1`.
     - After the first hit, it also gets `first read BBT 75: the attacker's double-attack roll succeeded (rate 1)`.
   - **If not:**
     - A `first apply` line but normal damage and no `first read BBT 75` means the player-side block did not run. Tell me "no first read", and which monster hit you.
     - No `first apply` line means the buff did not land; send any `[JUNE_BUFF]` line naming `990075`.
   - **Undo:** wait 60 seconds.

2. **Two MP-gain blocks: the last one wins for both skills (type 91, review fix 1).**
   - **Where:** anywhere in the same session, after the setup.
   - **Paste, first** (puts the buff on you):
     ```
     g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 363, 1 )
     ```
   - **Paste, in between** (added after the review, at the user's suggestion; checked from source and under LuaJIT, never run): spawns the one March damage effect that carries skill 31012, with you as its owner, so the real host runs:
     ```
     local u = g_pX2Game:GetMyUnit() local p = u:GetPos() g_pX2Game:GetDamageEffect():CreateInstance_LUA( u, "DamageEffect_SA_RHB_Lockon_Stratos_Attack", p, p.y )
     ```
     - **Why it works:** 91 is applied when the effect's damage data loads, not at the hit (`SetDamageDataFromLUA` calls `SetDamageExceptionProcess` under `MODIFY_SET_DAMAGE_DATA`, `X2DamageManager.cpp:593`). The owner is set first (`X2DamageEffect.cpp:1499`), and nothing returns early before the load (`:1763`). `CX2GUUser`'s override chains to the base.
     - **What you see:** probably nothing. The Rose mesh `RHB_Lockon_Stratos_Attack.X` is not in March's assets, so the effect dies at once (`m_bLive = false`), after its damage data has loaded. The `ASSERT( owner is an NPC )` in `CreateInstance_LUA` compiles to nothing in this build (`NDEBUG`).
     - **Look for:** `first read BBT 91: skill 31012 MP gain x 2` in `ErrorLog.txt`. That proves the live host path (`SetDamageExceptionProcess` → the slot-109 virtual → the store). It must come **before** the second paste, which would otherwise use up the one-shot line.
     - **If not:** no `first read BBT 91` line means the effect table is not in the packed `DamageEffect.lua`, or the host did not run; send any line naming `Lockon_Stratos` or `[JUNE_BUFF]`.
   - **Paste, second** (writes what the game holds for both skills into the log):
     ```
     local u = g_pX2Game:GetMyUnit() u:JuneBuffTestNewSkill_LUA( 91, 31012, 0, 2 ) u:JuneBuffTestNewSkill_LUA( 91, 31013, 0, 2 )
     ```
   - **What it does:** gives you a buff whose first block says "×0.5 for skill 31012" and whose second says "×2 for skill 31013", then logs the rate the game uses for each skill. Nothing changes on screen.
   - **Look for** in `ErrorLog.txt`:
     - `first apply BBT 91 (charge MP hit by skill id): BTI 990091, 2 block(s), first rate 0.5`.
     - Then `reader BBT 91: skill 31012 GetChargeMpRateBySkillID = 2` and `reader BBT 91: skill 31013 GetChargeMpRateBySkillID = 2`.
     - **2 for both is right.** June carries skill 31012 into the second block, and the last block wins.
   - **If not:**
     - `= 0.5` for 31012 means the old exe is still running. Check that you launched `X2_offline.exe` after 18:41.
     - `= 1` for either skill means the buff did not land; send any `[JUNE_BUFF]` line naming `990091`.

**Log lines to read afterwards** (this check's own, in `ErrorLog.txt` in the game directory):
- `[JUNE_BUFF] first apply BBT 75` and `first apply BBT 91`.
- `first read BBT 75` and `first read BBT 91`.
- `console hook BBT 91 … read` and the two `reader BBT 91` lines.
- Any `[JUNE_BUFF]` line naming 990075, 990091, 362 or 363.

## Retired inline kickoff prompts (2026-10-04)

The ready-to-paste prompts that stood in the plan for the then-unfinished phases, verbatim.
They were replaced by `kickoffs/june-buff/*.md`, which are the maintained versions.

### Kickoff prompt, Phase 6b

```
Continuing the June buff/debuff port. Run this session on Opus 5.5 at high effort.
Read JUNE_BUFF_PORT_PHASES.md in the repo root in full -- "Starting context", the
"Investigation ledger", the "June buff function map", "Phase 1c, item 1", the
"Phases 5-7" shared checklist, the Phase 6 Done note and "Phase 6b". Branch:
mods/june-buff-port. Never launch X2_offline.exe without asking first.

Your task (Phase 6b, group B hosts): wire the Phase 6 classes into June's host
functions: 89/90/98 into CX2GUUser::DoStateEndStart (June 3203c3a0; March
X2GUUser.cpp:6350) and the summoned-NPC power helper 322ece70 (March
CX2Game::SetUserSummonedNPCInfo, X2Game.cpp:14060), 99 into CX2GUUser::GetActualMPConsume
(June 3203cf60; March X2GUUser.cpp:21081), and 100 into the cooldown helper 3235a710
and its 16 callers. Default to porting June's WHOLE host function (no partial June
updates); if a whole port needs a system March lacks and cannot be finished here, stop
that host, write what it needs in the doc, and hand the decision to me. Open Ghidra at
the addresses "Phase 1c, item 1" gives; do not search the binary for a function the
doc lacks. Cite June addresses in comments; never invent a value or formula. Use flag
SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL with the original code in #else. Done means: each
of 89, 90, 98, 99, 100 has its host wired (or a written reason it does not), the build
is rc=0, and the strings are in the deployed exe. Write per-type "how to see it in
game" lines (a June BTI/BFI and where it comes from) into the Phase 6b Done note for
Phase 14, and note the 98 -> BFT 16 ordering for Phase 8.
Commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Review prompt, Phase 6b

```
Reviewing the June buff/debuff port, Phase 6b (fresh Opus 5.5 session, high effort).
Read JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", "Phase 1c, item 1",
the shared checklist and the Phase 6b Done note. Branch: mods/june-buff-port. Never
launch X2_offline.exe without asking first.

Review the Phase 6b commit(s): every host June rewrote (DoStateEndStart and the
power-rate cache, the summon helper and its 21 call sites, GetActualMPConsume, the
cooldown helper and its 16 callers) against "Phase 1c, item 1", including what the
flag-off (#else) path does compared with March, any deviation recorded, and
SERV_IRUHADEV_ / CP949 hygiene (run `file` on every touched file and check `git diff
--stat`; X2GUUser.cpp is a 37k-line CP949 source, and a diff of hundreds of lines for a
small change means the file was re-encoded). Spot-check 2-3 host sites directly in Ghidra
(read-only); if a spot-check fails, re-check the whole phase. Fix clear transcription
errors under the same flag; hand design decisions back to me. Write the findings into the
doc as "Phase 6b -- Review", and commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 7

```
Continuing the June buff/debuff port. Run this session on Opus 5.5 at high effort.
Read JUNE_BUFF_PORT_PHASES.md in the repo root in full -- "Starting context", the
"Investigation ledger", the "June buff function map", "Phase 1c, item 2", the
"Phases 5-7" shared checklist and "Phase 7". Branch: mods/june-buff-port. Never launch
X2_offline.exe without asking first.

Your task (Phase 7, group C): implement these five June behavior types on March
classes: 73 BBT_IGNORE_STATE_GUARD (add the BTI 975 C++ enumerator), 94
BBT_IMMUNE_BUFF_DEBUFF (fill the Phase 4a stub IsImmuneBuffByBuffBehavior, June
322a80a0), 95 BBT_IMMUNE_NON_CONTROL_DEBUFF (fill IsImmuneNonControlDebuffByBuffBehavior,
June 3227b790; port DeleteNonControlDebuff 3229dcc0 as a unit member -- the Lua binding
is Phase 10's), 104 BBT_JUMP_IMPOSSIBLE and 105 BBT_IMMUNE_EDT. Open Ghidra ONLY at the
addresses "Phase 1c, item 2" gives; do not search the binary for a function the doc
lacks. Follow the 8-step checklist. Transcribe 94's two June quirks verbatim. Leave
105's enchant rate math to Phase 4b and say so. Cite June addresses in comments; never
invent a value or formula. Use flag SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG with the original
code in #else. Rose/Ain/Lu-Ciel cases are ported anyway (user decision). If a type needs
a design decision, stop it, write the question in the doc, and continue. Done means:
buff_census.py --phase 7 lists none of 73, 94, 95, 104, 105 (74, 77, 97, 101, 102 and
110 are Phase 7b), the build is rc=0, and the strings are in the deployed exe. Write a
per-type "how to see it in game" line (a June BTI/BFI and where it comes from) into the
Phase 7 Done note for Phase 14. Commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Review prompt, Phase 7

```
Reviewing the June buff/debuff port, Phase 7 (fresh Opus 5.5 session, high effort).
Read JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", "Phase 1c, item 2",
the shared checklist and the Phase 7 Done note. Branch: mods/june-buff-port. Never
launch X2_offline.exe without asking first.

Review the Phase 7 commit(s) for types 73, 94, 95, 104 and 105 against item 2: enum
values and the BTI 975 enumerator, identity-store insert/erase, the two stubs filled
with exactly June's body, 94's two verbatim quirks, 95's DeleteNonControlDebuff walk,
104's user and NPC gates, 105's reader and single-line host change, and SERV_IRUHADEV_ /
#else / CP949 hygiene (run `file` on every touched file and check `git diff --stat`).
Spot-check 2-3 types directly in Ghidra (read-only); if a spot-check fails, re-check the
whole phase. Fix clear transcription errors under the same flag; hand design decisions
back to me. Write the findings into the doc as "Phase 7 -- Review", and commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 7b

```
Continuing the June buff/debuff port. Run this session on Opus 5.5 at high effort.
Read JUNE_BUFF_PORT_PHASES.md in the repo root in full -- "Starting context", the
"Investigation ledger", the "June buff function map", "Phase 1c, item 2", the
"Phases 5-7" shared checklist, the Phase 7 Done note and "Phase 7b". Branch:
mods/june-buff-port. Never launch X2_offline.exe without asking first.

Your task (Phase 7b, group C hosts): implement 74 BBT_DETONATION_IMPOSSIBLE and
102 BBT_DETONATION_IMPOSSIBLE_GAGE_RESET (June's split of m_bNoDetonation into two
unit bytes, plus the three rewritten hosts DetonationEventProcess,
DamageReactStateChange and CX2GUNPC::CommonEventProcessNonHost with CAN_DETONATION),
77 BBT_INVINCIBLE (getter, CommonFrameMove block, and the check of March's 12 direct
bInvincible reads), 97 BBT_CHANGE_MAX_CHARACTER_ADD_RESOURCE (the CX2GameUnit slot-201
virtual and the Add/Rose/Luciel/Ara/Chung overrides), 101 BBT_IGNORE_HP_DECREASE
(virtual UpNowHp/SetNowHp user overrides; decode the two undecoded gates first) and 110
BBT_AWAKE_IMPOSSIBLE (June's TryHyperModeStateChange replacing the
ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE macro). 111 and 112 are enum-only: confirm the
census excludes them. Open Ghidra at the addresses "Phase 1c, item 2" gives; open further
functions only where the doc says something was not decoded. Follow the 8-step
checklist. Default to porting June's WHOLE host function (no partial June updates); if
that needs a system March lacks, stop that type, write what it needs in the doc, and hand
the decision to me. Cite June addresses in comments; never invent a value or formula. Use
flag SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG with the original code in #else. Rose/Ain/Lu-Ciel
cases are ported anyway (user decision). If the session grows long, finish 74/102 and 77
first, commit, and record where the rest resumes. Done means: buff_census.py --phase 7
lists nothing but the enum-only 111 and 112 (or says which types are deferred and why),
the build is rc=0, and the strings are in the deployed exe. Write a per-type "how to see
it in game" line (a June BTI/BFI and where it comes from) into the Phase 7b Done note for
Phase 14. Commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Review prompt, Phase 7b

```
Reviewing the June buff/debuff port, Phase 7b (fresh Opus 5.5 session, high effort).
Read JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", "Phase 1c, item 2",
the shared checklist and the Phase 7b Done note. Branch: mods/june-buff-port. Never
launch X2_offline.exe without asking first.

Review the Phase 7b commit(s) for the types that landed (74, 77, 97, 101, 102, 110)
against item 2: the m_bNoDetonation split (every write site of the two bytes, the
charge-zeroing behavior change in DetonationEventProcess), the 77 getter and
CommonFrameMove block and what was decided about the 12 direct bInvincible reads, 97's
base virtual and per-class overrides, 101's virtual HP setters and their extra gates, 110's
rewritten consumer, and SERV_IRUHADEV_ / #else / CP949 hygiene (run `file` on every
touched file and check `git diff --stat`; X2GUUser.cpp and X2GUNPC.cpp are large CP949
sources, and a diff of hundreds of lines for a small change means the file was
re-encoded). Because every one of these touches a state machine every unit runs, read the
#else branches and confirm the flag-off path equals March. Spot-check 2-3 types
directly in Ghidra (read-only); if a spot-check fails, re-check the whole phase. Fix clear
transcription errors under the same flag; hand design decisions back to me. Write the
findings into the doc as "Phase 7b -- Review", and commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 8

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger" (March runtime map: finalizers
poll counters synced over UDP), the "June buff function map" and "Phase 8". Branch:
mods/june-buff-port. Never launch X2_offline.exe without asking first.

Your task (Phase 8): port June's BFT_AURA, BFT_STICK_SHAKE,
BFT_USE_SKILL_BY_SKILL_SLOTTYPE and BFT_ATTACK, from parse to finalizer class, unit
counter and increment site. Take the hook sites from June's binary; the doc lists March
candidates to confirm. Sync the new counters over UDP the same way March syncs
hit/hitted count (X2UDPPacket.h:310), and note the peer-build requirement in MODS.md.
New flag SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER. Done means buff_census.py lists no
BFT, the build is rc=0, and the deploy is done. Commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 9

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger" (the 17 unhandled custom-function
BTIs), the "June buff function map" and "Phase 9". Branch: mods/june-buff-port. Never
launch X2_offline.exe without asking first.

Your task (Phase 9): port June's whole CX2GameUnit::CreateAndInsertCustomFunctionByBuff
(March: X2GameUnit.cpp:13448, 3 cases) and every CX2CustomFunctionByBuff* class and
hook body it needs, including the Rose/Ain/Lu-Ciel cases (user decision: port now).
Where June relocated March's per-class FLASH_BANG/PANDEMONIUM_FEAR handling, follow
June. New flag SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION. If this is too large, finish one
character family, commit, and record where 9b resumes. Build, deploy, commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 10

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger" (June-only bindings and keys),
the "June buff function map" and "Phase 10". Branch: mods/june-buff-port. Never launch
X2_offline.exe without asking first.

Your task (Phase 10): port June's buff-group system and the June-only buff script
bindings. Load UnitApplyBuffGroup.lua through AddBuffGroup_LUA/AddOfficiallyBuffList_LUA,
replacing March's hard-coded officially-debuff list if June did. Bind
SetMonsterBuffGroupID, DeleteNonControlDebuff, GetBuffAccumulationCountNow,
ApplyBuffFactor and AddRandomBuffList (plus the other two if June's client uses them).
Read the APPLY_LEARNED_SKILL_*, BUFF_FACTOR_ID_ULTIMATE/_PVP, DELETE_BUFF_WHEN_DIE,
DUMMY_BUFF_TEMPLET_ID, IS_AURA_BUFF, LIMIT_APPLY_BUFF_COUNT_EACH_UNIT and
CHECK_RIDINGPET_SPACIAR_BUFF keys where June reads them (this unblocks effect-plan 8b).
New flag SERV_IRUHADEV_JUNE_BUFF_GROUP. Stage UnitApplyBuffGroup.lua (mar-2014 compile)
in march2014/update/data036 AND kom_contents/data036_kom + _manifest.json. Build,
deploy, then hand the pack to me. Commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 11

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger" (premium buffs) and "Phase 11",
plus CLAUDE.md's Offline-mode and live-DB rules. Branch: mods/june-buff-port. Never
launch X2_offline.exe without asking first.

Your task (Phase 11): bring the premium/world buff path (X2PremiumBuffManager,
BuffTemplet.lua) to June: port the parse/apply diff, add NOT_USE_IN_PVE, and stage
June's BuffTemplet.lua (mar-2014 compile). Then establish how an item-granted buff
reaches a unit offline. If it depends on server packets the emulator leaves UNHANDLED,
implement them in X2Lib/Offline. If that needs a stored procedure or table, ask me for
it from the live DB rather than reading DataBase/. Flags:
SERV_IRUHADEV_JUNE_BUFF_PREMIUM for client code, SERV_IRUHADEV_OFFLINE for the
emulator. Build, deploy, then hand the pack to me. Commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 13

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context" (pairing hazard), "The exit metric" and "Phase 13". Branch:
mods/june-buff-port. Never launch X2_offline.exe without asking first.

Your task (Phase 13): adopt June's BattleFieldBuffTemplet.lua and
BattleFieldBuffFactor.lua (and _INT per Phase 1's finding), taking the sources from
june_src_fixed/data036_kom/*.src.lua (already decompiled+fixed; do not re-decompile).
Compile with elstools -v mar-2014 and stage into march2014/data036 and kom_contents/data036_kom + _manifest.json.
buff_census.py must report 0 unsupported first. Build and deploy the exe, then give me
ONE pack list covering every file staged in Phases 2/10/11/12/13. After I pack, verify
the live koms with elstools inspect. Then ask me before launching, and check ErrorLog.txt
for the [JUNE_BUFF] load summary (0 placeholders, no unsupported lines). Commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 14

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context" and "Phase 14", including every per-type "how to see it"
line from Phases 5-9. Branch: mods/june-buff-port. Never launch X2_offline.exe without
asking first; ask before each launch.

Your task (Phase 14): play-test the port with me. For each group (stat, skill, flag and
immunity, finalizer, custom-function, mob-applied, premium), tell me exactly what to
do in game, then read offline_server.log and ErrorLog.txt for the [JUNE_BUFF] evidence.
Fix confirmed defects under the owning phase's flag. Then disable
SERV_IRUHADEV_JUNE_BUFF_DEBUG, update MODS.md and JUNE2017_CHARACTER_PORT_CHECKLIST.md,
mark the doc's phases done with dates, and commit.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 15

```
Continuing the June buff/debuff port's side phase. Read JUNE_BUFF_PORT_PHASES.md in the repo
root -- "Starting context", "Phase 4b-i, part 4" (its enchant note) and "Phase 15". Branch:
mods/june-buff-port. Never launch X2_offline.exe without asking first.

Your task (Phase 15, enchant system parity): do Phase 15's scope in order. Start with the RE
(ProcessEnchant* slots 214-219, the five new EnchantData fields' readers, g_pData+0x284) and
the EDT enum reconciliation with a full consumer audit, and stop to ask the user before
renumbering any EDT value March data depends on. Then port the EnchantData fields, the loader,
32272d90 and the process bodies under SERV_IRUHADEV_JUNE_ENCHANT_PARITY: June's whole
functions, byte-level CP949 edits, insert-only, flag-off unifdef check per file. Stage June's
AttribEnchantItem.lua (compile -v mar-2014, both staging dirs, manifest) and name it for the
user to pack. Build, deploy, commit; record where you stopped.
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Kickoff prompt, Phase 16

```
Continuing the June buff/debuff port. Run this session on Opus 5.5 at high effort.
Read JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", "Phase 5c -- Done"
(including its "Resolved" block and "Already decoded for 92") and "Phase 16". Branch:
mods/june-buff-port. Never launch X2_offline.exe without asking first.

Your task (Phase 16, socket-option parity): do step 1 (RE, read-only) first and record
verdicts in the doc, then steps 2-4: June's socket loader keys, the per-unit socket
aggregate, InitAdditionalStatOptionValue seeds and passive init, the refresh variants and
the option-data template 320839e0, June's SocketOptionForm.lua (staged for me to pack,
after the arity gate), then BBT 92 end to end and 78's three socket terms, plus the
designed attack range (10% of attack, both physical and magical, users only; see Q3) under
SERV_IRUHADEV_JUNE_ATTACK_RANGE_DESIGN. Flag SERV_IRUHADEV_JUNE_SOCKET_PARITY (92 stays
under NEW_STAT). Port June's whole function
wherever it differs; cite June addresses; never invent a value; byte-level CP949 edits;
flag-off unifdef per file; count the reach on March's live data before building. Build,
deploy, commit; write a Done note with per-item "how to see it in game" lines and
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)".
```

## Phase 11 — the March server's buff system (2026-10-05)

This is the source the emulator's item-buff route is transcribed from (user decision B, 2026-10-05). Paths are under `KncWX2Server/`.

The flag states below were settled by running `cl /EP` on `Common/ServerDefine.h` with `/DWIN32 /DNDEBUG /D_WINDOWS /DSERV_COUNTRY_US`.

### Flags

- **ON:**
  - `SERV_SERVER_BUFF_SYSTEM`, `SERV_REFORM_THE_GATE_OF_DARKNESS`, `SERV_2012_PVP_SEASON2_2`;
  - `SERV_NEW_DEFENCE_DUNGEON` (+ `_ENTER_LOGIC`, `_NO_USE_DEFENSE_BUFF_AND_DEBUFF`);
  - `SERV_EVENT_RURIEL_MANA_ENERGIZE_POTION`, `SERV_CHANGE_UPDATE_BUFF_INFO_PROCESS(_TEMP)`, `SERV_COME_BACK_USER_REWARD`;
  - `SERV_BATTLE_FIELD_SYSTEM`, `SERV_NEW_UNIT_TRADE_LIMIT`, `SERV_PVP_REMATCH`.
- **OFF:** `SERV_EVENT_ARA_NEW_CHAR_THE_ESSENCE_OF_HERETIC`, `SERV_PLAY_WITH_CHAR_PARTY_BONUS_EXP` (so `m_mapBonusRate` is not on the wire), `SERV_BONUS_BUFF_SYSTEM`.

### `KUserServerBuffManager` (`GameServer/UserServerBuffManager.cpp`)

**State.** `map<int, KBuffInfo> m_mapBuff`, keyed by BTI, plus `m_iInsertTime`. `KUserServerBuff` is dead code.

**`ActivateBuff(kBuff, out, tUseTime=0, iPeriod=0)`** (`:40-96`):
1. A missing templet → false.
2. **Already active → false**: no refresh, and no change to the stack count.
3. Start = now (or `tUseTime`), in epoch seconds as `__int64`.
4. End:
   - `iPeriod==0` → `m_fBuffPeriodSecond==0 ? start : start+(int64)m_fBuffPeriodSecond`;
   - otherwise `start+iPeriod`.
5. Insert, then push the full `KBuffInfo` to `out`.

A server-made `KBuffInfo` carries only the BTI, `m_iFactorID` (the templet's first `BUFF_FACTOR`, `KGSUser::ActivateBuff` GSUserFunction.cpp:17110) and start/end. Everything else is default.

**Other members:**
- `DeactivateBuff`: not active → false; otherwise erase and push the id.
- `GetBuffList(vec, iRoomType=RT_NONE)` (`:152-176`):
  - With a room type, a buff is kept only if that type is in the templet's `USING_BUFF_ROOM_TYPE` (`CheckRoomType`, XSLBuffManager.cpp:140-150). An empty list means the buff is never in a room list.
  - Without a room type it returns every active buff.
  - Server room types: `RT_PVP 0`, `RT_DUNGEON 1`, `RT_BATTLE_FIELD 7`.
- `CheckBuffPeriod` (`:272-303`):
  - every 1 s, once the state is past `S_SERVER_SELECT`;
  - a buff with `start != end` and `now > end` (strictly) is deactivated;
  - a period-0 buff never expires;
  - the room-type argument is unused.

**End flags.** These are the only readers.

| Flag | Where | Trigger |
|---|---|---|
| `m_bEndBuffAtTheLeaveDungeonRoom` | `OnLeaveDungeonRoom` (`:369-384`) | leaving a dungeon room (`ERM_LEAVE_ROOM_ACK`, leave-for-village, `SetRoomUID(0)`) |
| `m_bEndBuffAtTheEndDungeonGame` | `OnEndDungeonGame` (`:492-507`) | `ERM_UPDATE_DUNGEON_UNIT_INFO_NOT` (dungeon end) |
| `m_bEndBuffAtTheLeaveBattleFieldRoom` | `OnLeaveBattleField` (`:436-441`) | battlefield leave |
| `m_bEndBuffAtTheLeavePvpRoom` | `OnLeavePVPRoom` (`:571`) | PvP room leave |
| `m_bEndBuffAtTheUserDied` | `OnUserDied` (`:534`) | `ERM_USER_UNIT_DIE_ACK` |
| `m_bEndBuffAtTheEndPvpGame`, `m_bKeepBuffAtTheUserLogOut` | none | parsed only |

The hard-coded cases:
- Rally of Hero 118-120 are always removed on dungeon leave.
- `REST_OF_RELLY` 121 depends on bad attitude.
- `FIELD_DEFENCE` 187 is removed on battlefield leave.
- `WARM_ENERGE_OF_EL` 122 is removed on field leave.
- PvP end/leave strips every buff whose templet lists `RT_PVP`.

### When the client is told

**`SendUpdateBuffInfo`** (GSUserFunction.cpp:17015-17093):
- In a PvP, dungeon or battle-field room, it sends `EGS_UPDATE_BUFF_INFO_IN_ROOM_NOT{ unit UID, room-filtered list, unbuffed KStat }`, broadcast to the room.
- Anywhere else it sends `EGS_UPDATE_BUFF_INFO_NOT{ unfiltered list }`.

**Callers:**
- The LoginServer ACK (`ELG_UPDATE_USER_UNIT_BUFF_INFO_ACK`, GSUserGameCommon.cpp:8065-8087). That ACK follows every REQ, so each REQ site is an indirect send:
  - item use, the expiry tick, unit select;
  - field join (122) and field leave;
  - room leave, dungeon end, PvP leave/end;
  - death, a Gate of Darkness revive;
  - party / come-back events.
- `EGS_GAME_LOADING_REQ` at 100 % (GSUserRoomCommon.cpp:1223).
- `ERM_UPDATE_BUFF_INFO_NOT` (game start, battle-field join, PvP intrude).
- `EGS_UPDATE_BUFF_INFO_NOT` is also sent directly after a successful `EGS_STATE_CHANGE_FIELD_REQ`, if the list is non-empty (GSUserField.cpp:215).
- The room/slot info carries the room-filtered list in `KRoomUserInfo.m_vecBuffInfo`.

### Item use

**Pre-checks** (`EGS_USE_ITEM_IN_INVENTORY_REQ`, GSUserInventory.cpp:4055). There is no village or room restriction.
- **Arcanum**, always: `STRENGTHENING_BODY` (117) active → `ERR_USE_ITEM_IN_INVENTORY_07` (`:4394-4400`). This is the only check on 117.
- **Arcanum**, `m_iTempCode != 0`: any of 109-112 active → `ERR_07`.
- **Elixir / Ruriel**, `m_iTempCode != 0`: any of 113-116, 191-198 or 1320 active → `ERR_06` (`:4436-4468`).

**Then:**
1. `OnUseItemInInventory` (GSUserFunction.cpp:7279-7400) runs `ActivateItemBuff` (`:7372`, return value ignored).
2. The REQ goes to the LoginServer, the ACK comes back, and `SendUpdateBuffInfo` runs.
3. Nothing quick-slot related is sent; quick-slot use never activates a server buff.
4. Gate: `IsBuffItem` = the item templet's `BUFF_FACTOR` first entry > 0 (XSLItemManager.cpp:2004, parsed from Item.lua at `:360`).

**`ActivateItemBuff`** (GSUserFunction.cpp:17117-17273). Item IDs are from Common/X2Data/XSLItem.h; BTIs from XSLBuffManager.h.

| Item | ID | BTI |
|---|---|---|
| ARCANUM_NUT / PTAH / RACHEL / ZADKIEL | 78520 / 78530 / 78540 / 78550 | 109 / 110 / 111 / 112 |
| ELIXIR_SERAPHIM / GEB / AMON / CRONOS | 78560 / 78570 / 78580 / 78590 | 113 / 114 / 115 / 116 |
| ELIXIR_GIANT_POTION … BIG_HAND_POTION | 78894 … 78901 | 191 … 198 |
| EI_BUFF_RURIEL_MANA_ENERGIZE_POTION | 60008058 | 1320 |

- **Arcanum group:** deactivate 109-112, activate the mapped buff, then activate 117.
- **Elixir group:** deactivate 113-116, 191-198 and 1320 (the Heretic pair 1318/1319 is OFF), then activate the mapped buff. There is no 117 in this group.
- Re-use: deactivate + activate in one REQ, which acts as a replace with a fresh period.

### Persistence

- The LoginServer keeps buffs in RAM only (`LoginServer/BuffManager.cpp`).
  - `P_GBuffeffect_UPD` never runs, because nothing fills its vector.
  - The 6-hour purge has no caller, and `DeleteUnitAllBuffInfo` has an inverted check.
- **On unit select** (GSUserFunction.cpp:5472-5661):
  1. A REQ is always sent.
  2. The ACK returns the stored buffs.
  3. `EGS_UPDATE_BUFF_INFO_NOT` follows.
- **On deselect** (`CheckDeselectUnitBuff` `:17295-17356`): 122, 118-120, 130-132 and 138 are always deleted; 213 and 133 conditionally.
- **Everything else** (Arcanum, Elixirs, 117) survives with absolute epoch end times. Time keeps running while logged out, and an expired buff is resent once, then removed by the next 1 s tick.

### Death and revive

- **Death:** `ERM_USER_UNIT_DIE_ACK` → `OnUserDied` removes `m_bEndBuffAtTheUserDied` buffs (Arcanum 109-112 in June's data), then REQ → ACK → in-room resend.
- **Revive:** only the Gate of Darkness continue path re-activates 189. Arcanum and Elixirs are not restored by the server. The client re-applies stored world buffs in `ReInit`.

**The server's own `BuffTemplet.lua` is not in the tree** (GSSimLayer.cpp:1112 loads it). The client's copy (March and June) uses the same keys and the `URT_*` room enum.
