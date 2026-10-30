# One function per name - Phase D1 review (covers D1-5 and D1-5b; it closes D1)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence D1 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | D1-5 and D1-5b done |
| Flag | the `SERV_IRUHADEV_JUNE_DIVERGE_*` flags of D1-5 and D1-5b; it closes D1 |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Rules" "D1 split (2026-10-29, D1-1 session)"
```

and the Done notes of D1-5 and D1-5b (`docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md --list` names them), `june_march_divergent.tsv`.

## Task

Review D1-5 and D1-5b (they close D1): each replaced body against its June listing (Ghidra / `funcdis.py`), `#else` byte-intact, the live March branch (`cl /P`),
one unported call site per row, guards, `MODS.md` / `JUNE_BUGS.md` rows, stand-ins named and routed.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out). Paste the `files:` /
`in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding. Fix clear errors
under the same flag (then `tools/build.py --deploy`); hand design decisions to the user. No play-test (the smoke runs at the plan end).
Write the review section into the plan doc, update STATUS, check `D2.md`, commit.

Lead from the D1-2 review: March's `CX2Camera::ManualTrackingCamera` and `CalcNormalCameraPosition` apply the camera height
compensation for `GT_DUNGEON` / `GT_BATTLE_FIELD` only, as `NomalTrackingCamera` / `NomalDirectCamera` did; June's `31fb3640` / `31fb3c50`
apply it for game types 2-5 (fixed under `LUA_CAMERA`). Find their June counterparts (funcmap, callers of the two) and compare them whole.

Leads from D1-5 (2026-10-30): rows 360-363 got no code (the four popups already log under `JUNE_BUFF_BEHAVIOR_PARITY`'s `DISPLAY_ERROR` redirect); confirm
that inside a buff capture `RuntimeError` raises no popup either. `SET_MANAGER_OBJECT` makes every object own a matrix (June embeds it): check
an object that is managed, then unmanaged (`KTDGObject.cpp` deleter `SetManagerObject( NULL )`) keeps a sane matrix. `GetFixedDistancePositionInAllLine`'s
body past the flip was compared by branch shape only; compare it line by line.
