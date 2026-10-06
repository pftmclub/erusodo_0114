# June buff port - Phase 16 step 3b, part 7 - the rest of the skill-group power readers, and m_fAllStatRelLV

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b7` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b5 (done: loader, slot pass and reader `GetSkillAdditionalPowerRateJune`); 16-step3b6 and its review done (review `b7dce8b` touched `EraseDebuffAndAddMP` in X2GUUser.cpp) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (same flag; `_DEBUG` for first-apply lines) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | write `16-step3b7-review.md` when done |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-step3b5 -- Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3b5" carry the decode this phase starts from (`3201ad10`, `31fe14f0`,
`32024b00`, `3201aff0`).

## Task

Split off by 16-step3b5 (advisor split by file, 2026-10-06). 3b5 ported the reader `3201ad10` as
`CX2GUUser::GetSkillAdditionalPowerRateJune( skill )` and wired its callers in X2GUUser.cpp / X2Game.cpp. Port the rest,
cite June addresses, byte-level CP949 edits, June's callers take the rate + 1.0 (`334355b0`).

1. **The three Phase 9a placeholders** in `X2BuffTemplet.cpp` (`fJuneSkillDamageAddRate = 0.0f`): Mark of Command
   (`3236df00 @3236e002`, skill 26214), Moonlight Rhapsody (`32370430 @323705a2`, 29024) and `32369bd0 @32369c98`
   (0x7560 = 30048). Each multiplies by `( 3201ad10( skill ) + 1 )` for a user attacker; replace the 0 with the attacker
   user's `GetSkillAdditionalPowerRateJune`.
2. **`32773ee0 @32774310`** (`CX2GUNPC::AttackResultByType`, the Mark of Commander shelling block, skill 0x6666 = 26214):
   find which user `ecx` is (`[esp+0x14]`) and port the term in X2GUNPC.cpp.
3. **`3201f850`** (sets a state's level / power rate from a templet, then `x ( 3201ad10 + 1 )`), called from `32028190`
   and class code (`32968f20` x9 ...): name the March host of `32028190` first; class callers go to checklist section 7 (D8).
4. **The summon helper's flag-0x200 store** (`322ece70 @322ed394`): `3201ad10 + 1` goes to the NPC double `+0x8800`.
   Find its reader first (dispsweep `8800`); port, or leave dormant with one line.
5. **Socket 215434's term in `3201ad10`** (its tail, after the `32358970` term): for a hyper skill (type 13), while a CX2Unit flag
   (`31f4c400` result byte +1) is set, the dungeon type is not 2, the game type is not PvP and `g_pData +0x134 != 0x9c61`,
   June adds the entry of socket 215434's own map (`+0x48`, keys through `328905b0`) that matches the skill. Socket 215434
   (Hyper Active +10 %, "Group 14") is in the staged OptionItemData. Decode the flag and the 0x9c61 test, then port.
6. **`m_fAllStatRelLV`** (`32727838`, templet/aggregate `+0x268`): the loader key, the plain add in `3207cd70`
   (`@3207d3fc`, after the five `KItemStatRelLVData` members) and its reader (start from June's `KItemStatRelLVData`
   users). Port it, or leave it dormant with one line if no reader exists. Reach: 30 June OptionItemData entries.

7. **The per-class hook after the slot loop** (from the 3b5 review). After the `31fe14f0` loop, June's SumSocketOption calls user vslot 485
   (`+0x794`, `@3204f4df..4ed`) with `( aggregate, templet )`; the base `33093ad0` is a bare `ret 8`. Add it to `CX2GUUser` as an empty
   virtual with that signature and call it right after 3b5's slot loop, under the flag. It stays dormant: no March class overrides
   it. The overrides (Lu/Ciel `32b0e670`, Ara `32badc80`) are a checklist §7 row; do not port them here.

A reader that cannot be settled from the disassembly stays dormant with one `[JUNE_BUFF]` line and is listed as open.
