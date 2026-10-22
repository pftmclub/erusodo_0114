# June dungeon result screen: the in-state GFx result UI

Plan key `june-result` · branch `main` · created 2026-10-21 (user: "separate the result screen from the UI port, make it its own plan";
split out of backlog Stage 9) · log tag `[JUNE_RESULT]` · flags `SERV_IRUHADEV_JUNE_RESULT_*` · kickoffs `kickoffs/june-result/` ·
backlog Stage 29.

## Context

**What the player sees today.** A dungeon clear runs March's path: `m_bGameEnd` -> `m_fGameEndWaitTime` -> the host's
`StateChangeResultReq()` (`X2StateDungeonGame.cpp:1072-1082`, `:2702`) -> `EGS_STATE_CHANGE_RESULT_NOT` ->
`CX2DungeonGame::Handler_EGS_STATE_CHANGE_RESULT_NOT` (`X2DungeonGame.cpp:2381-2387`) -> `XS_DUNGEON_RESULT` -> `CX2StateDungeonResult`
(`X2StateDungeonResult.cpp`, 3,074 lines; KTDGUI `DLG_Dungeon_Result_Back/Front.lua`; the 15-step `DUNGEON_RESULT_ANIMATE_ORDER`).
The data comes from `Handler_EGS_END_GAME_DUNGEON_RESULT_DATA_NOT` (`X2DungeonGame.cpp:2348`) -> `g_pData->ResetDungeonResultInfo`.

**What June shows.** June keeps the player in the dungeon game state and opens a Scaleform widget over the frozen game. Planning
evidence (2026-10-21, Ghidra read-only; [D] = seen in the listing, [I] = inferred, R0 confirms):

| Piece | June | Notes |
|---|---|---|
| Widget file | `data194` `dungeon_result.gfx` (46,835 B) + 13 `*_I*.dds` | [D] staged byte-exact by june-gfx GA (`ga_staged_files.tsv:4178`), data194 deployed |
| Widget id | GFx UI 8, `UIDungeonResult` (`33539ed8` `.showUI`, `33539ef0` `.hideUI`) | [D] |
| Adapter | `CX2UIDungeonResultAdapter` ctor `32eea7e0`, dtor `32eea090` (deleting `32eeafd0`), setter `32eea6a0`, `enableRetryButton` `32eea1f0` | [D]; RTTI also `CX2UIDungeonResultAdapterCallbackHandler`, `DW_DUNGEON_RESULT_VALUE` |
| Bindings | `GameResult.updateDungeonInfo`, `getItem0..11`, `showRetryButton`, `enableRetryButton`, `onTabKeyProcess` | [D] strings `33539be4..3353a03c` |
| Callbacks | `notifyInitialize` `32eea300`, `notifyCloseResultUI` `32eea2c0` (dungeon; PvE / PvP twins `32ee82d0` / `32eedfe0`) | [D] vtable slots only, no direct callers |
| Owner | dungeon game `+0x7c4`, made by vslot 46 `324c8b80`, items by `324cf8b0` (12 slots, empty = `( 0, 0, -1, -1 )`) | [D]; `324cf8b0` callers `324d60e0`, `324d1210`, `324d1281` |
| End packet | `324db6b0` (dispatcher `324dbc20` case `0xbd`): result info `g_pData+0x148` set -> mode 1 for 30 s, else leave (`31f16530`) | [D] funcmap 2524 |
| Mode machine | `324cf3d0` (dungeon / training vslot `0xd4`): mode 1 -> after 0.1 s vslot `0xc8`, GFx UI 8, `SetShow( false, 2 )`; no info -> vslot `0xf0`; `g_pMain+0x16a` bypass; mode 2 -> vslots `0xc0` / `0xdc`, mode 3; countdown `+0x218 - +0x214` (vslot `0xd8`), mode 4 | [D] funcmap 2526; `+0x16a` [I] |
| State support | `CX2State` result-UI mode setter `31ef7be0( mode, time )` (`+0x210`, `+0x218`), base vslot `0xd4` `31ef7390` (`+0x214 += elapsed`), getter `31ef7c60` | [D] funcmap 2512-2513 |
| Hide modes | `324c966e` / `324d880f` / `324cf508` (2), mode 3 also hides the minimap | [D] C3p-2 review |
| ESC + exit box | `HandleMsgByESC @31f1e214-31f1e261` eats ESC while a mode is set; exit box `31f1dd70` (`DLG_UI_ExitPartyMsgBox.lua`, msg `0xbdb`) | [D] funcmap 2514 |
| Results confirm | June-new game-state `Handler_EGS_RESULT_ALL_UNIT_SUCCESS_NOT` (`324dbb9a`); the machine ends in `EGS_RESULT_SUCCESS_REQ` | [D] `c14c_tutorial_reads.tsv` 36 / 43 |
| Retry | June-only ACK `0x649` (`324c95b0`): int result + wstring; failure -> vslot `0xe4( 1, t )` (`324c83f0` -> `enableRetryButton`), `SetShow( false, 2 )` | [I] retry ACK; the name is not in the binary |
| Legacy state | June still builds a KTDGUI `CX2StateDungeonResult` for state 0xD (factory `31d88bf0`, ctor `32515670`, setup `32513d30`, OnFrameMove `3250f6c0`); training result 0x13 (`324c0f80`) derives from it | [D]; **which cases still reach it is unknown** |

