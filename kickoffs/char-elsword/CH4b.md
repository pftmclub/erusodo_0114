# June character Elsword (Stage 7) - Phase CH4b - Damage exception and overrides

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH4b` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH4a review. One at a time: `X2GUElsword_SwordMan.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_PASSIVE` (+ `_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ELSWORD PASSIVE`) |
| Review | `kickoffs/char-elsword/CH4b-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "Starting context" "Phase CH4 - Passives and class mechanics"
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "CH4b - Done (2026-11-03)"
```
Then the Elsword lines of the checklist rows named below (`june2017_re/char_elsword/plan_checklist_rows.md` gives the line numbers), the
`CH4b` rows of `ch0_*.tsv`, and Chung's matching CH4 Done note as the worked example.

## Task

`SetDamageExceptionProcess` `32d1e0e0` whole (June-only, 7b; March has none): 21419 `RNSI_P_EIS_OPEN_MIND` with `ATTACK_TRIGGER_TYPE`
0xf -> `+0x510` = 1.0; 21420 `RNSI_P_EIS_INFINITY_POTENTIAL` with 0x10 -> vslot 0x238 (21420, 4, 1) and the `322b5320` push (the trigger-type
values come from effect C5). The other overrides CH0 found (`GetActualMPConsume` / `GetActualCoolTime` style); checklist §7 l.281 power-rate
socket range (`32d076f0`); the death-prevention snapshot override `32d058a0` (vslot 0x740) unless CH0 gave it to CH1q (Chung's CH4b
`32bed760` is the worked example; Crest of Time's cast is char-add's).

June's whole bodies, June quirks verbatim with `JUNE_BUGS.md` rows. You are on Opus: delegate the reads, greps and known-address Ghidra dumps
(x2-phase, *When the phase is Opus*). CP949 files through `bytepatch.py`. Mark rows `(Elsword: CH4b)`. Done gate: each item's effect visible in
game or in a named log line.

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

## Amended by CH0 (2026-11-03)

- **Functions: 3** (`awk -F'\t' '$7=="CH4b"' june2017_re/char_elsword/ch0_class_map.tsv`; inside 1, new 2).
  June addresses: `32cf4b70` `32cf4df0`(new GetActualCoolTime) `32d1e0e0`(new SetDamageExceptionProcess).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7b L328 per-class SetDamageExceptionProcess blocks, funcmap 32d1e0e0 (Elsword).
- Read whether June's `GetActualMPConsume` `32cf4b70` still applies the Vigor x0.7 MP rule (CH0's WoS read found no WoS access; unproven).

## Amended by CH1k (2026-11-04)

- June's skill-use cooldown `32d0d8e0` (CH1k's `SpecialAttackCooldown`) asks `GetActualCoolTime( templet, 1, 0 )` (JUNE_BUGS 524) and no longer carries March's cool time extras: memo 22 Phoenix Talon -5 s, memo 6 Endurance -5 s, the Compact Counter cool time and `ST_ACTIVE` x `m_fIncreaseCoolTimeRel`. Read `32cf4df0` (`GetActualCoolTime`, vslot 0x6a4) for where June applies them.
- CH1k's lookup `32d1bd10` applies Mind of Fighter (`m_iBrutalSlayerActivatedLevel` x `m_fMindOfFighterConsumeMpRate`) and the Vigor x 0.7 MP rule itself, after `GetActualMPConsume`: check that `32cf4b70` does not apply either again.

## Amended by CH1q (2026-11-04)

- Way of the Sword pieces CH0 did not locate in June (`ch0_wos.tsv` rows 83 / 84 / 87 / 95), still March under `_WOS`: the shock / frozen end
  `m_bCured` branch (`X2GUUser.cpp:20093`, `:22310`) and `CureExtraDamage` (`X2GameUnit.cpp:9738`, June `3227d390` confirmed), `ResetAnimSpeed`'s vigor
  factor (`X2GUUser.cpp:35094`), the `SAT_WAY_OF_SWORD_GAUGE_UP` reader (`X2ItemManager.cpp:1120`, `X2Game.cpp:11785`). Read June's for each and port
  any change under `_WOS`.
