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

**Gate:** grep count 0; `DISPLAY_BUFF_DURATION_TIME` and `JUNE_BUFF_DURATION_TIME` sites byte-identical. The build may ride H2's.

## Phase H2 - Buff row layout and two strings (Sonnet 5.5, high; Opus if H0 marks the party / PvP creators as differing)

**Run with:** `/x2-phase june-hud H2`. Depends on H1. Flag `SERV_IRUHADEV_JUNE_HUD_BUFF_ROW`.
- `UpdateBuffIcon` `32e67050`: June's state skip list, the dummy-pos anchor (`GetDummyPos`; index 1 in the two named states, else 0), size 24.
- Vslot 0x50 `32e672c0`, with `32e5d0a0` (Hero-dungeon BTIs by name).
- `SetBuffIconStatic` `32e661d0`: string 0 duration and string 1 counter, with June's fonts, colours and offsets.
- The party / PvP creators if H0 says they differ, including June's other-unit timer behaviour.
- One `[JUNE_HUD] BUFF anchor <x>,<y> state <name>` line per dialog create.

**Gate:**
- icons start at x 993 and grow left; debuffs are one row below;
- my unit's time text is still June's 11b text;
- other units' icons do what H0 found.

## Phase H3 - The buff counter (Sonnet 5.5, high)

**Run with:** `/x2-phase june-hud H3`. Depends on H2. Flag `SERV_IRUHADEV_JUNE_HUD_BUFF_COUNT` (with `#error` if `_BUFF_ROW` is off; prove with `cl /EP`).
- `CX2StatusGageUI::PushBuff` with a count (`32e64530`), and the counter setter `32e60480` (a member named from the class's pattern).
- `CX2GageManager::PushBuff` (`32489810`), and the tail `3236a430` with `324943c0` / `32580100` as H0 scoped them.
- Every March caller passes June's applied count: the five former mod sites, using the `JUNE_BUFF_TEMPLET_PARITY` count.

**Gate:** a buff at count 1 shows no counter; at 2+ it shows STR_ID_30715's text in yellow; a re-push updates it.

## Phase H4 - The quick slot (Sonnet 5.5, high)

**Run with:** `/x2-phase june-hud H4`. Depends on H0 only (runs beside H2 / H3: no shared file but `Always.h`). Flag `SERV_IRUHADEV_JUNE_HUD_QUICKSLOT`.
- **Assets:** `luasrc.py adopt-june data034 DLG_UI_Quick_Slot.lua` (and `_HighLight` if it differs); `kom.py stage-june` for its textures (same name -> June's).
- **June's bodies:**
  - ctor `3294dd50`;
  - `SetSlotItemIcon` `3294d320` and `SetSummonMonsterItemIcon` `32950060` (711 / 686, no `MOVE_BAR_Y`; June's control names);
  - the rest of `SetEnable` `3294c460`, and `3266d250`;
  - the cooltime trio if H0 took it.
- **`QUICK_SLOT_FULL_FREE` keeps working inside June's `SetEnable`:** 6 open slots, `LOCK` and buy-guide hidden.
- **Amusement:** as June leaves it outside its mode; a path needing a missing system logs `[JUNE_HUD] STUB <name> -> Stage 9`.
- **Packing:** `/elstools pack data034` (+ texture koms); ask before each copy.

**Gate:**
- frames, icons, key numbers and the cooldown overlay line up at June's y;
- keys 1-6 use potions in a dungeon and a field;
- the cooldown survives a stage change;
- 0 EXCEPTION.

## Review (Opus 5.5, high)

**Run with:** `/x2-phase june-hud H4 review`. Covers H1-H4.
- Re-read every ported body against the assembly.
- `junediverge.py --check-all`.
- `flagcheck` per flag.
- JUNE_BUGS rows for kept quirks.
- H1's deletion re-proven against its parent commit.

## Phase Z - Close-out (Sonnet 5.5, medium)

**Run with:** `/x2-phase june-hud Z`. MODS.md "June HUD: Stage 37" section, backlog Stage 37 CLOSED line, memory, `_DEBUG` off, STATUS block to the archive.
