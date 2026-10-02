# June 2017 Buff/Debuff Port — phased plan

> Written 2026-10-02 and committed on `mods/lanox-port`. Start with Phase 0's kickoff
> prompt in a new conversation. Every kickoff prompt below points at this file.

## Context

The user wants **every June 2017 buff and debuff type**, for both characters and mobs, to work in the March 2014 client. This follows the standing goal of March→June engine parity ([[goal-march-client-runs-june-effects]]).

Today March can load June's enum names; Ranox/Phase 11 added about 160 of them. It cannot *run* most June buffs, for four reasons:

- **Enums.** June's `Enum.lua` has 705 more `BUFF_TEMPLET_ID`s, 734 more `BUFF_FACTOR_ID`s, 38 more `BUFF_BEHAVIOR_TYPE`s and 3 more `BUFF_FINALIZER_TYPE`s than March's. It also has 6 buff enum tables March lacks entirely.
- **Missing handlers.** The C++ factories have no case for 41 behavior/finalizer types that June data uses. Five of those are already in March's Lua Enum but have no C++: `BBT_INVINCIBLE`=77, `BBT_IMMUNE_NON_CONTROL_DEBUFF`=95, `BFT_AURA`=14, `BFT_STICK_SHAKE`, `BFT_USE_SKILL_BY_SKILL_SLOTTYPE`.
- **Unread keys.** About 40 data keys are never read by March. Examples: `IS_STORE_BUFF`, `BUFF_REPLACE_DETAIL_OPERATION`, `SKILL_CATEGORY_n`, `APPLY_CHECK_CONDITION`, `ACCUMULATION_DECREASE_*`.
- **One failure cascades.** March's `AddBuffFactor_LUA` (`X2Lib/X2BuffTempletManager.cpp:75`) only accepts factor N while exactly N-1 factors are already loaded. So one unparseable June factor silently drops **every later factor**. Each failure also raises a modal `DISPLAY_ERROR` popup, because `_SERVICE_` is not defined under `US_SERVICE`.

The intended outcome:
- June's own `BattleFieldBuffTemplet.lua`, `BattleFieldBuffFactor.lua`, `UnitApplyBuffGroup.lua` and `BuffTemplet.lua` load cleanly. They replace March's copies.
- Every June BBT, BFT and BDT runs on March classes, transcribed from the June binary.
- Buffs placed by mob scripts and damage data (241 June scripts carry inline `BUFF_FACTOR` blocks) work once those scripts are adopted.

The user decided (2026-10-02):
- **Branch.** Settle the open Lanox edits, then branch `mods/june-buff-port` off `mods/lanox-port`.
- **Absent characters.** Port June's full functions now, including the Rose, Ain and Lu/Ciel cases. They stay dormant until those characters arrive.

---

## Fix philosophy (the user's standing rules, applied here)

- **No partial June updates** ([[no-partial-june-updates]]). If June changed a function, parser or `case` that March already has, port June's whole version. Diff every case.
- **June wins** ([[june-behaviour-wins-even-when-it-removes-features]]). If June dropped or renumbered something, follow June.
- **No guessing.** Every new C++ path cites the June function address in a comment. Never port raw struct offsets; re-express the logic on March classes.
- **Key names need two sources:** the decrypted string in `june2017_re/xorstrings.tsv` and the name as it appears in the decompiled June Lua.
- **Missing systems: parse, store, log.** When June depends on a system March lacks entirely, parse and store the data, then log it as `[JUNE_BUFF] unsupported ...`. Never drop the entry silently.
- **House rules from `CLAUDE.md`:**
  - Gate every change behind a `SERV_IRUHADEV_JUNE_BUFF_*` flag in `KTDXLIB/Always.h`, keep the original code in `#else`, and use the `#endif FLAG` style.
  - Make CP949 edits at the byte level, then verify with `file` and `git diff --stat`.
  - Never touch `KncWX2Server/Common`.

## Model, effort, review

| Phase | Item | Depends on | Kind | Model | Effort | Review |
|---|---|---|---|---|---|---|
| 0 | Branch, doc, `buff_census.py` | – | tooling, read-only | Sonnet 5.5 | medium | none |
| 1 | June buff engine RE map | 0 | Ghidra, read-only | Opus 5.5 | **xhigh** | none (self-check vs census) |
| 2 | Enum reconciliation (Lua + C++) | 1 | data + C++ | Sonnet 5.5 | high | required (Opus 5.5) |
| 3 | Loader hardening + `[JUNE_BUFF]` logging | 1, 2 | C++ | Opus 5.5 | high | required |
| 4a | Templet/factor top-level parity (stacking, replace, conditions) | 3 | C++ | Opus 5.5 | high | required |
| 4b | Existing BBT/BFT/BDT parser + runtime parity | 3 | C++ | Opus 5.5 | high | required |
| 5 | New BBT group A: stat/multiplier | 2, 4b | C++ transcription | Sonnet 5.5 | high | required (Opus 5.5) |
| 6 | New BBT group B: skill-scoped | 2, 4b | C++ transcription | Sonnet 5.5 | high | required (Opus 5.5) |
| 7 | New BBT group C: flags/immunity | 2, 4b | C++ (new mechanisms) | Opus 5.5 | high | required |
| 8 | New finalizers + UDP counter sync | 2, 4b | C++ (new mechanisms) | Opus 5.5 | high | required |
| 9 | Custom-function buffs (17 June BTIs) | 4b | C++ (new classes) | Opus 5.5 | high | required |
| 10 | Buff groups + June-only script bindings | 3 | C++ + data | Opus 5.5 | high | required |
| 11 | Premium/world buffs (`BuffTemplet.lua`) + offline path | 3 | C++ + data (+ offline) | Opus 5.5 | high | required |
| 12 | Buff icons/UI assets | 2 | data | Sonnet 5.5 | medium | light |
| 13 | Adopt June buff data, build, hand off pack | 4–12 | data + build | Sonnet 5.5 | medium | required (Opus 5.5) |
| 14 | Play-test sweep + close-out | 13 | verify + root-cause | Opus 5.5 | high | none |

Phases 5–9 are independent of each other and can run in any order. Phase 13 must come last among the data phases.

### Model selection: cost vs performance

**Prices.** First-party API, $ per million tokens, from the `claude-api` skill's model table (cached 2026-09-25):

| Model | Input | Output | Cache read | Context |
|---|---|---|---|---|
| Opus 5.5 | 4 | 20 | 0.20 | 1M |
| Sonnet 5.5 | 2 | 10 | 0.20 | 1M |
| Haiku 4.5 | 1 | 5 | – | **200K** |

**Rules behind the column.**

1. **Price the failure, not the request.** A wrong enum value, a misread Ghidra branch, or a mis-sliced float layout compiles cleanly. Nobody notices it until a pack plus a play-test, and every peer must then rebuild. That cycle costs more than the token gap between models. So any phase that *derives* behaviour from the June binary runs on Opus 5.5.
2. **Most input is cache reads, and those cost the same on Opus and Sonnet ($0.20).** In long Claude Code sessions the repeated context (the plan doc, decompiles, big source files) is mostly cache reads. So the *input* gap between Opus and Sonnet is well under the list 2×. Output, including thinking, keeps the full 2× gap. This narrows the savings from downgrading. Downgrade only where the method is fully written down.
3. **Reuse the house split: identify with a strong model, transcribe with a cheap one** (`JUNE2017_EFFECT_REBUILD_PLAN.md` used Opus to identify, then Sonnet plus an Opus `/code-review`).
   - Phase 1 must record, for each type, everything a transcriber needs: the parse layout, the store, the read sites and pseudocode notes.
   - Phases 5 and 6 add new behaviour types into stores and patterns March already has (`InsertAdditionalStat`, the skill-ID lists). That is transcription, so Sonnet 5.5 at high effort does it, and an Opus 5.5 review is required before build and pack.
   - **Escalation rule:** if a Sonnet session finds that the Phase 1 map does not settle a type, it stops on that type and logs it in the doc for an Opus pass. It does not guess.
