# June effect completion - C9d - identify June's per-emotion effect sets (`32355a50`) (Opus, no code)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C9d` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C9c-1 review (done 2026-10-07; it found this branch unowned) |
| Flag | none (no code) |
| Log tag | `[JUNE_FX]` |
| Review | none |

User decision (2026-10-07): give this branch its own phase and follow June ("sure, always follow june's").

## Read first (and nothing else from the plan docs unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9c-1 - Review" "C9b - Done"
```

Also read: `june2017_re/funcmap.tsv` rows `32043380`, `31fcf490`, `31fcf4d0`, `3203a620`, `3206eaf0`.

## What is known

- `32355a50( out wstring, EMOTION_TYPE )` maps June emotion ids to effect-set names: `0x1c..0x20` `EffectSet_{Keroro,Kururu,Giroro,
  Dororo,Tamama}_Haul`, `0x23` `EffectSet_Emotion_GirlFriend_Hea...`, `0x26` `..._Twice`, `0x27`/`0x28` `..._2015XMas_Box_S/E...`,
  `0x2f`/`0x30` `..._2016Maid_Man/girl`. Any other id returns "".
- Callers: `CX2GUUser::PlayEmotion` `32043380` @`32043a3c`, `323fef90` @`323ff8c5`, `31fa52b0` @`31fa599b`. C9b called `31fa52b0`
  one of June's own viewer effects and left it out of scope.
- In `PlayEmotion` (@`32043a41-32043b62`), when the name is non-empty, June stops `+0x9434`, plays the set on the UI effect set
  (`g_pData +0x9c`) with the unit as owner, and keeps the handle at `+0x9434`. Read the full argument list of that `321e1e60` call.
- `CX2GUUser::OnFrameMove` `3206eaf0` @`3206eb64-3206ed70` is a June-only emotion block. It switches on `m_ePlayedEmotion`
  (`+0x89d8`, jump table `32070188`/`32070198`), stops `+0x9434`, and drains the handle vectors `+0x93dc` / `+0x93ec`. It also
  compares `31fac950(...)` against `L""` and calls `32375250( m_ePlayedEmotion )` on the unit manager. `~CX2GUUser` `3203a620`
  stops `+0x9434` (@`3203af9f`) and both vectors (before `@3203af89`).
- March's `CX2Unit::EMOTION_TYPE` (`X2Lib/X2Unit.h:251`) ends around `ET_EMOTION_AVATAR7`. None of June's `0x1c+` ids exist in
  March, so the branch is inert until those emotions arrive.

## Task

Identify, in Ghidra, and write funcmap rows (`Effect C9d`):
1. June's `EMOTION_TYPE` enum from `0x10` to `0x30`. Where do those emotions come from: `EmotionData.lua` (`31ee6100` opens it
   after `UnitViewerEffectSetTable.lua`), `32375250`, items, the emotion UI? Is there a March counterpart for each part?
2. `323fef90` and `31fa52b0`: what they are, and whether either is reachable in March.
3. The `OnFrameMove` block: every case of the jump table, what fills `+0x93dc` / `+0x93ec`, and what `31fac950` and `32375250` return.
4. Whether all 11 effect sets are in June's `UIEffectSet.src.lua` (`june_src_fixed/`) and missing from March's.

End with a port list for a Sonnet phase (C9e: the `32355a50` mapping, the `PlayEmotion` branch and its `+0x9434` lifecycle, the
`OnFrameMove` block, the dtor stops, and staging the templets), following June verbatim.
- If the June emotion ids themselves are missing in March (enum, data, items, UI), that is a missing system.
- Name it, and say whether it belongs to the June UI port (`JUNE_PORT_BACKLOG_ORDER.md`) or to C9e.
- A missing system is the one thing that goes back to the user. Do not write game code.
