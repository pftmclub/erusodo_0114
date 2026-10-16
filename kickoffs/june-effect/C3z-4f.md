# June effect completion - C3z-4f - the summoner field of June's NPC create request

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3z-4f` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3z-4 batch 1 |
| Flag | `SERV_IRUHADEV_JUNE_NPC_SUMMONER_FIELD` (a `CommonPacket.h` change: also `#define` it in `ServerDefine.h` and add it to `SECOND_HOMES`) |
| Log tag | `JUNE_FX C3z-4` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-4 - Done (batch 1)"
```

## Task

June's NPC-side create wrappers (`327790e0` ... `32779cb0`) pass the summoner's templet unit ID (`+0x85f8[0]`) as the last
argument of the create core `322e67c0`, which stores it in a `KNPCUnitReq` field March does not have (stack slot `local_47`, after
`m_iBossGroupID` and 8 more June-only bytes). Other callers pass 0. C3z-4 bound the wrappers without it (user, 2026-10-10).

1. Name the field: read June's `KNPCUnitReq` serializer (Put / Get list fields in order) and the 8 bytes before it.
2. Find its reader on the NPC-create path (June's `EGS_NPC_UNIT_CREATE_NOT` handler and what it hands the new CX2GUNPC).
3. If the consumer is one field with a small reader, port field + Put/Get + the emulator's echo + the reader behind the flag. If
   the chain is large, stop and give the user the size and the choice.
