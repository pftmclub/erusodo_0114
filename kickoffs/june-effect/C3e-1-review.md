# June effect completion - C3e-1 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3e-1 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3e-1 done (its Done note, commit `a39a4ce`) and its regression play-test read |
| Flag | `SERV_IRUHADEV_JUNE_LINE_INDEX16` and `SERV_IRUHADEV_JUNE_SYNC_CLASS_FLOAT` |
| Log tag | - (no runtime log; `python tools/logs.py` for the play-test summary) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3e-1 - Done (2026-10-09, Sonnet 5.5 high)"
grep -n "(C3e)" june2017_re/funcmap.tsv
awk -F'\t' '$1=="#"||$1=="C"||$1=="G"' june2017_re/c3e_user_sync_fields.tsv
grep -n "^| 146 " JUNE_BUGS.md
```

## Task

Review C3e-1's commit against the June binary (Ghidra read-only).

1. **Encode / decode width (the phase's one deviation from the kickoff).** Confirm `32006d80` (encode) and `32000f70` (decode) test
   `param & 0xff` and nothing wider, and that `3203d4d0` passes the full 16-bit index only to the packet (`uStack_30e`, packet `+0x12`). If
   either function reads more than the low byte, widen `_EncodeUserSyncPos` / `_DecodeUserSyncPos` under `LINE_INDEX16` and drop JUNE_BUGS #146.
2. **NPC twin.** Confirm `327837c0` -> `32d5c1f0` writes one byte for the line index and that no June NPC reader (`32d5b...`, the NPC receive
   path) reads two. The 3 `X2GUNPC.cpp` sites where an NPC copies a user's index (`USHORT` -> `unsigned char`) are the only places the widths meet.
3. **G on the wire.** Serialize / Deserialize order against `32d5be30` / `32d5c2e0`: `0x8000`, then `0x200000` (4-byte float, compared as a
   float), then `0x800000`. `eUnitUserSync_DataField_All` must include `0x200000`, else `ReceiveFrameData` drops every packet; check the
   `m_usDataFieldFlag` accumulation in `Deserialize` still ends at `All`.
4. **G in SyncData.** Default 0 in `Init`, copied in the sender and in `SetData`, and flows Future -> Now through the whole-struct copy
   (`FrameData::operator=`). `SyncData::Verify` does not check it (June's checker not read): decide whether June's Verify covers slot `+0xa4`.
5. **Line-map overloads.** `KTDGLineMap.h` `unsigned short*` overloads: same body shape as the `unsigned char*` ones; no call site silently
   picks the `int*` version through an implicit conversion.
6. **Hygiene.** `python tools/flagcheck.py JUNE_LINE_INDEX16 --with JUNE_SYNC_CLASS_FLOAT --range a39a4ce^..a39a4ce`. The commit was rebuilt
   from exact patched copies of the HEAD blobs after a first attempt misplaced hunks; confirm the committed `X2GUUser.cpp` hunks sit where the
   worktree edit put them (`git show a39a4ce -- X2Lib/X2GUUser.cpp`: encode call inside the loop, G copy after `m_dwRelativePos`, SetData lines).

Fix clear transcription errors under the same flags (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"C3e-1 - Review" under the phase in the plan, update the STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.
