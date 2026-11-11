# June character port: Add (Stage 7, eighth and last March character; CH-N reused from Ara and Elesis)

Plan key `char-add` · branch `main` · kickoffs `kickoffs/char-add/` · log tag `[JUNE_CHAR]` (lines start `[JUNE_CHAR] ADD <sub>`, sub-tags
`DATA`, `CREATE`, `CORE`, `DP`, `NASOD`, `CLASS`, `CREST`, `SUMMON`, `SKILL`, `PASSIVE`, `UI`, `NUM`) · flags `SERV_IRUHADEV_JUNE_CHAR_ADD_*` for Add's
class code, `SERV_IRUHADEV_JUNE_CLASS_PATH` (Ara's CHNa) for the class-generic class-path fixes, `SERV_IRUHADEV_JUNE_USER_CREST_OF_TIME` for the
shared Crest of Time engine (+ a temporary `..._DEBUG` per sub-phase) · backlog Stage 7 (`JUNE_PORT_BACKLOG_ORDER.md`, brief
`kickoffs/june-backlog/stage7-plan.md`) · written 2026-11-11 by the `new-plan char-add` session from the Ara plan (`JUNE_CHARACTER_ARA_PHASES.md`, the
first CH-N run) and the Elesis plan (`JUNE_CHARACTER_ELESIS_PHASES.md`, the most recent worked example).

## Context

**What this plan does.** It moves Add (unit type `UT_ADD` 9, script `Add_NasodRuler.lua`, class `CX2GUAdd` in `X2Lib/X2GUAdd.cpp` / `.h`, seven
classes up to transcendence: Nasod Ruler / Tracer 9, Psychic Tracer 32, Lunatic Psyker 122, **Arc Tracer 33, Mastermind 123, Time Tracer 36, Diabolic
Esper 126**) from his March kit to his June 2017 kit, end to end: **his entry on the character-creation screen** (none today, user 2026-11-11), June
script, skill states for June's tree, numerical damage, motions, class-only engine hooks (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` §1-§7av), the DP /
formation-mode system and the nasod armor / barrier / Phase Shift mechanics (live March code, reworked by June), the shared Crest of Time engine
routed here by char-chung, his summons, UI pieces, data, his transcendence rows, and **his two June-only class paths (CH-N)**.

**Corrections to the brief.** The stage7 brief and the Aisha template call Add's March kit a "skeleton" with "gap 0". It is not: `X2GUAdd.cpp` is
11,319 lines with about 130 state registrations and 54 `case CX2SkillTree::SI_` arms, Psychic Tracer and Lunatic Psyker included
(`SERV_ADD_LUNATIC_PSYKER`), and the save already holds two Add units. "Gap 0" means `charport_scan.py` prints no NUMERIC line for
`Add_NasodRuler` (June's script carries its numbers in `DAMAGE_DATA` blocks the scan does not flag), not that nothing is missing: June's
`DamageEffect*.lua` has 140 `c1a` numerical rows for 55 of his skill ids, none in March's templets. What the brief got right: no stat rows for
9 / 32 / 122, 4 new motion files, `CUSTOM_RENDER_STATE` (x7), and the routed Crest of Time engine.

**Why Add has no creation-screen entry (the user's point, 2026-11-11).** `SERV_LIMIT_UNIT` is defined for the global build
(`KncWX2Server/Common/OnlyGlobal/ServerDefine/ServerDefine_Global.h:1184`) with `_CONST_LIMIT_UNIT_::iLimitType = 8` for US (`:1190`), i.e. up to
`UT_ELESIS`: `X2StateCreateUnit.cpp:132-135` hides `AddCreateUnitSlot` because `8 < UT_ADD`. Everything else is there and unreachable: `SelectAdd()`
(`X2StateCreateUnit.cpp:2346`, under `SERV_9TH_NEW_CHARACTER`), the entry switch's `UT_ADD` arm (`:207-209`), the button slot in March's dialog, and
the emulator's live-DB beginner gear row for class 9 (`X2Lib/Offline/X2OfflineInventory.cpp:1236`, commented "unreachable"). June's create-unit
state has no per-type limit (char-luciel NU4 amendment: June ctor `3255bd10` -> `32556f60`); June's `SelectAdd` is `32555320`. Since `UT_ADD` 9 ==
`UC_ADD_NASOD_RULER` 9, March's create-request trap (unit type sent as the class, `X2StateCreateUnit.cpp:2505`) does not bite Add. Phase **CH0c**.

**Why Add is last (stage7-plan, 2026-10-22).** Four June-only classes on two paths, no stat rows at all, 4 new motion files, the biggest class: last,
with every template piece proven. CH-N reuses Ara's class-generic `SERV_IRUHADEV_JUNE_CLASS_PATH` and Elesis's run of it.

**His size.** June's class spans `0x329cf870`-`0x32a316dd` (Eve ends at `329cf86f`, char-eve plan; the last override `32a316c0` is the vtable's
slot 0; after it come small helpers `32a31740`-`32a317d0` and the gauge-data helpers from `32a31900`, which CH0 assigns): **733** carved functions,
vtable `334daaec` (544 slots), **98 overrides, all in range, 58 of them not carved** (listing-only; Ara 77 / 32, Elesis 66). June's script has **328**
tables: **322 `ANSI_`** + 6 `INIT_*`. Expect more than Ara's size (Ara 540 + 32 planned at 18 CH1 sub-phases); the phase table names **21 CH1
sub-phases** up front.

**Written June-body-inside-March from the start (CLAUDE.md, 2026-10-28).** A June body with a March function of the same name goes inside it under
`_LIVE`, March's in `#else`, proven by a one-file probe compile that defines `_LIVE`; a June function March's class lacks is written under `_CORE`
with a plain name in the class's pattern.

**Scope: June 2017 up to transcendence (user, 2026-10-20).** Third jobs (`UC_150`-`UC_186`) are out. Transcendence is live (Stage 6): his
transcendence rows (122 / 123 / 126) are in CH3d and CH6.

**Order.** char-ara is at CH2a and char-elesis is planned (CH0 ready), both 2026-11-11. **CH0 runs now** (read-only, alongside anything); **CH0c
after CH0 and char-ara's CHNa review** (the creation state is shared; Ara's and Elesis's creation-screen edits come first, one at a time, and CH0c
plays on March's kit, so it does not wait for Add's class code); **CH0b after char-elesis CH6's walk** (it packs and copies koms the walk uses); **every
class-code phase after char-elesis Z** (one character's code at a time: `Always.h`, `X2GUUser.*` are shared). Lu/Ciel stays parked until Stage 7
closes (user, 2026-11-03): Add's Z closes Stage 7.

**The state he starts from.**
- **Class code (March, live).** `X2GUAdd.cpp` 11,319 lines (all CRLF), `X2GUAdd.h` 1,380 (1,376 CRLF, 3 LF), CP949. The class sits inside
  `#ifdef SERV_9TH_NEW_CHARACTER` (`.cpp:7-11319`, `.h:3-1380`; defined at `ServerDefine.h:4323`), Lunatic Psyker inside `SERV_ADD_LUNATIC_PSYKER`
  (`:4368`); CH0 confirms both live by `cl /P`. Enums: `ADD_NASODRULER_STATE_ID` (`.h:369`), a March `CUSTOM_RENDER_STATE` (`.h:485`, kept separate:
  `kickoffs/june-engine/R5.md:20`), `POWER_EXCHANGER_STATE` (`.h:495`). Functions: ctor `:114`, dtor `:188`, Init `:220`, InitState `:303`,
  InitComponent `:362`, InitPassiveSkillState `:385`, `SetEquippedSkillFuncBySkillSlotIndex` `:612`, CommonFrameMove `:1258`,
  SpecialAttackEventProcess `:1360`, InitStateCommon `:1549`, InitStateByUnitClass `:2314`, SetSkillLevelStateData `:2458`, CanUseSpecialAttack
  `:2493`, ReInit `:2754`, DamageReact `:2810`, HyperModeFrameMove `:3057`, `UpNowDPValue` `:9100`, ApplyHyperModeBuff `:9532`,
  SetNasodArmorAttackTarget `:9863`, DieDamageEffectProc `:10412`, two `CreateNPCReq` sites `:6103` / `:6120`; **no `ReservedSkillStart`**. 19
  `SERV_IRUHADEV_*` lines from earlier engine plans (`JUNE_DAMAGE_MATH`, `JUNE_SOCKET_PARITY`, `JUNE_SKILL_STATE_PTR`, `JUNE_RANDOM_MISSION`,
  `JUNE_DIVERGE_RIDING_ANIM_NAME`, `JUNE_BUFF_NEW_STAT`, `JUNE_FX_DEBUG`, the buff port's R1 DP edit).
- **DP and formation mode (live March code, June reworked; "the user ports Add from June", user 2026-10-04, `JUNE_BUFF_PORT_ARCHIVE.md:1711-1719`).**
  June: `UpNowDPValue` vslot 0x2a4 `329e5b00` (body `329e56c0`, `329e5140`, `329e5180`), every spend x `1 - result(+0x6bb0)`, formation entry
  `32a08580` with the Fantasy Tracer refund from DP actually spent, formation cost without March's decrease term; `ChangeMaxCharacterAddResource`
  `329e31e0`; `ApplyHyperModeBuff` `329e2410` (BFI 846 when transcended, else 315; skills 29401 / 29406 / 29411; checklist l.285 "port it whole"); the
  hyper hooks `329e2340` / `329e2360` (with `UAI_RELEASE_FORMATION_MODE`), `CanHyperMode` `32a31550`, the hyper StateStart `32a08150` (`+0x9330`
  countdown, `31fc8c20`); spec A / B readers `329e59f0` / `32a01ca2`; `SAT_ADD_DP_PERCENT_UP` (item 100001054); `PartyFeverAllUser`'s DP step
  (divergent row 113); gauge `CX2AddGageData` (RTTI `33734c08`; June has no `CX2AddMyGageUI` RTTI: CH0 says what draws his gauge). Phase **CH1q**.
- **Nasod armor, barrier and Phase Shift (live March code, June rewrote).** `SetNasodArmorAttackTarget` `329f63a0` whole (four reform ids
  29201 / 29202 / 29216 / 29217; user 2026-10-07: follow June), `POSSIBLE_NASOD_BARRIER` (38 script lines) with user slots 0x4c4 `329e7d60` / 0x4c8
  `32a047a0`, `DieDamageEffectProc` `329f73a0` / `329f7370` (both Phase Shift ids), `PlayLoopSound` `329e2660` (JUNE_BUGS 125, verbatim), the
  packet-buff refusal `329d4640` (BTI 342 / 438). Phase **CH1q2**.
- **Crest of Time and death prevention (routed whole to this port by char-chung CH4b, 2026-11-03).** Add's `ANSI_SA_COMMON_CREST_OF_TIME` is June's
  only cast of BTI 436 / 926 (skills 207, 29212, 29230). The engine is `CX2GUUser`'s: slot-250 pre-apply hook `3201ffb0`, snapshot / restore bases
  `32009780` / `3204da60` (vslots 0x740 / 0x744, members `+0x92a4..+0x92c4`), the death-prevention gate `320072d0` in `UpNowHp` / `SetNowHp` (also
  27409 / 23402) and trigger `3202b9a0`; nine classes override 0x740 / 0x744 in June (Add `329e2f80` / `329e3000`, Ain, Rose, Lu/Ciel, Elesis
  `32b6ac80` / `32b6acb0`, Ara `32ba4840` / `32ba7b50`, Chung `32bed760` / `32bf4600`, Rena `32c852f0` / `32c89230`, Elsword `32d058a0` / `32d058d0`;
  Eve, Raven, Aisha keep the base). Chung's, Elsword's, Rena's and Ara's overrides are written with the base call left out as a stand-in. Phase
  **CH2a0** (Opus, shared code, own review), before the skill phase that casts it.
- **Summons.** Six June `CreateNPC` call sites in his range (`329f350a`, `329feafb`, `32a05ace`, `329f5430`, `32a0c997`, `32a0ca9d`; two push NPC
  `0x1444` 5188; `329f5430` creates Apocalypse 1604), against March's two `CreateNPCReq` sites. Apocalypse's NPC script needs `GetEffectSetInstance`
  (June `321d97c0`) and `SetLocalRotateDegree` (`31e4c520`) bound (`lua_unbound_known.tsv` rows owned by "add port"); the dying Apocalypse
  `329fb6c0` clears Mastermind's `+0x98e4` (char-eve CH2b4 ported the rest). Phase **CH2b0** (Opus) before the first skill phase that summons.
- **vslot 0x524 (`GetNowStateSkillID`).** Add alone overrides it (`32a315a0`, reads `+0x7cbc` / `+0x7cec`); his four calls of `329cc340`
  (`32a10fe9`, `32a266ce`, `32a26e8f`, `32a26f17`) need his own now-state id (funcmap row `329cc340 (CH2b0)`). Phase **CH1o**.
- **Training-game keys** (backlog Stage 2 block, `JUNE_PORT_BACKLOG_ORDER.md:235-241`): his F6 orb value (vslot 0x6c4) and F7 gauge step (vslot
  0x6f4). The backlog names `329e2170` / `329e20e0`; his vtable holds `329e21d0` / `329e2130` at those slots: CH0 settles it. The overrides are his
  (CH1i); the shared key block stays Stage 2's.
- **Skill system (Stage 3).** June's tree: 261 rows over the seven classes (9: 15; 32 / 33 / 36: 29 each; 122 / 123 / 126: 53 each), **123** distinct
  ids (106 .. 29605; `RNSI_SA` 143 rows, `RNSI_A` 75, `RNSI_P` 30, `SI_ETC` 7, `RNSI_HA` 6). The offline auto-learn and the S3 census need rows for
  33 / 36 / 123 / 126 (CH0 item 11).
- **Motions.** June data207 (+ data033 / data148 / data165 / data225): **4 June-only job files** `Motion_ADD_AT.x` (3.4 MB), `_MM.x` (2.9 MB),
  `_TT.x` (4.3 MB), `_DE.x` (3.7 MB); 7 same-name files larger in June (`Motion_ADD.x` 5.1 -> 12.1 MB, `_Common` 1.4 -> 7.4, `_Emotion`, `_Field`,
  `_Lobby`, `_LP`, `_PT`); `Motion_Standard_Add.X`, about 50 emotion motions, `Motion_Npc_Add_Arctracer.x` (data148), `Motion_Tutorial_Add_object.x`
  June-only; the `CENTER_*` / `Time_*` / `Npc_ADD` / tutorial-stage files identical. March's game dir has no `data207.kom`: CH0 names the target kom.
  Whether June drops a name March plays decides CH0b vs CH1 final (7ad; checklist l.760 names `APT_NA_Ready` / `APT_NA_preAttack`).
- **June-only class paths (CH-N).** `X2Unit.h:162-180` already names 33 / 36 / 123 / 126 under `SERV_IRUHADEV_JUNE_ENUM_NAMES`. `UC_VALIDE_END` = 123
  and `UC_END` = 124, so **Mastermind (123) fails every `< UC_VALIDE_END` test and Diabolic Esper (126) every `< UC_END` test**; 33 / 36 pass.
  June's route (`FieldQuest.src.lua`): Arc Tracer 35540-35543 (`CJT_JOB_SECOND`, from 9 at level 15), Mastermind 35550-35554 (from 33 at 35), Time
  Tracer 35580-35583 (`CJT_JOB_THIRD`, from 9 at 15), Diabolic Esper 35590-35594 (from 36 at 35); March's own paths are 35470-35473 (32) and
  35480-35484 (122). `GetX2UnitClassCompatibility` (checklist l.1009) needs his four cases.
- **Stat rows.** `KncWX2Server/ServerResource/US/StatTable.lua` has **no rows for any Add class** (classes 1-8, 10-31, 100-119 only;
  `june2017_re/char_luciel/plan_march_side.md:136-140`): Add plays on the loader's fallback today. June's `CharacterStatTable` relative rows:
  `UT_ADD` `380, 88, 480, 103, 9500`; Lunatic Psyker `400, 92, 530, 115, 11000`; Mastermind `440, 95, 510, 110, 10750`; Diabolic Esper `390, 100, 530,
  115, 10500` (`CharacterStatTable.src.lua:159-409`). Decided: Open decision 1.
- **Saves.** Units 36 `LunaticPsykerT` (122 lv70) and 37 `AddTest` (32 lv40); no unit of 9, 33, 36, 123, 126. `createchar.py` knows 9 / 32 / 122.
- **Hooks waiting on the class port.** 56 checklist rows name Add and 31 class-generic rows apply with no Add mark (`plan_checklist_rows.md`); none
  carries an `(Add: CHn)` mark yet. The l.358 Class column says Rena for `RNSI_SA_ALP_SUPERSONIC` 29058, an Add skill: CH0 corrects it.

**Out of scope (owned elsewhere).** Third jobs; new characters (Stage 8); UI that is not Add's own (Stage 9; his creation-screen entry **is** his,
user 2026-11-10 / 2026-11-11); tutorials and training dungeons (june-dungeons); the shared training-game key block (Stage 2); NPC / boss copies of
Add; emotion motions' behaviour (Stage 10; CH0b stages the files); `FORCE_DYNAMIC_CAMERA_INDEX` x15 / `RETAIN_CAMERA_STATE` (Stage 9);
`CUSTOM_RENDER_STATE`'s reader (june-engine R5; his rows get the labelled stand-in, 7w); PvP-only paths.

## Rules

The Elesis plan's Rules hold (`python tools/docsection.py JUNE_CHARACTER_ELESIS_PHASES.md "Rules"`), with Add's names and these changes:
- **Standing rules** (`JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules"): scope rule, no partial June updates, June wins, June bugs verbatim with a
  `JUNE_BUGS.md` row, cite June addresses, dev free-skill gate (`CX2GUUser::IsDevUnlimitedSkillUse()`) in his skill-use check.
- **`ReservedSkillStart` (user, 2026-11-05).** March's `CX2GUAdd` has none. CH1k writes `CX2GUAdd::ReservedSkillStart` = `SpecialAttackCooldown(
  skillID, m_fReservedSkillMp )` then `SkillMemoMpRefund( ... )`, widening the base hook chain with his `_LIVE` flag; **his DP-cost skills spend DP
  there, not MP** (June's skill-use `32a07dc0` and `GetActualDPConsume` `329e7820`, the only `false` caller of the June MP-consume base): CH1k reads
  both before writing it. CH1 final's play-test casts each skill twice.
- **Slots by role, never by another class's offset.** Every override's role is confirmed from its body (CH0 item 1); 58 of his 98 are listing-only.
- **Listing-only functions.** Ghidra stays read-only: `charport_adis.py add <va>` (CH0 adds `add` to `CLASS_RANGE` as `(0x329cf870, 0x32a316e0)`
  or the range CH0 settles). A Sonnet phase whose function has no decompile and a control flow the listing does not make plain says "this needs Opus"
  for that function and goes on with the rest.
- **June's ids in passive / skill code.** Every `SI_*` id March's code reads is checked against June's `RNSI_*` id (March `SI_A_AN_*` etc., June 29000s).
  CH0 lists them; CH4a fixes what CH1 did not.
- **The checklist is the work list.** Every Add row in §1-§7av is ported by a phase here or marked n/a by CH0 with the reason; the porting phase
  appends `(Add: CHn)` in place.
- **One deploy unit.** June's `Add_NasodRuler.lua`, his June motions (held for CH1 if CH0 finds a dropped name) and the C++ that reads them go live
  together in CH1 final, behind `SERV_IRUHADEV_JUNE_CHAR_ADD_LIVE` (`//#define` until then). Between phases Add stays playable on his March kit
  (and creatable from CH0c on). Classes 33 / 36 / 123 / 126 are playable only from CHNa on.
- **Enum gate (user, 2026-11-11).** Before his class script is packed, `python tools/luabind.py check Add_NasodRuler` passes its ENUM and STATE gates
  (Ara's script died at load on a missing `CUSTOM_RENDER_STATE` member and drifted `USER_STATE_ID` names): CH0 lists the gaps, CH1 final fills them.
- **Flags:** `_CREATE` (CH0c, the creation-screen entry and the emulator's create path for his classes), `_CORE`, `_LIVE`, `_DP` (CH1q; only with
  `_LIVE`), `_NASOD` (CH1q2; only with `_LIVE`), `_CLASS` (his 33 / 36 / 123 / 126 rows, CHNa / CHNb), `_SUMMON` (CH2b0), `_SKILL`, `_TRANSCEND`,
  `_PASSIVE`, `_UI`, `_ASSET` (only if a June asset breaks a March reader), a temporary `_DEBUG` per sub-phase retired in its review; the class-generic
  `SERV_IRUHADEV_JUNE_CLASS_PATH` (widen, never copy) and `SERV_IRUHADEV_JUNE_USER_CREST_OF_TIME` (CH2a0, `CX2GUUser` and every class's 0x740 / 0x744
  base call). New namespaces are `_CONST_ADD_<phase>_` (CH0 lists March's existing `_CONST_*` names in `X2GUAdd.cpp` first).
- **Names (CLAUDE.md, 2026-10-28)**, **phase tags** (`// char-add CH1n`, 2026-11-09), `junediverge.py --list` before transcribing and `--check
  X2Lib/X2GUAdd.cpp X2Lib/X2GUAdd.h <other files>` = `RESULT: PASS` at every finish. Divergent rows that touch him: 113 `PartyFeverAllUser`, 121
  `SetMultiplyPowerRate_LUA`, 371 `RidingHyperModeEnd`, 401 `ResetAnimSpeed` (+ allow row 17).
- **Shared `CX2GUUser` helpers, never copied into the class** (skill-level and skill-use helpers, `CREATE_EFFECT_SET`, `LandingPrediction` hoisted by
  Ara CH1g, `IsNowStateSkillUpgraded` by Chung CH2b0, the earlier characters' bases): a phase that needs one widens its `#if`.
- **No hardcoded enum literals** (CLAUDE.md, 2026-10-26): every June id by name; `enumscan.py --check X2Lib/X2GUAdd.cpp` stays 0. March's
  `ADD_NASODRULER_STATE_ID` takes June's missing names with June's ids (resolved from the exe's state-name strings where June's `Enum.lua` has none).
- **Shared Lua per character.** `UnitTemplet.lua` is core-list source: CH5 replaces his 3 rows and adds June's 33 / 36 / 123 / 126 rows. His class
  script is adopted whole (`adopt-june`, bytecode unless a phase must edit it) and passes `luabind.py check Add_NasodRuler`; so do his summon scripts.
- **Assets first, whole** (CH0b), byte-exact through `kom.py stage-june`; pack and copy yourself (CLAUDE.md, 2026-11-10). CH0c stages only the
  creation-screen files it needs; CH0b stages the rest.
- **CP949.** `X2GUAdd.*` (CRLF), `X2GUUser.*`, `X2Gage*.*`, `X2Unit.h`, `X2StateCreateUnit.*`, `X2DamageManager.*` are CP949: `bytepatch.py` only;
  `cl /P` decides every `#ifdef` branch. Never edit `ServerDefine_Global.h`: the creation limit is lifted at the client's use sites under `_CREATE`.
- **Damage numbers.** Record March vs June damage per changed skill in `MODS.md`.
- **Saves.** Units 36 (122 lv70), 37 (32 lv40), transcendence 0. CH0c creates a 9 through the creation screen itself (the play-test); CH0b adds a
  level-99 122 and a level-40 9 with `createchar.py`; CHNa creates 33 / 36 / 123 / 126. Play-test saves slot every tested skill in slots 0-3 / 5-8,
  never 4 / 9 unless the check is about transcendence (user, 2026-11-11), and never ask the user to swap.
- **Offline reach.** Each hand-off names only checks the offline client can reach; a June path only a server or PvP drives is a stand-in or a routed
  item, decided in the phase.
- **One code phase at a time in `X2GUAdd.*`** (CH1-CH4). CH0c, CH5 and CHNa / CHNb may run beside CH2-CH4 when their Touches do not overlap.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_CHARACTER_ADD_PHASES.md "<heading>"` (`--no-sub` to skip Done notes) |
| Checklist rows | `python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "<heading>"`; Add's rows with line numbers: `june2017_re/char_add/plan_checklist_rows.md` |
| Routed items | `june2017_re/char_add/plan_routed_items.md` (what other plans handed to Add, with sources) |
| Planning evidence | `june2017_re/char_add/plan_*` (below); CH0 writes `ch0_*` beside them |
| Character tools | `june2017_re/charport_states.py`, `charport_keys.py`, `charport_closure.py`, `charport_adis.py` (CH0 adds `add`), `charport_regsim.py`, `s3_no_state.py`, `cap99_gen.py`; Elesis's / Ara's builders `june2017_re/char_elesis/ch0_*.py`, `char_ara/ch0_*.py`; `char_arme/ch0c_listing_check.py`; `char_eve/ch0_build_assets.py` (CH0 adds `add`) |
| June Lua (reading) | `june_src_fixed/data036_kom/Add_NasodRuler.src.lua`, `UnitTemplet.src.lua`, `Enum.src.lua`, `CharacterStatTable.src.lua`, `FieldQuest.src.lua`, `DLG_Unit_Create_Unit_Button_New.src.lua` |
| Ghidra | `ghidra` skill; range `0x329cf870-0x32a316dd`, vtable `334daaec` (544 slots, 98 overrides, 58 listing-only), ctors `32a22170` / `32a23b90`, June `SelectAdd` `32555320`; read-only in CH0 and reviews |
| Gap scan | `cd june2017_re && python census.py && python charport_scan.py <short scratch dir>` (revert the regenerated `census.tsv` / `JUNE2017_EFFECT_KEY_MATRIX.md` after, only if they were clean before), `python charport_keys.py Add_NasodRuler` |
| Bring June Lua in | `/x2-adopt-june` (`luasrc.py adopt-june data036 Add_NasodRuler.lua`, `--dry-run` first) |
| Stage June assets | `python tools/kom.py stage-june <list>`, `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `junediverge.py --check ...`, `enumscan.py --check ...`, `luabind.py check ...`, `build.py --deploy` (background), `logs.py --grep "JUNE_CHAR ADD"` |
| Save / test character | `/x2-db` (`tools/elsdb.py setchar`, `settranscend <unit> 1`), `/x2-create-char <name> <class> <level>` |
| Dev console | auth level 3, END key; items `/additem <id> [qty]`, quests `/qdone <id>`, `/tc 1` |

## Planning evidence (2026-11-11)

Kept in `june2017_re/char_add/` (digests by Sonnet subagents and by the planning session from its Ghidra pulls; CH0 re-verifies anything it builds
on): `plan_charport_scan_20261111.txt`, `plan_charport_keys.txt` / `.tsv`, `plan_checklist_rows.md`, `plan_numerical_rows.tsv`,
`plan_routed_items.md`. The March-side, June-side and creation-screen digests were not finished (session restarts); their headline facts are in
Context above, each with its source, and CH0 items 1, 9, 10 and 13 rebuild them as `ch0_*` files.

| Measure | Add | (Ara) |
|---|---|---|
| June class range | `329cf870`-`32a316dd`, 733 carved; vtable `334daaec`, 544 slots, 98 overrides (all in range), **58 listing-only** | 540 + 32, 77 |
| June skill-tree rows | 261 over the 7 classes (9: 15; 32 / 33 / 36: 29; 122 / 123 / 126: 53), 123 distinct ids 106 .. 29605 | 197 / 98 |
| June script | `Add_NasodRuler.src.lua`: 322 `ANSI_` + 6 `INIT_*`; groups SA 67, A 30, RIDING 27, COMBO 25, DAMAGE 21, ATT 19, DASH 18, ALP 16, ADE 16, AAT 15, AMM 14, APT 9, JUMP 6, STANDUP 6, PEPPER 5, DANCE 3, HA 3, then small groups (DP 1, COMMON 1); job tags in SA / A / P / HA: ADE 20, ALP 19, APT 14, ATT 13, AN 12, AMM 12, AAT 9 | 331 |
| Key census (`charport_keys.py Add_NasodRuler`) | 222 keys: both 206; june-only **8** (`DP_RATE_BY_FORMATION_MODE_USE_MP_3RD_LINE_SKILL`, `DP_USE_RATE_WITH_FORMATION`, `VOID_IMPACT_UP_ANGLE`, `VOID_IMPACT_DOWN_ANGLE`: `INIT_COMPONENT`, reader `329cf870` range; the camera pair (Stage 9); `BBT_CHANGE_NOW_HP_PER_SECOND` / `BFT_TIME` x3 no reader); neither 4 (`CUSTOM_RENDER_STATE` x7, `DASHL_CANCEL_AFTER` x5, `HEAD_IK` x9, `SPEED_y` x1); march-only-site 4 | 250 / 14 |
| Gap scan | no NUMERIC line; ENUM `{'CUSTOM_RENDER_STATE': 7}` | 14 |
| Numerical blocks | 140 `c1a` rows, 55 ids 29000-29602 (110 `DamageEffect.lua`, 30 `DamageEffect_Character.lua`), none in March's templets | 160 / 59 |
| Motions | 4 June-only job files (AT / MM / TT / DE), 7 larger in June, ~50 June-only emotions, `Standard`, `Npc_Add_Arctracer`; March has no `data207` | 2 new |
| Summons | 6 June sites (5188 x2, Apocalypse 1604, 3 to name), March 2 | none |
| Class mechanics | DP + formation mode (CH1q), nasod armor / barrier / Phase Shift (CH1q2), Crest of Time engine (CH2a0) | FP / Specter |
| March class | `.cpp` 11,319 lines, `.h` 1,380; ~130 states, 54 skill cases | 17,140 |
| Stat rows | none for any Add class; June relative rows for `UT_ADD`, 122, 123, 126 | 1-99 for 5 |
| Creation screen | slot hidden by `SERV_LIMIT_UNIT` (US limit 8); `SelectAdd` `:2346` and class-9 beginner gear exist | shown |
| Save | units 36 (122 lv70), 37 (32 lv40) | 3 |

## Phases

| Phase | What | Model | Depends |
|---|---|---|---|
| CH0 | Add map: class map (every listing-only override included), state / skill table, checklist rows, assets + effect closure, keys + enum gate gaps, registration listing check, unmapped helpers, the DP / formation map, the nasod map, the Crest of Time map, the summon map, the CH-N map (33 / 36 / 123 / 126), the creation-screen map, the motion question | Opus high | - (runs now) |
| CH0c | **Add on the creation screen** (March classes 9 / 32 / 122): lift the US limit for `UT_ADD`, June's `SelectAdd` and class buttons, the create path, his stat rows 9 / 32 / 122 (decision 1) | Opus high, Opus review, user play-test | CH0, char-ara CHNa review |
| CH0b | Every June Add asset, whole, first | Sonnet high, user play-test | CH0, char-elesis CH6 walk |
| CH1a-CH1k2 | Class core transcription by state group and vslot band (14 sub-phases, below) | Sonnet high; Opus review every 2-4 phases | CH0, char-elesis Z; one at a time |
| CH1m-CH1p | Wiring behind `_LIVE` (registration; base callbacks + slot head; virtuals incl. 0x524; 7ag + hooks + June-only keys) | Opus high, Opus review each | the CH1a-CH1k2 reviews |
| CH1q | DP and formation mode: June's rework of the live March system, flag `_DP` | Opus high, Opus review | CH1p review |
| CH1q2 | Nasod armor, barrier and Phase Shift, flag `_NASOD` | Opus high, Opus review | CH1q review |
| CH1 | Final: define `_LIVE` (+ `_DP`, `_NASOD`), adopt the script, enum gate, copy any held motion koms, play-test on 9 / 32 / 122 | Opus high, Opus review, user play-test | CH1q2 review, CH0b |
| CH5 | UI and data: his 3 `UnitTemplet` rows June's + the 33 / 36 / 123 / 126 rows, cut-in table, tree-UI icon blocks (7aj / 7ak) | Sonnet high, Opus review, user play-test | CH1 review |
| CHNa | CH-N (a): 33 / 36 / 123 / 126 load and play: `UC_VALIDE_END` / `UC_END` tests for 123 / 126, his class arms, stat rows, units, the creation screen's June-only entries | Opus high, Opus review, user play-test | CH5 review, char-elesis CHNa review |
| CHNb | CH-N (b): the job-change routes to 33 -> 123 and 36 -> 126 and the class-change dialogs | Opus high, Opus review, user play-test | CHNa review |
| CH2a0 | Crest of Time and death-prevention engine (shared `CX2GUUser`), flag `SERV_IRUHADEV_JUNE_USER_CREST_OF_TIME` | Opus high, Opus review | CH1 review; before the skill phase CH0 names |
| CH2b0 | Summons: the six June spawn sites, Apocalypse's binders, flag `_SUMMON` | Opus high, Opus review | CH1 review; before the first skill phase that summons |
| CH2a-CH2d | Skills: Nasod Ruler + first jobs (9, 32, 33, 36) | Sonnet high; one Opus review at CH2d; user play-test | CH1 review (CH2c / CH2d: CHNb review) |
| CH3a-CH3d | Skills: second jobs (122, 123, 126), hyper actives, transcendence rows | Sonnet high (CH3d Opus); one Opus review at CH3d; user play-test | CH2d review (CH3b / CH3c: CHNb review) |
| CH`n`0 | One Opus sub-phase per unmapped helper CH0 finds, before the Sonnet phase that needs it | Opus high, Opus review | as its phase |
| CH4a-CH4b | Passives; overrides + memo | Opus high; one Opus review at CH4b; user play-test | CH3d review |
| CH6 | Whole-character walk and gap reruns | Sonnet medium, user walk | CH4b review, CH5 review, CHNb review |
| Z | Close-out; closes Stage 7 | Sonnet medium | CH6 |

CH1-CH4 all edit `X2GUAdd.*`: one at a time. CH0c, CH5, CHNa and CHNb touch `X2StateCreateUnit.*`, `UnitTemplet.lua`, `X2Unit.h`, `X2GageManager.cpp`,
the emulator and the quest Lua: they may run beside a CH2 phase when the Touches columns do not overlap. If CH0's counts put a group over ~45
functions (or over ~25 where most rows are listing-only), CH0 splits it with a digit suffix and writes the kickoff and STATUS row; it does not merge
groups. Play-test follow-ups after CH1 final get the next letters (`CH1r`).

**Review cadence (user, 2026-11-10).** CH1a-CH1k2 are inert transcriptions in one class file: reviewed in runs (CH1b covers CH1a-CH1b, CH1d2 covers
CH1c-CH1d2, CH1g covers CH1e-CH1g, CH1k2 covers CH1h-CH1k2). CH0c, CH1m-CH1q2, CH1 final, CH5, CHNa, CHNb, CH2a0 and CH2b0 keep their own reviews
(shared code). CH2a-CH2d have one review at CH2d, CH3a-CH3d one at CH3d, CH4a-CH4b one at CH4b: the earlier phases say "no review of its own" and
keep their `_DEBUG` define on; the stage review retires them and gives a verdict line to each incidental shared-file hunk.

## Phase CH0 - Add map (read-only)

**Run with:** Opus 5.5 high. No code, no flag, no build. Ghidra read-only. Delegate reads, greps, tsv joins and known-address dumps to
subagents. Outputs in `june2017_re/char_add/`.

**Step 0 (the pilot's recipe fixes).** Find June's InitState wrapper and body (vslot 0x7a4), `SetEquippedSkillFuncBySkillSlotIndex` `32a28e00`
(0x7b4, listing-only), build the registration map with `charport_states.py init ...` and `skill 32a28e00`, then run `ch0c_listing_check.py` on it:
every row must match the raw listing; `~` rows and std::string-named rows by hand. Map every switch in `32a28e00`. Copy Elesis's / Ara's builders into
`char_add/` with Add's range: say which. Add `add` to `charport_adis.py`'s `CLASS_RANGE`; confirm it lists the 58 listing-only overrides; settle the
range's end (`32a316dd` vs the helpers `32a31740`-`32a317d0` and the gauge-data helpers from `32a31900`).

**Scope** (Ara's / Elesis's CH0 items, with Add's numbers):
1. **Class map** `ch0_class_map.tsv`: every function in range **plus the 58 listing-only overrides**, March pair, verdict, target sub-phase, the
   **`inside` / `new`** column (plain name for every `new` row). Funcmap rows `Char Add CH0`. Every override's role from its body. Rebuild the June-side
   digest as `ch0_june_side.md` (Ara's `plan_june_side.md` shape: key-function table by vslot).
2. **State and skill table** `ch0_state_skill.tsv`: 328 / 328 script tables, every skill id; map June's job tags to classes (`AN` base, `APT` /
   `ALP` Psychic Tracer / Lunatic Psyker, `AAT` / `AMM` Arc Tracer / Mastermind, `ATT` / `ADE` Time Tracer / Diabolic Esper, `COMMON` Crest of Time)
   and say what `A`, `P`, `HA`, `DP`, `CAN` are and which phase each goes to; June's ids for every `ANSI_` name `ADD_NASODRULER_STATE_ID` lacks or
   numbers differently; nothing in `X2GUAdd.*` stores a state id in a `UCHAR` (7l).
3. **Keys** `ch0_keys.tsv`: the 4 June-only `INIT_COMPONENT` keys (DP / formation / Void Impact angles) and `POSSIBLE_NASOD_BARRIER` get an owner; the
   neither keys (`CUSTOM_RENDER_STATE` stand-in owner, `DASHL_CANCEL_AFTER`, `HEAD_IK`, `SPEED_y`) and the 4 march-only-site keys. **Enum gate:**
   `python tools/luabind.py check Add_NasodRuler` (ENUM + STATE): every gap listed with the phase that fills it (CH1 final at the latest).
4. **Checklist rows** `ch0_checklist_rows.tsv`: every §1-§7av row -> phase or n/a (start from `plan_checklist_rows.md` Part A 56 rows and Part B 31);
   correct l.358's Class column.
5. **Assets** `ch0_assets.tsv` + `ch0b_stage_list.tsv` (routes a name / b data / c `item.kim` Add items incl. the 33 / 36 / 123 / 126 promotion and
   costume ids, unioned; `ch0_build_assets.py` gains `add`) and **effect closure** `ch0_effect_closure.tsv`. **The motion kom:** June's data207 has no
   March kom: name the target (March's class motions are in data033). **The motion question:** for each of the 7 differing motions, the animation
   names March's script or class code plays that June's file lacks (`APT_NA_Ready` / `APT_NA_preAttack`, checklist l.760); if any, those files are
   held for CH1 final (`ch1_held_motions.tsv`). Stage the class-change dialogs, `POPUP_TR_*` names from `DLG_Transcendental_Change_Guide_Add.lua`, the
   creation-screen files (item 13), the Moonlight Rhapsody `.tga`, `CREST_OF_TIME_Ring.dds` / `_Time.tga`.
6. **Unmapped helpers.** Per skill, callees outside the class range funcmap does not map (`ch0_unmapped_callees.py`). Each gets an Opus `CH<n>0`
   sub-phase before the Sonnet phase that needs it (kickoff + STATUS row by CH0).
7. **DP / formation map** `ch0_dp.tsv`: every March DP, formation-mode, awakening and hyper function paired with June's (Context lists the known
   ones), the gauge (`CX2AddGageData` vtable vs March's gauge classes in `X2GageData.*` / `X2GageUI.*` / `X2GageManager.cpp`), the R1 edit it
   replaces, the `ConvertAndAdd` 0.5 question (l.279), `SAT_ADD_DP_PERCENT_UP`, the emulator's DP answer. Sized for CH1q (split CH1q3 if over ~45).
8. **Nasod map** `ch0_nasod.tsv`: nasod armor target, barrier slots, Phase Shift die hook, `PlayLoopSound`, packet-buff refusal, the
   `IsSkillUseCountExcludedState` indices `0x123`-`0x126` / `0x12b` mapped to names. Sized for CH1q2.
9. **Crest of Time map** `ch0_crest.tsv`: the hook, the bases, the gate and trigger, every class override (and which earlier ports left a stand-in
   to restore), the skill phase that casts 207 / 29212 / 29230 (CH2a0 runs before it). **Summon map** `ch0_summon.tsv`: the six sites, each NPC id,
   its script and binders (`luabind.py check`), the March pair, the skill and phase; CH2b0 runs before the first.
10. **CH-N map** `ch0_classn.tsv`: every client and emulator site that tests a class against `UC_END` / `UC_VALIDE_END` or switches on class with no
    33 / 36 / 123 / 126 arm, minus what Ara's and Elesis's CHNa already fixed under `SERV_IRUHADEV_JUNE_CLASS_PATH`; June's bound for 123 and 126;
    the job-change routes (quests above, rewards through `Handlers_Quest.cpp`, promotion items in `item.kim`, `X2OfflineClassChangeSeed.h`); the
    `LevelUpEvent.lua` level-15 / level-35 rows and `DLG_1ST_` / `2ND_Class_Change_Guide_Add.lua`; `GetX2UnitClassCompatibility` cases.
11. **Shared data.** State names the C++ enum needs; every `SI_*` id with its June `RNSI_*` id; his 7 `UnitTemplet` rows June vs March; **S3 baseline**:
    `s3_no_state.py --family X2GUAdd.cpp --preprocess` with 33 / 36 / 123 / 126 rows (regenerate the S3 tsv if they are absent). Rebuild the March-side
    digest as `ch0_march_side.md` (Ara's `plan_march_side.md` shape, `cl /P` macro states included).
12. **Stat rows (decision 1, answered).** Fit the formula Lu/Ciel's CH0 found (the existing rows follow June's relative table) against one existing
    base class first, then give the exact rows CH0c (9 / 32 / 122) and CHNa (33 / 36 / 123 / 126) will write, with the column order checked against
    Ara's 118 rows.
13. **Creation-screen map** `ch0_create.tsv`: every June function the create-unit state and unit viewer run for Add (from NU4's list: `SelectAdd`
    `32555320`, UICustomEventProc `32558860` / table `3255aabc`, SelectUpperClass `325519d0`, ChangeCreateUnitClassChangeInfoDlg `325514e0`, the
    class-change dialog loader `325525a0`, info texture `325506e0`, stand mesh `32551e30`, InitDummyUnits `3255b800`, PlaySoundCreateUnit `32550aa0`,
    the create request `325584f0`) with March's pair; every reader of `_CONST_LIMIT_UNIT_::iLimitType` and what lifting it for `UT_ADD` changes;
    June's Add slot in `DLG_Unit_Create_Unit_Button_New.lua` vs March's; `DLG_Unit_Create_ClassChange_Button_Add.lua` (who reads it, the 7 buttons);
    `Movie_Create_Add_*`, voice and background files; the emulator's create path for class 9 (`Handlers_Unit.cpp`, `SeedBeginnerGear`); the select
    screen's Add arms. Sized for CH0c (split if over ~25 functions).
14. **Amend the kickoffs** CH0b-CH5, CHNa, CHNb, CH2a0, CH2b0, CH0c ("Amended by CH0" sections).

**Done gate:** the tsvs exist; `ch0c_listing_check.py` passes on every registration row; every Add checklist row has a phase or n/a; 328 / 328 tables
and every skill id mapped; the 58 listing-only overrides have rows; assets union routes a / b / c and the motion kom is named; the motion question
answered; the DP, nasod, Crest, summon, CH-N and creation maps exist; the enum-gate gaps listed; kickoffs amended; open questions listed.

## Phase CH0c - Add on the creation screen (March classes)

**Run with:** Opus 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ADD_CREATE`. Sub-tag `CREATE`. After CH0 and char-ara's CHNa
review (shared creation state); before Add's class code (it plays on his March kit).

**Scope** (from `ch0_create.tsv`): lift the US limit for `UT_ADD` at the client's use sites (`X2StateCreateUnit.cpp:106-136` and `:173-230`, and every
other `iLimitType` reader CH0 lists) under `_CREATE`, March's code in `#else`; June's `SelectAdd` `32555320` inside March's (`:2346`) and every June
function CH0 maps for his entry (portrait, name / description pictures, stand mesh and lobby animation, voice, background); June's
`DLG_Unit_Create_ClassChange_Button_Add.lua` (`adopt-june`, `luabind.py check`) with his March classes' previews and `Movie_Create_Add_*` for 9 / 32
/ 122; the four June-only buttons harmless before CHNa (check `SelectUpperClass`; if not harmless, hidden under `_CREATE` until CHNa); the slot in
`DLG_Unit_Create_Unit_Button_New.lua` only if June's differs (core-list rule); the emulator's create path for class 9 (create-unit handler, the live-DB
beginner gear row `X2OfflineInventory.cpp:1236` becomes reachable; its comment updated); **stat rows for 9 / 32 / 122** per decision 1 into
`KncWX2Server/ServerResource/US/StatTable.lua` as a labelled stand-in (server Lua: the user packs it into data036; until then the loader's fallback,
logged); stage the creation-screen assets it needs (byte-exact, pack, copy). Full build; play-test: open the creation screen, Add's slot shows, his
preview, voice and class buttons for Psychic Tracer / Lunatic Psyker play June's, create an Add, he is listed on the select screen with his beginner
gear, enters the village and a dungeon, survives a relog; no other character's slot changed.

**Done gate:** a new Add made on the creation screen plays; the stat loader logs his rows (or the logged fallback until the user packs); no new error
against a pre-CH0c log.

## Phase CH0b - Every June Add asset, whole, first

**Run with:** Sonnet 5.5 high, then a user play-test. No C++ unless CH0 found a March reader a June asset breaks (`..._ASSET`). Sub-tag `DATA`.
After char-elesis CH6's walk.

**Scope:** every non-Lua row of `ch0b_stage_list.tsv` not staged by CH0c: motions (to the kom CH0 named; held ones packed and kept for CH1 final),
body / face / hair, equipment (33 / 36 / 123 / 126 promotion sets included), textures, effect meshes, sounds, cut-in art, portraits, `POPUP_TR_*`,
the emotion motions; the class-change dialogs (`adopt-june`, `luabind.py check`). Stage byte-exact, pack each kom, copy each. Save
`data\ErrorLog_pre_CH0b_add.txt` first. After a `db_backup`, `createchar.py` a level-40 9 and a level-99 122. Gate: `python tools/skillicons.py check
<Add's skill-id range from CH0>` = PASS (shared `DLG_SKILL_Passive_*` sheets included; a June-side gap is a `JUNE_BUGS.md` row).

**Done gate:** the stage list re-runs 0 `missing` / `march-copy`; in game one Add per March class, village and dungeon, no new `KEM_ERROR` / `Motion
Error` / missing texture or mesh against the pre-CH0b log; a costume, a weapon and three skills look right.

## Phase CH1 - Deploy unit: June script, motions and the class core

**Groups** (CH0's map fills the counts and the exact function lists; the kickoffs carry the filters):

| Sub-phase | Run with | Scope |
|---|---|---|
| CH1a | Sonnet high | Movement and slot-head states: wait (2) + wait-state-change, walk, jump (6), air / levitation, throw item, common buff, hyper (2) + hyper wait, `CAN` states |
| CH1b | Sonnet high, Opus review (covers CH1a-CH1b) | Reactions: damage (21 `ANSI_DAMAGE*`), stand-up (6), die (2), start (2), win / lose blocks (4), amusement (2), frozen, transformed |
| CH1c | Sonnet high | Riding (27 `ANSI_RIDING*`), pepper run (5), dance (3) |
| CH1d | Sonnet high | Ground combos (25 `ANSI_COMBO*`) and the base `AN` lines |
| CH1d2 | Sonnet high, Opus review (covers CH1c-CH1d2) | Dash and dash / jump combos (18 `ANSI_DASH*`) |
| CH1e | Sonnet high | Psychic Tracer / Lunatic Psyker lines (`APT`, `ALP` and the `A` / `P` states CH0 assigns them) |
| CH1f | Sonnet high | Arc Tracer / Mastermind lines (`AAT`, `AMM`, ...; June-only classes: compiled, reached from CHNa) |
| CH1g | Sonnet high, Opus review (covers CH1e-CH1g) | Time Tracer / Diabolic Esper lines (`ATT`, `ADE`, ...; June-only classes) |
| CH1h | Sonnet high | Carved overrides below vslot 0x600 (lifecycle, render, reaction virtuals; ctor `32a22170` / `32a23b90` offset map; InitComponent with the 4 June-only `INIT_COMPONENT` keys (DP / formation reads under `_DP`)) |
| CH1h2 | Sonnet high | Listing-only overrides below 0x600 (`charport_adis.py add`): 0x30, 0x40, 0x44, 0x5c, 0x60 DamageReact `32a07fc0` (`ResponseEmergency` byte, l.289; super-armor call, l.470), 0x94, 0xb4, 0xc4, 0x1d8, 0x24c / 0x250, 0x29c, 0x2a4 / 0x324 as declarations (CH1q), 0x2ac, 0x2b8 `329e4a50` (= March), 0x3d8, 0x3e0 `329d4640` (CH1q2), 0x4b8, 0x4bc `329e65c0`, 0x4d4, 0x4e0 `32a28860`, 0x500, 0x540, 0x548, 0x54c, 0x560 (CH1q) |
| CH1i | Sonnet high | Overrides 0x600-0x7bc (0x600 HyperModeFrameMove `329ffe90`, 0x658, 0x670, 0x684, 0x6a4 `329e6610`, 0x6bc, **0x6c4 / 0x6f4 training values**, 0x6c8 / 0x6cc and 0x6d4-0x6e8 as declarations for CH1q, 0x71c, 0x738, 0x73c as declaration (CH4b), 0x740 as declaration (CH1q / CH2a0), 0x7b8 `329d1730`; InitPassiveSkillState and the passive hook as declarations: CH4a) |
| CH1j | Sonnet high | Common* overrides 0x7c0-0x7e8 (0x7cc `32a24560`, 0x7d8 / 0x7dc listing-only), CommonFrameMove and the helpers only it reaches |
| CH1k | Sonnet high | Skill-use overrides from 0x7f0: SpecialAttackEventProcess, CanUseSpecialAttack, 0x7fc `32a1e3c0`, 0x800 `32a07dc0` (the seven `CX2GUUser` skill-use helpers called, never copied), slot-510 lookup `32a20e70` (quick-slot and riding-key waits, l.262 / l.265), **`ReservedSkillStart`** with the DP cost (Rules) |
| CH1k2 | Sonnet high, Opus review (covers CH1h-CH1k2) | The rest from 0x810: 0x810 `329e8960`, 0x818, 0x834 `329e80f0`, 0x854 `329ff960` (listing-only), NoStateChangeActionFrameMove, and any CH1h-CH1k row CH0 moves here |
| CH1m | Opus high, Opus review | Registration: June's InitState wrapper / body against March's `InitState` / `InitStateCommon` / `InitStateByUnitClass`, the hyper-mode table names, START / WIN / LOSE roll (`INIT_COMMON_RANDOM_STATE`), base event states, the `_LIVE` gate |
| CH1n | Opus high, Opus review | The `CX2GUUser` base callbacks he registers (only rows earlier characters did not cover); `SetEquippedSkillFuncBySkillSlotIndex` `32a28e00` head (bounds, slot tables, the 7i rule, the slot-state store of l.444 dropped) + the dev free-skill gate |
| CH1o | Opus high, Opus review | The virtuals under `_LIVE`: **vslot 0x524 `32a315a0`** as his `GetNowStateSkillID` override and his four `329cc340` calls on it; `SetSkillLevelStateData` with the `( 3201ad10( skill ) + 1.0 )` term (l.286); `IsSkillUseCountExcludedState` (l.266); gaps the CH1a-CH1k2 reviews routed |
| CH1p | Opus high, Opus review | 7ag pressed-index space (his decode, `CountUsedSkill`, every reader together; `X2GUUser.cpp` "Elesis, Add not ported" set); hooks 7h / 7l / 7o / 7p / 7x; the June-only script keys CH0 gives an owner here |
| CH1q | Opus high, Opus review | DP and formation mode (`ch0_dp.tsv`): `UpNowDPValue` `329e5b00` / `329e56c0` and the spend / gain rates, formation entry `32a08580` + Fantasy Tracer refund, `ChangeMaxCharacterAddResource` `329e31e0`, `ApplyHyperModeBuff` `329e2410` (with `RidingHyperModeEnd`'s base-clear skip), hyper hooks `329e2340` / `329e2360` + `UAI_RELEASE_FORMATION_MODE`, `CanHyperMode` `32a31550` and the five vslot-343 callers, hyper StateStart `32a08150` + `+0x9330`, spec A `329e59f0`, `GetActualDPConsume` `329e7820`, the 0x740 gauge snapshot `329e2f80` as `StoreGageSnapshot` (base call per CH2a0), `CX2AddGageData`, `SAT_ADD_DP_PERCENT_UP`, `PartyFeverAllUser`'s DP step, the buff port's R1 edit replaced; sub-tag `DP` |
| CH1q2 | Opus high, Opus review | Nasod armor, barrier and Phase Shift (`ch0_nasod.tsv`): `SetNasodArmorAttackTarget` `329f63a0` whole, `POSSIBLE_NASOD_BARRIER` and slots 0x4c4 `329e7d60` / 0x4c8 `32a047a0` (base virtuals added), `DieDamageEffectProc` `329f73a0` / `329f7370`, `PlayLoopSound` `329e2660` (JUNE_BUGS 125 verbatim), `329d4640` (BTI 342 / 438 registered if missing); sub-tag `NASOD` |
| CH1 | Opus high, Opus review, user play-test | Define `_LIVE` (+ `_DP`, `_NASOD`); adopt `Add_NasodRuler.lua`; enum gate PASS; copy CH0b's held motion koms (if any); pack, copy; the play-test on 9 / 32 / 122 |

**Transcription rules (CH1a-CH1k2):** as Elesis's (`JUNE_CHARACTER_ELESIS_PHASES.md`, "Phase CH1 - Deploy unit: June script, motions and the class
core", "Transcription rules"), with Add's flags and range: `inside` rows inside March's function under `SERV_IRUHADEV_JUNE_CHAR_ADD_LIVE` with March's
in `#else` and a one-file probe compile; `new` rows under `SERV_IRUHADEV_JUNE_CHAR_ADD_CORE`; a later group's callee declared with `// June <va>: body
in <phase>`; an unmapped engine callee is "this needs Opus" (stop). June's numbers and quirks verbatim; `m_fNowStatePowerRate` (7i) with June's
operator; June's BFI arguments (7ai). **DP / formation lines and nasod lines inside a function a CH1a-CH1k2 phase transcribes** go under `..._DP` /
`..._NASOD` (an `#ifdef` inside the `_LIVE` body) when they call a June piece CH1q / CH1q2 has not written; March's live DP calls stay March's until
CH1q.

**Done gate (CH1 final):** Add (9 / 32 / 122) loads June's script and motions; `[JUNE_CHAR] ADD DATA` logs the load; `luabind.py check
Add_NasodRuler` PASS; no new `KEM_ERROR` / `Motion Error` / missing-animation line against a pre-CH1 run; combos, dash, jump, damage and riding play
June's animations; DP fills, spends and shows on his gauge as June; formation mode enters, costs DP and ends as June; the nasod armor combo follows a
June hit; each skill cast twice shows its cool time and spends MP or DP; `charport_scan.py` / `charport_keys.py Add_NasodRuler` show nothing new.

## Phase CH5 - UI and data: UnitTemplet rows, cut-ins, tree-UI pieces

**Run with:** Sonnet 5.5 high, Opus review, user play-test. Flag `SERV_IRUHADEV_JUNE_CHAR_ADD_UI`. Sub-tag `UI`.

**Scope:** his 3 `UnitTemplet.lua` rows become June's and June's rows for 33 / 36 / 123 / 126 are added (BOM source, `luasrc.py refresh` after; if CH0
found the templet loader refuses 123 / 126, those rows wait for CHNa): `m_fLobbyRotationY`, `m_HyperCutIn` for 122 / 123 / 126, the names; the cut-in
table and InitState index switch replaced by June's names and layout (7t); 7ah; the tree-UI pieces 7aj / 7ak (`328e0aa0`'s `+0x138` / `+0x139`, the
install / G-Core marks `328e0290` for unit type 9, the decide-ACK refresh), his `UpdateSkillSlotUI` class pass (5 + 5 wide); lobby particle
hard-codes in `X2UnitViewerUI.*` retired where June's `AnimationNameTable` plays them (Ara's decision 2 precedent). The creation screen is CH0c's
(March classes) and CHNa's (June-only classes), not CH5's.

**Done gate:** a select-screen pass of all nine March characters with no new error; his skill and hyper cut-ins play June's art; the class names
show June's strings; the skill tree's install / G-Core marks show on his slots.

## Phase CH-N - Add's June-only class paths: Arc Tracer (33) -> Mastermind (123), Time Tracer (36) -> Diabolic Esper (126)

The template block is `JUNE_CHARACTER_AISHA_PHASES.md` "Phase CH-N" as Ara's Z rewrites it; Ara ran it first, Elesis second. **What Add reuses**
(flag `SERV_IRUHADEV_JUNE_CLASS_PATH`): Ara's socket-loader bound, `GetClassLevel`, the log sites, the June `FieldQuest.lua`, the quest reward path
that accepts any class; Elesis's third-line helper arms. **What is his** (flag `SERV_IRUHADEV_JUNE_CHAR_ADD_CLASS`): everything in `ch0_classn.tsv`
marked `Add`. **His new problem:** two classes at or past the end markers (123 = `UC_VALIDE_END`, 126 > `UC_END`): every test CH0 lists gets June's
bound, read from June's own test, never a guessed end value. Sub-tag `CLASS`.

| Sub-phase | Scope |
|---|---|
| CHNa | **The four classes load and play** (`ch0_classn.tsv`): the end-marker tests for 123 / 126; the gauge create for all four (`X2GageManager.cpp`); `GetX2UnitClassCompatibility` cases (l.1009, June `31ed6500`); the second- and third-line helper arms and their callers; the class switches inside `X2GUAdd.cpp`; `X2OfflineLuaEnumSeed.h` names; `tools/createchar.py` (`CLASSES` / `PARENT` / `PROMO`); **stat rows** for 33 / 36 / 123 / 126 per decision 1 (`StatTable.lua`, the user packs it); `Motion_ADD_AT.x` / `_MM.x` / `_TT.x` / `_DE.x` reach the unit; **the creation screen's June-only entries** (the four class buttons, previews, `Movie_Create_Add_*`); the units: `createchar.py` a 33 and a 36 (level 20), a 123 and a 126 (level 40), a level-99 123 and 126 for CH3. Play-test: all four load in the village and a dungeon, combos and the DP gauge work, the job name shows, a socketed weapon keeps its socket, the creation screen shows all seven entries. |
| CHNb | **The job-change routes and dialogs** (`ch0_classn.tsv`): quests 35540-35543 (to 33), 35550-35554 (to 123), 35580-35583 (to 36), 35590-35594 (to 126) with their rewards through `Handlers_Quest.cpp`; `LevelUpEvent.lua`'s level-15 row opens `DLG_1ST_Class_Change_Guide_Add.lua` (three paths) and the level-35 row the second guide; the `DLG_Class_Change_Add_*` dialogs; `X2OfflineClassChangeSeed.h` rows only if June's cash route needs them; `luabind.py check` on each dialog. Play-test: a level-15 Nasod Ruler sees the three-path dialog, takes 35540-35543 (`/qdone`) and becomes Arc Tracer; a level-35 Arc Tracer takes 35550-35554 and becomes Mastermind; a second level-15 unit takes 35580-35583 -> Time Tracer, then 35590-35594 -> Diabolic Esper; a third still reaches Psychic Tracer; all survive a relog. |

**Done gate:** a 33, 36, 123 and 126 reached both by job change and by `createchar.py` load, fight and relog; the gauge, socket and stat paths log no
refusal; no other class changed (a select-screen pass of all nine March characters).

## Phase CH2 - Skills: Nasod Ruler and the first jobs (classes 9, 32, 33, 36)

**Run with:** Sonnet 5.5 high (transcription from CH0's map), one Opus review at CH2d (covers CH2a-CH2d), user play-test per phase. Flag
`SERV_IRUHADEV_JUNE_CHAR_ADD_SKILL`. Sub-tags `SKILL`, `NUM`. A skill's line decides its phase: class 9 has it -> CH2a; else the first of 32 / 33 / 36.

| Sub-phase | Scope |
|---|---|
| CH2a0 | **Opus.** Crest of Time and death prevention (`ch0_crest.tsv`): slot-250 hook `3201ffb0`, the bases `32009780` / `3204da60` (vslots 0x740 / 0x744, members `+0x92a4..+0x92c4`), the gate `320072d0` in `UpNowHp` / `SetNowHp` after the BBT 101 refusal (27409 / 23402) and trigger `3202b9a0`; every class's override calls the base (Chung, Elsword, Rena, Ara, Elesis stand-ins restored; Add's `329e2f80` / `329e3000`); flag `SERV_IRUHADEV_JUNE_USER_CREST_OF_TIME` (own review); the cast itself is the skill phase's |
| CH2b0 | **Opus.** Summons (`ch0_summon.tsv`): the six June spawn sites with June's spawn rules, NPC ids by name (`X2UnitManager.h` under `JUNE_ENUM_NAMES` if missing), the summon scripts adopted with `luabind.py check` (bind `GetEffectSetInstance` `321d97c0` and `SetLocalRotateDegree` `31e4c520`, drop their `lua_unbound_known.tsv` rows), summon follow-ups of `3230c1a0` (l.280); flag `_SUMMON` (own review) |
| CH2a | Nasod Ruler (9) skills |
| CH2b | Psychic Tracer (32) skills |
| CH2c | Arc Tracer (33) skills (after CHNb: tested on a 33 unit) |
| CH2d | Time Tracer (36) skills (after CHNb); CH2 stage review |

For each id: its case in `SetEquippedSkillFuncBySkillSlotIndex` with June's state functions whole; its attached rows (7u names, 7ai BFI arguments,
7h UPDOWN cancels, the power-rate operator, the numerical-only rows of `plan_numerical_rows.tsv`, the effect-modifier tags 7a l.306 / l.311 and alpha
tags 7c l.356, `USEFUL_VALUE` readers l.427, `GetLandPosition( ..., true )` l.680, the `CanDown( NULL, &idx )` shape l.781); its effect closure (a
name found only now is staged and added to `ch0_assets.tsv`); its DP gain / cost on CH1q's code. Upgrade ids reach their base through
`m_iDeleteSkillIDByUpgrade` (June `32071720`). `NUM use` lines; March vs June damage in `MODS.md`.

**Done gate:** every class-9 / 32 / 33 / 36 id in the S3 census has a state (except CH0's passive / CH4 ids); each skill in an A and a B slot casts
June's state, motion and effect, deals damage, shows its cool time and spends MP or DP; no missing particle / mesh / texture line.

## Phase CH3 - Skills: second jobs (122, 123, 126), hyper actives, transcendence

**Run with:** as CH2 (CH3d on Opus: it carries engine rows); one Opus review at CH3d (covers CH3a-CH3d). Sub-tag `SKILL`.

| Sub-phase | Scope |
|---|---|
| CH3a | Lunatic Psyker (122) skills (Supersonic 29058's HP-rate gate, l.358) |
| CH3b | Mastermind (123) skills (after CHNb): Apocalypse on CH2b0's summon path, the `+0x98e4` store and its readers (l.288) |
| CH3c | Diabolic Esper (126) skills (after CHNb) |
| CH3d | **Opus.** Hyper actives (`RNSI_HA`, June's hyper switch in `32a28e00`), the `SHOW_HYPER_CUT_IN_TIME` states (7t), and the **transcendence rows**: 122 / 123 / 126 on the offline transcend allow-list (`TranscendCheck`, `Handlers_Quest.cpp`), his `LevelUpEvent.lua` TYPE rows + `DLG_Transcendental_Change_Guide_Add.lua`; checklist 7ao whole (l.968 `+0x9330`; l.982's 13 readers of `CX2Unit +0x3c`, each branch whole; the four base helpers); hyper-class skills (`SPRT_HYPER_CLASS`), `TRANSCENDENTAL_SKILL_DATA`, the transcended hyper table and `HQ_Cutin_*_Transcendence`; `list_TR_*` rows in `GetCharacterImageName` (T6c) and his transcended pictures from June's row pushes; `POPUP_TR_*` present; CH3 stage review |

**Done gate:** as CH2 for 122 / 123 / 126 and the hyper actives; a transcended Add of each second job (`settranscend`) casts his hyper-class
skills, slots A-5 / B-5 work, the transcended picture and cut-in show; `python tools/skillicons.py check <Add's skill-id range>` = PASS.

## Phase CH4 - Passives and class mechanics

**Run with:** Opus 5.5 high, one Opus review at CH4b (covers CH4a-CH4b), user play-test (CH0 adds CH4c if a passive engine piece or the memo is over
~45 functions).

| Sub-phase | Scope |
|---|---|
| CH4a | `InitPassiveSkillState` and his passive ids, their objects and readers; the delegate registrations `32a1ccb0` (A `32a04ce0`, `329d4190`, `32a01d20`; D `32a01dd0`) replacing March's pushes (l.924 / l.932); BBT 92 `CHANGE_MAXIMIZE` with `+0x9680` (l.251, if buff Phase 5c landed; else routed); the user float `+0x92a0` his passives add (l.1053, vslot 0x824, divergent row 401); the passive-id check of the Rules; flag `_PASSIVE` |
| CH4b | Overrides and memo: vslot 0x73c `329d46b0` (l.943), spec B `32a01ca2` (l.945), 0x3ac / 0x6a8 if CH0 finds he overrides them (l.991-994), the socket power-rate range if a function of his calls `3201ad10` (l.283), his memo condition class (if any) and the memo readers; CH4 stage review |

**Done gate:** each passive's effect shows (CH0 names the stat or log line per passive); 0 Add `no_state` rows left except passives with no state in
June.

## Phase CH6 - Whole-character walk and gap reruns

**Run with:** Sonnet 5.5 medium, then the user's walk. Back up the save; one Add per class (9 / 32 / 33 / 36 / 122 / 123 / 126), one transcended per
second job. Walk: a new Add made on the creation screen; every skill in an A and a B slot (each cast twice: cool time, MP or DP), a hyper, DP (fill,
spend, formation mode in and out, gauge), the nasod armor combo, Phase Shift, Crest of Time (a death prevented, HP restored), each summon, the passives,
one dungeon per job, all three first-job changes on fresh units (CHNb's quests), then **leave the game and return to the village**. Re-run
`census.py`, `charport_scan.py`, `charport_keys.py Add_NasodRuler` and `s3_no_state.py`; before / after numbers in the Done note. **The
becoming-transcended transition on un-transcended units (user rule, 2026-10-28):** one unit per second-job class (122, 123, 126) by `/tc 1`, quest
42030 (`/qdone 42030`) and the book 288493 (`/additem 288493 1`): the guide popup with that class's picture, the new skill-tree rows, the HUD
portrait, a relog. A defect goes back to its phase's flag.

**Done gate:** the walk passes; 0 unexplained `no_state` rows; no new `KEM_ERROR`.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium. `MODS.md` "June character: Add" section (flags, files, data, revert order: exe + data036 + motion koms together;
the user-packed `StatTable.lua` rows; the creation-screen limit); no `_DEBUG` flag left defined; checklist rows marked `(Add: CHn)`; the Aisha plan's
"Template for Stage 7" gets an Add paragraph (what he took against this plan); backlog Stage 7 closed (all eight March characters); the
`stage7-plan` row closed; `char-luciel`'s parked rows unparked per the user's 2026-11-03 rule (Stage 7 closed), and the `char-add` block moved to
`STATUS_ARCHIVE.md` with a *Closed plans* row.

## Play-test hand-off (every phase)

Dev console first; copy-ready lines with real values (unit, class, skill id and slot, `/additem`, `/qdone`); per check: where / paste / what it does /
look for / if not. Log tag `[JUNE_CHAR]`, `python tools/logs.py --grep "JUNE_CHAR ADD <sub>"`.

## Open decisions (for the user; asked at the end of planning)

1. **Stat rows for every Add class.** `StatTable.lua` has none for 9 / 32 / 122 and none can exist for 33 / 36 / 123 / 126; June ships only relative
   rows (`UT_ADD` `380, 88, 480, 103, 9500`; 122 `400, 92, 530, 115, 11000`; 123 `440, 95, 510, 110, 10750`; 126 `390, 100, 530, 115, 10500`).
   **(a, recommended)** derive: class 9 from June's `UT_ADD` row with the formula Lu/Ciel's CH0 found (levels 1-80; 81-99 by `cap99_gen.py`), checked
   against an existing base class first; 32 / 33 / 36 = class 9's rows verbatim (Ara's rule for first jobs); 122 / 123 / 126 = class 9's rows with level 1
   = 0.8 x their own relative row and the top levels regenerated (Ara's rule for second jobs); a labelled stand-in; you pack `StatTable.lua`. **(b)** you
   supply newer rows. **(c)** leave the loader's fallback.
   **Answered (user, 2026-11-11): (a), derive.** CH0 item 12 fits and checks the formula; CH0c writes 9 / 32 / 122 (Add becomes creatable there),
   CHNa writes 33 / 36 / 123 / 126; the user packs `StatTable.lua`; `MODS.md` records the rows as a stand-in.

Settled by standing rules, not decisions: the creation-screen entry is this port's (user, 2026-11-10 / 2026-11-11) and June shows Add, so the US
limit is lifted for him; the motion koms deploy with CH1 final if June drops a name March plays (7ad); Ghidra stays read-only and listing-only
functions are read with `charport_adis.py`; `CUSTOM_RENDER_STATE` is a labelled stand-in until june-engine R5 (7w); `PlayLoopSound`'s use-after-free
is kept (JUNE_BUGS 125); the nasod armor gate follows June (user, 2026-10-07); June-side icon gaps are `JUNE_BUGS.md` rows; emotion motions are staged,
their behaviour is Stage 10; third jobs are out of scope.
