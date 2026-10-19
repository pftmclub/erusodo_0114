# June character Aisha (Stage 5 pilot) - Phase CH1a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1a review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1a done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1a - Done"
awk -F'\t' '$6=="CH1a"' june2017_re/char_arme/ch0_class_map.tsv      # the 40 June functions
grep -a "Char CH1a" june2017_re/funcmap.tsv                            # the 44 rows the phase wrote
```

The 40 `June_*` members are at the end of `X2Lib/X2GUArme_VioletMage.cpp`, declared after `ChargeMPEnd` in the header.

## Task

Review the transcription against June, function by function. **Delegate the mundane reads** (x2-phase, *When the phase is Opus*): the Ghidra dumps of the 40 functions (`decompile_function` at each `june_fn`) go to the scratchpad, you read the parts below. What the phase could get wrong:

- **Branch structure that the decompiler hides.** `32cc60a0` (dash jump, the `@32cc62fd` block runs on the ground too), `32cb9a70` / `32cb9ba0` / `32cb9ce0` (jump down, levitation, dash levitation), `32cd0200` / `32cd21d0` (the charge-MP entry and exit). Read the listing with `june2017_re/funcdis.py --func <va>`, not only the decompile.
- **Compare directions.** Every `X < Y != (X == Y)` is `X <= Y`; every `(0 < f) == (f == 0)` skip means `f < 0`. Check each `0.0f <= nowSpeed.y`, `0.5f <= m_fMPChargeWaitTime`, `GetNowAnimationTime() <= GetMaxAnimationTime() / 3.0f`.
- **Constants.** Several are qwords in `unpacked_x2.exe` (45.0 `33472998`, 30.0 `33439c68`, 40.0 `33439c70`, 0.2 `334411a8`, 0.7 `3344af88`, 1.2 `3346f488`, 3.0 `33439b50`, 0.5 `3342ebc8`, 1.0 `334355b0`, 10.0 `33438088`). `june2017_re/` has no tool for them; the phase's scan is in the Done note. Re-read two.
- **InputData bytes.** `+0x86da` is `pureLeft`; groups of 4 in March's member order (`86e2` pureUp, `86e6` pureDown, `86ea` Z, `86ee` X, `86f2` C, `86f6` A, `86fa` S, `86fe` D, `8706` Q, `870a` W, `870e` E, `8712` R, `8722` LCtr). Check the 15-key condition of `32cd07e0`.
- **The open items** the Done note lists (the `+0x9770` field, the CX2Game `+0xc20` byte and the `+0xb38` setter `322cc030`, the 2.8 float, `June_IsMemoRiseChargeActive`). Decide each: an owner in CH1 / CH1h / CH1i, or a user question. These need Opus; the phase did not guess them.

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1a commit>^..<CH1a commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review.

Spot-check 3 functions directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1a -- Review" into the plan doc under the Done note, update the CH1a row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1b.md` still matches, and commit with explicit pathspecs.
