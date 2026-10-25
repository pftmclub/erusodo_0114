# June character Eve (Stage 7) - Phase CH1c review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1c review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1c done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1c - Done"
awk -F'	' '$6=="CH1c"' june2017_re/char_eve/ch0_class_map.tsv      # 17 thunks (329ac330 moved to CH1k, six Stage10 / PvP rows left out)
grep -a "Char Eve CH1c" june2017_re/funcmap.tsv                      # the 17 PORTED rows and 3 "not ported here" rows
```

The 17 `June_ENSI_*` members are in the third `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp`; declarations at the end of the class in `X2Lib/X2GuEve.h`.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1c --out <scratchpad dir>`. You decide:

- **Thunks:** each of the 17 is `mov eax,[ecx]; jmp [eax+slot]`. Check every slot against Eve's vtable `334cf45c` (`ch0_vtable_all.tsv`) and that the member's call is the matching `CX2GUUser` virtual (`X2GUUser.h:2169-2181`, `:2351-2382`). `RidingOffEnd` is non-virtual in March's header (June slot `0x610`): confirm Eve does not hide it.
- **The three decisions:** (1) the six PHOENIX / DANCE thunks (`329cc030` `329cc130` `329cc1c0` / `329cc040` `329cc140` `329cc1d0`) are left out: their targets are June-only `CX2GUUser` functions March lacks (Stage 10 dance in `JUNE_PORT_BACKLOG_ORDER.md`, PvP amusement), as the pilot left them out; (2) `329ac330` moved to CH1k with its callee `32997c60` (CH1k kickoff amended); (3) JUNE_BUGS 422.
- **Spot-check the decisions in Ghidra** (read-only): `31fcbb60` / `31ff9280` read as dance and wait callbacks; `329ac330` is `32997c60( 0, 0.1, true )`.
- **Link question:** the CH1k kickoff says to declare later-phase callees with "body in CHn". Say whether an undefined declaration called from a compiled member links (US_SERVICE has no function-level linking); if not, the CH1j-CH1q kickoffs need the same move.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1c commit>^..<CH1c commit>`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1c -- Review" into the plan doc under the Done note, update the CH1c row in `STATUS.md`, check that `kickoffs/char-eve/CH1d.md` still matches, and commit with explicit pathspecs.
