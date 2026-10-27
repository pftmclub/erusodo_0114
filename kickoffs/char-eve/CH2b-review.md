# June character Eve (Stage 7) - Phase CH2b review - Skills: Architecture (19)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2b review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2b done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (+ the temporary `..._SKILL_DEBUG`, retired in this review) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2b - Done (2026-10-27, Sonnet 5.5)"
```
Then the 22 `Char Eve CH2b` rows at the end of `june2017_re/funcmap.tsv` and JUNE_BUGS 450 / 451.

## Task

Review the CH2b transcriptions against the raw listings (`python june2017_re/charport_adis.py eve <va>`; Ghidra has no function for 8 of the 22 addresses, read the listing, not the decompile). The listings the port leans on and where it could be wrong:
- **`3299b810`** (`June_SummonSurfaceCuting`): the three early pushes `3, -1, 0` are the last three `CreateNPCReq` arguments ( createType 3 = `NCT_CHANGESTAGE_REMAIN`, ally team -1, elite 0 ) and vslot 0x54 takes none; key code = the helper's first argument; `0x198` is the gate (`IsMyUnit`), not a callee that eats the unit / level / active pushes. Time = `+0x9c4c` x 60.0 + `+0x4070`, then x `SA 106` only in PvP. `3227b920` / `3227ba10` = `GetBuffFinalizerTempletPtrList` / `ChangeBuffFinalizerTempletPtrList`; the vslot 0x38 call is `SetDurationTime`.
- **`329b2e10`** (Oberon Guard FrameMove): the `322e67c0` call has 19 stack words; the first-pushed three are `0` ( createType NCT_NONE ), the gate word and a trailing `0`; delay `0.3`, key `-1`, `NAT_ALLY`, level 0, active 1.
- **`3298d550` / `3298d710`** (Heaven's Fist) and **`329a99a0`** (Sweeper): `322ceae0` is `CX2Game::GetEffectSet()` (its pushes are `PlayEffectSet`'s); the scale vector is `GetVec3ScaleByUnit` through vslot 0x24 into a local; the trailing `1.0` is the force-down default; `bCustomPosition` is 1 with `GetPos`, `GetRotateDegree`, `GetDirVector`. The Sweeper left branch adds `( 0, 180, 0 )` to the rotation.
- **`329c4380`** (Faena): the loop clobbers `ebp` ( level ) and `edi` ( skill id ) (JUNE_BUGS 451); the SA ids are 31 / 158 / 159; vslot 0x3c8 is `GetIsBosRaidNPC`; the boss branch uses target vslot 0x3e4 and the narrow name at `334d3828`; the other branch is `PlayEffectSet( name, this, target, false, -1, -1, ( 1, 1, 1 ), false, 0, 0, 0 )`.
- **`32974bb0`**: the walk / dash cancels carry the on-line test; the skill cancel is the macro's JUNE_BUFF_AWAKE_LOCK expansion.
- **Registration** (`329c8a00`, `ch0_skill.tsv` lines 446 / 463 / 472 / 485 / 497): `june2017_re/char_eve/ch0_skill.tsv` rows for the ten ids; which tables take `June_CopyStateCallbacks` (Sonic Wave, Oberon Guard) and which list only FrameMove + EventProcess in the hyper table (Surface Cuting, Sweeper); Heaven's Fist's hyper table keeps Init.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <CH2b commit>^..HEAD --with JUNE_CHAR_EVE_SKILL_DEBUG`; `python tools/enumscan.py --check X2Lib/X2GuEve.cpp`. Retire `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL_DEBUG` (its `#define` in `Always.h` and the five log blocks).

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH2b - Review" into the plan doc, update the CH2b STATUS.md row, check the next kickoff (`CH2c`), and commit.