4. **Phase 1 runs on Opus 5.5 at `xhigh` effort, the only phase above `high`.**
   - Phase 1 is the one long-horizon reasoning task. It maps about 45 types, the loaders and 17 custom-function classes out of a 43 MB binary with XOR-encrypted keys.
   - Every later phase builds on what it records, so a mistake there is copied into up to ten phases. The extra thinking pays for itself here and nowhere else.
   - **Fable 5.1 is not used in this port** (the user's decision, 2026-10-02: too expensive). Do not propose it for any phase or escalation.
   - **Optional cost control:** if a single session grows too large (many decompiles re-read as cache), split Phase 1 into two sessions. 1a covers the loaders, parsers and the BBT/BFT parse and level-slice cases. 1b covers the runtime stores, read sites, custom functions and bindings. Each session writes into the same function-map table.
5. **No Haiku 4.5.** Its 200K context cannot hold June's `Enum.src.lua` (336 KB) next to a buff file (the factor file alone is 2.1 MB source). The phases that are actually mechanical (12, 13) still need a judgment call at their gates: adopt vs merge for an atlas, and "census not 0 → stop". Haiku saves too little to cover a missed gate.
6. **Phases 2 and 13 run on Sonnet.**
   - Phase 2's Lua half is a scripted diff, and each value is checked against two sources (June's Enum plus census). Its C++ half is about 45 enumerators with explicit values, plus a consumer audit of 5 drifted IDs. That is bounded grep-and-read work, and the required Opus review catches value slips.
   - Phase 13 is compile, stage and pack-list work behind a hard numeric gate.
7. **Opus 5.5 everywhere else.**
   - Phases 3, 4a and 4b change code that every buff in the game runs through.
   - Phases 7–9 add new mechanisms inside `X2GUUser.cpp`'s state machine: invincibility, immunity, input gates, UDP counters, and whole new hook bodies. March has no pattern to copy for these, and the file is a 37k-line CP949 source.
   - Phase 10 ports binding bodies and the blocked `APPLY_LEARNED_SKILL_*` path.
   - Phase 11 touches the offline emulator, with live-DB judgment calls.
   - Phase 14 is open-ended root-causing.
8. **Effort must be stated in the kickoff.**
   - Opus 5.5 defaults to `medium`, so every Opus kickoff says `high` (Phase 1: `xhigh`).
   - Sonnet 5.5 defaults to `high`. Phases 12 and 13 set `medium`: their work is scripted and their gates are numeric.
9. **Measure and revise.**
   - At the end of each phase, record the session's cost or usage in the phase's "Done" note. Use `/cost` dollars on API billing; on a subscription, record the usage or limit consumption instead.
   - **A Phase 7 type may move to Sonnet** only if Phase 1 marked it transcription-ready. Clean results from Phases 5 and 6 do not justify it: they only show that Sonnet transcribes faithfully, not that it can build a mechanism March lacks.
   - **Upgrade Phases 5 and 6 to Opus** if their reviews find value or layout errors.
10. **Scope of the required reviews (keeps the Sonnet saving real).**
    - A required review is a fresh Opus 5.5 high session that checks the diff against the Phase 1 function map: enum values, float/key order, store member, read sites, and `SERV_IRUHADEV_` / `#else` / CP949 hygiene.
    - It spot-checks **2–3 types** directly in Ghidra and does not re-derive the phase. If a spot-check fails, it escalates to a full re-check of that phase.
    - Phase 2's review checks the values against June's `Enum.src.lua` plus the consumer audit. Phase 13's review checks the staged file set, the manifests and the census=0 output.
    - The review's cost counts toward the phase.

---

## Starting context (paste with any phase)

- Repo root: `F:\elsword stuff\elsword_2014\els_2014\ElswordFiles\ready_and_built\ProjectX2_SVN\ProjectX2_SVN\source\EU_CN_US\Trunk`
- Branch: `mods/june-buff-port`, created in Phase 0 off `mods/lanox-port`.
- Game directory (deploy target, logs, save): `F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data`
- Build and deploy:
  - Follow `CLAUDE.md` *Toolchains* and *Deploying the offline client* exactly. Config is `US_SERVICE`, msbuild is called by full path, and `SolutionDir` needs a trailing `/`.
  - After any header edit, `touch X2Lib/stdafx.cpp` first.
  - Copy `X2/US_SERVICE/x2.exe` to `$DATA/X2_offline.exe` and confirm the size and mtime programmatically.
  - **Ask the user before launching, every time.**
- Lua sources:
  - **June Lua comes from `june_src_fixed/data###_kom/<Name>.src.lua`, always.**
    - Every file there is already decompiled and fixed (the opcode-table and dropped-assignment bugs).
    - Read it, diff it, census it, and compile from it with `elstools compile-batch -v mar-2014`.
    - Do **not** re-decompile June files with the `june2017_re/` tools or `elstools`/`ljd`, and do not read the raw `.src.lua` files beside `14JUN17/data/dataNNN_kom/`.
    - Use the `june2017_re/` tools only to extract a June file that is **missing** from `june_src_fixed/`, and record in the doc which file that was.
    - Non-Lua June assets (`.tga`/`.tet`) still come from `14JUN17/data/dataNNN_kom/`.
  - The buff files used here: `june_src_fixed/data036_kom/` BattleFieldBuffTemplet, BattleFieldBuffFactor, the `_INT` variants, UnitApplyBuffGroup, BuffTemplet and Enum. Mob, character and damage-effect scripts are in `data036_kom`, `data037_kom` and `datai016_kom`.
  - March: the editable Enum is `march2014/data036/Enum.src.lua`.
  - March's live `BattleFieldBuff*.lua` exist only as bytecode in `kom_contents/data036_kom/`; there is no decompile. Their string constants are readable.
- Compile and stage:
  - Compile with `elstools compile-batch -v mar-2014`. Never use `-v jun-*`.
  - Stage the result in **both** `march2014/data036/` and `kom_contents/data036_kom/`, and update that directory's `_manifest.json` (`.lua` = "2"; CRLF, 2-space indent, no trailing newline).
  - The user packs. Verify against the live `.kom` with `elstools inspect`, never against the staging directory.
- **Pairing hazard:**
  - The `Enum.lua` already staged in `kom_contents/data036_kom/` carries unshipped Lanox and June work. It requires the `HIT_TYPE`-widened exe (`JUNE2017_TRIAL_LOAD.md:61-67`).
  - Every buff phase that changes Lua data must ship **together with** the exe built in the same phase. Templet and factor files must also ship as one version, because `CX2BuffTemplet::SetFactor` fails when their behavior sets differ.
- June binary:
  - `14JUN17/data/unpacked_x2.exe` in Ghidra (`F:\elsword stuff\els_jun_2017_RE\els_jun_2017.gpr`), imagebase `0x31aa0000`. Drive it with the `ghidra` skill (`curl --noproxy '*'`).
  - Lua keys are XOR-encrypted per string; the decrypted strings are in `june2017_re/xorstrings.tsv` (cite only rows with status `ok`).
  - Record confirmed functions in `june2017_re/funcmap.tsv`.
- Logs:
  - Load time: `ErrorLog.txt`, `OpenScriptFile.log`.
  - Runtime: `offline_server.log` (`CX2OfflineLog::Server`).
  - Prefix every new line `[JUNE_BUFF]`. Debug-only lines go behind `SERV_IRUHADEV_JUNE_BUFF_DEBUG`.
- Never delete `els_db.sql`. Kill processes by PID only, never by image name.

## Investigation ledger (measured 2026-10-02 — do not redo)

**Enum diff (June `Enum.src.lua` vs March staged `march2014/data036/Enum.src.lua`):**

