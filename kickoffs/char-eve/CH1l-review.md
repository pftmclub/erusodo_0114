# June character Eve (Stage 7) - Phase CH1l review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1l review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1l - Done"
awk -F'	' '$6=="CH1l"' june2017_re/char_eve/ch0_class_map.tsv         # 11 rows ( 10 + 329cc500 from CH1e )
grep -a "Char Eve CH1l" june2017_re/funcmap.tsv                         # 18 rows: the 11 class-map rows, 7 callee rows
```

The members are in the thirteenth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (namespace `_CONST_EVE_JUNE_CH1l_`); declarations at the end of the class in `X2Lib/X2GuEve.h`. JUNE_BUGS 432-433.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1l --out <scratchpad dir>` (the 11 listings). You decide:

- **`32968f20` (vslot 0x7b8).** The five skill blocks and their state ids (211 / 212 / 213 twice each, 222 / 223 twice, 239-241, 205 / 206, 242 with no templet test), the ByID stand-in for June's dirty-state batch (`3207c1a0` / `31fdf970`, the Aisha precedent), and the `bChangeAll_` / skill-id gate on each.
- **`32977390` (vslot 0x684).** The three slot searches (June's 0-9 slot space, A 0-4 / B 5-9, no `GetEnabledSkillSlotB`), `31edf560` against `32206000` (tree null first, then equal or `GetUpgradedSkillID`), `31edf580` for the Kugelblitz loop, the B lock on MY unit only in that loop (JUNE_BUGS 432), `m_iJune1c0` = 3 / 1 from `+0x97e4` (`m_bSystemEnable`) and `+0x9978` (`m_bHyperOpticalResearch`), and the Queen's Throne tail (`+0x96b8` = `m_sEGQueensThrone.m_fPowerRate`, x ( `3201ad10` + 1.0 )). The member offsets were probed with `offsetprobe` against March's layout (the `EveElectraSystem` base is `+0x97e4`).
- **`32978e80` (vslot 0x6fc).** The jump tables (`329792cc` / `329792f4` / `3297931c`, case i -> body i), the key bytes by role (June `+0x50` / `+0x54` = March `pureSkill5` / `pureSkillEx5`), the branch order (Mega, Taser, Kugelblitz, else Init), and that the tutorial block (`32978ec2`, `320708f0`) is left out on purpose (backlog l.480).
- **`329b7390` + `329b4ac0` (vslot 0x7b0).** The four arguments, the Energy of Concentration clone (25402), the base call with 0 (JUNE_BUGS 433), the factor index choice, `AdjustBuffFactorByIndex( 25201 )` (June passes the constant), the two `32d41910` calls (`JuneSetTimeFinalizerValue( v, true )`), the loop filters (team, user, 490000, `CanApply`, both `SetBuffFactorToGameUnit`), no party test, the core particle by `32205440` (`JuneEveAccelCoreParticleName`: the whole June table), and the one alpha tag `329b77ad`.
- **The small ones.** `32979a70` (vslot 0x65c; the NPC byte `+0x8751` stands in as false: check that no Eve-range code sets it), `3298c580` (vslot 0x6ac; the arg is an NPC id), `3299aee0` (vslot 0x77c; class 109, `+0x8b40` = self-destruction), `32977090` (vslot 0x6a8), `3299cf70` (0.05 / 0.83 / false).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1l commit>^..<CH1l commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1l - Review" into the plan doc under the Done note, update the CH1l row in `STATUS.md`, check that `kickoffs/char-eve/CH1l2.md` still matches, and commit with explicit pathspecs.
