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
| 6 | **play-test** of the review's fixes (review `51701bb`, exe md5 `f413b8bf`, 18:41) | - | - | - | hand-off at the end of "Phase 6 -- Review" |
| 6b | ready (after 6's play-test) | Opus 5.5 / high | 6 | X2GUUser.cpp (DoStateEndStart, GetActualMPConsume), X2Game.cpp (summon helper), per-class X2GU*.cpp (cooldowns) | `6b.md` |
| 6b review | blocked | Opus 5.5 / high | 6b | - | `6b-review.md` |
| 7 | ready | Opus 5.5 / high | 4a | X2GameUnit.cpp (GetFinalDamage, EDT), X2GUUser.cpp (SetInputData), X2GUNPC.cpp, X2BuffBehaviorTemplet, X2BuffTemplet.h | `7.md` |
| 7 review | blocked | Opus 5.5 / high | 7 | - | `7-review.md` |
| 7b | blocked | Opus 5.5 / high | 7 | X2GUUser.cpp, per-class X2GU*.cpp, X2GameUnit.cpp | `7b.md` |
| 7b review | blocked | Opus 5.5 / high | 7b | - | `7b-review.md` |
| 8 | blocked (needs 6b's 98 -> BFT 16 ordering) | Opus 5.5 / high | 2, 4b, 6b | X2BuffFinalizerTemplet, X2UDPPacket, X2GUUser.cpp | `8.md` |
| 9 | ready | Opus 5.5 / high | 4b | X2GameUnit.cpp (custom functions), new classes | `9.md` |
| 10 | ready | Opus 5.5 / high | 3 | X2Main.cpp (bindings), buff-group code | `10.md` |
| 11 | ready | Opus 5.5 / high | 3 | X2PremiumBuffManager.cpp, X2Lib/Offline/ | `11.md` |
| 12 | done, play-tested | - | - | - | - |
| 13 | blocked (last data phase) | Sonnet 5.5 / medium | 4-12 | buff Lua data, staging dirs | `13.md` |
| 14 | blocked | Opus 5.5 / high | 13 | - (play-test sweep) | `14.md` |
| 15 | step 1 done (RE); **step 2 ready, opens with 5 user decisions** | Opus 5.5 / high | - | X2EnchantItem.cpp, X2GameUnit.cpp, X2Data.cpp, XSLItem.h (shared header) | `15-step2.md` |
| 16 | step 1 done (RE); steps 2-4 ready | Opus 5.5 / high | 5c | socket data + aggregate, X2GameUnit.cpp, X2GUUser.cpp, SocketOptionForm.lua | `16-step2.md` |

Parallel pairs that do not share a main file: **9 + 11**, **10 + 11**, **15-step2 + 11**.
Phases 6b, 7, 7b and 16 all edit `X2GUUser.cpp`/`X2GameUnit.cpp`: run them one at a time.
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