**June data the screen may use** (adoption state unchecked): June-only textures `data063` `DLG_UI_Dungeon_Result_01/_02` (`.tga` +
`.TET`); same-name June dialogs `data034` `DLG_Dungeon_Result_{Back,Front,Info,Reward_Box,Reward_Window1-3,Window1-3}`, `datai014`
`DLG_UI_Dungeon_Result.lua`; sounds `data085` `Result*.ogg` (June's legacy state drops `ResultStamp.ogg`, `c9h7_playsound_census.tsv:92`).

**Offline emulator today** (`X2Lib/Offline/Handlers_Room.cpp`): `EndDungeonGame` `:4541` (holds the result data, sends
`EGS_UPDATE_UNIT_INFO_NOT` and `EGS_END_GAME_NOT`), `SendDungeonResultData` `:4383` (`EGS_END_GAME_DUNGEON_RESULT_DATA_NOT`, ranks via
`CX2OfflineDungeonRank`), `Handler_EGS_STATE_CHANGE_RESULT_REQ` `:4779`, `Handler_EGS_RESULT_SUCCESS_REQ` `:4815`. The reward box is
not emulated (`:5123`).

## Scope fence

The user asked that the phases do not drift from the subject. This section is the test every phase applies.

**In this plan:** the dungeon result, from the end-of-game packet in a PvE dungeon to leaving the dungeon (field / village) or
retrying it:
- the GFx widget, its adapter, bindings and callbacks;
- the result-UI mode on `CX2State` and the dungeon game state's mode machine and the vslots it calls;
- ESC during the result and June's exit box;
- the retry button and its offline answer;
- the dialog hide modes this flow sets;
- the offline emulator's answers to whatever June's flow sends;
- June's legacy `CX2StateDungeonResult` only if R0 proves a route offline reaches it;
- training, Henir, secret and defence dungeons only where they run these same June functions.

**Not in this plan (owner):**

| Item | Owner |
|---|---|
| PvE result (`XS_PVE_RESULT`, `UIPvEResult`, `PVE_result.gfx`, `324ab1c0`) and PvP result (`BATTLE_PVP_result.gfx`, `3252ba10`; no PvP offline) | backlog Stage 9 |
| Cash shop `0xb`, option window, bingo, the profession-manager close `@31f1e27a`, hide mode 5 `PresentProlBlackBack` `328c3750` | backlog Stage 9 |
| Dungeon first-clear / intro movies | backlog Stage 26 (june-movies) |
| Party-manager handlers (`3243ee30`) | backlog Stage 28 (june-party) |
| `OpenResultMission` `31f06000`, quest popups | quest-system |
| Fonts in the widget | backlog Stage 25 (june-fonts) |
| Rank math | stays `CX2OfflineDungeonRank` (`SERV_IRUHADEV_JUNE_QUEST_RESULT_RANK`) |
| EXP, ED, drop values | unchanged; the screen shows what the emulator already sends |

**Drift rule.** A June branch a phase finds outside the fence is **identified** in that phase's Done note (June addresses, March
counterpart, offline reach, related branches, funcmap rows) and routed to its owner above as a dated bullet. It is never added here.
**One exception:** a softlock or crash on the result path, which gets a phase here (backlog "Standing rules", *Scope*, item 2).

## Rules

1. **June wins; no partial updates; June bugs verbatim** with a `JUNE_BUGS.md` row. Cite June addresses; never port raw offsets.
   A June function this plan ports is ported whole; its callees are decided by the scope fence.
2. **Flags.** One `SERV_IRUHADEV_JUNE_RESULT_*` flag per logical change in `KTDXLIB/Always.h`, March code in `#else`, `#endif FLAG`.
   A new event ID or packet struct follows CLAUDE.md's offline override (the `CommonPacket.h` second home where it applies).
3. **Data.** June's files go in **byte-exact** through `kom.py stage-june` / `luasrc.py adopt-june` (memory
   `june-assets-byte-exact-no-edits`); a same-name June file replaces March's. No `.lua` is invented. `/elstools pack`, then ask
   before every `/x2-kom-packed copy`.
4. **Offline only.** The emulator is the only other side. Keep `X2OfflineDB` unchanged unless R0 says a column is needed.
5. **Degrade visibly.** If `dungeon_result.gfx` fails to load, log it and follow June's own no-widget branch; never guess.
6. **Opus phases delegate** reads, greps, builds and log reads (memory `opus-phases-delegate-mundane-work`).
7. **Never launch `X2_offline.exe`**: ask the user.

