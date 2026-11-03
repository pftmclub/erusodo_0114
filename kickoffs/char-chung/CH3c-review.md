# June character Chung (Stage 7) - Phase CH3c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH3c review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH3c done (its Done note, commit and the user's play-test + "check logs") |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_SKILL` (+ `..._SKILL_DEBUG`, retired by this review) |
| Log tag | `[JUNE_CHAR]` (`CHUNG SKILL`, `SUMMON`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH3c - Done (2026-11-03)"
grep -a "Char Chung CH3c" june2017_re/funcmap.tsv
python june2017_re/charport_adis.py chung --phase CH3c --out <short scratch dir>
```

## Task

Check CH3c against June (read-only; read the listings, not the decompiles). In order of risk:
- **Heavy Field `32c0b7f0` / `32c0b9b0` / `32c0ba60` / `32c0bd20` / `32c0c1c0`** (registration `32c1f530` @32c21ec5-32c21f88). READY frame move: once at 0.001 s, `GetIsRight()`, `GetPos()`, line map `GetFixedDistancePosition( pos, +0x8c10, right )`,
  `CreateNPCReq( 0x9dd, ... )` only when vslot `0x198` (JUNE_BUGS 512: `+0x8c10` is `m_fEventTime[0].keyInputStart`, check the member map). READY event process: `FollowSlotSwap`, Future time `>= 0.6`,
  NPC at `+0x98e8` -> state `0x151`, else `5 < state time` or `3201bd60( false )` false -> `0x152`; keys cleared. LOOP frame move: NPC camera writes (`+0x8750`, `+0x8d34`, `+0x8e39`)
  only for my unit, `( int ) +0x8178 - 1` jump table `32c0bcfc` (aim 5 = idle). LOOP event process: the three-way end test (`keyInputStart > time` aims; released -> end; `time < keyInputEnd` aims), the aim
  block is the sniper's `3200d450` copy (compare the nine key branches and the write of `+0x7ed8`). END start: NPC `+0x8750 / +0x8d34 = 0`, `SHOT_ARTILLERY_SELF_DESTRUCTION`, UID -1, rotation y + 180 when facing left,
  `EffectSet_SA_CTT_Heavy_Field_End_Projectile`.
- **Mark of Commander `32c1c840`.** Templet `31fca8e0( 0x6666, 1 )`, factor count `> 1`, level `GetSkillLevel( id, 0, 0, 1 )` min 1, `322b5320( factor[0], level, 1, 31fdbfc0( id ) )`, then
  `32206fb0( 1, level, &( 2.0 ), 0 )` and `32610e90( 0, value )` on success.
- **Tactical Field `32c1cc70`** (templet `0x65a8`, memo type `0xf` x `+0x44c` of both effects, handle `+0x8db0`, `322b5320` on the zone, `SetCannonBallCount( max )` only) and **Cannon Strike `32c07040`**
  (`SetCannonBallCount( max )`, extra balls under `32c24d00`'s class guard, second effect set at 1.5 s) and **Carpet Bombing `32c0f6d0`** (`0x65a7`, `SA 0x1e`, 18 / `+ 9`, rate 0 then
  `CanConsumeCannonBallForShot( 6, 6, 1, &rate )`).
- **Registration `32c1f530`.** Labels on 26214 / 26237, 26023 / 26058, 26215 / 26238, 26026 / 26061, 26024 / 26059; new blocks 26228 (start `32bf48e0`), 26041, 26042, 26064; `ch1m_regcheck.py`.
- **The "= March" rows** (funcmap `Char Chung CH3c`): confirm 4 against the listing, e.g. `32be6200`, `32be75b0`, `32bf3830`, `32c12470`.
- **Play-test results:** `heavy field targeting npc requested`, `heavy field ready ends` / `loop ends` / `heavy field end`, `SUMMON npc 2525 ... skill 26064 ... flags 0x0`, `SUMMON npc 1100 ... skill 26215 ... flags 0x44a`,
  `SUMMON npc 863 ... skill 26025`, `remote mortar uid ... added / erased`, `carpet bombing start, max shots 18 / 27`, `cannon strike balls full`, `tactical field`, `mark of commander buff`,
  `ES_CREATE` for the effect sets, `ErrorLog.txt` (no missing mesh / particle / sound of JUNE_BUGS 505's kind, no `DAMAGE_EFFECT` / NPC state error for the SHOT_ARTILLERY states).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_CHUNG_SKILL --range <CH3c commit>^..<CH3c commit> --with JUNE_CHAR_CHUNG_SKILL_DEBUG`, `git diff --patience --numstat`, `junediverge.py --check-all`,
`enumscan.py --check X2Lib/X2GUChung.cpp X2Lib/X2GUChung.h`, `skillicons.py check` (the 16 ids), `ch1m_regcheck.py`. `_SKILL_DEBUG` is already retired (CH3c2 review, 2026-11-03): judge the play-test from the CH3c Done note's round-1 lines; do not re-define it. Write
"CH3c - Review" into the plan doc, update the STATUS rows (CH3d unblocks) and commit.
