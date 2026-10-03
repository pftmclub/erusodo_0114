# June 2017 Buff/Debuff Port — phased plan

> Written 2026-10-02 and committed on `mods/lanox-port`. Start with Phase 0's kickoff
> prompt in a new conversation. Every kickoff prompt below points at this file.

## Context

The user wants **every June 2017 buff and debuff type**, for both characters and mobs, to work in the March 2014 client. This follows the standing goal of March→June engine parity ([[goal-march-client-runs-june-effects]]).

Today March can load June's enum names; Ranox/Phase 11 added about 160 of them. It cannot *run* most June buffs, for four reasons:

- **Enums.** June's `Enum.lua` has 705 more `BUFF_TEMPLET_ID`s, 734 more `BUFF_FACTOR_ID`s, 38 more `BUFF_BEHAVIOR_TYPE`s and 3 more `BUFF_FINALIZER_TYPE`s than March's. It also has 6 buff enum tables March lacks entirely.
- **Missing handlers.** The C++ factories have no case for 44 behavior/finalizer types: 40 BBT (73–112) and 4 BFT (14–17). June data uses 40 of them (37 BBT, 3 BFT). Three of the 44 are already in March's Lua Enum but have no C++: `BBT_INVINCIBLE`=77, `BBT_IMMUNE_NON_CONTROL_DEBUFF`=95, `BFT_AURA`=14. *(Corrected in Phase 0; this used to read "41 … five". See "Census baseline".)*
- **Unread keys.** About 40 data keys are never read by March. Examples: `IS_STORE_BUFF`, `BUFF_REPLACE_DETAIL_OPERATION`, `SKILL_CATEGORY_n`, `APPLY_CHECK_CONDITION`, `ACCUMULATION_DECREASE_*`.
- **One failure cascades.** March's `AddBuffFactor_LUA` (`X2Lib/X2BuffTempletManager.cpp:75`) only accepts factor N while exactly N-1 factors are already loaded. So one unparseable June factor silently drops **every later factor**. Each failure also raises a modal `DISPLAY_ERROR` popup, because `_SERVICE_` is not defined under `US_SERVICE`.

