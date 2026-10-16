# June buff port - Phase 11 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 11 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 11 done (its Done note and commits, see STATUS.md) |
| Flag | `SERV_IRUHADEV_JUNE_BUFF_PREMIUM` (client), `SERV_IRUHADEV_OFFLINE_ITEM_BUFF` (emulator), `SERV_IRUHADEV_JUNE_BUFF_NOT_USE_IN` (stand-in), `SERV_IRUHADEV_OFFLINE_VILLAGE_BUFF` (village Rest buff 122, user request) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`); emulator lines are `BUFF     ` in `offline_server.log` (`--grep "BUFF     "`) |
| Review | none |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 11 — RE (2026-10-05)" "Phase 11 — Done (2026-10-05)"
python tools/docsection.py JUNE_BUFF_PORT_ARCHIVE.md "Phase 11 — the March server's buff system (2026-10-05)"
```

The "Buff Phase 11" rows of `june2017_re/funcmap.tsv` hold the RE. `JUNE_BUGS.md` #27-#29 are 11's.

## Task

Review Phase 11 against June (client) and against the March GameServer (emulator). Check these first:

1. **The two handlers** (`326c9030`, `326c9530`): every skip, its condition and its game type
   (`BFI_EMPTY_EXP_BUFF` in-room `== 3 || == 4`, village always; `BFI_BUFF_2013_DEFENSE_BUFF`
   in PvP; `BFI_SPA_STRONG_BUFF` in `XS_PVP_LOBBY`), the record fields (BTI and count from the
   packet, `0 < end - start <= 3600`), the apply count, the keep rule (applied, `GetNowHp() <= 0`,
   or `GetNowStateID() == GUSI_DIE`), the in-room gage erase with `false`, the village erase with
   `IsDeBuff`, and the order of the BTI-138 toggle against the factor/clone checks. Re-read the
   assembly, not the decompile: the 154/195 branch structure in the decompile is misleading.
2. **What 11 recorded instead of porting**: BFI 500 (comeback end time at `this+0x60`, "gone"
   UI `0x2a` + `this+0x68`; June's comeback handler `326c7a10` is rewritten), BFI 10060's in-room
   emblem 34, BTI 1033 in `XS_GUILD_AGIT_GAME`, the PC-room remaining-time warning (`326c5cf0`,
   `g_pData+0x334`), and the icon stack count (`32489810`). Is there a March home 11 missed?
3. **`32022380` (ReInit)**, **`323644a0`** (`IsPve` early-out; the always-walk) and the village
   `3235a080( 0 )` -> `ResetIncreaseSkillLevelBySocket()` mapping (candidate: same skill walk;
   June passes an argument March has no parameter for).
4. **The emulator against the March server**, citations in the archive section: the item ->
   BTI table and both deactivation groups, the pre-checks (117 lockout unconditional; the two
   temp-code guards), `ActivateBuff`'s "already active = refuse", the end-time rule (period 0 =
   start), `CheckBuffPeriod` (strictly past; period-0 never), the room filter (empty list = never),
   and the trigger points (select unit, field change only-if-any, loading 100, leave room, dungeon
   end, death, the 3-second expiry tick standing in for the 1-second one). Two deliberate
   departures: activation refuses a factor the client has not loaded or that names another BTI;
   Phase 19's `USE_QUICK_SLOT_NOT` is skipped for a server buff item.
5. **The Lua binding hand-back**: `BuffManager` is re-declared to the client's manager after the
   emulator's run; confirm the client's own `OpenScriptFile` count lines are unchanged by it.
6. **The village Rest buff (BTI 122):** granted at `EGS_FIELD_LOADING_COMPLETE_REQ`, removed in `OpenRoom`, at back-to-select and at select; compare with `ERM_JOIN_FIELD_ACK`, `OnLeaveField` and `CheckDeselectUnitBuff` (archive section).
7. Confirm `BuffTemplet.lua` / `_INT` staged in both places are the mar-2014 compile of
   `june_src_fixed` (round-trip count 138 / 71) and the live `.kom` sizes once packed.

Hygiene, one command per flag with `--range <first Phase 11 code commit>^..<last>` and
`--with JUNE_BUFF_PREMIUM OFFLINE_ITEM_BUFF JUNE_BUFF_NOT_USE_IN OFFLINE_VILLAGE_BUFF OFFLINE JUNE_BUFF_DEBUG`.
Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and
every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "Phase 11 — Review" into the plan doc, update the 11 review
row in STATUS.md, check `kickoffs/june-buff/11b.md`, and commit.
