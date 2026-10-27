# June character Eve (Stage 7) - Phase CH2c review - Skills: Electra (26)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2c review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2c done and play-tested (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` (no debug flag in CH2c) |
| Log tag | `[JUNE_CHAR]` (`EVE SKILL`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2c - Done (2026-10-27, Sonnet 5.5)"
```
Then the 27 `Char Eve CH2c` rows at the end of `june2017_re/funcmap.tsv`, JUNE_BUGS 452, and `june2017_re/char_eve/ch0_skill.tsv` lines 36 / 37 / 38 / 51 / 54.

## Task

Review the CH2c transcriptions against the raw listings (`python june2017_re/charport_adis.py eve <va>`; a listing runs on into the next function after the first `int3`, stop there: `32993730` ends at `32993c3e`). Where the port could be wrong:
- **`329a67b0`** (Particle Ray upgrade FrameMove): `@329a6df1` is reached on every frame, not only at 0.43333 s; `[ebp+0xe8]` is the effect handle stored to `+0x9c48` after `32980000`; `31f64190( handle, 1 )` on the damage effect manager is `GetInstance( handle, true )`; `+0x5a8` is `m_vRotateLocal` ( funcmap `329cb2f0` ); the clamp reads SA `0x9f` through `32007c40` with swap 0 ( push `0` before the call is the swap, vslot `0x524` takes no argument ), default 45; the new Z is `class-skill float Now + old Z`.
- **`3297d730`** (upgrade EventProcess): `FollowSlotSwap` first; the steering branch is the `>= 0.4` side of the first compare; SA `0x9e`, default 5; pureUp +S, pureDown -S, else 0, written with `SetClassSkillFloat`.
- **`32993730`** (Photon Flare): the listing ends at `32993c3e`; the compare against `600.0` is a double; the memo block ( type 15 = `MI_CHECK_AND_KILL_1` ) multiplies the protected time at effect `+0x25c`; the early `je 32993c2d` for a null unit skips `CommonFrameMove`.
- **`329aa730`** (Kugelblitz): templet id `0x61c0`; the two `GetSkillAbilityValue` calls ( SA `0x20` + 1.5 kept, SA `0x21` dropped ); `[esi+0x9ae0] = 0` only in the templet branch; the second `CreateInstance` is `Damage_KugelBlitz_SelfDamage` with the same position, rotation, power and land position; no `FlushMp`, no not-enough-MP branch.
- **`329733b0` / `3297d3f0`** (cancel chain): `+0x8c90 .. 0x8c9c` are skill / bwalk / walk / dash; walk and dash carry `bFootOnLine` ( the `JUNE_EVE_` macros ).
- **Registration** (`329c8a00`, the `ch0_skill.tsv` rows above): Particle Ray 25023 and 25063 are separate cases with the callbacks of the rows; the hyper tables of Photon Flare and Particle Ray have no Init, Spectro's hyper table keeps Init; Photon Flare and Kugelblitz upgrades share the base case.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_SKILL --range <CH2c commit>^..HEAD`; `python tools/enumscan.py --check X2Lib/X2GuEve.cpp`.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH2c - Review" into the plan doc, update the CH2c STATUS.md row, check the next kickoff (`CH2c2`), and commit.
