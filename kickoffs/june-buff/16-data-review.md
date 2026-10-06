# June buff port - Phase 16 data step review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-data review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-data done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16 -- Review" "Phase 16-data -- Done"
```

## Task

Review 16-data's commit against June's `Enum.src.lua`, `OptionItemData.src.lua` (both in
`june_src_fixed/data036_kom/`) and the June binary. What this step could get wrong:

1. **Lua Enum values.** Every added `SOCKET_DATA_TYPE` (273), SIEE (12-20), `HMET_FEVER` and EDT (18)
   member equals June's value; `HMET_END` is 4. Nothing March already had changed value (the
   Done note's global dump: one changed line, 301 added). The 45 June-renumbered `EDT_ENCHANT_*`
   values and June's `EDT_BEDCLOTHES_FEATHER = 66` were left out on purpose: confirm no June data
   file this port stages names them.
2. **C++ enumerators.** `X2SocketItem.h` (SDT, SIEE, HMET) and `X2DamageManager.h` (EDT) carry the same
   explicit values as the Lua. `HMET_END` moves only under the flag; check nothing sizes an array
   by it.
3. **The gate `31fe4730`.** June files SIEE 1-20 (`dec`; `cmp eax, 0x13`; `ja`) and its caller
   `3204f190` skips the rest of the socket (`@3204f5f6`). The port's new `case` labels in
   `InitSocketEffectEx` must give exactly that, and nothing else in March may iterate
   `m_mapSocketItemEffectEx` and trip on the new keys.
4. **Custom-use routing** (`32726850 @3273661d..32736630`): HMET 0, else SIEE in {11, 13, 17}.
5. **The bytecode.** Re-run the equivalence check (the KOG LuaJIT loads the compiled `Enum.lua` +
   `OptionItemData.lua` under a stub `g_pCX2SocketItem`, against the sources; the entry dumps must
   be byte-identical) and the load test (0 unresolved names), on the packed `data036.kom`
   entries, not the staging copies (`elstools extract` the live archive to scratch).
6. The reach numbers in the Done note.

Hygiene is one command:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_SOCKET_PARITY --range <16-data commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole step. Fix clear
transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions
back to the user. Write "Phase 16-data -- Review" into the plan doc, update the STATUS.md row,
check `16-step3b.md`, and commit.
