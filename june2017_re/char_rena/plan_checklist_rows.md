# Rena (char-rena): checklist rows

Source: JUNE2017_CHARACTER_PORT_CHECKLIST.md (1048 lines; "L" = current line numbers, 2026-11-07). Read-only; only this file written.
Grep terms: Rena, Lire, LIRE, Lena, natural force, NaturalForce, NATURAL_FORCE, Wind Sneaker, Grand Archer, Night Watcher, Trapping/Combat/Sniping/Elven Ranger, LESI, LWS, LGA, LNW, RTR, RNW, June addresses `32c6xxxx`..`32caxxxx`. Class ids 3 / 12 / 13 / 24 / 102 / 103 / 114 as bare numbers match nothing specific to Rena (too generic; not listed).
June class code sits near `32c85838`..`32cb6290` (Rena vtable `33518d70`, registrar `32cb06d0`, SetEquippedSkillFunc `32cb06d0` in L444 row, state cancel `32cb9ee0`, delegate list `32ca7860`, hyper effect start `32c9b3d0`, NF count `32cb50c0`/`32cb6290`).
No `(Rena: CHn)` mark exists in any row yet (grep found none).

## Part A - rows naming Rena / Lire (37 hits, 34 rows)

| Sec | L | Row text (trimmed) | June addresses | Suggested phase |
|---|---|---|---|---|
| intro | 4 | "June update of a March one (Elsword, Aisha, Rena, Raven, Eve, Chung, Ara, Elesis, ..." | - | n/a (scope) |
| 1 | 56 | `Lire_ElvenRanger`: 12 numerical-only DAMAGE_DATA tables / 65 other | - | CH2/CH3 (gap 12; backlog L361 also says Rena 12) |
| 2 | 118 | `NATURAL_FORCE_TYPE`: unsupported (parsed, logged), "8c, done", used `Lire_ElvenRanger` x77. See section 3 | - | decision first (L149); data arrives with the script (CH1) |
| 3 | 149 | **Natural force**: code exists, gated off: `ADD_RENA_SYSTEM` is **off** under `US_SERVICE`; Rena (77 `NATURAL_FORCE_TYPE` uses). "**a decision for you**: turn `ADD_RENA_SYSTEM` on (the studio's own Rena code, untested in this build) or leave Rena without natural force. Phase 8c's read steps aside by itself when the flag is on. The same flag also gates the first-attack / relate-skill copies (`DamageData::m_eFirstAttack` / `m_RelateSkillData`) in the EffectSet and every damage-effect spawner, which June has unconditionally (C6a2: funcmap `321ef5ea-321ef632`, `DD+0x5a4`)" | `321ef5ea-321ef632` | CH0 decision (backlog L354, L366, L1524, L1724 say "ask when Rena is next / waits for Rena in Stage 7") |
| 6 | 206 | "4. Decide on `ADD_RENA_SYSTEM` (section 3) before touching Rena." | - | CH0 |
| 7 | 258 | Death-prevention `320072d0` / `3202b9a0` (every user, skill passives); trimmed row only matches via a Lire address | `320072d0`, `3202b9a0` | n/a (every class; check `32c8..` hit in the row) |
| 7 | 262 | Every class's vslot 510 skill lookup (June `SpecialAttackEventProcess` 509 calls first): row lists Add/Ain/Rose/Luciel only; Rena's `32cac200` is in March's file already as the quick-slot prefix (`X2GULire_ElvenRanger.cpp:5238`, Phase 7d) | `32cac200` (not in the row) | CH1 (lookup body / virtual 510) |
| 7 | 278 | Spec A / spec readers (option `+0x6bb0`, result `+0x6c50`); a `32c8..` reader is named | `328eab00`, `3297ee20`.. | check in CH0 which readers are Rena's (L939 lists Rena `32c85838`) |
| 7 | 286 | `3201f850` `SetSkillLevelStateData` hoisted into `CX2GUUser` (June base form) and its callers incl. `32c886b0`; "March's copies: El, Add, Ara, **Rena**, Raven"; multiply by `( 3201ad10( skill ) + 1.0 )` | `3201f850`, `32028190`, `32c886b0` | CH1l (Rena's copy `X2GULire_ElvenRanger.cpp:22990`; add the term or hoist). Marks: Aisha CH1i, Raven CH1l |
| 7 | 291 | June `data037_kom/Lire_Evoke_trap.src.lua` and `Lire_Evoke_trap_Memo.src.lua` (`INIT_COMPONENT USE_OWNERS_ENCHANT = TRUE`) (buff Phase 15 step 2b): trap NPC uses its owner's weapon enchant; NPC slots 111/112 forward to owner | `3275aa20`, `3275aa80` | CH0/CH1 (adopt both NPC scripts; Rena owns the Evoke trap) |
| 7b | 330 | per-class SetDamageExceptionProcess blocks incl. funcmap `32cad510` (Rena): passives keyed on June reform skill IDs (`RNSI_*`) | `32cad510` (also `329bc560`, `32b80f20`, `32bf28f0`, `32c5c080`, `32cccc70`, `32d1e0e0`, `32bca010`) | CH4b (March has no Rena override: follow Elsword's `32d1e0e0` shape; brief `kickoffs/june-effect/C8c.md`) |
| 7b | 331 | D8 (completion plan): **Rena's Powerful Bowstring 3405 -> 23405 and Sharp Kick 3400 -> 23400**; Aisha's Fitness 2413 replaced by Magical Staff 22409; "March's blocks stay until this port, which swaps in June's versions together with the reform skills" | - | CH4b (Rena re-keys; Aisha's CH4c done) |
| 7c | 357 | Effect-alpha tags that only character code applies: 8 sites (`32c89310`, `32c89460`, `32c895b0`; `32296da0` handles) | Rena | `32c89310`, `32c89460`, `32c895b0`, `32296da0` | CH2/CH3 (inside the skill states) |
| 7c | 358 | `322726f0`: the HP-rate gate in `GetFinalDamage` returns false for `RNSI_SA_ALP_SUPERSONIC` (29058) against an NPC defender (GameUnitType 1) when it has `+0x8aeb` / `+0x8ae9` set... | `322726f0` | CH4b (Supersonic HP-rate skip; row's owner column: Rena) |
| 7e | 363 | Unit-viewer effect rows: effects (MagicKnight, SPK, SBH, ALP, CDC, ETK/EIS, **RTR, RNW**: the table's 11 live rows now drive them) | - | n/a (done rows; the remainder is L372) |
| 7e | 372 | "lost under June's player: the SBH / ALP end windows and forced emit rates ..., the CDC / ETK / **RTR / RNW** weapon-equipped gate, the class-change mesh outline, and the effects in the avatar screen capture"; Elesis, Lunatic Psyker, Chung, Elsword, **Rena** | - | CH5 / UI sweep (RTR = Trapping Ranger, RNW = Night Watcher) |
| 7h | 422 | Per-class state EventProcess cancel branch: `UPDOWN_CANCEL_AFTER` (user `+0x8ca4`) up/down cancel; "Elsword 25 functions ..., Raven `32c2b360`, **Lire `32cb9ee0`**" | `32cb9ee0` | CH2/CH3 (inside that state's EventProcess; marks Elsword CH3a-c, Raven CH2/3) |
| 7h | 423 | `32024ff0` effect-set list variants (`EFFECT_SET_LIST_EX_TRANS_1/2`, `_ULTIMATE_EX`, `_FINAL`...), picked by the skill-memo entry `31fdbbb0`; matched via a Rena address (class-agnostic row) | `32024ff0`, `31fdbbb0` | CH2/CH3 (hit by any class using EFFECT_SET lists) |
| 7i | 444 | `SetEquippedSkillFuncBySkillSlotIndex` **Lire `32cb06d0`**: drop snapshot call, set state byte `+0x11` = 1 on normal and hyper entries (+`0x10` = `m_bHyperState`) | `32cb06d0` | CH1n (head). Marks: Aisha CH1k, Raven CH1n |
| 7n | 493 | User-sync class-skill float: **Rena** `32c88050` (Grand Archer `RNSI_P_RGA_RANDOM_SHOT_PLEASURE`, 23415: +/- by the up / down keys) via `32c86620`, `32c9d420`, `32ca2520`; "Registered by `32cb06d0` (Rena vtable `33518d70`)" | `32c88050`, `32c86620`, `32c9d420`, `32ca2520`, `32cb06d0` | CH4b / CH1 (Random Shot Pleasure passive) |
| 7u | 638 | March-only effect templets dropped: Rena: `X2GULire_ElvenRanger.cpp`: `DamageEffect_LGA_Arc_Shot`, `DamageEffect_Cryotron_Bolt_GUIDED_ARROW`; `Lire_ElvenRanger.lua`: `Effectset_Rena_DiveKickBombingA/B`, `..._B_Memo` | `c11a_march_only_templets.tsv` | CH3 (take June's caller, drop the March names) |
| 7u | 644-646 | Rena: `DamageEffect_Cryotron_Bolt_GUIDED_ARROW` is a June case rename, not a loss (June `DamageEffect_Cryotron_Bolt_Guided_Arrow`, `june2017_re/june_lua/DamageEffect.src.lua:28956`); March's `X2GULire_ElvenRanger.cpp:23238` asks for the all-caps key, Lua lookup is case-sensitive. "The Rena port takes June's caller and its spelling." | - | CH3 (Grand Archer Cryotron Bolt hyper) |
| 7x | 669 | Combo hits must carry June's `AT_COMMAND` tag: June scripts tag combo DAMAGE_DATA `ATTACK_TYPE = ATTACK_TYPE.AT_COMMAND` (e.g. `Lire_ElvenRanger`: **52 `AT_COMMAND`, 11 `AT_SPECIAL`**); March's leave combos untagged | - | CH1 script + CH2 (combo states; see Raven's 46 AT_COMMAND note) |
| 7ae | 724 | "falls into that switch's `default:` and plays the class's first skill state (Elsword Mega Slash, **Lire Power Attack**, Arme Power Attack, Raven, ...)" (census of June skill ids with no March state) | `s3_reform_ids_no_state.tsv` | CH2 / CH3 (bind each id; check unnamed ids in CH0). Row 718/723 census is general |
| 7ad | 758, 760 | Character motions: June's renamed animations: "Lire's `ComboX3` / `ComboX4` / `ComboX4Up` / `ComboX4Down` / `DashAttackX` became `Lire_ComboX3` ..." and "skill animations of Add, Ara, Chung, Elesis and Lire" | `r12b_anim_loss.tsv` | CH0b (stage June's Lire motions with the script, deploy with CH1) |
| 7ac | 782 | CanDown's June call shape in class code: "Rena Grand Archer `LESI_RGA_GUNGNIR_READY` (`@32ca111a`)" | `32ca111a` | CH3 (Grand Archer Gungnir ready) |
| 7am | 912 | Unit delegate-list registrations per class: **Rena `32ca7860`**: A `32ca2180` (Open Mind), A `32ca2390`, A `32c86790`, A `32ca28b0` | `32ca7860`, `32ca2180`, `32ca2390`, `32c86790`, `32ca28b0` | CH4b |
| 7am | 924, 925 | "A `3275a490` (owner `UT_LIRE` -> Rena's `32ca2180`). Port each with its class (Eve, Rena); the registration lives in `CX2GUNPC`." / Eve's `3275a120` and `CX2GUNPC::June_InitDelegateProcess`... "Rena adds `3275a490` there." | `3275a490`, `32ca2180` | CH4b (NPC-side delegate for the Evoke trap) |
| 7an | 939 | `GetCharacterSpecializationB()` (June `+0x6d04`) readers: Eve `329b407b`, Add `32a01ca2`, ..., Raven `32c49410` / `32c507da`, **Rena `32c85838`** (Aisha `32cd0197`) | `32c85838` | CH1q (marks: Eve CH1q, Raven CH1q) |
| 7ao | 959 | Transcendence per class: "**Rena**: `ApplyHyperModeBuff` `32298590 @322985ec`: unit type 3 -> `32cb50c0` count x 15.0 to vslot `0xf0`, then `32cb6290( 5 )` (ADD_RENA_SYSTEM's ...)" | `32298590`, `32cb50c0`, `32cb6290` | CH5/CH6 (tied to the ADD_RENA_SYSTEM decision; backlog L236 cites `32cb6290( 1 )` case 3 `UT_LIRE`) |
| 7ao | 969 | Transcendence table: **Rena** `32c9b3d0` (hyper effect start, vslot 49) | `32c9b3d0` | CH5/CH6 (`HyperModeBuffEffectStart` .cpp:21333) |
| 7ap | 987 | June slots CH1o2 gave a base and a March call: "**Lire:** 0x6a8 `32c7bed0` (skill use `32cac5a0`)" (Chung 0x6a8 `32bdba60`) | `32c7bed0`, `32cac5a0` | CH1o / CH1l (ShownSkillCoolTime for Lire) |
| 7av | 1045 | "A class's state animation rate, user vslot 0x824: **Lire:** March's inline `SI_P_LCR_AGILE_MOVEMENT` block in `ResetAnimSpeed` moves to Lire's `0x824` override (read its vtable)" | vslot `0x824` | CH1 (animation rate override) |

Notes: L258 / L262 / L278 / L423 are class-generic rows that surfaced through a Lire / `32c8..` address; confirm in CH0 before assigning a phase. Rows L1-L3 of the intro and L56 carry the numerical-only gap count (12).

## Part B - JUNE_PORT_BACKLOG_ORDER.md lines naming Rena / Lire / natural force / ADD_RENA_SYSTEM

| L | Text (trimmed) |
|---|---|
| 129 | "later paths for existing characters: Ara, Elesis, Add, Aisha, Chung, Eve and Rena;" |
| 236 | "... Rena `32cb6290( 1 )` (case 3 `UT_LIRE`; Raven has no case, char-raven CH0), Chung cannon balls and extra balls +1, Ara `32a31a00` + 1, ..." |
| 354 | "**`ADD_RENA_SYSTEM`** needs the user's decision if the pilot is Rena; otherwise it waits for Stage 7." |
| 361 | "**Gap scan** (`charport_scan.py`, attacks whose damage is numerical-only): Aisha 1, Eve 3, Rena 12, Ara 14, Chung 15, Elsword 20, Elesis 20, Raven 25. Add's March kit is a skeleton." |
| 366 | "**`ADD_RENA_SYSTEM`:** waits for Rena in Stage 7." |
| 410 | "**Closed 2026-11-07 (char-raven Z).** Raven's seven classes (4, 16, 17, 25, 106, 107, 115) run on June's core ..." (template for Rena's close-out line; no Rena text) |
| 1413 | "`CX2GUNPC::AttackResultByType` `32773ee0` for every non-user attacker; March does it only under `ADD_RENA_SYSTEM` (off; char-rena's planning asks for it)." |
| 1524 | "\| **`ADD_RENA_SYSTEM`** (Rena's natural force) \| 5 or 7 \| Ask when Rena is next. \|" |
| 1724 | "... characters by charport_scan.py gap; ask the ADD_RENA_SYSTEM decision when Rena is ..." |
