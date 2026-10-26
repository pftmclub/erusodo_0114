# June character Eve (Stage 7) - Phase CH1m review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1m review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1m done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (the two registrars, `CX2GUUser::JuneHyperModeEnd`), `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (commented out in `Always.h` until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1m - Done"
grep -a "Char Eve CH1m" june2017_re/funcmap.tsv        # 5 rows
python june2017_re/char_eve/ch1m_regcheck.py           # must print: matched 771 registrations, 0 problems
```

The code is the `//{{ ... CH1m` block at the end of `X2Lib/X2GuEve.cpp` (`June_InitStateCommon`, `June_InitState`), the `_LIVE` pair in `CX2GUEve::InitState`, two declarations at the end of `X2Lib/X2GuEve.h`, and `CX2GUUser::JuneHyperModeEnd` at the end of the `_CORE` block of `X2Lib/X2GUUser.cpp` (+1 line in `X2GUUser.h`).

## Task

Review against June. **Delegate the mundane reads**: fresh Ghidra decompiles of `329ba9c0`, `329b8030`, `3201b8e0`, `32006c00`, `329cc0e0`, `32cf0fa0` to the scratchpad; re-run `ch1m_regsim.py` on the two registrar decompiles and diff against `ch1m_reg_by_class.tsv`. You decide:

- **The path simulation is right where it is weakest:** the three unit-data `if`s (@312 / @368 / @418), the templet `switch` @995 (default jumps past the trailing call), the `if / else if / else goto` chain @1105-1220, and the joined `&&` test @1233 (19 / 109 only). Check each gate against the listing, not only the decompile.
- **Head and tail:** cut-in index has no Electra (26) case, the sub-index chain, both `0xd` stores = `InitStateID`, `3202b110` + `31fe22d0` = `InitStateCommonRandom`, WIN2 / LOSE2 flags (`+0x97e0` / `+0x97e1` = `m_bIsWinMotion2` / `m_bIsLoseMotion2`, set only, never cleared), tail vslots `0x690` / `0x684( 0 )` / `31fd8f00`. `InitPassiveSkillState` stays March's until CH4a.
- **`JuneHyperModeEnd`:** `3201b8e0` == `32006c00` byte for byte; the `+0x952c` byte = `JuneIsHyperModeBuffPending()`; vslot `0xc0` = `ApplyHyperModeBuff`; `+0x8d44` = `m_iLastStateHyperModeCount`. Hyper End through `329cc0e0` (vslot 0x604) and riding-hyper End through the Aisha-folded `32cf0fa0` (vslot 0x608) both land on it for Eve.
- **Ids:** `ch1m_idmap.tsv` is one-to-one (134 June ids); 72 = `USI_RIDING_START` (March's `GetRidingStartStateID()`), 180 = `ENSI_JUNE_DASH_COMBO_ZX`; numbers stay March's (the pilot's CH1j decision).
- **Left out, with owners:** phoenix 123-126 (PvP), dance 127-129 (Stage 10), riding 93-96 + the `32002f70` frame move + the riding-die end (Stage 17), die `3202a7d0` (runs in `StateChange`), parrying 224-226 (CH1q), recovery 238 (CH4c). The event states 111-116 come from `CommonSkillState.lua` via `JuneInitEventStates`, not Eve's script.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1m commit>^..<CH1m commit>` and the same for `JUNE_CHAR_EVE_LIVE` (its "not #defined" FAIL is by design until CH1 final); paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1m - Review" into the plan doc under the Done note, update the CH1m row in `STATUS.md`, check that `kickoffs/char-eve/CH1n.md` still matches, and commit with explicit pathspecs.
