# June character Rena (Stage 7) - Phase CH1o review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1o review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1o done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`; the 0x318 / 0x1e8 overrides, the `CX2GameUnit` member / getter / reset / 0x1e8 base, the stores and calls in `X2GUNPC` / `X2GUUser` / `X2DamageEffect`, June's `ProcessIKillThisNpc` / `User`, `SetSkillLevelStateData`, one `Always.h` guard), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (`GetRageFungusTargetLockCount`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1o - Done"
awk -F'\t' '$7=="CH1o"' june2017_re/char_rena/ch0_class_map.tsv
grep "Char Rena CH1o" june2017_re/funcmap.tsv
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1o - Review"
```

## Task

Review CH1o against June from fresh decompiles and the exe bytes (`june2017_re/disasm_range.py`, `charport_adis.py rena`; vtables from the PE sections).
Spot-check at least:
- **0x318.** `CX2GameUnit` / `CX2GUUser` / `CX2GUNPC` vtables + 0x318 = `3208ddc0` (`ret 4`), Rena's = `32cb3200`; the `[r+0x318]` call sweep finds only
  `322cf0ee`, `322cf195`, `320149eb`. `32cb3200`'s argument is the killed unit (`mov ecx, edi; call 32cb6dd0`), `32cb6dd0` copies `+0x763c`; the rest
  of the body = CH1h's transcription (triggers 0x25 / 0x26, `SA_EFFECTIVE_COUNT` before / after `++`, the loop effect set).
- **`+0x763c`.** The three stores follow `32076140` (`@3204a1e5`, `@327b4cb6`, `@327b4e95`) and copy the DamageData argument; their March sites
  (X2GUNPC.cpp after both `SetGameUnitWhoAttackedMe( pAttackerUnit )`, X2GUUser.cpp after the owner-credit one) are the matching branches; the six
  `31f8f550` sites follow the observer reset (= the widened inline `ResetGameUnitWhoAttackedMe`). JUNE_BUGS 573 holds.
- **Handlers.** `322cf060` / `322cf100` = the `_LIVE` bodies (tests, packet offsets +0 / +9 and +4 / +0xc / +0x14, game vslots 0xa8 / 0xa0 =
  `GetNPCUnitByUID` / `GetUserUnitByUID`); the three new `june_march_divergent.tsv` rows.
- **0x1e8.** Vtables: `CX2GameUnit` / `CX2GUNPC` + 0x1e8 = `3208ddc0`, `CX2GUUser` = `32011b80`, Rena = `32ca9ca0`; call sites `31f86a92`,
  `320139a4`, `3276a7a8` each follow the `+0x66ac` run with the same DamageData; Rena's base call `@32ca9cc9` comes first.
- **`SetSkillLevelStateData`** = `3201f850` (Raven's review); Rena vtable 0x7a8 = `32028190`.
- **Helpers.** `32c83580` / `32c6dbd0` / `32c6dd10` / `32c6ddc0` = March's four `Append*ToDeviceList`; `32c85b20` = `GetRageFungusTargetLockCount`
  (`0.0 < value` keeps, erase via `32311800`, word at +0x97fc); `31c8a5c0` = `CKTDGXMeshPlayer::AppendToDeviceList` (= March).
- **Routes.** `stage33-plan.md` (Rena block of `3229a530`), backlog "attack-result hooks" (`32011b80`), stage7-plan row 8 (Rena's 0x740).

Hygiene: `flagcheck SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1o commit>^..<CH1o commit> --files
KTDXLIB/Always.h X2Lib/X2GameUnit.h X2Lib/X2DamageEffect.cpp X2Lib/X2GUUser.cpp X2Lib/X2GUNPC.cpp X2Lib/X2Game.cpp X2Lib/X2GULire_ElvenRanger.h
X2Lib/X2GULire_ElvenRanger.cpp` (FAIL only for the commented-out `#define`), `junediverge.py --check-all`, `enumscan.py --check` on the changed `.cpp`
files, one-file `/Zs` probes of the five changed `.cpp` files off and with `-DSERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`.
Fix clear transcription errors in place (bytepatch only). Write "CH1o - Review" into the plan doc, update the STATUS row, check
`kickoffs/char-rena/CH1p.md`, commit with explicit pathspecs.
