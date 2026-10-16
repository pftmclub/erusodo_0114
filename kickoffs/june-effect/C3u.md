# June effect completion - C3u - June's summon-NPC socket gate in SumSocketOption (port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3u` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3r (found there). Not with a phase editing `CX2GUUser::SumSocketOption` |
| Flag | `SERV_IRUHADEV_JUNE_SUMMON_NPC_SOCKET_GATE` |
| Log tag | `JUNE_FX C3u` |
| Review | none unless the data shows a reachable socket |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3r - Done" "C3u - port: June's summon-NPC socket gate"
grep -n "3204f190" june2017_re/funcmap.tsv
```

## Task

`CX2GUUser::SumSocketOption` (`3204f190 @3204fb89-3204fc1d`; EBX = the socket templet, EDI = the user) is March's summon-NPC socket
block (`X2GUUser.cpp` SumSocketOption, `m_iSummonNpcId` `+0x240` > 0 and `m_fSummonNpcRate` `+0x244` > 0 -> `SummonNpcSocket`
{ id, rate, `m_fSummonNpcCoolTime` `+0x248` } pushed onto `m_vecSummonNpcSocket` `+0x8d80` via `3210aae0`), with one June addition:
NPC ids `0x1017` and `0x1018` (4119 / 4120) are pushed only when `g_pX2Game` vslot `0x10c` (GetGameType) returns 2 (`GT_DUNGEON`).
Confirm the rate test (`fldz; fcomp +0x244; test ah, 5; jp` = rate > 0), resolve the two NPC names from the live enum (`cl /P`, or cite the
bare ids; never guess), check which socket templets carry them (June `SocketItem` data), and port the gate at June's position with
March's push left in `#else`. If no socket in March's or June's data names those ids, say so and the port is dormant.