| Table | June | March | New in June | Value changed |
|---|---|---|---|---|
| BUFF_TEMPLET_ID | 1065 | 360 | 705 | `BTI_BUFF_HAMEL_SECRET_DUNGEON_ARMOR/WING_EVENT` 1321/1322 → 347/348 (June reuses 1321/1322 for `BTI_PHYSIC_MAGIC_ATTACK_DEFENCE_BUFF` / `BTI_MOVE_FAST_BUFF`) |
| BUFF_FACTOR_ID | 1236 | 503 | 734 | `BFI_BUFF_THE_ESSENCE_OF_WEAK_HERETIC_POTION` / `_HERETIC_POTION` / `BFI_BUFF_RURIEL_MANA_ENERGIZE_POTION` 362–364 → 481–483. March-only: `BFI_DEBUFF_TELEPORT_CONSUME_MP`=290 (June: `..._PVP`=290) |
| BUFF_BEHAVIOR_TYPE | 113 | 75 | 38 (73–112) | none |
| BUFF_FINALIZER_TYPE | 18 | 15 | 3 (`BFT_STICK_SHAKE` 15, `BFT_USE_SKILL_BY_SKILL_SLOTTYPE` 16, `BFT_ATTACK` 17) | none |
| BUFF_DISPLAYER_TYPE | 14 | 14 | 0 | none |

The 6 tables June has and March lacks:
- `BUFF_APPLY_CHECK_CONDITION_TYPE` (`BACCT_*` 0–2)
- `BUFF_VALUE_CALCULATE_TYPE` (`BVC_RALLY_OF_HERO`)
- `BUFF_REPLACE_DETAIL_OP` (`BRDO_*` 0–2)
- `BUFF_GROUP_ID` (`BGI_*` 0–5)
- `BUFF_GROUP_APPLY_DEFAULT_VALUE` (`BGADV_*` 0–5)
- `SPA_BUFF_TYPE` (`SBT_GROW`/`SBT_STRONG`)

**C++ (`X2Lib/X2BuffFactor.h`) vs March Lua.**
- BBT 0–72, BFT 0–13 and BDT match exactly.
- The C++ enums **lack** `BBT_INVINCIBLE`=77, `BBT_IMMUNE_NON_CONTROL_DEBUFF`=95 and `BFT_AURA`=14, even though March's Lua has them.
- New enumerators **must carry an explicit `= N`**, because June has gaps.

**C++ BTI/BFI drift against June:**

| C++ name | C++ value | June |
|---|---|---|
| `BTI_SI_SA_EEG_ATOMIC_SHIELD` | 124 | 141 |
| `BFI_BUFF_SWORD_ENCHANT_CHARGE_MP` | 338 | 337 |
| `BTI_BUFF_INDURANCE_POWER` | 238 | named `BTI_BUFF_ENDURANCE_POWER` |
| `BTI_CHANGE_PHYSIC_DEFENCE_AND_PHYSIC_DAMAGE` | 292 | named `BTI_CHANGE_ATTACK_AND_DEFENCE` |
| `BFI_DEBUFF_TELEPORT_CONSUME_MP` | 290 | absent (`#ifdef FIX_SKILL_BALANCE_AISHA_LENA`) |

**Data shape:**
- June `BattleFieldBuffFactor.src.lua` has 1130 `AddBuffFactor_LUA` calls. The IDs are **contiguous 1..1130 in file order**, so March's ordinal loader can take the file as-is.
- `_INT` factors are 10001–10069 (69 entries) and `_INT` templets 10001–10072. These break the ordinal rule; Phase 1 decides how June stores them.
- Templets: 929 entries, IDs 1..1333, unsorted, 22 gaps. They are stored in a map, which is fine.
- June also defines 34 templets and 135 factors for IDs that are in March's enum but have no March data row.

**June keys March never reads** (entries using them):
- Templet level: `IS_STORE_BUFF` 930, `BUFF_REPLACE_DETAIL_OPERATION` 21, `EXP_RATE` 12, `ACCUMULATION_DECREASE_WHEN_FINISH` 5, `ACCUMULATION_DECREASE_COUNT_WHEN_FINISH` 4, `ACCUMULATION_DELAY_TIME` 3, `IS_INSTANCE_KILL_DEBUFF` 3, `IS_PARTY_BUFF` 1, `NOT_APPLY_BUFF_WHEN_SAME_BUFF_OWNER` 1.
- Factor level: `APPLY_CHECK_CONDITION` / `AND_OPERATION` / `BUFF_APPLY_CHECK_CONDITION_TYPE` / `CONDITION_0..1` / `CHECK_BUFF_TEMPLET_ID` / `CHECK_HP_RATE_UP` / `HP_RATE` / `APPLY_WHEN_HAVE` (4), `ACCUMULATION_AT_ONE_GO` 2, `DISPLAYER` inside a factor 6.
- Behavior level: `SKILL_CATEGORY_0..4` 22/12/12/12/8, `MULTIPLY_DAMAGE_RATE` 20, `BUFF_VALUE_CALCULATE_TYPE` 10, `VALUE` 7, `SKILL_ID_LIST` 5, `IMMUNE_BUFF_TEMPLATE_ID` 5, `ONLY_MY_UNIT` 3, `NOT_DELETE_EFFECT_SET_WHEN_FINISH` 3, `SKILL_SLOT_TYPE` 3, `ADDITIONAL_ATTACK_TYPE` 3, `DROP_RATE` 3, `FORCE_SKILL_ID` 2, `MULTIPLY_MP_CHARGE_RATE` 2, `APPLY_TO_SPECIAL_ATTACK` 2, `LUCKYTIMINGEVENT_*`.
- Once each: `FROZEN_TIME`, `IGNORE_DEFENCE_RATE`, `HYPER_COUNT`, `ALL_DEBUFF`, `IGNORE_SKILL_ID`, `OFFSET_POSITION`, `DELETE_BUFF_WHEN_DIE`, `FORCE_SET`, `USE_CREATOR_UNIT_DATA`.
- Finalizer level: `SHAKE_DECREASE_MAX_TIME`, `SHAKE_DECREASE_TIME_AT_ONCE`.
- Displayer level: `REPLAY_EFFECT_SET_ACCUMULATION_LIMIT1` 18; per-class keys `ADD`/`AIN`/`LUCIEL`/`ROSE`.
- Note: March reads the BBT/BFT/BDT sub-tables by stringizing the enum name (`CASE_*` macros), so those names are *not* unread keys.

**New type usage**: templets / factors, the bucket, and an example.

