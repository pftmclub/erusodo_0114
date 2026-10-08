# June effect completion - C9h-3a review - the in-game riding pet's UI effect sets (engine holder + riding pet side)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9h-3a review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-3a done (its Done note), play-test logs read |
| Flag | `SERV_IRUHADEV_JUNE_RIDINGPET_UI_FX` (+ `_DEBUG`, off) |
| Log tag | `JUNE_FX RIDINGPET_FX` (`python tools/logs.py --grep "JUNE_FX RIDINGPET_FX"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-3a - Done" "C9h-3 - Done"
grep -n "Effect C9h-3a" june2017_re/funcmap.tsv
grep -n "^| 10[78] |\|^| 11[01] |\|^| 118 |" JUNE_BUGS.md
```

## Task

Review the C9h-3a code against June in Ghidra (read-only) and fix transcription errors under the same flag. The things it could get wrong:
- **The game / village split (the Done note's correction of C9h-3).** Re-verify in the disassembly of `32d2f560` that the in-game user block ends in `JNZ @32d350aa` / `JMP @32d350db`
  (both to `@32d35f00`) and that the only jumps into `@32d35a05` are the village block's (`@32d350e8`, `@32d35105`, `@32d35154`) plus its fall-through. If a path from the user
  block into `@32d35a05` exists, the `else` in `CX2RidingPet::StateChange` is wrong and JUNE_BUGS 108 must be restored to the double play.
- **The loader `32d2b0d0`:** key spellings, the tuple order (name, index, playCond, deleteCond), bases 1, 5 .. 97, the break rule, `insert` keeping the first of a repeated index
  (June's `32899920` is a map insert, not `operator[]`), the play after both tables are closed, handles pushed into `+0x4b0` even when -1; called after InitState and before the motion open.
- **The tail `32d2bad0` and the queued stops (JUNE_BUGS 111).** `RESPC_NOT_PLAY == 1` skips only when `32d292b0` finds a live instance by templet name in `+0x4b0`; the stops
  collected by `@32d31013` run after the tail. Check that `321d97c0` (June's GetInstance) really has no queue test, and `321dafb0` only pushes.
- **The engine holder:** `321e2790`'s writes (power rate -1, life time -1, mode 3, hyper 0, no scale), and the five consumers: `GetEffectPosition @321dcf6b` (bone only when the pet's skin
  anim is LOADED, `320719f0`; no offset on any of its three exits), `GetCombineMatrix @321dc81f` (raw `32071a10`), `GetEffectPositionByBoneName @321dd7ac` (EffectData bone, not the
  caller's), `GetUnitRotation @321ddf69` (`+0x324`), the create gate `@321eeb1e` (a holder without a skin anim falls through to the generic test).
- **Member mapping:** pet `+0x33c` = `m_ConditionData.m_vLandPosition`, `+0x324` = `m_vDirDegree`, `+0x390` = `m_SyncData.m_vPos` (struct order bool, float, float, dirDegree, dir, landPosition
  matches `+0x324 / +0x330 / +0x33c`).
- **Visibility:** `OnFrameRender_Prepare`'s village flag `+0x518` = `GetPlanRender` (identified by behaviour: the no-render return); the mirror sync; `NotifyShowObjectChanged`;
  `RemoveRidingPet` (`32d293e0` after the erase); `32d291a0` zeroing the mirror (JUNE_BUGS 110); the dtor not stopping (JUNE_BUGS 109).
- **Data:** `Enum.lua` carries `RESERVED_EFFECT_SET_PLAY_COND` and `RESERVED_UI_EFFECT_SET_DELETE_COND` verbatim from `june_src_fixed/data036_kom/Enum.src.lua:17597-17604`; the packed
  `data036.kom` differs from the pre-copy backup only in `Enum.lua`.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py JUNE_RIDINGPET_UI_FX --range <first C9h-3a commit>^..HEAD`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every
OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand
design decisions back to the user. Write "C9h-3a - Review" into the plan doc, update the
phase's STATUS.md row, check the next kickoff (`C9h-4b.md` is blocked on C9h-4a's review only), and commit.
