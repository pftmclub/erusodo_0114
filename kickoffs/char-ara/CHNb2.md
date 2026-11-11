# June character Ara (Stage 7) - Phase CHNb2 - the class-change guides filled in for my class

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CHNb2` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CHNb (commit `db3bf791` + its follow-up) |
| Flag | `SERV_IRUHADEV_JUNE_CLASS_CHANGE_GUIDE` (class-generic, CHNb's; reuse it, one logical change: June's guides) |
| Log tag | `[JUNE_CHAR]` (`ARA CLASS`; a `_CHAR_ARA_CLASS_DEBUG` line may come back for this phase, the review retires it) |
| Review | `kickoffs/char-ara/CHNb-review.md` (covers CHNb + CHNb2) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "Rules" "CHNb - Done (2026-11-11)"
grep -n "32713c10\|32713420\|32712e50\|327141e0\|32710710\|32711490" june2017_re/funcmap.tsv
```

## Task

The user's CHNb play-test (2026-11-11): the 1ST guide's three class descriptions run into each other, and the 2ND guide opens with its script placeholders
("Psychic Tracer", empty portrait, "Primary Skill", empty "Primary Command"). June's `CX2LVUpEventMgr::LVUpEvent::CheckLevel` (`327141e0`) calls
`32713c10( event, dialog, ... )` on every dialog it opens; March's `CheckLevel` (`X2LVUpEventMgr.cpp`) only runs the transcendence branch (`32711490` /
`32710710`). Port `32713c10` whole into the event's open path (June's call order in `327141e0`), plus what it needs:
- **Event 2 (1ST guide):** `g_pStatic_String_ClassDescription`'s strings 0-3 re-wrapped (`31cbcc90( i )`, `31d471f0` wrap width, `31aa92b0` set).
- **Event 4 (2ND guide):** my line = `31ef05e0( class )`, next class = `3234b390( type, 2, line )` (= `GetUnitClassByTierAndLine`), its templet `32373770`; picture of
  my line shown (`31cbcd10( line )`), the accept / buy buttons' custom message rebound to 1541 / 1545 + line - 1 (`+0x27c`), the class name from the templet,
  then the skill / command lists (`3214f050`, `32714e80`, `3220bdb0`, `32713420`, `32714ec0`; funcmap `32712e50` "MajorCommandBack_%d" may be the same builder).
  Read every callee you transcribe; name controls by the strings `31d41c90` is given.
- **Event 6:** the control call (vslot 0x68) June makes on the transcendence guide; check it against March's `InitTranscendGuide`.
- Then cases 1551 / 1552 (`g_pStatic_ClassSkillDescription`, routed by CHNb) if `32713420` turns out to be their sender; otherwise leave them routed.
June names, no `June` in identifiers, the divergence gate, phase tag `// char-ara CHNb2`. Full build and deploy; play-test both guides again with fresh
level-14 Martial Artist / level-34 Little Specter units (`createchar.py`, `elsdb.py setchar --exp` 100 below the level), and a level-34 Little Hsien / Little Devil
for the line test (their 2ND guide must name Sakra Devanam / Yama Raja and its buy button open 264390 / 264394).
