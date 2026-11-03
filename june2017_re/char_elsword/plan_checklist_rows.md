# Elsword (char-elsword): checklist rows

Source: JUNE2017_CHARACTER_PORT_CHECKLIST.md (1038 lines; "L" = that file's current line numbers, 2026-11-03; Chung's copy cites the older 1019-line file, so its L numbers are 2-3 lower after row 7ao).
Also grepped: JUNE_PORT_BACKLOG_ORDER.md, STATUS.md, STATUS_ARCHIVE.md, JUNE_BUGS.md, kickoffs/june-backlog/stage7-plan.md, JUNE_CHARACTER_AISHA_PHASES.md, MODS.md, funcmap.tsv. Read-only; nothing edited except this file.
Grep terms: Elsword, ELSWORD, SwordMan, Sword_Man, Rune Slayer, Lord Knight, Infinity Sword, Sheath Knight, Magic Knight, Way of the Sword / WOS, ESSI_, RunSlayer, S3d, Crest. Classes: UC_ELSWORD_SWORDMAN 1, KNIGHT 10, MAGIC_KNIGHT 11, SHEATH_KNIGHT 22, LORD_KNIGHT 100, RUNE_SLAYER 101, INFINITY_SWORD 112 (June class `32d0d660`-`32d27a..`, vtable `33522914`; Aisha ends `32cf2da0`).
Phase vocabulary is the Chung plan's. "Aisha/Eve/Chung" marks are written by the phase that ports the row.

## Part A - rows naming Elsword

