# June character Raven (Stage 7) - Phase CH1o review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-raven CH1o review` |
| Plan doc | `JUNE_CHARACTER_RAVEN_PHASES.md` (branch `main`) |
| Depends on | CH1o done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE` (`//#define` until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RAVEN_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "Rules" "CH1o - Done (2026-11-06)"
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "CH1o - Review (2026-11-04)"   # Elsword's review, the worked example
```
Then the funcmap rows tagged `Char Raven CH1o` (`grep -n "Char Raven CH1o" june2017_re/funcmap.tsv`).

## Task

Check CH1o against June (read-only in Ghidra; bytes with capstone over `unpacked_x2.exe`, `charport_adis.py raven <va>`). In order of risk:
- **Vslot 0x300 = `AdjustDamageDataBeforeDamageReact`.** Redo the whole-`.text` sweep for vtable loads of `+0x300` (`dispsweep.py 300`, keep
  `mov r,[r]` + `mov r,[r+0x300]` + `call r`): the three `CX2DamageManager` sites UnitToUnit @3262546d / MeshToUnit @32628652 / EffectToUnit @3262f1e6
  and the NPC forward @32779046, nothing else. At each site: attacker register, the DamageData pushed, the hit-only path (the miss flag), and the
  defender's `DamageReact` (vslot 0x60) right after (EffectToUnit: attacker vslot 0x3dc between them). March's three `ADD_RENA_SYSTEM` blocks are the same
  spots; the `_LIVE` inserts sit after them and before `DamageReact`, and the `cl /P` count with `_LIVE` is 3 calls + 1 base declaration.
  Confirm `CX2GameUnit` / `CX2GUUser` vtables hold `3208ddc0` at +0x300, `CX2GUNPC` `32778fe0` (owner forward through `CX2AllyNPCAI`), and that
  Raven's `32c63660` jumps to `32c5d540`. Judge the user-only test: exact for Raven (no summons), the NPC forward routed to char-luciel CH1k.
- **`SetSkillLevelStateData`** against `3201f850`: null templet / unit return, `GetSkillLevel( id, 0, 0, 1 )` twice (= `max( 1, .. )`), `m_SPLevel`
  (byte store `+4`), `GetSkillPowerRateValue`, id at `+0xc`, `x ( 3201ad10( id ) + 1.0 )`; Raven does not override vslot 0x7a8 (`ch0_vtable_all.tsv`).
- **The nine triage rows:** `32c28100` (memo decide, always false), `32c69d80` (shared memo dtor), `32c69f40` / `32c69f50` (adjustor thunks to
  `32c6a620`), `32c3d1c0` / `32c28880` (= March's `AppendEffectToDeviceList` / `AppendMajorParticleToDeviceList`; March's minor list empty),
  `32c35fc0` (tail of `32c35f90`; `SKILL_CANCEL_AFTER_FLY` gives `CommonEventProcess` as the `else if` body, = CH1g2's port), and the two CH4a delegates
  `32c3c7a0` / `32c4e5b0` (CH4a kickoff amendment matches the bytes).
- **0x4ac and "510":** Raven's vtable keeps the base at 0x4ac; "510" was CH1k's step number (lookup `32c58c30`, ported).

Hygiene: `flagcheck JUNE_CHAR_RAVEN_LIVE --with JUNE_CHAR_RAVEN_CORE --range <CH1o commit>^..<CH1o commit> --files X2Lib/X2GURaven.cpp X2Lib/X2GURaven.h
X2Lib/X2GUUser.h X2Lib/X2DamageManager.cpp` (numstat 16 / 2, 1 / 1, 5 / 0, 21 / 0; the 3 deleted lines are the rename inside `_CORE`),
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GURaven.cpp X2Lib/X2DamageManager.cpp`, one-file probes of `X2GURaven.cpp`,
`X2DamageManager.cpp`, `X2GUUser.cpp` with and without `-DSERV_IRUHADEV_JUNE_CHAR_RAVEN_LIVE`. No build is needed if the review changes no code.
Write "CH1o - Review" into the plan doc, update the STATUS rows (CH1p becomes ready), check `kickoffs/char-raven/CH1p.md` and commit with explicit pathspecs.
