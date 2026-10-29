# One function per name - Phase C5 review

| | |
|---|---|
| Start | new conversation on **Opus 5.5, high** effort, then type `/x2-phase june-march-divergence C5 review` |
| Plan doc | `JUNE_MARCH_DIVERGENCE_PHASES.md` (branch `main`) |
| Depends on | C5 done |
| Flag | existing flags |
| Log tag | none |

## Read first

```sh
python tools/docsection.py JUNE_MARCH_DIVERGENCE_PHASES.md "Context" "Rules" "Starting context" "Phase C5 - Members, locals, constants, namespaces (Sonnet 5.5, high)"
```

and C5's Done note.

## Task

Review C5: member merges (same type and meaning), names against context, enum aliases, the census at 0, the smoke logs.

Hygiene is one command per flag, not a fresh script:
`python tools/flagcheck.py <FLAG> --range <first commit>^..<last commit>` (a `..HEAD` range can time out), plus the flag-off
equivalence the C0 tool reports. Paste the `files:` / `in this flag's files:` / `RESULT:` lines. Explain every OTHER row and every
deleted line; a FAIL or ERROR is a finding. Fix clear errors under the same flag (then `tools/build.py --deploy`); hand design
decisions to the user. Write the review section into the plan doc, update STATUS, check the next kickoff, commit.

## From C5 (2026-10-29)

- The map's `enum` rows were matched by number across enum families, so check the family, not only the value: C5 found and fixed the ten `UAI_*` user-action ids (turned into `BFI_*` / `BTI_*`)
  and `EDT_MANA_DAMAGE_WITH_NO_DISPLAY` (turned into `BFI_DEBUFF_SIDE_EFFECT_MEDICINE`). A scan of the other 200 rows by name stem found no more; read `DTT_END` for `DTT_PANDEMONIUM_FEAR` and
  `BTI_HYPER_MODE` for the hyper skill category (kept by the "March's name when names share a value" rule) and decide if the rule should bend there.
- `junecollapse.py --scope-locals` is new (function-scoped local collision check); review its `local_conflict` once. The 12 hand-named locals and the C5 follow-up maps `c5{v,w,x,y,z}_followup_map.tsv`
  are in the Done note. `s_lSkinKem433` / `SkinAnimKem433` (`KTDGXSkinAnim.cpp`) keep the error id in the name: say if that should be renamed.
- 10067 in `GetNPCFaceTextureKey` uses March's `NUI_EVENT_ADAMS_UI_SHOP` (June calls it `NUI_EDEL`); March's `NUI_EDEL` is 10072.
