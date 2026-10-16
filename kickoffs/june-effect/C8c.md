# June effect completion - C8 step 3 - port the per-class SetDamageExceptionProcess overrides

> **Handed off (D8, 2026-10-06):** not run in the effect plan. Kept as the brief for the character ports (`JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7b); each port takes its own class's blocks. Under D8 (a), March's Bowstring, Sharp Kick and Fitness blocks are replaced by June's only in the Rena/Aisha ports.

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C8c` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C8b (the `0x780` hook, the layers); **C5** (DamageData `ATTACK_TRIGGER_TYPE` `+0x53c` and the `ADDITIONAL_ATTACK_TRIGGER_TYPE` reader `32610fb0`); **D8** answered (decides the Rena/Aisha re-keyed blocks) |
| Flag | `SERV_IRUHADEV_JUNE_DAMAGE_EXCEPTION` (C8b's) |
| Log tag | `[JUNE_FX]` |
| Review | `kickoffs/june-effect/C8c-review.md` |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C8a - Done"
python tools/docsection.py JUNE2017_EFFECT_REBUILD_PLAN.md "Starting context"
```

Also read:
- the funcmap rows tagged `Effect C8a` (each class's row lists its blocks, gate skill IDs and helper addresses);
- the D8 row in the plan's decision table;
- `JUNE2017_CHARACTER_PORT_CHECKLIST.md` section 7b.

Edit CP949 files with `tools/bytepatch.py` only. Build with `tools/build.py --deploy`. Never launch X2_offline.exe: ask the user.

## Task

Port the C8a note's "C8c port list" verbatim, one block at a time, reading each block's June disassembly (`june2017_re/disasm_range.py`; Ghidra for the ones it carved) and citing its address range:
- Eve `329bc560`;
- Chung `32bf28f0`;
- Raven `32c5c080`;
- Elsword `32d1e0e0`;
- Elesis `@32b81008-32b811ef`;
- Rena's additions;
- Aisha's Cyclone (22011/22051);
- Ara's vslot `0x780` `32bca010`.

The D8 blocks follow the user's answer:
- under (a), leave March's Bowstring, Sharp Kick and Fitness blocks untouched and port nothing for them;
- under (b), port June's versions with March's in the `#else`.

**How to treat the gates:**
- Every gate is a June reform skill ID (`RNSI_*`) that March's `SKILL_ID` enum lacks. Use the numeric ID with a comment naming the June enum entry, and do not add enum entries.
- These blocks are dormant on March data. Say so in the Done note: the hand-off is a regression check (each class still hits as before), not a feature test.
- Decode `32d40dd0` (called by `32610e90`, the Chung block) before porting that block.
- Add and Elesis's Way of the Sword are identical to March: nothing to do.

Update MODS.md's `_DAMAGE_EXCEPTION` entry with the per-class list.