Flags (debug: `SERV_IRUHADEV_JUNE_RESULT_DEBUG`): `_JUNE_RESULT_WIDGET` (R1), `_JUNE_RESULT_OFFLINE` (R2), `_JUNE_RESULT_FLOW` (R3),
`_JUNE_RESULT_LEGACY` (R4, if run). Log tag **`[JUNE_RESULT]`**, sub-tags `MAP`, `WIDGET`, `EMU`, `FLOW`, `LEGACY`:
`python tools/logs.py --tag JUNE_RESULT`. Funcmap tag `Result <phase>`.

## Starting context

| Need | Command |
|---|---|
| One section of this doc | `python tools/docsection.py JUNE_DUNGEON_RESULT_PHASES.md "<heading>"` |
| The C3p-2 result-UI notes | `python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3p-2 - Done (2026-10-15, Opus 5.5 high; every gate mapped, nothing portable, no code, no build)" "C3p-2 - Review (2026-10-15, Opus 5.5 high; PASS with four corrections, no code, no build)"` |
| The GFx manager API | `python tools/docsection.py JUNE_GFX_RUNTIME_PHASES.md "Phase G2 - Done" "Phase G3 - Event scenes"` (`--list` for the exact headings) and `kickoffs/june-gfx/G3.md` "Amended by G2" |
| Ghidra | `ghidra` skill (read-only in R0 and reviews) |
| June files | `F:\elsword stuff\elsword_2014\els_2014\237311\14JUN17\data\`; `python tools/junekom.py list / extract`; June Lua from `june_src_fixed/` |
| Pack / copy | `/elstools pack dataNNN`, `/x2-kom-packed copy dataNNN` (ask first) |
| Edit a CP949 source | `python tools/bytepatch.py show / find / apply` (x2-byte-edit) |
| Hygiene / build / logs | `flagcheck.py <FLAG> --worktree --files ...`, `build.py --deploy` (background), `logs.py --tag JUNE_RESULT` |
| Save / packets | `python tools/elsdb.py ...` (x2-db); `offline_packets.log` for the result packets |
| Dev console | auth level 3 (`python tools/elsdb.py setting auth_level 3`), END key |

## Phases

| Phase | What | Run with | Code | Play-test |
|---|---|---|---|---|
| R0 | Port map: the end -> result -> leave path, the widget's bindings, packet `0x649`, the legacy-state routes, June data vs the ledger | Opus 5.5 high | no | no |
| R1 | The GFx widget: adapter + callback handler + `DW_DUNGEON_RESULT_VALUE`, the item fill, UI 8 load; dev-console opener | Sonnet 5.5 high + Opus review | yes | yes (console) |
| R2 | Offline emulator: the retry REQ / ACK, result-confirm timing without the state change, whatever else R0 lists | Sonnet 5.5 high + Opus review (medium) | yes | with R3 |
| R3 | The in-state flow: `CX2State` mode, end handler, mode machine + vslots, ESC gate, exit box, hide modes, `ALL_UNIT_SUCCESS` in the game state, `0x649` handler | Opus 5.5 high + review | yes | yes |
| R4 | June's legacy `CX2StateDungeonResult` (only if R0 proves a reachable route) | Sonnet 5.5 high + Opus review | yes | yes |
| Z | Close-out | Sonnet 5.5 medium | no | no |

**Model choice.**
- **Opus for R0:** every later phase transcribes its map, so a misread vslot or binding costs them all.
- **Opus for R3:** it replaces a March state transition that runs on every dungeon clear. A wrong mode step is a softlock or an
  eaten ESC on every dungeon, and the June / March state split (in-state UI vs a separate state) needs judgment at each call site.
- **Sonnet for R1, R2 and R4:** they transcribe R0's rows onto proven patterns (G2 / G3's adapter and listener, the emulator's
  handler shape, a known class), each with a scoped Opus review.
- Fable is not used; Opus 5.5 high is enough for every phase here.

**Order and parallelism.**
- R0 is read-only and can run now, alongside anything.
- R1 and R2 can run together after R0: separate files, and builds queue through the lock.
- R3 needs the R1 review and R2.
- R4 runs after the R3 review, and only if R0 opened it.
- Z comes last.
- R3 edits `X2State.cpp` / `X2StateDungeonGame.cpp` / `X2DungeonGame.cpp`: check STATUS for a running quest-system, june-engine or
  pilot-char row on those files first.

## Phase R0 - Port map

**Run with:** Opus 5.5 high, read-only (Ghidra, repo, no code, no build). Output `june2017_re/result_r0_map.tsv`
(`june_va | june_name | march_equivalent | verdict | target | evidence`, the `g0_gfx_map.tsv` shape) + funcmap rows `Result R0`;
amend the R1-R4 kickoffs (an "Amended by R0" section each).

1. **End packet.** `324db6b0` whole: what case `0xbd` is in March's `EventID_*.h` (`EGS_END_GAME_NOT`?), every store, the
   no-result-info leave `31f16530`, and what March's `X2StateDungeonGame` handler does at the same point.
2. **Mode machine.** `324cf3d0` and every dungeon / training vslot it reaches (`0xc0`, `0xc8`, `0xd0`, `0xd8`, `0xdc`, `0xe4`,
   `0xec`, `0xf0`): name each against March's `CX2StateDungeonGame` / `CX2State` virtuals or mark it June-only. What `g_pMain+0x16a`
   is (an option, a cheat, a flag set by something offline reaches).
3. **Widget.** The adapter whole (`32eea7e0` / `32eea090` / `32eea6a0` / `32eea1f0` / `showRetryButton` / `updateDungeonInfo`), the
   callback handler (`notifyInitialize` `32eea300`, `notifyCloseResultUI` `32eea2c0`, `onTabKeyProcess`), `DW_DUNGEON_RESULT_VALUE`,
   vslot 46 `324c8b80`, the item fill `324cf8b0` and its callers. For **every value pushed to the widget**, name its source in
   March's `DungeonResultInfo` / `KDungeonUnitResultInfo` / `mapItemObtained` (`ClientPacket.h:916`, `:1044`, `:1879`), or mark it
   June-only with what June reads. How the widget is loaded (which `CKTDGGFXManager` wrapper, which path); whether it needs a GFx
   piece june-gfx left as a stand-in (IME id 0, resize).
4. **Leaving.** Who sends `EGS_RESULT_SUCCESS_REQ` and when; June's game-state `Handler_EGS_RESULT_ALL_UNIT_SUCCESS_NOT`
   (`324dbb9a`); how the player gets to the field / village; the AI party's end (`EndOfflinePartyBots` today runs from the
   result-data handler).
5. **Retry.** Packet `0x649` and the REQ that precedes it: map both to March's event list or prove them June-only (the June binary
   has no name table; read the REQ's Put and the ACK's Get layouts). What the retry does client-side.
6. **ESC and exit box.** `@31f1e214-31f1e261` and `31f1dd70` against March's `HandleMsgByESC` and its exit-party message box.
7. **Hide modes.** The mode 2-4 callers in this flow against june-gfx G5's `JuneSetShow` presets.
8. **Legacy routes.** When June still reaches `CX2StateDungeonResult` (state 0xD) and the training result (0x13): vslot `0xf0`,
   `+0x16a`, Henir / defence / secret, training. **Decide R4:** open it with the reachable routes, or close it here with the proof.
9. **Training.** Whether June's training game (`324c2a80`, base `324d9f80`) shares every function above. If it does, R3 covers it;
   anything training-only goes to the drift rule.
10. **Data.** Against `tools/data/lua_source_ledger.tsv`, the asset-port staging tsvs and `kom_contents`: are the `data063`
    `DLG_UI_Dungeon_Result_*` textures, the `data034` / `datai014` result dialogs and the `Result*.ogg` June's already? List what R1 or
    R4 must stage.
11. **Emulator list.** Every packet June's flow sends that `X2Lib/Offline/` does not answer, and every packet the emulator sends that
    June's flow no longer reads (e.g. `EGS_STATE_CHANGE_RESULT_NOT`). This is R2's list.

**Done gate:** the map tsv has a verdict for every address in Context plus everything R0 found; R1-R4 kickoffs amended; R4 opened or
closed with the proof; anything outside the fence identified and routed (drift rule).

### R0 - Done (2026-10-22, Opus 5.5 high; read-only, no code, no build)

Map: `june2017_re/result_r0_map.tsv` (100 rows; verdict D = seen, I = inferred, open, closed). Funcmap rows `(Result R0)`. Kickoffs
R1-R4 have an "Amended by R0" section each. No deploy; nothing for the user to play.

1. **End packet.** June case 0xbd is **`EGS_END_GAME_DUNGEON_RESULT_DATA_NOT`**, not `EGS_END_GAME_NOT`, which corrects funcmap 2524.
   - Proof: its Get (`320edc20`) is that packet with June-only ints at +4 / +9 and a 0xd2 B unit record.
   - June renumbered this id range by 0xa-0xd, so packets are matched by layout. Cases 0xbb, 0xc0, 0xc1, 0xd5, 0x10d and 0x10e were
     matched the same way.
   - What the handler does: calls the game's handler (`327c99e0`, so `EndOfflinePartyBots` still runs), then sets mode 1 for 30 s.
     With no result info it leaves through `31f16530` (`ReturnToPlaceWhereBeforeDungeonStart`).
2. **Mode machine.** All vslots are named: 0xb8-0xcc are the adapter's, 0xd0 is mode 4, 0xd8 / 0xdc are the base party-member
   dialog, 0xe0-0xec are retry and ESC, 0xf0 is the confirm.
   - `g_pMain+0x16a` is `m_bIsPlayingTutorial` (funcmap 1080, ported). It is **not a user option**, so there is no question for the user.
   - State `+0x7a9` is written only by June event 0x3e7, which offline never sends.
3. **Widget.** It is owned by the dungeon state (`+0x7c4`) and opened as UI 8 through the ported G2 wrappers. The tsv lists all 16
   bindings and each member with its June and March source.
   - Values June keeps absent are 0 or empty: hit rank, reward type, bestSec, resonance, the bonus strings, 3 of the 6 detail values.
   - The item list is June's dungeon-game list (`+0xdf4`). Its writer is a virtual call R1 must locate.
4. **Leaving.** vslot 0xf0 sends `EGS_RESULT_SUCCESS_REQ` with **no body**, after the 30 s countdown or the exit box OK.
   - June's game state handles the ACK and `RESULT_ALL_UNIT_SUCCESS_NOT` (`324d4cd0` / `324dbb30`), then calls `31f16530`.
   - June keeps March's host `STATE_CHANGE_RESULT_REQ` timer (`324ccda0`). Its NOT handler (`327c9330`) dropped the `XS_DUNGEON_RESULT`
     change.
5. **Retry.** REQ 0x648 has no body; ACK 0x649 is `{ int, wstring }`. Neither exists in March (probe).
   - The leader sends the REQ at once; solo goes through a confirm popup (type 9, custom msg 14).
   - The restart itself was server-side, so R2 proposes a stand-in to the user.
6. **ESC / exit box:** as C3p-2. The exit box needs June-only `DLG_UI_ExitPartyMsgBox.lua`.
7. **Hide modes:** only `SetShow( false, 2 )`, at `324cf508`, `324c966e` and `324d880f`. `JuneSetShow` is ported (G5).
8. **R4: OPENED, one route.** State 0xD is unreachable: all 72 `XGM_STATE_CHANGE` (0x471) pushes were resolved and none is 0xD. The
   training result (0x13) is reached offline: `END_TC_GAME_ACK` -> 5 s -> state 0x13 -> `324c0f80` = June's `CX2StateDungeonResult`
   (`32515670`), as in March.
9. **Training** shares every vslot from 0xb8 to 0xf0, but never receives 0xbd, so the widget never opens there. Its result is R4's
   route.
10. **Data.**
    - Stage byte-exact: data063 `DLG_UI_Dungeon_Result_01/_02` `.tga` + `.TET` (R1); data034 `DLG_UI_ExitPartyMsgBox.lua` and
      `DLG_Party_Member_Result_State.lua`, both June-only (R3); data034 `DLG_Dungeon_Result_Reward_Window3.lua`, which differs (R4).
    - Already June's: data194 widget + 13 dds, the other 9 `DLG_Dungeon_Result_*`, `DLG_UI_Dungeon_Result.lua`, the 4 `Result*.ogg`.
11. **Emulator (R2).**
    - Accept a bodyless `RESULT_SUCCESS_REQ`; today `ReadReq` fails on it.
    - Answer 0x648 with 0x649 (stand-in) and ignore 0x1f2 (the mode-3 fast-solo-clear report).
    - Nothing the emulator sends goes unread.

**Drift (routed, dated bullets in `JUNE_PORT_BACKLOG_ORDER.md`):**
- 0x3e7 `324d1290`, the post-clear NPC spawn and `+0x7a9` -> Stage 22.
- The resonance system (`3247b460`, unit `+0xa99` / `+0xa9d`) -> Stage 6.
- The party manager's retry refresh (`@32434dd6`) and the ACK's `324334d0( 0x72 )` branch -> Stage 28.
- PvE shares the retry vslots -> Stage 9.
- The per-dungeon attribute bit 14 hides retry -> the Stage 22 attribute-set bullet.

## Phase R1 - The GFx widget

**Run with:** Sonnet 5.5 high, then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_RESULT_WIDGET` (+ `_JUNE_RESULT_DEBUG`).
Sub-tag `WIDGET`.

