# June effect completion - C9h-2d - June's pet-preview loader, lookup and PetPreView.lua (port)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-2d` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-2c (mapped it), C9h-2b (the cash-shop caller `PetPreview` is already June's) |
| Flag | `SERV_IRUHADEV_JUNE_PET_PREVIEW_LOOKUP`, also `#define`d at the end of `KncWX2Server/Common/ServerDefine.h` and listed in `SECOND_HOMES` (step 4: `ClientPacket_Global.h` precedes `Always.h`) |
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
     - **Get order (C9h-2c, `320de0f0`):** the map, then `std::set<int>` -> `+0xa94`, then `std::set<int>` -> `+0xaa4`.
       Add both sets to `KEGS_UPDATE_CODE_EVENT_NOT` (`ClientPacket_Global.h:531`), plus its Put/Get in
       `ClientPacket_Global.cpp`.
     - **`ClientPacket_Global.h` is reached before `Always.h`**: `X2Main.h:4` -> `X2ServerProtocolLib.h:80` ->
       `X2ServerPacket.h:8` -> `ClientPacket.h:11056`. So the flag also needs its `ServerDefine.h` second home and a
       `SECOND_HOMES` entry in `tools/flagcheck.py` (the `CommonPacket.h` rule in CLAUDE.md).
     - Nothing sends this packet offline, so both sets stay empty: June's state with no code events.
   - **Server time-zone.** `31c55270` shifts each parsed date by (`CX2Data+0x470` - `_timezone`).
     - **Mapped by C9h-2c:**
       - `+0x470` has one writer, `31edce90`. Its only caller is June's `EGS_CURRENT_TIME_ACK` handler `32571b80`, which
         passes the packet's 3rd field (Get `320a2930`: wstring, `__int64` x3; March's packet has wstring + one `__int64`).
       - June's ctor `31eef470` never writes `+0x470`.
       - Offline the emulator sends March's ack, so June has no value here. A stand-in is needed.
     - **Stand-in:** use the user's answer, recorded in STATUS's C9h-2c row. If it has none, ask at phase start.
       - The proposal is `_timezone`: the shift is 0, so a date parses exactly as March's `ConvertStringToCTime`.
       - The alternative is 0, which shifts every date by `-_timezone`.
       - Either way it is inert on June's data.
     - Port the member, its getter and the shifted parse. Do not widen `EGS_CURRENT_TIME_ACK`: its other two fields have
       no reader in this port.
5. **Data.** Adopt June's `PetPreView.lua` whole:
   - copy `june_src_fixed/data036_kom/PetPreView.src.lua` verbatim to `kom_contents/data036_kom/PetPreView.src.lua`;
   - `python tools/kom.py compile data036`, then `/elstools pack data036`;
   - check that `kom.py status data036` shows `PetPreView.lua` changed. Any other changed entry must be a peer's pending
     data036 work (C9h-3a also packs data036 for `Enum.lua`): name it in the Done note, do not revert it;
   - ask before `/x2-kom-packed copy data036`.
   June's file is a superset of March's: all of March's preview items (500010, 500150, 500020, 90002050/70, 550000-550030,
   269731) are in it.
   - **`datai015.kom`** holds an entry `PetPreView_1.lua`, a different name from `PetPreView.lua`.
   - March opens only `L"PetPreView.lua"` (`X2Data.cpp:1784`), so data036's copy is the one that loads, and the
     datai015 entry is never read.
6. **Reach census** for the Done note:
   - Of the 241 rows that resolve on March's enums, how many have a March pet / riding-pet templet? Rows without one are
     Stage 17's, like the 152.
   - Does `ResetPet` / `ResetRidingPet` on a missing templet stay silent? (It should, since C9h-2a.)

**Done =**
- 152 `skip` lines on startup, and 0 popups;
- a preview item that is new against March's file shows its pet. Choose the hand-off's item from step 6's census, among rows
  whose pet has a March templet; do not pick one before the census;
- March's preview items behave as in C9h-2b's play-test;
- the `JUNE_FX PET_PREVIEW` lines from C9h-2b still log.

The June-only ids arrive as 0 and are skipped at load, so they never reach the viewer. **Stop and ask only if** step 6 finds
resolved ids with no March templet that `ResetPet` / `ResetRidingPet` would not survive.
