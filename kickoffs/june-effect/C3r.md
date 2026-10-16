# June effect completion - C3r - June's user CHANGE_WORLD_COLOR_DATA window (port)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3r` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3o (members, `RestoreJuneWorldColor`, the NPC side). Not with a phase editing `CX2GUUser::CommonStateStart`'s tail |
| Flag | `SERV_IRUHADEV_JUNE_WORLD_COLOR_WINDOW` (C3o's; the user side is the same logical change) |
| Log tag | `JUNE_FX C3r` |
| Review | none unless the user side differs from the NPC's |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3o - Done" "C3r - port: June's user CHANGE_WORLD_COLOR_DATA window"
grep -n "(C3o)" june2017_re/funcmap.tsv
```

## Task

C3o ported the NPC side of `CHANGE_WORLD_COLOR_DATA` (CX2GameUnit `+0x244 / +0x248`, `RestoreJuneWorldColor` = `32296be0`). The user
side: `CX2GUUser::CommonStateStart 32057b80 @32060286-320603b2` (the same parse; note `@32060290`'s extra store to `[ebx]` on a miss
before the table, find what `ebx` is), the reader `3204f190 @3204fb99-3204fbbc`, `31fd3260` (`@31fd3271`, a bool used at
`32063650 @3206cdb9`: true -> `32001460`, false -> `32296be0`, the restore). Map each to March's user functions, port them at June's
positions. Data: only Ain's class script (`Ain_Apostasia`, not in March) has the table, so it is dormant: regression play-test.
