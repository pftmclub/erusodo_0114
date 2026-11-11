# Add (char-add): checklist rows and generic rows every port must handle

Source: `JUNE2017_CHARACTER_PORT_CHECKLIST.md` (1055 lines; `L` = line numbers of 2026-11-11). Read-only digest for the char-add planning session; only the three `june2017_re/char_add/plan_*` files were written.
Grep terms: `CX2GUAdd`, `UT_ADD`, `UC_ADD`, `ANSI_`, `Add_NasodRuler`, `X2GUAdd`, `Add` as class word, `ADD`, `DP`, dynamo, `Nasod armor`, `NASOD`, formation, `Crest of Time`, `CREST_OF_TIME`, `436`, `926`, death prevention, `0x740`, `0x744`, `329xxxxx`, `32a0xxxx`-`32a3xxxx`, skill names (Phase Shift, Apocalypse, Mastermind, Lunatic Psyker, Fantasy Tracer, Library of Limitless Mana), and the mark pattern `(Name: CHn)`. False hits dropped: the English verb "add"; Eve / Ara / Rose / Ain addresses that only matched `329xxxxx` / `32a3xxxx`; L107 (Eve `329968a0` / `3299a9d0`), L152 (`UT_ADD` as the end of `UNIT_TYPE`, a Rose row), L579 (`X2DungeonManager.cpp:252-552` reads "... ARA / ELESIS / ADD only": Add has a dungeon-registry branch already), L1007 heading.

Add's June class code: vtable `334daaec` (xrefs: constructors in `FUN_32a22170`, `FUN_32a23b90`), first override `329cf870`, last override `32a316c0` (june2017_re/char_eve/plan_checklist_rows.md L95); Eve ends before `329cf870` (JUNE_CHARACTER_EVE_PHASES.md L20), Ain starts at `32a38770`. So `329cf870`-`32a316c0` is Add. `32a31900`-`32a32640`, `32a327a0`, `32a34020`, `32a3d1f0`, `32a3d220` are Ain / Ara / Lu gauge or Ain code, not Add (Part D). `329cc340` / `329cc370` sit before Add (CX2GUUser helpers); Add only calls them.
Classes (`X2Lib/X2Unit.h`): March 9 `UC_ADD_NASOD_RULER`, 32 `UC_ADD_PSYCHIC_TRACER`, 122 `UC_ADD_LUNATIC_PSYKER`; June-only, registered by name after `UC_END` under `JUNE_ENUM_NAMES` (X2Unit.h:162-180): 33 `UC_ADD_ARC_TRACER`, 36 `UC_ADD_TIME_TRACER`, 123 `UC_ADD_MASTERMIND`, 126 `UC_ADD_DIABOLIC_ESPER`. (`UC_VALIDE_END` = 123, `UC_END` = 124 per char_luciel/plan_march_side.md L60; so 123 fails `< UC_VALIDE_END` and 126 fails `< UC_END`: inferred from those two facts.)
**No `(Add: CHn)` mark exists anywhere in the checklist** (`grep -c "(Add:"` = 0). Add is the last Stage 7 character; every row below is unmarked. Aisha plan L99 says Add has "13 section-7 rows"; this scan finds 56 Add rows (many are address-only or L-grouped).
Owner buckets: `CC` = class core (CH1); `CM` = class mechanic (DP / dynamo / nasod armor); `SK` = skills (CH2/CH3); `PA` = passives/overrides (CH4); `UI` = UI/data (CH5); `CP` = class path (CH-N); `TR` = transcendence; `EN` = engine piece (own Opus sub-phase); `NA` = already in March / n/a?.

## Part A - rows that name Add (56 rows)

