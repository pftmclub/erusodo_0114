# June port backlog - Stage 35 planning - March behaviour inside June code

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase new-plan june-march-divergence` |
| Output | plan key `june-march-divergence` (`kickoffs/june-march-divergence/`, a `june-march-divergence` STATUS block) |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 35), output `JUNE_MARCH_DIVERGENCE_PHASES.md` (new) |
| Branch | `main` |
| Depends on | - (any time; its fix phases touch class files, so they never run beside a phase of that class's port) |
| Flag | fixes go under the flag that owns the code being fixed (e.g. `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE`); the gate tool needs none |
| Log tag | none expected (propose one only if a phase needs a log line) |
| Review | none (a planning session); each planned phase gets its own review kickoff |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 35: March behaviour inside June code (user, 2026-10-28)" "Standing rules (every stage, every kickoff)"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH4a - Review (2026-10-28, Opus 5.5): PASS after one fix"
cat tools/junediverge.py tools/data/june_march_divergent.tsv     # draft from the CH4a review, uncommitted (may be missing: then start fresh)
python tools/junediverge.py --check X2Lib/*.cpp X2Lib/Offline/*.cpp KTDXLIB/*.cpp
grep -n -i -E "stub|always returns|always answers|unconditionally|differs from March|March has no" june2017_re/funcmap.tsv
grep -n -E "322cc1a0|322ece70|GT_PVE" X2Lib/X2GUArme_VioletMage.cpp X2Lib/X2GuEve.cpp X2Lib/X2GUUser.cpp X2Lib/X2DamageManager.cpp
```

## Task

The user (2026-10-28): "i just dont want the march related issue keep happening on june's logic or work". Plan how to make that structural,
not something only a review catches. Delegate the greps and doc reads to subagents; keep the judgement calls.

- **The failure.** A June body is transcribed with March's functions by name. Where a March function's June namesake is a stub or behaves
  differently, the port silently keeps March's behaviour, and the build and play-test both pass. Examples:
  - char-eve CH4a: `GetEqippedSkillMemo`. June's `31fca9e0` always answers false; March's answers true when the memo is equipped, or under
    `_IN_HOUSE_` frame scale.
  - Aisha CH2a: the PvP-or-PvE flag `322cc1a0`.
  - Aisha CH3c: the `322ece70` power-rate flags.
- **Phase 0, the table (`tools/data/june_march_divergent.tsv`).** One row per March function or idiom whose June counterpart differs:
  name, June address, what June does, what to write instead, source (funcmap row / review).
  - Seed it from the funcmap ("stub", "always ...", "June-only", `GT_PVE`) and from the Done / review notes that fixed this kind of slip
    (`git log --grep "transcription"`).
  - Rule each candidate in or out. A March name whose June body is the same is not a row.
- **The gate.** Finish or rewrite `tools/junediverge.py --check <files>`.
  - June code = `June_*` / `June*` member bodies, and the ON branch of a `SERV_IRUHADEV_JUNE_*` `#if`; never the `#else`.
  - Prove it on the known cases: the CH4a site before the fix (`git show 2be896e6:X2Lib/X2GuEve.cpp`) must hit, today's tree must hit
    `X2GUArme_VioletMage.cpp:27702`, and March `#else` bodies must not hit.
  - Wire it into x2-phase "Finish" and the Stage 7 character template, next to `enumscan` / `luabind` / `skillicons`.
  - Add a table-maintenance rule: a review that finds a new divergent function adds the row in the same commit.
- **Phase 1, the sweep.** Every current hit is checked against the June listing:
  - a real slip is fixed under its owning flag, with a rebuild and a JUNE_BUGS row if June's behaviour is a bug;
  - a false hit gets an allow-list entry with the reason.
  - Start with the Aisha hit above (check `June_AVSI_ABM_DASH_COMBO_Z2a_FrameMove` against its June address in the funcmap).
- **Also consider** (recommend, do not decide):
  - wrapper functions that make the June behaviour the only one reachable from June code (e.g. a `JuneGetEqippedSkillMemo()` that returns
    false, which the gate then requires);
  - whether the table should drive the transcription itself (the porting phases read it before writing a body).
- **Out of scope:** collapsing the `June_*` prefix back into March's functions (a separate per-character cleanup after each character's
  Z phase, the user's call).

Output:
- `JUNE_MARCH_DIVERGENCE_PHASES.md` (Phase 0 table + gate, Phase 1 sweep, a review each);
- one kickoff per phase in `kickoffs/june-march-divergence/`;
- a STATUS block;
- this brief's STATUS row marked done.

Open design questions go to the user at the end, each with a recommendation.
