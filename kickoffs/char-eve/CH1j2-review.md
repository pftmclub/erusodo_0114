# June character Eve (Stage 7) - Phase CH1j2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1j2 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1j2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1j2 - Done"
awk -F'	' '$6=="CH1j2"' june2017_re/char_eve/ch0_class_map.tsv      # 46 rows
grep -a "Char Eve CH1j2" june2017_re/funcmap.tsv                      # 46 PORTED rows (45 state functions + the 32979a00 helper)
```

The 46 members are in the eleventh `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` (heading comment "Eve's Battle Seraph line"); declarations at the end of the class in `X2Lib/X2GuEve.h`. No new enumerator, field or `JUNE_BUGS.md` row.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1j2 --out <scratchpad dir>` (46 files) and a Ghidra decompile where a function object exists (six of the 46 have none: `32979fe0` `3297a110` `3297a240` `3297e2a0` `32995120` `32995ba0`). You decide:

- **Raw reads vs the triples:** the boost-dash, air and boost-attack EventProcess read `m_PhysicParam` fields at `+0x32c` / `+0x330` (March's `m_PhysicParam.GetJumpSpeed()` / `GetDashJumpSpeed()`), not the unit-level triples CH1e used for the plain dash; `GetDashSpeed()` is the triple (`32071a80`). Confirm each of each site.
- **"= March" bodies** (about 30): strings in order, constants (0.3 / 0.4 / 0.65 windows, 1 / 2 / 3 s speed steps, 0.1 / 0.2 / 0.3 steps), the boost dash FrameMoveFuture's x87 form against `GetDashSpeed() + GetDashSpeed() * ( H + n )`.
- **Facing block** (`329677d0`, tail of `32978500`): both keys held -> `oneRight` / `oneLeft`, else the single key.
- **AIR_START / AIR EventProcess** (`32978890`, `32979da0`): wall landing live (`bFootOnWall`, `oneUp` or `oneDoubleUp`), `32976f00` limit, no `m_bDownForce` resets (3297a400 clears it; JUNE_BUGS 421 is withdrawn), `32979da0` X arm (`+0x9b56` = 0, `+0x9b58` = false, then `32978c60`).
- **ZFront / ZUp / ZDown EventProcess** (`32979fe0` and two copies) and the helper **`32979a00`**: Open Mind 25419 ability `0x80`, Electronic Move 25223 (`JuneIsEquippedSkillKeyPressed`, add = 0), the helper's three-way test (`JuneIsSameOrUpgradeSkill( future skill, 25223 )`, `+0x9b5c > 0`, `IsUpgradedSkillLearned( 25029 )`, count < add + 3), windows 0.13333 / 0.59 / 0.833. CH0 labelled the helper "called by 32979da0"; its callers are `3297a07d` `3297a1ad` `3297a2dd`.
- **XZ FrameMove `329a8840`:** the 0.533 hyper arm, AddLaser before the handle store, the 0.733-1.233 / 1.233-1.733 rotate bands, the 1.833 end block and the MP drain. XZ EventProcess's nowAction store is the inlined KProtectedType Set.
- **ECB dash X-X `329ad6c0`** (June-only, no March body): the else-if chain order 0.066 / 0.2 / 0.315 / 0.23, the three `Light_Eve_AttackX3` offsets (10 / -55 / 85 along the direction, y +50 / -80 / -80), the Triangle effect (hand mix 0.8 / 0.2, direction x 50, power `+0x97c0`, force down `+0x97c4`, empty name arguments to `June_AddEffectToSystem`), the 0.23 Triangle particle (y +85), the three tag sites.
- **Alpha tags (7c):** the sites file counts 9 (`329950fc` `32995b6e` `32995d38` `32996163` `329963da` `329966da` `329ada7f` `329adaf9` `329ae08f`); the port has 12 calls because three sites are shared tails. Confirm.
- **JUNE_BUGS:** none was written; say so if you find one.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1j2 commit>^..<CH1j2 commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1j2 - Review" into the plan doc under the Done note, update the CH1j2 row in `STATUS.md`, check that `kickoffs/char-eve/CH1k.md` still matches, and commit with explicit pathspecs.
