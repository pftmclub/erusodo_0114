# June effect completion - C3k review - June stopped counting fly hits (m_FlyCount)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3k review` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3k done (its Done note and commit) and its play-test |
| Flag | `SERV_IRUHADEV_JUNE_FLY_COUNT` (+ `SERV_IRUHADEV_JUNE_FLY_COUNT_DEBUG`, turn it OFF here) |
| Log tag | `JUNE_FX C3k` (unbracketed: `logs.py --grep "JUNE_FX C3k"`) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3k - port: June stopped counting fly hits" "C3k - Done (2026-10-10, Opus 5.5 high)"
grep -n "(C3k)" june2017_re/funcmap.tsv
```

## Task

Review C3k's commit against the June binary:

1. The sweep. Re-run `june2017_re/dispsweep.py df8` and the sub-object offsets (`c58`, `c68`); confirm no `+0xdf8` write other than the
   four zero stores, and that `327cf18c` / `327ca2f0` belong to the other class (vector at `+0xdf4/+0xdf8/+0xdfc`).
   Spot-check in Ghidra (read-only) that neither DamageReact (user `320462a0`, NPC `327b1f30`) carries a `bForceFly` branch
   that writes the counter.
2. The port: both DamageReacts put the whole `bForceFly` reset + count block in `#else`; the two CommonFrameMove resets
   (`@3206c7db`, `@327bca82`) and the `> 2` damping reads are untouched (dead code in June too).
3. The play-test lines: fly count 0 on every line, nowSpeed.y = backSpeed.y on the 3rd and later launch of the same NPC.

Hygiene: `python tools/flagcheck.py JUNE_FLY_COUNT --range <C3k commit>^..HEAD --with JUNE_FLY_COUNT_DEBUG`.
Paste its `files:` / `in this flag's files:` / `RESULT:` lines into the review. Explain every OTHER row and every deleted line;
a FAIL or ERROR is a finding.

Turn `SERV_IRUHADEV_JUNE_FLY_COUNT_DEBUG` off (comment the `#define`, as the C9h-4g review did), then `tools/build.py --deploy`.
Fix clear transcription errors under the same flag; hand design decisions back to the user. Write "C3k - Review" into the plan doc,
update the STATUS.md rows, and commit with explicit pathspecs.
