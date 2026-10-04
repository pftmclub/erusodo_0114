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
| 5 | New BBT group A: ChangeStat types (76, 85–87, 96, 108, 109) | 2, 4b | C++ transcription | Sonnet 5.5 | high | required (Opus 5.5) |
| 5b | Group A: transcription-ready stat types (79–84, 93, 106, 107) | 5 | C++ transcription | Sonnet 5.5 | high | required (Opus 5.5) |
| 5c | Group A: rewritten hosts (78, 88, 92) | 5b, 4b | C++ (June host rewrites) | Opus 5.5 | high | required |
| 6 | New BBT group B: skill-scoped (75, 91, 103; classes of 89, 90, 98–100) | 5, 4b | C++ transcription | Sonnet 5.5 | high | required (Opus 5.5) |
| 6b | Group B: hosts of 89, 90, 98, 99, 100 | 6 | C++ (June host rewrites) | Opus 5.5 | high | required |
| 7 | New BBT group C: flags/immunity with ready readers (73, 94, 95, 104, 105) | 2, 4a | C++ | Opus 5.5 | high | required |
| 7b | Group C: rewritten hosts, per-class virtuals (74, 77, 97, 101, 102, 110) | 7 | C++ (new mechanisms) | Opus 5.5 | high | required |
| 8 | New finalizers + UDP counter sync | 2, 4b | C++ (new mechanisms) | Opus 5.5 | high | required |
| 9 | Custom-function buffs (17 June BTIs) | 4b | C++ (new classes) | Opus 5.5 | high | required |
| 10 | Buff groups + June-only script bindings | 3 | C++ + data | Opus 5.5 | high | required |
| 11 | Premium/world buffs (`BuffTemplet.lua`) + offline path | 3 | C++ + data (+ offline) | Opus 5.5 | high | required |
| 12 | Buff icons/UI assets | 2 | data | Sonnet 5.5 | medium | light |
| 13 | Adopt June buff data, build, hand off pack | 4–12 | data + build | Sonnet 5.5 | medium | required (Opus 5.5) |
| 14 | Play-test sweep + close-out | 13 | verify + root-cause | Opus 5.5 | high | none |
| 15 | Enchant system parity (added 2026-10-04) | – | RE + C++ + data | Opus 5.5 | high | required |

Phases 5–9 are independent of each other and can run in any order, except inside a group: 5 → 5b → 5c (92 needs Phase 4b's BBT 7/8 second members), 5 → 6 → 6b (91 shares 85/86's host function) and 7 → 7b. Phases 5–7b each have a ready-to-paste kickoff prompt and review prompt under "Phases 5–7"; nothing in them needs substituting. Phase 13 must come last among the data phases.

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
| `SERV_IRUHADEV_JUNE_ENCHANT_PARITY` | 15 |

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

**Review cost:** `/cost` is not readable from inside the session; ask the user.

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

**Cost:** `/cost` is not readable from inside the session; ask the user.

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

**Review cost:** `/cost` is not readable from inside the session; ask the user.

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

**Cost:** `/cost` is not readable from inside the session; ask the user.

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

**Review cost:** `/cost` is not readable from inside the session; ask the user.

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

**Cost:** `/cost` is not readable from inside the session; ask the user.

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

**Review cost:** `/cost` is not readable from inside the session; ask the user.

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

**Cost:** `/cost` is not readable from inside the session; ask the user.

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

**Review cost:** `/cost` is not readable from inside the session; ask the user.

## Phase 4b-iii — Unit-side stores and read sites (4b-ii stop item 7)

**Why this is still Phase 4b.** Phase 4b's scope (above) diffs four places per type. The fourth is "the unit-side store and read sites". 4b-i covered the behaviors' unit side, and 4b-ii diffed every finalizer and displayer class slot. What remains is the `CX2GameUnit` code those finalizer and displayer slots call, and the places that read what they store. Run it before Phase 5: it lives in `X2GameUnit.cpp`, where Phase 5 adds its members and readers, so any fix here should land on a clean baseline first.

**Scope.** For each item, diff June's unit function against March's counterpart, find every reader of what it stores, and diff those too:
- render param insert/erase `3227ac80` / `32279b20`, and the by-unit-type path through unit vslot 154;
- after-image `3227b220` / `3227a2f0`;
- weapon after-image `3227b300` / `3227a3a0`;
- impact point / hitted / normal attack inserts `3227b0a0` / `3227b160` / `3227b540` and erases `3227a180` / `3227a220` / `3227a4c0` (call shapes in the 4b-ii review, row 7);
- the slash-trace field `+0x4238`;
- the finalizer counters: hit `+0x4231` and hitted `+0x4230`, with every increment site. The March sites are in the ledger's "March runtime map".

Not in scope (already owned elsewhere): the per-instance visibility flags (EffectSet `+0x1bc`, particle `+0x1b0` via `32296da0`; effect port), HIT_COUNT `ATTACK_TYPE`'s per-type counters `32274800` (4b-ii hand-off 5), and BFT 15/16 counters (Phase 1c item 4 / Phase 8).

**Phase 4b is done when:**
1. Every item above, and every reader found, has a verdict in a table: **same**, **ported**, or **June-only, handed to <owner>**.
2. Every difference that March can reach is ported whole under `SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY`, with March's code in `#else` (no partial June updates).
3. `buff_census.py --phase 4b` still shows 0 type/key pairs.
4. If code changed: a flag-off unifdef per file, an `X2Lib` + `X2` build at `US_SERVICE`, a deploy as `X2_offline.exe` with the md5 recorded, and the user's play-test.
5. The review below finds no open code defect.

Then mark Phase 4b complete in this doc, and Phase 5 starts.

**Run with:** Opus 5.5, high effort. Review required if code changed; if the session ends read-only with every item "same", the review is the user's call.

```
Continuing the June buff/debuff port. Run this session on Opus 5.5 at high effort. Read
JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", the "Investigation ledger"
(its March runtime map), the "June buff function map", "Phase 4b", "Phase 4b-ii -- Done"
(its stop list), "Phase 4b-ii -- Review" and "Phase 4b-iii". Branch: mods/june-buff-port.
Never launch X2_offline.exe without asking first.

Your task (Phase 4b-iii, the last part of Phase 4b): diff the CX2GameUnit functions that the
finalizer and displayer slots call, and every site that reads what they store, against
June. The list is in "Phase 4b-iii -- Scope". Ghidra is read-only; many June slot bodies
have no Ghidra function, so read them from the raw asm. Port every difference March can
reach in full under SERV_IRUHADEV_JUNE_BUFF_BEHAVIOR_PARITY (insert-only, byte-level CP949
edits, original in #else, flag-off unifdef per file). Hand June-only systems to their owner
instead. Record a verdict table, confirmed funcmap.tsv rows and "Phase 4b is done when"
status in the doc. If code changed: rebuild X2Lib and X2 at US_SERVICE (touch stdafx.cpp
after a header edit), deploy as X2_offline.exe, record the md5, and ask before launching.
Another session may have uncommitted edits in the doc: stage only your own hunks. Commit.
Write the review kickoff into the doc. Record the cost line.
```

**Review prompt (fresh Opus 5.5, high effort):**

