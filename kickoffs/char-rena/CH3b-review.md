# June character Rena (Stage 7) - Phase CH3b review ( covers CH3a + CH3b )

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH3b review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH3b done (its Done note, commit and the user's play-test + "check logs"); CH3a's play-test logs are already in its Done note |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL` and `..._SKILL_DEBUG` (the debug define retires here: `//#define` in `Always.h`) |
| Log tag | `[JUNE_CHAR]` (`RENA SKILL equip`, `wind blast lock on`, `ace in the hole fire` / `mp refund`, `stigma arrow ... buff factor`, `entangle power rate`), `[JUNE_SKILL]` (`--grep NUM`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH3a - Done (2026-11-10)" "CH3b - Done (2026-11-10)"
grep -n "Rena: CH3a\|Rena: CH3b" JUNE2017_CHARACTER_PORT_CHECKLIST.md
grep -n "Char Rena CH3a\|Char Rena CH3b" june2017_re/funcmap.tsv
```
Plus `june2017_re/char_rena/ch0_skill_listing_rows.tsv` (the 10 class-102 and the 13 class-103 table rows) and `MODS.md` "Stage 7 Rena, CH3a" / "CH3b".

## Task

Check CH3a's and CH3b's code against June from the listings (`python june2017_re/charport_adis.py rena --phase CH3a --out <scratch dir>`, the same for `CH3b`; a listing runs on into the next function: stop at the `int3`; Ghidra read-only for a spot-check).
CH3a wrote 21 function bodies and ten cases; CH3b wrote 29 function bodies (10 inside March's under the flag, 19 new) and 10 cases ( 16 ids, both ids where June has two ), the other 10 of its 39 class-map functions are "= March" with no code (the funcmap rows say which).
Spot-check at least:
- **CH3a:** the list in `kickoffs/char-rena/CH3a.md`'s Done note ( Sharpfall landing start `32c87ec0`, Slide Kick EventProcess `32c96910`, Airelinna FrameMove `32c96aa0`, Nature Force start `32c6c350`, the registration `@32cb15ca`-`@32cb28f9` ), and its play-test logs.
- **Registration `32cb06d0`, CH3b.** Each table name and callback of the 10 cases' rows: Gungnir `@32cb1877` / `@32cb18c1` and the 11 states `@32cb193b`-`@32cb1b92` ( CH1f2's functions, the state ids 284-294 ), Wind Blast `@32cb0eb4` / `@32cb0ee8`, Aero Strafe `@32cb1f90` / `@32cb1fe5` ( the hyper EndFuture is the normal one ), Freezing Arrow `@32cb1e5c` / `@32cb1e90`, Stigma Arrow `@32cb1f29` / `@32cb1f5d` ( the hyper FrameMove `32cacff0` ), Entangle `@32cb2413` / `@32cb2445`, Blast Arrow `@32cb276d`, Cutlass Wind `@32cb27a2`, One Target Shot `@32cb2876`, Ace in the Hole `@32cb292a` / `@32cb2969` and the six states `@32cb29c8`-`@32cb2b19` ( 298 and 300 use the table of 296 ).
- **Wind Blast FrameMove `32c9d7b0`.** `CommonFrameMove` runs before the 1.0 s event; the `CreateInstance` arguments are March's defaults; the lock-on block ( `322d7670` with range ( 0, 1200 ), user uid at `+0x938`, NPC uid at `+0x940`, the random seed at `+0x948` ) and JUNE_BUGS 580.
- **Stigma Arrow `32cacda0` / `32cacff0`.** The argument list ( force-down rate `+0x4050`, `m_fArrowRangeRate`, hit count, hit gap, mesh player index ), the swapped 23211 templet, `PushBuffFactorWithMemoRate` ( `322b5320` + `31fdbfc0` ), the early `return` when `GetUnit()` is null.
- **Entangle `32c9e7b0` / `32c7d5a0`.** Hunter's Ability ( `0x5b70` ) with no PvP test, `SA_DAMAGE_REL`, the memo `603023`; the cancel order dash `0x8c9c`, walk `0x8c98`, skill `0x8c90`.
- **Aero Strafe `32c87060` / `32c6c530`.** No damage effect in FrameMove; the hyper form has no cut-in light.
- **Ace in the Hole.** Start `32c6d8a0` ( the three camera stand-ins ), StartFuture `32c6d8f0`, the four EventProcesses `32c833b0` / `32c833e0` / `32c83410` / `32c83440`, the three wait EventProcesses `32c85430` / `32c85500` / `32c855d0` ( refund x 3 / 2 / 1, the end by `SA_EFFECTIVE_TIME` ), FrameMoveFuture `32c885a0` ( `SA_DEGREE_Z` ), FrameMove `32ca2520` ( clamp to `SA_DEGREE_MAX`, `SetRotateBone`, the effect set per state at 0.2 s, the local rotation ).
- **The "= March" claims.** Every funcmap row marked `= March`, in particular `32c84b50` / `32c84c70` ( Aero Strafe's three speed steps ) and `32c793f0` / `32c79430`.
- **Templet swap.** Every term that reads a class-102 / 103 skill id in the new code reads it through the swap ( `GetEquippedActiveSkillTemplet( id, true )`, `GetNowStateSkillID()`, `GetFutureStateSkillID()` ), never a base id alone.
- **Play-test.** Read `logs.py --grep "RENA SKILL"` for CH3b's set ( the `equip skill` lines per id, `wind blast lock on`, `ace in the hole fire` x3 and `mp refund`, `stigma arrow buff factor`, `entangle power rate` ), `ErrorLog.txt` ( 0 Motion Error / missing mesh or particle ), and the `[JUNE_SKILL]` lines against `MODS.md` "Stage 7 Rena, CH3b".

Hygiene: `flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL --with SERV_IRUHADEV_JUNE_CHAR_RENA_SKILL_DEBUG --range <CH3a commit>^..<CH3b commit>` ( 35f73de4 is CH3a's code commit ), `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GULire_ElvenRanger.cpp`, a one-file `/Zs` probe of the cpp. Retire `_SKILL_DEBUG` (`//#define`; a build with the only change a commented `_DEBUG` define starts but is not awaited).
Fix clear errors in place (bytepatch only). Write "CH3b - Review" in the plan doc (CH3a's findings in it), update the STATUS rows (CH3b done, CH3c0 ready), and check `CH3c0.md`.
