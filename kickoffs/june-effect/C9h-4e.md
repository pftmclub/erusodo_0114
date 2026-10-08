# June effect completion - C9h-4e - the riding pet's June user / square-unit keys, then June's riding-pet scripts for March's 15 pets

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-4e` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4a, C9h-4b, C9h-4c, C9h-4d (the members it writes), C9h-3a (same StateChange; run after it, never alongside) |
| Flag | `SERV_IRUHADEV_JUNE_RIDINGPET_USER_KEYS` (+ `_DEBUG` only if you need temporary lines) |
| Log tag | `JUNE_FX RIDINGPET_USER_KEYS` |
| Review | `kickoffs/june-effect/C9h-4e-review.md` (write it when done, Opus 5.5 high) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4 - Done" "C9h-3b - Done"
grep -n "Effect C9h-4\|Effect C9h-3b" june2017_re/funcmap.tsv
grep -P "\tridingpet\t" june2017_re/c9h4_unit_key_census.tsv
```

March: `X2Lib/X2RidingPet.cpp`, i.e. StateChange's user block and C9h-3b's end-of-block comment, plus StateChangeFuture.

## Task

Replace C9h-3b's comment with June's writes (`32d2f560 @32d3110d-32d35a05`, `32d2c8b0`), in June's order. Funcmap row
`32d2f560 @32d3110d-32d35a05 (C9h-4)` lists them:
- `DISABLE_GRAVITY_TIME_START` / `_END` into March's `m_vDisableGravityInScriptTime` (a March member);
- `CAN_PUSH_UNIT_TIME%d` / `CAN_PASS_UNIT_TIME%d` in StateChangeFuture, into March's two time vectors;
- `SOUND_PLAY` through C9h-4a's reader, then `SOUND_PLAY_ALL_OPTION`, into the user's sound list (game only);
- `UPDOWN_CANCEL_AFTER` into C9h-4c's member;
- `DELETE_EFFECT_SET_OFF_RIDING` / `_ON_SKILL_STATE_END` into C9h-4d's bytes;
- **the village block `32d350e0`**: the square unit's skin anim `+0x24c` = 0, `323f1730` (C9h-4b's stopper), then the square unit's sound
  list cleared and refilled from `SOUND_PLAY_ALL_OPTION[n]`.

**Then June's scripts for March's 15 riding pets.**
- **Why.** Each June script moves the pet's summon and start sounds out of `SOUND_PLAY` into `SOUND_PLAY_ALL_OPTION`. So adopt them only
  now, when both readers exist: the in-game reader (C9h-4a plus this phase) and the village one (C9h-4b plus this phase). Earlier
  adoption silences those sounds.
- **The files.** Take June's `RidingPet_*.lua` for the 15 names in `kom_contents` from `june_src_fixed/data037_kom/`, compiled for March.
  Follow the june-assets-byte-exact rule: no hand edits.
- **Check first.** Before packing, check that every key and enum each script uses resolves in March. C9h-3a adds the two RESERVED enums;
  `DELETE_CONDITION_SOUND` is already in `Enum.lua`.
- **Pack.** Pack the archive that holds them, and **ask the user before the copy**.

**Reach.** This is the first visible change of the C9h-4 family. Each of March's 15 pets plays its summon and start sounds through
`SOUND_PLAY_ALL_OPTION`, in a dungeon and in a village.

**Play-test:**
- summon each kind of pet (Mobi, Hameling, Pporu, Oberon, Koucika, Scorpion, GodBeast) in a village and in a dungeon;
- listen for the summon and start sounds;
- the sounds must stop on a state change (`DCS_STATE_END`), and nothing may loop forever after unsummon.

**Done gate:** a `JUNE_FX RIDINGPET_USER_KEYS` line per StateChange that wrote a June key while `_DEBUG` is on; strings in the deployed
exe; `PACK OK` / `COPY OK` lines.
