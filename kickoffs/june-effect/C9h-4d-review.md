# June effect completion - C9h-4d review - effect-set delete conditions, the played-handle list, STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-4d review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-4d done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_EFFECT_SET_DELETE` (+ `_DEBUG`) |
| Log tag | `JUNE_FX EFFECT_SET_DELETE` (`python tools/logs.py --grep "JUNE_FX EFFECT_SET_DELETE"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-4d - Done" "C9h-4 - Done"
grep -n "Effect C9h-4d" june2017_re/funcmap.tsv
```

## Task

Review the C9h-4d code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- **`3228a120` is `StopCreateNewEffect`'s walker, not `DeleteEffectSetOnStateEnd`.** Re-read `3228a120`, `3228a200` and `321da070`: the port's
  `JuneStopCreateNewEffectOnStateEnd` must set every bit of the instance's created vector only when `EffectSetData+0x50` is set, then `resize( 0 )` the list. Confirm the
  `+0x84` / `+0x94` pair is `m_vecEffectInstanceCreated` (data pointer / size) by reading `EffectSetInstance`'s ctor `321df230`.
- **`3227a570`**: the push into `+0x3e90` is per condition and before the switch (the port does the same); case 5 is user-only and needs a skill id above 0.
  Confirm no case-1..4 body differs from March's.
- **The parse tail `32024ff0`**: the three byte offsets `+0x8624 / +0x8628 / +0x862b` map to the three keys the port reads (decode the XOR buffers again for the key
  names, the port's strings are `DELETE_EFFECT_SET_ON_STAGE_END`, `DELETE_EFFECT_SET_OFF_RIDING`, `DELETE_EFFECT_SET_ON_SKILL_STATE_END`). The hyper check: `+0x10` of the
  now-state data = `UserUnitStateData::m_bHyperState` (the port reads `m_NowStateData`), and the condition is `== 1` and `HYPER_EFFECT_SET == 0`.
- **The play site `32007570`**: order does not matter (separate lists), but confirm the skill-pair condition (skill id > 0) and that nothing else in the function reads
  the three new bytes.
- **The stoppers**: `3228a190` only from `324d9550 @324d966b` (loop over the user list at the top; the port loops `GetUserUnit( i )` and skips NULL), `3228a3b0` only from
  `3204d120 @3204d330` after `RemoveRidingPet`, `3228a490` only from `32057b80 @320606a2` (a raw caller scan of all three, not only the xrefs Ghidra lists).
  The `+0x93da = 0` store after `3228a3b0` was not ported: confirm what it is (a riding flag the riding pet's `RemoveRidingPet` or `SetRidingOn` already covers in March?).
- **CommonStateEnd order**: June runs `3228a200`, `32276ca0`, `3228a120`; March has the sound stop first (C9h-4a's placement, from the misidentification). Note
  whether swapping matters (it does not: disjoint lists); do not reorder.
- **Reach**: `STOP_CREATE_NEW_EFFECT_WHEN_STATE_CHANGE` in March effect-set data (grep the extracted `*.src.lua` of the March koms) and in the June effect-set scripts that
  will be adopted; the Done note says what the play-test lines showed.
- **Data**: `Enum.lua` in the live `data036.kom` has `DCES_SKILL_STATE_END = 5` (after the user's copy) and is otherwise unchanged vs the pre-copy backup.
- **Hygiene:** `python tools/flagcheck.py JUNE_EFFECT_SET_DELETE --with JUNE_EFFECT_SET_DELETE_DEBUG --range <first C9h-4d commit>^..HEAD`.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Turn `_DEBUG` off in the review commit. Write
"C9h-4d - Review" into the plan doc, update the phase's STATUS.md row, check the next kickoff (`C9h-4e.md` and `C9h-4f.md` exist), and commit.