| Type | T/F | Bucket | Example |
|---|---|---|---|
| IGNORE_STATE_GUARD 73 | 3/4 | skill, group | PANDEMONIUM_FEAR |
| DETONATION_IMPOSSIBLE 74 | 3/3 | dmgeff | ASSAULT_STRIKE |
| DOUBLE_ATTACK_BUFF 75 | 7/9 | skill, item | BRUTAL_CUTTER |
| CHANGE_HYPER_DURATION 76 | 3/3 | item | ERDA_RESOURCE |
| INVINCIBLE 77 | 5/4 | npc, item | ERDA_BLESSING |
| TYPE_ADDITIONAL_ATTACK_MULTIPLIER 78 | 2/3 | npc, item | DEVIL_CHAIN_BALL |
| CHANGE_NOW_FORCE_DOWN_PER_SECOND 79 | 3/3 | dmgeff | JUDGEMENT_FIRE |
| *_BY_CONDITION 80–84 | 5/5 each | item | RALLY_OF_HERO_LEVEL_n |
| RANGE/MELEE_COMMAND_* 85/86/87/96 | 3/2/2/1 | skill | MIND_OF_MARKMAN |
| IGNORE_DEFENCE 88 | 1/1 | skill | CUTTING_SWORD_IGNORE_DEFENCE |
| CHANGE_SKILL_DAMAGE 89 | 15/16 | item, skill | 2013_DEFENSE_BUFF |
| SKILL_DAMAGE_BY_SKILL_ID 90 | 3/3 | skill | SI_A_LWS_NATURE_FORCE |
| CHARGE_MP_HIT_BY_SKILL_ID 91 | 1/2 | skill | – |
| CHANGE_MAXIMIZE 92 | 20/21 | skill, item | SI_P_EIS_LIGHTNING_STEP |
| EVERY_ATTR_DEF_HAVE_BUFF_OWNER 93 | 2/2 | – | DECREASE_ENEMY_RESIST |
| IMMUNE_BUFF_DEBUFF 94 | 4/5 | class, dmgeff | BLOODLUST |
| IMMUNE_NON_CONTROL_DEBUFF 95 | 1/0 | dmgeff | – |
| MAX_CHARACTER_ADD_RESOURCE 97 | 1/1 | – | – |
| SKILL_DAMAGE_BY_SKILL_SLOTTYPE 98 | 1/1 | – | TRANSCENDENTAL_SLOT |
| CONSUME_MP_RATE_BY_SKILLTYPE 99 | 0/0 | unused | – |
| COOLTIME_RATE_BY_SKILLTYPE 100 | 1/1 | – | OVER_STRIKE_RSR |
| IGNORE_HP_DECREASE 101 | 1/1 | item | SOLESS_TITLE_INVINCIBLE |
| DETONATION_IMPOSSIBLE_GAGE_RESET 102 | 1/1 | dmgeff | – |
| CHANGE_SKILL_COOL_TIME_SPEED 103 | 1/1 | skill | RAPID_GUARDIAN |
| JUMP_IMPOSSIBLE 104 | 1/1 | dmgeff | HEAVY_FIELD |
| IMMUNE_EDT 105 | 1/1 | class | ALPTRAUM_READY |
| FORCE_DOWN_*_ATTACKED 106/107 | 1/1 | item | – |
| FINAL_DAMAGE_MULTIPLIER_NPC/USER 108/109 | 4/4, 2/2 | – | SPELL_OF_DESTROY |
| AWAKE_IMPOSSIBLE 110 | 1/1 | dmgeff | VOID_FIELD |
| MP_NATURAL_RECOVERY_IMPOSSIBLE 111 | 0/0 | unused | – |
| REFLECT_PHYSIC 112 | 0/0 | unused | – |
| BFT_AURA 14 | 10/10 | item | PHASE_OF_PERKISAS |
| BFT_STICK_SHAKE 15 | 2/2 | npc | HOLD_WITH_STICK_SHAKE |
| BFT_USE_SKILL_BY_SKILL_SLOTTYPE 16 | 1/1 | – | – |
| BFT_ATTACK 17 | 0/0 | unused in any June lua | – |

The three types unused in June data (`BBT_CONSUME_MP_RATE_BY_SKILLTYPE`, `BBT_MP_NATURAL_RECOVERY_IMPOSSIBLE`, `BBT_REFLECT_PHYSIC`) plus `BFT_ATTACK` are still ported if June's binary implements them. That is the no-partial rule.

**Custom-function buffs.**
- June has 20 `BBT_CUSTOM_FUNCTION` templets. March's `CX2GameUnit::CreateAndInsertCustomFunctionByBuff` (`X2GameUnit.cpp:13448`) handles 3 of them: PHANTOM_SWORD, UNFIXED_CLIP and MARK_OF_COMMANDER.
- The 17 unhandled ones:
  - BLOOD_REVENGE_COUNTER, BLOOD_RUNAWAY, DEATH_SIGN
  - NITRO_SERVO, NITRO_SERVO_UPGRADE
  - DEBUFF_AIMING_STIGMA, ATOMIC_BLASTER_PLASMAGUN_UPGRADE, ATTACK_ME, A_AAP_MODER_MARKING
  - FLASH_BANG, GEIST_HERRSCHAFT, MOONLIGHT_RHAPSODY, PANDEMONIUM_FEAR
  - SA_RCR_CHAIN_VIPER, SINISTER_MIND_SEED, DEBUFF_TARGET, TRAPPINGARROW_FUNGUS_DEBUFF
- March handles FLASH_BANG and PANDEMONIUM_FEAR elsewhere, in per-class code (`X2GUAra.cpp:13854`, `X2GURaven.cpp:21764`).

**Scripts outside the buff files:**
- 241 June scripts carry inline `BUFF_FACTOR = {` blocks. These go through the same factor parser, with `bIsSkill=false` (`X2DamageManager.cpp:3830`).
- `IMMUNITY_LIST_AT_THIS_STATE`: 248 uses in 60 files. It is a positional BTI list, and a nil entry truncates it silently.

**June-only Lua bindings:**

| Binding | Where June uses it |
|---|---|
| `AddBuffGroup_LUA` / `AddOfficiallyBuffList_LUA` | UnitApplyBuffGroup |
| `SetMonsterBuffGroupID` | ELSION_MAYA(_HENIR) |
| `DeleteNonControlDebuff` | FIRE_DEVIL_CHAIN_IPLITAN(_HENIR) |
| `GetBuffAccumulationCountNow` | ELYSION_M_SOLESS |
| `ApplyBuffFactor` | GameEditCommandList (cheat) |
| `AddRandomBuffList` | CustomEventScript |
| `SetPveBalanceBuffGiveScore_LUA` | – |
| `SendRandomMissionRewardBuffCheat` | – |

- June-only keys found in the binary (location to be confirmed in Phase 1): `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_BUFF_TEMPLATE_ID` / `_SKILL_ID`, `DELETE_BUFF_WHEN_DIE`, `DUMMY_BUFF_TEMPLET_ID`, `IS_AURA_BUFF`, `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`, `CHECK_RIDINGPET_SPACIAR_BUFF`, `ALL_BUFF` / `ALL_DEBUFF`.

**Strings and icons:**
- All 1,097 `STR_ID`s that June templets use exist in March's `march2014/data036/StringID_def.src.lua`.
- Icons are read under `BUFF_ICON_UI` (defined at `Always.h:2205`).
- 315 June templets use an icon key missing from March's `DLG_UI_Common_Texture_Buff.tet` (195 keys in March, 370 in June; same `.tga` size, different art). Take the June file wholesale from June `data215` to March `data057`.
- `DLG_UI_Common_Texture_Buff_INT.tga/.tet` is absent from March (18 `_INT` templets use it).

**Premium buffs (`BuffTemplet.lua`, `BuffManager:AddBuffInfo`):**
- 138 June entries plus 71 `_INT`, against 57 in March.
- The only new key is `NOT_USE_IN_PVE`.
- It is read by `X2PremiumBuffManager.cpp:981` and keyed by factor ordinal (`m_iFactorID`).

**Characters absent from March:**
- Class files: Rose (Freyja, Gunner, Mechanic, Ranger), Ain (Angel, Apostasia, Arme_Thaumaturgy, Erbluhen_Emotion), Luciel (Demonio, Duo).
- Buffs referenced only by these characters: 11 BTI and 15 BFI.
- Their C++ is ported now (user decision) and stays dormant until the characters arrive.

**March runtime map (from the survey):**
- *Loading.* `X2StateStartUp.cpp:229-230` calls `OpenScriptFileForTemplet`/`ForFactor`. `X2Main.cpp:5149-5150` binds the Lua functions.
- *Parsing.*
  - Templet parse: `X2BuffTemplet.cpp:14-101`.
  - Factor parse: `X2BuffFactor.cpp:492-516`. Behavior cases at `:108-221` (`UPGRADE_SKILL_SYSTEM_2013` is on); finalizer cases at `:368-414`.
  - Level-slice: `GetBehaviorFactorToLevel` `:2273`; `GetFinalizerFactorToLevel` `:2444`. Each default branch mis-slices or drops values for an unknown type.
