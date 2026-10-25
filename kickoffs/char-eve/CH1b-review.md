# June character Eve (Stage 7) - Phase CH1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1b review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1b done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1b - Done"
awk -F'\t' '$6=="CH1b"' june2017_re/char_eve/ch0_class_map.tsv      # 38 functions (3299b810 moved to CH2b by the phase)
grep -a "Char Eve CH1b" june2017_re/funcmap.tsv                      # the 38 rows the phase wrote
```

The 38 `June_*` members (and the nested `JuneEveBlock9c50`) are in the second `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp`; declarations at the end of the class in `X2Lib/X2GuEve.h`.

## Task

Review against June, function by function. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1b --out <scratchpad dir>`. You read:

- **The five helpers** (the only non-trivial bodies): `3299d140` (SetSelfDestructSummonedNPC: the six ids and what each does; NPC vslot `0xf4` read as `SetNowHp( 0 )` from funcmap rows `3202ff60` / `322824b0`; `31e4d560` = StartSelfDestruction; the HP / AI-type / ally-uid tests), `3298cfe0` (Queens Throne reset, `m_dMaxLifeTime = 30.0`), `3299bcd0` (the `+0x9c50` block: local-copy stops, fields cleared only for `true`), `329ae670`, `32997c10`.
- **Fields named by use / mapped to March:** `+0x96a8` = `m_sEGQueensThrone`, `+0x976c` = `m_hOberonGuard`, `+0x99c0` = `m_hEffectSweepParadeTriangle` (confirm the ctor `3299bd70` writes -1 there), `+0x9c08` June-only 64-bit uid, `+0x9c50` June-only pointer. CH1k's ctor / dtor map owns the final names and the allocation.
- **`ClearManeuverCore( true )`** (`329ac350`) is kept under `#ifdef ADD_EVE_SYSTEM_2014` (dead today); CH1q must make it live (see its kickoff).
- **Thunks:** `329cc050` `329cc0b0` `329cc0c0` `329cc150` `329cc160` `329cc1e0` against Eve's vtable `334cf45c` slots `0x5d4`-`0x5e8` (`ch0_vtable_all.tsv`).
- **`3299b810`** was a CH0 attribution error (the `3299d140` listing runs on into `3299d390`); its callers are CH2b and CH3b. Confirm and check the CH2b kickoff amendment.
- **Spot-check CH1a2** (CH1a review item): `CX2GUUser::June_GetSkillLevel` / `June_GetLearnedSkillAbilityValue` match `31fceab0` / `32007b10`; the Aisha and Eve copies are gone behind `#ifndef`; flag-off is today's tree (the `cl /P` probe: no `::June_GetSkillLevel` definition in the two class `.cpp`s).
- Spot-check 3 functions in Ghidra (read-only).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1b commit>^..<CH1b commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1b -- Review" into the plan doc under the Done note, update the CH1b row in `STATUS.md`, check that `kickoffs/char-eve/CH1c.md` still matches, and commit with explicit pathspecs.
