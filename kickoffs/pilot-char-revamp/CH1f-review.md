# June character Aisha (Stage 5 pilot) - Phase CH1f review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1f review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1f done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1f - Done"
awk -F'\t' '$6=="CH1f"' june2017_re/char_arme/ch0_class_map.tsv      # the 31 June functions
grep -a "Char CH1f" june2017_re/funcmap.tsv                            # the 31 rows the phase wrote
python june2017_re/charport_consts.py CH1f                             # every constant, with its width
```

The `June_AVSI_*` members of CH1f are in the sixth block (`//{{ ... Phase CH1f`) at the end of `X2Lib/X2GUArme_VioletMage.cpp`, declared after CH1e's in the header. No enum member was added.
**Not carved in Ghidra (read with `june2017_re/funcdis.py --func <va>`):** `32cb7de0`, `32cb8840`, `32cb8be0`, `32cbc880`, `32cbd120`, `32cd4450` / `32cd4650` (one bounds range), `32cd5180`, `32ce6b30` (its range also holds `32ce6660` / `32ce6830` / `32ce69b0`, start at `32ce6b30`).
`32cdfb70` is carved but its decompile is flagged "type propagation not settling": the Done note says it was read from the listing.

## Task

Review the 31 transcribed functions function by function. What the phase could get wrong:
1. **`32cbd120` (X2 EventProcess, shared with the HM X2).** The four X windows (flags 0..3, windows 0..3): the key tests (pureDown; forward; back = `pureRight && !bIsRight || pureLeft && bIsRight`; none), the states `0xab / 0xaa / 0xad / 0xa9`, the `0.966` (qword) 12-key tail, the animation-end `else if`, and `m_bCanChangeState` (`+0x96f0`) set from flag 3 after the chain. `SKILL_CANCEL_AFTER( 0.01 )` is the awakening-lock expansion; confirm `32cbd197-32cbd21d` matches it.
2. **`32cdfb70` (Summon Bat Heavy FrameMove).** Skill lookup is `CX2SkillTree::GetSkillTemplet( 0x56c9, 1 )` (whole tree; ported as the one-argument form of the live build) where March used the equipped-skill lookup; the NULL-unit exit skips `CommonFrameMove`. The level is `max( 1, GetSkillLevel( id, 0, 0, 1 ) )`, the lifetime `GetSkillAbilityValue( 1, level, 0, 0.0 )` (type 1 = `SA_EFFECTIVE_TIME`). Both `PlayEffectSet` calls carry March's arguments; the second is `( .., false, -1, -1, (1,1,1), true, vPos, GetRotateDegree(), GetDirVector() )` with `vPos = GetPos() +/- 100 x dir`, `y + 150`, `+ GetZVector() x -50`.
3. **`32ce6b30` (AVP DASH_JUMP_X FrameMove).** Cost `4 x +0x95bc` through `32cc2b90`; class test BM `0x17` / ADW `0x71` (June reads the unit's templet pointer); transformed (`+0x8c0c`) -> `ARME_ACTIVE_MAGIC_BALL2`, else `32ce4180` type 0 with `rot.z = -30`; other classes `AISHA_GRAVITY_BALL` with power `+0x9640`. **`+0x9640` is read as `m_fGravityBallPower`** (the only readers are fireball sites; `32ce445e` inside the `32ce4180` helper is the gravity-ball power scale; ctor sets 1.0): confirm or name the member.
4. **`32ce2b20` (ADM dash-jump ZX).** `DamageEffect_ADM_DashJumpZX` with power `+0x9640`, `rot.z = -30`, bone `TRACE_START0`; the failure effect z is `-30`.
5. **`32cc3ed0` (ADM dash-jump ZX EventProcess).** Landing `0x16` leads and skips the rest (else-if); the hyper half has no skill half; the walk / bwalk / dash / ZX cancels carry `IsOnSomethingFuture()` and are therefore dead after the landing test (June keeps them).
6. **`32cbf810` / `32cbf9a0` (AVP ZZfrontZ / ZZfrontZZ).** `CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 )`, then end / air / flag (`0.8` or end) / arm (`< 0.8`, oneZ) / `0.94` 12-key; `0.8` and `0.94` are qwords of floats, `1.65` too. States `0xc9`, `0xca`, `0xd`, `0x11` (no y shift in ZZfrontZ).
7. **Fireball arguments** (`32cd4450` FIREBALL_SHORT / 7 x rate / range `+0x95c8`; `32cd4650` and `32cd5180` DARK_SHOCK_BALL / flat 6 / range 1; `32ce1950` DARK_SHOCK_BALL_AIR / flat 6 / +-87 x dir / range 1): the `CreateInstance` argument blocks (`-1, 1, range`, then March's defaults), power `GetPowerRate()` (slot `0x204`), failure args.
8. **Pure `= March` bodies** (`32cbc520`, `32cbc880`, `32cb7400`, `32cd3be0`, `32cbfa70`, `32cbfaa0`, the five Inits, `32cb7db0`): the constants (`charport_consts.py CH1f`) and the call order only.

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1f commit>^..<CH1f commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH1f -- Review" into the plan doc under the Done note, update the CH1f row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1g.md` still matches, and commit with explicit pathspecs.
