# June buff port - Phase 16 step 3c - June-only extra-damage types and HMET_FEVER

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3c` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b (socket readers) and june-effect **C5** (it owns `X2DamageManager.cpp` / `X2DamageEffect.cpp`); read C6a/C6b's state for effect-set spawning |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` for enum/loader routing; one new flag per handler family only if it is logically separate (e.g. `SERV_IRUHADEV_JUNE_SOCKET_EDT`) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | write `16-step3c-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-data — Done"
```

## Task

Owner decided by the user (2026-10-06): this phase owns the handlers 16-data logs and drops.
- **18 June-only `EXTRA_DAMAGE_TYPE`s**, at June's values (`X2DamageManager.h`, under `_SOCKET_PARITY`):
  68 FIRE_BURST, 69 RAIN_OF_FIRE, 70 METEOR_STRIKE, 71 VOLCANO, 72 FIRE_FALL, 145 EJECTION_BUSTER,
  146 SUPREME_PUNISHMENT, 147 SPECTRAL_SPEAR, 149 BALLISTA_BOLT, 150 DECREASE_ENEMY_RESIST,
  151 COMMAND_HIT_EXPLOSION, 152-155 FORWARD300_{FLAME,ICE,POISON,WIND}GROUND,
  156 COMMAND_10HIT_DECREASE_SKILL_COOLTIME, 157 MAGI_HIT_COUNT_ATTACK, 159 DECENT_TO_SHADOW.
  They are on 160 June socket entries (16-data review; the Done note's 179 was an over-count). Today the loader's switch (`X2SocketItem.cpp`, the `EXTRA_DAMAGE`
  block) drops them with one log line per type.
- **`HMET_FEVER` (3)**: 1 socket (74042). Today it is an explicit no-op case in the awakening switch
  (`X2GUUser.cpp`, after `HMET_ADD_HP_IMMEDIATELY`).

**Step 1 (read-only, then stop for the user if the size calls for a split):**
1. In June (`32726850`'s extra-damage switch, then the damage-manager / game-unit readers of
   `m_MultipleSocketExtraDamage` and the single `m_SocketExtraDamage`), find where each of the 18 types is
   stored and handled. Name each handler's June address in `june2017_re/funcmap.tsv`.
2. For each type: is it self-contained C++, or does it need June data March lacks (a damage-effect
   `.lua` in `datai016_kom`, an effect set, a BFI from Phase 13)? Check against `june_src_fixed/`
   and what is staged.
3. Find June's awakening handler for `HMET_FEVER` (the switch March has at the `HMET_*` cases).
4. Count the reach: which of the 160 entries the emulator's roll pools (`SocketItemTable.lua`) or the
   save can produce; which drop/box tables hand out items that carry them.
**Already located, for 150 `DECREASE_ENEMY_RESIST` and 159 `DECENT_TO_SHADOW` (june-effect C8a, funcmap row `32045340 @32045458-32045483`).** June does not store these two in `m_MultipleSocketExtraDamage`. They take a second, June-only list, and it is carried in five places:
- the loader `32726850 @32729310` pushes them into socket option `+0x384`;
- `SumSocketOption` `3204f89e` folds that into aggregate `+0x458`;
- `InitSocketData` `3206e917` copies it to the unit DamageData's `+0x3b4` (`+0x754` on the unit);
- `CX2GUUser::SetDamageExceptionProcess` `32045340 @32045458` copies the attacker's aggregate `+0x458` into any hit DamageData whose `+0x3b4` is empty, when the hit has physic or magic damage above 0;
- `DamageReact_ExtraDamage` `3203c9a0` (tail, `LAB_3203ce96`) rolls each `+0x3b4` entry unconditionally, with `SRO + i`.

Port that list with the two handlers. C8b deliberately leaves the `32045340` copy to this phase.
Then port what is portable under the flag (June bugs verbatim, `JUNE_BUGS.md` rows), and record the rest as
"located, not portable until <data/phase>".

Play-test: a console route if one exists (an item with one of these sockets added to the save), else
name the item and where it drops.
