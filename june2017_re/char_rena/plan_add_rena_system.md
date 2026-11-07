# ADD_RENA_SYSTEM census (read-only, 2026-11-07)

## 1. Defined under US_SERVICE?  NO (proven by preprocessor)
- Probe: `#include "stdafx.h"` (X2Lib; reaches KTDX.h / Always.h / InHouse*.h), then `#ifdef ADD_RENA_SYSTEM`, run with VS2010 `cl /EP` and the US_SERVICE
  defines (`WIN32;NDEBUG;_LIB;_USE_32BIT_TIME_T;_IN_HOUSE_;SERV_COUNTRY_US;CLIENT_COUNTRY_US;_HAS_ITERATOR_DEBUGGING=0;_CONVERT_VS_2010`) and X2Lib's include dirs.
  Output: `RENA_PROBE_UNDEFINED`, and in the same run `OFFLINE_PROBE_DEFINED` (SERV_IRUHADEV_OFFLINE), so Always.h really was read.
- Source: `KTDXLIB/Always.h:238` and `KTDXLIB/InHouse6.h:441` are both `//#define ADD_RENA_SYSTEM` (studio note 2013-01-02). Not in ServerDefine.h, not in tools/flagcheck.py.

## 2. Totals
23 files, 187 `#ifdef`/`#ifndef` blocks: Lire.cpp 111 (268 raw hits), Lire.h 9, 67 elsewhere. `#else` exists in 46 Lire.cpp blocks and 5 other blocks
(X2DamageEffect.cpp:5217, X2GUNPC.cpp:11404/11412, X2GUUser.cpp:10893/10901). 4 hits are inside `//` comments (X2BuffFactor.h).
All blocks are the studio's 2013 code. None has its own `SERV_IRUHADEV_` flag; local June-port flags sit beside or inside them (section 4).

## 3. Non-Lire sites (file:lines | what it gates | #else?)
CommonPacket.h (details in section 5):
- KncWX2Server/Common/CommonPacket.h:2055-2057 | enum member `KGamePlayStatus::CAC_NATURAL_FORCE` (last, after CAC_NP_AND_MUTATION_AND_FORMATION_MODE) | no
- CommonPacket.h:2134-2142 | `int KGamePlayStatus::GetNaturalForce() const` | no
- KncWX2Server/GameServer/GSGameDBThread.cpp:989-993 | server only (not built here): `case UT_LIRE: SetCharAbilType(CAC_NATURAL_FORCE)` | no

X2Define.h:
- :94-98 | consts MAX_NATURAL_FORCE_VALUE=5, NATURAL_FORCE_VALUE_PER_HIT=6, NATURAL_FORCE_BUFF=3 | no

X2DamageManager.h:
- :668-685 | enums `NATURAL_FORCE_TYPE` (NFT_NONE -1, COMBO 0, COMBO_BOMB 1, ACTIVE 2, SPECIAL_ACTIVE 3, RESULT_BOMB 4) and `FIRST_ATTACK_CHECK` (FAC_NOT_CHECK -1, NONE 0, FIRST_ATTACK 1) | no
- :1072-1086 | struct `DamageRelateSkillData { Byte m_byteSkillIndex, m_byteRelateData; Init() }` | no
- :1276-1278 | DamageData member `m_NaturalForceType` (enum) | no
- :1320-1323 | DamageData members `m_eFirstAttack`, `m_RelateSkillData` | no
- :1386-1388 | `#ifndef`: int `m_iNaturalForceType` (June +0x56c; local port) | n/a
- :1648-1651 | DamageData::Init: `m_eFirstAttack = FAC_NONE; m_RelateSkillData.Init()` | no
- :1663-1665 | `#ifndef`: `m_iNaturalForceType = -1` | n/a

