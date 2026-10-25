# June character Eve (Stage 7) - Phase CH3a0 - Helpers: the UI skill-tree notify and a mesh-instance float (before CH3a)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH3a0` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH2c2 review. One at a time: `X2GuEve.*` / `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL` |
| Log tag | none (inert until `_LIVE`; the phase that uses it play-tests it) |
| Review | `kickoffs/char-eve/CH3a0-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "Phase CH0 - Eve map (read-only)" "CH0 - Done (2026-10-26)"
awk -F'	' '$1=="31fd0620" || $1=="31c82df0"' june2017_re/char_eve/ch0_unmapped_identified.tsv
```

## Task

CH0 found callees outside Eve's class range that no doc had named (`june2017_re/char_eve/ch0_unmapped_identified.tsv`, kind / candidate name / evidence). The Sonnet phase `CH3a` would stop on them ("this needs Opus"). Map each one in Ghidra, name it, give it a March home under `SERV_IRUHADEV_JUNE_CHAR_EVE_SKILL`, and port its whole body (June's numbers and quirks verbatim, a `JUNE_BUGS.md` row each). Write a funcmap row per function (`Char Eve CH3a0`) so `CH3a` calls it by its March name.
- **`31fd0620`** (callers `32976fe0` Psyker Kinesis cooldown, `3299a670`): IsMyUnit && UI skill tree (`g_pData+0x2cc`, `3214f050`) -> optional upgrade swap `328905b0` -> UI skill-id notify `328da730` (Psyker Kinesis passes 0x6293).
- **`31c82df0`** (caller `3298d920`): sets one float at `+0x36c` of the main-effect mesh instance after CreateInstance `31f8ff00` (value = x / const + const). Name the CKTDGXMeshPlayer instance member.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only. Done gate: every address above has a funcmap row and a March body (or a recorded reason it needs none); the build passes; `CH3a`'s kickoff names the March symbols.
