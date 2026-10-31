# Chung (char-chung): checklist rows

Source: JUNE2017_CHARACTER_PORT_CHECKLIST.md (1019 lines; "L" = that file's current line numbers, 2026-10-31; Eve's copy cites the older 929-line file, so its L numbers no longer match).
Also grepped: JUNE_PORT_BACKLOG_ORDER.md, JUNE_BUGS.md. Read-only; nothing edited except this file.
Grep terms: chung, CX2GUChung, X2GUChung, UT_CHUNG, UC_CHUNG, Iron Paladin, Deadly Chaser, Tactical Trooper, Fury/Shooting/Shelling Guardian, cannonball, ChangeMaxCharacterAddResource. Zero hits for the job names, UT_/UC_ ids and "cannonball".

## Part A - rows naming Chung (39 grep lines in 19 sections plus the intro)

| Sec | L | Row text (trimmed) |
|---|---|---|
| intro | 4 | "June update of a March one (Elsword, Aisha, Rena, Raven, Eve, Chung, Ara, Elesis, Add)" |
| 1 | 53 | `Chung_Iron_Cannon` / 15 numerical-only DAMAGE_DATA tables / 74 other |
| 7 | 236 | `32bf4680` (non-virtual, after vslot 465) / Chung / Skips its timer work when the buff hyper count is 3 |
| 7 | 246 | `32bf4900` (`CX2GUChung::InitComponent`) and `32c16be0` (`InitPassiveSkillState`) / Chung / `32bf4900` reads new key `COMMAND_HIT_ADD_MP_BONUS` (default 1.0; June's `Chung_Iron_Cannon.lua:284` sets 1.15) into protected float `+0x98c4` ... |
| 7 | 254 | NOT Chung. BBT 97 `CHANGE_MAX_CHARACTER_ADD_RESOURCE` row lists Add `329e31e0`, Ara `32baafc0`, Rose `32ac5e20`, Luciel `32b14cd0` (unit vslot 201 `+0x324`); Chung has no override. Matched only on the search term |
| 7 | 255 | death-prevention pre-apply hook, per-class override list includes "Chung `32bed760`" (from user slot 250 `3201ffb0`, whenever BTI 436/926 is applied) |
| 7 | 259 | quick-slot special-item prefix already at the top of the eight March classes' `SpecialAttackEventProcess` (Elsword, Eve, Arme, EL, Ara, Chung, Raven, Lire); carries Aisha and Eve marks |
| 7 | 278 | Chung's Metabolism Boost in `CX2Game::UseItemSpecialAbility` (`322fbc60` HP up `@322fbe08`-`322fbe83`, HP % up `@322fd495`-`322fd510`): June tests unit class `0x14`/`0x6e` (`31e4d0c0`), skill `0x6720`, inlines `ability - 1.0 + 5e-8` |
| 7 | 280 | `3201ad10` (skill-group power socket rate) called from class code; Chung range `32bec286`, `32c058a0` (functions not decoded); core callers use `( rate + 1.0 )`; Eve mark present |
| 7 | 287 | engine readers keyed by a March skill ID where June pushes the reform ID; class column "Chung, Eve": per-skill stores `31fd7cc0` buff rate, `3201ad10` socket rate; Eve mark present |
| 7b | 327 | per-class SetDamageExceptionProcess blocks (Chung's is `32bf28f0`; list Eve, Elesis, Chung, Raven, Rena, Aisha, Elsword, Ara): passives keyed on June reform skill IDs (`RNSI_*`); Aisha and Eve marks |
| 7e | 369 | lost under June's player (SBH/ALP end windows, class-change mesh outline ...), classes Elesis, Lunatic Psyker, Chung, Elsword, Rena: "nothing to port" |
| 7i | 441 | `SetEquippedSkillFuncBySkillSlotIndex` snapshot call, Chung `32c1f530`, all 12 classes: "Nothing for the snapshot"; Aisha mark |
| 7l | 469 | "March's state id is a UCHAR and Chung's last state is already 255; June's id is 16-bit" (D10 (a): C3d-2 widened the id); Aisha mark |
| 7n | 492 | sync-float table row: Chung `32c0ba60` / `32c0ba60` / thunk `32c256f0`, registered by `32c1f530` (Chung vtable `33509e30`) |
| 7p | 546 | hit-stop sites: Chung DamageReact `32c0cf90` (vslot `0x60`) @32c0d18c / d19c / d1b4: attacker SetStopTime(0.5), own SetStopTime(0.5), own SetStop2Time(0) |
| 7r | 563-571 | Chung cinematic-UI writes (verbatim below); table rows L570, L571 |
| 7s | 576 | `X2DungeonManager.cpp:252-552` reads COMMON/ELSWORD/AISHA/RENA/RAVEN/EVE/CHUNG/ARA/ELESIS/ADD only: March already reads CHUNG |
| 7t | 599 | hyper cut-in: Chung unit vslot `0xa4` override `32c24b90`: `+0x9714 > GetRandomFloat( 0 )`, a berserk roll (identify `+0x9714`) -> override `JuneIsBerserkByChance` |
| 7t | 610, 614 | sound sets: "Chung `JuneIsBerserkByChance`" is one of the three overrides; `Chung_Iron_Cannon` among the scripts with sound sets |
| 7u | 637 | March-only templets dropped: `X2GUChung.cpp`: `DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell`, `EffectSet_CSI_CIP_LAND_DEMOLISHER_AFTER_EFFECT`; `X2BuffTemplet.cpp`: `CHUNG_BIGBANGSTREAM_MEMO`; June side `Chung_Iron_Cannon.lua`: `EffectSet_CSI_CSG_COMBO_XFRONTX_FIRE` |
| 7u | 644, 646 | "Chung: `X2BuffTemplet.cpp:1695-1706`. On the 20% memo-26 roll the `else` skips `HAND_GRENADE_0` ... grenade in five now fires nothing until the Chung port." |
| 7ae | 720, 728 | default-state fallback names "Chung Detonation"; "49 rows (Chung upgrade ids ...)" (verbatim below) |
| 7ad | 745, 752, 755 | June's `Motion_CHUNG.x` already taken into data033; stage pattern `^motion_(...chung...)`; lost skill animations of Add, Ara, Chung, Elesis, Lire |
| 7ac | 774 | CanDown call shape: "Chung Artillery Strike Ready (`@32bfe8c3`, `X2GUChung.cpp:17152`)" passes the old argument |
| 7am | 897 | delegate-list registrations: Chung `32c16be0` / A `32c12910`, A `32bdba10` |
| 7an | 920 | vslot `0x748` soul gain when hit: Ain `32a387b0`, Chung `32beeca0`, Raven `32c3c820`; "not ported yet" |
| 7an | 921 | `GetCharacterSpecializationB()` (+0x6d04) readers: Chung `32bf528d` -> "the reading function, whole"; Aisha mark |
| 7ao | 954 | transcendence: Chung `32c063d0` (hyper effect start, vslot 49) -> "the branch" |
| 7ap | 969 | Chung: 0x6a8 `32bdba60` (skill use `32c17b82`) |

### Verbatim: 7r (L563-571)
```
### 7r. Chung's cinematic-UI writes (from June effect completion C3o review)

June moved Chung's `SetFocusCamera / SetFocusCameraForce / SetApplyCinematicUI( false )` on the targeting NPC. Neither site changes
anything on screen (JUNE_BUGS 164; the readers need the NPC's start state), but the Chung port should match June's placement.

| Class | June site | March site | Port |
|---|---|---|---|
| Chung (Missile Shower 6019, Quantum Ballista 6011) | `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE` StateStartFuture `32bff3d0 @32bff412-32bff420` (state `0xf5`, registered `@32c1fffa`) | `CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_EventProcess` (`X2GUChung.cpp`, before `StateChange( ..._CONTROL_IDLE )`) | move the three writes into a CONTROL_IDLE StateStartFuture; June has no `IsMyUnit` test there |
| Chung (June-only Heavy Field) | `CSI_SA_CTT_HEAVY_FIELD_LOOP` (`0x151`) `32c0ba60 @32c0ba9f-32c0bac2`: every frame, when vslot `0x198` is true, the same three writes, then the NPC state from a 9-way switch (`SHOT_ARTILLERY_STRIKE_*`, only when it differs) | none | comes with the skill |
```

### Verbatim: 7ae (heading L708)
- L728: `- 49 rows (Chung upgrade ids 26027-26029, 26216-26219) have no June `SKILL_ID` name; name them from the templet.`
- L718-720 default fallback: "An id whose type the class switch lists (BUFF / ACTIVE / SPECIAL_ACTIVE / HYPER) falls into that switch's `default:` and plays the class's first skill state (Elsword Mega Slash, Lire Power Attack, Arme Power Attack, Raven, Eve, Chung Detonation, Ara Wind Wedge); Elesis and Add leave it blank."
- Census context (L710-714): `june2017_re/s3_reform_ids_no_state.tsv`, 2,044 rows of 55 classes, "1,989 rows have none". Families: base 15 / 14 each, first jobs 28-34 / 27-33, second jobs 52-57 / 51-56.
- The file says "49 rows" but lists only 7 ids (26027-26029, 26216-26219); the other 42 are not named anywhere in the checklist.

### Verbatim: gap row (section 1 table, L53)
`| `Chung_Iron_Cannon` | 15 | 74 |` (columns: numerical-only DAMAGE_DATA tables | other). Backlog L348: "Gap scan (`charport_scan.py`, attacks whose damage is numerical-only): Aisha 1, Eve 3, Rena 12, Ara 14, Chung 15, Elsword 20, Elesis 20, Raven 25." The checklist itself has no "gap" line for Chung; 15 matches the table.

## Part B - rows that apply to every class (checklist, current L numbers)
Pure every-class (11): 2 (L109; L116 APPLIED_SKILL_ID in 167 scripts), 3 (L144 systems), 4 (L158 enum tables), 5 (L182 per-character parser census), 6 (L199 order of work; L211 mark-format rule), 7k (L457 class-set callback +0x8dc0, unowned), 7l (L463; L469 bounding states in all 12), 7o (L498; L506 all 12 `<prefix>DAMAGE_BOUNDING_DOWN` -4000), 7p (L533; L538 `TIME_STOP_IMMUNE_STATE`), 7x (L663; L668 every class port takes June's script with AT_COMMAND tags), 7ai (L830 buff-factor calls by BFI).
Mixed, with every-class parts (11): section 7 intro table L216 (rows L249-250, 255, 257, 260, 272, 274, 280), 7a (L293 modifier list), 7h (L411 registered in every class InitState), 7i (L427, all 12), 7t (L585; every June class script sets the key, each port replaces its table), 7ae (L708), 7ag (L800 pressed-index trap), 7ad (L742 motions), 7al (L880 new CX2GUUser hooks for every class, LIVE), 7am (L883), 7ao (L925; "Every class also brings" L932-937: emulator allow-list id, LevelUpEvent TYPE row + `DLG_Transcendental_Change_Guide_<Class>.lua`, `JuneGetHyperModeTableName` case, hyper-class skills + TRANSCENDENTAL_SKILL_DATA, four base helpers `31fcac90` / `31fcad70` / `31fceab0` / `31ff1e80`, hyper-effect start returns at once for a transcended unit). Also L698 (7ab area): the UpdateEquipCollisionData pair "exists for every class".
New since Eve's copy, none every-class: 7ap (L962, Chung row L969), 7aq (L974 Raven/Eve), 7ar (L982 item class compatibility for classes March lacks), 7as (L988 Lu/Ciel, Rose, Ain), 7at (L1008 Ara), 7au (L1014 Ain).

## Marks
- `(Aisha: CHn)` on 34 lines: L100-104, 214, 240, 259, 260, 275, 283, 327, 425, 441, 469, 506, 545, 547, 592, 622, 636, 733, 775, 778, 806, 822, 836, 845, 858, 862, 871, 918, 919, 921.
- `(Eve: CHn)` on 50 lines: L105-107, 162, 240, 251, 259, 260, 262, 275, 277, 280, 284, 287, 290, 301, 302, 313, 326, 327, 339-344, 350, 420, 424, 437, 491, 506, 545, 589, 592, 615, 622, 670, 727, 733, 814, 824, 837-841, 853, 854, 906, 907, 921.
- Chung marks: none. The four "(Chung" strings (L280, 492, 610, 728) are prose parentheses, not marks. Chung rows already carrying other marks: L259, 280, 287, 327, 441, 469, 921. Rule L211: `(<Name>: CHn)`, written in the phase that ports the class.
- Chung rows with no mark: L236, 246, 255, 278, 369, 492, 546, 570-571, 599, 610, 637, 644, 720, 728, 774, 897, 920, 954, 969.

## JUNE_PORT_BACKLOG_ORDER.md
- L129: "later paths for existing characters: Ara, Elesis, Add, Aisha, Chung, Eve and Rena".
- L348: gap scan, Chung 15 (numerical-only attacks).
- L393 (Stage 7): Eve closed 2026-10-28; "Next: Chung (`char-chung`, planning session `/x2-phase new-plan char-chung`)".
- L285 (no Chung word): class states for reform IDs are character-port rows (checklist 7ae).
- No backlog line names "7r" or the 49 ids; the Chung-extras link exists only in checklist 7ae / 7r text.

## JUNE_BUGS.md
- #164 (L181): Chung Artillery Strike: cinematic-UI write overwritten at once (CONTROL_IDLE StateStartFuture `32bff3d0 @32bff420`, StateStart `32bff370`); the source of checklist 7r.
- #332 (L348): skill state memo, per-class memo condition always says no; names `Chung_Iron_Cannon` only as one of the scripts with `EFFECT_SET_LIST_MEMO` (Add_NasodRuler, Chung_Iron_Cannon, Eve_Nasod).
