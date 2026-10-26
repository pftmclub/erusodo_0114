# June character Eve (Stage 7) - Phase CH1j20 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-eve CH1j20 review` |
| Plan doc | `JUNE_CHARACTER_EVE_PHASES.md` (branch `main`) |
| Depends on | CH1j20 done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` |
| Log tag | none (inert code, no play-test) |

## Read first

```sh
python tools/docsection.py JUNE_CHARACTER_EVE_PHASES.md "Rules" "CH1j20 - Done"
grep -a "Char Eve CH1j20" june2017_re/funcmap.tsv                     # 6 rows
grep -n "^| 426 \|^| 427 " JUNE_BUGS.md
```

The code is one `SERV_IRUHADEV_JUNE_CHAR_EVE_CORE` block in `X2Lib/X2GUUser.cpp`, right after `JuneCommonSpecialAttackEventProcess`, and one block in `X2Lib/X2GUUser.h`, after `m_vecJuneSpecialOptionValue`.

## Task

Review against June. These are five small functions. Decompile and disassemble them yourself (`31fce710`, `31fce8b0`, `31fc89a0`, `31fce670`, `31fce6d0`), plus the ctor store `3203e850 @320403b5`.

- **Key bytes.** June offsets from `+0x86da` map onto March `InputData` members by name, not by offset: March's `Skill5` sits at `0x4c`, June's at `0x50`. The mapping used: A 0-4 = A / S / D / C / Skill5, B = Q / W / E / R / SkillEx5. `31fce710` reads the one-byte and the other two read `pure == 0`. Check it against `31fcadd0`'s `+0x872b` (= oneSkill5) and `X2Game.cpp`'s `InputData( GA_SKILL5 ... )` lines.
- **`31fce710`**:
  - the double `31fca8e0( id, 1 )` and the `+8` = `m_eID` swap;
  - A is searched before B;
  - the B-lock gate: it is skipped when `g_pData`, my user or the select unit is NULL; key 0 is for B 0-3 and key 1 for B 4; it reads my unit's tree (JUNE_BUGS 427). March's static `JuneIsMyUnitSlotUsable` was not reused because its NULL case returns false, where June's skips the gate.
- **`31fce670`**: `+5` when the stored byte is 1 and `-5` otherwise (`@31fce6a7` neg / sbb / and `-10` / `+5`; the CH0 tsv said "-11"), and the byte is flipped, not copied (JUNE_BUGS 426). Decide whether 426's wording is right.
- **`31fce6d0` / `31fce670`** have no `g_pData` test in June, so the port has none either. **`31fc89a0`** returns true outside 0-9.
- **`+0x115` = `m_bSkillSlotSwap`**: check `328db100` / `328daed0`, which write it. Check that the struct is a `CX2GUUser` member, since `+0x8b58` is below `+0x93a8`.
- **Not settled:** `+0x88bc`, the argument of `32967560` (CH2c's kickoff carries it). If the review can name it cheaply, amend `CH2c.md`.

Hygiene: `python tools/flagcheck.py JUNE_CHAR_EVE_CORE --range <CH1j20 commit>^..<CH1j20 commit>`. Paste its `files:` / `in this flag's files:` / `RESULT:` lines. Every OTHER row and every deleted line needs an explanation.

Fix clear transcription errors under the same flag, then `tools/build.py --deploy`. Write "CH1j20 - Review" into the plan doc, update the STATUS row, check that `CH1j2.md`'s "Amended by CH1j20" still matches, and commit. Next: `/x2-phase char-eve CH1j2` (Sonnet 5.5 high).
