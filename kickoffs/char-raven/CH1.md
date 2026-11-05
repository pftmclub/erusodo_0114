# June character Raven (Stage 7) - Phase CH1 - Final: LIVE, the script, the motions, the play-test

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1q review; CH0b done (its motion koms packed and held for this phase) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (defined here), `_CORE`, `_RAGE` |
| Log tag | `[JUNE_CHAR]` (`RAVEN DATA`, `RAVEN CORE`, `RAVEN RAGE`) |
| Review | `kickoffs/char-raven/CH1-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Raven map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1 - Done (2026-11-04)" "CH1 - Review (2026-11-04)"
```
Then the Raven CH1a-CH1q Done notes and reviews.

## Task

1. Define `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` in `Always.h`; `flagcheck.py JUNE_CHAR_RAVEN_LIVE`.
2. Adopt `RAVEN_FIGHTER.lua` (`luasrc.py adopt-june`, `--dry-run` first); `luabind.py check RAVEN_FIGHTER`. A `[JUNE_CHAR] RAVEN DATA` line logs
   the script load. Re-run `charport_scan.py` and `charport_keys.py RAVEN_FIGHTER`: nothing new.
3. Run a Lua load check on every core-list file the class reads (Eve's `DamageEffect.lua` lesson); confirm Raven's combo X back speed reads
   `INIT_PHYSIC.WALK_SPEED` as June (`SERV_IRUHADEV_JUNE_DAMAGE_EFFECT_INIT_PHYSIC`).
4. Check `CX2GURaven::ReservedSkillStart` is in place (CH1k) before the play-test.
5. Build, pack, ask before each copy (data036 with CH0b's held motion koms), then hand over the play-test: a Raven per job tier; combos, dash,
   jump, riding, a bounding hit, damage, parrying, the rage gauge filling, the Nasod balls attacking and the hyper orbs, awakening; each skill
   cast twice (cool time on the slot, MP drop); the `ErrorLog.txt` lines to compare against a pre-CH1 run.
- Check that March's `CommonStateStart` accepts June's empty `ANIM_NAME` on the wait-state-change table (7h).
- Mark the ported checklist rows `(Raven: CH1)`. A play-test defect that needs its own work gets the next letter (`CH1r`) with a kickoff and
  STATUS row.

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

## Amended by CH0 (2026-11-05)

- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): intro L14 character script RAVEN_FIGHTER.lua feeds SetDamageDataFromLUA; §2 L120 "ADDITIONAL_ATTACK_TRIGGER_TYPE x7 in RAVEN_FIGHTER, status ""to-imple; §5 L184-198 key BURNING_NASOD_HAND_POSSIBLE x24; §7h L423 EFFECT_SET_LIST_* variants (_FINAL / _FINAL_ULTIMATE); §7h L427 USEFUL_VALUE rows and getters; §7o L507 SET_LIMIT_MAX_G_SPEED in <prefix>DAMAGE_BOUNDING_DOWN (-4000) and Rave; §7p L548 TIME_STOP_IMMUNE_STATE = TRUE in the class script; §7t L592 SHOW_HYPER_CUT_IN_TIME in the class script; §7ad L752 stage June Motion_Raven* files with the class script; Raven's OT_Combo.
- Copy the held `Motion_Raven_OT.x` with the script (`kom.py stage-june june2017_re/char_raven/ch1_held_motions.tsv`, then pack and ask for data033). Adopt `RAVEN_FIGHTER.lua`. Its key typos `ANUM_SPEED` and `STOP_TIME_ATK` (8 tables) are in March's script too (no reader either side): `JUNE_BUGS.md` rows on adoption.
