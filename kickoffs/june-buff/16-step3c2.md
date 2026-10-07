# June buff port - Phase 16 step 3c, part 2 - EDT 156 and HMET_FEVER

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3c2` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3c (the split was the user's, 2026-10-07: 156 and Fever come second) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_EDT` for 156 (it extends `CX2GameUnit::JuneProcessSocketExtraDamage`); Fever is its own logical change: `SERV_IRUHADEV_JUNE_SOCKET_FEVER` |
| Log tag | `[JUNE_BUFF]` |
| Review | write `16-step3c2-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-step3c — Step 1, read-only (2026-10-07, Opus 5.5 high)" "Phase 16-step3c — Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3c step 1" carry the decodes: `32034e50 (EDT cases)` 156 @32039514,
`327995c0 (EDT cases)` 156 @3279d323, `32070e40`, `32893e00`, `322d4f80`, `3206d480 @3206d6ab`, `32430b80`.

## Task

**EDT 156 `COMMAND_10HIT_DECREASE_SKILL_COOLTIME`** (user `@32039514`, NPC `@3279d323`, the same on both sides). Today it
only logs "not ported (16-step3c2)" in `JuneProcessSocketExtraDamage`. Port it there:
- Gate: the hit's `attackType == AT_COMMAND` (2), the attacker a user (`+0x264 == 0`) with a `CX2Unit` (`+0x867c`).
- Two new `CX2GUUser` members, count `+0x9474` (int) and time `+0x9478` (float), set together by `32070e40`.
- `now` = `[337676e8] +0x80` (a double): identify it (likely March's game time). If `now - time <= 1.0` (`334355b0`) nothing
  happens. Else `++count`; at 10: `32893e00( clamp( m_DamagePerSec, 0, 1 ), false )` on the attacker's user skill tree
  (`32893e00`: the five equipped slots, then the `+0x260` block; force list `+0x478` applies, else types 12/13 only with the
  bool, ignore list `+0x468` skips; left `*= 1 - rate`, then `32489230`), erase BTI 778, `32070e40( 0, now )`;
  below 10: BFI 804 at the socket level on the attacker (`vslot 0x21c`), `32070e40( count, time )`, so the time is **not**
  refreshed below 10 (check this in the disassembly: `@3203962e` pushes the old time).
- `32893e00` is new on `CX2UserSkillTree`, beside buff 4b-i's RSCT port (the same two lists). Transcribe; do not approximate it
  with `ResetLeftSkillCoolTimeByBuff`.
- `GetCalcPerDamage` already keeps 156's raw value (16-step3c).
- Dormant: March's scripts never set `AT_COMMAND` (June's character scripts do), and BFI 804 / BTI 778 are June buff data
  (Phase 13). Say so in the Done note; a play-test can show only the log line.

**`HMET_FEVER` (3)** in `CX2GUUser::ApplyBuffByUseHyperMode` (`3206d480 @3206d6ab`; today a log and no cool time):
- Gate: party manager (`g_pData +0x5c`) and `DoIHaveParty()` (`32430b80`), `g_pX2Game`, and game `+0xbd4` clear. Then
  `PartyFeverAllUser( 30.0, true )` and `SetJuneSocketCoolTime( id, entry +4 )`. A failed gate sets no cool time.
- June's `PartyFeverAllUser` (`322d4f80`) is March's (`X2Game.cpp:6104`) plus a `bool bTitle`: with it, game `+0xbd4` = 1 for
  the pass and the `Fever_Mode_Title` particle instead of `Fever_Mode`. Compare the per-unit loop with March's line by line
  (state `vslot 0x174 != 4`, HP > 0, Add `vslot 0x2a4( ?, 1.0 )` vs March's `UpNowDPValue( 3000 )`, else `vslot 0x550( time )`,
  then `SetNowMp( GetMaxMp( vslot 0x198 ) )`) and the sound's focus/my-unit order. Port every June difference.
- The other callers (`32434f60`, the party fever packet; `323c5aa8`) pass the bool: read what they pass, and give March's
  caller (`X2PartyManager.cpp:1914`) June's value.
- Dormant offline: the emulator never gives the player a party (`m_iPartyUID = 0`, `Handlers_Room.cpp:521`). Socket 74042 is a
  June title March's `TitleTable.lua` does not have.

June bugs and quirks verbatim, with `JUNE_BUGS.md` rows. Play-test: a console route if one exists, else name what would reach it.
