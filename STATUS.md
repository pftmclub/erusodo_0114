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
`ready` (can start) · `blocked` (dependency not done) · `decision` (waiting on a user decision).

---

## june-buff - June 2017 buff/debuff port

Plan `JUNE_BUFF_PORT_PHASES.md` · branch `mods/june-buff-port` · log tag `[JUNE_BUFF]` ·
flags `SERV_IRUHADEV_JUNE_BUFF_*` (+ `_DEBUG`) · kickoffs `kickoffs/june-buff/`

| Phase | State | Model / effort | Depends | Touches (main files) | Kickoff |
|---|---|---|---|---|---|
| 0-5c | done, reviewed | - | - | - | - |
| 6 | done, reviewed, play-tested (review `51701bb`..`071e58d`; user kept fix 1) | - | - | - | - |
| 6b | **ready** | Opus 5.5 / high | 6 | X2GUUser.cpp (DoStateEndStart, GetActualMPConsume), X2Game.cpp (summon helper), per-class X2GU*.cpp (cooldowns) | `6b.md` |
| 6b review | blocked | Opus 5.5 / high | 6b | - | `6b-review.md` |
| 7 | ready | Opus 5.5 / high | 4a | X2GameUnit.cpp (GetFinalDamage, ProcessEnchantExtraDamage), X2GUUser.cpp (SetInputData), X2GUNPC.cpp (CommonEventProcess), X2BuffBehaviorTemplet, C++ BUFF_TEMPLET_ID enum | `7.md` |
| 7 review | blocked | Opus 5.5 / high | 7 | - | `7-review.md` |
| 7b | blocked | Opus 5.5 / high | 7 | X2GUUser.cpp/.h, X2GUNPC.cpp/.h, X2GameUnit.h, X2GUChung.cpp, X2GUAra.cpp | `7b.md` |
| 7b review | blocked | Opus 5.5 / high | 7b | - | `7b-review.md` |
| 8 | blocked (needs 6b's 98 -> BFT 16 ordering) | Opus 5.5 / high | 2, 4b, 6b | X2BuffFinalizerTemplet, X2UDPPacket.h (every peer needs the same build), X2GUUser.cpp, X2GUNPC.cpp | `8.md` |
| 9 | ready | Opus 5.5 / high | 4b | X2GameUnit.cpp (CreateAndInsertCustomFunction), X2GUUser.cpp, X2GURaven.cpp, X2GUAra.cpp, new classes | `9.md` |
| 10 | ready | Opus 5.5 / high | 3 | X2Main.cpp (bindings), X2Game.cpp, UnitApplyBuffGroup.lua (+ data036 staging manifest) | `10.md` |
| 11 | ready | Opus 5.5 / high | 3 | X2PremiumBuffManager.cpp, X2Lib/Offline/ (handlers as needed), BuffTemplet.lua (+ data036 staging manifest) | `11.md` |
| 12 | done, play-tested | - | - | - | - |
| 13 | blocked (last data phase) | Sonnet 5.5 / medium | 4-12 | buff Lua data, staging dirs | `13.md` |
| 14 | blocked | Opus 5.5 / high | 13 | - (play-test sweep) | `14.md` |
| 15 | step 1 done (RE); **step 2 ready, opens with 5 user decisions** | Opus 5.5 / high | - | X2EnchantItem.cpp, X2GameUnit.cpp/.h, X2GUUser.cpp, X2Item.cpp, X2Data.cpp, X2Define.h, XSLItem.h (shared header), Offline/ (Handlers_Inventory, X2OfflineAttribTable), AttribEnchant*.lua | `15-step2.md` |
| 16 | step 1 done (RE); steps 2-4 ready | Opus 5.5 / high | 5c | X2SocketItem.cpp/.h, X2GameUnit.cpp, X2GUUser.cpp, X2Unit.cpp, X2BuffTemplet.cpp, X2UserSkillTree.cpp, SocketOptionForm.lua | `16-step2.md` |

"Touches" lists the files each phase's section names (checked 2026-10-04 with
`tools/docsection.py`); a phase may touch more once it is underway.
Parallel pairs with no shared file: **9 + 10**, **9 + 11**. Phases 10 and 11 both stage Lua
into `data036` and its `_manifest.json`, so run them one after the other. 15 and 11 both edit
`X2Lib/Offline/`. Phases 6b, 7, 7b, 9, 15 and 16 all edit `X2GUUser.cpp` and/or `X2GameUnit.cpp`,
so run them one at a time; of these only 9 is known to be clear of 10's and 11's files.
Every code phase appends to `KTDXLIB/Always.h` (bytepatch re-reads it, so two appends in a row
are safe; two simultaneous ones are not).

Not yet migrated (2026-10-04): the plan doc itself still carries the inline kickoff prompts and
the long Done notes of 4b-i..5c. Moving those notes into `JUNE_BUFF_PORT_ARCHIVE.md` and
pointing the inline prompts at `kickoffs/` waits until no session is writing the doc (user's
call). Until then the `kickoffs/` files are the ones to use.

## Other plans

No `kickoffs/` files yet: `/x2-phase <plan> <phase>` writes the kickoff from the plan doc's
phase section (kickoffs/TEMPLATE.md) on first use and shows it before starting.

| Plan | Doc | State (2026-10-04, from the doc's headings; verify before starting) |
|---|---|---|
| backlog | `JUNE_PORT_BACKLOG_ORDER.md` | The order of everything below: stage 0 (close open loops, incl. this buff port) -> 1 Atlas/Elysion -> 2 engine remainder -> 3 skill core -> ... -> 9 UI. |
| june-effect | `JUNE2017_EFFECT_COMPLETION_PHASES.md` | C0-C12 not started; queued after the buff port (C1, numerical skill data, is the blocker). |
| atlas | `ATLAS_PORT_PHASES.md` | Done notes exist through A13. |
| elysion | `ELYSION_PORT_PHASES.md` | Done notes exist through E15. |
| lanox | `LANOX_REMAINING_PHASES.md` + `JUNE_UI_PARITY_PHASES.md` | 12 items in phases 0-11; check the doc for which have landed. |
| char-port | `JUNE2017_CHARACTER_PORT_CHECKLIST.md` | Checklist, not phased; buff phases append to section 7. |