- *Templet and finalizer factories.* `CASE_BEHAVIOR_TEMPLET` is at `X2BuffBehaviorTemplet.cpp:51-133`, with its default at `:147`. `CASE_FINALIZER_TEMPLET` is at `X2BuffFinalizerTemplet.cpp:49-67`.
- *Applying a buff.* `SetBuffFactorToGameUnit` (`X2GameUnit.cpp:11174`) → `PushNewBuffTempletToGameUnit` (`:11265`) → `OnFrameMoveBuff` (`:12662`).
- *Per-unit stores.* Declared at `X2GameUnit.h:4489-4597`. `InsertAdditionalStat` (`:11467`) and `EraseAdditionalStat` (`:11868`) write them; `CreateAndInsertBuffIdentity` (`:12931`) fills the flag lists.
- *Immunity.* Checked at `X2GameUnit.cpp:13630`.
- *Finalizers.* They poll unit counters. Hit count is updated at `X2GUUser.cpp:12721`, `X2GUNPC.cpp:13332` and `X2DamageEffect.cpp:5462`; hitted count at `X2GUUser.cpp:8965`. Both are synced over UDP (`X2UDPPacket.h:310`, `.cpp:227/274/367`) and applied at `X2GUUser.cpp:17441`.
- *Wire format.* `KBuffFactor` and friends are type + `vector<float>`, so new types don't change the wire format.
- *Offline mode.* There are no buff handlers in `X2Lib/Offline`.

**June RE leads** (from `xorstrings.tsv` / `func_strings.tsv`). These are unconfirmed; Phase 1 confirms them.

| Address | What it appears to be |
|---|---|
| `326c0020` | Templet file loader |
| `326c13a0` | Factor file loader |
| `326c26f0` | UnitApplyBuffGroup loader |
| `3258c470` | `BuffTemplet.lua` loader |
| `3236a9b0` | Templet keys |
| `32726850` | Factor/behavior keys |
| `32d4c9c0` | BEHAVIOR sub-table |
| `32d4ca60` | FINALIZER sub-table |
| `32d4bbf0`, `32e80450` | BBT names |
| `32d4c750`, `32ec36f0` | BFT names |
| `31d95080` | lua_tinker binds |

Already confirmed: `32630d20` `ParsingBuffFactorID` and `32632390` `SetDamageDataFromLUA`.

## Flags (one per logical change, all in `KTDXLIB/Always.h`)

| Flag | Phase |
|---|---|
| `SERV_IRUHADEV_JUNE_BUFF_ENUM` | 2 |
| `SERV_IRUHADEV_JUNE_BUFF_LOADER` | 3 |
| `SERV_IRUHADEV_JUNE_BUFF_DEBUG` | 3 onward, removable |
| `SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY` | 4a |
| `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY` | 4b |
| `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT` | 5 |
| `SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL` | 6 |
| `SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG` | 7 |
| `SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER` | 8 |
| `SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION` | 9 |
| `SERV_IRUHADEV_JUNE_BUFF_GROUP` | 10 |
| `SERV_IRUHADEV_JUNE_BUFF_PREMIUM` | 11 |

## The exit metric every phase uses

`june2017_re/buff_census.py` is built in Phase 0. It parses:
- March's C++, for the `CASE_*` type lists and the quoted key literals in `X2Buff*.cpp`;
- June's buff Lua, including the `_INT` files;
- the inline `BUFF_FACTOR` blocks in all `june_src_fixed` scripts.

Its June input is `june_src_fixed/` only, never raw bytecode or a fresh decompile.

It prints the BBT, BFT, BDT and keys that June data uses and March cannot handle, with entry counts, plus the enum diff. A phase is done when:
- its types and keys drop out of that report;
- the build returns rc=0;
- the deployed exe contains its new log strings.

The play-test confirmation comes in Phase 14.

---

## Phase 0 — Branch, doc, census tool

**Steps**
1. Check that `mods/lanox-port` is clean (`git status`). The Lanox edits and this doc were already committed on 2026-10-02. If anything is uncommitted, **ask the user** what to do with it before branching. Then create `mods/june-buff-port` from `mods/lanox-port`.
2. Write `june2017_re/buff_census.py`, covering the sources in *The exit metric* above. The baseline numbers must reproduce the ledger: 38 BBT, 3 BFT and the key list. Add `--phase N` filters by type group.
3. Record the baseline output in the doc.

**Run with:** Sonnet 5.5, medium effort. Tooling only.

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root
in full -- "Starting context", "Investigation ledger", "The exit metric" and
"Phase 0". Never launch X2_offline.exe without asking first.

Your task (Phase 0): confirm mods/lanox-port is clean (if anything is uncommitted,
ask me what to do with it first), then create branch mods/june-buff-port from it.
Then write
june2017_re/buff_census.py: parse March X2Lib/X2Buff*.cpp CASE_* macros and quoted
key literals, June june_src_fixed BattleFieldBuff*/UnitApplyBuffGroup/BuffTemplet
lua and every inline BUFF_FACTOR block in june_src_fixed, and the two Enum.src.lua
files; print unsupported BBT/BFT/BDT/keys with entry counts and the enum diff. Its
baseline must reproduce the ledger's numbers -- if it doesn't, find out which is wrong
before going on. Record the baseline in the doc and commit.
```

## Phase 1 — June buff engine RE map (read-only)

**Steps.** In Ghidra, confirm and name these June functions:
- `CX2BuffTempletManager::AddBuffTemplet_LUA`, `AddBuffFactor_LUA`, `AddBuffGroup_LUA`, `AddOfficiallyBuffList_LUA`, and the file loaders. Answer: does June keep the ordinal rule? How does it store `_INT` 10001+ factors? Does the US/June build load `_INT` at all?
- `CX2BuffTemplet::ParsingScript`, `CX2BuffFactor::ParsingBuffFactor`, the per-BBT/BFT parse switches, `GetBehaviorFactorToLevel` and `GetFinalizerFactorToLevel`.
- The behavior templet factory, the finalizer templet factory and the displayer factory.
- `CX2GameUnit::SetBuffFactorToGameUnit`, `PushNewBuffTempletToGameUnit`, `InsertAdditionalStat`/`EraseAdditionalStat`, `CreateAndInsertBuffIdentity`, `CreateAndInsertCustomFunctionByBuff`, `OnFrameMoveBuff`, and the immunity check.
- The binding sites of every June-only binding and key in the ledger. In particular, find where the `APPLY_LEARNED_SKILL_*` keys are read and identify `FUN_31fcf3e0` / `FUN_3227baf0` (blocked since effect-plan Phase 8b).
- Where `BFT_ATTACK`, `BBT_REFLECT_PHYSIC`, `BBT_MP_NATURAL_RECOVERY_IMPOSSIBLE` and `BBT_CONSUME_MP_RATE_BY_SKILLTYPE` are implemented, if anywhere.

**Output:**
- Add `funcmap.tsv` rows marked `confirmed`.
- Add a "June buff function map" table to the doc, listing address → March symbol → the phase that ports it.
- For each of the 41 new types, record: parse layout (the float vector order, which is the wire layout), the unit store it writes, and the read sites.
- Re-split Phases 5–9 if the RE shows a better grouping.
- **Transcription-ready bar.** Phases 5 and 6 run on Sonnet without doing their own RE, so each of their types needs all of the following:
  - the June parse function address and its key/float order;
  - the `GetFactor`/`SetFactorFromPacket` layout;
  - the unit-store member it writes, and its type;
  - every June read site, given as address plus a short pseudocode note.
- A type that cannot reach this bar is marked **"Opus-only"** in the map.

**Run with:** Opus 5.5, `xhigh` effort. This is the leverage phase (see *Model selection*, rule 4). Ghidra RE, read-only, no code changes. It may be split into 1a and 1b (rule 4).

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root
in full -- especially "Starting context", the "Investigation ledger" (its RE leads
table) and "Phase 1". Branch: mods/june-buff-port. Use the ghidra skill on
unpacked_x2.exe; do not mutate the Ghidra project without asking.

Your task (Phase 1, read-only): confirm and map the June 2017 buff engine. Start from
the RE leads (326c0020/326c13a0 loaders, 3236a9b0 templet keys, 32726850 factor keys,
32d4c9c0/32d4ca60 sub-tables, 32d4bbf0/32e80450 BBT names, 32d4c750/32ec36f0 BFT
names, 31d95080 binds). Settle: does June keep AddBuffFactor_LUA's ordinal rule, and
how are _INT ids 10001+ stored and when are they loaded? For each of the 38 new BBT and
4 BFT (incl. BFT_AURA, and BBT_INVINCIBLE/IMMUNE_NON_CONTROL_DEBUFF which March's Lua
already has), record parse layout, unit store, and read sites. Locate every June-only
binding/key in the ledger, and identify FUN_31fcf3e0 / FUN_3227baf0
(APPLY_LEARNED_SKILL_*). Write confirmed rows into june2017_re/funcmap.tsv and a
"June buff function map" table into the doc (address -> March symbol -> phase).
Every Phase 5/6 type must meet the doc's "transcription-ready bar", because those
phases run on Sonnet without RE; mark any type that can't as "Opus-only". Adjust the
Phase 5-9 grouping if the RE warrants it. Record this session's /cost in the Done note.
Commit.
```

