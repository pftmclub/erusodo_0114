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

## status / next (read-only)

Read `STATUS.md` only. Answer in plain words, short:
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
   and branch. Never touch, stage, revert or "clean up" files you did not change.
4. **Read only what the kickoff lists**, with
   `python tools/docsection.py <plan doc> "<section>" "<section>" ...`.
   Never read a plan doc "in full"; if you need something unlisted, `--list` the outline and
   pull that section. Where a doc's "Starting context" describes building, deploying,
   unifdef or log checks by hand, the tools below replace it.
5. **Do the task** in the kickoff. Standing rules (CLAUDE.md, memory) still apply: one
   `SERV_IRUHADEV_` flag per logical change in `KTDXLIB/Always.h`, original code in `#else`,
   cite June addresses, never invent values, design decisions go back to the user.

## Tools (use these, not hand-rolled equivalents)

| Need | Command | Skill |
|---|---|---|
| Edit a CP949 file | `python tools/bytepatch.py show/find/apply` | x2-byte-edit |
| Build + deploy | `python tools/build.py --deploy` (background) | x2-build-deploy |
| Flag hygiene / unifdef | `python tools/flagcheck.py <FLAG> --worktree` then `--range` | x2-byte-edit |
| Play-test logs | `python tools/logs.py --tag <TAG>` | x2-check-logs |
| One doc section | `python tools/docsection.py <doc> "<heading>"` | - |

## Finish (same session)

1. `flagcheck.py <FLAG> --worktree` before committing: RESULT must be PASS (or every FAIL explained and fixed).
2. `build.py --deploy`: paste its `DEPLOY OK` line into the Done note. Check the phase's
   strings are in the deployed exe if the phase's gate asks for it.
3. **Done note** in the plan doc, under the phase: what landed (commit, flag), hygiene lines
   from flagcheck, the deploy line, per-item "how to see it in game", open questions for the
   user, `/cost` if the user gives it. **Keep it under ~60 lines**; long RE tables go to a
   `.tsv` (e.g. `june2017_re/funcmap.tsv`) or the plan's archive file, and the note links them.
4. **STATUS.md**: update this phase's row (state, commit, what the user owes) in the same commit.
5. **Review kickoff**: if the phase needs a review and `kickoffs/<plan>/<phase>-review.md` does
   not exist, write it from `kickoffs/TEMPLATE-review.md`. After a review, write or check the
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
