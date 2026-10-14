# June effect completion - C3t-1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3t-1 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3t-1 done (its Done note and commit) and its play-test read ('check logs') |
| Flag | `SERV_IRUHADEV_JUNE_CUTIN_SHOW_OPTION` (+ `_DEBUG`, turn it OFF here) |
| Log tag | `JUNE_FX C3t-1` (`python tools/logs.py --grep "JUNE_FX C3t-1"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3t-1 - identify, then port: June's GetShowActiveSkillShow"
grep -n "(C3t-1)" june2017_re/funcmap.tsv
```

## Task

Review C3t-1's commit against June `31fd3180`, `31fcef00`, `324214a0 @32425865` and `32ede000 @32ede09d / @32ede0d1`. What C3t-1 could get wrong:
- **The option mapping.** That June's `g_pMain +0x274` is the loader's `+0xf4` (`this` = `g_pMain + 0x180` @3259affa) and a bitmask
  (bit 0 = `mySkillCutScene`, bit 1 = `teamSkillCutScene`), so `SCSS_ONESELF -> 1`, `SCSS_PARTY_MEMBER -> 3`, `SCSS_HIDE -> 0` is the
  by-name map; and that the three cases' tests (`cmp al, 1` / `test al, al` / `jz`) are transcribed with the right polarity.
- **The hyper test.** `31fcef00`'s null checks, the skill id from `GetNowStateSkillID` (vslot `0x524`), March's `GetSkillTemplet( id )` against
  June's `( id, 1 )` (the `IsEnableCode( 0x911 / 0x910 )` gate), and the category: `32070510` returns 5 only for type 13 = `ST_HYPER_ACTIVE_SKILL`.
- **The two hide terms left out.** `[g_pKTDXApp+0x1c8]` + `31c44710( 0xb )` (GFx), and `g_pMain +0x80a` (June's movie table at `+0x7f8`):
  confirm March has neither system, so leaving them out is reach-equal; check the backlog note.
- **The `#else`.** March's body is intact; the flag-off build is the base.

Read the play-test lines first: each `JUNE_FX C3t-1` line gives option, June value, now skill, hyper and the result. Turn
`SERV_IRUHADEV_JUNE_CUTIN_SHOW_OPTION_DEBUG` OFF (comment the `#define` with a dated reason), rebuild and deploy.

Hygiene is one command, not a fresh script:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_CUTIN_SHOW_OPTION --range <C3t-1's first commit>^..HEAD --with SERV_IRUHADEV_JUNE_CUTIN_SHOW_OPTION_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag
(then `tools/build.py --deploy`); hand design decisions back to the user. Write "C3t-1 - Review" into the plan doc, update the STATUS.md row,
check the next phase's kickoff (`C3t-2.md`), and commit.