```
Reviewing the June buff/debuff port, Phase 4b-iii (fresh Opus 5.5 session, high effort).
Read JUNE_BUFF_PORT_PHASES.md -- "Starting context", "Phase 4b", "Phase 4b-iii" and its
Done note. Branch: mods/june-buff-port. Never launch X2_offline.exe.

Independently re-check the verdict table in the raw asm (Ghidra read-only), especially
every "same" verdict and every reader the session found; look for readers it missed.
Use fresh scratch tools. Run the flag-off unifdef on the committed blobs against the
pre-session commit, and confirm the deployed X2_offline.exe md5. Report numbered findings
(R1, R2...) with a severity. Fix a code defect under the same flag, rebuild X2Lib and X2 at
US_SERVICE, deploy, record the md5, and ask before launching. If nothing is open, mark
Phase 4b complete. Stage only your own hunks. Commit. Record the review cost line.
```

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
5. Review with no open code defect: **pending** (kickoff below).

**Cost:** `/cost` is not readable from inside the session; ask the user.

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

## Phases 5–7 — New behavior types

Phases 5–7 are split into seven sessions. Each has its own scope, its own run-with line and a **ready-to-paste kickoff prompt and review prompt with nothing to substitute**. Take the prompt of the phase you are starting, and no other.

| Phase | Types | Flag | Model / effort | Needs first |
|---|---|---|---|---|
| 5 | 76, 85, 86, 87, 96, 108, 109 | `NEW_STAT` | Sonnet 5.5 / high | Phase 2 (`AT_COMMAND`), 4b |
| 5b | 79, 80, 81, 82, 83, 84, 93, 106, 107 | `NEW_STAT` | Sonnet 5.5 / high | Phase 5 |
| 5c | 78, 88, 92 | `NEW_STAT` | Opus 5.5 / high | Phase 5b, 4b-i part 2 (BBT 7/8 second members) |
| 6 | 75, 91, 103, plus the classes, stores and readers of 89, 90, 98, 99, 100 | `NEW_SKILL` | Sonnet 5.5 / high | Phase 5 (91 shares 85/86's host) |
| 6b | the hosts of 89, 90, 98, 99, 100 | `NEW_SKILL` | Opus 5.5 / high | Phase 6 |
| 7 | 73, 94, 95, 104, 105 | `NEW_FLAG` | Opus 5.5 / high | Phase 4a stubs (done) |
| 7b | 74, 77, 97, 101, 102, 110 (111 and 112 are enum-only) | `NEW_FLAG` | Opus 5.5 / high | Phase 7 |

Every flag is `SERV_IRUHADEV_JUNE_BUFF_<name>` and is defined once, in `KTDXLIB/Always.h`. The sub-phases of a group share their group's flag (it is one logical change in several sessions).

**Changes from the earlier plan.** The doc used to list Phase 5b as one Opus session for 12 types and Phase 6 as one session for 8. Phase 1c items 1–3 then settled the classes:
- 79, 82, 83, 84, 93, 106, 107 are transcription-ready (item 3), and 80 and 81 are ready except for a second member that follows Phase 4b. They moved to a **Sonnet** session (5b). Run 5b on Opus instead if you prefer; nothing in the prompt changes except the model line.
- 78, 88 and 92 depend on host functions that June rewrote, so they stay on Opus (5c).
- Phase 6's classes, stores and readers are transcription-ready, but the host functions of 89, 90, 98, 99 and 100 are June rewrites. They are Opus-only (6b).
- Phase 7 is split into the types whose class and reader are ready (7) and the ones whose hosts June rewrote or whose per-class virtual March lacks (7b). **110 AWAKE_IMPOSSIBLE** was missing from the old Phase 7 list; item 2 settled it, so it is in 7b.

### Shared checklist (every phase here)

Each new BBT follows an 8-step checklist:

1. Add the C++ enum, done in Phase 2.
2. Add the factor parse case in **both** the skill and non-skill macro lists.
3. Add the level-slice case. **Already done in Phase 4b-i:** June's whole `GetBehaviorFactorToLevel` (`32d47620`) is in, every new type's case included. Check the case is there; do not add another.
4. Add a `SetGameUnitBehavior` case where June's SGU `32d4d670` has one: 76, 78, 92 and 93 go to the ChangeStat SGU (`32d41700`), 80–84 to `32d4ce60`; no other new type has one. *(Corrected by the Phase 4b-i part 1 review; this used to read "if the type uses `BCT_RELATION_VALUE`".)*
5. Add the templet class and its `CASE_BEHAVIOR_TEMPLET` entry.
6. Add the unit store with insert and erase, plus June's read sites.
7. Cover the UDP layout through `GetFactor`/`SetFactorFromPacket`.
8. Log the type's first application under the DEBUG flag (`SERV_IRUHADEV_JUNE_BUFF_DEBUG`, prefix `[JUNE_BUFF]`).

Rules for all seven sessions:
- Cite the June address in a comment at every new path. Never invent a value or a formula, and never port a raw struct offset.
- Types June uses only for characters March lacks (Rose, Ain, Lu/Ciel) are ported anyway (user decision).
- **Escalation rule (Sonnet sessions).** If the map does not settle a type (missing layout, store or read site), do **not** reverse-engineer or guess it. Skip that type, mark it "Opus-only" in the doc with what is missing, and keep going.
- **Opus sessions that hit a design decision** (a deviation from June, a DamageData key March lacks, a system March does not have): stop that type, write the question into the doc's Done note, and continue with the others. Hand the decision to the user. Do not choose silently.
- Done means: `buff_census.py` lists none of the phase's types, the build is rc=0 with the PCH invalidated (`touch X2Lib/stdafx.cpp`), and the strings are in the deployed exe. `buff_census.py --phase <N>` lists the whole census group (for 5b, 5c and 7b that is wider than the sub-phase), so check the named types only.
- Write a per-type **"how to see it in game"** line (a June BTI/BFI and where it comes from) into the Done note, for Phase 14.
- Record the session's `/cost` or usage in the Done note. Commit.
- A fresh Opus 5.5 review follows each phase (rule 10). Its prompt is under each phase. Review fixes go in the same phase's Done note.

---

### Phase 5 — group A: ChangeStat-class types (Sonnet)

**Scope.** Seven types, all transcription-ready (see "Per-type map", *Phase 5 — ChangeStat-class types*). All share March's `ParsingChangeStatBehavior` layout (`32d445d0`) and class `CX2BuffChangeStatBehaviorTemplet`.
- **76 `BBT_CHANGE_HYPER_DURATION`.** New `CX2AdditionalStatByBuff` member `m_AdditionalHyperDuration`, basic 0.0. It joins both ChangeStat groups (lvl `32d47957`, SGU `32d4d6ef`). It is an **option value**: seed it in `InitAdditionalStatOptionValue` (June `32270dd0`, March `X2GameUnit.cpp:12442`) from `m_SocketData.m_fHyperModeTime`, and set 0 in the reset branch. Reader: `CX2GUUser::GetHyperModeTime` (June `31fd1c00`, March `X2GUUser.cpp:23448`), `fTotal += fTotal * m_AdditionalHyperDuration.GetResultValue() * 0.01f`. The class overrides (Ain `32a4fe50`, `329e5180`/`329e56c0`) are ported with their classes, not here.
- **85 `BBT_CHANGE_RANGE_COMMAND_MP_GAIN_RATE` / 86 `BBT_CHANGE_MELEE_COMMAND_MP_GAIN_RATE`.** Two new members, basic 1.0. Reader in `CX2GameUnit::SetDamageExceptionProcess` after `fHitAddMP = attacker->GetChargeMpHit{Melee,Range}Attack()` (March `X2GameUnit.cpp:14259-14269`): `if ( attackType == AT_COMMAND ) fHitAddMP *= attacker->{86 melee, 85 range}`. Needs `AT_COMMAND` from Phase 2. Chung's virtual `32c16be0` is not part of this phase.
- **87 `BBT_CHANGE_MELEE_COMMAND_DAMAGE_RATE` / 96 `BBT_CHANGE_RANGE_COMMAND_DAMAGE_RATE`.** Two new members, basic 1.0. Reader in `GetFinalDamage`'s main path (`@32286270`–`322862b9`): `if ( attackType == AT_COMMAND ) dmg *= m_bMeleeAttack ? attacker->87 : attacker->96`. Needs `AT_COMMAND`.
- **108 `BBT_CHANGE_FINAL_DAMAGE_MULTIPLIER_NPC` / 109 `…_USER`.** Two new members, basic 1.0. Readers in `GetFinalDamage`, both the fixed-damage path and the main path: `inc2 = dmg * ( ( defender is GUT_USER ? attacker->109 : attacker->108 ) - 1 )`, added beside March's `fIncreaseDamage` (`X2GameUnit.cpp:2613-2616`, `2704-2707`).
- **No SGU case and no ChangeStat lvl case** for 85, 86, 87, 96, 108 and 109: June leaves them out of both switches, so they take the default level slice even though their data carries `BUFF_RELATION_TYPE`. Only 76 joins both ChangeStat groups.
- Each needs `InsertAdditionalStat`/`EraseAdditionalStat` cases on its new member, `InitAdditionalStat` (basic value), `InitAdditionalPassiveMultipleAndSum` (multiple 1, sum 0), and `Verify()` plus the update-all loop like its siblings.
- **Hand-off to Phase 6:** 91 `CHARGE_MP_HIT_BY_SKILL_ID` scales the same `fHitAddMP` value, in the same function, unconditionally on attack type. Write in the Done note exactly where 85/86's block landed so Phase 6 can add 91 beside it.

**Run with:** Sonnet 5.5, high effort. Review required (Opus 5.5).

```
Continuing the June buff/debuff port. Run this session on Sonnet 5.5 at high effort.
Read JUNE_BUFF_PORT_PHASES.md in the repo root in full -- "Starting context", the
"Investigation ledger", the "June buff function map", the "Per-type map" (its Phase 5
table), the "Phases 5-7" shared checklist and "Phase 5". Branch: mods/june-buff-port.
Never launch X2_offline.exe without asking first.

Your task (Phase 5, group A): implement these seven June behavior types on March
classes: 76 BBT_CHANGE_HYPER_DURATION, 85 BBT_CHANGE_RANGE_COMMAND_MP_GAIN_RATE,
86 BBT_CHANGE_MELEE_COMMAND_MP_GAIN_RATE, 87 BBT_CHANGE_MELEE_COMMAND_DAMAGE_RATE,
96 BBT_CHANGE_RANGE_COMMAND_DAMAGE_RATE, 108 BBT_CHANGE_FINAL_DAMAGE_MULTIPLIER_NPC,
109 BBT_CHANGE_FINAL_DAMAGE_MULTIPLIER_USER. 85, 86, 87 and 96 need Phase 2's AT_COMMAND.
For each type open June's parse case, templet class and read sites in Ghidra ONLY at
the addresses the Per-type map already gives, transcribe them, then follow the 8-step
checklist. Do not search the binary for a function the map lacks. Cite the June address
in a comment; never invent a value or formula. Use flag
SERV_IRUHADEV_JUNE_BUFF_NEW_STAT. If the map does not settle a type, do NOT
reverse-engineer or guess it: skip it, mark it "Opus-only" in the doc, and keep going.
Record where 85/86's block landed in SetDamageExceptionProcess (Phase 6 needs it for 91).
Done means: buff_census.py --phase 5 lists none of these seven types, the build is
rc=0, and the strings are in the deployed exe. Write a per-type "how to see it in game"
line (a June BTI/BFI and where it comes from) into the Phase 5 Done note for Phase 14.
Record this session's /cost there. Commit.
```

**Review prompt (fresh Opus 5.5, high effort):**

```
Reviewing the June buff/debuff port, Phase 5 (fresh Opus 5.5 session, high effort).
Read JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", the "Per-type map"
Phase 5 table, the shared checklist and the Phase 5 Done note. Branch:
mods/june-buff-port. Never launch X2_offline.exe without asking first.

Review the Phase 5 commit(s) against the function map, for types 76, 85, 86, 87, 96, 108
and 109: enum values, float/key order, store member and basic value, insert/erase/update
cases, read sites, level-slice and SGU cases (76 only in both), and SERV_IRUHADEV_ / #else /
CP949 hygiene (run `file` on every touched file and check `git diff --stat`). Spot-check
2-3 types directly in Ghidra at the June addresses in the map (read-only); if a
spot-check fails, re-check the whole phase. Fix clear transcription errors under the
same flag; hand design decisions back to me. Write the findings into the doc as
"Phase 5 -- Review", record this session's /cost, and commit.
```

---

### Phase 5b — group A: transcription-ready stat types (Sonnet)

**Scope.** Nine types, all settled by "Phase 1c, item 3" (read that section for the addresses). Run after Phase 5.
- **79 `BBT_CHANGE_NOW_FORCE_DOWN_PER_SECOND`.** Transcription-ready. Class (create `32e86e90`): `m_eChangeType` is fixed at `BCT_FIX_VALUE` by the ctor, a `CKTDXCheckElapsedTime`, `m_fResultValue`, and two delegates. `ParsingBehavior` `32e755a0` requires `TIME_INTERVAL`. `SetFactor` `32e855f0` is March's `CX2BuffChangeNowHpPerSecondBehaviorTemplet::SetFactor` (`X2BuffBehaviorTemplet.cpp:249`) with regist fixed at 0 and no popups. `TIME_INTERVAL = 0` ticks every frame; the value is applied per tick, not per second. Transcribe as-is.
- **80–84 `…_BY_CONDITION`.** Class `CX2BuffChangeStateByConditionBehaviorTemplet` (vt `335350ac`); both `BUFF_CHANGE_TYPE` and `BUFF_VALUE_CALCULATE_TYPE` are required keys. Value `32e77f40`: `vct == 0 ? v[3] + v[4] * ( 4.0 - v[5] ) : 0`. The SGU case `32d4ce60` appends the runtime user count (`BVC_RALLY_OF_HERO`). The `GT_PVE` halving cannot run in March (its `GAME_TYPE` stops at `GT_BATTLE_FIELD`): port the branch only if March has `GT_PVE`, otherwise comment it as unreachable. Stores: **82, 83, 84 are March's cases 9, 10 and 6** (84 also calls `SetMaxHp`, March's MAX_HP case `X2GameUnit.cpp:11745`). **80 and 81 follow whatever Phase 4b-i part 2 ported for BBT 7/8**, including the second member `+0x44ec`/`+0x45a0`. Check the 4b-i part 2 notes: if the second member is in, wire it; if not, wire the first member only and say so in the Done note.
- **93 `BBT_CHANGE_EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER`.** Class `_HaveBuffOwner` (vt `33534ce4`): ChangeStat plus a `UidType` owner (`SetFactor` is a `jmp` to ChangeStat's; slots 14/15 return and set the UID). Store and readers are 42's. `GetFactor` `32e7d7f0` and the packet `32e76710` must be transcribed **verbatim, including June's index bug** (the UID words are written at indices 1–4 and read from 4–7). The owner aggregator `3227c620` is unreachable; skip it and say so in a comment.
- **106 / 107 `BBT_CHANGE_FORCE_DOWN_MULTIPLIER_MELEE_ATTACKED` / `…_RANGE_ATTACKED`.** Two new members, basic 0.0. Reader: `CX2GUUser::DamageReact` `@3204a118`–`3204a243`, against March's `SERV_SKILL_NOTE` block (`X2GUUser.cpp:9801-9812`). **User only**: NPCs ignore 106/107. It uses `m_fDecreaseForceDown` (DamageData `DECREASE_FORCE_DOWN`) and `m_bMeleeAttack`; check March parses both before relying on them.
- **Phase 4b hand-off, not here:** June's BBT 6 insert and erase rescale now-HP to keep the HP ratio; 84's blocks do not, and neither does March's 6.

**Run with:** Sonnet 5.5, high effort (the readiness bar in item 3 is met for all nine). Review required (Opus 5.5).

```
Continuing the June buff/debuff port. Run this session on Sonnet 5.5 at high effort.
Read JUNE_BUFF_PORT_PHASES.md in the repo root in full -- "Starting context", the
"Investigation ledger", the "June buff function map", "Phase 1c, item 3", the
"Phases 5-7" shared checklist, "Phase 5" (its Done note, for where the shared host
blocks landed) and "Phase 5b". Branch: mods/june-buff-port. Never launch X2_offline.exe
without asking first.

Your task (Phase 5b, group A): implement these nine June behavior types on March
classes: 79 BBT_CHANGE_NOW_FORCE_DOWN_PER_SECOND, 80 BBT_CHANGE_PHYSIC_ATTACK_BY_CONDITION,
81 BBT_CHANGE_MAGIC_ATTACK_BY_CONDITION, 82 BBT_CHANGE_PHYSIC_DEFENCE_BY_CONDITION,
83 BBT_CHANGE_MAGIC_DEFENCE_BY_CONDITION, 84 BBT_CHANGE_MAX_HP_BY_CONDITION,
93 BBT_CHANGE_EVERY_ATTRIBUTE_DEFENCE_HAVE_BUFF_OWNER,
106 BBT_CHANGE_FORCE_DOWN_MULTIPLIER_MELEE_ATTACKED,
107 BBT_CHANGE_FORCE_DOWN_MULTIPLIER_RANGE_ATTACKED. Open June's parse case, templet class
and read sites in Ghidra ONLY at the addresses "Phase 1c, item 3" gives, transcribe
them, then follow the 8-step checklist. Do not search the binary for a function the doc
lacks. Cite the June address in a comment; never invent a value or formula. Use flag
SERV_IRUHADEV_JUNE_BUFF_NEW_STAT. 80 and 81 need the second physic/magic member only if
Phase 4b-i part 2 ported it for BBT 7/8: check the doc, wire what exists, and say which
you did. Transcribe 93's UDP index bug verbatim. If the doc does not settle a type, do
NOT reverse-engineer or guess it: skip it, mark it "Opus-only", and keep going. Done
means: buff_census.py --phase 5b lists none of these nine types (it will still list 78,
88 and 92, which are Phase 5c), the build is rc=0, and the strings are in the deployed
exe. Write a per-type "how to see it in game" line into the Phase 5b Done note for
Phase 14. Record this session's /cost there. Commit.
```

**Review prompt (fresh Opus 5.5, high effort):**

```
Reviewing the June buff/debuff port, Phase 5b (fresh Opus 5.5 session, high effort).
Read JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", "Phase 1c, item 3",
the shared checklist and the Phase 5b Done note. Branch: mods/june-buff-port. Never
launch X2_offline.exe without asking first.

Review the Phase 5b commit(s) for types 79, 80, 81, 82, 83, 84, 93, 106 and 107
against "Phase 1c, item 3": enum values, float/key order (GetFactor/packet), store
member and which March case each type shares, the SGU case for 80-84 and 93, the 93
verbatim index bug, 106/107 user-only reader, and SERV_IRUHADEV_ / #else / CP949 hygiene
(run `file` on every touched file and check `git diff --stat`). Spot-check 2-3 types
directly in Ghidra at the June addresses in the doc (read-only); if a spot-check
fails, re-check the whole phase. Fix clear transcription errors under the same flag;
hand design decisions back to me. Write the findings into the doc as "Phase 5b --
Review", record this session's /cost, and commit.
```

---

### Phase 5c — group A: stat types with rewritten hosts (Opus)

**Scope.** Three types whose store and reader are mapped but whose host function is a June rewrite (item 3).
- **78 `BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER`.** Store: new member, basic 1.0, insert `32282dff`. Readers: `CX2GUUser::DamageReact` `@32049466`–`32049561` (March `X2GUUser.cpp:9276-9291`) and `CX2GUNPC::DamageReact` `@327b42b0`–`327b43be`. The formula is `k = attacker->Opt5248() + this->Get78() + this->Opt5194()`, minus the defender's `Opt5ac8() * 0.01`, `v = max( k, 0 ) * attacker->GetAdditionalAttackDamage( pDD )`. The three option values come from socket-data fields March lacks, so in a March build they are 0 and `k = max( Get78(), 0 )`. **That deviation is exact on March data and may be ported; record it in the doc.** One block up, June also gates March's `GetAdditionalDefence` reduction on `!DD[+0x534]` (`IGNORE_DEFENCE_DECLINE`), which needs the same DamageData key as 88.
- **88 `BBT_IGNORE_DEFENCE`.** Class, store and reader are ready (class vt `33535558`; list `+0x67dc`; reader `32274970` = `min( Σ rate, 1.0 )`, defender side). The host is June's rewrite of March's `fPiercingDefenceFactor` (`X2GameUnit.cpp:2330-2334`), which also needs four DamageData keys March does not parse: `IGNORE_DEFENCE_ON_PVP`, `IGNORE_DEFENCE_RATE`, `IGNORE_DEFENCE_RATE_ON_PVP`, `IGNORE_DEFENCE_DECLINE` (stores `+0x529`/`+0x52c`/`+0x530`/`+0x534` in `SetDamageDataFromLUA` `32632390`; defaults in item 3), and `CX2Game::IsPvpOrPve`. **Port June's whole block, parsing the four keys** (no partial updates). Skip the frozen-debuff resist scaling just above it: it is not a buff change. If the keys or `IsPvpOrPve` need a decision, record it and stop 88.
- **92 `BBT_CHANGE_MAXIMIZE`.** Opus-only; the roll is decoded in item 3. Porting it means porting June's stat-level attack range (two new fields in the stat block that fill the second physic/magic members `+0x44ec`/`+0x45a0`), the second members themselves (**Phase 4b-i part 2 lists them with BBT 7/8; if they are not in, defer 92 and say so**), the variance roll `322788c0`, and its two `GetFinalDamage` call sites (`@32284c1f` physic, `@32284c2f` magic). First verify that March has no variance roll in `GetFinalDamage`. Where June's stat calc fills the two new stat-block fields was not traced; trace it, or stop 92 and hand it to me.

**Run with:** Opus 5.5, high effort. Review required.

```
Continuing the June buff/debuff port. Run this session on Opus 5.5 at high effort.
Read JUNE_BUFF_PORT_PHASES.md in the repo root in full -- "Starting context", the
"Investigation ledger", the "June buff function map", "Phase 1c, item 3", the
"Phases 5-7" shared checklist, the Phase 4b-i part 2 notes (second physic/magic
members) and "Phase 5c". Branch: mods/june-buff-port. Never launch X2_offline.exe
without asking first.

Your task (Phase 5c, group A, hosts): implement 78 BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER,
88 BBT_IGNORE_DEFENCE and 92 BBT_CHANGE_MAXIMIZE on March classes, including the host
functions June rewrote. Use the addresses "Phase 1c, item 3" gives; open further
Ghidra functions only where the doc says something was not traced (92's stat-block
fill). Follow the 8-step checklist. 78: the three socket-derived terms are 0 on March
data, so porting k = max( Get78(), 0 ) is an allowed deviation -- record it. 88: port
June's whole fPiercingDefenceFactor block and parse its four DamageData keys; skip the
frozen-debuff resist scaling. 92: first check that Phase 4b-i part 2 landed the
second physic/magic members, and that March has no variance roll; if either blocks it,
defer 92 and write why. Cite June addresses in comments; never invent a value or
formula. Use flag SERV_IRUHADEV_JUNE_BUFF_NEW_STAT. If a type needs a design decision,
stop that type, write the question in the Done note, and continue with the others. Done
means: buff_census.py --phase 5b lists none of 78, 88, 92 (or says which are deferred
and why), the build is rc=0, and the strings are in the deployed exe. Write a per-type
"how to see it in game" line into the Done note for Phase 14. Record this session's
/cost. Commit.
```

**Review prompt (fresh Opus 5.5, high effort):**

```
Reviewing the June buff/debuff port, Phase 5c (fresh Opus 5.5 session, high effort).
Read JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", "Phase 1c, item 3",
the shared checklist and the Phase 5c Done note. Branch: mods/june-buff-port. Never
launch X2_offline.exe without asking first.

Review the Phase 5c commit(s) for 78, 88 and 92 (whichever landed) against item 3: the
host rewrites (78's multiplier block in both DamageReacts, 88's fPiercingDefenceFactor
block and its four DamageData keys with their defaults, 92's roll and both
GetFinalDamage call sites), the recorded deviations, and SERV_IRUHADEV_ / #else / CP949
hygiene (run `file` on every touched file and check `git diff --stat`). Because every
one of these is a damage-path change, diff the flag-off build path against March by
reading the #else branches. Spot-check 2 of the 3 types directly in Ghidra
(read-only); if a spot-check fails, re-check the whole phase. Fix clear transcription
errors under the same flag; hand design decisions back to me. Write the findings into
the doc as "Phase 5c -- Review", record this session's /cost, and commit.
```

---

### Phase 6 — group B: skill-scoped types (Sonnet)

**Scope.** Everything in group B that "Phase 1c, item 1" marked transcription-ready, without the host functions June rewrote. Run after Phase 5.
- **Complete types (class, store, reader, host): 75, 91, 103.**
  - **75 `BBT_DOUBLE_ATTACK_BUFF`.** Class `CX2BuffDoubleAttackBuffBehaviorTemplet` (vt `33535070`), same as March's type-56 class with a **new** unit list `vector<CX2DoubleAttackBuffPtr>` beside 56's (`m_vecDoubleAttack`); insert `32281b00`, erase `32279d20`, reader `32275050` (= March `IsDoubleAttackByBuff`, `X2GameUnit.cpp:12428`, on the new list). `SetFactor` `32e77be0` returns false with no popup unless the change type is `BCT_PERCENT`. Host: both `DamageReact`s, `@32048da7`–`32048dbc` (user) and `@327b3ac7`–`327b3ae5` (NPC, plus `!m_bNoDamage`): `if ( DD.attackType != AT_SPECIAL && attacker && attacker->IsDoubleAttackBuffByBuff() ) bDoubleAttack = true`. **That block also carries June's changes to type 56** (`!= AT_SPECIAL` on the defender's own list where March has `== AT_NORMAL` at `X2GUUser.cpp:9137`, the NPC `!m_bNoDamage`, and the `BTI_BUFF_SHADE_THE_ILLUSION 430` skill rule). Check whether Phase 4b-i part 2 already landed them; if not, land them under the Phase 4b flag that owns type 56 (see "Flags"), or leave 56 to 4b and say so in the Done note. Do not leave the block half-ported.
  - **91 `BBT_CHARGE_MP_HIT_BY_SKILL_ID`.** Class `CX2BuffChargeMpHitBySkillIDBehaviorTemplet` (vt `335355d0`), unit list (`3200af50` insert, `31fe2a30` erase), element getter `32369b10` (**last matching block wins**), reader `31fd7df0`, reached only through `CX2GUUser` slot 109 `GetChargeMpRateByBuff( const DamageData* )`; declare it virtual on `CX2GameUnit` with a base returning 1.0. Host: `SetDamageExceptionProcess` `@32299510`–`3229954e`, `fHitAddMP *= attacker->GetChargeMpRateByBuff( pDamageData )`, **unconditional on attack type**, on the same value Phase 5's 85/86 block scales. Read Phase 5's Done note for where that block is.
  - **103 `BBT_CHANGE_SKILL_COOL_TIME_SPEED`.** Class (vt `3353542c`; **no packet override**, so peers never receive it). `SetFactor` `32e7bb50` and `DoFinish` `32e75ea0` set a unit dirty flag. Store: a unit `map< int SKILL_TYPE, float >` rebuilt by `32039fb0` from every live type-103 behavior factor (`BCT_PERCENT` only). Reader `31fd8bc0`, reached by `32896850( skillId )` (local player only). Host: `CX2UserSkillTree::OnFrameMove` (June `32896930`, March `X2UserSkillTree.cpp:48`, subtractions at `:70` and `:85`): `left -= 32896850( slotSkillId ) * fElapsedTime`. June rebuilds in `CommonFrameMove` (slot 499); rebuilding lazily inside the reader when the flag is set gives the same result.
