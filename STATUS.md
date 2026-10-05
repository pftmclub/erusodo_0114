# STATUS - what is running, what is next

One board for every phased plan. **Updated by the session that finishes a phase or review, in
the same commit as its Done note** (the x2-phase skill's Finish list). If a row and `git log`
disagree, `git log` wins; fix the row.

**How to start anything below:** open a new conversation, pick the model and effort in the row,
type `/x2-phase <plan> <phase>` (or `... review`). `/x2-phase status` answers "what's next" and
"what can run together" from this file.

**Running two sessions at once** is fine when (a) neither depends on the other, (b) at most one
of them edits code, or their "Touches" do not share a file, and (c) both commit with explicit
paths. Builds queue through one lock (all worktrees share it). Read-only steps (RE, reviews
that find nothing, planning) can always run alongside.

States: `done` · `review` (done, review pending) · `play-test` (waiting on the user) ·
`ready` (can start) · `queued` (could start, but the user ordered another plan first) ·
`blocked` (dependency not done) · `decision` (waiting on a user decision).

---

## june-buff - June 2017 buff/debuff port

Plan `JUNE_BUFF_PORT_PHASES.md` · branch `mods/june-buff-port` · log tag `[JUNE_BUFF]` ·
flags `SERV_IRUHADEV_JUNE_BUFF_*` (+ `_DEBUG`) · kickoffs `kickoffs/june-buff/`

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| 0-5c | done, reviewed | - | - | - | - |
| 6 | done, reviewed, play-tested (review `51701bb`..`071e58d`; user kept fix 1) | - | - | - | - |
| 6b | done, play-tested 2026-10-04 (buff terms only, user decision; checks 1-2 passed, logs read) | Opus 5.5 / high | 6 | X2GUUser.cpp/.h (DoStateEndStart, GetActualMPConsume), X2Game.cpp (summon cases), X2Unit.cpp/.h, X2User.cpp, X2GU{Eve,Ara,Arme,Chung,EL,Elsword,Lire,Raven}.cpp (cooldowns) | `6b.md` |
| 6b review | done 2026-10-04: 1 fix (Wally 9th / Fire Eve / Guardian SP1 summons pass skill 0, as June's 31fcf520); deployed 5a81b0ee; play-tested 2026-10-04 (no summon line, Nasod Element appeared); user owes /cost only | Opus 5.5 / high | 6b | X2Game.cpp (3 summon lines) | `6b-review.md` |
| 7 | done, play-tested 2026-10-04 (5/5 passed, logs read, check 2 re-run confirmed the 95 refuse line); user owes /cost only | Opus 5.5 / high | 4a | X2GameUnit.cpp (GetFinalDamage, ProcessEnchantExtraDamage), X2GUUser.cpp (SetInputData), X2GUNPC.cpp (CommonEventProcess), X2BuffBehaviorTemplet, C++ BUFF_TEMPLET_ID enum | `7.md` |
| 7 review | done 2026-10-04: no transcription errors, no code change (exe stays 532eecfb); 94 restack leak kept as June (user, 2026-10-04; latent with June data); user owes /cost only | Opus 5.5 / high | 7 | - | `7-review.md` |
| 7b | done, play-tested 2026-10-05 (5/5 passed, logs read; exe bc9099a7): 74/77/97/101/102 complete, 110 store only; decision (a) = A (user, 2026-10-05) → 7c; **user owes /cost** | Opus 5.5 / high | 7 | X2GUUser.cpp/.h, X2GUNPC.cpp/.h, X2GameUnit.cpp/.h, X2GUChung.cpp/.h, X2BuffFactor.cpp, X2BuffBehaviorTemplet.cpp/.h | `7b.md` |
| 7b review | done 2026-10-05: no transcription errors, no code change (exe stays bc9099a7); whole-binary detonation-byte sweep matches the port; JUNE_BUGS #10 wording corrected; user owes /cost only | Opus 5.5 / high | 7b | - | `7b-review.md` |
| 7c | done 2026-10-05 (`e888197`), play-tested 2026-10-05 (4/4 passed, logs read; exe 3f4d41ba): 110 host = June's TryHyperModeStateChange at all 52 macro sites + Ara's six + June's SKILL_CANCEL_AFTER expansion; RE corrected: `+0x8d78` is June's quick-slot special-item wait (armed in `322fa960`), not an awakening lockout; decision (b) = A (user, 2026-10-05) → 7d; user owes /cost | Opus 5.5 / high | 7b | X2GUUser.cpp/.h, X2GUAra.cpp (the macro lives in X2GUUser.h, so no other class file changed) | `7c.md` |
| 7c review | done 2026-10-05: one transcription error fixed (June clears `+0x8d4c` in `HyperModeStart`/`RidingHyperModeStart` = `31ffd490`/`31fe96c0`, not the `End`s; moved, same flag, inert until (b)); everything else matches June; exe 46ef8895 deployed; regression play-test done 2026-10-05 (BBT 110 refuse line logged on 46ef8895); user owes /cost | Opus 5.5 / high | 7c | X2GUUser.cpp/.h, Always.h (comment) | `7c-review.md` |
| 7d | done 2026-10-05, exe 28343b61 deployed: `322fa960` = `CX2Game::CheckQuickSlotItemSpecialAbility`, KeyEventProcess routed through it as June's 32951670/32951420, the 10 s arm, the "no skill" prefix at the top of 8 classes' SpecialAttackEventProcess (slot 510 corrected: not NoStateChangeActionFrameMove); June-only quick-slot systems recorded, not ported; JUNE_BUGS #13, #14; play-tested 2026-10-05 (3/3 passed, logs read: `first arm quick-slot special item wait: item 78390, 10 s`); user owes /cost | Opus 5.5 / high | 7c review | X2UIQuickSlot.cpp, X2Game.cpp/.h, X2GUUser.h, X2GU{Eve,Ara,Arme,Chung,EL,Elsword,Lire,Raven}.cpp (SpecialAttackEventProcess), Always.h | `7d.md` |
| 7d review | done 2026-10-05: no code change (exe stays 28343b61); **decision (c) owed:** before the start the port refuses a throw item and locks skills for 10 s in March tutorials (June allows there: `322f1f90` = stage `m_bStartMotion`, 38100 = Henir exists), and SAT 38's refusal has a March home; decision (c) = **B** (user, 2026-10-05: port the `+0x92cc` timer too); SAT 38 refusal = yes (user, 2026-10-05); user owes /cost | Opus 5.5 / high | 7d | review only (fixes under the same flag) | `7d-review.md` |
| 7e | done 2026-10-05 (`f20a31c` START_DELAY + 7e's QUICKSLOT_ITEM part, `726607a` RIDING_KEY_WAIT), exe fa090292 deployed: June's start-motion delay +0x92cc (StageLoad stores it, OnFrameMove counts down -> 31fcf040; riding key and pet Ride button wait for -1), 322f5140 pre-start allow for SAT 18/34 (Henir path = DM_HENIR_PRACTICE), SAT 38 refused with STR 30375, June's 0.1 s riding-key skill wait in all 8 classes; JUNE_BUGS #13 latent again; play-tested 2026-10-05 (4/4 passed, logs read: pre-start allow line with start motion 0 / delay -1, start-motion delay end, no refused line); user owes /cost | Opus 5.5 / high | 7d review | X2Game.cpp/.h, X2DungeonGame.cpp, X2DungeonStage.cpp/.h, X2GUUser.cpp/.h, X2UIPetInfo.cpp, X2GU{Elsword,Arme,Lire,Raven,Eve,Chung,Ara,EL}.cpp (riding-key wait reader), Always.h | `7e.md` |
| 7e review | done 2026-10-05, exe be4fb8fe deployed: 7e matches June (items 1-6 checked in Ghidra); one 7d placement fix under `_QUICKSLOT_ITEM`: Ara's `+0x8d78` test moved from SpecialAttackEventProcess to CanUseSpecialAttack (June slot 510 `32bcfaa0`, which Ara's 37 FP-cancel sites also call); kickoff's START_DELAY/QUICKSLOT_ITEM flagcheck commands FAIL only for want of the sibling flag in `--with`; play-tested 2026-10-05 (2/2 passed, logs read); user owes /cost | Opus 5.5 / high | 7e | X2GUAra.cpp, X2GUUser.h + Always.h (comments) | `7e-review.md` |
| 8 | done 2026-10-05 (`cb83083`, `ec6aece`, fix `f52a21b`), exe 8b491d7d deployed: BFT 14/15/16 parse + classes (census: no BFT left; 17 has no June code), stick-shake input/prompt, BFT 16 count at state start, SHAKE_DECREASE trigger, buff-change counter + its UDP resync (field flag 32-bit, XPT 27: **every peer needs this build**, MODS.md); aura socket side -> Phase 16 (user, 2026-10-05); JUNE_BUGS #15, #16; play-tested 2026-10-05 (4/4 passed, logs read); user owes /cost | Opus 5.5 / high | 2, 4b, 6b | X2BuffFactor.cpp, X2BuffFinalizerTemplet.h/.cpp, X2BuffTemplet.h/.cpp, X2GameUnit.h/.cpp, X2GUUser.h/.cpp, X2Game.h/.cpp, X2UDPPacket.h/.cpp, Always.h | `8.md` |
| 8 review | done 2026-10-05: no transcription errors, no code change (exe stays 8b491d7d); input mapping confirmed (June `InputData` 0x25/0x26/0x23/0x24 = March LEFT/RIGHT/UP/DOWN, each +2), observer timer / XPT 27 / bit `0x800000` / finish hook checked in Ghidra; the sync is unexercised offline (needs two peers); kickoff's `..HEAD` flagcheck now FAILs only on Phase 10's `Always.h` append (scoped `..f52a21b` PASS); user owes /cost only | Opus 5.5 / high | 8 | - | `8-review.md` |
| 9 (9a) | done 2026-10-05 (`b8bc0a1`): June's 9-slot custom-function engine (AttackResultByType / NPC final damage / buff applied / erase hooks + call sites; Phase 4a's "slots 3-6" corrected), creator info + templet creator power rate, switch cases 29/56 (users only)/62/223; June's Phantom Sword, Bigbang Stream (cool time), Mark of Commander bodies + new Pandemonium Fear (its explosion is not in March data yet); JUNE_BUGS #26; exe 0dd05880 deployed (Phase 11's final exe, built at 9d70f0c + da55146; carries all of 9a); play-tested 2026-10-05 (4/4 passed, logs read: BTI 29/56/62 created, shells fell; 223 dormant: March's templet has no CUSTOM_FUNCTION until Phase 13); **user owes:** 2 decisions in the Done note, /cost | Opus 5.5 / high | 4b | X2BuffTemplet.cpp/.h, X2BuffBehaviorTemplet.cpp, X2GameUnit.cpp/.h, X2GUUser.cpp, X2GUNPC.cpp, X2GUAra.cpp, Always.h | `9.md` |
| 9a review | done 2026-10-05: no transcription errors, no code change (exe stays 0dd05880); all 8 items checked in Ghidra/rawfn (slot map + arg counts, call sites, creator info, case 56 numbers, Phantom Sword fields, Bigbang cool/FABS, MoC order + `3201ad10` = 0 justified, Pandemonium/`31fd5ef0`/`32293b80`); NPC slot 6 sees March final damage without June-only modifiers (noted in `9b.md`); nothing to play-test; user owes /cost only | Opus 5.5 / high | 9a | review only (fixes under the same flag): X2BuffTemplet.cpp/.h, X2GameUnit.cpp, X2GUNPC.cpp | `9a-review.md` |
| 9b | ready | Opus 5.5 / high | 9a review | X2BuffTemplet.cpp/.h, X2GameUnit.cpp, X2BuffFactor.h (BTI enum), X2DamageManager.h (+0x5d4 field), X2GUUser.cpp / X2GUNPC.cpp (DamageReact crit), X2GURaven.cpp; **not in parallel with 11** (X2GUUser.cpp) | `9b.md` |
| 10 | done 2026-10-05 (`9eba9ad`, docs `677a7e5`, matrix `cc1ad03`), exe 8b491d7d deployed: buff groups + officially-debuff set (UnitApplyBuffGroup.lua), `CanApplyBuffToGameUnit(BTI)` + NPC group gate, June's damage-react apply, bindings (DeleteNonControlDebuff, GetBuffAccumulationCountNow, ApplyBuffFactor cheat), June's ParsingBuffFactorID, IS_AURA_BUFF/LIMIT parsed; packed + play-tested 2026-10-05 (4/4 passed, logs read: 3 groups / 109 debuffs, group 4 and 3 refusals of BTI 37); **user owes:** /cost (`_MEMO_DROP` decided in the review: on) | Opus 5.5 / high | 3 | X2BuffTempletManager, X2GameUnit.cpp/.h, X2GUUser.cpp/.h, X2GUNPC.cpp/.h, X2DamageManager.cpp/.h, X2Main.cpp, X2Game.cpp, X2GUAra.cpp, X2GameEdit, X2BuffFinalizerTemplet.h, X2StateStartUp.cpp, UnitApplyBuffGroup.lua (+ data036 staging manifest) | `10.md` |
| 10 review | done 2026-10-05 (`e107d9b`), exe 58398ae2 deployed: no transcription errors in live code; items 1-6 checked in Ghidra and the exe bytes; `3229b0a0` (June-only skill apply at a given level, Rose's only) ported dormant as `SetBuffFactorToGameUnitAtLevel` + checklist §7 row; `AddBuffGroup_LUA` comment corrected (the open table goes on return to Lua); slot 476 (skill memo) recorded, not ported; JUNE_BUGS #21; nothing to play-test; `_MEMO_DROP` turned on by the user's decision (`e6757fb`, exe 0b06cc94); **user owes:** /cost | Opus 5.5 / high | 10 | X2GameUnit.cpp/.h, X2BuffTempletManager.cpp, Always.h | `10-review.md` |
| 10b | done 2026-10-05 (`a9ff45a`), exe 3e033022 deployed: IS_AURA_BUFF party gate at all 7 hit paths (`_AURA_PARTY`; target = my own unit only, slot 0x198 = IsMyUnit), aura hit skips the hitted-by-buff effect sets (`_AURA_REACT`), June's guild Chance to Reverse check `32610df0` at its 8 sites, now also on a hit (`_GUILD_REVERSE`); LIMIT_APPLY_BUFF_COUNT_EACH_UNIT is dead in June (counter/check uncalled), not ported; gate dormant (no March data sets the key); JUNE_BUGS #22-#24; play-tested 2026-10-05 (regression passed, logs read: one `checked on a hit too` line, attacker 25; no aura lines); **user owes:** /cost | Opus 5.5 / high | 10 | X2DamageManager.cpp, X2GUUser.cpp, X2GUNPC.cpp, Always.h | `10b.md` |
| 10b review | done 2026-10-05: matches June, no transcription errors (all 6 checks + own `+0x5f0`/`+0x7f0` scan); hit log now null-guards both UIDs (`_GUILD_REVERSE`); JUNE_BUGS #25 (EffectToUnit re-reads a possibly removed effect's owner; port keeps `pAttackUnit`); exe 826c409e deployed; play-tested 2026-10-05 (regression passed: hit log attacker 25, target 4); **user owes:** /cost | Opus 5.5 / high | 10b | review only (fixes under the same flags) | `10b-review.md` |
| 11 | done 2026-10-05 (`af7741b`, `da55146`, `1e29455`, docs `179cbe0`), exe 3f3f756b deployed; play-test 2026-10-05 PASSED (checks 1-3 on `0dd05880`; 4-5 on `551a1dab` after the village-icon fix `1e29455`; logs read); village Rest buff 122 added (`af37a76`, user request), play-tested 2026-10-05 (passed, logs read); **user owes:** /cost: June's world-buff handlers (record + stack count, skips, PC-room refresh), `_INT` premium file, ReInit ×count, June's skill level (`_PREMIUM`); emulator owns item buffs (user decision B: Arcanum/Elixir groups, 117 lockout, v14 `unit_buff`, end flags, `EGS_UPDATE_BUFF_INFO(_IN_ROOM)_NOT`; `_OFFLINE_ITEM_BUFF`); NOT_USE_IN stand-in (Henir live, PVE/HUGE dormant); split → 11b; JUNE_BUGS #27-#29; **user owes:** pack `BuffTemplet.lua` + `BuffTemplet_INT.lua` (march2014/update/data036) into data036.kom, play-test, /cost | Opus 5.5 / high | 3 | X2PremiumBuffManager.cpp, X2StateLoading.cpp, X2Unit.cpp/.h, X2GUUser.cpp (ReInit), X2BuffFactor.h, X2Lib/Offline/ (Handlers_Inventory/Room/Field/Unit, X2OfflineDB, X2OfflineServer.h, X2OfflineBuff.h), Always.h, BuffTemplet(_INT).lua (+ data036 manifests) | `11.md` |
| 11 review | ready after 11's play-test | Opus 5.5 / high | 11 | review only (fixes under the same flags) | `11-review.md` |
| 11b | ready after 11's review | Opus 5.5 / high | 11 | X2GUUser.cpp (slots 250/251), X2GameUnit.cpp (IS_STORE_BUFF clear-all), X2GageManager (32499c10), X2Unit.cpp (repair discount), new UI if any, Always.h | `11b.md` |
| 12 | done, play-tested | - | - | - | - |
| 13 | blocked (last data phase) | Sonnet 5.5 / medium | 4-12 | buff Lua data, staging dirs | `13.md` |
| 14 | blocked | Opus 5.5 / high | 13 | - (play-test sweep) | `14.md` |
| 15 | step 1 done (RE); **step 2 ready, opens with 5 user decisions** | Opus 5.5 / high | - | X2EnchantItem.cpp, X2GameUnit.cpp/.h, X2GUUser.cpp, X2Item.cpp, X2Data.cpp, X2Define.h, XSLItem.h (shared header), Offline/ (Handlers_Inventory, X2OfflineAttribTable), AttribEnchant*.lua | `15-step2.md` |
| 16 | step 1 done (RE); steps 2-4 ready; **also owns BFT_AURA's socket side from Phase 8** (user, 2026-10-05): SIEE 12-20 incl. 15 `SIEE_ALWAYS_PLAY_EFFECT_SET` (emitter `ProcessSocketEffectEx` case 15 `32029fff`), the socket-data `EFFECT_NAME` / interval / `BUFF_FACTOR_ID` list (`+0x29c`), and `CX2GUUser::HasSocketAuraFor`'s inner loop (slot 181, `3204e380`) | Opus 5.5 / high | 5c | X2SocketItem.cpp/.h, X2GameUnit.cpp, X2GUUser.cpp, X2Unit.cpp, X2BuffTemplet.cpp, X2UserSkillTree.cpp, SocketOptionForm.lua | `16-step2.md` |

"Touches" lists the files each phase's section names (checked 2026-10-04 with
`tools/docsection.py`); a phase may touch more once it is underway.
Parallel pairs with no shared file: none open now. 9a and 11 ran together on 2026-10-05, but both edited `X2GUUser.cpp` and `Always.h`, so each staged only its own hunks; 9b and 11 must not run together. (10 turned out to edit X2GameUnit/X2GUUser/X2GUNPC, so 9 + 10b was not a pair either.) Phases 10 and 11 both stage Lua
into `data036` and its `_manifest.json`, so run them one after the other. 15 and 11 both edit
`X2Lib/Offline/`. Phases 6b, 7, 7b, 7d, 8, 9, 15 and 16 all edit `X2GUUser.cpp` and/or `X2GameUnit.cpp`,
so run them one at a time; of these only 9 is known to be clear of 10's and 11's files.
Every code phase appends to `KTDXLIB/Always.h` (bytepatch re-reads it, so two appends in a row
are safe; two simultaneous ones are not).

Plan doc migrated 2026-10-04: finished Done/Review notes of 4b-i..6 are in
`JUNE_BUFF_PORT_ARCHIVE.md` (headings kept as stubs that `tools/docsection.py` follows), and
the inline prompts of unfinished phases point at `kickoffs/june-buff/`.

## june-effect - June 2017 effect engine completion

Plan `JUNE2017_EFFECT_COMPLETION_PHASES.md` (C0-C12; reference `JUNE2017_EFFECT_REBUILD_PLAN.md`) ·
current branch · log tag `[JUNE_FX]` (ErrorLog.txt) · flags `SERV_IRUHADEV_JUNE_*` (one per phase,
named in each kickoff) · kickoffs `kickoffs/june-effect/` ·
**user's order (D1, 2026-10-04): finish the buff port first, then this plan.**

The plan runs "Opus identifies, Sonnet transcribes, Opus reviews". The four phases that did both
in one session (C1, C6, C8, C10) are split here into **a** (Opus, no code) and **b** (Sonnet,
code), so each half gets its own conversation and model. "Sonnet 5" in the plan doc means
today's Sonnet 5.5.

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| C0 | done 2026-10-05: D2 build, D3 hand off (Rena), D4 build, D5 decide after C2, D6 keep; census now has `handed-off`/`declined`, owners, item-1/3 checks (`census_completion.py`); item-1 shows 1,913 numerical + 7 HP-rate blocks | Sonnet 5.5 / high | - | `june2017_re/census.py`, `census_completion.py`, C0's decision table | `C0.md` |
| C1a | queued | Opus 5.5 / high | C0 | `funcmap.tsv` (no code) | `C1a.md` |
| C1b | blocked | Sonnet 5.5 / high | C1a | X2DamageManager.cpp (SetDamageDataFromLUA), a new table loader, NewSkillTempletVer2.lua (user packs), MODS.md | `C1b.md` |
| C1b review | blocked | Opus 5.5 / high | C1b | - | `C1b-review.md` |
| C2 | queued | Opus 5.5 / high | - | `funcmap.tsv`, `june2017_re/README.md` (no code) | `C2.md` |
| C3 | blocked | Sonnet 5.5 / high | C2 | X2DamageManager.cpp, consumers in X2DamageEffect.cpp / X2GameUnit.cpp | `C3.md` |
| C3 review | blocked | Opus 5.5 / high | C3 | - | `C3-review.md` |
| C4 | blocked | Sonnet 5.5 / high | C2 | X2DamageManager.cpp, X2GameUnit.cpp (GetFinalDamage), X2GUAdd.cpp | `C4.md` |
| C4 review | blocked | Opus 5.5 / high | C4 | - | `C4-review.md` |
| C5 | blocked | Sonnet 5.5 / high | C2 (+ coordinate with june-buff) | X2DamageManager.cpp, X2DamageEffect.cpp | `C5.md` |
| C5 review | blocked | Opus 5.5 / high | C5 | - | `C5-review.md` |
| C6a | queued (identification started 2026-09-29) | Opus 5.5 / high | - | `funcmap.tsv` (no code) | `C6a.md` |
| C6b | blocked | Sonnet 5.5 / high | C6a | X2DamageEffect.cpp (CEffect, hit/die spawners) | `C6b.md` |
| C6b review | blocked | Opus 5.5 / high | C6b | - | `C6b-review.md` |
| C7 | blocked | Sonnet 5.5 / high | C6b | X2DamageEffect.cpp, KTDXLIB/KTDGXMeshPlayer.h (CXMeshInstance) | `C7.md` |
| C7 review | blocked | Opus 5.5 / high | C7 | - | `C7-review.md` |
| C8a | blocked | Opus 5.5 / high | C2 | `funcmap.tsv`, a port list in the plan (no code) | `C8a.md` |
| C8b | blocked | Sonnet 5.5 / high | C8a, C1b | X2DamageEffect.cpp (hit paths), X2GameUnit.cpp, X2GUUser.cpp, X2GUNPC.cpp, per-class X2GU*.cpp (SetDamageExceptionProcess) | `C8b.md` |
| C8b review | blocked | Opus 5.5 / high | C8b | - | `C8b-review.md` |
| C9 | blocked | Sonnet 5.5 / high | C2 | X2Main.cpp (bindings), X2DamageManager.cpp, data036 staging (user packs) | `C9.md` |
| C9 review | blocked | Opus 5.5 / high | C9 | - | `C9-review.md` |
| C10a | blocked | Opus 5.5 / high | C0 (D2-D5), C2 | `funcmap.tsv` (no code) | `C10a.md` |
| C10b | blocked | Sonnet 5.5 / high | C10a | per decided system (game option UI/save, damage path) | `C10b.md` |
| C10b review | blocked | Opus 5.5 / high | C10b | - | `C10b-review.md` |
| C11 | blocked | Sonnet 5.5 / high | C1-C10 | - (verification; one .dds staged) | `C11.md` |
| C12 | blocked | Sonnet 5.5 / high | C11 | docs, MODS.md, maybe Always.h (D6) | `C12.md` |

Running together: C0, C1a, C2, C6a and C8a/C10a touch no game code, so they can run alongside a
code session. C1a, C2, C6a, C8a and C10a all add rows to `funcmap.tsv`, though, so run those one
after another. C3, C4, C5 and C9 all edit X2DamageManager.cpp; C6b, C7 and C8b all edit
X2DamageEffect.cpp: one at a time within each group.

## Other plans

No `kickoffs/` files yet: `/x2-phase <plan> <phase>` writes the kickoff from the plan doc's
phase section (kickoffs/TEMPLATE.md) on first use and shows it before starting.

| Plan | Doc | State (2026-10-04, from the doc's headings; verify before starting) |
|---|---|---|
| backlog | `JUNE_PORT_BACKLOG_ORDER.md` | The order of everything below: stage 0 (close open loops, incl. this buff port) -> 1 Atlas/Elysion -> 2 engine remainder -> 3 skill core -> ... -> 9 UI. |
| atlas | `ATLAS_PORT_PHASES.md` | Done notes exist through A13. |
| elysion | `ELYSION_PORT_PHASES.md` | Done notes exist through E15. |
| lanox | `LANOX_REMAINING_PHASES.md` + `JUNE_UI_PARITY_PHASES.md` | 12 items in phases 0-11; check the doc for which have landed. |
| char-port | `JUNE2017_CHARACTER_PORT_CHECKLIST.md` | Checklist, not phased; buff phases append to section 7. |
