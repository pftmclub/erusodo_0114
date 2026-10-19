# June skill system core - Phase S7n - June's new-title notice (emblem 18)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S7n` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S7l (and its review) |
| Flag | one per logical change, `SERV_IRUHADEV_JUNE_SKILL_*` |
| Log tag | `[JUNE_SKILL] EMBLEM` (`python tools/logs.py --tag JUNE_SKILL`) |
| Review | `kickoffs/skill-system-core/S7n-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S7l - Done (2026-10-19, Opus 5.5 high)"
```
`june2017_re/funcmap.tsv` rows tagged `Skill S7l` (the title-notice rows say `S7n`).

## Task

Added by S7l (2026-10-19, unowned-branch rule). June shows newly earned titles through emblem 18 `EI_NEW_TITLE_INFO_NOTICE` (data036 `Emblem_Manager.lua` = June's,
`LUA_FILE_NAME = DLG_Get_New_Skill_or_Memo.lua`, the same dialog as emblem 17). March has the title system (`CX2TitleManager`) but not this notice. Port each June function whole:

1. **The feed `32701960`** (called from `CX2State::OnFrameMove` `31f0d390 @31f0daa1`): name its March counterpart (title manager / state), what it reads, and its five
   `3244adb0` pushes and four `PlayEmblem( 0x12, ... )` calls.
2. **`3244adb0`**: the manager's map `+0x78` ( key 0 / 1 -> a deque of title ids; ctor `3244bd00` ), March has no member yet.
3. **Case 0x12 of `3244c4b0`** with `3244c2b0` (rows 0 / 1, at most two per pop, ten tries), `32447b50` (hide `g_pStaticIcon1..2`), `32448420` (title icon:
   `326f8400( id )` + `.tga`), `3244bfb0` (the text), and the re-show rule (map key 1 then 0 non-empty -> `+6 = 1`, `+0x63 = 0`).

**Play-test:** earn a title offline (name one the emulator can grant) and see the notice pop with its icon and text; the log names the rows shown.
