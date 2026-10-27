# June character Eve (Stage 7) - Phase CH1r review - June's LoadEffectSetFromScript list variants

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1r review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH1r done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_EFFECT_SET_LIST_VARIANTS` (engine, `CX2GUUser`; every class) |
| Log tag | `[JUNE_CHAR]` (`EVE CORE`); the phase itself writes no log line |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1r - Done (2026-10-27)"
```
Then the `(CH1r)` / `32020ec0` / `3200b030` / `32007e90` rows of `june2017_re/funcmap.tsv` and checklist l.411-412.

## Task

Review CH1r's commit against June `32024ff0` (list part, up to `@320268e0`), `32020ec0`, `3200b030`, `32007570 @320075e5-32007662` and `32007e90`. Things this phase could get wrong:
- the branch flags: `ba` / `b9` ( `[esp+0x32]` / `[esp+0x33]` ), the ULTIMATE_EX_APPLIED_SKILL found-but-not-learned path falling to ULTIMATE_EX, EX_APPLIED_SKILL skipped only by `b9`, and the EX gate ( memo byte: `!a9`; else `!ba && !b9`, `@32026685-320266ca` );
- the TRANS pick: memo row type 19 / 20 = `MI_TRANSCENDENCE_1` / `_2`, of `GetNowStateSkillID()`, read for every state;
- `POSSIBLE_NASOD_BARRIER` read from `m_LuaManager` ( this `+0x2b4` ) and stored only on EX rows ( `+0x34` );
- `32020ec0`: the `RATE` roll as float ( `GetRandomFloat( 0 ) x 100.0` ), `sum >= roll` picks the first index, -1 when none; returns false only when the list table is absent;
- `3200b030`: `IS_LEARN_SKILL_CHECK` drops the row with no unit or level < 1 ( raw `GetSkillLevel( id, 0, 0, 1 )`, not `31fceab0` ), defaults ( START_TIME 0, CONSUME_MP -1, RELATE_SKILL_ID 0 ), one row when the index is >= 0;
- the play gate: `0 < CONSUME_MP` ( NaN plays ), `IsEnoughMPForAttack( mp, GetBonePos( L"Bip01" ), 0, 0-vector )`, a failed row erased unplayed; and `IsEnoughMPForAttack`'s -100 facing left;
- the `#else` is March's list code byte for byte, and the delete-key tail stays shared.

Hygiene: `python tools/flagcheck.py JUNE_EFFECT_SET_LIST_VARIANTS --range <CH1r commit>^..<CH1r commit>`; `enumscan --check X2Lib/X2GUUser.cpp`; `/Zs` of `X2GUUser.cpp` and `X2GUNPC.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1r - Review" into the plan doc, update the CH1r STATUS.md row, check `kickoffs/char-eve/CH1s.md` still matches, and commit.
