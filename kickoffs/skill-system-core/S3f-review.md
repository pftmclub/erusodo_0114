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
- The hoist: S3f computes the flag once, right after `CountReservedSkillUse` (June `@32057bfb`), and reads it at all three consumers. Confirm that nothing between
  `@32057c03` and `@3205ad70` could change the equipped templet.
- `31fcd280` vs S3f's new `CX2GUUser::SetDamageData` (ported whole, one argument): the `_ULTIMATE` -> base fallback, the force-down x87 branches (`@31fcd2c4-31fcd3a6`:
  > 0 multiply, < 0 with rate > 1 / rate < 1), and `+0x4058` = the melee rate (init `322a4335`, setter clamp `31fe1b5c`). Also check vslot `0x83c` = `ChangeDamageData`.
- `LoadEffectSetFromScript`: June's second argument = MemoCondition `+0x7d10` slot 1, and S3f's "always false". Check `31fd4450` and the 12 class overrides
  (funcmap row `334720bc`, JUNE_BUGS 332). The third argument (`bJuneUltimate_`) is plumbed and unread. Confirm that S3f did not port list choices that belong to checklist row 397.
- `DAMAGE_DATA_NEXT`: June's `320454a0` vs the port, whole.
- `DAMAGE_DATA_MEMO` / `DAMAGE_DATA_NEXT_MEMO`: absent as plain text and from `xorstrings.tsv` (`APPLY_MEMO` is present, in `31fc8cf0`). The `#else` keeps March's path intact.

Hygiene is one command per flag: `python tools/flagcheck.py <FLAG> --range <S3f commit>^..<S3f commit>` (plus `git log -S<FLAG> <commit>..HEAD`).
Paste the `files:` / `in this flag's files:` / `RESULT:` lines and explain every OTHER row. Fix clear transcription errors under the same flag
(then `tools/build.py --deploy`); hand design decisions back to the user. Write "Phase S3f -- Review" into the plan doc, update the S3f STATUS.md row,
and commit. Read the play-test log first if the user has run it (`python tools/logs.py --tag JUNE_SKILL`).
