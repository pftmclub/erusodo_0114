# June HUD: the buff icon row (top right, with its counter) and the quick slot (bottom left)

Plan key `june-hud` · branch `main` · created 2026-10-31 on Opus (user: "a separate UI port from Stage 9 for June's buff display with its counter on
the top right corner; remove the buff-duration mod and use what June does instead; and the consumable item UI on the bottom left corner") · log tag
`[JUNE_HUD]` (sub-tags `BUFF`, `SLOT`) · flags `SERV_IRUHADEV_JUNE_HUD_*` · kickoffs `kickoffs/june-hud/` · evidence `june2017_re/hud_functions.tsv` ·
backlog Stage 37, carved out of Stage 9.

## Context

Planning evidence (2026-10-31, read-only: Ghidra, `june_src_fixed/`, the live kom_contents). [D] = seen in the listing or file, [I] = inferred, H0 confirms.

### Buff row: what March draws today

Three writers share one text slot on each `Buff_%d` icon [D]:
- **`DISPLAY_BUFF_DURATION_TIME`** (studio, `Always.h:677`):
  - creates text slot 0 on every icon (`X2GageUI.cpp:956-969`);
  - the time finalizer (`X2BuffFinalizerTemplet.cpp:200-207`) calls `CX2GageManager::SetDurationTime` (`X2GageManager.cpp:3524-3535`) for **my unit only**;
  - that writes `CX2MyGageUI::SetDurationTime` (`X2GageUI.cpp:1772-1858`).
- **`SERV_IRUHADEV_JUNE_BUFF_DURATION_TIME`** (`Always.h:5164`, buff 11b): June's `32e608f0` body, nested inside that function. **Stays.**
- **`SERV_IRUHADEV_BUFF_DURATION_TEXT`** (`Always.h:3462`, the user's 2026-08-25 mod). **Deleted by H1** (user, 2026-10-31: delete the code, not only the define).
  - What it adds: `BuffIcon::fRemainTime` (`X2GageUI.h:79-91`) and `UpdateBuffDurationText` (`X2GageUI.cpp:1088-1125`), called only from the party-member and
    PvP gauges' `OnFrameMove` (`:2940`, `:3461`).
  - Supporting getters: `GetRemainTime` (`X2BuffFinalizerTemplet.h:55-58,142-144`) and `GetRemainDurationTime` (`X2BuffTemplet.h:185-188`, `.cpp:621-636`).
  - Five push sites pass the time: `X2BuffTemplet.cpp:268`, `X2GameUnit.cpp:14108,16747`, `X2PremiumBuffManager.cpp:785,907`.
  - Nothing nests inside it or requires it, and no `#error` names it. Its only visible effect is a countdown on **other units'** icons.

**Layout** [D]:
- `CX2MyGageUI::UpdateBuffIcon` (`X2GageUI.cpp:1646-1699`) creates `DLG_BuffManager.lua`. That file is already June's 365 B copy (buff Phase 12): `SetPos( 720, 2 )`,
  `AddDummyPos( 993, 26 )`, `AddDummyPos( 993, 2 )`. March never reads the dummies, so the row sits at x 720.
- My icons are 24 x 24 at `( -25 * index, y )`, so they grow left. Debuffs are at y 29. Party / PvP icons are 16 x 16 at `( 150, 125 + 44 * pos )` and grow right.
- No counter is drawn. June's applied count is stored (`X2GameUnit.cpp:14103`, `JUNE_BUFF_TEMPLET_PARITY`) and never shown.

### Buff row: June [D unless marked]

| June | Role | March |
|---|---|---|
| `32e67050` `CX2MyGageUI::UpdateBuffIcon` (vslot 0x3c) | skips raw states 7 / 0x16 / 0x1a; `SetPos` to dummy 1 (993, 2) in states 9 / 0x12, else dummy 0 (993, 26); vslot 0x50 with size 24 | `X2GageUI.cpp:1646-1699` (`SetPos` commented out, skips only `XS_PVP_LOBBY` / `XS_WEDDING_GAME`) |
| `32e672c0` (vslot 0x50) | base `32e661d0`, then `32e5d0a0` (Hero-dungeon BTI 0x22e-0x231 sound + UI particles) | none |
| `32e661d0` `CX2StatusGageUI::SetBuffIconStatic` | step `-( size + 1 ) * index`; debuff y `size + 1 + 4`; tooltip +140; **two strings: 0 = duration, white (12, 16); 1 = counter, yellow (1,1,0,1) at (0.5, -2)**; an existing icon refreshes its duration through `3227bfa0` then vslot 0x48 | inside `:773-1126` |
| `32e64530` `PushBuff( icon, bDebuff, count )` (no Ghidra function; `disasm_range.py`, `ret 0xc`) | new -> vslot 0x3c, known -> 0x44, then always vslot 0x30 = `32e60480` | March push (no count) |
| `32e60480` | counter: string 1 = `""` if count < 2, else `GET_REPLACED_STRING( STR_ID_30715, "i", count )`; string fields +0x58 = 0.5, +0x55 = 1 | none |
| `32489810` `CX2GageManager::PushBuff( uid, icon, bDebuff, count )` | funcmap 5746 "differs from March" | `X2GageManager` push |
| `3236a430` | stack-count tail: BuffIcon ctor `322ab560` -> `324943c0` -> `32489810` -> `32580100` | funcmap 705 / 728 "NOT PORTED" |
| `32e68660`, `32e68de0` | the other two `DLG_BuffManager` creators: party / PvP `UpdateBuffIcon` [I] | `:3096`, `:3488` |
| `32e608f0` (+ `32e5cb30`) | `SetDurationTime`; already ported (11b) | `:1773-1834` |
| `32e70960` | `SetBuffIconGuideDesc` | `:1704` |

Open for H0:
- the text of STR_ID_30715 (the counter format);
- the March names of raw states 7 / 0x16 / 0x1a / 9 / 0x12 (June's enum is shifted: funcmap calls 0x12 `XS_TRAINING_GAME`, March's is 17);
- the string fields +0x4c / +0x50 / +0x55 / +0x58;
- whether June shows a timer or counter on other units' icons.

### Quick slot: what March draws today

`CX2UIQuickSlot` (`X2UIQuickSlot.{h,cpp}`) [D]:
- **Dialogs:** opens `DLG_UI_Quick_Slot.lua` and `DLG_UI_QuickSlot_HighLight.lua` (ctor `:14-77`). The live data034 copy is March's bytecode (8,278 B).
- **Slots:** 6 slots (`MAX_QUICK_SLOT_NUM`; 4 card slots in the defence dungeon), each 40 x 40 at `( 9 + 46 i, 711 - 20 )` (`:2123-2129`, `:2222`). The -20 is
  `INT_WIDE_BAR`'s `MOVE_BAR_Y` (`Always_Global.h:240`), so the bar sits bottom-left.
- **Cooldown** (`OnFrameMove_CoolTime` `:2541-2646`) shrinks picture 1 and writes a yellow `CoolTime` text. It is saved to and restored from the gage manager
  (`:2646`, `:2008`).
- **Keys:** `GA_QUICKSLOT1..6` (`:2338-2370`), labels at `:2024-2102`. Use sends `EGS_USE_QUICK_SLOT_REQ` (`:847`).
- **Data:** reads `ST_E_QUICK_SLOT` (`:1238`, `:2132`); the card slots read `ST_E_DEFENCE_QUICK_SLOT` (`:236`).
- **Created** by `CX2UIManager` (`X2UIManager.cpp:198-200`, `:3699-3703`). **Shown / hidden** from `X2StateMenu.cpp`, `X2StateDungeonGame.cpp` and
  `X2UIDungeonResultAdapter.cpp:883`.
- **Flags already in it:**
  - `SERV_IRUHADEV_QUICK_SLOT_FULL_FREE` (`:1819-1844`; **stays**, user 2026-10-31);
  - `JUNE_TUTORIAL_MODE` (`:1753-1759`);
  - `JUNE_BUFF_QUICKSLOT_ITEM` (`:471-715`);
  - `JUNE_SOUND_AUTO_RELEASE`, `JUNE_RANDOM_MISSION`.

### Quick slot: June [D unless marked]

- **June's file** `june_src_fixed/data034_kom/DLG_UI_Quick_Slot.src.lua` (10,270 B, no `SetPos`):
  - 6 frames `DLG_UI_Common_Texture64_NEW` / `QUICK_SLOT` at x 6..236, y **708**;
  - card slots at y 681 (`Texture64_NEW` / `Texture63_NEW` `DARK_CARD_QUICK_SLOT`);
  - `Static_QUICK_NUM` "1".."6" at y 711;
  - `Slot_Black`, `Button_Buy_Guide_Quick_Slot` and `LOCK` overlays;
  - a June-only **Amusement** block (`AMUSEMENT_MODE_QUICK_SLOT_X_POS` / `_Y_POS`, `SLOT_AMUSEMENT` on `DLG_UI_Image_02`, `DescriptionCheckBox`, `BG_EXPLAIN`,
    STR_ID 40550-40554).
  - March's file uses `MOVE_BAR_Y`; June's does not.
- **`DLG_UI_QuickSlot_HighLight`** (y 723): June adds `SetDisableUnderWindow`. `DLG_Dungeon_Game_QuickSlot_List` / `_Empty` are the same as March's.
  `DLG_UI_PVE_Quick_Slot` (Erda growth-point buttons) has no exe string: out.
- **Functions:**
  - ctor `3294dd50`;
  - `SetSlotItemIcon` `3294d320` and `SetSummonMonsterItemIcon` `32950060`: 711.0 / 686.0 with **no -20**, and the June-only control name
    `DLG_UI_Quick_Slot_Item_%d` (`334c93a0`);
  - `SetEnable` `3294c460`: only the tutorial term is ported (14c); its `+0x100` / `+0x101` slot-count modes and unit `+0xdba` are not;
  - `SetShowUI`'s quick-slot term `3266d250`;
  - the gage cooltime trio `324943c0` / `32489e30` / `3248ac30` (the last two unnamed);
  - the PvP item bar `329515f0` (`+0x101`; no PvP offline);
  - `32597410` (a name table; guide or tutorial [I]).
- **Already ported, not redone:** `KeyEventProcess` `32951670`, `32951420`, `322fa960` (buff 7d), `322f5140` (7e), and `3294e290` / `3294ea00` / `32952930` /
  `32951f70` (C9h-7a).

## Scope fence

**In this plan:**
- **Buff row:**
  - deleting `SERV_IRUHADEV_BUFF_DURATION_TEXT`;
  - June's `UpdateBuffIcon` anchor and state list, `SetBuffIconStatic` with its two strings, and the Hero-dungeon tail `32e5d0a0`;
  - the counter (`32e64530`, `32e60480`, `32489810`, `3236a430`);
  - the party / PvP `UpdateBuffIcon` if June's differ;
  - June's other-unit timer behaviour, whatever H0 finds, "none" included.
- **Quick slot:**
  - June's `DLG_UI_Quick_Slot.lua` (and `_HighLight` if it differs) and its textures, byte-exact;
  - June's ctor, the two icon fills, the rest of `SetEnable`, and `3266d250`;
  - the cooltime trio, if H0 shows it is the quick slot's store.

**Drawn as June leaves it, logs `STUB` if it needs a missing system:** the Amusement block (`[JUNE_HUD] STUB <name> -> Stage 9`).

**Not in this plan:**
- the PvP item bar `329515f0` (no PvP offline; recorded);
- `DLG_UI_PVE_Quick_Slot`;
- the rest of `SetShowUI` (shop early return, minimap, guide gates: Stage 9);
- the skill slot bar;
- the HP / MP gauges and portrait (Stage 9 / june-transcend);
- `QUICK_SLOT_FULL_FREE` (stays on).

## Rules

- **Branch and flags:** branch `main`. One flag per logical change in `KTDXLIB/Always.h`, original code in `#else`.
- **Editing:** `X2GageUI.*`, `X2GageManager.*`, `X2Buff*`, `X2GameUnit.cpp`, `X2PremiumBuffManager.cpp` and `X2UIQuickSlot.*` are CP949: `bytepatch.py` only. One-line
  comments; file-local constants in `_CONST_<CLASS>_H<n>_`.
- **Naming:** June's body goes inside March's own function; a function March lacks gets the class's naming pattern, never `June`.
  `junediverge.py --list` before transcribing; `--check` PASS.
- **No enum literals:** states and BTIs by name, registered under `JUNE_ENUM_NAMES` if C++ lacks them.
- **June files:** in through `luasrc.py adopt-june` and `kom.py stage-june`; never edited. Quirks are kept verbatim with a `JUNE_BUGS.md` row.
- **H1 is a deletion**, the one exception to insert-only. It is proven equal to flag-off with `flagcheck --base HEAD`.
- **Packing:** `/elstools pack`; **ask before every `/x2-kom-packed copy`**. Hand-offs: dev console `/additem`, copy-ready (CLAUDE.md).

## Starting context

Tools only: `python tools/build.py --deploy` (background), `flagcheck.py`, `bytepatch.py`, `docsection.py`, `logs.py --tag JUNE_HUD`, `exestr.py`, `luasrc.py`,
`kom.py`, `elsdb.py`, `kim.py`, `junediverge.py`. June: Ghidra (`.claude/skills/ghidra`; `32e64530` has no Ghidra function: `june2017_re/disasm_range.py`),
`june_src_fixed/`. Funcmap rows are tagged `(HUD H<n>)`.

## Phase H0 - Identify, read-only (Opus 5.5, high)

**Run with:** `/x2-phase june-hud H0`. No code, no build. Delegate pulls to Sonnet subagents; keep the readings.
1. **Buff row:** every buff-row address in the Context table, plus `324943c0` and `32580100`.
   - Map raw states 7 / 0x16 / 0x1a / 9 / 0x12 to March `X2_STATE` names.
   - Decode string fields +0x4c / +0x50 / +0x55 / +0x58.
   - Read STR_ID_30715's text from June's and the live string table.
   - Settle `32e68660` / `32e68de0`: are they party / PvP, and does June draw a duration or counter on other units' icons? That answer is what replaces the mod for
     party icons (user, 2026-10-31: do what June does, even if that is nothing).
   - List every March caller that must pass the count: the mod's five push sites, and the `3236a430` path.
   - Pick a stackable play-test buff item (`kim.py`).
2. **Quick slot:** `3294dd50`, `3294d320`, `32950060`, `3294c460`, `3266d250`, `324943c0` / `32489e30` / `3248ac30`, `329515f0`, `32597410` against `X2UIQuickSlot.cpp`.
   - Who drives the Amusement controls?
   - Is the cooltime trio the quick slot's store (`:2646` / `:2008`)?
   - How do June's `LOCK` / buy-guide controls meet `QUICK_SLOT_FULL_FREE`?
   - Run `kom.py stage-june --dry-run` on both dialogs' textures, and check `_HighLight` against March's.
3. **Write-up:** verdicts in `june2017_re/hud_functions.tsv` (`same` / `differs (<what>)` / `stub -> <owner>` / `out (<why>)`), funcmap rows `(HUD H0)`,
   `junediverge.py --list` hits, and the H0 Done note (<= 60 lines). Correct H1-H4 (sections and kickoffs) if the readings move them.

**Gate:** every tsv row has a verdict; every June function above has a March counterpart or "none (why)".

**Done (2026-10-31, Opus 5.5, read-only; no code, no build).** Verdicts for 33 June functions in `june2017_re/hud_functions.tsv`; 16 funcmap rows `(HUD H0)`;
10 `replace` rows in `tools/data/june_march_divergent.tsv` routed to H2 / H3 / H4 / Stage 9 with `* *` allow rows (row 254 `PushBuff` corrected and flipped to
`replace`). `junediverge.py --check` on the five files H2-H4 touch: `RESULT: PASS` (9 allow-listed); `--selftest` PASS.
- **States:** June's raw ids are June's enum (shifted after 14): 7 `XS_PVP_LOBBY`, 0x16 `XS_WEDDING_GAME` (March 25), 0x1a `XS_PHOTOZONE_GAME` (no March
  member: that skip is dead here), 9 `XS_PVP_GAME`, 0x12 `XS_TRAINING_GAME` (March 17).
- **Strings:** STR 30715 = `x@1` (so "x2"); the live `general.ess` is June's (md5 `1c039a0b`). String ids stay numbers. UIStringData +0x4c `sortFlag`
  (DT_RIGHT), +0x50 `fontStyle` (FS_SHELL), +0x55 `bSpread`, +0x58 `fSpreadTimeGap` (0.5), +0x5c / +0x60 spread counters: the counter types itself in.
- **Party / PvP (the mod's replacement):** both differ (dialog y 155 + 44 idx, March 125). Their `SetDurationTime` slot is the empty base, so June draws **no
  timer** on other units' icons; it **does** draw the stack counter there (same slot-12 setter, and `CX2GageManager::PushBuff` forwards the count to every
  gage). After H1 the party icons show nothing until H3 adds the counter.
- **`SetBuffIconStatic`:** string 0 is March's duration string unchanged; string 1 is the counter (yellow, (0.5, -2)). An existing icon refreshes its time
  through `SetDurationTime( BTI, CRT( GetMyUnit()->GetBuffTimeFinalizerDuration( BTI ) ) )` (CRT = IAT `333bc928`, ceil or floor: H2 resolves). Icons are
  always shown in `VMI_SPA_RUBEN` / `VMI_SPA_RANOX` (`g_pData +0x268` = `CX2LocationManager`, +0x64 = village).
- **Push sites (H3):** June's six callers of `32489810`: `3229dee0` (`X2GameUnit.cpp:14109`), `322a6d00` (`:16748`), `326c9530` (`X2PremiumBuffManager.cpp:786`,
  a record byte), `3236a190` (= the tail of `SetFactor` (`X2BuffTemplet.cpp:275`) and of `RestartDisplayerForAccumulationDecrease`), `3236a430`
  (`SetAccumulationCountNow`), `3236a2f0` (June-only, Ara skill code: checklist 7at). `326c9030` (in-room, `X2PremiumBuffManager.cpp:908`) is not a caller.
- **Tooltip:** `SetBuffIconGuideDesc` `32e70960` differs (in-house id form, PC-room / `BTI_NONE_INT` time lines, no Key line): added to H2.
- **Quick slot:** the "cooltime trio" was a misreading: `324943c0` = `CX2GageManager::GetInstance`, `32489e30` = June's `CX2GageManager::SetShow( b, bForce )`
  (Stage 9), `3248ac30` = `SetShowBuffIcon` (same). The cooldown store stays March's `SetMyQuickSlotCoolTimeList`. `3266d250` is a boss-raid term (out).
  There is no `MOVE_BAR_Y` symbol: the -20 is `INT_WIDE_BAR` (`Always_Global.h:240`).
- **Amusement:** driven only by the PvP item-bar mode (+0x101) in June's `SetShow` `32951900` and `329506d0`; offline it stays hidden. The one live piece is
  `DescriptionCheckBox`, checked from a June game-option byte (`g_pMain +0x2a8`) March lacks.
- **`QUICK_SLOT_FULL_FREE`:** lives in `SetExpandQuickSlot` (`LOCK` / buy-guide). June's `SetEnable` reads the unit's expand flag (+0xdba = March
  `IsExpandQuickSlot()` [I]), as March's does; H4 keeps today's behaviour.
- **Assets:** the 8 textures both quick-slot dialogs use are identical in March (`stage-june --dry-run`: 0 new, 0 replaced). `_HighLight` differs by one line
  (`SetDisableUnderWindow(false)`): adopt it too. Hero-buff oggs (`Hero_Dungeon_Buff_*.ogg`) are not in March: H2 stages them.
- **Play-test buff (H3):** no stacking consumable works offline (78978 Power-up Potion is a PvP amusement item). Use Sheath Knight's passive Brutal Slayer
  (skill 1411 `SI_P_ETK_BRUTAL_SLAYER`, `BTI_SI_P_ETK_BRUTAL_SLAYER`, `ACCUMULATION_LIMIT 5`, a stack per 5 combo hits, 7 s; `BattleFieldBuffTemplet.src.lua:868`).
- **Kickoffs moved:** H2 runs on Opus (party / PvP differ, and the tooltip callees still need reading); H2 / H3 / H4 sections below are rewritten.

## Phase H1 - Delete the buff-duration mod (Sonnet 5.5, medium)

**Run with:** `/x2-phase june-hud H1`. Depends on H0 (it settles what party icons show afterwards).
1. **Re-grep** `BUFF_DURATION_TEXT` over `X2Lib`, `KTDXLIB`, `X2` (comments and `InHouse*.h` included).
   - Sites from planning: `X2GageUI.h:79-91,314-316`; `X2GageUI.cpp:796-801,1088-1125,2940-2942,3461-3463`; `X2BuffFinalizerTemplet.h:55-58,142-144`;
     `X2BuffTemplet.h:185-188`; `X2BuffTemplet.cpp:268-274,621-636`; `X2GameUnit.cpp:14108-14114,16747-16753`; `X2PremiumBuffManager.cpp:785-791,907-913`.
2. **Delete** with `bytepatch.py` `replace`, at each site:
   - an `#ifdef` body without an `#else` goes;
   - an `#ifdef` / `#else` pair keeps only the `#else` body;
   - an `#ifndef` keeps its body.
   - Then delete the `Always.h` house block and its `#define`.
3. **Proof:**
   - `python tools/flagcheck.py BUFF_DURATION_TEXT --base HEAD <the files>`: every file PASS, `Always.h` DEFINE.
   - `git diff --stat`: deletions only.
   - `bytepatch.py info` per file: encoding and EOL unchanged.
4. **MODS.md:** delete the flag's entry, table row and "Rebuild requirements" mention. Add one line: "`SERV_IRUHADEV_BUFF_DURATION_TEXT` (2026-08-25) deleted
   2026-10-31 by june-hud H1; June's buff row replaces it". Correct the stale `QUICK_SLOT_FULL_FREE` line numbers (`X2UIQuickSlot.cpp:1819-1844`, `Always.h`).

**Gate:** grep count 0; `DISPLAY_BUFF_DURATION_TIME` and `JUNE_BUFF_DURATION_TIME` sites byte-identical. The build may ride H2's. Afterwards party icons show
no time, which is June's behaviour (H0); their counter comes with H3.

**Done note (H1, 2026-10-31, Sonnet 5.5):**
- `SERV_IRUHADEV_BUFF_DURATION_TEXT` deleted at all 15 sites in 7 files plus its `Always.h` house block and define: flag-off code kept, nothing else changed (121 deletions, 0 insertions).
- `flagcheck.py BUFF_DURATION_TEXT --base HEAD`: `files: 6 PASS, 1 PASS~ (X2GageUI.h, one blank line), 1 DEFINE, 0 FAIL, 0 ERROR` (its last line says FAIL only because the define is gone, as intended).
- Source grep of `BUFF_DURATION_TEXT`, `GetRemainDurationTime`, `UpdateBuffDurationText` over `X2Lib`, `KTDXLIB`, `X2`: 0 (hits left are build artifacts: `.map`, `.pdb`, `.pch`).
- `junediverge.py --check`: `RESULT: PASS`. Encoding and EOL of all 8 files unchanged (`bytepatch.py info`).
- `MODS.md`: table row and "Rebuild requirements" mention removed, one deletion line added, `QUICK_SLOT_FULL_FREE` rows corrected (`Always.h:3463`, `X2UIQuickSlot.cpp:1819-1844`).
- One missed dependency: `GetBuffTimeFinalizerDuration` (June 3227bfa0, Eve passive) called the deleted `GetRemainTime()`; first build failed at `X2GameUnit.cpp(14738)`. It now calls a new `CX2BuffTimeFinalizerTemplet::GetDurationTime()` (same `m_fDurationTime`) under `SERV_IRUHADEV_JUNE_CHAR_EVE_PASSIVE`.
- `DEPLOY OK: X2_offline.exe ... md5 c4686fcbe0db0e3c56b6bb2a4d6ccdbe` (built at 553ec094 + uncommitted files, peers' `X2UIQuickSlot` included). Party icons show no time now (June's behaviour); the counter comes with H3.

## Phase H2 - Buff row layout and two strings (Opus 5.5, high)

**Run with:** `/x2-phase june-hud H2`. Depends on H1. Flag `SERV_IRUHADEV_JUNE_HUD_BUFF_ROW`. Verdicts: `june2017_re/hud_functions.tsv`.
- **`CX2MyGageUI::UpdateBuffIcon` `32e67050`:** skip `XS_PVP_LOBBY` and `XS_WEDDING_GAME` (June's third state, 0x1a photozone, has no March member: one-line
  comment); `SetPos( GetDummyPos( 1 ) )` in `XS_PVP_GAME` / `XS_TRAINING_GAME`, else `GetDummyPos( 0 )`, (0,0) when the dummy is missing; size 24.
- **Vslot 0x50 `32e672c0`:** the base `SetBuffIconStatic` then the Hero tail `32e5d0a0` for my gage only (party / PvP call the base). Register `BTI_BUFF_HERO_STOP`
  558 / `_UNLIMITED_HP` 559 / `_UNLIMITED_MP` 560 / `_COOLTIME_FREE` 561 under `JUNE_ENUM_NAMES` (`X2BuffFactor.h`); `kom.py stage-june` the four
  `Hero_Dungeon_Buff_*.ogg`, and check the eight UI particle names exist in March's UI particle tables.
- **`SetBuffIconStatic` `32e661d0`:** June's whole body: string 1 (counter, empty until H3 fills it), the existing-icon `SetDurationTime` refresh (settle the CRT
  import at IAT `333bc928` first), and the spa-village `SetShow( true )` (register `VMI_SPA_RUBEN` 1500 / `VMI_SPA_RANOX` 1501 if C++ lacks them).
