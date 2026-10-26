# June character Eve (Stage 7) - Phase CH1l3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1l3 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1l3 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` (names registered under `SERV_IRUHADEV_JUNE_ENUM_NAMES`) |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1l3 - Done"
awk -F'	' '$6=="CH1l3"' june2017_re/char_eve/ch0_class_map.tsv         # 24 rows: 17 June_ members, 7 resolved to March / existing code
grep -a "Char Eve CH1l3" june2017_re/funcmap.tsv                        # 34 rows
```

The members are in the fifteenth `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block at the end of `X2Lib/X2GuEve.cpp` ( +1113 lines ); declarations at the end of the class and inside `MEGA_ELECTRONBALL_Data`, `TASER_PILUM_Data`, `EveElectraSystem` in `X2Lib/X2GuEve.h`.
Registered names: 10 `RNSI_*` in `X2SkillTree.h`, 3 `BFI_*` and 1 `BTI_*` in `X2BuffFactor.h`. JUNE_BUGS 435 and 436.

## Task

Review against June. **Delegate the mundane reads**: `python june2017_re/charport_adis.py eve --phase CH1l3 --out <scratchpad dir>` ( 24 listings; the two routed rows `3297baa0` / `3299ba70` are not in it ). You decide:

- **`329b5af0` ( step 510 ), the big one.** The Mega / Taser release logic ( `+0x9784` / `+0x9a54` enable, key released by `31fce8b0`, the 5 s timeout, the `32207210` base id, the two slot lookups ), the use-condition substitutions ( `m_iJune1c0` = `+0x99a4` for 25202 / 25215, 2 for Photon Blink with 25420 ), the MP rules ( 25202 / 25215 while `+0x97e4`, 25028 / 25068 with a count ), the `+0x9c58` Conversion HP test and the `+0x9b98` parry state 5 ( under `ADD_EVE_SYSTEM_2014` ), and that the dev free-skill gate wraps exactly June's cool-time / MP / hyper block ( the pilot's form ).
- **`32981130` ( step 512 ).** The Spectro cool-time table ( E / S / O, `+0x97e4` / `+0x9831` / `+0x9978`, exception `+0x9980`, `+0x804b` ), Open Mind ( 25419 ) refund with `329cc390`, the Nasod gauge line ( JUNE_BUGS 435 ), `3235a710` / `31fcd3d0` order, the `328941a0` callee ( JUNE_BUGS 436: is March's `ResetLeftSkillCoolTimeAll` an acceptable stand-in, or does the user want June's form? ask ).
- **`329b6810`.** All 13 actions against the listing ( jump table `329b72f4` / `329b732c` ), the Thousand Stars enemy-team test and `1 - (int)( n * -0.5 )` form, Taser `+0x9a7c` = ability `0xa9`, Mega `+0x15`, the case 0x40 calls left out ( CH3a ).
- **`3297c570`, `3297a6d0`, `32976f30`, `32972480`, `3297a800`, `329b4b40`, `329a0ae0`, `329cc390`, `3299ebd0`, `3299f390`, `329b3cc0`, `32969300`, `329693e0`.** Argument order of `31edf580` ( reversed in `329b3cc0` ), BFI argument order ( creator, level, adjust, skip ), `m_iJune1c0` writes.
- **The routed rows** ( class map and kickoffs CH1l4 / CH2a2 / CH3a / CH3b amended ) and the Eve-local copies of CX2GUUser functions ( 7 now; ask the user whether a CH1a2-style shared home is wanted ).

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1l3 commit>^..<CH1l3 commit>` and the same for `JUNE_ENUM_NAMES`; paste the `files:` / `in this flag's files:` / `RESULT:` lines. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write "CH1l3 - Review" into the plan doc under the Done note, update the CH1l3 row in `STATUS.md`, check that `kickoffs/char-eve/CH1l40.md` still matches, and commit with explicit pathspecs.
