# June character Eve (Stage 7) - Phase CH1l40 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1l40 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l40 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (no class pushes the new tags yet; nothing in game) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1l40 - Done (2026-10-26)"
grep -a "Char Eve CH1l40" june2017_re/funcmap.tsv                        # 9 rows
```

The code is two insertions: `CX2EffectSet::ApplyDamageEffectModifiers` cases 20 / 5 / 12 / 16 (`X2Lib/X2EffectSet.cpp`), and two `friend` lines at the end of `CEffect`'s `SERV_IRUHADEV_JUNE_SPAWN_INHERIT` block (`X2Lib/X2DamageEffect.h`).

## Task

Review against June (read-only Ghidra; delegate the dumps):
- **`321db1b0` cases 0x14 / 5 / 0xc / 0x10** (jump table `321dbc60`): `+0xad4` set before the fall-through; add when `b == 1` else multiply; the NEXT tables take the result (JUNE_BUGS 437); the `fcomp` / `test ah,5` / `jp` reads as `0 < v`; `b44` / `b47` take the entry byte. The March members: `m_fCriticalRate` (DamageData `+0x510`), `m_bIgnoreStateDefence` (`+0x541`), `m_bIgnoreDefence` / `_OnPvp` (`+0x528` / `+0x529`), `m_bInheritCriticalRate` (CEffect `+0xad4`).
- **The four names.** `31f8ae10` = `CXMeshInstance::SetManualDirDegree` (ECX = the main effect in `329c3420`), `32371c00` = `CEffect::SetAttackTime` (ctor `@31f7d5ed` / `@31f7d720`), `31f8adb0` = `CXMeshInstance::GetScale` (`31c1a440` writes the same `+0x510`). Confirm the register origins at the call sites.
- **The customize struct** (`321cdbb0` / `321cd590` / `3264e800`): only `3297b840` uses it, so it goes to Stage 12 with Aisha's precedent. Agree or say what CH2a2 needs instead.
- **Tag 15 routed to CH3a0** (Eve's Spit Fire / Grenade push it): is CH3a0 the right owner, and is its amendment complete?
- Kickoff amendments: CH1l4, CH2a2, CH3a0.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1l40 commit>^..<CH1l40 commit>`; paste its `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear errors under the same flag, then `tools/build.py --deploy`. Write "CH1l40 - Review" into the plan doc, update the STATUS row, check `CH1l4.md`, and commit. Next: `/x2-phase char-eve CH1l4` (Sonnet 5.5 high).