- Port `CX2UIDungeonResultAdapter` and its callback handler whole from R0's rows. Register the callbacks through the manager's
  listener table (G3's `AddListener` pattern). Port `DW_DUNGEON_RESULT_VALUE`, vslot 46 `324c8b80` (the adapter at the dungeon
  game's `+0x7c4` equivalent) and the item fill `324cf8b0`.
- Load `dungeon_result.gfx` as GFx UI 8 the way June does (R0 item 3).
- Stage any June-only texture R0 found missing, byte-exact.
- A dev-console opener under `_DEBUG` (a C++ binding, no `.lua`) that opens the widget in a dungeon from the current
  `g_pData` result info. With no info it fills nothing and logs `no result info`; it never invents values.
- March's result flow is untouched in this phase.

**Done gate:** in a dungeon, the console opener shows the widget; empty item slots read June's `( 0, 0, -1, -1 )`; the close
button's `notifyCloseResultUI` and the tab key log `[JUNE_RESULT] WIDGET` lines; the widget unloads cleanly; 0 EXCEPTION; a normal
clear still runs March's result screen.

### R1 - Done (2026-10-22, Sonnet 5.5 high; flags `SERV_IRUHADEV_JUNE_RESULT_WIDGET` + `_DEBUG`, log `[JUNE_RESULT] WIDGET`)

**What landed** (one commit, `main`; funcmap rows `(Result R1)`, 12):
- `KTDGGFXManager.{h,cpp}`: `CKTDGGFXResultBridge` = June's `CX2UIDungeonResultAdapter` Scaleform half. Listener `UIDungeonResult`, the AS call `gameDataResultFunctionInit( obj )` that puts the five native callbacks on `obj` (table `33539ea8`, "notfy" spelling kept), the 16 bindings (`GameResult.updateDungeonInfo`, `getItem0..11`, `onTabKeyProcess`, `showRetryButton`, `enableRetryButton`), and the two value objects (`DW_DUNGEON_RESULT_VALUE` `32eeb1d0`, item `32eeaff0`: exact member names, order and Scaleform types). G3's `CreateBindingImpl` now walks the listeners' new `OnBindingCreated` (June `31d1ed90`); the result listener answers true for every name (JUNE_BUGS 403), so the widget gets its first `updateDungeonInfo` the moment its observer is added.
- `X2Lib/X2UIDungeonResultAdapter.{h,cpp}` (new, in `X2Lib_2010.vcxproj`): the host. Icon path = `GetShopImage` of the item templet. The arrows (`324d1210` / `324d1270`) are real; close / retry / initialize are R3's (see below).
- `CX2StateDungeonGame`: `m_pJuneResultAdapter` (+0x7c4) and the page / retry members; `JuneResultCreateAdapter` / `Delete` / `Recreate` / `CloseUI` / `CloseAndDelete` (vslots 0xb8 / 0xbc / 0xc8 / 0xc4 / 0xcc), `JuneResultFill` (0xc0, `324d60e0`), `JuneResultFillItems` (`324cf8b0`), `JuneResultEnableRetry` (0xe4), `JuneResultRetryAllowed` (`324d1120`). The destructor closes UI 8 and frees the adapter when one exists. Training inherits it.
- **Obtained-item list (the open R0 item).** The `0x21c` caller is `CX2Inventory::UpdateInventorySlotList` (`3286d590 @3286d612-3286d707`; June adds a second bool argument, false for every literal June caller and for all March callers): per changed slot with a UID, every category but `ST_E_DEFENCE_QUICK_SLOT` offers the item to the dungeon game, quantity items by their change. `CX2DungeonGame::JuneAddObtainedItem` / `JuneGetObtainedItems` / `JuneHasObtainedItemPage` / `JuneSortObtainedItems` = vslots 0x21c-0x228 (`CX2Item::ItemData` is the 0xc0 B record).
- Fill details worth knowing: the six lists use June's order (`324ddca0`: detail descending, ties by name descending, category 4 ascending, equal entries dropped); `bExactFit` = `GetResolutionScaleY() < 1`, `fSizeX` = viewport width; exp range = `ExpTable( level + 1 ).m_nNeedExp`; all June-only terms read 0 (hit rank, reward type, best time, resonance, bonus strings, categories 1 / 4 / 5). The fill needs my own row in the result info, as June.
- `X2Main.cpp` binds `JuneResultOpen()` / `JuneResultClose()` (debug). Data: June's `data063` `DLG_UI_Dungeon_Result_01/_02` `.tga` + `.TET` staged byte-exact and packed (`PACK OK data063`, +4 added); **the copy to the game dir waits for the user's yes.** `data194`'s `dungeon_result.gfx` was already June's.
- JUNE_BUGS 403-406; `MODS.md` revert block.

**Left to R3** (`kickoffs/june-result/R3.md`, "Amended by R1"): `JuneResultOnClose` (exit box `31f1dd70`), `JuneResultOnRetry` (vslot 0xe0), `JuneResultOnInitialize` (result mode 2), the cool-down tick `324c83a0`, `324d11e0`, `SetShow( false, 2 )`. In the dev console the close button just closes the widget and notifyInitialize runs the fill.

**Drift** (identified, routed): `324d60e0` starts with two dialog-stack flag walks (`31d05fe0` / `31d05ec0`, objects at `g_pKTDXApp +0x1fc +0x38c`) that close marked dialogs; not ported, bullet in `JUNE_PORT_BACKLOG_ORDER.md` Stage 9. Resonance (`3247b460`, value 0) stays with the Stage 6 bullet R0 wrote. The 83 `UpdateInventorySlotList` callers' second argument is only resolved for the literal pushes (all 0).

**Hygiene** (`flagcheck.py JUNE_RESULT_WIDGET --worktree --with JUNE_RESULT_DEBUG --files <10 code files>`): files: 5 PASS, 2 PASS~ (blank/marker only), 1 DEFINE, 1 OTHER (X2Main.cpp: the `_DEBUG` flag), 1 FAIL, 0 ERROR; in this flag's files: 0 deleted line(s), 0 non-ASCII inserted, 0 EOL-mismatched, 2 #else/#endif without the flag token (the `Always.h` `#error` guards, plain `#endif` as the file's other guards), 0 encoding change(s); RESULT: FAIL = the new `.cpp`'s `#include "StdAfx.h"` PCH line only (accepted, as G1 / G3). Range run follows the commit.
**Deploy:** `DEPLOY OK: X2_offline.exe is 25,776,640 B, dated 2026-10-22 13:12:40, md5 5b05a7ab0efb3a5199c9f0ee441c3f3c, equal to X2/US_SERVICE/x2.exe (2026-10-22 13:12:32); built at main 0ae9bdcc + 11 uncommitted code file(s).` Strings `JuneResultOpen`, `gameDataResultFunctionInit`, `GameResult.updateDungeonInfo`, `notfyitemSlotArrowRight`, `[JUNE_RESULT] WIDGET ` are in the exe. The first build stopped on one error of mine (`GetPartyMemberData` is on `PartyData`), fixed.

