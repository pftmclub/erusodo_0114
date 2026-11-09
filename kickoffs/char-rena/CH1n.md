# June character Rena (Stage 7) - Phase CH1n - Base callbacks and the slot head

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase char-rena CH1n` |
| Plan doc | `JUNE_CHARACTER_RENA_PHASES.md` (branch `main`) |
| Depends on | CH1m review. One at a time: `X2GULire_ElvenRanger.*`, `X2GUUser.*` |
| Flag | `SERV_IRUHADEV_JUNE_CHAR_RENA_LIVE` (undefined until CH1 final), `SERV_IRUHADEV_JUNE_CHAR_RENA_CORE` |
| Log tag | none (nothing runs until `_LIVE`) |
| Review | `kickoffs/char-rena/CH1n-review.md` (Opus 5.5 high; the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_RENA_PHASES.md --no-sub "Rules" "Starting context" "Phase CH0 - Rena map (read-only)" "Phase CH1 - Deploy unit: June script, motions and the class core"
python tools/docsection.py JUNE_CHARACTER_RAVEN_PHASES.md "CH1n - Done" "CH1n - Review"
```
Then CH0's Done note, the earlier Rena CH1 Done notes and reviews (their routed gaps), and the matching rows of
`june2017_re/char_rena/ch0_*.tsv`. Raven's matching sub-phase is the worked example.

## Task

Port the `CX2GUUser` base callbacks Rena registers (Raven's CH1n triage first: only rows earlier characters did not cover) and the head of
`SetEquippedSkillFuncBySkillSlotIndex` `32cb06d0` (vslot 0x7b4: bounds, slot tables, the 7i rule, the pre-switches) plus the dev free-skill gate
(`CX2GUUser::IsDevUnlimitedSkillUse()` in her skill-use check). The skill and hyper switch bodies belong to CH2 / CH3; map from `ch0_state_skill.tsv`
and CH0's switch map. Shared helpers (`GetSkillLevel`, the seven skill-use helpers) are called, never copied. Done gate: the build passes with
`_LIVE` undefined and the one-file probe with it defined compiles; no play-test.

You are on Opus: delegate the reads, greps and known-address Ghidra dumps (x2-phase, *When the phase is Opus*). CP949 files through
`bytepatch.py`; `cl /P` each `#ifdef` you edit. Nothing runs until `_LIVE`: no play-test; the hand-off names the next phase.

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

## Amended by CH0 (2026-11-07)

- **Functions: 2** (`awk -F'\t' '$7=="CH1n"' june2017_re/char_rena/ch0_class_map.tsv`; inside 1, new 1).
  June addresses: `32cb06d0` `32cb5760`(new ?).
- **Checklist rows** (`ch0_checklist_rows.tsv` col 5): §7b L328 skill-memo entry 31fdbbb0 and the class callers of 31fdbcc0; §7i L444 SetEquippedSkillFuncBySkillSlotIndex Lire 32cb06d0: drop the snapshot ; §7ae L726 types 14-16 / 17 / 18 named in the class switch.
- `32cb06d0` has three switches (skill type: byte tbl `32cb3004` / jump tbl `32cb2ff4`; base-id chain with the 6-case hyper switch, jump tbl `32cb31e0`; skill-id switch: byte tbl `32cb30fc` / jump tbl `32cb3010`): `ch0_skill_listing_rows.tsv` 157 rows, 111 ids with events; 19 ids (106, 23400-23417; types 1 / 8) leave at the type switch. The default branch plays `LESI_SI_LE_POWER_ATTACK1` (CH2a).

## Amended by CH1m (2026-11-09)

- **User callbacks:** `june2017_re/char_rena/ch1m_user_callbacks.tsv` lists the 131 `CX2GUUser` members Rena's registration names; all are on Raven's CH1m list except `RandomLoseEventProcess` ( already used by every ported class ), so the base-callback triage finds no row earlier characters did not cover.
- **Slot states are yours:** ids 167 ( `LESI_SIEGE_READY` ), 202, 283, 295-302 are made in `32cb06d0`, not in the two registrars ( 295 `LESI_SA_LCR_LOW_KICK` is also registered by InitStateCommon `@97`, kept ). Register them by script table name as the CH1i amendment in `CH1m.md` lists.
