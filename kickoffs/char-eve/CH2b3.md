# June character Eve (Stage 7) - Phase CH2b3 - Three unbound NPC Lua methods (Oberon's START_STATE_END)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2b3` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2b play-test 2 (done). Not beside another `X2GUNPC.*` / `X2Game.*` phase; read `JUNE_STAT_CALC` plan rules first (stat math is whole-or-nothing) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (stay inside it) or a new `SERV_IRUHADEV_JUNE_NPC_LUA_*` if the stat part is shared |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`) |
| Review | `kickoffs/char-eve/CH2b3-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH2b3 - Three unbound NPC Lua methods"
python tools/luabind.py check SI_SA_AT_SURFACE_CUTING Nasod_Watch_Eve SI_SA_EEP_ELECTRONIC_FIELD
```

## Task

`OBERON_START_STATE_END` (June script `SI_SA_AT_SURFACE_CUTING`, line 589) calls three `CX2GUNPC` methods March never bound. The first aborts the function, so the
third line, `SetFlag_LUA( 0, true )`, never runs (seen in play-test 2: ErrorLog "can't find 'ApplyPassiveSkillOwnerUser_LUA' class variable"). The same calls sit in
`Nasod_Watch_Eve` and `SI_SA_EEP_ELECTRONIC_FIELD`, and across June's NPC scripts `ResetMultiplyPowerRate_LUA` is used by 7, `SetMultiplyPowerRate_LUA` by 5 and
`ApplyPassiveSkillOwnerUser_LUA` by 3 (`python tools/luabind.py check --dir june_src_fixed/data037_kom`). Port all three whole, together:
- **`31e8d6a0` `ResetMultiplyPowerRate_LUA`** (binder site `31e0a802`): the protected float at NPC `+0x8ce4` (XOR-folded, `KProtectedType`; 1.0 at the NPC ctor `327b0263`) is written 1.0, skipped if already equal.
- **`31e8d610` `SetMultiplyPowerRate_LUA`** (site `31e0aa83` / `31e0abe3`): the same store with the float argument.
- **`32759630` `GetPowerRate`** (vslot 0x204) = `+0x8ce0` x that protected value; March's `CX2GUNPC::GetPowerRate` has no second factor. Port the reader with the writers or the setter does nothing. Writers of `+0x8ce0`: the summon helper `322ece70` (funcmap rows 1449 / 1458 / 1503; `JuneStoreSummonGroupPowerRateNumber` in `X2Game.cpp` is the dormant `+0x8800` store that feeds `SetMultiplyPowerRate_LUA( GetNumber_LUA() )`).
- **`32763c10` `ApplyPassiveSkillOwnerUser_LUA`** (site `31e0a533` / `31e0a70d`): reads the owner unit through the NPC's `+0x8d5c`, scales by the owner's `+0x3ec8` / `+0x3ecc` / `+0x3ec4`, fills a stat block with `32758d50`, applies it with `32763950`, then refreshes three protected owner blocks with `3236ca50` (`+0x6c18`, `+0x43ec`, `+0x44a0`). This is the tail of `CX2Game::SetUserSummonedNPCInfo` (funcmap 935). It is stat math: port the whole call tree (no partial update) and check it against the Stage 33 stat-calc plan (`june-stat-calc-plan`: every reader / refresher of a stat together).

Done gate: `python tools/luabind.py check SI_SA_AT_SURFACE_CUTING Nasod_Watch_Eve SI_SA_EEP_ELECTRONIC_FIELD` PASSES; funcmap rows for every June function above; ErrorLog has no "can't find" for the three; in game Oberon's START_STATE_END completes (add one `[JUNE_CHAR]` line from `SetFlag_LUA(0,true)`'s effect, or log the new factor on the Oberon). If the call tree turns out larger than one phase, stop and hand it to the stat-calc plan with the evidence above, and put the three names in `tools/data/lua_unbound_known.tsv` with that reason.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.
