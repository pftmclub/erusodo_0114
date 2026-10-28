# June character Eve (Stage 7) - Phase CH3a review - Skills: Code: Nemesis (108)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH3a review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH3a done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (no debug flag in CH3a) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH3a - Done (2026-10-28, Sonnet 5.5)"
```
Then the `Char Eve CH3a` rows at the end of `june2017_re/funcmap.tsv` and the ten CH3a cases in `june2017_re/char_eve/ch0_skill.tsv` (lines for 25008 / 25009 / 25010 / 25011 / 25043 / 25044 / 25205 / 25206 / 25221 / 25235).

## Task

Review the CH3a transcriptions against the raw listings (`python june2017_re/charport_adis.py eve <va>`). Where the port could be wrong:
- **`329903e0`** (Atomic Shield FrameMove): the 0.8 s branch has no `SetSelfDestructSummonedNPC` and no `m_fAtomicShieldLifeTime`; the discarded first `GetSkillAbilityValue( 1 )`; `SA_DAMAGED_REL` ( 0x10 ) x 100 through `_ftol2`, replaced by `SA_DAMAGED_REL_PVP` ( 0xb6 ) when `322cc1a0`; the `CreateNPCReq` argument order ( `... iKey, team, NAT_ALLY, UID, false, team, 0` ) and the gate ( vslot 0x198 ); BFI 440 as `0 | 0 | level | 0 | 0x1b8` ( `s3e_bybfi_callers.tsv` line 152 ).
- **`3298d920`** (`June_CreateSpearBurst`): power rate from vslot 0x204, the unused level reads left out, the size ability ( 0x52 ) of `GetNowStateSkillID()` through `32007ce0` with upgrade swap, then `SetScale` ( x3 ). Landing states ( 0xcd / 0xce ) are made with `Init()`, so their `GetNowStateSkillID()` is the none id: confirm June's own extra-state data gives the same.
- **`3299a670`** (Psyker Kinesis FrameMove), **`329aea60`**, **`329cc200`**, **`32976fe0`**: the block fields ( `+4` start set, `+8` attack ball set, `+0xc` countdown from `SA_EFFECTIVE_TIME`, default 6.0, `+0x10` power rate ), the element filter ( alive, data type 1 ), `Dummy02`, the lock-on UID store ( `+0x938` ), the cool-time call order, and the two call sites ( `June_NoStateChangeActionFrameMove` case 0x40, `June_UpdateBlock9c50Timer` ).
- **`329701f0`** (Junk Break EventProcess): chain order skill / BWALK / WALK / DASH, the 15-key skip list and 5.33 s; hyper `329704f0` and Atomic Shield `32972800` = `3296f0e0` ( re-diff one ). **`32974db0`** / **`32976a10`**: walk, skill, dash order.
- **Identical-listing claims:** hyper Iron Scraps Init `32966a80` = March, Start / EventProcess / End pairs, Burst Spear hyper and Landing pairs, Tesla Shock Start `32976960` = `3296fac0` plus an unused lookup.
- **Registration** ( `329c8a00` @374 / @391 / @404 / @425 / @831 / @837 / @1000 / @1021 / @1214 / @1245 ): which states take an Init, which have no FrameMove, which hyper copies ( `329cc2c0` ) and which names the tables use.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <CH3a commit>^..HEAD`; `python tools/enumscan.py --check X2Lib/X2GuEve.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH3a - Review" into the plan doc, update the CH3a STATUS.md row, check the next kickoff (`CH3b`), and commit.
