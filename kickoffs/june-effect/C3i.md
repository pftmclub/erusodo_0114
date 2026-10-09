# June effect completion - C3i - June's NOT_UP_STATE in the NPC reaction cases

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3i` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch: the current working branch) |
| Depends on | C3d-3 review (it reads the key into `CX2GUNPC::m_bJuneNotUpState` under `SERV_IRUHADEV_JUNE_BOUNDING_STATE` and tests it in the RT_BOUNDING case only) |
| Flag | new `SERV_IRUHADEV_JUNE_NOT_UP_STATE` (`#error` without `SERV_IRUHADEV_JUNE_BOUNDING_STATE`, which owns the member and the key read) |
| Log tag | `JUNE_FX C3i` |
| Review | `kickoffs/june-effect/C3i-review.md` (write it when done) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3d-3 - Review"
grep -n "(C3d-3 review)" june2017_re/funcmap.tsv
```

## Task

`NOT_UP_STATE` is an NPC INIT key (bool, default false; June `327a0fa0 @327a5d52` into `+0x8ac4`, proven by decoding the key buffer
`0x337aa208..213`). 36 June NPC scripts set it to TRUE, and those scripts ship today (ljconv, `a5f7298`), so March ignores live data.
June's `CX2GUNPC::DamageReactStateChange` (`32769060`) tests it in six reaction cases besides RT_BOUNDING:
`@32769b1a`, `@32769b72`, `@32769be9`, `@32769c41`, `@32769cad`, `@32769ced` (`cmp byte ptr [esi+0x8ac4], 0`).

1. For each site: name the reaction case (switch table of `32769060`), and say what the set flag skips. `@32769b72` (RT_UP, `m_UpDamage` at
   `+0x8690`) skips only the `nowSpeed.y` write and still changes state; `@32769b1a` sits on the super-armour branch. Do not assume the
   others follow either shape.
2. Port all six into March's `CX2GUNPC::DamageReactStateChange` under the new flag, original code in `#else`. Keep June's quirks verbatim
   (JUNE_BUGS row if one looks unintended).
3. Check the user side has no `NOT_UP_STATE` reader (the user `+0x8ac4` is a different member: `31fcadd0` row in funcmap).
4. Play-test: an NPC whose June script sets `NOT_UP_STATE = TRUE` (e.g. data037 `ELSION_DEFENDER`) hit by a launcher takes the up
   state without the upward speed, as June does (step 1 settles the exact look per case); an ordinary monster still launches.
