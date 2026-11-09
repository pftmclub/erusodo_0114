# June character Rena (Stage 7) - Phase CH1q review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1q review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1q done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (`//#define` in `KTDXLIB/Always.h` and `KncWX2Server/Common/ServerDefine.h`; defines `ADD_RENA_SYSTEM`), `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1q - Done"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1q - Review (2026-10-27)"
grep -n "Rena: CH1q" JUNE2017_CHARACTER_PORT_CHECKLIST.md
```
Plus `june2017_re/char_rena/ch1q_nf_outcome.tsv`, the `Char Rena CH1q` rows of `june2017_re/funcmap.tsv`, JUNE_BUGS 574, and the "Amended by CH1q"
sections of `kickoffs/char-rena/CH1q2.md`, `CH4a.md` and `CH1.md`.

## Task

Check CH1q's code against June from the exe bytes (`june2017_re/charport_adis.py rena`, `funcdis.py`, `dispsweep.py`; Ghidra read-only) and a fresh
`cl /P` of `X2GULire_ElvenRanger.cpp` / `X2GUUser.cpp` / `X2DamageEffect.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE /DSERV_IRUHADEV_JUNE_CHAR_RENA_NF`.
Spot-check at least:
- **AttackResultByType `32ca7130`.** The switch (`js` / `jle 2` / `jne 3`): only `NFT_SPECIAL_ACTIVE` takes the skill path and NFT_ACTIVE falls into the combo
  path; `+0x5a4 == 1` gate; full test against gauge `+0x120`; the 3.0 compare `@32ca73fd` (`test ah, 0x41; jp`: charge >= 3); the combo bomb's relate
  gate; the inlined UpNaturalForce `@32ca75e2` (+1 when `+0x6c50` > roll); `+0x5a4 = -1` only on the combo path.
- **Helpers.** ConsumeNaturalForce `32c9f7a0` (the `0 < +0x97e0` test, vslot 0x9c = IsFullHyperMode, Rena's slot = base `31e4c550` false), SetNaturalForceBuff
  `32c85200` (erase BTI 333 `3229e160` = EraseBuffTempletFromGameUnit, then vslot 0x21c pushes 0x163 / 0 / 1 / 0 / 0 = `( BFI 355, 1, NULL, 0, false )`),
  Set / UnSet `32c85260` / `32c85290` (5 and 822; `count < 1`; `32281440` = ReserveToFinishBuffTempletFromGameUnit), `32c6d4a0` = `ret 4`, `32c89230`
  (vslot 0x198 = IsMyUnit, `+0x92b4`, then `jmp 3204da60`).
- **XX FrameMove `32c8b2c0`** against CH1f's X FrameMove `32c8aa40`: 6.0, the XX arrow name for every class, the three angles, the per-arrow stores.
- **Equal rows.** Pick five `= March` rows of the outcome table (one state start, CreateGageData, Init, SetNaturalForceEffect, the NPC-die relate cleanup
  `3230bc70`) and confirm them from the listing.
- **Flag plumbing.** `_NF` alone stops at the `#error`; `_LIVE` + `_NF` gives `CAC_NATURAL_FORCE` in `X2GUUser.i` (the ServerDefine.h home), no `NFP: %f`,
  no `__forceinline int CX2GULire_ElvenRanger::GetNaturalForceCount`; `X2Define.h` gives 3 per hit; the RT_CREATE_DAMAGE_EFFECT arm runs the copy, SetScale,
  InheritFromParent and ApplyCreateDamageEffectRate in June's order (`31f860fd-31f86148`).

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_NF --range <CH1q commit>^..<CH1q commit> --files <its code files>` (FAIL only for the commented
`//#define`), `junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`, one-file `/Zs` probes off and with `_LIVE` + `_NF`.
Fix clear errors in place (bytepatch only). Write "CH1q - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1q2.md`, commit
with explicit pathspecs. No play-test: full build skipped.