- **Classes, stores and readers only (inert until Phase 6b wires their hosts): 89, 90, 98, 99, 100.** Do not touch `DoStateEndStart`, the summon helper, `GetActualMPConsume` or the cooldown helper `3235a710`.
  - 89 `BBT_CHANGE_SKILL_DAMAGE`: class `CX2BuffChangeSkillDamageBehaviorTemplet` (vt `33535378`), `SetFactor` `32e7a9e0`, list `+0x92d4` with `InsertSkillDamage`/`UpdateSkillDamage`, reader `GetSkillDamageRateByBuff` `31fd7cc0`. **Phase 4a hand-off:** its slot 7 `32e7c020` (`OnAccumulationCountDecreased`) must be ported with the class, otherwise `BTI_BUFF_HEAVYFIREARMS_MASTERY` (`ACCUMULATION_DECREASE_WHEN_FINISH`) ends at its first timeout instead of decaying.
  - 90 `BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_ID`: class vt `33535594`, `SetFactor` `32e849c0`, list (June `+0x92e4`), getter `32369aa0` (last matching block wins); read through 89's `31fd7cc0`.
  - 98 `BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE`: class `CX2AddDamageOfSkillSlotType` (vt `33535160`), `SetFactor` `32e78080` (negative slot type → false), **no store**, reader `32039e00`, `SlotIndexToSlotType` `3214f3d0`, `FindEquippedSlotIndex` `3288fff0`.
  - 99 `BBT_CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE` / 100 `BBT_CHANGE_COOLTIME_RATE_BY_SKILLTYPE`: byte-identical classes (vt `335353b4` / `335353f0`), **no store** (readers walk `GetBuffFactor`, June `3229fd80`, March `X2GameUnit.cpp:14168`), readers `3203a250` / `3203a420`.
