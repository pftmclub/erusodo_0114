# June character Eve (Stage 7) - Phase CH1o2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1o2 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1o2 done (its Done note and commit) |
| Flag | five new engine flags, all defined: `SERV_IRUHADEV_JUNE_FINAL_DAMAGE_UNIT_HOOK` (0x3ac / 0x3b0), `..._JUNE_USER_SKILL_ICON_OVERRIDE` (0x56c), `..._JUNE_ALLY_RALLY_USER_GATE` (0x584), `..._JUNE_SUMMON_CRITICAL_OWNER_BONUS` (0x65c), `..._JUNE_USER_SHOWN_SKILL_COOL_TIME` (0x6a8); `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (Eve's overrides and the three March-override wraps); `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (five member renames) |
| Log tag | none (nothing of Eve's runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1o2 - Done (2026-10-27)"
grep -a -E "^0x(56c|584|65c|6a8|6ac|77c|828)	vslot" june2017_re/char_eve/ch1_offsets.tsv
grep -a "(CH1o2)" june2017_re/funcmap.tsv
```

## Task

Review against June. **Delegate the mundane reads**: fresh listings to the scratchpad of `32284aa0` @322857a0-32285840, `328e0aa0`, `328e18d0` @328e1ab8-328e1cb4, `32f10e60` / `32f0e260`, `3275a820` (raw: `disasm_range.py`), `322e07e0` @322e12f0-322e1470, `32001660` / `3298c580`, and the slot values of the 13 RTTI vtables. You decide:

- **Bases:** each matches June's (`33108560` = `ret 0xc` for 0x3ac / 0x3b0 / 0x56c; `31fc8070` = `!arg->HaveThisBuff( 0x3d8 )`, the argument not `this`; `322b5520` = 0.0; `3208e190` = the float argument; `32001660` = March's `CX2GUUser::CallSummonedNPC`).
- **Call sites:**
  - `GetFinalDamage`: AT_SPECIAL -> 0x3b0, the rest -> 0x3ac, after the divide-range scale; is keeping March's AT_NORMAL call for Ara right?
  - icon: the id June passes is the slot's own (`local_80`), and the strings are the same templet's as S7b's `SetItemTex`; are the gates June's?
  - rally: after the LIRE_MEMO test, before the distances; a non-user ally passes.
  - NPC crit: `+0x4d48` = `GetCriticalRate()`, and the owner test is a guard only.
  - cool time: both reads, `> 0.0` (`[0x3342e84c]`).
- **Eve wiring:**
  - every override calls the renamed member with all arguments;
  - March's 4-arg `ModifyFinalDamageByGameUnitType` is empty under LIVE (June has the proc only in 0x3ac);
  - `CallSummonedNPC` -> `June_CallSummonedNPC`;
  - `ApplyAuraAcceleratorBuffToGameUnit` passes index 0 (offline `IsPvpOrPve()` is false). Is any game type that offline uses `GT_PVE`?
- **Routing:** checklist §7ap (Ara / Chung / Lire / Ain / Rose / Lu/Ciel rows from the vtables), the CH4c amendment (`322d9250`, `322d99d0`, both tree members), the backlog Stage 2 `+0x944c` bullet and the accelerator-aura note. 0x77c / 0x828 need no hook.

Hygiene: `python tools/flagcheck.py <FLAG> --range <CH1o2 commit>^..<CH1o2 commit>` for the seven flags (LIVE's "not #defined" FAIL is by design until CH1 final; CORE's deleted lines are the five renames); paste the `files:` / `in this flag's files:` / `RESULT:` lines. Run `enumscan --check` on the 8 code files (the 2 `X2GUUser.h:2537-2538` rows are older than CH1o2), and the `/Zs` probe of the 7 `.cpp` with and without `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (`census.cl_cmd` + `/c /Zs`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1o2 - Review" into the plan doc under the Done note, update the CH1o2 row in `STATUS.md`, check that `kickoffs/char-eve/CH1p.md` still matches, and commit with explicit pathspecs.
