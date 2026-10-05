# June buff port - Phase 16 review - socket parity part 1, 92, designed range

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | Phase 16 steps 2-4 part 1 (commit in STATUS) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY`, 92 under `_NEW_STAT`, `SERV_IRUHADEV_JUNE_ATTACK_RANGE_DESIGN` |
| Log tag | `[JUNE_BUFF]` |
| Review | this file |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Phase 16 -- Step 1 RE" "Phase 16 -- Steps 2-4 Done, part 1"
```

## Task

0. Add the part-1 identifications to `june2017_re/funcmap.tsv` (tag "Buff Phase 16"; part 1 left them out because a
   peer had uncommitted rows there): `322a9a10` / `322a9c20` take the unit type as the template's third argument
   (refresh prologue `@32290926`..`3229094a`); the factory `32e80450` jump table `32e84808` is indexed by type - 1; BBT 78
   and 92 share the ChangeStat ctor `32e866f0`; `3236ca50` = `SetBasicStatValue`; `32001570` = `CanApplySocketDataByGameType`
   (types 2/3/5, 4, default).

Check part 1 against June, item by item, in Ghidra / `june2017_re/funcdis.py` (Ghidra's
`strings` endpoint is too slow here; use the raw-file scripts):

1. Loader `32726850`: every key part 1 added, its default and its destination (`loaderkeys`-style
   sweep: getter call, `lea reg, [base + off]`, the store after a failed read). The MP sum
   (`@32732977`, `@32732be4`), the `+0x29c` push (`@32735b36`..`32735be3`, incl. which branch sets
   the HAMEL byte), the x100 tail.
2. `ConvertAndAdd` body `3207cd70` against `X2SocketItem.h` (the plain-add list, the 17 floats,
   the spec dispatchers `32073860` / `320739e0`).
3. Seeds `32270dd0`, basics `32271540`, passive `322718e0`, refresh `32290950`: every new member,
   its basic, its refresh variant, the third template argument (unit type).
4. Template `320839e0` against `X2BuffTemplet.h` (the x87 order: is `c *= modifier` before
   `1 + c`, is the minus-1 before the passive product, the `P > 0` branches).
5. 92: factory case `@32e83509` (ctor `32e866f0`), SGU group, insert / update / erase, the roll
   `322788c0` (base, range and maximize offsets, the seed `& 0xffff`, mean / sigma, floor and cap,
   whose slot 113 is used), the call sites `@32284c1f` / `@32284c2f`, the summon copy `3230ca6d`.
6. 78's k in both DamageReacts (`@32049466`..`3204951e`, `@327b42b0`..`327b439c`): which unit each
   option is read from.
7. `CanApplySocketDataByGameType` = `32001570`; is game vslot `+0x1c8` really `GetDungeonType`?
8. The designed range (not June): users only, both `SetGameStat` overloads, one log line.

Then the reach numbers in the Done note (curve tables vs March's live bytecode, the template's
factor counts). Record transcription errors as fixes under the same flags; nothing else changes.