| Sec | L | Digest (June addresses and March file:line verbatim) | Owner bucket | Note |
|---|---|---|---|---|
| intro | 5 | Scope list: "a June update of a March one (Elsword, Aisha, Rena, Raven, Eve, Chung, Ara, Elesis, Add)". | already in March / n/a? | scope only |
| 2 | 116 | `APPLIED_SKILL_ID` (167 uses, "8c, done") is in "every character script except `Add_NasodRuler`, `Ain_Angel`, `Rose_Mechanic` and `Rose_Ranger`": Add's script has none. | already in March / n/a? | n/a for Add (confirm with `charport_keys.py Add`) |
| 4 | 166 | `CUSTOM_RENDER_STATE` enum table (June `Enum.src.lua` 17473): "**Add** x7, **Ara** x1 (both exist in March)"; "Add: top-level statements 272-328 of 328, from `ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_START` on" are cut off on March's `Enum.lua`. March's own `X2GUAdd.h:485-491` enum shares the name with other values (JUNE_ENGINE_REMAINDER L108). | class core (CH1) | CC (core-list Enum member) + engine R5 (june-engine, ready, STATUS L177) |
| 7 | 239 | Buff-hyper-count reader `329f538a` "(inside the vslot-16 range)", Add: "Reads the buff hyper count" (store `CX2GameUnit::GetHyperModeCountByBuff()`, June `+0x6844`). | class core (CH1) | hyper |
| 7 | 250 | `CX2GUAdd::UpNowDPValue` (vslot 0x2a4) `329e5b00` with body `329e56c0` (inlines dead `329e5180`), `329e5140`, formation-mode entry `32a08580`. DP spend `( 1 - m_fUsingDPValueChangeRate ) + BBT 76 result * 0.01`; gain HyperCharge + `( m_fGainDPValueChangeRate - 1 )`; every spend x `1 - result(+0x6bb0)`; formation cost `[+0x9620] * [+0x95f0]` with no decrease term; Fantasy Tracer refund from DP actually spent. "Add on this branch is incomplete; the user ports Add from June". March: `X2GUAdd.cpp:9474`, `:9086` (R1 read), `:9443`, `:9458` (JUNE_BUFF_PORT_ARCHIVE L1703-1720). | class mechanic (DP / dynamo / nasod armor) | DP |
| 7 | 251 | `CX2GUAdd::InitPassiveSkillState` (vslot 420) region `32a1ccb0`, call `@32a1e0fd` -> `32a2bbc0` = `SetPassiveSum` on BBT 92 `CHANGE_MAXIMIZE` (`+0x6d18`) with skill ability `0xc4` + Add's own `+0x9680` total. "none yet: BBT 92 is deferred" (buff Phase 5c). | passives/overrides (CH4) | BBT 92 deferred |
| 7 | 253 | Add skill-use function vtable `+0x800` `32a07dc0`: sets cooldown (`32893880`) then cool-time helper `3235a710` (BBT 100); "Add is not [wired], because Add is ported from June". March inline cooldown `X2GUAdd.cpp:2683`; quest RM1 hook after the cooldown set (`X2GUAdd.cpp:2686`, kickoffs/quest-system/RM1.md:48-49). Ara pattern: `SpecialAttackCooldown` (new, `_CORE`). | class core (CH1) | `ApplySkillCoolTimeRateByBuff` call |
| 7 | 257 | `ChangeMaxCharacterAddResource` vslot 201 (`+0x324`) Add `329e31e0`: "DP = gage `+0x11c` x rate through vslot `0x2a4` (`UpNowDPValue`)" (empty base on `CX2GameUnit`; class override needed). | class mechanic (DP / dynamo / nasod armor) | DP |
| 7 | 258 | Death prevention / **Crest of Time**: user slot 250 pre-apply hook `3201ffb0` (BTI 436 / 926 / 807), snapshot vslot `0x740` `32009780` with Add's gauge override `329e2f80`, restore vslot `0x744` `3204da60`, gates `320072d0` (also skills 27409, 23402), members `+0x92a4..+0x92c4`. "Chung: CH4b routed it whole to `char-add`: the cast is Add's `ANSI_SA_COMMON_CREST_OF_TIME`, no Chung state". Chung / Elsword (`32d058a0`) / Rena (`32c852f0`) overrides already written, bases are stand-ins. | engine piece (own Opus sub-phase) | Crest of Time engine (+ CM: Add's own 0x740 override) |
| 7 | 260 | Hook `TryHyperModeStateChange` (`31fe9a90`): Add `329e2340` / `329e2360` (vslots 434 / 435). "The `+0x7e84` = 0x1c write is not ported"; 0x1c = `UAI_RELEASE_FORMATION_MODE` (28): "When porting Add, add `m_FrameDataFuture.syncData.nowAction = UAI_RELEASE_FORMATION_MODE` to the hook branch". | class mechanic (DP / dynamo / nasod armor) | formation / awakening |
| 7 | 261 | `CanHyperMode` (vslot 344) `32a31550` and five Add functions calling vslot 343: `329eaa40`, `329eafe0`, `329eb210`, `329eb560`, `329eba20`; "March's `X2GUAdd.cpp` has no `ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE` use". | class core (CH1) | awakening |
| 7 | 262 | Slot 510 lookup Add `32a20e70`: quick-slot special-item wait (`+0x8d78`) prefix; "these four [Add, Ain, Rose, Luciel] get it with their port" (`CX2GUUser::IsQuickSlotSpecialItemWaiting()`). | class core (CH1) | Ara CH1k pattern |
| 7 | 265 | Same slot 510 copy, riding-key wait (`+0x92d0`) Add `@32a211cc` after `CheckSkillUsePVPOfficial`; "these four get it with their port". | class core (CH1) | Ara CH1k pattern |
| 7 | 266 | Slot 303 `329e65c0` `IsSkillUseCountExcludedState`: true for state index `0x123`-`0x126` and `0x12b` ("June Add states; map them to March's Add enum with the port"); base virtual `CX2GUUser::IsSkillUseCountExcludedState()` exists. | class core (CH1) | state map to `ANSI_*` |
| 7 | 278 | Spec A readers: Add `329e59f0` (`UpNowDPValue` DP consume); spec B reader `32a01b40` (L945 names `32a01ca2`). | class mechanic (DP / dynamo / nasod armor) | DP + CH1q-type readers |
| 7 | 279 | March's `ConvertAndAdd` halves Add's socket hyper charge / hyper time (`SERV_9TH_NEW_CHARACTER`, `fReformValue` 0.5); June has no 0.5. Phase 16 dropped it under `SERV_IRUHADEV_JUNE_SOCKET_PARITY`: "check Add's hyper code for a June-side 0.5 when porting Add". | class mechanic (DP / dynamo / nasod armor) | hyper charge |
| 7 | 280 | Summon follow-ups of `3230c1a0`: the list includes `32a2dec0` (Add range); "Port each class's summon cases with the class, from June's flags and follow-ups". Add summon: NPC 1604 Apocalypse (L255, L287). | skills (CH2/CH3) | summon case |
| 7 | 283 | `3201ad10` skill-group power socket rate callers by address: `329fdbe0` "(Add range)"; "classes by address range; the functions are not decoded". | skills (CH2/CH3) | add the 1.0 term |
| 7 | 285 | `CX2GUAdd::ApplyHyperModeBuff` `329e2410` (June form): BFI 846 `BFI_TRANSCENDENTAL_FORMATION_MODE` when `CX2Unit +0x3c` set, else 315 `BFI_FORMATION_MODE`; first learned of skills 29401 / 29406 / 29411 -> slot `0x23c( skill, 0, 0 )` (March: `BFI_BUFF_LIBRARY_OF_LIMITLESS_MANA_RECOVERY` by `m_iLibraryOfLimitlessLevel`). "**March has Add's function and June reworked it: port it whole.**" Tail `ApplyBuffByUseHyperMode` `3206d480`, no user test. March `X2GUAdd.cpp:9529`. | class mechanic (DP / dynamo / nasod armor) | formation mode (TR overlap) |
| 7 | 286 | `3201f850` `SetSkillLevelStateData`: March copy `X2GUAdd.cpp:2455` gets `( 3201ad10( skill ) + 1.0 )` ("Ara: CH1o, the term inside March's class copy"). | class core (CH1) | as Ara CH1o |
| 7 | 287 | June NPC script `SI_HA_APOCALYPSE` (Add, NPC 1604, June-only) calls `SetMultiplyPowerRate_LUA( GetNumber_LUA() )`; binding `31e8d610`. Binding already ported by char-eve CH2b3 (`tools/data/june_march_divergent.tsv` row `SetMultiplyPowerRate_LUA`). | skills (CH2/CH3) | Apocalypse |
| 7 | 288 | `329fb6c0` (`CX2GUAdd`): dying Apocalypse NPC 1604, owner `UC_ADD_MASTERMIND` with `BTI_BUFF_APOCALYPSE` (345): clears Mastermind member `+0x98e4`, erases BTI 345, refreshes slots. Eve CH2b4 ported the erase and `UpdateSkillSlotUI( false )` inline (`SERV_IRUHADEV_JUNE_SUMMON_UNIQUE`); "port the `+0x98e4` store there with the member, and find the member's readers". | skills (CH2/CH3) | Apocalypse / Mastermind |
| 7 | 289 | `32a07fc0 @32a080fe`: Add DamageReact sets `CX2GUUser` byte `+0x8fc2` (Ferdinand / Apocalypse `ResponseEmergency` flag); "the Add port overrides it the same way and sets its own member in Add's DamageReact" (virtual `CX2GUUser::ResponseEmergency` exists). | class core (CH1) | DamageReact |
| 7a | 306 | Effect-modifier tag sites in Add: `329f3980`, `329fdb60` (Stardust Shower: tag 12), `32a047a0` (Reverse Circle / Void Field: 12, 13, 14), `32a060d0` (Moonlight Breaker: 12), `32a19bb0` (Flick Disc: 12), `32a0e5f0` (Quake Buster Hyper: 14), `329fc3f0` (Space Crack) / `329fd0c0` (Maximum Strike) set `b44` directly, `32a0f830` (Conqueror: `b45` + DD `+0x5fd`), `329f10e0` (Pulse Cannon), `32a04ce0` reader of DD `+0x5fd` (skills `0x7150` / `0x7225`); "plus Add's count-as-skill-hit mark". Funcmap 4361: tags 1, 3-23 "log once and wait for their classes". | skills (CH2/CH3) | CH2/CH3 per skill |
| 7a | 311 | Attach buff factors to an effect set: `321e3650` from `32a0e5f0`; `321e3830` (one modifier) from `32a04a2f`, `32a04b03`, `32a06d06`. "Do not port `321e3730` / `321e3830` (Rose / Add only): they are checklist 7a's" (kickoffs/june-effect/C6c.md:44). | skills (CH2/CH3) | per skill |
| 7c | 356 | Alpha-tag sites: "12 sites (`329f4350`, `329f9ff0`, `32a03b70`, `32a27360`; four of them effect-set handles via `32296f30`) / Add / as above". | skills (CH2/CH3) | one tag call per site |
| 7c | 358 | Row `322726f0` (GetFinalDamage HP-rate gate returns false for `RNSI_SA_ALP_SUPERSONIC` 29058 vs an NPC defender; `+0x8aeb` / `+0x8ae9` / `+0x8e80` / `+0x8f08`) is labelled **Rena** in the Class column, but ALP = Lunatic Psyker = Add skill 29058 (Supersonic); "C10b-1 calls `GetHpRateDamage` unconditionally until then". Verify the label in CH0. | skills (CH2/CH3) | Supersonic; label suspect |
| 7d | 380 | `CX2GUAdd::SetNasodArmorAttackTarget` gate `329f63a0`: top switch `0x7211`, `0x7212`, `0x7220`, `0x7221` = 29201 / 29202 / 29216 / 29217 (`RNSI_A_AN_PHASE_SHIFT`, `RNSI_A_AN_PULSE_BULLET` + `_UPGRADE`); March passes `SI_A_AN_PULSE_BULLET` / `SI_A_AN_PHASE_SHIFT` instead (`X2GUAdd.cpp:9843`); 52 `CAN_ACTIVATE_NASOD_ARMOR_COMBO = TRUE` lines in June `DamageEffect*.lua`; "user, 2026-10-07: follow June, accept off". | class mechanic (DP / dynamo / nasod armor) | nasod armor |
| 7d | 382 | Rest of `SetNasodArmorAttackTarget` (`@329f64c1` on) rewritten: NPC-defender test `*(npc+0x85f8)+4 == 0`, skill-memo swap `328905b0`, `32278580`, bone lookup vslot `0x80`, line-map walk vs March's `GetPos` distance; "June's whole body ... read `329f63a0` end to end when porting". | class mechanic (DP / dynamo / nasod armor) | nasod armor |
| 7e | 372 | Unit-viewer "lost under June's player: the SBH / ALP end windows and forced emit rates"; Elesis, Lunatic Psyker, Chung, Elsword, Rena; "nothing to port". | already in March / n/a? | nothing to port |
| 7g | 412 | Special ability 41 `SAT_ADD_DP_PERCENT_UP`: "Add / 1 item (100001054). / The ability's effect on Add's DP." (backlog L936: "goes to the character-port checklist (Add)"). | class mechanic (DP / dynamo / nasod armor) | DP |
| 7h | 423 | `EFFECT_SET_LIST_EX_TRANS_*` / `_FINAL` / `_FINAL_ULTIMATE` list variants: "every class with June scripts (... Ain, Ara, Add, Elsword: 23 scripts for `_FINAL`, 10 for `_FINAL_ULTIMATE`)"; selection per skill memo / transcendence / applied-skill level / ultimate flag; Eve CH1r. | skills (CH2/CH3) | script + loader |
| 7h | 424 | `EffectSetToPlay` entry `+0x34` `POSSIBLE_NASOD_BARRIER` (38 June script lines) and user slots `0x4c4` `329e7d60` / `0x4c8` `32a047a0` around each play in `32007570`; "Find the class readers of the two fields and port the slot overrides with the class (add the two base virtuals then)". Funcmap 5370. | class mechanic (DP / dynamo / nasod armor) | nasod barrier |
| 7h | 425 | `SOUND_PLAY_EX`, `STATE_SOUND_PLAY`, `CUSTOM_STATE_DELETE_SOUND`: "Add, Elesis, Luciel, Arme, Aisha (18 / 2 / 2 June scripts)"; "Nothing extra in code: they work once the class scripts are adopted". | already in March / n/a? | script adoption (CH1) |
| 7h | 427 | `USEFUL_VALUE0..4` readers in class code; Add-range examples Future `329d7220`, `32a02f10` (`c9h4f_useful_value_readers.tsv`); read via `JuneGetUsefulValueFuture / Now`. | skills (CH2/CH3) | state functions |
| 7i | 444 | `SetEquippedSkillFuncBySkillSlotIndex` Add `32a28e00` (vslot 0x7b4, vtable `334daaec` slot 493): drop the snapshot call; state byte `+0x11` = 1 on normal and hyper entries: "a class port drops the store" (Aisha CH1k, Raven / Rena CH1n, Ara CH1n done). | class core (CH1) | CH1n-type |
| 7l | 470 | Add DamageReact override `32a07fc0 @32a080cc`: vslot `0x81c( 0, DD )` (`JuneIsSuperArmorAgainst`, C3c) after its own DamageReact calls by reActType; false -> `32a03850`. "The Add port calls it where June does". | class core (CH1) | DamageReact |
| 7l | 475 | `CX2GUAdd 329d4640` overrides `JuneIsPacketBuffRefused( BTI )`: true for BTI `0x156` (342) and `0x1b6` (438); "they exist in March's `BUFF_TEMPLET_ID` only if the Add port adds them". | class core (CH1) | BTI 342 / 438 enum |
| 7m | 478-483 | `CX2GUAdd::DieDamageEffectProc` `329f73a0` (June: `CEffect*` argument; only override of the `NOTIFY_TO_OWNER_WHEN_DIE` hook, owner vslot `0x2c4`): `m_eUsedSkillID` (DD `+0x590`) = `RNSI_A_AN_PHASE_SHIFT` 29201 **or** `_UPGRADE` 29216, `m_bNoDamage` (DD `+0x4dc`) 1, handle `+0x9764` valid -> zero `+0x9758..+0x9760`, handle -1, `329f7370( this +0x9750 )`. "March checks `SI_A_AN_PHASE_SHIFT` only." Engine side ported (`..._NOTIFY_OWNER_FIRST`); play-test waits for the Add port (EFFECT_COMPLETION L9088). | skills (CH2/CH3) | Phase Shift |
| 7o | 511 | Fall-speed limit data: `ANSI_APT_DASH_JUMP_COMBO_ZX` -4000 (one of 32 June states; EFFECT_COMPLETION L1667-1668). | class core (CH1) | script adoption |
| 7t | 616-617 | Sound-set keys `SOUND_SET3_RATE` and up, `STATE_SOUND_PLAY<n>` inside a sound set: `Add_NasodRuler` named. | already in March / n/a? | script adoption (CH1) |
| 7w | 664 | `CUSTOM_RENDER_STATE` class users: "`Add_NasodRuler` (`CRS_INVISIBLE` x6, `CRS_UNIT_FADE_WHITE_WITHOUT_WEAPON` x1) ... come with their ports"; engine R5 ready. | class core (CH1) | script + R5 |
| 7y | 680-682 | June adds `GetLandPosition( ..., true )` in Add's `ANSI_A_AN_PHASE_SHIFT_BLINK_StateStart` (`32a176c0` `@32a1787e`): "lands the return position, March `X2GUAdd.cpp:8224` sets it unlanded". | skills (CH2/CH3) | Phase Shift Blink |
| 7aa | 703 | `UpdateEquipCollisionData` pair exists for every class; "takes a non-Add / non-Rose item's hit lines from the basic item's anim" (corrected 2026-10-30; engine R14c ready). | already in March / n/a? | engine R14c |
| 7ae | 722, 725 | "Add the `RNSI_*` enumerators ... and the state for each one in the class's `SetEquippedSkillFuncBySkillSlotIndex`"; an unmapped id "plays the class's first skill state ...; Elesis and Add leave it blank". | skills (CH2/CH3) | CH2/CH3 enumerators + states |
| 7ad | 760 | Motions: "Add's `APT_NA_Ready` / `APT_NA_preAttack`, plus skill animations of Add, Ara, Chung, Elesis and Lire" renamed in June (`r12b_anim_loss.tsv`); "CH0b and CH1 deploy together" (backlog L140: Add 4 new job motion files). | class core (CH1) | CH0b stage + CH1 deploy |
| 7ac | 781-782 | June-only helpers with `NULL, &idx`: user landing prediction `32021390` (Add / Luciel / Arme callers; other callers `329d22b0`, `329d2340`, `329f092b` per AISHA L941), line clamp `32294530` (Add). JUNE_BUGS 375. | skills (CH2/CH3) | CanDown call shape |
| 7aj | 877-879 | Tree-UI class pieces: equipped-slot icon override `328e0aa0` "Add's `+0x138` / `+0x139`" (tree ctor `328e76a0` zeroes them), "the 0x7b / 0x72 / 0x73 / 0x7e and Add blocks stay with their classes"; Install / G-Core marks `328e0290`: "classes 9 and 0xb toggle `Static_Install_Skill` / `Static_GCore_Skill` per slot through the game unit's vslot 0x71c". | UI/data (CH5) | CH5 |
| 7ak | 896 | Skill-page decide ACK: "**Unit type 9 (Add):** the tree UI's `328e0290( string 3347f014 )` (`@3220f53a`), the same install / G-Core mark refresh as 7aj". | UI/data (CH5) | CH5 |
| 7am | 924, 932 | Delegate-list registration: Add registering function `32a1ccb0`: A `32a04ce0`, A `329d4190`, A `32a01d20`, D `32a01dd0`; "March's own pushes (Elsword, Raven, Ara, Elesis, Add) are March's callbacks; a port replaces them with June's set". | passives/overrides (CH4) | CH4b |
| 7an | 943 | Vslot `0x73c`: Add `329d46b0` "the override, whole". | passives/overrides (CH4) | CH4c-type |
| 7an | 945 | `GetCharacterSpecializationB()` (June `+0x6d04`) reader Add `32a01ca2`, "the reading function, whole". | passives/overrides (CH4) | CH1q-type |
| 7ao | 968 | "Add / Ain: `OnFrameMove` `3206eaf0 @3206fed1` counts down `+0x9330` (their hyper StateStart sets it before `31fc8c20`)" (engine ported by T5, funcmap 4646: "Add / Ain, not ported: checklist 7ao"). | transcendence | hyper StateStart |
| 7ao | 982 | Add's June readers of `CX2Unit +0x3c`: `329e23a0` (BFI 846 formation buff instead of 315); `329e6f30`, `329e6f80` (skill-id map gates, 12 / 94 callers); `329e7210`, `329e7ca0`, `329e7d60`, `329e99a0`, `329f10e0`, `329f9380`, `32a02260`, `32a047a0`, `32a0bfd0`, `32a15580`, `32a19bb0`; `32a08150` (hyper StateStart, calls `31fc8c20`): "each branch, whole (Stage 7)". | transcendence | each branch whole |
| 7ar | 1009-1010 | `GetX2UnitClassCompatibility` `31ed6500` (March `X2Unit.cpp:1465`): unit classes 33-46 / 123-136 / 200-202 belong to classes March lacks; "the port that adds such a class adds its cases from June's switch, under that port's flag" (verdict `out`). Add 33 / 36 / 123 / 126 fall in the range (inferred; the row does not name Add). | class path (CH-N) | class path |
| 7av | 1053 | "**Add** (Stage 7) / **Ain** (Stage 8): June's user float `+0x92a0` (ctor `3203e850`, reset by base `InitPassiveSkillState` `31fe19d0`, added by Add `32a1ccb0` / Ain `32a4f760` passives) multiplies after the world rate; March lacks the member"; vslot 0x824 `GetStateAnimSpeedRate`; divergent row `ResetAnimSpeed`. | passives/overrides (CH4) | CH4 / anim rate |

