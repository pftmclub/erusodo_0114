# June character Rena (Stage 7) - Phase CH1q2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1q2 review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1q2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (`//#define` in `KTDXLIB/Always.h` and `KncWX2Server/Common/ServerDefine.h`; defines `ADD_RENA_SYSTEM`), `SERV_IRUHADEV_JUNE_CHAR_RENA_NF_DEBUG` (`//#define`), `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1q2 - Done (2026-11-09)"
grep -n "Rena: CH1q2" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Rena CH1q2" june2017_re/funcmap.tsv
```
Plus `june2017_re/char_rena/ch1q2_nf_outcome.tsv`, the `CX2RenaMyGageUI::InitUI` row of `tools/data/june_march_divergent.tsv`, and the "Amended by CH1q2"
section of `kickoffs/char-rena/CH1.md`.

## Task

Check CH1q2's code against June from the exe bytes (`june2017_re/funcdis.py`, `charport_consts.read_va`; Ghidra read-only) and a fresh `cl /P` of
`X2GageUI.cpp` / `X2GameUnit.cpp` / `X2Game.cpp` / `Offline/X2OfflineStatTable.cpp` with `/DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE /DSERV_IRUHADEV_JUNE_CHAR_RENA_NF
/DADD_RENA_SYSTEM`. Spot-check at least:
- **InitUI `32e6e190`.** `32e66e60` (InitStatusUI), vslot `0x54` (InitWakeOrb; Chung's row `32e6d6c0`), then `31cd5910` with `335347e8`
  (`DLG_Rena_NaturalForce.lua`), `31cd3f20` with `335322c8` (`g_pStatic_NF_Gage_Back`), vslot `0x68( 1 )`: the `_NF` arm in that order.
- **ApplyHyperModeBuff `32298590 @322985cd-32298632`.** `+0x264` user gate, `+0x867c` / `+0xaf0` / type 3, `32cb50c0` count, `jle` skip, vslot `0xf0` (Rena vtable
  `335185bc` + 0xf0 = `3208dd00` UpNowMp) with count x the double at `33449130` (15.0), then `push 5; call 32cb6290` whatever the count; placement before the
  transcendence test; Rena's vslot 0xc0 = base.
- **Equal rows.** Pick five `= March` rows of the outcome table (SetNowNaturalForce `32a36df0`, SetMyPlayStatusFromPacket `32a34fa0`, UpdateNaturalForceUI
  `32e5ec10`, the NPC branch `32773ee0 @3277452b`, InitWhenGameIsOver `32a34f80`) and confirm them from the listing.
- **Dev key.** June's KeyProcess `322f7170`: no auth-level F7 fill (its `push 0x41` sites are the chat-macro keys and the training block `@322f7b6e`);
  the `_NF` arm in `X2Game.cpp` drops only Rena's fill.
- **Emulator.** The `UT_LIRE` case matches `GSGameDBThread.cpp:989-993`; the save path (`Handlers_Field.cpp` SaveGamePlayStatus, `X2OfflineServer.cpp`
  `m_iCharAbilCount = kRow.m_iAbilCount`) carries the count with no schema change.
- **Debug flag.** `_NF_DEBUG` alone stops at its `#error`; with `_LIVE` + `_NF` + `_NF_DEBUG` the 5 files compile; every line sits inside `_NF` code.

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_NF --range <CH1q2 commit>^..<CH1q2 commit> --files <its code files>` (and `_NF_DEBUG`; FAIL only for
the commented `//#define`s), `junediverge.py --check-all`, `enumscan.py --check <the 5 files>`, one-file `/Zs` probes off and with `_LIVE` + `_NF`.
Fix clear errors in place (bytepatch only). Write "CH1q2 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1.md`, commit
with explicit pathspecs. No play-test: full build skipped.
