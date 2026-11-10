# June character Rena (Stage 7) - Phase CH3c0 - Night Watcher engine hook (`31fcd700` -> `32ca6cd0`) (Opus)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH3c0` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH3b2 review (same `X2DamageManager.*`; CH3a / CH3b have no review of their own since 2026-11-10; runs before CH3c); one at a time with the other `X2GULire_ElvenRanger.*` / `X2GUUser.*` phases |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` (or the owning shared flag when the helper is `CX2GUUser` / engine code) |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | `kickoffs/char-rena/CH3c0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md "Rules" "Phase CH0 - Rena map (read-only)" "CH0 - Done (2026-11-07)"
awk -F'\t' '$7=="CH3c0"' june2017_re/char_rena/ch0_class_map.tsv

```

## Task

CH0 found a Night Watcher engine hook no doc maps: base `CX2GUUser` `31fcd700` calls Rena's `32ca6cd0` only when the unit class is 114, from three engine sites (`32037afb` in user code, `322739d9` in the damage path, `3279bbd8` in NPC code). Identify what the hook does (CH0 guess: Night Watcher's delayed-firing marks; verify), give each piece a March home (the base hook in `CX2GUUser` under the owning shared flag, the class functions under `_CORE` with plain names), and port the whole chain: `32ca6cd0` and the class functions only it reaches (the class map rows with phase `CH3c0`). CH3c (Night Watcher skills) depends on it.


You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address above has a funcmap row (`Char Rena CH3c0`) and a March body (or a recorded reason it needs none); the build passes; `CH3c`'s kickoff names the March symbols; `junediverge.py --check` and `enumscan.py --check` PASS.

## Amended by CH0 (2026-11-07)

- **Functions: 14** (`awk -F'\t' '$7=="CH3c0"' june2017_re/char_rena/ch0_class_map.tsv`; new 14).
  June addresses: `32c9a810`(new ?) `32c9aa60`(new ?) `32c9aae0`(new ?) `32ca6cd0`(new ?) `32cb5060`(new ?) `32cb5310`(new ?) `32cb5390`(new ?) `32cb5550`(new ?) `32cb56d0`(new ?) `32cb5810`(new ?) `32cb5a00`(new ?) `32cb5a60`(new ?) `32cb5c60`(new ?) `32cb5df0`(new ?).