- Port every June DamageData key these readers use only where March already parses it (`NO_DAMAGE`, `DECREASE_FORCE_DOWN`, `USED_SKILL_ID`, `APPLIED_SKILL_ID`); for any it does not, say so in the Done note.

**Run with:** Sonnet 5.5, high effort. Review required (Opus 5.5).

```
Continuing the June buff/debuff port. Run this session on Sonnet 5.5 at high effort.
Read JUNE_BUFF_PORT_PHASES.md in the repo root in full -- "Starting context", the
"Investigation ledger", the "June buff function map", "Phase 1c, item 1", the
"Phases 5-7" shared checklist, "Phase 5" (its Done note: where 85/86's block landed in
SetDamageExceptionProcess) and "Phase 6". Branch: mods/june-buff-port. Never launch
X2_offline.exe without asking first.

Your task (Phase 6, group B): implement June behavior types 75 BBT_DOUBLE_ATTACK_BUFF,
91 BBT_CHARGE_MP_HIT_BY_SKILL_ID and 103 BBT_CHANGE_SKILL_COOL_TIME_SPEED completely
(class, store, reader and host), and add the classes, stores and readers ONLY -- not
the host functions -- for 89 BBT_CHANGE_SKILL_DAMAGE, 90 BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_ID,
98 BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE, 99 BBT_CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE
and 100 BBT_CHANGE_COOLTIME_RATE_BY_SKILLTYPE (Phase 6b wires their hosts; do not edit
DoStateEndStart, the summon-NPC helper, GetActualMPConsume or the cooldown setters).
Port 89's slot-7 OnAccumulationCountDecreased with its class. Open Ghidra ONLY at the
addresses "Phase 1c, item 1" gives; do not search the binary for a function the doc
lacks. Follow the 8-step checklist. 75's host block also carries June's type-56
changes: check whether Phase 4b-i part 2 landed them and either land them with 75 or say
you left them. Cite the June address in a comment; never invent a value or formula. Use
flag SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL. If the doc does not settle a type, do NOT
reverse-engineer or guess it: skip it, mark it "Opus-only", and keep going. Done means:
buff_census.py --phase 6 lists none of the eight types, the build is rc=0, and the
strings are in the deployed exe. Write a per-type "how to see it in game" line into
the Phase 6 Done note for Phase 14 (89/90/98/99/100 say "inert until 6b"). Record this
session's /cost there. Commit.
```

