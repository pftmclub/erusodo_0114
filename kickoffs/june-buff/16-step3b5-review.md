# June buff port - Phase 16 step 3b, part 5 - review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step3b5 review` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `mods/june-buff-port`) |
| Depends on | 16-step3b5 done (see its STATUS row for the commit) |
| Flag | `SERV_IRUHADEV_JUNE_SOCKET_PARITY` (`_DEBUG` for the first-apply lines) |
| Log tag | `[JUNE_BUFF]` |

## Read first

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 16-step3b4 -- Review" "Phase 16-step3b5 -- Done"
```

The `funcmap.tsv` rows tagged "Buff Phase 16-step3b5" carry the decodes.

## Task

Review 16-step3b5 against June:
- **Item 0, `3201aff0` / `32000320`.** The set loop's sum (`[esp+0x10]`, from the FLDZ `@3201aff9`), the interval test
  direction (`@3201b087`), the store find / skip / set order (`@3201b0ab`..`3201b11e`: set only when the reduction is > 0),
  the title loop's return value (+ the set sum) and its PVP_ONLY test (`322cc1a0`, JUNE_BUGS #56), and the helper's
  `DUNGEON_ONLY_TYPE` test. Confirm the US build really has no `ADJUST_SECRET_ITEM_OPTION` (`Always_US.h:4`, `cl /P`), which
  the phase found by a failed build.
- **Items 1-2, the loader.** The `"Group 0"` / no-space / last-token-`"0"` skips (`@3272e4e2`, `@3272e4ff`, `@3272e72f`), the
  first-wins inserts (`32737f00`, `32078230` + `32821f20`), `32715680` returning every member (no class filter), the
  converters `327150b0` / `32715110`, and the moved load of `SkillLevelUpSocketGroup.lua` (June `32716860 @32716aff`).
  Re-run the load census against the live data036 and compare with the Done note's numbers.
- **Item 3, `31fe14f0`.** The type x power-rate-type double loop, the `3 -> 2` remap on templet `+0x174`, the both-empty path,
  and the slot loop's place in `SumSocketOption` (`@3204f485..4dd`, original templet). Check `m_eJunePowerRateType` is
  set for every templet the loader builds.
- **Item 4, the reader and its three ported callers.** `3201ad10`'s June-only fallbacks; `SetEquippedSkillLevelCommon`
  (`32024b00 @32024d36/@32024d4d`), Reflex Magic (`@32046bae`, before the reflect-by-buff override), and the summon
  helper's `@322ed2f7` folded into `GetSummonPowerRateBySocket` (same skill as `31fd7cc0`).

Hygiene: `python tools/flagcheck.py JUNE_SOCKET_PARITY --range <3b5 commit>^..HEAD --with JUNE_BUFF_DEBUG`. Paste the
summary lines; explain every deleted line. Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the phase.
Fix transcription errors under the same flag; hand design decisions back. Check `16-step3b7.md` against what you find.
