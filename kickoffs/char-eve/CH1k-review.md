# June character Eve (Stage 7) - Phase CH1k review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1k review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1k done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1k - Done"
awk -F'	' '$6=="CH1k"' june2017_re/char_eve/ch0_class_map.tsv         # 40 rows ( 39 + 329ac330 from CH1c )
grep -a "Char Eve CH1k" june2017_re/funcmap.tsv                         # 46 rows: the 40 class-map rows ( 30 with a member, 10 resolved to March / engine code ) + 6 callee rows
```

The members are in the twelfth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (heading comment "lifecycle, render and reaction overrides"); declarations at the end of the class in `X2Lib/X2GuEve.h`, plus `JuneEveBlock9c50::StopEffectSets` and `EveElectraSystem::m_iJune1c0`. The constructor map is `june2017_re/char_eve/ch1k_ctor_offsets.tsv`. JUNE_BUGS 429-431.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1k --out <scratchpad dir>` (the listings of 329ce310 / 329ce210 / 329ce0a0 / 329cd860 / 329cd440 are the engine's slash-trace manager and its vector, see below). Ghidra has no function for `32977130` and `32973e70`. You decide:

- **Row resolution.** Five rows are engine / STL code folded into the class range (`329ce310` CreateUnitSlashTraceManager, `329ce210` its constructor, `329ce0a0` / `329cd860` / `329cd440` the vector of 0x50-byte entries), three are March struct constructors (`329cda50` EG_QueensThroneData = March; `329cc3b0` ENERGY_NEEDLES_Data = March; `32990da0` EveElectraSystem = March + `+0x1c0 = 1`), `32991c00` is `FlyingImpactData::GetLiveDamageEffect`, `329cf670` is the destructor's scalar-deleting wrapper. Confirm each is what the Done note says, and that no June-only behaviour was lost.
- **`32973d30` (vslot 0x3ac).** June's hook is `( DamageData*, float&, float& )`, called for attackType != AT_SPECIAL (`3228580a`), with `0x3b0` the AT_SPECIAL twin. Confirm AT_COMMAND (2) as the only gate, the roll (SA 5, or the buff `3229a470( 0x6348, 1, 1 )`), magic += SA 15, force down -= SA 42, no DT_MIX / x2.
- **`329a4680`.** The early return on DD `+0x628`, the defender-is-not-me test (`+0x278` protected value compared as one 32-bit word; the port compares UnitUID), the thousand star fire and lock-on updates against March, trigger 44 (`320017d0` + `CX2GUNPC::SetPosition` `32767930`), and the Nasod Core blocks left under `ADD_EVE_SYSTEM_2014` (gauge gain x ( 1 + `+0x6c50` ), trigger 5).
- **`329af460` / `329aece0` / `3299bd70`.** ReInit and the destructor against the listings (the six summon ids, `329ae670`, `329a2910`, `329cf170`); the constructor: every mapped member in `ch1k_ctor_offsets.tsv` (the March members, the June-only members, the struct ranges, the unmapped slots) and its value, the four March members June no longer has (`m_timerSummoned`, `m_fSummonedLifeTime`, `m_ChargingBoosterSkillDataBasePtr`, the Atomic Shield pair).
- **`329c47e0` / `329c3da0`.** The ten Lua keys (`ch0_keys.tsv`), their defaults and class groups (18 / 108, 19 / 109, 26 / 116), the tooltip's string ids 0x8fe1-0x8fe4 and argument orders, the unset buff time (JUNE_BUGS 429), and the two CX2Game calls left for CH4c (`322dee90`, `322e65c0`).
- **`32997c60` + `329ac330` + `32994750`.** March's Eve body against June (first oneshot, shock, trace, ApplyHyperModeBuff, Open Mind 25415 reset), the stand-ins (`32011d20` / `31fd66b0` / `32011f50`, `31febf50`), the Nasod Core gauge +40 under `ADD_EVE_SYSTEM_2014`.
- **Small ones:** `3297fdd0` (skill 25214 super armor), `32973e70`, `32977130` (the HP-fraction read, the `+0x8fc2` flag), `3297b4b0`, `329770d0`, `3299cdc0`.
- **Alpha tag (7c):** the one site in this phase, `329a2a0b` (the die mesh in `June_ClearThousandStar`).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1k commit>^..<CH1k commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1k - Review" into the plan doc under the Done note, update the CH1k row in `STATUS.md`, check that `kickoffs/char-eve/CH1l.md` still matches, and commit with explicit pathspecs.
