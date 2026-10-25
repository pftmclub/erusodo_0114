# Eve (char-eve): checklist rows and June class range

Source A: JUNE2017_CHARACTER_PORT_CHECKLIST.md (929 lines; line numbers below are that file's, "L" prefix).
Source B: june2017_re/funcmap.tsv and the live Ghidra HTTP server (127.0.0.1:8080; read-only calls only: list_functions, get_function_by_address, xrefs_to, xrefs_from).
Nothing was edited, committed or renamed.

## Part A - rows

Scope key: EVE = names Eve in the section; ALL = applies to every class port (stated or by nature); OTHER = another character's only.
Row count: 6 numbered sections, the section-7 intro table, and 42 headed subsections 7a..7ao (the file has two headings called "7g": L368 Rose, L386 item fields; here 7g(a) and 7g(b)).
In the Eve column, `|` inside a quote is written as `/` to keep the table intact.

| Sec | Title | Lines | Scope | Eve text (verbatim, line) |
|---|---|---|---|---|
| 1 | Blocker: June damage numbers come from a table this build lacks | 37-105 | EVE + ALL | L57 `Eve_Nasod` row: 3 numerical-only DAMAGE_DATA tables / 25 other |
| 2 | DamageData keys March does not parse | 106-140 | ALL | not named; L113 "167: every character script except `Add_NasodRuler`, `Ain_Angel`, `Rose_Mechanic` and `Rose_Ranger`" (APPLIED_SKILL_ID) |
| 3 | Systems a character needs | 141-154 | ALL (rows L145, L147); Rena / Lu / Rose rows OTHER | not named |
| 4 | Enum tables | 155-178 | ALL (tables per port) | not named |
| 5 | What no scan covers | 179-195 | ALL (per-character parser census) | not named |
| 6 | Order of work | 196-212 | ALL (L211 holds the mark-format rule) | not named |
| 7 | Buff-engine hooks (intro table, 58 rows) | 213-287 | EVE + ALL + OTHER | L237 "Eve (`3299b7f9`, BFI 812)"; L248 "/ `32976fe0` (non-virtual, caller `329b4ab1`) / Eve / Sets the cooldown of `RNSI_SA_ECN_PSYKER_KINESIS` (25235, a June-only Eve skill)"; L256 "(Elsword, Eve, Arme, EL, Ara, Chung, Raven, Lire)" (7d already put the quick-slot prefix into March's Eve); L277 "`329968a0` (Eve range)"; L281 "June NPC scripts `Nasod_Watch_Eve`, `SI_HA_FERDINAND` (Eve) ... call `SetMultiplyPowerRate_LUA`" (row class "Eve, Add"); L285 "`32978ec2` (`CX2GUEve` vslot `0x6fc`)" tutorial-flag read. ALL rows: L249, L250, L257 (dev free-skill gate), L272, L274, L280 |
| 7a | Effect-engine hooks (modifier list, spawn inherit) | 288-309 | EVE + ALL | L297 "`3299a9d0` (`SA_EBS_Extreme_Virtual`: tags 16, 12, 20); `329c12a0` (`31f649d0` x5, `31f65e40`/`31f65f90`); `329a3040`, `329a3760` (Thousand Star); `32968c50`, `3297bad0` ... / Eve / Ignore-defence and damage-rate inheritance on her projectiles"; ALL: L296 (`AddDamageEffectModifier`, "all of the below"); L306 is the Aisha tag-2 note |
| 7b | SetDamageExceptionProcess hooks | 310-322 | EVE + ALL | L320 "the per-class blocks in funcmap rows `329bc560`, `32b80f20`, ... / Eve, Elesis, Chung, Raven, Rena, Aisha, Elsword, Ara / Passives keyed on June reform skill IDs (`RNSI_*`)" |
| 7c | Effect-alpha tags | 323-343 | EVE | L337 "167 sites in 0x32965e30-0x329cf260 (`32296d60` particle x119, `32296ee0` mesh x41, `32296da0` / `32296e40` handles x7) / Eve / Tags her particles and meshes with her alpha category right after creating them / one `CX2Game` tag-helper call per site, inside the class code being ported" |
| 7e | Unit-viewer effect rows | 344-357 | OTHER (Ara, Elesis, Lu/Ciel, Rose, Ain) | not named |
| 7d | Damage-math hooks | 358-367 | OTHER (Add, Luciel) | not named |
| 7g(a) | Lobby habit motion, unit-type remap | 368-377 | OTHER (Rose) | not named |
| 7f | NPC anim-speed hook | 378-385 | OTHER (Rose) | not named |
| 7g(b) | Item-templet fields | 386-397 | OTHER (Lu/Ciel, Add) | not named |
| 7h | Unit-state readers | 398-412 | EVE + ALL | L411 "registered in every class InitState (all 12, e.g. Eve `329ba9c0 @329bae2a-329bae48` ...) and WIN's OnEventProcess = the delegate `320718d0` (all 12 class Inits, e.g. Eve `329bac9e`)"; ALL rows L407, L410; Elsword / Raven / Lire row OTHER |
| 7i | Skill-slot re-equip snapshot call | 413-428 | ALL + EVE | L415 "Every June class's `SetEquippedSkillFuncBySkillSlotIndex` (vslot `0x7b4`, slot 493) calls `31fd2af0( normal state )`"; L427 "... `329c8a00` Eve ..." / all 12 / "Nothing for the snapshot"; the +0x11 byte store is dropped at the port (Aisha CH1k mark) |
| 7j | DamageReact damage steps | 429-442 | OTHER (Elesis, Lu/Ciel, Rose, Elsword) | not named |
| 7k | Class-set callback (+0x8dc0) | 443-448 | ALL, unowned ("The class port that finds a writer adds the member") | not named |
| 7l | Super-armour and bounding hooks | 449-460 | ALL (L455 bounding states in all 12) + Add / Rose / Luciel OTHER | not named (Eve implied by "all 12") |
| 7m | Die notify | 461-467 | OTHER (Add) | not named |
| 7n | User-sync class-skill float | 468-483 | EVE | L477 "/ Eve / `3297d730` / `329a67b0` / Registered by `329c8a00` (Eve vtable `334cfc10`)." (writer `3297d730`, reader `329a67b0`) |
| 7o | Fall-speed limit data | 484-518 | ALL (L492 all 12 classes, `<prefix>DAMAGE_BOUNDING_DOWN` -4000) + others | not named |
| 7p | Hit-stop sites, time-stop immune | 519-535 | ALL (L531 `TIME_STOP_IMMUNE_STATE`, all classes) + Chung / Aisha / Elsword OTHER | not named |
| 7q | Luciel down-state getters | 536-548 | OTHER | not named |
| 7r | Chung cinematic-UI writes | 549-558 | OTHER | not named ("Eve" appears on L556 only inside a Chung state name) |
| 7s | Dungeon-registry class tables | 559-570 | OTHER (new classes) | L562 "(`X2DungeonManager.cpp:252-552` reads COMMON / ELSWORD / AISHA / RENA / RAVEN / EVE / CHUNG / ARA / ELESIS / ADD only)": March already reads Eve, nothing to add |
| 7t | Hyper cut-in data and class hooks | 571-609 | ALL + EVE | L575 "every June class script sets the key in its awakening states"; L600 sound sets: "... `Eve_Nasod`, `Rose_Ranger`), and `STATE_SOUND_PLAY<n>`" (`SOUND_SET3_RATE` and up); L607 "Each class port replaces its table with June's names" |
| 7u | March-only effect templets dropped | 610-633 | EVE | L624 "/ Eve / `X2GuEve.cpp`: `EVE_EP_DASH_COMBO_Z` / - /" |
| 7v | Lu/Ciel minor-unit slot key | 634-640 | OTHER | not named |
| 7w | CUSTOM_RENDER_STATE class users | 641-648 | OTHER (Add, Ara, Ain, Luciel, Rose) | not named |
| 7x | Combo hits must carry AT_COMMAND | 649-657 | ALL | not named (L654 "Every class port takes June's script with its tags") |
| 7y | GetLandPosition bMonster | 658-667 | EVE, no work | L662 "Eve's Dimension Link states pass `true` (already March's)" |
| 7z | Lu/Ciel skin equipment | 668-674 | OTHER | not named |
| 7aa | Lu/Ciel attach-anim models | 675-683 | OTHER | not named |
| 7ab | Rose weapon attach frames | 684-689 | OTHER | not named |
| 7ae | June skill-tree ids with no March class state | 690-722 | ALL + EVE | L700-702 an id whose type the class switch lists "falls into that switch's `default:` and plays the class's first skill state (Elsword Mega Slash, Lire Power Attack, Arme Power Attack, Raven, / Eve, Chung Detonation, Ara Wind Wedge); Elesis and Add leave it blank." |
| 7ad | Character motions: renamed animations | 723-744 | ALL March classes + EVE | L729 "(Elsword, Arme and Eve: none)" lost animations; L733 stage pattern `^motion_(elsword/arme/lire/raven/eve/chung/ara/el/add)[_.]` |
| 7ac | CanDown June call shape | 745-760 | OTHER (Ara, Chung, Aisha, Add, Rena, Luciel) | not named |
| 7af | Camera linear tracking / view roll | 761-777 | OTHER (Ain) | not named |
| 7ag | Pressed-index space, 5th slot, A5/B5 (the trap) | 778-792 | ALL + EVE | L788 "`X2GuEve.cpp` (Mega Electronball, Taser Pilum, Kugelblitz) stores a **slot id** that is March's 4 + 4 (A 0-3, B 4-7), the pressed-index space: its readers are the key switches (`case 4: oneQ`), the `iSkillSlotIndexPressed_ =` assignments, the three `GetSkillSlotData*` decoders (`> 3`, `-4`) and the state-array lookup."; L790 "(2026-10-18) put them back to March's (`if( iSlot >= 8 ) break;`) ... An Eve port converts the encoders, every reader above and the pressed index together." |
| 7ah | 10-slot dialog class statics, key-config | 793-805 | EVE | L797-798 "`Static_EveRecoveryGuide` / `_Cooltime` / `Button_EveRecoveryToolTip`" (hidden by the file; the class port shows and feeds them; June driver `CX2Game` `322cd5c0`-`322e07e0`) |
| 7ai | Buff-factor calls by BFI, reserved-buff hook | 806-819 | ALL | not named (Eve's BFI 812 call is L237; map in `june2017_re/s3e_bybfi_callers.tsv`) |
| 7aj | Tree-UI class pieces | 820-837 | EVE (+ others) | L823 "per class type (0x6d, 0x7b, 0x72, 0x73, 0x1a Eve, 0x7e, 0x24) a skill shows another icon while a class state holds"; L824 "(Eve's El Crystal index at tree `+0x134`, Add's `+0x138` / `+0x139`, the game unit's vslot 0x98 / 0x56c). March keeps only its Eve `UpdateEveElectraSkillSlotIcon` call, which S7b ports as June's tail." |
| 7ak | Skill page decide ACK | 838-843 | OTHER (Aisha, Add) | not named |
| 7al | Class pieces CH1l found in user base | 844-852 | OTHER (an unnamed "class 13"); L850 new CX2GUUser hooks "for every class" are already LIVE | not named |
| 7am | Unit delegate-list registrations | 853-878 | ALL + EVE | L866 "/ Eve / none / (her summons register through the NPC, below) /"; L875-876 "NPC-side pushes for two classes' summons ... every NPC gets A `3275a120` (owner `UT_EVE`, skill 25417, MP text) and A `3275a490` (owner `UT_LIRE` -> Rena's `32ca2180`). Port each with its class (Eve, Rena); the registration lives in `CX2GUNPC`." |
| 7an | User passive hooks, specialization readers | 879-893 | EVE | L890 "`GetCharacterSpecializationB()` (June `+0x6d04`) readers / Eve `329b407b`, Add `32a01ca2`, ... / the reading function, whole" (vslot 0x428 / 0x73c override rows L887-888 name no Eve override) |
| 7ao | Transcendence, per class | 894-929 | EVE + ALL | L922 "/ Eve / `32994710` (hyper effect start, vslot 19) / the branch /"; ALL: L901-907 "Every class also brings" (emulator allow-list id, LevelUpEvent TYPE row + `DLG_Transcendental_Change_Guide_<Class>.lua`, `JuneGetHyperModeTableName` case, hyper-class skills + `TRANSCENDENTAL_SKILL_DATA`, four base helpers `31fcac90` / `31fcad70` / `31fceab0` / `31ff1e80` if copied, hyper-effect start returns at once for a transcended unit) |

Totals over the 49 rows (6 + the 7 intro + 42 subsections):
- Eve named explicitly: 20 (1, 7, 7a, 7b, 7c, 7h, 7i, 7n, 7s [no-op], 7t, 7u, 7y [no-op], 7ae, 7ad, 7ag, 7ah, 7aj, 7am, 7an, 7ao).
- Every-class, Eve not named: 11 (2, 3, 4, 5, 6, 7k, 7l, 7o, 7p, 7x, 7ai). Several Eve-named rows are also every-class (1, 7, 7a, 7h, 7i, 7t, 7ae, 7ag, 7am, 7ao).
- Other character only: 18 (7e, 7d, 7g(a), 7f, 7g(b), 7j, 7m, 7q, 7r, 7v, 7w, 7z, 7aa, 7ab, 7ac, 7af, 7ak, 7al).
- Eve rows that need no work: 7s (March reads EVE), 7y (March already passes true), 7ad (no lost animations), the L256 prefix (already in March's Eve).

### Known Eve items, located
- 167 alpha-tag sites (7c): L337. `june2017_re/c10a_alpha_sites.tsv` has exactly 167 rows with class_region `CX2GUEve`, disposition `checklist 7c`; first site 32982623 (in FUN_329824c0), last 329c84f8.
- Slot encoders in X2GuEve.cpp (7ag trap): L788-791 (Mega Electronball, Taser Pilum, Kugelblitz).
- El Crystal icons: 7aj L823-824 (Eve unit type 0x1a, tree +0x134, `UpdateEveElectraSkillSlotIcon`). Recovery statics: 7ah L797-798 (`Static_EveRecoveryGuide`, `_Cooltime`, `Button_EveRecoveryToolTip`). 7aj does not mention the recovery statics.
- 7a modifier: L296 (the list, all classes) + L297 (Eve's tags 16 / 12 / 20 and spawn-inherit sites).
- 7n sync float: L477.
- 7ae unnamed upgrade ids: L700-702.
- 7am delegate-list pushes: L866 (none for Eve's own class), L875-876 (NPC-side push, owner UT_EVE, skill 25417).
- 7an hooks: L890 (`GetCharacterSpecializationB` reader `329b407b`).
- 7ao transcendence readers: L922 (hyper effect start `32994710`, vslot 19). Eve's row lists no engine-code readers (Elsword's L918 has `32063650`, `32610cb0`); the four base helpers are in the "Every class also brings" list (L905).

### Mark format used for Aisha (so Eve can copy it)
- Marks go in place at the end of the row or sub-bullet: `(Aisha: CH2b - <what was done>)`, `(Aisha: CH1m, <what>)`, `(Aisha: CH4c)`, `(Aisha: CH4d, <what>)`. Mark lines: L100-104, 211, 237, 256-257, 272, 280, 320-321, 411, 427, 455, 492, 531, 533, 578, 608, 622, 714, 756, 759, 784, 799, 812, 816, 828, 832, 841, 887-890.
- Variants: `n/a:` form "(Aisha: CH4c, n/a: Aisha never calls it. ...)" (L714); "(Aisha CH0, 2026-10-20: n/a. ...)" (L533); the 7ao done-row L919 "**done, june-transcend T6 (2026-10-24):** ...".
- Marks name a phase (CHn), never a commit hash; the file has no `<commit>` field. Stage-7 rule L211: "Stage 7 characters mark their rows the same way, `(<Name>: CHn)`, in the phase that ports them." So Eve would be `(Eve: CHn ...)`.

## Part B - Eve's June class range in unpacked_x2.exe

Result: Eve occupies 0x32965dd0 - 0x329cf86f (first vtable override 0x32965dd0, last override 0x329cf670; Add's first override 0x329cf870 is the next class).

Method (calibrated on Aisha): read the class vtable entry by entry with Ghidra `xrefs_from` (each vtable slot address returns "To <addr> to function ..."), then take the lowest and highest override in the class's own block (excluding base-class and engine targets).
- Calibration: Aisha vtable 3351d0b4 (funcmap row `32cede00`, "RTTI vtable 3351d0b4 slot 493"; row at funcmap L2026) gives in-block overrides 32cb6e00 .. 32cf2d50. This reproduces the stated Aisha range: start 32cb6e00, end = Elsword start 32cf2da0 (Ghidra: Body 32cf2da0 - 32cf2e16). The Aisha start 32cb6e00 is not a defined Ghidra function (get_function_by_address: "No function found").
- Eve vtable 334cf45c. Evidence: funcmap L2016 (`329c8a00 CX2GUEve::SetEquippedSkillFuncBySkillSlotIndex ... RTTI vtable 334cf45c slot 493`), L197 (`329bc560 ... slot 0x2b8 of 334cf45c`; Ghidra xrefs_from 334cf714 -> 329bc560). Ghidra xrefs_to 0x334cf45c: written by FUN_3299bd70 (`MOV [ESI],0x334cf45c`, Body 3299bd70-3299cdb9, the constructor) and FUN_329aece0 (Body 329aece0-329af453). 549 slots read, 441 distinct targets, 69 inside the Eve block.
- In-block overrides: first 0x32965dd0 (Body 32965dd0-32965df0), last 0x329cf670 (Body 329cf670-329cf68d). The checklist's own span (L337, 0x32965e30-0x329cf260) lies inside this; the 167 c10a sites lie 32982623-329c84f8.
- Neighbours: Add (vtable 334daaec; xrefs_to: constructors in FUN_32a22170 and FUN_32a23b90) has first in-block override 0x329cf870 (Body 329cf870-329d172d) and last 0x32a316c0; Ain (vtable 334e0e2c) starts at 0x32a38770. Between Eve's last override and Add's first sit 329cf6d0 (Body 329cf6d0-329cf832; funcmap: DamageData::PushBuffFactor variants), 329cf850 and 329cf860; which TU owns them is not decided here. Before Eve: 32965d90 and 32965dc0 are zero-size Ghidra entries, and 329656d0-329658b0 are Eqip / engine helpers per funcmap (CKTDXDeviceXSkinMesh frame lookup, CX2Unit minor-unit item id, CKTDGXSkinAnim), not a class.
- Ghidra function counts (list_functions; 55,556 functions in the program): 643 in [32965dd0, 329cf670]; 646 in [32965dd0, 329cf870) (adds 329cf6d0, 329cf850, 329cf860); 641 in the checklist span [32965e30, 329cf260]. Aisha, same method: 426 in [32cb6e00, 32cf2da0).
- funcmap.tsv rows with a June address in [32965dd0, 329cf870): 31. Rows naming CX2GUEve: 329bc560 (L197), 32978420 (L842, ApplyRenderParamOnEmptyRenderParamByBuff), 32969300 (L1059, SpecialAttackEventProcess), 329c8a00 (L2016), plus L3122 (`32989843 (R12b) X2GuEve.cpp:15552 / 15570 AddModelXSkinMesh (no Ghidra function)`) and L3029 (vslot 0x7e4 over the vtables 334781f4, 334cf45c ...).
- Not found: an RTTI string `.?AVCX2GUEve@@` (Ghidra `strings` shows only `.?AVMemoConditionEve@CX2GUEve@@` at 3373d0f0 and the path string `X2GuEve.cpp` at 334cdbc0, which has no xrefs); no class-range table in june2017_re/*.tsv (c3z4_class_gap.tsv is a method census, not a range table).
- Small discrepancies in the existing notes: funcmap L1059 calls 334cfc54 "slot 509", but 334cf45c + 0x7f8 = 334cfc54 is slot 510 by address; the checklist's "Eve vtable `334cfc10`" (L477) is the slot-0x7b4 address (334cf45c + 0x7b4), not the vtable base 334cf45c.
