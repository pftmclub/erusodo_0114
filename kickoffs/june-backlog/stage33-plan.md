# June port backlog - Stage 33 planning - June damage and character stat calculation

| | |
|---|---|
| Start | new conversation on **Opus 5.5, xhigh** effort, then type `/x2-phase new-plan june-stat-calc` |
| Output | plan key `june-stat-calc` (`kickoffs/june-stat-calc/`, a `june-stat-calc` STATUS block) |
| Plan doc | `JUNE_PORT_BACKLOG_ORDER.md` (Stage 33), output `JUNE_STAT_CALC_PHASES.md` (new) |
| Branch | `main` |
| Depends on | - for planning (read-only, any time); code after the character ports, Stages 7 and 8 (user, 2026-10-27) |
| Flag | one family, `SERV_IRUHADEV_JUNE_STATCALC_*` (none in this planning session: no code) |
| Log tag | `JUNE_STATCALC` (propose it in the plan) |
| Review | none (a planning session); each planned phase gets its own review kickoff |

## Read first (and nothing else unless you need it)

```sh
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 33: June damage and character stat calculation (user, 2026-10-27)" "Standing rules (every stage, every kickoff)"
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Phase 16 — Socket-option parity (added 2026-10-04; carries 92 and 78's socket terms)"
grep -n -E "GetFinalDamage|SetDamageExceptionProcess|DamageReact|SetGameStat|ResetMaxMP|320839e0|32290950|322b4130|322b4170|320870a0" june2017_re/funcmap.tsv
grep -n "SERV_IRUHADEV_JUNE_SOCKET_HIT_MP_CURVE" X2Lib/X2GameUnit.cpp X2Lib/X2GameUnit.h
```

Also `tools/data/june_march_divergent.tsv` rows with verdict `stage33` (8 formulas plan `june-march-divergence` D0b handed here, 2026-10-28).

## Task

Plan the replacement of every mixed March / June stat and damage formula with June's whole calculation (the user, 2026-10-27).
Do the RE read-only in Ghidra and `june2017_re/disasm_range.py`; delegate greps and doc reads to subagents.

- **The trigger.** Buff Phase 16 seeded the raw socket sums as the option value of MP members 24/25/26 and moved their refresh `32290950`
  to the option-data curve. The per-hit refreshes stayed March's plain update, so a ranged hit added +6722 MP. June uses the curve there
  too (`322b4130` / `322b4170` / `320870a0`); fixed by `SERV_IRUHADEV_JUNE_SOCKET_HIT_MP_CURVE`. Find every other pair like it: a June
  seed, curve or refresh with a March reader or per-event update beside it.
- **The map (phase 0 of the plan).** One `.tsv` row per formula function and per `CX2AdditionalStatByBuff` member / stat field. Columns:
  - June address and the March counterpart (`file:line`);
  - port state **June / March / mixed**, and the flag that last touched it;
  - who reads the result.

  Cover all of these:
  - the character stat sheet: base stats, equipment, set items, sockets / options, titles, pets, passives, buff members,
    `SetGameStat` `322814a0`, `ResetMaxHP` / `ResetMaxMP` `31fd2810`, and the character-info UI numbers;
  - damage: `GetFinalDamage` `32284aa0` and its helpers, `SetDamageExceptionProcess` `322990e0` + the per-class overrides, user /
    NPC `DamageReact`, defence / resist / enchant, PvE / PvP / boss rates;
    (char-elsword CH1o, 2026-11-04: the user hooks 0x508 / 0x50c @32284d2e-32284d76 are ported under `_LIVE`; the next call, `this` vslot 0x6c
    `3229a530` ( pDamageData, &rP, &rM ) @32284d98 that multiplies both attacks, is not: it belongs here);
  - resource gain: MP on hit / hit taken, awakening gauge, regen, MP cost (incl. the six `GetSkillMpDecreaseRate` `3234d840` call sites, moved from june-march-divergence D1-3f 2026-10-29; see backlog Stage 33).
- **Inherited, not redone** (verify each row's June claim by a spot-check, do not re-port): the june-buff members, socket parity, effect
  C3 damage data, ExtraNPCStat, Stage 3 skill values, transcendence.
- **The phases.** One phase per area of mixed rows. Each one:
  - ports June's whole function bodies, March's in `#else`;
  - gets an Opus review.
- **Verification design.** Plan a `SERV_IRUHADEV_JUNE_STATCALC_DEBUG` stat-sheet and per-hit breakdown log (the temporary
  `SERV_IRUHADEV_MP_JUMP_DEBUG` probe in `X2GageData` / `X2GameUnit` is the model). Plan Unicorn runs of June's pure formula
  functions against the port for the same inputs (precedent: buff Phase 16 review emulated `320839e0` on 360 input sets).

Output:
- `JUNE_STAT_CALC_PHASES.md` (map phase first, Opus review each);
- one kickoff per phase in `kickoffs/june-stat-calc/`;
- a STATUS block;
- funcmap rows tagged `StatCalc S33`;
- the map `.tsv` in `june2017_re/statcalc/`.

The code runs after the character ports (Stages 7 and 8; user, 2026-10-27), so plan the play-tests across every character, each on a
geared level-99 unit with and without sockets. A mixed row that breaks play before then is fixed at once under *Scope* and marked June
in the map. Open design questions go to the user at the end, each with a recommendation.
