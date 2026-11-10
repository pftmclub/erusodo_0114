# Ara (char-ara): checklist rows and routed items

Source: JUNE2017_CHARACTER_PORT_CHECKLIST.md (1053 lines; "L" = current line numbers, 2026-11-10), plus backlog, STATUS, funcmap, JUNE_BUGS, MODS and the other plan docs. Read-only; only this file written.
Grep terms: Ara, CX2GUAra, X2GUAra, ASI_, ALH, ASD, ALD, AYR, Little Hsien / Sakra / Little Devil / Yama Raja, UT_ARA, Ara_Martial_Artist, "every class / all 12 / every user / every character", and June addresses `32b90c20`..`32bd86ef`.
Ara's June class code: vtable `33505314`; funcmap puts its first function at `32b90c20` (`SpecialAttackEventProcess`, vtable `33505b0c`) and the last at `32bd85f0` (vslot 0x51c); Chung starts `32bd86f0`. Elesis (CX2GUEL, vtable `334fa61c`) ends at the thunks `32b906b0` / `32b909b0` (L496) and the EL-only `32b90420` (L264), so `32b8....` / `32b90...` addresses in rows L264, L275, L455, L496 are Elesis, not Ara.
No `(Ara: CHn)` mark exists in any row yet (grep found none). Ara's June classes 34 (Little Devil, ALD) and 124 (Yama Raja, AYR) are June-only; ALH (Little Hsien) and ASD (Sakra Devanam) are March classes. No row names ALH / ASD / ALD / AYR / Sakra / Yama as a class key; they appear only inside skill-state names (`ASI_SA_ALH_*`, `SA_ALD_*`, `P_AYR_*`, `RNSI_P_ASD_OPEN_MIND`, `RNSI_P_AYR_OPEN_MIND`).
Marks of earlier characters are quoted in the last column to show which rows every port has already walked.

## Part A - rows that name Ara (Ara-specific)

