# One function per name - Phase D1-4 review (covers D1-3, D1-3b, D1-4, D1-4b and D1-2c)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence D1-4 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | D1-3, D1-3b, D1-4, D1-4b and D1-2c done |
| Flag | the `SERV_IRUHADEV_JUNE_DIVERGE_*` flags of D1-3, D1-3b, D1-4, D1-4b and D1-2c |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Rules" "D1 split (2026-10-29, D1-1 session)"
```

and the Done notes of D1-3, D1-3b, D1-4, D1-4b and D1-2c (`docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md --list` names them), `june_march_divergent.tsv`.

## Task

Review D1-3, D1-3b, D1-4, D1-4b and D1-2c: each replaced body against its June listing (Ghidra / `funcdis.py`), `#else` byte-intact, the live March branch (`cl /P`),
one unported call site per row, guards, `MODS.md` / `JUNE_BUGS.md` rows, stand-ins named and routed.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out). Paste the `files:` /
`in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding. Fix clear errors
under the same flag (then `tools/build.py --deploy`); hand design decisions to the user. No play-test (the smoke runs at the plan end).
Write the review section into the plan doc, update STATUS, check `D1-5.md`, commit.

Lead from D1-4b: `32696fa0` is the custom-event manager (funcmap rows 1132, 4330; its `+0x36c` set is `CustomEventScript.lua`'s
`MISSION_QUEST_ID`), so D1-4's helper name `IsFriendshipQuest` for its `+0x510` map (`3267b440`) is unconfirmed. Find the `+0x510`
filler (`32681130` / `32681280`, called from `32694ba0`, a vtable slot at `334b21bc`) and rename the helper if it is not the friendship list.

Leads from D1-3: the Bow pair (`InitEnchantParticleForBow` / `AppendEnchantParticleForBowToDeviceList`) was not compared with June
(rows 261 / 262 named only the Sword pair); `CX2OfflineInventory`'s server-side item count still mirrors March's `GetNumItemByTID`
exclusions (shared bank skipped), which is the emulator's server view, not the client's; confirm it should stay.