- **Party / PvP `UpdateBuffIcon` `32e68660` / `32e68de0`:** June's y 155 + 44 idx (PvP: 125 + 30 on create). No timer on their icons (June's slot is empty).
- **`SetBuffIconGuideDesc` `32e70960`:** June's whole body; read its callees first (`31e71c10` id form, `326c8280` / `326c86b0` + `326c55f0` PC room,
  `32e6f890` trade block, `32e5d050` `BTI_NONE_INT` 10000, `32e71a80` icon copy). A callee needing a missing system is a labelled stand-in.
- Update the five `replace` rows routed to H2 (`june_march_divergent.tsv`) and their allow rows to "ported under `SERV_IRUHADEV_JUNE_HUD_BUFF_ROW`".
- One `[JUNE_HUD] BUFF anchor <x>,<y> state <name>` line per dialog create.

**Gate:**
- icons start at x 993 and grow left; debuffs are one row below;
- my unit's time text is still June's 11b text, and a re-applied buff refreshes it;
- other units' icons show no time.

**Done note (H2, 2026-10-31, Opus 5.5):**
- Flags `SERV_IRUHADEV_JUNE_HUD_BUFF_ROW` + `SERV_IRUHADEV_JUNE_HUD_DEBUG` (`Always.h:11019` / `:11038`; `#error` guards on `JUNE_ENUM_NAMES`, `RANOX_PORT`,
  `JUNE_CHAR_EVE_PASSIVE`, `JUNE_SOUND_AUTO_RELEASE`, all ON by `cl /P` probe). Code: `X2GageUI.{h,cpp}`; enums `X2BuffFactor.h`, `Enum.h`.
