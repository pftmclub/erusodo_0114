# June character Elsword (Stage 7) - Phase CH1f review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-elsword CH1f review` |
| Plan doc | `JUNE_CHARACTER_ELSWORD_PHASES.md` (branch `main`) |
| Depends on | CH1f done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (commented out until CH1 final) and `_CORE` (defined by CH1d) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_ELSWORD_PHASES.md "Rules" "CH1f - Done (2026-11-04)"
awk -F'\t' '$7=="CH1f"' june2017_re/char_elsword/ch0_class_map.tsv
```

## Task

Review the 36 CH1f functions against June (`python june2017_re/charport_adis.py elsword --phase CH1f --out <dir>`; a listing can run past its function, stop at the `int3`). The branchy and the FrameMove ones carry the risk:
- Rune Slayer X Z EP `32cfb970` (inside): foot-first fall, flag 0 / `m_fEventTime[0]` to `ESSI_ERS_XZ_CHARGE` (Z key), UPDOWN / skill / back-walk / walk / dash / Z / X cancels. Z3a EP `32cf8a00` (no state change on the fall, walk cancel on a line, ZXC skip after 1.2 to WAIT). Dash jump X Z Z EP `32cfbee0` (flag macro with the animation-end clause; window `t < m_fAttackCancelAfter`).
- The new EventProcess bodies (`_CORE`): the two charge states `32d03d00` / `32d047c0` (the `stateChange` / `keyInputStart` / `keyInputEnd` branch, no y offset), X Z `32d03880`, X Z Z `32d040c0`, full-charge X Z Z `32d04440` / `32d04b80`, Z Z Z `32d04f00` (no UPDOWN, no Z / X), Z Z Z Z `32d05250`, Magic Knight dash jump `32d03250` / `32d035d0` (awakening half only, walk before back-walk, `m_fAttackCancelAfterEx` oneZ / oneX).
- The FrameMoves: new `32d19040` `32d19300` `32d195c0` `32d19880` (cost-0 `IsEnoughMPForAttack` at `Dummy2_Lhand`, the +0x960e flag set / cleared, timers 0.3 / 0.133 / 0.133 / 0.01 and 0.38, effect-set names, whether the Rune Slayer full charge really leaves the flag set when the gate fails); changed inside `32d128b0` (second push into +0x3ec0), `32d131d0` (vigor cost x 0.5, `337aef00` filled at `333ac8c0`), `32d0f640` (templet class); written = March: `32d12550` `32d12c50` `32d0fc00`.
- Constants and state ids: every time constant, the 6 new `ESSI` members (250 / 252 / 253 / 254 / 256 / 257), `_CORE needs BUFF_AWAKE_LOCK`.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_ELSWORD_CORE --with JUNE_CHAR_ELSWORD_LIVE --worktree --files X2Lib/X2GUElsword_SwordMan.cpp X2Lib/X2GUElsword_SwordMan.h KTDXLIB/Always.h`; `python tools/junediverge.py --check-all`; one-file `cl /Zs` probe with `-DSERV_IRUHADEV_JUNE_CHAR_ELSWORD_LIVE` (a Git Bash `/D` argument is path-converted: use `-D`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1f - Review" into the plan doc, update the STATUS row, check `kickoffs/char-elsword/CH1g.md`, commit with explicit pathspecs.