The intended outcome:
- June's own `BattleFieldBuffTemplet.lua`, `BattleFieldBuffFactor.lua`, `UnitApplyBuffGroup.lua` and `BuffTemplet.lua` load cleanly. They replace March's copies.
- Every June BBT, BFT and BDT runs on March classes, transcribed from the June binary.
- Buffs placed by mob scripts and damage data (239 June scripts carry inline `BUFF_FACTOR` blocks, 1485 blocks in all) work once those scripts are adopted.

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
- Templets: 929 entries, IDs 1..1333, unsorted; 404 absent IDs in 1..1333 forming 5 runs (269–286, 306–311, 550–556, 573–659, 1047–1332). They are stored in a map, which is fine. *(Phase 0 measured; the ledger said "22 gaps".)*
- June also defines 37 templets and 134 factors for IDs that are in March's enum but have no row in March's live bytecode. *(Phase 0 measured by name against the bytecode string constants; the ledger said 34 and 135 and the difference could not be reproduced. Immaterial: Phase 13 adopts June's files wholesale.)*

**June keys March never reads** (entries using them):
- Templet level: `IS_STORE_BUFF` 930, `BUFF_REPLACE_DETAIL_OPERATION` 21, `EXP_RATE` 12, `ACCUMULATION_DECREASE_WHEN_FINISH` 5, `ACCUMULATION_DECREASE_COUNT_WHEN_FINISH` 4, `ACCUMULATION_DELAY_TIME` 3, `IS_INSTANCE_KILL_DEBUFF` 3, `IS_PARTY_BUFF` 1, `NOT_APPLY_BUFF_WHEN_SAME_BUFF_OWNER` 1.
- Factor level: `APPLY_CHECK_CONDITION` / `AND_OPERATION` / `BUFF_APPLY_CHECK_CONDITION_TYPE` / `CONDITION_0..1` / `CHECK_BUFF_TEMPLET_ID` / `CHECK_HP_RATE_UP` / `HP_RATE` / `APPLY_WHEN_HAVE` (4), `ACCUMULATION_AT_ONE_GO` 2, `DISPLAYER` inside a factor 6.
- Behavior level: `SKILL_CATEGORY_0..4` 22/12/12/12/8, `MULTIPLY_DAMAGE_RATE` 20, `BUFF_VALUE_CALCULATE_TYPE` 10, `VALUE` 7, `SKILL_ID_LIST` 5, `IMMUNE_BUFF_TEMPLATE_ID` 5, `ONLY_MY_UNIT` 3, `NOT_DELETE_EFFECT_SET_WHEN_FINISH` 3, `SKILL_SLOT_TYPE` 3, `ADDITIONAL_ATTACK_TYPE` 3, `DROP_RATE` 3, `FORCE_SKILL_ID` 2, `MULTIPLY_MP_CHARGE_RATE` 2, `APPLY_TO_SPECIAL_ATTACK` 2, `LUCKYTIMINGEVENT_*`.
- Once each: `FROZEN_TIME`, `IGNORE_DEFENCE_RATE`, `HYPER_COUNT`, `ALL_DEBUFF`, `IGNORE_SKILL_ID`, `OFFSET_POSITION`, `DELETE_BUFF_WHEN_DIE`, `FORCE_SET`, `USE_CREATOR_UNIT_DATA`.
- Finalizer level: `FROZEN_TIME` 1, `SHAKE_DECREASE_MAX_TIME` 1, `SHAKE_DECREASE_TIME_AT_ONCE` 1, all under the existing `BFT_TIME` (Phase 4b, not Phase 8: the only `SHAKE_DECREASE_*` entry is `BTI_AMUSEMENT_ITEM_WATER_BALLOON_IN_DAMAGE`; `BFT_STICK_SHAKE`'s own sub-table carries only `BUFF_USE_COUNT_TYPE` and `COUNT`), plus `SKILL_SLOT_TYPE` 1 under `BFT_USE_SKILL_BY_SKILL_SLOTTYPE` (Phase 8).
- Displayer level: per-class keys `ADD`/`AIN`/`LUCIEL`/`ROSE` (2 each, under `BDT_EFFECT_SET_ONLY_MY_CLASS`), `NOT_DELETE_EFFECT_SET_WHEN_FINISH` 3, `ONLY_MY_UNIT` 3. *(`REPLAY_EFFECT_SET_ACCUMULATION_LIMIT1` was listed here with 18 uses; March does read it, `X2BuffDisplayerTemplet.cpp:305`, under `BDT_EFFECT_SET`. Removed in Phase 0.)*
- Phase 0 additions to the lists above: factor-level `APPLIED_SKILL_ID` 2 (inline blocks in `DamageEffect`), behavior-level `CAN_DOWN` 1 (`BBT_NEVER_MOVE`). `OFFSET_POSITION` is 2 (1 templet, 1 inline), not 1. `DROP_RATE` 3, `VP_RATE` 1 and `LUCKYTIMINGEVENT_*` are templet-level keys, not behavior-level. The factor-level `APPLY_CHECK_CONDITION` group is 4 entries, and `APPLY_WHEN_HAVE` itself is 2.
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
- 239 June scripts carry inline `BUFF_FACTOR = {` blocks (1485 blocks: `data036_kom` 954, `data037_kom` 391, `datai016_kom` 140). A grep finds 244 files; 5 of them are the buff data files themselves. These go through the same factor parser, with `bIsSkill=false` (`X2DamageManager.cpp:3830`).
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
- There are three new keys against March's `BuffTemplet.lua`: `NOT_USE_IN_PVE` (62 entries), `NOT_USE_IN_HUGE_BOSS_RAID` (6) and `NOT_USE_IN_HENIR` (6). *(The ledger listed only the first; corrected in Phase 0.)* March's C++ reads only `m_eBuffID`, `m_strBuffName`, `m_fBuffPeriodSecond`, `USING_BUFF_ROOM_TYPE` and `SOCKET_OPTION`, so `BUFF_FACTOR` and the `m_bEndBuffAt*` keys already in March's data are never read by March either.
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

### Phase 0 — Done (2026-10-02)

- **Branch.** `mods/lanox-port` was clean. `mods/june-buff-port` already existed at the same commit (`198fbb8`, the commit that added this doc), so no new branch was needed. Nothing was uncommitted.
- **Tool.** `june2017_re/buff_census.py` plus `june2017_re/buff_census_flags.tsv`.
  - The TSV caches the `US_SERVICE` flag states that `flagprobe.py` resolved for the conditionals in `X2Buff*.cpp/.h`, `X2GameUnit.cpp`, `X2PremiumBuffManager.cpp` and the binding-check sources (700 studio flags).
  - **`SERV_IRUHADEV_*` flags are never cached.** They change as the port proceeds, so a flag probed OFF before its `#define` lands would stay OFF and a finished phase would never clear. Every run reads them fresh from the `#define` lines in `KTDXLIB/Always.h` (our house rule makes them plain unconditional defines), and `--reprobe` asks the compiler instead.
  - A studio flag that is missing from the cache is probed through `cl /EP` automatically (about 45 s). `--reprobe` re-resolves every flag, ours included; `--no-probe` never runs the probe.
  - A full run with a warm cache takes about 12 s.
- **Usage.** `python june2017_re/buff_census.py [--phase 2|3|4a|4b|5|5b|6|7|8|9|10|11] [-v] [--json FILE] [--no-probe] [--reprobe]`. `--phase 5..8` also lists every key the data puts under that phase's types (the transcription checklist).
- **March data.** March's live buff Lua exists only as bytecode, so "March supports X" always means the C++. The script also reads the bytecode string constants from `kom_contents/data036_kom/` (game directory, outside the repo; override with `BUFF_CENSUS_KOM_CONTENTS`) for the `in-March-data` column. Without that directory the column prints `?`.
- **Method.**
  - The CASE lists are resolved with the probed flags, so `#ifdef`'d cases count only if the flag is on.
  - Hand-written `case BBT_EMPTY:` labels count as handled. Missing them made `BBT_EMPTY` look unsupported.
  - Per-type key readers are the quoted literals in the type's factor-parse function (a `CX2BuffFactor` member) and in the templet class's `ParsingBehavior/Finalizer/Displayer`, following the calls they make. `Q::f(` resolves in `Q`, a plain `f(` in the caller's class and its base classes, and `obj.f(` / `p->f(` is skipped, so one class cannot inherit another class's literals. Keys in templet entries are checked against the templet reader and keys in factor or inline entries against the factor reader. This is a literal match, not a data-flow proof, so spot-check a key in Ghidra or source before acting on it.
  - A key literal containing `%d` (for example `"SKILL_CATEGORY_%d"`) counts as a pattern for `SKILL_CATEGORY_<digits>`.
  - Templet-level and factor-level supported keys come from `CX2BuffTemplet::ParsingScript` and `CX2BuffFactor::ParsingBuffFactor` plus the helpers they call, so a ported version that reads keys in helpers is recognised.
  - Each unread behavior/finalizer/displayer key carries an `owner`: the phase of the type it sits under (4b if March already handles the type, 5–8 if not). `--phase 4b` therefore never lists a key that belongs to a Phase 5–8 type, and `--phase 6` lists `SKILL_CATEGORY_n`.
  - The Lua-binding "present/ABSENT" check scans the preprocessed March sources, so a binding added under an OFF flag does not count.
- **Phase grouping in the report.** Types map to Phases 5–8 exactly as listed under "Phases 5–7" and "Phase 8". If Phase 1 re-splits them, edit `PHASE_BBT_VALUES` and `PHASE_BFT_VALUES` at the top of the script.
- **Cost.** Not recorded for this session (no `/cost` figure was available).

### Census baseline (2026-10-02, before any port work)

**Reproduced exactly:**
- Enum diff, every table (BTI 1065/360/705, BFI 1236/503/734, BBT 113/75/38, BFT 18/15/3, BDT 14/14/0), the 6 June-only tables, and all 5 value changes.
- The 5-row C++ drift table.
- Data counts: 1130 factors contiguous, 69 `_INT` factors (10001–10069), 929 templets, 72 `_INT` templets (10001–10072), 138 + 71 premium entries, 248 `IMMUNITY_LIST_AT_THIS_STATE` uses in 60 files.
- Per-type templet/factor counts in "New type usage" (every row).
- The 17 unhandled custom-function BTIs.
- The unread-key counts: `IS_STORE_BUFF` 930, `BUFF_REPLACE_DETAIL_OPERATION` 21, `EXP_RATE` 12, `ACCUMULATION_*`, `SKILL_CATEGORY_0..4` 22/12/12/12/8, `MULTIPLY_DAMAGE_RATE` 20, `VALUE` 7, `SKILL_ID_LIST` 5, and the rest of the ledger list.

**Found wrong in the ledger (corrected above in place):**

| Ledger said | Census measured |
|---|---|
| 41 unhandled types that June data uses; five of them already in March's Lua | 44 unhandled in C++ (BBT 73–112 = 40, BFT 14–17 = 4); June data uses 40 of them (37 BBT + 3 BFT). Only 3 (77, 95, `BFT_AURA`) are in March's Lua; `BFT_STICK_SHAKE`/`USE_SKILL_BY_SKILL_SLOTTYPE` are new in June. Unused by any data: 99, 111, 112 and `BFT_ATTACK` |
| 241 scripts with inline blocks | 239 scripts, 1485 blocks. The grep's 244 includes the 5 buff data files |
| `REPLAY_EFFECT_SET_ACCUMULATION_LIMIT1` unread (18) | March reads it (`X2BuffDisplayerTemplet.cpp:305`, `BDT_EFFECT_SET`) |
| Premium: only `NOT_USE_IN_PVE` is new | also `NOT_USE_IN_HUGE_BOSS_RAID` (6) and `NOT_USE_IN_HENIR` (6) |
| 22 template ID gaps | 404 absent IDs in 5 runs |
| 34 templets / 135 factors with a March enum ID but no March row | 37 / 134 (by name against bytecode constants) |
| `OFFSET_POSITION` once; `DROP_RATE`/`LUCKYTIMINGEVENT_*` behavior-level | 2 uses; they are templet-level. New: `APPLIED_SKILL_ID` 2 (factor-level, inline) and `CAN_DOWN` 1 |

**Headline numbers for the exit metric** (`python june2017_re/buff_census.py`):

| Item | Baseline |
|---|---|
| New BBT / BFT in June's enum | 38 / 3 |
| C++ lacks BBT / BFT | 40 (73–112) / 4 (14–17); 3 of these are in March's Lua already |
| Of those, used by June data | 37 BBT / 3 BFT (types assigned per phase: 5 = 20, 6 = 8, 7 = 12, 8 = 4) |
| BDT unhandled | 0 |
| C++ drift vs June | 1 BTI value, 2 BTI renames, 1 BFI value, 1 BFI name (`TELEPORT_CONSUME_MP`) |
| Level-wide unread keys | templet 17, factor 4, behavior 24, finalizer 4, displayer 6 (owner split: 4a = 21; 4b = 11 behavior + 3 finalizer + 6 displayer; the rest sit under Phase 5–8 types) |
| Existing types with keys their parse never reads | 41 type/key pairs (326 entry-uses); phase 4b scope |
| `BBT_CUSTOM_FUNCTION` templets / unhandled | 20 / 17 |
| June-only bindings March lacks | 9 of 9 (`AddBuffGroup_LUA`, `AddOfficiallyBuffList_LUA`, `SetMonsterBuffGroupID`, `DeleteNonControlDebuff`, `GetBuffAccumulationCountNow`, `ApplyBuffFactor`, `AddRandomBuffList`, `SetPveBalanceBuffGiveScore_LUA`, `SendRandomMissionRewardBuffCheat`) |

Run the script with no arguments for the full report, including the per-type table and the sub-keys of the unread factor-level groups.

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

### Phase 1 — Done, part 1a (2026-10-02): loaders, factories, settled answers

Method notes for anyone re-checking:
- **Ghidra has no RTTI namespaces for this binary.** The class vtables were recovered by scanning the PE: RTTI TypeDescriptor (`.?AVCX2Buff…@@`) → CompleteObjectLocator → vftable. `.text` is merged with read-only data here (`0x31aa1000`–`0x337ba000`), so a vtable ends at the next COL pointer, not at the first non-`.text` pointer.
- Each factory case was mapped by its create function, the vtable that function stores, and that vtable's RTTI name. Every row below is that chain; nothing is inferred from names alone.
- Ghidra was used read-only (no renames or comments).

**Settled questions.**

| Question | June answer | Evidence |
|---|---|---|
| Does June keep `AddBuffFactor_LUA`'s ordinal rule? | **Yes, unchanged for ids < 10000.** A factor is pushed only when `vec.size() == id-1`, and one that fails to parse is simply not pushed, so **the cascade is kept too.** June only drops the popup: errors are formatted and discarded (no log either). | `326c4050` |
| How are `_INT` ids 10001+ stored? | **In a second vector** (manager `+0x20`; the main vector is `+0x10`) with its own ordinal rule, `size == id-10001`. The gate runs before the main check, and it returns "handled" for **every** id ≥ 10000, even when that ordinal check fails, so a bad `_INT` id is dropped silently. Lookup: `GetBuffFactorPtr` sends id > 10000 to `vecINT[id-10001]`. `_INT` **templets** need nothing special: they go into the same map as the rest. | `326c3f30`, `326c32a0`, `326c3990` |
| When are `_INT` files loaded? | **Always, right after the main file, in the same function.** `OpenScriptFileForTemplet` loads `BattleFieldBuffTemplet.lua` then `BattleFieldBuffTemplet_INT.lua`; `OpenScriptFileForFactor` does the same for the factor pair. A failed main load logs `0x2712` and falls through to the `_INT` load. So Phases 3, 11 and 13 include `_INT`. | `326c0020`, `326c13a0` |
| `BBT_MP_NATURAL_RECOVERY_IMPOSSIBLE` (111), `BBT_REFLECT_PHYSIC` (112), `BFT_ATTACK` (17) | **Not implemented in June's client.** The templet factories stop at BBT 110 (`(type-1) > 0x6d` → default) and BFT 16, and so do both factor-parse switches. An entry using them fails to parse in June too. Port the enum value (Phase 2) and nothing else; the no-partial rule is satisfied. | `32e80450` @`32e804da`, `32ec36f0`, `32d4bbf0`, `32d4c750` |
| Does June load premium `BuffTemplet_INT.lua`? | **Yes, unconditionally.** The bulk loader `3258c470` hands `BuffTemplet.lua` (`@3258e8cb`) and then `BuffTemplet_INT.lua` (`@3258ea8c`) to the same per-file loader `326c6e20`. The only branch between them is the XOR-init guard. So Phase 11 includes the 71 `_INT` premium entries. | `3258c470` `@3258ea0f`/`@3258ec1f` |
| `BBT_CONSUME_MP_RATE_BY_SKILLTYPE` (99) | **Implemented** (class `CX2BuffChangeConsumeMpRateBySkillType`, parse `32d4aa30`), just unused by June data. It is ported. | `32e87be0` |
| `FUN_3227baf0` | **`CX2GameUnit::HaveThisBuff(BUFF_TEMPLET_ID)`**: it is the target bound under the Lua name `HaveThisBuff`. So `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID`'s second gate is "the unit currently has `…_RELATIVE_BUFF_TEMPLATE_ID`". March has `CX2GameUnit::HaveThisBuff`; nothing new is needed. | bind @`31debe27` |
| Factory `default:` behavior | Unknown types return false with no popup; `BBT_EMPTY` returns true. The whole templet then fails `ParsingScript` and is not inserted. | `32e80450` tail |

**Templet-side factories (type → June class).** Most types reuse an existing March class, so the "new class" column is the real work list.

| Type | June class (create fn) | New class? |
|---|---|---|
| 73 IGNORE_STATE_GUARD | `CX2BuffIgnoreStateGuardBehaviorTemplet` (`32e86b70`) | yes |
| 74 DETONATION_IMPOSSIBLE, 77 INVINCIBLE, 101 IGNORE_HP_DECREASE, 102 DETONATION_IMPOSSIBLE_GAGE_RESET, 104 JUMP_IMPOSSIBLE, 110 AWAKE_IMPOSSIBLE | `CX2BuffIdentityBehaviorTemplet` (`32e866b0`) | no |
| 75 DOUBLE_ATTACK_BUFF | `CX2BuffDoubleAttackBuffBehaviorTemplet` (`32e86bb0`) | yes |
| 76, 78, 85, 86, 87, 92, 96, 106, 107, 108, 109 | `CX2BuffChangeStatBehaviorTemplet` (`32e866f0`) | no |
| 79 CHANGE_NOW_FORCE_DOWN_PER_SECOND | `CX2BuffChangeNowForceDownPerSecondBehaviorTemplet` (`32e86e90`) | yes |
| 80–84 *_BY_CONDITION | `CX2BuffChangeStateByConditionBehaviorTemplet` (`32e86c00`) | yes |
| 88 IGNORE_DEFENCE | `CX2BuffIgnoreDefenceBehaviorTemplet` (`32e895a0` → ctor `32e890b0`) | yes |
| 89 CHANGE_SKILL_DAMAGE | `CX2BuffChangeSkillDamageBehaviorTemplet` (`32e87970`) | yes |
| 90 CHANGE_SKILL_DAMAGE_BY_SKILL_ID | `CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet` (`32e89830`) | yes |
| 91 CHARGE_MP_HIT_BY_SKILL_ID | `CX2BuffChargeMpHitBySkillIDBehaviorTemplet` (`32e898a0`) | yes |
| 93 CHANGE_EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER | `CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner` (`32e86740`) | yes (subclass of ChangeStat) |
| 94 IMMUNE_BUFF_DEBUFF | `CX2BuffImmuneBuffDebuffBehaviorTemplet` (`32e87510` → ctor `32e871d0`) | yes |
| 95 IMMUNE_NON_CONTROL_DEBUFF | `CX2BuffImmuneNonControlDebuffBehaviorTemplet` (`32e86c50`) | yes |
| 97 CHANGE_MAX_CHARACTER_ADD_RESOURCE | `CX2BuffChangeCharacterAddResourceBehaviorTemplet` (`32e86c90`) | yes |
| 98 CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE | `CX2AddDamageOfSkillSlotType` (`32e86cd0`, vt `33535160`) | yes |
| 99 CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE | `CX2BuffChangeConsumeMpRateBySkillType` (`32e87be0`) | yes |
| 100 CHANGE_COOLTIME_RATE_BY_SKILLTYPE | `CX2BuffChangeCooltimeRateBySkillType` (`32e87c30`) | yes |
| 103 CHANGE_SKILL_COOL_TIME_SPEED | `CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet` (`32e87d10`) | yes |
| 105 IMMUNE_EDT | `CX2BuffImmuneEDTBehaviorTemplet` (`32e87550`) | yes |
| BFT 14 AURA | `CX2BuffAuraFinalizerTemplet` (`32ec6260`) | yes |
| BFT 15 STICK_SHAKE | `CX2BuffStickShakeCountFinalizerTemplet` (`32ec5e10`) | yes |
| BFT 16 USE_SKILL_BY_SKILL_SLOTTYPE | `CX2BuffUseSkillBySkillSlotTypeFinalizerTemplet` (`32ec5e60`) | yes |

The displayer factory (`32ebb530`, BDT 1–13) uses the same classes as March. One existing-type change belongs to Phase 4b: June has no `CX2BuffChangeMpHitBehaviorTemplet` or `CX2BuffChangeMpHittedBehaviorTemplet` (March has them commented out anyway).

**Factor-parse functions (type → June parse function).** `CX2BuffFactor::ParsingCombinationBehavior` is `32d4bbf0`, and `ParsingCombinationFinalizer` is `32d4c750`.
- Empty parse `32d45b60` (no factor values): 73, 74, 77, 95, **97**, 101, 102, 104, 110.
- `ParsingChangeStatBehavior` `32d445d0`: 76, 78, 85, 86, 87, 92, 93, 96, 106–109.
- Own parser: 75 `32d46800` (shared with 56 DOUBLE_ATTACK), 79 `32d42650`, 80–84 `32d42910`, 88 `32d49210`, 89 `32d49460`, 90 `32d49810`, 91 `32d49da0`, 94 `32d4a770`, 98 `32d4a370`, 99 `32d4aa30`, 100 `32d4b020`, 103 `32d4b610`, 105 `32d45c50` (**shared with 43 CUSTOM_FUNCTION**).
- Finalizers: 14 AURA uses the empty finalizer parse `32d44230`, 15 `32d44a80`, 16 `32d44d60`.

### June buff function map

Every row is confirmed in `june2017_re/funcmap.tsv` unless marked *candidate*. "Phase" is the phase that ports or uses it.

**How to read June's stores (method, so later phases can extend the map).**
- A `CX2AdditionalStatByBuff` member is `0xb4` bytes. Its result is a protected float at member `+0xa0`, read as `[M+0xa4] ^ [M+0xa8] ^ (base+M+0xa0)`. Readers reach it by displacement **or** by `ADD reg, M+0xa0` on a computed pointer, and often through `EBP`. A search that drops `EBP` operands or immediates misses most readers. That happened once in this phase and was corrected.
- `DamageData+0xc` (`optrAttackerGameUnit`) points at the unit's `+0x1a0` sub-object, so readers subtract `0x1a0` first.
- June's decompiler output hides the `ECX` of `vector::push_back` (`31fac820`). It mislabelled a constant push at least once (BBT 98, below). Parse layouts marked *(asm)* were checked in the disassembly.

**Core functions.**

| June | March symbol | Phase |
|---|---|---|
| `326c0020` / `326c13a0` | `CX2BuffTempletManager::OpenScriptFileForTemplet` / `…ForFactor` (both load `_INT` too) | 3, 13 |
| `326c26f0` | none: `UnitApplyBuffGroup.lua` loader | 10 |
| `326c3990` / `326c4050` | `AddBuffTemplet_LUA` / `AddBuffFactor_LUA` | 3 |
| `326c3f30` | none: `_INT` factor branch (second vector, base 10001) | 3 |
| `326c32a0` | `GetBuffFactorPtr` (`_INT` aware) | 3 |
| `3236a9b0` | `CX2BuffTemplet::ParsingScript` | 4a |
| `32d4d850` | `CX2BuffFactor::ParsingBuffFactor` | 4a |
| `32d4bbf0` / `32d4c750` | `CX2BuffFactor::ParsingCombinationBehavior` / `…Finalizer` | 4b–8 |
| `32d41a80` | `CX2BuffFactor::ParsingTableForWhile` (unchanged: `[count?, v1..vn]`, count only when `bIsSkill`) | – |
| `32e80450` / `32ec36f0` / `32ebb530` | `ParsingCombinationBehavior` / `…Finalizer` / `…Displayer` (templet factories) | 4b–8 |
| `320844a0` | levelled `BUFF_FACTOR` copy ctor (`X2BuffFactor.h:509`) | – |
| `32d47620` | `BUFF_FACTOR::GetBehaviorFactorToLevel` (switch `type-1`, 1..105) | 4b–7 |
| `32d4cb00` | `BUFF_FACTOR::GetFinalizerFactorToLevel` (1..16) | 4b, 8 |
| `32d4d670` | `CX2BuffFactor::SetGameUnitBuffFactor` with `SetGameUnitBehavior` inlined (1..93) | 4b, 5 |
| `32282700` | `CX2GameUnit::InsertAdditionalStat` (switch `type-6`) | 5 |
| `322830f0` | `CX2GameUnit::EraseAdditionalStat` | 5 |
| `3226fd70` | update-or-insert additional stat (March's `UpdateAddtionalStatFactor` path) | 5 |
| `32271540` | `InitAdditionalStat` (basic values) | 5 |
| `32270dd0` | `CX2GameUnit::InitAdditionalStatOptionValue` (`X2GameUnit.cpp:12442`) | 5 |
| `322718e0` | `InitAdditionalPassiveMultipleAndSum` | 5 |
| `3227ad60` | `CX2GameUnit::CreateAndInsertBuffIdentity` (switch `type-4`) | 7 |
| `32279d70` | identity erase (the identity templet's `DoFinish`) | 7 |
| `3236d110` / `3236d350` | `CX2AdditionalStatByBuff::CreateAndInsertStatFactor` / result update | – |
| `32284aa0` | `CX2GameUnit::GetFinalDamage` (vtable slot 28 in all unit classes) | 5 |
| `322990e0` | `CX2GameUnit::SetDamageExceptionProcess` (`CX2GameUnit` vtable slot 174) | 5 |
| `320462a0` / `327b1f30` | `CX2GUUser::DamageReact` / `CX2GUNPC::DamageReact` (slot 24; base slots 22–25 are pure: `GetSkinAnimPtr`, `StateChange`, `DamageReact`, `AttackResult`) | 5 |
| `31fd1c00` | `CX2GUUser` vtable slot 45: hyper-mode duration getter (`32a4fe50` is Ain's override) | 5 |
| `322788c0` | none: June damage-variance roll, called twice from `GetFinalDamage` (`@32284c1f`, `@32284c2f`) | 5 (see 92) |
| `3227baf0` / `3227bbc0` | `CX2GameUnit::HaveThisBuff` / June-only `GetBuffAccumulationCountNow` | 10 |
| `3229dcc0` | June-only `CX2GameUnit::DeleteNonControlDebuff` (also called when BBT 95 is applied) | 7, 10 |

**Behavior templet vtable, June (14 slots; March has 8).** `[0]` dtor, `[1]` `OnFrameMove(unit, templet, dt)`, `[2]` `SetFactor`, `[3]` new `bool(4 args)` (default true; overridden by ChangeStat, EnchantAttackRate, NowMp), `[4]` `GetClonePtr`, `[5]` `GetFactor`, `[6]` `DoFinish`, `[7]` new `bool(2 args)` (default false), `[8]` anti-tamper checksum (not ported), `[9]`–`[11]` new `void(1 arg)`, `[12]` `ParsingBehavior`, `[13]` `SetFactorFromPacket`. ChangeStat adds `[14]` `GetBuffOwnerUID` and `[15]` `SetBuffOwnerUID`, set by the `_HaveBuffOwner` subclass. Slots `[3]`, `[7]`, `[9]`–`[11]` are Phase 4b's to name.

**June adds `ATTACK_TYPE` `AT_COMMAND = 2`** (`Enum.src.lua:490`; March has only `AT_NORMAL`/`AT_SPECIAL`). BBT 85, 86, 87 and 96 only act when `DamageData.attackType == AT_COMMAND`. Phase 2 must add the Lua name and the C++ enumerator; `SetDamageDataFromLUA` already reads `ATTACK_TYPE` (`X2DamageManager.cpp:137`).

### Per-type map (Phase 1b, 2026-10-02)

Abbreviations: **parse** = factor-parse function and float order (the wire layout); **lvl** = `GetBehaviorFactorToLevel` case; **SGU** = `SetGameUnitBehavior` case; **store** = unit member (offset in June's `CX2GameUnit`, not to be ported as a number); **reads** = every June reader.

**Phase 5 — ChangeStat-class types. Transcription-ready.** All share March's `ParsingChangeStatBehavior` layout (`32d445d0`) and class `CX2BuffChangeStatBehaviorTemplet`. Each needs: a parse case in both macro lists; the lvl ChangeStat group (`32d47957`) for 76/78/92/93 or the default slice (`32d4812a`) for the rest; the SGU ChangeStat group (`32d4d6ef`) for 76/78/92/93; an `InsertAdditionalStat`/`EraseAdditionalStat` case on a **new** `CX2AdditionalStatByBuff` member; `InitAdditionalStat` (basic value) and `InitAdditionalPassiveMultipleAndSum` (multiple 1, sum 0); and `Verify()` plus the update-all loop like its siblings. **No SGU case and no ChangeStat lvl case for 85, 86, 87, 96, 108 or 109**: June leaves them out of both switches, so they take the default level slice (March's default branch) even though their data carries `BUFF_RELATION_TYPE`. Only 76 joins both ChangeStat groups. Neither `SetFactor` nor slot `[3]` of ChangeStat compares against any new type value (checked: `32e78410`, `32e786d0`, `32e73500`, `32e73640`, `32e764a0`).

| Type | New member, basic value | Insert case | June reads |
|---|---|---|---|
| 76 CHANGE_HYPER_DURATION | `m_AdditionalHyperDuration`, 0.0 | `32282ddc` | **Option value:** `InitAdditionalStatOptionValue` (`32270dd0`, March `X2GameUnit.cpp:12442`) seeds it from `m_SocketData.m_fHyperModeTime` (`@322710a1`, beside HyperCharge's `m_fHyperModeChargeRate` `@32271089`), and sets 0 in the reset branch (`@3227143d`). **Reader:** `CX2GUUser::GetHyperModeTime` (`31fd1c00`, `CX2GUUser` slot 45; March `X2GUUser.cpp:23448`): June replaces `m_SocketData.m_fHyperModeTime` with this member's result, i.e. `fTotal += fTotal * m_AdditionalHyperDuration.GetResultValue() * 0.01f`. Class overrides read it too: Ain `32a4fe50`, and `329e5180`/`329e56c0` (a class with members at `+0x96xx`, not yet attributed). Port those with their classes. |
| 85 RANGE_COMMAND_MP_GAIN_RATE / 86 MELEE_COMMAND_MP_GAIN_RATE | two new members, 1.0 | `32282ee6` / `32282ec3` | `SetDamageExceptionProcess`, after `fHitAddMP = attacker->GetChargeMpHit{Melee,Range}Attack()` (March `X2GameUnit.cpp:14259-14269`): `if ( attackType == AT_COMMAND ) fHitAddMP *= attacker->{86 melee @3229935a, 85 range @3229947c}`. The Chung virtual `32c16be0` also sets both members' passive values (port with Chung's passive code, not here). |
| 87 MELEE_COMMAND_DAMAGE_RATE / 96 RANGE_COMMAND_DAMAGE_RATE | two new members, 1.0 | `32282f09` / `32282f2c` | `GetFinalDamage` main path `@32286270`–`322862b9`, after the final-defence/final-damage adds: `if ( attackType == AT_COMMAND ) dmg *= m_bMeleeAttack ? attacker->87 : attacker->96`. |
| 108 FINAL_DAMAGE_MULTIPLIER_NPC / 109 …_USER | two new members, 1.0 | `32282f72` / `32282f95` | `GetFinalDamage`, both paths: fixed-damage path `@32285fba`–`32285fec` and main path `@3228620a`–`32286252`. `inc2 = dmg * ((defender->GetGameUnitType() == GUT_USER ? attacker->109 : attacker->108) - 1)`, added beside March's `fIncreaseDamage` (`X2GameUnit.cpp:2613-2616`, `2704-2707`). The unit-type field is `+0x264`. |

**Phase 5 types that are not simple transcription.** Their store and readers are found, but each depends on a June mechanism or field March lacks.
*1c item 3 settles every type in this list; its readiness summary supersedes the verdicts below.*
- **78 TYPE_ADDITIONAL_ATTACK_MULTIPLIER → Opus-only (reader context).** The store is ready: a new member, basic 1.0, insert `32282dff`. The readers sit in `CX2GUUser::DamageReact` `@320494a0`–`32049505` (`EBP` = this, `EBX` = DamageData). The value read is `ESI->32071b10 + this->78 (getter 32071b50) + this->32071af0`, minus `EDI->32071b90 * k`. `ESI` and `EDI` are other units: `EDI` is the unit at June's `DamageData+0x470`. The other getters read members that buffs do not insert into (`+0x51cc`, `+0x5118`, `+0x5a4c`). `CX2GUNPC::DamageReact` reads it at `@327b42bf`–`327b4340`. Before Sonnet can transcribe this, an Opus pass must map `DamageData+0x470` and those three members to March.
- **106 / 107 FORCE_DOWN_MULTIPLIER_MELEE / RANGE_ATTACKED → Opus-only (reader context).** The store is ready: two new members, basic 0.0, insert `32282bf2`/`32282c15`. Reader: `CX2GUUser::DamageReact` `@3204a123`–`3204a243`. It computes `reduce = DD[+0x514]; if ( unit at DD+0x470 is valid ) reduce -= ( DD.m_bMeleeAttack ? unit->106 : unit->107 ); fForceDown(DD[+0x3f0]) -= reduce`, then clamps when `DD[+0x4c5]`. The getters are `32071bb0`/`32071bd0`. `DD+0x470`/`+0x514`/`+0x4c5` are June DamageData fields that are not yet mapped to March.
- **92 CHANGE_MAXIMIZE → Opus-only.** It is stored like a ChangeStat member (`32282f4f`, basic 0.0). Its reader `322788c0` is June's **damage-variance roll**. Nothing like it was found in March's `GetFinalDamage`; verify that before porting. `GetFinalDamage` calls it for physic (`param 0`) and magic (`param 1`). It takes a ± range from a **second** physic/magic member (`+0x44ec`/`+0x45a0`, which plain BBT 7/8 and 80/81 also insert into) and draws a random value whose upper bias grows with MAXIMIZE. Porting 92 means porting that variance system and the second members first (Phase 4b for 7/8). Further readers are `3230c1a0` and `32763c10` (`ApplyPassiveSkillOwnerUser_LUA`).
- **93 CHANGE_EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER → Opus-only.** It inserts into the same six attribute-defence members as 42 (`32282d0a`), so it needs no new reader. The difference lives in its class, `_HaveBuffOwner` (`SetFactor` `32e78850`, slot `[3]` `32e786d0`, `[14]`/`[15]` owner UID), which is not yet decoded.
- **80–84 *_BY_CONDITION → Opus-only (needs class RE).** Parse `32d42910`: `[BUFF_CHANGE_TYPE, BUFF_VALUE_CALCULATE_TYPE, count, v1..vn]`. lvl `32d48063` keeps the whole table. SGU has its own case (`32d4d70b`). Stores are existing members: 80 → PhysicAttack and its second member, 81 → MagicAttack and its second member, 82 → PhysicDefence, 83 → MagicDefence, 84 → MaxHp plus `SetMaxHp`. Class `CX2BuffChangeStateByConditionBehaviorTemplet`: `SetFactor` `32e78860` (220 instructions; picks the value by condition through `32e77f40` and calls `3226fd70`/`32282700`), `DoFinish` `32e75720`, packet `32e77ef0`. The condition rule (`BVC_RALLY_OF_HERO`) is not yet decoded.
- **79 CHANGE_NOW_FORCE_DOWN_PER_SECOND → Opus-only (needs class RE).** Parse `32d42650`: `[BUFF_CHANGE_TYPE, then FIX_VALUE table only if the type is 1]`. A non-1 change type pushes **no** factor. lvl `32d478aa`: `[type, count<2 ? v2 : v[level+1]]`. Class: `OnFrameMove` `32e756f0`, `SetFactor` `32e855f0`, `Parsing` `32e755a0`, packet `32e85790`. The tick logic is not yet decoded.
- **97 CHANGE_MAX_CHARACTER_ADD_RESOURCE → move to Phase 7.** Empty factor parse. Its class only overrides `SetFactor` (`32e75c70`), which calls unit virtual `+0x324` (`CX2GameUnit` slot 201) with `1.0f`, and has no finish or packet override. The base slot is empty; the overrides are `CX2GUAdd` `329e31e0`, `CX2GURose` `32ac5e20`, `CX2GULuciel` `32b14cd0`, `CX2GUAra` `32baafc0` and `CX2GUChung` `32bf4880`. This is a per-class resource mechanism.
- **88 IGNORE_DEFENCE → stays Phase 5, Opus-only until its store is read.** Parse `32d49210`: `[IGNORE_DEFENCE_RATE]`, a float defaulting to 1.0. lvl is copy-as-is (`32d47a96`). Class: `SetFactor` `32e7d260` and packet `32e7d3f0` call unit `32282390`; `DoFinish` `32e75b00` calls unit `3227a8c0`. The member and its readers are not yet found.

**Phase 6 — skill-scoped types. Parse layouts done; class and store need Phase 1b.** Every class gates on `unit+0x264` (user only) and calls `CX2GUUser`/skill code. None meets the bar yet, so **all are Opus-only until 1b finishes them.**
- **75 DOUBLE_ATTACK_BUFF.** Parse `32d46800` (shared with 56): `[BUFF_CHANGE_TYPE, PERCENT table only if the type is 3]`. lvl default. Class: `SetFactor` `32e77be0` (calls `32281b00`), `DoFinish` `32e75540` (`32279d20`), packet `32e754f0`, `Parsing` `32e75360`.
- **89 CHANGE_SKILL_DAMAGE.** Parse `32d49460`: for each `SKILL_CATEGORY_%d` table, `[SKILL_TYPE, count, rate1..rateN]` from `MULTIPLY_DAMAGE_RATE`; it stops at the first missing or zero `SKILL_TYPE`. lvl `32d47a9e`: per block `[SKILL_TYPE, rate[min(level,count)]]`. Class: `SetFactor` `32e7a9e0`, `[7]` `32e7c020`, `DoFinish` `32e75b70` (`31fe2990`), packet `32e7af60`.
- **90 CHANGE_SKILL_DAMAGE_BY_SKILL_ID / 91 CHARGE_MP_HIT_BY_SKILL_ID.** Parse `32d49810` / `32d49da0`: for each `SKILL_CATEGORY_%d`, `[rate, skillId1..skillIdN, -1.0]`. The rate is `MULTIPLY_DAMAGE_RATE` / `MULTIPLY_MP_CHARGE_RATE`; IDs come from `SKILL_ID_LIST`; an entry is skipped if the rate is ≤ 0 (90) or < 0 (91), or the list is empty. lvl copy-as-is. Class `SetFactor` `32e849c0` / `32e84d70`, `DoFinish` `32e75b90` (`31fe29e0`) / `32e75bb0` (`31fe2a30`).
- **98 CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE.** Parse `32d4a370` *(asm)*: `[SKILL_SLOT_TYPE, MULTIPLY_DAMAGE_RATE, -1.0]`. lvl `32d47e66`: `[slotType, rate]`. Class `CX2AddDamageOfSkillSlotType`: `SetFactor` `32e78080`, `DoFinish` `32e75cc0` (`31fe2990`, shared with 89), packet `32e78170`, `Parsing` (slot 12).
- **99 / 100 CONSUME_MP_RATE / COOLTIME_RATE_BY_SKILLTYPE.** Parse `32d4aa30` / `32d4b020`: for each `SKILL_CATEGORY_%d`, `[SKILL_TYPE, BUFF_CHANGE_TYPE, count, v1..vn]`. The `VALUE` table is read only when the change type is 0, 1 or 3. lvl `32d47b85`: per block `[SKILL_TYPE, changeType, v[min(level,count)]]`. Classes: `SetFactor` `32e7b0d0` / `32e7b610`, packet `32e7b530` / `32e7ba70`. Both call `3210aae0` with `32e87810`.
- **103 CHANGE_SKILL_COOL_TIME_SPEED.** Same parse shape (`32d4b610`), but `VALUE` is read only when the change type is 3. lvl as 99. `SetFactor` `32e7bb50` and `DoFinish` `32e75ea0` set the user flag `+0x9314 = 1` (a cooldown-recalculation dirty flag).

**Phase 7 — flag and immunity types. Stores and readers found; reader semantics need Phase 1b.** The identity types use class `CX2BuffIdentityBehaviorTemplet`: `SetFactor` → `CreateAndInsertBuffIdentity(type, identity)`, `DoFinish` → erase, packet → insert. Each store is a list, and readers test it for non-empty (`[X] != [X+4]`).

| Type | Store | Readers to decode |
|---|---|---|
| 74 DETONATION_IMPOSSIBLE | `+0x67ec` | `31feb9f0` (`CX2GUUser` slot 504 = `DetonationEventProcess`), `3276cb20` (= `CX2GUNPC::CommonEventProcessNonHost`, corrected in 1c item 2) |
| 77 INVINCIBLE | `+0x67fc` | *Corrected in 1c item 2:* `31fcee80` (`CX2GUUser` slot 96 = `GetInvincible`), `32758760` (NPC slot 96), `3206ce71` inside `32063650` (slot 499 = `CommonFrameMove`). `31fcee40` and `32758470` were wrong attributions; neither touches the list. |
| 101 IGNORE_HP_DECREASE | `+0x680c` | `322726d0` (a getter; callers are user slots 59/61, see 1c item 2) |
| 102 DETONATION_IMPOSSIBLE_GAGE_RESET | `+0x681c` | `31feabd0` (`CX2GUUser` slot 308 = `DamageReactStateChange`), `31feb9f0`, `3276cb20` |
| 104 JUMP_IMPOSSIBLE | `+0x42c0` (*corrected in 1c item 2:* the insert is a plain push like the other identity cases; `3207b700` is the range-erase every identity erase uses) | `31fe83a0` (`CX2GUUser::SetInputData`), `3276e442` inside `3276d980` (`CX2GUNPC` slot 278 = `CommonEventProcess`) |
| 110 AWAKE_IMPOSSIBLE | `+0x682c` | *Corrected in 1c item 2:* `31fd4790` (`CX2GUUser` slot 345, a getter), reached through slot 343 `31fe9a90`. `31fd4460` was a wrong attribution. |
| 73 IGNORE_STATE_GUARD | own class. `SetFactor` `32e752b0` → unit `3227b600` (list `+0x67cc`), `DoFinish` `32e75300` → `3227a870`, packet `32e728b0` | `GetFinalDamage` `@322862bf` and others; not yet swept |
| 95 IMMUNE_NON_CONTROL_DEBUFF | own class. `SetFactor` `32e75c00` → unit `3227aa40` (list `+0x75ac`) **then `DeleteNonControlDebuff` `3229dcc0`**, `DoFinish` `32e75c50` → `32279a10` | not yet swept |
| 94 IMMUNE_BUFF_DEBUFF | Parse `32d4a770`: `[ALL_BUFF 0/1, ALL_DEBUFF 0/1, count, BTI1..BTIn]` (`IMMUNE_BUFF_TEMPLATE_ID`). lvl copy-as-is. Class `SetFactor` `32e79e30` (unit `3227a910`, `322780f0`, `32278120`, `3227b6b0`), `DoFinish` `32e75bd0` | not yet decoded |
| 105 IMMUNE_EDT | Parse `32d45c50` (**shared with 43 CUSTOM_FUNCTION**): `[v1..vn]`, the EDT list, no count. lvl copy-as-is. Class `SetFactor` `32e79770` → `31f8b860` | not yet decoded |
| 111 / 112 | **not implemented in June's client**. Port the enum only (Phase 2). | – |

**Phase 8 — finalizers.**
- **BFT 14 AURA.** Empty factor parse. Class `CX2BuffAuraFinalizerTemplet` overrides 10 slots (`32ec1370` OnFrameMove, `32ec51d0` SetFactor, …). Not yet decoded.
- **BFT 15 STICK_SHAKE / 16 USE_SKILL_BY_SKILL_SLOTTYPE.** Parse `32d44a80` / `32d44d60`: `[BUFF_USE_COUNT_TYPE, count, c1..cn, 0.0]`; the trailing 0 is the running counter. lvl `32d4cbc0` / `32d4cbd2`. 16's `SKILL_SLOT_TYPE` is read by its templet's `ParsingFinalizer` (`32ec0240`), not by the factor.
- **BFT 17 ATTACK**: not implemented in June's client.

**June-only bindings and keys (task item "locate").** Binding targets come from `31d95080` (`mov`/`push` after each name). Key owners are the functions that decrypt or reference the key string.

| Binding / key | June location | Owning phase |
|---|---|---|
| `AddBuffGroup_LUA` / `AddOfficiallyBuffList_LUA` | `326c3450` / `326bfd80` (loaded by `326c26f0`) | 10 |
| `SetMonsterBuffGroupID` | `31e4d770`: inline setter on the NPC, `[+0x8f70] = id` | 10 |
| `DeleteNonControlDebuff` | `3229dcc0` (also called by BBT 95's `SetFactor`) | 10 (and 7) |
| `GetBuffAccumulationCountNow` | `3227bbc0` | 10 |
| `ApplyBuffFactor` (cheat) / `AddRandomBuffList` | `3274c880` / `3267c370` (`AddRandomBuffList` also reads `DUMMY_BUFF_TEMPLET_ID`) | 10 |
| `SetPveBalanceBuffGiveScore_LUA` / `SendRandomMissionRewardBuffCheat` | `31edd0c0` / `322f0e30` | 10 (if client-side) |
| `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_BUFF_TEMPLATE_ID` / `_SKILL_ID` | `32630d20` (`ParsingBuffFactorID`) | 10 |
| `BUFF_FACTOR_ID_ULTIMATE` / `_ULTIMATE_PVP` (plaintext) | `32630d20` | 10 |
| `IS_AURA_BUFF`, `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT` | `32632390` (`SetDamageDataFromLUA`) | 10 |
| `CHECK_RIDINGPET_SPACIAR_BUFF` | `32d2f560` (only caller `32d3cb10`; not yet attributed) | 10 |
| **`DELETE_BUFF_WHEN_DIE`, `OFFSET_POSITION`, `CAN_DOWN`** | `32e7a4a0` = **`CX2BuffNeverMoveBehaviorTemplet::ParsingBehavior`** (slot 12) | **4b, not 10** (an existing class) |
| `ALL_BUFF` / `ALL_DEBUFF` / `IMMUNE_BUFF_TEMPLATE_ID` | `32d4a770` (BBT 94 factor parse) | 7 |
| `FORCE_SKILL_ID`, `IGNORE_SKILL_ID` | `32e78e90` = `CX2BuffResetSkillCoolTimeBehaviorTemplet::ParsingBehavior` | 4b |
| `BUFF_VALUE_CALCULATE_TYPE` | `32d42910` (80–84 factor parse) and `32e75790` (ByCondition `ParsingBehavior`) | 5b |
| `SKILL_SLOT_TYPE` | `32d4a370` (98 parse), `32e75d20` (`CX2AddDamageOfSkillSlotType` slot 12), `32ec0240` (BFT 16 `ParsingFinalizer`), `32e78e90` | 6, 8, 4b |
| `APPLY_CHECK_CONDITION` group (`BUFF_APPLY_CHECK_CONDITION_TYPE`, `CHECK_BUFF_TEMPLET_ID`, `APPLY_WHEN_HAVE`, `CHECK_HP_RATE_UP`) | `32d4d190`, called twice by `ParsingBuffFactor` | 4a |
| `AND_OPERATION` | `32d4d850` | 4a |
| `ACCUMULATION_AT_ONE_GO` | `32d4d850` | 4a |
| `IS_STORE_BUFF`, `BUFF_REPLACE_DETAIL_OPERATION`, `ACCUMULATION_DECREASE_*`, `ACCUMULATION_DELAY_TIME`, `IS_INSTANCE_KILL_DEBUFF` | `3236a9b0` (`CX2BuffTemplet::ParsingScript`) | 4a |
| `USE_CREATOR_UNIT_DATA` / `HYPER_COUNT` / `APPLY_TO_SPECIAL_ATTACK` / `ADDITIONAL_ATTACK_TYPE` | factor parses of existing types: 1 `32d42030`, 27 `32d45860`, 22 `32d45040`, 53 `32d46000` | 4b |
| `FROZEN_TIME`, `SHAKE_DECREASE_MAX_TIME`, `SHAKE_DECREASE_TIME_AT_ONCE` | `32ebe8b0` = `CX2BuffTimeFinalizerTemplet::ParsingFinalizer` | 4b |
| `ONLY_MY_UNIT`, `NOT_DELETE_EFFECT_SET_WHEN_FINISH` | `32eb82b0` (`CX2BuffEffectSetDisplayerTemplet` slot 6) and `32eb7460` (`CX2BuffSoundDisplayTemplet` slot 6) | 4b |
| **`EXP_RATE`, `IS_PARTY_BUFF`, `NOT_APPLY_BUFF_WHEN_SAME_BUFF_OWNER`, `DROP_RATE`, `VP_RATE`, `LUCKYTIMINGEVENT_*`, `NOT_USE_IN_PVE`/`_HUGE_BOSS_RAID`/`_HENIR`** | **not read by June's client.** They are absent as plaintext, as wide strings, and from all 6,240 decrypted XOR buffers, while the templet parser's other keys are all present. They are server-side keys. | 4a/11: **do not port**. Note the absence and leave them unread. |

**`FUN_31fcf3e0` / `FUN_3227baf0` status (the effect plan's Phase 8b blocker).**
- **`APPLY_LEARNED_SKILL_*` is unblocked.** Its two gates in `ParsingBuffFactorID` both map to March. The skill-level gate is `31fceab0`: it returns 1 when the unit has no `+0x867c`; otherwise it calls `UserSkillTree::GetSkillLevel` `32890620(id, 0, 0, 1)` (capped at 30), with a sealed-skill exception when the skill templet's `+0x174 == 5`. The buff gate is `3227baf0` = `CX2GameUnit::HaveThisBuff`.
- **`_ULTIMATE` stays blocked on `31fcf3e0`.** Structurally it takes unit `+0x867c` → `+0x494` (the user skill tree) and maps the skill id through a per-user map (`328905b0`; map at `+0x4ac`/`+0x4b0`, via `SkillTree::32205c00`). It then returns whether `g_pData->GetSkillTree()` (`33775860 +0x50`) `->GetSkillTemplet(id)` has `+0x178 != 0`. March has no skill-memo or ultimate flag. Finishing this is Phase 10's; `funcmap.tsv` carries the same note.

**Re-split recommendation.**
- Phase 5 (Sonnet) keeps the transcription-ready set: **76, 85, 86, 87, 96, 108, 109.** 85, 86, 87 and 96 also need Phase 2's `AT_COMMAND`.
- **78, 106, 107, 92, 93, 79, 80–84 and 88 go to an Opus session** (Phase 5b, or join Phase 7). 92 needs the damage-variance system plus the second physic/magic members (coordinate with Phase 4b's BBT 7/8 work).
- **97 moves to Phase 7**, because it is a per-class virtual.
- Phase 6 stays as listed but runs on **Opus** until a Phase 1b pass decodes its classes.

### Phase 1 — Done note (2026-10-02)

- **Complete.**
  - Loaders, factories, the factor-parse switches and the `_INT` design: all settled questions are in the 1a table, premium `_INT` included.
  - The store/level-slice/SGU switches.
  - The parse layout of **every** new BBT/BFT.
  - Every June reader of the ChangeStat-class stores, and the identity stores with their reader functions.
  - Bindings and key locations, and the `31fcf3e0` status.
  - 54 confirmed `funcmap.tsv` rows (plus 3 candidates: `31fceab0`, `326c6e20`, and the updated `31fcf3e0`).
  - The re-split, with `buff_census.py` taught `--phase 5b`, plus a "not implemented in June" exclusion (BBT 111/112, BFT 17) so the Phase 13 gate can reach 0. The census now reports BBT "not handled 38" and BFT "not handled 3".
- **Transcription-ready for Sonnet:** Phase 5 = 76, 85, 86, 87, 96, 108, 109. Everything else is marked Opus-only above.
- **Phase 1c (remaining RE, Opus 5.5 high).** None of this blocks Phases 2, 3, 4a or 5.
  1. Phase 6 classes: `SetFactor`/`DoFinish`/packet of 75, 89, 90, 91, 98, 99, 100, 103 and their `CX2GUUser` stores (`31fe2990`, `31fe29e0`, `31fe2a30`, `3210aae0`/`32e87810`, flag `+0x9314`).
  2. Phase 7 reader semantics for 74, 77, 101, 102, 104, 110 (functions listed in the table); readers for 73 (`+0x67cc`) and 95 (`+0x75ac`); classes 94 and 105.
  3. Phase 5b: classes 79, 80–84, 88, 93; `DamageData+0x470`/`+0x514`/`+0x4c5` and the three getters beside 78; the damage-variance roll for 92.
  4. Phase 8: the BFT 14 AURA class (10 slots); BFT 15/16 runtime counters, increment sites and UDP sync.
  5. Phase 4a/7/9 entry points: name `322a6960`'s virtual calls (`+0x210`, `+0x224`, `+0x234`, `+0x770`; one is likely the immunity check, one `PushNewBuffTempletToGameUnit`); `OnFrameMoveBuff`; `CreateAndInsertCustomFunctionByBuff` (from `CX2BuffCustomFunctionBehaviorTemplet::SetFactor` `32e79b00`).
- **Tools:** `june2017_re/buffre/` holds the RTTI→vtable scanner, the jump-table sweep, the switch dumper, the displacement/immediate xref sweep and the compact decompiler (see its README). The method is in "How to read June's stores" above.
- **Cost** (subscription, `/usage` at the end of the session, 2026-10-02): the 5-hour session window was **91% used**, and the week (all models) 12%. The window was shared with other sessions that day (10 sessions in 24h), so not all of it is Phase 1. `/ghidra` accounted for 52% of the last 24h; 82% of usage was at >150k context. All Opus, cache hit 97%. **Lesson for 1c:** split it into two sessions (it runs long and decompile-heavy, and most of the cost comes from context size).

```
Continuing the June buff/debuff port. Read JUNE_BUFF_PORT_PHASES.md in the repo root in
full -- "Starting context", the "June buff function map", "Per-type map" and the Phase 1
Done note. Branch: mods/june-buff-port. Use the ghidra skill on unpacked_x2.exe; do not
mutate the Ghidra project without asking.

Your task (Phase 1c, read-only, Opus 5.5 high): finish the Phase 1c list in the Done note,
in its order, so Phases 5b-9 can transcribe. For each item, record parse/store/read-site
pseudocode at the June addresses in the Per-type map, and add confirmed funcmap.tsv rows.
Read stores with the method in "How to read June's stores" (EBP bases, ADD-immediates,
the +0x1a0 sub-object). Promote a type to "transcription-ready" only when it meets the bar.
Record this session's /cost in a Done note. Commit.
```

### Phase 1c, item 1 — Phase 6 classes (2026-10-03)

Read-only Ghidra (no renames or comments). Uncarved functions were read with `buffre/rawfn.py`. Every address below is in `funcmap.tsv`.

**Shared facts used by several types.**
- `GetFactor`/packet layouts below are read from the class's own slot 5 / slot 13, not from the factor parse. They are the wire layout.
- Accumulation helpers on the templet: `32369da0` = `CX2BuffTemplet::GetAccumulationCountNow` (`UCM_ACCUMULATION_COUNT_NOW`, rounded), `32369f10` = `GetAccumulationMultiflier` (`UCM_ACCUMULATION_MULTIPLIER`), templet `+0x3c` = `m_uiAccumulationLimit`. `31c7b640` = float equality with epsilon (`IsSamef`).
- `3229fd80` = `CX2GameUnit::GetBuffFactor( vector<KBuffFactor>& )` (March `X2GameUnit.cpp:14168`: every live, unfinished buff's `GetFactor`). **Types 98, 99, 100 and 103 have no unit store.** Their readers call this, walk every `KBuffBehaviorFactor` whose type is theirs, and read the values straight out of it. The templet's own member is the store.
- `32206450( id, 1 )` = `CX2SkillTree::GetSkillTemplet`; skill templet `+4` = `m_eType` (`SKILL_TYPE`).
- New DamageData fields that the readers below use, matched to March by the key `SetDamageDataFromLUA` (`32632390`) decrypts at the store: `+0x4dc` `NO_DAMAGE` = `m_bNoDamage`; `+0x514` `DECREASE_FORCE_DOWN` = `m_fDecreaseForceDown`; `+0x590` `USED_SKILL_ID` = `m_eUsedSkillID`; `+0x5c4` `APPLIED_SKILL_ID` = March's `m_iAppliedSkillID` (already parsed, `X2DamageManager.h:1158`).

**75 DOUBLE_ATTACK_BUFF — transcription-ready.**
- Class `CX2BuffDoubleAttackBuffBehaviorTemplet` (vt `33535070`). Members: `m_eChangeType` (+0x10, int), `m_fRate` (+0x14).
- `ParsingBehavior` `32e75360`: `BUFF_CHANGE_TYPE` → `m_eChangeType`, 0 if absent. Same as March's type-56 class.
- `SetFactor` `32e77be0`: no factor → false. `ct = (int)v[0]`; if `ct != m_eChangeType` or `ct != BCT_PERCENT` → false **with no popup**. `rate = size>1 ? v[1] : 0`. If `DidStart()`: erase this identity first. `m_fRate = rate`; insert `( identity, rate )`; true.
- `GetFactor` `32e7e9f0`: `[ (float)m_eChangeType, m_fRate ]`. Packet `32e754f0`: if `size>1`: `m_eChangeType=(int)v[0]`, `m_fRate=v[1]`, insert (no erase). `DoFinish` `32e75540`: erase.
- Store: a **new** unit list beside type 56's, `vector<CX2DoubleAttackBuffPtr>` (June `+0x65b4`; 56's is `+0x65a4` = March `m_vecDoubleAttack`). Element class `CX2DoubleAttackBuff` (RTTI, ctor `322af570`) has the same layout as `CX2DoubleAttack` (`322af360`): identity + rate. Insert `32281b00`, erase `32279d20`. Reader `32275050` is byte-identical to 56's `32274f60` (= March `IsDoubleAttackByBuff`, `X2GameUnit.cpp:12428`) on the other list: `for each: if GetRandomFloat() < rate return true`.
- Reads (the attacker's list, not the defender's):
  - `CX2GUUser::DamageReact` `320462a0` @`32048da7`–`32048dbc`: `attacker = DD.optrAttackerGameUnit; if ( DD.attackType != AT_SPECIAL && attacker && attacker->IsDoubleAttackBuffByBuff() ) bDoubleAttack = true`.
  - `CX2GUNPC::DamageReact` `327b1f30` @`327b3ac7`–`327b3ae5`: same, plus `&& !DD.m_bNoDamage`.
  - Then (both) `if ( bDoubleAttack ) { fFinalDamage *= 2; bShowRedPicture = true; }` (@`32048dfa`, @`327b3b23`). March has that tail at `X2GUUser.cpp:9143`.
- **Phase 4b, type 56 in the same block (not new, but June changed it):** June's 56 check is `DD.attackType != AT_SPECIAL` on the **defender's own** list (`this`, @`32048d86`; March `X2GUUser.cpp:9137` uses `== AT_NORMAL`), and the NPC adds `!m_bNoDamage` (@`327b3a97`). June also adds, after 75: `if ( DD.m_eUsedSkillID ∈ { SI_SA_SDK_BRUTAL_CUTTER 8021, SI_SA_SCA_CRIMSON_BREAK 8025, SI_SA_SCA_DREAD_WEAPON 8026 } && attacker->HaveThisBuff( BTI_BUFF_SHADE_THE_ILLUSION 430 ) ) bDoubleAttack = true` (@`32048dc1`, @`327b3aea`).

**89 CHANGE_SKILL_DAMAGE — class, store and reader transcription-ready; the hosts are Opus-only (see "Hosts" under 98).**
- Class `CX2BuffChangeSkillDamageBehaviorTemplet` (vt `33535378`). Member `m_vecSkillTypeRate` (+0x10), `vector< pair<int SKILL_TYPE, float rate> >`. `ParsingBehavior` is the shared default (`32ec5990`).
- `SetFactor` `32e7a9e0` (T = templet, `lim = T->m_uiAccumulationLimit`):
  - `if ( lim >= 2 && T->GetAccumulationCountNow() >= lim ) return true;` (at the cap: no-op).
  - No factor, or unit not `GUT_USER` (`+0x264 != 0`) → false.
  - If `!DidStart()`: for `i = 0; i+1 < n; i += 2`: `push( (int)v[i], v[i+1] )`; `unit->InsertSkillDamage( identity, m_vec )` (`31fe3b30`: insert only if the identity is not already there); true.
  - Else: `m = lim<2 ? 1 : ( IsSamef( mult, 0 ) ? 1 : mult )`; `tmp[i] = ( (int)v[i], v[i+1]*m )`. If `lim == 1`: `m_vec = tmp`. Otherwise for each `t` in `tmp`: if `m_vec` has `t.type`, `that.rate += t.rate - m`; else append `t`. Then `unit->UpdateSkillDamage( identity, m_vec )` (`31fe3c10`: erase identity, then insert); true.
- Slot `[7]` `32e7c020( unit, T )` (bool; recompute for the current stack count): null args → false; `v = GetFactor()`; not a user → false. `c = T->GetAccumulationCountNow()`, `m = IsSamef( mult, 0 ) ? 1 : mult`. `base[i] = ( (int)v[i], ( v[i+1] + c*m ) / ( c*m + 1.0 ) )`. If `c > 1`: `res = base`, then `c-1` times `res[i].rate += base[i].rate*m - m`. `m_vec = c > 1 ? res : base`; `UpdateSkillDamage`; true.
- `GetFactor` `32e7ecf0`: `[ t1, r1, t2, r2, ... ]` (type as float). Packet `32e7af60`: rebuild pairs from `[t, r]*`; if the unit is a user, insert. `DoFinish` `32e75b70` → `31fe2990` erase.
- Store: new unit list `vector< ptr >` (June `+0x92d4`), element (ctor `3207bec0`, vt `33474684`) = identity + the pair vector. Element getter `32369a30( skillType )`: the first pair with that type → `max( rate, 0 )`; none → 1.0.
- Reader `31fd7cc0` = `GetSkillDamageRateByBuff( skillId )` (a `CX2GameUnit` member; `ECX` = user):
  ```
  r = 1;
  if ( skillId > 0 ) {
      if ( g_pData->GetSkillTree() && ( pT = GetSkillTemplet( skillId ) ) )
          for each e in list89: r *= e->Get( pT->m_eType );      // 89
      for each e in list90: r *= e->Get( skillId );               // 90, outside the templet check
  }
  return r;
  ```
- Consumers of `31fd7cc0` (they are the read sites for 89 **and** 90):
  - `CX2GUUser::DoStateEndStart` (`3203c3a0`, March `X2GUUser.cpp:6350`), right after `m_NowStateData` is set (@`3203c7de`–`3203c801`). June caches the state power rate in a protected float: `m_fPowerRate = m_NowStateData.m_fPowerRate; m_fPowerRate *= GetSkillDamageRateByBuff( m_NowStateData.m_eSkillID ); m_fPowerRate *= rate98` (98, below). June's `GetPowerRate` is `CX2GUUser` slot 129 (`31fcb230`) and returns that cache; March's (`X2GUUser.cpp:23045`) returns `m_NowStateData.m_fPowerRate` directly. Per-class state code multiplies the cache further (e.g. `32ad6ac0` by `+0x974c`, `32b1cd20`), so the buff term survives class rescaling.
  - The summoned-NPC power helper `322ece70`, called 21 times from `CX2Game::SetUserSummonedNPCInfo` (`3230c1a0`, March `X2Game.cpp:14060`): `rate = ( base + ... ) * ( 3201ad10( id ) + 1 ) * GetSkillDamageRateByBuff( id ) * rate98summon * GetAdditionalAttackByType( AT_SPECIAL )` (@`322ed2f7`–`322ed32d`). March multiplies only `GetAdditionalAttackByType( AT_SPECIAL )` inline at each `case` (e.g. `X2Game.cpp:14277`, under `ADD_SUMMON_NPC_SPECIAL_ACTOVE_POWER_RATE`).
  - Phase 9 only: `32369bd0` (`RNSI_SA_LDN_SINISTER_MIND` 30048, @`32369c84`) and `3236df00`/`32773ee0` (`RNSI_A_CHG_MARK_OF_COMMANDER` 26214) are custom-function bodies; port them with those classes.

**90 CHANGE_SKILL_DAMAGE_BY_SKILL_ID / 91 CHARGE_MP_HIT_BY_SKILL_ID — 91 transcription-ready; 90's class, store and reader are, its hosts (shared with 89) are Opus-only.**
- Classes `CX2BuffChangeSkillDamageBySkillIDBehaviorTemplet` (vt `33535594`) and `CX2BuffChargeMpHitBySkillIDBehaviorTemplet` (vt `335355d0`). Member (+0x10) `vector< { float rate; vector<int> skillIds; } >` (element 0x14).
- `SetFactor` `32e849c0` / `32e84d70`: no factor or not a user → false. If `DidStart() && T->GetAccumulationCountNow() == 0` erase the identity. Clear the member. Walk `v`: the value `-1.0` (`334380f0`) closes a block (`push( rate, ids )`, reset); the first value after a close is the rate; later values are `(int)` skill ids. Then insert the identity with the member (`3200ae70` for 90, `3200af50` for 91; insert only if absent); true.
- `GetFactor` `32e7ef50` / `32e7f2e0`: per block `[ rate, id1..idn, -1.0 ]`. Packet `32e84b90` / `32e84f40`: same walk; insert if a user. `DoFinish` `32e75b90` / `32e75bb0`: if a user, erase (`31fe29e0` / `31fe2a30`).
- Stores: new unit lists (June `+0x92e4` for 90, `+0x92f4` for 91), element ctors `320849a0` / `32084ae0`. Element getter (`32369aa0` for 90, `32369b10` for 91, byte-identical): `r = 1.0; for each block: if skillId ∈ ids: r = block.rate; return r` (**the last matching block wins**, no clamp).
- 90 is read by `31fd7cc0` above.
- 91 reader `31fd7df0( skillId )`: `r = 1; if ( skillId > 0 ) for each e in list91: r *= e->Get( skillId ); return r`. It is reached only through `CX2GUUser` vtable slot 109 (`31fd7ea0`): `GetChargeMpRateByBuff( const DamageData* pDD ) { return pDD ? 31fd7df0( pDD->m_iAppliedSkillID ) : 1.0f; }`. The `CX2GameUnit`/`CX2GUNPC` slot is `3208dd70` (`return 1.0f`), so declare it virtual on `CX2GameUnit`.
- 91 read site: `CX2GameUnit::SetDamageExceptionProcess` (`322990e0`) @`32299510`–`3229954e`: `fHitAddMP *= attacker->GetChargeMpRateByBuff( pDamageData )`, on the same protected value that 85/86 scale just before it (@`3229947c`–`322994a3`). **It is unconditional:** both `attackType != AT_COMMAND` branches (@`32299342`, @`32299460`) jump straight to `32299510`, so 91 applies to every attack type. The host is the same function Phase 5 edits for 85/86, so 91 follows Phase 5's treatment of it.

**98 CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE — class and reader transcription-ready; the host is Opus-only.**
- Class `CX2AddDamageOfSkillSlotType` (vt `33535160`). Members `m_eSlotType` (+0x10, int), `m_fRate` (+0x14).
- `ParsingBehavior` `32e75d20`: `SKILL_SLOT_TYPE` → `m_eSlotType`, 0 if absent.
- `SetFactor` `32e78080`: no factor → false. `m_eSlotType = (int)v[0]` (0 if empty); **if negative → false**. `m_fRate = size>1 ? v[1] : 0`; true. No unit call.
- `GetFactor` `32e7f890`: `[ (float)m_eSlotType, m_fRate ]`. Packet `32e78170`: `m_eSlotType = (int)v[0]`, `m_fRate = size>1 ? v[1] : 0`. `DoFinish` `32e75cc0`: if a user, erase from 89's list (`31fe2990`; harmless, 98 never inserts there).
- No store. Reader `32039e00( skillId )`, the only function that compares a factor type against `0x62`:
  ```
  pT = GetSkillTemplet( skillId ); if ( !pT ) return 1;
  if ( pT->m_eType == ST_HYPER_ACTIVE_SKILL /*13*/ ) return 1;
  slot = userSkillTree.FindEquippedSlotIndex( skillId );   // 3288fff0: 0-4 = slot A row, 5-9 = slot B row; none -> return 1
  r = 1;
  for each KBuffFactor kf in GetBuffFactor(): for each bf in kf: if ( bf.type == 98 && bf.size() > 1 )
      if ( SlotIndexToSlotType( slot ) == (int)round( bf[0] ) ) r *= bf[1];
  return r;
  ```
  `3214f3d0` = `SlotIndexToSlotType`: `0–3 → SKILL_SLOT_A (1)`, `5–8 → SKILL_SLOT_B (2)`, `4, 9 → SKILL_SLOT_TC (3)`, else 0.
- Consumers: both in `CX2GUUser::DoStateEndStart` (`3203c3a0`):
  - `if ( m_NowStateData.m_eSkillID != previous state's ) rate98 = 32039e00( skillId )` (@`3203c649`); it multiplies the state power rate (above).
  - `if ( IsSummonSkill( skillId ) ) rate98summon = 32039e00( skillId )` (@`3203c586`/`3203c599`). It is **not** reset for other skills, so the last summon skill's value persists. The summon helper reads it through `31fea430`.
  - `31fc8200` = `IsSummonSkill( skillId )` after `32207210` (`GetSkillTemplet(id)->+0x178` if non-zero, the same "memo/base id" field `31fcf3e0` reads, else the id). Its 54 skill ids were enumerated by interpreting the function over 0–70000 (scratch `emu_switch.py`; the largest compare in it is 31320, so the list is complete): 22020, 22027, 22036, 23015, 23213, 23216, 23218, 25000, 25011, 25014, 25015, 25020, 25030, 25055–25057, 25062, 25601, 26014, 26025, 26050, 26060, 26064, 26215, 26238, 28018, 28023, 28054, 28057, 29003, 29029, 29601, 30013, 30016, 30053, 31002, 31007, 31015, 31038, 31044, 31045, 31047, 31053–31055, 31059, 31065, 31082, 31087, 31096, 31229, 31232, 31234, 31320 (names: `RNSI_SA_AVP_HELL_DROP` … `RNSI_A_ROM_G_EX_MAGNETICFORCE_UPGRADE`; resolve them against June's `Enum.src.lua` when porting). The `+0x178` mapping needs the same skill-templet field that blocks `_ULTIMATE`; March has no such field, so the port uses the plain id and says so.
- BFT 16 shares the slot-type idea (Phase 8); this type has no finalizer of its own. **Phase 8 hand-off:** the only 98 buff in June data (`BTI_TRANSCENDENTAL_SLOT_*`) ends on `BFT_USE_SKILL_BY_SKILL_SLOTTYPE`, and 98's rate is cached in `DoStateEndStart` when the skill state starts. When item 4 maps BFT 16's increment site, record whether it fires before or after that cache; if before, the buff ends before it ever applies. *Answered in item 4: after (`@3203c649` precedes `CommonStateStart` `@3203c978`), so the buff applies to the skill that ends it.*
- **Hosts (89, 90, 98), Opus-only under the no-partial rule.** Both hosts are June rewrites, not March functions with a buff term added:
  - June's `DoStateEndStart` adds, besides the buff terms, a `+0x80d9` byte gate, a random sound/voice pick for state types 9/11/12, the protected power-rate cache (`+0x7cc8`/`+0x7cd4`/`+0x7ce0`, read by `GetPowerRate` and `31fea430`) and the `+0x2557..0x255a` resets. March's is 25 lines (`X2GUUser.cpp:6350`).
  - June routes all 21 summon cases through `322ece70`, which also adds `3201ad10( id ) + 1`, `31fea430` and stat-scaling flags March lacks.
  - The Opus session ports both whole (or decides with the user to keep March's and add only the buff terms, recording that as a deviation).

**99 CONSUME_MP_RATE_BY_SKILLTYPE / 100 COOLTIME_RATE_BY_SKILLTYPE — class and reader transcription-ready; the host functions are Opus-only.**
- Classes `CX2BuffChangeConsumeMpRateBySkillType` (vt `335353b4`) and `CX2BuffChangeCooltimeRateBySkillType` (vt `335353f0`) have byte-identical `SetFactor`, `GetFactor` and packet (`32e7b0d0`/`32e7b610`, `32e7f940`/`32e7fcb0`, `32e7b530`/`32e7ba70`). `DoFinish` is empty (`33093ad0`), `ParsingBehavior` is the default. Member (+0x10) `vector< { int skillType; int changeType; float value; } >`.
- `SetFactor`: no factor or not a user → false. If `!DidStart()`: for each triple `( (int)v[i], (int)v[i+1], v[i+2] )` push; true. Else (restack): `m = lim<2 ? 1 : ( IsSamef( mult, 0 ) ? 1 : mult )`, triples scaled `value*m`; `lim == 1` → replace; else for each new triple whose `(skillType, changeType)` already exists and whose `changeType` is `BCT_FIX_VALUE` or `BCT_PERCENT`: `old.value += new.value` (others are dropped); true.
- `GetFactor`: triples `[ (float)skillType, (float)changeType, value ]*`. Packet: rebuild the triples.
- No store. Readers (`r` is in/out; same body, different type id):
  ```
  apply( pSkillTemplet, float& r ):           // 99: 3203a250 ; 100: 3203a420(skillId, &r) returns "any applied"
      for each KBuffFactor kf in GetBuffFactor(): for each bf with type 99 (100):
          for each triple: if ( (int)triple[0] == pT->m_eType )
              switch ( (int)triple[1] ) { case BCT_SWAP_VALUE: r = value; case BCT_FIX_VALUE: r += value;
                                          case BCT_PERCENT: r *= 1 + value; default (100 only): applied = false }
      r = max( r, 0 );
  ```
- 99 host: June `CX2GUUser::GetActualMPConsume` (`3203cf60`, `CX2GUUser` slot 423) calls it @`3203d024`, between the passive MP multipliers and the `+0x74dc` reduction. June's function is much larger than March's (`X2GUUser.cpp:21081`, which only returns the templet value): it adds `vfunc +0x75c`, `32039a50( id, 0x3f )`, `32358970( 0xb )` and the `+0x74dc` term. Under the no-partial rule, porting 99 means porting June's whole `GetActualMPConsume` first. **Opus-only.**
- 100 host: `3235a710` (a `CX2Unit`-side helper: `this+0x494` is the user skill tree, `this+0x28` the unit UID) computes a cooldown from two rate getters and the skill's base, applies 100 through `32358770` (finds the user by UID and calls `3203a420`), and writes `SetSkillCoolTimeLeft` (`32893880`). It is called from 16 sites: the generic user path `31fd8ff0` and one per-class override each. March sets cooldowns inline in each class (`X2GUAdd.cpp:2683`, `X2GUAra.cpp:3924`, `X2GUChung.cpp:5983`, …). Mapping those 16 sites is **Opus-only**.

**103 CHANGE_SKILL_COOL_TIME_SPEED — transcription-ready.**
- Class `CX2BuffChangeSkillCoolTimeSpeedBehaviorTemplet` (vt `3353542c`; **no packet override**, slot 13 is the empty base, so peers never receive it). Member as 99.
- `SetFactor` `32e7bb50`: user only (`+0x264 == 0`), else false. Same triple build and restack as 99, but the merge applies only when `changeType == BCT_PERCENT`. Sets the unit dirty flag (June `+0x9314`) on both paths; true. `DoFinish` `32e75ea0`: `if ( unit && T && unit is a user ) dirty = true`.
- Store: a unit `map< int SKILL_TYPE, float >` (June `+0x931c`) plus the dirty flag. Rebuild `32039fb0`, called every frame from `CX2GUUser` slot 499 (`32063650` @`320636a8`):
  ```
  if ( !dirty ) return; dirty = false;
  for each entry in map: entry = 1.0;                          // keys are kept
  for each bf of type 103 in GetBuffFactor(): for each triple with (int)triple[1] == BCT_PERCENT:
      if ( map has key (int)triple[0] ) map[key] *= value; else map.insert( key, value );
  ```
- Reader `31fd8bc0( skillType )`: `map[key]` or 1.0. Its only caller is `32896850( skillId )`: if `g_pX2Game` and its `GetMyUnit()`: `myUnit->31fd8bc0( 32205ab0( skillId ) )`, where `32205ab0( id )` = `GetSkillTemplet( id )->m_eType`. Confirmed: it looks up the same skill-tree map with the same finder (`3221d5e0`) as `GetSkillTemplet` (`32206450`), which returns `node+0x10` (the templet, `m_eID` first), while `32205ab0` returns `node+0x14` (templet `+4`, `m_eType`). It skips `GetSkillTemplet`'s level gate. The data keys are `SKILL_TYPE` too (`ST_ACTIVE`, `ST_SPECIAL_ACTIVE_GRADE_1`, …).
- Read site: `CX2UserSkillTree::OnFrameMove` (`32896930`, March `X2UserSkillTree.cpp:48`): for each of the 5 + 5 equipped slots with time left: `left -= 32896850( slotSkillId ) * fElapsedTime` (March: `-= fElapsedTime` at `:70`/`:85`), then clamp at 0 and `CX2GageManager::SetMySkillCoolTimeList` (`32489230`). Apart from that multiplier, June's function is March's (March's extra `NEXON_QA_CHEAT_REQ` branch aside), so the host is clean. VALUE 2 therefore means "cooldowns run twice as fast", and only for the local player.
- June rebuilds in slot 499, which is `CX2GUUser::CommonFrameMove` (named in 1c item 2; it is also an INVINCIBLE reader). Rebuilding lazily inside the reader when the flag is set gives the same result.

**Phase 6 readiness after item 1:**
- **Transcription-ready (Sonnet):** 75, 91, 103. 75's host block in both `DamageReact`s also carries June's type-56 changes; the Phase 6 session lands them together under the Phase 4b flag or leaves 56 to 4b and says so.
- **Class, store and reader ready; host Opus-only:** 89, 90, 98 (`DoStateEndStart` and the summon helper), 99 (`GetActualMPConsume`), 100 (`3235a710` and its 16 callers). A Sonnet session may add their classes, stores and readers; the Opus session wires the hosts.

### Phase 1c, item 2 — Phase 7 reader semantics (2026-10-03)

Read-only Ghidra, same method as item 1. Every reader below came from a fresh displacement sweep of each list's begin/end pair, with function bounds taken from int3 padding (Ghidra has not carved most of them). Hits in the ctor (`322a4xxx`), dtor/clear (`322a0xxx`–`322a1xxx`), `CreateAndInsertBuffIdentity` and the erase helpers are excluded, and so is XOR-string noise (`[esp+…]`). **Phase 7 runs on Opus, so "ready" here only matters for rule 9.** Each type gets a one-line verdict, and every host diff says how far it went.

**Identity-type store recap (74, 77, 101, 102, 104, 110).** The class is `CX2BuffIdentityBehaviorTemplet`. `CreateAndInsertBuffIdentity` `3227ad60` builds one `CX2BuffIdentity` (identity, 8 bytes) and `push_back`s it (`322ad1e0`) onto the type's `vector<CX2BuffIdentityPtr>` on `CX2GameUnit`, next to March's existing identity vectors. Every reader tests `!vec.empty()`.

**74 DETONATION_IMPOSSIBLE / 102 DETONATION_IMPOSSIBLE_GAGE_RESET — Opus-only (June rewrote `m_bNoDetonation` itself).**
- `31feb9f0` = `CX2GUUser::DetonationEventProcess` (slot 504; March `X2GUUser.cpp:19238`):
  ```
  if ( slot128() /* byte +0x91e6 getter, where March has GetRidingOn() */ ) return;
  if ( b6890 || b6892 || !list102.empty() ) { gage->SetNowChargeMpForDetonation( 0 ); gage->m_bCanChargeMpForDetonation = false; }
  else if ( list74.empty() && ( m_OrgInputData.pureZ || m_OrgInputData.pureX ) ) gage->m_bCanChargeMpForDetonation = true;
  ```
  March only has `if ( GetNoDetonation() ) return;` before setting the flag. **June zeroes the charge where March just returns**, so every existing March skill that calls `SetNoDetonation(true)` now also wipes the stored charge. `32081640` = `CX2GageData::SetNowChargeMpForDetonation` and gage `+0xf6` = `m_bCanChargeMpForDetonation`. The uncarved gage code at `32a33b9x` is March's `X2GageData.cpp:26-40` (clamp to max, zero, `SetCanCharge(false)`).
- `31feabd0` = `CX2GUUser::DamageReactStateChange` (slot 308; March `:11262`). Only the region around the read was diffed: right before March's `if ( IsDamagedReaction( pDD ) )` (`:11894`) June inserts the same reset block (`b6890 || b6892 || list102` → charge 0, flag false). It also adds `pDD->damageType != DT_BUFF (6)` to that `if`, which is March's `SERV_9TH_NEW_CHARACTER` early return in `DetonationStateChange` (`:11964`) moved to the caller. The tail `if ( pDD->m_bNoDetonation ) b6892 = true` is March's `:11947`.
- **June split March's `m_bNoDetonation` into two `CX2GameUnit` bytes.**
  - `+0x6890` is the state flag. It is written 1/0 in pairs by per-class state code (about 50 writes in `329xxxxx`–`32cexxxx`, the sites where March calls `SetNoDetonation(true/false)`, e.g. `X2GUAdd.cpp:4963/4993`), and zeroed by `31fcde81`, `32015afe`, `320608b2` and `322a5e38`.
  - `+0x6892` is the damage flag. It is written from `pDD->m_bNoDetonation` (`32046314` in `CX2GUUser::DamageReact`, March `:7644`; `327b1fd0` in the NPC one), and also at `31feb964`, `3205e9df/e8` and `327af377/80`.
  - Porting 74/102 therefore means porting the split and both rewritten hosts.
- NPC: `3276cb20` = **`CX2GUNPC::CommonEventProcessNonHost`** (March `X2GUNPC.cpp:20775`: die-state timer → `StateChangeToDieForce`, then the HP≤0 `m_bNeverMove = false` block; so `+0x872b` = `m_bNeverMove`). Its state-change condition chain gains a June-only condition **`CAN_DETONATION`** (condition byte `+0xba`, parsed in `32784c00` @`32794f62`, the only June parse site of the key; March parses conditions at `X2GUNPC.cpp:6035` and `:15719`). It sits after `IS_ANOTHER_TEAM` (`+0xb9`, March `:21358`) and passes only when `list74.empty() && list102.empty() && !b6890 && !b6892`. The next condition, `CHECK_NEVER_MOVE_TRUE` (`+0xbb`, passes when `m_bNeverMove`), is a non-buff June addition in the same chain. **The host path `3276d980` (`CommonEventProcess`) tests `+0xb7`, `+0xb8`, `+0xb9` and `+0xbb` but not `+0xba`**, so June evaluates `CAN_DETONATION` only for non-host NPCs. No June script uses `CAN_DETONATION` (0 hits in `june_src_fixed`).

**77 INVINCIBLE — getter ready; the frame-move half is Opus-only.**
- `31fcee80` (`CX2GUUser` slot 96) = `GetInvincible`: `if ( !list77.empty() ) return true; return m_FrameDataNow.stateParam.bInvincible;` (protected bool `+0x7fe8`). The NPC slot 96 `32758760` is the same on `m_NPCFrameData.stateParam.bInvincible` (`+0x8368`). March: `X2GUUser.h:1582`, `X2GUNPC.h:1950`. The base slot is pure.
- **March reads `stateParam.bInvincible` directly in 12 places**, which bypass the getter: `X2GUChung.cpp:613/646`, `X2GUNPC.cpp:8922/10268/11920/12719/18853/29201/29214`, `X2GUUser.cpp:7809/9919/17516`. Phase 7 must check in June whether each one calls slot 96 or reads the field. Until then the getter prefix is not the whole reader set.
- `3206ce71`, at the tail of `32063650` = **`CX2GUUser::CommonFrameMove`** (slot 499; March `:15219`; the function that calls `UserSkillTree::OnFrameMove`):
  ```
  if ( !list77.empty() && m_fForceInvincibleTime <= 0 ) { SetForceInvincible( 0.5f ); SetShowInvincible( 0.5f ); }   // slots 71/72; 0.5 = 3343777c
  ```
  Slot 71 `3208ed80` writes the protected float `+0xdbc`, slot 72 `3208dd40` writes `+0xdd4`. **Only the tail was diffed.** It matches March's `:17888-17896` (`ShowActiveSkillCutInAndLightByScript` = `IsSamef(-1, +0x9278)` → slot `0x834`). After it June adds a second cut-in (`+0x9280` → `0x838`), `0x84c`, `322754f0`, `0x3c4`, `320458f0` and `31fcfb10`, then the 77 block, a position save to `+0x94d4` and `32296b10`. The host is a June superset of 2,700 lines and was not diffed in full.

**101 IGNORE_HP_DECREASE — user-only in June; Opus-only (the hosts became virtual and grew).**
- `322726d0` is a getter (`!list101.empty()`). Its only callers are `CX2GUUser` slot 59 `3202fcf0` (`UpNowHp(delta, min)`) and slot 61 `3202ff60` (`SetNowHp(v)`):
  - slot 59: `if ( nowHp > 0 && delta < 0 && IgnoreHpDecrease() ) return;`
  - slot 61: `if ( nowHp > 0 && v < nowHp && IgnoreHpDecrease() ) return;`
- March's `UpNowHp`/`SetNowHp` are non-virtual inlines on `CX2GameUnit` (`X2GameUnit.h:2311/2315`). In June they are virtual. The NPC overrides (`32784200`/`32778c60`) never call the getter, so **NPCs ignore 101.**
- The user `SetNowHp` override also adds two more refuse-decrease gates (`320072d0`, `3202b9a0`; not decoded). It then calls the base (`322824b0` = March's `if ( gage ) gage->SetNowHp( v )`) and, for the own unit, a low-HP UI call `3289db00( 0xd, 0|1, -1 )` (not decoded).

**104 JUMP_IMPOSSIBLE — user half ready; NPC half ready, with a 4b hand-off.**
- User `31fe83a0` = `CX2GUUser::SetInputData` (March `:28554`). June's `if ( m_fCanNotJumpTime > 0 )` Up-key clear (`:28668`) becomes `if ( m_fCanNotJumpTime > 0 || !list104.empty() )`. The rest matches March block for block, except that both attack-impossible blocks (`:28578`, `:28611`) also OR the byte `+0xded`. **That byte is dead in March:** its only writers are in `CX2PVPGameWinner` (`32ed5d97`–`32eda3a4`), a June PvP mode March lacks. `InputData` offsets checked: `m_InputData` is at `+0x86da` and `m_OrgInputData` at `+0x8732`.
- NPC: `3276d980` = **`CX2GUNPC::CommonEventProcess`** (slot 278; March `:21405`). Its AI-message switch gates `SCT_AI_JUMP` (12) on `list104.empty()` (@`3276e442`; March `:21819`).
- **Phase 4b hand-off.** The same switch also gates `SCT_AI_DASH` (11), `SCT_AI_JUMP_DIR` (14) and `SCT_AI_DOWN_DIR` (15) on `m_vecDashImpossiblePtr.empty()` (June `+0x4260`; the user's `SetInputData` uses the same list for the double-left/right clear). March's NPC switch has no dash-impossible gate. This belongs to the existing BBT `DASH_IMPOSSIBLE`, the same precedent as item 1's 75/56.
- Only the AI-message cases were diffed in this host.

**110 AWAKE_IMPOSSIBLE — Opus-only (June replaced a 56-use macro with a virtual).**
- Getter `31fd4790` (`CX2GUUser` slot 345) = `!list110.empty()`. Its consumer is slot 343 `31fe9a90`, `bool TryHyperModeStateChange( int state )` (Ain's copy is `32a53150`):
  ```
  if ( !m_InputData.oneLCtr ) return false;            // +0x8723 = m_InputData + 0x49
  if ( IsAwakeImpossible() /*345*/ ) return false;
  if ( slot434() /*class hook, base 31e4c550 = false*/ ) { protected short +0x7e84 = 0x1c; slot435( 0 ) /*base empty*/; return true; }
  if ( !IsThereEffectiveBlackHoleDamageEffect() /*322731a0: +0x3f78 != 0*/ && f8d78 <= 0 && CanHyperMode() /*344*/ )
      { b8d4c = true; StateChange( state ); return true; }
  return false;
  ```
  Slot 344 `31fd4730` = March's macro condition: `m_HyperModeCount > 0 && ( GetRemainHyperModeTime() <= 0 || GetCanChangeHyperModeInHyperModeState() /*+0x8fc3*/ )`.
- March expresses this as `ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE` (`X2GUUser.h:11`, 56 uses), and Ara inlines it (`X2GUAra.cpp:6348` and five more).
- June adds the class hook (slots 434/435) and the fields `+0x8d78`/`+0x8d4c`, none of which March has. Ara's six June sites (`32bb2d40`, `32bb2ff0`, `32bb3290`, `32bb34d0`, `32bb37e0`, `32bb3ab0`) call slot 345 inline.

**73 IGNORE_STATE_GUARD — ready (BTI 975 needs a C++ enumerator).**
- Class: `SetFactor` `32e752b0`: `if ( !DidStart() ) unit->insert( identity )` (`3227b600`, list `+0x67cc`); true. `DoFinish` `32e75300` erases (`3227a870`). Packet `32e728b0` inserts. `GetFactor` `32e7e960` writes the header only (no values).
- The only reader is `GetFinalDamage` @`322862bf`. In June's code EBP is the attacker (it reads 87/96's members) and EDI the defender:
  ```
  if ( !defender->list73.empty() ) pDD->m_bIgnoreStateDefence = true;          // DD +0x541
  if ( attacker->HaveThisBuff( BTI_BUFF_SCHWERT_PLATZEN /*975*/ ) ) pDD->m_bIgnoreStateDefence = true;
  ```
  It is inserted right before March's state-defence block (`X2GameUnit.cpp:2716-2731`), which June keeps as is (DD`+0x4c5` = `m_bPenetrate`, `+0x9e4` = `m_DamageReducePercent`, DD`+0x478` = `m_bGuard`, `+0x9f0` = `m_DamageGuardPercent`, clamp 100). So a unit carrying 73 loses its damage reduction and guard. June's users are debuffs (e.g. `BTI_DEBUFF_PANDEMONIUM_FEAR`, `IS_DEBUF = TRUE`).
- BTI 975 is in March's Lua Enum (Phase 2) but not in the C++ `BUFF_TEMPLET_ID` enum.
- **Flag for Phase 14:** June writes the flag into the attacker's `DamageData`, so on multi-target damage data it stays set for every later target. Transcribe it verbatim.

**95 IMMUNE_NON_CONTROL_DEBUFF — ready.**
- `SetFactor` `32e75c00`: `if ( !DidStart() ) { unit->insert( identity ) /*3227aa40, list +0x75ac*/; unit->DeleteNonControlDebuff(); }` true.
- Packet `32e728d0`: **insert only, no delete**.
- `DoFinish` `32e75c50` erases (`32279a10`). `GetFactor` `32e7f770` writes the header only.
- `DeleteNonControlDebuff` `3229dcc0`: walk the unit's templet vector (`+0xe14`); erase (`3229d320`) every templet with `IS_DEBUF` (`+0x64`) and `IsNonControlDebuff()` (`32369cc0`), otherwise advance.
- The only reader is `3227b790` (Phase 4a's stub): `return !list95.empty() && templet->IsNonControlDebuff();`.

**94 IMMUNE_BUFF_DEBUFF — ready; two June quirks are transcribed verbatim.**
- Factor: `32d4a770` pushes `ALL_BUFF`, `ALL_DEBUFF`, then `ParsingTableForWhile( "IMMUNE_BUFF_TEMPLATE_ID", vec, true )`. The `true` is a literal, so the count is always pushed, **and an empty `{}` leaves the 1.0 placeholder**.
- `SetFactor` `32e79e30` (args `factor, unit, templet`):
  ```
  if ( DidStart() && T->GetAccumulationCountNow() == 0 ) unit->EraseImmuneList( T->identity );   // 3227a910
  m_vecBTI.clear();
  if ( v.size() >= 1 && v[0] != 0 ) unit->m_setImmuneAllBuff.insert( BTI );     // 322780f0, std::set at +0x65d4 (size +0x65dc)
  if ( v.size() >= 2 && v[1] != 0 ) unit->m_setImmuneAllDebuff.insert( BTI );   // 32278120, set at +0x65e4 (size +0x65ec)
  for ( i = 2; i < v.size(); ++i ) m_vecBTI.push_back( (int)v[i] );
  unit->InsertImmuneList( identity, m_vecBTI );   // 3227b6b0: element {identity, vector<int>} -> list +0x65c4
  return true;
  ```
- **Quirk 1:** the loop starts at index 2, which is the count, so the count is stored as an immune BTI. BTI 1 is `BTI_DEBUFF_SILENCE`. June's single-entry block (`BattleFieldBuffFactor.src.lua:111419`, SLEEP only) and its empty block (`:116444`) therefore also grant SILENCE immunity.
- The packet `32e7a040` handler is the same as `SetFactor` without the erase. `DoFinish` `32e75bd0` does the list erase and both set erases (`32278840`, `32278880`).
- **Quirk 2:** `GetFactor` `32e7f670` sends `[ (float)byte+0x20, (float)byte+0x21, m_vecBTI… ]`. Those two bytes are zeroed by the ctor (`32e871d0` @`32e87219`), copied by the clone (`32e7a930`), and written nowhere else, so **peers always receive `ALL_BUFF = ALL_DEBUFF = 0`**.
- Fill-in for Phase 4a's stub `IsImmuneBuffByBuffBehavior` (`322a80a0`): `T = GetBuffTempletPtr( BTI ); return ( setAllBuff.size() && !T->IsDeBuff() ) || ( setAllDebuff.size() && T->IsDeBuff() ) || any element e in list +0x65c4 with BTI ∈ e.vec` (`32369d70`).

**105 IMMUNE_EDT — class and reader ready; the host's rate math goes to 4b.**
- `SetFactor` `32e79770`: no unit call. `DidStart()` triggers a discarded `GetAccumulationCountNow()`. Then `m_vecEDT.clear(); for each v: push_back( (int)v )`; true.
- Packet `32e79810` is the same. `GetFactor` `32e80390` = `[ (float)edt… ]`. `DoFinish` is the empty base `33093ad0`. **No unit store:** the templet member is the store.
- Reader `322a0400` `IsImmuneEDTByBuff( edt )`: walk `GetBuffFactor()` (`3229fd80`); true if any type-105 (`0x69`) behavior factor holds `(int)value == edt`.
- Its only caller is `322a0510` = **`CX2GameUnit::ProcessEnchantExtraDamage`** (slot 213; March `X2GameUnit.cpp:3529`). Inside the attacker/enchant-data block, after `if ( rate < fRandomValue ) return false;`, June adds `if ( IsImmuneEDTByBuff( basicExtraDamageType ) ) return false;`. The switch over the six enchant EDTs (1001, 1011, 1101, 1543, 1654, 1765) is March's, except that the `ProcessEnchant*` calls are virtual (slots 214–219).
- The host's rate math has March's shape (`(rate·modifier)·increase + fix`), but `32274df0( edt, BCT )` (attacker list `+0x6594`, special-cases EDT `0x3e9`) and `32272d90` (branches on unit type) are not proven equal to March's `GetChangeEnchantAttackRate`/`EnchantAccumulationRateModifier`. **Hand that to 4b**, which owns `CHANGE_ENCHANT_ATTACK_RATE`.
- Search scope: only the 19 callers of `GetBuffFactor` were scanned (within 0x400 bytes of each call) for a `0x69` compare. A reader that walks templets directly would not be found.

**Item 2 readiness.**
- **Ready:** 73 (plus the BTI 975 enumerator), 94, 95, 105 (except the host's rate math → 4b), 104 (plus the dash hand-off → 4b), and 77's getter.
- **Opus-only:** 74 and 102 (the `m_bNoDetonation` split and two rewritten hosts), 77's `CommonFrameMove` block and the 12 direct `bInvincible` reads, 101 (virtual, user-only HP setters with undecoded extra gates), and 110 (the macro → virtual rewrite).

### Phase 1c, item 3 — Phase 5b classes and reader context (2026-10-03)

Read-only Ghidra, same method as items 1 and 2. Uncarved functions were read with `buffre/rawfn.py`. Callers were taken from a raw `E8`/`E9` scan of the whole `.text` (`31aa1000`–`3375ca00`), not from Ghidra's xrefs, which miss uncarved code. The scan has to cover the whole range, because code continues above `33200000` (for example `3322a380`).

**Corrections to Phase 1b.**
- **78's three other getters:** `32071af0`, `32071b10` and `32071b90` read the **option value** (the protected float at member `+0x24`) of members `+0x5194`, `+0x5248` and `+0x5ac8`. They do not read the result (`+0xa0`) of `+0x5118`/`+0x51cc`/`+0x5a4c`, which are not member starts.
- **92's second physic/magic members** (`+0x44ec`/`+0x45a0`) are not filled only by buffs. Their basic values come from two new fields in June's stat block; see 92.

**79 CHANGE_NOW_FORCE_DOWN_PER_SECOND — transcription-ready.**
- **Class** (vt `335351d8`, create `32e86e90`, 0x30 bytes):
  - `m_eChangeType` (`+0x10`): the ctor sets `BCT_FIX_VALUE` (1) and nothing else writes it, so the templet's `BUFF_CHANGE_TYPE` is never read.
  - `CKTDXCheckElapsedTime` (`+0x14` target, `+0x18` sum).
  - `m_fResultValue` (`+0x1c`).
  - Two delegates bound to `this`: `+0x20`/`+0x24` (tick `32e89800`) and `+0x28`/`+0x2c` (value `32e86d80`).
- **`ParsingBehavior` `32e755a0`:** `TIME_INTERVAL` → target. The key is **required**: when it is absent the function returns false (`AL` checked at both `ret`s). June data uses 0, 1 and 0.
- **`SetFactor` `32e855f0`:** March's `CX2BuffChangeNowHpPerSecondBehaviorTemplet::SetFactor` (`X2BuffBehaviorTemplet.cpp:249`), with regist fixed at 0 and no popups:
  ```
  if ( !GetBehaviorFactor( type, &v ) ) return false;
  if ( (int)v[0] != m_eChangeType ) return false;
  bind delegates;
  fNew = v.size() > 1 ? v[1] : 0;                      // 32e86d80( v, 0 ) = ( 1 - 0 ) * v[1]
  if ( !DidStart() ) m = fNew;
  else if ( ( c = T->GetAccumulationCountNow() ) == 0 ) m = |fNew| > |m| ? fNew : m;
  else { mul = T->GetAccumulationMultiflier(); base = m / ( 1 + mul*(c-1) );
         m = ( |base| >= |fNew| ? base : fNew ) * ( 1 + mul*c ); }
  return true;
  ```
  This is March's non-`ADJUST_BUFF_ACCMULATION_CALCULATE` branch. June has no `BCT_PERCENT` branch here, which matches its NowHp class (4b-i).
- **`OnFrameMove` `32e756f0`:** `timer.OnFrameMove( dt )`, then the tick `32e89800`: `if ( sum >= target ) { sum = 0; pUnit->GetGageData()->UpNowForceDown( m ); }`.
  - Unit `+0xc04` is the gage data, and gage `+0x5c` is the force-down `Gage`. June's `Gage` is 0x38 bytes, so the detonation gage is at `+0x94` (`32081640`).
  - `31f8de20` = `Gage::Increase( f, 0 )`, the same body as March's `X2GageData.cpp`.
  - So this is March's `CheckAndResetElapsedTime` followed by `UpNowForceDown`.
- **`GetFactor` `32e7eaa0`:** `[ m ]`. **Packet** `32e85790`: `m = v[0]` (0 if empty), then the delegates are bound.
- No `DoFinish` (empty base), no unit store, no host: the class writes the gage directly, and March has `UpNowForceDown`.
- **Data note:** `TIME_INTERVAL = 0` ticks every frame (`BTI_DEBUFF_JUDGEMENT_FIRE`, `BTI_DEBUFF_DECREASE_FORCE_DOWN_IMMEDIATELY`; `BTI_DEBUFF_SHADOW` uses 1). The value is applied per tick, not per second. Transcribe this as-is.

**80–84 *_BY_CONDITION — 82, 83 and 84 transcription-ready; 80 and 81 ready except the second member, which follows 4b's BBT 7/8.**
- **Class** `CX2BuffChangeStateByConditionBehaviorTemplet` (vt `335350ac`): `m_eChangeType` `+0x10`, `m_eValueCalculateType` `+0x14`, `m_fResultValue` `+0x18`. `ParsingBehavior` `32e75790` reads `BUFF_CHANGE_TYPE` and `BUFF_VALUE_CALCULATE_TYPE`; **both are required** (`AL` checked).
- **The factor vector** goes through three steps:
  1. Parse `32d42910`. `bIsSkill` is true for `BattleFieldBuffFactor`, the default at `X2BuffFactor.h:835`.
  2. Level slice `32d48063` copies `[ct, vct, n, v1..vn]` whole.
  3. The SGU case `32d4ce60` appends a runtime value:
     ```
     if ( (int)v[1] == BVC_RALLY_OF_HERO /*0*/ ) {
         x = g_pX2Game ? (float)g_pX2Game->GetUserUnitNum() : 0;   // CX2Game slot 63, 322ccd00: non-null m_UserUnitList entries (March X2Game.cpp:5278)
         if ( g_pX2Game->GetGameType() /*slot 67, +0x2b0*/ == GT_PVE /*4*/ ) x *= 0.5;
         v.push_back( x );
     }
     ```
     `GT_PVE` is June-only (`Enum.src.lua:726`). March's `GAME_TYPE` stops at `GT_BATTLE_FIELD` (3), so in a March build the halving can never run.

  The result is `v = [ct, vct, n, v1, v2, x]`.
- **The value** (`32e77f40( v, unit, regist )`): `vct == 0 ? v[3] + v[4] * ( 4.0 - v[5] ) : 0`. A missing element reads as 0, and unit and regist are unused.
  - This is right only when n = 2, and all 25 June factors carry `VALUE = { a, b }`. For example, `{ 1.06, 0.35 }` gives 1.06 + 0.35 × (4 − user count).
- **`SetFactor` `32e78860`:** March's `CX2BuffChangeStatBehaviorTemplet::SetFactor` (`X2BuffBehaviorTemplet.cpp:896`, the `ADJUST_BUFF_ACCMULATION_CALCULATE` branch, which `Always.h:639` defines), with three differences:
  1. `ct != m_eChangeType || m_eChangeType == BCT_RELATION_VALUE` → false. There is no relation path.
  2. The value comes from `32e77f40`.
  3. No popups: a zero multiplier returns false silently.

  Regist (`unit->GetEnchantResist( T->GetRegistType() ) / 500`, with `32369eb0` reading UCM 2) is computed and passed, but never used.
- **`GetFactor` `32e7eb90`:** `[ m ]`.
- **Packet `32e77ef0`:** `m = v[0]; InsertAdditionalStat( type, ct, kBuffFactor.m_BuffIdentity, m )`.
- **`DoFinish` `32e75720`:** `EraseAdditionalStat( type, ct, identity )`.
- Slot 7 is the base (false).
- **Stores**, read from the three switches (insert `32282700`, erase `322830f0`, update `3226fd70`):

  | Type | Member | Insert | Erase | Update |
  |---|---|---|---|---|
  | 80 | PhysicAttack `+0x4384` and second `+0x44ec` | shares case 7 | shares case 7 | own block, same as 7 |
  | 81 | MagicAttack `+0x4438` and second `+0x45a0` | shares case 8 | shares case 8 | own block, same as 8 |
  | 82 | PhysicDefence `+0x4654` | own block, same as 9 | own block, same as 9 | shares 9 |
  | 83 | MagicDefence `+0x4708` | own block, same as 10 | own block, same as 10 | shares 10 |
  | 84 | MaxHp `+0x42d0` | own block: insert, update, `GetGageData()->SetMaxHp( result )`, i.e. March's MAX_HP case (`X2GameUnit.cpp:11745`) | own block, the same shape (March's erase case) | shares 6 (March's update, no `SetMaxHp`) |

  So 82–84 are March's cases 9, 10 and 6. 80 and 81 follow whatever 4b ports for 7 and 8, since June's 7/8 also write the second member.

**88 IGNORE_DEFENCE — class, store and reader ready; the host is Opus-only.**
- **Class** (vt `33535558`, ctor `32e890b0`): `m_fIgnoreDefenceRate` at `+0x10`, a `KProtectedType<float>` that the ctor sets to 1.0.
  - `SetFactor` `32e7d260`: `if ( !DidStart() ) { m = v.size() ? v[0] : 0; unit->InsertIgnoreDefence( identity, m ); } return true;`. There is no restack.
  - Packet `32e7d3f0`: the same, without the `DidStart` test.
  - `GetFactor` `32e7ec40`: `[ m ]`. `DoFinish` `32e75b00`: erase.
- **Store:** list `+0x67dc`; element (0x1c, ctor `322afc00`) = identity + protected rate. Insert `32282390`, erase `3227a8c0`.
- **Reader `32274970`** (`GetIgnoreDefenceRate()`): `min( Σ rate, 1.0 )`.
  - Its only callers (raw scan) are `GetFinalDamage` `@322856f8`/`@32285716`, called on `this`, i.e. **the defender** (`[esp+0x20]` = `ECX` at `32284ae4`).
  - June data has one user: the debuff `BTI_DEBUFF_CUTTING_SWORD_IGNORE_DEFENCE` (`IGNORE_DEFENCE_RATE = 1`, ended by `BFT_HITTED_COUNT`).
- **Host:** June rewrote March's `fPiercingDefenceFactor` (`X2GameUnit.cpp:2330-2334`):
  ```
  bIgnore = g_pX2Game->GetGameType() == GT_PVP ? DD[+0x529] /*IGNORE_DEFENCE_ON_PVP*/ : DD.m_bIgnoreDefence /*+0x528*/;
  if ( DD.m_bPenetrate || bIgnore ) f = 0;
  else { r = g_pX2Game->IsPvpOrPve() /*322cc1a0: type 1 or 4*/ ? DD[+0x530] /*IGNORE_DEFENCE_RATE_ON_PVP*/ : DD[+0x52c] /*IGNORE_DEFENCE_RATE*/;
         f = max( 1 - this->GetIgnoreDefenceRate() - r, 0 ); }
  ```
  - The keys are confirmed at the stores in `SetDamageDataFromLUA` (`32632390`): `IGNORE_DEFENCE` → `+0x528` (`@3263c691`), `IGNORE_DEFENCE_ON_PVP` → `+0x529` (`@3263c863`), `IGNORE_DEFENCE_RATE` → `+0x52c` (`@3263ca14`), `IGNORE_DEFENCE_RATE_ON_PVP` → `+0x530` (`@3263cc40`), `IGNORE_DEFENCE_DECLINE` → `+0x534` (`@3263ce2b`).
  - **Defaults (miss branches):** `IGNORE_DEFENCE` and `IGNORE_DEFENCE_RATE` default to 0. Each `_ON_PVP` key copies its sibling when absent: `@3263c8a0` copies `+0x528` into `+0x529`, and `@3263cc7e` copies `+0x52c` into `+0x530`. So a PvP hit keeps March's `IGNORE_DEFENCE` behavior unless a script sets the `_ON_PVP` key.
  - March parses only `IGNORE_DEFENCE` (`X2DamageManager.cpp:511`).
  - The frozen-debuff block just above (`32285611`–`3228568a`) changed too: June multiplies each `m_*DefenceDebuffPercent / 100` by `1 − GetEnchantResist( 2 ) / 500`, which March (`:2311-2321`) does not. That is not a buff change.

**93 CHANGE_EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER — transcription-ready.**
- **Class** `CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner` (vt `33534ce4`): ChangeStat plus a `UidType` owner at `+0x20`.
  - `SetFactor` `32e78850` is a `jmp` to ChangeStat's `32e78410`. Slots 3, 6, 7, 8 and 12 are ChangeStat's.
  - ChangeStat's `SetFactor` calls slot 15 after its type checks: `SetBuffOwnerUID( BuffFactor_ +0x30 )` (`@32e784a9`–`32e784b8`). `+0x30` is the applier's UID, stored by `SetGameUnitBuffFactor` (`32d4d670`).
  - Slot 15 is empty in ChangeStat (`33093ad0`); here it is `32e86510`. Slot 14 `32ec60c0` returns the UID.
- **Store and readers are 42's.** Insert and erase share 42's blocks (`32282d0a`, `322835ce`). Update has its own block (`32270afb`), which touches the same six members `+0x5e4c`…`+0x61d0` in the same order as 42's (`322706ad`).
- **`GetFactor` `32e7d7f0`:** `[ m, uid & 0xffff, (uid>>16) & 0xffff, (uid>>32) & 0xffff, uid>>48 ]`.
- **Packet `32e76710`:** inserts `m = v[0]`, then rebuilds the UID from **`v[4]..v[7]`** (low word first).
  - This is a June index bug: the words are written at indices 1–4 and read from 4–7, so peers get a garbage owner.
  - Transcribe it verbatim; it is harmless, because the UID's only consumer is unreachable (next bullet).
- **The owner UID's only consumer is dead code.**
  - `3227c620( { BTI, perOwnerLimit, bMin } )` groups the unit's `_HaveBuffOwner` behaviors for one BTI by owner. It sums each owner's results up to the limit and returns the smallest (or largest) sum.
  - Its only caller is `32758790` = `CX2GUNPC` slot 183, which passes `{ BTI_DECREASE_ENEMY_RESIST 761, 10, true }` for attributes 1–6. The base and user slot 183 is `322b5520`.
  - Nothing in `.text` calls slot 183: there is no `call [reg+0x2dc]` and no `mov reg, [reg+0x2dc]` followed by `call reg`.
  - The TypeDescriptor `33733b88` has exactly one code reference, `3227c720` inside `3227c620`.
  - **Port 93 as ChangeStat plus the UID member.** The aggregator can be skipped as unreachable; the port should say so.

**78 TYPE_ADDITIONAL_ATTACK_MULTIPLIER — store and reader context mapped; the host is Opus-only.** June added a four-term multiplier and a new gate to March's additional-attack block.
- **DD `+0x470` = `optrDefenderGameUnit`.** It sits directly before `m_bGuard` (`+0x478`, mapped in item 2), as in March (`X2DamageManager.h:990`).
- **User reader:** `CX2GUUser::DamageReact` (`320462a0`) `@32049466`–`32049561`, against March `X2GUUser.cpp:9276-9291`. Registers: `EBP` = this, `EBX` = DD, `ESI` = attacker (`DD+0xc`), `EDI` = defender (`DD+0x470`); DD `+0x57c` = `m_bLvDamage`.
  ```
  if ( !DD.m_bLvDamage && attacker ) {
      k = attacker->Opt5248() + this->Get78() + this->Opt5194();
      if ( defender && defender->Opt5ac8() > 0 ) k -= defender->Opt5ac8() * 0.01;
      k = max( k, 0 );
      v = k * attacker->GetAdditionalAttackDamage( pDD );   // slot 149 (+0x254); user 31fc7c20 -> 32291c00
      if ( v > 0 ) { dmg += v; if ( m_bDoubleAttack ) dmg += v; }
  }
  ```
  - March has the same block without `k`.
  - One block up, June also gates March's `GetAdditionalDefence` reduction (`:9267-9271`) on `!DD[+0x534]` (`IGNORE_DEFENCE_DECLINE`).
  - `32291c00` reads BBT 20's result (`+0x5018`) and calls `pow`; its body was not diffed.
- **NPC reader:** `CX2GUNPC::DamageReact` (`327b1f30`) `@327b42b0`–`327b43be` computes the same formula inline, except that it subtracts the defender term whenever the defender exists (there is no `> 0` test).
- **Where the three extra terms come from.**
  - `Opt5194`, `Opt5248` and `Opt5ac8` (`32071af0`, `32071b10`, `32071b90`) are option values. They are set only by `InitAdditionalStatOptionValue` (`32270dd0`), from June socket-data fields `+0xc8`, `+0xcc` and `+0xd4` (the socket data is at unit `+0x87b4`). The siblings are `+0xc4` → `+0x50e0` and `+0xd0` → `+0x5a14`. They are reset to 0 at `3227131c`–`32271366`.
  - March's `InitAdditionalStatOptionValue` (`X2GameUnit.cpp:12716`) ends with AdditionalAttack, AdditionalDefence and HyperCharge, and has no such members or socket fields.
  - **So in a March build the three terms are 0, and `k` is 78's result (basic value 1.0).** The Opus session may port `k = max( Get78(), 0 )` as a deviation that is exact on March data, and record it. The socket keys behind `+0xc4`…`+0xd4` were not identified.

**106 / 107 FORCE_DOWN_MULTIPLIER_MELEE / RANGE_ATTACKED — transcription-ready (user only).**
- **Reader:** `CX2GUUser::DamageReact` `@3204a118`–`3204a243`, against March's `SERV_SKILL_NOTE` block (`X2GUUser.cpp:9801-9812`):
  ```
  fForceDownValue = DD.fForceDownValue;          // +0x3f0, key FORCE_DOWN (store @3263892e)
  dec = DD.m_fDecreaseForceDown;                 // +0x514
  if ( defender /*DD+0x470*/ ) dec -= DD.m_bMeleeAttack /*+0x2c, key MELEE_ATTACK @32633030*/ ? defender->Get106() : defender->Get107();   // new in June
  fForceDownValue -= dec;
  if ( DD.m_bPenetrate ) fForceDownValue = fForceDownValue < 0 ? fForceDownValue - 10 : -10;   // March ENCHANT_BALANCE_101014
  ```
  Only this region of the host was diffed.
- The getters `32071bb0`/`32071bd0` have one caller each (raw scan), and nothing reads their results (`+0x5c1c`/`+0x5cd0`) inline, so **NPCs ignore 106/107**.
- The basic value is 0.0, so with no buff active the result equals March's.

**92 CHANGE_MAXIMIZE — Opus-only; the roll is decoded.**
- `322788c0( attacker, arg, physicOrMagic, float* out )` (`this` = defender) is called twice from `GetFinalDamage`: `@32284c1f` (physic) and `@32284c2f` (magic). `*out` is pre-loaded with the attacker's PhysicAttack/MagicAttack result (`+0x4424`/`+0x44d8`).
  ```
  base = attacker main result (+0x4384 / +0x4438); range = attacker second result (+0x44ec / +0x45a0);
  if ( range > 0 ) {
      maxi = attacker->Get92();                 // +0x6d18 result
      base *= attacker->slot121( arg );         // 3208dd70 = 1.0 in GameUnit, GUUser, GUNPC (per-class overrides not checked)
      seed( this->slot113( 0 ) & 0xffff );      // user 3208e0a0 = word +0x80f8, NPC 327559f0 = word +0x8474 (counters, not mapped)
      s = normal( mean = base + range*maxi, sigma = range*0.5 );   // 322ab740 / 322ab1c0 on a ~5 KB engine (likely mt19937)
      *out = clamp( s, base - range + 2*range*maxi, base + range );
  }
  ```
- **The range basics come from June's stat block (correction).**
  - `322814a0`/`32281690` (`CX2GameUnit` slots 120/119) set seven basic values (`3236ca50`, the protected float at member `+4`) from a stat struct: `{ float hp; int atkPhysic, atkMagic, <new>, <new>, defPhysic, defMagic }`. The new `+0xc`/`+0x10` fields go to `+0x44ec`/`+0x45a0`.
  - Where June's stat calc fills those two fields was not traced.
  - User-summoned NPCs copy the owner's range and MAXIMIZE results (`3230ca8c`–`3230cae3`, `32763d6e`–`32763dc5`).
- So porting 92 means porting June's stat-level attack range, the second members (which 4b-i lists with BBT 7/8), this roll and its `GetFinalDamage` call sites.

**Hand-offs from item 3.**
- **4b:**
  - June's BBT 6 insert (`32282723`) and erase (`32283113`) rescale now-HP to keep the HP ratio (`31e4f900`/`31e551a0`, gage `32081260`). 84's blocks don't, and March's 6 doesn't.
  - The BBT 7/8 second members (already on 4b-i part 2's list).
- **No owning phase yet.** These are non-buff DamageData, game-type and socket changes. The Opus host work for 78 and 88 has to decide them, or hand them to the user:
  - the keys `IGNORE_DEFENCE_ON_PVP`, `IGNORE_DEFENCE_RATE(_ON_PVP)` and `IGNORE_DEFENCE_DECLINE`;
  - the resist-scaled frozen-defence debuff;
  - `GT_PVE`;
  - the socket-option terms in 78's multiplier.

**Item 3 readiness.**
- **Transcription-ready:** 79, 82, 83, 84, 93, 106, 107.
- **Ready except the second member** (follows 4b's BBT 7/8): 80, 81.
- **Opus-only host:** 78 (the multiplier block), 88 (the `fPiercingDefenceFactor` rewrite and four DamageData keys), 92 (the variance system).

### Phase 1c, item 5 — `CreateAndInsertCustomFunctionByBuff` (2026-10-03)

Read-only Ghidra, same method. This is Phase 9's entry map; the hook bodies of each class stay Phase 9's.

**Behavior class** `CX2BuffCustomFunctionBehaviorTemplet` (vt `33535288`).
- `SetFactor` `32e79b00`: `if ( !DidStart() ) { m_kBuffBehaviorFactor = *pBF; 3229e1a0( T->GetBuffIdentity(), *pBF, info ); } return true;`. A missing factor returns false with no popup. This is March's (`X2BuffBehaviorTemplet.cpp:2094`) plus a third argument:
  - `info = { bool bNotUser = !factor[+0x38], UidType creator = factor[+0x30], T[+0xbc] }`.
  - `+0x30`/`+0x38` are the applier's UID and is-user flag that `SetGameUnitBuffFactor` (`32d4d670`) records.
- Packet `32e73f90`: when not started, it builds the same `info` from the `KBuffFactor` (`+0x28`/`+0x2c` UID, `+0x3d`, `+0x57`) and calls `3229e1a0( kBuffFactor +0x20, values, info )`.
- `DoFinish` `32e73fe0`: `if ( DidStart() ) EraseCustomFunctionByBuff( identity )` (`3227a1d0`), as March's (`:2145`).
- `GetFactor` `32e7e000`: copies the stored factor, as March's.

**`CX2GameUnit::CreateAndInsertCustomFunctionByBuff` = `3229e1a0`** (March `X2GameUnit.cpp:13729`, 3 cases).
- It switches on `identity.m_eBuffTempletID`. Each case `new`s a class and `push_back`s it (`322ad1e0`) onto the unit's custom-function list.
- Classes were resolved through the ctor, its vtable store and the vtable's RTTI name. BTI names are from June's `Enum.src.lua`.

| BTI | Ctor | Class | Gate (June) |
|---|---|---|---|
| 29 `BTI_SI_SA_ETK_PHANTOM_SWORD` | `3236eee0` | `CX2CustomFunctionByBuffPhantomSword` | – (March has it) |
| 56 `BTI_BUFF_UNFIXED_CLIP` | `322ab4e0` | `CX2CustomFunctionByBuffBigBangStream` | **user only** (`unit+0x264 == 0`; March has no gate). It passes a skill-level value (`31fcf430`, `32007b10` → `UserSkillTree::GetSkillLevel` `32890620`). |
| 62 `BTI_DEBUFF_MARK_OF_COMMANDER` | `322aad80` | `CX2CustomFunctionByBuffMarkOfCommand` | – (March has it) |
| 223 `BTI_DEBUFF_PANDEMONIUM_FEAR` | `322aae50` | `CX2CustomFunctionByBuffPandemoniumFear` | – |
| 439 `BTI_DEBUFF_MOONLIGHT_RHAPSODY` | `322aaef0` | `CX2CustomFunctionByBuffMoonlightRhapsody` | – |
| 720 `BTI_BUFF_NITRO_SERVO`, 920 `…_UPGRADE` | `323698b0` | `CX2CustomFunctionByBuffServoNitro` | – (one class for both) |
| 767 `BTI_DEBUFF_SINISTER_MIND_SEED` | `322aaf80` | `CX2CustomFunctionByBuffSinisterMind` | `g_pX2Game->GetUserUnitByUID( info.creator )` must exist; skill-level value via `32007b10` |
| 819 `BTI_DEBUFF_TARGET`, 894 `BTI_DEBUFF_AIMING_STIGMA` | `3236f6b0` | `CX2CustomFunctionBySummonNpcTarget` | creator user must exist (819 jumps into 894's branch) |
| 891 `BTI_DEBUFF_SA_RCR_CHAIN_VIPER` | `3236f890` | `CX2CustomFunctionByChainViper` | creator user must exist |
| 927 `BTI_DEBUFF_ATOMIC_BLASTER_PLASMAGUN_UPGRADE` | `322a8e80` | `CX2CustomFunctionByAtomicBlaster` | – |
| 952 `BTI_DEBUFF_ATTACK_ME` | `3236fc50` | `CX2CustomFunctionByAttackMe` | **NPC only.** On any other unit June calls `3229e160( 952 )`, which erases the buff itself, and creates nothing. |
| 1010 `BTI_DEBUFF_GEIST_HERRSCHAFT` | `3236fd90` | `CX2CustomFunctionByGeistHerrschaft` | – |
| 1022 `BTI_DEBUFF_A_AAP_MODER_MARKING` | `32370210` | `CX2CustomFunctionByModer` | – |
| 1038 `BTI_BUFF_DEATH_SIGN` | `322a8e50` | `CX2CustomFunctionByBuffDeathSign` | – |

- **The creator check.** "Creator user must exist" is `CX2Game` slot 40 (`322cca00`) = `GetUserUnitByUID`: it walks `m_UserUnitList` and compares each unit's slot-21 `GetUnitUID` with the 64-bit UID. March has `CX2Game::GetUserUnitByUID`.
  - The UID passed is `info`'s creator UID (`info+8`/`+0xc`, packed by `32e79b00` from factor `+0x30`), not the unit's own. Checked at `3229e424`, `3229e485` and `3229e572`.
- **Four of the ledger's 17 "unhandled" BTIs have no case** and fall to `default` (nothing is created): `BLOOD_REVENGE_COUNTER`, `BLOOD_RUNAWAY`, `FLASH_BANG` and `TRAPPINGARROW_FUNGUS_DEBUFF`. Whatever they do in June lives elsewhere, most likely in per-class `HaveThisBuff` checks (March keeps FLASH_BANG in `X2GUAra.cpp:13854`). Phase 9 must look for them there, not in this switch.
- **`CX2CustomFunctionByBuffSolessTitleInvincible` (vt `3349011c`) is dead.** The vtable's only code reference is its ctor `32369b80`, which has no callers and no data references.
- **Vtables:** 9 slots, 10 for `BigBangStream`. `CX2CustomFunctionByAttackMe` overrides only its dtor: `[0]` `322aaf60`, `[1]`–`[5]` and `[7]` `33093ad0` (empty), `[6]` `33108560`, `[8]` `3208ddc0`.
  - Phase 4a's note places March's four hooks at slots 3–6 and June's new apply hook at slot 7 (caller `32275860`). Item 5 did not re-derive that.
  - Regenerate the per-class overrides with `buffre/vt.py CustomFunction`.

### Phase 1c, item 4 — Phase 8 finalizers (2026-10-03)

Read-only Ghidra, same method as items 1–3. Uncarved slots were read with `buffre/rawfn.py`. Callers come from a raw `E8`/`E9` scan of the whole `.text`, and virtual calls from a byte scan for `mov reg,[reg+disp]` followed by `call reg`, plus the `call [reg+disp]` form. Only the first form occurs for every slot below. Wire layouts marked *(asm)* were read in the disassembly, not the decompile.

**June finalizer vtable (14 slots; AURA 15, Time 16).** `[0]` dtor, `[1]` OnFrameMove(unit, dt), `[2]` SetFactor(factor, unit, templet), `[3]` GetClonePtr, `[4]` DidFinish(unit), `[5]` GetFactor(vec, unit), `[6]` SetBuffTempletID (base `3208ddc0`, empty), `[7]` anti-tamper checksum (decodes the protected type at `+0x14`; StickShake and UseSkill fold in their `+0x1c`; not ported), `[8]` RestartForAccumulationDecrease (Phase 4a), `[9]` new: `int(unit, creator)` (base `31cbc910` returns 0), `[10]` new: `UidType GetCreatorUID()` (base `32ec5980` returns 0; LiveCreator `32ec60c0` and AURA override it), `[11]` new bool (Time only), `[12]` ParsingFinalizer, `[13]` SetFactorFromPacket. The scan found no external caller of slot 10 or AURA's slot 14 in the buff code (`3226f000`–`322b6000`, `32360000`–`32372000`, `326b0000`–`326d0000`, `32d40000`–`32d50000`, `32e70000`–`32ed0000`). Every `+0x28` hit there is a unit call gated on `+0x172`.

**Shared fact.** `CX2BuffFactor +0x28` is the factor's `BUFF_TEMPLET_ID`: `ParsingBuffFactor` (`32d4d850`) stores it there right after reading that key (`@32d4d9d4`; `0` on a miss, `@32d4d9b7`).

**BFT 14 AURA — class decoded; Opus-only (its creator check and its emitter need June's SIEE 15 socket effect).**
- **Class** `CX2BuffAuraFinalizerTemplet` (vt `33537d94`, create `32ec6260`, 0x40 bytes, clone `32ec2170` → copy ctor `32ec60d0`, dtor `32ec61b0`). Members:
  - `m_iState` `+0x1c`;
  - `m_optrCreator` `+0x20`/`+0x24` (an observer pointer; `31e56b80` takes the unit's `+0x1a4` weak handle);
  - `m_fElapsed` `+0x28`;
  - `m_uidCreator` `+0x30`/`+0x34` (ctor -1);
  - `m_eBTI` `+0x38`.
- **ParsingFinalizer:** the base (`32ec5990`, true). The factor parse is the empty one (Phase 1), so `BFT_AURA = {}` carries nothing.
- **SetFactor `32ec51d0`** *(asm)*:
  ```
  if ( !pUnit ) return true;
  if ( !factor.GetFinalizerFactor( m_eType, &ff ) ) return false;
  m_uidCreator = factor[+0x30];  m_eBTI = factor[+0x28];          // applier UID, BTI
  if ( factor[+0x38] /*applier is a user*/ == 1 ) {
      pCreator = g_pX2Game->GetUserUnitByUID( m_uidCreator );       // CX2Game slot 40
      if ( pCreator ) { m_iState = slot9( pUnit, pCreator );
                        if ( m_iState == 0 ) { m_optrCreator = pCreator; m_iState = 1; } }
  }
  return true;
  ```
  An NPC applier leaves `m_iState` at 0.
- **Slot 9 `32ec13e0`** `CheckAura( pUnit, pCreator )`, returning 0, 1 or 2 *(asm)*:
  ```
  if ( !pUnit || !pCreator ) return 2;
  if ( !pUnit->IsMyUnit() ) return 1;                     // unit slot 102: GUUser 31ff1f30 = g_pX2Game->GetMyUnit()==this; GameUnit/NPC false
  if ( pCreator->GetNowHp() <= 0 ) return 2;              // 31e4f900: gage +0x20 -> protected fNow; 1.0 with no gage
  if ( !pCreator->HasSocketAuraFor( SIEE 15, m_eBTI ) ) return 2;     // unit slot 181, below
  if ( pCreator->GetUnitUID() != pUnit->GetUnitUID() ) {
      if ( !g_pData[+0x5c] ) return 2;
      if ( !g_pData[+0x5c]->32431360( creatorUID ) ) return 2;        // party membership (the same g_pData+0x5c check as 4a's 32045dc0)
      if ( pUnit->GetGameUnitType() != GUT_USER ) return 2;           // +0x264
  }
  if ( pUnit->HasFinalizerFactorWithCreator( m_uidCreator, BFT_AURA ) // unit slot 182, below
       && pUnit->322a0090( m_eBTI ) ) return 1;           // the BTI's KBuffFactor m_ucAccumulationCountNow (+0x34), 0 if absent
  return 0;
  ```
- **OnFrameMove `32ec1370`:** only when `pUnit->IsMyUnit()`: `m_fElapsed += dt; if ( m_fElapsed >= 0.5 ) { m_iState = slot9( pUnit, m_optrCreator ); m_fElapsed = 0; }`.
- **DidFinish `32ec5890`:**
  ```
  if ( !g_pX2Game || !pUnit ) return true;
  if ( m_iState != 2 ) return false;
  if ( pUnit->IsMyUnit() ) { ++pUnit[+0x6dd0]; pUnit[+0x6dd4] = 0; }   // the sync counter, below
  return true;
  ```
- **GetFactor `32ec4b10`** *(asm)*: `[ uid & 0xffff, (uid>>16) & 0xffff, (uid>>32) & 0xffff, uid>>48, (float)m_eBTI ]`. This is LiveCreator's `FACTOR_ORDER` packing (`X2BuffFinalizerTemplet.h`, back-down word first), with the BTI in place of `FO_IS_USER_UNIT`.
- **SetFactorFromPacket `32ebe7f0`: empty (`ret 8`).** A peer's copy keeps `m_iState = 0` and never finishes by itself. Only the owner's resync removes it (see the `+0x6dd0` section).
- Slot 10 `32ec6150` returns `m_uidCreator`; slot 14 `33180f80` returns `m_eBTI` (used by slot 9).
- **Unit slots it calls (new in June).**
  - Slot 181 (`+0x2d4`): GameUnit/NPC `330fb7d0` false. GUUser `3204e380( siee, bti )`:
    ```
    it = m_mapSocketItemEffectEx.find( siee );            // +0x8b64 = March m_mapSocketItemEffectEx (X2GUUser.cpp:30461)
    if ( it == end || siee != 15 ) return false;
    for each socketId in it->second:
        pData = g_pData[+0x280] /*CX2SocketItem*/->GetSocketData( socketId );   // 32715320: map find, node +0x10
        for each e in pData[+0x29c..+0x2a0] (16 bytes): if ( e.byte9 == 1 && GetBuffFactorPtr( e.id )->BTI(+0x28) == bti ) return true;
    return false;
    ```
  - Slot 182 (`+0x2d8`): GameUnit/NPC `31ae1500` false. GUUser `32039bf0( uid, bft )`: walks `GetBuffFactor()` (`3229fd80`). For each finalizer factor of type `bft`, it rebuilds `v0 | v1<<16 | v2<<32 | v3<<48` and returns true on a match.
- **The emitter.** The aura buff is applied by `CX2GUUser::ProcessSocketEffectEx` (`32028ba0`; March `X2GUUser.cpp:30479`, which stops at `SIEE_ACTIVE_BUFF` 11).
  - June adds cases 12–20, and case 15 `SIEE_ALWAYS_PLAY_EFFECT_SET` (`32029fff`) does this: for each socket ID in `map[15]` whose data has an `EFFECT_NAME` list (`+0x1fc`), if the interval check passes (`m_fIntervalTime` `+0x1e8` against unit slot 115, and `31fd8b70( id ) > k`), it plays each effect set (`g_pX2Game` → `322ceae0` → `321e1e60`) and attaches every entry of the `BUFF_FACTOR_ID` list (`+0x29c`, flag byte 9) to it (`321e3650`). It then restarts the cooldown (`31fe0b90( id, +0x1f8 )`).
  - The caller is `CX2GUUser` slot 484 (`3206d770`) with `push 0xf` (`@3207012a`), once per frame.
  - The map is filled by `31fe4730` (March's insert at `:30461`) from the equip routine `3204f190`, keyed by socket data `+0x1dc` (the SIEE type), and cleared by `3204b4c0`.
- **Data.** All 10 June `BFT_AURA` buffs are socket options in `OptionItemData.src.lua` (`AddSocketData_LUA`, `m_eTypeSetItemEffectEx = SIEE_ALWAYS_PLAY_EFFECT_SET`, `EFFECT_NAME = { "EffectSet_Hero_Dungeon_Aura_Buff" }`, `BUFF_FACTOR_ID = …`). `BTI_BUFF_PHASE_OF_PERKISAS` is also in `DamageEffect.src.lua:351076`. March's `Enum` has no `SIEE_ALWAYS_PLAY_EFFECT_SET`.
- **Verdict.** The class, slot 9 and both unit slots are transcribable. But the creator check (slot 181) and the only emitter both need June's SIEE 12–20 socket-effect cases and the socket-data fields `EFFECT_NAME`, `m_fIntervalTime` and the `BUFF_FACTOR_ID` list, which March's `CX2SocketItem` lacks. **Opus-only; no owning phase yet** (Phase 8 plus a socket-option port, or the user decides).
  - **Lifetime, by case** (slot 9's `IsMyUnit` return `@32ec140f` comes before slots 181/182):
    - **A target that is not the local unit** (an NPC, or a remote user's copy): slot 9 returns 1 at SetFactor, and OnFrameMove never re-checks it. It never ends through `DidFinish`; only the sync removes it (see `+0x6dd0`).
    - **An NPC applier, on the local unit:** `m_optrCreator` stays null, so the first 0.5 s check passes null (`@32ec13ab`-`13bf`), slot 9 returns 2 and the aura ends.
    - **A class-only port** (slot 181 always false), user creator, local unit: SetFactor already stores state 2, so the aura ends at the first `DidFinish` poll.

**BFT 15 STICK_SHAKE — class, counter and increment site decoded; Opus-only (June added a new input counter and new unit virtuals).**
- **Class** `CX2BuffStickShakeCountFinalizerTemplet` (vt `33537c68`, create `32ec5e10`, 0x28 bytes, clone `32ec1500`). Members: `m_eCountType` `+0x1c`, `m_iTarget` `+0x20`, `m_ucSnapshot` `+0x24`, `m_bStarted` `+0x25`.
- **ParsingFinalizer `32ebffe0`:** `BUFF_USE_COUNT_TYPE` → `+0x1c`. **Required:** a missing key stores 0 and returns false (AL is the lookup's 0 at `32ec0178`; `mov al,1` only on the hit path).
- **SetFactor `32ec1e00`** *(asm)*:
  ```
  if ( !pUnit || !factor.GetFinalizerFactor( m_eType, &ff ) ) return false;
  if ( (int)ff[0] != m_eCountType ) return false;
  pUnit->BeginStickShake();                                   // unit slot 256 (+0x400)
  c = (BYTE)pUnit[+0x75a8];
  if ( !m_bStarted ) { m_ucSnapshot = c; m_bStarted = true; }
  n = NeededCount( ff, pUnit );                               // 32ec1620
  if ( m_iTarget - c <= n ) m_iTarget = c + n;
  return true;
  ```
- **`NeededCount` `32ec1620( ff, pUnit )`:** `!pUnit` → 0. If `ff[2] > 0` return `trunc( ff[2] )`. Otherwise switch on `m_eCountType`:
  - `BUCT_NOT_USE` (0): `trunc( ff[1] )`;
  - `BUCT_MY_BUFF_COUNT` (1): `trunc( pUnit->32281480( false ) * ff[1] )`;
  - `BUCT_MY_DEBUFF_COUNT` (2): `trunc( pUnit->32281480( true ) * ff[1] )`;
  - the enemy types 3/4: 0.

  `32281480( bDebuff )` counts the unit's templets (`+0xe14`) whose `IS_DEBUF` equals the argument. The level slice gives `[type, COUNT[level], 0.0]`, so `ff[2]` is normally 0. All June data is `BUCT_NOT_USE`.
- **OnFrameMove `32ec01a0`:** `if ( pUnit && m_bStarted && m_iTarget >= 256 && (BYTE)pUnit[+0x75a8] < m_ucSnapshot ) m_iTarget -= 256;` (byte wraparound).
- **DidFinish `32ec01d0`:** `pUnit && m_bStarted && ( m_iTarget <= pUnit[+0x75a8] /*the full dword*/ || m_bReserveToFinish /*+0x18*/ )`.
- **GetFactor `32ec4c50`:** `[ (float)m_iTarget, (float)m_ucSnapshot ]`.
- **SetFactorFromPacket `32ec1580`** *(asm)*: `m_iTarget = (int)v[0]; m_ucSnapshot = (BYTE)trunc( v[1] ); m_bStarted = true; if ( (BYTE)pUnit[+0x75a8] < m_ucSnapshot ) m_iTarget -= 256;`. It does **not** call slot 256.
- **The counter (new in June).** `pUnit[+0x75a8]` is an int (`inc dword`), next to a reference count at `+0x75a4`:
  - Slot 256 (`+0x400`) `BeginStickShake`: base/NPC `322b5580` `++ref`. GUUser `32011130`: `++ref`, clears the two direction latches (`+0x9498` word = 0), and on the own unit with no prompt playing (`+0x949c == -1`) plays the stick-shake prompt effect and stores its handle.
  - Slot 257 (`+0x404`) `EndStickShake`: base/NPC `322b5590`: `if ( ref > 0 ) --ref; if ( ref == 0 ) count = 0`. GUUser `32011210` does the same and destroys the own unit's prompt. Its only caller is the BFT 15 finish hook `3236a6a0` (`@3236a6c2`), which also bumps `+0x6dd0`.
  - `31fc9f90` `IsStickShaking() = ref > 0`.
  - **Increment** `31fc9fa0( bLeft, bRight )`: `if ( bLeft && !bRight && !latchL ) { latchL = 1, latchR = 0; ++count; } else if ( bRight && !bLeft && !latchR ) { ++count; latchR = 1, latchL = 0; }`. Its only caller is `322f7170` (a `CX2Game`-side input routine, not named here) `@322f74da`, run only while `IsStickShaking()`, with two key states read from the input manager (`[0x337676e8]+0x1f8`, codes `0x25`/`0x26`).
- **Host diff.** March's grab escape (`X2GUUser.cpp:14651`, `TEST_GROUP_GRAP`) uses the same alternating latch, but on `m_iGrapShakeCount` (counting **down**, fed by NPC `GRAP_SHAKE_COUNT`) inside the user's own input code. June's counter counts up, is reset by slot 257, is driven from `CX2Game`, and its slots 256/257 are new virtuals. That is a new mechanism, not a buff term in a March function. **Opus-only.**
- **Data.** Two June buffs: `HOLD_WITH_STICK_SHAKE` (NPC-applied), `COUNT = 4` at every level.

**BFT 16 USE_SKILL_BY_SKILL_SLOTTYPE — class, counter and increment site decoded; Opus-only (the host is June's `CommonStateStart`).**
- **Class** `CX2BuffUseSkillBySkillSlotTypeFinalizerTemplet` (vt `33537ca4`, create `32ec5e60`, 0x2c bytes, clone `32ec17c0`). Members: `m_eCountType` `+0x1c`, `m_eSlotType` `+0x20`, `m_iTarget` `+0x24`, `m_ucSnapshot` `+0x28`, `m_bStarted` `+0x29`.
- **ParsingFinalizer `32ec0240`:** `BUFF_USE_COUNT_TYPE` → `+0x1c`, then `SKILL_SLOT_TYPE` → `+0x20`. **Both are required** (false at `32ec03d7` and `32ec051f`).
- **SetFactor `32ec1eb0`, OnFrameMove `32ebe800`, DidFinish `32ec0540`, packet `32ec1840`:** BFT 15's bodies, with one change: the counter is `(BYTE)pUnit->GetSkillUseCount( m_eSlotType )` (unit slot 258) in place of `+0x75a8`. SetFactor does not call slot 256. `NeededCount` is `32ec18f0`, which has `32ec1620`'s shape (not diffed byte for byte).
- **GetFactor `32ec4d20`:** `[ (float)m_iTarget, (float)m_ucSnapshot ]`.
- **The counter (new in June).** A unit `map<int, int>` at `+0x75c8`, accessed through three new virtuals shared by every unit class:
  - slot 258 `3208ed20` `Get( k ) = map[k]`;
  - slot 259 `3208ed40` `Set( k, v )`, with no caller found;
  - slot 260 `3208ed60` `Add( k, v )`.
- **Increment: `CX2GUUser::CommonStateStart`** (`32057b80`, slot 497; all 12 class overrides call it). The slot order 495–504 matches March's header (`DyingStart`, `CommonStateStartFuture`, `CommonStateStart`, `CommonFrameMoveFuture`, `CommonFrameMove` = 499, …, `DetonationEventProcess` = 504).
  - Inside the special-attack block at the top (`if ( m_iReservedSkillState(+0x8eac) == GetNowStateID() && pSkill && !slot516( pSkill ) )`, followed by the MP consumption):
    ```
    if ( !slot303() && GetNowStateID() > 0 && 32070580( pSkill ) ) {
        AddSkillUseCount( 0, 1 );                                  // key 0: any skill
        if ( userSkillTree.FindEquippedSlotIndex( GetNowStateID(), &slot ) )   // 3288fff0
            AddSkillUseCount( SlotIndexToSlotType( slot ) /*3214f3d0*/, 1 );    // keys 1-3: SKILL_SLOT_A/B/TC
    }
    ```
    The call sites are `@32057d17` and `@32057d7c`.
  - March's `CommonStateStart` (`X2GUUser.cpp:13776`) begins with Lua state reads. June's begins with this skill block, so the host is a June rewrite at that point. **Opus-only.**
  - March's `CountUsedSkill` (`:32138`) has no callers and is unrelated (slot A/B arrays for the dungeon rank).
- **Hand-off answer for 98 (item 1).** `DoStateEndStart` (`3203c3a0`) caches 98's rate at `@3203c649` and then calls the new state's start delegate or `CommonStateStart` (`@3203c95c`–`3203c978`). So **BFT 16 counts after the cache**, and the `TRANSCENDENTAL_SLOT` buff applies to the skill that ends it.
- **Open.** Whether `+0x8eac` is written only by local input (then remote copies never count) was not checked. Run `dispxref.py 0x8eac`.

**The `+0x6dd0` sync counter (UDP).**
- **Bumped by:** `PushNewBuffTempletToGameUnit` (`3229dfc0`), `OnFrameMoveBuff` (`3229d759`), the BFT 15 finish hook (`3236a6cc`) and AURA's `DidFinish` (`32ec58f4`). Each does `++[+0x6dd0]` and sets `[+0x6dd4] = 0`, so it is a 64-bit pair. The ctor zeroes it (`322a5387`). Setter `32070c90` `SetBuffSyncCount( v )`.
- **`CommonFrameMoveFuture`** (slot 498, `32061bf0` `@32061d4e`): `if ( (u64)field7e68 != counter ) field7e68 = counter`. This sits where March writes `m_FrameDataFuture.syncData.ucNumOfDeBuff = GetNumOfDeBuff()` (`X2GUUser.cpp:14923`).
- **`CommonFrameMove`** (slot 499, `32063650` `@3206386c`). The guard is only `g_pX2Game->GetMyUnit() != NULL` (`@3206384d`), with no comparison against `this`, so it runs for **every user unit, remote copies included**:
  ```
  if ( t9448 <= 0 && field8108 != counter ) t9448 = 1.0;
  else if ( field8108 == counter ) t9448 = 0;
  if ( t9448 > 0 && ( t9448 -= dt ) <= 0 ) {
      SetBuffSyncCount( field8108 );
      g_pData[+0x140]->33224850( this->GetUnitUID() /*destination*/, 0x1b, &myUnit->GetUnitUID() /*payload*/, 8 );   // not decoded
      [+0x9541] = 0;
  }
  ```
  `+0x7e68`/`+0x8108` look like the same sync field in `m_FrameDataFuture`/`m_FrameDataNow` (0x2a0 apart), but that was not proven.
- **March** syncs `m_ucNumOfDeBuff` beside hit/hitted count (`X2UDPPacket.h:312`, flag `eUnitUserSync_DataField_NumOfDeBuff` `0x4000`), and peers apply it at `X2GUUser.cpp:17443`. June appears to repurpose that field as a buff-change counter. **This block is the observer side:** when a unit's synced value (`+0x8108`) has disagreed with the local counter for 1 s, the client adopts the synced value and sends message `0x1b` to that unit's owner with its own UID. Neither AURA (empty packet slot) nor BFT 15 (the packet path skips slot 256) finishes by itself on a remote copy, so the owner's reply is what removes them there. **Still open (Phase 8 blocker):** where the received field lands in `+0x8108` for a remote unit, who handles `0x1b` on the owner and what it sends back, and how that reply removes a finished AURA or BFT 15 on the peer.
- **Search plan:** find June's unit-sync serializer through the `0x4000` field flag (field width, and where it is applied to `+0x8108`); find the `0x1b` handler on the receiving side (and its March XPT name) and what it replies with.

**Corrections.**
- **User slot 476 (`+0x770`, `32028250`) is not BFT 15/16 scaling** (Phase 4a RE, Phase 8 hand-off, `funcmap.tsv`).
  - It takes `( skillId, factor )`; the caller `322a6960` passes the factor's skill or the unit's `+0x9274`. It looks up the user's skill-memo entry (`31fdbbb0`, the same lookup row 101 describes).
  - Entry type **15** sets `BFT_TIME`'s `v[1]` (the duration) to `v[1] * value` (`32d41910( BFT_TIME, value, false )`).
  - Type **16** rescales every behavior factor's values (`32d41820`); the formula was read only from the decompile, whose x87 operands are noise, so it is **not decoded**.
  - It belongs to the skill-memo work (Phase 10, with `_ULTIMATE`), not Phase 8.
- **`32057b80`** is `CX2GUUser::CommonStateStart` (slot 497), not "unit/NPC state script parser?" (`funcmap.tsv` row 46).
- **Phase 8's candidate hooks** (`AttackResult` `:12721`, `CommonSpecialAttackEventProcess`/`CountUsedSkill`, `m_iGrapShakeCount`) are superseded by June's real sites: `@32057d17`/`@32057d7c` for BFT 16 and `31fc9fa0` via `322f7170` for BFT 15. `BFT_ATTACK` stays unimplemented (Phase 1).

**Item 4 readiness.**
- **Opus-only, all three.**
  - AURA needs June's SIEE 15 socket effect and socket-data fields (no owning phase).
  - BFT 15 needs a new counter, slots 256/257 and the `CX2Game` input hook.
  - BFT 16 needs a new map, slots 258–260 and an edit inside June's rewritten `CommonStateStart`.
- The class bodies, wire layouts and increment pseudocode above are complete. The open items are the `0x1b` handler and its reply (blocks Phase 8's sync design), `+0x8eac` locality, `32ec18f0` byte equality, and naming `322f7170`.

### Phase 1c — progress (2026-10-03)

- **Item 1 done** (above), committed in two steps after an advisor review corrected the readiness split.
- **Item 2 done** (above, second 1c session). It also corrected Phase 1's 77, 110 and 104 rows and named slot 499 (`CommonFrameMove`), `3276cb20` (`CommonEventProcessNonHost`) and `3276d980` (`CommonEventProcess`).
- **Item 3 done** (above, third 1c session). It corrected Phase 1b's 78 getter members and the premise that 92's second members are filled only by buffs.
- **Item 5 done** (above, same session as item 3).
- **Item 4 done** (above, fourth 1c session). It found all three types Opus-only, corrected Phase 4a's slot-476 note and row 46, and answered item 1's 98 hand-off (BFT 16 counts after the cache). Still open: the `0x1b` handler and its reply, which block Phase 8's sync design, and `+0x8eac` locality.
- **Cost, fourth 1c session (item 4)** (subscription, `/usage` at the end of the session, 2026-10-03): the 5-hour session window was **64% used**, and the week (all models) 11%. The window was shared with other sessions (19 sessions in 24h), so not all of it is item 4. `/ghidra` accounted for 42% of the last 24h, and 90% of usage was at >150k context.
- **Cost, third 1c session (items 3 and 5):** not readable in-session; ask the user.
- **Item 5 is reduced.** Phase 4a already named `322a6960`'s virtuals (`+0x210` `CanApplyBuffToGameUnit`, `+0x224` `SetBuffFactorToGameUnit`, `+0x234` empty, `+0x770` BFT 15/16 scaling) and `OnFrameMoveBuff` (`3229d620`). Only `CreateAndInsertCustomFunctionByBuff` remains (from `CX2BuffCustomFunctionBehaviorTemplet::SetFactor` `32e79b00`).
- **Cost:** `/cost` is not readable from inside the session. Ask the user and add it here, for both the first 1c session and the second.

**Phase 1c is complete** (items 1-5). Open RE for the Opus sessions that pick these types up: the `0x1b` handler and reply (Phase 8), `+0x8eac` locality (BFT 16), `32ec18f0` byte equality, naming `322f7170`, and the slot-476 type-16 formula (Phase 10).

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

### Phase 2 — Done (2026-10-03)

**Lua (`march2014/data036/Enum.src.lua`, compiled `Enum.lua`).**
- Added every June member March lacked, at June's values, sorted by value in one `--{{ Iruha … --}}` block at the end of each table: BTI 705, BFI 734, BBT 38, BFT 3. The 6 new tables (`BUFF_APPLY_CHECK_CONDITION_TYPE`, `BUFF_GROUP_APPLY_DEFAULT_VALUE`, `BUFF_GROUP_ID`, `BUFF_REPLACE_DETAIL_OP`, `BUFF_VALUE_CALCULATE_TYPE`, `SPA_BUFF_TYPE`) sit right after `BUFF_FACTOR_ID`. `AT_COMMAND` was already in `ATTACK_TYPE` (effect plan Phase 11).
- Renumbered in place, each with a comment naming the old value: `BTI_BUFF_HAMEL_SECRET_DUNGEON_ARMOR/WING_EVENT` 1321/1322 → 347/348, and the three potion BFIs 362–364 → 481–483.
- Free-value check: comma-agnostic (`= N,?\s*$` scoped to each table) plus a scripted collision check. The only values June's new members take from March members are the 5 that June itself renumbered away.
- **Deliberate deviation: `BFI_DEBUFF_TELEPORT_CONSUME_MP` = 290 is kept as an alias** beside June's `BFI_DEBUFF_TELEPORT_CONSUME_MP_PVP` = 290. March's live `BattleFieldBuffFactor.lua` names it. Simulating `AddBuffFactor_LUA`'s ordinal rule on March's live factor file shows that removing it would drop factors 290–364, **77 of them**. Remove it in Phase 13 (below).
- Verified: every BUFF table now equals June's (0 new, 0 changed). The one March-only member is the alias. The compiled file has the `KL\x81` mar-2014 header, and decompiling it back gives the same 316 tables and 10,785 members as the source.
- Staged byte-identical in `march2014/data036/Enum.lua` and `kom_contents/data036_kom/Enum.lua` (md5 `81f0b944…`), with the `.src.lua` beside each. The manifest already listed `Enum.lua`, so it is unchanged. The new Enum was merged into the previously staged one (Lanox/June-effect work), not swapped for it.

**What March's current data does with this Enum** (measured on March's live factor bytecode, decompiled to scratch, not committed):
- Before this phase, 364 of 366 factors load. `BFI_BUFF_HAMEL_SECRET_DUNGEON_ARMOR/WING_EVENT` (372/373) already fail the ordinal check, and **each launch already shows 2 `BUFF_FACTOR_ID invalid` MessageBoxes** (`State.log` records them; `DISPLAY_ERROR` is a real `MessageBox` here because `_SERVICE_` is not defined).
- After it, 361 load. The three potions fall out at their new ids, so **each launch shows 5 boxes until Phase 3** replaces them with log lines. They are the tail of the file, so there is no cascade.
- **`Item.kim` bakes factor ids as numbers.** Under `X2OPTIMIZE_ITEM_TEMPLET_PREPROCESSING` (ON) the client always loads the prebuilt `Item.kim`; no `Item.lua` exists anywhere. Item 85003830 (Weak Heretic potion) carries WORD 362 and 85003831 (Heretic potion) carries 363. This was checked by cross-reference: the `[count=1, id]` run sits at file offset 20107068 / 20107412, and it is pointed to from that item's record, +132 bytes from its item id. March's `Item.kim` has no Ruriel potion item, and no item references 372/373.
  - Until June's factor file lands, using either potion pops `uiBuffFactorNumber_ invaild`, and then **dereferences a null factor pointer** (`X2Game.cpp` `ApplyBuffFactorToGUUser`, the `UPGRADE_SKILL_SYSTEM_2013` branch).
  - The save holds neither potion and `cash_product` does not sell them, so the hazard is latent (reachable only through an item cheat).
    - The one other source is random box 65001078 (`RandomItemTable.lua`, group 170023, which gives 85003830). Nothing else in `ServerResource/US` names that box, the save holds none, and the cash shop does not sell it.
    - Only `ApplyBuffFactorToGUUser` dereferences an item's factor pointers; every other caller only counts them. So viewing or hovering the potion is safe, and only *using* it is the hazard.
- **No shipped script changes meaning.** The Enum now defines 1,439 BTI/BFI names that used to read as `nil`. A byte scan of every `.lua` in `kom_contents/*_kom/` and `march2014/data036/` finds none of them; the scan itself sees 709 distinct buff tokens in 290 files. So the only load-time change is the 3 potion factors above.
  - After Phase 13, the baked 362/363 would *silently* apply June's `BFI_BUFF_FIGHTING_SUPER_ARMOR` / `BFI_BUFF_FIGHTING_HEAL`. See the Phase 13 to-do.
- March's premium `BuffTemplet.lua` names the three potion BFIs too. It does so only under `BUFF_FACTOR`, which March's C++ never reads, and its key, `m_eBuffID`, is the unchanged BTI. Not affected.
- `X2Lib/Offline` stores no BTI/BFI numbers (it goes through item-templet factor pointers only). `els_db.sql` has no buff table (schema read with a read-only connection).

**C++ (`SERV_IRUHADEV_JUNE_BUFF_ENUM`, `KTDXLIB/Always.h`).**
- `X2BuffFactor.h`: BBT 73–112 and BFT 14–17 are added with explicit `= N` (111, 112 and `BFT_ATTACK` 17 are commented as not implemented in June either). The drift fixes and renames each keep the original line in `#else`:
  - `BTI_SI_SA_EEG_ATOMIC_SHIELD` 124 → 141;
  - `BFI_BUFF_SWORD_ENCHANT_CHARGE_MP` 338 → 337;
  - `BTI_BUFF_INDURANCE_POWER` → `BTI_BUFF_ENDURANCE_POWER`;
  - `BTI_CHANGE_PHYSIC_DEFENCE_AND_PHYSIC_DAMAGE` → `BTI_CHANGE_ATTACK_AND_DEFENCE`;
  - `BFI_DEBUFF_TELEPORT_CONSUME_MP` dropped (no consumer; June has no such name).
- `X2DamageManager.h`: `ATTACK_TYPE` gains `AT_COMMAND = 2`, which Phase 5's 85/86/87/96 need. June's `AT_NONE = -1` is not a buff enum and was left out.
- No BTI/BFI enumerators were added: no C++ code compares against any June-only id yet.
- **Consumer audit.**
  - `X2GUEL.cpp` (3 sites) and `X2Game.cpp:19074` are renamed under the flag.
  - Two consumers change value only and need no edit, but each is a **behavior fix**:
    - `X2GageManager.cpp:3380` (`IsNotUseNotifyTimeBuff`): its "no 5-second blink" exemption hit 124 = `BTI_BUFF_BREATH_OF_DRAGON` and now hits Atomic Shield.
    - `X2GUElsword_SwordMan.cpp:14990`: the Sword Enchant memo applied 338 = `BFI_BUFF_SWORD_ENCHANT_ICE_MEMO` and now applies Charge MP.
  - No C++ references the HAMEL or potion BFIs. `X2Define.h`'s potion constants are item ids.
  - The server's `XSLBuffManager.h` BTI enum is the separate premium namespace and was not touched.
- CP949 edits were done at the byte level, insert-only (87/5/18/6 lines), and `file` signatures are unchanged (including `X2BuffFactor.h`'s one LF-only line).
- `buff_census.py --phase 2`: every BUFF table 0 new / 0 changed; C++ BBT 113/113, BFT 18/18, BDT 14/14, value drift 0 in all five enums, 0 C++-only names.

**Phase 13 to-dos created here.**
1. Remove the Lua alias `BFI_DEBUFF_TELEPORT_CONSUME_MP` only once **no shipped March Lua names it**. Today two files do: `BattleFieldBuffFactor.lua`, which Phase 13 replaces, and **`NewSkillTempletVer2.lua`** (data036), where a skill's positional `BUFF_FACTOR` list would truncate silently at the nil. Re-grep `kom_contents` for the name before removing it.
   - Also by name, and so consistent with the new Enum: `DamageEffect_INT.lua` (datai015) and the premium `BuffTemplet.lua` name the HAMEL_EVENT BTI.
2. Make `Item.kim`'s two potions point at 481/482. Either patch the two WORDs at file offsets 20107068 / 20107412 (re-verify them with the cross-reference method above first), or re-bake. Do this in the same pack as June's factor file. The alternative is a silent wrong buff.

**Build and deploy.**
- The `X2ServerProtocol` / `KTDXLIB` / `X2Lib` / `X2` chain built at `US_SERVICE`, rc=0 for each, 0 errors. Both `stdafx.cpp` files were touched first and rebuilt.
- `flagprobe.py` resolves `SERV_IRUHADEV_JUNE_BUFF_ENUM` **ON** through `cl /EP`.
- `x2.exe` (18,991,104 bytes, md5 `21539f99…`) was deployed as `X2_offline.exe`; the hash matched after the copy. Not launched.
- **Pack:** `Enum.lua` from `kom_contents/data036_kom/` (same bytes as `march2014/data036/Enum.lua`) goes into `data036.kom`, paired with this exe. Per the pairing hazard, it ships only together with it.

**Review.** Done 2026-10-03; see "Phase 2 — Review" below.

**Cost.** Not recorded: no `/cost` figure was available in-session. Ask the user.

### Phase 2 — Review (2026-10-03, Opus 5.5 high)

Independent review of `2068485` and `e68005e`. It used fresh scratch scripts (own Lua table parser, own C++ enum evaluator, own byte-diff, own ordinal-loader simulation, own bytecode scan); none of the Phase 2 session's scripts were reused. Ghidra was used read-only.

**Verdict: no defects. Whether `Enum.lua` can be packed depends on the 7b answer:**
- **7b option 1 (keep):** pack now.
- **7b option 2 (keep + null guard):** `Enum.lua` does not change and is safe to pack now. The guard needs a rebuild and redeploy, so pack with that exe or before it (both exes have the enum flag).
- **7b option 3 (revert):** do **not** pack. `Enum.lua` is recompiled and re-staged first.

**Pack preconditions, checked:**
- **Exe pairing.** `X2/US_SERVICE/x2.exe` and `$DATA/X2_offline.exe` are both md5 `21539f99…`, 18,991,104 bytes.
  - The exe was built at 02:10, before the 02:12 commit time of `2068485`. `git log 2068485..HEAD -- X2Lib X2 KTDXLIB X2ServerProtocol` is empty, and the touched files' worktree equals HEAD. So the deployed exe is the Phase 2 code.
- **No loose shadows.** There is no loose `Enum.lua`, `BattleFieldBuff*.lua` or `BuffTemplet*.lua` in `$DATA` (depth 2). The only loose `.lua` there are user config files (`GameOptions`, `GamePadConfig`, `LoginKey`, `SelectedServerGroup`, `TutorSystem`).
- **Scan coverage.** The 7c scan covers the `kom_contents` extracts, not loose files. Two files in it are not mar-2014 bytecode and could not be read reliably: `data036_kom/PvPDropTable.lua` (unrecognised header) and `data014_kom/UI_NPC_KELAINO.lua` (`KL\x82`).

| # | Check | Result |
|---|---|---|
| 1 | Lua values | Every `BUFF_*` table plus `SPA_BUFF_TYPE` (16 tables) equals June's member-for-member: BTI 1065, BFI 1236 + the alias, BBT 113, BFT 18, BDT 14, and the 6 new tables (3/1/3/6/6/2 members). The only extra member is `BFI_DEBUFF_TELEPORT_CONSUME_MP`; the only duplicate value in any table is 290 (alias + `_PVP`). The 5 renumbered members each carry a `--{{ … --}}` comment naming the old value and its June reuse. No non-buff table changed (old vs new parse, 311 → 317 tables, +1,501 members = 705 + 734 + 38 + 3 + 21). |
| 2 | Staged bytecode | `march2014/data036/Enum.lua` and `kom_contents/data036_kom/Enum.lua` are byte-identical (314,915 bytes, md5 `81f0b944…`), and so are the two `.src.lua`. Header `1b 4b 4c 81` (mar-2014). A scratch copy decompiled with `elstools decompile-batch` matches the source on all 317 tables, 0 differ. |
| 3 | C++ values | `X2BuffFactor.h` evaluated with the flag on and off. ON: BBT 113 names (0–112), BFT 18 (0–17), BDT 14, BTI 114, BFI 57, with zero drift, zero C++-only names and zero duplicate values against June. OFF reproduces exactly the 5-row drift table. Every new enumerator has an explicit `= N`, and `BBT_CHANGE_HYPER_CHARGE_SPEED` still lands on 72. `AT_COMMAND = 2` is in `X2DamageManager.h:78`. |
| 3 | Flag hygiene | `SERV_IRUHADEV_JUNE_BUFF_ENUM` is defined only at `Always.h:4680` (no `.vcxproj` or other `#define`). Every replaced line keeps its original in `#else`, and every block closes `#endif SERV_IRUHADEV_JUNE_BUFF_ENUM`. Both headers are reached only through `X2Main.h:91/106`, after `KTDX.h` at `:7`, and the only non-PCH TU in `X2Lib` is `sqlite3.c`. |
| 4 | Consumer audit | The only C++ consumers of the 5 drifted names are the ones the Done note lists: `X2GUEL.cpp:3544/3621/6417`, `X2Game.cpp:19076`, `X2GageManager.cpp:3380` and `X2GUElsword_SwordMan.cpp:14990`. No buff id is passed as a literal or cast anywhere in `X2Lib`/`X2`/`KTDXLIB`/`X2ServerProtocol`. The `OpenBuffName( false, 124 )` calls in `X2PartyManager.cpp` take a UI picture index, not a BTI. Nothing uses a BBT/BFT member as a loop or array bound. The old March Lua already had Atomic Shield = 141 and Charge MP = 337, and the ordinal simulation (7a) puts `SWORD_ENCHANT_CHARGE_MP` in factor slot 337, so both value fixes are real behaviour fixes. |
| 4 | Server/packets | The server's `KncWX2Server/Common/X2Data/XSLBuffManager.h` has no HAMEL member, and its potion BTIs (1318–1320, used by `GSUserFunction.cpp:17215`) are unchanged. `X2Lib/Offline` sends no buff ids (it only replays `EGS_USE_QUICK_SLOT_NOT`; the `1321`/`1322` in `X2OfflineCashSeed.h` are product ids). No renumbered value crosses a packet the server interprets. |
| 5 | CP949 | All 5 C++ files are insert-only (+15/+87/+5/+18/+6, −0), the inserted lines are ASCII and CRLF, and the high-byte counts are identical before and after. `file` signatures are unchanged, and `X2BuffFactor.h` still has exactly one LF-only line. `Enum.src.lua` has 9 replaced lines (4 trailing-comma additions + the 5 renumbers), all CRLF, with no BOM before or after. |
| 6 | Ghidra (3 items) | BBT templet factory `32e804d9`: `DEC EAX; CMP EAX,0x6d; JA` → types 1–110. BFT factory `32ec3766`: `DEC EAX; CMP EAX,0xf; JA` → 1–16. `GetFinalDamage` `32286228`: `CMP dword ptr [EBX+0x8],0x2` (DamageData attack type == `AT_COMMAND`) feeds the `JNZ` at `3228626e`. All three agree with Phase 1 and the enums. |

**Item 7: the deviations and the Done note's claims.**
- **7a, TELEPORT alias: keep it (agreed).** The live factor file in `kom_contents` matches the live `data036.kom` entry (353,105 bytes); it was decompiled to scratch. Simulating `AddBuffFactor_LUA`'s ordinal rule on that file gives:
  - Old Enum: 364 of 366 load (the HAMEL BFIs, rows 365–366).
  - Enum as committed: 361 of 366 load (+ the 3 potions).
  - Without the alias: 289 of 366 load; row 290 gets a nil id and every later row fails.
  - So removing the alias costs **72 more factors (slots 290–361)**, 77 failures in all. The Done note's "factors 290–364, 77 of them" mixes those two numbers; the substance is right.
- **7b, potion renumber: open, the user's call.** The hazard is confirmed. `KItemFormatTemplet::GetBuffFactorPtr` (`X2Item_Preprocessing.cpp:181`) resolves `Item.kim`'s baked WORD lazily, so nothing happens at load. On *use*, `GetBuffFactorPtr( 362 )` pops `uiBuffFactorNumber_ invaild` and returns null, and `ApplyBuffFactorToGUUser`'s `UPGRADE_SKILL_SYSTEM_2013` branch calls `->GetClonePtr( 0 )` on it unchecked (`X2Game.cpp:18160`; the other branch checks). Reachability is as the Done note says: neither potion is in the save, in `cash_product`, or held via box 65001078.
  - Options:
    1. **Keep** (as staged). It matches June and the stated duplicate-value rule.
    2. **Keep and add a one-line null guard** at `X2Game.cpp:18160`, behind this flag. That closes the crash for any item whose factor failed to load, which matters again in Phase 13 for June items.
    3. **Revert** the 3 BFIs to 362–364 until Phase 13. That adds 3 more duplicate values (with June's `FIGHTING_SUPER_ARMOR`/`FIGHTING_HEAL`/`BRAVE_TOWER`) and needs a recompile and re-stage.
  - **Reviewer's recommendation: option 2.**
  - **User's decision (2026-10-03): option 1, keep the renumber as staged.** No code change and no null guard. `Enum.lua` is cleared to pack with the deployed Phase 2 exe. The crash on using either Heretic potion stays latent until Phase 13 re-points `Item.kim` (Phase 13 to-do 2).
- **7c, claims checked:**
  - **"2 → 5 launch popups": confirmed.** Each failure path in `AddBuffFactor_LUA` is a `DISPLAY_ERROR`, which is a `MessageBox` here (`KTDXCommonFunc.h:1462-1469`, `#ifndef _SERVICE_`).
  - **"No shipped Lua names any of the 1,439 new names": confirmed** with an exact-constant scan (`ULEB128(len+5)` + name) over all 4,838 non-Enum `.lua` in `kom_contents/*_kom` plus `march2014/data036`. There are 0 hits; the one bare-substring hit, `BTI_DEBUFF_SHADOW`, is a prefix of longer March names. (A greedy-token scan under-reports here, because the next constant's length byte can be a letter or `_`; the exact-constant method is the one to reuse.)
- **Doc corrections** (low severity, no action needed beyond reading this):
  - `DamageEffect_INT.lua` (datai015) names the HAMEL **BFI** pair (372/373, unchanged), not the HAMEL BTI. The BTI pair is named by `BattleFieldBuffTemplet.lua`, `BattleFieldBuffFactor.lua` and the premium `BuffTemplet.lua`.
  - The premium `BuffTemplet.lua` names all three potion BFIs, as the note says.
  - The TELEPORT alias is named exactly by `BattleFieldBuffFactor.lua` and `NewSkillTempletVer2.lua`.

**Review cost.** `/usage` is not readable from inside the session. Ask the user, and add the figure here.

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

### Phase 3 — Done (2026-10-03), launch checks pending

**Status.** Code, build and deploy are done. Both exit checks need a client launch, which only the user may start. Until that happens the exit metric is **pending, not met**. The expected log lines are written out below, so the check is a diff.

**What changed (`SERV_IRUHADEV_JUNE_BUFF_LOADER`, `KTDXLIB/Always.h`).** Every edit is a pure insertion (438 lines, 0 removed). Each replaced body stays intact in `#else`, and every file keeps its `file` signature.
- **`X2BuffTempletManager.cpp`, ported from June:**
  - `AddBuffTemplet_LUA` (`326c3990`) has the same shape as March.
  - `AddBuffFactor_LUA` (`326c4050`): an unreadable id stays 0 and falls through to the ordinal check (no early return).
  - The `_INT` branch, new as `AddBuffFactorINT` (`326c3f30`), claims every id ≥ 10000. It uses a second vector, `m_vecBuffFactorPtrINT`, under its own rule `size == id-10001`, so id 10000 itself is always rejected.
  - `GetBuffFactorPtr` (`326c32a0`): ids ≤ 10000 go to the main vector and 10001+ to `_INT`. Id 0 is silent. A bad id gets one `[JUNE_BUFF]` line per id, not a popup.
  - `OpenScriptFileForTemplet`/`ForFactor` load `BattleFieldBuff{Templet,Factor}_INT.lua` after the main file (`326c0020`/`326c13a0`). When the file is absent they log one "not found ... expected until Phase 13" line.
- **Placeholders (the planned deviation from June).** A factor that passes the ordinal check but fails to parse, has no `BUFF_FACTOR` table, or fails to create gets a NULL slot. Later ids therefore stay aligned. `GetBuffFactorPtr` returns NULL for the slot, so it is never applied. Both June and March drop such a factor and keep the cascade.
- **Ordinal rejects are not placeholders.** An id that fails `size == id-1` is dropped, exactly as in June and March, and logged as `ordinal reject BFI n`. Padding gaps instead would have invented about 117 slots for March's five out-of-range ids.
- **No more popups on the parse path.**
  - `KTDXCommonFunc.h`'s `DisplayErrorMessage` (both overloads) hands its text to a per-thread sink, `__declspec(thread) g_pIruhaDisplayErrorSink`, defined in `KTDXApp.cpp`, whenever one is installed. `StateLog` is kept and only the `MessageBox` is skipped. The sink is per-thread because Lua is also parsed on loader threads (`X2OPTIMIZE_DISABLE_LUA_MULTITHREADING` is OFF).
  - `CX2JuneBuffCapture` (in the manager header) installs the sink for one entry. The outermost capture on a thread owns the entry and writes one line: `[JUNE_BUFF] unsupported <reason> in <BTI n | BFI n (BTI m) | inline BUFF_FACTOR (BTI m)> -> <templet dropped | placeholder | not added>: <every captured text>`.
  - An entry that parsed but still raised errors gets `[JUNE_BUFF] warning in ...` instead.
  - `CX2BuffFactor::ParsingBuffFactor` is wrapped (`ParsingBuffFactorBody` holds the original), so the inline `BUFF_FACTOR` blocks of damage data (`CX2DamageManager::ParsingBuffFactor`) are covered too.
  - The five factory/parse `default:` branches (`X2BuffFactor.cpp` ×3 including the dead non-`UPGRADE_SKILL_SYSTEM_2013` copy, and the behavior, finalizer and displayer templet factories) raise `unsupported BBT|BFT|BDT <n>`, so each line names the type. Outside a capture this is still a MessageBox, now with the number.
  - Lines go to `ErrorLog.txt` through `KLOG`, under a lock.
  - Limitation: the sink sits inside `#ifndef _SERVICE_`. If a build ever defines `_SERVICE_`, there are no popups and also no reason text.
- **Load summary**, after the factor files: `[JUNE_BUFF] load summary: templets N loaded, F failed, D duplicate; factors M loaded + P placeholder(s); _INT factors Mi loaded + Pi placeholder(s); ordinal rejects R (+ Ri _INT)`.
- `SERV_IRUHADEV_JUNE_BUFF_DEBUG` was not needed and was not added.

**Build and deploy.**
- `X2ServerProtocol`, `KTDXLIB`, `X2Lib` and `X2` at `US_SERVICE` all built rc=0 with 0 errors; both `stdafx.cpp` files were touched first.
- `x2.exe` is 18,999,808 B (2026-10-03 03:55:52, md5 `841b930a…`) and contains `load summary: templets`, `ordinal reject BFI`, `unsupported %s in %s` and `BattleFieldBuffFactor_INT.lua`.
- Deployed as `X2_offline.exe`; the md5 matched after the copy. Not launched.
- This replaces the exe that the Phase 2 review's "exe pairing" check names (md5 `21539f99…`). Pairing still holds: `SERV_IRUHADEV_JUNE_BUFF_ENUM` is still defined, so the Phase 2 `Enum.lua` can be packed with this exe.
- Effect on the review's 7b hazard: using a potion now logs `BFI 362 requested but not loaded ...` once, in place of the `uiBuffFactorNumber_ invaild` popup. `GetBuffFactorPtr` still returns NULL, and `ApplyBuffFactorToGUUser` still dereferences it, so the crash remains. Option 2's guard is still the open decision.

**Prediction tool.** `june2017_re/buff_trial_predict.py` walks every entry the way March's C++ parses it (rules in its docstring), using the census's flag-resolved type lists. With `--log ErrorLog.txt` it diffs a trial's `[JUNE_BUFF]` lines entry by entry against both the prediction and the census set (entries listing ≥ 1 type the C++ lacks). `--source DIR` and `--enum FILE` point it at other data.

**Exit check 1: March data, real game directory.**
- Live `data036.kom` still has the **pre-Phase-2** `Enum.lua` (265,394 B; the staged one is 314,915 B).
- Baseline before this phase: `State.log` (2026-10-02 05:43) shows exactly 2 buff popups, both `BUFF_FACTOR_ID invalid`.
- Prediction, from a scratch decompile of the live bytecode (not committed) with the pre-Phase-2 Enum (`git show 2068485^:march2014/data036/Enum.src.lua`):
  ```
  [JUNE_BUFF] duplicate BTI 87: the first definition is kept (map insert)
  [JUNE_BUFF] BattleFieldBuffTemplet_INT.lua not found, so no _INT buffs this run (...)
  [JUNE_BUFF] ordinal reject BFI 372: 364 factor slot(s) filled, so the next id must be 365; ...
  [JUNE_BUFF] ordinal reject BFI 373: 364 factor slot(s) filled, ...
  [JUNE_BUFF] BattleFieldBuffFactor_INT.lua not found, so no _INT buffs this run (...)
  [JUNE_BUFF] load summary: templets 320 loaded, 0 failed, 1 duplicate; factors 364 loaded + 0 placeholder(s); _INT factors 0 loaded + 0 placeholder(s); ordinal rejects 2 (+ 0 _INT)
  ```
  - The two rejects are the two old popups, now log lines, and no popup is raised at all.
  - The duplicate is `BTI_DEBUFF_BLIND_SMOKE`, which March's file defines twice. March has always kept the first silently. It is new as a line, not as behavior.
- Once the Phase 2 `Enum.lua` is packed: factors 361, ordinal rejects 5 (372, 373, 481–483), and everything else the same.

**Exit check 2: June's files, trial copy.**
- **Trial directory:** `F:\elsword stuff\elsword_2014\els_2014\237311\trial_june_buff_p3\`, made by a scratch script.
  - The `.kom` files and the `eventscene`/`movie`/`music`/`ui`/`XTrap` trees are hardlinked; nothing writes to them.
  - Everything else is copied: the exe, dlls, option `.lua`, `start_offline.bat`, and a **copy** of `els_db.sql` (the WAL was empty).
  - Old logs are not copied. The real directory was only read.
- Loose files in the trial root, all mar-2014 bytecode (`1b4b4c81`):
  - `Enum.lua`, the staged Phase 2 one (md5 `81f0b944…`);
  - June's `BattleFieldBuffTemplet.lua`, `BattleFieldBuffFactor.lua` and the two `_INT` files, compiled from `june_src_fixed` with `elstools compile-batch -v mar-2014`. The compile uses **absolute** `-i` paths; relative ones fail with "cannot open".
  - Loose files beat the `.kom` (`MASS_FILE_FIRST` off) and are not XOR-decrypted (`X2OPTIMIZE_INDICATE_LOCAL_MASSFILE` ON, probed).
- `JUNE2017_TRIAL_LOAD.md` records the user's preference for a *packed* trial `data036.kom`. A loose file inside the throwaway copy was used instead; this goes to the user with the launch request.
- **Prediction:** `load summary: templets 894 loaded, 107 failed, 0 duplicate; factors 1024 loaded + 106 placeholder(s); _INT factors 69 loaded + 0 placeholder(s); ordinal rejects 0 (+ 0 _INT)`.
  - Templets: 98 of the failures are unhandled types and 9 are a missing `ACCUMULATION_MULTIPLIER`.
  - Factors: all 106 placeholders are unhandled types.
  - The census set is 100 templets and 106 factors. The 2-templet gap is census entries whose first failure is the `ACCUMULATION_MULTIPLIER` check.
  - First-reason counts: BBT 92 ×40, 89 ×29, BFT 14 ×18, BBT 75 ×14, 80 ×10, 77 ×9, 108 ×8, 73 ×7, and the rest ≤ 6.
  - No failure is predicted from the 208/219 "COMBINATION entry without table" anomalies. All of them are `BBT_EMPTY` or unhandled types.
- **Compare:** `python june2017_re/buff_trial_predict.py --log "<trial>\ErrorLog.txt"`. It must end `RESULT: trial matches the prediction and the census`.

**Hand-offs.**
- **Phase 4a:** 9 June templets (BTI 842, 864, 874, 881, 884, 912, 971, 986, 1020) have `ACCUMULATION_LIMIT > 1` and no `ACCUMULATION_MULTIPLIER`. March's `ParsingScript` requires the key in that case, so they fail. Check whether June's `3236a9b0` reads it optionally.
- **Phase 13:** the trial directory is throwaway. Delete it once Phase 3 is confirmed; it holds only hardlinks and copies.

**Review.** Done; see "Phase 3 — Review" below. No code defects. Exit check 1 is met. Exit check 2's expected line above is wrong as staged (finding R1).

**Cost.** Not recorded (no `/cost` figure in-session).

### Phase 3 — Review (2026-10-03, Opus 5.5 high)

Independent review of `2116952`, `e041a83` and `e527a72` (`SERV_IRUHADEV_JUNE_BUFF_LOADER`). It used fresh scratch scripts only; none of the Phase 3 session's scripts were reused. They were:
- the six June functions decompiled and disassembled, with the XOR-built file names decoded;
- `cl -EP` of the six buff TUs at `US_SERVICE` defines, from which the compiled `case` labels and C++ enum values were extracted;
- a scratch Lua 5.1 interpreter built from `luaLib/` with the local VS2010, running a loader simulation written in Lua against the **real** buff files. Unlike a text parser, it fails exactly where the client's Lua would;
- a fresh `elstools decompile-batch` of March's live buff bytecode (sizes equal to the live `data036.kom` entries).

Ghidra was used read-only.

**Verdict: no code defects, so nothing was changed or rebuilt.** One Medium finding (R1) means exit check 2 as written will report a mismatch on a loader that is behaving correctly. It also hides a hazard for Phase 13.

**Exit check 1 (March data, real game directory): met.** The user packed the Phase 2 `Enum.lua` at 05:15. The live `data036.kom` now holds `Enum.lua` at 314,915 B; the buff files are unchanged at 353,105 / 152,564 B. The 05:15:52 launch logged exactly the Done note's "once the Phase 2 `Enum.lua` is packed" variant:
- the duplicate BTI 87;
- both `_INT` not-found lines;
- ordinal rejects 481, 482, 483, 372 and 373, in that order (my simulation gives the same order);
- `load summary: templets 320 loaded, 0 failed, 1 duplicate; factors 361 loaded + 0 placeholder(s); _INT factors 0 loaded + 0 placeholder(s); ordinal rejects 5 (+ 0 _INT)`;
- 0 `unsupported` and 0 `warning` lines. My simulation also predicts 0 warnings.

Two caveats:
- March data raised no captured error, so the sink and `Report()` were **code-reviewed, not exercised**. The trial is their first real run.
- "No popups" holds for load time only. See R4 for 87 runtime popups from a path Phase 3 did not touch.

**Exit check 2 (June files, trial copy): not run.** The trial directory has no `ErrorLog.txt`, and its save files carry their copy-time mtimes, so it was never launched. `buff_trial_predict.py --log` therefore could not be run. What it will show is in R1.

| # | Check | Result |
|---|---|---|
| 1 | June parity | Port matches all six functions. **`_INT` add `326c3f30`:** `CMP EAX,0x2710; JNC` at `326c3f5c`, so ids ≥ 10000 are claimed and the function returns 1 whatever happens. The rule is `size == id + 0xffffd8ef` (= id − 10001) at `326c3f7d`, so 10000 is always rejected; a parse failure pushes nothing. **`GetBuffFactorPtr` `326c32a0`:** `CMP ESI,0x2710; JBE` at `326c32d4`, so ≤ 10000 is main. Id 0 returns null silently. A NULL slot returns null with no message. `_INT` index is `[id−10001]`, and bad ids format a string that is discarded. **`AddBuffFactor_LUA` `326c4050`:** an unreadable id becomes 0 with no return, `326c3f30` is called before `size == id−1` (`SAR EDX,2; DEC EAX; CMP` at `326c41df`), and the parse is `ParsingBuffFactor( lua, 1 )`. **`AddBuffTemplet_LUA` `326c3990`:** create, `ParsingScript`, map insert; the inserted node is registered with June's anti-tamper call (not ported, per Phase 1). **Loaders `326c0020`/`326c13a0`:** they build `BattleFieldBuff{Templet,Factor}.lua` and then `…_INT.lua`. The success path (`326c0a61`) and both failure paths (`326c0a4b` → `326c0a80`) fall through to `326c0a9c`, the `_INT` block, so the `_INT` load is unconditional, as ported. |
| 2 | Placeholder callers | 25 direct sites. Null-checked: `X2DamageManager.cpp:3988`, `X2GameUnit.cpp:11045/11077/11112`, `X2PremiumBuffManager.cpp:513/548/600/660`, `X2Unit.cpp:1682/4281`, `X2BuffTempletManager.cpp:356` (`…FromBuffFactorList` drops NULLs), `X2ItemManager_Preprocessing.cpp:1173` (log only). `X2SocketItem.cpp:182/309/697/701` store the pointer, and every consumer checks it (`X2GUUser.cpp:3220/3295/11225/30547/30706`, `X2GUNPC.cpp:11616`). `X2Item_Preprocessing.cpp:181` passes it through to `X2Game.cpp:18160` (7b, known). **Unchecked dereferences:** `X2Game.cpp:18160` (7b), and `X2BuffBehaviorTemplet.cpp:1152/1157` (`SetFactor`) and `:1207/1210` (`SetFactorFromPacket`), both `*ptrBuffFactor` of `BBT_CREATE_BUFF_FACTOR`'s target. The last two are **pre-existing, not new** (finding R2). |
| 3 | DisplayErrorMessage sink | **TLS:** static `__declspec(thread)` in an exe is fine, and `KTDXApp.cpp` already uses it for `ms_pLuaBinder`. **Visibility:** `KTDXCommonFunc.h` is reachable only through `KTDX.h:1198`, after `Always.h` at `:190`, so every TU sees the flag. The inline function has one definition (no ODR split), and the `extern` is in the same header as the only definition's users. **Ownership:** the outermost capture owns, nested ones are no-ops, and the destructor clears only when it owns. No Lua error can be raised while a capture is live, because the entry's table is built before the C++ call, so RAII never depends on unwinding through Lua. **Outside a capture:** `StateLog` plus `MessageBox` as before. Two texts changed by design: default branches now name the type, and `GetBuffFactorPtr`'s bad-id popup became a log line (June parity). **Narrow copy:** 511 bytes max; an overflow makes `WideCharToMultiByte` fail and the zeroed buffer stays terminated. `CP_ACP` would mangle Korean text, but parse-path texts are ASCII. **Line buffer:** 1024 with `_TRUNCATE`. **Lifetime:** the manager is destroyed in `CX2Data`'s destructor (`X2Data.cpp:1053`), before CRT teardown; R3 notes the lock's scope. |
| 4 | Reporting | `Report()`: failed → `unsupported`; parsed with texts → `warning`; else silent. The five `default:` branches return `UnsupportedType()`, i.e. `DisplayErrorMessage()`, which is `false` on every path, as before. The `ParsingBuffFactor` wrapper keeps `bIsSkill = true` (`X2BuffFactor.h:757`); `ParsingBuffFactorBody` has no default. The callers are the manager (default) and `X2DamageManager.cpp:3835` (`false`, inline path). That path pushes only on success, so the inline "not added" outcome is true. |
| 5 | Predictor rules and numbers | Every rule matches the compiled code. The `-EP` case sets equal the census's for all five switches (72/13/13/72/13). `BBT_EMPTY` is `return true` in the templet factory and `break` in factor parse. The `bExceptionCase` guard is compiled for BEHAVIOR and DISPLAYER only. A factor BEHAVIOR without COMBINATION returns true and FINALIZER returns false. Every sub-parser requires its own sub-table. `ACCUMULATION_MULTIPLIER` is required when `ACCUMULATION_LIMIT > 1`. My simulation reproduces **March** exactly (pre-Phase-2 Enum: 320 + 1 duplicate (BTI 87), 364, rejects 372/373; packed Enum: 361, 5 rejects). For **June** it gives 894/107, 1024+106, `_INT` 69+0, 0 rejects, with the same 107 BTIs and 106 BFIs as `buff_trial_predict.py -v` (0 differ), **but only when `SKILL_SLOT_TYPE` is defined** (R1). |
| 6 | Hygiene | All 10 code files are insert-only (`--numstat` deletions 0) and every inserted byte is ASCII. High-byte counts and `file` signatures are unchanged. Every inserted `#else`/`#endif` carries ` SERV_IRUHADEV_JUNE_BUFF_LOADER`. `KTDXCommonFunc.h`'s insertions are LF, matching its LF region (1568 → 1596 LF, CRLF 6 → 6). `X2BuffTempletManager.cpp`'s are all CRLF (LF-only stays 39), 46 of them next to the file's existing LF lines. |
| 7 | Build, deploy, trial | `X2/US_SERVICE/x2.exe`, `$DATA/X2_offline.exe` and the trial's `X2_offline.exe` are all md5 `841b930a…`, 18,999,808 B, built 03:55:52, before the 04:05 commit. `git log 2116952..HEAD -- X2Lib X2 KTDXLIB X2ServerProtocol` is empty. All six new strings are present (narrow, and UTF-16 for `BattleFieldBuffFactor_INT.lua` and `unsupported %s %u`). Trial: 183 `.kom` plus every file under the five asset trees are hardlinks (same file id). The newest file in those trees in the real directory is 2026-09-19, so they are never written. All 71 root files are copies. `start_offline.bat` is byte-identical to the real one and uses only `cd /d "%~dp0"`. The real directory has no loose `.lua` besides the 5 user config files. |
| 8 | Ghidra spot-checks (rule 10) | Three items, all pass: (a) `326c3f5c` `JNC` + `326c3f7d` `ADD 0xffffd8ef`, the `_INT` add boundary and rule; (b) `326c32d4` `JBE`, the lookup boundary; (c) `326c0a4b`/`326c0a61` both reach `326c0a9c`, the unconditional `_INT` load. No escalation. |

**Findings**
- **R1, Medium: June's buff files need `SKILL_SLOT_TYPE`, which the staged March `Enum.lua` lacks, so both main files abort partway.**
  - June's `Enum.src.lua:17838` defines `SKILL_SLOT_TYPE = { SKILL_SLOT_None 0, A 1, B 2, TC 3 }`. Phase 2 ported only `BUFF_*` tables, and no C++ binding and none of 5,452 March `.lua` define it.
  - `BattleFieldBuffTemplet.lua:26196` (inside BTI 820, `BTI_TRANSCENDENTAL_SLOT_ATTACK_EFFECT`) and `BattleFieldBuffFactor.lua:115261` (inside BFI 843) index it. That is a Lua runtime error, so `DoMemory` fails, and the client logs `KEM_ERROR73` and `XEM_ERROR2` for each file. Every entry before the error has already been added; every entry from it on is never run. The `_INT` files don't use it.
  - **Expected trial result as staged:**
    `load summary: templets 713 loaded, 61 failed, 0 duplicate; factors 779 loaded + 63 placeholder(s); _INT factors 69 loaded + 0 placeholder(s); ordinal rejects 0 (+ 0 _INT)`, 0 warnings.
    - Its failure sets are strict subsets of the full run's.
    - All 46 templets and 43 factors in the difference are BTI 820 / BFI 843 or later.
    - `buff_trial_predict.py --log` will print those as "predicted, did not fail", plus a census miss for the same entries, and end `MISMATCH`.
  - **With the table defined,** the Done note's line is right: 894/107, 1024+106, 69+0.
  - **Recommendation:** for the trial, put a trial-only `Enum.lua` in the throwaway directory that adds June's 4-member table. That keeps exit check 2 a test of the loader. The alternative is teaching the predictor to stop at the first unresolved global.
  - Not done here: the staged/packed `Enum.lua` is the user's, and the trial format is an open question.
- **R1 hand-off to Phase 13 (the important part).** This truncation **passes Phase 13's gate as written**. "0 placeholders, no unsupported lines" can hold with about 227 templets and 288 factors never run. Phase 13 needs three more things:
  - the loaded counts in the load summary must equal the source files' entry counts;
  - no `KEM_ERROR73`/`XEM_ERROR2` for any buff file;
  - `SKILL_SLOT_TYPE` (and any other June-only global) added to `Enum.src.lua`. Run an unknown-global pass (this review's harness: scratch `lua51.exe` from `luaLib/` plus `D3DXCOLOR`/`D3DXVECTOR2/3` stubs and an `_G` `__index` logger) on every June `.lua` Phase 13 adopts. In this run the only unknown globals besides `SKILL_SLOT_TYPE` were `STR_ID_*`, which are read with defaults.
- **R2, Low: unchecked `*ptrBuffFactor` in `CX2BuffCreateBuffFactortBehaviorTemplet` (`X2BuffBehaviorTemplet.cpp:1152/1157/1207/1210`). Pre-existing; placeholders widen the trigger.**
  - Before Phase 3 an unloaded target already returned NULL (out of range); now an in-range placeholder can too.
  - Not reachable today:
    - all 12 March targets are ≤ 167 and loaded under the packed Enum;
    - no other March `.lua` names `BBT_CREATE_BUFF_FACTOR` (byte scan of `kom_contents` + `march2014`);
    - none of June's 18 targets is one of the 106 placeholders.
  - Checked against the factor files only. `SetFactorFromPacket` takes the id from a peer.
  - Same class as 7b; a one-line guard belongs with Phase 4b's parity work on this type.
- **R2b, Low: `GetBuffFactorPtrFromBuffFactorList` drops NULLs, so a placeholder in a skill's or item's `BUFF_FACTOR` list shifts later entries down.** Position-indexed users then get a different factor (`X2GameUnit.cpp:11140/11152`, `X2GUAra.cpp:1595`'s `[0]`). This already happened for out-of-range ids; placeholders make it possible for in-range ones. It matters only once June data with placeholders ships (Phase 13 requires 0 placeholders, which closes it).
- **R3, Low: the `[JUNE_BUFF]` lock serialises only `[JUNE_BUFF]` writers.** `KLOG` is the global `KDbgStm` (`KSuperAssert.h:109`), and `ErrorLogMsg` writers on loader threads do not take this lock. The race is pre-existing; the worst case is an interleaved line.
- **R4, Medium, outside Phase 3's scope: 87 runtime popups in the 05:15 session.**
  - `State.log` has 87 `BuffTemplet doesn't exist` entries between 05:21:21 and 05:21:52, in the Ranox secret stage. They come from `CX2GameUnit::SetBuffFactorToGameUnit` (`X2GameUnit.cpp:11179`), outside any capture, so each one was a MessageBox. A factor was applied whose BTI has no templet among the 320 loaded.
  - `MISS MATCH BUFF_CHANGE_TYPE` ×10 in the same session is likely related.
  - The BTI is not logged, so it is unidentified here, and there is no earlier `State.log` to show whether it predates Phase 2/3.
  - Hand-off: Phase 4a (apply path) or the Lanox phases. First step: log the BTI at that site behind a `_DEBUG` flag.
- **Info.**
  - BFI 512 (`BFI_BUFF_MAG_STONE_ATTACK_B`) was requested at 05:23:39, right after a `DamageEffect_NUI_MAG_STONE_Attack_A` hit. It is a June-era damage effect naming a factor March's file lacks, reached through the null-checked damage-data path. It was logged once where it used to be a popup. It is a Phase 13 data gap.
  - The 05:15 repack gave the real `data036.kom` a new file, so the trial's `data036.kom` is now a stale copy (6,256,502 B). It is shadowed by the trial's loose `Enum.lua`. The link broke in the safe direction.
- **Doc correction (low):** the Done note's exit-check-2 prediction needs R1's precondition. Its numbers hold only with `SKILL_SLOT_TYPE` defined.

**Review cost.** `/usage` is not readable from inside the session; asked of the user.

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

### Phase 4a — RE (2026-10-03)

Recorded before any C++ was written. Every address has a `funcmap.tsv` row (43 new rows, "Buff Phase 4a").

**`CX2BuffTemplet::ParsingScript` `3236a9b0`.** Keys, in read order. Guard bits in `xorstrings.tsv` (`337a1bd8:1`…`:10000`) tie each key to its read.

| Key | Required? | Store (June offset) | Default |
|---|---|---|---|
| `BUFF_TEMPLET_ID` | yes | +0x34 | — |
| `IS_WORLD_BUFF`, `IS_DEBUF` | yes | +0x66, +0x64 | — |
| `IS_STORE_BUFF` | no | +0x67 | false |
| `ACCUMULATION_LIMIT` | yes | +0x3c | — |
| `ACCUMULATION_DELAY_TIME` | no | +0x48 (and a running timer at +0x4c) | 0 |
| `NOT_SAVE_GAGEMANAGER` | no | +0xb9 | false |
| `IS_INSTANCE_KILL_DEBUFF` | no | +0xba | false |
| limit < 2: `BUFF_REPLACE_DETAIL_OPERATION` | no | +0x60 | **2** (`BRDO_ADD_TIME`) when absent; the ctor's 0 when the limit ≥ 2 |
| limit ≥ 2: `ACCUMULATION_MULTIPLIER` | **no** | UCM 1 (plus UCM 0 = 0) | **1.0**. This settles the Phase 3 hand-off: March requires it, so the 9 June templets without it failed |
| limit ≥ 2: `ACCUMULATION_DECREASE_WHEN_FINISH` / `_COUNT_WHEN_FINISH` | no | +0x40 / +0x44 | false / 1 |
| `REGIST_TYPE` | yes | UCM 2 if > 0 | — |
| `BEHAVIOR`, `FINALIZER` (required), `DISPLAYER` (optional), icon keys | as March | | `BUFF_NAME` 2674, `BUFF_DESC` 2606 |

The ctor (`326c48a0`) leaves +0x4c = 0, so the first push is never delay-blocked.

**`CX2BuffFactor::ParsingBuffFactor` `32d4d850`.**
- Reads `BUFF_TEMPLET_ID`, `RATE` (`ParsingTableForWhile`), `IGNORE_REGIST_FOR_RATE` (required), `ACCUMULATION_AT_ONE_GO` (uint, default 0), the `APPLY_CHECK_CONDITION` table, then `BEHAVIOR` and `FINALIZER`.
- **It never reads a factor-level `DISPLAYER` or `APPLIED_SKILL_ID`.** June reads `APPLIED_SKILL_ID` only at damage-data level, as the first key of `SetDamageDataFromLUA` (`32632446`), before any `BUFF_FACTOR` table is open. The 2 copies nested inside inline `BUFF_FACTOR` blocks (`DamageEffect.src.lua:9104`, `:33345`) are ignored by June too.
- `APPLY_CHECK_CONDITION` (`32d4d190`, called once for `AND_OPERATION` and once for `OR_OPERATION`): each `CONDITION_%d` (from 0, stops at the first missing) has `BUFF_APPLY_CHECK_CONDITION_TYPE`.
  - `BACCT_CHECK_UNIT_HP_RATE` (1): `HP_RATE` = float list, `CHECK_HP_RATE_UP` = bool list.
  - `BACCT_CHECK_BUFF_TEMPLET_ID` (2): `CHECK_BUFF_TEMPLET_ID[1]` = float, `APPLY_WHEN_HAVE[1]` = bool.
  - A condition whose type is 0 or unreadable is not stored.
  - `FALSE` in this data is March's numeric global (`Enum.src.lua:2`), and `KLuaManager::GetValue( int, bool& )` reads it as a number (`LuaCommon/KLuaManager.cpp:696`). So `{ FALSE }` does parse as false.
- The level ctor (`320844a0`) copies `ACCUMULATION_AT_ONE_GO` and level-slices the conditions (`32d4d030`): it takes element `min(level−1, n−1)` of each list and drops a condition whose lists are both empty.

**Apply path.** Details are in the `322a83b0` and `3229dee0` rows. The differences from March are:
1. The "templet missing" case is a silent `false` in June. March raises a popup (Phase 3 review R4: 87 of them).
2. After the rate/RESIST checks, there are four new rejections:
   - `IsImmuneBuffBySocketOption(BTI)` (user slot 179: `IsImmuneBySocketOption( BTI→EDT )` via `CX2Game` `322cc0a0`) → RESIST text;
   - immune-by-buff (`322a80a0`, BBT 94 stores);
   - the factor's apply conditions (`3227cad0`/`3227c4b0`);
   - immune-to-control-debuff (`3227b790`, BBT 95 list + `IsNonControlDebuff`).
3. A pre-hook (slot 250) and a post-hook (slot 251). Both are empty in the base class; only `CX2GUUser` overrides them, for June-only BTIs and skills.
4. BTI 265/266 (HAMEL armor/wing) is skipped, and still returns true, while 347/348 (the `_EVENT` versions) is on.
5. The stacking mode is the unit's per-BTI override (`+0x6dd8`, written only by Rose code) or else the templet limit. Modes:
   - 0: duplicate, as March.
   - 1: by `BRDO`. `NORMAL` (1) erases the old buff and pushes the new one. `ADD_TIME` (2) is March's `SetFactor` on the existing buff. `NOTDEFINED` (0) does nothing when other buffs exist.
   - ≥ 2: re-application during the delay timer returns false. Otherwise the timer is reset to `ACCUMULATION_DELAY_TIME`, then as March, compared against the mode value.
6. `ACCUMULATION_AT_ONE_GO`: up to N more `SetFactor`s on the found templet while its count is below the limit.
7. `PushNew` adds the delay gate and timer, a per-BTI apply-count map (`+0x7628`; its only reader is NPC code `3275886a`), and a sync counter (`+0x6dd0`, Phase 8).
8. `OnFrameMoveBuff` ticks the delay timer down by the elapsed time while it is > 0.
9. `OnFrameMoveFinalizerAndCheckFinish` (`3236c570`) handles `ACCUMULATION_DECREASE_WHEN_FINISH`. A finished finalizer whose templet has the flag runs three steps in order:
   - decrease the count by N, if count > N;
   - finalizer slot 8, which only `BFT_TIME` overrides (reset the remaining time);
   - every behavior's slot 7, where ChangeStat `32e73500` rescales its value from the old count to the new count and calls `UpdateAdditionalStat`.

   If all three succeed, the displayers restart and the buff keeps running. All 5 June templets with the flag are `BFT_TIME`-only. Four use only ChangeStat behaviors; `HEAVYFIREARMS_MASTERY` uses BBT 89, which is Phase 6.
10. `IsNonControlDebuff` (`32369cc0`) gains an `IS_DEBUF` gate and BBT 104.

**Same-owner rule: June's client has none.** `NOT_APPLY_BUFF_WHEN_SAME_BUFF_OWNER` is not among `3236a9b0`'s 17 keys, and `322a83b0` has no owner comparison. Like `EXP_RATE`, `IS_PARTY_BUFF`, `DROP_RATE`, `VP_RATE` and `LUCKYTIMINGEVENT_*`, it is a server-side key (Phase 1 table).

**Consumers outside the apply path (hand-offs, not ported in 4a).**
- **`IS_STORE_BUFF`:** June's clear-all with a keep-store flag (`3229fb10`, 6 callers) and a gage lookup (`32499c10`).
- **`IS_INSTANCE_KILL_DEBUFF`:** `CX2GUUser` slot 200 (`31fe4150`) → `DamageReact` stores it in a protected bool (`32082710`, user +0x6ba4). That bool's readers are not mapped.
- **`NOT_SAVE_GAGEMANAGER`:** no reader found with the +0xb9 sweep (it is a common offset; noisy).
- **Custom-function slot 7** (`32275860`): Phase 9.
- **The `+0x6dd0` counter:** Phase 8.

**June's by-ID entry `322a6960` (`SetBuffFactorToGameUnitByBuffFactorID`, vtable slot 135), diffed against March's (`X2GameUnit.cpp:11069`).**
- Same shape: `GetBuffFactorPtr`, `CanApplyBuffToGameUnit`, `GetClonePtr(level)`, `SetGameUnitBuffFactor`, `SetBuffFactorToGameUnit(factor)`.
- **`+0x210` `CanApplyBuffToGameUnit` now takes `(BTI, factor)`.**
  - Base `3229a2e0` ignores both and equals March's base (not immune to extra damage, HP > 0, state ≠ `GUSI_DIE`).
  - NPC override `327bdb60`.
  - User `32045dc0`: March's checks (base, plus the state is neither of the two die states from slots `0x460`/`0x464`). It adds two gates keyed on templet bytes `+0xa9` and `+0xaa`: party membership through `g_pData+0x5c`, and the factor's creator UID against the unit. **The only writer of those two bytes is the templet ctor (`326c4995`, zero).** No parsed key or loader sets them, so for every June templet the user check reduces to March's. Nothing is ported; if a later phase finds a writer, these gates come with it.
- **`+0x770`:** user only, when the unit is a user and the last argument is false. It scales BFT 15/16 factor values. Phase 8 (hand-off above). *Corrected in 1c item 4: it is skill-memo scaling (entry type 15 = `BFT_TIME` duration, 16 = behavior values), not BFT 15/16; Phase 10.*
- **`+0x234`:** empty in every unit class (`322b5630` only releases its argument). Nothing to port.
- Checked: no March subclass overrides `SetBuffFactorToGameUnit( const CX2BuffFactor& )`. June's slot 137 is the same `322a83b0` in GameUnit, GUUser and GUNPC.

### Phase 4a — Done (2026-10-03), launch check pending

**What changed (`SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY`, `KTDXLIB/Always.h`).**
- 15 files, insert-only (867 lines added, 0 removed). Every inserted byte is ASCII and CRLF, and every `file` signature is unchanged. The bodies that were replaced (`ParsingScript`, `SetBuffFactorToGameUnit`, the finalizer finish check) stay intact in `#else`.
- **`X2BuffTemplet.cpp`:**
  - `ParsingScript` is June's `3236a9b0`.
  - The finalizer loop has June's `ACCUMULATION_DECREASE_WHEN_FINISH` branch (`3236c570`), with three new helpers: `DecreaseAccumulationCountNow` (`32369e40`), `ReapplyBehaviorForAccumulationDecrease` (`3236a930`) and `RestartDisplayerForAccumulationDecrease` (`3236a190`, without June's stack-count icon refresh, since March's gage has no count).
  - `IsNonControlDebuff` gains June's `IS_DEBUF` gate and BBT 104 (`32369cc0`).
- **`X2BuffTemplet.h`:** 8 new members with getters, set in both ctors.
- **`X2BuffFactor.{h,cpp}`:**
  - `BUFF_APPLY_CHECK_CONDITION_TYPE` / `BUFF_REPLACE_DETAIL_OP` enums and the `BUFF_APPLY_CHECK_CONDITION` struct.
  - `ACCUMULATION_AT_ONE_GO` and `APPLY_CHECK_CONDITION` parsed (`32d4d850`, `32d4d190`) and level-sliced (`32d4d030`).
  - `BTI_BUFF_HAMEL_SECRET_DUNGEON_ARMOR/WING(_EVENT)` 265/266/347/348 added, because the apply path compares them.
- **Behavior slot 7** (`OnAccumulationCountDecreased`): base `false`; ChangeStat rescales (`32e73500`).
- **Finalizer slot 8** (`RestartForAccumulationDecrease`): base `false`; Time resets to `m_fFullDurationTime` (`32ec6b30`). That is a new member set at the end of both `SetFactor` and `SetFactorFromPacket`, as June sets +0x28.
- **`X2GameUnit.{h,cpp}`:**
  - `SetBuffFactorToGameUnit` is June's `322a83b0`.
  - `PushNewBuffTempletToGameUnit` adds the delay gate/timer and the per-BTI apply count (`3229dee0`).
  - `OnFrameMoveBuff` ticks the delay timer (`3229d620`).
  - New helpers: `CheckBuffApplyCondition` / `CheckOneBuffApplyCondition`, `HaveUnfinishedBuff`, `SetBuffAccumulationLimitOverride`, `GetBuffAppliedCount`.
  - `IsImmuneBuffByBuffBehavior` and `IsImmuneNonControlDebuffByBuffBehavior` return `false`. That is exactly June's result while no BBT 94/95 buff exists; Phase 7 fills them when it adds those stores.
- **`X2GUUser`:** `IsImmuneBuffBySocketOption` (`31ff8fd0`).
- **`X2Game.h`:** the BTI→EDT map (`322cc0a0`, nine cases). The EDT numbers were checked equal in March's and June's Enum.
- **`X2BuffTempletManager.cpp`:** one `[JUNE_BUFF] unsupported (stored, consumer not ported): ...` line after the load summary. It counts `IS_STORE_BUFF`, `IS_INSTANCE_KILL_DEBUFF` and `NOT_SAVE_GAGEMANAGER`, and is printed only if any count is non-zero.
- **Not ported, by design:**
  - the `+0x6dd0` sync counter (Phase 8);
  - the user pre/post-apply hooks (June-only BTIs and skills);
  - custom-function slot 7 (Phase 9);
  - `BFT_STICK_SHAKE`'s finish hook (Phase 8);
  - the consumers of `IS_STORE_BUFF` / `IS_INSTANCE_KILL_DEBUFF` (stored and counted).

**Behavior changes on March's current data.**
- **Socket-option immunity now blocks the buff itself.** A user with an EDT-immunity socket option who passes the roll gets RESIST for the nine mapped debuffs (fire, poison, stun, …). March only checked that roll for the damage-data extra damage.
- **A factor whose BTI has no templet** is one `[JUNE_BUFF] BTI n has no templet ...` line per BTI, not a MessageBox. This closes the Phase 3 review's R4 popups, and the line names the BTI R4 could not identify.
- **Unchanged:**
  - Limit 0/1 buffs: `BRDO_ADD_TIME` is the default when the key is absent, and that is March's behavior.
  - Limit ≥ 2 buffs: the delay is 0 by default.
  - March's templets all carry `ACCUMULATION_MULTIPLIER`.
  - `IsNonControlDebuff`: both callers (`X2GUEL.cpp:3597`, `:9340`) already require `IsDeBuff()`, and no March data uses BBT 104.

**Census.** `buff_census.py --phase 4a` shows **0 unread templet-level and 0 unread factor-level keys** (baseline 17 + 4).
- 13 keys are now listed apart as "not read by June's client either" (the `NOT_READ_BY_JUNE_CLIENT` table in the script):
  - **Templet level:** `EXP_RATE`, `IS_PARTY_BUFF`, `NOT_APPLY_BUFF_WHEN_SAME_BUFF_OWNER`, `DROP_RATE`, `VP_RATE` and `LUCKYTIMINGEVENT_*`.
  - **Factor level:**
    - `DISPLAYER`: `32d4d850` has no such read.
    - `APPLIED_SKILL_ID`: June reads it only at damage-data level.
- The full report still runs, rc=0.

**Build and deploy.**
- `KTDXLIB` / `X2Lib` / `X2` built at `US_SERVICE`, rc=0, 0 errors; both `stdafx.cpp` were touched first. `X2ServerProtocol` was not rebuilt, since nothing under it changed.
- `x2.exe` is 19,009,024 B (07:43:01, md5 `a2890447…`) and contains the new strings: `has no templet: a factor for it was not applied`, `unsupported (stored, consumer not ported)`, `ACCUMULATION_AT_ONE_GO`, `CONDITION_%d`, `BUFF_REPLACE_DETAIL_OPERATION`.
- Deployed as `X2_offline.exe`; the md5 matched after the copy. **Not launched.**
- **Pairing:** the Phase 2 `Enum.lua` is already packed (Phase 3 review). This exe needs no new Lua.

**Launch check: met for load (user play-test, 2026-10-03 10:00–10:05, exe md5 `a2890447…`).**
- `ErrorLog.txt` matches the expected lines exactly: duplicate BTI 87, both `_INT` not-found lines, ordinal rejects 481/482/483/372/373, and `load summary: templets 320 loaded, 0 failed, 1 duplicate; factors 361 loaded + 0 placeholder(s); _INT factors 0 loaded + 0 placeholder(s); ordinal rejects 5 (+ 0 _INT)`.
- There was no `unsupported (stored ...)` line, and `State.log` has 0 buff errors or popups.
- The session covered two dungeon runs and the battle field, 43 kills and 7 quick-slot uses, with 0 `UNHANDLED`/`EXCEPTION` and a clean quit.
- The Ranox secret stage was not visited, so the `BTI n has no templet` line is still unexercised.
- In-game buff behavior (awakening, recast refresh, monster debuffs, potion icons) is as reported by the user.

Original expectation, kept for reference. Expected with March's data:
- the same load summary as the Phase 3 review's exit check 1;
- **no** `unsupported (stored ...)` line, because March's templets carry none of those keys;
- in the Ranox secret stage, `BTI n has no templet` lines in place of R4's popups.

A trial of June's files would also exercise `ACCUMULATION_MULTIPLIER`'s new default. The 9 templets the Phase 3 hand-off named should stop failing, as long as R1's `SKILL_SLOT_TYPE` precondition holds.

**Trial prediction with this exe (`buff_trial_predict.py`, which now reads the flag).** The predictor drops March's "multiplier required" rule when `SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY` is defined.
- **With `SKILL_SLOT_TYPE` defined:** `templets 901 loaded, 100 failed` (was 894/107). The 100 failures are exactly the census set: of the 9 multiplier-less templets, 7 now load and 2 fail on an unhandled type. Factors are unchanged (1024 + 106, `_INT` 69 + 0).
- **As staged (R1 truncation):** unchanged at 713/61, 779 + 63, because all 9 of those templets come after the abort at BTI 820.
- The trial directory's `X2_offline.exe` is still Phase 3's (`841b930a…`). Copy this exe there before running the trial with Phase 4a.

**Review.** Required (Opus 5.5); not yet run. Spot-check suggestions:
- `322a83b0`'s BRDO switch (`322a86cd`–`322a87be`);
- the `3236c570` decrease chain;
- `32d4d190`'s condition reads;
- **HP-rate operand order in `3227c4b0`.** The port reads stat `+0x20`'s protected float at `+4` as now-HP and the one at `+0x10` as max-HP (rate = now/max). The data (`HP_RATE 0.2`, `CHECK_HP_RATE_UP FALSE` = "at or below 20%") supports that, but the gauge layout was not confirmed in asm.

**Cost.** Not recorded (no `/cost` figure in-session).

### Phase 4a — Review (2026-10-03, Opus 5.5 high)

Independent review of `a546850`, `9133871` and `aeb6a21` (`SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY`). It used fresh scratch scripts only; none of the Phase 4a session's scripts were reused. They were:
- Ghidra decompiles and disassembly of every function named in the task, plus their callees;
- raw capstone disassembly from `unpacked_x2.exe` for the addresses where Ghidra has no function (`32e73500`, `32e72980`, the `32ec34a5` and `32ec55d8` regions);
- `cl /EP` of `X2GameUnit.cpp` at `US_SERVICE` defines, with the C++ enum values evaluated from the output;
- a displacement scan of June's `.text`, for the BBT 94/95 unit stores and for byte stores at templet `+0xa9`/`+0xaa`;
- a fresh `elstools decompile-batch` of March's live buff bytecode (sizes equal the live `data036.kom` entries), run under the bundled `luajit` with a stub manager, followed by an old-rules/new-rules simulation;
- an unifdef of the commit with the flag **undefined**, compared against `9133871^`.

Ghidra was used read-only.

**Verdict: no code defects, so nothing was changed or rebuilt.**
- The port matches June in every function checked, and March's current data behaves exactly as before.
- There are three Low findings. R1 is a `BFT_TIME` packet-path gap, dormant on March data and handed to Phase 4b. R2 is line-ending hygiene. R3 is a flag dependency.
- There are four doc corrections.

| # | Check | Result |
|---|---|---|
| 1 | Ghidra spot-checks (rule 10) | **All four pass; no escalation.** (a) **BRDO switch:** `322a86cd` `SUB EAX,0`/`JZ 322a87be` = mode 0 (count, then push with the count); `DEC`/`JZ 322a8761` = mode 1: `[templet+0x60]==1` → erase `3229e160(factor BTI)` → push 1, `==2` → find → `3236d400` (SetFactor) or push 1, else fall to `ACCUMULATION_AT_ONE_GO`. Mode ≥ 2: `322a8e20` true → `XOR AL,AL` (false); else `[+0x4c]=[+0x48]`, count `<` mode → SetFactor, else `3236a6f0` (only-finalizer). (b) **`3236c570`:** finalizer slot 1 (OnFrameMove), slot 4 (DidFinish), `[+0x40]==1`, `32369e40([+0x44])`, finalizer slot 8, `3236a930`, then `3236a190(unit, BTI)`; any failure sets `+0x65`. (c) **`32d4d190`:** `CONDITION_%d` from 0 until the first missing. A type that is 0 or unreadable is not stored; a non-zero unknown type is stored with empty lists. Type 1 reads `HP_RATE` (`32d41a80`, no count) and `CHECK_HP_RATE_UP` (`32d41f00`: index 1.. until the first fail; `32fe9ed0` is `lua_isnumber` then `!= 0`, the same as March's `KLuaManager::GetValue(int, bool&)`). Type 2 reads `CHECK_BUFF_TEMPLET_ID[1]` and `APPLY_WHEN_HAVE[1]`. The OR flag is set from the argument. (d) **HP-rate operands in `3227c4b0`:** rate = (protected float at `gage+0x20 → +4`) / (the one at `+0x10`). March's `CX2GageData::Gage` is `bool bShow; KProtectedType<float> fNow; fMax;`, so `+4` is now-HP and `+0x10` is max-HP: **now/max, as ported.** With no gage data the rate is 1.0. The flag true needs rate ≥ `HP_RATE`; false needs ≤. |
| 2 | June parity, the rest | **`3236a9b0`:** the order, required keys and defaults match the RE table; the guard bits tie each read to its key. BRDO absent → 2; multiplier absent → `0x3f800000`, inserted with UCM 0 = 0; decrease count → 1; `REGIST_TYPE` inserted only if > 0.0; `DISPLAYER` optional (a parse failure fails the templet); `BUFF_NAME`/`BUFF_DESC` 2674/2606. The only `MOV AL,1` is at `3236c436`, and every failure exits through `3236ab1a`. **`32d4d850`:** BTI, `RATE`, `IGNORE_REGIST_FOR_RATE` (required), `ACCUMULATION_AT_ONE_GO` (uint, 0), `APPLY_CHECK_CONDITION` AND(0)/OR(1), BEHAVIOR, FINALIZER. **`32d4d030`:** index max(level−1, 0) clamped to the last element; a condition with both lists empty is dropped. **`322a83b0`:** null templet → false; rand > rate → false; rand > rate×(1−regist) → RESIST; slot `0x2cc` (179) → RESIST; `322a80a0` → false; `3227cad0` false → false; `3227b790` → false; pre-hook `0x3e8`; empty vector → push 1. 265 checks `3227b860(0x15b)` = 347, 266 checks `0x15c` = 348, and both skip with `MOV AL,1`. Mode from the `+0x6dd8` map, else the templet limit. `ACCUMULATION_AT_ONE_GO` uses the **templet** limit, not the override. Post-hook `0x3ec` and `32275860` follow. **`3229dee0`:** the delay gate is in the same condition as the immune-debuff check; `+0x4c=+0x48` and the unique number are set before SetFactor; on success `+0x6dd0`++ (Phase 8), the debuff/buff count, slot `0x24c`, push_back, then `+0x7628[BTI]`++. **`3229d620`:** tick before the buff's frame move; the compiled branch is `fElapsedTime` (`X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE` is on). **`32369e40`:** subtracts only if `(float)n < count`. **`3236a930`:** returns the last non-null behavior's slot-7 result, false with no unit. **`3236a190`:** displayer slot 1 and `+0xc = 1`; icon refresh not ported, as documented. **`32e73500`:** "before" = now + `[+0x44]` with the flag, else now + 1; types 1/2 → V/before×now, 3 → 1 + now×(V−1)/before (March `BCT_FIX/RELATION/PERCENT` = 1/2/3); then `3226fd70` (update-or-insert = March's `UpdateAdditionalStat`); null unit or templet → false. The multiplier read at the top is popped unused. **`32ec6b30`:** writes `[+0x28]` into the protected duration only if different, clears `+0x38` (= `m_bIsAlreadyNotifyDurationTime`), and returns true on both paths. **`32369cc0`:** `IS_DEBUF` gate, BBT {3,4,5,35,46,48,51,60,61,64,65,104}. **`322cc0a0`:** 25→9, 31→18, 37→3, 39→1, 41→4, 52→24, 60→8, 80→17, 99→55. Both tables are equal to March's enum values from `cl /EP`, as are `BACCT_*`/`BRDO_*` against June's `Enum.src.lua:11741`/`:14240`. **Except R1**, the `+0x28` packet path. |
| 3 | Claims made in the doc | **13 excluded keys:** no templet-level key appears in the June binary (plain, UTF-16, or `xorstrings.tsv`). `DISPLAYER`'s only string user is `3236a9b0`. `APPLIED_SKILL_ID` is read by `31f7aab0` (CEffect), `32024ff0` and `321e48e0` (effect set) and `32632390` (damage data), never inside a factor table. **The exclusion holds**; see correction 2. **`+0xa9`/`+0xaa`:** of 126 one-byte stores with those displacements, 28 are not stack stores. Inside templet code they are the ctor (`326c498e` word store covering `+0xa8/+0xa9`, `326c4995` `+0xaa`) and the copy ctor `322ad860`. None of the 18 writer functions calls `GetBuffTempletPtr` `326c3170`. `32045dc0` reads the manager's templet, and with both bytes 0 it falls through to March's base and die-state checks. **Holds**; see correction 3. **Stubs:** `+0x65dc`/`+0x65ec` sit in sub-objects `+0x65d4`/`+0x65e4`, touched only by `322780f0`/`32278120` (BBT 94 class), `32278840`/`32278880` (BBT 94 DoFinish `32e75bd0`), the ctor `322a2700` and the dtor `322a0730`. List `+0x65c4`: `3227b6b0` (BBT 94) and `3227a910` (DoFinish). List `+0x75ac`: `3227aa40` (BBT 95 SetFactor `32e75c00` and its slot 13 `32e728d0`) and `32279a10`. **Holds.** |
| 4 | March side | **Copy ctor:** all 8 new members are copied and all 8 are set in the default ctor. The `operator=` declarations in `CX2BuffTemplet`/`CX2BuffFactor`/`CX2BuffFinalizerTemplet` are private and undefined (copy ban), and the Time finalizer clones through its implicit copy ctor, so `m_fFullDurationTime` travels. **`BUFF_FACTOR`:** default, copy and levelled ctors carry both fields; `operator=` is implicit. **Simulation** (March's live bytecode, packed Phase 2 `Enum.lua`): 321 templets (320 + duplicate BTI 87) and 366 factors (361 + 5 ordinal rejects), as in the launch log. Limits: 0×11, 1×291, 2×3, 3×5, 5×7, 10×4. All 19 stackable templets carry `ACCUMULATION_MULTIPLIER`. **0** templets or factors carry any new key, and load outcome, apply branch, multiplier, delay and decrease are identical for all 321. A byte scan of all 4,842 March bytecode `.lua` (`kom_contents` + `march2014`) finds the new key strings only in `Enum.lua`'s enum names. **`IsNonControlDebuff`:** 14 non-debuff templets flip (BBT 35/48), unobservable because both callers (`X2GUEL.cpp:3597/9340`) gate on `IsDeBuff()`. **Missing templet:** `CX2JuneBuffCapture::Line` → `KLOG("ErrorLog.txt")` under a lock, once per BTI (a static set), never a MessageBox. **Socket immunity:** the base virtual returns false and only `CX2GUUser` overrides it (one definition of `IsImmuneBySocketOption`). |
| 5 | Hygiene | All 15 code files are insert-only (`--numstat` deletions 0); high-byte counts and `file` signatures are unchanged; inserted bytes are ASCII. Inserted lines are CRLF everywhere, matching their region **except `X2Game.h`** (R2). **With the flag undefined, all 14 X2Lib files reduce byte-for-byte to `9133871^`**, apart from inserted blank and `//{{ … //}}` marker lines outside the flag, so every replaced body is intact in `#else`. Every inserted `#else`/`#endif` carries the flag name, and `#define SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY` appears only at `KTDXLIB/Always.h:4721`. |
| 6 | Tools | `buff_census.py --phase 4a` rc 0: no templet-level or factor-level section, so **0 unread**, and 13 keys excluded (with counts). The full run gives rc 0, 236 lines. `buff_trial_predict.py`: flag on → `1001 entries, 100 fail` (**901/100**); flag off, by exec with only the `TEMPLET_PARITY` assignment forced to False and `Always.h` untouched → `107 fail` (**894/107**). Its source has 0 control bytes (only TAB/LF), is all ASCII, and uses LF line endings. |
| 7 | Deploy | `X2/US_SERVICE/x2.exe` and `$DATA/X2_offline.exe`: 19,009,024 B, md5 `a2890447e47074ee57a47cf4cf8cda01`. The game directory's exe names, read programmatically, are `x2.exe` and `X2_offline.exe`. `x2.exe` was linked at 07:43:01, after the newest touched source (`X2Lib/stdafx.cpp`, 07:33:22); `X2Lib.lib` 07:42:20, `KTDXLIB.lib` 07:34:39. The commit time (09:49) is later than the build, as expected. The tree is clean, and `git log 9133871..HEAD -- X2Lib X2 KTDXLIB X2ServerProtocol` is empty. All 8 strings checked are present in both files, including `ACCUMULATION_DECREASE_COUNT_WHEN_FINISH`, `APPLY_WHEN_HAVE` and `CHECK_HP_RATE_UP`. |

**Findings**
- **R1, Low (dormant): the `BFT_TIME` packet path doesn't carry June's full duration.**
  - **June.** `GetFactor` `32ec5290` sends `[duration, frame count, +0x28]`. `SetFactorFromPacket` `32ec53d0` sets `+0x28 = values[2]`, or **0** when the packet has only two values (`32ec55d8`/`32ec55ed`).
  - **The port.** March's `GetFactor` (untouched) sends two values, and the port's `SetFactorFromPacket` stores the time *remaining at receipt*. So on a peer-synced finalizer, a decrease-on-finish restart runs for less than the full duration.
  - **Not reachable today:** no March templet has `ACCUMULATION_DECREASE_WHEN_FINISH`. It becomes reachable with June's 5 templets (Phase 13), and only for peer-synced (packet-built) finalizers.
  - **Hand-off to Phase 4b (`BFT_TIME` runtime parity):** push `m_fFullDurationTime` as `values[2]` and read it back on receipt. Appending keeps the existing float order.
  - **Phase 4b must decide one thing and record it:** June's missing-`[2]` fallback (0) finishes the buff on the next frame, while the port's (remaining time) is gentler.
  - If 4b doesn't close this, Phase 13 ships the gap.
- **R2, Low (hygiene): `X2Game.h`'s 19 inserted lines are CRLF inside an LF region.** The surrounding lines 1502–1528 are LF, and the file is 1954 LF / 28 CRLF.
  - It is cosmetic: MSVC accepts mixed line endings and the line count is the same.
  - It is not changed here, because doing so would force a rebuild and break the play-tested `a2890447…` pairing.
  - Normalise those lines to LF the next time a phase touches `X2Game.h`.
- **R3, Low: `SERV_IRUHADEV_JUNE_BUFF_TEMPLET_PARITY` requires `SERV_IRUHADEV_JUNE_BUFF_LOADER`.** `CX2BuffFactor::ParsingApplyCheckCondition` is declared only inside the LOADER block (`X2BuffFactor.h`), but defined under PARITY alone. So undefining LOADER while PARITY is on fails to compile; the reverse (PARITY off) is clean (check 5). Note the dependency in PARITY's `Always.h` comment the next time that file is edited.
- **Info.**
  - **June-only multiplier in `BFT_TIME`.** June's `BFT_TIME::SetFactor` `32ec27d0` applies one more debuff-duration multiplier (unit protected float `+0x7428`, non-world debuffs only) before setting `+0x28`. March has no such system; it belongs with Phase 4b's `BFT_TIME` diff.
  - **June's finish pass.** June's `OnFrameMoveBuff` runs its finish pass separately, and calls the BFT 15 finish hook `3236a6a0` from `3236c570`. Both are pre-existing structure or Phase 8, not 4a.
- **funcmap.tsv:** 13 rows added ("Buff Phase 4a review"). They cover:
  - the Time finalizer's `SetFactor`/`GetFactor`/`SetFactorFromPacket` (vtable `33537e1c`);
  - the base behavior slot 7 `32e72980`;
  - the BBT 94/95 store helpers;
  - BBT 95's slot 13.

  `322ad860` was raised from candidate to confirmed.

**Doc corrections**
1. **Done note, finalizer slot 8:** "set at the end of both `SetFactor` and `SetFactorFromPacket`, as June sets +0x28" is right for `SetFactor` only. June's `SetFactorFromPacket` sets `+0x28` from the packet's `values[2]` (0 if absent); see R1. The inline comment in `X2BuffFinalizerTemplet.cpp`'s `SetFactorFromPacket` insertion says the same and should be corrected when Phase 4b touches that function.
2. **RE, `ParsingBuffFactor`:** "June reads `APPLIED_SKILL_ID` only at damage-data level" → it is also read at effect level (`31f7aab0`) and effect-set level (`32024ff0`, `321e48e0`), and never inside a factor table. The exclusion is unchanged.
3. **RE, by-ID entry:** "The only writer of those two bytes is the templet ctor (`326c4995`, zero)" → the ctor writes `+0xa9` with the word store at `326c498e` and `+0xaa` at `326c4995`; the copy ctor `322ad860` copies both. The conclusion is unchanged.
4. **Done note:** "Every inserted byte is ASCII and CRLF" is true, but CRLF does not match `X2Game.h`'s LF region (R2).

**Review cost.** `/cost` is not readable from inside the session; ask the user.

## Phase 4b — Existing BBT/BFT/BDT parity

**Scope.** For each of the 73 existing BBT, 14 BFT and 14 BDT, diff June against March in four places, and port June in full wherever they differ:
- the factor parse case;
- the level-slice case;
- the templet class (`ParsingBehavior`, `SetFactor`, `DoFinish`, `OnFrameMove`, and `GetFactor`/`SetFactorFromPacket`, which must keep the same float order);
- the unit-side store and read sites.

Known new keys inside existing types: `SKILL_CATEGORY_n`, `MULTIPLY_DAMAGE_RATE`, `VALUE`, `BUFF_VALUE_CALCULATE_TYPE`, `SKILL_ID_LIST`, `ONLY_MY_UNIT`, `NOT_DELETE_EFFECT_SET_WHEN_FINISH`, `REPLAY_EFFECT_SET_ACCUMULATION_LIMIT1`, the per-class displayer keys, `FROZEN_TIME`, `HYPER_COUNT`, `OFFSET_POSITION`, and the rest of the ledger list. If this is too large for one session, split it at 4b-i (behaviors) and 4b-ii (finalizers and displayers).

Hand-offs from the Phase 4a review: `BFT_TIME`'s packet path must carry the full duration as a third value (R1: June `32ec5290`/`32ec53d0`; appending keeps the existing float order; decide the missing-value fallback), and June's `BFT_TIME::SetFactor` `32ec27d0` applies a debuff-duration multiplier March lacks (unit `+0x7428`).

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
- **Cost:** `/cost` cannot be read from inside the session; ask the user.

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

**Review cost.** `/cost` is not readable from inside the session; ask the user.

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

**Cost:** `/cost` is not readable from inside the session; ask the user.

## Phases 5–7 — New behavior types

Each new BBT follows an 8-step checklist:

1. Add the C++ enum, done in Phase 2.
2. Add the factor parse case in **both** the skill and non-skill macro lists.
3. Add the level-slice case. **Already done in Phase 4b-i:** June's whole `GetBehaviorFactorToLevel` (`32d47620`) is in, every new type's case included. Check the case is there; do not add another.
4. Add a `SetGameUnitBehavior` case where June's SGU `32d4d670` has one: 76, 78, 92 and 93 go to the ChangeStat SGU (`32d41700`), 80–84 to `32d4ce60`; no other new type has one. *(Corrected by the Phase 4b-i part 1 review; this used to read "if the type uses `BCT_RELATION_VALUE`".)*
5. Add the templet class and its `CASE_BEHAVIOR_TEMPLET` entry.
6. Add the unit store with insert and erase, plus June's read sites.
7. Cover the UDP layout through `GetFactor`/`SetFactorFromPacket`.
8. Log the type's first application under the DEBUG flag.

**Phase 5, group A: stat and multiplier types** (flag `..._NEW_STAT`). *Re-split by Phase 1; see "Per-type map".*
- **Phase 5 (Sonnet, transcription-ready):** 76 HYPER_DURATION, 85/86 *_COMMAND_MP_GAIN_RATE, 87/96 *_COMMAND_DAMAGE_RATE, 108/109 FINAL_DAMAGE_MULTIPLIER_NPC/USER. 85, 86, 87 and 96 need Phase 2's `AT_COMMAND`.
- **Phase 5b (Opus, same flag):** 78 TYPE_ADDITIONAL_ATTACK_MULTIPLIER, 106/107 FORCE_DOWN_*_ATTACKED (their readers use June DamageData fields not yet mapped to March), 79 NOW_FORCE_DOWN_PER_SECOND, 80–84 *_BY_CONDITION, 88 IGNORE_DEFENCE (own classes not yet decoded), 92 MAXIMIZE (needs June's damage-variance roll), 93 EVERY_ATTR_DEF_HAVE_BUFF_OWNER (owner-UID subclass). `buff_census.py --phase 5b`.

**Phase 6, group B: skill-scoped types** (flag `..._NEW_SKILL`). 75 DOUBLE_ATTACK_BUFF, 89 SKILL_DAMAGE, 90 BY_SKILL_ID, 91 CHARGE_MP_HIT_BY_SKILL_ID, 98 BY_SKILL_SLOTTYPE, 99 CONSUME_MP_RATE_BY_SKILLTYPE, 100 COOLTIME_RATE_BY_SKILLTYPE, 103 SKILL_COOL_TIME_SPEED. *Parse layouts are in the per-type map; the classes are not yet decoded, so this phase runs on Opus until a Phase 1b pass finishes them.*

**Phase 7, group C: flag and immunity types** (flag `..._NEW_FLAG`). 73 IGNORE_STATE_GUARD, 74 DETONATION_IMPOSSIBLE, 102 DETONATION_IMPOSSIBLE_GAGE_RESET, 77 INVINCIBLE, 94 IMMUNE_BUFF_DEBUFF (`IMMUNE_BUFF_TEMPLATE_ID`, `ALL_BUFF`/`ALL_DEBUFF`), 95 IMMUNE_NON_CONTROL_DEBUFF (on apply it also calls `DeleteNonControlDebuff`; coordinate with Phase 10), 97 CHANGE_MAX_CHARACTER_ADD_RESOURCE (moved here: a per-class virtual), 101 IGNORE_HP_DECREASE, 104 JUMP_IMPOSSIBLE, 105 IMMUNE_EDT. **111 MP_NATURAL_RECOVERY_IMPOSSIBLE and 112 REFLECT_PHYSIC are not implemented in June's client: enum only.**

**Phase 4a hand-offs these phases must pick up** (stubs and slots Phase 4a added for them):
- **Phase 6, BBT 89:** `CX2BuffChangeSkillDamageBehaviorTemplet` needs `OnAccumulationCountDecreased` (June behavior slot 7, `32e7c020`). Without it, `BTI_BUFF_HEAVYFIREARMS_MASTERY` (`ACCUMULATION_DECREASE_WHEN_FINISH`) ends at its first timeout instead of decaying.
- **Phase 7, BBT 94:** fill `CX2GameUnit::IsImmuneBuffByBuffBehavior` (June `322a80a0`). It reads the `ALL_BUFF`/`ALL_DEBUFF` counters and the `IMMUNE_BUFF_TEMPLATE_ID` lists. It returns false until this phase lands, which equals June's result with no BBT 94 buff on.
- **Phase 7, BBT 95:** fill `CX2GameUnit::IsImmuneNonControlDebuffByBuffBehavior` (June `3227b790`): if the BBT 95 list is non-empty, return the templet's `IsNonControlDebuff()`. Same stub rule. `IsNonControlDebuff` already has June's BBT 104 case.

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
- Port `BFT_AURA` 14, `BFT_STICK_SHAKE` 15, `BFT_USE_SKILL_BY_SKILL_SLOTTYPE` 16 and `BFT_ATTACK` 17. Each needs its enum (done in Phase 2), a parse case, a level-slice case, `SetGameUnitFinalizer`, a class with its `CASE_FINALIZER_TEMPLET` entry, a unit counter, the site that increments it, and **UDP sync of the counter**, alongside hit and hitted count in `X2UDPPacket.h:310`.
- Candidate hook sites (*superseded by 1c item 4, which maps June's real BFT 15/16 sites*):
  - `BFT_ATTACK`: `CX2GUUser::AttackResult` `X2GUUser.cpp:12721`.
  - `BFT_USE_SKILL_BY_SKILL_SLOTTYPE`: `CommonSpecialAttackEventProcess` `:20966` / `CountUsedSkill` `:32125`.
  - `BFT_STICK_SHAKE`: the grab-escape counter `m_iGrapShakeCount` `:14651`, plus NPC `SHOW_STICK_SHAKE` `X2GUNPC.cpp:5154`.
  - Confirm every one of these against June.
- **Phase 4a hand-offs:**
  - June's `PushNewBuffTempletToGameUnit` (`3229dee0`), `OnFrameMoveBuff` (`3229d620`) and the BFT 15 finish hook `3236a6a0` all bump a unit counter at `+0x6dd0` (and clear `+0x6dd4`). Find what it syncs and port it with the counters.
  - `EraseBuffTempletFromGameUnitByIterator` (`3229d320`) and `OnFrameMoveBuff`'s inline erase call `3236a6a0` (BFT 15: unit slot `+0x404`).
  - ~~`SetBuffFactorToGameUnit(ID)` (`322a6960`) calls user slot `+0x770` (`32028250`) to scale BFT 15/16.~~ *Corrected in 1c item 4: that slot is skill-memo scaling of `BFT_TIME` and behavior values, not BFT 15/16; it moves to Phase 10.* The real BFT 15/16 sites are in 1c item 4.

**Run with:** Opus 5.5, high effort. Review required. The UDP change means every peer must run the same build; say so in `MODS.md`.

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
```

## Phase 9 — Custom-function buffs

**Scope.**
- Port June's whole `CreateAndInsertCustomFunctionByBuff` switch and every `CX2CustomFunctionByBuff*` class it creates, for the 17 BTIs in the ledger. Include the Rose, Ain and Lu/Ciel cases (user decision).
- Port each hook body: CommonStateStart, FrameMove, StateEnd and DamageReact (`X2GUUser.cpp:14613/17894/19467/8327`).
- Where June moved per-class handling of FLASH_BANG or PANDEMONIUM_FEAR (`X2GUAra.cpp:13854`, `X2GURaven.cpp:21764`) into the buff system, follow June.
- Split into 9a and 9b by character family if needed.
- **Phase 4a hand-off:** June's `SetBuffFactorToGameUnit` ends with `32275860(factor)`, which calls slot 7 of every object in the custom-function-by-buff list (`unit+0x6734`; slots 3–6 are March's four hooks). Port slot 7 with the classes and call it at the end of the Phase 4a body.

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
- **Phase 4a hand-off: the user pre/post-apply hooks.** June's `SetBuffFactorToGameUnit` calls user slot 250 (`3201ffb0`) before stacking and slot 251 (`3204dcd0`) after. Both are empty in the base class.
  - Slot 250 handles `BTI_BUFF_CREST_OF_TIME(_UPGRADE)` (436/926; slot `+0x740`) and `BTI_AMUSEMENT_ITEM_STAT_BUFF` (807; a game-mode check, then `32d58dc0`). These are item buffs, so they are this phase's.
  - Slot 251 is a skill passive gated on `BTI_BUFF_SOUL_COLLECTOR` (462) / `BFI_BUFF_SOUL_COLLECTOR` (523), skills `0x76c9`/`0x76d3`. It belongs to the class port that brings that skill; record it there if this phase finds no item use.
- **Phase 4a hand-off: stored keys whose consumers are not ported.**
  - `IS_STORE_BUFF`: June's clear-all keeps flagged buffs (`3229fb10`, 6 callers) and the gage lookup `32499c10`. That is item/world-buff persistence, so it is this phase's.
  - `IS_INSTANCE_KILL_DEBUFF`: `CX2GUUser` slot 200 (`31fe4150`) → `DamageReact` → protected bool `+0x6ba4`. Owner: Phase 14 triage, unless an item phase finds it first.

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

**Precondition (Phase 4b-i part 1 review, 6c):** June's damage-manager reader `326309f0` must be ported (Phase 4b-i part 2) before June's factor data is adopted. Until then `CX2BuffCreateBuffFactortBehaviorTemplet` inserts created factors for NPCs too, and both NPC and user created factors apply on every attack type. June's 18 CREATE entries include `MS_DA_UP_PER_GROUP2/3` and `FEAR_OF_ABYSS`, which look monster-side, and two with `APPLY_TO_SPECIAL_ATTACK = TRUE` (`REASON_PRIDE_OBERON/OPHELIA`).

**Steps**
1. Compile June's `BattleFieldBuffTemplet.lua` and `BattleFieldBuffFactor.lua` (plus `_INT` per Phase 1) with mar-2014. Stage them in `march2014/data036/` and `kom_contents/data036_kom/`; they replace March's copies.
2. Rerun `buff_census.py`; it must report **0 unsupported**.
3. Run a local trial-load harness against the C++ parse expectations.
4. Build and deploy. Give the user one pack list naming every staged file across Phases 2, 10, 11, 12 and 13, together with this exe.
5. After the user packs, confirm with `elstools inspect` on the live koms. Then ask before launching. Confirm `ErrorLog.txt` shows the load summary with 0 placeholders and no `[JUNE_BUFF] unsupported` lines. Also confirm (Phase 3 review R1) that the loaded counts equal the source files' entry counts, and that no buff file logged `KEM_ERROR73`/`XEM_ERROR2`. A Lua runtime error truncates a file silently, and the first two checks still pass. Before staging, add `SKILL_SLOT_TYPE` (June `Enum.src.lua:17838`) and any other June-only global the adopted files read to `Enum.src.lua`.
6. Clear the Phase 2 to-dos: remove the `BFI_DEBUFF_TELEPORT_CONSUME_MP` alias from `Enum.src.lua` (only once `NewSkillTempletVer2.lua` no longer names it), and repoint `Item.kim`'s Heretic potions (85003830 / 85003831) from 362/363 to 481/482 (see "Phase 2 — Done").

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
