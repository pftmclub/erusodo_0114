# June character Rena (Stage 7) - Phase CH1m - Registration and the LIVE gate

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1m` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1k review. One at a time: `X2GULire_ElvenRanger.*`, `Always.h` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-rena/CH1m-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1m - Done" "CH1m - Review"
```
Then CH0's Done note, the earlier Rena CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_rena/ch0_*.tsv`. Raven's matching sub-phase is the worked example.

## Task

Port June's `InitState` `32cadc90` (vslot 0x7a4) and `InitStateCommon` whole (find InitStateCommon from vtable `335185bc`): cut-in set index (March's switch
`:1672-1715`), base event states, the START / WIN / LOSE roll (June's `INIT_COMMON_RANDOM_STATE` is keyed by class name), the hyper-mode table name
(`GetHyperModeTableName`, `31fdd490`), the per-job blocks for 3 / 12 / 13 / 24 / 102 / 103 / 114, every callback pointed at the function CH1a-CH1k wrote
(March's own name for `inside` rows, the new name for `new` rows), all from `ch0_state_skill.tsv` only (CH0 checked it against the listing). March's
registration stays in the `#else`, under `_LIVE`. March-only states June drops are not registered; June quirks are kept with a `JUNE_BUGS.md` row.
Reuse Raven's `ch1m_regsim.py` / `ch1m_regcheck.py` (per-class path simulation) with Rena's addresses. Done gate: the build passes with `_LIVE`
undefined, and the one-file probe with it defined compiles the new path; no play-test.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Nothing runs until `_LIVE`: no play-test; the hand-off names the next phase.

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

## Amended by CH0 (2026-11-07)

- **Functions: 3** (`awk -F'\t' '$7=="CH1m"' june2017_re/char_rena/ch0_class_map.tsv`; inside 1, new 2).
  June addresses: `32c96d60`(new ?) `32ca9dc0` `32cadc90`(new InitState).
- **States: 5** (`awk -F'\t' '$8=="CH1m"' june2017_re/char_rena/ch0_state_skill.tsv`): `USI_EVENT_PROTON_BOMB_CONTROL_FIRE`, `USI_EVENT_PROTON_BOMB_CONTROL_IDLE`, `USI_EVENT_SNIPER_CONTROL_FIRE`, `USI_EVENT_SNIPER_CONTROL_IDLE`, `USI_EVENT_SNIPER_END`.
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7h L428 USI_WAIT_STATE_CHANGE registered in every class InitState; §7l L472 bounding user states USI_BOUNDING_DOWN / _LANDCRASH / _UP in every cla; §7ao L951 every class also brings: emulator allow-list id, LevelUpEvent TYPE row.
- Registrars: InitStateCommon `32ca9dc0` and InitState `32cadc90` (all line states inline; no line registrar functions; InitPassiveSkillState registers no state). `charport_states.py init 32ca9dc0 32cadc90 3202b110` -> `ch0_reg.tsv`; `ch0c_listing_check.py` **202 / 202**, one per-arm row (`LESI_DASH`, other arm read by hand). START / WIN / LOSE rows 9 / 11 / 12 are std::string rolls; a `LESI_RNW_WIN2` roll sets byte `+0x9714`. InitState's tail: vslot 0x690 (InitPassiveSkillState), 0x684( 0 ), `31fd8f00`, `32c96d60`, then `32c85200( 1 )` (SetNaturalForceBuff) when the gauge count > 0 (CH1q's line). State ids: `ch0_state_enum_names.tsv` (218 registered, 89 shared names all with a June id different from the C++ value: register by June's id).

- **Amended by CH1i (2026-11-09): ten state names added to `LIRE_ELVENRANGER_STATE_ID` under `_CORE` for `SetEquippedSkillLevel`; register them by script table name:** `LESI_SA_LCR_SHARPFALL_HYPER_LANDING` 202, `LESI_RGA_GUNGNIR_READY` 283 ( the READY slot state; the id is the gap before 284 ), `LESI_SA_LCR_LOW_KICK` 295 ( an existing script table ), `LESI_SA_RGA_ACEINTHEHOLE_WAIT` 296 / `_1ST_ATTACK` 297 / `_WAIT_AFTER_1ST_ATTACK` 298 ( the 296 table again ) / `_2ND_ATTACK` 299 / `_WAIT_AFTER_2ND_ATTACK` 300 ( the 296 table again ) / `_3RD_ATTACK` 301, `LESI_SA_RNW_ERADICATION_ATTACK` 302. Siege READY is 167 ( `LESI_SIEGE_READY`, March member 115 + 52 ). `LESI_SI_A_LE_RISING_FALCON_LANDING` ( 227 ) keeps March's name ( June's table is `LESI_A_LE_RISING_FALCON_LANDING`, the same state ).
