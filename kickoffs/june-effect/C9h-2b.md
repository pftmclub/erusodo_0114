# June effect completion - C9h-2b - the cash-shop pet preview and the viewer hide sites (identify the rest, then port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-2b` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-2a (the viewer's handle stop `32e4fde0`, IsAnimExist and SetScale must exist) |
| Flag | `SERV_IRUHADEV_JUNE_PET_PREVIEW` (+ `_DEBUG` only if you need temporary lines) |
| Log tag | `JUNE_FX PET_PREVIEW` (`python tools/logs.py --grep "JUNE_FX PET_PREVIEW"`) |
| Review | `kickoffs/june-effect/C9h-2b-review.md` (write it when done, Opus 5.5 high) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h - Done"
grep -n "Effect C9h" june2017_re/funcmap.tsv
python tools/docsection.py JUNE_PORT_BACKLOG_ORDER.md "Stage 12: character customize system (june-effect C9g, 2026-10-07)"
```

March files: `X2Lib/X2CashShop.cpp/.h` (`ResetPetViewer` :12531, `PetPreview` :12505, the no-argument hides :1400 / :3011 / :12503),
`X2Lib/X2UIPetInfo.cpp`. Both are CP949: edit with `tools/bytepatch.py`.

## Task

June rewrote `CX2CashShop::ResetPetViewer` (`32228c90`) and its only caller, `PetPreview` (`32234b10`). It also turned March's three
`ResetPetViewer()` hides into inline blocks that add the viewer's handle stop and a group-1 stop. Port these whole, and port
the pet-info riding view's clears the same way.

1. **Finish the mapping first (Ghidra, read-only):**
   - confirm `3223d680` = SetOpen and `3223c5d0` = UpdateDLGPetViewerUI;
   - map `322383c0` to its March counterpart (`UICustomEventProc` :1400 or June-only; its callers are the customize preview `3224d0f0`);
   - map the pet-info sites `32902000`, `32902700` and `328fce40` (they stop the riding viewer `+0xb8`) to `X2UIPetInfo.cpp`;
   - find every writer of cash shop `+0x51c` (C9h found only the zeroing in SetOpen `@3223e1a3`; look for LEA/copy writes and the members around `+0x518..+0x528`);
   - name `DAT_3379fce4` (Stage 12's customize-preview flag, writers `3224a680` / `3224d0f0`).

   Add funcmap rows, `Effect C9h-2b`.
2. **Port `ResetPetViewer` = `32228c90`:**
   - the `type != -1` guard (June has no hide for -1);
   - the stop and group-1 stop first;
   - `ResetPet( id, 3, 0 )`;
   - the 0.5 / 1.0 / 1.4 scales (pets 35-36: confirm March's implicit enum values with a `cl` probe);
   - `IsAnimExist( L"Wait_Transform" )` with a `L"Wait"` fallback;
   - the position.

   For `DAT_3379fce4`, keep June's default branch (the flag is Stage 12's and stays false; cite it).
   **The one decision that may go to the user:** June positions the preview at `+0x51c + (260,-30,0)`, while March hard-codes `(-270,0,0)` for March's cash-shop dialog. If `+0x51c` turns out to be a June-only dialog position, so that June's offset would misplace the preview in March's dialog, that is "copying June breaks March": stop and ask, with both numbers and a screenshot-free description of where each lands. Otherwise port it.
3. **Port `PetPreview` = `32234b10`:**
   - the item-manager guard;
   - both lookups with the `< 1` tests;
   - the `rand()` odd pick when both a pet and a riding pet exist;
   - the trailing hide when the previous item is no longer in the pre-equip list (`32234380`).

   `32234380` reads June's pre-equip list model (`3265ec70`, keyed by `+0x528`). If March's `m_vecPreEquipItemID` can answer the same question, use it and cite it. If it cannot, that list model is June cash-shop UI (backlog Stage 9): stop and route it rather than inventing a substitute.
4. **The inline hides** at March's three no-argument call sites, and the pet-info clears, as June has them.
5. **The pet-info pet viewer** (`X2UIPetInfo.cpp:1183`, June `328f0aa0`) keeps passing `cVariant` 0, with a comment citing the June
   `KPetInfo +0xca` byte and backlog Stage 17. The cash shop passes June's explicit `ResetPet( id, 3, 0 )`.

**Done gate:** the strings `JUNE_FX PET_PREVIEW` are in the deployed exe; one line per preview, `JUNE_FX PET_PREVIEW item=<n> pet=<n> riding=<n> pick=<pet|riding|none>`.

**Play-test:**
- preview a pet item, a riding-pet item and a non-pet item;
- close and reopen the cash shop;
- open pet info and switch riding pets.

The preview shows, hides and moves as June's code says. The hand-off names the item ids (from March's `PetPreView.lua`).
