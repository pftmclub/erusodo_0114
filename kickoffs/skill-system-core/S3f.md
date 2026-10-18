# June skill system core - Phase S3f - CommonStateStart's upgraded-skill flag: the state's damage data and the effect-set argument

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase skill-system-core S3f` |
| Plan doc | `JUNE_SKILL_SYSTEM_PHASES.md` (branch `main`) |
| Depends on | S3e review (it found this). Not at the same time as S8 (both edit `X2GUUser.cpp` CommonStateStart / frame move) |
| Flag | `SERV_IRUHADEV_JUNE_SKILL_STATE_ULTIMATE` (+ `SERV_IRUHADEV_JUNE_SKILL_DEBUG`) |
| Log tag | `[JUNE_SKILL] SULT` (`python tools/logs.py --grep "JUNE_SKILL] SULT"`) |
| Review | `kickoffs/skill-system-core/S3f-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_SKILL_SYSTEM_PHASES.md "Rules" "Starting context" "Phase S3e -- Review (2026-10-18, Opus 5.5 high)" "Phase S3f - CommonStateStart's upgraded-skill flag"
```
Then the funcmap rows tagged `(S3f)` and `(S3e)` (`grep -E "\((S3f|S3e)\)" june2017_re/funcmap.tsv`), and checklist row 397
(`grep -n "32024ff0. list variants" JUNE2017_CHARACTER_PORT_CHECKLIST.md`).

## Task

Added by the S3e review (2026-10-18), an unowned June branch (memory `plan-unowned-june-branches-without-asking`). June's CommonStateStart
`32057b80` computes one flag, "the equipped version of the now-state skill is upgraded": `31fca8e0( GetNowStateSkillID(), 1 )` `@32057c03-32057c1d`,
templet `+0x178` > 0 `@3205ad70-3205ad82`, byte `[esp+0x40]`. S3e ported it only at the `ParsingBuffFactorID` call (`@32060693`, computed inline at
`X2GUUser.cpp` CommonStateStart). June passes the same byte to two more places, and March still runs its memo path there:

1. **The state's damage data.** `@3205c865-3205c883`: flag set -> `31fcd280( "DAMAGE_DATA_ULTIMATE" )`, and on false -> `31fcd280( "DAMAGE_DATA" )`.
   March: `APPLY_MEMO` -> `m_bApplyMemo` (`X2GUUser.cpp:16212-16221`) and `DAMAGE_DATA_MEMO` (`:16432-16441`). RE `31fcd280` vs March's `SetDamageData`.
2. **`LoadEffectSetFromScript`'s argument.** `32024ff0` gets the byte as its third push `@3205ad87-3205ad90`. March passes `m_bApplyMemo`
   (`:16221`). Settle what June's argument selects inside `32024ff0` and pass June's value. **The list choice itself stays checklist row 397**.
   Port only the argument and whatever the plain lists need to keep working.
3. **`DAMAGE_DATA_NEXT`.** March's frame move reads `DAMAGE_DATA_NEXT_MEMO` with `m_bApplyMemo` (`:43433-43442`). Find June's equivalent
   (`DAMAGE_DATA_NEXT_ULTIMATE` is plaintext at `3346f064`) and port its whole function.
4. **`APPLY_MEMO` / `DAMAGE_DATA_MEMO`.** Neither key is a plaintext string in June. Check the XOR-string table before concluding that June dropped them
   (`june2017_re/` XOR table). If June dropped them, retire the memo path under the flag, keeping March's in the `#else`. Hoist the flag to where June
   computes it (the top of CommonStateStart), and feed it to `ParsingBuffFactorID` too (S3e's inline block), so one value serves all three.

- **Gate:** one `SULT` line per state start that picks a table (debug flag, throttled like `SBUF`); flagcheck PASS; build + deploy. Play-test smoke:
  states run as before. A real `_ULTIMATE` pick needs a June-id class script, which arrives with the class ports (as S3e's item 2 did).
- Stop and ask only where copying June breaks March.
