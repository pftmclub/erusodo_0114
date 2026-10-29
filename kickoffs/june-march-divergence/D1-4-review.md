# One function per name - Phase D1-4 review (covers D1-3, D1-3b, D1-3c, D1-3d, D1-3e, D1-3f, D1-3h, D1-4 and D1-4b)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence D1-4 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | D1-3, D1-3b, D1-3c, D1-3d, D1-3e, D1-3f, D1-3h, D1-4 and D1-4b done |
| Flag | the `SERV_IRUHADEV_JUNE_DIVERGE_*` flags of D1-3, D1-3b, D1-3c, D1-3d, D1-3e, D1-3f, D1-3h, D1-4 and D1-4b |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Rules" "D1 split (2026-10-29, D1-1 session)"
```

and the Done notes of D1-3, D1-3b, D1-3c, D1-3d, D1-3e, D1-3f, D1-3h, D1-4 and D1-4b (`docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md --list` names them), `june_march_divergent.tsv`.

## Task

Review D1-3, D1-3b, D1-3c, D1-3d, D1-3e, D1-3f, D1-3h, D1-4 and D1-4b: each replaced body against its June listing (Ghidra / `funcdis.py`), `#else` byte-intact, the live March branch (`cl /P`),
one unported call site per row, guards, `MODS.md` / `JUNE_BUGS.md` rows, stand-ins named and routed.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out). Paste the `files:` /
`in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding. Fix clear errors
under the same flag (then `tools/build.py --deploy`); hand design decisions to the user. No play-test (the smoke runs at the plan end).
Write the review section into the plan doc, update STATUS, check `D1-5.md`, commit.

Lead from D1-4b: `32696fa0` is the custom-event manager (funcmap rows 1132, 4330; its `+0x36c` set is `CustomEventScript.lua`'s
`MISSION_QUEST_ID`), so D1-4's helper name `IsFriendshipQuest` for its `+0x510` map (`3267b440`) is unconfirmed. Find the `+0x510`
filler (`32681130` / `32681280`, called from `32694ba0`, a vtable slot at `334b21bc`) and rename the helper if it is not the friendship list.

Leads from D1-3: the Bow pair (`InitEnchantParticleForBow` / `AppendEnchantParticleForBowToDeviceList`) was not compared with June
(rows 261 / 262 named only the Sword pair); `CX2OfflineInventory`'s server-side item count still mirrors March's `GetNumItemByTID`
exclusions (shared bank skipped), which is the emulator's server view, not the client's; confirm it should stay.

D1-3c (2026-10-29) points for the review: `CX2GUUser::CanShowWeaponEffect` (D1-3b) asks the local player's riding (`321f8a40` = June's `GetRidingOn`, matches); the viewer Process's Ara spark group also tests sequences 7 / 8 (`31f9a900` @`31f9af07`): check whether the weapon Process `32292610` does too (D1-3b ports only 5 / 6 / 9); `X2Eqip::SetRenderParam`'s UV speed is left at March US's (R6b owns June's rewrite).

D1-3f (2026-10-30) points for the review: `CX2Data::SetUnitReForm` (June `31ee1f40`) was read from the disassembly by mapping each call's
`[ESP+x]` slot to the lookup that filled it (funcmap `31ee1f40` row); recheck the Dummy2_Lhand <- `Dummy1_Lhand` row and the pelvis `( 1, 1, 1 )`.
`CX2DefaultBoneInfo::AddBoneInfo` returns false on a missing `UNIT_ID` (June's return value is not visible in the decompile). The Lua dev
reload `31d8e660` (auth >= 1, DIK_APOSTROPHE) was not ported; March's viewer keeps its own DIK_R refresh.
D1-3h (2026-10-30) points for the review: `UpdateEquipCollisionData` (`31fd8c00`, Aisha `32cc8d30`) takes an equip's normal anims only for
Add / Rose items (templet `+0xd2`); check that `+0x584` is only Lu/Ciel's minor-unit array (funcmap R14b `3295dfa0`), since that reading drops
every other class's attach-anim equip boxes. `RestoreHead` (`31fde420`) no longer restores `Bip01_Head` before the reform: confirm the
reform's full pass (JUNE_BUGS 477) leaves the head as March's restore did. Lead: June's `ScaleHead` (`31fd64a0`) takes ( bone, head scale,
face scale, body ) with 9 callers, while March's `ScaleHead()` is fixed at 2.0 / 1.8 and has no tsv row; decide whether it is a D1 row or
belongs to the buff owner (Stage 2, `SetSpecificValueByBuffTempletID` `3204b5e0`).
