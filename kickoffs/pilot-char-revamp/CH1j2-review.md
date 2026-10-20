# June character Aisha (Stage 5 pilot) - Phase CH1j2 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1j2 review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1j2 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` (the `InitState` gate: `SERV_IRUHADEV_JUNE_CHAR_ARME_LIVE`, undefined) |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1j2 - Done (2026-10-21, Sonnet 5.5)"
grep -a "Char CH1j2" june2017_re/funcmap.tsv        # the five PORTED rows
python june2017_re/char_arme/ch1j_review_regcheck.py | grep MISSING   # none of the five addresses may print
```

The code is the `//{{ ... Phase CH1j2` block at the end of `X2Lib/X2GUArme_VioletMage.cpp` (five `June_*` members), the five registrations in
`June_InitState`, and the declarations after the CH1j block in the header.

## Task

All five functions have no Ghidra function; read them with `python june2017_re/funcdis.py --func <va>` (it merges neighbours: stop at the first
`ret` / `jmp eax`; `32cbca20` also has a tail block at `32cbcff0`). In the order of risk:
1. **`32cbca20`** (HM / EM X2 EventProcess, 400 lines). Check against the listing: the five windows and their `m_fEventTime` triples
   (`+0x8c10 + 12 * i`), the state ids (0xab / 0xaa / 0xad / 0xa9 / 0xc6 = `AVSI_AEM_XZ`), the class test on windows 2 and 4 (`[[user+0x867c]+0xaf0]+4`
   == 0x68 = Elemental Master), the oneX / oneZ / pureDown / back-key conditions per window, the 12-key tail at 0.966 (tail block `32cbcff0`),
   the animation-end WAIT, and `+0x96f0` while flag 3 (`m_bCanChangeState`).
2. **`32cc42a0`** (AVP dash attack X2 EventProcess): the fall branch, the animation-end WAIT, then `SKILL_CANCEL_AFTER( +0x8c90 )`, walk `+0x8c98`,
   bwalk `+0x8c94`, dash `+0x8c9c`, Z / X `+0x8c88`; confirm the cancels are ground-only and that the order is the listing's.
3. **`32ce2cc0`** (AVP dash attack X2 FrameMove): diff against `32ce2b20` (the existing ADM dash-jump ZX member); FlushMp( 10 ), bone Dummy2_Lhand,
   rotation unturned, failure ( 0, 0, 0 ). JUNE_BUGS 380 records that the Void Princess fires the ADM effect.
4. **`32ce7170`** (AEM dash jump X3 FrameMove): 0.16 timer, 4 x rate, type 1 of `June_CreateComboFireBall`, rotation z -30, failure ( 0, 0, -30 ).
5. **`32cc3040`**: the EventTimer( 0.27 ) result is dropped; confirm nothing else is in the function.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1j2 commit>^..<CH1j2 commit>`. Paste its `files:` / `in this flag's files:` /
`RESULT:` lines. The 5 deleted lines are CH1j's own stand-in and four comment lines in `June_InitState`.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear errors under the same flag (then
`tools/build.py --deploy`); hand design decisions back to the user. Write "CH1j2 -- Review" into the plan doc under the Done note, update the CH1j2
row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1k.md` still matches, and commit with explicit pathspecs.