**Review prompt (fresh Opus 5.5, high effort):**

```
Reviewing the June buff/debuff port, Phase 6 (fresh Opus 5.5 session, high effort).
Read JUNE_BUFF_PORT_PHASES.md in the repo root -- "Starting context", "Phase 1c, item 1",
the shared checklist and the Phase 6 Done note. Branch: mods/june-buff-port. Never
launch X2_offline.exe without asking first.

Review the Phase 6 commit(s) for types 75, 91, 103 (complete) and 89, 90, 98, 99, 100
(class, store, reader only) against "Phase 1c, item 1": enum values, GetFactor/packet
float order, restack arithmetic in SetFactor, store members and getters (90 and 91:
last matching block wins), 89's slot 7, 103's missing packet override and its
rebuild/dirty flag, 75's DamageReact blocks (user and NPC, including what was done about
type 56), 91's unconditional host, and SERV_IRUHADEV_ / #else / CP949 hygiene (run
`file` on every touched file and check `git diff --stat`). Confirm no host function of
89/90/98/99/100 was edited. Spot-check 2-3 types directly in Ghidra (read-only); if a
spot-check fails, re-check the whole phase. Fix clear transcription errors under the same
flag; hand design decisions back to me. Write the findings into the doc as "Phase 6 --
Review", record this session's /cost, and commit.
```

