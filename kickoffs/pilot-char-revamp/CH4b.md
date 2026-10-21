# June character Aisha (Stage 5 pilot) - Phase CH4b - June's unit delegate lists and the 22416 passive

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase pilot-char-revamp CH4b` |
| Plan doc | `JUNE_CHARACTER_AISHA_PHASES.md` (branch `main`) |
| Depends on | CH4a review; not alongside CH4c / CH4d (same class files) |
| Flags | a new engine flag for the delegate lists (name it in the phase), and `SERV_IRUHADEV_JUNE_CHAR_ARME_PASSIVE` for the 22416 tail |
| Log tag | `[JUNE_CHAR]` (`ARME SKILL`) |
| Review | `kickoffs/pilot-char-revamp/CH4b-review.md` (the phase writes it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE_CHARACTER_AISHA_PHASES.md "Rules" "CH4 split (2026-10-22, Opus)" "CH4a - Done (2026-10-22, Opus 5.5)"
grep -a "^322752b0\|^31fe19d0\|^32ce9f90\|^32cecd70" june2017_re/funcmap.tsv
```

## Task

1. **The engine side, whole.** June's `CX2GameUnit` holds five vectors of `{ object, function }` pairs at `+0x66ac`, `+0x66bc`, `+0x66cc`, `+0x66dc`
   and `+0x66ec`. Map them in the listing:
   - who clears them (`31fe19d0` with `326df550( 0 )`; the ctor / dtor `322a0730` `@322a1545`.., `322a4c52`);
   - who runs them (`322752b0`, `32275280`) at which call site with which argument (11 sites: CEffect::AttackResult `31f85910`,
     CX2GUNPC::AttackResult `3276a320`, `32012b50`, both DamageReacts, `32624610`, `32063650`, `327b53b0`);
   - what each list means (attack result, damage react, ...).
   Port the lists, their clears and every run site under one new engine flag.
2. **Aisha:** the 22416 (Open Mind, Dimension Witch) tail of `32cecd70` (`@32ced5fc`-`32ced623`: `June_GetSkillLevel( 22416 ) > 0` -> push
   `{ this, 32cf2d70 }` on `+0x66ac`) and its callback `32ce9f90` (thunk `32cf2d70`, `edx` = the argument). Port the callback whole.
3. Other classes' registrations (the `+0x66ac` .. `+0x66ec` pushes in `32a`/`32b`/`32c` code) are not ported here: list them per class in the checklist
   (memory `hand-char-port-material-to-char-ports`).

Delegate the mechanical reads, builds and log reads to subagents. Edit CP949 files with `bytepatch.py` only.