## Phase 2 — Enum reconciliation

**Steps**
1. **Lua.** Add every June buff table and member to `march2014/data036/Enum.src.lua`, at June's values:
   - BTI/BFI/BBT/BFT members, plus the 6 new tables;
   - the HAMEL BTI pair, the three potion BFIs and `BFI_..._TELEPORT_CONSUME_MP_PVP`, which take June's numbers.

   Before adding, check that each value is free with a comma-agnostic pattern (see `RANOX_PORT_PLAN.md:1670`). Compile with mar-2014 and stage it, merging into the already-staged `Enum.lua`, not replacing it.
2. **C++, `X2BuffFactor.h`.**
   - Add every new BBT and BFT enumerator with an explicit `= N`, including 77, 95 and `BFT_AURA` 14.
   - Fix the C++/June drift in the ledger table. Audit every C++ consumer of each renumbered or renamed ID.
   - Add BTI/BFI enumerators **only** where C++ code compares against them; never all 705/734.
   - Confirm `X2Lib/Offline` and `els_db.sql` store no BTI/BFI numbers.
3. Build, deploy, and leave the Enum staged for the user's pack. Ship it paired with the exe.

**Run with:** Sonnet 5.5, high effort. The Lua half is a scripted diff checked against two sources, and the C++ half is about 45 explicit enumerators plus a 5-ID consumer audit. An Opus 5.5 `/code-review` is **required** before the pack hand-off, because a wrong value is silent.

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context" (note the pairing hazard), the "Investigation ledger" (enum
diff and C++ drift tables) and "Phase 2". Branch: mods/june-buff-port. Never launch
X2_offline.exe without asking first.

