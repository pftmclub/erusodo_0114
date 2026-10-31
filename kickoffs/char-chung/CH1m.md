# June character Chung (Stage 7) - Phase CH1m - Registration and the LIVE gate

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1m` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1l4 review. One at a time: `X2GUChung.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_CHUNG_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_CHUNG_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-chung/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "Starting context" "Phase CH0 - Chung map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "CH1m - Done (2026-10-26)" "CH1m - Review (2026-10-27)"
```
Then CH0's Done note, the CH1a-CH1l4 Done notes and reviews (their routed gaps), and the matching rows of `june2017_re/char_chung/ch0_*.tsv`.
Eve's matching sub-phase is the worked example.

## Task

Port June's `InitState` `32c22540` (vslot 0x7a4) and `InitStateCommon` whole: cut-in set index, base event states, the START / WIN / LOSE roll
`31fe22d0`, the hyper-mode table name (`GetHyperModeTableName`, `31fdd490`), the per-job blocks for 6 / 20 / 21 / 27 / 110 / 111 / 117, every
callback pointed at the function CH1a-CH1l4 wrote (March's own name for `inside` rows, the new name for `new` rows), all from `ch0_state_skill.tsv`
only (CH0 checked it against the listing). March's registration stays in the `#else`, under `_LIVE`. March-only states June drops are not
registered; June quirks (a registration with no script table) are kept with a `JUNE_BUGS.md` row. Reuse Eve's `ch1m_regsim.py` / `ch1m_regcheck.py`
(per-class path simulation) with Chung's addresses.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GUChung.*` / `X2GUUser.*` are
CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit. Done gate: the build passes with `_LIVE` undefined, and the one-file probe with it defined compiles the new path; no play-test.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.

## Amended by CH0 (2026-10-31)

- **Functions: 3** (`awk -F'\t' '$7=="CH1m"' june2017_re/char_chung/ch0_class_map.tsv`; inside 3).
  June addresses: `32c12a30` `32c1d1d0` `32c22540`.
- **States: 44** (`awk -F'\t' '$8=="CH1m"' june2017_re/char_chung/ch0_state_skill.tsv`): `CSI_START`, `CSI_WIN`, `CSI_WIN2`, `CSI_LOSE`, `CSI_LOSE2`, `CSI_CFG_START`, `CSI_CFG_WIN`, `CSI_CFG_WIN2`, `CSI_CFG_LOSE`, `CSI_CFG_LOSE2`, `CSI_CSG_START`, `CSI_CSG_WIN`, `CSI_CSG_WIN2`, `CSI_CSG_LOSE`, `CSI_CSG_LOSE2`, `CSI_CIP_START`, `CSI_CIP_WIN`, `CSI_CIP_WIN2`, `CSI_CIP_LOSE`, `CSI_CIP_LOSE2`, `CSI_CDC_START`, `CSI_CDC_WIN`, `CSI_CDC_WIN2`, `CSI_CDC_LOSE`, `CSI_CDC_LOSE2`, `CSI_CHG_START`, `CSI_CHG_WIN`, `CSI_CHG_WIN2`, `CSI_CHG_LOSE`, `CSI_CHG_LOSE2`, `CSI_CTT_START`, `CSI_CTT_WIN`, `CSI_CTT_WIN2`, `CSI_CTT_LOSE`, `CSI_CTT_LOSE2`, `CSI_HYPER_MODE`, `CSI_CIP_HYPER_MODE`, `CSI_CDC_HYPER_MODE`, `CSI_CTT_HYPER_MODE`, `USI_EVENT_PROTON_BOMB_CONTROL_FIRE`, `USI_EVENT_PROTON_BOMB_CONTROL_IDLE`, `USI_EVENT_SNIPER_CONTROL_FIRE`, `USI_EVENT_SNIPER_CONTROL_IDLE`, `USI_EVENT_SNIPER_END`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7h L425 USI_WAIT_STATE_CHANGE registered in every class InitState; WIN OnEvent; §7l L469 bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every cla; §7ao L932-937 every class: emulator transcend allow-list id, LevelUpEvent TYPE row, .
- **Callees this Opus phase maps itself:** `32078bc0` June polymorphic object ctor (vtable 33473170, 0x19 dwords zeroed, +8 = 1.0f).
- InitState `32c22540` is **not carved in Ghidra** (no decompile): its 4 registrations were read from the listing (`ch0_reg.tsv` rows `@32c2277e` / `@32c22852` / `@32c2291f` / `@32c229db`: START / WIN / LOSE roll `31fe22d0`, `CSI_WAIT_STATE_CHANGE` id 130). It then sets +0x7d04 = 0xa2 and +0x4088 = 13 and the cut-in set index +0x8ac8 by class: 6 -> 0, 20 -> 1, 21 -> 2, 27 -> 5, 110 -> 3, 111 -> 4, 117 -> 6 (byte table `32c22af8`).
- InitStateCommon is two functions: `32c12a30` (110 rows, no gate; `USI_HYPER_MODE` id 23 named by `31fdd490` at line 737) and `32c1d1d0` (116 rows, class-gated on *(+0x867c)+0x38, two switches with fallthroughs 110 -> 20 and 117 -> 27; line ranges in `ch0_build_state_skill.py` GATES). `ch0c_listing_check.py` reproduced 188 / 188 distinct rows.
- June registers six names with no script table (`CSI_RIDING_DAMAGE_BACK`, `USI_EVENT_SNIPER_*` x3, `USI_EVENT_PROTON_BOMB_*` x2 from the base `3202b110`) and drops three March states (`CSI_SI_A_CDC_LEG_SHOT`, `CSI_SI_A_SIEGE_SHELLING_READY_HYPER`, `CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL`: June wins).
- Skill 26032 registers `DUAL_BUSTER` a second time with id 336 (0x150) (listing, `ch0_skill_listing_rows.tsv`). `32078bc0` (unknown, called 21x by the registrars) is yours to map.

