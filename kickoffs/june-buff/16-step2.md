# June buff port - Phase 16 steps 2-4 - socket-option parity, then BBT 92

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-buff 16-step2` |
| Plan doc | `JUNE_BUFF_PORT_PHASES.md` (branch `main`) |
| Depends on | Phase 16 step 1 (done, read-only RE) |
| Flag | SERV_IRUHADEV_JUNE_SOCKET_PARITY (92 stays under NEW_STAT; designed attack range under SERV_IRUHADEV_JUNE_ATTACK_RANGE_DESIGN) |
| Log tag | `[JUNE_BUFF]` (`python tools/logs.py --tag JUNE_BUFF`) |
| Review | 16-review (write it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Starting context" "Play-test hand-off (Phase 5 onward)" "Phase 5c -- Done" "Phase 16 -- Socket-option parity"
```

The plan doc's "Starting context" predates `tools/`: wherever it (or the task below) says to
build, deploy, check size/mtime, run a fresh unifdef, run `file`/`git diff --stat` for hygiene,
or read logs by hand, use `tools/build.py --deploy`, `tools/flagcheck.py`, `tools/logs.py` and
`tools/bytepatch.py` instead (the x2-phase skill's "Tools" and "Finish" lists).
Never launch X2_offline.exe; ask the user.

## Task (verbatim from the plan doc)

Continue Phase 16 at **step 2**. Step 1 (RE) is done and recorded in "Phase 16 -- Step 1 RE"
(inside the "Phase 16" section above), including its addenda: do not redo it. Its
"Corrections to the text above" override the original scope where they disagree, and its
"Open decode items before code" must each be finished in step 3, not assumed.

Steps 2-4, as the original Phase 16 kickoff says: June's socket loader keys, the per-unit
socket aggregate, InitAdditionalStatOptionValue seeds and passive init, the refresh variants
and the option-data template 320839e0, June's SocketOptionForm.lua (staged for the user to
pack, after the arity gate), then BBT 92 end to end and 78's three socket terms, plus the
designed attack range (10% of attack, both physical and magical, users only; see Q3) under
SERV_IRUHADEV_JUNE_ATTACK_RANGE_DESIGN. Port June's whole function wherever it differs;
cite June addresses; never invent a value; byte-level CP949 edits; flag-off check per file;
count the reach on March's live data before building. Step 1's "For checklist section 7"
items go into JUNE2017_CHARACTER_PORT_CHECKLIST.md section 7 in this session. Build, deploy,
commit; write a Done note with per-item "how to see it in game" lines.

## Added by Phase 8 (2026-10-05): BFT_AURA's socket side

The user put the aura's socket half here. Phase 8 ported the `BFT_AURA` class, its check (slot 9)
and unit slot 182; `CX2GUUser::HasSocketAuraFor` (slot 181, June `3204e380`) stops at the SIEE 15
lookup and logs one `unsupported (Phase 16)` line. Port, with the socket loader:
- SIEE 12-20 (at least 15, `SIEE_ALWAYS_PLAY_EFFECT_SET`) and the socket-data fields `EFFECT_NAME`
  (`+0x1fc`), `m_fIntervalTime` (`+0x1e8`) and the `BUFF_FACTOR_ID` list (`+0x29c`, flag byte 9);
- slot 181's inner loop (socket ids under `map[15]` -> their list -> a factor of the asked BTI);
- the emitter: `ProcessSocketEffectEx` case 15 (`32029fff`), called from user slot 484 with 0xf.

Read `python tools/docsection.py JUNE_BUFF_PORT_PHASES.md "Phase 1c, item 4" "Phase 8 -- Done"`
(the BFT 14 block) first. Data: the 10 June `BFT_AURA` buffs in `OptionItemData.src.lua`.
