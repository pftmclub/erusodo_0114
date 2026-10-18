# June engine remainder - Phase W1f review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-engine W1f review` |
| Plan doc | `JUNE_ENGINE_REMAINDER_PHASES.md` (branch `main`) |
| Depends on | Phase W1f done (its Done note and commit). No play-test: no March map has a droppable monster road |
| Flag | `SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_SIGNATURE` |
| Log tag | `[JUNE_ENGINE] WORLD` (none emitted by W1f) |

## Read first

```sh
python tools/docsection.py JUNE_ENGINE_REMAINDER_PHASES.md "Rules" "Phase W1f - CanDown's June signature and its callers" "Phase W1f -- Done"
grep -a "(W1f)" june2017_re/funcmap.tsv
```

and the 19 `DIFFERS` rows of `june2017_re/w1f_candown_callers.tsv` (`awk -F'\t' '$11=="DIFFERS"'`).

## Task

Review Phase W1f's commit against June's `CKTDGLineMap::CanDown` `31c6c310` and its call sites. What this phase could get wrong:
- the signature: in index `[EBP+0x1c]` read only at the `bOnLine` head (`@31c6c320`) and the fallback (`@31c6f747`), out index `[EBP+0x20]` written only
  at `@31c6f86c` (and only when `touch`); March's 7-arg forwarder must pass its pointer twice and `false`;
- the `LT_MONSTER_ROAD` case `@31c6c491-31c6c4ae` (monster always; else `+0xa4` and `[EBP+0x30]` both set); the loader default (`@31c716ec-31c716f0`: absent = 0);
- the four ported sites: re-read the pushes of `31f71146` (FollowLine, `bOnLine` 0), `31f82653` (CreateDieDamageEffect), `3273c114` / `3273c599`
  (drop items, `bDroppable` 1) and confirm March's lines are the same calls (`X2DamageEffect.cpp` FollowLine / CreateDieDamageEffect,
  `X2DropItemManager.cpp` OnFrameMove);
- the push-reading script's 324 "March shape" rows: spot-check 5 (both-NULL and same-pointer, including a wrapper pair such as `31fd749d` / `31fd74d6`);
- the "no change (equivalent)" verdicts (pet `326f35ad`, Chung `32bfe8c3`, Aisha `32cd97ba`): March's out locals really are unread after the call;
- overload resolution: no March call silently binds to the 9-arg form (`cl /P` or a grep of 9-argument `CanDown(` calls = only W1f's four).

Hygiene is one command:
`python tools/flagcheck.py SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_SIGNATURE --range <W1f commit>^..<W1f commit>`
(`git log -S SERV_IRUHADEV_JUNE_LINEMAP_CANDOWN_SIGNATURE` names it). Paste its `files:` / `in this flag's files:` / `RESULT:` lines.
Explain every OTHER row and every deleted line; a FAIL or ERROR is a finding.

Fix clear transcription errors under the same flag (then `tools/build.py --deploy`); hand design decisions back to the user. Write
"Phase W1f -- Review" into the plan doc, update the W1f STATUS.md row, check `kickoffs/june-engine/W1g.md`, and commit.
