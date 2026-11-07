# June character Rena (Stage 7) - Phase CH1b review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1b review` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1b done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (`//#define`) and `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (defined, one `new` row: `KillMySummonedNPC`) |
| Log tag | none (nothing runs until `_LIVE`) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "CH1b - Done"
awk -F'\t' '$7=="CH1b"' june2017_re/char_rena/ch0_class_map.tsv
```

## Task

Review the CH1b functions against June (`python june2017_re/charport_adis.py rena <va>...`; the listing carries the `StateChange` arguments and the `fcomp` / `test ah` direction). Spot-check at least:
- **`32c6e630` / `32c6ea80` / `32c6ee80`** (DamageDownFront / Back, AirDownLanding EventProcess): the three agile-movement / stand-up flags fire on `stateChange < time || IsAnimationEnd()` (the Chung `OR_ANIMATION_END` expansion, defined file-locally in the Rena `.cpp`), the time tests run before the key test, flag indices 5 / 6 / 7 (AirDownLanding 4 / 5 / 6), state ids per listing; the no-down roll at `+0x4060` leads to the same attack state both ways (also March); the key-input arms (Back swaps left / right).
- **`32ca2010` = `KillMySummonedNPC`** (new, `_CORE`): confirm the field reads (NPC `+0x264 == 1`, templet `+0x85f8[0]`, gage `+0xc04` / `+0x20` HP > 0, AI `+0x7c98` type `+0x38 == 1`, AI owner `+0x78` == my UID `vslot 0x54`, id == `0x823` before `SetNowHp( 0 )`), and that the second argument (3.0) is never read. CH1h wires the ctor / ReInit callers (`32ca42f0` / `32ca4a40`).
- **`32ca6e90`** (CH0 said `new ?`) = March `ClearStartOfDelayedFiringData`, and `32cb6310` / `32cb61e0` = the lambda and `ApplyFunctionToLiveInstances` instantiation: confirm the six type values (2 / 4 / 6 / 10 / 12 / 14) from the byte table at `32cb6278`.
- **Die** Start (`32ca9230` / `32ca9360`) and FrameMove (`32ca5290` / `32ca52d0`): March's Night Watcher block is unconditional in June; FrameMove adds the `KillMySummonedNPC` call.
- `june2017_re/char_rena/ch1_offsets.tsv` (10 CH1b rows) against the listings.

Hygiene: `python tools/flagcheck.py SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE --with SERV_IRUHADEV_JUNE_CHAR_RENA_CORE --range <CH1b commit>^..<CH1b commit>` (FAIL only for the commented-out `#define`); `python tools/junediverge.py --check-all`; one-file `/Zs` probe of `X2GULire_ElvenRanger.cpp` off, with `/D SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE`, and with `_LIVE` + `_NF` + `ADD_RENA_SYSTEM`. Fix clear transcription errors in place (bytepatch only). Write "CH1b - Review" into the plan doc, update the STATUS row, check `kickoffs/char-rena/CH1c.md`, commit with explicit pathspecs.