Your task (Phase 2): bring every buff enum to June. In march2014/data036/Enum.src.lua add
all June BUFF_* members and the 6 new tables at June's values. Apply June's renumbering
of the HAMEL BTI pair, the three potion BFIs and TELEPORT_CONSUME_MP(_PVP), checking
that each value is free with a comma-agnostic pattern. Compile with elstools -v mar-2014
and stage it into both march2014/data036 and kom_contents/data036_kom, merging with the
already-staged Enum. In X2Lib/X2BuffFactor.h, under new flag
SERV_IRUHADEV_JUNE_BUFF_ENUM (original in #else), add all new BBT/BFT with explicit
=N values. Fix the C++ drift (ATOMIC_SHIELD 124->141, SWORD_ENCHANT_CHARGE_MP
338->337, ENDURANCE and ATTACK_AND_DEFENCE renames) and audit every consumer.
Add BTI/BFI enumerators only where C++ compares. Byte-level CP949 edits; verify with
`file` + `git diff --stat`. Rerun buff_census.py, build, deploy, then hand the pack to
me naming both staging places. Commit.
```

## Phase 3 — Loader hardening and `[JUNE_BUFF]` logging

**Steps**
1. Port June's `AddBuffTemplet_LUA` and `AddBuffFactor_LUA` whole (Phase 1 addresses). Include June's handling of `_INT` IDs if June has it.
2. Wherever March would drop an entry, keep ordinals aligned: push a placeholder factor that logs and is never applied. This is the parse/store/log rule.
3. Replace `DISPLAY_ERROR` popups on the buff parse path with `[JUNE_BUFF] unsupported <type/key> in <BTI/BFI>` lines, written once per entry to `ErrorLog.txt`.
4. Add a load summary line: templets loaded, factors loaded, placeholders.
5. Optionally, gate debug detail behind `SERV_IRUHADEV_JUNE_BUFF_DEBUG`.

**Exit.** Load March's current data: the summary shows 0 placeholders and behavior is unchanged. A local trial (the `JUNE2017_TRIAL_LOAD.md` harness method, **not** a loose file in the game directory) of June's files produces an unsupported list that matches `buff_census.py`.

**Run with:** Opus 5.5, high effort. Review required.

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger", the "June buff function map" from
Phase 1, and "Phase 3". Branch: mods/june-buff-port. Never launch X2_offline.exe
without asking first.

Your task (Phase 3): make the buff loader survive June data. March's AddBuffFactor_LUA
(X2BuffTempletManager.cpp:75) drops any factor that fails to parse, and that cascades to
every later factor; each failure is also a modal DISPLAY_ERROR popup. Port June's
AddBuffTemplet_LUA/AddBuffFactor_LUA whole, including its _INT id handling, under new
flag SERV_IRUHADEV_JUNE_BUFF_LOADER. Where an entry can't be parsed, push a placeholder
so ordinals stay aligned, and log `[JUNE_BUFF] unsupported ...` instead of popping a
box. Add a load summary line. Verify that March data loads with 0 placeholders, and
that a trial of June's files (TRIAL_LOAD harness method, never a loose file in the game
dir) gives an unsupported list matching buff_census.py. Build, deploy, commit.
```

## Phase 4a — Templet/factor top-level parity

**Scope.** Port June's whole `CX2BuffTemplet::ParsingScript` and `CX2BuffFactor::ParsingBuffFactor`, plus the apply path they feed (`SetBuffFactorToGameUnit` and `PushNewBuffTempletToGameUnit`, diffed in full). This covers:
- the templet keys: `IS_STORE_BUFF`, `BUFF_REPLACE_DETAIL_OPERATION` (`BRDO_*`), `ACCUMULATION_DECREASE_*`, `ACCUMULATION_DELAY_TIME`, `EXP_RATE`, `IS_INSTANCE_KILL_DEBUFF`, `IS_PARTY_BUFF`, `NOT_APPLY_BUFF_WHEN_SAME_BUFF_OWNER`;
- the factor keys: `APPLY_CHECK_CONDITION` (`BACCT_*`, `AND_OPERATION`, `CONDITION_n`), `ACCUMULATION_AT_ONE_GO`, and a factor-level `DISPLAYER`.

Keys whose system March lacks (for example EXP_RATE consumers) follow the parse/store/log rule.

**Run with:** Opus 5.5, high effort. Review required.

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger" (the unread-keys list), the
"June buff function map" and "Phase 4a". Branch: mods/june-buff-port. Never launch
X2_offline.exe without asking first.

Your task (Phase 4a): port June's whole CX2BuffTemplet::ParsingScript and
CX2BuffFactor::ParsingBuffFactor, plus June's SetBuffFactorToGameUnit and
PushNewBuffTempletToGameUnit (stacking, replace-detail, accumulation decrease/delay,
apply-check conditions, same-owner rule), diffing every branch against March. Do this
under new flag SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY with the original in #else, and
cite June addresses in comments. Keys whose consuming system March lacks: parse, store,
log `[JUNE_BUFF] unsupported`. buff_census.py must show these keys cleared. Byte-level
CP949 edits. Build, deploy, commit; ask before any launch.
```

## Phase 4b — Existing BBT/BFT/BDT parity

**Scope.** For each of the 73 existing BBT, 14 BFT and 14 BDT, diff June against March in four places, and port June in full wherever they differ:
- the factor parse case;
- the level-slice case;
- the templet class (`ParsingBehavior`, `SetFactor`, `DoFinish`, `OnFrameMove`, and `GetFactor`/`SetFactorFromPacket`, which must keep the same float order);
- the unit-side store and read sites.

Known new keys inside existing types: `SKILL_CATEGORY_n`, `MULTIPLY_DAMAGE_RATE`, `VALUE`, `BUFF_VALUE_CALCULATE_TYPE`, `SKILL_ID_LIST`, `ONLY_MY_UNIT`, `NOT_DELETE_EFFECT_SET_WHEN_FINISH`, `REPLAY_EFFECT_SET_ACCUMULATION_LIMIT1`, the per-class displayer keys, `FROZEN_TIME`, `HYPER_COUNT`, `OFFSET_POSITION`, and the rest of the ledger list. If this is too large for one session, split it at 4b-i (behaviors) and 4b-ii (finalizers and displayers).

**Run with:** Opus 5.5, high effort. Review required.

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger", the "June buff function map" and
"Phase 4b". Branch: mods/june-buff-port. Never launch X2_offline.exe without asking
first.

Your task (Phase 4b): bring every EXISTING March buff behavior (73 BBT), finalizer
(14 BFT) and displayer (14 BDT) to June parity. Per type, diff the factor parse case
(X2BuffFactor.cpp:108-221 / 368-414), the level-slice case (:2273 / :2444), the templet
class virtuals, and the CX2GameUnit store and read sites against June. Port June's
version whole wherever it differs (no partial June updates). Keep
GetFactor/SetFactorFromPacket float order identical, because it is the UDP wire
layout. New flag SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY. If the scope is too large,
finish behaviors, commit, and record exactly where you stopped in the doc so 4b-ii
resumes there. buff_census.py must show the in-type keys cleared. Build, deploy, commit.
```

## Phases 5–7 — New behavior types

Each new BBT follows an 8-step checklist:

1. Add the C++ enum, done in Phase 2.
2. Add the factor parse case in **both** the skill and non-skill macro lists.
3. Add the level-slice case.
4. Add a `SetGameUnitBehavior` case, if the type uses `BCT_RELATION_VALUE`.
5. Add the templet class and its `CASE_BEHAVIOR_TEMPLET` entry.
6. Add the unit store with insert and erase, plus June's read sites.
7. Cover the UDP layout through `GetFactor`/`SetFactorFromPacket`.
8. Log the type's first application under the DEBUG flag.

**Phase 5, group A: stat and multiplier types** (flag `..._NEW_STAT`).
76 HYPER_DURATION, 78 TYPE_ADDITIONAL_ATTACK_MULTIPLIER, 79 NOW_FORCE_DOWN_PER_SECOND, 80–84 *_BY_CONDITION, 85/86/87/96 *_COMMAND_*, 88 IGNORE_DEFENCE, 92 MAXIMIZE, 93 EVERY_ATTR_DEF_HAVE_BUFF_OWNER, 97 MAX_CHARACTER_ADD_RESOURCE, 106/107 FORCE_DOWN_*_ATTACKED, 108/109 FINAL_DAMAGE_MULTIPLIER_NPC/USER.

**Phase 6, group B: skill-scoped types** (flag `..._NEW_SKILL`).
75 DOUBLE_ATTACK_BUFF, 89 SKILL_DAMAGE, 90 BY_SKILL_ID, 91 CHARGE_MP_HIT_BY_SKILL_ID, 98 BY_SKILL_SLOTTYPE, 99 CONSUME_MP_RATE_BY_SKILLTYPE, 100 COOLTIME_RATE_BY_SKILLTYPE, 103 SKILL_COOL_TIME_SPEED.

**Phase 7, group C: flag and immunity types** (flag `..._NEW_FLAG`).
73 IGNORE_STATE_GUARD, 74 DETONATION_IMPOSSIBLE, 102 DETONATION_IMPOSSIBLE_GAGE_RESET, 77 INVINCIBLE, 94 IMMUNE_BUFF_DEBUFF (`IMMUNE_BUFF_TEMPLATE_ID`, `ALL_BUFF`/`ALL_DEBUFF`), 95 IMMUNE_NON_CONTROL_DEBUFF (needs June's "officially debuff" list; coordinate with Phase 10), 101 IGNORE_HP_DECREASE, 104 JUMP_IMPOSSIBLE, 105 IMMUNE_EDT, 110 AWAKE_IMPOSSIBLE, 111 MP_NATURAL_RECOVERY_IMPOSSIBLE, 112 REFLECT_PHYSIC.

**Run with:**
- **Phases 5 and 6:** Sonnet 5.5, high effort. These are pure transcription into stores and patterns March already has, from Phase 1's transcription-ready map. An Opus 5.5 `/code-review` of the diff is required before build and pack.
- **Phase 7:** Opus 5.5, high effort. It adds new mechanisms inside the user/NPC state machine. Review required.
- Any type Phase 1 marked "Opus-only" moves to an Opus session, whichever group it is in.
- Under rule 9, a Phase 7 type may move to Sonnet only if Phase 1 marked it transcription-ready; Phases 5 and 6 move to Opus if their reviews find errors.

Kickoff template (substitute N, group and type list):

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger", the "June buff function map" and
"Phases 5-7" (the 8-step per-BBT checklist). Branch: mods/june-buff-port. Never launch
X2_offline.exe without asking first.

Your task (Phase <N>, group <A|B|C>): implement these June behavior types on March
classes: <type list from the doc>. For each one, open June's parse case, templet class
and unit-side read sites in Ghidra ONLY at the addresses the function map already
gives, transcribe them, then follow the 8-step checklist. Do not search the binary
for a function the map lacks. Cite the June address in a comment and never invent a value or
formula. Use new flag SERV_IRUHADEV_JUNE_BUFF_<NEW_STAT|NEW_SKILL|NEW_FLAG>. Types
June only uses for characters March lacks (Rose/Ain/Lu-Ciel) are ported anyway --
user decision. If you are running on Sonnet and the Phase 1 map does not settle a
type (missing layout, store or read site), do NOT reverse-engineer or guess it: skip
that type, mark it "Opus-only" in the doc, and keep going. Done means
buff_census.py lists none of the types you completed, the build is rc=0, and the
strings are in the deployed exe. Write a per-type "how to see it in game" line (a June
BTI/BFI and where it comes from) into the doc for Phase 14. Record this session's
/cost in the Done note. Commit.
```

## Phase 8 — New finalizers and counter sync

**Scope.**
- Port `BFT_AURA` 14, `BFT_STICK_SHAKE` 15 (with `SHAKE_DECREASE_*`), `BFT_USE_SKILL_BY_SKILL_SLOTTYPE` 16 and `BFT_ATTACK` 17. Each needs its enum (done in Phase 2), a parse case, a level-slice case, `SetGameUnitFinalizer`, a class with its `CASE_FINALIZER_TEMPLET` entry, a unit counter, the site that increments it, and **UDP sync of the counter**, alongside hit and hitted count in `X2UDPPacket.h:310`.
- Candidate hook sites:
  - `BFT_ATTACK`: `CX2GUUser::AttackResult` `X2GUUser.cpp:12721`.
  - `BFT_USE_SKILL_BY_SKILL_SLOTTYPE`: `CommonSpecialAttackEventProcess` `:20966` / `CountUsedSkill` `:32125`.
  - `BFT_STICK_SHAKE`: the grab-escape counter `m_iGrapShakeCount` `:14651`, plus NPC `SHOW_STICK_SHAKE` `X2GUNPC.cpp:5154`.
  - Confirm every one of these against June.

**Run with:** Opus 5.5, high effort. Review required. The UDP change means every peer must run the same build; say so in `MODS.md`.

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger" (March runtime map: finalizers
poll counters synced over UDP), the "June buff function map" and "Phase 8". Branch:
mods/june-buff-port. Never launch X2_offline.exe without asking first.

Your task (Phase 8): port June's BFT_AURA, BFT_STICK_SHAKE (SHAKE_DECREASE_*),
BFT_USE_SKILL_BY_SKILL_SLOTTYPE and BFT_ATTACK, from parse to finalizer class, unit
counter and increment site. Take the hook sites from June's binary; the doc lists March
candidates to confirm. Sync the new counters over UDP the same way March syncs
hit/hitted count (X2UDPPacket.h:310), and note the peer-build requirement in MODS.md.
New flag SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER. Done means buff_census.py lists no
BFT, the build is rc=0, and the deploy is done. Commit.
```

## Phase 9 — Custom-function buffs

**Scope.**
- Port June's whole `CreateAndInsertCustomFunctionByBuff` switch and every `CX2CustomFunctionByBuff*` class it creates, for the 17 BTIs in the ledger. Include the Rose, Ain and Lu/Ciel cases (user decision).
- Port each hook body: CommonStateStart, FrameMove, StateEnd and DamageReact (`X2GUUser.cpp:14613/17894/19467/8327`).
- Where June moved per-class handling of FLASH_BANG or PANDEMONIUM_FEAR (`X2GUAra.cpp:13854`, `X2GURaven.cpp:21764`) into the buff system, follow June.
- Split into 9a and 9b by character family if needed.

**Run with:** Opus 5.5, high effort. Review required.

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
```

## Phase 10 — Buff groups and June-only script bindings

**Scope.**
- Load `UnitApplyBuffGroup.lua` with `AddBuffGroup_LUA` and `AddOfficiallyBuffList_LUA`. Follow June: it likely replaces March's hard-coded `m_vecPossibleOfficiallyDebuffList` (`X2Game.cpp:19051`).
- Add the bindings `SetMonsterBuffGroupID`, `DeleteNonControlDebuff`, `GetBuffAccumulationCountNow`, `ApplyBuffFactor` and `AddRandomBuffList`, plus `SetPveBalanceBuffGiveScore_LUA` and `SendRandomMissionRewardBuffCheat` if June wires them on the client.
- Add the damage-data and NPC keys located in Phase 1: `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_*`, `BUFF_FACTOR_ID_ULTIMATE(_PVP)`/`_PVP`, `DELETE_BUFF_WHEN_DIE`, `DUMMY_BUFF_TEMPLET_ID`, `IS_AURA_BUFF`, `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`, `CHECK_RIDINGPET_SPACIAR_BUFF`.
- Stage `UnitApplyBuffGroup.lua` as a **new** file in `march2014/update/data036/` **and** `kom_contents/data036_kom/` + `_manifest.json`.

**Run with:** Opus 5.5, high effort. Review required.

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
```

## Phase 11 — Premium/world buffs and the offline path

**Scope.**
- Diff June's `X2PremiumBuffManager` parse and apply against March (`:981`, `:546-560`). Add `NOT_USE_IN_PVE`. Adopt June `BuffTemplet.lua`, and `_INT` if Phase 1 says June loads it.
- Then check how an item-granted buff (consumable, title, set option) reaches a unit **offline**. Run `grep UNHANDLED offline_packets.log` for `EGS_UPDATE_BUFF_INFO*` / `EGS_UPDATE_USER_UNIT_BUFF_INFO_NOT`.
- If the emulator must answer those packets, implement them in `X2Lib/Offline` under `SERV_IRUHADEV_OFFLINE`. Use real factor ordinals, never invented ones. Pull any stored procedure from the live DB as `CLAUDE.md` requires.

**Run with:** Opus 5.5, high effort. Review required.

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
```

## Phase 12 — Buff icons and UI assets

**Scope.**
- Adopt June's `DLG_UI_Common_Texture_Buff.tga` and `.tet` wholesale, from June `data215` into March `data057` (June asset wins).
- Add `DLG_UI_Common_Texture_Buff_INT.tga/.tet` if the `_INT` templets are adopted.
- Diff `DLG_BuffManager.lua` and `DLG_UI_Buff_*.lua` between June and March, and adopt June's.
- Check that every icon key the adopted templets use exists in the June `.tet`; 12 are missing even in June, so note them.

**Run with:** Sonnet 5.5, medium effort. Light review.

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "Investigation ledger" (Strings and icons) and
"Phase 12". Branch: mods/june-buff-port.

Your task (Phase 12, data only): adopt June's buff icon atlas
DLG_UI_Common_Texture_Buff.tga/.tet (June data215 -> March data057) wholesale, plus the
_INT atlas if _INT templets are in scope. Diff and adopt June's DLG_BuffManager.lua and
DLG_UI_Buff_*.lua. Check that every BUFF_ICON_TEXTURE_KEY in June's templets resolves
in the adopted .tet, and list the ones that don't (about 12 are missing even in June).
Stage into kom_contents/data057_kom + _manifest.json (and march2014/), then hand the
pack to me. Commit.
```

## Phase 13 — Adopt June buff data and ship

**Steps**
1. Compile June's `BattleFieldBuffTemplet.lua` and `BattleFieldBuffFactor.lua` (plus `_INT` per Phase 1) with mar-2014. Stage them in `march2014/data036/` and `kom_contents/data036_kom/`; they replace March's copies.
2. Rerun `buff_census.py`; it must report **0 unsupported**.
3. Run a local trial-load harness against the C++ parse expectations.
4. Build and deploy. Give the user one pack list naming every staged file across Phases 2, 10, 11, 12 and 13, together with this exe.
5. After the user packs, confirm with `elstools inspect` on the live koms. Then ask before launching. Confirm `ErrorLog.txt` shows the load summary with 0 placeholders and no `[JUNE_BUFF] unsupported` lines.

**Run with:** Sonnet 5.5, medium effort. The work is compile, stage and pack-list behind a hard gate: census = 0, otherwise stop and report. An Opus 5.5 review of the staged set is required before the pack hand-off.

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
```

## Phase 14 — Play-test sweep and close-out

**Steps**
1. Use the per-type "how to see it" lines written in Phases 5–9. Play-test one buff per group:
   - a stat buff and a debuff on the player, e.g. Silence `BTI_DEBUFF_SILENCE`;
   - a mob-applied debuff from a June-ported dungeon;
   - an immunity case;
   - each new finalizer;
   - one custom-function buff on an existing class.
2. Read `offline_server.log` and `ErrorLog.txt` for `[JUNE_BUFF]` lines.
3. Fix or file anything that fails.
4. Disable `SERV_IRUHADEV_JUNE_BUFF_DEBUG`.
5. Update `MODS.md`, `JUNE2017_CHARACTER_PORT_CHECKLIST.md` (buff blockers cleared) and the memory index.

**Run with:** Opus 5.5, high effort.

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
```

---

## Critical files

- `X2Lib/X2BuffFactor.h/.cpp`
- `X2Lib/X2BuffTemplet.cpp`
- `X2Lib/X2BuffBehaviorTemplet.h/.cpp`
- `X2Lib/X2BuffFinalizerTemplet.h/.cpp`
- `X2Lib/X2BuffDisplayerTemplet.cpp`
- `X2Lib/X2BuffTempletManager.cpp`
- `X2Lib/X2GameUnit.h/.cpp`
- `X2Lib/X2GUUser.cpp`
- `X2Lib/X2GUNPC.cpp`
- `X2Lib/X2UDPPacket.h/.cpp`
- `X2Lib/X2Main.cpp` (bindings)
- `X2Lib/X2PremiumBuffManager.cpp`
- `X2Lib/X2Game.cpp`
- `KTDXLIB/Always.h`
- `march2014/data036/Enum.src.lua`
- `kom_contents/data036_kom/` and `data057_kom/`
- `june2017_re/funcmap.tsv`
- `june2017_re/buff_census.py` (new)

## Verification (end to end)

1. `buff_census.py` goes to 0 unsupported, phase by phase.
2. Every build returns rc=0 with the PCH invalidated after header edits, and the deployed `X2_offline.exe` is confirmed by size and mtime.
3. After the user's pack, the live kom contents are confirmed with `elstools inspect`.
4. `ErrorLog.txt` shows the `[JUNE_BUFF]` load summary with 0 placeholders and no unsupported lines.
5. The Phase 14 play-test shows each group firing in `offline_server.log`.
