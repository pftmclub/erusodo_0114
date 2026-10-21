# June character Aisha (Stage 5 pilot) - Phase CH3a2 - Skills: Lightning Shower, Fire Ring, Dark Ring (Elemental Master / Void Princess)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH3a2` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH3a review (its Done note and commit); not beside CH3b / CH4 (same files) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_SKILL` (one flag for every skill sub-phase) |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |
| Review | `kickoffs/pilot-char-revamp/CH3a2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH3a - Done"
grep -a "Char CH3a" june2017_re/funcmap.tsv | grep -a "NOT MAPPED\|ROUTED"
awk -F'\t' '$1=="22012" || $1=="22052" || $1=="22224" || $1=="22225"' june2017_re/char_arme/ch0_state_skill.tsv
python tools/docsection.py JUNE2017_CHARACTER_PORT_CHECKLIST.md "7af. Camera linear tracking and view roll that only class code drives (from June engine R13c)"
```

Tools for the listing (Ghidra has not carved most of these): `python june2017_re/funcann.py <va>` (annotated listing: strings, floats, call names, vslots),
`python june2017_re/funccmp.py <a> <b>` (equal bodies), `python june2017_re/funcdis.py --func <va>`, `python june2017_re/charport_consts.py --fn <va>`.
The CH3a block at the end of `X2Lib/X2GUArme_VioletMage.cpp` (`//{{ ... Phase CH3a`) is the pattern: registration case in `June_SetEquippedSkillFuncHead`, `June_*` bodies after the CH2c block.

## Task

CH3a stopped at two June engine callees the map does not name. Map them, then port the three skills they block.

1. **`31c69af0`** (`CKTDGLineMap` method, `ret` after four stack args: hidden return pointer, position by value, a bool; `this` = the line map from `CX2Game::GetLineMap` `31e4dbf0`).
   - It starts from `( x, +-99999, z )` (the bool picks the sign) and walks the line list (`+0x190`), skipping `!bEnable` (`+8`) and `LT_WALL` (`+4 == 1`); read it whole (about 1,480 listing lines).
   - Name it, give it a March home in `KTDXLIB/KTDGLineMap.*` under the SKILL flag (or a `CX2GUArme` member if it is only Aisha's), port the whole body.
2. **Lightning Shower** (22012, upgrade 22052; states 221 / 222 loop, 223 end). Registration: `32cede00` case `0x55fc` (decompile line 510).
   - Normal state: Init `32cb82e0` (= March's `AVSI_SA_AHM_Lightning_Shower_Init`), Start `32ccd170`, FrameMove `32cdebd0`, EventProcess `32cc2040`, End `32cb83c0` (= March's StateEnd); the hyper state copies Start / FrameMove / EventProcess / End.
   - Loop states 221 / 222 (table `AVSI_SA_AHM_LIGHTNING_SHOWER_LOOP`): Start `32ccd320`, FrameMove `32cdf240`, EventProcess `32cc7200` (already `June_AVSI_SA_AHM_Lightning_Shower_Loop_EventProcess`, CH1k), End `32ccd400`.
   - End state 223: EventProcess `32cc2110` (CH1k member), End `32cdf740` (the class map missed it: it zeroes the two air-step particle emit rates, then CommonStateEnd).
   - What CH3a already read: the fire block differs from March. Radius = `( rand( n * 4 + 1 ) * 3 - 150 ) * 4`, then +250 / -250 by the count's parity, then `* -1` when facing right (no +-500); the effect position is `GetPos() + dir * radius`, y + 10; `CanDown( pos, 30, NULL, NULL, NULL, 1, 0, 1, 0 )`, a second probe from `y = 99999` and then `31c69af0` when both fail; the damage effect is created at that point (`DamageEffect_Lightning_Shower_LineCrash`). Start also clears the EnchantShock time (`+0x2480`) with `y -= LINE_RADIUS * 1.5`, like Meteor Shower's. Confirm each value in the listing, the decompile reuses stack slots.
3. **Fire Ring** (22224) and **Dark Ring** (22225): FrameMoves `32ce3880` / `32ce3b80` (registered `32cf2c10` / `32cf2c20`), EventProcess `JuneGenericSpecialActiveSkillEventProcess`. Both call `31ff5440`, June's SUB_CAMERA time sequencer (checklist 7af: it picks `SUB_CAMERA%d` by elapsed time, plays it with `31fb6af0`, then sets the camera step time through `31fae7b0`). Map it (funcdis merges it into a ~1,280 line block: find its real bounds), port it as a `CX2GUUser` member, then both FrameMoves. CH3b's Energy Ring `32ce3e80` reuses it.
4. Register the three cases in `June_SetEquippedSkillFuncHead` (Lightning Shower 22012 / 22052, Fire Ring 22224, Dark Ring 22225; the last two have no upgrade ids), write the March vs June damage to `MODS.md` (June Lightning Shower values: `c1a_numerical_blocks.tsv`), add `JUNE_BUGS.md` rows for any June quirk.

Done gate: the S3 census shows 0 `no_state` for these ids; each skill equipped in an A or B slot casts June's state with June's motion and effect and deals damage; no missing particle / mesh / texture line in `ErrorLog.txt`. The play-test save needs an Elemental Master (class 104) with Lightning Shower and Fire Ring and a Void Princess (105) with Dark Ring: `/x2-create-char` + `/x2-db`.
