# June character Raven (Stage 7) - Phase CH1g review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1g review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1g done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1g - Done"
python june2017_re/charport_adis.py raven --phase CH1g --out <scratch dir>   # 35 listings, one file per address (a file runs on into the next function when June has no int3 between them: 32c33fd0 into 32c342c0)
```

## Task

Review the 35 CH1g class-map rows (all `inside`) against the listings. **Count every `push` between a stack store and its reload** (the CH1d lesson) and check each float test by its x87 flag test (`test ah, 5` / `0x41` / `1` with `jp` / `jnp` / `jne`):
- **Where the end test and the fall sit** in the nine EventProcesses: `33c30` (fall -> CommonEventProcess; else flag window 0.1-0.702, `> 0.702` branch, chain as an `else if` of the branch, end -> WAIT as a separate `if`), `33fd0` (flag window `<= 0.737`, branch `>= 0.737` with the foot, end test first, chain as its `else`; walk / dash / back-walk / Z-X **without a foot test**, each clearing flag 0, skill last), `342c0`, `345f0`, `34970` (fall zeroes `speed.x`, passive -1; chain; end separate), `34cc0` (skill + hyper with the foot keeping the flag; back-walk / walk / dash / Z-X without the foot clearing it; end test last, separate), `35020` (no fall, chain, end), `5b740`, `59b60`.
- **Dash-jump `35330` / `35440`:** the added `StateChange( USI_JUMP_DOWN )` and the order `m_bDisableGravity = false` before it.
- **The overheat setters:** `32c526e0` (one argument; `RNSI_P_RVC_OPEN_MIND` 0x5f65 = 24421; BFI 84 / 217 and BTI 103 / 250 by `+0x9670 > 0`; the five `SetBuffFactorToGameUnitByBuffFactorID` pushes `ebx|ebx|eax|ebx|ecx` = BFI, creator 0, level, adjust 0, skip false; finalizer slot 5 = `GetFactor`; `SetFinalizerValue( 1, v, 2 )`, `SetGameUnitBuffFactor( this, -1, true )`, vslot 0x224; `+0x96d8` only when > 0), `32c598c0` (default 0.25, `+0x9734` and `+0x9735` or `0 < +0x9668` -> `+0x9738`; message needs vslot 0x198 `IsMyUnit`; text 0x3c4d = 15437), `32c5fd90` (read the Ghidra decompile too: penalty 1.0 / 0.2, `HP / max < +0x9668` -> 0, `*= +0x96d0 * penalty` on both numbers, clamp `[0, HP - max * +0x9668]`, `HP - fFinal <= 0` -> `HP - 1.0`, vslot 0x198 then vslot 0xec `( -f, 0 )`), and the inlined copy in `32c528c0`.
- Enum: `RNSI_P_RVC_OPEN_MIND` 24421, `BTI_BUFF_OVER_HEAT` 103, `BTI_BUFF_OVER_HEAT_ENHANCE` 250 against June's `Enum.src.lua`.
- `JUNE_BUGS.md` #540; `ch1_offsets.tsv`'s 11 new rows (member names for `+0x9668`-`+0x9738` come from the function's use, check them against March's declaration order).
- Probe both ways (`census.cl_cmd`, `/c /Zs /EHsc`), read `cl /P` for each branch; `junediverge.py --check-all`; `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Check the CH1g2 kickoff in the Review note.
