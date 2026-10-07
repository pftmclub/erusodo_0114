# June effect completion - C9h-2a - the pet / riding-pet viewer effect sets (port: viewer + engine)

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-2a` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h (identify) |
| Flag | `SERV_IRUHADEV_JUNE_PET_VIEWER_FX` (+ `_DEBUG` only if you need temporary lines) |
| Log tag | `JUNE_FX PET_VIEWER` (`python tools/logs.py --grep "JUNE_FX PET_VIEWER"`) |
| Review | `kickoffs/june-effect/C9h-2a-review.md` (write it when done, Opus 5.5 high) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h - Done"
grep -n "Effect C9h" june2017_re/funcmap.tsv
grep -n "^32959640\|^329594c0\|^329585f0\|^32957260\|^321e1bf0\|^31d38a20" june2017_re/funcmap.tsv
grep -n "^| 10[456] |" JUNE_BUGS.md
```

March files: `X2Lib/X2NPCUnitViewerUI.h/.cpp`, `X2Lib/X2BaseUnitViewerUI.h` (the C9c-2b player `JuneViewerFxPlay/Stop`, read only),
`X2Lib/X2EffectSet.h/.cpp`, `X2Lib/X2PetManager.h/.cpp`. All are CP949: edit with `tools/bytepatch.py`.

## Task

Port June's `CX2NPCUnitViewerUI` changes and the engine pieces they need, **whole** (no partial June updates). Every address,
offset and constant is in the `Effect C9h` funcmap rows; transcribe them, do not re-derive.

1. **Viewer members and lifetime.** Add `std::vector<CX2EffectSet::Handle>` (June `+0x2a8`). The ctor (`32e52dd0`) only constructs it.
   The dtor (`32e52e60`) frees it **without** stopping the handles (JUNE_BUGS 105, keep it).
2. **Frame hooks (June has them, March's NPC viewer does not).**
   - `OnFrameMove` `32e4f600` ends with the base player update (`329585f0` = `CX2BaseUnitViewerUI::OnFrameMove`, `@32e4f75c`).
   - `OnFrameRender_Prepare` `32e4f820` calls `32957260`.
   - `OnFrameRender_Draw` `32e4f8f0` calls no base Draw; keep that.

   Read each June function and place the call where June has it. Without these, group-1 sets never update or render.
3. **New methods.**
   - `IsAnimExist( const WCHAR* )` `32e4fe30`. Map `31d34110` (the skin-anim has-animation lookup) to March's KTDGXSkinAnim equivalent and cite it.
   - The handle stop `32e4fde0`: guarded by `g_pData` and `GetUIEffectSet()`; it stops each handle, sets it to -1 and never erases (JUNE_BUGS 104).
   - June's `SetScale` `32e4fcf0`: March's body, plus each live handle's instance gets `m_vScale = scale` and `m_bDamageEffectScale = false`.
     Confirm the `+0x70/+0x7c` names against March's `EffectSetInstance` members.
4. **`ResetPet( int petId, char iLv, char cVariant = 0 )` = `32e50de0`.**
   - The variant branch `@32e517f1-32e51a6f`, transcribed exactly: the `iLv == 3` test, the templet's 4-entry `+0x6c` vector (identify that member in March's `PetTemplet`), and the `substr` lengths and suffixes.
   - The suffix helper `326cc990` goes on `CX2PetManager`. Confirm `g_pData+0x328` is `m_pPetManager` with a `cl` offset probe.
   - Then stop group 1 (`JuneViewerFxStop( 1 )`) and play `INIT_EFFECT.UNIT_VIEWER_EFFECT_SET_LIST[1..99]` into group 1 (`JuneViewerFxPlay`).
   - March callers keep passing 2 arguments, so `cVariant` is 0 (C9h-2b ports the cash shop's explicit 0). The pet-info variant byte is **Stage 17**: leave one cited comment at `X2UIPetInfo.cpp:1183`. Do not edit that file here.
5. **`ResetRidingPet` = `32e51ea0`.**
   - `RIDINGPET_EFFECT_SET_LIST_BY_UIEFFECTSET` is collected first.
   - `RIDINGPET_UNIT_VIEWER_EFFECT_SET_LIST` plays into group 1 with **no stop** (JUNE_BUGS 106).
   - After the tables close, each collected name goes through the new overload (item 6), and its handle goes into the vector.
6. **Engine.**
   - `CX2EffectSet::PlayEffectSet( const WCHAR*, CKTDGXSkinAnimPtr )` = `321e28d0`, called on `g_pData->GetUIEffectSet()`. It returns -1 on a null templet or skin, and sets the instance's new viewer flag (June `+0xdd`, ctor 0: `321df230 @321df3e3`).
   - The flag's three consumers:
     - (a) GetEffectPosition `@321dd0e0-321dd19e`: the bone pick, `+512`, `(y-384)*-1`, `z=0`;
     - (b) effect creation `@321f2522`: the new particle sequence gets `SetOverUI(true)`, `SetParticleType(PT_2D_PLANE)` and `SetResolutionConvert(true)`;
     - (c) GetEffectPositionByBoneName `@321dd926`. Decide whether it sits inside the empty-name branches March's port omitted (see the comment above that function). Port it if it is reachable; otherwise extend that comment.
   - The `+0xe0` branch right after (a), `@321dd1a3`, belongs to **C9h-3**. Do not port it here.

**Done gate:** the strings `JUNE_FX PET_VIEWER` are in the deployed exe.
- One load line per viewer reset that played something: `JUNE_FX PET_VIEWER reset=<pet|riding> id=<n> group1=<n> uiset=<n>`.
- One line when a variant branch fires (it cannot fire today).

**Reach in March data is 0**, which is expected, so the play-test is a regression check:
- the cash-shop pet and riding-pet previews still show;
- the pet-info pet and riding-pet views still show;
- no EXCEPTION.

Name in the hand-off which preview item ids reach a pet (`PetPreView.lua`).
