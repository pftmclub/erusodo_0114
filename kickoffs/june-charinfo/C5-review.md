# June character info window - Phase C5 review (covers C1-C5)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-charinfo C5 review` |
| Plan doc | `JUNE_CHARINFO_PHASES.md` (branch `main`) |
| Depends on | C5 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_CHARINFO_DLG` / `_SHOW` / `_STATS` / `_VIEW` / `_ACTIONS` |
| Log tag | `[JUNE_CHARINFO]` |

## Read first

```sh
python tools/docsection.py JUNE_CHARINFO_PHASES.md "Context" "Scope fence" "Rules" "Starting context" "Review (Opus 5.5, high)"
```

and the C0-C5 Done notes (`python tools/docsection.py JUNE_CHARINFO_PHASES.md --list`, then pull each "Done" section), `june2017_re/charinfo_functions.tsv`.

## Task

Review C1-C5 against Ghidra and `charinfo_functions.tsv`: each ported body line by line (UICustomEventProc's id -> body map and the title-manager
pre-forward, SetShow's order, the option rows and their Lua coefficient calls, the socket-total group filter, the slot paths, the packet senders and
the emulator answers), the `#else` paths, the CIM / `UI_ACHIEVEMENT_SYSTEM_CUSTOM_MSG` values against June's `Enum.src.lua`, and the staged files
against June's bytes (`kom.py status`).

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit of that flag>^..<its last commit>` (a `..HEAD` range can time out).
Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.
Also `enumscan.py --check` on the touched files and `luabind.py check DLG_UI_CHARACTER_NEW_4`.

Retire C3's `_DEBUG` flag. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the
user. Write "Review - Done" into the plan doc, update STATUS.md, check `kickoffs/june-charinfo/Z.md`, and commit.
