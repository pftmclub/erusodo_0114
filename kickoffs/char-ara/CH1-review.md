# June character Ara (Stage 7) - Phase CH1 review - Final: LIVE, FP, SPECTER, the script, the motions, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1 review` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | Phase CH1 done (its Done note, commit and the user's play-test); any `CH1r`-style fix the play-test opened done |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, `_FP`, `_SPECTER`, `SERV_IRUHADEV_JUNE_CHAR_SPECTER_STATE`, `SERV_IRUHADEV_JUNE_CHAR_CHAIN_ANIM_SPEED` (all defined by CH1; no debug define) |
| Log tag | `[JUNE_CHAR]` (`ARA DATA`; the `ARA FP` lines were retired by the CH1q review) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1 - Done (2026-11-11)" "CH1q - Review (2026-11-11): pass, debug define retired" "CH1q2 - Review (2026-11-11): pass, no code change"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1 - Review (2026-11-09)"
```

## Task

Review CH1 final against its Done gate (plan section "Phase CH1 - Deploy unit", *Done gate (CH1 final)*):
- **Deploy unit:** data036 `Ara_Martial_Artist.lua` equals the `june2017_re/june_adopt_20261111_121334.tsv` ljconv row (size, sha1, header
  `1B 4B 4C 81`); no `.src.lua` sidecar; data033's `Motion_Ara_LD.x` / `_SD.x` are June's bytes (`kom.py stage-june --dry-run
  june2017_re/char_ara/ch1_held_motions.tsv`: 2 identical); `kom.py status data036 data033` in sync with live. The deployed exe holds
  `ARA DATA` once (`tools/exestr.py --form wide`).
- **`ARA DATA` lines:** state count per class (C++ registrations on the class path from `june2017_re/char_ara/ch1m_idmap.tsv` + the hook states
  (`DAMAGE_BOUNDING` x3, `ASI_WAIT_STATE_CHANGE`) + `USI_EVENT_*` + 20 slot states + key 0 + learned-skill sub-states, as Rena's review),
  `wait_state_change 1`, a June `MOTION_ARA*` device id, script `Ara_Martial_Artist.lua`.
- **START / WIN / LOSE:** the rolls pick real `ASI_` tables (no `list size 0` / `re-bind skipped` in the log).
- **Force power / berserk (`_FP`, visual, per the CH1q amendment):** fill on hits, drop on a spend with MP rising, hyper-count gain 3 / 6 / 10,
  berserk at three orbs (rage portrait and back), the berserk kill reward; max 10 at every level. A step the user's run missed is asked once.
- **Tails (`_SPECTER`):** the berserk tail matching the buff hyper count shows, no stray attack box; leaving berserk restores the view list.
- **Chain anim speed:** on unit 98 (118, Fullmoon Slash 27402 learned) a chained Pulling Pierce plays faster than one cast alone (optional if the
  run did not chain).
- **Skills:** the two-cast check (cool time shows, MP drops: `ReservedSkillStart`) only if the run cast one; skills still run March's id switch
  until CH2a, so a skill that misbehaves is CH2a's, not a CH1 defect.
- **ErrorLog** against `data\ErrorLog_pre_CH1_ara.txt` (425,351 B, saved 2026-11-11 before CH1) and, for Ara-load lines,
  `ErrorLog_pre_CH0b_ara.txt`: no new `KEM_ERROR` / `Motion Error` / missing Ara animation and no new `[LUA_SRC]` error. CH0b's routed
  `ErrorEnum(433) Anim Frame : 80, Mesh Frame : 81` face-mesh lines: still present with June's script and motions -> a `JUNE_BUGS.md` row (CH0b
  Done note); gone -> say so. Anything else new is routed under the scope rule.
- The `(Ara: CH1 ...)` checklist marks (l.16, 212, 423, 427, 509, 548, 592, 674, 760) and the `charport_scan.py` / `charport_keys.py` verdict.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ARA_LIVE --range <CH1 commit>^..<CH1 commit>` (and `_FP`, `_SPECTER`, `JUNE_CHAR_SPECTER_STATE`,
`JUNE_CHAR_CHAIN_ANIM_SPEED`), `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUAra.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only). Fix clear transcription errors under the same flag; hand design decisions back to the
user. Write "CH1 - Review" into the plan doc, update the CH1 STATUS.md row (CH5, CH2a, CH2b0 and CH2c0 become ready), check
`kickoffs/char-ara/CH5.md`, `CH2a.md`, `CH2b0.md` and `CH2c0.md` still match (line references moved since CH0), and commit.
