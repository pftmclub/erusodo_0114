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
| 74 DETONATION_IMPOSSIBLE | `+0x67ec` | `31feb9f0` (`CX2GUUser` slot 504), `3276cb20` |
| 77 INVINCIBLE | `+0x67fc` | `31fcee40`, `32063650` (`CX2GUUser` slot 499), `32758470` |
| 101 IGNORE_HP_DECREASE | `+0x680c` | `322726d0` |
| 102 DETONATION_IMPOSSIBLE_GAGE_RESET | `+0x681c` | `31feabd0` (`CX2GUUser` slot 308), `31feb9f0`, `3276cb20` |
| 104 JUMP_IMPOSSIBLE | `+0x42c0` (the insert also calls `3207b700`) | `31fe83a0`, `3276d980` (`CX2GUNPC` slot 278) |
| 110 AWAKE_IMPOSSIBLE | `+0x682c` | `31fd4460` (`CX2GUUser` slot 234) |
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

**Review.** The required Opus 5.5 review has **not** been run yet.

**Cost.** Not recorded: no `/cost` figure was available in-session. Ask the user.

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

**Phase 5, group A: stat and multiplier types** (flag `..._NEW_STAT`). *Re-split by Phase 1; see "Per-type map".*
- **Phase 5 (Sonnet, transcription-ready):** 76 HYPER_DURATION, 85/86 *_COMMAND_MP_GAIN_RATE, 87/96 *_COMMAND_DAMAGE_RATE, 108/109 FINAL_DAMAGE_MULTIPLIER_NPC/USER. 85, 86, 87 and 96 need Phase 2's `AT_COMMAND`.
- **Phase 5b (Opus, same flag):** 78 TYPE_ADDITIONAL_ATTACK_MULTIPLIER, 106/107 FORCE_DOWN_*_ATTACKED (their readers use June DamageData fields not yet mapped to March), 79 NOW_FORCE_DOWN_PER_SECOND, 80–84 *_BY_CONDITION, 88 IGNORE_DEFENCE (own classes not yet decoded), 92 MAXIMIZE (needs June's damage-variance roll), 93 EVERY_ATTR_DEF_HAVE_BUFF_OWNER (owner-UID subclass). `buff_census.py --phase 5b`.

**Phase 6, group B: skill-scoped types** (flag `..._NEW_SKILL`). 75 DOUBLE_ATTACK_BUFF, 89 SKILL_DAMAGE, 90 BY_SKILL_ID, 91 CHARGE_MP_HIT_BY_SKILL_ID, 98 BY_SKILL_SLOTTYPE, 99 CONSUME_MP_RATE_BY_SKILLTYPE, 100 COOLTIME_RATE_BY_SKILLTYPE, 103 SKILL_COOL_TIME_SPEED. *Parse layouts are in the per-type map; the classes are not yet decoded, so this phase runs on Opus until a Phase 1b pass finishes them.*

**Phase 7, group C: flag and immunity types** (flag `..._NEW_FLAG`). 73 IGNORE_STATE_GUARD, 74 DETONATION_IMPOSSIBLE, 102 DETONATION_IMPOSSIBLE_GAGE_RESET, 77 INVINCIBLE, 94 IMMUNE_BUFF_DEBUFF (`IMMUNE_BUFF_TEMPLATE_ID`, `ALL_BUFF`/`ALL_DEBUFF`), 95 IMMUNE_NON_CONTROL_DEBUFF (on apply it also calls `DeleteNonControlDebuff`; coordinate with Phase 10), 97 CHANGE_MAX_CHARACTER_ADD_RESOURCE (moved here: a per-class virtual), 101 IGNORE_HP_DECREASE, 104 JUMP_IMPOSSIBLE, 105 IMMUNE_EDT. **111 MP_NATURAL_RECOVERY_IMPOSSIBLE and 112 REFLECT_PHYSIC are not implemented in June's client: enum only.**

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
