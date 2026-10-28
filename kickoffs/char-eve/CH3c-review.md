# June character Eve (Stage 7) - Phase CH3c review - Skills: Code: Battle Seraph (116)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH3c review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH3c done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (no debug flag in CH3c) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH3c - Done (2026-10-28, Sonnet 5.5)"
```
Then the `Char Eve CH3c` rows at the end of `june2017_re/funcmap.tsv` and `june2017_re/char_eve/ch0_skill.tsv` rows for 25026-25029, 25047, 25048, 25211, 25212, 25223, 25237.

## Task

Review the CH3c port against the raw listings (`python june2017_re/charport_adis.py eve --phase CH3c --out <dir>`, plus `329c8a00`). Where it could be wrong:
- **Registration.** `329c8a00` cases for the 16 ids (and the memo ids 25034 / 25071, 25035 / 25036 / 25072 / 25073 repointed to June's bodies): which callbacks each normal / hyper state gets, the table names (`Eve_Nasod.lua`), Electronic Move's three extra states (239-241 copy 219-221, `UpdateUserUnitStateData` as March's inline `MakeTableReference` + `m_StateList`), the hyper states that register fewer callbacks (Energy Needles / Energetic Heart / Linear Divider / Tesla Shock / Thousand Stars).
- **Thousand Stars.** The memo switch on `GetJuneTranscendenceMemoSkillID` ( jump table `329a7210` / `329a721c`: 25035 and 25072 -> 1, 25036 and 25073 -> 2 ), the create `329a2b70` ( count 120 / 100 at `@329a2f81` ), `UpdateSkillSlotUI( false )`.
- **Energy Needles `329968a0`.** The two templet fetches ( `31fdbf00` is the memo templet, null falls back to the equipped one ), the order of the unit test ( `m_bEnable` is set before it ), `- 0.33`, `x ( 3201ad10 + 1.0 )`, `SetLockOn( effect, 0, 8, 30 )`.
- **Energetic Heart `329b4f10`.** The vslot 0x770 call ( skill id 25029 ), `ApplyExternalFactorToFinalizer` arguments, the formation particle.
- **Taser Pilum.** Start `32973f10` ( rank table, tiny data store ) and FrameMove `329a7f70`: the 0.13 gate, the "larger of the two" power rate, the pierce count, the Open Mind return ( `31fceab0( 25419 )`, `32007b10( 0x42 )`, x 0 / 1 / 3 ), the MP rate `32039a50( 25212, 63 )`, the three alpha tags, the not-enough-MP effect. Check `m_dJuneElapsedTime` ( `TASER_PILUM_TinyData`, June +0x9a98, zero at construction and in `reset()` ) against the constructor `3299c373`.
- **Linear Divider `329aafc0` / `3297eae0`.** Giga plane arguments, the hyper laser widths ( 0.7 / 0.8 ), the reverse y flip, the cancel-window order.
- **Tesla Shock ( Battle Seraph ) and `329809c0`.** The `else if` chain of `329ae0c0`, the effect-set arguments ( `Bip01`, rotate twice ), the handle stored at data +0x8c, the End = Iron Scraps' ( `32968890` ).
- **Extreme Virtual `3299a9d0`.** The formation jump table `3299ad48` ( 0 Red, 1 Blue, 2 Green, 3 Mix ), modifier tags 16, 12, 20 and their conditions ( BTI 672, formation != 4, ability 0x33 of 25411 ).
- **Kept as March:** the 18 "= March" functions in the funcmap rows ( Init / Start / End / EventProcess / Future ).
- **Hand-off check:** the save unit `EveSeraph` ( 60, transcended, loadout in the Done note ) and the log lines the hand-off lists.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <first CH3c commit>^..HEAD`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions to the user. Write the next phase's kickoff if it is missing (`CH3d.md` exists).
