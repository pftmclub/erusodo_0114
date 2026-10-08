# June effect completion - C9h-4e review - the riding pet's user / village keys and June's RidingPet scripts

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-4e review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4e done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS` (+ `_DEBUG`) |
| Log tag | `JUNE_FX RIDINGPET_USER_KEYS` (`python tools/logs.py --grep "JUNE_FX RIDINGPET_USER_KEYS"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4e - Done" "C9h-4 - Done"
grep -n "Effect C9h-4e\|(C9h-4e)" june2017_re/funcmap.tsv
grep -n "^| 129 \|^| 130 " JUNE_BUGS.md
```

## Task

Review the C9h-4e code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- **Order inside `32d2f560`'s user block.** Re-read the call list of `@32d3110d-@32d35a05` (`python june2017_re/disasm_range.py 32d3110d 32d35a05`, keys decoded with
  `python june2017_re/xordecrypt.py 0x32d2f560`): gravity after SPEED_TIME and before INVINCIBLE, the sound block in place of March's SOUND_PLAY loop, UPDOWN after
  DASH_CANCEL_AFTER and before `LoadEffectSetFromScript`, the two delete bytes after `_ON_DIE`. Check the miss defaults in each (gravity 0 / 0, UPDOWN `[3346ede8]`,
  the bytes 0) and the stores (`+0x84f8 / +0x84fc`, `+0x8ca4`, `+0x8628`, `+0x862b`).
- **The ALL_OPTION loop.** The user copy `@32d32853-@32d33049` and the village copy `@32d35163-@32d359f4` must match `CX2GameUnit::JuneLoadSoundPlayAllOption` field by field
  (the port took the defaults from both copies; confirm the DELETE_CONDITION `erase( all )` at `@32d35951` and `@32d33...`).
- **The village block `@32d350e0`.** The port resets the **pet's** `m_pXSkinAnim` oneshot timer (`[ebp+0x238] -> +0x24c`), where the kickoff and funcmap row 1968 said the
  square unit's anim. Confirm `ebp` is the pet there (`[ebp+0x43c]` is the owner uid) and `+0x238` is the pet's anim (`32d286c0`). Confirm `[33775870]` is the village field
  game that `323a8dd0` is a method of, and that nothing in the block runs with a game.
- **JUNE_BUGS 129** (the square's StateStart wipes the refilled list): re-check that all 8 callers of `32d3cb10` in `323fad80` / `323fec40` are followed by the clear, and
  look for any pet StateChange in a village that is not (the four other callers `3249d150`, `324bd250`, `324d1de0`, `32a08150` were read as game states: confirm).
- **Future:** `32d3e2f0` / `32d3e330` clear the vectors at `+0x8b14` / `+0x8b24` (Future) and the loops are 0-based with defaults true / false.
- **Data:** the 14 pets' compiled `.lua` match June's original bytecode in size and string set (the Done note says so); re-run that comparison on the live `data037.kom`
  (`python tools/junekom.py extract --names ...`; the live kom is `python tools/kom.py status data037`). `RidingPet_GodBeast_Fire` was held back (15 missing assets): check
  the asset port has staged `Motion_Riding_GodBeast_Fire.x`, `Riding_GodBeast_Fire.tga` and the 13 `RidingPet_Godbeast_Fire_*.ogg`; if it has, adopt that script too.
- **Hygiene:** `python tools/flagcheck.py JUNE_RIDINGPET_USER_KEYS --with JUNE_RIDINGPET_USER_KEYS_DEBUG --range <first C9h-4e commit>^..HEAD`.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Turn `_DEBUG` off in the review commit. Write
"C9h-4e - Review" into the plan doc, update the phase's STATUS.md row, and commit.
