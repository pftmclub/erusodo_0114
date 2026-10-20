# June character Aisha (Stage 5 pilot) - Phase CH1i review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH1i review` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH1i done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARME_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH1i - Done"
grep -a "Char CH1i" june2017_re/funcmap.tsv                            # the 60 rows the phase wrote (PORTED / PARTIAL / STAND-IN / ROUTED / N/A)
awk -F'\t' '$6=="CH1i"' june2017_re/char_arme/ch0_class_map.tsv        # the 45 class-map rows
```

The `June_*` members are in the ninth block (`//{{ ... Phase CH1i`) at the end of `X2Lib/X2GUArme_VioletMage.cpp`, declared after CH1h's in the header.
**Not carved in Ghidra (read with `python june2017_re/funcdis.py --func <va>`; its bounds merge neighbours, so stop at the first `ret`):** `32cb6ea0`, `32cb8ab0`, `32cb9650`, `32cc5b00`, `32cc7b30`, `32cc8cf0`, `32cc98b0`, `32ccbca0`, `32ccdae0`, `32ce15a0`, `32ce5760`, `32cf25a0`, and the two big ones `32cebe80` ( ends at its `ret 4`, `32ceca68`; `funcdis` starts too early, `june2017_re/disasm_range.py 32cebe80 32ceca70` reads it ) and `32ced640`.

## Task

What the phase could get wrong, in the order of risk:
1. **The 510 lookup `32cebe80` -> `June_SpecialAttackLookup`** (about 700 instructions read from the listing). Check the order of the gates against the listing: quick-slot wait, `31fcadd0`, the kind ( `32cc7e60` ) and the kind-2 branch ( `0x880` contains, then `+0x9758` = 0.1 s and `31fcec40` ), the templet / slot id mismatch text `0x8bae`, Teleport + skill 22417 widening the use condition to `ASUT_ANYTIME_EXCEPT_AIR` in the three damage states ( ids 0x1e / 0x1f / 0x25 ), `31fca9f0`, `3202daf0`, `3202cba0`, `3202d140`, `3202d3c0`, `329cc370`, vslot `0x818`, then the MP / sync-field / cool-time block (the clamp is `min( P1, fCool )`, derived from `fcomp st(1)` + `test ah, 0x41`), the MP test `ea8 + eb0 > NowMp` and the hyper stone test. Check the text ids `226`, `2549`, `26119`, `0x6219`, `0x7697`, `0x8bae`, `0x8399`, `0xaf9a`.
2. **`32ccf4e0` -> `June_SpecialAttackCooldown` and `32ced640` -> `June_SpecialAttackUse`**: the mode tests ( 2 / 3 ), `31fc86e0` ( bytes `+0x90d3 + slot` ), the Strong Mind refund ( rates at `+8` / `+0xc`, restore rate `+0x14`, `3.0` s ), `0x764` ( memo type 10 ), `31fd32e0` ( `0x33472040` tables: A, hyper A, B, hyper B; slot >= 5 is B ).
3. **`32cce4b0` -> `June_CommonFrameMove`**: the `+0x95f0` fade ( `m_ahEffectSet[ EffSetID_June2 ]`, `+0x9780`; scale = min( t, 0.33 ) / 0.33 ), the UFO block ( transcendence memo of 22020, the 20 skill-slot states + 10 Hell Drop states are skipped ), `32cc2ab0`, the hammer and staff mesh blocks, the final `+0x9754` copy for other units.
4. **`32ce7480`** ( the nine Angor Hammer states, the Hell Drop timer, the empowered cast ) and **`32ccf9f0`** + `32008e40` ( the UAI ids 0x24 / 0x25 / 0x29 / 0x36-0x39 are June's, no March enumerator ).
5. **`32ccb830`**: state ids ( Done note ) and the `SetSkillLevelStateData` form ( `32890620( id, 0, 0, 1 )`, x ( `3201ad10` + 1 ) ).
6. **Stand-ins and gaps in the Done note ( 1-9 )**: confirm each is a real gap and say which phase takes it. The CX2Game HUD `322f4740` goes to CH4 with `322deb80`. `June_Vslot828` needs `31fd7640` ( the list: wait `0x438`, `+0x7d04`, walk `0x43c`, dash `0x440 / 0x444 / 0x448 / 0x44c`, jump `0x450 / 0x454 / 0x458 / 0x45c`, `0x4c`, `0x4b`, `0x4d`-`0x54`, then `0x520` / `0x578` false ).
7. **Skill 900 ( `SI_ETC_WS_COMMON_LOVE` )**: `3202d140` and `3202d3c0` only matter for it; check the game type numbers ( 4 = PVE, 2 = dungeon ).

Hygiene is one command: `python tools/flagcheck.py JUNE_CHAR_ARME_CORE --range <CH1i commit>^..<CH1i commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review ( the 31 deleted lines are CH1a / CH1b / CH1h stand-ins, in the Done note ).

Spot-check 3 functions directly in Ghidra / the listing (read-only); if one fails, re-check the whole phase. Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user.
Write "CH1i -- Review" into the plan doc under the Done note, update the CH1i row in `STATUS.md`, check that `kickoffs/pilot-char-revamp/CH1.md` still matches ( add what the review moves ), and commit with explicit pathspecs.
