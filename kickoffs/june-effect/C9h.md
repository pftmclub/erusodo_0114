# June effect completion - C9h - the pet and riding-pet viewer effect sets (identify)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9g (it found the branch) |
| Flag | none (no code; the port phase this one plans gets its own) |
| Log tag | none |
| Review | none (identify only); this phase writes `C9h-2.md` + its STATUS row, or routes the work to a backlog stage |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9g - Done"
grep -n "^32e50de0\|^32e51ea0\|^32228c90" june2017_re/funcmap.tsv
```

## Task

C9g found that June changed two March functions on the NPC unit viewer (`CX2NPCUnitViewerUI`, `X2Lib/X2NPCUnitViewerUI.cpp`, which
derives from `CX2BaseUnitViewerUI` and so already has C9c-2b's viewer effect player):
- **`ResetPet` = `32e50de0`**: after March's keys it stops group 1 (`@32e51cf6`) and plays every name of the pet script's
  `INIT_EFFECT.UNIT_VIEWER_EFFECT_SET_LIST` (1..99) into group 1 on `+0x1e4`.
- **`ResetRidingPet` = `32e51ea0`**: the same with `RIDINGPET_UNIT_VIEWER_EFFECT_SET_LIST`. June's riding-pet scripts also carry
  `RIDINGPET_RESERVED_UI_EFFECT_SET_LIST` (name, index, `RESERVED_EFFECT_SET_PLAY_COND`, `RESERVED_UI_EFFECT_SET_DELETE_COND`) and the
  binary has the key `PLAY_RESERVED_UI_EFFECT_SET`.
- **`32228c90`** (a cash-shop preview, viewer `+0xec`) stops group 1 before calling either.

Identify, in Ghidra, read-only:
1. The full June diff of `ResetPet` / `ResetRidingPet` against March's, key by key (decoded keys: `june2017_re/xorstrings.tsv`).
2. The reserved UI effect-set system: who reads the reserved list, when each condition plays or deletes a set, and its enums
   (`june_src_fixed/data036_kom/Enum.src.lua`).
3. Who owns `32228c90` in March, if anyone (March's cash-shop pet preview).
4. Reach: the June pets that carry these keys (`PET_Santagirl`, `RidingPet_Jet_Ski`, `RidingPet_Lord_Of_Inferno`, `_2P`) are not in
   March's data; count which March pets would gain data from June's pet scripts. The user plans to adopt June's `item.kim`, so judge
   item reach against June's.

Done = funcmap rows (tag `Effect C9h`), the port list in the Done note, and either `kickoffs/june-effect/C9h-2.md` + its STATUS row
(Sonnet 5.5 high, its own flag) or a routing line to a backlog stage for parts that are a missing system.
