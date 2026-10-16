# June dungeons - C3z-4t - June's tutorial guide manager (Battle Field tutorial dungeons)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-dungeons C3z-4t` |
| Plan doc | `JUNE_DUNGEONS_PHASES.md` (branch `main`); this phase's text and the C3z-4 Done notes still sit in `JUNE2017_EFFECT_COMPLETION_PHASES.md`, which the commands below read; write the new Done note into `JUNE_DUNGEONS_PHASES.md` |
| Depends on | june-effect C3z-4 batch 1 (done) |
| Flag | one `SERV_IRUHADEV_JUNE_TUTORIAL_GUIDE*` per logical change |
| Log tag | `JUNE_FX C3z-4` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3z-4 - Done (batch 1)"
```

## Task

Nine Battle Field tutorial dungeons reach `benders_actor.lua`, whose `BENDERS_ACTOR_DYING_END_FUNC` calls
`slot1:GetTutorialGuideManager()` (`322cc010`: `&g_pMain->m_TutorialGuideManager`, embedded at `+0x8c4`, `31d70b00`, never nil),
then `CheckToNeedTutorialGuide()` (`31fbde90`: quest 11000 (`0x2af8`) done -> false, else a per-unit check) and `OnBeginGuideNo(101)`
(`31fc5e20` -> `31fc5d70`), then creates `NUI_INVISIBLE_DUMMY`. March has no guide manager (June class list: `CX2TutorialGuideManager`
with 9 methods in `func_strings.tsv` `31d95080`). User 2026-10-10: this is its own row, not a stub.

Scope the June system first (the manager, its UI, who drives it, its data files) and report the size to the user before porting.

Also owed (june-effect C3p-2, 2026-10-15): June's `CX2State::HandleMsgByESC` `31f1e1a0 @31f1e1e8-31f1e212` asks the manager first: a guide
running (`31fbda90`, `[+0x14] != 0`) and `31fbdb20` (`[+0x66] != 1`) -> the ESC is eaten (return false). Port it with the manager, at the
insertion point the C3p-2 Done note names (`python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3p-2 - Done (2026-10-15, Opus 5.5 high; every gate mapped, nothing portable, no code, no build)"`).
While there (C3p-2 review): the comment in C3p's `SERV_IRUHADEV_JUNE_SKIP_START` block in `CX2State::HandleMsgByESC` lists "the chat box" among
June's earlier gates; the gate is GFx `closeToTopZorder` (`31c43bb0`). Fix the comment with the bytepatch that adds the guide gate.