## Amended by CH1b review (2026-10-31)

- **§7l bounding:** the three `CSI_DAMAGE_BOUNDING_*` rows carry five callbacks (thunks `329cc970`-`329cc9b0` to `CX2GUUser` vslots 0x5d4-0x5e4); Chung overrides none, and the bodies are already `CX2GUUser`'s (Effect C3d-3, `SERV_IRUHADEV_JUNE_BOUNDING_STATE`). Register them with `InitBoundingStates( "CSI_" )` (`X2GUUser.cpp:32816`), the way Arme (`X2GUArme_VioletMage.cpp:3920`) and Eve (`X2GuEve.cpp:1979`) do; CH1n has no bounding body to write.

## Amended by CH1c (2026-10-31)

- **Leave out the 8 states with no March callbacks, as Eve and Aisha did:** `CSI_AMUSEMENT_ITEM_PHOENIX_RUN` (PvP, one state with three callback triples `329cc870..329cc930`), `CSI_DANCE_Z` / `_X` / `_WAIT` (Stage 10) and `CSI_RIDING_SPECIFIC` / `CSI_RIDING_ATTACK_X_READY` / `_READY_2` / `_END` (Stage 17, `JUNE_PORT_BACKLOG_ORDER.md`). Each gets a comment line with its owner. Every other riding / pepper-run row (`CSI_PEPPER_RUN*`, `CSI_RIDING_*`, 27 tables) takes the existing `CX2GUUser` member by its March name; `CSI_RIDING_HYPER_MODE` FrameMove is `CX2GUChung::RidingHyperModeFrameMove` (CH1c) and `CSI_RIDING_DIE` uses CH1b's `DieFront*`.
- `CSI_RIDING_DAMAGE_BACK` has no script table in `Chung_Iron_Cannon.lua`: register it as June does (JUNE_BUGS 489).

## Amended by CH1a review (2026-10-31)

- **Register `JumpUpStart`** (June `32bdbcd0`, compiled under `_CORE` since CH1a) as `CSI_JUMP_UP`'s StateStart. CH0 proposed
  `CSI_JUMP_UP_StateStart`; CH1a named it in the class pattern, and the class map's `proposed_name` now says `JumpUpStart`.

## Amended by CH1f (2026-10-31)

- **Register the two `new` Iron Paladin dash-jump functions:** `CIPComboDashJumpXXEventProcess` (June `32be9930`, `CSI_CIP_COMBO_DASHJUMP_XX`, script id 325)
  and `CIPComboDashJumpXXLandingEventProcess` (June `32be9960`, `CSI_CIP_COMBO_DASHJUMP_XX_LANDING`, script id 326, enumerator added by CH1f).
  The Landing FrameMove is CH1e's `CFGComboDashZFrameMove` (`32c01be0`). Neither state has a Start or End callback of its own in June's tables.
