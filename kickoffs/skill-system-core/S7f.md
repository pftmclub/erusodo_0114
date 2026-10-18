# June skill system core - Phase S7f - Memo resync in other ACKs, the item tooltip's skill lists

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7f` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7c (the memo packets, the unit-info merge); not alongside S7d (both touch the slot / page ACKs) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_SKILL_*` (a packet-struct change also goes in `ServerDefine.h` + `SECOND_HOMES`) |
| Log tag | `[JUNE_SKILL] MEMO` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7f-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7c - Done (2026-10-18, Opus 5.5 high)" "Phase S5 -- Done (2026-10-18, Sonnet 5.5 high)"
```

## Task

Added by S7c (2026-10-18, unowned-branch rule). Two June paths S7c found and did not port.

1. **The memo map in other ACKs.** June's merge `32892410` has four more callers besides the two unit-info appliers S7c ported (`3235eb20` /
   `32360090`): the slot-change ACK `31ed4c80` (Clear `32892050`, then Merge from ACK `+0xe9`; the ACK carries a whole KUnitSkillData at
   `+0x41`, whose `+0x76` vector also goes to unit data `+0x8dc` through `32853ff0`), the 0x47b ACK `3220f2b0` (@3220f416), `31f276b0`
   (@31f2791a) and `3257b610` (@3257b8d9). Identify each packet, compare with March's struct (S5 ported the slot ACK without the
   KUnitSkillData), and port the June layout and the merge where March has the packet. Offline the client's list already matches the
   save after every memo ACK, so the play-test is "nothing changes": the memos survive a slot change, a page change and a job change.
2. **The item tooltip's skill lists.** `3220d2b0` (caller `31d885e0`, item description) builds per-type skill lists through `3220b990`:
   type 1 / 3 for skills whose first related id is the item's value, 4 for a later index, 2 for memo rows ( `32451250` ). Identify the
   filter struct and the description code that prints the lists, then port it.
3. Also identify the unit-data `+0x8b4` / `+0x8dc` copy ( `32853ff0` from KUnitSkillData `+0x76`, the slot March fills with
   `m_vecSkillNote` ) and say whether anything reads it.
