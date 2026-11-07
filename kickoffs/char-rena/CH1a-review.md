# June character Rena (Stage 7) - Phase CH1a review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1a review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1a done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` and `SERV_IRUHADEV_JUNE_CHAR_RENA_NF` (both `//#define` until CH1 final / CH1q); no `_CORE` yet (CH1a had no `new` row) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1a - Done"
awk -F'\t' '$7=="CH1a"' june2017_re/char_rena/ch0_class_map.tsv
```

## Task

Review the 32 CH1a functions against June (`python june2017_re/charport_adis.py rena <va>...`; the listing, not the decompile, carries the `StateChange` arguments and the `fcomp` / `test ah` compare direction). Spot-check at least these, end to end:
- **`32c89bd0` Wait EventProcess**: the charge-MP test (`fStateTime >= 1.0` at +0x7de8, `GetNowMp() != GetMaxMp()`, slide show absent, +0x9700 clear; inner: emotion +0x89d8 none, `g_pX2Game`, CX2Game +0xc20 clear). `322cdfa0` read as `GetSlideShot()->IsPresentNow()` and `31aadcb0` as `g_pX2Game != NULL` are *candidates* in `ch0_unmapped_identified.tsv`: confirm or correct.
- **`32c83850` DashJump EventProcess** (both-keys branch, class Z switch 24 / 103 / 114 -> 0xdf / 0xd5 / 0xdf), **`32c83b90` / `32c83c10`** (both-keys facing + speed), **`32c861c0` / `32c6e1a0`** (X: state 0x9e for every class; JJX guide only outside 13 / 103; `ShowComboCommand` third argument is `true`).
- **`32c8bb50` ChargeMP StartFuture**: the x0.8 scale and the claim that `+0x6894` has no writer (`scanfield`-style scan: 7 hits, all readers). `32c8cb90` EventProcess `+0xc20` exit; `32c8c080` FrameMove / `32c8ce00` End = March (constants listed in the Done note).
- **`ADD_RENA_SYSTEM` arms**: the dash-jump branches in JumpUp / JumpDown and the `LESI_SNIPING_RANGER_DOUBLE_JUMP_ATTACK_X` arm of both double jumps are written plain (June has no macro; every state they name exists in March). Only WaitStart's `m_eSaveStateSkillId` / relate-data lines sit under `_NF`. Confirm that split, or move a line.
- `june2017_re/char_rena/ch1_offsets.tsv` (seeded from Raven's base rows, 16 Rena rows added): check the class offsets against the listings.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_NF --range <CH1a commit>^..<CH1a commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (and with `_NF` + `ADD_RENA_SYSTEM`). Fix clear transcription errors under the same flag (then `tools/build.py --deploy`).
Write "CH1a - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1b.md`, commit with explicit pathspecs.