---

### Phase 6b — group B: skill-scoped hosts (Opus)

**Scope.** Wire the inert Phase 6 types into the functions June rewrote. Under the no-partial rule the default is to **port June's whole function**, not to add only the buff term. If a whole-function port would pull in a system March lacks and cannot be finished in this session, stop that host, write what it needs into the Done note, and hand the decision to me.
- **89, 90 and 98 → `CX2GUUser::DoStateEndStart`** (June `3203c3a0`, March `X2GUUser.cpp:6350`; the buff terms are at `@3203c7de`–`3203c801` for 89/90 and `@3203c649` for 98). June also adds a `+0x80d9` byte gate, a random sound/voice pick for state types 9/11/12, a protected power-rate cache read by `GetPowerRate` (June slot 129 `31fcb230`; March `X2GUUser.cpp:23045`) and `31fea430`, and the `+0x2557..0x255a` resets. 98's rate is cached at state start, so BFT 16 (Phase 8) ends the buff after the skill it applied to; Phase 8 depends on this order.
- **89, 90 and 98 → the summoned-NPC power helper** `322ece70`, called 21 times from `CX2Game::SetUserSummonedNPCInfo` (June `3230c1a0`, March `X2Game.cpp:14060`). June multiplies `( 3201ad10( id ) + 1 ) * GetSkillDamageRateByBuff( id ) * rate98summon * GetAdditionalAttackByType( AT_SPECIAL )`; March multiplies only `GetAdditionalAttackByType( AT_SPECIAL )` inline at each case. `IsSummonSkill` `31fc8200` has 54 ids (listed in item 1). The `+0x178` memo/base id field March lacks means the port uses the plain skill id; say so.
- **99 → `CX2GUUser::GetActualMPConsume`** (June `3203cf60`, slot 423; March `X2GUUser.cpp:21081`). June's function is much larger (`vfunc +0x75c`, `32039a50( id, 0x3f )`, `32358970( 0xb )`, the `+0x74dc` term); port it whole, then call 99's reader `3203a250` at `@3203d024`. Some of those terms may need systems March lacks: stop and report if so.
- **100 → the cooldown helper `3235a710`** and its 16 callers (the generic user path `31fd8ff0` plus one per-class override each). March sets cooldowns inline in each class (`X2GUAdd.cpp:2683`, `X2GUAra.cpp:3924`, `X2GUChung.cpp:5983`, …). Map the 16 sites to March's inline setters, then route them through one helper that applies 100 through `3203a420`.
- **Phase 9 only, do not port here:** `32369bd0`, `3236df00`, `32773ee0` (custom-function bodies that also read 89's list).

**Run with:** Opus 5.5, high effort. Review required.

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
Phase 14, and note the 98 -> BFT 16 ordering for Phase 8. Record this session's /cost.
Commit.
```

**Review prompt (fresh Opus 5.5, high effort):**

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
doc as "Phase 6b -- Review", record this session's /cost, and commit.
```

---

### Phase 7 — group C: flag and immunity types with ready readers (Opus)

**Scope.** Five types whose class and reader are settled in "Phase 1c, item 2".
- **73 `BBT_IGNORE_STATE_GUARD`.** Own class: `SetFactor` `32e752b0` (insert, list June `+0x67cc`), `DoFinish` `32e75300`, packet `32e728b0`; `GetFactor` `32e7e960` writes the header only. Reader in `GetFinalDamage` `@322862bf`, inserted before March's state-defence block (`X2GameUnit.cpp:2716-2731`): `if ( !defender->list73.empty() ) pDD->m_bIgnoreStateDefence = true; if ( attacker->HaveThisBuff( BTI_BUFF_SCHWERT_PLATZEN /*975*/ ) ) pDD->m_bIgnoreStateDefence = true;`. **BTI 975 is in March's Lua Enum but not in the C++ `BUFF_TEMPLET_ID` enum: add the enumerator.** June writes the flag into the attacker's DamageData, so on multi-target damage data it stays set for later targets; transcribe verbatim and note it for Phase 14.
- **94 `BBT_IMMUNE_BUFF_DEBUFF`.** Factor parse `32d4a770`; `SetFactor` `32e79e30`; unit helpers `3227a910`, `322780f0`, `32278120`, `3227b6b0`. **Fill in Phase 4a's stub `CX2GameUnit::IsImmuneBuffByBuffBehavior`** (June `322a80a0`). Transcribe both June quirks verbatim: the loop starts at index 2 (the count is stored as an immune BTI, so an empty block also grants SILENCE immunity), and `GetFactor` sends the two `ALL_*` bytes as 0 so peers always receive 0.
- **95 `BBT_IMMUNE_NON_CONTROL_DEBUFF`.** `SetFactor` `32e75c00` inserts into the list (`+0x75ac`) **and calls `DeleteNonControlDebuff`** (`3229dcc0`); the packet handler `32e728d0` inserts only. **Fill in the stub `IsImmuneNonControlDebuffByBuffBehavior`** (June `3227b790`): `!list95.empty() && templet->IsNonControlDebuff()`. Port `DeleteNonControlDebuff` as a `CX2GameUnit` member here; the Lua binding of the same name is Phase 10's.
- **104 `BBT_JUMP_IMPOSSIBLE`.** Identity type, list `+0x42c0`. User: `CX2GUUser::SetInputData` (June `31fe83a0`, March `:28554`), the Up-key clear (`:28668`) becomes `m_fCanNotJumpTime > 0 || !list104.empty()`. NPC: `CX2GUNPC::CommonEventProcess` (June `3276d980`, March `:21405`), gate `SCT_AI_JUMP` on the list (`@3276e442`, March `:21819`). The same NPC switch also gates `SCT_AI_DASH`, `SCT_AI_JUMP_DIR` and `SCT_AI_DOWN_DIR` on the existing DASH_IMPOSSIBLE list (a Phase 4b item): land it if Phase 4b-i part 2 did not, or say so. Do not port the dead `+0xded` byte.
- **105 `BBT_IMMUNE_EDT`.** Class `SetFactor` `32e79770`, no unit store (the templet's `m_vecEDT` is the store); reader `IsImmuneEDTByBuff` `322a0400` walks `GetBuffFactor()` for type 105. Host: `CX2GameUnit::ProcessEnchantExtraDamage` (June `322a0510`, slot 213; March `X2GameUnit.cpp:3529`), after `if ( rate < fRandomValue ) return false;` add `if ( IsImmuneEDTByBuff( basicExtraDamageType ) ) return false;`. **The host's rate math is Phase 4b's** (`CHANGE_ENCHANT_ATTACK_RATE`, `32274df0`/`32272d90`); do not touch it and say so.

**Run with:** Opus 5.5, high effort. Review required.

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
Phase 7 Done note for Phase 14. Record this session's /cost there. Commit.
```

**Review prompt (fresh Opus 5.5, high effort):**

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
back to me. Write the findings into the doc as "Phase 7 -- Review", record this session's
/cost, and commit.
```

---

### Phase 7b — group C: rewritten hosts and per-class virtuals (Opus)

**Scope.** The Phase 7 types that need June's rewritten hosts or a mechanism March lacks ("Phase 1c, item 2", *Opus-only*).
- **74 `BBT_DETONATION_IMPOSSIBLE` / 102 `BBT_DETONATION_IMPOSSIBLE_GAGE_RESET`.** Port June's split of March's `m_bNoDetonation` into two `CX2GameUnit` bytes (`+0x6890` the state flag, written by the per-class state code at about 50 sites where March calls `SetNoDetonation( true/false )`; `+0x6892` the damage flag, written from `pDD->m_bNoDetonation`). Hosts: `CX2GUUser::DetonationEventProcess` (June `31feb9f0`, March `X2GUUser.cpp:19238`; June **zeroes the charge** where March only returns, so every existing March skill that calls `SetNoDetonation( true )` also wipes the stored charge), `DamageReactStateChange` (June `31feabd0`, March `:11262`, around March's `:11894`, plus the `pDD->damageType != DT_BUFF` condition moved from `:11964`), and `CX2GUNPC::CommonEventProcessNonHost` (June `3276cb20`, March `X2GUNPC.cpp:20775`) with its June-only `CAN_DETONATION` condition (parsed at `32784c00` `@32794f62`; evaluated for non-host NPCs only; no June script uses it).
- **77 `BBT_INVINCIBLE`.** Getter: `CX2GUUser` slot 96 `GetInvincible` (`31fcee80`) and the NPC slot 96 (`32758760`): `!list77.empty() || stateParam.bInvincible` (March `X2GUUser.h:1582`, `X2GUNPC.h:1950`). Frame-move half: the tail of `CX2GUUser::CommonFrameMove` (June `32063650`, slot 499, `@3206ce71`): `if ( !list77.empty() && m_fForceInvincibleTime <= 0 ) { SetForceInvincible( 0.5f ); SetShowInvincible( 0.5f ); }`; June's frame move is a 2,700-line superset of March's and was not diffed in full. **Check the 12 places where March reads `stateParam.bInvincible` directly** (`X2GUChung.cpp:613/646`, `X2GUNPC.cpp:8922/10268/11920/12719/18853/29201/29214`, `X2GUUser.cpp:7809/9919/17516`): in June, does each call slot 96 or read the field?
- **97 `BBT_CHANGE_MAX_CHARACTER_ADD_RESOURCE`.** Empty factor parse; class overrides only `SetFactor` (`32e75c70`), which calls unit virtual `+0x324` (`CX2GameUnit` slot 201) with `1.0f`. The base slot is empty; June's overrides are `CX2GUAdd` `329e31e0`, `CX2GURose` `32ac5e20`, `CX2GULuciel` `32b14cd0`, `CX2GUAra` `32baafc0` and `CX2GUChung` `32bf4880`. Port the base virtual and each override for classes March has; Rose and Lu/Ciel stay dormant until those characters arrive (user decision).
- **101 `BBT_IGNORE_HP_DECREASE`.** User only (NPCs ignore it). June made `UpNowHp`/`SetNowHp` virtual on `CX2GameUnit` (March `X2GameUnit.h:2311/2315`: non-virtual inlines); the user overrides are `3202fcf0` (slot 59) and `3202ff60` (slot 61), gated on `IgnoreHpDecrease()` (`322726d0`). They also carry two more refuse-decrease gates (`320072d0`, `3202b9a0`) and a low-HP UI call `3289db00( 0xd, 0|1, -1 )`, **none decoded**: decode them first, or stop and report.
- **110 `BBT_AWAKE_IMPOSSIBLE`.** Getter `31fd4790` (slot 345). Consumer: `CX2GUUser::TryHyperModeStateChange` (slot 343 `31fe9a90`; Ain's copy `32a53150`). June replaced March's `ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE` macro (`X2GUUser.h:11`, 56 uses; Ara inlines it, `X2GUAra.cpp:6348` and five more) with this virtual, adding class hook slots 434/435 and fields `+0x8d78`/`+0x8d4c` that March lacks. Port June's whole function, or stop and report what it needs.
- **111 `BBT_MP_NATURAL_RECOVERY_IMPOSSIBLE` and 112 `BBT_REFLECT_PHYSIC`:** not implemented in June's client; enum only (Phase 2 did that). Confirm the census excludes them and do nothing else.

**Run with:** Opus 5.5, high effort. Review required. If the session grows long, finish 74/102 and 77 first, commit, and record where the rest resumes.

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
Phase 14. Record this session's /cost there. Commit.
```

**Review prompt (fresh Opus 5.5, high effort):**

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
findings into the doc as "Phase 7b -- Review", record this session's /cost, and commit.
```

**Phase 4a hand-offs this group picks up** (stubs and slots Phase 4a added):
- **BBT 94:** `CX2GameUnit::IsImmuneBuffByBuffBehavior` (June `322a80a0`) returns false until Phase 7 lands, which equals June's result with no BBT 94 buff on.
- **BBT 95:** `CX2GameUnit::IsImmuneNonControlDebuffByBuffBehavior` (June `3227b790`), same stub rule. `IsNonControlDebuff` already has June's BBT 104 case.
- **BBT 89** (Phase 6): `CX2BuffChangeSkillDamageBehaviorTemplet` needs `OnAccumulationCountDecreased` (June behavior slot 7, `32e7c020`).
- Any type Phase 1 marked "Opus-only" moves to an Opus session, whichever group it is in. Under rule 9, a Phase 7 type may move to Sonnet only if Phase 1 marked it transcription-ready; Phases 5, 5b and 6 move to Opus if their reviews find errors.

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

## Phase 15 — Enchant system parity (attribute enchant rework)

**Added 2026-10-04 at the user's request** (decision on part 4's open question: yes, this gets its own phase). It is not a buff behaviour: it changes how weapon attribute enchants (blaze, frozen, poison, piercing, shock, snatch) roll and proc. Phase 4b-i part 4 found it while diffing the CHANGE_ENCHANT_ATTACK_RATE host.

**What June changed (measured in part 4):**
- **Accumulation modifier** `32272d90` (March `CX2GameUnit::EnchantAccumulationRateModifier`, `X2GameUnit.cpp:4983`). March doubles the proc rate when the defender's accumulation of that element is 1 or 2. June adds `1×` / `2×` the attacker's `EnchantData+0x3c` (`m_fAttribReiterationRate`) instead, for accumulation 1 / 2, and only when the `g_pData` member at `+0x284` is non-null (not yet identified) and the attacker exists. It picks `GetEnchantData` (user, `32708ce0`) or `GetNPCEnchantData` (`32708d10`) by unit type.
- **A fourth tier per element.** The element test accepts four EDTs each: e.g. blaze 1001 / 2002 / 3003 / 4004. June's `Enum.src.lua` has 78 `EDT_ENCHANT_*` names against March's 45, including `EDT_ENCHANT_QUADRA_*_MASTER` and `EDT_ENCHANT_DOUBLE_MASTER_SHOCK_OLD`. **The EDT values were renumbered** (March `EDT_ENCHANT_BLAZE = 100`; June's element base is 1001), so this phase starts with an enum reconciliation like Phase 2's.
- **New `EnchantData` fields**, read by June's loader `32708f00` after `m_MPDrain`: `m_fAttribReiterationRate` (+0x3c), `m_fBlazeMpDrain`, `m_fPiercingDamageFactor`, `m_iShockDebuffTime`, `m_fShockDebuffDamageFactor`. Their readers (the `ProcessEnchant*` bodies, slots 214–219, now virtual) are not yet decoded.
- **Data.** June's `june_src_fixed/data036_kom/AttribEnchantItem.src.lua` (37 entries) carries the new keys and the quadra tiers. March loads `AttribEnchantItem.lua` and `AttribEnchantRequire.lua` (`X2EnchantItem.cpp:7-8`); `AttribEnchantTable.lua` also exists in the staged `data036_kom`.

**Unchanged (do not redo):** the host rate formula in `ProcessEnchantExtraDamage` (`322a0510`) and the buff-side split `32274df0` equal March's (part 4).

**Scope.**
1. RE: the six `ProcessEnchant*` bodies (June slots 214–219) against March's, the readers of the five new fields, the `g_pData+0x284` flag, and June's `AttribEnchantRequire`/`AttribEnchantTable` loaders. Record in `funcmap.tsv`.
2. EDT enum reconciliation (Lua and C++), with a consumer audit of every hard-coded EDT value in `X2Lib` (socket options, item scripts and damage data name EDTs too).
3. `EnchantData` fields and the loader; the accumulation modifier; the per-element process bodies.
4. Adopt June's `AttribEnchantItem.lua` (and the two siblings if they differ). **Pairing:** the Lua and the exe ship together; the user packs.

**Flag:** `SERV_IRUHADEV_JUNE_ENCHANT_PARITY` (new, `KTDXLIB/Always.h`).
**Depends on:** nothing in the buff port; it can run any time. Phase 4b-i part 4's "Enchant host math" note is its starting point.
**Run with:** Opus 5.5, high effort (RE plus an enum renumbering with a consumer audit). Review required (Opus 5.5).

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
