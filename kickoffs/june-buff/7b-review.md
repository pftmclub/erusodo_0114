# June buff port - Phase 7b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 7b review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 7b done |
| Flag | SERV_IRUHADEV_JUNE_BUFF_NEW_FLAG |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 1c, item 2" "Shared checklist" "Phase 7b -- group C" "Phase 7b -- Done"
```

The plan doc's "Starting context" predates `tools/`: wherever it (or the task below) says to
build, deploy, check size/mtime, run a fresh unifdef, run `file`/`git diff --stat` for hygiene,
or read logs by hand, use `tools/build.py --deploy`, `tools/flagcheck.py`, `tools/logs.py` and
`tools/bytepatch.py` instead (the x2-phase skill's "Tools" and "Finish" lists).
Never launch X2_offline.exe; ask the user.

## Task (verbatim from the plan doc)

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

## Scope as landed (Phase 7b, commit 25f51ed; added after the phase)

- **110 is store and getter only.** `TryHyperModeStateChange` was not ported: it needs June's
  awakening-lockout timer `+0x8d78` and flag `+0x8d4c`. Review decision (a) in the Done note
  (options A/B/C are the user's), not a ported consumer.
- Pieces 7b added that the task above does not name; check each against June:
  - the `CAN_MANA_BREAK` state key (June `@320608b2`), placed next to the carry block in
    `CX2GUUser::CommonStateStart` instead of where June has it; confirm nothing between the two
    June positions reads `+0x6890`/`+0x6892`;
  - `CHECK_NEVER_MOVE_TRUE` (`+0xbb`) in both NPC condition chains, and `CAN_DETONATION`
    (`+0xba`) in the non-host chain only;
  - the NPC carry block, anchored at the top of `CX2GUNPC::CommonStateStart` rather than at
    June's `@327af365`; confirm no early return in March's NPC `CommonStateStart` lies between
    the two positions;
  - `JUNE_BUGS.md` #10 (the vestigial user carry block).
- 97's Add/Ara/Rose/Luciel overrides and 101's death-prevention gates and low-HP UI went to
  `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7 by the standing rule; check the
  attribution (Ara's `+0x9644`, Chung's gage layout), not their absence.
