# June character Aisha (Stage 5 pilot) - Phase CH0c - Registration map correction

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH0c` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1b review (done; it found the defect) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (comment corrections only) |
| Log tag | none (no play-test) |
| Review | none (the next phase reviews against the corrected map) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "Phase CH0c - Registration map correction" "CH1b -- Review"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH0 - Done (2026-10-20)"   # what the maps are and how they were built
grep -a "^32ce77b0 @32ce9b5d\|^320775a0" june2017_re/funcmap.tsv                      # the Init call and the reset
diff june2017_re/char_arme/ch1b_review_reg_old.tsv june2017_re/char_arme/ch1b_review_reg_fixed.tsv | head -40
```
The builders are `june2017_re/char_arme/ch0_build_state_skill.py` and `ch0_build_class_map.py` (their docstrings name their inputs). The extractor is
`june2017_re/charport_states.py` (read `cmd_init`).

## Task

Do the plan section "Phase CH0c" (its Scope 1-5 and Done gate). In short:
- **`~` rows.** Make `charport_states.py` snapshot a per-class branch at its closing brace (its own stores since the branch began, the id it sets), then add the stores
  made after the merge and before the `31fd89b0` call. Check all 33 `~` rows against the listing (`june2017_re/funcdis.py --func 32ce77b0` / `32cea0c0`, saved to
  the scratchpad; read the pushes before each call). A row the tool still cannot read is written by hand from the listing, marked `listing`.
- **Rebuild** `ch0_state_skill.tsv` part 1 and the class map's `states` column from the fixed `reg.tsv` (and an unchanged `skill.tsv`). Do not move a phase CH1a / CH1b already
  ported. List every phase that would move among CH1c-CH1i and move it only if unported. Update the affected `CH1c.md`-`CH1i.md` counts.
- **Correct the claims** written from the old map: the registration comments of the CH1a and CH1b blocks in `X2Lib/X2GUArme_VioletMage.cpp` (CP949: `bytepatch.py`
  only; the CH1b list is in "CH1b -- Review", and also fix the header comment's `GetJumpSpeed() ( +0x4978 )` -> `+0x4a78`); the `Char CH1a` / `Char CH1b` funcmap rows; the
  CH1 kickoff where it relies on registrations. Leave the CH1a / CH1b Done notes as they are; say what changed in this phase's Done note.
- Delegate the mechanical parts (x2-phase, *When the phase is Opus*): the listing pulls, the builder re-runs, the diff tables, the bytepatch spec run and flagcheck,
  the build. Keep the `~` row verdicts and the phase-move decisions.
- If June's registration shows something the CH1 plan does not cover (a state with no March counterpart, a callback outside the class range that the map missed),
  apply the scope rule: in this plan only if it is Aisha's; otherwise identify it and give it an owner.

Done gate: as in the plan section. Nothing to play-test; the hand-off says so and names CH1c next.
