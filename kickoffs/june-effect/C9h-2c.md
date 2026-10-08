# June effect completion - C9h-2c - June's pet-preview lookup and PetPreView.lua (identify)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-2c` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-2b (found it; does not block it: June's `< 1` tests work with March's `-1` miss) |
| Flag | none (no code; the port this phase plans gets its own) |
| Log tag | none |
| Review | none (identify only); this phase writes the port kickoff + STATUS row, or a routing line |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-2b - Done"
grep -n "^32808770\|^3280b370\|^32234b10 (C9h-2b)" june2017_re/funcmap.tsv
```

March: `X2Lib/X2ItemManager.cpp` (`AddPetPreViewData_LUA` :4238, `GetPreviewIDByItemID` :4304), March's
`PetPreView.lua` (compiled, `kom_contents/data036_kom`; the C9h-2a Done note lists its ids). June:
`june_src_fixed/data036_kom/PetPreView.src.lua` (393 rows, bound as `g_pCashItemManager:AddPetPreViewData`).

## Task

June's cash shop finds the pet to preview through `3280b370` / `3280b380` -> `32808770`, a rewrite of March's
`GetPreviewIDByItemID`. March's lookup is a plain `rand()` pick over `m_vecPetPreViewData`. June's walks a 0x4c-stride
vector at `+0x1cc` and adds:
- CTime windows (`31c55270`, the ATL `CTime` compares);
- a code-event test (`32822650`);
- two id fields.

1. **Map it.** Name:
   - the object it runs on (June's `g_pCashItemManager`, or the item manager `g_pData+0x34`);
   - the 0x4c row layout and who fills it (`AddPetPreViewData` and its Lua binding);
   - what each date and id field means;
   - whether `rand()` survives.
2. **Compare the data.** Check June's `PetPreView.lua` arguments against March's `AddPetPreViewData_LUA( item, pet, type )`. Count the June rows that need a June-only pet or riding-pet id (backlog Stage 17), and the rows whose item is in June's `item.kim` (live since K2).
3. **Route it.**
   - If it is a self-contained lookup plus loader on March's item manager, write a port kickoff (Sonnet 5.5 high, flag `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP`) and its STATUS row.
   - If it needs June's cash-item manager as a system, give it a backlog line (Stage 9 or 16) instead, per the standing rule.

**Done =**
- funcmap rows, `Effect C9h-2c`;
- a short Done note;
- the kickoff or the routing line.
