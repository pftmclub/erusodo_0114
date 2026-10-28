# June character Eve (Stage 7) - Phase CH3b review - Skills: Code: Empress (109)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH3b review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH3b done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`, `SERV_IRUHADEV_JUNE_EFFECTSET_POSITION_OFFSET` (no debug flag in CH3b) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH3b - Done (2026-10-28, Sonnet 5.5)"
```
Then the `Char Eve CH3b` rows at the end of `june2017_re/funcmap.tsv` and `june2017_re/char_eve/ch0_skill.tsv` rows for 25017-25020, 25045, 25046, 25208, 25209, 25222, 25236.

## Task

Review the CH3b port against the raw listings (`python june2017_re/charport_adis.py eve --phase CH3b --out <dir>`, plus `3299ba70`). Where it could be wrong:
- **Registration.** `329c8a00` cases for the 16 ids: which callbacks each normal / hyper state gets (`ch0_skill.tsv` columns `normal_callbacks` / `hyper_callbacks`), the table names (`Eve_Nasod.lua`), the hyper states that register fewer callbacks than the normal one (Genocide Ripper, Assault Spear, Space Wrench, Electronic Field, Spit Fire, Tesla Shock), and the extra state 0xf2 of Reason of Pride.
- **Effect-set calls.** Genocide Ripper / Space Wrench / Assault Spear play their set with `bCustomPosition` true and `GetVec3ScaleByUnit()` (vslot 0x24); the handles go to `m_aiJune9c30[2]` / `[3]` / `[4]` (+0x9c38 / +0x9c3c / +0x9c40, normal states only for Genocide and Assault Spear; Space Wrench's two bodies both store). Spit Fire: the hyper variant passes `bHyper` true, the handle joins `m_vecEffectSetToDeleteOnDamageReact` (+0x3eb0), the size row is modifier tag 15 `( s, s, s )` not add, and the Y row (`SA_DEGREE_Y`, 0xc6) goes to the instance offset `( 0, v, 0 )`.
- **Instance position offset.** The engine edit: every `SetEffectSetOffset` caller passes the sum of the instance offset (+0x1a4) and the effect's own offset (scan the six callers of `321d9820`), and `329cc290` writes only the three floats.
- **Electronic Field summon (`3299ba70`).** BFI 442 / BTI 413, the time finalizer (`+0x9c4c` x 60 + `+0x4070`, x `SA_DECREASE_REL` in PvP / PvE), `CreateNPCReq` arguments (the key code is `hyper ? 1 : -1` from `3299ecb0`, 2 from `329ae4f0`), and that it is the twin of `3299b810`.
- **Tesla Shock / Tesla Flare maps.** `m_mapJuneNpcEffectSet` is cleared by Tesla Shock only, filled by both with `insert` (no overwrite), the handle also joins the state-end delete list (+0x3ec0); `3299b000` then moves the mesh.
- **Reason of Pride.** `EventTimerOneshot( USEFUL_VALUE0[0] Now )`, the two line-map positions (150 ahead for Surface Cuting, 150 behind for Electronic Field, key code 2 for both), BTI 961 erased, `+0x9c54` cleared.
- **Key-cancel EventProcesses.** The 15-key cluster and the three times (1.33333, 0.666667, 3.0) against the listings.
- **Unported on purpose:** Tesla Shock's dropped level read; the Tesla Shock `Init` is the Nemesis one (same bytes); Space Wrench / Genocide Ripper Start, End and EventProcess are March's (byte-equal in the listings).
- **Hand-off check:** the save unit `EveEmpress` and the log lines the hand-off lists.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <first CH3b commit>^..HEAD` and `... JUNE_EFFECTSET_POSITION_OFFSET ...`. Paste their `files:` / `in this flag's files:` / `RESULT:` lines. Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH3b - Review" into the plan doc, update the STATUS.md row, check the CH3c kickoff, and commit.
