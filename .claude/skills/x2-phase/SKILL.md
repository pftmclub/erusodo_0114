---
name: x2-phase
description: Start, review, finish or plan a phase of a ProjectX2 port plan (June buff port, effect completion, Atlas/Elysion, Lanox...) from STATUS.md and kickoffs/. Use when the user types /x2-phase, says "start phase 6b", "review phase 6", "what's next", "what can I run in parallel", "what's the kickoff", or asks to write a new phased plan.
---

# Phase workflow

The user's loop is: open a new conversation, set the model/effort STATUS.md names, type
`/x2-phase <plan> <phase>` (or `... review`), play-test what you hand over, say "check logs".
Everything else (reading the right sections, building, deploying, hygiene, bookkeeping) is yours.

Arguments: `status` | `next` | `<plan> <phase>` | `<plan> <phase> review` | `new-plan <name>`.
`<plan>` is the key in STATUS.md (e.g. `june-buff`); `<phase>` as written there (e.g. `6b`, `15-step2`).

**`kickoffs/` is the source of truth for kickoff prompts.** Plan docs written before
2026-10-04 still carry inline copies; those are frozen. Never amend or "supersede" an inline
kickoff in a plan doc: change the file in `kickoffs/` instead, and say why in the Done note.

## status / next (read-only)

Read `STATUS.md` only (it holds open plans and open rows; never read `STATUS_ARCHIVE.md` whole, grep it by
phase id when a Depends or old commit needs checking). Answer in plain words, short:
- what is running or waiting on the user (play-test, a decision, a pack);
- what can start now, with model/effort and the exact command to type;
- which of those can run **at the same time** (STATUS "Touches" columns do not overlap and
  neither depends on the other; builds queue through the build lock automatically).
Verify against `git log --oneline -15` before claiming a phase is done; STATUS can lag a peer's commit.

## Start a phase or review

1. **Locate.** Read the plan's block in `STATUS.md` and the kickoff file it names
   (`kickoffs/<plan>/<phase>.md`, or `<phase>-review.md`). If the row's dependencies are not
   done, stop and say which.
   **No kickoff file yet** (plans not migrated): pull the phase's section and any inline
   kickoff prompt with `tools/docsection.py`, write `kickoffs/<plan>/<phase>.md` from
   `kickoffs/TEMPLATE.md` (section-scoped read list; task text verbatim; no build/deploy
   boilerplate), add the plan's rows to STATUS.md if missing, show the user the kickoff in
   two lines (what it will do, which model), and proceed unless the model is wrong.
2. **Model check.** If the kickoff names a different model than the one you are running on,
   tell the user in one line and ask whether to continue. Effort cannot be checked; it is
   named in STATUS for the user.
3. **Peers.** `git status --short` and `git log -3 --oneline`. Other sessions share this tree
   and its one branch, `main` (user, 2026-10-16): never create, switch to or merge a branch or a
   worktree, and a new plan's kickoffs say "branch `main`". Never touch, stage, revert or "clean up" files you did not change.
4. **Read only what the kickoff lists** (on an Opus phase, have a subagent do the reading: see *When the phase is Opus*), with
   `python tools/docsection.py <plan doc> "<section>" "<section>" ...`.
   Never read a plan doc "in full"; if you need something unlisted, `--list` the outline and
   pull that section. Where a doc's "Starting context" describes building, deploying,
   unifdef or log checks by hand, the tools below replace it.
