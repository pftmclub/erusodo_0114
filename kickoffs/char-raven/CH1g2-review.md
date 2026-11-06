# June character Raven (Stage 7) - Phase CH1g2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1g2 review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1g2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (commented out until CH1 final) and `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1g2 - Done"
python june2017_re/charport_adis.py raven --phase CH1g2 --out <scratch dir>   # 33 listings, one file per address (a file runs on into the next function when June has no int3 between them: 32c5a860 into 32c5a940, 32c4add0 into 32c4b290)
```

## Task

Review the 33 CH1g2 class-map rows (32 `inside`, 1 `new`) against the listings. The Done note says 23 equal March's body, 9 differ, 1 is new: check the **equal** claims as hard as the differing ones (the CH0 map said Create / Delete "DIFFER"; the Done note says they do not). **Count every `push` between a stack store and its reload** and check each float test by its x87 flag test:
- **Cancel chains** (`SKILL_CANCEL_AFTER( m_fSkillCancelAfter )`, then `RAVEN_GROUND_BWALK / WALK / DASH / ATTACK_CANCEL`, all with the foot test; CH1d's macros): where each sits. Z-Z-Z `32c36440` (Oneshot 0.501 `if`, chain, end test **last** as the final `else if`), Push-Z READY `32c367d0` (end -> PushZ, chain), Push-Z `32c5ac10` (`0.3 <` Z-released `if`, then `!_CanOverheat( false ) || m_bEndFrameThrow` -> FINISH, `else if` end, chain), FINISH `32c36af0` (end, chain). The new dash Z-Z EP `32c36020`: end -> WAIT, chain, then `else if` no foot -> JUMP_DOWN with `y - 45`, `RAVEN_ELSE_IF_FLAG_STATE_CHANGE( 1, 0.43f, ... )`, then the flag set when `t < 100`, `oneZ` and class 115 or 25 read from the unit data (`IsDataClass`, `32c2a343`).
- **Dash-jump X `32c35e20` / X-X finish `32c35f90`:** the added `USI_JUMP_DOWN` in the air at the end; `SKILL_CANCEL_AFTER_FLY` keeps March's macro, so `CommonEventProcess()` is its body (`JUNE_BUGS.md` #541: confirm `jne` goes to the bare return at `@32c35ebe` / `@32c5a92f` / `@32c36003`).
- **One-argument calls:** `_BurnOverheatHP( dmg )` and `_SetOverheat( true )` in `32c61580`, `32c619e0`, `32c5a940`; `_CanOverheat( true )` (`push ebx` = 1) equals March's default, `_CanOverheat( false )` in `32c5ac10`.
- **Overheat object pieces** `32c4dfe0` / `32c4e3c0` / `32c4add0`: the unit-class test is `GetUnitClass()` (NULL-safe, = `+0x38` of the unit data), `PlaySound( name )` is `( name, 1, 0 )` = March's defaults, the handle members `+0x975c` / `+0x9760` / `+0x9764` / `+0x9740` / `+0x9744` against `ch1_offsets.tsv`'s new rows and March's declaration order, the three icon strings and the 140 / 60 / 50 / 20 offsets.
- Everything else (Init / Start / FrameMove / FrameMoveFuture rows) = March: spot-check the floats (0.21 / 0.7 / 0.26 / 0.29 / 700, 0.006, 0.025, 0.03 and -800 / 1200, 42.5, the 0.2 s loop, 30.0, count `> 20`).
- Probe both ways (`census.cl_cmd`, `/c /Zs /EHsc`; pass `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`, **not** `/D...`: Git Bash rewrites a leading `/D` into a path), `cl /EP` string counts for each branch; `junediverge.py --check-all`; `enumscan.py --check X2Lib/X2GURaven.cpp`.

No build is needed if the review changes no code. Check the CH1h kickoff in the Review note.
