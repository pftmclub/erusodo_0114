# Raven (char-raven): checklist rows

Source: JUNE2017_CHARACTER_PORT_CHECKLIST.md (1044 lines; "L" = current line numbers, 2026-11-05; Elsword's copy cites the 1038-line file, so its L numbers run 0-6 lower after about L240).
Also grepped: JUNE_PORT_BACKLOG_ORDER.md, STATUS.md, STATUS_ARCHIVE.md, JUNE_BUGS.md, stage7-plan.md, the Aisha/Elsword/Chung phase docs, MODS.md, funcmap.tsv. Read-only; only this file written.
Grep terms: Raven, RAVEN, RRF_, RBM_, RVC_, RWT_, Blade Master, Reckless Fist, Veteran Commander, Nasod, Overheat, Shadow Step, `32c2..32c7` addresses.
June class sits near `32c2b360`-`32c641a0` (Chung ends `32c25a1f`; Rena/Lire follow). Class ids not verified here (read `X2Unit.h` in CH0).
Phase vocabulary is the Elsword plan's. No `(Raven: CHn)` mark exists yet; rule L211 says `(Raven: CHn)`.

## Part A - rows naming Raven (25)

| Sec | L | Row text (trimmed) | June addresses | Suggested phase / earlier marks |
|---|---|---|---|---|
| intro | 4 | "June update of a March one (Elsword, Aisha, Rena, Raven, ...)" | - | n/a (scope) |
| intro | 14 | Character scripts (`RAVEN_FIGHTER.lua`, ...) feed `SetDamageDataFromLUA` | - | CH0 (adopt script, core-list check) |
| 1 | 50 | `RAVEN_FIGHTER` 25 numerical-only DAMAGE_DATA tables / 70 other | `32632390` | CH2/CH3 (each skill's tables run the engine; log the `NUM use` line). Gap 25 = biggest plain character (stage7-plan:28) |
| 2 | 120 | `ADDITIONAL_ATTACK_TRIGGER_TYPE` x7 in `RAVEN_FIGHTER` (8b group 3), status "to-implement" | reader `32610fb0` (ported, `DamageData::JuneIsAttackTrigger`) | status looks STALE (funcmap 1278 says PORTED, effect C5); data arrives with script (CH1), consumed in CH4b |
| 2 | 122 | `IGNORE_DEFENCE_RATE`, `_ON_PVP` x2 each in `RAVEN_FIGHTER` (8b group 1), "to-implement" | - | status looks STALE (Elsword rows 124/125 say engine DONE); confirm in CH0, use in CH2/CH3 |
| 7 | 256 | Per-class code writing the state power rate after `DoStateEndStart`; March's Raven ASSIGNS `m_fAttackPowerRateShadowStepThrust`, `m_fShadowBusterPowerRate`, `m_fAttackPowerRateShadowStepPunisher`, dropping buff terms; June multiplies cache `+0x7cc8` | e.g. `32ad6ac0`, `32b1cd20` | CH0 list the 3 states, CH2/CH3 keep June's operator (`*=` or `=`). Raven is the class the row names. Marks: Elsword x5 (CH1b, CH2a, CH3a-c "none write it") |
| 7 | 262 | Every class's vslot 510 lookup; Phase 7d put the quick-slot prefix at the top of the eight March classes' `SpecialAttackEventProcess`, Raven among them | Raven's lookup address not listed | CH1l (lookup body) / CH1o (virtual 510); prefix already in March Raven. Marks: Aisha CH1i, Eve CH1o, Chung CH1l3 |
| 7 | 286 | `3201f850` `SetSkillLevelStateData` hoisted into `CX2GUUser`; March copy `X2GURaven.cpp:22987`; multiply by `( 3201ad10( skill ) + 1.0 )` | `3201f850`, `32028190` | CH1l (Raven's copy: add the term or hoist). Mark: Aisha CH1i |
| 7b | 327 | Burning Nasod Hand: `32c57660` (`+0x9688` = skill 24401 learned); `32c43120 @32c43ac4` (`+0x4108` = 2 with `ChangeDamageType`); gate `32045340 @320453a0`; June `RAVEN_FIGHTER.lua` marks 46 attacks `AT_COMMAND`, March marks none, so the `= 2` write alone switches the passive off | `32c57660`, `32c43120`, `32045340` | CH4a with CH1 script (the 24401 setter and the `= 2` write together; MODS.md:2559 "belongs to the Raven port"). No marks |
| 7b | 330 | per-class SetDamageExceptionProcess blocks incl. funcmap `32c5c080` (Raven): passives keyed on `RNSI_*` | `32c5c080` (merged with `32c5bf80`); funcmap 201: 24417 `RNSI_P_RBM_OPEN_MIND` (skill type in {7,14..16}, vslot 0x524) -> `32007b10(0xf,24417)`; 24420 `RNSI_P_RRF_INFINITY_RAGE` + trigger 0x12 -> DT_MAGIC, `DamageEffect_RRF_ExplosiveHand_Impact_NEW`; 24422 `RNSI_P_RVC_WIN_AND_WIN` + trigger 0x13 -> vslot 0x238 -> `329cf6d0` | CH4b (March has NO Raven override; follow Elsword's `32d1e0e0` shape). Marks: Aisha CH4c, Eve CH4b, Chung CH4b, Elsword CH4b |
| 7h | 422 | Raven `32c2b360`: `UPDOWN_CANCEL_AFTER` (`+0x8ca4`) up/down cancel branch in a state EventProcess; data June `RAVEN_FIGHTER` | `32c2b360` | CH2/CH3 (inside that state's EventProcess; member exists). Marks: Elsword CH3a-c |
| 7i | 444 | `SetEquippedSkillFuncBySkillSlotIndex` Raven `32c641a0`: drop snapshot call, set state byte `+0x11` | `32c641a0` (funcmap 2024, vslot 0x7b4) | CH1n (head). Mark: Aisha CH1k |
| 7o | 510 | fall-speed limit: Raven `RRF_DASH_JUMP_COMBO_XX` = -5000 (plus all-12 `DAMAGE_BOUNDING_DOWN` -4000, L507) | - | CH1 final (arrives with the script) |
| 7s | 579 | `X2DungeonManager.cpp:252-552` reads RAVEN already | - | n/a |
| 7t | 623 | cut-in: June `HQ_CutIn_Raven_BM_01`; March `HQ_CutIn_Raven_Blade_Master_1.dds` (no `.gfx`) | `329cd890`+`32078b70`; funcmap 3555 (G4) | CH5 (June `.gfx` names, `m_HyperCutIn` rows in core-list `UnitTemplet.lua`) |
| 7u | 636 | March-only templets dropped: `RAVEN_SEVEN_BURST_1/2_HYPER`, `RAVEN_MAXIMUM_CANNON_HYPER`, `_FLY_HYPER`, `RAVEN_OVER_TAKER_GUARDIAN_STRIKE[_HYPER]`, `RAVEN_OVER_TAKER_XX_FRONT_X`, `EffectSet_RAVEN_VJAVELIN`; June uses `DamageEffect_RRF_ExplosiveHand_Impact`, `EffectSet_RAVEN_OVER_TAKER_GUARDIAN_STRIKE(_HYPER_MEMO)`, `EffectSet_RAVEN_VJAVELIN_LAND`, `Effectset_A_RWT_FLAME_SWORD_MEMO` | `c11a_march_only_templets.tsv` | CH3 (take June's caller, drop the March names). JUNE_BUGS 200 (L217) |
| 7ae | 718, 723 | census: base classes 15 / 14 (Raven 14 / 13); an unbound id falls to `default:` and plays the class's first state (Raven) | `s3_reform_ids_no_state.tsv` | CH2 (first jobs) / CH3 (second jobs) bind each id; check unnamed ids in CH0 |
| 7ad | 749, 756, 758 | R12b already took June's `Motion_Raven.x`; other `Motion_Raven*` still March's; Raven's `OT_ComboX3Down`..`OT_ComboX5Down`, `OT_SP1a_ChargedBolt` renamed | `r12b_anim_loss.tsv` | CH0b (stage June's Raven motions with the script, deploy with CH1; Aisha-doc L318 lists Raven) |
| 7am | 909, 922 | delegate list `32c57660`: A `32c3df50`, C `32c4e5b0`, A `32c28160`; March's own pushes (Raven among them) are replaced | those 3 | CH4 (each callback whole, replace March's pushes) |
| 7an | 933 | vslot `0x73c` override Raven `32c3c7f0` | `32c3c7f0` | CH4a (override, whole). Mark: Aisha CH4c |
| 7an | 934 | vslot `0x748` soul gain when hit, Raven `32c3c820` (= `1 + +0x95c4`, Chung-doc L3301); base and `DamageReact @32049d8a` call already ported | `32c3c820` | CH4a (override only; row says "Ain / Raven follow in their ports"). Mark: Chung CH4b |
| 7an | 935 | `GetCharacterSpecializationB()` readers Raven `32c49410` / `32c507da` | both | CH1q (class mechanic) / CH4 (reading functions whole). Marks: Eve CH1q, Chung CH1q, Aisha CH4c |
| 7ao | 954 | T5 branch: `HyperModeStart` `31ffd490`, skill 24419 (`0x5f63`) learned while awake -> vslot `0x23c( 24419, 1, 1 )` | `31ffd490` (funcmap 4643) | CH3d. No marks |
| 7ao | 966 | transcend: Raven hyper effect start `32c4e7c0` (vslot 49) returns at once for a transcended unit | `32c4e7c0` | CH3d. No marks |
| 7aq | 988-993 | three-orb ball: `32e6d900`, `raven_powrUp_UI_01` at (74, 60) x resolution (`3352d6c0` / `33457f50`; March 100, 120); March `CX2RavenMyGageUI::UpdateHyperOrb` uses `GetUIMajorParticle()` but the template is only in `GameMajorParticle7.txt`, so the ball is never made | `32e6d900` | CH1q / CH5 (port June's body; Eve did `June_UpdateHyperBall` `32e6a210`) |

By evidence, not by name: 7x (L667, `RAVEN_FIGHTER` 46 hits need `AT_COMMAND`), 7ag (L804-827), 7ai (L842), and L702 (`m_RavenLeftArmItemID` basic-item pair, every class).

## Part B - rows that apply to every class (current L numbers)

Pure every-class: 2 (L109; APPLIED_SKILL_ID L116, every script except Add_NasodRuler / Ain_Angel / Rose_Mechanic / Rose_Ranger), 3 (L144), 4 (L158), 5 (L182), 6 (L199, mark rule L211), 7k (L460), 7l (L466; bounding states in all 12, L472), 7o (L501, L507), 7p (L536, `TIME_STOP_IMMUNE_STATE` L546), 7x (L667), 7ai (L842), 7av (L1035; Raven has no 0x824 override listed).
Section 7 intro table (row line = 235 + table index): L243 AnimPlay, L247-248 after-image, L256 (Raven-named), L258 death prevention `320072d0`/`3202b9a0` (no Raven override listed), L263 skill-use check + `JuneIsDevUnlimitedSkillUse`, L274 summon-NPC-target, L278 Spec A readers (Raven not listed), L280 summon stats `3230c1a0`, L282 `32011280`/`322a0180`, L283 `3201ad10` callers (Raven not listed), L286 (Raven-named), L293 tutorial flag.
Mixed: 7a (L296), 7h (L426 `USI_WAIT_STATE_CHANGE`), 7i (L442), 7t (L586-600), 7ae (L712), 7ag (L804), 7ad (L746), 7al (L888-895 new CX2GUUser hooks), 7am (L897), 7ao (L939-947 "Every class also brings": allow-list id, `LevelUpEvent.lua` TYPE row + `DLG_Transcendental_Change_Guide_Raven`, `JuneGetHyperModeTableName` cases, `HQ_Cutin_*_Transcendence`, SPRT_HYPER_CLASS skills + `TRANSCENDENTAL_SKILL_DATA`, helpers `31fcac90` / `31fcad70` / `31fceab0` / `31ff1e80`).
Not Raven: 7q (Lu/Ciel), 7r (Chung), 7v-7ab (Lu/Ciel/Rose), 7at (Ara), 7au (Ain), 7m (Add), 7n (Rena/Eve/Chung/Elesis), 7d/7f/7g, 7aq Eve half (done).

## Marks
Raven rows carry no Raven mark. Other marks seen on Raven-relevant rows: L256 Elsword; L262 Aisha/Eve/Chung; L286 Aisha; L330 Aisha/Eve/Chung/Elsword; L422 Elsword; L444 Aisha; L933 Aisha; L934 Chung; L935 Eve/Chung. Marks elsewhere not tallied.

## Explanations
- 7b L327 / funcmap 1343-1345: `32c43120 @32c43888-32c43ac4` is the Lua-init `ResetDamageType` / `ChangeDamageType` (March `X2GURaven.cpp:5111-5116`); `32c57660` is Burning Nasod Hand setup (March :2287-2335). On March data Raven's `+0x4108` stays -1 so gate `32045340 @320453a0` is inert (MODS.md:2555-2559). Port the 24401 setter and the `= 2` write with the script's AT_COMMAND tags, never one alone.
- Same as March, no work: funcmap 1242 `32c273f0` GetReduceDebuffDurationRate (same 12 BTIs); funcmap 843 `32c3deb0` ApplyRenderParamOnEmptyRenderParamByBuff (vslot 197).
- `DamageEffect.lua` stopped loading at line 351020 (`INIT_PHYSIC.WALK_SPEED`, a Raven combo). FIXED 2026-10-28 by char-eve CH3a (`SERV_IRUHADEV_JUNE_DAMAGE_EFFECT_INIT_PHYSIC`, 500 = Raven's `RAVEN_FIGHTER.lua` value). CH1 still runs the Lua load check after adoption.

## Routed to Raven from other docs (file:line)
- JUNE_PORT_BACKLOG_ORDER.md:405 ("Raven's port owns it": INIT_PHYSIC; fixed, check the dropped blocks beyond Raven); :361 (gap scan Raven 25); :236 (hyper-ball counts, "Raven `32cb6290( 1 )`": verify the class in CH0).
- kickoffs/june-backlog/stage7-plan.md:5 (`char-raven` in the order), :28 (order #4, "Gap 25, the biggest plain character").
- STATUS.md:116 (queued `/x2-phase new-plan char-raven`, set ready by char-elsword Z), :180 (char-elsword Z: `stage7-plan` -> `char-raven`); kickoffs/char-elsword/Z.md:24; JUNE_CHARACTER_ELSWORD_PHASES.md:2578.
- JUNE_CHARACTER_AISHA_PHASES.md:2576 (Known extras: Raven 25 | 7b passive setter `+0x4108`; 7u effect names; power-rate operator); :318 (CH0b and CH1 deploy together); :2065 (7am per class); :2562 (Lua load-check lesson).
- JUNE_CHARACTER_CHUNG_PHASES.md:185 (boundary `32c25880`-`32c25a1f`, Raven's first override); :3231 (Raven 24404 is Raven's own override); :3257, :3301 (Raven `32c3c820` stays on row 7an); :1925 (stand-in lists Raven as unported).
- MODS.md:2557-2559 (`= 2` write belongs to the Raven port); :1992 (`SERV_RAVEN_VETERAN_COMMANDER` ON); :3173 (`EffectSet_RAVEN_NUCLEAR` path).
- june2017_re/funcmap.tsv:201 (`32c5c080`), :1343-1345, :2024 (`32c641a0`), :1278, :4643 (`31ffd490`).
- JUNE_BUGS.md:217 (row 200 `DamageEffect_RRF_ExplosiveHand_Impact`). No JUNE_BUGS row names Raven's own code.
