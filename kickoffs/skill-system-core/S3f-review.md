# June skill system core - Phase S3f review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3f review` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | Phase S3f done (its Done note and commit) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_STATE_ULTIMATE` (+ `SERV_IRUHADEV_JUNE_SKILL_DEBUG`) |
| Log tag | `[JUNE_SKILL] SULT` |

## Read first

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Phase S3f - CommonStateStart's upgraded-skill flag"
```
Then the funcmap rows tagged `(S3f)`.

## Task

Review Phase S3f's commit against June's binary (read-only Ghidra; `june2017_re/disasm_range.py` where Ghidra has no function):
- The flag's one computation (`32057b80 @32057c03-32057c1d`, `@3205ad70-3205ad82`) and every consumer that reads `[esp+0x40]`: `32024ff0` `@3205ad90`,
  `31fcd280` `@3205c865-3205c883`, `31fe32d0` `@32060693`. Check each read at its stack depth (a write under pending pushes is another slot).
- `31fcd280` vs the March `SetDamageData` path S3f uses: the `_ULTIMATE` -> base fallback, and what a miss leaves in `m_DamageData`.
- `LoadEffectSetFromScript`'s argument: what June's third argument selects in `32024ff0`. Confirm that S3f did not port list choices that belong to checklist row 397.
- `DAMAGE_DATA_NEXT`: June's function vs the port, whole.
- `APPLY_MEMO` / `DAMAGE_DATA_MEMO`: the XOR-table evidence for "June dropped them", and the `#else` keeping March's path intact.

Hygiene is one command per flag: `python tools/flagcheck.py <FLAG> --range <S3f commit>^..<S3f commit>` (plus `git log -S<FLAG> <commit>..HEAD`).
Paste the `files:` / `in this flag's files:` / `RESULT:` lines and explain every OTHER row. Fix clear transcription errors under the same flag
(then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase S3f -- Review" into the plan doc, update the S3f STATUS.md row,
and commit. Read the play-test log first if the user has run it (`python tools/logs.py --tag JUNE_SKILL`).
