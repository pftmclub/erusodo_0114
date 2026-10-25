# June character Eve (Stage 7) - Phase CH1q - Nasod Core

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1q` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1p review (decision 1 answered 2026-10-25: (a), March's dormant code as the base) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE` (+ `..._NCORE_DEBUG`); it runs only with `_LIVE` |
| Log tag | `[JUNE_CHAR]` (`EVE NCORE`), seen from CH1 final |
| Review | `kickoffs/char-eve/CH1q-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Context" "Rules" "Phase CH0 - Eve map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core" "Open decisions (for the user; asked at the end of planning)"
```
Then CH0's Done note and `june2017_re/char_eve/ch0_nasod_core.tsv`.

## Task

Port the Nasod Core as decision 1 says: maneuver core, maneuver gauge, `ENSI_PARRYING_SMALL / _PARRYING / _PARRYING_BLINK`, `ENSI_BERSERK_MODE`, `DTT_EVE_MANEUVER_ATTACK`, the gauge-UI orb (`CX2EveMyGageUI`, `PGUWO_ORB_EVE`), the `X2GUUser` virtuals (`AddManeuverGauge`, `IsWaitManeuverCore`, `SetAttackManeuverCore`), the NPC side in `X2GUNPC.cpp`. Every function June has is June's whole body; what June removed goes; the parrying states register through CH1m's June registration.
- Decision (a), the user's answer: define `ADD_EVE_SYSTEM_2014` / `FIX_ADD_EVE_SYSTEM_2014` only inside `#ifdef SERV_IRUHADEV_JUNE_CHAR_EVE_NCORE` in `Always.h`; prove with `cl /P` what else the two macros switch on (other classes, `X2GUUser` layout, the gauge manager), then replace each body that differs from June under the NCORE flag.
- Over ~45 functions in the map: stop after the first half and write `CH1q2`.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). Done gate: the build passes; the Done note lists every `ch0_nasod_core.tsv` row with its outcome (June body / March = June / dropped). The play-test comes with CH1 final.

## Amended by CH0 (2026-10-26)

- **Functions: 49** (`awk -F'\t' '$6=="CH1q"' june2017_re/char_eve/ch0_class_map.tsv`; helper 28, override 9, state-fn 12).
  June addresses: `32968390` `329683a0` `329683b0` `329683d0` `32968430` `32968470` `32975610` `32975740` `329757d0` `32975860` `32975920` `32975970` `329759b0` `32975a10` `3297a370` `3297a640` `3297ee20` `32998930` `32998990` `32998a00` `32998a80` `32998b00` `32998b90` `32998bf0` `32998ca0` `32998d70` `32999200` `329ac350` `329ac4a0` `329acaa0` `329acea0` `329acf70` `329ad1d0` `329ad5e0` `329b4060` `329b41f0` `329b4210` `329b46a0` `329b5130` `329b5230` `329b8010` `329cd410` `329cd650` `329cf220` `329cf230` `329cf240` `329cf260` `329cf280` `329cf4b0`.
- **States: 3** (`awk -F'\t' '$8=="CH1q"' june2017_re/char_eve/ch0_state_skill.tsv`): `ENSI_PARRYING_SMALL`, `ENSI_PARRYING`, `ENSI_PARRYING_BLINK`.
- **Checklist rows** (`ch0_checklist_rows.tsv`): §7 L272 (spec A readers).
- **Callees outside the class no doc had named** (`ch0_unmapped_identified.tsv`): `321e3100` CX2EffectSet::PlayEffectSetWithCustomPos_LUA( const char*, CX2GameUnit*, D3DXVECTOR3 vPos, D3DXVECTOR3 vRot ) (X2EffectSet.cpp:5804) (confirmed).
- **The map:** `june2017_re/char_eve/ch0_nasod_core.tsv`, 126 rows (same 100, differs 13, june-only 9, absent-in-june 4). March sites: `ch0_nasod_core_sites.tsv` (77 sites in 9 files; every `ADD_EVE_SYSTEM_2014` body is dead today, proven with `cl /E`; recipe `june2017_re/char_eve/ch0_live_lines.py`).
- **What to transcribe: 19 June functions** (verdict differs / june-only): `3297ee20` AddManeuverGauge, `329b4060` DoParrying (Ghidra merges it into `329b41f0`), `32975740` PARRYING_BLINK Start (revamp ids 25200 / 25213), `329b5230` OnFrameMove_ManeuverCore, `329ac4a0` AttackManeuverCore, `329acea0` SetAttackManeuverCore, `32968470` GetHypetMpdeStateID, `329cf280` IsSkillSummonedMonster (+ NPC ids 1621, 1622, 2091), the sites in `32981130` / `329a4680` / `329c5bc0` (OPEN_MIND gauge floor), and the gauge UI `32e64790` ctor, `32e71990` OnFrameMove, `32e6a210` hyper-ball update, `32e5eca0` SetShow, `32e725e0` dtor (vtable `335336cc`, created by `3248e290` @3248e5ef). The 100 `same` rows keep March's ADD bodies. The class map lists 49 CH1q functions, incl. the `same` ones you do not rewrite, so **no split**.
- **June's form:** the FIX form throughout (the parry effect is a DamageEffect at `+0x9bcc`, collision-data bone targeting). **`ENSI_BERSERK_MODE` is not in June** (no string in the exe or the script): drop the FIX registration `X2GuEve.cpp:1538` and the enum `X2GuEve.h:138`. `GetHypetMpdeStateID` returns `USI_HYPER_MODE` (0x17), and all 7 FIX hyper-state sites call it.
- **Gauge rules June changed:** AddManeuverGauge, the AttackResultByType gain and the skill-MP / 3 gain are all scaled by `( +0x6c50 + 1.0 )`. DoParrying and the AFTER_ATTACK cost are skipped with chance `+0x6d04`, and DoParrying stamps `clock()` into `+0x9c44`. AttackManeuverCore adds MP `+0x9bfc * lv` and `+0x9c00 * lv` (members from `3297f290`, CH4a). The hyper ball moved into `CX2EveMyGageUI`. The March `_OPEN_TEST_` / hack-verify RenderName text is absent in June.
- **Turning ADD on also removes** the live early `return;` at `X2GuEve.cpp:26992` (`#ifndef ADD_EVE_SYSTEM_2014`). `GetHyperModeStateID` has no live base declaration (FIX alone does not compile without ADD).
- **Open (from the map):** the setters of user `+0x6c50` / `+0x6d04`; the `32894e40` call in AttackManeuverCore; `CX2AddMyGageUI::InitWakeOrb` = `32e5b650` or `32e5b8a0`; the PARRYING EventProcess 0.3 s SKILL_CANCEL tail was matched by shape only.