| Sec | L | What Ara needs | June addresses | Class-generic? / suggested phase |
|---|---|---|---|---|
| intro | 4 | scope list names Ara | - | n/a |
| 1 | 55 | `Ara_Martial_Artist`: 14 numerical-only DAMAGE_DATA tables of 49 (backlog L367: gap 14) | - | CH2/CH3 (Chung/Rena rows mark per CH2x which ran through the engine) |
| 2 | 121 | `BUFF_FACTOR_ID_ULTIMATE` x3 in `Ara_Martial_Artist` ("to-implement, 8b group 2") | - | CH1 script / CH2 (check if still unimplemented) |
| 2 | 166 | `CUSTOM_RENDER_STATE` x1 in Ara: statement 338 of 338 (`ASI_SA_ASR_FOX_PHANTOM`); the June enum table truncates Ara's script on March's `Enum.lua` (also L837 effect plan, backlog L144) | - | CH0/CH1; see also L659-664 (7w); Enum is core-list |
| 7 | 236 | `32bb22d0` / `32bb2740` vslots 49/50 `HyperModeBuffEffectStart/End`: when `+0x9744 > 0` and buff hyper count < 3 a loop over June-only members; End writes the count to 0; uses `GetHyperModeCountByBuff()` (June `+0x6844`). "March has both, June reworked them: port each whole" | `32bb22d0`, `32bb2740` | CH1 (hyper start/end). Rena L974 / Raven L975 / Chung L977 have the same per-class hyper-start row; Raven / Rena mark CH3d "transcended return is CH1h's port" |
| 7 | 237 | buff hyper count readers/writers: `32ba6cc0` (vslot 39), `32ba6d00` (vslot 147: count 1 -> state `0x294`, 2 -> `0x295`), `32badc80` region (485), `32baf270` (397), `32bc8be0` (198), `32bc8030` (vslot 17 writes) | those | CH1 (same port as L236; L284 says port `32badc80` together) |
| 7 | 257 | `ChangeMaxCharacterAddResource` Ara `32baafc0` (vslot 201 `+0x324`): n = (int)(max force power x rate); when protected `CX2GUUser +0x9644` > 0, force power over max x `+0x9644` via vslot 60 (`3208dd00`), then June's `SetForcePower` `32ba6ef0` (adds a `+0x6d04` spend conversion); skill 27417 `RNSI_P_AYR_OPEN_MIND` learned -> vslot 143 `(27417,0,1)`. Funcmap L1019 confirms | `32baafc0`, `32ba6ef0` | Add/Ara/Rose/Luciel row; Chung's override ported (buff 7b). Needs a member for `+0x9644` |
| 7 | 258 | Death prevention `320072d0` is every-user; Ara pieces: skill 27409 learned with `+0x867c->+0xaf0` type 7 and `32bc5a50` -> force-invincible, `+0x8e74` = 0x27; **Crest of Time snapshot**: vslot `0x740` per-class gauge override Ara `32ba4840` | `32bc5a50`, `32ba4840` | Marks: Elsword CH1q (`StoreGageSnapshot` / `RestoreGageSnapshot` under `_CORE`), Rena CH1q (`RestoreGageSnapshot` 32c89230), Chung routed to char-add. Ara has its own gauge override, so write both |
| 7 | 262 | Every-class vslot 510 lookup prefix: Ara already has it (Phase 7d put it at the top of March's Ara `SpecialAttackEventProcess`; funcmap L1062 `32b90c20` calls 510 `32bcfaa0` first) | `32bcfaa0` | Done in March; Ara's 510 = `CanUseSpecialAttack` (funcmap L1082). Port keeps it |
| 7 | 265 | riding-key wait: Phase 7e put it after `CheckSkillUsePVPOfficial` in Ara's `CanUseSpecialAttack` | `32bcfaa0` | Done in March (Eve / Chung / Raven did it in `SpecialAttackLookup`) |
| 7 | 267 | `CX2GUAra::GetReduceDebuffDurationRate` `32bd32f0 @32bd339e-32bd3454` (vslot 167): after the officially-debuff test, skill 27415 (`RNSI_P_ASD_OPEN_MIND`, June-only) learned (`31fceab0` > 0), counter `+0x114` (`32a31a00`) reached `+0x120` (`32a31a20`) and the buff is a debuff -> rate x `32007b10( 0x7c, 27415 )`; funcmap L1126 | `32bd32f0` | CH4 (Open Mind passive; the `SERV_IRUHADEV_JUNE_BUFF_GROUP` block is where it goes) |
| 7 | 284 | vslot 485 override `32badc80` (Ara vtable `33505314`; the other override is Luciel `32b0e670`): runs the skill-group power socket pass `31fe14f0` for skills outside the 10 slots: every skill in the list `32d3f040` builds, unless `31fca980( skill, 1 )`. Override the empty hook (16-step3b7), call `AddJuneSkillAdditionalPowerRateBySlot( kSocketData, skill )` | `32badc80` | Ara+Luciel only. Ports with L237 |
| 7 | 286 | `SetSkillLevelStateData` June base form `3201f850`; callers `32bd358d`..`32bd3db2` are Ara's; March's Ara copy `X2GUAra.cpp:2989`; multiply state power rate by `( 3201ad10( skill ) + 1.0 )` | `3201f850`, `32028190`, `32bd358d`..`32bd3db2` | Class-generic (El, Add, Ara, Rena, Raven copies). Mark: Aisha CH1i only on the row |
| 7 | 293 | tutorial flag in `CX2GUAra::InitComponent` `32bb0e2c`: tutorial -> force power 3 (`32a32640( 3 )`; March handles PvP and training only) | `32bb0e2c` | Ara-specific; read `g_pMain->GetIsPlayingTutorial()` directly (Eve CH1l did the same) |
| 7a | 312 | CEffect `+0xb18` writer `32bb0bd6` (in a function called from `32bbcdc0`, `32bbcff0`, `32bbd240`, `32bbd3a0`) and overrides of unit vslot `0x328` (base `3208ddc0`): "not yet attributed (the `32bb....` range)" = Ara; sets the create-damage-effect rate. JUNE_BUGS 103 `latent` | `32bb0bd6` etc. | Ara (address in range). Engine half is C7; the class override is the Ara port |
| 7b | 330 | passives keyed on `RNSI_*`: Ara's vslot `0x780` `32bca010` (27415 `RNSI_P_ASD_OPEN_MIND` / 27417 `RNSI_P_AYR_OPEN_MIND` blocks; funcmap L1347; MODS L2572-2573 "empty hook, Ara overrides it"); brief `kickoffs/june-effect/C8c.md`; effect D8 hands it to the character ports | `32bca010` | CH4b. Marks: Aisha CH4c, Eve CH4b, Raven CH4b, Chung CH4b, Elsword CH4b (each "whole"). Hook `CX2GUUser::JuneDamageExceptionClassHook` exists (empty) |
| 7e | 368 | Unit-viewer `ALD_LobbyWait` row: C9g-2 adopted June's `Motion_ARA_Common.x` + `AnimationNameTable.lua`, so the row matches; March's hand-made orb (`m_hSeqEnergyBullet1-3`: ctor, dtor, `OnFrameMove`, `PlayAnim`, `NotifyShowObjectChanged`, `OnFrameRender_Draw`, `SetUnit`) is dead code keyed `ALD_lobbyWait`. **Retire the three handles under the player's flag with the user's OK** (kept by user 2026-10-07) | - | Ara-specific; **needs the user's OK** (Aisha plan L2586 repeats it) -> CH5 / UI |
| 7h | 424 | `EffectSetToPlay` `+0x34` / `+0x38` and user slots `0x4c4` / `0x4c8` around each play in `32007570`: Ara `32bc1530` is one of the overrides (Add `329e7d60` / `32a047a0`, Ain `32a63ba0`, Rose `32ac30c0`); `RELATE_SKILL_ID` in Ain / Ara / Elesis / Raven / Rose scripts | `32bc1530` | Ara; Eve CH1r stored the entry fields, no reader found yet. Port the slot override (add the two base virtuals then) |
| 7i | 444 | `SetEquippedSkillFuncBySkillSlotIndex` Ara `32bd09d0`: drop the snapshot call, state byte `+0x11` = 1 on normal and hyper entries (`+0x10` = `m_bHyperState`); funcmap L2022 | `32bd09d0` | Class-generic (all 12). Marks: Aisha CH1k, Raven CH1n, Rena CH1n ("both stores dropped") |
| 7o | 516-521 | `APPLY_MOVEMENT_SPEED = TRUE` in one state `ASI_AIRIAL_DASH` (`Ara_Martial_Artist.src.lua:7334`, also `PASSIVE_SPEED_X = INIT_PHYSIC.RUN_SPEED * 1.5`); engine side ported (C3h); carry the key verbatim with Ara's June table, does nothing until then | - | Ara-specific (script adoption in CH1); the C3h play-test used Ara Yama Raja |
| 7o | 522-526 | June SPEED_TIME additions (C3l): `ASI_A_ARA_QUICK_STAB` `SPEED_TIME_ULTIMATE0/1` (2 rows); `ASI_SA_ALS_SPECTER_OF_STORM_ULTIMATE_FINISH` `SPEED_TIME0 = { 2550, 0, 0.496, 2, TRUE }`; carry verbatim | - | Ara-specific (script) |
| 7t | 592 | every June class script sets the awakening key in its awakening states, "plus Ara's berserk ones" | - | Class-generic; marks Eve / Chung / Elsword / Raven / Rena CH1 "script adopted" |
| 7t | 603 | user vslot `0x51c` override `32bd85f0`: now state `0x17` or `0x9b` (`ASI_BERSERK_MODE`) -> override `JuneIsHyperModeState` (funcmap L2442: base ported) | `32bd85f0` | Ara/Luciel only (Chung has the `JuneIsBerserkByChance` row) |
| 7t | 613-615 | sound-set pick: until the override lands Ara's `ASI_BERSERK_MODE` picks as a plain state; June class scripts bring the keys | - | Ara/Luciel; C3t-2 play-test used Ara Z-Z-Z |
| 7ae | 724-725 | census: ids with no March state fall into the class switch `default:` ("Ara Wind Wedge" plays); reform `RNSI_*` ids must be added to the enum and bound in Ara's `SetEquippedSkillFuncBySkillSlotIndex` | `s3_reform_ids_no_state.tsv` | Class-generic (Chung CH2a-d, Rena CH2a-d marks). Ara: regenerate the tsv for 34 / 124 (CH-N) |
| 7ad | 757-760 | motions: pattern `^motion_(...|ara|...)`; "plus skill animations of Add, Ara, Chung, Elesis and Lire" renamed in June; keep script and motions in the same pack (`r12b_anim_loss.tsv`); backlog L140: Ara 2 new motion files (`Motion_Ara_LS.x`, `Motion_Ara_AA.x`, JUNE_ENGINE_REMAINDER L131) | `r12b_anim_loss.tsv` | CH0b (stage) + CH1 (deploy together). Marks: Chung / Elsword / Raven / Rena CH0b / CH1 |
| 7ac | 775-778 | `SLockOnSystem::ProcessLockOn` `3200fa20 @3200ff98` (March `X2GUAra.cpp:5435`), `ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK` EventProcess (`@32bc204c`, `:11264`) and `_FINISH` StateStart (`@32bb70ff`, `:11371`) pass `NULL, &lineIndex` (March seeds `lineIndex = 0`); June also calls `CanDown` in the candidate loop (`@3200fcad`); shared with Luciel. STATUS L239: char-luciel CH3c20 hoists it out of `CX2GUAra` | `3200fa20`, `32bc204c`, `32bb70ff` | Ara; Luciel CH3c20 does the shared function, Ara's two call sites are Ara's CH3 |
| 7ac | 781-782 | June-only helpers with `NULL, &idx`: "Ara landing helper `32bc0420`" | `32bc0420` | Ara CH3 |
| 7ai | 854 | `ByBuffFactorID` calls: "e.g. Ara `32a4dcaa` 0x7ea0" (address lies in Ain's range `32a4....`; label looks wrong, verify before use); pass June's arguments at every call ported (`s3e_bybfi_callers.tsv`) | `32a4dcaa` | Class-generic; check the tsv for Ara's real call sites |
| 7ai | 866-867 | `JuneShowSkillNameStateString( iSkillID )` `3200ab60` callers include `32bc84b0` (Ara range) | `32bc84b0` | Ara calls it where June does (Aisha CH1i did the five in hers) |
| 7am | 920 | delegate lists: Ara registering function `32bcd9c0` (and `32bb0d10`, called from it): A `32b95910`, A `32ba7e40`, A `32b95980`, A `32b959a0`; A `32baf810`, A `32baad30` | those | CH4b. Marks: Rena CH4a (all four pushed under `_PASSIVE`), Elsword CH4a, Raven CH4a |
| 7am | 930 | "March's own pushes (Elsword, Raven, Ara, Elesis, Add) are March's callbacks; a port replaces them with June's set" | - | CH4b (Rena: March had none) |
| 7an | 940 | vslot `0x428` `JuneGetDefenceRateBySkill`: Ara `32b956a0` (the override, whole; Ain `32a4d570`, EL `32b52260`, Aisha `32cc28b0` = Aisha CH4c) | `32b956a0` | Ara CH4c-type |
| 7an | 943 | spec-B reader `GetCharacterSpecializationB()` Ara `32ba6f31` ("the reading function, whole"); spec readers also in L278: A `32bc9a60`, B `32ba6ef0` (= `SetForcePower`) | `32ba6f31`, `32bc9a60`, `32ba6ef0` | Marks: Eve CH1q, Raven CH1q, Rena CH1q (A in `UpNaturalForce`), Chung CH1q |
| 7ao | 978 | transcendence Ara: `32bb22d0` hyper effect start (vslot 49) branch; `31fdd800` berserk table by class, only caller `32bca580`, port whole (funcmap L4578 `candidate`: unit type 7 `ASI_BERSERK_MODE`; when `CX2Unit +0x3c != 0`, by class `0x76 -> ASI_ASD_BERSERK_MODE` ...) | `32bb22d0`, `31fdd800`, `32bca580` | CH5/CH6; Raven / Rena marks "transcended return is CH1h's port" |
| 7ap | 991 | `JuneModifyFinalDamageByGameUnitType` (0x3ac) Ara `32ba45d0`: move March's live 4-arg `ModifyFinalDamageByGameUnitType` body to June's override (empty the March one, as Eve's LIVE does); 0x6a8 `JuneShownSkillCoolTime` Ara `32b958e0`, also called from its skill use `32bcff21` | `32ba45d0`, `32b958e0`, `32bcff21` | Ara (Eve CH1o2 ported the engine hooks; Chung CH1o `ShownSkillCoolTime`) |
| 7at | 1031-1035 | add-N-stacks buff path: `3229da40` (CX2GameUnit add N to a buff's stack count by BTI; erase at 0) calls `3236a2f0`; only callers `32bbb050` / `32bbb720` (`SA_ALD_WOLF_Break`, `P_AYR_GOOD_ENERGY_BALL` effects); March has neither; Ara port brings both and passes the count (june-hud H3 gave `PushBuff` a count: DONE 2026-10-31, archive L1052) | `3229da40`, `3236a2f0`, `32bbb050`, `32bbb720` | Ara-specific; unblocked |

## Part B - class-generic rows (every class / all 12 / every user / every character) that Ara must also satisfy

| Sec | L | Digest (what Ara needs) | Earlier marks |
|---|---|---|---|
| 2 | 116, 150 | `APPLIED_SKILL_ID` used by every character script (167 uses; "8c, done"); `31fcf3e0` reader | Chung marks per table |
| 6 | 214 | Standing check: class overrides `ReservedSkillStart` (June `32057b80 @32057c4c-cb2`: `SpecialAttackCooldown` + `SkillMemoMpRefund`) and extends the `X2GUUser` chain; `InitPassiveSkillState` reads `RNSI_*`; hand-off asks two casts per skill (memory `char-port-reserved-skill-start-hook`) | Eve / Chung / Elsword; Raven / Rena did |
| 7 | 243 | user `AnimPlay` `31fe8130` extra skin anims (`+0x8e84/+0x8e88`), `+0x8080` (every user) | none (engine) |
| 7 | 245, 247, 248 | second after-image / `+0x7c80` buff after-image / `+0x1d1` suspend-restore (every user) | none (4b-iii hand-off) |
| 7 | 255 | `IsSummonSkill` `31fc8200` (54 skill ids) + summon cases of `3230c1a0`; Ara's summon NPC ids: not found in the row | engine |
| 7 | 256 | per-class code writing the state power rate after `DoStateEndStart` (June cache `+0x7cc8`): keep June's operator `*=` / `=` in each ported state; no Ara address named | Elsword CH1b / CH3a-b, Raven, Eve, Chung each noted |
| 7 | 263 | skill-use check wrapped in `JuneIsDevUnlimitedSkillUse()` in Ara's 510 lookup `32bcfaa0`; `32bd09d0` has no such check (to confirm, as CH1n did for the others) | Aisha CH4a, Eve CH1l3, Chung CH1l3, Elsword CH1k, Raven CH1k, Rena |
| 7 | 274, 280 | ally-summon focus target (`32011960`, `31fd0590`); `3230c1a0` / `322ece70` summon stats ("every class with a summon"); no Ara summon found | engine |
| 7 | 278 | spec A/B readers: Ara `32bc9a60` (A), `32ba6ef0` (B) | Eve / Rena / Raven / Chung CH1q |
| 7 | 283 | `3201ad10` socket power rate callers by address range (no Ara address listed; "functions not decoded") | Chung, Elsword |
| 7 | 328 | skill-memo `31fdbbb0` (every user): lookup real since S3b / S3d; class code stays | S3 |
| 7h | 423 | `32024ff0` effect-set list variants (`EFFECT_SET_LIST_EX_TRANS_*`, `_FINAL`, `_FINAL_ULTIMATE`); Ara among "every class with June scripts" | Eve CH1r |
| 7h | 427 | `USEFUL_VALUE0..4` Future/Now readers in class code (`c9h4f_useful_value_readers.tsv`); read via `JuneGetUsefulValueFuture/Now` | per class state |
| 7h | 428 | `USI_WAIT_STATE_CHANGE` registered in every class InitState (all 12); all 13 June class scripts carry the table | Eve etc. |
| 7l | 472, 509 | bounding user states registered in every class's own InitState (all 12) + `<prefix>DAMAGE_BOUNDING_DOWN` in every class script | Aisha CH1m; Chung / Elsword / Raven / Rena CH1 |
| 7al | 902-903 | new `CX2GUUser` hooks for every class (`JuneAfterEquipSlotChange` 0x654, `JuneAfterImageFrameMove` 0x850, `JuneSummonedNPCInfo` 0x860, `JuneReservedSkillStart` 512, `UpdateEquipCollisionData` 0x638): override the ones June's Ara vtable overrides | CH1l (Eve, Chung, Elsword, Raven, Rena) |
| 7t | 624, 632 | cut-in name tables per class: "each class port replaces its table with June's names" (`g4_cutin_names.tsv`; funcmap L3555 routed to the character ports) | Elsword / Raven / Rena CH3d / CH5 |
| 7x | 667-673 | combos must carry `AT_COMMAND`; every class port takes June's script with its tags | counts per class (Eve 23, Chung 53, Elsword 67, Raven 46, Rena 52) |
| 7z | 700-704 | `UpdateEquipCollisionData` `31fd8c00` / `329657d0` pair exists for every class | engine R14c |
| 7ai | 874 | equipped-slot icon overrides `328e0aa0` per class type (Ara's class type not found in the row) | Eve |
| 7ao | 952-958 | every class port also brings: transcend allow-list id, `LevelUpEvent.lua` TYPE row + guide, `JuneGetHyperModeTableName` cases (June `31fdd490`), hyper cut-in names, SPRT_HYPER_CLASS skills / `TRANSCENDENTAL_SKILL_DATA` rows, four base skill helpers (not copied) | Elsword / Raven / Rena CH3d |
| 7ar | 1007-1009 | `GetX2UnitClassCompatibility` `31ed6500`: June cases for unit classes 33-46 / 123-136 / 200-202 belong to classes March lacks; Ara 34 and 124 fall in that range (inferred, the row does not name Ara): the Ara port adds those cases from June's switch under its flag; `june_march_divergent.tsv` row verdict `out` | none |

## Part C - rows matched only through an address in Ara's range (not naming Ara)

| L | Address(es) | Row | Verdict |
|---|---|---|---|
| 258 | `32ba4840`, `32bc5a50` | death prevention / Crest of Time snapshot | in Part A (L258) |
| 312 | `32bb0bd6`, `32bbcdc0`, `32bbcff0`, `32bbd240`, `32bbd3a0` | CEffect `+0xb18` writer | Ara (row says "not yet attributed, `32bb....`") |
| 867 | `32bc84b0` | `JuneShowSkillNameStateString` caller | Ara |
| 920 | `32b95910`, `32b95980`, `32b959a0`, `32ba7e40`, `32baf810`, `32baad30` | delegate callbacks | Ara (row names Ara) |
| 940, 991 | `32b956a0`, `32b958e0` | named for Ara | Ara |
| 264, 275, 455, 496 | `32b90420`, `32b87880`, `32b898a0`, `32b73bf0`, `32b906b0`, `32b909b0` | Elesis / EL rows | NOT Ara (below `32b90c20`) |
| 280, 919 | `32bdb250`, `32bdba10` | Chung range (>= `32bd86f0`) | NOT Ara |

## Part D - JUNE_PORT_BACKLOG_ORDER.md (1813 lines)

| L | Digest |
|---|---|
| 132 | "later paths for existing characters: Ara, Elesis, Add, Aisha, Chung, Eve and Rena" |
| 136 | Scope: "June class lines of March characters (Ara 34 / 124, Elesis 35 / 125, Add 33 / 36 / 123 / 126)"; third jobs `UC_150`-`UC_186` out |
| 140 | Model diff M3: no new base model for the 9 March characters; June adds new job motion files (Ara 2, Elesis 2, Add 4) |
| 144 | June-only `CUSTOM_RENDER_STATE` enum truncates June's Add and Ara scripts on March's `Enum.lua` (checklist) |
| 239-241 | training-game F6 / F7 block (`322ed400`): "Ara `32a31a00` + 1" (the force power counter); March: F6 = `ChargeMpAndEtcInTrainingGame` with Chung / Ara / Add overrides (`X2Game.cpp:5775-5800`); "every class; port the block and `322ed400` whole, with stand-ins for classes not ported" |
| 367 | Gap scan: Aisha 1, Eve 3, Rena 12, **Ara 14**, Chung 15, Elsword 20, Elesis 20, Raven 25 |
| 370 | "The class-path step (CH-N) is not exercised... written as the template block CH-N, and Ara, Elesis or Add runs it first in Stage 7" |
| 452-453 | June's equip-view rewrite: `UpdateEquipViewListInfo` `3201b930` takes a third `slotId` argument on vslot 0x644 for every override: Chung `32bdc750`, **Ara `32bc89e0`**, Arme `32cc9950`; verdict `stage8` (JUNE_CHARACTER_LUCIEL DU3 L386 owns the engine side; funcmap L6697 / L6698 `differs: Stage 8`) |

### "Open decisions that gate stages" (heading L1561): stat rows for June-only classes

- L1566 (table row, quoted whole): "**Stat rows for June-only class IDs.** The user authorised inventing `StatTable`/`NpcExpTable` rows 81-99 only for classes that already exist (Atlas A2). The later class paths and the new characters need a **full** per-level row set, and `KncWX2Server/ServerResource/US/` has none for them. | 5, 7, 8 | First look for June-side data: the June binary, any June server Lua, or the live DB. If there is none, ask the user whether A2's permission extends to these classes, and how the rows should be derived."
- L358-359 (Stage 5 per-character list): "For classes March already has, rows 81-99 follow Stage 1's A2 method. For a **new** class ID (a June-only later path), see the stat-row decision below. A2's permission to invent rows does not extend to it."
- L32: "The user authorised invented `StatTable`/`NpcExpTable` rows 81-99, for those two tables only."
- L371: "**Stat rows:** every Aisha class already has rows 1-99." (no Ara equivalent stated)
- Ara 34 / 124 are June-only class IDs, so they sit inside this open decision. Ara-specific text: JUNE_CHARACTER_AISHA_PHASES.md L2442 (CH-N template): "A2's permission does not extend to a new class ID. First look for June data (June binary, June server Lua, the live DB). If there is none, ask the user, then use A2's generator (`june2017_re/cap99_gen.py`) only with their yes. Until then the loader falls back to the base class's rows and logs it."
- Precedent for the same question, **settled for Lu/Ciel (user, 2026-11-02)**: JUNE_CHARACTER_LUCIEL_PHASES.md L252-256 and L520-523: June has no per-level table (`CharacterStatTable` is gear-stat scaling, one relative row per unit type); every existing `StatTable.lua` row (50 classes, levels 1-80) = that relative row x (0.8 + 0.0225 (L-1)), HP x (1 + 0.15 (L-1)); "Settled (user, 2026-11-02): derive from June. NU4 writes Lu/Ciel's `StatTable.lua` rows from June's `CharacterStatTable` relative rows with the fitted formula (levels 1-80; 81-99 by `cap99_gen.py` as for the other classes), a labelled stand-in under its own flag; the user packs it." Whether that answer covers Ara 34 / 124 is not stated anywhere I found: Ara's planning session must cite it or ask (AISHA L2442 still says "ask").
- Starter items (backlog L1565): not needed for a later path (AISHA L2445: "a later path is reached by job change, not creation").

## Part E - STATUS.md / STATUS_ARCHIVE.md / JUNE_BUGS.md / MODS.md

STATUS.md (only 4 rows reference Ara; no `char-ara` block yet):
- L109 (char-rena Z): "close-out, `MODS.md`, template row, `stage7-plan` -> `char-ara`".
- L129 (`stage7-plan`): "queued: `/x2-phase new-plan char-ara` (first CH-N), set ready by char-rena Z".
- L153 (`R6b`, june-engine, ready): June writers of the mesh fields R6 reads: CX2Eqip mesh-flag switch `32964ae0` (**alpha ids, Ara cases gone**), equip glow `3295b970`, `31fd0880`. March's Ara `SetUseRenderParam` cases are no longer read; the Ara glow double pass is "Cosmetic, Ara only; routed to R6b; the end smoke's Ara weapon check expects this until R6b" (JUNE_MARCH_DIVERGENCE L654-655; JUNE_ENGINE_REMAINDER L1257, L1272, L1330; archive L1028).
- L239 (char-luciel `CH3c20`, blocked on CH1 review): "`SLockOnSystem::ProcessLockOn` out of `CX2GUAra`, Garden of Haures NPC binders (4 unbound)"; touches `X2GUAra.*`. Ara keeps her two Swallow Assault call sites (checklist L775-778).
- No STATUS row for june-hud H3 (closed plan: archive L1050-1052; H3 done 2026-10-31, play-tested on EveEmpress BFI 877). The Ara route is checklist 7at and JUNE_HUD_PHASES L174, L306 ("`3236a2f0` is out, char-port material").

STATUS_ARCHIVE.md Ara hits (L): 18 (6b cooldowns), 24 (7c: Ara's six awakening sites), 26 / 28-29 (7d / 7e: Ara `+0x8d78` test moved to `CanUseSpecialAttack`), 32, 38 (10: Ara `GetReduceDebuffDurationRate`), 68, 129 (C3h `APPLY_MOVEMENT_SPEED`: Ara `ASI_AIRIAL_DASH`), 184, 188 (C3t-2: Ara Z-Z-Z play-test, `ASI_BERSERK_MODE`), 199, 542 (L2), 612 + 619 (stage5 / stage7 planning: Ara listed, gap 14), 658 (R6b), 729, 807 (CH-N first run by Ara / Elesis / Add), 840 (CH4b: unit 24 Ara `1 entries, 1 bound`), 964 (CH1o2: March AT_NORMAL call kept for Ara), 1028 (C4: Ara glow double pass routed to R6b).
Important: archive L542 and LUA_SOURCE_PHASES L501-509, L572: **`Ara_Martial_Artist` is not T1** (constructor order; T2 order-only); "Ara's route is C3d-4's call". Check `luasrc.py ledger` before CH1 adoption.

JUNE_BUGS.md: L28 (#11 awakening-impossible sites), **L29 (#12: Ara's six awakening sites `32bb2d40`..`32bb3ab0` never set `+0x8d4c`; fixed since Phase 7d)**, L120 (#103 create-damage-effect rate set by class code `32bb0bd6`, latent). L138, L170, L564 mention Ara only as an example pattern.
MODS.md Ara lines: 119 (quick-slot expansion ticket, Ara / Elesis), 2133-2158 (play-tests with Ara `ASD_Dragon_Falling_Explosion`, `SA_ALD_WOLF_Break`, `EffectSet_SA_AYR_ORB_KI_FX_DAMAGE`), 2572-2573 (`0x780` `JuneDamageExceptionClassHook` empty, Ara overrides it), 3443 (Ara skill-chain ids off by 3 if data036 and exe mismatch), 3499 (Enum.lua: Ara state ids differ), 3505 (BattleFieldData Ara / Ruben play-test), 4370 (state power rate cache readers in Ara), 4691, 5354 (`FORCE_HYPER_MODE`: Ara's berserk state), 5449 (`WEAPON_EFFECT_SHOW`: Ara weapon enchant frame move).

## Part F - funcmap.tsv (9401 data rows)

45 rows mention `CX2GUAra` or the word Ara. By status: confirmed 29, ported 5, PORTED 3, candidate 2, dead 2, `differs: Stage 8` 2, mapped 1, same 1. Only 13 rows have an address inside `32b90c20`-`32bd86ef` (11 confirmed, 1 dead, 1 candidate that is a std::vector at `32bd7920`, not Ara): Ara's June class is almost entirely unmapped (CH0 carves it).
Rows that say routed / not ported / character port / candidate (funcmap line, address):
- 1083, `Ara+0x9621`: candidate, "June-only, character-port territory; not ported" (byte beside `m_bFPCancel` `+0x9620`, set 1 before the window test, 0 on failure, e.g. `@32bac143` / `@32bac199`; a word write at `+0x9620` sets both).
- 4578, `31fdd800` (Transcend S6): candidate, berserk-mode table name by class (checklist L978).
- 1347, `32045340 @32045488`: Ara's vslot `0x780` `32bca010` (27415 / 27417 blocks); "C8b adds the empty hook; Ara's body is C8c" (character port).
- 3555, `32014fc0` + 7 class functions (GFx G4): per-class cut-in name tables "routed to the character ports (checklist 7u)"; March's Ara already calls the matching overloads.
- 6697 `31fd2f00`, 6698 `31ffe990`: `differs: Stage 8` (equip view; Ara override `32bc89e0`).
- 7009, `3200fa20`: `SLockOnSystem::ProcessLockOn` (March `X2GUAra.h:428` / `.cpp:5435`), shared by Ara and Luciel, CH3c20 hoists it, callers `32b07a40` `32b15290`.
- 6695, `32b90da0`: dead (Ara slot 0x688 thunk to `31fcd740`, never called); 5818, `32bd6de0`: `ResetChainingFromNext`, caller is Ara `CommonFrameMoveFuture 32bb0bf0` (not in funcmap).
- Rows that fix Ara's shape: 839 (`32ba6e50` vslot 197 `ApplyRenderParamOnEmptyRenderParamByBuff`, March `X2GUAra.cpp:5818`), 1019 (`32baafc0`), 1040 (`32ba46c0` `GetHypetMpdeStateID` slot 430: count >= 3 ? `0x9b` : `0x17`), 1041 (`32bb2d40` awakening site 1 of 6), 1062 (`32b90c20`), 1082 (`32bcfaa0`), 1126 (`32bd32f0`), 2022 (`32bd09d0`), 2442 (`32bd85f0`), 2485 (`DamageReact` Ara avoidance guard: June's `31e4d0e0 == 7` (UT_ARA) before protected bool `+0x96f0` = `m_bCanAvoidance`, ported under `JUNE_DAMAGE_REACT_ORDER`), 3718 (RM1 `OnSkillUse` includes Ara's class site), 6103, 6192 (enchant frame moves for Ara).

## Part G - other plan docs that leave pieces to the Ara port

- JUNE_CHARACTER_AISHA_PHASES.md: L26 ("Ara, the smallest character that would exercise every template step"), L36, L2428-2450 (CH-N template: candidates Ara 34 / 124; class enum and `UC_END` arrays (the socket loader drops classes >= `UC_END` 124), `Motion_Ara_LS.x`, job change (`CJT_*`, `X2OfflineClassChangeSeed.h`, `X2OfflineLuaEnumSeed.h`), skills (regenerate `s3_reform_ids_no_state.tsv`; `SeedJuneAutoLearn` / `june_skill_autolearn.tsv`), stat rows open), L2586 (Ara row: gap 14, classes 34 / 124, `CUSTOM_RENDER_STATE` (7w), 2 new motion files, orb handle retirement needs the user's OK (7e)), L2065 (7am registrations per class), L2106 (unit 24 = Ara).
- JUNE_CHARACTER_{CHUNG,ELSWORD,EVE,RAVEN,RENA}_PHASES.md: "Ara runs it [CH-N] first" at CHUNG L3429, ELSWORD L2560, EVE L2722, RAVEN L2370, RENA L2462. Eve L1375 / L1383 / L1402 / L1408 (0x3ac Ara `32ba45d0`, 0x6a8 Ara `32b958e0`; "March's AT_NORMAL 4-arg call stays for Ara, `JUNE_FINAL_DAMAGE_UNIT_HOOK`"), Eve L199 (`329cf6d0` PushBuffFactor shared with Rose and Ara), Eve L1553 (slots 0x4c4 / 0x4c8: Ara overrides). Chung L21 (class range `32bd86f0`-`32c25a1f`, Ara before), L1333 / L1362 (`32b90da0`). Elsword L1348 / L1387 and Raven L1294 (stand-in comments list Ara: unported classes reach `CountUsedSkill` / `GetPressedSkillState` / `m_sCurrentSkillKeyManager`); Raven L2019 (an awakened Ara keeps `EP_AC_LEG` particles, `IsFullHyperMode`, `UT_ARA`); Rena L1394 (Ara's override ignores the argument).
- JUNE_CHARACTER_LUCIEL_PHASES.md: L386 (DU3 equip-view: Ara `32bc89e0`), L252-256 + L520-523 (stat rows precedent), CH3c20 (STATUS L239).
- JUNE_HUD_PHASES.md: L174, L306 (`3236a2f0` is Ara skill code, 7at).
- JUNE_TRANSCENDENCE_PHASES.md: L316 (berserk-end redraws for Chung and Ara in `X2GageUI.cpp` pass my unit's flag to the 5-arg `GetCharacterImageName`).
- JUNE_ENGINE_REMAINDER_PHASES.md: L111 / L1233 / L1235 (Enum truncation; "class-specific use (Add, Ara, Ain, Lu/Ciel, Rose) goes to the checklist"), L131 (Ara 9/9 motions 223 -> 281: `Motion_Ara_LS.x`, `Motion_Ara_AA.x`), L1257 / L1272 / L1330 (R6b: `ARA_CHARACTER_BASE`, March's Ara `SetUseRenderParam` hair 41510 no longer read), L3021 ("Class code -> checklist 7ac: Ara `ProcessLockOn` + Swallow Assault x2, June-only helpers").
- JUNE_MARCH_DIVERGENCE_PHASES.md: L253 (`SetFullHyperMode` `+0x9700` and `IsFullHyperMode` arm (Ara port, checklist)), L553 (4-arg `ModifyFinalDamageByGameUnitType` Ara override), L632-655 (Ara glowing weapon expects the R6b cosmetic until then), L815 (Ara awakening set renders undimmed until ported), L1082, L1304, L1521 (Ara checks not covered).
- JUNE2017_EFFECT_COMPLETION_PHASES.md: L115 (D8: Ara's `0x780` body handed to the character ports), L837 (`CUSTOM_RENDER_STATE` cuts off Ara's script), L1731 / L1772-1773 (`ASI_AIRIAL_DASH` reaches play with the Ara port, 7o), L3196-3206, L3423 (Ara `ASI_BERSERK_MODE` override is 7t), L6219 / L6233 (vslot 0x780: only Ara overrides).
- JUNE_BUFF_PORT_PHASES.md: L898, L2778-2887, L2909, L2933, L3218-3266 (Ara's six awakening sites and `+0x8d78` placement), L2769 / L6077 (Add, Ara, Rose, Luciel overrides of `ChangeMaxCharacterAddResource`; "March Ara's class port will override the hook"), L4517 (Ara readers to checklist 7), L6233 (`SetSkillLevelStateData` hoist, Ara copy), L3384 / L3675 / L3731 (Ara's 27415 tail is in 7). JUNE_BUFF_PORT_ARCHIVE.md: L402, L409 (hyper count readers: Ara 8, Chung 1, Add 1; "Ara's vslots 49/50 exist in March and were reworked in June; they are character-port work"), L1410.
- JUNE_SKILL_SYSTEM_PHASES.md: L1057 (June's own Ara memo list is dead; MemoCondition not ported), L2502-2504 (Ara chain info shared through `shared_ptr`; 11 sites).
- LUA_SOURCE_PHASES.md: L501-509, L572 (Ara script not T1; C3d-4's call).

## Known gaps / not found

- No row names ALH / ASD / ALD / AYR / Little Hsien / Sakra / Yama Raja as class keys (L368 says "Ara (Little Devil)"); only skill-state names.
- No `(Ara: CHn)` marks exist. No stat-row decision for classes 34 / 124 has been answered; only Lu/Ciel's (2026-11-02).
- Checklist L854 labels `32a4dcaa` as Ara but the address lies in Ain's range: verify in CH0.
- `kickoffs/` has no `char-ara` directory; `june2017_re/char_ara/` already held other planning-session files (`plan_charport_keys.*`, `plan_numerical_rows.tsv`, `plan_charport_scan_20261110.txt`), untouched.