| Sec | L | Row text (trimmed) | June addresses | Suggested phase |
|---|---|---|---|---|
| intro | 4 | "June update of a March one (Elsword, Aisha, ...)" | - | n/a (scope) |
| intro | 14 | Character scripts (`Elsword_SwordMan.lua`, ...) feed `SetDamageDataFromLUA` | - | CH0 (adopt script) |
| 1 | 51 | `Elsword_SwordMan` / 20 numerical-only DAMAGE_DATA tables / 87 other | `32632390` numerical override | CH2 / CH3 (each skill's tables run through the engine; log the `NUM use` line) |
| 2 | 124 | `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID`, `_RELATIVE_SKILL_ID` x1 each in `Elsword_SwordMan` (8b group 2) | `32630e0e-3263168a` | engine DONE (`X2DamageManager.cpp:5068-5140`); data use is `Elsword_SwordMan.src.lua:5182-5186` (RNSI_P_ELK_FIGHT_GOD 21416, BTI_SI_A_ES_ENDURANCE): CH3 (Endurance, Lord Knight) with CH4 Fight God |
| 2 | 125 | bold `..._RELATIVE_BUFF_TEMPLATE_ID` x1 in `Elsword_SwordMan` | same | same (parsed at `X2DamageManager.cpp:5114`) |
| 2 | 140 | `DAMAGE_SKILL_ID` x103, "all in `Elsword_DamageData.lua`" | see below | CH0 verdict: stale, n/a (see Explanations) |
| 7 | 256 | Death prevention; Crest of Time snapshot slot 0x740 per-class overrides, Elsword `32d058a0` | `320072d0`, `3202b9a0`, `3201ffb0`, `32009780`, `32d058a0`, `3204da60` | CH1q (write Elsword's gauge snapshot/restore as Chung did, `StoreGageSnapshot`/`RestoreGageSnapshot`); the cast and slot-250 hook are char-add's (Chung CH4b routed it); (Chung: CH4b) mark is on this row |
| 7 | 260 | vslot 510 skill lookup prefix `IsQuickSlotSpecialItemWaiting` already at top of Elsword's `SpecialAttackEventProcess` (`X2GUElsword_SwordMan.cpp:4965-4972`; June lookup `32d1bd10`) | `32d1bd10` | CH1l (lookup body) / CH1o (virtual 510); Aisha/Eve/Chung marks present |
| 7 | 262 | `32d27130` Elsword vslot 0x4ac `GetSpecialItemUsableExtraStateID` returns `0xce` (June-only state; March Elsword enum ends `0xc1`) | `32d27130`, `322fa960` | CH1o (override) once state 0xce exists (CH1l/CH2) |
| 7 | 281 | skill-group power socket rate `3201ad10` called from Elsword range `32d076f0`; use `( rate + 1.0 )` | `3201ad10`, `32d076f0` | CH2/CH3 (the skill state containing `32d076f0`; decode it in CH0); Eve/Chung marks present |
| 7 | 291 | tutorial flag read in `CX2GUElsword_SwordMan::DamageReact` `32d1af6c`: special reaction needs `+0x80bc && !tutorial` | `32d1af6c` | CH1m/CH1o (DamageReact override); Eve mark present |
| 7a | 306 | `32d18230` `RunSlayer_RUNE_GUARD_FIRE` pushes modifier tag 19 (main-effect life-time rate, inherited by children) | `32d18230`, `321db1b0` | CH3 (Rune Slayer Rune Guard); tag 19 members `m_bInheritMainEffectLifeTimeRate`/`m_fMainEffectLifeTimeRate` land with it (not yet ported; Eve did 5/12/16/20, Aisha 2) |
| 7b | 328 | per-class SetDamageExceptionProcess blocks, funcmap `32d1e0e0` (Elsword); passives keyed on `RNSI_*`, brief `kickoffs/june-effect/C8c.md` | `32d1e0e0` (merged with `32d1ddf0`); funcmap 204: 21419 `RNSI_P_EIS_OPEN_MIND` + `ATTACK_TRIGGER_TYPE` (+0x53c) == 0xf -> +0x510 = 1.0; 21420 `RNSI_P_EIS_INFINITY_POTENTIAL` + +0x53c == 0x10 -> vslot 0x238(21420, 4, 1) -> `322b5320` push | CH4 (passives; March has NO Elsword override, `X2GUElsword_SwordMan.cpp` has 0 hits of SetDamageExceptionProcess; needs effect C5 ATTACK_TRIGGER_TYPE values 0xf/0x10); Aisha/Eve/Chung marks present |
| 7c | 352 | 29 alpha-tag sites `32d0d660`..`32d1ecc0` | `c10a_alpha_sites.tsv`: 29 rows = 19 mesh instance + 8 particle ptr + 2 particle handle, in 14 functions (`32d14100` x9, `32d10df0` x7, `32d0f300` x2, 11 singles) | split across CH2/CH3: each skill phase tags its own sites; CH0 assigns the 29 by function |
| 7e | 370 | "lost under June's player" list includes Elsword | - | n/a (nothing to port) |
| 7h | 420 | Elsword 25 functions `32cf8230`..`32d06c50` (e.g. `32d01040`): `UPDOWN_CANCEL_AFTER` cancel branches in state EventProcess | `32d01040` + 24 | CH2/CH3 (inside each state's EventProcess; member `+0x8ca4` exists, C9h-4c) |
| 7h | 421 | `EFFECT_SET_LIST_*` variants; Elsword among scripts using `_FINAL` / `_FINAL_ULTIMATE` | `32024ff0` | n/a (engine done; arrives with script, CH1) |
| 7i | 442 | `SetEquippedSkillFuncBySkillSlotIndex` Elsword `32d223e0`: drop `31fd2af0` call, set state byte `+0x11` | `32d223e0` | CH1n (head, as Chung/Eve); Aisha mark present |
| 7j | 456 | Rune Slayer magic-reflect, `DamageReact 320462a0 @32046a55-32046aa0` + `32d0c820` | see Explanations | CH4 (21418 Magic Coating, hyper-class passive; with 7ao) |
| 7p | 549 | March `THROW_WOODEN_PIECE_FrameMove` `SetStopTime( 1.5f )`: no June call loads 1.5, drop it if June dropped it | `+0x100` call sites | CH2 (confirm and drop, with the state) |
| 7s | 577 | `X2DungeonManager.cpp:252-552` already reads ELSWORD | - | n/a |
| 7t | 620-623 | cut-in names: June `HQ_CutIn_Elsword_01`; March `s_SkillCutInSet` names `HQ_CutIn_Elsword_1.dds` (no `.gfx`) | `329cd890`+`32078b70` per class | CH5 (replace table with June `.gfx` names; `m_HyperCutIn` rows in `UnitTemplet.lua`) |
| 7u | 635 | March-only templets dropped: `X2GUElsword_SwordMan.cpp` `LORD_KNIGHT_SAND_STORM`, `EffectSet_ELSWORD_DOUBLE_SLASH_HYPER` | `c11a_march_only_templets.tsv` | CH3 (take June's caller, drop these names) |
| 7ae | 721 | default-state fallback: an unbound reform id plays "Elsword Mega Slash" | - | CH2/CH3 (bind each id) |
| 7ae | 714-716 | census: Elsword = 261 rows, 254 no_state, 7 has_state (`SI_ETC_WS_COMMON_LOVE` 900, one per class); 0 unnamed | `s3_reform_ids_no_state.tsv` | CH2 (first jobs 10/11/22: 29 each; base 1: 15) / CH3 (100/101/112: 53 each) |
| 7ad | 750 | `r12b_anim_loss.tsv`: "Elsword, Arme and Eve: none" lost; stage pattern includes `elsword` | - | CH0b (stage `^motion_elsword`), no renames to chase |
| 7ah | 827 | `Static_EL_SKILL_B(_B)` / `_R(_B)` Elsword way of sword; March `CX2Game::UpdateElswordSkillSlotEffect` (`X2Game.cpp:13535`) drives them over 10 slots | driver `322cd5c0`-`322e07e0` | CH5 (check it still matches June; MODS L4147 lists it among the 4+4 loops widened) |
| 7am | 901 | delegate list `32d1c560`: A `32d0c4e0`, A `32d1b9d0`, C `32d08ba0`, A `32d0baf0`, A `32d0bc60`, D `32d0c180`, A `32d05af0`, A `32d1a370`; 916: March pushes replaced | those 8 | CH4 (each callback whole; `32d0c180` = transcend SetEquippedSkillFunc, `32d1b9d0` = skill 0x53ac) |
| 7an | 930 | `AttackResult` Way-of-Sword MP factor (`32012b50 @32012d98`, `@32012ee3`): `1 - SpecializationB` in Vigor/Destruction else `1 + SpecializationA` | `32012b50` | CH1q (class gauge / WOS) or CH4 (engine block with the class) |
| 7ao | 957 | transcend: `32d17e90` hyper effect start (vslot 19); `32d0c180`; `32d0c820` (21418); `32d1b9d0` (0x53ac == 1); `32d1c560` (vslot 420, three == 1); engine `32063650` (21416 Fight God), `32610cb0` (21417 Open Mind) | listed | CH3 (hyper effect start branch, hyper-class skills) / CH4 (passive branches, engine blocks); Z waits on june-transcend rows |
| 7ao | 940-945 | "Every class also brings": emulator allow-list id, LevelUpEvent TYPE row + `DLG_Transcendental_Change_Guide_Elsword`, `JuneGetHyperModeTableName` cases (class 100.., Elsword 100/101/112), `HQ_Cutin_*_Transcendence`, hyper-class skills + `TRANSCENDENTAL_SKILL_DATA`, four helpers `31fcac90`/`31fcad70`/`31fceab0`/`31ff1e80` | - | CH1m (GetHyperModeTableName), CH3 (skills), CH5 (cut-in, guide, pictures) |

Also in Part A by evidence, not by name: 7ag (L802-821) and 7ai (L837) apply to Elsword's own code, see Explanations.

## Part B - rows that apply to every class (current L numbers)

Pure every-class: 2 (L109; APPLIED_SKILL_ID L116), 3 (L144), 4 (L158), 5 (L182), 6 (L199, mark rule L211), 7k (L458), 7l (L464; bounding states in all 12, L470), 7o (L499; `<prefix>DAMAGE_BOUNDING_DOWN` -4000, L507; Elsword's LANDCRASH names `EffectSet_Bounding_Test`, JUNE_BUGS 140), 7p (L534; `TIME_STOP_IMMUNE_STATE` L546), 7x (L665; AT_COMMAND tags), 7ai (L837), 7av (L1029; Elsword vigor rate is in the base `ResetAnimSpeed`, no Elsword override in June's list).
Mixed with every-class parts: 7 intro table L216 (rows L241-246, 254, 256, 261, 272, 278, 281, 284), 7a (L294), 7h (L426 `USI_WAIT_STATE_CHANGE`), 7i (L428), 7t (L586), 7ae (L710), 7ag (L802), 7ad (L744), 7al (L882-889 new CX2GUUser hooks), 7am (L891), 7ao (L933).
Not Elsword: 7aq (Raven/Eve), 7ar, 7as, 7at (Ara), 7au (Ain), 7q, 7r (Chung), 7v-7ab (Lu/Ciel/Rose), 7n (writers Rena/Eve/Chung/Elesis only), 7m, 7d, 7f, 7g, 7e Elsword-only line aside.

## Marks (current lines)
- `(Aisha: CHn)`: L100-104, 214, 240, 260, 261, 276, 284, 328, 329, 426, 442, 470, 507, 546, 593, 623, 637, 735, 777, 780, 808, 843, 853, 866, 870, 879, 926, 927.
- `(Eve: CHn)`: L105-107, 162, 240, 252, 260, 261, 263, 276, 278, 281, 285, 288, 291, 302, 303, 314, 327, 328, 340-345, 351, 421, 425, 438, 492, 507, 546, 590, 593, 616, 623, 672, 729, 735, 816, 831, 844-848, 862, 915, 929.
- `(Chung: CHn)`: L53, 116, 117, 162, 178, 236, 247, 253, 256, 260, 261, 263, 278, 279, 281, 288, 327, 328, 421, 424-426, 437, 438, 470, 493, 507, 546, 571, 572, 590, 600, 616, 638, 647, 672, 714, 730, 757, 817, 849, 905, 928, 929, 942, 977, 1034.
- Elsword rows already carrying other marks: L256 (Chung), 260 (Aisha, Eve, Chung), 281 (Eve, Chung), 291 (Eve), 328 (Aisha, Eve, Chung), 442 (Aisha). Elsword rows with no mark: L51, 124, 125, 306, 352, 420, 456, 549, 635, 827, 901, 930, 957, 262.
- Elsword marks: none yet. Rule L211: `(Elsword: CHn)`.

## Explanations

### Section 2 and DAMAGE_SKILL_ID x103 (L140; Aisha-doc L2571 and stage7-plan L27 repeat it as "a large table job")
L140 was written 2026-09-29: "`Elsword_DamageData.lua` ... InsertDamageData scope, census `unassigned`: March has no reader". That is stale. Effect completion C9 (2026-10-07, flag `SERV_IRUHADEV_JUNE_DAMAGE_DATA_TABLE`, `Always.h:5393`) ported it: `CX2DamageManager::InsertDamageData_LUA` (`X2DamageManager.cpp:3836`), `OpenDamageDataScript( L"Elsword_DamageData.lua" )` in the ctor (`:181-183`), lookup `ApplyDamageDataRecord`, Lua bind `X2Main.cpp:3764`; STATUS_ARCHIVE L225: play-test passed, `rows=107`. The file is already in data036.
What the 103 are: `june_src_fixed/data036_kom/Elsword_DamageData.src.lua` has 107 entries, 103 with `DAMAGE_SKILL_ID`, 46 distinct ids, all 31011-31607. By June `Enum.src.lua` these are all Rose `RNSI_*_RHB/RSR/RVK/RFY/RMH/ROM` skills (e.g. 31207 `RNSI_A_RHB_CANNON_BALL`). Elsword's reform ids are 21xxx. The file name is a studio legacy. Consumers are `DAMAGE_DATA_KEY_NAME` in `DamageEffect.lua` (2) and `DamageEffect_INT.lua` (1); `Elsword_SwordMan.src.lua` has zero. So Elsword owes no table job: suggested CH0 note "n/a, Rose's data, loader done". The real Elsword damage gap is L51: 20 numerical-only tables (`Elsword_SwordMan.src.lua` has 20 `NUMERICAL_DATA_INDEX`, 25 `APPLIED_SKILL_ID`), done per skill in CH2/CH3.

### 7j Rune Slayer reflect (L444-456)
June's `DamageReact` (`320462a0 @32046a55-32046aa0`) replaces part of March's NEW_SKILL_2010_11 magic-reflect roll (`X2GUUser.cpp` ~9387-9435, divergent.tsv L220 `out`). Gate: `m_pUnit +0x38 == 0x65` (UC_ELSWORD_RUNE_SLAYER 101). If DD `+0x447 bCanReflexMagic` and `32d0c820()` is true: DD `+0x3c` (magic damage) = 0, no roll; else March's roll. `32d0c820` is true when buff test `3227baf0( 0x12 )`, skill 0x53aa = 21418 `RNSI_P_ERS_MAGIC_COATING` learned with templet `+0x174 == 5` (hyper class), and `m_pUnit +0x3c` (transcended) != 0; it then adds MP by skill value 0x2b (vslot 0xf0) and creates an effect (`322d38f0`). Same function is the 7ao row (funcmap L4612: engine reader of 21418). Work: one CH4 phase, Elsword class function `32d0c820` whole plus the engine branch in `X2GUUser.cpp` DamageReact, under a flag; waits on the transcend flag (june-transcend done).

### S3d memo consumers
S3d = June skill system phase (done 2026-10-18; flags `JUNE_SKILL_MEMO_FX/_DAMAGE/_BUFF/_MP/_STATE`, JUNE_BUGS 289-293). Engine side only: skill-memo switch cases 2/0xc/0xf/0x10/0x16, slot 476, MP alternate templet. Its play-test waited for S7, and STATUS_ARCHIVE L715 (S7c) says "the S3d consumer lines not shown: Elsword's skill state does not start until his class port". Backlog L285: memo consumers "show their numbers only once a class port gives a skill a state". Class-side readers of `31fdbbb0`/`31fdbcc0` are in checklist L326-327: `320284c0 @3202858e` (skills 31301/31321, Rose) and `3230c1a0 @3230c742` (memo 22027): not Elsword's. So for Elsword it means: when CH2/CH3 binds his skill states, the first memo-equipped skill must show the S3d lines (memo effect-set/damage/buff/MP) in the play-test; no Elsword-specific code is listed. Treat as a CH6 walk item, not a phase.

### The requested rows
- 7ag pressed-index: yes, Elsword has its own March 4+4 decode. `SpecialAttackEventProcess` (`X2GUElsword_SwordMan.cpp:4965`) calls `CommonSpecialAttackEventProcess` (L4982), `CountUsedSkill( iPressedSkillSlotIndex )` (L5152), and decodes `iPressedSkillSlotIndex > 3` / `- 4` into `aNormalSkillStateIDSlotB` (L5232-5234) and `aHyperSkillStateIDSlotB` (L5243-5245); also `GetSkillSlotIndexUsed` at L12896. `X2GUUser.cpp:26532` stand-in lists Elsword among unported classes (`bMarchSlotIndex`). Port: CH1p, convert to June's A 0-4 / B 5-9 (slot + 1 for key-pressed) and join `UT_ELSWORD` to the June set (as `UT_CHUNG` under `_LIVE`); `CurrentSkillKeyManager` users move to `m_kSkillSlotKey`.
- 7c: L352, 29 sites (above).
- 7ad: no Elsword renames (r12b "none"); only stage the motion files (CH0b).
- 7ae unnamed ids: none (0 of 261 lack a June `RNSI_*` name; the 7 non-RNSI rows are id 900). No Chung-style "unnamed upgrade ids" task.
- 7r cinematic: Chung only, n/a. 7n sync float: Elsword not a writer/reader, n/a. 7t berserk-by-chance: Chung only, n/a (Elsword's 7t rows are only cut-in names, L620). 7am: L901. 7an soul gain (vslot 0x748): Ain/Chung/Raven, not Elsword; Elsword appears at L930 only.
- 7ao hyper effect start `32d17e90` (vslot 19): CH3. 7b: 21419/21420 at `32d1e0e0` above. Death prevention/Crest of Time: L256, Elsword override `32d058a0`; Chung CH4b wrote its own overrides and routed the cast to char-add (stage7-plan L35, `STATUS`), so Elsword writes `32d058a0` in CH1q and nothing else.

## Routed to Elsword from other docs (file:line)
- kickoffs/june-backlog/stage7-plan.md:5 and :27 (order #3; "plan its data phase first", stale per §2 above).
- JUNE_CHARACTER_AISHA_PHASES.md:2571 (Known extras: DAMAGE_SKILL_ID x103, Rune Slayer reflect 7j, S3d memo consumers); :2065 (other classes' delegate registrations go to 7am).
- STATUS.md:116 (stage7-plan queued: next `/x2-phase new-plan char-elsword`), :181 (char-chung Z points stage7-plan -> char-elsword).
- JUNE_CHARACTER_CHUNG_PHASES.md:3468 (Z note) ; kickoffs/char-chung/Z.md:24.
- JUNE_CHARACTER_LUCIEL_PHASES.md:23 and :492 (Luciel code phases start after char-chung Z, ahead of char-elsword; Chung CH2-CH4 and NU1-DU5 both edit X2GUUser.*/X2Game.cpp).
- JUNE_PORT_BACKLOG_ORDER.md:137 (Elsword animation sets 235 -> 300, no skeleton work), :353 (gap scan Elsword 20), :285 (S3d memo consumers wait for a class state).
- STATUS_ARCHIVE.md:715 (S7c: Elsword skill state not started until his port); MODS.md:4147 (X2Game Elsword slot-icon update widened to 10 slots).
- tools/data/june_march_divergent.tsv:220 (Rune Slayer reflect `out`), :401 (ResetAnimSpeed, Elsword vigor base).
- Not found: no line says "Elsword's port owns it" in JUNE_PORT_BACKLOG_ORDER.md or STATUS.md beyond the above.

## JUNE_BUGS.md rows naming Elsword
- 140 (L157): Elsword's `ESSI_DAMAGE_BOUNDING_LANDCRASH` names `EffectSet_Bounding_Test`, which June's `EffectSet.lua` lacks, so it never plays; verbatim when the class tables land (C3d-4). Reach: yes.
- 74 (L91): spawner tail b47 branch copies IgnoreDefence but not the PvP flag; gate flags set by class code (Add, Eve, Rose, Elsword, Ain); latent, verbatim (C6b).
- 472 / 473 (L488-489): upgrade-weapon glow 4th sequence made from the 3rd name; an "Elsword-type" +11 weapon shows `Lightning03` twice (in-game and unit viewer). Engine rows, not port work.
- 486 (L502): unit viewer sword sparks trace-frame re-lookup; engine row.