X2DamageManager.cpp:
- :297-299 | SetDamageDataFromLUA reads `NATURAL_FORCE_TYPE` (LUA_GET_VALUE_ENUM) | no
- :314-322 | `#ifndef`: June int read of the same key plus unsupported-key log (local, JUNE_DAMAGEDATA); compiles out when flag on | n/a
- :1076-1086 | SetDamageDataFromLUA tail: `m_RelateSkillData = attacker user's GetNowDamageRelateSkillData()` | no
- :1673-1678 GroundHit, :1872-1877 AreaHit, :2254-2259 / :2272-2277 / :2299-2305 UnitToUnit, :3105-3110 / :3137-3142 EffectToUnit, :3565-3570 / :3582-3587 MeshToUnit, :3803-3808 / :3821-3826 CollisionDataToUnit | each marks first attack (hitUnitList empty and m_eFirstAttack != FAC_NOT_CHECK -> FAC_FIRST_ATTACK) | no
- :2192-2200 UnitToUnit, :3028-3036 EffectToUnit, :3496-3504 MeshToUnit | attacker user's `AdjustDamageDataBeforeDamageReact(pDamageData)` before DamageReact | no; the `#ifndef` twin at :2202-2206 / :3038-3042 / :3506-3510 is the local June vslot 0x300 call (Raven), mutually exclusive

X2DamageEffect.cpp (copy first-attack / RelateSkillData parent -> child effect):
- :3176-3180 OnFrameMove, :5349-5353 AttackResult, :7515-7519 CreateHitDamageEffect, :7894-7898 CreateDieDamageEffect, :10302-10306 SetDamageDataNext, :10357-10360 SetDamageDataNextByChainIndex, :10797-10801 CreateTouchLandDamageEffect | no
- :5217-5259 AttackResult RT_CREATE_DAMAGE_EFFECT | YES, `#else@5227` is the local June SERV_IRUHADEV_JUNE_SPAWN_INHERIT child sequence (its comment: "the ADD_RENA_SYSTEM branch above is not the live one"). Flag ON drops it.

X2EffectSet.cpp / .h:
- X2EffectSet.cpp:1272-1280 OnFrameMove_CreateEffect (instance -> effect copy) | no; :4912-4914 EffectSetInstance ctor init `m_eFirstAttack(FAC_NONE)` | no; X2EffectSet.h:830-833 instance members | no

X2GUNPC.cpp / .h:
- X2GUNPC.cpp:11404-11408 and :11412-11418 DamageReact: no HitCount++ for NFT_RESULT_BOMB | YES (else: plain HitCount++)
- X2GUNPC.cpp:32453-32491 AttackResultByType: NFT_ACTIVE / SPECIAL_ACTIVE first-attack, owner Rena consumes NF and spawns `NATURAL_FORCE_EXPL...` damage effect scaled by NF count | no
- X2GUNPC.h:3407-3410 `GetRelateSkillData()` (SetRelateSkillData is used at X2Game.cpp:17323; not re-checked where declared); :4777-4779 member `m_RelateSkillData` | no

X2GUUser.cpp / .h:
- X2GUUser.cpp:1715-1718 dtor, :1956-1959 Init, :2530-2533 ReInit | `m_AllDamageRelateSkillData.Init()` | no
- X2GUUser.cpp:10893-10897 and :10901-10907 DamageReact HitCount, inside the local BUFF_BEHAVIOR_PARITY #ifdef (flag-off arm uses literal `4` for NFT_RESULT_BOMB) | YES
- X2GUUser.cpp:29719-29740 debug overlay above a Lire unit ("NF: n", "NFP: f") | no
- X2GUUser.cpp:50491-50527 SetNowDamageRelateSkillData and helpers | no
- X2GUUser.h:1371-1408 `AllDamageRelateSkillData`, Get/SetNow..., virtual Check/DeleteDamageRelateSkillData, virtual AdjustDamageDataBeforeDamageReact | no
- X2GUUser.h:1410-1412 `#ifndef`: local virtual AdjustDamageDataBeforeDamageReact (June vslot 0x300, CHAR_RAVEN_LIVE) | n/a
- X2GUUser.h:5255-5257 member `m_AllDamageRelateSkillData` | no

