# June character Rena (Stage 7) - Phase CH1n review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1n review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1n done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`; the head of `SetEquippedSkillFuncBySkillSlotIndex`, March's `default:` calling the helper, three new `#error` guards in `Always.h`), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`SetPowerAttackSkillFunc`, `.cpp` + `.h`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1n - Done"
awk -F'\t' '$7=="CH1n"' june2017_re/char_rena/ch0_class_map.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1n - Review"
```

## Task

Review the head against June, from a fresh decompile of `32cb06d0` and the exe bytes (`june2017_re/disasm_range.py`; tables from the PE
sections). Spot-check at least:
- **Head.** Bounds `0 > slot || 4 < slot`; tables `335153c0` / `335153e8` (normal A / B) and `335153d4` / `335153fc` (hyper) = `_CONST_GUUSER_`;
  `31fd2af0` dropped (7i), `+0x11` / `0x101`; `31fca8e0( id, 1 )`; the memo exit @32cb07cd (`31fdbcc0`, then `32206450( memo, 1 )`).
- **Types.** Byte table `32cb3004` / jump table `32cb2ff4`: 5 buff (FM `329ccd90` -> vslot 0x5f0, Rena's `32c849f0`; EP `31fcbd60`), 6 / 7 / 0xd-0x10
  active, 0xc + 900 throw (`329ccda0` / `329ccdb0`, vslots 0x5f4 / 0x5f8 not overridden), else exit.
- **Base id.** Both switches take `32207210( id )` = `GetUpgradeBaseSkillID` (the C++ switches the hyper part on it; Raven's head used the raw id
  because his June head does).
- **Hyper switch.** `0x5c30 < base` -> 0x5c31 / 0x5c34 Cryotron, 0x5c32 / 0x5c35 Innocent, 0x5c33 -> Spiral, default `@32cb2eef`; `== 0x5c30` Spiral;
  below -> the id switch. Every thunk target = the March callback the C++ names, in the same slot; the stack-struct sub-states 0xf6 / 0xf7 / 0xf8 /
  0xf9 / 0xfb by table name; Innocent's hyper slot gets the EventProcess only.
- **`SetPowerAttackSkillFunc`.** The id-switch default (decompile @911) and the hyper default both land on `@32cb2eef`; the helper = March's
  `default:` case line for line, thunk targets `32c84740` / `32c86b60` / `32c74a40` / `32c847b0` / `32c86b90` / `32c74d50`.
- **`32cb5760`** = March's `RapidShotData::CreateRapidShotData()` (only xref @32cb0e51).
- **Callbacks.** `june2017_re/char_rena/ch1n_user_callback_triage.tsv` (`ch1n_triage.py`): 157 targets, 130 joined by VA to the earlier triages,
  27 `n/a` (left out by CH1m, the shared bounding / wait-state helpers, the vslot 0x600 thunk).
- **Always.h.** `JUNE_SKILL_TYPE`, `JUNE_SKILL_NOTE`, `JUNE_DAMAGE_EXCEPTION` guards name what the head uses.

Hygiene: `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1n commit>^..<CH1n commit> --files
X2Lib/X2GULire_ElvenRanger.cpp X2Lib/X2GULire_ElvenRanger.h KTDXLIB/Always.h` (FAIL only for the commented-out `#define`), `junediverge.py --check-all`,
`enumscan.py --check` on the class files, one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off and with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`.
Fix clear transcription errors in place (bytepatch only). Write "CH1n - Review" into the plan doc, update the STATUS row, check
`kickoffs/char-rena/CH1o.md`, commit with explicit pathspecs.