**How to see it:** auth level 3 (set), a dungeon, dev console (END). `JuneResultOpen()` opens the widget (empty before a clear: log `fill: no result info`); `JuneResultClose()` closes it. Log: `python tools/logs.py --tag JUNE_RESULT`.

**Play-test PASSED 2026-10-22** (user: "worked perfectly"; `data063` copied: `COPY OK data063 ... 440 entries, md5 2b96d7f79a8ca6471a883ec7156ff8e6`). ErrorLog: `open: adapter bound, UI 8 opened`, `notifyInitialize`, `fill: no result info` (before a clear), `close: UI 8 closed, adapter deleted`; 0 EXCEPTION, only the baseline `E_DISABLE_HB_CHECK_REQ` UNHANDLED.

## Phase R2 - Offline emulator for June's flow

**Run with:** Sonnet 5.5 high, then an Opus 5.5 medium review. Flag `SERV_IRUHADEV_JUNE_RESULT_OFFLINE`. Sub-tag `EMU`.

- Answer R0 item 11's list in `X2Lib/Offline/` (`Handlers_Room.cpp` and friends). Expected:
  - the retry REQ / `0x649` ACK (re-entering the same dungeon with the same party);
  - the result-confirm timing (`EGS_RESULT_SUCCESS_REQ` / `EGS_RESULT_ALL_UNIT_SUCCESS_NOT`) when the client never sends
    `EGS_STATE_CHANGE_RESULT_REQ`.
  - A June-only event ID is appended at the end of its `EventID_*.h` block behind the flag (offline override), with Put / Get
    exactly as R0 read them.
