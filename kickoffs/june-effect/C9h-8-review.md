# June effect completion - C9h-8 review - Damage-effect sound runtime

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-8 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-8 done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_DAMAGEEFFECT_SOUND` (+ `_DEBUG`, turn it off in the review if the play-test is clean) |
| Log tag | `JUNE_FX DE_SOUND` (`python tools/logs.py --grep "JUNE_FX DE_SOUND"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-8 - Done"
grep -n "Effect C9h-8" june2017_re/funcmap.tsv
grep -n "^| 13[45] " JUNE_BUGS.md
```

## Task

Review the C9h-8 code against June in Ghidra (read-only), and fix transcription errors under the same flag. Things it could get wrong:
- **The play `31f6eca0`** (`python june2017_re/disasm_range.py 31f6eca0 31f6ee9a`):
  - the `PlaySound` argument order (`+0x25` LOOP, `+0x27` 3D, then 1);
  - the pair order: stores `@31f6edee` / `@31f6edf9` around the `push edx`, so `.first` = `+0x60` channel;
  - the push gated on `+0x26`;
  - `31d41710` after the push;
  - the erase taken when the timing test passed, even when the rate roll fails.
  - Confirm the `SoundData` byte offsets `0x20` / `0x24..0x27` are the March members `iRate` / `bUseGlobalTime` / `bLoop` / `bDeleteWhenEffectEnd` / `bThreeDimensionalSound`.
- **The stopper** (`disasm_range.py 31f785d1 31f7864c`):
  - the three-way branch (fail -> stop, match -> stop, other -> skip);
  - the null-channel skip;
  - its place before `CloseSound` in `~CEffect`.
  - Check that no other `+0xb34` reader exists: dispsweep `b34 b38 b3c`, CEffect hits only.
- **`31d41710` vs `JuneSet3DPositionAndUpdate`**:
  - a missing channel returns before the store;
  - `SetPaused` happens only when `Update3DPosition` is true.
  - Re-check the import `333bcf14` = `FMOD_Channel_SetPaused` (dumped IAT target `61e16100`, fmodex export RVA `0x96100`, base `0x61d80000`; neighbours `IsPlaying`, `System_Create`).
- **`31d411c0`'s error text** and the claim that `+0x70` holds the same string as `m_DeviceID` (June ctor `@31d419af`, base ctor `31d40f00`).
- **The data change.** The 13 `LOOP = FALSE -- C9h-8` rows in `kom_contents/data036_kom/DamageEffect.src.lua` must equal June's values for the same templets (`june_src_fixed/data036_kom/DamageEffect*.src.lua`), and must have been copied into the game's `data036.kom` (`python tools/kom.py status data036`).
- **The play-test:**
  - `play` lines with `loop=0` for the EP effects;
  - no droning sound;
  - no `stop` / `push` lines (March data has no DELETE_WHEN_EFFECT_END);
  - 0 EXCEPTION.
- **Hygiene:** `python tools/flagcheck.py SERV_IRUHADEV_JUNE_DAMAGEEFFECT_SOUND --with SERV_IRUHADEV_JUNE_DAMAGEEFFECT_SOUND_DEBUG --range <first C9h-8 commit>^..HEAD`.

Write "C9h-8 - Review" into the plan doc, update the row, check the next kickoff in STATUS, commit.