### Part A tally by owner bucket (rows)

| Bucket | Rows |
|---|---|
| class core (CH1) | 15 |
| class mechanic (DP / dynamo / nasod armor) | 10 |
| skills (CH2/CH3) | 14 |
| passives/overrides (CH4) | 5 |
| UI/data (CH5) | 2 |
| class path (CH-N) | 1 |
| transcendence | 2 |
| engine piece (own Opus sub-phase) | 1 |
| already in March / n/a? | 6 |
| **total** | **56** |

## Part B - class-generic rows that carry per-character marks but no Add mark (43 rows; 31 not already in Part A)

Every earlier port (Aisha, Eve, Chung, Elsword, Raven, Rena, Ara) marked these rows in place with `(<Name>: CHn)`; none has an `(Add: ...)`. Ara's CH-phase marks are present (Ara plan is running). Rows are Add's too unless noted. "also Part A" = Add has its own row there as well.

| Sec | L | Label | Also Part A |
|---|---|---|---|
| 2 | 117 | NUMERICAL_DATA_INDEX tables, per-skill damage numbers | - |
| 4 | 162 | Enum tables per character script (Add: CUSTOM_RENDER_STATE row) | - |
| 4 | 178 | Missing members of tables March has (SKILL_ID names) | - |
| 5 | 184 | Every non-DamageData key in the character script | - |
| 6 | 214 | Standing check: ReservedSkillStart override, X2GUUser chain | - |
| 6 | 216 | Mark convention (Name: CHn) for rows each phase ports | - |
| 7 | 255 | IsSummonSkill ids and summon cases (NPC 1604 is Add) | - |
| 7 | 256 | State power rate operator after DoStateEndStart (+0x7cc8) | - |
| 7 | 258 | Death prevention; Add-specific, also in Part A | yes |
| 7 | 262 | Slot 510 quick-slot prefix; also in Part A | yes |
| 7 | 263 | Dev free-skill gate in the 510 lookup (JuneIsDevUnlimitedSkillUse) | - |
| 7 | 265 | Riding-key wait in the 510 lookup; also in Part A | yes |
| 7 | 278 | Spec A / B readers; Add rows also in Part A | yes |
| 7 | 280 | Summon stats helper 322ece70, per-class cases; also Part A | yes |
| 7 | 283 | 3201ad10 socket power rate callers; Add range in Part A | yes |
| 7 | 286 | SetSkillLevelStateData + 3201ad10 term; also in Part A | yes |
| 7b | 329 | Skill-memo consumers 31fdbbb0 in class content | - |
| 7b | 330 | Passives keyed on RNSI_* (Add not listed; Add 329e4a50 = March) | - |
| 7h | 423 | Effect-set list variants _FINAL; also in Part A | yes |
| 7h | 427 | USEFUL_VALUE readers in class state code; also Part A | yes |
| 7h | 428 | USI_WAIT_STATE_CHANGE registered in every class InitState | - |
| 7i | 439 | Class code multiplying m_fNowStatePowerRate after rebind | - |
| 7i | 440 | SetEquippedSkillFuncBySkillSlotIndex head (snapshot, +0x7cc8) | - |
| 7i | 444 | Snapshot call drop in equip func; also in Part A | yes |
| 7l | 472 | Bounding user states registered in every class InitState | - |
| 7o | 509 | DAMAGE_BOUNDING_DOWN table in every class script | - |
| 7p | 548 | TIME_STOP_IMMUNE_STATE in 76 June states, all classes | - |
| 7t | 592 | Awakening key set in every class script | - |
| 7t | 595 | m_HyperCutIn / m_BerserkCutIn UnitTemplet rows per class | - |
| 7t | 618 | Per-class overrides for hyper sound-set pick | - |
| 7t | 624-625 | Cut-in name table per class (s_SkillCutInSet) | - |
| 7x | 667-674 | Combos must carry AT_COMMAND; per-class tag counts | - |
| 7ae | 717-719 | Reform ids with no class state; per-family counts | - |
| 7ae | 729 | SpecialAttackEventProcess charges unbound ids; bind states | - |
| 7ae | 738 | ResetLeftSkillCoolTimeBySkillType: type 7 matches 14-16 | - |
| 7ac | 783 | CanDown call shape and landing-prediction helpers | - |
| 7ag | 811-820 | Pressed-index decode (slot + 1, > 4 / - 5) per class | - |
| 7ai | 857-869 | BuffFactor-by-BFI calls and skill-name string callers | - |
| 7am | 917-932 | Delegate-list registrations per class; Add also Part A | yes |
| 7an | 942-946 | Per-class passive hooks and specialization readers | - |
| 7ao | 957-960 | Transcend allow-list, LevelUpEvent row, hyper table, helpers | - |
| 7ap | 991-994 | JuneModifyFinalDamageByGameUnitType 0x3ac, ShownSkillCoolTime 0x6a8 | - |
| 7av | 1051 | vslot 0x824 state anim rate; Add in Part A | yes |

