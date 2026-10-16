# June buff port - Phase 16 step 3b review (items 7 and 8)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | 16-step3b items 7-8 done (its Done note and commits) and their play-test |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (`_DEBUG` for the first-apply lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16 -- Review" "Phase 16-step3b -- Done, items 7-8"
```

## Task

Review the item 7 and item 8 commits against the June binary (Ghidra, read-only) and June's
`PVPData.src.lua` / `PVEData.src.lua` / `Enum.src.lua` (`june_src_fixed/data036_kom/`). What they could
get wrong:

1. **Item 7, skill-use reader** (`CX2GUUser::CommonStateStart`, June `@32060bda..32060ccb`): the gate
   order (slot 329 != 0, then slot 330( false ) == 1, then the skill tree), the category function
   `32070510` (5->6, 6->1, 7/14->2, 13->5, 15->3, 16->4, else 0), and that June has no IsMyUnit test.
   `IsJuneSkillSlotState` stands for June's state byte `+0x11`: re-check one or two of the 12 class
   slot-493 functions (`funcmap.tsv` row `3208e430`) and that no March class enters a skill through a
   state that is not one of the four `_CONST_GUUSER_` arrays.
2. **Item 7, kill hook** (`CX2Game::ApplyBuffFactorOnNpcKill`, June `322d6f20`): GetMyUnit, not the
   killer; the `m_uiAttUnit != -1` gate (`@3230bd1b`); the 0x287 -> 0x286 loop over NPCs with the AI
   type test (`+0x7c98 -> +0x38`, read as `m_eAIType`; confirm the offset). Dormant by design: its input
   `3277cf10` is server-sent stage data (decision for the user in the Done note).
3. **Item 8, scaled copy** (`SocketData::SetJuneRankLinkedScaled`, June `320925f0`): the 34 fields and
   the stat block, nothing else (JUNE_BUGS #50). The two lookups `31edfe70` / `31edfeb0` (the MP flag,
   JUNE_BUGS #49), the rank key (`+0x114` = `m_cRank`, sign-extended), and the three insert bindings.
4. **Item 8, data.** Re-run the stub-`g_pData` load of the live `data036.kom`'s `Enum.lua`, `PVPData.lua`
   and `PVEData.lua` (`elstools extract` to scratch): PVP 11 rows, PVE 10, PVE-for-MP 10, and only the
   7 June-only calls the Done note lists. The Enum diff against the previous live one: `PVPRANK_RANK_STAR`
   and `PVE_RANK` added, nothing else.

Hygiene is one command:
`python tools/flagcheck.py JUNE_SOCKET_PARITY --range 7199cd5^..HEAD --with JUNE_BUFF_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line (the Done note names them); a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "Phase 16-step3b -- Review" into the plan doc, update the STATUS.md
row, check `kickoffs/june-buff/16-step3b2.md`, and commit.
