# June character Chung (Stage 7) - Phase CH1n3 - June's PhysicProcess_Jumping (jump-up lines), every class

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-chung CH1n3` |
| Plan doc | `JUNE_CHARACTER_CHUNG_PHASES.md` (branch `main`) |
| Depends on | CH1n2 review. One at a time: `X2GUUser.*` (CH1o touches it too) |
| Flag | a new `SERV_IRUHADEV_JUNE_USER_JUMP_UP_LINE` (every class) in `KTDXLIB/Always.h` |
| Log tag | none new |
| Review | `kickoffs/char-chung/CH1n3-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_CHUNG_PHASES.md "Rules" "CH1n2 - Done (2026-11-02)"
grep -a "PhysicProcess_Jumping" tools/data/june_march_divergent.tsv tools/data/june_diverge_allow.tsv
```

## Task

Split from CH1n2: June's `CX2GUUser::PhysicProcess_Jumping` (`31fd6ce0`) writes `+0x8fc1`, which CH1n2 ported as `m_bOnJumpUpLine` and which June's
PEPPER_RUN_JUMP_UP StateStartFuture / FrameMoveFuture read; March's body (`X2GUUser.cpp`, `PhysicProcess_Jumping`) differs. Port June's whole body inside
March's function, `#ifdef` June `#else` March (`python june2017_re/charport_adis.py chung 31fd6ce0`, about 450 instructions; the Ghidra decompile merges
the two line-type branches, so read the listing):
- Line type 4 (`LT_JUMP_UP_REL`) and 5 (`LT_JUMP_UP_ABS`) set `m_bOnJumpUpLine`; any other foot line clears it and resets `+0x88f0` (= -1) / `+0x88f4` (= 0).
- Identify: the five state tests (the protected value at `+0x7cbc` against the state vector `+0x7cec` / 100, vslots 0x118 / 0x494 / 0x490 / 0x458
  against `3208ec80`) by March's `USI_*` names; `CanDown` in June's signature (`31c6c310`); the game-state test `[0x3377585c] +0x8c == 0xb` (by its
  `CX2Main` state name); LineData `+0xe0` (direction 1 / 2), `+0xe4` (key-stop time -> `+0xde8`), `+0x84..0x8c` (jump speed -> `m_vAddJumpSpeed`
  `+0x88e0`), `+0x88ec` (`m_bSteppedJumpLinemap`); the repeat-touch guard (`+0x88f0` = line id, `+0x88f4` = `timeGetTime`, `0.001` x elapsed against
  `+0xe4`). New members get class-pattern names (no `June`, no address); a June quirk is kept with a `JUNE_BUGS.md` row.
- Remove the `PhysicProcess_Jumping` row from `tools/data/june_diverge_allow.tsv` and mark the `june_march_divergent.tsv` row ported in the same commit.

CP949: `bytepatch.py` only; `cl /P` each `#ifdef` (`ADD_LINEMAP_JUMPUP_DIRECTION` is on). One-line comments with the June address. No hardcoded enum
literals, no `June` in names (CLAUDE.md). Done gate: build passes, flag on; `flagcheck`, `junediverge --check`, `enumscan --check` clean; a play-test
hand-off on a map with a jump pad (Aisha or Eve): step on the pad, hold down on it, step on it twice quickly, pepper run onto it.
