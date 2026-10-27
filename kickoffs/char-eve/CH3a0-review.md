# June character Eve (Stage 7) - Phase CH3a0 review - Helpers: UI skill-slot notify, mesh float, modifier tag 15

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH3a0 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH3a0 done (its Done note and commit). No play-test: the code is inert until CH3a / CH3b / CH3d call it |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH3a0 - Done (2026-10-28, Opus 5.5)"
grep -a "Char Eve CH3a0" june2017_re/funcmap.tsv
```
Then the code: `CX2GUUser::JuneRefreshSkillSlotUI` (X2GUUser.cpp), `CX2UISkillTreeNew::JuneMarkEquipSlotUpdated` (X2UISkillTreeNew2015.cpp),
both `CX2EffectSet::AddDamageEffectModifier` forms and `ApplyDamageEffectModifiers` case 15 (X2EffectSet.cpp), and JUNE_BUGS row 453.

## Task

Review against the raw listings (Ghidra `disassemble_function`, read-only). Where the port could be wrong:
- **`31fd0620`**: the order of the three gates (IsMyUnit vt `+0x198`, `g_pData+0x2cc`, `3214f050` result), the swap only when the byte argument is exactly 1 and user `+0x867c` is set, and that the id passed on is the swapped one (`@31fd066a` jumps past the reload).
- **`328da730`**: the slot name (`SlotA` for 0-4, `SlotB` for 5-9, then `i % 5 + 1`), the slot data at control `+0x3c4`, the compare at data `+0xc`, the store at data `+4`, and the early return on the first match. The port tests the dialog once before the loop where June tests the handle each pass: confirm nothing in the loop can change it. The port picks the memorize slot data's id by type; confirm both classes keep the id at `+0x0c` (header comments).
- **Case 15** (jump-table target `321dba36`, table `321dbc60` index 14): no main effect -> skip; add vs multiply by `byte [entry] == 1`; `SetScale` gets the old base (`[ESP+0x48..0x50]` read after `31f8aef0`). Check that `JuneSetScaleBase` / `JuneGetScaleBase` / the multiplying `SetScale` are live in this build (flag `SERV_IRUHADEV_JUNE_SKILL_MEMO_FX`) and that every March `SetScale` caller keeps its scale while the base is (1, 1, 1).
- **`321dad70`**: the dead-handle return and the 32-byte entry `{ b, tag, v, 0, 0 }`; the float form now calls it, and Aisha's two callers (`X2GUArme_VioletMage.cpp`, tags 1 and 2) must build the same entry as before.
- **`31c82df0`**: confirm the claim of no port (one store to `+0x36c`; March `SetNotCheckLandTime` at `KTDGXMeshPlayer.cpp:2973`) and that the CH3a kickoff amendment names the March call at `X2GuEve.cpp:20988-20991`.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <CH3a0 commit>^..<CH3a0 commit>` and `git log -S JuneRefreshSkillSlotUI <CH3a0 commit>..HEAD`; `python tools/enumscan.py --check X2Lib/X2EffectSet.cpp X2Lib/X2GUUser.cpp X2Lib/X2UISkillTreeNew2015.cpp`.

Spot-check 2-3 items directly in Ghidra; if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH3a0 - Review" into the plan doc, update the CH3a0 STATUS.md row, check the next kickoff (`CH3a`), and commit.
