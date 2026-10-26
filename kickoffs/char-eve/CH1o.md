# June character Eve (Stage 7) - Phase CH1o - Virtuals under LIVE

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1o` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1n review. One at a time: `X2GuEve.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE`, `SERV_IRUHADEV_JUNE_CHAR_EVE_LIVE` (undefined until CH1 final) |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-eve/CH1o-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Starting context" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "CH1 split (2026-10-21, user)"
```
Then CH0's Done note, the earlier CH1 Done notes / reviews (their routed gaps), and the `CH1o` rows of `june2017_re/char_eve/ch0_*.tsv`. Aisha's matching sub-phase (CH1j / CH1k / CH1l / CH1m) is the worked example.

## Task

Switch the virtual overrides CH1k / CH1l transcribed to their `June_*` bodies under `_LIVE` (March body in `#else`), and close every gap the CH1a-CH1l reviews routed to "CH1o". Hoist into CX2GUUser what June moved there (the pilot's CH1l pattern).

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). `X2GuEve.*` / `X2GUUser.*` are CP949: `bytepatch.py` only; `cl /P` each `#ifdef` you edit (the `SKILL_SLOT_10` / `UPGRADE_SKILL_SYSTEM_2013` pairs). Done gate: the build passes with `_LIVE` undefined, and a `cl /P` probe with it defined compiles the new path; no play-test.

## Amended by CH0 (2026-10-26)

- **Checklist rows** (`ch0_checklist_rows.tsv`): §7 L256 (vslot 510 skill lookup); §7 L259 (riding-key skill wait in vslot 510).
- **No class-map rows:** CH1o wires what CH1k / CH1l-CH1l4 transcribed. The 69 overrides are in `ch0_vtable_slots.tsv`. Seven have no name (0x56c, 0x584, 0x65c, 0x6a8, 0x6ac, 0x77c, 0x828); Eve overrides 25 slots Aisha does not (the tsv footer).
- A March Eve virtual June does not override goes away (June wins); check `X2GuEve.h` against the tsv.

## Amended by CH1h (2026-10-26)

- **Gap routed here:** June's `PlayEffectSet` (`321e1e60`) has a trailing float (default 1.0, instance `+0x14c`, reaches the set's damage effects' force-down slot) that March's signature lacks. `June_ENSI_DASH_COMBO_ZX_FrameMove` (`32986d30`) passes `m_fForceDownValueRateElectronBall` there and the member omits it. Decide whether to add the parameter (engine change in `X2EffectSet.*`) before `_LIVE`.
- **Decided by the CH1h review (2026-10-26): add it**, before `_LIVE`, under its own `SERV_IRUHADEV_JUNE_*` flag (an engine change, not Eve core): the trailing `float` on `PlayEffectSet` (default 1.0), stored in the instance (`+0x14c`) and handed to the set's damage effects as their force-down rate (read the reader in `321e1e60`'s callees first); then pass `m_fForceDownValueRateElectronBall` in `June_ENSI_DASH_COMBO_ZX_FrameMove` and drop its comment line. Other June callers follow with their own character ports.

## Amended by the CH1l5 review (2026-10-26)

- **Gap routed here:** June's base slot 514 `31fc9410` (`CX2GUUser`) clears `+0x8eac` and `+0x8ea8` together. `m_fJune8ea8` now lives in `CX2GUUser` (CH1l5), so hoist that clear into the base `ClearSkillUseReserve` (`X2GUUser.h:3362`, under `SERV_IRUHADEV_JUNE_USER_SKILL_USE_HELPERS`) before Eve's skill use goes live; otherwise a cast leaves its MP reserved ("You need more mana", Aisha play-test fix 1, 2026-10-21). Aisha's override (`X2GUArme_VioletMage.cpp:35321`) then becomes redundant; keep or retire it in the same change.
