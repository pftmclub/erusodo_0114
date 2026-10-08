# June effect completion - C3e - identify June's user-sync packet extension (Opus, no code)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3e` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | - (found by C3d-1). Run it after C3d-2 if it proposes a port: same `X2UDPPacket.h/.cpp` |
| Flag | none (no code); a port phase, if any, gets its own flag |
| Log tag | - |
| Review | none (its port, if any, is reviewed) |

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3d-1 - Done (2026-10-08, Opus 5.5 high; no code, no flag, no build)" "C3e - identify: June's user-sync packet extension"
grep -n "^32d5be30" june2017_re/funcmap.tsv
sed -n 1p june2017_re/c3d1_state_id_sites.tsv; grep -n "^11	" june2017_re/c3d1_state_id_sites.tsv
```

Ghidra is read-only (`ghidra` skill). Verdicts go to `june2017_re/funcmap.tsv`, tagged `Effect C3e`.

## Task

June's `KDYNAMIC_UNIT_USER_SYNC::Deserialize` (`32d5be30`) reads fields that March's packet (`X2Lib/X2UDPPacket.h:240-330`) lacks. Buff
Phase 8 already ported bit `0x800000`; C3d-2 widens bits 1 and 2. The rest are not ported:

- bit `0x400000`: 1 byte at packet `+9`;
- bits `0x10000` - `0x200000` and `0x1000000` - `0x40000000`: 14 fields at packet `+0x1c` .. `+0x75`.

1. **Each field.** Record:
   - its width;
   - the June `SyncData` member it is filled from: find June's sender, the counterpart of March `X2GUUser.cpp:24051`;
   - the member it decodes into: June's `SyncData::SetData`, the counterpart of `X2GUUser.cpp:35877`;
   - who reads that member, and what for.
2. **Reach.** March sends the packet only when there are UDP peers (`X2GUUser.cpp:24025`), and the offline client has none. Say which
   fields would still change local behaviour through the `SyncData` member itself (written and read in-process), which ones only matter on
   the wire, and which belong to a June system March lacks.
3. **Plan.** Follow June: for each field with a March counterpart, propose a port phase (row, flag, kickoff). Fields that belong to an
   absent system go to the system's owner (checklist or backlog), with no question. Write a Done note and update the STATUS row.
