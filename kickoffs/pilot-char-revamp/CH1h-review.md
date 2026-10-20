# June character Aisha (Stage 5 pilot) - Phase CH1h review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1h review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1h done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1h - Done"
grep -a "Char CH1h" june2017_re/funcmap.tsv                            # the 47 rows the phase wrote (33 ported, 14 routed / n-a)
awk -F'\t' '$6=="CH1h"' june2017_re/char_arme/ch0_class_map.tsv        # the 33 ported functions; the 29 moved rows carry a note "CH1h: ..." in column 7
```

The `June_*` members are in the eighth block (`//{{ ... Phase CH1h`) at the end of `X2Lib/X2GUArme_VioletMage.cpp`, declared after CH1g's in the header. `EffSetID_June2..4` were added to `EffectSetInstanceID` (June's `m_ahEffectSet` has five slots; March's two keep their numbers).
**Not carved in Ghidra (read with `python june2017_re/funcdis.py --func <va>`; its bounds merge neighbours, so stop at the first `ret`):** `32cb6e00`, `32cb7eb0`, `32cb7ef0`, `32cb96b0`, `32cc5ca0`, `32cc6e50`, `32cddcc0`, `32ce4db0`, `32ce4e80`, `32ce55e0` (the listing after its `ret` is `32ce5700`, a CH1i function).

## Task

What the phase could get wrong:
1. **Constructor `32cc99d0` -> `June_ConstructorInit`.** June's members run in March's declaration order, so each June offset maps to a March member by position (`+0x95a8` = `m_hSeqArmeAirStepRight`, protected floats take 12 bytes). Re-derive the map for `+0x95a8..+0x9854` (ctor stores `param_1[idx]`, byte offset = idx * 4) and check every value: `m_fElementalResearchRate = 1.0` (March 0), `m_fHellDropCreateEffectCooltime` is the plain float at `+0x974c` (Hell Drop `32cd9af0` adds 0.25 to it), `m_bIsHellDropTimerStart` the byte at `+0x9750`, `m_iMaxFireCount` / `ByLoopState` = 10 and `m_fUseMPConsume` 5.0 / `m_fFireGap` 0.2 are the protected values at `+0x96f8..+0x9728`. The June-only fields (`m_iJune96e4` .. `m_vecJune9848`, `m_fJune8fb8/8fbc`) stay offset-named; identify any whose role is readable from a caller. The container at `+0x9760` is still the `m_iJune9770` stand-in (its element type is open since the CH1a review; `3288ff80` takes a `UserSkillTree`-style test, so the entries look like skill ids).
2. **Destructor body `32ce4600` -> `June_Destructor`.** Order of the releases, the second unconditional `m_SubAttackListSet.clear()`, the extra mesh handle `+0x96dc`.
3. **`June_InitComponent`** drops March's PVP `SetNowMp( 50 )` and only calls `GetGameType`; **`June_ReInit`** keeps a `GetNowMp() < 50.0 && GameType != PVP` floor and the MemoRise clear in game types 1 and 4 (4 has no March `GAME_TYPE`). Win / Lose / Draw use the names `USI_RIDING_WAIT_HABIT` / `USI_RIDING_WAIT` for June's `0x4b` / `0x4c` (March's enum is 74 / 75; June's Enum.lua has `GUSI_END + 70 / 71`): confirm June's `GUSI_END`.
4. **Stand-ins to confirm or replace:** `June_UnitFlag3c` (CX2Unit `+0x3c` read by `32cddcc0`, and by `31fceab0` as "transcended"), `June_GameRefreshMemoRise` (CX2Game `322deb80`, unmapped; it checks the unit's class templet == 2 and `32cc2880`).
5. **Routing.** The ten skill-state callbacks sent to CH2a / CH2b / CH3a, `32ce9f90` to CH4 and the four to CH1i were moved on the evidence of the registrations in `32cede00` (thunks `32cf2600` .. `32cf2be0` at `32cee3c0`, `32ceeb13`, `32cef319`, `32cefbe1`, `32cefcbb`, `32ceff2d`) and the callers; check two of them in Ghidra, and the "next class" call for `32cf2da0` / `32cf2e90`.
6. **`= March` bodies** (`32cb9110`, `32cc1c50`, `32cc6e50`, `32cc7fa0`, `32ce4e80`, `32ce55e0`, the five `Append*ToDeviceList`): names, order and constants only (`LightningBolt10Red` is missing from the append list in both).

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1h commit>^..<CH1h commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH1h -- Review" into the plan doc under the Done note, update the CH1h row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1i.md` still matches, and commit with explicit pathspecs.
