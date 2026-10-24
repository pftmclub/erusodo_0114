# June transcendence - Phase T6c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-transcend T6c review` |
| Plan doc | `JUNE_TRANSCENDENCE_PHASES.md` (branch `main`) |
| Depends on | Phase T6c done (its Done note and commits) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_TRANSCEND_IMAGE` |
| Log tag | `[JUNE_TRANSCEND] IMAGE` (`python tools/logs.py --grep "JUNE_TRANSCEND] IMAGE"`) |

## Read first

```sh
python tools/docsection.py JUNE_TRANSCENDENCE_PHASES.md "Phase T6c - Transcended class pictures" "T6c - Done"
grep -n "Transcend T6c" june2017_re/funcmap.tsv
```

## Task

Review T6c's commit(s) against June:
- `31ee0de0`: the type switch (type 9 base `337962e0`, type 0xa base `33798ce0`, row stride 0x70, transcended pair at +0x38 / +0x54, taken only when the flag `== 1` and the row's +0x48 length is not 0).
  Read the Aisha EM / VP / DW rows of both tables (the unit slot is `list_TR_E_MASTER` / `_V_PRINCESS` / `_D_WITCH` and `_GRAY`; the pieces are in June's TET, but the table strings were NOT read off the static
  initializer in T6c: confirm them from the initializer ctor `31ef0ef0`, e.g. by emulating the stores or the first play-test log line).
- `3256d6e0` (select-screen refresh): `[unit +0x9d0] == 1` is March's `m_bDeleted`; type 0xa then type 9; point ( 23, 11 ), size 38, colour 1.0 on both paths (March tinted 0.5).
- `32564d50 @325659c9`, `3291df90 @3291e505`, `32513d30 @32515156` (both the type 2 and 3 calls), `328b89c0 @328b8e6f`, `324924a0 @324939f9`: the flag is the unit's `+0x3c`, March passes `1 == m_iUnitClassTran`.
- The staged data: data057 / datai021 `DLG_UI_Common_Texture_GUI_character_01(.TET/.tga)` and `_gray` are June's byte-exact (`kom.py status data057 datai021`).

Hygiene: `python tools/flagcheck.py JUNE_TRANSCEND_IMAGE --range <first T6c commit>^..<last T6c commit>`, then `git log -SJUNE_TRANSCEND_IMAGE <last T6c commit>..HEAD`. Paste the `files:` / `in this flag's files:` / `RESULT:` lines.

Spot-check 2-3 items directly in Ghidra (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`);
hand design decisions back to the user. Write "T6c - Review" into the plan doc, update the T6c STATUS row, and commit.