- June bodies, March's in `#else`: `CX2MyGageUI::UpdateBuffIcon` `32e67050` (dummy 0 / 1 anchor every call, logs on create), `SetBuffIconStatic`
  `32e661d0` (now virtual, as June's slot 20: strings 0 and 1, existing-icon `SetDurationTime( BTI, (int)ceil( GetBuffTimeFinalizerDuration ) )`, shown
  in the spa villages), party / PvP `UpdateBuffIcon` `32e68660` / `32e68de0`, `SetBuffIconGuideDesc` `32e70960`, `UpdateTradeBlockDesc` `32e6f890`
  (+ the remain-time string `328651e0` as `_CONST_MY_GAGE_UI_H2_::AppendRemainTimeString`). New `CX2MyGageUI::SetBuffIconStatic` override = `32e672c0`
  (base, then the Hero tail `32e5d0a0`: ogg + 5 `GetUIMajorParticle()` sequences, all 8 present in `UIMajorParticle.txt`).
- **CRT settled:** IAT `333bc928` is **ceil** (8 of 8 neighbouring slots match msvcr100 exports at base 61c00000; funcmap `(HUD H2)`).
- Enums under `JUNE_ENUM_NAMES`: `BTI_BUFF_HERO_STOP` 558 / `_UNLIMITED_HP` 559 / `_UNLIMITED_MP` 560 / `_COOLTIME_FREE` 561, `VMI_SPA_RUBEN` 1500.
- Assets: `kom.py stage-june` the four `Hero_Dungeon_Buff_*.ogg` (4 new, data078); **`PACK OK data078`**. The copy is not done: ask the user.
- Tooltip: June's "[%S]1_2_3" id form is its `GET_STRING` macro (`31e71c10`, ~3000 callers), so March's `GET_STRING` stays. `_IN_HOUSE_` Key and
  `ERASE_BUFF_CHEAT` lines are kept inside June's bodies: a service binary cannot show in-house code. Stand-ins (one-line comments): PC-room text
  `326c8280` (March's `GetPcBangBuffString`) and time line `326c86b0`, and the `BTI_NONE_INT` end-time line `32e5d050` (no PC room / premium end time
  offline). Trade block is never set offline (`Handlers_Unit.cpp:876`), so `UpdateTradeBlockDesc` is ported dormant; `3234c170` = `GetClassLevel`.