5. **Do the task** in the kickoff. Standing rules (CLAUDE.md, memory) still apply: one
   `SERV_IRUHADEV_` flag per logical change in `KTDXLIB/Always.h`, original code in `#else`,
   cite June addresses, never invent values, design decisions go back to the user.
   **June bugs and quirks are kept verbatim without asking** (user rule, 2026-10-04): a leak, an
   off-by-one, a value June sends as 0, a wrong-looking order. Port it as June has it, and add a
   row to `JUNE_BUGS.md` (its rules say what goes in a row) in the same commit, not a question. Ask only when
   copying June would break March (a crash, an unloadable script, a missing enum or system).
   **No hardcoded enum literals** (user rule, 2026-10-26; CLAUDE.md): an id that has a name in an Enum
   (June's `Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the
   member, and register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value
   with no Enum entry stays a number with a one-line comment. The one exception is a table whose rows are the
   data itself (a `.tsv` of ids), not code. `python tools/enumscan.py --check <files>` before committing.
   **No `June` prefix** (user rule, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
   March's in `#else`; never a `June_X` / `JuneX` copy. A function March lacks is created under a name that follows the
   class's own pattern (look at its neighbours), with no `June` in any function, member, class, file, namespace or Lua name.
   Every name, locals and constants included, says what the thing is in context; never a Ghidra address or a phase tag
   (`June32481fa0`, `JUNE_CH1I_...`, `bJuneMemo`): the address goes in the one-line comment.
   Call a March function from June code only if June's namesake does the same; if June's is a stub or differs, replace
   March's definition instead (`tools/data/june_march_divergent.tsv`).
   **Before transcribing June code, run `python tools/junediverge.py --list`** (user, 2026-10-30; plan `june-march-divergence` D2): the
   names to avoid and what to write instead, the idiom rows (a PvP-only game-type test where June calls `322cc1a0` is
   `GetIsNowPVPCorrectionGame()`), and the `replace` rows not yet replaced.

## Tools (use these, not hand-rolled equivalents)

| Need | Command | Skill |
|---|---|---|
| Edit a CP949 file | `python tools/bytepatch.py show/find/apply` | x2-byte-edit |
| Build + deploy | `python tools/build.py --deploy` (background) | x2-build-deploy |
| Flag hygiene / unifdef | `python tools/flagcheck.py <FLAG> --worktree` then `--range` | x2-byte-edit |
| Play-test logs | `python tools/logs.py --tag <TAG>` | x2-check-logs |
| One doc section | `python tools/docsection.py <doc> "<heading>"` | - |
| Convert / edit / prove a March `.lua` as BOM source | `python tools/luasrc.py find/convert/restore/refresh/ledger` (`kom.py compile` folds `X.src.lua`) | x2-lua |
| Bring a June `.lua` into March (default June's `ljconv` bytecode; `--source` + core-list row only when it must differ or be edited) | `python tools/luasrc.py adopt-june [<kom>] X.lua` / `core --check / --add` | x2-adopt-june |

## When the phase is Opus: delegate the mundane work to subagents (user rule, 2026-10-11)

If the kickoff / STATUS row names **Opus** (identify, review, RE first, any "Opus identifies" half), you are the
expensive model. Keep your context for the judgment calls and hand everything mechanical to the `Agent` tool
(`subagent_type: general-purpose`, `model: sonnet`, or `haiku` for pure lookups; `run_in_background: false` when your next
step needs the answer, otherwise let it run). The user has authorised this for Opus phases; on a Sonnet phase do the work yourself.

**Opus keeps (never delegate):**
- Reading June's code in Ghidra and deciding what it does; line-by-line June-vs-March diffs; the review verdict.
- Writing or approving each transcription of June logic, flag design, and anything that is a decision for the user.
- Judging whether a subagent's finding is enough to act on.

**Delegate (each is a fixed-shape job with a short written report):**
- The kickoff's read list: `docsection.py` sections, `funcmap.tsv` greps, census / tsv lookups. Ask for a digest with
  `file:line` / row numbers and the verbatim lines you will quote, not a retelling.
- Locating March call sites, callers, readers, enum values, flag state (`grep` over `X2Lib`, `KTDXLIB`; `cl /P` probes).
- Mechanical Ghidra pulls you already know the address for (dump function N, list xrefs, list a vtable): raw output to a file
  in the scratchpad, path returned. You read the part that matters.
- Build + deploy (`build.py --deploy`), `flagcheck.py` runs (worktree and range), `logs.py --tag` reads: report the
  `DEPLOY OK` / `RESULT` / summary lines and anything not PASS, nothing else.
- Bookkeeping drafts: Done-note draft (under ~60 lines), STATUS row text, next-kickoff file from the template, hand-off
  checklist. You review and correct the draft before it is committed.
- Large mechanical edits you have fully specified (a `bytepatch.py` spec, a table of repeated one-line insertions): hand the
  spec, get back the `flagcheck` verdict.

**Rules for every subagent prompt:**
- Self-contained: the phase, the exact files / addresses / sections, the output shape, and "report under N lines".
- Say what it must **not** do: no commits, no `git add`, no editing files it was not given, no launching the game, no
  killing processes by image name, no touching peers' uncommitted files, CP949 sources only through `bytepatch.py`,
  no new `.lua` files, no invented values (report "not found" instead).
- Independent jobs go out in **one message in parallel** (reads, greps, a build alongside a log read).
  Edits to the same file, and anything behind the build lock, go one at a time.
- Trust but spot-check: before a subagent's claim decides a port (an address, an offset, "no other caller"), open the cited
  line yourself. A report with no citation is a lead, not a fact.
- Do not delegate what takes you one tool call, and do not spawn an agent to "think about" the task.
- Commit and the final hand-off to the user are always yours.

## Finish (same session)

1. `flagcheck.py <FLAG> --worktree --files <the code files you changed>` before committing
   (`--files` keeps peers' uncommitted edits out of your verdict): RESULT must be PASS, every
   FAIL fixed, every OTHER row and deleted line explained. After committing, run it once more
   with `--range <your first commit>^..HEAD` and paste those summary lines.
   **Lua binder gate (user, 2026-10-27):** for every June `.lua` the phase stages or depends on (an NPC / summon / dungeon script,
   a class script), run `python tools/luabind.py check <NAME ...>`: every method it calls must be bound in March. An unbound
   name is bound whole from June's function (address in the funcmap) or added to `tools/data/lua_unbound_known.tsv` with the
   reason; otherwise the script dies at that line in game (Oberon's slash, 2026-10-27). The hand-off then lists the log lines
   the user's test must produce (`ES_CREATE <effect set>`, the summon's state lines), and "check logs" confirms each by name.
   **Skill icon gate (user, 2026-10-28):** a character-port phase that adds or registers skills (CH0b, CH2-CH3, transcendence
   rows, CH6) runs `python tools/skillicons.py check <the class's skill-id range>`. It must PASS, or the phase stages the files
   it lists (`kom.py stage-june`, then pack). Shared sheets such as `DLG_SKILL_Passive_12` / `_13` (every class's transcendence
   passives) are never caught by name-based staging; Aisha's and Eve's transcendence icons both shipped missing that way.
   **June / March divergence gate (user, 2026-10-30):** every phase that touches client source runs `python tools/junediverge.py
   --check <the changed files>`; it must print `RESULT: PASS` (paste the line into the Done note, beside `flagcheck`). It fails on a
   June-named, address-named or phase-tag identifier, a PvP-only game-type test inside June code, and a call to a `replace`-row
   function that is not replaced. A hit is fixed, or allow-listed in `tools/data/june_diverge_allow.tsv` with the June address that
   justifies it (never to silence it). A review runs `--check-all`.
2. `build.py --deploy`: paste its `DEPLOY OK` line into the Done note. Check the phase's
   strings are in the deployed exe if the phase's gate asks for it.
   Exception (user, 2026-11-09): only a phase or review that hands off a play-test runs the full build. Any other
   phase proves compilation with a one-file probe build (0 errors) and writes "full build skipped: no play-test"
   in place of the `DEPLOY OK` line.
   Exception (user, 2026-11-03): if the only code change is commenting out a `_DEBUG` `#define`
   (a review retiring its debug lines), start the build in the background and do not wait for it;
   write "not awaited" in place of the `DEPLOY OK` line, then commit and hand off.
3. **Done note** in the plan doc, under the phase: what landed (commit, flag), hygiene lines
   from flagcheck, the deploy line, per-item "how to see it in game", open questions for the
   user. **Keep it under ~60 lines**; long RE tables go to a
   `.tsv` (e.g. `june2017_re/funcmap.tsv`) or the plan's archive file, and the note links them.
   **Branches you found but did not port**: apply the scope rule (`JUNE_PORT_BACKLOG_ORDER.md`, "Standing rules",
   *Scope*; user 2026-10-19). Add a new phase to *this* plan only if the branch is the plan's subject or playability
   needs it. Anything that drifts is identified in the note, together with its related branches, and gets its own plan
   in this session: a bullet and brief in the stage that owns it, or a new backlog stage with
   `kickoffs/june-backlog/stageN-plan.md`. Never chase the call graph phase after phase, and never drop a branch.
4. **STATUS.md**: update this phase's row (state, commit, what the user owes) in the same commit. A row is one
   line of about 200 characters: evidence (md5s, counts, log lines, flagcheck tallies) goes in the Done note,
   never in the row. A phase that is done and owes nothing moves its id to the block's `Done:` line. When a
   plan closes (Z done), append its block to `STATUS_ARCHIVE.md` and replace it with one *Closed plans* row.
5. **Review kickoff**: if the phase needs a review and `kickoffs/<plan>/<phase>-review.md` does
   not exist, write it from `kickoffs/TEMPLATE-review.md`.
   **Review cadence (user, 2026-11-10):** a review is per series, not per phase. In a plan's new kickoffs (char ports
   from `char-ara` on), a run of same-file skill / passive phases that each hand off a play-test (CH2c-d, CH3a-b, CH4a-b)
   gets one review at the series' last phase, which reads every covered Done note, runs `flagcheck --range` over all of
   them and `junediverge --check-all`, and retires the debug defines. The earlier phases say "no review of its own" and
   the Done note says the debug define stays on. Engine or shared-code phases (CH`n`0, anything in `X2GUUser.*`,
   `X2GUNPC.*`, `X2DamageManager.*`, `Enum*`, core-list Lua) keep their own review. A run of inert transcription phases in
   one class file (char-luciel CH1b-CH1l) is reviewed every 2-4 phases, not each. Existing kickoffs keep theirs unless amended
   (char-rena and char-luciel are amended). After a review, write or check the
   next phase's kickoff the same way. The user should never have to ask "what's the kickoff".
6. **Commit with explicit pathspecs** (`git add <your files>`; never `-A` or `.`), message
   `<Plan> Phase <n>: <what>`. The plan doc, STATUS.md and kickoffs go in the same commit as the
   code's Done note or right after it.
7. **Hand-off to the user** (the plan's "Play-test hand-off" section): dev console first,
   copy-ready one-line commands with real values, per check: where / paste / what it does /
   look for / if not. End with the log tag you will read. Never launch the game.
8. Last line to the user: what they do next in plain words ("play-test the 3 checks above, then
   say 'check logs'"; or "next: `/x2-phase june-buff 6b review` on Opus 5.5 high").

## new-plan <name>

A new phased plan follows this shape so it works with the loop above:
- `<NAME>_PHASES.md`: Context; rules; **Starting context** that points at the tools (never at
  hand-typed msbuild); one `##` section per phase with Scope / Run with / Read list / Done gate.
  Short Done notes; long evidence goes to `<NAME>_ARCHIVE.md` or `.tsv` files.
- `kickoffs/<plan-key>/<phase>.md` for each phase, from `kickoffs/TEMPLATE.md`. Kickoffs name
  sections to read; they never say "read in full" and never repeat build/deploy instructions.
- A block in `STATUS.md` with one row per phase (State, Model/effort, Depends, Touches, Kickoff).
- One log tag and one flag family, named in the STATUS block.
- **Every plan runs through `/x2-phase` (user, 2026-10-16).** A phase of an existing plan starts with
  `/x2-phase <plan-key> <phase>`; a planning session (a plan that has no phases yet) starts with
  `/x2-phase new-plan <plan-key>`. A kickoff's Start line names one of the two, never "paste ...".
- **Planning brief.** On `new-plan <plan-key>`, first look for a brief that names that key in its
  Output row (backlog stages: `kickoffs/june-backlog/<stage>-plan.md`, listed in STATUS.md's
  `june-backlog` block). If one exists, its read list and task are the session's scope; then write
  the plan in the shape above, mark the brief's STATUS row done, and commit.
