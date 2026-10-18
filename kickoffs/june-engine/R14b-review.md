# June engine remainder - Phase R14b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine R14b review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase R14b done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_EQIP_ATTACH_ANIM` |
| Log tag | `[JUNE_ENGINE] EQATTACH` (`python tools/logs.py --grep "JUNE_ENGINE] EQATTACH"`) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Starting context" "Phase R14b - Attach-anim equipment init (June rework)" "Phase R14b -- Done"
```

Then the funcmap rows tagged `(R14b)`, checklist rows 7aa / 7ab (`JUNE2017_CHARACTER_PORT_CHECKLIST.md`) and the R14b bullet of backlog Stage 11
(`JUNE_PORT_BACKLOG_ORDER.md`).

## Task

Review Phase R14b's commit against June (Ghidra, read-only) at `3295dfa0`:

- The Add frame gate: OWNER unit type (+0x1b4 -> +0xaf0) 9 and +0xd5 = 2 / 0x19, `Weapon0<i+1>` on the unit skin anim (+0x1c0), skip when either is
  missing; no owner unit = normal path. The index remap stays on the TEMPLET unit type (= March).
- The jiggle: DITT 30 (0x1e) through `IsDivisionItem`, tension 1.5 (33471a1c), expand 0 normal / 4 hyper, `SetUnitXSkinAnim` first; no other jiggle
  on the normal or hyper anims (`3295b230` only feeds the routed minor-unit block).
- The hyper half's Add remap to `m_ModelNameHyper[0]` (+0x724).
- The tail: the Add `Weapon0<n>` lookup null-checks +0x1c0; `"TraceLandPos"` takes the unit skin's clone frame root (+0x324 = `m_pCloneFrameRoot`,
  `31d32620`) whatever `GetCloneFrame` found; the Rose branch (0xb, slots 1..4 -> frame 1) and the DITT 72 tail are comments only.
- Routed material is named: checklist 7aa (Lu/Ciel minor-unit pair +0x584 / +0x5e4) and 7ab (Rose frames), Stage 11 (DITT 30 data, DITT 72).
- R14's texture chain and R12's AddModelXSkinMesh calls in this function are untouched.
- `flagcheck --range` over the phase's commit: every changed line inside the flag, `#else` = March.