Gauges (whole new classes, no #else):
- X2GageData.h:985-1044 `CX2RenaGageData : CX2GageData`; X2GageData.cpp:1271-1430 its methods
- X2GageUI.h:864-884 `CX2RenaMyGageUI : CX2MyGageUI` (InitNaturalForceUI / UpdateNaturalForceUI); X2GageUI.cpp:5119-5206 its methods
- X2GageManager.cpp:223-235 CreateMyGageUI: the 7 UC_LIRE_* classes get CX2RenaGageData + CX2RenaMyGageUI (flag off: fall to the default case)

X2Game.cpp:
- :5855-5865 KeyProcess: Lire `UpNaturalForce(GetMaxNaturalForce())` dev key | no
- :9534-9543 Handler_EGS_NPC_UNIT_DIE_ACK: owner Check/DeleteDamageRelateSkillData for the dying summon | no
- :17321-17324 SetUserSummonedNPCInfo: `pNPC->SetRelateSkillData( pUser->GetNowDamageRelateSkillData() )` | no

X2BuffFactor.h:450-452 and :683-685: `//#ifdef` already commented (section 4).

### Lire (X2GULire_ElvenRanger.cpp, 111 blocks) grouped; function names are the nearest preceding definition, approximate
| lines | blocks | gated |
|---|---|---|
| 278-406 (ctor, dtor, ReInit) | 3 | NF charge member init/reset |
| 797, 2697 | 2 | `SetNaturalForceCount(0)`; `GetNaturalForceCount() >= NATURAL_FORCE_BUFF` |
| 973-1046, 1853-1870, 2444-2547 (InitStateCommon / InitState) | 12 | StateStart registrations for NF-aware states; most have `#else` (March start function) |
| 4940-5105 (CommonStateStart, CommonFrameMove*) | 3 | NF hooks at state start, per-frame charge |
| 5725-6177 (Wait/Walk/Jump/Dash event processes) | 7 | NF charge on walk / dash |
| 6628-7064 (SNIPING_RANGER_DOUBLE_JUMP_ATTACK_X / XX) | 12 | new double-jump attack variant, `#else` = old transitions |
| 8210 | 1 | SiegeReady line |
| 8904-10372 (ComboZ1-Z4, ComboX1-X4, Up/Down) | 17 | NF-gain combo cancels with `#else`; `m_NaturalForceType = NFT_*` writes at 10061-10323 |
| 10395-11376 (dash combos, SNIPING dash X1-X3) | 12 | same pattern |
| 11545-12198 (dash-jump combos) | 14 | same; NFT_* writes 11941-12198 |
| 12265-13442, 15596 (power attack, RNW combos, REFLEX_MAGIC) | 9 | NF cancel conditions |
| 16524-16992 (LWS, LGA combos) | 13 | same; NFT_* 16928-16965 |
| 17913-18370, 20280-20586, 23207-23497 (RTR, Sniping X2-X4, HA skills) | 14 | same; NFT_* 20556 |
| 23576-24004 (GetNaturalForceBuff 23795, CreateGageData) | 3 | `SetBuffFactorToGameUnitByBuffFactorID(BFI_BUFF_NATURAL_FORCE)` @23772, Erase/Reserve `BTI_BUFF_NATURAL_FORCE` @23776 / @23846, gauge creation |
- 18 `m_NaturalForceType = CX2DamageManager::NFT_*` writes. June's `Lire_ElvenRanger.src.lua` has 77 `NATURAL_FORCE_TYPE` uses.
- X2GULire_ElvenRanger.h (9 blocks, none with `#else`): 174-176 enum value LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_XX; 530-548 ConsumeNaturalForce and NF methods; 561-577 AttackResultByType override; 799-806 NF members; 852-854 WalkStart; 874-876 DashStart; 920-924 DOUBLE_JUMP_ATTACK_XX_StateStart; 1180-1182 DashJumpComboX3StateStart; 1490-1492 LESI_LGA_ZZXX_StateStart.
- Lire.cpp: a local `SERV_IRUHADEV_` flag appears only near 4947-4951 (inside the 4940-4984 block).

## 4. Commented-out sites and local-port flags
- Commented out: X2BuffFactor.h:450-452 and :683-685. `BTI_BUFF_NATURAL_FORCE = 333` and `BFI_BUFF_NATURAL_FORCE = 355` are therefore always compiled (June Enum.lua 12867 / 13211 agree).
- Sites where a local June port shares the block (flipping the flag switches these arms): X2DamageManager.h:1386, :1663; X2DamageManager.cpp:314-322, :2202-2206, :3038-3042, :3506-3510; X2GUUser.h:1409-1413; X2GUUser.cpp:10890-10908; X2DamageEffect.cpp:5217-5259; X2GUNPC.cpp:11401-11419; plus SERV_IRUHADEV text within 20 lines of most X2DamageManager.cpp / X2DamageEffect.cpp / X2EffectSet.cpp blocks.

## 5. CommonPacket.h
- Exactly two edits, both in `KGamePlayStatus`: enum value `CAC_NATURAL_FORCE` (appended last; numeric value depends on the earlier #ifdef'd members) and accessor `GetNaturalForce()`. No new field, no layout change. The wire data is the existing `char m_cCharAbilType` + `int m_iCharAbilCount` (CommonPacket.h:2075).
- Reached before Always.h: per CLAUDE.md a flag there also needs `#define ADD_RENA_SYSTEM` in `KncWX2Server/Common/ServerDefine.h` (absent now) and an entry in `SECOND_HOMES` of tools/flagcheck.py (absent).
- Offline: nothing in X2Lib/Offline or X2OfflineDB names CAC_NATURAL_FORCE or GetNaturalForce. The emulator does fill KGamePlayStatus: X2OfflineServer.cpp:1224 `kOut.m_cCharAbilType = CX2OfflineStatTable::CharAbilTypeOf( unit class )`; X2OfflineStatTable.cpp:289-313 returns WSP / CANNON_BALL_COUNT / FORCE_POWER, else CAC_NONE (Lire gets CAC_NONE; the studio server GSGameDBThread.cpp:989 would give UT_LIRE CAC_NATURAL_FORCE); Handlers_Field.cpp:274-285 keeps m_iCharAbilCount per session. To use it the emulator needs a UT_LIRE case in CharAbilTypeOf and NF count save/restore (no DB column exists).

## 6. Quoted rows (file:line; truncated with ...)
- JUNE2017_CHARACTER_PORT_CHECKLIST.md:118 `| NATURAL_FORCE_TYPE | unsupported (parsed, logged) | 8c, done | Lire_ElvenRanger x77. See section 3 |`
- JUNE2017_CHARACTER_PORT_CHECKLIST.md:149 `Natural force | code exists, gated off: ADD_RENA_SYSTEM is off under US_SERVICE | Rena (Lire_ElvenRanger, 77 NATURAL_FORCE_TYPE uses) | a decision for you: turn ADD_RENA_SYSTEM on (the studio's own Rena code, untested in this build) or leave Rena without natural force.`
- JUNE2017_CHARACTER_PORT_CHECKLIST.md:206 `4. Decide on ADD_RENA_SYSTEM (section 3) before touching Rena.`
- JUNE2017_CHARACTER_PORT_CHECKLIST.md:959 `Rena: ApplyHyperModeBuff 32298590 @322985ec: unit type 3 -> 32cb50c0 count x 15.0 to vslot 0xf0, then 32cb6290( 5 ) (ADD_RENA_SYSTEM's ...`
- JUNE_PORT_BACKLOG_ORDER.md:236 `... Rena 32cb6290( 1 ) (case 3 UT_LIRE; Raven has no case, char-raven CH0) ...`
- JUNE_PORT_BACKLOG_ORDER.md:354 `ADD_RENA_SYSTEM needs the user's decision if the pilot is Rena; otherwise it waits for Stage 7.` (:366 same idea; :1524 `| ADD_RENA_SYSTEM (Rena's natural force) | 5 or 7 | Ask when Rena is next. |`; :1724 `ask the ADD_RENA_SYSTEM decision when Rena is`)
- JUNE_PORT_BACKLOG_ORDER.md:1413 `CX2GUNPC::AttackResultByType 32773ee0 for every non-user attacker; March does it only under ADD_RENA_SYSTEM (off; char-rena's planning asks for it).`
- funcmap.tsv:4647 `32298590 (Transcend T5) CX2GameUnit::ApplyHyperModeBuff (June form) PORTED ... User: unit type 3 -> 32cb50c0 / 32cb6290( 5 ) (Rena, checklist 7ao); ...`
- funcmap.tsv:1180 `32773ee0 CX2GUNPC::AttackResultByType confirmed Buff Phase 9a: slot 5 32275700 first, then the Shelling Guardian / Mark of Commander block ... Callers DamageManager 32625000, EffectToUnit 3262b6e0 @3262b899`; also :1149, :1176, :1500
- funcmap.tsv:1327 `DD+0x5a4 DamageData::m_eFirstAttack (FIRST_ATTACK_CHECK, March X2DamageManager.h:1159) confirmed Effect C6a: int. SimpleInit 31f8ee50 @31f8f31c = 0 (FAC_NONE) ...`
- funcmap.tsv:1328 `DD+0x5a8 DamageData::m_RelateSkillData (DamageRelateSkillData {Byte skill index, Byte relate data}) confirmed ... SetDamageDataFromLUA tail @326405aa-326405b1: from the attacking user's word +0x9289`
- funcmap.tsv:1533 (321ef5ea-321ef632 EffectSet damage case first-attack copy = March X2EffectSet.cpp:1098-1121); :1588 (31f85910 @31f860f2 RT_CREATE_DAMAGE_EFFECT child sequence, "+0x7a4/+0x7a8 first-attack copy (ADD_REN...)")
- funcmap.tsv:8206 `32c5d540 (CH1o) CX2GURaven::AdjustDamageDataBeforeDamageReact (vslot 0x300) PORTED ... renamed to March's ADD_RENA_SYSTEM virtual (X2GUUser.h, empty base, Lire's override) ...`
- No funcmap row for 32cb6290 / 32cb50c0 themselves (Rena's own June functions); they appear only in funcmap:4647 and the checklist.

## 7. Risks
- Identifiers spot-checked with the flag on: CX2RenaGageData / CX2RenaMyGageUI (defined under the same flag), bases CX2GageData / CX2MyGageUI, 7 UC_LIRE_* in X2Unit.h, BFI/BTI_BUFF_NATURAL_FORCE (unconditional), NFT_* / FAC_* / DamageRelateSkillData (X2DamageManager.h). No missing identifier found, but there is no compile proof: nobody has built the tree with the flag on since the studio's March 2014 code (probably never, with later June additions around it).
- (a) X2DamageEffect.cpp:5217 flag ON removes June's SPAWN_INHERIT child sequence.
- (b) With ON only the enum `m_NaturalForceType` exists; the int `m_iNaturalForceType` (June +0x56c) and its unsupported-key log vanish; flag-off June code reads literal `4` / `-1` for NFT_RESULT_BOMB / NFT_NONE.
- (c) March's 2013 NF behaviour (gain per hit, NFT_* per combo state, 18 writes) is not June 2017's (`32cb50c0` count x 15.0 hyper buff, `32cb6290`, NPC path `32773ee0`): ON is a base to port onto, not parity.
- (d) Offline emulator gives Lire CAC_NONE; the debug overlay (X2GUUser.cpp:29719) and the dev "fill NF" key (X2Game.cpp:5855) would ship live.
- (e) CommonPacket.h needs the second #define + SECOND_HOMES entry.
