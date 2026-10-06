# June 2017 Buff/Debuff Port — phased plan

> Written 2026-10-02 and committed on `mods/lanox-port`. Start with Phase 0's kickoff
> prompt in a new conversation. Every kickoff prompt below points at this file.
>
> **2026-10-04:**
> - Phase state lives in `STATUS.md`. Kickoff and review prompts for unfinished phases live in `kickoffs/june-buff/` (start one with `/x2-phase june-buff <phase>`); inline prompts that remain below are history.
> - The finished Done/Review notes of Phases 4b-i to 6 moved to `JUNE_BUFF_PORT_ARCHIVE.md`; their headings stay here as stubs.
> - Read this file by section with `tools/docsection.py`, never in full.

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
| 16 | Socket-option parity, then BBT 92 (added 2026-10-04, from 5c) | 5c | RE + C++ + data | Opus 5.5 | high | required |

Phases 5–9 are independent of each other and can run in any order, except inside a group: 5 → 5b → 5c (92 needs Phase 4b's BBT 7/8 second members), 5 → 6 → 6b (91 shares 85/86's host function) and 7 → 7b. Kickoff and review prompts for the unfinished phases are in `kickoffs/june-buff/` (see `STATUS.md`). Phase 13 must come last among the data phases.

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
- Build, deploy, edit, hygiene (scripted since 2026-10-04; see `CLAUDE.md` *Workflow automation*):
  - `python tools/build.py --deploy`, run in the background. It builds in order with PCH invalidation, deploys `X2_offline.exe`, verifies it, and prints the `DEPLOY OK` line for the Done note.
  - Edit CP949 sources with `tools/bytepatch.py` (x2-byte-edit skill), never with Edit/Write.
  - Flag hygiene: `python tools/flagcheck.py <FLAG> --worktree --files <your files>`, then `--range` after committing. It replaces the per-phase "fresh byte-level unifdef" and the `file`/`git diff --stat` checks.
  - Read this doc by section: `python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "<heading>"`. Never read it in full.
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
  - Read them with `python tools/logs.py --tag JUNE_BUFF`; its header says whether the run used the deployed exe.
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
| `SERV_IRUHADEV_JUNE_SOCKET_PARITY` | 16 |
| `SERV_IRUHADEV_JUNE_ATTACK_RANGE_DESIGN` | 16 (designed, not June) |

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
| `32270dd0` | `CX2GameUnit::InitAdditionalStatOptionValue` (`X2GameUnit.cpp:12956`) | 5 |
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
| 76 CHANGE_HYPER_DURATION | `m_AdditionalHyperDuration`, 0.0 | `32282ddc` | **Option value:** `InitAdditionalStatOptionValue` (`32270dd0`, March `X2GameUnit.cpp:12956`) seeds it from `m_SocketData.m_fHyperModeTime` (`@322710a1`, beside HyperCharge's `m_fHyperModeChargeRate` `@32271089`), and sets 0 in the reset branch (`@3227143d`). **Reader:** `CX2GUUser::GetHyperModeTime` (`31fd1c00`, `CX2GUUser` slot 45; March `X2GUUser.cpp:23584`): June replaces `m_SocketData.m_fHyperModeTime` with this member's result, i.e. `fTotal += fTotal * m_AdditionalHyperDuration.GetResultValue() * 0.01f`. Class overrides read it too: Ain `32a4fe50`, and `329e5180`/`329e56c0` (a class with members at `+0x96xx`, not yet attributed). Port those with their classes. *Corrected by the Phase 5 review: `329e5180` is `CX2GUAdd::GetEffectiveDecreaseDPConsume` and `329e56c0` is the body of `CX2GUAdd::UpNowDPValue`. March has both, so the review ported the read (`X2GUAdd.cpp`). Only Ain's is for a class March lacks.* |
| 85 RANGE_COMMAND_MP_GAIN_RATE / 86 MELEE_COMMAND_MP_GAIN_RATE | two new members, 1.0 | `32282ee6` / `32282ec3` | `SetDamageExceptionProcess`, after `fHitAddMP = attacker->GetChargeMpHit{Melee,Range}Attack()` (March `X2GameUnit.cpp:14854-14866`): `if ( attackType == AT_COMMAND ) fHitAddMP *= attacker->{86 melee @3229935a, 85 range @3229947c}`. The Chung virtual `32c16be0` also sets both members' passive values (port with Chung's passive code, not here). *Phase 5 review: `32c16be0` is `CX2GUChung::InitPassiveSkillState`, which March has. It multiplies both members' passive multiple by a June Chung init-script key, `COMMAND_HIT_ADD_MP_BONUS`. Not ported; it is a decision for the user (see "Phase 5 — Review", R2).* |
| 87 MELEE_COMMAND_DAMAGE_RATE / 96 RANGE_COMMAND_DAMAGE_RATE | two new members, 1.0 | `32282f09` / `32282f2c` | `GetFinalDamage` main path `@32286270`–`322862b9`, after the final-defence/final-damage adds: `if ( attackType == AT_COMMAND ) dmg *= m_bMeleeAttack ? attacker->87 : attacker->96`. |
| 108 FINAL_DAMAGE_MULTIPLIER_NPC / 109 …_USER | two new members, 1.0 | `32282f72` / `32282f95` | `GetFinalDamage`, both paths: fixed-damage path `@32285fba`–`32285fec` and main path `@3228620a`–`32286252`. `inc2 = dmg * ((defender->GetGameUnitType() == GUT_USER ? attacker->109 : attacker->108) - 1)`, added beside March's `fIncreaseDamage` (`X2GameUnit.cpp:2616-2621`, `2707-2710`). The unit-type field is `+0x264`. |

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
  - *Corrected by the Phase 6 review (2026-10-04): there is no reset.* The ids vector is zeroed once before the loop and never cleared inside it (`32e849c0` `[esp+0x1c]`, `32e84d70` likewise, packets `32e84b90` / `32e84f40` `[esp+0x20]`); at each `-1.0` the block is **copied** into the member (`32e87770` -> vector copy ctor `32079c50`). So block k lists the ids of blocks 1..k, and with the getter's last-match rule every listed id gets the last block's rate. The rate variable is not reset either (`fld1` once), which shows only on two `-1.0` in a row. June's own 90 / 91 data has one block per factor, where nothing differs.
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

**Status: complete (2026-10-04).** 4b-i parts 1-5, 4b-ii and 4b-iii are done, play-tested and reviewed. The 4b-iii review found no open code defect. Phase 5 is next.

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

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-i part 1 — Review (2026-10-03, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-i, part 2 — progress (2026-10-03), launch check pending

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-i part 2 — Review (2026-10-03, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-i, part 3 — progress (2026-10-03)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-i part 3 — Review (2026-10-03, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-i, part 4 — progress (2026-10-04), launch check pending

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-i part 4 — Review (2026-10-04, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-i, part 5 — Done (2026-10-04), play-tested

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-i part 5 — Review (2026-10-04, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-ii — RE (2026-10-04)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-ii — Done (2026-10-04), launch check pending

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-ii — Review (2026-10-04, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

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

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 4b-iii — Review (2026-10-04, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

## Play-test hand-off (Phase 5 onward)

This is the user's standing request (2026-10-04). It applies to every play-test a session asks for from Phase 5 on, including a review that changes code. Every kickoff and review prompt from Phase 5 on points here.

**Use the dev console first.**
- Open it with **END** while in a field or dungeon. It takes raw Lua, one line at a time.
- It needs `settings.auth_level` = 3 in the game directory's `els_db.sql`. That is already set.
- Use another route (an item, a skill, a particular monster or map) only when the console cannot reach the check, and say why.

**Make every command copy-ready.**
- Fill in every value yourself: the real BFI/BTI number from the staged data or the Enum, and the real level. Never leave a `<BFI>`, "your monster here" or "pick a level" for the user to replace.
- If the right value depends on something only visible in game (which NPC, which slot), write the command so it does not need it, for example a loop over all NPCs.
- Use level 1 unless you have checked the factor's per-level tables. Many end in 0, and a 0 `PERCENT` raises a modal popup per behavior.
- Check the command in the session before handing it over: the method is Lua-bound (`lua_tinker::class_def` in `X2Lib/X2Main.cpp`) and the BFI exists in the data the exe will load. Say so if a line is checked from source but has never been run.

**Format, one numbered block per check:**
1. **Where:** village, field or dungeon, and anything to do first (for example "stand next to a monster").
2. **Paste:** the command, in its own code block, on one line.
3. **What it does:** one plain sentence, with no addresses or internal names.
4. **Look for:** what you should see if it works.
5. **If not:** what failure looks like, and what to send back (a screenshot, or "tell me nothing happened").

End the hand-off with the log lines the session will read afterwards. Keep that check to this phase's own lines.

**Command patterns.** The first is the one used in Phases 4b-i and 4b-iii. The second is checked from source but not yet run: `GetNPCUnit` returns `nil` for an empty slot (`lua_tinker.h:205`), and `CX2GUNPC` inherits the buff method (`X2Main.cpp:3895-3896`, `4147`, `4196`).
- On yourself:
  ```
  g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 152, 1 )
  ```
- On every monster on the map:
  ```
  for i = 0, g_pX2Game:GetNPCUnitListSize() - 1 do local n = g_pX2Game:GetNPCUnit( i ) if n ~= nil then n:SetBuffFactorToGameUnitByBuffFactorID_LUA( 152, 1 ) end end
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
- **March line numbers in a scope are hints.** They were measured when the scope was written, and every phase's edits shift them. Find the function by name first, then use the number to locate the spot inside it. Phase 5's numbers were re-measured on 2026-10-04 at `25f5bf3`; the later phases' numbers were not.
- Cite the June address in a comment at every new path. Never invent a value or a formula, and never port a raw struct offset.
- Types June uses only for characters March lacks (Rose, Ain, Lu/Ciel) are ported anyway (user decision).
- **Attribute an override before you defer it as "not in March"** (added by the Phase 5 review). March has Elsword, Aisha, Rena, Raven, Eve, Chung, Ara and Add; only Rose, Ain and Lu/Ciel are absent. Phase 5's map line called `CX2GUAdd` and `CX2GUChung` readers "not in March", and the review found both. Check every class-override address in the map (5b, 5c, 6, and 7b's BBT 97 list) against the class's vtable or neighbouring functions before you skip it.
- **Character code goes to the character port, buff code goes in regardless** (user, 2026-10-04).
  - June's characters are mostly new, and the March ones were revamped. Expect a buff type to need character code, script keys or assets March does not have.
  - Implement the buff side anyway (store, reader hook, getter/setter), so the character port only has to wire it.
  - When the missing half lives in a character function, add a row to `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7: June address, class, what it does, and the buff-side store. Do not graft it onto March's version of a function June rewrote.
  - Add on this branch is incomplete and will be ported from June; do not extend March's Add.
- **Escalation rule (Sonnet sessions).** If the map does not settle a type (missing layout, store or read site), do **not** reverse-engineer or guess it. Skip that type, mark it "Opus-only" in the doc with what is missing, and keep going.
- **Opus sessions that hit a design decision** (a deviation from June, a DamageData key March lacks, a system March does not have): stop that type, write the question into the doc's Done note, and continue with the others. Hand the decision to the user. Do not choose silently.
- **June bugs and quirks are kept verbatim; never ask** (user, 2026-10-04, after the Phase 7 review asked about the 94 restack leak). If June has a leak, an off-by-one, a value it always sends as 0, or an odd order, port it exactly, and add a row to `JUNE_BUGS.md` in the same commit. That is not a design decision. Ask only when copying June would break March: a crash, an unloadable script, or a missing enum or system.
- Done means: `buff_census.py` lists none of the phase's types, the build is rc=0 with the PCH invalidated (`touch X2Lib/stdafx.cpp`), and the strings are in the deployed exe. `buff_census.py --phase <N>` lists the whole census group (for 5b, 5c and 7b that is wider than the sub-phase), so check the named types only.
- Write a per-type **"how to see it in game"** line (a June BTI/BFI and where it comes from) into the Done note, for Phase 14.
- Record the session's `/cost` or usage in the Done note. Commit.
- A fresh Opus 5.5 review follows each phase (rule 10). Its prompt is under each phase. Review fixes go in the same phase's Done note.

---

### Phase 5 — group A: ChangeStat-class types (Sonnet)

**Scope.** Seven types, all transcription-ready (see "Per-type map", *Phase 5 — ChangeStat-class types*). The March line numbers below and in that table were re-measured on 2026-10-04 at `25f5bf3`. All share March's `ParsingChangeStatBehavior` layout (`32d445d0`) and class `CX2BuffChangeStatBehaviorTemplet`.
- **76 `BBT_CHANGE_HYPER_DURATION`.** New `CX2AdditionalStatByBuff` member `m_AdditionalHyperDuration`, basic 0.0. It joins both ChangeStat groups (lvl `32d47957`, SGU `32d4d6ef`). It is an **option value**: seed it in `InitAdditionalStatOptionValue` (June `32270dd0`, March `X2GameUnit.cpp:12956`) from `m_SocketData.m_fHyperModeTime`, and set 0 in the reset branch. Reader: `CX2GUUser::GetHyperModeTime` (June `31fd1c00`, March `X2GUUser.cpp:23584`), `fTotal += fTotal * m_AdditionalHyperDuration.GetResultValue() * 0.01f`. The class overrides (Ain `32a4fe50`, `329e5180`/`329e56c0`) are ported with their classes, not here.
- **85 `BBT_CHANGE_RANGE_COMMAND_MP_GAIN_RATE` / 86 `BBT_CHANGE_MELEE_COMMAND_MP_GAIN_RATE`.** Two new members, basic 1.0. Reader in `CX2GameUnit::SetDamageExceptionProcess` after `fHitAddMP = attacker->GetChargeMpHit{Melee,Range}Attack()` (March `X2GameUnit.cpp:14854-14866`): `if ( attackType == AT_COMMAND ) fHitAddMP *= attacker->{86 melee, 85 range}`. Needs `AT_COMMAND` from Phase 2. Chung's virtual `32c16be0` is not part of this phase.
- **87 `BBT_CHANGE_MELEE_COMMAND_DAMAGE_RATE` / 96 `BBT_CHANGE_RANGE_COMMAND_DAMAGE_RATE`.** Two new members, basic 1.0. Reader in `GetFinalDamage`'s main path (`@32286270`–`322862b9`): `if ( attackType == AT_COMMAND ) dmg *= m_bMeleeAttack ? attacker->87 : attacker->96`. Needs `AT_COMMAND`.
- **108 `BBT_CHANGE_FINAL_DAMAGE_MULTIPLIER_NPC` / 109 `…_USER`.** Two new members, basic 1.0. Readers in `GetFinalDamage`, both the fixed-damage path and the main path: `inc2 = dmg * ( ( defender is GUT_USER ? attacker->109 : attacker->108 ) - 1 )`, added beside March's `fIncreaseDamage` (`X2GameUnit.cpp:2616-2621`, `2707-2710`).
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
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
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
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Phase 5 — Done (2026-10-04), play-tested (user)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 5 — Review (2026-10-04, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

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
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
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
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Phase 5b — Done (2026-10-04), play-tested (user, logs checked)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 5b — Review (2026-10-04, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

---

### Phase 5c — group A: stat types with rewritten hosts (Opus)

**Scope.** Three types whose store and reader are mapped but whose host function is a June rewrite (item 3).
- **78 `BBT_TYPE_ADDITIONAL_ATTACK_MULTIPLIER`.** Store: new member, basic 1.0, insert `32282dff`. Readers: `CX2GUUser::DamageReact` `@32049466`–`32049561` (March `X2GUUser.cpp:9276-9291`) and `CX2GUNPC::DamageReact` `@327b42b0`–`327b43be`. The formula is `k = attacker->Opt5248() + this->Get78() + this->Opt5194()`, minus the defender's `Opt5ac8() * 0.01`, `v = max( k, 0 ) * attacker->GetAdditionalAttackDamage( pDD )`. The three option values come from socket-data fields March lacks, so in a March build they are 0 and `k = max( Get78(), 0 )`. **That deviation is exact on March data and may be ported; record it in the doc.** One block up, June also gates March's `GetAdditionalDefence` reduction on `!DD[+0x534]` (`IGNORE_DEFENCE_DECLINE`), which needs the same DamageData key as 88.
- **88 `BBT_IGNORE_DEFENCE`.** Class, store and reader are ready (class vt `33535558`; list `+0x67dc`; reader `32274970` = `min( Σ rate, 1.0 )`, defender side). The host is June's rewrite of March's `fPiercingDefenceFactor` (`X2GameUnit.cpp:2330-2334`), which also needs four DamageData keys March does not parse: `IGNORE_DEFENCE_ON_PVP`, `IGNORE_DEFENCE_RATE`, `IGNORE_DEFENCE_RATE_ON_PVP`, `IGNORE_DEFENCE_DECLINE` (stores `+0x529`/`+0x52c`/`+0x530`/`+0x534` in `SetDamageDataFromLUA` `32632390`; defaults in item 3), and `CX2Game::IsPvpOrPve`. **Port June's whole block, parsing the four keys** (no partial updates). Skip the frozen-debuff resist scaling just above it: it is not a buff change. If the keys or `IsPvpOrPve` need a decision, record it and stop 88.
- **Carry-over from the Phase 5b review (D1), comment only.** When `X2BuffBehaviorTemplet.cpp` is next edited, correct the comment on `CX2BuffChangeStatBehaviorTemplet_HaveBuffOwner::SetFactorFromPacket` (about line 4524). It says "June reads past the end", but June size-tests each of v[4..7] and reads 0, as the port does. No behaviour change.
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
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
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
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Phase 5c — Done (2026-10-04): 78 and 88 ported, 92 deferred

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 5c — Review (2026-10-04, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

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
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
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
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Phase 6 — Done (2026-10-04), play-tested (user checks 1-2; logs checked for 3 and the clean check 4)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

---

### Phase 6 — Review (2026-10-04, Opus 5.5 high)

Moved to `JUNE_BUFF_PORT_ARCHIVE.md` (same heading); `tools/docsection.py` follows this stub.

### Phase 6b — group B: skill-scoped hosts (Opus)

**Scope.** Wire the inert Phase 6 types into the functions June rewrote. Under the no-partial rule the default is to **port June's whole function**, not to add only the buff term. If a whole-function port would pull in a system March lacks and cannot be finished in this session, stop that host, write what it needs into the Done note, and hand the decision to me.
- **89, 90 and 98 → `CX2GUUser::DoStateEndStart`** (June `3203c3a0`, March `X2GUUser.cpp:6350`; the buff terms are at `@3203c7de`–`3203c801` for 89/90 and `@3203c649` for 98). June also adds a `+0x80d9` byte gate, a random sound/voice pick for state types 9/11/12, a protected power-rate cache read by `GetPowerRate` (June slot 129 `31fcb230`; March `X2GUUser.cpp:23045`) and `31fea430`, and the `+0x2557..0x255a` resets. 98's rate is cached at state start, so BFT 16 (Phase 8) ends the buff after the skill it applied to; Phase 8 depends on this order.
- **89, 90 and 98 → the summoned-NPC power helper** `322ece70`, called 21 times from `CX2Game::SetUserSummonedNPCInfo` (June `3230c1a0`, March `X2Game.cpp:14060`). June multiplies `( 3201ad10( id ) + 1 ) * GetSkillDamageRateByBuff( id ) * rate98summon * GetAdditionalAttackByType( AT_SPECIAL )`; March multiplies only `GetAdditionalAttackByType( AT_SPECIAL )` inline at each case. `IsSummonSkill` `31fc8200` has 54 ids (listed in item 1). The `+0x178` memo/base id field March lacks means the port uses the plain skill id; say so.
- **99 → `CX2GUUser::GetActualMPConsume`** (June `3203cf60`, slot 423; March `X2GUUser.cpp:21081`). June's function is much larger (`vfunc +0x75c`, `32039a50( id, 0x3f )`, `32358970( 0xb )`, the `+0x74dc` term); port it whole, then call 99's reader `3203a250` at `@3203d024`. Some of those terms may need systems March lacks: stop and report if so.
- **100 → the cooldown helper `3235a710`** and its 16 callers (the generic user path `31fd8ff0` plus one per-class override each). March sets cooldowns inline in each class (`X2GUAdd.cpp:2683`, `X2GUAra.cpp:3924`, `X2GUChung.cpp:5983`, …). Map the 16 sites to March's inline setters, then route them through one helper that applies 100 through `3203a420`.
- **Phase 9 only, do not port here:** `32369bd0`, `3236df00`, `32773ee0` (custom-function bodies that also read 89's list).

**Run with:** Opus 5.5, high effort. Review required.

**Kickoff:** `kickoffs/june-buff/6b.md` - start it with `/x2-phase june-buff 6b`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.


**Review:** `kickoffs/june-buff/6b-review.md` - start it with `/x2-phase june-buff 6b review`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

### Phase 6b — Done (2026-10-04)

**Play-test (2026-10-04, user, jtCodeEmpress; exe `c85baf49…`, run 19:58:14 after the 19:44 deploy): checks 1 and 2 passed.** `ErrorLog.txt`: load baseline unchanged (`factors 361 loaded`, no reject for 362–365); `:189-192` `first apply BBT 89/98/99/100`; `:193` `first host BBT 99 … skill 5003 MP 300 -> 150`; `:194` `first host BBT 100 … skill 5000 cool time left 9 -> 4.5`; `:195` `first host BBT 89/90/98 DoStateEndStart: state 53 skill 5000 power rate x 1.5 (89 x 90) x 1.2 (98) = 1.8`; `:197` `first host BBT 89/90/98 summon: skill 5003 power rate x 1.5 (89 x 90 1.5, 98 summon 1)`. All four hosts are shown live. 90 shares 89's reader and host, and was shown numerically in Phase 6. Only the review is owed.

**Result.** 89, 90, 98, 99 and 100 have their hosts wired under `SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL` (no new flag; step-8 lines under `_DEBUG`). Each whole-function port pulled in June-only systems, so the user chose **"buff terms only"** for all four hosts (2026-10-04). June's buff math, order and timing are ported exactly; the June-only parts listed below stay out as recorded deviations.

| Host (June) | March site | What landed | Not ported (user decision) |
|---|---|---|---|
| `DoStateEndStart` `3203c3a0` | `X2GUUser.cpp` `DoStateEndStart`, after the state copy, before StateStart | @3203c586: `IsSummonSkill` → summon 98 cache. @3203c649: 98 recomputed only when the skill changes (old id kept before the copy). @3203c7de/@3203c801: `m_fPowerRate *= GetSkillDamageRateByBuff × 98`. Both caches start at 1.0 (June ctor `FLD1` @3203ef02/@3203f0f6). March's `m_NowStateData` is a per-state copy that `GetPowerRate` returns, so it is the cache. | random START/WIN/LOSE state-table pick (`31fd65f0`; states 9/11/12, lists `+0x229f/+0x22a3/+0x22a7`), the one-shot StateEnd override (`+0x2557..+0x255a`), the per-class hook at vtable `+0x78c` (empty in `CX2GUUser`, `31f3f070`) |
| summon helper `322ece70` (21 calls in `3230c1a0`) | `X2Game.cpp` `SetUserSummonedNPCInfo`: the 14 cases that set a power rate (Ferdinand, Wally 9th, Fire Eve, Guardian SP1/SP2, Atomic Shield, Anger of Elf, UFO, UFO memo, both Chung targeting, Evoke, Distortion, Remote Mortar) | `× GetSummonPowerRateByBuff( the case's skill )` = 89 × 90 × summon 98 (@322ed30d, @322ed31c), beside March's AT_SPECIAL multiply. Mortar: before March's `-= 1` (June flag `0x400`, @322ed366) | `3201ad10( id ) + 1`, the flag-driven stat lookups and `0x200` member (`+0x8ce4`). June also routes the default and every other summon through the helper; the 6 March cases that set no power rate (default incl. AI-party bots, Nasod Watch, Entangle, Stone Wall, Particle Prism, Fire Blossom) are left as March has them |
| `GetActualMPConsume` `3203cf60` | `X2GUUser.cpp`, inside 4b-i part 4's `BEHAVIOR_PARITY` block | @3203d024: `ApplySkillTypeConsumeMpRate` on the MP value, then `× BBT 63 rate` (@3203d097) | alternate templet `323511f0`, vslot 471, `32358970( 0xb )`, the `+0x74dc` cut floored at 0.8 (`33478a78`) |
| cool-time helper `3235a710` (16 callers) | new `CX2Unit::ApplySkillCoolTimeRateByBuff` (`X2Unit.cpp`), called after the set in `CX2GUUser::ChangeEquippedSkillState` (`31fd8ff0`), `CX2User::ChangeEquippedSkillState` (`3264ad40`), Raven `InitShadowStep` (`32c4de60`) and the vtable-`+0x800` skill-use function of Eve, Ara, Arme, Chung, EL, Elsword, Lire, Raven (`SpecialAttackEventProcess` / `StateChangeSpecialAttack`) | reads the left time back (`GetSkillSlot` = `3288ff80`), applies 100 through the game's `CX2GUUser` (`32358770` → `3203a420`), writes only if 100 applied (June: or a rate ≠ 1) | `32358480` (June skill-upgrade cool-time modifier) and `max( 3234fbd0, 0.8 )` (socket factor, **Phase 16 adds it in the marked line**). Add (`32a07dc0`), Ain, Rose, Luciel and Eve's Psyker Kinesis (`32976fe0`) → checklist §7 |

**Callers mapped** (RTTI of the data refs; all 10 virtual ones are vtable slot `+0x800`): Eve `32981130`, Add `32a07dc0`, Luciel `32b1eef0`, EL `32b73e60`, Ara `32bb1ff0`, Chung `32bf5130`, Raven `32c43bf0`, Lire `32c89a20`, Arme `32ccf4e0`, Elsword `32d0d8e0`; non-virtual: `31fd8ff0`, `3264ad40`, Eve `32976fe0`, Ain `32a4df00`, Rose `32aad080`, Raven `32c4de60`. June's 21 summon calls were mapped to NPC ids by emulating the switch over ids 0–70000 (scratch `emu_summon.py`); the cases March lacks are listed in checklist §7. **Phase 16 conflict settled:** `32358770` is `__fastcall` on the `CX2Unit` and passes `(skill, &coolTime)` straight to `3203a420`, so its Phase 1c row stands.

**Skill ids.** June's player skills use the reformed `RNSI_` numbering (e.g. `RNSI_SA_AVP_HELL_DROP` 22020); March's C++ uses the old one (`SI_SA_EN_ILLUSION_STRIKE` 3000). So `IsSummonSkillByJune` (the 54 ids, ported verbatim with names) matches no March skill, and **the summon 98 cache stays 1.0 on March**. 98 itself needs June's transcendence slot to do anything (Phase 6 Done). June's `+0x178`/`328905b0` base-id mapping is absent; plain ids are used throughout.

**Phase 8 hand-off.** 98 is cached in `DoStateEndStart` **before** `CommonStateStart` (June @3203c649 < @3203c978; March: the new block sits before `BeginTableByReference`/StateStart), so a BFT 16 finalizer that fires in StateStart ends the buff after it applied to that skill. Phase 8 is unblocked.

**Hygiene.** `flagcheck JUNE_BUFF_NEW_SKILL --worktree --with JUNE_BUFF_DEBUG` on the 14 code files: `files: 14 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. Two replaced lines keep March's in `#else` (the MP return, Distortion's `SetDefaultPowerRate`). The rest are pure insertions.

**Build and deploy.** `DEPLOY OK: X2_offline.exe is 19,092,992 B, dated 2026-10-04 19:44:46, md5 c85baf49d7a57882051c1e19e2308b57, equal to X2/US_SERVICE/x2.exe (2026-10-04 19:44:41); built at mods/june-buff-port 0d3bff8 + 14 uncommitted code file(s).` Full X2Lib rebuild (PCH invalidated by `X2Unit.h`). The four `first host BBT …` strings are in the exe. **Not launched.**

**How to see it in game (Phase 14).** June numbers from `june_src_fixed/data036_kom/` (BFI = ordinal). 89: `BFI_BUFF_2013_DEFENSE_BUFF` 195 / `BFI_BUFF_OVER_STRIKE` 707: skill damage numbers of the listed skill types rise, summons included. 90: `BFI_SI_A_LWS_NATURE_FORCE` 23: ×1.3 on the listed skill ids. 98: `BFI_TRANSCENDENTAL_SLOT_ATTACK_EFFECT` 843: needs the transcendence slot (none in March). 99: no June data. 100: `BFI_BUFF_OVER_STRIKE_RSR_TRANSCENDENCE` 918: cooldowns of the grade skill types 10% shorter, in a field or dungeon only (no game, no 100). All of them need June's `SKILL_TYPE` grade names in the Enum (Phase 6 finding 1).

**Open for the user / review.** (1) Raven's three state-power-rate assignments overwrite the buff terms for those states (checklist §7; June not checked). (2) `GetSummonPowerRateByBuff` uses the March case's skill, where June derives the skill from the NPC (`31fcf520`/`31fcf430`, not read in this phase). The review may confirm that June's mapping gives the same skill per case. (3) **Ferdinand** (`NUI_SI_HA_FERDINAND`, June flags `0x260`): June writes this case's buff-scaled rate to the NPC's `+0x8ce4` member, not to `+0x8ce0` (the default power rate). The port multiplies March's default power rate, the member March uses for this case. Dropping that one site would be a design change, so it is the user's call. (4) **Cooldown writes** go through March's `SetSkillCoolTimeLeft`. That applies the item-event cut `GetSkillCoolTimeDecreaseRate` again (`SERV_BALANCE_FINALITY_SKILL_EVENT` is on, `ServerDefine_Global.h:1238`). June's writer `32893880` does the same with `3234cc10` (same item 60008065, same skill list), so the double cut matches June. March's setter also resets relationship and hyper-active skills to their templet time (`FINALITY_SKILL_SYSTEM`), which June's writer does not do: 100 cannot change those two skill types on March. (5) **Cost:** `/cost` cannot be read from inside the session: user, please paste it here.

**Play-test hand-off (dev console, END; `settings.auth_level` = 3 is set).** The setup lines are Phase 6's 363/366/367/368 lines, which ran in game, with their ids renumbered to 362–365 (the last load logged `factors 361 loaded`) and 100's value changed from −0.1 to −0.5. They have not been run with these ids. **Character: jtCodeEmpress (Eve, Code: Empress, Lv 80).** It has Dimension Link Guardian (5003, summons the Nasod Element / Fire Eve, one of the patched summon cases), Space Wrench (5014), Illusion Stinger (5000) and Illusion Strike (5200) in the slot A row, skill types 7 and 6. The summon check cannot be done from the console on any character: it needs a character whose own skill summons one of the 14 patched NPCs. (Skill ids from the active `UPGRADE_SKILL_SYSTEM_2013` enum, `X2SkillTree.h:85`. An earlier draft read them from the inactive `#else` enum.) The buffs last 60 s.

0. **Setup.** Where: a field or dungeon with ordinary monsters, **fresh launch**. Paste all four, in order. Look for: no popup, no `ordinal reject BFI 362`–`365`. If not: send the `ordinal reject … next id must be N` line.
   - Paste, factor 362 (89, +50 % power for skill types 6 and 7):
     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990089, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 89 }, BBT_CHANGE_SKILL_DAMAGE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 362, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990089, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 89 }, BBT_CHANGE_SKILL_DAMAGE = { SKILL_CATEGORY_0 = { SKILL_TYPE = 6, MULTIPLY_DAMAGE_RATE = { 1.5 } }, SKILL_CATEGORY_1 = { SKILL_TYPE = 7, MULTIPLY_DAMAGE_RATE = { 1.5 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - Paste, factor 363 (98, x1.2 for skills in the slot A row):
     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990098, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 98 }, BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 363, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990098, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 98 }, BBT_CHANGE_SKILL_DAMAGE_BY_SKILL_SLOTTYPE = { MULTIPLY_DAMAGE_RATE = 1.2, SKILL_SLOT_TYPE = 1 } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - Paste, factor 364 (99, half MP cost for skill types 6 and 7):
     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990099, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 99 }, BBT_CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 364, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990099, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 99 }, BBT_CHANGE_CONSUME_MP_RATE_BY_SKILLTYPE = { SKILL_CATEGORY_0 = { SKILL_TYPE = 6, BUFF_CHANGE_TYPE = 3, VALUE = { -0.5 } }, SKILL_CATEGORY_1 = { SKILL_TYPE = 7, BUFF_CHANGE_TYPE = 3, VALUE = { -0.5 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
   - Paste, factor 365 (100, half cooldown for skill types 6 and 7):
     ```
     local t = {} local r = {} for i = 1, 30 do t[i] = 60 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990100, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 100 }, BBT_CHANGE_COOLTIME_RATE_BY_SKILLTYPE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 365, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990100, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 100 }, BBT_CHANGE_COOLTIME_RATE_BY_SKILLTYPE = { SKILL_CATEGORY_0 = { SKILL_TYPE = 6, BUFF_CHANGE_TYPE = 3, VALUE = { -0.5 } }, SKILL_CATEGORY_1 = { SKILL_TYPE = 7, BUFF_CHANGE_TYPE = 3, VALUE = { -0.5 } } } }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
     ```
1. **Skill power, MP and cooldown (89, 98, 99, 100).** Where: same map. As jtCodeEmpress, use Illusion Stinger (slot A, 3rd) once on a monster first and note its damage, the MP it took and its cooldown. Wait for the cooldown, then paste:
   ```
   local u = g_pX2Game:GetMyUnit() for _, f in ipairs( { 362, 363, 364, 365 } ) do u:SetBuffFactorToGameUnitByBuffFactorID_LUA( f, 1 ) end
   ```
   and use Illusion Stinger again within 60 s. What it does: your type-6/7 skills hit 1.5 × 1.2 harder, cost half the MP and get half the cooldown. Look for: damage about ×1.8, the MP bar drops about half as much (the cost printed on the slot does not change; it is cached at equip), the cooldown overlay about half as long. `ErrorLog.txt`: `first host BBT 89/90/98 DoStateEndStart: state … skill 5000 power rate x 1.5 (89 x 90) x 1.2 (98) = …`, `first host BBT 99 GetActualMPConsume: skill 5000 MP X -> X/2`, `first host BBT 100 cool time: skill 5000 cool time left L -> L/2`. If not: send every `[JUNE_BUFF]` line naming 362-365, 990089/990098/990099/990100, BBT 89/98/99/100 or `first host`.
2. **Summon power (89 on the Nasod Element).** Where: same map, within the same 60 s (or paste the apply line again). As jtCodeEmpress, use Dimension Link Guardian (slot A, 1st). What it does: the summoned Nasod Element's power rate gets the skill's 89 term. Look for: `first host BBT 89/90/98 summon: skill 5003 power rate x 1.5 (89 x 90 1.5, 98 summon 1)`; 98 summon stays 1 because March's skill ids are not June's (Done note, "Skill ids"). If not: send the `[JUNE_BUFF]` lines and say whether the Nasod Element appeared.

Log lines read afterwards: `first host BBT …` and any `[JUNE_BUFF]` line naming 362–365 or 990089/990098/990099/990100.

### Phase 6b — Review (2026-10-04, Opus 5.5 high)

**Play-test passed (2026-10-04, jtCodeEmpress; exe `5a81b0ee`, run 20:16:06 after the 20:13 deploy, clean exit).** `ErrorLog.txt:191` `first apply BBT 89 … BTI 990089`; `:192` `first host BBT 89/90/98 DoStateEndStart: state 49 skill 5003 power rate x 1.5 (89 x 90) x 1 (98) = 1.5`; no `first host … summon` line in the whole run (the old exe logged ×1.5 for the same skill). The user confirmed that the Nasod Element appeared. Load baseline unchanged (factors 361). The review is closed.

**Verdict: one transcription error, fixed under `SERV_IRUHADEV_JUNE_BUFF_NEW_SKILL`.** Everything else matches June. The deviations the Done note records (non-buff parts left out by user decision) are as described.

**Hygiene, 6b code commit.** `flagcheck JUNE_BUFF_NEW_SKILL --commits 57546e5 --with JUNE_BUFF_DEBUG`: `files: 14 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. So the flag-off path is March byte for byte. The two replaced lines (the MP return, Distortion's `SetDefaultPowerRate`) keep March's code in `#else`. The three doc commits touch only `.md` files.

**Ghidra spot-checks (read-only). All three pass.**
- `GetActualMPConsume` `3203cf60` (raw disassembly; not carved). `rate = vslot471 × 32039a50( id, 0x3f )`. The bool argument adds `32358970( 0xb )`. Then `3203a250( templet, &mp )` @3203d024 applies 99 to the **MP value**; the `+0x74dc` cut (bool only) follows; the function returns `rate × mp` (@3203d09c). The port runs 99 on the MP value, then × the BBT 63 rate. Same order.
- Cool-time helper `3235a710`:
  1. Reads the left time back (`32891460`) and continues only if it is > 0.
  2. Calls `32358770( id, &left )`.
  3. If `max( 3234fbd0, 0.8 ) × 32358480 == 1`, it writes only when 100 applied; otherwise it scales.
  4. Writes with `32893880`.
  
  `CX2Unit::ApplySkillCoolTimeRateByBuff` is this function with the product fixed at 1.0. `31fd8ff0` calls it outside the templet test, as the port does.
- `DoStateEndStart` `3203c3a0`, in this order:
  1. `IsSummonSkill` → summon 98 (`+0x7ce0`) @3203c586.
  2. 98 (`+0x7cd4`) only if the old state's skill ≠ the new one's (@3203c63d/@3203c649).
  3. Hook `+0x78c`.
  4. Cache = the new state's `m_fPowerRate` (@3203c770).
  5. × `31fd7cc0` (@3203c7e4/@3203c801).
  6. × 98 (@3203c89a).
  7. `CommonStateStart` (@3203c978).
  
  The port has the same order, without the hook (recorded deviation).

**Further checks.**
- All 54 `IsSummonSkillByJune` ids and their names match June's `Enum.src.lua`.
- In all 8 classes, Raven `InitShadowStep` and both equip paths, the helper call comes after every cooldown set, with no `return` in between. Those 10 sites are null-safe.
- Mortar: the buff terms come before `-= 1`, as June's flag `0x400` tail does.
- The summon 98 term is unconditional in `322ece70`, which matches `GetSummonPowerRateByBuff`.
- Other writes to `m_NowStateData.m_fPowerRate`:
  - Chung, Elsword and Lire only multiply it, so the buff terms survive.
  - Raven's three `=` assignments (`X2GURaven.cpp` ShadowStepThrust, ShadowBuster, ShadowStepPunisher) are the only overwrites. They are already Done-note open item (1).

**Fix: the summon skill per case (Done-note open item 2).** June derives the summon's skill from the NPC: `31fcf520( npcId )`, then `31fcf430`, which is the identity when `+0x867c` is 0. It does not use the case's skill.
- 10 of March's 14 patched cases match June's table by name. Each NPC id comes from June's `Enum.src.lua`:
  - Ferdinand 1454 → 25601 `LINK_OVERCHARGE_ILLUSION`
  - Guardian SP2 411 / 520 → 25013 `SONIC_WAVE`
  - Atomic Shield 682 → 25011
  - Anger of Elf 1420 / 1421 → 23216
  - UFO 495 / 1575 → 22020 `HELL_DROP`
  - Chung targeting 1419 → 26014 `QUANTUM_BALLISTA`
  - Chung targeting 863 → 26025 `ARTILLERY_STRIKE_MISSILE_SHOWER`
  - Evoke 1020 / 1025 → 23218
  - Distortion 1091 → 22027
  - Mortar 1100 → 26215
- **Wally 9th (385), Fire Eve (386) and Guardian SP1 (410, memo 674) are not in `31fcf520`, so June returns 0.**
  - They still reach the helper through `3230c1a0`'s switch default (flags 0, @3230ca64).
  - `31fd7cc0( 0 )` returns 1.0, so June applies only the summon 98 term.
  - The port had passed Dimension Link Blader, Dimension Link Guardian and Surface Cutting, so those summons got 89 × 90.
  - **Fixed** (`X2Game.cpp`, inside 6b's flag block): those three cases now pass `GetSummonPowerRateByBuff( 0 )`.
  - `flagcheck --worktree --files X2Lib/X2Game.cpp`: `1 PASS`; `6 deleted line(s)`, all of them 6b's own flagged lines being replaced; `0 encoding change(s)`; `RESULT: PASS`.
  - Phase 6b play-test check 2 (Fire Eve × 1.5) exercised the old, wrong mapping.
- The reverse case: June maps Nasod Watch (384 / 519) to 25000 `DIMENSION_LINK_SENTRY`. March's Nasod Watch case sets no power rate (one of the 6 cases left as March). That stays with the character port.
- **User veto:** if you would rather keep the case skill for these March-only summons, revert the three lines; they are commented.

**Build.** `BUILD OK: X2/US_SERVICE/x2.exe 19,092,992 B, 2026-10-04 20:11:39` (`X2Game.obj` recompiled 20:11:30). The first deploy was refused while the game ran; after it closed: `DEPLOY OK: X2_offline.exe is 19,092,992 B, dated 2026-10-04 20:13:31, md5 5a81b0eeecba7c4891b937da79c7aba1, equal to X2/US_SERVICE/x2.exe (2026-10-04 20:11:39); built at mods/june-buff-port c3010ac.` Range check `--range c3010ac^..c3010ac`: `1 PASS`, `RESULT: PASS`.

**Answers to the Done note's open items.** (2) Answered and fixed, above. (1), (3) and (4) are unchanged. (4)'s claim that June's writer applies the event cut again was not re-read. (5) `/cost`: user, please paste it.

**Play-test hand-off (dev console, END).**
- **Where:** a field or dungeon, fresh launch, as **jtCodeEmpress** (Eve, Code: Empress, Lv 80).
- **Setup:** paste Done-note setup line 0's factor **362** (89).
- **Paste:**
  ```
  g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 362, 1 )
  ```
  Then use **Dimension Link Guardian** (slot A, 1st) within 60 s, then Illusion Stinger once.
- **What it does:** the summoned Nasod Element no longer gets the skill-damage buff, as in June. Your own skills still do.
- **Look for in `ErrorLog.txt`:**
  - No `first host BBT 89/90/98 summon` line.
  - `first host BBT 89/90/98 DoStateEndStart: … x 1.5 (89 x 90)` still appears.
- **If not:** a `summon:` line means the exe is stale. Send the `logs.py` header.

No character in the save has a summon skill whose case matches June, so the positive summon path is left to Phase 14.

---

### Phase 7 — group C: flag and immunity types with ready readers (Opus)

**Scope.** Five types whose class and reader are settled in "Phase 1c, item 2".
- **73 `BBT_IGNORE_STATE_GUARD`.** Own class: `SetFactor` `32e752b0` (insert, list June `+0x67cc`), `DoFinish` `32e75300`, packet `32e728b0`; `GetFactor` `32e7e960` writes the header only. Reader in `GetFinalDamage` `@322862bf`, inserted before March's state-defence block (`X2GameUnit.cpp:2716-2731`): `if ( !defender->list73.empty() ) pDD->m_bIgnoreStateDefence = true; if ( attacker->HaveThisBuff( BTI_BUFF_SCHWERT_PLATZEN /*975*/ ) ) pDD->m_bIgnoreStateDefence = true;`. **BTI 975 is in March's Lua Enum but not in the C++ `BUFF_TEMPLET_ID` enum: add the enumerator.** June writes the flag into the attacker's DamageData, so on multi-target damage data it stays set for later targets; transcribe verbatim and note it for Phase 14.
- **94 `BBT_IMMUNE_BUFF_DEBUFF`.** Factor parse `32d4a770`; `SetFactor` `32e79e30`; unit helpers `3227a910`, `322780f0`, `32278120`, `3227b6b0`. **Fill in Phase 4a's stub `CX2GameUnit::IsImmuneBuffByBuffBehavior`** (June `322a80a0`). Transcribe both June quirks verbatim: the loop starts at index 2 (the count is stored as an immune BTI, so an empty block also grants SILENCE immunity), and `GetFactor` sends the two `ALL_*` bytes as 0 so peers always receive 0.
- **95 `BBT_IMMUNE_NON_CONTROL_DEBUFF`.** `SetFactor` `32e75c00` inserts into the list (`+0x75ac`) **and calls `DeleteNonControlDebuff`** (`3229dcc0`); the packet handler `32e728d0` inserts only. **Fill in the stub `IsImmuneNonControlDebuffByBuffBehavior`** (June `3227b790`): `!list95.empty() && templet->IsNonControlDebuff()`. Port `DeleteNonControlDebuff` as a `CX2GameUnit` member here; the Lua binding of the same name is Phase 10's.
- **104 `BBT_JUMP_IMPOSSIBLE`.** Identity type, list `+0x42c0`. User: `CX2GUUser::SetInputData` (June `31fe83a0`, March `:28554`), the Up-key clear (`:28668`) becomes `m_fCanNotJumpTime > 0 || !list104.empty()`. NPC: `CX2GUNPC::CommonEventProcess` (June `3276d980`, March `:21405`), gate `SCT_AI_JUMP` on the list (`@3276e442`, March `:21819`). The same NPC switch also gates `SCT_AI_DASH`, `SCT_AI_JUMP_DIR` and `SCT_AI_DOWN_DIR` on the existing DASH_IMPOSSIBLE list (a Phase 4b item): land it if Phase 4b-i part 2 did not, or say so. Do not port the dead `+0xded` byte.
- **105 `BBT_IMMUNE_EDT`.** Class `SetFactor` `32e79770`, no unit store (the templet's `m_vecEDT` is the store); reader `IsImmuneEDTByBuff` `322a0400` walks `GetBuffFactor()` for type 105. Host: `CX2GameUnit::ProcessEnchantExtraDamage` (June `322a0510`, slot 213; March `X2GameUnit.cpp:3529`), after `if ( rate < fRandomValue ) return false;` add `if ( IsImmuneEDTByBuff( basicExtraDamageType ) ) return false;`. **The host's rate math is Phase 4b's** (`CHANGE_ENCHANT_ATTACK_RATE`, `32274df0`/`32272d90`); do not touch it and say so.

**Run with:** Opus 5.5, high effort. Review required.

**Kickoff:** `kickoffs/june-buff/7.md` - start it with `/x2-phase june-buff 7`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.


**Review:** `kickoffs/june-buff/7-review.md` - start it with `/x2-phase june-buff 7 review`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

### Phase 7 — Done (2026-10-04)

**Play-test (2026-10-04, user; exe `532eecfb`, run 20:52:53 after the 20:46 deploy): all 5 checks passed by eye.** `ErrorLog.txt`:
- `:177-178` 104 first apply (BTI 990104) and jump key cleared.
- `:179-180` 95 erased BTI 990004, then first apply.
- `:181-182` 73 first apply and state defence ignored.
- `:481-482` 94 first apply, `ALL_DEBUFF` refused BTI 990073.
- `:502-503` 105 first apply, refused EDT 100.

Load baseline unchanged (`factors 361 loaded`).

Open:
- (a) Settled: check 2 was re-run at 21:02 on the same exe. `:177-179` show 95 erased BTI 990004, its first apply, then `a new non-control debuff was refused, BTI (990004)` 9 s later. Dash kept working.
- (b) The burn came from the ground fire in a Burning Forest dungeon. 94 refused its debuff (no icon, no tint); the remaining damage is most likely the fire's contact hits, which are not a buff. Not verified. If the review or Phase 14 sees damage keep ticking after leaving the fire with 94 on, check how June routes that burn.
- (c) The console templets have no icon keys, so the missing buff icons are expected.

**Result.** 73, 94, 95, 104 and 105 are complete (parse, class, store, reader, host, packet) under the new flag `SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG` (`Always.h`); step-8 lines are under `_DEBUG`. `buff_census.py --phase 7 --reprobe` lists none of the five (left: 74, 77, 97, 101, 102, 110 = Phase 7b). Uncarved class functions were read with `june2017_re/disasm_range.py`; every item-2 decode held. One addition: each `SetFactor` first tests `GetBehaviorFactor` (`32d41780`) and returns false without it.

| Type | Parse (switch `32d4bbf0`) | Class / store | Reader and host |
|---|---|---|---|
| 73 | empty `32d45b60` | own class; `m_vecIgnoreStateGuardPtr` (+0x67cc), insert first apply / packet, erase one | `GetFinalDamage` after the 87/96 block: defender's list or attacker `HaveThisBuff( BTI_BUFF_SCHWERT_PLATZEN = 975 )` (new enumerator) sets `m_bIgnoreStateDefence` |
| 94 | `ParsingImmuneBuffDebuffBehavior` (`32d4a770`): ALL_BUFF, ALL_DEBUFF, list with count forced | own class; `std::set` all-buff / all-debuff by BTI, `m_vecImmuneBuffList` | stub `IsImmuneBuffByBuffBehavior` filled with `322a80a0` (null templet → false). Both quirks verbatim (loop from index 2; `GetFactor` sends 0/0) |
| 95 | empty | own class; `m_vecImmuneNonControlDebuffPtr` (+0x75ac); first apply also `DeleteNonControlDebuff` (`3229dcc0`, new member, index walk over `EraseBuffTempletFromGameUnitByIterator`); packet inserts only | stub `IsImmuneNonControlDebuffByBuffBehavior` filled with `3227b790` |
| 104 | empty | `CX2BuffIdentityBehaviorTemplet`; `m_vecJumpImpossiblePtr` via the identity switch | `CX2GUUser::SetInputData` Up clear `|| !list.empty()`; `CX2GUNPC::CommonEventProcess` `SCT_AI_JUMP` gated (@3276e442). The dash / jump-dir / down-dir gates were already in from 4b (`BEHAVIOR_PARITY`); `+0xded` not ported |
| 105 | `ParsingCustomBehavior` (`32d45c50`, shared with CUSTOM_FUNCTION) | own class; `m_vecEDT` is the store | `IsImmuneEDTByBuff` (`322a0400`, via `GetBuffFactor` = `3229fd80`, same skips) inside `ProcessEnchantExtraDamage`'s attacker block after the rate test. **The rate math is untouched: Phase 4b's** (`32274df0`/`32272d90`) |

**Hygiene.** `flagcheck JUNE_BUFF_NEW_FLAG --worktree` on the 9 code files: `files: 4 PASS, 4 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 4 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 4 are plain `#else` lines (the stubs, the two jump gates), as in 4b's NPC gates. The stubs keep March's `return false;` in `#else`.

**Build and deploy.** `DEPLOY OK: X2_offline.exe is 19,104,256 B, dated 2026-10-04 20:46:55, md5 532eecfb210243d56152a5731af4b879, equal to X2/US_SERVICE/x2.exe (2026-10-04 20:46:50); built at mods/june-buff-port a663319 + 9 uncommitted code file(s).` The `first apply BBT %d (%s)`, `first apply BBT 104`, `first read BBT %d: %s (%d)`, `ALL_DEBUFF` and `IMMUNE_BUFF_TEMPLATE_ID` strings are in the exe. **Not launched.**

**How to see it in game (Phase 14; June data, needs Phase 13).**
- 73: `BFI_DEBUFF_PANDEMONIUM_FEAR` 212 → BTI 223 (Raven). The feared monster takes hits through guard and damage reduction. BTI 975 is Ain's (absent).
- 94: `BFI_BUFF_BLOODLUST` 779 → BTI 753: refuses its listed BTIs. `BFI_DEBUFF_SLEEP` 811: quirk 1 also grants SILENCE immunity.
- 95: BTI 809 `BTI_AMUSEMENT_ITEM_SHIELD_IN_DAMAGE`, an inline factor in `DamageEffect_ETC.src.lua:1290`. On apply it strips stun-free debuffs (dash/attack/jump-impossible…) and refuses new ones. `FIRE_DEVIL_CHAIN_IPLITAN` calls `DeleteNonControlDebuff` from Lua (Phase 10 binding).
- 104: `BFI_DEBUFF_HEAVY_FIELD` 992 → BTI 937 (damage effect). No jumping; NPCs skip their jump AI.
- 105: `BFI_BUFF_ALPTRAUM_READY` 1086 → BTI 1008 (Ain): immune to `EDT_ENCHANT_SHOCK`.

**Notes for the review / Phase 14.**
1. June renumbered `EXTRA_DAMAGE_TYPE` (BLAZE 1001; March 100). June data names the type (`EXTRA_DAMAGE_TYPE.EDT_ENCHANT_SHOCK`), so compiled against March's Enum it matches March's C++. A raw number in June data would not; none was found.
2. `DeleteNonControlDebuff` erases from `m_vecBuffTempletPtr`. If a 95 buff were created from inside a loop over that vector (e.g. `BBT_CREATE_BUFF_FACTOR` during `OnFrameMoveBuff`), the loop would shift. June has the same order. Parity, not guarded.
3. 73 and 104 take June's level-slice default branch (empty factor → `[0, 0]`), as June does; their classes ignore the values.
4. 73's flag stays on the attacker's DamageData for later targets of a multi-target hit (verbatim).
5. Cost: `/cost` cannot be read from inside the session. User, please paste it here.

**Play-test hand-off** (dev console, END). Use 6b's line shape, one factor per line, ids 362–367 in order after a **fresh launch** (`factors 361 loaded`). Checked from source, never run. Every buff lasts 30 s. Id map: 362 → 73 (BTI 990073, debuff), 363 → 94 (990094, `ALL_DEBUFF = TRUE`, empty list), 364 → 95 (990095), 365 → 104 (990104, debuff), 366 → BBT 4 dash-impossible (990004, debuff, a non-control test target), 367 → 105 (990105, EDT 100–105). Checks, in order, waiting 30 s between them:
1. 104 on self: no jumping.
2. 366, then 364, then 366 again: dash comes back, and the second 366 is refused.
3. 363, then 362 on self: refused.
4. 362 on every NPC, then hit one.
5. 367 on every NPC, then hit them as **Iivue** (Eve, Code: Nemesis, Lv 80; equipped weapon Blaze + Blaze): no burn.

Log lines read afterwards: `first apply BBT 73/94/95/104/105` and `first read BBT …` (73 state defence; 94 ALL_DEBUFF refused BTI 990073; 95 erased / refused BTI 990004; 104 jump key; 105 EDT 100).

### Phase 7 — Review (2026-10-04)

**Verdict: no transcription errors, no code change.** Reviewed `6892072` (code) plus the doc-only commits `fd4cc95`, `1d7fc10` and `58165a3`. The deployed exe is still Phase 7's: `X2_offline.exe` md5 `532eecfb210243d56152a5731af4b879`, 19,104,256 B, 2026-10-04 20:46:55, re-read from the game directory. With no code change there was nothing to rebuild or deploy, and nothing new to play-test.

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG --range 6892072^..HEAD`:
- `files: 4 PASS, 4 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
- `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 4 #else/#endif without the flag token, 0 encoding change(s)`
- `RESULT: PASS`

The 4 are the plain `#else` lines (the two stubs and the two jump gates), as the Done note says. Every replaced line keeps March's code in `#else`.

**Checked against June.** Each was read with `june2017_re/disasm_range.py`.
- **Enums.** BBT 73/94/95/104/105 match June's `Enum.src.lua`. `BTI_BUFF_SCHWERT_PLATZEN = 975` matches March's Lua Enum `:9368` and June `@322862e0` (`push 0x3cf`).
- **73** (`@322862bf`).
  - It reads list `+0x67cc` on the same register the state-defence block below uses for `+0x9e4` (`m_DamageReducePercent`), then `HaveThisBuff( 0x3cf )` on the attacker, and writes DD`+0x541`.
  - The port reads `pDefender`. All 6 March callers call `GetFinalDamage` on the defender (the Raven overheat one sets `optrDefenderGameUnit = this`), so `pDefender == this`. Equivalent.
- **94 SetFactor** (`32e79e30`).
  - Restack erase when `DidStart` (`+0xc`) and `GetAccumulationCountNow` (`32369da0`) == 0.
  - Clear; `v[0]`/`v[1] != 0` into the two sets, keyed by the templet's BTI (`+0x34`); loop from index 2 (quirk 1); `InsertImmuneList`.
  - Erase helper `3227a910`: find first (`322a9090`), erase one. Matches.
- **94 stub** (`322a80a0`).
  - Null templet returns false before the list is read (`322a8138` → `322a8226`, `xor al, al`).
  - Then the all-buff set with `!IS_DEBUF` (`+0x64`), the all-debuff set with `IS_DEBUF`, and the list walk (`32369d70`, linear BTI match). Matches.
- **95** `DeleteNonControlDebuff` (`3229dcc0`).
  - Walks `+0xe14`, skipping null, `!IS_DEBUF` (`+0x64`) and `!IsNonControlDebuff` (`32369cc0`).
  - Erases through `3229d320` (`EraseBuffTempletFromGameUnitByIterator`) and keeps the returned iterator. The port's index walk is the same.
  - `IsNonControlDebuff` already has BBT 104 (Phase 4a, `X2BuffTemplet.cpp:1676`), so the Done note's "strips jump-impossible" holds.
- **104**.
  - User `31fe8479`: `m_fCanNotJumpTime` (`+0xde4`) `> 0 || list +0x42c0 non-empty` clears `[InputData+8]` as one dword. That dword is March's `pureUp`/`oneUp`/`pureDoubleUp`/`oneDoubleUp`.
  - NPC `@3276e442`: the message match falls through to the list test, and a non-empty list skips the state change without clearing the message. The port does the same.
- **105**.
  - The host (`322a0510`) sets `edi = this` at `322a051f` and calls `IsImmuneEDTByBuff( basicExtraDamageType )` on it (`322a05f5`) right after the rate test.
  - The reader (`322a0400`) walks `GetBuffFactor` (`3229fd80`) for type `0x69`, `ftol(value) == edt`.
  - March's `GetBuffFactor` skips finished and world buffs the same way, and fills values through the templet's `GetFactor`, which is `m_vecEDT`. Matches.
- **Store lifetime.** No store needs a bulk clear. March never clears its identity vectors (`m_vecDashImpossiblePtr`, `m_vecXImpossiblePtr`…) outside `DoFinish`; they die with the unit. The 6 new stores follow the same pattern.

**Open, for the user (a decision, low stakes).**
- **94 restack leak, verbatim June.** A restack whose accumulation count is above 0 pushes a **second** immune-list entry with the same identity, and `DoFinish` erases only one. The leftover entry keeps refusing its BTIs after the buff ends, until the unit is destroyed.
  - By quirk 1 the leftover entry also holds the count as a BTI: 1 = `BTI_DEBUFF_SILENCE` for an empty or one-entry list, 4 = `BTI_SI_SA_EMK_PHOENIX_TALON` for Bloodlust's four.
  - The two sets are keyed by BTI and are not affected.
- **Latent with June data.** June has five 94 factors on four templets: Bloodlust ×2, Sleep, Infinity Potential and Alptraum Ready. All four templets have `ACCUMULATION_LIMIT = 1` (`BattleFieldBuffTemplet.src.lua:23619`, `:24717`, `:26536`, `:33369`). The count only increments when the limit is above 1 (`X2BuffTemplet.cpp:229`), and it reads 0 otherwise (`:316`). So every June restack takes the erase-first branch, and the leak never fires.
- **Decided (user, 2026-10-04): keep June's behaviour.** No code change. The user made this a standing rule: June bugs and quirks are kept verbatim without asking (see "Shared checklist").

**New Phase 14 note: quirk 2 also hits the player's own unit.**
- A field↔dungeon move saves the unit's buffs through `GetBuffFactor`. That goes through `GetFactor`, which sends `ALL_BUFF = ALL_DEBUFF = 0` (`X2GageManager.cpp:3539`).
- The buffs are re-applied through `SetFactorFromPacket` (`SetKBuffFactorFromGageManagerToGameUnit`, `X2GameUnit.cpp:15555`).
- So a player-side 94 with `ALL_DEBUFF` (June: Infinity Potential) loses that immunity at every stage change for the rest of its duration. The `IMMUNE_BUFF_TEMPLATE_ID` list survives.
- This is verbatim June; the pre-sync join path (`X2Game.cpp:17840`/`17857`) behaves the same. Not changed.

**Phase 14 notes (carried).** The Done note's items 1–4 stand: the EDT renumbering, `DeleteNonControlDebuff` erasing inside a loop, empty level slices for 73/104, and 73's flag sticking on multi-target DamageData.

**Cost.** `/cost` cannot be read from inside the session. User, please paste it here.

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

**Kickoff:** `kickoffs/june-buff/7b.md` - start it with `/x2-phase june-buff 7b`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.


**Review:** `kickoffs/june-buff/7b-review.md` - start it with `/x2-phase june-buff 7b review`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

**Phase 4a hand-offs this group picks up** (stubs and slots Phase 4a added):
- **BBT 94:** `CX2GameUnit::IsImmuneBuffByBuffBehavior` (June `322a80a0`) returns false until Phase 7 lands, which equals June's result with no BBT 94 buff on.
- **BBT 95:** `CX2GameUnit::IsImmuneNonControlDebuffByBuffBehavior` (June `3227b790`), same stub rule. `IsNonControlDebuff` already has June's BBT 104 case.
- **BBT 89** (Phase 6): `CX2BuffChangeSkillDamageBehaviorTemplet` needs `OnAccumulationCountDecreased` (June behavior slot 7, `32e7c020`).
- Any type Phase 1 marked "Opus-only" moves to an Opus session, whichever group it is in. Under rule 9, a Phase 7 type may move to Sonnet only if Phase 1 marked it transcription-ready; Phases 5, 5b and 6 move to Opus if their reviews find errors.

### Phase 7b — Done (2026-10-04)

**Play-test (2026-10-05, user; exe `bc9099a7`, run 03:55:27 after the 21:58 deploy): all 5 checks passed by eye.** `ErrorLog.txt`: load baseline unchanged (`factors 361 loaded`, the same 5 ordinal rejects as before); `:174-175` 77 first apply and `forced invincibility renewed for 0.5 s`; `:176-177` 101 first apply and `UpNowHp refused a decrease of -4874.42`; `:219` 74 and `:220` 102 first apply (no Mana Break with either, baseline worked); `:240` 97 first apply (Chung gained a cannon ball); `:241` 110 first apply (log only; host awaits decision (a)). `:207-218` are a second paste of the setup lines after a character change: duplicate BTI / ordinal reject 362–367, harmless (the first definitions stay).

**Result.** Under `SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG` (Phase 7's flag; `Always.h` comment extended): all six types parse (June switch `32d4bbf0`: every one is `ParsingEmptyBehavior` `32d45b60`) and have their templet (factory `32e80450`: 74/77/101/102/110 = identity class `32e866b0`, 97 = own class `32e86c90`). Identity stores from `CreateAndInsertBuffIdentity` `3227ad60`: 74 `+0x67ec`, 77 `+0x67fc`, 101 `+0x680c`, 102 `+0x681c`, 110 `+0x682c` (erase `32279d70`). `buff_census.py --phase 7` lists nothing; 111/112 stay "not implemented in the June client either". New RE rows are in `june2017_re/funcmap.tsv` (17 rows, "Buff Phase 7b").

| Type | Hosts ported |
|---|---|
| 74 / 102 | June's split of `CX2GUUser::m_bNoDetonation` into `CX2GameUnit` bytes `+0x6890` (state: March's `SetNoDetonation`, kept for the 24 per-class sites), `+0x6891` (damage-react) and `+0x6892` (damage). Complete reader/writer sweep: user `DetonationEventProcess` (`31feb9f0`), `DamageReactStateChange` (`31feabd0`: reset block, `!= DT_BUFF` gate on the whole `IsDamagedReaction` branch, tail on the damage byte), `DamageReact` (`@32046314`), `CommonStateStart` (`@3205e9cd` carry block; `@320608b2` new state key **`CAN_MANA_BREAK`**, default TRUE, and damage byte = 0), `CommonStateEnd` (`@32015afe`), `RidingOffEnd` (`31fcde30`), ctor (`@322a5e37`); NPC `DamageReact` (`@327b1fd0`), `CommonStateStart` (`@327af365`), `CommonEventProcessNonHost` `CAN_DETONATION` (`@3276d79a`, non-host only) |
| 77 | Both `GetInvincible` (slot 96, `31fcee80` / `32758760`); `CommonFrameMove` tail (`@3206ce71`, 0.5 s renew). **The 12 direct reads:** an exhaustive `+0x7fe8` / `+0x8368` sweep finds no raw read in June outside the two getters, the INVINCIBLE time-table setters and `CommonStateStart`; every March host located in June calls the getter (user `DamageReact` ×2, user/NPC `CommonFrameMove`, Chung `DamageReact`) or NPC slot 95 (`3275bbf0`, both NPC `DamageReact` sites). All 12 now call `GetInvincible()`. The NPC `DamageReactStateChange`, `_CheckOnly` and `GetNearestUnitPosByOwnerUnit` sites were not read in June; the sweep settles them |
| 97 | Unit virtual `ChangeMaxCharacterAddResource` (slot 201, empty base `3208ddc0`), class SetFactor `32e75c70` (1.0 on first apply), Chung override `32bf4880` (June's gage layout = March's `CX2ChungGageData`). Add, Ara, Rose, Luciel: checklist §7 |
| 101 | `UpNowHp` / `SetNowHp` virtual on `CX2GameUnit`; user overrides (`3202fcf0` / `3202ff60`) refuse a decrease on a live unit, then call March's base. NPCs ignore 101, as in June |
| 110 | Store and `IsAwakeImpossible()` only. **Host deferred: decision (a) below** |

**Hygiene.** `flagcheck JUNE_BUFF_NEW_FLAG --worktree` on the 12 code files: `files: 9 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 27 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 27 are plain `#else` lines; every `#else` keeps March's line verbatim (the CP949 comment lines of `DamageReact` stay in place: the new code was inserted around them).

**Build and deploy.** `DEPLOY OK: X2_offline.exe is 19,110,400 B, dated 2026-10-04 21:58:42, md5 bc9099a703cbf19be3cf3beb3c632697, equal to X2/US_SERVICE/x2.exe (2026-10-04 21:58:38); built at mods/june-buff-port 3b3b839 + 12 uncommitted code file(s).` `CAN_MANA_BREAK`, `CAN_DETONATION`, `CHECK_NEVER_MOVE_TRUE`, `first apply BBT 97`, `first read BBT 77`, `first read BBT 101: UpNowHp` and `first apply BBT %d (%s): BTI %d` are in the exe. **Not launched.**

**Decisions for the user.**
- (a) **110's host.** June replaced March's `ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE` (56 uses, 6 inlined in Ara) with `TryHyperModeStateChange` (`31fe9a90`). Porting it whole needs two June systems March lacks: an awakening-lockout timer `+0x8d78` (set to -1 by the inits `31ffaad0` / `3203e850`, armed with a constant by `322f8920` when `+0x8d4c` is clear, counted down in `3206d770`, read by 13 per-class functions) and the flag `+0x8d4c` (set when awakening starts; 8+ writers). The class hooks 434/435 are Add-only (checklist §7). Options: **A** port the timer system with the function (new Phase 7c); **B** port the function with the timer treated as always ≤ 0 (a deviation: lockout never fires); **C** add only `!IsAwakeImpossible()` to March's macro and Ara's six copies (grafts onto a function June rewrote; the standing rule says not to). Until you choose, a 110 buff applies and logs but does not block awakening. **User chose A (2026-10-05): Phase 7c.**

**Notes for the review / Phase 14** (June kept verbatim).
1. A state that calls `SetNoDetonation( true )` now also empties the stored detonation charge (June zeroes it where March returned). All 13 March `true` sites run after `CommonStateStart`, so `CAN_MANA_BREAK` (default TRUE → 0) does not wipe them.
2. `CommonStateStart` clears the damage byte after its carry block, which makes the carry block vestigial (`JUNE_BUGS.md` #10).
3. A `DT_BUFF` hit no longer reaches `DamageFlushMp` / `FlushForceDown` (whole branch gated). The user `DamageReact` takes the hit's `NO_DETONATION` unconditionally (March's `SERV_9TH_NEW_CHARACTER` keep-on-DT_BUFF test is gone).
4. Not ported, non-buff: NPC slot 95's per-attacker invincible list (`+0x83f0`); the user HP overrides' death gates, `g_pData` vector clear, previous-HP store (gage `+0x108`) and low-HP UI (checklist §7). `CHECK_NEVER_MOVE_TRUE` (`+0xbb`, the condition next to `CAN_DETONATION`, both chains) **is** ported. No June script uses `CAN_DETONATION` or `CHECK_NEVER_MOVE_TRUE`.
5. Cost: `/cost` cannot be read from inside the session. User, please paste it here.

**How to see it in game (Phase 14; June data, needs Phase 13).**
- 74: `BFI_BUFF_ASSAULT_STRIKE` 468 → BTI 431 (skill buff): no detonation charge while it lasts.
- 77: `BFI_BUFF_ERDA_BLESSING` 507 → BTI 448, also `BFI_INVINCIBLE_BY_BOSS_SHIELD` 835: hits do nothing, the unit blinks.
- 97: `BFI_AMUSEMENT_ITEM_SUPER_RESET_COOLTIME` 837 → BTI 815 (item): Chung gains one cannon ball (outside berserk).
- 101: `BFI_BUFF_SOLESS_TITLE_INVINCIBLE` 938 → BTI 909 (title): the user's HP does not drop.
- 102: `BFI_DEBUFF_SURFACE_CUTING_UPGRADE` 987 → BTI 933 (damage effect): the stored detonation charge is emptied each frame.
- 110: `BFI_DEBUFF_VOID_FIELD` 475 → BTI 437 (damage effect): blocks awakening once decision (a) lands.

**Play-test hand-off** (dev console, END). **Fresh launch** (`factors 361 loaded`), then paste the six setup lines in order (ids 362–367 = BBT 74, 77, 97, 101, 102, 110; templets 990074 … 990110; 30 s each; checked from source, never run):
```
local t = {} local r = {} for i = 1, 30 do t[i] = 30 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990074, IS_WORLD_BUFF = FALSE, IS_DEBUF = TRUE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 74 }, BBT_DETONATION_IMPOSSIBLE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 362, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990074, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 74 }, BBT_DETONATION_IMPOSSIBLE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
local t = {} local r = {} for i = 1, 30 do t[i] = 30 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990077, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 77 }, BBT_INVINCIBLE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 363, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990077, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 77 }, BBT_INVINCIBLE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
local t = {} local r = {} for i = 1, 30 do t[i] = 30 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990097, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 97 }, BBT_CHANGE_MAX_CHARACTER_ADD_RESOURCE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 364, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990097, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 97 }, BBT_CHANGE_MAX_CHARACTER_ADD_RESOURCE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
local t = {} local r = {} for i = 1, 30 do t[i] = 30 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990101, IS_WORLD_BUFF = FALSE, IS_DEBUF = FALSE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 101 }, BBT_IGNORE_HP_DECREASE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 365, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990101, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 101 }, BBT_IGNORE_HP_DECREASE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
local t = {} local r = {} for i = 1, 30 do t[i] = 30 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990102, IS_WORLD_BUFF = FALSE, IS_DEBUF = TRUE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 102 }, BBT_DETONATION_IMPOSSIBLE_GAGE_RESET = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 366, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990102, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 102 }, BBT_DETONATION_IMPOSSIBLE_GAGE_RESET = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
local t = {} local r = {} for i = 1, 30 do t[i] = 30 r[i] = 1 end g_pBuffTempletManager:AddBuffTemplet_LUA( { ACCUMULATION_LIMIT = 1, BUFF_TEMPLET_ID = 990110, IS_WORLD_BUFF = FALSE, IS_DEBUF = TRUE, REGIST_TYPE = 0, BEHAVIOR = { COMBINATION = { 110 }, BBT_AWAKE_IMPOSSIBLE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1 } } } ) g_pBuffTempletManager:AddBuffFactor_LUA( { BUFF_FACTOR_ID = 367, BUFF_FACTOR = { BUFF_TEMPLET_ID = 990110, RATE = r, IGNORE_REGIST_FOR_RATE = TRUE, BEHAVIOR = { COMBINATION = { 110 }, BBT_AWAKE_IMPOSSIBLE = {} }, FINALIZER = { COMBINATION = { 1 }, BFT_TIME = { BUFF_DURATION_TIME_TYPE = 1, NORMAL_TIME = t } } } } )
```
Checks (wait 30 s between them): 1. 363 on self among monsters: no damage, blinking. 2. 365 on self: hits show numbers, HP bar does not drop. 3. Baseline Mana Break without buffs, then 362, then 366: no Mana Break while either is on. 4. As **Freils** (Chung, Deadly Chaser, Lv 72), empty the cannon balls, 364 on self: one ball back. 5. 367 on self: log line only (awakening still works: decision (a)).
Log lines read afterwards: `first apply BBT 74/77/97/101/102/110`, `first read BBT 77`, `first read BBT 101`.

### Phase 7b — Review (2026-10-05)

**Verdict: no transcription errors, no code change.** Reviewed the code commit `25f51ed`, plus the doc-only commits `c1f370a`, `01158f4` and `5e18b95` (the peer's 7c plan: docs, a kickoff and two RE helpers, no code). The deployed exe is still 7b's: `X2_offline.exe` md5 `bc9099a703cbf19be3cf3beb3c632697`, 19,110,400 B, 2026-10-04 21:58:42, re-read from the game directory. With no code change there was nothing to rebuild or deploy, and nothing new to play-test. Decision (a) is settled: the user chose A, which is Phase 7c.

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG --range 25f51ed^..25f51ed`:
- `files: 9 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
- `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 27 #else/#endif without the flag token, 0 encoding change(s)`
- `RESULT: PASS`

`file` reports the same encoding as the parent for all 12 code files; the `CRLF, LF` on the four headers was already there. The PASS~ lines are a blank line and the closing brace of `DetonationEventProcess` (now after the flag's `#endif`) in X2GUUser.cpp, and the `//{{`/`//}}` markers in X2GameUnit.cpp. I read every `#else`: each holds March's line verbatim, so the flag-off build is March.

**Checked against June** (read-only, with `june2017_re/disasm_range.py`, `funcdis.py` and `dispsweep.py`; Ghidra was not needed).
- **Detonation bytes: whole-`.text` sweep** (`dispsweep 6890 6891 6892`).
  - Outside the per-class code, every hit matches a port site:
    - the ctor (`@322a5e38`, a dword store that zeroes all three bytes);
    - `RidingOffEnd` and `CommonStateEnd` (`bl` = 0 after `xor bl, bl`), which clear both `+0x6890` and `+0x6892`;
    - the user `DamageReact` (`@32046314`), which takes DD`+0x4f8` unconditionally and sets `+0x6891` = 1;
    - user `CommonStateStart` (the carry block `@3205e9cd`, then `@320608b2`/`@320608ba`);
    - user `DamageReactStateChange` and `DetonationEventProcess`;
    - NPC `DamageReact` (`@327b1fd0`/`d6`), NPC `CommonStateStart` (`@327af366`–`87`), and the non-host chain.
  - The 48 per-class hits all write `+0x6890` only.
  - Phase 1c item 2 listed fewer writers than this; the port follows June, not 1c.
- **`DamageReactStateChange`** (`31feb837`–`31feb964`).
  - The reset block reads `+0x6890`, `+0x6892` and the 102 list `+0x681c`, then sets charge 0 and gage `+0xf6` = 0.
  - `IsDamagedReaction` (`3226de80`) is followed by `damageType != 6`, and that test gates **both** `DamageFlushMp` (gage slot 12) and `FlushForceDown` (`32a31e10`).
  - The tail writes `+0x6892` from DD`+0x4f8`. Matches.
- **User `CommonStateStart`.**
  - Between `@3205e9ef` and `@320608b2`, nothing reads or writes the bytes.
  - March's `m_LuaManager` is at the state table there, because the top-level `ENABLE_ATTACK_BOX` reads come just before it.
  - All 12 per-class `SetNoDetonation( true )` sites call `CommonStateStart()` first, so the merged position is safe (Done note 1 holds).
- **NPC conditions** (`3276cb20` `@3276d76c`).
  - The non-host order is `+0xba` → 74 list `+0x67ec` → 102 list → `+0x6890` → `+0x6892` → `+0xbb`/`+0x872b` → `m_iRate` (`+0x14`). That is the port's order, before the random draw.
  - The host chain `3276d980` tests `+0xb7`/`b8`/`b9`/`bb` and never `+0xba`.
  - March's NPC `CommonStateStart` has one early return (`m_pXSkinAnim`), and the carry block sits after it.
  - Not checked: whether June feeds the two keys into `m_bNPCNonHostSimulatable`. No June script uses either key, so this is dormant.
- **Keys.** `CAN_MANA_BREAK` (`@3206079f`), `CAN_DETONATION` (`@32794f62`) and `CHECK_NEVER_MOVE_TRUE` (`@327950be`) are status `ok` in `xorstrings.tsv`, and the port spells them the same. June's state scripts use `CAN_MANA_BREAK = FALSE` 32 times, so Phase 13 data will exercise it.
- **97.**
  - SetFactor `32e75c70`: `GetBehaviorFactor` failure returns false; `DidStart` (`+0xc`); unit `+0x324` with `fld1`.
  - Chung `32bf4880`: `ftol`, then slot 39 (berserk) skips. Otherwise it adds to the protected now-count at gage `+0x110`, sets `+0x142`, and calls `3231d7a0`, which clamps to max and to 0 like March's `SetNowCannonBallCount`.
  - Ara `32baafc0` (checklist §7): `+0x9644` is a protected int and the row's reading is right. Its "gage vslot 3" goes through unit slot 60 (`3208dd00`, forwards `(v, 1, 0)`), and the row now says so.
- **77 / 101 coverage.**
  - Every remaining March `stateParam.bInvincible` read and every `GetNoDetonation()` reader is in an `#else`.
  - March's direct gage HP writes (`X2GameUnit.cpp:12277/12894`, `X2GUUser.cpp:34166`, `X2GageManager`) rescale or sync max HP; none is damage that bypasses 101.

**Doc fix.** `JUNE_BUGS.md` #10 said `DamageReactStateChange`'s tail keeps the damage byte past the state change. It does not: `StateChange` only queues `DoStateEndStartFuture`, and `DoStateEndStart` (`OnFrameMove`, `X2GUUser.cpp:6802`) runs `CommonStateEnd` first, which clears the byte before the carry block reads it. The byte lives from the hit to the next state switch, which is shorter than March, where the carry worked. The row is corrected; the behaviour is June's and unchanged.

**Cost.** `/cost` cannot be read from inside the session. User, please paste it here.

### Phase 7c — BBT 110 host: TryHyperModeStateChange and June's awakening lockout (Opus)

**Why.** Phase 7b decision (a): the user chose option A (2026-10-05). Port June's awakening check whole, with the two June systems it reads.

**Scope.**
- **The function.** `CX2GUUser::TryHyperModeStateChange( int state )` (June slot 343, `31fe9a90`; Ain's copy `32a53150`), which replaces March's `ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE` (`X2GUUser.h:11`, 56 uses in 9 class files) and Ara's six hand-inlined copies (June's Ara sites `32bb2d40`, `32bb2ff0`, `32bb3290`, `32bb34d0`, `32bb37e0`, `32bb3ab0` call slot 345 inline: check each). Body, decoded in 7b: `!m_InputData.oneLCtr` → false; `IsAwakeImpossible()` (slot 345, `31fd4790`, ported in 7b) → false; class hook slot 434 true → protected short `+0x7e84` = `0x1c`, slot 435( 0 ), true; `!IsThereEffectiveBlackHoleDamageEffect()` (`322731a0`) `&& +0x8d78 <= 0 && CanHyperMode()` (slot 344, `31fd4730` = March's macro condition) → `+0x8d4c` = true, `StateChange( state )`, true; else false.
- **The lockout timer `+0x8d78` (float).** Set to -1 by `31ffaad0` and `3203e850` (inits); armed with the constant at `0x3344a51c` by `322f8920` (`@322faee8`, also `@322fb40b`) when `+0x8d4c` and `+0x8ac4` are clear; counted down per frame at `@3206fe04` (`fstart` puts it in `3206d770`: confirm the real host); read by `@32070d62` and 13 per-class functions (`329b5af0`, `32a20e70`, `32a53150`, `32a6dcd0`, `32ad7f80`, `32b3a310`, `32b3acf0`, `32b84a50`, `32bcfaa0`, `32c17830`, `32c58c30`, `32cac200`, `32cea0c0`, `32d1bd10`). **Not identified in 7b: what `322f8920` is and who calls it.** Do that first.
- **The flag `+0x8d4c` (byte).** Set by `TryHyperModeStateChange` and `31fe7610`; cleared by `31fe96c0` and `31fcc350`, `31fcc740`, `31fcc9d0`, `31fcce20`, `31fcfb70`, `31fcfde0` (and more: re-run the sweep). Read by `322f8920`.
- **Hooks 434/435.** Virtual, empty base (`31e4c550` false, `3208ddc0`); only Add overrides them (`329e2340`/`329e2360`): character checklist §7 row already written. Identify `+0x7e84` (protected short) before writing it.
- Attribute every per-class reader to a class and March function before deferring it (Shared checklist rule). Readers in classes March has whose host June rewrote go to checklist §7; the buff side (store, getter, function, timer) goes in regardless.
- If `322f8920` turns out to belong to a system March lacks entirely (a June-only mode or UI), stop, write what it needs here, and hand the decision to the user.

**Tools.** `june2017_re/dispsweep.py <hex offsets>` (every displacement hit, with function start) and `june2017_re/funcdis.py --func <va>` (uncarved functions), both from 7b.

**Flag.** `SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK` (new; requires `_NEW_FLAG`). March's macro and Ara's copies stay in `#else`.

**Done gate.** `TryHyperModeStateChange` and the timer and flag are ported with every June read/write site that has a March home; build rc=0; a `first read BBT 110` line (DEBUG) and the function's strings in the deployed exe; a play-test where 7b's factor 367 (BTI 990110) blocks Ctrl awakening with full beads, and awakening works again after 30 s. Per-type "how to see it": `BFI_DEBUFF_VOID_FIELD` 475 → BTI 437.

**Run with:** Opus 5.5, high effort. Review required (`kickoffs/june-buff/7c-review.md`, written by the phase). Touches `X2GUUser.cpp/.h` and every class file that uses the macro, so it runs alone among the `X2GUUser.cpp` phases (6b, 8, 9, 15, 16).

**Kickoff:** `kickoffs/june-buff/7c.md` - start it with `/x2-phase june-buff 7c`.

### Phase 7c — Done (2026-10-05)

**Play-test (2026-10-05, user; exe `3f4d41ba`, run 04:52 after the 04:47 deploy): all 4 checks passed by eye.** `ErrorLog.txt:204` `first apply BBT 110 (awake impossible): BTI 990110`; `:205` `first read BBT 110: TryHyperModeStateChange refused awakening (state 23)`; load baseline unchanged. Correction to the hand-off: test templet 990110 has no icon data, so there is no buff icon to watch; time the 30 s instead.

**The RE premise was wrong, and the record is corrected.** `322f8920` is `CX2Game::ProcessExpListByNpcUnitDie`. 7b's arm sites `@322faee8`/`@322fb40b` sit in a different function that the int3 range merged in: **`322fa960`, June's quick-slot item check**. It is June's form of the `SAT_SPECIAL_SKILL` branch in March's `CX2UIQuickSlot::KeyEventProcess` (same STR_ID_20120). When a throw/install item is accepted there, it sets `+0x8d78` = **10.0 s** (`3344a51c`). So `+0x8d78` is not an awakening cooldown. It is a two-way interlock:
- While a quick-slot special item is pending: no awakening (`TryHyperModeStateChange`, the skill-cancel and Ara paths) and no no-state-change skill (every class's slot 510 `NoStateChangeActionFrameMove`).
- While `+0x8d4c` (awakening started by a key) is set: no special item.

20 rows were added to `june2017_re/funcmap.tsv` ("Buff Phase 7c"). The 13 per-class `+0x8d78` readers are each class's slot 510 (Arme's is `32cebe80`), plus Ain's 343 copy and Luciel's slot 545.

**Landed** (`SERV_IRUHADEV_JUNE_BUFF_AWAKE_LOCK`, requires `_NEW_FLAG`). The 110 block does not depend on the timer.
- `CX2GUUser::TryHyperModeStateChange` (343, `31fe9a90`) in June's order: `oneLCtr`, `IsAwakeImpossible` (345, logs `first read BBT 110`), hook 434 → 435, BlackHole, timer ≤ 0, `CanHyperMode` (344, virtual for Add's override), then flag = 1 and `StateChange`.
- `ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE` now expands to `else if( TryHyperModeStateChange( s ) ) {}`, so every live March use goes through 343 (52 in 8 files). June's slot-343 call sites per class match March's live macro uses: User 5/5, Eve 7/7, Arme 6/6, Chung 7/7, EL 6/6, Elsword 6/6, Lire 10/10, Raven 5/5. Add's 5 are June-only (checklist §7).
- Ara's six inlined copies gain June's two gates: the item wait and `IsAwakeImpossible`. All six June sites (`32bb2d40`…`32bb3ab0`) are the same code; Ara's slot 430 equals March's ≥3-bead `ASI_BERSERK_MODE` choice.
- `SKILL_CANCEL_AFTER` / `IF_SKILL_CANCEL_AFTER` take June's expansion (`+0x8c90` = `m_fSkillCancelAfter`):
  - the skill branch also needs `IsOnSomethingFuture()` (`+0x7e1c` = `unitCondition.bFootOnLine`: Eve `@3296a4f6` tests it explicitly; CX2GUUser `31fcc350` reuses `dl` loaded `@31fcc443`, where `!bFootOnLine` already left via `USI_JUMP_DOWN`). **This changes play with no buff on:** a skill cancel now also requires being on the ground (June parity, all classes);
  - the awakening branch uses `CanHyperMode`, adds `!IsQuickSlotSpecialItemWaiting()`, and sets the flag.
- Members `m_fQuickSlotSpecialItemWaitTime` (`+0x8d78`) and `m_bHyperModeStartedByInput` (`+0x8d4c`), with June's homes:
  - ctor −1 / false;
  - `THROW_ITEM_StateEnd` −1;
  - `OnFrameMove` countdown (inside `IsLocalUnit`, after `PushFrameData`);
  - ~~`HyperModeEnd` / `RidingHyperModeEnd` clear the flag~~ **Review: June clears it in `HyperModeStart` (`31ffd490`) / `RidingHyperModeStart` (`31fe96c0`); moved there (see Phase 7c — Review).**
- **The timer stays at −1 until decision (b) below**, so its readers are inert for now.

**Not ported** (no March home, or waiting on (b)):
- June's `CommonEventProcess` common-cancel block (`USE_COMMON_CANCEL_AFTER`, `+0x8ca0`), a June-only system.
- The `+0x7e84` = 0x1c write in the Add-only hook branch (member not identified; checklist §7 Add row).
- The arm, the slot-510 prefixes and the flag's quick-slot reader: decision (b).

**Hygiene** (commit `e888197`; `--range e888197^..HEAD` gives the same lines). `flagcheck JUNE_BUFF_AWAKE_LOCK --worktree` on the 4 code files: `files: 2 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 9 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 9 are plain `#else` lines (3 macros, 6 Ara sites). The PASS~ is one blank line before the header block.

**Build and deploy.** `DEPLOY OK: X2_offline.exe is 19,111,424 B, dated 2026-10-05 04:47:15, md5 3f4d41ba0de832875ae4fc3984471dc7, equal to X2/US_SERVICE/x2.exe (2026-10-05 04:47:11); built at mods/june-buff-port 6f34e2d + 4 uncommitted code file(s).` The string `first read BBT 110: TryHyperModeStateChange refused awakening (state %d)` is in the exe. **Not launched.**

**JUNE_BUGS.md:**
- #11: the skill-cancel awakening ignores 110.
- #12: Ara's six sites never set `+0x8d4c` (latent until (b)).

**Decision for the user.**
- (b) **The quick-slot arm.** **Decided: A (user, 2026-10-05) → Phase 7d.** The only writer of the 10 s wait is inside June's rewrite of the quick-slot item check (`322fa960`). That rewrite adds gates March lacks (`+0x8ac4`, protected `+0x7cb0`, slot `0x524`, the Value1 0xd/0xe and Value2 4 cases) and is called from three places, one of them a June PvP mode. Options:
  - **A:** a new phase ports `322fa960` whole, with the slot-510 prefixes and the flag reader (quick-slot work, not buff work).
  - **B:** arm only, inside March's `KeyEventProcess` accept branch (a partial update; the standing rule needs your OK).
  - **C:** leave it out. Throw items then never lock awakening or skills, and awakening never locks items.

  Until you choose, 110 works fully and the interlock is off.
- Cost: `/cost` cannot be read from inside the session. User, please paste it here.

**How to see it in game (Phase 14).** 110: `BFI_DEBUFF_VOID_FIELD` 475 → BTI 437. Ctrl does nothing from wait/walk/dash while it lasts; inside a skill's cancel window it still awakens (#11).

**Play-test hand-off.** Use a fresh launch (`factors 361 loaded`). The ordinal rule means the next factor id must be 362, so the 110 setup line uses factor **362** (templet 990110, 30 s), not 7b's 367. Checks:
1. `jtLordKnight` (Elsword, Lord Knight): with 3 beads and 362 on, Ctrl in wait does nothing; 30 s later it awakens.
2. `arasss` (Ara, Yama Raja): with 3 beads and no buff, Ctrl gives Berserk Mode.
3. `arasss` again: with 362 on, Ctrl does nothing; 30 s later Berserk Mode.
4. `jtLordKnight`, no buff: a skill pressed inside a ground combo's cancel window still fires (regression check for the ground gate).

Log lines read afterwards: `first apply BBT 110` and `first read BBT 110: TryHyperModeStateChange refused awakening (state 23)`.

### Phase 7c — Review (2026-10-05)

**Regression play-test (2026-10-05, user; exe `46ef8895`, run 07:01:40 after the 05:22 deploy):** the user re-ran 7c check 1 and a plain Ctrl awakening. `ErrorLog.txt:187` `first apply BBT 110 (awake impossible): BTI 990110`; `:188` `first read BBT 110: TryHyperModeStateChange refused awakening (state 23)`. The load baseline is unchanged (`factors 361 loaded`, the same 5 ordinal rejects). The 30 s recovery and the plain awakening are eye checks. There is no exception in the run.

**Verdict: one transcription error, fixed under the same flag; everything else matches June.** June clears `+0x8d4c` when the hyper state **starts**, not when it ends. `31ffd490` is `CX2GUUser::HyperModeStart` and `31fe96c0` is `RidingHyperModeStart`. The proof is in Elsword's state init `32d1f8f0`: thunk `329caef0` (→ `31ffd490`) fills the StateStart delegate (`+0x28`) of state `0x17` `USI_HYPER_MODE`, and `329cb5d0` (→ `31fe96c0`) fills that of `0x2c` `USI_RIDING_HYPER_MODE`. The StateData base is fixed by `fPowerRate` = 1.0 at `+8`. `0x7c4` is `CommonStateStart` (funcmap row `3203c3a0`; `0x7dc` is `CommonStateEnd`). `31fe96c0` also has March's `m_ptrRidingPet->StateChange( wait )`. June's real `HyperModeEnd` (StateEnd `+0x58`, thunk `329cc5a0`) never writes the flag. Fix: the clear moved from `HyperModeEnd`/`RidingHyperModeEnd` to the end of `HyperModeStart`/`RidingHyperModeStart` (June writes it last, beside `+0x9176`); the header and `Always.h` comments were corrected. Every class's hyper and riding-hyper states (Ara's `ASI_BERSERK_MODE` included, `X2GUAra.cpp:517`) start through these two functions, so every setter is covered. **Effect:** the flag now means "awakening key pressed, hyper state not yet started on the Now side", which is the race guard June's quick-slot check reads. It is still inert until decision (b), so play does not change. `funcmap.tsv` rows `31fe96c0`/`31ffd490` were corrected and confirmed; rows were added for Ain's copy `32a599c0` and Add's `32a08150` (the other two zero-writes). `JUNE_BUGS.md` #12 was reworded to match.

**Hygiene.**
- Phase as committed, `flagcheck JUNE_BUFF_AWAKE_LOCK --range e888197^..HEAD --with JUNE_BUFF_DEBUG`:
  - `files: 2 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
  - `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 9 #else/#endif without the flag token, 0 encoding change(s)`;
  - `RESULT: PASS`.
  - The 9 are plain `#else` lines (3 macros, 6 Ara sites); each holds March's text verbatim, so flag-off = March. The PASS~ is one blank line in `X2GUUser.h`.
- Review fix: `flagcheck JUNE_BUFF_AWAKE_LOCK --worktree` on `X2GUUser.cpp`, `X2GUUser.h`, `Always.h`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; `12 deleted line(s)` (all 7c's own lines: the two End-side blocks and the two reworded comments), `3 #else/#endif without the flag token` (the macros' existing plain `#else`), `0 encoding change(s)`; `RESULT: PASS`. Re-preprocessed after the fix: `HyperModeStart` and `RidingHyperModeStart` each hold one `m_bHyperModeStartedByInput = false`, the two `End`s none. Post-commit `--range`: `--range 4a0589b^..4a0589b` gives the same lines (`RESULT: PASS`); `--range e888197^..HEAD` (phase + review, summed per revision pair) gives `files: 4 PASS, 1 PASS~, 2 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `12 deleted line(s)` (the review commit's removal of 7c's own lines; net against `e888197^` is insert-only), `12 #else/#endif without the flag token` (9 + 3, the same plain `#else` lines), `RESULT: PASS`.

**Checked against June** (Ghidra read-only, `dispsweep.py`, capstone over `.text`).
1. **Re-attribution holds.**
   - Between `322f8920` and the switch table `322fbc28` the only function boundary is `ret`/`int3` and an SEH prologue at `322fa960`, so `@322faee8` and `@322fb40b` are `322fa960`'s.
   - Callers: `322f8920` is called only from the two die handlers (`3230bb6b`, `32d53610`); `322fa960` is called directly from `329515d9`.
   - `3344a51c` = 10.0f.
   - The switch is `SAT − 16` → byte table `322fbc44`. SAT 18 lands on `322faa35`, which holds STR `0x4e98` (20120) and both `+0x8d78` writes, before case 34 at `322fb416`.
   - `XSLItem.h` gives `SAT_SPECIAL_SKILL` = 18. March's `X2UIQuickSlot.cpp:546` gates on the same 5 states.
   - (`CX2Game::UseItemSpecialAbility`'s case, `X2Game.cpp:10507`, is the later THROW/INSTALL state change, not the check.)
2. **`TryHyperModeStateChange`** `31fe9a90`, matches test for test:
   - `oneLCtr == 1`; slot 345 (`0x564`) → false; slot 434 (`0x6c8`) → `+0x7e84` write, slot 435(0), true;
   - BlackHole; `0 < timer` fails; slot 344 (`0x560`); flag = AL (1); `StateChange( state, 0 )`; true.
   - Leaving out `+0x7e84` holds: it is a protected short set to `0x1c` only when it differs, and the branch is reachable only through a 434 override. No March class has one (new name; grep is clean, and `CX2Eqip::CanHyperMode` is unrelated), so the branch is dead here.
3. **Macro sites.** Counted from preprocessed output (`cl -EP`, X2Lib US_SERVICE defines), not grep:
   - User 5, Eve 7, Arme 6, Chung 7, EL 6, Elsword 6, Lire 10, Raven 5 = **52**; Ara 0, Add 0.
   - The 66 textual uses are those 52, 6 commented out in Ara, and 8 in dead `#ifdef`s (Eve 7, User 1). The plan's "56 in 9" was a raw count.
   - June's slot-343 loads (`dispsweep 55c`), bucketed by 64K address range with class boundaries assumed from known per-class functions, are consistent with the same eight numbers (corroboration, not an independent count), and Ara's range has none.
   - One site per class was sampled: each does `push 0x17` (User `0x2c`, Eve a register) and calls `[vt+0x55c]`.
4. **Cancel expansion** (`31fcc350` `@31fcc56b-64f`, Eve `3296a1f0` `@3296a4cb-5ac`, Eve `32972b80` `@32972cc8`): `time ≥ 0`, `anim > time`, foot byte == 1, then SAEP(`push 1` = `ASUT_GROUND`, March's default).
   - The foot test is in the macro, not in Eve's code. Whole binary: of 1000 `+0x8d4c` set sites, the 954 that follow an SAEP call (`[vt+0x7f4]`) all test a byte == 1 right before it: 717 as `[+0x7e1c]`, the rest through a register loaded from `+0x7e1c` (checked `@31fcc443`, `@32972b85`, `@3296cab4`).
   - `+0x7e1c` is Future `bFootOnLine`: `@31fcc443` is March's `if( false == IsOnSomethingFuture() ) { StateChange( USI_JUMP_DOWN ); position.y -= … }` (`X2GUUser.cpp:25115`).
   - The awakening branch never calls slot 345 (#11 holds).
   - Flag polarity: EBX = 1 from `@31fcc40a`, so these sites **set** `+0x8d4c`. 7b's "cleared by `31fcc350`…" list was wrong. The sweep shows ~1000 sets (`bl` = 1 or imm 1; the ctor's `bl` = 0), 2 `al` (343 and Ain's copy) and only 4 clears (the two Starts, Add `32a08150`, Ain `32a599c0`).
5. **Ara** `32bb3ab0` `@32bb3d0c-81`:
   - BlackHole, `oneLCtr`, count > 0, (remain ≤ 0 || `+0x8fc3`), `!32070d60`, `!slot 345`, `StateChange( slot 430 )`;
   - no flag write. Matches, including #12.
6. **Homes.** All four were confirmed live in the preprocessed `X2GUUser.i`:
   - ctor `@32042d98` −1 and `@32042e47` `bl` = 0;
   - `THROW_ITEM_StateEnd` `@31ffb0eb` −1, then tail-jump `CommonStateEnd`;
   - countdown `@3206fe04`: `if timer > 0: timer -= [ebp+0x10]` (fElapsedTime), right after `PushFrameData`;
   - the clear: see the verdict.
7. **Flag off = March:** the three macros are whole `#ifdef/#else` copies and every `#else` is March verbatim.

**Build and deploy.** `DEPLOY OK: X2_offline.exe is 19,111,424 B, dated 2026-10-05 05:22:36, md5 46ef8895befd8fbacd846a97ebf668c6, equal to X2/US_SERVICE/x2.exe (2026-10-05 05:22:32); built at mods/june-buff-port e52bb0b + 3 uncommitted code file(s).` Not launched. Play-test is **optional**: the flag has no reader until (b), but `HyperModeStart` now runs the moved line on every awakening, so a quick regression check on `46ef8895` is offered (7c's check 1 plus a plain Ctrl awakening).

**For the user.**
- Decision (b) still stands. The interlock window is short: from the awakening key until the Now-side StateStart, not the whole awakening. That makes option B's arm cheaper to reason about, but it is still your call.
- `/cost`: please paste it here.

### Phase 7d — June's quick-slot item check and the special-item interlock (Opus)

**Why.** Phase 7c decision (b): the user chose option A (2026-10-05). Port June's quick-slot item check whole, as its own phase. That gives the quick-slot special-item wait `+0x8d78` and the awakening-by-key flag `+0x8d4c` their June writer and readers; 7c ported both and they are inert so far.

**What is already known** (7c, 7c review; `june2017_re/funcmap.tsv` "Buff Phase 7c" rows):
- **`322fa960`** (CX2Game, no March function): args (itemTemplet, user, bool* pAllow, ?, ?), ret 0x14. It walks the templet's special abilities (`328673c0`/`328673e0`, 24-byte rows) and switches on SAT 16/18/34/38/39/43 (`SAT − 16` → byte table `322fbc44` → jump table `322fbc28`).
  - Case 18 `SAT_SPECIAL_SKILL` (`322faa35`) is June's form of the `SAT_SPECIAL_SKILL` branch in March's `CX2UIQuickSlot::KeyEventProcess` (`X2UIQuickSlot.cpp:546`). It uses the same STR_ID_20120 (`0x4e98`) and the same wait/walk/dash/chargeMP/dashEnd states.
  - On accept, it sets `*pAllow` = 1, game `+0x3c5` (`EnableAllKeyProcess`) = 0, and user `+0x8d78` = 10.0 s (`3344a51c`) at `@322faee8` and `@322fb40b`.
  - June adds gates March lacks: `+0x8ac4` == 0, protected `+0x7cb0`→`+0xc` == 0, slot `0x524` == 0, and `+0x8d4c` == 0 (`@322faec8`/`@322fb3e8`). It also adds the Value1 `0xd`/`0xe` and Value2 `4` cases.
- **Callers:**
  - `32951420`, June's quick-slot use: item templet `+0xc0`, HaveThisBuff gates `0x32d`/`0x324`/`0x13f`/`0x34`, returns the allow byte; direct call `@329515d9`;
  - `32024510`, a CX2GUUser virtual slot;
  - `3200edb0`, a PvP-winner range.
- **Readers of `+0x8d78`:** every class's slot 510 `NoStateChangeActionFrameMove` starts with "if `+0x8d78` > 0, skip":
  - March classes: Elsword `32d1bd10`, Eve `329b5af0`, Arme `32cebe80`, EL `32b84a50`, Ara `32bcfaa0`, Chung `32c17830`, Raven `32c58c30`, Lire `32cac200`, Add `32a20e70`;
  - June-only: Ain `32a6dcd0`, Rose `32ad7f80`, Luciel `32b3a310` (+ slot 545 `32b3acf0`).
  - `TryHyperModeStateChange`, the cancel macros and Ara's six sites already read it (7c).
- **`+0x8d4c`** is set by an awakening key and cleared in `HyperModeStart`/`RidingHyperModeStart` (7c review). So the item check refuses an item only between the key and the Now-side state start.

**Scope.**
1. **Identify first** (read-only Ghidra, `funcdis.py`/`dispsweep.py`), and record each finding in `funcmap.tsv`:
   - the remaining cases of `322fa960` (SAT 16, 34, 38, 39, 43; Value1 `0xd`/`0xe`; Value2 `4`), against March's `KeyEventProcess` loop and `CX2Game::UseItemSpecialAbility` (`X2Game.cpp`, the `SAT_SPECIAL_SKILL` case at ~10507);
   - the June gates `+0x8ac4`, `+0x7cb0`→`+0xc` and slot `0x524`;
   - `32951420`, `32024510` and `3200edb0`, and which March function each one replaces.
2. **Port:**
   - `322fa960` as a `CX2Game` member, with every case that has a March home;
   - route March's `KeyEventProcess` special-ability loop through it, the way June's quick-slot use does;
   - the slot-510 `+0x8d78` prefix in the eight March classes' `NoStateChangeActionFrameMove`. Add's row goes to checklist §7 (Add is ported from June).
   - March's code stays in `#else`.
3. **Stop and hand back** if a gate or caller belongs to a system March lacks entirely (the PvP-winner range, a June-only member): write what it needs here and ask. Never invent a gate's value. Port June quirks verbatim, each with a `JUNE_BUGS.md` row.

**Flag.** `SERV_IRUHADEV_JUNE_BUFF_QUICKSLOT_ITEM` (new; requires `_AWAKE_LOCK`).

**Done gate.**
- Build rc=0, the new strings in the deployed exe, and a DEBUG `first arm quick-slot special item wait` line.
- A play-test: a throw/install item used from the quick slot blocks Ctrl awakening and slot-510 skills until the throw ends (at most 10 s).
- Regression checks: ordinary quick-slot items (potions) still work, and a throw item used outside the allowed states still shows STR_ID_20120.

**Run with:** Opus 5.5, high effort. Review required (`kickoffs/june-buff/7d-review.md`, written by the phase). It touches `X2UIQuickSlot.cpp`, `X2Game.cpp/.h`, `X2GUUser.cpp/.h` and the eight class files' `NoStateChangeActionFrameMove`, so it runs alone among the `X2GUUser.cpp` phases (8, 9, 15, 16).

**Kickoff:** `kickoffs/june-buff/7d.md` - start it with `/x2-phase june-buff 7d`.

### Phase 7d — Done (2026-10-05)

**Play-test (2026-10-05, user; exe `28343b61`, run 07:56:47 after the 07:46 deploy): all 3 checks passed by eye.** `ErrorLog.txt:176` `first arm quick-slot special item wait: item 78390, 10 s`; no `armed before the start` line; load baseline unchanged (`factors 361 loaded`, the same 5 ordinal rejects); no exception in the run.

**Landed** (`SERV_IRUHADEV_JUNE_BUFF_QUICKSLOT_ITEM`, requires `_AWAKE_LOCK`). Evidence is in the "Buff Phase 7d" rows of `june2017_re/funcmap.tsv`: 15 new, 4 corrected.
- `CX2Game::CheckQuickSlotItemSpecialAbility` = `322fa960`, case for case:
  - 16 pepper: riding, plus June's Future-state-skill refusal (STR 813);
  - 18 special skill: Value1 13/14 dungeon-only except item 100005189 (STR 20120); Value2 4 allowed with no wait; `bLandConnect`/`EnableAllKeyProcess`; the pre-start branch; riding (STR 24528); the six states; the four gates. The arm sets the ACK wait, keys off and **`+0x8d78` = 10 s**;
  - 34 summon card, 39 riding stamina (STR 43633, which has no March STR_ID, so it shows empty), default → ACK wait.
- `CX2UIQuickSlot::KeyEventProcess` follows June's `32951670`/`32951420`/`32950d60`: the pet GM check moves after the cooldown, the ability loop is that call, and a refusal clears the ACK wait (`@329517d0`).
- The **"no skill while the item waits"** prefix went at the top of `SpecialAttackEventProcess` in all 8 March classes.
- New: `CX2GUUser::GetSpecialItemUsableExtraStateID()` (vslot `0x4ac`, base = wait) and `GetSpecialAttackEventProcessedAtThisFrame()` (`+0x8ac4`).

**Corrections to earlier records.**
1. `322fa960` has one caller. Raw scan: `329515d9` only. 7c's `32024510`/`3200edb0` call `322fbc60` = `UseItemSpecialAbility`, so the PvP-winner stop condition does not apply.
2. Its accept sets the **ACK wait** (4th arg), not `*pAllow`.
3. **Slot 510 is not `NoStateChangeActionFrameMove`.** It returns bool, calls `31fcadd0` (= `CommonSpecialAttackEventProcess`), and is called first by slot 509 `SpecialAttackEventProcess` (Elsword `32cf3290`). That is why the prefix went where it did.
4. June's state 21 = `XS_BATTLE_FIELD`: every `CX2StateBattleField` call `324bbd80` is gated on state `0x15`.
5. `+0x8ac4` = `m_bSpecialAttackEventProcessedAtThisFrame`; `+0x8b61` = `m_bStarted`.

**Scope widened under the no-partial-updates rule.** June's whole quick-slot use was compared, not only `322fa960`. These parts are June-only systems, recorded and not ported:
- the PvP item bar: quick slot `+0x101`, PvP room `+0xf0 == 2`, keys `0x2f`/`0x30`, `329515f0`, and the PvP buff gates in `32951420`;
- SAT 38 (Erda Island, GT 4; its flag is off in March) and SAT 43 (past `SAT_END`);
- the GT 4 riding refusal;
- event items 291687 (BTI 931) and 100003917;
- June's pet-item check `3230fe70`;
- the item-group `0x67` gate;
- the in-house cooldown bypass;
- `322f5140`'s pre-start allow paths (dungeon 38100, the `+0x92cc` start timer).

**June quirks.** `JUNE_BUGS.md` #13 is new: an arm before the dungeon start, then a refusal, leaves the keys off. It is latent because units sit in `USI_START` until `m_bStarted`, and a DEBUG line marks it if it ever fires. #12 (Ara) is now reached.

**Character port (§7):**
- the vslot 510 prefix for Add/Ain/Rose/Luciel;
- vslot `0x4ac` for Elsword `0xce` and EL `0xbb`, which are June-only states. March's EL `0xbb` is a skill state, so the number must not be copied.

**Hygiene.** `flagcheck JUNE_BUFF_QUICKSLOT_ITEM --worktree --with JUNE_BUFF_DEBUG` on the 13 code files:
- `files: 12 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
- `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)`;
- `RESULT: PASS`.

The 2 are plain `#else` lines in `X2UIQuickSlot.cpp`; each holds March's text verbatim. Post-commit, `--range 50d4e32^..50d4e32` gives the same three lines (`RESULT: PASS`).

**Build and deploy.** `DEPLOY OK: X2_offline.exe is 19,113,472 B, dated 2026-10-05 07:46:16, md5 28343b611d94ec3a55f33bf687f20c95, equal to X2/US_SERVICE/x2.exe (2026-10-05 07:46:12); built at mods/june-buff-port f46ef86 + 13 uncommitted code file(s).` Both new DEBUG strings are in the exe, and the new code produced no compiler warnings. **Not launched.**

**How to see it in game (Phase 14).** Use a throw orb from the quick slot, e.g. 78390 Wind Orbs. `ErrorLog.txt` then gets `first arm quick-slot special item wait: item 78390, 10 s`, and no skill or Ctrl awakening fires until the throw ends.

**Correction to the kickoff's gate.** A throw item used mid-attack is refused **silently** in both March and June. STR 20120 appears only for Value1 13/14 outside a dungeon (no such item in this build's `item.kim`) and for a summon card outside a dungeon.

**`JUNE_BUGS.md` #14 (reached, kept verbatim).** Only the throw state's end resets the wait. An install item (77220) or a direct-use item (e.g. 90000703) therefore locks skills and Ctrl for the full 10 s.

**What a player can see.** March's throw state does not handle Ctrl either. Its only skill path is `CommonEventProcess`'s "anytime" skills. So "Ctrl does nothing during the throw" looks the same with the flag off, and **the log line is the proof of the arm**. The visible checks below are regressions: the lock must end with the throw, potions must be unaffected, and a refusal must change nothing.

**Play-test hand-off.** The dev console is not used, because quick-slot use is a key press and the console cannot press keys. Character: **reyaa** (Rena, Grand Archer, Lv 80). Quick slot 3 holds Complete Recovery Potion 130165; slot 5 holds 83 Wind Orbs 78390, a throw item. Play in a field, or in a dungeon after the start motion ends; before that, the item is refused by design.
1. Stand still and press **5**. The orb is thrown and the count drops to 82. Press a skill key right after the throw animation ends: it fires at once, with no 10 s delay.
2. Take a hit, then press **3**. HP and MP refill as before.
3. Start a combo (Z Z Z) and press **5** during it. Nothing happens, there is no chat message and the count does not change. Then press 5 while standing: it throws.

Log check (`python tools/logs.py --tag JUNE_BUFF`): `first arm quick-slot special item wait: item 78390, 10 s` must appear, and `armed before the start and then refused` must **not**.

**Open for the user:** none blocking. `/cost`: please paste it here.

### Phase 7d — Review (2026-10-05)

**Verdict: no code change (exe stays `28343b61`). One finding needs a decision (c), and a smaller one goes with it; everything else matches June.** Evidence rows are in `june2017_re/funcmap.tsv` ("7d review").

**Finding 1, decision (c): before the start, the port refuses where June allows, and that refusal locks skills for 10 s.**
- `322f1f90` is not June-only. It is "the now stage has a start motion": `m_pDungeon` (`+0xc44`) → `m_pNowStage` (`+4`) → `m_pStageData` (`+0`) → byte 0 = `StageData::m_bStartMotion` (`X2DungeonStage.h:29`), or 1 if any pointer is null.
- `+0x92cc` is June's start-motion delay. `CX2DungeonGame::StageLoad` `327cd1b0` runs March's loop (`X2DungeonGame.cpp:814-848`), but a start-motion stage stores the stage's delay there (`@327cdb01`), and a stage without one changes to the wait state and stores −1 (`@327cdb46`). Nothing turns −1 back into ≥ 0 that March can reach: `322f11d0` zeroes only values other than −1 (`@322f1259-64`), and `31d87420` only in dungeon 41470, which March does not have. **So on a no-start-motion stage `322f5140` is true: June arms, allows and uses the item.** The port arms and then refuses: the item is not used, and skills and Ctrl awakening stay locked for the full 10 s (only the throw state's end resets the wait, #14). The keys-off itself lasts one frame, because `CommonFrameMove` turns keys back on every frame unless the unit is frozen or stunned (June `32063650` @32067393-3ff, March `X2GUUser.cpp:16326-16398`). So #13's "frozen until `ReInit` or death" was wrong in June too; the row is corrected.
- **Reachable in March data.** `m_bStarted` has one writer, `StartEnd` (`X2GUUser.cpp:26210`). In March's dungeon scripts (`es-server-mar/ClientScript/Dungeon`, first stage block) only 15 dungeons start without a start motion: the 8 `Dungeon_Battle_Field_Tutorial_*` (39600+, the tutorial after character creation, `XS_DUNGEON_GAME`), the 6 `Dungeon_ElTutorial_*` and `Dungeon_Training_Free` (`XS_TRAINING_GAME`, where the quick slot is off). Repro: in the post-creation tutorial, stand still and press a throw item in the quick slot. Nothing is thrown, and skills do nothing for 10 s. The 7d play-test ran in a field.
- SAT 34 (summon card) asks the same `322f5140( false )`. June allows it there, with no ACK wait.
- Dungeon 38100 **is** in March: `DI_ELDER_HENIR_SPACE` (`DungeonEnum.h:351`), which corrects 7d. June's party path (party `+0x38 == 1`, a candidate for `m_iDungeonMode == DM_HENIR_CHALLENGE`) changes nothing reachable: Henir's first stage has a start motion, and a unit in `USI_START` never arms.
- **Options:**
  - **A (recommended):** port `322f5140` the way March can express it: allowed when the now stage's `m_bStartMotion` is false. That stage always has June's `+0x92cc` < 0, as shown above. Leave out the Henir path, since it is unreachable. Apply it to SAT 18 and SAT 34. Flag off stays March. #13 goes back to latent, because only start-motion stages refuse and their units sit in `USI_START`.
  - **B:** also port the `+0x92cc` member, its writes and the countdown (`@32070053`, `31fcf040` at 0). March has no start delay (`START_MOTION_DELAYTIME` is a June key), so this adds nothing over A.
  - **C:** keep the refusal but skip the arm before the start. Not June; the smallest change.
  - **D:** keep it as it is, and record #13 as reachable.

**Finding 2 (goes with (c)): SAT 38 has a March home.** `SAT_RETURN_TO_BASE_AT_PVE` = 38 is in March's enum unconditionally (`X2Item_Preprocessing.inl`), and `STR_ID_30375` exists. June's case (`@322fb7d8`) refuses it, with STR 30375 as info text, whenever the game type is not 4, and March has no type 4. The port's `default` sets the ACK wait and sends the request instead, which is March's behaviour. Not checked: whether any March item carries SAT 38 (`item.kim` is binary). Recommended: port that refusal together with A.

**Also for the follow-up:** `Always.h`'s flag comment still says "NoStateChangeActionFrameMove (slot 510)" where the reader is each class's `SpecialAttackEventProcess`. The `X2GUUser.h` comment is right.

**Kickoff checks.**
1. **One caller.** A raw `e8`/`e9` scan of `.text` finds `322fa960` ← `329515d9` only, and no absolute reference anywhere in the file. `322fbc60` ← `3200ee66`, `32024aec`, `323049ca`, `3294e76c`.
2. **Argument roles.** `@329515a7-d9` pushes: `&slot+0x110`, `32951420`'s pWaitingAck, `&allow`, user, templet. The allow byte is only ever cleared, except Value2 4 sets it (`@322fad63` → `@322fbbe5`). The ACK wait is set on both accepts and in `default`.
3. **Slot 509.** Eve `32969300`, Chung `32bd8790`, EL `32b4ee20` and Ara `32b90c20` have Elsword's shape: they call `[vt+0x7f8]` (510) first. Eve's, Chung's and Ara's 510 open with `fldz; fcomp [+0x8d78]` → `jnp` to `return false` (Ara: `32bcfcbe xor al, al`) before anything else. Ara's `CanUseSpecialAttack` and `FIX_SKILL_CANCEL_ARA` block therefore sit after the test, so the early return skips nothing June runs.
4. **Cases.** 16, 18, 34, 39 and `default` match June test for test. SAT numbers were checked against `X2Item_Preprocessing.inl`; `GT_DUNGEON` = 2.
   - **Future/Now settled:** StageLoad `@327cdacc` reads `+0x7cbc`'s state index == 4 (`GUSI_DIE`), which is March's `GetNowStateID() != GUSI_DIE` (`:822`). So `+0x7cbc` is Now, `+0x7cb0` is Future, and `3208e410` is confirmed. Only SAT 16 depends on this.
   - STR 43633 is past March's table (highest STR_ID 30534); `GetGeneralString` returns empty, and release `ASSERT` is empty (`KTDX.h:855`).
5. **Left out.**
   - SAT 43 (past `SAT_END` = 40), game type 4, the PvP bar, BTI 931, June's pet manager and the cooldown bypass are June-only, as 7d said.
   - SAT 38 and the pre-start paths: see the two findings.
   - Item 100003917 and the group `0x67` gate were not re-checked.
6. **KeyEventProcess.** `bl` = 1 (`@3295168e`); `call 32951420; cmp al, bl; jne 329517d0` → `mov byte [ebp], 0` with ebp = `&+0xe4` (`@32951688`). The order cooldown → `32950d60` → no-game check (`@329514c5`) → `322fa960` matches the port, and the `#else` is March.

**Hygiene.** `flagcheck JUNE_BUFF_QUICKSLOT_ITEM --range 50d4e32^..HEAD --with JUNE_BUFF_DEBUG`:
- `files: 12 PASS, 0 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`;
- `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 2 #else/#endif without the flag token, 0 encoding change(s)`;
- `RESULT: PASS`.

The 2 are the plain `#else` lines at `X2UIQuickSlot.cpp:473` and `:570`, each holding March verbatim. No build: nothing changed.

**For the user.** Decision (c): A, B, C or D, and whether SAT 38's refusal goes with it. `kickoffs/june-buff/7e.md` is written for A plus SAT 38. `/cost`: please paste it here.

**Decision (c) = B (user, 2026-10-05):** port June's start-motion delay timer `+0x92cc` with `322f5140`. Read before rewriting the 7e kickoff:
- `StageData::LoadData` `327f4e30` reads `START_MOTION` (byte `+0`, default false) and `START_MOTION_DELAYTIME` (float `+0xac`, **default 0**). So `ebp` at StageLoad `@327cdafb` is the StageData.
- The countdown in `CommonFrameMove`'s tail (`@32070053-e2`) runs as follows:
  - While the timer is ≥ 0, it subtracts fElapsedTime.
  - When the result drops below 0, it calls `31fcf040`. That function does `StateChange( 0x48 )` (riding start) or `StateChange( 9 )` (`USI_START`).
  - Otherwise, if the Now state is 9 or `0x48`, it stores −1.
- **On March data no stage has the delay key, so a start-motion stage starts one frame after StageLoad instead of inside it.** Nothing else visible changes.
- Still open for 7e:
  - the constructor value (`@32041c8b`; the x87 stack suggests −1);
  - the `== −1` getter `3230f110` (callers `322f803e`, `32903cad`);
  - `3200bc20`'s `322f5140( true )` call (callers `3252bfb0`, `32eed115`);
  - `322f11d0`'s triggers: `322f4c60` (solo) and `324cf360` (party event `0x4ac`).
- SAT 38: **yes** (user, 2026-10-05): 7e ports June's refusal.

### Phase 7e — Done (2026-10-05)

**Play-test (2026-10-05, user; exe `fa090292`, run 09:46:47 after the 09:37 deploy): all 4 checks passed by eye.** `ErrorLog.txt:560` `first pre-start quick-slot allow (322f5140): item 78390, now stage start motion 0, start-motion delay -1` (the tutorial really is before the start, so the allow path ran); `:1032` `first start-motion delay end (31fcf040): ... riding 0`; `:1033` `first arm quick-slot special item wait: item 78390, 10 s`; no `armed before the start` line; load baseline unchanged (`factors 361 loaded`, the same 5 ordinal rejects); 0 exceptions (3 UNHANDLED `E_DISABLE_HB_CHECK_REQ`, unrelated).

**Landed** in two commits. The RE is in the "Buff Phase 7e" rows of `june2017_re/funcmap.tsv` (15 new, 6 corrected).
- **`f20a31c`, `SERV_IRUHADEV_JUNE_BUFF_START_DELAY`:** June's start-motion delay, CX2GUUser `+0x92cc`.
  - The ctor sets −1.0f (`@32041c8b`; the x87 stack holds `334380f0`).
  - `StageData` reads `START_MOTION_DELAYTIME` right after `START_MOTION`, default 0 (`327f4e30`).
  - `StageLoad` (`327cd1b0`): on a start-motion stage it stores the delay and does not change state. On any other stage it sets the (riding) wait state and −1.
  - The countdown sits at the end of `CX2GUUser::OnFrameMove` (`3206eaf0 @32070053-e2`). Below 0 it calls `StartMotionStateChange` (`31fcf040`: riding start if `GetUnit()->GetRidingPetInfo()`, else `USI_START`, bDirectChange false). Once the Now state is a start state, it stores −1.
  - Readers with a March home, via `3230f110` (== −1): the riding key in `CX2Game::KeyProcess` and the pet window's Ride button (`329031f0` case 44019). Each refuses with its existing 24526/24527 text.
- **Same commit, `SERV_IRUHADEV_JUNE_BUFF_QUICKSLOT_ITEM`:**
  - `322f1f90` → `CX2Game::IsNowStageStartMotion`, `322f5140` → `CanUseQuickSlotItemBeforeStart`.
  - SAT 18 before the start: arm, then allow on true (`@322faefb-57`). A failed state check or gate refuses either way.
  - SAT 34 before the start: allow on true with no ACK wait (`@322fb5a2`).
  - SAT 38 (`@322fb7d7`): the pre-start test, then any game type but 4 refuses with STR 30375 as info text. March has no type 4, so it always refuses.
  - The Henir path is ported: party `+0x38` is `m_iDungeonMode`, and `== 1` is **`DM_HENIR_PRACTICE`**, not CHALLENGE as the 7d review guessed. The writer `32563a8c` maps DT_HENIR → 1, as `X2LocalMapUI.cpp` does.
- **`726607a`, `SERV_IRUHADEV_JUNE_BUFF_RIDING_KEY_WAIT`:** June's riding-key skill wait `+0x92d0`, found in the riding block this phase edits.
  - The ctor sets −1. The riding key sets it to 0.1 s after its summon/unsummon request (`@322f80a8`). The countdown runs right after `+0x92cc`'s.
  - The reader `329cc370` (≠ −1 → return false) sits right after `CheckSkillUsePVPOfficial` in all 8 March classes: in `CanUseSpecialAttack` for Ara and Elesis, which hold that check there, and in `SpecialAttackEventProcess` for the rest.
  - Add/Ain/Rose/Luciel get it with their port (character checklist §7, new row).
- `Always.h`'s 7d comment and `X2GUUser.h`'s `+0x8d78` comment now name `SpecialAttackEventProcess`, as the 7d review asked.

**Corrections to earlier records.**
1. The countdown is in `OnFrameMove`'s tail (ret 0xc, fElapsedTime `[ebp+0x10]`), not `CommonFrameMove`. `funcdis` merges `3206d770` with `3206eaf0`, so funcmap row "3206d770 slot 484" was mis-bounded the same way.
2. `@3200c113` is in `3200c030`, not `3200bc20`. `3200bc20` is a PlagHeadMark effect refresh. `3200c030` is June's HP/MP auto-potion gate, which goes through `g_pData+0x328` → `329515f0` (7d's June-only item bar).

**Step 1 answers (not ported, no March home).**
- `322f11d0`'s triggers are June's skip-the-start system:
  - The solo trigger `322f4c60` comes from a base `CX2State` key handler (`31f1e1a0`).
  - The party trigger is event `0x4ac`'s answer, `324cf360`.
  - Both act on a game `+0xb40` struct that only the NPC init (`327af325`, NPC flag bit `0x2000`) arms. Dungeon 41470's `31d87420` has no March DUNGEON_ID.
- `31fcf040`'s other two tests are June-only: dungeon-data bitset bit 8 (`327d9df0`; March's `DungeonData` has no such set) and `32932230` (dungeon IDs ≥ 100000; March's highest is 41311).
- In the blocks 7e edits, June also has:
  - `+0x89e0 = 0` in StageLoad (June's class-mode slot, `31fe0bb0`, June-only per Phase 4b-ii);
  - the riding key's `g_pMain+0x16a` gate (setter `31d716c0`, no matching March `CX2Main` member);
  - the Ride button's UID recompute over June's sorted riding lists (`328f3900`);
  - two more skill-ban checks before the riding-wait reader (`3202d140`, `3202d3c0`; character-port territory).

  All are recorded in funcmap and not ported.

**Not checked:** no March dungeon script sets `START_MOTION_DELAYTIME`, so with March data a start-motion stage starts one frame after StageLoad instead of inside it. Nothing between the two reads the start state: `StageStart` only repositions, fades in when there is no start motion, and sets the camera.

**`JUNE_BUGS.md` #13** is latent again: on a no-start-motion stage `322f5140` is now true, so an armed item is used.

**Hygiene** (`flagcheck --worktree`, before each commit):
- `JUNE_BUFF_START_DELAY --with JUNE_BUFF_DEBUG`, 9 files:
  - `files: 4 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 3 OTHER (other flags), 0 FAIL, 0 ERROR`;
  - `RESULT: PASS`.
- `JUNE_BUFF_QUICKSLOT_ITEM --with JUNE_BUFF_DEBUG JUNE_BUFF_START_DELAY`:
  - `files: 2 PASS, 0 PASS~, 1 DEFINE, 6 OTHER, 0 FAIL, 0 ERROR`;
  - `RESULT: PASS`.
  - Without `START_DELAY` in `--with`, `Always.h` is FAIL only because its diff also carries `START_DELAY`'s new `#define`.
- `JUNE_BUFF_RIDING_KEY_WAIT`, 12 files:
  - `files: 11 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`;
  - `in this flag's files: 0 deleted line(s) ... 0 encoding change(s)`;
  - `RESULT: PASS`.
- Deleted lines, all in our own code:
  - 11 are 7d's SAT 18/34 pre-start code in `X2Game.cpp`, replaced under the same `_QUICKSLOT_ITEM` flag;
  - 3 are `Always.h` 7d comment lines;
  - 1 is the `X2GUUser.h` `+0x8d78` comment, in the `_AWAKE_LOCK` block.
- The PASS~ blank line is the separator before the `OnFrameMove` block.
- Post-commit, `--range f20a31c^..HEAD`:
  - `RIDING_KEY_WAIT --with JUNE_BUFF_DEBUG`: `files: 11 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`;
  - `START_DELAY --with JUNE_BUFF_DEBUG JUNE_BUFF_RIDING_KEY_WAIT`: `files: 7 PASS, 1 PASS~, 2 DEFINE, 11 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`;
  - `QUICKSLOT_ITEM --with JUNE_BUFF_DEBUG JUNE_BUFF_START_DELAY JUNE_BUFF_RIDING_KEY_WAIT`: `files: 12 PASS, 0 PASS~, 2 DEFINE, 7 OTHER, 0 FAIL, 0 ERROR`, `RESULT: PASS`.
  - Without the sibling flags in `--with`, `726607a`'s `Always.h` reports FAIL: it adds another 7e flag's `#define` block, which the check does not count as this flag's define.

**Build and deploy.** `DEPLOY OK: X2_offline.exe is 19,116,032 B, dated 2026-10-05 09:37:27, md5 fa09029220a974011a01da4f768e5f34, equal to X2/US_SERVICE/x2.exe (2026-10-05 09:37:22); built at mods/june-buff-port 726607a.` All four 7e strings are in the exe (the two new DEBUG lines, `START_MOTION_DELAYTIME`, and the kept refusal line). Compiler warnings for the new code were not checked line by line. **Not launched.**

**How to see it in game:** the hand-off below. The proof lines are `first pre-start quick-slot allow (322f5140)` (tutorial) and `first start-motion delay end (31fcf040)` (any dungeon start).

**Open for the user:** none blocking. `/cost`: please paste it here.

### Phase 7e — Review (2026-10-05)

**Verdict: 7e matches June. One placement error from 7d, found under kickoff item 5, is fixed under `SERV_IRUHADEV_JUNE_BUFF_QUICKSLOT_ITEM`** (commit below). Ghidra spot-checks, all read-only: `3206eaf0`, `31fcf040`, `3230f110`, `322f5140`, `322fa960` SAT 18/34/38, `329cc370`, `322f7170`'s riding block, Ara `32bcfaa0`, and the writer `32563a8c` (raw `disasm_range.py`).

**Finding (fixed): Ara's `+0x8d78` test was in the wrong function, so Ara's FP cancels skipped it.**
- June Ara slot 510 `32bcfaa0` is March's `CanUseSpecialAttack`. It opens with the test (`@32bcfae1-f0`).
- Its only static reference is the vtable (`33505b0c`). A raw `8b ?? f8 07 00 00` scan finds every `[vt+0x7f8]` load. In Ara they are 509 (`32b90c41`) and 37 FP-cancel sites (`32b99930`, `32b9a080`, `32b9ef43`, `32bac17e`-`32badb1e`, `32bc2158`, `32bc9f18`). All 37 were checked by script. Each has March's shape: `!+0x9620 && +0x9618 <= t && +0x961c > t && 510(1)` → `+0x9620 = 1`. Two of them (`32bac6a3`, `32bad853`) pass 2 instead of 1 and jump back to a shared write.
- 7d put the test at the top of Ara's `SpecialAttackEventProcess`. While a throw item waited, March's FP cancel still set `m_bFPCancel`, and `ComboXX...EventProcess` then calls `StateChangeSpecialAttack` directly (`X2GUAra.cpp:8771`). That is a skill June refuses.
- **Fix:** the test moved to the first statement of `CX2GUAra::CanUseSpecialAttack`. The 509 path is unchanged, because it calls `CanUseSpecialAttack` first.
- This corrects the 7d review's item 3 ("CanUseSpecialAttack sits after the test"), which holds only for 509.
- Elesis is left as it is. Her March `CanUseSpecialAttack` has one caller (`X2GUEL.cpp:1809`), so the placement changes nothing.
- The scan's other 11 hits are each class's 509 calling 510. The `31d*` hits are member reads at `+0x7f8`, not vtable calls.
- `Always.h` and `X2GUUser.h` comments are updated.
- June-only `+0x9621`, which the FP-cancel sites also write, goes to the character port. It is recorded in funcmap and not ported.
- Not reproduced in game: it needs an armed item whose throw never starts while Ara is mid-combo.

**Kickoff checks.**
1. **Countdown home.** `@32070053` is in `3206eaf0` (ret 0xc @32070183; fElapsedTime `[ebp+0x10]`). It runs for every user. The only bypass is the `GUSI_DIE` jump (`@3206f335-41` → `3207015e`), which is March's `:6648` return, so the port's place at the end of `OnFrameMove` is equivalent. The x87 conditions were checked by hand (`>= 0` subtracts; NaN takes the `else`, as `test ah,0x41; jp` does).
2. **`31fcf040`.** It tests `+0x867c` → `3234ca90` (`GetRidingPetInfo`). Both calls are `push 0`. `32932230` true or no pet → 9, else `0x48`.
   - The left-out tests have no March home: `X2Dungeon.h` has no bitset, and no March `DI_*` is ≥ 42000.
   - `USI_START` = 9 (`GUSI_END` = 5 → `USI_BASE` 6). `USI_RIDING_START`/`_WAIT` are 4 apart, as `0x48`/`0x4c` are.
3. **Henir path.** The writer `32563a8c` falls back to 30000 when the ID is invalid, then maps `GetDungeonType(+0x10)`: 2 → `+0x38` = 1, 3 → ebx, 5 → 5. That is March's `#else` mapping in `X2LocalMapUI.cpp` (`dd63297`, :197; `DT_HENIR` = 2, `DM_HENIR_PRACTICE` = 1, `X2Dungeon.h:36-47`). `32931920` is `== 0x94d4` (38100, `DI_ELDER_HENIR_SPACE`). June's party struct layout differs from March's `PartyData`, but the meaning of the fields is pinned.
4. **SAT 18/34/38.**
   - SAT 18: state check or gate failure clears the allow byte, then `322f5140(true)` is inlined.
   - SAT 34: `322f5140(false)`; true → no ACK wait (`@322fb5d2`).
   - SAT 38: the pre-start test, then `[vt+0x10c] != 4` refuses. `PushText(3 = XUF_DODUM_20_BOLD, STR 0x76a7, (1,1,1,1), (0,0,0,1), 1 = DT_CENTER, 1.f, 1.f)`.
   - All three match the port test for test.
5. **Riding-key wait reader.** In all 8 classes it sits right after `CheckSkillUsePVPOfficial` (the `726607a` hunks). Ara: see the finding above. The setter `@322f80a8` writes to `[vt+0x98]()` (`GetMyUnit`) after summon/unsummon. The `3230f110` test ANDs into the `GetRidingOnState` branch, and both fall through to the 24526/24527 text (`@322f80b9`).
6. **Recorded, not ported.** None has a March home:
   - `+0x89e0` (June class mode);
   - `g_pMain+0x16a` (no `CX2Main` member);
   - the Ride button UID recompute (June's sorted riding lists);
   - `3202d140`/`3202d3c0` (character port);
   - `322f11d0`/`322f4c60`/`324cf360` (June's skip-the-start: game `+0xb40`, NPC flag `0x2000`, event `0x4ac`);
   - the bitset bit 8 and `32932230` (item 2).

**Hygiene.**
- The kickoff's commands as given:
  - `START_DELAY --range f20a31c^..HEAD --with JUNE_BUFF_DEBUG`: `files: 4 PASS, 1 PASS~, 1 DEFINE, 14 OTHER, 1 FAIL, 0 ERROR`, `RESULT: FAIL`.
  - `QUICKSLOT_ITEM ... --with JUNE_BUFF_DEBUG JUNE_BUFF_START_DELAY`: `files: 2 PASS, 0 PASS~, 1 DEFINE, 17 OTHER, 1 FAIL, 0 ERROR`, `RESULT: FAIL`.
  - Both FAILs are `726607a`'s `Always.h`, which appends `RIDING_KEY_WAIT`'s `#define` after theirs. That is a problem with the command, not the code.
- With the sibling flag in `--with`:
  - `START_DELAY`: `files: 7 PASS, 1 PASS~, 2 DEFINE, 11 OTHER, 0 FAIL, 0 ERROR`, `3 deleted line(s) ... 0 encoding change(s)`, `RESULT: PASS`;
  - `QUICKSLOT_ITEM`: `files: 12 PASS, 0 PASS~, 2 DEFINE, 7 OTHER, 0 FAIL, 0 ERROR`, `14 deleted line(s) ... 0 encoding change(s)`, `RESULT: PASS`.
- `RIDING_KEY_WAIT --range f20a31c^..HEAD`: `files: 11 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, `0 deleted line(s) ... 0 encoding change(s)`, `RESULT: PASS`.
- Deleted lines were checked with `git show f20a31c -- Always.h X2GUUser.h`: 3 + 1 comment lines, as the Done note says. The other 11 are 7d's SAT 18/34 code, replaced under the same flag.
- The fix, `--worktree`: `files: 1 PASS, 0 PASS~, 1 DEFINE, 1 OTHER, 0 FAIL, 0 ERROR`, `7 deleted line(s), 0 non-ASCII ... 0 encoding change(s)`, `RESULT: PASS`.
  - The 7 deleted lines are the moved 7d block (6) and one comment line in each of `Always.h` and `X2GUUser.h`, all our own.
  - `X2GUUser.h` is OTHER because the `+0x8d78` member sits in 7c's `_AWAKE_LOCK` block.

**Build and deploy.** `DEPLOY OK: X2_offline.exe is 19,116,032 B, dated 2026-10-05 10:14:34, md5 be4fb8fe51ec74869ba07b26325001ac, equal to X2/US_SERVICE/x2.exe (2026-10-05 10:14:30); built at mods/june-buff-port a6684e4 + 3 uncommitted code file(s).` The three files are this fix's own (`X2GUAra.cpp`, `X2GUUser.h`, `Always.h`), committed as is right after. **Not launched.**

**Play-test (2026-10-05, user; exe `be4fb8fe`, run 10:24:43 after the 10:14 deploy): both regression checks passed by eye** (Ara FP cancel into a skill; field throw item, skills back afterwards). `ErrorLog.txt:195` `first arm quick-slot special item wait: item 78390, 10 s`; load baseline unchanged (`factors 361 loaded`, the same 5 ordinal rejects); 0 exceptions (2 UNHANDLED `E_DISABLE_HB_CHECK_REQ`, unrelated). `/cost`: please paste it here.

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

**Kickoff:** `kickoffs/june-buff/8.md` - start it with `/x2-phase june-buff 8`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

### Phase 8 — Done (2026-10-05)

**Play-test (2026-10-05, user; exe `8b491d7d`, run 11:49 after the 11:37 deploy): 4/4 passed, logs read.** `ErrorLog.txt:181` prompt shown; `:182-185` stick-shake count 1-4 alternating; `:186-187` the BFT 15 end hook twice (#15); `:188-189` skills 3014 and 3003 counted as slot A; `:190-209` twenty 0.1 s shake decreases, 2 s in total, then none; `:210` `aura (BFT 14) ended, BTI 9004, creator 12, my unit 1`. No ordinal reject for 362-365; 0 exceptions (2 UNHANDLED `E_DISABLE_HB_CHECK_REQ`, unrelated).

**Landed** under the new flag `SERV_IRUHADEV_JUNE_BUFF_NEW_FINALIZER`, in two code commits. The RE is in the "Buff Phase 8" rows of `june2017_re/funcmap.tsv` (34 rows).
- **`cb83083`: the three finalizers.** Factor parse (`32d4c750`): AURA is empty; 15 and 16 have the hit-count shape and read COUNT as a skill table with a literal 1. Level slice (`32d4cb00`): 15/16 = the hit slice under their own type; AURA takes the default. Factory and the classes `CX2BuffAura-`, `CX2BuffStickShakeCount-` and `CX2BuffUseSkillBySkillSlotTypeFinalizerTemplet`, transcribed from the asm (1c item 4). Unit side: `+0x75a4/+0x75a8` with slots 256/257, the skill-use map `+0x75c8` (258-260), slots 181/182, `322a0090`, and the buff-change counter `+0x6dd0`.
- **`ec6aece` (+ fix `f52a21b`): the increment sites and the sync.**
  - **BFT 15:** `CX2Game::KeyProcess` (`322f7170`) feeds a lone held left/right (June actions 0x25/0x26 = `GA_LEFT`/`GA_RIGHT`, from June's `InputData` call order) into `StickShakeInput` (`31fc9fa0`). The user's slots 256/257 show and remove the "StickShake" prompt (`32011130`/`32011210`); the dtor drops it too. The end hook `3236a6a0` became `CX2BuffTemplet::DoFinishStickShake`, run where June runs it (finish found + the three erases).
  - **SHAKE_DECREASE (4b-ii hand-off):** `+0xbae` = left and right both held (`322cfc60`); `OnFrameMoveBuff` calls `DecreaseDurationByShake` for my user unit and bumps the counter. 4b-ii's "unsupported (Phase 8)" line is gone (`#else`).
  - **BFT 16:** June reserves the used skill (slot 513, 1 s life) and counts it when the skill's state starts (`CommonStateStart` `@32057c23-d84`), after 6b's 98 cache. March has no reservation and spends MP at use, so `CountUsedSkill` (every class's skill use, just before the MP) takes the reservation and `CountReservedSkillUse` at the top of `CommonStateStart` counts key 0 and the slot type (Phase 6's `GetEquippedSkillSlotType`), skipping hyper/relationship skills (`32070580`). June's MP and its stale-reservation branch are not ported. **Deviation (`f52a21b`):** June's slot 516 test is not called, because March's `SpecialAttackNoStageChange` performs the action (writes `nowAction`); a no-stage-change skill never starts its state, so its reservation never matches anyway.
  - **Sync:** the kickoff said "sync the new counters like hit/hitted"; June does not. It syncs one buff-change counter. Ported as June has it: `+0x6dd0` bumped at June's four sites (push, shake, BFT 15 end, aura end on my unit); the user sync carries its low 32 bits as a DWORD at June's bit `0x800000` with the field flag widened to 32 bits (`32d5be30`); on a unit that is not mine, 1 s of disagreement adopts the value and sends **XPT 27** (June `0x1b`, right after the wedding sync) with my UID; the owner answers with March's existing `SendBuffInfoNot` (= June `3203d0b0`, `EGS_UPDATE_USER_UNIT_BUFF_INFO_NOT`). Every peer needs this build (`MODS.md`). March's debuff-count sync stays (June still sends it).
- **Debug lines** (`_DEBUG`): prompt shown, each stick-shake count, the BFT 15 end hook, each counted skill use, each shake decrease, an aura end, a resync request.
- **BFT_ATTACK (17):** nothing to port; June's factor-parse (`32d4c750`) and level-slice (`32d4cb00`) switches stop at 0x10.
- **Census:** `buff_census.py --phase 8` reports FINALIZER "not handled 0" (BFT_ATTACK excluded as June-unimplemented).

**Answers to 1c item 4's open items.**
1. `0x1b`: handled at `322ed9c0 @322edf68`; the reply is a server packet, no new reply type.
2. `+0x8108`: the Now syncData (`+0x80d4`) `+0x34`, filled from packet `+0x3d` by `3201f5f0`. June added a field; it did **not** repurpose the debuff count (`+0x80fc` → `+0x4235` is still applied, `@3206c233`).
3. **Correction:** the observer block runs only for units that are **not** mine (`IsMyUnit` → skip, `@320637d9`), not "every user unit".
4. `+0x8eac` locality: only the per-class skill use sets it (vslot 513); in the port the reservation is local by construction (`CountUsedSkill` runs on key input).
5. `32ec18f0` equals `32ec1620`; `322f7170` is `CX2Game::KeyProcess`.

**Hand-offs.** Aura's socket side → **Phase 16** (user, 2026-10-05): slot 181 needs SIEE 15 and the socket `BUFF_FACTOR_ID` list (`+0x29c`), and the emitter (`ProcessSocketEffectEx` case 15) is Phase 16's; until then a user's aura ends at its first check, and the slot logs one `unsupported (Phase 16)` line if it ever sees SIEE 15. Add's slot 303 override → checklist §7. June bugs kept: `JUNE_BUGS.md` #15 (double `EndStickShake`), #16 (packet-built 15/16/aura copies never finish by themselves).

**Hygiene** (`flagcheck.py JUNE_BUFF_NEW_FINALIZER --range cb83083^..HEAD --with JUNE_BUFF_DEBUG`, before `f52a21b`): `files: 16 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The PASS~ is the blank line before the new block in `X2BuffFinalizerTemplet.cpp`. `f52a21b` only edits this flag's own lines.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,141,120 B, dated 2026-10-05 11:37:26, md5 8b491d7d1a8108c1a208ad94ae196580, equal to X2/US_SERVICE/x2.exe (2026-10-05 11:37:21); built at mods/june-buff-port f52a21b + 18 uncommitted code file(s).` The 18 files are peers' work in progress (Phase 10 `JUNE_BUFF_GROUP`/`_BINDING` in X2BuffTempletManager, X2Main, X2StateStartUp, X2GameEdit, X2GUUser, X2Game; the effect work in X2DamageManager, X2GUNPC, X2GUAra), built in with it.

**How to see it in game.** No March data uses these types, so the play-test registers four throwaway buffs from the dev console after each launch: templets BTI 9001-9004, then factors 362-365 (the first ids after March's 361 slots; register them once, in this order). 9001 = BFT_TIME 30 s + BFT_STICK_SHAKE COUNT 4; 9002 = 60 s + BFT 16, slot A, COUNT 2; 9003 = 20 s with SHAKE_DECREASE 0.1 / max 2; 9004 = AURA. Apply with `g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 36x, 1 )`; the exact lines are in this phase's hand-off and the review kickoff. Without them, normal play only exercises the counter bumps and the shake trigger's key read.

### Phase 8 — Review (2026-10-05)

**Verdict: Phase 8 matches June. No transcription errors, no code change; the exe stays `8b491d7d`.** Ghidra spot-checks, all read-only: `322f7170` (the stick-shake block), `31c488c0`, `322cfc60`, `32ec4b10`, `32ec4c50`, `32ec1580`, `3236c570`, `32063650` (`@320636ad-6f1`, `@320637cd-393a`), `32057b80` (`@32057bfb-d86`), `32070580`, `32d5be30`, and after the advisor pass `32d44a80`, `32d44d60`, `32d48640`, `32d48eb0`, `32d49060`, `3229dee0` (`@3229dfb3-dcd`), `3229d620` (`@3229d712-766`), and with `buffre/rawfn.py` (uncarved) `32ec01d0`, `32ec5890`.

**Kickoff checks.**
1. **Class bodies.** Aura GetFactor `32ec4b10` has no unit test and packs `+0x30` low word, `>>16`, `>>32` (`__allshr`), the word at `+0x36`, then `(float)+0x38`, as the port does. BFT 15 GetFactor `32ec4c50` returns early with no unit (`param_3 != 0`). BFT 15 SetFactorFromPacket `32ec1580` stores v[0], then v[1] as a byte, sets started, and subtracts 0x100 when the unit's `+0x75a8` byte is below the snapshot. It has no slot 256 call. BFT 15 DidFinish `32ec01d0` = unit && `+0x25` && ( `+0x20` <= the whole dword `[unit+0x75a8]` (signed `jle`) || `+0x18` ). Aura DidFinish `32ec5890` = `g_pX2Game` valid and unit, else true; `+0x1c != 2` → false; `IsMyUnit` (slot `0x198`) → bump; true. Both as ported. The port's `DISPLAY_ERROR` on June's silent-false paths is this file's redefinition to `CX2JuneBuffCapture::RuntimeError` (`X2BuffFinalizerTemplet.cpp:15`), a log line with no popup.
2. **Factor parse and level slice.** `32d44a80` / `32d44d60` (identical bodies) push the type (`FILD`), then COUNT through `32d41a80( L, key, &vec, 1 )` with a literal 1 (`@32d44cd9`), then 0.0: the port's `ParsingHitCountFinalizer` with `true`. `32d48eb0` / `32d49060` are copies of the hit slice `32d48640` (type 2), with the result type 0xf / 0x10 set at the top. The port's slice-then-retag gives the same `[type, COUNT[level], 0.0]`.
3. **Input mapping: confirmed twice.**
   - June's `KeyProcess` calls `InputData` (`3230f5f0`, four outputs into `m_InputData` `+0x345`-`+0x353`) with 0x25, 0x26, 0x23, 0x24 (`@322f7525-7594`, and the same at `@322f7388-73f7`). March calls it with `GA_LEFT`, `GA_RIGHT`, `GA_UP`, `GA_DOWN` (`X2Game.cpp:5425-5428`).
   - March's `GAME_ACTION` enum has UP/DOWN/LEFT/RIGHT = 0x21-0x24 (`KTDIManager.h:97-100`). June's are the same four, each +2.
   - `31c488c0` returns `byte[this + 0x4d + action] != 0`, a plain array read with no lock. That is March's `GetPureActionState` (`m_State[Action]`), not the locking `GetActionState`.
   - The shake call pushes the 0x26 result first, so 0x25 is the first argument (`bLeft`). `322cfc60` = 0x25 held && 0x26 held.
4. **BFT 16's host.**
   - Every class's skill use reaches `CountUsedSkill`: the 8 base classes, Add `StateChangeSpecialAttack` (`X2GUAdd.cpp:2650`), EL (`X2GUEL.cpp:3157`), and both of Ara's variants (`X2GUAra.cpp:3916`, `:4004`).
   - The 1 s expiry `@320636ad-6f1` subtracts `+0x2b0` and, at `<= 0`, calls slot `0x808` = 514. The port's `CommonFrameMove` block does the same.
   - In `32057b80`, the skill test is slot `0x524` (the same value compared with `+0x8eac`) `> 0` (`@32057cfe`). So the port's `0 < eSkillID` is June's test; the 1c note's "GetNowStateID() > 0" was loose wording. `32070580` = type is neither 0xc nor 0xd = March `ST_RELATIONSHIP_SKILL` 12 / `ST_HYPER_ACTIVE_SKILL` 13 (`X2SkillTree.h:46-50`).
   - The rest of June's reserved branch (MP from `+0x8ea8` via slots `0x800`/`0x528`/`0x764`, `+0x9440`) is the MP spend March already makes at use. `f52a21b`'s slot 516 deviation stands: a no-stage-change skill never starts a state whose skill ID matches.
5. **The sync.**
   - Field flag: `32d5be30` reads a DWORD flag (`@32d5be42-53`, `OR [esi], ebx`). Bit `0x800000` (`@32d5c0f8`) reads a DWORD (`32d5bdc0`) into packet `+0x3d`.
   - Serialize/Deserialize order is this build's own; it is wire-compatible only between this build's peers (`MODS.md`).
   - `eUnitUserSync_DataField_All` includes the bit. The receiver presets `All` (`X2Game.cpp:899`), so `ReceiveFrameData`'s `!= All` gate (`X2GUUser.cpp:22853`) passes. `bySerialized` is `sizeof(KDYNAMIC_UNIT_USER_SYNC)` and grows with the struct.
   - XPT 27: `SP_END` = 16 (`KTDNUDP.h:66`), and `SERV_PET_SYSTEM` is on (`ServerDefine.h:460`), so `XPT_BUFF_INFO_REQ` = 16 + 11 = 27 = 0x1b. The handler checks `m_Size` against the payload and reads `m_pRecvBuffer`, the same pattern as `XPT_PING_TEST_REQ`.
   - Observer `@320637cd-393a`: `IsMyUnit` (slot `0x198`) skips the block (`@320637db`). It needs a game with my unit. It compares the low dwords `+0x8108` / `+0x6dd0`. Set to 1.0 / clear to 0 / subtract `+0x2b0` / fire at `<= 0`, then `32070c90( +0x8108 )`, then `33224850( this UID, 0x1b, &myUID, 8 )`. Every x87 branch was checked by hand, and the port is equivalent. June stores the negative remainder in place of 0; the next frame takes the same branch either way.
6. **Bump sites and the double end hook.** `3236c570` runs `3236a6a0` at the end of **every** call while the buff's finished byte `+0x65` is set, not only on the transition. The port's `if ( GetDidFinish() ) DoFinishStickShake` at the function's end is the same. With the erase-time call, that gives #15's double `EndStickShake` (ref count clamped at 0). The other three bump sites, read: push `@3229dfc0` runs only after `3236d400` (SetFactor) returns true, as the port does. Shake `@3229d712-766` = `IsMyUnit` && `+0x264 == 0` (`GUT_USER` = 0, June's own test) && game `+0xbae` && `3236c990` true. Aura `@32ec58f4` is inside the `IsMyUnit` test. June increments the low dword and stores 0 to the high one at every site; the port's `++` on 64 bits has the same low 32 bits (the only half synced or compared) until a 2^32 wrap.

**Not exercised.** The sync (XPT 27, the bit `0x800000` field) needs two peers running this build; offline play has none. Checked from source and June only.

**Hygiene.**
- The kickoff's command (`--range cb83083^..HEAD`) now spans Phase 10's `9eba9ad`: `files: 17 PASS, 1 PASS~, 1 DEFINE, 17 OTHER, 1 FAIL, 0 ERROR`, `RESULT: FAIL`. The FAIL is `9eba9ad`'s `Always.h`, which appends Phase 10's `#define`s after this flag's. The 17 OTHER rows are Phase 10's own flags. That is a problem with the command's range, not the code.
- Scoped to Phase 8's own commits (`--range cb83083^..f52a21b --with JUNE_BUFF_DEBUG`): `files: 17 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 3 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  - The 3 deleted lines are `f52a21b` rewriting 2 comment lines and the `SpecialAttackNoStageChange` condition, all inside this flag's own `CountReservedSkillUse`.
  - The PASS~ is the blank line before the new block in `X2BuffFinalizerTemplet.cpp`.

**No build or deploy:** nothing changed. **Owed by the user:** /cost only.

## Phase 9 — Custom-function buffs

**Scope.**
- Port June's whole `CreateAndInsertCustomFunctionByBuff` switch and every `CX2CustomFunctionByBuff*` class it creates, for the 17 BTIs in the ledger. Include the Rose, Ain and Lu/Ciel cases (user decision).
- Port each hook body: CommonStateStart, FrameMove, StateEnd and DamageReact (`X2GUUser.cpp:14613/17894/19467/8327`).
- Where June moved per-class handling of FLASH_BANG or PANDEMONIUM_FEAR (`X2GUAra.cpp:13854`, `X2GURaven.cpp:21764`) into the buff system, follow June.
- Split into 9a and 9b by character family if needed.
- **Phase 4a hand-off:** June's `SetBuffFactorToGameUnit` ends with `32275860(factor)`, which calls slot 7 of every object in the custom-function-by-buff list (`unit+0x6734`; slots 3–6 are March's four hooks). Port slot 7 with the classes and call it at the end of the Phase 4a body.

**Run with:** Opus 5.5, high effort. Review required.

**Kickoff:** `kickoffs/june-buff/9.md` - start it with `/x2-phase june-buff 9`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

### Phase 9a — Done (2026-10-05)

Split by family as the kickoff allows: **9a = the shared engine + Elsword + Chung** (the four custom-function BTIs March's live data carries: 29, 56, 62, 223). **9b resumes** with the dormant families (`kickoffs/june-buff/9b.md`). Flag `SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION`, commit `b8bc0a1` (code). RE rows: `june2017_re/funcmap.tsv` ("Buff Phase 9a").

**Slot map (corrects Phase 4a's "slots 3–6").** June's `CX2CustomFunctionByBuff` vtable has 9 slots: [0] dtor, [1]–[4] March's StateStart / FrameMove / StateEnd / DamageReact, then four June-only hooks, all ported with their dispatchers and call sites:
- [5] `AttackResultByType` (dispatch `32275700`): first thing in `CX2GUUser::AttackResultByType` (`320144d0`, vslot 312) and `CX2GUNPC::AttackResultByType` (`32773ee0`). June's Ara calls the base first (`32bceca0`), March's did not: added.
- [6] NPC final damage (`322757b0`): `CX2GUNPC::DamageReact` only, right before the damage number (`X2GUNPC.cpp`, before the `m_bDamageOutScreen` block).
- [7] buff applied (`32275860`): end of `SetBuffFactorToGameUnit` (Phase 4a body).
- [8] erase (`3227a1d0 @3227a206`): before the erase in `EraseCustomFunctionByBuff`.

**Creator info.** `CreateAndInsertCustomFunctionByBuff` takes June's third argument (`KCustomFunctionCreatorInfo`: not-user, creator UID, creator power rate). `CX2BuffTemplet::SetFactor` stores the creator's `GetPowerRate()` first, as June `3236d400` (+0xbc). The packet path has no power rate (June's `KBuffFactor +0x57` is absent from March's shared header): it keeps 1.0. Only 9b's classes read it.

**Switch (`3229e1a0`), 9a cases.** 29 Phantom Sword, 56 Unfixed Clip (**users only**, June's gate), 62 Mark of Commander, 223 Pandemonium Fear (new class). The other ten June cases still create nothing.

**Class bodies, June vs March:**
- **Phantom Sword:** StateStart / FrameMove / StateEnd / dtor match March. `CreateAndInsertThirdBladeData` (`32370e10`) halves techPoint, fHitAddMP and `m_fRateModifier` (new constant 0.5), where March zeroed fHitAddMP. It no longer forces `AT_SPECIAL`. The memo variants are gone: `31fca9e0`, June's `GetEqippedSkillMemo`, is a stub returning false. Not ported: `32296ee0`, the game-option effect alpha on the centre mesh.
- **Bigbang Stream:** gains a cool time, `SA_READY_TIME_REL` of `SI_SA_CHG_BIGBANG_STREAM` at the learned level (`32007b10`, ported as `JuneGetLearnedSkillAbilityValue`). StateStart skips while it runs and arms it after the table. FrameMove ticks it. DoExtraAttack always throws HAND_GRENADE_0: the memo grenade is gone, and June's ULTIMATE variant sits behind the absent skill-memo map. The throw takes `abs()` of both x and z.
- **Mark of Commander:** a user's shell power × `GetSkillDamageRateByBuff( SI_A_CHG_MARK_OF_COMMANDER )` × (1 + `3201ad10`). `3201ad10` is June's skill damage-up total; every source is June-only, so it is 0 here (Phase 16 may own its `+0x87b4` map). Always the plain shell (no memo).
- **Pandemonium Fear (new):** FrameMove ticks a value[0] interval. DamageReact copies Mark of Commander's guards (trigger 6). It creates `DAMAGE_EFFECT_Pandemonium_Fear_Explosion` at the hit unit's centre, re-aimed by June's `31fd5ef0` when a user hits an NPC (`JuneGetTargetHitPos` + `32293b80` / nearest collision frame), with power = damage by type × value[1]. **March's DamageEffect.lua has no such effect, so nothing appears until Phase 13.**

**FLASH_BANG / PANDEMONIUM_FEAR "per-class handling"** (scope bullet 3): both March sites are reduce-debuff-duration lists, not behaviour. Ara's list was replaced in Phase 10. Raven's (`X2GURaven.cpp` `GetReduceDebuffDurationRate`) and the four BTIs June has no case for (100, 241, 426, 427) are **9b items 4–5**.

**Hygiene / deploy:** `flagcheck.py JUNE_BUFF_CUSTOM_FUNCTION --range b8bc0a1^..b8bc0a1 --with JUNE_BUFF_DEBUG`: `files: 5 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. Always.h and X2GUUser.cpp also held Phase 11's uncommitted lines: only 9a's hunks were staged. Deploy: `DEPLOY OK: X2_offline.exe is 19,173,376 B, dated 2026-10-05 15:18:52, md5 9ebc4b93754f5af2ac7027f416a1e92d, equal to X2/US_SERVICE/x2.exe (2026-10-05 15:18:48); built at mods/june-buff-port 179cbe0.` The bytes are Phase 11's deploy (built from the tree once its code was committed, so it also carries `b8bc0a1`); a clean rebuild at 179cbe0 relinked nothing. The 9a strings (`custom function created: BTI`, `DAMAGE_EFFECT_Pandemonium_Fear_Explosion`) are in the deployed exe. The same exe serves Phase 11's play-test.

**How to see it in game:** see the hand-off (Elsword Phantom Sword, Chung Bigbang Stream and Mark of Commander regression; first-creation `[JUNE_BUFF]` lines).

**Play-test (2026-10-05, exe 0dd05880, user: 4/4 passed; logs read).** `custom function created` for BTI 29 (Lord Knight, creator itself), 56 (Freils, `cool time 1 s`: Bigbang Stream is not learned, so June's 1.0 default) and 62 (NPC, creator the NPC); a `ChungMarkOfCommanderShell` JUNE_FX line shows the shells fell. **No line for BTI 223, as it should be:** March's live templet for 223 (fresh `elstools decompile-batch` of `BattleFieldBuffTemplet.lua`, 152,564 B) is SKILL_ATTACK_IMPOSSIBLE + CHANGE_ANTI_CRITICAL_RATE with no BBT_CUSTOM_FUNCTION, so the class is created only once Phase 13 adopts June's templet (SKILL_ATTACK_IMPOSSIBLE + CUSTOM_FUNCTION + IGNORE_STATE_GUARD). The run's 3 UNHANDLED `E_DISABLE_HB_CHECK_REQ` are not this phase's.

**Open for the user:**
1. June's user `AttackResultByType` also runs `32011280` (June's Lanox "Secret Ammo" set: BTI 794, `DamageEffect_Ranox_Secret_Ammo_5`) and a riding-pet effect for BTI 562. Neither is a buff-port system. Left out; say if the Lanox plan should own it.
2. Packet-path creator power rate = 1.0 (see above). It matters only for 9b classes that read it.

### Phase 9a — Review (2026-10-05, Opus 5.5 high)

**Verdict: no transcription errors, no code change.** The exe stays 0dd05880 and there is nothing to play-test. All eight kickoff items were checked in Ghidra and with `rawfn.py`, read-only.

1. **Slot map: correct.**
   - Dispatchers: `32275700` calls `+0x14` (unit, arg); `322757b0` calls `+0x18` (unit, arg1, arg2); `32275860` calls `+0x1c`; `3227a1d0` calls `+0x20` at `@3227a206`, before the erase `3207b700` at `@3227a214`.
   - Every vtable from `vt.py CustomFunction` has the same empty stubs: `ret 8` at [1]–[5] and [7], `ret 0xc` at [6] (3 args), `ret 4` at [8] (1 arg). The overrides fall where 9a put them.
   - Slot 6's argument order is confirmed twice. ChainViper `3236fae0` does `fld [arg2]`, so the float pointer is the second argument. The caller `@327b4802` pushes the DamageData, then `&[esp+0x2c]`.
2. **Call sites: correct.**
   - User `320144d0`: `32011280`, then slot 5. NPC `32773ee0`: slot 5 first.
   - Ara `32bceca0 @32bcece3`: the base call is the first instruction after the prologue.
   - Slot 7 (`322a8885`) is the last call before `return true`, with the factor (EDI). The port's line 11757 sits in `SetBuffFactorToGameUnit`'s Phase 4a body; git's hunk header naming `...AtLevel` is misleading.
   - **NPC slot 6, the kickoff's question.** In June, `[esp+0x2c]` at the call is March's `fFinalDamage` after a few June-only steps that March does not have:
     - attacker-class hooks (vslots `0x6d4` / `0x4f8` / `0x4fc`, and a `+0x7ca8` buff apply);
     - an execute block for NPC 41350 (`0xa186`) below 20% HP, which sets the damage to 3e9;
     - `322cfca0`: × 0.5 in dungeon mode `+0xd8 == 8` for attackers of type ≠ 2;
     - the attacker's vslot `0x304`.

     Nothing sits between slot 6 and the damage number in either build, so 9a's placement is right. Only ChainViper (9b) reads the value, and it gets March's damage. This is recorded in the 9b kickoff.
3. **Creator info: correct.**
   - `3236d400` looks up the creator through CX2Game slot 50 (UID, not-user). It writes `+0xbc` only when the creator is found.
   - `32e79b00`: not-user = `sete` of factor `+0x38`.
   - `32e73f90`: not-user = `KBuffFactor +0x3d` directly. That is `m_bIsMesGameUnitNpc`: Phase 11's serializer row has the bool there, right before the factor-ID int, which matches March's tail.
   - Side note: March's `KBuffFactor` copy constructor does not copy that flag or the UID. 9a only prints them in the DEBUG line; 9b item 6 already owns the issue.
4. **Case 56: correct.**
   - `@3229e254`: `+0x264 != 0` leaves the case.
   - The cool time is `32007b10( 0x26, 31fcf430( 0x65a3 ) )`; the ctor takes (identity, unit, float).
   - June's parser `32216320` maps `"SA_EFFECTIVE_RATE"` to 5 (`@32218f22`) and `"SA_READY_TIME_REL"` to `0x26` (`@322193ed`). March's enum gives the same numbers, with no `#ifdef` gap before them.
   - `32007b10` matches `JuneGetLearnedSkillAbilityValue`.
5. **Phantom Sword: correct.**
   - June's `SimpleInit` `31f8ee50` identifies the three fields: `+0x34` is zeroed (techPoint), `+0x3f8` is the protected float set to 6.0 (fHitAddMP's March default), `+0x51c` is set to 1.0 (`m_fRateModifier`).
   - The write order at `32370f96`–`323710a6` matches the port.
   - `32370e10` has no `attackType` (`+8`) store. The one `+4 = 0` (`@32370ed2`) is `attackerType = AT_UNIT`, the same line March has.
6. **Bigbang Stream: correct.**
   - `3236c6f0` skips when cool > 0 (`test ah,0x41 / jp`) and arms the cool time only on the table-success path after `EndTable`.
   - `3236a040` runs the loop, then the tick, and clamps at ≤ 0.
   - `3236dbf0` takes FABS of x (`@3236de60`) and z (`@3236de6e`). Velocity x = (|x|+|z|) × 0.3 × 10 + 100, y = y × 10 + 100, z = 0. Everything after the land y is a `CreateInstance` default.
   - The ULTIMATE gate is `31fcf3e0` (memo map), which is false in March (Phase 10).
7. **Mark of Commander: correct.**
   - `3236df00` computes (physic + magic) × dungeon rate. For a user it then multiplies by `31fd7cc0(0x6666)` and by (1.0 + `3201ad10(0x6666)`). The guards are March's.
   - `3201ad10`'s skill-type source (`32358970` on the CX2Unit, map `+0xaac`, keys 6–10) is June's per-type socket stat. March's nearest analog, `m_fSkillDamageUpRate`, is already applied at hit time to every AT_SPECIAL hit (`X2GameUnit.cpp` `GetAdditionalAttackPowerRateByType`), and the shell is AT_SPECIAL (June `DamageEffect.src.lua` `CHUNG_MARK_OF_COMMANDER_SHELL`). Mapping `3201ad10` onto it would count it twice, so 0 is right.
8. **Pandemonium Fear: correct.**
   - `3236e2a0` uses the guards with trigger 6, and checks the attacker before the interval.
   - damageType 0 / 1 / 2 gives `+0x38` / `+0x3c` / (`+0x3c` + `+0x38`) × 0.5, else 1.0. March's `DT_*` and `Damage{fPhysic, fMagic}` agree.
   - The position is the hit unit's vslot 178. It is re-aimed only for a user attacker on an NPC, with `(target, 1, 1, &pos)`. Then × `+0x18`. The name at `0x334902b0` is `DAMAGE_EFFECT_Pandemonium_Fear_Explosion`; the rotation is the hit unit's and the rest are defaults.
   - `31fd5ef0`: the 700 compare is strict `>` (`0x3344ce98` is a double); the bone-nearer test is strict `<`. Mode 1 tests `32293b80 == vslot 0x170` and falls back to the centre under the same test; any other mode returns false.
   - `32293b80`: true without a game, world or line map; false for an out-of-range or null line; otherwise dot > 0.0, strict.

**Hygiene:** `flagcheck.py JUNE_BUFF_CUSTOM_FUNCTION --range b8bc0a1^..b8bc0a1 --with JUNE_BUFF_DEBUG`: `files: 5 PASS, 3 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.

**Kickoff change:** `kickoffs/june-buff/9b.md` item 1 (ChainViper) now notes that slot 6 sees March's final damage, without June's modifiers (item 2 above).

### Phase 9b — Done (2026-10-05)

Flag `SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION` (+ `_DEBUG`), commit `6d6e9e9`. RE rows: `june2017_re/funcmap.tsv` ("Buff Phase 9b"). June bugs #30–#35. Character halves: `JUNE2017_CHARACTER_PORT_CHECKLIST.md` §7 (8 rows).

**Gate met.** Every June case of `3229e1a0` now has its class: 439 Moonlight Rhapsody, 720/920 Servo Nitro, 767 Sinister Mind, 819/894 Summon NPC Target, 891 Chain Viper, 927 Atomic Blaster, 952 Attack Me (NPCs; any other unit erases 952, a no-op on a fresh apply: #34), 1010 Geist Herrschaft, 1022 Moder, 1038 Death Sign. The `custom function created: BTI n (class …)` DEBUG line names the class through `typeid`, so 9a's four are named too. Ctor arguments, vtables and every override, including the six non-default destructors, follow June. The classes are dormant: none of these BTIs is in March's live data until Phase 13, and the Rose / Ain / Lu-Ciel calls are empty `CX2GUUser` hooks. **Not dormant:** the per-hit bookkeeping below runs on every hit (the critical add stays 0 without Atomic Blaster; the state and serial writes change no damage).

**New state the classes read (all under the flag):**
- DamageData `m_fJuneAddCriticalRate` (+0x5d4, 0), added to `GetCriticalPercent`'s result in both DamageReacts before WayOfSword (`@32048a49`, `@327b36ef`); Atomic Blaster adds 1.0 on an `AT_COMMAND` hit.
- DamageData `m_iJuneAttackerStateID` (+0x5e0, -1) and `m_iJuneAttackSerial` (+0x5e8), written at the end of `SetDamageExceptionProcess` (`@322996e0`), with the attacker's serial `+0x40d8` (++, wraps to 0). Users reach it: `CX2GUUser`'s override calls the base first, and the class overrides call `CX2GUUser::` first, as June's user slot 174 (`32045340`) calls `322990e0` first.
- Unit `m_iJuneLastHitAttackSerial` (+0x40dc): both DamageReacts store the hit's serial first thing inside `IsDamagedReaction` (`@3204719d`, `@327b306b`). Chain Viper's count seed.
- Unit `m_bJuneReservedDeath` (+0x258, `32082790`): Death Sign's erase hook sets it; `CX2GUUser::OnFrameMove` clears it and calls `SetNowHp( 0 )` (`@3206f833`); `ReInit` clears it (`@320244ab`).
- `KBuffFactor`'s copy ctor (`CommonPacket.h`) now copies `m_uidMesGameUnit` and `m_bIsMesGameUnitNpc` (item 6). Only `X2Lib` compiles that header (`X2ServerProtocol`'s include is commented out), the servers never see `Always.h`, and Put/Get are unchanged: no wire change. Its only reader is 9a's packet-path creator info (`SetFactorFromPacket`), so no live behaviour changes. No `X2Lib` file compiled without the PCH includes a packet header (the only one is `sqlite3.c`; `X2` compiles just `X2.cpp`), so every copy of the inline ctor sees the flag.

**Kickoff items 2–8:**
2. `+0x5d4` writers: SimpleInit (0, ported); `3202ad3d` is the skill-memo switch `3202ac40` (memo absent here, recorded); `32d28444` / `32d3641d` initialise a different struct (`+0x5c0` is a float there, a word in DamageData). The DamageData copy (`31e9ca2a`) is March's implicit copy.
3. Ten hooks on `CX2GUUser` (Rose `32a800a0`, `32ad0e90`, `32ad0fd0`, `32ac2c50`, `32ac2d50`; Ain `32a4ddb0`, `32a4de20`; June-only summon target `32011960`, `31fd0590`, `32011ab0`). `32082790` is base code (ported). Attack Me's `3275b390` is March's `CX2NPCAI::SetTargetGameUnit`.
4. June Raven's slot 167 (`32c273f0`) lists the same 12 BTIs as March's `GetReduceDebuffDurationRate`: no change.
5. FLASH_BANG 100, TRAPPINGARROW_FUNGUS 241, BLOOD_REVENGE_COUNTER 427: no reader anywhere in June's binary (`HaveThisBuff`, `GetBuffAccumulationCountNow`, the BTI-taking helpers, the factor-list scans) or Lua; their templets are an empty CUSTOM_FUNCTION plus a timer, so in June they are inert tags too. BLOOD_RUNAWAY 426: only Elesis `32b87880` (§7 row). The `0x1a8`–`0x1ac` run is skill→state mapping, not BTIs.
6. See above. Geist Herrschaft is the only class that reads the power rate (question 1 below).
7. Slot 5 coverage matches March one for one: UnitToUnit user + NPC, MeshToUnit user, CollisionDataToUnit user, EffectToUnit NPC, DamageEffect `AttackResult` user.
8. `322a1457` / `322a4cee` are the unit's destruction / construction (vector release, zeroing). Neither runs slot 8, as in March.

**Hygiene:** `flagcheck.py JUNE_BUFF_CUSTOM_FUNCTION --range 6d6e9e9^..6d6e9e9 --with JUNE_BUFF_DEBUG`: `files: 3 PASS, 7 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 6 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. The 6 deleted lines are this flag's own code in `X2GameUnit.cpp`: 9a's switch brief (3 lines) and its DEBUG line (3), rewritten.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,186,176 B, dated 2026-10-05 16:59:03, md5 44c5b837c54db774400411c335c63ef8, equal to X2/US_SERVICE/x2.exe (2026-10-05 16:58:58); built at mods/june-buff-port 6d6e9e9.` The bytes were linked by a peer session's build from this shared tree, after all of 9b's edits (my own build relinked nothing); the 9b strings (`custom function created: BTI %d (%s)`, `Death Sign: reserved death fired`, `DamageEffect_Debuff_Geist_Herrschaft`, …) are in the deployed exe.

**How to see it in game:** the new classes show nothing until their data arrives. The play-test is regression: the `custom function created` lines (now with the class) for 29 / 56 / 62 from console applies, 241 still creating nothing, and a minute of ordinary combat for the per-hit bookkeeping (damage and criticals as before, no crash).

**Play-test (2026-10-05, exe 44c5b837, user: 5/5 passed; logs read).** ErrorLog `:198` `custom function created: BTI 29 (class CX2CustomFunctionByBuffPhantomSword)` (jtLordKnight, creator 25), `:216` BTI 56 `BigBangStream` (Freils, creator 18), `:218` BTI 62 `MarkOfCommand` (NPC, creator 17, not-user 1). No `Death Sign:` or `Sinister Mind:` line; 241 created nothing, as it should. The run's 3 UNHANDLED `E_DISABLE_HB_CHECK_REQ` are not this phase's.

**Open for the user:**
1. **Geist Herrschaft's power** is the creator's power rate. On the packet path it is the 1.0 stand-in (June's `KBuffFactor +0x57` has no March field), so a Geist debuff restored from a packet hits at power 1.0, not the creator's rate. Keep the 1.0 stand-in, or add the field to the shared header (a server-visible struct change)?
2. **Sinister Mind divides by `max / 3`** with no check (#33). Below a max of 3 that is a crash, so the port skips the effect swap there. Confirm the guard.
3. **Death Sign's dungeon exemption** is not ported: June spares the user when the current sub-stage's June field `+0x48` is 4 (game `+0xc44` → `+4` → `+4`), and I could not match that field to March's `CX2DungeonSubStage`. A reserved death therefore always fires here.

### Phase 9b — Review (2026-10-05, Opus 5.5 high)

**Verdict: no transcription errors, no code change.** The exe stays 44c5b837, and the 9b play-test (5/5) already covers the regression. Three new June bugs (JUNE_BUGS #36–#38), #31's open question answered, and one Done-note claim corrected. Read-only Ghidra / `disasm_range.py`; the June-side jump-table decode was done by hand from the exe.

**Hygiene:** `flagcheck.py JUNE_BUFF_CUSTOM_FUNCTION --range 6d6e9e9^..HEAD --with JUNE_BUFF_DEBUG`: `files: 3 PASS, 7 PASS~ (blank/marker only), 1 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 6 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`. The 6 deleted lines are 9a's switch brief and DEBUG line, rewritten (as the Done note says).

1. **Switch arguments: correct.**
   - Register map: `ESI` = identity, `[esp+0x2c]` = factor, `[esp+0x30]` = creator info, `EBX` = unit.
   - Both jump tables decode to the 14 cases. The `0x37e` table gives 894/920/927/952/1010/1022/1038.
   - Each ctor's pushes match the port: 439 (identity, factor copy); 720/920 (identity, unit); 819/894, 952, 1022 (identity, uid, unit); 891 (identity, factor *pointer*, uid, unit); 927, 1038 (identity); 1010 (identity, uid, `FLD [info+0x10]`, unit).
   - 767: both `32007b10` calls run with `ECX` = the creator (`2`, `0x7560` → `ftol`; `0x94`, `0x7560`). June's ctor also takes a factor copy it never reads, so the port drops it.
   - 952's gate is `[EBX+0x264] == 1`. On any other unit it calls `3229e160(0x3b8)`.
   - Attack Me's creator lookup is CX2Game slot 50 (`322d80a0`): unit-list walk, UID **and** `+0x264` type. It runs `(uid, 1)` (NPC), then `(uid, 0)`, matching the port's `GetNPCUnitByUID` → `GetUserUnitByUID`.
2. **Destructors: correct.** All six (`3236f390`, `3236f7c0`, `3236f9f0`, `32370000`, `32370300`, `3236f480`) were decompiled. Guards, the erase of 721 on the stored user, the Rose `[[+0x867c]+0xaf0] == 0xb` checks and the `(+0x18, +0x1c)` arguments all match. The Sinister Mind dtor's `CreateInstance` (`31f8ff00` on game `+0x3b4`) uses March's defaults exactly: land 0, reserve 0, −1, 1, 1, −1, −1, 0, −1. It has no reActType write.
3. **Sinister Mind: correct.**
   - The step is `ftol(max/3.0)` with an `idiv` and no zero check; the level is compared but not stored (#32, #33). The swap is `Seed_Plant_2` for level 1 and `_3` otherwise (strings at `33490308` / `33490360`).
   - At the full count it calls slot 136 `(0x31a, uid lo, uid hi, 1, 1)`, and `+0x28 = -1` follows both branches.
   - FrameMove writes `+0x224 = 7` (the dummy hit) only on its own effect, and times out on `<= 0` (`test ah,5 / jp`). The power-rate multiply order matches `32369bd0`.
4. **Moonlight Rhapsody: correct.**
   - Guards, then trigger 7 via `32610fb0(7)`.
   - The scan keeps the last non-zero `+0x34` (stride `0x5b`) of BTI `0x1b7`.
   - Count `<= 0` skips the erase (`JLE` past `3229e160`). Any count other than 1–3 erases without playing anything.
   - The attacker's hyper state uses `31e4f890` (remaining time) `> 0.0`.
   - `PlayEffectSet` gets (attacker, 0, hyper, power, −1, 1,1,1, custom 1, `&+0x460`, 0-vec, 0-vec, 1.0); the trailing 1.0 has no March parameter.
   - The erase acts on the debuffed unit (`[esp+0x24]`).
5. **New fields: correct.**
   - SetDamageExceptionProcess `@322996e0`: `+0x5e0` = vslot 67, `+0x5e8` = `+0x40d8`, `inc`/`JNS` wrap. June's early exits (null attacker, `@322990f4..3229910c`) skip the tail, as the port's `return` does.
   - `+0x40dc` is stored first inside `IsDamagedReaction`: user `@3204719d`, before vslot 200; NPC `@327b306b`.
   - The critical add is `fld [DD+0x5d4]; fadd` straight after GetCriticalPercent (vslot 117) at both `@32048a49` and `@327b36ef`.
   - `+0x258` is a KProtected bool (`@3206f833` xors `+0x25c`/`+0x260`), cleared with `32082790(0)` before `SetNowHp` (vslot 61) with 0.
6. **`KBuffFactor` copy ctor: correct, but the Done note's reason is wrong.**
   - The member order matches the declaration.
   - X2ServerProtocol **does** compile `CommonPacket.h`, without the flag: `StdAfx.h` → `X2ServerProtocolLib.h:68` → `SystemPacket.h:5`.
   - It emits no code for it: `X2Lib/X2ServerProtocol.lib` holds only debug type names, with no `??0KBuffFactor` and no mangled `KBuffFactor@@` / `KBuffInfo@@` symbol. So the only copy the linker can pick is `X2Lib`'s flagged one.
   - Re-check this if X2ServerProtocol ever starts copying `KBuffFactor` / `KBuffInfo`.
7. **Hooks: correct.** The 10 hooks and `32b87880` each appear in `JUNE2017_CHARACTER_PORT_CHECKLIST.md` §7. Argument orders match the calls above.

**New June bugs (rows added, all verbatim in the port):**
- **#36:** Moonlight Rhapsody erases its own buff inside the DamageReact loop. June's slot-4 dispatcher `32275650` walks the live vector with a cached end, like BOOST_FOREACH.
- **#37:** Atomic Blaster's `+= 1.0` lands in the attacker's shared DamageData, the same mechanism as #4.
- **#38:** Sinister Mind's ×1.2 keys off slot 45 `GetHyperModeTime`, which is non-zero only during the awakening state itself. March clears `m_iLastStateHyperModeCount` in `HyperModeEnd`.

**Also checked:**
- **#31 answered:** the DamageReact apply path hands over the attacker DamageData's own factor (`SetBuffFactorToGameUnit( **vItr )` → `PushNewBuffTempletToGameUnit` → `SetFactor`, no clone). Chain Viper's value[0] write therefore sticks for every later target of that DamageData.
- Servo Nitro's 721 has no custom function in June's templet, so its apply and erase inside the state loops cannot touch the vector.
- Geist's `DestroyInstanceHandle` takes the handle by reference and resets it, so FrameMove stops after one destroy.

**The user's answers (2026-10-05):** "for the questions, always follow June's flow, and when there is a potential bug, note it inside JUNE_BUGS.md". The project is offline-only, so server sync is not a concern. All three 9b questions are therefore answered by June and become **Phase 9c** (`kickoffs/june-buff/9c.md`).
- Death Sign's `+0x48` is June's pending sub-stage clear type: default 5, set by the clear REQ path `327f03c0`, reset by the ACK `327eec80`. 4 = `CT_GAME`.

**Not a finding:** Phase 13 makes these classes live. `kickoffs/june-buff/13.md` now carries a gate: the three questions above are to be answered before 13 ships data (Death Sign kills outright once 1038 is live).

### Phase 9c — Done (2026-10-05)

Flags `SERV_IRUHADEV_JUNE_BUFF_CUSTOM_FUNCTION` (items 1–2), and two new ones: `SERV_IRUHADEV_JUNE_BUFF_FACTOR_WIRE` (the `KBuffFactor` struct and wire, item 1) and `SERV_IRUHADEV_JUNE_BUFF_CLEAR_TYPE` (item 3). All use `_DEBUG`. Commit `508655f`. RE rows: `june2017_re/funcmap.tsv` "Buff Phase 9c". JUNE_BUGS #33 updated. All three 9b "Open for the user" items are **answered** (user, 2026-10-05: follow June's flow) and are ported below.

1. **Geist Herrschaft's power (9b question 1): ported, with the rest of June's creator round trip.**
   - `KBuffFactor` gets `m_fJuneCreatorPowerRate`. It defaults to 1.0 (ctor `3211ea50 @3211ea94`), and the copy ctor copies it.
   - Put/Get follow Get `320e3950`: uid after the identity, NPC flag before the factor ID, power last. June also carries an int `+0x35` and a float `+0x39`. Those are June-only. The ctor zeroes them and `GetFactor` (`32369f50`) does not write them; no other writer or reader was searched for. They are not carried. June's Put was not located.
   - The writer of `+0x57` is the templet's `GetFactor` (`32369f50 @3236a009`). Outside the accumulation test it writes UID, NPC flag and power from the templet's `+0xb0`/`+0xb8`/`+0xbc`. Porting it needed the two creator members 9a skipped, `m_uidJuneCreator` and `m_bJuneCreatorNotUser`:
     - ctor: -1 and 0 (`@326c499b`, `@326c49a7`);
     - copy: `322ad9cd`;
     - SetFactor: stored after the creator lookup, creator found or not (`3236d400`).
   - `SetFactorFromPacket` (`3236c440 @3236c4f0`) restores all three after the displayers. A restored buff therefore passes its creator on again.
   - The custom-function packet reader (`32e73f90`) now takes the power from `+0x57`, replacing the 1.0 stand-in.
   - **Offline path:** the Gage copies factors across games in memory. `GetFactor` → copy → `SetKBuffFactorPacketToGameUnit` is what carries the creator now; before 9c it arrived as UID 0, user, 1.0.
   - **Emulator:** `CX2OfflineBuff::GetBuffList` builds each `KBuffInfo` fresh, and `X2OfflineDB` stores only the factor ID. Item buffs get June's defaults (UID 0, user, 1.0). No schema change. Nothing in the emulator relays a client `KBuffFactor` back.
   - **9b correction (port error):** `CommonPacket.h` never sees an `Always.h` flag. `X2Main.h:4` reaches it through `X2ServerProtocolLib.h` → `SystemPacket.h`, before `KTDX.h` includes `Always.h`. 9b's copy-ctor change therefore compiled its `#else` and was dead code. The 9b Done note and the 9b review both said otherwise. The first build of 9c failed for the same reason: the member was missing, while `CommonPacket.cpp`'s Put/Get saw the flag.
   - **User decision (2026-10-05):** the struct and wire code moved to `_FACTOR_WIRE`, which is defined in `Always.h` **and** at the end of `ServerDefine.h` (`CommonPacket.h:16` includes it). CLAUDE.md's offline-only override now records the exception. `tools/flagcheck.py` gained `SECOND_HOMES` so the second define is checked like `Always.h`. The three power-rate lines in X2Lib nest `_FACTOR_WIRE` inside CUSTOM_FUNCTION.
   - **ABI:** X2ServerProtocol now compiles the same layout through `ServerDefine.h`. Its rebuilt `.lib` holds no `KBuffFactor@@` / `KBuffInfo@@` / `KUnitBuffInfo@@` / `KNpcUnitBuffInfo@@` symbol (dumpbin: 0 buff symbols), so it emits no code for the struct anyway. Other flagged shared headers (`Enum.h`, `DungeonEnum.h`, …) come after `KTDX.h` and are not affected.
2. **Sinister Mind (9b question 2, JUNE_BUGS #33): verbatim.**
   - The 0-step guard is gone. `count / ftol(max / 3.0)` divides as June does (`3236e506-3236e516`).
   - A `_DEBUG` line names the max just before a 0 divide. `KLOG << fileout` writes it out first.
3. **Death Sign's dungeon exemption (9b question 3): ported, with the sub-stage clear type it reads.**
   - **June's flow:**
     - The clear check (`327f0c10`) resets `+0x48` to 5 when it sends the REQ (`@327f127c`).
     - The ACK (`327eec80`, from vslot 157) resets it to 5 on success or refusal.
     - The clear NOT (vslot 158 → `327f03c0`) stores the type on every client, then runs the switch `327eee60`.
     - Lua's `ClearSubStage` (`327c9e60` → `327f05c0`, host only, type ≠ 0) stores it too.
     - For `CT_GAME` it stays 4 until the game ends.
   - **March port:**
     - March's `ClearSubStage` is the NOT and Lua step: it stores the type before the host test.
     - The reset sits before `PrepareClearSubStage`.
     - The ACK handler resets before it calls `ClearSubStage`.
     - Ctor default: 5.
     - The reader spares the user when `GT_DUNGEON` → dungeon → stage → sub-stage clear type == `CT_GAME`, and kills on any missing link. The flag is cleared first either way (`3206f8c9-3206f975`).
   - **Order (kickoff question):** the 9b review guessed that "the clear erases every buff". It does not. June's only user clear-all outside teardown is the die-state start (`3202a7d0`, via thunk `329cada0` in every class's `ENSI_DIE_FRONT`/`_BACK`). So the exemption covers any Death Sign erase (timeout or cure) after the final clear. OnFrameMove consumes the flag on the user's next frame.
   - **Not ported** (backlog, the rest of June's handshake):
     - the `+0x30` state machine (its state 4 also blocks March's `CT_STAGE_FORCE` re-check after a `CT_GAME` clear);
     - June's 5-int ACK and the NOT packet;
     - `327eec80`'s `CT_GAME` game writes (`+0xc50`/`+0xc4d`/`+0x3b9`/`+0x47c`) and its `CT_FAILURE` call.

     None of these is read by a buff.

**Hygiene** (`--worktree --files` the 11 code files, `--with JUNE_BUFF_DEBUG JUNE_BUFF_CUSTOM_FUNCTION JUNE_BUFF_CLEAR_TYPE JUNE_BUFF_FACTOR_WIRE`; each flag alone FAILs the files the others share):
- CUSTOM_FUNCTION: `files: 4 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 34 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
- FACTOR_WIRE: `files: 2 PASS, 2 PASS~, 2 DEFINE (Always.h, ServerDefine.h), 5 OTHER, 0 FAIL, 0 ERROR` / `30 deleted line(s)`, the rest 0 / `RESULT: PASS`.
- CLEAR_TYPE: `files: 4 PASS, 0 PASS~, 1 DEFINE, 6 OTHER, 0 FAIL, 0 ERROR` / `4 deleted line(s)`, the rest 0 / `RESULT: PASS`. On its own three files with only `_DEBUG`: `3 PASS … 0 deleted … RESULT: PASS`.
- **OTHER rows:** each is a file owned by one of the other two flags.
- **Deleted lines:** all are 9a/9b code rewritten under these flags:
  - Sinister Mind's guard (18 lines);
  - 9b's "not ported" and "Put/Get unchanged" comments;
  - 9b's copy-ctor `#ifdef` lines and initializer list, moved to `_FACTOR_WIRE` (`CommonPacket.h` 8);
  - the reader's two `SetNowHp` lines, now inside the `else` braces (`X2GUUser.cpp` 4);
  - the creator-info assignments, realigned (`X2BuffBehaviorTemplet.cpp` 4).
- After commit (`--range 508655f^..508655f`, same `--with`): CUSTOM_FUNCTION, FACTOR_WIRE and CLEAR_TYPE each give the same `files:` / deleted-line counts as above and `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,187,200 B, dated 2026-10-05 20:34:10, md5 bba821cf60ad5b4ac50f637658089d10, equal to X2/US_SERVICE/x2.exe (2026-10-05 20:34:05); built at mods/june-buff-port 05fefc2 + 11 uncommitted code file(s).` (the 11 are this phase's). Strings `reserved death spared`, `clear type %d -> %d`, `divide by zero next`, `clear ACK, June 327eec80` and `ClearSubStage, June 327f03c0` are each in the deployed exe once.

**How to see it in game:**
- Death Sign (1038) and Geist (1010) stay dormant until Phase 13, so the play-test is regression plus the new log lines:
  - 29 / 56 / 62 still create their classes;
  - the creator round trip (templet → `GetFactor` → Gage copy → restore) is **checked from source and by the clean link only**: the `custom function created` line logs once per BTI per launch, so a restore after a map change prints nothing;
  - a dungeon clear logs `clear type 5 -> 5 (clear REQ…)`, `5 -> 5 (clear ACK…)`, `5 -> n (ClearSubStage…)`, ending in `-> 4` at the last room.

**Play-test (2026-10-05, exe bba821cf, user: 4/4 passed; logs read).**
- ErrorLog `:195` BTI 29 `PhantomSword` (creator UID 25, not-user 0, power rate 1), `:212` BTI 56 `BigBangStream` (creator 18), `:225` BTI 62 `MarkOfCommand` (NPC, creator 22, not-user 1).
- Dungeon run: 9 room clears (`:643`–`:772`), each logged `5 -> 5` (REQ), `5 -> 5` (ACK), then `5 -> n` (ClearSubStage): n = 0 for sub-stage clears and 1 for stage clears. The last room went `5 -> 4` (`:772`, CT_GAME) and the dungeon ended normally.
- No `Death Sign` or `Sinister Mind` line, as expected while their BTIs are dormant. The run's 4 UNHANDLED `E_DISABLE_HB_CHECK_REQ` are not this phase's.

**Open for the user:** none. Backlog: June's sub-stage clear state machine (item 3, "Not ported"), and finding June's KBuffFactor Put (the review kickoff asks).

### Phase 9c — Review (2026-10-05, Opus 5.5 high)

**Verdict: no transcription errors, no code change.** The exe stays bba821cf; the 9c play-test (4/4) covers the regression, so there is nothing new to play-test. One tool fix (`b1b7e0d`, flagcheck) and JUNE_BUGS #41. The Ghidra reads were read-only. Every item below was checked against June directly, so it is more than a spot-check.

1. **`KBuffFactor` (`_FACTOR_WIRE`): correct.**
   - **The flag reaches `CommonPacket.h`.** A `cl /P` of X2Lib's `stdafx.h` lists every header before `Always.h`, and the struct there carries `m_fJuneCreatorPowerRate`. Only `CommonPacket.h` (the flag) and `ServerDefine.h` (the define) in that list name a `SERV_IRUHADEV_` flag. `CommonPacket.h`'s one mention of `_CUSTOM_FUNCTION` is in a comment.
   - **The ctor matches** `3211ea50` (`+0x57` = 1.0).
   - **The copy ctor matches** June's `31e8bfa0`, for every member the port carries.
   - **June's Put is `320cdc70`.** It mirrors Get `320e3950` field for field: uid `+0x28` after the identity, `+0x35` int, `+0x39` float, NPC flag `+0x3d`, factor `+0x3e`, power `+0x57` last. The port's order, minus `+0x35`/`+0x39`, matches.
   - **`+0x35`/`+0x39` are always 0 from the client.** Nothing reads or writes them, so they get **JUNE_BUGS #41**; the port does not carry them. Two searches:
     - a .text sweep for `[reg+0x35/0x39]` next to `+0x57`;
     - all 22 functions that call the ctor, copy ctor, Put or Get (stack copies included), plus `3236c440`.

     Only the ctor (zeroes) and the copy ctor / Put / Get touch them. The reader `32e73f90` reads `+0x28/+0x2c/+0x3d/+0x57` only.
   - June's members `+0x42..+0x53` are not on the wire either and are not ported.
2. **The templet's creator fields: correct at all five sites.**
   - **SetFactor `3236d400`:** `+0xbc` only when the slot-50 lookup finds the creator. `+0xb0/+0xb4` = uid and `+0xb8` = (isUser == 0) are stored whether or not it does (`@3236d45d-3236d476`).
   - **GetFactor `32369f50`:** returns at once if finished (`+0x65`), and runs the accumulation block only when the limit `+0x3c` ≥ 2. `@3236a009` is the label both paths reach.
   - **SetFactorFromPacket `3236c440 @3236c53d`:** runs after the displayer loop, on the success path only.
   - **Copy ctor `322ad9cd-322ad9ec`.**
   - **Ctor:** `+0xb0/+0xb4` = EAX = −1 (`or eax,-1 @326c4971`), `+0xb8` = BX = 0.
3. **ABI: confirmed.** `dumpbin /symbols X2Lib/X2ServerProtocol.lib` (the 9c rebuild, 20:24) shows 0 `KBuffFactor@@` / `KBuffInfo@@` / `KUnitBuffInfo@@` / `KNpcUnitBuffInfo@@` among its 6,100 externals. No other `SERV_IRUHADEV_` guard sits in a header before `KTDX.h` (the `cl /P` list above).
4. **Sinister Mind: correct.** June does `fild max; fdiv 3.0; ftol; idiv` with no check (`3236e4fc-3236e516`). The port's `_DEBUG` line runs before its divide.
5. **Death Sign / clear type: correct.**
   - The NOT `327f03c0` stores type/next/state before anything else, with no host test.
   - The REQ reset (`327f127c`) sits in the met-condition block, with state 1.
   - The reader is `GT_DUNGEON` = 2 (`cmp eax,2`), then four null links, then `+0x48 == 4` (`CT_GAME` = 4 in March's enum); any miss kills.
   - **Does the unported state machine change the answer? No.** June's clear check runs only while `+0x30 == 0`, so once a clear has run, nothing in June resets `+0x48`. March can overwrite a `CT_GAME` in two ways:
     - **(a) `CT_STAGE_FORCE` re-check:** the conditions are re-checked after `m_bDoneClearSubStage`, but in the same sub-stage only. A decompile census (scratchpad only) of March's three `CT_STAGE_FORCE` dungeons (Besma Secret Common and Hell, Peita Chapel Expert) puts it on stage 1 sub-stages 1–3 only, never with `CT_GAME`. June's dungeon scripts have no `CT_STAGE_FORCE` at all.
     - **(b) a Lua `ClearSubStage(0)`:** June's Lua path `327f05c0` skips type 0, and March stores it. Lua `ClearSubStage` appears only in 33 March tutorial, training and arcade scripts.

     Neither reaches a dungeon clear with Death Sign. Two minor divergences, recorded only:
     - June's ACK resets only when the ACK's stage/sub-stage match (`327c98d0`), and on a refusal only in state 1 (`327eec80`). March resets on every ACK, which is always the answer to a REQ.
     - June's Lua path is host only; offline, the player is always host.

**Hygiene.**
- **`flagcheck` bug, fixed in `b1b7e0d`.** `--range 508655f^..HEAD` first FAILed `Always.h` at `ea5f932` (the 11b review's DEATH_CLEAR block). In the define file, `classify()`'s others path never asked `define_file_only`. The same commit scopes `SECOND_HOMES` to its flag. A misplaced `#define` already failed RESULT through `bad_sites`; now the row says FAIL too. Selftest +4 cases, PASS. DEATH_CLEAR alone still PASSes.
- **After the fix**, `--range 508655f^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_CUSTOM_FUNCTION JUNE_BUFF_CLEAR_TYPE JUNE_BUFF_FACTOR_WIRE`:
  - CUSTOM_FUNCTION: `files: 4 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 9 OTHER (other flags), 0 FAIL, 0 ERROR` / `in this flag's files: 34 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)` / `RESULT: PASS`.
  - FACTOR_WIRE: `files: 2 PASS, 2 PASS~, 2 DEFINE, 9 OTHER, 0 FAIL, 0 ERROR` / `30 deleted line(s)`, the rest 0 / `RESULT: PASS`.
  - CLEAR_TYPE: `files: 4 PASS, 0 PASS~, 1 DEFINE, 10 OTHER, 0 FAIL, 0 ERROR` / `4 deleted line(s)`, the rest 0 / `RESULT: PASS`.
  - With only `_DEBUG`, on its three files: `files: 3 PASS … 0 deleted line(s) …` / `RESULT: PASS`.
- **OTHER rows:**
  - each 9c file owned by one of the other two flags;
  - `ServerDefine.h` for the two flags it does not define;
  - `ea5f932`'s four files (DEATH_CLEAR, a peer's).
- **Deleted lines:** the same as 9c's Done note explains (guard 18, the 9b comments, the copy-ctor move, `SetNowHp` braces, realignment); counts unchanged.

**Open for the user:** none. Backlog unchanged: June's sub-stage clear state machine (9c item 3). `kickoffs/june-buff/13.md` gates on 9c, which is done. It now also names the Sinister Mind crash to expect and the two clear-type divergences.

## Phase 10 — Buff groups and June-only script bindings

**Scope.**
- Load `UnitApplyBuffGroup.lua` with `AddBuffGroup_LUA` and `AddOfficiallyBuffList_LUA`. Follow June: it likely replaces March's hard-coded `m_vecPossibleOfficiallyDebuffList` (`X2Game.cpp:19051`).
- Add the bindings `SetMonsterBuffGroupID`, `DeleteNonControlDebuff`, `GetBuffAccumulationCountNow`, `ApplyBuffFactor` and `AddRandomBuffList`, plus `SetPveBalanceBuffGiveScore_LUA` and `SendRandomMissionRewardBuffCheat` if June wires them on the client.
- Add the damage-data and NPC keys located in Phase 1: `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID_RELATIVE_*`, `BUFF_FACTOR_ID_ULTIMATE(_PVP)`/`_PVP`, `DELETE_BUFF_WHEN_DIE`, `DUMMY_BUFF_TEMPLET_ID`, `IS_AURA_BUFF`, `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`, `CHECK_RIDINGPET_SPACIAR_BUFF`.
- Stage `UnitApplyBuffGroup.lua` as a **new** file in `march2014/update/data036/` **and** `kom_contents/data036_kom/` + `_manifest.json`.

**Run with:** Opus 5.5, high effort. Review required.

**Kickoff:** `kickoffs/june-buff/10.md` - start it with `/x2-phase june-buff 10`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

### Phase 10 — Done (2026-10-05)

**Landed** in `9eba9ad` (18 code files, 905 insertions, 0 deletions). The RE is in the "Buff Phase 10" rows of `june2017_re/funcmap.tsv` (23 rows; `31fceab0` promoted to confirmed).
- **`SERV_IRUHADEV_JUNE_BUFF_GROUP`** (`9eba9ad`):
  - `CX2BuffTempletManager::OpenScriptFileForBuffGroup` (June `326c26f0`) runs `UnitApplyBuffGroup.lua` right after the factor files and logs `N buff group(s), M officially debuff(s)`, or that the file is missing. June's checksum tail is anti-tamper (`31c59710`), not a sort: not ported.
  - `AddBuffGroup_LUA` (`326c3450`), `AddOfficiallyBuffList_LUA` (`326bfd80`, clears the set per call), `IsApplyBuffByGroup` (`326c31c0`), `IsOfficiallyDebuff` (`326bfd50`).
  - `CanApplyBuffToGameUnit` takes the factor's BTI (June slot 132) in all three classes. The BTI is passed at June's sites: `SetBuffFactorToGameUnitByBuffFactorID` (and `…AndCreatorID`), `SetBuffFactorToGameUnit(skill)`, `SetBuffAccumulationCount`, the damage-react apply. The class files pass none, as in June.
  - `CX2GUNPC` (`327bdb60`): `BUFF_GROUP_ID` read after `BOSS_RAID_NPC` in `INIT_COMPONENT` (`327ab348`), `SetMonsterBuffGroupID` (`31e4d770`), 0 in the ctor. With a group, the group's answer replaces the base check (JUNE_BUGS).
  - The officially-debuff set replaces `CX2Game::SetOfficiallyDebuffList` in `EraseDebuffAndAddMP` (`3204d440`: walks the unit's buffs, erases every member) and Ara's `GetReduceDebuffDurationRate` (`32bd32f0`). Its skill-27415 tail went to the character checklist §7.
- **`SERV_IRUHADEV_JUNE_BUFF_REACT_APPLY`:** June's `ApplyBuffToGameUnitInDamageReact` (`322a88c0`) whole: per factor can-apply(BTI) → key check (`3227a960`, passes until effect C3 registers `DAMAGE_DATA_KEY_NAME`) → immunity list → pre-apply roll (`322a7960`: its own RESIST roll; 265/266 under 347/348, and 347/348 go to a server event March lacks) → `SetBuffFactorToGameUnit`; true only when one applied.
- **`SERV_IRUHADEV_JUNE_BUFF_BINDING`:** `DeleteNonControlDebuff`, `GetBuffAccumulationCountNow` (`3227bbc0`), `g_pGameEdit:ApplyBuffFactor(BFI, time)` (`3274c880`) with `CX2BuffTimeFinalizerTemplet::SetDurationTime` (`32ec3580`).
- **`SERV_IRUHADEV_JUNE_BUFF_FACTOR_ID`:** June's `ParsingBuffFactorID` (`32630d20`): `APPLY_LEARNED_SKILL_BUFF_FACTOR_ID` (gates `31fceab0`, `HaveThisBuff`), `BUFF_FACTOR_ID_ULTIMATE(_PVP)` behind the skill-memo gate `31fcf3e0` (June's skill→memo map, absent; forced off like `X2DamageEffect`'s), `BUFF_FACTOR_ID_PVP`. The level rules are March's (June matches them).
- **`SERV_IRUHADEV_JUNE_BUFF_DAMAGEDATA_KEY`:** `IS_AURA_BUFF` (`+0x5f0`) and `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT` (`+0x604`, −1) parsed and stored; readers are **Phase 10b** (new section below).
- **Staged:** `UnitApplyBuffGroup.lua` (mar-2014, `1B 4B 4C 81`, 4,909 B; 3 groups, 109 officially debuffs, all 113 BTI names resolve in the staged Enum) in `march2014/update/data036/` and `kom_contents/data036_kom/` + both `_manifest.json`.

**Visible side effect (June's, kept):** the damage-react apply now returns true only when a factor really applied (March: when one passed the immunity list). `CX2GUUser`'s hit code (`X2GUUser.cpp` `bBuffApplied`) uses it to fire the socket effects `SIEE_STAT_UP` / `SIEE_UP_MANA_RECOVERY_VALUE`, so those now trigger only when a debuff lands, not on a resisted one.

**Matrix:** `census.py` rerun (`cc1ad03`): every Phase 10 key is `implemented`; `BUFF_FACTOR_ID_MEMO` / `_RELATIVE_MEMO_ID` stay `june-ignores` (kept by the memo decision).

**Recorded, not ported** (no March home; funcmap rows): `AddRandomBuffList`/`DUMMY_BUFF_TEMPLET_ID` (June's CustomEventManager, `3267c370`, and its branch in `322a88c0`); `CHECK_RIDINGPET_SPACIAR_BUFF` (transforming riding pet, `32d2f3d0`); `SetPveBalanceBuffGiveScore_LUA` (PvE mode); `SendRandomMissionRewardBuffCheat` (server event 0x497); `3204e6a0` (no callers). `DELETE_BUFF_WHEN_DIE` was already ported in 4b.

**Correction:** `31fceab0`'s other exit tests the skill's `m_ePowerRate` (templet `+0x174`) against 5 = June's `SPRT_HYPER_CLASS`, absent from March's enum (5 = `SPRT_END`); it cannot fire here. `+0x178` (6b's memo/base id) is written separately, so there is no conflict with 6b.

**Decision for the user (resolved in the review: on, `e6757fb`):** June removed March's `BUFF_FACTOR_ID_MEMO` / `BUFF_FACTOR_RELATIVE_MEMO_ID` branch (no such key in June's binary) for `_ULTIMATE`, whose gate is off here. March data uses it in `DamageEffect`, `Elsword_SwordMan`, `RAVEN_FIGHTER`. It is **kept** (the effect plan kept March's memo reads); `#define SERV_IRUHADEV_JUNE_BUFF_MEMO_DROP` in `Always.h` drops it.

**JUNE_BUGS.md:** the group replaces the base check; the unchecked templet in `326c31c0` (guarded here: a crash); `_ULTIMATE_PVP` skips the found test; the two independent rolls.

**Hygiene** (`flagcheck --range 9eba9ad^..9eba9ad`, each flag `--with` the other four + `JUNE_BUFF_DEBUG JUNE_BUFF_MEMO_DROP`):
- `_GROUP`: `files: 0 PASS, 12 PASS~ (blank/marker only), 1 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR`; `RESULT: PASS`.
- `_REACT_APPLY`: `files: 0 PASS, 2 PASS~, 1 DEFINE, 15 OTHER, 0 FAIL, 0 ERROR`; `_BINDING`: `0 PASS, 6 PASS~, 1 DEFINE, 11 OTHER`; `_FACTOR_ID`: `0 PASS, 1 PASS~, 1 DEFINE, 16 OTHER`; `_DAMAGEDATA_KEY`: `0 PASS, 2 PASS~, 1 DEFINE, 15 OTHER`; each `0 FAIL, 0 ERROR`, `RESULT: PASS`.
- Every flag: `0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`. The OTHER rows are this phase's other four flags.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,141,120 B, dated 2026-10-05 11:37:26, md5 8b491d7d1a8108c1a208ad94ae196580, equal to X2/US_SERVICE/x2.exe (2026-10-05 11:37:21); built at mods/june-buff-port f52a21b + 18 uncommitted code file(s).` (those 18 are exactly `9eba9ad`). All 13 phase strings found in the deployed exe.

**Play-test (2026-10-05, user; exe `8b491d7d`, run 11:59:24 after the 11:37 deploy, live `data036.kom` has `UnitApplyBuffGroup.lua` 4,909 B): 4/4 passed.** `ErrorLog.txt:152` `UnitApplyBuffGroup.lua: 3 buff group(s), 109 officially debuff(s)`; `:191` `buff group 4 refused BTI 37 on NPC 5086` (Sword Fire, the damage-react path); `:193` `buff group 3 refused BTI 37 on NPC 5084` (console); group 0 burned both times; the `ApplyBuffFactor` cheat burned for about 3 s (by eye, no log line). Load baseline unchanged (361 factors, 5 ordinal rejects); 0 exceptions. A first run (11:54) had the kom packed but not copied into the game directory, so it logged `not found`.

### Phase 10 — Review (2026-10-05)

**Verdict: Phase 10 matches June. No transcription errors in live code.** One comment was wrong, and one engine function was missing; both are fixed under `SERV_IRUHADEV_JUNE_BUFF_GROUP`. Ghidra checks, all read-only: `327bdb60`, `326c31c0`, `326c3170`, `3229a2e0`, `322a88c0`, `322a7960` (its switch bodies read from the exe bytes, `@322a7b1e-7b7e`), `32630d20`, `31fceab0` / `32205590`, `326c3450`, `326bfd80`, `3204d440`, `3274c880`, `322a6960`, `3229af40`, `3229b0a0`, `32fe9660`, plus the key reads `@3263f050` (`+0x5f0`), `@326404ba` (`+0x604`), `@327ab406` (`+0x8f70`) and `IS_DEBUF` `@3236accd` (templet `+0x64`).

**Kickoff checks.**
1. **Group decision.**
   - The default switch: 0/1 yes, 2 `!debuff`, 3 debuff, 4 no, and 5 and above no (the result byte is preset to 0).
   - A listed BTI returns its map value. No group with this id returns 1.
   - `327bdb60` overwrites the base result with the group's answer (JUNE_BUGS #17).
   - Templet `+0x64` is `IS_DEBUF`: the templet parser writes it there (`lea edi,[ebp+0x64]`).
2. **Slot-132 callers.**
   - An `.text` scan for `mov reg,[reg+0x210]` followed by `call reg` finds June's 12 game-unit sites; the other 5 hits are UI or field objects.
   - The five BTI sites (`322a6960`, `322a6b60`, `322a7660`, `3229af40`, `322a88c0`) each pass the factor's or the clone's `+0x28`, as the port does.
   - The six class-file sites each `push 0`.
   - **`3229b0a0` has no March counterpart.** It is `3229af40` with the level as an argument instead of a skill-tree read; its only caller is Rose `32ababc0 @32abaec8` (level 2). It is engine code, so it is now **ported dormant** as `CX2GameUnit::SetBuffFactorToGameUnitAtLevel`, and the Rose call site went to the character checklist §7.
3. **`322a88c0` / `322a7960`.**
   - The per-factor order is as ported: 132, `3227a960`, immunity, CustomEvent branch, 139, 141, 137.
   - `322a7960` returns 0 for a null templet, above-rate and RESIST (`XOR AL,AL @322a7ba5`). It returns 1 for 265/266 with 347/348 unfinished, and for 347/348 after event `0x71c` (`MOV AL,1 @322a7b4b`, `@322a7b6c`).
   - The RESIST draw and `GetRate(1)` = `m_vecRate[0]` match the 4a port of `322a83b0`.
   - Slot 141 is empty for every March class.
4. **`ParsingBuffFactorID`.**
   - The order is learned, then `_ULTIMATE_PVP`/`_ULTIMATE`, then `_PVP`/plain, then the level. Every list skips 0, and a `BeginTable` success counts as found even when the list is empty.
   - `31fceab0`'s other exit is `+0x174 == 5` (`32205590`); March's enum ends `SPRT_END = 5`.
   - March's memo branch is transcribed faithfully: a memo with no `BUFF_FACTOR_ID_MEMO` table yields nothing, as in March.
5. **Loader.**
   - The presets are BTI 0 / apply 1. The two ints default to 0. The set is cleared per call.
   - **Comment corrected:** the missing `EndTable` is not "dropped by the KLuaManager destructor". June's `32fe9660` and March's `DetachLuaState` only unref. The table goes when the C call returns to Lua (the void functor returns 0), so it is harmless.
   - The checksum tail is anti-tamper, as the Done note says.
6. **`3204d440`** walks `+0xe14`, erases every member, and does not advance after an erase. Ara's 27415 tail is in §7.

**Also checked.**
- No header outside the three changed classes declares `CanApplyBuffToGameUnit`, so nothing silently stops overriding it.
- `GetOfficiallyDebuffList` has no reader left outside the two `#else` branches.

**Recorded, not ported.** User slot 476 (`32028250`, skill-memo scaling of a factor) was handed to Phase 10 by the 4b-i correction, and Phase 10's Done note did not say what became of it.
- June calls it from `322a6960`, `3229af40` and `3229b0a0` as well as `322a7660`.
- It needs June's skill-memo table (`31fdbbb0`), which March lacks, the same reason the `_ULTIMATE` gate is off.
- The port keeps the `322a7660` call, which logs once and skips, and adds no others. It belongs with June's skill-memo system.

**New June quirk:** JUNE_BUGS #21. The key check runs per factor, so once C3 registers keys, a keyed damage data applies only its first factor.

**Hygiene.**
- Phase 10's five flags (`--range 9eba9ad^..HEAD`, each `--with` the other four + `JUNE_BUFF_DEBUG JUNE_BUFF_MEMO_DROP`), before this review: every `RESULT: PASS`, `0 deleted line(s)`. `_GROUP`: `0 PASS, 12 PASS~, 1 DEFINE, 5 OTHER, 0 FAIL, 0 ERROR`. `_REACT_APPLY`: `0/2/1/15`. `_BINDING`: `0/6/1/11`. `_FACTOR_ID`: `0/1/1/16`. `_DAMAGEDATA_KEY`: `0/2/1/15`. The OTHER rows are the other four flags.
- This review: `e107d9b` (`_GROUP`, `--range 9eba9ad^..HEAD`, same `--with`): `files: 1 PASS, 14 PASS~ (blank/marker only), 1 DEFINE, 5 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 2 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 2 deleted lines are the two `AddBuffGroup_LUA` comment lines that `9eba9ad` itself added inside this flag and `e107d9b` rewrote.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,141,632 B, dated 2026-10-05 12:54:57, md5 58398ae287e54921625ae284e1d4934f, equal to X2/US_SERVICE/x2.exe (2026-10-05 12:54:53); built at mods/june-buff-port bc77561 + 3 uncommitted code file(s).` (those 3 are exactly `e107d9b`).

**Play-test:** none. `SetBuffFactorToGameUnitAtLevel` has no caller and the other edit is a comment, so nothing in game changes.

**`_MEMO_DROP` decided (user, 2026-10-05: "follow what June offers"): on, `e6757fb`.** March's `BUFF_FACTOR_ID_MEMO` / `BUFF_FACTOR_RELATIVE_MEMO_ID` branch is gone, as in June. A skill with its memo equipped now takes `BUFF_FACTOR_ID` (March data: `RAVEN_OVER_TAKER_CHARGED_BOLT_BLOOD` in `DamageEffect`, and entries in `Elsword_SwordMan` and `RAVEN_FIGHTER`). June's replacement `_ULTIMATE` stays off (no skill-memo map). The effect plan's memo reads in `X2DamageEffect` (`APPLIED_SKILL_ID_NOT_AFFECT_MEMO`, the alternate damage data) are a separate system and do not change.
- Hygiene (`--range 9eba9ad^..HEAD`, `--with` the other five + `JUNE_BUFF_DEBUG`): `files: 0 PASS, 1 PASS~ (blank/marker only), 2 DEFINE, 16 OTHER (other flags), 0 FAIL, 0 ERROR`; `4 deleted line(s)` (the block's old OFF comment and the commented-out `#define`, rewritten), 0 non-ASCII, 0 EOL-mismatched, 0 encoding change(s); `RESULT: PASS`.
- `DEPLOY OK: X2_offline.exe is 19,141,120 B, dated 2026-10-05 13:11:12, md5 0b06cc942de046ab571c58019de43230, equal to X2/US_SERVICE/x2.exe (2026-10-05 13:11:07); built at mods/june-buff-port 48c3072 + 1 uncommitted code file(s).` (that file is exactly `e6757fb`; 512 B smaller than `58398ae2`, the dropped branch).
- Not play-tested: it shows only with one of those memos equipped, and the result is the plain buff, the same as without the memo.

**Owed by the user:** /cost.

## Phase 10b — The DamageData buff keys' readers (added 2026-10-05 by Phase 10)

Phase 10 parses and stores `IS_AURA_BUFF` and `LIMIT_APPLY_BUFF_COUNT_EACH_UNIT` (flag `SERV_IRUHADEV_JUNE_BUFF_DAMAGEDATA_KEY`); their readers are this phase's.

**Scope.**
- **`IS_AURA_BUFF`** (DamageData `+0x5f0`, bool). June reads it in the damage manager's team checks: `32610897`, `32610df0`, `32622990`, `32623110`, `32624610`, `326250fa`, `32625670`, `32628890`, `3262b6e0`, `326413c0`, plus `320471d6` (CX2GUUser) and `327b1f30` (CX2GUNPC::DamageReact `@327b3122`). In `32622990` an aura buff reaches a target only when the target is a user (slot `0x198`), the party manager exists (`g_pData+0x5c`) and the attacker's UID is in the party (`32431360`), or attacker and target are the same unit. Map each reader to its March function, then port the gate. June data: `DamageEffect_Hero_Dungeon_Aura_Buff` (`DamageEffect_ETC`).
- **`LIMIT_APPLY_BUFF_COUNT_EACH_UNIT`** (DamageData `+0x604`, int, −1 = none) and its per-unit list (`+0x608`/`+0x60c`: `{count, unit type, UID}`). `3227a270` counts a hit per unit (called from `32275990`, which compares the count with the limit before a buff applies; read `@322759c3`, `@32275a16`). Decide where the list lives in March's DamageData (it is copied per hit; check whether June's copy shares it) and port both. June data: Rose's `DamageEffect_SA_RFY_M18A1_Claymore_Project` only (Rose is absent), so it stays dormant until Rose arrives.
- Out of scope, recorded by Phase 10: `CHECK_RIDINGPET_SPACIAR_BUFF` (June's transforming riding pet, `32d2f560`/`32d2f3d0`; March has no Bumblebee pet), `AddRandomBuffList`/`DUMMY_BUFF_TEMPLET_ID` (June's CustomEventManager, `3267c370`, `322a7d50`), `3227a960`'s key-name dedupe (effect plan C3).

**Run with:** Opus 5.5, high effort. Review required.

**Kickoff:** `kickoffs/june-buff/10b.md` — start it with `/x2-phase june-buff 10b`.

### Phase 10b — Done (2026-10-05)

**Landed** in `a9ff45a` (`X2DamageManager.cpp`, `X2GUUser.cpp`, `X2GUNPC.cpp`, `Always.h`). The RE is in the "Buff Phase 10b" rows of `june2017_re/funcmap.tsv` (15 new rows; `32622990` and `3227a270` corrected).

**Reader census.** A byte scan of `.text` for every byte read of `[reg+0x5f0]` found the scope's 12 sites plus three: `32610897` = `32610880`, an uncalled out-of-line copy of `32610df0`'s guard; `31e9ca7f`, the DamageData copy; and `325c99xx`/`325cbxxx`, a different class (`+0x244`). Slot `0x198` is **IsMyUnit** (not "is a user"), so the gate only filters this client's own unit as a target.
- **`SERV_IRUHADEV_JUNE_BUFF_AURA_PARTY`:** June's party gate, inlined at all seven hit paths in front of the team check: GroundHit `@32623c5d`, AreaHit `@32624885`, UnitToUnit `@326250fa`, MeshToUnit `@326257d7`, CollisionDataToUnit `@326289e3`, EffectToUnit `@3262b89e` (attacker = the effect's owner), UnitToEffect `@326414ad` (target = the hit effect's owner). An aura buff skips my own unit when the party manager exists and the attacker's UID is neither in my party (`32431360`) nor my own. One helper, `JuneAuraBuffSkipsTarget`. GroundHit has no self exception (JUNE_BUGS #22), and its NULL attacker is guarded (#24).
- **`SERV_IRUHADEV_JUNE_BUFF_AURA_REACT`:** both DamageReacts (`320471d6`, `327b3122`) play the hitted-by-buff effect sets only for a non-aura hit.
- **`SERV_IRUHADEV_JUNE_BUFF_GUILD_REVERSE`:** `32610df0` is June's factored guild-skill **Chance to Reverse** check (`3202dd70` = `GuildSkillProcess`, `0x1fd` = `SI_GP_COMMON_CHANCE_TO_REVERSE`, `3202a890` = `DoGuildSkillChanceToReverse`). It is skipped for ATTACK_OUR_TEAM, IS_AURA_BUFF and DT_BUFF damage, and checks the attacker first. June calls it on a miss (March's four `GUILD_SKILL_PART_2` blocks, now in `#else`) **and on a hit right after AttackResult** in UnitToUnit, MeshToUnit, CollisionDataToUnit and EffectToUnit. It was ported as `JuneGuildSkillChanceToReverse` at all eight sites.
- **`_DAMAGEDATA_KEY` (Phase 10's lines):** its two comments and two parse log strings no longer say "Phase 10b" (these are the 5 deleted lines below).

**Not ported, with reasons.**
- `32622990` (team pre-filter): its first test is `IsPve` (`322cc1c0`), which needs June's PvE mode. Every caller repeats the gate inline.
- **LIMIT_APPLY_BUFF_COUNT_EACH_UNIT is dead in June.** The checker is `322759b0`, not `32275990`, which is the previous function's tail. Neither it nor the counter `3227a270` has a call, jmp, jcc, short jump or pointer anywhere in the binary. June parses the key and never enforces it, and Phase 10 already matches that. The per-unit list question settles itself: June's DamageData copy deep-copies the vector (`32388c70`), but nothing reads or writes it, so March gets no list member (JUNE_BUGS #23).

**Visible side effects.**
1. A guild member with Chance to Reverse learned now rolls it on every hit they land or take, as well as on a dodge. No character in `els_db.sql` has it (offline has no guild skills), so here the only trace is a one-time log line.
2. Nothing in March data sets `IS_AURA_BUFF`, so the gate and the effect-set skip are dormant until June's `DamageEffect_Hero_Dungeon_Aura_Buff` (data Phase 13, socket aura Phase 16) ships. Even then, offline has no party members or other players, so only the self case is reachable.

**Hygiene** (`flagcheck --worktree`, the four files, each flag `--with` the other three + `JUNE_BUFF_DEBUG`; `--range` after the commit below):
- `--range a9ff45a^..a9ff45a`, every flag `RESULT: PASS`, `0 FAIL, 0 ERROR`, 0 non-ASCII inserted, 0 EOL-mismatched, 0 `#else`/`#endif` without the flag token, 0 encoding changes.
- `_AURA_PARTY`: `files: 0 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags)`; `_AURA_REACT`: `0 PASS, 2 PASS~, 1 DEFINE, 1 OTHER`; `_GUILD_REVERSE` and `_DAMAGEDATA_KEY`: `0 PASS, 1 PASS~, 1 DEFINE, 2 OTHER`. The OTHER rows are the other 10b flags sharing `X2DamageManager.cpp`.
- `5 deleted line(s)` in the X2DamageManager.cpp rows: Phase 10's own comments (3 lines) and two log strings inside `_DAMAGEDATA_KEY`, rewritten. No studio line is deleted (March's guild-skill blocks stay in `#else`).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,142,656 B, dated 2026-10-05 13:41:08, md5 3e0330222c25ecf5199c7ffd240359e0, equal to X2/US_SERVICE/x2.exe (2026-10-05 13:41:04); built at mods/june-buff-port 67bd730 + 4 uncommitted code file(s).` (those 4 are exactly `a9ff45a`). All 9 phase strings checked in the deployed exe.

**Play-test (2026-10-05, user; exe `3e033022`, run 13:46:30 after the 13:41 deploy): passed, regression (melee, projectile, being hit).** `ErrorLog.txt:192` `guild Chance to Reverse checked on a hit too (first seen): attacker 25, target 18` (jtLordKnight's melee). No `aura buff skipped` and no `aura hit on` lines, as expected with no `IS_AURA_BUFF` data; no `not checked` line this run. Load baseline unchanged (361 factors, 5 ordinal rejects, 3 groups / 109 debuffs); 0 exceptions. The 3 `E_DISABLE_HB_CHECK_REQ` UNHANDLED packets are not this phase's.

**Owed by the user:** /cost. Next: `/x2-phase june-buff 10b review` (Opus 5.5, high).

### Phase 10b — Review (2026-10-05)

**Verdict: 10b matches June. No transcription errors.** One fix in our own log line (below) and one new JUNE_BUGS row (#25). Ghidra checks were read-only, plus an independent capstone scan of `.text` and a raw scan of the whole file.

**Kickoff checks.**
1. **Aura gate, seven sites.** Each was disassembled: `IsMyUnit` (vslot `0x198`) on the target, `g_pData+0x5c`, `IsMyPartyMember` (`32431360`) on the attacker's UID, then the same-UID compare, which is absent only in GroundHit (`JZ` straight to `continue` `@32623c98`, #22).
   - EffectToUnit's attacker is `31f8b560(effect)` (`+0x1dc`).
   - In UnitToEffect, `EDX = [effect+0x1dc]` is the hit effect's owner, the same optr `31f8b560` reads for the self compare. Its attacker is the lambda's `+8` capture (`[ESP+0x190]+8`; `+4` is the DamageData).
   - Order in UnitToUnit: `32622990`, then `+0x540`, then the gate, then `+0x50d`, then `+0x500` (ATTACK_OUR_TEAM). March's gate sits right before its DUNGEON_ITEM team block, which is the same place. The filters that come earlier only `continue`, so their order does not matter.
2. **Reader census (own scan).** I decoded every `F0 05 00 00` at every start 1-10 bytes back (143 decodes). The non-stack byte reads are exactly 10b's set plus `3263f050` (the parse's `lea`). `31f8f3c9` is a zero store in an init run.
   - June's CEffect embeds its DamageData at `+0x200`, so I also scanned `+0x7f0`. Its one byte read, `324d863f`, is `this+0x7f0` at the entry of `324d8600`. That function is vslot `0xe0` of `CX2StateDungeonGame`/`TrainingGame`/`PVEGame` (RTTI), a state member. Its next call, `32430b80`, is "party UID > 0". **No DamageData reader is missing.**
3. **`32610df0`.**
   - Guards in order: dd NULL, `+0x500`, `+0x5f0`, `[dd] != 6`. Then the attacker, then the target, each with `+0x264 == 0` (March `GUT_USER = 0`) and `0x1fd` (March `SI_GP_COMMON_CHANCE_TO_REVERSE = 509`). `RET 0xc`.
   - Vslot `0x290` of `334781f4` reads `3202dd70`.
   - `xrefs_to` gives exactly the eight sites. UnitToUnit pushes `(dd, attacker, defender)`. The EffectToUnit miss call passes `31f8b560(effect)`, like March's `pAttacker->GetOwnerUnit()`.
   - **`pAttackUnit` can differ from June.** June's hit call reads the owner at `@3262f524` whether or not `IsLiveInstance` at `@3262f511` passed. If DamageReact removed the effect, June reads a removed instance and gets NULL or garbage. While the effect lives, the two are the same unit. The port's value is the safe one; JUNE_BUGS #25 (latent).
   - **Hit log: guarded.** The helper treats both pointers as nullable, and the log fires when either side is a user, so a NULL partner would have crashed the log line. Both UIDs now use `NULL != p ? p->GetUnitUID() : (UidType)-1`, the aura helper's idiom (`_GUILD_REVERSE`). Every current site passes non-null for both.
4. **DamageReacts.** The scan has no other `+0x5f0` read in `320462a0` or `327b1f30`. Only `PlayEffectSetHittedByBuff` is gated.
5. **LIMIT dead code.** `3227a270`, `322759b0` and `32610880` are all function starts (CC padding; `322759b0` follows a `ret`). A raw scan finds no `E8`/`E9`/`0F 8x` rel32, no short `EB`/`7x`/`E0-E3`, and no absolute pointer anywhere in the file, so there is no vtable slot either. Not porting them matches June.
6. **`32622990`.** Its first test is `322cc1c0`, which is `vslot 0x10c (GetGameType) == 4`. March's `GAME_TYPE` ends at `GT_BATTLE_FIELD = 3`. `GT_NUMS` (4) is used only as a bound (`X2Main.cpp:680`, `:16399-16443`), and `m_GameType` is only ever set to PVP, DUNGEON or BATTLE_FIELD. **There is no March path.**

**Hygiene.**
- 10b, `--range a9ff45a^..a9ff45a`, each flag `--with` the other three + `JUNE_BUFF_DEBUG`:
  - `_AURA_PARTY`, `_GUILD_REVERSE`, `_DAMAGEDATA_KEY`: `files: 0 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags), 0 FAIL, 0 ERROR`.
  - `_AURA_REACT`: `0 PASS, 2 PASS~, 1 DEFINE, 1 OTHER`.
  - All four: `0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
  - OTHER rows: `X2GUUser.cpp`/`X2GUNPC.cpp` belong to `_AURA_REACT`, and `X2DamageManager.cpp` to the other three.
  - The `5 deleted line(s)` in the `X2DamageManager.cpp` flags are Phase 10's own `_DAMAGEDATA_KEY` comments (3) and log strings (2), rewritten.
- This review, `_GUILD_REVERSE` `--worktree --files X2Lib/X2DamageManager.cpp`: `files: 1 PASS, 0 PASS~, 0 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`. The `1 deleted line(s)` is 10b's own hit-log argument line, rewritten. `0 encoding change(s)`, `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,142,656 B, dated 2026-10-05 14:00:02, md5 826c409ee382e8340839004ed49926f8, equal to X2/US_SERVICE/x2.exe (2026-10-05 13:59:57); built at mods/june-buff-port fc51e7d + 1 uncommitted code file(s).` That file is exactly this review's commit.

**Play-test (2026-10-05, user; exe `826c409e`, run 14:02:22 after the 14:00 deploy): passed.** `ErrorLog.txt:192` `guild Chance to Reverse checked on a hit too (first seen): attacker 25, target 4` (both UIDs real). Load baseline unchanged (361 factors, 5 ordinal rejects, 3 groups / 109 debuffs); 0 exceptions; the 2 `E_DISABLE_HB_CHECK_REQ` UNHANDLED are not this phase's.

**Owed by the user:** /cost. Next: `/x2-phase june-buff 11` (Opus 5.5, high).

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

**Kickoff:** `kickoffs/june-buff/11.md` - start it with `/x2-phase june-buff 11`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

### Phase 11 — RE (2026-10-05)

The function rows are in `june2017_re/funcmap.tsv` (the "Buff Phase 11" rows).

**Parse: no diff.**
- June's `ParsingScript` (`326c6040`) and `AddBuffInfo_LUA` (`326c78f0`) are March's.
- The only difference is the load: June also loads `BuffTemplet_INT.lua` (`3258c470`).
- All five consumers of the premium templet match March one for one.

**`NOT_USE_IN_PVE` / `_HENIR` / `_HUGE_BOSS_RAID` and `m_bEndBuffAt*` are server-only.**
- There are 0 hits in the June exe, raw in ASCII or UTF-16, and none among the 6,241 decrypted XOR strings.
- The bare `NOT_USE` key belongs to the behavior parser (`32d4bbf0`).
- "PVE" is June's game type 4 (`GT_PVE`, `3231f4d0`), which March lacks.

**Apply: both world-buff handlers are rewritten in June.**
- `326c9030` (in room) and `326c9530` (village) replace March's `vector<BUFF_FACTOR_ID>` with a 40-byte record per buff at `CX2Unit+0xdbc`: factor, BTI, stack count, start, end, and "lasts at most 1 h".
- Each buff is applied `count` times.
- New skips:
  - `BFI_EMPTY_EXP_BUFF` in battle fields and `GT_PVE` (village: always);
  - `BFI_BUFF_2013_DEFENSE_BUFF` in PvP;
  - `BFI_SPA_STRONG_BUFF` in June state 7.
- Hooks:
  - returning hero (500 = comeback end time, 114 = emblem);
  - Rose event (10060 = emblem);
  - PC room (BTI 138 change → socket refresh);
  - guild agit (BTI 1033).
- The erase loops dereference the factor pointer unchecked.
- The village icon carries the stack count.

**Wire: no change needed.**
- June's `KBuffFactor` (`320e3950`) adds three fields, but every field the handlers read has a March member: BTI `+0x20`, count `m_ucAccumulationCountNow`, factor `m_iFactorID`, start/end.

**Readers of the record vector:**
- `ReInit` (`32022380`) re-applies each buff `count` times; March applies it once.
- June's `GetIncreaseSkillLevelByBuff` (`323644a0`) returns 0 in PvP/PvE and always adds the world buffs.
- Repair discount (`32363f00`), the factor-308 check (`32163070`) and the buff-list UI (`32e608f0`) go to **Phase 11b**.

**Split** (by function, so each function is ported whole):
- **11:** the loader with `_INT`, staging, both handlers, the record vector, `ReInit`, the skill level, and the emulator's item-buff route (user decision B, 2026-10-05: the emulator owns item buffs).
- **11b:** the Phase 4a hand-offs (slot 250, `IS_STORE_BUFF` consumers) plus the three readers above.
- March's `m_vecWorldBuffFactorID` stays filled as today, beside the new record vector, so its March readers keep working until 11b.

### Phase 11 — Done (2026-10-05)

**Landed** in `af7741b` + `da55146` (the emulator publishes `True`/`False` before running `BuffTemplet.lua`, and logs the parsed end-flag counts). The RE is in the "Buff Phase 11" funcmap rows; the server survey the emulator is transcribed from is in `JUNE_BUFF_PORT_ARCHIVE.md` ("Phase 11 — the March server's buff system").

**`SERV_IRUHADEV_JUNE_BUFF_PREMIUM` (client):**
- `X2StateLoading` also opens `BuffTemplet_INT.lua` (June `3258c470`). `OpenScriptFile` logs `<file>: N premium buff(s) added, M in total`, or `not found` with the pack instruction.
- Both world-buff handlers follow June (`326c9030`, `326c9530`):
  - a per-unit record (`CX2Unit::GetWorldBuffRecord`: factor, BTI, stack count, start, end, ≤1 h);
  - each buff applied `count` times (slot 135 at level 1);
  - kept when applied or dead;
  - June's skips: `BFI_EMPTY_EXP_BUFF` in fields / `GT_PVE` (village: always), `BFI_BUFF_2013_DEFENSE_BUFF` in PvP, `BFI_SPA_STRONG_BUFF` in `XS_PVP_LOBBY`;
  - the in-room gage erase;
  - the returning-hero emblem for 114 / 10060 in the village;
  - a PC-room (BTI 138) change refreshes the socket data (`UpdateSocketDataAndEnchantData` + `UpdatePassiveAndActiveSkillState` = user slot 176 in a room; `ResetIncreaseSkillLevelBySocket` in the village).
- Log lines: `world buff (room|village): … sent, … kept`, plus a line for an unloaded factor or a BTI mismatch.
- `ReInit` re-applies each record `count` times (`32022380`). `GetIncreaseSkillLevelByBuff` is June's (`323644a0`).
- **Recorded, not ported** (no March home; funcmap): BFI 500's comeback end and "gone" UI (June's rewritten comeback members), the in-room emblem 34, BTI 1033 in `XS_GUILD_AGIT_GAME`, the PC-room remaining-time warning, and the icon stack count (June's gage `PushBuff` takes a count; March's does not).

**`SERV_IRUHADEV_OFFLINE_ITEM_BUFF` (emulator; user decision B: the emulator owns item buffs):** this is the March GameServer's `SERV_SERVER_BUFF_SYSTEM`.
- `CX2OfflineBuff` (`X2OfflineBuff.h`; the bodies are in `Handlers_Inventory.cpp`, so no new `.cpp` carries an unflagged PCH line).
- **Loader:** it reads `BuffTemplet.lua` + `_INT` the server's way (`BUFF_FACTOR`, `m_bEndBuffAt*`, rooms). It takes the `BuffManager` global for its run and hands it back to the client's manager.
- **Item use:** the item → BTI groups (Arcanum 78520-78550 → 109-112 + 117; Elixirs 78560-78590 / 78894-78901 / Ruriel → 113-116 / 191-198 / 1320) and the pre-checks (117 lockout → `ERR_USE_ITEM_IN_INVENTORY_07`; the temp-code guards → `_07` / `_06`).
- **Activation:** an active buff is refused, not refreshed; the period comes from `m_fBuffPeriodSecond` in wall-clock epoch seconds.
- **Storage:** `els_db.sql` schema **v14** (`unit_buff`), so a buff survives rooms and logins as it did in the LoginServer's RAM.
- **Sends:** `EGS_UPDATE_BUFF_INFO_IN_ROOM_NOT` (room-filtered) or `_NOT` on select unit, field change (only if any), loading 100, leave room (end flags), dungeon end, death, and expiry (3 s beat, 1 s live).
- **Deliberate departures:**
  - activation refuses a factor the client has not loaded, or one that names another BTI;
  - such items no longer get Phase 19's `USE_QUICK_SLOT_NOT` (live never sends it).
- 17 of the 18 grantable buffs resolve to a live factor of the same BTI. **Ruriel (1320 → factor 483) does not until Phase 13**, so the emulator refuses it with a log line.
- Emulator lines start `BUFF     ` in `offline_server.log`.

**`SERV_IRUHADEV_JUNE_BUFF_NOT_USE_IN`** (stand-in, user decision 2026-10-05, "dormant, like June"):
- `NOT_USE_IN_HENIR` leaves the buff out of a Henir dungeon's list;
- `NOT_USE_IN_PVE` and `NOT_USE_IN_HUGE_BOSS_RAID` are parsed but never fire (June-only modes).

**Staged** (user packs into `data036.kom`):
- `BuffTemplet.lua` (23,870 B) and `BuffTemplet_INT.lua` (12,482 B), compiled with mar-2014 from `june_src_fixed` (`1B 4B 4C 81`; round trip 138 / 71 entries).
- They are in `march2014/update/data036/` and `kom_contents/data036_kom/`, and in both manifests.
- Every name resolves in the staged Enum except `URT_GUILD_AGIT` (3 uses). It indexes to `nil` in a list, so it is harmless.

**Save:** backed up before v14 as `db_backup/els_db_20261005_150843_pre_buff_phase11_v14.sql`. v14 is additive (`CREATE TABLE IF NOT EXISTS`), but a build with the flag off refuses a v14 save.

**JUNE_BUGS.md:** #27 (unchecked erase dereference, guarded: a crash), #28 (the skill level counts a world buff twice in a game), #29 (the Rose event plays the returning-hero emblem in the village).

**Hygiene:** `flagcheck --range af7741b^..af7741b`, each flag `--with` the other two: `_PREMIUM` `4 PASS, 2 PASS~, 1 DEFINE, 8 OTHER, 0 FAIL, 0 ERROR`; `_OFFLINE_ITEM_BUFF` `2 PASS, 6 PASS~, 1 DEFINE, 6 OTHER, 0 FAIL, 0 ERROR`; `_NOT_USE_IN` `1 PASS, 1 PASS~, 1 DEFINE, 12 OTHER, 0 FAIL, 0 ERROR`; all `RESULT: PASS`, `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`. OTHER = files carrying only the sibling flags (`X2OfflineDB.cpp`'s table list is `#ifdef`/`#else` with March's line intact).

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,173,376 B, dated 2026-10-05 15:25:08, md5 0dd0588096c23e6495e92d9c2a55af8c, equal to X2/US_SERVICE/x2.exe (2026-10-05 15:25:04); built at mods/june-buff-port 9d70f0c + 1 uncommitted code file(s).` (that 1 = `da55146`; the first deploy, `9ebc4b93` at 15:18, lacked it). Range hygiene `af7741b^..HEAD` for `_OFFLINE_ITEM_BUFF`: `3 PASS, 6 PASS~, 1 DEFINE, 6 OTHER, 0 FAIL, 0 ERROR`, RESULT PASS; its 1 deleted line is `da55146` rewriting Phase 11's own load-summary log line. The phase's strings (`world buff (room|village)`, `premium buff(s) added`, `BUFF     activated buff`, `schema upgraded to v14`, `NOT_USE_IN_HENIR`, `BuffTemplet_INT.lua`, `end at death`) are in the deployed exe. March's 58 premium BTIs are all still in June's 209 (none dropped; `X2ItemSlotManager.cpp:3384` null-checks anyway).

**Play-test (2026-10-05, user; exe `0dd05880`): checks 1-4 passed, 5 partly.** Logs:
- `209 premium buff templet(s) loaded: end at death 5, at dungeon leave 23, at dungeon end 23, at field leave 17, NOT_USE_IN_HENIR 6`;
- `activated buff 109 (factor 90) for 180 s` and `117 … 300 s`, then `world buff (room): … 2 sent, 2 kept`;
- `refused Arcanum 78520 - STRENGTHENING_BODY (117) is active`;
- on death, `deactivated buff 109` → `1 sent, 1 kept`.
Check 5: back in the village, no icons appeared. The emulator's send after `EGS_STATE_CHANGE_FIELD_ACK` (live `GSUserField.cpp:215`) arrives while the field game still exists, and the village handler drops it.
- Live's icons come from the `ERM_JOIN_FIELD_ACK` round trip, which grants `WARM_ENERGE_OF_EL` (122) and so resends the list.
- **Fix `1e29455`:** the list is resent at `EGS_FIELD_LOADING_COMPLETE_REQ` (village), only if non-empty.
- Redeploy: `DEPLOY OK: X2_offline.exe is 19,173,376 B, dated 2026-10-05 15:49:57, md5 551a1dabd875dc31ddada7f7abb16361, equal to X2/US_SERVICE/x2.exe (2026-10-05 15:49:52); built at mods/june-buff-port fafc865 + 1 uncommitted code file(s).` (that 1 = `1e29455`).
- **Recheck PASSED** (2026-10-05, exe `551a1dab`, run 15:57). Check 4 (not reached in the first run): `used item 78894` → `activated buff 191 (factor 186) for 1800 s` → `room: 1 sent, 1 kept`. Check 5: `FIELD … loaded` → `sent 1 buff(s) outside a room` → `village: 1 sent, 1 kept`, the same with 3 buffs and after the relog. The two buffs that expired while logged out were resent once at select, then removed by the first tick, as live does.
- **Village "Rest" buff (user, 2026-10-05; `af37a76`, `SERV_IRUHADEV_OFFLINE_VILLAGE_BUFF`).** BTI 122 (`STR_ID_20716` "Rest" / 20717 "HP Recovery", `BBT_EMPTY`: icon only) is granted at each village load, then the list is sent. It is removed at room entry (`OpenRoom` = live `OnLeaveField`), at back-to-select, and at select (a closed game sends no deselect). `DEPLOY OK: X2_offline.exe is 19,173,376 B, dated 2026-10-05 16:15:31, md5 3f3f756b25f7d1c770de8f656e80c43a, equal to X2/US_SERVICE/x2.exe (2026-10-05 16:15:26); built at mods/june-buff-port 45a9755 + 7 uncommitted code file(s).` (= `af37a76`). flagcheck `--worktree`: `4 PASS, 2 PASS~, 1 DEFINE, 0 FAIL`, RESULT PASS.
  **Play-test PASSED** (2026-10-05, run 16:20): each village load → `activated buff 122 (factor 103)` → `village: 2 sent, 2 kept`; each room entry → `deactivated buff 122` → `sent 1 buff(s) in room type 7` / `1`.

**How to see it in game:**
- Buy an Arcanum / Elixir (cash shop → Consumables → Restore, products 11-26) and claim it.
- Use it from the bag **inside a dungeon or field**: the client refuses these items elsewhere (`X2UIInventory.cpp:7259-7276`, `STR_ID_16529`).
- The client always sends temp code 1 (`:7268`), so re-using an item from the same group is refused, never a replace.
- Die with the console line `g_pX2Game:GetMyUnit():SetNowHP( 0 )`. Offline revival is refused (no stones).

### Phase 11 — Review (2026-10-05, Opus 5.5 high)

**Verdict: one transcription error, fixed (`323644a0`), and one missed March home, ported (BFI 500's end time). Everything else matches June and the March server.** Every June check below was read from the assembly (Ghidra, read-only), not the decompile.

**Port: BFI 500's end time → `m_iComeBackBuffEnd`** (`X2PremiumBuffManager.cpp`, both handlers, under `_PREMIUM`).
- Both June handlers store the packet's end time (`elem+0x63`) into manager `+0x60/+0x64` (in room `@326c92b4`, village `@326c9764`). This happens before the factor check.
- June's `STR_ID_2699` chat line (`@323bbcb9`) reads exactly that slot. It is guarded by `[mgr+0x1]` (`GetViewBuffChat`) and `[mgr+0x5d]` (`IsComeBackUser`). That is March's `X2TFieldGame.cpp:379`, which reads `GetComeBackBuffEnd()`. So `+0x60` is March's `m_iComeBackBuffEnd`.
- It is dormant offline: no BFI 500 is ever sent.
- Still without a March home: the "gone" path. It calls `ToggleUI(0x2a, false)`, and UI `0x2a` is June's `DLG_UI_Return_User_Popup_Reward_Global_Reform2016` (made in `32159be0`, kept at UI-manager `+0xd0`). It also clears the June-only `+0x68/+0x69` flags that `326c7a10` sets.

**Fix — `GetIncreaseSkillLevelByBuff` (`323644a0`), `X2Unit.cpp`, under `_PREMIUM`.** With a game unit, June returns `GetAddSkillLevelByBuff()` at once (`@323645e0` falls into the epilogue at `323645e4`; even the decompile says `goto LAB_323645e4`). The record walk (`@323645fb`) runs only without a unit, which is March's split. Phase 11 had the walk always run. The port now returns early; the `IsPve` (type 4) early-out is unchanged. **JUNE_BUGS #28 is withdrawn** (row kept, struck through); funcmap row `323644a0` corrected. No visible change offline: no grantable buff's factor carries `BBT_ADD_SKILL_LEVEL`.

**Kickoff checks.**
1. **Handlers.**
   - In room (`326c9030`): `factor - 0x9a` → type `== 3` or `== 4` skip; `- 0x29` (195) → type `== 1` skip. 500 / 10060 follow, then factor + clone null checks.
   - Record: BTI `+0x20`, count byte `+0x34` (0 → 1), `0 < end - start < 0xe11`. Slot `0x21c` `(factor,0,1,0,1)` × count; kept on the last `AL == 1`, `hp <= 0` (`FCOMP`/`TEST AH,0x41`) or state `== 4`.
   - BTI-138 toggle after the keep, i.e. only past the factor/clone checks. Erase: unchecked `[ptr+0x28]`, `3229e160`, then `324898c0(uid, BTI, 0)`.
   - Village (`326c9530`): drops the packet while a game exists. `154` skip / `114` emblem `0xc` / `949` with `[g_pMain+0x8c] == 7` skip is one else-if chain; 10060 → emblem `0xc`. The 138 toggle comes **before** the factor check (port: same).
   - Icon only when templet `+0xa8`; record whenever the clone succeeds. Post: `323646a0`, then 138 → `3235a080(0)`. The village erase passes `templet+0x64` (IsDeBuff).
   - Enum values: `GT_PVP 1`, `GT_BATTLE_FIELD 3`, `XS_PVP_LOBBY 7`, `EI_RETURN_OF_HERO 12`, BFI 154/195/114/949/10060, BTI 138. All match.
2. **Recorded, not ported.** BFI 500's end time **did** have a March home (ported above). Its "gone" UI and `+0x68` do not. The PC-room warning (`326c5cf0`) reads a June-only int64 end time at manager `+0xc8`. Note: `g_pData+0x334` is the premium-buff manager itself (the loader calls `326c6e20` on it). March's gage has no count field. Agit 1033 / emblem 34 have no March state or emblem. **No March home was missed.**
3. **ReInit** `@320242ab`: count `<= 0` → no apply, else slot 135 × count. Matches. **`3235a080(0)`** is March's `ResetIncreaseSkillLevelBySocket`: the active-page skill walk calls the socket getter `32357a70` (which takes June's extra argument) and the setter, then the skill-tree UI update. Candidate confirmed.
4. **Emulator vs the March server.**
   - Matches line for line: the parser (`XSLBuffManager.cpp:34-126`), `ActivateBuff` (`UserServerBuffManager.cpp:40-96`: refuse if active; period 0 → end = start), `GetBuffList` (`:152-176`), `CheckBuffPeriod` (`:272-303`) and `OnLeavePVPRoom` (`:556-582`: the flag only).
   - Also matching: both item groups and the pre-checks (`GSUserInventory.cpp:4390-4470`), `ActivateItemBuff` (`GSUserFunction.cpp:17117-17273`) and `CheckDeselectUnitBuff` 122 (`:17302`).
   - Trigger points: as listed in 11's Done note.
   - The deliberate factor refusal of Ruriel (1320, factor 483 until Phase 13) comes after the Elixir group is cleared, but nothing can be lost there. The client always sends temp code 1, so `CheckUseItem` already refuses any Elixir-group use while one is active (`ERR_06`). Ruriel is spent with no buff. Live would record 1320, but this client has no factor 483, so the player sees the same thing. No decision is needed.
5. **Lua hand-back.** This run's `ErrorLog.txt` shows `BuffTemplet.lua: 138 … 138 in total` and `_INT: 71 … 209 in total` at 16:20:35. The emulator's own `loaded 138` / `71` lines come after, at 16:20:43. The client's `OpenScriptFile` re-declares `BuffManager` before each load, so no interleaving can misroute rows.
6. **Village Rest buff.** Live `ERM_JOIN_FIELD_ACK` (`GSUserField.cpp:533-546`) grants 122 and resends only if it was newly activated. The emulator's fallback resend when 122 is already active (village to village) is `1e29455`'s documented fix: it sends more, never less. Removal in `OpenRoom` = `OnLeaveField` (`:305-326`).
7. **Lua files.** The live `data036.kom` holds `BuffTemplet.lua` 23,870 B and `_INT` 12,482 B (algo 2), the same as the staged copies. Both staged copies are md5-identical, start `1B 4B 4C 81`, and their `.src.lua` are byte-identical to `june_src_fixed`. There is no loose copy in the game directory. **The pack is confirmed.**

**Hygiene** (`--range af7741b^..af37a76 --with` all four + `OFFLINE JUNE_BUFF_DEBUG`):
- `_PREMIUM`: `4 PASS, 2 PASS~, 2 DEFINE, 14 OTHER, 0 FAIL, 0 ERROR`; 0 deleted; `RESULT: PASS`.
- `_OFFLINE_ITEM_BUFF`: `14 PASS, 1 PASS~, 2 DEFINE, 7 OTHER, 0 FAIL, 0 ERROR`; 1 deleted line; `RESULT: PASS`.
- `_NOT_USE_IN`: `4 PASS, 1 PASS~, 2 DEFINE, 16 OTHER, 0 FAIL, 0 ERROR`; 1 deleted line; `RESULT: PASS`.
- `_OFFLINE_VILLAGE_BUFF`: `6 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; 0 deleted; `RESULT: PASS`.
- The 1 deleted line is `da55146` rewriting 11's own load-summary log line in `Handlers_Inventory.cpp`. OTHER = files that carry only the sibling flags. No encoding, EOL or `#endif`-token issues.
- The fix: `flagcheck JUNE_BUFF_PREMIUM --worktree --files X2Lib/X2Unit.cpp`: `1 PASS … 0 FAIL`, `RESULT: PASS`. Its 4 deleted lines are 11's own flagged lines (3 comment lines and the assignment); March's `#else` is untouched.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,173,376 B, dated 2026-10-05 16:33:01, md5 778adb67f4ee3389964a784b101e7b33, equal to X2/US_SERVICE/x2.exe (2026-10-05 16:32:56); built at mods/june-buff-port 2c71b8c + 1 uncommitted code file(s).` (that 1 = the fix; `X2Unit.obj` was rebuilt at 16:32:46). After the BFI 500 port (`b78e9a4`): `DEPLOY OK: X2_offline.exe is 19,186,176 B, dated 2026-10-05 16:59:03, md5 44c5b837c54db774400411c335c63ef8, equal to X2/US_SERVICE/x2.exe (2026-10-05 16:58:58); built at mods/june-buff-port b78e9a4 + 11 uncommitted code file(s).` Those 11 are the 9b session's in-progress files, not this review's. The BFI 500 commit's hygiene (`--range 2a11a5d^..HEAD`): `2 PASS … 0 FAIL`, `RESULT: PASS`. Its 9 deleted lines are 11's own flagged lines: 4 code and comment lines in `X2Unit.cpp`, and 5 comment lines in `X2PremiumBuffManager.cpp`.

**Play-test:** nothing new is visible. An optional regression check: the village "Rest" icon appears, and an Elixir used in a dungeon still shows `room: 1 sent, 1 kept`.

### Phase 11b — Done (2026-10-05)

**Landed** in `32202c5`. Kickoff `kickoffs/june-buff/11b.md`; every June function has a "Buff Phase 11b" funcmap row.

**Ported:**
- **`SERV_IRUHADEV_JUNE_BUFF_STORE_BUFF`: defined but commented out** (user, 2026-10-05: "port now, flag off"). It is June's clear-all `3229fb10` = `FinishAndClearAllBuff( bool bKeepStoreBuff )`:
  - the gage save runs only when the bool is set; March saved on every call;
  - flagged templets skip their finish calls, but the vector is still emptied whole, so the flagged buff is erased unfinished (JUNE_BUGS #39);
  - the gage save `32494560` keeps only factors whose templet is `IS_STORE_BUFF` (predicate `32499c10`).
  - June passes true from `~CX2Game` and the five `Delete*Unit*` (CX2Game vtable slots 55-59), which are March's six `X2Game.cpp` sites.
  - Why it is off: March's live templets carry no `IS_STORE_BUFF` key, so with the flag on no buff would carry from one field or dungeon into the next. Turn it on with Phase 13's June templets (119 flagged).
  - Compile-tested with the flag on (X2Lib, 503 s, 0 errors).
- **`_REPAIR_DISCOUNT` (on):** June's `32363f00` reads the world buffs from Phase 11's record vector:
  - 202 +0.5, 203 −1.5, 204 +0.7 (March's three), plus June's 207 +0.1, 10027 +0.2, 10038 +0.5;
  - 876 would set the rate to 1.0, but its June event-map check has no March home, so the case is inert (JUNE_BUGS #40);
  - March's 0.1 `MAX_REPAIR_PRICE_DISCOUNT` clamp is gone, as in June;
  - all ten June shop callers apply a rate of 0 or more, so March's four `X2UIShop.cpp` `> 0.f` tests are now `>= 0.f`.
- **`_DURATION_TIME` (on):** June's `CX2MyGageUI::SetDurationTime` (`32e608f0`, slot 18 of every `*MyGageUI`; `32e5cb30`). For a world buff whose record lasts at most an hour, the icon shows the time left from the record's end time: minutes in green from 60 s, seconds in white from 59 to 1, unchanged at 0. Every other buff keeps March's 1..998 s text.

**Recorded, not ported** (funcmap rows):
- **Slot 250 (`3201ffb0`).**
  - BTI 436/926: Crest of Time, a June common SA. The hook takes an HP/MP/gauge snapshot (`0x740`, 9 class overrides), which only the death-prevention check `320072d0` restores (`0x744`, then BFI 683). This is folded into the existing checklist §7 death-prevention row.
  - BTI 807: a June PvP room type 8. March has types 0-3.
  - None of 436/807/926 is in March's live data.
- **Slot 251 (`3204dcd0`):** Luciel's Soul Collector. New checklist §7 row.
- **`32163070`:** the "rechallenge rest" check (BTI 313 / BFI 308). Its only caller is `CX2UIInventory`'s use branch for June item 100005712.
- **Clear-all callers passing false:** `3202a7d0`, `324ae320` and `3277ee10`, all June-only.
- **Open for the review:** June's base `DyingStart` was not located. March clears all buffs there, so the port's call there passes false (no gage save). This is dormant while the flag is off.

**`m_vecWorldBuffFactorID`:** with the flags as built, nothing reads it any more. Phase 11's two handlers only clear it and push to it, and every March reader sits in an `#else`. Removing it is the user's call.
- **Removed** (user, 2026-10-05: "if it's really unused, remove it") under `SERV_IRUHADEV_JUNE_BUFF_NO_WORLD_BUFF_ID`, which needs `_PREMIUM` and `_REPAIR_DISCOUNT` on.
  - It is checked against June first. Both handlers (`326c9030`, `326c9530`), `ReInit` (`32022380`) and the skill level (`323644a0`) touch only `+0xdbc`/`+0xdc0`, the record vector. A sweep for `+0xdb0..+0xdb8` finds only other classes.
  - The member, its getter, and Phase 11's six writes (`X2PremiumBuffManager.cpp`) are `#ifndef`-wrapped.
  - flagcheck `--worktree`: `2 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`, 0 deleted, `RESULT: PASS`.
  - Deploy: `DEPLOY OK: X2_offline.exe is 19,186,176 B, dated 2026-10-05 19:46:43, md5 abe45d3443526b32953e3e3223fba56d, equal to X2/US_SERVICE/x2.exe (2026-10-05 19:46:39); built at mods/june-buff-port 3d00f6b + 3 uncommitted code file(s).` Regression only: the field timer and the village resend should look as in the play-test above.

**Hygiene** (`--worktree --files` the 8 code files, `--with` the other two flags):
- `_REPAIR_DISCOUNT`: `2 PASS, 0 PASS~, 1 DEFINE, 5 OTHER, 0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII, 0 EOL-mismatched, 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- `_DURATION_TIME`: `1 PASS, 0 PASS~, 1 DEFINE, 6 OTHER, 0 FAIL, 0 ERROR`; same zeros; `RESULT: PASS`.
- `_STORE_BUFF`: `4 PASS, 0 PASS~, 1 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`; same zeros; `RESULT: PASS` (run with the `#define` briefly uncommented, since the tool requires one, then put back).
- OTHER = the files carrying only the sibling flags. `git diff --stat`: 247 insertions, 0 deletions.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,186,688 B, dated 2026-10-05 18:22:10, md5 d423c1c64cd89f418cbd75e3b260f6e5, equal to X2/US_SERVICE/x2.exe (2026-10-05 18:22:05); built at mods/june-buff-port d61ae5c + 8 uncommitted code file(s).` (the 8 = this phase's code files; `_STORE_BUFF` off in the deployed exe)

**How to see it in game:**
- **Buff time text:** in a dungeon or field, use an Arcanum (item 78520, 180 s) or an Elixir (78894, 1800 s), as in Phase 11's checks. The buff icon's number counts minutes in green, then seconds in white below one minute.
- **Repair:** prices are unchanged offline. No grantable buff is in the discount list, and the clamp only mattered above a 10 % socket discount.

**Play-test (2026-10-05, user; exe `d423c1c6`, run 19:28):**
- **Check 1 (timer) passed in the field.** Logs: `activated buff 109 (factor 90) for 180 s` and `117 … 300 s`, then `world buff (room): unit 12, 2 sent, 2 kept`.
- **No timer text in the village.** This is June's behaviour. June's `CX2GageManager::SetDurationTime` (`324895c0`, which inlines `IsNotUseNotifyTimeBuff`) has one caller, the time finalizer `32ec2670`, which ticks only on a game unit. The village resend (`village: 3 sent, 3 kept`) shows icons only, in June as in March.
- **Check 2 (repair regression) was not run:** all the user's gear is at 100 % durability. It is low risk:
  - the shop change only differs at a rate of exactly 0;
  - the clamp only differs above a 10 % socket discount.
- The run's 2 UNHANDLED lines are `E_DISABLE_HB_CHECK_REQ`, which is not this phase's.

### Phase 11b — Review (2026-10-05, Opus 5.5 high)

**Verdict: no transcription errors in 11b's three flags. The open point found a June change: buffs are cleared when a death state *ends*, not in `DyingStart`. It is ported under its own flag, `SERV_IRUHADEV_JUNE_BUFF_DEATH_CLEAR` (on).** Landed in `ea5f932`; post-commit `--range ea5f932^..ea5f932`: `3 PASS, 1 DEFINE, 0 FAIL`, `RESULT: PASS`. Every June check was read from the assembly. The funcmap rows `3229fb10`, `3277ee10`, `3202a7d0`, `3228a120` and `32015250` are updated.

**The open point: June's death clear (kickoff item 2).**
- **No `DyingStart` clears in June.** `3229fb10` has exactly nine direct call sites in a raw scan of `.text`. There is no inlined copy: the gage save `32494560` has one caller (`@3229fb62`).
- **Users: the die states' `StateEnd`.** `329cada0` → `3202a7d0` sits in the `StateEnd` delegate (`+0x54`) of every class's DIE_FRONT/DIE_BACK. The registration (`@329b8140`) fills March's nine-delegate `UserUnitStateData` layout (`+0x14..+0x54`, probed), so the Phase 9c row's "start function" was wrong. `3202a7d0` calls clear-all(false) and then tail-calls `CommonStateEnd` (`32015250`, likely). March's die states register no `StateEnd` (`DoStateEndStart` → `CommonStateEnd`).
- **NPCs: `CX2GUNPC::CommonStateEnd` (`3277ee10`, vslot 280).** It is not "not DyingStart because of user slot 280": slot 280 is past the 264-slot `CX2GameUnit` base. Its tail is clear-all(false) when:
  - the ending state has `DYING_END` (June bit 23) or `ESCAPE_END` (bit 24), read from June's parser `32784c00`; or
  - it is one of the four dying states (March's `_IsDyingState`).
  - Under event-map key 5 the two flags count only for the state sets `+0x9018`/`+0x9028`. March has no such map.
- **The ported move** (`X2GameUnit.cpp`, `X2GUUser.cpp`, `X2GUNPC.cpp`):
  - the base `DyingStart` no longer clears;
  - `DoStateEndStart` clears before `CommonStateEnd` for `USI_DIE_FRONT`/`USI_DIE_BACK` (all 9 classes register both, none with a `StateEnd`);
  - the NPC `CommonStateEnd` clears on June's default path.
  - **Effect:** a dead unit keeps its buffs until it revives, its state ends, or the game deletes it (`true` from `~CX2Game`/`Delete*Unit`). That is what lets an `IS_STORE_BUFF` buff survive a death once `_STORE_BUFF` is on.
  - **Why it is not under `_STORE_BUFF`:** it is its own logical change, live with that flag off.
  - **Safety:** HP-per-second behaviors skip HP ≤ 0 (`FIX_RECOVERY_HP`, on), so a corpse is neither healed nor ticked. June's `OnFrameMoveBuff` has no death guard either. March's gage already appended a dying player's buffs at death, so saving them at teardown carries nothing new.
- **Not ported (no March home or unidentified):** the key-5 branch; `31fffd50` and `ProcessSocketEffectEx(18)` in `3202a7d0` (March's SIEE ends at 11).
- **Small difference:** June's die-state end has no `m_bDyingStart` guard, and neither does the port.
- **Ranox `EraseAllBuffAndExtraDamage` (ours): keep `FinishAndClearAllBuff()` = false.** With `_STORE_BUFF` on, false finishes every buff (store buffs too) and skips the gage save, as June does for every clear but the six teardown sites. `true` would erase flagged buffs unfinished (JUNE_BUGS #39). Noted in `kickoffs/june-buff/13.md`.

**Kickoff checks.**
1. **Clear-all `3229fb10`: matches.**
   - The gage save runs only if vslot `0x198` (`IsMyUnit`) and the bool is 1.
   - Flagged templets (`+0x67`) skip the finish calls.
   - `322ae690(end, end, begin)` is a move with an empty source that returns `begin`, so the vector is emptied whole (JUNE_BUGS #39 holds).
   - It ends with `ClearBuffList` (`32489980`). The callers' arguments are as recorded: six pass 1, three pass 0.
2. **Gage save `32494560`: matches.**
   - It is gated on PvP (type 1), PvE (type 4, no March type) and main state `0x12`.
   - The predicate `32499c10` drops a factor whose templet is missing or not `+0x67`, via remove_if `32499ef0` and erase `32141240`.
   - The only caller pushes 0 (`@3229fb5e`), so `3229ff80` is unreachable.
3. **Repair `32363f00`: matches.**
   - The jump table `32364480` gives 202 +0.5, 203 −1.5, 204 +0.7, 205/206 nothing, 207 +0.1. Outside it: 10027 +0.2, 10038 +0.5, and 876 sets the rate to 1.0 if event key 17 is on.
   - The final step is `rate < 0 ? −rate : 1 − rate` with no clamp.
   - All ten callers skip only when the rate is below 0 or NaN (`fcom`/`test ah,1`), which is `>= 0.f`.
   - The ported block is live: `SERV_NEW_DEFENCE_DUNGEON` is ON (flagprobe).
4. **`SetDurationTime` `32e608f0`: matches.**
   - It uses the first record of the BTI and the flag at record `+0x20` (`32e5cb30`).
   - The time is the end time's low dword minus `_time64`.
   - From 60 s: green (0,1,0,1) at UIStringData `+0x2c`, and `/60`. From 1 to 59: white, in seconds. At 0 or below: unchanged. Otherwise March's 1..998 text, or `L""`.
5. **Recorded items: confirmed.**
   - BTI 807 tests `g_pData+0x6c`, the PvP room: June's MyUser is `+0x68`, and March has `m_pMyUser` `0x84` then `m_pPVPRoom` `0x88` (probed). The `+0xd8 == 8` game-type reading is inferred; March's PGT ends at 3 either way.
   - The only immediate 100005712 in `.text` (`321bdf0d`) is in `32163070`'s sole caller.
   - Slot 251 is `3204dcd0`.

**Hygiene** (11b, `--range 32202c5^..32202c5 --with` the other two; `_STORE_BUFF` with its `#define` briefly uncommented, `Always.h` md5 unchanged after):
- `_STORE_BUFF`: `files: 4 PASS, 0 PASS~, 1 DEFINE, 3 OTHER, 0 FAIL, 0 ERROR`. `_REPAIR_DISCOUNT`: `2 PASS, 0 PASS~, 1 DEFINE, 5 OTHER, 0 FAIL, 0 ERROR`. `_DURATION_TIME`: `1 PASS, 0 PASS~, 1 DEFINE, 6 OTHER, 0 FAIL, 0 ERROR`.
- All three: `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`.
- `_DEATH_CLEAR` (`--worktree --files` its 4 files): `files: 3 PASS, 0 PASS~, 1 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR`; the same zeros; `RESULT: PASS`.

**Deploy:** `DEPLOY OK: X2_offline.exe is 19,187,712 B, dated 2026-10-05 21:12:32, md5 229b4d0cf029fcbba1a4e5872aef58c5, equal to X2/US_SERVICE/x2.exe (2026-10-05 21:12:28); built at mods/june-buff-port fd5c92e + 4 uncommitted code file(s).` (the 4 = this review's files)

**How to see it in game:** a monster or your character dies with a buff on. The buff stays on the body until it is removed or you revive. Before this change it vanished at the moment of death. Two console checks: BFI 152 on every monster, then kill one; BFI 3 on yourself, then `SetNowHP( 0 )`.

**Play-test (2026-10-05, user; exe `229b4d0c`, run 21:15):** both checks passed (the fire stayed on a dying monster; the attack-buff icon stayed while the character was dead and was gone after the revive). Logs: run after the deploy, clean exit, load summary `320 loaded, 0 failed`, `361 loaded + 0 placeholder(s)`; neither check writes a line of its own. Unrelated: `BFI 512 requested but not loaded` (beyond the 361 March factors, Phase 13 data) and 2 `E_DISABLE_HB_CHECK_REQ` UNHANDLED.

**For the user:** the Phase 9c funcmap row `3202a7d0` carries a spliced word ("buffserpart found"). It is left as committed, with this review's correction appended.

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
Play-test hand-off: follow "Play-test hand-off (Phase 5 onward)" in the doc. Test through
the dev console (END key) wherever it can reach the check, and give me copy-ready
commands with every value already filled in (real BFI/BTI numbers, no placeholders).
For each check say where to be, the line to paste, what it does in one plain sentence,
and what I should see if it works and if it does not.
```

### Phase 12 — Done (2026-10-04), play-tested (user)

Data only: no C++, no flag, no exe change. Everything below is staged in `kom_contents/` and mirrored under `march2014/update/`; **the user packs.**

**What was staged** (every copy md5-checked against its June source):

| Archive | File | Source | Note |
|---|---|---|---|
| `data057.kom` | `DLG_UI_Common_Texture_Buff.TET` (34,728 B), `.tga` (1,048,620 B) | June `data215_kom/` | replaces 18,362 B / same-size `.tga`; manifest entries already existed |
| `datai021.kom` | `DLG_UI_Common_Texture_Buff_INT.TET`, `.tga` | June `datai021_kom/` | **new**; 2 manifest entries added (live `datai021.kom` has 90 files, no Buff_INT) |
| `datai020.kom` | `DLG_INT_Button02.TET` (3,126 B), `.tga` | June `data056_kom/` | **beyond the named atlases, see below**; replaces 268 B / 3 keys; manifest entries already existed |
| `data034.kom` | `DLG_BuffManager.lua` (365 B, algo 2) | `june_src_fixed/data034_kom/DLG_BuffManager.src.lua`, `elstools compile-batch -v mar-2014` | replaces 269 B; decompiled back and diffed against the source: identical |

Mirrors: `march2014/update/{data057,datai021,datai020,data034}/` (the two atlas pairs, the Button02 pair, `DLG_BuffManager.src.lua`) with their `_manifest.json` files (CRLF, 2-space, no trailing newline; each file's existing line-ending style kept; `datai021` is a new manifest). The originals that were overwritten are in `kom_contents_backup_june_buff_phase12/`.

**Before staging, the live `.kom`s were compared with the `kom_contents/` directories** (`elstools inspect --input`): `data057`, `datai020`, `datai021` and `data034` match by name and size, so this pack changes **only** the files above. (`data034_kom/` also holds 38 loose `.src.lua` files that are not in its manifest; they do not ship.)

**Findings**

- **June's Buff atlas is a strict superset of March's.** All 195 March keys are in it with byte-identical UV records (40 B each); 175 keys are new. Keys are upper-cased on load and on lookup (`KTDXDeviceTexture.cpp:800/814`), so case differences do not matter.
- **Art:** 186 of March's 195 icons have identical pixels in June's `.tga`; 9 differ: `BUFF_ICON_DISCHARGE`, `FIRE_BLOSSOMS`, `NASODARMOR`, `PLASMALINK`, `PLASMA_1`, `PYLON`, `RETURNOFELDRASIL`, `XMAS_2013`, `DEBUFF_ICON_JUDGEMENT_FIRE`. 166 of the 175 new keys sit on pixels that are transparent in March's `.tga`, so the new icons exist only in June's art.
- **No regression for the live March data.** March's live templets carry 262 icon keys, 258 of them on the Buff atlas. 256 of those 258 resolve with March's own atlas and the same 256 with June's. The 2 that do not (`BTI_DEBUFF_SHADOW_KNOT`/`DEBUFF_SHADOW_JAIL`, `BTI_DEBUFF_MIND_BREAK`/`Buff_Icon_MpRecovery`) fail with both.
- **June's 804 templets with an icon key** (735 in `BattleFieldBuffTemplet`, 69 in `_INT`), checked against the staged atlases by the texture name each templet gives:

  | Atlas named by the templet | Templets | Resolve before (March files) | Resolve after (staged) |
  |---|---|---|---|
  | `DLG_UI_Common_Texture_Buff.tga` | 779 | 487 | 767 |
  | `DLG_UI_Common_Texture_Buff_INT.tga` | 18 | 0 (no atlas) | 18 |
  | `DLG_INT_Button02.tga` | 5 | 0 | 5 |
  | `DLG_INT_Texture01.tga` | 2 | 2 | 2 |
  | **Unresolved, of 804** | | **315** | **12** |

  Without the optional `DLG_INT_Button02` pair the "after" figure is 17. The tool is `june2017_re/buff_icon_check.py` (`python june2017_re/buff_icon_check.py june`).
- **The 12 that do not resolve even in June** (4 distinct keys, all June data defects; transcribed, not patched):

  | Templet(s) | Key the templet asks for | What is in the atlas |
  |---|---|---|
  | `BTI_DEBUFF_SHADOW_KNOT` | `DEBUFF_SHADOW_JAIL` | the key exists **with a trailing space** (`'DEBUFF_SHADOW_JAIL '`); lookup does not trim |
  | `BTI_BUFF_AEM_OPEN_MIND` | `Buff_Icon_Open_Mind` | in no `.TET` in either tree |
  | `BTI_SPELL_OF_RECOVERY`, `BTI_BUFF_TR_HEAL` | `BUFF_ICON_POWER_ADRENALIM` | typo of `BUFF_ICON_POWER_ADRENALIN` |
  | 8 `_INT` events: `BTI_BUFF_EVENT_FOOD_KEBAB`, `_CHAFING_DISH`, `_STEAMED_MUTTON`, `_SHUANGPINAI`, `_XIAOLONGBAO`, `_BEIING_KAOYA`, `_QUMIZ`, `_FOTIAOQIANG` | `BUFF_ICON_EVENT_1` | in no `.TET` in either tree |

  What the client does with an unresolved key at draw time has **not** been observed; expect a blank icon. They stay as June shipped them.
- **`DLG_INT_Button02` is a third icon atlas.** Five `_INT` templets (`BTI_EXP_BUFF_555`, `BTI_ELIOS_HERO_5`, `BTI_EXP_BUFF_4YEAR`, `BTI_ELIOS_HERO_4YEAR`, `BTI_BUFF_RETURN_OF_HERO_GLOBAL`) take their icons from it, and March's copy (in `datai020`, 3 keys) has none of them. June keeps it in `data056` with 38 keys; it was staged into March's `datai020` so there is one copy only. The 3 keys March reads (`BTN_COINCHANGE_NORMAL/OVER`, `BTN_DUMMY_NORMAL`) are in June's with identical UV records, so the March UI that uses them keeps working; the `.tga` art differs (June wins). **Visible effect today:** at those three rects March's art is fully transparent and June's is opaque. They are used by `DLG_UI_Guide_List` (`Btn_GateOfDarkness`, `Btn_FriendSearch`, both `BTN_DUMMY_NORMAL`; the Guide Book list, referenced from `Book_Table.lua`) and `DLG_UI_Message_Ver1` (`GuraEvent2014`, `BTN_COINCHANGE_*`, `SetShow(false)` so unseen). So packing it makes two button-shaped images appear in the Guide Book list. No `X2Lib` code names these controls. **This file is an addition beyond the task's named atlases. Leave it out of the pack to skip it; the cost is 5 blank `_INT` buff icons (invisible until Phase 13 anyway).** `DLG_INT_Texture01.TET/.tga` are byte-identical between the two clients; nothing staged.
- **The three other buff UI Lua files are already identical.** `DLG_UI_Buff_Apink`, `_Icon` and `_Title` decompile (March, `mar-2014`) to the same source as `june_src_fixed`'s, so nothing was restaged. Only `DLG_BuffManager.lua` differs: June adds `AddDummyPos(993, 26, 0)` and `AddDummyPos(993, 2, 0)`. March's C++ never calls `GetDummyPos` on that dialog (`X2GageUI.cpp` only calls `SetPos`), so the adoption is invisible. **Follow-up (not done):** if June's binary reads those two positions (the `X2GageUI` buff-icon layout), port the reader. Owner: Phase 14 triage unless the UI parity work finds it first.
- **Premium `BuffTemplet.lua`/`_INT`** carry no `BUFF_ICON_TEXTURE_KEY`; nothing to check there.

**Pack list for Phase 12** (Phase 13 merges this into the single list):

| Archive | Source dir | Files |
|---|---|---|
| `data057.kom` | `kom_contents/data057_kom/` | `DLG_UI_Common_Texture_Buff.TET`, `.tga` |
| `datai021.kom` | `kom_contents/datai021_kom/` | `DLG_UI_Common_Texture_Buff_INT.TET`, `.tga` (new) |
| `datai020.kom` | `kom_contents/datai020_kom/` | `DLG_INT_Button02.TET`, `.tga` (optional, see above) |
| `data034.kom` | `kom_contents/data034_kom/` | `DLG_BuffManager.lua` (algo 2) |

Verify against the **live** `.kom`s with `elstools inspect --input`: `data057` Buff `.TET` 34,728 B; `datai021` 92 files with the `_INT` pair; `datai020` `DLG_INT_Button02.TET` 3,126 B; `data034` `DLG_BuffManager.lua` 365 B. No exe is needed for any of it (the same exe works before and after packing); the 12/17 new keys only become visible when Phase 13 adopts June's templets.

#### Play-test (run after packing; it is a regression check, nothing new is visible yet)

**Result (2026-10-04): the user reported the play-test passed.** Not recorded: whether `datai020.kom` (optional `DLG_INT_Button02`) was packed, so check 4 (Guide Book buttons) is unconfirmed. `ErrorLog.txt` was not read by the session.

The live templets are still March's and March ships no `_INT` buff files, so none of the 175 new keys, nor the `_INT` atlas, can be seen until Phase 13. What can be checked now: March's own icons still render after the swap, and a repainted icon shows June's art.

**IDs were checked, not counted.** `BFI_BUFF_VICTORIOUS_SWORD`, `BFI_NPC_PHYSIC_ATTACK_ACCEL`, `BFI_BUFF_FIRE_BLOSSOMS_APPLY` and `BFI_DEBUFF_SI_SA_ELK_SAND_STORM` were looked up in the Enum that the **live** `data036.kom` carries (the staged `Enum.lua`, 314,915 B, equals the live file's size; decompiled): 233, 3, 296 and 118. In the live `BattleFieldBuffFactor.lua` file position equals enum ID for positions 1-361; the only 5 entries where they differ (positions 362-366: IDs 481/482/483/372/373) are the known ordinal rejects. `SetBuffFactorToGameUnitByBuffFactorID_LUA` is bound at `X2Lib/X2Main.cpp:4147`. Level-1 values were read from the decompiled live factor file and are non-zero (no popup expected). None of the three lines has been run; the command pattern is the one run in Phases 4b-i and 4b-iii. No `X2Lib` code reacts to `BFI_NPC_PHYSIC_ATTACK_ACCEL` or `BFI_BUFF_FIRE_BLOSSOMS_APPLY`.

Do not use `BFI_NASOD_ARMOR` (353) for the repainted-icon check: Add's class code applies and ends that buff (`X2GUAdd.cpp:3095/3127`). Do not use Victorious Sword (233): `X2GUEL.cpp:3553/3630` has class cases for it.

1. **Where:** inside any dungeon, in the first room, with your buff icon row visible next to your own gauge. Open the console with END.
   **Paste:** `g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 3, 1 )`
   **What it does:** gives you a 30-second +40% physical attack buff, with an icon whose art did not change in June's atlas.
   **Look for:** an icon appears in the buff row and looks exactly as it did before the repack, then goes away after about 30 seconds.
   **If not:** no icon, a blank square or a different picture: send a screenshot.
2. **Where:** same room.
   **Paste:** `g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 296, 1 )`
   **What it does:** gives you a 20-second small attack boost and mana regeneration buff, whose icon June repainted.
   **Look for:** a second icon (a fire-blossom picture) appears for about 20 seconds. It may look different from what you remember; that is expected. A blank square is not.
   **If not:** a blank square or nothing at all: send a screenshot.
3. **Where:** same room.
   **Paste:** `g_pX2Game:GetMyUnit():SetBuffFactorToGameUnitByBuffFactorID_LUA( 118, 1 )`
   **What it does:** blinds you (lower accuracy) for 7 seconds.
   **Look for:** a debuff icon appears for about 7 seconds, then disappears. I have not seen this icon, so compare it with how it looked before the repack.
   **If not:** no icon, or it never leaves: tell me which.
4. **Only if `DLG_INT_Button02` was packed.** **Where:** a village or field, then open the in-game Guide Book list (the book button on the menu; I have not traced exactly how it opens, so skip this check if it is not obvious). **Paste:** nothing; this is a look. **What it does:** June's `DLG_INT_Button02` art is opaque where March's was fully transparent, and `DLG_UI_Guide_List` draws two buttons from it (`Btn_GateOfDarkness`, `Btn_FriendSearch`). **Look for:** two button-shaped images at the lower left (103, 370) and lower right (582, 530) of that list that were not there before. **If not:** if they look wrong or sit on top of something, say so and leave `datai020.kom` unpacked. The event button in `DLG_UI_Message_Ver1` (`GuraEvent2014`) is `SetShow(false)` in its script, so it stays hidden.

Also confirm the buff row is where it was; the `DLG_BuffManager` dummy positions change nothing in March's code.

Then read `ErrorLog.txt` for this phase's lines only: any `DLG_BuffManager.lua` or `Texture_Buff` error (there should be none) and no `OpenScriptFile.log` failure for `DLG_BuffManager.lua`.

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

**Kickoff:** `kickoffs/june-buff/13.md` - start it with `/x2-phase june-buff 13`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

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

**Kickoff:** `kickoffs/june-buff/14.md` - start it with `/x2-phase june-buff 14`. The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

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

**Kickoff:** `kickoffs/june-buff/15-step2.md` - start it with `/x2-phase june-buff 15-step2`. (Step 1 is done; this kickoff starts at step 2.) The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

### Phase 15, step 1 — RE (2026-10-04), read-only; step 2 not started

**Step 1 is done. The next session begins at step 2, using the inputs listed at the end of this subsection. Do not redo the RE that the kickoff above asks for.**

Ran on Opus 5.5 high. The session was read-only because a Phase 6 session and a Phase 16 session were working in the same tree: no code or Lua edits, no build, no Ghidra renames or comments.
- March bodies were read from a `cl /P` of `X2GameUnit.cpp`, `X2EnchantItem.cpp` and `X2Data.cpp` with the `US_SERVICE` flags (output in the scratchpad only).
- Blaze `3229b250` and Poison `3229bf90` have no Ghidra function. They were read with `disasm_range.py`.
- New `funcmap.tsv` rows are prefixed "Buff Phase 15 step 1:".

**Corrections to the Phase 15 text above**

1. **`g_pData+0x284` is `m_pCX2EnchantItem`, not a flag.** `31edc310` (`CX2Data::ResetEnchantItem`) deletes and re-creates it with `new` (0x34 bytes, ctor `3270eca0`). `32272d90`'s test is a null guard.
2. **`EnchantData` gains five scalars and three arrays, not five fields.** It grows from 0x3c to 0x74 bytes; the map is in the table below.
3. **Two different "tiers".** Keep them apart in step 2 and step 3:
   - The *EDT tier* is the weapon's attribute grade: blaze 1001 / 2002 / 3003 / 4004, frozen 1011 / 2022 / 3033 / 4044, poison 1101 / 2202 / 3303 / 4404.
   - The *accumulation* is the defender's stack count.
4. **Slots 214–219 are virtual, but nothing overrides them.** All 18 unit vtables hold the base bodies. That is `CX2GameUnit`, plus 17 from the `CX2GU*` scan: `CX2GUUser` and 12 character classes, `CX2GUNPC` and 3 NPC subclasses. The bodies are 214 `3229b250`, 215 `3229bd00`, 216 `3229bf90`, 217 `32289da0`, 218 `3229c980`, 219 `32278a90`.
5. **The readers of the new fields are outside the six bodies.** They are in `CX2GUUser::CommonFrameMove` (`32063650`), the NPC tick hosts (`327b53b0`, `3276b400`), `GetFinalDamage` (`32284aa0`, which already carries earlier-phase edits) and the tooltip (`3270d7b0` `GetToolTipForPure`, called three times by `3270ee40` `GetToolTip`). Step 3's scope grows by those five hosts.
6. **"Depends on: nothing in the buff port" is false.** Shock and the poison/frozen slows now apply buffs that March's buff data does not define. See *Buff-data dependency* below.

**`EnchantData` (June loader `32708f00`; node value at node+0x10)**

| Offset | Key | June type | March |
|---|---|---|---|
| +0x00 | `m_ExtraDamageType` (also the map key) | int | same |
| +0x04 / +0x08 | `m_fRate` / `m_Time` | float / int | same |
| +0x0c / +0x10 / +0x14 | `m_First/Second/ThirdDamagePercent` | **float** (getter `32fea370`) | int. June's data uses fractions (`37.5`, `112.5`) that an int field truncates |
| +0x18..+0x20 | `m_First/Second/ThirdSlowPercent` | int | same |
| +0x24..+0x2c | `m_First/Second/ThirdDefenceDebuffPercent` | int | same |
| +0x30 / +0x34 / +0x38 | `m_fTimeStun` / `m_HPDrainPercent` / `m_MPDrain` | float / int / int | same |
| +0x3c | `m_fAttribReiterationRate` | float | new |
| +0x40 | `m_fBlazeMpDrain` | float | new |
| +0x44 | `m_fPiercingDamageFactor` | float | new |
| +0x48 | `m_iShockDebuffTime` | int | new |
| +0x4c | `m_fShockDebuffDamageFactor` | float | new |
| +0x50 / +0x5c / +0x68 | `m_arrMovementSpeedDecrease` / `m_arrJumpSpeedDecrease` / `m_arrAttackSpeedDecrease` | float[3] each, read from Lua int lists 1..n | new |

Loader details the port must handle:
- A missing scalar becomes 0, as in March. `bIsForNPC` selects the map, and a duplicate key returns false, as in March.
- June's default value zeroes only March's 15 dwords, so the new tail is left uninitialised. When an array key is missing, the array stays as garbage.
- The array loop has no bound: a fourth list entry would overwrite the next field. **Port: zero-init all 29 dwords and stop at 3 entries.**
- `31c59710( pair, 0x78, … )` after the reads is an anti-tamper hash. It is not ported.

**The six bodies against March**

| Slot | Body | June | Verdict |
|---|---|---|---|
| 214 | Blaze | `3229b250` | **differs** (common changes plus the damage changes below) |
| 215 | Frozen | `3229bd00` | **differs** (common changes only; the time is `m_Time` as in March) |
| 216 | Poison | `3229bf90` | **differs** (as Blaze; no particle or camera, as in March) |
| 217 | Piercing | `32289da0` | **same**: `m_bPenetrate`, two particles, camera 20 / 0.3, sound |
| 218 | Shock | `3229c980` | **March plus one store**: the attacker goes into `m_EnchantShock.m_optrGameUnit` (`+0x2538`). Time is `m_fTimeStun`, camera 15 / 0.5 then 20 / 0.5, `AnimStop`, three particles, all as in March |
| 219 | Snatch | `32278a90` | **differs in the tail; not fully decoded** |

Common to Blaze, Frozen and Poison:
- **No `m_fTime > 0` gate.** The accumulation is the stored value + 1, capped at 3; March gives 0 unless the effect is live and caps at 2. So June's first proc sets 1 where March's sets 0.
  - The reset moved into the tick. When the element's `m_fTime ≤ 0`, the tick zeroes the accumulation and the damage timer.
  - User sites: `@32069a17` (blaze), `@3206adc3` (frozen), `@32068e94` (poison; guard read `@32068dd6`). NPC sites in `327b53b0`: `@327b9688`, `@327bb633`, `@327b94a2`.
  - So in steady state the effect equals March's gate.
- **A per-element "accumulation rose on this proc" byte:** `+0x7c7c` blaze, `+0x7c7d` frozen, `+0x7c7e` poison. It is cleared at the start of each body and zeroed in the unit ctor (`@322a5eb8`). Its reader is `32272780( element 1..3 )`, used by the slow ticks (below).
- **Each body stores `pDD->optrAttackerGameUnit` in the element's `m_optrGameUnit`** (`ExtraDamageData+0xc4`). The ticks use it to find the attacker's `EnchantData`.
- `m_ExtraDamagePack.Init( EDT )` is inlined as `element.Init(); element.m_ExtraDamageType = EDT`. That is March's effect.

Blaze and Poison damage:
- **The tier is read on the new 1-based accumulation.** 1 → First, 2 → Second, ≥ 3 → Third; at 0, `m_DamagePerSec` stays 0. The NPC-attacker path does the same.
- **New: `× ENCHANT_STAT_SCALE[ weapon item+0x18 ]`.** The table at `33487110` holds the values of March's `X2Define.h:233`. It is skipped when `32708c50` reports PvP (state 9) with channel class 9 (`PCC_FREE`). The index is not bounds-checked.
  - Against March's stat call this would not double-scale: March applies the +N scale only in `CX2Item` (`X2Item.cpp:650/659`), and its enchant bodies call the templet-only `CalculateItemStat`. But a whole-function port uses June's stat call, the templet-bit-7 branch below that was not diffed. Whether that call already applies the +N scale decides parity, so settle it in step 3.
- **The dungeon branch differs:**
  - It runs in states 11 and 23. March runs it in `XS_DUNGEON_GAME` only. June's 23 is probably `XS_BATTLE_FIELD`, which is 24 in March (23 there is `XS_ARCADE_ROOM`); that identity is unproven.
  - It has one quick-slot check (`328b26a0` on the room's dungeon ID). March has two: Henir challenge and defence dungeon.
  - The event list is March's 14 dungeons plus 41280 `DI_EVENT_WALLY_LAND`, which is June-only and missing from March's enum.
- **Not diffed:** the stat call branches on templet bit 7 (`[templet+4] >> 7 & 1`) between `328d72c0`/`328d6d70` and `322725d0`. Do not assume it equals March's level-linked loop plus `CalculateItemStat`.
- PvP (FREE/TOURNAMENT → stat, else × 0.1) and the tail (`ApplyEnchantResist`; Blaze `EnchantBlazeAttack` plus `UpDownCrashCamera(20, 0.5)`) are March's.

Snatch:
- The keys and the math match March: `m_HPDrainPercent × base / 100`, `m_MPDrain`, and two `ApplyEnchantResist(ET_DARK)` calls. It uses the same states as Blaze and has no +N multiply.
- **Not decoded:** the defender's loss goes through vslot `0xd8`, then `UpNowHp(−hp)` (`0xec`) and `UpNowMp(−mp)` (`0xf0`). March uses `SetNowHp` with a clamp plus `m_fDamageHistory`.
- **Not decoded:** a second "HP −/MP −" text is drawn on the unit behind `DD+0x470` when that pointer is set (pic char `g_pData+0x154`).

**Accumulation modifier `32272d90` (args `pDD, basicEDT, miscEDT, rate`)**
- `R = 0`. When `m_pCX2EnchantItem` exists and the attacker is live, `R` is the `m_fAttribReiterationRate` (+0x3c) of `GetEnchantData( misc )` for a user attacker or `GetNPCEnchantData( misc )` for an NPC (`+0x264` 0 / 1).
- Only basic 1001, 1011 or 1101 qualify, and misc must be one of that element's four EDT tiers; otherwise the rate is returned unchanged.
- With the defender's accumulation (before this proc's increment) at 1 the result is `rate + R`; at 2 it is `rate + 2R`; otherwise `rate`. March multiplies by 2 at accumulation 1–2.
- With the 1-based count, the bonus starts at the second proc; March's starts at the third.
- It reads the *misc EDT's* table directly, not slot 112, so `USE_OWNERS_ENCHANT` does not apply here.

**Readers of the new fields**

| Field | Reader | June behaviour |
|---|---|---|
| +0x3c | `32272d90` | above |
| +0x40 | user `32063650` `@320691de`; NPC `3276b400` (= March `CX2GUNPC::CommonFrameMoveHost_ExtraDamage`, candidate) `@3276b95d` | On the blaze tick, when the tick damage is > 0 and `m_EnchantBlaze.m_optrGameUnit` is live, the defender loses `m_fBlazeMpDrain × (1 − GetEnchantResist(ET_BLAZE)/500)` MP. |
| +0x44 | `GetFinalDamage` `32284aa0` `@322865ea` / `@322866c3` | When `DD.m_bPenetrate` (`+0x4c5`) is set, damage gains `factor/100 × base × 0.5 × (1 − defender GetEnchantResist(ET_WIND)/500)`. The base is the attacker's attack for an NPC, or the weapon item stat × `ENCHANT_STAT_SCALE` (same `32708c50` gate) for a user. |
| +0x48, +0x4c | user `32063650` `@32068bc0`; NPC `327b53b0` `@327ba136` | Applies **BFI 1000 `BFI_DEBUFF_HARD_BODY`** (BTI 945) to the defender: `r = 1 − GetEnchantResist(ET_LIGHT)/500`, BBT 7 value `1 − r × factor`, BBT 8 value `1 − x × factor` (x not traced), `BFT_TIME = m_iShockDebuffTime × r`, then `SetBuffFactorToGameUnit` (slot 137). The trigger condition was not traced. |
| arrays | user `@32067e9e` (poison), `@3206a3b0` (frozen); NPC `@327b901f`, `@327bab4f` | Entry `[accumulation − 1]` of each array, ÷ 100, scaled by `1 − GetEnchantResist(ET_NATURE or ET_WATER)/500`. Poison builds **BFI 1001 `BFI_DEBUFF_SLOW_POISON`** (BTI 946); frozen builds **BFI 1014 `BFI_DEBUFF_SLOW_FROZEN`** (BTI 958). Each sets BBT 11/13/12 (anim / jump / run). They re-apply, after erasing the old BTI (`3229e160`), only when the debuff is absent or `32272780( element )` is set. |

- The arrays replace March's direct `m_First/Second/ThirdSlowPercent` speed clamps (`X2GUUser.cpp:16388` ff.). Not checked: whether June still reads the Slow percents anywhere.
- In the ticks, June picks the table by the stored attacker's class (`GetEnchantData` for a user, `GetNPCEnchantData` for an NPC). March picks it by game type (PvP or not).
- The ticks find the attacker's EDT with `32708080` = `CX2EnchantItem::DecomposeEnchantExtraDamage` applied to the attacker's slot 111 (user `3208e090` returns `[this+0x3db0]`, the weapon's composed EDT).

**Getter surface**
- Slot 112 (`+0x1c0`) `GetWeaponEnchantData` is virtual in June:
  - User `31fcb1b0` = `GetEnchantData`, as in March.
  - NPC `3275aa80` forwards to the owner's slot 112 when `USE_OWNERS_ENCHANT` (`+0x7c7f`) is set and the owner (`+0x8d5c`) is live; otherwise it returns `GetNPCEnchantData`. Slot 111 (`3275aa20`) does the same.
  - The flag is read by `32289090` beside `MAX_HYPER_MODE_TIME`, so it is the `InitComponent` counterpart (candidate). Only `data037_kom/Lire_Evoke_trap(_Memo).src.lua` set it.
  - March has one non-virtual function (`X2GameUnit.h:2596`).
- There are 13 slot-`0x1c0` call sites:
  - five bodies (Piercing never calls slot 112);
  - `322a0510`;
  - the `GetFinalDamage` frozen block (`@322855ff`, which reads March's DefenceDebuff fields);
  - six sites in other classes: `31b806f7`, `327cfa35`, `327cfa4c`, `327d2e0f`, `327d4aed`, `331dad47`.
- Only the bodies are proven not to read `+0x3c..+0x70`, because they were read by hand. The `GetFinalDamage` block and the six other sites were cleared by an `eax`-relative filter only.
- The map lookup both getters wrap, `3270f540`, has two more callers:
  - **`3270d7b0` = `GetToolTipForPure`.** It reads every new scalar (node `+0x4c..+0x5c` = ED `+0x3c..+0x4c`; the two rates × 100) and formats them into the tooltip. A port needs June's string IDs for those lines, and March's string table may not have them.
  - `32807e00`, a lookup in another class's map (`+0x2c4`), which is unrelated.
- `32d418a0` (`bbt, value, op`: 0 add / 1 multiply / 2 set on `value[1]`) and `32d41910` (the finalizer twin) are June buff-factor setters. March has neither.

**Loaders**
- **Ctor `3270eca0`:** it now has a vtable and the same three maps. It loads `AttribEnchantItem.lua`, `AttribEnchantRequire.lua` and the new **`AttribEnchantRequireED.lua`**. The Lua bindings are still only `AddEnchantData_LUA` and `AddEnchantRequire_LUA`.
- **`AddEnchantRequire_LUA` `3270bb90`:** March's five grades plus **`LEGEND` → `IG_LEGEND = 6`** (June `Enum.src.lua:133`).
  - March has no `IG_LEGEND`, in the Lua Enum or in C++. Its `ITEM_GRADE` is in the shared `KncWX2Server/Common/X2Data/XSLItem.h:38`.
  - Not settled: whether a missing `LEGEND` defaults to 0 or fails the entry as March's `goto fail_proc` does.
- **`GetAttribEnchantRequireED` `327089e0`:** it calls the Lua `GetAttribEnchantRequiredED( count, bWeapon, level, grade )` and always returns true.
  - The script is March's formula (`X2EnchantItem.cpp` `GetAttribEnchantRequireED`/`GetRequireEDFactor`) extended to count 4 (× 24), `LEGEND` (+8) and armour counts 3–4; March returns false for those.
  - Consumer: the offline handler `Handlers_Inventory.cpp:2551`. The port needs a degrade-if-missing path.
- **`AttribEnchantTable.lua` / `AttribAttachTable.lua`:** the June client has no loader. They are server resources that `X2OfflineAttribTable.cpp` loads, so they are not Phase 15 client data.

**Buff-data dependency**
- The six IDs are BFI 1000/1001/1014 and BTI 945/946/958.
  - March's `Enum.src.lua` has the names.
  - The staged March `BattleFieldBuffFactor`/`BattleFieldBuffTemplet` define none of them; only `Enum` matches in `kom_contents/data036_kom`.
  - June defines them in `BattleFieldBuffFactor`, `BattleFieldBuffTemplet` and `UnitApplyBuffGroup`.
  - March C++ has no enumerators for them.
- June null-checks the factor (`@32068bfa`), so the code can land first. But the shock and slow play-test checks need those six entries shipped with the exe, either through Phase 13 or a staged subset.
- **Number collision:** `0x3e9` is both `EDT_ENCHANT_BLAZE` and BFI 1001 (the push `@32067fe1`). Step 2's audit must not count the BFI as an EDT consumer.

**Still open (step 3):**
- The tooltip `3270d7b0`/`3270ee40` and the string IDs it uses.
- Snatch's tail (`0xd8`, `DD+0x470`).
- Blaze/Poison's templet-bit-7 stat call.
- The shock-debuff trigger and its x.
- The slow debuffs' durations.
- Whether the Slow percents are still read.
- `LEGEND` fail handling.
- June's `X2_STATE` 23.

**User decisions to collect (step 2 onward):**
1. The EDT renumbering (step 2's stop).
2. `IG_LEGEND` in the shared `XSLItem.h`. That is a shared-header change; the backlog rule accepts it when it is flag-gated and offline-only.
3. The six buff entries: ship them with Phase 15, or wait for Phase 13.
4. `USE_OWNERS_ENCHANT`. June changed a March function here, so the no-partial-updates rule says port it.
5. The pack list grows. It is `AttribEnchantItem.lua`, `AttribEnchantRequire.lua` (whether it differs from March is not yet checked) and the new `AttribEnchantRequireED.lua`.

**Where this stopped:** step 1 done; **step 2 not started**. Step 2's inputs are the EDT values the hosts compare:
- `322a0510`'s switch (1001, 1011, 1101, 1543, 1654, 1765);
- `32272d90`'s 12 tier values;
- `32274df0`'s six;
- `32708080`'s decompose table, which is the place to start the June EDT map.

---

## Phase 16 — Socket-option parity (added 2026-10-04; carries 92 and 78's socket terms)

**Why it exists.** In Phase 5c the user decided, for 92's three questions, "follow June, so no half-baked port". In June the option-data stats are one system, and it has to move together:

> socket-option loader → per-unit socket aggregate → option seeds (`InitAdditionalStatOptionValue`) → option-data refresh template → Lua curves in `SocketOptionForm.lua`

June changed every link. 92 and the three socket terms in 78's multiplier sit at the end of the chain, so they land here, after the system.

**Already mapped** (Phase 5c and its follow-up; `funcmap.tsv`, "Buff Phase 5c"):
- **Template** `320839e0`: June's formula is in the 5c Done note, Q1. Every other `CX2AdditionalStatByBuff` method equals March's.
- **Refresh** `32290950`: the same as March apart from three members. 24/25/26 move to option-data with `CalculateFinalMpIncPercentByAttack` (`322a99e0`) and `…ByAttacked` (`32073ba0`). June-only option members `+0x6bb0`/`+0x6c64` use `322a9a10` and `322a9c20`, and `+0x52fc` uses the plain refresh. All curves are `(value, level)`.
- **Option seeds** `32270dd0`, user branch (the NPC branch zeroes every member). Each entry is member → socket-aggregate offset:

  | Member | Seed |
  |---|---|
  | 11 anim | `+0x14` |
  | 12 move (both members) | `+0xc` |
  | 13 jump (both members) | `+0x10` |
  | 14 accuracy, 15 avoidance | **0**; March seeds anti-evade and evade |
  | 16 critical rate | `+0xbc` |
  | 17 critical attack | `+0xc0` |
  | 18, 19 | 0 |
  | 20 additional attack | `+0xdc` |
  | 21 additional defence | `+0xd8` |
  | **24 and 25** (MP melee/range) | `+0x124` |
  | **26** (MP hitted) | `+0x12c` |
  | 36–41, 44 | 0 |
  | 72 hyper charge | `+0x18` |
  | 76 hyper duration | `+0x1c` |
  | 78 | 0 |
  | **78's three terms** `+0x5194`/`+0x5248`/`+0x5ac8` | `+0xc8`/`+0xcc`/`+0xd4` |
  | **92** | `+0x13c` (`m_fDamageMaximizeRate`) |
  | June-only `+0x50e0`, `+0x52fc`, `+0x5a14`, `+0x6bb0` (Add's DP consume, checklist section 7), `+0x6c64`, and `+0x6de8`…`+0x74f0` | `+0xc4`, `+0xe0`, `+0xd0`, `+0x134`, `+0x138`, `+0x2ac`…`+0x2e4` |

- **Passive init** `322718e0`: it sets the MP members 24/25/26 itself (`@32271ac6`–`32271b1b`). March instead puts `m_SocketData.m_fMPMultiplyByAll` into their passive sum (`X2GameUnit.cpp:13557-13559`). Diff it.
- **Loader keys:** 41 June-only and 12 March-only (both lists are in the 5c Done note's resolution). In the loader, the stores go into a templet struct (`edi`) whose offsets are **not** the aggregate's: `+0x18` is written both for `m_fHyperModeChargeRate` and for `m_iAnimationSpeedRate`. Map the aggregate separately.
- **Curves:** June's `SocketOptionForm.lua` adds `NewDecrease`/`NewDecreaseCoefficient`, `CalculateFinalDamageMaximizeRate`, the two MP curves and others. It also changes existing ones (`CalculateFinalAnitEvadePercent` returns 0).
- **92's whole client side** (5c Done note, "Already decoded for 92"): the store, the refresh, the roll `322788c0` (boost 1.44), the virtuals for slots 113 and 121, the call sites and the summon copy.

**Steps**
1. **RE (read-only, like 1c).**
   - The aggregate layout at unit `+0x87b4` and the function that builds it (March: `SocketData` `operator+=`, `X2SocketItem.h:779…`, and its limits `:872`).
   - The templet → aggregate copy.
   - Every reader of each June-only field: skill cool time, boss damage, low-HP damage, MP cost, debuff time, item drop and so on. Character-specific readers go to checklist section 7.
   - Record verdicts before writing code.
2. **Data gate for `SocketOptionForm.lua`.**
   - Every curve March's C++ calls through `lua_tinker::call` (`X2SocketItem.h` and elsewhere) must exist in June's file with the same arity. A missing curve returns 0 and silently zeroes that stat.
   - Also check June's file for globals March's Enum lacks.
   - List the curves whose values change; that is the "June wins" reach to report.
   - Compile with `elstools compile-batch -v mar-2014`, stage in both directories and update the manifest. The user packs it, together with the exe.
   - Identify June's socket-option *data* files (the tables the loader reads) and decide adoption under the same rules.
3. **Code, under a new flag `SERV_IRUHADEV_JUNE_SOCKET_PARITY`.** One commit per layer: loader + aggregate; seeds + passive; refresh variants + the template; then readers.
   - Count the reach on March's live factor and socket data first. Which factors and socket options change value, by member? Put the numbers in the Done note.
4. **92 under `SERV_IRUHADEV_JUNE_BUFF_NEW_STAT`** (its census type is 5c's), wrapped in `#if defined(NEW_STAT) && defined(BEHAVIOR_PARITY)` where it touches the range members.
   - The curve's degrade path: one `[JUNE_BUFF]` line, and 92 off in the roll, while `CalculateFinalDamageMaximizeRate` is missing.
   - Retire 78's recorded deviation by reading the three seeded terms.
5. **Play-test.**
   - **Roll:** a console-hook FIX on BBT 7 raises both attack and range, so the roll fires with maxi 0. Look for a visible damage spread and a first-roll log line.
   - **92:** a FIX factor (SPA-style `FIX 10`). PERCENT-only 92 gives 0 under June's formula, so it cannot be the check.
   - **Sanity:** a move-speed or attack-speed buff still looks right, and socket MP gain still works.

**Q3 (user, 2026-10-04): no June-era server source exists, so design one.** Players' attack range comes only from the server's `KStat`, and June's formula is not in the client. The user's rule is to design a logical stand-in rather than leave 92 dormant. This phase therefore adds a **designed, not June** range source under its own flag (for example `SERV_IRUHADEV_JUNE_ATTACK_RANGE_DESIGN`). It is labelled in code and logged once.

**Designed attack range — decided (user, 2026-10-04).**
- **Rule:** each player's range = **10% of that player's own attack**. The **same share applies to both damage types**: physical range = 10% of physical attack, magical range = 10% of magical attack. This matches the live game, where the spread covers both.
- **Why one share is enough:** every unit carries both stats, and each hit is tagged physical, magical or mixed. June rolls the physical stat and the magical stat separately (`322788c0`, arg 0 and arg 1), and the hit's type picks which result counts. A magic-leaning class (for example Rune Slayer) therefore gets a large magical range automatically, and a physical one (Lord Knight) a large physical range. No per-class numbers are needed.
- **Where it lives:** client side, when the stats are applied (`CX2GameUnit::SetGameStat`, both overloads). There it sets the basic values of `m_AdditionalPhysicAttackRange`/`m_AdditionalMagicAttackRange` to 0.10 × the physical/magical attack basic. `KncWX2Server/Common` and the emulator's `KStat` are untouched.
- **The 10% is one named constant** beside the code, so retuning is a one-line change. It is a balance value, not June data.
- **Users only.** NPCs keep range 0, which is June's behaviour (`32763770`). Summons copy their owner's range results in the tail of `SetUserSummonedNPCInfo`, as June does (`3230ca6d`).
- **Buffs:** June's BBT 7/8 (and 80/81) write the range member too, so a flat attack buff also widens the range. That is June's code and stays.
- **Effect** (example: attack 10,000, so range 1,000):
  - no MAXIMIZE: hits roll on a bell curve between 9,000 and 11,000, most within ±500;
  - MAXIMIZE +0.5: average 10,500, floor 10,000;
  - MAXIMIZE +1.0: every hit at 11,000.
  - March never had damage spread, so with the flag on **every player hit varies a little**, even without a 92 buff. The user accepted this.
- **Flag:** `SERV_IRUHADEV_JUNE_ATTACK_RANGE_DESIGN`, separate from `SERV_IRUHADEV_JUNE_SOCKET_PARITY`, so the spread can be switched off on its own. It needs `BEHAVIOR_PARITY` (the range members) and the roll (`NEW_STAT`).
- **Logging:** one `[JUNE_BUFF] designed attack range (not June): 10% of attack` line on first use, and the first roll logged under `DEBUG`.
- **Play-test:**
  - The same basic attack on a dummy monster now gives varied numbers, all within ±10% of the old value (the pop-up number over the monster, not the chat log).
  - With a 92 FIX buff, the numbers cluster toward the top.


**Run with:** Opus 5.5, high effort. Review required. Split the RE (step 1) into its own session if it grows.

**Kickoff:** `kickoffs/june-buff/16-step2.md` - start it with `/x2-phase june-buff 16-step2`. (Step 1 is done; this kickoff starts at step 2.) The inline prompt that stood here was retired on 2026-10-04; `kickoffs/` is the source of truth.

### Phase 16 — Step 1 RE (2026-10-04, Opus 5.5 high, read-only)

This session ran step 1 only. No code, Lua, build or Ghidra annotations were changed. Every new function is in `funcmap.tsv`, tag "Buff Phase 16". Steps 2–4 remain.

**Corrections to the text above.**
1. **The loader offsets are the aggregate's offsets.**
   - The note above says the loader writes `+0x18` for both `m_fHyperModeChargeRate` and `m_iAnimationSpeedRate`. That came from a base-register change.
   - The loader keeps two bases. `EDI` starts at the `SocketDataTemplet` (map node `+0x10`). Partway through, it is reloaded from `[ESP+0x14]` = node `+0xc` (the pair, 4 bytes lower).
   - With the base tracked, every loader offset equals the aggregate offset. Hyper charge is `+0x18`, anim `+0x14`.
   - Two independent readers use the same offsets: the seed function `32270dd0` and June's `ConvertAndAdd` body `3207cd70`.
2. **March-only keys: 4, not 12.**
   - Six of the earlier hits are table names June still reads as plain (non-XOR) strings: `STAT`, `STAT_RELATION_ADD_LEVEL`, `EXTRA_DAMAGE`, `RESIST_ENCHANT`, `ATTACK_ENCHANT`, `EFFECT_NAME`.
   - The genuine March-only keys:
     - `m_fEvadePercent` and `m_fAntiEvadePercent` (`X2SocketItem.cpp:399/401`).
     - `m_iAllSkillLevelUpEx` (`:491`, `SKILL_LEVEL_UP_BY_POWER_RATE_TYPE`). The only define of that flag is commented out (`InHouse5.h:617`), so it is probably already dead in this build.
     - `m_vecGrowUpSocketID` (`:707`, `SERV_GROW_UP_SOCKET`). The flag is defined in `ServerDefine_Global.h:899`, reachable from X2Lib. Its live state is not preprocessor-checked.
   - June-only keys: the 39 names listed under "Loader" below, plus the table sub-keys `m_SkillType` and `m_PowerRate`. June's 140 key uses include repeats: `BLAZE`…`DARK` twice, and `m_sBuffLevel` and `m_fSkillAdditionalPowerRate` twice each.
3. **78's three terms have names now:**
   - `+0x5248` ← socket `+0xcc` `m_fAdditionalDamage` (attacker side);
   - `+0x5194` ← `+0xc8` `m_fAdditionalDamageAfflicted`;
   - `+0x5ac8` ← `+0xd4` `m_fAntiAddDamageRate` (defender).

**The aggregate: classes and layout.**
- **What the unit holds.** User `+0x87b4` is a heap `CX2SocketItem::SocketData*`, not an inline struct. NPCs never read it: the NPC branch of the seed function zeroes the members instead.
- **Class tree (RTTI):**

  | Class | vtable | Size | ctor |
  |---|---|---|---|
  | `SocketDataCommon` (base) | `33478a8c` | – | `32090990` |
  | `SocketData` (the per-unit aggregate) | `33478a98` | 0x468 | `32091080` |
  | `SocketData_DuoUnit` | `33478ab0` | 0x488 | `32091730` |
  | `SocketDataTemplet` (the pool entry the loader fills) | `33478acc` | – | – |

- **Where the aggregate is created.** The user ctor `3203e850` allocates it (`@32042060`–`320420e2`): `SocketData_DuoUnit` when `GetUnit()+0xaf0` type == 10 (`UT_LUCIEL`), otherwise `SocketData`.
  - It is registered with the anti-tamper table `3376d8d8` (`31c59710`), and freed in the dtor (`@3203ae99`).
- **Layout.**
  - Every plain field below `+0x2f0` is a `SocketDataCommon` member at the same offset in the aggregate and in the templet.
  - `+0x2f0` differs between the two:
    - In the aggregate it is `KProtectedType<CX2Stat::Stat>`: vptr, checksum `+0x2f4`, then 13 XOR'd dwords.
    - In the templet it is the raw 13-dword `Stat`: `m_Increase*` at indices 0–5, base HP / attack P / attack M at 6–8, **range P/M at 9/10** (no loader key fills them), defence P/M at 11/12.
  - The `DuoUnit` keeps two protected per-mode pairs at `+0x468`/`+0x478`. They are read through `SocketData` vslots 3/4: `32091b10`/`32091b60`, with base versions `32091320`/`32091310`.
- **Loader** `32726850`: the key → offset map, for the fields the seeds and readers use. Offsets are relative to `SocketData`.

  | Offset | Key | Offset | Key |
  |---|---|---|---|
  | `+0xc` | `m_fMoveSpeed` | `+0x124` | `m_fMPMultiplyByAll` **+** `m_fMPIncByAttack_ReturnRate` |
  | `+0x10` | `m_fJumpSpeed` | `+0x12c` | `m_fMPMultiplyByAll` **+** `m_fMPIncByAttacked_ReturnRate` |
  | `+0x14` | `m_iAnimationSpeedRate` | `+0x134`/`+0x138` | `m_fCharacterSpecializationA`/`B` |
  | `+0x18` | `m_fHyperModeChargeRate` | `+0x13c` | `m_fDamageMaximizeRate` (92) |
  | `+0x1c` | `m_fHyperModeTime` | `+0x140` | `m_fDamageUpByAMADS` |
  | `+0x20` | `m_fRepairPriceSale` | `+0x154` | `m_AllSkillLevelUp` |
  | `+0x58` | `m_bIsPVERankLinkedStat` | `+0x18c`/`+0x190`/`+0x191`/`+0x192` | `DUNGEON_ONLY_TYPE` / `DUNGEON_ONLY` / `PVP_ONLY` / `OVER_LAP` |
  | `+0x5c`/`+0x60` | `m_fDamageReduce` / `m_fPercentDamageReduce` | `+0x194`/`+0x198` | super armor % / time |
  | `+0x90`/`+0x94` | `m_fDamageReduceByMonsterAttack` / `m_fPercentDRByMonsterAttack` | `+0x22c`…`+0x248` | `m_iAddMpValue` … `m_fSummonNpcCoolTime` (March's order) |
  | `+0xb8` | `m_fSkillAdditionalPowerRate` | `+0x24c`/`+0x250` | `m_bIsLevelLinkedStat` / `m_iAddLevel` |
  | `+0xbc` | `m_fPercentCritical` | `+0x254`…`+0x268` | `m_f*RelLV` (6) |
  | `+0xc0` | `m_fCritical` | `+0x26c`/`+0x270`/`+0x274` | `m_bSocketExtraction` / `m_iSocketApply` / `m_iSocketCondition` |
  | `+0xc4` | `m_fCriticalAfflicted` | `+0x290` | `m_fEffectiveValue` |
  | `+0xc8` | `m_fAdditionalDamageAfflicted` | `+0x294` | `m_bIsRankLinkedStat` |
  | `+0xcc` | `m_fAdditionalDamage` | `+0x296` | `m_sBuffLevel` (first) |
  | `+0xd0` | `m_fAntiCriticalRate` | `+0x298` | `m_fBonusTitleMissionCountingRate` |
  | `+0xd4` | `m_fAntiAddDamageRate` | `+0x29c` | conditional-buff vector (below) |
  | `+0xd8` | `m_fAdditionalDefenceValue` | `+0x2ac`/`+0x2b0` | `m_fAtkPhysic`/`MagicPiercingRate` |
  | `+0xdc` | `m_fAdditionalAttackValue` | `+0x2b4`/`+0x2b8` | `m_fHPRecoveryRate` / `m_fIncreaseItemDropRate` |
  | `+0xe0` | `m_fAdditionalAttackedDamageValue` | `+0x2bc`/`+0x2c0` | `m_fIncreaseDmgRateToBoss` / `m_fIncreaseDefRateToBoss` |
  | `+0xe4`/`+0xf4`/`+0x104`/`+0x114` | HP/MP-up-by-attack(ed) vectors | `+0x2c4`…`+0x2d0` | `m_fDecreaseActiveSkillCooltime`, `m_fDecrease1st`/`2nd`/`3rdSA_SkillCooltime` |
  | | | `+0x2d4`/`+0x2d8` | `m_fIncreaseDmgEnemyHPFew_DmgRate` / `m_fIncreaseDefMyHPFew_DefRate` |
  | | | `+0x2dc`/`+0x2e0`/`+0x2e4` | `m_fDecreaseDebuffTimeRate` / `m_fDecrease_MPCostRate` / `m_fIncreaseDmgRate_MyHpFew` |
  | | | `+0x2e8`/`+0x2ec` | `m_fAllSkillCooltimeDecreaseRate` / `m_fAllSkillDamageIncreaseRate` |

  - **The two MP fields are summed at load.** `m_fMPMultiplyByAll` is stored into both `+0x124` and `+0x12c`, and each `_ReturnRate` key is then added to its own field: `@32732977` (`fadd [esi+0x124]`) and `@32732be4`.
  - **The conditional-buff vector `+0x29c`** has 0x10-byte entries, pushed `@32735bc5` via `3207b5d0`:
    - `{ BUFF_FACTOR_ID, second m_sBuffLevel, m_bKillAbsoluteMonster (byte), m_eTypeSetItemEffectEx == 15 (byte), m_fIncreaseDmgEnemyHPFew_HpRate (an int) }`;
    - an entry is pushed only when the factor and the level are both set, and either the kill flag is set, the HP rate is at least 1, or the set-effect type is 15.
- **The loader's rescale tail** (`@32736fe7`–`32737097`), against March (`X2SocketItem.cpp:726-739`):
  - ×100 on move (`+0xc`), jump (`+0x10`), critical % (`+0xbc`), hyper charge (`+0x18`), the HP-rate stat, additional attack (`+0xdc`), additional defence (`+0xd8`). Same as March.
  - ×100, **new in June:** the MP pair `+0x124`/`+0x12c`, spec A/B `+0x134`/`+0x138`, and `m_fDamageMaximizeRate` `+0x13c`.
  - ×1.5 on hyper time. Same as March.
  - **Dropped:** March's ×100 on evade and anti-evade (June has neither key).

**The templet → aggregate copy: the builder chain.**
1. **`3204b4c0`** = `UpdateSocketDataAndEnchantData`. It calls aggregate vslot 2 (`ReInit`, `320922a0`; Duo `32092450`), clears the unit maps, calls vslot 484 (`+0x790`) with the aggregate, re-protects it (`31c594b0`), then calls vslot 487.
2. **`3206d770`** = user vslot 484 = `InitSocketData( SocketData* )`.
   - It calls `SumSocketOption` once for each source: equipment, pet, riding pet, title, set items (vslot `+0x798`) and premium buffs (`326c67f0`).
   - Then it copies the extra-damage data into `DamageData` (`31e94d60`/`31e9c020`).
   - Then `LimitMaximum` `320804f0`, then **`InitAdditionalStatOptionValue` `32270dd0`**, the same order as March (`X2GUUser.cpp:3077-3079`).
3. **`3204f190`** = `SumSocketOption`. Per socket ID:
   - It looks up the templet (`32715320`), with a game-mode gate: under `GT_PVE` (`322cc1c0`: game type == 4) only IDs 35318/35319 pass.
   - Then the apply gate **`32001570`** (June's `CanApplySocketDataByGameType`):
     - game types 2/3/5 reject `PVP_ONLY`, and also reject a `DUNGEON_ONLY_TYPE` that differs from the current dungeon type;
     - type 4 rejects everything;
     - the default case rejects `DUNGEON_ONLY`.
     - March (`X2GUUser.cpp` `CanApplySocketDataByGameType`) checks only `GT_BATTLE_FIELD`/`GT_DUNGEON` and the two bools. The type-5 case and `DUNGEON_ONLY_TYPE` are June-only; `GT_PVE` cannot occur on March.
   - Then `AddEnchantAttack`/`AddEnchantResist` (`31fddef0`/`31fe09c0`).
   - Then one of three branches:
     - **Rank-linked** (`+0x294`): the templet is copied (`32092030`) and scaled by a rank factor from `CX2Unit` maps (`31edfe70` at `+0x3b4`; the PVE map is `31edfeb0` at `+0x3f4`). **Located, not decoded.**
     - **PVE rank-linked** (`+0x58`): the same, with a `+0x124`/`+0x12c` test. **Not decoded.**
     - **Plain:** `ConvertAndAdd`.
   - Then the skill-level-up loop (10 slots, `3288fe00`/`31fe14f0`), and the per-class hook vslot `+0x794` (empty for `CX2GUUser`).
   - Then a recursion when the templet's `+0x38` list is non-empty.
   - **New in June:** each templet's skill cool-time cuts go into a `map<slot, float>` at `CX2Unit+0xe78` (`32355d70`; `CX2Unit` = user `+0x867c`).
     - Slot 1 gets Active (`+0x2c4`), slot 2 1st SA (`+0x2c8`), slot 3 2nd SA (`+0x2cc`), slot 4 3rd SA (`+0x2d0`).
     - The all-skill cut `+0x2e8` goes into slots 1, 2, 3, 4 and 6.
     - Socket 2000145 (`0x1e8511`) sets user byte `+0x7ca8`.
4. **`32080670`** = `ConvertAndAdd( rhs, level, unitType )`.
   - The stat part: `m_Stat` is unprotected, then `ConvertAndAddStat` with `CalculateHpIncrementValue( level )` (`320722f0`, `32070490`), then `LimitMaximum` and re-protection.
   - Then **`3207cd70`**, the body. Every term is `+=` unless marked:
     - Linear terms, `(int)( rhs.f · curve( level ) + 0.5 )`:

       | Field | Curve |
       |---|---|
       | move `+0xc` | `CalculateLinearMoveSpeedValue` |
       | jump `+0x10` | `CalculateLinearJumpSpeedValue` |
       | anim `+0x14` | `CalculateLinearAnimationSpeedValue` |
       | hyper charge `+0x18` | `CalculateLinearHyperGageChargeSpeedValue` |
       | critical % `+0xbc` | `CalculateLinearCriticalValue` |
       | additional defence `+0xd8` | `CalculateLinearAdditionalDefenceValue` |
       | additional attack `+0xdc` | `CalculateLinearAdditionalAttackValue` |
       | **MP by attack `+0x124`** | **`CalculateLinearMpIncValueByAttack`** |
       | **MP by attacked `+0x12c`** | **`CalculateLinearMpIncValueByAttacked`** |
       | **maximize `+0x13c`** | **`CalculateLinearDamageMaximizeRate`** |
       | **spec A `+0x134`** | **`32073860`**: `CalculateLinear<Class>SpecializationAValue` |
       | **spec B `+0x138`** | **`320739e0`**: `CalculateLinear<Class>SpecializationBValue` |

       The two spec dispatchers switch on the unit type, 1 Elsword … 12 Ain, and return 0 for any other type.
     - Plain adds: `+0x1c`, `+0x20`, `+0x5c`, `+0x60`, `+0xc0`…`+0xd4`, `+0xe0`, `+0x140`, `+0x180`…`+0x188`, `+0x154`, `+0x22c`…`+0x238`, `+0x254`…`+0x268`, and **`+0x2ac`…`+0x2ec`** (17 floats).
     - `+0x23c` is multiplied, not added (March's `m_fAllAttackEnchantRate *=`).
     - The four HP/MP vectors are appended. The super-armor max-merge goes into `+0x21c`…`+0x228`, as March's `SUPER_SOCKET_IN_TITLE`. Each `+0x29c` entry is appended.
   - **Removed against March** (`X2SocketItem.h` `ConvertAndAdd`):
     - evade and anti-evade;
     - `m_fMPMultiplyByAll`'s plain add (now inside the two linear MP terms);
     - **Add's 0.5 `fReformValue` on hyper charge and hyper time** (`SERV_9TH_NEW_CHARACTER`). The aggregate path has no 0.5 anywhere: `3204f190`, `32080670` and `3207cd70` were all checked. This is a June-removes-a-feature item for Add; whether June applies it somewhere downstream is not checked.
     - `m_iAllSkillLevelUpEx` (probably dead in this build anyway).
5. **`320804f0`** = `LimitMaximum`: the `Stat` limit, then hyper time ≤ 40, super-armor rate ≤ 0.5, super-armor time ≤ 10. **Identical to March**, with no limit on any June-only field.

**Seeds** (`32270dd0`, user branch, the complete table; `@32270df3`–`32271228`). Member ← socket offset:
- `47bc`←`14`
- `4870`/`4924`←`c`
- `49d8`/`4a8c`←`10`
- `4b40`/`4bf4`←0 (accuracy and avoidance; March seeds anti-evade/evade)
- `4ca8`←`bc`, `4d5c`←`c0`
- **`50e0`←`c4`**, **`5248`←`cc`**, **`5194`←`c8`**, **`5a14`←`d0`**, **`5ac8`←`d4`**
- `4e10`/`4ec4`/`53b0`←0
- `4f78`←`dc`, `502c`←`d8`
- **`52fc`←`e0`**
- **`5474`/`5528`←`124`**, **`55dc`←`12c`**
- `5e4c`…`6284` (7 members)←0
- `63ec`←`18`, `64a0`←`1c`
- **`6bb0`←`134`**, **`6c64`←`138`**, **`6d18`←`13c`**
- **`6de8`←`2ac`, `6e9c`←`2b0`, `6f50`←`2b4`, `7004`←`2b8`, `70b8`←`2bc`, `716c`←`2c0`, `7220`←`2d4`, `72d4`←`2d8`, `7388`←`2dc`, `743c`←`2e0`, `74f0`←`2e4`**

**Passive init: the "diff it" item** (`322718e0` `@32271ac0`–`32271b21`).
- June gives 24/25/26 (`+0x5474`/`+0x5528`/`+0x55dc`) multiple 1 and sum 0 (`3236ccc0`/`3236cbf0`), and also `+0x52fc`.
- **Verdict: March's `SetPassiveSum( m_SocketData.m_fMPMultiplyByAll )` (`X2GameUnit.cpp:13557-13559`) is replaced, not kept.**
- June routes the socket MP bonus through load (summed into `+0x124`/`+0x12c`), then the linear curve, then the seed, then the option-data refresh with `CalculateFinalMpIncPercentBy…`. Keeping March's passive sum as well would count it twice.

**Readers of each June-only field.**
- The sweep: every `+0x87b4` site (52), plus a displacement/immediate sweep of each June option member's result `M+0xa0`/`+0xa4` and option value `M+0x24`.
- Excluded as false positives:
  - `31d95080` (ESP stack temps in the `X2Main` binder);
  - the `push 0x539c`/`0x53a0`/`0x5ab4`/`0x5b68`/`0x7158` immediates (string or size constants, not member reads).
- In `GetFinalDamage`, `EBP` is the attacking side and `EDI`/`ESI` the defending side. The evidence is the boss gate, `[edi+0x85f8]+0xdc == 1` on the unit hit, and the enemy-HP test on `EDI`.
- **Unchecked:** whether `GetFinalDamage` runs on the attacker for every call path.

| Field (socket offset → member) | Reader | What June does | Verdict for step 3 |
|---|---|---|---|
| `m_fAtk*PiercingRate` `2ac`/`2b0` → `6de8`/`6e9c` | `GetFinalDamage` `@32285850`/`@32285952` (and the twins `@32285a54`/`@32285b3a`), inside the damage-type switch | added into the defence-ignore sums | port; formula **not yet decoded** past the add |
| `m_fHPRecoveryRate` `2b4` → `6f50` | `322fbc60` `@322fbd4f`, which reads `result − 1` | callers: user `DamageReact` `@3204780b`, user vslot `+0x7a0` `32024510`, `3200edb0`, `323046f0`, NPC `327c9c80` | **located, not decoded**; `322fbc60` is not identified |
| `m_fIncreaseItemDropRate` `2b8` → `7004` | **no client reader** (no read of `+0x70a4`/`+0x7028` outside the seed and refresh) | server-side in June | an offline stand-in would be a design proposal for the user, not June |
| `m_fIncreaseDmgRateToBoss` `2bc` → `70b8` | `GetFinalDamage` `@32286519` | `damage *= result` when the defender's NPC templet `+0xdc` == 1 (boss) or `bl` | port |
| `m_fIncreaseDefRateToBoss` `2c0` → `716c` | getter `32071bf0`, user `DamageReact` `@3204884f` | added to the defender's reduction sum when the attacker is a boss (templet `+0xdc`) | port, with the `52fc` row |
| skill cool-time cuts `2c4`…`2d0`, `2e8` | `SumSocketOption` puts them in `CX2Unit+0xe78`; read by **`3234fbd0`** | `max( 1 − Σ, 0 )` per skill. June skill type → slot: 6 Active → 1, 7 SA and 14 SA grade 1 → 2, 15 → 3, 16 → 4, 13 hyper → 5, 5 buff → 6 | see the cool-time note below |
| `m_fIncreaseDmgEnemyHPFew_DmgRate` `2d4` → `7220` | `GetFinalDamage` `@32285344` | when the defender's HP/MaxHP ≤ 0.3 (`3343bd78`), attacker P/M attack `*= 1 + r` | port |
| `m_fIncreaseDefMyHPFew_DefRate` `2d8` → `72d4` | `GetFinalDamage` `@3228692c` | when the defender's HP ratio ≤ 0.3, `damage *= 1 − r` | port |
| `m_fDecreaseDebuffTimeRate` `2dc` → `7388` | `CX2BuffTimeFinalizerTemplet::SetFactor` `@32ec33cf` | already decoded in Phase 4b-ii (the `+0x7428` line); ported dormant | becomes live once seeded |
| `m_fDecrease_MPCostRate` `2e0` → `743c` | `3203c9a0` `@3203d02d` (user skill MP cost) | when `r > 0` and skill type ≠ 13 (hyper): `cost *= max( 1 − r, 0.8 )` (`33478a78` = 0.8 as a double; the compare direction was checked) | port; `3203c9a0`'s March host is not named |
| `m_fIncreaseDmgRate_MyHpFew` `2e4` → `74f0` | `GetFinalDamage` `@322853e5` | attacker: `r' = r · (1 − HP/MaxHP)`, P/M attack `*= 1 + r'` | port |
| `m_fAllSkillDamageIncreaseRate` `2ec` | **`32275ba0`** = June's rewrite of `GetAdditionalAttackPowerRateByType` (`X2GameUnit.cpp:15780`), called from `GetFinalDamage` `@32286885`; also the summon helper `322ece70` `@322ed2bd` | `AT_SPECIAL`: `(1 + skillDamageUp [+ allSkillDamage unless skill type 13]) × GetAdditionalAttackByType`. A summoned-NPC branch reads the owner's (`+0x8614`) `+0x2ec` | **port the whole function** (June rewrote it) |
| `m_fCriticalAfflicted` `c4` → `50e0` | getter `32071ad0` (option value), four sites in user `DamageReact` `@32048b97`…`@32048cb5` | added to the critical-damage multiplier, then `× 32071ab0()` | port; `32071ab0`, `32070e00` and `32070de0` not identified |
| `m_fAntiCriticalRate` `d0` → `5a14` | `32273480` (option value), user `DamageReact` `@32048aa2` and NPC `327b1f30` | added to the critical-chance threshold before the random draw (vslot `+0x1cc`) | port; the threshold's sign is **not settled** |
| `m_fAdditionalAttackedDamageValue` `e0` → `52fc` | `GetFinalDamage` `@3228538e` (`EBP` side: P/M attack `*= 1 + r` when `r > 0`); getter `32071b30` in user `DamageReact` `@3204885e` | subtracted from the defender's reduction sum | **roles not settled**: read on both sides; decode both before porting |
| `m_fCharacterSpecializationA`/`B` `134`/`138` → `6bb0`/`6c64` | generic `CX2GUUser::AttackResult` `@32012da2` and `CEffect::AttackResult` `@31f86c29`: `x *= 1 + A`, or `1 − B` when vslot `+0x674` returns ±2; plus 18 (A) + 13 (B) per-class functions | class mechanics | **checklist section 7** (list below); the generic sites need vslot `+0x674` named |
| `m_fDamageMaximizeRate` `13c` → `6d18` | the roll `322788c0`, the summon copy `3230ca6d` | 5c's "Already decoded for 92" | step 4 |
| 78's three terms `c8`/`cc`/`d4` | both `DamageReact`s (5c) | – | retire the 5c deviation in step 4 |
| conditional-buff vector `29c` | `322d6f20` (from `CX2Game` `3230b7b0`, on an NPC kill): for each entry with the kill byte set, when the killed NPC's `+0x9058` is set, my unit (corrected in 16-step3b) gets vslot `+0x21c` (factor, 0, level, 0, 1); also `CX2GUUser::CommonStateStart` `@32060c3e` (not an HP rate: a skill-category reader, decoded in 16-step3b) | – | port; the NPC-side `+0x9048` death-buff list in the same function is NPC data, not socket |
| `m_fBonusTitleMissionCountingRate` `298` | no reader beyond the socket-description UI (`3271a950`); **title-mission readers not checked** | – | unchecked |
| `m_fSkillDamageUpRate` `238` (March field) | `32275ba0`, `322ece70` | March's reader is rewritten (above) | with `32275ba0` |
| `SocketData_DuoUnit` vslots 3/4 | `32b0fa90` (`@32b0fac4`…`32b0fbaa`) | Lu/Ciel per-mode stat pair | **checklist section 7** |

**The cool-time note.**
- June's application point is **`3235a710`**, called from 16 skill-use sites. It reads the skill's base cool time from `CX2Unit+0x494` (`32891460`) and, when it is > 0, multiplies it by `32358480( skill ) × max( 3234fbd0( skill ), 0.8 )`. It writes the result with `32893880`.
  - `32358480` is a June skill-upgrade cool-time modifier: a list walk through `32891e10`, `324550a0` and `324510e0`. It is **not** March's `CX2Unit::GetSkillCoolTimeDecreaseRate`, which is item-based (`X2Unit.cpp:4722`, called from `X2UserSkillTree.cpp:2404/2449`).
  - **Conflict to settle in step 3:** `32358770` is called here as `( skill, &coolTime )`. The existing funcmap row for it ("find user by unit UID and apply BBT 100", Phase 1c) does not fit that call shape. The row was **not** edited. Re-check it before porting.
- **Stop item for step 3:**
  - June's SA grades are `ST_SPECIAL_ACTIVE_GRADE_1/2/3` = 14/15/16 (June `Enum.src.lua:7133-7145`). March's `SKILL_TYPE` has none of them; it stops at 13 (`ST_HYPER_ACTIVE_SKILL`).
  - So on March data every SA is type 7, which maps to slot 2 ("1st SA"). The 2nd- and 3rd-SA cuts (`+0x2cc`/`+0x2d0`) can never match.
  - Port the map faithfully and report those two as dormant, or ask the user. Do not invent a grade mapping.

**Every `+0x87b4` site, by field.** "March" means a field March's `SocketData` already has, read the same way unless noted.

| Site(s) | Field | Kind |
|---|---|---|
| `31fcb2e0` | `+0x140` `m_fDamageUpByAMADS` | March (getter) |
| `31fcd6b4` | `+0x230` `m_iMPChangeValue` | March |
| `31fd284e`/`2881`, `322851be`/`5255` | the protected `m_Stat` | March (the `GetFinalDamage` use is a game-type switch on stat indices 2–5) |
| `31fd5833`/`5858`, `31fd6a2b`/`6a50` | `+0x38` list, `+0x154` `m_AllSkillLevelUp` | March (skill-level-up) |
| `31fda18c`/`a20a`, `32047d40` | `+0x114` `m_vecMpUpByAttacked` | March, but June's element is **0x18 bytes**, against March's `D3DXVECTOR4`. Not decoded |
| `31fe8719` | `+0x234` `m_fHPRelativeChangeRate` | March |
| `32001205`…`32001275` | `+0x21c`…`+0x228`, super armor | March |
| `3201ad1e`/`ad64`/`adba` | `+0x48` | March structure; member not identified |
| `32024f71` | `+0x98`/`+0x9c` vector | not identified |
| `3202d6d0`, `3202d8f0`, `32047b53` | `+0xe4`, `+0x104`, `+0xf4` HP/MP-up vectors | March |
| `32045458` | `+0x458` vector | June list; with `+0x448` it is copied to the user `+0x744`/`+0x754` (`322a604c`/`6064`) |
| `322a6035` | `+0x32c` extra damage → `DamageData` | March |
| `3206d4b5`, `32298cea`, `329e24d9`, `32a71d89` | `+0x280` hyper-mode-effective list | March (`HAMEL_SECRET_DUNGEON`) |
| `32ccc01e` | `+0x180` `m_fSpeedUpManaGather` | March |
| `32060c3e`, `322d6f53` | `+0x29c` | **June-only** |
| `32275c28`/`5ca1`, `322ed2bd`/`d2d7` | `+0x238`, `+0x2ec` | **June rewrite** |
| `32b0fac4`…`fbaa` | vslots 3/4 | **Duo** |
| `3203ae99`, `3204b4c3`…`b5bf`, `3204ea89`/`ea9d`, `322a021d`, `32042093`…`320420e2` | lifetime, `ReInit`, re-protect, null test | plumbing |
| `3201133a`, `32c8bcdc`, `32d0c99d` | `mov esi, 0x87b4` immediates | not examined |

**Curves the June C++ calls by name** (the input to step 2's arity gate; every one takes one argument, the level, except where marked):
- `CalculateHpIncrementValue`
- `CalculateLinearMoveSpeedValue`, `…JumpSpeedValue`, `…AnimationSpeedValue`, `…HyperGageChargeSpeedValue`, `…CriticalValue`, `…AdditionalDefenceValue`, `…AdditionalAttackValue`
- `…MpIncValueByAttack`, `…MpIncValueByAttacked`, `…DamageMaximizeRate`
- the 24 `CalculateLinear<Class>Specialization{A,B}Value` (Elsword, Arme, Lire, Raven, Eve, Chung, Ara, Elesis, Add, Luciel, Rose, Ain; strings `33472bac`…`33472f68`)
- the option-data `CalculateFinal*` curves of the refresh, `( value, level )`, listed in 5c: `CalculateFinalDamageMaximizeRate`, `CalculateFinalMpIncPercentByAttack`/`…ByAttacked`, and the March set
- June's `SocketOptionForm.src.lua` defines every Linear name above (lines 1–351).
- ~~Not swept: the `CalculateFinal*` curves the June-only option members `6de8`…`74f0` use in the refresh `32290950`. Step 2 must list them from `32290950` before the gate.~~ **Settled (addendum, same session):**
  - **There are none.** `32290950` refreshes all eleven, `6de8`, `6e9c`, `6f50`, `7004`, `70b8`, `716c`, `7220`, `72d4`, `7388`, `743c` and `74f0`, with the **plain** `3236d350` (`@32290ef0`–`32290f64`), and no curve.
  - The plain refresh equals March's `UpdateResultValue` (`X2BuffTemplet.cpp:835`): `basic + option + FIX + RELATION`, then `+ basic·(P − 1)`.
  - So those members take the raw socket sum: plain adds in `3207cd70`, no ×100, no Linear curve.
  - The only option-data (curve) members June adds are 24/25/26, `6bb0`/`6c64` (`322a9a10`/`322a9c20`, from 5c) and 92.
- **Base-offset sweep (addendum).**
  - `dispxref` on the 20 member bases `M` (as opposed to `M+0xa0`/`+0x24`), outside the seed, basic, passive, refresh, insert, erase and update functions, finds only:
    - the unit ctor/dtor (`322a0730`/`322a2700`);
    - 92's known sites (`3230ca8c`, `32763d6e`, Add `32a2bbc5`, Ain `32a4fbef`);
    - `cmp reg, 0x5a14` immediates in five per-class functions (`32c7bdd0`, `32c7bed0`, `32c85690`, `32c886b0`, `32ca2af0`), which are constants, not member reads.
  - No reader is missing from the table above, and `7004` (item drop) still has no client reader.

**For checklist section 7: copy later** (this session may not edit the checklist):
- `SocketData_DuoUnit` (Lu/Ciel): the per-mode protected stat pairs and vslots 3/4 (`32091b10`/`32091b60`), read in `32b0fa90`.
- Spec A/B (`6bb0`/`6c64`) per-class readers:
  - A: `328eab00`, `3297ee20`, `32981130`, `329a4680`, `329e59f0` (Add `UpNowDPValue`), `32a620e0`, `32ac8f10`, `32b14860`, `32bc9a60`, `32bed5d0`, `32c41170`, `32c43bf0`, `32c56be0`, `32ca7130`, `32cb6290`, `32cc7300`, `32ccda90`, `32ce7480`.
  - B: `329b3cc0`, `329b5230`, `32a01b40`, `32a59e70`, `32ac7700`, `32b14860`, `32b37910`, `32ba6ef0`, `32bf5280`, `32c491a0`, `32c50330`, `32c85690`, `32ccfe30`.
- Add's removed 0.5 socket hyper reform (above).

**What step 1 settles for steps 2–4.**
- The aggregate needs no new class to port. March's `SocketData` gains the June-only fields at its own offsets, the loader keys, the ×100 table, `ConvertAndAdd`'s June body and the cool-time map, all under `SOCKET_PARITY`. `DuoUnit` goes to section 7, because March has no Lu/Ciel.
- `LimitMaximum` is unchanged.
- **Open decode items before code** (step 3 must finish each, not assume a formula):
  - the piercing formula;
  - `322fbc60` (HP recovery);
  - the two roles of `52fc`;
  - anti-crit's sign;
  - `32071ab0`;
  - the rank-linked branches (inert on March data, because no March socket sets either rank flag; **counting that on March's live socket data is step 3's reach count**);
  - the `+0x29c` HP-rate reader `32060c3e`;
  - `3203c9a0`'s March host;
  - `32358770`.

### Phase 16 — Steps 2-4 Done, part 1 (2026-10-06, Opus 5.5 high)

**Landed** (`e485716`; post-commit `flagcheck --range e485716^..HEAD`: 6 PASS, 1 PASS~, 1 DEFINE, 5 OTHER, 0 FAIL, RESULT PASS): `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (loader, aggregate, seeds, passive, template, refresh, debuff-time reader, 78's terms), 92 under `_NEW_STAT`, and `SERV_IRUHADEV_JUNE_ATTACK_RANGE_DESIGN`. The remaining readers moved to **16-step3b** (kickoff written); none of them has a March socket that feeds it.

**Step 2, data gate.**
- Arity: every curve March's C++ calls exists in June's `SocketOptionForm.lua` with the same arity. The one June lacks, `…InDamageFreeGame`, is called only in `ADJUST_BUFF_CALCULATE`'s `#else`, which `Always.h:161` compiles out. No Enum globals.
- Compiled `-v mar-2014`; the bytecode equals June's source on 643,860 evaluations (a round-trip decompile differs only by `and`/`or` rendering). Staged in `march2014/data036/` (md5 in the hand-off). **Copying it into `kom_contents/data036_kom/` was refused by the permission check: user step.**
- Curve reach, against March's live bytecode: Linear equal for levels 2-90 (level 1: 9.5 to 10; above 90 June caps at 143). Final equal below each soft cap, different above (move/jump/anim at intermediate 40: 0.230 to 0.255; critical 60: 0.584 to 0.560; hyper 30: 0.297 to 0.285; add-defence 100: 0.416 to 0.450). Evade and anti-evade: 0. March sockets losing evade / anti-evade: 473 / 434; using `m_fMPMultiplyByAll`: 9. The save's 13 sockets are animation speed, critical and hyper charge only.
- **Template reach (5c Q1 was "small"; it is not):** March's staged factor file (366 factors) has PERCENT ≠ 1 on critical rate 25×, additional defence 4×, hyper charge 1×, and PERCENT > 1 on walk/run 34×, jump 8×. Under June's formula a PERCENT-only factor on a basic-0 member gives 0 without a socket bonus: BFI 27 (critical ×1.5) was +0.50 crit chance, is now 0 (with a 0.2 socket crit: 0.80 to 0.30). Walk/jump change only when a move/jump socket is present too.
- Socket data files (decision for the user, below): June's `OptionItemData.lua` loads against March's Enum (13,408 entries) but names 80 `SOCKET_DATA_TYPE`, 9 SIEE (12-20), `HMET_FEVER`, 18 `EXTRA_DAMAGE_TYPE` and 20 `STR_ID` values March lacks; 247 March IDs are absent from it (none in the save); 1,367 shared IDs differ. With March's file every June-only key reads 0.

**Code** (June addresses in the comments; `funcmap.tsv` unchanged).
- Loader `32726850`: 35 June keys (not loaded, no reader decoded: `m_fSkillAdditionalPowerRate` / `m_eSkillIDForAdditionalPowerRate` / the `m_SkillType`-`m_PowerRate` table, `m_bSocketExtraction`, `m_iSocketApply`, `m_iSocketCondition`, the first `m_sBuffLevel` `+0x296`, `m_fAllStatRelLV`: 3b), the MP sum, the `+0x29c` list and its two error messages (log lines), the x100 tail; evade keys dropped. Rank-linked sockets are skipped with one line each (scaling not decoded); grow-up kept (June has no key, its `+0x38` recursion is not decoded).
- `ConvertAndAdd` = `3207cd70` (no Add 0.5 reform: checklist §7); game-type gate = `32001570` (`GetDungeonType` stands for vslot `+0x1c8`).
- Seeds `32270dd0`, basics `32271540`, passive `322718e0` (March's MP passive sum removed), template `320839e0`, refresh `32290950` (MP 24/25/26, spec A/B with the unit type, 92, the eleven plain members). 20 new members.
- 92: parse, factory, SGU, insert/update/erase, refresh, roll `322788c0` (boost mt19937 + Box-Muller, seed = defender's slot 113), summon copy `3230ca6d`. Slot 121 is a virtual returning 1.0 (Rose: checklist §7).
- 78: June's whole k in both DamageReacts; 5c's deviation retired. JUNE_BUGS #42-#45.
- Degrade: one line each when June's curves are missing (`SocketOptionForm.lua has no June curves …`, `CalculateFinalDamageMaximizeRate is missing …`); the June-only curve calls are then skipped.
- Inherited, not changed: `SumSocketOption` passes **my** unit type for every user's specialization curve (March).
- The designed range is set only in `SetGameStat`; no other code writes a user's attack basic (the other writers are `CX2GUNPC`'s), so it never lags. AI party bots take the owner's range through the summon copy (every case of `SetUserSummonedNPCInfo` reaches it), so their hits vary too.
- The template counts are over all 366 `AddBuffFactor_LUA` calls in the staged factor file, including the 5 the loader rejects by ordinal.

**Hygiene.** `flagcheck JUNE_SOCKET_PARITY --with JUNE_BUFF_NEW_STAT JUNE_ATTACK_RANGE_DESIGN JUNE_BUFF_DEBUG --worktree`: 6 PASS, 1 PASS~ (one blank line in `X2GameUnit.cpp`), 1 DEFINE, 5 OTHER (the 92 wiring files, under `_NEW_STAT`), 0 FAIL; 0 deleted, 0 non-ASCII, 0 EOL-mismatched, 0 untagged `#else`/`#endif`, 0 encoding changes.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,190,272 B, dated 2026-10-06 05:45:33, md5 f530f04f1f273846584230412fd5c9b4, equal to X2/US_SERVICE/x2.exe (2026-10-06 05:45:28); built at mods/june-buff-port 033b87c + 13 uncommitted code file(s).` The phase strings are in it (`BBT_CHANGE_MAXIMIZE` ASCII and UTF-16; the eight new log texts and the June keys/curve names ASCII). Not launched.

**How to see it in game** (hand-off below has the commands).
- Damage spread: the same basic attack on a monster now varies within ±10% (floating number, not the chat log); log `designed attack range (not June)`, `first damage roll`.
- 92: a console `FIX` 92 buff pushes hits toward the top of the spread.
- Curve state: `June SocketOptionForm.lua curves loaded` or the missing-curves line.
- Crit buffs (BTI 26 etc.) without a critical socket no longer raise crit chance (template reach above).

**Play-test (2026-10-06, user: all checks passed; exe f530f04f, after the user copied and packed June's `SocketOptionForm.lua`).** Logs: `June SocketOptionForm.lua curves loaded`; `designed attack range (not June): 10% of attack … (first user: physic 115.9, magic 95.3)`; `first damage roll (physic): base 1159, range 115.9, maximize 0 -> 1190.07 (floor 1043.1, cap 1274.9)`; `first apply BBT 92, change type 1, value 22880`; no `is rank-linked`. Out of scope: 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ`.

**Decisions (user, 2026-10-06): "all June's flow and behaviour".** (1) Adopt June's `OptionItemData.lua` wholesale, without March's 247 IDs: kickoff `16-data`, before 16-step3b. (2) Keep June's template; nothing changes.

**Was open for the user.** (1) Adopt June's `OptionItemData.lua` (it needs the Enum additions above), June's plus March's 247 IDs, or keep March's. (2) Keep June's template given its reach on March's PERCENT crit/defence buffs (it is June's behaviour; a separate flag would let it be switched alone). /cost not available in-session.

### Phase 16 — Review (2026-10-06, Opus 5.5 high)

**Verdict.** Items 1-8 checked against June (raw disassembly; the template, the class dispatchers and the curve files were run, not read). There were three transcription errors, all fixed under `SERV_IRUHADEV_JUNE_SOCKET_PARITY`, and all dormant on March data. Everything else matches. New identifications are in `funcmap.tsv`, tagged "Buff Phase 16 review", and the old `32273480` row is corrected.

**Fixes** (`2c1fe79`):
1. **Rank-linked sockets** (`X2GUUser.cpp` `SumSocketOption`). In June `3204f190`, the rank-linked and PVE-rank-linked branches replace only `ConvertAndAdd`: copy `32092030`, scale it, then `32080670`. The enchant (`31fddef0`/`31fe09c0`) runs before the branch, and all three branches rejoin at `3204f485` (skill-level-up loop, hook, `+0x38` recursion). Part 1 skipped the whole socket. It now leaves out only the stat part, and the log says `its stats are not applied (enchant and skill level-up are)`.
2. **Loader** (`X2SocketItem.cpp`). June logs `m_bIsRankLinkedStat, m_bIsPVERankLinkedStat … must not both be set` (`@32736ee9`, `334b71b8`) and loads on. It is now a `[JUNE_BUFF]` line, like part 1's two `+0x29c` messages.
3. **NPC seeds** (`X2GameUnit.cpp` `InitAdditionalStatOptionValue`). June's NPC branch (`@32271234`..`32271531`) never reads the aggregate. It zeroes every new member except HP recovery, item drop and boss damage, which get **1.0** (`fld1 @322714a9/@322714ba/@322714cb`). Their basics are 1.0 too, so NPC results are 2.0. It is now written out explicitly. JUNE_BUGS #46.

**Items.**
- **1 Loader.** Every part-1 key has June's destination and its default of 0. The table is from a key-by-key sweep of `32726850`, getter, `lea`/`add` target and the store after a failed read.
  - The MP pair: `m_fMPMultiplyByAll` → `+0x124`/`+0x12c` (`@32732716`/`@32732725`), then `fadd` per `_ReturnRate`.
  - The `+0x29c` push matches: factor `[esp+0x54]`, level `[esp+0x60]`, kill byte, `+0x1dc == 15`, HP rate. Its branch sets the HAMEL byte `[esp+0x3b]` (`@32735bde`), the one the HAMEL gate reads (`@32736609`).
  - The ×100 tail matches.
  - **Outside part 1:** June's HAMEL custom-use routing also takes SIEE 13 and 17 (`+0x1dc` in {11, 13, 17}, `@3273661d`). That goes to 16-data / 16-step3b.
- **2 `ConvertAndAdd` `3207cd70`.** Both directions match: every June term against `X2SocketItem.h`, and every flag-resolved March `+=` (`flagprobe`: `MAGIC_NEC_FIX` on, `SKILL_LEVEL_UP_BY_POWER_RATE_TYPE` off, `SERV_9TH_NEW_CHARACTER` on).
  - The spec dispatchers `32073860`/`320739e0` take (level, unit type) and compute `(int)(rhs±0x134/0x138 × curve + 0.5)`.
  - Their four jump tables map types 1-9 to the same classes as March's `UNIT_TYPE`.
  - All 42 curve names exist in the staged `SocketOptionForm.src.lua`.
- **3 Seeds, basics, passive, refresh.** Every new member, its basic and its passive init match (`32270dd0`, `32271540`, `322718e0`). 92 has basic 0 and no passive init.
  - The refresh `32290950` passes the unit type in `EDI`: `*(CX2Unit+0xaf0)` when `+0x264 == 0`, else 0.
  - 24/25 refresh through `322a99e0`, 26 through `32073ba0`, 92 through `322a9e30`, and A/B through `322a9a10`/`322a9c20`. The eleven plain members use `3236d350`.
- **4 Template `320839e0`.** Emulated under Unicorn: 360 input sets, 0 mismatches. The basic-0, PERCENT-1.5 case gives 0 (March 0.5). The curve gets (option + FIX + RELATION, level, unit type).
- **5 BBT 92.**
  - Factory: table `32e84808`, index type − 1. Entries 78/92 → ctor `32e866f0`, and case 92 stores `0x5c`.
  - The SGU byte table sends 76/78/92/93 to `32d4d6ef`. Insert, update and erase all hit `+0x6d18`.
  - Roll `322788c0`:
    - base/range `+0x4424`/`+0x458c` (physic) and `+0x44d8`/`+0x4640` (magic);
    - maximize = 92 result `+0x6db8` × attacker slot 121(DD);
    - seed = this slot 113 `& 0xffff` (user `+0x80f8`, NPC `+0x8474`);
    - `N(base + range·M, 0.5·range)`, then `min(max(base − range + 2·range·M, draw), base + range)`, with ties as ported.
  - Call sites: this = `GetFinalDamage`'s this (`EDI`), arguments (DD attacker, DD).
  - Slot 121 is `fld1` everywhere except Rose.
  - Summon copy `3230ca6d`: `6d18`←`6db8`, `44ec`←`458c`, `45a0`←`4640`.
  - The vendored Boost (≤ 1.44) uses Box-Muller, like June.
- **6 78's k.** User: attacker (`[DD+0xc]`, `ESI`) `+0x526c`, plus this `+0x5450` and `+0x51b8`, minus the DD defender's `+0x5aec` ×0.01 only if > 0. NPC: the same with no > 0 test. Then `max(k, 0)`.
- **7 Gate `32001570`.** June types 2/3/5 = `GT_DUNGEON`/`GT_BATTLE_FIELD`/(none in March); 4 = `GT_PVE` rejects all.
  - Game vslot `+0x1c8` is `m_pDungeon->GetDungeonData()->m_eDungeonType` (`327c9ef0`), and 0 in a field (`3231f510`). That is March's `GetDungeonType`.
- **8 Designed range.** Users only, both `SetGameStat` overloads, one log line.

**Reach numbers, re-run** (bundled KOG LuaJIT, March's live `SocketOptionForm` bytecode against the staged June file; the factor file run under a stub manager):
- Every Done-note number reproduces: Linear level 1 9.5→10 and the cap at 143 above 90; move 0.230→0.255; crit 0.584→0.560; hyper 0.297→0.285; add-defence 0.4157→0.450; 366 factors, crit 25, add-defence 4, hyper 1, walk/run 34, jump 8.
- Two additions:
  - Final and Intermediate also differ at level 1, for the same 19/20 reason.
  - `CalculateFinalAdditionalAttackValue` differs above intermediate 40 (at 80: 0.276→0.263).
- Anim speed (37 PERCENT ≠ 1) is unaffected, because its basic is > 0 and its modifier is 1.

**What 16-data makes live** (June `OptionItemData.src.lua`, 13,408 entries):
- **185 rank-linked:** 120 rank, 65 PVE, none both. Fix 1 keeps their enchant (7 carry `RESIST_ENCHANT`), but their stats stay out until 16-step3b item 8 decodes the scaling.
- **About 280 `+0x29c` pushes.** Each sets the HAMEL byte, so the socket gets no `m_ptrBuffFactor`. Their buff does nothing until the readers (16-step3b item 7).
- **8 `DUNGEON_ONLY_TYPE` = `DT_SECRET`** (300000-300007).
- None of the save's 13 socket IDs is in any of these sets.

**Hygiene.** `flagcheck JUNE_SOCKET_PARITY --with JUNE_BUFF_NEW_STAT JUNE_ATTACK_RANGE_DESIGN JUNE_BUFF_DEBUG --worktree --files` (3 files): 3 PASS, RESULT PASS. The 11 deleted lines are part 1's own rank-skip block, moved. Post-commit `--range e485716^..HEAD`: 9 PASS, 1 PASS~, 1 DEFINE, 5 OTHER, 0 FAIL, RESULT PASS (the same 11 lines, deleted by `2c1fe79` from `e485716`'s insert); `--base e485716^ --at HEAD`: 0 deleted lines, RESULT PASS.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,204,608 B, dated 2026-10-06 07:40:56, md5 f42d59d2c5873a5ffa0037cb0b0853c9, equal to X2/US_SERVICE/x2.exe (2026-10-06 07:40:50); built at mods/june-buff-port cfbd8de + 3 uncommitted code file(s).` Both new log texts are in the exe (ASCII), and part 1's old skip text is gone. Not launched.

**Play-test (2026-10-06, user: passed; exe f42d59d2).** Logs: `June SocketOptionForm.lua curves loaded`; `designed attack range (not June): 10% … (first user: physic 115.9, magic 95.3)`; `first damage roll (physic): base 1159, range 115.9, maximize 0 -> 1155.33`; no `is rank-linked` / `must not both be set` (March data). Out of scope: 4 UNHANDLED `E_DISABLE_HB_CHECK_REQ`.

**Decision (user, 2026-10-06): accept the window.** 16-data runs next; 16-step3b does items 7-8 (conditional buffs, rank scaling) first.

**Was open for the user.** Before 16-data: accept the window above (185 rank-linked sockets without stats, about 280 conditional buffs inert until 16-step3b), or move 16-step3b items 7-8 ahead of 16-data. /cost not available in-session.

### Phase 16-data — Done (2026-10-06, Opus 5.5 high)

**Landed** (`01c297b`, all under `SERV_IRUHADEV_JUNE_SOCKET_PARITY`, no new flag): June's `OptionItemData.lua` wholesale (13,408 entries; March had 7,429), the Enum it needs (Lua and C++), June's SIEE gate and the 13/17 routing. Data staged in `march2014/data036/` and `kom_contents/data036_kom/` (manifest already listed both files). `PACK OK data036` (182 entries; vs live: ~ `Enum.lua`, ~ `OptionItemData.lua`); `COPY OK data036` (md5 f1ae990f, user OK; backup `kom_backup_tools/data036.kom.20261006_085804`).

**Enum** (`Enum.src.lua`, values from June's `Enum.src.lua`; Phase 2's comment style):
- `SOCKET_DATA_TYPE`: all 273 June-only members (122-…; none collides). `SOCKET_ITEM_EFFECT_EX`: 12-20. `HYPER_MODE_EFFECTIVE_TYPE`: `HMET_FEVER = 3`, `HMET_END` 3 → 4 (June).
- `EXTRA_DAMAGE_TYPE`: only the 18 June-only members OptionItemData names (68-72, 145-147, 149-157, 159). **Not taken:** June's `EDT_BEDCLOTHES_FEATHER = 66` (March's C++ has `EDT_EVENT_SMASH_3 = 66`) and June's renumbered `EDT_ENCHANT_*` block (45 conflicts, e.g. FROZEN 101 → 1011; March's C++ keeps 100-144). No June file names a raw EDT number.
- Global dump of the compiled Enum against the live one: 301 lines added, 1 changed (`HMET_END`), nothing removed.

**C++** (insert-only):
- `X2SocketItem.h`: the 273 SDT, SIEE 12-20, `HMET_FEVER`/`HMET_END = 4` (`#else` keeps 3). `X2DamageManager.h`: the 18 EDTs, explicit values.
- What March does with each (step 2):
  - SDT: March's code switches only on `SDT_SKILL_LV_GROUP` and the three grow-up types (`X2SocketItem.cpp` 542-581, 2830; `X2Unit.cpp` 4676); the new types take the default paths. No handler log needed.
  - EDT: the loader's switch had no case and dropped them silently. They now get explicit cases that drop them and log once per type.
  - SIEE 12-20: March's `InitSocketEffectEx` rejected them, so the socket fell through to the stat / extra-damage / buff tail. June `31fe4730` files every SIEE 1-20 (`dec`; `cmp eax,0x13`; `ja`) and `3204f190` then skips the socket's tail (`@3204f5f6`). **Ported:** 12-20 are filed (e.g. SIEE 19's `BUFF_FACTOR_ID` no longer becomes a permanent socket buff). No `ProcessSocketEffectEx` case (handlers not ported). SIEE 15 now reaches `HasSocketAuraFor`, which still answers false (16-step3b).
  - `HMET_FEVER`: pushed to the awakening list as before; the switch's `default` was an `ASSERT`. It now has an explicit no-op case.
- Loader routing `32726850 @3273661d..32736630`: HMET 0, else SIEE in {11, 13, 17} → custom-use pointer. 13 and 17 are added.
- Log, once per type at load: `socket N (first of its type): EXTRA_DAMAGE_TYPE / SIEE / HYPER_MODE_EFFECTIVE_TYPE V NAME … (not ported)`. That is 18 + 9 + 1 = 28 lines at most.

**STR_ID (step 3): nothing to stage.**
- Descriptions go `m_eDescStringID` → `m_mapSocketDescStrID` → `CKTDXStringTable` (`general.ess`, game directory).
- The game's `general.ess` has 45,160 entries (36743 "Free", 39285 "…Ballista bolt…"). The packed `StringID_def.lua` already defines all 49 IDs June's file uses.
- Only `StringID_def*.h` lacks the 20, and no C++ names them. Part 1's 20 came from a load test without `StringID_def.lua`. Its "80 SDT" is 69 distinct names; 97 unresolved = 69 + 9 + 18 + 1.

**Bytecode (step 4).**
- Compiled `-v mar-2014` (header `1b 4b 4c 81`).
- Under the bundled KOG LuaJIT, with a stub `g_pCX2SocketItem` and a trap on every Enum table and `_G`:
  - source Enum + source OptionItemData: 13,408 entries, 0 unresolved;
  - bytecode Enum + bytecode `StringID_def` + bytecode OptionItemData: the same dump, byte-identical at `%.17g` (md5 `c8e34870…`).
- md5 `Enum.lua a05b0333…`, `OptionItemData.lua 3e544cb7…`.

**Reach (step 5)**, on the data the game now loads:
- **IDs.** 247 March IDs gone, 6,226 June-only, 1,366 shared IDs differ (part 1 said 1,367; this compares every field at `%.17g`).
- **The save.** Its 13 socket IDs (34 items, `sockets`/`random_sockets`) are all identical in both files, and none is June-only, rank-linked, `+0x29c` or SIEE 12-20.
- **Emulator.** The roll pools (`SocketItemTable.lua`, 3,586 rows, 1,291 IDs) lose none; 840 of their IDs now carry June's rows. Built-in options lose none either: `TitleTable.lua` (266 IDs) and `SocketGroupDataForCashAvatar.lua` (22) name no dropped ID (one title ID is in neither file, as before). Set-item options in `Item.kim`: not counted. March's stat add (`ConvertAndAdd`, X2GUUser.cpp:3192) runs before the gate (:3234) and the buff insert (:3283) after it, the same order as June (`3204f485` < `3204f5ef`). `OptionItemDataTrans.lua`'s ID check is compiled out (`LUA_TRANS_DEVIDE` commented, `Always_Global.h:181`), and June's file has no duplicate ID (no `XEM_ERROR127` popup).
- **June-only keys,** entries per key:
  - skill power-rate pair: 2,491;
  - `m_bSocketExtraction`: 2,485;
  - `m_fDamageMaximizeRate`: 1,014;
  - MP return rates: 690 / 647;
  - specialization A/B: 612;
  - `m_SkillType`/`m_PowerRate`: 326;
  - `m_sBuffLevel`: 280;
  - rank: 120, PVE rank: 65;
  - `HYPER_MODE_EFFECTIVE_TYPE`: 32;
  - `m_fAllStatRelLV`: 30;
  - `DUNGEON_ONLY_TYPE`: 8;
  - 24 more under 100.
- **New types.** SDT ≥ 122 on 3,219 entries. SIEE 12-20 on 259 (15: 180, 13: 27, 14: 17, others ≤ 9). New EDTs on 179. `HMET_FEVER` on 1.
- `SOCKET_DATA_TYPE` is overwritten by the emulator's 122-member seed when `CX2OfflineLuaEnum::Publish` runs at login. `OptionItemData.lua` loads earlier, at StartUp, so it is not affected. The seed is left as is: `SocketItemTable.lua` names March types only.

**Hygiene.** `flagcheck JUNE_SOCKET_PARITY --worktree --files` (4 files): 4 PASS, 0 deleted, 0 non-ASCII, 0 EOL-mismatched, RESULT PASS. Post-commit `--range 01c297b^..HEAD`: 4 PASS, 0 OTHER, 0 FAIL, 0 deleted, RESULT PASS.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,207,168 B, dated 2026-10-06 08:23:00, md5 6184b9fd9ff94e88f2eb33edeac6c6f1, equal to X2/US_SERVICE/x2.exe (2026-10-06 08:22:56); built at mods/june-buff-port 59dd026 + 4 uncommitted code file(s).` The three new log texts and the type names are in the exe (ASCII). Not launched.

**How to see it in game.** At load, `[JUNE_BUFF]` lines in `ErrorLog.txt`: one per June-only EDT / SIEE / HMET type, plus the rank-linked lines (`is rank-linked … (enchant and skill level-up are)`), now reachable. The equipped sockets read and work as before (identical rows, same description string IDs).

**Play-test 1 (2026-10-06, exe 6184b9fd + copied data036): logs read.** Every June-only type logged once, as designed: 18 EDT, SIEE 12-20, `HMET_FEVER` (socket 74042). Also `June SocketOptionForm.lua curves loaded`, the designed range and the first damage roll. No `is rank-linked` line: the save has none. Two findings:
- **Fixed: 180 × `SIEE 15 without both BUFF_FACTOR_ID and m_sBuffLevel`** (100012000-…), although June's data sets both.
  - Cause: March reads `EFFECT_NAME` early (`X2SocketItem.cpp`, before part 1's June keys) and, like June, never calls `EndTable`. Every key after it therefore read as its default for the 236 sockets that have a list.
  - June reads the list after `m_fAllSkillDamageIncreaseRate` (`@32736174`; BeginTable `32fe9940`, GetValue `32feafd0`, no EndTable). Then only `m_fEffectiveValue`, HMET, level-linked, `m_eDescStringID` and the rank flags follow it, and of June's data only socket 2000171's description is lost.
  - The read now sits at June's place under `_SOCKET_PARITY` (March's place in `#else`). June's missing EndTable is kept: JUNE_BUGS #47.
  - `DEPLOY OK: X2_offline.exe is 19,207,168 B, dated 2026-10-06 09:06:18, md5 aa173377797fe2e79226b223ec3718a6, equal to X2/US_SERVICE/x2.exe (2026-10-06 09:06:13); built at mods/june-buff-port 4e1de6b + 1 uncommitted code file(s).`
- **Expected, not fixed:** 55 `BFI N requested but not loaded` (52 + 3 `_INT`). These are June socket buffs whose factors are not in March's staged factor file; they arrive with the June buff data (Phase 13). Those sockets carry no buff until then.

**Play-test 2 (2026-10-06, exe aa173377): PASSED, logs read.** 0 `without both` lines (180 before), the 28 per-type lines; `BFI … not loaded` 52 → 42 (the SIEE 15 factors now go to the conditional list, as in June, not through `GetBuffFactorPtr`).

**Decided (user, 2026-10-06).** The 18 EDT handlers and `HMET_FEVER` go to a new **16-step3c** (after 16-step3b and june-effect C5; kickoff written). SIEE 12-20 stay 16-step3b item 10. /cost not available in-session.

### Phase 16-data — Review (2026-10-06, Opus 5.5 high)

**Verdict: PASS. No code change, no transcription error.** Two reach counts in the Done note were over-counted, and that has no code consequence. The review covered `01c297b` and the play-test fix `9fa7e16`.

**Hygiene.** `flagcheck SERV_IRUHADEV_JUNE_SOCKET_PARITY --range 01c297b^..HEAD`:
- `files: 5 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
- `in this flag's files: 0 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
- `RESULT: PASS`

**1. Lua Enum.**
- Source check: each of the 301 added members (SDT 273, SIEE 9, EDT 18, `HMET_FEVER`) equals June's `Enum.src.lua` value. Only `HMET_END` changed (3 → 4, June's value). No table has a duplicate value.
- Shipped bytecode, every global dumped: the pre-copy backup (`kom_backup_tools/data036.kom.20261006_085804`) against the live `data036.kom` gives 301 added, 1 changed, 0 removed. No peer edit rode along.
- The 34 June-only EDT names that were not taken, the 45 renumbered `EDT_ENCHANT_*` and `EDT_BEDCLOTHES_FEATHER` appear in no staged file under `kom_contents/` or `march2014/`. Only June's `AttribEnchantItem` names them, and it is not staged.

**2. C++ enumerators.** Checked with `cl /P` on a probe that includes `StdAfx.h`, `X2DamageManager.h` and `X2SocketItem.h`, using X2Lib's US_SERVICE flags, then evaluating the four enums:
- every member shared with the Lua has the same value, and no enum has a duplicate value;
- `HMET_*` = −1..4 (`HMET_FEVER` 3, `HMET_END` 4);
- nothing reads `HMET_END` (and no `SIEE_END`/`EDT_END` exists).
- The 3 Lua-only EDTs (67, 148, 158) predate this phase.

**3. The gate.**
- Ghidra `31fe4730 @31fe4769`: `dec`, `cmp eax,0x13`, `ja`. `3204f190 @3204f5ef` calls the gate and `@3204f5f6` jumps to `320500f0`, the loop increment.
- March: the nine new `case` labels fall through to the filing code. `default` still returns false.
- `m_mapSocketItemEffectEx` is only cleared and `find`-ed (recursive grep over `X2Lib`, `KTDXLIB`, `X2`). Every `ProcessSocketEffectEx` caller passes a fixed SIEE between 1 and 11.
- `GetSocketDescEx`'s `default` writes `L""`.
- Stale comment: `HasSocketAuraFor`'s `@brief` still says nothing is filed under 15. Noted in `16-step3b.md`.

**4. Routing.** Ghidra `32726850 @32736614..32736630`: `[ESI+0x280]` (HMET) == 0, else `[ESI+0x1e0]` (SIEE) in {0xb, 0xd, 0x11} → custom use.
- The fields are identified by their keys: SIEE's store `@327319cf` follows `m_eTypeSetItemEffectEx` (`327316de`), and HMET's follows `32736426`.
- The loader's `ESI` sits 4 bytes below the socket base, so these are the gate's `+0x1dc` and the funcmap's `+0x27c`.
- The port gives the same set.

**4b. `9fa7e16` (EFFECT_NAME).**
- June's EndTable is `32fe9a10` (settop −2, depth −1). The loader calls BeginTable 8× and EndTable 7×, and no EndTable sits between the `EFFECT_NAME` BeginTable (`@32736174`, plain string `334b7238`) and the next key. JUNE_BUGS #47 holds.
- June's keys after it: `m_fEffectiveValue`, `HYPER_MODE_EFFECTIVE_TYPE`, `m_bIsLevelLinkedStat`, `m_iAddLevel`, `m_eDescStringID` and both rank flags. The port reads exactly these.
- March's `m_vecGrowUpSocketID` is compiled out: `SERV_GROW_UP_SOCKET` does not reach X2Lib, and the preprocessed header has no `m_mapGrowUpSocketID`.

**5. Bytecode.** Archive `data036.kom` md5 f1ae990f, extracted to scratch with `elstools extract -v mar-2014`. Entries: `Enum.lua` a05b0333, `OptionItemData.lua` 3e544cb7.
- Under `elstools/vendor/luajit`, with a stub `g_pCX2SocketItem` and a trap on `_G` and every Enum table:
  - the live bytecode (Enum + `StringID_def` + OptionItemData) and the source (`march2014` Enum + `june_src_fixed` OptionItemData) each give 13,408 entries and 0 unresolved names;
  - the two dumps are byte-identical. The md5 differs from the note's `c8e34870` only because this review's serializer differs.
- Control: the pre-phase Enum gives 97 unresolved, as the note says.
- The staged source md5 equals June's.
- No loose `Enum`/`OptionItemData`/`StringID_def` `.lua` sits in the game directory (`os.listdir`), so the archive is what loads.

**6. Reach.** Recounted on live against March (backup):
- confirmed: 7,429 → 13,408 entries; 247 gone, 6,226 June-only, 1,366 shared IDs differ; all 39 June-only key counts; SDT ≥ 122 on 3,219; rank 120, PVE rank 65; `HMET_FEVER` 1; the save's 13 IDs identical.
- **Corrected:**
  - SIEE 12-20 are on **248** entries, not 259: 12:8, 13:28, 14:18, 15:180, 16:9, 17:1, 18:2, 19:1, 20:1.
  - The 18 new EDTs are on **160** entries, not 179.
  - Both counts are confirmed by a grep of the source.
  - The counts in `16-step3b.md` and `16-step3c.md` are corrected.

**Deploy.** No code change, so no rebuild. The deployed `X2_offline.exe` md5 is aa173377, the same as `X2/US_SERVICE/x2.exe`. Play-test 2 passed on that exe. No play-test is owed.

### Phase 16-step3b — Done, items 7-8 (2026-10-06, Opus 5.5 high)

**Landed** under `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (first-apply lines under `_DEBUG`), one commit per item: `7199cd5` (item 7) and `1d511ed` (item 8). New rows in `june2017_re/funcmap.tsv` are tagged "Buff Phase 16-step3b"; row `322d6f20` is corrected. Items 1-6, 9 and 10 move to **16-step3b2** (`kickoffs/june-buff/16-step3b2.md`).

**Item 7: the `+0x29c` conditional buffs.** Corrections to step 1: entry `+0xc` is not an HP rate. It is the key `m_fIncreaseDmgEnemyHPFew_HpRate` read as an int and used as a **skill category** (June data: 1-4, 20 sockets each, "Upon using … Skill"). The kill hook buffs **my unit**, not the attacker, and its 0x287 loop walks NPCs, not users.
- **Skill-use reader** (`CommonStateStart @32060bda..32060ccb`; ported after the CAN_MANA_BREAK block). For every user unit (June has no IsMyUnit test): when the now state has a skill (slot 329 = `GetNowStateSkillID`) and is a skill-slot state, each entry whose `+0xc` equals `32070510`(skill templet) applies its factor at its level.
  - June's gate is state byte `+0x11` (slot 330). All 12 June classes set it on the slot's normal and hyper state in slot 493 (`SetEquippedSkillFuncBySkillSlotIndex`, e.g. Elsword `32d2248a`). March has no such byte, so `IsJuneSkillSlotState` (the four `_CONST_GUUSER_` arrays) stands for it. Rose's extra class states (`32adbc95`) go to checklist §7.
  - Not ported: June's slot 474 call in the same block (`31fe9ed0`, skill-memo type 14).
  - JUNE_BUGS #48: kill-only and SIEE-15 entries carry `+0xc` = 0, and so does a relationship skill's category, so a couple skill fires them.
- **Kill hook** `CX2Game::ApplyBuffFactorOnNpcKill` = `322d6f20`. It is called only from the dungeon die handler (`@3230bd2b`, when `m_uiAttUnit != -1`); the battlefield handler does not call it, as in June. It is ported whole: the NPC members `+0x9058`/`+0x9048` are on `CX2GUNPC`. It is **dormant**: their only writer, `3277cf10`, reads a June-only stage-load map (`+0x78`, keyed by NPC UID, read at sub-stage start `327cf620`) that the server sends. A kill-flag socket logs `socket kill buff BFI … not applied` once.
- Reach on June's data: 280 entries = 180 SIEE 15 (item 10) + 80 skill-category (40 reachable: categories 3/4 need SA grades 15/16, which March lacks) + 20 kill (dormant). None is in the save.

**Item 8: rank-linked scaling** (`3204f190 @3204f2a8/@3204f351`). The rate comes from `g_pData` tables filled by client Lua, not from `CX2Unit` maps.
- `SocketData::SetJuneRankLinkedScaled` = `320925f0`: the stat block plus 34 fields, each × rate; the `+0x29c` entries are copied; everything else stays default (JUNE_BUGS #50, latent).
- The copy is then `ConvertAndAdd`'ed in place of the socket. The enchant, skill level-up and monster sets still come from the socket.
- Rate: rank-linked uses the PVP table (`31edfe70`) at `m_cRank`. PVE-rank-linked uses the PVE table (`31edfeb0`), or its MP table when `+0x124 > 0 && == +0x12c`. The MP table is compared against the wrong end (JUNE_BUGS #49); a miss reads 0 here, the one deviation.
- Tables: `AddPvpReferData_LUA`, `AddPveReferData_LUA` and `AddPveReferDataForMP_LUA` (map insert; first rate kept). They are loaded by June's `PVPData.lua`/`PVEData.lua`, run after the event-scene scripts (June `3258c470`). The other 7 calls in those files are logging no-ops: `SetPVPRewardItemList`, `SetPVPRewardRankLinkedItemList`, `AddContinueKillSoundEffect`, `SetSendPveGameSyncPacketTimer`, `SetPveBalanceBuffGiveScore`, `SetPveGameTimeStopDistance`, `SetPveAreaTypeByPortalLineMap`.
- Enum (Lua): `PVPRANK_RANK_STAR = 10` and June's `PVE_RANK` table. Without them, `PVEData.lua` aborts at its first line.
- Bytecode equals source (stub `g_pData`, KOG LuaJIT): PVP 11 rows (ARRANGE 1.0 … STAR 7.5), PVE 10, PVE-for-MP 10.
- `PACK OK data036` (184 entries; vs live: +PVEData.lua, +PVPData.lua, ~Enum.lua). `COPY OK data036` (184 entries, md5 2f9247de, user OK; backup `kom_backup_tools/data036.kom.20261006_101131`).
- Reach: 120 rank + 65 PVE sockets, none with `EFFECT_NAME` (#47) and none with a dropped key; 1 PVE socket is an MP socket. Offline every character's PvP rank is ARRANGE (`DefaultPvpRank`), so rate 1.0. The PvE rank (June `CX2Unit +0xa00 >= 5 ? +0xa0c : 0`) has no March source, so the key is 0: rate 1.0, and 0 for the MP socket. None of these sockets is in the save.
- Not ported: June's `+0x87b8` resize after every ConvertAndAdd. It is the per-entry cooldown timer vector of June's 0x18-byte `m_vecMpUpByAttacked` element (ticked by `31fda180`, read in `DamageReact` via `31fd9100 @32047d55`); March keeps that timer in the element's `.w`. It is open, outside rank scaling.

**Hygiene.** Item 7 `--worktree --files` (7 files): 5 PASS, 2 PASS~ (a blank separator line), RESULT PASS. The 2 deleted lines are this flag's own `m_iEnemyHPFewHpRate` rename. Item 8 (6 files): 6 PASS, RESULT PASS. The 7 deleted lines are part 1's rank-skip body, replaced. Post-commit `--range 7199cd5^..HEAD --with JUNE_BUFF_DEBUG`: `files: 11 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 9 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS` (9 = 2 rename + 7 rank-skip).

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,225,088 B, dated 2026-10-06 10:06:54, md5 2902bd16759e5992f49c4bb06aa79de3, equal to X2/US_SERVICE/x2.exe (2026-10-06 10:06:27); built at mods/june-buff-port 7199cd5 + 6 uncommitted code file(s).` Those 6 files are item 8's. The new log texts and the 10 binding names are in the exe. Not launched.

**How to see it in game.** At load, `ErrorLog.txt` shows `[JUNE_BUFF] June rank tables: PVP 11, PVE 10, PVE for MP 10 row(s)` once data036 is copied (before that: the two `did not load` lines and `0, 0, 0`), plus one `June-only, not ported: g_pData:…` line per call above. In play, skills and dungeon kills behave as before: the save has no socket that reaches either reader.

**Play-test (2026-10-06, user; exe 2902bd16 + data036 2f9247de; logs read).** `ErrorLog.txt:238-244`: the seven `June-only, not ported: g_pData:…` lines; `:245` `June rank tables: PVP 11, PVE 10, PVE for MP 10 row(s)`; no `did not load`. A dungeon run (sub-stages 0 and 1, skills and kills) with a clean exit: 0 EXCEPTION; 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` (unrelated, as before). As expected, there were no skill-use, kill or rank-linked lines, because the save has no socket that reaches them. Those paths are verified by code reading only (16-step3b review).

**Decisions (user, 2026-10-06: "follow which is closest to June").** (1) Kill path stays dormant: "absolute (Supreme) monsters" exist only in June's Hero Dungeon mode (`HeroDungeonData_Client.lua`, `Add_HeroDungeon_AbsoluteMonsterTeleport`; the 20 sockets are `BFI_BUFF_HERO_DUNGEON_KILL_ABSOL_*` items), which March does not have, so June itself never fires them in any dungeon March has. (2) PvE rank key 0 stays: it is what June gives a character outside its June-only PvE mode. No stand-in for either.

### Phase 16-step3b — Review (2026-10-06, Opus 5.5 high)

**Verdict.** Items 7 and 8 match June (Ghidra and raw disassembly, read-only). The review found one transcription gap in the function item 8 edits (June `3204f190`) and fixed it (`14879a2`) under `SERV_IRUHADEV_JUNE_SOCKET_PARITY`. The fix is dormant offline. Everything else checked is unchanged.

**Fix: the unit type every ConvertAndAdd gets** (`X2GUUser.cpp` `SumSocketOption`).
- June computes `ESI = *(CX2Unit +0xaf0)` once, before the three-way branch (`@3204f28a..3204f2a6`; 0 when that pointer is null). It pushes `ESI` as the unit type in all three calls to `32080670`: `@3204f30d` (rank), `@3204f3df` (PVE rank) and `@3204f449` (plain). No CX2Unit (`@3204f290`) skips all three.
- March passed the local player's type (`GetMyUser()->GetSelectUnit()->GetType()`) for every user. Item 8 copied that into its rank branch.
- Now `eJuneUnitType` = `GetUnit()->GetType()`, or `UT_NONE` without a templet, is used in all three calls. The plain branch is gated on `GetUnit()` like June. March's block stays in the `#else`.
- This corrects the 16 review's "ConvertAndAdd matches" at the call site. That review checked `3207cd70`'s terms, not what the caller passes.
- Under the flag, the type only selects the spec A/B curves (612 June sockets carry `m_fCharacterSpecializationA`/`B`); Add's 0.5 reform is already gone.
- **Dormant offline.** AI-party bots are NPC slots (`X2Game.cpp` moves them into `m_vecNpcSlot`), not `CX2GUUser`, so the player is the only user that sums sockets, and the player's own type is the same value as before. A PvP opponent or real party member would get its own class's curve, as in June.
- Also: part 1's comment on `m_bIsRankLinkedStat` (`X2SocketItem.h`) no longer says "not decoded". New funcmap row `3204f190 @3204f28a`.

**Item 7, skill-use reader** (`@32060bda..32060ccb`):
- The gate order is June's: slot 329 `!= 0` → slot 474 (not ported) → slot 330(0) `== 1` → `g_pData+0x50` → loop. `GetSkillTemplet(slot329(), 1)` runs per entry and is compared with `+0xc`. Slot 135 is called with `(BFI, 0, level, 0, 1)`; arg 5 = 1 only skips slot 476's skill-memo scaling (`322a6960`), a system March does not have.
- There is no IsMyUnit test.
- `32070510`'s jump table (`3207054c`) maps 5→6, 6→1, 7/14→2, 13→5, 15→3, 16→4, else 0. March's `SKILL_TYPE` has the same numbers (`ST_BUFF` 5 … `ST_HYPER_ACTIVE_SKILL` 13).
- **The slot-state stand-in holds.** Elsword's slot 493 (`32d223e0`) sets `+0x11` on the normal state from `33521204`/`3352132c` (slot A/B) and `+0x10..11 = 0x101` on the hyper state from `33521318`/`33521340`. Those are June's arrays `{50..58}`, `{51..59}`, `{60..68}`, `{61..69}`, i.e. the four `_CONST_GUUSER_` arrays. All 9 March classes enter a skill only through those arrays (each class's `SetEquippedSkillFuncBySkillSlotIndex` and `StateChange` sites).

**Item 7, kill hook** (`322d6f20`):
- It uses `GetMyUnit` (game slot 0x98 = `3231f550`). The `+0x9058` gate and the kill byte `+8` loop over `+0x29c` are June's.
- The death list `+0x9048` goes to my unit at level 1. `0x287` gives `0x286` to every other NPC except the killed one, skipping an NPC whose AI (`+0x7c98`) has `+0x38 == 1` (`NAT_ALLY`; a null AI is buffed).
- Game slots 0xf4 = `3231f530` (NPC list size, `+0x324`) and 0xb8 = `322ccba0` (`GetNPCUnit`, index > 0x4f → null).
- Call site `@3230bd1b`: the 64-bit `m_uiAttUnit` (`UidType`) `!= -1`, before `GetOwnerGameUnit`. March's battlefield override does not reach the base handler either.

**Item 8, scaled copy** (`320925f0`, `RET 0x398` = templet by value + rate):
- The 13-dword stat block is copied and every element scaled (March has 11 of them; June's range P/M have no March field).
- 17 fields are scaled at the same offsets in and out (`+0xd0 … +0x13c`), the `+0x29c` list is cleared and re-pushed (`3207b5d0`), and `+0x2ac..+0x2ec` (17) are scaled. These are exactly the 34 fields in `SetJuneRankLinkedScaled`.
- March's `SocketData()` gives no summed member a non-zero default (the one 1.0, `m_fAllAttackEnchantRate`, is multiplied).
- Lookups: `31edfe70` (`+0x3b4`, 0 when absent) and `31edfeb0` (`+0x404`/`+0x3f4`, both compared with `+0x3f8`, JUNE_BUGS #49).
- Rank key: the protected char `+0x114` is sign-extended (`movsx`). Its only other reader is the getter `32071890`.
- MP flag from the x87 tests: `0 < +0x124` (`TEST AH,5`/`JP`) and `+0x124 == +0x12c` (`TEST AH,0x44`/`JNP`).
- The three bindings match.

**Item 8, data** (live `data036.kom` md5 2f9247de, extracted with `elstools -v mar-2014`, run under the bundled KOG LuaJIT with a stub `g_pData`):
- Tables: PVP 11 (`0=1 1=1 2=3.5 … 10=7.5`), PVE 10 (`0=1 … 9=7.5`), PVE for MP 10 (`0..5=0, 6=1, 7=3, 8=5, 9=6`). The MP socket's rate 0 at key 0 is a real table value, not a miss.
- Only the 7 named June-only calls.
- Enum, dumped against the previous live one (backup `.20261006_101131`, f1ae990f): `PVE_RANK` (11 values) and `PVP_RANK.PVPRANK_RANK_STAR = 10` added, nothing else. `PVP_RANK` 0-9 equals March's C++ `CX2PVPEmblem::PVP_RANK`.

**Hygiene.**
- Fix, `--worktree --files X2GUUser.cpp X2SocketItem.h --with JUNE_BUFF_DEBUG`: 2 PASS, RESULT PASS; 3 deleted lines (item 8's two `GetMyUser` lines, part 1's one comment line).
- Range before the fix, `--range 7199cd5^..HEAD --with JUNE_BUFF_DEBUG`:
  - `files: 11 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
  - `in this flag's files: 9 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
  - `RESULT: PASS`
  - The 9 deleted lines are the Done note's: 2 rename + 7 rank-skip. The two PASS~ are one blank line each.
- Post-commit range: after the fix commit `14879a2`, `--range 7199cd5^..HEAD --with JUNE_BUFF_DEBUG`: `files: 13 PASS, 2 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 12 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 12 deleted lines are the 9 above plus the fix's 3.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,224,576 B, dated 2026-10-06 10:37:46, md5 6cce4121c5534b3c5f1150775c8277c0, equal to X2/US_SERVICE/x2.exe (2026-10-06 10:37:42); built at mods/june-buff-port eb03b5a + 2 uncommitted code file(s).` Those two files are `14879a2`'s (eb03b5a is a peer's docs-only commit). data036 is unchanged (2f9247de). Not launched.

**How to see it in game.** Nothing changes for the player: no ConvertAndAdd result moves, because the player's own unit type is the value March already passed. The play-test is a regression run: load, a dungeon with skills and kills, and the same `[JUNE_BUFF]` load lines as the 16-step3b play-test.

**Play-test (2026-10-06, user: passed; exe 6cce4121, data036 2f9247de; logs read).** The run started after the deploy (`ErrorLog.txt` 10:46:34). `ErrorLog.txt:238-244`: the seven `June-only, not ported: g_pData:…` lines; `:245` `June rank tables: PVP 11, PVE 10, PVE for MP 10 row(s)`; no `did not load`. Field → dungeon (`State.log:142`, 75 `NPC_UNIT_DIE` packets) → field, clean exit (census present): 0 EXCEPTION. 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` are unrelated, as before. There were no rank-linked, skill-use or kill lines, as expected: the save has no socket that reaches them.

**Open for the user.** Nothing new. /cost not available in-session.

### Phase 16-step3b2 — Done, items 1-4 and 6 (2026-10-06, Opus 5.5 high)

**Landed** under `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (first-apply lines under `_DEBUG`): `e002c27` items 1-2, `e002237` item 3a, `60fd058` items 4 and 6, `fb92b03` the boss grade from the server packet + an anti-critical debug line. Decodes: 19 `funcmap.tsv` rows tagged "Buff Phase 16-step3b2". **Split at the item boundary:** items 3b, 5 and the rest of 9 go to **16-step3b3**, and item 10 to **16-step3b4** (kickoffs written).

**Item 1, `GetFinalDamage`** (the x87 blocks were run under Unicorn).
- User attacker only, after the pet-aura block (`@322852db`). Each term multiplies physic and magic attack by 1 + r:
  - enemy-HP-few, when the defender's HP is at most 30% and r ≠ 0;
  - the attacker's own 52fc, when r > 0;
  - my-HP-few, as r × (1 − HP ratio).
- Piercing (`@32285849`): DT_PHYSIC, DT_MAGIC and DT_MIX defence × (1 − min(r, 0.2)), before the piercing factor. DT_MIX scales each side before the average. The weapon types read no rate.
- Damage to boss (`@32286519`): damage × the attacker's result.
  - Boss means June templet `+0xdc` == 1, the monster grade that `323752f0` stores, which is the server's `KNPCUnitReq::m_cMonsterGrade`. Here it is that packet value (`m_cJuneSocketMonsterGrade`, set by the battlefield constructor). Dungeon NPCs lose it at `AddNPCData`, and the offline servers send 0 or 2, so the boss terms are **dormant offline**. (A first version reused the ExtraNPCStat port's designed grade; it was replaced before hand-off as a stand-in nobody approved.)
  - June's Space-of-Thought route (`32863880`, custom event manager `+0x604`) has neither half in March.
- My-HP-few defence (`@3228692c`): damage × (1 − r), before the clamp.

**Item 2, the DamageReacts.**
- User reduction (`@32048845`), with an NPC attacker: + boss defence (boss attacker), − this user's own 52fc (when > 0). The sum is then capped at 1.0, as June does. 52fc's two roles are now settled: more attack, and more damage taken from monsters.
- Critical chance: the attacker's rate + `32273480` is compared with a bare random (vslot `0x1cc` = `3208e0d0`).
  - `32273480` = panic (`+0x2b28`/`+0x2bc4` = `m_Panic`) − the anti-critical result − socket anti-critical × 0.01.
  - Sign settled: March subtracted the terms from the random, so anti-critical **raised** the chance; June lowers it. June's data follows the new sign except `BFI_DEBUFF_PANIC_PANDEMONIUM` (49, +0.15), which now protects its target (JUNE_BUGS #53; March's staged value is 1.15, so here the debuff blocks every critical hit).
- Critical damage (`GetJuneCriticalDamageRate`, both DamageReacts):
  - user attacker: + magic rate / half magic + half physic rate / physic rate, by damage type; + `m_fApplyCriticalDamage`;
  - user DamageReact only: + the socket critical-afflicted option;
  - then × critical defence (`32071ab0`). The ≥ 1 test now applies to every type.
  - The physic rate (June `+0x8dfc`, `32070e00`) is a new user member at 0; its writer is class code (checklist §7).

**Item 3a.** `32275ba0` ported whole:
- AT_SPECIAL: × (1 + skill damage + all-skill damage, without the all-skill term for a hyper APPLIED_SKILL_ID);
- AT_NORMAL: + all-skill damage;
- any other type: that type's rate.

March's Lua never sets APPLIED_SKILL_ID, so the hyper test is always false.

**Item 4.** The host is `GetActualMPConsume` (`3203cf60`, slot 423). June applies the cut only when its third (bool) argument is set: `31fd1840` passes it through, and the slot refresh `31fd6880` passes 1. March has no such argument (Phase 6b deviation). **Not applied:** one `[JUNE_BUFF]` line when a socket asks.

**Item 5.** `322fbc60` is `CX2Game::UseItemSpecialAbility`: HP recovery scales item healing (rates decoded in the funcmap row). The per-case port moves to 3b3.

**Item 6.**
- The `CX2Unit` map `+0xe78`:
  - cleared in `InitSocketData` (`@3206d909`);
  - filled at the end of each `SumSocketOption` socket (`@32050009`) from the templet: slots 1-4, then the all-skill cut to 1/2/3/4/6;
  - read through 6b's `ApplySkillCoolTimeRateByBuff`: rate = max(factor, 0.8). `32358480` is still 1.0.
- `32358770` re-checked: it fits the call (BBT 100 on the cool time).
- The 2nd and 3rd SA cuts are dormant, because March has no SA grades 14-16.

**Item 9, part.** `m_bSocketExtraction`, `m_iSocketApply` and `m_iSocketCondition` are read only by June's item socket-extraction system (`32883300`/`32883450`), which March lacks: no port. The first `m_sBuffLevel` (`+0x296`) has no reader. `m_fSkillAdditionalPowerRate` (2,491 June sockets) and `m_fAllStatRelLV` go to 3b3.

**Reach** (June `OptionItemData` entries; none in the save's 13 sockets): piercing 20 + 20, damage to boss 41, boss defence 40, enemy-HP-few 41, my-HP-few defence 40 / damage 20, 52fc 20, anti-critical 3, critical afflicted 0, all-skill damage 28, skill damage up 29, MP cost 25 (not applied), cool-time cuts 20 per slot + all-skill 25. **Not socket-gated** (June behaviour, now live): the anti-critical sign (every anti-critical buff, Panic), the ≥ 1 test on magic critical damage, DT_MIX taking half of El's magic critical rate, the reduction cap.

**Seen, not owned:** the NPC `+0x8fd4` multiplier, `+0x412c`, DamageData `+0x620`/`+0x624`, and the frozen-resist scaling in `GetFinalDamage`; the `+0x896c` socket-ID vector and socket 2000145's user flag `+0x7ca8` in `SumSocketOption`.

**JUNE_BUGS** #51 (my-HP-few has no MaxHp test; latent), #52 (an NPC attacker deals ×2 to bosses via #46; latent offline), #53 (the Panic debuff's anti-critical value under June's sign; reached).

**Hygiene.** `--worktree --files` before each commit: all PASS. Post-commit `--range e002c27^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_NEW_SKILL JUNE_DAMAGEDATA EXTRA_NPC_STAT`: `files: 13 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 8 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. All 8 deleted lines are this flag's own: part 1's `GetSocketAntiCriticalRate` comment, item 7's `static` on `JuneSkillCategory`, and the first version's boss-grade getter (6).

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,230,208 B, dated 2026-10-06 11:56:09, md5 f17026d45f4197851ca94b185b24c37c, equal to X2/US_SERVICE/x2.exe (2026-10-06 11:56:04); built at mods/june-buff-port fec3dc2 + 3 uncommitted code file(s).` Those three files are `fb92b03`'s. The new log texts are in the exe. Not launched.

**How to see it in game.** The save has no socket that reaches these readers, so expect no `[JUNE_BUFF] first socket …` lines and no MP-cost line in a dungeon run with skills, critical hits and kills. The one live change that can be seen is the anti-critical sign: put `BFI_DEBUFF_PANIC_PANDEMONIUM` (49) on every monster from the console. The log then shows `first critical threshold term: anti-critical …`, with June's chance negative and March's positive.

**Play-test (2026-10-06, user: all passed; exe f17026d4, logs read).** The run started after the deploy, exited cleanly (census present), 0 EXCEPTION; 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` (unrelated, as before). `ErrorLog.txt:539` `first critical threshold term: anti-critical 1.15, panic time 0 -> attacker's chance -1.15 (March's sign: +1.15)` after BFI 49 on every monster (March's staged value 1.15, June's 0.15: JUNE_BUGS #53). No `first socket …` and no MP-cost line, as expected.

**Decisions (user, 2026-10-06: "follow June").** (1) Item 4: port June's bool argument through `GetActualMPConsume` (June slots 423/424 and the class overrides) and apply the cut where June passes true: added to **16-step3b3** as its first item. (2) Boss terms, **revised by the user the same day ("the proper way")**: no client-side stand-in; the offline server must rank the mobs as the real server did. The real server read `MONSTER_GRADE` from each dungeon stage script (`XSLDungeonSubStage.cpp:197`) and sent it (`XSLDungeon.cpp:391`), and 227 of the client's stage scripts carry the key. The work is a new offline plan, to be created in plan mode (kickoff prompt given to the user, 2026-10-06); the boss terms ported here need no change and go live when it lands, together with JUNE_BUGS #52.

### Phase 16-step3b2 — Review (2026-10-06, Opus 5.5 high)

**Verdict.** Items 1-4 and 6 match June (raw disassembly; the piercing switch run under Unicorn), except the boss grade. The Done note's source for it was wrong, and the fix (`60d0571`, `SERV_IRUHADEV_JUNE_SOCKET_PARITY`) **makes the boss terms live on battlefield middle bosses**. One line to revert it is below.

**Fix: the boss grade is June's battlefield value, not the packet's `m_cMonsterGrade`.**
- `+0xdc` is written only by `323752f0`. The fetch `32384d10` calls it only for an NPC ID with an ExtraNPCStat row (`LANOX_EXTRA_NPC_STAT_PLAN.md` "What each fetch caller passes"), so every other NPC stays at the ctor's 0.
- The grade is `32384d10`'s 3rd argument:
  - battlefield `32d528f0`: `KNPCUnitReq +0x52 != 0`. June's `AddNPCData` copies req `+0x52` to NPCData `+0x3d` (`327edc2d`), so that field is `m_bShowBossName`. The Done note's "packet char" at `32d52951` is the level (`+8`).
  - battlefield `32d539c0`: `NpcCreatedInfo +0xc > 0` = `m_uiBossGroupId` (`@32d539f5`).
  - dungeon `327ef570`: NPCData `+0x44` = `m_cMonsterGrade` (`@327edc4f`).
  - `3230d5c9`: 0.
- `IsJuneSocketBossGrade()` is now `m_iExtraNPCStatMonsterGrade == 1` **and** `HasExtraNPCStat( id )` (`X2GUNPC.cpp`, guarded by `SOCKET_PARITY && EXTRA_NPC_STAT`).
  - That member is the ExtraNPCStat port's per-spawn copy of exactly these two battlefield values (`X2BattleFieldGame.cpp:786/:1000`).
  - `m_cJuneSocketMonsterGrade` and its ctor copy are gone (16 of this flag's own lines deleted).
  - So the Done note's "first version" was June's value, not a stand-in.
- **Decision (2) as revised by the user (`496f07e`)** said "the boss terms ported here need no change". On the corrected facts that does not hold: `fb92b03` read `m_cMonsterGrade` on battlefields (the offline middle boss sends 2, so never a boss), and no dungeon NPC ever got a grade. **User, 2026-10-06: keep this fix.** The offline ranking plan still owns dungeons. It must (a) send `MONSTER_GRADE` and (b) carry it through `AddNPCData` into the NPC at the dungeon spawn as `m_iExtraNPCStatMonsterGrade`, as June does (NPCData `+0x44` → `327ef71d`).
- **Kept per NPC.** June keeps `+0xdc` in the shared per-ID unit info, so the last fetch of an ID wins; the ExtraNPCStat port already declined that hazard.
- **Not plumbed here:** the dungeon grade (`m_cMonsterGrade`, which the emulator sends as 0, so 0 is June's result today).
- **Safe defaults checked.** A user without sockets has damage-to-boss result 1.0 (basic 1.0 + option 0, `X2GameUnit.cpp:14358`) and defence-to-boss 0, so a boss-grade NPC takes the same damage and deals the same damage as before.
- JUNE_BUGS #52 is now reached on battlefields: a summon or ally NPC hitting such a boss deals ×2. Row updated. funcmap rows `32286482` / `323752f0` are corrected.
- **Revert:** `git revert 60d0571` (back to `fb92b03`'s packet grade), or have `IsJuneSocketBossGrade` return false (terms off).

**Checked, no change.**
- **Attacker terms** `@322852db..32285481`, all inside `[ebp+0x264]==0` (GUT_USER = 0):
  - enemy-HP-few: `IsSamef(MaxHp, 0)`, NowHp > 0, ratio ≤ 0.3 (`3343bd78` = 0.3f as a double), r ≠ 0, through `+0x72c0`;
  - the attacker's own 52fc: `0 < r`, through `+0x539c`;
  - my-HP-few: `0 < r`, then NowHp > 0, then (1 − ratio)·r ≠ 0, through `+0x7590`, with no MaxHp test (#51).
  - Each is P += P·r, M += M·r, and the x87 stack balances. The block sits after the pet-aura switch and before the memo ×2 at `32285481`.
- **Piercing** `32285833..32285ccf` under Unicorn (DT 0/1/2 × 11 physic rates × 11 magic rates, including negative, > 0.2 and NaN): **363 cases, 0 mismatches** against `JuneSocketPiercingKeep`. The weapon cases 3-5 read no rate. March's later `fDefencePowerPhysic/Magic` use (`:3020`) reassigns them.
- **Damage to boss** `32286416..32286541`: `this` is GUT_NPC (`[edi+0x264]==1`), then SMA, then `+0x8fd4`, then the boss term (`+0x7158` result). There is no attacker-type test (#52). The `bl` Space-of-Thought route is not ported (Done note).
- **My-HP-few defence** `322868d1..3228696a`: the defender (`esi` = `[esp+0x38]`), with June's order `32275ba0` → defender 0xb8 → attacker 0x3d8 → this term → a `+0x470` block → clamp `32286995`. March has nothing between the attacker-skill rate and the clamp.
- **User DamageReact** `32048800..320488b7`: the boss and 52fc terms are inside `[esi+0x264]==1`. The cap is min(r, 1.0) (NaN passes uncapped, as in the port) and comes before the penetrate test `[ebx+0x4c5]`.
- **Critical chance** `@32048a87..32048ac8`: the rate is clamped at ≥ 0, plus `32273480`, and the critical fires when GetRandomFloat(1) ≤ that (`SRO_ATTACK_CRITICAL` = 1).
  - `32273480` = panic (time > 0) − `+0x4eb0` (= `+0x4e10` result) − socket `+0x5a38` × 0.01 (`33478b70`) when > 0.
  - `m_Panic` is confirmed independently: `+0x26b8` is Stigma (June's DamageReact `@32048ace` reads its time and then `+0x26c4` = `m_DamagePerSec`, March's own pattern), and March's pack runs Stigma, Dryad, Blind, WaterHold, Panic → `0x26b8 + 4·0x11c = 0x2b28`.
- **Critical damage tables** (index = type − 1):
  - `3204a834` = `{bbc magic, b35 mix, c1d physic, bbc, b35}`, default physic; `327b5390` has the same shape.
  - These match the port's switch: MAGIC/WEAPON_MAGIC → `de0`; MIX/WEAPON_MIX → half/half; anything else → `e00`.
  - The order is + `DD+0x59c` → + afflicted (user only) → × `+0x4ec4` result (inlined `+0x4f64` in the NPC one), and the multiplier applies when ≥ 1.
  - The NPC-attacker branch is (GetCV + afflicted) × defence.
- **`32275ba0`:** AT_SPECIAL → GAA(1)·(1 + `0x238` [+ `0x2ec` unless type 13]); AT_NORMAL → GAA(0) **+** `0x2ec`; other types → GAA(type); non-user → 1.0.
- **Item 6:**
  - Every March `SumSocketOption` call (equipment, pet aura, riding pet, title, set items `:3469/:3472`, premium `:3128`) runs inside `InitSocketData` after the clear.
  - June's tail uses `ebx` = the `32715320` templet (`[esp+0x28]`), not the rank copy. `31fe4730` (InitSocketEffectEx) jumps past it (`@3204f5f6` → `320500f0`), as March's `continue` does.
  - Slots are 1-4, then all-skill 1/2/3/4/6.
  - `3234fbd0`: the categories as `32070510`, default slot 0 → 1.0, max(1 − Σ, 0); `3235a710` applies max(·, 0.8).
- **Decision facts.** Item 4's bool is confirmed: `3203cf60` `RET 0xc`, `bl = [esp+0x18]` is the 3rd argument, and `test bl,bl` gates both the `32358970(0xb)` term (`@3203cff7`) and the `+0x74dc` cut (`@3203d029`). The callers that pass 1 (`31fd6880`) were not re-checked. The boss-grade facts are corrected above.

**Hygiene.**
- Fix, `--worktree --files X2GUNPC.cpp X2GUNPC.h --with EXTRA_NPC_STAT JUNE_BUFF_DEBUG`: 1 PASS, 1 PASS~ (one blank line after the ctor), RESULT PASS, 16 deleted lines (all `fb92b03`'s own).
- Range after `60d0571`, `--range e002c27^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_NEW_SKILL JUNE_DAMAGEDATA EXTRA_NPC_STAT`:
  - `files: 14 PASS, 1 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`
  - `in this flag's files: 24 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 0 #else/#endif without the flag token, 0 encoding change(s)`
  - `RESULT: PASS`
  - The 24 deleted lines are the Done note's 8 plus this fix's 16. The PASS~ is the one blank line.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,230,208 B, dated 2026-10-06 12:28:01, md5 631619ba9e9013d674f54d67f87f224f, equal to X2/US_SERVICE/x2.exe (2026-10-06 12:27:56); built at mods/june-buff-port 496f07e + 2 uncommitted code file(s).` Those two files are `60d0571`'s. The new `first socket boss grade` text is in the exe. Not launched.

**How to see it in game.** Offline middle bosses spawn through the NOT → `CreateNpcCreatedInfo` → `:987` route with `m_uiBossGroupId = SPAWN_ID ≥ 1`, so they are grade 1. `SHOW_BOSS_NAME` is FALSE in the data, so the `:782` route would give 0. All five Lanox middle bosses have ExtraNPCStat rows (field 1: `NUI_LOW_RAGE_FATHERFIG_FD_BOSS` and `NUI_LOW_SMITH_ALUMINUM_FD_BOSS`; 2: `NUI_LOW_GEYSER_ALUMINUM_FD_BOSS`; 3: `NUI_LOW_MOUNT_BERSERKER_BOSS`; 4: `NUI_LOW_FINALHALL_IGNIA_BOSS`). The older fields' tables are bytecode and were not crossed. So on a Lanox field, hit the middle boss. **Skipped (user, 2026-10-06):** reaching a middle boss takes about 200 kills (danger rate 4, boss at 800 in the repo's `BattleFieldServerData.lua`). The boss-grade path is therefore not verified in game; `first socket boss grade` / `first socket damage to boss` will show it the first time a middle boss is hit. The play-test is a regression run only. `ErrorLog.txt` then shows `[JUNE_BUFF] first socket boss grade: NPC <id> grade 1, ExtraNPCStat row yes -> boss yes` and, with no damage-to-boss socket, `first socket damage to boss (final damage): rate 1, X -> X` (the same number twice). The damage numbers are unchanged.

**Play-test (2026-10-06, user: regression passed; exe 631619ba, logs read).** The run started after the deploy (`ErrorLog.txt` 12:50:03) and exited cleanly (census present): 0 EXCEPTION, 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` (unrelated). Load lines as before (`:151` load summary, `:245` rank tables 11/10/10, `:249` curves). The dungeon run (`State.log:188`) shows skills (`:628`, `:654`), the first damage roll (`:629`) and a sub-stage clear (`:633-635`). No `first socket boss grade` line appeared, as expected: no middle boss was hit.

**Open for the user.** When creating the offline mob-ranking plan, include point (b) above. /cost is not available in-session.

### Phase 16-step3b3 — Done, items 0-2 (2026-10-06, Opus 5.5 high)

**Landed** under `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (first-apply lines under `_DEBUG`): `fed40d3` item 0, `23ea90a` item 1, `576b635` item 2. Decodes: 11 `funcmap.tsv` rows tagged "Buff Phase 16-step3b3". **Item 3 (the rest of item 9) is split off as 16-step3b5** (kickoff written): located and decoded, not ported.

**Item 0, the MP cost cut** (June `3203cf60`, slot 423).
- June applies the bool-gated terms after BBT 99: `mp *= max( 1 − r, 0.8 )` for the socket `m_fDecrease_MPCostRate` result r > 0 on a skill that is not type 13 (`@3203d02d`), and `rate *= 1 + 32358970( 0xb ) × 0.01` (`@3203cffb`).
- The bool, mapped: every June vtable call at `+0x6a0`/`+0x69c` passes 1 (about 40 sites). Every class override of slot 424 (`31fd1840`) calls the base with a hard-coded 1 (only June's Ain forwards its own). The one 0 is Add's `GetActualDPConsume` (`329e7820 @329e78a6`), a direct base call.
- So March's virtual `GetActualMPConsume` is June's 424 with true. A new base `GetActualMPConsumeJune( skill, level, bool )` carries the bool, and Add's DP cost calls it with false. No class signature changed. 3b2's "not applied" line is retired.
- Named and left at their neutral value, because March has none of these systems:
  - `32358970` is June's per-`CX2Unit` "ability" map (end `+0xab0`) × the `g_pData+0x5a4` table: 0 here;
  - slot 471 (`31fdbe20`) and the alternate templet `323511f0` read June's typed skill-memo list (`31fdbbb0`): 1.0 and none.

**Item 1, the summon helper's socket term** (`322ece70 @322ed2ae-322ed332`).
- Power rate = (1 + `m_fSkillDamageUpRate` [+ `m_fAllSkillDamageIncreaseRate` unless the equipped summoning skill is type 13; a null templet keeps it]) × the rate argument × …. The stat scale reads no socket field.
- `[esp+0xb0]` per case: 1.0 in 19 of the 21 calls. Evoke 1020/1025 (`@3230c6c4-3230c6fa`) and Anger of Elf 1420/1421 (`@3230c868-3230c89e`) pass skill 23408's ability 0xf when learned, which is March's own `fPowerRate` in those two cases.
- Port: `CX2GUUser::GetSummonPowerRateBySocket( skill )` is multiplied at Phase 6b's 14 power-rate sites, with the case's skill as 6b passes it, and before Mortar's −1 (flag 0x400). The five March stat-scale terms (`FIX_SUMMON_NPC_SKILL_DAMAGE_BY_USE_PROTECTION_OF_LAHELL`: Nasod Watch, Wally 9th, Fire Eve, Guardian SP1/SP2) are gone.
- Nasod Watch's term moves onto its default power rate. June flags `0x240` (`31fcf520`: skill 25000 = Dimension Link).
- **Settles Phase 6b open item 3:** June's flag-0x200 cases write the protected factor `+0x8ce4` (`31e8d610`), and the NPC's power rate is `+0x8ce0 × +0x8ce4` (`32759630`, the only reader of `+0x8ce0`). NPC init sets `+0x8ce0` to 1.0 (`@327627ac`), as March does, so scaling March's default power rate gives the same product (Ferdinand and Nasod Watch).
- June rewrote the rest of this function wholesale: summon stats come from the user's stat line, with per-case flags and follow-ups. That is class material, now a checklist §7 row (with the Metabolism one below).
- Not ported: `(3201ad10 + 1)`, which is item 3's reader (16-step3b5).

**Item 2, item HP recovery** (`322fbc60`, SAT jump table `322ff6d0`/`322ff65c` on type − 1).
- `[60]` = March's Master of Alchemist rate. `[64]` = the HP-recovery result (basic 1.0 + socket sum) − 1, or 0 without a `CX2Unit` (`@322fbd4f`).
- HP up (`@322fbdf3`), MP up (`@322fc258`) and HP % up (`@322fd47a`) take rate + `[64]`. MP % up (`@322fd965`) takes × (`[64]` + 1). Team HP up (`@322fdd83`) reads no rate.
- A user without the socket heals exactly as before. `InitAdditionalBasicStatValue` sets basic 1.0 and refreshes at once, the refresh also runs every frame, and the percent factor defaults to 1.0, so the result is exactly 1.0 and the term is 0. JUNE_BUGS #46's NPC 2.0 is not reachable here.
- **JUNE_BUGS #54:** MP items also get the HP-recovery socket.
- Chung's metabolism term stays as March has it. June changed it (class 0x14/0x6e, skill 0x6720, epsilon 5e-8 against March's `CalculateIncreasingRate` 5e-5); that goes to checklist §7.

**Item 3, split to 16-step3b5.**
- `m_eSkillIDForAdditionalPowerRate`: the "Group N" tokens are looked up in the `CX2SocketItem` group map `+0x40` (`32715680`), and each member `{ skill, rate }` goes into the templet map `+0x48`. The float lands at `+0xb8`; the `m_SkillType`/`m_PowerRate` table fills the vectors `+0x98`/`+0xa8`.
- The reader is `3201ad10` (the `+0x48` lookup), which has 17 callers. This corrects funcmap row 1183's "every source June-only".
- `m_fAllStatRelLV` is a sixth float at `+0x268`; no reader was found.
- Who fills the group map is the first open question.

**Reach** (June `OptionItemData` entries; **none in the save's 13 sockets**, 40005…40420, checked in `els_db.sql`): MP cost 25, skill damage up 29, all-skill damage 28, HP recovery 41, group power 2,491, all-stat RelLV 30.

**Hygiene.** `--worktree --files` before each commit: all PASS. Post-commit `--range fed40d3^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_NEW_SKILL JUNE_BUFF_BEHAVIOR_PARITY`: `files: 7 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 0 OTHER (other flags), 0 FAIL, 0 ERROR`; `in this flag's files: 6 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 4 #else/#endif without the flag token, 0 encoding change(s)`; `RESULT: PASS`. The 6 deleted lines are 3b2's own "not applied" block (item 4), retired here. The 4 token-less lines are two older bare `#else` lines in `X2GUUser.cpp` (`:9532`, `:21921`, not this phase's), counted once per commit that touches the file.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,231,232 B, dated 2026-10-06 13:23:20, md5 85077f68625a57f7208e29035151b125, equal to X2/US_SERVICE/x2.exe (2026-10-06 13:22:36); built at mods/june-buff-port 576b635.` The three new first-apply texts are in the exe, and 3b2's "not applied" text is gone. Not launched.

**How to see it in game.** No socket on the save reaches these readers, and no console route reaches the socket-only members. Expect a clean run with potions, MP potions, skills and (if the character has one) a summon, and **no** `first socket MP cost cut`, `first socket summon power rate` or `first socket item recovery` lines. An optional live check is in the hand-off: it puts two June sockets on one item, and only with the user's go-ahead.

**Play-test (2026-10-06, user: passed; exe 85077f68, logs read).** Setup, with the user's OK: June socket 74035 (MP cost, all-skill damage and all-skill cooldown, each 0.05) was appended to Iivue's weapon (item 579: `40110` → `40110,74035`; backup `db_backup/els_db_20261006_140204_pre_socket_74035.sql`), and the user changed Iivue's job to Code: Empress for the summon. The run started after the deploy and exited cleanly (census present): 0 EXCEPTION; 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` (unrelated, as before). `ErrorLog.txt`:
- `:289` `first socket MP cost cut: skill 5003, rate 0.05, MP 300 -> 285` (item 0);
- `:311` `first socket cool-time factor: skill 5003 slot 2 cut 0.05 -> factor 0.95` and `:312` BBT 100 `18 -> 17.1` (3b2 item 6, now live);
- `:314` `first socket summon power rate: skill 0 x 1.05 (skill damage 0, all-skill 0.05)` (item 1);
- `:409` `first socket all-skill damage (special active): rate 0.05, skill damage term 1.05` (3b2 item 3a).
**Finding, kept verbatim (JUNE_BUGS #55).** `:288` `first socket item recovery: item 230040, HP recovery -1` comes from the equip pass at unit init: `CX2GUUser::Init` calls `InitEqip` (`X2GUUser.cpp:1549`, every equipped item through `UseItemSpecialAbility`) before `InitAdditionalBasicStatValue` (`:1554`), so the HP-recovery result is still its constructor 0 and `[64]` is −1. June has the same order (`3204ea20`: vslot `0x7a0` = `32024510` `@3204ead3`, then the basics `32271540` `@3204eaf2`; the protected-float ctor `31c204b0` starts at 0), so June reads −1 there too. Potions used during play read 1.0 (the basics plus the per-frame refresh). Only an equipped item that carries an HP/MP SAT is affected (it heals 0, or MP % × 0, at init). The debug line fires for any item, so this line alone shows nothing was lost.

**Item 1b (user 2026-10-06, "follow June"), `02691f4`.** The summon cases where March sets no power rate all reach June's `322ece70` too: the switch default (flags 0, which includes the AI-party bots), Nasod Watch (0x240), Entangle Trap (0), Stone Wall (10), Fire Blossom (0) and Particle Prism (0x148). None has 0x400 or 0x800. A file-local `JuneApplySummonPowerRate` scales their default power rate by June's whole rate: socket term × 89 × 90 × 98 (`GetSummonPowerRateByBuff`) × `GetAdditionalAttackByType( AT_SPECIAL )`. This replaces item 1's socket-only Nasod Watch line and goes past Phase 6b's "buff terms only on the 14 sites" for these six cases. Case skill: March's own (Dimension Link, Stone Wall, Particle Prism), else 0. `--worktree --files X2Lib/X2Game.cpp`: 1 PASS, 1 deleted line (item 1's own Nasod Watch line), RESULT: PASS. `DEPLOY OK: X2_offline.exe is 19,232,256 B, dated 2026-10-06 14:32:54, md5 be1c9b56b9c6eb0a6bdbf092f20d074f, equal to X2/US_SERVICE/x2.exe (2026-10-06 14:32:49); built at mods/june-buff-port aab81d7 + 5 uncommitted code file(s).` Four of the five are a peer session's uncommitted offline-monster-grade work (`Always.h`, `Handlers_Room.cpp`, `X2DungeonSubStage.cpp/.h`); they are built into this exe too. **Play-test (2026-10-06, user: all passed; exe be1c9b56, logs read).** The run started after the deploy and exited cleanly: 0 EXCEPTION, 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` (as before). The three AI-party bots spawned at 14:55:32 (`offline_server.log`, ally of unit 19, stat rates 1.00, so the stat line is unchanged). In the same second `ErrorLog.txt:538` logged `first socket summon power rate: skill 0 x 1.05 (skill damage 0, all-skill 0.05)`, before the first Dimension Link Guardian cast at 14:55:51 (`:563`), so the default case (the bots) took the June rate. Items 0 and 2 and the cooldown logged as in the first play-test (`:530` MP 300 -> 285, `:541` factor 0.95).

**Open for the user.** /cost is not available in-session.

### Phase 16-step3b3 — Review (2026-10-06, Opus 5.5 high)

**Verdict.** Items 0, 1 and 2 match June. Item 1b had **one transcription error, now fixed** (`b7cdcf2`, same flag): three case skills were not June's. Read from June directly with `june2017_re/disasm_range.py`, because Ghidra has no function at `3203cf60`/`32759630`. Three new funcmap rows (`31fcf520 (full map)`, `3203cf60 + 31fd1840 (callers)`, `322ed3ae`), plus `june2017_re/summon_npc_skill_map.tsv`.

**Item 0 (bool map), confirmed.**
- **June's order** (`3203cf60`):
  - with no templet it returns its own mp argument; with no `CX2Unit` it returns 0;
  - rate = vslot 471 (`+0x75c`) × BBT 63 (`32039a50(id, 0x3f)`);
  - with the bool, rate × (1 + `32358970(0xb)` × 0.01) (`@3203cffb`; constants `334436a0` = 0.01, `334355b0` = 1.0);
  - BBT 99 (`3203a250`, `@3203d024`);
  - with the bool, the cut: r > 0 (`fcomp`/`test ah,5`/`jp`), skip when templet `+4` == 0xd (`@3203d056`), × max(1 − r, 0.8) (`33478a78` = 0.8; equality takes 1 − r);
  - returns rate × mp.
- March's `ST_HYPER_ACTIVE_SKILL` = 13. Slot 424 passes its own third argument to vtable `+0x69c` (`31fd190c`).
- **Every caller, by capstone sweep of `.text`:**
  - all 38 calls through `+0x6a0`/`+0x69c` pass 1;
  - the 17 direct calls of `31fd1840` pass 1, except Add's DP `329e78a6` (0, a direct base call) and June's Ain `32a67355` (forwards its own);
  - the register cases were traced (`ebx` = 1 at `32b79784`, `32b20540`, `32be1779`, `32be2479`);
  - nothing calls `3203cf60` directly.
- **March side:** the 9 class overrides all call the base qualified (true), with signatures unchanged. Add's DP cost is the only `false`. The virtual only forwards.

**Item 1, confirmed.**
- `@322ed2ae`: a null templet jumps to the all-skill sum (`@322ed2bd`), and type 13 drops the all-skill term.
- Each of the 14 sites passes the same skill as its Phase 6b `GetSummonPowerRateByBuff` call (15 calls of each, including the 1b helper).
- Mortar's term sits before the `-= 1`.
- The five stat-scale lines survive only in `#else`.
- `32759630` = `+0x8ce0` × protected `+0x8ce4`.
- **Evoke and Anger of Elf:** June's argument (`@3230c6c4`, `@3230c868`) is 23408 = `RNSI_P_RTR_HUNTERS_ABILITY` ability 0xf = `SA_DAMAGE_REL` alone. June applies the skill's own power rate to the stat scale (`32071720` = `GetSkillPowerRateValue`, `@322ed08c`, part of the class rewrite already in checklist §7), not to the power rate. The socket factor's product is unchanged; only the code comments ("March's own fPowerRate") were imprecise, now corrected.

**Item 1b, fixed.**
- **June's map.** `31fcf520( NPC id )` was emulated (unicorn) over ids 0–70000: 51 ids give a skill. Every one that exists in March has its own `SetUserSummonedNPCInfo` case, so the default's 0 is right. This also settles Phase 6b open item 2 for every case.
- **The three corrected cases:**
  - Entangle Trap 457 → 23213 `RNSI_A_RGA_ENTANGLE` = March `SI_A_LSR_ENTANGLE`, the skill that creates 457 (`X2GULire_ElvenRanger.cpp:15910`); it was 0;
  - Stone Wall 501 → 0 (June does not list it); it was `SI_A_AEM_STONE_WALL`;
  - Fire Blossom 1567 → 28211 `RNSI_A_SBH_SCALET_ROSE` = March `SI_A_SBH_SCALET_ROSE`; it was 0.
- Particle Prism, Nasod Watch and the default already matched.
- Only the 89 × 90 × 98 terms differ, and only with such a buff active. The socket term is the same, because none of these skills is hyper.
- **Store vs. scale.** Without flag 0x200, June *stores* the product in `+0x8ce0` (`@322ed3ae`). March's `m_fDefaultPowerRate` has no writer before these cases except the ctor's 1.0 (`X2GUNPC.cpp:2205`), so scaling is the store; the helper comment says so now.
- With 0x200, June also stores `3201ad10 + 1` to NPC `+0x8800`. That goes to the 16-step3b5 kickoff.

**Item 2, confirmed.**
- `[60]` = 1.0 / Master of Alchemist (`0x1f7`, ability 0x37). `[64]` = 0, set only with a `CX2Unit` (`@322fbcd7`), to the `+0x6ff0` result − 1.
- The only reads of `[64]`/`[60]` at the base stack depth are HP up `@322fbdf3`, MP up `@322fc258`, HP % up `@322fd47a` (all `[64] + [60]`) and MP % up `@322fd965` (`skill ? [60] : 1`, then × (`[64]` + 1)).
- `322fbd9a` and `322fe218` read other slots: one and 0x28 bytes of pushes deep (the templet argument and an `[esp+0x38]` local).
- March's adds sit after Chung's metabolism term, before the multiply. Without the socket, the result is 1.0 and the term is 0, as March.
- JUNE_BUGS #54's wording is accurate.

**Hygiene.**
- `--worktree --files X2Lib/X2Game.cpp`: 1 PASS, 8 deleted.
- Range `fed40d3^..HEAD --with JUNE_BUFF_DEBUG JUNE_BUFF_NEW_SKILL JUNE_BUFF_BEHAVIOR_PARITY`:
  - `files: 9 PASS, 0 PASS~ (blank/marker only), 0 DEFINE, 4 OTHER (other flags), 0 FAIL, 0 ERROR`
  - `in this flag's files: 15 deleted line(s), 0 non-ASCII inserted line(s), 0 EOL-mismatched inserted line(s), 4 #else/#endif without the flag token, 0 encoding change(s)`
  - `RESULT: PASS`
- The 15 deleted lines: 6 are 3b2's retired "not applied" block, 1 is 1b's own Nasod Watch line, and 8 are this review's own replaced 1b/comment lines.
- The 4 token-less lines are the two bare `#else` lines from Phase 4b-i part 4 (`b9999a2`, `X2GUUser.cpp:9532`, `:21921`, the `BEHAVIOR_PARITY` blocks), counted once per commit.
- The 4 OTHER rows are a peer's `ac0fe70` (offline grade Phase 1) inside the range.

**Deploy.** `DEPLOY OK: X2_offline.exe is 19,232,256 B, dated 2026-10-06 15:14:00, md5 f6f22eb20a640d436c0bf3c0155f4f32, equal to X2/US_SERVICE/x2.exe (2026-10-06 15:13:55); built at mods/june-buff-port 6ce3414 + 1 uncommitted code file(s).` The uncommitted file is this review's `X2Game.cpp`, committed as `b7cdcf2`. Not launched.

**Kickoffs.** `16-step3b4.md` needs nothing. `16-step3b5.md` item 4 now names the helper's two `3201ad10` uses.

**Closed.** The kickoff's open design question (the socket term on the no-power-rate summons and the default case) was settled by the user's item 1b decision ("follow June").

**Open for the user.** /cost. Regression play-test: Iivue (Code: Empress) dungeon run with the bots, Dimension Link Guardian, one HP and one MP potion; expect 3b3's first-apply lines (`skill 0 x 1.05` at bot spawn, `MP 300 -> 285`, factor 0.95, JUNE_BUGS #55's `HP recovery -1` at init) and 0 EXCEPTION. No character on the save has Entangle, Stone Wall or Scarlet Rose/Fire Blossoms, so the three changed cases are not exercised.

**Play-test (2026-10-06, user: passed; exe f6f22eb2, logs read).** The run started 15:37:36, after the deploy, and exited cleanly (census present): 0 EXCEPTION; 2 UNHANDLED `E_DISABLE_HB_CHECK_REQ` (unrelated, as before). `ErrorLog.txt` lines:
- `:490` `first socket item recovery: item 230040, HP recovery -1` (JUNE_BUGS #55, at init);
- `:491` `MP 300 -> 285`;
- `:493` `first socket summon power rate: skill 0 x 1.05`;
- `:503` factor 0.95 and `:504` BBT 100 `18 -> 17.1`;
- `:792` all-skill damage 1.05.

The bots spawned (`offline_server.log`, ally of 19) and the dungeon was cleared. This matches 3b3's runs: no regression.

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
2. Every build is `tools/build.py --deploy`, ending in a `DEPLOY OK` line (PCH invalidation, name, size and md5 are checked by the tool).
3. After the user's pack, the live kom contents are confirmed with `elstools inspect`.
4. `ErrorLog.txt` shows the `[JUNE_BUFF]` load summary with 0 placeholders and no unsupported lines.
5. The Phase 14 play-test shows each group firing in `offline_server.log`.
