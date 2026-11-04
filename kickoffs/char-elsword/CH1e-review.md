# June character Elsword (Stage 7) - Phase CH1e review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1e review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1e done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1e - Done (2026-11-04)"
awk -F'\t' '$7=="CH1e"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 30 CH1e functions against June (`python june2017_re/charport_adis.py elsword --phase CH1e --out <dir>`; a listing can run past its function, stop at the `int3`). The branchy ones carry the risk:
- Lord Knight X X X `32cfafa0` / X X X X `32cfb420` (inside): flag 0 on `m_fEventTime[0]` to `ESSI_ELK_XXXX` / `ESSI_ELK_XXXXX`, direction cancel only in X X X, then UPDOWN / `SKILL_CANCEL_AFTER` / back-walk / walk / dash / Z / X cancels, then the end of animation (X X X X: then the ZXC skip keys after 1.3).
- The new EventProcess bodies (`_CORE`): dash Z X `32d01040` (end block first, cancels, flag 0 to dash Z X X), dash Z X X `32d014c0` (no UPDOWN), dash Z charge X `32d017f0` and dash X charge X `32d01f10` (X released or `m_fEventTime[0].keyInputStart` reached goes to the next state), dash jump Z Z Z `32d01b70` / landing `32d01ba0` (landing: JUMP_DOWN without the y offset), dash X X `32d02290`, dash X X X `32d02710`, X X X X X `32d02a90` (flag first, no tail), X X X X X X `32d02ed0` (= dash X X X).
- Sword Knight Auto Guard: `32d180a0` Start (`UpNowMp( 1.f )` first), `32cff880` EP, `32cff8d0` small EP, `32cff920` END EP (the `m_fCanNotAttackTime = 0.1` cancel chain; dash, back-walk, walk, skill order), `32d0c740` End.
- Written = March: Z3a Start / EP, dash jump X2 (4) and X3 EP, ZZZfrontZ (2), the three FrameMove thunks, the two dash-StartFuture thunks, X2 End.
- Which flag and `m_fEventTime` index each branch uses, every time constant (`0.23`, `0.15`, `0.9`, `0.68`, `0.867`, `1.3`, `0.1`), and the `JUNE_BUGS` row 521 (no `ESSI_ELK_XXXXXX` table).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h` (the CH1d helper block now sits under `_CORE`: that is the 2 deleted lines); `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (a Git Bash `/D` argument is path-converted: use `-D`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1e - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1f.md`, commit with explicit pathspecs.
