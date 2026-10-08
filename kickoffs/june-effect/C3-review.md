# June effect completion - C3 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3 review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C3 done (its Done note and commits) |
| Flag | `SERV_IRUHADEV_JUNE_DAMAGEDATA` (extended) |
| Log tag | `[JUNE_FX]` |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3 - 8b, straightforward"
```

Also read: `june2017_re/funcmap.tsv` rows the phase cites, and its commits (`git log --oneline --grep="C3"`).

## Task

Review C3's commit(s) against the June binary and the plan's C3 section: each key's June parse site and June consumer, both cited; the binary-only keys' parse and consumer; DAMAGE_DATA_KEY_NAME and the mana no-display reader.

The plan's model rule is "transcription on Sonnet, then an Opus review"; this is that review.
Hygiene is one command: `python tools/flagcheck.py <FLAG> --range <first C3 commit>^..HEAD`
for each flag the phase used. Paste its `files:` / `in this flag's files:` / `RESULT:` lines, and
explain every OTHER row and deleted line; a FAIL or ERROR is a finding. Check `census.py` moved the
way the phase's Exit says.

Spot-check 2-3 sites directly in Ghidra (read-only); if one fails, re-check the whole phase.
Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design
decisions back to the user. Write "C3 - Review" under the phase in the plan, update the
STATUS.md row, and commit. Never launch X2_offline.exe; ask the user.

## Notes from the C3 session (2026-10-08) for the reviewer

- The review's own hygiene run needs the commit range; the Done note names the commit.
- C3 ported 19 keys' parse and these consumers; `funcmap.tsv` rows tagged `(C3)` hold every address. Spot-check at least these in Ghidra:
  `GetFinalDamage @3228676a-32286786` (SUPERAMOR gate, `vslot 0x264( 0 )` = IsSuperArmor) and `@3228682a-32286879` (PAUSE_NPC_AI_TIME, mask 0x05 / jp: continues for t > 0);
  the six `+0x50d` tests (`AreaHit @326248eb`, `MeshToUnit @32625852`, `EffectToUnit @3262b933`, ...);
  the NPC `DamageReact @327b5073-327b5171` (EXCEPT_ME_HP_UP gates only the HP heal and the heal effect; new `GetNowHp() > 0`).
- **Behaviour that is live in March now** (decide whether it is intended June parity): an effect hit by a user-owned effect sets the owner's
  future-frame `unitCondition.bHit` unless `IGNORE_ATTACK_SUCCESS` (`31fcee10` @31f85b1f); an invincible unit takes DT_BUFF hits that are
  ATTACK_OUR_TEAM; the NPC heal needs `GetNowHp() > 0`. The matching hit counter `31fcee40` was not ported (no March member).
- **Not ported, flagged as identification (phase C3b, Opus):** `31fd86b0` (armour-level check) and the `DamageReactStateChange` bounding case
  (FORCE_BACK_SPEED's second consumer, BOUNDING_BACK_SPEED_X/Y), the ADD_MEMBER_COUNT reader, `31fcee40`'s readers.
- `CEffect::SetScale` vs June's base multiply: the matrix and the funcmap row `32012b50 @32013d35` argue the default (1,1,1) is equivalent;
  confirm, or ask for the RT_CREATE_DAMAGE_EFFECT play-test with a scaled child.