Add-specific notes on these rows: L116 says the `APPLIED_SKILL_ID` marks list "every character script except `Add_NasodRuler`"; L330 does not list Add (funcmap row 198: June `CX2GUAdd::SetDamageExceptionProcess` `329e4a50` is "IDENTICAL to March `X2GUAdd.cpp:8603` ProcessBodyOfTranscendence (inlined)", Effect C8a). Row L994 (0x6a8 `JuneShownSkillCoolTime` overrides) names Ara / Chung / Lire only; whether Add has a 0x3ac / 0x6a8 override is not stated: check Add's vtable `334daaec`.

## Part C - rows matched only by an address near Add that are NOT Add

| L | Address(es) | Owner | Verdict |
|---|---|---|---|
| 107, 305, 342-353 | `3297xxxx`-`329cxxxx` | Eve (range `32965dd0`-`329cf86f`) | NOT Add |
| 268 | `329cc340` | CX2GUUser `IsNowStateSkillUpgraded` (Chung CH2b0 ported); Rose caller shown; Add's own callers `32a10fe9`, `32a266ce`, `32a26e8f`, `32a26f17` (funcmap L7020) | Add must call it with Add's own now-state id (see plan_routed_items.md) |
| 244, 259, 855, 1042 | `32a3d1f0`, `32a3d220`, `32a38770`, `32a3d330` | Ain (starts `32a38770`) | NOT Add |
| 281, 293 | `32a34020`, `32a31900`, `32a32640`, `32a31a00`, `32a31a20` | gauge-data helpers after Add's last override `32a316c0` (Ara / Ain / Chung village) | NOT Add |
| 312 | `32bb0bd6` etc. | Ara | NOT Add |

## Known gaps / not found

- No row names `Add_NasodRuler` beyond L116, L616-617, L664 (the script is June source `june_src_fixed/data036_kom/Add_NasodRuler.src.lua`); Add is absent from the Section 1 numerical-only table (Aisha plan L99: "Add | 0 (skeleton)"), so Add's numerical rows come from `plan_numerical_rows.tsv` (140 rows, skill ids 29000-29602).
- No checklist row covers Add's creation-screen entry, stat rows for classes 9 / 32 / 122, the training-game F5 / F6 / F7 keys (`329e2170` / `329e20e0`), or vslot `0x524` (`32a315a0`): those live in backlog / funcmap / plan docs (plan_routed_items.md).
- L5 / L152 / L579 / L1007 are context rows with no Add work (Part A "n/a").
