# June character Eve (Stage 7) - Phase CH2a2 review - Skills: Exotic Gear (18)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2a2 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2a2 done (its Done note and commit) and the user's play-test |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL registered ...`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2a2 - Done (2026-10-27, Sonnet 5.5)"
```
Then the `CH2a2` rows of `june2017_re/funcmap.tsv` (`grep "Char Eve CH2a2"`) and JUNE_BUGS 447 / 448.

## Task

Review CH2a2 against June `329c8a00` (cases 25003 / 25037 `@329c8f76`, 25004 / 25040 `@329c8efd`, 25005 / 25049 `@329c95d6`, 25006 / 25050 `@329c8eb0`, 25007 / 25051 `@329c8e11`, 25204 / 25218 `@329c9407`; the table names are read from the strings, the case labels from the jump table) and the bodies `3299d3f0` `3298f220` `3298f8f0` `3296fda0` `3296ff50` `3297b840` `3297baa0` plus the four Init lists `32966450` `329667e0` `32966820` `32966890`. Things this phase could get wrong:
- **Registration** (`ch0_state_skill.tsv` columns, listing `@329c8a00`): which slot of the state record each callback goes into (`+0x18` Init, `+0x28` Start, `+0x38` FrameMove, `+0x48` EventProcess, `+0x58` End), and which hyper copies exist: Black Hole and Hornet Sting hyper = EventProcess only, Explosion Impact hyper = FrameMove forwarder + EventProcess (no Start / End), Queen's Throne hyper = the normal table name with Start / End / FrameMove / EventProcess copied, Metal Dust and Atomic Blaster hyper = their own Init / Start / End.
- **The "= March" claims** (funcmap row `3296efa0 / ...`, 24 callbacks): read each pair in the listing, not the claim; the EventProcess pairs include the BALANCE_CODE_NEMESIS_20121213 cancel chain of Atomic Blaster (attack, skill, back-walk, walk, dash) and Metal Dust's 14-key skip at 0.66 s.
- `3299d3f0`: the five timers 0.06 / 0.5 (empty) / 1.3 / 1.4 / 1.9, the particle names and offsets (x - 20, x - 15 / y + 155), `UpDownCrashCameraNoReset( 20, 0.3 )`, the 1.9 s block order (wing destroy, bone, `3298cfe0` reset, enable, memo, four weapons, booster handle, timer, `June_SetSkillBuffFactors( 25416, 0 / 1, true )`), and the 13 alpha tags in the right places.
- `3298f220` / `3298f8f0`: the +-70 along `GetDirVector` (sign per `IsRight`), the 0.89 s dummy gate order, the force down (`SA_FORCE_DOWN_ABS`, `max( 1, level )`, effect `+0x5f0` = DamageData `+0x3f0`) and that only the normal body walks the NEXT tables (JUNE_BUGS 448).
- `3297b840`: gate order, table index `m_iSkillCutInSetIndex` x 3 + index, and that the two left-out blocks (item override -> Stage 12, event 0x8b3 pair -> CH5) are the only differences from March.
- Play-test: Eve (class 18 / 108 / 109 / 116 with these skills) casts each of the six skills in an A and a B slot; no Motion Error / KEM line; Queen's Throne spawns its four weapons at 1.9 s.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <CH2a2 commit>^..HEAD`; `python tools/enumscan.py --check X2Lib/X2GuEve.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH2a2 - Review" into the plan doc, update the CH2a2 STATUS.md row, check the next kickoff (`CH2a3`, then `CH2b0`).
