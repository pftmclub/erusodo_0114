# June effect completion - C3k - June stopped counting fly hits (m_FlyCount)

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-effect C3k` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` (branch `main`) |
| Depends on | C3d-3 review |
| Flag | new `SERV_IRUHADEV_JUNE_FLY_COUNT` |
| Log tag | `JUNE_FX C3k` |
| Review | `kickoffs/june-effect/C3k-review.md` (write it when done) |

## Read first

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C3d-3 - Review"
grep -n "(C3d-3 review)" june2017_re/funcmap.tsv
```

## Task

`m_FlyCount` is `CX2GameUnit +0xdf8` (June's `backSpeed.y / ( m_FlyCount - 1 )` sites: user `31feb2fb`, `31feb432`; NPC `32769b27`, `32769b7b`,
`32769bf6`, `32769c4e`). The C3d-3 review's operand sweep over all of June's `.text` found **no increment**. It found only zero stores: two
constructors (`31fab59f`, `322a59c1`), and two resets in the damage path (user `3206c7db`, NPC `327bca82`: vslot 0x180 true or
`IsSuperArmor`). The `327cee69..327cef3c` hits are a vector's begin/end on another class. So June counts no fly hits at all, and the
"> 2" lift-damping branches are dead. March still counts RT_UP / RT_FLY / RT_DRAG_UP in both DamageReacts (`X2GUUser.cpp:8195`,
`X2GUNPC.cpp:8920`), and resets on `bForceFly`.

1. Re-confirm the sweep, including `inc`/`add` through a computed pointer and any `+0xdf8` relative to a sub-object base. Identify
   `3206c7db` / `327bca82` (which function, and what vslot 0x180 is), and compare them with March's `bForceFly` reset.
2. Port June's behaviour (no increment; June's reset rule) under the new flag, March in `#else`. This changes air-combo feel
   (repeated launches no longer weaken), and that is June's behaviour (user rule: June wins).
3. Play-test: juggle a monster with repeated launchers; with the flag the 3rd+ launch is as high as the 1st.
