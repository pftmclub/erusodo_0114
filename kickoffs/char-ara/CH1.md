# June character Ara (Stage 7) - Phase CH1 - Deploy unit live: _LIVE, the June script, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1q2 review, CH0b |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE`, `_FP`, `_SPECTER` defined (+ `..._DEBUG` for the load lines) |
| Log tag | `[JUNE_CHAR]` (`ARA DATA`, `ARA CORE`, `ARA FP`) |
| Review | `kickoffs/char-ara/CH1-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "CH1 - Done (2026-11-09)" "CH1 - Review (2026-11-09)"
```

## Task

Define `SERV_IRUHADEV_JUNE_CHAR_ARA_LIVE` (+ `_FP`, `_SPECTER`); adopt `Ara_Martial_Artist.lua` (`/x2-adopt-june`, `--dry-run` first; March's
copy is not T1, adoption replaces it) and pass `python tools/luabind.py check Ara_Martial_Artist` (every unbound name bound whole or in
`lua_unbound_known.tsv`); copy CH0b's held motion koms (if any); pack, copy; full build and deploy; the play-test on units of 7 / 28 / 31 / 118 / 119
(CH0b's walk units and 24 / 38 / 48). Save a pre-CH1 ErrorLog first.

Done gate (plan, CH1 final): June's script and motions load (`ARA DATA` line); no new `KEM_ERROR` / `Motion Error` / missing-animation line against
the pre-CH1 run; combos, dash, jump, double jump, damage and riding play June's animations; force power fills, spends, cancels and shows on the gauge
as June; berserk enters at June's count; each skill cast twice shows its cool time and spends MP; `charport_scan.py` / `charport_keys.py
Ara_Martial_Artist` show nothing new. Hand-off: dev console first, per check where / paste / look for / if not.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0 (2026-11-10)

- **Checklist rows: 9** (`awk -F'\t' '$5 ~ /(^| )CH1( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L14 character script Ara_Martial_Artist.lua feeds SetDamageDataF; L208 adopt the character's June script with luasrc.py adopt-june,; L423 "EFFECT_SET_LIST_* variants (_FINAL / _FINAL_ULTIMATE; Ara a; L427 USEFUL_VALUE rows and getters (every class with June scripts; L509 SET_LIMIT_MAX_G_SPEED in <prefix>DAMAGE_BOUNDING_DOWN (-4000; L548 TIME_STOP_IMMUNE_STATE = TRUE in the class script (76 June s; L592 "SHOW_HYPER_CUT_IN_TIME in the class script (""plus Ara's be; L669 combo hits carry June's AT_COMMAND tag; adopt the script wit; L760 stage June Motion_ARA* files with the class script; skill an.
- **Specter / tail rows: 2** (`awk -F'\t' '$7 ~ /(^| )CH1( |$)/' june2017_re/char_ara/ch0_specter.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- Held motions `Motion_Ara_LD.x` / `_SD.x` (`ch1_held_motions.tsv`). Before deciding the LD case: June's script also plays the mixed-case `SI_SA_ALD_ENERGY_WAVE_Start` (src.lua:14486, 14614) against a file with `_START`: check June's animation lookup (March `KTDGXSkinAnim.cpp:2131` is case-sensitive); if June's is case-sensitive too it is June's bug (keep, `JUNE_BUGS.md` row).

## Amended by CH1q (2026-11-11)

- Turning on `_FP` brings June's force-power rework (CH1q Done note): the play-test checks force power filling on hits (`m_iHitAddForcePower`), dropping when a skill spends it (and MP rising by the spend x 20 x specialization B), the hyper-count gain (3 / 6 / 10 by orbs used), berserk at three orbs (portrait changes to the rage picture and back), and the berserk kill reward (HP + 3 percent, force power + 1, `EffectSet_KILL_ENEMY_WHEN_FULL_HYPER_MODE`). The `[JUNE_CHAR] ARA FP` log lines are retired by the CH1q review, so these checks are visual.
- Max force power is 10 for every level (June's push `0xa`), not March's level formula.

## Amended by CH1q2 (2026-11-11)

- Define the two class-agnostic `CX2GUUser` flags with Ara's: `SERV_IRUHADEV_JUNE_CHAR_SPECTER_STATE` (needed by `_SPECTER`) and `SERV_IRUHADEV_JUNE_CHAR_CHAIN_ANIM_SPEED` (needed by `_LIVE`); the `Always.h` guards stop the build if either is missing. They touch `StateParam` (`X2GameUnit.h`), so the build takes `--pch force`.
- Tail check (`_SPECTER`, CH1q2 Done note): in berserk the tail that matches the buff hyper count shows (`Motion_Ara_Tail_03.x` / `_06.x` / the 41580 tail), with no stray attack box on Rfoot / Lfoot / Lhand2; leaving berserk shows the normal view list again. `IS_SPECTER_STATE` itself only shows on 34 / 124 (CHNa).
- Chain anim speed (`CHAIN_ANIM_SPEED`): on a 118 with Fullmoon Slash / Valor Devotion learned, a chained skill plays faster than the same skill cast on its own. Optional if no such save exists; say so in the hand-off.
