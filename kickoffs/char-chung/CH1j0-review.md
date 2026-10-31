# June character Chung (Stage 7) - Phase CH1j0 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1j0 review` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1j0 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_USER_BONE_ROTATION`, `SERV_IRUHADEV_JUNE_USER_AIM_TARGET`, `SERV_IRUHADEV_JUNE_DIVERGE_ROTATE_BONE` (all defined) |
| Log tag | none (the map stays empty and the aim helper is unreferenced until Chung's `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1j0 - Done (2026-11-01)"
grep -a "Char Chung CH1j0" june2017_re/funcmap.tsv
```

## Task

CH1j0 ported six June `CX2GUUser` functions into `X2GUUser.cpp` / `.h`. Read each against Ghidra:
- **`32008660` -> `GetNearestTargetAimDegree`:** the skip tests and their order (IsValidUnit, team `+0x278`, HP gage `+0xc04` / `+0x20`,
  NPC `+0x264` -> `+0x85f8 +4`, vslot 0x180, `31fd5ef0( npc, 1, 1 )`; user -> `32293b80` vs vslot 0x170). The NaN behaviour of each x87
  compare (`test ah, 0x41` / `0x5` with `jz` / `jp` / `jnp`) against the C++ `<` the port wrote. The yaw fold `360 - y` (JUNE_BUGS 497) on
  both yaws, the 180 fold and the 0.5 factor (`charport_consts.py --fn 32008660`). The out vec3 = the angles, not the position.
- **`31fe9840` / `31fe4870` / `31fdb4e0`:** the value layout (bool, name, axis +0x20, degree +0x2c), the per-axis `FLT_EPSILON` tests on
  both axis and degree, the vector each `RotateBone` call builds, the call site in `OnFrameMove` (`@320657e4`, after the `m_bRotateBone`
  block) and the clear in `CommonStateStart` (`@32057e01`).
- **`31fd93e0` -> `RotateBone` (divergent):** `Dummy4_LWeapon01` / `Bip01_L_Finger1` (narrow strings, `m_pMotion->FindFrame`), the
  radians constant, the second-body stand-in; that the `#else` is March's body byte for byte.
- **`31fd5ef0` -> `GetTargetHitPos`:** same as `X2BuffTemplet.cpp`'s file-static; is the target-centre branch right for the NPC
  vslot 0x434?

Hygiene: `GIT_CONFIG_KEY_0=diff.algorithm GIT_CONFIG_VALUE_0=patience GIT_CONFIG_COUNT=1 python tools/flagcheck.py JUNE_USER_BONE_ROTATION --range <CH1j0 commit>^..<CH1j0 commit> --with JUNE_USER_AIM_TARGET JUNE_DIVERGE_ROTATE_BONE`,
`junediverge.py --check-all`, `enumscan.py --check X2Lib/X2GUUser.cpp X2Lib/X2GUUser.h`, the `X2GUUser.cpp` one-file probe.
Write "CH1j0 - Review" into the plan doc, update the STATUS row, check `kickoffs/char-chung/CH1j.md` "Amended by CH1j0", and commit
with explicit pathspecs.
