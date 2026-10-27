# June character Eve (Stage 7) - Phase CH2b4 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH2b4 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | Phase CH2b4 done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_SUMMON_UNIQUE` |
| Log tag | `[JUNE_SUMMON]` |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH2b4 - Done (2026-10-27, Sonnet 5.5)"
```

Rows: `june2017_re/funcmap.tsv` (grep `CH2b4`).

## Task

Review CH2b4 against June (`python june2017_re/charport_adis.py eve <va>` for the raw listings):
- `327b53b0` @`327bcac9..327bcfe0` against `CX2GUNPC::JuneDropOwnerSummonBuffOnDying`: where the `else` sits (inside the self-resurrection gate, on the `IsAnimationEnd` false path),
  the owner gates (`+0x8d5c`, `- 0x1a0`, twice), `+0x8b40`, the 0x5ae / 0x644 branch (owner's `+0x867c` unit class `[+0x38]`, 0 without a unit), the other-ids branch
  (`31fc96b0`, `GetBuffFactorPtr`, factor `+0x28` = `GetBuffTempletID`, the `32778b10` gate, `3229e160`, `+0x8b40 = 1`), and that nothing else in the June animation-end branch (`@327bcadc..327bcdf8`) calls the Ferdinand reset (so March's call is `#ifndef`'d).
- `32778b10` against `JuneIsAnotherSameSummonAlive`: the id test, `0 < HP` (NaN skips), `+0x8b40`, AI type 1, owner UID `+0x78` / `+0x7c`; the added null / type guard on my AI.
- `31fc8a90` against `CX2GUUser::JuneResetOnSummonEnd`: both class branches, `329684c0` / `329d4690` / `329fb6c0` callees; `BTI_BUFF_APOCALYPSE = 345` against `Enum.src.lua`.
- Is the Mastermind `+0x98e4` store routed to the Add / Mastermind port (a funcmap row, not a gap)?

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_SUMMON_UNIQUE --range <first phase commit>^..<commit>`, then `git log -S SERV_IRUHADEV_JUNE_SUMMON_UNIQUE <commit>..HEAD`.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH2b4 - Review" into the plan doc, update the STATUS row, check the next kickoff (`CH2c2.md`), and commit.
