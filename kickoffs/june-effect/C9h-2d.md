# June effect completion - C9h-2d - June's pet-preview loader, lookup and PetPreView.lua (port)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-2d` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-2c (mapped it), C9h-2b (the cash-shop caller `PetPreview` is already June's) |
| Flag | `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP` (+ a `CommonPacket.h`-style second home only if step 4 touches a header read before `Always.h`) |
| Log tag | `JUNE_FX PET_PREVIEW_LOOKUP` (unbracketed: `python tools/logs.py --grep "JUNE_FX PET_PREVIEW_LOOKUP"`) |
| Review | `kickoffs/june-effect/C9h-2d-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-2c - Done"
grep -n "Effect C9h-2c" june2017_re/funcmap.tsv
```

March: `X2Lib/X2ItemManager.h:12-34` (`PREVIEW_PET_DATA`), `:511`, `:688`; `X2Lib/X2ItemManager.cpp:4238-4320`
(`AddPetPreViewData_LUA`, `GetPreviewIDByItemID`); `X2Lib/X2Main.cpp:3544` (the `class_def`);
`X2Lib/X2InstanceData.h:427-437` (`SERV_CODE_EVENT`); `X2Lib/X2Data.cpp:2614` (`GetServerCurrentTime`).
June: `june_src_fixed/data036_kom/PetPreView.src.lua` (393 rows).

## Task

Port June's pet-preview table whole: the row, the loader, the lookup, and June's `PetPreView.lua`. The funcmap rows
tagged `Effect C9h-2c` carry every branch; port them as written there, in this order.

1. **Row.** Widen `PREVIEW_PET_DATA` to June's 0x4c row (`32822b60`):
   - item, id, type (as now);
   - second id;
   - start and end date `std::wstring`;
   - code event `int`.
2. **Loader = June `32812ce0`** (6 args: item, pet, type, second id, `const char*`, `const char*`). `lua_tinker` handles 6 args
   (`KNCSDK/Include_2010/lua_tinker.h` `mem_functor`), and nil reads as 0 / NULL.
   - **Drop March's `_IN_HOUSE_` block** (popup, `static bHasInvalidData` latch, templet checks). June has none of it.
   - In March that block is a live hazard with June's file: 152 of June's rows name a pet March's `Enum.lua` lacks. Each
     arrives as 0, and the first one would pop a box and reject every row after it.
   - Per the no-popups rule, log each skipped row instead: `JUNE_FX PET_PREVIEW_LOOKUP skip item=%d pet=%d` to ErrorLog.
     June's file should give exactly 152 such lines.
   - Strings, as June does it:
     - both strings given -> they are the two dates;
     - otherwise both dates are `L""`, and when only the first is given, `code = atoi( first )`.
3. **Lookup = June `32808770`** in full, replacing `GetPreviewIDByItemID`'s body:
   - the empty-date test, the window test, the second-id swap / dedupe, the code-event branches;
   - `vec[ rand() % size ]`, and `-1` on a miss.
   The two wrappers (`3280b370` / `3280b380`) are unchanged.
4. **The two externals.** Both are inert on June's data: no June row passes a 4th argument.
   - **Code-event set.** Add June's `std::set<int>` at `g_pInstanceData+0xa94` and its `contains` test (`32822650`) to
     `CX2InstanceData` under `SERV_CODE_EVENT`.
     - June's handler `31f00970` also fills it, plus a second set `+0xaa4` (the ice heater's, Stage 16), from a wider
       `EGS_UPDATE_CODE_EVENT_NOT`. June changed a function March has, so port the handler and the packet's two sets too.
     - Find June's Get order for that packet first (Ghidra), and check whether `ClientPacket_Global.h` is reached before
       `Always.h` (the `CommonPacket.h` rule in CLAUDE.md).
     - Nothing sends this packet offline, so both sets stay empty: June's state with no code events.
   - **Server time-zone.** `31c55270` shifts each parsed date by (`CX2Data+0x470` - `_timezone`).
     - Find `+0x470`'s writer and June's ctor default, then port the member and the shifted parse.
     - If only a server packet writes it, keep June's default, per `design-missing-june-pieces`, and say so in the Done note.
5. **Data.** Adopt June's `PetPreView.lua` whole:
   - copy `june_src_fixed/data036_kom/PetPreView.src.lua` verbatim to `kom_contents/data036_kom/PetPreView.src.lua`;
   - `python tools/kom.py compile data036`, then `/elstools pack data036`;
   - check that `kom.py status data036` changes only `PetPreView.lua`;
   - ask before `/x2-kom-packed copy data036`.
   June's file is a superset of March's: all of March's preview items (500010, 500150, 500020, 90002050/70, 550000-550030,
   269731) are in it.
6. **Reach census** for the Done note:
   - Of the 241 rows that resolve on March's enums, how many have a March pet / riding-pet templet? Rows without one are
     Stage 17's, like the 152.
   - Does `ResetPet` / `ResetRidingPet` on a missing templet stay silent? (It should, since C9h-2a.)

**Done =**
- 152 `skip` lines on startup, and 0 popups;
- a new June-only preview item shows its pet: pick one from the census, e.g. a `PUI_HALLOWEEN_PUMPKIN_*_F` row such as 90002292;
- March's preview items behave as in C9h-2b's play-test;
- the `JUNE_FX PET_PREVIEW` lines from C9h-2b still log.

Stop and ask only if a June-only pet id would crash March's viewer.
