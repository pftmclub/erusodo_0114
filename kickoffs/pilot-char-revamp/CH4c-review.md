# June character Aisha (Stage 5 pilot) - Phase CH4c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4c review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | Phase CH4c done (its Done note and commit) and its play-test |
| Flags | `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE`, `SERV_IRUHADEV_JUNE_USER_PASSIVE_HOOKS`, `SERV_IRUHADEV_JUNE_CHAR_ARME_OVERRIDE_DEBUG` (temporary) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL mp consume`, `cool time`, `magical staff`, `shadow death`, `cyclone upgrade`, `transform equip`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH4 split (2026-10-22, Opus)" "CH4c - Done (2026-10-22, Opus 5.5)"
grep -a "Char CH4c" june2017_re/funcmap.tsv
grep -a "^| 398 \|^| 399 " JUNE_BUGS.md
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7an. User passive hooks and specialization readers per class (from pilot-char-revamp CH4c)"
```

The code: the `//{{ ... Phase CH4c` block at the end of `X2Lib/X2GUArme_VioletMage.cpp`, the `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE` forwards in
`GetActualMPConsume`, `TransformStart` / `TransformEnd`, `SetSpecificValueByBuffTempletID`, `SetDamageExceptionProcess` and
`June_UpdateEquipViewListInfo`, the `.h` declarations, and the `SERV_IRUHADEV_JUNE_USER_PASSIVE_HOOKS` blocks in `X2GUUser.h`, `X2GUUser.cpp`
(`DamageReact`) and `X2GameUnit.cpp` (`GetFinalDamage`).

## Task

Review CH4c's commit against June, in the listing (`june2017_re/funcann.py <va>`; most are not carved in Ghidra):
1. **`32cccc70`:** the Cyclone test (`+0x5c4` 22011, `31fca8e0( 22011, 1 )`, `+0x178`, `32007b10( 0x2a, 22051 )` into `+0x514` =
   `m_fDecreaseForceDown`), the `DD +0 == 0` gate, 22409's two SA reads, and the negative force-down branch (`@32cccd2f`: `1 + ( 1 - v )`).
2. **`32ccfe30`:** the switch on `32207210` (jump table `32cd01d4`, index bytes `32cd01e0`), the aura level call `( 105, 0, 0, 1 )`, the fireball
   formula, Teleport's `+0x95b0` and the PvP-gated `+0x97a0` (JUNE_BUGS 398), Open Mind 22414 through `322d7920`, the clamp, and the memorize
   term `( 1 - +0x6d04 )` = `GetCharacterSpecializationB()` (vslots `0x718` / `0x880`, `31fcec40`).
3. **`32ce1cc0`:** types 12 / 13, Binding Circle's `+0x7e1c` test (`IsOnSomethingFuture`), Teleport's `SA_COOLTIME_1` add, `+0x9638`,
   Open Mind, the Time Limit test (vslot `0x240` `3229a470` over `32028af0`), the clamp.
4. **The hooks:** `32cc28b0` (22406 base level, `arg != 1`, `GetRandomFloat( 0x14 )`) and its caller `32284aa0 @32285cf9` (user attacker,
   multiplies the defence term `@32286073`; March's inline Shadow Death block is the `#else`); `32cc5c30` (JUNE_BUGS 399) and its caller
   `320462a0 @32049d78` (after the guild and Spiritualized Fury terms, before vslot `0x748`).
5. **Transform:** `32cddec0` / `32cde300`, `32cc91b0`, `32cdbd00` (weapon null test, the `+0x9848` replay through `3286a230` and vslot `0x644`,
   `32cc8d30`), and the queue writer `32cc9950` (item UID at `CX2Item +0`).
6. **`322d7920`:** the exclusion set, team, die state, HP, `GetInvincible`, the NPC class / size offset, `<=` against the shrinking range.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE --range <CH4c commit>^..<CH4c commit> --with SERV_IRUHADEV_JUNE_CHAR_ARME_OVERRIDE_DEBUG`
and `python tools/flagcheck.py SERV_IRUHADEV_JUNE_USER_PASSIVE_HOOKS --range <CH4c commit>^..<CH4c commit>`; paste the `files:` / `in this flag's
files:` / `RESULT:` lines. The `Always.h` ERROR rows are the `#error` guards that name two flags.

If the play-test passed, retire `SERV_IRUHADEV_JUNE_CHAR_ARME_OVERRIDE_DEBUG` (comment out its define, build, deploy).
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH4c -- Review" into the plan doc, update the STATUS row, check `kickoffs/pilot-char-revamp/CH4d.md`, and commit.