- Divergence: rows `SetBuffIconStatic` / `UpdateBuffIcon` / `SetBuffIconGuideDesc` marked ported, their 3 allow rows deleted; new rows
  `UpdateTradeBlockDesc` (replace, ported) and `GetPcBangBuffString` (out, no PC room). The kickoff's "five rows" were these three (one covers 3 VAs).
- No JUNE_BUGS row: nothing June does here looks unintended (PvP's create y 125 + 30 equals its re-place y 155).
- Hygiene: `flagcheck JUNE_HUD_BUFF_ROW --with JUNE_HUD_DEBUG --worktree`: `files: 1 PASS, 1 PASS~ (blank/marker only), 1 DEFINE, 2 OTHER (other flags),
  0 FAIL, 0 ERROR`; `0 deleted line(s), 0 non-ASCII inserted line(s)`; `RESULT: PASS` (OTHER = the enum rows under `JUNE_ENUM_NAMES`).
  `junediverge --check X2GageUI.{cpp,h}`: `RESULT: PASS` (5 allow-listed), `--selftest` PASS; `enumscan --check`: 0 rows.
- Build (PCH auto-touched for `Always.h`):
  `DEPLOY OK: X2_offline.exe is 25,865,216 B, dated 2026-10-31 07:54:48, md5 26e27ce47853fa6bb84d04318156563f`
  `EXESTR OK: 4 of 4 found` (`[JUNE_HUD] BUFF anchor`, `Hero_Dungeon_Buff_STOP.ogg`, `Hero_UiParticle_Action_ADD`, `HeroDungeon_Skill_Bonus`).
- H3 note: string 1 now exists on every icon (empty); H3's slot-12 setter writes it. `SetBuffIconStatic` is virtual now.

## Phase H3 - The buff counter (Sonnet 5.5, high)

**Run with:** `/x2-phase june-hud H3`. Depends on H2. Flag `SERV_IRUHADEV_JUNE_HUD_BUFF_COUNT` (with `#error` if `_BUFF_ROW` is off; prove with `cl /EP`).
- **Gage side:** `CX2StatusGageUI::PushBuff( icon, bDebuff, count )` `32e64530` (March's body, then the counter); the counter setter `32e60480` as a new
  `CX2GageUI` virtual named in the class's pattern (June puts it right after `PushBuff`); `CX2GageManager::PushBuff` `32489810` forwarding the count.
- **Every March caller passes June's count** (`GetAccumulationCountNow`, `JUNE_BUFF_TEMPLET_PARITY`):
  - `X2GameUnit.cpp:14109` / `:14112` (`3229dee0` @3229e0e8) and `:16748` / `:16751` (`322a6d00` @322a6f3d);
  - `X2BuffTemplet.cpp:275`, `SetFactor`'s tail = `3236a190`, which also ends `RestartDisplayerForAccumulationDecrease` (today it pushes nothing); the user-only
    unit vslot 0x788 before the push is empty in the `CX2GUUser` base: check for overrides;
  - `SetAccumulationCountNow` (`3236a430`): add June's icon tail;
  - `X2PremiumBuffManager.cpp:786` (`326c9530` @326c9958: the count is a record byte, read which); `:908` (`326c9030` does not call `32489810`: find how June
    shows the in-room icon before touching it).
  - `3236a2f0` is out (char-port material, `hud_functions.tsv`).
- Update row 254 `PushBuff` and the two `X2BuffTemplet` rows routed to H3, and their allow rows.

**Gate:** count 1 shows no counter; 2+ shows "x2".. in yellow at the icon's top left, typed in; a re-push updates it; party icons show the counter too.
Play-test: Sheath Knight with Brutal Slayer (skill 1411), five-hit combos in a dungeon.

## Phase H4 - The quick slot (Sonnet 5.5, high)

**Run with:** `/x2-phase june-hud H4`. Depends on H0 only (runs beside H2 / H3: no shared file but `Always.h`). Flag `SERV_IRUHADEV_JUNE_HUD_QUICKSLOT`.
- **Assets:** `luasrc.py adopt-june data034 DLG_UI_Quick_Slot.lua DLG_UI_QuickSlot_HighLight.lua` (June's adds `SetDisableUnderWindow(false)`). The 8 textures are
  already identical in March: no staging.
- **June's bodies:**
  - ctor `3294dd50` (`DescriptionCheckBox` checked from June's game-option byte `g_pMain +0x2a8`: a labelled stand-in member, June's default; `Slot_Black` shown;
    `_HighLight` hidden);
  - `SetSlotItemIcon` `3294d320` and `SetSummonMonsterItemIcon` `32950060`: y 711 / 686 without the `INT_WIDE_BAR` -20, item dialogs named
    `DLG_UI_Quick_Slot_Item_%d` (G5's hide-mode presets then find them), the hidden `pStaticSlotDisable`;
  - `SetEnable` `3294c460` whole (the PvP item-bar mode member exists, never set offline);
  - `SetShow` `32951900` whole (Amusement controls follow that mode, so they stay hidden).
- **Out:** `3266d250` (boss-raid), the "cooltime trio" (not the quick slot's), `329506d0` / `329515f0` (PvP item bar).
- **`QUICK_SLOT_FULL_FREE` unchanged:** 6 open slots, `LOCK` and buy-guide hidden; `SetEnable`'s expand test keeps reading `IsExpandQuickSlot()`.
- Update the four `replace` rows routed to H4 and their allow rows.
- **Packing:** `/elstools pack data034`; ask before the copy.

**Gate:**
- frames, icons, key numbers and the cooldown overlay line up at June's y;
- keys 1-6 use potions in a dungeon and a field;
- the cooldown survives a stage change;
- 0 EXCEPTION.

**Done note (H4, 2026-10-31, Sonnet 5.5):**
- Flags `SERV_IRUHADEV_JUNE_HUD_QUICKSLOT` + `_DEBUG` (`Always.h:11005`; H1's commit 6fca3e00 swept the define block and the MODS entry in with it). Code: `X2UIQuickSlot.{h,cpp}` only.
- June bodies, March's in `#else`: ctor `3294dd50` (checkbox from a stand-in byte, highlight `SetDisableUnderWindow( false )`), `SetSlotItemIcon` `3294d320` (y 711, item dialog `DLG_UI_Quick_Slot_Item_%d`, hidden `pStaticSlotDisable`), `SetSummonMonsterItemIcon` `32950060` (y 686; June inlines `SetEnable( true, true )`), `SetEnable` `3294c460`, `SetShow` `32951900`.
- **Found while porting:** `ResetQuickSlotUI` `329517f0` differs (returns while hidden; calls the Amusement fill in the +0x101 mode). Ported whole under the same flag; new `replace` row, done. `SetLayer` (`3294c320`) equals March's FIX_TOOLTIP body: not touched.
- Stand-ins: Amusement labels (g_pMain `+0x7ec` table, ids 0x2f / 0x30) and fill `329506d0` (mode never set offline); the option byte `+0x2a8` is `ms_bAmusementDescriptionChecked`, default false (June's default not found).
- Lua: `adopt-june data034 DLG_UI_Quick_Slot.lua DLG_UI_QuickSlot_HighLight.lua` (2 replaced, backups `kom_backup_stage_june61031_054307`); `luabind.py check`: 28 / 12 methods, 0 unbound, PASS. **`PACK OK data034`** (2 entries changed). The copy is not done: ask the user.
- `QUICK_SLOT_FULL_FREE` untouched. JUNE_BUGS #487 (OR-test null deref, latent).
- Tool: `junediverge.py` row_done also accepts "ported under SERV_IRUHADEV_JUNE_<X>"; 5 rows marked, 3 allow rows deleted, the `SetShow` allow row now parks only `CX2GageManager::SetShow`.
- Hygiene: `flagcheck JUNE_HUD_QUICKSLOT --with _DEBUG --worktree`: `files: 1 PASS, 1 PASS~ (blank/marker only), 0 FAIL`, `RESULT: PASS`, 0 deleted lines; `junediverge --check`: `RESULT: PASS` (6 allow-listed), `--selftest` PASS; `enumscan --check`: 0 rows.
- Build: the first run deployed without the code (X2Lib's PCH stale, exestr MISSING); rebuilt with `--pch force`.
  `DEPLOY OK: X2_offline.exe is 25,862,656 B, dated 2026-10-31 06:13:17, md5 bbd5b5cb81b085110c9e23da93cb4ebb`
  `EXESTR OK: 4 of 4 found` (`DLG_UI_Quick_Slot_Item_%d`, `pStaticSlotDisable`, `DescriptionCheckBox`, `[JUNE_HUD] SLOT`).
- Play-test (after the user's `/x2-kom-packed copy data034`): frames at y 708, icons at 711, key numbers at 711; potions in slots 1-6; keys 1-6; cooldown across a stage change. Log lines: `[JUNE_HUD] SLOT ctor`, `fill i=N dialog=DLG_UI_Quick_Slot_Item_N x=9+46N y=711`, `show=1`, `enable=`.

## Review (Opus 5.5, high)

**Run with:** `/x2-phase june-hud H4 review`. Covers H1-H4.
- Re-read every ported body against the assembly.
- `junediverge.py --check-all`.
- `flagcheck` per flag.
- JUNE_BUGS rows for kept quirks.
- H1's deletion re-proven against its parent commit.

## Phase Z - Close-out (Sonnet 5.5, medium)

**Run with:** `/x2-phase june-hud Z`. MODS.md "June HUD: Stage 37" section, backlog Stage 37 CLOSED line, memory, `_DEBUG` off, STATUS block to the archive.
