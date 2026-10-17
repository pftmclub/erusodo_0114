# June engine remainder - Phase U1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine U1 review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase U1 done (its Done note and commits) |
| Flags | `SERV_IRUHADEV_JUNE_UI_FRAMEWORK`, `_JUNE_UI_NATION_KEYS`, `_JUNE_UI_MOVIE_SUBTITLE`, `_JUNE_UI_LINE_BREAK` (U0 split; spec per item in `U1.md`), plus the short-lived `_JUNE_UI_DEBUG` (the `[JUNE_ENGINE] UI` lines) |
| Log tag | `[JUNE_ENGINE] UI` |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Starting context" "Investigation ledger (measured 2026-10-16)" "Phase U0 - UI framework RE" "Phase U1 - UI framework port" "Phase U1 -- Done"
```

## Task

Review Phase U1's commit(s) against the RE map it follows (R0, U0 or W0) and the June addresses it cites: enum values and table
order, every setter's source value, key names and defaults, the store members, every read site, and that the `#else` path is
March's code.

Hygiene is one command: `python tools/flagcheck.py <the phase's flag> --range <first phase commit>^..HEAD`. Paste its `files:` /
`RESULT:` lines into the review; explain every OTHER row and deleted line; a FAIL is a finding.

Spot-check 2-3 items in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same
flag (then `tools/build.py --deploy`); hand design decisions to the user. Write "Phase U1 -- Review" into the plan doc, update the
STATUS row, check the next phase's kickoff, and commit.

## What U1 changed against U0 (check these first)

- Four U0 rows were wrong and are corrected in funcmap (`Engine U1`): `SetCaretOffSet` moves the caret rectangle's top and bottom (not x) by
  `ftol( float( ( caretY / GetLineCount ) * GetLineCount ) * offset )`; vslot `0x60` in `31cd2af0` is `SetEnable`, not a check call (ported as
  `CKTDGUIDialog::SetEnableRadioButtonByGroupOffset`); `31d43ef0` is March's `GetStrByLineBreakColorInX2Main`, `31d42bc0` is March's WithEllipse and
  `31cbff90` (via `31d471f0`) is `GetStrByLineBreakInX2Main`; the re-wrap has seven callers, not five.
- Spot-check candidates in Ghidra: `31cbff90` against `KTDXWordWrap.cpp` `GetStrByLineBreak` (the `+3` / `+8` colour skip at the cut point),
  `31c9a310` against `CMovieData::GetSubtitle` (the three-way compare), `31cc1660` hover loop against `KTDGUIComboBox::HandleMouse`.
- X2Lib's two wrap functions now forward to the engine (every X2Lib caller wraps June's way): look at the `#else` bodies in `WordWrap.cpp`, they are March's, untouched.
- The done gate "a June DLG from each group opens in game" has no live exerciser for group A (kom_contents grep: no DLG calls the new methods); the
  census (`GAP LIST june-only-unbound: 0`) and the strings in the exe are the evidence. Decide whether that is enough or whether a June DLG
  (`DLG_Mail_Shop_Popup_Send`, `DLG_Magical_Closet_ComboBox`) should be adopted for a test.