- March's handlers stay for the March flow (flag off, or R4's legacy route).
- New handlers get dispatch rows and `IGNORED` reasons where June's flow sends something the emulator deliberately ignores.

**Done gate:** played with R3: a clear plus a retry plus a leave leaves `UNHANDLED` at baseline only, and every request has an
`[JUNE_RESULT] EMU` line. On its own, R2 builds clean and the March flow still works (regression).

### R2 - Done (2026-10-22, Sonnet 5.5 high; flag `SERV_IRUHADEV_JUNE_RESULT_OFFLINE`, log `[JUNE_RESULT] EMU`)

**What landed** (one commit, `main`):
- `Handler_EGS_RESULT_SUCCESS_REQ` (`Handlers_Room.cpp`) accepts an empty body (`m_kbuff.GetLength() == 0`, June `324d84c0` SendID) and still reads March's struct; the ACK + `EGS_RESULT_ALL_UNIT_SUCCESS_NOT` answer is unchanged.
- Three June-only IDs at the end of `EventID_Client.h`: `EGS_JUNE_DUNGEON_RETRY_REQ` (0x648, no body), `EGS_JUNE_DUNGEON_RETRY_ACK` (0x649, `KEGS_JUNE_DUNGEON_RETRY_ACK { int m_iOK; wstring m_wstrJuneUnread }`, Put / Get in `ClientPacket.cpp` in R0's order), `EGS_JUNE_FAST_SOLO_CLEAR_NOT` (0x1f2, no body). June's names are not recoverable.
- `Handler_EGS_JUNE_DUNGEON_RETRY_REQ` + dispatch row. **Retry stand-in, user's choice 2026-10-22** ("same as start dungeon, same difficulty and dungeon"): the room's dungeon id, difficulty, mode, loot rule and bots are kept, the room-scoped buffs leave (`OnLeaveRoom`), `OpenDungeonGameRoom` reopens it, ACK OK, then `SendDungeonGameStartNot` (`EGS_PARTY_GAME_START_NOT`). Refused with `ERR_ROOM_00` for a tutorial / non-dungeon / inactive room (June re-enables the button).
- 0x1f2 is an `IGNORED` row in `X2OfflineIgnore.cpp` with its reason; nothing answers it.
- Flag defined in `Always.h` and the `ServerDefine.h` second home (`flagcheck.py` `SECOND_HOMES` updated). `MODS.md` has the revert line.
- No `X2OfflineDB` change, no new Lua, no June bug to log.

**Not R2's, handed to R3** (`kickoffs/june-result/R3.md`, "Amended by R2"):
- Nothing sends 0x648 / 0x1f2 until R3 ports the senders `324c9530` / `324cf734`, so the new handler cannot run in game yet.
- June's client does nothing on retry OK, so whether the March client can take a second `EGS_PARTY_GAME_START_NOT` while in `XS_DUNGEON_GAME` (state routing, same-state `XGM_STATE_CHANGE`, `RememberComeBackInfoBeforeInstanceGameStart` has no dungeon-game case) is R3's play-test gate. First version of the stand-in was ALL_UNIT_SUCCESS_NOT; that sends the player to the field (March `ReturnToPlaceWhereBeforeDungeonStart`, June `324dbb30`), so it was dropped before writing.

**Hygiene** (`flagcheck.py JUNE_RESULT_OFFLINE --worktree --files ...`): files: 3 PASS, 4 PASS~, 2 DEFINE, 0 OTHER, 0 FAIL, 0 ERROR; in this flag's files: 0 deleted, 0 non-ASCII inserted, 0 EOL-mismatched, 0 #else/#endif without token, 0 encoding change; RESULT: PASS. (Range run follows the commit.)

**Deploy:** `DEPLOY OK: X2_offline.exe is 25,735,680 B, dated 2026-10-22 12:50:29, md5 06eaaa64d79fceb154c26c189536eab7, equal to X2/US_SERVICE/x2.exe (2026-10-22 12:50:21); built at main 23706fbc + 11 uncommitted code file(s).` The new strings (`[JUNE_RESULT] EMU retry: restarting`, both event names) are in the exe.

**Drift:** none found; no branch outside the fence.

## Phase R3 - The in-state result flow

**Run with:** Opus 5.5 high, then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_RESULT_FLOW`. Sub-tag `FLOW`.

- `CX2State`: the result-UI mode `+0x210` / time `+0x214` / limit `+0x218` with the setter `31ef7be0`, the base vslot `0xd4`
  `31ef7390` and the getter `31ef7c60`, including mode 3's minimap hide and mode 0's restore.
- The end handler `324db6b0` whole; the mode machine `324cf3d0` and every vslot R0 marked R3's; June's game-state
  `Handler_EGS_RESULT_ALL_UNIT_SUCCESS_NOT`; the `0x649` handler `324c95b0`.
- The ESC gate `@31f1e214-31f1e261` inside March's `HandleMsgByESC`, and the exit box `31f1dd70`. The profession-manager close
  `@31f1e27a` stays Stage 9's (fence).
- The mode 2-4 `JuneSetShow` callers in this flow (G5 ported the mechanism).
- March's `m_bGameEnd` -> `StateChangeResultReq` -> `XS_DUNGEON_RESULT` path moves to `#else`, or stays only for the routes R0
  says June still sends to the legacy state.
- The AI party's end moves to wherever June's flow ends the game (R0 item 4).

**Done gate:**
- A dungeon clear shows June's widget over the frozen game, with time, ranks, EXP / ED and items matching
  `EGS_END_GAME_DUNGEON_RESULT_DATA_NOT` / `EGS_UPDATE_UNIT_INFO_NOT` in `offline_packets.log`.
- ESC is eaten while the result shows. Close returns to the field / village. Retry re-enters the dungeon.
- The AI party bots end cleanly. A failed run behaves as June's (R0).
- A training clear works, if R0 says it shares the code.
- 0 EXCEPTION; UNHANDLED at baseline.

## Phase R4 - June's legacy result state (conditional)

**Run with:** Sonnet 5.5 high, then an Opus 5.5 high review. Flag `SERV_IRUHADEV_JUNE_RESULT_LEGACY`. Sub-tag `LEGACY`.
**Runs only if R0 opened it** with the reachable routes; otherwise R0's Done note closes it and this section says so.
**R0 (2026-10-22): OPENED for one route, the training result (state 0x13, `324c0f80`).** State 0xD is unreachable (proof in R0's
Done note, item 8).

- Port June's `CX2StateDungeonResult` whole (`32515670` ctor, `32513d30` setup, `3250f6c0` OnFrameMove, `3250ef90`, ...) for
  the routes R0 found, and the training result `324c0f80` if it is one of them.
- Adopt June's same-name `DLG_Dungeon_Result_*` dialogs byte-exact.
- `ResultStamp.ogg` is dropped as June drops it.

**Done gate:** the route R0 named shows June's dialogs; 0 EXCEPTION; no `Motion Error` / missing-texture lines for the dialogs.

## Phase Z - Close-out

**Run with:** Sonnet 5.5 medium.
- `MODS.md`: June flag-index rows and revert lines for every flag.
- Backlog: a dated note in Stage 29. Stage 9's "GFx owed gates" and "root-UI screens" bullets say what moved and what stayed.
- Memory `june-result-plan`.
- Every `_DEBUG` flag off, with a build if one was on.

## Open questions

None at creation. R0 hands any design choice it cannot settle from June's code to the user (e.g. if `g_pMain+0x16a` turns out to be
a user option).
- R0 (2026-10-22): `g_pMain+0x16a` is `m_bIsPlayingTutorial`, so no question. One open design choice is left for R2: the offline answer
  to retry (0x648 / 0x649). The restart lived on the June server, so R2 proposes a stand-in and the user decides.

## Play-test hand-off

Every code phase hands over in this shape (memory `playtest-handoff-dev-console-copy-ready`):
- **Setup:** character + job, dungeon (name + id, one the save can enter), party bots on / off, auth level 3.
- **Per check:**
  - **where:** the dungeon and stage;
  - **paste:** the full console line, with real values;
  - **what it does;**
  - **look for:** on screen, plus the `[JUNE_RESULT] <sub>` line;
  - **if not:** what to report.
- **End with:** "then say 'check logs'"; the log tag is `JUNE_RESULT`.
