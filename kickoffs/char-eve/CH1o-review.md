# June character Eve (Stage 7) - Phase CH1o review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1o review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1o done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (the 38 wraps, the 0x4b0 inline, the 0x860 / reserved-skill overrides and their base hooks), `SERV_IRUHADEV_JUNE_EFFECTSET_FORCE_DOWN_RATE` (new: `X2EffectSet.*` + the eight Eve calls), `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (the `June_ConstructorInit` stores), `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS` (the base `ClearSkillUseReserve`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1o - Done"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH1l - Done (2026-10-21, Opus)" "CH1l review (2026-10-21, Opus): PASS, no code change"
grep -a -E "^0x(56c|584|65c|6a8|6ac|77c|828)	vslot" june2017_re/char_eve/ch1_offsets.tsv
```

## Task

Review against June. **Delegate the mundane reads** (the wrap list against `ch0_vtable_slots.tsv`, fresh listings of `321e1e60` / `321ee600` @321ef300-321ef4c0 / `3299bd70` @3299ca40-3299cb20 to the scratchpad). You decide:

- **Wraps:** each of the 38 calls its own `June_*` member with every argument in order and returns its result; every March body is whole in the `#else`; no `June_*` body calls its own wrapped virtual (CH1o checked: only `June_COMMON_BUFF_FrameMove` -> `CommonFrameMove()` and `June_InitEffect` / `June_InitComponent` -> particle / slash virtuals, all intended). `ParseCommonRandomState` empty under LIVE (Aisha precedent).
- **Constructor:** the LIVE body is `June_ConstructorInit()` alone. Its six added stores: `+0x97b0` (protected 1.0) / `+0x97bc` 0 / `+0x97c0` 1.0 / `+0x97c4` 1.0 from June, `m_fElectronBallRangeRate` and `m_fSummonedLifeTime` as labelled stand-ins. Is any other March member that a still-March path reads left unset (`ch1k_ctor_offsets.tsv`)?
- **Destructor:** `June_Destructor()` alone. Does March's dtor free anything June's does not (Aisha needed `m_pStrongMindData`)?
- **Force-down float:** the trailing param lands in `+0x14c` (@321e2017, ctor 1.0 @321df48f) and is CreateInstance's `fForceDownValueRate` in every branch (pet @321ef3bb, normal / hyper @321ef462; `fReserveTime` = -1.0 `[0x334380f0]`, `fLandPos` 0, `bReserve` 0); the eight Eve calls pass `+0x97c4` with March's defaults for the rest. The pet overload `321e2340` writes `+0x14c` too (CH1o drift note).
- **0x4b4:** `32cb6a20` returns 172 = `ENSI_COMBO_X`, so March's `GetComboXStateID` stays.
- **Hooks:** the 0x860 / reserved-skill base virtuals and calls, now compiled for `EVE_LIVE` without `ARME_LIVE`; `JuneEveIsSkillSlotState` = `X2GUUser.cpp`'s `IsJuneSkillSlotState` tables; base `ClearSkillUseReserve` clears `+0x8ea8` (`31fc9410`), Aisha's override kept (redundant).
- **Routing:** CH1o2 takes 0x3ac (DD* signature, June's GetFinalDamage routing), 0x7b0 (4th arg) and the five June-only slots with March call sites; check the CH1o2 kickoff's table.

Hygiene: `python tools/flagcheck.py <FLAG> --range <CH1o commit>^..<CH1o commit>` for the four flags (LIVE's "not #defined" FAIL is by design until CH1 final); paste the `files:` / `in this flag's files:` / `RESULT:` lines. `enumscan --check X2GuEve.cpp X2EffectSet.cpp X2GUUser.cpp X2Game.cpp`; the `/Zs` probe with and without `/DSERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (CH1o's `zs_probe.py` shape: `census.cl_cmd` + `/c /Zs /EHsc`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1o - Review" into the plan doc under the Done note, update the CH1o row in `STATUS.md`, check that `kickoffs/char-eve/CH1o2.md` still matches, and commit with explicit pathspecs.
