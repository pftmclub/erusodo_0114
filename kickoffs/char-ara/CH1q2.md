# June character Ara (Stage 7) - Phase CH1q2 - Specter states and tails

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-ara CH1q2` |
| Plan doc | `JUNE_CHARACTER_ARA_PHASES.md` (branch `main`) |
| Depends on | CH1q review. One at a time with CH1-CH4: they all edit `X2GUAra.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_ARA_SPECTER` (class code) and a `SERV_IRUHADEV_JUNE_CHAR_*` flag for `CX2GUUser` pieces (+ `..._SPECTER_DEBUG`) |
| Log tag | `[JUNE_CHAR]` (`ARA SPECTER`) where the phase logs; otherwise none |
| Review | `kickoffs/char-ara/CH1q2-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md --no-sub "Rules" "Phase CH0 - Ara map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_ARA_PHASES.md "CH1q - Done (2026-11-11)" "CH1q - Review (2026-11-11): pass, debug define retired"
awk -F'\t' '$7=="CH1q2"' june2017_re/char_ara/ch0_class_map.tsv
```

## Task

Specter states and tails from `ch0_specter.tsv` (June-only): the `IS_SPECTER_STATE` key's loader and its `CX2GUUser` readers
(`CommonStateStartFuture` `320506f0`, `32057b80`), every class reader, `ANIM_NAME_TAIL` / `TRANSITION_TAIL` / `TAIL_TYPE` and the tail mesh attach
(`Motion_Ara_Tail_03.x` / `_06.x`, staged by CH0b). Class-agnostic pieces go in `CX2GUUser` under their own `SERV_IRUHADEV_JUNE_CHAR_*` flag (later
classes may set the key); Ara's pieces under `_SPECTER`. Sub-tag `SPECTER`. Classes 34 / 124 only load from CHNa: say in the Done note which checks
CHNa's play-test must carry.

You are on Opus: delegate the reads, greps, known-address Ghidra dumps, probe compiles and flagcheck runs (x2-phase, *When the phase is Opus*). CP949 sources: `bytepatch.py` only.

No hardcoded enum literals (user, 2026-10-26; CLAUDE.md): an id that has a name in an Enum (June's
`Enum.lua`, or a C++ enum) is never written as a number, in any new or touched code: use the member, and
register it under a `SERV_IRUHADEV_` flag with June's value and name if C++ lacks it. A value with no Enum
entry stays a number with a one-line comment. The one exception is a table whose rows are the data itself
(a `.tsv` of ids), not code.

No `June` prefix (user, 2026-10-28; CLAUDE.md): June's body goes inside March's own function under the flag,
March's in `#else`. A function March lacks gets a name in the class's own pattern, never with `June` in it.
Every name (locals and constants too) says what it is in context: no Ghidra address, phase tag or `June` in an identifier.
Call a March function from June code only if June's namesake does the same; otherwise replace March's definition.
Run `python tools/junediverge.py --list` before transcribing; a newly found divergent function gets its `june_march_divergent.tsv` row and the replacement in the same commit.
Phase tag (user, 2026-11-09): every edited or new code block carries a one-line `// char-ara <phase>` comment (at the start of an existing one-line comment, never a second line).


## Amended by CH0 (2026-11-10)

- **Functions: 1** (`awk -F'\t' '$7=="CH1q2"' june2017_re/char_ara/ch0_class_map.tsv`, col 7 = phase; col 5 inside / new; inside 1).
  June addresses: `32baf270`.
- **Checklist rows: 2** (`awk -F'\t' '$5 ~ /(^| )CH1q2( |$)/' june2017_re/char_ara/ch0_checklist_rows.tsv`; col 5 may name several phases): L184 class keys IS_SPECTER_STATE x40, TAIL_TYPE x1 (+ ANIM_NAME_T; L237 buff hyper count readers / writers: 32ba6cc0 (vslot 39), 32b.
- **Force-power rows: 1** (`awk -F'\t' '$7 ~ /(^| )CH1q2( |$)/' june2017_re/char_ara/ch0_fp.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count; col 5 relation: differs 1).
- **Specter / tail rows: 14** (`awk -F'\t' '$7 ~ /(^| )CH1q2( |$)/' june2017_re/char_ara/ch0_specter.tsv`, col 7 = proposed phase, free text: a note in parentheses is ignored in the count).
- `ch0_specter.tsv` (19 rows), about 14 functions / members: `IS_SPECTER_STATE` is read by `CX2GUUser::CommonStateStartFuture` `320506f0` (into `+0x7da9`) and `CommonStateStart` `32057b80` (into `+0x8049`; the census label "Lire ReservedSkillStart" is wrong) into `StateParam::bSpecterState` (+0x95); 40 tables (basic combo / dash / jump-attack / stand-up attack). One reader: Ara's `AttackResultByType` `32bceca0` @`32bcee1c` (Specter of Thunder buff: cooldown `+0x979c`, chance `+0x9790` from BTI 424 `BTI_BUFF_SPECTEROFTHUNDER` / skill 27023 via `32ba6d00` / `32b90d20`). Register BTI 423 / 424 under JUNE_ENUM_NAMES (March has neither). No class test: in practice only 34 / 124 reach it.
- Tails: `ANIM_NAME_TAIL` / `TRANSITION_TAIL` are March's already (`X2GUAra.cpp:6219`; June `ChangeAnimTail` `32bd7280` is the same). June-only: `TAIL_TYPE` (`LoadPromotionItemID` `32bab890`, class cases 34 / 124, vector at +0x95d8), `InitPromotionEquipList` `32baec10`, tail show `32badd50` / `32bae120` (index min(buff count, 3) - 1), `UpdateEquipCollisionData` 0x634 `32baf270`; items 41786 / 41787 (3 / 6 tails, in the live `item.kim`), 41580 (9 tails). No class test: all 7 classes, gated by the buff hyper count.

## Amended by CH1j (2026-11-11)

- The base `CX2GUUser::CommonStateStartFuture` `320506f0` @`32051bef` multiplies the state power rate member ( +0x8900 ) by +0x8908 and sets +0x8908 back to 1.0; CH1j holds the value in `CX2GUAra::m_fNowSkillPassiveRate` ( a stand-in ) and Ara's `CommonStateStartFuture` stores it. Port that reader with the `320506f0` part this phase already owns, then move the member into `CX2GUUser`.
- `CommonStateStart` `32bd2a40` plays Necromancy's fox effect set under `_SPECTER` ( `IsSpecterState`, `m_fNecromancyRate`, `m_fNecromancyFoxCoolTime` / `m_fNecromancyFoxPowerRate` ): check it against the loader when `IS_SPECTER_STATE` lands.
